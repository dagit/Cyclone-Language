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
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp65B;(
_tmp65B=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
msg,ap),_tag_dyneither(_tmp65B,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2:{struct Cyc_Absyn_Var_e_struct
_tmp65E;struct Cyc_Absyn_Var_e_struct*_tmp65D;e->r=(void*)((_tmp65D=_cycalloc(
sizeof(*_tmp65D)),((_tmp65D[0]=((_tmp65E.tag=1,((_tmp65E.f1=q,((_tmp65E.f2=(void*)
_tmp7,_tmp65E)))))),_tmp65D))));}goto _LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*
_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)_tmp5;if(_tmp8->tag != 3)goto _LL5;else{
_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4:{struct Cyc_Absyn_Enum_e_struct _tmp661;
struct Cyc_Absyn_Enum_e_struct*_tmp660;e->r=(void*)((_tmp660=_cycalloc(sizeof(*
_tmp660)),((_tmp660[0]=((_tmp661.tag=32,((_tmp661.f1=q,((_tmp661.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp9,((_tmp661.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp661)))))))),_tmp660))));}
goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6:{struct Cyc_Absyn_AnonEnum_e_struct _tmp664;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp663;e->r=(void*)((_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663[0]=((_tmp664.tag=
33,((_tmp664.f1=q,((_tmp664.f2=(void*)_tmpC,((_tmp664.f3=(struct Cyc_Absyn_Enumfield*)
_tmpD,_tmp664)))))))),_tmp663))));}goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpE=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;
else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8:{struct Cyc_Absyn_Datatype_e_struct
_tmp667;struct Cyc_Absyn_Datatype_e_struct*_tmp666;e->r=(void*)((_tmp666=
_cycalloc(sizeof(*_tmp666)),((_tmp666[0]=((_tmp667.tag=31,((_tmp667.f1=0,((
_tmp667.f2=_tmpF,((_tmp667.f3=_tmp10,_tmp667)))))))),_tmp666))));}goto _LL0;_LL9: {
struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)_tmp5;if(
_tmp11->tag != 1)goto _LL0;}_LLA:{const char*_tmp66B;void*_tmp66A[1];struct Cyc_String_pa_struct
_tmp669;(_tmp669.tag=0,((_tmp669.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp66A[0]=& _tmp669,Cyc_Tcutil_terr(e->loc,((
_tmp66B="bad occurrence of type name %s",_tag_dyneither(_tmp66B,sizeof(char),31))),
_tag_dyneither(_tmp66A,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp66E;struct Cyc_Absyn_Var_e_struct*_tmp66D;e->r=(void*)((_tmp66D=_cycalloc(
sizeof(*_tmp66D)),((_tmp66D[0]=((_tmp66E.tag=1,((_tmp66E.f1=q,((_tmp66E.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp66E)))))),_tmp66D))));}goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp671;struct Cyc_Absyn_Var_e_struct*_tmp670;e->r=(void*)((_tmp670=_cycalloc(
sizeof(*_tmp670)),((_tmp670[0]=((_tmp671.tag=1,((_tmp671.f1=q,((_tmp671.f2=(void*)((
void*)& Cyc_Absyn_Unresolved_b_val),_tmp671)))))),_tmp670))));}goto _LLB;_LLE:;
_LLF:(void)_throw(_tmp20);_LLB:;}};}struct _tuple17{struct Cyc_List_List*f1;struct
Cyc_Absyn_Exp*f2;};static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*
e){struct _tuple17*_tmp672;return(_tmp672=_cycalloc(sizeof(*_tmp672)),((_tmp672->f1=
0,((_tmp672->f2=e,_tmp672)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*
_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17:{struct Cyc_Absyn_FnCall_e_struct _tmp675;struct
Cyc_Absyn_FnCall_e_struct*_tmp674;e->r=(void*)((_tmp674=_cycalloc(sizeof(*
_tmp674)),((_tmp674[0]=((_tmp675.tag=11,((_tmp675.f1=e1,((_tmp675.f2=es,((
_tmp675.f3=0,_tmp675)))))))),_tmp674))));}_npop_handler(0);return;_LL18: {struct
Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;
if(_tmp2F->tag != 2)goto _LL1A;else{_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19:
if(_tmp31->typs == 0){const char*_tmp679;void*_tmp678[1];struct Cyc_String_pa_struct
_tmp677;(_tmp677.tag=0,((_tmp677.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp31->name)),((_tmp678[0]=& _tmp677,Cyc_Tcutil_terr(e->loc,((
_tmp679="%s is a constant, not a function",_tag_dyneither(_tmp679,sizeof(char),
33))),_tag_dyneither(_tmp678,sizeof(void*),1)))))));}{struct Cyc_Absyn_Datatype_e_struct
_tmp67C;struct Cyc_Absyn_Datatype_e_struct*_tmp67B;e->r=(void*)((_tmp67B=
_cycalloc(sizeof(*_tmp67B)),((_tmp67B[0]=((_tmp67C.tag=31,((_tmp67C.f1=es,((
_tmp67C.f2=_tmp30,((_tmp67C.f3=_tmp31,_tmp67C)))))))),_tmp67B))));}_npop_handler(
0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=_tmp32->f1;}}_LL1B: {struct Cyc_List_List*
_tmp3D=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Aggregate_e_struct
_tmp67F;struct Cyc_Absyn_Aggregate_e_struct*_tmp67E;e->r=(void*)((_tmp67E=
_cycalloc(sizeof(*_tmp67E)),((_tmp67E[0]=((_tmp67F.tag=29,((_tmp67F.f1=_tmp33->name,((
_tmp67F.f2=0,((_tmp67F.f3=_tmp3D,((_tmp67F.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp33,
_tmp67F)))))))))),_tmp67E))));}_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:{const char*_tmp683;void*_tmp682[1];
struct Cyc_String_pa_struct _tmp681;(_tmp681.tag=0,((_tmp681.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp27)),((_tmp682[
0]=& _tmp681,Cyc_Tcutil_terr(e->loc,((_tmp683="%s is an enum constructor, not a function",
_tag_dyneither(_tmp683,sizeof(char),42))),_tag_dyneither(_tmp682,sizeof(void*),1)))))));}
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp686;struct Cyc_Absyn_FnCall_e_struct*
_tmp685;e->r=(void*)((_tmp685=_cycalloc(sizeof(*_tmp685)),((_tmp685[0]=((_tmp686.tag=
11,((_tmp686.f1=e1,((_tmp686.f2=es,((_tmp686.f3=0,_tmp686)))))))),_tmp685))));}
return;_LL23:;_LL24:(void)_throw(_tmp44);_LL20:;}};}_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct
_tmp689;struct Cyc_Absyn_FnCall_e_struct*_tmp688;e->r=(void*)((_tmp688=_cycalloc(
sizeof(*_tmp688)),((_tmp688[0]=((_tmp689.tag=11,((_tmp689.f1=e1,((_tmp689.f2=es,((
_tmp689.f3=0,_tmp689)))))))),_tmp688))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp68C;
struct Cyc_Absyn_Array_e_struct*_tmp68B;e->r=(void*)((_tmp68B=_cycalloc(sizeof(*
_tmp68B)),((_tmp68B[0]=((_tmp68C.tag=27,((_tmp68C.f1=des,_tmp68C)))),_tmp68B))));}
return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo _tmp51;void*
_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30: {const char*_tmp68F;void*_tmp68E;(_tmp68E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp68F="struct type not properly set",
_tag_dyneither(_tmp68F,sizeof(char),29))),_tag_dyneither(_tmp68E,sizeof(void*),0)));}
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: {struct Cyc_Absyn_Aggregate_e_struct
_tmp692;struct Cyc_Absyn_Aggregate_e_struct*_tmp691;e->r=(void*)((_tmp691=
_cycalloc(sizeof(*_tmp691)),((_tmp691[0]=((_tmp692.tag=29,((_tmp692.f1=_tmp58->name,((
_tmp692.f2=0,((_tmp692.f3=des,((_tmp692.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp58,
_tmp692)))))))))),_tmp691))));}_LL2E:;}goto _LL25;_LL28: {struct Cyc_Absyn_ArrayType_struct*
_tmp50=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C;if(_tmp50->tag != 9)goto _LL2A;
else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;_tmp53=_tmp51.tq;}}_LL29:{struct
Cyc_Absyn_Array_e_struct _tmp695;struct Cyc_Absyn_Array_e_struct*_tmp694;e->r=(
void*)((_tmp694=_cycalloc(sizeof(*_tmp694)),((_tmp694[0]=((_tmp695.tag=27,((
_tmp695.f1=des,_tmp695)))),_tmp694))));}goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp698;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp697;e->r=(void*)((_tmp697=_cycalloc(sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=
30,((_tmp698.f1=(void*)t,((_tmp698.f2=des,_tmp698)))))),_tmp697))));}goto _LL25;
_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp69B;struct Cyc_Absyn_Array_e_struct*
_tmp69A;e->r=(void*)((_tmp69A=_cycalloc(sizeof(*_tmp69A)),((_tmp69A[0]=((_tmp69B.tag=
27,((_tmp69B.f1=des,_tmp69B)))),_tmp69A))));}goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp6A0;void*_tmp69F[2];struct Cyc_String_pa_struct _tmp69E;
struct Cyc_String_pa_struct _tmp69D;(_tmp69D.tag=0,((_tmp69D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp69E.tag=0,((_tmp69E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp69F[0]=& _tmp69E,((_tmp69F[1]=& _tmp69D,Cyc_Tcutil_terr(
e->loc,((_tmp6A0="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp6A0,sizeof(char),53))),_tag_dyneither(_tmp69F,sizeof(void*),2)))))))))))));}{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6A1;struct
_tuple0 _tmp6C=(_tmp6A1.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v,((_tmp6A1.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v,
_tmp6A1)));void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A: {
struct _tuple0*_tmp6A4;struct Cyc_Tcexp_TestEnv _tmp6A3;return(_tmp6A3.eq=((_tmp6A4=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6A4)),((_tmp6A4->f1=_tmp6F,((
_tmp6A4->f2=_tmp72,_tmp6A4)))))),((_tmp6A3.isTrue=_tmp69 == Cyc_Absyn_Eq,_tmp6A3)));}
_LL3B:;_LL3C: goto _LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}{struct
Cyc_Tcexp_TestEnv _tmp6A5;return(_tmp6A5.eq=0,((_tmp6A5.isTrue=0,_tmp6A5)));};}
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
_tmp84 == 0))goto _LL5B;_LL5A:{struct Cyc_Absyn_Const_e_struct _tmp6A8;struct Cyc_Absyn_Const_e_struct*
_tmp6A7;e->r=(void*)((_tmp6A7=_cycalloc(sizeof(*_tmp6A7)),((_tmp6A7[0]=((_tmp6A8.tag=
0,((_tmp6A8.f1=Cyc_Absyn_Null_c,_tmp6A8)))),_tmp6A7))));}{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp6BB;
struct Cyc_Absyn_PtrAtts _tmp6BA;struct Cyc_Core_Opt*_tmp6B9;struct Cyc_Core_Opt*
_tmp6B8;struct Cyc_Absyn_PtrInfo _tmp6B7;struct Cyc_Absyn_PointerType_struct*
_tmp6B6;t=(void*)((_tmp6B6=_cycalloc(sizeof(*_tmp6B6)),((_tmp6B6[0]=((_tmp6BB.tag=
5,((_tmp6BB.f1=((_tmp6B7.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((
_tmp6B8=_cycalloc(sizeof(*_tmp6B8)),((_tmp6B8->v=_tmp99,_tmp6B8))))),((_tmp6B7.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6B7.ptr_atts=((_tmp6BA.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp6B9=_cycalloc(sizeof(*_tmp6B9)),((
_tmp6B9->v=_tmp99,_tmp6B9))))),((_tmp6BA.nullable=Cyc_Absyn_true_conref,((
_tmp6BA.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6BA.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6BA.ptrloc=0,_tmp6BA)))))))))),_tmp6B7)))))),_tmp6BB)))),_tmp6B6))));}goto
_LL50;};_LL5B: {struct Cyc_Absyn_TagType_struct*_tmp95=(struct Cyc_Absyn_TagType_struct*)
_tmp87;if(_tmp95->tag != 20)goto _LL5D;else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {
struct Cyc_Absyn_ValueofType_struct _tmp6BE;struct Cyc_Absyn_ValueofType_struct*
_tmp6BD;struct Cyc_Absyn_ValueofType_struct*_tmpA0=(_tmp6BD=_cycalloc(sizeof(*
_tmp6BD)),((_tmp6BD[0]=((_tmp6BE.tag=19,((_tmp6BE.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0),_tmp6BE)))),_tmp6BD)));if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){{const char*_tmp6C3;void*_tmp6C2[2];struct Cyc_String_pa_struct _tmp6C1;
struct Cyc_String_pa_struct _tmp6C0;(_tmp6C0.tag=0,((_tmp6C0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0)),((
_tmp6C1.tag=0,((_tmp6C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp96)),((_tmp6C2[0]=& _tmp6C1,((_tmp6C2[1]=& _tmp6C0,Cyc_Tcutil_terr(loc,((
_tmp6C3="expecting %s but found %s",_tag_dyneither(_tmp6C3,sizeof(char),26))),
_tag_dyneither(_tmp6C2,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}{
struct Cyc_Absyn_TagType_struct _tmp6C6;struct Cyc_Absyn_TagType_struct*_tmp6C5;t=(
void*)((_tmp6C5=_cycalloc(sizeof(*_tmp6C5)),((_tmp6C5[0]=((_tmp6C6.tag=20,((
_tmp6C6.f1=(void*)((void*)_tmpA0),_tmp6C6)))),_tmp6C5))));}goto _LL50;}_LL5D:;
_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto
_LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);{struct Cyc_Core_Opt*_tmp6C7;elen->topt=((
_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7->v=Cyc_Absyn_uint_typ,_tmp6C7))));}{
struct Cyc_Absyn_Upper_b_struct _tmp6CA;struct Cyc_Absyn_Upper_b_struct*_tmp6C9;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(
0),(void*)((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9[0]=((_tmp6CA.tag=1,((
_tmp6CA.f1=elen,_tmp6CA)))),_tmp6C9)))),Cyc_Absyn_true_conref);}if(topt != 0){
void*_tmpAC=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual
_tmpAF;_LL64: {struct Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAC;if(_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}
_LL65: return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)
elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {
struct Cyc_Absyn_PointerType_struct*_tmpB0=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp6CB;e->topt=((_tmp6CB=_cycalloc(sizeof(*
_tmp6CB)),((_tmp6CB->v=t,_tmp6CB))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp6CE;struct Cyc_Absyn_Upper_b_struct*
_tmp6CD;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(
void*)((_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CE.tag=1,((
_tmp6CE.f1=elen,_tmp6CE)))),_tmp6CD)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*
topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*
_tmp6CF;e->topt=((_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF->v=t,_tmp6CF))));}
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}}goto
_LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag != 1)
goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*_tmpB5=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Absyn_PointerType_struct _tmp6E2;struct Cyc_Absyn_PtrAtts _tmp6E1;
struct Cyc_Core_Opt*_tmp6E0;struct Cyc_Core_Opt*_tmp6DF;struct Cyc_Absyn_PtrInfo
_tmp6DE;struct Cyc_Absyn_PointerType_struct*_tmp6DD;t=(void*)((_tmp6DD=_cycalloc(
sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6E2.tag=5,((_tmp6E2.f1=((_tmp6DE.elt_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp6DF=_cycalloc(
sizeof(*_tmp6DF)),((_tmp6DF->v=_tmpB5,_tmp6DF))))),((_tmp6DE.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp6DE.ptr_atts=((_tmp6E1.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((
_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0->v=_tmpB5,_tmp6E0))))),((_tmp6E1.nullable=
Cyc_Absyn_true_conref,((_tmp6E1.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6E1.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp6E1.ptrloc=0,_tmp6E1)))))))))),_tmp6DE)))))),_tmp6E2)))),_tmp6DD))));}goto
_LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void*b){
void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;
struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}_LL6C: {const char*_tmp6E6;void*_tmp6E5[1];
struct Cyc_String_pa_struct _tmp6E4;return(_tmp6E4.tag=0,((_tmp6E4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6E5[0]=&
_tmp6E4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6E6="undeclared identifier: %s",
_tag_dyneither(_tmp6E6,sizeof(char),26))),_tag_dyneither(_tmp6E5,sizeof(void*),1)))))));}
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
else{_tmpC7=_tmpC6->f1;}}_LL76:{union Cyc_Absyn_Nmspace _tmp6E7;(*q).f1=(((_tmp6E7.Loc_n).val=
0,(((_tmp6E7.Loc_n).tag=4,_tmp6E7))));}if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79:{struct Cyc_Absyn_Valueof_e_struct _tmp6EA;struct Cyc_Absyn_Valueof_e_struct*
_tmp6E9;e->r=(void*)((_tmp6E9=_cycalloc(sizeof(*_tmp6E9)),((_tmp6E9[0]=((_tmp6EA.tag=
39,((_tmp6EA.f1=(void*)_tmpCE,_tmp6EA)))),_tmp6E9))));}goto _LL77;_LL7A:;_LL7B:
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
char*_tmp6ED;void*_tmp6EC;(_tmp6EC=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmpDC->hd)->loc,((_tmp6ED="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp6ED,sizeof(char),49))),_tag_dyneither(_tmp6EC,sizeof(void*),0)));}};}}
return;_LL7C:;}if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpE0 != 0;(((desc_types=desc_types->tl,
_tmpE0=_tmpE0->tl)),arg_cnt ++)){int alias_coercion=0;void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;Cyc_Tcexp_tcExp(te,(void**)&
t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){{const char*_tmp6F2;void*
_tmp6F1[2];struct Cyc_String_pa_struct _tmp6F0;struct Cyc_String_pa_struct _tmp6EF;(
_tmp6EF.tag=0,((_tmp6EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6F0.tag=0,((_tmp6F0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F1[
0]=& _tmp6F0,((_tmp6F1[1]=& _tmp6EF,Cyc_Tcutil_terr(e->loc,((_tmp6F2="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp6F2,sizeof(char),51))),_tag_dyneither(_tmp6F1,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp6F3;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp6F3=_cycalloc(sizeof(*_tmp6F3)),((
_tmp6F3->hd=(void*)arg_cnt,((_tmp6F3->tl=*alias_arg_exps,_tmp6F3))))));}{struct
_RegionHandle*_tmpE6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpE6,t) && !Cyc_Tcutil_is_noalias_path(_tmpE6,e)){const char*_tmp6F6;void*
_tmp6F5;(_tmp6F5=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6F6="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp6F6,sizeof(char),49))),_tag_dyneither(_tmp6F5,sizeof(void*),0)));}};}if(
desc_types != 0){const char*_tmp6F9;void*_tmp6F8;(_tmp6F8=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp6F9="too few arguments",_tag_dyneither(_tmp6F9,sizeof(char),18))),
_tag_dyneither(_tmp6F8,sizeof(void*),0)));}if(_tmpE0 != 0){const char*_tmp6FC;void*
_tmp6FB;(_tmp6FB=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((
_tmp6FC="too many arguments",_tag_dyneither(_tmp6FC,sizeof(char),19))),
_tag_dyneither(_tmp6FB,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus:
_LL84: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp700;void*_tmp6FF[1];struct Cyc_String_pa_struct
_tmp6FE;(_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp6FF[0]=& _tmp6FE,Cyc_Tcutil_terr(loc,((_tmp700="expecting arithmetic type but found %s",
_tag_dyneither(_tmp700,sizeof(char),39))),_tag_dyneither(_tmp6FF,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp704;void*_tmp703[1];struct Cyc_String_pa_struct _tmp702;(_tmp702.tag=
0,((_tmp702.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp703[0]=& _tmp702,Cyc_Tcutil_terr(loc,((_tmp704="expecting integral or * type but found %s",
_tag_dyneither(_tmp704,sizeof(char),42))),_tag_dyneither(_tmp703,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp708;void*_tmp707[1];struct Cyc_String_pa_struct _tmp706;(_tmp706.tag=
0,((_tmp706.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp707[0]=& _tmp706,Cyc_Tcutil_terr(loc,((_tmp708="expecting integral type but found %s",
_tag_dyneither(_tmp708,sizeof(char),37))),_tag_dyneither(_tmp707,sizeof(void*),1)))))));}
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
_tmp70B;void*_tmp70A;(_tmp70A=0,Cyc_Tcutil_terr(loc,((_tmp70B="cannot apply numelts to a pointer with abstract bounds",
_tag_dyneither(_tmp70B,sizeof(char),55))),_tag_dyneither(_tmp70A,sizeof(void*),0)));}
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F: {const char*_tmp70F;void*_tmp70E[1];
struct Cyc_String_pa_struct _tmp70D;(_tmp70D.tag=0,((_tmp70D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp70E[0]=&
_tmp70D,Cyc_Tcutil_terr(loc,((_tmp70F="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp70F,sizeof(char),47))),_tag_dyneither(_tmp70E,sizeof(void*),1)))))));}
_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88: {const char*_tmp712;void*_tmp711;(
_tmp711=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp712="Non-unary primop",_tag_dyneither(_tmp712,sizeof(char),17))),
_tag_dyneither(_tmp711,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){{const char*_tmp716;void*_tmp715[
1];struct Cyc_String_pa_struct _tmp714;(_tmp714.tag=0,((_tmp714.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp715[0]=& _tmp714,Cyc_Tcutil_terr(e1->loc,((
_tmp716="type %s cannot be used here",_tag_dyneither(_tmp716,sizeof(char),28))),
_tag_dyneither(_tmp715,sizeof(void*),1)))))));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}if(!checker(e2)){{const char*_tmp71A;void*_tmp719[1];struct Cyc_String_pa_struct
_tmp718;(_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp719[0]=& _tmp718,Cyc_Tcutil_terr(e2->loc,((_tmp71A="type %s cannot be used here",
_tag_dyneither(_tmp71A,sizeof(char),28))),_tag_dyneither(_tmp719,sizeof(void*),1)))))));}
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
_tmp71D;void*_tmp71C;(_tmp71C=0,Cyc_Tcutil_terr(e1->loc,((_tmp71D="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp71D,sizeof(char),50))),_tag_dyneither(_tmp71C,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp720;void*_tmp71F;(_tmp71F=0,
Cyc_Tcutil_terr(e1->loc,((_tmp720="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp720,sizeof(char),54))),_tag_dyneither(_tmp71F,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp724;void*_tmp723[1];struct
Cyc_String_pa_struct _tmp722;(_tmp722.tag=0,((_tmp722.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp723[0]=& _tmp722,Cyc_Tcutil_terr(
e2->loc,((_tmp724="expecting int but found %s",_tag_dyneither(_tmp724,sizeof(
char),27))),_tag_dyneither(_tmp723,sizeof(void*),1)))))));}{void*_tmp11E=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp115);struct Cyc_Absyn_Exp*_tmp121;_LL9C: {struct Cyc_Absyn_DynEither_b_struct*
_tmp11F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp11E;if(_tmp11F->tag != 0)goto
_LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*_tmp120=(struct Cyc_Absyn_Upper_b_struct*)
_tmp11E;if(_tmp120->tag != 1)goto _LL9B;else{_tmp121=_tmp120->f1;}}_LL9F: if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp116)){const char*
_tmp727;void*_tmp726;(_tmp726=0,Cyc_Tcutil_warn(e1->loc,((_tmp727="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp727,sizeof(char),70))),_tag_dyneither(_tmp726,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp731;struct Cyc_Absyn_PtrAtts _tmp730;struct
Cyc_Absyn_PtrInfo _tmp72F;struct Cyc_Absyn_PointerType_struct*_tmp72E;struct Cyc_Absyn_PointerType_struct*
_tmp124=(_tmp72E=_cycalloc(sizeof(*_tmp72E)),((_tmp72E[0]=((_tmp731.tag=5,((
_tmp731.f1=((_tmp72F.elt_typ=_tmp110,((_tmp72F.elt_tq=_tmp111,((_tmp72F.ptr_atts=((
_tmp730.rgn=_tmp113,((_tmp730.nullable=Cyc_Absyn_true_conref,((_tmp730.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp730.zero_term=_tmp116,((_tmp730.ptrloc=0,
_tmp730)))))))))),_tmp72F)))))),_tmp731)))),_tmp72E)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp124,Cyc_Absyn_Other_coercion);return(void*)_tmp124;};_LL9B:;};
_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL96:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(
Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*
_tmp736;void*_tmp735[2];struct Cyc_String_pa_struct _tmp734;struct Cyc_String_pa_struct
_tmp733;(_tmp733.tag=0,((_tmp733.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((
_tmp734.tag=0,((_tmp734.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp735[0]=& _tmp734,((
_tmp735[1]=& _tmp733,Cyc_Tcutil_terr(e1->loc,((_tmp736="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp736,sizeof(char),59))),_tag_dyneither(_tmp735,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){const char*_tmp739;void*_tmp738;(
_tmp738=0,Cyc_Tcutil_terr(e1->loc,((_tmp739="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp739,sizeof(char),50))),_tag_dyneither(_tmp738,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp73C;void*_tmp73B;(_tmp73B=0,
Cyc_Tcutil_terr(e1->loc,((_tmp73C="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp73C,sizeof(char),54))),_tag_dyneither(_tmp73B,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp741;void*_tmp740[2];struct
Cyc_String_pa_struct _tmp73F;struct Cyc_String_pa_struct _tmp73E;(_tmp73E.tag=0,((
_tmp73E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp73F.tag=0,((_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp740[0]=& _tmp73F,((_tmp740[1]=& _tmp73E,Cyc_Tcutil_terr(
e2->loc,((_tmp741="expecting either %s or int but found %s",_tag_dyneither(
_tmp741,sizeof(char),40))),_tag_dyneither(_tmp740,sizeof(void*),2)))))))))))));}
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
goto pointer_cmp;}}}}}else{pointer_cmp: {struct _tuple0 _tmp742;struct _tuple0
_tmp136=(_tmp742.f1=Cyc_Tcutil_compress(t1),((_tmp742.f2=Cyc_Tcutil_compress(t2),
_tmp742)));void*_tmp137;struct Cyc_Absyn_PtrInfo _tmp139;void*_tmp13A;void*_tmp13B;
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
_LLA6: goto _LLA0;_LLA0:;}{const char*_tmp747;void*_tmp746[2];struct Cyc_String_pa_struct
_tmp745;struct Cyc_String_pa_struct _tmp744;(_tmp744.tag=0,((_tmp744.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp745.tag=
0,((_tmp745.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp746[0]=& _tmp745,((_tmp746[1]=& _tmp744,Cyc_Tcutil_terr(loc,((_tmp747="comparison not allowed between %s and %s",
_tag_dyneither(_tmp747,sizeof(char),41))),_tag_dyneither(_tmp746,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp748;
struct _tuple0 _tmp148=(_tmp748.f1=t1,((_tmp748.f2=t2,_tmp748)));void*_tmp149;void*
_tmp14B;void*_tmp14C;void*_tmp14E;_LLA8: _tmp149=_tmp148.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp149;if(_tmp14A->tag != 16)goto
_LLAA;else{_tmp14B=(void*)_tmp14A->f1;}};_tmp14C=_tmp148.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14D=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14C;if(_tmp14D->tag != 16)goto
_LLAA;else{_tmp14E=(void*)_tmp14D->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}{const char*_tmp74D;void*_tmp74C[2];struct Cyc_String_pa_struct
_tmp74B;struct Cyc_String_pa_struct _tmp74A;(_tmp74A.tag=0,((_tmp74A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp74B.tag=
0,((_tmp74B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp74C[0]=& _tmp74B,((_tmp74C[1]=& _tmp74A,Cyc_Tcutil_terr(loc,((_tmp74D="comparison not allowed between %s and %s",
_tag_dyneither(_tmp74D,sizeof(char),41))),_tag_dyneither(_tmp74C,sizeof(void*),2)))))))))))));}
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
default: _LLBD: {const char*_tmp750;void*_tmp74F;(_tmp74F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp750="bad binary primop",
_tag_dyneither(_tmp750,sizeof(char),18))),_tag_dyneither(_tmp74F,sizeof(void*),0)));}}}
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: {const char*_tmp753;void*_tmp752;return(_tmp752=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp753="primitive operator has 0 arguments",_tag_dyneither(_tmp753,
sizeof(char),35))),_tag_dyneither(_tmp752,sizeof(void*),0)));}case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: {const char*_tmp756;void*_tmp755;return(_tmp755=0,Cyc_Tcexp_expr_err(te,
loc,topt,((_tmp756="primitive operator has > 2 arguments",_tag_dyneither(_tmp756,
sizeof(char),37))),_tag_dyneither(_tmp755,sizeof(void*),0)));}}return t;};}struct
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
_tmp16D->tq).real_const){{const char*_tmp75A;void*_tmp759[1];struct Cyc_String_pa_struct
_tmp758;(_tmp758.tag=0,((_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp16D->name),((_tmp759[0]=& _tmp758,Cyc_Tcutil_terr(loc,((_tmp75A="attempt to over-write an aggregate with const member %s",
_tag_dyneither(_tmp75A,sizeof(char),56))),_tag_dyneither(_tmp759,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16D->type))return 0;}return 1;
_LLC9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp15A;if(_tmp162->tag != 4)goto _LLCB;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
if((_tmp164.KnownDatatypefield).tag != 2)goto _LLCB;_tmp165=(struct _tuple2)(
_tmp164.KnownDatatypefield).val;_tmp166=_tmp165.f2;}}_LLCA:{struct Cyc_List_List*
_tmp171=_tmp166->typs;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){struct _tuple18
_tmp173;struct Cyc_Absyn_Tqual _tmp174;void*_tmp175;struct _tuple18*_tmp172=(struct
_tuple18*)_tmp171->hd;_tmp173=*_tmp172;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;if(
_tmp174.real_const){{const char*_tmp75E;void*_tmp75D[1];struct Cyc_String_pa_struct
_tmp75C;(_tmp75C.tag=0,((_tmp75C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp166->name)),((_tmp75D[0]=& _tmp75C,Cyc_Tcutil_terr(
loc,((_tmp75E="attempt to over-write a datatype field (%s) with a const member",
_tag_dyneither(_tmp75E,sizeof(char),64))),_tag_dyneither(_tmp75D,sizeof(void*),1)))))));}
return 0;}if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp175))return 0;}}return 1;_LLCB: {
struct Cyc_Absyn_ArrayType_struct*_tmp167=(struct Cyc_Absyn_ArrayType_struct*)
_tmp15A;if(_tmp167->tag != 9)goto _LLCD;else{_tmp168=_tmp167->f1;_tmp169=_tmp168.elt_type;
_tmp16A=_tmp168.tq;}}_LLCC: if(_tmp16A.real_const){{const char*_tmp761;void*
_tmp760;(_tmp760=0,Cyc_Tcutil_terr(loc,((_tmp761="attempt to over-write a const array",
_tag_dyneither(_tmp761,sizeof(char),36))),_tag_dyneither(_tmp760,sizeof(void*),0)));}
return 0;}return Cyc_Tcexp_check_writable_aggr(loc,_tmp169);_LLCD: {struct Cyc_Absyn_TupleType_struct*
_tmp16B=(struct Cyc_Absyn_TupleType_struct*)_tmp15A;if(_tmp16B->tag != 11)goto
_LLCF;else{_tmp16C=_tmp16B->f1;}}_LLCE: for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct _tuple18 _tmp17C;struct Cyc_Absyn_Tqual _tmp17D;void*_tmp17E;struct _tuple18*
_tmp17B=(struct _tuple18*)_tmp16C->hd;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f1;_tmp17E=
_tmp17C.f2;if(_tmp17D.real_const){{const char*_tmp764;void*_tmp763;(_tmp763=0,Cyc_Tcutil_terr(
loc,((_tmp764="attempt to over-write a tuple field with a const member",
_tag_dyneither(_tmp764,sizeof(char),56))),_tag_dyneither(_tmp763,sizeof(void*),0)));}
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
_tmp1AC=_tmp1AA.f2;if(!_tmp1AC){{const char*_tmp767;void*_tmp766;(_tmp766=0,Cyc_Tcutil_terr(
e->loc,((_tmp767="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp767,sizeof(char),47))),_tag_dyneither(_tmp766,sizeof(void*),0)));}return;}{
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
_LLE7: goto _LLD1;_LLD1:;}{const char*_tmp76B;void*_tmp76A[1];struct Cyc_String_pa_struct
_tmp769;(_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp76A[0]=& _tmp769,Cyc_Tcutil_terr(e->loc,((
_tmp76B="attempt to write a const location: %s",_tag_dyneither(_tmp76B,sizeof(
char),38))),_tag_dyneither(_tmp76A,sizeof(void*),1)))))));};}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(
e)){const char*_tmp76E;void*_tmp76D;return(_tmp76D=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp76E="increment/decrement of non-lvalue",_tag_dyneither(_tmp76E,
sizeof(char),34))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),&
Cyc_Tcutil_tmk)){const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(e->loc,((
_tmp771="can't perform arithmetic on abstract pointer type",_tag_dyneither(
_tmp771,sizeof(char),50))),_tag_dyneither(_tmp770,sizeof(void*),0)));}if(Cyc_Tcutil_is_noalias_pointer(
t)){const char*_tmp774;void*_tmp773;(_tmp773=0,Cyc_Tcutil_terr(e->loc,((_tmp774="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp774,sizeof(char),54))),_tag_dyneither(_tmp773,sizeof(void*),0)));}}
else{const char*_tmp778;void*_tmp777[1];struct Cyc_String_pa_struct _tmp776;(
_tmp776.tag=0,((_tmp776.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp777[0]=& _tmp776,Cyc_Tcutil_terr(e->loc,((_tmp778="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp778,sizeof(char),44))),_tag_dyneither(_tmp777,sizeof(void*),1)))))));}}
return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){struct _tuple0*_tmp1DF;int _tmp1E0;const char*_tmp779;struct Cyc_Tcexp_TestEnv
_tmp1DE=Cyc_Tcexp_tcTest(te,e1,((_tmp779="conditional expression",_tag_dyneither(
_tmp779,sizeof(char),23))));_tmp1DF=_tmp1DE.eq;_tmp1E0=_tmp1DE.isTrue;Cyc_Tcexp_tcExp(
te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List _tmp77A;
struct Cyc_List_List _tmp1E1=(_tmp77A.hd=e3,((_tmp77A.tl=0,_tmp77A)));struct Cyc_List_List
_tmp77B;struct Cyc_List_List _tmp1E2=(_tmp77B.hd=e2,((_tmp77B.tl=(struct Cyc_List_List*)&
_tmp1E1,_tmp77B)));if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)&
_tmp1E2)){{const char*_tmp780;void*_tmp77F[2];struct Cyc_String_pa_struct _tmp77E;
struct Cyc_String_pa_struct _tmp77D;(_tmp77D.tag=0,((_tmp77D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v)),((_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp77F[0]=& _tmp77E,((_tmp77F[1]=& _tmp77D,Cyc_Tcutil_terr(
loc,((_tmp780="conditional clause types do not match: %s != %s",_tag_dyneither(
_tmp780,sizeof(char),48))),_tag_dyneither(_tmp77F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp781;Cyc_Tcexp_tcTest(te,e1,((_tmp781="logical-and expression",
_tag_dyneither(_tmp781,sizeof(char),23))));}{const char*_tmp782;Cyc_Tcexp_tcTest(
te,e2,((_tmp782="logical-and expression",_tag_dyneither(_tmp782,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp783;Cyc_Tcexp_tcTest(te,e1,((_tmp783="logical-or expression",
_tag_dyneither(_tmp783,sizeof(char),22))));}{const char*_tmp784;Cyc_Tcexp_tcTest(
te,e2,((_tmp784="logical-or expression",_tag_dyneither(_tmp784,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1ED=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1ED,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1EE=Cyc_Tcutil_compress(
t1);_LL111: {struct Cyc_Absyn_ArrayType_struct*_tmp1EF=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1EE;if(_tmp1EF->tag != 9)goto _LL113;}_LL112:{const char*_tmp787;void*_tmp786;(
_tmp786=0,Cyc_Tcutil_terr(loc,((_tmp787="cannot assign to an array",
_tag_dyneither(_tmp787,sizeof(char),26))),_tag_dyneither(_tmp786,sizeof(void*),0)));}
goto _LL110;_LL113:;_LL114: goto _LL110;_LL110:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),& Cyc_Tcutil_tmk)){const char*_tmp78A;void*_tmp789;(_tmp789=0,Cyc_Tcutil_terr(
loc,((_tmp78A="type is abstract (can't determine size).",_tag_dyneither(_tmp78A,
sizeof(char),41))),_tag_dyneither(_tmp789,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(
e1)){const char*_tmp78D;void*_tmp78C;return(_tmp78C=0,Cyc_Tcexp_expr_err(te,loc,
topt,((_tmp78D="assignment to non-lvalue",_tag_dyneither(_tmp78D,sizeof(char),25))),
_tag_dyneither(_tmp78C,sizeof(void*),0)));}Cyc_Tcexp_check_writable(te,e1);if(po
== 0){struct _RegionHandle*_tmp1F6=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F6,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F6,e2)){const char*_tmp790;void*
_tmp78F;(_tmp78F=0,Cyc_Tcutil_terr(e2->loc,((_tmp790="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp790,sizeof(char),49))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp795;void*_tmp794[2];struct
Cyc_String_pa_struct _tmp793;struct Cyc_String_pa_struct _tmp792;void*_tmp1F9=(
_tmp792.tag=0,((_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp794[0]=& _tmp793,((_tmp794[1]=& _tmp792,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp795="type mismatch: %s != %s",_tag_dyneither(_tmp795,sizeof(
char),24))),_tag_dyneither(_tmp794,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp1F9;}}else{enum Cyc_Absyn_Primop
_tmp1FE=(enum Cyc_Absyn_Primop)po->v;void*_tmp1FF=Cyc_Tcexp_tcBinPrimop(te,loc,0,
_tmp1FE,e1,e2);if(!(Cyc_Tcutil_unify(_tmp1FF,t1) || Cyc_Tcutil_coerceable(
_tmp1FF) && Cyc_Tcutil_coerceable(t1))){const char*_tmp79A;void*_tmp799[2];struct
Cyc_String_pa_struct _tmp798;struct Cyc_String_pa_struct _tmp797;void*_tmp200=(
_tmp797.tag=0,((_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp798.tag=0,((_tmp798.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp799[0]=& _tmp798,((_tmp799[1]=& _tmp797,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp79A="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp79A,sizeof(char),82))),_tag_dyneither(_tmp799,sizeof(void*),2)))))))))))));
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
struct Cyc_Absyn_Datatypefield*)fields->hd);};}{const char*_tmp79F;void*_tmp79E[2];
struct Cyc_String_pa_struct _tmp79D;struct Cyc_String_pa_struct _tmp79C;(_tmp79C.tag=
0,((_tmp79C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp79D.tag=0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(tu)),((_tmp79E[0]=& _tmp79D,((_tmp79E[1]=& _tmp79C,Cyc_Tcutil_terr(
e->loc,((_tmp79F="can't find a field in %s to inject a value of type %s",
_tag_dyneither(_tmp79F,sizeof(char),54))),_tag_dyneither(_tmp79E,sizeof(void*),2)))))))))))));}
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
const char*_tmp7A4;void*_tmp7A3[2];struct Cyc_String_pa_struct _tmp7A2;struct Cyc_String_pa_struct
_tmp7A1;(_tmp7A1.tag=0,((_tmp7A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp7A3[0]=& _tmp7A2,((_tmp7A3[1]=& _tmp7A1,Cyc_Tcutil_terr(
e1->loc,((_tmp7A4="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp7A4,sizeof(char),57))),_tag_dyneither(_tmp7A3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(alias_coercion){struct Cyc_List_List*_tmp7A5;*alias_arg_exps=(struct Cyc_List_List*)((
_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5->hd=(void*)_tmp21B,((_tmp7A5->tl=*
alias_arg_exps,_tmp7A5))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp21C,
t2) && !Cyc_Tcutil_is_noalias_path(_tmp21C,e1)){const char*_tmp7A8;void*_tmp7A7;(
_tmp7A7=0,Cyc_Tcutil_terr(e1->loc,((_tmp7A8="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7A8,sizeof(char),49))),_tag_dyneither(_tmp7A7,sizeof(void*),0)));}
_tmp21A=_tmp21A->tl;_tmp22E=_tmp22E->tl;++ _tmp21B;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp232;for(0;a != 0;a=a->tl){void*_tmp23A=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp23C;int _tmp23D;int _tmp23E;_LL129: {struct Cyc_Absyn_Format_att_struct*
_tmp23B=(struct Cyc_Absyn_Format_att_struct*)_tmp23A;if(_tmp23B->tag != 19)goto
_LL12B;else{_tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;_tmp23E=_tmp23B->f3;}}_LL12A:{
struct _handler_cons _tmp23F;_push_handler(& _tmp23F);{int _tmp241=0;if(setjmp(
_tmp23F.handler))_tmp241=1;if(!_tmp241){{struct Cyc_Absyn_Exp*_tmp242=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp23D - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp23E == 0)fmt_args=0;else{struct Cyc_Core_Opt*_tmp7A9;fmt_args=((
_tmp7A9=_cycalloc(sizeof(*_tmp7A9)),((_tmp7A9->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp23E - 1),_tmp7A9))));}
args_already_checked=1;{struct _RegionHandle*_tmp244=Cyc_Tcenv_get_fnrgn(te_orig);
switch(_tmp23C){case Cyc_Absyn_Printf_ft: _LL12D: Cyc_Tcexp_check_format_args(
_tmp21D,_tmp242,fmt_args,_tmp23E - 1,alias_arg_exps,_tmp244,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL12E: Cyc_Tcexp_check_format_args(_tmp21D,_tmp242,
fmt_args,_tmp23E - 1,alias_arg_exps,_tmp244,Cyc_Formatstr_get_scanf_typs);break;}};};
_pop_handler();}else{void*_tmp240=(void*)_exn_thrown;void*_tmp246=_tmp240;_LL131: {
struct Cyc_List_Nth_struct*_tmp247=(struct Cyc_List_Nth_struct*)_tmp246;if(_tmp247->tag
!= Cyc_List_Nth)goto _LL133;}_LL132:{const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,
Cyc_Tcutil_terr(loc,((_tmp7AC="bad format arguments",_tag_dyneither(_tmp7AC,
sizeof(char),21))),_tag_dyneither(_tmp7AB,sizeof(void*),0)));}goto _LL130;_LL133:;
_LL134:(void)_throw(_tmp246);_LL130:;}};}goto _LL128;_LL12B:;_LL12C: goto _LL128;
_LL128:;}}if(_tmp22E != 0){const char*_tmp7AF;void*_tmp7AE;(_tmp7AE=0,Cyc_Tcutil_terr(
loc,((_tmp7AF="too few arguments for function",_tag_dyneither(_tmp7AF,sizeof(
char),31))),_tag_dyneither(_tmp7AE,sizeof(void*),0)));}else{if((_tmp21A != 0  || 
_tmp22F) || _tmp230 != 0){if(_tmp22F)for(0;_tmp21A != 0;_tmp21A=_tmp21A->tl){Cyc_Tcexp_tcExp(
_tmp21D,0,(struct Cyc_Absyn_Exp*)_tmp21A->hd);}else{if(_tmp230 == 0){const char*
_tmp7B2;void*_tmp7B1;(_tmp7B1=0,Cyc_Tcutil_terr(loc,((_tmp7B2="too many arguments for function",
_tag_dyneither(_tmp7B2,sizeof(char),32))),_tag_dyneither(_tmp7B1,sizeof(void*),0)));}
else{void*_tmp24F;int _tmp250;struct Cyc_Absyn_VarargInfo _tmp24E=*_tmp230;_tmp24F=
_tmp24E.type;_tmp250=_tmp24E.inject;{struct Cyc_Absyn_VarargCallInfo*_tmp7B3;
struct Cyc_Absyn_VarargCallInfo*_tmp251=(_tmp7B3=_cycalloc(sizeof(*_tmp7B3)),((
_tmp7B3->num_varargs=0,((_tmp7B3->injectors=0,((_tmp7B3->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp230,_tmp7B3)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp251;if(!_tmp250)for(0;_tmp21A != 0;(_tmp21A=_tmp21A->tl,_tmp21B ++)){int
alias_coercion=0;struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;++
_tmp251->num_varargs;Cyc_Tcexp_tcExp(_tmp21D,(void**)& _tmp24F,e1);if(!Cyc_Tcutil_coerce_arg(
_tmp21D,e1,_tmp24F,& alias_coercion)){{const char*_tmp7B8;void*_tmp7B7[2];struct
Cyc_String_pa_struct _tmp7B6;struct Cyc_String_pa_struct _tmp7B5;(_tmp7B5.tag=0,((
_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((_tmp7B6.tag=0,((
_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp24F)),((_tmp7B7[0]=& _tmp7B6,((_tmp7B7[1]=& _tmp7B5,Cyc_Tcutil_terr(loc,((
_tmp7B8="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp7B8,sizeof(char),49))),_tag_dyneither(_tmp7B7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(alias_coercion){struct Cyc_List_List*_tmp7B9;*
alias_arg_exps=(struct Cyc_List_List*)((_tmp7B9=_cycalloc(sizeof(*_tmp7B9)),((
_tmp7B9->hd=(void*)_tmp21B,((_tmp7B9->tl=*alias_arg_exps,_tmp7B9))))));}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp21C,_tmp24F) && !Cyc_Tcutil_is_noalias_path(_tmp21C,e1)){const char*_tmp7BC;
void*_tmp7BB;(_tmp7BB=0,Cyc_Tcutil_terr(e1->loc,((_tmp7BC="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp7BC,sizeof(char),49))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}}
else{void*_tmp259=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp24F));
struct Cyc_Absyn_DatatypeInfo _tmp25B;union Cyc_Absyn_DatatypeInfoU _tmp25C;struct
Cyc_Absyn_Datatypedecl**_tmp25D;struct Cyc_Absyn_Datatypedecl*_tmp25E;struct Cyc_List_List*
_tmp25F;_LL136: {struct Cyc_Absyn_DatatypeType_struct*_tmp25A=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp259;if(_tmp25A->tag != 3)goto _LL138;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.datatype_info;
if((_tmp25C.KnownDatatype).tag != 2)goto _LL138;_tmp25D=(struct Cyc_Absyn_Datatypedecl**)(
_tmp25C.KnownDatatype).val;_tmp25E=*_tmp25D;_tmp25F=_tmp25B.targs;}}_LL137: {
struct Cyc_Absyn_Datatypedecl*_tmp260=*Cyc_Tcenv_lookup_datatypedecl(_tmp21D,loc,
_tmp25E->name);struct Cyc_List_List*fields=0;if(_tmp260->fields == 0){const char*
_tmp7C0;void*_tmp7BF[1];struct Cyc_String_pa_struct _tmp7BE;(_tmp7BE.tag=0,((
_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp24F)),((_tmp7BF[0]=& _tmp7BE,Cyc_Tcutil_terr(loc,((_tmp7C0="can't inject into abstract datatype %s",
_tag_dyneither(_tmp7C0,sizeof(char),39))),_tag_dyneither(_tmp7BF,sizeof(void*),1)))))));}
else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp260->fields))->v;}
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp24F),Cyc_Tcenv_curr_rgn(
_tmp21D))){const char*_tmp7C5;void*_tmp7C4[2];struct Cyc_String_pa_struct _tmp7C3;
struct Cyc_String_pa_struct _tmp7C2;(_tmp7C2.tag=0,((_tmp7C2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(
_tmp21D))),((_tmp7C3.tag=0,((_tmp7C3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24F)),((_tmp7C4[0]=& _tmp7C3,((_tmp7C4[
1]=& _tmp7C2,Cyc_Tcutil_terr(loc,((_tmp7C5="can't unify pointer region for %s to curr_rgn %s",
_tag_dyneither(_tmp7C5,sizeof(char),49))),_tag_dyneither(_tmp7C4,sizeof(void*),2)))))))))))));}{
struct _RegionHandle*_tmp268=Cyc_Tcenv_get_fnrgn(_tmp21D);{struct Cyc_List_List*
_tmp269=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp268,_tmp268,
_tmp260->tvs,_tmp25F);for(0;_tmp21A != 0;_tmp21A=_tmp21A->tl){++ _tmp251->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp21A->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp21D,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp268,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp268,e1)){const char*_tmp7C8;void*_tmp7C7;(_tmp7C7=0,Cyc_Tcutil_terr(e1->loc,((
_tmp7C8="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp7C8,sizeof(char),49))),_tag_dyneither(_tmp7C7,sizeof(void*),0)));}}{struct
Cyc_Absyn_Datatypefield*_tmp26C=Cyc_Tcexp_tcInjection(_tmp21D,e1,Cyc_Tcutil_pointer_elt_type(
_tmp24F),_tmp268,_tmp269,fields);if(_tmp26C != 0){struct Cyc_List_List*_tmp7C9;
_tmp251->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp251->injectors,((_tmp7C9=_cycalloc(sizeof(*_tmp7C9)),((
_tmp7C9->hd=(struct Cyc_Absyn_Datatypefield*)_tmp26C,((_tmp7C9->tl=0,_tmp7C9)))))));}};};}}
goto _LL135;};}_LL138:;_LL139:{const char*_tmp7CC;void*_tmp7CB;(_tmp7CB=0,Cyc_Tcutil_terr(
loc,((_tmp7CC="bad inject vararg type",_tag_dyneither(_tmp7CC,sizeof(char),23))),
_tag_dyneither(_tmp7CB,sizeof(void*),0)));}goto _LL135;_LL135:;}};}}}}Cyc_Tcenv_check_effect_accessible(
_tmp21D,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp22C))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp21D,loc,_tmp231);return _tmp22D;};_LL126:;_LL127: {const char*_tmp7CF;void*
_tmp7CE;return(_tmp7CE=0,Cyc_Tcexp_expr_err(_tmp21D,loc,topt,((_tmp7CF="expected pointer to function",
_tag_dyneither(_tmp7CF,sizeof(char),29))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}
_LL123:;};_LL121:;_LL122: {const char*_tmp7D2;void*_tmp7D1;return(_tmp7D1=0,Cyc_Tcexp_expr_err(
_tmp21D,loc,topt,((_tmp7D2="expected pointer to function",_tag_dyneither(_tmp7D2,
sizeof(char),29))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}_LL11E:;};}static
void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e){int bogus=0;void*_tmp275=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp275,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp275,&
bogus)){const char*_tmp7D7;void*_tmp7D6[2];struct Cyc_String_pa_struct _tmp7D5;
struct Cyc_String_pa_struct _tmp7D4;(_tmp7D4.tag=0,((_tmp7D4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp7D5.tag=0,((_tmp7D5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp275)),((_tmp7D6[0]=& _tmp7D5,((
_tmp7D6[1]=& _tmp7D4,Cyc_Tcutil_terr(loc,((_tmp7D7="expected %s but found %s",
_tag_dyneither(_tmp7D7,sizeof(char),25))),_tag_dyneither(_tmp7D6,sizeof(void*),2)))))))))))));}
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
te),k,1,(void*)ts->hd);{struct _tuple12*_tmp7DA;struct Cyc_List_List*_tmp7D9;
instantiation=((_tmp7D9=_region_malloc(_tmp28F,sizeof(*_tmp7D9)),((_tmp7D9->hd=((
_tmp7DA=_region_malloc(_tmp28F,sizeof(*_tmp7DA)),((_tmp7DA->f1=(struct Cyc_Absyn_Tvar*)
_tmp287->hd,((_tmp7DA->f2=(void*)ts->hd,_tmp7DA)))))),((_tmp7D9->tl=
instantiation,_tmp7D9))))));};}if(ts != 0){const char*_tmp7DD;void*_tmp7DC;return(
_tmp7DC=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DD="too many type variables in instantiation",
_tag_dyneither(_tmp7DD,sizeof(char),41))),_tag_dyneither(_tmp7DC,sizeof(void*),0)));}{
struct Cyc_Absyn_FnType_struct _tmp7E3;struct Cyc_Absyn_FnInfo _tmp7E2;struct Cyc_Absyn_FnType_struct*
_tmp7E1;void*new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp28F,instantiation,(void*)((
_tmp7E1=_cycalloc(sizeof(*_tmp7E1)),((_tmp7E1[0]=((_tmp7E3.tag=10,((_tmp7E3.f1=((
_tmp7E2.tvars=_tmp287,((_tmp7E2.effect=_tmp288,((_tmp7E2.ret_typ=_tmp289,((
_tmp7E2.args=_tmp28A,((_tmp7E2.c_varargs=_tmp28B,((_tmp7E2.cyc_varargs=_tmp28C,((
_tmp7E2.rgn_po=_tmp28D,((_tmp7E2.attributes=_tmp28E,_tmp7E2)))))))))))))))),
_tmp7E3)))),_tmp7E1)))));struct Cyc_Absyn_PointerType_struct _tmp7ED;struct Cyc_Absyn_PtrAtts
_tmp7EC;struct Cyc_Absyn_PtrInfo _tmp7EB;struct Cyc_Absyn_PointerType_struct*
_tmp7EA;return(void*)((_tmp7EA=_cycalloc(sizeof(*_tmp7EA)),((_tmp7EA[0]=((
_tmp7ED.tag=5,((_tmp7ED.f1=((_tmp7EB.elt_typ=new_fn_typ,((_tmp7EB.elt_tq=_tmp27E,((
_tmp7EB.ptr_atts=((_tmp7EC.rgn=_tmp280,((_tmp7EC.nullable=_tmp281,((_tmp7EC.bounds=
_tmp282,((_tmp7EC.zero_term=_tmp283,((_tmp7EC.ptrloc=0,_tmp7EC)))))))))),_tmp7EB)))))),
_tmp7ED)))),_tmp7EA))));};}_LL142:;_LL143: goto _LL13F;_LL13F:;}goto _LL13A;_LL13D:;
_LL13E: goto _LL13A;_LL13A:;}{const char*_tmp7F1;void*_tmp7F0[1];struct Cyc_String_pa_struct
_tmp7EF;return(_tmp7EF.tag=0,((_tmp7EF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7F0[0]=& _tmp7EF,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7F1="expecting polymorphic type but found %s",_tag_dyneither(
_tmp7F1,sizeof(char),40))),_tag_dyneither(_tmp7F0,sizeof(void*),1)))))));};};}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);Cyc_Tcexp_tcExp(te,(
void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(
Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(
c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,
loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{const char*
_tmp7F6;void*_tmp7F5[2];struct Cyc_String_pa_struct _tmp7F4;struct Cyc_String_pa_struct
_tmp7F3;void*_tmp29E=(_tmp7F3.tag=0,((_tmp7F3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7F4.tag=0,((_tmp7F4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7F5[0]=&
_tmp7F4,((_tmp7F5[1]=& _tmp7F3,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp7F6="cannot cast %s to %s",
_tag_dyneither(_tmp7F6,sizeof(char),21))),_tag_dyneither(_tmp7F5,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp29E;};}}}{struct _tuple0 _tmp7F7;struct
_tuple0 _tmp2A4=(_tmp7F7.f1=e->r,((_tmp7F7.f2=Cyc_Tcutil_compress(t),_tmp7F7)));
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
== 1){const char*_tmp7FA;void*_tmp7F9;(_tmp7F9=0,Cyc_Tcutil_warn(loc,((_tmp7FA="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp7FA,sizeof(char),60))),_tag_dyneither(_tmp7F9,sizeof(void*),0)));}
goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}goto _LL144;_LL147:;_LL148: goto
_LL144;_LL144:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp2B5=0;struct Cyc_Absyn_Tqual _tmp2B6=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp2B7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2B9;
void*_tmp2BA;struct Cyc_Absyn_Tqual _tmp2BB;struct Cyc_Absyn_PtrAtts _tmp2BC;union
Cyc_Absyn_Constraint*_tmp2BD;_LL14F: {struct Cyc_Absyn_PointerType_struct*_tmp2B8=(
struct Cyc_Absyn_PointerType_struct*)_tmp2B7;if(_tmp2B8->tag != 5)goto _LL151;else{
_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.elt_typ;_tmp2BB=_tmp2B9.elt_tq;_tmp2BC=
_tmp2B9.ptr_atts;_tmp2BD=_tmp2BC.zero_term;}}_LL150:{void**_tmp7FB;_tmp2B5=((
_tmp7FB=_cycalloc(sizeof(*_tmp7FB)),((_tmp7FB[0]=_tmp2BA,_tmp7FB))));}_tmp2B6=
_tmp2BB;goto _LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}{struct _RegionHandle*
_tmp2BF=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(
_tmp2BF,te),_tmp2B5,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp2BF,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v)){const char*_tmp7FE;void*_tmp7FD;(
_tmp7FD=0,Cyc_Tcutil_terr(e->loc,((_tmp7FE="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp7FE,sizeof(char),46))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));}{
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
_tmp801;void*_tmp800;(_tmp800=0,Cyc_Tcutil_terr(e->loc,((_tmp801="cannot take the address of a @tagged union member",
_tag_dyneither(_tmp801,sizeof(char),50))),_tag_dyneither(_tmp800,sizeof(void*),0)));}
goto _LL15D;_LL162:;_LL163: goto _LL15D;_LL15D:;}{int _tmp2D0;void*_tmp2D1;struct
_tuple13 _tmp2CF=Cyc_Tcutil_addressof_props(te,e);_tmp2D0=_tmp2CF.f1;_tmp2D1=
_tmp2CF.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2D0){tq.print_const=
1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp2D1,tq,Cyc_Absyn_false_conref);return t;};};};};}
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);if(!Cyc_Evexp_okay_szofarg(
t)){const char*_tmp805;void*_tmp804[1];struct Cyc_String_pa_struct _tmp803;(_tmp803.tag=
0,((_tmp803.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp804[0]=& _tmp803,Cyc_Tcutil_terr(loc,((_tmp805="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp805,sizeof(char),55))),_tag_dyneither(_tmp804,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp2D5=Cyc_Tcutil_compress(*topt);void*_tmp2D7;_LL165: {
struct Cyc_Absyn_TagType_struct*_tmp2D6=(struct Cyc_Absyn_TagType_struct*)_tmp2D5;
if(_tmp2D6->tag != 20)goto _LL167;else{_tmp2D7=(void*)_tmp2D6->f1;}}_LL166: {
struct Cyc_Absyn_Exp*_tmp2D8=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct
_tmp808;struct Cyc_Absyn_ValueofType_struct*_tmp807;struct Cyc_Absyn_ValueofType_struct*
_tmp2D9=(_tmp807=_cycalloc(sizeof(*_tmp807)),((_tmp807[0]=((_tmp808.tag=19,((
_tmp808.f1=_tmp2D8,_tmp808)))),_tmp807)));if(Cyc_Tcutil_unify(_tmp2D7,(void*)
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
_check_null((*_tmp2E5)->impl))->fields)){const char*_tmp80C;void*_tmp80B[1];
struct Cyc_String_pa_struct _tmp80A;(_tmp80A.tag=0,((_tmp80A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp2DE),((_tmp80B[0]=& _tmp80A,Cyc_Tcutil_terr(
loc,((_tmp80C="no field of struct/union has name %s",_tag_dyneither(_tmp80C,
sizeof(char),37))),_tag_dyneither(_tmp80B,sizeof(void*),1)))))));}bad_type=0;
goto _LL16E;_LL171: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2E6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2E1;if(_tmp2E6->tag != 13)goto _LL173;else{_tmp2E7=_tmp2E6->f2;}}_LL172: if(!((
int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp2DE,_tmp2E7)){const char*_tmp810;void*_tmp80F[1];struct Cyc_String_pa_struct
_tmp80E;(_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp2DE),((_tmp80F[0]=& _tmp80E,Cyc_Tcutil_terr(loc,((_tmp810="no field of struct/union has name %s",
_tag_dyneither(_tmp810,sizeof(char),37))),_tag_dyneither(_tmp80F,sizeof(void*),1)))))));}
bad_type=0;goto _LL16E;_LL173:;_LL174: goto _LL16E;_LL16E:;}if(bad_type){const char*
_tmp814;void*_tmp813[1];struct Cyc_String_pa_struct _tmp812;(_tmp812.tag=0,((
_tmp812.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp813[0]=& _tmp812,Cyc_Tcutil_terr(loc,((_tmp814="%s is not a known struct/union type",
_tag_dyneither(_tmp814,sizeof(char),36))),_tag_dyneither(_tmp813,sizeof(void*),1)))))));}
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
_tmp819;void*_tmp818[2];struct Cyc_Int_pa_struct _tmp817;struct Cyc_Int_pa_struct
_tmp816;(_tmp816.tag=1,((_tmp816.f1=(unsigned long)((int)_tmp2E0),((_tmp817.tag=
1,((_tmp817.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2F7),((_tmp818[0]=& _tmp817,((_tmp818[1]=& _tmp816,Cyc_Tcutil_terr(loc,((
_tmp819="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp819,
sizeof(char),46))),_tag_dyneither(_tmp818,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17A: {struct Cyc_Absyn_TupleType_struct*_tmp2F8=(struct Cyc_Absyn_TupleType_struct*)
_tmp2F1;if(_tmp2F8->tag != 11)goto _LL17C;else{_tmp2F9=_tmp2F8->f1;}}_LL17B: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F9)<= _tmp2E0){const char*
_tmp81E;void*_tmp81D[2];struct Cyc_Int_pa_struct _tmp81C;struct Cyc_Int_pa_struct
_tmp81B;(_tmp81B.tag=1,((_tmp81B.f1=(unsigned long)((int)_tmp2E0),((_tmp81C.tag=
1,((_tmp81C.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2F9),((_tmp81D[0]=& _tmp81C,((_tmp81D[1]=& _tmp81B,Cyc_Tcutil_terr(loc,((
_tmp81E="tuple has too few components: %d <= %d",_tag_dyneither(_tmp81E,sizeof(
char),39))),_tag_dyneither(_tmp81D,sizeof(void*),2)))))))))))));}bad_type=0;goto
_LL175;_LL17C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2FA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2F1;if(_tmp2FA->tag != 4)goto _LL17E;else{_tmp2FB=_tmp2FA->f1;_tmp2FC=_tmp2FB.field_info;
if((_tmp2FC.KnownDatatypefield).tag != 2)goto _LL17E;_tmp2FD=(struct _tuple2)(
_tmp2FC.KnownDatatypefield).val;_tmp2FE=_tmp2FD.f2;}}_LL17D: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp2FE->typs)< _tmp2E0){const char*_tmp823;
void*_tmp822[2];struct Cyc_Int_pa_struct _tmp821;struct Cyc_Int_pa_struct _tmp820;(
_tmp820.tag=1,((_tmp820.f1=(unsigned long)((int)_tmp2E0),((_tmp821.tag=1,((
_tmp821.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2FE->typs),((_tmp822[0]=& _tmp821,((_tmp822[1]=& _tmp820,Cyc_Tcutil_terr(loc,((
_tmp823="datatype field has too few components: %d < %d",_tag_dyneither(_tmp823,
sizeof(char),47))),_tag_dyneither(_tmp822,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL175;_LL17E:;_LL17F: goto _LL175;_LL175:;}if(bad_type){const char*_tmp827;
void*_tmp826[1];struct Cyc_String_pa_struct _tmp825;(_tmp825.tag=0,((_tmp825.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp826[
0]=& _tmp825,Cyc_Tcutil_terr(loc,((_tmp827="%s is not a known type",
_tag_dyneither(_tmp827,sizeof(char),23))),_tag_dyneither(_tmp826,sizeof(void*),1)))))));}
goto _LL169;}_LL169:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp30E=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp30F=Cyc_Tcenv_clear_notreadctxt(_tmp30E,te_orig);Cyc_Tcexp_tcExp(_tmp30F,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp310=t;struct Cyc_Absyn_PtrInfo _tmp313;void*_tmp314;struct Cyc_Absyn_PtrAtts
_tmp315;void*_tmp316;union Cyc_Absyn_Constraint*_tmp317;union Cyc_Absyn_Constraint*
_tmp318;_LL181: {struct Cyc_Absyn_Evar_struct*_tmp311=(struct Cyc_Absyn_Evar_struct*)
_tmp310;if(_tmp311->tag != 1)goto _LL183;}_LL182: {struct Cyc_List_List*_tmp319=Cyc_Tcenv_lookup_type_vars(
_tmp30F);struct Cyc_Core_Opt*_tmp828;void*_tmp31A=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mko,((_tmp828=_cycalloc(sizeof(*_tmp828)),((_tmp828->v=_tmp319,
_tmp828)))));struct Cyc_Core_Opt*_tmp829;void*_tmp31B=Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829->v=
_tmp319,_tmp829)))));union Cyc_Absyn_Constraint*_tmp31C=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*_tmp31D=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp82A;struct Cyc_Absyn_PtrAtts
_tmp31E=(_tmp82A.rgn=_tmp31B,((_tmp82A.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp82A.bounds=_tmp31C,((_tmp82A.zero_term=_tmp31D,((
_tmp82A.ptrloc=0,_tmp82A)))))))));struct Cyc_Absyn_PointerType_struct _tmp830;
struct Cyc_Absyn_PtrInfo _tmp82F;struct Cyc_Absyn_PointerType_struct*_tmp82E;struct
Cyc_Absyn_PointerType_struct*_tmp31F=(_tmp82E=_cycalloc(sizeof(*_tmp82E)),((
_tmp82E[0]=((_tmp830.tag=5,((_tmp830.f1=((_tmp82F.elt_typ=_tmp31A,((_tmp82F.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp82F.ptr_atts=_tmp31E,_tmp82F)))))),_tmp830)))),
_tmp82E)));Cyc_Tcutil_unify(t,(void*)_tmp31F);_tmp314=_tmp31A;_tmp316=_tmp31B;
_tmp317=_tmp31C;_tmp318=_tmp31D;goto _LL184;}_LL183: {struct Cyc_Absyn_PointerType_struct*
_tmp312=(struct Cyc_Absyn_PointerType_struct*)_tmp310;if(_tmp312->tag != 5)goto
_LL185;else{_tmp313=_tmp312->f1;_tmp314=_tmp313.elt_typ;_tmp315=_tmp313.ptr_atts;
_tmp316=_tmp315.rgn;_tmp317=_tmp315.bounds;_tmp318=_tmp315.zero_term;}}_LL184:
Cyc_Tcenv_check_rgn_accessible(_tmp30F,loc,_tmp316);Cyc_Tcutil_check_nonzero_bound(
loc,_tmp317);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp314),& Cyc_Tcutil_tmk)){
const char*_tmp833;void*_tmp832;(_tmp832=0,Cyc_Tcutil_terr(loc,((_tmp833="can't dereference abstract pointer type",
_tag_dyneither(_tmp833,sizeof(char),40))),_tag_dyneither(_tmp832,sizeof(void*),0)));}
return _tmp314;_LL185:;_LL186: {const char*_tmp837;void*_tmp836[1];struct Cyc_String_pa_struct
_tmp835;return(_tmp835.tag=0,((_tmp835.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp836[0]=& _tmp835,Cyc_Tcexp_expr_err(
_tmp30F,loc,topt,((_tmp837="expecting *, @, or ? type but found %s",
_tag_dyneither(_tmp837,sizeof(char),39))),_tag_dyneither(_tmp836,sizeof(void*),1)))))));}
_LL180:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){Cyc_Tcexp_tcExpNoPromote(te,0,e);*
is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp32B=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp32D;
union Cyc_Absyn_AggrInfoU _tmp32E;struct Cyc_Absyn_Aggrdecl**_tmp32F;struct Cyc_Absyn_Aggrdecl*
_tmp330;struct Cyc_List_List*_tmp331;enum Cyc_Absyn_AggrKind _tmp333;struct Cyc_List_List*
_tmp334;_LL188: {struct Cyc_Absyn_AggrType_struct*_tmp32C=(struct Cyc_Absyn_AggrType_struct*)
_tmp32B;if(_tmp32C->tag != 12)goto _LL18A;else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32D.aggr_info;
if((_tmp32E.KnownAggr).tag != 2)goto _LL18A;_tmp32F=(struct Cyc_Absyn_Aggrdecl**)(
_tmp32E.KnownAggr).val;_tmp330=*_tmp32F;_tmp331=_tmp32D.targs;}}_LL189: {struct
Cyc_Absyn_Aggrfield*_tmp339=Cyc_Absyn_lookup_decl_field(_tmp330,f);if(_tmp339 == 
0){const char*_tmp83C;void*_tmp83B[2];struct Cyc_String_pa_struct _tmp83A;struct Cyc_String_pa_struct
_tmp839;return(_tmp839.tag=0,((_tmp839.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp330->name)),((_tmp83B[0]=& _tmp83A,((
_tmp83B[1]=& _tmp839,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp83C="type %s has no %s field",
_tag_dyneither(_tmp83C,sizeof(char),24))),_tag_dyneither(_tmp83B,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp330->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp33E=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp33F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp33E,_tmp33E,_tmp330->tvs,_tmp331);t2=Cyc_Tcutil_rsubstitute(_tmp33E,_tmp33F,
_tmp339->type);if(((_tmp330->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp330->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2)){const char*_tmp840;void*_tmp83F[1];struct Cyc_String_pa_struct _tmp83E;return(
_tmp83E.tag=0,((_tmp83E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp83F[0]=& _tmp83E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp840="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp840,sizeof(char),56))),_tag_dyneither(_tmp83F,sizeof(void*),1)))))));}}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp330->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp844;void*_tmp843[1];struct Cyc_String_pa_struct _tmp842;return(
_tmp842.tag=0,((_tmp842.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp843[0]=& _tmp842,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp844="must use pattern-matching to access field %s\n\tdue to existential type variables.",
_tag_dyneither(_tmp844,sizeof(char),81))),_tag_dyneither(_tmp843,sizeof(void*),1)))))));}}
return t2;};}_LL18A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp332=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp32B;if(_tmp332->tag != 13)goto _LL18C;else{_tmp333=_tmp332->f1;_tmp334=_tmp332->f2;}}
_LL18B: {struct Cyc_Absyn_Aggrfield*_tmp346=Cyc_Absyn_lookup_field(_tmp334,f);if(
_tmp346 == 0){const char*_tmp848;void*_tmp847[1];struct Cyc_String_pa_struct _tmp846;
return(_tmp846.tag=0,((_tmp846.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp847[0]=& _tmp846,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp848="type has no %s field",
_tag_dyneither(_tmp848,sizeof(char),21))),_tag_dyneither(_tmp847,sizeof(void*),1)))))));}
if((_tmp333 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp346->type)){const char*_tmp84C;void*_tmp84B[1];struct Cyc_String_pa_struct
_tmp84A;return(_tmp84A.tag=0,((_tmp84A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp84B[0]=& _tmp84A,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp84C="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp84C,sizeof(char),56))),_tag_dyneither(_tmp84B,sizeof(void*),1)))))));}return
_tmp346->type;}_LL18C:{struct Cyc_Absyn_ArrayType_struct*_tmp335=(struct Cyc_Absyn_ArrayType_struct*)
_tmp32B;if(_tmp335->tag != 9)goto _LL18E;}{const char*_tmp84D;if(!(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp84D="size",_tag_dyneither(_tmp84D,sizeof(char),5))))
== 0))goto _LL18E;};_LL18D: goto _LL18F;_LL18E:{struct Cyc_Absyn_PointerType_struct*
_tmp337=(struct Cyc_Absyn_PointerType_struct*)_tmp32B;if(_tmp337->tag != 5)goto
_LL190;}{const char*_tmp84E;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp84E="size",
_tag_dyneither(_tmp84E,sizeof(char),5))))== 0))goto _LL190;};_LL18F:{const char*
_tmp852;void*_tmp851[1];struct Cyc_String_pa_struct _tmp850;(_tmp850.tag=0,((
_tmp850.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmp851[0]=& _tmp850,Cyc_Tcutil_warn(e->loc,((_tmp852="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp852,sizeof(char),54))),_tag_dyneither(_tmp851,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp858;struct Cyc_List_List*_tmp857;struct Cyc_Absyn_Primop_e_struct*
_tmp856;outer_e->r=(void*)((_tmp856=_cycalloc(sizeof(*_tmp856)),((_tmp856[0]=((
_tmp858.tag=3,((_tmp858.f1=Cyc_Absyn_Numelts,((_tmp858.f2=((_tmp857=_cycalloc(
sizeof(*_tmp857)),((_tmp857->hd=e,((_tmp857->tl=0,_tmp857)))))),_tmp858)))))),
_tmp856))));}return Cyc_Absyn_uint_typ;_LL190:;_LL191: {const char*_tmp859;if(Cyc_strcmp((
struct _dyneither_ptr)*f,((_tmp859="size",_tag_dyneither(_tmp859,sizeof(char),5))))
== 0){const char*_tmp85D;void*_tmp85C[1];struct Cyc_String_pa_struct _tmp85B;return(
_tmp85B.tag=0,((_tmp85B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp85C[0]=& _tmp85B,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp85D="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp85D,sizeof(char),44))),_tag_dyneither(_tmp85C,sizeof(void*),1)))))));}
else{const char*_tmp861;void*_tmp860[1];struct Cyc_String_pa_struct _tmp85F;return(
_tmp85F.tag=0,((_tmp85F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp860[0]=& _tmp85F,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp861="expecting struct or union, found %s",_tag_dyneither(
_tmp861,sizeof(char),36))),_tag_dyneither(_tmp860,sizeof(void*),1)))))));}}
_LL187:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*
is_read){struct _RegionHandle*_tmp35A=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
_tmp35A,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp35B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp35D;void*_tmp35E;struct Cyc_Absyn_PtrAtts _tmp35F;void*_tmp360;union Cyc_Absyn_Constraint*
_tmp361;union Cyc_Absyn_Constraint*_tmp362;_LL193: {struct Cyc_Absyn_PointerType_struct*
_tmp35C=(struct Cyc_Absyn_PointerType_struct*)_tmp35B;if(_tmp35C->tag != 5)goto
_LL195;else{_tmp35D=_tmp35C->f1;_tmp35E=_tmp35D.elt_typ;_tmp35F=_tmp35D.ptr_atts;
_tmp360=_tmp35F.rgn;_tmp361=_tmp35F.bounds;_tmp362=_tmp35F.zero_term;}}_LL194:
Cyc_Tcutil_check_nonzero_bound(loc,_tmp361);{void*_tmp363=Cyc_Tcutil_compress(
_tmp35E);struct Cyc_Absyn_AggrInfo _tmp365;union Cyc_Absyn_AggrInfoU _tmp366;struct
Cyc_Absyn_Aggrdecl**_tmp367;struct Cyc_Absyn_Aggrdecl*_tmp368;struct Cyc_List_List*
_tmp369;enum Cyc_Absyn_AggrKind _tmp36B;struct Cyc_List_List*_tmp36C;_LL198: {
struct Cyc_Absyn_AggrType_struct*_tmp364=(struct Cyc_Absyn_AggrType_struct*)
_tmp363;if(_tmp364->tag != 12)goto _LL19A;else{_tmp365=_tmp364->f1;_tmp366=_tmp365.aggr_info;
if((_tmp366.KnownAggr).tag != 2)goto _LL19A;_tmp367=(struct Cyc_Absyn_Aggrdecl**)(
_tmp366.KnownAggr).val;_tmp368=*_tmp367;_tmp369=_tmp365.targs;}}_LL199: {struct
Cyc_Absyn_Aggrfield*_tmp36D=Cyc_Absyn_lookup_decl_field(_tmp368,f);if(_tmp36D == 
0){const char*_tmp866;void*_tmp865[2];struct Cyc_String_pa_struct _tmp864;struct Cyc_String_pa_struct
_tmp863;return(_tmp863.tag=0,((_tmp863.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp368->name)),((_tmp865[0]=& _tmp864,((
_tmp865[1]=& _tmp863,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp866="type %s has no %s field",
_tag_dyneither(_tmp866,sizeof(char),24))),_tag_dyneither(_tmp865,sizeof(void*),2)))))))))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp368->impl))->tagged)*
is_tagged=1;{void*t3;struct _RegionHandle*_tmp372=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp373=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp372,_tmp372,_tmp368->tvs,_tmp369);t3=Cyc_Tcutil_rsubstitute(_tmp372,_tmp373,
_tmp36D->type);}if(((_tmp368->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp368->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3)){const char*_tmp86A;void*_tmp869[1];struct Cyc_String_pa_struct _tmp868;return(
_tmp868.tag=0,((_tmp868.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp869[0]=& _tmp868,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86A="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp86A,sizeof(char),56))),_tag_dyneither(_tmp869,sizeof(void*),1)))))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp368->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp86E;void*_tmp86D[1];struct Cyc_String_pa_struct _tmp86C;return(
_tmp86C.tag=0,((_tmp86C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp86D[0]=& _tmp86C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86E="must use pattern-matching to access field %s\n\tdue to extistential types",
_tag_dyneither(_tmp86E,sizeof(char),72))),_tag_dyneither(_tmp86D,sizeof(void*),1)))))));}}
return t3;};}_LL19A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp36A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp363;if(_tmp36A->tag != 13)goto _LL19C;else{_tmp36B=_tmp36A->f1;_tmp36C=_tmp36A->f2;}}
_LL19B: {struct Cyc_Absyn_Aggrfield*_tmp37A=Cyc_Absyn_lookup_field(_tmp36C,f);if(
_tmp37A == 0){const char*_tmp872;void*_tmp871[1];struct Cyc_String_pa_struct _tmp870;
return(_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
f),((_tmp871[0]=& _tmp870,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp872="type has no %s field",
_tag_dyneither(_tmp872,sizeof(char),21))),_tag_dyneither(_tmp871,sizeof(void*),1)))))));}
if((_tmp36B == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
_tmp37A->type)){const char*_tmp876;void*_tmp875[1];struct Cyc_String_pa_struct
_tmp874;return(_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp875[0]=& _tmp874,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp876="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp876,sizeof(char),56))),_tag_dyneither(_tmp875,sizeof(void*),1)))))));}return
_tmp37A->type;}_LL19C:;_LL19D: goto _LL197;_LL197:;}goto _LL192;_LL195:;_LL196: goto
_LL192;_LL192:;}{const char*_tmp87A;void*_tmp879[1];struct Cyc_String_pa_struct
_tmp878;return(_tmp878.tag=0,((_tmp878.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp879[0]=& _tmp878,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87A="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp87A,sizeof(char),44))),_tag_dyneither(_tmp879,sizeof(void*),1)))))));};}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp385;int
_tmp386;struct _tuple14 _tmp384=Cyc_Evexp_eval_const_uint_exp(index);_tmp385=
_tmp384.f1;_tmp386=_tmp384.f2;if(!_tmp386){const char*_tmp87D;void*_tmp87C;return(
_tmp87C=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp87D="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp87D,sizeof(char),47))),_tag_dyneither(_tmp87C,sizeof(void*),0)));}{
struct _handler_cons _tmp389;_push_handler(& _tmp389);{int _tmp38B=0;if(setjmp(
_tmp389.handler))_tmp38B=1;if(!_tmp38B){{void*_tmp38C=(*((struct _tuple18*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp385)).f2;_npop_handler(0);
return _tmp38C;};_pop_handler();}else{void*_tmp38A=(void*)_exn_thrown;void*
_tmp38E=_tmp38A;_LL19F: {struct Cyc_List_Nth_struct*_tmp38F=(struct Cyc_List_Nth_struct*)
_tmp38E;if(_tmp38F->tag != Cyc_List_Nth)goto _LL1A1;}_LL1A0: {const char*_tmp882;
void*_tmp881[2];struct Cyc_Int_pa_struct _tmp880;struct Cyc_Int_pa_struct _tmp87F;
return(_tmp87F.tag=1,((_tmp87F.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts),((_tmp880.tag=1,((_tmp880.f1=(unsigned long)((int)_tmp385),((
_tmp881[0]=& _tmp880,((_tmp881[1]=& _tmp87F,Cyc_Tcexp_expr_err(te,loc,0,((_tmp882="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp882,sizeof(char),41))),_tag_dyneither(_tmp881,sizeof(void*),2)))))))))))));}
_LL1A1:;_LL1A2:(void)_throw(_tmp38E);_LL19E:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp394=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp395=Cyc_Tcenv_clear_notreadctxt(_tmp394,
te_orig);Cyc_Tcexp_tcExp(_tmp395,0,e1);Cyc_Tcexp_tcExp(_tmp395,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp395,e2)){const char*_tmp886;void*_tmp885[1];
struct Cyc_String_pa_struct _tmp884;return(_tmp884.tag=0,((_tmp884.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp885[0]=&
_tmp884,Cyc_Tcexp_expr_err(_tmp395,e2->loc,topt,((_tmp886="expecting int subscript, found %s",
_tag_dyneither(_tmp886,sizeof(char),34))),_tag_dyneither(_tmp885,sizeof(void*),1)))))));}{
void*_tmp399=t1;struct Cyc_Absyn_PtrInfo _tmp39B;void*_tmp39C;struct Cyc_Absyn_Tqual
_tmp39D;struct Cyc_Absyn_PtrAtts _tmp39E;void*_tmp39F;union Cyc_Absyn_Constraint*
_tmp3A0;union Cyc_Absyn_Constraint*_tmp3A1;struct Cyc_List_List*_tmp3A3;_LL1A4: {
struct Cyc_Absyn_PointerType_struct*_tmp39A=(struct Cyc_Absyn_PointerType_struct*)
_tmp399;if(_tmp39A->tag != 5)goto _LL1A6;else{_tmp39B=_tmp39A->f1;_tmp39C=_tmp39B.elt_typ;
_tmp39D=_tmp39B.elt_tq;_tmp39E=_tmp39B.ptr_atts;_tmp39F=_tmp39E.rgn;_tmp3A0=
_tmp39E.bounds;_tmp3A1=_tmp39E.zero_term;}}_LL1A5: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3A1)){int emit_warning=1;{void*_tmp3A4=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp3A0);struct Cyc_Absyn_Exp*_tmp3A6;_LL1AB: {struct Cyc_Absyn_Upper_b_struct*
_tmp3A5=(struct Cyc_Absyn_Upper_b_struct*)_tmp3A4;if(_tmp3A5->tag != 1)goto _LL1AD;
else{_tmp3A6=_tmp3A5->f1;}}_LL1AC: if(Cyc_Tcutil_is_const_exp(_tmp395,e2)){
unsigned int _tmp3A9;int _tmp3AA;struct _tuple14 _tmp3A8=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp3A9=_tmp3A8.f1;_tmp3AA=_tmp3A8.f2;if(_tmp3AA){unsigned int _tmp3AC;int
_tmp3AD;struct _tuple14 _tmp3AB=Cyc_Evexp_eval_const_uint_exp(_tmp3A6);_tmp3AC=
_tmp3AB.f1;_tmp3AD=_tmp3AB.f2;if(_tmp3AD  && _tmp3AC > _tmp3A9)emit_warning=0;}}
goto _LL1AA;_LL1AD: {struct Cyc_Absyn_DynEither_b_struct*_tmp3A7=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp3A4;if(_tmp3A7->tag != 0)goto _LL1AA;}_LL1AE: emit_warning=0;goto _LL1AA;_LL1AA:;}
if(emit_warning){const char*_tmp889;void*_tmp888;(_tmp888=0,Cyc_Tcutil_warn(e2->loc,((
_tmp889="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp889,sizeof(char),63))),_tag_dyneither(_tmp888,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp395,e2)){unsigned int _tmp3B1;int _tmp3B2;
struct _tuple14 _tmp3B0=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3B1=_tmp3B0.f1;
_tmp3B2=_tmp3B0.f2;if(_tmp3B2)Cyc_Tcutil_check_bound(loc,_tmp3B1,_tmp3A0);}else{
if(Cyc_Tcutil_is_bound_one(_tmp3A0) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp3A1)){const char*_tmp88C;void*_tmp88B;(_tmp88B=0,
Cyc_Tcutil_warn(e1->loc,((_tmp88C="subscript applied to pointer to one object",
_tag_dyneither(_tmp88C,sizeof(char),43))),_tag_dyneither(_tmp88B,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3A0);}}Cyc_Tcenv_check_rgn_accessible(
_tmp395,loc,_tmp39F);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp39C),& Cyc_Tcutil_tmk)){
const char*_tmp88F;void*_tmp88E;(_tmp88E=0,Cyc_Tcutil_terr(e1->loc,((_tmp88F="can't subscript an abstract pointer",
_tag_dyneither(_tmp88F,sizeof(char),36))),_tag_dyneither(_tmp88E,sizeof(void*),0)));}
return _tmp39C;_LL1A6: {struct Cyc_Absyn_TupleType_struct*_tmp3A2=(struct Cyc_Absyn_TupleType_struct*)
_tmp399;if(_tmp3A2->tag != 11)goto _LL1A8;else{_tmp3A3=_tmp3A2->f1;}}_LL1A7: return
Cyc_Tcexp_ithTupleType(_tmp395,loc,_tmp3A3,e2);_LL1A8:;_LL1A9: {const char*
_tmp893;void*_tmp892[1];struct Cyc_String_pa_struct _tmp891;return(_tmp891.tag=0,((
_tmp891.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp892[0]=& _tmp891,Cyc_Tcexp_expr_err(_tmp395,loc,topt,((_tmp893="subscript applied to %s",
_tag_dyneither(_tmp893,sizeof(char),24))),_tag_dyneither(_tmp892,sizeof(void*),1)))))));}
_LL1A3:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp3BA=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3BC;
_LL1B0: {struct Cyc_Absyn_TupleType_struct*_tmp3BB=(struct Cyc_Absyn_TupleType_struct*)
_tmp3BA;if(_tmp3BB->tag != 11)goto _LL1B2;else{_tmp3BC=_tmp3BB->f1;}}_LL1B1: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3BC)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1AF;for(0;es != 0;(es=es->tl,_tmp3BC=_tmp3BC->tl)){
int bogus=0;void*_tmp3BD=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(
_tmp3BC))->hd)).f2;Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3BD,(struct Cyc_Absyn_Exp*)
es->hd);Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)
_tmp3BC->hd)).f2,& bogus);{struct _tuple18*_tmp896;struct Cyc_List_List*_tmp895;
fields=((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895->hd=((_tmp896=_cycalloc(
sizeof(*_tmp896)),((_tmp896->f1=(*((struct _tuple18*)_tmp3BC->hd)).f1,((_tmp896->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp896)))))),((_tmp895->tl=fields,_tmp895))))));};}done=1;goto _LL1AF;_LL1B2:;
_LL1B3: goto _LL1AF;_LL1AF:;}if(!done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(
te,0,(struct Cyc_Absyn_Exp*)es->hd);{struct _tuple18*_tmp899;struct Cyc_List_List*
_tmp898;fields=((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898->hd=((_tmp899=
_cycalloc(sizeof(*_tmp899)),((_tmp899->f1=Cyc_Absyn_empty_tqual(0),((_tmp899->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,
_tmp899)))))),((_tmp898->tl=fields,_tmp898))))));};}{struct Cyc_Absyn_TupleType_struct
_tmp89C;struct Cyc_Absyn_TupleType_struct*_tmp89B;return(void*)((_tmp89B=
_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89C.tag=11,((_tmp89C.f1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp89C)))),
_tmp89B))));};}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct _tuple8*t,struct Cyc_List_List*des){
const char*_tmp89F;void*_tmp89E;return(_tmp89E=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp89F="tcCompoundLit",_tag_dyneither(_tmp89F,sizeof(char),14))),_tag_dyneither(
_tmp89E,sizeof(void*),0)));}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,struct Cyc_List_List*
des){void*res_t2;struct _RegionHandle*_tmp3C6=Cyc_Tcenv_get_fnrgn(te);{int _tmp3C7=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp3C6,(struct Cyc_Absyn_Exp*(*)(
struct _tuple17*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp8A2;struct Cyc_Absyn_Const_e_struct*_tmp8A1;struct Cyc_Absyn_Const_e_struct*
_tmp3C8=(_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp8A2.tag=0,((
_tmp8A2.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3C7),_tmp8A2)))),_tmp8A1)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3C8,loc);if(zero_term){
struct Cyc_Absyn_Exp*_tmp3C9=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(es,_tmp3C7 - 1);if(!Cyc_Tcutil_is_zero(_tmp3C9)){const char*_tmp8A5;
void*_tmp8A4;(_tmp8A4=0,Cyc_Tcutil_terr(_tmp3C9->loc,((_tmp8A5="zero-terminated array doesn't end with zero.",
_tag_dyneither(_tmp8A5,sizeof(char),45))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}}{
struct Cyc_Core_Opt*_tmp8A6;sz_exp->topt=((_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((
_tmp8A6->v=Cyc_Absyn_uint_typ,_tmp8A6))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp8AA;void*_tmp8A9[1];struct Cyc_String_pa_struct _tmp8A8;(
_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp8A9[0]=& _tmp8A8,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp8AA="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp8AA,sizeof(char),53))),_tag_dyneither(_tmp8A9,sizeof(void*),1)))))));}}{
int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((
struct _tuple17*)des->hd)).f1;if(ds != 0){void*_tmp3D2=(void*)ds->hd;struct Cyc_Absyn_Exp*
_tmp3D5;_LL1B5: {struct Cyc_Absyn_FieldName_struct*_tmp3D3=(struct Cyc_Absyn_FieldName_struct*)
_tmp3D2;if(_tmp3D3->tag != 1)goto _LL1B7;}_LL1B6:{const char*_tmp8AD;void*_tmp8AC;(
_tmp8AC=0,Cyc_Tcutil_terr(loc,((_tmp8AD="only array index designators are supported",
_tag_dyneither(_tmp8AD,sizeof(char),43))),_tag_dyneither(_tmp8AC,sizeof(void*),0)));}
goto _LL1B4;_LL1B7: {struct Cyc_Absyn_ArrayElement_struct*_tmp3D4=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp3D2;if(_tmp3D4->tag != 0)goto _LL1B4;else{_tmp3D5=_tmp3D4->f1;}}_LL1B8: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3D5);{unsigned int _tmp3D9;int _tmp3DA;struct _tuple14 _tmp3D8=Cyc_Evexp_eval_const_uint_exp(
_tmp3D5);_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;if(!_tmp3DA){const char*_tmp8B0;
void*_tmp8AF;(_tmp8AF=0,Cyc_Tcutil_terr(_tmp3D5->loc,((_tmp8B0="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp8B0,sizeof(char),47))),_tag_dyneither(_tmp8AF,sizeof(void*),0)));}
else{if(_tmp3D9 != offset){const char*_tmp8B5;void*_tmp8B4[2];struct Cyc_Int_pa_struct
_tmp8B3;struct Cyc_Int_pa_struct _tmp8B2;(_tmp8B2.tag=1,((_tmp8B2.f1=(
unsigned long)((int)_tmp3D9),((_tmp8B3.tag=1,((_tmp8B3.f1=(unsigned long)offset,((
_tmp8B4[0]=& _tmp8B3,((_tmp8B4[1]=& _tmp8B2,Cyc_Tcutil_terr(_tmp3D5->loc,((_tmp8B5="expecting index designator %d but found %d",
_tag_dyneither(_tmp8B5,sizeof(char),43))),_tag_dyneither(_tmp8B4,sizeof(void*),2)))))))))))));}}
goto _LL1B4;};_LL1B4:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3E1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1BA: {struct Cyc_Absyn_TagType_struct*_tmp3E2=(
struct Cyc_Absyn_TagType_struct*)_tmp3E1;if(_tmp3E2->tag != 20)goto _LL1BC;}_LL1BB:
goto _LL1B9;_LL1BC:;_LL1BD: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*
_tmp8B9;void*_tmp8B8[1];struct Cyc_String_pa_struct _tmp8B7;(_tmp8B7.tag=0,((
_tmp8B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v)),((_tmp8B8[0]=& _tmp8B7,
Cyc_Tcutil_terr(bound->loc,((_tmp8B9="expecting unsigned int, found %s",
_tag_dyneither(_tmp8B9,sizeof(char),33))),_tag_dyneither(_tmp8B8,sizeof(void*),1)))))));}
_LL1B9:;}if(!(vd->tq).real_const){const char*_tmp8BC;void*_tmp8BB;(_tmp8BB=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp8BC="comprehension index variable is not declared const!",_tag_dyneither(
_tmp8BC,sizeof(char),52))),_tag_dyneither(_tmp8BB,sizeof(void*),0)));}{struct
_RegionHandle*_tmp3E8=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(
_tmp3E8,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(_tmp3E8,loc,te2);te2=Cyc_Tcenv_add_local_var(
_tmp3E8,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*_tmp3E9=te2;struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp3EA=0;struct Cyc_Absyn_Tqual*_tmp3EB=0;union Cyc_Absyn_Constraint**
_tmp3EC=0;if(topt != 0){void*_tmp3ED=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp3EF;struct Cyc_Absyn_ArrayInfo _tmp3F1;void*_tmp3F2;void**_tmp3F3;struct Cyc_Absyn_Tqual
_tmp3F4;struct Cyc_Absyn_Tqual*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;union Cyc_Absyn_Constraint*
_tmp3F7;union Cyc_Absyn_Constraint**_tmp3F8;_LL1BF: {struct Cyc_Absyn_PointerType_struct*
_tmp3EE=(struct Cyc_Absyn_PointerType_struct*)_tmp3ED;if(_tmp3EE->tag != 5)goto
_LL1C1;else{_tmp3EF=_tmp3EE->f1;}}_LL1C0: pinfo=_tmp3EF;_tmp3EA=(void**)& pinfo.elt_typ;
_tmp3EB=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;_tmp3EC=(union Cyc_Absyn_Constraint**)&(
pinfo.ptr_atts).zero_term;goto _LL1BE;_LL1C1: {struct Cyc_Absyn_ArrayType_struct*
_tmp3F0=(struct Cyc_Absyn_ArrayType_struct*)_tmp3ED;if(_tmp3F0->tag != 9)goto
_LL1C3;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F1.elt_type;_tmp3F3=(void**)&(
_tmp3F0->f1).elt_type;_tmp3F4=_tmp3F1.tq;_tmp3F5=(struct Cyc_Absyn_Tqual*)&(
_tmp3F0->f1).tq;_tmp3F6=_tmp3F1.num_elts;_tmp3F7=_tmp3F1.zero_term;_tmp3F8=(
union Cyc_Absyn_Constraint**)&(_tmp3F0->f1).zero_term;}}_LL1C2: _tmp3EA=(void**)
_tmp3F3;_tmp3EB=(struct Cyc_Absyn_Tqual*)_tmp3F5;_tmp3EC=(union Cyc_Absyn_Constraint**)
_tmp3F8;goto _LL1BE;_LL1C3:;_LL1C4: goto _LL1BE;_LL1BE:;}{void*t=Cyc_Tcexp_tcExp(
_tmp3E9,_tmp3EA,body);if(_tmp3E9->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp3E9,
bound)){const char*_tmp8BF;void*_tmp8BE;(_tmp8BE=0,Cyc_Tcutil_terr(bound->loc,((
_tmp8BF="bound is not constant",_tag_dyneither(_tmp8BF,sizeof(char),22))),
_tag_dyneither(_tmp8BE,sizeof(void*),0)));}if(!Cyc_Tcutil_is_const_exp(_tmp3E9,
body)){const char*_tmp8C2;void*_tmp8C1;(_tmp8C1=0,Cyc_Tcutil_terr(bound->loc,((
_tmp8C2="body is not constant",_tag_dyneither(_tmp8C2,sizeof(char),21))),
_tag_dyneither(_tmp8C1,sizeof(void*),0)));}}if(_tmp3EC != 0  && ((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3EC)){struct Cyc_Absyn_Exp*
_tmp3FD=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*_tmp8C3;_tmp3FD->topt=((
_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3->v=Cyc_Absyn_uint_typ,_tmp8C3))));}
bound=Cyc_Absyn_add_exp(bound,_tmp3FD,0);{struct Cyc_Core_Opt*_tmp8C4;bound->topt=((
_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4->v=Cyc_Absyn_uint_typ,_tmp8C4))));}*
is_zero_term=1;}{void*_tmp400=Cyc_Absyn_array_typ(t,_tmp3EB == 0?Cyc_Absyn_empty_tqual(
0):*_tmp3EB,(struct Cyc_Absyn_Exp*)bound,_tmp3EC == 0?Cyc_Absyn_false_conref:*
_tmp3EC,0);return _tmp400;};};};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp401;_push_handler(& _tmp401);{int _tmp403=0;if(setjmp(
_tmp401.handler))_tmp403=1;if(!_tmp403){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp402=(void*)_exn_thrown;void*_tmp405=_tmp402;_LL1C6: {
struct Cyc_Dict_Absent_struct*_tmp406=(struct Cyc_Dict_Absent_struct*)_tmp405;if(
_tmp406->tag != Cyc_Dict_Absent)goto _LL1C8;}_LL1C7:{const char*_tmp8C8;void*
_tmp8C7[1];struct Cyc_String_pa_struct _tmp8C6;(_tmp8C6.tag=0,((_tmp8C6.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8C7[0]=&
_tmp8C6,Cyc_Tcutil_terr(loc,((_tmp8C8="unbound struct/union name %s",
_tag_dyneither(_tmp8C8,sizeof(char),29))),_tag_dyneither(_tmp8C7,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1C8:;_LL1C9:(void)_throw(
_tmp405);_LL1C5:;}};}*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl
== 0){{const char*_tmp8D0;void*_tmp8CF[1];const char*_tmp8CE;const char*_tmp8CD;
struct Cyc_String_pa_struct _tmp8CC;(_tmp8CC.tag=0,((_tmp8CC.f1=(struct
_dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8CD="struct",_tag_dyneither(
_tmp8CD,sizeof(char),7)):((_tmp8CE="union",_tag_dyneither(_tmp8CE,sizeof(char),6)))),((
_tmp8CF[0]=& _tmp8CC,Cyc_Tcutil_terr(loc,((_tmp8D0="can't construct abstract %s",
_tag_dyneither(_tmp8D0,sizeof(char),28))),_tag_dyneither(_tmp8CF,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp40F=Cyc_Tcenv_get_fnrgn(te);struct _tuple11 _tmp8D1;struct
_tuple11 _tmp410=(_tmp8D1.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8D1.f2=_tmp40F,
_tmp8D1)));struct Cyc_List_List*_tmp411=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp40F,Cyc_Tcutil_r_make_inst_var,&
_tmp410,ad->tvs);struct Cyc_List_List*_tmp412=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp40F,Cyc_Tcutil_r_make_inst_var,&
_tmp410,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp413=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp411);struct Cyc_List_List*_tmp414=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,
_tmp412);struct Cyc_List_List*_tmp415=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp40F,_tmp411,_tmp412);struct Cyc_Absyn_AggrType_struct _tmp8DB;struct Cyc_Absyn_Aggrdecl**
_tmp8DA;struct Cyc_Absyn_AggrInfo _tmp8D9;struct Cyc_Absyn_AggrType_struct*_tmp8D8;
struct Cyc_Absyn_AggrType_struct*_tmp416=(_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((
_tmp8D8[0]=((_tmp8DB.tag=12,((_tmp8DB.f1=((_tmp8D9.aggr_info=Cyc_Absyn_KnownAggr(((
_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=ad,_tmp8DA))))),((_tmp8D9.targs=
_tmp413,_tmp8D9)))),_tmp8DB)))),_tmp8D8)));struct Cyc_List_List*_tmp417=*ts;
struct Cyc_List_List*_tmp418=_tmp414;while(_tmp417 != 0  && _tmp418 != 0){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp417->hd);Cyc_Tcutil_unify((
void*)_tmp417->hd,(void*)_tmp418->hd);_tmp417=_tmp417->tl;_tmp418=_tmp418->tl;}
if(_tmp417 != 0){const char*_tmp8DE;void*_tmp8DD;(_tmp8DD=0,Cyc_Tcutil_terr(loc,((
_tmp8DE="too many explicit witness types",_tag_dyneither(_tmp8DE,sizeof(char),32))),
_tag_dyneither(_tmp8DD,sizeof(void*),0)));}*ts=_tmp414;if(topt != 0)Cyc_Tcutil_unify((
void*)_tmp416,*topt);{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp40F,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){int bogus=0;struct _tuple19 _tmp41C;struct Cyc_Absyn_Aggrfield*
_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct _tuple19*_tmp41B=(struct _tuple19*)
fields->hd;_tmp41C=*_tmp41B;_tmp41D=_tmp41C.f1;_tmp41E=_tmp41C.f2;{void*_tmp41F=
Cyc_Tcutil_rsubstitute(_tmp40F,_tmp415,_tmp41D->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp41F,_tmp41E);if(!Cyc_Tcutil_coerce_arg(te,_tmp41E,_tmp41F,& bogus)){{
const char*_tmp8EA;void*_tmp8E9[5];struct Cyc_String_pa_struct _tmp8E8;const char*
_tmp8E7;const char*_tmp8E6;struct Cyc_String_pa_struct _tmp8E5;struct Cyc_String_pa_struct
_tmp8E4;struct Cyc_String_pa_struct _tmp8E3;struct Cyc_String_pa_struct _tmp8E2;(
_tmp8E2.tag=0,((_tmp8E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp41E->topt))->v)),((_tmp8E3.tag=0,((
_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp41F)),((_tmp8E4.tag=0,((_tmp8E4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8E5.tag=0,((_tmp8E5.f1=(
struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8E6="struct",
_tag_dyneither(_tmp8E6,sizeof(char),7)):((_tmp8E7="union",_tag_dyneither(_tmp8E7,
sizeof(char),6)))),((_tmp8E8.tag=0,((_tmp8E8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp41D->name),((_tmp8E9[0]=& _tmp8E8,((_tmp8E9[1]=& _tmp8E5,((
_tmp8E9[2]=& _tmp8E4,((_tmp8E9[3]=& _tmp8E3,((_tmp8E9[4]=& _tmp8E2,Cyc_Tcutil_terr(
_tmp41E->loc,((_tmp8EA="field %s of %s %s expects type %s != %s",_tag_dyneither(
_tmp8EA,sizeof(char),40))),_tag_dyneither(_tmp8E9,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*_tmp429=0;{struct Cyc_List_List*
_tmp42A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp42A != 0;_tmp42A=_tmp42A->tl){struct _tuple0*_tmp8ED;struct Cyc_List_List*
_tmp8EC;_tmp429=((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC->hd=((_tmp8ED=
_cycalloc(sizeof(*_tmp8ED)),((_tmp8ED->f1=Cyc_Tcutil_rsubstitute(_tmp40F,_tmp415,(*((
struct _tuple0*)_tmp42A->hd)).f1),((_tmp8ED->f2=Cyc_Tcutil_rsubstitute(_tmp40F,
_tmp415,(*((struct _tuple0*)_tmp42A->hd)).f2),_tmp8ED)))))),((_tmp8EC->tl=_tmp429,
_tmp8EC))))));}}_tmp429=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp429);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp429);return(void*)_tmp416;};};};}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){struct _RegionHandle*_tmp432=Cyc_Tcenv_get_fnrgn(
te);{void*_tmp433=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp435;struct
Cyc_List_List*_tmp436;_LL1CB: {struct Cyc_Absyn_AnonAggrType_struct*_tmp434=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp433;if(_tmp434->tag != 13)goto _LL1CD;
else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;}}_LL1CC: if(_tmp435 == Cyc_Absyn_UnionA){
const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_Tcutil_terr(loc,((_tmp8F0="expecting struct but found union",
_tag_dyneither(_tmp8F0,sizeof(char),33))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp432,
loc,args,Cyc_Absyn_StructA,_tmp436);for(0;fields != 0;fields=fields->tl){int bogus=
0;struct _tuple19 _tmp43A;struct Cyc_Absyn_Aggrfield*_tmp43B;struct Cyc_Absyn_Exp*
_tmp43C;struct _tuple19*_tmp439=(struct _tuple19*)fields->hd;_tmp43A=*_tmp439;
_tmp43B=_tmp43A.f1;_tmp43C=_tmp43A.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)&
_tmp43B->type,_tmp43C);if(!Cyc_Tcutil_coerce_arg(te,_tmp43C,_tmp43B->type,& bogus)){{
const char*_tmp8F6;void*_tmp8F5[3];struct Cyc_String_pa_struct _tmp8F4;struct Cyc_String_pa_struct
_tmp8F3;struct Cyc_String_pa_struct _tmp8F2;(_tmp8F2.tag=0,((_tmp8F2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp43C->topt))->v)),((_tmp8F3.tag=0,((_tmp8F3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp43B->type)),((
_tmp8F4.tag=0,((_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp43B->name),((
_tmp8F5[0]=& _tmp8F4,((_tmp8F5[1]=& _tmp8F3,((_tmp8F5[2]=& _tmp8F2,Cyc_Tcutil_terr(
_tmp43C->loc,((_tmp8F6="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp8F6,sizeof(char),41))),_tag_dyneither(_tmp8F5,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1CA;};_LL1CD:;_LL1CE: {const char*_tmp8F9;
void*_tmp8F8;(_tmp8F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp8F9="tcAnonStruct: wrong type",_tag_dyneither(_tmp8F9,
sizeof(char),25))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}_LL1CA:;}return ts;}
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp444=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp8FA;struct _tuple11 _tmp445=(_tmp8FA.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp8FA.f2=_tmp444,_tmp8FA)));struct Cyc_List_List*_tmp446=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp444,Cyc_Tcutil_r_make_inst_var,&
_tmp445,tud->tvs);struct Cyc_List_List*_tmp447=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,_tmp446);struct Cyc_Absyn_DatatypeFieldType_struct _tmp900;struct Cyc_Absyn_DatatypeFieldInfo
_tmp8FF;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp8FE;void*res=(void*)((
_tmp8FE=_cycalloc(sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp900.tag=4,((_tmp900.f1=((
_tmp8FF.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp8FF.targs=_tmp447,
_tmp8FF)))),_tmp900)))),_tmp8FE))));if(topt != 0){void*_tmp448=Cyc_Tcutil_compress(*
topt);_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp449=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp448;if(_tmp449->tag != 4)goto _LL1D2;}_LL1D1: Cyc_Tcutil_unify(*topt,res);goto
_LL1CF;_LL1D2:;_LL1D3: goto _LL1CF;_LL1CF:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){int bogus=0;struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp444,_tmp446,(*((
struct _tuple18*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t,& bogus)){{const char*_tmp908;void*_tmp907[3];struct Cyc_String_pa_struct
_tmp906;struct Cyc_String_pa_struct _tmp905;const char*_tmp904;struct Cyc_String_pa_struct
_tmp903;(_tmp903.tag=0,((_tmp903.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
e->topt == 0?(struct _dyneither_ptr)((_tmp904="?",_tag_dyneither(_tmp904,sizeof(
char),2))): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp905.tag=0,((_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp906.tag=0,((_tmp906.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp907[0]=& _tmp906,((_tmp907[1]=& _tmp905,((
_tmp907[2]=& _tmp903,Cyc_Tcutil_terr(e->loc,((_tmp908="datatype constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp908,sizeof(char),82))),_tag_dyneither(_tmp907,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp90C;void*_tmp90B[1];
struct Cyc_String_pa_struct _tmp90A;return(_tmp90A.tag=0,((_tmp90A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp90B[0]=& _tmp90A,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp90C="too many arguments for datatype constructor %s",_tag_dyneither(
_tmp90C,sizeof(char),47))),_tag_dyneither(_tmp90B,sizeof(void*),1)))))));}if(ts
!= 0){const char*_tmp910;void*_tmp90F[1];struct Cyc_String_pa_struct _tmp90E;return(
_tmp90E.tag=0,((_tmp90E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name)),((_tmp90F[0]=& _tmp90E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp910="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp910,sizeof(char),46))),_tag_dyneither(_tmp90F,sizeof(void*),1)))))));}
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp45A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp461;struct Cyc_Absyn_PtrAtts _tmp462;union Cyc_Absyn_Constraint*
_tmp463;struct Cyc_Absyn_ArrayInfo _tmp468;void*_tmp469;struct Cyc_List_List*
_tmp46C;struct Cyc_Absyn_AggrInfo _tmp46E;union Cyc_Absyn_AggrInfoU _tmp46F;struct
Cyc_List_List*_tmp470;struct Cyc_List_List*_tmp474;_LL1D5: {struct Cyc_Absyn_VoidType_struct*
_tmp45B=(struct Cyc_Absyn_VoidType_struct*)_tmp45A;if(_tmp45B->tag != 0)goto _LL1D7;}
_LL1D6: return 1;_LL1D7: {struct Cyc_Absyn_Evar_struct*_tmp45C=(struct Cyc_Absyn_Evar_struct*)
_tmp45A;if(_tmp45C->tag != 1)goto _LL1D9;}_LL1D8: goto _LL1DA;_LL1D9: {struct Cyc_Absyn_VarType_struct*
_tmp45D=(struct Cyc_Absyn_VarType_struct*)_tmp45A;if(_tmp45D->tag != 2)goto _LL1DB;}
_LL1DA: goto _LL1DC;_LL1DB: {struct Cyc_Absyn_DatatypeType_struct*_tmp45E=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp45A;if(_tmp45E->tag != 3)goto _LL1DD;}_LL1DC:
goto _LL1DE;_LL1DD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp45F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp45A;if(_tmp45F->tag != 4)goto _LL1DF;}_LL1DE: return 0;_LL1DF: {struct Cyc_Absyn_PointerType_struct*
_tmp460=(struct Cyc_Absyn_PointerType_struct*)_tmp45A;if(_tmp460->tag != 5)goto
_LL1E1;else{_tmp461=_tmp460->f1;_tmp462=_tmp461.ptr_atts;_tmp463=_tmp462.nullable;}}
_LL1E0: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp463);_LL1E1: {struct Cyc_Absyn_IntType_struct*_tmp464=(struct Cyc_Absyn_IntType_struct*)
_tmp45A;if(_tmp464->tag != 6)goto _LL1E3;}_LL1E2: goto _LL1E4;_LL1E3: {struct Cyc_Absyn_FloatType_struct*
_tmp465=(struct Cyc_Absyn_FloatType_struct*)_tmp45A;if(_tmp465->tag != 7)goto
_LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_Absyn_DoubleType_struct*_tmp466=(
struct Cyc_Absyn_DoubleType_struct*)_tmp45A;if(_tmp466->tag != 8)goto _LL1E7;}
_LL1E6: return 1;_LL1E7: {struct Cyc_Absyn_ArrayType_struct*_tmp467=(struct Cyc_Absyn_ArrayType_struct*)
_tmp45A;if(_tmp467->tag != 9)goto _LL1E9;else{_tmp468=_tmp467->f1;_tmp469=_tmp468.elt_type;}}
_LL1E8: return Cyc_Tcexp_zeroable_type(_tmp469);_LL1E9: {struct Cyc_Absyn_FnType_struct*
_tmp46A=(struct Cyc_Absyn_FnType_struct*)_tmp45A;if(_tmp46A->tag != 10)goto _LL1EB;}
_LL1EA: return 0;_LL1EB: {struct Cyc_Absyn_TupleType_struct*_tmp46B=(struct Cyc_Absyn_TupleType_struct*)
_tmp45A;if(_tmp46B->tag != 11)goto _LL1ED;else{_tmp46C=_tmp46B->f1;}}_LL1EC: for(0;(
unsigned int)_tmp46C;_tmp46C=_tmp46C->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple18*)_tmp46C->hd)).f2))return 0;}return 1;_LL1ED: {struct Cyc_Absyn_AggrType_struct*
_tmp46D=(struct Cyc_Absyn_AggrType_struct*)_tmp45A;if(_tmp46D->tag != 12)goto
_LL1EF;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46E.aggr_info;_tmp470=_tmp46E.targs;}}
_LL1EE: {struct Cyc_Absyn_Aggrdecl*_tmp480=Cyc_Absyn_get_known_aggrdecl(_tmp46F);
if(_tmp480->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp480->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp481=_new_region("r");struct _RegionHandle*r=&
_tmp481;_push_region(r);{struct Cyc_List_List*_tmp482=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp480->tvs,_tmp470);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp480->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp482,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp483=0;_npop_handler(0);return _tmp483;}}}{int _tmp484=1;_npop_handler(0);
return _tmp484;};};_pop_region(r);};}_LL1EF: {struct Cyc_Absyn_EnumType_struct*
_tmp471=(struct Cyc_Absyn_EnumType_struct*)_tmp45A;if(_tmp471->tag != 14)goto
_LL1F1;}_LL1F0: return 1;_LL1F1: {struct Cyc_Absyn_TagType_struct*_tmp472=(struct
Cyc_Absyn_TagType_struct*)_tmp45A;if(_tmp472->tag != 20)goto _LL1F3;}_LL1F2: return
1;_LL1F3: {struct Cyc_Absyn_AnonAggrType_struct*_tmp473=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp45A;if(_tmp473->tag != 13)goto _LL1F5;else{_tmp474=_tmp473->f2;}}_LL1F4: for(0;
_tmp474 != 0;_tmp474=_tmp474->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp474->hd)->type))return 0;}return 1;_LL1F5: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp475=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp45A;if(_tmp475->tag != 15)goto
_LL1F7;}_LL1F6: return 1;_LL1F7: {struct Cyc_Absyn_TypedefType_struct*_tmp476=(
struct Cyc_Absyn_TypedefType_struct*)_tmp45A;if(_tmp476->tag != 18)goto _LL1F9;}
_LL1F8: return 0;_LL1F9: {struct Cyc_Absyn_DynRgnType_struct*_tmp477=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp45A;if(_tmp477->tag != 17)goto _LL1FB;}_LL1FA: return 0;_LL1FB: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp478=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp45A;if(_tmp478->tag != 16)goto
_LL1FD;}_LL1FC: return 0;_LL1FD: {struct Cyc_Absyn_TypeDeclType_struct*_tmp479=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp45A;if(_tmp479->tag != 26)goto _LL1FF;}
_LL1FE: goto _LL200;_LL1FF: {struct Cyc_Absyn_ValueofType_struct*_tmp47A=(struct Cyc_Absyn_ValueofType_struct*)
_tmp45A;if(_tmp47A->tag != 19)goto _LL201;}_LL200: goto _LL202;_LL201: {struct Cyc_Absyn_HeapRgn_struct*
_tmp47B=(struct Cyc_Absyn_HeapRgn_struct*)_tmp45A;if(_tmp47B->tag != 21)goto _LL203;}
_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_UniqueRgn_struct*_tmp47C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp45A;if(_tmp47C->tag != 22)goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_AccessEff_struct*
_tmp47D=(struct Cyc_Absyn_AccessEff_struct*)_tmp45A;if(_tmp47D->tag != 23)goto
_LL207;}_LL206: goto _LL208;_LL207: {struct Cyc_Absyn_JoinEff_struct*_tmp47E=(
struct Cyc_Absyn_JoinEff_struct*)_tmp45A;if(_tmp47E->tag != 24)goto _LL209;}_LL208:
goto _LL20A;_LL209: {struct Cyc_Absyn_RgnsEff_struct*_tmp47F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp45A;if(_tmp47F->tag != 25)goto _LL1D4;}_LL20A: {const char*_tmp914;void*_tmp913[
1];struct Cyc_String_pa_struct _tmp912;(_tmp912.tag=0,((_tmp912.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp913[0]=&
_tmp912,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp914="bad type `%s' in zeroable type",_tag_dyneither(_tmp914,sizeof(char),31))),
_tag_dyneither(_tmp913,sizeof(void*),1)))))));}_LL1D4:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp488=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp48A;void*_tmp48B;_LL20C: {
struct Cyc_Absyn_PointerType_struct*_tmp489=(struct Cyc_Absyn_PointerType_struct*)
_tmp488;if(_tmp489->tag != 5)goto _LL20E;else{_tmp48A=_tmp489->f1;_tmp48B=_tmp48A.elt_typ;}}
_LL20D: Cyc_Tcutil_unify(_tmp48B,t);if(Cyc_Tcutil_bits_only(t) || allow_zero  && 
Cyc_Tcexp_zeroable_type(t))return;goto _LL20B;_LL20E:;_LL20F: goto _LL20B;_LL20B:;}{
const char*_tmp91D;void*_tmp91C[2];const char*_tmp91B;const char*_tmp91A;struct Cyc_String_pa_struct
_tmp919;struct Cyc_String_pa_struct _tmp918;(_tmp918.tag=0,((_tmp918.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp919.tag=0,((
_tmp919.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp91A="calloc",_tag_dyneither(_tmp91A,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp91B="malloc",_tag_dyneither(_tmp91B,sizeof(char),7))))),((
_tmp91C[0]=& _tmp919,((_tmp91C[1]=& _tmp918,Cyc_Tcutil_terr(loc,((_tmp91D="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp91D,sizeof(char),60))),_tag_dyneither(_tmp91C,sizeof(void*),2)))))))))))));};}
static void*Cyc_Tcexp_mallocRgn(void*rgn){switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(
rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL210: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL211: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(*ropt != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp920;
struct Cyc_Absyn_RgnHandleType_struct*_tmp91F;void*expected_type=(void*)((_tmp91F=
_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp920.tag=16,((_tmp920.f1=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp920)))),_tmp91F))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));void*_tmp492=Cyc_Tcutil_compress(
handle_type);void*_tmp494;_LL214: {struct Cyc_Absyn_RgnHandleType_struct*_tmp493=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp492;if(_tmp493->tag != 16)goto _LL216;
else{_tmp494=(void*)_tmp493->f1;}}_LL215: rgn=_tmp494;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL213;_LL216:;_LL217:{const char*_tmp924;void*_tmp923[1];struct
Cyc_String_pa_struct _tmp922;(_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp923[0]=& _tmp922,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp924="expecting region_t type but found %s",
_tag_dyneither(_tmp924,sizeof(char),37))),_tag_dyneither(_tmp923,sizeof(void*),1)))))));}
goto _LL213;_LL213:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn != (void*)& Cyc_Absyn_HeapRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*elt_type;struct Cyc_Absyn_Exp*
num_elts;int one_elt;if(*is_calloc){if(*t == 0){const char*_tmp927;void*_tmp926;(
_tmp926=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp927="calloc with empty type",_tag_dyneither(_tmp927,sizeof(char),23))),
_tag_dyneither(_tmp926,sizeof(void*),0)));}elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,
elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;one_elt=0;}
else{void*_tmp49C=(*e)->r;void*_tmp49E;enum Cyc_Absyn_Primop _tmp4A0;struct Cyc_List_List*
_tmp4A1;struct Cyc_List_List _tmp4A2;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_List_List*
_tmp4A4;struct Cyc_List_List _tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_List_List*
_tmp4A7;_LL219: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp49D=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp49C;if(_tmp49D->tag != 18)goto _LL21B;else{_tmp49E=(void*)_tmp49D->f1;}}_LL21A:
elt_type=_tmp49E;{void**_tmp928;*t=(void**)((_tmp928=_cycalloc(sizeof(*_tmp928)),((
_tmp928[0]=elt_type,_tmp928))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL218;_LL21B: {struct Cyc_Absyn_Primop_e_struct*
_tmp49F=(struct Cyc_Absyn_Primop_e_struct*)_tmp49C;if(_tmp49F->tag != 3)goto _LL21D;
else{_tmp4A0=_tmp49F->f1;if(_tmp4A0 != Cyc_Absyn_Times)goto _LL21D;_tmp4A1=_tmp49F->f2;
if(_tmp4A1 == 0)goto _LL21D;_tmp4A2=*_tmp4A1;_tmp4A3=(struct Cyc_Absyn_Exp*)_tmp4A2.hd;
_tmp4A4=_tmp4A2.tl;if(_tmp4A4 == 0)goto _LL21D;_tmp4A5=*_tmp4A4;_tmp4A6=(struct Cyc_Absyn_Exp*)
_tmp4A5.hd;_tmp4A7=_tmp4A5.tl;if(_tmp4A7 != 0)goto _LL21D;}}_LL21C:{struct _tuple0
_tmp929;struct _tuple0 _tmp4AA=(_tmp929.f1=_tmp4A3->r,((_tmp929.f2=_tmp4A6->r,
_tmp929)));void*_tmp4AB;void*_tmp4AD;void*_tmp4AE;void*_tmp4B0;_LL220: _tmp4AB=
_tmp4AA.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4AC=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AB;if(_tmp4AC->tag != 18)goto _LL222;else{_tmp4AD=(void*)_tmp4AC->f1;}};
_LL221: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4AD);elt_type=_tmp4AD;{void**
_tmp92A;*t=(void**)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=elt_type,
_tmp92A))));}num_elts=_tmp4A6;one_elt=0;goto _LL21F;_LL222: _tmp4AE=_tmp4AA.f2;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4AF=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp4AE;if(_tmp4AF->tag != 18)goto _LL224;else{_tmp4B0=(void*)_tmp4AF->f1;}};
_LL223: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4B0);elt_type=_tmp4B0;{void**
_tmp92B;*t=(void**)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=elt_type,
_tmp92B))));}num_elts=_tmp4A3;one_elt=0;goto _LL21F;_LL224:;_LL225: goto No_sizeof;
_LL21F:;}goto _LL218;_LL21D:;_LL21E: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**
_tmp92C;*t=(void**)((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C[0]=elt_type,
_tmp92C))));}num_elts=*e;one_elt=0;goto _LL218;_LL218:;}*e=num_elts;*is_fat=!
one_elt;{void*_tmp4B4=elt_type;struct Cyc_Absyn_AggrInfo _tmp4B6;union Cyc_Absyn_AggrInfoU
_tmp4B7;struct Cyc_Absyn_Aggrdecl**_tmp4B8;struct Cyc_Absyn_Aggrdecl*_tmp4B9;
_LL227: {struct Cyc_Absyn_AggrType_struct*_tmp4B5=(struct Cyc_Absyn_AggrType_struct*)
_tmp4B4;if(_tmp4B5->tag != 12)goto _LL229;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B6.aggr_info;
if((_tmp4B7.KnownAggr).tag != 2)goto _LL229;_tmp4B8=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4B7.KnownAggr).val;_tmp4B9=*_tmp4B8;}}_LL228: if(_tmp4B9->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B9->impl))->exist_vars != 0){const
char*_tmp92F;void*_tmp92E;(_tmp92E=0,Cyc_Tcutil_terr(loc,((_tmp92F="malloc with existential types not yet implemented",
_tag_dyneither(_tmp92F,sizeof(char),50))),_tag_dyneither(_tmp92E,sizeof(void*),0)));}
goto _LL226;_LL229:;_LL22A: goto _LL226;_LL226:;}{void*(*_tmp4BC)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4BD=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp4BE=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4C0;struct Cyc_Absyn_PtrAtts
_tmp4C1;union Cyc_Absyn_Constraint*_tmp4C2;union Cyc_Absyn_Constraint*_tmp4C3;
union Cyc_Absyn_Constraint*_tmp4C4;_LL22C: {struct Cyc_Absyn_PointerType_struct*
_tmp4BF=(struct Cyc_Absyn_PointerType_struct*)_tmp4BE;if(_tmp4BF->tag != 5)goto
_LL22E;else{_tmp4C0=_tmp4BF->f1;_tmp4C1=_tmp4C0.ptr_atts;_tmp4C2=_tmp4C1.nullable;
_tmp4C3=_tmp4C1.bounds;_tmp4C4=_tmp4C1.zero_term;}}_LL22D: _tmp4BD=_tmp4C4;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4C2))
_tmp4BC=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp4C4) && !(*is_calloc)){{const char*_tmp932;void*_tmp931;(_tmp931=0,Cyc_Tcutil_warn(
loc,((_tmp932="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp932,sizeof(char),55))),_tag_dyneither(_tmp931,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp4C7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4C3);struct Cyc_Absyn_Exp*_tmp4CA;_LL231: {struct Cyc_Absyn_DynEither_b_struct*
_tmp4C8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp4C7;if(_tmp4C8->tag != 0)goto
_LL233;}_LL232: goto _LL230;_LL233:{struct Cyc_Absyn_Upper_b_struct*_tmp4C9=(struct
Cyc_Absyn_Upper_b_struct*)_tmp4C7;if(_tmp4C9->tag != 1)goto _LL235;else{_tmp4CA=
_tmp4C9->f1;}}if(!(!one_elt))goto _LL235;_LL234: {int _tmp4CB=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp4CB  && Cyc_Evexp_same_const_exp(_tmp4CA,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4C3),_tmp4BD);}{void*_tmp4CC=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4CE;_LL238: {struct Cyc_Absyn_TagType_struct*_tmp4CD=(struct Cyc_Absyn_TagType_struct*)
_tmp4CC;if(_tmp4CD->tag != 20)goto _LL23A;else{_tmp4CE=(void*)_tmp4CD->f1;}}_LL239: {
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4CE,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4CF,
_tmp4CA)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4C3),_tmp4BD);}
goto _LL237;}_LL23A:;_LL23B: goto _LL237;_LL237:;}goto _LL230;}_LL235:;_LL236: goto
_LL230;_LL230:;}goto _LL22B;_LL22E:;_LL22F: goto _LL22B;_LL22B:;}if(!one_elt)
_tmp4BC=Cyc_Absyn_dyneither_typ;return _tmp4BC(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp4BD);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct
_RegionHandle*_tmp4D0=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*_tmp4D1=Cyc_Tcenv_enter_notreadctxt(
_tmp4D0,te);Cyc_Tcexp_tcExp(_tmp4D1,0,e1);Cyc_Tcexp_tcExp(_tmp4D1,(void**)((void**)((
void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);}{void*t1=(void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp4D2=Cyc_Tcutil_compress(t1);_LL23D: {struct
Cyc_Absyn_ArrayType_struct*_tmp4D3=(struct Cyc_Absyn_ArrayType_struct*)_tmp4D2;
if(_tmp4D3->tag != 9)goto _LL23F;}_LL23E:{const char*_tmp935;void*_tmp934;(_tmp934=
0,Cyc_Tcutil_terr(loc,((_tmp935="cannot assign to an array",_tag_dyneither(
_tmp935,sizeof(char),26))),_tag_dyneither(_tmp934,sizeof(void*),0)));}goto _LL23C;
_LL23F:;_LL240: goto _LL23C;_LL23C:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp938;void*_tmp937;(_tmp937=0,Cyc_Tcutil_terr(loc,((
_tmp938="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp938,sizeof(char),58))),_tag_dyneither(_tmp937,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp93B;void*_tmp93A;return(_tmp93A=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp93B="swap non-lvalue",_tag_dyneither(_tmp93B,sizeof(char),
16))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp93E;void*_tmp93D;return(_tmp93D=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp93E="swap non-lvalue",_tag_dyneither(_tmp93E,sizeof(char),16))),
_tag_dyneither(_tmp93D,sizeof(void*),0)));}{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp941;void*_tmp940;return(_tmp940=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp941="swap value in zeroterm array",_tag_dyneither(_tmp941,
sizeof(char),29))),_tag_dyneither(_tmp940,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp944;void*_tmp943;return(_tmp943=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp944="swap value in zeroterm array",_tag_dyneither(_tmp944,
sizeof(char),29))),_tag_dyneither(_tmp943,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp949;void*_tmp948[2];struct Cyc_String_pa_struct _tmp947;struct Cyc_String_pa_struct
_tmp946;void*_tmp4E0=(_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp947.tag=0,((_tmp947.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp948[0]=&
_tmp947,((_tmp948[1]=& _tmp946,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp949="type mismatch: %s != %s",
_tag_dyneither(_tmp949,sizeof(char),24))),_tag_dyneither(_tmp948,sizeof(void*),2)))))))))))));
return _tmp4E0;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=
0;static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4E5=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4E5,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4E6=s->r;struct Cyc_Absyn_Exp*
_tmp4E8;struct Cyc_Absyn_Stmt*_tmp4EA;struct Cyc_Absyn_Stmt*_tmp4EB;struct Cyc_Absyn_Decl*
_tmp4ED;struct Cyc_Absyn_Stmt*_tmp4EE;_LL242: {struct Cyc_Absyn_Exp_s_struct*
_tmp4E7=(struct Cyc_Absyn_Exp_s_struct*)_tmp4E6;if(_tmp4E7->tag != 1)goto _LL244;
else{_tmp4E8=_tmp4E7->f1;}}_LL243: {void*_tmp4EF=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E8->topt))->v;if(!Cyc_Tcutil_unify(_tmp4EF,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){{const char*_tmp94D;void*_tmp94C[1];struct
Cyc_String_pa_struct _tmp94B;(_tmp94B.tag=0,((_tmp94B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4EF)),((_tmp94C[0]=& _tmp94B,Cyc_Tcutil_terr(
loc,((_tmp94D="statement expression returns type %s",_tag_dyneither(_tmp94D,
sizeof(char),37))),_tag_dyneither(_tmp94C,sizeof(void*),1)))))));}Cyc_Tcutil_explain_failure();}
return _tmp4EF;}_LL244: {struct Cyc_Absyn_Seq_s_struct*_tmp4E9=(struct Cyc_Absyn_Seq_s_struct*)
_tmp4E6;if(_tmp4E9->tag != 2)goto _LL246;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;}}
_LL245: s=_tmp4EB;continue;_LL246: {struct Cyc_Absyn_Decl_s_struct*_tmp4EC=(struct
Cyc_Absyn_Decl_s_struct*)_tmp4E6;if(_tmp4EC->tag != 12)goto _LL248;else{_tmp4ED=
_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;}}_LL247: s=_tmp4EE;continue;_LL248:;_LL249: {
const char*_tmp950;void*_tmp94F;return(_tmp94F=0,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp950="statement expression must end with expression",_tag_dyneither(_tmp950,
sizeof(char),46))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}_LL241:;}}static
void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExp(te,0,e));{void*_tmp4F5=t;struct Cyc_Absyn_AggrInfo _tmp4F7;union
Cyc_Absyn_AggrInfoU _tmp4F8;struct Cyc_Absyn_Aggrdecl**_tmp4F9;struct Cyc_Absyn_Aggrdecl*
_tmp4FA;_LL24B: {struct Cyc_Absyn_AggrType_struct*_tmp4F6=(struct Cyc_Absyn_AggrType_struct*)
_tmp4F5;if(_tmp4F6->tag != 12)goto _LL24D;else{_tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F7.aggr_info;
if((_tmp4F8.KnownAggr).tag != 2)goto _LL24D;_tmp4F9=(struct Cyc_Absyn_Aggrdecl**)(
_tmp4F8.KnownAggr).val;_tmp4FA=*_tmp4F9;}}_LL24C: if((_tmp4FA->kind == Cyc_Absyn_UnionA
 && _tmp4FA->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FA->impl))->tagged)
goto _LL24A;goto _LL24E;_LL24D:;_LL24E:{const char*_tmp954;void*_tmp953[1];struct
Cyc_String_pa_struct _tmp952;(_tmp952.tag=0,((_tmp952.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp953[0]=& _tmp952,Cyc_Tcutil_terr(
loc,((_tmp954="expecting @tagged union but found %s",_tag_dyneither(_tmp954,
sizeof(char),37))),_tag_dyneither(_tmp953,sizeof(void*),1)))))));}goto _LL24A;
_LL24A:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct _tmp957;struct Cyc_Absyn_RgnHandleType_struct*
_tmp956;void*expected_type=(void*)((_tmp956=_cycalloc(sizeof(*_tmp956)),((
_tmp956[0]=((_tmp957.tag=16,((_tmp957.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp957)))),_tmp956))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle));void*_tmp4FE=Cyc_Tcutil_compress(handle_type);void*
_tmp500;_LL250: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4FF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4FE;if(_tmp4FF->tag != 16)goto _LL252;else{_tmp500=(void*)_tmp4FF->f1;}}_LL251:
rgn=_tmp500;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL24F;_LL252:;_LL253:{
const char*_tmp95B;void*_tmp95A[1];struct Cyc_String_pa_struct _tmp959;(_tmp959.tag=
0,((_tmp959.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type)),((_tmp95A[0]=& _tmp959,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_check_null(*rgn_handle))->loc,((_tmp95B="expecting region_t type but found %s",
_tag_dyneither(_tmp95B,sizeof(char),37))),_tag_dyneither(_tmp95A,sizeof(void*),1)))))));}
goto _LL24F;_LL24F:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){rgn=Cyc_Tcexp_mallocRgn(optrgn);if(
rgn != (void*)& Cyc_Absyn_HeapRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
void*_tmp506=e1->r;struct Cyc_Core_Opt*_tmp509;struct Cyc_List_List*_tmp50A;struct
Cyc_List_List*_tmp50C;union Cyc_Absyn_Cnst _tmp50E;struct _dyneither_ptr _tmp50F;
_LL255: {struct Cyc_Absyn_Comprehension_e_struct*_tmp507=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp506;if(_tmp507->tag != 28)goto _LL257;}_LL256: {void*_tmp510=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp511=Cyc_Tcutil_compress(_tmp510);struct Cyc_Absyn_ArrayInfo
_tmp513;void*_tmp514;struct Cyc_Absyn_Tqual _tmp515;struct Cyc_Absyn_Exp*_tmp516;
union Cyc_Absyn_Constraint*_tmp517;_LL260: {struct Cyc_Absyn_ArrayType_struct*
_tmp512=(struct Cyc_Absyn_ArrayType_struct*)_tmp511;if(_tmp512->tag != 9)goto
_LL262;else{_tmp513=_tmp512->f1;_tmp514=_tmp513.elt_type;_tmp515=_tmp513.tq;
_tmp516=_tmp513.num_elts;_tmp517=_tmp513.zero_term;}}_LL261: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp516);void*b;{void*_tmp518=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp51A;_LL265: {
struct Cyc_Absyn_TagType_struct*_tmp519=(struct Cyc_Absyn_TagType_struct*)_tmp518;
if(_tmp519->tag != 20)goto _LL267;else{_tmp51A=(void*)_tmp519->f1;}}_LL266:{struct
Cyc_Absyn_Upper_b_struct _tmp95E;struct Cyc_Absyn_Upper_b_struct*_tmp95D;b=(void*)((
_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D[0]=((_tmp95E.tag=1,((_tmp95E.f1=
Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp51A,0),0,Cyc_Absyn_No_coercion,
0),_tmp95E)))),_tmp95D))));}goto _LL264;_LL267:;_LL268: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp961;struct Cyc_Absyn_Upper_b_struct*
_tmp960;b=(void*)((_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960[0]=((_tmp961.tag=
1,((_tmp961.f1=bnd,_tmp961)))),_tmp960))));}else{b=(void*)& Cyc_Absyn_DynEither_b_val;}
_LL264:;}{struct Cyc_Absyn_PointerType_struct _tmp96B;struct Cyc_Absyn_PtrAtts
_tmp96A;struct Cyc_Absyn_PtrInfo _tmp969;struct Cyc_Absyn_PointerType_struct*
_tmp968;void*res_typ=(void*)((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968[0]=((
_tmp96B.tag=5,((_tmp96B.f1=((_tmp969.elt_typ=_tmp514,((_tmp969.elt_tq=_tmp515,((
_tmp969.ptr_atts=((_tmp96A.rgn=rgn,((_tmp96A.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp96A.bounds=((union Cyc_Absyn_Constraint*(*)(void*
x))Cyc_Absyn_new_conref)(b),((_tmp96A.zero_term=_tmp517,((_tmp96A.ptrloc=0,
_tmp96A)))))))))),_tmp969)))))),_tmp96B)))),_tmp968))));if(topt != 0){if(!Cyc_Tcutil_unify(*
topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*
_tmp96C;e->topt=((_tmp96C=_cycalloc(sizeof(*_tmp96C)),((_tmp96C->v=res_typ,
_tmp96C))));}Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL262:;_LL263: {const char*_tmp96F;void*_tmp96E;(
_tmp96E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp96F="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp96F,
sizeof(char),45))),_tag_dyneither(_tmp96E,sizeof(void*),0)));}_LL25F:;}_LL257: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp508=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp506;if(_tmp508->tag != 36)goto _LL259;else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;}}
_LL258:{struct Cyc_Absyn_Array_e_struct _tmp972;struct Cyc_Absyn_Array_e_struct*
_tmp971;e1->r=(void*)((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((
_tmp972.tag=27,((_tmp972.f1=_tmp50A,_tmp972)))),_tmp971))));}_tmp50C=_tmp50A;
goto _LL25A;_LL259: {struct Cyc_Absyn_Array_e_struct*_tmp50B=(struct Cyc_Absyn_Array_e_struct*)
_tmp506;if(_tmp50B->tag != 27)goto _LL25B;else{_tmp50C=_tmp50B->f1;}}_LL25A: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp528=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp52A;void*_tmp52B;void**_tmp52C;struct Cyc_Absyn_Tqual
_tmp52D;struct Cyc_Absyn_PtrAtts _tmp52E;union Cyc_Absyn_Constraint*_tmp52F;_LL26A: {
struct Cyc_Absyn_PointerType_struct*_tmp529=(struct Cyc_Absyn_PointerType_struct*)
_tmp528;if(_tmp529->tag != 5)goto _LL26C;else{_tmp52A=_tmp529->f1;_tmp52B=_tmp52A.elt_typ;
_tmp52C=(void**)&(_tmp529->f1).elt_typ;_tmp52D=_tmp52A.elt_tq;_tmp52E=_tmp52A.ptr_atts;
_tmp52F=_tmp52E.zero_term;}}_LL26B: elt_typ_opt=(void**)_tmp52C;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52F);goto _LL269;
_LL26C:;_LL26D: goto _LL269;_LL269:;}{void*_tmp530=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp50C);{struct Cyc_Core_Opt*_tmp973;e1->topt=((_tmp973=
_cycalloc(sizeof(*_tmp973)),((_tmp973->v=_tmp530,_tmp973))));}{void*res_typ;{
void*_tmp532=Cyc_Tcutil_compress(_tmp530);struct Cyc_Absyn_ArrayInfo _tmp534;void*
_tmp535;struct Cyc_Absyn_Tqual _tmp536;struct Cyc_Absyn_Exp*_tmp537;union Cyc_Absyn_Constraint*
_tmp538;_LL26F: {struct Cyc_Absyn_ArrayType_struct*_tmp533=(struct Cyc_Absyn_ArrayType_struct*)
_tmp532;if(_tmp533->tag != 9)goto _LL271;else{_tmp534=_tmp533->f1;_tmp535=_tmp534.elt_type;
_tmp536=_tmp534.tq;_tmp537=_tmp534.num_elts;_tmp538=_tmp534.zero_term;}}_LL270:{
struct Cyc_Absyn_PointerType_struct _tmp988;struct Cyc_Absyn_PtrAtts _tmp987;struct
Cyc_Absyn_Upper_b_struct _tmp986;struct Cyc_Absyn_Upper_b_struct*_tmp985;struct Cyc_Absyn_PtrInfo
_tmp984;struct Cyc_Absyn_PointerType_struct*_tmp983;res_typ=(void*)((_tmp983=
_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp988.tag=5,((_tmp988.f1=((_tmp984.elt_typ=
_tmp535,((_tmp984.elt_tq=_tmp536,((_tmp984.ptr_atts=((_tmp987.rgn=rgn,((_tmp987.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp987.bounds=((
union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp985=
_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp986.tag=1,((_tmp986.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp537),_tmp986)))),_tmp985))))),((_tmp987.zero_term=_tmp538,((
_tmp987.ptrloc=0,_tmp987)))))))))),_tmp984)))))),_tmp988)))),_tmp983))));}if(
topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,
loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp989;e->topt=((_tmp989=_cycalloc(
sizeof(*_tmp989)),((_tmp989->v=res_typ,_tmp989))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}goto _LL26E;_LL271:;_LL272: {
const char*_tmp98C;void*_tmp98B;(_tmp98B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp98C="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp98C,sizeof(char),50))),_tag_dyneither(_tmp98B,sizeof(void*),0)));}
_LL26E:;}return res_typ;};};}_LL25B: {struct Cyc_Absyn_Const_e_struct*_tmp50D=(
struct Cyc_Absyn_Const_e_struct*)_tmp506;if(_tmp50D->tag != 0)goto _LL25D;else{
_tmp50E=_tmp50D->f1;if((_tmp50E.String_c).tag != 7)goto _LL25D;_tmp50F=(struct
_dyneither_ptr)(_tmp50E.String_c).val;}}_LL25C: {void*_tmp542=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp543=Cyc_Tcexp_tcExp(te,(void**)& _tmp542,e1);
struct Cyc_Absyn_Upper_b_struct _tmp98F;struct Cyc_Absyn_Upper_b_struct*_tmp98E;
return Cyc_Absyn_atb_typ(_tmp543,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp98E=
_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=1,((_tmp98F.f1=Cyc_Absyn_uint_exp(
1,0),_tmp98F)))),_tmp98E)))),Cyc_Absyn_false_conref);}_LL25D:;_LL25E: {void**
topt2=0;if(topt != 0){void*_tmp546=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp548;void*_tmp549;void**_tmp54A;struct Cyc_Absyn_Tqual _tmp54B;_LL274: {struct
Cyc_Absyn_PointerType_struct*_tmp547=(struct Cyc_Absyn_PointerType_struct*)
_tmp546;if(_tmp547->tag != 5)goto _LL276;else{_tmp548=_tmp547->f1;_tmp549=_tmp548.elt_typ;
_tmp54A=(void**)&(_tmp547->f1).elt_typ;_tmp54B=_tmp548.elt_tq;}}_LL275: topt2=(
void**)_tmp54A;goto _LL273;_LL276: {struct Cyc_Absyn_DatatypeType_struct*_tmp54C=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp546;if(_tmp54C->tag != 3)goto _LL278;}
_LL277:{void**_tmp990;topt2=((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990[0]=*
topt,_tmp990))));}goto _LL273;_LL278:;_LL279: goto _LL273;_LL273:;}{void*telt=Cyc_Tcexp_tcExp(
te,topt2,e1);struct Cyc_Absyn_PointerType_struct _tmp99A;struct Cyc_Absyn_PtrAtts
_tmp999;struct Cyc_Absyn_PtrInfo _tmp998;struct Cyc_Absyn_PointerType_struct*
_tmp997;void*res_typ=(void*)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=((
_tmp99A.tag=5,((_tmp99A.f1=((_tmp998.elt_typ=telt,((_tmp998.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp998.ptr_atts=((_tmp999.rgn=rgn,((_tmp999.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp999.bounds=Cyc_Absyn_bounds_one_conref,((_tmp999.zero_term=
Cyc_Absyn_false_conref,((_tmp999.ptrloc=0,_tmp999)))))))))),_tmp998)))))),
_tmp99A)))),_tmp997))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp99B;e->topt=((_tmp99B=_cycalloc(
sizeof(*_tmp99B)),((_tmp99B->v=res_typ,_tmp99B))));}Cyc_Tcutil_unchecked_cast(te,
e,*topt,Cyc_Absyn_Other_coercion);res_typ=*topt;}}return res_typ;};}_LL254:;};}
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp553=t;
struct Cyc_Absyn_ArrayInfo _tmp555;void*_tmp556;struct Cyc_Absyn_Tqual _tmp557;
struct Cyc_Absyn_Exp*_tmp558;union Cyc_Absyn_Constraint*_tmp559;_LL27B: {struct Cyc_Absyn_ArrayType_struct*
_tmp554=(struct Cyc_Absyn_ArrayType_struct*)_tmp553;if(_tmp554->tag != 9)goto
_LL27D;else{_tmp555=_tmp554->f1;_tmp556=_tmp555.elt_type;_tmp557=_tmp555.tq;
_tmp558=_tmp555.num_elts;_tmp559=_tmp555.zero_term;}}_LL27C: {void*_tmp55B;
struct _tuple13 _tmp55A=Cyc_Tcutil_addressof_props(te,e);_tmp55B=_tmp55A.f2;{
struct Cyc_Absyn_Upper_b_struct _tmp99E;struct Cyc_Absyn_Upper_b_struct*_tmp99D;
void*_tmp55C=_tmp558 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp99D=
_cycalloc(sizeof(*_tmp99D)),((_tmp99D[0]=((_tmp99E.tag=1,((_tmp99E.f1=(struct Cyc_Absyn_Exp*)
_tmp558,_tmp99E)))),_tmp99D))));t=Cyc_Absyn_atb_typ(_tmp556,_tmp55B,_tmp557,
_tmp55C,_tmp559);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL27D:;_LL27E: return t;_LL27A:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp55F=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp55F,t) && !Cyc_Tcutil_is_noalias_path(_tmp55F,e)){const char*_tmp9A1;void*
_tmp9A0;(_tmp9A0=0,Cyc_Tcutil_terr(e->loc,((_tmp9A1="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp9A1,sizeof(char),49))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}{
void*_tmp562=e->r;union Cyc_Absyn_Cnst _tmp566;struct _dyneither_ptr _tmp567;_LL280: {
struct Cyc_Absyn_Array_e_struct*_tmp563=(struct Cyc_Absyn_Array_e_struct*)_tmp562;
if(_tmp563->tag != 27)goto _LL282;}_LL281: goto _LL283;_LL282: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp564=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp562;if(_tmp564->tag != 28)
goto _LL284;}_LL283: goto _LL285;_LL284: {struct Cyc_Absyn_Const_e_struct*_tmp565=(
struct Cyc_Absyn_Const_e_struct*)_tmp562;if(_tmp565->tag != 0)goto _LL286;else{
_tmp566=_tmp565->f1;if((_tmp566.String_c).tag != 7)goto _LL286;_tmp567=(struct
_dyneither_ptr)(_tmp566.String_c).val;}}_LL285: return t;_LL286:;_LL287: t=Cyc_Tcutil_compress(
t);{void*_tmp568=t;struct Cyc_Absyn_ArrayInfo _tmp56A;void*_tmp56B;struct Cyc_Absyn_Tqual
_tmp56C;struct Cyc_Absyn_Exp*_tmp56D;union Cyc_Absyn_Constraint*_tmp56E;_LL289: {
struct Cyc_Absyn_ArrayType_struct*_tmp569=(struct Cyc_Absyn_ArrayType_struct*)
_tmp568;if(_tmp569->tag != 9)goto _LL28B;else{_tmp56A=_tmp569->f1;_tmp56B=_tmp56A.elt_type;
_tmp56C=_tmp56A.tq;_tmp56D=_tmp56A.num_elts;_tmp56E=_tmp56A.zero_term;}}_LL28A: {
void*_tmp570;struct _tuple13 _tmp56F=Cyc_Tcutil_addressof_props(te,e);_tmp570=
_tmp56F.f2;{struct Cyc_Absyn_Upper_b_struct _tmp9A4;struct Cyc_Absyn_Upper_b_struct*
_tmp9A3;void*b=_tmp56D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9A3=
_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3[0]=((_tmp9A4.tag=1,((_tmp9A4.f1=(struct Cyc_Absyn_Exp*)
_tmp56D,_tmp9A4)))),_tmp9A3))));t=Cyc_Absyn_atb_typ(_tmp56B,_tmp570,_tmp56C,b,
_tmp56E);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL28B:;_LL28C: return t;_LL288:;};_LL27F:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp573=e->r;
struct Cyc_Absyn_Exp*_tmp575;_LL28E: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp574=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp573;if(_tmp574->tag != 13)
goto _LL290;else{_tmp575=_tmp574->f1;}}_LL28F: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp575);((struct Cyc_Core_Opt*)_check_null(_tmp575->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp575->topt))->v,0);e->topt=_tmp575->topt;
goto _LL28D;_LL290:;_LL291: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp576=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp578;void*_tmp579;struct Cyc_Absyn_Tqual
_tmp57A;struct Cyc_Absyn_PtrAtts _tmp57B;void*_tmp57C;union Cyc_Absyn_Constraint*
_tmp57D;union Cyc_Absyn_Constraint*_tmp57E;union Cyc_Absyn_Constraint*_tmp57F;
_LL293: {struct Cyc_Absyn_PointerType_struct*_tmp577=(struct Cyc_Absyn_PointerType_struct*)
_tmp576;if(_tmp577->tag != 5)goto _LL295;else{_tmp578=_tmp577->f1;_tmp579=_tmp578.elt_typ;
_tmp57A=_tmp578.elt_tq;_tmp57B=_tmp578.ptr_atts;_tmp57C=_tmp57B.rgn;_tmp57D=
_tmp57B.nullable;_tmp57E=_tmp57B.bounds;_tmp57F=_tmp57B.zero_term;}}_LL294:{void*
_tmp580=Cyc_Tcutil_compress(_tmp579);struct Cyc_Absyn_FnInfo _tmp582;struct Cyc_List_List*
_tmp583;struct Cyc_Core_Opt*_tmp584;void*_tmp585;struct Cyc_List_List*_tmp586;int
_tmp587;struct Cyc_Absyn_VarargInfo*_tmp588;struct Cyc_List_List*_tmp589;struct Cyc_List_List*
_tmp58A;_LL298: {struct Cyc_Absyn_FnType_struct*_tmp581=(struct Cyc_Absyn_FnType_struct*)
_tmp580;if(_tmp581->tag != 10)goto _LL29A;else{_tmp582=_tmp581->f1;_tmp583=_tmp582.tvars;
_tmp584=_tmp582.effect;_tmp585=_tmp582.ret_typ;_tmp586=_tmp582.args;_tmp587=
_tmp582.c_varargs;_tmp588=_tmp582.cyc_varargs;_tmp589=_tmp582.rgn_po;_tmp58A=
_tmp582.attributes;}}_LL299: if(_tmp583 != 0){struct _RegionHandle*_tmp58B=Cyc_Tcenv_get_fnrgn(
te);struct _tuple11 _tmp9A5;struct _tuple11 _tmp58C=(_tmp9A5.f1=Cyc_Tcenv_lookup_type_vars(
te),((_tmp9A5.f2=_tmp58B,_tmp9A5)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),
struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp58B,Cyc_Tcutil_r_make_inst_var,&
_tmp58C,_tmp583);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp9AB;struct Cyc_Absyn_FnInfo
_tmp9AA;struct Cyc_Absyn_FnType_struct*_tmp9A9;void*ftyp=Cyc_Tcutil_rsubstitute(
_tmp58B,inst,(void*)((_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9[0]=((_tmp9AB.tag=
10,((_tmp9AB.f1=((_tmp9AA.tvars=0,((_tmp9AA.effect=_tmp584,((_tmp9AA.ret_typ=
_tmp585,((_tmp9AA.args=_tmp586,((_tmp9AA.c_varargs=_tmp587,((_tmp9AA.cyc_varargs=
_tmp588,((_tmp9AA.rgn_po=_tmp589,((_tmp9AA.attributes=_tmp58A,_tmp9AA)))))))))))))))),
_tmp9AB)))),_tmp9A9)))));struct Cyc_Absyn_PointerType_struct _tmp9B5;struct Cyc_Absyn_PtrAtts
_tmp9B4;struct Cyc_Absyn_PtrInfo _tmp9B3;struct Cyc_Absyn_PointerType_struct*
_tmp9B2;struct Cyc_Absyn_PointerType_struct*_tmp58D=(_tmp9B2=_cycalloc(sizeof(*
_tmp9B2)),((_tmp9B2[0]=((_tmp9B5.tag=5,((_tmp9B5.f1=((_tmp9B3.elt_typ=ftyp,((
_tmp9B3.elt_tq=_tmp57A,((_tmp9B3.ptr_atts=((_tmp9B4.rgn=_tmp57C,((_tmp9B4.nullable=
_tmp57D,((_tmp9B4.bounds=_tmp57E,((_tmp9B4.zero_term=_tmp57F,((_tmp9B4.ptrloc=0,
_tmp9B4)))))))))),_tmp9B3)))))),_tmp9B5)))),_tmp9B2)));struct Cyc_Absyn_Exp*
_tmp58E=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct _tmp9B8;
struct Cyc_Absyn_Instantiate_e_struct*_tmp9B7;e->r=(void*)((_tmp9B7=_cycalloc(
sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B8.tag=14,((_tmp9B8.f1=_tmp58E,((_tmp9B8.f2=
ts,_tmp9B8)))))),_tmp9B7))));}{struct Cyc_Core_Opt*_tmp9B9;e->topt=((_tmp9B9=
_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->v=(void*)_tmp58D,_tmp9B9))));};}goto
_LL297;_LL29A:;_LL29B: goto _LL297;_LL297:;}goto _LL292;_LL295:;_LL296: goto _LL292;
_LL292:;}goto _LL28D;_LL28D:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){struct
_RegionHandle*_tmp59A=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp59B=0;{
void*_tmp59C=fn_exp->r;struct Cyc_List_List*_tmp59E;_LL29D: {struct Cyc_Absyn_FnCall_e_struct*
_tmp59D=(struct Cyc_Absyn_FnCall_e_struct*)_tmp59C;if(_tmp59D->tag != 11)goto
_LL29F;else{_tmp59E=_tmp59D->f2;}}_LL29E:{void*_tmp59F=e->r;struct Cyc_List_List*
_tmp5A1;_LL2A2: {struct Cyc_Absyn_FnCall_e_struct*_tmp5A0=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp59F;if(_tmp5A0->tag != 11)goto _LL2A4;else{_tmp5A1=_tmp5A0->f2;}}_LL2A3: {
struct Cyc_List_List*_tmp5A2=alias_arg_exps;int _tmp5A3=0;while(_tmp5A2 != 0){
while(_tmp5A3 != (int)_tmp5A2->hd){if(_tmp59E == 0){struct Cyc_Int_pa_struct _tmp9C3;
struct Cyc_Int_pa_struct _tmp9C2;void*_tmp9C1[2];const char*_tmp9C0;void*_tmp9BF;(
_tmp9BF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((
struct _dyneither_ptr)((_tmp9C3.tag=1,((_tmp9C3.f1=(unsigned long)((int)_tmp5A2->hd),((
_tmp9C2.tag=1,((_tmp9C2.f1=(unsigned long)_tmp5A3,((_tmp9C1[0]=& _tmp9C2,((
_tmp9C1[1]=& _tmp9C3,Cyc_aprintf(((_tmp9C0="bad count %d/%d for alias coercion!",
_tag_dyneither(_tmp9C0,sizeof(char),36))),_tag_dyneither(_tmp9C1,sizeof(void*),2)))))))))))))),
_tag_dyneither(_tmp9BF,sizeof(void*),0)));}++ _tmp5A3;_tmp59E=_tmp59E->tl;_tmp5A1=((
struct Cyc_List_List*)_check_null(_tmp5A1))->tl;}{struct Cyc_Absyn_Decl*_tmp5AA;
struct Cyc_Absyn_Exp*_tmp5AB;struct _tuple10 _tmp5A9=Cyc_Tcutil_insert_alias((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp59E))->hd,Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp5A1))->hd)->topt))->v));_tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;
_tmp59E->hd=(void*)_tmp5AB;{struct Cyc_List_List*_tmp9C4;_tmp59B=((_tmp9C4=
_region_malloc(_tmp59A,sizeof(*_tmp9C4)),((_tmp9C4->hd=_tmp5AA,((_tmp9C4->tl=
_tmp59B,_tmp9C4))))));}_tmp5A2=_tmp5A2->tl;};}goto _LL2A1;}_LL2A4:;_LL2A5: {const
char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C7="not a function call!",
_tag_dyneither(_tmp9C7,sizeof(char),21))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
_LL2A1:;}goto _LL29C;_LL29F:;_LL2A0: {const char*_tmp9CA;void*_tmp9C9;(_tmp9C9=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp9CA="not a function call!",_tag_dyneither(_tmp9CA,sizeof(char),21))),
_tag_dyneither(_tmp9C9,sizeof(void*),0)));}_LL29C:;}while(_tmp59B != 0){struct Cyc_Absyn_Decl*
_tmp5B1=(struct Cyc_Absyn_Decl*)_tmp59B->hd;fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(
_tmp5B1,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);_tmp59B=_tmp59B->tl;}e->topt=
0;e->r=fn_exp->r;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*
_tmp5B2=e->r;struct Cyc_Absyn_Exp*_tmp5B4;struct _tuple1*_tmp5B6;struct Cyc_Absyn_Exp*
_tmp5B8;struct Cyc_List_List*_tmp5B9;struct Cyc_Core_Opt*_tmp5BB;struct Cyc_List_List*
_tmp5BC;union Cyc_Absyn_Cnst _tmp5BE;union Cyc_Absyn_Cnst*_tmp5BF;struct _tuple1*
_tmp5C1;void*_tmp5C2;enum Cyc_Absyn_Primop _tmp5C4;struct Cyc_List_List*_tmp5C5;
struct Cyc_Absyn_Exp*_tmp5C7;enum Cyc_Absyn_Incrementor _tmp5C8;struct Cyc_Absyn_Exp*
_tmp5CA;struct Cyc_Core_Opt*_tmp5CB;struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Exp*
_tmp5CE;struct Cyc_Absyn_Exp*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*
_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Exp*
_tmp5D6;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*
_tmp5DB;struct Cyc_List_List*_tmp5DC;struct Cyc_Absyn_VarargCallInfo*_tmp5DD;
struct Cyc_Absyn_VarargCallInfo**_tmp5DE;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Exp*
_tmp5E2;struct Cyc_List_List*_tmp5E3;void*_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E6;
enum Cyc_Absyn_Coercion _tmp5E7;enum Cyc_Absyn_Coercion*_tmp5E8;struct Cyc_Absyn_Exp*
_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Exp**_tmp5ED;struct Cyc_Absyn_Exp*
_tmp5EE;struct Cyc_Absyn_Exp*_tmp5F0;void*_tmp5F2;void*_tmp5F4;void*_tmp5F5;
struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F9;struct _dyneither_ptr*
_tmp5FA;int _tmp5FB;int*_tmp5FC;int _tmp5FD;int*_tmp5FE;struct Cyc_Absyn_Exp*
_tmp600;struct _dyneither_ptr*_tmp601;int _tmp602;int*_tmp603;int _tmp604;int*
_tmp605;struct Cyc_Absyn_Exp*_tmp607;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_List_List*
_tmp60A;struct _tuple8*_tmp60C;struct Cyc_List_List*_tmp60D;struct Cyc_List_List*
_tmp60F;struct Cyc_Absyn_Stmt*_tmp611;struct Cyc_Absyn_Vardecl*_tmp613;struct Cyc_Absyn_Exp*
_tmp614;struct Cyc_Absyn_Exp*_tmp615;int _tmp616;int*_tmp617;struct _tuple1*_tmp619;
struct _tuple1**_tmp61A;struct Cyc_List_List*_tmp61B;struct Cyc_List_List**_tmp61C;
struct Cyc_List_List*_tmp61D;struct Cyc_Absyn_Aggrdecl*_tmp61E;struct Cyc_Absyn_Aggrdecl**
_tmp61F;void*_tmp621;struct Cyc_List_List*_tmp622;struct Cyc_List_List*_tmp624;
struct Cyc_Absyn_Datatypedecl*_tmp625;struct Cyc_Absyn_Datatypefield*_tmp626;
struct _tuple1*_tmp628;struct _tuple1**_tmp629;struct Cyc_Absyn_Enumdecl*_tmp62A;
struct Cyc_Absyn_Enumfield*_tmp62B;struct _tuple1*_tmp62D;struct _tuple1**_tmp62E;
void*_tmp62F;struct Cyc_Absyn_Enumfield*_tmp630;struct Cyc_Absyn_MallocInfo _tmp632;
int _tmp633;int*_tmp634;struct Cyc_Absyn_Exp*_tmp635;struct Cyc_Absyn_Exp**_tmp636;
void**_tmp637;void***_tmp638;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Exp**
_tmp63A;int _tmp63B;int*_tmp63C;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*
_tmp63F;struct Cyc_Absyn_Exp*_tmp641;struct _dyneither_ptr*_tmp642;void*_tmp644;
_LL2A7: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp5B3=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp5B2;if(_tmp5B3->tag != 13)goto _LL2A9;else{_tmp5B4=_tmp5B3->f1;}}_LL2A8: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp5B4);return;_LL2A9: {struct Cyc_Absyn_UnknownId_e_struct*_tmp5B5=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp5B2;if(_tmp5B5->tag != 2)goto _LL2AB;else{
_tmp5B6=_tmp5B5->f1;}}_LL2AA: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp5B6);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL2AB: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp5B7=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp5B2;if(_tmp5B7->tag != 10)goto _LL2AD;else{
_tmp5B8=_tmp5B7->f1;_tmp5B9=_tmp5B7->f2;}}_LL2AC: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp5B8,_tmp5B9);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2AD: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp5BA=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp5B2;if(_tmp5BA->tag != 36)
goto _LL2AF;else{_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BA->f2;}}_LL2AE: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp5BC);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL2AF: {struct
Cyc_Absyn_Const_e_struct*_tmp5BD=(struct Cyc_Absyn_Const_e_struct*)_tmp5B2;if(
_tmp5BD->tag != 0)goto _LL2B1;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=(union Cyc_Absyn_Cnst*)&
_tmp5BD->f1;}}_LL2B0: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp5BF,e);goto _LL2A6;_LL2B1: {struct Cyc_Absyn_Var_e_struct*_tmp5C0=(struct Cyc_Absyn_Var_e_struct*)
_tmp5B2;if(_tmp5C0->tag != 1)goto _LL2B3;else{_tmp5C1=_tmp5C0->f1;_tmp5C2=(void*)
_tmp5C0->f2;}}_LL2B2: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp5C1,_tmp5C2);goto _LL2A6;
_LL2B3: {struct Cyc_Absyn_Primop_e_struct*_tmp5C3=(struct Cyc_Absyn_Primop_e_struct*)
_tmp5B2;if(_tmp5C3->tag != 3)goto _LL2B5;else{_tmp5C4=_tmp5C3->f1;_tmp5C5=_tmp5C3->f2;}}
_LL2B4: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5C4,_tmp5C5);goto _LL2A6;_LL2B5: {
struct Cyc_Absyn_Increment_e_struct*_tmp5C6=(struct Cyc_Absyn_Increment_e_struct*)
_tmp5B2;if(_tmp5C6->tag != 5)goto _LL2B7;else{_tmp5C7=_tmp5C6->f1;_tmp5C8=_tmp5C6->f2;}}
_LL2B6: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5C7,_tmp5C8);goto _LL2A6;_LL2B7: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp5C9=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp5B2;if(_tmp5C9->tag != 4)goto _LL2B9;else{_tmp5CA=_tmp5C9->f1;_tmp5CB=_tmp5C9->f2;
_tmp5CC=_tmp5C9->f3;}}_LL2B8: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5CA,_tmp5CB,
_tmp5CC);goto _LL2A6;_LL2B9: {struct Cyc_Absyn_Conditional_e_struct*_tmp5CD=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp5B2;if(_tmp5CD->tag != 6)goto _LL2BB;
else{_tmp5CE=_tmp5CD->f1;_tmp5CF=_tmp5CD->f2;_tmp5D0=_tmp5CD->f3;}}_LL2BA: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp5CE,_tmp5CF,_tmp5D0);goto _LL2A6;_LL2BB: {struct Cyc_Absyn_And_e_struct*
_tmp5D1=(struct Cyc_Absyn_And_e_struct*)_tmp5B2;if(_tmp5D1->tag != 7)goto _LL2BD;
else{_tmp5D2=_tmp5D1->f1;_tmp5D3=_tmp5D1->f2;}}_LL2BC: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp5D2,_tmp5D3);goto _LL2A6;_LL2BD: {struct Cyc_Absyn_Or_e_struct*_tmp5D4=(struct
Cyc_Absyn_Or_e_struct*)_tmp5B2;if(_tmp5D4->tag != 8)goto _LL2BF;else{_tmp5D5=
_tmp5D4->f1;_tmp5D6=_tmp5D4->f2;}}_LL2BE: t=Cyc_Tcexp_tcOr(te,loc,_tmp5D5,_tmp5D6);
goto _LL2A6;_LL2BF: {struct Cyc_Absyn_SeqExp_e_struct*_tmp5D7=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp5B2;if(_tmp5D7->tag != 9)goto _LL2C1;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D7->f2;}}
_LL2C0: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5D8,_tmp5D9);goto _LL2A6;_LL2C1: {
struct Cyc_Absyn_FnCall_e_struct*_tmp5DA=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp5B2;if(_tmp5DA->tag != 11)goto _LL2C3;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=_tmp5DA->f2;
_tmp5DD=_tmp5DA->f3;_tmp5DE=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5DA->f3;}}
_LL2C2: {struct Cyc_List_List*alias_arg_exps=0;int ok=1;struct Cyc_Absyn_Exp*fn_exp;{
struct _handler_cons _tmp646;_push_handler(& _tmp646);{int _tmp648=0;if(setjmp(
_tmp646.handler))_tmp648=1;if(!_tmp648){fn_exp=Cyc_Tcutil_deep_copy_exp(e);;
_pop_handler();}else{void*_tmp647=(void*)_exn_thrown;void*_tmp64A=_tmp647;struct
_dyneither_ptr _tmp64C;_LL2FA: {struct Cyc_Core_Failure_struct*_tmp64B=(struct Cyc_Core_Failure_struct*)
_tmp64A;if(_tmp64B->tag != Cyc_Core_Failure)goto _LL2FC;else{_tmp64C=_tmp64B->f1;}}
_LL2FB: ok=0;fn_exp=e;goto _LL2F9;_LL2FC:;_LL2FD:(void)_throw(_tmp64A);_LL2F9:;}};}
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5DB,_tmp5DC,_tmp5DE,& alias_arg_exps);if(
alias_arg_exps != 0  && ok){alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(alias_arg_exps);Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,
alias_arg_exps);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;}goto _LL2A6;}_LL2C3: {
struct Cyc_Absyn_Throw_e_struct*_tmp5DF=(struct Cyc_Absyn_Throw_e_struct*)_tmp5B2;
if(_tmp5DF->tag != 12)goto _LL2C5;else{_tmp5E0=_tmp5DF->f1;}}_LL2C4: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp5E0);goto _LL2A6;_LL2C5: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5E1=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp5B2;if(_tmp5E1->tag != 14)goto
_LL2C7;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;}}_LL2C6: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5E2,_tmp5E3);goto _LL2A6;_LL2C7: {struct Cyc_Absyn_Cast_e_struct*
_tmp5E4=(struct Cyc_Absyn_Cast_e_struct*)_tmp5B2;if(_tmp5E4->tag != 15)goto _LL2C9;
else{_tmp5E5=(void*)_tmp5E4->f1;_tmp5E6=_tmp5E4->f2;_tmp5E7=_tmp5E4->f4;_tmp5E8=(
enum Cyc_Absyn_Coercion*)& _tmp5E4->f4;}}_LL2C8: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5E5,_tmp5E6,(enum Cyc_Absyn_Coercion*)_tmp5E8);goto _LL2A6;_LL2C9: {struct Cyc_Absyn_Address_e_struct*
_tmp5E9=(struct Cyc_Absyn_Address_e_struct*)_tmp5B2;if(_tmp5E9->tag != 16)goto
_LL2CB;else{_tmp5EA=_tmp5E9->f1;}}_LL2CA: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp5EA);goto _LL2A6;_LL2CB: {struct Cyc_Absyn_New_e_struct*_tmp5EB=(struct Cyc_Absyn_New_e_struct*)
_tmp5B2;if(_tmp5EB->tag != 17)goto _LL2CD;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=(struct
Cyc_Absyn_Exp**)& _tmp5EB->f1;_tmp5EE=_tmp5EB->f2;}}_LL2CC: t=Cyc_Tcexp_tcNew(te,
loc,topt,_tmp5ED,e,_tmp5EE);goto _LL2A6;_LL2CD: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp5EF=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp5B2;if(_tmp5EF->tag != 19)goto
_LL2CF;else{_tmp5F0=_tmp5EF->f1;}}_LL2CE: {void*_tmp64D=Cyc_Tcexp_tcExpNoPromote(
te,0,_tmp5F0);t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp64D);goto _LL2A6;}_LL2CF: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5F1=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5B2;if(_tmp5F1->tag != 18)goto _LL2D1;else{_tmp5F2=(void*)_tmp5F1->f1;}}_LL2D0:
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5F2);goto _LL2A6;_LL2D1: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp5F3=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp5B2;if(_tmp5F3->tag != 20)goto
_LL2D3;else{_tmp5F4=(void*)_tmp5F3->f1;_tmp5F5=(void*)_tmp5F3->f2;}}_LL2D2: t=Cyc_Tcexp_tcOffsetof(
te,loc,topt,_tmp5F4,_tmp5F5);goto _LL2A6;_LL2D3: {struct Cyc_Absyn_Deref_e_struct*
_tmp5F6=(struct Cyc_Absyn_Deref_e_struct*)_tmp5B2;if(_tmp5F6->tag != 21)goto _LL2D5;
else{_tmp5F7=_tmp5F6->f1;}}_LL2D4: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp5F7);goto
_LL2A6;_LL2D5: {struct Cyc_Absyn_AggrMember_e_struct*_tmp5F8=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp5B2;if(_tmp5F8->tag != 22)goto _LL2D7;else{_tmp5F9=_tmp5F8->f1;_tmp5FA=_tmp5F8->f2;
_tmp5FB=_tmp5F8->f3;_tmp5FC=(int*)& _tmp5F8->f3;_tmp5FD=_tmp5F8->f4;_tmp5FE=(int*)&
_tmp5F8->f4;}}_LL2D6: t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp5F9,_tmp5FA,
_tmp5FC,_tmp5FE);goto _LL2A6;_LL2D7: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp5FF=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp5B2;if(_tmp5FF->tag != 23)goto _LL2D9;
else{_tmp600=_tmp5FF->f1;_tmp601=_tmp5FF->f2;_tmp602=_tmp5FF->f3;_tmp603=(int*)&
_tmp5FF->f3;_tmp604=_tmp5FF->f4;_tmp605=(int*)& _tmp5FF->f4;}}_LL2D8: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp600,_tmp601,_tmp603,_tmp605);goto _LL2A6;_LL2D9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp606=(struct Cyc_Absyn_Subscript_e_struct*)_tmp5B2;if(_tmp606->tag != 24)goto
_LL2DB;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;}}_LL2DA: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp607,_tmp608);goto _LL2A6;_LL2DB: {struct Cyc_Absyn_Tuple_e_struct*
_tmp609=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5B2;if(_tmp609->tag != 25)goto _LL2DD;
else{_tmp60A=_tmp609->f1;}}_LL2DC: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp60A);goto
_LL2A6;_LL2DD: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp60B=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp5B2;if(_tmp60B->tag != 26)goto _LL2DF;else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60B->f2;}}
_LL2DE: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp60C,_tmp60D);goto _LL2A6;_LL2DF: {
struct Cyc_Absyn_Array_e_struct*_tmp60E=(struct Cyc_Absyn_Array_e_struct*)_tmp5B2;
if(_tmp60E->tag != 27)goto _LL2E1;else{_tmp60F=_tmp60E->f1;}}_LL2E0: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp64E=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp650;void*_tmp651;void**_tmp652;union Cyc_Absyn_Constraint*
_tmp653;_LL2FF: {struct Cyc_Absyn_ArrayType_struct*_tmp64F=(struct Cyc_Absyn_ArrayType_struct*)
_tmp64E;if(_tmp64F->tag != 9)goto _LL301;else{_tmp650=_tmp64F->f1;_tmp651=_tmp650.elt_type;
_tmp652=(void**)&(_tmp64F->f1).elt_type;_tmp653=_tmp650.zero_term;}}_LL300:
elt_topt=(void**)_tmp652;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp653);goto _LL2FE;_LL301:;_LL302: goto _LL2FE;_LL2FE:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp60F);goto _LL2A6;}_LL2E1: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp610=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp5B2;
if(_tmp610->tag != 37)goto _LL2E3;else{_tmp611=_tmp610->f1;}}_LL2E2: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp611);goto _LL2A6;_LL2E3: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp612=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp5B2;if(_tmp612->tag != 28)
goto _LL2E5;else{_tmp613=_tmp612->f1;_tmp614=_tmp612->f2;_tmp615=_tmp612->f3;
_tmp616=_tmp612->f4;_tmp617=(int*)& _tmp612->f4;}}_LL2E4: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp613,_tmp614,_tmp615,_tmp617);goto _LL2A6;_LL2E5: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp618=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5B2;if(_tmp618->tag != 29)goto
_LL2E7;else{_tmp619=_tmp618->f1;_tmp61A=(struct _tuple1**)& _tmp618->f1;_tmp61B=
_tmp618->f2;_tmp61C=(struct Cyc_List_List**)& _tmp618->f2;_tmp61D=_tmp618->f3;
_tmp61E=_tmp618->f4;_tmp61F=(struct Cyc_Absyn_Aggrdecl**)& _tmp618->f4;}}_LL2E6: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp61A,_tmp61C,_tmp61D,_tmp61F);goto _LL2A6;
_LL2E7: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp620=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp5B2;if(_tmp620->tag != 30)goto _LL2E9;else{_tmp621=(void*)_tmp620->f1;_tmp622=
_tmp620->f2;}}_LL2E8: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp621,_tmp622);goto _LL2A6;
_LL2E9: {struct Cyc_Absyn_Datatype_e_struct*_tmp623=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp5B2;if(_tmp623->tag != 31)goto _LL2EB;else{_tmp624=_tmp623->f1;_tmp625=_tmp623->f2;
_tmp626=_tmp623->f3;}}_LL2EA: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp624,
_tmp625,_tmp626);goto _LL2A6;_LL2EB: {struct Cyc_Absyn_Enum_e_struct*_tmp627=(
struct Cyc_Absyn_Enum_e_struct*)_tmp5B2;if(_tmp627->tag != 32)goto _LL2ED;else{
_tmp628=_tmp627->f1;_tmp629=(struct _tuple1**)& _tmp627->f1;_tmp62A=_tmp627->f2;
_tmp62B=_tmp627->f3;}}_LL2EC:*_tmp629=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp62B))->name;{struct Cyc_Absyn_EnumType_struct _tmp9CD;struct Cyc_Absyn_EnumType_struct*
_tmp9CC;t=(void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=
14,((_tmp9CD.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp62A))->name,((
_tmp9CD.f2=_tmp62A,_tmp9CD)))))),_tmp9CC))));}goto _LL2A6;_LL2ED: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp62C=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp5B2;if(_tmp62C->tag != 33)goto
_LL2EF;else{_tmp62D=_tmp62C->f1;_tmp62E=(struct _tuple1**)& _tmp62C->f1;_tmp62F=(
void*)_tmp62C->f2;_tmp630=_tmp62C->f3;}}_LL2EE:*_tmp62E=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp630))->name;t=_tmp62F;goto _LL2A6;_LL2EF: {struct Cyc_Absyn_Malloc_e_struct*
_tmp631=(struct Cyc_Absyn_Malloc_e_struct*)_tmp5B2;if(_tmp631->tag != 34)goto
_LL2F1;else{_tmp632=_tmp631->f1;_tmp633=_tmp632.is_calloc;_tmp634=(int*)&(
_tmp631->f1).is_calloc;_tmp635=_tmp632.rgn;_tmp636=(struct Cyc_Absyn_Exp**)&(
_tmp631->f1).rgn;_tmp637=_tmp632.elt_type;_tmp638=(void***)&(_tmp631->f1).elt_type;
_tmp639=_tmp632.num_elts;_tmp63A=(struct Cyc_Absyn_Exp**)&(_tmp631->f1).num_elts;
_tmp63B=_tmp632.fat_result;_tmp63C=(int*)&(_tmp631->f1).fat_result;}}_LL2F0: t=
Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp636,_tmp638,_tmp63A,_tmp634,_tmp63C);goto
_LL2A6;_LL2F1: {struct Cyc_Absyn_Swap_e_struct*_tmp63D=(struct Cyc_Absyn_Swap_e_struct*)
_tmp5B2;if(_tmp63D->tag != 35)goto _LL2F3;else{_tmp63E=_tmp63D->f1;_tmp63F=_tmp63D->f2;}}
_LL2F2: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp63E,_tmp63F);goto _LL2A6;_LL2F3: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp640=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp5B2;if(_tmp640->tag != 38)goto _LL2F5;else{_tmp641=_tmp640->f1;_tmp642=_tmp640->f2;}}
_LL2F4: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp641,_tmp642);goto _LL2A6;_LL2F5: {
struct Cyc_Absyn_Valueof_e_struct*_tmp643=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp5B2;if(_tmp643->tag != 39)goto _LL2F7;else{_tmp644=(void*)_tmp643->f1;}}_LL2F6:
if(!te->allow_valueof){const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,Cyc_Tcutil_terr(
e->loc,((_tmp9D0="valueof(-) can only occur within types",_tag_dyneither(_tmp9D0,
sizeof(char),39))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}t=Cyc_Absyn_sint_typ;
goto _LL2A6;_LL2F7: {struct Cyc_Absyn_Asm_e_struct*_tmp645=(struct Cyc_Absyn_Asm_e_struct*)
_tmp5B2;if(_tmp645->tag != 40)goto _LL2A6;}_LL2F8:{const char*_tmp9D3;void*_tmp9D2;(
_tmp9D2=0,Cyc_Tcutil_terr(e->loc,((_tmp9D3="asm expressions cannot occur within Cyclone code.",
_tag_dyneither(_tmp9D3,sizeof(char),50))),_tag_dyneither(_tmp9D2,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2A6:;}{struct Cyc_Core_Opt*
_tmp9D4;e->topt=((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4->v=t,_tmp9D4))));};}
