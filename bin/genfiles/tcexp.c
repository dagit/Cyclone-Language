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
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;struct _dyneither_ptr Cyc_vrprintf(
struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
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
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,
int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr
line;int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char
Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};typedef
unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;
struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct
_dyneither_ptr*Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple1*
Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*
Cyc_Absyn_typedef_name_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo
Cyc_Absyn_vararg_info_t;typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef
struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo
Cyc_Absyn_datatype_field_info_t;typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;
typedef void*Cyc_Absyn_rgntype_t;typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;typedef enum Cyc_Absyn_Incrementor
Cyc_Absyn_incrementor_t;typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;typedef void*Cyc_Absyn_raw_stmt_t;
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
print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,
Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind
 = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind
 = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;
enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned
 = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA
 = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};typedef union Cyc_Absyn_Constraint*
Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_struct{int tag;struct Cyc_Absyn_Kind*f1;};
struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;
union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};
struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};typedef void*Cyc_Absyn_pointer_qual_t;typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;
struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*
type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct Cyc_Core_Opt*
effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct
_tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
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
zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct
Cyc_Absyn_Datatype_td_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef
void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{
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
int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_struct{
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
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};
struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*
f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;
};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;unsigned int
f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;struct
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
topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{int tag;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
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
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;
struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{
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
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct _dyneither_ptr*f1;};extern
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};struct
Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
unsigned int);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;
extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slonglong_typ;void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,unsigned int ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum 
Cyc_Absyn_Coercion,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef struct Cyc_Dict_Dict
Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*
tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int
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
int tag;};struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};typedef
void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict
ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);struct
Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,
struct _tuple1*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*
f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
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
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*
k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_region(void*r,
int must_be_unique);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};struct _tuple14 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn);void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct
_tuple15{unsigned int f1;int f2;};struct _tuple15 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{
struct _tuple0*eq;int isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct
_union_RelnOp_LessEqConst LessEqConst;};typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;typedef struct Cyc_List_List*
Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;typedef struct _dyneither_ptr
Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int
fieldnum;};typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;
struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;
struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_struct{
int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple17
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};
struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,
struct _dyneither_ptr ap){{void*_tmp6BB;(_tmp6BB=0,Cyc_Tcutil_terr(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6BB,
sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons _tmp1;_push_handler(&
_tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))_tmp3=1;if(!_tmp3){{struct
_RegionHandle _tmp4=_new_region("r");struct _RegionHandle*r=& _tmp4;_push_region(r);{
void*_tmp5=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct
_tmp6BE;struct Cyc_Absyn_Var_e_struct*_tmp6BD;e->r=(void*)((_tmp6BD=_cycalloc(
sizeof(*_tmp6BD)),((_tmp6BD[0]=((_tmp6BE.tag=1,((_tmp6BE.f1=q,((_tmp6BE.f2=(void*)
_tmp7,_tmp6BE)))))),_tmp6BD))));}goto _LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*
_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)_tmp5;if(_tmp8->tag != 3)goto _LL5;else{
_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{struct Cyc_Absyn_Enum_e_struct _tmp6C1;
struct Cyc_Absyn_Enum_e_struct*_tmp6C0;e->r=(void*)((_tmp6C0=_cycalloc(sizeof(*
_tmp6C0)),((_tmp6C0[0]=((_tmp6C1.tag=32,((_tmp6C1.f1=q,((_tmp6C1.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp9,((_tmp6C1.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp6C1)))))))),_tmp6C0))));}
goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6:{struct Cyc_Absyn_AnonEnum_e_struct _tmp6C4;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp6C3;e->r=(void*)((_tmp6C3=_cycalloc(sizeof(*_tmp6C3)),((_tmp6C3[0]=((_tmp6C4.tag=
33,((_tmp6C4.f1=q,((_tmp6C4.f2=(void*)_tmpC,((_tmp6C4.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp6C4)))))))),_tmp6C3))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp6C7;struct Cyc_Absyn_Datatype_e_struct*_tmp6C6;e->r=(void*)((_tmp6C6=
_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6[0]=((_tmp6C7.tag=31,((_tmp6C7.f1=0,((
_tmp6C7.f2=_tmpF,((_tmp6C7.f3=_tmp10,_tmp6C7)))))))),_tmp6C6))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp6CB;void*_tmp6CA[1];struct Cyc_String_pa_struct
_tmp6C9;(_tmp6C9.tag=0,((_tmp6C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp6CA[0]=& _tmp6C9,Cyc_Tcutil_terr(e->loc,((
_tmp6CB="bad occurrence of type name %s",_tag_dyneither(_tmp6CB,sizeof(char),31))),
_tag_dyneither(_tmp6CA,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp6CE;struct Cyc_Absyn_Var_e_struct*_tmp6CD;e->r=(void*)((_tmp6CD=_cycalloc(
sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CE.tag=1,((_tmp6CE.f1=q,((_tmp6CE.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp6CE)))))),_tmp6CD))));}goto _LL0;_LL0:;};
_pop_region(r);};_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=
_tmp2;_LLC: {struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)
_tmp20;if(_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp6D1;struct Cyc_Absyn_Var_e_struct*_tmp6D0;e->r=(void*)((_tmp6D0=_cycalloc(
sizeof(*_tmp6D0)),((_tmp6D0[0]=((_tmp6D1.tag=1,((_tmp6D1.f1=q,((_tmp6D1.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp6D1)))))),_tmp6D0))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple18{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple18*_tmp6D2;return(_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2->f1=
0,((_tmp6D2->f2=e,_tmp6D2)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle
_tmp2B=_new_region("r");struct _RegionHandle*r=& _tmp2B;_push_region(r);{void*
_tmp2C=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*
_tmp30;struct Cyc_Absyn_Datatypefield*_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;
_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)
_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{_tmp2E=(void*)_tmp2D->f1;}}_LL17:{
struct Cyc_Absyn_FnCall_e_struct _tmp6D5;struct Cyc_Absyn_FnCall_e_struct*_tmp6D4;e->r=(
void*)((_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4[0]=((_tmp6D5.tag=11,((
_tmp6D5.f1=e1,((_tmp6D5.f2=es,((_tmp6D5.f3=0,_tmp6D5)))))))),_tmp6D4))));}
_npop_handler(1);return;_LL18: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;if(_tmp2F->tag != 2)goto _LL1A;else{
_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19: if(_tmp31->typs == 0){const char*
_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_struct _tmp6D7;(_tmp6D7.tag=0,((
_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp31->name)),((_tmp6D8[0]=& _tmp6D7,Cyc_Tcutil_terr(e->loc,((_tmp6D9="%s is a constant, not a function",
_tag_dyneither(_tmp6D9,sizeof(char),33))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Datatype_e_struct _tmp6DC;struct Cyc_Absyn_Datatype_e_struct*
_tmp6DB;e->r=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=
31,((_tmp6DC.f1=es,((_tmp6DC.f2=_tmp30,((_tmp6DC.f3=_tmp31,_tmp6DC)))))))),
_tmp6DB))));}_npop_handler(1);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*
_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)_tmp2C;if(_tmp32->tag != 1)goto _LL1C;
else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct _tmp6DF;struct
Cyc_Absyn_Aggregate_e_struct*_tmp6DE;e->r=(void*)((_tmp6DE=_cycalloc(sizeof(*
_tmp6DE)),((_tmp6DE[0]=((_tmp6DF.tag=29,((_tmp6DF.f1=_tmp33->name,((_tmp6DF.f2=0,((
_tmp6DF.f3=_tmp3D,((_tmp6DF.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,_tmp6DF)))))))))),
_tmp6DE))));}_npop_handler(1);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp6E3;void*_tmp6E2[1];
struct Cyc_String_pa_struct _tmp6E1;(_tmp6E1.tag=0,((_tmp6E1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp6E2[
0]=& _tmp6E1,Cyc_Tcutil_terr(e->loc,((_tmp6E3="%s is an enum constructor, not a function",
_tag_dyneither(_tmp6E3,sizeof(char),42))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));}
_npop_handler(1);return;_LL15:;};_pop_region(r);};_pop_handler();}else{void*
_tmp29=(void*)_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*
_tmp45=(struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)
goto _LL23;}_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp6E6;struct Cyc_Absyn_FnCall_e_struct*
_tmp6E5;e->r=(void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((_tmp6E5[0]=((_tmp6E6.tag=
11,((_tmp6E6.f1=e1,((_tmp6E6.f2=es,((_tmp6E6.f3=0,_tmp6E6)))))))),_tmp6E5))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp6E9;struct Cyc_Absyn_FnCall_e_struct*_tmp6E8;e->r=(void*)((_tmp6E8=_cycalloc(
sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=11,((_tmp6E9.f1=e1,((_tmp6E9.f2=es,((
_tmp6E9.f3=0,_tmp6E9)))))))),_tmp6E8))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp6EC;struct Cyc_Absyn_Array_e_struct*
_tmp6EB;e->r=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=
27,((_tmp6EC.f1=des,_tmp6EC)))),_tmp6EB))));}return;}{void*t=*topt;void*_tmp4C=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp4E;union Cyc_Absyn_AggrInfoU
_tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*_tmp52;struct Cyc_Absyn_Tqual _tmp53;
_LL26: {struct Cyc_Absyn_AggrType_struct*_tmp4D=(struct Cyc_Absyn_AggrType_struct*)
_tmp4C;if(_tmp4D->tag != 11)goto _LL28;else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}
_LL27:{union Cyc_Absyn_AggrInfoU _tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**
_tmp57;struct Cyc_Absyn_Aggrdecl*_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)
goto _LL31;_tmp56=(struct _tuple3)(_tmp55.UnknownAggr).val;_LL30: {const char*
_tmp6EF;void*_tmp6EE;(_tmp6EE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6EF="struct type not properly set",
_tag_dyneither(_tmp6EF,sizeof(char),29))),_tag_dyneither(_tmp6EE,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp6F2;struct Cyc_Absyn_Aggregate_e_struct*_tmp6F1;e->r=(void*)((_tmp6F1=
_cycalloc(sizeof(*_tmp6F1)),((_tmp6F1[0]=((_tmp6F2.tag=29,((_tmp6F2.f1=_tmp58->name,((
_tmp6F2.f2=0,((_tmp6F2.f3=des,((_tmp6F2.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp6F2)))))))))),_tmp6F1))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 8)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp6F5;struct Cyc_Absyn_Array_e_struct*_tmp6F4;e->r=(
void*)((_tmp6F4=_cycalloc(sizeof(*_tmp6F4)),((_tmp6F4[0]=((_tmp6F5.tag=27,((
_tmp6F5.f1=des,_tmp6F5)))),_tmp6F4))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 12)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp6F8;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp6F7;e->r=(void*)((_tmp6F7=_cycalloc(sizeof(*_tmp6F7)),((_tmp6F7[0]=((_tmp6F8.tag=
30,((_tmp6F8.f1=(void*)t,((_tmp6F8.f2=des,_tmp6F8)))))),_tmp6F7))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp6FB;struct Cyc_Absyn_Array_e_struct*
_tmp6FA;e->r=(void*)((_tmp6FA=_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA[0]=((_tmp6FB.tag=
27,((_tmp6FB.f1=des,_tmp6FB)))),_tmp6FA))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp700;void*_tmp6FF[2];struct Cyc_String_pa_struct _tmp6FE;
struct Cyc_String_pa_struct _tmp6FD;(_tmp6FD.tag=0,((_tmp6FD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp6FF[0]=& _tmp6FE,((_tmp6FF[1]=& _tmp6FD,Cyc_Tcutil_terr(
e->loc,((_tmp700="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp700,sizeof(char),53))),_tag_dyneither(_tmp6FF,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp701;struct
_tuple0 _tmp6C=(_tmp701.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp701.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp701)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 15)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 15)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp704;struct Cyc_Tcexp_TestEnv _tmp703;return(_tmp703.eq=((_tmp704=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp704)),((_tmp704->f1=_tmp6F,((
_tmp704->f2=_tmp72,_tmp704)))))),((_tmp703.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp703)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp705;return(_tmp705.eq=0,((_tmp705.isTrue=0,_tmp705)));};}
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){return 1;}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct
Cyc_Absyn_Exp*e){void*t;void*string_elt_typ=Cyc_Absyn_char_typ;int string_numelts=
0;{union Cyc_Absyn_Cnst _tmp76=*((union Cyc_Absyn_Cnst*)_check_null(c));struct
_tuple4 _tmp77;enum Cyc_Absyn_Sign _tmp78;struct _tuple4 _tmp79;enum Cyc_Absyn_Sign
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
_tmp86 == 0))goto _LL5F;_LL5E:{struct Cyc_Absyn_Const_e_struct _tmp708;struct Cyc_Absyn_Const_e_struct*
_tmp707;e->r=(void*)((_tmp707=_cycalloc(sizeof(*_tmp707)),((_tmp707[0]=((_tmp708.tag=
0,((_tmp708.f1=Cyc_Absyn_Null_c,_tmp708)))),_tmp707))));}{struct Cyc_List_List*
_tmp9C=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp71B;
struct Cyc_Absyn_PtrAtts _tmp71A;struct Cyc_Core_Opt*_tmp719;struct Cyc_Core_Opt*
_tmp718;struct Cyc_Absyn_PtrInfo _tmp717;struct Cyc_Absyn_PointerType_struct*
_tmp716;t=(void*)((_tmp716=_cycalloc(sizeof(*_tmp716)),((_tmp716[0]=((_tmp71B.tag=
5,((_tmp71B.f1=((_tmp717.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp718=_cycalloc(sizeof(*_tmp718)),((_tmp718->v=_tmp9C,_tmp718))))),((_tmp717.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp717.ptr_atts=((_tmp71A.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp719=_cycalloc(sizeof(*_tmp719)),((
_tmp719->v=_tmp9C,_tmp719))))),((_tmp71A.nullable=Cyc_Absyn_true_conref,((
_tmp71A.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp71A.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp71A.ptrloc=0,_tmp71A)))))))))),_tmp717)))))),_tmp71B)))),_tmp716))));}goto
_LL54;};_LL5F: {struct Cyc_Absyn_TagType_struct*_tmp98=(struct Cyc_Absyn_TagType_struct*)
_tmp8A;if(_tmp98->tag != 19)goto _LL61;else{_tmp99=(void*)_tmp98->f1;}}_LL60: {
struct Cyc_Absyn_ValueofType_struct _tmp71E;struct Cyc_Absyn_ValueofType_struct*
_tmp71D;struct Cyc_Absyn_ValueofType_struct*_tmpA3=(_tmp71D=_cycalloc(sizeof(*
_tmp71D)),((_tmp71D[0]=((_tmp71E.tag=18,((_tmp71E.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp86,0),_tmp71E)))),_tmp71D)));if(!Cyc_Tcutil_unify(_tmp99,(void*)
_tmpA3)){{const char*_tmp723;void*_tmp722[2];struct Cyc_String_pa_struct _tmp721;
struct Cyc_String_pa_struct _tmp720;(_tmp720.tag=0,((_tmp720.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA3)),((
_tmp721.tag=0,((_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp99)),((_tmp722[0]=& _tmp721,((_tmp722[1]=& _tmp720,Cyc_Tcutil_terr(loc,((
_tmp723="expecting %s but found %s",_tag_dyneither(_tmp723,sizeof(char),26))),
_tag_dyneither(_tmp722,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp726;struct Cyc_Absyn_TagType_struct*_tmp725;t=(
void*)((_tmp725=_cycalloc(sizeof(*_tmp725)),((_tmp725[0]=((_tmp726.tag=19,((
_tmp726.f1=(void*)((void*)_tmpA3),_tmp726)))),_tmp725))));}goto _LL54;}_LL61:;
_LL62: t=_tmp85 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL54;_LL54:;}goto _LL3D;_LL4E: if((_tmp76.String_c).tag != 8)goto _LL50;_tmp87=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4F: string_numelts=(int)
_get_dyneither_size(_tmp87,sizeof(char));_tmp88=_tmp87;goto _LL51;_LL50: if((
_tmp76.Wstring_c).tag != 9)goto _LL52;_tmp88=(struct _dyneither_ptr)(_tmp76.Wstring_c).val;
_LL51: if(string_numelts == 0){string_numelts=Cyc_Tcexp_wchar_numelts(_tmp88);
string_elt_typ=Cyc_Absyn_wchar_typ();}{struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);{struct Cyc_Core_Opt*
_tmp727;elen->topt=((_tmp727=_cycalloc(sizeof(*_tmp727)),((_tmp727->v=Cyc_Absyn_uint_typ,
_tmp727))));}{struct Cyc_Absyn_Upper_b_struct _tmp72A;struct Cyc_Absyn_Upper_b_struct*
_tmp729;t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp729=_cycalloc(sizeof(*_tmp729)),((_tmp729[0]=((_tmp72A.tag=1,((
_tmp72A.f1=elen,_tmp72A)))),_tmp729)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpB1;struct Cyc_Absyn_Tqual
_tmpB2;_LL68: {struct Cyc_Absyn_ArrayType_struct*_tmpB0=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAF;if(_tmpB0->tag != 8)goto _LL6A;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB1.tq;}}
_LL69: return Cyc_Absyn_array_typ(string_elt_typ,_tmpB2,(struct Cyc_Absyn_Exp*)elen,((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL6A: {struct Cyc_Absyn_PointerType_struct*
_tmpB3=(struct Cyc_Absyn_PointerType_struct*)_tmpAF;if(_tmpB3->tag != 5)goto _LL6C;}
_LL6B: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{
struct Cyc_Core_Opt*_tmp72B;e->topt=((_tmp72B=_cycalloc(sizeof(*_tmp72B)),((
_tmp72B->v=t,_tmp72B))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp72E;struct Cyc_Absyn_Upper_b_struct*
_tmp72D;t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp72D=_cycalloc(sizeof(*_tmp72D)),((_tmp72D[0]=((_tmp72E.tag=1,((
_tmp72E.f1=elen,_tmp72E)))),_tmp72D)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp72F;e->topt=((_tmp72F=_cycalloc(sizeof(*_tmp72F)),((_tmp72F->v=t,_tmp72F))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL67;_LL6C:;_LL6D: goto _LL67;_LL67:;}return t;};_LL52: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp89=(int)(_tmp76.Null_c).val;_LL53: {struct Cyc_List_List*_tmpB8=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp742;struct Cyc_Absyn_PtrAtts _tmp741;
struct Cyc_Core_Opt*_tmp740;struct Cyc_Core_Opt*_tmp73F;struct Cyc_Absyn_PtrInfo
_tmp73E;struct Cyc_Absyn_PointerType_struct*_tmp73D;t=(void*)((_tmp73D=_cycalloc(
sizeof(*_tmp73D)),((_tmp73D[0]=((_tmp742.tag=5,((_tmp742.f1=((_tmp73E.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp73F=_cycalloc(
sizeof(*_tmp73F)),((_tmp73F->v=_tmpB8,_tmp73F))))),((_tmp73E.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp73E.ptr_atts=((_tmp741.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp740=_cycalloc(sizeof(*_tmp740)),((_tmp740->v=_tmpB8,_tmp740))))),((_tmp741.nullable=
Cyc_Absyn_true_conref,((_tmp741.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp741.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp741.ptrloc=0,_tmp741)))))))))),_tmp73E)))))),_tmp742)))),_tmp73D))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){void*_tmpBF=b;
struct Cyc_Absyn_Vardecl*_tmpC2;struct Cyc_Absyn_Fndecl*_tmpC4;struct Cyc_Absyn_Vardecl*
_tmpC6;struct Cyc_Absyn_Vardecl*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpCA;_LL6F: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpC0=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBF;if(_tmpC0->tag != 0)goto _LL71;}_LL70: {const char*_tmp746;void*_tmp745[1];
struct Cyc_String_pa_struct _tmp744;return(_tmp744.tag=0,((_tmp744.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp745[0]=&
_tmp744,Cyc_Tcexp_expr_err(te,loc,0,((_tmp746="undeclared identifier: %s",
_tag_dyneither(_tmp746,sizeof(char),26))),_tag_dyneither(_tmp745,sizeof(void*),1)))))));}
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
else{_tmpCA=_tmpC9->f1;}}_LL7A:{union Cyc_Absyn_Nmspace _tmp747;(*q).f1=(((_tmp747.Loc_n).val=
0,(((_tmp747.Loc_n).tag=4,_tmp747))));}if(te->allow_valueof){void*_tmpCF=Cyc_Tcutil_compress(
_tmpCA->type);void*_tmpD1;_LL7C: {struct Cyc_Absyn_TagType_struct*_tmpD0=(struct
Cyc_Absyn_TagType_struct*)_tmpCF;if(_tmpD0->tag != 19)goto _LL7E;else{_tmpD1=(void*)
_tmpD0->f1;}}_LL7D:{struct Cyc_Absyn_Valueof_e_struct _tmp74A;struct Cyc_Absyn_Valueof_e_struct*
_tmp749;e->r=(void*)((_tmp749=_cycalloc(sizeof(*_tmp749)),((_tmp749[0]=((_tmp74A.tag=
39,((_tmp74A.f1=(void*)_tmpD1,_tmp74A)))),_tmp749))));}goto _LL7B;_LL7E:;_LL7F:
goto _LL7B;_LL7B:;}return _tmpCA->type;_LL6E:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int
arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
unsigned int)){struct Cyc_List_List*desc_types;{void*_tmpD4=fmt->r;union Cyc_Absyn_Cnst
_tmpD6;struct _dyneither_ptr _tmpD7;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp
_tmpDA;void*_tmpDB;union Cyc_Absyn_Cnst _tmpDD;struct _dyneither_ptr _tmpDE;_LL81: {
struct Cyc_Absyn_Const_e_struct*_tmpD5=(struct Cyc_Absyn_Const_e_struct*)_tmpD4;
if(_tmpD5->tag != 0)goto _LL83;else{_tmpD6=_tmpD5->f1;if((_tmpD6.String_c).tag != 8)
goto _LL83;_tmpD7=(struct _dyneither_ptr)(_tmpD6.String_c).val;}}_LL82: _tmpDE=
_tmpD7;goto _LL84;_LL83: {struct Cyc_Absyn_Cast_e_struct*_tmpD8=(struct Cyc_Absyn_Cast_e_struct*)
_tmpD4;if(_tmpD8->tag != 15)goto _LL85;else{_tmpD9=_tmpD8->f2;_tmpDA=*_tmpD9;
_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Const_e_struct*_tmpDC=(struct Cyc_Absyn_Const_e_struct*)
_tmpDB;if(_tmpDC->tag != 0)goto _LL85;else{_tmpDD=_tmpDC->f1;if((_tmpDD.String_c).tag
!= 8)goto _LL85;_tmpDE=(struct _dyneither_ptr)(_tmpDD.String_c).val;}};}}_LL84:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDE,fmt->loc);goto _LL80;
_LL85:;_LL86: if(opt_args != 0){struct Cyc_List_List*_tmpDF=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDF->hd);{struct _RegionHandle _tmpE0=_new_region("temp");struct _RegionHandle*
temp=& _tmpE0;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDF->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpDF->hd)){const
char*_tmp74D;void*_tmp74C;(_tmp74C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDF->hd)->loc,((_tmp74D="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp74D,sizeof(char),49))),_tag_dyneither(_tmp74C,sizeof(void*),0)));};
_pop_region(temp);};}}return;_LL80:;}if(opt_args != 0){struct Cyc_List_List*_tmpE3=(
struct Cyc_List_List*)opt_args->v;for(0;desc_types != 0  && _tmpE3 != 0;(((
desc_types=desc_types->tl,_tmpE3=_tmpE3->tl)),arg_cnt ++)){int alias_coercion=0;
void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE3->hd;
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,&
alias_coercion)){{const char*_tmp752;void*_tmp751[2];struct Cyc_String_pa_struct
_tmp750;struct Cyc_String_pa_struct _tmp74F;(_tmp74F.tag=0,((_tmp74F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp750.tag=0,((_tmp750.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp751[0]=& _tmp750,((_tmp751[
1]=& _tmp74F,Cyc_Tcutil_terr(e->loc,((_tmp752="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp752,sizeof(char),51))),_tag_dyneither(_tmp751,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp753;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp753=_cycalloc(sizeof(*_tmp753)),((
_tmp753->hd=(void*)arg_cnt,((_tmp753->tl=*alias_arg_exps,_tmp753))))));}{struct
_RegionHandle _tmpE9=_new_region("temp");struct _RegionHandle*temp=& _tmpE9;
_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(
temp,e)){const char*_tmp756;void*_tmp755;(_tmp755=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpE3->hd)->loc,((_tmp756="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp756,sizeof(char),49))),_tag_dyneither(_tmp755,sizeof(void*),0)));};
_pop_region(temp);};}if(desc_types != 0){const char*_tmp759;void*_tmp758;(_tmp758=
0,Cyc_Tcutil_terr(fmt->loc,((_tmp759="too few arguments",_tag_dyneither(_tmp759,
sizeof(char),18))),_tag_dyneither(_tmp758,sizeof(void*),0)));}if(_tmpE3 != 0){
const char*_tmp75C;void*_tmp75B;(_tmp75B=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpE3->hd)->loc,((_tmp75C="too many arguments",_tag_dyneither(_tmp75C,sizeof(
char),19))),_tag_dyneither(_tmp75B,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct
Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL87: goto _LL88;case Cyc_Absyn_Minus:
_LL88: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp760;void*_tmp75F[1];struct Cyc_String_pa_struct
_tmp75E;(_tmp75E.tag=0,((_tmp75E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp75F[0]=& _tmp75E,Cyc_Tcutil_terr(loc,((_tmp760="expecting arithmetic type but found %s",
_tag_dyneither(_tmp760,sizeof(char),39))),_tag_dyneither(_tmp75F,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL89: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp764;void*_tmp763[1];struct Cyc_String_pa_struct _tmp762;(_tmp762.tag=
0,((_tmp762.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp763[0]=& _tmp762,Cyc_Tcutil_terr(loc,((_tmp764="expecting integral or * type but found %s",
_tag_dyneither(_tmp764,sizeof(char),42))),_tag_dyneither(_tmp763,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL8A: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp768;void*_tmp767[1];struct Cyc_String_pa_struct _tmp766;(_tmp766.tag=
0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp767[0]=& _tmp766,Cyc_Tcutil_terr(loc,((_tmp768="expecting integral type but found %s",
_tag_dyneither(_tmp768,sizeof(char),37))),_tag_dyneither(_tmp767,sizeof(void*),1)))))));}
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
_tmp76B;void*_tmp76A;(_tmp76A=0,Cyc_Tcutil_terr(loc,((_tmp76B="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp76B,sizeof(char),55))),_tag_dyneither(_tmp76A,sizeof(void*),0)));}
goto _LL94;_LL94:;}goto _LL8D;_LL92:;_LL93: {const char*_tmp76F;void*_tmp76E[1];
struct Cyc_String_pa_struct _tmp76D;(_tmp76D.tag=0,((_tmp76D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp76E[0]=&
_tmp76D,Cyc_Tcutil_terr(loc,((_tmp76F="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp76F,sizeof(char),47))),_tag_dyneither(_tmp76E,sizeof(void*),1)))))));}
_LL8D:;}return Cyc_Absyn_uint_typ;default: _LL8C: {const char*_tmp772;void*_tmp771;(
_tmp771=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp772="Non-unary primop",_tag_dyneither(_tmp772,sizeof(char),17))),
_tag_dyneither(_tmp771,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp776;void*_tmp775[
1];struct Cyc_String_pa_struct _tmp774;(_tmp774.tag=0,((_tmp774.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp775[0]=& _tmp774,Cyc_Tcutil_terr(e1->loc,((
_tmp776="type %s cannot be used here",_tag_dyneither(_tmp776,sizeof(char),28))),
_tag_dyneither(_tmp775,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp77A;void*_tmp779[1];struct Cyc_String_pa_struct
_tmp778;(_tmp778.tag=0,((_tmp778.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp779[0]=& _tmp778,Cyc_Tcutil_terr(e2->loc,((_tmp77A="type %s cannot be used here",
_tag_dyneither(_tmp77A,sizeof(char),28))),_tag_dyneither(_tmp779,sizeof(void*),1)))))));}
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
_tmp77D;void*_tmp77C;(_tmp77C=0,Cyc_Tcutil_terr(e1->loc,((_tmp77D="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp77D,sizeof(char),50))),_tag_dyneither(_tmp77C,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp780;void*_tmp77F;(_tmp77F=0,
Cyc_Tcutil_terr(e1->loc,((_tmp780="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp780,sizeof(char),54))),_tag_dyneither(_tmp77F,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp784;void*_tmp783[1];struct
Cyc_String_pa_struct _tmp782;(_tmp782.tag=0,((_tmp782.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp783[0]=& _tmp782,Cyc_Tcutil_terr(
e2->loc,((_tmp784="expecting int but found %s",_tag_dyneither(_tmp784,sizeof(
char),27))),_tag_dyneither(_tmp783,sizeof(void*),1)))))));}{void*_tmp121=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp118);struct Cyc_Absyn_Exp*_tmp124;_LLA0: {struct Cyc_Absyn_DynEither_b_struct*
_tmp122=(struct Cyc_Absyn_DynEither_b_struct*)_tmp121;if(_tmp122->tag != 0)goto
_LLA2;}_LLA1: return t1;_LLA2: {struct Cyc_Absyn_Upper_b_struct*_tmp123=(struct Cyc_Absyn_Upper_b_struct*)
_tmp121;if(_tmp123->tag != 1)goto _LL9F;else{_tmp124=_tmp123->f1;}}_LLA3: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp119)){const char*
_tmp787;void*_tmp786;(_tmp786=0,Cyc_Tcutil_warn(e1->loc,((_tmp787="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp787,sizeof(char),70))),_tag_dyneither(_tmp786,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp791;struct Cyc_Absyn_PtrAtts _tmp790;struct
Cyc_Absyn_PtrInfo _tmp78F;struct Cyc_Absyn_PointerType_struct*_tmp78E;struct Cyc_Absyn_PointerType_struct*
_tmp127=(_tmp78E=_cycalloc(sizeof(*_tmp78E)),((_tmp78E[0]=((_tmp791.tag=5,((
_tmp791.f1=((_tmp78F.elt_typ=_tmp113,((_tmp78F.elt_tq=_tmp114,((_tmp78F.ptr_atts=((
_tmp790.rgn=_tmp116,((_tmp790.nullable=Cyc_Absyn_true_conref,((_tmp790.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp790.zero_term=_tmp119,((_tmp790.ptrloc=0,
_tmp790)))))))))),_tmp78F)))))),_tmp791)))),_tmp78E)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp127,Cyc_Absyn_Other_coercion);return(void*)_tmp127;};_LL9F:;};
_LL9D:;_LL9E: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL9A:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp796;void*_tmp795[2];struct Cyc_String_pa_struct _tmp794;struct Cyc_String_pa_struct
_tmp793;(_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp794.tag=0,((_tmp794.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp795[0]=& _tmp794,((
_tmp795[1]=& _tmp793,Cyc_Tcutil_terr(e1->loc,((_tmp796="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp796,sizeof(char),59))),_tag_dyneither(_tmp795,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp799;void*_tmp798;(
_tmp798=0,Cyc_Tcutil_terr(e1->loc,((_tmp799="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp799,sizeof(char),50))),_tag_dyneither(_tmp798,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp79C;void*_tmp79B;(_tmp79B=0,
Cyc_Tcutil_terr(e1->loc,((_tmp79C="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp79C,sizeof(char),54))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp7A1;void*_tmp7A0[2];struct
Cyc_String_pa_struct _tmp79F;struct Cyc_String_pa_struct _tmp79E;(_tmp79E.tag=0,((
_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp79F.tag=0,((_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7A0[0]=& _tmp79F,((_tmp7A0[1]=& _tmp79E,Cyc_Tcutil_terr(
e2->loc,((_tmp7A1="expecting either %s or int but found %s",_tag_dyneither(
_tmp7A1,sizeof(char),40))),_tag_dyneither(_tmp7A0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);if(Cyc_Tcutil_is_pointer_type(
t2))Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,
e1,t2,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp7A2;struct _tuple0
_tmp139=(_tmp7A2.f1=Cyc_Tcutil_compress(t1),((_tmp7A2.f2=Cyc_Tcutil_compress(t2),
_tmp7A2)));void*_tmp13A;struct Cyc_Absyn_PtrInfo _tmp13C;void*_tmp13D;void*_tmp13E;
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
_LLAA: goto _LLA4;_LLA4:;}{const char*_tmp7A7;void*_tmp7A6[2];struct Cyc_String_pa_struct
_tmp7A5;struct Cyc_String_pa_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7A5.tag=
0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7A6[0]=& _tmp7A5,((_tmp7A6[1]=& _tmp7A4,Cyc_Tcutil_terr(loc,((_tmp7A7="comparison not allowed between %s and %s",
_tag_dyneither(_tmp7A7,sizeof(char),41))),_tag_dyneither(_tmp7A6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,
e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp7A8;
struct _tuple0 _tmp14B=(_tmp7A8.f1=t1,((_tmp7A8.f2=t2,_tmp7A8)));void*_tmp14C;void*
_tmp14E;void*_tmp14F;void*_tmp151;_LLAC: _tmp14C=_tmp14B.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 15)goto
_LLAE;else{_tmp14E=(void*)_tmp14D->f1;}};_tmp14F=_tmp14B.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp150=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14F;if(_tmp150->tag != 15)goto
_LLAE;else{_tmp151=(void*)_tmp150->f1;}};_LLAD: return Cyc_Absyn_sint_typ;_LLAE:;
_LLAF: goto _LLAB;_LLAB:;}{const char*_tmp7AD;void*_tmp7AC[2];struct Cyc_String_pa_struct
_tmp7AB;struct Cyc_String_pa_struct _tmp7AA;(_tmp7AA.tag=0,((_tmp7AA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7AB.tag=
0,((_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp7AC[0]=& _tmp7AB,((_tmp7AC[1]=& _tmp7AA,Cyc_Tcutil_terr(loc,((_tmp7AD="comparison not allowed between %s and %s",
_tag_dyneither(_tmp7AD,sizeof(char),41))),_tag_dyneither(_tmp7AC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));};}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
switch(p){case Cyc_Absyn_Plus: _LLB0: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLB1: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLB2: goto _LLB3;case
Cyc_Absyn_Div: _LLB3: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLB4: goto _LLB5;case Cyc_Absyn_Bitand: _LLB5: goto _LLB6;case Cyc_Absyn_Bitor:
_LLB6: goto _LLB7;case Cyc_Absyn_Bitxor: _LLB7: goto _LLB8;case Cyc_Absyn_Bitlshift:
_LLB8: goto _LLB9;case Cyc_Absyn_Bitlrshift: _LLB9: goto _LLBA;case Cyc_Absyn_Bitarshift:
_LLBA: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLBB: goto _LLBC;case Cyc_Absyn_Neq: _LLBC: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLBD: goto _LLBE;case Cyc_Absyn_Lt: _LLBE: goto _LLBF;case Cyc_Absyn_Gte:
_LLBF: goto _LLC0;case Cyc_Absyn_Lte: _LLC0: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLC1: {const char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B0="bad binary primop",
_tag_dyneither(_tmp7B0,sizeof(char),18))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,
enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus  && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(te,topt,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLC3: {const char*_tmp7B3;void*_tmp7B2;return(_tmp7B2=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp7B3="primitive operator has 0 arguments",_tag_dyneither(_tmp7B3,
sizeof(char),35))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}case 1: _LLC4: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC5: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC6: {const char*_tmp7B6;void*_tmp7B5;return(_tmp7B5=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp7B6="primitive operator has > 2 arguments",_tag_dyneither(_tmp7B6,
sizeof(char),37))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));}}return t;};}struct
_tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};static int Cyc_Tcexp_check_writable_aggr(
unsigned int loc,void*t){void*_tmp15C=Cyc_Tcutil_compress(t);void*_tmp15D=_tmp15C;
struct Cyc_Absyn_AggrInfo _tmp15F;union Cyc_Absyn_AggrInfoU _tmp160;struct Cyc_Absyn_Aggrdecl**
_tmp161;struct Cyc_Absyn_Aggrdecl*_tmp162;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_DatatypeFieldInfo
_tmp166;union Cyc_Absyn_DatatypeFieldInfoU _tmp167;struct _tuple2 _tmp168;struct Cyc_Absyn_Datatypefield*
_tmp169;struct Cyc_Absyn_ArrayInfo _tmp16B;void*_tmp16C;struct Cyc_Absyn_Tqual
_tmp16D;struct Cyc_List_List*_tmp16F;_LLC9: {struct Cyc_Absyn_AggrType_struct*
_tmp15E=(struct Cyc_Absyn_AggrType_struct*)_tmp15D;if(_tmp15E->tag != 11)goto _LLCB;
else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.aggr_info;if((_tmp160.KnownAggr).tag != 
2)goto _LLCB;_tmp161=(struct Cyc_Absyn_Aggrdecl**)(_tmp160.KnownAggr).val;_tmp162=*
_tmp161;}}_LLCA: _tmp164=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp162->impl))->fields;
goto _LLCC;_LLCB: {struct Cyc_Absyn_AnonAggrType_struct*_tmp163=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp15D;if(_tmp163->tag != 12)goto _LLCD;else{_tmp164=_tmp163->f2;}}_LLCC: for(0;
_tmp164 != 0;_tmp164=_tmp164->tl){struct Cyc_Absyn_Aggrfield*_tmp170=(struct Cyc_Absyn_Aggrfield*)
_tmp164->hd;if((_tmp170->tq).real_const){{const char*_tmp7BA;void*_tmp7B9[1];
struct Cyc_String_pa_struct _tmp7B8;(_tmp7B8.tag=0,((_tmp7B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp170->name),((_tmp7B9[0]=& _tmp7B8,Cyc_Tcutil_terr(
loc,((_tmp7BA="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp7BA,sizeof(char),56))),_tag_dyneither(_tmp7B9,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp170->type))return 0;}return 1;
_LLCD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15D;if(_tmp165->tag != 4)goto _LLCF;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.field_info;
if((_tmp167.KnownDatatypefield).tag != 2)goto _LLCF;_tmp168=(struct _tuple2)(
_tmp167.KnownDatatypefield).val;_tmp169=_tmp168.f2;}}_LLCE:{struct Cyc_List_List*
_tmp174=_tmp169->typs;for(0;_tmp174 != 0;_tmp174=_tmp174->tl){struct _tuple19
_tmp176;struct Cyc_Absyn_Tqual _tmp177;void*_tmp178;struct _tuple19*_tmp175=(struct
_tuple19*)_tmp174->hd;_tmp176=*_tmp175;_tmp177=_tmp176.f1;_tmp178=_tmp176.f2;if(
_tmp177.real_const){{const char*_tmp7BE;void*_tmp7BD[1];struct Cyc_String_pa_struct
_tmp7BC;(_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp169->name)),((_tmp7BD[0]=& _tmp7BC,Cyc_Tcutil_terr(
loc,((_tmp7BE="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp7BE,sizeof(char),64))),_tag_dyneither(_tmp7BD,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp178))return 0;}}return 1;_LLCF: {
struct Cyc_Absyn_ArrayType_struct*_tmp16A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15D;if(_tmp16A->tag != 8)goto _LLD1;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16B.elt_type;
_tmp16D=_tmp16B.tq;}}_LLD0: if(_tmp16D.real_const){{const char*_tmp7C1;void*
_tmp7C0;(_tmp7C0=0,Cyc_Tcutil_terr(loc,((_tmp7C1="attempt to over-write a const array",
_tag_dyneither(_tmp7C1,sizeof(char),36))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp16C);_LLD1: {struct Cyc_Absyn_TupleType_struct*
_tmp16E=(struct Cyc_Absyn_TupleType_struct*)_tmp15D;if(_tmp16E->tag != 10)goto
_LLD3;else{_tmp16F=_tmp16E->f1;}}_LLD2: for(0;_tmp16F != 0;_tmp16F=_tmp16F->tl){
struct _tuple19 _tmp17F;struct Cyc_Absyn_Tqual _tmp180;void*_tmp181;struct _tuple19*
_tmp17E=(struct _tuple19*)_tmp16F->hd;_tmp17F=*_tmp17E;_tmp180=_tmp17F.f1;_tmp181=
_tmp17F.f2;if(_tmp180.real_const){{const char*_tmp7C4;void*_tmp7C3;(_tmp7C3=0,Cyc_Tcutil_terr(
loc,((_tmp7C4="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp7C4,sizeof(char),56))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}
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
_tmp1AF=_tmp1AD.f2;if(!_tmp1AF){{const char*_tmp7C7;void*_tmp7C6;(_tmp7C6=0,Cyc_Tcutil_terr(
e->loc,((_tmp7C7="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp7C7,sizeof(char),47))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}return;}{
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
_LLEB: goto _LLD5;_LLD5:;}{const char*_tmp7CB;void*_tmp7CA[1];struct Cyc_String_pa_struct
_tmp7C9;(_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp7CA[0]=& _tmp7C9,Cyc_Tcutil_terr(e->loc,((
_tmp7CB="attempt to write a const location: %s",_tag_dyneither(_tmp7CB,sizeof(
char),38))),_tag_dyneither(_tmp7CA,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum 
Cyc_Absyn_Incrementor i){struct _RegionHandle _tmp1D7=_new_region("temp");struct
_RegionHandle*temp=& _tmp1D7;_push_region(temp);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(
temp,te),0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*_tmp7CE;void*_tmp7CD;void*
_tmp1DA=(_tmp7CD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7CE="increment/decrement of non-lvalue",
_tag_dyneither(_tmp7CE,sizeof(char),34))),_tag_dyneither(_tmp7CD,sizeof(void*),0)));
_npop_handler(0);return _tmp1DA;}Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*
telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp7D1;void*_tmp7D0;(_tmp7D0=0,Cyc_Tcutil_terr(e->loc,((
_tmp7D1="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp7D1,sizeof(char),50))),_tag_dyneither(_tmp7D0,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp7D4;void*_tmp7D3;(_tmp7D3=0,Cyc_Tcutil_terr(e->loc,((_tmp7D4="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp7D4,sizeof(char),54))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));}}
else{const char*_tmp7D8;void*_tmp7D7[1];struct Cyc_String_pa_struct _tmp7D6;(
_tmp7D6.tag=0,((_tmp7D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp7D7[0]=& _tmp7D6,Cyc_Tcutil_terr(e->loc,((_tmp7D8="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp7D8,sizeof(char),44))),_tag_dyneither(_tmp7D7,sizeof(void*),1)))))));}}{
void*_tmp1E2=t;_npop_handler(0);return _tmp1E2;};};;_pop_region(temp);}static void*
Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct
_tuple0*_tmp1E5;int _tmp1E6;const char*_tmp7D9;struct Cyc_Tcexp_TestEnv _tmp1E4=Cyc_Tcexp_tcTest(
te,e1,((_tmp7D9="conditional expression",_tag_dyneither(_tmp7D9,sizeof(char),23))));
_tmp1E5=_tmp1E4.eq;_tmp1E6=_tmp1E4.isTrue;Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp7DA;struct Cyc_List_List _tmp1E7=(_tmp7DA.hd=e3,((
_tmp7DA.tl=0,_tmp7DA)));struct Cyc_List_List _tmp7DB;struct Cyc_List_List _tmp1E8=(
_tmp7DB.hd=e2,((_tmp7DB.tl=(struct Cyc_List_List*)& _tmp1E7,_tmp7DB)));if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp1E8)){{const char*_tmp7E0;void*_tmp7DF[2];struct
Cyc_String_pa_struct _tmp7DE;struct Cyc_String_pa_struct _tmp7DD;(_tmp7DD.tag=0,((
_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v)),((_tmp7DE.tag=0,((
_tmp7DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp7DF[0]=& _tmp7DE,((
_tmp7DF[1]=& _tmp7DD,Cyc_Tcutil_terr(loc,((_tmp7E0="conditional clause types do not match: %s != %s",
_tag_dyneither(_tmp7E0,sizeof(char),48))),_tag_dyneither(_tmp7DF,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*
_tmp7E1;Cyc_Tcexp_tcTest(te,e1,((_tmp7E1="logical-and expression",_tag_dyneither(
_tmp7E1,sizeof(char),23))));}{const char*_tmp7E2;Cyc_Tcexp_tcTest(te,e2,((_tmp7E2="logical-and expression",
_tag_dyneither(_tmp7E2,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){{const char*_tmp7E3;Cyc_Tcexp_tcTest(te,e1,((_tmp7E3="logical-or expression",
_tag_dyneither(_tmp7E3,sizeof(char),22))));}{const char*_tmp7E4;Cyc_Tcexp_tcTest(
te,e2,((_tmp7E4="logical-or expression",_tag_dyneither(_tmp7E4,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,
struct Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp1F3=_new_region("r");struct
_RegionHandle*r=& _tmp1F3;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(
r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((
void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp1F4=Cyc_Tcutil_compress(t1);_LL115: {struct
Cyc_Absyn_ArrayType_struct*_tmp1F5=(struct Cyc_Absyn_ArrayType_struct*)_tmp1F4;
if(_tmp1F5->tag != 8)goto _LL117;}_LL116:{const char*_tmp7E7;void*_tmp7E6;(_tmp7E6=
0,Cyc_Tcutil_terr(loc,((_tmp7E7="cannot assign to an array",_tag_dyneither(
_tmp7E7,sizeof(char),26))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}goto _LL114;
_LL117:;_LL118: goto _LL114;_LL114:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),& Cyc_Tcutil_tmk)){const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,Cyc_Tcutil_terr(
loc,((_tmp7EA="type is abstract (can't determine size).",_tag_dyneither(_tmp7EA,
sizeof(char),41))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp7ED;void*_tmp7EC;void*_tmp1FC=(_tmp7EC=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7ED="assignment to non-lvalue",_tag_dyneither(_tmp7ED,sizeof(
char),25))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));_npop_handler(0);return
_tmp1FC;}Cyc_Tcexp_check_writable(te,e1);if(po == 0){struct _RegionHandle _tmp1FD=
_new_region("temp");struct _RegionHandle*temp=& _tmp1FD;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){const char*_tmp7F0;void*_tmp7EF;(
_tmp7EF=0,Cyc_Tcutil_terr(e2->loc,((_tmp7F0="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7F0,sizeof(char),49))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp7F5;void*_tmp7F4[2];struct
Cyc_String_pa_struct _tmp7F3;struct Cyc_String_pa_struct _tmp7F2;void*_tmp200=(
_tmp7F2.tag=0,((_tmp7F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp7F3.tag=0,((_tmp7F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7F4[0]=& _tmp7F3,((_tmp7F4[1]=& _tmp7F2,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7F5="type mismatch: %s != %s",_tag_dyneither(_tmp7F5,sizeof(
char),24))),_tag_dyneither(_tmp7F4,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();{void*_tmp201=_tmp200;_npop_handler(1);
return _tmp201;};};_pop_region(temp);}else{enum Cyc_Absyn_Primop _tmp206=(enum Cyc_Absyn_Primop)
po->v;void*_tmp207=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp206,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp207,t1) || Cyc_Tcutil_coerceable(_tmp207) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7FA;void*_tmp7F9[2];struct Cyc_String_pa_struct _tmp7F8;struct Cyc_String_pa_struct
_tmp7F7;void*_tmp208=(_tmp7F7.tag=0,((_tmp7F7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7F8.tag=0,((_tmp7F8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7F9[0]=&
_tmp7F8,((_tmp7F9[1]=& _tmp7F7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FA="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp7FA,sizeof(char),82))),_tag_dyneither(_tmp7F9,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp207,t1);Cyc_Tcutil_explain_failure();{void*_tmp209=_tmp208;
_npop_handler(0);return _tmp209;};}{void*_tmp20E=_tmp207;_npop_handler(0);return
_tmp20E;};}{void*_tmp20F=t1;_npop_handler(0);return _tmp20F;};};;_pop_region(r);}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(
te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;}static
struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs){struct Cyc_List_List*fields;void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;{void*_tmp210=Cyc_Tcutil_compress(t1);int _tmp212;enum 
Cyc_Absyn_Size_of _tmp214;enum Cyc_Absyn_Size_of _tmp216;_LL11A: {struct Cyc_Absyn_FloatType_struct*
_tmp211=(struct Cyc_Absyn_FloatType_struct*)_tmp210;if(_tmp211->tag != 7)goto
_LL11C;else{_tmp212=_tmp211->f1;if(_tmp212 != 0)goto _LL11C;}}_LL11B: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);
goto _LL119;_LL11C: {struct Cyc_Absyn_IntType_struct*_tmp213=(struct Cyc_Absyn_IntType_struct*)
_tmp210;if(_tmp213->tag != 6)goto _LL11E;else{_tmp214=_tmp213->f2;if(_tmp214 != Cyc_Absyn_Char_sz)
goto _LL11E;}}_LL11D: goto _LL11F;_LL11E: {struct Cyc_Absyn_IntType_struct*_tmp215=(
struct Cyc_Absyn_IntType_struct*)_tmp210;if(_tmp215->tag != 6)goto _LL120;else{
_tmp216=_tmp215->f2;if(_tmp216 != Cyc_Absyn_Short_sz)goto _LL120;}}_LL11F: Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL119;
_LL120:;_LL121: goto _LL119;_LL119:;}for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp218;struct Cyc_List_List*_tmp219;unsigned int _tmp21A;enum Cyc_Absyn_Scope
_tmp21B;struct Cyc_Absyn_Datatypefield _tmp217=*((struct Cyc_Absyn_Datatypefield*)
fields->hd);_tmp218=_tmp217.name;_tmp219=_tmp217.typs;_tmp21A=_tmp217.loc;
_tmp21B=_tmp217.sc;if(_tmp219 == 0  || _tmp219->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(
r,inst,(*((struct _tuple19*)_tmp219->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(
struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp21D;struct Cyc_List_List*
_tmp21E;unsigned int _tmp21F;enum Cyc_Absyn_Scope _tmp220;struct Cyc_Absyn_Datatypefield
_tmp21C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp21D=_tmp21C.name;
_tmp21E=_tmp21C.typs;_tmp21F=_tmp21C.loc;_tmp220=_tmp21C.sc;if(_tmp21E == 0  || 
_tmp21E->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple19*)_tmp21E->hd)).f2);int bogus=0;if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}{
const char*_tmp7FF;void*_tmp7FE[2];struct Cyc_String_pa_struct _tmp7FD;struct Cyc_String_pa_struct
_tmp7FC;(_tmp7FC.tag=0,((_tmp7FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7FD.tag=0,((_tmp7FD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7FE[0]=& _tmp7FD,((_tmp7FE[
1]=& _tmp7FC,Cyc_Tcutil_terr(e->loc,((_tmp7FF="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp7FF,sizeof(char),54))),_tag_dyneither(_tmp7FE,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info,struct Cyc_List_List**alias_arg_exps){struct Cyc_List_List*
_tmp225=args;int _tmp226=0;struct _RegionHandle _tmp227=_new_region("ter");struct
_RegionHandle*ter=& _tmp227;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp228=Cyc_Tcenv_new_block(
ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp228,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp229=t;struct Cyc_Absyn_PtrInfo
_tmp22B;void*_tmp22C;struct Cyc_Absyn_Tqual _tmp22D;struct Cyc_Absyn_PtrAtts _tmp22E;
void*_tmp22F;union Cyc_Absyn_Constraint*_tmp230;union Cyc_Absyn_Constraint*_tmp231;
union Cyc_Absyn_Constraint*_tmp232;_LL123: {struct Cyc_Absyn_PointerType_struct*
_tmp22A=(struct Cyc_Absyn_PointerType_struct*)_tmp229;if(_tmp22A->tag != 5)goto
_LL125;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22B.elt_typ;_tmp22D=_tmp22B.elt_tq;
_tmp22E=_tmp22B.ptr_atts;_tmp22F=_tmp22E.rgn;_tmp230=_tmp22E.nullable;_tmp231=
_tmp22E.bounds;_tmp232=_tmp22E.zero_term;}}_LL124: Cyc_Tcenv_check_rgn_accessible(
_tmp228,loc,_tmp22F);Cyc_Tcutil_check_nonzero_bound(loc,_tmp231);{void*_tmp233=
Cyc_Tcutil_compress(_tmp22C);struct Cyc_Absyn_FnInfo _tmp235;struct Cyc_List_List*
_tmp236;struct Cyc_Core_Opt*_tmp237;struct Cyc_Absyn_Tqual _tmp238;void*_tmp239;
struct Cyc_List_List*_tmp23A;int _tmp23B;struct Cyc_Absyn_VarargInfo*_tmp23C;struct
Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23E;_LL128: {struct Cyc_Absyn_FnType_struct*
_tmp234=(struct Cyc_Absyn_FnType_struct*)_tmp233;if(_tmp234->tag != 9)goto _LL12A;
else{_tmp235=_tmp234->f1;_tmp236=_tmp235.tvars;_tmp237=_tmp235.effect;_tmp238=
_tmp235.ret_tqual;_tmp239=_tmp235.ret_typ;_tmp23A=_tmp235.args;_tmp23B=_tmp235.c_varargs;
_tmp23C=_tmp235.cyc_varargs;_tmp23D=_tmp235.rgn_po;_tmp23E=_tmp235.attributes;}}
_LL129: if(topt != 0)Cyc_Tcutil_unify(_tmp239,*topt);while(_tmp225 != 0  && _tmp23A
!= 0){int alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp225->hd;
void*t2=(*((struct _tuple9*)_tmp23A->hd)).f3;Cyc_Tcexp_tcExp(_tmp228,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp228,e1,t2,& alias_coercion)){{const char*_tmp804;
void*_tmp803[2];struct Cyc_String_pa_struct _tmp802;struct Cyc_String_pa_struct
_tmp801;(_tmp801.tag=0,((_tmp801.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp802.tag=0,((_tmp802.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp803[0]=& _tmp802,((_tmp803[1]=& _tmp801,Cyc_Tcutil_terr(
e1->loc,((_tmp804="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp804,sizeof(char),57))),_tag_dyneither(_tmp803,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp805;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp805=_cycalloc(sizeof(*_tmp805)),((_tmp805->hd=(void*)_tmp226,((_tmp805->tl=*
alias_arg_exps,_tmp805))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2)
 && !Cyc_Tcutil_is_noalias_path(ter,e1)){const char*_tmp808;void*_tmp807;(_tmp807=
0,Cyc_Tcutil_terr(e1->loc,((_tmp808="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp808,sizeof(char),49))),_tag_dyneither(_tmp807,sizeof(void*),0)));}
_tmp225=_tmp225->tl;_tmp23A=_tmp23A->tl;++ _tmp226;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp23E;for(0;a != 0;a=a->tl){void*_tmp246=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp248;int _tmp249;int _tmp24A;_LL12D: {struct Cyc_Absyn_Format_att_struct*
_tmp247=(struct Cyc_Absyn_Format_att_struct*)_tmp246;if(_tmp247->tag != 19)goto
_LL12F;else{_tmp248=_tmp247->f1;_tmp249=_tmp247->f2;_tmp24A=_tmp247->f3;}}_LL12E:{
struct _handler_cons _tmp24B;_push_handler(& _tmp24B);{int _tmp24D=0;if(setjmp(
_tmp24B.handler))_tmp24D=1;if(!_tmp24D){{struct Cyc_Absyn_Exp*_tmp24E=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp249 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp24A == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp809;fmt_args=((
_tmp809=_cycalloc(sizeof(*_tmp809)),((_tmp809->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp24A - 1),_tmp809))));}
args_already_checked=1;{struct _RegionHandle _tmp250=_new_region("temp");struct
_RegionHandle*temp=& _tmp250;_push_region(temp);switch(_tmp248){case Cyc_Absyn_Printf_ft:
_LL131: Cyc_Tcexp_check_format_args(_tmp228,_tmp24E,fmt_args,_tmp24A - 1,
alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);break;case Cyc_Absyn_Scanf_ft:
_LL132: Cyc_Tcexp_check_format_args(_tmp228,_tmp24E,fmt_args,_tmp24A - 1,
alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);break;};_pop_region(temp);};};
_pop_handler();}else{void*_tmp24C=(void*)_exn_thrown;void*_tmp252=_tmp24C;_LL135: {
struct Cyc_List_Nth_struct*_tmp253=(struct Cyc_List_Nth_struct*)_tmp252;if(_tmp253->tag
!= Cyc_List_Nth)goto _LL137;}_LL136:{const char*_tmp80C;void*_tmp80B;(_tmp80B=0,
Cyc_Tcutil_terr(loc,((_tmp80C="bad format arguments",_tag_dyneither(_tmp80C,
sizeof(char),21))),_tag_dyneither(_tmp80B,sizeof(void*),0)));}goto _LL134;_LL137:;
_LL138:(void)_throw(_tmp252);_LL134:;}};}goto _LL12C;_LL12F:;_LL130: goto _LL12C;
_LL12C:;}}if(_tmp23A != 0){const char*_tmp80F;void*_tmp80E;(_tmp80E=0,Cyc_Tcutil_terr(
loc,((_tmp80F="too few arguments for function",_tag_dyneither(_tmp80F,sizeof(
char),31))),_tag_dyneither(_tmp80E,sizeof(void*),0)));}else{if((_tmp225 != 0  || 
_tmp23B) || _tmp23C != 0){if(_tmp23B)for(0;_tmp225 != 0;_tmp225=_tmp225->tl){Cyc_Tcexp_tcExp(
_tmp228,0,(struct Cyc_Absyn_Exp*)_tmp225->hd);}else{if(_tmp23C == 0){const char*
_tmp812;void*_tmp811;(_tmp811=0,Cyc_Tcutil_terr(loc,((_tmp812="too many arguments for function",
_tag_dyneither(_tmp812,sizeof(char),32))),_tag_dyneither(_tmp811,sizeof(void*),0)));}
else{void*_tmp25B;int _tmp25C;struct Cyc_Absyn_VarargInfo _tmp25A=*_tmp23C;_tmp25B=
_tmp25A.type;_tmp25C=_tmp25A.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp813;
struct Cyc_Absyn_VarargCallInfo*_tmp25D=(_tmp813=_cycalloc(sizeof(*_tmp813)),((
_tmp813->num_varargs=0,((_tmp813->injectors=0,((_tmp813->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp23C,_tmp813)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp25D;if(!_tmp25C)for(0;_tmp225 != 0;(_tmp225=_tmp225->tl,_tmp226 ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp225->hd;++
_tmp25D->num_varargs;Cyc_Tcexp_tcExp(_tmp228,(void**)& _tmp25B,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp228,e1,_tmp25B,& alias_coercion)){{const char*_tmp818;void*_tmp817[2];struct
Cyc_String_pa_struct _tmp816;struct Cyc_String_pa_struct _tmp815;(_tmp815.tag=0,((
_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp816.tag=0,((
_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp25B)),((_tmp817[0]=& _tmp816,((_tmp817[1]=& _tmp815,Cyc_Tcutil_terr(loc,((
_tmp818="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp818,sizeof(char),49))),_tag_dyneither(_tmp817,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp819;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp819=_cycalloc(sizeof(*_tmp819)),((
_tmp819->hd=(void*)_tmp226,((_tmp819->tl=*alias_arg_exps,_tmp819))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
ter,_tmp25B) && !Cyc_Tcutil_is_noalias_path(ter,e1)){const char*_tmp81C;void*
_tmp81B;(_tmp81B=0,Cyc_Tcutil_terr(e1->loc,((_tmp81C="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp81C,sizeof(char),49))),_tag_dyneither(_tmp81B,sizeof(void*),0)));}}
else{void*_tmp265=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp25B));
struct Cyc_Absyn_DatatypeInfo _tmp267;union Cyc_Absyn_DatatypeInfoU _tmp268;struct
Cyc_Absyn_Datatypedecl**_tmp269;struct Cyc_Absyn_Datatypedecl*_tmp26A;struct Cyc_List_List*
_tmp26B;_LL13A: {struct Cyc_Absyn_DatatypeType_struct*_tmp266=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp265;if(_tmp266->tag != 3)goto _LL13C;else{_tmp267=_tmp266->f1;_tmp268=_tmp267.datatype_info;
if((_tmp268.KnownDatatype).tag != 2)goto _LL13C;_tmp269=(struct Cyc_Absyn_Datatypedecl**)(
_tmp268.KnownDatatype).val;_tmp26A=*_tmp269;_tmp26B=_tmp267.targs;}}_LL13B: {
struct Cyc_Absyn_Datatypedecl*_tmp26C=*Cyc_Tcenv_lookup_datatypedecl(_tmp228,loc,
_tmp26A->name);struct Cyc_List_List*fields=0;if(_tmp26C->fields == 0){const char*
_tmp820;void*_tmp81F[1];struct Cyc_String_pa_struct _tmp81E;(_tmp81E.tag=0,((
_tmp81E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp25B)),((_tmp81F[0]=& _tmp81E,Cyc_Tcutil_terr(loc,((_tmp820="can't inject into abstract datatype %s",
_tag_dyneither(_tmp820,sizeof(char),39))),_tag_dyneither(_tmp81F,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp26C->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp25B),Cyc_Tcenv_curr_rgn(
_tmp228))){const char*_tmp825;void*_tmp824[2];struct Cyc_String_pa_struct _tmp823;
struct Cyc_String_pa_struct _tmp822;(_tmp822.tag=0,((_tmp822.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp228))),((_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25B)),((_tmp824[0]=& _tmp823,((_tmp824[
1]=& _tmp822,Cyc_Tcutil_terr(loc,((_tmp825="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp825,sizeof(char),49))),_tag_dyneither(_tmp824,sizeof(void*),2)))))))))))));}{
struct _RegionHandle _tmp274=_new_region("rgn");struct _RegionHandle*rgn=& _tmp274;
_push_region(rgn);{struct Cyc_List_List*_tmp275=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp26C->tvs,_tmp26B);for(0;_tmp225 != 0;_tmp225=
_tmp225->tl){++ _tmp25D->num_varargs;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp225->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(_tmp228,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
rgn,e1)){const char*_tmp828;void*_tmp827;(_tmp827=0,Cyc_Tcutil_terr(e1->loc,((
_tmp828="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp828,sizeof(char),49))),_tag_dyneither(_tmp827,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp278=Cyc_Tcexp_tcInjection(_tmp228,e1,Cyc_Tcutil_pointer_elt_type(
_tmp25B),rgn,_tmp275,fields);if(_tmp278 != 0){struct Cyc_List_List*_tmp829;_tmp25D->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp25D->injectors,((_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829->hd=(struct
Cyc_Absyn_Datatypefield*)_tmp278,((_tmp829->tl=0,_tmp829)))))));}};};}}
_npop_handler(0);goto _LL139;;_pop_region(rgn);};}_LL13C:;_LL13D:{const char*
_tmp82C;void*_tmp82B;(_tmp82B=0,Cyc_Tcutil_terr(loc,((_tmp82C="bad inject vararg type",
_tag_dyneither(_tmp82C,sizeof(char),23))),_tag_dyneither(_tmp82B,sizeof(void*),0)));}
goto _LL139;_LL139:;}};}}}}Cyc_Tcenv_check_effect_accessible(_tmp228,loc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp237))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp228,loc,_tmp23D);{void*_tmp27D=_tmp239;_npop_handler(0);return _tmp27D;};};
_LL12A:;_LL12B: {const char*_tmp82F;void*_tmp82E;void*_tmp280=(_tmp82E=0,Cyc_Tcexp_expr_err(
_tmp228,loc,topt,((_tmp82F="expected pointer to function",_tag_dyneither(_tmp82F,
sizeof(char),29))),_tag_dyneither(_tmp82E,sizeof(void*),0)));_npop_handler(0);
return _tmp280;}_LL127:;};_LL125:;_LL126: {const char*_tmp832;void*_tmp831;void*
_tmp283=(_tmp831=0,Cyc_Tcexp_expr_err(_tmp228,loc,topt,((_tmp832="expected pointer to function",
_tag_dyneither(_tmp832,sizeof(char),29))),_tag_dyneither(_tmp831,sizeof(void*),0)));
_npop_handler(0);return _tmp283;}_LL122:;};};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){int
bogus=0;void*_tmp284=Cyc_Absyn_exn_typ();Cyc_Tcexp_tcExp(te,(void**)& _tmp284,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp284,& bogus)){const char*_tmp837;void*_tmp836[2];
struct Cyc_String_pa_struct _tmp835;struct Cyc_String_pa_struct _tmp834;(_tmp834.tag=
0,((_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp835.tag=0,((_tmp835.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp284)),((
_tmp836[0]=& _tmp835,((_tmp836[1]=& _tmp834,Cyc_Tcutil_terr(loc,((_tmp837="expected %s but found %s",
_tag_dyneither(_tmp837,sizeof(char),25))),_tag_dyneither(_tmp836,sizeof(void*),2)))))))))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(te,0,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp289=t1;struct Cyc_Absyn_PtrInfo _tmp28B;void*
_tmp28C;struct Cyc_Absyn_Tqual _tmp28D;struct Cyc_Absyn_PtrAtts _tmp28E;void*_tmp28F;
union Cyc_Absyn_Constraint*_tmp290;union Cyc_Absyn_Constraint*_tmp291;union Cyc_Absyn_Constraint*
_tmp292;_LL13F: {struct Cyc_Absyn_PointerType_struct*_tmp28A=(struct Cyc_Absyn_PointerType_struct*)
_tmp289;if(_tmp28A->tag != 5)goto _LL141;else{_tmp28B=_tmp28A->f1;_tmp28C=_tmp28B.elt_typ;
_tmp28D=_tmp28B.elt_tq;_tmp28E=_tmp28B.ptr_atts;_tmp28F=_tmp28E.rgn;_tmp290=
_tmp28E.nullable;_tmp291=_tmp28E.bounds;_tmp292=_tmp28E.zero_term;}}_LL140:{void*
_tmp293=Cyc_Tcutil_compress(_tmp28C);struct Cyc_Absyn_FnInfo _tmp295;struct Cyc_List_List*
_tmp296;struct Cyc_Core_Opt*_tmp297;struct Cyc_Absyn_Tqual _tmp298;void*_tmp299;
struct Cyc_List_List*_tmp29A;int _tmp29B;struct Cyc_Absyn_VarargInfo*_tmp29C;struct
Cyc_List_List*_tmp29D;struct Cyc_List_List*_tmp29E;_LL144: {struct Cyc_Absyn_FnType_struct*
_tmp294=(struct Cyc_Absyn_FnType_struct*)_tmp293;if(_tmp294->tag != 9)goto _LL146;
else{_tmp295=_tmp294->f1;_tmp296=_tmp295.tvars;_tmp297=_tmp295.effect;_tmp298=
_tmp295.ret_tqual;_tmp299=_tmp295.ret_typ;_tmp29A=_tmp295.args;_tmp29B=_tmp295.c_varargs;
_tmp29C=_tmp295.cyc_varargs;_tmp29D=_tmp295.rgn_po;_tmp29E=_tmp295.attributes;}}
_LL145: {struct _RegionHandle _tmp29F=_new_region("temp");struct _RegionHandle*temp=&
_tmp29F;_push_region(temp);{struct Cyc_List_List*instantiation=0;for(0;ts != 0  && 
_tmp296 != 0;(ts=ts->tl,_tmp296=_tmp296->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp296->hd,& Cyc_Tcutil_bk);Cyc_Tcutil_check_type(loc,te,
Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);Cyc_Tcutil_check_no_qual(loc,(
void*)ts->hd);{struct _tuple13*_tmp83A;struct Cyc_List_List*_tmp839;instantiation=((
_tmp839=_region_malloc(temp,sizeof(*_tmp839)),((_tmp839->hd=((_tmp83A=
_region_malloc(temp,sizeof(*_tmp83A)),((_tmp83A->f1=(struct Cyc_Absyn_Tvar*)
_tmp296->hd,((_tmp83A->f2=(void*)ts->hd,_tmp83A)))))),((_tmp839->tl=
instantiation,_tmp839))))));};}if(ts != 0){const char*_tmp83D;void*_tmp83C;void*
_tmp2A4=(_tmp83C=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp83D="too many type variables in instantiation",
_tag_dyneither(_tmp83D,sizeof(char),41))),_tag_dyneither(_tmp83C,sizeof(void*),0)));
_npop_handler(0);return _tmp2A4;}{struct Cyc_Absyn_FnType_struct _tmp843;struct Cyc_Absyn_FnInfo
_tmp842;struct Cyc_Absyn_FnType_struct*_tmp841;void*new_fn_typ=Cyc_Tcutil_rsubstitute(
temp,instantiation,(void*)((_tmp841=_cycalloc(sizeof(*_tmp841)),((_tmp841[0]=((
_tmp843.tag=9,((_tmp843.f1=((_tmp842.tvars=_tmp296,((_tmp842.effect=_tmp297,((
_tmp842.ret_tqual=_tmp298,((_tmp842.ret_typ=_tmp299,((_tmp842.args=_tmp29A,((
_tmp842.c_varargs=_tmp29B,((_tmp842.cyc_varargs=_tmp29C,((_tmp842.rgn_po=_tmp29D,((
_tmp842.attributes=_tmp29E,_tmp842)))))))))))))))))),_tmp843)))),_tmp841)))));
struct Cyc_Absyn_PointerType_struct _tmp84D;struct Cyc_Absyn_PtrAtts _tmp84C;struct
Cyc_Absyn_PtrInfo _tmp84B;struct Cyc_Absyn_PointerType_struct*_tmp84A;void*_tmp2A9=(
void*)((_tmp84A=_cycalloc(sizeof(*_tmp84A)),((_tmp84A[0]=((_tmp84D.tag=5,((
_tmp84D.f1=((_tmp84B.elt_typ=new_fn_typ,((_tmp84B.elt_tq=_tmp28D,((_tmp84B.ptr_atts=((
_tmp84C.rgn=_tmp28F,((_tmp84C.nullable=_tmp290,((_tmp84C.bounds=_tmp291,((
_tmp84C.zero_term=_tmp292,((_tmp84C.ptrloc=0,_tmp84C)))))))))),_tmp84B)))))),
_tmp84D)))),_tmp84A))));_npop_handler(0);return _tmp2A9;};};_pop_region(temp);}
_LL146:;_LL147: goto _LL143;_LL143:;}goto _LL13E;_LL141:;_LL142: goto _LL13E;_LL13E:;}{
const char*_tmp851;void*_tmp850[1];struct Cyc_String_pa_struct _tmp84F;return(
_tmp84F.tag=0,((_tmp84F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp850[0]=& _tmp84F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp851="expecting polymorphic type but found %s",
_tag_dyneither(_tmp851,sizeof(char),40))),_tag_dyneither(_tmp850,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,
void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(
loc,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(
te,loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp856;void*_tmp855[2];struct Cyc_String_pa_struct _tmp854;struct Cyc_String_pa_struct
_tmp853;void*_tmp2B0=(_tmp853.tag=0,((_tmp853.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp854.tag=0,((_tmp854.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp855[0]=&
_tmp854,((_tmp855[1]=& _tmp853,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp856="cannot cast %s to %s",
_tag_dyneither(_tmp856,sizeof(char),21))),_tag_dyneither(_tmp855,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp2B0;};}}}{struct _tuple0 _tmp857;struct
_tuple0 _tmp2B6=(_tmp857.f1=e->r,((_tmp857.f2=Cyc_Tcutil_compress(t),_tmp857)));
void*_tmp2B7;struct Cyc_Absyn_MallocInfo _tmp2B9;int _tmp2BA;void*_tmp2BB;struct Cyc_Absyn_PtrInfo
_tmp2BD;struct Cyc_Absyn_PtrAtts _tmp2BE;union Cyc_Absyn_Constraint*_tmp2BF;union
Cyc_Absyn_Constraint*_tmp2C0;union Cyc_Absyn_Constraint*_tmp2C1;_LL149: _tmp2B7=
_tmp2B6.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp2B8=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp2B7;if(_tmp2B8->tag != 34)goto _LL14B;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.fat_result;}};
_tmp2BB=_tmp2B6.f2;{struct Cyc_Absyn_PointerType_struct*_tmp2BC=(struct Cyc_Absyn_PointerType_struct*)
_tmp2BB;if(_tmp2BC->tag != 5)goto _LL14B;else{_tmp2BD=_tmp2BC->f1;_tmp2BE=_tmp2BD.ptr_atts;
_tmp2BF=_tmp2BE.nullable;_tmp2C0=_tmp2BE.bounds;_tmp2C1=_tmp2BE.zero_term;}};
_LL14A: if((_tmp2BA  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2C1)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2BF)){void*_tmp2C2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp2C0);struct Cyc_Absyn_Exp*_tmp2C4;_LL14E: {struct Cyc_Absyn_Upper_b_struct*
_tmp2C3=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C2;if(_tmp2C3->tag != 1)goto _LL150;
else{_tmp2C4=_tmp2C3->f1;}}_LL14F: if((Cyc_Evexp_eval_const_uint_exp(_tmp2C4)).f1
== 1){const char*_tmp85A;void*_tmp859;(_tmp859=0,Cyc_Tcutil_warn(loc,((_tmp85A="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp85A,sizeof(char),60))),_tag_dyneither(_tmp859,sizeof(void*),0)));}
goto _LL14D;_LL150:;_LL151: goto _LL14D;_LL14D:;}goto _LL148;_LL14B:;_LL14C: goto
_LL148;_LL148:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**
_tmp2C7=0;struct Cyc_Absyn_Tqual _tmp2C8=Cyc_Absyn_empty_tqual(0);if(topt != 0){
void*_tmp2C9=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2CB;void*
_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CD;struct Cyc_Absyn_PtrAtts _tmp2CE;union Cyc_Absyn_Constraint*
_tmp2CF;_LL153: {struct Cyc_Absyn_PointerType_struct*_tmp2CA=(struct Cyc_Absyn_PointerType_struct*)
_tmp2C9;if(_tmp2CA->tag != 5)goto _LL155;else{_tmp2CB=_tmp2CA->f1;_tmp2CC=_tmp2CB.elt_typ;
_tmp2CD=_tmp2CB.elt_tq;_tmp2CE=_tmp2CB.ptr_atts;_tmp2CF=_tmp2CE.zero_term;}}
_LL154:{void**_tmp85B;_tmp2C7=((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=
_tmp2CC,_tmp85B))));}_tmp2C8=_tmp2CD;goto _LL152;_LL155:;_LL156: goto _LL152;_LL152:;}{
struct _RegionHandle _tmp2D1=_new_region("r");struct _RegionHandle*r=& _tmp2D1;
_push_region(r);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(
r,te)),_tmp2C7,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){const char*_tmp85E;void*_tmp85D;(_tmp85D=0,Cyc_Tcutil_terr(
e->loc,((_tmp85E="Cannot take the address of an alias-free path",_tag_dyneither(
_tmp85E,sizeof(char),46))),_tag_dyneither(_tmp85D,sizeof(void*),0)));}{void*
_tmp2D4=e->r;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D7;_LL158: {
struct Cyc_Absyn_Subscript_e_struct*_tmp2D5=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp2D4;if(_tmp2D5->tag != 24)goto _LL15A;else{_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}
_LL159:{void*_tmp2D8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2D6->topt))->v);_LL15D: {struct Cyc_Absyn_TupleType_struct*
_tmp2D9=(struct Cyc_Absyn_TupleType_struct*)_tmp2D8;if(_tmp2D9->tag != 10)goto
_LL15F;}_LL15E: goto _LL15C;_LL15F:;_LL160: e0->r=(Cyc_Absyn_add_exp(_tmp2D6,
_tmp2D7,0))->r;{void*_tmp2DA=Cyc_Tcexp_tcPlus(te,_tmp2D6,_tmp2D7);_npop_handler(
0);return _tmp2DA;};_LL15C:;}goto _LL157;_LL15A:;_LL15B: goto _LL157;_LL157:;}{void*
_tmp2DB=e->r;int _tmp2DD;int _tmp2DF;_LL162: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp2DC=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp2DB;if(_tmp2DC->tag != 22)goto
_LL164;else{_tmp2DD=_tmp2DC->f3;if(_tmp2DD != 1)goto _LL164;}}_LL163: goto _LL165;
_LL164: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp2DE=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp2DB;if(_tmp2DE->tag != 23)goto _LL166;else{_tmp2DF=_tmp2DE->f3;if(_tmp2DF != 1)
goto _LL166;}}_LL165:{const char*_tmp861;void*_tmp860;(_tmp860=0,Cyc_Tcutil_terr(e->loc,((
_tmp861="cannot take the address of a @tagged union member",_tag_dyneither(
_tmp861,sizeof(char),50))),_tag_dyneither(_tmp860,sizeof(void*),0)));}goto _LL161;
_LL166:;_LL167: goto _LL161;_LL161:;}{int _tmp2E3;void*_tmp2E4;struct _tuple14
_tmp2E2=Cyc_Tcutil_addressof_props(te,e);_tmp2E3=_tmp2E2.f1;_tmp2E4=_tmp2E2.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2E4,0)){const char*_tmp864;void*_tmp863;(
_tmp863=0,Cyc_Tcutil_terr(e->loc,((_tmp864="using & would manufacture an alias to an alias-free pointer",
_tag_dyneither(_tmp864,sizeof(char),60))),_tag_dyneither(_tmp863,sizeof(void*),0)));}{
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2E3){tq.print_const=1;tq.real_const=
1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp2E4,tq,Cyc_Absyn_false_conref);void*_tmp2E7=t;_npop_handler(0);return _tmp2E7;};};};;
_pop_region(r);};}static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);if(!Cyc_Evexp_okay_szofarg(t)){const char*_tmp868;
void*_tmp867[1];struct Cyc_String_pa_struct _tmp866;(_tmp866.tag=0,((_tmp866.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp867[
0]=& _tmp866,Cyc_Tcutil_terr(loc,((_tmp868="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp868,sizeof(char),55))),_tag_dyneither(_tmp867,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2EB=Cyc_Tcutil_compress(*topt);void*_tmp2ED;_LL169: {
struct Cyc_Absyn_TagType_struct*_tmp2EC=(struct Cyc_Absyn_TagType_struct*)_tmp2EB;
if(_tmp2EC->tag != 19)goto _LL16B;else{_tmp2ED=(void*)_tmp2EC->f1;}}_LL16A: {
struct Cyc_Absyn_Exp*_tmp2EE=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp86B;struct Cyc_Absyn_ValueofType_struct*_tmp86A;struct Cyc_Absyn_ValueofType_struct*
_tmp2EF=(_tmp86A=_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86B.tag=18,((
_tmp86B.f1=_tmp2EE,_tmp86B)))),_tmp86A)));if(Cyc_Tcutil_unify(_tmp2ED,(void*)
_tmp2EF))return _tmp2EB;goto _LL168;}_LL16B:;_LL16C: goto _LL168;_LL168:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),&
Cyc_Tcutil_tmk,1,t);Cyc_Tcutil_check_no_qual(loc,t);{void*_tmp2F2=n;struct
_dyneither_ptr*_tmp2F4;unsigned int _tmp2F6;_LL16E: {struct Cyc_Absyn_StructField_struct*
_tmp2F3=(struct Cyc_Absyn_StructField_struct*)_tmp2F2;if(_tmp2F3->tag != 0)goto
_LL170;else{_tmp2F4=_tmp2F3->f1;}}_LL16F: {int bad_type=1;{void*_tmp2F7=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp2F9;union Cyc_Absyn_AggrInfoU _tmp2FA;struct Cyc_Absyn_Aggrdecl**
_tmp2FB;struct Cyc_List_List*_tmp2FD;_LL173: {struct Cyc_Absyn_AggrType_struct*
_tmp2F8=(struct Cyc_Absyn_AggrType_struct*)_tmp2F7;if(_tmp2F8->tag != 11)goto
_LL175;else{_tmp2F9=_tmp2F8->f1;_tmp2FA=_tmp2F9.aggr_info;if((_tmp2FA.KnownAggr).tag
!= 2)goto _LL175;_tmp2FB=(struct Cyc_Absyn_Aggrdecl**)(_tmp2FA.KnownAggr).val;}}
_LL174: if((*_tmp2FB)->impl == 0)goto _LL172;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2F4,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2FB)->impl))->fields)){const char*_tmp86F;void*_tmp86E[1];
struct Cyc_String_pa_struct _tmp86D;(_tmp86D.tag=0,((_tmp86D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2F4),((_tmp86E[0]=& _tmp86D,Cyc_Tcutil_terr(
loc,((_tmp86F="no field of struct/union has name %s",_tag_dyneither(_tmp86F,
sizeof(char),37))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))));}bad_type=0;
goto _LL172;_LL175: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2FC=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2F7;if(_tmp2FC->tag != 12)goto _LL177;else{_tmp2FD=_tmp2FC->f2;}}_LL176: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2F4,_tmp2FD)){const char*_tmp873;void*_tmp872[1];struct Cyc_String_pa_struct
_tmp871;(_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2F4),((_tmp872[0]=& _tmp871,Cyc_Tcutil_terr(loc,((_tmp873="no field of struct/union has name %s",
_tag_dyneither(_tmp873,sizeof(char),37))),_tag_dyneither(_tmp872,sizeof(void*),1)))))));}
bad_type=0;goto _LL172;_LL177:;_LL178: goto _LL172;_LL172:;}if(bad_type){const char*
_tmp877;void*_tmp876[1];struct Cyc_String_pa_struct _tmp875;(_tmp875.tag=0,((
_tmp875.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp876[0]=& _tmp875,Cyc_Tcutil_terr(loc,((_tmp877="%s is not a known struct/union type",
_tag_dyneither(_tmp877,sizeof(char),36))),_tag_dyneither(_tmp876,sizeof(void*),1)))))));}
goto _LL16D;}_LL170: {struct Cyc_Absyn_TupleIndex_struct*_tmp2F5=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2F2;if(_tmp2F5->tag != 1)goto _LL16D;else{_tmp2F6=_tmp2F5->f1;}}_LL171: {int
bad_type=1;{void*_tmp307=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp309;
union Cyc_Absyn_AggrInfoU _tmp30A;struct Cyc_Absyn_Aggrdecl**_tmp30B;struct Cyc_List_List*
_tmp30D;struct Cyc_List_List*_tmp30F;struct Cyc_Absyn_DatatypeFieldInfo _tmp311;
union Cyc_Absyn_DatatypeFieldInfoU _tmp312;struct _tuple2 _tmp313;struct Cyc_Absyn_Datatypefield*
_tmp314;_LL17A: {struct Cyc_Absyn_AggrType_struct*_tmp308=(struct Cyc_Absyn_AggrType_struct*)
_tmp307;if(_tmp308->tag != 11)goto _LL17C;else{_tmp309=_tmp308->f1;_tmp30A=_tmp309.aggr_info;
if((_tmp30A.KnownAggr).tag != 2)goto _LL17C;_tmp30B=(struct Cyc_Absyn_Aggrdecl**)(
_tmp30A.KnownAggr).val;}}_LL17B: if((*_tmp30B)->impl == 0)goto _LL179;_tmp30D=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp30B)->impl))->fields;goto _LL17D;
_LL17C: {struct Cyc_Absyn_AnonAggrType_struct*_tmp30C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp307;if(_tmp30C->tag != 12)goto _LL17E;else{_tmp30D=_tmp30C->f2;}}_LL17D: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30D)<= _tmp2F6){const char*
_tmp87C;void*_tmp87B[2];struct Cyc_Int_pa_struct _tmp87A;struct Cyc_Int_pa_struct
_tmp879;(_tmp879.tag=1,((_tmp879.f1=(unsigned long)((int)_tmp2F6),((_tmp87A.tag=
1,((_tmp87A.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp30D),((_tmp87B[0]=& _tmp87A,((_tmp87B[1]=& _tmp879,Cyc_Tcutil_terr(loc,((
_tmp87C="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp87C,
sizeof(char),46))),_tag_dyneither(_tmp87B,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL179;_LL17E: {struct Cyc_Absyn_TupleType_struct*_tmp30E=(struct Cyc_Absyn_TupleType_struct*)
_tmp307;if(_tmp30E->tag != 10)goto _LL180;else{_tmp30F=_tmp30E->f1;}}_LL17F: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F)<= _tmp2F6){const char*
_tmp881;void*_tmp880[2];struct Cyc_Int_pa_struct _tmp87F;struct Cyc_Int_pa_struct
_tmp87E;(_tmp87E.tag=1,((_tmp87E.f1=(unsigned long)((int)_tmp2F6),((_tmp87F.tag=
1,((_tmp87F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp30F),((_tmp880[0]=& _tmp87F,((_tmp880[1]=& _tmp87E,Cyc_Tcutil_terr(loc,((
_tmp881="tuple has too few components: %d <= %d",_tag_dyneither(_tmp881,sizeof(
char),39))),_tag_dyneither(_tmp880,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL179;_LL180: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp310=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp307;if(_tmp310->tag != 4)goto _LL182;else{_tmp311=_tmp310->f1;_tmp312=_tmp311.field_info;
if((_tmp312.KnownDatatypefield).tag != 2)goto _LL182;_tmp313=(struct _tuple2)(
_tmp312.KnownDatatypefield).val;_tmp314=_tmp313.f2;}}_LL181: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp314->typs)< _tmp2F6){const char*_tmp886;
void*_tmp885[2];struct Cyc_Int_pa_struct _tmp884;struct Cyc_Int_pa_struct _tmp883;(
_tmp883.tag=1,((_tmp883.f1=(unsigned long)((int)_tmp2F6),((_tmp884.tag=1,((
_tmp884.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp314->typs),((_tmp885[0]=& _tmp884,((_tmp885[1]=& _tmp883,Cyc_Tcutil_terr(loc,((
_tmp886="datatype field has too few components: %d < %d",_tag_dyneither(_tmp886,
sizeof(char),47))),_tag_dyneither(_tmp885,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL179;_LL182:;_LL183: goto _LL179;_LL179:;}if(bad_type){const char*_tmp88A;
void*_tmp889[1];struct Cyc_String_pa_struct _tmp888;(_tmp888.tag=0,((_tmp888.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp889[
0]=& _tmp888,Cyc_Tcutil_terr(loc,((_tmp88A="%s is not a known type",
_tag_dyneither(_tmp88A,sizeof(char),23))),_tag_dyneither(_tmp889,sizeof(void*),1)))))));}
goto _LL16D;}_LL16D:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp324=_new_region("r");struct _RegionHandle*r=& _tmp324;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp325=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(
r,te_orig));Cyc_Tcexp_tcExp(_tmp325,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp326=t;struct Cyc_Absyn_PtrInfo
_tmp329;void*_tmp32A;struct Cyc_Absyn_PtrAtts _tmp32B;void*_tmp32C;union Cyc_Absyn_Constraint*
_tmp32D;union Cyc_Absyn_Constraint*_tmp32E;_LL185: {struct Cyc_Absyn_Evar_struct*
_tmp327=(struct Cyc_Absyn_Evar_struct*)_tmp326;if(_tmp327->tag != 1)goto _LL187;}
_LL186: {struct Cyc_List_List*_tmp32F=Cyc_Tcenv_lookup_type_vars(_tmp325);struct
Cyc_Core_Opt*_tmp88B;void*_tmp330=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((
_tmp88B=_cycalloc(sizeof(*_tmp88B)),((_tmp88B->v=_tmp32F,_tmp88B)))));struct Cyc_Core_Opt*
_tmp88C;void*_tmp331=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C->v=_tmp32F,_tmp88C)))));union Cyc_Absyn_Constraint*
_tmp332=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*
_tmp333=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts
_tmp88D;struct Cyc_Absyn_PtrAtts _tmp334=(_tmp88D.rgn=_tmp331,((_tmp88D.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp88D.bounds=
_tmp332,((_tmp88D.zero_term=_tmp333,((_tmp88D.ptrloc=0,_tmp88D)))))))));struct
Cyc_Absyn_PointerType_struct _tmp893;struct Cyc_Absyn_PtrInfo _tmp892;struct Cyc_Absyn_PointerType_struct*
_tmp891;struct Cyc_Absyn_PointerType_struct*_tmp335=(_tmp891=_cycalloc(sizeof(*
_tmp891)),((_tmp891[0]=((_tmp893.tag=5,((_tmp893.f1=((_tmp892.elt_typ=_tmp330,((
_tmp892.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp892.ptr_atts=_tmp334,_tmp892)))))),
_tmp893)))),_tmp891)));Cyc_Tcutil_unify(t,(void*)_tmp335);_tmp32A=_tmp330;
_tmp32C=_tmp331;_tmp32D=_tmp332;_tmp32E=_tmp333;goto _LL188;}_LL187: {struct Cyc_Absyn_PointerType_struct*
_tmp328=(struct Cyc_Absyn_PointerType_struct*)_tmp326;if(_tmp328->tag != 5)goto
_LL189;else{_tmp329=_tmp328->f1;_tmp32A=_tmp329.elt_typ;_tmp32B=_tmp329.ptr_atts;
_tmp32C=_tmp32B.rgn;_tmp32D=_tmp32B.bounds;_tmp32E=_tmp32B.zero_term;}}_LL188:
Cyc_Tcenv_check_rgn_accessible(_tmp325,loc,_tmp32C);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp32D);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp32A),& Cyc_Tcutil_tmk)){
const char*_tmp896;void*_tmp895;(_tmp895=0,Cyc_Tcutil_terr(loc,((_tmp896="can't dereference abstract pointer type",
_tag_dyneither(_tmp896,sizeof(char),40))),_tag_dyneither(_tmp895,sizeof(void*),0)));}{
void*_tmp33E=_tmp32A;_npop_handler(0);return _tmp33E;};_LL189:;_LL18A: {const char*
_tmp89A;void*_tmp899[1];struct Cyc_String_pa_struct _tmp898;void*_tmp342=(_tmp898.tag=
0,((_tmp898.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp899[0]=& _tmp898,Cyc_Tcexp_expr_err(_tmp325,loc,topt,((_tmp89A="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp89A,sizeof(char),39))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));
_npop_handler(0);return _tmp342;}_LL184:;};};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){struct
_RegionHandle _tmp343=_new_region("r");struct _RegionHandle*r=& _tmp343;
_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp344=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp346;
union Cyc_Absyn_AggrInfoU _tmp347;struct Cyc_Absyn_Aggrdecl**_tmp348;struct Cyc_Absyn_Aggrdecl*
_tmp349;struct Cyc_List_List*_tmp34A;enum Cyc_Absyn_AggrKind _tmp34C;struct Cyc_List_List*
_tmp34D;_LL18C: {struct Cyc_Absyn_AggrType_struct*_tmp345=(struct Cyc_Absyn_AggrType_struct*)
_tmp344;if(_tmp345->tag != 11)goto _LL18E;else{_tmp346=_tmp345->f1;_tmp347=_tmp346.aggr_info;
if((_tmp347.KnownAggr).tag != 2)goto _LL18E;_tmp348=(struct Cyc_Absyn_Aggrdecl**)(
_tmp347.KnownAggr).val;_tmp349=*_tmp348;_tmp34A=_tmp346.targs;}}_LL18D: {struct
Cyc_Absyn_Aggrfield*_tmp352=Cyc_Absyn_lookup_decl_field(_tmp349,f);if(_tmp352 == 
0){const char*_tmp89F;void*_tmp89E[2];struct Cyc_String_pa_struct _tmp89D;struct Cyc_String_pa_struct
_tmp89C;void*_tmp357=(_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp89D.tag=0,((_tmp89D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp349->name)),((_tmp89E[0]=& _tmp89D,((
_tmp89E[1]=& _tmp89C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89F="type %s has no %s field",
_tag_dyneither(_tmp89F,sizeof(char),24))),_tag_dyneither(_tmp89E,sizeof(void*),2)))))))))))));
_npop_handler(0);return _tmp357;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp349->impl))->tagged)*is_tagged=1;{void*t2;struct _RegionHandle _tmp358=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp358;_push_region(rgn);{struct Cyc_List_List*
_tmp359=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp349->tvs,
_tmp34A);t2=Cyc_Tcutil_rsubstitute(rgn,_tmp359,_tmp352->type);if(((_tmp349->kind
== Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp349->impl))->tagged)
 && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){const char*
_tmp8A3;void*_tmp8A2[1];struct Cyc_String_pa_struct _tmp8A1;void*_tmp35D=(_tmp8A1.tag=
0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A2[0]=&
_tmp8A1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A3="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp8A3,sizeof(char),56))),_tag_dyneither(_tmp8A2,sizeof(void*),1)))))));
_npop_handler(1);return _tmp35D;}}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp349->impl))->exist_vars != 0){if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te)))){const char*_tmp8A7;void*_tmp8A6[1];struct Cyc_String_pa_struct _tmp8A5;void*
_tmp361=(_tmp8A5.tag=0,((_tmp8A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp8A6[0]=& _tmp8A5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A7="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp8A7,sizeof(char),81))),_tag_dyneither(_tmp8A6,sizeof(void*),1)))))));
_npop_handler(1);return _tmp361;}}{void*_tmp362=t2;_npop_handler(1);return _tmp362;};;
_pop_region(rgn);};}_LL18E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp34B=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp344;if(_tmp34B->tag != 12)goto _LL190;else{
_tmp34C=_tmp34B->f1;_tmp34D=_tmp34B->f2;}}_LL18F: {struct Cyc_Absyn_Aggrfield*
_tmp363=Cyc_Absyn_lookup_field(_tmp34D,f);if(_tmp363 == 0){const char*_tmp8AB;void*
_tmp8AA[1];struct Cyc_String_pa_struct _tmp8A9;void*_tmp367=(_tmp8A9.tag=0,((
_tmp8A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AA[0]=&
_tmp8A9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AB="type has no %s field",
_tag_dyneither(_tmp8AB,sizeof(char),21))),_tag_dyneither(_tmp8AA,sizeof(void*),1)))))));
_npop_handler(0);return _tmp367;}if((_tmp34C == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(_tmp363->type)){const char*_tmp8AF;void*_tmp8AE[1];
struct Cyc_String_pa_struct _tmp8AD;void*_tmp36B=(_tmp8AD.tag=0,((_tmp8AD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AE[0]=& _tmp8AD,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp8AF="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp8AF,sizeof(char),56))),_tag_dyneither(_tmp8AE,sizeof(void*),1)))))));
_npop_handler(0);return _tmp36B;}{void*_tmp36C=_tmp363->type;_npop_handler(0);
return _tmp36C;};}_LL190:{struct Cyc_Absyn_ArrayType_struct*_tmp34E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp344;if(_tmp34E->tag != 8)goto _LL192;}{const char*_tmp8B0;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp8B0="size",_tag_dyneither(_tmp8B0,sizeof(char),5))))
== 0))goto _LL192;};_LL191: goto _LL193;_LL192:{struct Cyc_Absyn_PointerType_struct*
_tmp350=(struct Cyc_Absyn_PointerType_struct*)_tmp344;if(_tmp350->tag != 5)goto
_LL194;}{const char*_tmp8B1;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8B1="size",
_tag_dyneither(_tmp8B1,sizeof(char),5))))== 0))goto _LL194;};_LL193:{const char*
_tmp8B5;void*_tmp8B4[1];struct Cyc_String_pa_struct _tmp8B3;(_tmp8B3.tag=0,((
_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp8B4[0]=& _tmp8B3,Cyc_Tcutil_warn(e->loc,((_tmp8B5="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp8B5,sizeof(char),54))),_tag_dyneither(_tmp8B4,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp8BB;struct Cyc_List_List*_tmp8BA;struct Cyc_Absyn_Primop_e_struct*
_tmp8B9;outer_e->r=(void*)((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9[0]=((
_tmp8BB.tag=3,((_tmp8BB.f1=Cyc_Absyn_Numelts,((_tmp8BB.f2=((_tmp8BA=_cycalloc(
sizeof(*_tmp8BA)),((_tmp8BA->hd=e,((_tmp8BA->tl=0,_tmp8BA)))))),_tmp8BB)))))),
_tmp8B9))));}{void*_tmp373=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp373;};
_LL194:;_LL195: {const char*_tmp8BC;if(Cyc_strcmp((struct _dyneither_ptr)*f,((
_tmp8BC="size",_tag_dyneither(_tmp8BC,sizeof(char),5))))== 0){const char*_tmp8C0;
void*_tmp8BF[1];struct Cyc_String_pa_struct _tmp8BE;void*_tmp378=(_tmp8BE.tag=0,((
_tmp8BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp8BF[0]=& _tmp8BE,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp8C0="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp8C0,sizeof(char),44))),_tag_dyneither(_tmp8BF,sizeof(void*),1)))))));
_npop_handler(0);return _tmp378;}else{const char*_tmp8C4;void*_tmp8C3[1];struct Cyc_String_pa_struct
_tmp8C2;void*_tmp37C=(_tmp8C2.tag=0,((_tmp8C2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp8C3[0]=& _tmp8C2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C4="expecting struct or union, found %s",
_tag_dyneither(_tmp8C4,sizeof(char),36))),_tag_dyneither(_tmp8C3,sizeof(void*),1)))))));
_npop_handler(0);return _tmp37C;}}_LL18B:;};;_pop_region(r);}static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle _tmp37D=
_new_region("r");struct _RegionHandle*r=& _tmp37D;_push_region(r);Cyc_Tcexp_tcExp(
Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp37E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp380;void*_tmp381;struct Cyc_Absyn_PtrAtts _tmp382;void*
_tmp383;union Cyc_Absyn_Constraint*_tmp384;union Cyc_Absyn_Constraint*_tmp385;
_LL197: {struct Cyc_Absyn_PointerType_struct*_tmp37F=(struct Cyc_Absyn_PointerType_struct*)
_tmp37E;if(_tmp37F->tag != 5)goto _LL199;else{_tmp380=_tmp37F->f1;_tmp381=_tmp380.elt_typ;
_tmp382=_tmp380.ptr_atts;_tmp383=_tmp382.rgn;_tmp384=_tmp382.bounds;_tmp385=
_tmp382.zero_term;}}_LL198: Cyc_Tcutil_check_nonzero_bound(loc,_tmp384);{void*
_tmp386=Cyc_Tcutil_compress(_tmp381);struct Cyc_Absyn_AggrInfo _tmp388;union Cyc_Absyn_AggrInfoU
_tmp389;struct Cyc_Absyn_Aggrdecl**_tmp38A;struct Cyc_Absyn_Aggrdecl*_tmp38B;
struct Cyc_List_List*_tmp38C;enum Cyc_Absyn_AggrKind _tmp38E;struct Cyc_List_List*
_tmp38F;_LL19C: {struct Cyc_Absyn_AggrType_struct*_tmp387=(struct Cyc_Absyn_AggrType_struct*)
_tmp386;if(_tmp387->tag != 11)goto _LL19E;else{_tmp388=_tmp387->f1;_tmp389=_tmp388.aggr_info;
if((_tmp389.KnownAggr).tag != 2)goto _LL19E;_tmp38A=(struct Cyc_Absyn_Aggrdecl**)(
_tmp389.KnownAggr).val;_tmp38B=*_tmp38A;_tmp38C=_tmp388.targs;}}_LL19D: {struct
Cyc_Absyn_Aggrfield*_tmp390=Cyc_Absyn_lookup_decl_field(_tmp38B,f);if(_tmp390 == 
0){const char*_tmp8C9;void*_tmp8C8[2];struct Cyc_String_pa_struct _tmp8C7;struct Cyc_String_pa_struct
_tmp8C6;void*_tmp395=(_tmp8C6.tag=0,((_tmp8C6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8C7.tag=0,((_tmp8C7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp38B->name)),((_tmp8C8[0]=& _tmp8C7,((
_tmp8C8[1]=& _tmp8C6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C9="type %s has no %s field",
_tag_dyneither(_tmp8C9,sizeof(char),24))),_tag_dyneither(_tmp8C8,sizeof(void*),2)))))))))))));
_npop_handler(0);return _tmp395;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp38B->impl))->tagged)*is_tagged=1;{void*t3;struct _RegionHandle _tmp396=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp396;_push_region(rgn);{struct Cyc_List_List*
_tmp397=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp38B->tvs,
_tmp38C);t3=Cyc_Tcutil_rsubstitute(rgn,_tmp397,_tmp390->type);}{struct Cyc_Absyn_Kind*
_tmp398=Cyc_Tcutil_typ_kind(t3);if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp398)){
const char*_tmp8CD;void*_tmp8CC[1];struct Cyc_String_pa_struct _tmp8CB;void*_tmp39C=(
_tmp8CB.tag=0,((_tmp8CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp8CC[0]=& _tmp8CB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CD="cannot get member %s since its type is abstract",
_tag_dyneither(_tmp8CD,sizeof(char),48))),_tag_dyneither(_tmp8CC,sizeof(void*),1)))))));
_npop_handler(1);return _tmp39C;}if(((_tmp38B->kind == Cyc_Absyn_UnionA  && !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp38B->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t3)){const char*_tmp8D1;void*_tmp8D0[1];struct Cyc_String_pa_struct
_tmp8CF;void*_tmp3A0=(_tmp8CF.tag=0,((_tmp8CF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp8D0[0]=& _tmp8CF,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp8D1="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp8D1,sizeof(char),56))),_tag_dyneither(_tmp8D0,sizeof(void*),1)))))));
_npop_handler(1);return _tmp3A0;}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp38B->impl))->exist_vars != 0){if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te)))){const char*_tmp8D5;void*_tmp8D4[1];struct Cyc_String_pa_struct _tmp8D3;void*
_tmp3A4=(_tmp8D3.tag=0,((_tmp8D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp8D4[0]=& _tmp8D3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D5="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp8D5,sizeof(char),72))),_tag_dyneither(_tmp8D4,sizeof(void*),1)))))));
_npop_handler(1);return _tmp3A4;}}{void*_tmp3A5=t3;_npop_handler(1);return _tmp3A5;};};;
_pop_region(rgn);};}_LL19E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp38D=(struct
Cyc_Absyn_AnonAggrType_struct*)_tmp386;if(_tmp38D->tag != 12)goto _LL1A0;else{
_tmp38E=_tmp38D->f1;_tmp38F=_tmp38D->f2;}}_LL19F: {struct Cyc_Absyn_Aggrfield*
_tmp3A6=Cyc_Absyn_lookup_field(_tmp38F,f);if(_tmp3A6 == 0){const char*_tmp8D9;void*
_tmp8D8[1];struct Cyc_String_pa_struct _tmp8D7;void*_tmp3AA=(_tmp8D7.tag=0,((
_tmp8D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D8[0]=&
_tmp8D7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D9="type has no %s field",
_tag_dyneither(_tmp8D9,sizeof(char),21))),_tag_dyneither(_tmp8D8,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3AA;}if((_tmp38E == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(_tmp3A6->type)){const char*_tmp8DD;void*_tmp8DC[1];
struct Cyc_String_pa_struct _tmp8DB;void*_tmp3AE=(_tmp8DB.tag=0,((_tmp8DB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DC[0]=& _tmp8DB,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp8DD="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp8DD,sizeof(char),56))),_tag_dyneither(_tmp8DC,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3AE;}{void*_tmp3AF=_tmp3A6->type;_npop_handler(0);
return _tmp3AF;};}_LL1A0:;_LL1A1: goto _LL19B;_LL19B:;}goto _LL196;_LL199:;_LL19A:
goto _LL196;_LL196:;}{const char*_tmp8E1;void*_tmp8E0[1];struct Cyc_String_pa_struct
_tmp8DF;void*_tmp3B3=(_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp8E0[0]=& _tmp8DF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E1="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp8E1,sizeof(char),44))),_tag_dyneither(_tmp8E0,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3B3;};;_pop_region(r);}static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*
index){unsigned int _tmp3B5;int _tmp3B6;struct _tuple15 _tmp3B4=Cyc_Evexp_eval_const_uint_exp(
index);_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;if(!_tmp3B6){const char*_tmp8E4;void*
_tmp8E3;return(_tmp8E3=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8E4="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp8E4,sizeof(char),47))),_tag_dyneither(_tmp8E3,sizeof(void*),0)));}{
struct _handler_cons _tmp3B9;_push_handler(& _tmp3B9);{int _tmp3BB=0;if(setjmp(
_tmp3B9.handler))_tmp3BB=1;if(!_tmp3BB){{void*_tmp3BC=(*((struct _tuple19*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3B5)).f2;_npop_handler(0);
return _tmp3BC;};_pop_handler();}else{void*_tmp3BA=(void*)_exn_thrown;void*
_tmp3BE=_tmp3BA;_LL1A3: {struct Cyc_List_Nth_struct*_tmp3BF=(struct Cyc_List_Nth_struct*)
_tmp3BE;if(_tmp3BF->tag != Cyc_List_Nth)goto _LL1A5;}_LL1A4: {const char*_tmp8E9;
void*_tmp8E8[2];struct Cyc_Int_pa_struct _tmp8E7;struct Cyc_Int_pa_struct _tmp8E6;
return(_tmp8E6.tag=1,((_tmp8E6.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp8E7.tag=1,((_tmp8E7.f1=(unsigned long)((int)_tmp3B5),((
_tmp8E8[0]=& _tmp8E7,((_tmp8E8[1]=& _tmp8E6,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8E9="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp8E9,sizeof(char),41))),_tag_dyneither(_tmp8E8,sizeof(void*),2)))))))))))));}
_LL1A5:;_LL1A6:(void)_throw(_tmp3BE);_LL1A2:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp3C4=_new_region("r");struct
_RegionHandle*r=& _tmp3C4;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp3C5=Cyc_Tcenv_clear_lhs(
r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));Cyc_Tcexp_tcExp(_tmp3C5,0,e1);Cyc_Tcexp_tcExp(
_tmp3C5,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);if(!Cyc_Tcutil_coerce_sint_typ(_tmp3C5,e2)){const char*
_tmp8ED;void*_tmp8EC[1];struct Cyc_String_pa_struct _tmp8EB;void*_tmp3C9=(_tmp8EB.tag=
0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp8EC[0]=& _tmp8EB,Cyc_Tcexp_expr_err(_tmp3C5,e2->loc,topt,((_tmp8ED="expecting int subscript, found %s",
_tag_dyneither(_tmp8ED,sizeof(char),34))),_tag_dyneither(_tmp8EC,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3C9;}{void*_tmp3CA=t1;struct Cyc_Absyn_PtrInfo _tmp3CC;
void*_tmp3CD;struct Cyc_Absyn_Tqual _tmp3CE;struct Cyc_Absyn_PtrAtts _tmp3CF;void*
_tmp3D0;union Cyc_Absyn_Constraint*_tmp3D1;union Cyc_Absyn_Constraint*_tmp3D2;
struct Cyc_List_List*_tmp3D4;_LL1A8: {struct Cyc_Absyn_PointerType_struct*_tmp3CB=(
struct Cyc_Absyn_PointerType_struct*)_tmp3CA;if(_tmp3CB->tag != 5)goto _LL1AA;else{
_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CC.elt_typ;_tmp3CE=_tmp3CC.elt_tq;_tmp3CF=
_tmp3CC.ptr_atts;_tmp3D0=_tmp3CF.rgn;_tmp3D1=_tmp3CF.bounds;_tmp3D2=_tmp3CF.zero_term;}}
_LL1A9: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp3D2)){int emit_warning=1;{void*_tmp3D5=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3D1);struct Cyc_Absyn_Exp*
_tmp3D7;_LL1AF: {struct Cyc_Absyn_Upper_b_struct*_tmp3D6=(struct Cyc_Absyn_Upper_b_struct*)
_tmp3D5;if(_tmp3D6->tag != 1)goto _LL1B1;else{_tmp3D7=_tmp3D6->f1;}}_LL1B0: if(Cyc_Tcutil_is_const_exp(
_tmp3C5,e2)){unsigned int _tmp3DA;int _tmp3DB;struct _tuple15 _tmp3D9=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3DA=_tmp3D9.f1;_tmp3DB=_tmp3D9.f2;if(_tmp3DB){unsigned int _tmp3DD;int
_tmp3DE;struct _tuple15 _tmp3DC=Cyc_Evexp_eval_const_uint_exp(_tmp3D7);_tmp3DD=
_tmp3DC.f1;_tmp3DE=_tmp3DC.f2;if(_tmp3DE  && _tmp3DD > _tmp3DA)emit_warning=0;}}
goto _LL1AE;_LL1B1: {struct Cyc_Absyn_DynEither_b_struct*_tmp3D8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3D5;if(_tmp3D8->tag != 0)goto _LL1AE;}_LL1B2: emit_warning=0;goto _LL1AE;_LL1AE:;}
if(emit_warning){const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_Tcutil_warn(e2->loc,((
_tmp8F0="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp8F0,sizeof(char),63))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp3C5,e2)){unsigned int _tmp3E2;int _tmp3E3;
struct _tuple15 _tmp3E1=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3E2=_tmp3E1.f1;
_tmp3E3=_tmp3E1.f2;if(_tmp3E3)Cyc_Tcutil_check_bound(loc,_tmp3E2,_tmp3D1);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3D1) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3D2)){const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,
Cyc_Tcutil_warn(e1->loc,((_tmp8F3="subscript applied to pointer to one object",
_tag_dyneither(_tmp8F3,sizeof(char),43))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3D1);}}Cyc_Tcenv_check_rgn_accessible(
_tmp3C5,loc,_tmp3D0);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3CD),& Cyc_Tcutil_tmk)){
const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,Cyc_Tcutil_terr(e1->loc,((_tmp8F6="can't subscript an abstract pointer",
_tag_dyneither(_tmp8F6,sizeof(char),36))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}{
void*_tmp3E8=_tmp3CD;_npop_handler(0);return _tmp3E8;};_LL1AA: {struct Cyc_Absyn_TupleType_struct*
_tmp3D3=(struct Cyc_Absyn_TupleType_struct*)_tmp3CA;if(_tmp3D3->tag != 10)goto
_LL1AC;else{_tmp3D4=_tmp3D3->f1;}}_LL1AB: {void*_tmp3E9=Cyc_Tcexp_ithTupleType(
_tmp3C5,loc,_tmp3D4,e2);_npop_handler(0);return _tmp3E9;}_LL1AC:;_LL1AD: {const
char*_tmp8FA;void*_tmp8F9[1];struct Cyc_String_pa_struct _tmp8F8;void*_tmp3ED=(
_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp8F9[0]=& _tmp8F8,Cyc_Tcexp_expr_err(_tmp3C5,loc,topt,((_tmp8FA="subscript applied to %s",
_tag_dyneither(_tmp8FA,sizeof(char),24))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))));
_npop_handler(0);return _tmp3ED;}_LL1A7:;};};};_pop_region(r);}static void*Cyc_Tcexp_tcTuple(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){int
done=0;struct Cyc_List_List*fields=0;if(topt != 0){void*_tmp3EE=Cyc_Tcutil_compress(*
topt);struct Cyc_List_List*_tmp3F0;_LL1B4: {struct Cyc_Absyn_TupleType_struct*
_tmp3EF=(struct Cyc_Absyn_TupleType_struct*)_tmp3EE;if(_tmp3EF->tag != 10)goto
_LL1B6;else{_tmp3F0=_tmp3EF->f1;}}_LL1B5: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp3F0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))goto _LL1B3;for(0;
es != 0;(es=es->tl,_tmp3F0=_tmp3F0->tl)){int bogus=0;void*_tmp3F1=(*((struct
_tuple19*)((struct Cyc_List_List*)_check_null(_tmp3F0))->hd)).f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp3F1,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(te,(
struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp3F0->hd)).f2,& bogus);{struct
_tuple19*_tmp8FD;struct Cyc_List_List*_tmp8FC;fields=((_tmp8FC=_cycalloc(sizeof(*
_tmp8FC)),((_tmp8FC->hd=((_tmp8FD=_cycalloc(sizeof(*_tmp8FD)),((_tmp8FD->f1=(*((
struct _tuple19*)_tmp3F0->hd)).f1,((_tmp8FD->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp8FD)))))),((_tmp8FC->tl=
fields,_tmp8FC))))));};}done=1;goto _LL1B3;_LL1B6:;_LL1B7: goto _LL1B3;_LL1B3:;}if(
!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple19*_tmp900;struct Cyc_List_List*_tmp8FF;fields=((_tmp8FF=
_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF->hd=((_tmp900=_cycalloc(sizeof(*_tmp900)),((
_tmp900->f1=Cyc_Absyn_empty_tqual(0),((_tmp900->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp900)))))),((_tmp8FF->tl=
fields,_tmp8FF))))));};}{struct Cyc_Absyn_TupleType_struct _tmp903;struct Cyc_Absyn_TupleType_struct*
_tmp902;return(void*)((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902[0]=((
_tmp903.tag=10,((_tmp903.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp903)))),_tmp902))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){const char*
_tmp906;void*_tmp905;return(_tmp905=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp906="tcCompoundLit",
_tag_dyneither(_tmp906,sizeof(char),14))),_tag_dyneither(_tmp905,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
void*res_t2;struct _RegionHandle _tmp3FA=_new_region("r");struct _RegionHandle*r=&
_tmp3FA;_push_region(r);{int _tmp3FB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
des);struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(
r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct
Cyc_Absyn_Const_e_struct _tmp909;struct Cyc_Absyn_Const_e_struct*_tmp908;struct Cyc_Absyn_Const_e_struct*
_tmp3FC=(_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=0,((
_tmp909.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3FB),_tmp909)))),_tmp908)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3FC,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3FD=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp3FB - 1);if(!Cyc_Tcutil_is_zero(_tmp3FD)){const char*_tmp90C;
void*_tmp90B;(_tmp90B=0,Cyc_Tcutil_terr(_tmp3FD->loc,((_tmp90C="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp90C,sizeof(char),45))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp90D;sz_exp->topt=((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((
_tmp90D->v=Cyc_Absyn_uint_typ,_tmp90D))));}res_t2=Cyc_Absyn_array_typ(res,(
unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)
sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);{struct Cyc_List_List*
es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct
Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,es)){const char*
_tmp911;void*_tmp910[1];struct Cyc_String_pa_struct _tmp90F;(_tmp90F.tag=0,((
_tmp90F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp910[0]=& _tmp90F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp911="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp911,sizeof(char),53))),_tag_dyneither(_tmp910,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple18*)des->hd)).f1;if(ds != 0){void*_tmp406=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp409;_LL1B9: {struct Cyc_Absyn_FieldName_struct*_tmp407=(struct Cyc_Absyn_FieldName_struct*)
_tmp406;if(_tmp407->tag != 1)goto _LL1BB;}_LL1BA:{const char*_tmp914;void*_tmp913;(
_tmp913=0,Cyc_Tcutil_terr(loc,((_tmp914="only array index designators are supported",
_tag_dyneither(_tmp914,sizeof(char),43))),_tag_dyneither(_tmp913,sizeof(void*),0)));}
goto _LL1B8;_LL1BB: {struct Cyc_Absyn_ArrayElement_struct*_tmp408=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp406;if(_tmp408->tag != 0)goto _LL1B8;else{_tmp409=_tmp408->f1;}}_LL1BC: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp409);{unsigned int _tmp40D;int _tmp40E;struct _tuple15 _tmp40C=Cyc_Evexp_eval_const_uint_exp(
_tmp409);_tmp40D=_tmp40C.f1;_tmp40E=_tmp40C.f2;if(!_tmp40E){const char*_tmp917;
void*_tmp916;(_tmp916=0,Cyc_Tcutil_terr(_tmp409->loc,((_tmp917="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp917,sizeof(char),47))),_tag_dyneither(_tmp916,sizeof(void*),0)));}
else{if(_tmp40D != offset){const char*_tmp91C;void*_tmp91B[2];struct Cyc_Int_pa_struct
_tmp91A;struct Cyc_Int_pa_struct _tmp919;(_tmp919.tag=1,((_tmp919.f1=(
unsigned long)((int)_tmp40D),((_tmp91A.tag=1,((_tmp91A.f1=(unsigned long)offset,((
_tmp91B[0]=& _tmp91A,((_tmp91B[1]=& _tmp919,Cyc_Tcutil_terr(_tmp409->loc,((_tmp91C="expecting index designator %d but found %d",
_tag_dyneither(_tmp91C,sizeof(char),43))),_tag_dyneither(_tmp91B,sizeof(void*),2)))))))))))));}}
goto _LL1B8;};_LL1B8:;}}}{void*_tmp415=res_t2;_npop_handler(0);return _tmp415;};;
_pop_region(r);}static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,
struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*
_tmp416=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);
_LL1BE: {struct Cyc_Absyn_TagType_struct*_tmp417=(struct Cyc_Absyn_TagType_struct*)
_tmp416;if(_tmp417->tag != 19)goto _LL1C0;}_LL1BF: goto _LL1BD;_LL1C0:;_LL1C1: if(!
Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*_tmp920;void*_tmp91F[1];struct
Cyc_String_pa_struct _tmp91E;(_tmp91E.tag=0,((_tmp91E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v)),((_tmp91F[0]=& _tmp91E,Cyc_Tcutil_terr(bound->loc,((
_tmp920="expecting unsigned int, found %s",_tag_dyneither(_tmp920,sizeof(char),
33))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))));}_LL1BD:;}if(!(vd->tq).real_const){
const char*_tmp923;void*_tmp922;(_tmp922=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp923="comprehension index variable is not declared const!",
_tag_dyneither(_tmp923,sizeof(char),52))),_tag_dyneither(_tmp922,sizeof(void*),0)));}{
struct _RegionHandle _tmp41D=_new_region("r");struct _RegionHandle*r=& _tmp41D;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 
0){te2=Cyc_Tcenv_new_block(r,loc,te2);te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
struct Cyc_Tcenv_Tenv*_tmp41E=te2;void**_tmp41F=0;struct Cyc_Absyn_Tqual*_tmp420=0;
union Cyc_Absyn_Constraint**_tmp421=0;if(topt != 0){void*_tmp422=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp424;struct Cyc_Absyn_ArrayInfo _tmp426;void*
_tmp427;struct Cyc_Absyn_Tqual _tmp428;struct Cyc_Absyn_Exp*_tmp429;union Cyc_Absyn_Constraint*
_tmp42A;_LL1C3: {struct Cyc_Absyn_PointerType_struct*_tmp423=(struct Cyc_Absyn_PointerType_struct*)
_tmp422;if(_tmp423->tag != 5)goto _LL1C5;else{_tmp424=_tmp423->f1;}}_LL1C4:{void**
_tmp924;_tmp41F=((_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924[0]=
_tmp424.elt_typ,_tmp924))));}{struct Cyc_Absyn_Tqual*_tmp925;_tmp420=((_tmp925=
_region_malloc(r,sizeof(*_tmp925)),((_tmp925[0]=_tmp424.elt_tq,_tmp925))));}{
union Cyc_Absyn_Constraint**_tmp926;_tmp421=((_tmp926=_region_malloc(r,sizeof(*
_tmp926)),((_tmp926[0]=(_tmp424.ptr_atts).zero_term,_tmp926))));}goto _LL1C2;
_LL1C5: {struct Cyc_Absyn_ArrayType_struct*_tmp425=(struct Cyc_Absyn_ArrayType_struct*)
_tmp422;if(_tmp425->tag != 8)goto _LL1C7;else{_tmp426=_tmp425->f1;_tmp427=_tmp426.elt_type;
_tmp428=_tmp426.tq;_tmp429=_tmp426.num_elts;_tmp42A=_tmp426.zero_term;}}_LL1C6:{
void**_tmp927;_tmp41F=((_tmp927=_region_malloc(r,sizeof(*_tmp927)),((_tmp927[0]=
_tmp427,_tmp927))));}{struct Cyc_Absyn_Tqual*_tmp928;_tmp420=((_tmp928=
_region_malloc(r,sizeof(*_tmp928)),((_tmp928[0]=_tmp428,_tmp928))));}{union Cyc_Absyn_Constraint**
_tmp929;_tmp421=((_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929[0]=
_tmp42A,_tmp929))));}goto _LL1C2;_LL1C7:;_LL1C8: goto _LL1C2;_LL1C2:;}{void*t=Cyc_Tcexp_tcExp(
_tmp41E,_tmp41F,body);if(_tmp41E->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp41E,
bound)){const char*_tmp92C;void*_tmp92B;(_tmp92B=0,Cyc_Tcutil_terr(bound->loc,((
_tmp92C="bound is not constant",_tag_dyneither(_tmp92C,sizeof(char),22))),
_tag_dyneither(_tmp92B,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp41E,
body)){const char*_tmp92F;void*_tmp92E;(_tmp92E=0,Cyc_Tcutil_terr(bound->loc,((
_tmp92F="body is not constant",_tag_dyneither(_tmp92F,sizeof(char),21))),
_tag_dyneither(_tmp92E,sizeof(void*),0)));}}if(_tmp421 != 0  && ((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp421)){struct Cyc_Absyn_Exp*
_tmp435=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp930;_tmp435->topt=((
_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930->v=Cyc_Absyn_uint_typ,_tmp930))));}
bound=Cyc_Absyn_add_exp(bound,_tmp435,0);{struct Cyc_Core_Opt*_tmp931;bound->topt=((
_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->v=Cyc_Absyn_uint_typ,_tmp931))));}*
is_zero_term=1;}{void*_tmp438=Cyc_Absyn_array_typ(t,_tmp420 == 0?Cyc_Absyn_empty_tqual(
0):*_tmp420,(struct Cyc_Absyn_Exp*)bound,_tmp421 == 0?Cyc_Absyn_false_conref:*
_tmp421,0);void*_tmp439=_tmp438;_npop_handler(0);return _tmp439;};};};};
_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,
void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,
struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(
struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{struct _handler_cons _tmp43A;
_push_handler(& _tmp43A);{int _tmp43C=0;if(setjmp(_tmp43A.handler))_tmp43C=1;if(!
_tmp43C){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;_pop_handler();}else{void*
_tmp43B=(void*)_exn_thrown;void*_tmp43E=_tmp43B;_LL1CA: {struct Cyc_Dict_Absent_struct*
_tmp43F=(struct Cyc_Dict_Absent_struct*)_tmp43E;if(_tmp43F->tag != Cyc_Dict_Absent)
goto _LL1CC;}_LL1CB:{const char*_tmp935;void*_tmp934[1];struct Cyc_String_pa_struct
_tmp933;(_tmp933.tag=0,((_tmp933.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn)),((_tmp934[0]=& _tmp933,Cyc_Tcutil_terr(loc,((
_tmp935="unbound struct/union name %s",_tag_dyneither(_tmp935,sizeof(char),29))),
_tag_dyneither(_tmp934,sizeof(void*),1)))))));}return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;
_LL1CC:;_LL1CD:(void)_throw(_tmp43E);_LL1C9:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)
ad;*tn=ad->name;}if(ad->impl == 0){{const char*_tmp93D;void*_tmp93C[1];const char*
_tmp93B;const char*_tmp93A;struct Cyc_String_pa_struct _tmp939;(_tmp939.tag=0,((
_tmp939.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp93A="struct",
_tag_dyneither(_tmp93A,sizeof(char),7)):((_tmp93B="union",_tag_dyneither(_tmp93B,
sizeof(char),6)))),((_tmp93C[0]=& _tmp939,Cyc_Tcutil_terr(loc,((_tmp93D="can't construct abstract %s",
_tag_dyneither(_tmp93D,sizeof(char),28))),_tag_dyneither(_tmp93C,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle _tmp448=_new_region("rgn");struct _RegionHandle*rgn=& _tmp448;
_push_region(rgn);{struct _tuple12 _tmp93E;struct _tuple12 _tmp449=(_tmp93E.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp93E.f2=rgn,_tmp93E)));struct Cyc_List_List*_tmp44A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),
struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp449,ad->tvs);struct Cyc_List_List*_tmp44B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct
_tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp449,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp44C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,
_tmp44A);struct Cyc_List_List*_tmp44D=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,
_tmp44B);struct Cyc_List_List*_tmp44E=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp44A,_tmp44B);struct Cyc_Absyn_AggrType_struct _tmp948;struct Cyc_Absyn_Aggrdecl**
_tmp947;struct Cyc_Absyn_AggrInfo _tmp946;struct Cyc_Absyn_AggrType_struct*_tmp945;
struct Cyc_Absyn_AggrType_struct*_tmp44F=(_tmp945=_cycalloc(sizeof(*_tmp945)),((
_tmp945[0]=((_tmp948.tag=11,((_tmp948.f1=((_tmp946.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947[0]=ad,_tmp947))))),((_tmp946.targs=
_tmp44C,_tmp946)))),_tmp948)))),_tmp945)));struct Cyc_List_List*_tmp450=*ts;
struct Cyc_List_List*_tmp451=_tmp44D;while(_tmp450 != 0  && _tmp451 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp450->hd);Cyc_Tcutil_check_no_qual(
loc,(void*)_tmp450->hd);Cyc_Tcutil_unify((void*)_tmp450->hd,(void*)_tmp451->hd);
_tmp450=_tmp450->tl;_tmp451=_tmp451->tl;}if(_tmp450 != 0){const char*_tmp94B;void*
_tmp94A;(_tmp94A=0,Cyc_Tcutil_terr(loc,((_tmp94B="too many explicit witness types",
_tag_dyneither(_tmp94B,sizeof(char),32))),_tag_dyneither(_tmp94A,sizeof(void*),0)));}*
ts=_tmp44D;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp44F,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct
Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple20 _tmp455;struct Cyc_Absyn_Aggrfield*
_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct _tuple20*_tmp454=(struct _tuple20*)
fields->hd;_tmp455=*_tmp454;_tmp456=_tmp455.f1;_tmp457=_tmp455.f2;{void*_tmp458=
Cyc_Tcutil_rsubstitute(rgn,_tmp44E,_tmp456->type);Cyc_Tcexp_tcExpInitializer(te,(
void**)& _tmp458,_tmp457);if(!Cyc_Tcutil_coerce_arg(te,_tmp457,_tmp458,& bogus)){{
const char*_tmp957;void*_tmp956[5];struct Cyc_String_pa_struct _tmp955;const char*
_tmp954;const char*_tmp953;struct Cyc_String_pa_struct _tmp952;struct Cyc_String_pa_struct
_tmp951;struct Cyc_String_pa_struct _tmp950;struct Cyc_String_pa_struct _tmp94F;(
_tmp94F.tag=0,((_tmp94F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp457->topt))->v)),((_tmp950.tag=0,((
_tmp950.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp458)),((_tmp951.tag=0,((_tmp951.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp952.tag=0,((_tmp952.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp953="struct",
_tag_dyneither(_tmp953,sizeof(char),7)):((_tmp954="union",_tag_dyneither(_tmp954,
sizeof(char),6)))),((_tmp955.tag=0,((_tmp955.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp456->name),((_tmp956[0]=& _tmp955,((_tmp956[1]=& _tmp952,((
_tmp956[2]=& _tmp951,((_tmp956[3]=& _tmp950,((_tmp956[4]=& _tmp94F,Cyc_Tcutil_terr(
_tmp457->loc,((_tmp957="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp957,sizeof(char),40))),_tag_dyneither(_tmp956,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp462=0;{struct Cyc_List_List*
_tmp463=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp463 != 0;_tmp463=_tmp463->tl){struct _tuple0*_tmp95A;struct Cyc_List_List*
_tmp959;_tmp462=((_tmp959=_cycalloc(sizeof(*_tmp959)),((_tmp959->hd=((_tmp95A=
_cycalloc(sizeof(*_tmp95A)),((_tmp95A->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp44E,(*((
struct _tuple0*)_tmp463->hd)).f1),((_tmp95A->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp44E,(*((struct _tuple0*)_tmp463->hd)).f2),_tmp95A)))))),((_tmp959->tl=_tmp462,
_tmp959))))));}}_tmp462=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp462);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp462);{void*_tmp466=(void*)
_tmp44F;_npop_handler(0);return _tmp466;};};};};_pop_region(rgn);};}static void*
Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*
args){struct _RegionHandle _tmp46C=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp46C;_push_region(rgn);{void*_tmp46D=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind
_tmp46F;struct Cyc_List_List*_tmp470;_LL1CF: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp46E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp46D;if(_tmp46E->tag != 12)goto
_LL1D1;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46E->f2;}}_LL1D0: if(_tmp46F == Cyc_Absyn_UnionA){
const char*_tmp95D;void*_tmp95C;(_tmp95C=0,Cyc_Tcutil_terr(loc,((_tmp95D="expecting struct but found union",
_tag_dyneither(_tmp95D,sizeof(char),33))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,
_tmp470);for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple20 _tmp474;
struct Cyc_Absyn_Aggrfield*_tmp475;struct Cyc_Absyn_Exp*_tmp476;struct _tuple20*
_tmp473=(struct _tuple20*)fields->hd;_tmp474=*_tmp473;_tmp475=_tmp474.f1;_tmp476=
_tmp474.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp475->type,_tmp476);if(!Cyc_Tcutil_coerce_arg(
te,_tmp476,_tmp475->type,& bogus)){{const char*_tmp963;void*_tmp962[3];struct Cyc_String_pa_struct
_tmp961;struct Cyc_String_pa_struct _tmp960;struct Cyc_String_pa_struct _tmp95F;(
_tmp95F.tag=0,((_tmp95F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp476->topt))->v)),((_tmp960.tag=0,((
_tmp960.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp475->type)),((_tmp961.tag=0,((_tmp961.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp475->name),((_tmp962[0]=& _tmp961,((_tmp962[1]=& _tmp960,((
_tmp962[2]=& _tmp95F,Cyc_Tcutil_terr(_tmp476->loc,((_tmp963="field %s of struct expects type %s != %s",
_tag_dyneither(_tmp963,sizeof(char),41))),_tag_dyneither(_tmp962,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CE;};_LL1D1:;_LL1D2: {const char*_tmp966;
void*_tmp965;(_tmp965=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp966="tcAnonStruct: wrong type",_tag_dyneither(_tmp966,
sizeof(char),25))),_tag_dyneither(_tmp965,sizeof(void*),0)));}_LL1CE:;}{void*
_tmp47E=ts;_npop_handler(0);return _tmp47E;};;_pop_region(rgn);}static void*Cyc_Tcexp_tcDatatype(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*
tuf){struct _RegionHandle _tmp47F=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp47F;_push_region(rgn);{struct _tuple12 _tmp967;struct _tuple12 _tmp480=(_tmp967.f1=
Cyc_Tcenv_lookup_type_vars(te),((_tmp967.f2=rgn,_tmp967)));struct Cyc_List_List*
_tmp481=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(
struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp480,tud->tvs);struct Cyc_List_List*
_tmp482=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*
x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp481);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp96D;struct Cyc_Absyn_DatatypeFieldInfo _tmp96C;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp96B;void*res=(void*)((_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B[0]=((
_tmp96D.tag=4,((_tmp96D.f1=((_tmp96C.field_info=Cyc_Absyn_KnownDatatypefield(tud,
tuf),((_tmp96C.targs=_tmp482,_tmp96C)))),_tmp96D)))),_tmp96B))));if(topt != 0){
void*_tmp483=Cyc_Tcutil_compress(*topt);_LL1D4: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp484=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp483;if(_tmp484->tag != 4)
goto _LL1D6;}_LL1D5: Cyc_Tcutil_unify(*topt,res);goto _LL1D3;_LL1D6:;_LL1D7: goto
_LL1D3;_LL1D3:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,
ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=
Cyc_Tcutil_rsubstitute(rgn,_tmp481,(*((struct _tuple19*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(
te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){{const char*_tmp975;
void*_tmp974[3];struct Cyc_String_pa_struct _tmp973;struct Cyc_String_pa_struct
_tmp972;const char*_tmp971;struct Cyc_String_pa_struct _tmp970;(_tmp970.tag=0,((
_tmp970.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct
_dyneither_ptr)((_tmp971="?",_tag_dyneither(_tmp971,sizeof(char),2))): Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((_tmp972.tag=0,((
_tmp972.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp973.tag=0,((_tmp973.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp974[0]=& _tmp973,((_tmp974[1]=& _tmp972,((_tmp974[2]=& _tmp970,
Cyc_Tcutil_terr(e->loc,((_tmp975="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp975,sizeof(char),82))),_tag_dyneither(_tmp974,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp979;void*_tmp978[1];
struct Cyc_String_pa_struct _tmp977;void*_tmp48E=(_tmp977.tag=0,((_tmp977.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp978[0]=& _tmp977,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp979="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp979,sizeof(char),47))),_tag_dyneither(_tmp978,sizeof(void*),1)))))));
_npop_handler(0);return _tmp48E;}if(ts != 0){const char*_tmp97D;void*_tmp97C[1];
struct Cyc_String_pa_struct _tmp97B;void*_tmp492=(_tmp97B.tag=0,((_tmp97B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp97C[0]=& _tmp97B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp97D="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp97D,sizeof(char),46))),_tag_dyneither(_tmp97C,sizeof(void*),1)))))));
_npop_handler(0);return _tmp492;}{void*_tmp493=res;_npop_handler(0);return _tmp493;};};};
_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp498=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp49F;struct Cyc_Absyn_PtrAtts _tmp4A0;union Cyc_Absyn_Constraint*
_tmp4A1;struct Cyc_Absyn_ArrayInfo _tmp4A5;void*_tmp4A6;struct Cyc_List_List*
_tmp4A9;struct Cyc_Absyn_AggrInfo _tmp4AB;union Cyc_Absyn_AggrInfoU _tmp4AC;struct
Cyc_List_List*_tmp4AD;struct Cyc_List_List*_tmp4B1;_LL1D9: {struct Cyc_Absyn_VoidType_struct*
_tmp499=(struct Cyc_Absyn_VoidType_struct*)_tmp498;if(_tmp499->tag != 0)goto _LL1DB;}
_LL1DA: return 1;_LL1DB: {struct Cyc_Absyn_Evar_struct*_tmp49A=(struct Cyc_Absyn_Evar_struct*)
_tmp498;if(_tmp49A->tag != 1)goto _LL1DD;}_LL1DC: goto _LL1DE;_LL1DD: {struct Cyc_Absyn_VarType_struct*
_tmp49B=(struct Cyc_Absyn_VarType_struct*)_tmp498;if(_tmp49B->tag != 2)goto _LL1DF;}
_LL1DE: goto _LL1E0;_LL1DF: {struct Cyc_Absyn_DatatypeType_struct*_tmp49C=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp498;if(_tmp49C->tag != 3)goto _LL1E1;}_LL1E0:
goto _LL1E2;_LL1E1: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp49D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp498;if(_tmp49D->tag != 4)goto _LL1E3;}_LL1E2: return 0;_LL1E3: {struct Cyc_Absyn_PointerType_struct*
_tmp49E=(struct Cyc_Absyn_PointerType_struct*)_tmp498;if(_tmp49E->tag != 5)goto
_LL1E5;else{_tmp49F=_tmp49E->f1;_tmp4A0=_tmp49F.ptr_atts;_tmp4A1=_tmp4A0.nullable;}}
_LL1E4: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp4A1);_LL1E5: {struct Cyc_Absyn_IntType_struct*_tmp4A2=(struct Cyc_Absyn_IntType_struct*)
_tmp498;if(_tmp4A2->tag != 6)goto _LL1E7;}_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_Absyn_FloatType_struct*
_tmp4A3=(struct Cyc_Absyn_FloatType_struct*)_tmp498;if(_tmp4A3->tag != 7)goto
_LL1E9;}_LL1E8: return 1;_LL1E9: {struct Cyc_Absyn_ArrayType_struct*_tmp4A4=(struct
Cyc_Absyn_ArrayType_struct*)_tmp498;if(_tmp4A4->tag != 8)goto _LL1EB;else{_tmp4A5=
_tmp4A4->f1;_tmp4A6=_tmp4A5.elt_type;}}_LL1EA: return Cyc_Tcexp_zeroable_type(
_tmp4A6);_LL1EB: {struct Cyc_Absyn_FnType_struct*_tmp4A7=(struct Cyc_Absyn_FnType_struct*)
_tmp498;if(_tmp4A7->tag != 9)goto _LL1ED;}_LL1EC: return 0;_LL1ED: {struct Cyc_Absyn_TupleType_struct*
_tmp4A8=(struct Cyc_Absyn_TupleType_struct*)_tmp498;if(_tmp4A8->tag != 10)goto
_LL1EF;else{_tmp4A9=_tmp4A8->f1;}}_LL1EE: for(0;(unsigned int)_tmp4A9;_tmp4A9=
_tmp4A9->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp4A9->hd)).f2))
return 0;}return 1;_LL1EF: {struct Cyc_Absyn_AggrType_struct*_tmp4AA=(struct Cyc_Absyn_AggrType_struct*)
_tmp498;if(_tmp4AA->tag != 11)goto _LL1F1;else{_tmp4AB=_tmp4AA->f1;_tmp4AC=_tmp4AB.aggr_info;
_tmp4AD=_tmp4AB.targs;}}_LL1F0: {struct Cyc_Absyn_Aggrdecl*_tmp4BE=Cyc_Absyn_get_known_aggrdecl(
_tmp4AC);if(_tmp4BE->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp4BE->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp4BF=_new_region("r");
struct _RegionHandle*r=& _tmp4BF;_push_region(r);{struct Cyc_List_List*_tmp4C0=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4BE->tvs,_tmp4AD);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BE->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4C0,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4C1=0;_npop_handler(0);return
_tmp4C1;}}}{int _tmp4C2=1;_npop_handler(0);return _tmp4C2;};};_pop_region(r);};}
_LL1F1: {struct Cyc_Absyn_EnumType_struct*_tmp4AE=(struct Cyc_Absyn_EnumType_struct*)
_tmp498;if(_tmp4AE->tag != 13)goto _LL1F3;}_LL1F2: return 1;_LL1F3: {struct Cyc_Absyn_TagType_struct*
_tmp4AF=(struct Cyc_Absyn_TagType_struct*)_tmp498;if(_tmp4AF->tag != 19)goto _LL1F5;}
_LL1F4: return 1;_LL1F5: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4B0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp498;if(_tmp4B0->tag != 12)goto _LL1F7;else{_tmp4B1=_tmp4B0->f2;}}_LL1F6: for(0;
_tmp4B1 != 0;_tmp4B1=_tmp4B1->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp4B1->hd)->type))return 0;}return 1;_LL1F7: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp4B2=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp498;if(_tmp4B2->tag != 14)goto
_LL1F9;}_LL1F8: return 1;_LL1F9: {struct Cyc_Absyn_TypedefType_struct*_tmp4B3=(
struct Cyc_Absyn_TypedefType_struct*)_tmp498;if(_tmp4B3->tag != 17)goto _LL1FB;}
_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_DynRgnType_struct*_tmp4B4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp498;if(_tmp4B4->tag != 16)goto _LL1FD;}_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp4B5=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp498;if(_tmp4B5->tag != 15)goto
_LL1FF;}_LL1FE: return 0;_LL1FF: {struct Cyc_Absyn_TypeDeclType_struct*_tmp4B6=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp498;if(_tmp4B6->tag != 26)goto _LL201;}
_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_ValueofType_struct*_tmp4B7=(struct Cyc_Absyn_ValueofType_struct*)
_tmp498;if(_tmp4B7->tag != 18)goto _LL203;}_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_HeapRgn_struct*
_tmp4B8=(struct Cyc_Absyn_HeapRgn_struct*)_tmp498;if(_tmp4B8->tag != 20)goto _LL205;}
_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_UniqueRgn_struct*_tmp4B9=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp498;if(_tmp4B9->tag != 21)goto _LL207;}_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp4BA=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp498;if(_tmp4BA->tag != 22)goto
_LL209;}_LL208: goto _LL20A;_LL209: {struct Cyc_Absyn_AccessEff_struct*_tmp4BB=(
struct Cyc_Absyn_AccessEff_struct*)_tmp498;if(_tmp4BB->tag != 23)goto _LL20B;}
_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_JoinEff_struct*_tmp4BC=(struct Cyc_Absyn_JoinEff_struct*)
_tmp498;if(_tmp4BC->tag != 24)goto _LL20D;}_LL20C: goto _LL20E;_LL20D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp4BD=(struct Cyc_Absyn_RgnsEff_struct*)_tmp498;if(_tmp4BD->tag != 25)goto _LL1D8;}
_LL20E: {const char*_tmp981;void*_tmp980[1];struct Cyc_String_pa_struct _tmp97F;(
_tmp97F.tag=0,((_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp980[0]=& _tmp97F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp981="bad type `%s' in zeroable type",_tag_dyneither(
_tmp981,sizeof(char),31))),_tag_dyneither(_tmp980,sizeof(void*),1)))))));}_LL1D8:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,
void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))
return;if(topt != 0){void*_tmp4C6=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp4C8;void*_tmp4C9;_LL210: {struct Cyc_Absyn_PointerType_struct*_tmp4C7=(struct
Cyc_Absyn_PointerType_struct*)_tmp4C6;if(_tmp4C7->tag != 5)goto _LL212;else{
_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C8.elt_typ;}}_LL211: Cyc_Tcutil_unify(_tmp4C9,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL20F;_LL212:;_LL213: goto _LL20F;_LL20F:;}{const char*_tmp98A;void*_tmp989[2];
const char*_tmp988;const char*_tmp987;struct Cyc_String_pa_struct _tmp986;struct Cyc_String_pa_struct
_tmp985;(_tmp985.tag=0,((_tmp985.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp986.tag=0,((_tmp986.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp987="calloc",
_tag_dyneither(_tmp987,sizeof(char),7))):(struct _dyneither_ptr)((_tmp988="malloc",
_tag_dyneither(_tmp988,sizeof(char),7))))),((_tmp989[0]=& _tmp986,((_tmp989[1]=&
_tmp985,Cyc_Tcutil_terr(loc,((_tmp98A="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp98A,sizeof(char),60))),_tag_dyneither(_tmp989,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL214: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL215: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,
void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp98D;struct Cyc_Absyn_RgnHandleType_struct*
_tmp98C;void*expected_type=(void*)((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((
_tmp98C[0]=((_tmp98D.tag=15,((_tmp98D.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp98D)))),_tmp98C))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*ropt));void*_tmp4D0=Cyc_Tcutil_compress(handle_type);void*_tmp4D2;
_LL218: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4D1=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4D0;if(_tmp4D1->tag != 15)goto _LL21A;else{_tmp4D2=(void*)_tmp4D1->f1;}}_LL219:
rgn=_tmp4D2;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL217;_LL21A:;_LL21B:{
const char*_tmp991;void*_tmp990[1];struct Cyc_String_pa_struct _tmp98F;(_tmp98F.tag=
0,((_tmp98F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp990[0]=& _tmp98F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*ropt))->loc,((_tmp991="expecting region_t type but found %s",
_tag_dyneither(_tmp991,sizeof(char),37))),_tag_dyneither(_tmp990,sizeof(void*),1)))))));}
goto _LL217;_LL217:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp994;void*_tmp993;(
_tmp993=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp994="calloc with empty type",_tag_dyneither(_tmp994,sizeof(char),23))),
_tag_dyneither(_tmp993,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcutil_check_no_qual(loc,elt_type);Cyc_Tcexp_check_malloc_type(1,
loc,topt,elt_type);num_elts=*e;one_elt=0;}else{void*er=(*e)->r;retry_sizeof: {
void*_tmp4DA=er;void*_tmp4DC;enum Cyc_Absyn_Primop _tmp4DE;struct Cyc_List_List*
_tmp4DF;struct Cyc_List_List _tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_List_List*
_tmp4E2;struct Cyc_List_List _tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_List_List*
_tmp4E5;_LL21D: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4DB=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4DA;if(_tmp4DB->tag != 18)goto _LL21F;else{_tmp4DC=(void*)_tmp4DB->f1;}}_LL21E:
elt_type=_tmp4DC;{void**_tmp995;*t=(void**)((_tmp995=_cycalloc(sizeof(*_tmp995)),((
_tmp995[0]=elt_type,_tmp995))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL21C;_LL21F: {struct Cyc_Absyn_Primop_e_struct*
_tmp4DD=(struct Cyc_Absyn_Primop_e_struct*)_tmp4DA;if(_tmp4DD->tag != 3)goto _LL221;
else{_tmp4DE=_tmp4DD->f1;if(_tmp4DE != Cyc_Absyn_Times)goto _LL221;_tmp4DF=_tmp4DD->f2;
if(_tmp4DF == 0)goto _LL221;_tmp4E0=*_tmp4DF;_tmp4E1=(struct Cyc_Absyn_Exp*)_tmp4E0.hd;
_tmp4E2=_tmp4E0.tl;if(_tmp4E2 == 0)goto _LL221;_tmp4E3=*_tmp4E2;_tmp4E4=(struct Cyc_Absyn_Exp*)
_tmp4E3.hd;_tmp4E5=_tmp4E3.tl;if(_tmp4E5 != 0)goto _LL221;}}_LL220:{struct _tuple0
_tmp996;struct _tuple0 _tmp4E8=(_tmp996.f1=_tmp4E1->r,((_tmp996.f2=_tmp4E4->r,
_tmp996)));void*_tmp4E9;void*_tmp4EB;void*_tmp4EC;void*_tmp4EE;_LL224: _tmp4E9=
_tmp4E8.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4EA=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4E9;if(_tmp4EA->tag != 18)goto _LL226;else{_tmp4EB=(void*)_tmp4EA->f1;}};
_LL225: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4EB);elt_type=_tmp4EB;{void**
_tmp997;*t=(void**)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=elt_type,
_tmp997))));}num_elts=_tmp4E4;one_elt=0;goto _LL223;_LL226: _tmp4EC=_tmp4E8.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4ED=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4EC;if(_tmp4ED->tag != 18)goto _LL228;else{_tmp4EE=(void*)_tmp4ED->f1;}};
_LL227: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4EE);elt_type=_tmp4EE;{void**
_tmp998;*t=(void**)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=elt_type,
_tmp998))));}num_elts=_tmp4E1;one_elt=0;goto _LL223;_LL228:;_LL229: goto No_sizeof;
_LL223:;}goto _LL21C;_LL221:;_LL222: No_sizeof: {struct Cyc_Absyn_Exp*_tmp4F1=*e;{
void*_tmp4F2=_tmp4F1->r;struct Cyc_Absyn_Exp*_tmp4F4;_LL22B: {struct Cyc_Absyn_Cast_e_struct*
_tmp4F3=(struct Cyc_Absyn_Cast_e_struct*)_tmp4F2;if(_tmp4F3->tag != 15)goto _LL22D;
else{_tmp4F4=_tmp4F3->f2;}}_LL22C: _tmp4F1=_tmp4F4;goto _LL22A;_LL22D:;_LL22E: goto
_LL22A;_LL22A:;}{void*_tmp4F5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4F1->topt))->v);void*_tmp4F7;_LL230: {struct Cyc_Absyn_TagType_struct*
_tmp4F6=(struct Cyc_Absyn_TagType_struct*)_tmp4F5;if(_tmp4F6->tag != 19)goto _LL232;
else{_tmp4F7=(void*)_tmp4F6->f1;}}_LL231:{void*_tmp4F8=Cyc_Tcutil_compress(
_tmp4F7);struct Cyc_Absyn_Exp*_tmp4FA;_LL235: {struct Cyc_Absyn_ValueofType_struct*
_tmp4F9=(struct Cyc_Absyn_ValueofType_struct*)_tmp4F8;if(_tmp4F9->tag != 18)goto
_LL237;else{_tmp4FA=_tmp4F9->f1;}}_LL236: er=_tmp4FA->r;goto retry_sizeof;_LL237:;
_LL238: goto _LL234;_LL234:;}goto _LL22F;_LL232:;_LL233: goto _LL22F;_LL22F:;}
elt_type=Cyc_Absyn_char_typ;{void**_tmp999;*t=(void**)((_tmp999=_cycalloc(
sizeof(*_tmp999)),((_tmp999[0]=elt_type,_tmp999))));}num_elts=*e;one_elt=0;}goto
_LL21C;_LL21C:;}}*is_fat=!one_elt;{void*_tmp4FC=elt_type;struct Cyc_Absyn_AggrInfo
_tmp4FE;union Cyc_Absyn_AggrInfoU _tmp4FF;struct Cyc_Absyn_Aggrdecl**_tmp500;struct
Cyc_Absyn_Aggrdecl*_tmp501;_LL23A: {struct Cyc_Absyn_AggrType_struct*_tmp4FD=(
struct Cyc_Absyn_AggrType_struct*)_tmp4FC;if(_tmp4FD->tag != 11)goto _LL23C;else{
_tmp4FE=_tmp4FD->f1;_tmp4FF=_tmp4FE.aggr_info;if((_tmp4FF.KnownAggr).tag != 2)
goto _LL23C;_tmp500=(struct Cyc_Absyn_Aggrdecl**)(_tmp4FF.KnownAggr).val;_tmp501=*
_tmp500;}}_LL23B: if(_tmp501->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp501->impl))->exist_vars != 0){const char*_tmp99C;void*_tmp99B;(
_tmp99B=0,Cyc_Tcutil_terr(loc,((_tmp99C="malloc with existential types not yet implemented",
_tag_dyneither(_tmp99C,sizeof(char),50))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
goto _LL239;_LL23C:;_LL23D: goto _LL239;_LL239:;}{void*(*_tmp504)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp505=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp506=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp508;struct Cyc_Absyn_PtrAtts
_tmp509;union Cyc_Absyn_Constraint*_tmp50A;union Cyc_Absyn_Constraint*_tmp50B;
union Cyc_Absyn_Constraint*_tmp50C;_LL23F: {struct Cyc_Absyn_PointerType_struct*
_tmp507=(struct Cyc_Absyn_PointerType_struct*)_tmp506;if(_tmp507->tag != 5)goto
_LL241;else{_tmp508=_tmp507->f1;_tmp509=_tmp508.ptr_atts;_tmp50A=_tmp509.nullable;
_tmp50B=_tmp509.bounds;_tmp50C=_tmp509.zero_term;}}_LL240: _tmp505=_tmp50C;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp50A))
_tmp504=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp50C) && !(*is_calloc)){{const char*_tmp99F;void*_tmp99E;(_tmp99E=0,Cyc_Tcutil_warn(
loc,((_tmp99F="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp99F,sizeof(char),55))),_tag_dyneither(_tmp99E,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp50F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp50B);struct Cyc_Absyn_Exp*_tmp512;_LL244: {struct Cyc_Absyn_DynEither_b_struct*
_tmp510=(struct Cyc_Absyn_DynEither_b_struct*)_tmp50F;if(_tmp510->tag != 0)goto
_LL246;}_LL245: goto _LL243;_LL246:{struct Cyc_Absyn_Upper_b_struct*_tmp511=(struct
Cyc_Absyn_Upper_b_struct*)_tmp50F;if(_tmp511->tag != 1)goto _LL248;else{_tmp512=
_tmp511->f1;}}if(!(!one_elt))goto _LL248;_LL247: {int _tmp513=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp513  && Cyc_Evexp_same_const_exp(_tmp512,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp50B),_tmp505);}{void*_tmp514=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp516;_LL24B: {struct Cyc_Absyn_TagType_struct*_tmp515=(struct Cyc_Absyn_TagType_struct*)
_tmp514;if(_tmp515->tag != 19)goto _LL24D;else{_tmp516=(void*)_tmp515->f1;}}_LL24C: {
struct Cyc_Absyn_Exp*_tmp517=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp516,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp517,
_tmp512)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp50B),_tmp505);}
goto _LL24A;}_LL24D:;_LL24E: goto _LL24A;_LL24A:;}goto _LL243;}_LL248:;_LL249: goto
_LL243;_LL243:;}goto _LL23E;_LL241:;_LL242: goto _LL23E;_LL23E:;}if(!one_elt)
_tmp504=Cyc_Absyn_dyneither_typ;return _tmp504(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp505);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,
unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle _tmp518=_new_region("r");struct _RegionHandle*r=& _tmp518;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp519=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(
r,te));Cyc_Tcexp_tcExp(_tmp519,0,e1);Cyc_Tcexp_tcExp(_tmp519,(void**)((void**)((
void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp51A=Cyc_Tcutil_compress(t1);_LL250: {struct
Cyc_Absyn_ArrayType_struct*_tmp51B=(struct Cyc_Absyn_ArrayType_struct*)_tmp51A;
if(_tmp51B->tag != 8)goto _LL252;}_LL251:{const char*_tmp9A2;void*_tmp9A1;(_tmp9A1=
0,Cyc_Tcutil_terr(loc,((_tmp9A2="cannot assign to an array",_tag_dyneither(
_tmp9A2,sizeof(char),26))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));}goto _LL24F;
_LL252:;_LL253: goto _LL24F;_LL24F:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp9A5;void*_tmp9A4;(_tmp9A4=0,Cyc_Tcutil_terr(loc,((
_tmp9A5="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp9A5,sizeof(char),58))),_tag_dyneither(_tmp9A4,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp9A8;void*_tmp9A7;void*_tmp522=(
_tmp9A7=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9A8="swap non-lvalue",
_tag_dyneither(_tmp9A8,sizeof(char),16))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));
_npop_handler(0);return _tmp522;}if(!Cyc_Absyn_is_lvalue(e2)){const char*_tmp9AB;
void*_tmp9AA;void*_tmp525=(_tmp9AA=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((
_tmp9AB="swap non-lvalue",_tag_dyneither(_tmp9AB,sizeof(char),16))),
_tag_dyneither(_tmp9AA,sizeof(void*),0)));_npop_handler(0);return _tmp525;}{void*
t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){const
char*_tmp9AE;void*_tmp9AD;void*_tmp528=(_tmp9AD=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp9AE="swap value in zeroterm array",_tag_dyneither(_tmp9AE,sizeof(char),
29))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));_npop_handler(0);return _tmp528;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp9B1;
void*_tmp9B0;void*_tmp52B=(_tmp9B0=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((
_tmp9B1="swap value in zeroterm array",_tag_dyneither(_tmp9B1,sizeof(char),29))),
_tag_dyneither(_tmp9B0,sizeof(void*),0)));_npop_handler(0);return _tmp52B;}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp9B6;void*_tmp9B5[2];struct Cyc_String_pa_struct _tmp9B4;struct Cyc_String_pa_struct
_tmp9B3;void*_tmp52C=(_tmp9B3.tag=0,((_tmp9B3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp9B4.tag=0,((_tmp9B4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9B5[0]=&
_tmp9B4,((_tmp9B5[1]=& _tmp9B3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9B6="type mismatch: %s != %s",
_tag_dyneither(_tmp9B6,sizeof(char),24))),_tag_dyneither(_tmp9B5,sizeof(void*),2)))))))))))));
void*_tmp52D=_tmp52C;_npop_handler(0);return _tmp52D;}{void*_tmp532=(void*)& Cyc_Absyn_VoidType_val;
_npop_handler(0);return _tmp532;};};};};;_pop_region(r);}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**
topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle _tmp533=_new_region("r");struct
_RegionHandle*r=& _tmp533;_push_region(r);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);Cyc_Tcexp_in_stmt_exp=
old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
while(1){void*_tmp534=s->r;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Stmt*
_tmp538;struct Cyc_Absyn_Stmt*_tmp539;struct Cyc_Absyn_Decl*_tmp53B;struct Cyc_Absyn_Stmt*
_tmp53C;_LL255: {struct Cyc_Absyn_Exp_s_struct*_tmp535=(struct Cyc_Absyn_Exp_s_struct*)
_tmp534;if(_tmp535->tag != 1)goto _LL257;else{_tmp536=_tmp535->f1;}}_LL256: {void*
_tmp53D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp536->topt))->v;if(!Cyc_Tcutil_unify(
_tmp53D,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*
_tmp9BA;void*_tmp9B9[1];struct Cyc_String_pa_struct _tmp9B8;(_tmp9B8.tag=0,((
_tmp9B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp53D)),((_tmp9B9[0]=& _tmp9B8,Cyc_Tcutil_terr(loc,((_tmp9BA="statement expression returns type %s",
_tag_dyneither(_tmp9BA,sizeof(char),37))),_tag_dyneither(_tmp9B9,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{void*_tmp541=_tmp53D;_npop_handler(0);return
_tmp541;};}_LL257: {struct Cyc_Absyn_Seq_s_struct*_tmp537=(struct Cyc_Absyn_Seq_s_struct*)
_tmp534;if(_tmp537->tag != 2)goto _LL259;else{_tmp538=_tmp537->f1;_tmp539=_tmp537->f2;}}
_LL258: s=_tmp539;continue;_LL259: {struct Cyc_Absyn_Decl_s_struct*_tmp53A=(struct
Cyc_Absyn_Decl_s_struct*)_tmp534;if(_tmp53A->tag != 12)goto _LL25B;else{_tmp53B=
_tmp53A->f1;_tmp53C=_tmp53A->f2;}}_LL25A: s=_tmp53C;continue;_LL25B:;_LL25C: {
const char*_tmp9BD;void*_tmp9BC;void*_tmp544=(_tmp9BC=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp9BD="statement expression must end with expression",_tag_dyneither(
_tmp9BD,sizeof(char),46))),_tag_dyneither(_tmp9BC,sizeof(void*),0)));
_npop_handler(0);return _tmp544;}_LL254:;};_pop_region(r);}static void*Cyc_Tcexp_tcTagcheck(
struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));{void*
_tmp545=t;struct Cyc_Absyn_AggrInfo _tmp547;union Cyc_Absyn_AggrInfoU _tmp548;struct
Cyc_Absyn_Aggrdecl**_tmp549;struct Cyc_Absyn_Aggrdecl*_tmp54A;_LL25E: {struct Cyc_Absyn_AggrType_struct*
_tmp546=(struct Cyc_Absyn_AggrType_struct*)_tmp545;if(_tmp546->tag != 11)goto
_LL260;else{_tmp547=_tmp546->f1;_tmp548=_tmp547.aggr_info;if((_tmp548.KnownAggr).tag
!= 2)goto _LL260;_tmp549=(struct Cyc_Absyn_Aggrdecl**)(_tmp548.KnownAggr).val;
_tmp54A=*_tmp549;}}_LL25F: if((_tmp54A->kind == Cyc_Absyn_UnionA  && _tmp54A->impl
!= 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp54A->impl))->tagged)
goto _LL25D;goto _LL261;_LL260:;_LL261:{const char*_tmp9C1;void*_tmp9C0[1];struct
Cyc_String_pa_struct _tmp9BF;(_tmp9BF.tag=0,((_tmp9BF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9C0[0]=& _tmp9BF,Cyc_Tcutil_terr(
loc,((_tmp9C1="expecting @tagged union but found %s",_tag_dyneither(_tmp9C1,
sizeof(char),37))),_tag_dyneither(_tmp9C0,sizeof(void*),1)))))));}goto _LL25D;
_LL25D:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;if(*rgn_handle
!= 0){struct Cyc_Absyn_RgnHandleType_struct _tmp9C4;struct Cyc_Absyn_RgnHandleType_struct*
_tmp9C3;void*expected_type=(void*)((_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((
_tmp9C3[0]=((_tmp9C4.tag=15,((_tmp9C4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9C4)))),_tmp9C3))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle));void*_tmp54E=Cyc_Tcutil_compress(handle_type);void*
_tmp550;_LL263: {struct Cyc_Absyn_RgnHandleType_struct*_tmp54F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp54E;if(_tmp54F->tag != 15)goto _LL265;else{_tmp550=(void*)_tmp54F->f1;}}_LL264:
rgn=_tmp550;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL262;_LL265:;_LL266:{
const char*_tmp9C8;void*_tmp9C7[1];struct Cyc_String_pa_struct _tmp9C6;(_tmp9C6.tag=
0,((_tmp9C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp9C7[0]=& _tmp9C6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle))->loc,((_tmp9C8="expecting region_t type but found %s",
_tag_dyneither(_tmp9C8,sizeof(char),37))),_tag_dyneither(_tmp9C7,sizeof(void*),1)))))));}
goto _LL262;_LL262:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp556=e1->r;struct Cyc_Core_Opt*_tmp559;struct Cyc_List_List*_tmp55A;struct
Cyc_List_List*_tmp55C;union Cyc_Absyn_Cnst _tmp55E;struct _dyneither_ptr _tmp55F;
union Cyc_Absyn_Cnst _tmp561;struct _dyneither_ptr _tmp562;_LL268: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp557=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp556;if(_tmp557->tag != 28)
goto _LL26A;}_LL269: {void*_tmp563=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*
_tmp564=Cyc_Tcutil_compress(_tmp563);struct Cyc_Absyn_ArrayInfo _tmp566;void*
_tmp567;struct Cyc_Absyn_Tqual _tmp568;struct Cyc_Absyn_Exp*_tmp569;union Cyc_Absyn_Constraint*
_tmp56A;_LL275: {struct Cyc_Absyn_ArrayType_struct*_tmp565=(struct Cyc_Absyn_ArrayType_struct*)
_tmp564;if(_tmp565->tag != 8)goto _LL277;else{_tmp566=_tmp565->f1;_tmp567=_tmp566.elt_type;
_tmp568=_tmp566.tq;_tmp569=_tmp566.num_elts;_tmp56A=_tmp566.zero_term;}}_LL276: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp569);void*b;{void*
_tmp56B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp56D;_LL27A: {struct Cyc_Absyn_TagType_struct*_tmp56C=(struct Cyc_Absyn_TagType_struct*)
_tmp56B;if(_tmp56C->tag != 19)goto _LL27C;else{_tmp56D=(void*)_tmp56C->f1;}}_LL27B:{
struct Cyc_Absyn_Upper_b_struct _tmp9CB;struct Cyc_Absyn_Upper_b_struct*_tmp9CA;b=(
void*)((_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CB.tag=1,((
_tmp9CB.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp56D,0),
0,Cyc_Absyn_No_coercion,0),_tmp9CB)))),_tmp9CA))));}goto _LL279;_LL27C:;_LL27D:
if(Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp9CE;struct
Cyc_Absyn_Upper_b_struct*_tmp9CD;b=(void*)((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((
_tmp9CD[0]=((_tmp9CE.tag=1,((_tmp9CE.f1=bnd,_tmp9CE)))),_tmp9CD))));}else{b=(
void*)& Cyc_Absyn_DynEither_b_val;}_LL279:;}{struct Cyc_Absyn_PointerType_struct
_tmp9D8;struct Cyc_Absyn_PtrAtts _tmp9D7;struct Cyc_Absyn_PtrInfo _tmp9D6;struct Cyc_Absyn_PointerType_struct*
_tmp9D5;void*res_typ=(void*)((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=((
_tmp9D8.tag=5,((_tmp9D8.f1=((_tmp9D6.elt_typ=_tmp567,((_tmp9D6.elt_tq=_tmp568,((
_tmp9D6.ptr_atts=((_tmp9D7.rgn=rgn,((_tmp9D7.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp9D7.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp9D7.zero_term=_tmp56A,((_tmp9D7.ptrloc=0,
_tmp9D7)))))))))),_tmp9D6)))))),_tmp9D8)))),_tmp9D5))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp9D9;e->topt=((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9->v=res_typ,
_tmp9D9))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL277:;_LL278: {const char*_tmp9DC;void*_tmp9DB;(
_tmp9DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9DC="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9DC,
sizeof(char),45))),_tag_dyneither(_tmp9DB,sizeof(void*),0)));}_LL274:;}_LL26A: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp558=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp556;if(_tmp558->tag != 36)goto _LL26C;else{_tmp559=_tmp558->f1;_tmp55A=_tmp558->f2;}}
_LL26B:{struct Cyc_Absyn_Array_e_struct _tmp9DF;struct Cyc_Absyn_Array_e_struct*
_tmp9DE;e1->r=(void*)((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=((
_tmp9DF.tag=27,((_tmp9DF.f1=_tmp55A,_tmp9DF)))),_tmp9DE))));}_tmp55C=_tmp55A;
goto _LL26D;_LL26C: {struct Cyc_Absyn_Array_e_struct*_tmp55B=(struct Cyc_Absyn_Array_e_struct*)
_tmp556;if(_tmp55B->tag != 27)goto _LL26E;else{_tmp55C=_tmp55B->f1;}}_LL26D: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp57B=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp57D;void*_tmp57E;void**_tmp57F;struct Cyc_Absyn_Tqual
_tmp580;struct Cyc_Absyn_PtrAtts _tmp581;union Cyc_Absyn_Constraint*_tmp582;_LL27F: {
struct Cyc_Absyn_PointerType_struct*_tmp57C=(struct Cyc_Absyn_PointerType_struct*)
_tmp57B;if(_tmp57C->tag != 5)goto _LL281;else{_tmp57D=_tmp57C->f1;_tmp57E=_tmp57D.elt_typ;
_tmp57F=(void**)&(_tmp57C->f1).elt_typ;_tmp580=_tmp57D.elt_tq;_tmp581=_tmp57D.ptr_atts;
_tmp582=_tmp581.zero_term;}}_LL280: elt_typ_opt=(void**)_tmp57F;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp582);goto _LL27E;
_LL281:;_LL282: goto _LL27E;_LL27E:;}{void*_tmp583=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,0,zero_term,_tmp55C);{struct Cyc_Core_Opt*_tmp9E0;e1->topt=((_tmp9E0=
_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0->v=_tmp583,_tmp9E0))));}{void*res_typ;{
void*_tmp585=Cyc_Tcutil_compress(_tmp583);struct Cyc_Absyn_ArrayInfo _tmp587;void*
_tmp588;struct Cyc_Absyn_Tqual _tmp589;struct Cyc_Absyn_Exp*_tmp58A;union Cyc_Absyn_Constraint*
_tmp58B;_LL284: {struct Cyc_Absyn_ArrayType_struct*_tmp586=(struct Cyc_Absyn_ArrayType_struct*)
_tmp585;if(_tmp586->tag != 8)goto _LL286;else{_tmp587=_tmp586->f1;_tmp588=_tmp587.elt_type;
_tmp589=_tmp587.tq;_tmp58A=_tmp587.num_elts;_tmp58B=_tmp587.zero_term;}}_LL285:{
struct Cyc_Absyn_PointerType_struct _tmp9F5;struct Cyc_Absyn_PtrAtts _tmp9F4;struct
Cyc_Absyn_Upper_b_struct _tmp9F3;struct Cyc_Absyn_Upper_b_struct*_tmp9F2;struct Cyc_Absyn_PtrInfo
_tmp9F1;struct Cyc_Absyn_PointerType_struct*_tmp9F0;res_typ=(void*)((_tmp9F0=
_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F5.tag=5,((_tmp9F5.f1=((_tmp9F1.elt_typ=
_tmp588,((_tmp9F1.elt_tq=_tmp589,((_tmp9F1.ptr_atts=((_tmp9F4.rgn=rgn,((_tmp9F4.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9F4.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9F2=
_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=1,((_tmp9F3.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp58A),_tmp9F3)))),_tmp9F2))))),((_tmp9F4.zero_term=_tmp58B,((
_tmp9F4.ptrloc=0,_tmp9F4)))))))))),_tmp9F1)))))),_tmp9F5)))),_tmp9F0))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp9F6;e->topt=((_tmp9F6=_cycalloc(
sizeof(*_tmp9F6)),((_tmp9F6->v=res_typ,_tmp9F6))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL283;_LL286:;_LL287: {
const char*_tmp9F9;void*_tmp9F8;(_tmp9F8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9F9="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp9F9,sizeof(char),50))),_tag_dyneither(_tmp9F8,sizeof(void*),0)));}
_LL283:;}return res_typ;};};}_LL26E: {struct Cyc_Absyn_Const_e_struct*_tmp55D=(
struct Cyc_Absyn_Const_e_struct*)_tmp556;if(_tmp55D->tag != 0)goto _LL270;else{
_tmp55E=_tmp55D->f1;if((_tmp55E.String_c).tag != 8)goto _LL270;_tmp55F=(struct
_dyneither_ptr)(_tmp55E.String_c).val;}}_LL26F: {void*_tmp595=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp596=Cyc_Tcexp_tcExp(te,(void**)& _tmp595,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9FC;struct Cyc_Absyn_Upper_b_struct*_tmp9FB;
return Cyc_Absyn_atb_typ(_tmp596,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9FB=
_cycalloc(sizeof(*_tmp9FB)),((_tmp9FB[0]=((_tmp9FC.tag=1,((_tmp9FC.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9FC)))),_tmp9FB)))),Cyc_Absyn_false_conref);}_LL270: {struct Cyc_Absyn_Const_e_struct*
_tmp560=(struct Cyc_Absyn_Const_e_struct*)_tmp556;if(_tmp560->tag != 0)goto _LL272;
else{_tmp561=_tmp560->f1;if((_tmp561.Wstring_c).tag != 9)goto _LL272;_tmp562=(
struct _dyneither_ptr)(_tmp561.Wstring_c).val;}}_LL271: {void*_tmp599=Cyc_Absyn_atb_typ(
Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp59A=Cyc_Tcexp_tcExp(te,(void**)& _tmp599,e1);
struct Cyc_Absyn_Upper_b_struct _tmp9FF;struct Cyc_Absyn_Upper_b_struct*_tmp9FE;
return Cyc_Absyn_atb_typ(_tmp59A,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9FE=
_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=1,((_tmp9FF.f1=Cyc_Absyn_uint_exp(
1,0),_tmp9FF)))),_tmp9FE)))),Cyc_Absyn_false_conref);}_LL272:;_LL273: {void**
topt2=0;if(topt != 0){void*_tmp59D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp59F;void*_tmp5A0;void**_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;_LL289: {struct
Cyc_Absyn_PointerType_struct*_tmp59E=(struct Cyc_Absyn_PointerType_struct*)
_tmp59D;if(_tmp59E->tag != 5)goto _LL28B;else{_tmp59F=_tmp59E->f1;_tmp5A0=_tmp59F.elt_typ;
_tmp5A1=(void**)&(_tmp59E->f1).elt_typ;_tmp5A2=_tmp59F.elt_tq;}}_LL28A: topt2=(
void**)_tmp5A1;goto _LL288;_LL28B: {struct Cyc_Absyn_DatatypeType_struct*_tmp5A3=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp59D;if(_tmp5A3->tag != 3)goto _LL28D;}
_LL28C:{void**_tmpA00;topt2=((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00[0]=*
topt,_tmpA00))));}goto _LL288;_LL28D:;_LL28E: goto _LL288;_LL288:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmpA0A;struct Cyc_Absyn_PtrAtts
_tmpA09;struct Cyc_Absyn_PtrInfo _tmpA08;struct Cyc_Absyn_PointerType_struct*
_tmpA07;void*res_typ=(void*)((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07[0]=((
_tmpA0A.tag=5,((_tmpA0A.f1=((_tmpA08.elt_typ=telt,((_tmpA08.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpA08.ptr_atts=((_tmpA09.rgn=rgn,((_tmpA09.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmpA09.bounds=Cyc_Absyn_bounds_one_conref,((_tmpA09.zero_term=
Cyc_Absyn_false_conref,((_tmpA09.ptrloc=0,_tmpA09)))))))))),_tmpA08)))))),
_tmpA0A)))),_tmpA07))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmpA0B;e->topt=((_tmpA0B=_cycalloc(
sizeof(*_tmpA0B)),((_tmpA0B->v=res_typ,_tmpA0B))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL267:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp5AA=t;
struct Cyc_Absyn_ArrayInfo _tmp5AC;void*_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AE;
struct Cyc_Absyn_Exp*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5B0;_LL290: {struct Cyc_Absyn_ArrayType_struct*
_tmp5AB=(struct Cyc_Absyn_ArrayType_struct*)_tmp5AA;if(_tmp5AB->tag != 8)goto
_LL292;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=_tmp5AC.elt_type;_tmp5AE=_tmp5AC.tq;
_tmp5AF=_tmp5AC.num_elts;_tmp5B0=_tmp5AC.zero_term;}}_LL291: {void*_tmp5B2;
struct _tuple14 _tmp5B1=Cyc_Tcutil_addressof_props(te,e);_tmp5B2=_tmp5B1.f2;{
struct Cyc_Absyn_Upper_b_struct _tmpA0E;struct Cyc_Absyn_Upper_b_struct*_tmpA0D;
void*_tmp5B3=_tmp5AF == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmpA0D=
_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0E.tag=1,((_tmpA0E.f1=(struct Cyc_Absyn_Exp*)
_tmp5AF,_tmpA0E)))),_tmpA0D))));t=Cyc_Absyn_atb_typ(_tmp5AD,_tmp5B2,_tmp5AE,
_tmp5B3,_tmp5B0);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL292:;_LL293: return t;_LL28F:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle _tmp5B6=_new_region("temp");struct _RegionHandle*temp=& _tmp5B6;
_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(
temp,e)){const char*_tmpA11;void*_tmpA10;(_tmpA10=0,Cyc_Tcutil_terr(e->loc,((
_tmpA11="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmpA11,sizeof(char),49))),_tag_dyneither(_tmpA10,sizeof(void*),0)));}{void*
_tmp5B9=e->r;union Cyc_Absyn_Cnst _tmp5BD;struct _dyneither_ptr _tmp5BE;union Cyc_Absyn_Cnst
_tmp5C0;struct _dyneither_ptr _tmp5C1;_LL295: {struct Cyc_Absyn_Array_e_struct*
_tmp5BA=(struct Cyc_Absyn_Array_e_struct*)_tmp5B9;if(_tmp5BA->tag != 27)goto _LL297;}
_LL296: goto _LL298;_LL297: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5BB=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp5B9;if(_tmp5BB->tag != 28)goto _LL299;}
_LL298: goto _LL29A;_LL299: {struct Cyc_Absyn_Const_e_struct*_tmp5BC=(struct Cyc_Absyn_Const_e_struct*)
_tmp5B9;if(_tmp5BC->tag != 0)goto _LL29B;else{_tmp5BD=_tmp5BC->f1;if((_tmp5BD.Wstring_c).tag
!= 9)goto _LL29B;_tmp5BE=(struct _dyneither_ptr)(_tmp5BD.Wstring_c).val;}}_LL29A:
goto _LL29C;_LL29B: {struct Cyc_Absyn_Const_e_struct*_tmp5BF=(struct Cyc_Absyn_Const_e_struct*)
_tmp5B9;if(_tmp5BF->tag != 0)goto _LL29D;else{_tmp5C0=_tmp5BF->f1;if((_tmp5C0.String_c).tag
!= 8)goto _LL29D;_tmp5C1=(struct _dyneither_ptr)(_tmp5C0.String_c).val;}}_LL29C: {
void*_tmp5C2=t;_npop_handler(0);return _tmp5C2;}_LL29D:;_LL29E: t=Cyc_Tcutil_compress(
t);{void*_tmp5C3=t;struct Cyc_Absyn_ArrayInfo _tmp5C5;void*_tmp5C6;struct Cyc_Absyn_Tqual
_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C8;union Cyc_Absyn_Constraint*_tmp5C9;_LL2A0: {
struct Cyc_Absyn_ArrayType_struct*_tmp5C4=(struct Cyc_Absyn_ArrayType_struct*)
_tmp5C3;if(_tmp5C4->tag != 8)goto _LL2A2;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C5.elt_type;
_tmp5C7=_tmp5C5.tq;_tmp5C8=_tmp5C5.num_elts;_tmp5C9=_tmp5C5.zero_term;}}_LL2A1: {
void*_tmp5CB;struct _tuple14 _tmp5CA=Cyc_Tcutil_addressof_props(te,e);_tmp5CB=
_tmp5CA.f2;{struct Cyc_Absyn_Upper_b_struct _tmpA14;struct Cyc_Absyn_Upper_b_struct*
_tmpA13;void*b=_tmp5C8 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmpA13=
_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA14.tag=1,((_tmpA14.f1=(struct Cyc_Absyn_Exp*)
_tmp5C8,_tmpA14)))),_tmpA13))));t=Cyc_Absyn_atb_typ(_tmp5C6,_tmp5CB,_tmp5C7,b,
_tmp5C9);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{void*
_tmp5CC=t;_npop_handler(0);return _tmp5CC;};};}_LL2A2:;_LL2A3: {void*_tmp5CF=t;
_npop_handler(0);return _tmp5CF;}_LL29F:;};_LL294:;};;_pop_region(temp);}static
void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){{void*_tmp5D0=e->r;struct Cyc_Absyn_Exp*_tmp5D2;_LL2A5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp5D1=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp5D0;if(_tmp5D1->tag != 13)
goto _LL2A7;else{_tmp5D2=_tmp5D1->f1;}}_LL2A6: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp5D2);((struct Cyc_Core_Opt*)_check_null(_tmp5D2->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp5D2->topt))->v,0);e->topt=_tmp5D2->topt;
goto _LL2A4;_LL2A7:;_LL2A8: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp5D3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp5D5;void*_tmp5D6;struct Cyc_Absyn_Tqual
_tmp5D7;struct Cyc_Absyn_PtrAtts _tmp5D8;void*_tmp5D9;union Cyc_Absyn_Constraint*
_tmp5DA;union Cyc_Absyn_Constraint*_tmp5DB;union Cyc_Absyn_Constraint*_tmp5DC;
_LL2AA: {struct Cyc_Absyn_PointerType_struct*_tmp5D4=(struct Cyc_Absyn_PointerType_struct*)
_tmp5D3;if(_tmp5D4->tag != 5)goto _LL2AC;else{_tmp5D5=_tmp5D4->f1;_tmp5D6=_tmp5D5.elt_typ;
_tmp5D7=_tmp5D5.elt_tq;_tmp5D8=_tmp5D5.ptr_atts;_tmp5D9=_tmp5D8.rgn;_tmp5DA=
_tmp5D8.nullable;_tmp5DB=_tmp5D8.bounds;_tmp5DC=_tmp5D8.zero_term;}}_LL2AB:{void*
_tmp5DD=Cyc_Tcutil_compress(_tmp5D6);struct Cyc_Absyn_FnInfo _tmp5DF;struct Cyc_List_List*
_tmp5E0;struct Cyc_Core_Opt*_tmp5E1;struct Cyc_Absyn_Tqual _tmp5E2;void*_tmp5E3;
struct Cyc_List_List*_tmp5E4;int _tmp5E5;struct Cyc_Absyn_VarargInfo*_tmp5E6;struct
Cyc_List_List*_tmp5E7;struct Cyc_List_List*_tmp5E8;_LL2AF: {struct Cyc_Absyn_FnType_struct*
_tmp5DE=(struct Cyc_Absyn_FnType_struct*)_tmp5DD;if(_tmp5DE->tag != 9)goto _LL2B1;
else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DF.tvars;_tmp5E1=_tmp5DF.effect;_tmp5E2=
_tmp5DF.ret_tqual;_tmp5E3=_tmp5DF.ret_typ;_tmp5E4=_tmp5DF.args;_tmp5E5=_tmp5DF.c_varargs;
_tmp5E6=_tmp5DF.cyc_varargs;_tmp5E7=_tmp5DF.rgn_po;_tmp5E8=_tmp5DF.attributes;}}
_LL2B0: if(_tmp5E0 != 0){struct _RegionHandle _tmp5E9=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp5E9;_push_region(rgn);{struct _tuple12 _tmpA15;struct
_tuple12 _tmp5EA=(_tmpA15.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA15.f2=rgn,
_tmpA15)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct
_tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp5EA,_tmp5E0);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmpA1B;struct Cyc_Absyn_FnInfo
_tmpA1A;struct Cyc_Absyn_FnType_struct*_tmpA19;void*ftyp=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1B.tag=
9,((_tmpA1B.f1=((_tmpA1A.tvars=0,((_tmpA1A.effect=_tmp5E1,((_tmpA1A.ret_tqual=
_tmp5E2,((_tmpA1A.ret_typ=_tmp5E3,((_tmpA1A.args=_tmp5E4,((_tmpA1A.c_varargs=
_tmp5E5,((_tmpA1A.cyc_varargs=_tmp5E6,((_tmpA1A.rgn_po=_tmp5E7,((_tmpA1A.attributes=
_tmp5E8,_tmpA1A)))))))))))))))))),_tmpA1B)))),_tmpA19)))));struct Cyc_Absyn_PointerType_struct
_tmpA25;struct Cyc_Absyn_PtrAtts _tmpA24;struct Cyc_Absyn_PtrInfo _tmpA23;struct Cyc_Absyn_PointerType_struct*
_tmpA22;struct Cyc_Absyn_PointerType_struct*_tmp5EB=(_tmpA22=_cycalloc(sizeof(*
_tmpA22)),((_tmpA22[0]=((_tmpA25.tag=5,((_tmpA25.f1=((_tmpA23.elt_typ=ftyp,((
_tmpA23.elt_tq=_tmp5D7,((_tmpA23.ptr_atts=((_tmpA24.rgn=_tmp5D9,((_tmpA24.nullable=
_tmp5DA,((_tmpA24.bounds=_tmp5DB,((_tmpA24.zero_term=_tmp5DC,((_tmpA24.ptrloc=0,
_tmpA24)))))))))),_tmpA23)))))),_tmpA25)))),_tmpA22)));struct Cyc_Absyn_Exp*
_tmp5EC=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmpA28;
struct Cyc_Absyn_Instantiate_e_struct*_tmpA27;e->r=(void*)((_tmpA27=_cycalloc(
sizeof(*_tmpA27)),((_tmpA27[0]=((_tmpA28.tag=14,((_tmpA28.f1=_tmp5EC,((_tmpA28.f2=
ts,_tmpA28)))))),_tmpA27))));}{struct Cyc_Core_Opt*_tmpA29;e->topt=((_tmpA29=
_cycalloc(sizeof(*_tmpA29)),((_tmpA29->v=(void*)_tmp5EB,_tmpA29))));};};
_pop_region(rgn);}goto _LL2AE;_LL2B1:;_LL2B2: goto _LL2AE;_LL2AE:;}goto _LL2A9;
_LL2AC:;_LL2AD: goto _LL2A9;_LL2A9:;}goto _LL2A4;_LL2A4:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*
alias_arg_exps){struct _RegionHandle _tmp5F8=_new_region("r");struct _RegionHandle*
r=& _tmp5F8;_push_region(r);{struct Cyc_List_List*_tmp5F9=0;{void*_tmp5FA=fn_exp->r;
struct Cyc_List_List*_tmp5FC;_LL2B4: {struct Cyc_Absyn_FnCall_e_struct*_tmp5FB=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp5FA;if(_tmp5FB->tag != 11)goto _LL2B6;else{
_tmp5FC=_tmp5FB->f2;}}_LL2B5:{void*_tmp5FD=e->r;struct Cyc_List_List*_tmp5FF;
_LL2B9: {struct Cyc_Absyn_FnCall_e_struct*_tmp5FE=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5FD;if(_tmp5FE->tag != 11)goto _LL2BB;else{_tmp5FF=_tmp5FE->f2;}}_LL2BA: {
struct Cyc_List_List*_tmp600=alias_arg_exps;int _tmp601=0;while(_tmp600 != 0){
while(_tmp601 != (int)_tmp600->hd){if(_tmp5FC == 0){struct Cyc_Int_pa_struct _tmpA33;
struct Cyc_Int_pa_struct _tmpA32;void*_tmpA31[2];const char*_tmpA30;void*_tmpA2F;(
_tmpA2F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmpA33.tag=1,((_tmpA33.f1=(unsigned long)((int)_tmp600->hd),((
_tmpA32.tag=1,((_tmpA32.f1=(unsigned long)_tmp601,((_tmpA31[0]=& _tmpA32,((
_tmpA31[1]=& _tmpA33,Cyc_aprintf(((_tmpA30="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmpA30,sizeof(char),36))),_tag_dyneither(_tmpA31,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmpA2F,sizeof(void*),0)));}++ _tmp601;_tmp5FC=_tmp5FC->tl;_tmp5FF=((
struct Cyc_List_List*)_check_null(_tmp5FF))->tl;}{struct Cyc_Absyn_Decl*_tmp608;
struct Cyc_Absyn_Exp*_tmp609;struct _tuple11 _tmp607=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5FC))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5FF))->hd)->topt))->v));_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;
_tmp5FC->hd=(void*)_tmp609;{struct Cyc_List_List*_tmpA34;_tmp5F9=((_tmpA34=
_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34->hd=_tmp608,((_tmpA34->tl=_tmp5F9,
_tmpA34))))));}_tmp600=_tmp600->tl;};}goto _LL2B8;}_LL2BB:;_LL2BC: {const char*
_tmpA37;void*_tmpA36;(_tmpA36=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA37="not a function call!",
_tag_dyneither(_tmpA37,sizeof(char),21))),_tag_dyneither(_tmpA36,sizeof(void*),0)));}
_LL2B8:;}goto _LL2B3;_LL2B6:;_LL2B7: {const char*_tmpA3A;void*_tmpA39;(_tmpA39=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpA3A="not a function call!",_tag_dyneither(_tmpA3A,sizeof(char),21))),
_tag_dyneither(_tmpA39,sizeof(void*),0)));}_LL2B3:;}while(_tmp5F9 != 0){struct Cyc_Absyn_Decl*
_tmp60F=(struct Cyc_Absyn_Decl*)_tmp5F9->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp60F,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp5F9=_tmp5F9->tl;}e->topt=
0;e->r=fn_exp->r;};_pop_region(r);}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){unsigned int loc=e->loc;void*t;{void*_tmp610=
e->r;struct Cyc_Absyn_Exp*_tmp612;struct _tuple1*_tmp614;struct Cyc_Absyn_Exp*
_tmp616;struct Cyc_List_List*_tmp617;struct Cyc_Core_Opt*_tmp619;struct Cyc_List_List*
_tmp61A;union Cyc_Absyn_Cnst _tmp61C;union Cyc_Absyn_Cnst*_tmp61D;struct _tuple1*
_tmp61F;void*_tmp620;enum Cyc_Absyn_Primop _tmp622;struct Cyc_List_List*_tmp623;
struct Cyc_Absyn_Exp*_tmp625;enum Cyc_Absyn_Incrementor _tmp626;struct Cyc_Absyn_Exp*
_tmp628;struct Cyc_Core_Opt*_tmp629;struct Cyc_Absyn_Exp*_tmp62A;struct Cyc_Absyn_Exp*
_tmp62C;struct Cyc_Absyn_Exp*_tmp62D;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Exp*
_tmp630;struct Cyc_Absyn_Exp*_tmp631;struct Cyc_Absyn_Exp*_tmp633;struct Cyc_Absyn_Exp*
_tmp634;struct Cyc_Absyn_Exp*_tmp636;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Exp*
_tmp639;struct Cyc_List_List*_tmp63A;struct Cyc_Absyn_VarargCallInfo*_tmp63B;
struct Cyc_Absyn_VarargCallInfo**_tmp63C;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*
_tmp640;struct Cyc_List_List*_tmp641;void*_tmp643;struct Cyc_Absyn_Exp*_tmp644;
enum Cyc_Absyn_Coercion _tmp645;enum Cyc_Absyn_Coercion*_tmp646;struct Cyc_Absyn_Exp*
_tmp648;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp**_tmp64B;struct Cyc_Absyn_Exp*
_tmp64C;struct Cyc_Absyn_Exp*_tmp64E;void*_tmp650;void*_tmp652;void*_tmp653;
struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Exp*_tmp657;struct _dyneither_ptr*
_tmp658;int _tmp659;int*_tmp65A;int _tmp65B;int*_tmp65C;struct Cyc_Absyn_Exp*
_tmp65E;struct _dyneither_ptr*_tmp65F;int _tmp660;int*_tmp661;int _tmp662;int*
_tmp663;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_Absyn_Exp*_tmp666;struct Cyc_List_List*
_tmp668;struct _tuple9*_tmp66A;struct Cyc_List_List*_tmp66B;struct Cyc_List_List*
_tmp66D;struct Cyc_Absyn_Stmt*_tmp66F;struct Cyc_Absyn_Vardecl*_tmp671;struct Cyc_Absyn_Exp*
_tmp672;struct Cyc_Absyn_Exp*_tmp673;int _tmp674;int*_tmp675;struct _tuple1*_tmp677;
struct _tuple1**_tmp678;struct Cyc_List_List*_tmp679;struct Cyc_List_List**_tmp67A;
struct Cyc_List_List*_tmp67B;struct Cyc_Absyn_Aggrdecl*_tmp67C;struct Cyc_Absyn_Aggrdecl**
_tmp67D;void*_tmp67F;struct Cyc_List_List*_tmp680;struct Cyc_List_List*_tmp682;
struct Cyc_Absyn_Datatypedecl*_tmp683;struct Cyc_Absyn_Datatypefield*_tmp684;
struct _tuple1*_tmp686;struct _tuple1**_tmp687;struct Cyc_Absyn_Enumdecl*_tmp688;
struct Cyc_Absyn_Enumfield*_tmp689;struct _tuple1*_tmp68B;struct _tuple1**_tmp68C;
void*_tmp68D;struct Cyc_Absyn_Enumfield*_tmp68E;struct Cyc_Absyn_MallocInfo _tmp690;
int _tmp691;int*_tmp692;struct Cyc_Absyn_Exp*_tmp693;struct Cyc_Absyn_Exp**_tmp694;
void**_tmp695;void***_tmp696;struct Cyc_Absyn_Exp*_tmp697;struct Cyc_Absyn_Exp**
_tmp698;int _tmp699;int*_tmp69A;struct Cyc_Absyn_Exp*_tmp69C;struct Cyc_Absyn_Exp*
_tmp69D;struct Cyc_Absyn_Exp*_tmp69F;struct _dyneither_ptr*_tmp6A0;void*_tmp6A2;
_LL2BE: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp611=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp610;if(_tmp611->tag != 13)goto _LL2C0;else{_tmp612=_tmp611->f1;}}_LL2BF: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp612);return;_LL2C0: {struct Cyc_Absyn_UnknownId_e_struct*_tmp613=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp610;if(_tmp613->tag != 2)goto _LL2C2;else{
_tmp614=_tmp613->f1;}}_LL2C1: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp614);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2C2: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp615=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp610;if(_tmp615->tag != 10)goto _LL2C4;else{
_tmp616=_tmp615->f1;_tmp617=_tmp615->f2;}}_LL2C3: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp616,_tmp617);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C4: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp618=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp610;if(_tmp618->tag != 36)
goto _LL2C6;else{_tmp619=_tmp618->f1;_tmp61A=_tmp618->f2;}}_LL2C5: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp61A);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2C6: {struct
Cyc_Absyn_Const_e_struct*_tmp61B=(struct Cyc_Absyn_Const_e_struct*)_tmp610;if(
_tmp61B->tag != 0)goto _LL2C8;else{_tmp61C=_tmp61B->f1;_tmp61D=(union Cyc_Absyn_Cnst*)&
_tmp61B->f1;}}_LL2C7: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp61D,e);goto _LL2BD;_LL2C8: {struct Cyc_Absyn_Var_e_struct*_tmp61E=(struct Cyc_Absyn_Var_e_struct*)
_tmp610;if(_tmp61E->tag != 1)goto _LL2CA;else{_tmp61F=_tmp61E->f1;_tmp620=(void*)
_tmp61E->f2;}}_LL2C9: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp61F,_tmp620);goto _LL2BD;
_LL2CA: {struct Cyc_Absyn_Primop_e_struct*_tmp621=(struct Cyc_Absyn_Primop_e_struct*)
_tmp610;if(_tmp621->tag != 3)goto _LL2CC;else{_tmp622=_tmp621->f1;_tmp623=_tmp621->f2;}}
_LL2CB: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp622,_tmp623);goto _LL2BD;_LL2CC: {
struct Cyc_Absyn_Increment_e_struct*_tmp624=(struct Cyc_Absyn_Increment_e_struct*)
_tmp610;if(_tmp624->tag != 5)goto _LL2CE;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;}}
_LL2CD: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp625,_tmp626);goto _LL2BD;_LL2CE: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp627=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp610;if(_tmp627->tag != 4)goto _LL2D0;else{_tmp628=_tmp627->f1;_tmp629=_tmp627->f2;
_tmp62A=_tmp627->f3;}}_LL2CF: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp628,_tmp629,
_tmp62A);goto _LL2BD;_LL2D0: {struct Cyc_Absyn_Conditional_e_struct*_tmp62B=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp610;if(_tmp62B->tag != 6)goto _LL2D2;
else{_tmp62C=_tmp62B->f1;_tmp62D=_tmp62B->f2;_tmp62E=_tmp62B->f3;}}_LL2D1: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp62C,_tmp62D,_tmp62E);goto _LL2BD;_LL2D2: {struct Cyc_Absyn_And_e_struct*
_tmp62F=(struct Cyc_Absyn_And_e_struct*)_tmp610;if(_tmp62F->tag != 7)goto _LL2D4;
else{_tmp630=_tmp62F->f1;_tmp631=_tmp62F->f2;}}_LL2D3: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp630,_tmp631);goto _LL2BD;_LL2D4: {struct Cyc_Absyn_Or_e_struct*_tmp632=(struct
Cyc_Absyn_Or_e_struct*)_tmp610;if(_tmp632->tag != 8)goto _LL2D6;else{_tmp633=
_tmp632->f1;_tmp634=_tmp632->f2;}}_LL2D5: t=Cyc_Tcexp_tcOr(te,loc,_tmp633,_tmp634);
goto _LL2BD;_LL2D6: {struct Cyc_Absyn_SeqExp_e_struct*_tmp635=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp610;if(_tmp635->tag != 9)goto _LL2D8;else{_tmp636=_tmp635->f1;_tmp637=_tmp635->f2;}}
_LL2D7: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp636,_tmp637);goto _LL2BD;_LL2D8: {
struct Cyc_Absyn_FnCall_e_struct*_tmp638=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp610;if(_tmp638->tag != 11)goto _LL2DA;else{_tmp639=_tmp638->f1;_tmp63A=_tmp638->f2;
_tmp63B=_tmp638->f3;_tmp63C=(struct Cyc_Absyn_VarargCallInfo**)& _tmp638->f3;}}
_LL2D9: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp6A4;_push_handler(& _tmp6A4);{int _tmp6A6=0;if(setjmp(
_tmp6A4.handler))_tmp6A6=1;if(!_tmp6A6){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp6A5=(void*)_exn_thrown;void*_tmp6A8=_tmp6A5;struct
_dyneither_ptr _tmp6AA;_LL311: {struct Cyc_Core_Failure_struct*_tmp6A9=(struct Cyc_Core_Failure_struct*)
_tmp6A8;if(_tmp6A9->tag != Cyc_Core_Failure)goto _LL313;else{_tmp6AA=_tmp6A9->f1;}}
_LL312: ok=0;fn_exp=e;goto _LL310;_LL313:;_LL314:(void)_throw(_tmp6A8);_LL310:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp639,_tmp63A,_tmp63C,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2BD;}_LL2DA: {
struct Cyc_Absyn_Throw_e_struct*_tmp63D=(struct Cyc_Absyn_Throw_e_struct*)_tmp610;
if(_tmp63D->tag != 12)goto _LL2DC;else{_tmp63E=_tmp63D->f1;}}_LL2DB: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp63E);goto _LL2BD;_LL2DC: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp63F=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp610;if(_tmp63F->tag != 14)goto
_LL2DE;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL2DD: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp640,_tmp641);goto _LL2BD;_LL2DE: {struct Cyc_Absyn_Cast_e_struct*
_tmp642=(struct Cyc_Absyn_Cast_e_struct*)_tmp610;if(_tmp642->tag != 15)goto _LL2E0;
else{_tmp643=(void*)_tmp642->f1;_tmp644=_tmp642->f2;_tmp645=_tmp642->f4;_tmp646=(
enum Cyc_Absyn_Coercion*)& _tmp642->f4;}}_LL2DF: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp643,_tmp644,(enum Cyc_Absyn_Coercion*)_tmp646);goto _LL2BD;_LL2E0: {struct Cyc_Absyn_Address_e_struct*
_tmp647=(struct Cyc_Absyn_Address_e_struct*)_tmp610;if(_tmp647->tag != 16)goto
_LL2E2;else{_tmp648=_tmp647->f1;}}_LL2E1: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp648);goto _LL2BD;_LL2E2: {struct Cyc_Absyn_New_e_struct*_tmp649=(struct Cyc_Absyn_New_e_struct*)
_tmp610;if(_tmp649->tag != 17)goto _LL2E4;else{_tmp64A=_tmp649->f1;_tmp64B=(struct
Cyc_Absyn_Exp**)& _tmp649->f1;_tmp64C=_tmp649->f2;}}_LL2E3: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp64B,e,_tmp64C);goto _LL2BD;_LL2E4: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp64D=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp610;if(_tmp64D->tag != 19)goto
_LL2E6;else{_tmp64E=_tmp64D->f1;}}_LL2E5: {void*_tmp6AB=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp64E);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6AB);goto _LL2BD;}_LL2E6: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp64F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp610;if(_tmp64F->tag != 18)goto _LL2E8;else{_tmp650=(void*)_tmp64F->f1;}}_LL2E7:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp650);goto _LL2BD;_LL2E8: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp651=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp610;if(_tmp651->tag != 20)goto
_LL2EA;else{_tmp652=(void*)_tmp651->f1;_tmp653=(void*)_tmp651->f2;}}_LL2E9: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp652,_tmp653);goto _LL2BD;_LL2EA: {struct Cyc_Absyn_Deref_e_struct*
_tmp654=(struct Cyc_Absyn_Deref_e_struct*)_tmp610;if(_tmp654->tag != 21)goto _LL2EC;
else{_tmp655=_tmp654->f1;}}_LL2EB: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp655);goto
_LL2BD;_LL2EC: {struct Cyc_Absyn_AggrMember_e_struct*_tmp656=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp610;if(_tmp656->tag != 22)goto _LL2EE;else{_tmp657=_tmp656->f1;_tmp658=_tmp656->f2;
_tmp659=_tmp656->f3;_tmp65A=(int*)& _tmp656->f3;_tmp65B=_tmp656->f4;_tmp65C=(int*)&
_tmp656->f4;}}_LL2ED: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp657,_tmp658,
_tmp65A,_tmp65C);goto _LL2BD;_LL2EE: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp65D=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp610;if(_tmp65D->tag != 23)goto _LL2F0;
else{_tmp65E=_tmp65D->f1;_tmp65F=_tmp65D->f2;_tmp660=_tmp65D->f3;_tmp661=(int*)&
_tmp65D->f3;_tmp662=_tmp65D->f4;_tmp663=(int*)& _tmp65D->f4;}}_LL2EF: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp65E,_tmp65F,_tmp661,_tmp663);goto _LL2BD;_LL2F0: {struct Cyc_Absyn_Subscript_e_struct*
_tmp664=(struct Cyc_Absyn_Subscript_e_struct*)_tmp610;if(_tmp664->tag != 24)goto
_LL2F2;else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;}}_LL2F1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp665,_tmp666);goto _LL2BD;_LL2F2: {struct Cyc_Absyn_Tuple_e_struct*
_tmp667=(struct Cyc_Absyn_Tuple_e_struct*)_tmp610;if(_tmp667->tag != 25)goto _LL2F4;
else{_tmp668=_tmp667->f1;}}_LL2F3: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp668);goto
_LL2BD;_LL2F4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp669=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp610;if(_tmp669->tag != 26)goto _LL2F6;else{_tmp66A=_tmp669->f1;_tmp66B=_tmp669->f2;}}
_LL2F5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp66A,_tmp66B);goto _LL2BD;_LL2F6: {
struct Cyc_Absyn_Array_e_struct*_tmp66C=(struct Cyc_Absyn_Array_e_struct*)_tmp610;
if(_tmp66C->tag != 27)goto _LL2F8;else{_tmp66D=_tmp66C->f1;}}_LL2F7: {void**
elt_topt=0;struct Cyc_Absyn_Tqual*elt_tqopt=0;int zero_term=0;if(topt != 0){void*
_tmp6AC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp6AE;void*_tmp6AF;
void**_tmp6B0;struct Cyc_Absyn_Tqual _tmp6B1;struct Cyc_Absyn_Tqual*_tmp6B2;union
Cyc_Absyn_Constraint*_tmp6B3;_LL316: {struct Cyc_Absyn_ArrayType_struct*_tmp6AD=(
struct Cyc_Absyn_ArrayType_struct*)_tmp6AC;if(_tmp6AD->tag != 8)goto _LL318;else{
_tmp6AE=_tmp6AD->f1;_tmp6AF=_tmp6AE.elt_type;_tmp6B0=(void**)&(_tmp6AD->f1).elt_type;
_tmp6B1=_tmp6AE.tq;_tmp6B2=(struct Cyc_Absyn_Tqual*)&(_tmp6AD->f1).tq;_tmp6B3=
_tmp6AE.zero_term;}}_LL317: elt_topt=(void**)_tmp6B0;elt_tqopt=(struct Cyc_Absyn_Tqual*)
_tmp6B2;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp6B3);goto _LL315;_LL318:;_LL319: goto _LL315;_LL315:;}t=Cyc_Tcexp_tcArray(te,
loc,elt_topt,elt_tqopt,zero_term,_tmp66D);goto _LL2BD;}_LL2F8: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp66E=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp610;if(_tmp66E->tag != 37)goto
_LL2FA;else{_tmp66F=_tmp66E->f1;}}_LL2F9: t=Cyc_Tcexp_tcStmtExp(te,loc,topt,
_tmp66F);goto _LL2BD;_LL2FA: {struct Cyc_Absyn_Comprehension_e_struct*_tmp670=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp610;if(_tmp670->tag != 28)goto _LL2FC;
else{_tmp671=_tmp670->f1;_tmp672=_tmp670->f2;_tmp673=_tmp670->f3;_tmp674=_tmp670->f4;
_tmp675=(int*)& _tmp670->f4;}}_LL2FB: t=Cyc_Tcexp_tcComprehension(te,loc,topt,
_tmp671,_tmp672,_tmp673,_tmp675);goto _LL2BD;_LL2FC: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp676=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp610;if(_tmp676->tag != 29)goto
_LL2FE;else{_tmp677=_tmp676->f1;_tmp678=(struct _tuple1**)& _tmp676->f1;_tmp679=
_tmp676->f2;_tmp67A=(struct Cyc_List_List**)& _tmp676->f2;_tmp67B=_tmp676->f3;
_tmp67C=_tmp676->f4;_tmp67D=(struct Cyc_Absyn_Aggrdecl**)& _tmp676->f4;}}_LL2FD: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp678,_tmp67A,_tmp67B,_tmp67D);goto _LL2BD;
_LL2FE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp67E=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp610;if(_tmp67E->tag != 30)goto _LL300;else{_tmp67F=(void*)_tmp67E->f1;_tmp680=
_tmp67E->f2;}}_LL2FF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp67F,_tmp680);goto _LL2BD;
_LL300: {struct Cyc_Absyn_Datatype_e_struct*_tmp681=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp610;if(_tmp681->tag != 31)goto _LL302;else{_tmp682=_tmp681->f1;_tmp683=_tmp681->f2;
_tmp684=_tmp681->f3;}}_LL301: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp682,
_tmp683,_tmp684);goto _LL2BD;_LL302: {struct Cyc_Absyn_Enum_e_struct*_tmp685=(
struct Cyc_Absyn_Enum_e_struct*)_tmp610;if(_tmp685->tag != 32)goto _LL304;else{
_tmp686=_tmp685->f1;_tmp687=(struct _tuple1**)& _tmp685->f1;_tmp688=_tmp685->f2;
_tmp689=_tmp685->f3;}}_LL303:*_tmp687=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp689))->name;{struct Cyc_Absyn_EnumType_struct _tmpA3D;struct Cyc_Absyn_EnumType_struct*
_tmpA3C;t=(void*)((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C[0]=((_tmpA3D.tag=
13,((_tmpA3D.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp688))->name,((
_tmpA3D.f2=_tmp688,_tmpA3D)))))),_tmpA3C))));}goto _LL2BD;_LL304: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp68A=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp610;if(_tmp68A->tag != 33)goto
_LL306;else{_tmp68B=_tmp68A->f1;_tmp68C=(struct _tuple1**)& _tmp68A->f1;_tmp68D=(
void*)_tmp68A->f2;_tmp68E=_tmp68A->f3;}}_LL305:*_tmp68C=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp68E))->name;t=_tmp68D;goto _LL2BD;_LL306: {struct Cyc_Absyn_Malloc_e_struct*
_tmp68F=(struct Cyc_Absyn_Malloc_e_struct*)_tmp610;if(_tmp68F->tag != 34)goto
_LL308;else{_tmp690=_tmp68F->f1;_tmp691=_tmp690.is_calloc;_tmp692=(int*)&(
_tmp68F->f1).is_calloc;_tmp693=_tmp690.rgn;_tmp694=(struct Cyc_Absyn_Exp**)&(
_tmp68F->f1).rgn;_tmp695=_tmp690.elt_type;_tmp696=(void***)&(_tmp68F->f1).elt_type;
_tmp697=_tmp690.num_elts;_tmp698=(struct Cyc_Absyn_Exp**)&(_tmp68F->f1).num_elts;
_tmp699=_tmp690.fat_result;_tmp69A=(int*)&(_tmp68F->f1).fat_result;}}_LL307: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp694,_tmp696,_tmp698,_tmp692,_tmp69A);goto
_LL2BD;_LL308: {struct Cyc_Absyn_Swap_e_struct*_tmp69B=(struct Cyc_Absyn_Swap_e_struct*)
_tmp610;if(_tmp69B->tag != 35)goto _LL30A;else{_tmp69C=_tmp69B->f1;_tmp69D=_tmp69B->f2;}}
_LL309: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp69C,_tmp69D);goto _LL2BD;_LL30A: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp69E=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp610;if(_tmp69E->tag != 38)goto _LL30C;else{_tmp69F=_tmp69E->f1;_tmp6A0=_tmp69E->f2;}}
_LL30B: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp69F,_tmp6A0);goto _LL2BD;_LL30C: {
struct Cyc_Absyn_Valueof_e_struct*_tmp6A1=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp610;if(_tmp6A1->tag != 39)goto _LL30E;else{_tmp6A2=(void*)_tmp6A1->f1;}}_LL30D:
if(!te->allow_valueof){const char*_tmpA40;void*_tmpA3F;(_tmpA3F=0,Cyc_Tcutil_terr(
e->loc,((_tmpA40="valueof(-) can only occur within types",_tag_dyneither(_tmpA40,
sizeof(char),39))),_tag_dyneither(_tmpA3F,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2BD;_LL30E: {struct Cyc_Absyn_Asm_e_struct*_tmp6A3=(struct Cyc_Absyn_Asm_e_struct*)
_tmp610;if(_tmp6A3->tag != 40)goto _LL2BD;}_LL30F:{const char*_tmpA43;void*_tmpA42;(
_tmpA42=0,Cyc_Tcutil_terr(e->loc,((_tmpA43="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmpA43,sizeof(char),50))),_tag_dyneither(_tmpA42,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2BD:;}{struct Cyc_Core_Opt*
_tmpA44;e->topt=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->v=t,_tmpA44))));};}
