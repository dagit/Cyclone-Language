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
Cyc_Absyn_UniqueRgn_val;extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
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
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_wchar_typ();void*Cyc_Absyn_exn_typ();
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
struct _tuple10{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10
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
struct Cyc_Absyn_Fndecl*);struct _tuple11{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);void
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
struct _RegionHandle*,void*t);struct _tuple13{int f1;void*f2;};struct _tuple13 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn);void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*loc,
void*t);struct _tuple14{unsigned int f1;int f2;};struct _tuple14 Cyc_Evexp_eval_const_uint_exp(
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
unsigned int val;};struct _tuple15{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple15 val;};struct _union_RelnOp_LessNumelts{
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
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _dyneither_ptr msg,struct
_dyneither_ptr ap){{void*_tmp680;(_tmp680=0,Cyc_Tcutil_terr(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp680,
sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons _tmp1;_push_handler(&
_tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))_tmp3=1;if(!_tmp3){{struct
_RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=Cyc_Tcenv_lookup_ordinary(
_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*_tmp9;struct Cyc_Absyn_Enumfield*
_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;struct Cyc_Absyn_Datatypedecl*
_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {struct Cyc_Tcenv_VarRes_struct*
_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(_tmp6->tag != 0)goto _LL3;else{
_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct _tmp683;struct Cyc_Absyn_Var_e_struct*
_tmp682;e->r=(void*)((_tmp682=_cycalloc(sizeof(*_tmp682)),((_tmp682[0]=((_tmp683.tag=
1,((_tmp683.f1=q,((_tmp683.f2=(void*)_tmp7,_tmp683)))))),_tmp682))));}goto _LL0;
_LL3: {struct Cyc_Tcenv_EnumRes_struct*_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp5;if(_tmp8->tag != 3)goto _LL5;else{_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{
struct Cyc_Absyn_Enum_e_struct _tmp686;struct Cyc_Absyn_Enum_e_struct*_tmp685;e->r=(
void*)((_tmp685=_cycalloc(sizeof(*_tmp685)),((_tmp685[0]=((_tmp686.tag=32,((
_tmp686.f1=q,((_tmp686.f2=(struct Cyc_Absyn_Enumdecl*)_tmp9,((_tmp686.f3=(struct
Cyc_Absyn_Enumfield*)_tmpA,_tmp686)))))))),_tmp685))));}goto _LL0;_LL5: {struct
Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5;
if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}_LL6:{
struct Cyc_Absyn_AnonEnum_e_struct _tmp689;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp688;e->r=(void*)((_tmp688=_cycalloc(sizeof(*_tmp688)),((_tmp688[0]=((_tmp689.tag=
33,((_tmp689.f1=q,((_tmp689.f2=(void*)_tmpC,((_tmp689.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp689)))))))),_tmp688))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp68C;struct Cyc_Absyn_Datatype_e_struct*_tmp68B;e->r=(void*)((_tmp68B=
_cycalloc(sizeof(*_tmp68B)),((_tmp68B[0]=((_tmp68C.tag=31,((_tmp68C.f1=0,((
_tmp68C.f2=_tmpF,((_tmp68C.f3=_tmp10,_tmp68C)))))))),_tmp68B))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp690;void*_tmp68F[1];struct Cyc_String_pa_struct
_tmp68E;(_tmp68E.tag=0,((_tmp68E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp68F[0]=& _tmp68E,Cyc_Tcutil_terr(e->loc,((
_tmp690="bad occurrence of type name %s",_tag_dyneither(_tmp690,sizeof(char),31))),
_tag_dyneither(_tmp68F,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp693;struct Cyc_Absyn_Var_e_struct*_tmp692;e->r=(void*)((_tmp692=_cycalloc(
sizeof(*_tmp692)),((_tmp692[0]=((_tmp693.tag=1,((_tmp693.f1=q,((_tmp693.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp693)))))),_tmp692))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp696;struct Cyc_Absyn_Var_e_struct*_tmp695;e->r=(void*)((_tmp695=_cycalloc(
sizeof(*_tmp695)),((_tmp695[0]=((_tmp696.tag=1,((_tmp696.f1=q,((_tmp696.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp696)))))),_tmp695))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple17{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple17*_tmp697;return(_tmp697=_cycalloc(sizeof(*_tmp697)),((_tmp697->f1=
0,((_tmp697->f2=e,_tmp697)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp69A;struct
Cyc_Absyn_FnCall_e_struct*_tmp699;e->r=(void*)((_tmp699=_cycalloc(sizeof(*
_tmp699)),((_tmp699[0]=((_tmp69A.tag=11,((_tmp69A.f1=e1,((_tmp69A.f2=es,((
_tmp69A.f3=0,_tmp69A)))))))),_tmp699))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp69E;void*_tmp69D[1];struct Cyc_String_pa_struct
_tmp69C;(_tmp69C.tag=0,((_tmp69C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp69D[0]=& _tmp69C,Cyc_Tcutil_terr(e->loc,((
_tmp69E="%s is a constant, not a function",_tag_dyneither(_tmp69E,sizeof(char),
33))),_tag_dyneither(_tmp69D,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp6A1;struct Cyc_Absyn_Datatype_e_struct*_tmp6A0;e->r=(void*)((_tmp6A0=
_cycalloc(sizeof(*_tmp6A0)),((_tmp6A0[0]=((_tmp6A1.tag=31,((_tmp6A1.f1=es,((
_tmp6A1.f2=_tmp30,((_tmp6A1.f3=_tmp31,_tmp6A1)))))))),_tmp6A0))));}_npop_handler(
0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*
_tmp3D=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp6A4;struct Cyc_Absyn_Aggregate_e_struct*_tmp6A3;e->r=(void*)((_tmp6A3=
_cycalloc(sizeof(*_tmp6A3)),((_tmp6A3[0]=((_tmp6A4.tag=29,((_tmp6A4.f1=_tmp33->name,((
_tmp6A4.f2=0,((_tmp6A4.f3=_tmp3D,((_tmp6A4.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp6A4)))))))))),_tmp6A3))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp6A8;void*_tmp6A7[1];
struct Cyc_String_pa_struct _tmp6A6;(_tmp6A6.tag=0,((_tmp6A6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp6A7[
0]=& _tmp6A6,Cyc_Tcutil_terr(e->loc,((_tmp6A8="%s is an enum constructor, not a function",
_tag_dyneither(_tmp6A8,sizeof(char),42))),_tag_dyneither(_tmp6A7,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp6AB;struct Cyc_Absyn_FnCall_e_struct*
_tmp6AA;e->r=(void*)((_tmp6AA=_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=
11,((_tmp6AB.f1=e1,((_tmp6AB.f2=es,((_tmp6AB.f3=0,_tmp6AB)))))))),_tmp6AA))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp6AE;struct Cyc_Absyn_FnCall_e_struct*_tmp6AD;e->r=(void*)((_tmp6AD=_cycalloc(
sizeof(*_tmp6AD)),((_tmp6AD[0]=((_tmp6AE.tag=11,((_tmp6AE.f1=e1,((_tmp6AE.f2=es,((
_tmp6AE.f3=0,_tmp6AE)))))))),_tmp6AD))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp6B1;
struct Cyc_Absyn_Array_e_struct*_tmp6B0;e->r=(void*)((_tmp6B0=_cycalloc(sizeof(*
_tmp6B0)),((_tmp6B0[0]=((_tmp6B1.tag=27,((_tmp6B1.f1=des,_tmp6B1)))),_tmp6B0))));}
return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp6B4;void*_tmp6B3;(_tmp6B3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6B4="struct type not properly set",
_tag_dyneither(_tmp6B4,sizeof(char),29))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp6B7;struct Cyc_Absyn_Aggregate_e_struct*_tmp6B6;e->r=(void*)((_tmp6B6=
_cycalloc(sizeof(*_tmp6B6)),((_tmp6B6[0]=((_tmp6B7.tag=29,((_tmp6B7.f1=_tmp58->name,((
_tmp6B7.f2=0,((_tmp6B7.f3=des,((_tmp6B7.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp6B7)))))))))),_tmp6B6))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 9)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp6BA;struct Cyc_Absyn_Array_e_struct*_tmp6B9;e->r=(
void*)((_tmp6B9=_cycalloc(sizeof(*_tmp6B9)),((_tmp6B9[0]=((_tmp6BA.tag=27,((
_tmp6BA.f1=des,_tmp6BA)))),_tmp6B9))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp6BD;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp6BC;e->r=(void*)((_tmp6BC=_cycalloc(sizeof(*_tmp6BC)),((_tmp6BC[0]=((_tmp6BD.tag=
30,((_tmp6BD.f1=(void*)t,((_tmp6BD.f2=des,_tmp6BD)))))),_tmp6BC))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp6C0;struct Cyc_Absyn_Array_e_struct*
_tmp6BF;e->r=(void*)((_tmp6BF=_cycalloc(sizeof(*_tmp6BF)),((_tmp6BF[0]=((_tmp6C0.tag=
27,((_tmp6C0.f1=des,_tmp6C0)))),_tmp6BF))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6C5;void*_tmp6C4[2];struct Cyc_String_pa_struct _tmp6C3;
struct Cyc_String_pa_struct _tmp6C2;(_tmp6C2.tag=0,((_tmp6C2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp6C3.tag=0,((_tmp6C3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6C4[0]=& _tmp6C3,((_tmp6C4[1]=& _tmp6C2,Cyc_Tcutil_terr(
e->loc,((_tmp6C5="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6C5,sizeof(char),53))),_tag_dyneither(_tmp6C4,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6C6;struct
_tuple0 _tmp6C=(_tmp6C6.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp6C6.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp6C6)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp6C9;struct Cyc_Tcexp_TestEnv _tmp6C8;return(_tmp6C8.eq=((_tmp6C9=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6C9)),((_tmp6C9->f1=_tmp6F,((
_tmp6C9->f2=_tmp72,_tmp6C9)))))),((_tmp6C8.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp6C8)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp6CA;return(_tmp6CA.eq=0,((_tmp6CA.isTrue=0,_tmp6CA)));};}
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){return 1;}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst*
c,struct Cyc_Absyn_Exp*e){void*t;void*string_elt_typ=Cyc_Absyn_char_typ;int
string_numelts=0;{union Cyc_Absyn_Cnst _tmp76=*((union Cyc_Absyn_Cnst*)_check_null(
c));struct _tuple4 _tmp77;enum Cyc_Absyn_Sign _tmp78;struct _tuple4 _tmp79;enum Cyc_Absyn_Sign
_tmp7A;struct _tuple4 _tmp7B;enum Cyc_Absyn_Sign _tmp7C;struct _dyneither_ptr _tmp7D;
struct _tuple5 _tmp7E;enum Cyc_Absyn_Sign _tmp7F;struct _tuple7 _tmp80;enum Cyc_Absyn_Sign
_tmp81;struct _dyneither_ptr _tmp82;struct _tuple6 _tmp83;enum Cyc_Absyn_Sign _tmp84;
int _tmp85;struct _dyneither_ptr _tmp86;struct _dyneither_ptr _tmp87;int _tmp88;_LL3E:
if((_tmp76.Char_c).tag != 2)goto _LL40;_tmp77=(struct _tuple4)(_tmp76.Char_c).val;
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
_LL4A: if((_tmp76.Float_c).tag != 7)goto _LL4C;_tmp82=(struct _dyneither_ptr)(_tmp76.Float_c).val;
_LL4B: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4C: if((_tmp76.Int_c).tag != 5)goto _LL4E;
_tmp83=(struct _tuple6)(_tmp76.Int_c).val;_tmp84=_tmp83.f1;_tmp85=_tmp83.f2;_LL4D:
if(topt == 0)t=_tmp84 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
else{void*_tmp89=Cyc_Tcutil_compress(*topt);enum Cyc_Absyn_Sign _tmp8B;enum Cyc_Absyn_Size_of
_tmp8C;enum Cyc_Absyn_Sign _tmp8E;enum Cyc_Absyn_Size_of _tmp8F;enum Cyc_Absyn_Sign
_tmp91;enum Cyc_Absyn_Size_of _tmp92;enum Cyc_Absyn_Sign _tmp94;enum Cyc_Absyn_Size_of
_tmp95;void*_tmp98;_LL55: {struct Cyc_Absyn_IntType_struct*_tmp8A=(struct Cyc_Absyn_IntType_struct*)
_tmp89;if(_tmp8A->tag != 6)goto _LL57;else{_tmp8B=_tmp8A->f1;_tmp8C=_tmp8A->f2;if(
_tmp8C != Cyc_Absyn_Char_sz)goto _LL57;}}_LL56: switch(_tmp8B){case Cyc_Absyn_Unsigned:
_LL63: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL64: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL65: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp8B,(char)_tmp85);goto _LL54;_LL57: {struct Cyc_Absyn_IntType_struct*_tmp8D=(
struct Cyc_Absyn_IntType_struct*)_tmp89;if(_tmp8D->tag != 6)goto _LL59;else{_tmp8E=
_tmp8D->f1;_tmp8F=_tmp8D->f2;if(_tmp8F != Cyc_Absyn_Short_sz)goto _LL59;}}_LL58: t=
_tmp8E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=Cyc_Absyn_Short_c(
_tmp8E,(short)_tmp85);goto _LL54;_LL59: {struct Cyc_Absyn_IntType_struct*_tmp90=(
struct Cyc_Absyn_IntType_struct*)_tmp89;if(_tmp90->tag != 6)goto _LL5B;else{_tmp91=
_tmp90->f1;_tmp92=_tmp90->f2;if(_tmp92 != Cyc_Absyn_Int_sz)goto _LL5B;}}_LL5A: t=
_tmp91 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL54;
_LL5B: {struct Cyc_Absyn_IntType_struct*_tmp93=(struct Cyc_Absyn_IntType_struct*)
_tmp89;if(_tmp93->tag != 6)goto _LL5D;else{_tmp94=_tmp93->f1;_tmp95=_tmp93->f2;if(
_tmp95 != Cyc_Absyn_Long_sz)goto _LL5D;}}_LL5C: t=_tmp94 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL54;_LL5D:{struct Cyc_Absyn_PointerType_struct*_tmp96=(
struct Cyc_Absyn_PointerType_struct*)_tmp89;if(_tmp96->tag != 5)goto _LL5F;}if(!(
_tmp85 == 0))goto _LL5F;_LL5E:{struct Cyc_Absyn_Const_e_struct _tmp6CD;struct Cyc_Absyn_Const_e_struct*
_tmp6CC;e->r=(void*)((_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC[0]=((_tmp6CD.tag=
0,((_tmp6CD.f1=Cyc_Absyn_Null_c,_tmp6CD)))),_tmp6CC))));}{struct Cyc_List_List*
_tmp9B=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp6E0;
struct Cyc_Absyn_PtrAtts _tmp6DF;struct Cyc_Core_Opt*_tmp6DE;struct Cyc_Core_Opt*
_tmp6DD;struct Cyc_Absyn_PtrInfo _tmp6DC;struct Cyc_Absyn_PointerType_struct*
_tmp6DB;t=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6E0.tag=
5,((_tmp6E0.f1=((_tmp6DC.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD->v=_tmp9B,_tmp6DD))))),((_tmp6DC.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6DC.ptr_atts=((_tmp6DF.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((
_tmp6DE->v=_tmp9B,_tmp6DE))))),((_tmp6DF.nullable=Cyc_Absyn_true_conref,((
_tmp6DF.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6DF.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6DF.ptrloc=0,_tmp6DF)))))))))),_tmp6DC)))))),_tmp6E0)))),_tmp6DB))));}goto
_LL54;};_LL5F: {struct Cyc_Absyn_TagType_struct*_tmp97=(struct Cyc_Absyn_TagType_struct*)
_tmp89;if(_tmp97->tag != 20)goto _LL61;else{_tmp98=(void*)_tmp97->f1;}}_LL60: {
struct Cyc_Absyn_ValueofType_struct _tmp6E3;struct Cyc_Absyn_ValueofType_struct*
_tmp6E2;struct Cyc_Absyn_ValueofType_struct*_tmpA2=(_tmp6E2=_cycalloc(sizeof(*
_tmp6E2)),((_tmp6E2[0]=((_tmp6E3.tag=19,((_tmp6E3.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp85,0),_tmp6E3)))),_tmp6E2)));if(!Cyc_Tcutil_unify(_tmp98,(void*)
_tmpA2)){{const char*_tmp6E8;void*_tmp6E7[2];struct Cyc_String_pa_struct _tmp6E6;
struct Cyc_String_pa_struct _tmp6E5;(_tmp6E5.tag=0,((_tmp6E5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA2)),((
_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp98)),((_tmp6E7[0]=& _tmp6E6,((_tmp6E7[1]=& _tmp6E5,Cyc_Tcutil_terr(loc,((
_tmp6E8="expecting %s but found %s",_tag_dyneither(_tmp6E8,sizeof(char),26))),
_tag_dyneither(_tmp6E7,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp6EB;struct Cyc_Absyn_TagType_struct*_tmp6EA;t=(
void*)((_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA[0]=((_tmp6EB.tag=20,((
_tmp6EB.f1=(void*)((void*)_tmpA2),_tmp6EB)))),_tmp6EA))));}goto _LL54;}_LL61:;
_LL62: t=_tmp84 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL54;_LL54:;}goto _LL3D;_LL4E: if((_tmp76.String_c).tag != 8)goto _LL50;_tmp86=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4F: string_numelts=(int)
_get_dyneither_size(_tmp86,sizeof(char));_tmp87=_tmp86;goto _LL51;_LL50: if((
_tmp76.Wstring_c).tag != 9)goto _LL52;_tmp87=(struct _dyneither_ptr)(_tmp76.Wstring_c).val;
_LL51: if(string_numelts == 0){string_numelts=Cyc_Tcexp_wchar_numelts(_tmp87);
string_elt_typ=Cyc_Absyn_wchar_typ();}{struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);{struct Cyc_Core_Opt*
_tmp6EC;elen->topt=((_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC->v=Cyc_Absyn_uint_typ,
_tmp6EC))));}{struct Cyc_Absyn_Upper_b_struct _tmp6EF;struct Cyc_Absyn_Upper_b_struct*
_tmp6EE;t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=1,((
_tmp6EF.f1=elen,_tmp6EF)))),_tmp6EE)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAE=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpB0;struct Cyc_Absyn_Tqual
_tmpB1;_LL68: {struct Cyc_Absyn_ArrayType_struct*_tmpAF=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAE;if(_tmpAF->tag != 9)goto _LL6A;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpB0.tq;}}
_LL69: return Cyc_Absyn_array_typ(string_elt_typ,_tmpB1,(struct Cyc_Absyn_Exp*)elen,((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL6A: {struct Cyc_Absyn_PointerType_struct*
_tmpB2=(struct Cyc_Absyn_PointerType_struct*)_tmpAE;if(_tmpB2->tag != 5)goto _LL6C;}
_LL6B: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{
struct Cyc_Core_Opt*_tmp6F0;e->topt=((_tmp6F0=_cycalloc(sizeof(*_tmp6F0)),((
_tmp6F0->v=t,_tmp6F0))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp6F3;struct Cyc_Absyn_Upper_b_struct*
_tmp6F2;t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp6F2=_cycalloc(sizeof(*_tmp6F2)),((_tmp6F2[0]=((_tmp6F3.tag=1,((
_tmp6F3.f1=elen,_tmp6F3)))),_tmp6F2)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp6F4;e->topt=((_tmp6F4=_cycalloc(sizeof(*_tmp6F4)),((_tmp6F4->v=t,_tmp6F4))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL67;_LL6C:;_LL6D: goto _LL67;_LL67:;}return t;};_LL52: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp88=(int)(_tmp76.Null_c).val;_LL53: {struct Cyc_List_List*_tmpB7=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp707;struct Cyc_Absyn_PtrAtts _tmp706;
struct Cyc_Core_Opt*_tmp705;struct Cyc_Core_Opt*_tmp704;struct Cyc_Absyn_PtrInfo
_tmp703;struct Cyc_Absyn_PointerType_struct*_tmp702;t=(void*)((_tmp702=_cycalloc(
sizeof(*_tmp702)),((_tmp702[0]=((_tmp707.tag=5,((_tmp707.f1=((_tmp703.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp704=_cycalloc(
sizeof(*_tmp704)),((_tmp704->v=_tmpB7,_tmp704))))),((_tmp703.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp703.ptr_atts=((_tmp706.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp705=_cycalloc(sizeof(*_tmp705)),((_tmp705->v=_tmpB7,_tmp705))))),((_tmp706.nullable=
Cyc_Absyn_true_conref,((_tmp706.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp706.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp706.ptrloc=0,_tmp706)))))))))),_tmp703)))))),_tmp707)))),_tmp702))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){
void*_tmpBE=b;struct Cyc_Absyn_Vardecl*_tmpC1;struct Cyc_Absyn_Fndecl*_tmpC3;
struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*_tmpC7;struct Cyc_Absyn_Vardecl*
_tmpC9;_LL6F: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBF=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBE;if(_tmpBF->tag != 0)goto _LL71;}_LL70: {const char*_tmp70B;void*_tmp70A[1];
struct Cyc_String_pa_struct _tmp709;return(_tmp709.tag=0,((_tmp709.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp70A[0]=&
_tmp709,Cyc_Tcexp_expr_err(te,loc,0,((_tmp70B="undeclared identifier: %s",
_tag_dyneither(_tmp70B,sizeof(char),26))),_tag_dyneither(_tmp70A,sizeof(void*),1)))))));}
_LL71: {struct Cyc_Absyn_Global_b_struct*_tmpC0=(struct Cyc_Absyn_Global_b_struct*)
_tmpBE;if(_tmpC0->tag != 1)goto _LL73;else{_tmpC1=_tmpC0->f1;}}_LL72:*q=*_tmpC1->name;
return _tmpC1->type;_LL73: {struct Cyc_Absyn_Funname_b_struct*_tmpC2=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBE;if(_tmpC2->tag != 2)goto _LL75;else{_tmpC3=_tmpC2->f1;}}_LL74:*q=*_tmpC3->name;
return Cyc_Tcutil_fndecl2typ(_tmpC3);_LL75: {struct Cyc_Absyn_Pat_b_struct*_tmpC4=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBE;if(_tmpC4->tag != 5)goto _LL77;else{_tmpC5=
_tmpC4->f1;}}_LL76: _tmpC7=_tmpC5;goto _LL78;_LL77: {struct Cyc_Absyn_Local_b_struct*
_tmpC6=(struct Cyc_Absyn_Local_b_struct*)_tmpBE;if(_tmpC6->tag != 4)goto _LL79;
else{_tmpC7=_tmpC6->f1;}}_LL78: _tmpC9=_tmpC7;goto _LL7A;_LL79: {struct Cyc_Absyn_Param_b_struct*
_tmpC8=(struct Cyc_Absyn_Param_b_struct*)_tmpBE;if(_tmpC8->tag != 3)goto _LL6E;
else{_tmpC9=_tmpC8->f1;}}_LL7A:{union Cyc_Absyn_Nmspace _tmp70C;(*q).f1=(((_tmp70C.Loc_n).val=
0,(((_tmp70C.Loc_n).tag=4,_tmp70C))));}if(te->allow_valueof){void*_tmpCE=Cyc_Tcutil_compress(
_tmpC9->type);void*_tmpD0;_LL7C: {struct Cyc_Absyn_TagType_struct*_tmpCF=(struct
Cyc_Absyn_TagType_struct*)_tmpCE;if(_tmpCF->tag != 20)goto _LL7E;else{_tmpD0=(void*)
_tmpCF->f1;}}_LL7D:{struct Cyc_Absyn_Valueof_e_struct _tmp70F;struct Cyc_Absyn_Valueof_e_struct*
_tmp70E;e->r=(void*)((_tmp70E=_cycalloc(sizeof(*_tmp70E)),((_tmp70E[0]=((_tmp70F.tag=
39,((_tmp70F.f1=(void*)_tmpD0,_tmp70F)))),_tmp70E))));}goto _LL7B;_LL7E:;_LL7F:
goto _LL7B;_LL7B:;}return _tmpC9->type;_LL6E:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int
arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpD3=fmt->r;
union Cyc_Absyn_Cnst _tmpD5;struct _dyneither_ptr _tmpD6;struct Cyc_Absyn_Exp*_tmpD8;
struct Cyc_Absyn_Exp _tmpD9;void*_tmpDA;union Cyc_Absyn_Cnst _tmpDC;struct
_dyneither_ptr _tmpDD;_LL81: {struct Cyc_Absyn_Const_e_struct*_tmpD4=(struct Cyc_Absyn_Const_e_struct*)
_tmpD3;if(_tmpD4->tag != 0)goto _LL83;else{_tmpD5=_tmpD4->f1;if((_tmpD5.String_c).tag
!= 8)goto _LL83;_tmpD6=(struct _dyneither_ptr)(_tmpD5.String_c).val;}}_LL82: _tmpDD=
_tmpD6;goto _LL84;_LL83: {struct Cyc_Absyn_Cast_e_struct*_tmpD7=(struct Cyc_Absyn_Cast_e_struct*)
_tmpD3;if(_tmpD7->tag != 15)goto _LL85;else{_tmpD8=_tmpD7->f2;_tmpD9=*_tmpD8;
_tmpDA=_tmpD9.r;{struct Cyc_Absyn_Const_e_struct*_tmpDB=(struct Cyc_Absyn_Const_e_struct*)
_tmpDA;if(_tmpDB->tag != 0)goto _LL85;else{_tmpDC=_tmpDB->f1;if((_tmpDC.String_c).tag
!= 8)goto _LL85;_tmpDD=(struct _dyneither_ptr)(_tmpDC.String_c).val;}};}}_LL84:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDD,fmt->loc);goto _LL80;
_LL85:;_LL86: if(opt_args != 0){struct Cyc_List_List*_tmpDE=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDE != 0;_tmpDE=_tmpDE->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDE->hd);{struct _RegionHandle*_tmpDF=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpDF,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDE->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpDF,(struct Cyc_Absyn_Exp*)_tmpDE->hd)){const
char*_tmp712;void*_tmp711;(_tmp711=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDE->hd)->loc,((_tmp712="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp712,sizeof(char),49))),_tag_dyneither(_tmp711,sizeof(void*),0)));}};}}
return;_LL80:;}if(opt_args != 0){struct Cyc_List_List*_tmpE2=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE2 != 0;(((desc_types=desc_types->tl,
_tmpE2=_tmpE2->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE2->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp717;void*
_tmp716[2];struct Cyc_String_pa_struct _tmp715;struct Cyc_String_pa_struct _tmp714;(
_tmp714.tag=0,((_tmp714.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp715.tag=0,((_tmp715.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp716[
0]=& _tmp715,((_tmp716[1]=& _tmp714,Cyc_Tcutil_terr(e->loc,((_tmp717="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp717,sizeof(char),51))),_tag_dyneither(_tmp716,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp718;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp718=_cycalloc(sizeof(*_tmp718)),((
_tmp718->hd=(void*)arg_cnt,((_tmp718->tl=*alias_arg_exps,_tmp718))))));}{struct
_RegionHandle*_tmpE8=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE8,t) && !Cyc_Tcutil_is_noalias_path(_tmpE8,e)){const char*_tmp71B;void*
_tmp71A;(_tmp71A=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE2->hd)->loc,((
_tmp71B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp71B,sizeof(char),49))),_tag_dyneither(_tmp71A,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp71E;void*_tmp71D;(_tmp71D=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp71E="too few arguments",_tag_dyneither(_tmp71E,sizeof(char),18))),
_tag_dyneither(_tmp71D,sizeof(void*),0)));}if(_tmpE2 != 0){const char*_tmp721;void*
_tmp720;(_tmp720=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE2->hd)->loc,((
_tmp721="too many arguments",_tag_dyneither(_tmp721,sizeof(char),19))),
_tag_dyneither(_tmp720,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL87: goto _LL88;case Cyc_Absyn_Minus:
_LL88: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp725;void*_tmp724[1];struct Cyc_String_pa_struct
_tmp723;(_tmp723.tag=0,((_tmp723.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp724[0]=& _tmp723,Cyc_Tcutil_terr(loc,((_tmp725="expecting arithmetic type but found %s",
_tag_dyneither(_tmp725,sizeof(char),39))),_tag_dyneither(_tmp724,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL89: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp729;void*_tmp728[1];struct Cyc_String_pa_struct _tmp727;(_tmp727.tag=
0,((_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp728[0]=& _tmp727,Cyc_Tcutil_terr(loc,((_tmp729="expecting integral or * type but found %s",
_tag_dyneither(_tmp729,sizeof(char),42))),_tag_dyneither(_tmp728,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL8A: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp72D;void*_tmp72C[1];struct Cyc_String_pa_struct _tmp72B;(_tmp72B.tag=
0,((_tmp72B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp72C[0]=& _tmp72B,Cyc_Tcutil_terr(loc,((_tmp72D="expecting integral type but found %s",
_tag_dyneither(_tmp72D,sizeof(char),37))),_tag_dyneither(_tmp72C,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Numelts:
_LL8B:{void*_tmpF8=t;struct Cyc_Absyn_PtrInfo _tmpFB;struct Cyc_Absyn_PtrAtts _tmpFC;
union Cyc_Absyn_Constraint*_tmpFD;_LL8E: {struct Cyc_Absyn_ArrayType_struct*_tmpF9=(
struct Cyc_Absyn_ArrayType_struct*)_tmpF8;if(_tmpF9->tag != 9)goto _LL90;}_LL8F:
goto _LL8D;_LL90: {struct Cyc_Absyn_PointerType_struct*_tmpFA=(struct Cyc_Absyn_PointerType_struct*)
_tmpF8;if(_tmpFA->tag != 5)goto _LL92;else{_tmpFB=_tmpFA->f1;_tmpFC=_tmpFB.ptr_atts;
_tmpFD=_tmpFC.bounds;}}_LL91:{void*_tmpFE=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFD);struct Cyc_Absyn_Exp*
_tmp101;_LL95: {struct Cyc_Absyn_DynEither_b_struct*_tmpFF=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFE;if(_tmpFF->tag != 0)goto _LL97;}_LL96: goto _LL94;_LL97: {struct Cyc_Absyn_Upper_b_struct*
_tmp100=(struct Cyc_Absyn_Upper_b_struct*)_tmpFE;if(_tmp100->tag != 1)goto _LL94;
else{_tmp101=_tmp100->f1;}}_LL98: if(!Cyc_Evexp_c_can_eval(_tmp101)){const char*
_tmp730;void*_tmp72F;(_tmp72F=0,Cyc_Tcutil_terr(loc,((_tmp730="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp730,sizeof(char),55))),_tag_dyneither(_tmp72F,sizeof(void*),0)));}
goto _LL94;_LL94:;}goto _LL8D;_LL92:;_LL93: {const char*_tmp734;void*_tmp733[1];
struct Cyc_String_pa_struct _tmp732;(_tmp732.tag=0,((_tmp732.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp733[0]=&
_tmp732,Cyc_Tcutil_terr(loc,((_tmp734="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp734,sizeof(char),47))),_tag_dyneither(_tmp733,sizeof(void*),1)))))));}
_LL8D:;}return Cyc_Absyn_uint_typ;default: _LL8C: {const char*_tmp737;void*_tmp736;(
_tmp736=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp737="Non-unary primop",_tag_dyneither(_tmp737,sizeof(char),17))),
_tag_dyneither(_tmp736,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp73B;void*_tmp73A[
1];struct Cyc_String_pa_struct _tmp739;(_tmp739.tag=0,((_tmp739.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp73A[0]=& _tmp739,Cyc_Tcutil_terr(e1->loc,((
_tmp73B="type %s cannot be used here",_tag_dyneither(_tmp73B,sizeof(char),28))),
_tag_dyneither(_tmp73A,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp73F;void*_tmp73E[1];struct Cyc_String_pa_struct
_tmp73D;(_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp73E[0]=& _tmp73D,Cyc_Tcutil_terr(e2->loc,((_tmp73F="type %s cannot be used here",
_tag_dyneither(_tmp73F,sizeof(char),28))),_tag_dyneither(_tmp73E,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);};}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmp10F=t1;struct Cyc_Absyn_PtrInfo _tmp111;void*
_tmp112;struct Cyc_Absyn_Tqual _tmp113;struct Cyc_Absyn_PtrAtts _tmp114;void*_tmp115;
union Cyc_Absyn_Constraint*_tmp116;union Cyc_Absyn_Constraint*_tmp117;union Cyc_Absyn_Constraint*
_tmp118;_LL9B: {struct Cyc_Absyn_PointerType_struct*_tmp110=(struct Cyc_Absyn_PointerType_struct*)
_tmp10F;if(_tmp110->tag != 5)goto _LL9D;else{_tmp111=_tmp110->f1;_tmp112=_tmp111.elt_typ;
_tmp113=_tmp111.elt_tq;_tmp114=_tmp111.ptr_atts;_tmp115=_tmp114.rgn;_tmp116=
_tmp114.nullable;_tmp117=_tmp114.bounds;_tmp118=_tmp114.zero_term;}}_LL9C: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp112),& Cyc_Tcutil_tmk)){const char*
_tmp742;void*_tmp741;(_tmp741=0,Cyc_Tcutil_terr(e1->loc,((_tmp742="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp742,sizeof(char),50))),_tag_dyneither(_tmp741,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp745;void*_tmp744;(_tmp744=0,
Cyc_Tcutil_terr(e1->loc,((_tmp745="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp745,sizeof(char),54))),_tag_dyneither(_tmp744,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp749;void*_tmp748[1];struct
Cyc_String_pa_struct _tmp747;(_tmp747.tag=0,((_tmp747.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp748[0]=& _tmp747,Cyc_Tcutil_terr(
e2->loc,((_tmp749="expecting int but found %s",_tag_dyneither(_tmp749,sizeof(
char),27))),_tag_dyneither(_tmp748,sizeof(void*),1)))))));}{void*_tmp120=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp117);struct Cyc_Absyn_Exp*_tmp123;_LLA0: {struct Cyc_Absyn_DynEither_b_struct*
_tmp121=(struct Cyc_Absyn_DynEither_b_struct*)_tmp120;if(_tmp121->tag != 0)goto
_LLA2;}_LLA1: return t1;_LLA2: {struct Cyc_Absyn_Upper_b_struct*_tmp122=(struct Cyc_Absyn_Upper_b_struct*)
_tmp120;if(_tmp122->tag != 1)goto _LL9F;else{_tmp123=_tmp122->f1;}}_LLA3: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){const char*
_tmp74C;void*_tmp74B;(_tmp74B=0,Cyc_Tcutil_warn(e1->loc,((_tmp74C="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp74C,sizeof(char),70))),_tag_dyneither(_tmp74B,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp756;struct Cyc_Absyn_PtrAtts _tmp755;struct
Cyc_Absyn_PtrInfo _tmp754;struct Cyc_Absyn_PointerType_struct*_tmp753;struct Cyc_Absyn_PointerType_struct*
_tmp126=(_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753[0]=((_tmp756.tag=5,((
_tmp756.f1=((_tmp754.elt_typ=_tmp112,((_tmp754.elt_tq=_tmp113,((_tmp754.ptr_atts=((
_tmp755.rgn=_tmp115,((_tmp755.nullable=Cyc_Absyn_true_conref,((_tmp755.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp755.zero_term=_tmp118,((_tmp755.ptrloc=0,
_tmp755)))))))))),_tmp754)))))),_tmp756)))),_tmp753)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp126,Cyc_Absyn_Other_coercion);return(void*)_tmp126;};_LL9F:;};
_LL9D:;_LL9E: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL9A:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp75B;void*_tmp75A[2];struct Cyc_String_pa_struct _tmp759;struct Cyc_String_pa_struct
_tmp758;(_tmp758.tag=0,((_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp759.tag=0,((_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp75A[0]=& _tmp759,((
_tmp75A[1]=& _tmp758,Cyc_Tcutil_terr(e1->loc,((_tmp75B="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp75B,sizeof(char),59))),_tag_dyneither(_tmp75A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp75E;void*_tmp75D;(
_tmp75D=0,Cyc_Tcutil_terr(e1->loc,((_tmp75E="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp75E,sizeof(char),50))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp761;void*_tmp760;(_tmp760=0,
Cyc_Tcutil_terr(e1->loc,((_tmp761="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp761,sizeof(char),54))),_tag_dyneither(_tmp760,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp766;void*_tmp765[2];struct
Cyc_String_pa_struct _tmp764;struct Cyc_String_pa_struct _tmp763;(_tmp763.tag=0,((
_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp764.tag=0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp765[0]=& _tmp764,((_tmp765[1]=& _tmp763,Cyc_Tcutil_terr(
e2->loc,((_tmp766="expecting either %s or int but found %s",_tag_dyneither(
_tmp766,sizeof(char),40))),_tag_dyneither(_tmp765,sizeof(void*),2)))))))))))));}
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
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp767;struct _tuple0
_tmp138=(_tmp767.f1=Cyc_Tcutil_compress(t1),((_tmp767.f2=Cyc_Tcutil_compress(t2),
_tmp767)));void*_tmp139;struct Cyc_Absyn_PtrInfo _tmp13B;void*_tmp13C;void*_tmp13D;
struct Cyc_Absyn_PtrInfo _tmp13F;void*_tmp140;void*_tmp141;void*_tmp143;_LLA5:
_tmp139=_tmp138.f1;{struct Cyc_Absyn_PointerType_struct*_tmp13A=(struct Cyc_Absyn_PointerType_struct*)
_tmp139;if(_tmp13A->tag != 5)goto _LLA7;else{_tmp13B=_tmp13A->f1;_tmp13C=_tmp13B.elt_typ;}};
_tmp13D=_tmp138.f2;{struct Cyc_Absyn_PointerType_struct*_tmp13E=(struct Cyc_Absyn_PointerType_struct*)
_tmp13D;if(_tmp13E->tag != 5)goto _LLA7;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13F.elt_typ;}};
_LLA6: if(Cyc_Tcutil_unify(_tmp13C,_tmp140))return Cyc_Absyn_sint_typ;goto _LLA4;
_LLA7: _tmp141=_tmp138.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp142=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp141;if(_tmp142->tag != 16)goto _LLA9;};_tmp143=
_tmp138.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp144=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp143;if(_tmp144->tag != 16)goto _LLA9;};_LLA8: return Cyc_Absyn_sint_typ;_LLA9:;
_LLAA: goto _LLA4;_LLA4:;}{const char*_tmp76C;void*_tmp76B[2];struct Cyc_String_pa_struct
_tmp76A;struct Cyc_String_pa_struct _tmp769;(_tmp769.tag=0,((_tmp769.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp76A.tag=
0,((_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp76B[0]=& _tmp76A,((_tmp76B[1]=& _tmp769,Cyc_Tcutil_terr(loc,((_tmp76C="comparison not allowed between %s and %s",
_tag_dyneither(_tmp76C,sizeof(char),41))),_tag_dyneither(_tmp76B,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp76D;
struct _tuple0 _tmp14A=(_tmp76D.f1=t1,((_tmp76D.f2=t2,_tmp76D)));void*_tmp14B;void*
_tmp14D;void*_tmp14E;void*_tmp150;_LLAC: _tmp14B=_tmp14A.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14B;if(_tmp14C->tag != 16)goto
_LLAE;else{_tmp14D=(void*)_tmp14C->f1;}};_tmp14E=_tmp14A.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14E;if(_tmp14F->tag != 16)goto
_LLAE;else{_tmp150=(void*)_tmp14F->f1;}};_LLAD: return Cyc_Absyn_sint_typ;_LLAE:;
_LLAF: goto _LLAB;_LLAB:;}{const char*_tmp772;void*_tmp771[2];struct Cyc_String_pa_struct
_tmp770;struct Cyc_String_pa_struct _tmp76F;(_tmp76F.tag=0,((_tmp76F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp770.tag=
0,((_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp771[0]=& _tmp770,((_tmp771[1]=& _tmp76F,Cyc_Tcutil_terr(loc,((_tmp772="comparison not allowed between %s and %s",
_tag_dyneither(_tmp772,sizeof(char),41))),_tag_dyneither(_tmp771,sizeof(void*),2)))))))))))));}
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
default: _LLC1: {const char*_tmp775;void*_tmp774;(_tmp774=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp775="bad binary primop",
_tag_dyneither(_tmp775,sizeof(char),18))),_tag_dyneither(_tmp774,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLC3: {const char*_tmp778;void*_tmp777;return(_tmp777=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp778="primitive operator has 0 arguments",_tag_dyneither(_tmp778,
sizeof(char),35))),_tag_dyneither(_tmp777,sizeof(void*),0)));}case 1: _LLC4: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC5: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC6: {const char*_tmp77B;void*_tmp77A;return(_tmp77A=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp77B="primitive operator has > 2 arguments",_tag_dyneither(_tmp77B,
sizeof(char),37))),_tag_dyneither(_tmp77A,sizeof(void*),0)));}}return t;};}struct
_tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static int Cyc_Tcexp_check_writable_aggr(
struct Cyc_Position_Segment*loc,void*t){void*_tmp15B=Cyc_Tcutil_compress(t);void*
_tmp15C=_tmp15B;struct Cyc_Absyn_AggrInfo _tmp15E;union Cyc_Absyn_AggrInfoU _tmp15F;
struct Cyc_Absyn_Aggrdecl**_tmp160;struct Cyc_Absyn_Aggrdecl*_tmp161;struct Cyc_List_List*
_tmp163;struct Cyc_Absyn_DatatypeFieldInfo _tmp165;union Cyc_Absyn_DatatypeFieldInfoU
_tmp166;struct _tuple2 _tmp167;struct Cyc_Absyn_Datatypefield*_tmp168;struct Cyc_Absyn_ArrayInfo
_tmp16A;void*_tmp16B;struct Cyc_Absyn_Tqual _tmp16C;struct Cyc_List_List*_tmp16E;
_LLC9: {struct Cyc_Absyn_AggrType_struct*_tmp15D=(struct Cyc_Absyn_AggrType_struct*)
_tmp15C;if(_tmp15D->tag != 12)goto _LLCB;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15E.aggr_info;
if((_tmp15F.KnownAggr).tag != 2)goto _LLCB;_tmp160=(struct Cyc_Absyn_Aggrdecl**)(
_tmp15F.KnownAggr).val;_tmp161=*_tmp160;}}_LLCA: _tmp163=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp161->impl))->fields;goto _LLCC;_LLCB: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp162=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp15C;if(_tmp162->tag != 13)goto
_LLCD;else{_tmp163=_tmp162->f2;}}_LLCC: for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct Cyc_Absyn_Aggrfield*_tmp16F=(struct Cyc_Absyn_Aggrfield*)_tmp163->hd;if((
_tmp16F->tq).real_const){{const char*_tmp77F;void*_tmp77E[1];struct Cyc_String_pa_struct
_tmp77D;(_tmp77D.tag=0,((_tmp77D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp16F->name),((_tmp77E[0]=& _tmp77D,Cyc_Tcutil_terr(loc,((_tmp77F="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp77F,sizeof(char),56))),_tag_dyneither(_tmp77E,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16F->type))return 0;}return 1;
_LLCD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp164=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15C;if(_tmp164->tag != 4)goto _LLCF;else{_tmp165=_tmp164->f1;_tmp166=_tmp165.field_info;
if((_tmp166.KnownDatatypefield).tag != 2)goto _LLCF;_tmp167=(struct _tuple2)(
_tmp166.KnownDatatypefield).val;_tmp168=_tmp167.f2;}}_LLCE:{struct Cyc_List_List*
_tmp173=_tmp168->typs;for(0;_tmp173 != 0;_tmp173=_tmp173->tl){struct _tuple18
_tmp175;struct Cyc_Absyn_Tqual _tmp176;void*_tmp177;struct _tuple18*_tmp174=(struct
_tuple18*)_tmp173->hd;_tmp175=*_tmp174;_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;if(
_tmp176.real_const){{const char*_tmp783;void*_tmp782[1];struct Cyc_String_pa_struct
_tmp781;(_tmp781.tag=0,((_tmp781.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp168->name)),((_tmp782[0]=& _tmp781,Cyc_Tcutil_terr(
loc,((_tmp783="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp783,sizeof(char),64))),_tag_dyneither(_tmp782,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp177))return 0;}}return 1;_LLCF: {
struct Cyc_Absyn_ArrayType_struct*_tmp169=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15C;if(_tmp169->tag != 9)goto _LLD1;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp16A.elt_type;
_tmp16C=_tmp16A.tq;}}_LLD0: if(_tmp16C.real_const){{const char*_tmp786;void*
_tmp785;(_tmp785=0,Cyc_Tcutil_terr(loc,((_tmp786="attempt to over-write a const array",
_tag_dyneither(_tmp786,sizeof(char),36))),_tag_dyneither(_tmp785,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp16B);_LLD1: {struct Cyc_Absyn_TupleType_struct*
_tmp16D=(struct Cyc_Absyn_TupleType_struct*)_tmp15C;if(_tmp16D->tag != 11)goto
_LLD3;else{_tmp16E=_tmp16D->f1;}}_LLD2: for(0;_tmp16E != 0;_tmp16E=_tmp16E->tl){
struct _tuple18 _tmp17E;struct Cyc_Absyn_Tqual _tmp17F;void*_tmp180;struct _tuple18*
_tmp17D=(struct _tuple18*)_tmp16E->hd;_tmp17E=*_tmp17D;_tmp17F=_tmp17E.f1;_tmp180=
_tmp17E.f2;if(_tmp17F.real_const){{const char*_tmp789;void*_tmp788;(_tmp788=0,Cyc_Tcutil_terr(
loc,((_tmp789="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp789,sizeof(char),56))),_tag_dyneither(_tmp788,sizeof(void*),0)));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp180))return 0;}return 1;_LLD3:;
_LLD4: return 1;_LLC8:;}static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v))return;{void*_tmp183=e->r;void*_tmp185;
struct Cyc_Absyn_Vardecl*_tmp187;void*_tmp189;struct Cyc_Absyn_Vardecl*_tmp18B;
void*_tmp18D;struct Cyc_Absyn_Vardecl*_tmp18F;void*_tmp191;struct Cyc_Absyn_Vardecl*
_tmp193;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*
_tmp198;struct _dyneither_ptr*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct
_dyneither_ptr*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;
struct Cyc_Absyn_Exp*_tmp1A2;_LLD6: {struct Cyc_Absyn_Var_e_struct*_tmp184=(struct
Cyc_Absyn_Var_e_struct*)_tmp183;if(_tmp184->tag != 1)goto _LLD8;else{_tmp185=(void*)
_tmp184->f2;{struct Cyc_Absyn_Param_b_struct*_tmp186=(struct Cyc_Absyn_Param_b_struct*)
_tmp185;if(_tmp186->tag != 3)goto _LLD8;else{_tmp187=_tmp186->f1;}};}}_LLD7:
_tmp18B=_tmp187;goto _LLD9;_LLD8: {struct Cyc_Absyn_Var_e_struct*_tmp188=(struct
Cyc_Absyn_Var_e_struct*)_tmp183;if(_tmp188->tag != 1)goto _LLDA;else{_tmp189=(void*)
_tmp188->f2;{struct Cyc_Absyn_Local_b_struct*_tmp18A=(struct Cyc_Absyn_Local_b_struct*)
_tmp189;if(_tmp18A->tag != 4)goto _LLDA;else{_tmp18B=_tmp18A->f1;}};}}_LLD9:
_tmp18F=_tmp18B;goto _LLDB;_LLDA: {struct Cyc_Absyn_Var_e_struct*_tmp18C=(struct
Cyc_Absyn_Var_e_struct*)_tmp183;if(_tmp18C->tag != 1)goto _LLDC;else{_tmp18D=(void*)
_tmp18C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp18E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp18D;if(_tmp18E->tag != 5)goto _LLDC;else{_tmp18F=_tmp18E->f1;}};}}_LLDB:
_tmp193=_tmp18F;goto _LLDD;_LLDC: {struct Cyc_Absyn_Var_e_struct*_tmp190=(struct
Cyc_Absyn_Var_e_struct*)_tmp183;if(_tmp190->tag != 1)goto _LLDE;else{_tmp191=(void*)
_tmp190->f2;{struct Cyc_Absyn_Global_b_struct*_tmp192=(struct Cyc_Absyn_Global_b_struct*)
_tmp191;if(_tmp192->tag != 1)goto _LLDE;else{_tmp193=_tmp192->f1;}};}}_LLDD: if(!(
_tmp193->tq).real_const)return;goto _LLD5;_LLDE: {struct Cyc_Absyn_Subscript_e_struct*
_tmp194=(struct Cyc_Absyn_Subscript_e_struct*)_tmp183;if(_tmp194->tag != 24)goto
_LLE0;else{_tmp195=_tmp194->f1;_tmp196=_tmp194->f2;}}_LLDF:{void*_tmp1A3=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp195->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1A5;struct Cyc_Absyn_Tqual _tmp1A6;struct Cyc_Absyn_ArrayInfo _tmp1A8;struct Cyc_Absyn_Tqual
_tmp1A9;struct Cyc_List_List*_tmp1AB;_LLED: {struct Cyc_Absyn_PointerType_struct*
_tmp1A4=(struct Cyc_Absyn_PointerType_struct*)_tmp1A3;if(_tmp1A4->tag != 5)goto
_LLEF;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A5.elt_tq;}}_LLEE: _tmp1A9=_tmp1A6;
goto _LLF0;_LLEF: {struct Cyc_Absyn_ArrayType_struct*_tmp1A7=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1A3;if(_tmp1A7->tag != 9)goto _LLF1;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A8.tq;}}
_LLF0: if(!_tmp1A9.real_const)return;goto _LLEC;_LLF1: {struct Cyc_Absyn_TupleType_struct*
_tmp1AA=(struct Cyc_Absyn_TupleType_struct*)_tmp1A3;if(_tmp1AA->tag != 11)goto
_LLF3;else{_tmp1AB=_tmp1AA->f1;}}_LLF2: {unsigned int _tmp1AD;int _tmp1AE;struct
_tuple14 _tmp1AC=Cyc_Evexp_eval_const_uint_exp(_tmp196);_tmp1AD=_tmp1AC.f1;
_tmp1AE=_tmp1AC.f2;if(!_tmp1AE){{const char*_tmp78C;void*_tmp78B;(_tmp78B=0,Cyc_Tcutil_terr(
e->loc,((_tmp78C="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp78C,sizeof(char),47))),_tag_dyneither(_tmp78B,sizeof(void*),0)));}return;}{
struct _handler_cons _tmp1B1;_push_handler(& _tmp1B1);{int _tmp1B3=0;if(setjmp(
_tmp1B1.handler))_tmp1B3=1;if(!_tmp1B3){{struct _tuple18 _tmp1B5;struct Cyc_Absyn_Tqual
_tmp1B6;struct _tuple18*_tmp1B4=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp1AB,(int)_tmp1AD);_tmp1B5=*_tmp1B4;_tmp1B6=_tmp1B5.f1;if(!
_tmp1B6.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp1B2=(
void*)_exn_thrown;void*_tmp1B8=_tmp1B2;_LLF6: {struct Cyc_List_Nth_struct*_tmp1B9=(
struct Cyc_List_Nth_struct*)_tmp1B8;if(_tmp1B9->tag != Cyc_List_Nth)goto _LLF8;}
_LLF7: return;_LLF8:;_LLF9:(void)_throw(_tmp1B8);_LLF5:;}};}goto _LLEC;}_LLF3:;
_LLF4: goto _LLEC;_LLEC:;}goto _LLD5;_LLE0: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp197=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp183;if(_tmp197->tag != 22)goto
_LLE2;else{_tmp198=_tmp197->f1;_tmp199=_tmp197->f2;}}_LLE1:{void*_tmp1BA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp198->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp1BC;union Cyc_Absyn_AggrInfoU _tmp1BD;struct Cyc_Absyn_Aggrdecl**_tmp1BE;struct
Cyc_List_List*_tmp1C0;_LLFB: {struct Cyc_Absyn_AggrType_struct*_tmp1BB=(struct Cyc_Absyn_AggrType_struct*)
_tmp1BA;if(_tmp1BB->tag != 12)goto _LLFD;else{_tmp1BC=_tmp1BB->f1;_tmp1BD=_tmp1BC.aggr_info;
if((_tmp1BD.KnownAggr).tag != 2)goto _LLFD;_tmp1BE=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1BD.KnownAggr).val;}}_LLFC: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp1BE == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1BE,_tmp199);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFA;}_LLFD: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1BF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1BA;if(_tmp1BF->tag != 13)goto _LLFF;else{_tmp1C0=_tmp1BF->f2;}}_LLFE: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1C0,_tmp199);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLFA;}_LLFF:;_LL100: goto _LLFA;_LLFA:;}goto _LLD5;_LLE2: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp19A=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp183;if(_tmp19A->tag != 23)goto
_LLE4;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;}}_LLE3:{void*_tmp1C1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp19B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1C3;void*_tmp1C4;_LL102: {struct Cyc_Absyn_PointerType_struct*_tmp1C2=(struct
Cyc_Absyn_PointerType_struct*)_tmp1C1;if(_tmp1C2->tag != 5)goto _LL104;else{
_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C3.elt_typ;}}_LL103:{void*_tmp1C5=Cyc_Tcutil_compress(
_tmp1C4);struct Cyc_Absyn_AggrInfo _tmp1C7;union Cyc_Absyn_AggrInfoU _tmp1C8;struct
Cyc_Absyn_Aggrdecl**_tmp1C9;struct Cyc_List_List*_tmp1CB;_LL107: {struct Cyc_Absyn_AggrType_struct*
_tmp1C6=(struct Cyc_Absyn_AggrType_struct*)_tmp1C5;if(_tmp1C6->tag != 12)goto
_LL109;else{_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C7.aggr_info;if((_tmp1C8.KnownAggr).tag
!= 2)goto _LL109;_tmp1C9=(struct Cyc_Absyn_Aggrdecl**)(_tmp1C8.KnownAggr).val;}}
_LL108: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp1C9 == 0?0:
Cyc_Absyn_lookup_decl_field(*_tmp1C9,_tmp19C);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL106;}_LL109: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1CA=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp1C5;if(_tmp1CA->tag != 13)goto _LL10B;else{
_tmp1CB=_tmp1CA->f2;}}_LL10A: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp1CB,_tmp19C);if(sf == 0  || !(sf->tq).real_const)return;goto _LL106;}_LL10B:;
_LL10C: goto _LL106;_LL106:;}goto _LL101;_LL104:;_LL105: goto _LL101;_LL101:;}goto
_LLD5;_LLE4: {struct Cyc_Absyn_Deref_e_struct*_tmp19D=(struct Cyc_Absyn_Deref_e_struct*)
_tmp183;if(_tmp19D->tag != 21)goto _LLE6;else{_tmp19E=_tmp19D->f1;}}_LLE5:{void*
_tmp1CC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp19E->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1CE;struct Cyc_Absyn_Tqual _tmp1CF;struct Cyc_Absyn_ArrayInfo
_tmp1D1;struct Cyc_Absyn_Tqual _tmp1D2;_LL10E: {struct Cyc_Absyn_PointerType_struct*
_tmp1CD=(struct Cyc_Absyn_PointerType_struct*)_tmp1CC;if(_tmp1CD->tag != 5)goto
_LL110;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CE.elt_tq;}}_LL10F: _tmp1D2=_tmp1CF;
goto _LL111;_LL110: {struct Cyc_Absyn_ArrayType_struct*_tmp1D0=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1CC;if(_tmp1D0->tag != 9)goto _LL112;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D1.tq;}}
_LL111: if(!_tmp1D2.real_const)return;goto _LL10D;_LL112:;_LL113: goto _LL10D;_LL10D:;}
goto _LLD5;_LLE6: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp19F=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp183;if(_tmp19F->tag != 13)goto _LLE8;else{_tmp1A0=_tmp19F->f1;}}_LLE7: _tmp1A2=
_tmp1A0;goto _LLE9;_LLE8: {struct Cyc_Absyn_Instantiate_e_struct*_tmp1A1=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmp183;if(_tmp1A1->tag != 14)goto _LLEA;else{
_tmp1A2=_tmp1A1->f1;}}_LLE9: Cyc_Tcexp_check_writable(te,_tmp1A2);return;_LLEA:;
_LLEB: goto _LLD5;_LLD5:;}{const char*_tmp790;void*_tmp78F[1];struct Cyc_String_pa_struct
_tmp78E;(_tmp78E.tag=0,((_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp78F[0]=& _tmp78E,Cyc_Tcutil_terr(e->loc,((
_tmp790="attempt to write a const location: %s",_tag_dyneither(_tmp790,sizeof(
char),38))),_tag_dyneither(_tmp78F,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){struct _RegionHandle*_tmp1D6=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(_tmp1D6,te),0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp793;void*_tmp792;return(_tmp792=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp793="increment/decrement of non-lvalue",_tag_dyneither(_tmp793,
sizeof(char),34))),_tag_dyneither(_tmp792,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp796;void*_tmp795;(_tmp795=0,Cyc_Tcutil_terr(e->loc,((
_tmp796="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp796,sizeof(char),50))),_tag_dyneither(_tmp795,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp799;void*_tmp798;(_tmp798=0,Cyc_Tcutil_terr(e->loc,((_tmp799="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp799,sizeof(char),54))),_tag_dyneither(_tmp798,sizeof(void*),0)));}}
else{const char*_tmp79D;void*_tmp79C[1];struct Cyc_String_pa_struct _tmp79B;(
_tmp79B.tag=0,((_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp79C[0]=& _tmp79B,Cyc_Tcutil_terr(e->loc,((_tmp79D="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp79D,sizeof(char),44))),_tag_dyneither(_tmp79C,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1E2;int _tmp1E3;const char*_tmp79E;struct Cyc_Tcexp_TestEnv
_tmp1E1=Cyc_Tcexp_tcTest(te,e1,((_tmp79E="conditional expression",_tag_dyneither(
_tmp79E,sizeof(char),23))));_tmp1E2=_tmp1E1.eq;_tmp1E3=_tmp1E1.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp79F;
struct Cyc_List_List _tmp1E4=(_tmp79F.hd=e3,((_tmp79F.tl=0,_tmp79F)));struct Cyc_List_List
_tmp7A0;struct Cyc_List_List _tmp1E5=(_tmp7A0.hd=e2,((_tmp7A0.tl=(struct Cyc_List_List*)&
_tmp1E4,_tmp7A0)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1E5)){{const char*_tmp7A5;void*_tmp7A4[2];struct Cyc_String_pa_struct _tmp7A3;
struct Cyc_String_pa_struct _tmp7A2;(_tmp7A2.tag=0,((_tmp7A2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp7A3.tag=0,((_tmp7A3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp7A4[0]=& _tmp7A3,((_tmp7A4[1]=& _tmp7A2,Cyc_Tcutil_terr(
loc,((_tmp7A5="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp7A5,sizeof(char),48))),_tag_dyneither(_tmp7A4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp7A6;Cyc_Tcexp_tcTest(te,e1,((_tmp7A6="logical-and expression",
_tag_dyneither(_tmp7A6,sizeof(char),23))));}{const char*_tmp7A7;Cyc_Tcexp_tcTest(
te,e2,((_tmp7A7="logical-and expression",_tag_dyneither(_tmp7A7,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp7A8;Cyc_Tcexp_tcTest(te,e1,((_tmp7A8="logical-or expression",
_tag_dyneither(_tmp7A8,sizeof(char),22))));}{const char*_tmp7A9;Cyc_Tcexp_tcTest(
te,e2,((_tmp7A9="logical-or expression",_tag_dyneither(_tmp7A9,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1F0=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(_tmp1F0,Cyc_Tcenv_enter_notreadctxt(
_tmp1F0,te)),0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp1F1=Cyc_Tcutil_compress(t1);_LL115: {struct Cyc_Absyn_ArrayType_struct*
_tmp1F2=(struct Cyc_Absyn_ArrayType_struct*)_tmp1F1;if(_tmp1F2->tag != 9)goto
_LL117;}_LL116:{const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,Cyc_Tcutil_terr(loc,((
_tmp7AC="cannot assign to an array",_tag_dyneither(_tmp7AC,sizeof(char),26))),
_tag_dyneither(_tmp7AB,sizeof(void*),0)));}goto _LL114;_LL117:;_LL118: goto _LL114;
_LL114:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){const
char*_tmp7AF;void*_tmp7AE;(_tmp7AE=0,Cyc_Tcutil_terr(loc,((_tmp7AF="type is abstract (can't determine size).",
_tag_dyneither(_tmp7AF,sizeof(char),41))),_tag_dyneither(_tmp7AE,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7B2;void*_tmp7B1;return(_tmp7B1=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7B2="assignment to non-lvalue",_tag_dyneither(_tmp7B2,sizeof(
char),25))),_tag_dyneither(_tmp7B1,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp1F9=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F9,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F9,e2)){const char*_tmp7B5;void*
_tmp7B4;(_tmp7B4=0,Cyc_Tcutil_terr(e2->loc,((_tmp7B5="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7B5,sizeof(char),49))),_tag_dyneither(_tmp7B4,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp7BA;void*_tmp7B9[2];struct
Cyc_String_pa_struct _tmp7B8;struct Cyc_String_pa_struct _tmp7B7;void*_tmp1FC=(
_tmp7B7.tag=0,((_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7B8.tag=0,((_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7B9[0]=& _tmp7B8,((_tmp7B9[1]=& _tmp7B7,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7BA="type mismatch: %s != %s",_tag_dyneither(_tmp7BA,sizeof(
char),24))),_tag_dyneither(_tmp7B9,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1FC;}}else{enum Cyc_Absyn_Primop
_tmp201=(enum Cyc_Absyn_Primop)po->v;void*_tmp202=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp201,e1,e2);if(!(Cyc_Tcutil_unify(_tmp202,t1) || Cyc_Tcutil_coerceable(
_tmp202) && Cyc_Tcutil_coerceable(t1))){const char*_tmp7BF;void*_tmp7BE[2];struct
Cyc_String_pa_struct _tmp7BD;struct Cyc_String_pa_struct _tmp7BC;void*_tmp203=(
_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7BD.tag=0,((_tmp7BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7BE[0]=& _tmp7BD,((_tmp7BE[1]=& _tmp7BC,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7BF="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp7BF,sizeof(char),82))),_tag_dyneither(_tmp7BE,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp202,t1);Cyc_Tcutil_explain_failure();return _tmp203;}return
_tmp202;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp208=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp20B;enum Cyc_Absyn_Size_of _tmp20D;_LL11A: {struct
Cyc_Absyn_FloatType_struct*_tmp209=(struct Cyc_Absyn_FloatType_struct*)_tmp208;
if(_tmp209->tag != 7)goto _LL11C;}_LL11B: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL119;_LL11C: {struct Cyc_Absyn_IntType_struct*
_tmp20A=(struct Cyc_Absyn_IntType_struct*)_tmp208;if(_tmp20A->tag != 6)goto _LL11E;
else{_tmp20B=_tmp20A->f2;if(_tmp20B != Cyc_Absyn_Char_sz)goto _LL11E;}}_LL11D: goto
_LL11F;_LL11E: {struct Cyc_Absyn_IntType_struct*_tmp20C=(struct Cyc_Absyn_IntType_struct*)
_tmp208;if(_tmp20C->tag != 6)goto _LL120;else{_tmp20D=_tmp20C->f2;if(_tmp20D != Cyc_Absyn_Short_sz)
goto _LL120;}}_LL11F: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL119;_LL120:;_LL121: goto _LL119;_LL119:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp20F;struct Cyc_List_List*
_tmp210;struct Cyc_Position_Segment*_tmp211;enum Cyc_Absyn_Scope _tmp212;struct Cyc_Absyn_Datatypefield
_tmp20E=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp20F=_tmp20E.name;
_tmp210=_tmp20E.typs;_tmp211=_tmp20E.loc;_tmp212=_tmp20E.sc;if(_tmp210 == 0  || 
_tmp210->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple18*)_tmp210->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp214;struct Cyc_List_List*_tmp215;struct Cyc_Position_Segment*
_tmp216;enum Cyc_Absyn_Scope _tmp217;struct Cyc_Absyn_Datatypefield _tmp213=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp214=_tmp213.name;_tmp215=_tmp213.typs;
_tmp216=_tmp213.loc;_tmp217=_tmp213.sc;if(_tmp215 == 0  || _tmp215->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp215->hd)).f2);
int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp7C4;void*_tmp7C3[2];
struct Cyc_String_pa_struct _tmp7C2;struct Cyc_String_pa_struct _tmp7C1;(_tmp7C1.tag=
0,((_tmp7C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7C2.tag=0,((_tmp7C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp7C3[0]=& _tmp7C2,((_tmp7C3[1]=& _tmp7C1,Cyc_Tcutil_terr(
e->loc,((_tmp7C4="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7C4,sizeof(char),54))),_tag_dyneither(_tmp7C3,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp21D=args;int _tmp21E=0;struct _RegionHandle*_tmp21F=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp220=Cyc_Tcenv_new_block(_tmp21F,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp220,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp221=t;struct Cyc_Absyn_PtrInfo _tmp223;void*
_tmp224;struct Cyc_Absyn_Tqual _tmp225;struct Cyc_Absyn_PtrAtts _tmp226;void*_tmp227;
union Cyc_Absyn_Constraint*_tmp228;union Cyc_Absyn_Constraint*_tmp229;union Cyc_Absyn_Constraint*
_tmp22A;_LL123: {struct Cyc_Absyn_PointerType_struct*_tmp222=(struct Cyc_Absyn_PointerType_struct*)
_tmp221;if(_tmp222->tag != 5)goto _LL125;else{_tmp223=_tmp222->f1;_tmp224=_tmp223.elt_typ;
_tmp225=_tmp223.elt_tq;_tmp226=_tmp223.ptr_atts;_tmp227=_tmp226.rgn;_tmp228=
_tmp226.nullable;_tmp229=_tmp226.bounds;_tmp22A=_tmp226.zero_term;}}_LL124: Cyc_Tcenv_check_rgn_accessible(
_tmp220,loc,_tmp227);Cyc_Tcutil_check_nonzero_bound(loc,_tmp229);{void*_tmp22B=
Cyc_Tcutil_compress(_tmp224);struct Cyc_Absyn_FnInfo _tmp22D;struct Cyc_List_List*
_tmp22E;struct Cyc_Core_Opt*_tmp22F;struct Cyc_Absyn_Tqual _tmp230;void*_tmp231;
struct Cyc_List_List*_tmp232;int _tmp233;struct Cyc_Absyn_VarargInfo*_tmp234;struct
Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp236;_LL128: {struct Cyc_Absyn_FnType_struct*
_tmp22C=(struct Cyc_Absyn_FnType_struct*)_tmp22B;if(_tmp22C->tag != 10)goto _LL12A;
else{_tmp22D=_tmp22C->f1;_tmp22E=_tmp22D.tvars;_tmp22F=_tmp22D.effect;_tmp230=
_tmp22D.ret_tqual;_tmp231=_tmp22D.ret_typ;_tmp232=_tmp22D.args;_tmp233=_tmp22D.c_varargs;
_tmp234=_tmp22D.cyc_varargs;_tmp235=_tmp22D.rgn_po;_tmp236=_tmp22D.attributes;}}
_LL129: if(topt != 0)Cyc_Tcutil_unify(_tmp231,*topt);while(_tmp21D != 0  && _tmp232
!= 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21D->hd;
void*t2=(*((struct _tuple8*)_tmp232->hd)).f3;Cyc_Tcexp_tcExp(_tmp220,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp220,e1,t2,& alias_coercion)){{const char*_tmp7C9;
void*_tmp7C8[2];struct Cyc_String_pa_struct _tmp7C7;struct Cyc_String_pa_struct
_tmp7C6;(_tmp7C6.tag=0,((_tmp7C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7C7.tag=0,((_tmp7C7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp7C8[0]=& _tmp7C7,((_tmp7C8[1]=& _tmp7C6,Cyc_Tcutil_terr(
e1->loc,((_tmp7C9="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7C9,sizeof(char),57))),_tag_dyneither(_tmp7C8,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7CA;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7CA=_cycalloc(sizeof(*_tmp7CA)),((_tmp7CA->hd=(void*)_tmp21E,((_tmp7CA->tl=*
alias_arg_exps,_tmp7CA))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp21F,
t2) && !Cyc_Tcutil_is_noalias_path(_tmp21F,e1)){const char*_tmp7CD;void*_tmp7CC;(
_tmp7CC=0,Cyc_Tcutil_terr(e1->loc,((_tmp7CD="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7CD,sizeof(char),49))),_tag_dyneither(_tmp7CC,sizeof(void*),0)));}
_tmp21D=_tmp21D->tl;_tmp232=_tmp232->tl;++ _tmp21E;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp236;for(0;a != 0;a=a->tl){void*_tmp23E=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp240;int _tmp241;int _tmp242;_LL12D: {struct Cyc_Absyn_Format_att_struct*
_tmp23F=(struct Cyc_Absyn_Format_att_struct*)_tmp23E;if(_tmp23F->tag != 19)goto
_LL12F;else{_tmp240=_tmp23F->f1;_tmp241=_tmp23F->f2;_tmp242=_tmp23F->f3;}}_LL12E:{
struct _handler_cons _tmp243;_push_handler(& _tmp243);{int _tmp245=0;if(setjmp(
_tmp243.handler))_tmp245=1;if(!_tmp245){{struct Cyc_Absyn_Exp*_tmp246=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp241 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp242 == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp7CE;fmt_args=((
_tmp7CE=_cycalloc(sizeof(*_tmp7CE)),((_tmp7CE->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp242 - 1),_tmp7CE))));}
args_already_checked=1;{struct _RegionHandle*_tmp248=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp240){case Cyc_Absyn_Printf_ft: _LL131: Cyc_Tcexp_check_format_args(
_tmp220,_tmp246,fmt_args,_tmp242 - 1,alias_arg_exps,_tmp248,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL132: Cyc_Tcexp_check_format_args(_tmp220,_tmp246,
fmt_args,_tmp242 - 1,alias_arg_exps,_tmp248,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp244=(void*)_exn_thrown;void*_tmp24A=_tmp244;_LL135: {
struct Cyc_List_Nth_struct*_tmp24B=(struct Cyc_List_Nth_struct*)_tmp24A;if(_tmp24B->tag
!= Cyc_List_Nth)goto _LL137;}_LL136:{const char*_tmp7D1;void*_tmp7D0;(_tmp7D0=0,
Cyc_Tcutil_terr(loc,((_tmp7D1="bad format arguments",_tag_dyneither(_tmp7D1,
sizeof(char),21))),_tag_dyneither(_tmp7D0,sizeof(void*),0)));}goto _LL134;_LL137:;
_LL138:(void)_throw(_tmp24A);_LL134:;}};}goto _LL12C;_LL12F:;_LL130: goto _LL12C;
_LL12C:;}}if(_tmp232 != 0){const char*_tmp7D4;void*_tmp7D3;(_tmp7D3=0,Cyc_Tcutil_terr(
loc,((_tmp7D4="too few arguments for function",_tag_dyneither(_tmp7D4,sizeof(
char),31))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));}else{if((_tmp21D != 0  || 
_tmp233) || _tmp234 != 0){if(_tmp233)for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){Cyc_Tcexp_tcExp(
_tmp220,0,(struct Cyc_Absyn_Exp*)_tmp21D->hd);}else{if(_tmp234 == 0){const char*
_tmp7D7;void*_tmp7D6;(_tmp7D6=0,Cyc_Tcutil_terr(loc,((_tmp7D7="too many arguments for function",
_tag_dyneither(_tmp7D7,sizeof(char),32))),_tag_dyneither(_tmp7D6,sizeof(void*),0)));}
else{void*_tmp253;int _tmp254;struct Cyc_Absyn_VarargInfo _tmp252=*_tmp234;_tmp253=
_tmp252.type;_tmp254=_tmp252.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp7D8;
struct Cyc_Absyn_VarargCallInfo*_tmp255=(_tmp7D8=_cycalloc(sizeof(*_tmp7D8)),((
_tmp7D8->num_varargs=0,((_tmp7D8->injectors=0,((_tmp7D8->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp234,_tmp7D8)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp255;if(!_tmp254)for(0;_tmp21D != 0;(_tmp21D=_tmp21D->tl,_tmp21E ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21D->hd;++
_tmp255->num_varargs;Cyc_Tcexp_tcExp(_tmp220,(void**)& _tmp253,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp220,e1,_tmp253,& alias_coercion)){{const char*_tmp7DD;void*_tmp7DC[2];struct
Cyc_String_pa_struct _tmp7DB;struct Cyc_String_pa_struct _tmp7DA;(_tmp7DA.tag=0,((
_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp7DB.tag=0,((
_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp253)),((_tmp7DC[0]=& _tmp7DB,((_tmp7DC[1]=& _tmp7DA,Cyc_Tcutil_terr(loc,((
_tmp7DD="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp7DD,sizeof(char),49))),_tag_dyneither(_tmp7DC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp7DE;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp7DE=_cycalloc(sizeof(*_tmp7DE)),((
_tmp7DE->hd=(void*)_tmp21E,((_tmp7DE->tl=*alias_arg_exps,_tmp7DE))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp21F,_tmp253) && !Cyc_Tcutil_is_noalias_path(_tmp21F,e1)){const char*_tmp7E1;
void*_tmp7E0;(_tmp7E0=0,Cyc_Tcutil_terr(e1->loc,((_tmp7E1="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7E1,sizeof(char),49))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));}}
else{void*_tmp25D=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp253));
struct Cyc_Absyn_DatatypeInfo _tmp25F;union Cyc_Absyn_DatatypeInfoU _tmp260;struct
Cyc_Absyn_Datatypedecl**_tmp261;struct Cyc_Absyn_Datatypedecl*_tmp262;struct Cyc_List_List*
_tmp263;_LL13A: {struct Cyc_Absyn_DatatypeType_struct*_tmp25E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp25D;if(_tmp25E->tag != 3)goto _LL13C;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25F.datatype_info;
if((_tmp260.KnownDatatype).tag != 2)goto _LL13C;_tmp261=(struct Cyc_Absyn_Datatypedecl**)(
_tmp260.KnownDatatype).val;_tmp262=*_tmp261;_tmp263=_tmp25F.targs;}}_LL13B: {
struct Cyc_Absyn_Datatypedecl*_tmp264=*Cyc_Tcenv_lookup_datatypedecl(_tmp220,loc,
_tmp262->name);struct Cyc_List_List*fields=0;if(_tmp264->fields == 0){const char*
_tmp7E5;void*_tmp7E4[1];struct Cyc_String_pa_struct _tmp7E3;(_tmp7E3.tag=0,((
_tmp7E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp253)),((_tmp7E4[0]=& _tmp7E3,Cyc_Tcutil_terr(loc,((_tmp7E5="can't inject into abstract datatype %s",
_tag_dyneither(_tmp7E5,sizeof(char),39))),_tag_dyneither(_tmp7E4,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp264->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp253),Cyc_Tcenv_curr_rgn(
_tmp220))){const char*_tmp7EA;void*_tmp7E9[2];struct Cyc_String_pa_struct _tmp7E8;
struct Cyc_String_pa_struct _tmp7E7;(_tmp7E7.tag=0,((_tmp7E7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp220))),((_tmp7E8.tag=0,((_tmp7E8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp253)),((_tmp7E9[0]=& _tmp7E8,((_tmp7E9[
1]=& _tmp7E7,Cyc_Tcutil_terr(loc,((_tmp7EA="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp7EA,sizeof(char),49))),_tag_dyneither(_tmp7E9,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp26C=Cyc_Tcenv_get_fnrgn(_tmp220);{struct Cyc_List_List*
_tmp26D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp26C,_tmp26C,
_tmp264->tvs,_tmp263);for(0;_tmp21D != 0;_tmp21D=_tmp21D->tl){++ _tmp255->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21D->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp220,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp26C,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp26C,e1)){const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7ED="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7ED,sizeof(char),49))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp270=Cyc_Tcexp_tcInjection(_tmp220,e1,Cyc_Tcutil_pointer_elt_type(
_tmp253),_tmp26C,_tmp26D,fields);if(_tmp270 != 0){struct Cyc_List_List*_tmp7EE;
_tmp255->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp255->injectors,((_tmp7EE=_cycalloc(sizeof(*_tmp7EE)),((
_tmp7EE->hd=(struct Cyc_Absyn_Datatypefield*)_tmp270,((_tmp7EE->tl=0,_tmp7EE)))))));}};};}}
goto _LL139;};}_LL13C:;_LL13D:{const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_Tcutil_terr(
loc,((_tmp7F1="bad inject vararg type",_tag_dyneither(_tmp7F1,sizeof(char),23))),
_tag_dyneither(_tmp7F0,sizeof(void*),0)));}goto _LL139;_LL139:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp220,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp22F))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp220,loc,_tmp235);return _tmp231;};_LL12A:;_LL12B: {const char*_tmp7F4;void*
_tmp7F3;return(_tmp7F3=0,Cyc_Tcexp_expr_err(_tmp220,loc,topt,((_tmp7F4="expected pointer to function",
_tag_dyneither(_tmp7F4,sizeof(char),29))),_tag_dyneither(_tmp7F3,sizeof(void*),0)));}
_LL127:;};_LL125:;_LL126: {const char*_tmp7F7;void*_tmp7F6;return(_tmp7F6=0,Cyc_Tcexp_expr_err(
_tmp220,loc,topt,((_tmp7F7="expected pointer to function",_tag_dyneither(_tmp7F7,
sizeof(char),29))),_tag_dyneither(_tmp7F6,sizeof(void*),0)));}_LL122:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp279=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp279,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp279,&
bogus)){const char*_tmp7FC;void*_tmp7FB[2];struct Cyc_String_pa_struct _tmp7FA;
struct Cyc_String_pa_struct _tmp7F9;(_tmp7F9.tag=0,((_tmp7F9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp279)),((_tmp7FB[0]=& _tmp7FA,((
_tmp7FB[1]=& _tmp7F9,Cyc_Tcutil_terr(loc,((_tmp7FC="expected %s but found %s",
_tag_dyneither(_tmp7FC,sizeof(char),25))),_tag_dyneither(_tmp7FB,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp27E=t1;struct Cyc_Absyn_PtrInfo _tmp280;void*
_tmp281;struct Cyc_Absyn_Tqual _tmp282;struct Cyc_Absyn_PtrAtts _tmp283;void*_tmp284;
union Cyc_Absyn_Constraint*_tmp285;union Cyc_Absyn_Constraint*_tmp286;union Cyc_Absyn_Constraint*
_tmp287;_LL13F: {struct Cyc_Absyn_PointerType_struct*_tmp27F=(struct Cyc_Absyn_PointerType_struct*)
_tmp27E;if(_tmp27F->tag != 5)goto _LL141;else{_tmp280=_tmp27F->f1;_tmp281=_tmp280.elt_typ;
_tmp282=_tmp280.elt_tq;_tmp283=_tmp280.ptr_atts;_tmp284=_tmp283.rgn;_tmp285=
_tmp283.nullable;_tmp286=_tmp283.bounds;_tmp287=_tmp283.zero_term;}}_LL140:{void*
_tmp288=Cyc_Tcutil_compress(_tmp281);struct Cyc_Absyn_FnInfo _tmp28A;struct Cyc_List_List*
_tmp28B;struct Cyc_Core_Opt*_tmp28C;struct Cyc_Absyn_Tqual _tmp28D;void*_tmp28E;
struct Cyc_List_List*_tmp28F;int _tmp290;struct Cyc_Absyn_VarargInfo*_tmp291;struct
Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp293;_LL144: {struct Cyc_Absyn_FnType_struct*
_tmp289=(struct Cyc_Absyn_FnType_struct*)_tmp288;if(_tmp289->tag != 10)goto _LL146;
else{_tmp28A=_tmp289->f1;_tmp28B=_tmp28A.tvars;_tmp28C=_tmp28A.effect;_tmp28D=
_tmp28A.ret_tqual;_tmp28E=_tmp28A.ret_typ;_tmp28F=_tmp28A.args;_tmp290=_tmp28A.c_varargs;
_tmp291=_tmp28A.cyc_varargs;_tmp292=_tmp28A.rgn_po;_tmp293=_tmp28A.attributes;}}
_LL145: {struct _RegionHandle*_tmp294=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*
instantiation=0;for(0;ts != 0  && _tmp28B != 0;(ts=ts->tl,_tmp28B=_tmp28B->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp28B->hd,&
Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(
void*)ts->hd);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{struct _tuple12*
_tmp7FF;struct Cyc_List_List*_tmp7FE;instantiation=((_tmp7FE=_region_malloc(
_tmp294,sizeof(*_tmp7FE)),((_tmp7FE->hd=((_tmp7FF=_region_malloc(_tmp294,sizeof(*
_tmp7FF)),((_tmp7FF->f1=(struct Cyc_Absyn_Tvar*)_tmp28B->hd,((_tmp7FF->f2=(void*)
ts->hd,_tmp7FF)))))),((_tmp7FE->tl=instantiation,_tmp7FE))))));};}if(ts != 0){
const char*_tmp802;void*_tmp801;return(_tmp801=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp802="too many type variables in instantiation",_tag_dyneither(_tmp802,
sizeof(char),41))),_tag_dyneither(_tmp801,sizeof(void*),0)));}{struct Cyc_Absyn_FnType_struct
_tmp808;struct Cyc_Absyn_FnInfo _tmp807;struct Cyc_Absyn_FnType_struct*_tmp806;void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp294,instantiation,(void*)((_tmp806=
_cycalloc(sizeof(*_tmp806)),((_tmp806[0]=((_tmp808.tag=10,((_tmp808.f1=((_tmp807.tvars=
_tmp28B,((_tmp807.effect=_tmp28C,((_tmp807.ret_tqual=_tmp28D,((_tmp807.ret_typ=
_tmp28E,((_tmp807.args=_tmp28F,((_tmp807.c_varargs=_tmp290,((_tmp807.cyc_varargs=
_tmp291,((_tmp807.rgn_po=_tmp292,((_tmp807.attributes=_tmp293,_tmp807)))))))))))))))))),
_tmp808)))),_tmp806)))));struct Cyc_Absyn_PointerType_struct _tmp812;struct Cyc_Absyn_PtrAtts
_tmp811;struct Cyc_Absyn_PtrInfo _tmp810;struct Cyc_Absyn_PointerType_struct*
_tmp80F;return(void*)((_tmp80F=_cycalloc(sizeof(*_tmp80F)),((_tmp80F[0]=((
_tmp812.tag=5,((_tmp812.f1=((_tmp810.elt_typ=new_fn_typ,((_tmp810.elt_tq=_tmp282,((
_tmp810.ptr_atts=((_tmp811.rgn=_tmp284,((_tmp811.nullable=_tmp285,((_tmp811.bounds=
_tmp286,((_tmp811.zero_term=_tmp287,((_tmp811.ptrloc=0,_tmp811)))))))))),_tmp810)))))),
_tmp812)))),_tmp80F))));};}_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;
_LL142: goto _LL13E;_LL13E:;}{const char*_tmp816;void*_tmp815[1];struct Cyc_String_pa_struct
_tmp814;return(_tmp814.tag=0,((_tmp814.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp815[0]=& _tmp814,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp816="expecting polymorphic type but found %s",_tag_dyneither(
_tmp816,sizeof(char),40))),_tag_dyneither(_tmp815,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(
loc,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp81B;void*_tmp81A[2];struct Cyc_String_pa_struct _tmp819;struct Cyc_String_pa_struct
_tmp818;void*_tmp2A3=(_tmp818.tag=0,((_tmp818.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp819.tag=0,((_tmp819.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp81A[0]=&
_tmp819,((_tmp81A[1]=& _tmp818,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp81B="cannot cast %s to %s",
_tag_dyneither(_tmp81B,sizeof(char),21))),_tag_dyneither(_tmp81A,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp2A3;};}}}{struct _tuple0 _tmp81C;struct
_tuple0 _tmp2A9=(_tmp81C.f1=e->r,((_tmp81C.f2=Cyc_Tcutil_compress(t),_tmp81C)));
void*_tmp2AA;struct Cyc_Absyn_MallocInfo _tmp2AC;int _tmp2AD;void*_tmp2AE;struct Cyc_Absyn_PtrInfo
_tmp2B0;struct Cyc_Absyn_PtrAtts _tmp2B1;union Cyc_Absyn_Constraint*_tmp2B2;union
Cyc_Absyn_Constraint*_tmp2B3;union Cyc_Absyn_Constraint*_tmp2B4;_LL149: _tmp2AA=
_tmp2A9.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp2AB=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2AA;if(_tmp2AB->tag != 34)goto _LL14B;else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AC.fat_result;}};
_tmp2AE=_tmp2A9.f2;{struct Cyc_Absyn_PointerType_struct*_tmp2AF=(struct Cyc_Absyn_PointerType_struct*)
_tmp2AE;if(_tmp2AF->tag != 5)goto _LL14B;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2B0.ptr_atts;
_tmp2B2=_tmp2B1.nullable;_tmp2B3=_tmp2B1.bounds;_tmp2B4=_tmp2B1.zero_term;}};
_LL14A: if((_tmp2AD  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B4)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B2)){void*_tmp2B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2B3);struct Cyc_Absyn_Exp*_tmp2B7;_LL14E: {struct Cyc_Absyn_Upper_b_struct*
_tmp2B6=(struct Cyc_Absyn_Upper_b_struct*)_tmp2B5;if(_tmp2B6->tag != 1)goto _LL150;
else{_tmp2B7=_tmp2B6->f1;}}_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp2B7)).f1
== 1){const char*_tmp81F;void*_tmp81E;(_tmp81E=0,Cyc_Tcutil_warn(loc,((_tmp81F="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp81F,sizeof(char),60))),_tag_dyneither(_tmp81E,sizeof(void*),0)));}
goto _LL14D;_LL150:;_LL151: goto _LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto
_LL148;_LL148:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp2BA=0;struct Cyc_Absyn_Tqual _tmp2BB=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp2BC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2BE;
void*_tmp2BF;struct Cyc_Absyn_Tqual _tmp2C0;struct Cyc_Absyn_PtrAtts _tmp2C1;union
Cyc_Absyn_Constraint*_tmp2C2;_LL153: {struct Cyc_Absyn_PointerType_struct*_tmp2BD=(
struct Cyc_Absyn_PointerType_struct*)_tmp2BC;if(_tmp2BD->tag != 5)goto _LL155;else{
_tmp2BE=_tmp2BD->f1;_tmp2BF=_tmp2BE.elt_typ;_tmp2C0=_tmp2BE.elt_tq;_tmp2C1=
_tmp2BE.ptr_atts;_tmp2C2=_tmp2C1.zero_term;}}_LL154:{void**_tmp820;_tmp2BA=((
_tmp820=_cycalloc(sizeof(*_tmp820)),((_tmp820[0]=_tmp2BF,_tmp820))));}_tmp2BB=
_tmp2C0;goto _LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{struct _RegionHandle*
_tmp2C4=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(
_tmp2C4,Cyc_Tcenv_clear_notreadctxt(_tmp2C4,te)),_tmp2BA,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp2C4,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*
_tmp823;void*_tmp822;(_tmp822=0,Cyc_Tcutil_terr(e->loc,((_tmp823="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp823,sizeof(char),46))),_tag_dyneither(_tmp822,sizeof(void*),0)));}{
void*_tmp2C7=e->r;struct Cyc_Absyn_Exp*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2CA;_LL158: {
struct Cyc_Absyn_Subscript_e_struct*_tmp2C8=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp2C7;if(_tmp2C8->tag != 24)goto _LL15A;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;}}
_LL159:{void*_tmp2CB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C9->topt))->v);_LL15D: {struct Cyc_Absyn_TupleType_struct*
_tmp2CC=(struct Cyc_Absyn_TupleType_struct*)_tmp2CB;if(_tmp2CC->tag != 11)goto
_LL15F;}_LL15E: goto _LL15C;_LL15F:;_LL160: e0->r=(Cyc_Absyn_add_exp(_tmp2C9,
_tmp2CA,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp2C9,_tmp2CA);_LL15C:;}goto _LL157;
_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*_tmp2CD=e->r;int _tmp2CF;int _tmp2D1;
_LL162: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2CE=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2CD;if(_tmp2CE->tag != 22)goto _LL164;else{_tmp2CF=_tmp2CE->f3;if(_tmp2CF != 1)
goto _LL164;}}_LL163: goto _LL165;_LL164: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2D0=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2CD;if(_tmp2D0->tag != 23)goto
_LL166;else{_tmp2D1=_tmp2D0->f3;if(_tmp2D1 != 1)goto _LL166;}}_LL165:{const char*
_tmp826;void*_tmp825;(_tmp825=0,Cyc_Tcutil_terr(e->loc,((_tmp826="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp826,sizeof(char),50))),_tag_dyneither(_tmp825,sizeof(void*),0)));}
goto _LL161;_LL166:;_LL167: goto _LL161;_LL161:;}{int _tmp2D5;void*_tmp2D6;struct
_tuple13 _tmp2D4=Cyc_Tcutil_addressof_props(te,e);_tmp2D5=_tmp2D4.f1;_tmp2D6=
_tmp2D4.f2;if(Cyc_Tcutil_is_noalias_region(_tmp2D6,0)){const char*_tmp829;void*
_tmp828;(_tmp828=0,Cyc_Tcutil_terr(e->loc,((_tmp829="using & would manufacture an alias to an alias-free pointer",
_tag_dyneither(_tmp829,sizeof(char),60))),_tag_dyneither(_tmp828,sizeof(void*),0)));}{
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2D5){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp2D6,tq,Cyc_Absyn_false_conref);return t;};};};};}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)
return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp82D;void*_tmp82C[1];struct Cyc_String_pa_struct _tmp82B;(_tmp82B.tag=
0,((_tmp82B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp82C[0]=& _tmp82B,Cyc_Tcutil_terr(loc,((_tmp82D="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp82D,sizeof(char),55))),_tag_dyneither(_tmp82C,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2DC=Cyc_Tcutil_compress(*topt);void*_tmp2DE;_LL169: {
struct Cyc_Absyn_TagType_struct*_tmp2DD=(struct Cyc_Absyn_TagType_struct*)_tmp2DC;
if(_tmp2DD->tag != 20)goto _LL16B;else{_tmp2DE=(void*)_tmp2DD->f1;}}_LL16A: {
struct Cyc_Absyn_Exp*_tmp2DF=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp830;struct Cyc_Absyn_ValueofType_struct*_tmp82F;struct Cyc_Absyn_ValueofType_struct*
_tmp2E0=(_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F[0]=((_tmp830.tag=19,((
_tmp830.f1=_tmp2DF,_tmp830)))),_tmp82F)));if(Cyc_Tcutil_unify(_tmp2DE,(void*)
_tmp2E0))return _tmp2DC;goto _LL168;}_LL16B:;_LL16C: goto _LL168;_LL168:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);{void*_tmp2E3=n;struct
_dyneither_ptr*_tmp2E5;unsigned int _tmp2E7;_LL16E: {struct Cyc_Absyn_StructField_struct*
_tmp2E4=(struct Cyc_Absyn_StructField_struct*)_tmp2E3;if(_tmp2E4->tag != 0)goto
_LL170;else{_tmp2E5=_tmp2E4->f1;}}_LL16F: {int bad_type=1;{void*_tmp2E8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp2EA;union Cyc_Absyn_AggrInfoU _tmp2EB;struct Cyc_Absyn_Aggrdecl**
_tmp2EC;struct Cyc_List_List*_tmp2EE;_LL173: {struct Cyc_Absyn_AggrType_struct*
_tmp2E9=(struct Cyc_Absyn_AggrType_struct*)_tmp2E8;if(_tmp2E9->tag != 12)goto
_LL175;else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2EA.aggr_info;if((_tmp2EB.KnownAggr).tag
!= 2)goto _LL175;_tmp2EC=(struct Cyc_Absyn_Aggrdecl**)(_tmp2EB.KnownAggr).val;}}
_LL174: if((*_tmp2EC)->impl == 0)goto _LL172;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2E5,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2EC)->impl))->fields)){const char*_tmp834;void*_tmp833[1];
struct Cyc_String_pa_struct _tmp832;(_tmp832.tag=0,((_tmp832.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2E5),((_tmp833[0]=& _tmp832,Cyc_Tcutil_terr(
loc,((_tmp834="no field of struct/union has name %s",_tag_dyneither(_tmp834,
sizeof(char),37))),_tag_dyneither(_tmp833,sizeof(void*),1)))))));}bad_type=0;
goto _LL172;_LL175: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2ED=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2E8;if(_tmp2ED->tag != 13)goto _LL177;else{_tmp2EE=_tmp2ED->f2;}}_LL176: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2E5,_tmp2EE)){const char*_tmp838;void*_tmp837[1];struct Cyc_String_pa_struct
_tmp836;(_tmp836.tag=0,((_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2E5),((_tmp837[0]=& _tmp836,Cyc_Tcutil_terr(loc,((_tmp838="no field of struct/union has name %s",
_tag_dyneither(_tmp838,sizeof(char),37))),_tag_dyneither(_tmp837,sizeof(void*),1)))))));}
bad_type=0;goto _LL172;_LL177:;_LL178: goto _LL172;_LL172:;}if(bad_type){const char*
_tmp83C;void*_tmp83B[1];struct Cyc_String_pa_struct _tmp83A;(_tmp83A.tag=0,((
_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp83B[0]=& _tmp83A,Cyc_Tcutil_terr(loc,((_tmp83C="%s is not a known struct/union type",
_tag_dyneither(_tmp83C,sizeof(char),36))),_tag_dyneither(_tmp83B,sizeof(void*),1)))))));}
goto _LL16D;}_LL170: {struct Cyc_Absyn_TupleIndex_struct*_tmp2E6=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E3;if(_tmp2E6->tag != 1)goto _LL16D;else{_tmp2E7=_tmp2E6->f1;}}_LL171: {int
bad_type=1;{void*_tmp2F8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2FA;
union Cyc_Absyn_AggrInfoU _tmp2FB;struct Cyc_Absyn_Aggrdecl**_tmp2FC;struct Cyc_List_List*
_tmp2FE;struct Cyc_List_List*_tmp300;struct Cyc_Absyn_DatatypeFieldInfo _tmp302;
union Cyc_Absyn_DatatypeFieldInfoU _tmp303;struct _tuple2 _tmp304;struct Cyc_Absyn_Datatypefield*
_tmp305;_LL17A: {struct Cyc_Absyn_AggrType_struct*_tmp2F9=(struct Cyc_Absyn_AggrType_struct*)
_tmp2F8;if(_tmp2F9->tag != 12)goto _LL17C;else{_tmp2FA=_tmp2F9->f1;_tmp2FB=_tmp2FA.aggr_info;
if((_tmp2FB.KnownAggr).tag != 2)goto _LL17C;_tmp2FC=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2FB.KnownAggr).val;}}_LL17B: if((*_tmp2FC)->impl == 0)goto _LL179;_tmp2FE=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2FC)->impl))->fields;goto _LL17D;
_LL17C: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2FD=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F8;if(_tmp2FD->tag != 13)goto _LL17E;else{_tmp2FE=_tmp2FD->f2;}}_LL17D: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FE)<= _tmp2E7){const char*
_tmp841;void*_tmp840[2];struct Cyc_Int_pa_struct _tmp83F;struct Cyc_Int_pa_struct
_tmp83E;(_tmp83E.tag=1,((_tmp83E.f1=(unsigned long)((int)_tmp2E7),((_tmp83F.tag=
1,((_tmp83F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FE),((_tmp840[0]=& _tmp83F,((_tmp840[1]=& _tmp83E,Cyc_Tcutil_terr(loc,((
_tmp841="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp841,
sizeof(char),46))),_tag_dyneither(_tmp840,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL179;_LL17E: {struct Cyc_Absyn_TupleType_struct*_tmp2FF=(struct Cyc_Absyn_TupleType_struct*)
_tmp2F8;if(_tmp2FF->tag != 11)goto _LL180;else{_tmp300=_tmp2FF->f1;}}_LL17F: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp300)<= _tmp2E7){const char*
_tmp846;void*_tmp845[2];struct Cyc_Int_pa_struct _tmp844;struct Cyc_Int_pa_struct
_tmp843;(_tmp843.tag=1,((_tmp843.f1=(unsigned long)((int)_tmp2E7),((_tmp844.tag=
1,((_tmp844.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp300),((_tmp845[0]=& _tmp844,((_tmp845[1]=& _tmp843,Cyc_Tcutil_terr(loc,((
_tmp846="tuple has too few components: %d <= %d",_tag_dyneither(_tmp846,sizeof(
char),39))),_tag_dyneither(_tmp845,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL179;_LL180: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp301=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2F8;if(_tmp301->tag != 4)goto _LL182;else{_tmp302=_tmp301->f1;_tmp303=_tmp302.field_info;
if((_tmp303.KnownDatatypefield).tag != 2)goto _LL182;_tmp304=(struct _tuple2)(
_tmp303.KnownDatatypefield).val;_tmp305=_tmp304.f2;}}_LL181: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp305->typs)< _tmp2E7){const char*_tmp84B;
void*_tmp84A[2];struct Cyc_Int_pa_struct _tmp849;struct Cyc_Int_pa_struct _tmp848;(
_tmp848.tag=1,((_tmp848.f1=(unsigned long)((int)_tmp2E7),((_tmp849.tag=1,((
_tmp849.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp305->typs),((_tmp84A[0]=& _tmp849,((_tmp84A[1]=& _tmp848,Cyc_Tcutil_terr(loc,((
_tmp84B="datatype field has too few components: %d < %d",_tag_dyneither(_tmp84B,
sizeof(char),47))),_tag_dyneither(_tmp84A,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL179;_LL182:;_LL183: goto _LL179;_LL179:;}if(bad_type){const char*_tmp84F;
void*_tmp84E[1];struct Cyc_String_pa_struct _tmp84D;(_tmp84D.tag=0,((_tmp84D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp84E[
0]=& _tmp84D,Cyc_Tcutil_terr(loc,((_tmp84F="%s is not a known type",
_tag_dyneither(_tmp84F,sizeof(char),23))),_tag_dyneither(_tmp84E,sizeof(void*),1)))))));}
goto _LL16D;}_LL16D:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp315=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp316=Cyc_Tcenv_clear_lhs(_tmp315,Cyc_Tcenv_clear_notreadctxt(_tmp315,te_orig));
Cyc_Tcexp_tcExp(_tmp316,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp317=t;struct Cyc_Absyn_PtrInfo _tmp31A;void*
_tmp31B;struct Cyc_Absyn_PtrAtts _tmp31C;void*_tmp31D;union Cyc_Absyn_Constraint*
_tmp31E;union Cyc_Absyn_Constraint*_tmp31F;_LL185: {struct Cyc_Absyn_Evar_struct*
_tmp318=(struct Cyc_Absyn_Evar_struct*)_tmp317;if(_tmp318->tag != 1)goto _LL187;}
_LL186: {struct Cyc_List_List*_tmp320=Cyc_Tcenv_lookup_type_vars(_tmp316);struct
Cyc_Core_Opt*_tmp850;void*_tmp321=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((
_tmp850=_cycalloc(sizeof(*_tmp850)),((_tmp850->v=_tmp320,_tmp850)))));struct Cyc_Core_Opt*
_tmp851;void*_tmp322=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp851=_cycalloc(sizeof(*_tmp851)),((_tmp851->v=_tmp320,_tmp851)))));union Cyc_Absyn_Constraint*
_tmp323=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*
_tmp324=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp852;struct Cyc_Absyn_PtrAtts _tmp325=(_tmp852.rgn=_tmp322,((_tmp852.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp852.bounds=
_tmp323,((_tmp852.zero_term=_tmp324,((_tmp852.ptrloc=0,_tmp852)))))))));struct
Cyc_Absyn_PointerType_struct _tmp858;struct Cyc_Absyn_PtrInfo _tmp857;struct Cyc_Absyn_PointerType_struct*
_tmp856;struct Cyc_Absyn_PointerType_struct*_tmp326=(_tmp856=_cycalloc(sizeof(*
_tmp856)),((_tmp856[0]=((_tmp858.tag=5,((_tmp858.f1=((_tmp857.elt_typ=_tmp321,((
_tmp857.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp857.ptr_atts=_tmp325,_tmp857)))))),
_tmp858)))),_tmp856)));Cyc_Tcutil_unify(t,(void*)_tmp326);_tmp31B=_tmp321;
_tmp31D=_tmp322;_tmp31E=_tmp323;_tmp31F=_tmp324;goto _LL188;}_LL187: {struct Cyc_Absyn_PointerType_struct*
_tmp319=(struct Cyc_Absyn_PointerType_struct*)_tmp317;if(_tmp319->tag != 5)goto
_LL189;else{_tmp31A=_tmp319->f1;_tmp31B=_tmp31A.elt_typ;_tmp31C=_tmp31A.ptr_atts;
_tmp31D=_tmp31C.rgn;_tmp31E=_tmp31C.bounds;_tmp31F=_tmp31C.zero_term;}}_LL188:
Cyc_Tcenv_check_rgn_accessible(_tmp316,loc,_tmp31D);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp31E);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp31B),& Cyc_Tcutil_tmk)){
const char*_tmp85B;void*_tmp85A;(_tmp85A=0,Cyc_Tcutil_terr(loc,((_tmp85B="can't dereference abstract pointer type",
_tag_dyneither(_tmp85B,sizeof(char),40))),_tag_dyneither(_tmp85A,sizeof(void*),0)));}
return _tmp31B;_LL189:;_LL18A: {const char*_tmp85F;void*_tmp85E[1];struct Cyc_String_pa_struct
_tmp85D;return(_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp85E[0]=& _tmp85D,Cyc_Tcexp_expr_err(
_tmp316,loc,topt,((_tmp85F="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp85F,sizeof(char),39))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))));}
_LL184:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp332=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(_tmp332,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp333=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp335;
union Cyc_Absyn_AggrInfoU _tmp336;struct Cyc_Absyn_Aggrdecl**_tmp337;struct Cyc_Absyn_Aggrdecl*
_tmp338;struct Cyc_List_List*_tmp339;enum Cyc_Absyn_AggrKind _tmp33B;struct Cyc_List_List*
_tmp33C;_LL18C: {struct Cyc_Absyn_AggrType_struct*_tmp334=(struct Cyc_Absyn_AggrType_struct*)
_tmp333;if(_tmp334->tag != 12)goto _LL18E;else{_tmp335=_tmp334->f1;_tmp336=_tmp335.aggr_info;
if((_tmp336.KnownAggr).tag != 2)goto _LL18E;_tmp337=(struct Cyc_Absyn_Aggrdecl**)(
_tmp336.KnownAggr).val;_tmp338=*_tmp337;_tmp339=_tmp335.targs;}}_LL18D: {struct
Cyc_Absyn_Aggrfield*_tmp341=Cyc_Absyn_lookup_decl_field(_tmp338,f);if(_tmp341 == 
0){const char*_tmp864;void*_tmp863[2];struct Cyc_String_pa_struct _tmp862;struct Cyc_String_pa_struct
_tmp861;return(_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp862.tag=0,((_tmp862.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp338->name)),((_tmp863[0]=& _tmp862,((
_tmp863[1]=& _tmp861,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp864="type %s has no %s field",
_tag_dyneither(_tmp864,sizeof(char),24))),_tag_dyneither(_tmp863,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp338->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp346=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp346,_tmp346,_tmp338->tvs,_tmp339);t2=Cyc_Tcutil_rsubstitute(_tmp346,_tmp347,
_tmp341->type);if(((_tmp338->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp338->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp868;void*_tmp867[1];struct Cyc_String_pa_struct _tmp866;return(
_tmp866.tag=0,((_tmp866.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp867[0]=& _tmp866,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp868="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp868,sizeof(char),56))),_tag_dyneither(_tmp867,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp338->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp86C;void*_tmp86B[1];struct Cyc_String_pa_struct _tmp86A;return(
_tmp86A.tag=0,((_tmp86A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp86B[0]=& _tmp86A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86C="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp86C,sizeof(char),81))),_tag_dyneither(_tmp86B,sizeof(void*),1)))))));}}
return t2;};}_LL18E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp33A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp333;if(_tmp33A->tag != 13)goto _LL190;else{_tmp33B=_tmp33A->f1;_tmp33C=_tmp33A->f2;}}
_LL18F: {struct Cyc_Absyn_Aggrfield*_tmp34E=Cyc_Absyn_lookup_field(_tmp33C,f);if(
_tmp34E == 0){const char*_tmp870;void*_tmp86F[1];struct Cyc_String_pa_struct _tmp86E;
return(_tmp86E.tag=0,((_tmp86E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp86F[0]=& _tmp86E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp870="type has no %s field",
_tag_dyneither(_tmp870,sizeof(char),21))),_tag_dyneither(_tmp86F,sizeof(void*),1)))))));}
if((_tmp33B == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp34E->type)){const char*_tmp874;void*_tmp873[1];struct Cyc_String_pa_struct
_tmp872;return(_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp873[0]=& _tmp872,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp874="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp874,sizeof(char),56))),_tag_dyneither(_tmp873,sizeof(void*),1)))))));}return
_tmp34E->type;}_LL190:{struct Cyc_Absyn_ArrayType_struct*_tmp33D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp333;if(_tmp33D->tag != 9)goto _LL192;}{const char*_tmp875;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp875="size",_tag_dyneither(_tmp875,sizeof(char),5))))
== 0))goto _LL192;};_LL191: goto _LL193;_LL192:{struct Cyc_Absyn_PointerType_struct*
_tmp33F=(struct Cyc_Absyn_PointerType_struct*)_tmp333;if(_tmp33F->tag != 5)goto
_LL194;}{const char*_tmp876;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp876="size",
_tag_dyneither(_tmp876,sizeof(char),5))))== 0))goto _LL194;};_LL193:{const char*
_tmp87A;void*_tmp879[1];struct Cyc_String_pa_struct _tmp878;(_tmp878.tag=0,((
_tmp878.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp879[0]=& _tmp878,Cyc_Tcutil_warn(e->loc,((_tmp87A="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp87A,sizeof(char),54))),_tag_dyneither(_tmp879,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp880;struct Cyc_List_List*_tmp87F;struct Cyc_Absyn_Primop_e_struct*
_tmp87E;outer_e->r=(void*)((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=((
_tmp880.tag=3,((_tmp880.f1=Cyc_Absyn_Numelts,((_tmp880.f2=((_tmp87F=_cycalloc(
sizeof(*_tmp87F)),((_tmp87F->hd=e,((_tmp87F->tl=0,_tmp87F)))))),_tmp880)))))),
_tmp87E))));}return Cyc_Absyn_uint_typ;_LL194:;_LL195: {const char*_tmp881;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp881="size",_tag_dyneither(_tmp881,sizeof(char),5))))
== 0){const char*_tmp885;void*_tmp884[1];struct Cyc_String_pa_struct _tmp883;return(
_tmp883.tag=0,((_tmp883.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp884[0]=& _tmp883,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp885="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp885,sizeof(char),44))),_tag_dyneither(_tmp884,sizeof(void*),1)))))));}
else{const char*_tmp889;void*_tmp888[1];struct Cyc_String_pa_struct _tmp887;return(
_tmp887.tag=0,((_tmp887.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp888[0]=& _tmp887,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp889="expecting struct or union, found %s",_tag_dyneither(
_tmp889,sizeof(char),36))),_tag_dyneither(_tmp888,sizeof(void*),1)))))));}}
_LL18B:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp362=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(
_tmp362,Cyc_Tcenv_clear_notreadctxt(_tmp362,te)),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp363=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp365;void*_tmp366;struct Cyc_Absyn_PtrAtts _tmp367;void*
_tmp368;union Cyc_Absyn_Constraint*_tmp369;union Cyc_Absyn_Constraint*_tmp36A;
_LL197: {struct Cyc_Absyn_PointerType_struct*_tmp364=(struct Cyc_Absyn_PointerType_struct*)
_tmp363;if(_tmp364->tag != 5)goto _LL199;else{_tmp365=_tmp364->f1;_tmp366=_tmp365.elt_typ;
_tmp367=_tmp365.ptr_atts;_tmp368=_tmp367.rgn;_tmp369=_tmp367.bounds;_tmp36A=
_tmp367.zero_term;}}_LL198: Cyc_Tcutil_check_nonzero_bound(loc,_tmp369);{void*
_tmp36B=Cyc_Tcutil_compress(_tmp366);struct Cyc_Absyn_AggrInfo _tmp36D;union Cyc_Absyn_AggrInfoU
_tmp36E;struct Cyc_Absyn_Aggrdecl**_tmp36F;struct Cyc_Absyn_Aggrdecl*_tmp370;
struct Cyc_List_List*_tmp371;enum Cyc_Absyn_AggrKind _tmp373;struct Cyc_List_List*
_tmp374;_LL19C: {struct Cyc_Absyn_AggrType_struct*_tmp36C=(struct Cyc_Absyn_AggrType_struct*)
_tmp36B;if(_tmp36C->tag != 12)goto _LL19E;else{_tmp36D=_tmp36C->f1;_tmp36E=_tmp36D.aggr_info;
if((_tmp36E.KnownAggr).tag != 2)goto _LL19E;_tmp36F=(struct Cyc_Absyn_Aggrdecl**)(
_tmp36E.KnownAggr).val;_tmp370=*_tmp36F;_tmp371=_tmp36D.targs;}}_LL19D: {struct
Cyc_Absyn_Aggrfield*_tmp375=Cyc_Absyn_lookup_decl_field(_tmp370,f);if(_tmp375 == 
0){const char*_tmp88E;void*_tmp88D[2];struct Cyc_String_pa_struct _tmp88C;struct Cyc_String_pa_struct
_tmp88B;return(_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp88C.tag=0,((_tmp88C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp370->name)),((_tmp88D[0]=& _tmp88C,((
_tmp88D[1]=& _tmp88B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88E="type %s has no %s field",
_tag_dyneither(_tmp88E,sizeof(char),24))),_tag_dyneither(_tmp88D,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp37A=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp37B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp37A,_tmp37A,_tmp370->tvs,_tmp371);t3=Cyc_Tcutil_rsubstitute(_tmp37A,_tmp37B,
_tmp375->type);}{struct Cyc_Absyn_Kind*_tmp37C=Cyc_Tcutil_typ_kind(t3);if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,_tmp37C)){const char*_tmp892;void*_tmp891[1];struct Cyc_String_pa_struct
_tmp890;return(_tmp890.tag=0,((_tmp890.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp891[0]=& _tmp890,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp892="cannot get member %s since its type is abstract",_tag_dyneither(_tmp892,
sizeof(char),48))),_tag_dyneither(_tmp891,sizeof(void*),1)))))));}if(((_tmp370->kind
== Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->tagged)
 && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){const char*
_tmp896;void*_tmp895[1];struct Cyc_String_pa_struct _tmp894;return(_tmp894.tag=0,((
_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp895[0]=&
_tmp894,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp896="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp896,sizeof(char),56))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_struct _tmp898;return(
_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp899[0]=& _tmp898,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89A="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp89A,sizeof(char),72))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));}}
return t3;};};}_LL19E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp372=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp36B;if(_tmp372->tag != 13)goto _LL1A0;else{_tmp373=_tmp372->f1;_tmp374=_tmp372->f2;}}
_LL19F: {struct Cyc_Absyn_Aggrfield*_tmp386=Cyc_Absyn_lookup_field(_tmp374,f);if(
_tmp386 == 0){const char*_tmp89E;void*_tmp89D[1];struct Cyc_String_pa_struct _tmp89C;
return(_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp89D[0]=& _tmp89C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89E="type has no %s field",
_tag_dyneither(_tmp89E,sizeof(char),21))),_tag_dyneither(_tmp89D,sizeof(void*),1)))))));}
if((_tmp373 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp386->type)){const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_String_pa_struct
_tmp8A0;return(_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8A1[0]=& _tmp8A0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8A2="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp8A2,sizeof(char),56))),_tag_dyneither(_tmp8A1,sizeof(void*),1)))))));}return
_tmp386->type;}_LL1A0:;_LL1A1: goto _LL19B;_LL19B:;}goto _LL196;_LL199:;_LL19A: goto
_LL196;_LL196:;}{const char*_tmp8A6;void*_tmp8A5[1];struct Cyc_String_pa_struct
_tmp8A4;return(_tmp8A4.tag=0,((_tmp8A4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp8A5[0]=& _tmp8A4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A6="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp8A6,sizeof(char),44))),_tag_dyneither(_tmp8A5,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp391;int
_tmp392;struct _tuple14 _tmp390=Cyc_Evexp_eval_const_uint_exp(index);_tmp391=
_tmp390.f1;_tmp392=_tmp390.f2;if(!_tmp392){const char*_tmp8A9;void*_tmp8A8;return(
_tmp8A8=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8A9="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp8A9,sizeof(char),47))),_tag_dyneither(_tmp8A8,sizeof(void*),0)));}{
struct _handler_cons _tmp395;_push_handler(& _tmp395);{int _tmp397=0;if(setjmp(
_tmp395.handler))_tmp397=1;if(!_tmp397){{void*_tmp398=(*((struct _tuple18*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp391)).f2;_npop_handler(0);
return _tmp398;};_pop_handler();}else{void*_tmp396=(void*)_exn_thrown;void*
_tmp39A=_tmp396;_LL1A3: {struct Cyc_List_Nth_struct*_tmp39B=(struct Cyc_List_Nth_struct*)
_tmp39A;if(_tmp39B->tag != Cyc_List_Nth)goto _LL1A5;}_LL1A4: {const char*_tmp8AE;
void*_tmp8AD[2];struct Cyc_Int_pa_struct _tmp8AC;struct Cyc_Int_pa_struct _tmp8AB;
return(_tmp8AB.tag=1,((_tmp8AB.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp8AC.tag=1,((_tmp8AC.f1=(unsigned long)((int)_tmp391),((
_tmp8AD[0]=& _tmp8AC,((_tmp8AD[1]=& _tmp8AB,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8AE="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp8AE,sizeof(char),41))),_tag_dyneither(_tmp8AD,sizeof(void*),2)))))))))))));}
_LL1A5:;_LL1A6:(void)_throw(_tmp39A);_LL1A2:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp3A0=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp3A1=Cyc_Tcenv_clear_lhs(_tmp3A0,Cyc_Tcenv_clear_notreadctxt(
_tmp3A0,te_orig));Cyc_Tcexp_tcExp(_tmp3A1,0,e1);Cyc_Tcexp_tcExp(_tmp3A1,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3A1,e2)){const char*_tmp8B2;void*_tmp8B1[1];
struct Cyc_String_pa_struct _tmp8B0;return(_tmp8B0.tag=0,((_tmp8B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp8B1[0]=&
_tmp8B0,Cyc_Tcexp_expr_err(_tmp3A1,e2->loc,topt,((_tmp8B2="expecting int subscript, found %s",
_tag_dyneither(_tmp8B2,sizeof(char),34))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))));}{
void*_tmp3A5=t1;struct Cyc_Absyn_PtrInfo _tmp3A7;void*_tmp3A8;struct Cyc_Absyn_Tqual
_tmp3A9;struct Cyc_Absyn_PtrAtts _tmp3AA;void*_tmp3AB;union Cyc_Absyn_Constraint*
_tmp3AC;union Cyc_Absyn_Constraint*_tmp3AD;struct Cyc_List_List*_tmp3AF;_LL1A8: {
struct Cyc_Absyn_PointerType_struct*_tmp3A6=(struct Cyc_Absyn_PointerType_struct*)
_tmp3A5;if(_tmp3A6->tag != 5)goto _LL1AA;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A7.elt_typ;
_tmp3A9=_tmp3A7.elt_tq;_tmp3AA=_tmp3A7.ptr_atts;_tmp3AB=_tmp3AA.rgn;_tmp3AC=
_tmp3AA.bounds;_tmp3AD=_tmp3AA.zero_term;}}_LL1A9: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3AD)){int emit_warning=1;{void*_tmp3B0=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp3AC);struct Cyc_Absyn_Exp*_tmp3B2;_LL1AF: {struct Cyc_Absyn_Upper_b_struct*
_tmp3B1=(struct Cyc_Absyn_Upper_b_struct*)_tmp3B0;if(_tmp3B1->tag != 1)goto _LL1B1;
else{_tmp3B2=_tmp3B1->f1;}}_LL1B0: if(Cyc_Tcutil_is_const_exp(_tmp3A1,e2)){
unsigned int _tmp3B5;int _tmp3B6;struct _tuple14 _tmp3B4=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;if(_tmp3B6){unsigned int _tmp3B8;int
_tmp3B9;struct _tuple14 _tmp3B7=Cyc_Evexp_eval_const_uint_exp(_tmp3B2);_tmp3B8=
_tmp3B7.f1;_tmp3B9=_tmp3B7.f2;if(_tmp3B9  && _tmp3B8 > _tmp3B5)emit_warning=0;}}
goto _LL1AE;_LL1B1: {struct Cyc_Absyn_DynEither_b_struct*_tmp3B3=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3B0;if(_tmp3B3->tag != 0)goto _LL1AE;}_LL1B2: emit_warning=0;goto _LL1AE;_LL1AE:;}
if(emit_warning){const char*_tmp8B5;void*_tmp8B4;(_tmp8B4=0,Cyc_Tcutil_warn(e2->loc,((
_tmp8B5="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp8B5,sizeof(char),63))),_tag_dyneither(_tmp8B4,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp3A1,e2)){unsigned int _tmp3BD;int _tmp3BE;
struct _tuple14 _tmp3BC=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3BD=_tmp3BC.f1;
_tmp3BE=_tmp3BC.f2;if(_tmp3BE)Cyc_Tcutil_check_bound(loc,_tmp3BD,_tmp3AC);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3AC) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3AD)){const char*_tmp8B8;void*_tmp8B7;(_tmp8B7=0,
Cyc_Tcutil_warn(e1->loc,((_tmp8B8="subscript applied to pointer to one object",
_tag_dyneither(_tmp8B8,sizeof(char),43))),_tag_dyneither(_tmp8B7,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3AC);}}Cyc_Tcenv_check_rgn_accessible(
_tmp3A1,loc,_tmp3AB);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3A8),& Cyc_Tcutil_tmk)){
const char*_tmp8BB;void*_tmp8BA;(_tmp8BA=0,Cyc_Tcutil_terr(e1->loc,((_tmp8BB="can't subscript an abstract pointer",
_tag_dyneither(_tmp8BB,sizeof(char),36))),_tag_dyneither(_tmp8BA,sizeof(void*),0)));}
return _tmp3A8;_LL1AA: {struct Cyc_Absyn_TupleType_struct*_tmp3AE=(struct Cyc_Absyn_TupleType_struct*)
_tmp3A5;if(_tmp3AE->tag != 11)goto _LL1AC;else{_tmp3AF=_tmp3AE->f1;}}_LL1AB: return
Cyc_Tcexp_ithTupleType(_tmp3A1,loc,_tmp3AF,e2);_LL1AC:;_LL1AD: {const char*
_tmp8BF;void*_tmp8BE[1];struct Cyc_String_pa_struct _tmp8BD;return(_tmp8BD.tag=0,((
_tmp8BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp8BE[0]=& _tmp8BD,Cyc_Tcexp_expr_err(_tmp3A1,loc,topt,((_tmp8BF="subscript applied to %s",
_tag_dyneither(_tmp8BF,sizeof(char),24))),_tag_dyneither(_tmp8BE,sizeof(void*),1)))))));}
_LL1A7:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp3C6=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3C8;
_LL1B4: {struct Cyc_Absyn_TupleType_struct*_tmp3C7=(struct Cyc_Absyn_TupleType_struct*)
_tmp3C6;if(_tmp3C7->tag != 11)goto _LL1B6;else{_tmp3C8=_tmp3C7->f1;}}_LL1B5: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3C8)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1B3;for(0;es != 0;(es=es->tl,_tmp3C8=_tmp3C8->tl)){
int bogus=0;void*_tmp3C9=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(
_tmp3C8))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3C9,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)
_tmp3C8->hd)).f2,& bogus);{struct _tuple18*_tmp8C2;struct Cyc_List_List*_tmp8C1;
fields=((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1->hd=((_tmp8C2=_cycalloc(
sizeof(*_tmp8C2)),((_tmp8C2->f1=(*((struct _tuple18*)_tmp3C8->hd)).f1,((_tmp8C2->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp8C2)))))),((_tmp8C1->tl=fields,_tmp8C1))))));};}done=1;goto _LL1B3;_LL1B6:;
_LL1B7: goto _LL1B3;_LL1B3:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);{struct _tuple18*_tmp8C5;struct Cyc_List_List*
_tmp8C4;fields=((_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4->hd=((_tmp8C5=
_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5->f1=Cyc_Absyn_empty_tqual(0),((_tmp8C5->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp8C5)))))),((_tmp8C4->tl=fields,_tmp8C4))))));};}{struct Cyc_Absyn_TupleType_struct
_tmp8C8;struct Cyc_Absyn_TupleType_struct*_tmp8C7;return(void*)((_tmp8C7=
_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7[0]=((_tmp8C8.tag=11,((_tmp8C8.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8C8)))),
_tmp8C7))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){
const char*_tmp8CB;void*_tmp8CA;return(_tmp8CA=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8CB="tcCompoundLit",_tag_dyneither(_tmp8CB,sizeof(char),14))),_tag_dyneither(
_tmp8CA,sizeof(void*),0)));}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,struct Cyc_List_List*
des){void*res_t2;struct _RegionHandle*_tmp3D2=Cyc_Tcenv_get_fnrgn(te);{int _tmp3D3=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp3D2,(struct Cyc_Absyn_Exp*(*)(
struct _tuple17*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp8CE;struct Cyc_Absyn_Const_e_struct*_tmp8CD;struct Cyc_Absyn_Const_e_struct*
_tmp3D4=(_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD[0]=((_tmp8CE.tag=0,((
_tmp8CE.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3D3),_tmp8CE)))),_tmp8CD)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3D4,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3D5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp3D3 - 1);if(!Cyc_Tcutil_is_zero(_tmp3D5)){const char*_tmp8D1;
void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_terr(_tmp3D5->loc,((_tmp8D1="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp8D1,sizeof(char),45))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp8D2;sz_exp->topt=((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((
_tmp8D2->v=Cyc_Absyn_uint_typ,_tmp8D2))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp8D6;void*_tmp8D5[1];struct Cyc_String_pa_struct _tmp8D4;(
_tmp8D4.tag=0,((_tmp8D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp8D5[0]=& _tmp8D4,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp8D6="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp8D6,sizeof(char),53))),_tag_dyneither(_tmp8D5,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple17*)des->hd)).f1;if(ds != 0){void*_tmp3DE=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3E1;_LL1B9: {struct Cyc_Absyn_FieldName_struct*_tmp3DF=(struct Cyc_Absyn_FieldName_struct*)
_tmp3DE;if(_tmp3DF->tag != 1)goto _LL1BB;}_LL1BA:{const char*_tmp8D9;void*_tmp8D8;(
_tmp8D8=0,Cyc_Tcutil_terr(loc,((_tmp8D9="only array index designators are supported",
_tag_dyneither(_tmp8D9,sizeof(char),43))),_tag_dyneither(_tmp8D8,sizeof(void*),0)));}
goto _LL1B8;_LL1BB: {struct Cyc_Absyn_ArrayElement_struct*_tmp3E0=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3DE;if(_tmp3E0->tag != 0)goto _LL1B8;else{_tmp3E1=_tmp3E0->f1;}}_LL1BC: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3E1);{unsigned int _tmp3E5;int _tmp3E6;struct _tuple14 _tmp3E4=Cyc_Evexp_eval_const_uint_exp(
_tmp3E1);_tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;if(!_tmp3E6){const char*_tmp8DC;
void*_tmp8DB;(_tmp8DB=0,Cyc_Tcutil_terr(_tmp3E1->loc,((_tmp8DC="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp8DC,sizeof(char),47))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}
else{if(_tmp3E5 != offset){const char*_tmp8E1;void*_tmp8E0[2];struct Cyc_Int_pa_struct
_tmp8DF;struct Cyc_Int_pa_struct _tmp8DE;(_tmp8DE.tag=1,((_tmp8DE.f1=(
unsigned long)((int)_tmp3E5),((_tmp8DF.tag=1,((_tmp8DF.f1=(unsigned long)offset,((
_tmp8E0[0]=& _tmp8DF,((_tmp8E0[1]=& _tmp8DE,Cyc_Tcutil_terr(_tmp3E1->loc,((_tmp8E1="expecting index designator %d but found %d",
_tag_dyneither(_tmp8E1,sizeof(char),43))),_tag_dyneither(_tmp8E0,sizeof(void*),2)))))))))))));}}
goto _LL1B8;};_LL1B8:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3ED=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1BE: {struct Cyc_Absyn_TagType_struct*_tmp3EE=(
struct Cyc_Absyn_TagType_struct*)_tmp3ED;if(_tmp3EE->tag != 20)goto _LL1C0;}_LL1BF:
goto _LL1BD;_LL1C0:;_LL1C1: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_struct _tmp8E3;(_tmp8E3.tag=0,((
_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp8E4[0]=& _tmp8E3,
Cyc_Tcutil_terr(bound->loc,((_tmp8E5="expecting unsigned int, found %s",
_tag_dyneither(_tmp8E5,sizeof(char),33))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))));}
_LL1BD:;}if(!(vd->tq).real_const){const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8E8="comprehension index variable is not declared const!",_tag_dyneither(
_tmp8E8,sizeof(char),52))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3F4=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3F4,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3F4,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3F4,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3F5=te2;void**_tmp3F6=0;struct Cyc_Absyn_Tqual*
_tmp3F7=0;union Cyc_Absyn_Constraint**_tmp3F8=0;if(topt != 0){void*_tmp3F9=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3FB;struct Cyc_Absyn_ArrayInfo _tmp3FD;void*
_tmp3FE;struct Cyc_Absyn_Tqual _tmp3FF;struct Cyc_Absyn_Exp*_tmp400;union Cyc_Absyn_Constraint*
_tmp401;_LL1C3: {struct Cyc_Absyn_PointerType_struct*_tmp3FA=(struct Cyc_Absyn_PointerType_struct*)
_tmp3F9;if(_tmp3FA->tag != 5)goto _LL1C5;else{_tmp3FB=_tmp3FA->f1;}}_LL1C4:{void**
_tmp8E9;_tmp3F6=((_tmp8E9=_region_malloc(_tmp3F4,sizeof(*_tmp8E9)),((_tmp8E9[0]=
_tmp3FB.elt_typ,_tmp8E9))));}{struct Cyc_Absyn_Tqual*_tmp8EA;_tmp3F7=((_tmp8EA=
_region_malloc(_tmp3F4,sizeof(*_tmp8EA)),((_tmp8EA[0]=_tmp3FB.elt_tq,_tmp8EA))));}{
union Cyc_Absyn_Constraint**_tmp8EB;_tmp3F8=((_tmp8EB=_region_malloc(_tmp3F4,
sizeof(*_tmp8EB)),((_tmp8EB[0]=(_tmp3FB.ptr_atts).zero_term,_tmp8EB))));}goto
_LL1C2;_LL1C5: {struct Cyc_Absyn_ArrayType_struct*_tmp3FC=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3F9;if(_tmp3FC->tag != 9)goto _LL1C7;else{_tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FD.elt_type;
_tmp3FF=_tmp3FD.tq;_tmp400=_tmp3FD.num_elts;_tmp401=_tmp3FD.zero_term;}}_LL1C6:{
void**_tmp8EC;_tmp3F6=((_tmp8EC=_region_malloc(_tmp3F4,sizeof(*_tmp8EC)),((
_tmp8EC[0]=_tmp3FE,_tmp8EC))));}{struct Cyc_Absyn_Tqual*_tmp8ED;_tmp3F7=((_tmp8ED=
_region_malloc(_tmp3F4,sizeof(*_tmp8ED)),((_tmp8ED[0]=_tmp3FF,_tmp8ED))));}{
union Cyc_Absyn_Constraint**_tmp8EE;_tmp3F8=((_tmp8EE=_region_malloc(_tmp3F4,
sizeof(*_tmp8EE)),((_tmp8EE[0]=_tmp401,_tmp8EE))));}goto _LL1C2;_LL1C7:;_LL1C8:
goto _LL1C2;_LL1C2:;}{void*t=Cyc_Tcexp_tcExp(_tmp3F5,_tmp3F6,body);if(_tmp3F5->le
== 0){if(!Cyc_Tcutil_is_const_exp(_tmp3F5,bound)){const char*_tmp8F1;void*_tmp8F0;(
_tmp8F0=0,Cyc_Tcutil_terr(bound->loc,((_tmp8F1="bound is not constant",
_tag_dyneither(_tmp8F1,sizeof(char),22))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(_tmp3F5,body)){const char*_tmp8F4;void*_tmp8F3;(
_tmp8F3=0,Cyc_Tcutil_terr(bound->loc,((_tmp8F4="body is not constant",
_tag_dyneither(_tmp8F4,sizeof(char),21))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}}
if(_tmp3F8 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,*_tmp3F8)){struct Cyc_Absyn_Exp*_tmp40C=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*
_tmp8F5;_tmp40C->topt=((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5->v=Cyc_Absyn_uint_typ,
_tmp8F5))));}bound=Cyc_Absyn_add_exp(bound,_tmp40C,0);{struct Cyc_Core_Opt*
_tmp8F6;bound->topt=((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->v=Cyc_Absyn_uint_typ,
_tmp8F6))));}*is_zero_term=1;}{void*_tmp40F=Cyc_Absyn_array_typ(t,_tmp3F7 == 0?
Cyc_Absyn_empty_tqual(0):*_tmp3F7,(struct Cyc_Absyn_Exp*)bound,_tmp3F8 == 0?Cyc_Absyn_false_conref:*
_tmp3F8,0);return _tmp40F;};};};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp410;_push_handler(& _tmp410);{int _tmp412=0;if(setjmp(
_tmp410.handler))_tmp412=1;if(!_tmp412){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp411=(void*)_exn_thrown;void*_tmp414=_tmp411;_LL1CA: {
struct Cyc_Dict_Absent_struct*_tmp415=(struct Cyc_Dict_Absent_struct*)_tmp414;if(
_tmp415->tag != Cyc_Dict_Absent)goto _LL1CC;}_LL1CB:{const char*_tmp8FA;void*
_tmp8F9[1];struct Cyc_String_pa_struct _tmp8F8;(_tmp8F8.tag=0,((_tmp8F8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8F9[0]=&
_tmp8F8,Cyc_Tcutil_terr(loc,((_tmp8FA="unbound struct/union name %s",
_tag_dyneither(_tmp8FA,sizeof(char),29))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1CC:;_LL1CD:(void)_throw(
_tmp414);_LL1C9:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp902;void*_tmp901[1];const char*_tmp900;const char*_tmp8FF;
struct Cyc_String_pa_struct _tmp8FE;(_tmp8FE.tag=0,((_tmp8FE.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8FF="struct",_tag_dyneither(
_tmp8FF,sizeof(char),7)):((_tmp900="union",_tag_dyneither(_tmp900,sizeof(char),6)))),((
_tmp901[0]=& _tmp8FE,Cyc_Tcutil_terr(loc,((_tmp902="can't construct abstract %s",
_tag_dyneither(_tmp902,sizeof(char),28))),_tag_dyneither(_tmp901,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp41E=Cyc_Tcenv_get_fnrgn(te);struct _tuple11 _tmp903;struct
_tuple11 _tmp41F=(_tmp903.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp903.f2=_tmp41E,
_tmp903)));struct Cyc_List_List*_tmp420=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41E,Cyc_Tcutil_r_make_inst_var,&
_tmp41F,ad->tvs);struct Cyc_List_List*_tmp421=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41E,Cyc_Tcutil_r_make_inst_var,&
_tmp41F,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp422=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp420);struct Cyc_List_List*_tmp423=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp421);struct Cyc_List_List*_tmp424=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp41E,_tmp420,_tmp421);struct Cyc_Absyn_AggrType_struct _tmp90D;struct Cyc_Absyn_Aggrdecl**
_tmp90C;struct Cyc_Absyn_AggrInfo _tmp90B;struct Cyc_Absyn_AggrType_struct*_tmp90A;
struct Cyc_Absyn_AggrType_struct*_tmp425=(_tmp90A=_cycalloc(sizeof(*_tmp90A)),((
_tmp90A[0]=((_tmp90D.tag=12,((_tmp90D.f1=((_tmp90B.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C[0]=ad,_tmp90C))))),((_tmp90B.targs=
_tmp422,_tmp90B)))),_tmp90D)))),_tmp90A)));struct Cyc_List_List*_tmp426=*ts;
struct Cyc_List_List*_tmp427=_tmp423;while(_tmp426 != 0  && _tmp427 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp426->hd);Cyc_Tcutil_check_no_qual(
loc,(void*)_tmp426->hd);Cyc_Tcutil_unify((void*)_tmp426->hd,(void*)_tmp427->hd);
_tmp426=_tmp426->tl;_tmp427=_tmp427->tl;}if(_tmp426 != 0){const char*_tmp910;void*
_tmp90F;(_tmp90F=0,Cyc_Tcutil_terr(loc,((_tmp910="too many explicit witness types",
_tag_dyneither(_tmp910,sizeof(char),32))),_tag_dyneither(_tmp90F,sizeof(void*),0)));}*
ts=_tmp423;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp425,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp41E,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields);for(0;fields != 0;fields=fields->tl){int bogus=0;
struct _tuple19 _tmp42B;struct Cyc_Absyn_Aggrfield*_tmp42C;struct Cyc_Absyn_Exp*
_tmp42D;struct _tuple19*_tmp42A=(struct _tuple19*)fields->hd;_tmp42B=*_tmp42A;
_tmp42C=_tmp42B.f1;_tmp42D=_tmp42B.f2;{void*_tmp42E=Cyc_Tcutil_rsubstitute(
_tmp41E,_tmp424,_tmp42C->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp42E,
_tmp42D);if(!Cyc_Tcutil_coerce_arg(te,_tmp42D,_tmp42E,& bogus)){{const char*
_tmp91C;void*_tmp91B[5];struct Cyc_String_pa_struct _tmp91A;const char*_tmp919;
const char*_tmp918;struct Cyc_String_pa_struct _tmp917;struct Cyc_String_pa_struct
_tmp916;struct Cyc_String_pa_struct _tmp915;struct Cyc_String_pa_struct _tmp914;(
_tmp914.tag=0,((_tmp914.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp42D->topt))->v)),((_tmp915.tag=0,((
_tmp915.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp42E)),((_tmp916.tag=0,((_tmp916.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp917.tag=0,((_tmp917.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp918="struct",
_tag_dyneither(_tmp918,sizeof(char),7)):((_tmp919="union",_tag_dyneither(_tmp919,
sizeof(char),6)))),((_tmp91A.tag=0,((_tmp91A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp42C->name),((_tmp91B[0]=& _tmp91A,((_tmp91B[1]=& _tmp917,((
_tmp91B[2]=& _tmp916,((_tmp91B[3]=& _tmp915,((_tmp91B[4]=& _tmp914,Cyc_Tcutil_terr(
_tmp42D->loc,((_tmp91C="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp91C,sizeof(char),40))),_tag_dyneither(_tmp91B,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp438=0;{struct Cyc_List_List*
_tmp439=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp439 != 0;_tmp439=_tmp439->tl){struct _tuple0*_tmp91F;struct Cyc_List_List*
_tmp91E;_tmp438=((_tmp91E=_cycalloc(sizeof(*_tmp91E)),((_tmp91E->hd=((_tmp91F=
_cycalloc(sizeof(*_tmp91F)),((_tmp91F->f1=Cyc_Tcutil_rsubstitute(_tmp41E,_tmp424,(*((
struct _tuple0*)_tmp439->hd)).f1),((_tmp91F->f2=Cyc_Tcutil_rsubstitute(_tmp41E,
_tmp424,(*((struct _tuple0*)_tmp439->hd)).f2),_tmp91F)))))),((_tmp91E->tl=_tmp438,
_tmp91E))))));}}_tmp438=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp438);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp438);return(void*)_tmp425;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp441=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp442=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp444;struct
Cyc_List_List*_tmp445;_LL1CF: {struct Cyc_Absyn_AnonAggrType_struct*_tmp443=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp442;if(_tmp443->tag != 13)goto _LL1D1;
else{_tmp444=_tmp443->f1;_tmp445=_tmp443->f2;}}_LL1D0: if(_tmp444 == Cyc_Absyn_UnionA){
const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_terr(loc,((_tmp922="expecting struct but found union",
_tag_dyneither(_tmp922,sizeof(char),33))),_tag_dyneither(_tmp921,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp441,
loc,args,Cyc_Absyn_StructA,_tmp445);for(0;fields != 0;fields=fields->tl){int bogus=
0;struct _tuple19 _tmp449;struct Cyc_Absyn_Aggrfield*_tmp44A;struct Cyc_Absyn_Exp*
_tmp44B;struct _tuple19*_tmp448=(struct _tuple19*)fields->hd;_tmp449=*_tmp448;
_tmp44A=_tmp449.f1;_tmp44B=_tmp449.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp44A->type,_tmp44B);if(!Cyc_Tcutil_coerce_arg(te,_tmp44B,_tmp44A->type,& bogus)){{
const char*_tmp928;void*_tmp927[3];struct Cyc_String_pa_struct _tmp926;struct Cyc_String_pa_struct
_tmp925;struct Cyc_String_pa_struct _tmp924;(_tmp924.tag=0,((_tmp924.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp44B->topt))->v)),((_tmp925.tag=0,((_tmp925.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp44A->type)),((
_tmp926.tag=0,((_tmp926.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp44A->name),((
_tmp927[0]=& _tmp926,((_tmp927[1]=& _tmp925,((_tmp927[2]=& _tmp924,Cyc_Tcutil_terr(
_tmp44B->loc,((_tmp928="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp928,sizeof(char),41))),_tag_dyneither(_tmp927,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CE;};_LL1D1:;_LL1D2: {const char*_tmp92B;
void*_tmp92A;(_tmp92A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp92B="tcAnonStruct: wrong type",_tag_dyneither(_tmp92B,
sizeof(char),25))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}_LL1CE:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp453=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp92C;struct _tuple11 _tmp454=(_tmp92C.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp92C.f2=_tmp453,_tmp92C)));struct Cyc_List_List*_tmp455=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp453,Cyc_Tcutil_r_make_inst_var,&
_tmp454,tud->tvs);struct Cyc_List_List*_tmp456=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,_tmp455);struct Cyc_Absyn_DatatypeFieldType_struct _tmp932;struct Cyc_Absyn_DatatypeFieldInfo
_tmp931;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp930;void*res=(void*)((
_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp932.tag=4,((_tmp932.f1=((
_tmp931.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp931.targs=_tmp456,
_tmp931)))),_tmp932)))),_tmp930))));if(topt != 0){void*_tmp457=Cyc_Tcutil_compress(*
topt);_LL1D4: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp458=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp457;if(_tmp458->tag != 4)goto _LL1D6;}_LL1D5: Cyc_Tcutil_unify(*topt,res);goto
_LL1D3;_LL1D6:;_LL1D7: goto _LL1D3;_LL1D3:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp453,_tmp455,(*((
struct _tuple18*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp93A;void*_tmp939[3];struct Cyc_String_pa_struct
_tmp938;struct Cyc_String_pa_struct _tmp937;const char*_tmp936;struct Cyc_String_pa_struct
_tmp935;(_tmp935.tag=0,((_tmp935.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp936="?",_tag_dyneither(_tmp936,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp937.tag=0,((_tmp937.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp938.tag=0,((_tmp938.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp939[0]=& _tmp938,((_tmp939[1]=& _tmp937,((
_tmp939[2]=& _tmp935,Cyc_Tcutil_terr(e->loc,((_tmp93A="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp93A,sizeof(char),82))),_tag_dyneither(_tmp939,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp93E;void*_tmp93D[1];
struct Cyc_String_pa_struct _tmp93C;return(_tmp93C.tag=0,((_tmp93C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp93D[0]=& _tmp93C,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp93E="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp93E,sizeof(char),47))),_tag_dyneither(_tmp93D,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp942;void*_tmp941[1];struct Cyc_String_pa_struct _tmp940;return(
_tmp940.tag=0,((_tmp940.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp941[0]=& _tmp940,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp942="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp942,sizeof(char),46))),_tag_dyneither(_tmp941,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp469=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp470;struct Cyc_Absyn_PtrAtts _tmp471;union Cyc_Absyn_Constraint*
_tmp472;struct Cyc_Absyn_ArrayInfo _tmp477;void*_tmp478;struct Cyc_List_List*
_tmp47B;struct Cyc_Absyn_AggrInfo _tmp47D;union Cyc_Absyn_AggrInfoU _tmp47E;struct
Cyc_List_List*_tmp47F;struct Cyc_List_List*_tmp483;_LL1D9: {struct Cyc_Absyn_VoidType_struct*
_tmp46A=(struct Cyc_Absyn_VoidType_struct*)_tmp469;if(_tmp46A->tag != 0)goto _LL1DB;}
_LL1DA: return 1;_LL1DB: {struct Cyc_Absyn_Evar_struct*_tmp46B=(struct Cyc_Absyn_Evar_struct*)
_tmp469;if(_tmp46B->tag != 1)goto _LL1DD;}_LL1DC: goto _LL1DE;_LL1DD: {struct Cyc_Absyn_VarType_struct*
_tmp46C=(struct Cyc_Absyn_VarType_struct*)_tmp469;if(_tmp46C->tag != 2)goto _LL1DF;}
_LL1DE: goto _LL1E0;_LL1DF: {struct Cyc_Absyn_DatatypeType_struct*_tmp46D=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp469;if(_tmp46D->tag != 3)goto _LL1E1;}_LL1E0:
goto _LL1E2;_LL1E1: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp46E=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp469;if(_tmp46E->tag != 4)goto _LL1E3;}_LL1E2: return 0;_LL1E3: {struct Cyc_Absyn_PointerType_struct*
_tmp46F=(struct Cyc_Absyn_PointerType_struct*)_tmp469;if(_tmp46F->tag != 5)goto
_LL1E5;else{_tmp470=_tmp46F->f1;_tmp471=_tmp470.ptr_atts;_tmp472=_tmp471.nullable;}}
_LL1E4: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp472);_LL1E5: {struct Cyc_Absyn_IntType_struct*_tmp473=(struct Cyc_Absyn_IntType_struct*)
_tmp469;if(_tmp473->tag != 6)goto _LL1E7;}_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_Absyn_FloatType_struct*
_tmp474=(struct Cyc_Absyn_FloatType_struct*)_tmp469;if(_tmp474->tag != 7)goto
_LL1E9;}_LL1E8: goto _LL1EA;_LL1E9: {struct Cyc_Absyn_DoubleType_struct*_tmp475=(
struct Cyc_Absyn_DoubleType_struct*)_tmp469;if(_tmp475->tag != 8)goto _LL1EB;}
_LL1EA: return 1;_LL1EB: {struct Cyc_Absyn_ArrayType_struct*_tmp476=(struct Cyc_Absyn_ArrayType_struct*)
_tmp469;if(_tmp476->tag != 9)goto _LL1ED;else{_tmp477=_tmp476->f1;_tmp478=_tmp477.elt_type;}}
_LL1EC: return Cyc_Tcexp_zeroable_type(_tmp478);_LL1ED: {struct Cyc_Absyn_FnType_struct*
_tmp479=(struct Cyc_Absyn_FnType_struct*)_tmp469;if(_tmp479->tag != 10)goto _LL1EF;}
_LL1EE: return 0;_LL1EF: {struct Cyc_Absyn_TupleType_struct*_tmp47A=(struct Cyc_Absyn_TupleType_struct*)
_tmp469;if(_tmp47A->tag != 11)goto _LL1F1;else{_tmp47B=_tmp47A->f1;}}_LL1F0: for(0;(
unsigned int)_tmp47B;_tmp47B=_tmp47B->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple18*)_tmp47B->hd)).f2))return 0;}return 1;_LL1F1: {struct Cyc_Absyn_AggrType_struct*
_tmp47C=(struct Cyc_Absyn_AggrType_struct*)_tmp469;if(_tmp47C->tag != 12)goto
_LL1F3;else{_tmp47D=_tmp47C->f1;_tmp47E=_tmp47D.aggr_info;_tmp47F=_tmp47D.targs;}}
_LL1F2: {struct Cyc_Absyn_Aggrdecl*_tmp490=Cyc_Absyn_get_known_aggrdecl(_tmp47E);
if(_tmp490->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp490->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp491=_new_region("r");struct _RegionHandle*r=&
_tmp491;_push_region(r);{struct Cyc_List_List*_tmp492=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp490->tvs,_tmp47F);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp490->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp492,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp493=0;_npop_handler(0);return _tmp493;}}}{int _tmp494=1;_npop_handler(0);
return _tmp494;};};_pop_region(r);};}_LL1F3: {struct Cyc_Absyn_EnumType_struct*
_tmp480=(struct Cyc_Absyn_EnumType_struct*)_tmp469;if(_tmp480->tag != 14)goto
_LL1F5;}_LL1F4: return 1;_LL1F5: {struct Cyc_Absyn_TagType_struct*_tmp481=(struct
Cyc_Absyn_TagType_struct*)_tmp469;if(_tmp481->tag != 20)goto _LL1F7;}_LL1F6: return
1;_LL1F7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp482=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp469;if(_tmp482->tag != 13)goto _LL1F9;else{_tmp483=_tmp482->f2;}}_LL1F8: for(0;
_tmp483 != 0;_tmp483=_tmp483->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp483->hd)->type))return 0;}return 1;_LL1F9: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp484=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp469;if(_tmp484->tag != 15)goto
_LL1FB;}_LL1FA: return 1;_LL1FB: {struct Cyc_Absyn_TypedefType_struct*_tmp485=(
struct Cyc_Absyn_TypedefType_struct*)_tmp469;if(_tmp485->tag != 18)goto _LL1FD;}
_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_DynRgnType_struct*_tmp486=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp469;if(_tmp486->tag != 17)goto _LL1FF;}_LL1FE: return 0;_LL1FF: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp487=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp469;if(_tmp487->tag != 16)goto
_LL201;}_LL200: return 0;_LL201: {struct Cyc_Absyn_TypeDeclType_struct*_tmp488=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp469;if(_tmp488->tag != 27)goto _LL203;}
_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_ValueofType_struct*_tmp489=(struct Cyc_Absyn_ValueofType_struct*)
_tmp469;if(_tmp489->tag != 19)goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_HeapRgn_struct*
_tmp48A=(struct Cyc_Absyn_HeapRgn_struct*)_tmp469;if(_tmp48A->tag != 21)goto _LL207;}
_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_UniqueRgn_struct*_tmp48B=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp469;if(_tmp48B->tag != 22)goto _LL209;}_LL208: goto _LL20A;_LL209: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp48C=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp469;if(_tmp48C->tag != 23)goto
_LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_AccessEff_struct*_tmp48D=(
struct Cyc_Absyn_AccessEff_struct*)_tmp469;if(_tmp48D->tag != 24)goto _LL20D;}
_LL20C: goto _LL20E;_LL20D: {struct Cyc_Absyn_JoinEff_struct*_tmp48E=(struct Cyc_Absyn_JoinEff_struct*)
_tmp469;if(_tmp48E->tag != 25)goto _LL20F;}_LL20E: goto _LL210;_LL20F: {struct Cyc_Absyn_RgnsEff_struct*
_tmp48F=(struct Cyc_Absyn_RgnsEff_struct*)_tmp469;if(_tmp48F->tag != 26)goto _LL1D8;}
_LL210: {const char*_tmp946;void*_tmp945[1];struct Cyc_String_pa_struct _tmp944;(
_tmp944.tag=0,((_tmp944.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp945[0]=& _tmp944,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp946="bad type `%s' in zeroable type",_tag_dyneither(
_tmp946,sizeof(char),31))),_tag_dyneither(_tmp945,sizeof(void*),1)))))));}_LL1D8:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp498=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp49A;void*_tmp49B;_LL212: {struct Cyc_Absyn_PointerType_struct*_tmp499=(struct
Cyc_Absyn_PointerType_struct*)_tmp498;if(_tmp499->tag != 5)goto _LL214;else{
_tmp49A=_tmp499->f1;_tmp49B=_tmp49A.elt_typ;}}_LL213: Cyc_Tcutil_unify(_tmp49B,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL211;_LL214:;_LL215: goto _LL211;_LL211:;}{const char*_tmp94F;void*_tmp94E[2];
const char*_tmp94D;const char*_tmp94C;struct Cyc_String_pa_struct _tmp94B;struct Cyc_String_pa_struct
_tmp94A;(_tmp94A.tag=0,((_tmp94A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp94B.tag=0,((_tmp94B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp94C="calloc",
_tag_dyneither(_tmp94C,sizeof(char),7))):(struct _dyneither_ptr)((_tmp94D="malloc",
_tag_dyneither(_tmp94D,sizeof(char),7))))),((_tmp94E[0]=& _tmp94B,((_tmp94E[1]=&
_tmp94A,Cyc_Tcutil_terr(loc,((_tmp94F="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp94F,sizeof(char),60))),_tag_dyneither(_tmp94E,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL216: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL217: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp952;
struct Cyc_Absyn_RgnHandleType_struct*_tmp951;void*expected_type=(void*)((_tmp951=
_cycalloc(sizeof(*_tmp951)),((_tmp951[0]=((_tmp952.tag=16,((_tmp952.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp952)))),_tmp951))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));void*_tmp4A2=Cyc_Tcutil_compress(
handle_type);void*_tmp4A4;_LL21A: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4A3=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4A2;if(_tmp4A3->tag != 16)goto _LL21C;
else{_tmp4A4=(void*)_tmp4A3->f1;}}_LL21B: rgn=_tmp4A4;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL219;_LL21C:;_LL21D:{const char*_tmp956;void*_tmp955[1];struct
Cyc_String_pa_struct _tmp954;(_tmp954.tag=0,((_tmp954.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp955[0]=& _tmp954,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp956="expecting region_t type but found %s",
_tag_dyneither(_tmp956,sizeof(char),37))),_tag_dyneither(_tmp955,sizeof(void*),1)))))));}
goto _LL219;_LL219:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp959;void*_tmp958;(
_tmp958=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp959="calloc with empty type",_tag_dyneither(_tmp959,sizeof(char),23))),
_tag_dyneither(_tmp958,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcutil_check_no_qual(loc,elt_type);Cyc_Tcexp_check_malloc_type(1,
loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*er=(*e)->r;retry_sizeof: {
void*_tmp4AC=er;void*_tmp4AE;enum Cyc_Absyn_Primop _tmp4B0;struct Cyc_List_List*
_tmp4B1;struct Cyc_List_List _tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_List_List*
_tmp4B4;struct Cyc_List_List _tmp4B5;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_List_List*
_tmp4B7;_LL21F: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4AD=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AC;if(_tmp4AD->tag != 18)goto _LL221;else{_tmp4AE=(void*)_tmp4AD->f1;}}_LL220:
elt_type=_tmp4AE;{void**_tmp95A;*t=(void**)((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((
_tmp95A[0]=elt_type,_tmp95A))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL21E;_LL221: {struct Cyc_Absyn_Primop_e_struct*
_tmp4AF=(struct Cyc_Absyn_Primop_e_struct*)_tmp4AC;if(_tmp4AF->tag != 3)goto _LL223;
else{_tmp4B0=_tmp4AF->f1;if(_tmp4B0 != Cyc_Absyn_Times)goto _LL223;_tmp4B1=_tmp4AF->f2;
if(_tmp4B1 == 0)goto _LL223;_tmp4B2=*_tmp4B1;_tmp4B3=(struct Cyc_Absyn_Exp*)_tmp4B2.hd;
_tmp4B4=_tmp4B2.tl;if(_tmp4B4 == 0)goto _LL223;_tmp4B5=*_tmp4B4;_tmp4B6=(struct Cyc_Absyn_Exp*)
_tmp4B5.hd;_tmp4B7=_tmp4B5.tl;if(_tmp4B7 != 0)goto _LL223;}}_LL222:{struct _tuple0
_tmp95B;struct _tuple0 _tmp4BA=(_tmp95B.f1=_tmp4B3->r,((_tmp95B.f2=_tmp4B6->r,
_tmp95B)));void*_tmp4BB;void*_tmp4BD;void*_tmp4BE;void*_tmp4C0;_LL226: _tmp4BB=
_tmp4BA.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4BC=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4BB;if(_tmp4BC->tag != 18)goto _LL228;else{_tmp4BD=(void*)_tmp4BC->f1;}};
_LL227: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4BD);elt_type=_tmp4BD;{void**
_tmp95C;*t=(void**)((_tmp95C=_cycalloc(sizeof(*_tmp95C)),((_tmp95C[0]=elt_type,
_tmp95C))));}num_elts=_tmp4B6;one_elt=0;goto _LL225;_LL228: _tmp4BE=_tmp4BA.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4BF=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4BE;if(_tmp4BF->tag != 18)goto _LL22A;else{_tmp4C0=(void*)_tmp4BF->f1;}};
_LL229: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C0);elt_type=_tmp4C0;{void**
_tmp95D;*t=(void**)((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D[0]=elt_type,
_tmp95D))));}num_elts=_tmp4B3;one_elt=0;goto _LL225;_LL22A:;_LL22B: goto No_sizeof;
_LL225:;}goto _LL21E;_LL223:;_LL224: No_sizeof: {struct Cyc_Absyn_Exp*_tmp4C3=*e;{
void*_tmp4C4=_tmp4C3->r;struct Cyc_Absyn_Exp*_tmp4C6;_LL22D: {struct Cyc_Absyn_Cast_e_struct*
_tmp4C5=(struct Cyc_Absyn_Cast_e_struct*)_tmp4C4;if(_tmp4C5->tag != 15)goto _LL22F;
else{_tmp4C6=_tmp4C5->f2;}}_LL22E: _tmp4C3=_tmp4C6;goto _LL22C;_LL22F:;_LL230: goto
_LL22C;_LL22C:;}{void*_tmp4C7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C3->topt))->v);void*_tmp4C9;_LL232: {struct Cyc_Absyn_TagType_struct*
_tmp4C8=(struct Cyc_Absyn_TagType_struct*)_tmp4C7;if(_tmp4C8->tag != 20)goto _LL234;
else{_tmp4C9=(void*)_tmp4C8->f1;}}_LL233:{void*_tmp4CA=Cyc_Tcutil_compress(
_tmp4C9);struct Cyc_Absyn_Exp*_tmp4CC;_LL237: {struct Cyc_Absyn_ValueofType_struct*
_tmp4CB=(struct Cyc_Absyn_ValueofType_struct*)_tmp4CA;if(_tmp4CB->tag != 19)goto
_LL239;else{_tmp4CC=_tmp4CB->f1;}}_LL238: er=_tmp4CC->r;goto retry_sizeof;_LL239:;
_LL23A: goto _LL236;_LL236:;}goto _LL231;_LL234:;_LL235: goto _LL231;_LL231:;}
elt_type=Cyc_Absyn_char_typ;{void**_tmp95E;*t=(void**)((_tmp95E=_cycalloc(
sizeof(*_tmp95E)),((_tmp95E[0]=elt_type,_tmp95E))));}num_elts=*e;one_elt=0;}goto
_LL21E;_LL21E:;}}*is_fat=!one_elt;{void*_tmp4CE=elt_type;struct Cyc_Absyn_AggrInfo
_tmp4D0;union Cyc_Absyn_AggrInfoU _tmp4D1;struct Cyc_Absyn_Aggrdecl**_tmp4D2;struct
Cyc_Absyn_Aggrdecl*_tmp4D3;_LL23C: {struct Cyc_Absyn_AggrType_struct*_tmp4CF=(
struct Cyc_Absyn_AggrType_struct*)_tmp4CE;if(_tmp4CF->tag != 12)goto _LL23E;else{
_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4D0.aggr_info;if((_tmp4D1.KnownAggr).tag != 2)
goto _LL23E;_tmp4D2=(struct Cyc_Absyn_Aggrdecl**)(_tmp4D1.KnownAggr).val;_tmp4D3=*
_tmp4D2;}}_LL23D: if(_tmp4D3->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4D3->impl))->exist_vars != 0){const char*_tmp961;void*_tmp960;(
_tmp960=0,Cyc_Tcutil_terr(loc,((_tmp961="malloc with existential types not yet implemented",
_tag_dyneither(_tmp961,sizeof(char),50))),_tag_dyneither(_tmp960,sizeof(void*),0)));}
goto _LL23B;_LL23E:;_LL23F: goto _LL23B;_LL23B:;}{void*(*_tmp4D6)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4D7=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp4D8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4DA;struct Cyc_Absyn_PtrAtts
_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DC;union Cyc_Absyn_Constraint*_tmp4DD;
union Cyc_Absyn_Constraint*_tmp4DE;_LL241: {struct Cyc_Absyn_PointerType_struct*
_tmp4D9=(struct Cyc_Absyn_PointerType_struct*)_tmp4D8;if(_tmp4D9->tag != 5)goto
_LL243;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4DA.ptr_atts;_tmp4DC=_tmp4DB.nullable;
_tmp4DD=_tmp4DB.bounds;_tmp4DE=_tmp4DB.zero_term;}}_LL242: _tmp4D7=_tmp4DE;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DC))
_tmp4D6=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp4DE) && !(*is_calloc)){{const char*_tmp964;void*_tmp963;(_tmp963=0,Cyc_Tcutil_warn(
loc,((_tmp964="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp964,sizeof(char),55))),_tag_dyneither(_tmp963,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp4E1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4DD);struct Cyc_Absyn_Exp*_tmp4E4;_LL246: {struct Cyc_Absyn_DynEither_b_struct*
_tmp4E2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp4E1;if(_tmp4E2->tag != 0)goto
_LL248;}_LL247: goto _LL245;_LL248:{struct Cyc_Absyn_Upper_b_struct*_tmp4E3=(struct
Cyc_Absyn_Upper_b_struct*)_tmp4E1;if(_tmp4E3->tag != 1)goto _LL24A;else{_tmp4E4=
_tmp4E3->f1;}}if(!(!one_elt))goto _LL24A;_LL249: {int _tmp4E5=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4E5  && Cyc_Evexp_same_const_exp(_tmp4E4,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DD),_tmp4D7);}{void*_tmp4E6=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4E8;_LL24D: {struct Cyc_Absyn_TagType_struct*_tmp4E7=(struct Cyc_Absyn_TagType_struct*)
_tmp4E6;if(_tmp4E7->tag != 20)goto _LL24F;else{_tmp4E8=(void*)_tmp4E7->f1;}}_LL24E: {
struct Cyc_Absyn_Exp*_tmp4E9=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4E8,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4E9,
_tmp4E4)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DD),_tmp4D7);}
goto _LL24C;}_LL24F:;_LL250: goto _LL24C;_LL24C:;}goto _LL245;}_LL24A:;_LL24B: goto
_LL245;_LL245:;}goto _LL240;_LL243:;_LL244: goto _LL240;_LL240:;}if(!one_elt)
_tmp4D6=Cyc_Absyn_dyneither_typ;return _tmp4D6(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp4D7);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4EA=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4EB=Cyc_Tcenv_enter_lhs(
_tmp4EA,Cyc_Tcenv_enter_notreadctxt(_tmp4EA,te));Cyc_Tcexp_tcExp(_tmp4EB,0,e1);
Cyc_Tcexp_tcExp(_tmp4EB,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp4EC=Cyc_Tcutil_compress(t1);_LL252: {struct Cyc_Absyn_ArrayType_struct*
_tmp4ED=(struct Cyc_Absyn_ArrayType_struct*)_tmp4EC;if(_tmp4ED->tag != 9)goto
_LL254;}_LL253:{const char*_tmp967;void*_tmp966;(_tmp966=0,Cyc_Tcutil_terr(loc,((
_tmp967="cannot assign to an array",_tag_dyneither(_tmp967,sizeof(char),26))),
_tag_dyneither(_tmp966,sizeof(void*),0)));}goto _LL251;_LL254:;_LL255: goto _LL251;
_LL251:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){const char*
_tmp96A;void*_tmp969;(_tmp969=0,Cyc_Tcutil_terr(loc,((_tmp96A="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp96A,sizeof(char),58))),_tag_dyneither(_tmp969,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp96D;void*_tmp96C;return(_tmp96C=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp96D="swap non-lvalue",_tag_dyneither(_tmp96D,sizeof(char),
16))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp970;void*_tmp96F;return(_tmp96F=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp970="swap non-lvalue",_tag_dyneither(_tmp970,sizeof(char),16))),
_tag_dyneither(_tmp96F,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp973;void*_tmp972;return(_tmp972=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp973="swap value in zeroterm array",_tag_dyneither(_tmp973,
sizeof(char),29))),_tag_dyneither(_tmp972,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp976;void*_tmp975;return(_tmp975=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp976="swap value in zeroterm array",_tag_dyneither(_tmp976,
sizeof(char),29))),_tag_dyneither(_tmp975,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp97B;void*_tmp97A[2];struct Cyc_String_pa_struct _tmp979;struct Cyc_String_pa_struct
_tmp978;void*_tmp4FA=(_tmp978.tag=0,((_tmp978.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp979.tag=0,((_tmp979.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp97A[0]=&
_tmp979,((_tmp97A[1]=& _tmp978,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp97B="type mismatch: %s != %s",
_tag_dyneither(_tmp97B,sizeof(char),24))),_tag_dyneither(_tmp97A,sizeof(void*),2)))))))))))));
return _tmp4FA;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4FF=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4FF,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp500=s->r;struct Cyc_Absyn_Exp*
_tmp502;struct Cyc_Absyn_Stmt*_tmp504;struct Cyc_Absyn_Stmt*_tmp505;struct Cyc_Absyn_Decl*
_tmp507;struct Cyc_Absyn_Stmt*_tmp508;_LL257: {struct Cyc_Absyn_Exp_s_struct*
_tmp501=(struct Cyc_Absyn_Exp_s_struct*)_tmp500;if(_tmp501->tag != 1)goto _LL259;
else{_tmp502=_tmp501->f1;}}_LL258: {void*_tmp509=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp502->topt))->v;if(!Cyc_Tcutil_unify(_tmp509,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp97F;void*_tmp97E[1];struct
Cyc_String_pa_struct _tmp97D;(_tmp97D.tag=0,((_tmp97D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp509)),((_tmp97E[0]=& _tmp97D,Cyc_Tcutil_terr(
loc,((_tmp97F="statement expression returns type %s",_tag_dyneither(_tmp97F,
sizeof(char),37))),_tag_dyneither(_tmp97E,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp509;}_LL259: {struct Cyc_Absyn_Seq_s_struct*_tmp503=(struct Cyc_Absyn_Seq_s_struct*)
_tmp500;if(_tmp503->tag != 2)goto _LL25B;else{_tmp504=_tmp503->f1;_tmp505=_tmp503->f2;}}
_LL25A: s=_tmp505;continue;_LL25B: {struct Cyc_Absyn_Decl_s_struct*_tmp506=(struct
Cyc_Absyn_Decl_s_struct*)_tmp500;if(_tmp506->tag != 12)goto _LL25D;else{_tmp507=
_tmp506->f1;_tmp508=_tmp506->f2;}}_LL25C: s=_tmp508;continue;_LL25D:;_LL25E: {
const char*_tmp982;void*_tmp981;return(_tmp981=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp982="statement expression must end with expression",_tag_dyneither(_tmp982,
sizeof(char),46))),_tag_dyneither(_tmp981,sizeof(void*),0)));}_LL256:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp50F=t;struct Cyc_Absyn_AggrInfo _tmp511;union
Cyc_Absyn_AggrInfoU _tmp512;struct Cyc_Absyn_Aggrdecl**_tmp513;struct Cyc_Absyn_Aggrdecl*
_tmp514;_LL260: {struct Cyc_Absyn_AggrType_struct*_tmp510=(struct Cyc_Absyn_AggrType_struct*)
_tmp50F;if(_tmp510->tag != 12)goto _LL262;else{_tmp511=_tmp510->f1;_tmp512=_tmp511.aggr_info;
if((_tmp512.KnownAggr).tag != 2)goto _LL262;_tmp513=(struct Cyc_Absyn_Aggrdecl**)(
_tmp512.KnownAggr).val;_tmp514=*_tmp513;}}_LL261: if((_tmp514->kind == Cyc_Absyn_UnionA
 && _tmp514->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp514->impl))->tagged)
goto _LL25F;goto _LL263;_LL262:;_LL263:{const char*_tmp986;void*_tmp985[1];struct
Cyc_String_pa_struct _tmp984;(_tmp984.tag=0,((_tmp984.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp985[0]=& _tmp984,Cyc_Tcutil_terr(
loc,((_tmp986="expecting @tagged union but found %s",_tag_dyneither(_tmp986,
sizeof(char),37))),_tag_dyneither(_tmp985,sizeof(void*),1)))))));}goto _LL25F;
_LL25F:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp989;struct Cyc_Absyn_RgnHandleType_struct*
_tmp988;void*expected_type=(void*)((_tmp988=_cycalloc(sizeof(*_tmp988)),((
_tmp988[0]=((_tmp989.tag=16,((_tmp989.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp989)))),_tmp988))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle));void*_tmp518=Cyc_Tcutil_compress(handle_type);void*
_tmp51A;_LL265: {struct Cyc_Absyn_RgnHandleType_struct*_tmp519=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp518;if(_tmp519->tag != 16)goto _LL267;else{_tmp51A=(void*)_tmp519->f1;}}_LL266:
rgn=_tmp51A;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL264;_LL267:;_LL268:{
const char*_tmp98D;void*_tmp98C[1];struct Cyc_String_pa_struct _tmp98B;(_tmp98B.tag=
0,((_tmp98B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp98C[0]=& _tmp98B,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle))->loc,((_tmp98D="expecting region_t type but found %s",
_tag_dyneither(_tmp98D,sizeof(char),37))),_tag_dyneither(_tmp98C,sizeof(void*),1)))))));}
goto _LL264;_LL264:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp520=e1->r;struct Cyc_Core_Opt*_tmp523;struct Cyc_List_List*_tmp524;struct
Cyc_List_List*_tmp526;union Cyc_Absyn_Cnst _tmp528;struct _dyneither_ptr _tmp529;
union Cyc_Absyn_Cnst _tmp52B;struct _dyneither_ptr _tmp52C;_LL26A: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp521=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp520;if(_tmp521->tag != 28)
goto _LL26C;}_LL26B: {void*_tmp52D=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*
_tmp52E=Cyc_Tcutil_compress(_tmp52D);struct Cyc_Absyn_ArrayInfo _tmp530;void*
_tmp531;struct Cyc_Absyn_Tqual _tmp532;struct Cyc_Absyn_Exp*_tmp533;union Cyc_Absyn_Constraint*
_tmp534;_LL277: {struct Cyc_Absyn_ArrayType_struct*_tmp52F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp52E;if(_tmp52F->tag != 9)goto _LL279;else{_tmp530=_tmp52F->f1;_tmp531=_tmp530.elt_type;
_tmp532=_tmp530.tq;_tmp533=_tmp530.num_elts;_tmp534=_tmp530.zero_term;}}_LL278: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp533);void*b;{void*
_tmp535=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp537;_LL27C: {struct Cyc_Absyn_TagType_struct*_tmp536=(struct Cyc_Absyn_TagType_struct*)
_tmp535;if(_tmp536->tag != 20)goto _LL27E;else{_tmp537=(void*)_tmp536->f1;}}_LL27D:{
struct Cyc_Absyn_Upper_b_struct _tmp990;struct Cyc_Absyn_Upper_b_struct*_tmp98F;b=(
void*)((_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp990.tag=1,((
_tmp990.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp537,0),
0,Cyc_Absyn_No_coercion,0),_tmp990)))),_tmp98F))));}goto _LL27B;_LL27E:;_LL27F:
if(Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp993;struct
Cyc_Absyn_Upper_b_struct*_tmp992;b=(void*)((_tmp992=_cycalloc(sizeof(*_tmp992)),((
_tmp992[0]=((_tmp993.tag=1,((_tmp993.f1=bnd,_tmp993)))),_tmp992))));}else{b=(
void*)& Cyc_Absyn_DynEither_b_val;}_LL27B:;}{struct Cyc_Absyn_PointerType_struct
_tmp99D;struct Cyc_Absyn_PtrAtts _tmp99C;struct Cyc_Absyn_PtrInfo _tmp99B;struct Cyc_Absyn_PointerType_struct*
_tmp99A;void*res_typ=(void*)((_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A[0]=((
_tmp99D.tag=5,((_tmp99D.f1=((_tmp99B.elt_typ=_tmp531,((_tmp99B.elt_tq=_tmp532,((
_tmp99B.ptr_atts=((_tmp99C.rgn=rgn,((_tmp99C.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp99C.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp99C.zero_term=_tmp534,((_tmp99C.ptrloc=0,
_tmp99C)))))))))),_tmp99B)))))),_tmp99D)))),_tmp99A))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp99E;e->topt=((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E->v=res_typ,
_tmp99E))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL279:;_LL27A: {const char*_tmp9A1;void*_tmp9A0;(
_tmp9A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9A1="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9A1,
sizeof(char),45))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}_LL276:;}_LL26C: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp522=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp520;if(_tmp522->tag != 36)goto _LL26E;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}
_LL26D:{struct Cyc_Absyn_Array_e_struct _tmp9A4;struct Cyc_Absyn_Array_e_struct*
_tmp9A3;e1->r=(void*)((_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3[0]=((
_tmp9A4.tag=27,((_tmp9A4.f1=_tmp524,_tmp9A4)))),_tmp9A3))));}_tmp526=_tmp524;
goto _LL26F;_LL26E: {struct Cyc_Absyn_Array_e_struct*_tmp525=(struct Cyc_Absyn_Array_e_struct*)
_tmp520;if(_tmp525->tag != 27)goto _LL270;else{_tmp526=_tmp525->f1;}}_LL26F: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp545=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp547;void*_tmp548;void**_tmp549;struct Cyc_Absyn_Tqual
_tmp54A;struct Cyc_Absyn_PtrAtts _tmp54B;union Cyc_Absyn_Constraint*_tmp54C;_LL281: {
struct Cyc_Absyn_PointerType_struct*_tmp546=(struct Cyc_Absyn_PointerType_struct*)
_tmp545;if(_tmp546->tag != 5)goto _LL283;else{_tmp547=_tmp546->f1;_tmp548=_tmp547.elt_typ;
_tmp549=(void**)&(_tmp546->f1).elt_typ;_tmp54A=_tmp547.elt_tq;_tmp54B=_tmp547.ptr_atts;
_tmp54C=_tmp54B.zero_term;}}_LL282: elt_typ_opt=(void**)_tmp549;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp54C);goto _LL280;
_LL283:;_LL284: goto _LL280;_LL280:;}{void*_tmp54D=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp526);{struct Cyc_Core_Opt*_tmp9A5;e1->topt=((_tmp9A5=
_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5->v=_tmp54D,_tmp9A5))));}{void*res_typ;{
void*_tmp54F=Cyc_Tcutil_compress(_tmp54D);struct Cyc_Absyn_ArrayInfo _tmp551;void*
_tmp552;struct Cyc_Absyn_Tqual _tmp553;struct Cyc_Absyn_Exp*_tmp554;union Cyc_Absyn_Constraint*
_tmp555;_LL286: {struct Cyc_Absyn_ArrayType_struct*_tmp550=(struct Cyc_Absyn_ArrayType_struct*)
_tmp54F;if(_tmp550->tag != 9)goto _LL288;else{_tmp551=_tmp550->f1;_tmp552=_tmp551.elt_type;
_tmp553=_tmp551.tq;_tmp554=_tmp551.num_elts;_tmp555=_tmp551.zero_term;}}_LL287:{
struct Cyc_Absyn_PointerType_struct _tmp9BA;struct Cyc_Absyn_PtrAtts _tmp9B9;struct
Cyc_Absyn_Upper_b_struct _tmp9B8;struct Cyc_Absyn_Upper_b_struct*_tmp9B7;struct Cyc_Absyn_PtrInfo
_tmp9B6;struct Cyc_Absyn_PointerType_struct*_tmp9B5;res_typ=(void*)((_tmp9B5=
_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9BA.tag=5,((_tmp9BA.f1=((_tmp9B6.elt_typ=
_tmp552,((_tmp9B6.elt_tq=_tmp553,((_tmp9B6.ptr_atts=((_tmp9B9.rgn=rgn,((_tmp9B9.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9B9.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9B7=
_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=1,((_tmp9B8.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp554),_tmp9B8)))),_tmp9B7))))),((_tmp9B9.zero_term=_tmp555,((
_tmp9B9.ptrloc=0,_tmp9B9)))))))))),_tmp9B6)))))),_tmp9BA)))),_tmp9B5))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9BB;e->topt=((_tmp9BB=_cycalloc(
sizeof(*_tmp9BB)),((_tmp9BB->v=res_typ,_tmp9BB))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL285;_LL288:;_LL289: {
const char*_tmp9BE;void*_tmp9BD;(_tmp9BD=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9BE="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp9BE,sizeof(char),50))),_tag_dyneither(_tmp9BD,sizeof(void*),0)));}
_LL285:;}return res_typ;};};}_LL270: {struct Cyc_Absyn_Const_e_struct*_tmp527=(
struct Cyc_Absyn_Const_e_struct*)_tmp520;if(_tmp527->tag != 0)goto _LL272;else{
_tmp528=_tmp527->f1;if((_tmp528.String_c).tag != 8)goto _LL272;_tmp529=(struct
_dyneither_ptr)(_tmp528.String_c).val;}}_LL271: {void*_tmp55F=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp560=Cyc_Tcexp_tcExp(te,(void**)& _tmp55F,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9C1;struct Cyc_Absyn_Upper_b_struct*_tmp9C0;
return Cyc_Absyn_atb_typ(_tmp560,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9C0=
_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0[0]=((_tmp9C1.tag=1,((_tmp9C1.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9C1)))),_tmp9C0)))),Cyc_Absyn_false_conref);}_LL272: {struct Cyc_Absyn_Const_e_struct*
_tmp52A=(struct Cyc_Absyn_Const_e_struct*)_tmp520;if(_tmp52A->tag != 0)goto _LL274;
else{_tmp52B=_tmp52A->f1;if((_tmp52B.Wstring_c).tag != 9)goto _LL274;_tmp52C=(
struct _dyneither_ptr)(_tmp52B.Wstring_c).val;}}_LL273: {void*_tmp563=Cyc_Absyn_atb_typ(
Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp564=Cyc_Tcexp_tcExp(te,(void**)& _tmp563,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9C4;struct Cyc_Absyn_Upper_b_struct*_tmp9C3;
return Cyc_Absyn_atb_typ(_tmp564,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9C3=
_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=1,((_tmp9C4.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9C4)))),_tmp9C3)))),Cyc_Absyn_false_conref);}_LL274:;_LL275: {void**
topt2=0;if(topt != 0){void*_tmp567=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp569;void*_tmp56A;void**_tmp56B;struct Cyc_Absyn_Tqual _tmp56C;_LL28B: {struct
Cyc_Absyn_PointerType_struct*_tmp568=(struct Cyc_Absyn_PointerType_struct*)
_tmp567;if(_tmp568->tag != 5)goto _LL28D;else{_tmp569=_tmp568->f1;_tmp56A=_tmp569.elt_typ;
_tmp56B=(void**)&(_tmp568->f1).elt_typ;_tmp56C=_tmp569.elt_tq;}}_LL28C: topt2=(
void**)_tmp56B;goto _LL28A;_LL28D: {struct Cyc_Absyn_DatatypeType_struct*_tmp56D=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp567;if(_tmp56D->tag != 3)goto _LL28F;}
_LL28E:{void**_tmp9C5;topt2=((_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5[0]=*
topt,_tmp9C5))));}goto _LL28A;_LL28F:;_LL290: goto _LL28A;_LL28A:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp9CF;struct Cyc_Absyn_PtrAtts
_tmp9CE;struct Cyc_Absyn_PtrInfo _tmp9CD;struct Cyc_Absyn_PointerType_struct*
_tmp9CC;void*res_typ=(void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((
_tmp9CF.tag=5,((_tmp9CF.f1=((_tmp9CD.elt_typ=telt,((_tmp9CD.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp9CD.ptr_atts=((_tmp9CE.rgn=rgn,((_tmp9CE.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9CE.bounds=Cyc_Absyn_bounds_one_conref,((_tmp9CE.zero_term=
Cyc_Absyn_false_conref,((_tmp9CE.ptrloc=0,_tmp9CE)))))))))),_tmp9CD)))))),
_tmp9CF)))),_tmp9CC))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9D0;e->topt=((_tmp9D0=_cycalloc(
sizeof(*_tmp9D0)),((_tmp9D0->v=res_typ,_tmp9D0))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL269:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp574=t;
struct Cyc_Absyn_ArrayInfo _tmp576;void*_tmp577;struct Cyc_Absyn_Tqual _tmp578;
struct Cyc_Absyn_Exp*_tmp579;union Cyc_Absyn_Constraint*_tmp57A;_LL292: {struct Cyc_Absyn_ArrayType_struct*
_tmp575=(struct Cyc_Absyn_ArrayType_struct*)_tmp574;if(_tmp575->tag != 9)goto
_LL294;else{_tmp576=_tmp575->f1;_tmp577=_tmp576.elt_type;_tmp578=_tmp576.tq;
_tmp579=_tmp576.num_elts;_tmp57A=_tmp576.zero_term;}}_LL293: {void*_tmp57C;
struct _tuple13 _tmp57B=Cyc_Tcutil_addressof_props(te,e);_tmp57C=_tmp57B.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp9D3;struct Cyc_Absyn_Upper_b_struct*_tmp9D2;
void*_tmp57D=_tmp579 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9D2=
_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=1,((_tmp9D3.f1=(struct Cyc_Absyn_Exp*)
_tmp579,_tmp9D3)))),_tmp9D2))));t=Cyc_Absyn_atb_typ(_tmp577,_tmp57C,_tmp578,
_tmp57D,_tmp57A);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL294:;_LL295: return t;_LL291:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp580=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp580,t) && !Cyc_Tcutil_is_noalias_path(_tmp580,e)){const char*_tmp9D6;void*
_tmp9D5;(_tmp9D5=0,Cyc_Tcutil_terr(e->loc,((_tmp9D6="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp9D6,sizeof(char),49))),_tag_dyneither(_tmp9D5,sizeof(void*),0)));}{
void*_tmp583=e->r;union Cyc_Absyn_Cnst _tmp587;struct _dyneither_ptr _tmp588;union
Cyc_Absyn_Cnst _tmp58A;struct _dyneither_ptr _tmp58B;_LL297: {struct Cyc_Absyn_Array_e_struct*
_tmp584=(struct Cyc_Absyn_Array_e_struct*)_tmp583;if(_tmp584->tag != 27)goto _LL299;}
_LL298: goto _LL29A;_LL299: {struct Cyc_Absyn_Comprehension_e_struct*_tmp585=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp583;if(_tmp585->tag != 28)goto _LL29B;}
_LL29A: goto _LL29C;_LL29B: {struct Cyc_Absyn_Const_e_struct*_tmp586=(struct Cyc_Absyn_Const_e_struct*)
_tmp583;if(_tmp586->tag != 0)goto _LL29D;else{_tmp587=_tmp586->f1;if((_tmp587.Wstring_c).tag
!= 9)goto _LL29D;_tmp588=(struct _dyneither_ptr)(_tmp587.Wstring_c).val;}}_LL29C:
goto _LL29E;_LL29D: {struct Cyc_Absyn_Const_e_struct*_tmp589=(struct Cyc_Absyn_Const_e_struct*)
_tmp583;if(_tmp589->tag != 0)goto _LL29F;else{_tmp58A=_tmp589->f1;if((_tmp58A.String_c).tag
!= 8)goto _LL29F;_tmp58B=(struct _dyneither_ptr)(_tmp58A.String_c).val;}}_LL29E:
return t;_LL29F:;_LL2A0: t=Cyc_Tcutil_compress(t);{void*_tmp58C=t;struct Cyc_Absyn_ArrayInfo
_tmp58E;void*_tmp58F;struct Cyc_Absyn_Tqual _tmp590;struct Cyc_Absyn_Exp*_tmp591;
union Cyc_Absyn_Constraint*_tmp592;_LL2A2: {struct Cyc_Absyn_ArrayType_struct*
_tmp58D=(struct Cyc_Absyn_ArrayType_struct*)_tmp58C;if(_tmp58D->tag != 9)goto
_LL2A4;else{_tmp58E=_tmp58D->f1;_tmp58F=_tmp58E.elt_type;_tmp590=_tmp58E.tq;
_tmp591=_tmp58E.num_elts;_tmp592=_tmp58E.zero_term;}}_LL2A3: {void*_tmp594;
struct _tuple13 _tmp593=Cyc_Tcutil_addressof_props(te,e);_tmp594=_tmp593.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp9D9;struct Cyc_Absyn_Upper_b_struct*_tmp9D8;
void*b=_tmp591 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9D8=_cycalloc(
sizeof(*_tmp9D8)),((_tmp9D8[0]=((_tmp9D9.tag=1,((_tmp9D9.f1=(struct Cyc_Absyn_Exp*)
_tmp591,_tmp9D9)))),_tmp9D8))));t=Cyc_Absyn_atb_typ(_tmp58F,_tmp594,_tmp590,b,
_tmp592);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL2A4:;_LL2A5: return t;_LL2A1:;};_LL296:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp597=e->r;
struct Cyc_Absyn_Exp*_tmp599;_LL2A7: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp598=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp597;if(_tmp598->tag != 13)
goto _LL2A9;else{_tmp599=_tmp598->f1;}}_LL2A8: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp599);((struct Cyc_Core_Opt*)_check_null(_tmp599->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp599->topt))->v,0);e->topt=_tmp599->topt;
goto _LL2A6;_LL2A9:;_LL2AA: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp59A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp59C;void*_tmp59D;struct Cyc_Absyn_Tqual
_tmp59E;struct Cyc_Absyn_PtrAtts _tmp59F;void*_tmp5A0;union Cyc_Absyn_Constraint*
_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A2;union Cyc_Absyn_Constraint*_tmp5A3;
_LL2AC: {struct Cyc_Absyn_PointerType_struct*_tmp59B=(struct Cyc_Absyn_PointerType_struct*)
_tmp59A;if(_tmp59B->tag != 5)goto _LL2AE;else{_tmp59C=_tmp59B->f1;_tmp59D=_tmp59C.elt_typ;
_tmp59E=_tmp59C.elt_tq;_tmp59F=_tmp59C.ptr_atts;_tmp5A0=_tmp59F.rgn;_tmp5A1=
_tmp59F.nullable;_tmp5A2=_tmp59F.bounds;_tmp5A3=_tmp59F.zero_term;}}_LL2AD:{void*
_tmp5A4=Cyc_Tcutil_compress(_tmp59D);struct Cyc_Absyn_FnInfo _tmp5A6;struct Cyc_List_List*
_tmp5A7;struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5AA;
struct Cyc_List_List*_tmp5AB;int _tmp5AC;struct Cyc_Absyn_VarargInfo*_tmp5AD;struct
Cyc_List_List*_tmp5AE;struct Cyc_List_List*_tmp5AF;_LL2B1: {struct Cyc_Absyn_FnType_struct*
_tmp5A5=(struct Cyc_Absyn_FnType_struct*)_tmp5A4;if(_tmp5A5->tag != 10)goto _LL2B3;
else{_tmp5A6=_tmp5A5->f1;_tmp5A7=_tmp5A6.tvars;_tmp5A8=_tmp5A6.effect;_tmp5A9=
_tmp5A6.ret_tqual;_tmp5AA=_tmp5A6.ret_typ;_tmp5AB=_tmp5A6.args;_tmp5AC=_tmp5A6.c_varargs;
_tmp5AD=_tmp5A6.cyc_varargs;_tmp5AE=_tmp5A6.rgn_po;_tmp5AF=_tmp5A6.attributes;}}
_LL2B2: if(_tmp5A7 != 0){struct _RegionHandle*_tmp5B0=Cyc_Tcenv_get_fnrgn(te);
struct _tuple11 _tmp9DA;struct _tuple11 _tmp5B1=(_tmp9DA.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp9DA.f2=_tmp5B0,_tmp9DA)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp5B0,Cyc_Tcutil_r_make_inst_var,&
_tmp5B1,_tmp5A7);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp9E0;struct Cyc_Absyn_FnInfo
_tmp9DF;struct Cyc_Absyn_FnType_struct*_tmp9DE;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp5B0,inst,(void*)((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=((_tmp9E0.tag=
10,((_tmp9E0.f1=((_tmp9DF.tvars=0,((_tmp9DF.effect=_tmp5A8,((_tmp9DF.ret_tqual=
_tmp5A9,((_tmp9DF.ret_typ=_tmp5AA,((_tmp9DF.args=_tmp5AB,((_tmp9DF.c_varargs=
_tmp5AC,((_tmp9DF.cyc_varargs=_tmp5AD,((_tmp9DF.rgn_po=_tmp5AE,((_tmp9DF.attributes=
_tmp5AF,_tmp9DF)))))))))))))))))),_tmp9E0)))),_tmp9DE)))));struct Cyc_Absyn_PointerType_struct
_tmp9EA;struct Cyc_Absyn_PtrAtts _tmp9E9;struct Cyc_Absyn_PtrInfo _tmp9E8;struct Cyc_Absyn_PointerType_struct*
_tmp9E7;struct Cyc_Absyn_PointerType_struct*_tmp5B2=(_tmp9E7=_cycalloc(sizeof(*
_tmp9E7)),((_tmp9E7[0]=((_tmp9EA.tag=5,((_tmp9EA.f1=((_tmp9E8.elt_typ=ftyp,((
_tmp9E8.elt_tq=_tmp59E,((_tmp9E8.ptr_atts=((_tmp9E9.rgn=_tmp5A0,((_tmp9E9.nullable=
_tmp5A1,((_tmp9E9.bounds=_tmp5A2,((_tmp9E9.zero_term=_tmp5A3,((_tmp9E9.ptrloc=0,
_tmp9E9)))))))))),_tmp9E8)))))),_tmp9EA)))),_tmp9E7)));struct Cyc_Absyn_Exp*
_tmp5B3=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp9ED;
struct Cyc_Absyn_Instantiate_e_struct*_tmp9EC;e->r=(void*)((_tmp9EC=_cycalloc(
sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=14,((_tmp9ED.f1=_tmp5B3,((_tmp9ED.f2=
ts,_tmp9ED)))))),_tmp9EC))));}{struct Cyc_Core_Opt*_tmp9EE;e->topt=((_tmp9EE=
_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE->v=(void*)_tmp5B2,_tmp9EE))));};}goto
_LL2B0;_LL2B3:;_LL2B4: goto _LL2B0;_LL2B0:;}goto _LL2AB;_LL2AE:;_LL2AF: goto _LL2AB;
_LL2AB:;}goto _LL2A6;_LL2A6:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct
_RegionHandle*_tmp5BF=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp5C0=0;{
void*_tmp5C1=fn_exp->r;struct Cyc_List_List*_tmp5C3;_LL2B6: {struct Cyc_Absyn_FnCall_e_struct*
_tmp5C2=(struct Cyc_Absyn_FnCall_e_struct*)_tmp5C1;if(_tmp5C2->tag != 11)goto
_LL2B8;else{_tmp5C3=_tmp5C2->f2;}}_LL2B7:{void*_tmp5C4=e->r;struct Cyc_List_List*
_tmp5C6;_LL2BB: {struct Cyc_Absyn_FnCall_e_struct*_tmp5C5=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5C4;if(_tmp5C5->tag != 11)goto _LL2BD;else{_tmp5C6=_tmp5C5->f2;}}_LL2BC: {
struct Cyc_List_List*_tmp5C7=alias_arg_exps;int _tmp5C8=0;while(_tmp5C7 != 0){
while(_tmp5C8 != (int)_tmp5C7->hd){if(_tmp5C3 == 0){struct Cyc_Int_pa_struct _tmp9F8;
struct Cyc_Int_pa_struct _tmp9F7;void*_tmp9F6[2];const char*_tmp9F5;void*_tmp9F4;(
_tmp9F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmp9F8.tag=1,((_tmp9F8.f1=(unsigned long)((int)_tmp5C7->hd),((
_tmp9F7.tag=1,((_tmp9F7.f1=(unsigned long)_tmp5C8,((_tmp9F6[0]=& _tmp9F7,((
_tmp9F6[1]=& _tmp9F8,Cyc_aprintf(((_tmp9F5="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp9F5,sizeof(char),36))),_tag_dyneither(_tmp9F6,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp9F4,sizeof(void*),0)));}++ _tmp5C8;_tmp5C3=_tmp5C3->tl;_tmp5C6=((
struct Cyc_List_List*)_check_null(_tmp5C6))->tl;}{struct Cyc_Absyn_Decl*_tmp5CF;
struct Cyc_Absyn_Exp*_tmp5D0;struct _tuple10 _tmp5CE=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5C3))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5C6))->hd)->topt))->v));_tmp5CF=_tmp5CE.f1;_tmp5D0=_tmp5CE.f2;
_tmp5C3->hd=(void*)_tmp5D0;{struct Cyc_List_List*_tmp9F9;_tmp5C0=((_tmp9F9=
_region_malloc(_tmp5BF,sizeof(*_tmp9F9)),((_tmp9F9->hd=_tmp5CF,((_tmp9F9->tl=
_tmp5C0,_tmp9F9))))));}_tmp5C7=_tmp5C7->tl;};}goto _LL2BA;}_LL2BD:;_LL2BE: {const
char*_tmp9FC;void*_tmp9FB;(_tmp9FB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9FC="not a function call!",
_tag_dyneither(_tmp9FC,sizeof(char),21))),_tag_dyneither(_tmp9FB,sizeof(void*),0)));}
_LL2BA:;}goto _LL2B5;_LL2B8:;_LL2B9: {const char*_tmp9FF;void*_tmp9FE;(_tmp9FE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9FF="not a function call!",_tag_dyneither(_tmp9FF,sizeof(char),21))),
_tag_dyneither(_tmp9FE,sizeof(void*),0)));}_LL2B5:;}while(_tmp5C0 != 0){struct Cyc_Absyn_Decl*
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
_tmp62F;struct _tuple8*_tmp631;struct Cyc_List_List*_tmp632;struct Cyc_List_List*
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
_LL2C0: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp5D8=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp5D7;if(_tmp5D8->tag != 13)goto _LL2C2;else{_tmp5D9=_tmp5D8->f1;}}_LL2C1: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp5D9);return;_LL2C2: {struct Cyc_Absyn_UnknownId_e_struct*_tmp5DA=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp5D7;if(_tmp5DA->tag != 2)goto _LL2C4;else{
_tmp5DB=_tmp5DA->f1;}}_LL2C3: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp5DB);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2C4: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp5DC=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp5D7;if(_tmp5DC->tag != 10)goto _LL2C6;else{
_tmp5DD=_tmp5DC->f1;_tmp5DE=_tmp5DC->f2;}}_LL2C5: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp5DD,_tmp5DE);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C6: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5DF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp5D7;if(_tmp5DF->tag != 36)
goto _LL2C8;else{_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5DF->f2;}}_LL2C7: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp5E1);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C8: {struct
Cyc_Absyn_Const_e_struct*_tmp5E2=(struct Cyc_Absyn_Const_e_struct*)_tmp5D7;if(
_tmp5E2->tag != 0)goto _LL2CA;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=(union Cyc_Absyn_Cnst*)&
_tmp5E2->f1;}}_LL2C9: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp5E4,e);goto _LL2BF;_LL2CA: {struct Cyc_Absyn_Var_e_struct*_tmp5E5=(struct Cyc_Absyn_Var_e_struct*)
_tmp5D7;if(_tmp5E5->tag != 1)goto _LL2CC;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=(void*)
_tmp5E5->f2;}}_LL2CB: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp5E6,_tmp5E7);goto _LL2BF;
_LL2CC: {struct Cyc_Absyn_Primop_e_struct*_tmp5E8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5D7;if(_tmp5E8->tag != 3)goto _LL2CE;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E8->f2;}}
_LL2CD: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5E9,_tmp5EA);goto _LL2BF;_LL2CE: {
struct Cyc_Absyn_Increment_e_struct*_tmp5EB=(struct Cyc_Absyn_Increment_e_struct*)
_tmp5D7;if(_tmp5EB->tag != 5)goto _LL2D0;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;}}
_LL2CF: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5EC,_tmp5ED);goto _LL2BF;_LL2D0: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5EE=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5D7;if(_tmp5EE->tag != 4)goto _LL2D2;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;
_tmp5F1=_tmp5EE->f3;}}_LL2D1: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5EF,_tmp5F0,
_tmp5F1);goto _LL2BF;_LL2D2: {struct Cyc_Absyn_Conditional_e_struct*_tmp5F2=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp5D7;if(_tmp5F2->tag != 6)goto _LL2D4;
else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;_tmp5F5=_tmp5F2->f3;}}_LL2D3: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5F3,_tmp5F4,_tmp5F5);goto _LL2BF;_LL2D4: {struct Cyc_Absyn_And_e_struct*
_tmp5F6=(struct Cyc_Absyn_And_e_struct*)_tmp5D7;if(_tmp5F6->tag != 7)goto _LL2D6;
else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;}}_LL2D5: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5F7,_tmp5F8);goto _LL2BF;_LL2D6: {struct Cyc_Absyn_Or_e_struct*_tmp5F9=(struct
Cyc_Absyn_Or_e_struct*)_tmp5D7;if(_tmp5F9->tag != 8)goto _LL2D8;else{_tmp5FA=
_tmp5F9->f1;_tmp5FB=_tmp5F9->f2;}}_LL2D7: t=Cyc_Tcexp_tcOr(te,loc,_tmp5FA,_tmp5FB);
goto _LL2BF;_LL2D8: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5FC=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp5D7;if(_tmp5FC->tag != 9)goto _LL2DA;else{_tmp5FD=_tmp5FC->f1;_tmp5FE=_tmp5FC->f2;}}
_LL2D9: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5FD,_tmp5FE);goto _LL2BF;_LL2DA: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5FF=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5D7;if(_tmp5FF->tag != 11)goto _LL2DC;else{_tmp600=_tmp5FF->f1;_tmp601=_tmp5FF->f2;
_tmp602=_tmp5FF->f3;_tmp603=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5FF->f3;}}
_LL2DB: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp66B;_push_handler(& _tmp66B);{int _tmp66D=0;if(setjmp(
_tmp66B.handler))_tmp66D=1;if(!_tmp66D){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp66C=(void*)_exn_thrown;void*_tmp66F=_tmp66C;struct
_dyneither_ptr _tmp671;_LL313: {struct Cyc_Core_Failure_struct*_tmp670=(struct Cyc_Core_Failure_struct*)
_tmp66F;if(_tmp670->tag != Cyc_Core_Failure)goto _LL315;else{_tmp671=_tmp670->f1;}}
_LL314: ok=0;fn_exp=e;goto _LL312;_LL315:;_LL316:(void)_throw(_tmp66F);_LL312:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp600,_tmp601,_tmp603,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2BF;}_LL2DC: {
struct Cyc_Absyn_Throw_e_struct*_tmp604=(struct Cyc_Absyn_Throw_e_struct*)_tmp5D7;
if(_tmp604->tag != 12)goto _LL2DE;else{_tmp605=_tmp604->f1;}}_LL2DD: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp605);goto _LL2BF;_LL2DE: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp606=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp5D7;if(_tmp606->tag != 14)goto
_LL2E0;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;}}_LL2DF: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp607,_tmp608);goto _LL2BF;_LL2E0: {struct Cyc_Absyn_Cast_e_struct*
_tmp609=(struct Cyc_Absyn_Cast_e_struct*)_tmp5D7;if(_tmp609->tag != 15)goto _LL2E2;
else{_tmp60A=(void*)_tmp609->f1;_tmp60B=_tmp609->f2;_tmp60C=_tmp609->f4;_tmp60D=(
enum Cyc_Absyn_Coercion*)& _tmp609->f4;}}_LL2E1: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp60A,_tmp60B,(enum Cyc_Absyn_Coercion*)_tmp60D);goto _LL2BF;_LL2E2: {struct Cyc_Absyn_Address_e_struct*
_tmp60E=(struct Cyc_Absyn_Address_e_struct*)_tmp5D7;if(_tmp60E->tag != 16)goto
_LL2E4;else{_tmp60F=_tmp60E->f1;}}_LL2E3: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp60F);goto _LL2BF;_LL2E4: {struct Cyc_Absyn_New_e_struct*_tmp610=(struct Cyc_Absyn_New_e_struct*)
_tmp5D7;if(_tmp610->tag != 17)goto _LL2E6;else{_tmp611=_tmp610->f1;_tmp612=(struct
Cyc_Absyn_Exp**)& _tmp610->f1;_tmp613=_tmp610->f2;}}_LL2E5: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp612,e,_tmp613);goto _LL2BF;_LL2E6: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp614=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5D7;if(_tmp614->tag != 19)goto
_LL2E8;else{_tmp615=_tmp614->f1;}}_LL2E7: {void*_tmp672=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp615);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp672);goto _LL2BF;}_LL2E8: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp616=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5D7;if(_tmp616->tag != 18)goto _LL2EA;else{_tmp617=(void*)_tmp616->f1;}}_LL2E9:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp617);goto _LL2BF;_LL2EA: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp618=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp5D7;if(_tmp618->tag != 20)goto
_LL2EC;else{_tmp619=(void*)_tmp618->f1;_tmp61A=(void*)_tmp618->f2;}}_LL2EB: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp619,_tmp61A);goto _LL2BF;_LL2EC: {struct Cyc_Absyn_Deref_e_struct*
_tmp61B=(struct Cyc_Absyn_Deref_e_struct*)_tmp5D7;if(_tmp61B->tag != 21)goto _LL2EE;
else{_tmp61C=_tmp61B->f1;}}_LL2ED: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp61C);goto
_LL2BF;_LL2EE: {struct Cyc_Absyn_AggrMember_e_struct*_tmp61D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp5D7;if(_tmp61D->tag != 22)goto _LL2F0;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61D->f2;
_tmp620=_tmp61D->f3;_tmp621=(int*)& _tmp61D->f3;_tmp622=_tmp61D->f4;_tmp623=(int*)&
_tmp61D->f4;}}_LL2EF: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp61E,_tmp61F,
_tmp621,_tmp623);goto _LL2BF;_LL2F0: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp624=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp5D7;if(_tmp624->tag != 23)goto _LL2F2;
else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;_tmp627=_tmp624->f3;_tmp628=(int*)&
_tmp624->f3;_tmp629=_tmp624->f4;_tmp62A=(int*)& _tmp624->f4;}}_LL2F1: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp625,_tmp626,_tmp628,_tmp62A);goto _LL2BF;_LL2F2: {struct Cyc_Absyn_Subscript_e_struct*
_tmp62B=(struct Cyc_Absyn_Subscript_e_struct*)_tmp5D7;if(_tmp62B->tag != 24)goto
_LL2F4;else{_tmp62C=_tmp62B->f1;_tmp62D=_tmp62B->f2;}}_LL2F3: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp62C,_tmp62D);goto _LL2BF;_LL2F4: {struct Cyc_Absyn_Tuple_e_struct*
_tmp62E=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5D7;if(_tmp62E->tag != 25)goto _LL2F6;
else{_tmp62F=_tmp62E->f1;}}_LL2F5: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp62F);goto
_LL2BF;_LL2F6: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp630=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp5D7;if(_tmp630->tag != 26)goto _LL2F8;else{_tmp631=_tmp630->f1;_tmp632=_tmp630->f2;}}
_LL2F7: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp631,_tmp632);goto _LL2BF;_LL2F8: {
struct Cyc_Absyn_Array_e_struct*_tmp633=(struct Cyc_Absyn_Array_e_struct*)_tmp5D7;
if(_tmp633->tag != 27)goto _LL2FA;else{_tmp634=_tmp633->f1;}}_LL2F9: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp673=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp675;void*_tmp676;void**_tmp677;union Cyc_Absyn_Constraint*
_tmp678;_LL318: {struct Cyc_Absyn_ArrayType_struct*_tmp674=(struct Cyc_Absyn_ArrayType_struct*)
_tmp673;if(_tmp674->tag != 9)goto _LL31A;else{_tmp675=_tmp674->f1;_tmp676=_tmp675.elt_type;
_tmp677=(void**)&(_tmp674->f1).elt_type;_tmp678=_tmp675.zero_term;}}_LL319:
elt_topt=(void**)_tmp677;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp678);goto _LL317;_LL31A:;_LL31B: goto _LL317;_LL317:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp634);goto _LL2BF;}_LL2FA: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp635=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp5D7;
if(_tmp635->tag != 37)goto _LL2FC;else{_tmp636=_tmp635->f1;}}_LL2FB: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp636);goto _LL2BF;_LL2FC: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp637=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp5D7;if(_tmp637->tag != 28)
goto _LL2FE;else{_tmp638=_tmp637->f1;_tmp639=_tmp637->f2;_tmp63A=_tmp637->f3;
_tmp63B=_tmp637->f4;_tmp63C=(int*)& _tmp637->f4;}}_LL2FD: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp638,_tmp639,_tmp63A,_tmp63C);goto _LL2BF;_LL2FE: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp63D=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5D7;if(_tmp63D->tag != 29)goto
_LL300;else{_tmp63E=_tmp63D->f1;_tmp63F=(struct _tuple1**)& _tmp63D->f1;_tmp640=
_tmp63D->f2;_tmp641=(struct Cyc_List_List**)& _tmp63D->f2;_tmp642=_tmp63D->f3;
_tmp643=_tmp63D->f4;_tmp644=(struct Cyc_Absyn_Aggrdecl**)& _tmp63D->f4;}}_LL2FF: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp63F,_tmp641,_tmp642,_tmp644);goto _LL2BF;
_LL300: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp645=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp5D7;if(_tmp645->tag != 30)goto _LL302;else{_tmp646=(void*)_tmp645->f1;_tmp647=
_tmp645->f2;}}_LL301: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp646,_tmp647);goto _LL2BF;
_LL302: {struct Cyc_Absyn_Datatype_e_struct*_tmp648=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp5D7;if(_tmp648->tag != 31)goto _LL304;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;
_tmp64B=_tmp648->f3;}}_LL303: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp649,
_tmp64A,_tmp64B);goto _LL2BF;_LL304: {struct Cyc_Absyn_Enum_e_struct*_tmp64C=(
struct Cyc_Absyn_Enum_e_struct*)_tmp5D7;if(_tmp64C->tag != 32)goto _LL306;else{
_tmp64D=_tmp64C->f1;_tmp64E=(struct _tuple1**)& _tmp64C->f1;_tmp64F=_tmp64C->f2;
_tmp650=_tmp64C->f3;}}_LL305:*_tmp64E=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp650))->name;{struct Cyc_Absyn_EnumType_struct _tmpA02;struct Cyc_Absyn_EnumType_struct*
_tmpA01;t=(void*)((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA02.tag=
14,((_tmpA02.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp64F))->name,((
_tmpA02.f2=_tmp64F,_tmpA02)))))),_tmpA01))));}goto _LL2BF;_LL306: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp651=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp5D7;if(_tmp651->tag != 33)goto
_LL308;else{_tmp652=_tmp651->f1;_tmp653=(struct _tuple1**)& _tmp651->f1;_tmp654=(
void*)_tmp651->f2;_tmp655=_tmp651->f3;}}_LL307:*_tmp653=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp655))->name;t=_tmp654;goto _LL2BF;_LL308: {struct Cyc_Absyn_Malloc_e_struct*
_tmp656=(struct Cyc_Absyn_Malloc_e_struct*)_tmp5D7;if(_tmp656->tag != 34)goto
_LL30A;else{_tmp657=_tmp656->f1;_tmp658=_tmp657.is_calloc;_tmp659=(int*)&(
_tmp656->f1).is_calloc;_tmp65A=_tmp657.rgn;_tmp65B=(struct Cyc_Absyn_Exp**)&(
_tmp656->f1).rgn;_tmp65C=_tmp657.elt_type;_tmp65D=(void***)&(_tmp656->f1).elt_type;
_tmp65E=_tmp657.num_elts;_tmp65F=(struct Cyc_Absyn_Exp**)&(_tmp656->f1).num_elts;
_tmp660=_tmp657.fat_result;_tmp661=(int*)&(_tmp656->f1).fat_result;}}_LL309: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp65B,_tmp65D,_tmp65F,_tmp659,_tmp661);goto
_LL2BF;_LL30A: {struct Cyc_Absyn_Swap_e_struct*_tmp662=(struct Cyc_Absyn_Swap_e_struct*)
_tmp5D7;if(_tmp662->tag != 35)goto _LL30C;else{_tmp663=_tmp662->f1;_tmp664=_tmp662->f2;}}
_LL30B: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp663,_tmp664);goto _LL2BF;_LL30C: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp665=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp5D7;if(_tmp665->tag != 38)goto _LL30E;else{_tmp666=_tmp665->f1;_tmp667=_tmp665->f2;}}
_LL30D: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp666,_tmp667);goto _LL2BF;_LL30E: {
struct Cyc_Absyn_Valueof_e_struct*_tmp668=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp5D7;if(_tmp668->tag != 39)goto _LL310;else{_tmp669=(void*)_tmp668->f1;}}_LL30F:
if(!te->allow_valueof){const char*_tmpA05;void*_tmpA04;(_tmpA04=0,Cyc_Tcutil_terr(
e->loc,((_tmpA05="valueof(-) can only occur within types",_tag_dyneither(_tmpA05,
sizeof(char),39))),_tag_dyneither(_tmpA04,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2BF;_LL310: {struct Cyc_Absyn_Asm_e_struct*_tmp66A=(struct Cyc_Absyn_Asm_e_struct*)
_tmp5D7;if(_tmp66A->tag != 40)goto _LL2BF;}_LL311:{const char*_tmpA08;void*_tmpA07;(
_tmpA07=0,Cyc_Tcutil_terr(e->loc,((_tmpA08="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmpA08,sizeof(char),50))),_tag_dyneither(_tmpA07,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2BF:;}{struct Cyc_Core_Opt*
_tmpA09;e->topt=((_tmpA09=_cycalloc(sizeof(*_tmpA09)),((_tmpA09->v=t,_tmpA09))));};}
