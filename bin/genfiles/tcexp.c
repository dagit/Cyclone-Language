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
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
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
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
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
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct
_tuple13{int f1;void*f2;};struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);struct _tuple14{unsigned int f1;int f2;};struct _tuple14 Cyc_Evexp_eval_const_uint_exp(
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
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp65C;(
_tmp65C=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
msg,ap),_tag_dyneither(_tmp65C,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct
_tmp65F;struct Cyc_Absyn_Var_e_struct*_tmp65E;e->r=(void*)((_tmp65E=_cycalloc(
sizeof(*_tmp65E)),((_tmp65E[0]=((_tmp65F.tag=1,((_tmp65F.f1=q,((_tmp65F.f2=(void*)
_tmp7,_tmp65F)))))),_tmp65E))));}goto _LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*
_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)_tmp5;if(_tmp8->tag != 3)goto _LL5;else{
_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{struct Cyc_Absyn_Enum_e_struct _tmp662;
struct Cyc_Absyn_Enum_e_struct*_tmp661;e->r=(void*)((_tmp661=_cycalloc(sizeof(*
_tmp661)),((_tmp661[0]=((_tmp662.tag=32,((_tmp662.f1=q,((_tmp662.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp9,((_tmp662.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp662)))))))),_tmp661))));}
goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6:{struct Cyc_Absyn_AnonEnum_e_struct _tmp665;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp664;e->r=(void*)((_tmp664=_cycalloc(sizeof(*_tmp664)),((_tmp664[0]=((_tmp665.tag=
33,((_tmp665.f1=q,((_tmp665.f2=(void*)_tmpC,((_tmp665.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp665)))))))),_tmp664))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp668;struct Cyc_Absyn_Datatype_e_struct*_tmp667;e->r=(void*)((_tmp667=
_cycalloc(sizeof(*_tmp667)),((_tmp667[0]=((_tmp668.tag=31,((_tmp668.f1=0,((
_tmp668.f2=_tmpF,((_tmp668.f3=_tmp10,_tmp668)))))))),_tmp667))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp66C;void*_tmp66B[1];struct Cyc_String_pa_struct
_tmp66A;(_tmp66A.tag=0,((_tmp66A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp66B[0]=& _tmp66A,Cyc_Tcutil_terr(e->loc,((
_tmp66C="bad occurrence of type name %s",_tag_dyneither(_tmp66C,sizeof(char),31))),
_tag_dyneither(_tmp66B,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp66F;struct Cyc_Absyn_Var_e_struct*_tmp66E;e->r=(void*)((_tmp66E=_cycalloc(
sizeof(*_tmp66E)),((_tmp66E[0]=((_tmp66F.tag=1,((_tmp66F.f1=q,((_tmp66F.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp66F)))))),_tmp66E))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp672;struct Cyc_Absyn_Var_e_struct*_tmp671;e->r=(void*)((_tmp671=_cycalloc(
sizeof(*_tmp671)),((_tmp671[0]=((_tmp672.tag=1,((_tmp672.f1=q,((_tmp672.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp672)))))),_tmp671))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple17{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple17*_tmp673;return(_tmp673=_cycalloc(sizeof(*_tmp673)),((_tmp673->f1=
0,((_tmp673->f2=e,_tmp673)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp676;struct
Cyc_Absyn_FnCall_e_struct*_tmp675;e->r=(void*)((_tmp675=_cycalloc(sizeof(*
_tmp675)),((_tmp675[0]=((_tmp676.tag=11,((_tmp676.f1=e1,((_tmp676.f2=es,((
_tmp676.f3=0,_tmp676)))))))),_tmp675))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp67A;void*_tmp679[1];struct Cyc_String_pa_struct
_tmp678;(_tmp678.tag=0,((_tmp678.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp679[0]=& _tmp678,Cyc_Tcutil_terr(e->loc,((
_tmp67A="%s is a constant, not a function",_tag_dyneither(_tmp67A,sizeof(char),
33))),_tag_dyneither(_tmp679,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp67D;struct Cyc_Absyn_Datatype_e_struct*_tmp67C;e->r=(void*)((_tmp67C=
_cycalloc(sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67D.tag=31,((_tmp67D.f1=es,((
_tmp67D.f2=_tmp30,((_tmp67D.f3=_tmp31,_tmp67D)))))))),_tmp67C))));}_npop_handler(
0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*
_tmp3D=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp680;struct Cyc_Absyn_Aggregate_e_struct*_tmp67F;e->r=(void*)((_tmp67F=
_cycalloc(sizeof(*_tmp67F)),((_tmp67F[0]=((_tmp680.tag=29,((_tmp680.f1=_tmp33->name,((
_tmp680.f2=0,((_tmp680.f3=_tmp3D,((_tmp680.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp680)))))))))),_tmp67F))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp684;void*_tmp683[1];
struct Cyc_String_pa_struct _tmp682;(_tmp682.tag=0,((_tmp682.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp683[
0]=& _tmp682,Cyc_Tcutil_terr(e->loc,((_tmp684="%s is an enum constructor, not a function",
_tag_dyneither(_tmp684,sizeof(char),42))),_tag_dyneither(_tmp683,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp687;struct Cyc_Absyn_FnCall_e_struct*
_tmp686;e->r=(void*)((_tmp686=_cycalloc(sizeof(*_tmp686)),((_tmp686[0]=((_tmp687.tag=
11,((_tmp687.f1=e1,((_tmp687.f2=es,((_tmp687.f3=0,_tmp687)))))))),_tmp686))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp68A;struct Cyc_Absyn_FnCall_e_struct*_tmp689;e->r=(void*)((_tmp689=_cycalloc(
sizeof(*_tmp689)),((_tmp689[0]=((_tmp68A.tag=11,((_tmp68A.f1=e1,((_tmp68A.f2=es,((
_tmp68A.f3=0,_tmp68A)))))))),_tmp689))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp68D;
struct Cyc_Absyn_Array_e_struct*_tmp68C;e->r=(void*)((_tmp68C=_cycalloc(sizeof(*
_tmp68C)),((_tmp68C[0]=((_tmp68D.tag=27,((_tmp68D.f1=des,_tmp68D)))),_tmp68C))));}
return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp690;void*_tmp68F;(_tmp68F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp690="struct type not properly set",
_tag_dyneither(_tmp690,sizeof(char),29))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp693;struct Cyc_Absyn_Aggregate_e_struct*_tmp692;e->r=(void*)((_tmp692=
_cycalloc(sizeof(*_tmp692)),((_tmp692[0]=((_tmp693.tag=29,((_tmp693.f1=_tmp58->name,((
_tmp693.f2=0,((_tmp693.f3=des,((_tmp693.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp693)))))))))),_tmp692))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 9)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp696;struct Cyc_Absyn_Array_e_struct*_tmp695;e->r=(
void*)((_tmp695=_cycalloc(sizeof(*_tmp695)),((_tmp695[0]=((_tmp696.tag=27,((
_tmp696.f1=des,_tmp696)))),_tmp695))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp699;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp698;e->r=(void*)((_tmp698=_cycalloc(sizeof(*_tmp698)),((_tmp698[0]=((_tmp699.tag=
30,((_tmp699.f1=(void*)t,((_tmp699.f2=des,_tmp699)))))),_tmp698))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp69C;struct Cyc_Absyn_Array_e_struct*
_tmp69B;e->r=(void*)((_tmp69B=_cycalloc(sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=
27,((_tmp69C.f1=des,_tmp69C)))),_tmp69B))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6A1;void*_tmp6A0[2];struct Cyc_String_pa_struct _tmp69F;
struct Cyc_String_pa_struct _tmp69E;(_tmp69E.tag=0,((_tmp69E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp69F.tag=0,((_tmp69F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6A0[0]=& _tmp69F,((_tmp6A0[1]=& _tmp69E,Cyc_Tcutil_terr(
e->loc,((_tmp6A1="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6A1,sizeof(char),53))),_tag_dyneither(_tmp6A0,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6A2;struct
_tuple0 _tmp6C=(_tmp6A2.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp6A2.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp6A2)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp6A5;struct Cyc_Tcexp_TestEnv _tmp6A4;return(_tmp6A4.eq=((_tmp6A5=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6A5)),((_tmp6A5->f1=_tmp6F,((
_tmp6A5->f2=_tmp72,_tmp6A5)))))),((_tmp6A4.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp6A4)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp6A6;return(_tmp6A6.eq=0,((_tmp6A6.isTrue=0,_tmp6A6)));};}
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
_tmp84 == 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp6A9;struct Cyc_Absyn_Const_e_struct*
_tmp6A8;e->r=(void*)((_tmp6A8=_cycalloc(sizeof(*_tmp6A8)),((_tmp6A8[0]=((_tmp6A9.tag=
0,((_tmp6A9.f1=Cyc_Absyn_Null_c,_tmp6A9)))),_tmp6A8))));}{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp6BC;
struct Cyc_Absyn_PtrAtts _tmp6BB;struct Cyc_Core_Opt*_tmp6BA;struct Cyc_Core_Opt*
_tmp6B9;struct Cyc_Absyn_PtrInfo _tmp6B8;struct Cyc_Absyn_PointerType_struct*
_tmp6B7;t=(void*)((_tmp6B7=_cycalloc(sizeof(*_tmp6B7)),((_tmp6B7[0]=((_tmp6BC.tag=
5,((_tmp6BC.f1=((_tmp6B8.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp6B9=_cycalloc(sizeof(*_tmp6B9)),((_tmp6B9->v=_tmp99,_tmp6B9))))),((_tmp6B8.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6B8.ptr_atts=((_tmp6BB.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp6BA=_cycalloc(sizeof(*_tmp6BA)),((
_tmp6BA->v=_tmp99,_tmp6BA))))),((_tmp6BB.nullable=Cyc_Absyn_true_conref,((
_tmp6BB.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6BB.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6BB.ptrloc=0,_tmp6BB)))))))))),_tmp6B8)))))),_tmp6BC)))),_tmp6B7))));}goto
_LL50;};_LL5B: {struct Cyc_Absyn_TagType_struct*_tmp95=(struct Cyc_Absyn_TagType_struct*)
_tmp87;if(_tmp95->tag != 20)goto _LL5D;else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {
struct Cyc_Absyn_ValueofType_struct _tmp6BF;struct Cyc_Absyn_ValueofType_struct*
_tmp6BE;struct Cyc_Absyn_ValueofType_struct*_tmpA0=(_tmp6BE=_cycalloc(sizeof(*
_tmp6BE)),((_tmp6BE[0]=((_tmp6BF.tag=19,((_tmp6BF.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0),_tmp6BF)))),_tmp6BE)));if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){{const char*_tmp6C4;void*_tmp6C3[2];struct Cyc_String_pa_struct _tmp6C2;
struct Cyc_String_pa_struct _tmp6C1;(_tmp6C1.tag=0,((_tmp6C1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0)),((
_tmp6C2.tag=0,((_tmp6C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp96)),((_tmp6C3[0]=& _tmp6C2,((_tmp6C3[1]=& _tmp6C1,Cyc_Tcutil_terr(loc,((
_tmp6C4="expecting %s but found %s",_tag_dyneither(_tmp6C4,sizeof(char),26))),
_tag_dyneither(_tmp6C3,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp6C7;struct Cyc_Absyn_TagType_struct*_tmp6C6;t=(
void*)((_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6[0]=((_tmp6C7.tag=20,((
_tmp6C7.f1=(void*)((void*)_tmpA0),_tmp6C7)))),_tmp6C6))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);{struct Cyc_Core_Opt*_tmp6C8;elen->topt=((
_tmp6C8=_cycalloc(sizeof(*_tmp6C8)),((_tmp6C8->v=Cyc_Absyn_uint_typ,_tmp6C8))));}{
struct Cyc_Absyn_Upper_b_struct _tmp6CB;struct Cyc_Absyn_Upper_b_struct*_tmp6CA;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6CA=_cycalloc(sizeof(*_tmp6CA)),((_tmp6CA[0]=((_tmp6CB.tag=1,((
_tmp6CB.f1=elen,_tmp6CB)))),_tmp6CA)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual
_tmpAF;_LL64: {struct Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAC;if(_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}
_LL65: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)
elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {
struct Cyc_Absyn_PointerType_struct*_tmpB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp6CC;e->topt=((_tmp6CC=_cycalloc(sizeof(*
_tmp6CC)),((_tmp6CC->v=t,_tmp6CC))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp6CF;struct Cyc_Absyn_Upper_b_struct*
_tmp6CE;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE[0]=((_tmp6CF.tag=1,((
_tmp6CF.f1=elen,_tmp6CF)))),_tmp6CE)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp6D0;e->topt=((_tmp6D0=_cycalloc(sizeof(*_tmp6D0)),((_tmp6D0->v=t,_tmp6D0))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*_tmpB5=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp6E3;struct Cyc_Absyn_PtrAtts _tmp6E2;
struct Cyc_Core_Opt*_tmp6E1;struct Cyc_Core_Opt*_tmp6E0;struct Cyc_Absyn_PtrInfo
_tmp6DF;struct Cyc_Absyn_PointerType_struct*_tmp6DE;t=(void*)((_tmp6DE=_cycalloc(
sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6E3.tag=5,((_tmp6E3.f1=((_tmp6DF.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp6E0=_cycalloc(
sizeof(*_tmp6E0)),((_tmp6E0->v=_tmpB5,_tmp6E0))))),((_tmp6DF.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp6DF.ptr_atts=((_tmp6E2.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1->v=_tmpB5,_tmp6E1))))),((_tmp6E2.nullable=
Cyc_Absyn_true_conref,((_tmp6E2.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6E2.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6E2.ptrloc=0,_tmp6E2)))))))))),_tmp6DF)))))),_tmp6E3)))),_tmp6DE))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){
void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;
struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}_LL6C: {const char*_tmp6E7;void*_tmp6E6[1];
struct Cyc_String_pa_struct _tmp6E5;return(_tmp6E5.tag=0,((_tmp6E5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6E6[0]=&
_tmp6E5,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6E7="undeclared identifier: %s",
_tag_dyneither(_tmp6E7,sizeof(char),26))),_tag_dyneither(_tmp6E6,sizeof(void*),1)))))));}
_LL6D: {struct Cyc_Absyn_Global_b_struct*_tmpBE=(struct Cyc_Absyn_Global_b_struct*)
_tmpBC;if(_tmpBE->tag != 1)goto _LL6F;else{_tmpBF=_tmpBE->f1;}}_LL6E:*q=*_tmpBF->name;
return _tmpBF->type;_LL6F: {struct Cyc_Absyn_Funname_b_struct*_tmpC0=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBC;if(_tmpC0->tag != 2)goto _LL71;else{_tmpC1=_tmpC0->f1;}}_LL70:*q=*_tmpC1->name;
return Cyc_Tcutil_fndecl2typ(_tmpC1);_LL71: {struct Cyc_Absyn_Pat_b_struct*_tmpC2=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBC;if(_tmpC2->tag != 5)goto _LL73;else{_tmpC3=
_tmpC2->f1;}}_LL72: _tmpC5=_tmpC3;goto _LL74;_LL73: {struct Cyc_Absyn_Local_b_struct*
_tmpC4=(struct Cyc_Absyn_Local_b_struct*)_tmpBC;if(_tmpC4->tag != 4)goto _LL75;
else{_tmpC5=_tmpC4->f1;}}_LL74: _tmpC7=_tmpC5;goto _LL76;_LL75: {struct Cyc_Absyn_Param_b_struct*
_tmpC6=(struct Cyc_Absyn_Param_b_struct*)_tmpBC;if(_tmpC6->tag != 3)goto _LL6A;
else{_tmpC7=_tmpC6->f1;}}_LL76:{union Cyc_Absyn_Nmspace _tmp6E8;(*q).f1=(((_tmp6E8.Loc_n).val=
0,(((_tmp6E8.Loc_n).tag=4,_tmp6E8))));}if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79:{struct Cyc_Absyn_Valueof_e_struct _tmp6EB;struct Cyc_Absyn_Valueof_e_struct*
_tmp6EA;e->r=(void*)((_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA[0]=((_tmp6EB.tag=
39,((_tmp6EB.f1=(void*)_tmpCE,_tmp6EB)))),_tmp6EA))));}goto _LL77;_LL7A:;_LL7B:
goto _LL77;_LL77:;}return _tmpC7->type;_LL6A:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int
arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmpD1=fmt->r;
union Cyc_Absyn_Cnst _tmpD3;struct _dyneither_ptr _tmpD4;struct Cyc_Absyn_Exp*_tmpD6;
struct Cyc_Absyn_Exp _tmpD7;void*_tmpD8;union Cyc_Absyn_Cnst _tmpDA;struct
_dyneither_ptr _tmpDB;_LL7D: {struct Cyc_Absyn_Const_e_struct*_tmpD2=(struct Cyc_Absyn_Const_e_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL7F;else{_tmpD3=_tmpD2->f1;if((_tmpD3.String_c).tag
!= 7)goto _LL7F;_tmpD4=(struct _dyneither_ptr)(_tmpD3.String_c).val;}}_LL7E: _tmpDB=
_tmpD4;goto _LL80;_LL7F: {struct Cyc_Absyn_Cast_e_struct*_tmpD5=(struct Cyc_Absyn_Cast_e_struct*)
_tmpD1;if(_tmpD5->tag != 15)goto _LL81;else{_tmpD6=_tmpD5->f2;_tmpD7=*_tmpD6;
_tmpD8=_tmpD7.r;{struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)
_tmpD8;if(_tmpD9->tag != 0)goto _LL81;else{_tmpDA=_tmpD9->f1;if((_tmpDA.String_c).tag
!= 7)goto _LL81;_tmpDB=(struct _dyneither_ptr)(_tmpDA.String_c).val;}};}}_LL80:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDB,fmt->loc);goto _LL7C;
_LL81:;_LL82: if(opt_args != 0){struct Cyc_List_List*_tmpDC=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDC->hd);{struct _RegionHandle*_tmpDD=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpDD,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpDD,(struct Cyc_Absyn_Exp*)_tmpDC->hd)){const
char*_tmp6EE;void*_tmp6ED;(_tmp6ED=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDC->hd)->loc,((_tmp6EE="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp6EE,sizeof(char),49))),_tag_dyneither(_tmp6ED,sizeof(void*),0)));}};}}
return;_LL7C:;}if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE0 != 0;(((desc_types=desc_types->tl,
_tmpE0=_tmpE0->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp6F3;void*
_tmp6F2[2];struct Cyc_String_pa_struct _tmp6F1;struct Cyc_String_pa_struct _tmp6F0;(
_tmp6F0.tag=0,((_tmp6F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6F1.tag=0,((_tmp6F1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F2[
0]=& _tmp6F1,((_tmp6F2[1]=& _tmp6F0,Cyc_Tcutil_terr(e->loc,((_tmp6F3="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp6F3,sizeof(char),51))),_tag_dyneither(_tmp6F2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp6F4;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp6F4=_cycalloc(sizeof(*_tmp6F4)),((
_tmp6F4->hd=(void*)arg_cnt,((_tmp6F4->tl=*alias_arg_exps,_tmp6F4))))));}{struct
_RegionHandle*_tmpE6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE6,t) && !Cyc_Tcutil_is_noalias_path(_tmpE6,e)){const char*_tmp6F7;void*
_tmp6F6;(_tmp6F6=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6F7="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp6F7,sizeof(char),49))),_tag_dyneither(_tmp6F6,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp6FA;void*_tmp6F9;(_tmp6F9=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp6FA="too few arguments",_tag_dyneither(_tmp6FA,sizeof(char),18))),
_tag_dyneither(_tmp6F9,sizeof(void*),0)));}if(_tmpE0 != 0){const char*_tmp6FD;void*
_tmp6FC;(_tmp6FC=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6FD="too many arguments",_tag_dyneither(_tmp6FD,sizeof(char),19))),
_tag_dyneither(_tmp6FC,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus:
_LL84: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp701;void*_tmp700[1];struct Cyc_String_pa_struct
_tmp6FF;(_tmp6FF.tag=0,((_tmp6FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp700[0]=& _tmp6FF,Cyc_Tcutil_terr(loc,((_tmp701="expecting arithmetic type but found %s",
_tag_dyneither(_tmp701,sizeof(char),39))),_tag_dyneither(_tmp700,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp705;void*_tmp704[1];struct Cyc_String_pa_struct _tmp703;(_tmp703.tag=
0,((_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp704[0]=& _tmp703,Cyc_Tcutil_terr(loc,((_tmp705="expecting integral or * type but found %s",
_tag_dyneither(_tmp705,sizeof(char),42))),_tag_dyneither(_tmp704,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp709;void*_tmp708[1];struct Cyc_String_pa_struct _tmp707;(_tmp707.tag=
0,((_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp708[0]=& _tmp707,Cyc_Tcutil_terr(loc,((_tmp709="expecting integral type but found %s",
_tag_dyneither(_tmp709,sizeof(char),37))),_tag_dyneither(_tmp708,sizeof(void*),1)))))));}
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
_tmp70C;void*_tmp70B;(_tmp70B=0,Cyc_Tcutil_terr(loc,((_tmp70C="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp70C,sizeof(char),55))),_tag_dyneither(_tmp70B,sizeof(void*),0)));}
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F: {const char*_tmp710;void*_tmp70F[1];
struct Cyc_String_pa_struct _tmp70E;(_tmp70E.tag=0,((_tmp70E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp70F[0]=&
_tmp70E,Cyc_Tcutil_terr(loc,((_tmp710="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp710,sizeof(char),47))),_tag_dyneither(_tmp70F,sizeof(void*),1)))))));}
_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88: {const char*_tmp713;void*_tmp712;(
_tmp712=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp713="Non-unary primop",_tag_dyneither(_tmp713,sizeof(char),17))),
_tag_dyneither(_tmp712,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp717;void*_tmp716[
1];struct Cyc_String_pa_struct _tmp715;(_tmp715.tag=0,((_tmp715.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp716[0]=& _tmp715,Cyc_Tcutil_terr(e1->loc,((
_tmp717="type %s cannot be used here",_tag_dyneither(_tmp717,sizeof(char),28))),
_tag_dyneither(_tmp716,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp71B;void*_tmp71A[1];struct Cyc_String_pa_struct
_tmp719;(_tmp719.tag=0,((_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp71A[0]=& _tmp719,Cyc_Tcutil_terr(e2->loc,((_tmp71B="type %s cannot be used here",
_tag_dyneither(_tmp71B,sizeof(char),28))),_tag_dyneither(_tmp71A,sizeof(void*),1)))))));}
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
_tmp71E;void*_tmp71D;(_tmp71D=0,Cyc_Tcutil_terr(e1->loc,((_tmp71E="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp71E,sizeof(char),50))),_tag_dyneither(_tmp71D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp721;void*_tmp720;(_tmp720=0,
Cyc_Tcutil_terr(e1->loc,((_tmp721="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp721,sizeof(char),54))),_tag_dyneither(_tmp720,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp725;void*_tmp724[1];struct
Cyc_String_pa_struct _tmp723;(_tmp723.tag=0,((_tmp723.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp724[0]=& _tmp723,Cyc_Tcutil_terr(
e2->loc,((_tmp725="expecting int but found %s",_tag_dyneither(_tmp725,sizeof(
char),27))),_tag_dyneither(_tmp724,sizeof(void*),1)))))));}{void*_tmp11E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp115);struct Cyc_Absyn_Exp*_tmp121;_LL9C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp11F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp11E;if(_tmp11F->tag != 0)goto
_LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*_tmp120=(struct Cyc_Absyn_Upper_b_struct*)
_tmp11E;if(_tmp120->tag != 1)goto _LL9B;else{_tmp121=_tmp120->f1;}}_LL9F: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp116)){const char*
_tmp728;void*_tmp727;(_tmp727=0,Cyc_Tcutil_warn(e1->loc,((_tmp728="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp728,sizeof(char),70))),_tag_dyneither(_tmp727,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp732;struct Cyc_Absyn_PtrAtts _tmp731;struct
Cyc_Absyn_PtrInfo _tmp730;struct Cyc_Absyn_PointerType_struct*_tmp72F;struct Cyc_Absyn_PointerType_struct*
_tmp124=(_tmp72F=_cycalloc(sizeof(*_tmp72F)),((_tmp72F[0]=((_tmp732.tag=5,((
_tmp732.f1=((_tmp730.elt_typ=_tmp110,((_tmp730.elt_tq=_tmp111,((_tmp730.ptr_atts=((
_tmp731.rgn=_tmp113,((_tmp731.nullable=Cyc_Absyn_true_conref,((_tmp731.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp731.zero_term=_tmp116,((_tmp731.ptrloc=0,
_tmp731)))))))))),_tmp730)))))),_tmp732)))),_tmp72F)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp124,Cyc_Absyn_Other_coercion);return(void*)_tmp124;};_LL9B:;};
_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL96:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp737;void*_tmp736[2];struct Cyc_String_pa_struct _tmp735;struct Cyc_String_pa_struct
_tmp734;(_tmp734.tag=0,((_tmp734.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp735.tag=0,((_tmp735.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp736[0]=& _tmp735,((
_tmp736[1]=& _tmp734,Cyc_Tcutil_terr(e1->loc,((_tmp737="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp737,sizeof(char),59))),_tag_dyneither(_tmp736,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp73A;void*_tmp739;(
_tmp739=0,Cyc_Tcutil_terr(e1->loc,((_tmp73A="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp73A,sizeof(char),50))),_tag_dyneither(_tmp739,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp73D;void*_tmp73C;(_tmp73C=0,
Cyc_Tcutil_terr(e1->loc,((_tmp73D="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp73D,sizeof(char),54))),_tag_dyneither(_tmp73C,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp742;void*_tmp741[2];struct
Cyc_String_pa_struct _tmp740;struct Cyc_String_pa_struct _tmp73F;(_tmp73F.tag=0,((
_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp740.tag=0,((_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp741[0]=& _tmp740,((_tmp741[1]=& _tmp73F,Cyc_Tcutil_terr(
e2->loc,((_tmp742="expecting either %s or int but found %s",_tag_dyneither(
_tmp742,sizeof(char),40))),_tag_dyneither(_tmp741,sizeof(void*),2)))))))))))));}
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
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp743;struct _tuple0
_tmp136=(_tmp743.f1=Cyc_Tcutil_compress(t1),((_tmp743.f2=Cyc_Tcutil_compress(t2),
_tmp743)));void*_tmp137;struct Cyc_Absyn_PtrInfo _tmp139;void*_tmp13A;void*_tmp13B;
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
_LLA6: goto _LLA0;_LLA0:;}{const char*_tmp748;void*_tmp747[2];struct Cyc_String_pa_struct
_tmp746;struct Cyc_String_pa_struct _tmp745;(_tmp745.tag=0,((_tmp745.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp746.tag=
0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp747[0]=& _tmp746,((_tmp747[1]=& _tmp745,Cyc_Tcutil_terr(loc,((_tmp748="comparison not allowed between %s and %s",
_tag_dyneither(_tmp748,sizeof(char),41))),_tag_dyneither(_tmp747,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp749;
struct _tuple0 _tmp148=(_tmp749.f1=t1,((_tmp749.f2=t2,_tmp749)));void*_tmp149;void*
_tmp14B;void*_tmp14C;void*_tmp14E;_LLA8: _tmp149=_tmp148.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp149;if(_tmp14A->tag != 16)goto
_LLAA;else{_tmp14B=(void*)_tmp14A->f1;}};_tmp14C=_tmp148.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 16)goto
_LLAA;else{_tmp14E=(void*)_tmp14D->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}{const char*_tmp74E;void*_tmp74D[2];struct Cyc_String_pa_struct
_tmp74C;struct Cyc_String_pa_struct _tmp74B;(_tmp74B.tag=0,((_tmp74B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp74C.tag=
0,((_tmp74C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp74D[0]=& _tmp74C,((_tmp74D[1]=& _tmp74B,Cyc_Tcutil_terr(loc,((_tmp74E="comparison not allowed between %s and %s",
_tag_dyneither(_tmp74E,sizeof(char),41))),_tag_dyneither(_tmp74D,sizeof(void*),2)))))))))))));}
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
default: _LLBD: {const char*_tmp751;void*_tmp750;(_tmp750=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp751="bad binary primop",
_tag_dyneither(_tmp751,sizeof(char),18))),_tag_dyneither(_tmp750,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: {const char*_tmp754;void*_tmp753;return(_tmp753=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp754="primitive operator has 0 arguments",_tag_dyneither(_tmp754,
sizeof(char),35))),_tag_dyneither(_tmp753,sizeof(void*),0)));}case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: {const char*_tmp757;void*_tmp756;return(_tmp756=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp757="primitive operator has > 2 arguments",_tag_dyneither(_tmp757,
sizeof(char),37))),_tag_dyneither(_tmp756,sizeof(void*),0)));}}return t;};}struct
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
_tmp16D->tq).real_const){{const char*_tmp75B;void*_tmp75A[1];struct Cyc_String_pa_struct
_tmp759;(_tmp759.tag=0,((_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp16D->name),((_tmp75A[0]=& _tmp759,Cyc_Tcutil_terr(loc,((_tmp75B="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp75B,sizeof(char),56))),_tag_dyneither(_tmp75A,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16D->type))return 0;}return 1;
_LLC9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15A;if(_tmp162->tag != 4)goto _LLCB;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
if((_tmp164.KnownDatatypefield).tag != 2)goto _LLCB;_tmp165=(struct _tuple2)(
_tmp164.KnownDatatypefield).val;_tmp166=_tmp165.f2;}}_LLCA:{struct Cyc_List_List*
_tmp171=_tmp166->typs;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){struct _tuple18
_tmp173;struct Cyc_Absyn_Tqual _tmp174;void*_tmp175;struct _tuple18*_tmp172=(struct
_tuple18*)_tmp171->hd;_tmp173=*_tmp172;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;if(
_tmp174.real_const){{const char*_tmp75F;void*_tmp75E[1];struct Cyc_String_pa_struct
_tmp75D;(_tmp75D.tag=0,((_tmp75D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp166->name)),((_tmp75E[0]=& _tmp75D,Cyc_Tcutil_terr(
loc,((_tmp75F="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp75F,sizeof(char),64))),_tag_dyneither(_tmp75E,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp175))return 0;}}return 1;_LLCB: {
struct Cyc_Absyn_ArrayType_struct*_tmp167=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15A;if(_tmp167->tag != 9)goto _LLCD;else{_tmp168=_tmp167->f1;_tmp169=_tmp168.elt_type;
_tmp16A=_tmp168.tq;}}_LLCC: if(_tmp16A.real_const){{const char*_tmp762;void*
_tmp761;(_tmp761=0,Cyc_Tcutil_terr(loc,((_tmp762="attempt to over-write a const array",
_tag_dyneither(_tmp762,sizeof(char),36))),_tag_dyneither(_tmp761,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp169);_LLCD: {struct Cyc_Absyn_TupleType_struct*
_tmp16B=(struct Cyc_Absyn_TupleType_struct*)_tmp15A;if(_tmp16B->tag != 11)goto
_LLCF;else{_tmp16C=_tmp16B->f1;}}_LLCE: for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct _tuple18 _tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct _tuple18*
_tmp17B=(struct _tuple18*)_tmp16C->hd;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f1;_tmp17E=
_tmp17C.f2;if(_tmp17D.real_const){{const char*_tmp765;void*_tmp764;(_tmp764=0,Cyc_Tcutil_terr(
loc,((_tmp765="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp765,sizeof(char),56))),_tag_dyneither(_tmp764,sizeof(void*),0)));}
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
_tmp1AC=_tmp1AA.f2;if(!_tmp1AC){{const char*_tmp768;void*_tmp767;(_tmp767=0,Cyc_Tcutil_terr(
e->loc,((_tmp768="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp768,sizeof(char),47))),_tag_dyneither(_tmp767,sizeof(void*),0)));}return;}{
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
_LLE7: goto _LLD1;_LLD1:;}{const char*_tmp76C;void*_tmp76B[1];struct Cyc_String_pa_struct
_tmp76A;(_tmp76A.tag=0,((_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp76B[0]=& _tmp76A,Cyc_Tcutil_terr(e->loc,((
_tmp76C="attempt to write a const location: %s",_tag_dyneither(_tmp76C,sizeof(
char),38))),_tag_dyneither(_tmp76B,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp76F;void*_tmp76E;return(_tmp76E=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp76F="increment/decrement of non-lvalue",_tag_dyneither(_tmp76F,
sizeof(char),34))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp772;void*_tmp771;(_tmp771=0,Cyc_Tcutil_terr(e->loc,((
_tmp772="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp772,sizeof(char),50))),_tag_dyneither(_tmp771,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp775;void*_tmp774;(_tmp774=0,Cyc_Tcutil_terr(e->loc,((_tmp775="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp775,sizeof(char),54))),_tag_dyneither(_tmp774,sizeof(void*),0)));}}
else{const char*_tmp779;void*_tmp778[1];struct Cyc_String_pa_struct _tmp777;(
_tmp777.tag=0,((_tmp777.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp778[0]=& _tmp777,Cyc_Tcutil_terr(e->loc,((_tmp779="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp779,sizeof(char),44))),_tag_dyneither(_tmp778,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1DF;int _tmp1E0;const char*_tmp77A;struct Cyc_Tcexp_TestEnv
_tmp1DE=Cyc_Tcexp_tcTest(te,e1,((_tmp77A="conditional expression",_tag_dyneither(
_tmp77A,sizeof(char),23))));_tmp1DF=_tmp1DE.eq;_tmp1E0=_tmp1DE.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp77B;
struct Cyc_List_List _tmp1E1=(_tmp77B.hd=e3,((_tmp77B.tl=0,_tmp77B)));struct Cyc_List_List
_tmp77C;struct Cyc_List_List _tmp1E2=(_tmp77C.hd=e2,((_tmp77C.tl=(struct Cyc_List_List*)&
_tmp1E1,_tmp77C)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1E2)){{const char*_tmp781;void*_tmp780[2];struct Cyc_String_pa_struct _tmp77F;
struct Cyc_String_pa_struct _tmp77E;(_tmp77E.tag=0,((_tmp77E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp780[0]=& _tmp77F,((_tmp780[1]=& _tmp77E,Cyc_Tcutil_terr(
loc,((_tmp781="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp781,sizeof(char),48))),_tag_dyneither(_tmp780,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp782;Cyc_Tcexp_tcTest(te,e1,((_tmp782="logical-and expression",
_tag_dyneither(_tmp782,sizeof(char),23))));}{const char*_tmp783;Cyc_Tcexp_tcTest(
te,e2,((_tmp783="logical-and expression",_tag_dyneither(_tmp783,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp784;Cyc_Tcexp_tcTest(te,e1,((_tmp784="logical-or expression",
_tag_dyneither(_tmp784,sizeof(char),22))));}{const char*_tmp785;Cyc_Tcexp_tcTest(
te,e2,((_tmp785="logical-or expression",_tag_dyneither(_tmp785,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1ED=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1ED,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1EE=Cyc_Tcutil_compress(
t1);_LL111: {struct Cyc_Absyn_ArrayType_struct*_tmp1EF=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1EE;if(_tmp1EF->tag != 9)goto _LL113;}_LL112:{const char*_tmp788;void*_tmp787;(
_tmp787=0,Cyc_Tcutil_terr(loc,((_tmp788="cannot assign to an array",
_tag_dyneither(_tmp788,sizeof(char),26))),_tag_dyneither(_tmp787,sizeof(void*),0)));}
goto _LL110;_LL113:;_LL114: goto _LL110;_LL110:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),& Cyc_Tcutil_tmk)){const char*_tmp78B;void*_tmp78A;(_tmp78A=0,Cyc_Tcutil_terr(
loc,((_tmp78B="type is abstract (can't determine size).",_tag_dyneither(_tmp78B,
sizeof(char),41))),_tag_dyneither(_tmp78A,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp78E;void*_tmp78D;return(_tmp78D=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp78E="assignment to non-lvalue",_tag_dyneither(_tmp78E,sizeof(char),25))),
_tag_dyneither(_tmp78D,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp1F6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F6,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F6,e2)){const char*_tmp791;void*
_tmp790;(_tmp790=0,Cyc_Tcutil_terr(e2->loc,((_tmp791="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp791,sizeof(char),49))),_tag_dyneither(_tmp790,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp796;void*_tmp795[2];struct
Cyc_String_pa_struct _tmp794;struct Cyc_String_pa_struct _tmp793;void*_tmp1F9=(
_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp794.tag=0,((_tmp794.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp795[0]=& _tmp794,((_tmp795[1]=& _tmp793,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp796="type mismatch: %s != %s",_tag_dyneither(_tmp796,sizeof(
char),24))),_tag_dyneither(_tmp795,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1F9;}}else{enum Cyc_Absyn_Primop
_tmp1FE=(enum Cyc_Absyn_Primop)po->v;void*_tmp1FF=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp1FE,e1,e2);if(!(Cyc_Tcutil_unify(_tmp1FF,t1) || Cyc_Tcutil_coerceable(
_tmp1FF) && Cyc_Tcutil_coerceable(t1))){const char*_tmp79B;void*_tmp79A[2];struct
Cyc_String_pa_struct _tmp799;struct Cyc_String_pa_struct _tmp798;void*_tmp200=(
_tmp798.tag=0,((_tmp798.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp799.tag=0,((_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp79A[0]=& _tmp799,((_tmp79A[1]=& _tmp798,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp79B="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp79B,sizeof(char),82))),_tag_dyneither(_tmp79A,sizeof(void*),2)))))))))))));
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
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp7A0;void*_tmp79F[2];
struct Cyc_String_pa_struct _tmp79E;struct Cyc_String_pa_struct _tmp79D;(_tmp79D.tag=
0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp79E.tag=0,((_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp79F[0]=& _tmp79E,((_tmp79F[1]=& _tmp79D,Cyc_Tcutil_terr(
e->loc,((_tmp7A0="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7A0,sizeof(char),54))),_tag_dyneither(_tmp79F,sizeof(void*),2)))))))))))));}
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
_tmp22B;struct Cyc_Core_Opt*_tmp22C;void*_tmp22D;struct Cyc_List_List*_tmp22E;int
_tmp22F;struct Cyc_Absyn_VarargInfo*_tmp230;struct Cyc_List_List*_tmp231;struct Cyc_List_List*
_tmp232;_LL124: {struct Cyc_Absyn_FnType_struct*_tmp229=(struct Cyc_Absyn_FnType_struct*)
_tmp228;if(_tmp229->tag != 10)goto _LL126;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp22A.tvars;
_tmp22C=_tmp22A.effect;_tmp22D=_tmp22A.ret_typ;_tmp22E=_tmp22A.args;_tmp22F=
_tmp22A.c_varargs;_tmp230=_tmp22A.cyc_varargs;_tmp231=_tmp22A.rgn_po;_tmp232=
_tmp22A.attributes;}}_LL125: if(topt != 0)Cyc_Tcutil_unify(_tmp22D,*topt);while(
_tmp21A != 0  && _tmp22E != 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp21A->hd;void*t2=(*((struct _tuple8*)_tmp22E->hd)).f3;Cyc_Tcexp_tcExp(
_tmp21D,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp21D,e1,t2,& alias_coercion)){{
const char*_tmp7A5;void*_tmp7A4[2];struct Cyc_String_pa_struct _tmp7A3;struct Cyc_String_pa_struct
_tmp7A2;(_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7A3.tag=0,((_tmp7A3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp7A4[0]=& _tmp7A3,((_tmp7A4[1]=& _tmp7A2,Cyc_Tcutil_terr(
e1->loc,((_tmp7A5="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7A5,sizeof(char),57))),_tag_dyneither(_tmp7A4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7A6;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7A6=_cycalloc(sizeof(*_tmp7A6)),((_tmp7A6->hd=(void*)_tmp21B,((_tmp7A6->tl=*
alias_arg_exps,_tmp7A6))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp21C,
t2) && !Cyc_Tcutil_is_noalias_path(_tmp21C,e1)){const char*_tmp7A9;void*_tmp7A8;(
_tmp7A8=0,Cyc_Tcutil_terr(e1->loc,((_tmp7A9="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7A9,sizeof(char),49))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}
_tmp21A=_tmp21A->tl;_tmp22E=_tmp22E->tl;++ _tmp21B;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp232;for(0;a != 0;a=a->tl){void*_tmp23A=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp23C;int _tmp23D;int _tmp23E;_LL129: {struct Cyc_Absyn_Format_att_struct*
_tmp23B=(struct Cyc_Absyn_Format_att_struct*)_tmp23A;if(_tmp23B->tag != 19)goto
_LL12B;else{_tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;_tmp23E=_tmp23B->f3;}}_LL12A:{
struct _handler_cons _tmp23F;_push_handler(& _tmp23F);{int _tmp241=0;if(setjmp(
_tmp23F.handler))_tmp241=1;if(!_tmp241){{struct Cyc_Absyn_Exp*_tmp242=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp23D - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp23E == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp7AA;fmt_args=((
_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp23E - 1),_tmp7AA))));}
args_already_checked=1;{struct _RegionHandle*_tmp244=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp23C){case Cyc_Absyn_Printf_ft: _LL12D: Cyc_Tcexp_check_format_args(
_tmp21D,_tmp242,fmt_args,_tmp23E - 1,alias_arg_exps,_tmp244,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL12E: Cyc_Tcexp_check_format_args(_tmp21D,_tmp242,
fmt_args,_tmp23E - 1,alias_arg_exps,_tmp244,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp240=(void*)_exn_thrown;void*_tmp246=_tmp240;_LL131: {
struct Cyc_List_Nth_struct*_tmp247=(struct Cyc_List_Nth_struct*)_tmp246;if(_tmp247->tag
!= Cyc_List_Nth)goto _LL133;}_LL132:{const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,
Cyc_Tcutil_terr(loc,((_tmp7AD="bad format arguments",_tag_dyneither(_tmp7AD,
sizeof(char),21))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}goto _LL130;_LL133:;
_LL134:(void)_throw(_tmp246);_LL130:;}};}goto _LL128;_LL12B:;_LL12C: goto _LL128;
_LL128:;}}if(_tmp22E != 0){const char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,Cyc_Tcutil_terr(
loc,((_tmp7B0="too few arguments for function",_tag_dyneither(_tmp7B0,sizeof(
char),31))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}else{if((_tmp21A != 0  || 
_tmp22F) || _tmp230 != 0){if(_tmp22F)for(0;_tmp21A != 0;_tmp21A=_tmp21A->tl){Cyc_Tcexp_tcExp(
_tmp21D,0,(struct Cyc_Absyn_Exp*)_tmp21A->hd);}else{if(_tmp230 == 0){const char*
_tmp7B3;void*_tmp7B2;(_tmp7B2=0,Cyc_Tcutil_terr(loc,((_tmp7B3="too many arguments for function",
_tag_dyneither(_tmp7B3,sizeof(char),32))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}
else{void*_tmp24F;int _tmp250;struct Cyc_Absyn_VarargInfo _tmp24E=*_tmp230;_tmp24F=
_tmp24E.type;_tmp250=_tmp24E.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp7B4;
struct Cyc_Absyn_VarargCallInfo*_tmp251=(_tmp7B4=_cycalloc(sizeof(*_tmp7B4)),((
_tmp7B4->num_varargs=0,((_tmp7B4->injectors=0,((_tmp7B4->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp230,_tmp7B4)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp251;if(!_tmp250)for(0;_tmp21A != 0;(_tmp21A=_tmp21A->tl,_tmp21B ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;++
_tmp251->num_varargs;Cyc_Tcexp_tcExp(_tmp21D,(void**)& _tmp24F,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp21D,e1,_tmp24F,& alias_coercion)){{const char*_tmp7B9;void*_tmp7B8[2];struct
Cyc_String_pa_struct _tmp7B7;struct Cyc_String_pa_struct _tmp7B6;(_tmp7B6.tag=0,((
_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp7B7.tag=0,((
_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp24F)),((_tmp7B8[0]=& _tmp7B7,((_tmp7B8[1]=& _tmp7B6,Cyc_Tcutil_terr(loc,((
_tmp7B9="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp7B9,sizeof(char),49))),_tag_dyneither(_tmp7B8,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp7BA;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((
_tmp7BA->hd=(void*)_tmp21B,((_tmp7BA->tl=*alias_arg_exps,_tmp7BA))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp21C,_tmp24F) && !Cyc_Tcutil_is_noalias_path(_tmp21C,e1)){const char*_tmp7BD;
void*_tmp7BC;(_tmp7BC=0,Cyc_Tcutil_terr(e1->loc,((_tmp7BD="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7BD,sizeof(char),49))),_tag_dyneither(_tmp7BC,sizeof(void*),0)));}}
else{void*_tmp259=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp24F));
struct Cyc_Absyn_DatatypeInfo _tmp25B;union Cyc_Absyn_DatatypeInfoU _tmp25C;struct
Cyc_Absyn_Datatypedecl**_tmp25D;struct Cyc_Absyn_Datatypedecl*_tmp25E;struct Cyc_List_List*
_tmp25F;_LL136: {struct Cyc_Absyn_DatatypeType_struct*_tmp25A=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp259;if(_tmp25A->tag != 3)goto _LL138;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.datatype_info;
if((_tmp25C.KnownDatatype).tag != 2)goto _LL138;_tmp25D=(struct Cyc_Absyn_Datatypedecl**)(
_tmp25C.KnownDatatype).val;_tmp25E=*_tmp25D;_tmp25F=_tmp25B.targs;}}_LL137: {
struct Cyc_Absyn_Datatypedecl*_tmp260=*Cyc_Tcenv_lookup_datatypedecl(_tmp21D,loc,
_tmp25E->name);struct Cyc_List_List*fields=0;if(_tmp260->fields == 0){const char*
_tmp7C1;void*_tmp7C0[1];struct Cyc_String_pa_struct _tmp7BF;(_tmp7BF.tag=0,((
_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp24F)),((_tmp7C0[0]=& _tmp7BF,Cyc_Tcutil_terr(loc,((_tmp7C1="can't inject into abstract datatype %s",
_tag_dyneither(_tmp7C1,sizeof(char),39))),_tag_dyneither(_tmp7C0,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp260->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp24F),Cyc_Tcenv_curr_rgn(
_tmp21D))){const char*_tmp7C6;void*_tmp7C5[2];struct Cyc_String_pa_struct _tmp7C4;
struct Cyc_String_pa_struct _tmp7C3;(_tmp7C3.tag=0,((_tmp7C3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp21D))),((_tmp7C4.tag=0,((_tmp7C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24F)),((_tmp7C5[0]=& _tmp7C4,((_tmp7C5[
1]=& _tmp7C3,Cyc_Tcutil_terr(loc,((_tmp7C6="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp7C6,sizeof(char),49))),_tag_dyneither(_tmp7C5,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp268=Cyc_Tcenv_get_fnrgn(_tmp21D);{struct Cyc_List_List*
_tmp269=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp268,_tmp268,
_tmp260->tvs,_tmp25F);for(0;_tmp21A != 0;_tmp21A=_tmp21A->tl){++ _tmp251->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp21D,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp268,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp268,e1)){const char*_tmp7C9;void*_tmp7C8;(_tmp7C8=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7C9="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7C9,sizeof(char),49))),_tag_dyneither(_tmp7C8,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp26C=Cyc_Tcexp_tcInjection(_tmp21D,e1,Cyc_Tcutil_pointer_elt_type(
_tmp24F),_tmp268,_tmp269,fields);if(_tmp26C != 0){struct Cyc_List_List*_tmp7CA;
_tmp251->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp251->injectors,((_tmp7CA=_cycalloc(sizeof(*_tmp7CA)),((
_tmp7CA->hd=(struct Cyc_Absyn_Datatypefield*)_tmp26C,((_tmp7CA->tl=0,_tmp7CA)))))));}};};}}
goto _LL135;};}_LL138:;_LL139:{const char*_tmp7CD;void*_tmp7CC;(_tmp7CC=0,Cyc_Tcutil_terr(
loc,((_tmp7CD="bad inject vararg type",_tag_dyneither(_tmp7CD,sizeof(char),23))),
_tag_dyneither(_tmp7CC,sizeof(void*),0)));}goto _LL135;_LL135:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp21D,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp22C))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp21D,loc,_tmp231);return _tmp22D;};_LL126:;_LL127: {const char*_tmp7D0;void*
_tmp7CF;return(_tmp7CF=0,Cyc_Tcexp_expr_err(_tmp21D,loc,topt,((_tmp7D0="expected pointer to function",
_tag_dyneither(_tmp7D0,sizeof(char),29))),_tag_dyneither(_tmp7CF,sizeof(void*),0)));}
_LL123:;};_LL121:;_LL122: {const char*_tmp7D3;void*_tmp7D2;return(_tmp7D2=0,Cyc_Tcexp_expr_err(
_tmp21D,loc,topt,((_tmp7D3="expected pointer to function",_tag_dyneither(_tmp7D3,
sizeof(char),29))),_tag_dyneither(_tmp7D2,sizeof(void*),0)));}_LL11E:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp275=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp275,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp275,&
bogus)){const char*_tmp7D8;void*_tmp7D7[2];struct Cyc_String_pa_struct _tmp7D6;
struct Cyc_String_pa_struct _tmp7D5;(_tmp7D5.tag=0,((_tmp7D5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7D6.tag=0,((_tmp7D6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp275)),((_tmp7D7[0]=& _tmp7D6,((
_tmp7D7[1]=& _tmp7D5,Cyc_Tcutil_terr(loc,((_tmp7D8="expected %s but found %s",
_tag_dyneither(_tmp7D8,sizeof(char),25))),_tag_dyneither(_tmp7D7,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp27A=t1;struct Cyc_Absyn_PtrInfo _tmp27C;void*
_tmp27D;struct Cyc_Absyn_Tqual _tmp27E;struct Cyc_Absyn_PtrAtts _tmp27F;void*_tmp280;
union Cyc_Absyn_Constraint*_tmp281;union Cyc_Absyn_Constraint*_tmp282;union Cyc_Absyn_Constraint*
_tmp283;_LL13B: {struct Cyc_Absyn_PointerType_struct*_tmp27B=(struct Cyc_Absyn_PointerType_struct*)
_tmp27A;if(_tmp27B->tag != 5)goto _LL13D;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27C.elt_typ;
_tmp27E=_tmp27C.elt_tq;_tmp27F=_tmp27C.ptr_atts;_tmp280=_tmp27F.rgn;_tmp281=
_tmp27F.nullable;_tmp282=_tmp27F.bounds;_tmp283=_tmp27F.zero_term;}}_LL13C:{void*
_tmp284=Cyc_Tcutil_compress(_tmp27D);struct Cyc_Absyn_FnInfo _tmp286;struct Cyc_List_List*
_tmp287;struct Cyc_Core_Opt*_tmp288;void*_tmp289;struct Cyc_List_List*_tmp28A;int
_tmp28B;struct Cyc_Absyn_VarargInfo*_tmp28C;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*
_tmp28E;_LL140: {struct Cyc_Absyn_FnType_struct*_tmp285=(struct Cyc_Absyn_FnType_struct*)
_tmp284;if(_tmp285->tag != 10)goto _LL142;else{_tmp286=_tmp285->f1;_tmp287=_tmp286.tvars;
_tmp288=_tmp286.effect;_tmp289=_tmp286.ret_typ;_tmp28A=_tmp286.args;_tmp28B=
_tmp286.c_varargs;_tmp28C=_tmp286.cyc_varargs;_tmp28D=_tmp286.rgn_po;_tmp28E=
_tmp286.attributes;}}_LL141: {struct _RegionHandle*_tmp28F=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp287 != 0;(ts=ts->tl,
_tmp287=_tmp287->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp287->hd,& Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);{struct _tuple12*_tmp7DB;struct Cyc_List_List*_tmp7DA;
instantiation=((_tmp7DA=_region_malloc(_tmp28F,sizeof(*_tmp7DA)),((_tmp7DA->hd=((
_tmp7DB=_region_malloc(_tmp28F,sizeof(*_tmp7DB)),((_tmp7DB->f1=(struct Cyc_Absyn_Tvar*)
_tmp287->hd,((_tmp7DB->f2=(void*)ts->hd,_tmp7DB)))))),((_tmp7DA->tl=
instantiation,_tmp7DA))))));};}if(ts != 0){const char*_tmp7DE;void*_tmp7DD;return(
_tmp7DD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DE="too many type variables in instantiation",
_tag_dyneither(_tmp7DE,sizeof(char),41))),_tag_dyneither(_tmp7DD,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp7E4;struct Cyc_Absyn_FnInfo _tmp7E3;struct Cyc_Absyn_FnType_struct*
_tmp7E2;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp28F,instantiation,(void*)((
_tmp7E2=_cycalloc(sizeof(*_tmp7E2)),((_tmp7E2[0]=((_tmp7E4.tag=10,((_tmp7E4.f1=((
_tmp7E3.tvars=_tmp287,((_tmp7E3.effect=_tmp288,((_tmp7E3.ret_typ=_tmp289,((
_tmp7E3.args=_tmp28A,((_tmp7E3.c_varargs=_tmp28B,((_tmp7E3.cyc_varargs=_tmp28C,((
_tmp7E3.rgn_po=_tmp28D,((_tmp7E3.attributes=_tmp28E,_tmp7E3)))))))))))))))),
_tmp7E4)))),_tmp7E2)))));struct Cyc_Absyn_PointerType_struct _tmp7EE;struct Cyc_Absyn_PtrAtts
_tmp7ED;struct Cyc_Absyn_PtrInfo _tmp7EC;struct Cyc_Absyn_PointerType_struct*
_tmp7EB;return(void*)((_tmp7EB=_cycalloc(sizeof(*_tmp7EB)),((_tmp7EB[0]=((
_tmp7EE.tag=5,((_tmp7EE.f1=((_tmp7EC.elt_typ=new_fn_typ,((_tmp7EC.elt_tq=_tmp27E,((
_tmp7EC.ptr_atts=((_tmp7ED.rgn=_tmp280,((_tmp7ED.nullable=_tmp281,((_tmp7ED.bounds=
_tmp282,((_tmp7ED.zero_term=_tmp283,((_tmp7ED.ptrloc=0,_tmp7ED)))))))))),_tmp7EC)))))),
_tmp7EE)))),_tmp7EB))));};}_LL142:;_LL143: goto _LL13F;_LL13F:;}goto _LL13A;_LL13D:;
_LL13E: goto _LL13A;_LL13A:;}{const char*_tmp7F2;void*_tmp7F1[1];struct Cyc_String_pa_struct
_tmp7F0;return(_tmp7F0.tag=0,((_tmp7F0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7F1[0]=& _tmp7F0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7F2="expecting polymorphic type but found %s",_tag_dyneither(
_tmp7F2,sizeof(char),40))),_tag_dyneither(_tmp7F1,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcexp_tcExp(te,(
void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(
Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(
c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,
loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp7F7;void*_tmp7F6[2];struct Cyc_String_pa_struct _tmp7F5;struct Cyc_String_pa_struct
_tmp7F4;void*_tmp29E=(_tmp7F4.tag=0,((_tmp7F4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7F5.tag=0,((_tmp7F5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7F6[0]=&
_tmp7F5,((_tmp7F6[1]=& _tmp7F4,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp7F7="cannot cast %s to %s",
_tag_dyneither(_tmp7F7,sizeof(char),21))),_tag_dyneither(_tmp7F6,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp29E;};}}}{struct _tuple0 _tmp7F8;struct
_tuple0 _tmp2A4=(_tmp7F8.f1=e->r,((_tmp7F8.f2=Cyc_Tcutil_compress(t),_tmp7F8)));
void*_tmp2A5;struct Cyc_Absyn_MallocInfo _tmp2A7;int _tmp2A8;void*_tmp2A9;struct Cyc_Absyn_PtrInfo
_tmp2AB;struct Cyc_Absyn_PtrAtts _tmp2AC;union Cyc_Absyn_Constraint*_tmp2AD;union
Cyc_Absyn_Constraint*_tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;_LL145: _tmp2A5=
_tmp2A4.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp2A6=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2A5;if(_tmp2A6->tag != 34)goto _LL147;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A7.fat_result;}};
_tmp2A9=_tmp2A4.f2;{struct Cyc_Absyn_PointerType_struct*_tmp2AA=(struct Cyc_Absyn_PointerType_struct*)
_tmp2A9;if(_tmp2AA->tag != 5)goto _LL147;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AB.ptr_atts;
_tmp2AD=_tmp2AC.nullable;_tmp2AE=_tmp2AC.bounds;_tmp2AF=_tmp2AC.zero_term;}};
_LL146: if((_tmp2A8  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2AF)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2AD)){void*_tmp2B0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2AE);struct Cyc_Absyn_Exp*_tmp2B2;_LL14A: {struct Cyc_Absyn_Upper_b_struct*
_tmp2B1=(struct Cyc_Absyn_Upper_b_struct*)_tmp2B0;if(_tmp2B1->tag != 1)goto _LL14C;
else{_tmp2B2=_tmp2B1->f1;}}_LL14B: if((Cyc_Evexp_eval_const_uint_exp(_tmp2B2)).f1
== 1){const char*_tmp7FB;void*_tmp7FA;(_tmp7FA=0,Cyc_Tcutil_warn(loc,((_tmp7FB="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp7FB,sizeof(char),60))),_tag_dyneither(_tmp7FA,sizeof(void*),0)));}
goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}goto _LL144;_LL147:;_LL148: goto
_LL144;_LL144:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp2B5=0;struct Cyc_Absyn_Tqual _tmp2B6=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp2B7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2B9;
void*_tmp2BA;struct Cyc_Absyn_Tqual _tmp2BB;struct Cyc_Absyn_PtrAtts _tmp2BC;union
Cyc_Absyn_Constraint*_tmp2BD;_LL14F: {struct Cyc_Absyn_PointerType_struct*_tmp2B8=(
struct Cyc_Absyn_PointerType_struct*)_tmp2B7;if(_tmp2B8->tag != 5)goto _LL151;else{
_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.elt_typ;_tmp2BB=_tmp2B9.elt_tq;_tmp2BC=
_tmp2B9.ptr_atts;_tmp2BD=_tmp2BC.zero_term;}}_LL150:{void**_tmp7FC;_tmp2B5=((
_tmp7FC=_cycalloc(sizeof(*_tmp7FC)),((_tmp7FC[0]=_tmp2BA,_tmp7FC))));}_tmp2B6=
_tmp2BB;goto _LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}{struct _RegionHandle*
_tmp2BF=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp2BF,te),_tmp2B5,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp2BF,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*_tmp7FF;void*_tmp7FE;(
_tmp7FE=0,Cyc_Tcutil_terr(e->loc,((_tmp7FF="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp7FF,sizeof(char),46))),_tag_dyneither(_tmp7FE,sizeof(void*),0)));}{
void*_tmp2C2=e->r;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;_LL154: {
struct Cyc_Absyn_Subscript_e_struct*_tmp2C3=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp2C2;if(_tmp2C3->tag != 24)goto _LL156;else{_tmp2C4=_tmp2C3->f1;_tmp2C5=_tmp2C3->f2;}}
_LL155:{void*_tmp2C6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C4->topt))->v);_LL159: {struct Cyc_Absyn_TupleType_struct*
_tmp2C7=(struct Cyc_Absyn_TupleType_struct*)_tmp2C6;if(_tmp2C7->tag != 11)goto
_LL15B;}_LL15A: goto _LL158;_LL15B:;_LL15C: e0->r=(Cyc_Absyn_add_exp(_tmp2C4,
_tmp2C5,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp2C4,_tmp2C5);_LL158:;}goto _LL153;
_LL156:;_LL157: goto _LL153;_LL153:;}{void*_tmp2C8=e->r;int _tmp2CA;int _tmp2CC;
_LL15E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2C9=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2C8;if(_tmp2C9->tag != 22)goto _LL160;else{_tmp2CA=_tmp2C9->f3;if(_tmp2CA != 1)
goto _LL160;}}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2CB=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2C8;if(_tmp2CB->tag != 23)goto
_LL162;else{_tmp2CC=_tmp2CB->f3;if(_tmp2CC != 1)goto _LL162;}}_LL161:{const char*
_tmp802;void*_tmp801;(_tmp801=0,Cyc_Tcutil_terr(e->loc,((_tmp802="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp802,sizeof(char),50))),_tag_dyneither(_tmp801,sizeof(void*),0)));}
goto _LL15D;_LL162:;_LL163: goto _LL15D;_LL15D:;}{int _tmp2D0;void*_tmp2D1;struct
_tuple13 _tmp2CF=Cyc_Tcutil_addressof_props(te,e);_tmp2D0=_tmp2CF.f1;_tmp2D1=
_tmp2CF.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2D0){tq.print_const=
1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp2D1,tq,Cyc_Absyn_false_conref);return t;};};};};}
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp806;void*_tmp805[1];struct Cyc_String_pa_struct _tmp804;(_tmp804.tag=
0,((_tmp804.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp805[0]=& _tmp804,Cyc_Tcutil_terr(loc,((_tmp806="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp806,sizeof(char),55))),_tag_dyneither(_tmp805,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2D5=Cyc_Tcutil_compress(*topt);void*_tmp2D7;_LL165: {
struct Cyc_Absyn_TagType_struct*_tmp2D6=(struct Cyc_Absyn_TagType_struct*)_tmp2D5;
if(_tmp2D6->tag != 20)goto _LL167;else{_tmp2D7=(void*)_tmp2D6->f1;}}_LL166: {
struct Cyc_Absyn_Exp*_tmp2D8=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp809;struct Cyc_Absyn_ValueofType_struct*_tmp808;struct Cyc_Absyn_ValueofType_struct*
_tmp2D9=(_tmp808=_cycalloc(sizeof(*_tmp808)),((_tmp808[0]=((_tmp809.tag=19,((
_tmp809.f1=_tmp2D8,_tmp809)))),_tmp808)));if(Cyc_Tcutil_unify(_tmp2D7,(void*)
_tmp2D9))return _tmp2D5;goto _LL164;}_LL167:;_LL168: goto _LL164;_LL164:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);{void*_tmp2DC=n;struct _dyneither_ptr*_tmp2DE;
unsigned int _tmp2E0;_LL16A: {struct Cyc_Absyn_StructField_struct*_tmp2DD=(struct
Cyc_Absyn_StructField_struct*)_tmp2DC;if(_tmp2DD->tag != 0)goto _LL16C;else{
_tmp2DE=_tmp2DD->f1;}}_LL16B: {int bad_type=1;{void*_tmp2E1=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_AggrInfo _tmp2E3;union Cyc_Absyn_AggrInfoU _tmp2E4;struct Cyc_Absyn_Aggrdecl**
_tmp2E5;struct Cyc_List_List*_tmp2E7;_LL16F: {struct Cyc_Absyn_AggrType_struct*
_tmp2E2=(struct Cyc_Absyn_AggrType_struct*)_tmp2E1;if(_tmp2E2->tag != 12)goto
_LL171;else{_tmp2E3=_tmp2E2->f1;_tmp2E4=_tmp2E3.aggr_info;if((_tmp2E4.KnownAggr).tag
!= 2)goto _LL171;_tmp2E5=(struct Cyc_Absyn_Aggrdecl**)(_tmp2E4.KnownAggr).val;}}
_LL170: if((*_tmp2E5)->impl == 0)goto _LL16E;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2DE,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2E5)->impl))->fields)){const char*_tmp80D;void*_tmp80C[1];
struct Cyc_String_pa_struct _tmp80B;(_tmp80B.tag=0,((_tmp80B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2DE),((_tmp80C[0]=& _tmp80B,Cyc_Tcutil_terr(
loc,((_tmp80D="no field of struct/union has name %s",_tag_dyneither(_tmp80D,
sizeof(char),37))),_tag_dyneither(_tmp80C,sizeof(void*),1)))))));}bad_type=0;
goto _LL16E;_LL171: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2E6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2E1;if(_tmp2E6->tag != 13)goto _LL173;else{_tmp2E7=_tmp2E6->f2;}}_LL172: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2DE,_tmp2E7)){const char*_tmp811;void*_tmp810[1];struct Cyc_String_pa_struct
_tmp80F;(_tmp80F.tag=0,((_tmp80F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2DE),((_tmp810[0]=& _tmp80F,Cyc_Tcutil_terr(loc,((_tmp811="no field of struct/union has name %s",
_tag_dyneither(_tmp811,sizeof(char),37))),_tag_dyneither(_tmp810,sizeof(void*),1)))))));}
bad_type=0;goto _LL16E;_LL173:;_LL174: goto _LL16E;_LL16E:;}if(bad_type){const char*
_tmp815;void*_tmp814[1];struct Cyc_String_pa_struct _tmp813;(_tmp813.tag=0,((
_tmp813.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp814[0]=& _tmp813,Cyc_Tcutil_terr(loc,((_tmp815="%s is not a known struct/union type",
_tag_dyneither(_tmp815,sizeof(char),36))),_tag_dyneither(_tmp814,sizeof(void*),1)))))));}
goto _LL169;}_LL16C: {struct Cyc_Absyn_TupleIndex_struct*_tmp2DF=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2DC;if(_tmp2DF->tag != 1)goto _LL169;else{_tmp2E0=_tmp2DF->f1;}}_LL16D: {int
bad_type=1;{void*_tmp2F1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2F3;
union Cyc_Absyn_AggrInfoU _tmp2F4;struct Cyc_Absyn_Aggrdecl**_tmp2F5;struct Cyc_List_List*
_tmp2F7;struct Cyc_List_List*_tmp2F9;struct Cyc_Absyn_DatatypeFieldInfo _tmp2FB;
union Cyc_Absyn_DatatypeFieldInfoU _tmp2FC;struct _tuple2 _tmp2FD;struct Cyc_Absyn_Datatypefield*
_tmp2FE;_LL176: {struct Cyc_Absyn_AggrType_struct*_tmp2F2=(struct Cyc_Absyn_AggrType_struct*)
_tmp2F1;if(_tmp2F2->tag != 12)goto _LL178;else{_tmp2F3=_tmp2F2->f1;_tmp2F4=_tmp2F3.aggr_info;
if((_tmp2F4.KnownAggr).tag != 2)goto _LL178;_tmp2F5=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2F4.KnownAggr).val;}}_LL177: if((*_tmp2F5)->impl == 0)goto _LL175;_tmp2F7=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2F5)->impl))->fields;goto _LL179;
_LL178: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2F6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F1;if(_tmp2F6->tag != 13)goto _LL17A;else{_tmp2F7=_tmp2F6->f2;}}_LL179: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F7)<= _tmp2E0){const char*
_tmp81A;void*_tmp819[2];struct Cyc_Int_pa_struct _tmp818;struct Cyc_Int_pa_struct
_tmp817;(_tmp817.tag=1,((_tmp817.f1=(unsigned long)((int)_tmp2E0),((_tmp818.tag=
1,((_tmp818.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2F7),((_tmp819[0]=& _tmp818,((_tmp819[1]=& _tmp817,Cyc_Tcutil_terr(loc,((
_tmp81A="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp81A,
sizeof(char),46))),_tag_dyneither(_tmp819,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17A: {struct Cyc_Absyn_TupleType_struct*_tmp2F8=(struct Cyc_Absyn_TupleType_struct*)
_tmp2F1;if(_tmp2F8->tag != 11)goto _LL17C;else{_tmp2F9=_tmp2F8->f1;}}_LL17B: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F9)<= _tmp2E0){const char*
_tmp81F;void*_tmp81E[2];struct Cyc_Int_pa_struct _tmp81D;struct Cyc_Int_pa_struct
_tmp81C;(_tmp81C.tag=1,((_tmp81C.f1=(unsigned long)((int)_tmp2E0),((_tmp81D.tag=
1,((_tmp81D.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2F9),((_tmp81E[0]=& _tmp81D,((_tmp81E[1]=& _tmp81C,Cyc_Tcutil_terr(loc,((
_tmp81F="tuple has too few components: %d <= %d",_tag_dyneither(_tmp81F,sizeof(
char),39))),_tag_dyneither(_tmp81E,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL175;_LL17C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2FA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2F1;if(_tmp2FA->tag != 4)goto _LL17E;else{_tmp2FB=_tmp2FA->f1;_tmp2FC=_tmp2FB.field_info;
if((_tmp2FC.KnownDatatypefield).tag != 2)goto _LL17E;_tmp2FD=(struct _tuple2)(
_tmp2FC.KnownDatatypefield).val;_tmp2FE=_tmp2FD.f2;}}_LL17D: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp2FE->typs)< _tmp2E0){const char*_tmp824;
void*_tmp823[2];struct Cyc_Int_pa_struct _tmp822;struct Cyc_Int_pa_struct _tmp821;(
_tmp821.tag=1,((_tmp821.f1=(unsigned long)((int)_tmp2E0),((_tmp822.tag=1,((
_tmp822.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FE->typs),((_tmp823[0]=& _tmp822,((_tmp823[1]=& _tmp821,Cyc_Tcutil_terr(loc,((
_tmp824="datatype field has too few components: %d < %d",_tag_dyneither(_tmp824,
sizeof(char),47))),_tag_dyneither(_tmp823,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17E:;_LL17F: goto _LL175;_LL175:;}if(bad_type){const char*_tmp828;
void*_tmp827[1];struct Cyc_String_pa_struct _tmp826;(_tmp826.tag=0,((_tmp826.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp827[
0]=& _tmp826,Cyc_Tcutil_terr(loc,((_tmp828="%s is not a known type",
_tag_dyneither(_tmp828,sizeof(char),23))),_tag_dyneither(_tmp827,sizeof(void*),1)))))));}
goto _LL169;}_LL169:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp30E=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp30F=Cyc_Tcenv_clear_notreadctxt(_tmp30E,te_orig);Cyc_Tcexp_tcExp(_tmp30F,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp310=t;struct Cyc_Absyn_PtrInfo _tmp313;void*_tmp314;struct Cyc_Absyn_PtrAtts
_tmp315;void*_tmp316;union Cyc_Absyn_Constraint*_tmp317;union Cyc_Absyn_Constraint*
_tmp318;_LL181: {struct Cyc_Absyn_Evar_struct*_tmp311=(struct Cyc_Absyn_Evar_struct*)
_tmp310;if(_tmp311->tag != 1)goto _LL183;}_LL182: {struct Cyc_List_List*_tmp319=Cyc_Tcenv_lookup_type_vars(
_tmp30F);struct Cyc_Core_Opt*_tmp829;void*_tmp31A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mko,((_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829->v=_tmp319,
_tmp829)))));struct Cyc_Core_Opt*_tmp82A;void*_tmp31B=Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A->v=
_tmp319,_tmp82A)))));union Cyc_Absyn_Constraint*_tmp31C=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*_tmp31D=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp82B;struct Cyc_Absyn_PtrAtts
_tmp31E=(_tmp82B.rgn=_tmp31B,((_tmp82B.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp82B.bounds=_tmp31C,((_tmp82B.zero_term=_tmp31D,((
_tmp82B.ptrloc=0,_tmp82B)))))))));struct Cyc_Absyn_PointerType_struct _tmp831;
struct Cyc_Absyn_PtrInfo _tmp830;struct Cyc_Absyn_PointerType_struct*_tmp82F;struct
Cyc_Absyn_PointerType_struct*_tmp31F=(_tmp82F=_cycalloc(sizeof(*_tmp82F)),((
_tmp82F[0]=((_tmp831.tag=5,((_tmp831.f1=((_tmp830.elt_typ=_tmp31A,((_tmp830.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp830.ptr_atts=_tmp31E,_tmp830)))))),_tmp831)))),
_tmp82F)));Cyc_Tcutil_unify(t,(void*)_tmp31F);_tmp314=_tmp31A;_tmp316=_tmp31B;
_tmp317=_tmp31C;_tmp318=_tmp31D;goto _LL184;}_LL183: {struct Cyc_Absyn_PointerType_struct*
_tmp312=(struct Cyc_Absyn_PointerType_struct*)_tmp310;if(_tmp312->tag != 5)goto
_LL185;else{_tmp313=_tmp312->f1;_tmp314=_tmp313.elt_typ;_tmp315=_tmp313.ptr_atts;
_tmp316=_tmp315.rgn;_tmp317=_tmp315.bounds;_tmp318=_tmp315.zero_term;}}_LL184:
Cyc_Tcenv_check_rgn_accessible(_tmp30F,loc,_tmp316);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp317);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp314),& Cyc_Tcutil_tmk)){
const char*_tmp834;void*_tmp833;(_tmp833=0,Cyc_Tcutil_terr(loc,((_tmp834="can't dereference abstract pointer type",
_tag_dyneither(_tmp834,sizeof(char),40))),_tag_dyneither(_tmp833,sizeof(void*),0)));}
return _tmp314;_LL185:;_LL186: {const char*_tmp838;void*_tmp837[1];struct Cyc_String_pa_struct
_tmp836;return(_tmp836.tag=0,((_tmp836.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp837[0]=& _tmp836,Cyc_Tcexp_expr_err(
_tmp30F,loc,topt,((_tmp838="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp838,sizeof(char),39))),_tag_dyneither(_tmp837,sizeof(void*),1)))))));}
_LL180:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp32B=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(_tmp32B,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp32C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp32E;
union Cyc_Absyn_AggrInfoU _tmp32F;struct Cyc_Absyn_Aggrdecl**_tmp330;struct Cyc_Absyn_Aggrdecl*
_tmp331;struct Cyc_List_List*_tmp332;enum Cyc_Absyn_AggrKind _tmp334;struct Cyc_List_List*
_tmp335;_LL188: {struct Cyc_Absyn_AggrType_struct*_tmp32D=(struct Cyc_Absyn_AggrType_struct*)
_tmp32C;if(_tmp32D->tag != 12)goto _LL18A;else{_tmp32E=_tmp32D->f1;_tmp32F=_tmp32E.aggr_info;
if((_tmp32F.KnownAggr).tag != 2)goto _LL18A;_tmp330=(struct Cyc_Absyn_Aggrdecl**)(
_tmp32F.KnownAggr).val;_tmp331=*_tmp330;_tmp332=_tmp32E.targs;}}_LL189: {struct
Cyc_Absyn_Aggrfield*_tmp33A=Cyc_Absyn_lookup_decl_field(_tmp331,f);if(_tmp33A == 
0){const char*_tmp83D;void*_tmp83C[2];struct Cyc_String_pa_struct _tmp83B;struct Cyc_String_pa_struct
_tmp83A;return(_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp331->name)),((_tmp83C[0]=& _tmp83B,((
_tmp83C[1]=& _tmp83A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp83D="type %s has no %s field",
_tag_dyneither(_tmp83D,sizeof(char),24))),_tag_dyneither(_tmp83C,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp331->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp33F=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp340=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp33F,_tmp33F,_tmp331->tvs,_tmp332);t2=Cyc_Tcutil_rsubstitute(_tmp33F,_tmp340,
_tmp33A->type);if(((_tmp331->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp331->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp841;void*_tmp840[1];struct Cyc_String_pa_struct _tmp83F;return(
_tmp83F.tag=0,((_tmp83F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp840[0]=& _tmp83F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp841="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp841,sizeof(char),56))),_tag_dyneither(_tmp840,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp331->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp845;void*_tmp844[1];struct Cyc_String_pa_struct _tmp843;return(
_tmp843.tag=0,((_tmp843.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp844[0]=& _tmp843,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp845="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp845,sizeof(char),81))),_tag_dyneither(_tmp844,sizeof(void*),1)))))));}}
return t2;};}_LL18A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp333=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp32C;if(_tmp333->tag != 13)goto _LL18C;else{_tmp334=_tmp333->f1;_tmp335=_tmp333->f2;}}
_LL18B: {struct Cyc_Absyn_Aggrfield*_tmp347=Cyc_Absyn_lookup_field(_tmp335,f);if(
_tmp347 == 0){const char*_tmp849;void*_tmp848[1];struct Cyc_String_pa_struct _tmp847;
return(_tmp847.tag=0,((_tmp847.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp848[0]=& _tmp847,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp849="type has no %s field",
_tag_dyneither(_tmp849,sizeof(char),21))),_tag_dyneither(_tmp848,sizeof(void*),1)))))));}
if((_tmp334 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp347->type)){const char*_tmp84D;void*_tmp84C[1];struct Cyc_String_pa_struct
_tmp84B;return(_tmp84B.tag=0,((_tmp84B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp84C[0]=& _tmp84B,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp84D="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp84D,sizeof(char),56))),_tag_dyneither(_tmp84C,sizeof(void*),1)))))));}return
_tmp347->type;}_LL18C:{struct Cyc_Absyn_ArrayType_struct*_tmp336=(struct Cyc_Absyn_ArrayType_struct*)
_tmp32C;if(_tmp336->tag != 9)goto _LL18E;}{const char*_tmp84E;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp84E="size",_tag_dyneither(_tmp84E,sizeof(char),5))))
== 0))goto _LL18E;};_LL18D: goto _LL18F;_LL18E:{struct Cyc_Absyn_PointerType_struct*
_tmp338=(struct Cyc_Absyn_PointerType_struct*)_tmp32C;if(_tmp338->tag != 5)goto
_LL190;}{const char*_tmp84F;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp84F="size",
_tag_dyneither(_tmp84F,sizeof(char),5))))== 0))goto _LL190;};_LL18F:{const char*
_tmp853;void*_tmp852[1];struct Cyc_String_pa_struct _tmp851;(_tmp851.tag=0,((
_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp852[0]=& _tmp851,Cyc_Tcutil_warn(e->loc,((_tmp853="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp853,sizeof(char),54))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp859;struct Cyc_List_List*_tmp858;struct Cyc_Absyn_Primop_e_struct*
_tmp857;outer_e->r=(void*)((_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857[0]=((
_tmp859.tag=3,((_tmp859.f1=Cyc_Absyn_Numelts,((_tmp859.f2=((_tmp858=_cycalloc(
sizeof(*_tmp858)),((_tmp858->hd=e,((_tmp858->tl=0,_tmp858)))))),_tmp859)))))),
_tmp857))));}return Cyc_Absyn_uint_typ;_LL190:;_LL191: {const char*_tmp85A;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp85A="size",_tag_dyneither(_tmp85A,sizeof(char),5))))
== 0){const char*_tmp85E;void*_tmp85D[1];struct Cyc_String_pa_struct _tmp85C;return(
_tmp85C.tag=0,((_tmp85C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp85D[0]=& _tmp85C,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp85E="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp85E,sizeof(char),44))),_tag_dyneither(_tmp85D,sizeof(void*),1)))))));}
else{const char*_tmp862;void*_tmp861[1];struct Cyc_String_pa_struct _tmp860;return(
_tmp860.tag=0,((_tmp860.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp861[0]=& _tmp860,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp862="expecting struct or union, found %s",_tag_dyneither(
_tmp862,sizeof(char),36))),_tag_dyneither(_tmp861,sizeof(void*),1)))))));}}
_LL187:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp35B=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
_tmp35B,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp35C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp35E;void*_tmp35F;struct Cyc_Absyn_PtrAtts _tmp360;void*_tmp361;union Cyc_Absyn_Constraint*
_tmp362;union Cyc_Absyn_Constraint*_tmp363;_LL193: {struct Cyc_Absyn_PointerType_struct*
_tmp35D=(struct Cyc_Absyn_PointerType_struct*)_tmp35C;if(_tmp35D->tag != 5)goto
_LL195;else{_tmp35E=_tmp35D->f1;_tmp35F=_tmp35E.elt_typ;_tmp360=_tmp35E.ptr_atts;
_tmp361=_tmp360.rgn;_tmp362=_tmp360.bounds;_tmp363=_tmp360.zero_term;}}_LL194:
Cyc_Tcutil_check_nonzero_bound(loc,_tmp362);{void*_tmp364=Cyc_Tcutil_compress(
_tmp35F);struct Cyc_Absyn_AggrInfo _tmp366;union Cyc_Absyn_AggrInfoU _tmp367;struct
Cyc_Absyn_Aggrdecl**_tmp368;struct Cyc_Absyn_Aggrdecl*_tmp369;struct Cyc_List_List*
_tmp36A;enum Cyc_Absyn_AggrKind _tmp36C;struct Cyc_List_List*_tmp36D;_LL198: {
struct Cyc_Absyn_AggrType_struct*_tmp365=(struct Cyc_Absyn_AggrType_struct*)
_tmp364;if(_tmp365->tag != 12)goto _LL19A;else{_tmp366=_tmp365->f1;_tmp367=_tmp366.aggr_info;
if((_tmp367.KnownAggr).tag != 2)goto _LL19A;_tmp368=(struct Cyc_Absyn_Aggrdecl**)(
_tmp367.KnownAggr).val;_tmp369=*_tmp368;_tmp36A=_tmp366.targs;}}_LL199: {struct
Cyc_Absyn_Aggrfield*_tmp36E=Cyc_Absyn_lookup_decl_field(_tmp369,f);if(_tmp36E == 
0){const char*_tmp867;void*_tmp866[2];struct Cyc_String_pa_struct _tmp865;struct Cyc_String_pa_struct
_tmp864;return(_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp369->name)),((_tmp866[0]=& _tmp865,((
_tmp866[1]=& _tmp864,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp867="type %s has no %s field",
_tag_dyneither(_tmp867,sizeof(char),24))),_tag_dyneither(_tmp866,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp373=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp373,_tmp373,_tmp369->tvs,_tmp36A);t3=Cyc_Tcutil_rsubstitute(_tmp373,_tmp374,
_tmp36E->type);}if(((_tmp369->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp369->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp86B;void*_tmp86A[1];struct Cyc_String_pa_struct _tmp869;return(
_tmp869.tag=0,((_tmp869.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp86A[0]=& _tmp869,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86B="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp86B,sizeof(char),56))),_tag_dyneither(_tmp86A,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_struct _tmp86D;return(
_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp86E[0]=& _tmp86D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86F="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp86F,sizeof(char),72))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))));}}
return t3;};}_LL19A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp364;if(_tmp36B->tag != 13)goto _LL19C;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36B->f2;}}
_LL19B: {struct Cyc_Absyn_Aggrfield*_tmp37B=Cyc_Absyn_lookup_field(_tmp36D,f);if(
_tmp37B == 0){const char*_tmp873;void*_tmp872[1];struct Cyc_String_pa_struct _tmp871;
return(_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp872[0]=& _tmp871,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp873="type has no %s field",
_tag_dyneither(_tmp873,sizeof(char),21))),_tag_dyneither(_tmp872,sizeof(void*),1)))))));}
if((_tmp36C == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp37B->type)){const char*_tmp877;void*_tmp876[1];struct Cyc_String_pa_struct
_tmp875;return(_tmp875.tag=0,((_tmp875.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp876[0]=& _tmp875,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp877="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp877,sizeof(char),56))),_tag_dyneither(_tmp876,sizeof(void*),1)))))));}return
_tmp37B->type;}_LL19C:;_LL19D: goto _LL197;_LL197:;}goto _LL192;_LL195:;_LL196: goto
_LL192;_LL192:;}{const char*_tmp87B;void*_tmp87A[1];struct Cyc_String_pa_struct
_tmp879;return(_tmp879.tag=0,((_tmp879.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp87A[0]=& _tmp879,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87B="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp87B,sizeof(char),44))),_tag_dyneither(_tmp87A,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp386;int
_tmp387;struct _tuple14 _tmp385=Cyc_Evexp_eval_const_uint_exp(index);_tmp386=
_tmp385.f1;_tmp387=_tmp385.f2;if(!_tmp387){const char*_tmp87E;void*_tmp87D;return(
_tmp87D=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp87E="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp87E,sizeof(char),47))),_tag_dyneither(_tmp87D,sizeof(void*),0)));}{
struct _handler_cons _tmp38A;_push_handler(& _tmp38A);{int _tmp38C=0;if(setjmp(
_tmp38A.handler))_tmp38C=1;if(!_tmp38C){{void*_tmp38D=(*((struct _tuple18*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp386)).f2;_npop_handler(0);
return _tmp38D;};_pop_handler();}else{void*_tmp38B=(void*)_exn_thrown;void*
_tmp38F=_tmp38B;_LL19F: {struct Cyc_List_Nth_struct*_tmp390=(struct Cyc_List_Nth_struct*)
_tmp38F;if(_tmp390->tag != Cyc_List_Nth)goto _LL1A1;}_LL1A0: {const char*_tmp883;
void*_tmp882[2];struct Cyc_Int_pa_struct _tmp881;struct Cyc_Int_pa_struct _tmp880;
return(_tmp880.tag=1,((_tmp880.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp881.tag=1,((_tmp881.f1=(unsigned long)((int)_tmp386),((
_tmp882[0]=& _tmp881,((_tmp882[1]=& _tmp880,Cyc_Tcexp_expr_err(te,loc,0,((_tmp883="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp883,sizeof(char),41))),_tag_dyneither(_tmp882,sizeof(void*),2)))))))))))));}
_LL1A1:;_LL1A2:(void)_throw(_tmp38F);_LL19E:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp395=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp396=Cyc_Tcenv_clear_notreadctxt(_tmp395,
te_orig);Cyc_Tcexp_tcExp(_tmp396,0,e1);Cyc_Tcexp_tcExp(_tmp396,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp396,e2)){const char*_tmp887;void*_tmp886[1];
struct Cyc_String_pa_struct _tmp885;return(_tmp885.tag=0,((_tmp885.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp886[0]=&
_tmp885,Cyc_Tcexp_expr_err(_tmp396,e2->loc,topt,((_tmp887="expecting int subscript, found %s",
_tag_dyneither(_tmp887,sizeof(char),34))),_tag_dyneither(_tmp886,sizeof(void*),1)))))));}{
void*_tmp39A=t1;struct Cyc_Absyn_PtrInfo _tmp39C;void*_tmp39D;struct Cyc_Absyn_Tqual
_tmp39E;struct Cyc_Absyn_PtrAtts _tmp39F;void*_tmp3A0;union Cyc_Absyn_Constraint*
_tmp3A1;union Cyc_Absyn_Constraint*_tmp3A2;struct Cyc_List_List*_tmp3A4;_LL1A4: {
struct Cyc_Absyn_PointerType_struct*_tmp39B=(struct Cyc_Absyn_PointerType_struct*)
_tmp39A;if(_tmp39B->tag != 5)goto _LL1A6;else{_tmp39C=_tmp39B->f1;_tmp39D=_tmp39C.elt_typ;
_tmp39E=_tmp39C.elt_tq;_tmp39F=_tmp39C.ptr_atts;_tmp3A0=_tmp39F.rgn;_tmp3A1=
_tmp39F.bounds;_tmp3A2=_tmp39F.zero_term;}}_LL1A5: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3A2)){int emit_warning=1;{void*_tmp3A5=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp3A1);struct Cyc_Absyn_Exp*_tmp3A7;_LL1AB: {struct Cyc_Absyn_Upper_b_struct*
_tmp3A6=(struct Cyc_Absyn_Upper_b_struct*)_tmp3A5;if(_tmp3A6->tag != 1)goto _LL1AD;
else{_tmp3A7=_tmp3A6->f1;}}_LL1AC: if(Cyc_Tcutil_is_const_exp(_tmp396,e2)){
unsigned int _tmp3AA;int _tmp3AB;struct _tuple14 _tmp3A9=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A9.f2;if(_tmp3AB){unsigned int _tmp3AD;int
_tmp3AE;struct _tuple14 _tmp3AC=Cyc_Evexp_eval_const_uint_exp(_tmp3A7);_tmp3AD=
_tmp3AC.f1;_tmp3AE=_tmp3AC.f2;if(_tmp3AE  && _tmp3AD > _tmp3AA)emit_warning=0;}}
goto _LL1AA;_LL1AD: {struct Cyc_Absyn_DynEither_b_struct*_tmp3A8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3A5;if(_tmp3A8->tag != 0)goto _LL1AA;}_LL1AE: emit_warning=0;goto _LL1AA;_LL1AA:;}
if(emit_warning){const char*_tmp88A;void*_tmp889;(_tmp889=0,Cyc_Tcutil_warn(e2->loc,((
_tmp88A="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp88A,sizeof(char),63))),_tag_dyneither(_tmp889,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp396,e2)){unsigned int _tmp3B2;int _tmp3B3;
struct _tuple14 _tmp3B1=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3B2=_tmp3B1.f1;
_tmp3B3=_tmp3B1.f2;if(_tmp3B3)Cyc_Tcutil_check_bound(loc,_tmp3B2,_tmp3A1);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3A1) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3A2)){const char*_tmp88D;void*_tmp88C;(_tmp88C=0,
Cyc_Tcutil_warn(e1->loc,((_tmp88D="subscript applied to pointer to one object",
_tag_dyneither(_tmp88D,sizeof(char),43))),_tag_dyneither(_tmp88C,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3A1);}}Cyc_Tcenv_check_rgn_accessible(
_tmp396,loc,_tmp3A0);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp39D),& Cyc_Tcutil_tmk)){
const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(e1->loc,((_tmp890="can't subscript an abstract pointer",
_tag_dyneither(_tmp890,sizeof(char),36))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}
return _tmp39D;_LL1A6: {struct Cyc_Absyn_TupleType_struct*_tmp3A3=(struct Cyc_Absyn_TupleType_struct*)
_tmp39A;if(_tmp3A3->tag != 11)goto _LL1A8;else{_tmp3A4=_tmp3A3->f1;}}_LL1A7: return
Cyc_Tcexp_ithTupleType(_tmp396,loc,_tmp3A4,e2);_LL1A8:;_LL1A9: {const char*
_tmp894;void*_tmp893[1];struct Cyc_String_pa_struct _tmp892;return(_tmp892.tag=0,((
_tmp892.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp893[0]=& _tmp892,Cyc_Tcexp_expr_err(_tmp396,loc,topt,((_tmp894="subscript applied to %s",
_tag_dyneither(_tmp894,sizeof(char),24))),_tag_dyneither(_tmp893,sizeof(void*),1)))))));}
_LL1A3:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp3BB=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3BD;
_LL1B0: {struct Cyc_Absyn_TupleType_struct*_tmp3BC=(struct Cyc_Absyn_TupleType_struct*)
_tmp3BB;if(_tmp3BC->tag != 11)goto _LL1B2;else{_tmp3BD=_tmp3BC->f1;}}_LL1B1: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3BD)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AF;for(0;es != 0;(es=es->tl,_tmp3BD=_tmp3BD->tl)){
int bogus=0;void*_tmp3BE=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(
_tmp3BD))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3BE,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)
_tmp3BD->hd)).f2,& bogus);{struct _tuple18*_tmp897;struct Cyc_List_List*_tmp896;
fields=((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896->hd=((_tmp897=_cycalloc(
sizeof(*_tmp897)),((_tmp897->f1=(*((struct _tuple18*)_tmp3BD->hd)).f1,((_tmp897->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp897)))))),((_tmp896->tl=fields,_tmp896))))));};}done=1;goto _LL1AF;_LL1B2:;
_LL1B3: goto _LL1AF;_LL1AF:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);{struct _tuple18*_tmp89A;struct Cyc_List_List*
_tmp899;fields=((_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899->hd=((_tmp89A=
_cycalloc(sizeof(*_tmp89A)),((_tmp89A->f1=Cyc_Absyn_empty_tqual(0),((_tmp89A->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp89A)))))),((_tmp899->tl=fields,_tmp899))))));};}{struct Cyc_Absyn_TupleType_struct
_tmp89D;struct Cyc_Absyn_TupleType_struct*_tmp89C;return(void*)((_tmp89C=
_cycalloc(sizeof(*_tmp89C)),((_tmp89C[0]=((_tmp89D.tag=11,((_tmp89D.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp89D)))),
_tmp89C))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){
const char*_tmp8A0;void*_tmp89F;return(_tmp89F=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8A0="tcCompoundLit",_tag_dyneither(_tmp8A0,sizeof(char),14))),_tag_dyneither(
_tmp89F,sizeof(void*),0)));}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,struct Cyc_List_List*
des){void*res_t2;struct _RegionHandle*_tmp3C7=Cyc_Tcenv_get_fnrgn(te);{int _tmp3C8=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp3C7,(struct Cyc_Absyn_Exp*(*)(
struct _tuple17*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp8A3;struct Cyc_Absyn_Const_e_struct*_tmp8A2;struct Cyc_Absyn_Const_e_struct*
_tmp3C9=(_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2[0]=((_tmp8A3.tag=0,((
_tmp8A3.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3C8),_tmp8A3)))),_tmp8A2)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3C9,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3CA=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp3C8 - 1);if(!Cyc_Tcutil_is_zero(_tmp3CA)){const char*_tmp8A6;
void*_tmp8A5;(_tmp8A5=0,Cyc_Tcutil_terr(_tmp3CA->loc,((_tmp8A6="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp8A6,sizeof(char),45))),_tag_dyneither(_tmp8A5,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp8A7;sz_exp->topt=((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((
_tmp8A7->v=Cyc_Absyn_uint_typ,_tmp8A7))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp8AB;void*_tmp8AA[1];struct Cyc_String_pa_struct _tmp8A9;(
_tmp8A9.tag=0,((_tmp8A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp8AA[0]=& _tmp8A9,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp8AB="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp8AB,sizeof(char),53))),_tag_dyneither(_tmp8AA,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple17*)des->hd)).f1;if(ds != 0){void*_tmp3D3=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3D6;_LL1B5: {struct Cyc_Absyn_FieldName_struct*_tmp3D4=(struct Cyc_Absyn_FieldName_struct*)
_tmp3D3;if(_tmp3D4->tag != 1)goto _LL1B7;}_LL1B6:{const char*_tmp8AE;void*_tmp8AD;(
_tmp8AD=0,Cyc_Tcutil_terr(loc,((_tmp8AE="only array index designators are supported",
_tag_dyneither(_tmp8AE,sizeof(char),43))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}
goto _LL1B4;_LL1B7: {struct Cyc_Absyn_ArrayElement_struct*_tmp3D5=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3D3;if(_tmp3D5->tag != 0)goto _LL1B4;else{_tmp3D6=_tmp3D5->f1;}}_LL1B8: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3D6);{unsigned int _tmp3DA;int _tmp3DB;struct _tuple14 _tmp3D9=Cyc_Evexp_eval_const_uint_exp(
_tmp3D6);_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D9.f2;if(!_tmp3DB){const char*_tmp8B1;
void*_tmp8B0;(_tmp8B0=0,Cyc_Tcutil_terr(_tmp3D6->loc,((_tmp8B1="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp8B1,sizeof(char),47))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}
else{if(_tmp3DA != offset){const char*_tmp8B6;void*_tmp8B5[2];struct Cyc_Int_pa_struct
_tmp8B4;struct Cyc_Int_pa_struct _tmp8B3;(_tmp8B3.tag=1,((_tmp8B3.f1=(
unsigned long)((int)_tmp3DA),((_tmp8B4.tag=1,((_tmp8B4.f1=(unsigned long)offset,((
_tmp8B5[0]=& _tmp8B4,((_tmp8B5[1]=& _tmp8B3,Cyc_Tcutil_terr(_tmp3D6->loc,((_tmp8B6="expecting index designator %d but found %d",
_tag_dyneither(_tmp8B6,sizeof(char),43))),_tag_dyneither(_tmp8B5,sizeof(void*),2)))))))))))));}}
goto _LL1B4;};_LL1B4:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3E2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1BA: {struct Cyc_Absyn_TagType_struct*_tmp3E3=(
struct Cyc_Absyn_TagType_struct*)_tmp3E2;if(_tmp3E3->tag != 20)goto _LL1BC;}_LL1BB:
goto _LL1B9;_LL1BC:;_LL1BD: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_struct _tmp8B8;(_tmp8B8.tag=0,((
_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp8B9[0]=& _tmp8B8,
Cyc_Tcutil_terr(bound->loc,((_tmp8BA="expecting unsigned int, found %s",
_tag_dyneither(_tmp8BA,sizeof(char),33))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))));}
_LL1B9:;}if(!(vd->tq).real_const){const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8BD="comprehension index variable is not declared const!",_tag_dyneither(
_tmp8BD,sizeof(char),52))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3E9=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3E9,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3E9,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3E9,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3EA=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp3EB=0;struct Cyc_Absyn_Tqual*_tmp3EC=0;union Cyc_Absyn_Constraint**
_tmp3ED=0;if(topt != 0){void*_tmp3EE=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3F0;struct Cyc_Absyn_ArrayInfo _tmp3F2;void*_tmp3F3;void**_tmp3F4;struct Cyc_Absyn_Tqual
_tmp3F5;struct Cyc_Absyn_Tqual*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F7;union Cyc_Absyn_Constraint*
_tmp3F8;union Cyc_Absyn_Constraint**_tmp3F9;_LL1BF: {struct Cyc_Absyn_PointerType_struct*
_tmp3EF=(struct Cyc_Absyn_PointerType_struct*)_tmp3EE;if(_tmp3EF->tag != 5)goto
_LL1C1;else{_tmp3F0=_tmp3EF->f1;}}_LL1C0: pinfo=_tmp3F0;_tmp3EB=(void**)& pinfo.elt_typ;
_tmp3EC=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp3ED=(union Cyc_Absyn_Constraint**)&(
pinfo.ptr_atts).zero_term;goto _LL1BE;_LL1C1: {struct Cyc_Absyn_ArrayType_struct*
_tmp3F1=(struct Cyc_Absyn_ArrayType_struct*)_tmp3EE;if(_tmp3F1->tag != 9)goto
_LL1C3;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F2.elt_type;_tmp3F4=(void**)&(
_tmp3F1->f1).elt_type;_tmp3F5=_tmp3F2.tq;_tmp3F6=(struct Cyc_Absyn_Tqual*)&(
_tmp3F1->f1).tq;_tmp3F7=_tmp3F2.num_elts;_tmp3F8=_tmp3F2.zero_term;_tmp3F9=(
union Cyc_Absyn_Constraint**)&(_tmp3F1->f1).zero_term;}}_LL1C2: _tmp3EB=(void**)
_tmp3F4;_tmp3EC=(struct Cyc_Absyn_Tqual*)_tmp3F6;_tmp3ED=(union Cyc_Absyn_Constraint**)
_tmp3F9;goto _LL1BE;_LL1C3:;_LL1C4: goto _LL1BE;_LL1BE:;}{void*t=Cyc_Tcexp_tcExp(
_tmp3EA,_tmp3EB,body);if(_tmp3EA->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp3EA,
bound)){const char*_tmp8C0;void*_tmp8BF;(_tmp8BF=0,Cyc_Tcutil_terr(bound->loc,((
_tmp8C0="bound is not constant",_tag_dyneither(_tmp8C0,sizeof(char),22))),
_tag_dyneither(_tmp8BF,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp3EA,
body)){const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,Cyc_Tcutil_terr(bound->loc,((
_tmp8C3="body is not constant",_tag_dyneither(_tmp8C3,sizeof(char),21))),
_tag_dyneither(_tmp8C2,sizeof(void*),0)));}}if(_tmp3ED != 0  && ((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3ED)){struct Cyc_Absyn_Exp*
_tmp3FE=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp8C4;_tmp3FE->topt=((
_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4->v=Cyc_Absyn_uint_typ,_tmp8C4))));}
bound=Cyc_Absyn_add_exp(bound,_tmp3FE,0);{struct Cyc_Core_Opt*_tmp8C5;bound->topt=((
_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5->v=Cyc_Absyn_uint_typ,_tmp8C5))));}*
is_zero_term=1;}{void*_tmp401=Cyc_Absyn_array_typ(t,_tmp3EC == 0?Cyc_Absyn_empty_tqual(
0):*_tmp3EC,(struct Cyc_Absyn_Exp*)bound,_tmp3ED == 0?Cyc_Absyn_false_conref:*
_tmp3ED,0);return _tmp401;};};};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp402;_push_handler(& _tmp402);{int _tmp404=0;if(setjmp(
_tmp402.handler))_tmp404=1;if(!_tmp404){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp403=(void*)_exn_thrown;void*_tmp406=_tmp403;_LL1C6: {
struct Cyc_Dict_Absent_struct*_tmp407=(struct Cyc_Dict_Absent_struct*)_tmp406;if(
_tmp407->tag != Cyc_Dict_Absent)goto _LL1C8;}_LL1C7:{const char*_tmp8C9;void*
_tmp8C8[1];struct Cyc_String_pa_struct _tmp8C7;(_tmp8C7.tag=0,((_tmp8C7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8C8[0]=&
_tmp8C7,Cyc_Tcutil_terr(loc,((_tmp8C9="unbound struct/union name %s",
_tag_dyneither(_tmp8C9,sizeof(char),29))),_tag_dyneither(_tmp8C8,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1C8:;_LL1C9:(void)_throw(
_tmp406);_LL1C5:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp8D1;void*_tmp8D0[1];const char*_tmp8CF;const char*_tmp8CE;
struct Cyc_String_pa_struct _tmp8CD;(_tmp8CD.tag=0,((_tmp8CD.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8CE="struct",_tag_dyneither(
_tmp8CE,sizeof(char),7)):((_tmp8CF="union",_tag_dyneither(_tmp8CF,sizeof(char),6)))),((
_tmp8D0[0]=& _tmp8CD,Cyc_Tcutil_terr(loc,((_tmp8D1="can't construct abstract %s",
_tag_dyneither(_tmp8D1,sizeof(char),28))),_tag_dyneither(_tmp8D0,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp410=Cyc_Tcenv_get_fnrgn(te);struct _tuple11 _tmp8D2;struct
_tuple11 _tmp411=(_tmp8D2.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8D2.f2=_tmp410,
_tmp8D2)));struct Cyc_List_List*_tmp412=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp410,Cyc_Tcutil_r_make_inst_var,&
_tmp411,ad->tvs);struct Cyc_List_List*_tmp413=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp410,Cyc_Tcutil_r_make_inst_var,&
_tmp411,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp414=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp412);struct Cyc_List_List*_tmp415=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp413);struct Cyc_List_List*_tmp416=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp410,_tmp412,_tmp413);struct Cyc_Absyn_AggrType_struct _tmp8DC;struct Cyc_Absyn_Aggrdecl**
_tmp8DB;struct Cyc_Absyn_AggrInfo _tmp8DA;struct Cyc_Absyn_AggrType_struct*_tmp8D9;
struct Cyc_Absyn_AggrType_struct*_tmp417=(_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((
_tmp8D9[0]=((_tmp8DC.tag=12,((_tmp8DC.f1=((_tmp8DA.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB[0]=ad,_tmp8DB))))),((_tmp8DA.targs=
_tmp414,_tmp8DA)))),_tmp8DC)))),_tmp8D9)));struct Cyc_List_List*_tmp418=*ts;
struct Cyc_List_List*_tmp419=_tmp415;while(_tmp418 != 0  && _tmp419 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp418->hd);Cyc_Tcutil_unify((
void*)_tmp418->hd,(void*)_tmp419->hd);_tmp418=_tmp418->tl;_tmp419=_tmp419->tl;}
if(_tmp418 != 0){const char*_tmp8DF;void*_tmp8DE;(_tmp8DE=0,Cyc_Tcutil_terr(loc,((
_tmp8DF="too many explicit witness types",_tag_dyneither(_tmp8DF,sizeof(char),32))),
_tag_dyneither(_tmp8DE,sizeof(void*),0)));}*ts=_tmp415;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp417,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp410,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple19 _tmp41D;struct Cyc_Absyn_Aggrfield*
_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;struct _tuple19*_tmp41C=(struct _tuple19*)
fields->hd;_tmp41D=*_tmp41C;_tmp41E=_tmp41D.f1;_tmp41F=_tmp41D.f2;{void*_tmp420=
Cyc_Tcutil_rsubstitute(_tmp410,_tmp416,_tmp41E->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp420,_tmp41F);if(!Cyc_Tcutil_coerce_arg(te,_tmp41F,_tmp420,& bogus)){{
const char*_tmp8EB;void*_tmp8EA[5];struct Cyc_String_pa_struct _tmp8E9;const char*
_tmp8E8;const char*_tmp8E7;struct Cyc_String_pa_struct _tmp8E6;struct Cyc_String_pa_struct
_tmp8E5;struct Cyc_String_pa_struct _tmp8E4;struct Cyc_String_pa_struct _tmp8E3;(
_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp41F->topt))->v)),((_tmp8E4.tag=0,((
_tmp8E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp420)),((_tmp8E5.tag=0,((_tmp8E5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8E6.tag=0,((_tmp8E6.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8E7="struct",
_tag_dyneither(_tmp8E7,sizeof(char),7)):((_tmp8E8="union",_tag_dyneither(_tmp8E8,
sizeof(char),6)))),((_tmp8E9.tag=0,((_tmp8E9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp41E->name),((_tmp8EA[0]=& _tmp8E9,((_tmp8EA[1]=& _tmp8E6,((
_tmp8EA[2]=& _tmp8E5,((_tmp8EA[3]=& _tmp8E4,((_tmp8EA[4]=& _tmp8E3,Cyc_Tcutil_terr(
_tmp41F->loc,((_tmp8EB="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp8EB,sizeof(char),40))),_tag_dyneither(_tmp8EA,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp42A=0;{struct Cyc_List_List*
_tmp42B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp42B != 0;_tmp42B=_tmp42B->tl){struct _tuple0*_tmp8EE;struct Cyc_List_List*
_tmp8ED;_tmp42A=((_tmp8ED=_cycalloc(sizeof(*_tmp8ED)),((_tmp8ED->hd=((_tmp8EE=
_cycalloc(sizeof(*_tmp8EE)),((_tmp8EE->f1=Cyc_Tcutil_rsubstitute(_tmp410,_tmp416,(*((
struct _tuple0*)_tmp42B->hd)).f1),((_tmp8EE->f2=Cyc_Tcutil_rsubstitute(_tmp410,
_tmp416,(*((struct _tuple0*)_tmp42B->hd)).f2),_tmp8EE)))))),((_tmp8ED->tl=_tmp42A,
_tmp8ED))))));}}_tmp42A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp42A);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp42A);return(void*)_tmp417;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp433=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp434=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp436;struct
Cyc_List_List*_tmp437;_LL1CB: {struct Cyc_Absyn_AnonAggrType_struct*_tmp435=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp434;if(_tmp435->tag != 13)goto _LL1CD;
else{_tmp436=_tmp435->f1;_tmp437=_tmp435->f2;}}_LL1CC: if(_tmp436 == Cyc_Absyn_UnionA){
const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,Cyc_Tcutil_terr(loc,((_tmp8F1="expecting struct but found union",
_tag_dyneither(_tmp8F1,sizeof(char),33))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp433,
loc,args,Cyc_Absyn_StructA,_tmp437);for(0;fields != 0;fields=fields->tl){int bogus=
0;struct _tuple19 _tmp43B;struct Cyc_Absyn_Aggrfield*_tmp43C;struct Cyc_Absyn_Exp*
_tmp43D;struct _tuple19*_tmp43A=(struct _tuple19*)fields->hd;_tmp43B=*_tmp43A;
_tmp43C=_tmp43B.f1;_tmp43D=_tmp43B.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp43C->type,_tmp43D);if(!Cyc_Tcutil_coerce_arg(te,_tmp43D,_tmp43C->type,& bogus)){{
const char*_tmp8F7;void*_tmp8F6[3];struct Cyc_String_pa_struct _tmp8F5;struct Cyc_String_pa_struct
_tmp8F4;struct Cyc_String_pa_struct _tmp8F3;(_tmp8F3.tag=0,((_tmp8F3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43D->topt))->v)),((_tmp8F4.tag=0,((_tmp8F4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp43C->type)),((
_tmp8F5.tag=0,((_tmp8F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp43C->name),((
_tmp8F6[0]=& _tmp8F5,((_tmp8F6[1]=& _tmp8F4,((_tmp8F6[2]=& _tmp8F3,Cyc_Tcutil_terr(
_tmp43D->loc,((_tmp8F7="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp8F7,sizeof(char),41))),_tag_dyneither(_tmp8F6,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CA;};_LL1CD:;_LL1CE: {const char*_tmp8FA;
void*_tmp8F9;(_tmp8F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp8FA="tcAnonStruct: wrong type",_tag_dyneither(_tmp8FA,
sizeof(char),25))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}_LL1CA:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp445=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp8FB;struct _tuple11 _tmp446=(_tmp8FB.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp8FB.f2=_tmp445,_tmp8FB)));struct Cyc_List_List*_tmp447=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp445,Cyc_Tcutil_r_make_inst_var,&
_tmp446,tud->tvs);struct Cyc_List_List*_tmp448=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,_tmp447);struct Cyc_Absyn_DatatypeFieldType_struct _tmp901;struct Cyc_Absyn_DatatypeFieldInfo
_tmp900;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp8FF;void*res=(void*)((
_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp901.tag=4,((_tmp901.f1=((
_tmp900.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp900.targs=_tmp448,
_tmp900)))),_tmp901)))),_tmp8FF))));if(topt != 0){void*_tmp449=Cyc_Tcutil_compress(*
topt);_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp44A=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp449;if(_tmp44A->tag != 4)goto _LL1D2;}_LL1D1: Cyc_Tcutil_unify(*topt,res);goto
_LL1CF;_LL1D2:;_LL1D3: goto _LL1CF;_LL1CF:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp445,_tmp447,(*((
struct _tuple18*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp909;void*_tmp908[3];struct Cyc_String_pa_struct
_tmp907;struct Cyc_String_pa_struct _tmp906;const char*_tmp905;struct Cyc_String_pa_struct
_tmp904;(_tmp904.tag=0,((_tmp904.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp905="?",_tag_dyneither(_tmp905,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp906.tag=0,((_tmp906.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp907.tag=0,((_tmp907.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp908[0]=& _tmp907,((_tmp908[1]=& _tmp906,((
_tmp908[2]=& _tmp904,Cyc_Tcutil_terr(e->loc,((_tmp909="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp909,sizeof(char),82))),_tag_dyneither(_tmp908,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp90D;void*_tmp90C[1];
struct Cyc_String_pa_struct _tmp90B;return(_tmp90B.tag=0,((_tmp90B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp90C[0]=& _tmp90B,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp90D="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp90D,sizeof(char),47))),_tag_dyneither(_tmp90C,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp911;void*_tmp910[1];struct Cyc_String_pa_struct _tmp90F;return(
_tmp90F.tag=0,((_tmp90F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp910[0]=& _tmp90F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp911="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp911,sizeof(char),46))),_tag_dyneither(_tmp910,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp45B=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp462;struct Cyc_Absyn_PtrAtts _tmp463;union Cyc_Absyn_Constraint*
_tmp464;struct Cyc_Absyn_ArrayInfo _tmp469;void*_tmp46A;struct Cyc_List_List*
_tmp46D;struct Cyc_Absyn_AggrInfo _tmp46F;union Cyc_Absyn_AggrInfoU _tmp470;struct
Cyc_List_List*_tmp471;struct Cyc_List_List*_tmp475;_LL1D5: {struct Cyc_Absyn_VoidType_struct*
_tmp45C=(struct Cyc_Absyn_VoidType_struct*)_tmp45B;if(_tmp45C->tag != 0)goto _LL1D7;}
_LL1D6: return 1;_LL1D7: {struct Cyc_Absyn_Evar_struct*_tmp45D=(struct Cyc_Absyn_Evar_struct*)
_tmp45B;if(_tmp45D->tag != 1)goto _LL1D9;}_LL1D8: goto _LL1DA;_LL1D9: {struct Cyc_Absyn_VarType_struct*
_tmp45E=(struct Cyc_Absyn_VarType_struct*)_tmp45B;if(_tmp45E->tag != 2)goto _LL1DB;}
_LL1DA: goto _LL1DC;_LL1DB: {struct Cyc_Absyn_DatatypeType_struct*_tmp45F=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp45B;if(_tmp45F->tag != 3)goto _LL1DD;}_LL1DC:
goto _LL1DE;_LL1DD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp460=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp45B;if(_tmp460->tag != 4)goto _LL1DF;}_LL1DE: return 0;_LL1DF: {struct Cyc_Absyn_PointerType_struct*
_tmp461=(struct Cyc_Absyn_PointerType_struct*)_tmp45B;if(_tmp461->tag != 5)goto
_LL1E1;else{_tmp462=_tmp461->f1;_tmp463=_tmp462.ptr_atts;_tmp464=_tmp463.nullable;}}
_LL1E0: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp464);_LL1E1: {struct Cyc_Absyn_IntType_struct*_tmp465=(struct Cyc_Absyn_IntType_struct*)
_tmp45B;if(_tmp465->tag != 6)goto _LL1E3;}_LL1E2: goto _LL1E4;_LL1E3: {struct Cyc_Absyn_FloatType_struct*
_tmp466=(struct Cyc_Absyn_FloatType_struct*)_tmp45B;if(_tmp466->tag != 7)goto
_LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_Absyn_DoubleType_struct*_tmp467=(
struct Cyc_Absyn_DoubleType_struct*)_tmp45B;if(_tmp467->tag != 8)goto _LL1E7;}
_LL1E6: return 1;_LL1E7: {struct Cyc_Absyn_ArrayType_struct*_tmp468=(struct Cyc_Absyn_ArrayType_struct*)
_tmp45B;if(_tmp468->tag != 9)goto _LL1E9;else{_tmp469=_tmp468->f1;_tmp46A=_tmp469.elt_type;}}
_LL1E8: return Cyc_Tcexp_zeroable_type(_tmp46A);_LL1E9: {struct Cyc_Absyn_FnType_struct*
_tmp46B=(struct Cyc_Absyn_FnType_struct*)_tmp45B;if(_tmp46B->tag != 10)goto _LL1EB;}
_LL1EA: return 0;_LL1EB: {struct Cyc_Absyn_TupleType_struct*_tmp46C=(struct Cyc_Absyn_TupleType_struct*)
_tmp45B;if(_tmp46C->tag != 11)goto _LL1ED;else{_tmp46D=_tmp46C->f1;}}_LL1EC: for(0;(
unsigned int)_tmp46D;_tmp46D=_tmp46D->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple18*)_tmp46D->hd)).f2))return 0;}return 1;_LL1ED: {struct Cyc_Absyn_AggrType_struct*
_tmp46E=(struct Cyc_Absyn_AggrType_struct*)_tmp45B;if(_tmp46E->tag != 12)goto
_LL1EF;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46F.aggr_info;_tmp471=_tmp46F.targs;}}
_LL1EE: {struct Cyc_Absyn_Aggrdecl*_tmp481=Cyc_Absyn_get_known_aggrdecl(_tmp470);
if(_tmp481->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp481->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp482=_new_region("r");struct _RegionHandle*r=&
_tmp482;_push_region(r);{struct Cyc_List_List*_tmp483=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp481->tvs,_tmp471);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp481->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp483,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp484=0;_npop_handler(0);return _tmp484;}}}{int _tmp485=1;_npop_handler(0);
return _tmp485;};};_pop_region(r);};}_LL1EF: {struct Cyc_Absyn_EnumType_struct*
_tmp472=(struct Cyc_Absyn_EnumType_struct*)_tmp45B;if(_tmp472->tag != 14)goto
_LL1F1;}_LL1F0: return 1;_LL1F1: {struct Cyc_Absyn_TagType_struct*_tmp473=(struct
Cyc_Absyn_TagType_struct*)_tmp45B;if(_tmp473->tag != 20)goto _LL1F3;}_LL1F2: return
1;_LL1F3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp474=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp45B;if(_tmp474->tag != 13)goto _LL1F5;else{_tmp475=_tmp474->f2;}}_LL1F4: for(0;
_tmp475 != 0;_tmp475=_tmp475->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp475->hd)->type))return 0;}return 1;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp476=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp45B;if(_tmp476->tag != 15)goto
_LL1F7;}_LL1F6: return 1;_LL1F7: {struct Cyc_Absyn_TypedefType_struct*_tmp477=(
struct Cyc_Absyn_TypedefType_struct*)_tmp45B;if(_tmp477->tag != 18)goto _LL1F9;}
_LL1F8: return 0;_LL1F9: {struct Cyc_Absyn_DynRgnType_struct*_tmp478=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp45B;if(_tmp478->tag != 17)goto _LL1FB;}_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp479=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45B;if(_tmp479->tag != 16)goto
_LL1FD;}_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_TypeDeclType_struct*_tmp47A=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp45B;if(_tmp47A->tag != 26)goto _LL1FF;}
_LL1FE: goto _LL200;_LL1FF: {struct Cyc_Absyn_ValueofType_struct*_tmp47B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp45B;if(_tmp47B->tag != 19)goto _LL201;}_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_HeapRgn_struct*
_tmp47C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp45B;if(_tmp47C->tag != 21)goto _LL203;}
_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_UniqueRgn_struct*_tmp47D=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp45B;if(_tmp47D->tag != 22)goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_AccessEff_struct*
_tmp47E=(struct Cyc_Absyn_AccessEff_struct*)_tmp45B;if(_tmp47E->tag != 23)goto
_LL207;}_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_JoinEff_struct*_tmp47F=(
struct Cyc_Absyn_JoinEff_struct*)_tmp45B;if(_tmp47F->tag != 24)goto _LL209;}_LL208:
goto _LL20A;_LL209: {struct Cyc_Absyn_RgnsEff_struct*_tmp480=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp45B;if(_tmp480->tag != 25)goto _LL1D4;}_LL20A: {const char*_tmp915;void*_tmp914[
1];struct Cyc_String_pa_struct _tmp913;(_tmp913.tag=0,((_tmp913.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp914[0]=&
_tmp913,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp915="bad type `%s' in zeroable type",_tag_dyneither(_tmp915,sizeof(char),31))),
_tag_dyneither(_tmp914,sizeof(void*),1)))))));}_LL1D4:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp489=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp48B;void*_tmp48C;_LL20C: {
struct Cyc_Absyn_PointerType_struct*_tmp48A=(struct Cyc_Absyn_PointerType_struct*)
_tmp489;if(_tmp48A->tag != 5)goto _LL20E;else{_tmp48B=_tmp48A->f1;_tmp48C=_tmp48B.elt_typ;}}
_LL20D: Cyc_Tcutil_unify(_tmp48C,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL20B;_LL20E:;_LL20F: goto _LL20B;_LL20B:;}{
const char*_tmp91E;void*_tmp91D[2];const char*_tmp91C;const char*_tmp91B;struct Cyc_String_pa_struct
_tmp91A;struct Cyc_String_pa_struct _tmp919;(_tmp919.tag=0,((_tmp919.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp91A.tag=0,((
_tmp91A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp91B="calloc",_tag_dyneither(_tmp91B,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp91C="malloc",_tag_dyneither(_tmp91C,sizeof(char),7))))),((
_tmp91D[0]=& _tmp91A,((_tmp91D[1]=& _tmp919,Cyc_Tcutil_terr(loc,((_tmp91E="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp91E,sizeof(char),60))),_tag_dyneither(_tmp91D,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL210: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL211: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp921;
struct Cyc_Absyn_RgnHandleType_struct*_tmp920;void*expected_type=(void*)((_tmp920=
_cycalloc(sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=16,((_tmp921.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp921)))),_tmp920))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));void*_tmp493=Cyc_Tcutil_compress(
handle_type);void*_tmp495;_LL214: {struct Cyc_Absyn_RgnHandleType_struct*_tmp494=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp493;if(_tmp494->tag != 16)goto _LL216;
else{_tmp495=(void*)_tmp494->f1;}}_LL215: rgn=_tmp495;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL213;_LL216:;_LL217:{const char*_tmp925;void*_tmp924[1];struct
Cyc_String_pa_struct _tmp923;(_tmp923.tag=0,((_tmp923.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp924[0]=& _tmp923,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp925="expecting region_t type but found %s",
_tag_dyneither(_tmp925,sizeof(char),37))),_tag_dyneither(_tmp924,sizeof(void*),1)))))));}
goto _LL213;_LL213:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn != (void*)& Cyc_Absyn_HeapRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp928;void*_tmp927;(
_tmp927=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp928="calloc with empty type",_tag_dyneither(_tmp928,sizeof(char),23))),
_tag_dyneither(_tmp927,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;one_elt=0;}
else{void*_tmp49D=(*e)->r;void*_tmp49F;enum Cyc_Absyn_Primop _tmp4A1;struct Cyc_List_List*
_tmp4A2;struct Cyc_List_List _tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_List_List*
_tmp4A5;struct Cyc_List_List _tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*
_tmp4A8;_LL219: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp49E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp49D;if(_tmp49E->tag != 18)goto _LL21B;else{_tmp49F=(void*)_tmp49E->f1;}}_LL21A:
elt_type=_tmp49F;{void**_tmp929;*t=(void**)((_tmp929=_cycalloc(sizeof(*_tmp929)),((
_tmp929[0]=elt_type,_tmp929))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL218;_LL21B: {struct Cyc_Absyn_Primop_e_struct*
_tmp4A0=(struct Cyc_Absyn_Primop_e_struct*)_tmp49D;if(_tmp4A0->tag != 3)goto _LL21D;
else{_tmp4A1=_tmp4A0->f1;if(_tmp4A1 != Cyc_Absyn_Times)goto _LL21D;_tmp4A2=_tmp4A0->f2;
if(_tmp4A2 == 0)goto _LL21D;_tmp4A3=*_tmp4A2;_tmp4A4=(struct Cyc_Absyn_Exp*)_tmp4A3.hd;
_tmp4A5=_tmp4A3.tl;if(_tmp4A5 == 0)goto _LL21D;_tmp4A6=*_tmp4A5;_tmp4A7=(struct Cyc_Absyn_Exp*)
_tmp4A6.hd;_tmp4A8=_tmp4A6.tl;if(_tmp4A8 != 0)goto _LL21D;}}_LL21C:{struct _tuple0
_tmp92A;struct _tuple0 _tmp4AB=(_tmp92A.f1=_tmp4A4->r,((_tmp92A.f2=_tmp4A7->r,
_tmp92A)));void*_tmp4AC;void*_tmp4AE;void*_tmp4AF;void*_tmp4B1;_LL220: _tmp4AC=
_tmp4AB.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4AD=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AC;if(_tmp4AD->tag != 18)goto _LL222;else{_tmp4AE=(void*)_tmp4AD->f1;}};
_LL221: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4AE);elt_type=_tmp4AE;{void**
_tmp92B;*t=(void**)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=elt_type,
_tmp92B))));}num_elts=_tmp4A7;one_elt=0;goto _LL21F;_LL222: _tmp4AF=_tmp4AB.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4B0=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AF;if(_tmp4B0->tag != 18)goto _LL224;else{_tmp4B1=(void*)_tmp4B0->f1;}};
_LL223: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4B1);elt_type=_tmp4B1;{void**
_tmp92C;*t=(void**)((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C[0]=elt_type,
_tmp92C))));}num_elts=_tmp4A4;one_elt=0;goto _LL21F;_LL224:;_LL225: goto No_sizeof;
_LL21F:;}goto _LL218;_LL21D:;_LL21E: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**
_tmp92D;*t=(void**)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=elt_type,
_tmp92D))));}num_elts=*e;one_elt=0;goto _LL218;_LL218:;}*e=num_elts;*is_fat=!
one_elt;{void*_tmp4B5=elt_type;struct Cyc_Absyn_AggrInfo _tmp4B7;union Cyc_Absyn_AggrInfoU
_tmp4B8;struct Cyc_Absyn_Aggrdecl**_tmp4B9;struct Cyc_Absyn_Aggrdecl*_tmp4BA;
_LL227: {struct Cyc_Absyn_AggrType_struct*_tmp4B6=(struct Cyc_Absyn_AggrType_struct*)
_tmp4B5;if(_tmp4B6->tag != 12)goto _LL229;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B7.aggr_info;
if((_tmp4B8.KnownAggr).tag != 2)goto _LL229;_tmp4B9=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4B8.KnownAggr).val;_tmp4BA=*_tmp4B9;}}_LL228: if(_tmp4BA->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BA->impl))->exist_vars != 0){const
char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_terr(loc,((_tmp930="malloc with existential types not yet implemented",
_tag_dyneither(_tmp930,sizeof(char),50))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}
goto _LL226;_LL229:;_LL22A: goto _LL226;_LL226:;}{void*(*_tmp4BD)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4BE=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp4BF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4C1;struct Cyc_Absyn_PtrAtts
_tmp4C2;union Cyc_Absyn_Constraint*_tmp4C3;union Cyc_Absyn_Constraint*_tmp4C4;
union Cyc_Absyn_Constraint*_tmp4C5;_LL22C: {struct Cyc_Absyn_PointerType_struct*
_tmp4C0=(struct Cyc_Absyn_PointerType_struct*)_tmp4BF;if(_tmp4C0->tag != 5)goto
_LL22E;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C1.ptr_atts;_tmp4C3=_tmp4C2.nullable;
_tmp4C4=_tmp4C2.bounds;_tmp4C5=_tmp4C2.zero_term;}}_LL22D: _tmp4BE=_tmp4C5;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4C3))
_tmp4BD=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp4C5) && !(*is_calloc)){{const char*_tmp933;void*_tmp932;(_tmp932=0,Cyc_Tcutil_warn(
loc,((_tmp933="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp933,sizeof(char),55))),_tag_dyneither(_tmp932,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp4C8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4C4);struct Cyc_Absyn_Exp*_tmp4CB;_LL231: {struct Cyc_Absyn_DynEither_b_struct*
_tmp4C9=(struct Cyc_Absyn_DynEither_b_struct*)_tmp4C8;if(_tmp4C9->tag != 0)goto
_LL233;}_LL232: goto _LL230;_LL233:{struct Cyc_Absyn_Upper_b_struct*_tmp4CA=(struct
Cyc_Absyn_Upper_b_struct*)_tmp4C8;if(_tmp4CA->tag != 1)goto _LL235;else{_tmp4CB=
_tmp4CA->f1;}}if(!(!one_elt))goto _LL235;_LL234: {int _tmp4CC=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4CC  && Cyc_Evexp_same_const_exp(_tmp4CB,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4C4),_tmp4BE);}{void*_tmp4CD=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4CF;_LL238: {struct Cyc_Absyn_TagType_struct*_tmp4CE=(struct Cyc_Absyn_TagType_struct*)
_tmp4CD;if(_tmp4CE->tag != 20)goto _LL23A;else{_tmp4CF=(void*)_tmp4CE->f1;}}_LL239: {
struct Cyc_Absyn_Exp*_tmp4D0=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4CF,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4D0,
_tmp4CB)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4C4),_tmp4BE);}
goto _LL237;}_LL23A:;_LL23B: goto _LL237;_LL237:;}goto _LL230;}_LL235:;_LL236: goto
_LL230;_LL230:;}goto _LL22B;_LL22E:;_LL22F: goto _LL22B;_LL22B:;}if(!one_elt)
_tmp4BD=Cyc_Absyn_dyneither_typ;return _tmp4BD(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp4BE);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4D1=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4D2=Cyc_Tcenv_enter_notreadctxt(
_tmp4D1,te);Cyc_Tcexp_tcExp(_tmp4D2,0,e1);Cyc_Tcexp_tcExp(_tmp4D2,(void**)((void**)((
void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp4D3=Cyc_Tcutil_compress(t1);_LL23D: {struct
Cyc_Absyn_ArrayType_struct*_tmp4D4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4D3;
if(_tmp4D4->tag != 9)goto _LL23F;}_LL23E:{const char*_tmp936;void*_tmp935;(_tmp935=
0,Cyc_Tcutil_terr(loc,((_tmp936="cannot assign to an array",_tag_dyneither(
_tmp936,sizeof(char),26))),_tag_dyneither(_tmp935,sizeof(void*),0)));}goto _LL23C;
_LL23F:;_LL240: goto _LL23C;_LL23C:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp939;void*_tmp938;(_tmp938=0,Cyc_Tcutil_terr(loc,((
_tmp939="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp939,sizeof(char),58))),_tag_dyneither(_tmp938,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp93C;void*_tmp93B;return(_tmp93B=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp93C="swap non-lvalue",_tag_dyneither(_tmp93C,sizeof(char),
16))),_tag_dyneither(_tmp93B,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp93F;void*_tmp93E;return(_tmp93E=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp93F="swap non-lvalue",_tag_dyneither(_tmp93F,sizeof(char),16))),
_tag_dyneither(_tmp93E,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp942;void*_tmp941;return(_tmp941=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp942="swap value in zeroterm array",_tag_dyneither(_tmp942,
sizeof(char),29))),_tag_dyneither(_tmp941,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp945;void*_tmp944;return(_tmp944=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp945="swap value in zeroterm array",_tag_dyneither(_tmp945,
sizeof(char),29))),_tag_dyneither(_tmp944,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp94A;void*_tmp949[2];struct Cyc_String_pa_struct _tmp948;struct Cyc_String_pa_struct
_tmp947;void*_tmp4E1=(_tmp947.tag=0,((_tmp947.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp948.tag=0,((_tmp948.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp949[0]=&
_tmp948,((_tmp949[1]=& _tmp947,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp94A="type mismatch: %s != %s",
_tag_dyneither(_tmp94A,sizeof(char),24))),_tag_dyneither(_tmp949,sizeof(void*),2)))))))))))));
return _tmp4E1;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4E6=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4E6,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4E7=s->r;struct Cyc_Absyn_Exp*
_tmp4E9;struct Cyc_Absyn_Stmt*_tmp4EB;struct Cyc_Absyn_Stmt*_tmp4EC;struct Cyc_Absyn_Decl*
_tmp4EE;struct Cyc_Absyn_Stmt*_tmp4EF;_LL242: {struct Cyc_Absyn_Exp_s_struct*
_tmp4E8=(struct Cyc_Absyn_Exp_s_struct*)_tmp4E7;if(_tmp4E8->tag != 1)goto _LL244;
else{_tmp4E9=_tmp4E8->f1;}}_LL243: {void*_tmp4F0=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E9->topt))->v;if(!Cyc_Tcutil_unify(_tmp4F0,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp94E;void*_tmp94D[1];struct
Cyc_String_pa_struct _tmp94C;(_tmp94C.tag=0,((_tmp94C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F0)),((_tmp94D[0]=& _tmp94C,Cyc_Tcutil_terr(
loc,((_tmp94E="statement expression returns type %s",_tag_dyneither(_tmp94E,
sizeof(char),37))),_tag_dyneither(_tmp94D,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp4F0;}_LL244: {struct Cyc_Absyn_Seq_s_struct*_tmp4EA=(struct Cyc_Absyn_Seq_s_struct*)
_tmp4E7;if(_tmp4EA->tag != 2)goto _LL246;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;}}
_LL245: s=_tmp4EC;continue;_LL246: {struct Cyc_Absyn_Decl_s_struct*_tmp4ED=(struct
Cyc_Absyn_Decl_s_struct*)_tmp4E7;if(_tmp4ED->tag != 12)goto _LL248;else{_tmp4EE=
_tmp4ED->f1;_tmp4EF=_tmp4ED->f2;}}_LL247: s=_tmp4EF;continue;_LL248:;_LL249: {
const char*_tmp951;void*_tmp950;return(_tmp950=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp951="statement expression must end with expression",_tag_dyneither(_tmp951,
sizeof(char),46))),_tag_dyneither(_tmp950,sizeof(void*),0)));}_LL241:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp4F6=t;struct Cyc_Absyn_AggrInfo _tmp4F8;union
Cyc_Absyn_AggrInfoU _tmp4F9;struct Cyc_Absyn_Aggrdecl**_tmp4FA;struct Cyc_Absyn_Aggrdecl*
_tmp4FB;_LL24B: {struct Cyc_Absyn_AggrType_struct*_tmp4F7=(struct Cyc_Absyn_AggrType_struct*)
_tmp4F6;if(_tmp4F7->tag != 12)goto _LL24D;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F8.aggr_info;
if((_tmp4F9.KnownAggr).tag != 2)goto _LL24D;_tmp4FA=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4F9.KnownAggr).val;_tmp4FB=*_tmp4FA;}}_LL24C: if((_tmp4FB->kind == Cyc_Absyn_UnionA
 && _tmp4FB->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FB->impl))->tagged)
goto _LL24A;goto _LL24E;_LL24D:;_LL24E:{const char*_tmp955;void*_tmp954[1];struct
Cyc_String_pa_struct _tmp953;(_tmp953.tag=0,((_tmp953.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp954[0]=& _tmp953,Cyc_Tcutil_terr(
loc,((_tmp955="expecting @tagged union but found %s",_tag_dyneither(_tmp955,
sizeof(char),37))),_tag_dyneither(_tmp954,sizeof(void*),1)))))));}goto _LL24A;
_LL24A:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp958;struct Cyc_Absyn_RgnHandleType_struct*
_tmp957;void*expected_type=(void*)((_tmp957=_cycalloc(sizeof(*_tmp957)),((
_tmp957[0]=((_tmp958.tag=16,((_tmp958.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp958)))),_tmp957))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle));void*_tmp4FF=Cyc_Tcutil_compress(handle_type);void*
_tmp501;_LL250: {struct Cyc_Absyn_RgnHandleType_struct*_tmp500=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4FF;if(_tmp500->tag != 16)goto _LL252;else{_tmp501=(void*)_tmp500->f1;}}_LL251:
rgn=_tmp501;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL24F;_LL252:;_LL253:{
const char*_tmp95C;void*_tmp95B[1];struct Cyc_String_pa_struct _tmp95A;(_tmp95A.tag=
0,((_tmp95A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp95B[0]=& _tmp95A,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle))->loc,((_tmp95C="expecting region_t type but found %s",
_tag_dyneither(_tmp95C,sizeof(char),37))),_tag_dyneither(_tmp95B,sizeof(void*),1)))))));}
goto _LL24F;_LL24F:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn != (void*)& Cyc_Absyn_HeapRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp507=e1->r;struct Cyc_Core_Opt*_tmp50A;struct Cyc_List_List*_tmp50B;struct
Cyc_List_List*_tmp50D;union Cyc_Absyn_Cnst _tmp50F;struct _dyneither_ptr _tmp510;
_LL255: {struct Cyc_Absyn_Comprehension_e_struct*_tmp508=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp507;if(_tmp508->tag != 28)goto _LL257;}_LL256: {void*_tmp511=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp512=Cyc_Tcutil_compress(_tmp511);struct Cyc_Absyn_ArrayInfo
_tmp514;void*_tmp515;struct Cyc_Absyn_Tqual _tmp516;struct Cyc_Absyn_Exp*_tmp517;
union Cyc_Absyn_Constraint*_tmp518;_LL260: {struct Cyc_Absyn_ArrayType_struct*
_tmp513=(struct Cyc_Absyn_ArrayType_struct*)_tmp512;if(_tmp513->tag != 9)goto
_LL262;else{_tmp514=_tmp513->f1;_tmp515=_tmp514.elt_type;_tmp516=_tmp514.tq;
_tmp517=_tmp514.num_elts;_tmp518=_tmp514.zero_term;}}_LL261: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp517);void*b;{void*_tmp519=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp51B;_LL265: {
struct Cyc_Absyn_TagType_struct*_tmp51A=(struct Cyc_Absyn_TagType_struct*)_tmp519;
if(_tmp51A->tag != 20)goto _LL267;else{_tmp51B=(void*)_tmp51A->f1;}}_LL266:{struct
Cyc_Absyn_Upper_b_struct _tmp95F;struct Cyc_Absyn_Upper_b_struct*_tmp95E;b=(void*)((
_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95F.tag=1,((_tmp95F.f1=
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp51B,0),0,Cyc_Absyn_No_coercion,
0),_tmp95F)))),_tmp95E))));}goto _LL264;_LL267:;_LL268: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp962;struct Cyc_Absyn_Upper_b_struct*
_tmp961;b=(void*)((_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp962.tag=
1,((_tmp962.f1=bnd,_tmp962)))),_tmp961))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}
_LL264:;}{struct Cyc_Absyn_PointerType_struct _tmp96C;struct Cyc_Absyn_PtrAtts
_tmp96B;struct Cyc_Absyn_PtrInfo _tmp96A;struct Cyc_Absyn_PointerType_struct*
_tmp969;void*res_typ=(void*)((_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969[0]=((
_tmp96C.tag=5,((_tmp96C.f1=((_tmp96A.elt_typ=_tmp515,((_tmp96A.elt_tq=_tmp516,((
_tmp96A.ptr_atts=((_tmp96B.rgn=rgn,((_tmp96B.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp96B.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp96B.zero_term=_tmp518,((_tmp96B.ptrloc=0,
_tmp96B)))))))))),_tmp96A)))))),_tmp96C)))),_tmp969))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp96D;e->topt=((_tmp96D=_cycalloc(sizeof(*_tmp96D)),((_tmp96D->v=res_typ,
_tmp96D))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL262:;_LL263: {const char*_tmp970;void*_tmp96F;(
_tmp96F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp970="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp970,
sizeof(char),45))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}_LL25F:;}_LL257: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp509=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp507;if(_tmp509->tag != 36)goto _LL259;else{_tmp50A=_tmp509->f1;_tmp50B=_tmp509->f2;}}
_LL258:{struct Cyc_Absyn_Array_e_struct _tmp973;struct Cyc_Absyn_Array_e_struct*
_tmp972;e1->r=(void*)((_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972[0]=((
_tmp973.tag=27,((_tmp973.f1=_tmp50B,_tmp973)))),_tmp972))));}_tmp50D=_tmp50B;
goto _LL25A;_LL259: {struct Cyc_Absyn_Array_e_struct*_tmp50C=(struct Cyc_Absyn_Array_e_struct*)
_tmp507;if(_tmp50C->tag != 27)goto _LL25B;else{_tmp50D=_tmp50C->f1;}}_LL25A: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp529=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp52B;void*_tmp52C;void**_tmp52D;struct Cyc_Absyn_Tqual
_tmp52E;struct Cyc_Absyn_PtrAtts _tmp52F;union Cyc_Absyn_Constraint*_tmp530;_LL26A: {
struct Cyc_Absyn_PointerType_struct*_tmp52A=(struct Cyc_Absyn_PointerType_struct*)
_tmp529;if(_tmp52A->tag != 5)goto _LL26C;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52B.elt_typ;
_tmp52D=(void**)&(_tmp52A->f1).elt_typ;_tmp52E=_tmp52B.elt_tq;_tmp52F=_tmp52B.ptr_atts;
_tmp530=_tmp52F.zero_term;}}_LL26B: elt_typ_opt=(void**)_tmp52D;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp530);goto _LL269;
_LL26C:;_LL26D: goto _LL269;_LL269:;}{void*_tmp531=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp50D);{struct Cyc_Core_Opt*_tmp974;e1->topt=((_tmp974=
_cycalloc(sizeof(*_tmp974)),((_tmp974->v=_tmp531,_tmp974))));}{void*res_typ;{
void*_tmp533=Cyc_Tcutil_compress(_tmp531);struct Cyc_Absyn_ArrayInfo _tmp535;void*
_tmp536;struct Cyc_Absyn_Tqual _tmp537;struct Cyc_Absyn_Exp*_tmp538;union Cyc_Absyn_Constraint*
_tmp539;_LL26F: {struct Cyc_Absyn_ArrayType_struct*_tmp534=(struct Cyc_Absyn_ArrayType_struct*)
_tmp533;if(_tmp534->tag != 9)goto _LL271;else{_tmp535=_tmp534->f1;_tmp536=_tmp535.elt_type;
_tmp537=_tmp535.tq;_tmp538=_tmp535.num_elts;_tmp539=_tmp535.zero_term;}}_LL270:{
struct Cyc_Absyn_PointerType_struct _tmp989;struct Cyc_Absyn_PtrAtts _tmp988;struct
Cyc_Absyn_Upper_b_struct _tmp987;struct Cyc_Absyn_Upper_b_struct*_tmp986;struct Cyc_Absyn_PtrInfo
_tmp985;struct Cyc_Absyn_PointerType_struct*_tmp984;res_typ=(void*)((_tmp984=
_cycalloc(sizeof(*_tmp984)),((_tmp984[0]=((_tmp989.tag=5,((_tmp989.f1=((_tmp985.elt_typ=
_tmp536,((_tmp985.elt_tq=_tmp537,((_tmp985.ptr_atts=((_tmp988.rgn=rgn,((_tmp988.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp988.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp986=
_cycalloc(sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=1,((_tmp987.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp538),_tmp987)))),_tmp986))))),((_tmp988.zero_term=_tmp539,((
_tmp988.ptrloc=0,_tmp988)))))))))),_tmp985)))))),_tmp989)))),_tmp984))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp98A;e->topt=((_tmp98A=_cycalloc(
sizeof(*_tmp98A)),((_tmp98A->v=res_typ,_tmp98A))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL26E;_LL271:;_LL272: {
const char*_tmp98D;void*_tmp98C;(_tmp98C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp98D="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp98D,sizeof(char),50))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
_LL26E:;}return res_typ;};};}_LL25B: {struct Cyc_Absyn_Const_e_struct*_tmp50E=(
struct Cyc_Absyn_Const_e_struct*)_tmp507;if(_tmp50E->tag != 0)goto _LL25D;else{
_tmp50F=_tmp50E->f1;if((_tmp50F.String_c).tag != 7)goto _LL25D;_tmp510=(struct
_dyneither_ptr)(_tmp50F.String_c).val;}}_LL25C: {void*_tmp543=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp544=Cyc_Tcexp_tcExp(te,(void**)& _tmp543,e1);
struct Cyc_Absyn_Upper_b_struct _tmp990;struct Cyc_Absyn_Upper_b_struct*_tmp98F;
return Cyc_Absyn_atb_typ(_tmp544,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp98F=
_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp990.tag=1,((_tmp990.f1=Cyc_Absyn_uint_exp(
1,0),_tmp990)))),_tmp98F)))),Cyc_Absyn_false_conref);}_LL25D:;_LL25E: {void**
topt2=0;if(topt != 0){void*_tmp547=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp549;void*_tmp54A;void**_tmp54B;struct Cyc_Absyn_Tqual _tmp54C;_LL274: {struct
Cyc_Absyn_PointerType_struct*_tmp548=(struct Cyc_Absyn_PointerType_struct*)
_tmp547;if(_tmp548->tag != 5)goto _LL276;else{_tmp549=_tmp548->f1;_tmp54A=_tmp549.elt_typ;
_tmp54B=(void**)&(_tmp548->f1).elt_typ;_tmp54C=_tmp549.elt_tq;}}_LL275: topt2=(
void**)_tmp54B;goto _LL273;_LL276: {struct Cyc_Absyn_DatatypeType_struct*_tmp54D=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp547;if(_tmp54D->tag != 3)goto _LL278;}
_LL277:{void**_tmp991;topt2=((_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991[0]=*
topt,_tmp991))));}goto _LL273;_LL278:;_LL279: goto _LL273;_LL273:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp99B;struct Cyc_Absyn_PtrAtts
_tmp99A;struct Cyc_Absyn_PtrInfo _tmp999;struct Cyc_Absyn_PointerType_struct*
_tmp998;void*res_typ=(void*)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=((
_tmp99B.tag=5,((_tmp99B.f1=((_tmp999.elt_typ=telt,((_tmp999.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp999.ptr_atts=((_tmp99A.rgn=rgn,((_tmp99A.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp99A.bounds=Cyc_Absyn_bounds_one_conref,((_tmp99A.zero_term=
Cyc_Absyn_false_conref,((_tmp99A.ptrloc=0,_tmp99A)))))))))),_tmp999)))))),
_tmp99B)))),_tmp998))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp99C;e->topt=((_tmp99C=_cycalloc(
sizeof(*_tmp99C)),((_tmp99C->v=res_typ,_tmp99C))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL254:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp554=t;
struct Cyc_Absyn_ArrayInfo _tmp556;void*_tmp557;struct Cyc_Absyn_Tqual _tmp558;
struct Cyc_Absyn_Exp*_tmp559;union Cyc_Absyn_Constraint*_tmp55A;_LL27B: {struct Cyc_Absyn_ArrayType_struct*
_tmp555=(struct Cyc_Absyn_ArrayType_struct*)_tmp554;if(_tmp555->tag != 9)goto
_LL27D;else{_tmp556=_tmp555->f1;_tmp557=_tmp556.elt_type;_tmp558=_tmp556.tq;
_tmp559=_tmp556.num_elts;_tmp55A=_tmp556.zero_term;}}_LL27C: {void*_tmp55C;
struct _tuple13 _tmp55B=Cyc_Tcutil_addressof_props(te,e);_tmp55C=_tmp55B.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp99F;struct Cyc_Absyn_Upper_b_struct*_tmp99E;
void*_tmp55D=_tmp559 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp99E=
_cycalloc(sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=1,((_tmp99F.f1=(struct Cyc_Absyn_Exp*)
_tmp559,_tmp99F)))),_tmp99E))));t=Cyc_Absyn_atb_typ(_tmp557,_tmp55C,_tmp558,
_tmp55D,_tmp55A);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL27D:;_LL27E: return t;_LL27A:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp560=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp560,t) && !Cyc_Tcutil_is_noalias_path(_tmp560,e)){const char*_tmp9A2;void*
_tmp9A1;(_tmp9A1=0,Cyc_Tcutil_terr(e->loc,((_tmp9A2="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp9A2,sizeof(char),49))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));}{
void*_tmp563=e->r;union Cyc_Absyn_Cnst _tmp567;struct _dyneither_ptr _tmp568;_LL280: {
struct Cyc_Absyn_Array_e_struct*_tmp564=(struct Cyc_Absyn_Array_e_struct*)_tmp563;
if(_tmp564->tag != 27)goto _LL282;}_LL281: goto _LL283;_LL282: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp565=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp563;if(_tmp565->tag != 28)
goto _LL284;}_LL283: goto _LL285;_LL284: {struct Cyc_Absyn_Const_e_struct*_tmp566=(
struct Cyc_Absyn_Const_e_struct*)_tmp563;if(_tmp566->tag != 0)goto _LL286;else{
_tmp567=_tmp566->f1;if((_tmp567.String_c).tag != 7)goto _LL286;_tmp568=(struct
_dyneither_ptr)(_tmp567.String_c).val;}}_LL285: return t;_LL286:;_LL287: t=Cyc_Tcutil_compress(
t);{void*_tmp569=t;struct Cyc_Absyn_ArrayInfo _tmp56B;void*_tmp56C;struct Cyc_Absyn_Tqual
_tmp56D;struct Cyc_Absyn_Exp*_tmp56E;union Cyc_Absyn_Constraint*_tmp56F;_LL289: {
struct Cyc_Absyn_ArrayType_struct*_tmp56A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp569;if(_tmp56A->tag != 9)goto _LL28B;else{_tmp56B=_tmp56A->f1;_tmp56C=_tmp56B.elt_type;
_tmp56D=_tmp56B.tq;_tmp56E=_tmp56B.num_elts;_tmp56F=_tmp56B.zero_term;}}_LL28A: {
void*_tmp571;struct _tuple13 _tmp570=Cyc_Tcutil_addressof_props(te,e);_tmp571=
_tmp570.f2;{struct Cyc_Absyn_Upper_b_struct _tmp9A5;struct Cyc_Absyn_Upper_b_struct*
_tmp9A4;void*b=_tmp56E == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9A4=
_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=1,((_tmp9A5.f1=(struct Cyc_Absyn_Exp*)
_tmp56E,_tmp9A5)))),_tmp9A4))));t=Cyc_Absyn_atb_typ(_tmp56C,_tmp571,_tmp56D,b,
_tmp56F);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL28B:;_LL28C: return t;_LL288:;};_LL27F:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp574=e->r;
struct Cyc_Absyn_Exp*_tmp576;_LL28E: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp575=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp574;if(_tmp575->tag != 13)
goto _LL290;else{_tmp576=_tmp575->f1;}}_LL28F: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp576);((struct Cyc_Core_Opt*)_check_null(_tmp576->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp576->topt))->v,0);e->topt=_tmp576->topt;
goto _LL28D;_LL290:;_LL291: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp577=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp579;void*_tmp57A;struct Cyc_Absyn_Tqual
_tmp57B;struct Cyc_Absyn_PtrAtts _tmp57C;void*_tmp57D;union Cyc_Absyn_Constraint*
_tmp57E;union Cyc_Absyn_Constraint*_tmp57F;union Cyc_Absyn_Constraint*_tmp580;
_LL293: {struct Cyc_Absyn_PointerType_struct*_tmp578=(struct Cyc_Absyn_PointerType_struct*)
_tmp577;if(_tmp578->tag != 5)goto _LL295;else{_tmp579=_tmp578->f1;_tmp57A=_tmp579.elt_typ;
_tmp57B=_tmp579.elt_tq;_tmp57C=_tmp579.ptr_atts;_tmp57D=_tmp57C.rgn;_tmp57E=
_tmp57C.nullable;_tmp57F=_tmp57C.bounds;_tmp580=_tmp57C.zero_term;}}_LL294:{void*
_tmp581=Cyc_Tcutil_compress(_tmp57A);struct Cyc_Absyn_FnInfo _tmp583;struct Cyc_List_List*
_tmp584;struct Cyc_Core_Opt*_tmp585;void*_tmp586;struct Cyc_List_List*_tmp587;int
_tmp588;struct Cyc_Absyn_VarargInfo*_tmp589;struct Cyc_List_List*_tmp58A;struct Cyc_List_List*
_tmp58B;_LL298: {struct Cyc_Absyn_FnType_struct*_tmp582=(struct Cyc_Absyn_FnType_struct*)
_tmp581;if(_tmp582->tag != 10)goto _LL29A;else{_tmp583=_tmp582->f1;_tmp584=_tmp583.tvars;
_tmp585=_tmp583.effect;_tmp586=_tmp583.ret_typ;_tmp587=_tmp583.args;_tmp588=
_tmp583.c_varargs;_tmp589=_tmp583.cyc_varargs;_tmp58A=_tmp583.rgn_po;_tmp58B=
_tmp583.attributes;}}_LL299: if(_tmp584 != 0){struct _RegionHandle*_tmp58C=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp9A6;struct _tuple11 _tmp58D=(_tmp9A6.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp9A6.f2=_tmp58C,_tmp9A6)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp58C,Cyc_Tcutil_r_make_inst_var,&
_tmp58D,_tmp584);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp9AC;struct Cyc_Absyn_FnInfo
_tmp9AB;struct Cyc_Absyn_FnType_struct*_tmp9AA;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp58C,inst,(void*)((_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AC.tag=
10,((_tmp9AC.f1=((_tmp9AB.tvars=0,((_tmp9AB.effect=_tmp585,((_tmp9AB.ret_typ=
_tmp586,((_tmp9AB.args=_tmp587,((_tmp9AB.c_varargs=_tmp588,((_tmp9AB.cyc_varargs=
_tmp589,((_tmp9AB.rgn_po=_tmp58A,((_tmp9AB.attributes=_tmp58B,_tmp9AB)))))))))))))))),
_tmp9AC)))),_tmp9AA)))));struct Cyc_Absyn_PointerType_struct _tmp9B6;struct Cyc_Absyn_PtrAtts
_tmp9B5;struct Cyc_Absyn_PtrInfo _tmp9B4;struct Cyc_Absyn_PointerType_struct*
_tmp9B3;struct Cyc_Absyn_PointerType_struct*_tmp58E=(_tmp9B3=_cycalloc(sizeof(*
_tmp9B3)),((_tmp9B3[0]=((_tmp9B6.tag=5,((_tmp9B6.f1=((_tmp9B4.elt_typ=ftyp,((
_tmp9B4.elt_tq=_tmp57B,((_tmp9B4.ptr_atts=((_tmp9B5.rgn=_tmp57D,((_tmp9B5.nullable=
_tmp57E,((_tmp9B5.bounds=_tmp57F,((_tmp9B5.zero_term=_tmp580,((_tmp9B5.ptrloc=0,
_tmp9B5)))))))))),_tmp9B4)))))),_tmp9B6)))),_tmp9B3)));struct Cyc_Absyn_Exp*
_tmp58F=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp9B9;
struct Cyc_Absyn_Instantiate_e_struct*_tmp9B8;e->r=(void*)((_tmp9B8=_cycalloc(
sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=14,((_tmp9B9.f1=_tmp58F,((_tmp9B9.f2=
ts,_tmp9B9)))))),_tmp9B8))));}{struct Cyc_Core_Opt*_tmp9BA;e->topt=((_tmp9BA=
_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA->v=(void*)_tmp58E,_tmp9BA))));};}goto
_LL297;_LL29A:;_LL29B: goto _LL297;_LL297:;}goto _LL292;_LL295:;_LL296: goto _LL292;
_LL292:;}goto _LL28D;_LL28D:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct
_RegionHandle*_tmp59B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp59C=0;{
void*_tmp59D=fn_exp->r;struct Cyc_List_List*_tmp59F;_LL29D: {struct Cyc_Absyn_FnCall_e_struct*
_tmp59E=(struct Cyc_Absyn_FnCall_e_struct*)_tmp59D;if(_tmp59E->tag != 11)goto
_LL29F;else{_tmp59F=_tmp59E->f2;}}_LL29E:{void*_tmp5A0=e->r;struct Cyc_List_List*
_tmp5A2;_LL2A2: {struct Cyc_Absyn_FnCall_e_struct*_tmp5A1=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5A0;if(_tmp5A1->tag != 11)goto _LL2A4;else{_tmp5A2=_tmp5A1->f2;}}_LL2A3: {
struct Cyc_List_List*_tmp5A3=alias_arg_exps;int _tmp5A4=0;while(_tmp5A3 != 0){
while(_tmp5A4 != (int)_tmp5A3->hd){if(_tmp59F == 0){struct Cyc_Int_pa_struct _tmp9C4;
struct Cyc_Int_pa_struct _tmp9C3;void*_tmp9C2[2];const char*_tmp9C1;void*_tmp9C0;(
_tmp9C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmp9C4.tag=1,((_tmp9C4.f1=(unsigned long)((int)_tmp5A3->hd),((
_tmp9C3.tag=1,((_tmp9C3.f1=(unsigned long)_tmp5A4,((_tmp9C2[0]=& _tmp9C3,((
_tmp9C2[1]=& _tmp9C4,Cyc_aprintf(((_tmp9C1="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp9C1,sizeof(char),36))),_tag_dyneither(_tmp9C2,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp9C0,sizeof(void*),0)));}++ _tmp5A4;_tmp59F=_tmp59F->tl;_tmp5A2=((
struct Cyc_List_List*)_check_null(_tmp5A2))->tl;}{struct Cyc_Absyn_Decl*_tmp5AB;
struct Cyc_Absyn_Exp*_tmp5AC;struct _tuple10 _tmp5AA=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp59F))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5A2))->hd)->topt))->v));_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;
_tmp59F->hd=(void*)_tmp5AC;{struct Cyc_List_List*_tmp9C5;_tmp59C=((_tmp9C5=
_region_malloc(_tmp59B,sizeof(*_tmp9C5)),((_tmp9C5->hd=_tmp5AB,((_tmp9C5->tl=
_tmp59C,_tmp9C5))))));}_tmp5A3=_tmp5A3->tl;};}goto _LL2A1;}_LL2A4:;_LL2A5: {const
char*_tmp9C8;void*_tmp9C7;(_tmp9C7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C8="not a function call!",
_tag_dyneither(_tmp9C8,sizeof(char),21))),_tag_dyneither(_tmp9C7,sizeof(void*),0)));}
_LL2A1:;}goto _LL29C;_LL29F:;_LL2A0: {const char*_tmp9CB;void*_tmp9CA;(_tmp9CA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9CB="not a function call!",_tag_dyneither(_tmp9CB,sizeof(char),21))),
_tag_dyneither(_tmp9CA,sizeof(void*),0)));}_LL29C:;}while(_tmp59C != 0){struct Cyc_Absyn_Decl*
_tmp5B2=(struct Cyc_Absyn_Decl*)_tmp59C->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp5B2,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp59C=_tmp59C->tl;}e->topt=
0;e->r=fn_exp->r;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*
_tmp5B3=e->r;struct Cyc_Absyn_Exp*_tmp5B5;struct _tuple1*_tmp5B7;struct Cyc_Absyn_Exp*
_tmp5B9;struct Cyc_List_List*_tmp5BA;struct Cyc_Core_Opt*_tmp5BC;struct Cyc_List_List*
_tmp5BD;union Cyc_Absyn_Cnst _tmp5BF;union Cyc_Absyn_Cnst*_tmp5C0;struct _tuple1*
_tmp5C2;void*_tmp5C3;enum Cyc_Absyn_Primop _tmp5C5;struct Cyc_List_List*_tmp5C6;
struct Cyc_Absyn_Exp*_tmp5C8;enum Cyc_Absyn_Incrementor _tmp5C9;struct Cyc_Absyn_Exp*
_tmp5CB;struct Cyc_Core_Opt*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CD;struct Cyc_Absyn_Exp*
_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Exp*
_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D6;struct Cyc_Absyn_Exp*
_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Exp*
_tmp5DC;struct Cyc_List_List*_tmp5DD;struct Cyc_Absyn_VarargCallInfo*_tmp5DE;
struct Cyc_Absyn_VarargCallInfo**_tmp5DF;struct Cyc_Absyn_Exp*_tmp5E1;struct Cyc_Absyn_Exp*
_tmp5E3;struct Cyc_List_List*_tmp5E4;void*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E7;
enum Cyc_Absyn_Coercion _tmp5E8;enum Cyc_Absyn_Coercion*_tmp5E9;struct Cyc_Absyn_Exp*
_tmp5EB;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp**_tmp5EE;struct Cyc_Absyn_Exp*
_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F1;void*_tmp5F3;void*_tmp5F5;void*_tmp5F6;
struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FA;struct _dyneither_ptr*
_tmp5FB;int _tmp5FC;int*_tmp5FD;int _tmp5FE;int*_tmp5FF;struct Cyc_Absyn_Exp*
_tmp601;struct _dyneither_ptr*_tmp602;int _tmp603;int*_tmp604;int _tmp605;int*
_tmp606;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_Absyn_Exp*_tmp609;struct Cyc_List_List*
_tmp60B;struct _tuple8*_tmp60D;struct Cyc_List_List*_tmp60E;struct Cyc_List_List*
_tmp610;struct Cyc_Absyn_Stmt*_tmp612;struct Cyc_Absyn_Vardecl*_tmp614;struct Cyc_Absyn_Exp*
_tmp615;struct Cyc_Absyn_Exp*_tmp616;int _tmp617;int*_tmp618;struct _tuple1*_tmp61A;
struct _tuple1**_tmp61B;struct Cyc_List_List*_tmp61C;struct Cyc_List_List**_tmp61D;
struct Cyc_List_List*_tmp61E;struct Cyc_Absyn_Aggrdecl*_tmp61F;struct Cyc_Absyn_Aggrdecl**
_tmp620;void*_tmp622;struct Cyc_List_List*_tmp623;struct Cyc_List_List*_tmp625;
struct Cyc_Absyn_Datatypedecl*_tmp626;struct Cyc_Absyn_Datatypefield*_tmp627;
struct _tuple1*_tmp629;struct _tuple1**_tmp62A;struct Cyc_Absyn_Enumdecl*_tmp62B;
struct Cyc_Absyn_Enumfield*_tmp62C;struct _tuple1*_tmp62E;struct _tuple1**_tmp62F;
void*_tmp630;struct Cyc_Absyn_Enumfield*_tmp631;struct Cyc_Absyn_MallocInfo _tmp633;
int _tmp634;int*_tmp635;struct Cyc_Absyn_Exp*_tmp636;struct Cyc_Absyn_Exp**_tmp637;
void**_tmp638;void***_tmp639;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp**
_tmp63B;int _tmp63C;int*_tmp63D;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*
_tmp640;struct Cyc_Absyn_Exp*_tmp642;struct _dyneither_ptr*_tmp643;void*_tmp645;
_LL2A7: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp5B4=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp5B3;if(_tmp5B4->tag != 13)goto _LL2A9;else{_tmp5B5=_tmp5B4->f1;}}_LL2A8: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp5B5);return;_LL2A9: {struct Cyc_Absyn_UnknownId_e_struct*_tmp5B6=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp5B3;if(_tmp5B6->tag != 2)goto _LL2AB;else{
_tmp5B7=_tmp5B6->f1;}}_LL2AA: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp5B7);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2AB: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp5B8=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp5B3;if(_tmp5B8->tag != 10)goto _LL2AD;else{
_tmp5B9=_tmp5B8->f1;_tmp5BA=_tmp5B8->f2;}}_LL2AC: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp5B9,_tmp5BA);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2AD: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5BB=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp5B3;if(_tmp5BB->tag != 36)
goto _LL2AF;else{_tmp5BC=_tmp5BB->f1;_tmp5BD=_tmp5BB->f2;}}_LL2AE: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp5BD);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2AF: {struct
Cyc_Absyn_Const_e_struct*_tmp5BE=(struct Cyc_Absyn_Const_e_struct*)_tmp5B3;if(
_tmp5BE->tag != 0)goto _LL2B1;else{_tmp5BF=_tmp5BE->f1;_tmp5C0=(union Cyc_Absyn_Cnst*)&
_tmp5BE->f1;}}_LL2B0: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp5C0,e);goto _LL2A6;_LL2B1: {struct Cyc_Absyn_Var_e_struct*_tmp5C1=(struct Cyc_Absyn_Var_e_struct*)
_tmp5B3;if(_tmp5C1->tag != 1)goto _LL2B3;else{_tmp5C2=_tmp5C1->f1;_tmp5C3=(void*)
_tmp5C1->f2;}}_LL2B2: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp5C2,_tmp5C3);goto _LL2A6;
_LL2B3: {struct Cyc_Absyn_Primop_e_struct*_tmp5C4=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5B3;if(_tmp5C4->tag != 3)goto _LL2B5;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;}}
_LL2B4: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5C5,_tmp5C6);goto _LL2A6;_LL2B5: {
struct Cyc_Absyn_Increment_e_struct*_tmp5C7=(struct Cyc_Absyn_Increment_e_struct*)
_tmp5B3;if(_tmp5C7->tag != 5)goto _LL2B7;else{_tmp5C8=_tmp5C7->f1;_tmp5C9=_tmp5C7->f2;}}
_LL2B6: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5C8,_tmp5C9);goto _LL2A6;_LL2B7: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5CA=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5B3;if(_tmp5CA->tag != 4)goto _LL2B9;else{_tmp5CB=_tmp5CA->f1;_tmp5CC=_tmp5CA->f2;
_tmp5CD=_tmp5CA->f3;}}_LL2B8: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5CB,_tmp5CC,
_tmp5CD);goto _LL2A6;_LL2B9: {struct Cyc_Absyn_Conditional_e_struct*_tmp5CE=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp5B3;if(_tmp5CE->tag != 6)goto _LL2BB;
else{_tmp5CF=_tmp5CE->f1;_tmp5D0=_tmp5CE->f2;_tmp5D1=_tmp5CE->f3;}}_LL2BA: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5CF,_tmp5D0,_tmp5D1);goto _LL2A6;_LL2BB: {struct Cyc_Absyn_And_e_struct*
_tmp5D2=(struct Cyc_Absyn_And_e_struct*)_tmp5B3;if(_tmp5D2->tag != 7)goto _LL2BD;
else{_tmp5D3=_tmp5D2->f1;_tmp5D4=_tmp5D2->f2;}}_LL2BC: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5D3,_tmp5D4);goto _LL2A6;_LL2BD: {struct Cyc_Absyn_Or_e_struct*_tmp5D5=(struct
Cyc_Absyn_Or_e_struct*)_tmp5B3;if(_tmp5D5->tag != 8)goto _LL2BF;else{_tmp5D6=
_tmp5D5->f1;_tmp5D7=_tmp5D5->f2;}}_LL2BE: t=Cyc_Tcexp_tcOr(te,loc,_tmp5D6,_tmp5D7);
goto _LL2A6;_LL2BF: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5D8=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp5B3;if(_tmp5D8->tag != 9)goto _LL2C1;else{_tmp5D9=_tmp5D8->f1;_tmp5DA=_tmp5D8->f2;}}
_LL2C0: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5D9,_tmp5DA);goto _LL2A6;_LL2C1: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5DB=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5B3;if(_tmp5DB->tag != 11)goto _LL2C3;else{_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;
_tmp5DE=_tmp5DB->f3;_tmp5DF=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5DB->f3;}}
_LL2C2: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp647;_push_handler(& _tmp647);{int _tmp649=0;if(setjmp(
_tmp647.handler))_tmp649=1;if(!_tmp649){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp648=(void*)_exn_thrown;void*_tmp64B=_tmp648;struct
_dyneither_ptr _tmp64D;_LL2FA: {struct Cyc_Core_Failure_struct*_tmp64C=(struct Cyc_Core_Failure_struct*)
_tmp64B;if(_tmp64C->tag != Cyc_Core_Failure)goto _LL2FC;else{_tmp64D=_tmp64C->f1;}}
_LL2FB: ok=0;fn_exp=e;goto _LL2F9;_LL2FC:;_LL2FD:(void)_throw(_tmp64B);_LL2F9:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5DC,_tmp5DD,_tmp5DF,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2A6;}_LL2C3: {
struct Cyc_Absyn_Throw_e_struct*_tmp5E0=(struct Cyc_Absyn_Throw_e_struct*)_tmp5B3;
if(_tmp5E0->tag != 12)goto _LL2C5;else{_tmp5E1=_tmp5E0->f1;}}_LL2C4: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp5E1);goto _LL2A6;_LL2C5: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5E2=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp5B3;if(_tmp5E2->tag != 14)goto
_LL2C7;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E2->f2;}}_LL2C6: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5E3,_tmp5E4);goto _LL2A6;_LL2C7: {struct Cyc_Absyn_Cast_e_struct*
_tmp5E5=(struct Cyc_Absyn_Cast_e_struct*)_tmp5B3;if(_tmp5E5->tag != 15)goto _LL2C9;
else{_tmp5E6=(void*)_tmp5E5->f1;_tmp5E7=_tmp5E5->f2;_tmp5E8=_tmp5E5->f4;_tmp5E9=(
enum Cyc_Absyn_Coercion*)& _tmp5E5->f4;}}_LL2C8: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5E6,_tmp5E7,(enum Cyc_Absyn_Coercion*)_tmp5E9);goto _LL2A6;_LL2C9: {struct Cyc_Absyn_Address_e_struct*
_tmp5EA=(struct Cyc_Absyn_Address_e_struct*)_tmp5B3;if(_tmp5EA->tag != 16)goto
_LL2CB;else{_tmp5EB=_tmp5EA->f1;}}_LL2CA: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp5EB);goto _LL2A6;_LL2CB: {struct Cyc_Absyn_New_e_struct*_tmp5EC=(struct Cyc_Absyn_New_e_struct*)
_tmp5B3;if(_tmp5EC->tag != 17)goto _LL2CD;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=(struct
Cyc_Absyn_Exp**)& _tmp5EC->f1;_tmp5EF=_tmp5EC->f2;}}_LL2CC: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp5EE,e,_tmp5EF);goto _LL2A6;_LL2CD: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp5F0=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5B3;if(_tmp5F0->tag != 19)goto
_LL2CF;else{_tmp5F1=_tmp5F0->f1;}}_LL2CE: {void*_tmp64E=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp5F1);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp64E);goto _LL2A6;}_LL2CF: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5F2=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5B3;if(_tmp5F2->tag != 18)goto _LL2D1;else{_tmp5F3=(void*)_tmp5F2->f1;}}_LL2D0:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5F3);goto _LL2A6;_LL2D1: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp5F4=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp5B3;if(_tmp5F4->tag != 20)goto
_LL2D3;else{_tmp5F5=(void*)_tmp5F4->f1;_tmp5F6=(void*)_tmp5F4->f2;}}_LL2D2: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp5F5,_tmp5F6);goto _LL2A6;_LL2D3: {struct Cyc_Absyn_Deref_e_struct*
_tmp5F7=(struct Cyc_Absyn_Deref_e_struct*)_tmp5B3;if(_tmp5F7->tag != 21)goto _LL2D5;
else{_tmp5F8=_tmp5F7->f1;}}_LL2D4: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp5F8);goto
_LL2A6;_LL2D5: {struct Cyc_Absyn_AggrMember_e_struct*_tmp5F9=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp5B3;if(_tmp5F9->tag != 22)goto _LL2D7;else{_tmp5FA=_tmp5F9->f1;_tmp5FB=_tmp5F9->f2;
_tmp5FC=_tmp5F9->f3;_tmp5FD=(int*)& _tmp5F9->f3;_tmp5FE=_tmp5F9->f4;_tmp5FF=(int*)&
_tmp5F9->f4;}}_LL2D6: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp5FA,_tmp5FB,
_tmp5FD,_tmp5FF);goto _LL2A6;_LL2D7: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp600=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp5B3;if(_tmp600->tag != 23)goto _LL2D9;
else{_tmp601=_tmp600->f1;_tmp602=_tmp600->f2;_tmp603=_tmp600->f3;_tmp604=(int*)&
_tmp600->f3;_tmp605=_tmp600->f4;_tmp606=(int*)& _tmp600->f4;}}_LL2D8: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp601,_tmp602,_tmp604,_tmp606);goto _LL2A6;_LL2D9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp607=(struct Cyc_Absyn_Subscript_e_struct*)_tmp5B3;if(_tmp607->tag != 24)goto
_LL2DB;else{_tmp608=_tmp607->f1;_tmp609=_tmp607->f2;}}_LL2DA: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp608,_tmp609);goto _LL2A6;_LL2DB: {struct Cyc_Absyn_Tuple_e_struct*
_tmp60A=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5B3;if(_tmp60A->tag != 25)goto _LL2DD;
else{_tmp60B=_tmp60A->f1;}}_LL2DC: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp60B);goto
_LL2A6;_LL2DD: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp60C=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp5B3;if(_tmp60C->tag != 26)goto _LL2DF;else{_tmp60D=_tmp60C->f1;_tmp60E=_tmp60C->f2;}}
_LL2DE: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp60D,_tmp60E);goto _LL2A6;_LL2DF: {
struct Cyc_Absyn_Array_e_struct*_tmp60F=(struct Cyc_Absyn_Array_e_struct*)_tmp5B3;
if(_tmp60F->tag != 27)goto _LL2E1;else{_tmp610=_tmp60F->f1;}}_LL2E0: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp64F=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp651;void*_tmp652;void**_tmp653;union Cyc_Absyn_Constraint*
_tmp654;_LL2FF: {struct Cyc_Absyn_ArrayType_struct*_tmp650=(struct Cyc_Absyn_ArrayType_struct*)
_tmp64F;if(_tmp650->tag != 9)goto _LL301;else{_tmp651=_tmp650->f1;_tmp652=_tmp651.elt_type;
_tmp653=(void**)&(_tmp650->f1).elt_type;_tmp654=_tmp651.zero_term;}}_LL300:
elt_topt=(void**)_tmp653;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp654);goto _LL2FE;_LL301:;_LL302: goto _LL2FE;_LL2FE:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp610);goto _LL2A6;}_LL2E1: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp611=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp5B3;
if(_tmp611->tag != 37)goto _LL2E3;else{_tmp612=_tmp611->f1;}}_LL2E2: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp612);goto _LL2A6;_LL2E3: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp613=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp5B3;if(_tmp613->tag != 28)
goto _LL2E5;else{_tmp614=_tmp613->f1;_tmp615=_tmp613->f2;_tmp616=_tmp613->f3;
_tmp617=_tmp613->f4;_tmp618=(int*)& _tmp613->f4;}}_LL2E4: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp614,_tmp615,_tmp616,_tmp618);goto _LL2A6;_LL2E5: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp619=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5B3;if(_tmp619->tag != 29)goto
_LL2E7;else{_tmp61A=_tmp619->f1;_tmp61B=(struct _tuple1**)& _tmp619->f1;_tmp61C=
_tmp619->f2;_tmp61D=(struct Cyc_List_List**)& _tmp619->f2;_tmp61E=_tmp619->f3;
_tmp61F=_tmp619->f4;_tmp620=(struct Cyc_Absyn_Aggrdecl**)& _tmp619->f4;}}_LL2E6: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp61B,_tmp61D,_tmp61E,_tmp620);goto _LL2A6;
_LL2E7: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp621=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp5B3;if(_tmp621->tag != 30)goto _LL2E9;else{_tmp622=(void*)_tmp621->f1;_tmp623=
_tmp621->f2;}}_LL2E8: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp622,_tmp623);goto _LL2A6;
_LL2E9: {struct Cyc_Absyn_Datatype_e_struct*_tmp624=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp5B3;if(_tmp624->tag != 31)goto _LL2EB;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;
_tmp627=_tmp624->f3;}}_LL2EA: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp625,
_tmp626,_tmp627);goto _LL2A6;_LL2EB: {struct Cyc_Absyn_Enum_e_struct*_tmp628=(
struct Cyc_Absyn_Enum_e_struct*)_tmp5B3;if(_tmp628->tag != 32)goto _LL2ED;else{
_tmp629=_tmp628->f1;_tmp62A=(struct _tuple1**)& _tmp628->f1;_tmp62B=_tmp628->f2;
_tmp62C=_tmp628->f3;}}_LL2EC:*_tmp62A=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp62C))->name;{struct Cyc_Absyn_EnumType_struct _tmp9CE;struct Cyc_Absyn_EnumType_struct*
_tmp9CD;t=(void*)((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9CE.tag=
14,((_tmp9CE.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp62B))->name,((
_tmp9CE.f2=_tmp62B,_tmp9CE)))))),_tmp9CD))));}goto _LL2A6;_LL2ED: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp62D=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp5B3;if(_tmp62D->tag != 33)goto
_LL2EF;else{_tmp62E=_tmp62D->f1;_tmp62F=(struct _tuple1**)& _tmp62D->f1;_tmp630=(
void*)_tmp62D->f2;_tmp631=_tmp62D->f3;}}_LL2EE:*_tmp62F=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp631))->name;t=_tmp630;goto _LL2A6;_LL2EF: {struct Cyc_Absyn_Malloc_e_struct*
_tmp632=(struct Cyc_Absyn_Malloc_e_struct*)_tmp5B3;if(_tmp632->tag != 34)goto
_LL2F1;else{_tmp633=_tmp632->f1;_tmp634=_tmp633.is_calloc;_tmp635=(int*)&(
_tmp632->f1).is_calloc;_tmp636=_tmp633.rgn;_tmp637=(struct Cyc_Absyn_Exp**)&(
_tmp632->f1).rgn;_tmp638=_tmp633.elt_type;_tmp639=(void***)&(_tmp632->f1).elt_type;
_tmp63A=_tmp633.num_elts;_tmp63B=(struct Cyc_Absyn_Exp**)&(_tmp632->f1).num_elts;
_tmp63C=_tmp633.fat_result;_tmp63D=(int*)&(_tmp632->f1).fat_result;}}_LL2F0: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp637,_tmp639,_tmp63B,_tmp635,_tmp63D);goto
_LL2A6;_LL2F1: {struct Cyc_Absyn_Swap_e_struct*_tmp63E=(struct Cyc_Absyn_Swap_e_struct*)
_tmp5B3;if(_tmp63E->tag != 35)goto _LL2F3;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63E->f2;}}
_LL2F2: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp63F,_tmp640);goto _LL2A6;_LL2F3: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp641=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp5B3;if(_tmp641->tag != 38)goto _LL2F5;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;}}
_LL2F4: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp642,_tmp643);goto _LL2A6;_LL2F5: {
struct Cyc_Absyn_Valueof_e_struct*_tmp644=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp5B3;if(_tmp644->tag != 39)goto _LL2F7;else{_tmp645=(void*)_tmp644->f1;}}_LL2F6:
if(!te->allow_valueof){const char*_tmp9D1;void*_tmp9D0;(_tmp9D0=0,Cyc_Tcutil_terr(
e->loc,((_tmp9D1="valueof(-) can only occur within types",_tag_dyneither(_tmp9D1,
sizeof(char),39))),_tag_dyneither(_tmp9D0,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2A6;_LL2F7: {struct Cyc_Absyn_Asm_e_struct*_tmp646=(struct Cyc_Absyn_Asm_e_struct*)
_tmp5B3;if(_tmp646->tag != 40)goto _LL2A6;}_LL2F8:{const char*_tmp9D4;void*_tmp9D3;(
_tmp9D3=0,Cyc_Tcutil_terr(e->loc,((_tmp9D4="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmp9D4,sizeof(char),50))),_tag_dyneither(_tmp9D3,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2A6:;}{struct Cyc_Core_Opt*
_tmp9D5;e->topt=((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5->v=t,_tmp9D5))));};}
