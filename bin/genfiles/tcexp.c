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
_dyneither_ptr ap){{void*_tmp674;(_tmp674=0,Cyc_Tcutil_terr(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp674,
sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons _tmp1;_push_handler(&
_tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))_tmp3=1;if(!_tmp3){{struct
_RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=Cyc_Tcenv_lookup_ordinary(
_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*_tmp9;struct Cyc_Absyn_Enumfield*
_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;struct Cyc_Absyn_Datatypedecl*
_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {struct Cyc_Tcenv_VarRes_struct*
_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(_tmp6->tag != 0)goto _LL3;else{
_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct _tmp677;struct Cyc_Absyn_Var_e_struct*
_tmp676;e->r=(void*)((_tmp676=_cycalloc(sizeof(*_tmp676)),((_tmp676[0]=((_tmp677.tag=
1,((_tmp677.f1=q,((_tmp677.f2=(void*)_tmp7,_tmp677)))))),_tmp676))));}goto _LL0;
_LL3: {struct Cyc_Tcenv_EnumRes_struct*_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp5;if(_tmp8->tag != 3)goto _LL5;else{_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{
struct Cyc_Absyn_Enum_e_struct _tmp67A;struct Cyc_Absyn_Enum_e_struct*_tmp679;e->r=(
void*)((_tmp679=_cycalloc(sizeof(*_tmp679)),((_tmp679[0]=((_tmp67A.tag=32,((
_tmp67A.f1=q,((_tmp67A.f2=(struct Cyc_Absyn_Enumdecl*)_tmp9,((_tmp67A.f3=(struct
Cyc_Absyn_Enumfield*)_tmpA,_tmp67A)))))))),_tmp679))));}goto _LL0;_LL5: {struct
Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5;
if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}_LL6:{
struct Cyc_Absyn_AnonEnum_e_struct _tmp67D;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp67C;e->r=(void*)((_tmp67C=_cycalloc(sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67D.tag=
33,((_tmp67D.f1=q,((_tmp67D.f2=(void*)_tmpC,((_tmp67D.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp67D)))))))),_tmp67C))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp680;struct Cyc_Absyn_Datatype_e_struct*_tmp67F;e->r=(void*)((_tmp67F=
_cycalloc(sizeof(*_tmp67F)),((_tmp67F[0]=((_tmp680.tag=31,((_tmp680.f1=0,((
_tmp680.f2=_tmpF,((_tmp680.f3=_tmp10,_tmp680)))))))),_tmp67F))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp684;void*_tmp683[1];struct Cyc_String_pa_struct
_tmp682;(_tmp682.tag=0,((_tmp682.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp683[0]=& _tmp682,Cyc_Tcutil_terr(e->loc,((
_tmp684="bad occurrence of type name %s",_tag_dyneither(_tmp684,sizeof(char),31))),
_tag_dyneither(_tmp683,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp687;struct Cyc_Absyn_Var_e_struct*_tmp686;e->r=(void*)((_tmp686=_cycalloc(
sizeof(*_tmp686)),((_tmp686[0]=((_tmp687.tag=1,((_tmp687.f1=q,((_tmp687.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp687)))))),_tmp686))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp68A;struct Cyc_Absyn_Var_e_struct*_tmp689;e->r=(void*)((_tmp689=_cycalloc(
sizeof(*_tmp689)),((_tmp689[0]=((_tmp68A.tag=1,((_tmp68A.f1=q,((_tmp68A.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp68A)))))),_tmp689))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple17{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple17*_tmp68B;return(_tmp68B=_cycalloc(sizeof(*_tmp68B)),((_tmp68B->f1=
0,((_tmp68B->f2=e,_tmp68B)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp68E;struct
Cyc_Absyn_FnCall_e_struct*_tmp68D;e->r=(void*)((_tmp68D=_cycalloc(sizeof(*
_tmp68D)),((_tmp68D[0]=((_tmp68E.tag=11,((_tmp68E.f1=e1,((_tmp68E.f2=es,((
_tmp68E.f3=0,_tmp68E)))))))),_tmp68D))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp692;void*_tmp691[1];struct Cyc_String_pa_struct
_tmp690;(_tmp690.tag=0,((_tmp690.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp691[0]=& _tmp690,Cyc_Tcutil_terr(e->loc,((
_tmp692="%s is a constant, not a function",_tag_dyneither(_tmp692,sizeof(char),
33))),_tag_dyneither(_tmp691,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp695;struct Cyc_Absyn_Datatype_e_struct*_tmp694;e->r=(void*)((_tmp694=
_cycalloc(sizeof(*_tmp694)),((_tmp694[0]=((_tmp695.tag=31,((_tmp695.f1=es,((
_tmp695.f2=_tmp30,((_tmp695.f3=_tmp31,_tmp695)))))))),_tmp694))));}_npop_handler(
0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*
_tmp3D=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp698;struct Cyc_Absyn_Aggregate_e_struct*_tmp697;e->r=(void*)((_tmp697=
_cycalloc(sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=29,((_tmp698.f1=_tmp33->name,((
_tmp698.f2=0,((_tmp698.f3=_tmp3D,((_tmp698.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp698)))))))))),_tmp697))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp69C;void*_tmp69B[1];
struct Cyc_String_pa_struct _tmp69A;(_tmp69A.tag=0,((_tmp69A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp69B[
0]=& _tmp69A,Cyc_Tcutil_terr(e->loc,((_tmp69C="%s is an enum constructor, not a function",
_tag_dyneither(_tmp69C,sizeof(char),42))),_tag_dyneither(_tmp69B,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp69F;struct Cyc_Absyn_FnCall_e_struct*
_tmp69E;e->r=(void*)((_tmp69E=_cycalloc(sizeof(*_tmp69E)),((_tmp69E[0]=((_tmp69F.tag=
11,((_tmp69F.f1=e1,((_tmp69F.f2=es,((_tmp69F.f3=0,_tmp69F)))))))),_tmp69E))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp6A2;struct Cyc_Absyn_FnCall_e_struct*_tmp6A1;e->r=(void*)((_tmp6A1=_cycalloc(
sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=11,((_tmp6A2.f1=e1,((_tmp6A2.f2=es,((
_tmp6A2.f3=0,_tmp6A2)))))))),_tmp6A1))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp6A5;
struct Cyc_Absyn_Array_e_struct*_tmp6A4;e->r=(void*)((_tmp6A4=_cycalloc(sizeof(*
_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=27,((_tmp6A5.f1=des,_tmp6A5)))),_tmp6A4))));}
return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp6A8;void*_tmp6A7;(_tmp6A7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6A8="struct type not properly set",
_tag_dyneither(_tmp6A8,sizeof(char),29))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp6AB;struct Cyc_Absyn_Aggregate_e_struct*_tmp6AA;e->r=(void*)((_tmp6AA=
_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=29,((_tmp6AB.f1=_tmp58->name,((
_tmp6AB.f2=0,((_tmp6AB.f3=des,((_tmp6AB.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp6AB)))))))))),_tmp6AA))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 9)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp6AE;struct Cyc_Absyn_Array_e_struct*_tmp6AD;e->r=(
void*)((_tmp6AD=_cycalloc(sizeof(*_tmp6AD)),((_tmp6AD[0]=((_tmp6AE.tag=27,((
_tmp6AE.f1=des,_tmp6AE)))),_tmp6AD))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp6B1;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp6B0;e->r=(void*)((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0[0]=((_tmp6B1.tag=
30,((_tmp6B1.f1=(void*)t,((_tmp6B1.f2=des,_tmp6B1)))))),_tmp6B0))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp6B4;struct Cyc_Absyn_Array_e_struct*
_tmp6B3;e->r=(void*)((_tmp6B3=_cycalloc(sizeof(*_tmp6B3)),((_tmp6B3[0]=((_tmp6B4.tag=
27,((_tmp6B4.f1=des,_tmp6B4)))),_tmp6B3))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6B9;void*_tmp6B8[2];struct Cyc_String_pa_struct _tmp6B7;
struct Cyc_String_pa_struct _tmp6B6;(_tmp6B6.tag=0,((_tmp6B6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp6B7.tag=0,((_tmp6B7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6B8[0]=& _tmp6B7,((_tmp6B8[1]=& _tmp6B6,Cyc_Tcutil_terr(
e->loc,((_tmp6B9="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6B9,sizeof(char),53))),_tag_dyneither(_tmp6B8,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6BA;struct
_tuple0 _tmp6C=(_tmp6BA.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp6BA.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp6BA)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp6BD;struct Cyc_Tcexp_TestEnv _tmp6BC;return(_tmp6BC.eq=((_tmp6BD=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6BD)),((_tmp6BD->f1=_tmp6F,((
_tmp6BD->f2=_tmp72,_tmp6BD)))))),((_tmp6BC.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp6BC)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp6BE;return(_tmp6BE.eq=0,((_tmp6BE.isTrue=0,_tmp6BE)));};}
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
_tmp84 == 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp6C1;struct Cyc_Absyn_Const_e_struct*
_tmp6C0;e->r=(void*)((_tmp6C0=_cycalloc(sizeof(*_tmp6C0)),((_tmp6C0[0]=((_tmp6C1.tag=
0,((_tmp6C1.f1=Cyc_Absyn_Null_c,_tmp6C1)))),_tmp6C0))));}{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp6D4;
struct Cyc_Absyn_PtrAtts _tmp6D3;struct Cyc_Core_Opt*_tmp6D2;struct Cyc_Core_Opt*
_tmp6D1;struct Cyc_Absyn_PtrInfo _tmp6D0;struct Cyc_Absyn_PointerType_struct*
_tmp6CF;t=(void*)((_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF[0]=((_tmp6D4.tag=
5,((_tmp6D4.f1=((_tmp6D0.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1->v=_tmp99,_tmp6D1))))),((_tmp6D0.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6D0.ptr_atts=((_tmp6D3.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((
_tmp6D2->v=_tmp99,_tmp6D2))))),((_tmp6D3.nullable=Cyc_Absyn_true_conref,((
_tmp6D3.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6D3.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6D3.ptrloc=0,_tmp6D3)))))))))),_tmp6D0)))))),_tmp6D4)))),_tmp6CF))));}goto
_LL50;};_LL5B: {struct Cyc_Absyn_TagType_struct*_tmp95=(struct Cyc_Absyn_TagType_struct*)
_tmp87;if(_tmp95->tag != 20)goto _LL5D;else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {
struct Cyc_Absyn_ValueofType_struct _tmp6D7;struct Cyc_Absyn_ValueofType_struct*
_tmp6D6;struct Cyc_Absyn_ValueofType_struct*_tmpA0=(_tmp6D6=_cycalloc(sizeof(*
_tmp6D6)),((_tmp6D6[0]=((_tmp6D7.tag=19,((_tmp6D7.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0),_tmp6D7)))),_tmp6D6)));if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){{const char*_tmp6DC;void*_tmp6DB[2];struct Cyc_String_pa_struct _tmp6DA;
struct Cyc_String_pa_struct _tmp6D9;(_tmp6D9.tag=0,((_tmp6D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0)),((
_tmp6DA.tag=0,((_tmp6DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp96)),((_tmp6DB[0]=& _tmp6DA,((_tmp6DB[1]=& _tmp6D9,Cyc_Tcutil_terr(loc,((
_tmp6DC="expecting %s but found %s",_tag_dyneither(_tmp6DC,sizeof(char),26))),
_tag_dyneither(_tmp6DB,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp6DF;struct Cyc_Absyn_TagType_struct*_tmp6DE;t=(
void*)((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6DF.tag=20,((
_tmp6DF.f1=(void*)((void*)_tmpA0),_tmp6DF)))),_tmp6DE))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);{struct Cyc_Core_Opt*_tmp6E0;elen->topt=((
_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0->v=Cyc_Absyn_uint_typ,_tmp6E0))));}{
struct Cyc_Absyn_Upper_b_struct _tmp6E3;struct Cyc_Absyn_Upper_b_struct*_tmp6E2;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6E2=_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2[0]=((_tmp6E3.tag=1,((
_tmp6E3.f1=elen,_tmp6E3)))),_tmp6E2)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual
_tmpAF;_LL64: {struct Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAC;if(_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}
_LL65: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)
elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {
struct Cyc_Absyn_PointerType_struct*_tmpB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp6E4;e->topt=((_tmp6E4=_cycalloc(sizeof(*
_tmp6E4)),((_tmp6E4->v=t,_tmp6E4))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp6E7;struct Cyc_Absyn_Upper_b_struct*
_tmp6E6;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6[0]=((_tmp6E7.tag=1,((
_tmp6E7.f1=elen,_tmp6E7)))),_tmp6E6)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp6E8;e->topt=((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8->v=t,_tmp6E8))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*_tmpB5=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp6FB;struct Cyc_Absyn_PtrAtts _tmp6FA;
struct Cyc_Core_Opt*_tmp6F9;struct Cyc_Core_Opt*_tmp6F8;struct Cyc_Absyn_PtrInfo
_tmp6F7;struct Cyc_Absyn_PointerType_struct*_tmp6F6;t=(void*)((_tmp6F6=_cycalloc(
sizeof(*_tmp6F6)),((_tmp6F6[0]=((_tmp6FB.tag=5,((_tmp6FB.f1=((_tmp6F7.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp6F8=_cycalloc(
sizeof(*_tmp6F8)),((_tmp6F8->v=_tmpB5,_tmp6F8))))),((_tmp6F7.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp6F7.ptr_atts=((_tmp6FA.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp6F9=_cycalloc(sizeof(*_tmp6F9)),((_tmp6F9->v=_tmpB5,_tmp6F9))))),((_tmp6FA.nullable=
Cyc_Absyn_true_conref,((_tmp6FA.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6FA.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6FA.ptrloc=0,_tmp6FA)))))))))),_tmp6F7)))))),_tmp6FB)))),_tmp6F6))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){
void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;
struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}_LL6C: {const char*_tmp6FF;void*_tmp6FE[1];
struct Cyc_String_pa_struct _tmp6FD;return(_tmp6FD.tag=0,((_tmp6FD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6FE[0]=&
_tmp6FD,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6FF="undeclared identifier: %s",
_tag_dyneither(_tmp6FF,sizeof(char),26))),_tag_dyneither(_tmp6FE,sizeof(void*),1)))))));}
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
else{_tmpC7=_tmpC6->f1;}}_LL76:{union Cyc_Absyn_Nmspace _tmp700;(*q).f1=(((_tmp700.Loc_n).val=
0,(((_tmp700.Loc_n).tag=4,_tmp700))));}if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79:{struct Cyc_Absyn_Valueof_e_struct _tmp703;struct Cyc_Absyn_Valueof_e_struct*
_tmp702;e->r=(void*)((_tmp702=_cycalloc(sizeof(*_tmp702)),((_tmp702[0]=((_tmp703.tag=
39,((_tmp703.f1=(void*)_tmpCE,_tmp703)))),_tmp702))));}goto _LL77;_LL7A:;_LL7B:
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
char*_tmp706;void*_tmp705;(_tmp705=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDC->hd)->loc,((_tmp706="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp706,sizeof(char),49))),_tag_dyneither(_tmp705,sizeof(void*),0)));}};}}
return;_LL7C:;}if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE0 != 0;(((desc_types=desc_types->tl,
_tmpE0=_tmpE0->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp70B;void*
_tmp70A[2];struct Cyc_String_pa_struct _tmp709;struct Cyc_String_pa_struct _tmp708;(
_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp709.tag=0,((_tmp709.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp70A[
0]=& _tmp709,((_tmp70A[1]=& _tmp708,Cyc_Tcutil_terr(e->loc,((_tmp70B="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp70B,sizeof(char),51))),_tag_dyneither(_tmp70A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp70C;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp70C=_cycalloc(sizeof(*_tmp70C)),((
_tmp70C->hd=(void*)arg_cnt,((_tmp70C->tl=*alias_arg_exps,_tmp70C))))));}{struct
_RegionHandle*_tmpE6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE6,t) && !Cyc_Tcutil_is_noalias_path(_tmpE6,e)){const char*_tmp70F;void*
_tmp70E;(_tmp70E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp70F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp70F,sizeof(char),49))),_tag_dyneither(_tmp70E,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp712;void*_tmp711;(_tmp711=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp712="too few arguments",_tag_dyneither(_tmp712,sizeof(char),18))),
_tag_dyneither(_tmp711,sizeof(void*),0)));}if(_tmpE0 != 0){const char*_tmp715;void*
_tmp714;(_tmp714=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp715="too many arguments",_tag_dyneither(_tmp715,sizeof(char),19))),
_tag_dyneither(_tmp714,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus:
_LL84: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp719;void*_tmp718[1];struct Cyc_String_pa_struct
_tmp717;(_tmp717.tag=0,((_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp718[0]=& _tmp717,Cyc_Tcutil_terr(loc,((_tmp719="expecting arithmetic type but found %s",
_tag_dyneither(_tmp719,sizeof(char),39))),_tag_dyneither(_tmp718,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp71D;void*_tmp71C[1];struct Cyc_String_pa_struct _tmp71B;(_tmp71B.tag=
0,((_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp71C[0]=& _tmp71B,Cyc_Tcutil_terr(loc,((_tmp71D="expecting integral or * type but found %s",
_tag_dyneither(_tmp71D,sizeof(char),42))),_tag_dyneither(_tmp71C,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp721;void*_tmp720[1];struct Cyc_String_pa_struct _tmp71F;(_tmp71F.tag=
0,((_tmp71F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp720[0]=& _tmp71F,Cyc_Tcutil_terr(loc,((_tmp721="expecting integral type but found %s",
_tag_dyneither(_tmp721,sizeof(char),37))),_tag_dyneither(_tmp720,sizeof(void*),1)))))));}
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
_tmp724;void*_tmp723;(_tmp723=0,Cyc_Tcutil_terr(loc,((_tmp724="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp724,sizeof(char),55))),_tag_dyneither(_tmp723,sizeof(void*),0)));}
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F: {const char*_tmp728;void*_tmp727[1];
struct Cyc_String_pa_struct _tmp726;(_tmp726.tag=0,((_tmp726.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp727[0]=&
_tmp726,Cyc_Tcutil_terr(loc,((_tmp728="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp728,sizeof(char),47))),_tag_dyneither(_tmp727,sizeof(void*),1)))))));}
_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88: {const char*_tmp72B;void*_tmp72A;(
_tmp72A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp72B="Non-unary primop",_tag_dyneither(_tmp72B,sizeof(char),17))),
_tag_dyneither(_tmp72A,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp72F;void*_tmp72E[
1];struct Cyc_String_pa_struct _tmp72D;(_tmp72D.tag=0,((_tmp72D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp72E[0]=& _tmp72D,Cyc_Tcutil_terr(e1->loc,((
_tmp72F="type %s cannot be used here",_tag_dyneither(_tmp72F,sizeof(char),28))),
_tag_dyneither(_tmp72E,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp733;void*_tmp732[1];struct Cyc_String_pa_struct
_tmp731;(_tmp731.tag=0,((_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp732[0]=& _tmp731,Cyc_Tcutil_terr(e2->loc,((_tmp733="type %s cannot be used here",
_tag_dyneither(_tmp733,sizeof(char),28))),_tag_dyneither(_tmp732,sizeof(void*),1)))))));}
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
_tmp736;void*_tmp735;(_tmp735=0,Cyc_Tcutil_terr(e1->loc,((_tmp736="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp736,sizeof(char),50))),_tag_dyneither(_tmp735,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp739;void*_tmp738;(_tmp738=0,
Cyc_Tcutil_terr(e1->loc,((_tmp739="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp739,sizeof(char),54))),_tag_dyneither(_tmp738,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp73D;void*_tmp73C[1];struct
Cyc_String_pa_struct _tmp73B;(_tmp73B.tag=0,((_tmp73B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp73C[0]=& _tmp73B,Cyc_Tcutil_terr(
e2->loc,((_tmp73D="expecting int but found %s",_tag_dyneither(_tmp73D,sizeof(
char),27))),_tag_dyneither(_tmp73C,sizeof(void*),1)))))));}{void*_tmp11E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp115);struct Cyc_Absyn_Exp*_tmp121;_LL9C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp11F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp11E;if(_tmp11F->tag != 0)goto
_LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*_tmp120=(struct Cyc_Absyn_Upper_b_struct*)
_tmp11E;if(_tmp120->tag != 1)goto _LL9B;else{_tmp121=_tmp120->f1;}}_LL9F: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp116)){const char*
_tmp740;void*_tmp73F;(_tmp73F=0,Cyc_Tcutil_warn(e1->loc,((_tmp740="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp740,sizeof(char),70))),_tag_dyneither(_tmp73F,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp74A;struct Cyc_Absyn_PtrAtts _tmp749;struct
Cyc_Absyn_PtrInfo _tmp748;struct Cyc_Absyn_PointerType_struct*_tmp747;struct Cyc_Absyn_PointerType_struct*
_tmp124=(_tmp747=_cycalloc(sizeof(*_tmp747)),((_tmp747[0]=((_tmp74A.tag=5,((
_tmp74A.f1=((_tmp748.elt_typ=_tmp110,((_tmp748.elt_tq=_tmp111,((_tmp748.ptr_atts=((
_tmp749.rgn=_tmp113,((_tmp749.nullable=Cyc_Absyn_true_conref,((_tmp749.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp749.zero_term=_tmp116,((_tmp749.ptrloc=0,
_tmp749)))))))))),_tmp748)))))),_tmp74A)))),_tmp747)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp124,Cyc_Absyn_Other_coercion);return(void*)_tmp124;};_LL9B:;};
_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL96:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp74F;void*_tmp74E[2];struct Cyc_String_pa_struct _tmp74D;struct Cyc_String_pa_struct
_tmp74C;(_tmp74C.tag=0,((_tmp74C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp74D.tag=0,((_tmp74D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp74E[0]=& _tmp74D,((
_tmp74E[1]=& _tmp74C,Cyc_Tcutil_terr(e1->loc,((_tmp74F="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp74F,sizeof(char),59))),_tag_dyneither(_tmp74E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp752;void*_tmp751;(
_tmp751=0,Cyc_Tcutil_terr(e1->loc,((_tmp752="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp752,sizeof(char),50))),_tag_dyneither(_tmp751,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp755;void*_tmp754;(_tmp754=0,
Cyc_Tcutil_terr(e1->loc,((_tmp755="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp755,sizeof(char),54))),_tag_dyneither(_tmp754,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp75A;void*_tmp759[2];struct
Cyc_String_pa_struct _tmp758;struct Cyc_String_pa_struct _tmp757;(_tmp757.tag=0,((
_tmp757.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp758.tag=0,((_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp759[0]=& _tmp758,((_tmp759[1]=& _tmp757,Cyc_Tcutil_terr(
e2->loc,((_tmp75A="expecting either %s or int but found %s",_tag_dyneither(
_tmp75A,sizeof(char),40))),_tag_dyneither(_tmp759,sizeof(void*),2)))))))))))));}
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
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp75B;struct _tuple0
_tmp136=(_tmp75B.f1=Cyc_Tcutil_compress(t1),((_tmp75B.f2=Cyc_Tcutil_compress(t2),
_tmp75B)));void*_tmp137;struct Cyc_Absyn_PtrInfo _tmp139;void*_tmp13A;void*_tmp13B;
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
_LLA6: goto _LLA0;_LLA0:;}{const char*_tmp760;void*_tmp75F[2];struct Cyc_String_pa_struct
_tmp75E;struct Cyc_String_pa_struct _tmp75D;(_tmp75D.tag=0,((_tmp75D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp75E.tag=
0,((_tmp75E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp75F[0]=& _tmp75E,((_tmp75F[1]=& _tmp75D,Cyc_Tcutil_terr(loc,((_tmp760="comparison not allowed between %s and %s",
_tag_dyneither(_tmp760,sizeof(char),41))),_tag_dyneither(_tmp75F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp761;
struct _tuple0 _tmp148=(_tmp761.f1=t1,((_tmp761.f2=t2,_tmp761)));void*_tmp149;void*
_tmp14B;void*_tmp14C;void*_tmp14E;_LLA8: _tmp149=_tmp148.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp149;if(_tmp14A->tag != 16)goto
_LLAA;else{_tmp14B=(void*)_tmp14A->f1;}};_tmp14C=_tmp148.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 16)goto
_LLAA;else{_tmp14E=(void*)_tmp14D->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}{const char*_tmp766;void*_tmp765[2];struct Cyc_String_pa_struct
_tmp764;struct Cyc_String_pa_struct _tmp763;(_tmp763.tag=0,((_tmp763.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp764.tag=
0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp765[0]=& _tmp764,((_tmp765[1]=& _tmp763,Cyc_Tcutil_terr(loc,((_tmp766="comparison not allowed between %s and %s",
_tag_dyneither(_tmp766,sizeof(char),41))),_tag_dyneither(_tmp765,sizeof(void*),2)))))))))))));}
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
default: _LLBD: {const char*_tmp769;void*_tmp768;(_tmp768=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp769="bad binary primop",
_tag_dyneither(_tmp769,sizeof(char),18))),_tag_dyneither(_tmp768,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: {const char*_tmp76C;void*_tmp76B;return(_tmp76B=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp76C="primitive operator has 0 arguments",_tag_dyneither(_tmp76C,
sizeof(char),35))),_tag_dyneither(_tmp76B,sizeof(void*),0)));}case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: {const char*_tmp76F;void*_tmp76E;return(_tmp76E=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp76F="primitive operator has > 2 arguments",_tag_dyneither(_tmp76F,
sizeof(char),37))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}}return t;};}struct
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
_tmp16D->tq).real_const){{const char*_tmp773;void*_tmp772[1];struct Cyc_String_pa_struct
_tmp771;(_tmp771.tag=0,((_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp16D->name),((_tmp772[0]=& _tmp771,Cyc_Tcutil_terr(loc,((_tmp773="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp773,sizeof(char),56))),_tag_dyneither(_tmp772,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16D->type))return 0;}return 1;
_LLC9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15A;if(_tmp162->tag != 4)goto _LLCB;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
if((_tmp164.KnownDatatypefield).tag != 2)goto _LLCB;_tmp165=(struct _tuple2)(
_tmp164.KnownDatatypefield).val;_tmp166=_tmp165.f2;}}_LLCA:{struct Cyc_List_List*
_tmp171=_tmp166->typs;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){struct _tuple18
_tmp173;struct Cyc_Absyn_Tqual _tmp174;void*_tmp175;struct _tuple18*_tmp172=(struct
_tuple18*)_tmp171->hd;_tmp173=*_tmp172;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;if(
_tmp174.real_const){{const char*_tmp777;void*_tmp776[1];struct Cyc_String_pa_struct
_tmp775;(_tmp775.tag=0,((_tmp775.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp166->name)),((_tmp776[0]=& _tmp775,Cyc_Tcutil_terr(
loc,((_tmp777="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp777,sizeof(char),64))),_tag_dyneither(_tmp776,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp175))return 0;}}return 1;_LLCB: {
struct Cyc_Absyn_ArrayType_struct*_tmp167=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15A;if(_tmp167->tag != 9)goto _LLCD;else{_tmp168=_tmp167->f1;_tmp169=_tmp168.elt_type;
_tmp16A=_tmp168.tq;}}_LLCC: if(_tmp16A.real_const){{const char*_tmp77A;void*
_tmp779;(_tmp779=0,Cyc_Tcutil_terr(loc,((_tmp77A="attempt to over-write a const array",
_tag_dyneither(_tmp77A,sizeof(char),36))),_tag_dyneither(_tmp779,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp169);_LLCD: {struct Cyc_Absyn_TupleType_struct*
_tmp16B=(struct Cyc_Absyn_TupleType_struct*)_tmp15A;if(_tmp16B->tag != 11)goto
_LLCF;else{_tmp16C=_tmp16B->f1;}}_LLCE: for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct _tuple18 _tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct _tuple18*
_tmp17B=(struct _tuple18*)_tmp16C->hd;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f1;_tmp17E=
_tmp17C.f2;if(_tmp17D.real_const){{const char*_tmp77D;void*_tmp77C;(_tmp77C=0,Cyc_Tcutil_terr(
loc,((_tmp77D="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp77D,sizeof(char),56))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}
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
_tmp1AC=_tmp1AA.f2;if(!_tmp1AC){{const char*_tmp780;void*_tmp77F;(_tmp77F=0,Cyc_Tcutil_terr(
e->loc,((_tmp780="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp780,sizeof(char),47))),_tag_dyneither(_tmp77F,sizeof(void*),0)));}return;}{
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
_LLE7: goto _LLD1;_LLD1:;}{const char*_tmp784;void*_tmp783[1];struct Cyc_String_pa_struct
_tmp782;(_tmp782.tag=0,((_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp783[0]=& _tmp782,Cyc_Tcutil_terr(e->loc,((
_tmp784="attempt to write a const location: %s",_tag_dyneither(_tmp784,sizeof(
char),38))),_tag_dyneither(_tmp783,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){struct _RegionHandle*_tmp1D4=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(_tmp1D4,te),0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp787;void*_tmp786;return(_tmp786=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp787="increment/decrement of non-lvalue",_tag_dyneither(_tmp787,
sizeof(char),34))),_tag_dyneither(_tmp786,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp78A;void*_tmp789;(_tmp789=0,Cyc_Tcutil_terr(e->loc,((
_tmp78A="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp78A,sizeof(char),50))),_tag_dyneither(_tmp789,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp78D;void*_tmp78C;(_tmp78C=0,Cyc_Tcutil_terr(e->loc,((_tmp78D="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp78D,sizeof(char),54))),_tag_dyneither(_tmp78C,sizeof(void*),0)));}}
else{const char*_tmp791;void*_tmp790[1];struct Cyc_String_pa_struct _tmp78F;(
_tmp78F.tag=0,((_tmp78F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp790[0]=& _tmp78F,Cyc_Tcutil_terr(e->loc,((_tmp791="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp791,sizeof(char),44))),_tag_dyneither(_tmp790,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1E0;int _tmp1E1;const char*_tmp792;struct Cyc_Tcexp_TestEnv
_tmp1DF=Cyc_Tcexp_tcTest(te,e1,((_tmp792="conditional expression",_tag_dyneither(
_tmp792,sizeof(char),23))));_tmp1E0=_tmp1DF.eq;_tmp1E1=_tmp1DF.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp793;
struct Cyc_List_List _tmp1E2=(_tmp793.hd=e3,((_tmp793.tl=0,_tmp793)));struct Cyc_List_List
_tmp794;struct Cyc_List_List _tmp1E3=(_tmp794.hd=e2,((_tmp794.tl=(struct Cyc_List_List*)&
_tmp1E2,_tmp794)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1E3)){{const char*_tmp799;void*_tmp798[2];struct Cyc_String_pa_struct _tmp797;
struct Cyc_String_pa_struct _tmp796;(_tmp796.tag=0,((_tmp796.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp797.tag=0,((_tmp797.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp798[0]=& _tmp797,((_tmp798[1]=& _tmp796,Cyc_Tcutil_terr(
loc,((_tmp799="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp799,sizeof(char),48))),_tag_dyneither(_tmp798,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp79A;Cyc_Tcexp_tcTest(te,e1,((_tmp79A="logical-and expression",
_tag_dyneither(_tmp79A,sizeof(char),23))));}{const char*_tmp79B;Cyc_Tcexp_tcTest(
te,e2,((_tmp79B="logical-and expression",_tag_dyneither(_tmp79B,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp79C;Cyc_Tcexp_tcTest(te,e1,((_tmp79C="logical-or expression",
_tag_dyneither(_tmp79C,sizeof(char),22))));}{const char*_tmp79D;Cyc_Tcexp_tcTest(
te,e2,((_tmp79D="logical-or expression",_tag_dyneither(_tmp79D,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1EE=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(_tmp1EE,Cyc_Tcenv_enter_notreadctxt(
_tmp1EE,te)),0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp1EF=Cyc_Tcutil_compress(t1);_LL111: {struct Cyc_Absyn_ArrayType_struct*
_tmp1F0=(struct Cyc_Absyn_ArrayType_struct*)_tmp1EF;if(_tmp1F0->tag != 9)goto
_LL113;}_LL112:{const char*_tmp7A0;void*_tmp79F;(_tmp79F=0,Cyc_Tcutil_terr(loc,((
_tmp7A0="cannot assign to an array",_tag_dyneither(_tmp7A0,sizeof(char),26))),
_tag_dyneither(_tmp79F,sizeof(void*),0)));}goto _LL110;_LL113:;_LL114: goto _LL110;
_LL110:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){const
char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(loc,((_tmp7A3="type is abstract (can't determine size).",
_tag_dyneither(_tmp7A3,sizeof(char),41))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7A6;void*_tmp7A5;return(_tmp7A5=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7A6="assignment to non-lvalue",_tag_dyneither(_tmp7A6,sizeof(
char),25))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp1F7=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F7,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F7,e2)){const char*_tmp7A9;void*
_tmp7A8;(_tmp7A8=0,Cyc_Tcutil_terr(e2->loc,((_tmp7A9="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7A9,sizeof(char),49))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp7AE;void*_tmp7AD[2];struct
Cyc_String_pa_struct _tmp7AC;struct Cyc_String_pa_struct _tmp7AB;void*_tmp1FA=(
_tmp7AB.tag=0,((_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7AC.tag=0,((_tmp7AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7AD[0]=& _tmp7AC,((_tmp7AD[1]=& _tmp7AB,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7AE="type mismatch: %s != %s",_tag_dyneither(_tmp7AE,sizeof(
char),24))),_tag_dyneither(_tmp7AD,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1FA;}}else{enum Cyc_Absyn_Primop
_tmp1FF=(enum Cyc_Absyn_Primop)po->v;void*_tmp200=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp1FF,e1,e2);if(!(Cyc_Tcutil_unify(_tmp200,t1) || Cyc_Tcutil_coerceable(
_tmp200) && Cyc_Tcutil_coerceable(t1))){const char*_tmp7B3;void*_tmp7B2[2];struct
Cyc_String_pa_struct _tmp7B1;struct Cyc_String_pa_struct _tmp7B0;void*_tmp201=(
_tmp7B0.tag=0,((_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7B2[0]=& _tmp7B1,((_tmp7B2[1]=& _tmp7B0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7B3="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp7B3,sizeof(char),82))),_tag_dyneither(_tmp7B2,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp200,t1);Cyc_Tcutil_explain_failure();return _tmp201;}return
_tmp200;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp206=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp209;enum Cyc_Absyn_Size_of _tmp20B;_LL116: {struct
Cyc_Absyn_FloatType_struct*_tmp207=(struct Cyc_Absyn_FloatType_struct*)_tmp206;
if(_tmp207->tag != 7)goto _LL118;}_LL117: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL115;_LL118: {struct Cyc_Absyn_IntType_struct*
_tmp208=(struct Cyc_Absyn_IntType_struct*)_tmp206;if(_tmp208->tag != 6)goto _LL11A;
else{_tmp209=_tmp208->f2;if(_tmp209 != Cyc_Absyn_Char_sz)goto _LL11A;}}_LL119: goto
_LL11B;_LL11A: {struct Cyc_Absyn_IntType_struct*_tmp20A=(struct Cyc_Absyn_IntType_struct*)
_tmp206;if(_tmp20A->tag != 6)goto _LL11C;else{_tmp20B=_tmp20A->f2;if(_tmp20B != Cyc_Absyn_Short_sz)
goto _LL11C;}}_LL11B: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL115;_LL11C:;_LL11D: goto _LL115;_LL115:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp20D;struct Cyc_List_List*
_tmp20E;struct Cyc_Position_Segment*_tmp20F;enum Cyc_Absyn_Scope _tmp210;struct Cyc_Absyn_Datatypefield
_tmp20C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp20D=_tmp20C.name;
_tmp20E=_tmp20C.typs;_tmp20F=_tmp20C.loc;_tmp210=_tmp20C.sc;if(_tmp20E == 0  || 
_tmp20E->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple18*)_tmp20E->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_Position_Segment*
_tmp214;enum Cyc_Absyn_Scope _tmp215;struct Cyc_Absyn_Datatypefield _tmp211=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp212=_tmp211.name;_tmp213=_tmp211.typs;
_tmp214=_tmp211.loc;_tmp215=_tmp211.sc;if(_tmp213 == 0  || _tmp213->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp213->hd)).f2);
int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp7B8;void*_tmp7B7[2];
struct Cyc_String_pa_struct _tmp7B6;struct Cyc_String_pa_struct _tmp7B5;(_tmp7B5.tag=
0,((_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7B6.tag=0,((_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp7B7[0]=& _tmp7B6,((_tmp7B7[1]=& _tmp7B5,Cyc_Tcutil_terr(
e->loc,((_tmp7B8="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7B8,sizeof(char),54))),_tag_dyneither(_tmp7B7,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp21B=args;int _tmp21C=0;struct _RegionHandle*_tmp21D=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp21E=Cyc_Tcenv_new_block(_tmp21D,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp21E,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp21F=t;struct Cyc_Absyn_PtrInfo _tmp221;void*
_tmp222;struct Cyc_Absyn_Tqual _tmp223;struct Cyc_Absyn_PtrAtts _tmp224;void*_tmp225;
union Cyc_Absyn_Constraint*_tmp226;union Cyc_Absyn_Constraint*_tmp227;union Cyc_Absyn_Constraint*
_tmp228;_LL11F: {struct Cyc_Absyn_PointerType_struct*_tmp220=(struct Cyc_Absyn_PointerType_struct*)
_tmp21F;if(_tmp220->tag != 5)goto _LL121;else{_tmp221=_tmp220->f1;_tmp222=_tmp221.elt_typ;
_tmp223=_tmp221.elt_tq;_tmp224=_tmp221.ptr_atts;_tmp225=_tmp224.rgn;_tmp226=
_tmp224.nullable;_tmp227=_tmp224.bounds;_tmp228=_tmp224.zero_term;}}_LL120: Cyc_Tcenv_check_rgn_accessible(
_tmp21E,loc,_tmp225);Cyc_Tcutil_check_nonzero_bound(loc,_tmp227);{void*_tmp229=
Cyc_Tcutil_compress(_tmp222);struct Cyc_Absyn_FnInfo _tmp22B;struct Cyc_List_List*
_tmp22C;struct Cyc_Core_Opt*_tmp22D;struct Cyc_Absyn_Tqual _tmp22E;void*_tmp22F;
struct Cyc_List_List*_tmp230;int _tmp231;struct Cyc_Absyn_VarargInfo*_tmp232;struct
Cyc_List_List*_tmp233;struct Cyc_List_List*_tmp234;_LL124: {struct Cyc_Absyn_FnType_struct*
_tmp22A=(struct Cyc_Absyn_FnType_struct*)_tmp229;if(_tmp22A->tag != 10)goto _LL126;
else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22B.tvars;_tmp22D=_tmp22B.effect;_tmp22E=
_tmp22B.ret_tqual;_tmp22F=_tmp22B.ret_typ;_tmp230=_tmp22B.args;_tmp231=_tmp22B.c_varargs;
_tmp232=_tmp22B.cyc_varargs;_tmp233=_tmp22B.rgn_po;_tmp234=_tmp22B.attributes;}}
_LL125: if(topt != 0)Cyc_Tcutil_unify(_tmp22F,*topt);while(_tmp21B != 0  && _tmp230
!= 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21B->hd;
void*t2=(*((struct _tuple8*)_tmp230->hd)).f3;Cyc_Tcexp_tcExp(_tmp21E,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp21E,e1,t2,& alias_coercion)){{const char*_tmp7BD;
void*_tmp7BC[2];struct Cyc_String_pa_struct _tmp7BB;struct Cyc_String_pa_struct
_tmp7BA;(_tmp7BA.tag=0,((_tmp7BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7BB.tag=0,((_tmp7BB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp7BC[0]=& _tmp7BB,((_tmp7BC[1]=& _tmp7BA,Cyc_Tcutil_terr(
e1->loc,((_tmp7BD="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7BD,sizeof(char),57))),_tag_dyneither(_tmp7BC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7BE;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7BE=_cycalloc(sizeof(*_tmp7BE)),((_tmp7BE->hd=(void*)_tmp21C,((_tmp7BE->tl=*
alias_arg_exps,_tmp7BE))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp21D,
t2) && !Cyc_Tcutil_is_noalias_path(_tmp21D,e1)){const char*_tmp7C1;void*_tmp7C0;(
_tmp7C0=0,Cyc_Tcutil_terr(e1->loc,((_tmp7C1="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7C1,sizeof(char),49))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));}
_tmp21B=_tmp21B->tl;_tmp230=_tmp230->tl;++ _tmp21C;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp234;for(0;a != 0;a=a->tl){void*_tmp23C=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp23E;int _tmp23F;int _tmp240;_LL129: {struct Cyc_Absyn_Format_att_struct*
_tmp23D=(struct Cyc_Absyn_Format_att_struct*)_tmp23C;if(_tmp23D->tag != 19)goto
_LL12B;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;_tmp240=_tmp23D->f3;}}_LL12A:{
struct _handler_cons _tmp241;_push_handler(& _tmp241);{int _tmp243=0;if(setjmp(
_tmp241.handler))_tmp243=1;if(!_tmp243){{struct Cyc_Absyn_Exp*_tmp244=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp23F - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp240 == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp7C2;fmt_args=((
_tmp7C2=_cycalloc(sizeof(*_tmp7C2)),((_tmp7C2->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp240 - 1),_tmp7C2))));}
args_already_checked=1;{struct _RegionHandle*_tmp246=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp23E){case Cyc_Absyn_Printf_ft: _LL12D: Cyc_Tcexp_check_format_args(
_tmp21E,_tmp244,fmt_args,_tmp240 - 1,alias_arg_exps,_tmp246,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL12E: Cyc_Tcexp_check_format_args(_tmp21E,_tmp244,
fmt_args,_tmp240 - 1,alias_arg_exps,_tmp246,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp242=(void*)_exn_thrown;void*_tmp248=_tmp242;_LL131: {
struct Cyc_List_Nth_struct*_tmp249=(struct Cyc_List_Nth_struct*)_tmp248;if(_tmp249->tag
!= Cyc_List_Nth)goto _LL133;}_LL132:{const char*_tmp7C5;void*_tmp7C4;(_tmp7C4=0,
Cyc_Tcutil_terr(loc,((_tmp7C5="bad format arguments",_tag_dyneither(_tmp7C5,
sizeof(char),21))),_tag_dyneither(_tmp7C4,sizeof(void*),0)));}goto _LL130;_LL133:;
_LL134:(void)_throw(_tmp248);_LL130:;}};}goto _LL128;_LL12B:;_LL12C: goto _LL128;
_LL128:;}}if(_tmp230 != 0){const char*_tmp7C8;void*_tmp7C7;(_tmp7C7=0,Cyc_Tcutil_terr(
loc,((_tmp7C8="too few arguments for function",_tag_dyneither(_tmp7C8,sizeof(
char),31))),_tag_dyneither(_tmp7C7,sizeof(void*),0)));}else{if((_tmp21B != 0  || 
_tmp231) || _tmp232 != 0){if(_tmp231)for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){Cyc_Tcexp_tcExp(
_tmp21E,0,(struct Cyc_Absyn_Exp*)_tmp21B->hd);}else{if(_tmp232 == 0){const char*
_tmp7CB;void*_tmp7CA;(_tmp7CA=0,Cyc_Tcutil_terr(loc,((_tmp7CB="too many arguments for function",
_tag_dyneither(_tmp7CB,sizeof(char),32))),_tag_dyneither(_tmp7CA,sizeof(void*),0)));}
else{void*_tmp251;int _tmp252;struct Cyc_Absyn_VarargInfo _tmp250=*_tmp232;_tmp251=
_tmp250.type;_tmp252=_tmp250.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp7CC;
struct Cyc_Absyn_VarargCallInfo*_tmp253=(_tmp7CC=_cycalloc(sizeof(*_tmp7CC)),((
_tmp7CC->num_varargs=0,((_tmp7CC->injectors=0,((_tmp7CC->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp232,_tmp7CC)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp253;if(!_tmp252)for(0;_tmp21B != 0;(_tmp21B=_tmp21B->tl,_tmp21C ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21B->hd;++
_tmp253->num_varargs;Cyc_Tcexp_tcExp(_tmp21E,(void**)& _tmp251,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp21E,e1,_tmp251,& alias_coercion)){{const char*_tmp7D1;void*_tmp7D0[2];struct
Cyc_String_pa_struct _tmp7CF;struct Cyc_String_pa_struct _tmp7CE;(_tmp7CE.tag=0,((
_tmp7CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp7CF.tag=0,((
_tmp7CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp251)),((_tmp7D0[0]=& _tmp7CF,((_tmp7D0[1]=& _tmp7CE,Cyc_Tcutil_terr(loc,((
_tmp7D1="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp7D1,sizeof(char),49))),_tag_dyneither(_tmp7D0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp7D2;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp7D2=_cycalloc(sizeof(*_tmp7D2)),((
_tmp7D2->hd=(void*)_tmp21C,((_tmp7D2->tl=*alias_arg_exps,_tmp7D2))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp21D,_tmp251) && !Cyc_Tcutil_is_noalias_path(_tmp21D,e1)){const char*_tmp7D5;
void*_tmp7D4;(_tmp7D4=0,Cyc_Tcutil_terr(e1->loc,((_tmp7D5="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7D5,sizeof(char),49))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}}
else{void*_tmp25B=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp251));
struct Cyc_Absyn_DatatypeInfo _tmp25D;union Cyc_Absyn_DatatypeInfoU _tmp25E;struct
Cyc_Absyn_Datatypedecl**_tmp25F;struct Cyc_Absyn_Datatypedecl*_tmp260;struct Cyc_List_List*
_tmp261;_LL136: {struct Cyc_Absyn_DatatypeType_struct*_tmp25C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp25B;if(_tmp25C->tag != 3)goto _LL138;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25D.datatype_info;
if((_tmp25E.KnownDatatype).tag != 2)goto _LL138;_tmp25F=(struct Cyc_Absyn_Datatypedecl**)(
_tmp25E.KnownDatatype).val;_tmp260=*_tmp25F;_tmp261=_tmp25D.targs;}}_LL137: {
struct Cyc_Absyn_Datatypedecl*_tmp262=*Cyc_Tcenv_lookup_datatypedecl(_tmp21E,loc,
_tmp260->name);struct Cyc_List_List*fields=0;if(_tmp262->fields == 0){const char*
_tmp7D9;void*_tmp7D8[1];struct Cyc_String_pa_struct _tmp7D7;(_tmp7D7.tag=0,((
_tmp7D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp251)),((_tmp7D8[0]=& _tmp7D7,Cyc_Tcutil_terr(loc,((_tmp7D9="can't inject into abstract datatype %s",
_tag_dyneither(_tmp7D9,sizeof(char),39))),_tag_dyneither(_tmp7D8,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp262->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp251),Cyc_Tcenv_curr_rgn(
_tmp21E))){const char*_tmp7DE;void*_tmp7DD[2];struct Cyc_String_pa_struct _tmp7DC;
struct Cyc_String_pa_struct _tmp7DB;(_tmp7DB.tag=0,((_tmp7DB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp21E))),((_tmp7DC.tag=0,((_tmp7DC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp251)),((_tmp7DD[0]=& _tmp7DC,((_tmp7DD[
1]=& _tmp7DB,Cyc_Tcutil_terr(loc,((_tmp7DE="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp7DE,sizeof(char),49))),_tag_dyneither(_tmp7DD,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp26A=Cyc_Tcenv_get_fnrgn(_tmp21E);{struct Cyc_List_List*
_tmp26B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp26A,_tmp26A,
_tmp262->tvs,_tmp261);for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){++ _tmp253->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21B->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp21E,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp26A,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp26A,e1)){const char*_tmp7E1;void*_tmp7E0;(_tmp7E0=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7E1="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7E1,sizeof(char),49))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp26E=Cyc_Tcexp_tcInjection(_tmp21E,e1,Cyc_Tcutil_pointer_elt_type(
_tmp251),_tmp26A,_tmp26B,fields);if(_tmp26E != 0){struct Cyc_List_List*_tmp7E2;
_tmp253->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp253->injectors,((_tmp7E2=_cycalloc(sizeof(*_tmp7E2)),((
_tmp7E2->hd=(struct Cyc_Absyn_Datatypefield*)_tmp26E,((_tmp7E2->tl=0,_tmp7E2)))))));}};};}}
goto _LL135;};}_LL138:;_LL139:{const char*_tmp7E5;void*_tmp7E4;(_tmp7E4=0,Cyc_Tcutil_terr(
loc,((_tmp7E5="bad inject vararg type",_tag_dyneither(_tmp7E5,sizeof(char),23))),
_tag_dyneither(_tmp7E4,sizeof(void*),0)));}goto _LL135;_LL135:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp21E,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp22D))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp21E,loc,_tmp233);return _tmp22F;};_LL126:;_LL127: {const char*_tmp7E8;void*
_tmp7E7;return(_tmp7E7=0,Cyc_Tcexp_expr_err(_tmp21E,loc,topt,((_tmp7E8="expected pointer to function",
_tag_dyneither(_tmp7E8,sizeof(char),29))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));}
_LL123:;};_LL121:;_LL122: {const char*_tmp7EB;void*_tmp7EA;return(_tmp7EA=0,Cyc_Tcexp_expr_err(
_tmp21E,loc,topt,((_tmp7EB="expected pointer to function",_tag_dyneither(_tmp7EB,
sizeof(char),29))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}_LL11E:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp277=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp277,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp277,&
bogus)){const char*_tmp7F0;void*_tmp7EF[2];struct Cyc_String_pa_struct _tmp7EE;
struct Cyc_String_pa_struct _tmp7ED;(_tmp7ED.tag=0,((_tmp7ED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7EE.tag=0,((_tmp7EE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp277)),((_tmp7EF[0]=& _tmp7EE,((
_tmp7EF[1]=& _tmp7ED,Cyc_Tcutil_terr(loc,((_tmp7F0="expected %s but found %s",
_tag_dyneither(_tmp7F0,sizeof(char),25))),_tag_dyneither(_tmp7EF,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp27C=t1;struct Cyc_Absyn_PtrInfo _tmp27E;void*
_tmp27F;struct Cyc_Absyn_Tqual _tmp280;struct Cyc_Absyn_PtrAtts _tmp281;void*_tmp282;
union Cyc_Absyn_Constraint*_tmp283;union Cyc_Absyn_Constraint*_tmp284;union Cyc_Absyn_Constraint*
_tmp285;_LL13B: {struct Cyc_Absyn_PointerType_struct*_tmp27D=(struct Cyc_Absyn_PointerType_struct*)
_tmp27C;if(_tmp27D->tag != 5)goto _LL13D;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27E.elt_typ;
_tmp280=_tmp27E.elt_tq;_tmp281=_tmp27E.ptr_atts;_tmp282=_tmp281.rgn;_tmp283=
_tmp281.nullable;_tmp284=_tmp281.bounds;_tmp285=_tmp281.zero_term;}}_LL13C:{void*
_tmp286=Cyc_Tcutil_compress(_tmp27F);struct Cyc_Absyn_FnInfo _tmp288;struct Cyc_List_List*
_tmp289;struct Cyc_Core_Opt*_tmp28A;struct Cyc_Absyn_Tqual _tmp28B;void*_tmp28C;
struct Cyc_List_List*_tmp28D;int _tmp28E;struct Cyc_Absyn_VarargInfo*_tmp28F;struct
Cyc_List_List*_tmp290;struct Cyc_List_List*_tmp291;_LL140: {struct Cyc_Absyn_FnType_struct*
_tmp287=(struct Cyc_Absyn_FnType_struct*)_tmp286;if(_tmp287->tag != 10)goto _LL142;
else{_tmp288=_tmp287->f1;_tmp289=_tmp288.tvars;_tmp28A=_tmp288.effect;_tmp28B=
_tmp288.ret_tqual;_tmp28C=_tmp288.ret_typ;_tmp28D=_tmp288.args;_tmp28E=_tmp288.c_varargs;
_tmp28F=_tmp288.cyc_varargs;_tmp290=_tmp288.rgn_po;_tmp291=_tmp288.attributes;}}
_LL141: {struct _RegionHandle*_tmp292=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*
instantiation=0;for(0;ts != 0  && _tmp289 != 0;(ts=ts->tl,_tmp289=_tmp289->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp289->hd,&
Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(
void*)ts->hd);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{struct _tuple12*
_tmp7F3;struct Cyc_List_List*_tmp7F2;instantiation=((_tmp7F2=_region_malloc(
_tmp292,sizeof(*_tmp7F2)),((_tmp7F2->hd=((_tmp7F3=_region_malloc(_tmp292,sizeof(*
_tmp7F3)),((_tmp7F3->f1=(struct Cyc_Absyn_Tvar*)_tmp289->hd,((_tmp7F3->f2=(void*)
ts->hd,_tmp7F3)))))),((_tmp7F2->tl=instantiation,_tmp7F2))))));};}if(ts != 0){
const char*_tmp7F6;void*_tmp7F5;return(_tmp7F5=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp7F6="too many type variables in instantiation",_tag_dyneither(_tmp7F6,
sizeof(char),41))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}{struct Cyc_Absyn_FnType_struct
_tmp7FC;struct Cyc_Absyn_FnInfo _tmp7FB;struct Cyc_Absyn_FnType_struct*_tmp7FA;void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp292,instantiation,(void*)((_tmp7FA=
_cycalloc(sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FC.tag=10,((_tmp7FC.f1=((_tmp7FB.tvars=
_tmp289,((_tmp7FB.effect=_tmp28A,((_tmp7FB.ret_tqual=_tmp28B,((_tmp7FB.ret_typ=
_tmp28C,((_tmp7FB.args=_tmp28D,((_tmp7FB.c_varargs=_tmp28E,((_tmp7FB.cyc_varargs=
_tmp28F,((_tmp7FB.rgn_po=_tmp290,((_tmp7FB.attributes=_tmp291,_tmp7FB)))))))))))))))))),
_tmp7FC)))),_tmp7FA)))));struct Cyc_Absyn_PointerType_struct _tmp806;struct Cyc_Absyn_PtrAtts
_tmp805;struct Cyc_Absyn_PtrInfo _tmp804;struct Cyc_Absyn_PointerType_struct*
_tmp803;return(void*)((_tmp803=_cycalloc(sizeof(*_tmp803)),((_tmp803[0]=((
_tmp806.tag=5,((_tmp806.f1=((_tmp804.elt_typ=new_fn_typ,((_tmp804.elt_tq=_tmp280,((
_tmp804.ptr_atts=((_tmp805.rgn=_tmp282,((_tmp805.nullable=_tmp283,((_tmp805.bounds=
_tmp284,((_tmp805.zero_term=_tmp285,((_tmp805.ptrloc=0,_tmp805)))))))))),_tmp804)))))),
_tmp806)))),_tmp803))));};}_LL142:;_LL143: goto _LL13F;_LL13F:;}goto _LL13A;_LL13D:;
_LL13E: goto _LL13A;_LL13A:;}{const char*_tmp80A;void*_tmp809[1];struct Cyc_String_pa_struct
_tmp808;return(_tmp808.tag=0,((_tmp808.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp809[0]=& _tmp808,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp80A="expecting polymorphic type but found %s",_tag_dyneither(
_tmp80A,sizeof(char),40))),_tag_dyneither(_tmp809,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(
loc,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp80F;void*_tmp80E[2];struct Cyc_String_pa_struct _tmp80D;struct Cyc_String_pa_struct
_tmp80C;void*_tmp2A1=(_tmp80C.tag=0,((_tmp80C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp80D.tag=0,((_tmp80D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp80E[0]=&
_tmp80D,((_tmp80E[1]=& _tmp80C,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp80F="cannot cast %s to %s",
_tag_dyneither(_tmp80F,sizeof(char),21))),_tag_dyneither(_tmp80E,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp2A1;};}}}{struct _tuple0 _tmp810;struct
_tuple0 _tmp2A7=(_tmp810.f1=e->r,((_tmp810.f2=Cyc_Tcutil_compress(t),_tmp810)));
void*_tmp2A8;struct Cyc_Absyn_MallocInfo _tmp2AA;int _tmp2AB;void*_tmp2AC;struct Cyc_Absyn_PtrInfo
_tmp2AE;struct Cyc_Absyn_PtrAtts _tmp2AF;union Cyc_Absyn_Constraint*_tmp2B0;union
Cyc_Absyn_Constraint*_tmp2B1;union Cyc_Absyn_Constraint*_tmp2B2;_LL145: _tmp2A8=
_tmp2A7.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp2A9=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2A8;if(_tmp2A9->tag != 34)goto _LL147;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2AA.fat_result;}};
_tmp2AC=_tmp2A7.f2;{struct Cyc_Absyn_PointerType_struct*_tmp2AD=(struct Cyc_Absyn_PointerType_struct*)
_tmp2AC;if(_tmp2AD->tag != 5)goto _LL147;else{_tmp2AE=_tmp2AD->f1;_tmp2AF=_tmp2AE.ptr_atts;
_tmp2B0=_tmp2AF.nullable;_tmp2B1=_tmp2AF.bounds;_tmp2B2=_tmp2AF.zero_term;}};
_LL146: if((_tmp2AB  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B2)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B0)){void*_tmp2B3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2B1);struct Cyc_Absyn_Exp*_tmp2B5;_LL14A: {struct Cyc_Absyn_Upper_b_struct*
_tmp2B4=(struct Cyc_Absyn_Upper_b_struct*)_tmp2B3;if(_tmp2B4->tag != 1)goto _LL14C;
else{_tmp2B5=_tmp2B4->f1;}}_LL14B: if((Cyc_Evexp_eval_const_uint_exp(_tmp2B5)).f1
== 1){const char*_tmp813;void*_tmp812;(_tmp812=0,Cyc_Tcutil_warn(loc,((_tmp813="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp813,sizeof(char),60))),_tag_dyneither(_tmp812,sizeof(void*),0)));}
goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}goto _LL144;_LL147:;_LL148: goto
_LL144;_LL144:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp2B8=0;struct Cyc_Absyn_Tqual _tmp2B9=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp2BA=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2BC;
void*_tmp2BD;struct Cyc_Absyn_Tqual _tmp2BE;struct Cyc_Absyn_PtrAtts _tmp2BF;union
Cyc_Absyn_Constraint*_tmp2C0;_LL14F: {struct Cyc_Absyn_PointerType_struct*_tmp2BB=(
struct Cyc_Absyn_PointerType_struct*)_tmp2BA;if(_tmp2BB->tag != 5)goto _LL151;else{
_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BC.elt_typ;_tmp2BE=_tmp2BC.elt_tq;_tmp2BF=
_tmp2BC.ptr_atts;_tmp2C0=_tmp2BF.zero_term;}}_LL150:{void**_tmp814;_tmp2B8=((
_tmp814=_cycalloc(sizeof(*_tmp814)),((_tmp814[0]=_tmp2BD,_tmp814))));}_tmp2B9=
_tmp2BE;goto _LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}{struct _RegionHandle*
_tmp2C2=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(
_tmp2C2,Cyc_Tcenv_clear_notreadctxt(_tmp2C2,te)),_tmp2B8,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp2C2,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*
_tmp817;void*_tmp816;(_tmp816=0,Cyc_Tcutil_terr(e->loc,((_tmp817="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp817,sizeof(char),46))),_tag_dyneither(_tmp816,sizeof(void*),0)));}{
void*_tmp2C5=e->r;struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C8;_LL154: {
struct Cyc_Absyn_Subscript_e_struct*_tmp2C6=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp2C5;if(_tmp2C6->tag != 24)goto _LL156;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C6->f2;}}
_LL155:{void*_tmp2C9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2C7->topt))->v);_LL159: {struct Cyc_Absyn_TupleType_struct*
_tmp2CA=(struct Cyc_Absyn_TupleType_struct*)_tmp2C9;if(_tmp2CA->tag != 11)goto
_LL15B;}_LL15A: goto _LL158;_LL15B:;_LL15C: e0->r=(Cyc_Absyn_add_exp(_tmp2C7,
_tmp2C8,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp2C7,_tmp2C8);_LL158:;}goto _LL153;
_LL156:;_LL157: goto _LL153;_LL153:;}{void*_tmp2CB=e->r;int _tmp2CD;int _tmp2CF;
_LL15E: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2CC=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp2CB;if(_tmp2CC->tag != 22)goto _LL160;else{_tmp2CD=_tmp2CC->f3;if(_tmp2CD != 1)
goto _LL160;}}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp2CE=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp2CB;if(_tmp2CE->tag != 23)goto
_LL162;else{_tmp2CF=_tmp2CE->f3;if(_tmp2CF != 1)goto _LL162;}}_LL161:{const char*
_tmp81A;void*_tmp819;(_tmp819=0,Cyc_Tcutil_terr(e->loc,((_tmp81A="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp81A,sizeof(char),50))),_tag_dyneither(_tmp819,sizeof(void*),0)));}
goto _LL15D;_LL162:;_LL163: goto _LL15D;_LL15D:;}{int _tmp2D3;void*_tmp2D4;struct
_tuple13 _tmp2D2=Cyc_Tcutil_addressof_props(te,e);_tmp2D3=_tmp2D2.f1;_tmp2D4=
_tmp2D2.f2;if(Cyc_Tcutil_is_noalias_region(_tmp2D4,0)){const char*_tmp81D;void*
_tmp81C;(_tmp81C=0,Cyc_Tcutil_terr(e->loc,((_tmp81D="using & would manufacture an alias to an alias-free pointer",
_tag_dyneither(_tmp81D,sizeof(char),60))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}{
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2D3){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp2D4,tq,Cyc_Absyn_false_conref);return t;};};};};}static void*Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)
return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp821;void*_tmp820[1];struct Cyc_String_pa_struct _tmp81F;(_tmp81F.tag=
0,((_tmp81F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp820[0]=& _tmp81F,Cyc_Tcutil_terr(loc,((_tmp821="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp821,sizeof(char),55))),_tag_dyneither(_tmp820,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2DA=Cyc_Tcutil_compress(*topt);void*_tmp2DC;_LL165: {
struct Cyc_Absyn_TagType_struct*_tmp2DB=(struct Cyc_Absyn_TagType_struct*)_tmp2DA;
if(_tmp2DB->tag != 20)goto _LL167;else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL166: {
struct Cyc_Absyn_Exp*_tmp2DD=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp824;struct Cyc_Absyn_ValueofType_struct*_tmp823;struct Cyc_Absyn_ValueofType_struct*
_tmp2DE=(_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=((_tmp824.tag=19,((
_tmp824.f1=_tmp2DD,_tmp824)))),_tmp823)));if(Cyc_Tcutil_unify(_tmp2DC,(void*)
_tmp2DE))return _tmp2DA;goto _LL164;}_LL167:;_LL168: goto _LL164;_LL164:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);{void*_tmp2E1=n;struct
_dyneither_ptr*_tmp2E3;unsigned int _tmp2E5;_LL16A: {struct Cyc_Absyn_StructField_struct*
_tmp2E2=(struct Cyc_Absyn_StructField_struct*)_tmp2E1;if(_tmp2E2->tag != 0)goto
_LL16C;else{_tmp2E3=_tmp2E2->f1;}}_LL16B: {int bad_type=1;{void*_tmp2E6=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp2E8;union Cyc_Absyn_AggrInfoU _tmp2E9;struct Cyc_Absyn_Aggrdecl**
_tmp2EA;struct Cyc_List_List*_tmp2EC;_LL16F: {struct Cyc_Absyn_AggrType_struct*
_tmp2E7=(struct Cyc_Absyn_AggrType_struct*)_tmp2E6;if(_tmp2E7->tag != 12)goto
_LL171;else{_tmp2E8=_tmp2E7->f1;_tmp2E9=_tmp2E8.aggr_info;if((_tmp2E9.KnownAggr).tag
!= 2)goto _LL171;_tmp2EA=(struct Cyc_Absyn_Aggrdecl**)(_tmp2E9.KnownAggr).val;}}
_LL170: if((*_tmp2EA)->impl == 0)goto _LL16E;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2E3,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2EA)->impl))->fields)){const char*_tmp828;void*_tmp827[1];
struct Cyc_String_pa_struct _tmp826;(_tmp826.tag=0,((_tmp826.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2E3),((_tmp827[0]=& _tmp826,Cyc_Tcutil_terr(
loc,((_tmp828="no field of struct/union has name %s",_tag_dyneither(_tmp828,
sizeof(char),37))),_tag_dyneither(_tmp827,sizeof(void*),1)))))));}bad_type=0;
goto _LL16E;_LL171: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2EB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2E6;if(_tmp2EB->tag != 13)goto _LL173;else{_tmp2EC=_tmp2EB->f2;}}_LL172: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2E3,_tmp2EC)){const char*_tmp82C;void*_tmp82B[1];struct Cyc_String_pa_struct
_tmp82A;(_tmp82A.tag=0,((_tmp82A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2E3),((_tmp82B[0]=& _tmp82A,Cyc_Tcutil_terr(loc,((_tmp82C="no field of struct/union has name %s",
_tag_dyneither(_tmp82C,sizeof(char),37))),_tag_dyneither(_tmp82B,sizeof(void*),1)))))));}
bad_type=0;goto _LL16E;_LL173:;_LL174: goto _LL16E;_LL16E:;}if(bad_type){const char*
_tmp830;void*_tmp82F[1];struct Cyc_String_pa_struct _tmp82E;(_tmp82E.tag=0,((
_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp82F[0]=& _tmp82E,Cyc_Tcutil_terr(loc,((_tmp830="%s is not a known struct/union type",
_tag_dyneither(_tmp830,sizeof(char),36))),_tag_dyneither(_tmp82F,sizeof(void*),1)))))));}
goto _LL169;}_LL16C: {struct Cyc_Absyn_TupleIndex_struct*_tmp2E4=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2E1;if(_tmp2E4->tag != 1)goto _LL169;else{_tmp2E5=_tmp2E4->f1;}}_LL16D: {int
bad_type=1;{void*_tmp2F6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2F8;
union Cyc_Absyn_AggrInfoU _tmp2F9;struct Cyc_Absyn_Aggrdecl**_tmp2FA;struct Cyc_List_List*
_tmp2FC;struct Cyc_List_List*_tmp2FE;struct Cyc_Absyn_DatatypeFieldInfo _tmp300;
union Cyc_Absyn_DatatypeFieldInfoU _tmp301;struct _tuple2 _tmp302;struct Cyc_Absyn_Datatypefield*
_tmp303;_LL176: {struct Cyc_Absyn_AggrType_struct*_tmp2F7=(struct Cyc_Absyn_AggrType_struct*)
_tmp2F6;if(_tmp2F7->tag != 12)goto _LL178;else{_tmp2F8=_tmp2F7->f1;_tmp2F9=_tmp2F8.aggr_info;
if((_tmp2F9.KnownAggr).tag != 2)goto _LL178;_tmp2FA=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2F9.KnownAggr).val;}}_LL177: if((*_tmp2FA)->impl == 0)goto _LL175;_tmp2FC=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2FA)->impl))->fields;goto _LL179;
_LL178: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2FB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F6;if(_tmp2FB->tag != 13)goto _LL17A;else{_tmp2FC=_tmp2FB->f2;}}_LL179: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FC)<= _tmp2E5){const char*
_tmp835;void*_tmp834[2];struct Cyc_Int_pa_struct _tmp833;struct Cyc_Int_pa_struct
_tmp832;(_tmp832.tag=1,((_tmp832.f1=(unsigned long)((int)_tmp2E5),((_tmp833.tag=
1,((_tmp833.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FC),((_tmp834[0]=& _tmp833,((_tmp834[1]=& _tmp832,Cyc_Tcutil_terr(loc,((
_tmp835="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp835,
sizeof(char),46))),_tag_dyneither(_tmp834,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17A: {struct Cyc_Absyn_TupleType_struct*_tmp2FD=(struct Cyc_Absyn_TupleType_struct*)
_tmp2F6;if(_tmp2FD->tag != 11)goto _LL17C;else{_tmp2FE=_tmp2FD->f1;}}_LL17B: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FE)<= _tmp2E5){const char*
_tmp83A;void*_tmp839[2];struct Cyc_Int_pa_struct _tmp838;struct Cyc_Int_pa_struct
_tmp837;(_tmp837.tag=1,((_tmp837.f1=(unsigned long)((int)_tmp2E5),((_tmp838.tag=
1,((_tmp838.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FE),((_tmp839[0]=& _tmp838,((_tmp839[1]=& _tmp837,Cyc_Tcutil_terr(loc,((
_tmp83A="tuple has too few components: %d <= %d",_tag_dyneither(_tmp83A,sizeof(
char),39))),_tag_dyneither(_tmp839,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL175;_LL17C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2FF=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2F6;if(_tmp2FF->tag != 4)goto _LL17E;else{_tmp300=_tmp2FF->f1;_tmp301=_tmp300.field_info;
if((_tmp301.KnownDatatypefield).tag != 2)goto _LL17E;_tmp302=(struct _tuple2)(
_tmp301.KnownDatatypefield).val;_tmp303=_tmp302.f2;}}_LL17D: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp303->typs)< _tmp2E5){const char*_tmp83F;
void*_tmp83E[2];struct Cyc_Int_pa_struct _tmp83D;struct Cyc_Int_pa_struct _tmp83C;(
_tmp83C.tag=1,((_tmp83C.f1=(unsigned long)((int)_tmp2E5),((_tmp83D.tag=1,((
_tmp83D.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp303->typs),((_tmp83E[0]=& _tmp83D,((_tmp83E[1]=& _tmp83C,Cyc_Tcutil_terr(loc,((
_tmp83F="datatype field has too few components: %d < %d",_tag_dyneither(_tmp83F,
sizeof(char),47))),_tag_dyneither(_tmp83E,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17E:;_LL17F: goto _LL175;_LL175:;}if(bad_type){const char*_tmp843;
void*_tmp842[1];struct Cyc_String_pa_struct _tmp841;(_tmp841.tag=0,((_tmp841.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp842[
0]=& _tmp841,Cyc_Tcutil_terr(loc,((_tmp843="%s is not a known type",
_tag_dyneither(_tmp843,sizeof(char),23))),_tag_dyneither(_tmp842,sizeof(void*),1)))))));}
goto _LL169;}_LL169:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp313=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp314=Cyc_Tcenv_clear_lhs(_tmp313,Cyc_Tcenv_clear_notreadctxt(_tmp313,te_orig));
Cyc_Tcexp_tcExp(_tmp314,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp315=t;struct Cyc_Absyn_PtrInfo _tmp318;void*
_tmp319;struct Cyc_Absyn_PtrAtts _tmp31A;void*_tmp31B;union Cyc_Absyn_Constraint*
_tmp31C;union Cyc_Absyn_Constraint*_tmp31D;_LL181: {struct Cyc_Absyn_Evar_struct*
_tmp316=(struct Cyc_Absyn_Evar_struct*)_tmp315;if(_tmp316->tag != 1)goto _LL183;}
_LL182: {struct Cyc_List_List*_tmp31E=Cyc_Tcenv_lookup_type_vars(_tmp314);struct
Cyc_Core_Opt*_tmp844;void*_tmp31F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((
_tmp844=_cycalloc(sizeof(*_tmp844)),((_tmp844->v=_tmp31E,_tmp844)))));struct Cyc_Core_Opt*
_tmp845;void*_tmp320=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp845=_cycalloc(sizeof(*_tmp845)),((_tmp845->v=_tmp31E,_tmp845)))));union Cyc_Absyn_Constraint*
_tmp321=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*
_tmp322=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp846;struct Cyc_Absyn_PtrAtts _tmp323=(_tmp846.rgn=_tmp320,((_tmp846.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp846.bounds=
_tmp321,((_tmp846.zero_term=_tmp322,((_tmp846.ptrloc=0,_tmp846)))))))));struct
Cyc_Absyn_PointerType_struct _tmp84C;struct Cyc_Absyn_PtrInfo _tmp84B;struct Cyc_Absyn_PointerType_struct*
_tmp84A;struct Cyc_Absyn_PointerType_struct*_tmp324=(_tmp84A=_cycalloc(sizeof(*
_tmp84A)),((_tmp84A[0]=((_tmp84C.tag=5,((_tmp84C.f1=((_tmp84B.elt_typ=_tmp31F,((
_tmp84B.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp84B.ptr_atts=_tmp323,_tmp84B)))))),
_tmp84C)))),_tmp84A)));Cyc_Tcutil_unify(t,(void*)_tmp324);_tmp319=_tmp31F;
_tmp31B=_tmp320;_tmp31C=_tmp321;_tmp31D=_tmp322;goto _LL184;}_LL183: {struct Cyc_Absyn_PointerType_struct*
_tmp317=(struct Cyc_Absyn_PointerType_struct*)_tmp315;if(_tmp317->tag != 5)goto
_LL185;else{_tmp318=_tmp317->f1;_tmp319=_tmp318.elt_typ;_tmp31A=_tmp318.ptr_atts;
_tmp31B=_tmp31A.rgn;_tmp31C=_tmp31A.bounds;_tmp31D=_tmp31A.zero_term;}}_LL184:
Cyc_Tcenv_check_rgn_accessible(_tmp314,loc,_tmp31B);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp31C);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp319),& Cyc_Tcutil_tmk)){
const char*_tmp84F;void*_tmp84E;(_tmp84E=0,Cyc_Tcutil_terr(loc,((_tmp84F="can't dereference abstract pointer type",
_tag_dyneither(_tmp84F,sizeof(char),40))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}
return _tmp319;_LL185:;_LL186: {const char*_tmp853;void*_tmp852[1];struct Cyc_String_pa_struct
_tmp851;return(_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp852[0]=& _tmp851,Cyc_Tcexp_expr_err(
_tmp314,loc,topt,((_tmp853="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp853,sizeof(char),39))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));}
_LL180:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp330=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(_tmp330,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp331=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp333;
union Cyc_Absyn_AggrInfoU _tmp334;struct Cyc_Absyn_Aggrdecl**_tmp335;struct Cyc_Absyn_Aggrdecl*
_tmp336;struct Cyc_List_List*_tmp337;enum Cyc_Absyn_AggrKind _tmp339;struct Cyc_List_List*
_tmp33A;_LL188: {struct Cyc_Absyn_AggrType_struct*_tmp332=(struct Cyc_Absyn_AggrType_struct*)
_tmp331;if(_tmp332->tag != 12)goto _LL18A;else{_tmp333=_tmp332->f1;_tmp334=_tmp333.aggr_info;
if((_tmp334.KnownAggr).tag != 2)goto _LL18A;_tmp335=(struct Cyc_Absyn_Aggrdecl**)(
_tmp334.KnownAggr).val;_tmp336=*_tmp335;_tmp337=_tmp333.targs;}}_LL189: {struct
Cyc_Absyn_Aggrfield*_tmp33F=Cyc_Absyn_lookup_decl_field(_tmp336,f);if(_tmp33F == 
0){const char*_tmp858;void*_tmp857[2];struct Cyc_String_pa_struct _tmp856;struct Cyc_String_pa_struct
_tmp855;return(_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp856.tag=0,((_tmp856.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp336->name)),((_tmp857[0]=& _tmp856,((
_tmp857[1]=& _tmp855,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp858="type %s has no %s field",
_tag_dyneither(_tmp858,sizeof(char),24))),_tag_dyneither(_tmp857,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp336->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp344=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp344,_tmp344,_tmp336->tvs,_tmp337);t2=Cyc_Tcutil_rsubstitute(_tmp344,_tmp345,
_tmp33F->type);if(((_tmp336->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp336->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp85C;void*_tmp85B[1];struct Cyc_String_pa_struct _tmp85A;return(
_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp85B[0]=& _tmp85A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp85C="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp85C,sizeof(char),56))),_tag_dyneither(_tmp85B,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp336->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp860;void*_tmp85F[1];struct Cyc_String_pa_struct _tmp85E;return(
_tmp85E.tag=0,((_tmp85E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp85F[0]=& _tmp85E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp860="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp860,sizeof(char),81))),_tag_dyneither(_tmp85F,sizeof(void*),1)))))));}}
return t2;};}_LL18A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp338=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp331;if(_tmp338->tag != 13)goto _LL18C;else{_tmp339=_tmp338->f1;_tmp33A=_tmp338->f2;}}
_LL18B: {struct Cyc_Absyn_Aggrfield*_tmp34C=Cyc_Absyn_lookup_field(_tmp33A,f);if(
_tmp34C == 0){const char*_tmp864;void*_tmp863[1];struct Cyc_String_pa_struct _tmp862;
return(_tmp862.tag=0,((_tmp862.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp863[0]=& _tmp862,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp864="type has no %s field",
_tag_dyneither(_tmp864,sizeof(char),21))),_tag_dyneither(_tmp863,sizeof(void*),1)))))));}
if((_tmp339 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp34C->type)){const char*_tmp868;void*_tmp867[1];struct Cyc_String_pa_struct
_tmp866;return(_tmp866.tag=0,((_tmp866.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp867[0]=& _tmp866,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp868="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp868,sizeof(char),56))),_tag_dyneither(_tmp867,sizeof(void*),1)))))));}return
_tmp34C->type;}_LL18C:{struct Cyc_Absyn_ArrayType_struct*_tmp33B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp331;if(_tmp33B->tag != 9)goto _LL18E;}{const char*_tmp869;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp869="size",_tag_dyneither(_tmp869,sizeof(char),5))))
== 0))goto _LL18E;};_LL18D: goto _LL18F;_LL18E:{struct Cyc_Absyn_PointerType_struct*
_tmp33D=(struct Cyc_Absyn_PointerType_struct*)_tmp331;if(_tmp33D->tag != 5)goto
_LL190;}{const char*_tmp86A;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp86A="size",
_tag_dyneither(_tmp86A,sizeof(char),5))))== 0))goto _LL190;};_LL18F:{const char*
_tmp86E;void*_tmp86D[1];struct Cyc_String_pa_struct _tmp86C;(_tmp86C.tag=0,((
_tmp86C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp86D[0]=& _tmp86C,Cyc_Tcutil_warn(e->loc,((_tmp86E="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp86E,sizeof(char),54))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp874;struct Cyc_List_List*_tmp873;struct Cyc_Absyn_Primop_e_struct*
_tmp872;outer_e->r=(void*)((_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((
_tmp874.tag=3,((_tmp874.f1=Cyc_Absyn_Numelts,((_tmp874.f2=((_tmp873=_cycalloc(
sizeof(*_tmp873)),((_tmp873->hd=e,((_tmp873->tl=0,_tmp873)))))),_tmp874)))))),
_tmp872))));}return Cyc_Absyn_uint_typ;_LL190:;_LL191: {const char*_tmp875;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp875="size",_tag_dyneither(_tmp875,sizeof(char),5))))
== 0){const char*_tmp879;void*_tmp878[1];struct Cyc_String_pa_struct _tmp877;return(
_tmp877.tag=0,((_tmp877.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp878[0]=& _tmp877,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp879="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp879,sizeof(char),44))),_tag_dyneither(_tmp878,sizeof(void*),1)))))));}
else{const char*_tmp87D;void*_tmp87C[1];struct Cyc_String_pa_struct _tmp87B;return(
_tmp87B.tag=0,((_tmp87B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp87C[0]=& _tmp87B,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp87D="expecting struct or union, found %s",_tag_dyneither(
_tmp87D,sizeof(char),36))),_tag_dyneither(_tmp87C,sizeof(void*),1)))))));}}
_LL187:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp360=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(
_tmp360,Cyc_Tcenv_clear_notreadctxt(_tmp360,te)),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp361=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp363;void*_tmp364;struct Cyc_Absyn_PtrAtts _tmp365;void*
_tmp366;union Cyc_Absyn_Constraint*_tmp367;union Cyc_Absyn_Constraint*_tmp368;
_LL193: {struct Cyc_Absyn_PointerType_struct*_tmp362=(struct Cyc_Absyn_PointerType_struct*)
_tmp361;if(_tmp362->tag != 5)goto _LL195;else{_tmp363=_tmp362->f1;_tmp364=_tmp363.elt_typ;
_tmp365=_tmp363.ptr_atts;_tmp366=_tmp365.rgn;_tmp367=_tmp365.bounds;_tmp368=
_tmp365.zero_term;}}_LL194: Cyc_Tcutil_check_nonzero_bound(loc,_tmp367);{void*
_tmp369=Cyc_Tcutil_compress(_tmp364);struct Cyc_Absyn_AggrInfo _tmp36B;union Cyc_Absyn_AggrInfoU
_tmp36C;struct Cyc_Absyn_Aggrdecl**_tmp36D;struct Cyc_Absyn_Aggrdecl*_tmp36E;
struct Cyc_List_List*_tmp36F;enum Cyc_Absyn_AggrKind _tmp371;struct Cyc_List_List*
_tmp372;_LL198: {struct Cyc_Absyn_AggrType_struct*_tmp36A=(struct Cyc_Absyn_AggrType_struct*)
_tmp369;if(_tmp36A->tag != 12)goto _LL19A;else{_tmp36B=_tmp36A->f1;_tmp36C=_tmp36B.aggr_info;
if((_tmp36C.KnownAggr).tag != 2)goto _LL19A;_tmp36D=(struct Cyc_Absyn_Aggrdecl**)(
_tmp36C.KnownAggr).val;_tmp36E=*_tmp36D;_tmp36F=_tmp36B.targs;}}_LL199: {struct
Cyc_Absyn_Aggrfield*_tmp373=Cyc_Absyn_lookup_decl_field(_tmp36E,f);if(_tmp373 == 
0){const char*_tmp882;void*_tmp881[2];struct Cyc_String_pa_struct _tmp880;struct Cyc_String_pa_struct
_tmp87F;return(_tmp87F.tag=0,((_tmp87F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp880.tag=0,((_tmp880.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp36E->name)),((_tmp881[0]=& _tmp880,((
_tmp881[1]=& _tmp87F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp882="type %s has no %s field",
_tag_dyneither(_tmp882,sizeof(char),24))),_tag_dyneither(_tmp881,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36E->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp378=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp379=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp378,_tmp378,_tmp36E->tvs,_tmp36F);t3=Cyc_Tcutil_rsubstitute(_tmp378,_tmp379,
_tmp373->type);}{struct Cyc_Absyn_Kind*_tmp37A=Cyc_Tcutil_typ_kind(t3);if(Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,_tmp37A)){const char*_tmp886;void*_tmp885[1];struct Cyc_String_pa_struct
_tmp884;return(_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp885[0]=& _tmp884,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp886="cannot get member %s since its type is abstract",_tag_dyneither(_tmp886,
sizeof(char),48))),_tag_dyneither(_tmp885,sizeof(void*),1)))))));}if(((_tmp36E->kind
== Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36E->impl))->tagged)
 && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){const char*
_tmp88A;void*_tmp889[1];struct Cyc_String_pa_struct _tmp888;return(_tmp888.tag=0,((
_tmp888.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp889[0]=&
_tmp888,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88A="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp88A,sizeof(char),56))),_tag_dyneither(_tmp889,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36E->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp88E;void*_tmp88D[1];struct Cyc_String_pa_struct _tmp88C;return(
_tmp88C.tag=0,((_tmp88C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp88D[0]=& _tmp88C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88E="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp88E,sizeof(char),72))),_tag_dyneither(_tmp88D,sizeof(void*),1)))))));}}
return t3;};};}_LL19A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp370=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp369;if(_tmp370->tag != 13)goto _LL19C;else{_tmp371=_tmp370->f1;_tmp372=_tmp370->f2;}}
_LL19B: {struct Cyc_Absyn_Aggrfield*_tmp384=Cyc_Absyn_lookup_field(_tmp372,f);if(
_tmp384 == 0){const char*_tmp892;void*_tmp891[1];struct Cyc_String_pa_struct _tmp890;
return(_tmp890.tag=0,((_tmp890.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp891[0]=& _tmp890,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp892="type has no %s field",
_tag_dyneither(_tmp892,sizeof(char),21))),_tag_dyneither(_tmp891,sizeof(void*),1)))))));}
if((_tmp371 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp384->type)){const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_struct
_tmp894;return(_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp895[0]=& _tmp894,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp896="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp896,sizeof(char),56))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}return
_tmp384->type;}_LL19C:;_LL19D: goto _LL197;_LL197:;}goto _LL192;_LL195:;_LL196: goto
_LL192;_LL192:;}{const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_struct
_tmp898;return(_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp899[0]=& _tmp898,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89A="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp89A,sizeof(char),44))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp38F;int
_tmp390;struct _tuple14 _tmp38E=Cyc_Evexp_eval_const_uint_exp(index);_tmp38F=
_tmp38E.f1;_tmp390=_tmp38E.f2;if(!_tmp390){const char*_tmp89D;void*_tmp89C;return(
_tmp89C=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp89D="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp89D,sizeof(char),47))),_tag_dyneither(_tmp89C,sizeof(void*),0)));}{
struct _handler_cons _tmp393;_push_handler(& _tmp393);{int _tmp395=0;if(setjmp(
_tmp393.handler))_tmp395=1;if(!_tmp395){{void*_tmp396=(*((struct _tuple18*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp38F)).f2;_npop_handler(0);
return _tmp396;};_pop_handler();}else{void*_tmp394=(void*)_exn_thrown;void*
_tmp398=_tmp394;_LL19F: {struct Cyc_List_Nth_struct*_tmp399=(struct Cyc_List_Nth_struct*)
_tmp398;if(_tmp399->tag != Cyc_List_Nth)goto _LL1A1;}_LL1A0: {const char*_tmp8A2;
void*_tmp8A1[2];struct Cyc_Int_pa_struct _tmp8A0;struct Cyc_Int_pa_struct _tmp89F;
return(_tmp89F.tag=1,((_tmp89F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp8A0.tag=1,((_tmp8A0.f1=(unsigned long)((int)_tmp38F),((
_tmp8A1[0]=& _tmp8A0,((_tmp8A1[1]=& _tmp89F,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8A2="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp8A2,sizeof(char),41))),_tag_dyneither(_tmp8A1,sizeof(void*),2)))))))))))));}
_LL1A1:;_LL1A2:(void)_throw(_tmp398);_LL19E:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp39E=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp39F=Cyc_Tcenv_clear_lhs(_tmp39E,Cyc_Tcenv_clear_notreadctxt(
_tmp39E,te_orig));Cyc_Tcexp_tcExp(_tmp39F,0,e1);Cyc_Tcexp_tcExp(_tmp39F,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp39F,e2)){const char*_tmp8A6;void*_tmp8A5[1];
struct Cyc_String_pa_struct _tmp8A4;return(_tmp8A4.tag=0,((_tmp8A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp8A5[0]=&
_tmp8A4,Cyc_Tcexp_expr_err(_tmp39F,e2->loc,topt,((_tmp8A6="expecting int subscript, found %s",
_tag_dyneither(_tmp8A6,sizeof(char),34))),_tag_dyneither(_tmp8A5,sizeof(void*),1)))))));}{
void*_tmp3A3=t1;struct Cyc_Absyn_PtrInfo _tmp3A5;void*_tmp3A6;struct Cyc_Absyn_Tqual
_tmp3A7;struct Cyc_Absyn_PtrAtts _tmp3A8;void*_tmp3A9;union Cyc_Absyn_Constraint*
_tmp3AA;union Cyc_Absyn_Constraint*_tmp3AB;struct Cyc_List_List*_tmp3AD;_LL1A4: {
struct Cyc_Absyn_PointerType_struct*_tmp3A4=(struct Cyc_Absyn_PointerType_struct*)
_tmp3A3;if(_tmp3A4->tag != 5)goto _LL1A6;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A5.elt_typ;
_tmp3A7=_tmp3A5.elt_tq;_tmp3A8=_tmp3A5.ptr_atts;_tmp3A9=_tmp3A8.rgn;_tmp3AA=
_tmp3A8.bounds;_tmp3AB=_tmp3A8.zero_term;}}_LL1A5: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3AB)){int emit_warning=1;{void*_tmp3AE=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp3AA);struct Cyc_Absyn_Exp*_tmp3B0;_LL1AB: {struct Cyc_Absyn_Upper_b_struct*
_tmp3AF=(struct Cyc_Absyn_Upper_b_struct*)_tmp3AE;if(_tmp3AF->tag != 1)goto _LL1AD;
else{_tmp3B0=_tmp3AF->f1;}}_LL1AC: if(Cyc_Tcutil_is_const_exp(_tmp39F,e2)){
unsigned int _tmp3B3;int _tmp3B4;struct _tuple14 _tmp3B2=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;if(_tmp3B4){unsigned int _tmp3B6;int
_tmp3B7;struct _tuple14 _tmp3B5=Cyc_Evexp_eval_const_uint_exp(_tmp3B0);_tmp3B6=
_tmp3B5.f1;_tmp3B7=_tmp3B5.f2;if(_tmp3B7  && _tmp3B6 > _tmp3B3)emit_warning=0;}}
goto _LL1AA;_LL1AD: {struct Cyc_Absyn_DynEither_b_struct*_tmp3B1=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3AE;if(_tmp3B1->tag != 0)goto _LL1AA;}_LL1AE: emit_warning=0;goto _LL1AA;_LL1AA:;}
if(emit_warning){const char*_tmp8A9;void*_tmp8A8;(_tmp8A8=0,Cyc_Tcutil_warn(e2->loc,((
_tmp8A9="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp8A9,sizeof(char),63))),_tag_dyneither(_tmp8A8,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp39F,e2)){unsigned int _tmp3BB;int _tmp3BC;
struct _tuple14 _tmp3BA=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3BB=_tmp3BA.f1;
_tmp3BC=_tmp3BA.f2;if(_tmp3BC)Cyc_Tcutil_check_bound(loc,_tmp3BB,_tmp3AA);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3AA) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3AB)){const char*_tmp8AC;void*_tmp8AB;(_tmp8AB=0,
Cyc_Tcutil_warn(e1->loc,((_tmp8AC="subscript applied to pointer to one object",
_tag_dyneither(_tmp8AC,sizeof(char),43))),_tag_dyneither(_tmp8AB,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3AA);}}Cyc_Tcenv_check_rgn_accessible(
_tmp39F,loc,_tmp3A9);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3A6),& Cyc_Tcutil_tmk)){
const char*_tmp8AF;void*_tmp8AE;(_tmp8AE=0,Cyc_Tcutil_terr(e1->loc,((_tmp8AF="can't subscript an abstract pointer",
_tag_dyneither(_tmp8AF,sizeof(char),36))),_tag_dyneither(_tmp8AE,sizeof(void*),0)));}
return _tmp3A6;_LL1A6: {struct Cyc_Absyn_TupleType_struct*_tmp3AC=(struct Cyc_Absyn_TupleType_struct*)
_tmp3A3;if(_tmp3AC->tag != 11)goto _LL1A8;else{_tmp3AD=_tmp3AC->f1;}}_LL1A7: return
Cyc_Tcexp_ithTupleType(_tmp39F,loc,_tmp3AD,e2);_LL1A8:;_LL1A9: {const char*
_tmp8B3;void*_tmp8B2[1];struct Cyc_String_pa_struct _tmp8B1;return(_tmp8B1.tag=0,((
_tmp8B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp8B2[0]=& _tmp8B1,Cyc_Tcexp_expr_err(_tmp39F,loc,topt,((_tmp8B3="subscript applied to %s",
_tag_dyneither(_tmp8B3,sizeof(char),24))),_tag_dyneither(_tmp8B2,sizeof(void*),1)))))));}
_LL1A3:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp3C4=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3C6;
_LL1B0: {struct Cyc_Absyn_TupleType_struct*_tmp3C5=(struct Cyc_Absyn_TupleType_struct*)
_tmp3C4;if(_tmp3C5->tag != 11)goto _LL1B2;else{_tmp3C6=_tmp3C5->f1;}}_LL1B1: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3C6)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AF;for(0;es != 0;(es=es->tl,_tmp3C6=_tmp3C6->tl)){
int bogus=0;void*_tmp3C7=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(
_tmp3C6))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3C7,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)
_tmp3C6->hd)).f2,& bogus);{struct _tuple18*_tmp8B6;struct Cyc_List_List*_tmp8B5;
fields=((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5->hd=((_tmp8B6=_cycalloc(
sizeof(*_tmp8B6)),((_tmp8B6->f1=(*((struct _tuple18*)_tmp3C6->hd)).f1,((_tmp8B6->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp8B6)))))),((_tmp8B5->tl=fields,_tmp8B5))))));};}done=1;goto _LL1AF;_LL1B2:;
_LL1B3: goto _LL1AF;_LL1AF:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);{struct _tuple18*_tmp8B9;struct Cyc_List_List*
_tmp8B8;fields=((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8->hd=((_tmp8B9=
_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9->f1=Cyc_Absyn_empty_tqual(0),((_tmp8B9->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp8B9)))))),((_tmp8B8->tl=fields,_tmp8B8))))));};}{struct Cyc_Absyn_TupleType_struct
_tmp8BC;struct Cyc_Absyn_TupleType_struct*_tmp8BB;return(void*)((_tmp8BB=
_cycalloc(sizeof(*_tmp8BB)),((_tmp8BB[0]=((_tmp8BC.tag=11,((_tmp8BC.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8BC)))),
_tmp8BB))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){
const char*_tmp8BF;void*_tmp8BE;return(_tmp8BE=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8BF="tcCompoundLit",_tag_dyneither(_tmp8BF,sizeof(char),14))),_tag_dyneither(
_tmp8BE,sizeof(void*),0)));}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,struct Cyc_List_List*
des){void*res_t2;struct _RegionHandle*_tmp3D0=Cyc_Tcenv_get_fnrgn(te);{int _tmp3D1=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp3D0,(struct Cyc_Absyn_Exp*(*)(
struct _tuple17*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp8C2;struct Cyc_Absyn_Const_e_struct*_tmp8C1;struct Cyc_Absyn_Const_e_struct*
_tmp3D2=(_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8C2.tag=0,((
_tmp8C2.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3D1),_tmp8C2)))),_tmp8C1)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3D2,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3D3=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp3D1 - 1);if(!Cyc_Tcutil_is_zero(_tmp3D3)){const char*_tmp8C5;
void*_tmp8C4;(_tmp8C4=0,Cyc_Tcutil_terr(_tmp3D3->loc,((_tmp8C5="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp8C5,sizeof(char),45))),_tag_dyneither(_tmp8C4,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp8C6;sz_exp->topt=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((
_tmp8C6->v=Cyc_Absyn_uint_typ,_tmp8C6))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp8CA;void*_tmp8C9[1];struct Cyc_String_pa_struct _tmp8C8;(
_tmp8C8.tag=0,((_tmp8C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp8C9[0]=& _tmp8C8,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp8CA="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp8CA,sizeof(char),53))),_tag_dyneither(_tmp8C9,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple17*)des->hd)).f1;if(ds != 0){void*_tmp3DC=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3DF;_LL1B5: {struct Cyc_Absyn_FieldName_struct*_tmp3DD=(struct Cyc_Absyn_FieldName_struct*)
_tmp3DC;if(_tmp3DD->tag != 1)goto _LL1B7;}_LL1B6:{const char*_tmp8CD;void*_tmp8CC;(
_tmp8CC=0,Cyc_Tcutil_terr(loc,((_tmp8CD="only array index designators are supported",
_tag_dyneither(_tmp8CD,sizeof(char),43))),_tag_dyneither(_tmp8CC,sizeof(void*),0)));}
goto _LL1B4;_LL1B7: {struct Cyc_Absyn_ArrayElement_struct*_tmp3DE=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3DC;if(_tmp3DE->tag != 0)goto _LL1B4;else{_tmp3DF=_tmp3DE->f1;}}_LL1B8: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3DF);{unsigned int _tmp3E3;int _tmp3E4;struct _tuple14 _tmp3E2=Cyc_Evexp_eval_const_uint_exp(
_tmp3DF);_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;if(!_tmp3E4){const char*_tmp8D0;
void*_tmp8CF;(_tmp8CF=0,Cyc_Tcutil_terr(_tmp3DF->loc,((_tmp8D0="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp8D0,sizeof(char),47))),_tag_dyneither(_tmp8CF,sizeof(void*),0)));}
else{if(_tmp3E3 != offset){const char*_tmp8D5;void*_tmp8D4[2];struct Cyc_Int_pa_struct
_tmp8D3;struct Cyc_Int_pa_struct _tmp8D2;(_tmp8D2.tag=1,((_tmp8D2.f1=(
unsigned long)((int)_tmp3E3),((_tmp8D3.tag=1,((_tmp8D3.f1=(unsigned long)offset,((
_tmp8D4[0]=& _tmp8D3,((_tmp8D4[1]=& _tmp8D2,Cyc_Tcutil_terr(_tmp3DF->loc,((_tmp8D5="expecting index designator %d but found %d",
_tag_dyneither(_tmp8D5,sizeof(char),43))),_tag_dyneither(_tmp8D4,sizeof(void*),2)))))))))))));}}
goto _LL1B4;};_LL1B4:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3EB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1BA: {struct Cyc_Absyn_TagType_struct*_tmp3EC=(
struct Cyc_Absyn_TagType_struct*)_tmp3EB;if(_tmp3EC->tag != 20)goto _LL1BC;}_LL1BB:
goto _LL1B9;_LL1BC:;_LL1BD: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp8D9;void*_tmp8D8[1];struct Cyc_String_pa_struct _tmp8D7;(_tmp8D7.tag=0,((
_tmp8D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp8D8[0]=& _tmp8D7,
Cyc_Tcutil_terr(bound->loc,((_tmp8D9="expecting unsigned int, found %s",
_tag_dyneither(_tmp8D9,sizeof(char),33))),_tag_dyneither(_tmp8D8,sizeof(void*),1)))))));}
_LL1B9:;}if(!(vd->tq).real_const){const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8DC="comprehension index variable is not declared const!",_tag_dyneither(
_tmp8DC,sizeof(char),52))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3F2=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3F2,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3F2,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3F2,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3F3=te2;void**_tmp3F4=0;struct Cyc_Absyn_Tqual*
_tmp3F5=0;union Cyc_Absyn_Constraint**_tmp3F6=0;if(topt != 0){void*_tmp3F7=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3F9;struct Cyc_Absyn_ArrayInfo _tmp3FB;void*
_tmp3FC;struct Cyc_Absyn_Tqual _tmp3FD;struct Cyc_Absyn_Exp*_tmp3FE;union Cyc_Absyn_Constraint*
_tmp3FF;_LL1BF: {struct Cyc_Absyn_PointerType_struct*_tmp3F8=(struct Cyc_Absyn_PointerType_struct*)
_tmp3F7;if(_tmp3F8->tag != 5)goto _LL1C1;else{_tmp3F9=_tmp3F8->f1;}}_LL1C0:{void**
_tmp8DD;_tmp3F4=((_tmp8DD=_region_malloc(_tmp3F2,sizeof(*_tmp8DD)),((_tmp8DD[0]=
_tmp3F9.elt_typ,_tmp8DD))));}{struct Cyc_Absyn_Tqual*_tmp8DE;_tmp3F5=((_tmp8DE=
_region_malloc(_tmp3F2,sizeof(*_tmp8DE)),((_tmp8DE[0]=_tmp3F9.elt_tq,_tmp8DE))));}{
union Cyc_Absyn_Constraint**_tmp8DF;_tmp3F6=((_tmp8DF=_region_malloc(_tmp3F2,
sizeof(*_tmp8DF)),((_tmp8DF[0]=(_tmp3F9.ptr_atts).zero_term,_tmp8DF))));}goto
_LL1BE;_LL1C1: {struct Cyc_Absyn_ArrayType_struct*_tmp3FA=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3F7;if(_tmp3FA->tag != 9)goto _LL1C3;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FB.elt_type;
_tmp3FD=_tmp3FB.tq;_tmp3FE=_tmp3FB.num_elts;_tmp3FF=_tmp3FB.zero_term;}}_LL1C2:{
void**_tmp8E0;_tmp3F4=((_tmp8E0=_region_malloc(_tmp3F2,sizeof(*_tmp8E0)),((
_tmp8E0[0]=_tmp3FC,_tmp8E0))));}{struct Cyc_Absyn_Tqual*_tmp8E1;_tmp3F5=((_tmp8E1=
_region_malloc(_tmp3F2,sizeof(*_tmp8E1)),((_tmp8E1[0]=_tmp3FD,_tmp8E1))));}{
union Cyc_Absyn_Constraint**_tmp8E2;_tmp3F6=((_tmp8E2=_region_malloc(_tmp3F2,
sizeof(*_tmp8E2)),((_tmp8E2[0]=_tmp3FF,_tmp8E2))));}goto _LL1BE;_LL1C3:;_LL1C4:
goto _LL1BE;_LL1BE:;}{void*t=Cyc_Tcexp_tcExp(_tmp3F3,_tmp3F4,body);if(_tmp3F3->le
== 0){if(!Cyc_Tcutil_is_const_exp(_tmp3F3,bound)){const char*_tmp8E5;void*_tmp8E4;(
_tmp8E4=0,Cyc_Tcutil_terr(bound->loc,((_tmp8E5="bound is not constant",
_tag_dyneither(_tmp8E5,sizeof(char),22))),_tag_dyneither(_tmp8E4,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(_tmp3F3,body)){const char*_tmp8E8;void*_tmp8E7;(
_tmp8E7=0,Cyc_Tcutil_terr(bound->loc,((_tmp8E8="body is not constant",
_tag_dyneither(_tmp8E8,sizeof(char),21))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}}
if(_tmp3F6 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,*_tmp3F6)){struct Cyc_Absyn_Exp*_tmp40A=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*
_tmp8E9;_tmp40A->topt=((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9->v=Cyc_Absyn_uint_typ,
_tmp8E9))));}bound=Cyc_Absyn_add_exp(bound,_tmp40A,0);{struct Cyc_Core_Opt*
_tmp8EA;bound->topt=((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->v=Cyc_Absyn_uint_typ,
_tmp8EA))));}*is_zero_term=1;}{void*_tmp40D=Cyc_Absyn_array_typ(t,_tmp3F5 == 0?
Cyc_Absyn_empty_tqual(0):*_tmp3F5,(struct Cyc_Absyn_Exp*)bound,_tmp3F6 == 0?Cyc_Absyn_false_conref:*
_tmp3F6,0);return _tmp40D;};};};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp40E;_push_handler(& _tmp40E);{int _tmp410=0;if(setjmp(
_tmp40E.handler))_tmp410=1;if(!_tmp410){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp40F=(void*)_exn_thrown;void*_tmp412=_tmp40F;_LL1C6: {
struct Cyc_Dict_Absent_struct*_tmp413=(struct Cyc_Dict_Absent_struct*)_tmp412;if(
_tmp413->tag != Cyc_Dict_Absent)goto _LL1C8;}_LL1C7:{const char*_tmp8EE;void*
_tmp8ED[1];struct Cyc_String_pa_struct _tmp8EC;(_tmp8EC.tag=0,((_tmp8EC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8ED[0]=&
_tmp8EC,Cyc_Tcutil_terr(loc,((_tmp8EE="unbound struct/union name %s",
_tag_dyneither(_tmp8EE,sizeof(char),29))),_tag_dyneither(_tmp8ED,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1C8:;_LL1C9:(void)_throw(
_tmp412);_LL1C5:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp8F6;void*_tmp8F5[1];const char*_tmp8F4;const char*_tmp8F3;
struct Cyc_String_pa_struct _tmp8F2;(_tmp8F2.tag=0,((_tmp8F2.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8F3="struct",_tag_dyneither(
_tmp8F3,sizeof(char),7)):((_tmp8F4="union",_tag_dyneither(_tmp8F4,sizeof(char),6)))),((
_tmp8F5[0]=& _tmp8F2,Cyc_Tcutil_terr(loc,((_tmp8F6="can't construct abstract %s",
_tag_dyneither(_tmp8F6,sizeof(char),28))),_tag_dyneither(_tmp8F5,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp41C=Cyc_Tcenv_get_fnrgn(te);struct _tuple11 _tmp8F7;struct
_tuple11 _tmp41D=(_tmp8F7.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8F7.f2=_tmp41C,
_tmp8F7)));struct Cyc_List_List*_tmp41E=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41C,Cyc_Tcutil_r_make_inst_var,&
_tmp41D,ad->tvs);struct Cyc_List_List*_tmp41F=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp41C,Cyc_Tcutil_r_make_inst_var,&
_tmp41D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp420=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp41E);struct Cyc_List_List*_tmp421=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp41F);struct Cyc_List_List*_tmp422=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp41C,_tmp41E,_tmp41F);struct Cyc_Absyn_AggrType_struct _tmp901;struct Cyc_Absyn_Aggrdecl**
_tmp900;struct Cyc_Absyn_AggrInfo _tmp8FF;struct Cyc_Absyn_AggrType_struct*_tmp8FE;
struct Cyc_Absyn_AggrType_struct*_tmp423=(_tmp8FE=_cycalloc(sizeof(*_tmp8FE)),((
_tmp8FE[0]=((_tmp901.tag=12,((_tmp901.f1=((_tmp8FF.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900[0]=ad,_tmp900))))),((_tmp8FF.targs=
_tmp420,_tmp8FF)))),_tmp901)))),_tmp8FE)));struct Cyc_List_List*_tmp424=*ts;
struct Cyc_List_List*_tmp425=_tmp421;while(_tmp424 != 0  && _tmp425 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp424->hd);Cyc_Tcutil_check_no_qual(
loc,(void*)_tmp424->hd);Cyc_Tcutil_unify((void*)_tmp424->hd,(void*)_tmp425->hd);
_tmp424=_tmp424->tl;_tmp425=_tmp425->tl;}if(_tmp424 != 0){const char*_tmp904;void*
_tmp903;(_tmp903=0,Cyc_Tcutil_terr(loc,((_tmp904="too many explicit witness types",
_tag_dyneither(_tmp904,sizeof(char),32))),_tag_dyneither(_tmp903,sizeof(void*),0)));}*
ts=_tmp421;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp423,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp41C,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields);for(0;fields != 0;fields=fields->tl){int bogus=0;
struct _tuple19 _tmp429;struct Cyc_Absyn_Aggrfield*_tmp42A;struct Cyc_Absyn_Exp*
_tmp42B;struct _tuple19*_tmp428=(struct _tuple19*)fields->hd;_tmp429=*_tmp428;
_tmp42A=_tmp429.f1;_tmp42B=_tmp429.f2;{void*_tmp42C=Cyc_Tcutil_rsubstitute(
_tmp41C,_tmp422,_tmp42A->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp42C,
_tmp42B);if(!Cyc_Tcutil_coerce_arg(te,_tmp42B,_tmp42C,& bogus)){{const char*
_tmp910;void*_tmp90F[5];struct Cyc_String_pa_struct _tmp90E;const char*_tmp90D;
const char*_tmp90C;struct Cyc_String_pa_struct _tmp90B;struct Cyc_String_pa_struct
_tmp90A;struct Cyc_String_pa_struct _tmp909;struct Cyc_String_pa_struct _tmp908;(
_tmp908.tag=0,((_tmp908.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp42B->topt))->v)),((_tmp909.tag=0,((
_tmp909.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp42C)),((_tmp90A.tag=0,((_tmp90A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp90B.tag=0,((_tmp90B.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp90C="struct",
_tag_dyneither(_tmp90C,sizeof(char),7)):((_tmp90D="union",_tag_dyneither(_tmp90D,
sizeof(char),6)))),((_tmp90E.tag=0,((_tmp90E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp42A->name),((_tmp90F[0]=& _tmp90E,((_tmp90F[1]=& _tmp90B,((
_tmp90F[2]=& _tmp90A,((_tmp90F[3]=& _tmp909,((_tmp90F[4]=& _tmp908,Cyc_Tcutil_terr(
_tmp42B->loc,((_tmp910="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp910,sizeof(char),40))),_tag_dyneither(_tmp90F,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp436=0;{struct Cyc_List_List*
_tmp437=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp437 != 0;_tmp437=_tmp437->tl){struct _tuple0*_tmp913;struct Cyc_List_List*
_tmp912;_tmp436=((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912->hd=((_tmp913=
_cycalloc(sizeof(*_tmp913)),((_tmp913->f1=Cyc_Tcutil_rsubstitute(_tmp41C,_tmp422,(*((
struct _tuple0*)_tmp437->hd)).f1),((_tmp913->f2=Cyc_Tcutil_rsubstitute(_tmp41C,
_tmp422,(*((struct _tuple0*)_tmp437->hd)).f2),_tmp913)))))),((_tmp912->tl=_tmp436,
_tmp912))))));}}_tmp436=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp436);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp436);return(void*)_tmp423;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp43F=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp440=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp442;struct
Cyc_List_List*_tmp443;_LL1CB: {struct Cyc_Absyn_AnonAggrType_struct*_tmp441=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp440;if(_tmp441->tag != 13)goto _LL1CD;
else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;}}_LL1CC: if(_tmp442 == Cyc_Absyn_UnionA){
const char*_tmp916;void*_tmp915;(_tmp915=0,Cyc_Tcutil_terr(loc,((_tmp916="expecting struct but found union",
_tag_dyneither(_tmp916,sizeof(char),33))),_tag_dyneither(_tmp915,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp43F,
loc,args,Cyc_Absyn_StructA,_tmp443);for(0;fields != 0;fields=fields->tl){int bogus=
0;struct _tuple19 _tmp447;struct Cyc_Absyn_Aggrfield*_tmp448;struct Cyc_Absyn_Exp*
_tmp449;struct _tuple19*_tmp446=(struct _tuple19*)fields->hd;_tmp447=*_tmp446;
_tmp448=_tmp447.f1;_tmp449=_tmp447.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp448->type,_tmp449);if(!Cyc_Tcutil_coerce_arg(te,_tmp449,_tmp448->type,& bogus)){{
const char*_tmp91C;void*_tmp91B[3];struct Cyc_String_pa_struct _tmp91A;struct Cyc_String_pa_struct
_tmp919;struct Cyc_String_pa_struct _tmp918;(_tmp918.tag=0,((_tmp918.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp449->topt))->v)),((_tmp919.tag=0,((_tmp919.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp448->type)),((
_tmp91A.tag=0,((_tmp91A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp448->name),((
_tmp91B[0]=& _tmp91A,((_tmp91B[1]=& _tmp919,((_tmp91B[2]=& _tmp918,Cyc_Tcutil_terr(
_tmp449->loc,((_tmp91C="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp91C,sizeof(char),41))),_tag_dyneither(_tmp91B,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CA;};_LL1CD:;_LL1CE: {const char*_tmp91F;
void*_tmp91E;(_tmp91E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp91F="tcAnonStruct: wrong type",_tag_dyneither(_tmp91F,
sizeof(char),25))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}_LL1CA:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp451=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp920;struct _tuple11 _tmp452=(_tmp920.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp920.f2=_tmp451,_tmp920)));struct Cyc_List_List*_tmp453=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp451,Cyc_Tcutil_r_make_inst_var,&
_tmp452,tud->tvs);struct Cyc_List_List*_tmp454=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,_tmp453);struct Cyc_Absyn_DatatypeFieldType_struct _tmp926;struct Cyc_Absyn_DatatypeFieldInfo
_tmp925;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp924;void*res=(void*)((
_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((_tmp926.tag=4,((_tmp926.f1=((
_tmp925.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp925.targs=_tmp454,
_tmp925)))),_tmp926)))),_tmp924))));if(topt != 0){void*_tmp455=Cyc_Tcutil_compress(*
topt);_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp456=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp455;if(_tmp456->tag != 4)goto _LL1D2;}_LL1D1: Cyc_Tcutil_unify(*topt,res);goto
_LL1CF;_LL1D2:;_LL1D3: goto _LL1CF;_LL1CF:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp451,_tmp453,(*((
struct _tuple18*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp92E;void*_tmp92D[3];struct Cyc_String_pa_struct
_tmp92C;struct Cyc_String_pa_struct _tmp92B;const char*_tmp92A;struct Cyc_String_pa_struct
_tmp929;(_tmp929.tag=0,((_tmp929.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp92A="?",_tag_dyneither(_tmp92A,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp92B.tag=0,((_tmp92B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp92C.tag=0,((_tmp92C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp92D[0]=& _tmp92C,((_tmp92D[1]=& _tmp92B,((
_tmp92D[2]=& _tmp929,Cyc_Tcutil_terr(e->loc,((_tmp92E="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp92E,sizeof(char),82))),_tag_dyneither(_tmp92D,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp932;void*_tmp931[1];
struct Cyc_String_pa_struct _tmp930;return(_tmp930.tag=0,((_tmp930.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp931[0]=& _tmp930,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp932="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp932,sizeof(char),47))),_tag_dyneither(_tmp931,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp936;void*_tmp935[1];struct Cyc_String_pa_struct _tmp934;return(
_tmp934.tag=0,((_tmp934.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp935[0]=& _tmp934,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp936="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp936,sizeof(char),46))),_tag_dyneither(_tmp935,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp467=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp46E;struct Cyc_Absyn_PtrAtts _tmp46F;union Cyc_Absyn_Constraint*
_tmp470;struct Cyc_Absyn_ArrayInfo _tmp475;void*_tmp476;struct Cyc_List_List*
_tmp479;struct Cyc_Absyn_AggrInfo _tmp47B;union Cyc_Absyn_AggrInfoU _tmp47C;struct
Cyc_List_List*_tmp47D;struct Cyc_List_List*_tmp481;_LL1D5: {struct Cyc_Absyn_VoidType_struct*
_tmp468=(struct Cyc_Absyn_VoidType_struct*)_tmp467;if(_tmp468->tag != 0)goto _LL1D7;}
_LL1D6: return 1;_LL1D7: {struct Cyc_Absyn_Evar_struct*_tmp469=(struct Cyc_Absyn_Evar_struct*)
_tmp467;if(_tmp469->tag != 1)goto _LL1D9;}_LL1D8: goto _LL1DA;_LL1D9: {struct Cyc_Absyn_VarType_struct*
_tmp46A=(struct Cyc_Absyn_VarType_struct*)_tmp467;if(_tmp46A->tag != 2)goto _LL1DB;}
_LL1DA: goto _LL1DC;_LL1DB: {struct Cyc_Absyn_DatatypeType_struct*_tmp46B=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp467;if(_tmp46B->tag != 3)goto _LL1DD;}_LL1DC:
goto _LL1DE;_LL1DD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp46C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp467;if(_tmp46C->tag != 4)goto _LL1DF;}_LL1DE: return 0;_LL1DF: {struct Cyc_Absyn_PointerType_struct*
_tmp46D=(struct Cyc_Absyn_PointerType_struct*)_tmp467;if(_tmp46D->tag != 5)goto
_LL1E1;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46E.ptr_atts;_tmp470=_tmp46F.nullable;}}
_LL1E0: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp470);_LL1E1: {struct Cyc_Absyn_IntType_struct*_tmp471=(struct Cyc_Absyn_IntType_struct*)
_tmp467;if(_tmp471->tag != 6)goto _LL1E3;}_LL1E2: goto _LL1E4;_LL1E3: {struct Cyc_Absyn_FloatType_struct*
_tmp472=(struct Cyc_Absyn_FloatType_struct*)_tmp467;if(_tmp472->tag != 7)goto
_LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_Absyn_DoubleType_struct*_tmp473=(
struct Cyc_Absyn_DoubleType_struct*)_tmp467;if(_tmp473->tag != 8)goto _LL1E7;}
_LL1E6: return 1;_LL1E7: {struct Cyc_Absyn_ArrayType_struct*_tmp474=(struct Cyc_Absyn_ArrayType_struct*)
_tmp467;if(_tmp474->tag != 9)goto _LL1E9;else{_tmp475=_tmp474->f1;_tmp476=_tmp475.elt_type;}}
_LL1E8: return Cyc_Tcexp_zeroable_type(_tmp476);_LL1E9: {struct Cyc_Absyn_FnType_struct*
_tmp477=(struct Cyc_Absyn_FnType_struct*)_tmp467;if(_tmp477->tag != 10)goto _LL1EB;}
_LL1EA: return 0;_LL1EB: {struct Cyc_Absyn_TupleType_struct*_tmp478=(struct Cyc_Absyn_TupleType_struct*)
_tmp467;if(_tmp478->tag != 11)goto _LL1ED;else{_tmp479=_tmp478->f1;}}_LL1EC: for(0;(
unsigned int)_tmp479;_tmp479=_tmp479->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple18*)_tmp479->hd)).f2))return 0;}return 1;_LL1ED: {struct Cyc_Absyn_AggrType_struct*
_tmp47A=(struct Cyc_Absyn_AggrType_struct*)_tmp467;if(_tmp47A->tag != 12)goto
_LL1EF;else{_tmp47B=_tmp47A->f1;_tmp47C=_tmp47B.aggr_info;_tmp47D=_tmp47B.targs;}}
_LL1EE: {struct Cyc_Absyn_Aggrdecl*_tmp48E=Cyc_Absyn_get_known_aggrdecl(_tmp47C);
if(_tmp48E->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48E->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp48F=_new_region("r");struct _RegionHandle*r=&
_tmp48F;_push_region(r);{struct Cyc_List_List*_tmp490=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp48E->tvs,_tmp47D);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp48E->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp490,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp491=0;_npop_handler(0);return _tmp491;}}}{int _tmp492=1;_npop_handler(0);
return _tmp492;};};_pop_region(r);};}_LL1EF: {struct Cyc_Absyn_EnumType_struct*
_tmp47E=(struct Cyc_Absyn_EnumType_struct*)_tmp467;if(_tmp47E->tag != 14)goto
_LL1F1;}_LL1F0: return 1;_LL1F1: {struct Cyc_Absyn_TagType_struct*_tmp47F=(struct
Cyc_Absyn_TagType_struct*)_tmp467;if(_tmp47F->tag != 20)goto _LL1F3;}_LL1F2: return
1;_LL1F3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp480=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp467;if(_tmp480->tag != 13)goto _LL1F5;else{_tmp481=_tmp480->f2;}}_LL1F4: for(0;
_tmp481 != 0;_tmp481=_tmp481->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp481->hd)->type))return 0;}return 1;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp482=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp467;if(_tmp482->tag != 15)goto
_LL1F7;}_LL1F6: return 1;_LL1F7: {struct Cyc_Absyn_TypedefType_struct*_tmp483=(
struct Cyc_Absyn_TypedefType_struct*)_tmp467;if(_tmp483->tag != 18)goto _LL1F9;}
_LL1F8: return 0;_LL1F9: {struct Cyc_Absyn_DynRgnType_struct*_tmp484=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp467;if(_tmp484->tag != 17)goto _LL1FB;}_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp485=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp467;if(_tmp485->tag != 16)goto
_LL1FD;}_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_TypeDeclType_struct*_tmp486=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp467;if(_tmp486->tag != 27)goto _LL1FF;}
_LL1FE: goto _LL200;_LL1FF: {struct Cyc_Absyn_ValueofType_struct*_tmp487=(struct Cyc_Absyn_ValueofType_struct*)
_tmp467;if(_tmp487->tag != 19)goto _LL201;}_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_HeapRgn_struct*
_tmp488=(struct Cyc_Absyn_HeapRgn_struct*)_tmp467;if(_tmp488->tag != 21)goto _LL203;}
_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_UniqueRgn_struct*_tmp489=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp467;if(_tmp489->tag != 22)goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp48A=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp467;if(_tmp48A->tag != 23)goto
_LL207;}_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_AccessEff_struct*_tmp48B=(
struct Cyc_Absyn_AccessEff_struct*)_tmp467;if(_tmp48B->tag != 24)goto _LL209;}
_LL208: goto _LL20A;_LL209: {struct Cyc_Absyn_JoinEff_struct*_tmp48C=(struct Cyc_Absyn_JoinEff_struct*)
_tmp467;if(_tmp48C->tag != 25)goto _LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp48D=(struct Cyc_Absyn_RgnsEff_struct*)_tmp467;if(_tmp48D->tag != 26)goto _LL1D4;}
_LL20C: {const char*_tmp93A;void*_tmp939[1];struct Cyc_String_pa_struct _tmp938;(
_tmp938.tag=0,((_tmp938.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp939[0]=& _tmp938,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp93A="bad type `%s' in zeroable type",_tag_dyneither(
_tmp93A,sizeof(char),31))),_tag_dyneither(_tmp939,sizeof(void*),1)))))));}_LL1D4:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp496=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp498;void*_tmp499;_LL20E: {struct Cyc_Absyn_PointerType_struct*_tmp497=(struct
Cyc_Absyn_PointerType_struct*)_tmp496;if(_tmp497->tag != 5)goto _LL210;else{
_tmp498=_tmp497->f1;_tmp499=_tmp498.elt_typ;}}_LL20F: Cyc_Tcutil_unify(_tmp499,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20D;_LL210:;_LL211: goto _LL20D;_LL20D:;}{const char*_tmp943;void*_tmp942[2];
const char*_tmp941;const char*_tmp940;struct Cyc_String_pa_struct _tmp93F;struct Cyc_String_pa_struct
_tmp93E;(_tmp93E.tag=0,((_tmp93E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp93F.tag=0,((_tmp93F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp940="calloc",
_tag_dyneither(_tmp940,sizeof(char),7))):(struct _dyneither_ptr)((_tmp941="malloc",
_tag_dyneither(_tmp941,sizeof(char),7))))),((_tmp942[0]=& _tmp93F,((_tmp942[1]=&
_tmp93E,Cyc_Tcutil_terr(loc,((_tmp943="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp943,sizeof(char),60))),_tag_dyneither(_tmp942,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL212: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL213: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp946;
struct Cyc_Absyn_RgnHandleType_struct*_tmp945;void*expected_type=(void*)((_tmp945=
_cycalloc(sizeof(*_tmp945)),((_tmp945[0]=((_tmp946.tag=16,((_tmp946.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp946)))),_tmp945))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));void*_tmp4A0=Cyc_Tcutil_compress(
handle_type);void*_tmp4A2;_LL216: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4A1=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4A0;if(_tmp4A1->tag != 16)goto _LL218;
else{_tmp4A2=(void*)_tmp4A1->f1;}}_LL217: rgn=_tmp4A2;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL215;_LL218:;_LL219:{const char*_tmp94A;void*_tmp949[1];struct
Cyc_String_pa_struct _tmp948;(_tmp948.tag=0,((_tmp948.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp949[0]=& _tmp948,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp94A="expecting region_t type but found %s",
_tag_dyneither(_tmp94A,sizeof(char),37))),_tag_dyneither(_tmp949,sizeof(void*),1)))))));}
goto _LL215;_LL215:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp94D;void*_tmp94C;(
_tmp94C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp94D="calloc with empty type",_tag_dyneither(_tmp94D,sizeof(char),23))),
_tag_dyneither(_tmp94C,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcutil_check_no_qual(loc,elt_type);Cyc_Tcexp_check_malloc_type(1,
loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*er=(*e)->r;retry_sizeof: {
void*_tmp4AA=er;void*_tmp4AC;enum Cyc_Absyn_Primop _tmp4AE;struct Cyc_List_List*
_tmp4AF;struct Cyc_List_List _tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_List_List*
_tmp4B2;struct Cyc_List_List _tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_List_List*
_tmp4B5;_LL21B: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4AB=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AA;if(_tmp4AB->tag != 18)goto _LL21D;else{_tmp4AC=(void*)_tmp4AB->f1;}}_LL21C:
elt_type=_tmp4AC;{void**_tmp94E;*t=(void**)((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((
_tmp94E[0]=elt_type,_tmp94E))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL21A;_LL21D: {struct Cyc_Absyn_Primop_e_struct*
_tmp4AD=(struct Cyc_Absyn_Primop_e_struct*)_tmp4AA;if(_tmp4AD->tag != 3)goto _LL21F;
else{_tmp4AE=_tmp4AD->f1;if(_tmp4AE != Cyc_Absyn_Times)goto _LL21F;_tmp4AF=_tmp4AD->f2;
if(_tmp4AF == 0)goto _LL21F;_tmp4B0=*_tmp4AF;_tmp4B1=(struct Cyc_Absyn_Exp*)_tmp4B0.hd;
_tmp4B2=_tmp4B0.tl;if(_tmp4B2 == 0)goto _LL21F;_tmp4B3=*_tmp4B2;_tmp4B4=(struct Cyc_Absyn_Exp*)
_tmp4B3.hd;_tmp4B5=_tmp4B3.tl;if(_tmp4B5 != 0)goto _LL21F;}}_LL21E:{struct _tuple0
_tmp94F;struct _tuple0 _tmp4B8=(_tmp94F.f1=_tmp4B1->r,((_tmp94F.f2=_tmp4B4->r,
_tmp94F)));void*_tmp4B9;void*_tmp4BB;void*_tmp4BC;void*_tmp4BE;_LL222: _tmp4B9=
_tmp4B8.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4BA=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4B9;if(_tmp4BA->tag != 18)goto _LL224;else{_tmp4BB=(void*)_tmp4BA->f1;}};
_LL223: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4BB);elt_type=_tmp4BB;{void**
_tmp950;*t=(void**)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=elt_type,
_tmp950))));}num_elts=_tmp4B4;one_elt=0;goto _LL221;_LL224: _tmp4BC=_tmp4B8.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4BD=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4BC;if(_tmp4BD->tag != 18)goto _LL226;else{_tmp4BE=(void*)_tmp4BD->f1;}};
_LL225: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4BE);elt_type=_tmp4BE;{void**
_tmp951;*t=(void**)((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951[0]=elt_type,
_tmp951))));}num_elts=_tmp4B1;one_elt=0;goto _LL221;_LL226:;_LL227: goto No_sizeof;
_LL221:;}goto _LL21A;_LL21F:;_LL220: No_sizeof: {struct Cyc_Absyn_Exp*_tmp4C1=*e;{
void*_tmp4C2=_tmp4C1->r;struct Cyc_Absyn_Exp*_tmp4C4;_LL229: {struct Cyc_Absyn_Cast_e_struct*
_tmp4C3=(struct Cyc_Absyn_Cast_e_struct*)_tmp4C2;if(_tmp4C3->tag != 15)goto _LL22B;
else{_tmp4C4=_tmp4C3->f2;}}_LL22A: _tmp4C1=_tmp4C4;goto _LL228;_LL22B:;_LL22C: goto
_LL228;_LL228:;}{void*_tmp4C5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C1->topt))->v);void*_tmp4C7;_LL22E: {struct Cyc_Absyn_TagType_struct*
_tmp4C6=(struct Cyc_Absyn_TagType_struct*)_tmp4C5;if(_tmp4C6->tag != 20)goto _LL230;
else{_tmp4C7=(void*)_tmp4C6->f1;}}_LL22F:{void*_tmp4C8=Cyc_Tcutil_compress(
_tmp4C7);struct Cyc_Absyn_Exp*_tmp4CA;_LL233: {struct Cyc_Absyn_ValueofType_struct*
_tmp4C9=(struct Cyc_Absyn_ValueofType_struct*)_tmp4C8;if(_tmp4C9->tag != 19)goto
_LL235;else{_tmp4CA=_tmp4C9->f1;}}_LL234: er=_tmp4CA->r;goto retry_sizeof;_LL235:;
_LL236: goto _LL232;_LL232:;}goto _LL22D;_LL230:;_LL231: goto _LL22D;_LL22D:;}
elt_type=Cyc_Absyn_char_typ;{void**_tmp952;*t=(void**)((_tmp952=_cycalloc(
sizeof(*_tmp952)),((_tmp952[0]=elt_type,_tmp952))));}num_elts=*e;one_elt=0;}goto
_LL21A;_LL21A:;}}*is_fat=!one_elt;{void*_tmp4CC=elt_type;struct Cyc_Absyn_AggrInfo
_tmp4CE;union Cyc_Absyn_AggrInfoU _tmp4CF;struct Cyc_Absyn_Aggrdecl**_tmp4D0;struct
Cyc_Absyn_Aggrdecl*_tmp4D1;_LL238: {struct Cyc_Absyn_AggrType_struct*_tmp4CD=(
struct Cyc_Absyn_AggrType_struct*)_tmp4CC;if(_tmp4CD->tag != 12)goto _LL23A;else{
_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CE.aggr_info;if((_tmp4CF.KnownAggr).tag != 2)
goto _LL23A;_tmp4D0=(struct Cyc_Absyn_Aggrdecl**)(_tmp4CF.KnownAggr).val;_tmp4D1=*
_tmp4D0;}}_LL239: if(_tmp4D1->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4D1->impl))->exist_vars != 0){const char*_tmp955;void*_tmp954;(
_tmp954=0,Cyc_Tcutil_terr(loc,((_tmp955="malloc with existential types not yet implemented",
_tag_dyneither(_tmp955,sizeof(char),50))),_tag_dyneither(_tmp954,sizeof(void*),0)));}
goto _LL237;_LL23A:;_LL23B: goto _LL237;_LL237:;}{void*(*_tmp4D4)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4D5=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp4D6=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4D8;struct Cyc_Absyn_PtrAtts
_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4DB;
union Cyc_Absyn_Constraint*_tmp4DC;_LL23D: {struct Cyc_Absyn_PointerType_struct*
_tmp4D7=(struct Cyc_Absyn_PointerType_struct*)_tmp4D6;if(_tmp4D7->tag != 5)goto
_LL23F;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D8.ptr_atts;_tmp4DA=_tmp4D9.nullable;
_tmp4DB=_tmp4D9.bounds;_tmp4DC=_tmp4D9.zero_term;}}_LL23E: _tmp4D5=_tmp4DC;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DA))
_tmp4D4=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp4DC) && !(*is_calloc)){{const char*_tmp958;void*_tmp957;(_tmp957=0,Cyc_Tcutil_warn(
loc,((_tmp958="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp958,sizeof(char),55))),_tag_dyneither(_tmp957,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp4DF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4DB);struct Cyc_Absyn_Exp*_tmp4E2;_LL242: {struct Cyc_Absyn_DynEither_b_struct*
_tmp4E0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp4DF;if(_tmp4E0->tag != 0)goto
_LL244;}_LL243: goto _LL241;_LL244:{struct Cyc_Absyn_Upper_b_struct*_tmp4E1=(struct
Cyc_Absyn_Upper_b_struct*)_tmp4DF;if(_tmp4E1->tag != 1)goto _LL246;else{_tmp4E2=
_tmp4E1->f1;}}if(!(!one_elt))goto _LL246;_LL245: {int _tmp4E3=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4E3  && Cyc_Evexp_same_const_exp(_tmp4E2,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DB),_tmp4D5);}{void*_tmp4E4=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4E6;_LL249: {struct Cyc_Absyn_TagType_struct*_tmp4E5=(struct Cyc_Absyn_TagType_struct*)
_tmp4E4;if(_tmp4E5->tag != 20)goto _LL24B;else{_tmp4E6=(void*)_tmp4E5->f1;}}_LL24A: {
struct Cyc_Absyn_Exp*_tmp4E7=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4E6,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4E7,
_tmp4E2)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DB),_tmp4D5);}
goto _LL248;}_LL24B:;_LL24C: goto _LL248;_LL248:;}goto _LL241;}_LL246:;_LL247: goto
_LL241;_LL241:;}goto _LL23C;_LL23F:;_LL240: goto _LL23C;_LL23C:;}if(!one_elt)
_tmp4D4=Cyc_Absyn_dyneither_typ;return _tmp4D4(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp4D5);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4E8=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4E9=Cyc_Tcenv_enter_lhs(
_tmp4E8,Cyc_Tcenv_enter_notreadctxt(_tmp4E8,te));Cyc_Tcexp_tcExp(_tmp4E9,0,e1);
Cyc_Tcexp_tcExp(_tmp4E9,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp4EA=Cyc_Tcutil_compress(t1);_LL24E: {struct Cyc_Absyn_ArrayType_struct*
_tmp4EB=(struct Cyc_Absyn_ArrayType_struct*)_tmp4EA;if(_tmp4EB->tag != 9)goto
_LL250;}_LL24F:{const char*_tmp95B;void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_terr(loc,((
_tmp95B="cannot assign to an array",_tag_dyneither(_tmp95B,sizeof(char),26))),
_tag_dyneither(_tmp95A,sizeof(void*),0)));}goto _LL24D;_LL250:;_LL251: goto _LL24D;
_LL24D:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){const char*
_tmp95E;void*_tmp95D;(_tmp95D=0,Cyc_Tcutil_terr(loc,((_tmp95E="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp95E,sizeof(char),58))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp961;void*_tmp960;return(_tmp960=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp961="swap non-lvalue",_tag_dyneither(_tmp961,sizeof(char),
16))),_tag_dyneither(_tmp960,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp964;void*_tmp963;return(_tmp963=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp964="swap non-lvalue",_tag_dyneither(_tmp964,sizeof(char),16))),
_tag_dyneither(_tmp963,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp967;void*_tmp966;return(_tmp966=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp967="swap value in zeroterm array",_tag_dyneither(_tmp967,
sizeof(char),29))),_tag_dyneither(_tmp966,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp96A;void*_tmp969;return(_tmp969=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp96A="swap value in zeroterm array",_tag_dyneither(_tmp96A,
sizeof(char),29))),_tag_dyneither(_tmp969,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp96F;void*_tmp96E[2];struct Cyc_String_pa_struct _tmp96D;struct Cyc_String_pa_struct
_tmp96C;void*_tmp4F8=(_tmp96C.tag=0,((_tmp96C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp96D.tag=0,((_tmp96D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp96E[0]=&
_tmp96D,((_tmp96E[1]=& _tmp96C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp96F="type mismatch: %s != %s",
_tag_dyneither(_tmp96F,sizeof(char),24))),_tag_dyneither(_tmp96E,sizeof(void*),2)))))))))))));
return _tmp4F8;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4FD=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4FD,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4FE=s->r;struct Cyc_Absyn_Exp*
_tmp500;struct Cyc_Absyn_Stmt*_tmp502;struct Cyc_Absyn_Stmt*_tmp503;struct Cyc_Absyn_Decl*
_tmp505;struct Cyc_Absyn_Stmt*_tmp506;_LL253: {struct Cyc_Absyn_Exp_s_struct*
_tmp4FF=(struct Cyc_Absyn_Exp_s_struct*)_tmp4FE;if(_tmp4FF->tag != 1)goto _LL255;
else{_tmp500=_tmp4FF->f1;}}_LL254: {void*_tmp507=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp500->topt))->v;if(!Cyc_Tcutil_unify(_tmp507,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp973;void*_tmp972[1];struct
Cyc_String_pa_struct _tmp971;(_tmp971.tag=0,((_tmp971.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp507)),((_tmp972[0]=& _tmp971,Cyc_Tcutil_terr(
loc,((_tmp973="statement expression returns type %s",_tag_dyneither(_tmp973,
sizeof(char),37))),_tag_dyneither(_tmp972,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp507;}_LL255: {struct Cyc_Absyn_Seq_s_struct*_tmp501=(struct Cyc_Absyn_Seq_s_struct*)
_tmp4FE;if(_tmp501->tag != 2)goto _LL257;else{_tmp502=_tmp501->f1;_tmp503=_tmp501->f2;}}
_LL256: s=_tmp503;continue;_LL257: {struct Cyc_Absyn_Decl_s_struct*_tmp504=(struct
Cyc_Absyn_Decl_s_struct*)_tmp4FE;if(_tmp504->tag != 12)goto _LL259;else{_tmp505=
_tmp504->f1;_tmp506=_tmp504->f2;}}_LL258: s=_tmp506;continue;_LL259:;_LL25A: {
const char*_tmp976;void*_tmp975;return(_tmp975=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp976="statement expression must end with expression",_tag_dyneither(_tmp976,
sizeof(char),46))),_tag_dyneither(_tmp975,sizeof(void*),0)));}_LL252:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp50D=t;struct Cyc_Absyn_AggrInfo _tmp50F;union
Cyc_Absyn_AggrInfoU _tmp510;struct Cyc_Absyn_Aggrdecl**_tmp511;struct Cyc_Absyn_Aggrdecl*
_tmp512;_LL25C: {struct Cyc_Absyn_AggrType_struct*_tmp50E=(struct Cyc_Absyn_AggrType_struct*)
_tmp50D;if(_tmp50E->tag != 12)goto _LL25E;else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50F.aggr_info;
if((_tmp510.KnownAggr).tag != 2)goto _LL25E;_tmp511=(struct Cyc_Absyn_Aggrdecl**)(
_tmp510.KnownAggr).val;_tmp512=*_tmp511;}}_LL25D: if((_tmp512->kind == Cyc_Absyn_UnionA
 && _tmp512->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp512->impl))->tagged)
goto _LL25B;goto _LL25F;_LL25E:;_LL25F:{const char*_tmp97A;void*_tmp979[1];struct
Cyc_String_pa_struct _tmp978;(_tmp978.tag=0,((_tmp978.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp979[0]=& _tmp978,Cyc_Tcutil_terr(
loc,((_tmp97A="expecting @tagged union but found %s",_tag_dyneither(_tmp97A,
sizeof(char),37))),_tag_dyneither(_tmp979,sizeof(void*),1)))))));}goto _LL25B;
_LL25B:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp97D;struct Cyc_Absyn_RgnHandleType_struct*
_tmp97C;void*expected_type=(void*)((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((
_tmp97C[0]=((_tmp97D.tag=16,((_tmp97D.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp97D)))),_tmp97C))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle));void*_tmp516=Cyc_Tcutil_compress(handle_type);void*
_tmp518;_LL261: {struct Cyc_Absyn_RgnHandleType_struct*_tmp517=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp516;if(_tmp517->tag != 16)goto _LL263;else{_tmp518=(void*)_tmp517->f1;}}_LL262:
rgn=_tmp518;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL260;_LL263:;_LL264:{
const char*_tmp981;void*_tmp980[1];struct Cyc_String_pa_struct _tmp97F;(_tmp97F.tag=
0,((_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp980[0]=& _tmp97F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle))->loc,((_tmp981="expecting region_t type but found %s",
_tag_dyneither(_tmp981,sizeof(char),37))),_tag_dyneither(_tmp980,sizeof(void*),1)))))));}
goto _LL260;_LL260:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp51E=e1->r;struct Cyc_Core_Opt*_tmp521;struct Cyc_List_List*_tmp522;struct
Cyc_List_List*_tmp524;union Cyc_Absyn_Cnst _tmp526;struct _dyneither_ptr _tmp527;
_LL266: {struct Cyc_Absyn_Comprehension_e_struct*_tmp51F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp51E;if(_tmp51F->tag != 28)goto _LL268;}_LL267: {void*_tmp528=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp529=Cyc_Tcutil_compress(_tmp528);struct Cyc_Absyn_ArrayInfo
_tmp52B;void*_tmp52C;struct Cyc_Absyn_Tqual _tmp52D;struct Cyc_Absyn_Exp*_tmp52E;
union Cyc_Absyn_Constraint*_tmp52F;_LL271: {struct Cyc_Absyn_ArrayType_struct*
_tmp52A=(struct Cyc_Absyn_ArrayType_struct*)_tmp529;if(_tmp52A->tag != 9)goto
_LL273;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52B.elt_type;_tmp52D=_tmp52B.tq;
_tmp52E=_tmp52B.num_elts;_tmp52F=_tmp52B.zero_term;}}_LL272: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp52E);void*b;{void*_tmp530=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp532;_LL276: {
struct Cyc_Absyn_TagType_struct*_tmp531=(struct Cyc_Absyn_TagType_struct*)_tmp530;
if(_tmp531->tag != 20)goto _LL278;else{_tmp532=(void*)_tmp531->f1;}}_LL277:{struct
Cyc_Absyn_Upper_b_struct _tmp984;struct Cyc_Absyn_Upper_b_struct*_tmp983;b=(void*)((
_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp984.tag=1,((_tmp984.f1=
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp532,0),0,Cyc_Absyn_No_coercion,
0),_tmp984)))),_tmp983))));}goto _LL275;_LL278:;_LL279: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp987;struct Cyc_Absyn_Upper_b_struct*
_tmp986;b=(void*)((_tmp986=_cycalloc(sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=
1,((_tmp987.f1=bnd,_tmp987)))),_tmp986))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}
_LL275:;}{struct Cyc_Absyn_PointerType_struct _tmp991;struct Cyc_Absyn_PtrAtts
_tmp990;struct Cyc_Absyn_PtrInfo _tmp98F;struct Cyc_Absyn_PointerType_struct*
_tmp98E;void*res_typ=(void*)((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((
_tmp991.tag=5,((_tmp991.f1=((_tmp98F.elt_typ=_tmp52C,((_tmp98F.elt_tq=_tmp52D,((
_tmp98F.ptr_atts=((_tmp990.rgn=rgn,((_tmp990.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp990.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp990.zero_term=_tmp52F,((_tmp990.ptrloc=0,
_tmp990)))))))))),_tmp98F)))))),_tmp991)))),_tmp98E))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp992;e->topt=((_tmp992=_cycalloc(sizeof(*_tmp992)),((_tmp992->v=res_typ,
_tmp992))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL273:;_LL274: {const char*_tmp995;void*_tmp994;(
_tmp994=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp995="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp995,
sizeof(char),45))),_tag_dyneither(_tmp994,sizeof(void*),0)));}_LL270:;}_LL268: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp520=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp51E;if(_tmp520->tag != 36)goto _LL26A;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;}}
_LL269:{struct Cyc_Absyn_Array_e_struct _tmp998;struct Cyc_Absyn_Array_e_struct*
_tmp997;e1->r=(void*)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=((
_tmp998.tag=27,((_tmp998.f1=_tmp522,_tmp998)))),_tmp997))));}_tmp524=_tmp522;
goto _LL26B;_LL26A: {struct Cyc_Absyn_Array_e_struct*_tmp523=(struct Cyc_Absyn_Array_e_struct*)
_tmp51E;if(_tmp523->tag != 27)goto _LL26C;else{_tmp524=_tmp523->f1;}}_LL26B: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp540=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp542;void*_tmp543;void**_tmp544;struct Cyc_Absyn_Tqual
_tmp545;struct Cyc_Absyn_PtrAtts _tmp546;union Cyc_Absyn_Constraint*_tmp547;_LL27B: {
struct Cyc_Absyn_PointerType_struct*_tmp541=(struct Cyc_Absyn_PointerType_struct*)
_tmp540;if(_tmp541->tag != 5)goto _LL27D;else{_tmp542=_tmp541->f1;_tmp543=_tmp542.elt_typ;
_tmp544=(void**)&(_tmp541->f1).elt_typ;_tmp545=_tmp542.elt_tq;_tmp546=_tmp542.ptr_atts;
_tmp547=_tmp546.zero_term;}}_LL27C: elt_typ_opt=(void**)_tmp544;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp547);goto _LL27A;
_LL27D:;_LL27E: goto _LL27A;_LL27A:;}{void*_tmp548=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp524);{struct Cyc_Core_Opt*_tmp999;e1->topt=((_tmp999=
_cycalloc(sizeof(*_tmp999)),((_tmp999->v=_tmp548,_tmp999))));}{void*res_typ;{
void*_tmp54A=Cyc_Tcutil_compress(_tmp548);struct Cyc_Absyn_ArrayInfo _tmp54C;void*
_tmp54D;struct Cyc_Absyn_Tqual _tmp54E;struct Cyc_Absyn_Exp*_tmp54F;union Cyc_Absyn_Constraint*
_tmp550;_LL280: {struct Cyc_Absyn_ArrayType_struct*_tmp54B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp54A;if(_tmp54B->tag != 9)goto _LL282;else{_tmp54C=_tmp54B->f1;_tmp54D=_tmp54C.elt_type;
_tmp54E=_tmp54C.tq;_tmp54F=_tmp54C.num_elts;_tmp550=_tmp54C.zero_term;}}_LL281:{
struct Cyc_Absyn_PointerType_struct _tmp9AE;struct Cyc_Absyn_PtrAtts _tmp9AD;struct
Cyc_Absyn_Upper_b_struct _tmp9AC;struct Cyc_Absyn_Upper_b_struct*_tmp9AB;struct Cyc_Absyn_PtrInfo
_tmp9AA;struct Cyc_Absyn_PointerType_struct*_tmp9A9;res_typ=(void*)((_tmp9A9=
_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9[0]=((_tmp9AE.tag=5,((_tmp9AE.f1=((_tmp9AA.elt_typ=
_tmp54D,((_tmp9AA.elt_tq=_tmp54E,((_tmp9AA.ptr_atts=((_tmp9AD.rgn=rgn,((_tmp9AD.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9AD.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9AB=
_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AC.tag=1,((_tmp9AC.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp54F),_tmp9AC)))),_tmp9AB))))),((_tmp9AD.zero_term=_tmp550,((
_tmp9AD.ptrloc=0,_tmp9AD)))))))))),_tmp9AA)))))),_tmp9AE)))),_tmp9A9))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9AF;e->topt=((_tmp9AF=_cycalloc(
sizeof(*_tmp9AF)),((_tmp9AF->v=res_typ,_tmp9AF))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL27F;_LL282:;_LL283: {
const char*_tmp9B2;void*_tmp9B1;(_tmp9B1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9B2="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp9B2,sizeof(char),50))),_tag_dyneither(_tmp9B1,sizeof(void*),0)));}
_LL27F:;}return res_typ;};};}_LL26C: {struct Cyc_Absyn_Const_e_struct*_tmp525=(
struct Cyc_Absyn_Const_e_struct*)_tmp51E;if(_tmp525->tag != 0)goto _LL26E;else{
_tmp526=_tmp525->f1;if((_tmp526.String_c).tag != 7)goto _LL26E;_tmp527=(struct
_dyneither_ptr)(_tmp526.String_c).val;}}_LL26D: {void*_tmp55A=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp55B=Cyc_Tcexp_tcExp(te,(void**)& _tmp55A,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9B5;struct Cyc_Absyn_Upper_b_struct*_tmp9B4;
return Cyc_Absyn_atb_typ(_tmp55B,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9B4=
_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B5.tag=1,((_tmp9B5.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9B5)))),_tmp9B4)))),Cyc_Absyn_false_conref);}_LL26E:;_LL26F: {void**
topt2=0;if(topt != 0){void*_tmp55E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp560;void*_tmp561;void**_tmp562;struct Cyc_Absyn_Tqual _tmp563;_LL285: {struct
Cyc_Absyn_PointerType_struct*_tmp55F=(struct Cyc_Absyn_PointerType_struct*)
_tmp55E;if(_tmp55F->tag != 5)goto _LL287;else{_tmp560=_tmp55F->f1;_tmp561=_tmp560.elt_typ;
_tmp562=(void**)&(_tmp55F->f1).elt_typ;_tmp563=_tmp560.elt_tq;}}_LL286: topt2=(
void**)_tmp562;goto _LL284;_LL287: {struct Cyc_Absyn_DatatypeType_struct*_tmp564=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp55E;if(_tmp564->tag != 3)goto _LL289;}
_LL288:{void**_tmp9B6;topt2=((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6[0]=*
topt,_tmp9B6))));}goto _LL284;_LL289:;_LL28A: goto _LL284;_LL284:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp9C0;struct Cyc_Absyn_PtrAtts
_tmp9BF;struct Cyc_Absyn_PtrInfo _tmp9BE;struct Cyc_Absyn_PointerType_struct*
_tmp9BD;void*res_typ=(void*)((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((
_tmp9C0.tag=5,((_tmp9C0.f1=((_tmp9BE.elt_typ=telt,((_tmp9BE.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp9BE.ptr_atts=((_tmp9BF.rgn=rgn,((_tmp9BF.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9BF.bounds=Cyc_Absyn_bounds_one_conref,((_tmp9BF.zero_term=
Cyc_Absyn_false_conref,((_tmp9BF.ptrloc=0,_tmp9BF)))))))))),_tmp9BE)))))),
_tmp9C0)))),_tmp9BD))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9C1;e->topt=((_tmp9C1=_cycalloc(
sizeof(*_tmp9C1)),((_tmp9C1->v=res_typ,_tmp9C1))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL265:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp56B=t;
struct Cyc_Absyn_ArrayInfo _tmp56D;void*_tmp56E;struct Cyc_Absyn_Tqual _tmp56F;
struct Cyc_Absyn_Exp*_tmp570;union Cyc_Absyn_Constraint*_tmp571;_LL28C: {struct Cyc_Absyn_ArrayType_struct*
_tmp56C=(struct Cyc_Absyn_ArrayType_struct*)_tmp56B;if(_tmp56C->tag != 9)goto
_LL28E;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56D.elt_type;_tmp56F=_tmp56D.tq;
_tmp570=_tmp56D.num_elts;_tmp571=_tmp56D.zero_term;}}_LL28D: {void*_tmp573;
struct _tuple13 _tmp572=Cyc_Tcutil_addressof_props(te,e);_tmp573=_tmp572.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp9C4;struct Cyc_Absyn_Upper_b_struct*_tmp9C3;
void*_tmp574=_tmp570 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9C3=
_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=1,((_tmp9C4.f1=(struct Cyc_Absyn_Exp*)
_tmp570,_tmp9C4)))),_tmp9C3))));t=Cyc_Absyn_atb_typ(_tmp56E,_tmp573,_tmp56F,
_tmp574,_tmp571);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL28E:;_LL28F: return t;_LL28B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp577=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp577,t) && !Cyc_Tcutil_is_noalias_path(_tmp577,e)){const char*_tmp9C7;void*
_tmp9C6;(_tmp9C6=0,Cyc_Tcutil_terr(e->loc,((_tmp9C7="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp9C7,sizeof(char),49))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}{
void*_tmp57A=e->r;union Cyc_Absyn_Cnst _tmp57E;struct _dyneither_ptr _tmp57F;_LL291: {
struct Cyc_Absyn_Array_e_struct*_tmp57B=(struct Cyc_Absyn_Array_e_struct*)_tmp57A;
if(_tmp57B->tag != 27)goto _LL293;}_LL292: goto _LL294;_LL293: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp57C=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp57A;if(_tmp57C->tag != 28)
goto _LL295;}_LL294: goto _LL296;_LL295: {struct Cyc_Absyn_Const_e_struct*_tmp57D=(
struct Cyc_Absyn_Const_e_struct*)_tmp57A;if(_tmp57D->tag != 0)goto _LL297;else{
_tmp57E=_tmp57D->f1;if((_tmp57E.String_c).tag != 7)goto _LL297;_tmp57F=(struct
_dyneither_ptr)(_tmp57E.String_c).val;}}_LL296: return t;_LL297:;_LL298: t=Cyc_Tcutil_compress(
t);{void*_tmp580=t;struct Cyc_Absyn_ArrayInfo _tmp582;void*_tmp583;struct Cyc_Absyn_Tqual
_tmp584;struct Cyc_Absyn_Exp*_tmp585;union Cyc_Absyn_Constraint*_tmp586;_LL29A: {
struct Cyc_Absyn_ArrayType_struct*_tmp581=(struct Cyc_Absyn_ArrayType_struct*)
_tmp580;if(_tmp581->tag != 9)goto _LL29C;else{_tmp582=_tmp581->f1;_tmp583=_tmp582.elt_type;
_tmp584=_tmp582.tq;_tmp585=_tmp582.num_elts;_tmp586=_tmp582.zero_term;}}_LL29B: {
void*_tmp588;struct _tuple13 _tmp587=Cyc_Tcutil_addressof_props(te,e);_tmp588=
_tmp587.f2;{struct Cyc_Absyn_Upper_b_struct _tmp9CA;struct Cyc_Absyn_Upper_b_struct*
_tmp9C9;void*b=_tmp585 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9C9=
_cycalloc(sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=1,((_tmp9CA.f1=(struct Cyc_Absyn_Exp*)
_tmp585,_tmp9CA)))),_tmp9C9))));t=Cyc_Absyn_atb_typ(_tmp583,_tmp588,_tmp584,b,
_tmp586);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL29C:;_LL29D: return t;_LL299:;};_LL290:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp58B=e->r;
struct Cyc_Absyn_Exp*_tmp58D;_LL29F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp58C=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp58B;if(_tmp58C->tag != 13)
goto _LL2A1;else{_tmp58D=_tmp58C->f1;}}_LL2A0: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp58D);((struct Cyc_Core_Opt*)_check_null(_tmp58D->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp58D->topt))->v,0);e->topt=_tmp58D->topt;
goto _LL29E;_LL2A1:;_LL2A2: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp58E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp590;void*_tmp591;struct Cyc_Absyn_Tqual
_tmp592;struct Cyc_Absyn_PtrAtts _tmp593;void*_tmp594;union Cyc_Absyn_Constraint*
_tmp595;union Cyc_Absyn_Constraint*_tmp596;union Cyc_Absyn_Constraint*_tmp597;
_LL2A4: {struct Cyc_Absyn_PointerType_struct*_tmp58F=(struct Cyc_Absyn_PointerType_struct*)
_tmp58E;if(_tmp58F->tag != 5)goto _LL2A6;else{_tmp590=_tmp58F->f1;_tmp591=_tmp590.elt_typ;
_tmp592=_tmp590.elt_tq;_tmp593=_tmp590.ptr_atts;_tmp594=_tmp593.rgn;_tmp595=
_tmp593.nullable;_tmp596=_tmp593.bounds;_tmp597=_tmp593.zero_term;}}_LL2A5:{void*
_tmp598=Cyc_Tcutil_compress(_tmp591);struct Cyc_Absyn_FnInfo _tmp59A;struct Cyc_List_List*
_tmp59B;struct Cyc_Core_Opt*_tmp59C;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59E;
struct Cyc_List_List*_tmp59F;int _tmp5A0;struct Cyc_Absyn_VarargInfo*_tmp5A1;struct
Cyc_List_List*_tmp5A2;struct Cyc_List_List*_tmp5A3;_LL2A9: {struct Cyc_Absyn_FnType_struct*
_tmp599=(struct Cyc_Absyn_FnType_struct*)_tmp598;if(_tmp599->tag != 10)goto _LL2AB;
else{_tmp59A=_tmp599->f1;_tmp59B=_tmp59A.tvars;_tmp59C=_tmp59A.effect;_tmp59D=
_tmp59A.ret_tqual;_tmp59E=_tmp59A.ret_typ;_tmp59F=_tmp59A.args;_tmp5A0=_tmp59A.c_varargs;
_tmp5A1=_tmp59A.cyc_varargs;_tmp5A2=_tmp59A.rgn_po;_tmp5A3=_tmp59A.attributes;}}
_LL2AA: if(_tmp59B != 0){struct _RegionHandle*_tmp5A4=Cyc_Tcenv_get_fnrgn(te);
struct _tuple11 _tmp9CB;struct _tuple11 _tmp5A5=(_tmp9CB.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp9CB.f2=_tmp5A4,_tmp9CB)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp5A4,Cyc_Tcutil_r_make_inst_var,&
_tmp5A5,_tmp59B);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp9D1;struct Cyc_Absyn_FnInfo
_tmp9D0;struct Cyc_Absyn_FnType_struct*_tmp9CF;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp5A4,inst,(void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D1.tag=
10,((_tmp9D1.f1=((_tmp9D0.tvars=0,((_tmp9D0.effect=_tmp59C,((_tmp9D0.ret_tqual=
_tmp59D,((_tmp9D0.ret_typ=_tmp59E,((_tmp9D0.args=_tmp59F,((_tmp9D0.c_varargs=
_tmp5A0,((_tmp9D0.cyc_varargs=_tmp5A1,((_tmp9D0.rgn_po=_tmp5A2,((_tmp9D0.attributes=
_tmp5A3,_tmp9D0)))))))))))))))))),_tmp9D1)))),_tmp9CF)))));struct Cyc_Absyn_PointerType_struct
_tmp9DB;struct Cyc_Absyn_PtrAtts _tmp9DA;struct Cyc_Absyn_PtrInfo _tmp9D9;struct Cyc_Absyn_PointerType_struct*
_tmp9D8;struct Cyc_Absyn_PointerType_struct*_tmp5A6=(_tmp9D8=_cycalloc(sizeof(*
_tmp9D8)),((_tmp9D8[0]=((_tmp9DB.tag=5,((_tmp9DB.f1=((_tmp9D9.elt_typ=ftyp,((
_tmp9D9.elt_tq=_tmp592,((_tmp9D9.ptr_atts=((_tmp9DA.rgn=_tmp594,((_tmp9DA.nullable=
_tmp595,((_tmp9DA.bounds=_tmp596,((_tmp9DA.zero_term=_tmp597,((_tmp9DA.ptrloc=0,
_tmp9DA)))))))))),_tmp9D9)))))),_tmp9DB)))),_tmp9D8)));struct Cyc_Absyn_Exp*
_tmp5A7=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp9DE;
struct Cyc_Absyn_Instantiate_e_struct*_tmp9DD;e->r=(void*)((_tmp9DD=_cycalloc(
sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DE.tag=14,((_tmp9DE.f1=_tmp5A7,((_tmp9DE.f2=
ts,_tmp9DE)))))),_tmp9DD))));}{struct Cyc_Core_Opt*_tmp9DF;e->topt=((_tmp9DF=
_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF->v=(void*)_tmp5A6,_tmp9DF))));};}goto
_LL2A8;_LL2AB:;_LL2AC: goto _LL2A8;_LL2A8:;}goto _LL2A3;_LL2A6:;_LL2A7: goto _LL2A3;
_LL2A3:;}goto _LL29E;_LL29E:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct
_RegionHandle*_tmp5B3=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp5B4=0;{
void*_tmp5B5=fn_exp->r;struct Cyc_List_List*_tmp5B7;_LL2AE: {struct Cyc_Absyn_FnCall_e_struct*
_tmp5B6=(struct Cyc_Absyn_FnCall_e_struct*)_tmp5B5;if(_tmp5B6->tag != 11)goto
_LL2B0;else{_tmp5B7=_tmp5B6->f2;}}_LL2AF:{void*_tmp5B8=e->r;struct Cyc_List_List*
_tmp5BA;_LL2B3: {struct Cyc_Absyn_FnCall_e_struct*_tmp5B9=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5B8;if(_tmp5B9->tag != 11)goto _LL2B5;else{_tmp5BA=_tmp5B9->f2;}}_LL2B4: {
struct Cyc_List_List*_tmp5BB=alias_arg_exps;int _tmp5BC=0;while(_tmp5BB != 0){
while(_tmp5BC != (int)_tmp5BB->hd){if(_tmp5B7 == 0){struct Cyc_Int_pa_struct _tmp9E9;
struct Cyc_Int_pa_struct _tmp9E8;void*_tmp9E7[2];const char*_tmp9E6;void*_tmp9E5;(
_tmp9E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmp9E9.tag=1,((_tmp9E9.f1=(unsigned long)((int)_tmp5BB->hd),((
_tmp9E8.tag=1,((_tmp9E8.f1=(unsigned long)_tmp5BC,((_tmp9E7[0]=& _tmp9E8,((
_tmp9E7[1]=& _tmp9E9,Cyc_aprintf(((_tmp9E6="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp9E6,sizeof(char),36))),_tag_dyneither(_tmp9E7,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp9E5,sizeof(void*),0)));}++ _tmp5BC;_tmp5B7=_tmp5B7->tl;_tmp5BA=((
struct Cyc_List_List*)_check_null(_tmp5BA))->tl;}{struct Cyc_Absyn_Decl*_tmp5C3;
struct Cyc_Absyn_Exp*_tmp5C4;struct _tuple10 _tmp5C2=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B7))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5BA))->hd)->topt))->v));_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;
_tmp5B7->hd=(void*)_tmp5C4;{struct Cyc_List_List*_tmp9EA;_tmp5B4=((_tmp9EA=
_region_malloc(_tmp5B3,sizeof(*_tmp9EA)),((_tmp9EA->hd=_tmp5C3,((_tmp9EA->tl=
_tmp5B4,_tmp9EA))))));}_tmp5BB=_tmp5BB->tl;};}goto _LL2B2;}_LL2B5:;_LL2B6: {const
char*_tmp9ED;void*_tmp9EC;(_tmp9EC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9ED="not a function call!",
_tag_dyneither(_tmp9ED,sizeof(char),21))),_tag_dyneither(_tmp9EC,sizeof(void*),0)));}
_LL2B2:;}goto _LL2AD;_LL2B0:;_LL2B1: {const char*_tmp9F0;void*_tmp9EF;(_tmp9EF=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9F0="not a function call!",_tag_dyneither(_tmp9F0,sizeof(char),21))),
_tag_dyneither(_tmp9EF,sizeof(void*),0)));}_LL2AD:;}while(_tmp5B4 != 0){struct Cyc_Absyn_Decl*
_tmp5CA=(struct Cyc_Absyn_Decl*)_tmp5B4->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp5CA,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp5B4=_tmp5B4->tl;}e->topt=
0;e->r=fn_exp->r;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*
_tmp5CB=e->r;struct Cyc_Absyn_Exp*_tmp5CD;struct _tuple1*_tmp5CF;struct Cyc_Absyn_Exp*
_tmp5D1;struct Cyc_List_List*_tmp5D2;struct Cyc_Core_Opt*_tmp5D4;struct Cyc_List_List*
_tmp5D5;union Cyc_Absyn_Cnst _tmp5D7;union Cyc_Absyn_Cnst*_tmp5D8;struct _tuple1*
_tmp5DA;void*_tmp5DB;enum Cyc_Absyn_Primop _tmp5DD;struct Cyc_List_List*_tmp5DE;
struct Cyc_Absyn_Exp*_tmp5E0;enum Cyc_Absyn_Incrementor _tmp5E1;struct Cyc_Absyn_Exp*
_tmp5E3;struct Cyc_Core_Opt*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Absyn_Exp*
_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Exp*
_tmp5EB;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5EE;struct Cyc_Absyn_Exp*
_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*
_tmp5F4;struct Cyc_List_List*_tmp5F5;struct Cyc_Absyn_VarargCallInfo*_tmp5F6;
struct Cyc_Absyn_VarargCallInfo**_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Exp*
_tmp5FB;struct Cyc_List_List*_tmp5FC;void*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FF;
enum Cyc_Absyn_Coercion _tmp600;enum Cyc_Absyn_Coercion*_tmp601;struct Cyc_Absyn_Exp*
_tmp603;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Exp**_tmp606;struct Cyc_Absyn_Exp*
_tmp607;struct Cyc_Absyn_Exp*_tmp609;void*_tmp60B;void*_tmp60D;void*_tmp60E;
struct Cyc_Absyn_Exp*_tmp610;struct Cyc_Absyn_Exp*_tmp612;struct _dyneither_ptr*
_tmp613;int _tmp614;int*_tmp615;int _tmp616;int*_tmp617;struct Cyc_Absyn_Exp*
_tmp619;struct _dyneither_ptr*_tmp61A;int _tmp61B;int*_tmp61C;int _tmp61D;int*
_tmp61E;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_List_List*
_tmp623;struct _tuple8*_tmp625;struct Cyc_List_List*_tmp626;struct Cyc_List_List*
_tmp628;struct Cyc_Absyn_Stmt*_tmp62A;struct Cyc_Absyn_Vardecl*_tmp62C;struct Cyc_Absyn_Exp*
_tmp62D;struct Cyc_Absyn_Exp*_tmp62E;int _tmp62F;int*_tmp630;struct _tuple1*_tmp632;
struct _tuple1**_tmp633;struct Cyc_List_List*_tmp634;struct Cyc_List_List**_tmp635;
struct Cyc_List_List*_tmp636;struct Cyc_Absyn_Aggrdecl*_tmp637;struct Cyc_Absyn_Aggrdecl**
_tmp638;void*_tmp63A;struct Cyc_List_List*_tmp63B;struct Cyc_List_List*_tmp63D;
struct Cyc_Absyn_Datatypedecl*_tmp63E;struct Cyc_Absyn_Datatypefield*_tmp63F;
struct _tuple1*_tmp641;struct _tuple1**_tmp642;struct Cyc_Absyn_Enumdecl*_tmp643;
struct Cyc_Absyn_Enumfield*_tmp644;struct _tuple1*_tmp646;struct _tuple1**_tmp647;
void*_tmp648;struct Cyc_Absyn_Enumfield*_tmp649;struct Cyc_Absyn_MallocInfo _tmp64B;
int _tmp64C;int*_tmp64D;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Exp**_tmp64F;
void**_tmp650;void***_tmp651;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Exp**
_tmp653;int _tmp654;int*_tmp655;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*
_tmp658;struct Cyc_Absyn_Exp*_tmp65A;struct _dyneither_ptr*_tmp65B;void*_tmp65D;
_LL2B8: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp5CC=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp5CB;if(_tmp5CC->tag != 13)goto _LL2BA;else{_tmp5CD=_tmp5CC->f1;}}_LL2B9: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp5CD);return;_LL2BA: {struct Cyc_Absyn_UnknownId_e_struct*_tmp5CE=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp5CB;if(_tmp5CE->tag != 2)goto _LL2BC;else{
_tmp5CF=_tmp5CE->f1;}}_LL2BB: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp5CF);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2BC: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp5D0=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp5CB;if(_tmp5D0->tag != 10)goto _LL2BE;else{
_tmp5D1=_tmp5D0->f1;_tmp5D2=_tmp5D0->f2;}}_LL2BD: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp5D1,_tmp5D2);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2BE: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5D3=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp5CB;if(_tmp5D3->tag != 36)
goto _LL2C0;else{_tmp5D4=_tmp5D3->f1;_tmp5D5=_tmp5D3->f2;}}_LL2BF: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp5D5);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C0: {struct
Cyc_Absyn_Const_e_struct*_tmp5D6=(struct Cyc_Absyn_Const_e_struct*)_tmp5CB;if(
_tmp5D6->tag != 0)goto _LL2C2;else{_tmp5D7=_tmp5D6->f1;_tmp5D8=(union Cyc_Absyn_Cnst*)&
_tmp5D6->f1;}}_LL2C1: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp5D8,e);goto _LL2B7;_LL2C2: {struct Cyc_Absyn_Var_e_struct*_tmp5D9=(struct Cyc_Absyn_Var_e_struct*)
_tmp5CB;if(_tmp5D9->tag != 1)goto _LL2C4;else{_tmp5DA=_tmp5D9->f1;_tmp5DB=(void*)
_tmp5D9->f2;}}_LL2C3: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp5DA,_tmp5DB);goto _LL2B7;
_LL2C4: {struct Cyc_Absyn_Primop_e_struct*_tmp5DC=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5CB;if(_tmp5DC->tag != 3)goto _LL2C6;else{_tmp5DD=_tmp5DC->f1;_tmp5DE=_tmp5DC->f2;}}
_LL2C5: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5DD,_tmp5DE);goto _LL2B7;_LL2C6: {
struct Cyc_Absyn_Increment_e_struct*_tmp5DF=(struct Cyc_Absyn_Increment_e_struct*)
_tmp5CB;if(_tmp5DF->tag != 5)goto _LL2C8;else{_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5DF->f2;}}
_LL2C7: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5E0,_tmp5E1);goto _LL2B7;_LL2C8: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5E2=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5CB;if(_tmp5E2->tag != 4)goto _LL2CA;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E2->f2;
_tmp5E5=_tmp5E2->f3;}}_LL2C9: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5E3,_tmp5E4,
_tmp5E5);goto _LL2B7;_LL2CA: {struct Cyc_Absyn_Conditional_e_struct*_tmp5E6=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp5CB;if(_tmp5E6->tag != 6)goto _LL2CC;
else{_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;_tmp5E9=_tmp5E6->f3;}}_LL2CB: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5E7,_tmp5E8,_tmp5E9);goto _LL2B7;_LL2CC: {struct Cyc_Absyn_And_e_struct*
_tmp5EA=(struct Cyc_Absyn_And_e_struct*)_tmp5CB;if(_tmp5EA->tag != 7)goto _LL2CE;
else{_tmp5EB=_tmp5EA->f1;_tmp5EC=_tmp5EA->f2;}}_LL2CD: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5EB,_tmp5EC);goto _LL2B7;_LL2CE: {struct Cyc_Absyn_Or_e_struct*_tmp5ED=(struct
Cyc_Absyn_Or_e_struct*)_tmp5CB;if(_tmp5ED->tag != 8)goto _LL2D0;else{_tmp5EE=
_tmp5ED->f1;_tmp5EF=_tmp5ED->f2;}}_LL2CF: t=Cyc_Tcexp_tcOr(te,loc,_tmp5EE,_tmp5EF);
goto _LL2B7;_LL2D0: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5F0=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp5CB;if(_tmp5F0->tag != 9)goto _LL2D2;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;}}
_LL2D1: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5F1,_tmp5F2);goto _LL2B7;_LL2D2: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5F3=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5CB;if(_tmp5F3->tag != 11)goto _LL2D4;else{_tmp5F4=_tmp5F3->f1;_tmp5F5=_tmp5F3->f2;
_tmp5F6=_tmp5F3->f3;_tmp5F7=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5F3->f3;}}
_LL2D3: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp65F;_push_handler(& _tmp65F);{int _tmp661=0;if(setjmp(
_tmp65F.handler))_tmp661=1;if(!_tmp661){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp660=(void*)_exn_thrown;void*_tmp663=_tmp660;struct
_dyneither_ptr _tmp665;_LL30B: {struct Cyc_Core_Failure_struct*_tmp664=(struct Cyc_Core_Failure_struct*)
_tmp663;if(_tmp664->tag != Cyc_Core_Failure)goto _LL30D;else{_tmp665=_tmp664->f1;}}
_LL30C: ok=0;fn_exp=e;goto _LL30A;_LL30D:;_LL30E:(void)_throw(_tmp663);_LL30A:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5F4,_tmp5F5,_tmp5F7,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2B7;}_LL2D4: {
struct Cyc_Absyn_Throw_e_struct*_tmp5F8=(struct Cyc_Absyn_Throw_e_struct*)_tmp5CB;
if(_tmp5F8->tag != 12)goto _LL2D6;else{_tmp5F9=_tmp5F8->f1;}}_LL2D5: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp5F9);goto _LL2B7;_LL2D6: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5FA=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp5CB;if(_tmp5FA->tag != 14)goto
_LL2D8;else{_tmp5FB=_tmp5FA->f1;_tmp5FC=_tmp5FA->f2;}}_LL2D7: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5FB,_tmp5FC);goto _LL2B7;_LL2D8: {struct Cyc_Absyn_Cast_e_struct*
_tmp5FD=(struct Cyc_Absyn_Cast_e_struct*)_tmp5CB;if(_tmp5FD->tag != 15)goto _LL2DA;
else{_tmp5FE=(void*)_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;_tmp600=_tmp5FD->f4;_tmp601=(
enum Cyc_Absyn_Coercion*)& _tmp5FD->f4;}}_LL2D9: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5FE,_tmp5FF,(enum Cyc_Absyn_Coercion*)_tmp601);goto _LL2B7;_LL2DA: {struct Cyc_Absyn_Address_e_struct*
_tmp602=(struct Cyc_Absyn_Address_e_struct*)_tmp5CB;if(_tmp602->tag != 16)goto
_LL2DC;else{_tmp603=_tmp602->f1;}}_LL2DB: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp603);goto _LL2B7;_LL2DC: {struct Cyc_Absyn_New_e_struct*_tmp604=(struct Cyc_Absyn_New_e_struct*)
_tmp5CB;if(_tmp604->tag != 17)goto _LL2DE;else{_tmp605=_tmp604->f1;_tmp606=(struct
Cyc_Absyn_Exp**)& _tmp604->f1;_tmp607=_tmp604->f2;}}_LL2DD: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp606,e,_tmp607);goto _LL2B7;_LL2DE: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp608=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5CB;if(_tmp608->tag != 19)goto
_LL2E0;else{_tmp609=_tmp608->f1;}}_LL2DF: {void*_tmp666=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp609);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp666);goto _LL2B7;}_LL2E0: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp60A=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5CB;if(_tmp60A->tag != 18)goto _LL2E2;else{_tmp60B=(void*)_tmp60A->f1;}}_LL2E1:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp60B);goto _LL2B7;_LL2E2: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp60C=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp5CB;if(_tmp60C->tag != 20)goto
_LL2E4;else{_tmp60D=(void*)_tmp60C->f1;_tmp60E=(void*)_tmp60C->f2;}}_LL2E3: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp60D,_tmp60E);goto _LL2B7;_LL2E4: {struct Cyc_Absyn_Deref_e_struct*
_tmp60F=(struct Cyc_Absyn_Deref_e_struct*)_tmp5CB;if(_tmp60F->tag != 21)goto _LL2E6;
else{_tmp610=_tmp60F->f1;}}_LL2E5: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp610);goto
_LL2B7;_LL2E6: {struct Cyc_Absyn_AggrMember_e_struct*_tmp611=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp5CB;if(_tmp611->tag != 22)goto _LL2E8;else{_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;
_tmp614=_tmp611->f3;_tmp615=(int*)& _tmp611->f3;_tmp616=_tmp611->f4;_tmp617=(int*)&
_tmp611->f4;}}_LL2E7: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp612,_tmp613,
_tmp615,_tmp617);goto _LL2B7;_LL2E8: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp618=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp5CB;if(_tmp618->tag != 23)goto _LL2EA;
else{_tmp619=_tmp618->f1;_tmp61A=_tmp618->f2;_tmp61B=_tmp618->f3;_tmp61C=(int*)&
_tmp618->f3;_tmp61D=_tmp618->f4;_tmp61E=(int*)& _tmp618->f4;}}_LL2E9: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp619,_tmp61A,_tmp61C,_tmp61E);goto _LL2B7;_LL2EA: {struct Cyc_Absyn_Subscript_e_struct*
_tmp61F=(struct Cyc_Absyn_Subscript_e_struct*)_tmp5CB;if(_tmp61F->tag != 24)goto
_LL2EC;else{_tmp620=_tmp61F->f1;_tmp621=_tmp61F->f2;}}_LL2EB: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp620,_tmp621);goto _LL2B7;_LL2EC: {struct Cyc_Absyn_Tuple_e_struct*
_tmp622=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5CB;if(_tmp622->tag != 25)goto _LL2EE;
else{_tmp623=_tmp622->f1;}}_LL2ED: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp623);goto
_LL2B7;_LL2EE: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp624=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp5CB;if(_tmp624->tag != 26)goto _LL2F0;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;}}
_LL2EF: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp625,_tmp626);goto _LL2B7;_LL2F0: {
struct Cyc_Absyn_Array_e_struct*_tmp627=(struct Cyc_Absyn_Array_e_struct*)_tmp5CB;
if(_tmp627->tag != 27)goto _LL2F2;else{_tmp628=_tmp627->f1;}}_LL2F1: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp667=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp669;void*_tmp66A;void**_tmp66B;union Cyc_Absyn_Constraint*
_tmp66C;_LL310: {struct Cyc_Absyn_ArrayType_struct*_tmp668=(struct Cyc_Absyn_ArrayType_struct*)
_tmp667;if(_tmp668->tag != 9)goto _LL312;else{_tmp669=_tmp668->f1;_tmp66A=_tmp669.elt_type;
_tmp66B=(void**)&(_tmp668->f1).elt_type;_tmp66C=_tmp669.zero_term;}}_LL311:
elt_topt=(void**)_tmp66B;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp66C);goto _LL30F;_LL312:;_LL313: goto _LL30F;_LL30F:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp628);goto _LL2B7;}_LL2F2: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp629=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp5CB;
if(_tmp629->tag != 37)goto _LL2F4;else{_tmp62A=_tmp629->f1;}}_LL2F3: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp62A);goto _LL2B7;_LL2F4: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp62B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp5CB;if(_tmp62B->tag != 28)
goto _LL2F6;else{_tmp62C=_tmp62B->f1;_tmp62D=_tmp62B->f2;_tmp62E=_tmp62B->f3;
_tmp62F=_tmp62B->f4;_tmp630=(int*)& _tmp62B->f4;}}_LL2F5: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp62C,_tmp62D,_tmp62E,_tmp630);goto _LL2B7;_LL2F6: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp631=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5CB;if(_tmp631->tag != 29)goto
_LL2F8;else{_tmp632=_tmp631->f1;_tmp633=(struct _tuple1**)& _tmp631->f1;_tmp634=
_tmp631->f2;_tmp635=(struct Cyc_List_List**)& _tmp631->f2;_tmp636=_tmp631->f3;
_tmp637=_tmp631->f4;_tmp638=(struct Cyc_Absyn_Aggrdecl**)& _tmp631->f4;}}_LL2F7: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp633,_tmp635,_tmp636,_tmp638);goto _LL2B7;
_LL2F8: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp639=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp5CB;if(_tmp639->tag != 30)goto _LL2FA;else{_tmp63A=(void*)_tmp639->f1;_tmp63B=
_tmp639->f2;}}_LL2F9: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp63A,_tmp63B);goto _LL2B7;
_LL2FA: {struct Cyc_Absyn_Datatype_e_struct*_tmp63C=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp5CB;if(_tmp63C->tag != 31)goto _LL2FC;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;
_tmp63F=_tmp63C->f3;}}_LL2FB: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp63D,
_tmp63E,_tmp63F);goto _LL2B7;_LL2FC: {struct Cyc_Absyn_Enum_e_struct*_tmp640=(
struct Cyc_Absyn_Enum_e_struct*)_tmp5CB;if(_tmp640->tag != 32)goto _LL2FE;else{
_tmp641=_tmp640->f1;_tmp642=(struct _tuple1**)& _tmp640->f1;_tmp643=_tmp640->f2;
_tmp644=_tmp640->f3;}}_LL2FD:*_tmp642=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp644))->name;{struct Cyc_Absyn_EnumType_struct _tmp9F3;struct Cyc_Absyn_EnumType_struct*
_tmp9F2;t=(void*)((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=
14,((_tmp9F3.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp643))->name,((
_tmp9F3.f2=_tmp643,_tmp9F3)))))),_tmp9F2))));}goto _LL2B7;_LL2FE: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp645=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp5CB;if(_tmp645->tag != 33)goto
_LL300;else{_tmp646=_tmp645->f1;_tmp647=(struct _tuple1**)& _tmp645->f1;_tmp648=(
void*)_tmp645->f2;_tmp649=_tmp645->f3;}}_LL2FF:*_tmp647=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp649))->name;t=_tmp648;goto _LL2B7;_LL300: {struct Cyc_Absyn_Malloc_e_struct*
_tmp64A=(struct Cyc_Absyn_Malloc_e_struct*)_tmp5CB;if(_tmp64A->tag != 34)goto
_LL302;else{_tmp64B=_tmp64A->f1;_tmp64C=_tmp64B.is_calloc;_tmp64D=(int*)&(
_tmp64A->f1).is_calloc;_tmp64E=_tmp64B.rgn;_tmp64F=(struct Cyc_Absyn_Exp**)&(
_tmp64A->f1).rgn;_tmp650=_tmp64B.elt_type;_tmp651=(void***)&(_tmp64A->f1).elt_type;
_tmp652=_tmp64B.num_elts;_tmp653=(struct Cyc_Absyn_Exp**)&(_tmp64A->f1).num_elts;
_tmp654=_tmp64B.fat_result;_tmp655=(int*)&(_tmp64A->f1).fat_result;}}_LL301: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp64F,_tmp651,_tmp653,_tmp64D,_tmp655);goto
_LL2B7;_LL302: {struct Cyc_Absyn_Swap_e_struct*_tmp656=(struct Cyc_Absyn_Swap_e_struct*)
_tmp5CB;if(_tmp656->tag != 35)goto _LL304;else{_tmp657=_tmp656->f1;_tmp658=_tmp656->f2;}}
_LL303: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp657,_tmp658);goto _LL2B7;_LL304: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp659=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp5CB;if(_tmp659->tag != 38)goto _LL306;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;}}
_LL305: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp65A,_tmp65B);goto _LL2B7;_LL306: {
struct Cyc_Absyn_Valueof_e_struct*_tmp65C=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp5CB;if(_tmp65C->tag != 39)goto _LL308;else{_tmp65D=(void*)_tmp65C->f1;}}_LL307:
if(!te->allow_valueof){const char*_tmp9F6;void*_tmp9F5;(_tmp9F5=0,Cyc_Tcutil_terr(
e->loc,((_tmp9F6="valueof(-) can only occur within types",_tag_dyneither(_tmp9F6,
sizeof(char),39))),_tag_dyneither(_tmp9F5,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2B7;_LL308: {struct Cyc_Absyn_Asm_e_struct*_tmp65E=(struct Cyc_Absyn_Asm_e_struct*)
_tmp5CB;if(_tmp65E->tag != 40)goto _LL2B7;}_LL309:{const char*_tmp9F9;void*_tmp9F8;(
_tmp9F8=0,Cyc_Tcutil_terr(e->loc,((_tmp9F9="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmp9F9,sizeof(char),50))),_tag_dyneither(_tmp9F8,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2B7:;}{struct Cyc_Core_Opt*
_tmp9FA;e->topt=((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA->v=t,_tmp9FA))));};}
