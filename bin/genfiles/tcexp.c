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
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*
f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
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
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
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
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
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
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
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
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp636;(
_tmp636=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
msg,ap),_tag_dyneither(_tmp636,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct
_tmp639;struct Cyc_Absyn_Var_e_struct*_tmp638;e->r=(void*)((_tmp638=_cycalloc(
sizeof(*_tmp638)),((_tmp638[0]=((_tmp639.tag=1,((_tmp639.f1=q,((_tmp639.f2=(void*)
_tmp7,_tmp639)))))),_tmp638))));}goto _LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*
_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)_tmp5;if(_tmp8->tag != 3)goto _LL5;else{
_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{struct Cyc_Absyn_Enum_e_struct _tmp63C;
struct Cyc_Absyn_Enum_e_struct*_tmp63B;e->r=(void*)((_tmp63B=_cycalloc(sizeof(*
_tmp63B)),((_tmp63B[0]=((_tmp63C.tag=32,((_tmp63C.f1=q,((_tmp63C.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp9,((_tmp63C.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp63C)))))))),_tmp63B))));}
goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6:{struct Cyc_Absyn_AnonEnum_e_struct _tmp63F;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp63E;e->r=(void*)((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E[0]=((_tmp63F.tag=
33,((_tmp63F.f1=q,((_tmp63F.f2=(void*)_tmpC,((_tmp63F.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp63F)))))))),_tmp63E))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp642;struct Cyc_Absyn_Datatype_e_struct*_tmp641;e->r=(void*)((_tmp641=
_cycalloc(sizeof(*_tmp641)),((_tmp641[0]=((_tmp642.tag=31,((_tmp642.f1=0,((
_tmp642.f2=_tmpF,((_tmp642.f3=_tmp10,_tmp642)))))))),_tmp641))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp646;void*_tmp645[1];struct Cyc_String_pa_struct
_tmp644;(_tmp644.tag=0,((_tmp644.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp645[0]=& _tmp644,Cyc_Tcutil_terr(e->loc,((
_tmp646="bad occurrence of type name %s",_tag_dyneither(_tmp646,sizeof(char),31))),
_tag_dyneither(_tmp645,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp649;struct Cyc_Absyn_Var_e_struct*_tmp648;e->r=(void*)((_tmp648=_cycalloc(
sizeof(*_tmp648)),((_tmp648[0]=((_tmp649.tag=1,((_tmp649.f1=q,((_tmp649.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp649)))))),_tmp648))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp64C;struct Cyc_Absyn_Var_e_struct*_tmp64B;e->r=(void*)((_tmp64B=_cycalloc(
sizeof(*_tmp64B)),((_tmp64B[0]=((_tmp64C.tag=1,((_tmp64C.f1=q,((_tmp64C.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp64C)))))),_tmp64B))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple17{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple17*_tmp64D;return(_tmp64D=_cycalloc(sizeof(*_tmp64D)),((_tmp64D->f1=
0,((_tmp64D->f2=e,_tmp64D)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp650;struct
Cyc_Absyn_FnCall_e_struct*_tmp64F;e->r=(void*)((_tmp64F=_cycalloc(sizeof(*
_tmp64F)),((_tmp64F[0]=((_tmp650.tag=11,((_tmp650.f1=e1,((_tmp650.f2=es,((
_tmp650.f3=0,_tmp650)))))))),_tmp64F))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp654;void*_tmp653[1];struct Cyc_String_pa_struct
_tmp652;(_tmp652.tag=0,((_tmp652.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp653[0]=& _tmp652,Cyc_Tcutil_terr(e->loc,((
_tmp654="%s is a constant, not a function",_tag_dyneither(_tmp654,sizeof(char),
33))),_tag_dyneither(_tmp653,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp657;struct Cyc_Absyn_Datatype_e_struct*_tmp656;e->r=(void*)((_tmp656=
_cycalloc(sizeof(*_tmp656)),((_tmp656[0]=((_tmp657.tag=31,((_tmp657.f1=es,((
_tmp657.f2=_tmp30,((_tmp657.f3=_tmp31,_tmp657)))))))),_tmp656))));}_npop_handler(
0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*
_tmp3D=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp65A;struct Cyc_Absyn_Aggregate_e_struct*_tmp659;e->r=(void*)((_tmp659=
_cycalloc(sizeof(*_tmp659)),((_tmp659[0]=((_tmp65A.tag=29,((_tmp65A.f1=_tmp33->name,((
_tmp65A.f2=0,((_tmp65A.f3=_tmp3D,((_tmp65A.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp65A)))))))))),_tmp659))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp65E;void*_tmp65D[1];
struct Cyc_String_pa_struct _tmp65C;(_tmp65C.tag=0,((_tmp65C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp65D[
0]=& _tmp65C,Cyc_Tcutil_terr(e->loc,((_tmp65E="%s is an enum constructor, not a function",
_tag_dyneither(_tmp65E,sizeof(char),42))),_tag_dyneither(_tmp65D,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp661;struct Cyc_Absyn_FnCall_e_struct*
_tmp660;e->r=(void*)((_tmp660=_cycalloc(sizeof(*_tmp660)),((_tmp660[0]=((_tmp661.tag=
11,((_tmp661.f1=e1,((_tmp661.f2=es,((_tmp661.f3=0,_tmp661)))))))),_tmp660))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp664;struct Cyc_Absyn_FnCall_e_struct*_tmp663;e->r=(void*)((_tmp663=_cycalloc(
sizeof(*_tmp663)),((_tmp663[0]=((_tmp664.tag=11,((_tmp664.f1=e1,((_tmp664.f2=es,((
_tmp664.f3=0,_tmp664)))))))),_tmp663))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp667;
struct Cyc_Absyn_Array_e_struct*_tmp666;e->r=(void*)((_tmp666=_cycalloc(sizeof(*
_tmp666)),((_tmp666[0]=((_tmp667.tag=27,((_tmp667.f1=des,_tmp667)))),_tmp666))));}
return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp66A;void*_tmp669;(_tmp669=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp66A="struct type not properly set",
_tag_dyneither(_tmp66A,sizeof(char),29))),_tag_dyneither(_tmp669,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp66D;struct Cyc_Absyn_Aggregate_e_struct*_tmp66C;e->r=(void*)((_tmp66C=
_cycalloc(sizeof(*_tmp66C)),((_tmp66C[0]=((_tmp66D.tag=29,((_tmp66D.f1=_tmp58->name,((
_tmp66D.f2=0,((_tmp66D.f3=des,((_tmp66D.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp66D)))))))))),_tmp66C))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 9)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp670;struct Cyc_Absyn_Array_e_struct*_tmp66F;e->r=(
void*)((_tmp66F=_cycalloc(sizeof(*_tmp66F)),((_tmp66F[0]=((_tmp670.tag=27,((
_tmp670.f1=des,_tmp670)))),_tmp66F))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp673;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp672;e->r=(void*)((_tmp672=_cycalloc(sizeof(*_tmp672)),((_tmp672[0]=((_tmp673.tag=
30,((_tmp673.f1=(void*)t,((_tmp673.f2=des,_tmp673)))))),_tmp672))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp676;struct Cyc_Absyn_Array_e_struct*
_tmp675;e->r=(void*)((_tmp675=_cycalloc(sizeof(*_tmp675)),((_tmp675[0]=((_tmp676.tag=
27,((_tmp676.f1=des,_tmp676)))),_tmp675))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp67B;void*_tmp67A[2];struct Cyc_String_pa_struct _tmp679;
struct Cyc_String_pa_struct _tmp678;(_tmp678.tag=0,((_tmp678.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp679.tag=0,((_tmp679.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp67A[0]=& _tmp679,((_tmp67A[1]=& _tmp678,Cyc_Tcutil_terr(
e->loc,((_tmp67B="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp67B,sizeof(char),53))),_tag_dyneither(_tmp67A,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp67C;struct
_tuple0 _tmp6C=(_tmp67C.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp67C.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp67C)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp67F;struct Cyc_Tcexp_TestEnv _tmp67E;return(_tmp67E.eq=((_tmp67F=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp67F)),((_tmp67F->f1=_tmp6F,((
_tmp67F->f2=_tmp72,_tmp67F)))))),((_tmp67E.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp67E)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp680;return(_tmp680.eq=0,((_tmp680.isTrue=0,_tmp680)));};}
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
_tmp84 == 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp683;struct Cyc_Absyn_Const_e_struct*
_tmp682;e->r=(void*)((_tmp682=_cycalloc(sizeof(*_tmp682)),((_tmp682[0]=((_tmp683.tag=
0,((_tmp683.f1=Cyc_Absyn_Null_c,_tmp683)))),_tmp682))));}{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp696;
struct Cyc_Absyn_PtrAtts _tmp695;struct Cyc_Core_Opt*_tmp694;struct Cyc_Core_Opt*
_tmp693;struct Cyc_Absyn_PtrInfo _tmp692;struct Cyc_Absyn_PointerType_struct*
_tmp691;t=(void*)((_tmp691=_cycalloc(sizeof(*_tmp691)),((_tmp691[0]=((_tmp696.tag=
5,((_tmp696.f1=((_tmp692.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp693=_cycalloc(sizeof(*_tmp693)),((_tmp693->v=_tmp99,_tmp693))))),((_tmp692.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp692.ptr_atts=((_tmp695.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp694=_cycalloc(sizeof(*_tmp694)),((
_tmp694->v=_tmp99,_tmp694))))),((_tmp695.nullable=Cyc_Absyn_true_conref,((
_tmp695.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp695.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp695.ptrloc=0,_tmp695)))))))))),_tmp692)))))),_tmp696)))),_tmp691))));}goto
_LL50;};_LL5B: {struct Cyc_Absyn_TagType_struct*_tmp95=(struct Cyc_Absyn_TagType_struct*)
_tmp87;if(_tmp95->tag != 20)goto _LL5D;else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {
struct Cyc_Absyn_ValueofType_struct _tmp699;struct Cyc_Absyn_ValueofType_struct*
_tmp698;struct Cyc_Absyn_ValueofType_struct*_tmpA0=(_tmp698=_cycalloc(sizeof(*
_tmp698)),((_tmp698[0]=((_tmp699.tag=19,((_tmp699.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0),_tmp699)))),_tmp698)));if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){{const char*_tmp69E;void*_tmp69D[2];struct Cyc_String_pa_struct _tmp69C;
struct Cyc_String_pa_struct _tmp69B;(_tmp69B.tag=0,((_tmp69B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0)),((
_tmp69C.tag=0,((_tmp69C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp96)),((_tmp69D[0]=& _tmp69C,((_tmp69D[1]=& _tmp69B,Cyc_Tcutil_terr(loc,((
_tmp69E="expecting %s but found %s",_tag_dyneither(_tmp69E,sizeof(char),26))),
_tag_dyneither(_tmp69D,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp6A1;struct Cyc_Absyn_TagType_struct*_tmp6A0;t=(
void*)((_tmp6A0=_cycalloc(sizeof(*_tmp6A0)),((_tmp6A0[0]=((_tmp6A1.tag=20,((
_tmp6A1.f1=(void*)((void*)_tmpA0),_tmp6A1)))),_tmp6A0))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);{struct Cyc_Core_Opt*_tmp6A2;elen->topt=((
_tmp6A2=_cycalloc(sizeof(*_tmp6A2)),((_tmp6A2->v=Cyc_Absyn_uint_typ,_tmp6A2))));}{
struct Cyc_Absyn_Upper_b_struct _tmp6A5;struct Cyc_Absyn_Upper_b_struct*_tmp6A4;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6A4=_cycalloc(sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=1,((
_tmp6A5.f1=elen,_tmp6A5)))),_tmp6A4)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual
_tmpAF;_LL64: {struct Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAC;if(_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}
_LL65: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)
elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {
struct Cyc_Absyn_PointerType_struct*_tmpB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp6A6;e->topt=((_tmp6A6=_cycalloc(sizeof(*
_tmp6A6)),((_tmp6A6->v=t,_tmp6A6))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp6A9;struct Cyc_Absyn_Upper_b_struct*
_tmp6A8;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp6A8=_cycalloc(sizeof(*_tmp6A8)),((_tmp6A8[0]=((_tmp6A9.tag=1,((
_tmp6A9.f1=elen,_tmp6A9)))),_tmp6A8)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp6AA;e->topt=((_tmp6AA=_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA->v=t,_tmp6AA))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*_tmpB5=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp6BD;struct Cyc_Absyn_PtrAtts _tmp6BC;
struct Cyc_Core_Opt*_tmp6BB;struct Cyc_Core_Opt*_tmp6BA;struct Cyc_Absyn_PtrInfo
_tmp6B9;struct Cyc_Absyn_PointerType_struct*_tmp6B8;t=(void*)((_tmp6B8=_cycalloc(
sizeof(*_tmp6B8)),((_tmp6B8[0]=((_tmp6BD.tag=5,((_tmp6BD.f1=((_tmp6B9.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp6BA=_cycalloc(
sizeof(*_tmp6BA)),((_tmp6BA->v=_tmpB5,_tmp6BA))))),((_tmp6B9.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp6B9.ptr_atts=((_tmp6BC.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp6BB=_cycalloc(sizeof(*_tmp6BB)),((_tmp6BB->v=_tmpB5,_tmp6BB))))),((_tmp6BC.nullable=
Cyc_Absyn_true_conref,((_tmp6BC.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6BC.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6BC.ptrloc=0,_tmp6BC)))))))))),_tmp6B9)))))),_tmp6BD)))),_tmp6B8))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){
void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;
struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}_LL6C: {const char*_tmp6C1;void*_tmp6C0[1];
struct Cyc_String_pa_struct _tmp6BF;return(_tmp6BF.tag=0,((_tmp6BF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6C0[0]=&
_tmp6BF,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6C1="undeclared identifier: %s",
_tag_dyneither(_tmp6C1,sizeof(char),26))),_tag_dyneither(_tmp6C0,sizeof(void*),1)))))));}
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
else{_tmpC7=_tmpC6->f1;}}_LL76:{union Cyc_Absyn_Nmspace _tmp6C2;(*q).f1=(((_tmp6C2.Loc_n).val=
0,(((_tmp6C2.Loc_n).tag=4,_tmp6C2))));}if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79:{struct Cyc_Absyn_Valueof_e_struct _tmp6C5;struct Cyc_Absyn_Valueof_e_struct*
_tmp6C4;e->r=(void*)((_tmp6C4=_cycalloc(sizeof(*_tmp6C4)),((_tmp6C4[0]=((_tmp6C5.tag=
39,((_tmp6C5.f1=(void*)_tmpCE,_tmp6C5)))),_tmp6C4))));}goto _LL77;_LL7A:;_LL7B:
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
char*_tmp6C8;void*_tmp6C7;(_tmp6C7=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDC->hd)->loc,((_tmp6C8="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp6C8,sizeof(char),49))),_tag_dyneither(_tmp6C7,sizeof(void*),0)));}};}}
return;_LL7C:;}if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE0 != 0;(((desc_types=desc_types->tl,
_tmpE0=_tmpE0->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp6CD;void*
_tmp6CC[2];struct Cyc_String_pa_struct _tmp6CB;struct Cyc_String_pa_struct _tmp6CA;(
_tmp6CA.tag=0,((_tmp6CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6CB.tag=0,((_tmp6CB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6CC[
0]=& _tmp6CB,((_tmp6CC[1]=& _tmp6CA,Cyc_Tcutil_terr(e->loc,((_tmp6CD="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp6CD,sizeof(char),51))),_tag_dyneither(_tmp6CC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp6CE;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((
_tmp6CE->hd=(void*)arg_cnt,((_tmp6CE->tl=*alias_arg_exps,_tmp6CE))))));}{struct
_RegionHandle*_tmpE6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE6,t) && !Cyc_Tcutil_is_noalias_path(_tmpE6,e)){const char*_tmp6D1;void*
_tmp6D0;(_tmp6D0=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6D1="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp6D1,sizeof(char),49))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp6D4;void*_tmp6D3;(_tmp6D3=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp6D4="too few arguments",_tag_dyneither(_tmp6D4,sizeof(char),18))),
_tag_dyneither(_tmp6D3,sizeof(void*),0)));}if(_tmpE0 != 0){const char*_tmp6D7;void*
_tmp6D6;(_tmp6D6=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6D7="too many arguments",_tag_dyneither(_tmp6D7,sizeof(char),19))),
_tag_dyneither(_tmp6D6,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus:
_LL84: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp6DB;void*_tmp6DA[1];struct Cyc_String_pa_struct
_tmp6D9;(_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp6DA[0]=& _tmp6D9,Cyc_Tcutil_terr(loc,((_tmp6DB="expecting arithmetic type but found %s",
_tag_dyneither(_tmp6DB,sizeof(char),39))),_tag_dyneither(_tmp6DA,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6DF;void*_tmp6DE[1];struct Cyc_String_pa_struct _tmp6DD;(_tmp6DD.tag=
0,((_tmp6DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6DE[0]=& _tmp6DD,Cyc_Tcutil_terr(loc,((_tmp6DF="expecting integral or * type but found %s",
_tag_dyneither(_tmp6DF,sizeof(char),42))),_tag_dyneither(_tmp6DE,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6E3;void*_tmp6E2[1];struct Cyc_String_pa_struct _tmp6E1;(_tmp6E1.tag=
0,((_tmp6E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6E2[0]=& _tmp6E1,Cyc_Tcutil_terr(loc,((_tmp6E3="expecting integral type but found %s",
_tag_dyneither(_tmp6E3,sizeof(char),37))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));}
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
_tmp6E6;void*_tmp6E5;(_tmp6E5=0,Cyc_Tcutil_terr(loc,((_tmp6E6="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp6E6,sizeof(char),55))),_tag_dyneither(_tmp6E5,sizeof(void*),0)));}
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F: {const char*_tmp6EA;void*_tmp6E9[1];
struct Cyc_String_pa_struct _tmp6E8;(_tmp6E8.tag=0,((_tmp6E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E9[0]=&
_tmp6E8,Cyc_Tcutil_terr(loc,((_tmp6EA="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp6EA,sizeof(char),47))),_tag_dyneither(_tmp6E9,sizeof(void*),1)))))));}
_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88: {const char*_tmp6ED;void*_tmp6EC;(
_tmp6EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp6ED="Non-unary primop",_tag_dyneither(_tmp6ED,sizeof(char),17))),
_tag_dyneither(_tmp6EC,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp6F1;void*_tmp6F0[
1];struct Cyc_String_pa_struct _tmp6EF;(_tmp6EF.tag=0,((_tmp6EF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp6F0[0]=& _tmp6EF,Cyc_Tcutil_terr(e1->loc,((
_tmp6F1="type %s cannot be used here",_tag_dyneither(_tmp6F1,sizeof(char),28))),
_tag_dyneither(_tmp6F0,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp6F5;void*_tmp6F4[1];struct Cyc_String_pa_struct
_tmp6F3;(_tmp6F3.tag=0,((_tmp6F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp6F4[0]=& _tmp6F3,Cyc_Tcutil_terr(e2->loc,((_tmp6F5="type %s cannot be used here",
_tag_dyneither(_tmp6F5,sizeof(char),28))),_tag_dyneither(_tmp6F4,sizeof(void*),1)))))));}
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
_tmp6F8;void*_tmp6F7;(_tmp6F7=0,Cyc_Tcutil_terr(e1->loc,((_tmp6F8="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp6F8,sizeof(char),50))),_tag_dyneither(_tmp6F7,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp6FB;void*_tmp6FA;(_tmp6FA=0,
Cyc_Tcutil_terr(e1->loc,((_tmp6FB="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp6FB,sizeof(char),54))),_tag_dyneither(_tmp6FA,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp6FF;void*_tmp6FE[1];struct
Cyc_String_pa_struct _tmp6FD;(_tmp6FD.tag=0,((_tmp6FD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6FE[0]=& _tmp6FD,Cyc_Tcutil_terr(
e2->loc,((_tmp6FF="expecting int but found %s",_tag_dyneither(_tmp6FF,sizeof(
char),27))),_tag_dyneither(_tmp6FE,sizeof(void*),1)))))));}{void*_tmp11E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp115);struct Cyc_Absyn_Exp*_tmp121;_LL9C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp11F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp11E;if(_tmp11F->tag != 0)goto
_LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*_tmp120=(struct Cyc_Absyn_Upper_b_struct*)
_tmp11E;if(_tmp120->tag != 1)goto _LL9B;else{_tmp121=_tmp120->f1;}}_LL9F: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp116)){const char*
_tmp702;void*_tmp701;(_tmp701=0,Cyc_Tcutil_warn(e1->loc,((_tmp702="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp702,sizeof(char),70))),_tag_dyneither(_tmp701,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp70C;struct Cyc_Absyn_PtrAtts _tmp70B;struct
Cyc_Absyn_PtrInfo _tmp70A;struct Cyc_Absyn_PointerType_struct*_tmp709;struct Cyc_Absyn_PointerType_struct*
_tmp124=(_tmp709=_cycalloc(sizeof(*_tmp709)),((_tmp709[0]=((_tmp70C.tag=5,((
_tmp70C.f1=((_tmp70A.elt_typ=_tmp110,((_tmp70A.elt_tq=_tmp111,((_tmp70A.ptr_atts=((
_tmp70B.rgn=_tmp113,((_tmp70B.nullable=Cyc_Absyn_true_conref,((_tmp70B.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp70B.zero_term=_tmp116,((_tmp70B.ptrloc=0,
_tmp70B)))))))))),_tmp70A)))))),_tmp70C)))),_tmp709)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp124,Cyc_Absyn_Other_coercion);return(void*)_tmp124;};_LL9B:;};
_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL96:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp711;void*_tmp710[2];struct Cyc_String_pa_struct _tmp70F;struct Cyc_String_pa_struct
_tmp70E;(_tmp70E.tag=0,((_tmp70E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp710[0]=& _tmp70F,((
_tmp710[1]=& _tmp70E,Cyc_Tcutil_terr(e1->loc,((_tmp711="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp711,sizeof(char),59))),_tag_dyneither(_tmp710,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp714;void*_tmp713;(
_tmp713=0,Cyc_Tcutil_terr(e1->loc,((_tmp714="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp714,sizeof(char),50))),_tag_dyneither(_tmp713,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp717;void*_tmp716;(_tmp716=0,
Cyc_Tcutil_terr(e1->loc,((_tmp717="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp717,sizeof(char),54))),_tag_dyneither(_tmp716,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp71C;void*_tmp71B[2];struct
Cyc_String_pa_struct _tmp71A;struct Cyc_String_pa_struct _tmp719;(_tmp719.tag=0,((
_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp71A.tag=0,((_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp71B[0]=& _tmp71A,((_tmp71B[1]=& _tmp719,Cyc_Tcutil_terr(
e2->loc,((_tmp71C="expecting either %s or int but found %s",_tag_dyneither(
_tmp71C,sizeof(char),40))),_tag_dyneither(_tmp71B,sizeof(void*),2)))))))))))));}
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
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp71D;struct _tuple0
_tmp136=(_tmp71D.f1=Cyc_Tcutil_compress(t1),((_tmp71D.f2=Cyc_Tcutil_compress(t2),
_tmp71D)));void*_tmp137;struct Cyc_Absyn_PtrInfo _tmp139;void*_tmp13A;void*_tmp13B;
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
_LLA6: goto _LLA0;_LLA0:;}{const char*_tmp722;void*_tmp721[2];struct Cyc_String_pa_struct
_tmp720;struct Cyc_String_pa_struct _tmp71F;(_tmp71F.tag=0,((_tmp71F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp720.tag=
0,((_tmp720.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp721[0]=& _tmp720,((_tmp721[1]=& _tmp71F,Cyc_Tcutil_terr(loc,((_tmp722="comparison not allowed between %s and %s",
_tag_dyneither(_tmp722,sizeof(char),41))),_tag_dyneither(_tmp721,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp723;
struct _tuple0 _tmp148=(_tmp723.f1=t1,((_tmp723.f2=t2,_tmp723)));void*_tmp149;void*
_tmp14B;void*_tmp14C;void*_tmp14E;_LLA8: _tmp149=_tmp148.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp149;if(_tmp14A->tag != 16)goto
_LLAA;else{_tmp14B=(void*)_tmp14A->f1;}};_tmp14C=_tmp148.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 16)goto
_LLAA;else{_tmp14E=(void*)_tmp14D->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}{const char*_tmp728;void*_tmp727[2];struct Cyc_String_pa_struct
_tmp726;struct Cyc_String_pa_struct _tmp725;(_tmp725.tag=0,((_tmp725.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp726.tag=
0,((_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp727[0]=& _tmp726,((_tmp727[1]=& _tmp725,Cyc_Tcutil_terr(loc,((_tmp728="comparison not allowed between %s and %s",
_tag_dyneither(_tmp728,sizeof(char),41))),_tag_dyneither(_tmp727,sizeof(void*),2)))))))))))));}
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
default: _LLBD: {const char*_tmp72B;void*_tmp72A;(_tmp72A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp72B="bad binary primop",
_tag_dyneither(_tmp72B,sizeof(char),18))),_tag_dyneither(_tmp72A,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: {const char*_tmp72E;void*_tmp72D;return(_tmp72D=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp72E="primitive operator has 0 arguments",_tag_dyneither(_tmp72E,
sizeof(char),35))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: {const char*_tmp731;void*_tmp730;return(_tmp730=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp731="primitive operator has > 2 arguments",_tag_dyneither(_tmp731,
sizeof(char),37))),_tag_dyneither(_tmp730,sizeof(void*),0)));}}return t;};}struct
_tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp159=e->r;void*_tmp15B;
struct Cyc_Absyn_Vardecl*_tmp15D;void*_tmp15F;struct Cyc_Absyn_Vardecl*_tmp161;
void*_tmp163;struct Cyc_Absyn_Vardecl*_tmp165;void*_tmp167;struct Cyc_Absyn_Vardecl*
_tmp169;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*
_tmp16E;struct _dyneither_ptr*_tmp16F;struct Cyc_Absyn_Exp*_tmp171;struct
_dyneither_ptr*_tmp172;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp176;
struct Cyc_Absyn_Exp*_tmp178;_LLC5: {struct Cyc_Absyn_Var_e_struct*_tmp15A=(struct
Cyc_Absyn_Var_e_struct*)_tmp159;if(_tmp15A->tag != 1)goto _LLC7;else{_tmp15B=(void*)
_tmp15A->f2;{struct Cyc_Absyn_Param_b_struct*_tmp15C=(struct Cyc_Absyn_Param_b_struct*)
_tmp15B;if(_tmp15C->tag != 3)goto _LLC7;else{_tmp15D=_tmp15C->f1;}};}}_LLC6:
_tmp161=_tmp15D;goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp15E=(struct
Cyc_Absyn_Var_e_struct*)_tmp159;if(_tmp15E->tag != 1)goto _LLC9;else{_tmp15F=(void*)
_tmp15E->f2;{struct Cyc_Absyn_Local_b_struct*_tmp160=(struct Cyc_Absyn_Local_b_struct*)
_tmp15F;if(_tmp160->tag != 4)goto _LLC9;else{_tmp161=_tmp160->f1;}};}}_LLC8:
_tmp165=_tmp161;goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp162=(struct
Cyc_Absyn_Var_e_struct*)_tmp159;if(_tmp162->tag != 1)goto _LLCB;else{_tmp163=(void*)
_tmp162->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp164=(struct Cyc_Absyn_Pat_b_struct*)
_tmp163;if(_tmp164->tag != 5)goto _LLCB;else{_tmp165=_tmp164->f1;}};}}_LLCA:
_tmp169=_tmp165;goto _LLCC;_LLCB: {struct Cyc_Absyn_Var_e_struct*_tmp166=(struct
Cyc_Absyn_Var_e_struct*)_tmp159;if(_tmp166->tag != 1)goto _LLCD;else{_tmp167=(void*)
_tmp166->f2;{struct Cyc_Absyn_Global_b_struct*_tmp168=(struct Cyc_Absyn_Global_b_struct*)
_tmp167;if(_tmp168->tag != 1)goto _LLCD;else{_tmp169=_tmp168->f1;}};}}_LLCC: if(!(
_tmp169->tq).real_const)return;goto _LLC4;_LLCD: {struct Cyc_Absyn_Subscript_e_struct*
_tmp16A=(struct Cyc_Absyn_Subscript_e_struct*)_tmp159;if(_tmp16A->tag != 24)goto
_LLCF;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LLCE:{void*_tmp179=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16B->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp17B;struct Cyc_Absyn_Tqual _tmp17C;struct Cyc_Absyn_ArrayInfo _tmp17E;struct Cyc_Absyn_Tqual
_tmp17F;struct Cyc_List_List*_tmp181;_LLDC: {struct Cyc_Absyn_PointerType_struct*
_tmp17A=(struct Cyc_Absyn_PointerType_struct*)_tmp179;if(_tmp17A->tag != 5)goto
_LLDE;else{_tmp17B=_tmp17A->f1;_tmp17C=_tmp17B.elt_tq;}}_LLDD: _tmp17F=_tmp17C;
goto _LLDF;_LLDE: {struct Cyc_Absyn_ArrayType_struct*_tmp17D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp179;if(_tmp17D->tag != 9)goto _LLE0;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17E.tq;}}
_LLDF: if(!_tmp17F.real_const)return;goto _LLDB;_LLE0: {struct Cyc_Absyn_TupleType_struct*
_tmp180=(struct Cyc_Absyn_TupleType_struct*)_tmp179;if(_tmp180->tag != 11)goto
_LLE2;else{_tmp181=_tmp180->f1;}}_LLE1: {unsigned int _tmp183;int _tmp184;struct
_tuple14 _tmp182=Cyc_Evexp_eval_const_uint_exp(_tmp16C);_tmp183=_tmp182.f1;
_tmp184=_tmp182.f2;if(!_tmp184){{const char*_tmp734;void*_tmp733;(_tmp733=0,Cyc_Tcutil_terr(
e->loc,((_tmp734="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp734,sizeof(char),47))),_tag_dyneither(_tmp733,sizeof(void*),0)));}return;}{
struct _handler_cons _tmp187;_push_handler(& _tmp187);{int _tmp189=0;if(setjmp(
_tmp187.handler))_tmp189=1;if(!_tmp189){{struct _tuple18 _tmp18B;struct Cyc_Absyn_Tqual
_tmp18C;struct _tuple18*_tmp18A=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp181,(int)_tmp183);_tmp18B=*_tmp18A;_tmp18C=_tmp18B.f1;if(!
_tmp18C.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp188=(
void*)_exn_thrown;void*_tmp18E=_tmp188;_LLE5: {struct Cyc_List_Nth_struct*_tmp18F=(
struct Cyc_List_Nth_struct*)_tmp18E;if(_tmp18F->tag != Cyc_List_Nth)goto _LLE7;}
_LLE6: return;_LLE7:;_LLE8:(void)_throw(_tmp18E);_LLE4:;}};}goto _LLDB;}_LLE2:;
_LLE3: goto _LLDB;_LLDB:;}goto _LLC4;_LLCF: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp16D=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp159;if(_tmp16D->tag != 22)goto
_LLD1;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;}}_LLD0:{void*_tmp190=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16E->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp192;union Cyc_Absyn_AggrInfoU _tmp193;struct Cyc_Absyn_Aggrdecl**_tmp194;struct
Cyc_List_List*_tmp196;_LLEA: {struct Cyc_Absyn_AggrType_struct*_tmp191=(struct Cyc_Absyn_AggrType_struct*)
_tmp190;if(_tmp191->tag != 12)goto _LLEC;else{_tmp192=_tmp191->f1;_tmp193=_tmp192.aggr_info;
if((_tmp193.KnownAggr).tag != 2)goto _LLEC;_tmp194=(struct Cyc_Absyn_Aggrdecl**)(
_tmp193.KnownAggr).val;}}_LLEB: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp194 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp194,_tmp16F);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE9;}_LLEC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp195=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp190;if(_tmp195->tag != 13)goto _LLEE;else{_tmp196=_tmp195->f2;}}_LLED: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp196,_tmp16F);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp170=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp159;if(_tmp170->tag != 23)goto
_LLD3;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;}}_LLD2:{void*_tmp197=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp171->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp199;void*_tmp19A;_LLF1: {struct Cyc_Absyn_PointerType_struct*_tmp198=(struct
Cyc_Absyn_PointerType_struct*)_tmp197;if(_tmp198->tag != 5)goto _LLF3;else{_tmp199=
_tmp198->f1;_tmp19A=_tmp199.elt_typ;}}_LLF2:{void*_tmp19B=Cyc_Tcutil_compress(
_tmp19A);struct Cyc_Absyn_AggrInfo _tmp19D;union Cyc_Absyn_AggrInfoU _tmp19E;struct
Cyc_Absyn_Aggrdecl**_tmp19F;struct Cyc_List_List*_tmp1A1;_LLF6: {struct Cyc_Absyn_AggrType_struct*
_tmp19C=(struct Cyc_Absyn_AggrType_struct*)_tmp19B;if(_tmp19C->tag != 12)goto _LLF8;
else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19D.aggr_info;if((_tmp19E.KnownAggr).tag != 
2)goto _LLF8;_tmp19F=(struct Cyc_Absyn_Aggrdecl**)(_tmp19E.KnownAggr).val;}}_LLF7: {
struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp19F == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp19F,_tmp172);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF5;}_LLF8: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp1A0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19B;if(_tmp1A0->tag != 13)goto _LLFA;else{_tmp1A1=_tmp1A0->f2;}}_LLF9: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1A1,_tmp172);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto
_LLF0;_LLF0:;}goto _LLC4;_LLD3: {struct Cyc_Absyn_Deref_e_struct*_tmp173=(struct
Cyc_Absyn_Deref_e_struct*)_tmp159;if(_tmp173->tag != 21)goto _LLD5;else{_tmp174=
_tmp173->f1;}}_LLD4:{void*_tmp1A2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp174->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1A4;struct Cyc_Absyn_Tqual
_tmp1A5;struct Cyc_Absyn_ArrayInfo _tmp1A7;struct Cyc_Absyn_Tqual _tmp1A8;_LLFD: {
struct Cyc_Absyn_PointerType_struct*_tmp1A3=(struct Cyc_Absyn_PointerType_struct*)
_tmp1A2;if(_tmp1A3->tag != 5)goto _LLFF;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.elt_tq;}}
_LLFE: _tmp1A8=_tmp1A5;goto _LL100;_LLFF: {struct Cyc_Absyn_ArrayType_struct*
_tmp1A6=(struct Cyc_Absyn_ArrayType_struct*)_tmp1A2;if(_tmp1A6->tag != 9)goto
_LL101;else{_tmp1A7=_tmp1A6->f1;_tmp1A8=_tmp1A7.tq;}}_LL100: if(!_tmp1A8.real_const)
return;goto _LLFC;_LL101:;_LL102: goto _LLFC;_LLFC:;}goto _LLC4;_LLD5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp175=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp159;if(_tmp175->tag != 13)
goto _LLD7;else{_tmp176=_tmp175->f1;}}_LLD6: _tmp178=_tmp176;goto _LLD8;_LLD7: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp177=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp159;if(_tmp177->tag != 14)goto _LLD9;else{_tmp178=_tmp177->f1;}}_LLD8: Cyc_Tcexp_check_writable(
te,_tmp178);return;_LLD9:;_LLDA: goto _LLC4;_LLC4:;}{const char*_tmp738;void*
_tmp737[1];struct Cyc_String_pa_struct _tmp736;(_tmp736.tag=0,((_tmp736.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp737[0]=&
_tmp736,Cyc_Tcutil_terr(e->loc,((_tmp738="attempt to write a const location: %s",
_tag_dyneither(_tmp738,sizeof(char),38))),_tag_dyneither(_tmp737,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*_tmp73B;void*_tmp73A;return(
_tmp73A=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp73B="increment/decrement of non-lvalue",
_tag_dyneither(_tmp73B,sizeof(char),34))),_tag_dyneither(_tmp73A,sizeof(void*),0)));}
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(
t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){const char*_tmp73E;void*_tmp73D;(
_tmp73D=0,Cyc_Tcutil_terr(e->loc,((_tmp73E="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp73E,sizeof(char),50))),_tag_dyneither(_tmp73D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp741;void*_tmp740;(_tmp740=0,
Cyc_Tcutil_terr(e->loc,((_tmp741="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp741,sizeof(char),54))),_tag_dyneither(_tmp740,sizeof(void*),0)));}}
else{const char*_tmp745;void*_tmp744[1];struct Cyc_String_pa_struct _tmp743;(
_tmp743.tag=0,((_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp744[0]=& _tmp743,Cyc_Tcutil_terr(e->loc,((_tmp745="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp745,sizeof(char),44))),_tag_dyneither(_tmp744,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1B7;int _tmp1B8;const char*_tmp746;struct Cyc_Tcexp_TestEnv
_tmp1B6=Cyc_Tcexp_tcTest(te,e1,((_tmp746="conditional expression",_tag_dyneither(
_tmp746,sizeof(char),23))));_tmp1B7=_tmp1B6.eq;_tmp1B8=_tmp1B6.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp747;
struct Cyc_List_List _tmp1B9=(_tmp747.hd=e3,((_tmp747.tl=0,_tmp747)));struct Cyc_List_List
_tmp748;struct Cyc_List_List _tmp1BA=(_tmp748.hd=e2,((_tmp748.tl=(struct Cyc_List_List*)&
_tmp1B9,_tmp748)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1BA)){{const char*_tmp74D;void*_tmp74C[2];struct Cyc_String_pa_struct _tmp74B;
struct Cyc_String_pa_struct _tmp74A;(_tmp74A.tag=0,((_tmp74A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp74B.tag=0,((_tmp74B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp74C[0]=& _tmp74B,((_tmp74C[1]=& _tmp74A,Cyc_Tcutil_terr(
loc,((_tmp74D="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp74D,sizeof(char),48))),_tag_dyneither(_tmp74C,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp74E;Cyc_Tcexp_tcTest(te,e1,((_tmp74E="logical-and expression",
_tag_dyneither(_tmp74E,sizeof(char),23))));}{const char*_tmp74F;Cyc_Tcexp_tcTest(
te,e2,((_tmp74F="logical-and expression",_tag_dyneither(_tmp74F,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp750;Cyc_Tcexp_tcTest(te,e1,((_tmp750="logical-or expression",
_tag_dyneither(_tmp750,sizeof(char),22))));}{const char*_tmp751;Cyc_Tcexp_tcTest(
te,e2,((_tmp751="logical-or expression",_tag_dyneither(_tmp751,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1C5=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1C5,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1C6=Cyc_Tcutil_compress(
t1);_LL104: {struct Cyc_Absyn_ArrayType_struct*_tmp1C7=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C6;if(_tmp1C7->tag != 9)goto _LL106;}_LL105:{const char*_tmp754;void*_tmp753;(
_tmp753=0,Cyc_Tcutil_terr(loc,((_tmp754="cannot assign to an array",
_tag_dyneither(_tmp754,sizeof(char),26))),_tag_dyneither(_tmp753,sizeof(void*),0)));}
goto _LL103;_LL106:;_LL107: goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),& Cyc_Tcutil_tmk)){const char*_tmp757;void*_tmp756;(_tmp756=0,Cyc_Tcutil_terr(
loc,((_tmp757="type is abstract (can't determine size).",_tag_dyneither(_tmp757,
sizeof(char),41))),_tag_dyneither(_tmp756,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp75A;void*_tmp759;return(_tmp759=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp75A="assignment to non-lvalue",_tag_dyneither(_tmp75A,sizeof(char),25))),
_tag_dyneither(_tmp759,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp1CE=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1CE,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1CE,e2)){const char*_tmp75D;void*
_tmp75C;(_tmp75C=0,Cyc_Tcutil_terr(e2->loc,((_tmp75D="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp75D,sizeof(char),49))),_tag_dyneither(_tmp75C,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp762;void*_tmp761[2];struct
Cyc_String_pa_struct _tmp760;struct Cyc_String_pa_struct _tmp75F;void*_tmp1D1=(
_tmp75F.tag=0,((_tmp75F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp760.tag=0,((_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp761[0]=& _tmp760,((_tmp761[1]=& _tmp75F,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp762="type mismatch: %s != %s",_tag_dyneither(_tmp762,sizeof(
char),24))),_tag_dyneither(_tmp761,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1D1;}}else{enum Cyc_Absyn_Primop
_tmp1D6=(enum Cyc_Absyn_Primop)po->v;void*_tmp1D7=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp1D6,e1,e2);if(!(Cyc_Tcutil_unify(_tmp1D7,t1) || Cyc_Tcutil_coerceable(
_tmp1D7) && Cyc_Tcutil_coerceable(t1))){const char*_tmp767;void*_tmp766[2];struct
Cyc_String_pa_struct _tmp765;struct Cyc_String_pa_struct _tmp764;void*_tmp1D8=(
_tmp764.tag=0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp766[0]=& _tmp765,((_tmp766[1]=& _tmp764,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp767="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp767,sizeof(char),82))),_tag_dyneither(_tmp766,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp1D7,t1);Cyc_Tcutil_explain_failure();return _tmp1D8;}return
_tmp1D7;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp1DD=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp1E0;enum Cyc_Absyn_Size_of _tmp1E2;_LL109: {struct
Cyc_Absyn_FloatType_struct*_tmp1DE=(struct Cyc_Absyn_FloatType_struct*)_tmp1DD;
if(_tmp1DE->tag != 7)goto _LL10B;}_LL10A: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL108;_LL10B: {struct Cyc_Absyn_IntType_struct*
_tmp1DF=(struct Cyc_Absyn_IntType_struct*)_tmp1DD;if(_tmp1DF->tag != 6)goto _LL10D;
else{_tmp1E0=_tmp1DF->f2;if(_tmp1E0 != Cyc_Absyn_Char_sz)goto _LL10D;}}_LL10C: goto
_LL10E;_LL10D: {struct Cyc_Absyn_IntType_struct*_tmp1E1=(struct Cyc_Absyn_IntType_struct*)
_tmp1DD;if(_tmp1E1->tag != 6)goto _LL10F;else{_tmp1E2=_tmp1E1->f2;if(_tmp1E2 != Cyc_Absyn_Short_sz)
goto _LL10F;}}_LL10E: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;_LL108:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp1E4;struct Cyc_List_List*
_tmp1E5;struct Cyc_Position_Segment*_tmp1E6;enum Cyc_Absyn_Scope _tmp1E7;struct Cyc_Absyn_Datatypefield
_tmp1E3=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E4=_tmp1E3.name;
_tmp1E5=_tmp1E3.typs;_tmp1E6=_tmp1E3.loc;_tmp1E7=_tmp1E3.sc;if(_tmp1E5 == 0  || 
_tmp1E5->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple18*)_tmp1E5->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_Position_Segment*
_tmp1EB;enum Cyc_Absyn_Scope _tmp1EC;struct Cyc_Absyn_Datatypefield _tmp1E8=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E9=_tmp1E8.name;_tmp1EA=_tmp1E8.typs;
_tmp1EB=_tmp1E8.loc;_tmp1EC=_tmp1E8.sc;if(_tmp1EA == 0  || _tmp1EA->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp1EA->hd)).f2);
int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp76C;void*_tmp76B[2];
struct Cyc_String_pa_struct _tmp76A;struct Cyc_String_pa_struct _tmp769;(_tmp769.tag=
0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp76A.tag=0,((_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp76B[0]=& _tmp76A,((_tmp76B[1]=& _tmp769,Cyc_Tcutil_terr(
e->loc,((_tmp76C="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp76C,sizeof(char),54))),_tag_dyneither(_tmp76B,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp1F2=args;int _tmp1F3=0;struct _RegionHandle*_tmp1F4=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp1F5=Cyc_Tcenv_new_block(_tmp1F4,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp1F5,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp1F6=t;struct Cyc_Absyn_PtrInfo _tmp1F8;void*
_tmp1F9;struct Cyc_Absyn_Tqual _tmp1FA;struct Cyc_Absyn_PtrAtts _tmp1FB;void*_tmp1FC;
union Cyc_Absyn_Constraint*_tmp1FD;union Cyc_Absyn_Constraint*_tmp1FE;union Cyc_Absyn_Constraint*
_tmp1FF;_LL112: {struct Cyc_Absyn_PointerType_struct*_tmp1F7=(struct Cyc_Absyn_PointerType_struct*)
_tmp1F6;if(_tmp1F7->tag != 5)goto _LL114;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F8.elt_typ;
_tmp1FA=_tmp1F8.elt_tq;_tmp1FB=_tmp1F8.ptr_atts;_tmp1FC=_tmp1FB.rgn;_tmp1FD=
_tmp1FB.nullable;_tmp1FE=_tmp1FB.bounds;_tmp1FF=_tmp1FB.zero_term;}}_LL113: Cyc_Tcenv_check_rgn_accessible(
_tmp1F5,loc,_tmp1FC);Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FE);{void*_tmp200=
Cyc_Tcutil_compress(_tmp1F9);struct Cyc_Absyn_FnInfo _tmp202;struct Cyc_List_List*
_tmp203;struct Cyc_Core_Opt*_tmp204;void*_tmp205;struct Cyc_List_List*_tmp206;int
_tmp207;struct Cyc_Absyn_VarargInfo*_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_List_List*
_tmp20A;_LL117: {struct Cyc_Absyn_FnType_struct*_tmp201=(struct Cyc_Absyn_FnType_struct*)
_tmp200;if(_tmp201->tag != 10)goto _LL119;else{_tmp202=_tmp201->f1;_tmp203=_tmp202.tvars;
_tmp204=_tmp202.effect;_tmp205=_tmp202.ret_typ;_tmp206=_tmp202.args;_tmp207=
_tmp202.c_varargs;_tmp208=_tmp202.cyc_varargs;_tmp209=_tmp202.rgn_po;_tmp20A=
_tmp202.attributes;}}_LL118: if(topt != 0)Cyc_Tcutil_unify(_tmp205,*topt);while(
_tmp1F2 != 0  && _tmp206 != 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp1F2->hd;void*t2=(*((struct _tuple8*)_tmp206->hd)).f3;Cyc_Tcexp_tcExp(
_tmp1F5,(void**)& t2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1F5,e1,t2,& alias_coercion)){{
const char*_tmp771;void*_tmp770[2];struct Cyc_String_pa_struct _tmp76F;struct Cyc_String_pa_struct
_tmp76E;(_tmp76E.tag=0,((_tmp76E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp76F.tag=0,((_tmp76F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp770[0]=& _tmp76F,((_tmp770[1]=& _tmp76E,Cyc_Tcutil_terr(
e1->loc,((_tmp771="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp771,sizeof(char),57))),_tag_dyneither(_tmp770,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp772;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp772=_cycalloc(sizeof(*_tmp772)),((_tmp772->hd=(void*)_tmp1F3,((_tmp772->tl=*
alias_arg_exps,_tmp772))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1F4,
t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F4,e1)){const char*_tmp775;void*_tmp774;(
_tmp774=0,Cyc_Tcutil_terr(e1->loc,((_tmp775="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp775,sizeof(char),49))),_tag_dyneither(_tmp774,sizeof(void*),0)));}
_tmp1F2=_tmp1F2->tl;_tmp206=_tmp206->tl;++ _tmp1F3;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp20A;for(0;a != 0;a=a->tl){void*_tmp212=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp214;int _tmp215;int _tmp216;_LL11C: {struct Cyc_Absyn_Format_att_struct*
_tmp213=(struct Cyc_Absyn_Format_att_struct*)_tmp212;if(_tmp213->tag != 19)goto
_LL11E;else{_tmp214=_tmp213->f1;_tmp215=_tmp213->f2;_tmp216=_tmp213->f3;}}_LL11D:{
struct _handler_cons _tmp217;_push_handler(& _tmp217);{int _tmp219=0;if(setjmp(
_tmp217.handler))_tmp219=1;if(!_tmp219){{struct Cyc_Absyn_Exp*_tmp21A=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp215 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp216 == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp776;fmt_args=((
_tmp776=_cycalloc(sizeof(*_tmp776)),((_tmp776->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp216 - 1),_tmp776))));}
args_already_checked=1;{struct _RegionHandle*_tmp21C=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp214){case Cyc_Absyn_Printf_ft: _LL120: Cyc_Tcexp_check_format_args(
_tmp1F5,_tmp21A,fmt_args,_tmp216 - 1,alias_arg_exps,_tmp21C,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL121: Cyc_Tcexp_check_format_args(_tmp1F5,_tmp21A,
fmt_args,_tmp216 - 1,alias_arg_exps,_tmp21C,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp218=(void*)_exn_thrown;void*_tmp21E=_tmp218;_LL124: {
struct Cyc_List_Nth_struct*_tmp21F=(struct Cyc_List_Nth_struct*)_tmp21E;if(_tmp21F->tag
!= Cyc_List_Nth)goto _LL126;}_LL125:{const char*_tmp779;void*_tmp778;(_tmp778=0,
Cyc_Tcutil_terr(loc,((_tmp779="bad format arguments",_tag_dyneither(_tmp779,
sizeof(char),21))),_tag_dyneither(_tmp778,sizeof(void*),0)));}goto _LL123;_LL126:;
_LL127:(void)_throw(_tmp21E);_LL123:;}};}goto _LL11B;_LL11E:;_LL11F: goto _LL11B;
_LL11B:;}}if(_tmp206 != 0){const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_terr(
loc,((_tmp77C="too few arguments for function",_tag_dyneither(_tmp77C,sizeof(
char),31))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}else{if((_tmp1F2 != 0  || 
_tmp207) || _tmp208 != 0){if(_tmp207)for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){Cyc_Tcexp_tcExp(
_tmp1F5,0,(struct Cyc_Absyn_Exp*)_tmp1F2->hd);}else{if(_tmp208 == 0){const char*
_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_Tcutil_terr(loc,((_tmp77F="too many arguments for function",
_tag_dyneither(_tmp77F,sizeof(char),32))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}
else{void*_tmp227;int _tmp228;struct Cyc_Absyn_VarargInfo _tmp226=*_tmp208;_tmp227=
_tmp226.type;_tmp228=_tmp226.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp780;
struct Cyc_Absyn_VarargCallInfo*_tmp229=(_tmp780=_cycalloc(sizeof(*_tmp780)),((
_tmp780->num_varargs=0,((_tmp780->injectors=0,((_tmp780->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp208,_tmp780)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp229;if(!_tmp228)for(0;_tmp1F2 != 0;(_tmp1F2=_tmp1F2->tl,_tmp1F3 ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1F2->hd;++
_tmp229->num_varargs;Cyc_Tcexp_tcExp(_tmp1F5,(void**)& _tmp227,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp1F5,e1,_tmp227,& alias_coercion)){{const char*_tmp785;void*_tmp784[2];struct
Cyc_String_pa_struct _tmp783;struct Cyc_String_pa_struct _tmp782;(_tmp782.tag=0,((
_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp783.tag=0,((
_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp227)),((_tmp784[0]=& _tmp783,((_tmp784[1]=& _tmp782,Cyc_Tcutil_terr(loc,((
_tmp785="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp785,sizeof(char),49))),_tag_dyneither(_tmp784,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp786;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp786=_cycalloc(sizeof(*_tmp786)),((
_tmp786->hd=(void*)_tmp1F3,((_tmp786->tl=*alias_arg_exps,_tmp786))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F4,_tmp227) && !Cyc_Tcutil_is_noalias_path(_tmp1F4,e1)){const char*_tmp789;
void*_tmp788;(_tmp788=0,Cyc_Tcutil_terr(e1->loc,((_tmp789="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp789,sizeof(char),49))),_tag_dyneither(_tmp788,sizeof(void*),0)));}}
else{void*_tmp231=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp227));
struct Cyc_Absyn_DatatypeInfo _tmp233;union Cyc_Absyn_DatatypeInfoU _tmp234;struct
Cyc_Absyn_Datatypedecl**_tmp235;struct Cyc_Absyn_Datatypedecl*_tmp236;struct Cyc_List_List*
_tmp237;_LL129: {struct Cyc_Absyn_DatatypeType_struct*_tmp232=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp231;if(_tmp232->tag != 3)goto _LL12B;else{_tmp233=_tmp232->f1;_tmp234=_tmp233.datatype_info;
if((_tmp234.KnownDatatype).tag != 2)goto _LL12B;_tmp235=(struct Cyc_Absyn_Datatypedecl**)(
_tmp234.KnownDatatype).val;_tmp236=*_tmp235;_tmp237=_tmp233.targs;}}_LL12A: {
struct Cyc_Absyn_Datatypedecl*_tmp238=*Cyc_Tcenv_lookup_datatypedecl(_tmp1F5,loc,
_tmp236->name);struct Cyc_List_List*fields=0;if(_tmp238->fields == 0){const char*
_tmp78D;void*_tmp78C[1];struct Cyc_String_pa_struct _tmp78B;(_tmp78B.tag=0,((
_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp227)),((_tmp78C[0]=& _tmp78B,Cyc_Tcutil_terr(loc,((_tmp78D="can't inject into abstract datatype %s",
_tag_dyneither(_tmp78D,sizeof(char),39))),_tag_dyneither(_tmp78C,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp238->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp227),Cyc_Tcenv_curr_rgn(
_tmp1F5))){const char*_tmp792;void*_tmp791[2];struct Cyc_String_pa_struct _tmp790;
struct Cyc_String_pa_struct _tmp78F;(_tmp78F.tag=0,((_tmp78F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp1F5))),((_tmp790.tag=0,((_tmp790.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp227)),((_tmp791[0]=& _tmp790,((_tmp791[
1]=& _tmp78F,Cyc_Tcutil_terr(loc,((_tmp792="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp792,sizeof(char),49))),_tag_dyneither(_tmp791,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp240=Cyc_Tcenv_get_fnrgn(_tmp1F5);{struct Cyc_List_List*
_tmp241=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp240,_tmp240,
_tmp238->tvs,_tmp237);for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){++ _tmp229->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1F2->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp1F5,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp240,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp240,e1)){const char*_tmp795;void*_tmp794;(_tmp794=0,Cyc_Tcutil_terr(e1->loc,((
_tmp795="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp795,sizeof(char),49))),_tag_dyneither(_tmp794,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp244=Cyc_Tcexp_tcInjection(_tmp1F5,e1,Cyc_Tcutil_pointer_elt_type(
_tmp227),_tmp240,_tmp241,fields);if(_tmp244 != 0){struct Cyc_List_List*_tmp796;
_tmp229->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp229->injectors,((_tmp796=_cycalloc(sizeof(*_tmp796)),((
_tmp796->hd=(struct Cyc_Absyn_Datatypefield*)_tmp244,((_tmp796->tl=0,_tmp796)))))));}};};}}
goto _LL128;};}_LL12B:;_LL12C:{const char*_tmp799;void*_tmp798;(_tmp798=0,Cyc_Tcutil_terr(
loc,((_tmp799="bad inject vararg type",_tag_dyneither(_tmp799,sizeof(char),23))),
_tag_dyneither(_tmp798,sizeof(void*),0)));}goto _LL128;_LL128:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp1F5,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp204))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp1F5,loc,_tmp209);return _tmp205;};_LL119:;_LL11A: {const char*_tmp79C;void*
_tmp79B;return(_tmp79B=0,Cyc_Tcexp_expr_err(_tmp1F5,loc,topt,((_tmp79C="expected pointer to function",
_tag_dyneither(_tmp79C,sizeof(char),29))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}
_LL116:;};_LL114:;_LL115: {const char*_tmp79F;void*_tmp79E;return(_tmp79E=0,Cyc_Tcexp_expr_err(
_tmp1F5,loc,topt,((_tmp79F="expected pointer to function",_tag_dyneither(_tmp79F,
sizeof(char),29))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}_LL111:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp24D=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp24D,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp24D,&
bogus)){const char*_tmp7A4;void*_tmp7A3[2];struct Cyc_String_pa_struct _tmp7A2;
struct Cyc_String_pa_struct _tmp7A1;(_tmp7A1.tag=0,((_tmp7A1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D)),((_tmp7A3[0]=& _tmp7A2,((
_tmp7A3[1]=& _tmp7A1,Cyc_Tcutil_terr(loc,((_tmp7A4="expected %s but found %s",
_tag_dyneither(_tmp7A4,sizeof(char),25))),_tag_dyneither(_tmp7A3,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp252=t1;struct Cyc_Absyn_PtrInfo _tmp254;void*
_tmp255;struct Cyc_Absyn_Tqual _tmp256;struct Cyc_Absyn_PtrAtts _tmp257;void*_tmp258;
union Cyc_Absyn_Constraint*_tmp259;union Cyc_Absyn_Constraint*_tmp25A;union Cyc_Absyn_Constraint*
_tmp25B;_LL12E: {struct Cyc_Absyn_PointerType_struct*_tmp253=(struct Cyc_Absyn_PointerType_struct*)
_tmp252;if(_tmp253->tag != 5)goto _LL130;else{_tmp254=_tmp253->f1;_tmp255=_tmp254.elt_typ;
_tmp256=_tmp254.elt_tq;_tmp257=_tmp254.ptr_atts;_tmp258=_tmp257.rgn;_tmp259=
_tmp257.nullable;_tmp25A=_tmp257.bounds;_tmp25B=_tmp257.zero_term;}}_LL12F:{void*
_tmp25C=Cyc_Tcutil_compress(_tmp255);struct Cyc_Absyn_FnInfo _tmp25E;struct Cyc_List_List*
_tmp25F;struct Cyc_Core_Opt*_tmp260;void*_tmp261;struct Cyc_List_List*_tmp262;int
_tmp263;struct Cyc_Absyn_VarargInfo*_tmp264;struct Cyc_List_List*_tmp265;struct Cyc_List_List*
_tmp266;_LL133: {struct Cyc_Absyn_FnType_struct*_tmp25D=(struct Cyc_Absyn_FnType_struct*)
_tmp25C;if(_tmp25D->tag != 10)goto _LL135;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25E.tvars;
_tmp260=_tmp25E.effect;_tmp261=_tmp25E.ret_typ;_tmp262=_tmp25E.args;_tmp263=
_tmp25E.c_varargs;_tmp264=_tmp25E.cyc_varargs;_tmp265=_tmp25E.rgn_po;_tmp266=
_tmp25E.attributes;}}_LL134: {struct _RegionHandle*_tmp267=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp25F != 0;(ts=ts->tl,
_tmp25F=_tmp25F->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp25F->hd,& Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);{struct _tuple12*_tmp7A7;struct Cyc_List_List*_tmp7A6;
instantiation=((_tmp7A6=_region_malloc(_tmp267,sizeof(*_tmp7A6)),((_tmp7A6->hd=((
_tmp7A7=_region_malloc(_tmp267,sizeof(*_tmp7A7)),((_tmp7A7->f1=(struct Cyc_Absyn_Tvar*)
_tmp25F->hd,((_tmp7A7->f2=(void*)ts->hd,_tmp7A7)))))),((_tmp7A6->tl=
instantiation,_tmp7A6))))));};}if(ts != 0){const char*_tmp7AA;void*_tmp7A9;return(
_tmp7A9=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7AA="too many type variables in instantiation",
_tag_dyneither(_tmp7AA,sizeof(char),41))),_tag_dyneither(_tmp7A9,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp7B0;struct Cyc_Absyn_FnInfo _tmp7AF;struct Cyc_Absyn_FnType_struct*
_tmp7AE;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp267,instantiation,(void*)((
_tmp7AE=_cycalloc(sizeof(*_tmp7AE)),((_tmp7AE[0]=((_tmp7B0.tag=10,((_tmp7B0.f1=((
_tmp7AF.tvars=_tmp25F,((_tmp7AF.effect=_tmp260,((_tmp7AF.ret_typ=_tmp261,((
_tmp7AF.args=_tmp262,((_tmp7AF.c_varargs=_tmp263,((_tmp7AF.cyc_varargs=_tmp264,((
_tmp7AF.rgn_po=_tmp265,((_tmp7AF.attributes=_tmp266,_tmp7AF)))))))))))))))),
_tmp7B0)))),_tmp7AE)))));struct Cyc_Absyn_PointerType_struct _tmp7BA;struct Cyc_Absyn_PtrAtts
_tmp7B9;struct Cyc_Absyn_PtrInfo _tmp7B8;struct Cyc_Absyn_PointerType_struct*
_tmp7B7;return(void*)((_tmp7B7=_cycalloc(sizeof(*_tmp7B7)),((_tmp7B7[0]=((
_tmp7BA.tag=5,((_tmp7BA.f1=((_tmp7B8.elt_typ=new_fn_typ,((_tmp7B8.elt_tq=_tmp256,((
_tmp7B8.ptr_atts=((_tmp7B9.rgn=_tmp258,((_tmp7B9.nullable=_tmp259,((_tmp7B9.bounds=
_tmp25A,((_tmp7B9.zero_term=_tmp25B,((_tmp7B9.ptrloc=0,_tmp7B9)))))))))),_tmp7B8)))))),
_tmp7BA)))),_tmp7B7))));};}_LL135:;_LL136: goto _LL132;_LL132:;}goto _LL12D;_LL130:;
_LL131: goto _LL12D;_LL12D:;}{const char*_tmp7BE;void*_tmp7BD[1];struct Cyc_String_pa_struct
_tmp7BC;return(_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7BD[0]=& _tmp7BC,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7BE="expecting polymorphic type but found %s",_tag_dyneither(
_tmp7BE,sizeof(char),40))),_tag_dyneither(_tmp7BD,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcexp_tcExp(te,(
void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(
Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(
c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,
loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp7C3;void*_tmp7C2[2];struct Cyc_String_pa_struct _tmp7C1;struct Cyc_String_pa_struct
_tmp7C0;void*_tmp276=(_tmp7C0.tag=0,((_tmp7C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7C1.tag=0,((_tmp7C1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7C2[0]=&
_tmp7C1,((_tmp7C2[1]=& _tmp7C0,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp7C3="cannot cast %s to %s",
_tag_dyneither(_tmp7C3,sizeof(char),21))),_tag_dyneither(_tmp7C2,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp276;};}}}{struct _tuple0 _tmp7C4;struct
_tuple0 _tmp27C=(_tmp7C4.f1=e->r,((_tmp7C4.f2=Cyc_Tcutil_compress(t),_tmp7C4)));
void*_tmp27D;struct Cyc_Absyn_MallocInfo _tmp27F;int _tmp280;void*_tmp281;struct Cyc_Absyn_PtrInfo
_tmp283;struct Cyc_Absyn_PtrAtts _tmp284;union Cyc_Absyn_Constraint*_tmp285;union
Cyc_Absyn_Constraint*_tmp286;union Cyc_Absyn_Constraint*_tmp287;_LL138: _tmp27D=
_tmp27C.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp27E=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp27D;if(_tmp27E->tag != 34)goto _LL13A;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27F.fat_result;}};
_tmp281=_tmp27C.f2;{struct Cyc_Absyn_PointerType_struct*_tmp282=(struct Cyc_Absyn_PointerType_struct*)
_tmp281;if(_tmp282->tag != 5)goto _LL13A;else{_tmp283=_tmp282->f1;_tmp284=_tmp283.ptr_atts;
_tmp285=_tmp284.nullable;_tmp286=_tmp284.bounds;_tmp287=_tmp284.zero_term;}};
_LL139: if((_tmp280  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp287)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp285)){void*_tmp288=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp286);struct Cyc_Absyn_Exp*_tmp28A;_LL13D: {struct Cyc_Absyn_Upper_b_struct*
_tmp289=(struct Cyc_Absyn_Upper_b_struct*)_tmp288;if(_tmp289->tag != 1)goto _LL13F;
else{_tmp28A=_tmp289->f1;}}_LL13E: if((Cyc_Evexp_eval_const_uint_exp(_tmp28A)).f1
== 1){const char*_tmp7C7;void*_tmp7C6;(_tmp7C6=0,Cyc_Tcutil_warn(loc,((_tmp7C7="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp7C7,sizeof(char),60))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}
goto _LL13C;_LL13F:;_LL140: goto _LL13C;_LL13C:;}goto _LL137;_LL13A:;_LL13B: goto
_LL137;_LL137:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp28D=0;struct Cyc_Absyn_Tqual _tmp28E=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp28F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp291;
void*_tmp292;struct Cyc_Absyn_Tqual _tmp293;struct Cyc_Absyn_PtrAtts _tmp294;union
Cyc_Absyn_Constraint*_tmp295;_LL142: {struct Cyc_Absyn_PointerType_struct*_tmp290=(
struct Cyc_Absyn_PointerType_struct*)_tmp28F;if(_tmp290->tag != 5)goto _LL144;else{
_tmp291=_tmp290->f1;_tmp292=_tmp291.elt_typ;_tmp293=_tmp291.elt_tq;_tmp294=
_tmp291.ptr_atts;_tmp295=_tmp294.zero_term;}}_LL143:{void**_tmp7C8;_tmp28D=((
_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=_tmp292,_tmp7C8))));}_tmp28E=
_tmp293;goto _LL141;_LL144:;_LL145: goto _LL141;_LL141:;}{struct _RegionHandle*
_tmp297=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp297,te),_tmp28D,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp297,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*_tmp7CB;void*_tmp7CA;(
_tmp7CA=0,Cyc_Tcutil_terr(e->loc,((_tmp7CB="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp7CB,sizeof(char),46))),_tag_dyneither(_tmp7CA,sizeof(void*),0)));}{
void*_tmp29A=e->r;struct Cyc_Absyn_Exp*_tmp29C;struct Cyc_Absyn_Exp*_tmp29D;_LL147: {
struct Cyc_Absyn_Subscript_e_struct*_tmp29B=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp29A;if(_tmp29B->tag != 24)goto _LL149;else{_tmp29C=_tmp29B->f1;_tmp29D=_tmp29B->f2;}}
_LL148:{void*_tmp29E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp29C->topt))->v);_LL14C: {struct Cyc_Absyn_TupleType_struct*
_tmp29F=(struct Cyc_Absyn_TupleType_struct*)_tmp29E;if(_tmp29F->tag != 11)goto
_LL14E;}_LL14D: goto _LL14B;_LL14E:;_LL14F: e0->r=(Cyc_Absyn_add_exp(_tmp29C,
_tmp29D,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp29C,_tmp29D);_LL14B:;}goto _LL146;
_LL149:;_LL14A: goto _LL146;_LL146:;}{void*_tmp2A0=e->r;int _tmp2A2;int _tmp2A4;
_LL151: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2A1=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2A0;if(_tmp2A1->tag != 22)goto _LL153;else{_tmp2A2=_tmp2A1->f3;if(_tmp2A2 != 1)
goto _LL153;}}_LL152: goto _LL154;_LL153: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2A3=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2A0;if(_tmp2A3->tag != 23)goto
_LL155;else{_tmp2A4=_tmp2A3->f3;if(_tmp2A4 != 1)goto _LL155;}}_LL154:{const char*
_tmp7CE;void*_tmp7CD;(_tmp7CD=0,Cyc_Tcutil_terr(e->loc,((_tmp7CE="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp7CE,sizeof(char),50))),_tag_dyneither(_tmp7CD,sizeof(void*),0)));}
goto _LL150;_LL155:;_LL156: goto _LL150;_LL150:;}{int _tmp2A8;void*_tmp2A9;struct
_tuple13 _tmp2A7=Cyc_Tcutil_addressof_props(te,e);_tmp2A8=_tmp2A7.f1;_tmp2A9=
_tmp2A7.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2A8){tq.print_const=
1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp2A9,tq,Cyc_Absyn_false_conref);return t;};};};};}
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp7D2;void*_tmp7D1[1];struct Cyc_String_pa_struct _tmp7D0;(_tmp7D0.tag=
0,((_tmp7D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp7D1[0]=& _tmp7D0,Cyc_Tcutil_terr(loc,((_tmp7D2="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp7D2,sizeof(char),55))),_tag_dyneither(_tmp7D1,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2AD=Cyc_Tcutil_compress(*topt);void*_tmp2AF;_LL158: {
struct Cyc_Absyn_TagType_struct*_tmp2AE=(struct Cyc_Absyn_TagType_struct*)_tmp2AD;
if(_tmp2AE->tag != 20)goto _LL15A;else{_tmp2AF=(void*)_tmp2AE->f1;}}_LL159: {
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp7D5;struct Cyc_Absyn_ValueofType_struct*_tmp7D4;struct Cyc_Absyn_ValueofType_struct*
_tmp2B1=(_tmp7D4=_cycalloc(sizeof(*_tmp7D4)),((_tmp7D4[0]=((_tmp7D5.tag=19,((
_tmp7D5.f1=_tmp2B0,_tmp7D5)))),_tmp7D4)));if(Cyc_Tcutil_unify(_tmp2AF,(void*)
_tmp2B1))return _tmp2AD;goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);{void*_tmp2B4=n;struct _dyneither_ptr*_tmp2B6;
unsigned int _tmp2B8;_LL15D: {struct Cyc_Absyn_StructField_struct*_tmp2B5=(struct
Cyc_Absyn_StructField_struct*)_tmp2B4;if(_tmp2B5->tag != 0)goto _LL15F;else{
_tmp2B6=_tmp2B5->f1;}}_LL15E: {int bad_type=1;{void*_tmp2B9=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_AggrInfo _tmp2BB;union Cyc_Absyn_AggrInfoU _tmp2BC;struct Cyc_Absyn_Aggrdecl**
_tmp2BD;struct Cyc_List_List*_tmp2BF;_LL162: {struct Cyc_Absyn_AggrType_struct*
_tmp2BA=(struct Cyc_Absyn_AggrType_struct*)_tmp2B9;if(_tmp2BA->tag != 12)goto
_LL164;else{_tmp2BB=_tmp2BA->f1;_tmp2BC=_tmp2BB.aggr_info;if((_tmp2BC.KnownAggr).tag
!= 2)goto _LL164;_tmp2BD=(struct Cyc_Absyn_Aggrdecl**)(_tmp2BC.KnownAggr).val;}}
_LL163: if((*_tmp2BD)->impl == 0)goto _LL161;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2B6,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2BD)->impl))->fields)){const char*_tmp7D9;void*_tmp7D8[1];
struct Cyc_String_pa_struct _tmp7D7;(_tmp7D7.tag=0,((_tmp7D7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2B6),((_tmp7D8[0]=& _tmp7D7,Cyc_Tcutil_terr(
loc,((_tmp7D9="no field of struct/union has name %s",_tag_dyneither(_tmp7D9,
sizeof(char),37))),_tag_dyneither(_tmp7D8,sizeof(void*),1)))))));}bad_type=0;
goto _LL161;_LL164: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2BE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2B9;if(_tmp2BE->tag != 13)goto _LL166;else{_tmp2BF=_tmp2BE->f2;}}_LL165: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2B6,_tmp2BF)){const char*_tmp7DD;void*_tmp7DC[1];struct Cyc_String_pa_struct
_tmp7DB;(_tmp7DB.tag=0,((_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2B6),((_tmp7DC[0]=& _tmp7DB,Cyc_Tcutil_terr(loc,((_tmp7DD="no field of struct/union has name %s",
_tag_dyneither(_tmp7DD,sizeof(char),37))),_tag_dyneither(_tmp7DC,sizeof(void*),1)))))));}
bad_type=0;goto _LL161;_LL166:;_LL167: goto _LL161;_LL161:;}if(bad_type){const char*
_tmp7E1;void*_tmp7E0[1];struct Cyc_String_pa_struct _tmp7DF;(_tmp7DF.tag=0,((
_tmp7DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp7E0[0]=& _tmp7DF,Cyc_Tcutil_terr(loc,((_tmp7E1="%s is not a known struct/union type",
_tag_dyneither(_tmp7E1,sizeof(char),36))),_tag_dyneither(_tmp7E0,sizeof(void*),1)))))));}
goto _LL15C;}_LL15F: {struct Cyc_Absyn_TupleIndex_struct*_tmp2B7=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2B4;if(_tmp2B7->tag != 1)goto _LL15C;else{_tmp2B8=_tmp2B7->f1;}}_LL160: {int
bad_type=1;{void*_tmp2C9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2CB;
union Cyc_Absyn_AggrInfoU _tmp2CC;struct Cyc_Absyn_Aggrdecl**_tmp2CD;struct Cyc_List_List*
_tmp2CF;struct Cyc_List_List*_tmp2D1;struct Cyc_Absyn_DatatypeFieldInfo _tmp2D3;
union Cyc_Absyn_DatatypeFieldInfoU _tmp2D4;struct _tuple2 _tmp2D5;struct Cyc_Absyn_Datatypefield*
_tmp2D6;_LL169: {struct Cyc_Absyn_AggrType_struct*_tmp2CA=(struct Cyc_Absyn_AggrType_struct*)
_tmp2C9;if(_tmp2CA->tag != 12)goto _LL16B;else{_tmp2CB=_tmp2CA->f1;_tmp2CC=_tmp2CB.aggr_info;
if((_tmp2CC.KnownAggr).tag != 2)goto _LL16B;_tmp2CD=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2CC.KnownAggr).val;}}_LL16A: if((*_tmp2CD)->impl == 0)goto _LL168;_tmp2CF=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2CD)->impl))->fields;goto _LL16C;
_LL16B: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2CE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C9;if(_tmp2CE->tag != 13)goto _LL16D;else{_tmp2CF=_tmp2CE->f2;}}_LL16C: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2CF)<= _tmp2B8){const char*
_tmp7E6;void*_tmp7E5[2];struct Cyc_Int_pa_struct _tmp7E4;struct Cyc_Int_pa_struct
_tmp7E3;(_tmp7E3.tag=1,((_tmp7E3.f1=(unsigned long)((int)_tmp2B8),((_tmp7E4.tag=
1,((_tmp7E4.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2CF),((_tmp7E5[0]=& _tmp7E4,((_tmp7E5[1]=& _tmp7E3,Cyc_Tcutil_terr(loc,((
_tmp7E6="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp7E6,
sizeof(char),46))),_tag_dyneither(_tmp7E5,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL168;_LL16D: {struct Cyc_Absyn_TupleType_struct*_tmp2D0=(struct Cyc_Absyn_TupleType_struct*)
_tmp2C9;if(_tmp2D0->tag != 11)goto _LL16F;else{_tmp2D1=_tmp2D0->f1;}}_LL16E: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D1)<= _tmp2B8){const char*
_tmp7EB;void*_tmp7EA[2];struct Cyc_Int_pa_struct _tmp7E9;struct Cyc_Int_pa_struct
_tmp7E8;(_tmp7E8.tag=1,((_tmp7E8.f1=(unsigned long)((int)_tmp2B8),((_tmp7E9.tag=
1,((_tmp7E9.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2D1),((_tmp7EA[0]=& _tmp7E9,((_tmp7EA[1]=& _tmp7E8,Cyc_Tcutil_terr(loc,((
_tmp7EB="tuple has too few components: %d <= %d",_tag_dyneither(_tmp7EB,sizeof(
char),39))),_tag_dyneither(_tmp7EA,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL168;_LL16F: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2D2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2C9;if(_tmp2D2->tag != 4)goto _LL171;else{_tmp2D3=_tmp2D2->f1;_tmp2D4=_tmp2D3.field_info;
if((_tmp2D4.KnownDatatypefield).tag != 2)goto _LL171;_tmp2D5=(struct _tuple2)(
_tmp2D4.KnownDatatypefield).val;_tmp2D6=_tmp2D5.f2;}}_LL170: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp2D6->typs)< _tmp2B8){const char*_tmp7F0;
void*_tmp7EF[2];struct Cyc_Int_pa_struct _tmp7EE;struct Cyc_Int_pa_struct _tmp7ED;(
_tmp7ED.tag=1,((_tmp7ED.f1=(unsigned long)((int)_tmp2B8),((_tmp7EE.tag=1,((
_tmp7EE.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2D6->typs),((_tmp7EF[0]=& _tmp7EE,((_tmp7EF[1]=& _tmp7ED,Cyc_Tcutil_terr(loc,((
_tmp7F0="datatype field has too few components: %d < %d",_tag_dyneither(_tmp7F0,
sizeof(char),47))),_tag_dyneither(_tmp7EF,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL168;_LL171:;_LL172: goto _LL168;_LL168:;}if(bad_type){const char*_tmp7F4;
void*_tmp7F3[1];struct Cyc_String_pa_struct _tmp7F2;(_tmp7F2.tag=0,((_tmp7F2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7F3[
0]=& _tmp7F2,Cyc_Tcutil_terr(loc,((_tmp7F4="%s is not a known type",
_tag_dyneither(_tmp7F4,sizeof(char),23))),_tag_dyneither(_tmp7F3,sizeof(void*),1)))))));}
goto _LL15C;}_LL15C:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp2E6=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp2E7=Cyc_Tcenv_clear_notreadctxt(_tmp2E6,te_orig);Cyc_Tcexp_tcExp(_tmp2E7,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp2E8=t;struct Cyc_Absyn_PtrInfo _tmp2EB;void*_tmp2EC;struct Cyc_Absyn_PtrAtts
_tmp2ED;void*_tmp2EE;union Cyc_Absyn_Constraint*_tmp2EF;union Cyc_Absyn_Constraint*
_tmp2F0;_LL174: {struct Cyc_Absyn_Evar_struct*_tmp2E9=(struct Cyc_Absyn_Evar_struct*)
_tmp2E8;if(_tmp2E9->tag != 1)goto _LL176;}_LL175: {struct Cyc_List_List*_tmp2F1=Cyc_Tcenv_lookup_type_vars(
_tmp2E7);struct Cyc_Core_Opt*_tmp7F5;void*_tmp2F2=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mko,((_tmp7F5=_cycalloc(sizeof(*_tmp7F5)),((_tmp7F5->v=_tmp2F1,
_tmp7F5)))));struct Cyc_Core_Opt*_tmp7F6;void*_tmp2F3=Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp7F6=_cycalloc(sizeof(*_tmp7F6)),((_tmp7F6->v=
_tmp2F1,_tmp7F6)))));union Cyc_Absyn_Constraint*_tmp2F4=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*_tmp2F5=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp7F7;struct Cyc_Absyn_PtrAtts
_tmp2F6=(_tmp7F7.rgn=_tmp2F3,((_tmp7F7.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp7F7.bounds=_tmp2F4,((_tmp7F7.zero_term=_tmp2F5,((
_tmp7F7.ptrloc=0,_tmp7F7)))))))));struct Cyc_Absyn_PointerType_struct _tmp7FD;
struct Cyc_Absyn_PtrInfo _tmp7FC;struct Cyc_Absyn_PointerType_struct*_tmp7FB;struct
Cyc_Absyn_PointerType_struct*_tmp2F7=(_tmp7FB=_cycalloc(sizeof(*_tmp7FB)),((
_tmp7FB[0]=((_tmp7FD.tag=5,((_tmp7FD.f1=((_tmp7FC.elt_typ=_tmp2F2,((_tmp7FC.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp7FC.ptr_atts=_tmp2F6,_tmp7FC)))))),_tmp7FD)))),
_tmp7FB)));Cyc_Tcutil_unify(t,(void*)_tmp2F7);_tmp2EC=_tmp2F2;_tmp2EE=_tmp2F3;
_tmp2EF=_tmp2F4;_tmp2F0=_tmp2F5;goto _LL177;}_LL176: {struct Cyc_Absyn_PointerType_struct*
_tmp2EA=(struct Cyc_Absyn_PointerType_struct*)_tmp2E8;if(_tmp2EA->tag != 5)goto
_LL178;else{_tmp2EB=_tmp2EA->f1;_tmp2EC=_tmp2EB.elt_typ;_tmp2ED=_tmp2EB.ptr_atts;
_tmp2EE=_tmp2ED.rgn;_tmp2EF=_tmp2ED.bounds;_tmp2F0=_tmp2ED.zero_term;}}_LL177:
Cyc_Tcenv_check_rgn_accessible(_tmp2E7,loc,_tmp2EE);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2EF);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2EC),& Cyc_Tcutil_tmk)){
const char*_tmp800;void*_tmp7FF;(_tmp7FF=0,Cyc_Tcutil_terr(loc,((_tmp800="can't dereference abstract pointer type",
_tag_dyneither(_tmp800,sizeof(char),40))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}
return _tmp2EC;_LL178:;_LL179: {const char*_tmp804;void*_tmp803[1];struct Cyc_String_pa_struct
_tmp802;return(_tmp802.tag=0,((_tmp802.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp803[0]=& _tmp802,Cyc_Tcexp_expr_err(
_tmp2E7,loc,topt,((_tmp804="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp804,sizeof(char),39))),_tag_dyneither(_tmp803,sizeof(void*),1)))))));}
_LL173:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){Cyc_Tcexp_tcExpNoPromote(te,0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp303=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp305;
union Cyc_Absyn_AggrInfoU _tmp306;struct Cyc_Absyn_Aggrdecl**_tmp307;struct Cyc_Absyn_Aggrdecl*
_tmp308;struct Cyc_List_List*_tmp309;enum Cyc_Absyn_AggrKind _tmp30B;struct Cyc_List_List*
_tmp30C;_LL17B: {struct Cyc_Absyn_AggrType_struct*_tmp304=(struct Cyc_Absyn_AggrType_struct*)
_tmp303;if(_tmp304->tag != 12)goto _LL17D;else{_tmp305=_tmp304->f1;_tmp306=_tmp305.aggr_info;
if((_tmp306.KnownAggr).tag != 2)goto _LL17D;_tmp307=(struct Cyc_Absyn_Aggrdecl**)(
_tmp306.KnownAggr).val;_tmp308=*_tmp307;_tmp309=_tmp305.targs;}}_LL17C: {struct
Cyc_Absyn_Aggrfield*_tmp311=Cyc_Absyn_lookup_decl_field(_tmp308,f);if(_tmp311 == 
0){const char*_tmp809;void*_tmp808[2];struct Cyc_String_pa_struct _tmp807;struct Cyc_String_pa_struct
_tmp806;return(_tmp806.tag=0,((_tmp806.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp807.tag=0,((_tmp807.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp308->name)),((_tmp808[0]=& _tmp807,((
_tmp808[1]=& _tmp806,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp809="type %s has no %s field",
_tag_dyneither(_tmp809,sizeof(char),24))),_tag_dyneither(_tmp808,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp316=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp316,_tmp316,_tmp308->tvs,_tmp309);t2=Cyc_Tcutil_rsubstitute(_tmp316,_tmp317,
_tmp311->type);if(((_tmp308->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp308->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp80D;void*_tmp80C[1];struct Cyc_String_pa_struct _tmp80B;return(
_tmp80B.tag=0,((_tmp80B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp80C[0]=& _tmp80B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80D="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp80D,sizeof(char),56))),_tag_dyneither(_tmp80C,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp811;void*_tmp810[1];struct Cyc_String_pa_struct _tmp80F;return(
_tmp80F.tag=0,((_tmp80F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp810[0]=& _tmp80F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp811="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp811,sizeof(char),81))),_tag_dyneither(_tmp810,sizeof(void*),1)))))));}}
return t2;};}_LL17D: {struct Cyc_Absyn_AnonAggrType_struct*_tmp30A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp303;if(_tmp30A->tag != 13)goto _LL17F;else{_tmp30B=_tmp30A->f1;_tmp30C=_tmp30A->f2;}}
_LL17E: {struct Cyc_Absyn_Aggrfield*_tmp31E=Cyc_Absyn_lookup_field(_tmp30C,f);if(
_tmp31E == 0){const char*_tmp815;void*_tmp814[1];struct Cyc_String_pa_struct _tmp813;
return(_tmp813.tag=0,((_tmp813.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp814[0]=& _tmp813,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp815="type has no %s field",
_tag_dyneither(_tmp815,sizeof(char),21))),_tag_dyneither(_tmp814,sizeof(void*),1)))))));}
if((_tmp30B == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp31E->type)){const char*_tmp819;void*_tmp818[1];struct Cyc_String_pa_struct
_tmp817;return(_tmp817.tag=0,((_tmp817.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp818[0]=& _tmp817,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp819="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp819,sizeof(char),56))),_tag_dyneither(_tmp818,sizeof(void*),1)))))));}return
_tmp31E->type;}_LL17F:{struct Cyc_Absyn_ArrayType_struct*_tmp30D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp303;if(_tmp30D->tag != 9)goto _LL181;}{const char*_tmp81A;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp81A="size",_tag_dyneither(_tmp81A,sizeof(char),5))))
== 0))goto _LL181;};_LL180: goto _LL182;_LL181:{struct Cyc_Absyn_PointerType_struct*
_tmp30F=(struct Cyc_Absyn_PointerType_struct*)_tmp303;if(_tmp30F->tag != 5)goto
_LL183;}{const char*_tmp81B;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp81B="size",
_tag_dyneither(_tmp81B,sizeof(char),5))))== 0))goto _LL183;};_LL182:{const char*
_tmp81F;void*_tmp81E[1];struct Cyc_String_pa_struct _tmp81D;(_tmp81D.tag=0,((
_tmp81D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp81E[0]=& _tmp81D,Cyc_Tcutil_warn(e->loc,((_tmp81F="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp81F,sizeof(char),54))),_tag_dyneither(_tmp81E,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp825;struct Cyc_List_List*_tmp824;struct Cyc_Absyn_Primop_e_struct*
_tmp823;outer_e->r=(void*)((_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=((
_tmp825.tag=3,((_tmp825.f1=Cyc_Absyn_Numelts,((_tmp825.f2=((_tmp824=_cycalloc(
sizeof(*_tmp824)),((_tmp824->hd=e,((_tmp824->tl=0,_tmp824)))))),_tmp825)))))),
_tmp823))));}return Cyc_Absyn_uint_typ;_LL183:;_LL184: {const char*_tmp826;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp826="size",_tag_dyneither(_tmp826,sizeof(char),5))))
== 0){const char*_tmp82A;void*_tmp829[1];struct Cyc_String_pa_struct _tmp828;return(
_tmp828.tag=0,((_tmp828.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp829[0]=& _tmp828,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp82A="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp82A,sizeof(char),44))),_tag_dyneither(_tmp829,sizeof(void*),1)))))));}
else{const char*_tmp82E;void*_tmp82D[1];struct Cyc_String_pa_struct _tmp82C;return(
_tmp82C.tag=0,((_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp82D[0]=& _tmp82C,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp82E="expecting struct or union, found %s",_tag_dyneither(
_tmp82E,sizeof(char),36))),_tag_dyneither(_tmp82D,sizeof(void*),1)))))));}}
_LL17A:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp332=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
_tmp332,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp333=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp335;void*_tmp336;struct Cyc_Absyn_PtrAtts _tmp337;void*_tmp338;union Cyc_Absyn_Constraint*
_tmp339;union Cyc_Absyn_Constraint*_tmp33A;_LL186: {struct Cyc_Absyn_PointerType_struct*
_tmp334=(struct Cyc_Absyn_PointerType_struct*)_tmp333;if(_tmp334->tag != 5)goto
_LL188;else{_tmp335=_tmp334->f1;_tmp336=_tmp335.elt_typ;_tmp337=_tmp335.ptr_atts;
_tmp338=_tmp337.rgn;_tmp339=_tmp337.bounds;_tmp33A=_tmp337.zero_term;}}_LL187:
Cyc_Tcutil_check_nonzero_bound(loc,_tmp339);{void*_tmp33B=Cyc_Tcutil_compress(
_tmp336);struct Cyc_Absyn_AggrInfo _tmp33D;union Cyc_Absyn_AggrInfoU _tmp33E;struct
Cyc_Absyn_Aggrdecl**_tmp33F;struct Cyc_Absyn_Aggrdecl*_tmp340;struct Cyc_List_List*
_tmp341;enum Cyc_Absyn_AggrKind _tmp343;struct Cyc_List_List*_tmp344;_LL18B: {
struct Cyc_Absyn_AggrType_struct*_tmp33C=(struct Cyc_Absyn_AggrType_struct*)
_tmp33B;if(_tmp33C->tag != 12)goto _LL18D;else{_tmp33D=_tmp33C->f1;_tmp33E=_tmp33D.aggr_info;
if((_tmp33E.KnownAggr).tag != 2)goto _LL18D;_tmp33F=(struct Cyc_Absyn_Aggrdecl**)(
_tmp33E.KnownAggr).val;_tmp340=*_tmp33F;_tmp341=_tmp33D.targs;}}_LL18C: {struct
Cyc_Absyn_Aggrfield*_tmp345=Cyc_Absyn_lookup_decl_field(_tmp340,f);if(_tmp345 == 
0){const char*_tmp833;void*_tmp832[2];struct Cyc_String_pa_struct _tmp831;struct Cyc_String_pa_struct
_tmp830;return(_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp831.tag=0,((_tmp831.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp340->name)),((_tmp832[0]=& _tmp831,((
_tmp832[1]=& _tmp830,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp833="type %s has no %s field",
_tag_dyneither(_tmp833,sizeof(char),24))),_tag_dyneither(_tmp832,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp340->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp34A=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp34A,_tmp34A,_tmp340->tvs,_tmp341);t3=Cyc_Tcutil_rsubstitute(_tmp34A,_tmp34B,
_tmp345->type);}if(((_tmp340->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp340->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp837;void*_tmp836[1];struct Cyc_String_pa_struct _tmp835;return(
_tmp835.tag=0,((_tmp835.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp836[0]=& _tmp835,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp837="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp837,sizeof(char),56))),_tag_dyneither(_tmp836,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp340->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp83B;void*_tmp83A[1];struct Cyc_String_pa_struct _tmp839;return(
_tmp839.tag=0,((_tmp839.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp83A[0]=& _tmp839,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp83B="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp83B,sizeof(char),72))),_tag_dyneither(_tmp83A,sizeof(void*),1)))))));}}
return t3;};}_LL18D: {struct Cyc_Absyn_AnonAggrType_struct*_tmp342=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp33B;if(_tmp342->tag != 13)goto _LL18F;else{_tmp343=_tmp342->f1;_tmp344=_tmp342->f2;}}
_LL18E: {struct Cyc_Absyn_Aggrfield*_tmp352=Cyc_Absyn_lookup_field(_tmp344,f);if(
_tmp352 == 0){const char*_tmp83F;void*_tmp83E[1];struct Cyc_String_pa_struct _tmp83D;
return(_tmp83D.tag=0,((_tmp83D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp83E[0]=& _tmp83D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp83F="type has no %s field",
_tag_dyneither(_tmp83F,sizeof(char),21))),_tag_dyneither(_tmp83E,sizeof(void*),1)))))));}
if((_tmp343 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp352->type)){const char*_tmp843;void*_tmp842[1];struct Cyc_String_pa_struct
_tmp841;return(_tmp841.tag=0,((_tmp841.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp842[0]=& _tmp841,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp843="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp843,sizeof(char),56))),_tag_dyneither(_tmp842,sizeof(void*),1)))))));}return
_tmp352->type;}_LL18F:;_LL190: goto _LL18A;_LL18A:;}goto _LL185;_LL188:;_LL189: goto
_LL185;_LL185:;}{const char*_tmp847;void*_tmp846[1];struct Cyc_String_pa_struct
_tmp845;return(_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp846[0]=& _tmp845,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp847="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp847,sizeof(char),44))),_tag_dyneither(_tmp846,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp35D;int
_tmp35E;struct _tuple14 _tmp35C=Cyc_Evexp_eval_const_uint_exp(index);_tmp35D=
_tmp35C.f1;_tmp35E=_tmp35C.f2;if(!_tmp35E){const char*_tmp84A;void*_tmp849;return(
_tmp849=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp84A="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp84A,sizeof(char),47))),_tag_dyneither(_tmp849,sizeof(void*),0)));}{
struct _handler_cons _tmp361;_push_handler(& _tmp361);{int _tmp363=0;if(setjmp(
_tmp361.handler))_tmp363=1;if(!_tmp363){{void*_tmp364=(*((struct _tuple18*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp35D)).f2;_npop_handler(0);
return _tmp364;};_pop_handler();}else{void*_tmp362=(void*)_exn_thrown;void*
_tmp366=_tmp362;_LL192: {struct Cyc_List_Nth_struct*_tmp367=(struct Cyc_List_Nth_struct*)
_tmp366;if(_tmp367->tag != Cyc_List_Nth)goto _LL194;}_LL193: {const char*_tmp84F;
void*_tmp84E[2];struct Cyc_Int_pa_struct _tmp84D;struct Cyc_Int_pa_struct _tmp84C;
return(_tmp84C.tag=1,((_tmp84C.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp84D.tag=1,((_tmp84D.f1=(unsigned long)((int)_tmp35D),((
_tmp84E[0]=& _tmp84D,((_tmp84E[1]=& _tmp84C,Cyc_Tcexp_expr_err(te,loc,0,((_tmp84F="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp84F,sizeof(char),41))),_tag_dyneither(_tmp84E,sizeof(void*),2)))))))))))));}
_LL194:;_LL195:(void)_throw(_tmp366);_LL191:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp36C=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp36D=Cyc_Tcenv_clear_notreadctxt(_tmp36C,
te_orig);Cyc_Tcexp_tcExp(_tmp36D,0,e1);Cyc_Tcexp_tcExp(_tmp36D,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp36D,e2)){const char*_tmp853;void*_tmp852[1];
struct Cyc_String_pa_struct _tmp851;return(_tmp851.tag=0,((_tmp851.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp852[0]=&
_tmp851,Cyc_Tcexp_expr_err(_tmp36D,e2->loc,topt,((_tmp853="expecting int subscript, found %s",
_tag_dyneither(_tmp853,sizeof(char),34))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));}{
void*_tmp371=t1;struct Cyc_Absyn_PtrInfo _tmp373;void*_tmp374;struct Cyc_Absyn_Tqual
_tmp375;struct Cyc_Absyn_PtrAtts _tmp376;void*_tmp377;union Cyc_Absyn_Constraint*
_tmp378;union Cyc_Absyn_Constraint*_tmp379;struct Cyc_List_List*_tmp37B;_LL197: {
struct Cyc_Absyn_PointerType_struct*_tmp372=(struct Cyc_Absyn_PointerType_struct*)
_tmp371;if(_tmp372->tag != 5)goto _LL199;else{_tmp373=_tmp372->f1;_tmp374=_tmp373.elt_typ;
_tmp375=_tmp373.elt_tq;_tmp376=_tmp373.ptr_atts;_tmp377=_tmp376.rgn;_tmp378=
_tmp376.bounds;_tmp379=_tmp376.zero_term;}}_LL198: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp379)){int emit_warning=1;{void*_tmp37C=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp378);struct Cyc_Absyn_Exp*_tmp37E;_LL19E: {struct Cyc_Absyn_Upper_b_struct*
_tmp37D=(struct Cyc_Absyn_Upper_b_struct*)_tmp37C;if(_tmp37D->tag != 1)goto _LL1A0;
else{_tmp37E=_tmp37D->f1;}}_LL19F: if(Cyc_Tcutil_is_const_exp(_tmp36D,e2)){
unsigned int _tmp381;int _tmp382;struct _tuple14 _tmp380=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp381=_tmp380.f1;_tmp382=_tmp380.f2;if(_tmp382){unsigned int _tmp384;int
_tmp385;struct _tuple14 _tmp383=Cyc_Evexp_eval_const_uint_exp(_tmp37E);_tmp384=
_tmp383.f1;_tmp385=_tmp383.f2;if(_tmp385  && _tmp384 > _tmp381)emit_warning=0;}}
goto _LL19D;_LL1A0: {struct Cyc_Absyn_DynEither_b_struct*_tmp37F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp37C;if(_tmp37F->tag != 0)goto _LL19D;}_LL1A1: emit_warning=0;goto _LL19D;_LL19D:;}
if(emit_warning){const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_Tcutil_warn(e2->loc,((
_tmp856="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp856,sizeof(char),63))),_tag_dyneither(_tmp855,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp36D,e2)){unsigned int _tmp389;int _tmp38A;
struct _tuple14 _tmp388=Cyc_Evexp_eval_const_uint_exp(e2);_tmp389=_tmp388.f1;
_tmp38A=_tmp388.f2;if(_tmp38A)Cyc_Tcutil_check_bound(loc,_tmp389,_tmp378);}else{
if(Cyc_Tcutil_is_bound_one(_tmp378) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp379)){const char*_tmp859;void*_tmp858;(_tmp858=0,
Cyc_Tcutil_warn(e1->loc,((_tmp859="subscript applied to pointer to one object",
_tag_dyneither(_tmp859,sizeof(char),43))),_tag_dyneither(_tmp858,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp378);}}Cyc_Tcenv_check_rgn_accessible(
_tmp36D,loc,_tmp377);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp374),& Cyc_Tcutil_tmk)){
const char*_tmp85C;void*_tmp85B;(_tmp85B=0,Cyc_Tcutil_terr(e1->loc,((_tmp85C="can't subscript an abstract pointer",
_tag_dyneither(_tmp85C,sizeof(char),36))),_tag_dyneither(_tmp85B,sizeof(void*),0)));}
return _tmp374;_LL199: {struct Cyc_Absyn_TupleType_struct*_tmp37A=(struct Cyc_Absyn_TupleType_struct*)
_tmp371;if(_tmp37A->tag != 11)goto _LL19B;else{_tmp37B=_tmp37A->f1;}}_LL19A: return
Cyc_Tcexp_ithTupleType(_tmp36D,loc,_tmp37B,e2);_LL19B:;_LL19C: {const char*
_tmp860;void*_tmp85F[1];struct Cyc_String_pa_struct _tmp85E;return(_tmp85E.tag=0,((
_tmp85E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp85F[0]=& _tmp85E,Cyc_Tcexp_expr_err(_tmp36D,loc,topt,((_tmp860="subscript applied to %s",
_tag_dyneither(_tmp860,sizeof(char),24))),_tag_dyneither(_tmp85F,sizeof(void*),1)))))));}
_LL196:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp392=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp394;
_LL1A3: {struct Cyc_Absyn_TupleType_struct*_tmp393=(struct Cyc_Absyn_TupleType_struct*)
_tmp392;if(_tmp393->tag != 11)goto _LL1A5;else{_tmp394=_tmp393->f1;}}_LL1A4: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp394)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1A2;for(0;es != 0;(es=es->tl,_tmp394=_tmp394->tl)){
int bogus=0;void*_tmp395=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(
_tmp394))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp395,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)
_tmp394->hd)).f2,& bogus);{struct _tuple18*_tmp863;struct Cyc_List_List*_tmp862;
fields=((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->hd=((_tmp863=_cycalloc(
sizeof(*_tmp863)),((_tmp863->f1=(*((struct _tuple18*)_tmp394->hd)).f1,((_tmp863->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp863)))))),((_tmp862->tl=fields,_tmp862))))));};}done=1;goto _LL1A2;_LL1A5:;
_LL1A6: goto _LL1A2;_LL1A2:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);{struct _tuple18*_tmp866;struct Cyc_List_List*
_tmp865;fields=((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865->hd=((_tmp866=
_cycalloc(sizeof(*_tmp866)),((_tmp866->f1=Cyc_Absyn_empty_tqual(0),((_tmp866->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp866)))))),((_tmp865->tl=fields,_tmp865))))));};}{struct Cyc_Absyn_TupleType_struct
_tmp869;struct Cyc_Absyn_TupleType_struct*_tmp868;return(void*)((_tmp868=
_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=((_tmp869.tag=11,((_tmp869.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp869)))),
_tmp868))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){
const char*_tmp86C;void*_tmp86B;return(_tmp86B=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp86C="tcCompoundLit",_tag_dyneither(_tmp86C,sizeof(char),14))),_tag_dyneither(
_tmp86B,sizeof(void*),0)));}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,struct Cyc_List_List*
des){void*res_t2;struct _RegionHandle*_tmp39E=Cyc_Tcenv_get_fnrgn(te);{int _tmp39F=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp39E,(struct Cyc_Absyn_Exp*(*)(
struct _tuple17*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp86F;struct Cyc_Absyn_Const_e_struct*_tmp86E;struct Cyc_Absyn_Const_e_struct*
_tmp3A0=(_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp86F.tag=0,((
_tmp86F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp39F),_tmp86F)))),_tmp86E)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3A0,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3A1=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp39F - 1);if(!Cyc_Tcutil_is_zero(_tmp3A1)){const char*_tmp872;
void*_tmp871;(_tmp871=0,Cyc_Tcutil_terr(_tmp3A1->loc,((_tmp872="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp872,sizeof(char),45))),_tag_dyneither(_tmp871,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp873;sz_exp->topt=((_tmp873=_cycalloc(sizeof(*_tmp873)),((
_tmp873->v=Cyc_Absyn_uint_typ,_tmp873))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp877;void*_tmp876[1];struct Cyc_String_pa_struct _tmp875;(
_tmp875.tag=0,((_tmp875.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp876[0]=& _tmp875,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp877="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp877,sizeof(char),53))),_tag_dyneither(_tmp876,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple17*)des->hd)).f1;if(ds != 0){void*_tmp3AA=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3AD;_LL1A8: {struct Cyc_Absyn_FieldName_struct*_tmp3AB=(struct Cyc_Absyn_FieldName_struct*)
_tmp3AA;if(_tmp3AB->tag != 1)goto _LL1AA;}_LL1A9:{const char*_tmp87A;void*_tmp879;(
_tmp879=0,Cyc_Tcutil_terr(loc,((_tmp87A="only array index designators are supported",
_tag_dyneither(_tmp87A,sizeof(char),43))),_tag_dyneither(_tmp879,sizeof(void*),0)));}
goto _LL1A7;_LL1AA: {struct Cyc_Absyn_ArrayElement_struct*_tmp3AC=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3AA;if(_tmp3AC->tag != 0)goto _LL1A7;else{_tmp3AD=_tmp3AC->f1;}}_LL1AB: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3AD);{unsigned int _tmp3B1;int _tmp3B2;struct _tuple14 _tmp3B0=Cyc_Evexp_eval_const_uint_exp(
_tmp3AD);_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3B0.f2;if(!_tmp3B2){const char*_tmp87D;
void*_tmp87C;(_tmp87C=0,Cyc_Tcutil_terr(_tmp3AD->loc,((_tmp87D="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp87D,sizeof(char),47))),_tag_dyneither(_tmp87C,sizeof(void*),0)));}
else{if(_tmp3B1 != offset){const char*_tmp882;void*_tmp881[2];struct Cyc_Int_pa_struct
_tmp880;struct Cyc_Int_pa_struct _tmp87F;(_tmp87F.tag=1,((_tmp87F.f1=(
unsigned long)((int)_tmp3B1),((_tmp880.tag=1,((_tmp880.f1=(unsigned long)offset,((
_tmp881[0]=& _tmp880,((_tmp881[1]=& _tmp87F,Cyc_Tcutil_terr(_tmp3AD->loc,((_tmp882="expecting index designator %d but found %d",
_tag_dyneither(_tmp882,sizeof(char),43))),_tag_dyneither(_tmp881,sizeof(void*),2)))))))))))));}}
goto _LL1A7;};_LL1A7:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3B9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1AD: {struct Cyc_Absyn_TagType_struct*_tmp3BA=(
struct Cyc_Absyn_TagType_struct*)_tmp3B9;if(_tmp3BA->tag != 20)goto _LL1AF;}_LL1AE:
goto _LL1AC;_LL1AF:;_LL1B0: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp886;void*_tmp885[1];struct Cyc_String_pa_struct _tmp884;(_tmp884.tag=0,((
_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp885[0]=& _tmp884,
Cyc_Tcutil_terr(bound->loc,((_tmp886="expecting unsigned int, found %s",
_tag_dyneither(_tmp886,sizeof(char),33))),_tag_dyneither(_tmp885,sizeof(void*),1)))))));}
_LL1AC:;}if(!(vd->tq).real_const){const char*_tmp889;void*_tmp888;(_tmp888=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp889="comprehension index variable is not declared const!",_tag_dyneither(
_tmp889,sizeof(char),52))),_tag_dyneither(_tmp888,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3C0=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3C0,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3C0,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3C0,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3C1=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp3C2=0;struct Cyc_Absyn_Tqual*_tmp3C3=0;union Cyc_Absyn_Constraint**
_tmp3C4=0;if(topt != 0){void*_tmp3C5=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3C7;struct Cyc_Absyn_ArrayInfo _tmp3C9;void*_tmp3CA;void**_tmp3CB;struct Cyc_Absyn_Tqual
_tmp3CC;struct Cyc_Absyn_Tqual*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;union Cyc_Absyn_Constraint*
_tmp3CF;union Cyc_Absyn_Constraint**_tmp3D0;_LL1B2: {struct Cyc_Absyn_PointerType_struct*
_tmp3C6=(struct Cyc_Absyn_PointerType_struct*)_tmp3C5;if(_tmp3C6->tag != 5)goto
_LL1B4;else{_tmp3C7=_tmp3C6->f1;}}_LL1B3: pinfo=_tmp3C7;_tmp3C2=(void**)& pinfo.elt_typ;
_tmp3C3=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp3C4=(union Cyc_Absyn_Constraint**)&(
pinfo.ptr_atts).zero_term;goto _LL1B1;_LL1B4: {struct Cyc_Absyn_ArrayType_struct*
_tmp3C8=(struct Cyc_Absyn_ArrayType_struct*)_tmp3C5;if(_tmp3C8->tag != 9)goto
_LL1B6;else{_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C9.elt_type;_tmp3CB=(void**)&(
_tmp3C8->f1).elt_type;_tmp3CC=_tmp3C9.tq;_tmp3CD=(struct Cyc_Absyn_Tqual*)&(
_tmp3C8->f1).tq;_tmp3CE=_tmp3C9.num_elts;_tmp3CF=_tmp3C9.zero_term;_tmp3D0=(
union Cyc_Absyn_Constraint**)&(_tmp3C8->f1).zero_term;}}_LL1B5: _tmp3C2=(void**)
_tmp3CB;_tmp3C3=(struct Cyc_Absyn_Tqual*)_tmp3CD;_tmp3C4=(union Cyc_Absyn_Constraint**)
_tmp3D0;goto _LL1B1;_LL1B6:;_LL1B7: goto _LL1B1;_LL1B1:;}{void*t=Cyc_Tcexp_tcExp(
_tmp3C1,_tmp3C2,body);if(_tmp3C1->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp3C1,
bound)){const char*_tmp88C;void*_tmp88B;(_tmp88B=0,Cyc_Tcutil_terr(bound->loc,((
_tmp88C="bound is not constant",_tag_dyneither(_tmp88C,sizeof(char),22))),
_tag_dyneither(_tmp88B,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp3C1,
body)){const char*_tmp88F;void*_tmp88E;(_tmp88E=0,Cyc_Tcutil_terr(bound->loc,((
_tmp88F="body is not constant",_tag_dyneither(_tmp88F,sizeof(char),21))),
_tag_dyneither(_tmp88E,sizeof(void*),0)));}}if(_tmp3C4 != 0  && ((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3C4)){struct Cyc_Absyn_Exp*
_tmp3D5=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp890;_tmp3D5->topt=((
_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890->v=Cyc_Absyn_uint_typ,_tmp890))));}
bound=Cyc_Absyn_add_exp(bound,_tmp3D5,0);{struct Cyc_Core_Opt*_tmp891;bound->topt=((
_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->v=Cyc_Absyn_uint_typ,_tmp891))));}*
is_zero_term=1;}{void*_tmp3D8=Cyc_Absyn_array_typ(t,_tmp3C3 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp3C3,(struct Cyc_Absyn_Exp*)bound,_tmp3C4 == 0?Cyc_Absyn_false_conref:*
_tmp3C4,0);return _tmp3D8;};};};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp3D9;_push_handler(& _tmp3D9);{int _tmp3DB=0;if(setjmp(
_tmp3D9.handler))_tmp3DB=1;if(!_tmp3DB){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp3DA=(void*)_exn_thrown;void*_tmp3DD=_tmp3DA;_LL1B9: {
struct Cyc_Dict_Absent_struct*_tmp3DE=(struct Cyc_Dict_Absent_struct*)_tmp3DD;if(
_tmp3DE->tag != Cyc_Dict_Absent)goto _LL1BB;}_LL1BA:{const char*_tmp895;void*
_tmp894[1];struct Cyc_String_pa_struct _tmp893;(_tmp893.tag=0,((_tmp893.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp894[0]=&
_tmp893,Cyc_Tcutil_terr(loc,((_tmp895="unbound struct/union name %s",
_tag_dyneither(_tmp895,sizeof(char),29))),_tag_dyneither(_tmp894,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1BB:;_LL1BC:(void)_throw(
_tmp3DD);_LL1B8:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp89D;void*_tmp89C[1];const char*_tmp89B;const char*_tmp89A;
struct Cyc_String_pa_struct _tmp899;(_tmp899.tag=0,((_tmp899.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp89A="struct",_tag_dyneither(
_tmp89A,sizeof(char),7)):((_tmp89B="union",_tag_dyneither(_tmp89B,sizeof(char),6)))),((
_tmp89C[0]=& _tmp899,Cyc_Tcutil_terr(loc,((_tmp89D="can't construct abstract %s",
_tag_dyneither(_tmp89D,sizeof(char),28))),_tag_dyneither(_tmp89C,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp3E7=Cyc_Tcenv_get_fnrgn(te);struct _tuple11 _tmp89E;struct
_tuple11 _tmp3E8=(_tmp89E.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp89E.f2=_tmp3E7,
_tmp89E)));struct Cyc_List_List*_tmp3E9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3E7,Cyc_Tcutil_r_make_inst_var,&
_tmp3E8,ad->tvs);struct Cyc_List_List*_tmp3EA=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3E7,Cyc_Tcutil_r_make_inst_var,&
_tmp3E8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp3EB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp3E9);struct Cyc_List_List*_tmp3EC=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp3EA);struct Cyc_List_List*_tmp3ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp3E7,_tmp3E9,_tmp3EA);struct Cyc_Absyn_AggrType_struct _tmp8A8;struct Cyc_Absyn_Aggrdecl**
_tmp8A7;struct Cyc_Absyn_AggrInfo _tmp8A6;struct Cyc_Absyn_AggrType_struct*_tmp8A5;
struct Cyc_Absyn_AggrType_struct*_tmp3EE=(_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((
_tmp8A5[0]=((_tmp8A8.tag=12,((_tmp8A8.f1=((_tmp8A6.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=ad,_tmp8A7))))),((_tmp8A6.targs=
_tmp3EB,_tmp8A6)))),_tmp8A8)))),_tmp8A5)));struct Cyc_List_List*_tmp3EF=*ts;
struct Cyc_List_List*_tmp3F0=_tmp3EC;while(_tmp3EF != 0  && _tmp3F0 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp3EF->hd);Cyc_Tcutil_unify((
void*)_tmp3EF->hd,(void*)_tmp3F0->hd);_tmp3EF=_tmp3EF->tl;_tmp3F0=_tmp3F0->tl;}
if(_tmp3EF != 0){const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,Cyc_Tcutil_terr(loc,((
_tmp8AB="too many explicit witness types",_tag_dyneither(_tmp8AB,sizeof(char),32))),
_tag_dyneither(_tmp8AA,sizeof(void*),0)));}*ts=_tmp3EC;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp3EE,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp3E7,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple19 _tmp3F4;struct Cyc_Absyn_Aggrfield*
_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;struct _tuple19*_tmp3F3=(struct _tuple19*)
fields->hd;_tmp3F4=*_tmp3F3;_tmp3F5=_tmp3F4.f1;_tmp3F6=_tmp3F4.f2;{void*_tmp3F7=
Cyc_Tcutil_rsubstitute(_tmp3E7,_tmp3ED,_tmp3F5->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp3F7,_tmp3F6);if(!Cyc_Tcutil_coerce_arg(te,_tmp3F6,_tmp3F7,& bogus)){{
const char*_tmp8B7;void*_tmp8B6[5];struct Cyc_String_pa_struct _tmp8B5;const char*
_tmp8B4;const char*_tmp8B3;struct Cyc_String_pa_struct _tmp8B2;struct Cyc_String_pa_struct
_tmp8B1;struct Cyc_String_pa_struct _tmp8B0;struct Cyc_String_pa_struct _tmp8AF;(
_tmp8AF.tag=0,((_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F6->topt))->v)),((_tmp8B0.tag=0,((
_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp3F7)),((_tmp8B1.tag=0,((_tmp8B1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8B2.tag=0,((_tmp8B2.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8B3="struct",
_tag_dyneither(_tmp8B3,sizeof(char),7)):((_tmp8B4="union",_tag_dyneither(_tmp8B4,
sizeof(char),6)))),((_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp3F5->name),((_tmp8B6[0]=& _tmp8B5,((_tmp8B6[1]=& _tmp8B2,((
_tmp8B6[2]=& _tmp8B1,((_tmp8B6[3]=& _tmp8B0,((_tmp8B6[4]=& _tmp8AF,Cyc_Tcutil_terr(
_tmp3F6->loc,((_tmp8B7="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp8B7,sizeof(char),40))),_tag_dyneither(_tmp8B6,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp401=0;{struct Cyc_List_List*
_tmp402=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp402 != 0;_tmp402=_tmp402->tl){struct _tuple0*_tmp8BA;struct Cyc_List_List*
_tmp8B9;_tmp401=((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9->hd=((_tmp8BA=
_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA->f1=Cyc_Tcutil_rsubstitute(_tmp3E7,_tmp3ED,(*((
struct _tuple0*)_tmp402->hd)).f1),((_tmp8BA->f2=Cyc_Tcutil_rsubstitute(_tmp3E7,
_tmp3ED,(*((struct _tuple0*)_tmp402->hd)).f2),_tmp8BA)))))),((_tmp8B9->tl=_tmp401,
_tmp8B9))))));}}_tmp401=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp401);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp401);return(void*)_tmp3EE;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp40A=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp40B=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp40D;struct
Cyc_List_List*_tmp40E;_LL1BE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp40C=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp40B;if(_tmp40C->tag != 13)goto _LL1C0;
else{_tmp40D=_tmp40C->f1;_tmp40E=_tmp40C->f2;}}_LL1BF: if(_tmp40D == Cyc_Absyn_UnionA){
const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,Cyc_Tcutil_terr(loc,((_tmp8BD="expecting struct but found union",
_tag_dyneither(_tmp8BD,sizeof(char),33))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp40A,
loc,args,Cyc_Absyn_StructA,_tmp40E);for(0;fields != 0;fields=fields->tl){int bogus=
0;struct _tuple19 _tmp412;struct Cyc_Absyn_Aggrfield*_tmp413;struct Cyc_Absyn_Exp*
_tmp414;struct _tuple19*_tmp411=(struct _tuple19*)fields->hd;_tmp412=*_tmp411;
_tmp413=_tmp412.f1;_tmp414=_tmp412.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp413->type,_tmp414);if(!Cyc_Tcutil_coerce_arg(te,_tmp414,_tmp413->type,& bogus)){{
const char*_tmp8C3;void*_tmp8C2[3];struct Cyc_String_pa_struct _tmp8C1;struct Cyc_String_pa_struct
_tmp8C0;struct Cyc_String_pa_struct _tmp8BF;(_tmp8BF.tag=0,((_tmp8BF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp414->topt))->v)),((_tmp8C0.tag=0,((_tmp8C0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp413->type)),((
_tmp8C1.tag=0,((_tmp8C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp413->name),((
_tmp8C2[0]=& _tmp8C1,((_tmp8C2[1]=& _tmp8C0,((_tmp8C2[2]=& _tmp8BF,Cyc_Tcutil_terr(
_tmp414->loc,((_tmp8C3="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp8C3,sizeof(char),41))),_tag_dyneither(_tmp8C2,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1BD;};_LL1C0:;_LL1C1: {const char*_tmp8C6;
void*_tmp8C5;(_tmp8C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp8C6="tcAnonStruct: wrong type",_tag_dyneither(_tmp8C6,
sizeof(char),25))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}_LL1BD:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp41C=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp8C7;struct _tuple11 _tmp41D=(_tmp8C7.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp8C7.f2=_tmp41C,_tmp8C7)));struct Cyc_List_List*_tmp41E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41C,Cyc_Tcutil_r_make_inst_var,&
_tmp41D,tud->tvs);struct Cyc_List_List*_tmp41F=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,_tmp41E);struct Cyc_Absyn_DatatypeFieldType_struct _tmp8CD;struct Cyc_Absyn_DatatypeFieldInfo
_tmp8CC;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp8CB;void*res=(void*)((
_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CD.tag=4,((_tmp8CD.f1=((
_tmp8CC.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp8CC.targs=_tmp41F,
_tmp8CC)))),_tmp8CD)))),_tmp8CB))));if(topt != 0){void*_tmp420=Cyc_Tcutil_compress(*
topt);_LL1C3: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp421=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp420;if(_tmp421->tag != 4)goto _LL1C5;}_LL1C4: Cyc_Tcutil_unify(*topt,res);goto
_LL1C2;_LL1C5:;_LL1C6: goto _LL1C2;_LL1C2:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp41C,_tmp41E,(*((
struct _tuple18*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp8D5;void*_tmp8D4[3];struct Cyc_String_pa_struct
_tmp8D3;struct Cyc_String_pa_struct _tmp8D2;const char*_tmp8D1;struct Cyc_String_pa_struct
_tmp8D0;(_tmp8D0.tag=0,((_tmp8D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp8D1="?",_tag_dyneither(_tmp8D1,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp8D2.tag=0,((_tmp8D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp8D3.tag=0,((_tmp8D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8D4[0]=& _tmp8D3,((_tmp8D4[1]=& _tmp8D2,((
_tmp8D4[2]=& _tmp8D0,Cyc_Tcutil_terr(e->loc,((_tmp8D5="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp8D5,sizeof(char),82))),_tag_dyneither(_tmp8D4,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp8D9;void*_tmp8D8[1];
struct Cyc_String_pa_struct _tmp8D7;return(_tmp8D7.tag=0,((_tmp8D7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp8D8[0]=& _tmp8D7,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp8D9="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp8D9,sizeof(char),47))),_tag_dyneither(_tmp8D8,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp8DD;void*_tmp8DC[1];struct Cyc_String_pa_struct _tmp8DB;return(
_tmp8DB.tag=0,((_tmp8DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp8DC[0]=& _tmp8DB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DD="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp8DD,sizeof(char),46))),_tag_dyneither(_tmp8DC,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp432=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp439;struct Cyc_Absyn_PtrAtts _tmp43A;union Cyc_Absyn_Constraint*
_tmp43B;struct Cyc_Absyn_ArrayInfo _tmp440;void*_tmp441;struct Cyc_List_List*
_tmp444;struct Cyc_Absyn_AggrInfo _tmp446;union Cyc_Absyn_AggrInfoU _tmp447;struct
Cyc_List_List*_tmp448;struct Cyc_List_List*_tmp44C;_LL1C8: {struct Cyc_Absyn_VoidType_struct*
_tmp433=(struct Cyc_Absyn_VoidType_struct*)_tmp432;if(_tmp433->tag != 0)goto _LL1CA;}
_LL1C9: return 1;_LL1CA: {struct Cyc_Absyn_Evar_struct*_tmp434=(struct Cyc_Absyn_Evar_struct*)
_tmp432;if(_tmp434->tag != 1)goto _LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_VarType_struct*
_tmp435=(struct Cyc_Absyn_VarType_struct*)_tmp432;if(_tmp435->tag != 2)goto _LL1CE;}
_LL1CD: goto _LL1CF;_LL1CE: {struct Cyc_Absyn_DatatypeType_struct*_tmp436=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp432;if(_tmp436->tag != 3)goto _LL1D0;}_LL1CF:
goto _LL1D1;_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp437=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp432;if(_tmp437->tag != 4)goto _LL1D2;}_LL1D1: return 0;_LL1D2: {struct Cyc_Absyn_PointerType_struct*
_tmp438=(struct Cyc_Absyn_PointerType_struct*)_tmp432;if(_tmp438->tag != 5)goto
_LL1D4;else{_tmp439=_tmp438->f1;_tmp43A=_tmp439.ptr_atts;_tmp43B=_tmp43A.nullable;}}
_LL1D3: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp43B);_LL1D4: {struct Cyc_Absyn_IntType_struct*_tmp43C=(struct Cyc_Absyn_IntType_struct*)
_tmp432;if(_tmp43C->tag != 6)goto _LL1D6;}_LL1D5: goto _LL1D7;_LL1D6: {struct Cyc_Absyn_FloatType_struct*
_tmp43D=(struct Cyc_Absyn_FloatType_struct*)_tmp432;if(_tmp43D->tag != 7)goto
_LL1D8;}_LL1D7: goto _LL1D9;_LL1D8: {struct Cyc_Absyn_DoubleType_struct*_tmp43E=(
struct Cyc_Absyn_DoubleType_struct*)_tmp432;if(_tmp43E->tag != 8)goto _LL1DA;}
_LL1D9: return 1;_LL1DA: {struct Cyc_Absyn_ArrayType_struct*_tmp43F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp432;if(_tmp43F->tag != 9)goto _LL1DC;else{_tmp440=_tmp43F->f1;_tmp441=_tmp440.elt_type;}}
_LL1DB: return Cyc_Tcexp_zeroable_type(_tmp441);_LL1DC: {struct Cyc_Absyn_FnType_struct*
_tmp442=(struct Cyc_Absyn_FnType_struct*)_tmp432;if(_tmp442->tag != 10)goto _LL1DE;}
_LL1DD: return 0;_LL1DE: {struct Cyc_Absyn_TupleType_struct*_tmp443=(struct Cyc_Absyn_TupleType_struct*)
_tmp432;if(_tmp443->tag != 11)goto _LL1E0;else{_tmp444=_tmp443->f1;}}_LL1DF: for(0;(
unsigned int)_tmp444;_tmp444=_tmp444->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple18*)_tmp444->hd)).f2))return 0;}return 1;_LL1E0: {struct Cyc_Absyn_AggrType_struct*
_tmp445=(struct Cyc_Absyn_AggrType_struct*)_tmp432;if(_tmp445->tag != 12)goto
_LL1E2;else{_tmp446=_tmp445->f1;_tmp447=_tmp446.aggr_info;_tmp448=_tmp446.targs;}}
_LL1E1: {struct Cyc_Absyn_Aggrdecl*_tmp457=Cyc_Absyn_get_known_aggrdecl(_tmp447);
if(_tmp457->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp457->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp458=_new_region("r");struct _RegionHandle*r=&
_tmp458;_push_region(r);{struct Cyc_List_List*_tmp459=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp457->tvs,_tmp448);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp457->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp459,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp45A=0;_npop_handler(0);return _tmp45A;}}}{int _tmp45B=1;_npop_handler(0);
return _tmp45B;};};_pop_region(r);};}_LL1E2: {struct Cyc_Absyn_EnumType_struct*
_tmp449=(struct Cyc_Absyn_EnumType_struct*)_tmp432;if(_tmp449->tag != 14)goto
_LL1E4;}_LL1E3: return 1;_LL1E4: {struct Cyc_Absyn_TagType_struct*_tmp44A=(struct
Cyc_Absyn_TagType_struct*)_tmp432;if(_tmp44A->tag != 20)goto _LL1E6;}_LL1E5: return
1;_LL1E6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp44B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp432;if(_tmp44B->tag != 13)goto _LL1E8;else{_tmp44C=_tmp44B->f2;}}_LL1E7: for(0;
_tmp44C != 0;_tmp44C=_tmp44C->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp44C->hd)->type))return 0;}return 1;_LL1E8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp44D=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp432;if(_tmp44D->tag != 15)goto
_LL1EA;}_LL1E9: return 1;_LL1EA: {struct Cyc_Absyn_TypedefType_struct*_tmp44E=(
struct Cyc_Absyn_TypedefType_struct*)_tmp432;if(_tmp44E->tag != 18)goto _LL1EC;}
_LL1EB: return 0;_LL1EC: {struct Cyc_Absyn_DynRgnType_struct*_tmp44F=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp432;if(_tmp44F->tag != 17)goto _LL1EE;}_LL1ED: return 0;_LL1EE: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp450=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp432;if(_tmp450->tag != 16)goto
_LL1F0;}_LL1EF: return 0;_LL1F0: {struct Cyc_Absyn_ValueofType_struct*_tmp451=(
struct Cyc_Absyn_ValueofType_struct*)_tmp432;if(_tmp451->tag != 19)goto _LL1F2;}
_LL1F1: goto _LL1F3;_LL1F2: {struct Cyc_Absyn_HeapRgn_struct*_tmp452=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp432;if(_tmp452->tag != 21)goto _LL1F4;}_LL1F3: goto _LL1F5;_LL1F4: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp453=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp432;if(_tmp453->tag != 22)goto
_LL1F6;}_LL1F5: goto _LL1F7;_LL1F6: {struct Cyc_Absyn_AccessEff_struct*_tmp454=(
struct Cyc_Absyn_AccessEff_struct*)_tmp432;if(_tmp454->tag != 23)goto _LL1F8;}
_LL1F7: goto _LL1F9;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*_tmp455=(struct Cyc_Absyn_JoinEff_struct*)
_tmp432;if(_tmp455->tag != 24)goto _LL1FA;}_LL1F9: goto _LL1FB;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct*
_tmp456=(struct Cyc_Absyn_RgnsEff_struct*)_tmp432;if(_tmp456->tag != 25)goto _LL1C7;}
_LL1FB: {const char*_tmp8E1;void*_tmp8E0[1];struct Cyc_String_pa_struct _tmp8DF;(
_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp8E0[0]=& _tmp8DF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp8E1="bad type `%s' in zeroable type",_tag_dyneither(
_tmp8E1,sizeof(char),31))),_tag_dyneither(_tmp8E0,sizeof(void*),1)))))));}_LL1C7:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp45F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp461;void*_tmp462;_LL1FD: {struct Cyc_Absyn_PointerType_struct*_tmp460=(struct
Cyc_Absyn_PointerType_struct*)_tmp45F;if(_tmp460->tag != 5)goto _LL1FF;else{
_tmp461=_tmp460->f1;_tmp462=_tmp461.elt_typ;}}_LL1FE: Cyc_Tcutil_unify(_tmp462,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL1FC;_LL1FF:;_LL200: goto _LL1FC;_LL1FC:;}{const char*_tmp8EA;void*_tmp8E9[2];
const char*_tmp8E8;const char*_tmp8E7;struct Cyc_String_pa_struct _tmp8E6;struct Cyc_String_pa_struct
_tmp8E5;(_tmp8E5.tag=0,((_tmp8E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp8E6.tag=0,((_tmp8E6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp8E7="calloc",
_tag_dyneither(_tmp8E7,sizeof(char),7))):(struct _dyneither_ptr)((_tmp8E8="malloc",
_tag_dyneither(_tmp8E8,sizeof(char),7))))),((_tmp8E9[0]=& _tmp8E6,((_tmp8E9[1]=&
_tmp8E5,Cyc_Tcutil_terr(loc,((_tmp8EA="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp8EA,sizeof(char),60))),_tag_dyneither(_tmp8E9,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL201: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL202: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp8ED;
struct Cyc_Absyn_RgnHandleType_struct*_tmp8EC;void*expected_type=(void*)((_tmp8EC=
_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=16,((_tmp8ED.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp8ED)))),_tmp8EC))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)ropt);void*_tmp469=Cyc_Tcutil_compress(
handle_type);void*_tmp46B;_LL205: {struct Cyc_Absyn_RgnHandleType_struct*_tmp46A=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp469;if(_tmp46A->tag != 16)goto _LL207;
else{_tmp46B=(void*)_tmp46A->f1;}}_LL206: rgn=_tmp46B;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL204;_LL207:;_LL208:{const char*_tmp8F1;void*_tmp8F0[1];struct
Cyc_String_pa_struct _tmp8EF;(_tmp8EF.tag=0,((_tmp8EF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp8F0[0]=& _tmp8EF,
Cyc_Tcutil_terr(ropt->loc,((_tmp8F1="expecting region_t type but found %s",
_tag_dyneither(_tmp8F1,sizeof(char),37))),_tag_dyneither(_tmp8F0,sizeof(void*),1)))))));}
goto _LL204;_LL204:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp8F4;void*_tmp8F3;(
_tmp8F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8F4="calloc with empty type",_tag_dyneither(_tmp8F4,sizeof(char),23))),
_tag_dyneither(_tmp8F3,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;one_elt=0;}
else{void*_tmp473=(*e)->r;void*_tmp475;enum Cyc_Absyn_Primop _tmp477;struct Cyc_List_List*
_tmp478;struct Cyc_List_List _tmp479;struct Cyc_Absyn_Exp*_tmp47A;struct Cyc_List_List*
_tmp47B;struct Cyc_List_List _tmp47C;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_List_List*
_tmp47E;_LL20A: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp474=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp473;if(_tmp474->tag != 18)goto _LL20C;else{_tmp475=(void*)_tmp474->f1;}}_LL20B:
elt_type=_tmp475;{void**_tmp8F5;*t=(void**)((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((
_tmp8F5[0]=elt_type,_tmp8F5))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL209;_LL20C: {struct Cyc_Absyn_Primop_e_struct*
_tmp476=(struct Cyc_Absyn_Primop_e_struct*)_tmp473;if(_tmp476->tag != 3)goto _LL20E;
else{_tmp477=_tmp476->f1;if(_tmp477 != Cyc_Absyn_Times)goto _LL20E;_tmp478=_tmp476->f2;
if(_tmp478 == 0)goto _LL20E;_tmp479=*_tmp478;_tmp47A=(struct Cyc_Absyn_Exp*)_tmp479.hd;
_tmp47B=_tmp479.tl;if(_tmp47B == 0)goto _LL20E;_tmp47C=*_tmp47B;_tmp47D=(struct Cyc_Absyn_Exp*)
_tmp47C.hd;_tmp47E=_tmp47C.tl;if(_tmp47E != 0)goto _LL20E;}}_LL20D:{struct _tuple0
_tmp8F6;struct _tuple0 _tmp481=(_tmp8F6.f1=_tmp47A->r,((_tmp8F6.f2=_tmp47D->r,
_tmp8F6)));void*_tmp482;void*_tmp484;void*_tmp485;void*_tmp487;_LL211: _tmp482=
_tmp481.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp483=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp482;if(_tmp483->tag != 18)goto _LL213;else{_tmp484=(void*)_tmp483->f1;}};
_LL212: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp484);elt_type=_tmp484;{void**
_tmp8F7;*t=(void**)((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7[0]=elt_type,
_tmp8F7))));}num_elts=_tmp47D;one_elt=0;goto _LL210;_LL213: _tmp485=_tmp481.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp486=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp485;if(_tmp486->tag != 18)goto _LL215;else{_tmp487=(void*)_tmp486->f1;}};
_LL214: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp487);elt_type=_tmp487;{void**
_tmp8F8;*t=(void**)((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8[0]=elt_type,
_tmp8F8))));}num_elts=_tmp47A;one_elt=0;goto _LL210;_LL215:;_LL216: goto No_sizeof;
_LL210:;}goto _LL209;_LL20E:;_LL20F: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**
_tmp8F9;*t=(void**)((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=elt_type,
_tmp8F9))));}num_elts=*e;one_elt=0;goto _LL209;_LL209:;}*e=num_elts;*is_fat=!
one_elt;{void*_tmp48B=elt_type;struct Cyc_Absyn_AggrInfo _tmp48D;union Cyc_Absyn_AggrInfoU
_tmp48E;struct Cyc_Absyn_Aggrdecl**_tmp48F;struct Cyc_Absyn_Aggrdecl*_tmp490;
_LL218: {struct Cyc_Absyn_AggrType_struct*_tmp48C=(struct Cyc_Absyn_AggrType_struct*)
_tmp48B;if(_tmp48C->tag != 12)goto _LL21A;else{_tmp48D=_tmp48C->f1;_tmp48E=_tmp48D.aggr_info;
if((_tmp48E.KnownAggr).tag != 2)goto _LL21A;_tmp48F=(struct Cyc_Absyn_Aggrdecl**)(
_tmp48E.KnownAggr).val;_tmp490=*_tmp48F;}}_LL219: if(_tmp490->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp490->impl))->exist_vars != 0){const
char*_tmp8FC;void*_tmp8FB;(_tmp8FB=0,Cyc_Tcutil_terr(loc,((_tmp8FC="malloc with existential types not yet implemented",
_tag_dyneither(_tmp8FC,sizeof(char),50))),_tag_dyneither(_tmp8FB,sizeof(void*),0)));}
goto _LL217;_LL21A:;_LL21B: goto _LL217;_LL217:;}{void*(*_tmp493)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp494=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp495=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp497;struct Cyc_Absyn_PtrAtts
_tmp498;union Cyc_Absyn_Constraint*_tmp499;union Cyc_Absyn_Constraint*_tmp49A;
union Cyc_Absyn_Constraint*_tmp49B;_LL21D: {struct Cyc_Absyn_PointerType_struct*
_tmp496=(struct Cyc_Absyn_PointerType_struct*)_tmp495;if(_tmp496->tag != 5)goto
_LL21F;else{_tmp497=_tmp496->f1;_tmp498=_tmp497.ptr_atts;_tmp499=_tmp498.nullable;
_tmp49A=_tmp498.bounds;_tmp49B=_tmp498.zero_term;}}_LL21E: _tmp494=_tmp49B;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp499))
_tmp493=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp49B) && !(*is_calloc)){{const char*_tmp8FF;void*_tmp8FE;(_tmp8FE=0,Cyc_Tcutil_warn(
loc,((_tmp8FF="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp8FF,sizeof(char),55))),_tag_dyneither(_tmp8FE,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp49E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp49A);struct Cyc_Absyn_Exp*_tmp4A1;_LL222: {struct Cyc_Absyn_DynEither_b_struct*
_tmp49F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp49E;if(_tmp49F->tag != 0)goto
_LL224;}_LL223: goto _LL221;_LL224:{struct Cyc_Absyn_Upper_b_struct*_tmp4A0=(struct
Cyc_Absyn_Upper_b_struct*)_tmp49E;if(_tmp4A0->tag != 1)goto _LL226;else{_tmp4A1=
_tmp4A0->f1;}}if(!(!one_elt))goto _LL226;_LL225: {int _tmp4A2=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4A2  && Cyc_Evexp_same_const_exp(_tmp4A1,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp49A),_tmp494);}{void*_tmp4A3=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4A5;_LL229: {struct Cyc_Absyn_TagType_struct*_tmp4A4=(struct Cyc_Absyn_TagType_struct*)
_tmp4A3;if(_tmp4A4->tag != 20)goto _LL22B;else{_tmp4A5=(void*)_tmp4A4->f1;}}_LL22A: {
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4A5,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4A6,
_tmp4A1)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp49A),_tmp494);}
goto _LL228;}_LL22B:;_LL22C: goto _LL228;_LL228:;}goto _LL221;}_LL226:;_LL227: goto
_LL221;_LL221:;}goto _LL21C;_LL21F:;_LL220: goto _LL21C;_LL21C:;}if(!one_elt)
_tmp493=Cyc_Absyn_dyneither_typ;return _tmp493(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp494);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4A7=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4A8=Cyc_Tcenv_enter_notreadctxt(
_tmp4A7,te);Cyc_Tcexp_tcExp(_tmp4A8,0,e1);Cyc_Tcexp_tcExp(_tmp4A8,(void**)((void**)((
void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp4A9=Cyc_Tcutil_compress(t1);_LL22E: {struct
Cyc_Absyn_ArrayType_struct*_tmp4AA=(struct Cyc_Absyn_ArrayType_struct*)_tmp4A9;
if(_tmp4AA->tag != 9)goto _LL230;}_LL22F:{const char*_tmp902;void*_tmp901;(_tmp901=
0,Cyc_Tcutil_terr(loc,((_tmp902="cannot assign to an array",_tag_dyneither(
_tmp902,sizeof(char),26))),_tag_dyneither(_tmp901,sizeof(void*),0)));}goto _LL22D;
_LL230:;_LL231: goto _LL22D;_LL22D:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(loc,((
_tmp905="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp905,sizeof(char),58))),_tag_dyneither(_tmp904,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp908;void*_tmp907;return(_tmp907=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp908="swap non-lvalue",_tag_dyneither(_tmp908,sizeof(char),
16))),_tag_dyneither(_tmp907,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp90B;void*_tmp90A;return(_tmp90A=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp90B="swap non-lvalue",_tag_dyneither(_tmp90B,sizeof(char),16))),
_tag_dyneither(_tmp90A,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp90E;void*_tmp90D;return(_tmp90D=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp90E="swap value in zeroterm array",_tag_dyneither(_tmp90E,
sizeof(char),29))),_tag_dyneither(_tmp90D,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp911;void*_tmp910;return(_tmp910=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp911="swap value in zeroterm array",_tag_dyneither(_tmp911,
sizeof(char),29))),_tag_dyneither(_tmp910,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp916;void*_tmp915[2];struct Cyc_String_pa_struct _tmp914;struct Cyc_String_pa_struct
_tmp913;void*_tmp4B7=(_tmp913.tag=0,((_tmp913.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp914.tag=0,((_tmp914.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp915[0]=&
_tmp914,((_tmp915[1]=& _tmp913,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp916="type mismatch: %s != %s",
_tag_dyneither(_tmp916,sizeof(char),24))),_tag_dyneither(_tmp915,sizeof(void*),2)))))))))))));
return _tmp4B7;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4BC=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4BC,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4BD=s->r;struct Cyc_Absyn_Exp*
_tmp4BF;struct Cyc_Absyn_Stmt*_tmp4C1;struct Cyc_Absyn_Stmt*_tmp4C2;struct Cyc_Absyn_Decl*
_tmp4C4;struct Cyc_Absyn_Stmt*_tmp4C5;_LL233: {struct Cyc_Absyn_Exp_s_struct*
_tmp4BE=(struct Cyc_Absyn_Exp_s_struct*)_tmp4BD;if(_tmp4BE->tag != 1)goto _LL235;
else{_tmp4BF=_tmp4BE->f1;}}_LL234: {void*_tmp4C6=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BF->topt))->v;if(!Cyc_Tcutil_unify(_tmp4C6,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp91A;void*_tmp919[1];struct
Cyc_String_pa_struct _tmp918;(_tmp918.tag=0,((_tmp918.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4C6)),((_tmp919[0]=& _tmp918,Cyc_Tcutil_terr(
loc,((_tmp91A="statement expression returns type %s",_tag_dyneither(_tmp91A,
sizeof(char),37))),_tag_dyneither(_tmp919,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp4C6;}_LL235: {struct Cyc_Absyn_Seq_s_struct*_tmp4C0=(struct Cyc_Absyn_Seq_s_struct*)
_tmp4BD;if(_tmp4C0->tag != 2)goto _LL237;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C0->f2;}}
_LL236: s=_tmp4C2;continue;_LL237: {struct Cyc_Absyn_Decl_s_struct*_tmp4C3=(struct
Cyc_Absyn_Decl_s_struct*)_tmp4BD;if(_tmp4C3->tag != 12)goto _LL239;else{_tmp4C4=
_tmp4C3->f1;_tmp4C5=_tmp4C3->f2;}}_LL238: s=_tmp4C5;continue;_LL239:;_LL23A: {
const char*_tmp91D;void*_tmp91C;return(_tmp91C=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp91D="statement expression must end with expression",_tag_dyneither(_tmp91D,
sizeof(char),46))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}_LL232:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp4CC=t;struct Cyc_Absyn_AggrInfo _tmp4CE;union
Cyc_Absyn_AggrInfoU _tmp4CF;struct Cyc_Absyn_Aggrdecl**_tmp4D0;struct Cyc_Absyn_Aggrdecl*
_tmp4D1;_LL23C: {struct Cyc_Absyn_AggrType_struct*_tmp4CD=(struct Cyc_Absyn_AggrType_struct*)
_tmp4CC;if(_tmp4CD->tag != 12)goto _LL23E;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CE.aggr_info;
if((_tmp4CF.KnownAggr).tag != 2)goto _LL23E;_tmp4D0=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4CF.KnownAggr).val;_tmp4D1=*_tmp4D0;}}_LL23D: if((_tmp4D1->kind == Cyc_Absyn_UnionA
 && _tmp4D1->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D1->impl))->tagged)
goto _LL23B;goto _LL23F;_LL23E:;_LL23F:{const char*_tmp921;void*_tmp920[1];struct
Cyc_String_pa_struct _tmp91F;(_tmp91F.tag=0,((_tmp91F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp920[0]=& _tmp91F,Cyc_Tcutil_terr(
loc,((_tmp921="expecting @tagged union but found %s",_tag_dyneither(_tmp921,
sizeof(char),37))),_tag_dyneither(_tmp920,sizeof(void*),1)))))));}goto _LL23B;
_LL23B:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp924;struct Cyc_Absyn_RgnHandleType_struct*
_tmp923;void*expected_type=(void*)((_tmp923=_cycalloc(sizeof(*_tmp923)),((
_tmp923[0]=((_tmp924.tag=16,((_tmp924.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp924)))),_tmp923))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp4D5=Cyc_Tcutil_compress(handle_type);void*_tmp4D7;_LL241: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp4D6=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4D5;if(_tmp4D6->tag != 16)goto _LL243;else{_tmp4D7=(void*)_tmp4D6->f1;}}_LL242:
rgn=_tmp4D7;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL240;_LL243:;_LL244:{
const char*_tmp928;void*_tmp927[1];struct Cyc_String_pa_struct _tmp926;(_tmp926.tag=
0,((_tmp926.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp927[0]=& _tmp926,Cyc_Tcutil_terr(rgn_handle->loc,((_tmp928="expecting region_t type but found %s",
_tag_dyneither(_tmp928,sizeof(char),37))),_tag_dyneither(_tmp927,sizeof(void*),1)))))));}
goto _LL240;_LL240:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
void*_tmp4DD=e1->r;struct Cyc_Core_Opt*_tmp4E0;struct Cyc_List_List*_tmp4E1;struct
Cyc_List_List*_tmp4E3;union Cyc_Absyn_Cnst _tmp4E5;struct _dyneither_ptr _tmp4E6;
_LL246: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4DE=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4DD;if(_tmp4DE->tag != 28)goto _LL248;}_LL247: {void*_tmp4E7=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp4E8=Cyc_Tcutil_compress(_tmp4E7);struct Cyc_Absyn_ArrayInfo
_tmp4EA;void*_tmp4EB;struct Cyc_Absyn_Tqual _tmp4EC;struct Cyc_Absyn_Exp*_tmp4ED;
union Cyc_Absyn_Constraint*_tmp4EE;_LL251: {struct Cyc_Absyn_ArrayType_struct*
_tmp4E9=(struct Cyc_Absyn_ArrayType_struct*)_tmp4E8;if(_tmp4E9->tag != 9)goto
_LL253;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4EA.elt_type;_tmp4EC=_tmp4EA.tq;
_tmp4ED=_tmp4EA.num_elts;_tmp4EE=_tmp4EA.zero_term;}}_LL252: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp4ED);void*b;{void*_tmp4EF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp4F1;_LL256: {
struct Cyc_Absyn_TagType_struct*_tmp4F0=(struct Cyc_Absyn_TagType_struct*)_tmp4EF;
if(_tmp4F0->tag != 20)goto _LL258;else{_tmp4F1=(void*)_tmp4F0->f1;}}_LL257:{struct
Cyc_Absyn_Upper_b_struct _tmp92B;struct Cyc_Absyn_Upper_b_struct*_tmp92A;b=(void*)((
_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=1,((_tmp92B.f1=
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4F1,0),0,Cyc_Absyn_No_coercion,
0),_tmp92B)))),_tmp92A))));}goto _LL255;_LL258:;_LL259: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp92E;struct Cyc_Absyn_Upper_b_struct*
_tmp92D;b=(void*)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92E.tag=
1,((_tmp92E.f1=bnd,_tmp92E)))),_tmp92D))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}
_LL255:;}{struct Cyc_Absyn_PointerType_struct _tmp938;struct Cyc_Absyn_PtrAtts
_tmp937;struct Cyc_Absyn_PtrInfo _tmp936;struct Cyc_Absyn_PointerType_struct*
_tmp935;void*res_typ=(void*)((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935[0]=((
_tmp938.tag=5,((_tmp938.f1=((_tmp936.elt_typ=_tmp4EB,((_tmp936.elt_tq=_tmp4EC,((
_tmp936.ptr_atts=((_tmp937.rgn=rgn,((_tmp937.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp937.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp937.zero_term=_tmp4EE,((_tmp937.ptrloc=0,
_tmp937)))))))))),_tmp936)))))),_tmp938)))),_tmp935))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp939;e->topt=((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939->v=res_typ,
_tmp939))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL253:;_LL254: {const char*_tmp93C;void*_tmp93B;(
_tmp93B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp93C="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp93C,
sizeof(char),45))),_tag_dyneither(_tmp93B,sizeof(void*),0)));}_LL250:;}_LL248: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp4DF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp4DD;if(_tmp4DF->tag != 36)goto _LL24A;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}
_LL249:{struct Cyc_Absyn_Array_e_struct _tmp93F;struct Cyc_Absyn_Array_e_struct*
_tmp93E;e1->r=(void*)((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E[0]=((
_tmp93F.tag=27,((_tmp93F.f1=_tmp4E1,_tmp93F)))),_tmp93E))));}_tmp4E3=_tmp4E1;
goto _LL24B;_LL24A: {struct Cyc_Absyn_Array_e_struct*_tmp4E2=(struct Cyc_Absyn_Array_e_struct*)
_tmp4DD;if(_tmp4E2->tag != 27)goto _LL24C;else{_tmp4E3=_tmp4E2->f1;}}_LL24B: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp4FF=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp501;void*_tmp502;void**_tmp503;struct Cyc_Absyn_Tqual
_tmp504;struct Cyc_Absyn_PtrAtts _tmp505;union Cyc_Absyn_Constraint*_tmp506;_LL25B: {
struct Cyc_Absyn_PointerType_struct*_tmp500=(struct Cyc_Absyn_PointerType_struct*)
_tmp4FF;if(_tmp500->tag != 5)goto _LL25D;else{_tmp501=_tmp500->f1;_tmp502=_tmp501.elt_typ;
_tmp503=(void**)&(_tmp500->f1).elt_typ;_tmp504=_tmp501.elt_tq;_tmp505=_tmp501.ptr_atts;
_tmp506=_tmp505.zero_term;}}_LL25C: elt_typ_opt=(void**)_tmp503;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp506);goto _LL25A;
_LL25D:;_LL25E: goto _LL25A;_LL25A:;}{void*_tmp507=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp4E3);{struct Cyc_Core_Opt*_tmp940;e1->topt=((_tmp940=
_cycalloc(sizeof(*_tmp940)),((_tmp940->v=_tmp507,_tmp940))));}{void*res_typ;{
void*_tmp509=Cyc_Tcutil_compress(_tmp507);struct Cyc_Absyn_ArrayInfo _tmp50B;void*
_tmp50C;struct Cyc_Absyn_Tqual _tmp50D;struct Cyc_Absyn_Exp*_tmp50E;union Cyc_Absyn_Constraint*
_tmp50F;_LL260: {struct Cyc_Absyn_ArrayType_struct*_tmp50A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp509;if(_tmp50A->tag != 9)goto _LL262;else{_tmp50B=_tmp50A->f1;_tmp50C=_tmp50B.elt_type;
_tmp50D=_tmp50B.tq;_tmp50E=_tmp50B.num_elts;_tmp50F=_tmp50B.zero_term;}}_LL261:{
struct Cyc_Absyn_PointerType_struct _tmp955;struct Cyc_Absyn_PtrAtts _tmp954;struct
Cyc_Absyn_Upper_b_struct _tmp953;struct Cyc_Absyn_Upper_b_struct*_tmp952;struct Cyc_Absyn_PtrInfo
_tmp951;struct Cyc_Absyn_PointerType_struct*_tmp950;res_typ=(void*)((_tmp950=
_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp955.tag=5,((_tmp955.f1=((_tmp951.elt_typ=
_tmp50C,((_tmp951.elt_tq=_tmp50D,((_tmp951.ptr_atts=((_tmp954.rgn=rgn,((_tmp954.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp954.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp952=
_cycalloc(sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=1,((_tmp953.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp50E),_tmp953)))),_tmp952))))),((_tmp954.zero_term=_tmp50F,((
_tmp954.ptrloc=0,_tmp954)))))))))),_tmp951)))))),_tmp955)))),_tmp950))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp956;e->topt=((_tmp956=_cycalloc(
sizeof(*_tmp956)),((_tmp956->v=res_typ,_tmp956))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL25F;_LL262:;_LL263: {
const char*_tmp959;void*_tmp958;(_tmp958=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp959="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp959,sizeof(char),50))),_tag_dyneither(_tmp958,sizeof(void*),0)));}
_LL25F:;}return res_typ;};};}_LL24C: {struct Cyc_Absyn_Const_e_struct*_tmp4E4=(
struct Cyc_Absyn_Const_e_struct*)_tmp4DD;if(_tmp4E4->tag != 0)goto _LL24E;else{
_tmp4E5=_tmp4E4->f1;if((_tmp4E5.String_c).tag != 7)goto _LL24E;_tmp4E6=(struct
_dyneither_ptr)(_tmp4E5.String_c).val;}}_LL24D: {void*_tmp519=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp51A=Cyc_Tcexp_tcExp(te,(void**)& _tmp519,e1);
struct Cyc_Absyn_Upper_b_struct _tmp95C;struct Cyc_Absyn_Upper_b_struct*_tmp95B;
return Cyc_Absyn_atb_typ(_tmp51A,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp95B=
_cycalloc(sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95C.tag=1,((_tmp95C.f1=Cyc_Absyn_uint_exp(
1,0),_tmp95C)))),_tmp95B)))),Cyc_Absyn_false_conref);}_LL24E:;_LL24F: {void**
topt2=0;if(topt != 0){void*_tmp51D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp51F;void*_tmp520;void**_tmp521;struct Cyc_Absyn_Tqual _tmp522;_LL265: {struct
Cyc_Absyn_PointerType_struct*_tmp51E=(struct Cyc_Absyn_PointerType_struct*)
_tmp51D;if(_tmp51E->tag != 5)goto _LL267;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51F.elt_typ;
_tmp521=(void**)&(_tmp51E->f1).elt_typ;_tmp522=_tmp51F.elt_tq;}}_LL266: topt2=(
void**)_tmp521;goto _LL264;_LL267: {struct Cyc_Absyn_DatatypeType_struct*_tmp523=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp51D;if(_tmp523->tag != 3)goto _LL269;}
_LL268:{void**_tmp95D;topt2=((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D[0]=*
topt,_tmp95D))));}goto _LL264;_LL269:;_LL26A: goto _LL264;_LL264:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp967;struct Cyc_Absyn_PtrAtts
_tmp966;struct Cyc_Absyn_PtrInfo _tmp965;struct Cyc_Absyn_PointerType_struct*
_tmp964;void*res_typ=(void*)((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964[0]=((
_tmp967.tag=5,((_tmp967.f1=((_tmp965.elt_typ=telt,((_tmp965.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp965.ptr_atts=((_tmp966.rgn=rgn,((_tmp966.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp966.bounds=Cyc_Absyn_bounds_one_conref,((_tmp966.zero_term=
Cyc_Absyn_false_conref,((_tmp966.ptrloc=0,_tmp966)))))))))),_tmp965)))))),
_tmp967)))),_tmp964))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp968;e->topt=((_tmp968=_cycalloc(
sizeof(*_tmp968)),((_tmp968->v=res_typ,_tmp968))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL245:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp52A=t;
struct Cyc_Absyn_ArrayInfo _tmp52C;void*_tmp52D;struct Cyc_Absyn_Tqual _tmp52E;
struct Cyc_Absyn_Exp*_tmp52F;union Cyc_Absyn_Constraint*_tmp530;_LL26C: {struct Cyc_Absyn_ArrayType_struct*
_tmp52B=(struct Cyc_Absyn_ArrayType_struct*)_tmp52A;if(_tmp52B->tag != 9)goto
_LL26E;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52C.elt_type;_tmp52E=_tmp52C.tq;
_tmp52F=_tmp52C.num_elts;_tmp530=_tmp52C.zero_term;}}_LL26D: {void*_tmp532;
struct _tuple13 _tmp531=Cyc_Tcutil_addressof_props(te,e);_tmp532=_tmp531.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp96B;struct Cyc_Absyn_Upper_b_struct*_tmp96A;
void*_tmp533=_tmp52F == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp96A=
_cycalloc(sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp96B.tag=1,((_tmp96B.f1=(struct Cyc_Absyn_Exp*)
_tmp52F,_tmp96B)))),_tmp96A))));t=Cyc_Absyn_atb_typ(_tmp52D,_tmp532,_tmp52E,
_tmp533,_tmp530);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL26E:;_LL26F: return t;_LL26B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp536=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp536,t) && !Cyc_Tcutil_is_noalias_path(_tmp536,e)){{const char*_tmp973;void*
_tmp972[3];const char*_tmp971;struct Cyc_String_pa_struct _tmp970;struct Cyc_String_pa_struct
_tmp96F;struct Cyc_String_pa_struct _tmp96E;(_tmp96E.tag=0,((_tmp96E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp96F.tag=0,((
_tmp96F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp970.tag=0,((_tmp970.f1=(
struct _dyneither_ptr)(topt == 0?(_tmp971="NULL",_tag_dyneither(_tmp971,sizeof(
char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt)),((_tmp972[0]=&
_tmp970,((_tmp972[1]=& _tmp96F,((_tmp972[2]=& _tmp96E,Cyc_fprintf(Cyc_stderr,((
_tmp973="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp973,sizeof(char),30))),
_tag_dyneither(_tmp972,sizeof(void*),3)))))))))))))))))));}{const char*_tmp976;
void*_tmp975;(_tmp975=0,Cyc_Tcutil_terr(e->loc,((_tmp976="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp976,sizeof(char),49))),_tag_dyneither(_tmp975,sizeof(void*),0)));};}{
void*_tmp53F=e->r;union Cyc_Absyn_Cnst _tmp543;struct _dyneither_ptr _tmp544;_LL271: {
struct Cyc_Absyn_Array_e_struct*_tmp540=(struct Cyc_Absyn_Array_e_struct*)_tmp53F;
if(_tmp540->tag != 27)goto _LL273;}_LL272: goto _LL274;_LL273: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp541=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp53F;if(_tmp541->tag != 28)
goto _LL275;}_LL274: goto _LL276;_LL275: {struct Cyc_Absyn_Const_e_struct*_tmp542=(
struct Cyc_Absyn_Const_e_struct*)_tmp53F;if(_tmp542->tag != 0)goto _LL277;else{
_tmp543=_tmp542->f1;if((_tmp543.String_c).tag != 7)goto _LL277;_tmp544=(struct
_dyneither_ptr)(_tmp543.String_c).val;}}_LL276: return t;_LL277:;_LL278: t=Cyc_Tcutil_compress(
t);{void*_tmp545=t;struct Cyc_Absyn_ArrayInfo _tmp547;void*_tmp548;struct Cyc_Absyn_Tqual
_tmp549;struct Cyc_Absyn_Exp*_tmp54A;union Cyc_Absyn_Constraint*_tmp54B;_LL27A: {
struct Cyc_Absyn_ArrayType_struct*_tmp546=(struct Cyc_Absyn_ArrayType_struct*)
_tmp545;if(_tmp546->tag != 9)goto _LL27C;else{_tmp547=_tmp546->f1;_tmp548=_tmp547.elt_type;
_tmp549=_tmp547.tq;_tmp54A=_tmp547.num_elts;_tmp54B=_tmp547.zero_term;}}_LL27B: {
void*_tmp54D;struct _tuple13 _tmp54C=Cyc_Tcutil_addressof_props(te,e);_tmp54D=
_tmp54C.f2;{struct Cyc_Absyn_Upper_b_struct _tmp979;struct Cyc_Absyn_Upper_b_struct*
_tmp978;void*b=_tmp54A == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp978=
_cycalloc(sizeof(*_tmp978)),((_tmp978[0]=((_tmp979.tag=1,((_tmp979.f1=(struct Cyc_Absyn_Exp*)
_tmp54A,_tmp979)))),_tmp978))));t=Cyc_Absyn_atb_typ(_tmp548,_tmp54D,_tmp549,b,
_tmp54B);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL27C:;_LL27D: return t;_LL279:;};_LL270:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp550=e->r;
struct Cyc_Absyn_Exp*_tmp552;_LL27F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp551=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp550;if(_tmp551->tag != 13)
goto _LL281;else{_tmp552=_tmp551->f1;}}_LL280: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp552);((struct Cyc_Core_Opt*)_check_null(_tmp552->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp552->topt))->v,0);e->topt=_tmp552->topt;
goto _LL27E;_LL281:;_LL282: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp553=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp555;void*_tmp556;struct Cyc_Absyn_Tqual
_tmp557;struct Cyc_Absyn_PtrAtts _tmp558;void*_tmp559;union Cyc_Absyn_Constraint*
_tmp55A;union Cyc_Absyn_Constraint*_tmp55B;union Cyc_Absyn_Constraint*_tmp55C;
_LL284: {struct Cyc_Absyn_PointerType_struct*_tmp554=(struct Cyc_Absyn_PointerType_struct*)
_tmp553;if(_tmp554->tag != 5)goto _LL286;else{_tmp555=_tmp554->f1;_tmp556=_tmp555.elt_typ;
_tmp557=_tmp555.elt_tq;_tmp558=_tmp555.ptr_atts;_tmp559=_tmp558.rgn;_tmp55A=
_tmp558.nullable;_tmp55B=_tmp558.bounds;_tmp55C=_tmp558.zero_term;}}_LL285:{void*
_tmp55D=Cyc_Tcutil_compress(_tmp556);struct Cyc_Absyn_FnInfo _tmp55F;struct Cyc_List_List*
_tmp560;struct Cyc_Core_Opt*_tmp561;void*_tmp562;struct Cyc_List_List*_tmp563;int
_tmp564;struct Cyc_Absyn_VarargInfo*_tmp565;struct Cyc_List_List*_tmp566;struct Cyc_List_List*
_tmp567;_LL289: {struct Cyc_Absyn_FnType_struct*_tmp55E=(struct Cyc_Absyn_FnType_struct*)
_tmp55D;if(_tmp55E->tag != 10)goto _LL28B;else{_tmp55F=_tmp55E->f1;_tmp560=_tmp55F.tvars;
_tmp561=_tmp55F.effect;_tmp562=_tmp55F.ret_typ;_tmp563=_tmp55F.args;_tmp564=
_tmp55F.c_varargs;_tmp565=_tmp55F.cyc_varargs;_tmp566=_tmp55F.rgn_po;_tmp567=
_tmp55F.attributes;}}_LL28A: if(_tmp560 != 0){struct _RegionHandle*_tmp568=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp97A;struct _tuple11 _tmp569=(_tmp97A.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp97A.f2=_tmp568,_tmp97A)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp568,Cyc_Tcutil_r_make_inst_var,&
_tmp569,_tmp560);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp980;struct Cyc_Absyn_FnInfo
_tmp97F;struct Cyc_Absyn_FnType_struct*_tmp97E;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp568,inst,(void*)((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=((_tmp980.tag=
10,((_tmp980.f1=((_tmp97F.tvars=0,((_tmp97F.effect=_tmp561,((_tmp97F.ret_typ=
_tmp562,((_tmp97F.args=_tmp563,((_tmp97F.c_varargs=_tmp564,((_tmp97F.cyc_varargs=
_tmp565,((_tmp97F.rgn_po=_tmp566,((_tmp97F.attributes=_tmp567,_tmp97F)))))))))))))))),
_tmp980)))),_tmp97E)))));struct Cyc_Absyn_PointerType_struct _tmp98A;struct Cyc_Absyn_PtrAtts
_tmp989;struct Cyc_Absyn_PtrInfo _tmp988;struct Cyc_Absyn_PointerType_struct*
_tmp987;struct Cyc_Absyn_PointerType_struct*_tmp56A=(_tmp987=_cycalloc(sizeof(*
_tmp987)),((_tmp987[0]=((_tmp98A.tag=5,((_tmp98A.f1=((_tmp988.elt_typ=ftyp,((
_tmp988.elt_tq=_tmp557,((_tmp988.ptr_atts=((_tmp989.rgn=_tmp559,((_tmp989.nullable=
_tmp55A,((_tmp989.bounds=_tmp55B,((_tmp989.zero_term=_tmp55C,((_tmp989.ptrloc=0,
_tmp989)))))))))),_tmp988)))))),_tmp98A)))),_tmp987)));struct Cyc_Absyn_Exp*
_tmp56B=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp98D;
struct Cyc_Absyn_Instantiate_e_struct*_tmp98C;e->r=(void*)((_tmp98C=_cycalloc(
sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=14,((_tmp98D.f1=_tmp56B,((_tmp98D.f2=
ts,_tmp98D)))))),_tmp98C))));}{struct Cyc_Core_Opt*_tmp98E;e->topt=((_tmp98E=
_cycalloc(sizeof(*_tmp98E)),((_tmp98E->v=(void*)_tmp56A,_tmp98E))));};}goto
_LL288;_LL28B:;_LL28C: goto _LL288;_LL288:;}goto _LL283;_LL286:;_LL287: goto _LL283;
_LL283:;}goto _LL27E;_LL27E:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct
_RegionHandle*_tmp577=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp578=0;{
void*_tmp579=fn_exp->r;struct Cyc_List_List*_tmp57B;_LL28E: {struct Cyc_Absyn_FnCall_e_struct*
_tmp57A=(struct Cyc_Absyn_FnCall_e_struct*)_tmp579;if(_tmp57A->tag != 11)goto
_LL290;else{_tmp57B=_tmp57A->f2;}}_LL28F:{void*_tmp57C=e->r;struct Cyc_List_List*
_tmp57E;_LL293: {struct Cyc_Absyn_FnCall_e_struct*_tmp57D=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp57C;if(_tmp57D->tag != 11)goto _LL295;else{_tmp57E=_tmp57D->f2;}}_LL294: {
struct Cyc_List_List*_tmp57F=alias_arg_exps;int _tmp580=0;while(_tmp57F != 0){
while(_tmp580 != (int)_tmp57F->hd){if(_tmp57B == 0){struct Cyc_Int_pa_struct _tmp998;
struct Cyc_Int_pa_struct _tmp997;void*_tmp996[2];const char*_tmp995;void*_tmp994;(
_tmp994=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmp998.tag=1,((_tmp998.f1=(unsigned long)((int)_tmp57F->hd),((
_tmp997.tag=1,((_tmp997.f1=(unsigned long)_tmp580,((_tmp996[0]=& _tmp997,((
_tmp996[1]=& _tmp998,Cyc_aprintf(((_tmp995="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp995,sizeof(char),36))),_tag_dyneither(_tmp996,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp994,sizeof(void*),0)));}++ _tmp580;_tmp57B=_tmp57B->tl;_tmp57E=((
struct Cyc_List_List*)_check_null(_tmp57E))->tl;}{struct Cyc_Absyn_Decl*_tmp587;
struct Cyc_Absyn_Exp*_tmp588;struct _tuple10 _tmp586=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp57B))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp57E))->hd)->topt))->v));_tmp587=_tmp586.f1;_tmp588=_tmp586.f2;
_tmp57B->hd=(void*)_tmp588;{struct Cyc_List_List*_tmp999;_tmp578=((_tmp999=
_region_malloc(_tmp577,sizeof(*_tmp999)),((_tmp999->hd=_tmp587,((_tmp999->tl=
_tmp578,_tmp999))))));}_tmp57F=_tmp57F->tl;};}goto _LL292;}_LL295:;_LL296: {const
char*_tmp99C;void*_tmp99B;(_tmp99B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp99C="not a function call!",
_tag_dyneither(_tmp99C,sizeof(char),21))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
_LL292:;}goto _LL28D;_LL290:;_LL291: {const char*_tmp99F;void*_tmp99E;(_tmp99E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp99F="not a function call!",_tag_dyneither(_tmp99F,sizeof(char),21))),
_tag_dyneither(_tmp99E,sizeof(void*),0)));}_LL28D:;}while(_tmp578 != 0){struct Cyc_Absyn_Decl*
_tmp58E=(struct Cyc_Absyn_Decl*)_tmp578->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp58E,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp578=_tmp578->tl;}e->topt=
0;e->r=fn_exp->r;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*
_tmp58F=e->r;struct Cyc_Absyn_Exp*_tmp591;struct _tuple1*_tmp593;struct Cyc_Absyn_Exp*
_tmp595;struct Cyc_List_List*_tmp596;struct Cyc_Core_Opt*_tmp598;struct Cyc_List_List*
_tmp599;union Cyc_Absyn_Cnst _tmp59B;union Cyc_Absyn_Cnst*_tmp59C;struct _tuple1*
_tmp59E;void*_tmp59F;enum Cyc_Absyn_Primop _tmp5A1;struct Cyc_List_List*_tmp5A2;
struct Cyc_Absyn_Exp*_tmp5A4;enum Cyc_Absyn_Incrementor _tmp5A5;struct Cyc_Absyn_Exp*
_tmp5A7;struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Exp*
_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Exp*
_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Absyn_Exp*
_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Exp*
_tmp5B8;struct Cyc_List_List*_tmp5B9;struct Cyc_Absyn_VarargCallInfo*_tmp5BA;
struct Cyc_Absyn_VarargCallInfo**_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*
_tmp5BF;struct Cyc_List_List*_tmp5C0;void*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C3;
enum Cyc_Absyn_Coercion _tmp5C4;enum Cyc_Absyn_Coercion*_tmp5C5;struct Cyc_Absyn_Exp*
_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CA;struct Cyc_Absyn_Exp*
_tmp5CC;void*_tmp5CE;void*_tmp5D0;void*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D3;
struct Cyc_Absyn_Exp*_tmp5D5;struct _dyneither_ptr*_tmp5D6;int _tmp5D7;int*_tmp5D8;
int _tmp5D9;int*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5DC;struct _dyneither_ptr*_tmp5DD;
int _tmp5DE;int*_tmp5DF;int _tmp5E0;int*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E3;struct
Cyc_Absyn_Exp*_tmp5E4;struct Cyc_List_List*_tmp5E6;struct _tuple8*_tmp5E8;struct
Cyc_List_List*_tmp5E9;struct Cyc_List_List*_tmp5EB;struct Cyc_Absyn_Stmt*_tmp5ED;
struct Cyc_Absyn_Vardecl*_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*
_tmp5F1;int _tmp5F2;int*_tmp5F3;struct _tuple1*_tmp5F5;struct _tuple1**_tmp5F6;
struct Cyc_List_List*_tmp5F7;struct Cyc_List_List**_tmp5F8;struct Cyc_List_List*
_tmp5F9;struct Cyc_Absyn_Aggrdecl*_tmp5FA;struct Cyc_Absyn_Aggrdecl**_tmp5FB;void*
_tmp5FD;struct Cyc_List_List*_tmp5FE;struct Cyc_List_List*_tmp600;struct Cyc_Absyn_Datatypedecl*
_tmp601;struct Cyc_Absyn_Datatypefield*_tmp602;struct _tuple1*_tmp604;struct
_tuple1**_tmp605;struct Cyc_Absyn_Enumdecl*_tmp606;struct Cyc_Absyn_Enumfield*
_tmp607;struct _tuple1*_tmp609;struct _tuple1**_tmp60A;void*_tmp60B;struct Cyc_Absyn_Enumfield*
_tmp60C;struct Cyc_Absyn_MallocInfo _tmp60E;int _tmp60F;int*_tmp610;struct Cyc_Absyn_Exp*
_tmp611;void**_tmp612;void***_tmp613;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_Absyn_Exp**
_tmp615;int _tmp616;int*_tmp617;struct Cyc_Absyn_Exp*_tmp619;struct Cyc_Absyn_Exp*
_tmp61A;struct Cyc_Absyn_Exp*_tmp61C;struct _dyneither_ptr*_tmp61D;void*_tmp61F;
_LL298: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp590=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp58F;if(_tmp590->tag != 13)goto _LL29A;else{_tmp591=_tmp590->f1;}}_LL299: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp591);return;_LL29A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp592=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp58F;if(_tmp592->tag != 2)goto _LL29C;else{
_tmp593=_tmp592->f1;}}_LL29B: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp593);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL29C: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp594=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp58F;if(_tmp594->tag != 10)goto _LL29E;else{
_tmp595=_tmp594->f1;_tmp596=_tmp594->f2;}}_LL29D: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp595,_tmp596);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL29E: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp597=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp58F;if(_tmp597->tag != 36)
goto _LL2A0;else{_tmp598=_tmp597->f1;_tmp599=_tmp597->f2;}}_LL29F: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp599);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2A0: {struct
Cyc_Absyn_Const_e_struct*_tmp59A=(struct Cyc_Absyn_Const_e_struct*)_tmp58F;if(
_tmp59A->tag != 0)goto _LL2A2;else{_tmp59B=_tmp59A->f1;_tmp59C=(union Cyc_Absyn_Cnst*)&
_tmp59A->f1;}}_LL2A1: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp59C,e);goto _LL297;_LL2A2: {struct Cyc_Absyn_Var_e_struct*_tmp59D=(struct Cyc_Absyn_Var_e_struct*)
_tmp58F;if(_tmp59D->tag != 1)goto _LL2A4;else{_tmp59E=_tmp59D->f1;_tmp59F=(void*)
_tmp59D->f2;}}_LL2A3: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp59E,_tmp59F);goto _LL297;
_LL2A4: {struct Cyc_Absyn_Primop_e_struct*_tmp5A0=(struct Cyc_Absyn_Primop_e_struct*)
_tmp58F;if(_tmp5A0->tag != 3)goto _LL2A6;else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A0->f2;}}
_LL2A5: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5A1,_tmp5A2);goto _LL297;_LL2A6: {
struct Cyc_Absyn_Increment_e_struct*_tmp5A3=(struct Cyc_Absyn_Increment_e_struct*)
_tmp58F;if(_tmp5A3->tag != 5)goto _LL2A8;else{_tmp5A4=_tmp5A3->f1;_tmp5A5=_tmp5A3->f2;}}
_LL2A7: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5A4,_tmp5A5);goto _LL297;_LL2A8: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5A6=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp58F;if(_tmp5A6->tag != 4)goto _LL2AA;else{_tmp5A7=_tmp5A6->f1;_tmp5A8=_tmp5A6->f2;
_tmp5A9=_tmp5A6->f3;}}_LL2A9: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5A7,_tmp5A8,
_tmp5A9);goto _LL297;_LL2AA: {struct Cyc_Absyn_Conditional_e_struct*_tmp5AA=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp58F;if(_tmp5AA->tag != 6)goto _LL2AC;
else{_tmp5AB=_tmp5AA->f1;_tmp5AC=_tmp5AA->f2;_tmp5AD=_tmp5AA->f3;}}_LL2AB: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5AB,_tmp5AC,_tmp5AD);goto _LL297;_LL2AC: {struct Cyc_Absyn_And_e_struct*
_tmp5AE=(struct Cyc_Absyn_And_e_struct*)_tmp58F;if(_tmp5AE->tag != 7)goto _LL2AE;
else{_tmp5AF=_tmp5AE->f1;_tmp5B0=_tmp5AE->f2;}}_LL2AD: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5AF,_tmp5B0);goto _LL297;_LL2AE: {struct Cyc_Absyn_Or_e_struct*_tmp5B1=(struct
Cyc_Absyn_Or_e_struct*)_tmp58F;if(_tmp5B1->tag != 8)goto _LL2B0;else{_tmp5B2=
_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;}}_LL2AF: t=Cyc_Tcexp_tcOr(te,loc,_tmp5B2,_tmp5B3);
goto _LL297;_LL2B0: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5B4=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp58F;if(_tmp5B4->tag != 9)goto _LL2B2;else{_tmp5B5=_tmp5B4->f1;_tmp5B6=_tmp5B4->f2;}}
_LL2B1: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5B5,_tmp5B6);goto _LL297;_LL2B2: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5B7=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp58F;if(_tmp5B7->tag != 11)goto _LL2B4;else{_tmp5B8=_tmp5B7->f1;_tmp5B9=_tmp5B7->f2;
_tmp5BA=_tmp5B7->f3;_tmp5BB=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5B7->f3;}}
_LL2B3: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp621;_push_handler(& _tmp621);{int _tmp623=0;if(setjmp(
_tmp621.handler))_tmp623=1;if(!_tmp623){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp622=(void*)_exn_thrown;void*_tmp625=_tmp622;struct
_dyneither_ptr _tmp627;_LL2EB: {struct Cyc_Core_Failure_struct*_tmp626=(struct Cyc_Core_Failure_struct*)
_tmp625;if(_tmp626->tag != Cyc_Core_Failure)goto _LL2ED;else{_tmp627=_tmp626->f1;}}
_LL2EC: ok=0;fn_exp=e;goto _LL2EA;_LL2ED:;_LL2EE:(void)_throw(_tmp625);_LL2EA:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5B8,_tmp5B9,_tmp5BB,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL297;}_LL2B4: {
struct Cyc_Absyn_Throw_e_struct*_tmp5BC=(struct Cyc_Absyn_Throw_e_struct*)_tmp58F;
if(_tmp5BC->tag != 12)goto _LL2B6;else{_tmp5BD=_tmp5BC->f1;}}_LL2B5: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp5BD);goto _LL297;_LL2B6: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5BE=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp58F;if(_tmp5BE->tag != 14)goto
_LL2B8;else{_tmp5BF=_tmp5BE->f1;_tmp5C0=_tmp5BE->f2;}}_LL2B7: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5BF,_tmp5C0);goto _LL297;_LL2B8: {struct Cyc_Absyn_Cast_e_struct*
_tmp5C1=(struct Cyc_Absyn_Cast_e_struct*)_tmp58F;if(_tmp5C1->tag != 15)goto _LL2BA;
else{_tmp5C2=(void*)_tmp5C1->f1;_tmp5C3=_tmp5C1->f2;_tmp5C4=_tmp5C1->f4;_tmp5C5=(
enum Cyc_Absyn_Coercion*)& _tmp5C1->f4;}}_LL2B9: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5C2,_tmp5C3,(enum Cyc_Absyn_Coercion*)_tmp5C5);goto _LL297;_LL2BA: {struct Cyc_Absyn_Address_e_struct*
_tmp5C6=(struct Cyc_Absyn_Address_e_struct*)_tmp58F;if(_tmp5C6->tag != 16)goto
_LL2BC;else{_tmp5C7=_tmp5C6->f1;}}_LL2BB: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp5C7);goto _LL297;_LL2BC: {struct Cyc_Absyn_New_e_struct*_tmp5C8=(struct Cyc_Absyn_New_e_struct*)
_tmp58F;if(_tmp5C8->tag != 17)goto _LL2BE;else{_tmp5C9=_tmp5C8->f1;_tmp5CA=_tmp5C8->f2;}}
_LL2BD: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5C9,e,_tmp5CA);goto _LL297;_LL2BE: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp5CB=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp58F;if(_tmp5CB->tag != 19)goto _LL2C0;else{_tmp5CC=_tmp5CB->f1;}}_LL2BF: {void*
_tmp628=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5CC);t=Cyc_Tcexp_tcSizeof(te,loc,topt,
_tmp628);goto _LL297;}_LL2C0: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5CD=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp58F;if(_tmp5CD->tag != 18)goto _LL2C2;else{
_tmp5CE=(void*)_tmp5CD->f1;}}_LL2C1: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5CE);
goto _LL297;_LL2C2: {struct Cyc_Absyn_Offsetof_e_struct*_tmp5CF=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp58F;if(_tmp5CF->tag != 20)goto _LL2C4;else{_tmp5D0=(void*)_tmp5CF->f1;_tmp5D1=(
void*)_tmp5CF->f2;}}_LL2C3: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp5D0,_tmp5D1);
goto _LL297;_LL2C4: {struct Cyc_Absyn_Deref_e_struct*_tmp5D2=(struct Cyc_Absyn_Deref_e_struct*)
_tmp58F;if(_tmp5D2->tag != 21)goto _LL2C6;else{_tmp5D3=_tmp5D2->f1;}}_LL2C5: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp5D3);goto _LL297;_LL2C6: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp5D4=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp58F;if(_tmp5D4->tag != 22)goto
_LL2C8;else{_tmp5D5=_tmp5D4->f1;_tmp5D6=_tmp5D4->f2;_tmp5D7=_tmp5D4->f3;_tmp5D8=(
int*)& _tmp5D4->f3;_tmp5D9=_tmp5D4->f4;_tmp5DA=(int*)& _tmp5D4->f4;}}_LL2C7: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp5D5,_tmp5D6,_tmp5D8,_tmp5DA);goto _LL297;_LL2C8: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp5DB=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp58F;if(_tmp5DB->tag != 23)goto
_LL2CA;else{_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;_tmp5DE=_tmp5DB->f3;_tmp5DF=(
int*)& _tmp5DB->f3;_tmp5E0=_tmp5DB->f4;_tmp5E1=(int*)& _tmp5DB->f4;}}_LL2C9: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp5DC,_tmp5DD,_tmp5DF,_tmp5E1);goto _LL297;_LL2CA: {struct Cyc_Absyn_Subscript_e_struct*
_tmp5E2=(struct Cyc_Absyn_Subscript_e_struct*)_tmp58F;if(_tmp5E2->tag != 24)goto
_LL2CC;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E2->f2;}}_LL2CB: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp5E3,_tmp5E4);goto _LL297;_LL2CC: {struct Cyc_Absyn_Tuple_e_struct*
_tmp5E5=(struct Cyc_Absyn_Tuple_e_struct*)_tmp58F;if(_tmp5E5->tag != 25)goto _LL2CE;
else{_tmp5E6=_tmp5E5->f1;}}_LL2CD: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp5E6);goto
_LL297;_LL2CE: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp5E7=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp58F;if(_tmp5E7->tag != 26)goto _LL2D0;else{_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;}}
_LL2CF: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp5E8,_tmp5E9);goto _LL297;_LL2D0: {
struct Cyc_Absyn_Array_e_struct*_tmp5EA=(struct Cyc_Absyn_Array_e_struct*)_tmp58F;
if(_tmp5EA->tag != 27)goto _LL2D2;else{_tmp5EB=_tmp5EA->f1;}}_LL2D1: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp629=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp62B;void*_tmp62C;void**_tmp62D;union Cyc_Absyn_Constraint*
_tmp62E;_LL2F0: {struct Cyc_Absyn_ArrayType_struct*_tmp62A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp629;if(_tmp62A->tag != 9)goto _LL2F2;else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62B.elt_type;
_tmp62D=(void**)&(_tmp62A->f1).elt_type;_tmp62E=_tmp62B.zero_term;}}_LL2F1:
elt_topt=(void**)_tmp62D;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp62E);goto _LL2EF;_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp5EB);goto _LL297;}_LL2D2: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp5EC=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp58F;
if(_tmp5EC->tag != 37)goto _LL2D4;else{_tmp5ED=_tmp5EC->f1;}}_LL2D3: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp5ED);goto _LL297;_LL2D4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp5EE=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp58F;if(_tmp5EE->tag != 28)
goto _LL2D6;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;_tmp5F1=_tmp5EE->f3;
_tmp5F2=_tmp5EE->f4;_tmp5F3=(int*)& _tmp5EE->f4;}}_LL2D5: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp5EF,_tmp5F0,_tmp5F1,_tmp5F3);goto _LL297;_LL2D6: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp5F4=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp58F;if(_tmp5F4->tag != 29)goto
_LL2D8;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=(struct _tuple1**)& _tmp5F4->f1;_tmp5F7=
_tmp5F4->f2;_tmp5F8=(struct Cyc_List_List**)& _tmp5F4->f2;_tmp5F9=_tmp5F4->f3;
_tmp5FA=_tmp5F4->f4;_tmp5FB=(struct Cyc_Absyn_Aggrdecl**)& _tmp5F4->f4;}}_LL2D7: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp5F6,_tmp5F8,_tmp5F9,_tmp5FB);goto _LL297;
_LL2D8: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp5FC=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp58F;if(_tmp5FC->tag != 30)goto _LL2DA;else{_tmp5FD=(void*)_tmp5FC->f1;_tmp5FE=
_tmp5FC->f2;}}_LL2D9: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp5FD,_tmp5FE);goto _LL297;
_LL2DA: {struct Cyc_Absyn_Datatype_e_struct*_tmp5FF=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp58F;if(_tmp5FF->tag != 31)goto _LL2DC;else{_tmp600=_tmp5FF->f1;_tmp601=_tmp5FF->f2;
_tmp602=_tmp5FF->f3;}}_LL2DB: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp600,
_tmp601,_tmp602);goto _LL297;_LL2DC: {struct Cyc_Absyn_Enum_e_struct*_tmp603=(
struct Cyc_Absyn_Enum_e_struct*)_tmp58F;if(_tmp603->tag != 32)goto _LL2DE;else{
_tmp604=_tmp603->f1;_tmp605=(struct _tuple1**)& _tmp603->f1;_tmp606=_tmp603->f2;
_tmp607=_tmp603->f3;}}_LL2DD:*_tmp605=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp607))->name;{struct Cyc_Absyn_EnumType_struct _tmp9A2;struct Cyc_Absyn_EnumType_struct*
_tmp9A1;t=(void*)((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A2.tag=
14,((_tmp9A2.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp606))->name,((
_tmp9A2.f2=_tmp606,_tmp9A2)))))),_tmp9A1))));}goto _LL297;_LL2DE: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp608=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp58F;if(_tmp608->tag != 33)goto
_LL2E0;else{_tmp609=_tmp608->f1;_tmp60A=(struct _tuple1**)& _tmp608->f1;_tmp60B=(
void*)_tmp608->f2;_tmp60C=_tmp608->f3;}}_LL2DF:*_tmp60A=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp60C))->name;t=_tmp60B;goto _LL297;_LL2E0: {struct Cyc_Absyn_Malloc_e_struct*
_tmp60D=(struct Cyc_Absyn_Malloc_e_struct*)_tmp58F;if(_tmp60D->tag != 34)goto
_LL2E2;else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60E.is_calloc;_tmp610=(int*)&(
_tmp60D->f1).is_calloc;_tmp611=_tmp60E.rgn;_tmp612=_tmp60E.elt_type;_tmp613=(
void***)&(_tmp60D->f1).elt_type;_tmp614=_tmp60E.num_elts;_tmp615=(struct Cyc_Absyn_Exp**)&(
_tmp60D->f1).num_elts;_tmp616=_tmp60E.fat_result;_tmp617=(int*)&(_tmp60D->f1).fat_result;}}
_LL2E1: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp611,_tmp613,_tmp615,_tmp610,_tmp617);
goto _LL297;_LL2E2: {struct Cyc_Absyn_Swap_e_struct*_tmp618=(struct Cyc_Absyn_Swap_e_struct*)
_tmp58F;if(_tmp618->tag != 35)goto _LL2E4;else{_tmp619=_tmp618->f1;_tmp61A=_tmp618->f2;}}
_LL2E3: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp619,_tmp61A);goto _LL297;_LL2E4: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp61B=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp58F;if(_tmp61B->tag != 38)goto _LL2E6;else{_tmp61C=_tmp61B->f1;_tmp61D=_tmp61B->f2;}}
_LL2E5: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp61C,_tmp61D);goto _LL297;_LL2E6: {
struct Cyc_Absyn_Valueof_e_struct*_tmp61E=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp58F;if(_tmp61E->tag != 39)goto _LL2E8;else{_tmp61F=(void*)_tmp61E->f1;}}_LL2E7:
if(!te->allow_valueof){const char*_tmp9A5;void*_tmp9A4;(_tmp9A4=0,Cyc_Tcutil_terr(
e->loc,((_tmp9A5="valueof(-) can only occur within types",_tag_dyneither(_tmp9A5,
sizeof(char),39))),_tag_dyneither(_tmp9A4,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL297;_LL2E8: {struct Cyc_Absyn_Asm_e_struct*_tmp620=(struct Cyc_Absyn_Asm_e_struct*)
_tmp58F;if(_tmp620->tag != 40)goto _LL297;}_LL2E9:{const char*_tmp9A8;void*_tmp9A7;(
_tmp9A7=0,Cyc_Tcutil_terr(e->loc,((_tmp9A8="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmp9A8,sizeof(char),50))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL297:;}{struct Cyc_Core_Opt*
_tmp9A9;e->topt=((_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->v=t,_tmp9A9))));};}
