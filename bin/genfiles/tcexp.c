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
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
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
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;
int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int
print_all_effects;int print_using_stmts;int print_externC_stmts;int
print_full_evars;int print_zeroterm;int generate_line_directives;int
use_curr_namespace;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(
void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
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
_dyneither_ptr ap){{void*_tmp665;(_tmp665=0,Cyc_Tcutil_terr(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp665,
sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons _tmp1;_push_handler(&
_tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))_tmp3=1;if(!_tmp3){{struct
_RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=Cyc_Tcenv_lookup_ordinary(
_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*_tmp9;struct Cyc_Absyn_Enumfield*
_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;struct Cyc_Absyn_Datatypedecl*
_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {struct Cyc_Tcenv_VarRes_struct*
_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(_tmp6->tag != 0)goto _LL3;else{
_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct _tmp668;struct Cyc_Absyn_Var_e_struct*
_tmp667;e->r=(void*)((_tmp667=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp667)),((_tmp667[0]=((_tmp668.tag=1,((_tmp668.f1=q,((_tmp668.f2=(void*)_tmp7,
_tmp668)))))),_tmp667))));}goto _LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*_tmp8=(
struct Cyc_Tcenv_EnumRes_struct*)_tmp5;if(_tmp8->tag != 3)goto _LL5;else{_tmp9=
_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{struct Cyc_Absyn_Enum_e_struct _tmp66B;struct Cyc_Absyn_Enum_e_struct*
_tmp66A;e->r=(void*)((_tmp66A=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp66A)),((_tmp66A[0]=((_tmp66B.tag=32,((_tmp66B.f1=q,((_tmp66B.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp9,((_tmp66B.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp66B)))))))),_tmp66A))));}
goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6:{struct Cyc_Absyn_AnonEnum_e_struct _tmp66E;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp66D;e->r=(void*)((_tmp66D=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp66D)),((_tmp66D[0]=((_tmp66E.tag=33,((_tmp66E.f1=q,((_tmp66E.f2=(void*)_tmpC,((
_tmp66E.f3=(struct Cyc_Absyn_Enumfield*)_tmpD,_tmp66E)))))))),_tmp66D))));}goto
_LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp5;if(_tmpE->tag != 2)goto _LL9;else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{
struct Cyc_Absyn_Datatype_e_struct _tmp671;struct Cyc_Absyn_Datatype_e_struct*
_tmp670;e->r=(void*)((_tmp670=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp670)),((_tmp670[0]=((_tmp671.tag=31,((_tmp671.f1=0,((_tmp671.f2=_tmpF,((
_tmp671.f3=_tmp10,_tmp671)))))))),_tmp670))));}goto _LL0;_LL9: {struct Cyc_Tcenv_AggrRes_struct*
_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(_tmp11->tag != 1)goto _LL0;}_LLA:{
const char*_tmp675;void*_tmp674[1];struct Cyc_String_pa_struct _tmp673;(_tmp673.tag=
0,((_tmp673.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp674[0]=& _tmp673,Cyc_Tcutil_terr(e->loc,((_tmp675="bad occurrence of type name %s",
_tag_dyneither(_tmp675,sizeof(char),31))),_tag_dyneither(_tmp674,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Var_e_struct _tmp678;struct Cyc_Absyn_Var_e_struct*_tmp677;e->r=(
void*)((_tmp677=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp677)),((
_tmp677[0]=((_tmp678.tag=1,((_tmp678.f1=q,((_tmp678.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),
_tmp678)))))),_tmp677))));}goto _LL0;_LL0:;};_pop_handler();}else{void*_tmp2=(
void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {struct Cyc_Dict_Absent_struct*_tmp21=(
struct Cyc_Dict_Absent_struct*)_tmp20;if(_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}
_LLD:{struct Cyc_Absyn_Var_e_struct _tmp67B;struct Cyc_Absyn_Var_e_struct*_tmp67A;e->r=(
void*)((_tmp67A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp67A)),((
_tmp67A[0]=((_tmp67B.tag=1,((_tmp67B.f1=q,((_tmp67B.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),
_tmp67B)))))),_tmp67A))));}goto _LLB;_LLE:;_LLF:(void)_throw(_tmp20);_LLB:;}};}
struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct _tuple17*_tmp67C;
return(_tmp67C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp67C)),((_tmp67C->f1=
0,((_tmp67C->f2=e,_tmp67C)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp67F;struct
Cyc_Absyn_FnCall_e_struct*_tmp67E;e->r=(void*)((_tmp67E=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp67E)),((_tmp67E[0]=((_tmp67F.tag=11,((_tmp67F.f1=e1,((_tmp67F.f2=es,((
_tmp67F.f3=0,_tmp67F)))))))),_tmp67E))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp683;void*_tmp682[1];struct Cyc_String_pa_struct
_tmp681;(_tmp681.tag=0,((_tmp681.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp682[0]=& _tmp681,Cyc_Tcutil_terr(e->loc,((
_tmp683="%s is a constant, not a function",_tag_dyneither(_tmp683,sizeof(char),
33))),_tag_dyneither(_tmp682,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp686;struct Cyc_Absyn_Datatype_e_struct*_tmp685;e->r=(void*)((_tmp685=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp685)),((_tmp685[0]=((_tmp686.tag=
31,((_tmp686.f1=es,((_tmp686.f2=_tmp30,((_tmp686.f3=_tmp31,_tmp686)))))))),
_tmp685))));}_npop_handler(0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*
_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)_tmp2C;if(_tmp32->tag != 1)goto _LL1C;
else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct _tmp689;struct
Cyc_Absyn_Aggregate_e_struct*_tmp688;e->r=(void*)((_tmp688=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp688)),((_tmp688[0]=((_tmp689.tag=29,((_tmp689.f1=_tmp33->name,((
_tmp689.f2=0,((_tmp689.f3=_tmp3D,((_tmp689.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp689)))))))))),_tmp688))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp68D;void*_tmp68C[1];
struct Cyc_String_pa_struct _tmp68B;(_tmp68B.tag=0,((_tmp68B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp68C[
0]=& _tmp68B,Cyc_Tcutil_terr(e->loc,((_tmp68D="%s is an enum constructor, not a function",
_tag_dyneither(_tmp68D,sizeof(char),42))),_tag_dyneither(_tmp68C,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp690;struct Cyc_Absyn_FnCall_e_struct*
_tmp68F;e->r=(void*)((_tmp68F=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp68F)),((_tmp68F[0]=((_tmp690.tag=11,((_tmp690.f1=e1,((_tmp690.f2=es,((
_tmp690.f3=0,_tmp690)))))))),_tmp68F))));}return;_LL23:;_LL24:(void)_throw(
_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct _tmp693;struct
Cyc_Absyn_FnCall_e_struct*_tmp692;e->r=(void*)((_tmp692=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp692)),((_tmp692[0]=((_tmp693.tag=11,((_tmp693.f1=e1,((_tmp693.f2=es,((
_tmp693.f3=0,_tmp693)))))))),_tmp692))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp696;
struct Cyc_Absyn_Array_e_struct*_tmp695;e->r=(void*)((_tmp695=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp695)),((_tmp695[0]=((_tmp696.tag=27,((_tmp696.f1=des,_tmp696)))),
_tmp695))));}return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp699;void*_tmp698;(_tmp698=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp699="struct type not properly set",
_tag_dyneither(_tmp699,sizeof(char),29))),_tag_dyneither(_tmp698,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp69C;struct Cyc_Absyn_Aggregate_e_struct*_tmp69B;e->r=(void*)((_tmp69B=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=
29,((_tmp69C.f1=_tmp58->name,((_tmp69C.f2=0,((_tmp69C.f3=des,((_tmp69C.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp58,_tmp69C)))))))))),_tmp69B))));}_LL2E:;}goto
_LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*_tmp50=(struct Cyc_Absyn_ArrayType_struct*)
_tmp4C;if(_tmp50->tag != 9)goto _LL2A;else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;
_tmp53=_tmp51.tq;}}_LL29:{struct Cyc_Absyn_Array_e_struct _tmp69F;struct Cyc_Absyn_Array_e_struct*
_tmp69E;e->r=(void*)((_tmp69E=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp69E)),((_tmp69E[0]=((_tmp69F.tag=27,((_tmp69F.f1=des,_tmp69F)))),_tmp69E))));}
goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4C;if(_tmp54->tag != 13)goto _LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct
_tmp6A2;struct Cyc_Absyn_AnonStruct_e_struct*_tmp6A1;e->r=(void*)((_tmp6A1=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=
30,((_tmp6A2.f1=(void*)t,((_tmp6A2.f2=des,_tmp6A2)))))),_tmp6A1))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp6A5;struct Cyc_Absyn_Array_e_struct*
_tmp6A4;e->r=(void*)((_tmp6A4=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=27,((_tmp6A5.f1=des,_tmp6A5)))),_tmp6A4))));}
goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct
Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6AA;void*_tmp6A9[2];struct Cyc_String_pa_struct _tmp6A8;
struct Cyc_String_pa_struct _tmp6A7;(_tmp6A7.tag=0,((_tmp6A7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp6A8.tag=0,((_tmp6A8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6A9[0]=& _tmp6A8,((_tmp6A9[1]=& _tmp6A7,Cyc_Tcutil_terr(
e->loc,((_tmp6AA="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6AA,sizeof(char),53))),_tag_dyneither(_tmp6A9,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6AB;struct
_tuple0 _tmp6C=(_tmp6AB.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp6AB.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp6AB)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp6AE;struct Cyc_Tcexp_TestEnv _tmp6AD;return(_tmp6AD.eq=((_tmp6AE=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6AE)),((_tmp6AE->f1=_tmp6F,((
_tmp6AE->f2=_tmp72,_tmp6AE)))))),((_tmp6AD.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp6AD)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp6AF;return(_tmp6AF.eq=0,((_tmp6AF.isTrue=0,_tmp6AF)));};}
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst
_tmp76=*((union Cyc_Absyn_Cnst*)_check_null(c));struct _tuple4 _tmp77;enum Cyc_Absyn_Sign
_tmp78;struct _tuple4 _tmp79;enum Cyc_Absyn_Sign _tmp7A;struct _tuple4 _tmp7B;enum 
Cyc_Absyn_Sign _tmp7C;struct _tuple5 _tmp7D;enum Cyc_Absyn_Sign _tmp7E;struct _tuple7
_tmp7F;enum Cyc_Absyn_Sign _tmp80;struct _dyneither_ptr _tmp81;struct _tuple6 _tmp82;
enum Cyc_Absyn_Sign _tmp83;int _tmp84;struct _dyneither_ptr _tmp85;int _tmp86;_LL3E:
if((_tmp76.Char_c).tag != 2)goto _LL40;_tmp77=(struct _tuple4)(_tmp76.Char_c).val;
_tmp78=_tmp77.f1;if(_tmp78 != Cyc_Absyn_Signed)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;
goto _LL3D;_LL40: if((_tmp76.Char_c).tag != 2)goto _LL42;_tmp79=(struct _tuple4)(
_tmp76.Char_c).val;_tmp7A=_tmp79.f1;if(_tmp7A != Cyc_Absyn_Unsigned)goto _LL42;
_LL41: t=Cyc_Absyn_uchar_typ;goto _LL3D;_LL42: if((_tmp76.Char_c).tag != 2)goto _LL44;
_tmp7B=(struct _tuple4)(_tmp76.Char_c).val;_tmp7C=_tmp7B.f1;if(_tmp7C != Cyc_Absyn_None)
goto _LL44;_LL43: t=Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp76.Short_c).tag != 3)
goto _LL46;_tmp7D=(struct _tuple5)(_tmp76.Short_c).val;_tmp7E=_tmp7D.f1;_LL45: t=
_tmp7E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL3D;
_LL46: if((_tmp76.LongLong_c).tag != 5)goto _LL48;_tmp7F=(struct _tuple7)(_tmp76.LongLong_c).val;
_tmp80=_tmp7F.f1;_LL47: t=_tmp80 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL3D;_LL48: if((_tmp76.Float_c).tag != 6)goto _LL4A;_tmp81=(struct
_dyneither_ptr)(_tmp76.Float_c).val;_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A:
if((_tmp76.Int_c).tag != 4)goto _LL4C;_tmp82=(struct _tuple6)(_tmp76.Int_c).val;
_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_LL4B: if(topt == 0)t=_tmp83 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp87=Cyc_Tcutil_compress(*topt);
enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8C;
enum Cyc_Absyn_Size_of _tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of
_tmp90;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;void*_tmp96;_LL51: {
struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp87;
if(_tmp88->tag != 6)goto _LL53;else{_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;if(_tmp8A
!= Cyc_Absyn_Char_sz)goto _LL53;}}_LL52: switch(_tmp89){case Cyc_Absyn_Unsigned:
_LL5F: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL60: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL61: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp89,(char)_tmp84);goto _LL50;_LL53: {struct Cyc_Absyn_IntType_struct*_tmp8B=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8B->tag != 6)goto _LL55;else{_tmp8C=
_tmp8B->f1;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Short_sz)goto _LL55;}}_LL54: t=
_tmp8C == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=Cyc_Absyn_Short_c(
_tmp8C,(short)_tmp84);goto _LL50;_LL55: {struct Cyc_Absyn_IntType_struct*_tmp8E=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8E->tag != 6)goto _LL57;else{_tmp8F=
_tmp8E->f1;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Int_sz)goto _LL57;}}_LL56: t=
_tmp8F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;
_LL57: {struct Cyc_Absyn_IntType_struct*_tmp91=(struct Cyc_Absyn_IntType_struct*)
_tmp87;if(_tmp91->tag != 6)goto _LL59;else{_tmp92=_tmp91->f1;_tmp93=_tmp91->f2;if(
_tmp93 != Cyc_Absyn_Long_sz)goto _LL59;}}_LL58: t=_tmp92 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59:{struct Cyc_Absyn_PointerType_struct*_tmp94=(
struct Cyc_Absyn_PointerType_struct*)_tmp87;if(_tmp94->tag != 5)goto _LL5B;}if(!(
_tmp84 == 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp6B2;struct Cyc_Absyn_Const_e_struct*
_tmp6B1;e->r=(void*)((_tmp6B1=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp6B1)),((_tmp6B1[0]=((_tmp6B2.tag=0,((_tmp6B2.f1=Cyc_Absyn_Null_c,_tmp6B2)))),
_tmp6B1))));}{struct Cyc_List_List*_tmp99=Cyc_Tcenv_lookup_type_vars(te);{struct
Cyc_Absyn_PointerType_struct _tmp6C5;struct Cyc_Absyn_PtrAtts _tmp6C4;struct Cyc_Core_Opt*
_tmp6C3;struct Cyc_Core_Opt*_tmp6C2;struct Cyc_Absyn_PtrInfo _tmp6C1;struct Cyc_Absyn_PointerType_struct*
_tmp6C0;t=(void*)((_tmp6C0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6C0)),((
_tmp6C0[0]=((_tmp6C5.tag=5,((_tmp6C5.f1=((_tmp6C1.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp6C2=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp6C2)),((_tmp6C2->v=_tmp99,_tmp6C2))))),((_tmp6C1.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp6C1.ptr_atts=((_tmp6C4.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp6C3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6C3)),((_tmp6C3->v=
_tmp99,_tmp6C3))))),((_tmp6C4.nullable=Cyc_Absyn_true_conref,((_tmp6C4.bounds=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6C4.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6C4.ptrloc=0,
_tmp6C4)))))))))),_tmp6C1)))))),_tmp6C5)))),_tmp6C0))));}goto _LL50;};_LL5B: {
struct Cyc_Absyn_TagType_struct*_tmp95=(struct Cyc_Absyn_TagType_struct*)_tmp87;
if(_tmp95->tag != 20)goto _LL5D;else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {struct Cyc_Absyn_ValueofType_struct
_tmp6C8;struct Cyc_Absyn_ValueofType_struct*_tmp6C7;struct Cyc_Absyn_ValueofType_struct*
_tmpA0=(_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7[0]=((_tmp6C8.tag=19,((
_tmp6C8.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp84,0),_tmp6C8)))),_tmp6C7)));if(
!Cyc_Tcutil_unify(_tmp96,(void*)_tmpA0)){{const char*_tmp6CD;void*_tmp6CC[2];
struct Cyc_String_pa_struct _tmp6CB;struct Cyc_String_pa_struct _tmp6CA;(_tmp6CA.tag=
0,((_tmp6CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmpA0)),((_tmp6CB.tag=0,((_tmp6CB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp96)),((_tmp6CC[0]=& _tmp6CB,((_tmp6CC[1]=&
_tmp6CA,Cyc_Tcutil_terr(loc,((_tmp6CD="expecting %s but found %s",_tag_dyneither(
_tmp6CD,sizeof(char),26))),_tag_dyneither(_tmp6CC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_struct _tmp6D0;struct Cyc_Absyn_TagType_struct*
_tmp6CF;t=(void*)((_tmp6CF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6CF)),((
_tmp6CF[0]=((_tmp6D0.tag=20,((_tmp6D0.f1=(void*)((void*)_tmpA0),_tmp6D0)))),
_tmp6CF))));}goto _LL50;}_LL5D:;_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 
7)goto _LL4E;_tmp85=(struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(
int)_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);{struct Cyc_Core_Opt*_tmp6D1;elen->topt=((
_tmp6D1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6D1)),((_tmp6D1->v=Cyc_Absyn_uint_typ,
_tmp6D1))));}{struct Cyc_Absyn_Upper_b_struct _tmp6D4;struct Cyc_Absyn_Upper_b_struct*
_tmp6D3;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6D3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6D3)),((
_tmp6D3[0]=((_tmp6D4.tag=1,((_tmp6D4.f1=elen,_tmp6D4)))),_tmp6D3)))),Cyc_Absyn_true_conref);}
if(topt != 0){void*_tmpAC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo
_tmpAE;struct Cyc_Absyn_Tqual _tmpAF;_LL64: {struct Cyc_Absyn_ArrayType_struct*
_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)_tmpAC;if(_tmpAD->tag != 9)goto _LL66;
else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}_LL65: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
_tmpAF,(struct Cyc_Absyn_Exp*)elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),
0);_LL66: {struct Cyc_Absyn_PointerType_struct*_tmpB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp6D5;e->topt=((_tmp6D5=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp6D5)),((_tmp6D5->v=t,_tmp6D5))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct
_tmp6D8;struct Cyc_Absyn_Upper_b_struct*_tmp6D7;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(
te)),Cyc_Absyn_const_tqual(0),(void*)((_tmp6D7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp6D7)),((_tmp6D7[0]=((_tmp6D8.tag=1,((_tmp6D8.f1=elen,_tmp6D8)))),
_tmp6D7)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp6D9;e->topt=((_tmp6D9=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp6D9)),((_tmp6D9->v=t,_tmp6D9))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto _LL63;_LL68:;_LL69: goto _LL63;
_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag != 1)goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;
_LL4F: {struct Cyc_List_List*_tmpB5=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp6EC;struct Cyc_Absyn_PtrAtts _tmp6EB;struct Cyc_Core_Opt*_tmp6EA;struct Cyc_Core_Opt*
_tmp6E9;struct Cyc_Absyn_PtrInfo _tmp6E8;struct Cyc_Absyn_PointerType_struct*
_tmp6E7;t=(void*)((_tmp6E7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6E7)),((
_tmp6E7[0]=((_tmp6EC.tag=5,((_tmp6EC.f1=((_tmp6E8.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp6E9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp6E9)),((_tmp6E9->v=_tmpB5,_tmp6E9))))),((_tmp6E8.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp6E8.ptr_atts=((_tmp6EB.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp6EA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp6EA)),((_tmp6EA->v=
_tmpB5,_tmp6EA))))),((_tmp6EB.nullable=Cyc_Absyn_true_conref,((_tmp6EB.bounds=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6EB.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6EB.ptrloc=0,
_tmp6EB)))))))))),_tmp6E8)))))),_tmp6EC)))),_tmp6E7))));}goto _LL3D;}_LL3D:;}
return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){void*_tmpBC=b;struct Cyc_Absyn_Vardecl*
_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*
_tmpC5;struct Cyc_Absyn_Vardecl*_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}
_LL6C: {const char*_tmp6F0;void*_tmp6EF[1];struct Cyc_String_pa_struct _tmp6EE;
return(_tmp6EE.tag=0,((_tmp6EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp6EF[0]=& _tmp6EE,Cyc_Tcexp_expr_err(te,loc,0,((
_tmp6F0="undeclared identifier: %s",_tag_dyneither(_tmp6F0,sizeof(char),26))),
_tag_dyneither(_tmp6EF,sizeof(void*),1)))))));}_LL6D: {struct Cyc_Absyn_Global_b_struct*
_tmpBE=(struct Cyc_Absyn_Global_b_struct*)_tmpBC;if(_tmpBE->tag != 1)goto _LL6F;
else{_tmpBF=_tmpBE->f1;}}_LL6E:*q=*_tmpBF->name;return _tmpBF->type;_LL6F: {
struct Cyc_Absyn_Funname_b_struct*_tmpC0=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBC;if(_tmpC0->tag != 2)goto _LL71;else{_tmpC1=_tmpC0->f1;}}_LL70:*q=*_tmpC1->name;
return Cyc_Tcutil_fndecl2typ(_tmpC1);_LL71: {struct Cyc_Absyn_Pat_b_struct*_tmpC2=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBC;if(_tmpC2->tag != 5)goto _LL73;else{_tmpC3=
_tmpC2->f1;}}_LL72: _tmpC5=_tmpC3;goto _LL74;_LL73: {struct Cyc_Absyn_Local_b_struct*
_tmpC4=(struct Cyc_Absyn_Local_b_struct*)_tmpBC;if(_tmpC4->tag != 4)goto _LL75;
else{_tmpC5=_tmpC4->f1;}}_LL74: _tmpC7=_tmpC5;goto _LL76;_LL75: {struct Cyc_Absyn_Param_b_struct*
_tmpC6=(struct Cyc_Absyn_Param_b_struct*)_tmpBC;if(_tmpC6->tag != 3)goto _LL6A;
else{_tmpC7=_tmpC6->f1;}}_LL76:{union Cyc_Absyn_Nmspace _tmp6F1;(*q).f1=(((_tmp6F1.Loc_n).val=
0,(((_tmp6F1.Loc_n).tag=4,_tmp6F1))));}if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79:{struct Cyc_Absyn_Valueof_e_struct _tmp6F4;struct Cyc_Absyn_Valueof_e_struct*
_tmp6F3;e->r=(void*)((_tmp6F3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp6F3)),((_tmp6F3[0]=((_tmp6F4.tag=39,((_tmp6F4.f1=(void*)_tmpCE,_tmp6F4)))),
_tmp6F3))));}goto _LL77;_LL7A:;_LL7B: goto _LL77;_LL77:;}return _tmpC7->type;_LL6A:;}
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmpD1=fmt->r;union Cyc_Absyn_Cnst _tmpD3;struct
_dyneither_ptr _tmpD4;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp _tmpD7;void*
_tmpD8;union Cyc_Absyn_Cnst _tmpDA;struct _dyneither_ptr _tmpDB;_LL7D: {struct Cyc_Absyn_Const_e_struct*
_tmpD2=(struct Cyc_Absyn_Const_e_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL7F;
else{_tmpD3=_tmpD2->f1;if((_tmpD3.String_c).tag != 7)goto _LL7F;_tmpD4=(struct
_dyneither_ptr)(_tmpD3.String_c).val;}}_LL7E: _tmpDB=_tmpD4;goto _LL80;_LL7F: {
struct Cyc_Absyn_Cast_e_struct*_tmpD5=(struct Cyc_Absyn_Cast_e_struct*)_tmpD1;if(
_tmpD5->tag != 15)goto _LL81;else{_tmpD6=_tmpD5->f2;_tmpD7=*_tmpD6;_tmpD8=_tmpD7.r;{
struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;
if(_tmpD9->tag != 0)goto _LL81;else{_tmpDA=_tmpD9->f1;if((_tmpDA.String_c).tag != 7)
goto _LL81;_tmpDB=(struct _dyneither_ptr)(_tmpDA.String_c).val;}};}}_LL80:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDB,fmt->loc);goto _LL7C;
_LL81:;_LL82: if(opt_args != 0){struct Cyc_List_List*_tmpDC=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDC->hd);{struct _RegionHandle*_tmpDD=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpDD,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpDD,(struct Cyc_Absyn_Exp*)_tmpDC->hd)){const
char*_tmp6F7;void*_tmp6F6;(_tmp6F6=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDC->hd)->loc,((_tmp6F7="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp6F7,sizeof(char),49))),_tag_dyneither(_tmp6F6,sizeof(void*),0)));}};}}
return;_LL7C:;}if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE0 != 0;(((desc_types=desc_types->tl,
_tmpE0=_tmpE0->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp6FC;void*
_tmp6FB[2];struct Cyc_String_pa_struct _tmp6FA;struct Cyc_String_pa_struct _tmp6F9;(
_tmp6F9.tag=0,((_tmp6F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6FA.tag=0,((_tmp6FA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6FB[
0]=& _tmp6FA,((_tmp6FB[1]=& _tmp6F9,Cyc_Tcutil_terr(e->loc,((_tmp6FC="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp6FC,sizeof(char),51))),_tag_dyneither(_tmp6FB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp6FD;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp6FD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp6FD)),((_tmp6FD->hd=(void*)arg_cnt,((_tmp6FD->tl=*alias_arg_exps,
_tmp6FD))))));}{struct _RegionHandle*_tmpE6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE6,t) && !Cyc_Tcutil_is_noalias_path(_tmpE6,e)){const char*_tmp700;void*
_tmp6FF;(_tmp6FF=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp700="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp700,sizeof(char),49))),_tag_dyneither(_tmp6FF,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp703;void*_tmp702;(_tmp702=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp703="too few arguments",_tag_dyneither(_tmp703,sizeof(char),18))),
_tag_dyneither(_tmp702,sizeof(void*),0)));}if(_tmpE0 != 0){const char*_tmp706;void*
_tmp705;(_tmp705=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp706="too many arguments",_tag_dyneither(_tmp706,sizeof(char),19))),
_tag_dyneither(_tmp705,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus:
_LL84: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp70A;void*_tmp709[1];struct Cyc_String_pa_struct
_tmp708;(_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp709[0]=& _tmp708,Cyc_Tcutil_terr(loc,((_tmp70A="expecting arithmetic type but found %s",
_tag_dyneither(_tmp70A,sizeof(char),39))),_tag_dyneither(_tmp709,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp70E;void*_tmp70D[1];struct Cyc_String_pa_struct _tmp70C;(_tmp70C.tag=
0,((_tmp70C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp70D[0]=& _tmp70C,Cyc_Tcutil_terr(loc,((_tmp70E="expecting integral or * type but found %s",
_tag_dyneither(_tmp70E,sizeof(char),42))),_tag_dyneither(_tmp70D,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp712;void*_tmp711[1];struct Cyc_String_pa_struct _tmp710;(_tmp710.tag=
0,((_tmp710.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp711[0]=& _tmp710,Cyc_Tcutil_terr(loc,((_tmp712="expecting integral type but found %s",
_tag_dyneither(_tmp712,sizeof(char),37))),_tag_dyneither(_tmp711,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Numelts:
_LL87:{void*_tmpF6=t;struct Cyc_Absyn_PtrInfo _tmpF9;struct Cyc_Absyn_PtrAtts _tmpFA;
union Cyc_Absyn_Constraint*_tmpFB;_LL8A: {struct Cyc_Absyn_ArrayType_struct*_tmpF7=(
struct Cyc_Absyn_ArrayType_struct*)_tmpF6;if(_tmpF7->tag != 9)goto _LL8C;}_LL8B:
goto _LL89;_LL8C: {struct Cyc_Absyn_PointerType_struct*_tmpF8=(struct Cyc_Absyn_PointerType_struct*)
_tmpF6;if(_tmpF8->tag != 5)goto _LL8E;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF9.ptr_atts;
_tmpFB=_tmpFA.bounds;}}_LL8D:{void*_tmpFC=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFB);struct Cyc_Absyn_Exp*
_tmpFF;_LL91: {struct Cyc_Absyn_DynEither_b_struct*_tmpFD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFC;if(_tmpFD->tag != 0)goto _LL93;}_LL92: goto _LL90;_LL93: {struct Cyc_Absyn_Upper_b_struct*
_tmpFE=(struct Cyc_Absyn_Upper_b_struct*)_tmpFC;if(_tmpFE->tag != 1)goto _LL90;
else{_tmpFF=_tmpFE->f1;}}_LL94: if(!Cyc_Evexp_c_can_eval(_tmpFF)){const char*
_tmp715;void*_tmp714;(_tmp714=0,Cyc_Tcutil_terr(loc,((_tmp715="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp715,sizeof(char),55))),_tag_dyneither(_tmp714,sizeof(void*),0)));}
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F: {const char*_tmp719;void*_tmp718[1];
struct Cyc_String_pa_struct _tmp717;(_tmp717.tag=0,((_tmp717.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp718[0]=&
_tmp717,Cyc_Tcutil_terr(loc,((_tmp719="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp719,sizeof(char),47))),_tag_dyneither(_tmp718,sizeof(void*),1)))))));}
_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88: {const char*_tmp71C;void*_tmp71B;(
_tmp71B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp71C="Non-unary primop",_tag_dyneither(_tmp71C,sizeof(char),17))),
_tag_dyneither(_tmp71B,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp720;void*_tmp71F[
1];struct Cyc_String_pa_struct _tmp71E;(_tmp71E.tag=0,((_tmp71E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp71F[0]=& _tmp71E,Cyc_Tcutil_terr(e1->loc,((
_tmp720="type %s cannot be used here",_tag_dyneither(_tmp720,sizeof(char),28))),
_tag_dyneither(_tmp71F,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp724;void*_tmp723[1];struct Cyc_String_pa_struct
_tmp722;(_tmp722.tag=0,((_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp723[0]=& _tmp722,Cyc_Tcutil_terr(e2->loc,((_tmp724="type %s cannot be used here",
_tag_dyneither(_tmp724,sizeof(char),28))),_tag_dyneither(_tmp723,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);};}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmp10D=t1;struct Cyc_Absyn_PtrInfo _tmp10F;void*
_tmp110;struct Cyc_Absyn_Tqual _tmp111;struct Cyc_Absyn_PtrAtts _tmp112;void*_tmp113;
union Cyc_Absyn_Constraint*_tmp114;union Cyc_Absyn_Constraint*_tmp115;union Cyc_Absyn_Constraint*
_tmp116;_LL97: {struct Cyc_Absyn_PointerType_struct*_tmp10E=(struct Cyc_Absyn_PointerType_struct*)
_tmp10D;if(_tmp10E->tag != 5)goto _LL99;else{_tmp10F=_tmp10E->f1;_tmp110=_tmp10F.elt_typ;
_tmp111=_tmp10F.elt_tq;_tmp112=_tmp10F.ptr_atts;_tmp113=_tmp112.rgn;_tmp114=
_tmp112.nullable;_tmp115=_tmp112.bounds;_tmp116=_tmp112.zero_term;}}_LL98: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp110),& Cyc_Tcutil_tmk)){const char*
_tmp727;void*_tmp726;(_tmp726=0,Cyc_Tcutil_terr(e1->loc,((_tmp727="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp727,sizeof(char),50))),_tag_dyneither(_tmp726,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp72A;void*_tmp729;(_tmp729=0,
Cyc_Tcutil_terr(e1->loc,((_tmp72A="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp72A,sizeof(char),54))),_tag_dyneither(_tmp729,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp72E;void*_tmp72D[1];struct
Cyc_String_pa_struct _tmp72C;(_tmp72C.tag=0,((_tmp72C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp72D[0]=& _tmp72C,Cyc_Tcutil_terr(
e2->loc,((_tmp72E="expecting int but found %s",_tag_dyneither(_tmp72E,sizeof(
char),27))),_tag_dyneither(_tmp72D,sizeof(void*),1)))))));}{void*_tmp11E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp115);struct Cyc_Absyn_Exp*_tmp121;_LL9C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp11F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp11E;if(_tmp11F->tag != 0)goto
_LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*_tmp120=(struct Cyc_Absyn_Upper_b_struct*)
_tmp11E;if(_tmp120->tag != 1)goto _LL9B;else{_tmp121=_tmp120->f1;}}_LL9F: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp116)){const char*
_tmp731;void*_tmp730;(_tmp730=0,Cyc_Tcutil_warn(e1->loc,((_tmp731="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp731,sizeof(char),70))),_tag_dyneither(_tmp730,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp73B;struct Cyc_Absyn_PtrAtts _tmp73A;struct
Cyc_Absyn_PtrInfo _tmp739;struct Cyc_Absyn_PointerType_struct*_tmp738;struct Cyc_Absyn_PointerType_struct*
_tmp124=(_tmp738=_cycalloc(sizeof(*_tmp738)),((_tmp738[0]=((_tmp73B.tag=5,((
_tmp73B.f1=((_tmp739.elt_typ=_tmp110,((_tmp739.elt_tq=_tmp111,((_tmp739.ptr_atts=((
_tmp73A.rgn=_tmp113,((_tmp73A.nullable=Cyc_Absyn_true_conref,((_tmp73A.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp73A.zero_term=_tmp116,((_tmp73A.ptrloc=0,
_tmp73A)))))))))),_tmp739)))))),_tmp73B)))),_tmp738)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp124,Cyc_Absyn_Other_coercion);return(void*)_tmp124;};_LL9B:;};
_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL96:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp740;void*_tmp73F[2];struct Cyc_String_pa_struct _tmp73E;struct Cyc_String_pa_struct
_tmp73D;(_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp73E.tag=0,((_tmp73E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp73F[0]=& _tmp73E,((
_tmp73F[1]=& _tmp73D,Cyc_Tcutil_terr(e1->loc,((_tmp740="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp740,sizeof(char),59))),_tag_dyneither(_tmp73F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp743;void*_tmp742;(
_tmp742=0,Cyc_Tcutil_terr(e1->loc,((_tmp743="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp743,sizeof(char),50))),_tag_dyneither(_tmp742,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp746;void*_tmp745;(_tmp745=0,
Cyc_Tcutil_terr(e1->loc,((_tmp746="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp746,sizeof(char),54))),_tag_dyneither(_tmp745,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp74B;void*_tmp74A[2];struct
Cyc_String_pa_struct _tmp749;struct Cyc_String_pa_struct _tmp748;(_tmp748.tag=0,((
_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp74A[0]=& _tmp749,((_tmp74A[1]=& _tmp748,Cyc_Tcutil_terr(
e2->loc,((_tmp74B="expecting either %s or int but found %s",_tag_dyneither(
_tmp74B,sizeof(char),40))),_tag_dyneither(_tmp74A,sizeof(void*),2)))))))))))));}
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
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp74C;struct _tuple0
_tmp136=(_tmp74C.f1=Cyc_Tcutil_compress(t1),((_tmp74C.f2=Cyc_Tcutil_compress(t2),
_tmp74C)));void*_tmp137;struct Cyc_Absyn_PtrInfo _tmp139;void*_tmp13A;void*_tmp13B;
struct Cyc_Absyn_PtrInfo _tmp13D;void*_tmp13E;void*_tmp13F;void*_tmp141;_LLA1:
_tmp137=_tmp136.f1;{struct Cyc_Absyn_PointerType_struct*_tmp138=(struct Cyc_Absyn_PointerType_struct*)
_tmp137;if(_tmp138->tag != 5)goto _LLA3;else{_tmp139=_tmp138->f1;_tmp13A=_tmp139.elt_typ;}};
_tmp13B=_tmp136.f2;{struct Cyc_Absyn_PointerType_struct*_tmp13C=(struct Cyc_Absyn_PointerType_struct*)
_tmp13B;if(_tmp13C->tag != 5)goto _LLA3;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13D.elt_typ;}};
_LLA2: if(Cyc_Tcutil_unify(_tmp13A,_tmp13E))return Cyc_Absyn_sint_typ;goto _LLA0;
_LLA3: _tmp13F=_tmp136.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp140=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp13F;if(_tmp140->tag != 16)goto _LLA5;};_tmp141=
_tmp136.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp142=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp141;if(_tmp142->tag != 16)goto _LLA5;};_LLA4: return Cyc_Absyn_sint_typ;_LLA5:;
_LLA6: goto _LLA0;_LLA0:;}{const char*_tmp751;void*_tmp750[2];struct Cyc_String_pa_struct
_tmp74F;struct Cyc_String_pa_struct _tmp74E;(_tmp74E.tag=0,((_tmp74E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp74F.tag=
0,((_tmp74F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp750[0]=& _tmp74F,((_tmp750[1]=& _tmp74E,Cyc_Tcutil_terr(loc,((_tmp751="comparison not allowed between %s and %s",
_tag_dyneither(_tmp751,sizeof(char),41))),_tag_dyneither(_tmp750,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp752;
struct _tuple0 _tmp148=(_tmp752.f1=t1,((_tmp752.f2=t2,_tmp752)));void*_tmp149;void*
_tmp14B;void*_tmp14C;void*_tmp14E;_LLA8: _tmp149=_tmp148.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp149;if(_tmp14A->tag != 16)goto
_LLAA;else{_tmp14B=(void*)_tmp14A->f1;}};_tmp14C=_tmp148.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 16)goto
_LLAA;else{_tmp14E=(void*)_tmp14D->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}{const char*_tmp757;void*_tmp756[2];struct Cyc_String_pa_struct
_tmp755;struct Cyc_String_pa_struct _tmp754;(_tmp754.tag=0,((_tmp754.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp755.tag=
0,((_tmp755.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp756[0]=& _tmp755,((_tmp756[1]=& _tmp754,Cyc_Tcutil_terr(loc,((_tmp757="comparison not allowed between %s and %s",
_tag_dyneither(_tmp757,sizeof(char),41))),_tag_dyneither(_tmp756,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));};}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){switch(p){case Cyc_Absyn_Plus: _LLAC: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLAE: goto _LLAF;case
Cyc_Absyn_Div: _LLAF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLB0: goto _LLB1;case Cyc_Absyn_Bitand: _LLB1: goto _LLB2;case Cyc_Absyn_Bitor:
_LLB2: goto _LLB3;case Cyc_Absyn_Bitxor: _LLB3: goto _LLB4;case Cyc_Absyn_Bitlshift:
_LLB4: goto _LLB5;case Cyc_Absyn_Bitlrshift: _LLB5: goto _LLB6;case Cyc_Absyn_Bitarshift:
_LLB6: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLB7: goto _LLB8;case Cyc_Absyn_Neq: _LLB8: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLB9: goto _LLBA;case Cyc_Absyn_Lt: _LLBA: goto _LLBB;case Cyc_Absyn_Gte:
_LLBB: goto _LLBC;case Cyc_Absyn_Lte: _LLBC: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLBD: {const char*_tmp75A;void*_tmp759;(_tmp759=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp75A="bad binary primop",
_tag_dyneither(_tmp75A,sizeof(char),18))),_tag_dyneither(_tmp759,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: {const char*_tmp75D;void*_tmp75C;return(_tmp75C=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp75D="primitive operator has 0 arguments",_tag_dyneither(_tmp75D,
sizeof(char),35))),_tag_dyneither(_tmp75C,sizeof(void*),0)));}case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: {const char*_tmp760;void*_tmp75F;return(_tmp75F=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp760="primitive operator has > 2 arguments",_tag_dyneither(_tmp760,
sizeof(char),37))),_tag_dyneither(_tmp75F,sizeof(void*),0)));}}return t;};}struct
_tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static int Cyc_Tcexp_check_writable_aggr(
struct Cyc_Position_Segment*loc,void*t){void*_tmp159=Cyc_Tcutil_compress(t);void*
_tmp15A=_tmp159;struct Cyc_Absyn_AggrInfo _tmp15C;union Cyc_Absyn_AggrInfoU _tmp15D;
struct Cyc_Absyn_Aggrdecl**_tmp15E;struct Cyc_Absyn_Aggrdecl*_tmp15F;struct Cyc_List_List*
_tmp161;struct Cyc_Absyn_DatatypeFieldInfo _tmp163;union Cyc_Absyn_DatatypeFieldInfoU
_tmp164;struct _tuple2 _tmp165;struct Cyc_Absyn_Datatypefield*_tmp166;struct Cyc_Absyn_ArrayInfo
_tmp168;void*_tmp169;struct Cyc_Absyn_Tqual _tmp16A;struct Cyc_List_List*_tmp16C;
_LLC5: {struct Cyc_Absyn_AggrType_struct*_tmp15B=(struct Cyc_Absyn_AggrType_struct*)
_tmp15A;if(_tmp15B->tag != 12)goto _LLC7;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15C.aggr_info;
if((_tmp15D.KnownAggr).tag != 2)goto _LLC7;_tmp15E=(struct Cyc_Absyn_Aggrdecl**)(
_tmp15D.KnownAggr).val;_tmp15F=*_tmp15E;}}_LLC6: _tmp161=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp15F->impl))->fields;goto _LLC8;_LLC7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp160=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp15A;if(_tmp160->tag != 13)goto
_LLC9;else{_tmp161=_tmp160->f2;}}_LLC8: for(0;_tmp161 != 0;_tmp161=_tmp161->tl){
struct Cyc_Absyn_Aggrfield*_tmp16D=(struct Cyc_Absyn_Aggrfield*)_tmp161->hd;if((
_tmp16D->tq).real_const){{const char*_tmp764;void*_tmp763[1];struct Cyc_String_pa_struct
_tmp762;(_tmp762.tag=0,((_tmp762.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp16D->name),((_tmp763[0]=& _tmp762,Cyc_Tcutil_terr(loc,((_tmp764="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp764,sizeof(char),56))),_tag_dyneither(_tmp763,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16D->type))return 0;}return 1;
_LLC9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15A;if(_tmp162->tag != 4)goto _LLCB;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
if((_tmp164.KnownDatatypefield).tag != 2)goto _LLCB;_tmp165=(struct _tuple2)(
_tmp164.KnownDatatypefield).val;_tmp166=_tmp165.f2;}}_LLCA:{struct Cyc_List_List*
_tmp171=_tmp166->typs;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){struct _tuple18
_tmp173;struct Cyc_Absyn_Tqual _tmp174;void*_tmp175;struct _tuple18*_tmp172=(struct
_tuple18*)_tmp171->hd;_tmp173=*_tmp172;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;if(
_tmp174.real_const){{const char*_tmp768;void*_tmp767[1];struct Cyc_String_pa_struct
_tmp766;(_tmp766.tag=0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp166->name)),((_tmp767[0]=& _tmp766,Cyc_Tcutil_terr(
loc,((_tmp768="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp768,sizeof(char),64))),_tag_dyneither(_tmp767,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp175))return 0;}}return 1;_LLCB: {
struct Cyc_Absyn_ArrayType_struct*_tmp167=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15A;if(_tmp167->tag != 9)goto _LLCD;else{_tmp168=_tmp167->f1;_tmp169=_tmp168.elt_type;
_tmp16A=_tmp168.tq;}}_LLCC: if(_tmp16A.real_const){{const char*_tmp76B;void*
_tmp76A;(_tmp76A=0,Cyc_Tcutil_terr(loc,((_tmp76B="attempt to over-write a const array",
_tag_dyneither(_tmp76B,sizeof(char),36))),_tag_dyneither(_tmp76A,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp169);_LLCD: {struct Cyc_Absyn_TupleType_struct*
_tmp16B=(struct Cyc_Absyn_TupleType_struct*)_tmp15A;if(_tmp16B->tag != 11)goto
_LLCF;else{_tmp16C=_tmp16B->f1;}}_LLCE: for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct _tuple18 _tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct _tuple18*
_tmp17B=(struct _tuple18*)_tmp16C->hd;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f1;_tmp17E=
_tmp17C.f2;if(_tmp17D.real_const){{const char*_tmp76E;void*_tmp76D;(_tmp76D=0,Cyc_Tcutil_terr(
loc,((_tmp76E="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp76E,sizeof(char),56))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17E))return 0;}return 1;_LLCF:;
_LLD0: return 1;_LLC4:;}static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v))return;{void*_tmp181=e->r;void*_tmp183;
struct Cyc_Absyn_Vardecl*_tmp185;void*_tmp187;struct Cyc_Absyn_Vardecl*_tmp189;
void*_tmp18B;struct Cyc_Absyn_Vardecl*_tmp18D;void*_tmp18F;struct Cyc_Absyn_Vardecl*
_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*
_tmp196;struct _dyneither_ptr*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct
_dyneither_ptr*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;
struct Cyc_Absyn_Exp*_tmp1A0;_LLD2: {struct Cyc_Absyn_Var_e_struct*_tmp182=(struct
Cyc_Absyn_Var_e_struct*)_tmp181;if(_tmp182->tag != 1)goto _LLD4;else{_tmp183=(void*)
_tmp182->f2;{struct Cyc_Absyn_Param_b_struct*_tmp184=(struct Cyc_Absyn_Param_b_struct*)
_tmp183;if(_tmp184->tag != 3)goto _LLD4;else{_tmp185=_tmp184->f1;}};}}_LLD3:
_tmp189=_tmp185;goto _LLD5;_LLD4: {struct Cyc_Absyn_Var_e_struct*_tmp186=(struct
Cyc_Absyn_Var_e_struct*)_tmp181;if(_tmp186->tag != 1)goto _LLD6;else{_tmp187=(void*)
_tmp186->f2;{struct Cyc_Absyn_Local_b_struct*_tmp188=(struct Cyc_Absyn_Local_b_struct*)
_tmp187;if(_tmp188->tag != 4)goto _LLD6;else{_tmp189=_tmp188->f1;}};}}_LLD5:
_tmp18D=_tmp189;goto _LLD7;_LLD6: {struct Cyc_Absyn_Var_e_struct*_tmp18A=(struct
Cyc_Absyn_Var_e_struct*)_tmp181;if(_tmp18A->tag != 1)goto _LLD8;else{_tmp18B=(void*)
_tmp18A->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp18C=(struct Cyc_Absyn_Pat_b_struct*)
_tmp18B;if(_tmp18C->tag != 5)goto _LLD8;else{_tmp18D=_tmp18C->f1;}};}}_LLD7:
_tmp191=_tmp18D;goto _LLD9;_LLD8: {struct Cyc_Absyn_Var_e_struct*_tmp18E=(struct
Cyc_Absyn_Var_e_struct*)_tmp181;if(_tmp18E->tag != 1)goto _LLDA;else{_tmp18F=(void*)
_tmp18E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp190=(struct Cyc_Absyn_Global_b_struct*)
_tmp18F;if(_tmp190->tag != 1)goto _LLDA;else{_tmp191=_tmp190->f1;}};}}_LLD9: if(!(
_tmp191->tq).real_const)return;goto _LLD1;_LLDA: {struct Cyc_Absyn_Subscript_e_struct*
_tmp192=(struct Cyc_Absyn_Subscript_e_struct*)_tmp181;if(_tmp192->tag != 24)goto
_LLDC;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LLDB:{void*_tmp1A1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp193->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1A3;struct Cyc_Absyn_Tqual _tmp1A4;struct Cyc_Absyn_ArrayInfo _tmp1A6;struct Cyc_Absyn_Tqual
_tmp1A7;struct Cyc_List_List*_tmp1A9;_LLE9: {struct Cyc_Absyn_PointerType_struct*
_tmp1A2=(struct Cyc_Absyn_PointerType_struct*)_tmp1A1;if(_tmp1A2->tag != 5)goto
_LLEB;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A3.elt_tq;}}_LLEA: _tmp1A7=_tmp1A4;
goto _LLEC;_LLEB: {struct Cyc_Absyn_ArrayType_struct*_tmp1A5=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1A1;if(_tmp1A5->tag != 9)goto _LLED;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.tq;}}
_LLEC: if(!_tmp1A7.real_const)return;goto _LLE8;_LLED: {struct Cyc_Absyn_TupleType_struct*
_tmp1A8=(struct Cyc_Absyn_TupleType_struct*)_tmp1A1;if(_tmp1A8->tag != 11)goto
_LLEF;else{_tmp1A9=_tmp1A8->f1;}}_LLEE: {unsigned int _tmp1AB;int _tmp1AC;struct
_tuple14 _tmp1AA=Cyc_Evexp_eval_const_uint_exp(_tmp194);_tmp1AB=_tmp1AA.f1;
_tmp1AC=_tmp1AA.f2;if(!_tmp1AC){{const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(
e->loc,((_tmp771="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp771,sizeof(char),47))),_tag_dyneither(_tmp770,sizeof(void*),0)));}return;}{
struct _handler_cons _tmp1AF;_push_handler(& _tmp1AF);{int _tmp1B1=0;if(setjmp(
_tmp1AF.handler))_tmp1B1=1;if(!_tmp1B1){{struct _tuple18 _tmp1B3;struct Cyc_Absyn_Tqual
_tmp1B4;struct _tuple18*_tmp1B2=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp1A9,(int)_tmp1AB);_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.f1;if(!
_tmp1B4.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp1B0=(
void*)_exn_thrown;void*_tmp1B6=_tmp1B0;_LLF2: {struct Cyc_List_Nth_struct*_tmp1B7=(
struct Cyc_List_Nth_struct*)_tmp1B6;if(_tmp1B7->tag != Cyc_List_Nth)goto _LLF4;}
_LLF3: return;_LLF4:;_LLF5:(void)_throw(_tmp1B6);_LLF1:;}};}goto _LLE8;}_LLEF:;
_LLF0: goto _LLE8;_LLE8:;}goto _LLD1;_LLDC: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp195=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp181;if(_tmp195->tag != 22)goto
_LLDE;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LLDD:{void*_tmp1B8=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp196->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp1BA;union Cyc_Absyn_AggrInfoU _tmp1BB;struct Cyc_Absyn_Aggrdecl**_tmp1BC;struct
Cyc_List_List*_tmp1BE;_LLF7: {struct Cyc_Absyn_AggrType_struct*_tmp1B9=(struct Cyc_Absyn_AggrType_struct*)
_tmp1B8;if(_tmp1B9->tag != 12)goto _LLF9;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1BA.aggr_info;
if((_tmp1BB.KnownAggr).tag != 2)goto _LLF9;_tmp1BC=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1BB.KnownAggr).val;}}_LLF8: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp1BC == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1BC,_tmp197);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF6;}_LLF9: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1BD=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B8;if(_tmp1BD->tag != 13)goto _LLFB;else{_tmp1BE=_tmp1BD->f2;}}_LLFA: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1BE,_tmp197);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF6;}_LLFB:;_LLFC: goto _LLF6;_LLF6:;}goto _LLD1;_LLDE: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp198=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp181;if(_tmp198->tag != 23)goto
_LLE0;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LLDF:{void*_tmp1BF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp199->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1C1;void*_tmp1C2;_LLFE: {struct Cyc_Absyn_PointerType_struct*_tmp1C0=(struct
Cyc_Absyn_PointerType_struct*)_tmp1BF;if(_tmp1C0->tag != 5)goto _LL100;else{
_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C1.elt_typ;}}_LLFF:{void*_tmp1C3=Cyc_Tcutil_compress(
_tmp1C2);struct Cyc_Absyn_AggrInfo _tmp1C5;union Cyc_Absyn_AggrInfoU _tmp1C6;struct
Cyc_Absyn_Aggrdecl**_tmp1C7;struct Cyc_List_List*_tmp1C9;_LL103: {struct Cyc_Absyn_AggrType_struct*
_tmp1C4=(struct Cyc_Absyn_AggrType_struct*)_tmp1C3;if(_tmp1C4->tag != 12)goto
_LL105;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.aggr_info;if((_tmp1C6.KnownAggr).tag
!= 2)goto _LL105;_tmp1C7=(struct Cyc_Absyn_Aggrdecl**)(_tmp1C6.KnownAggr).val;}}
_LL104: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp1C7 == 0?0:
Cyc_Absyn_lookup_decl_field(*_tmp1C7,_tmp19A);if(sf == 0  || !(sf->tq).real_const)
return;goto _LL102;}_LL105: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1C8=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp1C3;if(_tmp1C8->tag != 13)goto _LL107;else{
_tmp1C9=_tmp1C8->f2;}}_LL106: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp1C9,_tmp19A);if(sf == 0  || !(sf->tq).real_const)return;goto _LL102;}_LL107:;
_LL108: goto _LL102;_LL102:;}goto _LLFD;_LL100:;_LL101: goto _LLFD;_LLFD:;}goto _LLD1;
_LLE0: {struct Cyc_Absyn_Deref_e_struct*_tmp19B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp181;if(_tmp19B->tag != 21)goto _LLE2;else{_tmp19C=_tmp19B->f1;}}_LLE1:{void*
_tmp1CA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp19C->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1CC;struct Cyc_Absyn_Tqual _tmp1CD;struct Cyc_Absyn_ArrayInfo
_tmp1CF;struct Cyc_Absyn_Tqual _tmp1D0;_LL10A: {struct Cyc_Absyn_PointerType_struct*
_tmp1CB=(struct Cyc_Absyn_PointerType_struct*)_tmp1CA;if(_tmp1CB->tag != 5)goto
_LL10C;else{_tmp1CC=_tmp1CB->f1;_tmp1CD=_tmp1CC.elt_tq;}}_LL10B: _tmp1D0=_tmp1CD;
goto _LL10D;_LL10C: {struct Cyc_Absyn_ArrayType_struct*_tmp1CE=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1CA;if(_tmp1CE->tag != 9)goto _LL10E;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CF.tq;}}
_LL10D: if(!_tmp1D0.real_const)return;goto _LL109;_LL10E:;_LL10F: goto _LL109;_LL109:;}
goto _LLD1;_LLE2: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp19D=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp181;if(_tmp19D->tag != 13)goto _LLE4;else{_tmp19E=_tmp19D->f1;}}_LLE3: _tmp1A0=
_tmp19E;goto _LLE5;_LLE4: {struct Cyc_Absyn_Instantiate_e_struct*_tmp19F=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmp181;if(_tmp19F->tag != 14)goto _LLE6;else{
_tmp1A0=_tmp19F->f1;}}_LLE5: Cyc_Tcexp_check_writable(te,_tmp1A0);return;_LLE6:;
_LLE7: goto _LLD1;_LLD1:;}{const char*_tmp775;void*_tmp774[1];struct Cyc_String_pa_struct
_tmp773;(_tmp773.tag=0,((_tmp773.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp774[0]=& _tmp773,Cyc_Tcutil_terr(e->loc,((
_tmp775="attempt to write a const location: %s",_tag_dyneither(_tmp775,sizeof(
char),38))),_tag_dyneither(_tmp774,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp778;void*_tmp777;return(_tmp777=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp778="increment/decrement of non-lvalue",_tag_dyneither(_tmp778,
sizeof(char),34))),_tag_dyneither(_tmp777,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp77B;void*_tmp77A;(_tmp77A=0,Cyc_Tcutil_terr(e->loc,((
_tmp77B="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp77B,sizeof(char),50))),_tag_dyneither(_tmp77A,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp77E;void*_tmp77D;(_tmp77D=0,Cyc_Tcutil_terr(e->loc,((_tmp77E="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp77E,sizeof(char),54))),_tag_dyneither(_tmp77D,sizeof(void*),0)));}}
else{const char*_tmp782;void*_tmp781[1];struct Cyc_String_pa_struct _tmp780;(
_tmp780.tag=0,((_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp781[0]=& _tmp780,Cyc_Tcutil_terr(e->loc,((_tmp782="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp782,sizeof(char),44))),_tag_dyneither(_tmp781,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1DF;int _tmp1E0;const char*_tmp783;struct Cyc_Tcexp_TestEnv
_tmp1DE=Cyc_Tcexp_tcTest(te,e1,((_tmp783="conditional expression",_tag_dyneither(
_tmp783,sizeof(char),23))));_tmp1DF=_tmp1DE.eq;_tmp1E0=_tmp1DE.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp784;
struct Cyc_List_List _tmp1E1=(_tmp784.hd=e3,((_tmp784.tl=0,_tmp784)));struct Cyc_List_List
_tmp785;struct Cyc_List_List _tmp1E2=(_tmp785.hd=e2,((_tmp785.tl=(struct Cyc_List_List*)&
_tmp1E1,_tmp785)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1E2)){{const char*_tmp78A;void*_tmp789[2];struct Cyc_String_pa_struct _tmp788;
struct Cyc_String_pa_struct _tmp787;(_tmp787.tag=0,((_tmp787.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp788.tag=0,((_tmp788.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp789[0]=& _tmp788,((_tmp789[1]=& _tmp787,Cyc_Tcutil_terr(
loc,((_tmp78A="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp78A,sizeof(char),48))),_tag_dyneither(_tmp789,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp78B;Cyc_Tcexp_tcTest(te,e1,((_tmp78B="logical-and expression",
_tag_dyneither(_tmp78B,sizeof(char),23))));}{const char*_tmp78C;Cyc_Tcexp_tcTest(
te,e2,((_tmp78C="logical-and expression",_tag_dyneither(_tmp78C,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp78D;Cyc_Tcexp_tcTest(te,e1,((_tmp78D="logical-or expression",
_tag_dyneither(_tmp78D,sizeof(char),22))));}{const char*_tmp78E;Cyc_Tcexp_tcTest(
te,e2,((_tmp78E="logical-or expression",_tag_dyneither(_tmp78E,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1ED=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1ED,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1EE=Cyc_Tcutil_compress(
t1);_LL111: {struct Cyc_Absyn_ArrayType_struct*_tmp1EF=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1EE;if(_tmp1EF->tag != 9)goto _LL113;}_LL112:{const char*_tmp791;void*_tmp790;(
_tmp790=0,Cyc_Tcutil_terr(loc,((_tmp791="cannot assign to an array",
_tag_dyneither(_tmp791,sizeof(char),26))),_tag_dyneither(_tmp790,sizeof(void*),0)));}
goto _LL110;_LL113:;_LL114: goto _LL110;_LL110:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),& Cyc_Tcutil_tmk)){const char*_tmp794;void*_tmp793;(_tmp793=0,Cyc_Tcutil_terr(
loc,((_tmp794="type is abstract (can't determine size).",_tag_dyneither(_tmp794,
sizeof(char),41))),_tag_dyneither(_tmp793,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp797;void*_tmp796;return(_tmp796=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp797="assignment to non-lvalue",_tag_dyneither(_tmp797,sizeof(char),25))),
_tag_dyneither(_tmp796,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp1F6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F6,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F6,e2)){const char*_tmp79A;void*
_tmp799;(_tmp799=0,Cyc_Tcutil_terr(e2->loc,((_tmp79A="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp79A,sizeof(char),49))),_tag_dyneither(_tmp799,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp79F;void*_tmp79E[2];struct
Cyc_String_pa_struct _tmp79D;struct Cyc_String_pa_struct _tmp79C;void*_tmp1F9=(
_tmp79C.tag=0,((_tmp79C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp79D.tag=0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp79E[0]=& _tmp79D,((_tmp79E[1]=& _tmp79C,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp79F="type mismatch: %s != %s",_tag_dyneither(_tmp79F,sizeof(
char),24))),_tag_dyneither(_tmp79E,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1F9;}}else{enum Cyc_Absyn_Primop
_tmp1FE=(enum Cyc_Absyn_Primop)po->v;void*_tmp1FF=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp1FE,e1,e2);if(!(Cyc_Tcutil_unify(_tmp1FF,t1) || Cyc_Tcutil_coerceable(
_tmp1FF) && Cyc_Tcutil_coerceable(t1))){const char*_tmp7A4;void*_tmp7A3[2];struct
Cyc_String_pa_struct _tmp7A2;struct Cyc_String_pa_struct _tmp7A1;void*_tmp200=(
_tmp7A1.tag=0,((_tmp7A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7A3[0]=& _tmp7A2,((_tmp7A3[1]=& _tmp7A1,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7A4="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp7A4,sizeof(char),82))),_tag_dyneither(_tmp7A3,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp1FF,t1);Cyc_Tcutil_explain_failure();return _tmp200;}return
_tmp1FF;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp205=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp208;enum Cyc_Absyn_Size_of _tmp20A;_LL116: {struct
Cyc_Absyn_FloatType_struct*_tmp206=(struct Cyc_Absyn_FloatType_struct*)_tmp205;
if(_tmp206->tag != 7)goto _LL118;}_LL117: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL115;_LL118: {struct Cyc_Absyn_IntType_struct*
_tmp207=(struct Cyc_Absyn_IntType_struct*)_tmp205;if(_tmp207->tag != 6)goto _LL11A;
else{_tmp208=_tmp207->f2;if(_tmp208 != Cyc_Absyn_Char_sz)goto _LL11A;}}_LL119: goto
_LL11B;_LL11A: {struct Cyc_Absyn_IntType_struct*_tmp209=(struct Cyc_Absyn_IntType_struct*)
_tmp205;if(_tmp209->tag != 6)goto _LL11C;else{_tmp20A=_tmp209->f2;if(_tmp20A != Cyc_Absyn_Short_sz)
goto _LL11C;}}_LL11B: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL115;_LL11C:;_LL11D: goto _LL115;_LL115:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp20C;struct Cyc_List_List*
_tmp20D;struct Cyc_Position_Segment*_tmp20E;enum Cyc_Absyn_Scope _tmp20F;struct Cyc_Absyn_Datatypefield
_tmp20B=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp20C=_tmp20B.name;
_tmp20D=_tmp20B.typs;_tmp20E=_tmp20B.loc;_tmp20F=_tmp20B.sc;if(_tmp20D == 0  || 
_tmp20D->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple18*)_tmp20D->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_Position_Segment*
_tmp213;enum Cyc_Absyn_Scope _tmp214;struct Cyc_Absyn_Datatypefield _tmp210=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp211=_tmp210.name;_tmp212=_tmp210.typs;
_tmp213=_tmp210.loc;_tmp214=_tmp210.sc;if(_tmp212 == 0  || _tmp212->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp212->hd)).f2);
int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp7A9;void*_tmp7A8[2];
struct Cyc_String_pa_struct _tmp7A7;struct Cyc_String_pa_struct _tmp7A6;(_tmp7A6.tag=
0,((_tmp7A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7A7.tag=0,((_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp7A8[0]=& _tmp7A7,((_tmp7A8[1]=& _tmp7A6,Cyc_Tcutil_terr(
e->loc,((_tmp7A9="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7A9,sizeof(char),54))),_tag_dyneither(_tmp7A8,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp21A=args;int _tmp21B=0;struct _RegionHandle*_tmp21C=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp21D=Cyc_Tcenv_new_block(_tmp21C,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp21D,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp21E=t;struct Cyc_Absyn_PtrInfo _tmp220;void*
_tmp221;struct Cyc_Absyn_Tqual _tmp222;struct Cyc_Absyn_PtrAtts _tmp223;void*_tmp224;
union Cyc_Absyn_Constraint*_tmp225;union Cyc_Absyn_Constraint*_tmp226;union Cyc_Absyn_Constraint*
_tmp227;_LL11F: {struct Cyc_Absyn_PointerType_struct*_tmp21F=(struct Cyc_Absyn_PointerType_struct*)
_tmp21E;if(_tmp21F->tag != 5)goto _LL121;else{_tmp220=_tmp21F->f1;_tmp221=_tmp220.elt_typ;
_tmp222=_tmp220.elt_tq;_tmp223=_tmp220.ptr_atts;_tmp224=_tmp223.rgn;_tmp225=
_tmp223.nullable;_tmp226=_tmp223.bounds;_tmp227=_tmp223.zero_term;}}_LL120: Cyc_Tcenv_check_rgn_accessible(
_tmp21D,loc,_tmp224);Cyc_Tcutil_check_nonzero_bound(loc,_tmp226);{void*_tmp228=
Cyc_Tcutil_compress(_tmp221);struct Cyc_Absyn_FnInfo _tmp22A;struct Cyc_List_List*
_tmp22B;struct Cyc_Core_Opt*_tmp22C;struct Cyc_Absyn_Tqual _tmp22D;void*_tmp22E;
struct Cyc_List_List*_tmp22F;int _tmp230;struct Cyc_Absyn_VarargInfo*_tmp231;struct
Cyc_List_List*_tmp232;struct Cyc_List_List*_tmp233;_LL124: {struct Cyc_Absyn_FnType_struct*
_tmp229=(struct Cyc_Absyn_FnType_struct*)_tmp228;if(_tmp229->tag != 10)goto _LL126;
else{_tmp22A=_tmp229->f1;_tmp22B=_tmp22A.tvars;_tmp22C=_tmp22A.effect;_tmp22D=
_tmp22A.ret_tqual;_tmp22E=_tmp22A.ret_typ;_tmp22F=_tmp22A.args;_tmp230=_tmp22A.c_varargs;
_tmp231=_tmp22A.cyc_varargs;_tmp232=_tmp22A.rgn_po;_tmp233=_tmp22A.attributes;}}
_LL125: if(topt != 0)Cyc_Tcutil_unify(_tmp22E,*topt);while(_tmp21A != 0  && _tmp22F
!= 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;
void*t2=(*((struct _tuple8*)_tmp22F->hd)).f3;Cyc_Tcexp_tcExp(_tmp21D,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp21D,e1,t2,& alias_coercion)){{const char*_tmp7AE;
void*_tmp7AD[2];struct Cyc_String_pa_struct _tmp7AC;struct Cyc_String_pa_struct
_tmp7AB;(_tmp7AB.tag=0,((_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7AC.tag=0,((_tmp7AC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp7AD[0]=& _tmp7AC,((_tmp7AD[1]=& _tmp7AB,Cyc_Tcutil_terr(
e1->loc,((_tmp7AE="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7AE,sizeof(char),57))),_tag_dyneither(_tmp7AD,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7AF;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7AF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp7AF)),((_tmp7AF->hd=(
void*)_tmp21B,((_tmp7AF->tl=*alias_arg_exps,_tmp7AF))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp21C,t2) && !Cyc_Tcutil_is_noalias_path(_tmp21C,e1)){const char*_tmp7B2;void*
_tmp7B1;(_tmp7B1=0,Cyc_Tcutil_terr(e1->loc,((_tmp7B2="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7B2,sizeof(char),49))),_tag_dyneither(_tmp7B1,sizeof(void*),0)));}
_tmp21A=_tmp21A->tl;_tmp22F=_tmp22F->tl;++ _tmp21B;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp233;for(0;a != 0;a=a->tl){void*_tmp23B=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp23D;int _tmp23E;int _tmp23F;_LL129: {struct Cyc_Absyn_Format_att_struct*
_tmp23C=(struct Cyc_Absyn_Format_att_struct*)_tmp23B;if(_tmp23C->tag != 19)goto
_LL12B;else{_tmp23D=_tmp23C->f1;_tmp23E=_tmp23C->f2;_tmp23F=_tmp23C->f3;}}_LL12A:{
struct _handler_cons _tmp240;_push_handler(& _tmp240);{int _tmp242=0;if(setjmp(
_tmp240.handler))_tmp242=1;if(!_tmp242){{struct Cyc_Absyn_Exp*_tmp243=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp23E - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp23F == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp7B3;fmt_args=((
_tmp7B3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp7B3)),((_tmp7B3->v=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,
_tmp23F - 1),_tmp7B3))));}args_already_checked=1;{struct _RegionHandle*_tmp245=Cyc_Tcenv_get_fnrgn(
te_orig);switch(_tmp23D){case Cyc_Absyn_Printf_ft: _LL12D: Cyc_Tcexp_check_format_args(
_tmp21D,_tmp243,fmt_args,_tmp23F - 1,alias_arg_exps,_tmp245,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL12E: Cyc_Tcexp_check_format_args(_tmp21D,_tmp243,
fmt_args,_tmp23F - 1,alias_arg_exps,_tmp245,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp241=(void*)_exn_thrown;void*_tmp247=_tmp241;_LL131: {
struct Cyc_List_Nth_struct*_tmp248=(struct Cyc_List_Nth_struct*)_tmp247;if(_tmp248->tag
!= Cyc_List_Nth)goto _LL133;}_LL132:{const char*_tmp7B6;void*_tmp7B5;(_tmp7B5=0,
Cyc_Tcutil_terr(loc,((_tmp7B6="bad format arguments",_tag_dyneither(_tmp7B6,
sizeof(char),21))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));}goto _LL130;_LL133:;
_LL134:(void)_throw(_tmp247);_LL130:;}};}goto _LL128;_LL12B:;_LL12C: goto _LL128;
_LL128:;}}if(_tmp22F != 0){const char*_tmp7B9;void*_tmp7B8;(_tmp7B8=0,Cyc_Tcutil_terr(
loc,((_tmp7B9="too few arguments for function",_tag_dyneither(_tmp7B9,sizeof(
char),31))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}else{if((_tmp21A != 0  || 
_tmp230) || _tmp231 != 0){if(_tmp230)for(0;_tmp21A != 0;_tmp21A=_tmp21A->tl){Cyc_Tcexp_tcExp(
_tmp21D,0,(struct Cyc_Absyn_Exp*)_tmp21A->hd);}else{if(_tmp231 == 0){const char*
_tmp7BC;void*_tmp7BB;(_tmp7BB=0,Cyc_Tcutil_terr(loc,((_tmp7BC="too many arguments for function",
_tag_dyneither(_tmp7BC,sizeof(char),32))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}
else{void*_tmp250;int _tmp251;struct Cyc_Absyn_VarargInfo _tmp24F=*_tmp231;_tmp250=
_tmp24F.type;_tmp251=_tmp24F.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp7BD;
struct Cyc_Absyn_VarargCallInfo*_tmp252=(_tmp7BD=_cycalloc(sizeof(*_tmp7BD)),((
_tmp7BD->num_varargs=0,((_tmp7BD->injectors=0,((_tmp7BD->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp231,_tmp7BD)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp252;if(!_tmp251)for(0;_tmp21A != 0;(_tmp21A=_tmp21A->tl,_tmp21B ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;++
_tmp252->num_varargs;Cyc_Tcexp_tcExp(_tmp21D,(void**)& _tmp250,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp21D,e1,_tmp250,& alias_coercion)){{const char*_tmp7C2;void*_tmp7C1[2];struct
Cyc_String_pa_struct _tmp7C0;struct Cyc_String_pa_struct _tmp7BF;(_tmp7BF.tag=0,((
_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp7C0.tag=0,((
_tmp7C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp250)),((_tmp7C1[0]=& _tmp7C0,((_tmp7C1[1]=& _tmp7BF,Cyc_Tcutil_terr(loc,((
_tmp7C2="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp7C2,sizeof(char),49))),_tag_dyneither(_tmp7C1,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp7C3;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp7C3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp7C3)),((_tmp7C3->hd=(void*)_tmp21B,((_tmp7C3->tl=*alias_arg_exps,
_tmp7C3))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp21C,_tmp250) && !Cyc_Tcutil_is_noalias_path(
_tmp21C,e1)){const char*_tmp7C6;void*_tmp7C5;(_tmp7C5=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7C6="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7C6,sizeof(char),49))),_tag_dyneither(_tmp7C5,sizeof(void*),0)));}}else{void*
_tmp25A=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp250));struct Cyc_Absyn_DatatypeInfo
_tmp25C;union Cyc_Absyn_DatatypeInfoU _tmp25D;struct Cyc_Absyn_Datatypedecl**
_tmp25E;struct Cyc_Absyn_Datatypedecl*_tmp25F;struct Cyc_List_List*_tmp260;_LL136: {
struct Cyc_Absyn_DatatypeType_struct*_tmp25B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp25A;if(_tmp25B->tag != 3)goto _LL138;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25C.datatype_info;
if((_tmp25D.KnownDatatype).tag != 2)goto _LL138;_tmp25E=(struct Cyc_Absyn_Datatypedecl**)(
_tmp25D.KnownDatatype).val;_tmp25F=*_tmp25E;_tmp260=_tmp25C.targs;}}_LL137: {
struct Cyc_Absyn_Datatypedecl*_tmp261=*Cyc_Tcenv_lookup_datatypedecl(_tmp21D,loc,
_tmp25F->name);struct Cyc_List_List*fields=0;if(_tmp261->fields == 0){const char*
_tmp7CA;void*_tmp7C9[1];struct Cyc_String_pa_struct _tmp7C8;(_tmp7C8.tag=0,((
_tmp7C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp250)),((_tmp7C9[0]=& _tmp7C8,Cyc_Tcutil_terr(loc,((_tmp7CA="can't inject into abstract datatype %s",
_tag_dyneither(_tmp7CA,sizeof(char),39))),_tag_dyneither(_tmp7C9,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp261->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp250),Cyc_Tcenv_curr_rgn(
_tmp21D))){const char*_tmp7CF;void*_tmp7CE[2];struct Cyc_String_pa_struct _tmp7CD;
struct Cyc_String_pa_struct _tmp7CC;(_tmp7CC.tag=0,((_tmp7CC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp21D))),((_tmp7CD.tag=0,((_tmp7CD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp250)),((_tmp7CE[0]=& _tmp7CD,((_tmp7CE[
1]=& _tmp7CC,Cyc_Tcutil_terr(loc,((_tmp7CF="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp7CF,sizeof(char),49))),_tag_dyneither(_tmp7CE,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp269=Cyc_Tcenv_get_fnrgn(_tmp21D);{struct Cyc_List_List*
_tmp26A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp269,_tmp269,
_tmp261->tvs,_tmp260);for(0;_tmp21A != 0;_tmp21A=_tmp21A->tl){++ _tmp252->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp21D,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp269,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp269,e1)){const char*_tmp7D2;void*_tmp7D1;(_tmp7D1=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7D2="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7D2,sizeof(char),49))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp26D=Cyc_Tcexp_tcInjection(_tmp21D,e1,Cyc_Tcutil_pointer_elt_type(
_tmp250),_tmp269,_tmp26A,fields);if(_tmp26D != 0){struct Cyc_List_List*_tmp7D3;
_tmp252->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp252->injectors,((_tmp7D3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp7D3)),((_tmp7D3->hd=(struct Cyc_Absyn_Datatypefield*)_tmp26D,((
_tmp7D3->tl=0,_tmp7D3)))))));}};};}}goto _LL135;};}_LL138:;_LL139:{const char*
_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(loc,((_tmp7D6="bad inject vararg type",
_tag_dyneither(_tmp7D6,sizeof(char),23))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}
goto _LL135;_LL135:;}};}}}}Cyc_Tcenv_check_effect_accessible(_tmp21D,loc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp22C))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp21D,loc,_tmp232);return _tmp22E;};_LL126:;_LL127: {const char*_tmp7D9;void*
_tmp7D8;return(_tmp7D8=0,Cyc_Tcexp_expr_err(_tmp21D,loc,topt,((_tmp7D9="expected pointer to function",
_tag_dyneither(_tmp7D9,sizeof(char),29))),_tag_dyneither(_tmp7D8,sizeof(void*),0)));}
_LL123:;};_LL121:;_LL122: {const char*_tmp7DC;void*_tmp7DB;return(_tmp7DB=0,Cyc_Tcexp_expr_err(
_tmp21D,loc,topt,((_tmp7DC="expected pointer to function",_tag_dyneither(_tmp7DC,
sizeof(char),29))),_tag_dyneither(_tmp7DB,sizeof(void*),0)));}_LL11E:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp276=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp276,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp276,&
bogus)){const char*_tmp7E1;void*_tmp7E0[2];struct Cyc_String_pa_struct _tmp7DF;
struct Cyc_String_pa_struct _tmp7DE;(_tmp7DE.tag=0,((_tmp7DE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7DF.tag=0,((_tmp7DF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp276)),((_tmp7E0[0]=& _tmp7DF,((
_tmp7E0[1]=& _tmp7DE,Cyc_Tcutil_terr(loc,((_tmp7E1="expected %s but found %s",
_tag_dyneither(_tmp7E1,sizeof(char),25))),_tag_dyneither(_tmp7E0,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp27B=t1;struct Cyc_Absyn_PtrInfo _tmp27D;void*
_tmp27E;struct Cyc_Absyn_Tqual _tmp27F;struct Cyc_Absyn_PtrAtts _tmp280;void*_tmp281;
union Cyc_Absyn_Constraint*_tmp282;union Cyc_Absyn_Constraint*_tmp283;union Cyc_Absyn_Constraint*
_tmp284;_LL13B: {struct Cyc_Absyn_PointerType_struct*_tmp27C=(struct Cyc_Absyn_PointerType_struct*)
_tmp27B;if(_tmp27C->tag != 5)goto _LL13D;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27D.elt_typ;
_tmp27F=_tmp27D.elt_tq;_tmp280=_tmp27D.ptr_atts;_tmp281=_tmp280.rgn;_tmp282=
_tmp280.nullable;_tmp283=_tmp280.bounds;_tmp284=_tmp280.zero_term;}}_LL13C:{void*
_tmp285=Cyc_Tcutil_compress(_tmp27E);struct Cyc_Absyn_FnInfo _tmp287;struct Cyc_List_List*
_tmp288;struct Cyc_Core_Opt*_tmp289;struct Cyc_Absyn_Tqual _tmp28A;void*_tmp28B;
struct Cyc_List_List*_tmp28C;int _tmp28D;struct Cyc_Absyn_VarargInfo*_tmp28E;struct
Cyc_List_List*_tmp28F;struct Cyc_List_List*_tmp290;_LL140: {struct Cyc_Absyn_FnType_struct*
_tmp286=(struct Cyc_Absyn_FnType_struct*)_tmp285;if(_tmp286->tag != 10)goto _LL142;
else{_tmp287=_tmp286->f1;_tmp288=_tmp287.tvars;_tmp289=_tmp287.effect;_tmp28A=
_tmp287.ret_tqual;_tmp28B=_tmp287.ret_typ;_tmp28C=_tmp287.args;_tmp28D=_tmp287.c_varargs;
_tmp28E=_tmp287.cyc_varargs;_tmp28F=_tmp287.rgn_po;_tmp290=_tmp287.attributes;}}
_LL141: {struct _RegionHandle*_tmp291=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*
instantiation=0;for(0;ts != 0  && _tmp288 != 0;(ts=ts->tl,_tmp288=_tmp288->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp288->hd,&
Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(
void*)ts->hd);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{struct _tuple12*
_tmp7E4;struct Cyc_List_List*_tmp7E3;instantiation=((_tmp7E3=_region_malloc(
_tmp291,sizeof(*_tmp7E3)),((_tmp7E3->hd=((_tmp7E4=_region_malloc(_tmp291,sizeof(*
_tmp7E4)),((_tmp7E4->f1=(struct Cyc_Absyn_Tvar*)_tmp288->hd,((_tmp7E4->f2=(void*)
ts->hd,_tmp7E4)))))),((_tmp7E3->tl=instantiation,_tmp7E3))))));};}if(ts != 0){
const char*_tmp7E7;void*_tmp7E6;return(_tmp7E6=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp7E7="too many type variables in instantiation",_tag_dyneither(_tmp7E7,
sizeof(char),41))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}{struct Cyc_Absyn_FnType_struct
_tmp7ED;struct Cyc_Absyn_FnInfo _tmp7EC;struct Cyc_Absyn_FnType_struct*_tmp7EB;void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp291,instantiation,(void*)((_tmp7EB=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp7EB)),((_tmp7EB[0]=((_tmp7ED.tag=
10,((_tmp7ED.f1=((_tmp7EC.tvars=_tmp288,((_tmp7EC.effect=_tmp289,((_tmp7EC.ret_tqual=
_tmp28A,((_tmp7EC.ret_typ=_tmp28B,((_tmp7EC.args=_tmp28C,((_tmp7EC.c_varargs=
_tmp28D,((_tmp7EC.cyc_varargs=_tmp28E,((_tmp7EC.rgn_po=_tmp28F,((_tmp7EC.attributes=
_tmp290,_tmp7EC)))))))))))))))))),_tmp7ED)))),_tmp7EB)))));struct Cyc_Absyn_PointerType_struct
_tmp7F7;struct Cyc_Absyn_PtrAtts _tmp7F6;struct Cyc_Absyn_PtrInfo _tmp7F5;struct Cyc_Absyn_PointerType_struct*
_tmp7F4;return(void*)((_tmp7F4=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp7F4)),((_tmp7F4[0]=((_tmp7F7.tag=5,((_tmp7F7.f1=((_tmp7F5.elt_typ=new_fn_typ,((
_tmp7F5.elt_tq=_tmp27F,((_tmp7F5.ptr_atts=((_tmp7F6.rgn=_tmp281,((_tmp7F6.nullable=
_tmp282,((_tmp7F6.bounds=_tmp283,((_tmp7F6.zero_term=_tmp284,((_tmp7F6.ptrloc=0,
_tmp7F6)))))))))),_tmp7F5)))))),_tmp7F7)))),_tmp7F4))));};}_LL142:;_LL143: goto
_LL13F;_LL13F:;}goto _LL13A;_LL13D:;_LL13E: goto _LL13A;_LL13A:;}{const char*_tmp7FB;
void*_tmp7FA[1];struct Cyc_String_pa_struct _tmp7F9;return(_tmp7F9.tag=0,((_tmp7F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp7FA[0]=& _tmp7F9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FB="expecting polymorphic type but found %s",
_tag_dyneither(_tmp7FB,sizeof(char),40))),_tag_dyneither(_tmp7FA,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(
loc,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp800;void*_tmp7FF[2];struct Cyc_String_pa_struct _tmp7FE;struct Cyc_String_pa_struct
_tmp7FD;void*_tmp2A0=(_tmp7FD.tag=0,((_tmp7FD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7FE.tag=0,((_tmp7FE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7FF[0]=&
_tmp7FE,((_tmp7FF[1]=& _tmp7FD,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp800="cannot cast %s to %s",
_tag_dyneither(_tmp800,sizeof(char),21))),_tag_dyneither(_tmp7FF,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp2A0;};}}}{struct _tuple0 _tmp801;struct
_tuple0 _tmp2A6=(_tmp801.f1=e->r,((_tmp801.f2=Cyc_Tcutil_compress(t),_tmp801)));
void*_tmp2A7;struct Cyc_Absyn_MallocInfo _tmp2A9;int _tmp2AA;void*_tmp2AB;struct Cyc_Absyn_PtrInfo
_tmp2AD;struct Cyc_Absyn_PtrAtts _tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;union
Cyc_Absyn_Constraint*_tmp2B0;union Cyc_Absyn_Constraint*_tmp2B1;_LL145: _tmp2A7=
_tmp2A6.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp2A8=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2A7;if(_tmp2A8->tag != 34)goto _LL147;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A9.fat_result;}};
_tmp2AB=_tmp2A6.f2;{struct Cyc_Absyn_PointerType_struct*_tmp2AC=(struct Cyc_Absyn_PointerType_struct*)
_tmp2AB;if(_tmp2AC->tag != 5)goto _LL147;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AD.ptr_atts;
_tmp2AF=_tmp2AE.nullable;_tmp2B0=_tmp2AE.bounds;_tmp2B1=_tmp2AE.zero_term;}};
_LL146: if((_tmp2AA  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B1)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2AF)){void*_tmp2B2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2B0);struct Cyc_Absyn_Exp*_tmp2B4;_LL14A: {struct Cyc_Absyn_Upper_b_struct*
_tmp2B3=(struct Cyc_Absyn_Upper_b_struct*)_tmp2B2;if(_tmp2B3->tag != 1)goto _LL14C;
else{_tmp2B4=_tmp2B3->f1;}}_LL14B: if((Cyc_Evexp_eval_const_uint_exp(_tmp2B4)).f1
== 1){const char*_tmp804;void*_tmp803;(_tmp803=0,Cyc_Tcutil_warn(loc,((_tmp804="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp804,sizeof(char),60))),_tag_dyneither(_tmp803,sizeof(void*),0)));}
goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}goto _LL144;_LL147:;_LL148: goto
_LL144;_LL144:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp2B7=0;struct Cyc_Absyn_Tqual _tmp2B8=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp2B9=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2BB;
void*_tmp2BC;struct Cyc_Absyn_Tqual _tmp2BD;struct Cyc_Absyn_PtrAtts _tmp2BE;union
Cyc_Absyn_Constraint*_tmp2BF;_LL14F: {struct Cyc_Absyn_PointerType_struct*_tmp2BA=(
struct Cyc_Absyn_PointerType_struct*)_tmp2B9;if(_tmp2BA->tag != 5)goto _LL151;else{
_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BB.elt_typ;_tmp2BD=_tmp2BB.elt_tq;_tmp2BE=
_tmp2BB.ptr_atts;_tmp2BF=_tmp2BE.zero_term;}}_LL150:{void**_tmp805;_tmp2B7=((
_tmp805=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp805)),((_tmp805[0]=
_tmp2BC,_tmp805))));}_tmp2B8=_tmp2BD;goto _LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}{
struct _RegionHandle*_tmp2C1=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp2C1,te),_tmp2B7,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp2C1,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*_tmp808;void*_tmp807;(
_tmp807=0,Cyc_Tcutil_terr(e->loc,((_tmp808="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp808,sizeof(char),46))),_tag_dyneither(_tmp807,sizeof(void*),0)));}{
void*_tmp2C4=e->r;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C7;_LL154: {
struct Cyc_Absyn_Subscript_e_struct*_tmp2C5=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp2C4;if(_tmp2C5->tag != 24)goto _LL156;else{_tmp2C6=_tmp2C5->f1;_tmp2C7=_tmp2C5->f2;}}
_LL155:{void*_tmp2C8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C6->topt))->v);_LL159: {struct Cyc_Absyn_TupleType_struct*
_tmp2C9=(struct Cyc_Absyn_TupleType_struct*)_tmp2C8;if(_tmp2C9->tag != 11)goto
_LL15B;}_LL15A: goto _LL158;_LL15B:;_LL15C: e0->r=(Cyc_Absyn_add_exp(_tmp2C6,
_tmp2C7,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp2C6,_tmp2C7);_LL158:;}goto _LL153;
_LL156:;_LL157: goto _LL153;_LL153:;}{void*_tmp2CA=e->r;int _tmp2CC;int _tmp2CE;
_LL15E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2CB=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2CA;if(_tmp2CB->tag != 22)goto _LL160;else{_tmp2CC=_tmp2CB->f3;if(_tmp2CC != 1)
goto _LL160;}}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2CD=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2CA;if(_tmp2CD->tag != 23)goto
_LL162;else{_tmp2CE=_tmp2CD->f3;if(_tmp2CE != 1)goto _LL162;}}_LL161:{const char*
_tmp80B;void*_tmp80A;(_tmp80A=0,Cyc_Tcutil_terr(e->loc,((_tmp80B="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp80B,sizeof(char),50))),_tag_dyneither(_tmp80A,sizeof(void*),0)));}
goto _LL15D;_LL162:;_LL163: goto _LL15D;_LL15D:;}{int _tmp2D2;void*_tmp2D3;struct
_tuple13 _tmp2D1=Cyc_Tcutil_addressof_props(te,e);_tmp2D2=_tmp2D1.f1;_tmp2D3=
_tmp2D1.f2;if(Cyc_Tcutil_is_noalias_region(_tmp2D3,0)){const char*_tmp80E;void*
_tmp80D;(_tmp80D=0,Cyc_Tcutil_terr(e->loc,((_tmp80E="using & would manufacture an alias to an alias-free pointer",
_tag_dyneither(_tmp80E,sizeof(char),60))),_tag_dyneither(_tmp80D,sizeof(void*),0)));}{
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2D2){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp2D3,tq,Cyc_Absyn_false_conref);return t;};};};};}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)
return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp812;void*_tmp811[1];struct Cyc_String_pa_struct _tmp810;(_tmp810.tag=
0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp811[0]=& _tmp810,Cyc_Tcutil_terr(loc,((_tmp812="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp812,sizeof(char),55))),_tag_dyneither(_tmp811,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2D9=Cyc_Tcutil_compress(*topt);void*_tmp2DB;_LL165: {
struct Cyc_Absyn_TagType_struct*_tmp2DA=(struct Cyc_Absyn_TagType_struct*)_tmp2D9;
if(_tmp2DA->tag != 20)goto _LL167;else{_tmp2DB=(void*)_tmp2DA->f1;}}_LL166: {
struct Cyc_Absyn_Exp*_tmp2DC=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp815;struct Cyc_Absyn_ValueofType_struct*_tmp814;struct Cyc_Absyn_ValueofType_struct*
_tmp2DD=(_tmp814=_cycalloc(sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=19,((
_tmp815.f1=_tmp2DC,_tmp815)))),_tmp814)));if(Cyc_Tcutil_unify(_tmp2DB,(void*)
_tmp2DD))return _tmp2D9;goto _LL164;}_LL167:;_LL168: goto _LL164;_LL164:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);{void*_tmp2E0=n;struct
_dyneither_ptr*_tmp2E2;unsigned int _tmp2E4;_LL16A: {struct Cyc_Absyn_StructField_struct*
_tmp2E1=(struct Cyc_Absyn_StructField_struct*)_tmp2E0;if(_tmp2E1->tag != 0)goto
_LL16C;else{_tmp2E2=_tmp2E1->f1;}}_LL16B: {int bad_type=1;{void*_tmp2E5=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp2E7;union Cyc_Absyn_AggrInfoU _tmp2E8;struct Cyc_Absyn_Aggrdecl**
_tmp2E9;struct Cyc_List_List*_tmp2EB;_LL16F: {struct Cyc_Absyn_AggrType_struct*
_tmp2E6=(struct Cyc_Absyn_AggrType_struct*)_tmp2E5;if(_tmp2E6->tag != 12)goto
_LL171;else{_tmp2E7=_tmp2E6->f1;_tmp2E8=_tmp2E7.aggr_info;if((_tmp2E8.KnownAggr).tag
!= 2)goto _LL171;_tmp2E9=(struct Cyc_Absyn_Aggrdecl**)(_tmp2E8.KnownAggr).val;}}
_LL170: if((*_tmp2E9)->impl == 0)goto _LL16E;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2E2,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2E9)->impl))->fields)){const char*_tmp819;void*_tmp818[1];
struct Cyc_String_pa_struct _tmp817;(_tmp817.tag=0,((_tmp817.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2E2),((_tmp818[0]=& _tmp817,Cyc_Tcutil_terr(
loc,((_tmp819="no field of struct/union has name %s",_tag_dyneither(_tmp819,
sizeof(char),37))),_tag_dyneither(_tmp818,sizeof(void*),1)))))));}bad_type=0;
goto _LL16E;_LL171: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2EA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2E5;if(_tmp2EA->tag != 13)goto _LL173;else{_tmp2EB=_tmp2EA->f2;}}_LL172: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2E2,_tmp2EB)){const char*_tmp81D;void*_tmp81C[1];struct Cyc_String_pa_struct
_tmp81B;(_tmp81B.tag=0,((_tmp81B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2E2),((_tmp81C[0]=& _tmp81B,Cyc_Tcutil_terr(loc,((_tmp81D="no field of struct/union has name %s",
_tag_dyneither(_tmp81D,sizeof(char),37))),_tag_dyneither(_tmp81C,sizeof(void*),1)))))));}
bad_type=0;goto _LL16E;_LL173:;_LL174: goto _LL16E;_LL16E:;}if(bad_type){const char*
_tmp821;void*_tmp820[1];struct Cyc_String_pa_struct _tmp81F;(_tmp81F.tag=0,((
_tmp81F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp820[0]=& _tmp81F,Cyc_Tcutil_terr(loc,((_tmp821="%s is not a known struct/union type",
_tag_dyneither(_tmp821,sizeof(char),36))),_tag_dyneither(_tmp820,sizeof(void*),1)))))));}
goto _LL169;}_LL16C: {struct Cyc_Absyn_TupleIndex_struct*_tmp2E3=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E0;if(_tmp2E3->tag != 1)goto _LL169;else{_tmp2E4=_tmp2E3->f1;}}_LL16D: {int
bad_type=1;{void*_tmp2F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2F7;
union Cyc_Absyn_AggrInfoU _tmp2F8;struct Cyc_Absyn_Aggrdecl**_tmp2F9;struct Cyc_List_List*
_tmp2FB;struct Cyc_List_List*_tmp2FD;struct Cyc_Absyn_DatatypeFieldInfo _tmp2FF;
union Cyc_Absyn_DatatypeFieldInfoU _tmp300;struct _tuple2 _tmp301;struct Cyc_Absyn_Datatypefield*
_tmp302;_LL176: {struct Cyc_Absyn_AggrType_struct*_tmp2F6=(struct Cyc_Absyn_AggrType_struct*)
_tmp2F5;if(_tmp2F6->tag != 12)goto _LL178;else{_tmp2F7=_tmp2F6->f1;_tmp2F8=_tmp2F7.aggr_info;
if((_tmp2F8.KnownAggr).tag != 2)goto _LL178;_tmp2F9=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2F8.KnownAggr).val;}}_LL177: if((*_tmp2F9)->impl == 0)goto _LL175;_tmp2FB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2F9)->impl))->fields;goto _LL179;
_LL178: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2FA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F5;if(_tmp2FA->tag != 13)goto _LL17A;else{_tmp2FB=_tmp2FA->f2;}}_LL179: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FB)<= _tmp2E4){const char*
_tmp826;void*_tmp825[2];struct Cyc_Int_pa_struct _tmp824;struct Cyc_Int_pa_struct
_tmp823;(_tmp823.tag=1,((_tmp823.f1=(unsigned long)((int)_tmp2E4),((_tmp824.tag=
1,((_tmp824.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FB),((_tmp825[0]=& _tmp824,((_tmp825[1]=& _tmp823,Cyc_Tcutil_terr(loc,((
_tmp826="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp826,
sizeof(char),46))),_tag_dyneither(_tmp825,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17A: {struct Cyc_Absyn_TupleType_struct*_tmp2FC=(struct Cyc_Absyn_TupleType_struct*)
_tmp2F5;if(_tmp2FC->tag != 11)goto _LL17C;else{_tmp2FD=_tmp2FC->f1;}}_LL17B: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FD)<= _tmp2E4){const char*
_tmp82B;void*_tmp82A[2];struct Cyc_Int_pa_struct _tmp829;struct Cyc_Int_pa_struct
_tmp828;(_tmp828.tag=1,((_tmp828.f1=(unsigned long)((int)_tmp2E4),((_tmp829.tag=
1,((_tmp829.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FD),((_tmp82A[0]=& _tmp829,((_tmp82A[1]=& _tmp828,Cyc_Tcutil_terr(loc,((
_tmp82B="tuple has too few components: %d <= %d",_tag_dyneither(_tmp82B,sizeof(
char),39))),_tag_dyneither(_tmp82A,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL175;_LL17C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2FE=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2F5;if(_tmp2FE->tag != 4)goto _LL17E;else{_tmp2FF=_tmp2FE->f1;_tmp300=_tmp2FF.field_info;
if((_tmp300.KnownDatatypefield).tag != 2)goto _LL17E;_tmp301=(struct _tuple2)(
_tmp300.KnownDatatypefield).val;_tmp302=_tmp301.f2;}}_LL17D: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp302->typs)< _tmp2E4){const char*_tmp830;
void*_tmp82F[2];struct Cyc_Int_pa_struct _tmp82E;struct Cyc_Int_pa_struct _tmp82D;(
_tmp82D.tag=1,((_tmp82D.f1=(unsigned long)((int)_tmp2E4),((_tmp82E.tag=1,((
_tmp82E.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp302->typs),((_tmp82F[0]=& _tmp82E,((_tmp82F[1]=& _tmp82D,Cyc_Tcutil_terr(loc,((
_tmp830="datatype field has too few components: %d < %d",_tag_dyneither(_tmp830,
sizeof(char),47))),_tag_dyneither(_tmp82F,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17E:;_LL17F: goto _LL175;_LL175:;}if(bad_type){const char*_tmp834;
void*_tmp833[1];struct Cyc_String_pa_struct _tmp832;(_tmp832.tag=0,((_tmp832.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp833[
0]=& _tmp832,Cyc_Tcutil_terr(loc,((_tmp834="%s is not a known type",
_tag_dyneither(_tmp834,sizeof(char),23))),_tag_dyneither(_tmp833,sizeof(void*),1)))))));}
goto _LL169;}_LL169:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp312=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp313=Cyc_Tcenv_clear_notreadctxt(_tmp312,te_orig);Cyc_Tcexp_tcExp(_tmp313,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp314=t;struct Cyc_Absyn_PtrInfo _tmp317;void*_tmp318;struct Cyc_Absyn_PtrAtts
_tmp319;void*_tmp31A;union Cyc_Absyn_Constraint*_tmp31B;union Cyc_Absyn_Constraint*
_tmp31C;_LL181: {struct Cyc_Absyn_Evar_struct*_tmp315=(struct Cyc_Absyn_Evar_struct*)
_tmp314;if(_tmp315->tag != 1)goto _LL183;}_LL182: {struct Cyc_List_List*_tmp31D=Cyc_Tcenv_lookup_type_vars(
_tmp313);struct Cyc_Core_Opt*_tmp835;void*_tmp31E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mko,((_tmp835=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp835)),((
_tmp835->v=_tmp31D,_tmp835)))));struct Cyc_Core_Opt*_tmp836;void*_tmp31F=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp836=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp836)),((_tmp836->v=_tmp31D,_tmp836)))));union Cyc_Absyn_Constraint*
_tmp320=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*
_tmp321=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp837;struct Cyc_Absyn_PtrAtts _tmp322=(_tmp837.rgn=_tmp31F,((_tmp837.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp837.bounds=
_tmp320,((_tmp837.zero_term=_tmp321,((_tmp837.ptrloc=0,_tmp837)))))))));struct
Cyc_Absyn_PointerType_struct _tmp83D;struct Cyc_Absyn_PtrInfo _tmp83C;struct Cyc_Absyn_PointerType_struct*
_tmp83B;struct Cyc_Absyn_PointerType_struct*_tmp323=(_tmp83B=_cycalloc(sizeof(*
_tmp83B)),((_tmp83B[0]=((_tmp83D.tag=5,((_tmp83D.f1=((_tmp83C.elt_typ=_tmp31E,((
_tmp83C.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp83C.ptr_atts=_tmp322,_tmp83C)))))),
_tmp83D)))),_tmp83B)));Cyc_Tcutil_unify(t,(void*)_tmp323);_tmp318=_tmp31E;
_tmp31A=_tmp31F;_tmp31B=_tmp320;_tmp31C=_tmp321;goto _LL184;}_LL183: {struct Cyc_Absyn_PointerType_struct*
_tmp316=(struct Cyc_Absyn_PointerType_struct*)_tmp314;if(_tmp316->tag != 5)goto
_LL185;else{_tmp317=_tmp316->f1;_tmp318=_tmp317.elt_typ;_tmp319=_tmp317.ptr_atts;
_tmp31A=_tmp319.rgn;_tmp31B=_tmp319.bounds;_tmp31C=_tmp319.zero_term;}}_LL184:
Cyc_Tcenv_check_rgn_accessible(_tmp313,loc,_tmp31A);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp31B);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp318),& Cyc_Tcutil_tmk)){
const char*_tmp840;void*_tmp83F;(_tmp83F=0,Cyc_Tcutil_terr(loc,((_tmp840="can't dereference abstract pointer type",
_tag_dyneither(_tmp840,sizeof(char),40))),_tag_dyneither(_tmp83F,sizeof(void*),0)));}
return _tmp318;_LL185:;_LL186: {const char*_tmp844;void*_tmp843[1];struct Cyc_String_pa_struct
_tmp842;return(_tmp842.tag=0,((_tmp842.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp843[0]=& _tmp842,Cyc_Tcexp_expr_err(
_tmp313,loc,topt,((_tmp844="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp844,sizeof(char),39))),_tag_dyneither(_tmp843,sizeof(void*),1)))))));}
_LL180:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp32F=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(_tmp32F,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp330=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp332;
union Cyc_Absyn_AggrInfoU _tmp333;struct Cyc_Absyn_Aggrdecl**_tmp334;struct Cyc_Absyn_Aggrdecl*
_tmp335;struct Cyc_List_List*_tmp336;enum Cyc_Absyn_AggrKind _tmp338;struct Cyc_List_List*
_tmp339;_LL188: {struct Cyc_Absyn_AggrType_struct*_tmp331=(struct Cyc_Absyn_AggrType_struct*)
_tmp330;if(_tmp331->tag != 12)goto _LL18A;else{_tmp332=_tmp331->f1;_tmp333=_tmp332.aggr_info;
if((_tmp333.KnownAggr).tag != 2)goto _LL18A;_tmp334=(struct Cyc_Absyn_Aggrdecl**)(
_tmp333.KnownAggr).val;_tmp335=*_tmp334;_tmp336=_tmp332.targs;}}_LL189: {struct
Cyc_Absyn_Aggrfield*_tmp33E=Cyc_Absyn_lookup_decl_field(_tmp335,f);if(_tmp33E == 
0){const char*_tmp849;void*_tmp848[2];struct Cyc_String_pa_struct _tmp847;struct Cyc_String_pa_struct
_tmp846;return(_tmp846.tag=0,((_tmp846.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp847.tag=0,((_tmp847.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp335->name)),((_tmp848[0]=& _tmp847,((
_tmp848[1]=& _tmp846,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp849="type %s has no %s field",
_tag_dyneither(_tmp849,sizeof(char),24))),_tag_dyneither(_tmp848,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp335->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp343=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp343,_tmp343,_tmp335->tvs,_tmp336);t2=Cyc_Tcutil_rsubstitute(_tmp343,_tmp344,
_tmp33E->type);if(((_tmp335->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp335->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp84D;void*_tmp84C[1];struct Cyc_String_pa_struct _tmp84B;return(
_tmp84B.tag=0,((_tmp84B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp84C[0]=& _tmp84B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84D="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp84D,sizeof(char),56))),_tag_dyneither(_tmp84C,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp335->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp851;void*_tmp850[1];struct Cyc_String_pa_struct _tmp84F;return(
_tmp84F.tag=0,((_tmp84F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp850[0]=& _tmp84F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp851="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp851,sizeof(char),81))),_tag_dyneither(_tmp850,sizeof(void*),1)))))));}}
return t2;};}_LL18A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp337=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp330;if(_tmp337->tag != 13)goto _LL18C;else{_tmp338=_tmp337->f1;_tmp339=_tmp337->f2;}}
_LL18B: {struct Cyc_Absyn_Aggrfield*_tmp34B=Cyc_Absyn_lookup_field(_tmp339,f);if(
_tmp34B == 0){const char*_tmp855;void*_tmp854[1];struct Cyc_String_pa_struct _tmp853;
return(_tmp853.tag=0,((_tmp853.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp854[0]=& _tmp853,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp855="type has no %s field",
_tag_dyneither(_tmp855,sizeof(char),21))),_tag_dyneither(_tmp854,sizeof(void*),1)))))));}
if((_tmp338 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp34B->type)){const char*_tmp859;void*_tmp858[1];struct Cyc_String_pa_struct
_tmp857;return(_tmp857.tag=0,((_tmp857.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp858[0]=& _tmp857,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp859="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp859,sizeof(char),56))),_tag_dyneither(_tmp858,sizeof(void*),1)))))));}return
_tmp34B->type;}_LL18C:{struct Cyc_Absyn_ArrayType_struct*_tmp33A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp330;if(_tmp33A->tag != 9)goto _LL18E;}{const char*_tmp85A;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp85A="size",_tag_dyneither(_tmp85A,sizeof(char),5))))
== 0))goto _LL18E;};_LL18D: goto _LL18F;_LL18E:{struct Cyc_Absyn_PointerType_struct*
_tmp33C=(struct Cyc_Absyn_PointerType_struct*)_tmp330;if(_tmp33C->tag != 5)goto
_LL190;}{const char*_tmp85B;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp85B="size",
_tag_dyneither(_tmp85B,sizeof(char),5))))== 0))goto _LL190;};_LL18F:{const char*
_tmp85F;void*_tmp85E[1];struct Cyc_String_pa_struct _tmp85D;(_tmp85D.tag=0,((
_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp85E[0]=& _tmp85D,Cyc_Tcutil_warn(e->loc,((_tmp85F="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp85F,sizeof(char),54))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp865;struct Cyc_List_List*_tmp864;struct Cyc_Absyn_Primop_e_struct*
_tmp863;outer_e->r=(void*)((_tmp863=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp863)),((_tmp863[0]=((_tmp865.tag=3,((_tmp865.f1=Cyc_Absyn_Numelts,((
_tmp865.f2=((_tmp864=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp864)),((
_tmp864->hd=e,((_tmp864->tl=0,_tmp864)))))),_tmp865)))))),_tmp863))));}return Cyc_Absyn_uint_typ;
_LL190:;_LL191: {const char*_tmp866;if(Cyc_strcmp((struct _dyneither_ptr)*f,((
_tmp866="size",_tag_dyneither(_tmp866,sizeof(char),5))))== 0){const char*_tmp86A;
void*_tmp869[1];struct Cyc_String_pa_struct _tmp868;return(_tmp868.tag=0,((_tmp868.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp869[0]=& _tmp868,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp86A="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp86A,sizeof(char),44))),_tag_dyneither(_tmp869,sizeof(void*),1)))))));}
else{const char*_tmp86E;void*_tmp86D[1];struct Cyc_String_pa_struct _tmp86C;return(
_tmp86C.tag=0,((_tmp86C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp86D[0]=& _tmp86C,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp86E="expecting struct or union, found %s",_tag_dyneither(
_tmp86E,sizeof(char),36))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))));}}
_LL187:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp35F=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
_tmp35F,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp360=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp362;void*_tmp363;struct Cyc_Absyn_PtrAtts _tmp364;void*_tmp365;union Cyc_Absyn_Constraint*
_tmp366;union Cyc_Absyn_Constraint*_tmp367;_LL193: {struct Cyc_Absyn_PointerType_struct*
_tmp361=(struct Cyc_Absyn_PointerType_struct*)_tmp360;if(_tmp361->tag != 5)goto
_LL195;else{_tmp362=_tmp361->f1;_tmp363=_tmp362.elt_typ;_tmp364=_tmp362.ptr_atts;
_tmp365=_tmp364.rgn;_tmp366=_tmp364.bounds;_tmp367=_tmp364.zero_term;}}_LL194:
Cyc_Tcutil_check_nonzero_bound(loc,_tmp366);{void*_tmp368=Cyc_Tcutil_compress(
_tmp363);struct Cyc_Absyn_AggrInfo _tmp36A;union Cyc_Absyn_AggrInfoU _tmp36B;struct
Cyc_Absyn_Aggrdecl**_tmp36C;struct Cyc_Absyn_Aggrdecl*_tmp36D;struct Cyc_List_List*
_tmp36E;enum Cyc_Absyn_AggrKind _tmp370;struct Cyc_List_List*_tmp371;_LL198: {
struct Cyc_Absyn_AggrType_struct*_tmp369=(struct Cyc_Absyn_AggrType_struct*)
_tmp368;if(_tmp369->tag != 12)goto _LL19A;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp36A.aggr_info;
if((_tmp36B.KnownAggr).tag != 2)goto _LL19A;_tmp36C=(struct Cyc_Absyn_Aggrdecl**)(
_tmp36B.KnownAggr).val;_tmp36D=*_tmp36C;_tmp36E=_tmp36A.targs;}}_LL199: {struct
Cyc_Absyn_Aggrfield*_tmp372=Cyc_Absyn_lookup_decl_field(_tmp36D,f);if(_tmp372 == 
0){const char*_tmp873;void*_tmp872[2];struct Cyc_String_pa_struct _tmp871;struct Cyc_String_pa_struct
_tmp870;return(_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp36D->name)),((_tmp872[0]=& _tmp871,((
_tmp872[1]=& _tmp870,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp873="type %s has no %s field",
_tag_dyneither(_tmp873,sizeof(char),24))),_tag_dyneither(_tmp872,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36D->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp377=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp377,_tmp377,_tmp36D->tvs,_tmp36E);t3=Cyc_Tcutil_rsubstitute(_tmp377,_tmp378,
_tmp372->type);}if(((_tmp36D->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp36D->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp877;void*_tmp876[1];struct Cyc_String_pa_struct _tmp875;return(
_tmp875.tag=0,((_tmp875.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp876[0]=& _tmp875,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp877="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp877,sizeof(char),56))),_tag_dyneither(_tmp876,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36D->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp87B;void*_tmp87A[1];struct Cyc_String_pa_struct _tmp879;return(
_tmp879.tag=0,((_tmp879.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp87A[0]=& _tmp879,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87B="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp87B,sizeof(char),72))),_tag_dyneither(_tmp87A,sizeof(void*),1)))))));}}
return t3;};}_LL19A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp368;if(_tmp36F->tag != 13)goto _LL19C;else{_tmp370=_tmp36F->f1;_tmp371=_tmp36F->f2;}}
_LL19B: {struct Cyc_Absyn_Aggrfield*_tmp37F=Cyc_Absyn_lookup_field(_tmp371,f);if(
_tmp37F == 0){const char*_tmp87F;void*_tmp87E[1];struct Cyc_String_pa_struct _tmp87D;
return(_tmp87D.tag=0,((_tmp87D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp87E[0]=& _tmp87D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87F="type has no %s field",
_tag_dyneither(_tmp87F,sizeof(char),21))),_tag_dyneither(_tmp87E,sizeof(void*),1)))))));}
if((_tmp370 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp37F->type)){const char*_tmp883;void*_tmp882[1];struct Cyc_String_pa_struct
_tmp881;return(_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp882[0]=& _tmp881,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp883="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp883,sizeof(char),56))),_tag_dyneither(_tmp882,sizeof(void*),1)))))));}return
_tmp37F->type;}_LL19C:;_LL19D: goto _LL197;_LL197:;}goto _LL192;_LL195:;_LL196: goto
_LL192;_LL192:;}{const char*_tmp887;void*_tmp886[1];struct Cyc_String_pa_struct
_tmp885;return(_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp886[0]=& _tmp885,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp887="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp887,sizeof(char),44))),_tag_dyneither(_tmp886,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp38A;int
_tmp38B;struct _tuple14 _tmp389=Cyc_Evexp_eval_const_uint_exp(index);_tmp38A=
_tmp389.f1;_tmp38B=_tmp389.f2;if(!_tmp38B){const char*_tmp88A;void*_tmp889;return(
_tmp889=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp88A="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp88A,sizeof(char),47))),_tag_dyneither(_tmp889,sizeof(void*),0)));}{
struct _handler_cons _tmp38E;_push_handler(& _tmp38E);{int _tmp390=0;if(setjmp(
_tmp38E.handler))_tmp390=1;if(!_tmp390){{void*_tmp391=(*((struct _tuple18*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp38A)).f2;_npop_handler(0);
return _tmp391;};_pop_handler();}else{void*_tmp38F=(void*)_exn_thrown;void*
_tmp393=_tmp38F;_LL19F: {struct Cyc_List_Nth_struct*_tmp394=(struct Cyc_List_Nth_struct*)
_tmp393;if(_tmp394->tag != Cyc_List_Nth)goto _LL1A1;}_LL1A0: {const char*_tmp88F;
void*_tmp88E[2];struct Cyc_Int_pa_struct _tmp88D;struct Cyc_Int_pa_struct _tmp88C;
return(_tmp88C.tag=1,((_tmp88C.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp88D.tag=1,((_tmp88D.f1=(unsigned long)((int)_tmp38A),((
_tmp88E[0]=& _tmp88D,((_tmp88E[1]=& _tmp88C,Cyc_Tcexp_expr_err(te,loc,0,((_tmp88F="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp88F,sizeof(char),41))),_tag_dyneither(_tmp88E,sizeof(void*),2)))))))))))));}
_LL1A1:;_LL1A2:(void)_throw(_tmp393);_LL19E:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp399=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp39A=Cyc_Tcenv_clear_notreadctxt(_tmp399,
te_orig);Cyc_Tcexp_tcExp(_tmp39A,0,e1);Cyc_Tcexp_tcExp(_tmp39A,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp39A,e2)){const char*_tmp893;void*_tmp892[1];
struct Cyc_String_pa_struct _tmp891;return(_tmp891.tag=0,((_tmp891.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp892[0]=&
_tmp891,Cyc_Tcexp_expr_err(_tmp39A,e2->loc,topt,((_tmp893="expecting int subscript, found %s",
_tag_dyneither(_tmp893,sizeof(char),34))),_tag_dyneither(_tmp892,sizeof(void*),1)))))));}{
void*_tmp39E=t1;struct Cyc_Absyn_PtrInfo _tmp3A0;void*_tmp3A1;struct Cyc_Absyn_Tqual
_tmp3A2;struct Cyc_Absyn_PtrAtts _tmp3A3;void*_tmp3A4;union Cyc_Absyn_Constraint*
_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A6;struct Cyc_List_List*_tmp3A8;_LL1A4: {
struct Cyc_Absyn_PointerType_struct*_tmp39F=(struct Cyc_Absyn_PointerType_struct*)
_tmp39E;if(_tmp39F->tag != 5)goto _LL1A6;else{_tmp3A0=_tmp39F->f1;_tmp3A1=_tmp3A0.elt_typ;
_tmp3A2=_tmp3A0.elt_tq;_tmp3A3=_tmp3A0.ptr_atts;_tmp3A4=_tmp3A3.rgn;_tmp3A5=
_tmp3A3.bounds;_tmp3A6=_tmp3A3.zero_term;}}_LL1A5: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3A6)){int emit_warning=1;{void*_tmp3A9=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp3A5);struct Cyc_Absyn_Exp*_tmp3AB;_LL1AB: {struct Cyc_Absyn_Upper_b_struct*
_tmp3AA=(struct Cyc_Absyn_Upper_b_struct*)_tmp3A9;if(_tmp3AA->tag != 1)goto _LL1AD;
else{_tmp3AB=_tmp3AA->f1;}}_LL1AC: if(Cyc_Tcutil_is_const_exp(_tmp39A,e2)){
unsigned int _tmp3AE;int _tmp3AF;struct _tuple14 _tmp3AD=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3AE=_tmp3AD.f1;_tmp3AF=_tmp3AD.f2;if(_tmp3AF){unsigned int _tmp3B1;int
_tmp3B2;struct _tuple14 _tmp3B0=Cyc_Evexp_eval_const_uint_exp(_tmp3AB);_tmp3B1=
_tmp3B0.f1;_tmp3B2=_tmp3B0.f2;if(_tmp3B2  && _tmp3B1 > _tmp3AE)emit_warning=0;}}
goto _LL1AA;_LL1AD: {struct Cyc_Absyn_DynEither_b_struct*_tmp3AC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3A9;if(_tmp3AC->tag != 0)goto _LL1AA;}_LL1AE: emit_warning=0;goto _LL1AA;_LL1AA:;}
if(emit_warning){const char*_tmp896;void*_tmp895;(_tmp895=0,Cyc_Tcutil_warn(e2->loc,((
_tmp896="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp896,sizeof(char),63))),_tag_dyneither(_tmp895,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp39A,e2)){unsigned int _tmp3B6;int _tmp3B7;
struct _tuple14 _tmp3B5=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3B6=_tmp3B5.f1;
_tmp3B7=_tmp3B5.f2;if(_tmp3B7)Cyc_Tcutil_check_bound(loc,_tmp3B6,_tmp3A5);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3A5) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3A6)){const char*_tmp899;void*_tmp898;(_tmp898=0,
Cyc_Tcutil_warn(e1->loc,((_tmp899="subscript applied to pointer to one object",
_tag_dyneither(_tmp899,sizeof(char),43))),_tag_dyneither(_tmp898,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3A5);}}Cyc_Tcenv_check_rgn_accessible(
_tmp39A,loc,_tmp3A4);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3A1),& Cyc_Tcutil_tmk)){
const char*_tmp89C;void*_tmp89B;(_tmp89B=0,Cyc_Tcutil_terr(e1->loc,((_tmp89C="can't subscript an abstract pointer",
_tag_dyneither(_tmp89C,sizeof(char),36))),_tag_dyneither(_tmp89B,sizeof(void*),0)));}
return _tmp3A1;_LL1A6: {struct Cyc_Absyn_TupleType_struct*_tmp3A7=(struct Cyc_Absyn_TupleType_struct*)
_tmp39E;if(_tmp3A7->tag != 11)goto _LL1A8;else{_tmp3A8=_tmp3A7->f1;}}_LL1A7: return
Cyc_Tcexp_ithTupleType(_tmp39A,loc,_tmp3A8,e2);_LL1A8:;_LL1A9: {const char*
_tmp8A0;void*_tmp89F[1];struct Cyc_String_pa_struct _tmp89E;return(_tmp89E.tag=0,((
_tmp89E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp89F[0]=& _tmp89E,Cyc_Tcexp_expr_err(_tmp39A,loc,topt,((_tmp8A0="subscript applied to %s",
_tag_dyneither(_tmp8A0,sizeof(char),24))),_tag_dyneither(_tmp89F,sizeof(void*),1)))))));}
_LL1A3:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp3BF=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3C1;
_LL1B0: {struct Cyc_Absyn_TupleType_struct*_tmp3C0=(struct Cyc_Absyn_TupleType_struct*)
_tmp3BF;if(_tmp3C0->tag != 11)goto _LL1B2;else{_tmp3C1=_tmp3C0->f1;}}_LL1B1: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3C1)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AF;for(0;es != 0;(es=es->tl,_tmp3C1=_tmp3C1->tl)){
int bogus=0;void*_tmp3C2=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(
_tmp3C1))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3C2,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)
_tmp3C1->hd)).f2,& bogus);{struct _tuple18*_tmp8A3;struct Cyc_List_List*_tmp8A2;
fields=((_tmp8A2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8A2)),((
_tmp8A2->hd=((_tmp8A3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8A3)),((
_tmp8A3->f1=(*((struct _tuple18*)_tmp3C1->hd)).f1,((_tmp8A3->f2=(void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp8A3)))))),((
_tmp8A2->tl=fields,_tmp8A2))))));};}done=1;goto _LL1AF;_LL1B2:;_LL1B3: goto _LL1AF;
_LL1AF:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(
struct Cyc_Absyn_Exp*)es->hd);{struct _tuple18*_tmp8A6;struct Cyc_List_List*_tmp8A5;
fields=((_tmp8A5=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8A5)),((
_tmp8A5->hd=((_tmp8A6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8A6)),((
_tmp8A6->f1=Cyc_Absyn_empty_tqual(0),((_tmp8A6->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp8A6)))))),((_tmp8A5->tl=
fields,_tmp8A5))))));};}{struct Cyc_Absyn_TupleType_struct _tmp8A9;struct Cyc_Absyn_TupleType_struct*
_tmp8A8;return(void*)((_tmp8A8=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp8A8)),((_tmp8A8[0]=((_tmp8A9.tag=11,((_tmp8A9.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8A9)))),_tmp8A8))));};}
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){const char*_tmp8AC;void*
_tmp8AB;return(_tmp8AB=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AC="tcCompoundLit",
_tag_dyneither(_tmp8AC,sizeof(char),14))),_tag_dyneither(_tmp8AB,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des){void*res_t2;struct
_RegionHandle*_tmp3CB=Cyc_Tcenv_get_fnrgn(te);{int _tmp3CC=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(des);struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmp3CB,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,
des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_Absyn_Const_e_struct _tmp8AF;struct Cyc_Absyn_Const_e_struct*
_tmp8AE;struct Cyc_Absyn_Const_e_struct*_tmp3CD=(_tmp8AE=_cycalloc(sizeof(*
_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=0,((_tmp8AF.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,
_tmp3CC),_tmp8AF)))),_tmp8AE)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp3CD,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp3CE=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3CC - 1);if(!Cyc_Tcutil_is_zero(
_tmp3CE)){const char*_tmp8B2;void*_tmp8B1;(_tmp8B1=0,Cyc_Tcutil_terr(_tmp3CE->loc,((
_tmp8B2="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8B2,
sizeof(char),45))),_tag_dyneither(_tmp8B1,sizeof(void*),0)));}}{struct Cyc_Core_Opt*
_tmp8B3;sz_exp->topt=((_tmp8B3=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp8B3)),((_tmp8B3->v=Cyc_Absyn_uint_typ,_tmp8B3))));}res_t2=Cyc_Absyn_array_typ(
res,Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref:
Cyc_Absyn_false_conref,0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(
te,res,es)){const char*_tmp8B7;void*_tmp8B6[1];struct Cyc_String_pa_struct _tmp8B5;(
_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp8B6[0]=& _tmp8B5,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp8B7="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp8B7,sizeof(char),53))),_tag_dyneither(_tmp8B6,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple17*)des->hd)).f1;if(ds != 0){void*_tmp3D7=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3DA;_LL1B5: {struct Cyc_Absyn_FieldName_struct*_tmp3D8=(struct Cyc_Absyn_FieldName_struct*)
_tmp3D7;if(_tmp3D8->tag != 1)goto _LL1B7;}_LL1B6:{const char*_tmp8BA;void*_tmp8B9;(
_tmp8B9=0,Cyc_Tcutil_terr(loc,((_tmp8BA="only array index designators are supported",
_tag_dyneither(_tmp8BA,sizeof(char),43))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
goto _LL1B4;_LL1B7: {struct Cyc_Absyn_ArrayElement_struct*_tmp3D9=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3D7;if(_tmp3D9->tag != 0)goto _LL1B4;else{_tmp3DA=_tmp3D9->f1;}}_LL1B8: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3DA);{unsigned int _tmp3DE;int _tmp3DF;struct _tuple14 _tmp3DD=Cyc_Evexp_eval_const_uint_exp(
_tmp3DA);_tmp3DE=_tmp3DD.f1;_tmp3DF=_tmp3DD.f2;if(!_tmp3DF){const char*_tmp8BD;
void*_tmp8BC;(_tmp8BC=0,Cyc_Tcutil_terr(_tmp3DA->loc,((_tmp8BD="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp8BD,sizeof(char),47))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}
else{if(_tmp3DE != offset){const char*_tmp8C2;void*_tmp8C1[2];struct Cyc_Int_pa_struct
_tmp8C0;struct Cyc_Int_pa_struct _tmp8BF;(_tmp8BF.tag=1,((_tmp8BF.f1=(
unsigned long)((int)_tmp3DE),((_tmp8C0.tag=1,((_tmp8C0.f1=(unsigned long)offset,((
_tmp8C1[0]=& _tmp8C0,((_tmp8C1[1]=& _tmp8BF,Cyc_Tcutil_terr(_tmp3DA->loc,((_tmp8C2="expecting index designator %d but found %d",
_tag_dyneither(_tmp8C2,sizeof(char),43))),_tag_dyneither(_tmp8C1,sizeof(void*),2)))))))))))));}}
goto _LL1B4;};_LL1B4:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3E6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1BA: {struct Cyc_Absyn_TagType_struct*_tmp3E7=(
struct Cyc_Absyn_TagType_struct*)_tmp3E6;if(_tmp3E7->tag != 20)goto _LL1BC;}_LL1BB:
goto _LL1B9;_LL1BC:;_LL1BD: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp8C6;void*_tmp8C5[1];struct Cyc_String_pa_struct _tmp8C4;(_tmp8C4.tag=0,((
_tmp8C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp8C5[0]=& _tmp8C4,
Cyc_Tcutil_terr(bound->loc,((_tmp8C6="expecting unsigned int, found %s",
_tag_dyneither(_tmp8C6,sizeof(char),33))),_tag_dyneither(_tmp8C5,sizeof(void*),1)))))));}
_LL1B9:;}if(!(vd->tq).real_const){const char*_tmp8C9;void*_tmp8C8;(_tmp8C8=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8C9="comprehension index variable is not declared const!",_tag_dyneither(
_tmp8C9,sizeof(char),52))),_tag_dyneither(_tmp8C8,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3ED=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3ED,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3ED,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3ED,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3EE=te2;void**_tmp3EF=0;struct Cyc_Absyn_Tqual*
_tmp3F0=0;union Cyc_Absyn_Constraint**_tmp3F1=0;if(topt != 0){void*_tmp3F2=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3F4;struct Cyc_Absyn_ArrayInfo _tmp3F6;void*
_tmp3F7;struct Cyc_Absyn_Tqual _tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;union Cyc_Absyn_Constraint*
_tmp3FA;_LL1BF: {struct Cyc_Absyn_PointerType_struct*_tmp3F3=(struct Cyc_Absyn_PointerType_struct*)
_tmp3F2;if(_tmp3F3->tag != 5)goto _LL1C1;else{_tmp3F4=_tmp3F3->f1;}}_LL1C0:{void**
_tmp8CA;_tmp3EF=((_tmp8CA=_region_malloc(_tmp3ED,sizeof(*_tmp8CA)),((_tmp8CA[0]=
_tmp3F4.elt_typ,_tmp8CA))));}{struct Cyc_Absyn_Tqual*_tmp8CB;_tmp3F0=((_tmp8CB=
_region_malloc(_tmp3ED,sizeof(*_tmp8CB)),((_tmp8CB[0]=_tmp3F4.elt_tq,_tmp8CB))));}{
union Cyc_Absyn_Constraint**_tmp8CC;_tmp3F1=((_tmp8CC=_region_malloc(_tmp3ED,
sizeof(*_tmp8CC)),((_tmp8CC[0]=(_tmp3F4.ptr_atts).zero_term,_tmp8CC))));}goto
_LL1BE;_LL1C1: {struct Cyc_Absyn_ArrayType_struct*_tmp3F5=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3F2;if(_tmp3F5->tag != 9)goto _LL1C3;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F6.elt_type;
_tmp3F8=_tmp3F6.tq;_tmp3F9=_tmp3F6.num_elts;_tmp3FA=_tmp3F6.zero_term;}}_LL1C2:{
void**_tmp8CD;_tmp3EF=((_tmp8CD=_region_malloc(_tmp3ED,sizeof(*_tmp8CD)),((
_tmp8CD[0]=_tmp3F7,_tmp8CD))));}{struct Cyc_Absyn_Tqual*_tmp8CE;_tmp3F0=((_tmp8CE=
_region_malloc(_tmp3ED,sizeof(*_tmp8CE)),((_tmp8CE[0]=_tmp3F8,_tmp8CE))));}{
union Cyc_Absyn_Constraint**_tmp8CF;_tmp3F1=((_tmp8CF=_region_malloc(_tmp3ED,
sizeof(*_tmp8CF)),((_tmp8CF[0]=_tmp3FA,_tmp8CF))));}goto _LL1BE;_LL1C3:;_LL1C4:
goto _LL1BE;_LL1BE:;}{void*t=Cyc_Tcexp_tcExp(_tmp3EE,_tmp3EF,body);if(_tmp3EE->le
== 0){if(!Cyc_Tcutil_is_const_exp(_tmp3EE,bound)){const char*_tmp8D2;void*_tmp8D1;(
_tmp8D1=0,Cyc_Tcutil_terr(bound->loc,((_tmp8D2="bound is not constant",
_tag_dyneither(_tmp8D2,sizeof(char),22))),_tag_dyneither(_tmp8D1,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(_tmp3EE,body)){const char*_tmp8D5;void*_tmp8D4;(
_tmp8D4=0,Cyc_Tcutil_terr(bound->loc,((_tmp8D5="body is not constant",
_tag_dyneither(_tmp8D5,sizeof(char),21))),_tag_dyneither(_tmp8D4,sizeof(void*),0)));}}
if(_tmp3F1 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,*_tmp3F1)){struct Cyc_Absyn_Exp*_tmp405=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*
_tmp8D6;_tmp405->topt=((_tmp8D6=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp8D6)),((_tmp8D6->v=Cyc_Absyn_uint_typ,_tmp8D6))));}bound=Cyc_Absyn_add_exp(
bound,_tmp405,0);{struct Cyc_Core_Opt*_tmp8D7;bound->topt=((_tmp8D7=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8D7)),((_tmp8D7->v=Cyc_Absyn_uint_typ,
_tmp8D7))));}*is_zero_term=1;}{void*_tmp408=Cyc_Absyn_array_typ(t,_tmp3F0 == 0?
Cyc_Absyn_empty_tqual(0):*_tmp3F0,(struct Cyc_Absyn_Exp*)bound,_tmp3F1 == 0?Cyc_Absyn_false_conref:*
_tmp3F1,0);return _tmp408;};};};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp409;_push_handler(& _tmp409);{int _tmp40B=0;if(setjmp(
_tmp409.handler))_tmp40B=1;if(!_tmp40B){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp40A=(void*)_exn_thrown;void*_tmp40D=_tmp40A;_LL1C6: {
struct Cyc_Dict_Absent_struct*_tmp40E=(struct Cyc_Dict_Absent_struct*)_tmp40D;if(
_tmp40E->tag != Cyc_Dict_Absent)goto _LL1C8;}_LL1C7:{const char*_tmp8DB;void*
_tmp8DA[1];struct Cyc_String_pa_struct _tmp8D9;(_tmp8D9.tag=0,((_tmp8D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8DA[0]=&
_tmp8D9,Cyc_Tcutil_terr(loc,((_tmp8DB="unbound struct/union name %s",
_tag_dyneither(_tmp8DB,sizeof(char),29))),_tag_dyneither(_tmp8DA,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1C8:;_LL1C9:(void)_throw(
_tmp40D);_LL1C5:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp8E3;void*_tmp8E2[1];const char*_tmp8E1;const char*_tmp8E0;
struct Cyc_String_pa_struct _tmp8DF;(_tmp8DF.tag=0,((_tmp8DF.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8E0="struct",_tag_dyneither(
_tmp8E0,sizeof(char),7)):((_tmp8E1="union",_tag_dyneither(_tmp8E1,sizeof(char),6)))),((
_tmp8E2[0]=& _tmp8DF,Cyc_Tcutil_terr(loc,((_tmp8E3="can't construct abstract %s",
_tag_dyneither(_tmp8E3,sizeof(char),28))),_tag_dyneither(_tmp8E2,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp417=Cyc_Tcenv_get_fnrgn(te);struct _tuple11 _tmp8E4;struct
_tuple11 _tmp418=(_tmp8E4.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8E4.f2=_tmp417,
_tmp8E4)));struct Cyc_List_List*_tmp419=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp417,Cyc_Tcutil_r_make_inst_var,&
_tmp418,ad->tvs);struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp417,Cyc_Tcutil_r_make_inst_var,&
_tmp418,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp41B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp419);struct Cyc_List_List*_tmp41C=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp41A);struct Cyc_List_List*_tmp41D=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp417,_tmp419,_tmp41A);struct Cyc_Absyn_AggrType_struct _tmp8EE;struct Cyc_Absyn_Aggrdecl**
_tmp8ED;struct Cyc_Absyn_AggrInfo _tmp8EC;struct Cyc_Absyn_AggrType_struct*_tmp8EB;
struct Cyc_Absyn_AggrType_struct*_tmp41E=(_tmp8EB=_cycalloc(sizeof(*_tmp8EB)),((
_tmp8EB[0]=((_tmp8EE.tag=12,((_tmp8EE.f1=((_tmp8EC.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp8ED=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8ED)),((_tmp8ED[0]=ad,
_tmp8ED))))),((_tmp8EC.targs=_tmp41B,_tmp8EC)))),_tmp8EE)))),_tmp8EB)));struct
Cyc_List_List*_tmp41F=*ts;struct Cyc_List_List*_tmp420=_tmp41C;while(_tmp41F != 0
 && _tmp420 != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),&
Cyc_Tcutil_ak,1,(void*)_tmp41F->hd);Cyc_Tcutil_check_no_qual(loc,(void*)_tmp41F->hd);
Cyc_Tcutil_unify((void*)_tmp41F->hd,(void*)_tmp420->hd);_tmp41F=_tmp41F->tl;
_tmp420=_tmp420->tl;}if(_tmp41F != 0){const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,
Cyc_Tcutil_terr(loc,((_tmp8F1="too many explicit witness types",_tag_dyneither(
_tmp8F1,sizeof(char),32))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}*ts=
_tmp41C;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp41E,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp417,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields);for(0;fields != 0;fields=fields->tl){int bogus=0;
struct _tuple19 _tmp424;struct Cyc_Absyn_Aggrfield*_tmp425;struct Cyc_Absyn_Exp*
_tmp426;struct _tuple19*_tmp423=(struct _tuple19*)fields->hd;_tmp424=*_tmp423;
_tmp425=_tmp424.f1;_tmp426=_tmp424.f2;{void*_tmp427=Cyc_Tcutil_rsubstitute(
_tmp417,_tmp41D,_tmp425->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp427,
_tmp426);if(!Cyc_Tcutil_coerce_arg(te,_tmp426,_tmp427,& bogus)){{const char*
_tmp8FD;void*_tmp8FC[5];struct Cyc_String_pa_struct _tmp8FB;const char*_tmp8FA;
const char*_tmp8F9;struct Cyc_String_pa_struct _tmp8F8;struct Cyc_String_pa_struct
_tmp8F7;struct Cyc_String_pa_struct _tmp8F6;struct Cyc_String_pa_struct _tmp8F5;(
_tmp8F5.tag=0,((_tmp8F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp426->topt))->v)),((_tmp8F6.tag=0,((
_tmp8F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp427)),((_tmp8F7.tag=0,((_tmp8F7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8F8.tag=0,((_tmp8F8.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8F9="struct",
_tag_dyneither(_tmp8F9,sizeof(char),7)):((_tmp8FA="union",_tag_dyneither(_tmp8FA,
sizeof(char),6)))),((_tmp8FB.tag=0,((_tmp8FB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp425->name),((_tmp8FC[0]=& _tmp8FB,((_tmp8FC[1]=& _tmp8F8,((
_tmp8FC[2]=& _tmp8F7,((_tmp8FC[3]=& _tmp8F6,((_tmp8FC[4]=& _tmp8F5,Cyc_Tcutil_terr(
_tmp426->loc,((_tmp8FD="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp8FD,sizeof(char),40))),_tag_dyneither(_tmp8FC,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp431=0;{struct Cyc_List_List*
_tmp432=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp432 != 0;_tmp432=_tmp432->tl){struct _tuple0*_tmp900;struct Cyc_List_List*
_tmp8FF;_tmp431=((_tmp8FF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp8FF)),((
_tmp8FF->hd=((_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900->f1=Cyc_Tcutil_rsubstitute(
_tmp417,_tmp41D,(*((struct _tuple0*)_tmp432->hd)).f1),((_tmp900->f2=Cyc_Tcutil_rsubstitute(
_tmp417,_tmp41D,(*((struct _tuple0*)_tmp432->hd)).f2),_tmp900)))))),((_tmp8FF->tl=
_tmp431,_tmp8FF))))));}}_tmp431=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp431);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp431);
return(void*)_tmp41E;};};};}static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*args){struct
_RegionHandle*_tmp43A=Cyc_Tcenv_get_fnrgn(te);{void*_tmp43B=Cyc_Tcutil_compress(
ts);enum Cyc_Absyn_AggrKind _tmp43D;struct Cyc_List_List*_tmp43E;_LL1CB: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp43C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp43B;if(_tmp43C->tag != 13)goto _LL1CD;else{_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;}}
_LL1CC: if(_tmp43D == Cyc_Absyn_UnionA){const char*_tmp903;void*_tmp902;(_tmp902=0,
Cyc_Tcutil_terr(loc,((_tmp903="expecting struct but found union",_tag_dyneither(
_tmp903,sizeof(char),33))),_tag_dyneither(_tmp902,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp43A,loc,args,Cyc_Absyn_StructA,
_tmp43E);for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple19 _tmp442;
struct Cyc_Absyn_Aggrfield*_tmp443;struct Cyc_Absyn_Exp*_tmp444;struct _tuple19*
_tmp441=(struct _tuple19*)fields->hd;_tmp442=*_tmp441;_tmp443=_tmp442.f1;_tmp444=
_tmp442.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp443->type,_tmp444);if(!Cyc_Tcutil_coerce_arg(
te,_tmp444,_tmp443->type,& bogus)){{const char*_tmp909;void*_tmp908[3];struct Cyc_String_pa_struct
_tmp907;struct Cyc_String_pa_struct _tmp906;struct Cyc_String_pa_struct _tmp905;(
_tmp905.tag=0,((_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp444->topt))->v)),((_tmp906.tag=0,((
_tmp906.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp443->type)),((_tmp907.tag=0,((_tmp907.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp443->name),((_tmp908[0]=& _tmp907,((_tmp908[1]=& _tmp906,((
_tmp908[2]=& _tmp905,Cyc_Tcutil_terr(_tmp444->loc,((_tmp909="field %s of struct expects type %s != %s",
_tag_dyneither(_tmp909,sizeof(char),41))),_tag_dyneither(_tmp908,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CA;};_LL1CD:;_LL1CE: {const char*_tmp90C;
void*_tmp90B;(_tmp90B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp90C="tcAnonStruct: wrong type",_tag_dyneither(_tmp90C,
sizeof(char),25))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}_LL1CA:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp44C=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp90D;struct _tuple11 _tmp44D=(_tmp90D.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp90D.f2=_tmp44C,_tmp90D)));struct Cyc_List_List*_tmp44E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp44C,Cyc_Tcutil_r_make_inst_var,&
_tmp44D,tud->tvs);struct Cyc_List_List*_tmp44F=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,_tmp44E);struct Cyc_Absyn_DatatypeFieldType_struct _tmp913;struct Cyc_Absyn_DatatypeFieldInfo
_tmp912;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp911;void*res=(void*)((
_tmp911=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp911)),((_tmp911[0]=((
_tmp913.tag=4,((_tmp913.f1=((_tmp912.field_info=Cyc_Absyn_KnownDatatypefield(tud,
tuf),((_tmp912.targs=_tmp44F,_tmp912)))),_tmp913)))),_tmp911))));if(topt != 0){
void*_tmp450=Cyc_Tcutil_compress(*topt);_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp451=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp450;if(_tmp451->tag != 4)
goto _LL1D2;}_LL1D1: Cyc_Tcutil_unify(*topt,res);goto _LL1CF;_LL1D2:;_LL1D3: goto
_LL1CF;_LL1CF:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,
ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=
Cyc_Tcutil_rsubstitute(_tmp44C,_tmp44E,(*((struct _tuple18*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){{const char*_tmp91B;
void*_tmp91A[3];struct Cyc_String_pa_struct _tmp919;struct Cyc_String_pa_struct
_tmp918;const char*_tmp917;struct Cyc_String_pa_struct _tmp916;(_tmp916.tag=0,((
_tmp916.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct
_dyneither_ptr)((_tmp917="?",_tag_dyneither(_tmp917,sizeof(char),2))): Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((_tmp918.tag=0,((
_tmp918.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp919.tag=0,((_tmp919.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp91A[0]=& _tmp919,((_tmp91A[1]=& _tmp918,((_tmp91A[2]=& _tmp916,
Cyc_Tcutil_terr(e->loc,((_tmp91B="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp91B,sizeof(char),82))),_tag_dyneither(_tmp91A,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp91F;void*_tmp91E[1];
struct Cyc_String_pa_struct _tmp91D;return(_tmp91D.tag=0,((_tmp91D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp91E[0]=& _tmp91D,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp91F="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp91F,sizeof(char),47))),_tag_dyneither(_tmp91E,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp923;void*_tmp922[1];struct Cyc_String_pa_struct _tmp921;return(
_tmp921.tag=0,((_tmp921.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp922[0]=& _tmp921,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp923="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp923,sizeof(char),46))),_tag_dyneither(_tmp922,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp462=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp469;struct Cyc_Absyn_PtrAtts _tmp46A;union Cyc_Absyn_Constraint*
_tmp46B;struct Cyc_Absyn_ArrayInfo _tmp470;void*_tmp471;struct Cyc_List_List*
_tmp474;struct Cyc_Absyn_AggrInfo _tmp476;union Cyc_Absyn_AggrInfoU _tmp477;struct
Cyc_List_List*_tmp478;struct Cyc_List_List*_tmp47C;_LL1D5: {struct Cyc_Absyn_VoidType_struct*
_tmp463=(struct Cyc_Absyn_VoidType_struct*)_tmp462;if(_tmp463->tag != 0)goto _LL1D7;}
_LL1D6: return 1;_LL1D7: {struct Cyc_Absyn_Evar_struct*_tmp464=(struct Cyc_Absyn_Evar_struct*)
_tmp462;if(_tmp464->tag != 1)goto _LL1D9;}_LL1D8: goto _LL1DA;_LL1D9: {struct Cyc_Absyn_VarType_struct*
_tmp465=(struct Cyc_Absyn_VarType_struct*)_tmp462;if(_tmp465->tag != 2)goto _LL1DB;}
_LL1DA: goto _LL1DC;_LL1DB: {struct Cyc_Absyn_DatatypeType_struct*_tmp466=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp462;if(_tmp466->tag != 3)goto _LL1DD;}_LL1DC:
goto _LL1DE;_LL1DD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp467=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp462;if(_tmp467->tag != 4)goto _LL1DF;}_LL1DE: return 0;_LL1DF: {struct Cyc_Absyn_PointerType_struct*
_tmp468=(struct Cyc_Absyn_PointerType_struct*)_tmp462;if(_tmp468->tag != 5)goto
_LL1E1;else{_tmp469=_tmp468->f1;_tmp46A=_tmp469.ptr_atts;_tmp46B=_tmp46A.nullable;}}
_LL1E0: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp46B);_LL1E1: {struct Cyc_Absyn_IntType_struct*_tmp46C=(struct Cyc_Absyn_IntType_struct*)
_tmp462;if(_tmp46C->tag != 6)goto _LL1E3;}_LL1E2: goto _LL1E4;_LL1E3: {struct Cyc_Absyn_FloatType_struct*
_tmp46D=(struct Cyc_Absyn_FloatType_struct*)_tmp462;if(_tmp46D->tag != 7)goto
_LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_Absyn_DoubleType_struct*_tmp46E=(
struct Cyc_Absyn_DoubleType_struct*)_tmp462;if(_tmp46E->tag != 8)goto _LL1E7;}
_LL1E6: return 1;_LL1E7: {struct Cyc_Absyn_ArrayType_struct*_tmp46F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp462;if(_tmp46F->tag != 9)goto _LL1E9;else{_tmp470=_tmp46F->f1;_tmp471=_tmp470.elt_type;}}
_LL1E8: return Cyc_Tcexp_zeroable_type(_tmp471);_LL1E9: {struct Cyc_Absyn_FnType_struct*
_tmp472=(struct Cyc_Absyn_FnType_struct*)_tmp462;if(_tmp472->tag != 10)goto _LL1EB;}
_LL1EA: return 0;_LL1EB: {struct Cyc_Absyn_TupleType_struct*_tmp473=(struct Cyc_Absyn_TupleType_struct*)
_tmp462;if(_tmp473->tag != 11)goto _LL1ED;else{_tmp474=_tmp473->f1;}}_LL1EC: for(0;(
unsigned int)_tmp474;_tmp474=_tmp474->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple18*)_tmp474->hd)).f2))return 0;}return 1;_LL1ED: {struct Cyc_Absyn_AggrType_struct*
_tmp475=(struct Cyc_Absyn_AggrType_struct*)_tmp462;if(_tmp475->tag != 12)goto
_LL1EF;else{_tmp476=_tmp475->f1;_tmp477=_tmp476.aggr_info;_tmp478=_tmp476.targs;}}
_LL1EE: {struct Cyc_Absyn_Aggrdecl*_tmp489=Cyc_Absyn_get_known_aggrdecl(_tmp477);
if(_tmp489->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp489->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp48A=_new_region("r");struct _RegionHandle*r=&
_tmp48A;_push_region(r);{struct Cyc_List_List*_tmp48B=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp489->tvs,_tmp478);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp489->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp48B,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp48C=0;_npop_handler(0);return _tmp48C;}}}{int _tmp48D=1;_npop_handler(0);
return _tmp48D;};};_pop_region(r);};}_LL1EF: {struct Cyc_Absyn_EnumType_struct*
_tmp479=(struct Cyc_Absyn_EnumType_struct*)_tmp462;if(_tmp479->tag != 14)goto
_LL1F1;}_LL1F0: return 1;_LL1F1: {struct Cyc_Absyn_TagType_struct*_tmp47A=(struct
Cyc_Absyn_TagType_struct*)_tmp462;if(_tmp47A->tag != 20)goto _LL1F3;}_LL1F2: return
1;_LL1F3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp47B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp462;if(_tmp47B->tag != 13)goto _LL1F5;else{_tmp47C=_tmp47B->f2;}}_LL1F4: for(0;
_tmp47C != 0;_tmp47C=_tmp47C->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp47C->hd)->type))return 0;}return 1;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp47D=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp462;if(_tmp47D->tag != 15)goto
_LL1F7;}_LL1F6: return 1;_LL1F7: {struct Cyc_Absyn_TypedefType_struct*_tmp47E=(
struct Cyc_Absyn_TypedefType_struct*)_tmp462;if(_tmp47E->tag != 18)goto _LL1F9;}
_LL1F8: return 0;_LL1F9: {struct Cyc_Absyn_DynRgnType_struct*_tmp47F=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp462;if(_tmp47F->tag != 17)goto _LL1FB;}_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp480=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp462;if(_tmp480->tag != 16)goto
_LL1FD;}_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_TypeDeclType_struct*_tmp481=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp462;if(_tmp481->tag != 27)goto _LL1FF;}
_LL1FE: goto _LL200;_LL1FF: {struct Cyc_Absyn_ValueofType_struct*_tmp482=(struct Cyc_Absyn_ValueofType_struct*)
_tmp462;if(_tmp482->tag != 19)goto _LL201;}_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_HeapRgn_struct*
_tmp483=(struct Cyc_Absyn_HeapRgn_struct*)_tmp462;if(_tmp483->tag != 21)goto _LL203;}
_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_UniqueRgn_struct*_tmp484=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp462;if(_tmp484->tag != 22)goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp485=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp462;if(_tmp485->tag != 23)goto
_LL207;}_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_AccessEff_struct*_tmp486=(
struct Cyc_Absyn_AccessEff_struct*)_tmp462;if(_tmp486->tag != 24)goto _LL209;}
_LL208: goto _LL20A;_LL209: {struct Cyc_Absyn_JoinEff_struct*_tmp487=(struct Cyc_Absyn_JoinEff_struct*)
_tmp462;if(_tmp487->tag != 25)goto _LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp488=(struct Cyc_Absyn_RgnsEff_struct*)_tmp462;if(_tmp488->tag != 26)goto _LL1D4;}
_LL20C: {const char*_tmp927;void*_tmp926[1];struct Cyc_String_pa_struct _tmp925;(
_tmp925.tag=0,((_tmp925.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp926[0]=& _tmp925,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp927="bad type `%s' in zeroable type",_tag_dyneither(
_tmp927,sizeof(char),31))),_tag_dyneither(_tmp926,sizeof(void*),1)))))));}_LL1D4:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp491=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp493;void*_tmp494;_LL20E: {struct Cyc_Absyn_PointerType_struct*_tmp492=(struct
Cyc_Absyn_PointerType_struct*)_tmp491;if(_tmp492->tag != 5)goto _LL210;else{
_tmp493=_tmp492->f1;_tmp494=_tmp493.elt_typ;}}_LL20F: Cyc_Tcutil_unify(_tmp494,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20D;_LL210:;_LL211: goto _LL20D;_LL20D:;}{const char*_tmp930;void*_tmp92F[2];
const char*_tmp92E;const char*_tmp92D;struct Cyc_String_pa_struct _tmp92C;struct Cyc_String_pa_struct
_tmp92B;(_tmp92B.tag=0,((_tmp92B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp92C.tag=0,((_tmp92C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp92D="calloc",
_tag_dyneither(_tmp92D,sizeof(char),7))):(struct _dyneither_ptr)((_tmp92E="malloc",
_tag_dyneither(_tmp92E,sizeof(char),7))))),((_tmp92F[0]=& _tmp92C,((_tmp92F[1]=&
_tmp92B,Cyc_Tcutil_terr(loc,((_tmp930="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp930,sizeof(char),60))),_tag_dyneither(_tmp92F,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL212: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL213: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp933;
struct Cyc_Absyn_RgnHandleType_struct*_tmp932;void*expected_type=(void*)((_tmp932=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=
16,((_tmp933.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,
Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp933)))),_tmp932))));void*handle_type=Cyc_Tcexp_tcExp(
te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));void*_tmp49B=
Cyc_Tcutil_compress(handle_type);void*_tmp49D;_LL216: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp49C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp49B;if(_tmp49C->tag != 16)goto
_LL218;else{_tmp49D=(void*)_tmp49C->f1;}}_LL217: rgn=_tmp49D;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL215;_LL218:;_LL219:{const char*_tmp937;void*_tmp936[1];struct
Cyc_String_pa_struct _tmp935;(_tmp935.tag=0,((_tmp935.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp936[0]=& _tmp935,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp937="expecting region_t type but found %s",
_tag_dyneither(_tmp937,sizeof(char),37))),_tag_dyneither(_tmp936,sizeof(void*),1)))))));}
goto _LL215;_LL215:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp93A;void*_tmp939;(
_tmp939=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp93A="calloc with empty type",_tag_dyneither(_tmp93A,sizeof(char),23))),
_tag_dyneither(_tmp939,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcutil_check_no_qual(loc,elt_type);Cyc_Tcexp_check_malloc_type(1,
loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*_tmp4A5=(*e)->r;void*_tmp4A7;
enum Cyc_Absyn_Primop _tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_List_List
_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_List_List
_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_List_List*_tmp4B0;_LL21B: {struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmp4A6=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4A5;if(_tmp4A6->tag != 18)goto _LL21D;else{_tmp4A7=(void*)_tmp4A6->f1;}}_LL21C:
elt_type=_tmp4A7;{void**_tmp93B;*t=(void**)((_tmp93B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp93B)),((_tmp93B[0]=elt_type,_tmp93B))));}num_elts=Cyc_Absyn_uint_exp(
1,0);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto
_LL21A;_LL21D: {struct Cyc_Absyn_Primop_e_struct*_tmp4A8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp4A5;if(_tmp4A8->tag != 3)goto _LL21F;else{_tmp4A9=_tmp4A8->f1;if(_tmp4A9 != Cyc_Absyn_Times)
goto _LL21F;_tmp4AA=_tmp4A8->f2;if(_tmp4AA == 0)goto _LL21F;_tmp4AB=*_tmp4AA;
_tmp4AC=(struct Cyc_Absyn_Exp*)_tmp4AB.hd;_tmp4AD=_tmp4AB.tl;if(_tmp4AD == 0)goto
_LL21F;_tmp4AE=*_tmp4AD;_tmp4AF=(struct Cyc_Absyn_Exp*)_tmp4AE.hd;_tmp4B0=_tmp4AE.tl;
if(_tmp4B0 != 0)goto _LL21F;}}_LL21E:{struct _tuple0 _tmp93C;struct _tuple0 _tmp4B3=(
_tmp93C.f1=_tmp4AC->r,((_tmp93C.f2=_tmp4AF->r,_tmp93C)));void*_tmp4B4;void*
_tmp4B6;void*_tmp4B7;void*_tmp4B9;_LL222: _tmp4B4=_tmp4B3.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp4B5=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp4B4;if(_tmp4B5->tag != 18)goto
_LL224;else{_tmp4B6=(void*)_tmp4B5->f1;}};_LL223: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp4B6);elt_type=_tmp4B6;{void**_tmp93D;*t=(void**)((_tmp93D=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp93D)),((_tmp93D[0]=elt_type,
_tmp93D))));}num_elts=_tmp4AF;one_elt=0;goto _LL221;_LL224: _tmp4B7=_tmp4B3.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4B8=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4B7;if(_tmp4B8->tag != 18)goto _LL226;else{_tmp4B9=(void*)_tmp4B8->f1;}};
_LL225: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4B9);elt_type=_tmp4B9;{void**
_tmp93E;*t=(void**)((_tmp93E=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp93E)),((_tmp93E[0]=elt_type,_tmp93E))));}num_elts=_tmp4AC;one_elt=0;goto
_LL221;_LL226:;_LL227: goto No_sizeof;_LL221:;}goto _LL21A;_LL21F:;_LL220: No_sizeof:
elt_type=Cyc_Absyn_char_typ;{void**_tmp93F;*t=(void**)((_tmp93F=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp93F)),((_tmp93F[0]=elt_type,_tmp93F))));}
num_elts=*e;one_elt=0;goto _LL21A;_LL21A:;}*e=num_elts;*is_fat=!one_elt;{void*
_tmp4BD=elt_type;struct Cyc_Absyn_AggrInfo _tmp4BF;union Cyc_Absyn_AggrInfoU _tmp4C0;
struct Cyc_Absyn_Aggrdecl**_tmp4C1;struct Cyc_Absyn_Aggrdecl*_tmp4C2;_LL229: {
struct Cyc_Absyn_AggrType_struct*_tmp4BE=(struct Cyc_Absyn_AggrType_struct*)
_tmp4BD;if(_tmp4BE->tag != 12)goto _LL22B;else{_tmp4BF=_tmp4BE->f1;_tmp4C0=_tmp4BF.aggr_info;
if((_tmp4C0.KnownAggr).tag != 2)goto _LL22B;_tmp4C1=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4C0.KnownAggr).val;_tmp4C2=*_tmp4C1;}}_LL22A: if(_tmp4C2->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C2->impl))->exist_vars != 0){const
char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_terr(loc,((_tmp942="malloc with existential types not yet implemented",
_tag_dyneither(_tmp942,sizeof(char),50))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
goto _LL228;_LL22B:;_LL22C: goto _LL228;_LL228:;}{void*(*_tmp4C5)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4C6=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp4C7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4C9;struct Cyc_Absyn_PtrAtts
_tmp4CA;union Cyc_Absyn_Constraint*_tmp4CB;union Cyc_Absyn_Constraint*_tmp4CC;
union Cyc_Absyn_Constraint*_tmp4CD;_LL22E: {struct Cyc_Absyn_PointerType_struct*
_tmp4C8=(struct Cyc_Absyn_PointerType_struct*)_tmp4C7;if(_tmp4C8->tag != 5)goto
_LL230;else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C9.ptr_atts;_tmp4CB=_tmp4CA.nullable;
_tmp4CC=_tmp4CA.bounds;_tmp4CD=_tmp4CA.zero_term;}}_LL22F: _tmp4C6=_tmp4CD;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4CB))
_tmp4C5=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp4CD) && !(*is_calloc)){{const char*_tmp945;void*_tmp944;(_tmp944=0,Cyc_Tcutil_warn(
loc,((_tmp945="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp945,sizeof(char),55))),_tag_dyneither(_tmp944,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp4D0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4CC);struct Cyc_Absyn_Exp*_tmp4D3;_LL233: {struct Cyc_Absyn_DynEither_b_struct*
_tmp4D1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp4D0;if(_tmp4D1->tag != 0)goto
_LL235;}_LL234: goto _LL232;_LL235:{struct Cyc_Absyn_Upper_b_struct*_tmp4D2=(struct
Cyc_Absyn_Upper_b_struct*)_tmp4D0;if(_tmp4D2->tag != 1)goto _LL237;else{_tmp4D3=
_tmp4D2->f1;}}if(!(!one_elt))goto _LL237;_LL236: {int _tmp4D4=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4D4  && Cyc_Evexp_same_const_exp(_tmp4D3,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4CC),_tmp4C6);}{void*_tmp4D5=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4D7;_LL23A: {struct Cyc_Absyn_TagType_struct*_tmp4D6=(struct Cyc_Absyn_TagType_struct*)
_tmp4D5;if(_tmp4D6->tag != 20)goto _LL23C;else{_tmp4D7=(void*)_tmp4D6->f1;}}_LL23B: {
struct Cyc_Absyn_Exp*_tmp4D8=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4D7,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4D8,
_tmp4D3)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4CC),_tmp4C6);}
goto _LL239;}_LL23C:;_LL23D: goto _LL239;_LL239:;}goto _LL232;}_LL237:;_LL238: goto
_LL232;_LL232:;}goto _LL22D;_LL230:;_LL231: goto _LL22D;_LL22D:;}if(!one_elt)
_tmp4C5=Cyc_Absyn_dyneither_typ;return _tmp4C5(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp4C6);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4D9=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4DA=Cyc_Tcenv_enter_notreadctxt(
_tmp4D9,te);Cyc_Tcexp_tcExp(_tmp4DA,0,e1);Cyc_Tcexp_tcExp(_tmp4DA,(void**)((void**)((
void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp4DB=Cyc_Tcutil_compress(t1);_LL23F: {struct
Cyc_Absyn_ArrayType_struct*_tmp4DC=(struct Cyc_Absyn_ArrayType_struct*)_tmp4DB;
if(_tmp4DC->tag != 9)goto _LL241;}_LL240:{const char*_tmp948;void*_tmp947;(_tmp947=
0,Cyc_Tcutil_terr(loc,((_tmp948="cannot assign to an array",_tag_dyneither(
_tmp948,sizeof(char),26))),_tag_dyneither(_tmp947,sizeof(void*),0)));}goto _LL23E;
_LL241:;_LL242: goto _LL23E;_LL23E:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp94B;void*_tmp94A;(_tmp94A=0,Cyc_Tcutil_terr(loc,((
_tmp94B="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp94B,sizeof(char),58))),_tag_dyneither(_tmp94A,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp94E;void*_tmp94D;return(_tmp94D=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp94E="swap non-lvalue",_tag_dyneither(_tmp94E,sizeof(char),
16))),_tag_dyneither(_tmp94D,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp951;void*_tmp950;return(_tmp950=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp951="swap non-lvalue",_tag_dyneither(_tmp951,sizeof(char),16))),
_tag_dyneither(_tmp950,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp954;void*_tmp953;return(_tmp953=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp954="swap value in zeroterm array",_tag_dyneither(_tmp954,
sizeof(char),29))),_tag_dyneither(_tmp953,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp957;void*_tmp956;return(_tmp956=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp957="swap value in zeroterm array",_tag_dyneither(_tmp957,
sizeof(char),29))),_tag_dyneither(_tmp956,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp95C;void*_tmp95B[2];struct Cyc_String_pa_struct _tmp95A;struct Cyc_String_pa_struct
_tmp959;void*_tmp4E9=(_tmp959.tag=0,((_tmp959.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp95A.tag=0,((_tmp95A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp95B[0]=&
_tmp95A,((_tmp95B[1]=& _tmp959,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp95C="type mismatch: %s != %s",
_tag_dyneither(_tmp95C,sizeof(char),24))),_tag_dyneither(_tmp95B,sizeof(void*),2)))))))))))));
return _tmp4E9;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4EE=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4EE,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4EF=s->r;struct Cyc_Absyn_Exp*
_tmp4F1;struct Cyc_Absyn_Stmt*_tmp4F3;struct Cyc_Absyn_Stmt*_tmp4F4;struct Cyc_Absyn_Decl*
_tmp4F6;struct Cyc_Absyn_Stmt*_tmp4F7;_LL244: {struct Cyc_Absyn_Exp_s_struct*
_tmp4F0=(struct Cyc_Absyn_Exp_s_struct*)_tmp4EF;if(_tmp4F0->tag != 1)goto _LL246;
else{_tmp4F1=_tmp4F0->f1;}}_LL245: {void*_tmp4F8=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4F1->topt))->v;if(!Cyc_Tcutil_unify(_tmp4F8,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp960;void*_tmp95F[1];struct
Cyc_String_pa_struct _tmp95E;(_tmp95E.tag=0,((_tmp95E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F8)),((_tmp95F[0]=& _tmp95E,Cyc_Tcutil_terr(
loc,((_tmp960="statement expression returns type %s",_tag_dyneither(_tmp960,
sizeof(char),37))),_tag_dyneither(_tmp95F,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp4F8;}_LL246: {struct Cyc_Absyn_Seq_s_struct*_tmp4F2=(struct Cyc_Absyn_Seq_s_struct*)
_tmp4EF;if(_tmp4F2->tag != 2)goto _LL248;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;}}
_LL247: s=_tmp4F4;continue;_LL248: {struct Cyc_Absyn_Decl_s_struct*_tmp4F5=(struct
Cyc_Absyn_Decl_s_struct*)_tmp4EF;if(_tmp4F5->tag != 12)goto _LL24A;else{_tmp4F6=
_tmp4F5->f1;_tmp4F7=_tmp4F5->f2;}}_LL249: s=_tmp4F7;continue;_LL24A:;_LL24B: {
const char*_tmp963;void*_tmp962;return(_tmp962=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp963="statement expression must end with expression",_tag_dyneither(_tmp963,
sizeof(char),46))),_tag_dyneither(_tmp962,sizeof(void*),0)));}_LL243:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp4FE=t;struct Cyc_Absyn_AggrInfo _tmp500;union
Cyc_Absyn_AggrInfoU _tmp501;struct Cyc_Absyn_Aggrdecl**_tmp502;struct Cyc_Absyn_Aggrdecl*
_tmp503;_LL24D: {struct Cyc_Absyn_AggrType_struct*_tmp4FF=(struct Cyc_Absyn_AggrType_struct*)
_tmp4FE;if(_tmp4FF->tag != 12)goto _LL24F;else{_tmp500=_tmp4FF->f1;_tmp501=_tmp500.aggr_info;
if((_tmp501.KnownAggr).tag != 2)goto _LL24F;_tmp502=(struct Cyc_Absyn_Aggrdecl**)(
_tmp501.KnownAggr).val;_tmp503=*_tmp502;}}_LL24E: if((_tmp503->kind == Cyc_Absyn_UnionA
 && _tmp503->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp503->impl))->tagged)
goto _LL24C;goto _LL250;_LL24F:;_LL250:{const char*_tmp967;void*_tmp966[1];struct
Cyc_String_pa_struct _tmp965;(_tmp965.tag=0,((_tmp965.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp966[0]=& _tmp965,Cyc_Tcutil_terr(
loc,((_tmp967="expecting @tagged union but found %s",_tag_dyneither(_tmp967,
sizeof(char),37))),_tag_dyneither(_tmp966,sizeof(void*),1)))))));}goto _LL24C;
_LL24C:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp96A;struct Cyc_Absyn_RgnHandleType_struct*
_tmp969;void*expected_type=(void*)((_tmp969=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp969)),((_tmp969[0]=((_tmp96A.tag=16,((_tmp96A.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp96A)))),
_tmp969))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct
Cyc_Absyn_Exp*)_check_null(*rgn_handle));void*_tmp507=Cyc_Tcutil_compress(
handle_type);void*_tmp509;_LL252: {struct Cyc_Absyn_RgnHandleType_struct*_tmp508=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp507;if(_tmp508->tag != 16)goto _LL254;
else{_tmp509=(void*)_tmp508->f1;}}_LL253: rgn=_tmp509;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL251;_LL254:;_LL255:{const char*_tmp96E;void*_tmp96D[1];struct
Cyc_String_pa_struct _tmp96C;(_tmp96C.tag=0,((_tmp96C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp96D[0]=& _tmp96C,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp96E="expecting region_t type but found %s",
_tag_dyneither(_tmp96E,sizeof(char),37))),_tag_dyneither(_tmp96D,sizeof(void*),1)))))));}
goto _LL251;_LL251:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn != (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp50F=e1->r;struct Cyc_Core_Opt*_tmp512;struct Cyc_List_List*_tmp513;struct
Cyc_List_List*_tmp515;union Cyc_Absyn_Cnst _tmp517;struct _dyneither_ptr _tmp518;
_LL257: {struct Cyc_Absyn_Comprehension_e_struct*_tmp510=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp50F;if(_tmp510->tag != 28)goto _LL259;}_LL258: {void*_tmp519=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp51A=Cyc_Tcutil_compress(_tmp519);struct Cyc_Absyn_ArrayInfo
_tmp51C;void*_tmp51D;struct Cyc_Absyn_Tqual _tmp51E;struct Cyc_Absyn_Exp*_tmp51F;
union Cyc_Absyn_Constraint*_tmp520;_LL262: {struct Cyc_Absyn_ArrayType_struct*
_tmp51B=(struct Cyc_Absyn_ArrayType_struct*)_tmp51A;if(_tmp51B->tag != 9)goto
_LL264;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51C.elt_type;_tmp51E=_tmp51C.tq;
_tmp51F=_tmp51C.num_elts;_tmp520=_tmp51C.zero_term;}}_LL263: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp51F);void*b;{void*_tmp521=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp523;_LL267: {
struct Cyc_Absyn_TagType_struct*_tmp522=(struct Cyc_Absyn_TagType_struct*)_tmp521;
if(_tmp522->tag != 20)goto _LL269;else{_tmp523=(void*)_tmp522->f1;}}_LL268:{struct
Cyc_Absyn_Upper_b_struct _tmp971;struct Cyc_Absyn_Upper_b_struct*_tmp970;b=(void*)((
_tmp970=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp970)),((_tmp970[0]=((
_tmp971.tag=1,((_tmp971.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp523,0),0,Cyc_Absyn_No_coercion,0),_tmp971)))),_tmp970))));}goto _LL266;_LL269:;
_LL26A: if(Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp974;
struct Cyc_Absyn_Upper_b_struct*_tmp973;b=(void*)((_tmp973=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp973)),((_tmp973[0]=((_tmp974.tag=1,((_tmp974.f1=bnd,_tmp974)))),
_tmp973))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL266:;}{struct Cyc_Absyn_PointerType_struct
_tmp97E;struct Cyc_Absyn_PtrAtts _tmp97D;struct Cyc_Absyn_PtrInfo _tmp97C;struct Cyc_Absyn_PointerType_struct*
_tmp97B;void*res_typ=(void*)((_tmp97B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp97B)),((_tmp97B[0]=((_tmp97E.tag=5,((_tmp97E.f1=((_tmp97C.elt_typ=
_tmp51D,((_tmp97C.elt_tq=_tmp51E,((_tmp97C.ptr_atts=((_tmp97D.rgn=rgn,((_tmp97D.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp97D.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp97D.zero_term=
_tmp520,((_tmp97D.ptrloc=0,_tmp97D)))))))))),_tmp97C)))))),_tmp97E)))),_tmp97B))));
if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp97F;e->topt=((_tmp97F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp97F)),((_tmp97F->v=res_typ,
_tmp97F))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL264:;_LL265: {const char*_tmp982;void*_tmp981;(
_tmp981=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp982="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp982,
sizeof(char),45))),_tag_dyneither(_tmp981,sizeof(void*),0)));}_LL261:;}_LL259: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp511=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp50F;if(_tmp511->tag != 36)goto _LL25B;else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f2;}}
_LL25A:{struct Cyc_Absyn_Array_e_struct _tmp985;struct Cyc_Absyn_Array_e_struct*
_tmp984;e1->r=(void*)((_tmp984=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp984)),((_tmp984[0]=((_tmp985.tag=27,((_tmp985.f1=_tmp513,_tmp985)))),_tmp984))));}
_tmp515=_tmp513;goto _LL25C;_LL25B: {struct Cyc_Absyn_Array_e_struct*_tmp514=(
struct Cyc_Absyn_Array_e_struct*)_tmp50F;if(_tmp514->tag != 27)goto _LL25D;else{
_tmp515=_tmp514->f1;}}_LL25C: {void**elt_typ_opt=0;int zero_term=0;if(topt != 0){
void*_tmp531=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp533;void*
_tmp534;void**_tmp535;struct Cyc_Absyn_Tqual _tmp536;struct Cyc_Absyn_PtrAtts
_tmp537;union Cyc_Absyn_Constraint*_tmp538;_LL26C: {struct Cyc_Absyn_PointerType_struct*
_tmp532=(struct Cyc_Absyn_PointerType_struct*)_tmp531;if(_tmp532->tag != 5)goto
_LL26E;else{_tmp533=_tmp532->f1;_tmp534=_tmp533.elt_typ;_tmp535=(void**)&(
_tmp532->f1).elt_typ;_tmp536=_tmp533.elt_tq;_tmp537=_tmp533.ptr_atts;_tmp538=
_tmp537.zero_term;}}_LL26D: elt_typ_opt=(void**)_tmp535;zero_term=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp538);goto _LL26B;_LL26E:;
_LL26F: goto _LL26B;_LL26B:;}{void*_tmp539=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp515);{struct Cyc_Core_Opt*_tmp986;e1->topt=((_tmp986=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp986)),((_tmp986->v=_tmp539,
_tmp986))));}{void*res_typ;{void*_tmp53B=Cyc_Tcutil_compress(_tmp539);struct Cyc_Absyn_ArrayInfo
_tmp53D;void*_tmp53E;struct Cyc_Absyn_Tqual _tmp53F;struct Cyc_Absyn_Exp*_tmp540;
union Cyc_Absyn_Constraint*_tmp541;_LL271: {struct Cyc_Absyn_ArrayType_struct*
_tmp53C=(struct Cyc_Absyn_ArrayType_struct*)_tmp53B;if(_tmp53C->tag != 9)goto
_LL273;else{_tmp53D=_tmp53C->f1;_tmp53E=_tmp53D.elt_type;_tmp53F=_tmp53D.tq;
_tmp540=_tmp53D.num_elts;_tmp541=_tmp53D.zero_term;}}_LL272:{struct Cyc_Absyn_PointerType_struct
_tmp99B;struct Cyc_Absyn_PtrAtts _tmp99A;struct Cyc_Absyn_Upper_b_struct _tmp999;
struct Cyc_Absyn_Upper_b_struct*_tmp998;struct Cyc_Absyn_PtrInfo _tmp997;struct Cyc_Absyn_PointerType_struct*
_tmp996;res_typ=(void*)((_tmp996=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp996)),((_tmp996[0]=((_tmp99B.tag=5,((_tmp99B.f1=((_tmp997.elt_typ=_tmp53E,((
_tmp997.elt_tq=_tmp53F,((_tmp997.ptr_atts=((_tmp99A.rgn=rgn,((_tmp99A.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp99A.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp998=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=
1,((_tmp999.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp540),_tmp999)))),_tmp998))))),((
_tmp99A.zero_term=_tmp541,((_tmp99A.ptrloc=0,_tmp99A)))))))))),_tmp997)))))),
_tmp99B)))),_tmp996))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp99C;e->topt=((_tmp99C=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp99C)),((_tmp99C->v=res_typ,
_tmp99C))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}goto _LL270;_LL273:;_LL274: {const char*_tmp99F;void*_tmp99E;(
_tmp99E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp99F="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(
_tmp99F,sizeof(char),50))),_tag_dyneither(_tmp99E,sizeof(void*),0)));}_LL270:;}
return res_typ;};};}_LL25D: {struct Cyc_Absyn_Const_e_struct*_tmp516=(struct Cyc_Absyn_Const_e_struct*)
_tmp50F;if(_tmp516->tag != 0)goto _LL25F;else{_tmp517=_tmp516->f1;if((_tmp517.String_c).tag
!= 7)goto _LL25F;_tmp518=(struct _dyneither_ptr)(_tmp517.String_c).val;}}_LL25E: {
void*_tmp54B=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(
void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);void*_tmp54C=Cyc_Tcexp_tcExp(
te,(void**)& _tmp54B,e1);struct Cyc_Absyn_Upper_b_struct _tmp9A2;struct Cyc_Absyn_Upper_b_struct*
_tmp9A1;return Cyc_Absyn_atb_typ(_tmp54C,rgn,Cyc_Absyn_empty_tqual(0),(void*)((
_tmp9A1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9A1)),((_tmp9A1[0]=((
_tmp9A2.tag=1,((_tmp9A2.f1=Cyc_Absyn_uint_exp(1,0),_tmp9A2)))),_tmp9A1)))),Cyc_Absyn_false_conref);}
_LL25F:;_LL260: {void**topt2=0;if(topt != 0){void*_tmp54F=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp551;void*_tmp552;void**_tmp553;struct Cyc_Absyn_Tqual
_tmp554;_LL276: {struct Cyc_Absyn_PointerType_struct*_tmp550=(struct Cyc_Absyn_PointerType_struct*)
_tmp54F;if(_tmp550->tag != 5)goto _LL278;else{_tmp551=_tmp550->f1;_tmp552=_tmp551.elt_typ;
_tmp553=(void**)&(_tmp550->f1).elt_typ;_tmp554=_tmp551.elt_tq;}}_LL277: topt2=(
void**)_tmp553;goto _LL275;_LL278: {struct Cyc_Absyn_DatatypeType_struct*_tmp555=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp54F;if(_tmp555->tag != 3)goto _LL27A;}
_LL279:{void**_tmp9A3;topt2=((_tmp9A3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp9A3)),((_tmp9A3[0]=*topt,_tmp9A3))));}goto _LL275;_LL27A:;_LL27B: goto
_LL275;_LL275:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_struct
_tmp9AD;struct Cyc_Absyn_PtrAtts _tmp9AC;struct Cyc_Absyn_PtrInfo _tmp9AB;struct Cyc_Absyn_PointerType_struct*
_tmp9AA;void*res_typ=(void*)((_tmp9AA=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AD.tag=5,((_tmp9AD.f1=((_tmp9AB.elt_typ=
telt,((_tmp9AB.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9AB.ptr_atts=((_tmp9AC.rgn=
rgn,((_tmp9AC.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp9AC.bounds=Cyc_Absyn_bounds_one_conref,((_tmp9AC.zero_term=Cyc_Absyn_false_conref,((
_tmp9AC.ptrloc=0,_tmp9AC)))))))))),_tmp9AB)))))),_tmp9AD)))),_tmp9AA))));if(topt
!= 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,
res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9AE;e->topt=((_tmp9AE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp9AE)),((_tmp9AE->v=res_typ,_tmp9AE))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL256:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp55C=t;
struct Cyc_Absyn_ArrayInfo _tmp55E;void*_tmp55F;struct Cyc_Absyn_Tqual _tmp560;
struct Cyc_Absyn_Exp*_tmp561;union Cyc_Absyn_Constraint*_tmp562;_LL27D: {struct Cyc_Absyn_ArrayType_struct*
_tmp55D=(struct Cyc_Absyn_ArrayType_struct*)_tmp55C;if(_tmp55D->tag != 9)goto
_LL27F;else{_tmp55E=_tmp55D->f1;_tmp55F=_tmp55E.elt_type;_tmp560=_tmp55E.tq;
_tmp561=_tmp55E.num_elts;_tmp562=_tmp55E.zero_term;}}_LL27E: {void*_tmp564;
struct _tuple13 _tmp563=Cyc_Tcutil_addressof_props(te,e);_tmp564=_tmp563.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp9B1;struct Cyc_Absyn_Upper_b_struct*_tmp9B0;
void*_tmp565=_tmp561 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9B0=
_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=1,((_tmp9B1.f1=(struct Cyc_Absyn_Exp*)
_tmp561,_tmp9B1)))),_tmp9B0))));t=Cyc_Absyn_atb_typ(_tmp55F,_tmp564,_tmp560,
_tmp565,_tmp562);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL27F:;_LL280: return t;_LL27C:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp568=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp568,t) && !Cyc_Tcutil_is_noalias_path(_tmp568,e)){const char*_tmp9B4;void*
_tmp9B3;(_tmp9B3=0,Cyc_Tcutil_terr(e->loc,((_tmp9B4="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp9B4,sizeof(char),49))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}{
void*_tmp56B=e->r;union Cyc_Absyn_Cnst _tmp56F;struct _dyneither_ptr _tmp570;_LL282: {
struct Cyc_Absyn_Array_e_struct*_tmp56C=(struct Cyc_Absyn_Array_e_struct*)_tmp56B;
if(_tmp56C->tag != 27)goto _LL284;}_LL283: goto _LL285;_LL284: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp56D=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp56B;if(_tmp56D->tag != 28)
goto _LL286;}_LL285: goto _LL287;_LL286: {struct Cyc_Absyn_Const_e_struct*_tmp56E=(
struct Cyc_Absyn_Const_e_struct*)_tmp56B;if(_tmp56E->tag != 0)goto _LL288;else{
_tmp56F=_tmp56E->f1;if((_tmp56F.String_c).tag != 7)goto _LL288;_tmp570=(struct
_dyneither_ptr)(_tmp56F.String_c).val;}}_LL287: return t;_LL288:;_LL289: t=Cyc_Tcutil_compress(
t);{void*_tmp571=t;struct Cyc_Absyn_ArrayInfo _tmp573;void*_tmp574;struct Cyc_Absyn_Tqual
_tmp575;struct Cyc_Absyn_Exp*_tmp576;union Cyc_Absyn_Constraint*_tmp577;_LL28B: {
struct Cyc_Absyn_ArrayType_struct*_tmp572=(struct Cyc_Absyn_ArrayType_struct*)
_tmp571;if(_tmp572->tag != 9)goto _LL28D;else{_tmp573=_tmp572->f1;_tmp574=_tmp573.elt_type;
_tmp575=_tmp573.tq;_tmp576=_tmp573.num_elts;_tmp577=_tmp573.zero_term;}}_LL28C: {
void*_tmp579;struct _tuple13 _tmp578=Cyc_Tcutil_addressof_props(te,e);_tmp579=
_tmp578.f2;{struct Cyc_Absyn_Upper_b_struct _tmp9B7;struct Cyc_Absyn_Upper_b_struct*
_tmp9B6;void*b=_tmp576 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9B6=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=
1,((_tmp9B7.f1=(struct Cyc_Absyn_Exp*)_tmp576,_tmp9B7)))),_tmp9B6))));t=Cyc_Absyn_atb_typ(
_tmp574,_tmp579,_tmp575,b,_tmp577);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);
return t;};}_LL28D:;_LL28E: return t;_LL28A:;};_LL281:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp57C=e->r;
struct Cyc_Absyn_Exp*_tmp57E;_LL290: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp57D=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp57C;if(_tmp57D->tag != 13)
goto _LL292;else{_tmp57E=_tmp57D->f1;}}_LL291: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp57E);((struct Cyc_Core_Opt*)_check_null(_tmp57E->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp57E->topt))->v,0);e->topt=_tmp57E->topt;
goto _LL28F;_LL292:;_LL293: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp57F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp581;void*_tmp582;struct Cyc_Absyn_Tqual
_tmp583;struct Cyc_Absyn_PtrAtts _tmp584;void*_tmp585;union Cyc_Absyn_Constraint*
_tmp586;union Cyc_Absyn_Constraint*_tmp587;union Cyc_Absyn_Constraint*_tmp588;
_LL295: {struct Cyc_Absyn_PointerType_struct*_tmp580=(struct Cyc_Absyn_PointerType_struct*)
_tmp57F;if(_tmp580->tag != 5)goto _LL297;else{_tmp581=_tmp580->f1;_tmp582=_tmp581.elt_typ;
_tmp583=_tmp581.elt_tq;_tmp584=_tmp581.ptr_atts;_tmp585=_tmp584.rgn;_tmp586=
_tmp584.nullable;_tmp587=_tmp584.bounds;_tmp588=_tmp584.zero_term;}}_LL296:{void*
_tmp589=Cyc_Tcutil_compress(_tmp582);struct Cyc_Absyn_FnInfo _tmp58B;struct Cyc_List_List*
_tmp58C;struct Cyc_Core_Opt*_tmp58D;struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58F;
struct Cyc_List_List*_tmp590;int _tmp591;struct Cyc_Absyn_VarargInfo*_tmp592;struct
Cyc_List_List*_tmp593;struct Cyc_List_List*_tmp594;_LL29A: {struct Cyc_Absyn_FnType_struct*
_tmp58A=(struct Cyc_Absyn_FnType_struct*)_tmp589;if(_tmp58A->tag != 10)goto _LL29C;
else{_tmp58B=_tmp58A->f1;_tmp58C=_tmp58B.tvars;_tmp58D=_tmp58B.effect;_tmp58E=
_tmp58B.ret_tqual;_tmp58F=_tmp58B.ret_typ;_tmp590=_tmp58B.args;_tmp591=_tmp58B.c_varargs;
_tmp592=_tmp58B.cyc_varargs;_tmp593=_tmp58B.rgn_po;_tmp594=_tmp58B.attributes;}}
_LL29B: if(_tmp58C != 0){struct _RegionHandle*_tmp595=Cyc_Tcenv_get_fnrgn(te);
struct _tuple11 _tmp9B8;struct _tuple11 _tmp596=(_tmp9B8.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp9B8.f2=_tmp595,_tmp9B8)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp595,Cyc_Tcutil_r_make_inst_var,&
_tmp596,_tmp58C);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp9BE;struct Cyc_Absyn_FnInfo
_tmp9BD;struct Cyc_Absyn_FnType_struct*_tmp9BC;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp595,inst,(void*)((_tmp9BC=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp9BC)),((_tmp9BC[0]=((_tmp9BE.tag=10,((_tmp9BE.f1=((_tmp9BD.tvars=0,((_tmp9BD.effect=
_tmp58D,((_tmp9BD.ret_tqual=_tmp58E,((_tmp9BD.ret_typ=_tmp58F,((_tmp9BD.args=
_tmp590,((_tmp9BD.c_varargs=_tmp591,((_tmp9BD.cyc_varargs=_tmp592,((_tmp9BD.rgn_po=
_tmp593,((_tmp9BD.attributes=_tmp594,_tmp9BD)))))))))))))))))),_tmp9BE)))),
_tmp9BC)))));struct Cyc_Absyn_PointerType_struct _tmp9C8;struct Cyc_Absyn_PtrAtts
_tmp9C7;struct Cyc_Absyn_PtrInfo _tmp9C6;struct Cyc_Absyn_PointerType_struct*
_tmp9C5;struct Cyc_Absyn_PointerType_struct*_tmp597=(_tmp9C5=_cycalloc(sizeof(*
_tmp9C5)),((_tmp9C5[0]=((_tmp9C8.tag=5,((_tmp9C8.f1=((_tmp9C6.elt_typ=ftyp,((
_tmp9C6.elt_tq=_tmp583,((_tmp9C6.ptr_atts=((_tmp9C7.rgn=_tmp585,((_tmp9C7.nullable=
_tmp586,((_tmp9C7.bounds=_tmp587,((_tmp9C7.zero_term=_tmp588,((_tmp9C7.ptrloc=0,
_tmp9C7)))))))))),_tmp9C6)))))),_tmp9C8)))),_tmp9C5)));struct Cyc_Absyn_Exp*
_tmp598=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp9CB;
struct Cyc_Absyn_Instantiate_e_struct*_tmp9CA;e->r=(void*)((_tmp9CA=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CB.tag=
14,((_tmp9CB.f1=_tmp598,((_tmp9CB.f2=ts,_tmp9CB)))))),_tmp9CA))));}{struct Cyc_Core_Opt*
_tmp9CC;e->topt=((_tmp9CC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9CC)),((
_tmp9CC->v=(void*)_tmp597,_tmp9CC))));};}goto _LL299;_LL29C:;_LL29D: goto _LL299;
_LL299:;}goto _LL294;_LL297:;_LL298: goto _LL294;_LL294:;}goto _LL28F;_LL28F:;}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void Cyc_Tcexp_insert_alias_stmts(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct
Cyc_List_List*alias_arg_exps){struct _RegionHandle*_tmp5A4=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp5A5=0;{void*_tmp5A6=fn_exp->r;struct Cyc_List_List*
_tmp5A8;_LL29F: {struct Cyc_Absyn_FnCall_e_struct*_tmp5A7=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5A6;if(_tmp5A7->tag != 11)goto _LL2A1;else{_tmp5A8=_tmp5A7->f2;}}_LL2A0:{void*
_tmp5A9=e->r;struct Cyc_List_List*_tmp5AB;_LL2A4: {struct Cyc_Absyn_FnCall_e_struct*
_tmp5AA=(struct Cyc_Absyn_FnCall_e_struct*)_tmp5A9;if(_tmp5AA->tag != 11)goto
_LL2A6;else{_tmp5AB=_tmp5AA->f2;}}_LL2A5: {struct Cyc_List_List*_tmp5AC=
alias_arg_exps;int _tmp5AD=0;while(_tmp5AC != 0){while(_tmp5AD != (int)_tmp5AC->hd){
if(_tmp5A8 == 0){struct Cyc_Int_pa_struct _tmp9D6;struct Cyc_Int_pa_struct _tmp9D5;
void*_tmp9D4[2];const char*_tmp9D3;void*_tmp9D2;(_tmp9D2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmp9D6.tag=1,((_tmp9D6.f1=(unsigned long)((int)_tmp5AC->hd),((
_tmp9D5.tag=1,((_tmp9D5.f1=(unsigned long)_tmp5AD,((_tmp9D4[0]=& _tmp9D5,((
_tmp9D4[1]=& _tmp9D6,Cyc_aprintf(((_tmp9D3="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp9D3,sizeof(char),36))),_tag_dyneither(_tmp9D4,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp9D2,sizeof(void*),0)));}++ _tmp5AD;_tmp5A8=_tmp5A8->tl;_tmp5AB=((
struct Cyc_List_List*)_check_null(_tmp5AB))->tl;}{struct Cyc_Absyn_Decl*_tmp5B4;
struct Cyc_Absyn_Exp*_tmp5B5;struct _tuple10 _tmp5B3=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5A8))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5AB))->hd)->topt))->v));_tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;
_tmp5A8->hd=(void*)_tmp5B5;{struct Cyc_List_List*_tmp9D7;_tmp5A5=((_tmp9D7=
_region_malloc(_tmp5A4,sizeof(*_tmp9D7)),((_tmp9D7->hd=_tmp5B4,((_tmp9D7->tl=
_tmp5A5,_tmp9D7))))));}_tmp5AC=_tmp5AC->tl;};}goto _LL2A3;}_LL2A6:;_LL2A7: {const
char*_tmp9DA;void*_tmp9D9;(_tmp9D9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9DA="not a function call!",
_tag_dyneither(_tmp9DA,sizeof(char),21))),_tag_dyneither(_tmp9D9,sizeof(void*),0)));}
_LL2A3:;}goto _LL29E;_LL2A1:;_LL2A2: {const char*_tmp9DD;void*_tmp9DC;(_tmp9DC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9DD="not a function call!",_tag_dyneither(_tmp9DD,sizeof(char),21))),
_tag_dyneither(_tmp9DC,sizeof(void*),0)));}_LL29E:;}while(_tmp5A5 != 0){struct Cyc_Absyn_Decl*
_tmp5BB=(struct Cyc_Absyn_Decl*)_tmp5A5->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp5BB,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp5A5=_tmp5A5->tl;}e->topt=
0;e->r=fn_exp->r;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*
_tmp5BC=e->r;struct Cyc_Absyn_Exp*_tmp5BE;struct _tuple1*_tmp5C0;struct Cyc_Absyn_Exp*
_tmp5C2;struct Cyc_List_List*_tmp5C3;struct Cyc_Core_Opt*_tmp5C5;struct Cyc_List_List*
_tmp5C6;union Cyc_Absyn_Cnst _tmp5C8;union Cyc_Absyn_Cnst*_tmp5C9;struct _tuple1*
_tmp5CB;void*_tmp5CC;enum Cyc_Absyn_Primop _tmp5CE;struct Cyc_List_List*_tmp5CF;
struct Cyc_Absyn_Exp*_tmp5D1;enum Cyc_Absyn_Incrementor _tmp5D2;struct Cyc_Absyn_Exp*
_tmp5D4;struct Cyc_Core_Opt*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D6;struct Cyc_Absyn_Exp*
_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Exp*
_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Exp*
_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Exp*
_tmp5E5;struct Cyc_List_List*_tmp5E6;struct Cyc_Absyn_VarargCallInfo*_tmp5E7;
struct Cyc_Absyn_VarargCallInfo**_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*
_tmp5EC;struct Cyc_List_List*_tmp5ED;void*_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F0;
enum Cyc_Absyn_Coercion _tmp5F1;enum Cyc_Absyn_Coercion*_tmp5F2;struct Cyc_Absyn_Exp*
_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Exp**_tmp5F7;struct Cyc_Absyn_Exp*
_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FA;void*_tmp5FC;void*_tmp5FE;void*_tmp5FF;
struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Exp*_tmp603;struct _dyneither_ptr*
_tmp604;int _tmp605;int*_tmp606;int _tmp607;int*_tmp608;struct Cyc_Absyn_Exp*
_tmp60A;struct _dyneither_ptr*_tmp60B;int _tmp60C;int*_tmp60D;int _tmp60E;int*
_tmp60F;struct Cyc_Absyn_Exp*_tmp611;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_List_List*
_tmp614;struct _tuple8*_tmp616;struct Cyc_List_List*_tmp617;struct Cyc_List_List*
_tmp619;struct Cyc_Absyn_Stmt*_tmp61B;struct Cyc_Absyn_Vardecl*_tmp61D;struct Cyc_Absyn_Exp*
_tmp61E;struct Cyc_Absyn_Exp*_tmp61F;int _tmp620;int*_tmp621;struct _tuple1*_tmp623;
struct _tuple1**_tmp624;struct Cyc_List_List*_tmp625;struct Cyc_List_List**_tmp626;
struct Cyc_List_List*_tmp627;struct Cyc_Absyn_Aggrdecl*_tmp628;struct Cyc_Absyn_Aggrdecl**
_tmp629;void*_tmp62B;struct Cyc_List_List*_tmp62C;struct Cyc_List_List*_tmp62E;
struct Cyc_Absyn_Datatypedecl*_tmp62F;struct Cyc_Absyn_Datatypefield*_tmp630;
struct _tuple1*_tmp632;struct _tuple1**_tmp633;struct Cyc_Absyn_Enumdecl*_tmp634;
struct Cyc_Absyn_Enumfield*_tmp635;struct _tuple1*_tmp637;struct _tuple1**_tmp638;
void*_tmp639;struct Cyc_Absyn_Enumfield*_tmp63A;struct Cyc_Absyn_MallocInfo _tmp63C;
int _tmp63D;int*_tmp63E;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp**_tmp640;
void**_tmp641;void***_tmp642;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Exp**
_tmp644;int _tmp645;int*_tmp646;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*
_tmp649;struct Cyc_Absyn_Exp*_tmp64B;struct _dyneither_ptr*_tmp64C;void*_tmp64E;
_LL2A9: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp5BD=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp5BC;if(_tmp5BD->tag != 13)goto _LL2AB;else{_tmp5BE=_tmp5BD->f1;}}_LL2AA: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp5BE);return;_LL2AB: {struct Cyc_Absyn_UnknownId_e_struct*_tmp5BF=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp5BC;if(_tmp5BF->tag != 2)goto _LL2AD;else{
_tmp5C0=_tmp5BF->f1;}}_LL2AC: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp5C0);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2AD: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp5C1=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp5BC;if(_tmp5C1->tag != 10)goto _LL2AF;else{
_tmp5C2=_tmp5C1->f1;_tmp5C3=_tmp5C1->f2;}}_LL2AE: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp5C2,_tmp5C3);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2AF: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5C4=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp5BC;if(_tmp5C4->tag != 36)
goto _LL2B1;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;}}_LL2B0: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp5C6);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2B1: {struct
Cyc_Absyn_Const_e_struct*_tmp5C7=(struct Cyc_Absyn_Const_e_struct*)_tmp5BC;if(
_tmp5C7->tag != 0)goto _LL2B3;else{_tmp5C8=_tmp5C7->f1;_tmp5C9=(union Cyc_Absyn_Cnst*)&
_tmp5C7->f1;}}_LL2B2: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp5C9,e);goto _LL2A8;_LL2B3: {struct Cyc_Absyn_Var_e_struct*_tmp5CA=(struct Cyc_Absyn_Var_e_struct*)
_tmp5BC;if(_tmp5CA->tag != 1)goto _LL2B5;else{_tmp5CB=_tmp5CA->f1;_tmp5CC=(void*)
_tmp5CA->f2;}}_LL2B4: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp5CB,_tmp5CC);goto _LL2A8;
_LL2B5: {struct Cyc_Absyn_Primop_e_struct*_tmp5CD=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5BC;if(_tmp5CD->tag != 3)goto _LL2B7;else{_tmp5CE=_tmp5CD->f1;_tmp5CF=_tmp5CD->f2;}}
_LL2B6: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5CE,_tmp5CF);goto _LL2A8;_LL2B7: {
struct Cyc_Absyn_Increment_e_struct*_tmp5D0=(struct Cyc_Absyn_Increment_e_struct*)
_tmp5BC;if(_tmp5D0->tag != 5)goto _LL2B9;else{_tmp5D1=_tmp5D0->f1;_tmp5D2=_tmp5D0->f2;}}
_LL2B8: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5D1,_tmp5D2);goto _LL2A8;_LL2B9: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5D3=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5BC;if(_tmp5D3->tag != 4)goto _LL2BB;else{_tmp5D4=_tmp5D3->f1;_tmp5D5=_tmp5D3->f2;
_tmp5D6=_tmp5D3->f3;}}_LL2BA: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5D4,_tmp5D5,
_tmp5D6);goto _LL2A8;_LL2BB: {struct Cyc_Absyn_Conditional_e_struct*_tmp5D7=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp5BC;if(_tmp5D7->tag != 6)goto _LL2BD;
else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D7->f2;_tmp5DA=_tmp5D7->f3;}}_LL2BC: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5D8,_tmp5D9,_tmp5DA);goto _LL2A8;_LL2BD: {struct Cyc_Absyn_And_e_struct*
_tmp5DB=(struct Cyc_Absyn_And_e_struct*)_tmp5BC;if(_tmp5DB->tag != 7)goto _LL2BF;
else{_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;}}_LL2BE: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5DC,_tmp5DD);goto _LL2A8;_LL2BF: {struct Cyc_Absyn_Or_e_struct*_tmp5DE=(struct
Cyc_Absyn_Or_e_struct*)_tmp5BC;if(_tmp5DE->tag != 8)goto _LL2C1;else{_tmp5DF=
_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;}}_LL2C0: t=Cyc_Tcexp_tcOr(te,loc,_tmp5DF,_tmp5E0);
goto _LL2A8;_LL2C1: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5E1=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp5BC;if(_tmp5E1->tag != 9)goto _LL2C3;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;}}
_LL2C2: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5E2,_tmp5E3);goto _LL2A8;_LL2C3: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5E4=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5BC;if(_tmp5E4->tag != 11)goto _LL2C5;else{_tmp5E5=_tmp5E4->f1;_tmp5E6=_tmp5E4->f2;
_tmp5E7=_tmp5E4->f3;_tmp5E8=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5E4->f3;}}
_LL2C4: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp650;_push_handler(& _tmp650);{int _tmp652=0;if(setjmp(
_tmp650.handler))_tmp652=1;if(!_tmp652){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp651=(void*)_exn_thrown;void*_tmp654=_tmp651;struct
_dyneither_ptr _tmp656;_LL2FC: {struct Cyc_Core_Failure_struct*_tmp655=(struct Cyc_Core_Failure_struct*)
_tmp654;if(_tmp655->tag != Cyc_Core_Failure)goto _LL2FE;else{_tmp656=_tmp655->f1;}}
_LL2FD: ok=0;fn_exp=e;goto _LL2FB;_LL2FE:;_LL2FF:(void)_throw(_tmp654);_LL2FB:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5E5,_tmp5E6,_tmp5E8,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2A8;}_LL2C5: {
struct Cyc_Absyn_Throw_e_struct*_tmp5E9=(struct Cyc_Absyn_Throw_e_struct*)_tmp5BC;
if(_tmp5E9->tag != 12)goto _LL2C7;else{_tmp5EA=_tmp5E9->f1;}}_LL2C6: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp5EA);goto _LL2A8;_LL2C7: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5EB=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp5BC;if(_tmp5EB->tag != 14)goto
_LL2C9;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;}}_LL2C8: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5EC,_tmp5ED);goto _LL2A8;_LL2C9: {struct Cyc_Absyn_Cast_e_struct*
_tmp5EE=(struct Cyc_Absyn_Cast_e_struct*)_tmp5BC;if(_tmp5EE->tag != 15)goto _LL2CB;
else{_tmp5EF=(void*)_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;_tmp5F1=_tmp5EE->f4;_tmp5F2=(
enum Cyc_Absyn_Coercion*)& _tmp5EE->f4;}}_LL2CA: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5EF,_tmp5F0,(enum Cyc_Absyn_Coercion*)_tmp5F2);goto _LL2A8;_LL2CB: {struct Cyc_Absyn_Address_e_struct*
_tmp5F3=(struct Cyc_Absyn_Address_e_struct*)_tmp5BC;if(_tmp5F3->tag != 16)goto
_LL2CD;else{_tmp5F4=_tmp5F3->f1;}}_LL2CC: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp5F4);goto _LL2A8;_LL2CD: {struct Cyc_Absyn_New_e_struct*_tmp5F5=(struct Cyc_Absyn_New_e_struct*)
_tmp5BC;if(_tmp5F5->tag != 17)goto _LL2CF;else{_tmp5F6=_tmp5F5->f1;_tmp5F7=(struct
Cyc_Absyn_Exp**)& _tmp5F5->f1;_tmp5F8=_tmp5F5->f2;}}_LL2CE: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp5F7,e,_tmp5F8);goto _LL2A8;_LL2CF: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp5F9=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5BC;if(_tmp5F9->tag != 19)goto
_LL2D1;else{_tmp5FA=_tmp5F9->f1;}}_LL2D0: {void*_tmp657=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp5FA);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp657);goto _LL2A8;}_LL2D1: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5FB=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5BC;if(_tmp5FB->tag != 18)goto _LL2D3;else{_tmp5FC=(void*)_tmp5FB->f1;}}_LL2D2:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5FC);goto _LL2A8;_LL2D3: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp5FD=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp5BC;if(_tmp5FD->tag != 20)goto
_LL2D5;else{_tmp5FE=(void*)_tmp5FD->f1;_tmp5FF=(void*)_tmp5FD->f2;}}_LL2D4: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp5FE,_tmp5FF);goto _LL2A8;_LL2D5: {struct Cyc_Absyn_Deref_e_struct*
_tmp600=(struct Cyc_Absyn_Deref_e_struct*)_tmp5BC;if(_tmp600->tag != 21)goto _LL2D7;
else{_tmp601=_tmp600->f1;}}_LL2D6: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp601);goto
_LL2A8;_LL2D7: {struct Cyc_Absyn_AggrMember_e_struct*_tmp602=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp5BC;if(_tmp602->tag != 22)goto _LL2D9;else{_tmp603=_tmp602->f1;_tmp604=_tmp602->f2;
_tmp605=_tmp602->f3;_tmp606=(int*)& _tmp602->f3;_tmp607=_tmp602->f4;_tmp608=(int*)&
_tmp602->f4;}}_LL2D8: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp603,_tmp604,
_tmp606,_tmp608);goto _LL2A8;_LL2D9: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp609=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp5BC;if(_tmp609->tag != 23)goto _LL2DB;
else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;_tmp60C=_tmp609->f3;_tmp60D=(int*)&
_tmp609->f3;_tmp60E=_tmp609->f4;_tmp60F=(int*)& _tmp609->f4;}}_LL2DA: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp60A,_tmp60B,_tmp60D,_tmp60F);goto _LL2A8;_LL2DB: {struct Cyc_Absyn_Subscript_e_struct*
_tmp610=(struct Cyc_Absyn_Subscript_e_struct*)_tmp5BC;if(_tmp610->tag != 24)goto
_LL2DD;else{_tmp611=_tmp610->f1;_tmp612=_tmp610->f2;}}_LL2DC: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp611,_tmp612);goto _LL2A8;_LL2DD: {struct Cyc_Absyn_Tuple_e_struct*
_tmp613=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5BC;if(_tmp613->tag != 25)goto _LL2DF;
else{_tmp614=_tmp613->f1;}}_LL2DE: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp614);goto
_LL2A8;_LL2DF: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp615=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp5BC;if(_tmp615->tag != 26)goto _LL2E1;else{_tmp616=_tmp615->f1;_tmp617=_tmp615->f2;}}
_LL2E0: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp616,_tmp617);goto _LL2A8;_LL2E1: {
struct Cyc_Absyn_Array_e_struct*_tmp618=(struct Cyc_Absyn_Array_e_struct*)_tmp5BC;
if(_tmp618->tag != 27)goto _LL2E3;else{_tmp619=_tmp618->f1;}}_LL2E2: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp658=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp65A;void*_tmp65B;void**_tmp65C;union Cyc_Absyn_Constraint*
_tmp65D;_LL301: {struct Cyc_Absyn_ArrayType_struct*_tmp659=(struct Cyc_Absyn_ArrayType_struct*)
_tmp658;if(_tmp659->tag != 9)goto _LL303;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp65A.elt_type;
_tmp65C=(void**)&(_tmp659->f1).elt_type;_tmp65D=_tmp65A.zero_term;}}_LL302:
elt_topt=(void**)_tmp65C;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp65D);goto _LL300;_LL303:;_LL304: goto _LL300;_LL300:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp619);goto _LL2A8;}_LL2E3: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp61A=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp5BC;
if(_tmp61A->tag != 37)goto _LL2E5;else{_tmp61B=_tmp61A->f1;}}_LL2E4: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp61B);goto _LL2A8;_LL2E5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp61C=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp5BC;if(_tmp61C->tag != 28)
goto _LL2E7;else{_tmp61D=_tmp61C->f1;_tmp61E=_tmp61C->f2;_tmp61F=_tmp61C->f3;
_tmp620=_tmp61C->f4;_tmp621=(int*)& _tmp61C->f4;}}_LL2E6: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp61D,_tmp61E,_tmp61F,_tmp621);goto _LL2A8;_LL2E7: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp622=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5BC;if(_tmp622->tag != 29)goto
_LL2E9;else{_tmp623=_tmp622->f1;_tmp624=(struct _tuple1**)& _tmp622->f1;_tmp625=
_tmp622->f2;_tmp626=(struct Cyc_List_List**)& _tmp622->f2;_tmp627=_tmp622->f3;
_tmp628=_tmp622->f4;_tmp629=(struct Cyc_Absyn_Aggrdecl**)& _tmp622->f4;}}_LL2E8: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp624,_tmp626,_tmp627,_tmp629);goto _LL2A8;
_LL2E9: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp62A=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp5BC;if(_tmp62A->tag != 30)goto _LL2EB;else{_tmp62B=(void*)_tmp62A->f1;_tmp62C=
_tmp62A->f2;}}_LL2EA: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp62B,_tmp62C);goto _LL2A8;
_LL2EB: {struct Cyc_Absyn_Datatype_e_struct*_tmp62D=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp5BC;if(_tmp62D->tag != 31)goto _LL2ED;else{_tmp62E=_tmp62D->f1;_tmp62F=_tmp62D->f2;
_tmp630=_tmp62D->f3;}}_LL2EC: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp62E,
_tmp62F,_tmp630);goto _LL2A8;_LL2ED: {struct Cyc_Absyn_Enum_e_struct*_tmp631=(
struct Cyc_Absyn_Enum_e_struct*)_tmp5BC;if(_tmp631->tag != 32)goto _LL2EF;else{
_tmp632=_tmp631->f1;_tmp633=(struct _tuple1**)& _tmp631->f1;_tmp634=_tmp631->f2;
_tmp635=_tmp631->f3;}}_LL2EE:*_tmp633=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp635))->name;{struct Cyc_Absyn_EnumType_struct _tmp9E0;struct Cyc_Absyn_EnumType_struct*
_tmp9DF;t=(void*)((_tmp9DF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9DF)),((
_tmp9DF[0]=((_tmp9E0.tag=14,((_tmp9E0.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(
_tmp634))->name,((_tmp9E0.f2=_tmp634,_tmp9E0)))))),_tmp9DF))));}goto _LL2A8;
_LL2EF: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp636=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp5BC;if(_tmp636->tag != 33)goto _LL2F1;else{_tmp637=_tmp636->f1;_tmp638=(struct
_tuple1**)& _tmp636->f1;_tmp639=(void*)_tmp636->f2;_tmp63A=_tmp636->f3;}}_LL2F0:*
_tmp638=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp63A))->name;t=_tmp639;goto
_LL2A8;_LL2F1: {struct Cyc_Absyn_Malloc_e_struct*_tmp63B=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp5BC;if(_tmp63B->tag != 34)goto _LL2F3;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63C.is_calloc;
_tmp63E=(int*)&(_tmp63B->f1).is_calloc;_tmp63F=_tmp63C.rgn;_tmp640=(struct Cyc_Absyn_Exp**)&(
_tmp63B->f1).rgn;_tmp641=_tmp63C.elt_type;_tmp642=(void***)&(_tmp63B->f1).elt_type;
_tmp643=_tmp63C.num_elts;_tmp644=(struct Cyc_Absyn_Exp**)&(_tmp63B->f1).num_elts;
_tmp645=_tmp63C.fat_result;_tmp646=(int*)&(_tmp63B->f1).fat_result;}}_LL2F2: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp640,_tmp642,_tmp644,_tmp63E,_tmp646);goto
_LL2A8;_LL2F3: {struct Cyc_Absyn_Swap_e_struct*_tmp647=(struct Cyc_Absyn_Swap_e_struct*)
_tmp5BC;if(_tmp647->tag != 35)goto _LL2F5;else{_tmp648=_tmp647->f1;_tmp649=_tmp647->f2;}}
_LL2F4: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp648,_tmp649);goto _LL2A8;_LL2F5: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp64A=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp5BC;if(_tmp64A->tag != 38)goto _LL2F7;else{_tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;}}
_LL2F6: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp64B,_tmp64C);goto _LL2A8;_LL2F7: {
struct Cyc_Absyn_Valueof_e_struct*_tmp64D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp5BC;if(_tmp64D->tag != 39)goto _LL2F9;else{_tmp64E=(void*)_tmp64D->f1;}}_LL2F8:
if(!te->allow_valueof){const char*_tmp9E3;void*_tmp9E2;(_tmp9E2=0,Cyc_Tcutil_terr(
e->loc,((_tmp9E3="valueof(-) can only occur within types",_tag_dyneither(_tmp9E3,
sizeof(char),39))),_tag_dyneither(_tmp9E2,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2A8;_LL2F9: {struct Cyc_Absyn_Asm_e_struct*_tmp64F=(struct Cyc_Absyn_Asm_e_struct*)
_tmp5BC;if(_tmp64F->tag != 40)goto _LL2A8;}_LL2FA:{const char*_tmp9E6;void*_tmp9E5;(
_tmp9E5=0,Cyc_Tcutil_terr(e->loc,((_tmp9E6="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmp9E6,sizeof(char),50))),_tag_dyneither(_tmp9E5,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2A8:;}{struct Cyc_Core_Opt*
_tmp9E7;e->topt=((_tmp9E7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp9E7)),((
_tmp9E7->v=t,_tmp9E7))));};}
