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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;
void*f2;};void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[
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
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct
_tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
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
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
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
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
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
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
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
_tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();void*Cyc_Absyn_exn_typ();
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_star_typ(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;
int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;
int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;
int print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block(struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*
k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*
def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11
Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_region(
void*r,int must_be_unique);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};struct _tuple14 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn);void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*loc,
void*t);struct _tuple15{unsigned int f1;int f2;};struct _tuple15 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;union Cyc_CfFlowInfo_RelnOp rop;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{
int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;enum Cyc_CfFlowInfo_InitLevel
f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};
struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};
struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{int tag;struct Cyc_Absyn_Exp*
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple17 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _dyneither_ptr msg,struct
_dyneither_ptr ap){{void*_tmp682;(_tmp682=0,Cyc_Tcutil_terr(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp682,
sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons _tmp1;_push_handler(&
_tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))_tmp3=1;if(!_tmp3){{struct
_RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=Cyc_Tcenv_lookup_ordinary(
_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*_tmp9;struct Cyc_Absyn_Enumfield*
_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;struct Cyc_Absyn_Datatypedecl*
_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {struct Cyc_Tcenv_VarRes_struct*
_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(_tmp6->tag != 0)goto _LL3;else{
_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct _tmp685;struct Cyc_Absyn_Var_e_struct*
_tmp684;e->r=(void*)((_tmp684=_cycalloc(sizeof(*_tmp684)),((_tmp684[0]=((_tmp685.tag=
1,((_tmp685.f1=q,((_tmp685.f2=(void*)_tmp7,_tmp685)))))),_tmp684))));}goto _LL0;
_LL3: {struct Cyc_Tcenv_EnumRes_struct*_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp5;if(_tmp8->tag != 3)goto _LL5;else{_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{
struct Cyc_Absyn_Enum_e_struct _tmp688;struct Cyc_Absyn_Enum_e_struct*_tmp687;e->r=(
void*)((_tmp687=_cycalloc(sizeof(*_tmp687)),((_tmp687[0]=((_tmp688.tag=32,((
_tmp688.f1=q,((_tmp688.f2=(struct Cyc_Absyn_Enumdecl*)_tmp9,((_tmp688.f3=(struct
Cyc_Absyn_Enumfield*)_tmpA,_tmp688)))))))),_tmp687))));}goto _LL0;_LL5: {struct
Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5;
if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}_LL6:{
struct Cyc_Absyn_AnonEnum_e_struct _tmp68B;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp68A;e->r=(void*)((_tmp68A=_cycalloc(sizeof(*_tmp68A)),((_tmp68A[0]=((_tmp68B.tag=
33,((_tmp68B.f1=q,((_tmp68B.f2=(void*)_tmpC,((_tmp68B.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp68B)))))))),_tmp68A))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp68E;struct Cyc_Absyn_Datatype_e_struct*_tmp68D;e->r=(void*)((_tmp68D=
_cycalloc(sizeof(*_tmp68D)),((_tmp68D[0]=((_tmp68E.tag=31,((_tmp68E.f1=0,((
_tmp68E.f2=_tmpF,((_tmp68E.f3=_tmp10,_tmp68E)))))))),_tmp68D))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp692;void*_tmp691[1];struct Cyc_String_pa_struct
_tmp690;(_tmp690.tag=0,((_tmp690.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp691[0]=& _tmp690,Cyc_Tcutil_terr(e->loc,((
_tmp692="bad occurrence of type name %s",_tag_dyneither(_tmp692,sizeof(char),31))),
_tag_dyneither(_tmp691,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp695;struct Cyc_Absyn_Var_e_struct*_tmp694;e->r=(void*)((_tmp694=_cycalloc(
sizeof(*_tmp694)),((_tmp694[0]=((_tmp695.tag=1,((_tmp695.f1=q,((_tmp695.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp695)))))),_tmp694))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp698;struct Cyc_Absyn_Var_e_struct*_tmp697;e->r=(void*)((_tmp697=_cycalloc(
sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=1,((_tmp698.f1=q,((_tmp698.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp698)))))),_tmp697))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple18{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple18*_tmp699;return(_tmp699=_cycalloc(sizeof(*_tmp699)),((_tmp699->f1=
0,((_tmp699->f2=e,_tmp699)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp69C;struct
Cyc_Absyn_FnCall_e_struct*_tmp69B;e->r=(void*)((_tmp69B=_cycalloc(sizeof(*
_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=11,((_tmp69C.f1=e1,((_tmp69C.f2=es,((
_tmp69C.f3=0,_tmp69C)))))))),_tmp69B))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp6A0;void*_tmp69F[1];struct Cyc_String_pa_struct
_tmp69E;(_tmp69E.tag=0,((_tmp69E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp69F[0]=& _tmp69E,Cyc_Tcutil_terr(e->loc,((
_tmp6A0="%s is a constant, not a function",_tag_dyneither(_tmp6A0,sizeof(char),
33))),_tag_dyneither(_tmp69F,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp6A3;struct Cyc_Absyn_Datatype_e_struct*_tmp6A2;e->r=(void*)((_tmp6A2=
_cycalloc(sizeof(*_tmp6A2)),((_tmp6A2[0]=((_tmp6A3.tag=31,((_tmp6A3.f1=es,((
_tmp6A3.f2=_tmp30,((_tmp6A3.f3=_tmp31,_tmp6A3)))))))),_tmp6A2))));}_npop_handler(
0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*
_tmp3D=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp6A6;struct Cyc_Absyn_Aggregate_e_struct*_tmp6A5;e->r=(void*)((_tmp6A5=
_cycalloc(sizeof(*_tmp6A5)),((_tmp6A5[0]=((_tmp6A6.tag=29,((_tmp6A6.f1=_tmp33->name,((
_tmp6A6.f2=0,((_tmp6A6.f3=_tmp3D,((_tmp6A6.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp6A6)))))))))),_tmp6A5))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp6AA;void*_tmp6A9[1];
struct Cyc_String_pa_struct _tmp6A8;(_tmp6A8.tag=0,((_tmp6A8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp6A9[
0]=& _tmp6A8,Cyc_Tcutil_terr(e->loc,((_tmp6AA="%s is an enum constructor, not a function",
_tag_dyneither(_tmp6AA,sizeof(char),42))),_tag_dyneither(_tmp6A9,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp6AD;struct Cyc_Absyn_FnCall_e_struct*
_tmp6AC;e->r=(void*)((_tmp6AC=_cycalloc(sizeof(*_tmp6AC)),((_tmp6AC[0]=((_tmp6AD.tag=
11,((_tmp6AD.f1=e1,((_tmp6AD.f2=es,((_tmp6AD.f3=0,_tmp6AD)))))))),_tmp6AC))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp6B0;struct Cyc_Absyn_FnCall_e_struct*_tmp6AF;e->r=(void*)((_tmp6AF=_cycalloc(
sizeof(*_tmp6AF)),((_tmp6AF[0]=((_tmp6B0.tag=11,((_tmp6B0.f1=e1,((_tmp6B0.f2=es,((
_tmp6B0.f3=0,_tmp6B0)))))))),_tmp6AF))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp6B3;
struct Cyc_Absyn_Array_e_struct*_tmp6B2;e->r=(void*)((_tmp6B2=_cycalloc(sizeof(*
_tmp6B2)),((_tmp6B2[0]=((_tmp6B3.tag=27,((_tmp6B3.f1=des,_tmp6B3)))),_tmp6B2))));}
return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 11)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp6B6;void*_tmp6B5;(_tmp6B5=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6B6="struct type not properly set",
_tag_dyneither(_tmp6B6,sizeof(char),29))),_tag_dyneither(_tmp6B5,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp6B9;struct Cyc_Absyn_Aggregate_e_struct*_tmp6B8;e->r=(void*)((_tmp6B8=
_cycalloc(sizeof(*_tmp6B8)),((_tmp6B8[0]=((_tmp6B9.tag=29,((_tmp6B9.f1=_tmp58->name,((
_tmp6B9.f2=0,((_tmp6B9.f3=des,((_tmp6B9.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp6B9)))))))))),_tmp6B8))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 8)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp6BC;struct Cyc_Absyn_Array_e_struct*_tmp6BB;e->r=(
void*)((_tmp6BB=_cycalloc(sizeof(*_tmp6BB)),((_tmp6BB[0]=((_tmp6BC.tag=27,((
_tmp6BC.f1=des,_tmp6BC)))),_tmp6BB))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 12)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp6BF;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp6BE;e->r=(void*)((_tmp6BE=_cycalloc(sizeof(*_tmp6BE)),((_tmp6BE[0]=((_tmp6BF.tag=
30,((_tmp6BF.f1=(void*)t,((_tmp6BF.f2=des,_tmp6BF)))))),_tmp6BE))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp6C2;struct Cyc_Absyn_Array_e_struct*
_tmp6C1;e->r=(void*)((_tmp6C1=_cycalloc(sizeof(*_tmp6C1)),((_tmp6C1[0]=((_tmp6C2.tag=
27,((_tmp6C2.f1=des,_tmp6C2)))),_tmp6C1))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6C7;void*_tmp6C6[2];struct Cyc_String_pa_struct _tmp6C5;
struct Cyc_String_pa_struct _tmp6C4;(_tmp6C4.tag=0,((_tmp6C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp6C5.tag=0,((_tmp6C5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6C6[0]=& _tmp6C5,((_tmp6C6[1]=& _tmp6C4,Cyc_Tcutil_terr(
e->loc,((_tmp6C7="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6C7,sizeof(char),53))),_tag_dyneither(_tmp6C6,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6C8;struct
_tuple0 _tmp6C=(_tmp6C8.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp6C8.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp6C8)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 15)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 15)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp6CB;struct Cyc_Tcexp_TestEnv _tmp6CA;return(_tmp6CA.eq=((_tmp6CB=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6CB)),((_tmp6CB->f1=_tmp6F,((
_tmp6CB->f2=_tmp72,_tmp6CB)))))),((_tmp6CA.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp6CA)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp6CC;return(_tmp6CC.eq=0,((_tmp6CC.isTrue=0,_tmp6CC)));};}
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){return 1;}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst*
c,struct Cyc_Absyn_Exp*e){void*t;void*string_elt_typ=Cyc_Absyn_char_typ;int
string_numelts=0;{union Cyc_Absyn_Cnst _tmp76=*((union Cyc_Absyn_Cnst*)_check_null(
c));struct _tuple4 _tmp77;enum Cyc_Absyn_Sign _tmp78;struct _tuple4 _tmp79;enum Cyc_Absyn_Sign
_tmp7A;struct _tuple4 _tmp7B;enum Cyc_Absyn_Sign _tmp7C;struct _dyneither_ptr _tmp7D;
struct _tuple5 _tmp7E;enum Cyc_Absyn_Sign _tmp7F;struct _tuple7 _tmp80;enum Cyc_Absyn_Sign
_tmp81;struct _tuple8 _tmp82;int _tmp83;struct _tuple6 _tmp84;enum Cyc_Absyn_Sign
_tmp85;int _tmp86;struct _dyneither_ptr _tmp87;struct _dyneither_ptr _tmp88;int _tmp89;
_LL3E: if((_tmp76.Char_c).tag != 2)goto _LL40;_tmp77=(struct _tuple4)(_tmp76.Char_c).val;
_tmp78=_tmp77.f1;if(_tmp78 != Cyc_Absyn_Signed)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;
goto _LL3D;_LL40: if((_tmp76.Char_c).tag != 2)goto _LL42;_tmp79=(struct _tuple4)(
_tmp76.Char_c).val;_tmp7A=_tmp79.f1;if(_tmp7A != Cyc_Absyn_Unsigned)goto _LL42;
_LL41: t=Cyc_Absyn_uchar_typ;goto _LL3D;_LL42: if((_tmp76.Char_c).tag != 2)goto _LL44;
_tmp7B=(struct _tuple4)(_tmp76.Char_c).val;_tmp7C=_tmp7B.f1;if(_tmp7C != Cyc_Absyn_None)
goto _LL44;_LL43: t=Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp76.Wchar_c).tag != 3)
goto _LL46;_tmp7D=(struct _dyneither_ptr)(_tmp76.Wchar_c).val;_LL45: t=Cyc_Absyn_wchar_typ();
goto _LL3D;_LL46: if((_tmp76.Short_c).tag != 4)goto _LL48;_tmp7E=(struct _tuple5)(
_tmp76.Short_c).val;_tmp7F=_tmp7E.f1;_LL47: t=_tmp7F == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL3D;_LL48: if((_tmp76.LongLong_c).tag != 6)goto _LL4A;
_tmp80=(struct _tuple7)(_tmp76.LongLong_c).val;_tmp81=_tmp80.f1;_LL49: t=_tmp81 == 
Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL3D;
_LL4A: if((_tmp76.Float_c).tag != 7)goto _LL4C;_tmp82=(struct _tuple8)(_tmp76.Float_c).val;
_tmp83=_tmp82.f2;_LL4B: t=Cyc_Absyn_float_typ(_tmp83);goto _LL3D;_LL4C: if((_tmp76.Int_c).tag
!= 5)goto _LL4E;_tmp84=(struct _tuple6)(_tmp76.Int_c).val;_tmp85=_tmp84.f1;_tmp86=
_tmp84.f2;_LL4D: if(topt == 0)t=_tmp85 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
else{void*_tmp8A=Cyc_Tcutil_compress(*topt);enum Cyc_Absyn_Sign _tmp8C;enum Cyc_Absyn_Size_of
_tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of _tmp90;enum Cyc_Absyn_Sign
_tmp92;enum Cyc_Absyn_Size_of _tmp93;enum Cyc_Absyn_Sign _tmp95;enum Cyc_Absyn_Size_of
_tmp96;void*_tmp99;_LL55: {struct Cyc_Absyn_IntType_struct*_tmp8B=(struct Cyc_Absyn_IntType_struct*)
_tmp8A;if(_tmp8B->tag != 6)goto _LL57;else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8B->f2;if(
_tmp8D != Cyc_Absyn_Char_sz)goto _LL57;}}_LL56: switch(_tmp8C){case Cyc_Absyn_Unsigned:
_LL63: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL64: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL65: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp8C,(char)_tmp86);goto _LL54;_LL57: {struct Cyc_Absyn_IntType_struct*_tmp8E=(
struct Cyc_Absyn_IntType_struct*)_tmp8A;if(_tmp8E->tag != 6)goto _LL59;else{_tmp8F=
_tmp8E->f1;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Short_sz)goto _LL59;}}_LL58: t=
_tmp8F == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=Cyc_Absyn_Short_c(
_tmp8F,(short)_tmp86);goto _LL54;_LL59: {struct Cyc_Absyn_IntType_struct*_tmp91=(
struct Cyc_Absyn_IntType_struct*)_tmp8A;if(_tmp91->tag != 6)goto _LL5B;else{_tmp92=
_tmp91->f1;_tmp93=_tmp91->f2;if(_tmp93 != Cyc_Absyn_Int_sz)goto _LL5B;}}_LL5A: t=
_tmp92 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL54;
_LL5B: {struct Cyc_Absyn_IntType_struct*_tmp94=(struct Cyc_Absyn_IntType_struct*)
_tmp8A;if(_tmp94->tag != 6)goto _LL5D;else{_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;if(
_tmp96 != Cyc_Absyn_Long_sz)goto _LL5D;}}_LL5C: t=_tmp95 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL54;_LL5D:{struct Cyc_Absyn_PointerType_struct*_tmp97=(
struct Cyc_Absyn_PointerType_struct*)_tmp8A;if(_tmp97->tag != 5)goto _LL5F;}if(!(
_tmp86 == 0))goto _LL5F;_LL5E:{struct Cyc_Absyn_Const_e_struct _tmp6CF;struct Cyc_Absyn_Const_e_struct*
_tmp6CE;e->r=(void*)((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE[0]=((_tmp6CF.tag=
0,((_tmp6CF.f1=Cyc_Absyn_Null_c,_tmp6CF)))),_tmp6CE))));}{struct Cyc_List_List*
_tmp9C=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp6E2;
struct Cyc_Absyn_PtrAtts _tmp6E1;struct Cyc_Core_Opt*_tmp6E0;struct Cyc_Core_Opt*
_tmp6DF;struct Cyc_Absyn_PtrInfo _tmp6DE;struct Cyc_Absyn_PointerType_struct*
_tmp6DD;t=(void*)((_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6E2.tag=
5,((_tmp6E2.f1=((_tmp6DE.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp6DF=_cycalloc(sizeof(*_tmp6DF)),((_tmp6DF->v=_tmp9C,_tmp6DF))))),((_tmp6DE.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6DE.ptr_atts=((_tmp6E1.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((
_tmp6E0->v=_tmp9C,_tmp6E0))))),((_tmp6E1.nullable=Cyc_Absyn_true_conref,((
_tmp6E1.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6E1.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6E1.ptrloc=0,_tmp6E1)))))))))),_tmp6DE)))))),_tmp6E2)))),_tmp6DD))));}goto
_LL54;};_LL5F: {struct Cyc_Absyn_TagType_struct*_tmp98=(struct Cyc_Absyn_TagType_struct*)
_tmp8A;if(_tmp98->tag != 19)goto _LL61;else{_tmp99=(void*)_tmp98->f1;}}_LL60: {
struct Cyc_Absyn_ValueofType_struct _tmp6E5;struct Cyc_Absyn_ValueofType_struct*
_tmp6E4;struct Cyc_Absyn_ValueofType_struct*_tmpA3=(_tmp6E4=_cycalloc(sizeof(*
_tmp6E4)),((_tmp6E4[0]=((_tmp6E5.tag=18,((_tmp6E5.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp86,0),_tmp6E5)))),_tmp6E4)));if(!Cyc_Tcutil_unify(_tmp99,(void*)
_tmpA3)){{const char*_tmp6EA;void*_tmp6E9[2];struct Cyc_String_pa_struct _tmp6E8;
struct Cyc_String_pa_struct _tmp6E7;(_tmp6E7.tag=0,((_tmp6E7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA3)),((
_tmp6E8.tag=0,((_tmp6E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp99)),((_tmp6E9[0]=& _tmp6E8,((_tmp6E9[1]=& _tmp6E7,Cyc_Tcutil_terr(loc,((
_tmp6EA="expecting %s but found %s",_tag_dyneither(_tmp6EA,sizeof(char),26))),
_tag_dyneither(_tmp6E9,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp6ED;struct Cyc_Absyn_TagType_struct*_tmp6EC;t=(
void*)((_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC[0]=((_tmp6ED.tag=19,((
_tmp6ED.f1=(void*)((void*)_tmpA3),_tmp6ED)))),_tmp6EC))));}goto _LL54;}_LL61:;
_LL62: t=_tmp85 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL54;_LL54:;}goto _LL3D;_LL4E: if((_tmp76.String_c).tag != 8)goto _LL50;_tmp87=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4F: string_numelts=(int)
_get_dyneither_size(_tmp87,sizeof(char));_tmp88=_tmp87;goto _LL51;_LL50: if((
_tmp76.Wstring_c).tag != 9)goto _LL52;_tmp88=(struct _dyneither_ptr)(_tmp76.Wstring_c).val;
_LL51: if(string_numelts == 0){string_numelts=Cyc_Tcexp_wchar_numelts(_tmp88);
string_elt_typ=Cyc_Absyn_wchar_typ();}{struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);{struct Cyc_Core_Opt*
_tmp6EE;elen->topt=((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE->v=Cyc_Absyn_uint_typ,
_tmp6EE))));}{struct Cyc_Absyn_Upper_b_struct _tmp6F1;struct Cyc_Absyn_Upper_b_struct*
_tmp6F0;t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6F0=_cycalloc(sizeof(*_tmp6F0)),((_tmp6F0[0]=((_tmp6F1.tag=1,((
_tmp6F1.f1=elen,_tmp6F1)))),_tmp6F0)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpB1;struct Cyc_Absyn_Tqual
_tmpB2;_LL68: {struct Cyc_Absyn_ArrayType_struct*_tmpB0=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAF;if(_tmpB0->tag != 8)goto _LL6A;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB1.tq;}}
_LL69: return Cyc_Absyn_array_typ(string_elt_typ,_tmpB2,(struct Cyc_Absyn_Exp*)elen,((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL6A: {struct Cyc_Absyn_PointerType_struct*
_tmpB3=(struct Cyc_Absyn_PointerType_struct*)_tmpAF;if(_tmpB3->tag != 5)goto _LL6C;}
_LL6B: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{
struct Cyc_Core_Opt*_tmp6F2;e->topt=((_tmp6F2=_cycalloc(sizeof(*_tmp6F2)),((
_tmp6F2->v=t,_tmp6F2))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp6F5;struct Cyc_Absyn_Upper_b_struct*
_tmp6F4;t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp6F4=_cycalloc(sizeof(*_tmp6F4)),((_tmp6F4[0]=((_tmp6F5.tag=1,((
_tmp6F5.f1=elen,_tmp6F5)))),_tmp6F4)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp6F6;e->topt=((_tmp6F6=_cycalloc(sizeof(*_tmp6F6)),((_tmp6F6->v=t,_tmp6F6))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL67;_LL6C:;_LL6D: goto _LL67;_LL67:;}return t;};_LL52: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp89=(int)(_tmp76.Null_c).val;_LL53: {struct Cyc_List_List*_tmpB8=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp709;struct Cyc_Absyn_PtrAtts _tmp708;
struct Cyc_Core_Opt*_tmp707;struct Cyc_Core_Opt*_tmp706;struct Cyc_Absyn_PtrInfo
_tmp705;struct Cyc_Absyn_PointerType_struct*_tmp704;t=(void*)((_tmp704=_cycalloc(
sizeof(*_tmp704)),((_tmp704[0]=((_tmp709.tag=5,((_tmp709.f1=((_tmp705.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp706=_cycalloc(
sizeof(*_tmp706)),((_tmp706->v=_tmpB8,_tmp706))))),((_tmp705.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp705.ptr_atts=((_tmp708.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp707=_cycalloc(sizeof(*_tmp707)),((_tmp707->v=_tmpB8,_tmp707))))),((_tmp708.nullable=
Cyc_Absyn_true_conref,((_tmp708.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp708.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp708.ptrloc=0,_tmp708)))))))))),_tmp705)))))),_tmp709)))),_tmp704))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){
void*_tmpBF=b;struct Cyc_Absyn_Vardecl*_tmpC2;struct Cyc_Absyn_Fndecl*_tmpC4;
struct Cyc_Absyn_Vardecl*_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC8;struct Cyc_Absyn_Vardecl*
_tmpCA;_LL6F: {struct Cyc_Absyn_Unresolved_b_struct*_tmpC0=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBF;if(_tmpC0->tag != 0)goto _LL71;}_LL70: {const char*_tmp70D;void*_tmp70C[1];
struct Cyc_String_pa_struct _tmp70B;return(_tmp70B.tag=0,((_tmp70B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp70C[0]=&
_tmp70B,Cyc_Tcexp_expr_err(te,loc,0,((_tmp70D="undeclared identifier: %s",
_tag_dyneither(_tmp70D,sizeof(char),26))),_tag_dyneither(_tmp70C,sizeof(void*),1)))))));}
_LL71: {struct Cyc_Absyn_Global_b_struct*_tmpC1=(struct Cyc_Absyn_Global_b_struct*)
_tmpBF;if(_tmpC1->tag != 1)goto _LL73;else{_tmpC2=_tmpC1->f1;}}_LL72:*q=*_tmpC2->name;
return _tmpC2->type;_LL73: {struct Cyc_Absyn_Funname_b_struct*_tmpC3=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBF;if(_tmpC3->tag != 2)goto _LL75;else{_tmpC4=_tmpC3->f1;}}_LL74:*q=*_tmpC4->name;
return Cyc_Tcutil_fndecl2typ(_tmpC4);_LL75: {struct Cyc_Absyn_Pat_b_struct*_tmpC5=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBF;if(_tmpC5->tag != 5)goto _LL77;else{_tmpC6=
_tmpC5->f1;}}_LL76: _tmpC8=_tmpC6;goto _LL78;_LL77: {struct Cyc_Absyn_Local_b_struct*
_tmpC7=(struct Cyc_Absyn_Local_b_struct*)_tmpBF;if(_tmpC7->tag != 4)goto _LL79;
else{_tmpC8=_tmpC7->f1;}}_LL78: _tmpCA=_tmpC8;goto _LL7A;_LL79: {struct Cyc_Absyn_Param_b_struct*
_tmpC9=(struct Cyc_Absyn_Param_b_struct*)_tmpBF;if(_tmpC9->tag != 3)goto _LL6E;
else{_tmpCA=_tmpC9->f1;}}_LL7A:{union Cyc_Absyn_Nmspace _tmp70E;(*q).f1=(((_tmp70E.Loc_n).val=
0,(((_tmp70E.Loc_n).tag=4,_tmp70E))));}if(te->allow_valueof){void*_tmpCF=Cyc_Tcutil_compress(
_tmpCA->type);void*_tmpD1;_LL7C: {struct Cyc_Absyn_TagType_struct*_tmpD0=(struct
Cyc_Absyn_TagType_struct*)_tmpCF;if(_tmpD0->tag != 19)goto _LL7E;else{_tmpD1=(void*)
_tmpD0->f1;}}_LL7D:{struct Cyc_Absyn_Valueof_e_struct _tmp711;struct Cyc_Absyn_Valueof_e_struct*
_tmp710;e->r=(void*)((_tmp710=_cycalloc(sizeof(*_tmp710)),((_tmp710[0]=((_tmp711.tag=
39,((_tmp711.f1=(void*)_tmpD1,_tmp711)))),_tmp710))));}goto _LL7B;_LL7E:;_LL7F:
goto _LL7B;_LL7B:;}return _tmpCA->type;_LL6E:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int
arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpD4=fmt->r;
union Cyc_Absyn_Cnst _tmpD6;struct _dyneither_ptr _tmpD7;struct Cyc_Absyn_Exp*_tmpD9;
struct Cyc_Absyn_Exp _tmpDA;void*_tmpDB;union Cyc_Absyn_Cnst _tmpDD;struct
_dyneither_ptr _tmpDE;_LL81: {struct Cyc_Absyn_Const_e_struct*_tmpD5=(struct Cyc_Absyn_Const_e_struct*)
_tmpD4;if(_tmpD5->tag != 0)goto _LL83;else{_tmpD6=_tmpD5->f1;if((_tmpD6.String_c).tag
!= 8)goto _LL83;_tmpD7=(struct _dyneither_ptr)(_tmpD6.String_c).val;}}_LL82: _tmpDE=
_tmpD7;goto _LL84;_LL83: {struct Cyc_Absyn_Cast_e_struct*_tmpD8=(struct Cyc_Absyn_Cast_e_struct*)
_tmpD4;if(_tmpD8->tag != 15)goto _LL85;else{_tmpD9=_tmpD8->f2;_tmpDA=*_tmpD9;
_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Const_e_struct*_tmpDC=(struct Cyc_Absyn_Const_e_struct*)
_tmpDB;if(_tmpDC->tag != 0)goto _LL85;else{_tmpDD=_tmpDC->f1;if((_tmpDD.String_c).tag
!= 8)goto _LL85;_tmpDE=(struct _dyneither_ptr)(_tmpDD.String_c).val;}};}}_LL84:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDE,fmt->loc);goto _LL80;
_LL85:;_LL86: if(opt_args != 0){struct Cyc_List_List*_tmpDF=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDF->hd);{struct _RegionHandle*_tmpE0=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE0,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDF->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpE0,(struct Cyc_Absyn_Exp*)_tmpDF->hd)){const
char*_tmp714;void*_tmp713;(_tmp713=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDF->hd)->loc,((_tmp714="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp714,sizeof(char),49))),_tag_dyneither(_tmp713,sizeof(void*),0)));}};}}
return;_LL80:;}if(opt_args != 0){struct Cyc_List_List*_tmpE3=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE3 != 0;(((desc_types=desc_types->tl,
_tmpE3=_tmpE3->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE3->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp719;void*
_tmp718[2];struct Cyc_String_pa_struct _tmp717;struct Cyc_String_pa_struct _tmp716;(
_tmp716.tag=0,((_tmp716.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp717.tag=0,((_tmp717.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp718[
0]=& _tmp717,((_tmp718[1]=& _tmp716,Cyc_Tcutil_terr(e->loc,((_tmp719="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp719,sizeof(char),51))),_tag_dyneither(_tmp718,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp71A;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp71A=_cycalloc(sizeof(*_tmp71A)),((
_tmp71A->hd=(void*)arg_cnt,((_tmp71A->tl=*alias_arg_exps,_tmp71A))))));}{struct
_RegionHandle*_tmpE9=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE9,t) && !Cyc_Tcutil_is_noalias_path(_tmpE9,e)){const char*_tmp71D;void*
_tmp71C;(_tmp71C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE3->hd)->loc,((
_tmp71D="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp71D,sizeof(char),49))),_tag_dyneither(_tmp71C,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp720;void*_tmp71F;(_tmp71F=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp720="too few arguments",_tag_dyneither(_tmp720,sizeof(char),18))),
_tag_dyneither(_tmp71F,sizeof(void*),0)));}if(_tmpE3 != 0){const char*_tmp723;void*
_tmp722;(_tmp722=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE3->hd)->loc,((
_tmp723="too many arguments",_tag_dyneither(_tmp723,sizeof(char),19))),
_tag_dyneither(_tmp722,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL87: goto _LL88;case Cyc_Absyn_Minus:
_LL88: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp727;void*_tmp726[1];struct Cyc_String_pa_struct
_tmp725;(_tmp725.tag=0,((_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp726[0]=& _tmp725,Cyc_Tcutil_terr(loc,((_tmp727="expecting arithmetic type but found %s",
_tag_dyneither(_tmp727,sizeof(char),39))),_tag_dyneither(_tmp726,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL89: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp72B;void*_tmp72A[1];struct Cyc_String_pa_struct _tmp729;(_tmp729.tag=
0,((_tmp729.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp72A[0]=& _tmp729,Cyc_Tcutil_terr(loc,((_tmp72B="expecting integral or * type but found %s",
_tag_dyneither(_tmp72B,sizeof(char),42))),_tag_dyneither(_tmp72A,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL8A: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp72F;void*_tmp72E[1];struct Cyc_String_pa_struct _tmp72D;(_tmp72D.tag=
0,((_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp72E[0]=& _tmp72D,Cyc_Tcutil_terr(loc,((_tmp72F="expecting integral type but found %s",
_tag_dyneither(_tmp72F,sizeof(char),37))),_tag_dyneither(_tmp72E,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Numelts:
_LL8B:{void*_tmpF9=t;struct Cyc_Absyn_PtrInfo _tmpFC;struct Cyc_Absyn_PtrAtts _tmpFD;
union Cyc_Absyn_Constraint*_tmpFE;_LL8E: {struct Cyc_Absyn_ArrayType_struct*_tmpFA=(
struct Cyc_Absyn_ArrayType_struct*)_tmpF9;if(_tmpFA->tag != 8)goto _LL90;}_LL8F:
goto _LL8D;_LL90: {struct Cyc_Absyn_PointerType_struct*_tmpFB=(struct Cyc_Absyn_PointerType_struct*)
_tmpF9;if(_tmpFB->tag != 5)goto _LL92;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFC.ptr_atts;
_tmpFE=_tmpFD.bounds;}}_LL91:{void*_tmpFF=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFE);struct Cyc_Absyn_Exp*
_tmp102;_LL95: {struct Cyc_Absyn_DynEither_b_struct*_tmp100=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFF;if(_tmp100->tag != 0)goto _LL97;}_LL96: goto _LL94;_LL97: {struct Cyc_Absyn_Upper_b_struct*
_tmp101=(struct Cyc_Absyn_Upper_b_struct*)_tmpFF;if(_tmp101->tag != 1)goto _LL94;
else{_tmp102=_tmp101->f1;}}_LL98: if(!Cyc_Evexp_c_can_eval(_tmp102)){const char*
_tmp732;void*_tmp731;(_tmp731=0,Cyc_Tcutil_terr(loc,((_tmp732="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp732,sizeof(char),55))),_tag_dyneither(_tmp731,sizeof(void*),0)));}
goto _LL94;_LL94:;}goto _LL8D;_LL92:;_LL93: {const char*_tmp736;void*_tmp735[1];
struct Cyc_String_pa_struct _tmp734;(_tmp734.tag=0,((_tmp734.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp735[0]=&
_tmp734,Cyc_Tcutil_terr(loc,((_tmp736="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp736,sizeof(char),47))),_tag_dyneither(_tmp735,sizeof(void*),1)))))));}
_LL8D:;}return Cyc_Absyn_uint_typ;default: _LL8C: {const char*_tmp739;void*_tmp738;(
_tmp738=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp739="Non-unary primop",_tag_dyneither(_tmp739,sizeof(char),17))),
_tag_dyneither(_tmp738,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp73D;void*_tmp73C[
1];struct Cyc_String_pa_struct _tmp73B;(_tmp73B.tag=0,((_tmp73B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp73C[0]=& _tmp73B,Cyc_Tcutil_terr(e1->loc,((
_tmp73D="type %s cannot be used here",_tag_dyneither(_tmp73D,sizeof(char),28))),
_tag_dyneither(_tmp73C,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp741;void*_tmp740[1];struct Cyc_String_pa_struct
_tmp73F;(_tmp73F.tag=0,((_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp740[0]=& _tmp73F,Cyc_Tcutil_terr(e2->loc,((_tmp741="type %s cannot be used here",
_tag_dyneither(_tmp741,sizeof(char),28))),_tag_dyneither(_tmp740,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);};}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmp110=t1;struct Cyc_Absyn_PtrInfo _tmp112;void*
_tmp113;struct Cyc_Absyn_Tqual _tmp114;struct Cyc_Absyn_PtrAtts _tmp115;void*_tmp116;
union Cyc_Absyn_Constraint*_tmp117;union Cyc_Absyn_Constraint*_tmp118;union Cyc_Absyn_Constraint*
_tmp119;_LL9B: {struct Cyc_Absyn_PointerType_struct*_tmp111=(struct Cyc_Absyn_PointerType_struct*)
_tmp110;if(_tmp111->tag != 5)goto _LL9D;else{_tmp112=_tmp111->f1;_tmp113=_tmp112.elt_typ;
_tmp114=_tmp112.elt_tq;_tmp115=_tmp112.ptr_atts;_tmp116=_tmp115.rgn;_tmp117=
_tmp115.nullable;_tmp118=_tmp115.bounds;_tmp119=_tmp115.zero_term;}}_LL9C: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp113),& Cyc_Tcutil_tmk)){const char*
_tmp744;void*_tmp743;(_tmp743=0,Cyc_Tcutil_terr(e1->loc,((_tmp744="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp744,sizeof(char),50))),_tag_dyneither(_tmp743,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp747;void*_tmp746;(_tmp746=0,
Cyc_Tcutil_terr(e1->loc,((_tmp747="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp747,sizeof(char),54))),_tag_dyneither(_tmp746,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp74B;void*_tmp74A[1];struct
Cyc_String_pa_struct _tmp749;(_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp74A[0]=& _tmp749,Cyc_Tcutil_terr(
e2->loc,((_tmp74B="expecting int but found %s",_tag_dyneither(_tmp74B,sizeof(
char),27))),_tag_dyneither(_tmp74A,sizeof(void*),1)))))));}{void*_tmp121=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp118);struct Cyc_Absyn_Exp*_tmp124;_LLA0: {struct Cyc_Absyn_DynEither_b_struct*
_tmp122=(struct Cyc_Absyn_DynEither_b_struct*)_tmp121;if(_tmp122->tag != 0)goto
_LLA2;}_LLA1: return t1;_LLA2: {struct Cyc_Absyn_Upper_b_struct*_tmp123=(struct Cyc_Absyn_Upper_b_struct*)
_tmp121;if(_tmp123->tag != 1)goto _LL9F;else{_tmp124=_tmp123->f1;}}_LLA3: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp119)){const char*
_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_warn(e1->loc,((_tmp74E="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp74E,sizeof(char),70))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp758;struct Cyc_Absyn_PtrAtts _tmp757;struct
Cyc_Absyn_PtrInfo _tmp756;struct Cyc_Absyn_PointerType_struct*_tmp755;struct Cyc_Absyn_PointerType_struct*
_tmp127=(_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755[0]=((_tmp758.tag=5,((
_tmp758.f1=((_tmp756.elt_typ=_tmp113,((_tmp756.elt_tq=_tmp114,((_tmp756.ptr_atts=((
_tmp757.rgn=_tmp116,((_tmp757.nullable=Cyc_Absyn_true_conref,((_tmp757.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp757.zero_term=_tmp119,((_tmp757.ptrloc=0,
_tmp757)))))))))),_tmp756)))))),_tmp758)))),_tmp755)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp127,Cyc_Absyn_Other_coercion);return(void*)_tmp127;};_LL9F:;};
_LL9D:;_LL9E: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL9A:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp75D;void*_tmp75C[2];struct Cyc_String_pa_struct _tmp75B;struct Cyc_String_pa_struct
_tmp75A;(_tmp75A.tag=0,((_tmp75A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp75B.tag=0,((_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp75C[0]=& _tmp75B,((
_tmp75C[1]=& _tmp75A,Cyc_Tcutil_terr(e1->loc,((_tmp75D="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp75D,sizeof(char),59))),_tag_dyneither(_tmp75C,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp760;void*_tmp75F;(
_tmp75F=0,Cyc_Tcutil_terr(e1->loc,((_tmp760="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp760,sizeof(char),50))),_tag_dyneither(_tmp75F,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp763;void*_tmp762;(_tmp762=0,
Cyc_Tcutil_terr(e1->loc,((_tmp763="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp763,sizeof(char),54))),_tag_dyneither(_tmp762,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp768;void*_tmp767[2];struct
Cyc_String_pa_struct _tmp766;struct Cyc_String_pa_struct _tmp765;(_tmp765.tag=0,((
_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp766.tag=0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp767[0]=& _tmp766,((_tmp767[1]=& _tmp765,Cyc_Tcutil_terr(
e2->loc,((_tmp768="expecting either %s or int but found %s",_tag_dyneither(
_tmp768,sizeof(char),40))),_tag_dyneither(_tmp767,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(
e2);void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp769;struct _tuple0
_tmp139=(_tmp769.f1=Cyc_Tcutil_compress(t1),((_tmp769.f2=Cyc_Tcutil_compress(t2),
_tmp769)));void*_tmp13A;struct Cyc_Absyn_PtrInfo _tmp13C;void*_tmp13D;void*_tmp13E;
struct Cyc_Absyn_PtrInfo _tmp140;void*_tmp141;void*_tmp142;void*_tmp144;_LLA5:
_tmp13A=_tmp139.f1;{struct Cyc_Absyn_PointerType_struct*_tmp13B=(struct Cyc_Absyn_PointerType_struct*)
_tmp13A;if(_tmp13B->tag != 5)goto _LLA7;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13C.elt_typ;}};
_tmp13E=_tmp139.f2;{struct Cyc_Absyn_PointerType_struct*_tmp13F=(struct Cyc_Absyn_PointerType_struct*)
_tmp13E;if(_tmp13F->tag != 5)goto _LLA7;else{_tmp140=_tmp13F->f1;_tmp141=_tmp140.elt_typ;}};
_LLA6: if(Cyc_Tcutil_unify(_tmp13D,_tmp141))return Cyc_Absyn_sint_typ;goto _LLA4;
_LLA7: _tmp142=_tmp139.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp143=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp142;if(_tmp143->tag != 15)goto _LLA9;};_tmp144=
_tmp139.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp145=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp144;if(_tmp145->tag != 15)goto _LLA9;};_LLA8: return Cyc_Absyn_sint_typ;_LLA9:;
_LLAA: goto _LLA4;_LLA4:;}{const char*_tmp76E;void*_tmp76D[2];struct Cyc_String_pa_struct
_tmp76C;struct Cyc_String_pa_struct _tmp76B;(_tmp76B.tag=0,((_tmp76B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp76C.tag=
0,((_tmp76C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp76D[0]=& _tmp76C,((_tmp76D[1]=& _tmp76B,Cyc_Tcutil_terr(loc,((_tmp76E="comparison not allowed between %s and %s",
_tag_dyneither(_tmp76E,sizeof(char),41))),_tag_dyneither(_tmp76D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp76F;
struct _tuple0 _tmp14B=(_tmp76F.f1=t1,((_tmp76F.f2=t2,_tmp76F)));void*_tmp14C;void*
_tmp14E;void*_tmp14F;void*_tmp151;_LLAC: _tmp14C=_tmp14B.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 15)goto
_LLAE;else{_tmp14E=(void*)_tmp14D->f1;}};_tmp14F=_tmp14B.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp150=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14F;if(_tmp150->tag != 15)goto
_LLAE;else{_tmp151=(void*)_tmp150->f1;}};_LLAD: return Cyc_Absyn_sint_typ;_LLAE:;
_LLAF: goto _LLAB;_LLAB:;}{const char*_tmp774;void*_tmp773[2];struct Cyc_String_pa_struct
_tmp772;struct Cyc_String_pa_struct _tmp771;(_tmp771.tag=0,((_tmp771.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp772.tag=
0,((_tmp772.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp773[0]=& _tmp772,((_tmp773[1]=& _tmp771,Cyc_Tcutil_terr(loc,((_tmp774="comparison not allowed between %s and %s",
_tag_dyneither(_tmp774,sizeof(char),41))),_tag_dyneither(_tmp773,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));};}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){switch(p){case Cyc_Absyn_Plus: _LLB0: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLB1: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLB2: goto _LLB3;case
Cyc_Absyn_Div: _LLB3: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLB4: goto _LLB5;case Cyc_Absyn_Bitand: _LLB5: goto _LLB6;case Cyc_Absyn_Bitor:
_LLB6: goto _LLB7;case Cyc_Absyn_Bitxor: _LLB7: goto _LLB8;case Cyc_Absyn_Bitlshift:
_LLB8: goto _LLB9;case Cyc_Absyn_Bitlrshift: _LLB9: goto _LLBA;case Cyc_Absyn_Bitarshift:
_LLBA: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLBB: goto _LLBC;case Cyc_Absyn_Neq: _LLBC: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLBD: goto _LLBE;case Cyc_Absyn_Lt: _LLBE: goto _LLBF;case Cyc_Absyn_Gte:
_LLBF: goto _LLC0;case Cyc_Absyn_Lte: _LLC0: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLC1: {const char*_tmp777;void*_tmp776;(_tmp776=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp777="bad binary primop",
_tag_dyneither(_tmp777,sizeof(char),18))),_tag_dyneither(_tmp776,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLC3: {const char*_tmp77A;void*_tmp779;return(_tmp779=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp77A="primitive operator has 0 arguments",_tag_dyneither(_tmp77A,
sizeof(char),35))),_tag_dyneither(_tmp779,sizeof(void*),0)));}case 1: _LLC4: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC5: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC6: {const char*_tmp77D;void*_tmp77C;return(_tmp77C=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp77D="primitive operator has > 2 arguments",_tag_dyneither(_tmp77D,
sizeof(char),37))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}}return t;};}struct
_tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};static int Cyc_Tcexp_check_writable_aggr(
struct Cyc_Position_Segment*loc,void*t){void*_tmp15C=Cyc_Tcutil_compress(t);void*
_tmp15D=_tmp15C;struct Cyc_Absyn_AggrInfo _tmp15F;union Cyc_Absyn_AggrInfoU _tmp160;
struct Cyc_Absyn_Aggrdecl**_tmp161;struct Cyc_Absyn_Aggrdecl*_tmp162;struct Cyc_List_List*
_tmp164;struct Cyc_Absyn_DatatypeFieldInfo _tmp166;union Cyc_Absyn_DatatypeFieldInfoU
_tmp167;struct _tuple2 _tmp168;struct Cyc_Absyn_Datatypefield*_tmp169;struct Cyc_Absyn_ArrayInfo
_tmp16B;void*_tmp16C;struct Cyc_Absyn_Tqual _tmp16D;struct Cyc_List_List*_tmp16F;
_LLC9: {struct Cyc_Absyn_AggrType_struct*_tmp15E=(struct Cyc_Absyn_AggrType_struct*)
_tmp15D;if(_tmp15E->tag != 11)goto _LLCB;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.aggr_info;
if((_tmp160.KnownAggr).tag != 2)goto _LLCB;_tmp161=(struct Cyc_Absyn_Aggrdecl**)(
_tmp160.KnownAggr).val;_tmp162=*_tmp161;}}_LLCA: _tmp164=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp162->impl))->fields;goto _LLCC;_LLCB: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp163=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp15D;if(_tmp163->tag != 12)goto
_LLCD;else{_tmp164=_tmp163->f2;}}_LLCC: for(0;_tmp164 != 0;_tmp164=_tmp164->tl){
struct Cyc_Absyn_Aggrfield*_tmp170=(struct Cyc_Absyn_Aggrfield*)_tmp164->hd;if((
_tmp170->tq).real_const){{const char*_tmp781;void*_tmp780[1];struct Cyc_String_pa_struct
_tmp77F;(_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp170->name),((_tmp780[0]=& _tmp77F,Cyc_Tcutil_terr(loc,((_tmp781="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp781,sizeof(char),56))),_tag_dyneither(_tmp780,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp170->type))return 0;}return 1;
_LLCD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15D;if(_tmp165->tag != 4)goto _LLCF;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.field_info;
if((_tmp167.KnownDatatypefield).tag != 2)goto _LLCF;_tmp168=(struct _tuple2)(
_tmp167.KnownDatatypefield).val;_tmp169=_tmp168.f2;}}_LLCE:{struct Cyc_List_List*
_tmp174=_tmp169->typs;for(0;_tmp174 != 0;_tmp174=_tmp174->tl){struct _tuple19
_tmp176;struct Cyc_Absyn_Tqual _tmp177;void*_tmp178;struct _tuple19*_tmp175=(struct
_tuple19*)_tmp174->hd;_tmp176=*_tmp175;_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;if(
_tmp177.real_const){{const char*_tmp785;void*_tmp784[1];struct Cyc_String_pa_struct
_tmp783;(_tmp783.tag=0,((_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp169->name)),((_tmp784[0]=& _tmp783,Cyc_Tcutil_terr(
loc,((_tmp785="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp785,sizeof(char),64))),_tag_dyneither(_tmp784,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp178))return 0;}}return 1;_LLCF: {
struct Cyc_Absyn_ArrayType_struct*_tmp16A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15D;if(_tmp16A->tag != 8)goto _LLD1;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.elt_type;
_tmp16D=_tmp16B.tq;}}_LLD0: if(_tmp16D.real_const){{const char*_tmp788;void*
_tmp787;(_tmp787=0,Cyc_Tcutil_terr(loc,((_tmp788="attempt to over-write a const array",
_tag_dyneither(_tmp788,sizeof(char),36))),_tag_dyneither(_tmp787,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp16C);_LLD1: {struct Cyc_Absyn_TupleType_struct*
_tmp16E=(struct Cyc_Absyn_TupleType_struct*)_tmp15D;if(_tmp16E->tag != 10)goto
_LLD3;else{_tmp16F=_tmp16E->f1;}}_LLD2: for(0;_tmp16F != 0;_tmp16F=_tmp16F->tl){
struct _tuple19 _tmp17F;struct Cyc_Absyn_Tqual _tmp180;void*_tmp181;struct _tuple19*
_tmp17E=(struct _tuple19*)_tmp16F->hd;_tmp17F=*_tmp17E;_tmp180=_tmp17F.f1;_tmp181=
_tmp17F.f2;if(_tmp180.real_const){{const char*_tmp78B;void*_tmp78A;(_tmp78A=0,Cyc_Tcutil_terr(
loc,((_tmp78B="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp78B,sizeof(char),56))),_tag_dyneither(_tmp78A,sizeof(void*),0)));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp181))return 0;}return 1;_LLD3:;
_LLD4: return 1;_LLC8:;}static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v))return;{void*_tmp184=e->r;void*_tmp186;
struct Cyc_Absyn_Vardecl*_tmp188;void*_tmp18A;struct Cyc_Absyn_Vardecl*_tmp18C;
void*_tmp18E;struct Cyc_Absyn_Vardecl*_tmp190;void*_tmp192;struct Cyc_Absyn_Vardecl*
_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*
_tmp199;struct _dyneither_ptr*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct
_dyneither_ptr*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A1;
struct Cyc_Absyn_Exp*_tmp1A3;_LLD6: {struct Cyc_Absyn_Var_e_struct*_tmp185=(struct
Cyc_Absyn_Var_e_struct*)_tmp184;if(_tmp185->tag != 1)goto _LLD8;else{_tmp186=(void*)
_tmp185->f2;{struct Cyc_Absyn_Param_b_struct*_tmp187=(struct Cyc_Absyn_Param_b_struct*)
_tmp186;if(_tmp187->tag != 3)goto _LLD8;else{_tmp188=_tmp187->f1;}};}}_LLD7:
_tmp18C=_tmp188;goto _LLD9;_LLD8: {struct Cyc_Absyn_Var_e_struct*_tmp189=(struct
Cyc_Absyn_Var_e_struct*)_tmp184;if(_tmp189->tag != 1)goto _LLDA;else{_tmp18A=(void*)
_tmp189->f2;{struct Cyc_Absyn_Local_b_struct*_tmp18B=(struct Cyc_Absyn_Local_b_struct*)
_tmp18A;if(_tmp18B->tag != 4)goto _LLDA;else{_tmp18C=_tmp18B->f1;}};}}_LLD9:
_tmp190=_tmp18C;goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_struct*_tmp18D=(struct
Cyc_Absyn_Var_e_struct*)_tmp184;if(_tmp18D->tag != 1)goto _LLDC;else{_tmp18E=(void*)
_tmp18D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp18F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp18E;if(_tmp18F->tag != 5)goto _LLDC;else{_tmp190=_tmp18F->f1;}};}}_LLDB:
_tmp194=_tmp190;goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_struct*_tmp191=(struct
Cyc_Absyn_Var_e_struct*)_tmp184;if(_tmp191->tag != 1)goto _LLDE;else{_tmp192=(void*)
_tmp191->f2;{struct Cyc_Absyn_Global_b_struct*_tmp193=(struct Cyc_Absyn_Global_b_struct*)
_tmp192;if(_tmp193->tag != 1)goto _LLDE;else{_tmp194=_tmp193->f1;}};}}_LLDD: if(!(
_tmp194->tq).real_const)return;goto _LLD5;_LLDE: {struct Cyc_Absyn_Subscript_e_struct*
_tmp195=(struct Cyc_Absyn_Subscript_e_struct*)_tmp184;if(_tmp195->tag != 24)goto
_LLE0;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LLDF:{void*_tmp1A4=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp196->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1A6;struct Cyc_Absyn_Tqual _tmp1A7;struct Cyc_Absyn_ArrayInfo _tmp1A9;struct Cyc_Absyn_Tqual
_tmp1AA;struct Cyc_List_List*_tmp1AC;_LLED: {struct Cyc_Absyn_PointerType_struct*
_tmp1A5=(struct Cyc_Absyn_PointerType_struct*)_tmp1A4;if(_tmp1A5->tag != 5)goto
_LLEF;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.elt_tq;}}_LLEE: _tmp1AA=_tmp1A7;
goto _LLF0;_LLEF: {struct Cyc_Absyn_ArrayType_struct*_tmp1A8=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1A4;if(_tmp1A8->tag != 8)goto _LLF1;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.tq;}}
_LLF0: if(!_tmp1AA.real_const)return;goto _LLEC;_LLF1: {struct Cyc_Absyn_TupleType_struct*
_tmp1AB=(struct Cyc_Absyn_TupleType_struct*)_tmp1A4;if(_tmp1AB->tag != 10)goto
_LLF3;else{_tmp1AC=_tmp1AB->f1;}}_LLF2: {unsigned int _tmp1AE;int _tmp1AF;struct
_tuple15 _tmp1AD=Cyc_Evexp_eval_const_uint_exp(_tmp197);_tmp1AE=_tmp1AD.f1;
_tmp1AF=_tmp1AD.f2;if(!_tmp1AF){{const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(
e->loc,((_tmp78E="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp78E,sizeof(char),47))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}return;}{
struct _handler_cons _tmp1B2;_push_handler(& _tmp1B2);{int _tmp1B4=0;if(setjmp(
_tmp1B2.handler))_tmp1B4=1;if(!_tmp1B4){{struct _tuple19 _tmp1B6;struct Cyc_Absyn_Tqual
_tmp1B7;struct _tuple19*_tmp1B5=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp1AC,(int)_tmp1AE);_tmp1B6=*_tmp1B5;_tmp1B7=_tmp1B6.f1;if(!
_tmp1B7.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp1B3=(
void*)_exn_thrown;void*_tmp1B9=_tmp1B3;_LLF6: {struct Cyc_List_Nth_struct*_tmp1BA=(
struct Cyc_List_Nth_struct*)_tmp1B9;if(_tmp1BA->tag != Cyc_List_Nth)goto _LLF8;}
_LLF7: return;_LLF8:;_LLF9:(void)_throw(_tmp1B9);_LLF5:;}};}goto _LLEC;}_LLF3:;
_LLF4: goto _LLEC;_LLEC:;}goto _LLD5;_LLE0: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp198=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp184;if(_tmp198->tag != 22)goto
_LLE2;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LLE1:{void*_tmp1BB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp199->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp1BD;union Cyc_Absyn_AggrInfoU _tmp1BE;struct Cyc_Absyn_Aggrdecl**_tmp1BF;struct
Cyc_List_List*_tmp1C1;_LLFB: {struct Cyc_Absyn_AggrType_struct*_tmp1BC=(struct Cyc_Absyn_AggrType_struct*)
_tmp1BB;if(_tmp1BC->tag != 11)goto _LLFD;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BD.aggr_info;
if((_tmp1BE.KnownAggr).tag != 2)goto _LLFD;_tmp1BF=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1BE.KnownAggr).val;}}_LLFC: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp1BF == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1BF,_tmp19A);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFA;}_LLFD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1C0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BB;if(_tmp1C0->tag != 12)goto _LLFF;else{_tmp1C1=_tmp1C0->f2;}}_LLFE: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1C1,_tmp19A);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFA;}_LLFF:;_LL100: goto _LLFA;_LLFA:;}goto _LLD5;_LLE2: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp19B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp184;if(_tmp19B->tag != 23)goto
_LLE4;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LLE3:{void*_tmp1C2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp19C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1C4;void*_tmp1C5;_LL102: {struct Cyc_Absyn_PointerType_struct*_tmp1C3=(struct
Cyc_Absyn_PointerType_struct*)_tmp1C2;if(_tmp1C3->tag != 5)goto _LL104;else{
_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C4.elt_typ;}}_LL103:{void*_tmp1C6=Cyc_Tcutil_compress(
_tmp1C5);struct Cyc_Absyn_AggrInfo _tmp1C8;union Cyc_Absyn_AggrInfoU _tmp1C9;struct
Cyc_Absyn_Aggrdecl**_tmp1CA;struct Cyc_List_List*_tmp1CC;_LL107: {struct Cyc_Absyn_AggrType_struct*
_tmp1C7=(struct Cyc_Absyn_AggrType_struct*)_tmp1C6;if(_tmp1C7->tag != 11)goto
_LL109;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C8.aggr_info;if((_tmp1C9.KnownAggr).tag
!= 2)goto _LL109;_tmp1CA=(struct Cyc_Absyn_Aggrdecl**)(_tmp1C9.KnownAggr).val;}}
_LL108: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp1CA == 0?0:
Cyc_Absyn_lookup_decl_field(*_tmp1CA,_tmp19D);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL106;}_LL109: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1CB=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp1C6;if(_tmp1CB->tag != 12)goto _LL10B;else{
_tmp1CC=_tmp1CB->f2;}}_LL10A: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp1CC,_tmp19D);if(sf == 0  || !(sf->tq).real_const)return;goto _LL106;}_LL10B:;
_LL10C: goto _LL106;_LL106:;}goto _LL101;_LL104:;_LL105: goto _LL101;_LL101:;}goto
_LLD5;_LLE4: {struct Cyc_Absyn_Deref_e_struct*_tmp19E=(struct Cyc_Absyn_Deref_e_struct*)
_tmp184;if(_tmp19E->tag != 21)goto _LLE6;else{_tmp19F=_tmp19E->f1;}}_LLE5:{void*
_tmp1CD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp19F->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1CF;struct Cyc_Absyn_Tqual _tmp1D0;struct Cyc_Absyn_ArrayInfo
_tmp1D2;struct Cyc_Absyn_Tqual _tmp1D3;_LL10E: {struct Cyc_Absyn_PointerType_struct*
_tmp1CE=(struct Cyc_Absyn_PointerType_struct*)_tmp1CD;if(_tmp1CE->tag != 5)goto
_LL110;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CF.elt_tq;}}_LL10F: _tmp1D3=_tmp1D0;
goto _LL111;_LL110: {struct Cyc_Absyn_ArrayType_struct*_tmp1D1=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1CD;if(_tmp1D1->tag != 8)goto _LL112;else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D2.tq;}}
_LL111: if(!_tmp1D3.real_const)return;goto _LL10D;_LL112:;_LL113: goto _LL10D;_LL10D:;}
goto _LLD5;_LLE6: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp1A0=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp184;if(_tmp1A0->tag != 13)goto _LLE8;else{_tmp1A1=_tmp1A0->f1;}}_LLE7: _tmp1A3=
_tmp1A1;goto _LLE9;_LLE8: {struct Cyc_Absyn_Instantiate_e_struct*_tmp1A2=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmp184;if(_tmp1A2->tag != 14)goto _LLEA;else{
_tmp1A3=_tmp1A2->f1;}}_LLE9: Cyc_Tcexp_check_writable(te,_tmp1A3);return;_LLEA:;
_LLEB: goto _LLD5;_LLD5:;}{const char*_tmp792;void*_tmp791[1];struct Cyc_String_pa_struct
_tmp790;(_tmp790.tag=0,((_tmp790.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp791[0]=& _tmp790,Cyc_Tcutil_terr(e->loc,((
_tmp792="attempt to write a const location: %s",_tag_dyneither(_tmp792,sizeof(
char),38))),_tag_dyneither(_tmp791,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){struct _RegionHandle*_tmp1D7=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(_tmp1D7,te),0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp795;void*_tmp794;return(_tmp794=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp795="increment/decrement of non-lvalue",_tag_dyneither(_tmp795,
sizeof(char),34))),_tag_dyneither(_tmp794,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp798;void*_tmp797;(_tmp797=0,Cyc_Tcutil_terr(e->loc,((
_tmp798="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp798,sizeof(char),50))),_tag_dyneither(_tmp797,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp79B;void*_tmp79A;(_tmp79A=0,Cyc_Tcutil_terr(e->loc,((_tmp79B="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp79B,sizeof(char),54))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}}
else{const char*_tmp79F;void*_tmp79E[1];struct Cyc_String_pa_struct _tmp79D;(
_tmp79D.tag=0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp79E[0]=& _tmp79D,Cyc_Tcutil_terr(e->loc,((_tmp79F="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp79F,sizeof(char),44))),_tag_dyneither(_tmp79E,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1E3;int _tmp1E4;const char*_tmp7A0;struct Cyc_Tcexp_TestEnv
_tmp1E2=Cyc_Tcexp_tcTest(te,e1,((_tmp7A0="conditional expression",_tag_dyneither(
_tmp7A0,sizeof(char),23))));_tmp1E3=_tmp1E2.eq;_tmp1E4=_tmp1E2.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp7A1;
struct Cyc_List_List _tmp1E5=(_tmp7A1.hd=e3,((_tmp7A1.tl=0,_tmp7A1)));struct Cyc_List_List
_tmp7A2;struct Cyc_List_List _tmp1E6=(_tmp7A2.hd=e2,((_tmp7A2.tl=(struct Cyc_List_List*)&
_tmp1E5,_tmp7A2)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1E6)){{const char*_tmp7A7;void*_tmp7A6[2];struct Cyc_String_pa_struct _tmp7A5;
struct Cyc_String_pa_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp7A6[0]=& _tmp7A5,((_tmp7A6[1]=& _tmp7A4,Cyc_Tcutil_terr(
loc,((_tmp7A7="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp7A7,sizeof(char),48))),_tag_dyneither(_tmp7A6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp7A8;Cyc_Tcexp_tcTest(te,e1,((_tmp7A8="logical-and expression",
_tag_dyneither(_tmp7A8,sizeof(char),23))));}{const char*_tmp7A9;Cyc_Tcexp_tcTest(
te,e2,((_tmp7A9="logical-and expression",_tag_dyneither(_tmp7A9,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp7AA;Cyc_Tcexp_tcTest(te,e1,((_tmp7AA="logical-or expression",
_tag_dyneither(_tmp7AA,sizeof(char),22))));}{const char*_tmp7AB;Cyc_Tcexp_tcTest(
te,e2,((_tmp7AB="logical-or expression",_tag_dyneither(_tmp7AB,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1F1=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(_tmp1F1,Cyc_Tcenv_enter_notreadctxt(
_tmp1F1,te)),0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp1F2=Cyc_Tcutil_compress(t1);_LL115: {struct Cyc_Absyn_ArrayType_struct*
_tmp1F3=(struct Cyc_Absyn_ArrayType_struct*)_tmp1F2;if(_tmp1F3->tag != 8)goto
_LL117;}_LL116:{const char*_tmp7AE;void*_tmp7AD;(_tmp7AD=0,Cyc_Tcutil_terr(loc,((
_tmp7AE="cannot assign to an array",_tag_dyneither(_tmp7AE,sizeof(char),26))),
_tag_dyneither(_tmp7AD,sizeof(void*),0)));}goto _LL114;_LL117:;_LL118: goto _LL114;
_LL114:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){const
char*_tmp7B1;void*_tmp7B0;(_tmp7B0=0,Cyc_Tcutil_terr(loc,((_tmp7B1="type is abstract (can't determine size).",
_tag_dyneither(_tmp7B1,sizeof(char),41))),_tag_dyneither(_tmp7B0,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7B4;void*_tmp7B3;return(_tmp7B3=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7B4="assignment to non-lvalue",_tag_dyneither(_tmp7B4,sizeof(
char),25))),_tag_dyneither(_tmp7B3,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp1FA=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1FA,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1FA,e2)){const char*_tmp7B7;void*
_tmp7B6;(_tmp7B6=0,Cyc_Tcutil_terr(e2->loc,((_tmp7B7="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7B7,sizeof(char),49))),_tag_dyneither(_tmp7B6,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp7BC;void*_tmp7BB[2];struct
Cyc_String_pa_struct _tmp7BA;struct Cyc_String_pa_struct _tmp7B9;void*_tmp1FD=(
_tmp7B9.tag=0,((_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7BA.tag=0,((_tmp7BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7BB[0]=& _tmp7BA,((_tmp7BB[1]=& _tmp7B9,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7BC="type mismatch: %s != %s",_tag_dyneither(_tmp7BC,sizeof(
char),24))),_tag_dyneither(_tmp7BB,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1FD;}}else{enum Cyc_Absyn_Primop
_tmp202=(enum Cyc_Absyn_Primop)po->v;void*_tmp203=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp202,e1,e2);if(!(Cyc_Tcutil_unify(_tmp203,t1) || Cyc_Tcutil_coerceable(
_tmp203) && Cyc_Tcutil_coerceable(t1))){const char*_tmp7C1;void*_tmp7C0[2];struct
Cyc_String_pa_struct _tmp7BF;struct Cyc_String_pa_struct _tmp7BE;void*_tmp204=(
_tmp7BE.tag=0,((_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7BF.tag=0,((_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7C0[0]=& _tmp7BF,((_tmp7C0[1]=& _tmp7BE,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7C1="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp7C1,sizeof(char),82))),_tag_dyneither(_tmp7C0,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp203,t1);Cyc_Tcutil_explain_failure();return _tmp204;}return
_tmp203;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){struct Cyc_List_List*fields;void*
t1=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp209=Cyc_Tcutil_compress(
t1);int _tmp20B;enum Cyc_Absyn_Size_of _tmp20D;enum Cyc_Absyn_Size_of _tmp20F;
_LL11A: {struct Cyc_Absyn_FloatType_struct*_tmp20A=(struct Cyc_Absyn_FloatType_struct*)
_tmp209;if(_tmp20A->tag != 7)goto _LL11C;else{_tmp20B=_tmp20A->f1;if(_tmp20B != 0)
goto _LL11C;}}_LL11B: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_float_typ(1);goto _LL119;_LL11C: {struct Cyc_Absyn_IntType_struct*
_tmp20C=(struct Cyc_Absyn_IntType_struct*)_tmp209;if(_tmp20C->tag != 6)goto _LL11E;
else{_tmp20D=_tmp20C->f2;if(_tmp20D != Cyc_Absyn_Char_sz)goto _LL11E;}}_LL11D: goto
_LL11F;_LL11E: {struct Cyc_Absyn_IntType_struct*_tmp20E=(struct Cyc_Absyn_IntType_struct*)
_tmp209;if(_tmp20E->tag != 6)goto _LL120;else{_tmp20F=_tmp20E->f2;if(_tmp20F != Cyc_Absyn_Short_sz)
goto _LL120;}}_LL11F: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL119;_LL120:;_LL121: goto _LL119;_LL119:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp211;struct Cyc_List_List*
_tmp212;struct Cyc_Position_Segment*_tmp213;enum Cyc_Absyn_Scope _tmp214;struct Cyc_Absyn_Datatypefield
_tmp210=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp211=_tmp210.name;
_tmp212=_tmp210.typs;_tmp213=_tmp210.loc;_tmp214=_tmp210.sc;if(_tmp212 == 0  || 
_tmp212->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple19*)_tmp212->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp216;struct Cyc_List_List*_tmp217;struct Cyc_Position_Segment*
_tmp218;enum Cyc_Absyn_Scope _tmp219;struct Cyc_Absyn_Datatypefield _tmp215=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp216=_tmp215.name;_tmp217=_tmp215.typs;
_tmp218=_tmp215.loc;_tmp219=_tmp215.sc;if(_tmp217 == 0  || _tmp217->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp217->hd)).f2);
int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp7C6;void*_tmp7C5[2];
struct Cyc_String_pa_struct _tmp7C4;struct Cyc_String_pa_struct _tmp7C3;(_tmp7C3.tag=
0,((_tmp7C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7C4.tag=0,((_tmp7C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp7C5[0]=& _tmp7C4,((_tmp7C5[1]=& _tmp7C3,Cyc_Tcutil_terr(
e->loc,((_tmp7C6="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7C6,sizeof(char),54))),_tag_dyneither(_tmp7C5,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp21E=args;int _tmp21F=0;struct _RegionHandle*_tmp220=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp221=Cyc_Tcenv_new_block(_tmp220,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp221,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp222=t;struct Cyc_Absyn_PtrInfo _tmp224;void*
_tmp225;struct Cyc_Absyn_Tqual _tmp226;struct Cyc_Absyn_PtrAtts _tmp227;void*_tmp228;
union Cyc_Absyn_Constraint*_tmp229;union Cyc_Absyn_Constraint*_tmp22A;union Cyc_Absyn_Constraint*
_tmp22B;_LL123: {struct Cyc_Absyn_PointerType_struct*_tmp223=(struct Cyc_Absyn_PointerType_struct*)
_tmp222;if(_tmp223->tag != 5)goto _LL125;else{_tmp224=_tmp223->f1;_tmp225=_tmp224.elt_typ;
_tmp226=_tmp224.elt_tq;_tmp227=_tmp224.ptr_atts;_tmp228=_tmp227.rgn;_tmp229=
_tmp227.nullable;_tmp22A=_tmp227.bounds;_tmp22B=_tmp227.zero_term;}}_LL124: Cyc_Tcenv_check_rgn_accessible(
_tmp221,loc,_tmp228);Cyc_Tcutil_check_nonzero_bound(loc,_tmp22A);{void*_tmp22C=
Cyc_Tcutil_compress(_tmp225);struct Cyc_Absyn_FnInfo _tmp22E;struct Cyc_List_List*
_tmp22F;struct Cyc_Core_Opt*_tmp230;struct Cyc_Absyn_Tqual _tmp231;void*_tmp232;
struct Cyc_List_List*_tmp233;int _tmp234;struct Cyc_Absyn_VarargInfo*_tmp235;struct
Cyc_List_List*_tmp236;struct Cyc_List_List*_tmp237;_LL128: {struct Cyc_Absyn_FnType_struct*
_tmp22D=(struct Cyc_Absyn_FnType_struct*)_tmp22C;if(_tmp22D->tag != 9)goto _LL12A;
else{_tmp22E=_tmp22D->f1;_tmp22F=_tmp22E.tvars;_tmp230=_tmp22E.effect;_tmp231=
_tmp22E.ret_tqual;_tmp232=_tmp22E.ret_typ;_tmp233=_tmp22E.args;_tmp234=_tmp22E.c_varargs;
_tmp235=_tmp22E.cyc_varargs;_tmp236=_tmp22E.rgn_po;_tmp237=_tmp22E.attributes;}}
_LL129: if(topt != 0)Cyc_Tcutil_unify(_tmp232,*topt);while(_tmp21E != 0  && _tmp233
!= 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21E->hd;
void*t2=(*((struct _tuple9*)_tmp233->hd)).f3;Cyc_Tcexp_tcExp(_tmp221,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp221,e1,t2,& alias_coercion)){{const char*_tmp7CB;
void*_tmp7CA[2];struct Cyc_String_pa_struct _tmp7C9;struct Cyc_String_pa_struct
_tmp7C8;(_tmp7C8.tag=0,((_tmp7C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp7CA[0]=& _tmp7C9,((_tmp7CA[1]=& _tmp7C8,Cyc_Tcutil_terr(
e1->loc,((_tmp7CB="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7CB,sizeof(char),57))),_tag_dyneither(_tmp7CA,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7CC;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7CC=_cycalloc(sizeof(*_tmp7CC)),((_tmp7CC->hd=(void*)_tmp21F,((_tmp7CC->tl=*
alias_arg_exps,_tmp7CC))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp220,
t2) && !Cyc_Tcutil_is_noalias_path(_tmp220,e1)){const char*_tmp7CF;void*_tmp7CE;(
_tmp7CE=0,Cyc_Tcutil_terr(e1->loc,((_tmp7CF="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7CF,sizeof(char),49))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}
_tmp21E=_tmp21E->tl;_tmp233=_tmp233->tl;++ _tmp21F;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp237;for(0;a != 0;a=a->tl){void*_tmp23F=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp241;int _tmp242;int _tmp243;_LL12D: {struct Cyc_Absyn_Format_att_struct*
_tmp240=(struct Cyc_Absyn_Format_att_struct*)_tmp23F;if(_tmp240->tag != 19)goto
_LL12F;else{_tmp241=_tmp240->f1;_tmp242=_tmp240->f2;_tmp243=_tmp240->f3;}}_LL12E:{
struct _handler_cons _tmp244;_push_handler(& _tmp244);{int _tmp246=0;if(setjmp(
_tmp244.handler))_tmp246=1;if(!_tmp246){{struct Cyc_Absyn_Exp*_tmp247=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp242 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp243 == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp7D0;fmt_args=((
_tmp7D0=_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp243 - 1),_tmp7D0))));}
args_already_checked=1;{struct _RegionHandle*_tmp249=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp241){case Cyc_Absyn_Printf_ft: _LL131: Cyc_Tcexp_check_format_args(
_tmp221,_tmp247,fmt_args,_tmp243 - 1,alias_arg_exps,_tmp249,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL132: Cyc_Tcexp_check_format_args(_tmp221,_tmp247,
fmt_args,_tmp243 - 1,alias_arg_exps,_tmp249,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp245=(void*)_exn_thrown;void*_tmp24B=_tmp245;_LL135: {
struct Cyc_List_Nth_struct*_tmp24C=(struct Cyc_List_Nth_struct*)_tmp24B;if(_tmp24C->tag
!= Cyc_List_Nth)goto _LL137;}_LL136:{const char*_tmp7D3;void*_tmp7D2;(_tmp7D2=0,
Cyc_Tcutil_terr(loc,((_tmp7D3="bad format arguments",_tag_dyneither(_tmp7D3,
sizeof(char),21))),_tag_dyneither(_tmp7D2,sizeof(void*),0)));}goto _LL134;_LL137:;
_LL138:(void)_throw(_tmp24B);_LL134:;}};}goto _LL12C;_LL12F:;_LL130: goto _LL12C;
_LL12C:;}}if(_tmp233 != 0){const char*_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(
loc,((_tmp7D6="too few arguments for function",_tag_dyneither(_tmp7D6,sizeof(
char),31))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}else{if((_tmp21E != 0  || 
_tmp234) || _tmp235 != 0){if(_tmp234)for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){Cyc_Tcexp_tcExp(
_tmp221,0,(struct Cyc_Absyn_Exp*)_tmp21E->hd);}else{if(_tmp235 == 0){const char*
_tmp7D9;void*_tmp7D8;(_tmp7D8=0,Cyc_Tcutil_terr(loc,((_tmp7D9="too many arguments for function",
_tag_dyneither(_tmp7D9,sizeof(char),32))),_tag_dyneither(_tmp7D8,sizeof(void*),0)));}
else{void*_tmp254;int _tmp255;struct Cyc_Absyn_VarargInfo _tmp253=*_tmp235;_tmp254=
_tmp253.type;_tmp255=_tmp253.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp7DA;
struct Cyc_Absyn_VarargCallInfo*_tmp256=(_tmp7DA=_cycalloc(sizeof(*_tmp7DA)),((
_tmp7DA->num_varargs=0,((_tmp7DA->injectors=0,((_tmp7DA->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp235,_tmp7DA)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp256;if(!_tmp255)for(0;_tmp21E != 0;(_tmp21E=_tmp21E->tl,_tmp21F ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21E->hd;++
_tmp256->num_varargs;Cyc_Tcexp_tcExp(_tmp221,(void**)& _tmp254,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp221,e1,_tmp254,& alias_coercion)){{const char*_tmp7DF;void*_tmp7DE[2];struct
Cyc_String_pa_struct _tmp7DD;struct Cyc_String_pa_struct _tmp7DC;(_tmp7DC.tag=0,((
_tmp7DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp7DD.tag=0,((
_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp254)),((_tmp7DE[0]=& _tmp7DD,((_tmp7DE[1]=& _tmp7DC,Cyc_Tcutil_terr(loc,((
_tmp7DF="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp7DF,sizeof(char),49))),_tag_dyneither(_tmp7DE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp7E0;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp7E0=_cycalloc(sizeof(*_tmp7E0)),((
_tmp7E0->hd=(void*)_tmp21F,((_tmp7E0->tl=*alias_arg_exps,_tmp7E0))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp220,_tmp254) && !Cyc_Tcutil_is_noalias_path(_tmp220,e1)){const char*_tmp7E3;
void*_tmp7E2;(_tmp7E2=0,Cyc_Tcutil_terr(e1->loc,((_tmp7E3="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7E3,sizeof(char),49))),_tag_dyneither(_tmp7E2,sizeof(void*),0)));}}
else{void*_tmp25E=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp254));
struct Cyc_Absyn_DatatypeInfo _tmp260;union Cyc_Absyn_DatatypeInfoU _tmp261;struct
Cyc_Absyn_Datatypedecl**_tmp262;struct Cyc_Absyn_Datatypedecl*_tmp263;struct Cyc_List_List*
_tmp264;_LL13A: {struct Cyc_Absyn_DatatypeType_struct*_tmp25F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp25E;if(_tmp25F->tag != 3)goto _LL13C;else{_tmp260=_tmp25F->f1;_tmp261=_tmp260.datatype_info;
if((_tmp261.KnownDatatype).tag != 2)goto _LL13C;_tmp262=(struct Cyc_Absyn_Datatypedecl**)(
_tmp261.KnownDatatype).val;_tmp263=*_tmp262;_tmp264=_tmp260.targs;}}_LL13B: {
struct Cyc_Absyn_Datatypedecl*_tmp265=*Cyc_Tcenv_lookup_datatypedecl(_tmp221,loc,
_tmp263->name);struct Cyc_List_List*fields=0;if(_tmp265->fields == 0){const char*
_tmp7E7;void*_tmp7E6[1];struct Cyc_String_pa_struct _tmp7E5;(_tmp7E5.tag=0,((
_tmp7E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp254)),((_tmp7E6[0]=& _tmp7E5,Cyc_Tcutil_terr(loc,((_tmp7E7="can't inject into abstract datatype %s",
_tag_dyneither(_tmp7E7,sizeof(char),39))),_tag_dyneither(_tmp7E6,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp265->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp254),Cyc_Tcenv_curr_rgn(
_tmp221))){const char*_tmp7EC;void*_tmp7EB[2];struct Cyc_String_pa_struct _tmp7EA;
struct Cyc_String_pa_struct _tmp7E9;(_tmp7E9.tag=0,((_tmp7E9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp221))),((_tmp7EA.tag=0,((_tmp7EA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp254)),((_tmp7EB[0]=& _tmp7EA,((_tmp7EB[
1]=& _tmp7E9,Cyc_Tcutil_terr(loc,((_tmp7EC="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp7EC,sizeof(char),49))),_tag_dyneither(_tmp7EB,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp26D=Cyc_Tcenv_get_fnrgn(_tmp221);{struct Cyc_List_List*
_tmp26E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp26D,_tmp26D,
_tmp265->tvs,_tmp264);for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){++ _tmp256->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21E->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp221,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp26D,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp26D,e1)){const char*_tmp7EF;void*_tmp7EE;(_tmp7EE=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7EF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7EF,sizeof(char),49))),_tag_dyneither(_tmp7EE,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp271=Cyc_Tcexp_tcInjection(_tmp221,e1,Cyc_Tcutil_pointer_elt_type(
_tmp254),_tmp26D,_tmp26E,fields);if(_tmp271 != 0){struct Cyc_List_List*_tmp7F0;
_tmp256->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp256->injectors,((_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((
_tmp7F0->hd=(struct Cyc_Absyn_Datatypefield*)_tmp271,((_tmp7F0->tl=0,_tmp7F0)))))));}};};}}
goto _LL139;};}_LL13C:;_LL13D:{const char*_tmp7F3;void*_tmp7F2;(_tmp7F2=0,Cyc_Tcutil_terr(
loc,((_tmp7F3="bad inject vararg type",_tag_dyneither(_tmp7F3,sizeof(char),23))),
_tag_dyneither(_tmp7F2,sizeof(void*),0)));}goto _LL139;_LL139:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp221,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp230))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp221,loc,_tmp236);return _tmp232;};_LL12A:;_LL12B: {const char*_tmp7F6;void*
_tmp7F5;return(_tmp7F5=0,Cyc_Tcexp_expr_err(_tmp221,loc,topt,((_tmp7F6="expected pointer to function",
_tag_dyneither(_tmp7F6,sizeof(char),29))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}
_LL127:;};_LL125:;_LL126: {const char*_tmp7F9;void*_tmp7F8;return(_tmp7F8=0,Cyc_Tcexp_expr_err(
_tmp221,loc,topt,((_tmp7F9="expected pointer to function",_tag_dyneither(_tmp7F9,
sizeof(char),29))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));}_LL122:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp27A=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp27A,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp27A,&
bogus)){const char*_tmp7FE;void*_tmp7FD[2];struct Cyc_String_pa_struct _tmp7FC;
struct Cyc_String_pa_struct _tmp7FB;(_tmp7FB.tag=0,((_tmp7FB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7FC.tag=0,((_tmp7FC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27A)),((_tmp7FD[0]=& _tmp7FC,((
_tmp7FD[1]=& _tmp7FB,Cyc_Tcutil_terr(loc,((_tmp7FE="expected %s but found %s",
_tag_dyneither(_tmp7FE,sizeof(char),25))),_tag_dyneither(_tmp7FD,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp27F=t1;struct Cyc_Absyn_PtrInfo _tmp281;void*
_tmp282;struct Cyc_Absyn_Tqual _tmp283;struct Cyc_Absyn_PtrAtts _tmp284;void*_tmp285;
union Cyc_Absyn_Constraint*_tmp286;union Cyc_Absyn_Constraint*_tmp287;union Cyc_Absyn_Constraint*
_tmp288;_LL13F: {struct Cyc_Absyn_PointerType_struct*_tmp280=(struct Cyc_Absyn_PointerType_struct*)
_tmp27F;if(_tmp280->tag != 5)goto _LL141;else{_tmp281=_tmp280->f1;_tmp282=_tmp281.elt_typ;
_tmp283=_tmp281.elt_tq;_tmp284=_tmp281.ptr_atts;_tmp285=_tmp284.rgn;_tmp286=
_tmp284.nullable;_tmp287=_tmp284.bounds;_tmp288=_tmp284.zero_term;}}_LL140:{void*
_tmp289=Cyc_Tcutil_compress(_tmp282);struct Cyc_Absyn_FnInfo _tmp28B;struct Cyc_List_List*
_tmp28C;struct Cyc_Core_Opt*_tmp28D;struct Cyc_Absyn_Tqual _tmp28E;void*_tmp28F;
struct Cyc_List_List*_tmp290;int _tmp291;struct Cyc_Absyn_VarargInfo*_tmp292;struct
Cyc_List_List*_tmp293;struct Cyc_List_List*_tmp294;_LL144: {struct Cyc_Absyn_FnType_struct*
_tmp28A=(struct Cyc_Absyn_FnType_struct*)_tmp289;if(_tmp28A->tag != 9)goto _LL146;
else{_tmp28B=_tmp28A->f1;_tmp28C=_tmp28B.tvars;_tmp28D=_tmp28B.effect;_tmp28E=
_tmp28B.ret_tqual;_tmp28F=_tmp28B.ret_typ;_tmp290=_tmp28B.args;_tmp291=_tmp28B.c_varargs;
_tmp292=_tmp28B.cyc_varargs;_tmp293=_tmp28B.rgn_po;_tmp294=_tmp28B.attributes;}}
_LL145: {struct _RegionHandle*_tmp295=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*
instantiation=0;for(0;ts != 0  && _tmp28C != 0;(ts=ts->tl,_tmp28C=_tmp28C->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp28C->hd,&
Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(
void*)ts->hd);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{struct _tuple13*
_tmp801;struct Cyc_List_List*_tmp800;instantiation=((_tmp800=_region_malloc(
_tmp295,sizeof(*_tmp800)),((_tmp800->hd=((_tmp801=_region_malloc(_tmp295,sizeof(*
_tmp801)),((_tmp801->f1=(struct Cyc_Absyn_Tvar*)_tmp28C->hd,((_tmp801->f2=(void*)
ts->hd,_tmp801)))))),((_tmp800->tl=instantiation,_tmp800))))));};}if(ts != 0){
const char*_tmp804;void*_tmp803;return(_tmp803=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp804="too many type variables in instantiation",_tag_dyneither(_tmp804,
sizeof(char),41))),_tag_dyneither(_tmp803,sizeof(void*),0)));}{struct Cyc_Absyn_FnType_struct
_tmp80A;struct Cyc_Absyn_FnInfo _tmp809;struct Cyc_Absyn_FnType_struct*_tmp808;void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp295,instantiation,(void*)((_tmp808=
_cycalloc(sizeof(*_tmp808)),((_tmp808[0]=((_tmp80A.tag=9,((_tmp80A.f1=((_tmp809.tvars=
_tmp28C,((_tmp809.effect=_tmp28D,((_tmp809.ret_tqual=_tmp28E,((_tmp809.ret_typ=
_tmp28F,((_tmp809.args=_tmp290,((_tmp809.c_varargs=_tmp291,((_tmp809.cyc_varargs=
_tmp292,((_tmp809.rgn_po=_tmp293,((_tmp809.attributes=_tmp294,_tmp809)))))))))))))))))),
_tmp80A)))),_tmp808)))));struct Cyc_Absyn_PointerType_struct _tmp814;struct Cyc_Absyn_PtrAtts
_tmp813;struct Cyc_Absyn_PtrInfo _tmp812;struct Cyc_Absyn_PointerType_struct*
_tmp811;return(void*)((_tmp811=_cycalloc(sizeof(*_tmp811)),((_tmp811[0]=((
_tmp814.tag=5,((_tmp814.f1=((_tmp812.elt_typ=new_fn_typ,((_tmp812.elt_tq=_tmp283,((
_tmp812.ptr_atts=((_tmp813.rgn=_tmp285,((_tmp813.nullable=_tmp286,((_tmp813.bounds=
_tmp287,((_tmp813.zero_term=_tmp288,((_tmp813.ptrloc=0,_tmp813)))))))))),_tmp812)))))),
_tmp814)))),_tmp811))));};}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;
_LL142: goto _LL13E;_LL13E:;}{const char*_tmp818;void*_tmp817[1];struct Cyc_String_pa_struct
_tmp816;return(_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp817[0]=& _tmp816,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp818="expecting polymorphic type but found %s",_tag_dyneither(
_tmp818,sizeof(char),40))),_tag_dyneither(_tmp817,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(
loc,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp81D;void*_tmp81C[2];struct Cyc_String_pa_struct _tmp81B;struct Cyc_String_pa_struct
_tmp81A;void*_tmp2A4=(_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp81B.tag=0,((_tmp81B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp81C[0]=&
_tmp81B,((_tmp81C[1]=& _tmp81A,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp81D="cannot cast %s to %s",
_tag_dyneither(_tmp81D,sizeof(char),21))),_tag_dyneither(_tmp81C,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp2A4;};}}}{struct _tuple0 _tmp81E;struct
_tuple0 _tmp2AA=(_tmp81E.f1=e->r,((_tmp81E.f2=Cyc_Tcutil_compress(t),_tmp81E)));
void*_tmp2AB;struct Cyc_Absyn_MallocInfo _tmp2AD;int _tmp2AE;void*_tmp2AF;struct Cyc_Absyn_PtrInfo
_tmp2B1;struct Cyc_Absyn_PtrAtts _tmp2B2;union Cyc_Absyn_Constraint*_tmp2B3;union
Cyc_Absyn_Constraint*_tmp2B4;union Cyc_Absyn_Constraint*_tmp2B5;_LL149: _tmp2AB=
_tmp2AA.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp2AC=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2AB;if(_tmp2AC->tag != 34)goto _LL14B;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AD.fat_result;}};
_tmp2AF=_tmp2AA.f2;{struct Cyc_Absyn_PointerType_struct*_tmp2B0=(struct Cyc_Absyn_PointerType_struct*)
_tmp2AF;if(_tmp2B0->tag != 5)goto _LL14B;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B1.ptr_atts;
_tmp2B3=_tmp2B2.nullable;_tmp2B4=_tmp2B2.bounds;_tmp2B5=_tmp2B2.zero_term;}};
_LL14A: if((_tmp2AE  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B5)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B3)){void*_tmp2B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2B4);struct Cyc_Absyn_Exp*_tmp2B8;_LL14E: {struct Cyc_Absyn_Upper_b_struct*
_tmp2B7=(struct Cyc_Absyn_Upper_b_struct*)_tmp2B6;if(_tmp2B7->tag != 1)goto _LL150;
else{_tmp2B8=_tmp2B7->f1;}}_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp2B8)).f1
== 1){const char*_tmp821;void*_tmp820;(_tmp820=0,Cyc_Tcutil_warn(loc,((_tmp821="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp821,sizeof(char),60))),_tag_dyneither(_tmp820,sizeof(void*),0)));}
goto _LL14D;_LL150:;_LL151: goto _LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto
_LL148;_LL148:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp2BB=0;struct Cyc_Absyn_Tqual _tmp2BC=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp2BD=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2BF;
void*_tmp2C0;struct Cyc_Absyn_Tqual _tmp2C1;struct Cyc_Absyn_PtrAtts _tmp2C2;union
Cyc_Absyn_Constraint*_tmp2C3;_LL153: {struct Cyc_Absyn_PointerType_struct*_tmp2BE=(
struct Cyc_Absyn_PointerType_struct*)_tmp2BD;if(_tmp2BE->tag != 5)goto _LL155;else{
_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BF.elt_typ;_tmp2C1=_tmp2BF.elt_tq;_tmp2C2=
_tmp2BF.ptr_atts;_tmp2C3=_tmp2C2.zero_term;}}_LL154:{void**_tmp822;_tmp2BB=((
_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822[0]=_tmp2C0,_tmp822))));}_tmp2BC=
_tmp2C1;goto _LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle*
_tmp2C5=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(
_tmp2C5,Cyc_Tcenv_clear_notreadctxt(_tmp2C5,te)),_tmp2BB,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp2C5,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*
_tmp825;void*_tmp824;(_tmp824=0,Cyc_Tcutil_terr(e->loc,((_tmp825="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp825,sizeof(char),46))),_tag_dyneither(_tmp824,sizeof(void*),0)));}{
void*_tmp2C8=e->r;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2CB;_LL158: {
struct Cyc_Absyn_Subscript_e_struct*_tmp2C9=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp2C8;if(_tmp2C9->tag != 24)goto _LL15A;else{_tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2C9->f2;}}
_LL159:{void*_tmp2CC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CA->topt))->v);_LL15D: {struct Cyc_Absyn_TupleType_struct*
_tmp2CD=(struct Cyc_Absyn_TupleType_struct*)_tmp2CC;if(_tmp2CD->tag != 10)goto
_LL15F;}_LL15E: goto _LL15C;_LL15F:;_LL160: e0->r=(Cyc_Absyn_add_exp(_tmp2CA,
_tmp2CB,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp2CA,_tmp2CB);_LL15C:;}goto _LL157;
_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*_tmp2CE=e->r;int _tmp2D0;int _tmp2D2;
_LL162: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2CF=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2CE;if(_tmp2CF->tag != 22)goto _LL164;else{_tmp2D0=_tmp2CF->f3;if(_tmp2D0 != 1)
goto _LL164;}}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2D1=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2CE;if(_tmp2D1->tag != 23)goto
_LL166;else{_tmp2D2=_tmp2D1->f3;if(_tmp2D2 != 1)goto _LL166;}}_LL165:{const char*
_tmp828;void*_tmp827;(_tmp827=0,Cyc_Tcutil_terr(e->loc,((_tmp828="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp828,sizeof(char),50))),_tag_dyneither(_tmp827,sizeof(void*),0)));}
goto _LL161;_LL166:;_LL167: goto _LL161;_LL161:;}{int _tmp2D6;void*_tmp2D7;struct
_tuple14 _tmp2D5=Cyc_Tcutil_addressof_props(te,e);_tmp2D6=_tmp2D5.f1;_tmp2D7=
_tmp2D5.f2;if(Cyc_Tcutil_is_noalias_region(_tmp2D7,0)){const char*_tmp82B;void*
_tmp82A;(_tmp82A=0,Cyc_Tcutil_terr(e->loc,((_tmp82B="using & would manufacture an alias to an alias-free pointer",
_tag_dyneither(_tmp82B,sizeof(char),60))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}{
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2D6){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp2D7,tq,Cyc_Absyn_false_conref);return t;};};};};}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)
return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp82F;void*_tmp82E[1];struct Cyc_String_pa_struct _tmp82D;(_tmp82D.tag=
0,((_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp82E[0]=& _tmp82D,Cyc_Tcutil_terr(loc,((_tmp82F="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp82F,sizeof(char),55))),_tag_dyneither(_tmp82E,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2DD=Cyc_Tcutil_compress(*topt);void*_tmp2DF;_LL169: {
struct Cyc_Absyn_TagType_struct*_tmp2DE=(struct Cyc_Absyn_TagType_struct*)_tmp2DD;
if(_tmp2DE->tag != 19)goto _LL16B;else{_tmp2DF=(void*)_tmp2DE->f1;}}_LL16A: {
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp832;struct Cyc_Absyn_ValueofType_struct*_tmp831;struct Cyc_Absyn_ValueofType_struct*
_tmp2E1=(_tmp831=_cycalloc(sizeof(*_tmp831)),((_tmp831[0]=((_tmp832.tag=18,((
_tmp832.f1=_tmp2E0,_tmp832)))),_tmp831)));if(Cyc_Tcutil_unify(_tmp2DF,(void*)
_tmp2E1))return _tmp2DD;goto _LL168;}_LL16B:;_LL16C: goto _LL168;_LL168:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);{void*_tmp2E4=n;struct
_dyneither_ptr*_tmp2E6;unsigned int _tmp2E8;_LL16E: {struct Cyc_Absyn_StructField_struct*
_tmp2E5=(struct Cyc_Absyn_StructField_struct*)_tmp2E4;if(_tmp2E5->tag != 0)goto
_LL170;else{_tmp2E6=_tmp2E5->f1;}}_LL16F: {int bad_type=1;{void*_tmp2E9=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp2EB;union Cyc_Absyn_AggrInfoU _tmp2EC;struct Cyc_Absyn_Aggrdecl**
_tmp2ED;struct Cyc_List_List*_tmp2EF;_LL173: {struct Cyc_Absyn_AggrType_struct*
_tmp2EA=(struct Cyc_Absyn_AggrType_struct*)_tmp2E9;if(_tmp2EA->tag != 11)goto
_LL175;else{_tmp2EB=_tmp2EA->f1;_tmp2EC=_tmp2EB.aggr_info;if((_tmp2EC.KnownAggr).tag
!= 2)goto _LL175;_tmp2ED=(struct Cyc_Absyn_Aggrdecl**)(_tmp2EC.KnownAggr).val;}}
_LL174: if((*_tmp2ED)->impl == 0)goto _LL172;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2E6,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2ED)->impl))->fields)){const char*_tmp836;void*_tmp835[1];
struct Cyc_String_pa_struct _tmp834;(_tmp834.tag=0,((_tmp834.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2E6),((_tmp835[0]=& _tmp834,Cyc_Tcutil_terr(
loc,((_tmp836="no field of struct/union has name %s",_tag_dyneither(_tmp836,
sizeof(char),37))),_tag_dyneither(_tmp835,sizeof(void*),1)))))));}bad_type=0;
goto _LL172;_LL175: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2EE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2E9;if(_tmp2EE->tag != 12)goto _LL177;else{_tmp2EF=_tmp2EE->f2;}}_LL176: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2E6,_tmp2EF)){const char*_tmp83A;void*_tmp839[1];struct Cyc_String_pa_struct
_tmp838;(_tmp838.tag=0,((_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2E6),((_tmp839[0]=& _tmp838,Cyc_Tcutil_terr(loc,((_tmp83A="no field of struct/union has name %s",
_tag_dyneither(_tmp83A,sizeof(char),37))),_tag_dyneither(_tmp839,sizeof(void*),1)))))));}
bad_type=0;goto _LL172;_LL177:;_LL178: goto _LL172;_LL172:;}if(bad_type){const char*
_tmp83E;void*_tmp83D[1];struct Cyc_String_pa_struct _tmp83C;(_tmp83C.tag=0,((
_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp83D[0]=& _tmp83C,Cyc_Tcutil_terr(loc,((_tmp83E="%s is not a known struct/union type",
_tag_dyneither(_tmp83E,sizeof(char),36))),_tag_dyneither(_tmp83D,sizeof(void*),1)))))));}
goto _LL16D;}_LL170: {struct Cyc_Absyn_TupleIndex_struct*_tmp2E7=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E4;if(_tmp2E7->tag != 1)goto _LL16D;else{_tmp2E8=_tmp2E7->f1;}}_LL171: {int
bad_type=1;{void*_tmp2F9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2FB;
union Cyc_Absyn_AggrInfoU _tmp2FC;struct Cyc_Absyn_Aggrdecl**_tmp2FD;struct Cyc_List_List*
_tmp2FF;struct Cyc_List_List*_tmp301;struct Cyc_Absyn_DatatypeFieldInfo _tmp303;
union Cyc_Absyn_DatatypeFieldInfoU _tmp304;struct _tuple2 _tmp305;struct Cyc_Absyn_Datatypefield*
_tmp306;_LL17A: {struct Cyc_Absyn_AggrType_struct*_tmp2FA=(struct Cyc_Absyn_AggrType_struct*)
_tmp2F9;if(_tmp2FA->tag != 11)goto _LL17C;else{_tmp2FB=_tmp2FA->f1;_tmp2FC=_tmp2FB.aggr_info;
if((_tmp2FC.KnownAggr).tag != 2)goto _LL17C;_tmp2FD=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2FC.KnownAggr).val;}}_LL17B: if((*_tmp2FD)->impl == 0)goto _LL179;_tmp2FF=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2FD)->impl))->fields;goto _LL17D;
_LL17C: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2FE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F9;if(_tmp2FE->tag != 12)goto _LL17E;else{_tmp2FF=_tmp2FE->f2;}}_LL17D: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FF)<= _tmp2E8){const char*
_tmp843;void*_tmp842[2];struct Cyc_Int_pa_struct _tmp841;struct Cyc_Int_pa_struct
_tmp840;(_tmp840.tag=1,((_tmp840.f1=(unsigned long)((int)_tmp2E8),((_tmp841.tag=
1,((_tmp841.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FF),((_tmp842[0]=& _tmp841,((_tmp842[1]=& _tmp840,Cyc_Tcutil_terr(loc,((
_tmp843="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp843,
sizeof(char),46))),_tag_dyneither(_tmp842,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL179;_LL17E: {struct Cyc_Absyn_TupleType_struct*_tmp300=(struct Cyc_Absyn_TupleType_struct*)
_tmp2F9;if(_tmp300->tag != 10)goto _LL180;else{_tmp301=_tmp300->f1;}}_LL17F: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp301)<= _tmp2E8){const char*
_tmp848;void*_tmp847[2];struct Cyc_Int_pa_struct _tmp846;struct Cyc_Int_pa_struct
_tmp845;(_tmp845.tag=1,((_tmp845.f1=(unsigned long)((int)_tmp2E8),((_tmp846.tag=
1,((_tmp846.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp301),((_tmp847[0]=& _tmp846,((_tmp847[1]=& _tmp845,Cyc_Tcutil_terr(loc,((
_tmp848="tuple has too few components: %d <= %d",_tag_dyneither(_tmp848,sizeof(
char),39))),_tag_dyneither(_tmp847,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL179;_LL180: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp302=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2F9;if(_tmp302->tag != 4)goto _LL182;else{_tmp303=_tmp302->f1;_tmp304=_tmp303.field_info;
if((_tmp304.KnownDatatypefield).tag != 2)goto _LL182;_tmp305=(struct _tuple2)(
_tmp304.KnownDatatypefield).val;_tmp306=_tmp305.f2;}}_LL181: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp306->typs)< _tmp2E8){const char*_tmp84D;
void*_tmp84C[2];struct Cyc_Int_pa_struct _tmp84B;struct Cyc_Int_pa_struct _tmp84A;(
_tmp84A.tag=1,((_tmp84A.f1=(unsigned long)((int)_tmp2E8),((_tmp84B.tag=1,((
_tmp84B.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp306->typs),((_tmp84C[0]=& _tmp84B,((_tmp84C[1]=& _tmp84A,Cyc_Tcutil_terr(loc,((
_tmp84D="datatype field has too few components: %d < %d",_tag_dyneither(_tmp84D,
sizeof(char),47))),_tag_dyneither(_tmp84C,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL179;_LL182:;_LL183: goto _LL179;_LL179:;}if(bad_type){const char*_tmp851;
void*_tmp850[1];struct Cyc_String_pa_struct _tmp84F;(_tmp84F.tag=0,((_tmp84F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp850[
0]=& _tmp84F,Cyc_Tcutil_terr(loc,((_tmp851="%s is not a known type",
_tag_dyneither(_tmp851,sizeof(char),23))),_tag_dyneither(_tmp850,sizeof(void*),1)))))));}
goto _LL16D;}_LL16D:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp316=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp317=Cyc_Tcenv_clear_lhs(_tmp316,Cyc_Tcenv_clear_notreadctxt(_tmp316,te_orig));
Cyc_Tcexp_tcExp(_tmp317,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp318=t;struct Cyc_Absyn_PtrInfo _tmp31B;void*
_tmp31C;struct Cyc_Absyn_PtrAtts _tmp31D;void*_tmp31E;union Cyc_Absyn_Constraint*
_tmp31F;union Cyc_Absyn_Constraint*_tmp320;_LL185: {struct Cyc_Absyn_Evar_struct*
_tmp319=(struct Cyc_Absyn_Evar_struct*)_tmp318;if(_tmp319->tag != 1)goto _LL187;}
_LL186: {struct Cyc_List_List*_tmp321=Cyc_Tcenv_lookup_type_vars(_tmp317);struct
Cyc_Core_Opt*_tmp852;void*_tmp322=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((
_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852->v=_tmp321,_tmp852)))));struct Cyc_Core_Opt*
_tmp853;void*_tmp323=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp853=_cycalloc(sizeof(*_tmp853)),((_tmp853->v=_tmp321,_tmp853)))));union Cyc_Absyn_Constraint*
_tmp324=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*
_tmp325=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp854;struct Cyc_Absyn_PtrAtts _tmp326=(_tmp854.rgn=_tmp323,((_tmp854.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp854.bounds=
_tmp324,((_tmp854.zero_term=_tmp325,((_tmp854.ptrloc=0,_tmp854)))))))));struct
Cyc_Absyn_PointerType_struct _tmp85A;struct Cyc_Absyn_PtrInfo _tmp859;struct Cyc_Absyn_PointerType_struct*
_tmp858;struct Cyc_Absyn_PointerType_struct*_tmp327=(_tmp858=_cycalloc(sizeof(*
_tmp858)),((_tmp858[0]=((_tmp85A.tag=5,((_tmp85A.f1=((_tmp859.elt_typ=_tmp322,((
_tmp859.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp859.ptr_atts=_tmp326,_tmp859)))))),
_tmp85A)))),_tmp858)));Cyc_Tcutil_unify(t,(void*)_tmp327);_tmp31C=_tmp322;
_tmp31E=_tmp323;_tmp31F=_tmp324;_tmp320=_tmp325;goto _LL188;}_LL187: {struct Cyc_Absyn_PointerType_struct*
_tmp31A=(struct Cyc_Absyn_PointerType_struct*)_tmp318;if(_tmp31A->tag != 5)goto
_LL189;else{_tmp31B=_tmp31A->f1;_tmp31C=_tmp31B.elt_typ;_tmp31D=_tmp31B.ptr_atts;
_tmp31E=_tmp31D.rgn;_tmp31F=_tmp31D.bounds;_tmp320=_tmp31D.zero_term;}}_LL188:
Cyc_Tcenv_check_rgn_accessible(_tmp317,loc,_tmp31E);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp31F);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp31C),& Cyc_Tcutil_tmk)){
const char*_tmp85D;void*_tmp85C;(_tmp85C=0,Cyc_Tcutil_terr(loc,((_tmp85D="can't dereference abstract pointer type",
_tag_dyneither(_tmp85D,sizeof(char),40))),_tag_dyneither(_tmp85C,sizeof(void*),0)));}
return _tmp31C;_LL189:;_LL18A: {const char*_tmp861;void*_tmp860[1];struct Cyc_String_pa_struct
_tmp85F;return(_tmp85F.tag=0,((_tmp85F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp860[0]=& _tmp85F,Cyc_Tcexp_expr_err(
_tmp317,loc,topt,((_tmp861="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp861,sizeof(char),39))),_tag_dyneither(_tmp860,sizeof(void*),1)))))));}
_LL184:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp333=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(_tmp333,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp334=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp336;
union Cyc_Absyn_AggrInfoU _tmp337;struct Cyc_Absyn_Aggrdecl**_tmp338;struct Cyc_Absyn_Aggrdecl*
_tmp339;struct Cyc_List_List*_tmp33A;enum Cyc_Absyn_AggrKind _tmp33C;struct Cyc_List_List*
_tmp33D;_LL18C: {struct Cyc_Absyn_AggrType_struct*_tmp335=(struct Cyc_Absyn_AggrType_struct*)
_tmp334;if(_tmp335->tag != 11)goto _LL18E;else{_tmp336=_tmp335->f1;_tmp337=_tmp336.aggr_info;
if((_tmp337.KnownAggr).tag != 2)goto _LL18E;_tmp338=(struct Cyc_Absyn_Aggrdecl**)(
_tmp337.KnownAggr).val;_tmp339=*_tmp338;_tmp33A=_tmp336.targs;}}_LL18D: {struct
Cyc_Absyn_Aggrfield*_tmp342=Cyc_Absyn_lookup_decl_field(_tmp339,f);if(_tmp342 == 
0){const char*_tmp866;void*_tmp865[2];struct Cyc_String_pa_struct _tmp864;struct Cyc_String_pa_struct
_tmp863;return(_tmp863.tag=0,((_tmp863.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp339->name)),((_tmp865[0]=& _tmp864,((
_tmp865[1]=& _tmp863,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp866="type %s has no %s field",
_tag_dyneither(_tmp866,sizeof(char),24))),_tag_dyneither(_tmp865,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp339->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp347=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp348=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp347,_tmp347,_tmp339->tvs,_tmp33A);t2=Cyc_Tcutil_rsubstitute(_tmp347,_tmp348,
_tmp342->type);if(((_tmp339->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp339->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp86A;void*_tmp869[1];struct Cyc_String_pa_struct _tmp868;return(
_tmp868.tag=0,((_tmp868.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp869[0]=& _tmp868,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86A="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp86A,sizeof(char),56))),_tag_dyneither(_tmp869,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp339->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp86E;void*_tmp86D[1];struct Cyc_String_pa_struct _tmp86C;return(
_tmp86C.tag=0,((_tmp86C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp86D[0]=& _tmp86C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86E="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp86E,sizeof(char),81))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))));}}
return t2;};}_LL18E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp33B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp334;if(_tmp33B->tag != 12)goto _LL190;else{_tmp33C=_tmp33B->f1;_tmp33D=_tmp33B->f2;}}
_LL18F: {struct Cyc_Absyn_Aggrfield*_tmp34F=Cyc_Absyn_lookup_field(_tmp33D,f);if(
_tmp34F == 0){const char*_tmp872;void*_tmp871[1];struct Cyc_String_pa_struct _tmp870;
return(_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp871[0]=& _tmp870,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp872="type has no %s field",
_tag_dyneither(_tmp872,sizeof(char),21))),_tag_dyneither(_tmp871,sizeof(void*),1)))))));}
if((_tmp33C == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp34F->type)){const char*_tmp876;void*_tmp875[1];struct Cyc_String_pa_struct
_tmp874;return(_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp875[0]=& _tmp874,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp876="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp876,sizeof(char),56))),_tag_dyneither(_tmp875,sizeof(void*),1)))))));}return
_tmp34F->type;}_LL190:{struct Cyc_Absyn_ArrayType_struct*_tmp33E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp334;if(_tmp33E->tag != 8)goto _LL192;}{const char*_tmp877;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp877="size",_tag_dyneither(_tmp877,sizeof(char),5))))
== 0))goto _LL192;};_LL191: goto _LL193;_LL192:{struct Cyc_Absyn_PointerType_struct*
_tmp340=(struct Cyc_Absyn_PointerType_struct*)_tmp334;if(_tmp340->tag != 5)goto
_LL194;}{const char*_tmp878;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp878="size",
_tag_dyneither(_tmp878,sizeof(char),5))))== 0))goto _LL194;};_LL193:{const char*
_tmp87C;void*_tmp87B[1];struct Cyc_String_pa_struct _tmp87A;(_tmp87A.tag=0,((
_tmp87A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp87B[0]=& _tmp87A,Cyc_Tcutil_warn(e->loc,((_tmp87C="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp87C,sizeof(char),54))),_tag_dyneither(_tmp87B,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp882;struct Cyc_List_List*_tmp881;struct Cyc_Absyn_Primop_e_struct*
_tmp880;outer_e->r=(void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((
_tmp882.tag=3,((_tmp882.f1=Cyc_Absyn_Numelts,((_tmp882.f2=((_tmp881=_cycalloc(
sizeof(*_tmp881)),((_tmp881->hd=e,((_tmp881->tl=0,_tmp881)))))),_tmp882)))))),
_tmp880))));}return Cyc_Absyn_uint_typ;_LL194:;_LL195: {const char*_tmp883;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp883="size",_tag_dyneither(_tmp883,sizeof(char),5))))
== 0){const char*_tmp887;void*_tmp886[1];struct Cyc_String_pa_struct _tmp885;return(
_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp886[0]=& _tmp885,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp887="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp887,sizeof(char),44))),_tag_dyneither(_tmp886,sizeof(void*),1)))))));}
else{const char*_tmp88B;void*_tmp88A[1];struct Cyc_String_pa_struct _tmp889;return(
_tmp889.tag=0,((_tmp889.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp88A[0]=& _tmp889,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp88B="expecting struct or union, found %s",_tag_dyneither(
_tmp88B,sizeof(char),36))),_tag_dyneither(_tmp88A,sizeof(void*),1)))))));}}
_LL18B:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp363=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(
_tmp363,Cyc_Tcenv_clear_notreadctxt(_tmp363,te)),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp364=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp366;void*_tmp367;struct Cyc_Absyn_PtrAtts _tmp368;void*
_tmp369;union Cyc_Absyn_Constraint*_tmp36A;union Cyc_Absyn_Constraint*_tmp36B;
_LL197: {struct Cyc_Absyn_PointerType_struct*_tmp365=(struct Cyc_Absyn_PointerType_struct*)
_tmp364;if(_tmp365->tag != 5)goto _LL199;else{_tmp366=_tmp365->f1;_tmp367=_tmp366.elt_typ;
_tmp368=_tmp366.ptr_atts;_tmp369=_tmp368.rgn;_tmp36A=_tmp368.bounds;_tmp36B=
_tmp368.zero_term;}}_LL198: Cyc_Tcutil_check_nonzero_bound(loc,_tmp36A);{void*
_tmp36C=Cyc_Tcutil_compress(_tmp367);struct Cyc_Absyn_AggrInfo _tmp36E;union Cyc_Absyn_AggrInfoU
_tmp36F;struct Cyc_Absyn_Aggrdecl**_tmp370;struct Cyc_Absyn_Aggrdecl*_tmp371;
struct Cyc_List_List*_tmp372;enum Cyc_Absyn_AggrKind _tmp374;struct Cyc_List_List*
_tmp375;_LL19C: {struct Cyc_Absyn_AggrType_struct*_tmp36D=(struct Cyc_Absyn_AggrType_struct*)
_tmp36C;if(_tmp36D->tag != 11)goto _LL19E;else{_tmp36E=_tmp36D->f1;_tmp36F=_tmp36E.aggr_info;
if((_tmp36F.KnownAggr).tag != 2)goto _LL19E;_tmp370=(struct Cyc_Absyn_Aggrdecl**)(
_tmp36F.KnownAggr).val;_tmp371=*_tmp370;_tmp372=_tmp36E.targs;}}_LL19D: {struct
Cyc_Absyn_Aggrfield*_tmp376=Cyc_Absyn_lookup_decl_field(_tmp371,f);if(_tmp376 == 
0){const char*_tmp890;void*_tmp88F[2];struct Cyc_String_pa_struct _tmp88E;struct Cyc_String_pa_struct
_tmp88D;return(_tmp88D.tag=0,((_tmp88D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp88E.tag=0,((_tmp88E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp371->name)),((_tmp88F[0]=& _tmp88E,((
_tmp88F[1]=& _tmp88D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp890="type %s has no %s field",
_tag_dyneither(_tmp890,sizeof(char),24))),_tag_dyneither(_tmp88F,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp371->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp37B=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp37C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp37B,_tmp37B,_tmp371->tvs,_tmp372);t3=Cyc_Tcutil_rsubstitute(_tmp37B,_tmp37C,
_tmp376->type);}{struct Cyc_Absyn_Kind*_tmp37D=Cyc_Tcutil_typ_kind(t3);if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,_tmp37D)){const char*_tmp894;void*_tmp893[1];struct Cyc_String_pa_struct
_tmp892;return(_tmp892.tag=0,((_tmp892.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp893[0]=& _tmp892,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp894="cannot get member %s since its type is abstract",_tag_dyneither(_tmp894,
sizeof(char),48))),_tag_dyneither(_tmp893,sizeof(void*),1)))))));}if(((_tmp371->kind
== Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp371->impl))->tagged)
 && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){const char*
_tmp898;void*_tmp897[1];struct Cyc_String_pa_struct _tmp896;return(_tmp896.tag=0,((
_tmp896.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp897[0]=&
_tmp896,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp898="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp898,sizeof(char),56))),_tag_dyneither(_tmp897,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp371->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_struct _tmp89A;return(
_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp89B[0]=& _tmp89A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89C="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp89C,sizeof(char),72))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))));}}
return t3;};};}_LL19E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp373=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp36C;if(_tmp373->tag != 12)goto _LL1A0;else{_tmp374=_tmp373->f1;_tmp375=_tmp373->f2;}}
_LL19F: {struct Cyc_Absyn_Aggrfield*_tmp387=Cyc_Absyn_lookup_field(_tmp375,f);if(
_tmp387 == 0){const char*_tmp8A0;void*_tmp89F[1];struct Cyc_String_pa_struct _tmp89E;
return(_tmp89E.tag=0,((_tmp89E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp89F[0]=& _tmp89E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A0="type has no %s field",
_tag_dyneither(_tmp8A0,sizeof(char),21))),_tag_dyneither(_tmp89F,sizeof(void*),1)))))));}
if((_tmp374 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp387->type)){const char*_tmp8A4;void*_tmp8A3[1];struct Cyc_String_pa_struct
_tmp8A2;return(_tmp8A2.tag=0,((_tmp8A2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8A3[0]=& _tmp8A2,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8A4="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp8A4,sizeof(char),56))),_tag_dyneither(_tmp8A3,sizeof(void*),1)))))));}return
_tmp387->type;}_LL1A0:;_LL1A1: goto _LL19B;_LL19B:;}goto _LL196;_LL199:;_LL19A: goto
_LL196;_LL196:;}{const char*_tmp8A8;void*_tmp8A7[1];struct Cyc_String_pa_struct
_tmp8A6;return(_tmp8A6.tag=0,((_tmp8A6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp8A7[0]=& _tmp8A6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A8="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp8A8,sizeof(char),44))),_tag_dyneither(_tmp8A7,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp392;int
_tmp393;struct _tuple15 _tmp391=Cyc_Evexp_eval_const_uint_exp(index);_tmp392=
_tmp391.f1;_tmp393=_tmp391.f2;if(!_tmp393){const char*_tmp8AB;void*_tmp8AA;return(
_tmp8AA=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8AB="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp8AB,sizeof(char),47))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}{
struct _handler_cons _tmp396;_push_handler(& _tmp396);{int _tmp398=0;if(setjmp(
_tmp396.handler))_tmp398=1;if(!_tmp398){{void*_tmp399=(*((struct _tuple19*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp392)).f2;_npop_handler(0);
return _tmp399;};_pop_handler();}else{void*_tmp397=(void*)_exn_thrown;void*
_tmp39B=_tmp397;_LL1A3: {struct Cyc_List_Nth_struct*_tmp39C=(struct Cyc_List_Nth_struct*)
_tmp39B;if(_tmp39C->tag != Cyc_List_Nth)goto _LL1A5;}_LL1A4: {const char*_tmp8B0;
void*_tmp8AF[2];struct Cyc_Int_pa_struct _tmp8AE;struct Cyc_Int_pa_struct _tmp8AD;
return(_tmp8AD.tag=1,((_tmp8AD.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp8AE.tag=1,((_tmp8AE.f1=(unsigned long)((int)_tmp392),((
_tmp8AF[0]=& _tmp8AE,((_tmp8AF[1]=& _tmp8AD,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8B0="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp8B0,sizeof(char),41))),_tag_dyneither(_tmp8AF,sizeof(void*),2)))))))))))));}
_LL1A5:;_LL1A6:(void)_throw(_tmp39B);_LL1A2:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp3A1=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp3A2=Cyc_Tcenv_clear_lhs(_tmp3A1,Cyc_Tcenv_clear_notreadctxt(
_tmp3A1,te_orig));Cyc_Tcexp_tcExp(_tmp3A2,0,e1);Cyc_Tcexp_tcExp(_tmp3A2,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3A2,e2)){const char*_tmp8B4;void*_tmp8B3[1];
struct Cyc_String_pa_struct _tmp8B2;return(_tmp8B2.tag=0,((_tmp8B2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp8B3[0]=&
_tmp8B2,Cyc_Tcexp_expr_err(_tmp3A2,e2->loc,topt,((_tmp8B4="expecting int subscript, found %s",
_tag_dyneither(_tmp8B4,sizeof(char),34))),_tag_dyneither(_tmp8B3,sizeof(void*),1)))))));}{
void*_tmp3A6=t1;struct Cyc_Absyn_PtrInfo _tmp3A8;void*_tmp3A9;struct Cyc_Absyn_Tqual
_tmp3AA;struct Cyc_Absyn_PtrAtts _tmp3AB;void*_tmp3AC;union Cyc_Absyn_Constraint*
_tmp3AD;union Cyc_Absyn_Constraint*_tmp3AE;struct Cyc_List_List*_tmp3B0;_LL1A8: {
struct Cyc_Absyn_PointerType_struct*_tmp3A7=(struct Cyc_Absyn_PointerType_struct*)
_tmp3A6;if(_tmp3A7->tag != 5)goto _LL1AA;else{_tmp3A8=_tmp3A7->f1;_tmp3A9=_tmp3A8.elt_typ;
_tmp3AA=_tmp3A8.elt_tq;_tmp3AB=_tmp3A8.ptr_atts;_tmp3AC=_tmp3AB.rgn;_tmp3AD=
_tmp3AB.bounds;_tmp3AE=_tmp3AB.zero_term;}}_LL1A9: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3AE)){int emit_warning=1;{void*_tmp3B1=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp3AD);struct Cyc_Absyn_Exp*_tmp3B3;_LL1AF: {struct Cyc_Absyn_Upper_b_struct*
_tmp3B2=(struct Cyc_Absyn_Upper_b_struct*)_tmp3B1;if(_tmp3B2->tag != 1)goto _LL1B1;
else{_tmp3B3=_tmp3B2->f1;}}_LL1B0: if(Cyc_Tcutil_is_const_exp(_tmp3A2,e2)){
unsigned int _tmp3B6;int _tmp3B7;struct _tuple15 _tmp3B5=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3B6=_tmp3B5.f1;_tmp3B7=_tmp3B5.f2;if(_tmp3B7){unsigned int _tmp3B9;int
_tmp3BA;struct _tuple15 _tmp3B8=Cyc_Evexp_eval_const_uint_exp(_tmp3B3);_tmp3B9=
_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;if(_tmp3BA  && _tmp3B9 > _tmp3B6)emit_warning=0;}}
goto _LL1AE;_LL1B1: {struct Cyc_Absyn_DynEither_b_struct*_tmp3B4=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3B1;if(_tmp3B4->tag != 0)goto _LL1AE;}_LL1B2: emit_warning=0;goto _LL1AE;_LL1AE:;}
if(emit_warning){const char*_tmp8B7;void*_tmp8B6;(_tmp8B6=0,Cyc_Tcutil_warn(e2->loc,((
_tmp8B7="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp8B7,sizeof(char),63))),_tag_dyneither(_tmp8B6,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp3A2,e2)){unsigned int _tmp3BE;int _tmp3BF;
struct _tuple15 _tmp3BD=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3BE=_tmp3BD.f1;
_tmp3BF=_tmp3BD.f2;if(_tmp3BF)Cyc_Tcutil_check_bound(loc,_tmp3BE,_tmp3AD);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3AD) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3AE)){const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,
Cyc_Tcutil_warn(e1->loc,((_tmp8BA="subscript applied to pointer to one object",
_tag_dyneither(_tmp8BA,sizeof(char),43))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3AD);}}Cyc_Tcenv_check_rgn_accessible(
_tmp3A2,loc,_tmp3AC);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3A9),& Cyc_Tcutil_tmk)){
const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,Cyc_Tcutil_terr(e1->loc,((_tmp8BD="can't subscript an abstract pointer",
_tag_dyneither(_tmp8BD,sizeof(char),36))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}
return _tmp3A9;_LL1AA: {struct Cyc_Absyn_TupleType_struct*_tmp3AF=(struct Cyc_Absyn_TupleType_struct*)
_tmp3A6;if(_tmp3AF->tag != 10)goto _LL1AC;else{_tmp3B0=_tmp3AF->f1;}}_LL1AB: return
Cyc_Tcexp_ithTupleType(_tmp3A2,loc,_tmp3B0,e2);_LL1AC:;_LL1AD: {const char*
_tmp8C1;void*_tmp8C0[1];struct Cyc_String_pa_struct _tmp8BF;return(_tmp8BF.tag=0,((
_tmp8BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp8C0[0]=& _tmp8BF,Cyc_Tcexp_expr_err(_tmp3A2,loc,topt,((_tmp8C1="subscript applied to %s",
_tag_dyneither(_tmp8C1,sizeof(char),24))),_tag_dyneither(_tmp8C0,sizeof(void*),1)))))));}
_LL1A7:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp3C7=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3C9;
_LL1B4: {struct Cyc_Absyn_TupleType_struct*_tmp3C8=(struct Cyc_Absyn_TupleType_struct*)
_tmp3C7;if(_tmp3C8->tag != 10)goto _LL1B6;else{_tmp3C9=_tmp3C8->f1;}}_LL1B5: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3C9)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1B3;for(0;es != 0;(es=es->tl,_tmp3C9=_tmp3C9->tl)){
int bogus=0;void*_tmp3CA=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(
_tmp3C9))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3CA,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)
_tmp3C9->hd)).f2,& bogus);{struct _tuple19*_tmp8C4;struct Cyc_List_List*_tmp8C3;
fields=((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3->hd=((_tmp8C4=_cycalloc(
sizeof(*_tmp8C4)),((_tmp8C4->f1=(*((struct _tuple19*)_tmp3C9->hd)).f1,((_tmp8C4->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp8C4)))))),((_tmp8C3->tl=fields,_tmp8C3))))));};}done=1;goto _LL1B3;_LL1B6:;
_LL1B7: goto _LL1B3;_LL1B3:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);{struct _tuple19*_tmp8C7;struct Cyc_List_List*
_tmp8C6;fields=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6->hd=((_tmp8C7=
_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7->f1=Cyc_Absyn_empty_tqual(0),((_tmp8C7->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp8C7)))))),((_tmp8C6->tl=fields,_tmp8C6))))));};}{struct Cyc_Absyn_TupleType_struct
_tmp8CA;struct Cyc_Absyn_TupleType_struct*_tmp8C9;return(void*)((_tmp8C9=
_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CA.tag=10,((_tmp8CA.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8CA)))),
_tmp8C9))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
const char*_tmp8CD;void*_tmp8CC;return(_tmp8CC=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8CD="tcCompoundLit",_tag_dyneither(_tmp8CD,sizeof(char),14))),_tag_dyneither(
_tmp8CC,sizeof(void*),0)));}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,
int zero_term,struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp3D3=
Cyc_Tcenv_get_fnrgn(te);{int _tmp3D4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
des);struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmp3D3,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct
Cyc_Absyn_Const_e_struct _tmp8D0;struct Cyc_Absyn_Const_e_struct*_tmp8CF;struct Cyc_Absyn_Const_e_struct*
_tmp3D5=(_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=((_tmp8D0.tag=0,((
_tmp8D0.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3D4),_tmp8D0)))),_tmp8CF)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3D5,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3D6=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp3D4 - 1);if(!Cyc_Tcutil_is_zero(_tmp3D6)){const char*_tmp8D3;
void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_terr(_tmp3D6->loc,((_tmp8D3="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp8D3,sizeof(char),45))),_tag_dyneither(_tmp8D2,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp8D4;sz_exp->topt=((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((
_tmp8D4->v=Cyc_Absyn_uint_typ,_tmp8D4))));}res_t2=Cyc_Absyn_array_typ(res,(
unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es)){const char*
_tmp8D8;void*_tmp8D7[1];struct Cyc_String_pa_struct _tmp8D6;(_tmp8D6.tag=0,((
_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp8D7[0]=& _tmp8D6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp8D8="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp8D8,sizeof(char),53))),_tag_dyneither(_tmp8D7,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple18*)des->hd)).f1;if(ds != 0){void*_tmp3DF=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3E2;_LL1B9: {struct Cyc_Absyn_FieldName_struct*_tmp3E0=(struct Cyc_Absyn_FieldName_struct*)
_tmp3DF;if(_tmp3E0->tag != 1)goto _LL1BB;}_LL1BA:{const char*_tmp8DB;void*_tmp8DA;(
_tmp8DA=0,Cyc_Tcutil_terr(loc,((_tmp8DB="only array index designators are supported",
_tag_dyneither(_tmp8DB,sizeof(char),43))),_tag_dyneither(_tmp8DA,sizeof(void*),0)));}
goto _LL1B8;_LL1BB: {struct Cyc_Absyn_ArrayElement_struct*_tmp3E1=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3DF;if(_tmp3E1->tag != 0)goto _LL1B8;else{_tmp3E2=_tmp3E1->f1;}}_LL1BC: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3E2);{unsigned int _tmp3E6;int _tmp3E7;struct _tuple15 _tmp3E5=Cyc_Evexp_eval_const_uint_exp(
_tmp3E2);_tmp3E6=_tmp3E5.f1;_tmp3E7=_tmp3E5.f2;if(!_tmp3E7){const char*_tmp8DE;
void*_tmp8DD;(_tmp8DD=0,Cyc_Tcutil_terr(_tmp3E2->loc,((_tmp8DE="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp8DE,sizeof(char),47))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}
else{if(_tmp3E6 != offset){const char*_tmp8E3;void*_tmp8E2[2];struct Cyc_Int_pa_struct
_tmp8E1;struct Cyc_Int_pa_struct _tmp8E0;(_tmp8E0.tag=1,((_tmp8E0.f1=(
unsigned long)((int)_tmp3E6),((_tmp8E1.tag=1,((_tmp8E1.f1=(unsigned long)offset,((
_tmp8E2[0]=& _tmp8E1,((_tmp8E2[1]=& _tmp8E0,Cyc_Tcutil_terr(_tmp3E2->loc,((_tmp8E3="expecting index designator %d but found %d",
_tag_dyneither(_tmp8E3,sizeof(char),43))),_tag_dyneither(_tmp8E2,sizeof(void*),2)))))))))))));}}
goto _LL1B8;};_LL1B8:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3EE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1BE: {struct Cyc_Absyn_TagType_struct*_tmp3EF=(
struct Cyc_Absyn_TagType_struct*)_tmp3EE;if(_tmp3EF->tag != 19)goto _LL1C0;}_LL1BF:
goto _LL1BD;_LL1C0:;_LL1C1: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp8E7;void*_tmp8E6[1];struct Cyc_String_pa_struct _tmp8E5;(_tmp8E5.tag=0,((
_tmp8E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp8E6[0]=& _tmp8E5,
Cyc_Tcutil_terr(bound->loc,((_tmp8E7="expecting unsigned int, found %s",
_tag_dyneither(_tmp8E7,sizeof(char),33))),_tag_dyneither(_tmp8E6,sizeof(void*),1)))))));}
_LL1BD:;}if(!(vd->tq).real_const){const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8EA="comprehension index variable is not declared const!",_tag_dyneither(
_tmp8EA,sizeof(char),52))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3F5=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3F5,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3F5,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3F5,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3F6=te2;void**_tmp3F7=0;struct Cyc_Absyn_Tqual*
_tmp3F8=0;union Cyc_Absyn_Constraint**_tmp3F9=0;if(topt != 0){void*_tmp3FA=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3FC;struct Cyc_Absyn_ArrayInfo _tmp3FE;void*
_tmp3FF;struct Cyc_Absyn_Tqual _tmp400;struct Cyc_Absyn_Exp*_tmp401;union Cyc_Absyn_Constraint*
_tmp402;_LL1C3: {struct Cyc_Absyn_PointerType_struct*_tmp3FB=(struct Cyc_Absyn_PointerType_struct*)
_tmp3FA;if(_tmp3FB->tag != 5)goto _LL1C5;else{_tmp3FC=_tmp3FB->f1;}}_LL1C4:{void**
_tmp8EB;_tmp3F7=((_tmp8EB=_region_malloc(_tmp3F5,sizeof(*_tmp8EB)),((_tmp8EB[0]=
_tmp3FC.elt_typ,_tmp8EB))));}{struct Cyc_Absyn_Tqual*_tmp8EC;_tmp3F8=((_tmp8EC=
_region_malloc(_tmp3F5,sizeof(*_tmp8EC)),((_tmp8EC[0]=_tmp3FC.elt_tq,_tmp8EC))));}{
union Cyc_Absyn_Constraint**_tmp8ED;_tmp3F9=((_tmp8ED=_region_malloc(_tmp3F5,
sizeof(*_tmp8ED)),((_tmp8ED[0]=(_tmp3FC.ptr_atts).zero_term,_tmp8ED))));}goto
_LL1C2;_LL1C5: {struct Cyc_Absyn_ArrayType_struct*_tmp3FD=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3FA;if(_tmp3FD->tag != 8)goto _LL1C7;else{_tmp3FE=_tmp3FD->f1;_tmp3FF=_tmp3FE.elt_type;
_tmp400=_tmp3FE.tq;_tmp401=_tmp3FE.num_elts;_tmp402=_tmp3FE.zero_term;}}_LL1C6:{
void**_tmp8EE;_tmp3F7=((_tmp8EE=_region_malloc(_tmp3F5,sizeof(*_tmp8EE)),((
_tmp8EE[0]=_tmp3FF,_tmp8EE))));}{struct Cyc_Absyn_Tqual*_tmp8EF;_tmp3F8=((_tmp8EF=
_region_malloc(_tmp3F5,sizeof(*_tmp8EF)),((_tmp8EF[0]=_tmp400,_tmp8EF))));}{
union Cyc_Absyn_Constraint**_tmp8F0;_tmp3F9=((_tmp8F0=_region_malloc(_tmp3F5,
sizeof(*_tmp8F0)),((_tmp8F0[0]=_tmp402,_tmp8F0))));}goto _LL1C2;_LL1C7:;_LL1C8:
goto _LL1C2;_LL1C2:;}{void*t=Cyc_Tcexp_tcExp(_tmp3F6,_tmp3F7,body);if(_tmp3F6->le
== 0){if(!Cyc_Tcutil_is_const_exp(_tmp3F6,bound)){const char*_tmp8F3;void*_tmp8F2;(
_tmp8F2=0,Cyc_Tcutil_terr(bound->loc,((_tmp8F3="bound is not constant",
_tag_dyneither(_tmp8F3,sizeof(char),22))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(_tmp3F6,body)){const char*_tmp8F6;void*_tmp8F5;(
_tmp8F5=0,Cyc_Tcutil_terr(bound->loc,((_tmp8F6="body is not constant",
_tag_dyneither(_tmp8F6,sizeof(char),21))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}}
if(_tmp3F9 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,*_tmp3F9)){struct Cyc_Absyn_Exp*_tmp40D=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*
_tmp8F7;_tmp40D->topt=((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7->v=Cyc_Absyn_uint_typ,
_tmp8F7))));}bound=Cyc_Absyn_add_exp(bound,_tmp40D,0);{struct Cyc_Core_Opt*
_tmp8F8;bound->topt=((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8->v=Cyc_Absyn_uint_typ,
_tmp8F8))));}*is_zero_term=1;}{void*_tmp410=Cyc_Absyn_array_typ(t,_tmp3F8 == 0?
Cyc_Absyn_empty_tqual(0):*_tmp3F8,(struct Cyc_Absyn_Exp*)bound,_tmp3F9 == 0?Cyc_Absyn_false_conref:*
_tmp3F9,0);return _tmp410;};};};};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp411;_push_handler(& _tmp411);{int _tmp413=0;if(setjmp(
_tmp411.handler))_tmp413=1;if(!_tmp413){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp412=(void*)_exn_thrown;void*_tmp415=_tmp412;_LL1CA: {
struct Cyc_Dict_Absent_struct*_tmp416=(struct Cyc_Dict_Absent_struct*)_tmp415;if(
_tmp416->tag != Cyc_Dict_Absent)goto _LL1CC;}_LL1CB:{const char*_tmp8FC;void*
_tmp8FB[1];struct Cyc_String_pa_struct _tmp8FA;(_tmp8FA.tag=0,((_tmp8FA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8FB[0]=&
_tmp8FA,Cyc_Tcutil_terr(loc,((_tmp8FC="unbound struct/union name %s",
_tag_dyneither(_tmp8FC,sizeof(char),29))),_tag_dyneither(_tmp8FB,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1CC:;_LL1CD:(void)_throw(
_tmp415);_LL1C9:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp904;void*_tmp903[1];const char*_tmp902;const char*_tmp901;
struct Cyc_String_pa_struct _tmp900;(_tmp900.tag=0,((_tmp900.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp901="struct",_tag_dyneither(
_tmp901,sizeof(char),7)):((_tmp902="union",_tag_dyneither(_tmp902,sizeof(char),6)))),((
_tmp903[0]=& _tmp900,Cyc_Tcutil_terr(loc,((_tmp904="can't construct abstract %s",
_tag_dyneither(_tmp904,sizeof(char),28))),_tag_dyneither(_tmp903,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp41F=Cyc_Tcenv_get_fnrgn(te);struct _tuple12 _tmp905;struct
_tuple12 _tmp420=(_tmp905.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp905.f2=_tmp41F,
_tmp905)));struct Cyc_List_List*_tmp421=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct
_tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41F,Cyc_Tcutil_r_make_inst_var,&
_tmp420,ad->tvs);struct Cyc_List_List*_tmp422=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct
_tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41F,Cyc_Tcutil_r_make_inst_var,&
_tmp420,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp423=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,
_tmp421);struct Cyc_List_List*_tmp424=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,
_tmp422);struct Cyc_List_List*_tmp425=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp41F,_tmp421,_tmp422);struct Cyc_Absyn_AggrType_struct _tmp90F;struct Cyc_Absyn_Aggrdecl**
_tmp90E;struct Cyc_Absyn_AggrInfo _tmp90D;struct Cyc_Absyn_AggrType_struct*_tmp90C;
struct Cyc_Absyn_AggrType_struct*_tmp426=(_tmp90C=_cycalloc(sizeof(*_tmp90C)),((
_tmp90C[0]=((_tmp90F.tag=11,((_tmp90F.f1=((_tmp90D.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=ad,_tmp90E))))),((_tmp90D.targs=
_tmp423,_tmp90D)))),_tmp90F)))),_tmp90C)));struct Cyc_List_List*_tmp427=*ts;
struct Cyc_List_List*_tmp428=_tmp424;while(_tmp427 != 0  && _tmp428 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp427->hd);Cyc_Tcutil_check_no_qual(
loc,(void*)_tmp427->hd);Cyc_Tcutil_unify((void*)_tmp427->hd,(void*)_tmp428->hd);
_tmp427=_tmp427->tl;_tmp428=_tmp428->tl;}if(_tmp427 != 0){const char*_tmp912;void*
_tmp911;(_tmp911=0,Cyc_Tcutil_terr(loc,((_tmp912="too many explicit witness types",
_tag_dyneither(_tmp912,sizeof(char),32))),_tag_dyneither(_tmp911,sizeof(void*),0)));}*
ts=_tmp424;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp426,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp41F,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields);for(0;fields != 0;fields=fields->tl){int bogus=0;
struct _tuple20 _tmp42C;struct Cyc_Absyn_Aggrfield*_tmp42D;struct Cyc_Absyn_Exp*
_tmp42E;struct _tuple20*_tmp42B=(struct _tuple20*)fields->hd;_tmp42C=*_tmp42B;
_tmp42D=_tmp42C.f1;_tmp42E=_tmp42C.f2;{void*_tmp42F=Cyc_Tcutil_rsubstitute(
_tmp41F,_tmp425,_tmp42D->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp42F,
_tmp42E);if(!Cyc_Tcutil_coerce_arg(te,_tmp42E,_tmp42F,& bogus)){{const char*
_tmp91E;void*_tmp91D[5];struct Cyc_String_pa_struct _tmp91C;const char*_tmp91B;
const char*_tmp91A;struct Cyc_String_pa_struct _tmp919;struct Cyc_String_pa_struct
_tmp918;struct Cyc_String_pa_struct _tmp917;struct Cyc_String_pa_struct _tmp916;(
_tmp916.tag=0,((_tmp916.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp42E->topt))->v)),((_tmp917.tag=0,((
_tmp917.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp42F)),((_tmp918.tag=0,((_tmp918.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp919.tag=0,((_tmp919.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp91A="struct",
_tag_dyneither(_tmp91A,sizeof(char),7)):((_tmp91B="union",_tag_dyneither(_tmp91B,
sizeof(char),6)))),((_tmp91C.tag=0,((_tmp91C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp42D->name),((_tmp91D[0]=& _tmp91C,((_tmp91D[1]=& _tmp919,((
_tmp91D[2]=& _tmp918,((_tmp91D[3]=& _tmp917,((_tmp91D[4]=& _tmp916,Cyc_Tcutil_terr(
_tmp42E->loc,((_tmp91E="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp91E,sizeof(char),40))),_tag_dyneither(_tmp91D,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp439=0;{struct Cyc_List_List*
_tmp43A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp43A != 0;_tmp43A=_tmp43A->tl){struct _tuple0*_tmp921;struct Cyc_List_List*
_tmp920;_tmp439=((_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920->hd=((_tmp921=
_cycalloc(sizeof(*_tmp921)),((_tmp921->f1=Cyc_Tcutil_rsubstitute(_tmp41F,_tmp425,(*((
struct _tuple0*)_tmp43A->hd)).f1),((_tmp921->f2=Cyc_Tcutil_rsubstitute(_tmp41F,
_tmp425,(*((struct _tuple0*)_tmp43A->hd)).f2),_tmp921)))))),((_tmp920->tl=_tmp439,
_tmp920))))));}}_tmp439=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp439);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp439);return(void*)_tmp426;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp442=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp443=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp445;struct
Cyc_List_List*_tmp446;_LL1CF: {struct Cyc_Absyn_AnonAggrType_struct*_tmp444=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp443;if(_tmp444->tag != 12)goto _LL1D1;
else{_tmp445=_tmp444->f1;_tmp446=_tmp444->f2;}}_LL1D0: if(_tmp445 == Cyc_Absyn_UnionA){
const char*_tmp924;void*_tmp923;(_tmp923=0,Cyc_Tcutil_terr(loc,((_tmp924="expecting struct but found union",
_tag_dyneither(_tmp924,sizeof(char),33))),_tag_dyneither(_tmp923,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp442,
loc,args,Cyc_Absyn_StructA,_tmp446);for(0;fields != 0;fields=fields->tl){int bogus=
0;struct _tuple20 _tmp44A;struct Cyc_Absyn_Aggrfield*_tmp44B;struct Cyc_Absyn_Exp*
_tmp44C;struct _tuple20*_tmp449=(struct _tuple20*)fields->hd;_tmp44A=*_tmp449;
_tmp44B=_tmp44A.f1;_tmp44C=_tmp44A.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp44B->type,_tmp44C);if(!Cyc_Tcutil_coerce_arg(te,_tmp44C,_tmp44B->type,& bogus)){{
const char*_tmp92A;void*_tmp929[3];struct Cyc_String_pa_struct _tmp928;struct Cyc_String_pa_struct
_tmp927;struct Cyc_String_pa_struct _tmp926;(_tmp926.tag=0,((_tmp926.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp44C->topt))->v)),((_tmp927.tag=0,((_tmp927.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp44B->type)),((
_tmp928.tag=0,((_tmp928.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp44B->name),((
_tmp929[0]=& _tmp928,((_tmp929[1]=& _tmp927,((_tmp929[2]=& _tmp926,Cyc_Tcutil_terr(
_tmp44C->loc,((_tmp92A="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp92A,sizeof(char),41))),_tag_dyneither(_tmp929,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CE;};_LL1D1:;_LL1D2: {const char*_tmp92D;
void*_tmp92C;(_tmp92C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp92D="tcAnonStruct: wrong type",_tag_dyneither(_tmp92D,
sizeof(char),25))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}_LL1CE:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp454=Cyc_Tcenv_get_fnrgn(
te);struct _tuple12 _tmp92E;struct _tuple12 _tmp455=(_tmp92E.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp92E.f2=_tmp454,_tmp92E)));struct Cyc_List_List*_tmp456=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),
struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp454,Cyc_Tcutil_r_make_inst_var,&
_tmp455,tud->tvs);struct Cyc_List_List*_tmp457=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))
Cyc_Core_snd,_tmp456);struct Cyc_Absyn_DatatypeFieldType_struct _tmp934;struct Cyc_Absyn_DatatypeFieldInfo
_tmp933;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp932;void*res=(void*)((
_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932[0]=((_tmp934.tag=4,((_tmp934.f1=((
_tmp933.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp933.targs=_tmp457,
_tmp933)))),_tmp934)))),_tmp932))));if(topt != 0){void*_tmp458=Cyc_Tcutil_compress(*
topt);_LL1D4: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp459=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp458;if(_tmp459->tag != 4)goto _LL1D6;}_LL1D5: Cyc_Tcutil_unify(*topt,res);goto
_LL1D3;_LL1D6:;_LL1D7: goto _LL1D3;_LL1D3:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp454,_tmp456,(*((
struct _tuple19*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp93C;void*_tmp93B[3];struct Cyc_String_pa_struct
_tmp93A;struct Cyc_String_pa_struct _tmp939;const char*_tmp938;struct Cyc_String_pa_struct
_tmp937;(_tmp937.tag=0,((_tmp937.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp938="?",_tag_dyneither(_tmp938,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp939.tag=0,((_tmp939.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp93A.tag=0,((_tmp93A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp93B[0]=& _tmp93A,((_tmp93B[1]=& _tmp939,((
_tmp93B[2]=& _tmp937,Cyc_Tcutil_terr(e->loc,((_tmp93C="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp93C,sizeof(char),82))),_tag_dyneither(_tmp93B,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp940;void*_tmp93F[1];
struct Cyc_String_pa_struct _tmp93E;return(_tmp93E.tag=0,((_tmp93E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp93F[0]=& _tmp93E,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp940="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp940,sizeof(char),47))),_tag_dyneither(_tmp93F,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp944;void*_tmp943[1];struct Cyc_String_pa_struct _tmp942;return(
_tmp942.tag=0,((_tmp942.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp943[0]=& _tmp942,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp944="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp944,sizeof(char),46))),_tag_dyneither(_tmp943,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp46A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp471;struct Cyc_Absyn_PtrAtts _tmp472;union Cyc_Absyn_Constraint*
_tmp473;struct Cyc_Absyn_ArrayInfo _tmp477;void*_tmp478;struct Cyc_List_List*
_tmp47B;struct Cyc_Absyn_AggrInfo _tmp47D;union Cyc_Absyn_AggrInfoU _tmp47E;struct
Cyc_List_List*_tmp47F;struct Cyc_List_List*_tmp483;_LL1D9: {struct Cyc_Absyn_VoidType_struct*
_tmp46B=(struct Cyc_Absyn_VoidType_struct*)_tmp46A;if(_tmp46B->tag != 0)goto _LL1DB;}
_LL1DA: return 1;_LL1DB: {struct Cyc_Absyn_Evar_struct*_tmp46C=(struct Cyc_Absyn_Evar_struct*)
_tmp46A;if(_tmp46C->tag != 1)goto _LL1DD;}_LL1DC: goto _LL1DE;_LL1DD: {struct Cyc_Absyn_VarType_struct*
_tmp46D=(struct Cyc_Absyn_VarType_struct*)_tmp46A;if(_tmp46D->tag != 2)goto _LL1DF;}
_LL1DE: goto _LL1E0;_LL1DF: {struct Cyc_Absyn_DatatypeType_struct*_tmp46E=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp46A;if(_tmp46E->tag != 3)goto _LL1E1;}_LL1E0:
goto _LL1E2;_LL1E1: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp46F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp46A;if(_tmp46F->tag != 4)goto _LL1E3;}_LL1E2: return 0;_LL1E3: {struct Cyc_Absyn_PointerType_struct*
_tmp470=(struct Cyc_Absyn_PointerType_struct*)_tmp46A;if(_tmp470->tag != 5)goto
_LL1E5;else{_tmp471=_tmp470->f1;_tmp472=_tmp471.ptr_atts;_tmp473=_tmp472.nullable;}}
_LL1E4: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp473);_LL1E5: {struct Cyc_Absyn_IntType_struct*_tmp474=(struct Cyc_Absyn_IntType_struct*)
_tmp46A;if(_tmp474->tag != 6)goto _LL1E7;}_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_Absyn_FloatType_struct*
_tmp475=(struct Cyc_Absyn_FloatType_struct*)_tmp46A;if(_tmp475->tag != 7)goto
_LL1E9;}_LL1E8: return 1;_LL1E9: {struct Cyc_Absyn_ArrayType_struct*_tmp476=(struct
Cyc_Absyn_ArrayType_struct*)_tmp46A;if(_tmp476->tag != 8)goto _LL1EB;else{_tmp477=
_tmp476->f1;_tmp478=_tmp477.elt_type;}}_LL1EA: return Cyc_Tcexp_zeroable_type(
_tmp478);_LL1EB: {struct Cyc_Absyn_FnType_struct*_tmp479=(struct Cyc_Absyn_FnType_struct*)
_tmp46A;if(_tmp479->tag != 9)goto _LL1ED;}_LL1EC: return 0;_LL1ED: {struct Cyc_Absyn_TupleType_struct*
_tmp47A=(struct Cyc_Absyn_TupleType_struct*)_tmp46A;if(_tmp47A->tag != 10)goto
_LL1EF;else{_tmp47B=_tmp47A->f1;}}_LL1EE: for(0;(unsigned int)_tmp47B;_tmp47B=
_tmp47B->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp47B->hd)).f2))
return 0;}return 1;_LL1EF: {struct Cyc_Absyn_AggrType_struct*_tmp47C=(struct Cyc_Absyn_AggrType_struct*)
_tmp46A;if(_tmp47C->tag != 11)goto _LL1F1;else{_tmp47D=_tmp47C->f1;_tmp47E=_tmp47D.aggr_info;
_tmp47F=_tmp47D.targs;}}_LL1F0: {struct Cyc_Absyn_Aggrdecl*_tmp490=Cyc_Absyn_get_known_aggrdecl(
_tmp47E);if(_tmp490->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp490->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp491=_new_region("r");
struct _RegionHandle*r=& _tmp491;_push_region(r);{struct Cyc_List_List*_tmp492=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp490->tvs,_tmp47F);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp490->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp492,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp493=0;_npop_handler(0);return
_tmp493;}}}{int _tmp494=1;_npop_handler(0);return _tmp494;};};_pop_region(r);};}
_LL1F1: {struct Cyc_Absyn_EnumType_struct*_tmp480=(struct Cyc_Absyn_EnumType_struct*)
_tmp46A;if(_tmp480->tag != 13)goto _LL1F3;}_LL1F2: return 1;_LL1F3: {struct Cyc_Absyn_TagType_struct*
_tmp481=(struct Cyc_Absyn_TagType_struct*)_tmp46A;if(_tmp481->tag != 19)goto _LL1F5;}
_LL1F4: return 1;_LL1F5: {struct Cyc_Absyn_AnonAggrType_struct*_tmp482=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp46A;if(_tmp482->tag != 12)goto _LL1F7;else{_tmp483=_tmp482->f2;}}_LL1F6: for(0;
_tmp483 != 0;_tmp483=_tmp483->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp483->hd)->type))return 0;}return 1;_LL1F7: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp484=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp46A;if(_tmp484->tag != 14)goto
_LL1F9;}_LL1F8: return 1;_LL1F9: {struct Cyc_Absyn_TypedefType_struct*_tmp485=(
struct Cyc_Absyn_TypedefType_struct*)_tmp46A;if(_tmp485->tag != 17)goto _LL1FB;}
_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_DynRgnType_struct*_tmp486=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp46A;if(_tmp486->tag != 16)goto _LL1FD;}_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp487=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp46A;if(_tmp487->tag != 15)goto
_LL1FF;}_LL1FE: return 0;_LL1FF: {struct Cyc_Absyn_TypeDeclType_struct*_tmp488=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp46A;if(_tmp488->tag != 26)goto _LL201;}
_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_ValueofType_struct*_tmp489=(struct Cyc_Absyn_ValueofType_struct*)
_tmp46A;if(_tmp489->tag != 18)goto _LL203;}_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_HeapRgn_struct*
_tmp48A=(struct Cyc_Absyn_HeapRgn_struct*)_tmp46A;if(_tmp48A->tag != 20)goto _LL205;}
_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_UniqueRgn_struct*_tmp48B=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp46A;if(_tmp48B->tag != 21)goto _LL207;}_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp48C=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp46A;if(_tmp48C->tag != 22)goto
_LL209;}_LL208: goto _LL20A;_LL209: {struct Cyc_Absyn_AccessEff_struct*_tmp48D=(
struct Cyc_Absyn_AccessEff_struct*)_tmp46A;if(_tmp48D->tag != 23)goto _LL20B;}
_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_JoinEff_struct*_tmp48E=(struct Cyc_Absyn_JoinEff_struct*)
_tmp46A;if(_tmp48E->tag != 24)goto _LL20D;}_LL20C: goto _LL20E;_LL20D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp48F=(struct Cyc_Absyn_RgnsEff_struct*)_tmp46A;if(_tmp48F->tag != 25)goto _LL1D8;}
_LL20E: {const char*_tmp948;void*_tmp947[1];struct Cyc_String_pa_struct _tmp946;(
_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp947[0]=& _tmp946,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp948="bad type `%s' in zeroable type",_tag_dyneither(
_tmp948,sizeof(char),31))),_tag_dyneither(_tmp947,sizeof(void*),1)))))));}_LL1D8:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp498=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp49A;void*_tmp49B;_LL210: {struct Cyc_Absyn_PointerType_struct*_tmp499=(struct
Cyc_Absyn_PointerType_struct*)_tmp498;if(_tmp499->tag != 5)goto _LL212;else{
_tmp49A=_tmp499->f1;_tmp49B=_tmp49A.elt_typ;}}_LL211: Cyc_Tcutil_unify(_tmp49B,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20F;_LL212:;_LL213: goto _LL20F;_LL20F:;}{const char*_tmp951;void*_tmp950[2];
const char*_tmp94F;const char*_tmp94E;struct Cyc_String_pa_struct _tmp94D;struct Cyc_String_pa_struct
_tmp94C;(_tmp94C.tag=0,((_tmp94C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp94D.tag=0,((_tmp94D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp94E="calloc",
_tag_dyneither(_tmp94E,sizeof(char),7))):(struct _dyneither_ptr)((_tmp94F="malloc",
_tag_dyneither(_tmp94F,sizeof(char),7))))),((_tmp950[0]=& _tmp94D,((_tmp950[1]=&
_tmp94C,Cyc_Tcutil_terr(loc,((_tmp951="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp951,sizeof(char),60))),_tag_dyneither(_tmp950,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL214: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL215: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp954;
struct Cyc_Absyn_RgnHandleType_struct*_tmp953;void*expected_type=(void*)((_tmp953=
_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=15,((_tmp954.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp954)))),_tmp953))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));void*_tmp4A2=Cyc_Tcutil_compress(
handle_type);void*_tmp4A4;_LL218: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4A3=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4A2;if(_tmp4A3->tag != 15)goto _LL21A;
else{_tmp4A4=(void*)_tmp4A3->f1;}}_LL219: rgn=_tmp4A4;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL217;_LL21A:;_LL21B:{const char*_tmp958;void*_tmp957[1];struct
Cyc_String_pa_struct _tmp956;(_tmp956.tag=0,((_tmp956.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp957[0]=& _tmp956,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp958="expecting region_t type but found %s",
_tag_dyneither(_tmp958,sizeof(char),37))),_tag_dyneither(_tmp957,sizeof(void*),1)))))));}
goto _LL217;_LL217:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp95B;void*_tmp95A;(
_tmp95A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp95B="calloc with empty type",_tag_dyneither(_tmp95B,sizeof(char),23))),
_tag_dyneither(_tmp95A,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcutil_check_no_qual(loc,elt_type);Cyc_Tcexp_check_malloc_type(1,
loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*er=(*e)->r;retry_sizeof: {
void*_tmp4AC=er;void*_tmp4AE;enum Cyc_Absyn_Primop _tmp4B0;struct Cyc_List_List*
_tmp4B1;struct Cyc_List_List _tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_List_List*
_tmp4B4;struct Cyc_List_List _tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_List_List*
_tmp4B7;_LL21D: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4AD=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AC;if(_tmp4AD->tag != 18)goto _LL21F;else{_tmp4AE=(void*)_tmp4AD->f1;}}_LL21E:
elt_type=_tmp4AE;{void**_tmp95C;*t=(void**)((_tmp95C=_cycalloc(sizeof(*_tmp95C)),((
_tmp95C[0]=elt_type,_tmp95C))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL21C;_LL21F: {struct Cyc_Absyn_Primop_e_struct*
_tmp4AF=(struct Cyc_Absyn_Primop_e_struct*)_tmp4AC;if(_tmp4AF->tag != 3)goto _LL221;
else{_tmp4B0=_tmp4AF->f1;if(_tmp4B0 != Cyc_Absyn_Times)goto _LL221;_tmp4B1=_tmp4AF->f2;
if(_tmp4B1 == 0)goto _LL221;_tmp4B2=*_tmp4B1;_tmp4B3=(struct Cyc_Absyn_Exp*)_tmp4B2.hd;
_tmp4B4=_tmp4B2.tl;if(_tmp4B4 == 0)goto _LL221;_tmp4B5=*_tmp4B4;_tmp4B6=(struct Cyc_Absyn_Exp*)
_tmp4B5.hd;_tmp4B7=_tmp4B5.tl;if(_tmp4B7 != 0)goto _LL221;}}_LL220:{struct _tuple0
_tmp95D;struct _tuple0 _tmp4BA=(_tmp95D.f1=_tmp4B3->r,((_tmp95D.f2=_tmp4B6->r,
_tmp95D)));void*_tmp4BB;void*_tmp4BD;void*_tmp4BE;void*_tmp4C0;_LL224: _tmp4BB=
_tmp4BA.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4BC=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4BB;if(_tmp4BC->tag != 18)goto _LL226;else{_tmp4BD=(void*)_tmp4BC->f1;}};
_LL225: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4BD);elt_type=_tmp4BD;{void**
_tmp95E;*t=(void**)((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E[0]=elt_type,
_tmp95E))));}num_elts=_tmp4B6;one_elt=0;goto _LL223;_LL226: _tmp4BE=_tmp4BA.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4BF=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4BE;if(_tmp4BF->tag != 18)goto _LL228;else{_tmp4C0=(void*)_tmp4BF->f1;}};
_LL227: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C0);elt_type=_tmp4C0;{void**
_tmp95F;*t=(void**)((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F[0]=elt_type,
_tmp95F))));}num_elts=_tmp4B3;one_elt=0;goto _LL223;_LL228:;_LL229: goto No_sizeof;
_LL223:;}goto _LL21C;_LL221:;_LL222: No_sizeof: {struct Cyc_Absyn_Exp*_tmp4C3=*e;{
void*_tmp4C4=_tmp4C3->r;struct Cyc_Absyn_Exp*_tmp4C6;_LL22B: {struct Cyc_Absyn_Cast_e_struct*
_tmp4C5=(struct Cyc_Absyn_Cast_e_struct*)_tmp4C4;if(_tmp4C5->tag != 15)goto _LL22D;
else{_tmp4C6=_tmp4C5->f2;}}_LL22C: _tmp4C3=_tmp4C6;goto _LL22A;_LL22D:;_LL22E: goto
_LL22A;_LL22A:;}{void*_tmp4C7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C3->topt))->v);void*_tmp4C9;_LL230: {struct Cyc_Absyn_TagType_struct*
_tmp4C8=(struct Cyc_Absyn_TagType_struct*)_tmp4C7;if(_tmp4C8->tag != 19)goto _LL232;
else{_tmp4C9=(void*)_tmp4C8->f1;}}_LL231:{void*_tmp4CA=Cyc_Tcutil_compress(
_tmp4C9);struct Cyc_Absyn_Exp*_tmp4CC;_LL235: {struct Cyc_Absyn_ValueofType_struct*
_tmp4CB=(struct Cyc_Absyn_ValueofType_struct*)_tmp4CA;if(_tmp4CB->tag != 18)goto
_LL237;else{_tmp4CC=_tmp4CB->f1;}}_LL236: er=_tmp4CC->r;goto retry_sizeof;_LL237:;
_LL238: goto _LL234;_LL234:;}goto _LL22F;_LL232:;_LL233: goto _LL22F;_LL22F:;}
elt_type=Cyc_Absyn_char_typ;{void**_tmp960;*t=(void**)((_tmp960=_cycalloc(
sizeof(*_tmp960)),((_tmp960[0]=elt_type,_tmp960))));}num_elts=*e;one_elt=0;}goto
_LL21C;_LL21C:;}}*is_fat=!one_elt;{void*_tmp4CE=elt_type;struct Cyc_Absyn_AggrInfo
_tmp4D0;union Cyc_Absyn_AggrInfoU _tmp4D1;struct Cyc_Absyn_Aggrdecl**_tmp4D2;struct
Cyc_Absyn_Aggrdecl*_tmp4D3;_LL23A: {struct Cyc_Absyn_AggrType_struct*_tmp4CF=(
struct Cyc_Absyn_AggrType_struct*)_tmp4CE;if(_tmp4CF->tag != 11)goto _LL23C;else{
_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4D0.aggr_info;if((_tmp4D1.KnownAggr).tag != 2)
goto _LL23C;_tmp4D2=(struct Cyc_Absyn_Aggrdecl**)(_tmp4D1.KnownAggr).val;_tmp4D3=*
_tmp4D2;}}_LL23B: if(_tmp4D3->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4D3->impl))->exist_vars != 0){const char*_tmp963;void*_tmp962;(
_tmp962=0,Cyc_Tcutil_terr(loc,((_tmp963="malloc with existential types not yet implemented",
_tag_dyneither(_tmp963,sizeof(char),50))),_tag_dyneither(_tmp962,sizeof(void*),0)));}
goto _LL239;_LL23C:;_LL23D: goto _LL239;_LL239:;}{void*(*_tmp4D6)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4D7=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp4D8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4DA;struct Cyc_Absyn_PtrAtts
_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DC;union Cyc_Absyn_Constraint*_tmp4DD;
union Cyc_Absyn_Constraint*_tmp4DE;_LL23F: {struct Cyc_Absyn_PointerType_struct*
_tmp4D9=(struct Cyc_Absyn_PointerType_struct*)_tmp4D8;if(_tmp4D9->tag != 5)goto
_LL241;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4DA.ptr_atts;_tmp4DC=_tmp4DB.nullable;
_tmp4DD=_tmp4DB.bounds;_tmp4DE=_tmp4DB.zero_term;}}_LL240: _tmp4D7=_tmp4DE;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DC))
_tmp4D6=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp4DE) && !(*is_calloc)){{const char*_tmp966;void*_tmp965;(_tmp965=0,Cyc_Tcutil_warn(
loc,((_tmp966="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp966,sizeof(char),55))),_tag_dyneither(_tmp965,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp4E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4DD);struct Cyc_Absyn_Exp*_tmp4E4;_LL244: {struct Cyc_Absyn_DynEither_b_struct*
_tmp4E2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp4E1;if(_tmp4E2->tag != 0)goto
_LL246;}_LL245: goto _LL243;_LL246:{struct Cyc_Absyn_Upper_b_struct*_tmp4E3=(struct
Cyc_Absyn_Upper_b_struct*)_tmp4E1;if(_tmp4E3->tag != 1)goto _LL248;else{_tmp4E4=
_tmp4E3->f1;}}if(!(!one_elt))goto _LL248;_LL247: {int _tmp4E5=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4E5  && Cyc_Evexp_same_const_exp(_tmp4E4,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DD),_tmp4D7);}{void*_tmp4E6=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4E8;_LL24B: {struct Cyc_Absyn_TagType_struct*_tmp4E7=(struct Cyc_Absyn_TagType_struct*)
_tmp4E6;if(_tmp4E7->tag != 19)goto _LL24D;else{_tmp4E8=(void*)_tmp4E7->f1;}}_LL24C: {
struct Cyc_Absyn_Exp*_tmp4E9=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4E8,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4E9,
_tmp4E4)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DD),_tmp4D7);}
goto _LL24A;}_LL24D:;_LL24E: goto _LL24A;_LL24A:;}goto _LL243;}_LL248:;_LL249: goto
_LL243;_LL243:;}goto _LL23E;_LL241:;_LL242: goto _LL23E;_LL23E:;}if(!one_elt)
_tmp4D6=Cyc_Absyn_dyneither_typ;return _tmp4D6(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp4D7);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4EA=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4EB=Cyc_Tcenv_enter_lhs(
_tmp4EA,Cyc_Tcenv_enter_notreadctxt(_tmp4EA,te));Cyc_Tcexp_tcExp(_tmp4EB,0,e1);
Cyc_Tcexp_tcExp(_tmp4EB,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp4EC=Cyc_Tcutil_compress(t1);_LL250: {struct Cyc_Absyn_ArrayType_struct*
_tmp4ED=(struct Cyc_Absyn_ArrayType_struct*)_tmp4EC;if(_tmp4ED->tag != 8)goto
_LL252;}_LL251:{const char*_tmp969;void*_tmp968;(_tmp968=0,Cyc_Tcutil_terr(loc,((
_tmp969="cannot assign to an array",_tag_dyneither(_tmp969,sizeof(char),26))),
_tag_dyneither(_tmp968,sizeof(void*),0)));}goto _LL24F;_LL252:;_LL253: goto _LL24F;
_LL24F:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){const char*
_tmp96C;void*_tmp96B;(_tmp96B=0,Cyc_Tcutil_terr(loc,((_tmp96C="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp96C,sizeof(char),58))),_tag_dyneither(_tmp96B,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp96F;void*_tmp96E;return(_tmp96E=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp96F="swap non-lvalue",_tag_dyneither(_tmp96F,sizeof(char),
16))),_tag_dyneither(_tmp96E,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp972;void*_tmp971;return(_tmp971=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp972="swap non-lvalue",_tag_dyneither(_tmp972,sizeof(char),16))),
_tag_dyneither(_tmp971,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp975;void*_tmp974;return(_tmp974=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp975="swap value in zeroterm array",_tag_dyneither(_tmp975,
sizeof(char),29))),_tag_dyneither(_tmp974,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp978;void*_tmp977;return(_tmp977=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp978="swap value in zeroterm array",_tag_dyneither(_tmp978,
sizeof(char),29))),_tag_dyneither(_tmp977,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp97D;void*_tmp97C[2];struct Cyc_String_pa_struct _tmp97B;struct Cyc_String_pa_struct
_tmp97A;void*_tmp4FA=(_tmp97A.tag=0,((_tmp97A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp97B.tag=0,((_tmp97B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp97C[0]=&
_tmp97B,((_tmp97C[1]=& _tmp97A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp97D="type mismatch: %s != %s",
_tag_dyneither(_tmp97D,sizeof(char),24))),_tag_dyneither(_tmp97C,sizeof(void*),2)))))))))))));
return _tmp4FA;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4FF=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4FF,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp500=s->r;struct Cyc_Absyn_Exp*
_tmp502;struct Cyc_Absyn_Stmt*_tmp504;struct Cyc_Absyn_Stmt*_tmp505;struct Cyc_Absyn_Decl*
_tmp507;struct Cyc_Absyn_Stmt*_tmp508;_LL255: {struct Cyc_Absyn_Exp_s_struct*
_tmp501=(struct Cyc_Absyn_Exp_s_struct*)_tmp500;if(_tmp501->tag != 1)goto _LL257;
else{_tmp502=_tmp501->f1;}}_LL256: {void*_tmp509=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp502->topt))->v;if(!Cyc_Tcutil_unify(_tmp509,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp981;void*_tmp980[1];struct
Cyc_String_pa_struct _tmp97F;(_tmp97F.tag=0,((_tmp97F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp509)),((_tmp980[0]=& _tmp97F,Cyc_Tcutil_terr(
loc,((_tmp981="statement expression returns type %s",_tag_dyneither(_tmp981,
sizeof(char),37))),_tag_dyneither(_tmp980,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp509;}_LL257: {struct Cyc_Absyn_Seq_s_struct*_tmp503=(struct Cyc_Absyn_Seq_s_struct*)
_tmp500;if(_tmp503->tag != 2)goto _LL259;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;}}
_LL258: s=_tmp505;continue;_LL259: {struct Cyc_Absyn_Decl_s_struct*_tmp506=(struct
Cyc_Absyn_Decl_s_struct*)_tmp500;if(_tmp506->tag != 12)goto _LL25B;else{_tmp507=
_tmp506->f1;_tmp508=_tmp506->f2;}}_LL25A: s=_tmp508;continue;_LL25B:;_LL25C: {
const char*_tmp984;void*_tmp983;return(_tmp983=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp984="statement expression must end with expression",_tag_dyneither(_tmp984,
sizeof(char),46))),_tag_dyneither(_tmp983,sizeof(void*),0)));}_LL254:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp50F=t;struct Cyc_Absyn_AggrInfo _tmp511;union
Cyc_Absyn_AggrInfoU _tmp512;struct Cyc_Absyn_Aggrdecl**_tmp513;struct Cyc_Absyn_Aggrdecl*
_tmp514;_LL25E: {struct Cyc_Absyn_AggrType_struct*_tmp510=(struct Cyc_Absyn_AggrType_struct*)
_tmp50F;if(_tmp510->tag != 11)goto _LL260;else{_tmp511=_tmp510->f1;_tmp512=_tmp511.aggr_info;
if((_tmp512.KnownAggr).tag != 2)goto _LL260;_tmp513=(struct Cyc_Absyn_Aggrdecl**)(
_tmp512.KnownAggr).val;_tmp514=*_tmp513;}}_LL25F: if((_tmp514->kind == Cyc_Absyn_UnionA
 && _tmp514->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp514->impl))->tagged)
goto _LL25D;goto _LL261;_LL260:;_LL261:{const char*_tmp988;void*_tmp987[1];struct
Cyc_String_pa_struct _tmp986;(_tmp986.tag=0,((_tmp986.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp987[0]=& _tmp986,Cyc_Tcutil_terr(
loc,((_tmp988="expecting @tagged union but found %s",_tag_dyneither(_tmp988,
sizeof(char),37))),_tag_dyneither(_tmp987,sizeof(void*),1)))))));}goto _LL25D;
_LL25D:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp98B;struct Cyc_Absyn_RgnHandleType_struct*
_tmp98A;void*expected_type=(void*)((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((
_tmp98A[0]=((_tmp98B.tag=15,((_tmp98B.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp98B)))),_tmp98A))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle));void*_tmp518=Cyc_Tcutil_compress(handle_type);void*
_tmp51A;_LL263: {struct Cyc_Absyn_RgnHandleType_struct*_tmp519=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp518;if(_tmp519->tag != 15)goto _LL265;else{_tmp51A=(void*)_tmp519->f1;}}_LL264:
rgn=_tmp51A;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL262;_LL265:;_LL266:{
const char*_tmp98F;void*_tmp98E[1];struct Cyc_String_pa_struct _tmp98D;(_tmp98D.tag=
0,((_tmp98D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp98E[0]=& _tmp98D,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle))->loc,((_tmp98F="expecting region_t type but found %s",
_tag_dyneither(_tmp98F,sizeof(char),37))),_tag_dyneither(_tmp98E,sizeof(void*),1)))))));}
goto _LL262;_LL262:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp520=e1->r;struct Cyc_Core_Opt*_tmp523;struct Cyc_List_List*_tmp524;struct
Cyc_List_List*_tmp526;union Cyc_Absyn_Cnst _tmp528;struct _dyneither_ptr _tmp529;
union Cyc_Absyn_Cnst _tmp52B;struct _dyneither_ptr _tmp52C;_LL268: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp521=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp520;if(_tmp521->tag != 28)
goto _LL26A;}_LL269: {void*_tmp52D=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*
_tmp52E=Cyc_Tcutil_compress(_tmp52D);struct Cyc_Absyn_ArrayInfo _tmp530;void*
_tmp531;struct Cyc_Absyn_Tqual _tmp532;struct Cyc_Absyn_Exp*_tmp533;union Cyc_Absyn_Constraint*
_tmp534;_LL275: {struct Cyc_Absyn_ArrayType_struct*_tmp52F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp52E;if(_tmp52F->tag != 8)goto _LL277;else{_tmp530=_tmp52F->f1;_tmp531=_tmp530.elt_type;
_tmp532=_tmp530.tq;_tmp533=_tmp530.num_elts;_tmp534=_tmp530.zero_term;}}_LL276: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp533);void*b;{void*
_tmp535=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp537;_LL27A: {struct Cyc_Absyn_TagType_struct*_tmp536=(struct Cyc_Absyn_TagType_struct*)
_tmp535;if(_tmp536->tag != 19)goto _LL27C;else{_tmp537=(void*)_tmp536->f1;}}_LL27B:{
struct Cyc_Absyn_Upper_b_struct _tmp992;struct Cyc_Absyn_Upper_b_struct*_tmp991;b=(
void*)((_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991[0]=((_tmp992.tag=1,((
_tmp992.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp537,0),
0,Cyc_Absyn_No_coercion,0),_tmp992)))),_tmp991))));}goto _LL279;_LL27C:;_LL27D:
if(Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp995;struct
Cyc_Absyn_Upper_b_struct*_tmp994;b=(void*)((_tmp994=_cycalloc(sizeof(*_tmp994)),((
_tmp994[0]=((_tmp995.tag=1,((_tmp995.f1=bnd,_tmp995)))),_tmp994))));}else{b=(
void*)& Cyc_Absyn_DynEither_b_val;}_LL279:;}{struct Cyc_Absyn_PointerType_struct
_tmp99F;struct Cyc_Absyn_PtrAtts _tmp99E;struct Cyc_Absyn_PtrInfo _tmp99D;struct Cyc_Absyn_PointerType_struct*
_tmp99C;void*res_typ=(void*)((_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C[0]=((
_tmp99F.tag=5,((_tmp99F.f1=((_tmp99D.elt_typ=_tmp531,((_tmp99D.elt_tq=_tmp532,((
_tmp99D.ptr_atts=((_tmp99E.rgn=rgn,((_tmp99E.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp99E.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp99E.zero_term=_tmp534,((_tmp99E.ptrloc=0,
_tmp99E)))))))))),_tmp99D)))))),_tmp99F)))),_tmp99C))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp9A0;e->topt=((_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0->v=res_typ,
_tmp9A0))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL277:;_LL278: {const char*_tmp9A3;void*_tmp9A2;(
_tmp9A2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9A3="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9A3,
sizeof(char),45))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}_LL274:;}_LL26A: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp522=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp520;if(_tmp522->tag != 36)goto _LL26C;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}
_LL26B:{struct Cyc_Absyn_Array_e_struct _tmp9A6;struct Cyc_Absyn_Array_e_struct*
_tmp9A5;e1->r=(void*)((_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5[0]=((
_tmp9A6.tag=27,((_tmp9A6.f1=_tmp524,_tmp9A6)))),_tmp9A5))));}_tmp526=_tmp524;
goto _LL26D;_LL26C: {struct Cyc_Absyn_Array_e_struct*_tmp525=(struct Cyc_Absyn_Array_e_struct*)
_tmp520;if(_tmp525->tag != 27)goto _LL26E;else{_tmp526=_tmp525->f1;}}_LL26D: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp545=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp547;void*_tmp548;void**_tmp549;struct Cyc_Absyn_Tqual
_tmp54A;struct Cyc_Absyn_PtrAtts _tmp54B;union Cyc_Absyn_Constraint*_tmp54C;_LL27F: {
struct Cyc_Absyn_PointerType_struct*_tmp546=(struct Cyc_Absyn_PointerType_struct*)
_tmp545;if(_tmp546->tag != 5)goto _LL281;else{_tmp547=_tmp546->f1;_tmp548=_tmp547.elt_typ;
_tmp549=(void**)&(_tmp546->f1).elt_typ;_tmp54A=_tmp547.elt_tq;_tmp54B=_tmp547.ptr_atts;
_tmp54C=_tmp54B.zero_term;}}_LL280: elt_typ_opt=(void**)_tmp549;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp54C);goto _LL27E;
_LL281:;_LL282: goto _LL27E;_LL27E:;}{void*_tmp54D=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,0,zero_term,_tmp526);{struct Cyc_Core_Opt*_tmp9A7;e1->topt=((_tmp9A7=
_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7->v=_tmp54D,_tmp9A7))));}{void*res_typ;{
void*_tmp54F=Cyc_Tcutil_compress(_tmp54D);struct Cyc_Absyn_ArrayInfo _tmp551;void*
_tmp552;struct Cyc_Absyn_Tqual _tmp553;struct Cyc_Absyn_Exp*_tmp554;union Cyc_Absyn_Constraint*
_tmp555;_LL284: {struct Cyc_Absyn_ArrayType_struct*_tmp550=(struct Cyc_Absyn_ArrayType_struct*)
_tmp54F;if(_tmp550->tag != 8)goto _LL286;else{_tmp551=_tmp550->f1;_tmp552=_tmp551.elt_type;
_tmp553=_tmp551.tq;_tmp554=_tmp551.num_elts;_tmp555=_tmp551.zero_term;}}_LL285:{
struct Cyc_Absyn_PointerType_struct _tmp9BC;struct Cyc_Absyn_PtrAtts _tmp9BB;struct
Cyc_Absyn_Upper_b_struct _tmp9BA;struct Cyc_Absyn_Upper_b_struct*_tmp9B9;struct Cyc_Absyn_PtrInfo
_tmp9B8;struct Cyc_Absyn_PointerType_struct*_tmp9B7;res_typ=(void*)((_tmp9B7=
_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9BC.tag=5,((_tmp9BC.f1=((_tmp9B8.elt_typ=
_tmp552,((_tmp9B8.elt_tq=_tmp553,((_tmp9B8.ptr_atts=((_tmp9BB.rgn=rgn,((_tmp9BB.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9BB.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9B9=
_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9[0]=((_tmp9BA.tag=1,((_tmp9BA.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp554),_tmp9BA)))),_tmp9B9))))),((_tmp9BB.zero_term=_tmp555,((
_tmp9BB.ptrloc=0,_tmp9BB)))))))))),_tmp9B8)))))),_tmp9BC)))),_tmp9B7))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9BD;e->topt=((_tmp9BD=_cycalloc(
sizeof(*_tmp9BD)),((_tmp9BD->v=res_typ,_tmp9BD))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL283;_LL286:;_LL287: {
const char*_tmp9C0;void*_tmp9BF;(_tmp9BF=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C0="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp9C0,sizeof(char),50))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));}
_LL283:;}return res_typ;};};}_LL26E: {struct Cyc_Absyn_Const_e_struct*_tmp527=(
struct Cyc_Absyn_Const_e_struct*)_tmp520;if(_tmp527->tag != 0)goto _LL270;else{
_tmp528=_tmp527->f1;if((_tmp528.String_c).tag != 8)goto _LL270;_tmp529=(struct
_dyneither_ptr)(_tmp528.String_c).val;}}_LL26F: {void*_tmp55F=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp560=Cyc_Tcexp_tcExp(te,(void**)& _tmp55F,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9C3;struct Cyc_Absyn_Upper_b_struct*_tmp9C2;
return Cyc_Absyn_atb_typ(_tmp560,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9C2=
_cycalloc(sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=1,((_tmp9C3.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9C3)))),_tmp9C2)))),Cyc_Absyn_false_conref);}_LL270: {struct Cyc_Absyn_Const_e_struct*
_tmp52A=(struct Cyc_Absyn_Const_e_struct*)_tmp520;if(_tmp52A->tag != 0)goto _LL272;
else{_tmp52B=_tmp52A->f1;if((_tmp52B.Wstring_c).tag != 9)goto _LL272;_tmp52C=(
struct _dyneither_ptr)(_tmp52B.Wstring_c).val;}}_LL271: {void*_tmp563=Cyc_Absyn_atb_typ(
Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp564=Cyc_Tcexp_tcExp(te,(void**)& _tmp563,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9C6;struct Cyc_Absyn_Upper_b_struct*_tmp9C5;
return Cyc_Absyn_atb_typ(_tmp564,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9C5=
_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C6.tag=1,((_tmp9C6.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9C6)))),_tmp9C5)))),Cyc_Absyn_false_conref);}_LL272:;_LL273: {void**
topt2=0;if(topt != 0){void*_tmp567=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp569;void*_tmp56A;void**_tmp56B;struct Cyc_Absyn_Tqual _tmp56C;_LL289: {struct
Cyc_Absyn_PointerType_struct*_tmp568=(struct Cyc_Absyn_PointerType_struct*)
_tmp567;if(_tmp568->tag != 5)goto _LL28B;else{_tmp569=_tmp568->f1;_tmp56A=_tmp569.elt_typ;
_tmp56B=(void**)&(_tmp568->f1).elt_typ;_tmp56C=_tmp569.elt_tq;}}_LL28A: topt2=(
void**)_tmp56B;goto _LL288;_LL28B: {struct Cyc_Absyn_DatatypeType_struct*_tmp56D=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp567;if(_tmp56D->tag != 3)goto _LL28D;}
_LL28C:{void**_tmp9C7;topt2=((_tmp9C7=_cycalloc(sizeof(*_tmp9C7)),((_tmp9C7[0]=*
topt,_tmp9C7))));}goto _LL288;_LL28D:;_LL28E: goto _LL288;_LL288:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp9D1;struct Cyc_Absyn_PtrAtts
_tmp9D0;struct Cyc_Absyn_PtrInfo _tmp9CF;struct Cyc_Absyn_PointerType_struct*
_tmp9CE;void*res_typ=(void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((
_tmp9D1.tag=5,((_tmp9D1.f1=((_tmp9CF.elt_typ=telt,((_tmp9CF.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp9CF.ptr_atts=((_tmp9D0.rgn=rgn,((_tmp9D0.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9D0.bounds=Cyc_Absyn_bounds_one_conref,((_tmp9D0.zero_term=
Cyc_Absyn_false_conref,((_tmp9D0.ptrloc=0,_tmp9D0)))))))))),_tmp9CF)))))),
_tmp9D1)))),_tmp9CE))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9D2;e->topt=((_tmp9D2=_cycalloc(
sizeof(*_tmp9D2)),((_tmp9D2->v=res_typ,_tmp9D2))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL267:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp574=t;
struct Cyc_Absyn_ArrayInfo _tmp576;void*_tmp577;struct Cyc_Absyn_Tqual _tmp578;
struct Cyc_Absyn_Exp*_tmp579;union Cyc_Absyn_Constraint*_tmp57A;_LL290: {struct Cyc_Absyn_ArrayType_struct*
_tmp575=(struct Cyc_Absyn_ArrayType_struct*)_tmp574;if(_tmp575->tag != 8)goto
_LL292;else{_tmp576=_tmp575->f1;_tmp577=_tmp576.elt_type;_tmp578=_tmp576.tq;
_tmp579=_tmp576.num_elts;_tmp57A=_tmp576.zero_term;}}_LL291: {void*_tmp57C;
struct _tuple14 _tmp57B=Cyc_Tcutil_addressof_props(te,e);_tmp57C=_tmp57B.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp9D5;struct Cyc_Absyn_Upper_b_struct*_tmp9D4;
void*_tmp57D=_tmp579 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9D4=
_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=1,((_tmp9D5.f1=(struct Cyc_Absyn_Exp*)
_tmp579,_tmp9D5)))),_tmp9D4))));t=Cyc_Absyn_atb_typ(_tmp577,_tmp57C,_tmp578,
_tmp57D,_tmp57A);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL292:;_LL293: return t;_LL28F:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp580=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp580,t) && !Cyc_Tcutil_is_noalias_path(_tmp580,e)){const char*_tmp9D8;void*
_tmp9D7;(_tmp9D7=0,Cyc_Tcutil_terr(e->loc,((_tmp9D8="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp9D8,sizeof(char),49))),_tag_dyneither(_tmp9D7,sizeof(void*),0)));}{
void*_tmp583=e->r;union Cyc_Absyn_Cnst _tmp587;struct _dyneither_ptr _tmp588;union
Cyc_Absyn_Cnst _tmp58A;struct _dyneither_ptr _tmp58B;_LL295: {struct Cyc_Absyn_Array_e_struct*
_tmp584=(struct Cyc_Absyn_Array_e_struct*)_tmp583;if(_tmp584->tag != 27)goto _LL297;}
_LL296: goto _LL298;_LL297: {struct Cyc_Absyn_Comprehension_e_struct*_tmp585=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp583;if(_tmp585->tag != 28)goto _LL299;}
_LL298: goto _LL29A;_LL299: {struct Cyc_Absyn_Const_e_struct*_tmp586=(struct Cyc_Absyn_Const_e_struct*)
_tmp583;if(_tmp586->tag != 0)goto _LL29B;else{_tmp587=_tmp586->f1;if((_tmp587.Wstring_c).tag
!= 9)goto _LL29B;_tmp588=(struct _dyneither_ptr)(_tmp587.Wstring_c).val;}}_LL29A:
goto _LL29C;_LL29B: {struct Cyc_Absyn_Const_e_struct*_tmp589=(struct Cyc_Absyn_Const_e_struct*)
_tmp583;if(_tmp589->tag != 0)goto _LL29D;else{_tmp58A=_tmp589->f1;if((_tmp58A.String_c).tag
!= 8)goto _LL29D;_tmp58B=(struct _dyneither_ptr)(_tmp58A.String_c).val;}}_LL29C:
return t;_LL29D:;_LL29E: t=Cyc_Tcutil_compress(t);{void*_tmp58C=t;struct Cyc_Absyn_ArrayInfo
_tmp58E;void*_tmp58F;struct Cyc_Absyn_Tqual _tmp590;struct Cyc_Absyn_Exp*_tmp591;
union Cyc_Absyn_Constraint*_tmp592;_LL2A0: {struct Cyc_Absyn_ArrayType_struct*
_tmp58D=(struct Cyc_Absyn_ArrayType_struct*)_tmp58C;if(_tmp58D->tag != 8)goto
_LL2A2;else{_tmp58E=_tmp58D->f1;_tmp58F=_tmp58E.elt_type;_tmp590=_tmp58E.tq;
_tmp591=_tmp58E.num_elts;_tmp592=_tmp58E.zero_term;}}_LL2A1: {void*_tmp594;
struct _tuple14 _tmp593=Cyc_Tcutil_addressof_props(te,e);_tmp594=_tmp593.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp9DB;struct Cyc_Absyn_Upper_b_struct*_tmp9DA;
void*b=_tmp591 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9DA=_cycalloc(
sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=1,((_tmp9DB.f1=(struct Cyc_Absyn_Exp*)
_tmp591,_tmp9DB)))),_tmp9DA))));t=Cyc_Absyn_atb_typ(_tmp58F,_tmp594,_tmp590,b,
_tmp592);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL2A2:;_LL2A3: return t;_LL29F:;};_LL294:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp597=e->r;
struct Cyc_Absyn_Exp*_tmp599;_LL2A5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp598=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp597;if(_tmp598->tag != 13)
goto _LL2A7;else{_tmp599=_tmp598->f1;}}_LL2A6: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp599);((struct Cyc_Core_Opt*)_check_null(_tmp599->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp599->topt))->v,0);e->topt=_tmp599->topt;
goto _LL2A4;_LL2A7:;_LL2A8: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp59A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp59C;void*_tmp59D;struct Cyc_Absyn_Tqual
_tmp59E;struct Cyc_Absyn_PtrAtts _tmp59F;void*_tmp5A0;union Cyc_Absyn_Constraint*
_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A2;union Cyc_Absyn_Constraint*_tmp5A3;
_LL2AA: {struct Cyc_Absyn_PointerType_struct*_tmp59B=(struct Cyc_Absyn_PointerType_struct*)
_tmp59A;if(_tmp59B->tag != 5)goto _LL2AC;else{_tmp59C=_tmp59B->f1;_tmp59D=_tmp59C.elt_typ;
_tmp59E=_tmp59C.elt_tq;_tmp59F=_tmp59C.ptr_atts;_tmp5A0=_tmp59F.rgn;_tmp5A1=
_tmp59F.nullable;_tmp5A2=_tmp59F.bounds;_tmp5A3=_tmp59F.zero_term;}}_LL2AB:{void*
_tmp5A4=Cyc_Tcutil_compress(_tmp59D);struct Cyc_Absyn_FnInfo _tmp5A6;struct Cyc_List_List*
_tmp5A7;struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5AA;
struct Cyc_List_List*_tmp5AB;int _tmp5AC;struct Cyc_Absyn_VarargInfo*_tmp5AD;struct
Cyc_List_List*_tmp5AE;struct Cyc_List_List*_tmp5AF;_LL2AF: {struct Cyc_Absyn_FnType_struct*
_tmp5A5=(struct Cyc_Absyn_FnType_struct*)_tmp5A4;if(_tmp5A5->tag != 9)goto _LL2B1;
else{_tmp5A6=_tmp5A5->f1;_tmp5A7=_tmp5A6.tvars;_tmp5A8=_tmp5A6.effect;_tmp5A9=
_tmp5A6.ret_tqual;_tmp5AA=_tmp5A6.ret_typ;_tmp5AB=_tmp5A6.args;_tmp5AC=_tmp5A6.c_varargs;
_tmp5AD=_tmp5A6.cyc_varargs;_tmp5AE=_tmp5A6.rgn_po;_tmp5AF=_tmp5A6.attributes;}}
_LL2B0: if(_tmp5A7 != 0){struct _RegionHandle*_tmp5B0=Cyc_Tcenv_get_fnrgn(te);
struct _tuple12 _tmp9DC;struct _tuple12 _tmp5B1=(_tmp9DC.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp9DC.f2=_tmp5B0,_tmp9DC)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),
struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp5B0,Cyc_Tcutil_r_make_inst_var,&
_tmp5B1,_tmp5A7);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp9E2;struct Cyc_Absyn_FnInfo
_tmp9E1;struct Cyc_Absyn_FnType_struct*_tmp9E0;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp5B0,inst,(void*)((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E2.tag=
9,((_tmp9E2.f1=((_tmp9E1.tvars=0,((_tmp9E1.effect=_tmp5A8,((_tmp9E1.ret_tqual=
_tmp5A9,((_tmp9E1.ret_typ=_tmp5AA,((_tmp9E1.args=_tmp5AB,((_tmp9E1.c_varargs=
_tmp5AC,((_tmp9E1.cyc_varargs=_tmp5AD,((_tmp9E1.rgn_po=_tmp5AE,((_tmp9E1.attributes=
_tmp5AF,_tmp9E1)))))))))))))))))),_tmp9E2)))),_tmp9E0)))));struct Cyc_Absyn_PointerType_struct
_tmp9EC;struct Cyc_Absyn_PtrAtts _tmp9EB;struct Cyc_Absyn_PtrInfo _tmp9EA;struct Cyc_Absyn_PointerType_struct*
_tmp9E9;struct Cyc_Absyn_PointerType_struct*_tmp5B2=(_tmp9E9=_cycalloc(sizeof(*
_tmp9E9)),((_tmp9E9[0]=((_tmp9EC.tag=5,((_tmp9EC.f1=((_tmp9EA.elt_typ=ftyp,((
_tmp9EA.elt_tq=_tmp59E,((_tmp9EA.ptr_atts=((_tmp9EB.rgn=_tmp5A0,((_tmp9EB.nullable=
_tmp5A1,((_tmp9EB.bounds=_tmp5A2,((_tmp9EB.zero_term=_tmp5A3,((_tmp9EB.ptrloc=0,
_tmp9EB)))))))))),_tmp9EA)))))),_tmp9EC)))),_tmp9E9)));struct Cyc_Absyn_Exp*
_tmp5B3=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp9EF;
struct Cyc_Absyn_Instantiate_e_struct*_tmp9EE;e->r=(void*)((_tmp9EE=_cycalloc(
sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=14,((_tmp9EF.f1=_tmp5B3,((_tmp9EF.f2=
ts,_tmp9EF)))))),_tmp9EE))));}{struct Cyc_Core_Opt*_tmp9F0;e->topt=((_tmp9F0=
_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0->v=(void*)_tmp5B2,_tmp9F0))));};}goto
_LL2AE;_LL2B1:;_LL2B2: goto _LL2AE;_LL2AE:;}goto _LL2A9;_LL2AC:;_LL2AD: goto _LL2A9;
_LL2A9:;}goto _LL2A4;_LL2A4:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct
_RegionHandle*_tmp5BF=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp5C0=0;{
void*_tmp5C1=fn_exp->r;struct Cyc_List_List*_tmp5C3;_LL2B4: {struct Cyc_Absyn_FnCall_e_struct*
_tmp5C2=(struct Cyc_Absyn_FnCall_e_struct*)_tmp5C1;if(_tmp5C2->tag != 11)goto
_LL2B6;else{_tmp5C3=_tmp5C2->f2;}}_LL2B5:{void*_tmp5C4=e->r;struct Cyc_List_List*
_tmp5C6;_LL2B9: {struct Cyc_Absyn_FnCall_e_struct*_tmp5C5=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5C4;if(_tmp5C5->tag != 11)goto _LL2BB;else{_tmp5C6=_tmp5C5->f2;}}_LL2BA: {
struct Cyc_List_List*_tmp5C7=alias_arg_exps;int _tmp5C8=0;while(_tmp5C7 != 0){
while(_tmp5C8 != (int)_tmp5C7->hd){if(_tmp5C3 == 0){struct Cyc_Int_pa_struct _tmp9FA;
struct Cyc_Int_pa_struct _tmp9F9;void*_tmp9F8[2];const char*_tmp9F7;void*_tmp9F6;(
_tmp9F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmp9FA.tag=1,((_tmp9FA.f1=(unsigned long)((int)_tmp5C7->hd),((
_tmp9F9.tag=1,((_tmp9F9.f1=(unsigned long)_tmp5C8,((_tmp9F8[0]=& _tmp9F9,((
_tmp9F8[1]=& _tmp9FA,Cyc_aprintf(((_tmp9F7="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp9F7,sizeof(char),36))),_tag_dyneither(_tmp9F8,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp9F6,sizeof(void*),0)));}++ _tmp5C8;_tmp5C3=_tmp5C3->tl;_tmp5C6=((
struct Cyc_List_List*)_check_null(_tmp5C6))->tl;}{struct Cyc_Absyn_Decl*_tmp5CF;
struct Cyc_Absyn_Exp*_tmp5D0;struct _tuple11 _tmp5CE=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5C3))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5C6))->hd)->topt))->v));_tmp5CF=_tmp5CE.f1;_tmp5D0=_tmp5CE.f2;
_tmp5C3->hd=(void*)_tmp5D0;{struct Cyc_List_List*_tmp9FB;_tmp5C0=((_tmp9FB=
_region_malloc(_tmp5BF,sizeof(*_tmp9FB)),((_tmp9FB->hd=_tmp5CF,((_tmp9FB->tl=
_tmp5C0,_tmp9FB))))));}_tmp5C7=_tmp5C7->tl;};}goto _LL2B8;}_LL2BB:;_LL2BC: {const
char*_tmp9FE;void*_tmp9FD;(_tmp9FD=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9FE="not a function call!",
_tag_dyneither(_tmp9FE,sizeof(char),21))),_tag_dyneither(_tmp9FD,sizeof(void*),0)));}
_LL2B8:;}goto _LL2B3;_LL2B6:;_LL2B7: {const char*_tmpA01;void*_tmpA00;(_tmpA00=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpA01="not a function call!",_tag_dyneither(_tmpA01,sizeof(char),21))),
_tag_dyneither(_tmpA00,sizeof(void*),0)));}_LL2B3:;}while(_tmp5C0 != 0){struct Cyc_Absyn_Decl*
_tmp5D6=(struct Cyc_Absyn_Decl*)_tmp5C0->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp5D6,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp5C0=_tmp5C0->tl;}e->topt=
0;e->r=fn_exp->r;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*
_tmp5D7=e->r;struct Cyc_Absyn_Exp*_tmp5D9;struct _tuple1*_tmp5DB;struct Cyc_Absyn_Exp*
_tmp5DD;struct Cyc_List_List*_tmp5DE;struct Cyc_Core_Opt*_tmp5E0;struct Cyc_List_List*
_tmp5E1;union Cyc_Absyn_Cnst _tmp5E3;union Cyc_Absyn_Cnst*_tmp5E4;struct _tuple1*
_tmp5E6;void*_tmp5E7;enum Cyc_Absyn_Primop _tmp5E9;struct Cyc_List_List*_tmp5EA;
struct Cyc_Absyn_Exp*_tmp5EC;enum Cyc_Absyn_Incrementor _tmp5ED;struct Cyc_Absyn_Exp*
_tmp5EF;struct Cyc_Core_Opt*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*
_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Exp*
_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp*
_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FE;struct Cyc_Absyn_Exp*
_tmp600;struct Cyc_List_List*_tmp601;struct Cyc_Absyn_VarargCallInfo*_tmp602;
struct Cyc_Absyn_VarargCallInfo**_tmp603;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Exp*
_tmp607;struct Cyc_List_List*_tmp608;void*_tmp60A;struct Cyc_Absyn_Exp*_tmp60B;
enum Cyc_Absyn_Coercion _tmp60C;enum Cyc_Absyn_Coercion*_tmp60D;struct Cyc_Absyn_Exp*
_tmp60F;struct Cyc_Absyn_Exp*_tmp611;struct Cyc_Absyn_Exp**_tmp612;struct Cyc_Absyn_Exp*
_tmp613;struct Cyc_Absyn_Exp*_tmp615;void*_tmp617;void*_tmp619;void*_tmp61A;
struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Exp*_tmp61E;struct _dyneither_ptr*
_tmp61F;int _tmp620;int*_tmp621;int _tmp622;int*_tmp623;struct Cyc_Absyn_Exp*
_tmp625;struct _dyneither_ptr*_tmp626;int _tmp627;int*_tmp628;int _tmp629;int*
_tmp62A;struct Cyc_Absyn_Exp*_tmp62C;struct Cyc_Absyn_Exp*_tmp62D;struct Cyc_List_List*
_tmp62F;struct _tuple9*_tmp631;struct Cyc_List_List*_tmp632;struct Cyc_List_List*
_tmp634;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Vardecl*_tmp638;struct Cyc_Absyn_Exp*
_tmp639;struct Cyc_Absyn_Exp*_tmp63A;int _tmp63B;int*_tmp63C;struct _tuple1*_tmp63E;
struct _tuple1**_tmp63F;struct Cyc_List_List*_tmp640;struct Cyc_List_List**_tmp641;
struct Cyc_List_List*_tmp642;struct Cyc_Absyn_Aggrdecl*_tmp643;struct Cyc_Absyn_Aggrdecl**
_tmp644;void*_tmp646;struct Cyc_List_List*_tmp647;struct Cyc_List_List*_tmp649;
struct Cyc_Absyn_Datatypedecl*_tmp64A;struct Cyc_Absyn_Datatypefield*_tmp64B;
struct _tuple1*_tmp64D;struct _tuple1**_tmp64E;struct Cyc_Absyn_Enumdecl*_tmp64F;
struct Cyc_Absyn_Enumfield*_tmp650;struct _tuple1*_tmp652;struct _tuple1**_tmp653;
void*_tmp654;struct Cyc_Absyn_Enumfield*_tmp655;struct Cyc_Absyn_MallocInfo _tmp657;
int _tmp658;int*_tmp659;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp**_tmp65B;
void**_tmp65C;void***_tmp65D;struct Cyc_Absyn_Exp*_tmp65E;struct Cyc_Absyn_Exp**
_tmp65F;int _tmp660;int*_tmp661;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*
_tmp664;struct Cyc_Absyn_Exp*_tmp666;struct _dyneither_ptr*_tmp667;void*_tmp669;
_LL2BE: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp5D8=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp5D7;if(_tmp5D8->tag != 13)goto _LL2C0;else{_tmp5D9=_tmp5D8->f1;}}_LL2BF: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp5D9);return;_LL2C0: {struct Cyc_Absyn_UnknownId_e_struct*_tmp5DA=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp5D7;if(_tmp5DA->tag != 2)goto _LL2C2;else{
_tmp5DB=_tmp5DA->f1;}}_LL2C1: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp5DB);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2C2: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp5DC=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp5D7;if(_tmp5DC->tag != 10)goto _LL2C4;else{
_tmp5DD=_tmp5DC->f1;_tmp5DE=_tmp5DC->f2;}}_LL2C3: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp5DD,_tmp5DE);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C4: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5DF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp5D7;if(_tmp5DF->tag != 36)
goto _LL2C6;else{_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5DF->f2;}}_LL2C5: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp5E1);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C6: {struct
Cyc_Absyn_Const_e_struct*_tmp5E2=(struct Cyc_Absyn_Const_e_struct*)_tmp5D7;if(
_tmp5E2->tag != 0)goto _LL2C8;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=(union Cyc_Absyn_Cnst*)&
_tmp5E2->f1;}}_LL2C7: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp5E4,e);goto _LL2BD;_LL2C8: {struct Cyc_Absyn_Var_e_struct*_tmp5E5=(struct Cyc_Absyn_Var_e_struct*)
_tmp5D7;if(_tmp5E5->tag != 1)goto _LL2CA;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=(void*)
_tmp5E5->f2;}}_LL2C9: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp5E6,_tmp5E7);goto _LL2BD;
_LL2CA: {struct Cyc_Absyn_Primop_e_struct*_tmp5E8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5D7;if(_tmp5E8->tag != 3)goto _LL2CC;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E8->f2;}}
_LL2CB: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5E9,_tmp5EA);goto _LL2BD;_LL2CC: {
struct Cyc_Absyn_Increment_e_struct*_tmp5EB=(struct Cyc_Absyn_Increment_e_struct*)
_tmp5D7;if(_tmp5EB->tag != 5)goto _LL2CE;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;}}
_LL2CD: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5EC,_tmp5ED);goto _LL2BD;_LL2CE: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5EE=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D7;if(_tmp5EE->tag != 4)goto _LL2D0;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;
_tmp5F1=_tmp5EE->f3;}}_LL2CF: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5EF,_tmp5F0,
_tmp5F1);goto _LL2BD;_LL2D0: {struct Cyc_Absyn_Conditional_e_struct*_tmp5F2=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp5D7;if(_tmp5F2->tag != 6)goto _LL2D2;
else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;_tmp5F5=_tmp5F2->f3;}}_LL2D1: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5F3,_tmp5F4,_tmp5F5);goto _LL2BD;_LL2D2: {struct Cyc_Absyn_And_e_struct*
_tmp5F6=(struct Cyc_Absyn_And_e_struct*)_tmp5D7;if(_tmp5F6->tag != 7)goto _LL2D4;
else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;}}_LL2D3: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5F7,_tmp5F8);goto _LL2BD;_LL2D4: {struct Cyc_Absyn_Or_e_struct*_tmp5F9=(struct
Cyc_Absyn_Or_e_struct*)_tmp5D7;if(_tmp5F9->tag != 8)goto _LL2D6;else{_tmp5FA=
_tmp5F9->f1;_tmp5FB=_tmp5F9->f2;}}_LL2D5: t=Cyc_Tcexp_tcOr(te,loc,_tmp5FA,_tmp5FB);
goto _LL2BD;_LL2D6: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5FC=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp5D7;if(_tmp5FC->tag != 9)goto _LL2D8;else{_tmp5FD=_tmp5FC->f1;_tmp5FE=_tmp5FC->f2;}}
_LL2D7: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5FD,_tmp5FE);goto _LL2BD;_LL2D8: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5FF=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5D7;if(_tmp5FF->tag != 11)goto _LL2DA;else{_tmp600=_tmp5FF->f1;_tmp601=_tmp5FF->f2;
_tmp602=_tmp5FF->f3;_tmp603=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5FF->f3;}}
_LL2D9: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp66B;_push_handler(& _tmp66B);{int _tmp66D=0;if(setjmp(
_tmp66B.handler))_tmp66D=1;if(!_tmp66D){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp66C=(void*)_exn_thrown;void*_tmp66F=_tmp66C;struct
_dyneither_ptr _tmp671;_LL311: {struct Cyc_Core_Failure_struct*_tmp670=(struct Cyc_Core_Failure_struct*)
_tmp66F;if(_tmp670->tag != Cyc_Core_Failure)goto _LL313;else{_tmp671=_tmp670->f1;}}
_LL312: ok=0;fn_exp=e;goto _LL310;_LL313:;_LL314:(void)_throw(_tmp66F);_LL310:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp600,_tmp601,_tmp603,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2BD;}_LL2DA: {
struct Cyc_Absyn_Throw_e_struct*_tmp604=(struct Cyc_Absyn_Throw_e_struct*)_tmp5D7;
if(_tmp604->tag != 12)goto _LL2DC;else{_tmp605=_tmp604->f1;}}_LL2DB: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp605);goto _LL2BD;_LL2DC: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp606=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp5D7;if(_tmp606->tag != 14)goto
_LL2DE;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;}}_LL2DD: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp607,_tmp608);goto _LL2BD;_LL2DE: {struct Cyc_Absyn_Cast_e_struct*
_tmp609=(struct Cyc_Absyn_Cast_e_struct*)_tmp5D7;if(_tmp609->tag != 15)goto _LL2E0;
else{_tmp60A=(void*)_tmp609->f1;_tmp60B=_tmp609->f2;_tmp60C=_tmp609->f4;_tmp60D=(
enum Cyc_Absyn_Coercion*)& _tmp609->f4;}}_LL2DF: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp60A,_tmp60B,(enum Cyc_Absyn_Coercion*)_tmp60D);goto _LL2BD;_LL2E0: {struct Cyc_Absyn_Address_e_struct*
_tmp60E=(struct Cyc_Absyn_Address_e_struct*)_tmp5D7;if(_tmp60E->tag != 16)goto
_LL2E2;else{_tmp60F=_tmp60E->f1;}}_LL2E1: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp60F);goto _LL2BD;_LL2E2: {struct Cyc_Absyn_New_e_struct*_tmp610=(struct Cyc_Absyn_New_e_struct*)
_tmp5D7;if(_tmp610->tag != 17)goto _LL2E4;else{_tmp611=_tmp610->f1;_tmp612=(struct
Cyc_Absyn_Exp**)& _tmp610->f1;_tmp613=_tmp610->f2;}}_LL2E3: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp612,e,_tmp613);goto _LL2BD;_LL2E4: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp614=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5D7;if(_tmp614->tag != 19)goto
_LL2E6;else{_tmp615=_tmp614->f1;}}_LL2E5: {void*_tmp672=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp615);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp672);goto _LL2BD;}_LL2E6: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp616=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5D7;if(_tmp616->tag != 18)goto _LL2E8;else{_tmp617=(void*)_tmp616->f1;}}_LL2E7:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp617);goto _LL2BD;_LL2E8: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp618=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp5D7;if(_tmp618->tag != 20)goto
_LL2EA;else{_tmp619=(void*)_tmp618->f1;_tmp61A=(void*)_tmp618->f2;}}_LL2E9: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp619,_tmp61A);goto _LL2BD;_LL2EA: {struct Cyc_Absyn_Deref_e_struct*
_tmp61B=(struct Cyc_Absyn_Deref_e_struct*)_tmp5D7;if(_tmp61B->tag != 21)goto _LL2EC;
else{_tmp61C=_tmp61B->f1;}}_LL2EB: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp61C);goto
_LL2BD;_LL2EC: {struct Cyc_Absyn_AggrMember_e_struct*_tmp61D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp5D7;if(_tmp61D->tag != 22)goto _LL2EE;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61D->f2;
_tmp620=_tmp61D->f3;_tmp621=(int*)& _tmp61D->f3;_tmp622=_tmp61D->f4;_tmp623=(int*)&
_tmp61D->f4;}}_LL2ED: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp61E,_tmp61F,
_tmp621,_tmp623);goto _LL2BD;_LL2EE: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp624=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp5D7;if(_tmp624->tag != 23)goto _LL2F0;
else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;_tmp627=_tmp624->f3;_tmp628=(int*)&
_tmp624->f3;_tmp629=_tmp624->f4;_tmp62A=(int*)& _tmp624->f4;}}_LL2EF: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp625,_tmp626,_tmp628,_tmp62A);goto _LL2BD;_LL2F0: {struct Cyc_Absyn_Subscript_e_struct*
_tmp62B=(struct Cyc_Absyn_Subscript_e_struct*)_tmp5D7;if(_tmp62B->tag != 24)goto
_LL2F2;else{_tmp62C=_tmp62B->f1;_tmp62D=_tmp62B->f2;}}_LL2F1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp62C,_tmp62D);goto _LL2BD;_LL2F2: {struct Cyc_Absyn_Tuple_e_struct*
_tmp62E=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5D7;if(_tmp62E->tag != 25)goto _LL2F4;
else{_tmp62F=_tmp62E->f1;}}_LL2F3: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp62F);goto
_LL2BD;_LL2F4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp630=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp5D7;if(_tmp630->tag != 26)goto _LL2F6;else{_tmp631=_tmp630->f1;_tmp632=_tmp630->f2;}}
_LL2F5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp631,_tmp632);goto _LL2BD;_LL2F6: {
struct Cyc_Absyn_Array_e_struct*_tmp633=(struct Cyc_Absyn_Array_e_struct*)_tmp5D7;
if(_tmp633->tag != 27)goto _LL2F8;else{_tmp634=_tmp633->f1;}}_LL2F7: {void**
elt_topt=0;struct Cyc_Absyn_Tqual*elt_tqopt=0;int zero_term=0;if(topt != 0){void*
_tmp673=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp675;void*_tmp676;
void**_tmp677;struct Cyc_Absyn_Tqual _tmp678;struct Cyc_Absyn_Tqual*_tmp679;union
Cyc_Absyn_Constraint*_tmp67A;_LL316: {struct Cyc_Absyn_ArrayType_struct*_tmp674=(
struct Cyc_Absyn_ArrayType_struct*)_tmp673;if(_tmp674->tag != 8)goto _LL318;else{
_tmp675=_tmp674->f1;_tmp676=_tmp675.elt_type;_tmp677=(void**)&(_tmp674->f1).elt_type;
_tmp678=_tmp675.tq;_tmp679=(struct Cyc_Absyn_Tqual*)&(_tmp674->f1).tq;_tmp67A=
_tmp675.zero_term;}}_LL317: elt_topt=(void**)_tmp677;elt_tqopt=(struct Cyc_Absyn_Tqual*)
_tmp679;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp67A);goto _LL315;_LL318:;_LL319: goto _LL315;_LL315:;}t=Cyc_Tcexp_tcArray(te,
loc,elt_topt,elt_tqopt,zero_term,_tmp634);goto _LL2BD;}_LL2F8: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp635=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp5D7;if(_tmp635->tag != 37)goto
_LL2FA;else{_tmp636=_tmp635->f1;}}_LL2F9: t=Cyc_Tcexp_tcStmtExp(te,loc,topt,
_tmp636);goto _LL2BD;_LL2FA: {struct Cyc_Absyn_Comprehension_e_struct*_tmp637=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp5D7;if(_tmp637->tag != 28)goto _LL2FC;
else{_tmp638=_tmp637->f1;_tmp639=_tmp637->f2;_tmp63A=_tmp637->f3;_tmp63B=_tmp637->f4;
_tmp63C=(int*)& _tmp637->f4;}}_LL2FB: t=Cyc_Tcexp_tcComprehension(te,loc,topt,
_tmp638,_tmp639,_tmp63A,_tmp63C);goto _LL2BD;_LL2FC: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp63D=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5D7;if(_tmp63D->tag != 29)goto
_LL2FE;else{_tmp63E=_tmp63D->f1;_tmp63F=(struct _tuple1**)& _tmp63D->f1;_tmp640=
_tmp63D->f2;_tmp641=(struct Cyc_List_List**)& _tmp63D->f2;_tmp642=_tmp63D->f3;
_tmp643=_tmp63D->f4;_tmp644=(struct Cyc_Absyn_Aggrdecl**)& _tmp63D->f4;}}_LL2FD: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp63F,_tmp641,_tmp642,_tmp644);goto _LL2BD;
_LL2FE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp645=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp5D7;if(_tmp645->tag != 30)goto _LL300;else{_tmp646=(void*)_tmp645->f1;_tmp647=
_tmp645->f2;}}_LL2FF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp646,_tmp647);goto _LL2BD;
_LL300: {struct Cyc_Absyn_Datatype_e_struct*_tmp648=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp5D7;if(_tmp648->tag != 31)goto _LL302;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;
_tmp64B=_tmp648->f3;}}_LL301: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp649,
_tmp64A,_tmp64B);goto _LL2BD;_LL302: {struct Cyc_Absyn_Enum_e_struct*_tmp64C=(
struct Cyc_Absyn_Enum_e_struct*)_tmp5D7;if(_tmp64C->tag != 32)goto _LL304;else{
_tmp64D=_tmp64C->f1;_tmp64E=(struct _tuple1**)& _tmp64C->f1;_tmp64F=_tmp64C->f2;
_tmp650=_tmp64C->f3;}}_LL303:*_tmp64E=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp650))->name;{struct Cyc_Absyn_EnumType_struct _tmpA04;struct Cyc_Absyn_EnumType_struct*
_tmpA03;t=(void*)((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03[0]=((_tmpA04.tag=
13,((_tmpA04.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp64F))->name,((
_tmpA04.f2=_tmp64F,_tmpA04)))))),_tmpA03))));}goto _LL2BD;_LL304: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp651=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp5D7;if(_tmp651->tag != 33)goto
_LL306;else{_tmp652=_tmp651->f1;_tmp653=(struct _tuple1**)& _tmp651->f1;_tmp654=(
void*)_tmp651->f2;_tmp655=_tmp651->f3;}}_LL305:*_tmp653=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp655))->name;t=_tmp654;goto _LL2BD;_LL306: {struct Cyc_Absyn_Malloc_e_struct*
_tmp656=(struct Cyc_Absyn_Malloc_e_struct*)_tmp5D7;if(_tmp656->tag != 34)goto
_LL308;else{_tmp657=_tmp656->f1;_tmp658=_tmp657.is_calloc;_tmp659=(int*)&(
_tmp656->f1).is_calloc;_tmp65A=_tmp657.rgn;_tmp65B=(struct Cyc_Absyn_Exp**)&(
_tmp656->f1).rgn;_tmp65C=_tmp657.elt_type;_tmp65D=(void***)&(_tmp656->f1).elt_type;
_tmp65E=_tmp657.num_elts;_tmp65F=(struct Cyc_Absyn_Exp**)&(_tmp656->f1).num_elts;
_tmp660=_tmp657.fat_result;_tmp661=(int*)&(_tmp656->f1).fat_result;}}_LL307: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp65B,_tmp65D,_tmp65F,_tmp659,_tmp661);goto
_LL2BD;_LL308: {struct Cyc_Absyn_Swap_e_struct*_tmp662=(struct Cyc_Absyn_Swap_e_struct*)
_tmp5D7;if(_tmp662->tag != 35)goto _LL30A;else{_tmp663=_tmp662->f1;_tmp664=_tmp662->f2;}}
_LL309: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp663,_tmp664);goto _LL2BD;_LL30A: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp665=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp5D7;if(_tmp665->tag != 38)goto _LL30C;else{_tmp666=_tmp665->f1;_tmp667=_tmp665->f2;}}
_LL30B: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp666,_tmp667);goto _LL2BD;_LL30C: {
struct Cyc_Absyn_Valueof_e_struct*_tmp668=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp5D7;if(_tmp668->tag != 39)goto _LL30E;else{_tmp669=(void*)_tmp668->f1;}}_LL30D:
if(!te->allow_valueof){const char*_tmpA07;void*_tmpA06;(_tmpA06=0,Cyc_Tcutil_terr(
e->loc,((_tmpA07="valueof(-) can only occur within types",_tag_dyneither(_tmpA07,
sizeof(char),39))),_tag_dyneither(_tmpA06,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2BD;_LL30E: {struct Cyc_Absyn_Asm_e_struct*_tmp66A=(struct Cyc_Absyn_Asm_e_struct*)
_tmp5D7;if(_tmp66A->tag != 40)goto _LL2BD;}_LL30F:{const char*_tmpA0A;void*_tmpA09;(
_tmpA09=0,Cyc_Tcutil_terr(e->loc,((_tmpA0A="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmpA0A,sizeof(char),50))),_tag_dyneither(_tmpA09,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2BD:;}{struct Cyc_Core_Opt*
_tmpA0B;e->topt=((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->v=t,_tmpA0B))));};}
