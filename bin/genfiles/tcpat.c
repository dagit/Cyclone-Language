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
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[
8];struct Cyc_Core_Failure_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[
11];struct Cyc_Core_Impossible_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{char*tag;};extern char
Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;extern char
Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char
Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};inline
static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd
>> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr
 || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*
r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(struct Cyc_Set_Set*s);int Cyc_Set_is_empty(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct
_dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
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
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
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
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct
Cyc_Absyn_Kind*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*
k1,struct Cyc_Absyn_Kind*k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(
void*);extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);struct _tuple10{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);struct _tuple12{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple12*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Pat*p){void*_tmp0=p->r;struct _tuple1*_tmp2;struct _tuple1*_tmp4;struct
Cyc_List_List*_tmp5;int _tmp6;struct Cyc_Absyn_AggrInfo*_tmp8;struct Cyc_List_List*
_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;struct Cyc_Absyn_AggrInfo*_tmpD;struct
Cyc_Absyn_AggrInfo _tmpE;union Cyc_Absyn_AggrInfoU _tmpF;struct _tuple3 _tmp10;enum 
Cyc_Absyn_AggrKind _tmp11;struct _tuple1*_tmp12;struct Cyc_List_List*_tmp13;struct
Cyc_List_List*_tmp14;struct Cyc_List_List**_tmp15;struct Cyc_List_List*_tmp16;int
_tmp17;struct Cyc_Absyn_Exp*_tmp19;_LL1: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp1=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp0;if(_tmp1->tag != 14)goto _LL3;
else{_tmp2=_tmp1->f1;}}_LL2:{struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{
int _tmp1C=0;if(setjmp(_tmp1A.handler))_tmp1C=1;if(!_tmp1C){{struct _RegionHandle*
_tmp1D=Cyc_Tcenv_get_fnrgn(te);void*_tmp1E=Cyc_Tcenv_lookup_ordinary(_tmp1D,te,p->loc,
_tmp2);struct Cyc_Absyn_Datatypedecl*_tmp21;struct Cyc_Absyn_Datatypefield*_tmp22;
struct Cyc_Absyn_Enumdecl*_tmp24;struct Cyc_Absyn_Enumfield*_tmp25;void*_tmp27;
struct Cyc_Absyn_Enumfield*_tmp28;_LLE: {struct Cyc_Tcenv_AggrRes_struct*_tmp1F=(
struct Cyc_Tcenv_AggrRes_struct*)_tmp1E;if(_tmp1F->tag != 1)goto _LL10;}_LLF: p->r=(
void*)& Cyc_Absyn_Wild_p_val;{const char*_tmp391;void*_tmp390;(_tmp390=0,Cyc_Tcutil_terr(
p->loc,((_tmp391="struct tag used without arguments in pattern",_tag_dyneither(
_tmp391,sizeof(char),45))),_tag_dyneither(_tmp390,sizeof(void*),0)));}
_npop_handler(0);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp20=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp1E;if(_tmp20->tag != 2)goto _LL12;else{
_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL11:{struct Cyc_Absyn_Datatype_p_struct
_tmp394;struct Cyc_Absyn_Datatype_p_struct*_tmp393;p->r=(void*)((_tmp393=
_cycalloc(sizeof(*_tmp393)),((_tmp393[0]=((_tmp394.tag=7,((_tmp394.f1=_tmp21,((
_tmp394.f2=_tmp22,((_tmp394.f3=0,((_tmp394.f4=0,_tmp394)))))))))),_tmp393))));}
_npop_handler(0);return;_LL12: {struct Cyc_Tcenv_EnumRes_struct*_tmp23=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp1E;if(_tmp23->tag != 3)goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}
_LL13:{struct Cyc_Absyn_Enum_p_struct _tmp397;struct Cyc_Absyn_Enum_p_struct*
_tmp396;p->r=(void*)((_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396[0]=((_tmp397.tag=
12,((_tmp397.f1=_tmp24,((_tmp397.f2=_tmp25,_tmp397)))))),_tmp396))));}
_npop_handler(0);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp26=(
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp1E;if(_tmp26->tag != 4)goto _LL16;else{
_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}_LL15:{struct Cyc_Absyn_AnonEnum_p_struct
_tmp39A;struct Cyc_Absyn_AnonEnum_p_struct*_tmp399;p->r=(void*)((_tmp399=
_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((_tmp39A.tag=13,((_tmp39A.f1=(void*)
_tmp27,((_tmp39A.f2=_tmp28,_tmp39A)))))),_tmp399))));}_npop_handler(0);return;
_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp29=(struct Cyc_Tcenv_VarRes_struct*)
_tmp1E;if(_tmp29->tag != 0)goto _LLD;}_LL17: goto _LLD;_LLD:;};_pop_handler();}else{
void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {struct Cyc_Dict_Absent_struct*
_tmp34=(struct Cyc_Dict_Absent_struct*)_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)
goto _LL1B;}_LL1A: goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{union
Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;_LL1E:
if((_tmp35.Loc_n).tag != 4)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F: goto
_LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(
_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:(*_tmp2).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_Var_p_struct _tmp39D;struct Cyc_Absyn_Var_p_struct*_tmp39C;p->r=(
void*)((_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C[0]=((_tmp39D.tag=1,((
_tmp39D.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((
_tmp39D.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp39D)))))),
_tmp39C))));}return;_LL22:;_LL23:{const char*_tmp3A0;void*_tmp39F;(_tmp39F=0,Cyc_Tcutil_terr(
p->loc,((_tmp3A0="qualified variable in pattern",_tag_dyneither(_tmp3A0,sizeof(
char),30))),_tag_dyneither(_tmp39F,sizeof(void*),0)));}return;_LL1D:;};_LL3: {
struct Cyc_Absyn_UnknownCall_p_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=
_tmp3->f3;}}_LL4:{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;
if(setjmp(_tmp3C.handler))_tmp3E=1;if(!_tmp3E){{struct _RegionHandle*_tmp3F=Cyc_Tcenv_get_fnrgn(
te);void*_tmp40=Cyc_Tcenv_lookup_ordinary(_tmp3F,te,p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*
_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;
_LL25: {struct Cyc_Tcenv_AggrRes_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {struct Cyc_List_List*
_tmp49=0;for(0;_tmp5 != 0;_tmp5=_tmp5->tl){struct _tuple14*_tmp3A3;struct Cyc_List_List*
_tmp3A2;_tmp49=((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2->hd=((_tmp3A3=
_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3->f1=0,((_tmp3A3->f2=(struct Cyc_Absyn_Pat*)
_tmp5->hd,_tmp3A3)))))),((_tmp3A2->tl=_tmp49,_tmp3A2))))));}{struct Cyc_Absyn_Aggr_p_struct
_tmp3AD;struct Cyc_Absyn_Aggrdecl**_tmp3AC;struct Cyc_Absyn_AggrInfo*_tmp3AB;
struct Cyc_Absyn_Aggr_p_struct*_tmp3AA;p->r=(void*)((_tmp3AA=_cycalloc(sizeof(*
_tmp3AA)),((_tmp3AA[0]=((_tmp3AD.tag=6,((_tmp3AD.f1=((_tmp3AB=_cycalloc(sizeof(*
_tmp3AB)),((_tmp3AB->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3AC=_cycalloc(sizeof(*
_tmp3AC)),((_tmp3AC[0]=_tmp42,_tmp3AC))))),((_tmp3AB->targs=0,_tmp3AB)))))),((
_tmp3AD.f2=0,((_tmp3AD.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp49),((_tmp3AD.f4=_tmp6,_tmp3AD)))))))))),_tmp3AA))));}_npop_handler(0);
return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp43=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp40;if(_tmp43->tag != 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}
_LL28:{struct Cyc_Absyn_Datatype_p_struct _tmp3B0;struct Cyc_Absyn_Datatype_p_struct*
_tmp3AF;p->r=(void*)((_tmp3AF=_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF[0]=((_tmp3B0.tag=
7,((_tmp3B0.f1=_tmp44,((_tmp3B0.f2=_tmp45,((_tmp3B0.f3=_tmp5,((_tmp3B0.f4=_tmp6,
_tmp3B0)))))))))),_tmp3AF))));}_npop_handler(0);return;_LL29: {struct Cyc_Tcenv_EnumRes_struct*
_tmp46=(struct Cyc_Tcenv_EnumRes_struct*)_tmp40;if(_tmp46->tag != 3)goto _LL2B;}
_LL2A: goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp47=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp40;if(_tmp47->tag != 4)goto _LL2D;}_LL2C:{const char*_tmp3B3;void*_tmp3B2;(
_tmp3B2=0,Cyc_Tcutil_terr(p->loc,((_tmp3B3="enum tag used with arguments in pattern",
_tag_dyneither(_tmp3B3,sizeof(char),40))),_tag_dyneither(_tmp3B2,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;_npop_handler(0);return;_LL2D: {struct Cyc_Tcenv_VarRes_struct*
_tmp48=(struct Cyc_Tcenv_VarRes_struct*)_tmp40;if(_tmp48->tag != 0)goto _LL24;}
_LL2E: goto _LL24;_LL24:;};_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;
void*_tmp55=_tmp3D;_LL30: {struct Cyc_Dict_Absent_struct*_tmp56=(struct Cyc_Dict_Absent_struct*)
_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31: goto _LL2F;_LL32:;_LL33:(
void)_throw(_tmp55);_LL2F:;}};}{const char*_tmp3B7;void*_tmp3B6[1];struct Cyc_String_pa_struct
_tmp3B5;(_tmp3B5.tag=0,((_tmp3B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3B6[0]=& _tmp3B5,Cyc_Tcutil_terr(p->loc,((
_tmp3B7="%s is not a constructor in pattern",_tag_dyneither(_tmp3B7,sizeof(char),
35))),_tag_dyneither(_tmp3B6,sizeof(void*),1)))))));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmp7->tag != 6)goto _LL7;else{_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;
_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=_tmp7->f4;}}_LL6: if(topt == 0){const char*
_tmp3BA;void*_tmp3B9;(_tmp3B9=0,Cyc_Tcutil_terr(p->loc,((_tmp3BA="cannot determine pattern type",
_tag_dyneither(_tmp3BA,sizeof(char),30))),_tag_dyneither(_tmp3B9,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));{void*_tmp5C=t;struct
Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct Cyc_Absyn_AggrType_struct*_tmp5D=(struct
Cyc_Absyn_AggrType_struct*)_tmp5C;if(_tmp5D->tag != 12)goto _LL37;else{_tmp5E=
_tmp5D->f1;}}_LL36:{struct Cyc_Absyn_Aggr_p_struct _tmp3C0;struct Cyc_Absyn_AggrInfo*
_tmp3BF;struct Cyc_Absyn_Aggr_p_struct*_tmp3BE;p->r=(void*)((_tmp3BE=_cycalloc(
sizeof(*_tmp3BE)),((_tmp3BE[0]=((_tmp3C0.tag=6,((_tmp3C0.f1=((_tmp3BF=_cycalloc(
sizeof(*_tmp3BF)),((_tmp3BF[0]=_tmp5E,_tmp3BF)))),((_tmp3C0.f2=_tmp9,((_tmp3C0.f3=
_tmpA,((_tmp3C0.f4=_tmpB,_tmp3C0)))))))))),_tmp3BE))));}Cyc_Tcpat_resolve_pat(te,
topt,p);goto _LL34;_LL37:;_LL38:{const char*_tmp3C4;void*_tmp3C3[1];struct Cyc_String_pa_struct
_tmp3C2;(_tmp3C2.tag=0,((_tmp3C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3C3[0]=& _tmp3C2,Cyc_Tcutil_terr(p->loc,((
_tmp3C4="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3C4,
sizeof(char),45))),_tag_dyneither(_tmp3C3,sizeof(void*),1)))))));}goto _LL34;
_LL34:;}return;};_LL7: {struct Cyc_Absyn_Aggr_p_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;
_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;
_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;
_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;
_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:{struct _handler_cons _tmp65;
_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))_tmp67=1;if(!
_tmp67){{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmp12);struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;if(_tmp69->impl == 0){{const char*
_tmp3CC;void*_tmp3CB[1];const char*_tmp3CA;const char*_tmp3C9;struct Cyc_String_pa_struct
_tmp3C8;(_tmp3C8.tag=0,((_tmp3C8.f1=(struct _dyneither_ptr)(_tmp69->kind == Cyc_Absyn_StructA?(
_tmp3C9="struct",_tag_dyneither(_tmp3C9,sizeof(char),7)):((_tmp3CA="union",
_tag_dyneither(_tmp3CA,sizeof(char),6)))),((_tmp3CB[0]=& _tmp3C8,Cyc_Tcutil_terr(
p->loc,((_tmp3CC="can't destructure an abstract %s",_tag_dyneither(_tmp3CC,
sizeof(char),33))),_tag_dyneither(_tmp3CB,sizeof(void*),1)))))));}p->r=(void*)&
Cyc_Absyn_Wild_p_val;_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp15);if(more_exists < 0){{
const char*_tmp3CF;void*_tmp3CE;(_tmp3CE=0,Cyc_Tcutil_terr(p->loc,((_tmp3CF="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp3CF,sizeof(char),55))),_tag_dyneither(_tmp3CE,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp71=_tmp15;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars);for(0;n != 
0;-- n){_tmp71=&((struct Cyc_List_List*)_check_null(*_tmp71))->tl;}}*_tmp71=0;};}
else{if(more_exists > 0){struct Cyc_List_List*_tmp72=0;for(0;more_exists != 0;--
more_exists){struct Cyc_Absyn_Unknown_kb_struct*_tmp3D5;struct Cyc_Absyn_Unknown_kb_struct
_tmp3D4;struct Cyc_List_List*_tmp3D3;_tmp72=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((
_tmp3D3->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((
_tmp3D5[0]=((_tmp3D4.tag=1,((_tmp3D4.f1=0,_tmp3D4)))),_tmp3D5))))),((_tmp3D3->tl=
_tmp72,_tmp3D3))))));}*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp15,_tmp72);}}{struct Cyc_Absyn_Aggr_p_struct
_tmp3DB;struct Cyc_Absyn_AggrInfo*_tmp3DA;struct Cyc_Absyn_Aggr_p_struct*_tmp3D9;p->r=(
void*)((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DB.tag=6,((
_tmp3DB.f1=((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->aggr_info=Cyc_Absyn_KnownAggr(
_tmp68),((_tmp3DA->targs=_tmp13,_tmp3DA)))))),((_tmp3DB.f2=*_tmp15,((_tmp3DB.f3=
_tmp16,((_tmp3DB.f4=_tmp17,_tmp3DB)))))))))),_tmp3D9))));};};};_pop_handler();}
else{void*_tmp66=(void*)_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {struct Cyc_Dict_Absent_struct*
_tmp7B=(struct Cyc_Dict_Absent_struct*)_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)
goto _LL3C;}_LL3B:{const char*_tmp3DE;void*_tmp3DD;(_tmp3DD=0,Cyc_Tcutil_terr(p->loc,((
_tmp3DE="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3DE,
sizeof(char),43))),_tag_dyneither(_tmp3DD,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}return;_LL9: {struct Cyc_Absyn_Exp_p_struct*
_tmp18=(struct Cyc_Absyn_Exp_p_struct*)_tmp0;if(_tmp18->tag != 16)goto _LLB;else{
_tmp19=_tmp18->f1;}}_LLA: Cyc_Tcexp_tcExp(te,0,_tmp19);if(!Cyc_Tcutil_is_const_exp(
te,_tmp19)){{const char*_tmp3E1;void*_tmp3E0;(_tmp3E0=0,Cyc_Tcutil_terr(p->loc,((
_tmp3E1="non-constant expression in case pattern",_tag_dyneither(_tmp3E1,sizeof(
char),40))),_tag_dyneither(_tmp3E0,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
unsigned int _tmp81;int _tmp82;struct _tuple13 _tmp80=Cyc_Evexp_eval_const_uint_exp(
_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{struct Cyc_Absyn_Int_p_struct _tmp3E4;
struct Cyc_Absyn_Int_p_struct*_tmp3E3;p->r=(void*)((_tmp3E3=_cycalloc_atomic(
sizeof(*_tmp3E3)),((_tmp3E3[0]=((_tmp3E4.tag=9,((_tmp3E4.f1=Cyc_Absyn_None,((
_tmp3E4.f2=(int)_tmp81,_tmp3E4)))))),_tmp3E3))));}return;};_LLB:;_LLC: return;
_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**
topt){if(topt != 0)return*topt;{struct Cyc_Core_Opt*_tmp3E5;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((
_tmp3E5->v=s,_tmp3E5)))));};}static void*Cyc_Tcpat_num_type(void**topt,void*numt){
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp86=Cyc_Tcutil_compress(
numt);_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmp87=(struct Cyc_Absyn_EnumType_struct*)
_tmp86;if(_tmp87->tag != 14)goto _LL41;}_LL40: if(topt != 0)return*topt;goto _LL3E;
_LL41:;_LL42: goto _LL3E;_LL3E:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_List_List**v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(
0);{struct Cyc_List_List*_tmp3E6;*v_result_ptr=(struct Cyc_List_List*)((_tmp3E6=
_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6->hd=vd,((_tmp3E6->tl=*v_result_ptr,_tmp3E6))))));};}
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple12*_tmp8A;struct Cyc_List_List*
_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;
_tmp8B=_tmp89.patvars;{struct _tuple12*_tmp8D;struct Cyc_List_List*_tmp8E;struct
Cyc_Tcpat_TcPatResult _tmp8C=res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=
_tmp8C.patvars;if(_tmp8A != 0  || _tmp8D != 0){if(_tmp8A == 0){struct _tuple12*
_tmp3E7;_tmp8A=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->f1=0,((_tmp3E7->f2=
0,_tmp3E7))))));}if(_tmp8D == 0){struct _tuple12*_tmp3E8;_tmp8D=((_tmp3E8=
_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8->f1=0,((_tmp3E8->f2=0,_tmp3E8))))));}{
struct _tuple12*_tmp3EB;struct Cyc_Tcpat_TcPatResult _tmp3EA;return(_tmp3EA.tvars_and_bounds_opt=((
_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f1,(*
_tmp8D).f1),((_tmp3EB->f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f2,(*_tmp8D).f2),_tmp3EB)))))),((
_tmp3EA.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp3EA)));};}{struct Cyc_Tcpat_TcPatResult
_tmp3EC;return(_tmp3EC.tvars_and_bounds_opt=0,((_tmp3EC.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E),
_tmp3EC)));};};}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Pat*_tmp3ED;return(_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((
_tmp3ED->loc=loc,((_tmp3ED->topt=0,((_tmp3ED->r=(void*)& Cyc_Absyn_Wild_p_val,
_tmp3ED)))))));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple16{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,topt,p);{void*t;struct Cyc_Tcpat_TcPatResult
_tmp3EE;struct Cyc_Tcpat_TcPatResult res=(_tmp3EE.tvars_and_bounds_opt=0,((_tmp3EE.patvars=
0,_tmp3EE)));{void*_tmp95=p->r;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Pat*
_tmp99;struct Cyc_Absyn_Vardecl*_tmp9B;struct Cyc_Absyn_Pat*_tmp9C;struct Cyc_Absyn_Tvar*
_tmp9E;struct Cyc_Absyn_Vardecl*_tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Sign
_tmpA3;enum Cyc_Absyn_Sign _tmpA5;struct Cyc_Absyn_Enumdecl*_tmpA9;void*_tmpAB;
struct Cyc_Absyn_Pat*_tmpAE;struct Cyc_List_List*_tmpB0;struct Cyc_List_List**
_tmpB1;int _tmpB2;struct Cyc_Absyn_AggrInfo*_tmpB4;struct Cyc_Absyn_AggrInfo _tmpB5;
union Cyc_Absyn_AggrInfoU _tmpB6;struct Cyc_Absyn_Aggrdecl**_tmpB7;struct Cyc_Absyn_Aggrdecl*
_tmpB8;struct Cyc_List_List*_tmpB9;struct Cyc_List_List**_tmpBA;struct Cyc_List_List*
_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_List_List**_tmpBD;int _tmpBE;struct
Cyc_Absyn_Datatypedecl*_tmpC0;struct Cyc_Absyn_Datatypefield*_tmpC1;struct Cyc_List_List*
_tmpC2;struct Cyc_List_List**_tmpC3;int _tmpC4;struct Cyc_Absyn_AggrInfo*_tmpC6;
struct Cyc_Absyn_AggrInfo*_tmpC8;struct Cyc_Absyn_AggrInfo _tmpC9;union Cyc_Absyn_AggrInfoU
_tmpCA;struct _tuple3 _tmpCB;_LL44: {struct Cyc_Absyn_Wild_p_struct*_tmp96=(struct
Cyc_Absyn_Wild_p_struct*)_tmp95;if(_tmp96->tag != 0)goto _LL46;}_LL45: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL43;_LL46: {struct Cyc_Absyn_Var_p_struct*
_tmp97=(struct Cyc_Absyn_Var_p_struct*)_tmp95;if(_tmp97->tag != 1)goto _LL48;else{
_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL47: res=Cyc_Tcpat_tcPatRec(te,_tmp99,
topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp99->topt))->v;Cyc_Tcpat_set_vd(
_tmp98,& res.patvars,t);goto _LL43;_LL48: {struct Cyc_Absyn_Reference_p_struct*
_tmp9A=(struct Cyc_Absyn_Reference_p_struct*)_tmp95;if(_tmp9A->tag != 2)goto _LL4A;
else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL49: res=Cyc_Tcpat_tcPatRec(te,
_tmp9C,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->topt))->v;
if(rgn_opt == 0){{const char*_tmp3F1;void*_tmp3F0;(_tmp3F0=0,Cyc_Tcutil_terr(p->loc,((
_tmp3F1="* pattern would point into an unknown/unallowed region",_tag_dyneither(
_tmp3F1,sizeof(char),55))),_tag_dyneither(_tmp3F0,sizeof(void*),0)));}goto _LL43;}
else{if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp3F4;void*_tmp3F3;(
_tmp3F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp3F4="* pattern cannot take the address of an alias-free path",_tag_dyneither(
_tmp3F4,sizeof(char),56))),_tag_dyneither(_tmp3F3,sizeof(void*),0)));}}{struct
Cyc_Absyn_PointerType_struct _tmp3FE;struct Cyc_Absyn_PtrAtts _tmp3FD;struct Cyc_Absyn_PtrInfo
_tmp3FC;struct Cyc_Absyn_PointerType_struct*_tmp3FB;Cyc_Tcpat_set_vd(_tmp9B,& res.patvars,(
void*)((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB[0]=((_tmp3FE.tag=5,((
_tmp3FE.f1=((_tmp3FC.elt_typ=t,((_tmp3FC.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp3FC.ptr_atts=((_tmp3FD.rgn=*rgn_opt,((_tmp3FD.nullable=Cyc_Absyn_false_conref,((
_tmp3FD.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp3FD.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp3FD.ptrloc=0,_tmp3FD)))))))))),_tmp3FC)))))),_tmp3FE)))),_tmp3FB)))));}goto
_LL43;_LL4A: {struct Cyc_Absyn_TagInt_p_struct*_tmp9D=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp95;if(_tmp9D->tag != 3)goto _LL4C;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}
_LL4B: Cyc_Tcpat_set_vd(_tmp9F,& res.patvars,_tmp9F->type);{struct _RegionHandle*
_tmpD7=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Absyn_Tvar*_tmp3FF[1];Cyc_Tcenv_add_type_vars(
_tmpD7,p->loc,te,((_tmp3FF[0]=_tmp9E,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FF,sizeof(struct Cyc_Absyn_Tvar*),
1)))));}if(res.tvars_and_bounds_opt == 0){struct _tuple12*_tmp400;res.tvars_and_bounds_opt=((
_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400->f1=0,((_tmp400->f2=0,_tmp400))))));}{
struct Cyc_List_List*_tmp401;(*res.tvars_and_bounds_opt).f1=((_tmp401=_cycalloc(
sizeof(*_tmp401)),((_tmp401->hd=_tmp9E,((_tmp401->tl=(*res.tvars_and_bounds_opt).f1,
_tmp401))))));}t=Cyc_Absyn_uint_typ;goto _LL43;};_LL4C: {struct Cyc_Absyn_Int_p_struct*
_tmpA0=(struct Cyc_Absyn_Int_p_struct*)_tmp95;if(_tmpA0->tag != 9)goto _LL4E;else{
_tmpA1=_tmpA0->f1;if(_tmpA1 != Cyc_Absyn_Unsigned)goto _LL4E;}}_LL4D: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_uint_typ);goto _LL43;_LL4E: {struct Cyc_Absyn_Int_p_struct*_tmpA2=(
struct Cyc_Absyn_Int_p_struct*)_tmp95;if(_tmpA2->tag != 9)goto _LL50;else{_tmpA3=
_tmpA2->f1;if(_tmpA3 != Cyc_Absyn_None)goto _LL50;}}_LL4F: goto _LL51;_LL50: {struct
Cyc_Absyn_Int_p_struct*_tmpA4=(struct Cyc_Absyn_Int_p_struct*)_tmp95;if(_tmpA4->tag
!= 9)goto _LL52;else{_tmpA5=_tmpA4->f1;if(_tmpA5 != Cyc_Absyn_Signed)goto _LL52;}}
_LL51: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL43;_LL52: {struct Cyc_Absyn_Char_p_struct*
_tmpA6=(struct Cyc_Absyn_Char_p_struct*)_tmp95;if(_tmpA6->tag != 10)goto _LL54;}
_LL53: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL43;_LL54: {struct Cyc_Absyn_Float_p_struct*
_tmpA7=(struct Cyc_Absyn_Float_p_struct*)_tmp95;if(_tmpA7->tag != 11)goto _LL56;}
_LL55: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL43;_LL56: {struct Cyc_Absyn_Enum_p_struct*
_tmpA8=(struct Cyc_Absyn_Enum_p_struct*)_tmp95;if(_tmpA8->tag != 12)goto _LL58;
else{_tmpA9=_tmpA8->f1;}}_LL57:{struct Cyc_Absyn_EnumType_struct _tmp404;struct Cyc_Absyn_EnumType_struct*
_tmp403;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((
_tmp403[0]=((_tmp404.tag=14,((_tmp404.f1=_tmpA9->name,((_tmp404.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpA9,_tmp404)))))),_tmp403)))));}goto _LL43;_LL58: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmpAA=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp95;if(_tmpAA->tag != 13)goto _LL5A;
else{_tmpAB=(void*)_tmpAA->f1;}}_LL59: t=Cyc_Tcpat_num_type(topt,_tmpAB);goto
_LL43;_LL5A: {struct Cyc_Absyn_Null_p_struct*_tmpAC=(struct Cyc_Absyn_Null_p_struct*)
_tmp95;if(_tmpAC->tag != 8)goto _LL5C;}_LL5B: if(topt != 0){void*_tmpDD=Cyc_Tcutil_compress(*
topt);_LL6F: {struct Cyc_Absyn_PointerType_struct*_tmpDE=(struct Cyc_Absyn_PointerType_struct*)
_tmpDD;if(_tmpDE->tag != 5)goto _LL71;}_LL70: t=*topt;goto tcpat_end;_LL71:;_LL72:
goto _LL6E;_LL6E:;}{struct Cyc_Core_Opt*_tmpDF=Cyc_Tcenv_lookup_opt_type_vars(te);{
struct Cyc_Absyn_PointerType_struct _tmp40E;struct Cyc_Absyn_PtrAtts _tmp40D;struct
Cyc_Absyn_PtrInfo _tmp40C;struct Cyc_Absyn_PointerType_struct*_tmp40B;t=(void*)((
_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=((_tmp40E.tag=5,((_tmp40E.f1=((
_tmp40C.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmpDF),((
_tmp40C.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp40C.ptr_atts=((_tmp40D.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmpDF),((_tmp40D.nullable=Cyc_Absyn_true_conref,((
_tmp40D.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp40D.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp40D.ptrloc=0,_tmp40D)))))))))),_tmp40C)))))),_tmp40E)))),_tmp40B))));}goto
_LL43;};_LL5C: {struct Cyc_Absyn_Pointer_p_struct*_tmpAD=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp95;if(_tmpAD->tag != 5)goto _LL5E;else{_tmpAE=_tmpAD->f1;}}_LL5D: {void*
inner_typ=(void*)& Cyc_Absyn_VoidType_val;void**_tmpE4=0;int elt_const=0;if(topt != 
0){void*_tmpE5=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmpE7;void*
_tmpE8;struct Cyc_Absyn_Tqual _tmpE9;_LL74: {struct Cyc_Absyn_PointerType_struct*
_tmpE6=(struct Cyc_Absyn_PointerType_struct*)_tmpE5;if(_tmpE6->tag != 5)goto _LL76;
else{_tmpE7=_tmpE6->f1;_tmpE8=_tmpE7.elt_typ;_tmpE9=_tmpE7.elt_tq;}}_LL75:
inner_typ=_tmpE8;_tmpE4=(void**)& inner_typ;elt_const=_tmpE9.real_const;goto _LL73;
_LL76:;_LL77: goto _LL73;_LL73:;}{void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmpAE,_tmpE4,(void**)& ptr_rgn));{void*_tmpEA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v);struct Cyc_Absyn_DatatypeFieldInfo
_tmpEC;union Cyc_Absyn_DatatypeFieldInfoU _tmpED;struct _tuple2 _tmpEE;struct Cyc_Absyn_Datatypedecl*
_tmpEF;struct Cyc_Absyn_Datatypefield*_tmpF0;struct Cyc_List_List*_tmpF1;_LL79: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmpEB=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpEA;if(_tmpEB->tag != 4)goto _LL7B;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEC.field_info;
if((_tmpED.KnownDatatypefield).tag != 2)goto _LL7B;_tmpEE=(struct _tuple2)(_tmpED.KnownDatatypefield).val;
_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;_tmpF1=_tmpEC.targs;}}_LL7A:{void*_tmpF2=Cyc_Tcutil_compress(
inner_typ);_LL7E: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpF3=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF2;if(_tmpF3->tag != 4)goto _LL80;}_LL7F: goto DONT_PROMOTE;_LL80:;_LL81: goto
_LL7D;_LL7D:;}{struct Cyc_Absyn_DatatypeType_struct _tmp418;struct Cyc_Absyn_Datatypedecl**
_tmp417;struct Cyc_Absyn_DatatypeInfo _tmp416;struct Cyc_Absyn_DatatypeType_struct*
_tmp415;struct Cyc_Absyn_DatatypeType_struct*_tmpF4=(_tmp415=_cycalloc(sizeof(*
_tmp415)),((_tmp415[0]=((_tmp418.tag=3,((_tmp418.f1=((_tmp416.datatype_info=Cyc_Absyn_KnownDatatype(((
_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417[0]=_tmpEF,_tmp417))))),((_tmp416.targs=
_tmpF1,_tmp416)))),_tmp418)))),_tmp415)));((struct Cyc_Core_Opt*)_check_null(
_tmpAE->topt))->v=(void*)((void*)_tmpF4);{struct Cyc_Absyn_PointerType_struct
_tmp422;struct Cyc_Absyn_PtrAtts _tmp421;struct Cyc_Absyn_PtrInfo _tmp420;struct Cyc_Absyn_PointerType_struct*
_tmp41F;t=(void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=((_tmp422.tag=
5,((_tmp422.f1=((_tmp420.elt_typ=(void*)_tmpF4,((_tmp420.elt_tq=elt_const?Cyc_Absyn_const_tqual(
0): Cyc_Absyn_empty_tqual(0),((_tmp420.ptr_atts=((_tmp421.rgn=ptr_rgn,((_tmp421.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp421.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp421.zero_term=Cyc_Absyn_false_conref,((_tmp421.ptrloc=0,_tmp421)))))))))),
_tmp420)))))),_tmp422)))),_tmp41F))));}goto _LL78;};_LL7B:;_LL7C: DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_struct _tmp42C;struct Cyc_Absyn_PtrAtts _tmp42B;struct
Cyc_Absyn_PtrInfo _tmp42A;struct Cyc_Absyn_PointerType_struct*_tmp429;t=(void*)((
_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429[0]=((_tmp42C.tag=5,((_tmp42C.f1=((
_tmp42A.elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v,((
_tmp42A.elt_tq=elt_const?Cyc_Absyn_const_tqual(0): Cyc_Absyn_empty_tqual(0),((
_tmp42A.ptr_atts=((_tmp42B.rgn=ptr_rgn,((_tmp42B.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp42B.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp42B.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp42B.ptrloc=0,_tmp42B)))))))))),_tmp42A)))))),_tmp42C)))),_tmp429))));}_LL78:;}
goto _LL43;};}_LL5E: {struct Cyc_Absyn_Tuple_p_struct*_tmpAF=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp95;if(_tmpAF->tag != 4)goto _LL60;else{_tmpB0=_tmpAF->f1;_tmpB1=(struct Cyc_List_List**)&
_tmpAF->f1;_tmpB2=_tmpAF->f2;}}_LL5F: {struct Cyc_List_List*_tmp101=*_tmpB1;
struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*
_tmp102=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp104;_LL83: {struct Cyc_Absyn_TupleType_struct*
_tmp103=(struct Cyc_Absyn_TupleType_struct*)_tmp102;if(_tmp103->tag != 11)goto
_LL85;else{_tmp104=_tmp103->f1;}}_LL84: topt_ts=_tmp104;if(_tmpB2){int _tmp105=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101);int _tmp106=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp104);if(_tmp105 < _tmp106){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp106 - _tmp105;++ i){struct Cyc_List_List*_tmp42D;
wild_ps=((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp42D->tl=wild_ps,_tmp42D))))));}}*_tmpB1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp101,wild_ps);_tmp101=*_tmpB1;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp104)){const char*_tmp430;void*_tmp42F;(
_tmp42F=0,Cyc_Tcutil_warn(p->loc,((_tmp430="unnecessary ... in tuple pattern",
_tag_dyneither(_tmp430,sizeof(char),33))),_tag_dyneither(_tmp42F,sizeof(void*),0)));}}}
goto _LL82;_LL85:;_LL86: goto _LL82;_LL82:;}else{if(_tmpB2){const char*_tmp433;void*
_tmp432;(_tmp432=0,Cyc_Tcutil_terr(p->loc,((_tmp433="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp433,sizeof(char),57))),_tag_dyneither(_tmp432,sizeof(void*),0)));}}
for(0;_tmp101 != 0;_tmp101=_tmp101->tl){void**_tmp10C=0;if(topt_ts != 0){_tmp10C=(
void**)&(*((struct _tuple15*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp101->hd,_tmp10C,rgn_opt));{
struct _tuple15*_tmp436;struct Cyc_List_List*_tmp435;pat_ts=((_tmp435=_cycalloc(
sizeof(*_tmp435)),((_tmp435->hd=((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436->f1=
Cyc_Absyn_empty_tqual(0),((_tmp436->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmp101->hd)->topt))->v,_tmp436)))))),((_tmp435->tl=pat_ts,
_tmp435))))));};}{struct Cyc_Absyn_TupleType_struct _tmp439;struct Cyc_Absyn_TupleType_struct*
_tmp438;t=(void*)((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp439.tag=
11,((_tmp439.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp439)))),_tmp438))));}goto _LL43;}_LL60: {struct Cyc_Absyn_Aggr_p_struct*
_tmpB3=(struct Cyc_Absyn_Aggr_p_struct*)_tmp95;if(_tmpB3->tag != 6)goto _LL62;else{
_tmpB4=_tmpB3->f1;if(_tmpB4 == 0)goto _LL62;_tmpB5=*_tmpB4;_tmpB6=_tmpB5.aggr_info;
if((_tmpB6.KnownAggr).tag != 2)goto _LL62;_tmpB7=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB6.KnownAggr).val;_tmpB8=*_tmpB7;_tmpB9=_tmpB5.targs;_tmpBA=(struct Cyc_List_List**)&(*
_tmpB3->f1).targs;_tmpBB=_tmpB3->f2;_tmpBC=_tmpB3->f3;_tmpBD=(struct Cyc_List_List**)&
_tmpB3->f3;_tmpBE=_tmpB3->f4;}}_LL61: {struct Cyc_List_List*_tmp111=*_tmpBD;const
char*_tmp43B;const char*_tmp43A;struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?(
_tmp43B="struct",_tag_dyneither(_tmp43B,sizeof(char),7)):((_tmp43A="union",
_tag_dyneither(_tmp43A,sizeof(char),6)));if(_tmpB8->impl == 0){{const char*_tmp43F;
void*_tmp43E[1];struct Cyc_String_pa_struct _tmp43D;(_tmp43D.tag=0,((_tmp43D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp43E[0]=& _tmp43D,Cyc_Tcutil_terr(
p->loc,((_tmp43F="can't destructure an abstract %s",_tag_dyneither(_tmp43F,
sizeof(char),33))),_tag_dyneither(_tmp43E,sizeof(void*),1)))))));}t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;}if(_tmpBB != 0  || _tmpB8->kind == 
Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->tagged)
rgn_opt=0;{struct _RegionHandle _tmp115=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp115;_push_region(rgn);{struct Cyc_List_List*_tmp116=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmp117=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->exist_vars;{struct Cyc_List_List*t=_tmpBB;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp117))->hd;
_tmp117=_tmp117->tl;{void*_tmp118=Cyc_Absyn_compress_kb(tv->kind);void*_tmp119=
Cyc_Absyn_compress_kb(uv->kind);int error=0;struct Cyc_Absyn_Kind*k2;{void*_tmp11A=
_tmp119;struct Cyc_Absyn_Kind*_tmp11C;struct Cyc_Absyn_Kind*_tmp11E;_LL88: {struct
Cyc_Absyn_Less_kb_struct*_tmp11B=(struct Cyc_Absyn_Less_kb_struct*)_tmp11A;if(
_tmp11B->tag != 2)goto _LL8A;else{_tmp11C=_tmp11B->f2;}}_LL89: _tmp11E=_tmp11C;goto
_LL8B;_LL8A: {struct Cyc_Absyn_Eq_kb_struct*_tmp11D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp11A;if(_tmp11D->tag != 0)goto _LL8C;else{_tmp11E=_tmp11D->f1;}}_LL8B: k2=
_tmp11E;goto _LL87;_LL8C:;_LL8D: {const char*_tmp442;void*_tmp441;(_tmp441=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp442="unconstrained existential type variable in aggregate",
_tag_dyneither(_tmp442,sizeof(char),53))),_tag_dyneither(_tmp441,sizeof(void*),0)));}
_LL87:;}{void*_tmp121=_tmp118;struct Cyc_Absyn_Kind*_tmp123;struct Cyc_Core_Opt*
_tmp125;struct Cyc_Core_Opt**_tmp126;struct Cyc_Absyn_Kind*_tmp127;struct Cyc_Core_Opt*
_tmp129;struct Cyc_Core_Opt**_tmp12A;_LL8F: {struct Cyc_Absyn_Eq_kb_struct*_tmp122=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp121;if(_tmp122->tag != 0)goto _LL91;else{_tmp123=
_tmp122->f1;}}_LL90: if(!Cyc_Tcutil_kind_leq(k2,_tmp123))error=1;goto _LL8E;_LL91: {
struct Cyc_Absyn_Less_kb_struct*_tmp124=(struct Cyc_Absyn_Less_kb_struct*)_tmp121;
if(_tmp124->tag != 2)goto _LL93;else{_tmp125=_tmp124->f1;_tmp126=(struct Cyc_Core_Opt**)&
_tmp124->f1;_tmp127=_tmp124->f2;}}_LL92: _tmp12A=_tmp126;goto _LL94;_LL93: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp128=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp121;
if(_tmp128->tag != 1)goto _LL8E;else{_tmp129=_tmp128->f1;_tmp12A=(struct Cyc_Core_Opt**)&
_tmp128->f1;}}_LL94:{struct Cyc_Core_Opt*_tmp443;*_tmp12A=((_tmp443=_cycalloc(
sizeof(*_tmp443)),((_tmp443->v=_tmp119,_tmp443))));}goto _LL8E;_LL8E:;}if(error){
const char*_tmp449;void*_tmp448[3];struct Cyc_String_pa_struct _tmp447;struct Cyc_String_pa_struct
_tmp446;struct Cyc_String_pa_struct _tmp445;(_tmp445.tag=0,((_tmp445.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(k2)),((_tmp446.tag=
0,((_tmp446.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
_tmp118)),((_tmp447.tag=0,((_tmp447.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*tv->name),((_tmp448[0]=& _tmp447,((_tmp448[1]=& _tmp446,((_tmp448[
2]=& _tmp445,Cyc_Tcutil_terr(p->loc,((_tmp449="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp449,sizeof(char),60))),_tag_dyneither(_tmp448,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_Absyn_VarType_struct _tmp44C;struct Cyc_Absyn_VarType_struct*_tmp44B;
void*vartype=(void*)((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B[0]=((_tmp44C.tag=
2,((_tmp44C.f1=tv,_tmp44C)))),_tmp44B))));{struct Cyc_List_List*_tmp44D;_tmp116=((
_tmp44D=_region_malloc(rgn,sizeof(*_tmp44D)),((_tmp44D->hd=(void*)vartype,((
_tmp44D->tl=_tmp116,_tmp44D))))));}if(k2->kind == Cyc_Absyn_RgnKind){if(k2->aliasqual
== Cyc_Absyn_Aliasable){struct _tuple0*_tmp450;struct Cyc_List_List*_tmp44F;
outlives_constraints=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F->hd=((
_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450->f1=Cyc_Absyn_empty_effect,((
_tmp450->f2=vartype,_tmp450)))))),((_tmp44F->tl=outlives_constraints,_tmp44F))))));}
else{const char*_tmp453;void*_tmp452;(_tmp452=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp453="opened existential had unique or top region kind",
_tag_dyneither(_tmp453,sizeof(char),49))),_tag_dyneither(_tmp452,sizeof(void*),0)));}}};};}}
_tmp116=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp116);{struct _RegionHandle*_tmp138=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_add_type_vars(_tmp138,p->loc,te,_tmpBB);struct Cyc_List_List*
_tmp139=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple10 _tmp454;struct _tuple10
_tmp13A=(_tmp454.f1=_tmp139,((_tmp454.f2=rgn,_tmp454)));struct Cyc_List_List*
_tmp13B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(
struct _tuple10*,struct Cyc_Absyn_Tvar*),struct _tuple10*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13A,_tmpB8->tvs);struct Cyc_List_List*
_tmp13C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->exist_vars,_tmp116);struct Cyc_List_List*_tmp13D=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple11*))Cyc_Core_snd,_tmp13B);struct Cyc_List_List*_tmp13E=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple11*))Cyc_Core_snd,_tmp13C);struct Cyc_List_List*_tmp13F=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmp13B,_tmp13C);if(_tmpBB != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0){struct
_tuple12*_tmp455;res.tvars_and_bounds_opt=((_tmp455=_cycalloc(sizeof(*_tmp455)),((
_tmp455->f1=0,((_tmp455->f2=0,_tmp455))))));}(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmpBB);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp141=0;{struct Cyc_List_List*
_tmp142=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po;for(0;
_tmp142 != 0;_tmp142=_tmp142->tl){struct _tuple0*_tmp458;struct Cyc_List_List*
_tmp457;_tmp141=((_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457->hd=((_tmp458=
_cycalloc(sizeof(*_tmp458)),((_tmp458->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp13F,(*((
struct _tuple0*)_tmp142->hd)).f1),((_tmp458->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp13F,(*((struct _tuple0*)_tmp142->hd)).f2),_tmp458)))))),((_tmp457->tl=_tmp141,
_tmp457))))));}}_tmp141=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp141);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp141);};}*
_tmpBA=_tmp13D;{struct Cyc_Absyn_AggrType_struct _tmp462;struct Cyc_Absyn_Aggrdecl**
_tmp461;struct Cyc_Absyn_AggrInfo _tmp460;struct Cyc_Absyn_AggrType_struct*_tmp45F;
t=(void*)((_tmp45F=_cycalloc(sizeof(*_tmp45F)),((_tmp45F[0]=((_tmp462.tag=12,((
_tmp462.f1=((_tmp460.aggr_info=Cyc_Absyn_KnownAggr(((_tmp461=_cycalloc(sizeof(*
_tmp461)),((_tmp461[0]=_tmpB8,_tmp461))))),((_tmp460.targs=*_tmpBA,_tmp460)))),
_tmp462)))),_tmp45F))));}if(_tmpBE  && _tmpB8->kind == Cyc_Absyn_UnionA){const char*
_tmp465;void*_tmp464;(_tmp464=0,Cyc_Tcutil_warn(p->loc,((_tmp465="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp465,sizeof(char),43))),_tag_dyneither(_tmp464,sizeof(void*),0)));}
else{if(_tmpBE){int _tmp14B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp111);int _tmp14C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->fields);if(_tmp14B < _tmp14C){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp14C - _tmp14B;++ i){struct _tuple14*_tmp468;struct
Cyc_List_List*_tmp467;wild_dps=((_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467->hd=((
_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468->f1=0,((_tmp468->f2=Cyc_Tcpat_wild_pat(
p->loc),_tmp468)))))),((_tmp467->tl=wild_dps,_tmp467))))));}}*_tmpBD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp111,
wild_dps);_tmp111=*_tmpBD;}else{if(_tmp14B == _tmp14C){const char*_tmp46B;void*
_tmp46A;(_tmp46A=0,Cyc_Tcutil_warn(p->loc,((_tmp46B="unnecessary ... in struct pattern",
_tag_dyneither(_tmp46B,sizeof(char),34))),_tag_dyneither(_tmp46A,sizeof(void*),0)));}}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,
_tmp111,_tmpB8->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple16 _tmp152;struct Cyc_Absyn_Aggrfield*
_tmp153;struct Cyc_Absyn_Pat*_tmp154;struct _tuple16*_tmp151=(struct _tuple16*)
fields->hd;_tmp152=*_tmp151;_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{void*_tmp155=
Cyc_Tcutil_rsubstitute(rgn,_tmp13F,_tmp153->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp154,(void**)& _tmp155,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp154->topt))->v,_tmp155)){const char*
_tmp472;void*_tmp471[4];struct Cyc_String_pa_struct _tmp470;struct Cyc_String_pa_struct
_tmp46F;struct Cyc_String_pa_struct _tmp46E;struct Cyc_String_pa_struct _tmp46D;(
_tmp46D.tag=0,((_tmp46D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp154->topt))->v)),((_tmp46E.tag=0,((
_tmp46E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp155)),((_tmp46F.tag=0,((_tmp46F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp153->name),((_tmp471[0]=& _tmp470,((_tmp471[1]=& _tmp46F,((
_tmp471[2]=& _tmp46E,((_tmp471[3]=& _tmp46D,Cyc_Tcutil_terr(p->loc,((_tmp472="field %s of %s pattern expects type %s != %s",
_tag_dyneither(_tmp472,sizeof(char),45))),_tag_dyneither(_tmp471,sizeof(void*),4)))))))))))))))))))))))));}};}};};}
_npop_handler(0);goto _LL43;;_pop_region(rgn);};}_LL62: {struct Cyc_Absyn_Datatype_p_struct*
_tmpBF=(struct Cyc_Absyn_Datatype_p_struct*)_tmp95;if(_tmpBF->tag != 7)goto _LL64;
else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=_tmpBF->f3;_tmpC3=(struct Cyc_List_List**)&
_tmpBF->f3;_tmpC4=_tmpBF->f4;}}_LL63: {struct Cyc_List_List*_tmp15F=*_tmpC3;
struct _RegionHandle*_tmp160=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*tqts=
_tmpC1->typs;struct Cyc_List_List*_tmp161=Cyc_Tcenv_lookup_type_vars(te);struct
_tuple10 _tmp473;struct _tuple10 _tmp162=(_tmp473.f1=_tmp161,((_tmp473.f2=_tmp160,
_tmp473)));struct Cyc_List_List*_tmp163=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp160,Cyc_Tcutil_r_make_inst_var,&
_tmp162,_tmpC0->tvs);struct Cyc_List_List*_tmp164=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple11*))Cyc_Core_snd,_tmp163);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmp479;struct Cyc_Absyn_DatatypeFieldInfo _tmp478;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp477;t=(void*)((_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477[0]=((_tmp479.tag=
4,((_tmp479.f1=((_tmp478.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1),((
_tmp478.targs=_tmp164,_tmp478)))),_tmp479)))),_tmp477))));}if(_tmpC4){int _tmp168=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp15F);int _tmp169=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp168 < _tmp169){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp169 - _tmp168;++ i){struct Cyc_List_List*_tmp47A;
wild_ps=((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp47A->tl=wild_ps,_tmp47A))))));}}*_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp15F,wild_ps);_tmp15F=*_tmpC3;}
else{if(_tmp168 == _tmp169){const char*_tmp47E;void*_tmp47D[1];struct Cyc_String_pa_struct
_tmp47C;(_tmp47C.tag=0,((_tmp47C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC0->name)),((_tmp47D[0]=& _tmp47C,Cyc_Tcutil_warn(p->loc,((
_tmp47E="unnecessary ... in datatype field %s",_tag_dyneither(_tmp47E,sizeof(
char),37))),_tag_dyneither(_tmp47D,sizeof(void*),1)))))));}}}for(0;_tmp15F != 0
 && tqts != 0;(_tmp15F=_tmp15F->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp16E=(
struct Cyc_Absyn_Pat*)_tmp15F->hd;void*_tmp16F=Cyc_Tcutil_rsubstitute(_tmp160,
_tmp163,(*((struct _tuple15*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp16E,(void**)& _tmp16F,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp16E->topt))->v,_tmp16F)){const char*_tmp484;void*_tmp483[3];
struct Cyc_String_pa_struct _tmp482;struct Cyc_String_pa_struct _tmp481;struct Cyc_String_pa_struct
_tmp480;(_tmp480.tag=0,((_tmp480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp16E->topt))->v)),((
_tmp481.tag=0,((_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp16F)),((_tmp482.tag=0,((_tmp482.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp483[0]=& _tmp482,((
_tmp483[1]=& _tmp481,((_tmp483[2]=& _tmp480,Cyc_Tcutil_terr(_tmp16E->loc,((_tmp484="%s expects argument type %s, not %s",
_tag_dyneither(_tmp484,sizeof(char),36))),_tag_dyneither(_tmp483,sizeof(void*),3)))))))))))))))))));}}
if(_tmp15F != 0){const char*_tmp488;void*_tmp487[1];struct Cyc_String_pa_struct
_tmp486;(_tmp486.tag=0,((_tmp486.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp487[0]=& _tmp486,Cyc_Tcutil_terr(p->loc,((
_tmp488="too many arguments for datatype constructor %s",_tag_dyneither(_tmp488,
sizeof(char),47))),_tag_dyneither(_tmp487,sizeof(void*),1)))))));}if(tqts != 0){
const char*_tmp48C;void*_tmp48B[1];struct Cyc_String_pa_struct _tmp48A;(_tmp48A.tag=
0,((_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC1->name)),((_tmp48B[0]=& _tmp48A,Cyc_Tcutil_terr(p->loc,((_tmp48C="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp48C,sizeof(char),46))),_tag_dyneither(_tmp48B,sizeof(void*),1)))))));}}
goto _LL43;}_LL64: {struct Cyc_Absyn_Aggr_p_struct*_tmpC5=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp95;if(_tmpC5->tag != 6)goto _LL66;else{_tmpC6=_tmpC5->f1;if(_tmpC6 != 0)goto
_LL66;}}_LL65: goto _LL67;_LL66: {struct Cyc_Absyn_Aggr_p_struct*_tmpC7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp95;if(_tmpC7->tag != 6)goto _LL68;else{_tmpC8=_tmpC7->f1;if(_tmpC8 == 0)goto
_LL68;_tmpC9=*_tmpC8;_tmpCA=_tmpC9.aggr_info;if((_tmpCA.UnknownAggr).tag != 1)
goto _LL68;_tmpCB=(struct _tuple3)(_tmpCA.UnknownAggr).val;}}_LL67: goto _LL69;_LL68: {
struct Cyc_Absyn_UnknownId_p_struct*_tmpCC=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp95;if(_tmpCC->tag != 14)goto _LL6A;}_LL69: goto _LL6B;_LL6A: {struct Cyc_Absyn_Exp_p_struct*
_tmpCD=(struct Cyc_Absyn_Exp_p_struct*)_tmp95;if(_tmpCD->tag != 16)goto _LL6C;}
_LL6B: goto _LL6D;_LL6C: {struct Cyc_Absyn_UnknownCall_p_struct*_tmpCE=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp95;if(_tmpCE->tag != 15)goto _LL43;}_LL6D: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL43;_LL43:;}tcpat_end: {struct Cyc_Core_Opt*_tmp48D;p->topt=((_tmp48D=
_cycalloc(sizeof(*_tmp48D)),((_tmp48D->v=t,_tmp48D))));}return res;};}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp17E=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct
_RegionHandle*_tmp17F=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp48E;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp17F,Cyc_Tcpat_get_name,_tmp17E.patvars),
p->loc,((_tmp48E="pattern contains a repeated variable",_tag_dyneither(_tmp48E,
sizeof(char),37))));}return _tmp17E;}void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p){void*_tmp181=p->r;struct Cyc_Absyn_Pat*_tmp183;struct
Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;
_LL96: {struct Cyc_Absyn_Pointer_p_struct*_tmp182=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp181;if(_tmp182->tag != 5)goto _LL98;else{_tmp183=_tmp182->f1;}}_LL97: Cyc_Tcpat_check_pat_regions(
te,_tmp183);{struct _RegionHandle*_tmp18A=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp18A,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp183->topt))->v)){{const char*
_tmp491;void*_tmp490;(_tmp490=0,Cyc_Tcutil_terr(p->loc,((_tmp491="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp491,sizeof(char),56))),_tag_dyneither(_tmp490,sizeof(void*),0)));}
return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp18A,(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){{const char*_tmp494;void*_tmp493;(_tmp493=0,Cyc_Tcutil_terr(
p->loc,((_tmp494="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp494,sizeof(char),53))),_tag_dyneither(_tmp493,sizeof(void*),0)));}
return;}{void*_tmp18F=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp191;struct Cyc_Absyn_PtrAtts _tmp192;void*_tmp193;_LLA1: {
struct Cyc_Absyn_PointerType_struct*_tmp190=(struct Cyc_Absyn_PointerType_struct*)
_tmp18F;if(_tmp190->tag != 5)goto _LLA3;else{_tmp191=_tmp190->f1;_tmp192=_tmp191.ptr_atts;
_tmp193=_tmp192.rgn;}}_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp193);
return;_LLA3:;_LLA4: {const char*_tmp497;void*_tmp496;(_tmp496=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp497="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp497,sizeof(char),36))),_tag_dyneither(_tmp496,sizeof(void*),0)));}
_LLA0:;};};_LL98: {struct Cyc_Absyn_Aggr_p_struct*_tmp184=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp181;if(_tmp184->tag != 6)goto _LL9A;else{_tmp185=_tmp184->f3;}}_LL99: for(0;
_tmp185 != 0;_tmp185=_tmp185->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple14*)_tmp185->hd)).f2);}return;_LL9A: {struct Cyc_Absyn_Datatype_p_struct*
_tmp186=(struct Cyc_Absyn_Datatype_p_struct*)_tmp181;if(_tmp186->tag != 7)goto
_LL9C;else{_tmp187=_tmp186->f3;}}_LL9B: _tmp189=_tmp187;goto _LL9D;_LL9C: {struct
Cyc_Absyn_Tuple_p_struct*_tmp188=(struct Cyc_Absyn_Tuple_p_struct*)_tmp181;if(
_tmp188->tag != 4)goto _LL9E;else{_tmp189=_tmp188->f1;}}_LL9D: for(0;_tmp189 != 0;
_tmp189=_tmp189->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp189->hd);}return;_LL9E:;_LL9F: return;_LL95:;}struct _union_Name_value_Name_v{
int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};
union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct
_union_Name_value_Int_v Int_v;};union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct
_dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp498;return((_tmp498.Name_v).val=s,(((
_tmp498.Name_v).tag=1,_tmp498)));}union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){
union Cyc_Tcpat_Name_value _tmp499;return((_tmp499.Int_v).val=i,(((_tmp499.Int_v).tag=
2,_tmp499)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Any_struct{int tag;};struct
Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp198=c1->name;struct _dyneither_ptr _tmp199;int _tmp19A;
_LLA6: if((_tmp198.Name_v).tag != 1)goto _LLA8;_tmp199=(struct _dyneither_ptr)(
_tmp198.Name_v).val;_LLA7: {union Cyc_Tcpat_Name_value _tmp19B=c2->name;struct
_dyneither_ptr _tmp19C;int _tmp19D;_LLAB: if((_tmp19B.Name_v).tag != 1)goto _LLAD;
_tmp19C=(struct _dyneither_ptr)(_tmp19B.Name_v).val;_LLAC: return Cyc_strcmp((
struct _dyneither_ptr)_tmp199,(struct _dyneither_ptr)_tmp19C);_LLAD: if((_tmp19B.Int_v).tag
!= 2)goto _LLAA;_tmp19D=(int)(_tmp19B.Int_v).val;_LLAE: return - 1;_LLAA:;}_LLA8: if((
_tmp198.Int_v).tag != 2)goto _LLA5;_tmp19A=(int)(_tmp198.Int_v).val;_LLA9: {union
Cyc_Tcpat_Name_value _tmp19E=c2->name;struct _dyneither_ptr _tmp19F;int _tmp1A0;
_LLB0: if((_tmp19E.Name_v).tag != 1)goto _LLB2;_tmp19F=(struct _dyneither_ptr)(
_tmp19E.Name_v).val;_LLB1: return 1;_LLB2: if((_tmp19E.Int_v).tag != 2)goto _LLAF;
_tmp1A0=(int)(_tmp19E.Int_v).val;_LLB3: return _tmp19A - _tmp1A0;_LLAF:;}_LLA5:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static int Cyc_Tcpat_one_opt=
1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=256;static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){const char*_tmp49C;struct Cyc_Tcpat_Con_s*_tmp49B;return(_tmp49B=_region_malloc(
r,sizeof(*_tmp49B)),((_tmp49B->name=Cyc_Tcpat_Name_v(((_tmp49C="NULL",
_tag_dyneither(_tmp49C,sizeof(char),5)))),((_tmp49B->arity=0,((_tmp49B->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp49B->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp49B)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){const char*_tmp49F;struct Cyc_Tcpat_Con_s*_tmp49E;return(_tmp49E=
_region_malloc(r,sizeof(*_tmp49E)),((_tmp49E->name=Cyc_Tcpat_Name_v(((_tmp49F="&",
_tag_dyneither(_tmp49F,sizeof(char),2)))),((_tmp49E->arity=1,((_tmp49E->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp49E->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp49E)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){const char*_tmp4A2;struct Cyc_Tcpat_Con_s*_tmp4A1;return(_tmp4A1=_region_malloc(
r,sizeof(*_tmp4A1)),((_tmp4A1->name=Cyc_Tcpat_Name_v(((_tmp4A2="&",
_tag_dyneither(_tmp4A2,sizeof(char),2)))),((_tmp4A1->arity=1,((_tmp4A1->span=(
int*)& Cyc_Tcpat_one_opt,((_tmp4A1->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4A1)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4A3;return(_tmp4A3=_region_malloc(r,
sizeof(*_tmp4A3)),((_tmp4A3->name=Cyc_Tcpat_Int_v(i),((_tmp4A3->arity=0,((
_tmp4A3->span=0,((_tmp4A3->orig_pat=p,_tmp4A3)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4A4;return(_tmp4A4=_region_malloc(r,sizeof(*_tmp4A4)),((
_tmp4A4->name=Cyc_Tcpat_Name_v(f),((_tmp4A4->arity=0,((_tmp4A4->span=0,((_tmp4A4->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp4A4)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4A5;
return(_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5)),((_tmp4A5->name=Cyc_Tcpat_Int_v((
int)c),((_tmp4A5->arity=0,((_tmp4A5->span=(int*)& Cyc_Tcpat_twofiftysix_opt,((
_tmp4A5->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4A5)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){const char*
_tmp4A8;struct Cyc_Tcpat_Con_s*_tmp4A7;return(_tmp4A7=_region_malloc(r,sizeof(*
_tmp4A7)),((_tmp4A7->name=Cyc_Tcpat_Name_v(((_tmp4A8="$",_tag_dyneither(_tmp4A8,
sizeof(char),2)))),((_tmp4A7->arity=i,((_tmp4A7->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp4A7->orig_pat=p,_tmp4A7)))))))));}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4AB;struct
Cyc_Tcpat_Con_struct*_tmp4AA;return(void*)((_tmp4AA=_region_malloc(r,sizeof(*
_tmp4AA)),((_tmp4AA[0]=((_tmp4AB.tag=1,((_tmp4AB.f1=Cyc_Tcpat_null_con(r,p),((
_tmp4AB.f2=0,_tmp4AB)))))),_tmp4AA))));}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4AE;
struct Cyc_Tcpat_Con_struct*_tmp4AD;return(void*)((_tmp4AD=_region_malloc(r,
sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AE.tag=1,((_tmp4AE.f1=Cyc_Tcpat_int_con(r,
i,p),((_tmp4AE.f2=0,_tmp4AE)))))),_tmp4AD))));}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4B1;struct Cyc_Tcpat_Con_struct*_tmp4B0;return(void*)((_tmp4B0=_region_malloc(
r,sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B1.tag=1,((_tmp4B1.f1=Cyc_Tcpat_char_con(
r,c,p),((_tmp4B1.f2=0,_tmp4B1)))))),_tmp4B0))));}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4B4;struct Cyc_Tcpat_Con_struct*_tmp4B3;return(void*)((_tmp4B3=_region_malloc(
r,sizeof(*_tmp4B3)),((_tmp4B3[0]=((_tmp4B4.tag=1,((_tmp4B4.f1=Cyc_Tcpat_float_con(
r,f,p),((_tmp4B4.f2=0,_tmp4B4)))))),_tmp4B3))));}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_struct
_tmp4BA;struct Cyc_List_List*_tmp4B9;struct Cyc_Tcpat_Con_struct*_tmp4B8;return(
void*)((_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8)),((_tmp4B8[0]=((_tmp4BA.tag=1,((
_tmp4BA.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4BA.f2=((_tmp4B9=_region_malloc(r,
sizeof(*_tmp4B9)),((_tmp4B9->hd=p,((_tmp4B9->tl=0,_tmp4B9)))))),_tmp4BA)))))),
_tmp4B8))));}static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){struct Cyc_Tcpat_Con_struct _tmp4C0;struct Cyc_List_List*_tmp4BF;struct Cyc_Tcpat_Con_struct*
_tmp4BE;return(void*)((_tmp4BE=_region_malloc(r,sizeof(*_tmp4BE)),((_tmp4BE[0]=((
_tmp4C0.tag=1,((_tmp4C0.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4C0.f2=((_tmp4BF=
_region_malloc(r,sizeof(*_tmp4BF)),((_tmp4BF->hd=p,((_tmp4BF->tl=0,_tmp4BF)))))),
_tmp4C0)))))),_tmp4BE))));}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4C3;
struct Cyc_Tcpat_Con_struct*_tmp4C2;return(void*)((_tmp4C2=_region_malloc(r,
sizeof(*_tmp4C2)),((_tmp4C2[0]=((_tmp4C3.tag=1,((_tmp4C3.f1=Cyc_Tcpat_tuple_con(
r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4C3.f2=ss,
_tmp4C3)))))),_tmp4C2))));}static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,
struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4C4;struct Cyc_Tcpat_Con_s*c=(_tmp4C4=_region_malloc(
r,sizeof(*_tmp4C4)),((_tmp4C4->name=Cyc_Tcpat_Name_v(con_name),((_tmp4C4->arity=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4C4->span=span,((
_tmp4C4->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C4)))))))));struct Cyc_Tcpat_Con_struct
_tmp4C7;struct Cyc_Tcpat_Con_struct*_tmp4C6;return(void*)((_tmp4C6=_region_malloc(
r,sizeof(*_tmp4C6)),((_tmp4C6[0]=((_tmp4C7.tag=1,((_tmp4C7.f1=c,((_tmp4C7.f2=ps,
_tmp4C7)))))),_tmp4C6))));}static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*
r,struct Cyc_Absyn_Pat*p){void*s;{void*_tmp1BF=p->r;enum Cyc_Absyn_Sign _tmp1C4;
int _tmp1C5;char _tmp1C7;struct _dyneither_ptr _tmp1C9;struct Cyc_Absyn_Pat*_tmp1CB;
struct Cyc_Absyn_Pat*_tmp1CD;struct Cyc_Absyn_Pat*_tmp1CF;struct Cyc_Absyn_Datatypedecl*
_tmp1D1;struct Cyc_Absyn_Datatypefield*_tmp1D2;struct Cyc_List_List*_tmp1D3;struct
Cyc_List_List*_tmp1D5;struct Cyc_Absyn_AggrInfo*_tmp1D7;struct Cyc_Absyn_AggrInfo
_tmp1D8;union Cyc_Absyn_AggrInfoU _tmp1D9;struct Cyc_Absyn_Aggrdecl**_tmp1DA;struct
Cyc_Absyn_Aggrdecl*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Enumdecl*
_tmp1DE;struct Cyc_Absyn_Enumfield*_tmp1DF;void*_tmp1E1;struct Cyc_Absyn_Enumfield*
_tmp1E2;_LLB5: {struct Cyc_Absyn_Wild_p_struct*_tmp1C0=(struct Cyc_Absyn_Wild_p_struct*)
_tmp1BF;if(_tmp1C0->tag != 0)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_TagInt_p_struct*
_tmp1C1=(struct Cyc_Absyn_TagInt_p_struct*)_tmp1BF;if(_tmp1C1->tag != 3)goto _LLB9;}
_LLB8:{struct Cyc_Tcpat_Any_struct _tmp4CA;struct Cyc_Tcpat_Any_struct*_tmp4C9;s=(
void*)((_tmp4C9=_region_malloc(r,sizeof(*_tmp4C9)),((_tmp4C9[0]=((_tmp4CA.tag=0,
_tmp4CA)),_tmp4C9))));}goto _LLB4;_LLB9: {struct Cyc_Absyn_Null_p_struct*_tmp1C2=(
struct Cyc_Absyn_Null_p_struct*)_tmp1BF;if(_tmp1C2->tag != 8)goto _LLBB;}_LLBA: s=
Cyc_Tcpat_null_pat(r,p);goto _LLB4;_LLBB: {struct Cyc_Absyn_Int_p_struct*_tmp1C3=(
struct Cyc_Absyn_Int_p_struct*)_tmp1BF;if(_tmp1C3->tag != 9)goto _LLBD;else{_tmp1C4=
_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;}}_LLBC: s=Cyc_Tcpat_int_pat(r,_tmp1C5,(struct Cyc_Absyn_Pat*)
p);goto _LLB4;_LLBD: {struct Cyc_Absyn_Char_p_struct*_tmp1C6=(struct Cyc_Absyn_Char_p_struct*)
_tmp1BF;if(_tmp1C6->tag != 10)goto _LLBF;else{_tmp1C7=_tmp1C6->f1;}}_LLBE: s=Cyc_Tcpat_char_pat(
r,_tmp1C7,p);goto _LLB4;_LLBF: {struct Cyc_Absyn_Float_p_struct*_tmp1C8=(struct Cyc_Absyn_Float_p_struct*)
_tmp1BF;if(_tmp1C8->tag != 11)goto _LLC1;else{_tmp1C9=_tmp1C8->f1;}}_LLC0: s=Cyc_Tcpat_float_pat(
r,_tmp1C9,p);goto _LLB4;_LLC1: {struct Cyc_Absyn_Var_p_struct*_tmp1CA=(struct Cyc_Absyn_Var_p_struct*)
_tmp1BF;if(_tmp1CA->tag != 1)goto _LLC3;else{_tmp1CB=_tmp1CA->f2;}}_LLC2: s=Cyc_Tcpat_compile_pat(
r,_tmp1CB);goto _LLB4;_LLC3: {struct Cyc_Absyn_Reference_p_struct*_tmp1CC=(struct
Cyc_Absyn_Reference_p_struct*)_tmp1BF;if(_tmp1CC->tag != 2)goto _LLC5;else{_tmp1CD=
_tmp1CC->f2;}}_LLC4: s=Cyc_Tcpat_compile_pat(r,_tmp1CD);goto _LLB4;_LLC5: {struct
Cyc_Absyn_Pointer_p_struct*_tmp1CE=(struct Cyc_Absyn_Pointer_p_struct*)_tmp1BF;
if(_tmp1CE->tag != 5)goto _LLC7;else{_tmp1CF=_tmp1CE->f1;}}_LLC6:{void*_tmp1E9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1EB;struct Cyc_Absyn_PtrAtts _tmp1EC;union Cyc_Absyn_Constraint*_tmp1ED;_LLDA: {
struct Cyc_Absyn_PointerType_struct*_tmp1EA=(struct Cyc_Absyn_PointerType_struct*)
_tmp1E9;if(_tmp1EA->tag != 5)goto _LLDC;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EB.ptr_atts;
_tmp1ED=_tmp1EC.nullable;}}_LLDB: {int is_nullable=0;int still_working=1;while(
still_working){union Cyc_Absyn_Constraint*_tmp1EE=_tmp1ED;union Cyc_Absyn_Constraint
_tmp1EF;union Cyc_Absyn_Constraint*_tmp1F0;union Cyc_Absyn_Constraint _tmp1F1;int
_tmp1F2;union Cyc_Absyn_Constraint _tmp1F3;int _tmp1F4;_LLDF: _tmp1EF=*_tmp1EE;if((
_tmp1EF.Forward_constr).tag != 2)goto _LLE1;_tmp1F0=(union Cyc_Absyn_Constraint*)(
_tmp1EF.Forward_constr).val;_LLE0:*_tmp1ED=*_tmp1F0;continue;_LLE1: _tmp1F1=*
_tmp1EE;if((_tmp1F1.No_constr).tag != 3)goto _LLE3;_tmp1F2=(int)(_tmp1F1.No_constr).val;
_LLE2:{struct _union_Constraint_Eq_constr*_tmp4CB;(_tmp4CB=& _tmp1ED->Eq_constr,((
_tmp4CB->tag=1,_tmp4CB->val=0)));}is_nullable=0;still_working=0;goto _LLDE;_LLE3:
_tmp1F3=*_tmp1EE;if((_tmp1F3.Eq_constr).tag != 1)goto _LLDE;_tmp1F4=(int)(_tmp1F3.Eq_constr).val;
_LLE4: is_nullable=(int)_tmp1F4;still_working=0;goto _LLDE;_LLDE:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp1CF);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLD9;};}_LLDC:;_LLDD: {const char*_tmp4CE;void*_tmp4CD;(_tmp4CD=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4CE="expecting pointertype for pattern!",_tag_dyneither(_tmp4CE,sizeof(char),
35))),_tag_dyneither(_tmp4CD,sizeof(void*),0)));}_LLD9:;}goto _LLB4;_LLC7: {
struct Cyc_Absyn_Datatype_p_struct*_tmp1D0=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp1BF;if(_tmp1D0->tag != 7)goto _LLC9;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;
_tmp1D3=_tmp1D0->f3;}}_LLC8: {int*span;{void*_tmp1F8=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLE6: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1F9=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1F8;if(_tmp1F9->tag != 3)goto
_LLE8;}_LLE7: if(_tmp1D1->is_extensible)span=0;else{int*_tmp4CF;span=((_tmp4CF=
_region_malloc(r,sizeof(*_tmp4CF)),((_tmp4CF[0]=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D1->fields))->v),
_tmp4CF))));}goto _LLE5;_LLE8: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1FA=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1F8;if(_tmp1FA->tag != 4)goto _LLEA;}
_LLE9: span=(int*)& Cyc_Tcpat_one_opt;goto _LLE5;_LLEA:;_LLEB:{const char*_tmp4D2;
void*_tmp4D1;span=((_tmp4D1=0,((int*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D2="void datatype pattern has bad type",
_tag_dyneither(_tmp4D2,sizeof(char),35))),_tag_dyneither(_tmp4D1,sizeof(void*),0))));}
goto _LLE5;_LLE5:;}s=Cyc_Tcpat_con_pat(r,*(*_tmp1D2->name).f2,span,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D3),p);goto _LLB4;}_LLC9: {struct Cyc_Absyn_Tuple_p_struct*_tmp1D4=(struct
Cyc_Absyn_Tuple_p_struct*)_tmp1BF;if(_tmp1D4->tag != 4)goto _LLCB;else{_tmp1D5=
_tmp1D4->f1;}}_LLCA: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D5),(struct Cyc_Absyn_Pat*)p);goto _LLB4;_LLCB: {struct Cyc_Absyn_Aggr_p_struct*
_tmp1D6=(struct Cyc_Absyn_Aggr_p_struct*)_tmp1BF;if(_tmp1D6->tag != 6)goto _LLCD;
else{_tmp1D7=_tmp1D6->f1;if(_tmp1D7 == 0)goto _LLCD;_tmp1D8=*_tmp1D7;_tmp1D9=
_tmp1D8.aggr_info;if((_tmp1D9.KnownAggr).tag != 2)goto _LLCD;_tmp1DA=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1D9.KnownAggr).val;_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1D6->f3;}}_LLCC: if(_tmp1DB->kind
== Cyc_Absyn_StructA){struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DB->impl))->fields;for(0;fields != 
0;fields=fields->tl){const char*_tmp4D3;int found=Cyc_strcmp((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4D3="",_tag_dyneither(_tmp4D3,
sizeof(char),1))))== 0;{struct Cyc_List_List*dlps0=_tmp1DC;for(0;!found  && dlps0
!= 0;dlps0=dlps0->tl){struct _tuple14 _tmp1FF;struct Cyc_List_List*_tmp200;struct
Cyc_Absyn_Pat*_tmp201;struct _tuple14*_tmp1FE=(struct _tuple14*)dlps0->hd;_tmp1FF=*
_tmp1FE;_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;{struct Cyc_List_List*_tmp202=
_tmp200;struct Cyc_List_List _tmp203;void*_tmp204;struct _dyneither_ptr*_tmp206;
struct Cyc_List_List*_tmp207;_LLED: if(_tmp202 == 0)goto _LLEF;_tmp203=*_tmp202;
_tmp204=(void*)_tmp203.hd;{struct Cyc_Absyn_FieldName_struct*_tmp205=(struct Cyc_Absyn_FieldName_struct*)
_tmp204;if(_tmp205->tag != 1)goto _LLEF;else{_tmp206=_tmp205->f1;}};_tmp207=
_tmp203.tl;if(_tmp207 != 0)goto _LLEF;_LLEE: if(Cyc_strptrcmp(_tmp206,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){{struct Cyc_List_List*_tmp4D4;ps=((_tmp4D4=_region_malloc(
r,sizeof(*_tmp4D4)),((_tmp4D4->hd=Cyc_Tcpat_compile_pat(r,_tmp201),((_tmp4D4->tl=
ps,_tmp4D4))))));}found=1;}goto _LLEC;_LLEF:;_LLF0: {const char*_tmp4D7;void*
_tmp4D6;(_tmp4D6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4D7="bad designator(s)",_tag_dyneither(_tmp4D7,sizeof(char),18))),
_tag_dyneither(_tmp4D6,sizeof(void*),0)));}_LLEC:;};}}if(!found){const char*
_tmp4DA;void*_tmp4D9;(_tmp4D9=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DA="bad designator",_tag_dyneither(
_tmp4DA,sizeof(char),15))),_tag_dyneither(_tmp4D9,sizeof(void*),0)));}}}s=Cyc_Tcpat_tuple_pat(
r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DB->impl))->tagged){const char*_tmp4DD;void*_tmp4DC;(_tmp4DC=0,
Cyc_Tcutil_terr(p->loc,((_tmp4DD="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp4DD,sizeof(char),47))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}{
int*_tmp4DE;int*span=(_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DB->impl))->fields),_tmp4DE)));int field_name;struct Cyc_List_List*
_tmp210=_tmp1DC;struct Cyc_List_List _tmp211;struct _tuple14*_tmp212;struct _tuple14
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List _tmp215;void*_tmp216;
struct _dyneither_ptr*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_Absyn_Pat*
_tmp21A;struct Cyc_List_List*_tmp21B;_LLF2: if(_tmp210 == 0)goto _LLF4;_tmp211=*
_tmp210;_tmp212=(struct _tuple14*)_tmp211.hd;_tmp213=*_tmp212;_tmp214=_tmp213.f1;
if(_tmp214 == 0)goto _LLF4;_tmp215=*_tmp214;_tmp216=(void*)_tmp215.hd;{struct Cyc_Absyn_FieldName_struct*
_tmp217=(struct Cyc_Absyn_FieldName_struct*)_tmp216;if(_tmp217->tag != 1)goto _LLF4;
else{_tmp218=_tmp217->f1;}};_tmp219=_tmp215.tl;if(_tmp219 != 0)goto _LLF4;_tmp21A=
_tmp213.f2;_tmp21B=_tmp211.tl;if(_tmp21B != 0)goto _LLF4;_LLF3:{struct Cyc_List_List*
_tmp4DF;s=Cyc_Tcpat_con_pat(r,*_tmp218,span,((_tmp4DF=_region_malloc(r,sizeof(*
_tmp4DF)),((_tmp4DF->hd=Cyc_Tcpat_compile_pat(r,_tmp21A),((_tmp4DF->tl=0,_tmp4DF)))))),
p);}goto _LLF1;_LLF4:;_LLF5: {const char*_tmp4E2;void*_tmp4E1;(_tmp4E1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E2="bad union pattern",
_tag_dyneither(_tmp4E2,sizeof(char),18))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}
_LLF1:;};}goto _LLB4;_LLCD: {struct Cyc_Absyn_Enum_p_struct*_tmp1DD=(struct Cyc_Absyn_Enum_p_struct*)
_tmp1BF;if(_tmp1DD->tag != 12)goto _LLCF;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}
_LLCE: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1DE->fields))->v);{int*_tmp4E3;s=Cyc_Tcpat_con_pat(
r,*(*_tmp1DF->name).f2,((_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3)),((_tmp4E3[0]=
span,_tmp4E3)))),0,p);}goto _LLB4;}_LLCF: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E0=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1BF;if(_tmp1E0->tag != 13)goto
_LLD1;else{_tmp1E1=(void*)_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LLD0: {struct Cyc_List_List*
fields;{void*_tmp221=Cyc_Tcutil_compress(_tmp1E1);struct Cyc_List_List*_tmp223;
_LLF7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp222=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp221;if(_tmp222->tag != 15)goto _LLF9;else{_tmp223=_tmp222->f1;}}_LLF8: fields=
_tmp223;goto _LLF6;_LLF9:;_LLFA: {const char*_tmp4E6;void*_tmp4E5;(_tmp4E5=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E6="bad type in AnonEnum_p",
_tag_dyneither(_tmp4E6,sizeof(char),23))),_tag_dyneither(_tmp4E5,sizeof(void*),0)));}
_LLF6:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);{int*
_tmp4E7;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E2->name).f2,((_tmp4E7=_region_malloc(r,
sizeof(*_tmp4E7)),((_tmp4E7[0]=span,_tmp4E7)))),0,p);}goto _LLB4;};}_LLD1: {
struct Cyc_Absyn_UnknownId_p_struct*_tmp1E3=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1BF;if(_tmp1E3->tag != 14)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp1E4=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1BF;if(_tmp1E4->tag != 15)goto
_LLD5;}_LLD4: goto _LLD6;_LLD5: {struct Cyc_Absyn_Aggr_p_struct*_tmp1E5=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp1BF;if(_tmp1E5->tag != 6)goto _LLD7;}_LLD6: goto _LLD8;_LLD7: {struct Cyc_Absyn_Exp_p_struct*
_tmp1E6=(struct Cyc_Absyn_Exp_p_struct*)_tmp1BF;if(_tmp1E6->tag != 16)goto _LLB4;}
_LLD8: {struct Cyc_Tcpat_Any_struct _tmp4EA;struct Cyc_Tcpat_Any_struct*_tmp4E9;s=(
void*)((_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9)),((_tmp4E9[0]=((_tmp4EA.tag=0,
_tmp4EA)),_tmp4E9))));}_LLB4:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;
struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes
 = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};static void*Cyc_Tcpat_add_neg(struct
_RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp229=td;struct Cyc_Set_Set*
_tmp22B;_LLFC: {struct Cyc_Tcpat_Neg_struct*_tmp22A=(struct Cyc_Tcpat_Neg_struct*)
_tmp229;if(_tmp22A->tag != 1)goto _LLFE;else{_tmp22B=_tmp22A->f1;}}_LLFD: {struct
Cyc_Tcpat_Neg_struct _tmp4ED;struct Cyc_Tcpat_Neg_struct*_tmp4EC;return(void*)((
_tmp4EC=_region_malloc(r,sizeof(*_tmp4EC)),((_tmp4EC[0]=((_tmp4ED.tag=1,((
_tmp4ED.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp22B,c),_tmp4ED)))),_tmp4EC))));}
_LLFE: {struct Cyc_Tcpat_Pos_struct*_tmp22C=(struct Cyc_Tcpat_Pos_struct*)_tmp229;
if(_tmp22C->tag != 0)goto _LLFB;}_LLFF: {const char*_tmp4F0;void*_tmp4EF;(_tmp4EF=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4F0="add_neg called when td is Positive",_tag_dyneither(_tmp4F0,sizeof(char),
35))),_tag_dyneither(_tmp4EF,sizeof(void*),0)));}_LLFB:;}static enum Cyc_Tcpat_Answer
Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp231=td;struct
Cyc_Tcpat_Con_s*_tmp233;struct Cyc_Set_Set*_tmp235;_LL101: {struct Cyc_Tcpat_Pos_struct*
_tmp232=(struct Cyc_Tcpat_Pos_struct*)_tmp231;if(_tmp232->tag != 0)goto _LL103;
else{_tmp233=_tmp232->f1;}}_LL102: if(Cyc_Tcpat_compare_con(c,_tmp233)== 0)return
Cyc_Tcpat_Yes;else{return Cyc_Tcpat_No;}_LL103: {struct Cyc_Tcpat_Neg_struct*
_tmp234=(struct Cyc_Tcpat_Neg_struct*)_tmp231;if(_tmp234->tag != 1)goto _LL100;
else{_tmp235=_tmp234->f1;}}_LL104: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp235,c))return Cyc_Tcpat_No;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp235)+ 1)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_Maybe;}}_LL100:;}struct
_tuple18{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp236=ctxt;struct Cyc_List_List _tmp237;struct _tuple18*
_tmp238;struct _tuple18 _tmp239;struct Cyc_Tcpat_Con_s*_tmp23A;struct Cyc_List_List*
_tmp23B;struct Cyc_List_List*_tmp23C;_LL106: if(_tmp236 != 0)goto _LL108;_LL107:
return 0;_LL108: if(_tmp236 == 0)goto _LL105;_tmp237=*_tmp236;_tmp238=(struct
_tuple18*)_tmp237.hd;_tmp239=*_tmp238;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;
_tmp23C=_tmp237.tl;_LL109: {struct _tuple18*_tmp4F6;struct Cyc_List_List*_tmp4F5;
struct Cyc_List_List*_tmp4F4;return(_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4)),((
_tmp4F4->hd=((_tmp4F6=_region_malloc(r,sizeof(*_tmp4F6)),((_tmp4F6->f1=_tmp23A,((
_tmp4F6->f2=(struct Cyc_List_List*)((_tmp4F5=_region_malloc(r,sizeof(*_tmp4F5)),((
_tmp4F5->hd=dsc,((_tmp4F5->tl=_tmp23B,_tmp4F5)))))),_tmp4F6)))))),((_tmp4F4->tl=
_tmp23C,_tmp4F4)))));}_LL105:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp240=ctxt;
struct Cyc_List_List _tmp241;struct _tuple18*_tmp242;struct _tuple18 _tmp243;struct
Cyc_Tcpat_Con_s*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_List_List*_tmp246;
_LL10B: if(_tmp240 != 0)goto _LL10D;_LL10C: {const char*_tmp4F9;void*_tmp4F8;(
_tmp4F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4F9="norm_context: empty context",_tag_dyneither(_tmp4F9,sizeof(char),28))),
_tag_dyneither(_tmp4F8,sizeof(void*),0)));}_LL10D: if(_tmp240 == 0)goto _LL10A;
_tmp241=*_tmp240;_tmp242=(struct _tuple18*)_tmp241.hd;_tmp243=*_tmp242;_tmp244=
_tmp243.f1;_tmp245=_tmp243.f2;_tmp246=_tmp241.tl;_LL10E: {struct Cyc_Tcpat_Pos_struct
_tmp4FC;struct Cyc_Tcpat_Pos_struct*_tmp4FB;return Cyc_Tcpat_augment(r,_tmp246,(
void*)((_tmp4FB=_region_malloc(r,sizeof(*_tmp4FB)),((_tmp4FB[0]=((_tmp4FC.tag=0,((
_tmp4FC.f1=_tmp244,((_tmp4FC.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp245),_tmp4FC)))))),_tmp4FB)))));}
_LL10A:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple12 _tmp4FD;struct _tuple12
_tmp24C=(_tmp4FD.f1=ctxt,((_tmp4FD.f2=work,_tmp4FD)));struct Cyc_List_List*
_tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*
_tmp250;struct Cyc_List_List*_tmp251;struct Cyc_List_List _tmp252;struct _tuple18*
_tmp253;struct _tuple18 _tmp254;struct Cyc_Tcpat_Con_s*_tmp255;struct Cyc_List_List*
_tmp256;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp258;struct Cyc_List_List
_tmp259;struct _tuple17*_tmp25A;struct _tuple17 _tmp25B;struct Cyc_List_List*_tmp25C;
struct Cyc_List_List*_tmp25D;_LL110: _tmp24D=_tmp24C.f1;if(_tmp24D != 0)goto _LL112;
_tmp24E=_tmp24C.f2;if(_tmp24E != 0)goto _LL112;_LL111: return dsc;_LL112: _tmp24F=
_tmp24C.f1;if(_tmp24F != 0)goto _LL114;_LL113: goto _LL115;_LL114: _tmp250=_tmp24C.f2;
if(_tmp250 != 0)goto _LL116;_LL115: {const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp500="build_desc: ctxt and work don't match",_tag_dyneither(_tmp500,sizeof(
char),38))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}_LL116: _tmp251=_tmp24C.f1;
if(_tmp251 == 0)goto _LL10F;_tmp252=*_tmp251;_tmp253=(struct _tuple18*)_tmp252.hd;
_tmp254=*_tmp253;_tmp255=_tmp254.f1;_tmp256=_tmp254.f2;_tmp257=_tmp252.tl;
_tmp258=_tmp24C.f2;if(_tmp258 == 0)goto _LL10F;_tmp259=*_tmp258;_tmp25A=(struct
_tuple17*)_tmp259.hd;_tmp25B=*_tmp25A;_tmp25C=_tmp25B.f3;_tmp25D=_tmp259.tl;
_LL117: {struct Cyc_Tcpat_Pos_struct _tmp506;struct Cyc_List_List*_tmp505;struct Cyc_Tcpat_Pos_struct*
_tmp504;struct Cyc_Tcpat_Pos_struct*_tmp260=(_tmp504=_region_malloc(r,sizeof(*
_tmp504)),((_tmp504[0]=((_tmp506.tag=0,((_tmp506.f1=_tmp255,((_tmp506.f2=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,_tmp256),((_tmp505=_region_malloc(r,sizeof(*_tmp505)),((
_tmp505->hd=dsc,((_tmp505->tl=_tmp25C,_tmp505))))))),_tmp506)))))),_tmp504)));
return Cyc_Tcpat_build_desc(r,_tmp257,(void*)_tmp260,_tmp25D);}_LL10F:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp264=allmrules;struct
Cyc_List_List _tmp265;struct _tuple0*_tmp266;struct _tuple0 _tmp267;void*_tmp268;
void*_tmp269;struct Cyc_List_List*_tmp26A;_LL119: if(_tmp264 != 0)goto _LL11B;_LL11A: {
struct Cyc_Tcpat_Failure_struct _tmp509;struct Cyc_Tcpat_Failure_struct*_tmp508;
return(void*)((_tmp508=_cycalloc(sizeof(*_tmp508)),((_tmp508[0]=((_tmp509.tag=0,((
_tmp509.f1=(void*)dsc,_tmp509)))),_tmp508))));}_LL11B: if(_tmp264 == 0)goto _LL118;
_tmp265=*_tmp264;_tmp266=(struct _tuple0*)_tmp265.hd;_tmp267=*_tmp266;_tmp268=
_tmp267.f1;_tmp269=_tmp267.f2;_tmp26A=_tmp265.tl;_LL11C: return Cyc_Tcpat_match(r,
_tmp268,0,dsc,0,0,_tmp269,_tmp26A);_LL118:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_struct
_tmp50C;struct Cyc_Tcpat_Neg_struct*_tmp50B;return Cyc_Tcpat_or_match(r,(void*)((
_tmp50B=_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B[0]=((_tmp50C.tag=1,((
_tmp50C.f1=Cyc_Tcpat_empty_con_set(r),_tmp50C)))),_tmp50B)))),allmrules);}static
void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp26F=
work;struct Cyc_List_List _tmp270;struct _tuple17*_tmp271;struct _tuple17 _tmp272;
struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp274;struct Cyc_List_List*
_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List _tmp277;struct _tuple17*
_tmp278;struct _tuple17 _tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*
_tmp27B;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*_tmp27D;_LL11E: if(
_tmp26F != 0)goto _LL120;_LL11F: {struct Cyc_Tcpat_Success_struct _tmp50F;struct Cyc_Tcpat_Success_struct*
_tmp50E;return(void*)((_tmp50E=_region_malloc(r,sizeof(*_tmp50E)),((_tmp50E[0]=((
_tmp50F.tag=1,((_tmp50F.f1=(void*)right_hand_side,_tmp50F)))),_tmp50E))));}
_LL120: if(_tmp26F == 0)goto _LL122;_tmp270=*_tmp26F;_tmp271=(struct _tuple17*)
_tmp270.hd;_tmp272=*_tmp271;_tmp273=_tmp272.f1;if(_tmp273 != 0)goto _LL122;_tmp274=
_tmp272.f2;if(_tmp274 != 0)goto _LL122;_tmp275=_tmp272.f3;if(_tmp275 != 0)goto
_LL122;_tmp276=_tmp270.tl;_LL121: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp276,right_hand_side,rules);_LL122: if(_tmp26F == 0)goto _LL11D;_tmp277=*
_tmp26F;_tmp278=(struct _tuple17*)_tmp277.hd;_tmp279=*_tmp278;_tmp27A=_tmp279.f1;
_tmp27B=_tmp279.f2;_tmp27C=_tmp279.f3;_tmp27D=_tmp277.tl;_LL123: if((_tmp27A == 0
 || _tmp27B == 0) || _tmp27C == 0){const char*_tmp512;void*_tmp511;(_tmp511=0,Cyc_Tcutil_impos(((
_tmp512="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp512,sizeof(
char),38))),_tag_dyneither(_tmp511,sizeof(void*),0)));}{struct Cyc_List_List
_tmp283;void*_tmp284;struct Cyc_List_List*_tmp285;struct Cyc_List_List*_tmp282=(
struct Cyc_List_List*)_tmp27A;_tmp283=*_tmp282;_tmp284=(void*)_tmp283.hd;_tmp285=
_tmp283.tl;{struct Cyc_List_List _tmp287;struct Cyc_List_List*_tmp288;struct Cyc_List_List*
_tmp289;struct Cyc_List_List*_tmp286=(struct Cyc_List_List*)_tmp27B;_tmp287=*
_tmp286;_tmp288=(struct Cyc_List_List*)_tmp287.hd;_tmp289=_tmp287.tl;{struct Cyc_List_List
_tmp28B;void*_tmp28C;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*_tmp28A=(
struct Cyc_List_List*)_tmp27C;_tmp28B=*_tmp28A;_tmp28C=(void*)_tmp28B.hd;_tmp28D=
_tmp28B.tl;{struct _tuple17*_tmp513;struct _tuple17*_tmp28E=(_tmp513=
_region_malloc(r,sizeof(*_tmp513)),((_tmp513->f1=_tmp285,((_tmp513->f2=_tmp289,((
_tmp513->f3=_tmp28D,_tmp513)))))));struct Cyc_List_List*_tmp514;return Cyc_Tcpat_match(
r,_tmp284,_tmp288,_tmp28C,ctx,((_tmp514=_region_malloc(r,sizeof(*_tmp514)),((
_tmp514->hd=_tmp28E,((_tmp514->tl=_tmp27D,_tmp514)))))),right_hand_side,rules);};};};};
_LL11D:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp291=dsc;struct Cyc_Set_Set*_tmp293;
struct Cyc_List_List*_tmp295;_LL125: {struct Cyc_Tcpat_Neg_struct*_tmp292=(struct
Cyc_Tcpat_Neg_struct*)_tmp291;if(_tmp292->tag != 1)goto _LL127;else{_tmp293=
_tmp292->f1;}}_LL126: {struct Cyc_Tcpat_Neg_struct _tmp517;struct Cyc_Tcpat_Neg_struct*
_tmp516;void*any=(void*)((_tmp516=_region_malloc(r,sizeof(*_tmp516)),((_tmp516[0]=((
_tmp517.tag=1,((_tmp517.f1=Cyc_Tcpat_empty_con_set(r),_tmp517)))),_tmp516))));
struct Cyc_List_List*_tmp296=0;{int i=0;for(0;i < pcon->arity;++ i){struct Cyc_List_List*
_tmp518;_tmp296=((_tmp518=_region_malloc(r,sizeof(*_tmp518)),((_tmp518->hd=any,((
_tmp518->tl=_tmp296,_tmp518))))));}}return _tmp296;}_LL127: {struct Cyc_Tcpat_Pos_struct*
_tmp294=(struct Cyc_Tcpat_Pos_struct*)_tmp291;if(_tmp294->tag != 0)goto _LL124;
else{_tmp295=_tmp294->f2;}}_LL128: return _tmp295;_LL124:;}struct _tuple19{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(
struct _tuple19*env,int i){struct _tuple19 _tmp29B;struct _RegionHandle*_tmp29C;
struct Cyc_List_List*_tmp29D;struct _tuple19*_tmp29A=env;_tmp29B=*_tmp29A;_tmp29C=
_tmp29B.f1;_tmp29D=_tmp29B.f2;{struct Cyc_List_List*_tmp519;return(_tmp519=
_region_malloc(_tmp29C,sizeof(*_tmp519)),((_tmp519->hd=(void*)(i + 1),((_tmp519->tl=
_tmp29D,_tmp519)))));};}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct
_RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple19 _tmp51A;struct _tuple19 _tmp29F=(_tmp51A.f1=r,((_tmp51A.f2=obj,_tmp51A)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple19*,int),struct _tuple19*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp29F);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp2A1=p;struct Cyc_Tcpat_Con_s*
_tmp2A4;struct Cyc_List_List*_tmp2A5;_LL12A: {struct Cyc_Tcpat_Any_struct*_tmp2A2=(
struct Cyc_Tcpat_Any_struct*)_tmp2A1;if(_tmp2A2->tag != 0)goto _LL12C;}_LL12B:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL12C: {struct Cyc_Tcpat_Con_struct*_tmp2A3=(struct Cyc_Tcpat_Con_struct*)
_tmp2A1;if(_tmp2A3->tag != 1)goto _LL129;else{_tmp2A4=_tmp2A3->f1;_tmp2A5=_tmp2A3->f2;}}
_LL12D: switch(Cyc_Tcpat_static_match(_tmp2A4,dsc)){case Cyc_Tcpat_Yes: _LL12E: {
struct _tuple18*_tmp51D;struct Cyc_List_List*_tmp51C;struct Cyc_List_List*ctx2=(
_tmp51C=_region_malloc(r,sizeof(*_tmp51C)),((_tmp51C->hd=((_tmp51D=
_region_malloc(r,sizeof(*_tmp51D)),((_tmp51D->f1=_tmp2A4,((_tmp51D->f2=0,_tmp51D)))))),((
_tmp51C->tl=ctx,_tmp51C)))));struct _tuple17*_tmp51E;struct _tuple17*work_frame=(
_tmp51E=_region_malloc(r,sizeof(*_tmp51E)),((_tmp51E->f1=_tmp2A5,((_tmp51E->f2=
Cyc_Tcpat_getoargs(r,_tmp2A4,obj),((_tmp51E->f3=Cyc_Tcpat_getdargs(r,_tmp2A4,dsc),
_tmp51E)))))));struct Cyc_List_List*_tmp51F;struct Cyc_List_List*work2=(_tmp51F=
_region_malloc(r,sizeof(*_tmp51F)),((_tmp51F->hd=work_frame,((_tmp51F->tl=work,
_tmp51F)))));return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case
Cyc_Tcpat_No: _LL12F: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,
work),rules);case Cyc_Tcpat_Maybe: _LL130: {struct _tuple18*_tmp522;struct Cyc_List_List*
_tmp521;struct Cyc_List_List*ctx2=(_tmp521=_region_malloc(r,sizeof(*_tmp521)),((
_tmp521->hd=((_tmp522=_region_malloc(r,sizeof(*_tmp522)),((_tmp522->f1=_tmp2A4,((
_tmp522->f2=0,_tmp522)))))),((_tmp521->tl=ctx,_tmp521)))));struct _tuple17*
_tmp523;struct _tuple17*work_frame=(_tmp523=_region_malloc(r,sizeof(*_tmp523)),((
_tmp523->f1=_tmp2A5,((_tmp523->f2=Cyc_Tcpat_getoargs(r,_tmp2A4,obj),((_tmp523->f3=
Cyc_Tcpat_getdargs(r,_tmp2A4,dsc),_tmp523)))))));struct Cyc_List_List*_tmp524;
struct Cyc_List_List*work2=(_tmp524=_region_malloc(r,sizeof(*_tmp524)),((_tmp524->hd=
work_frame,((_tmp524->tl=work,_tmp524)))));void*_tmp2AA=Cyc_Tcpat_and_match(r,
ctx2,work2,right_hand_side,rules);void*_tmp2AB=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2A4),work),rules);struct Cyc_Tcpat_IfEq_struct
_tmp527;struct Cyc_Tcpat_IfEq_struct*_tmp526;return(void*)((_tmp526=
_region_malloc(r,sizeof(*_tmp526)),((_tmp526[0]=((_tmp527.tag=2,((_tmp527.f1=obj,((
_tmp527.f2=_tmp2A4,((_tmp527.f3=(void*)_tmp2AA,((_tmp527.f4=(void*)_tmp2AB,
_tmp527)))))))))),_tmp526))));}}_LL129:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp2B2=d;void*_tmp2B4;void*_tmp2B6;void*_tmp2B8;void*_tmp2B9;_LL133: {struct Cyc_Tcpat_Failure_struct*
_tmp2B3=(struct Cyc_Tcpat_Failure_struct*)_tmp2B2;if(_tmp2B3->tag != 0)goto _LL135;
else{_tmp2B4=(void*)_tmp2B3->f1;}}_LL134: if(!(*exhaust_done)){not_exhaust(r,env1,
_tmp2B4);*exhaust_done=1;}goto _LL132;_LL135: {struct Cyc_Tcpat_Success_struct*
_tmp2B5=(struct Cyc_Tcpat_Success_struct*)_tmp2B2;if(_tmp2B5->tag != 1)goto _LL137;
else{_tmp2B6=(void*)_tmp2B5->f1;}}_LL136: rhs_appears(env2,_tmp2B6);goto _LL132;
_LL137: {struct Cyc_Tcpat_IfEq_struct*_tmp2B7=(struct Cyc_Tcpat_IfEq_struct*)
_tmp2B2;if(_tmp2B7->tag != 2)goto _LL132;else{_tmp2B8=(void*)_tmp2B7->f3;_tmp2B9=(
void*)_tmp2B7->f4;}}_LL138: Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B8,not_exhaust,
env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B9,
not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL132;_LL132:;}struct
_tuple20{int f1;struct Cyc_Position_Segment*f2;};struct _tuple21{void*f1;struct
_tuple20*f2;};struct _tuple22{struct _RegionHandle*f1;int*f2;};static struct
_tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple22 _tmp2BB;struct _RegionHandle*_tmp2BC;int*_tmp2BD;struct
_tuple22*_tmp2BA=env;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.f1;_tmp2BD=_tmp2BB.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2BC,swc->pattern);struct _tuple20*_tmp528;
struct _tuple20*rhs=(_tmp528=_region_malloc(_tmp2BC,sizeof(*_tmp528)),((_tmp528->f1=
0,((_tmp528->f2=(swc->pattern)->loc,_tmp528)))));void*sp;if(swc->where_clause != 
0){{struct Cyc_List_List*_tmp52B;struct Cyc_List_List*_tmp52A;sp=Cyc_Tcpat_tuple_pat(
_tmp2BC,((_tmp52A=_region_malloc(_tmp2BC,sizeof(*_tmp52A)),((_tmp52A->hd=sp0,((
_tmp52A->tl=((_tmp52B=_region_malloc(_tmp2BC,sizeof(*_tmp52B)),((_tmp52B->hd=Cyc_Tcpat_int_pat(
_tmp2BC,*_tmp2BD,0),((_tmp52B->tl=0,_tmp52B)))))),_tmp52A)))))),0);}*_tmp2BD=*
_tmp2BD + 1;}else{struct Cyc_List_List*_tmp535;struct Cyc_Tcpat_Any_struct _tmp534;
struct Cyc_Tcpat_Any_struct*_tmp533;struct Cyc_List_List*_tmp532;sp=Cyc_Tcpat_tuple_pat(
_tmp2BC,((_tmp532=_region_malloc(_tmp2BC,sizeof(*_tmp532)),((_tmp532->hd=sp0,((
_tmp532->tl=((_tmp535=_region_malloc(_tmp2BC,sizeof(*_tmp535)),((_tmp535->hd=(
void*)((_tmp533=_region_malloc(_tmp2BC,sizeof(*_tmp533)),((_tmp533[0]=((_tmp534.tag=
0,_tmp534)),_tmp533)))),((_tmp535->tl=0,_tmp535)))))),_tmp532)))))),0);}{struct
_tuple21*_tmp536;return(_tmp536=_region_malloc(_tmp2BC,sizeof(*_tmp536)),((
_tmp536->f1=sp,((_tmp536->f2=rhs,_tmp536)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";
struct Cyc_Tcpat_Desc2string_struct{char*tag;};struct Cyc_Tcpat_Desc2string_struct
Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp2C7=d;struct Cyc_Tcpat_Con_s*_tmp2C9;
struct Cyc_List_List*_tmp2CA;struct Cyc_Set_Set*_tmp2CC;_LL13A: {struct Cyc_Tcpat_Pos_struct*
_tmp2C8=(struct Cyc_Tcpat_Pos_struct*)_tmp2C7;if(_tmp2C8->tag != 0)goto _LL13C;
else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;}}_LL13B: {union Cyc_Tcpat_Name_value
_tmp2CD=_tmp2C9->name;struct Cyc_Absyn_Pat*_tmp2CE=_tmp2C9->orig_pat;if(_tmp2CE == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp2CA))->hd);{void*_tmp2CF=_tmp2CE->r;struct Cyc_Absyn_Vardecl*_tmp2D2;struct
Cyc_Absyn_Vardecl*_tmp2D4;struct Cyc_Absyn_Tvar*_tmp2D6;struct Cyc_Absyn_Vardecl*
_tmp2D7;struct Cyc_Absyn_AggrInfo*_tmp2DB;struct Cyc_Absyn_AggrInfo _tmp2DC;union
Cyc_Absyn_AggrInfoU _tmp2DD;struct Cyc_Absyn_Aggrdecl**_tmp2DE;struct Cyc_Absyn_Aggrdecl*
_tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_Absyn_Datatypefield*_tmp2E2;int
_tmp2E5;char _tmp2E7;struct _dyneither_ptr _tmp2E9;struct Cyc_Absyn_Enumfield*
_tmp2EB;struct Cyc_Absyn_Enumfield*_tmp2ED;struct Cyc_Absyn_Exp*_tmp2EF;_LL13F: {
struct Cyc_Absyn_Wild_p_struct*_tmp2D0=(struct Cyc_Absyn_Wild_p_struct*)_tmp2CF;
if(_tmp2D0->tag != 0)goto _LL141;}_LL140: {const char*_tmp537;return(_tmp537="_",
_tag_dyneither(_tmp537,sizeof(char),2));}_LL141: {struct Cyc_Absyn_Var_p_struct*
_tmp2D1=(struct Cyc_Absyn_Var_p_struct*)_tmp2CF;if(_tmp2D1->tag != 1)goto _LL143;
else{_tmp2D2=_tmp2D1->f1;}}_LL142: return Cyc_Absynpp_qvar2string(_tmp2D2->name);
_LL143: {struct Cyc_Absyn_Reference_p_struct*_tmp2D3=(struct Cyc_Absyn_Reference_p_struct*)
_tmp2CF;if(_tmp2D3->tag != 2)goto _LL145;else{_tmp2D4=_tmp2D3->f1;}}_LL144: {const
char*_tmp53B;void*_tmp53A[1];struct Cyc_String_pa_struct _tmp539;return(struct
_dyneither_ptr)((_tmp539.tag=0,((_tmp539.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D4->name)),((_tmp53A[0]=& _tmp539,Cyc_aprintf(((
_tmp53B="*%s",_tag_dyneither(_tmp53B,sizeof(char),4))),_tag_dyneither(_tmp53A,
sizeof(void*),1))))))));}_LL145: {struct Cyc_Absyn_TagInt_p_struct*_tmp2D5=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp2CF;if(_tmp2D5->tag != 3)goto _LL147;else{
_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}_LL146: {const char*_tmp540;void*
_tmp53F[2];struct Cyc_String_pa_struct _tmp53E;struct Cyc_String_pa_struct _tmp53D;
return(struct _dyneither_ptr)((_tmp53D.tag=0,((_tmp53D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2D6->name),((_tmp53E.tag=0,((_tmp53E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D7->name)),((
_tmp53F[0]=& _tmp53E,((_tmp53F[1]=& _tmp53D,Cyc_aprintf(((_tmp540="%s<`%s>",
_tag_dyneither(_tmp540,sizeof(char),8))),_tag_dyneither(_tmp53F,sizeof(void*),2))))))))))))));}
_LL147: {struct Cyc_Absyn_Tuple_p_struct*_tmp2D8=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp2CF;if(_tmp2D8->tag != 4)goto _LL149;}_LL148: {const char*_tmp544;void*_tmp543[
1];struct Cyc_String_pa_struct _tmp542;return(struct _dyneither_ptr)((_tmp542.tag=0,((
_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2CA)),((_tmp543[0]=& _tmp542,Cyc_aprintf(((_tmp544="$(%s)",_tag_dyneither(
_tmp544,sizeof(char),6))),_tag_dyneither(_tmp543,sizeof(void*),1))))))));}_LL149: {
struct Cyc_Absyn_Pointer_p_struct*_tmp2D9=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp2CF;if(_tmp2D9->tag != 5)goto _LL14B;}_LL14A: {const char*_tmp548;void*_tmp547[
1];struct Cyc_String_pa_struct _tmp546;return(struct _dyneither_ptr)((_tmp546.tag=0,((
_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2CA)),((_tmp547[0]=& _tmp546,Cyc_aprintf(((_tmp548="&%s",_tag_dyneither(
_tmp548,sizeof(char),4))),_tag_dyneither(_tmp547,sizeof(void*),1))))))));}_LL14B: {
struct Cyc_Absyn_Aggr_p_struct*_tmp2DA=(struct Cyc_Absyn_Aggr_p_struct*)_tmp2CF;
if(_tmp2DA->tag != 6)goto _LL14D;else{_tmp2DB=_tmp2DA->f1;if(_tmp2DB == 0)goto
_LL14D;_tmp2DC=*_tmp2DB;_tmp2DD=_tmp2DC.aggr_info;if((_tmp2DD.KnownAggr).tag != 2)
goto _LL14D;_tmp2DE=(struct Cyc_Absyn_Aggrdecl**)(_tmp2DD.KnownAggr).val;_tmp2DF=*
_tmp2DE;_tmp2E0=_tmp2DA->f3;}}_LL14C: if(_tmp2DF->kind == Cyc_Absyn_UnionA){struct
Cyc_List_List*_tmp2FE=_tmp2E0;struct Cyc_List_List _tmp2FF;struct _tuple14*_tmp300;
struct _tuple14 _tmp301;struct Cyc_List_List*_tmp302;struct Cyc_List_List _tmp303;
void*_tmp304;struct _dyneither_ptr*_tmp306;_LL160: if(_tmp2FE == 0)goto _LL162;
_tmp2FF=*_tmp2FE;_tmp300=(struct _tuple14*)_tmp2FF.hd;_tmp301=*_tmp300;_tmp302=
_tmp301.f1;if(_tmp302 == 0)goto _LL162;_tmp303=*_tmp302;_tmp304=(void*)_tmp303.hd;{
struct Cyc_Absyn_FieldName_struct*_tmp305=(struct Cyc_Absyn_FieldName_struct*)
_tmp304;if(_tmp305->tag != 1)goto _LL162;else{_tmp306=_tmp305->f1;}};_LL161: {
const char*_tmp54E;void*_tmp54D[3];struct Cyc_String_pa_struct _tmp54C;struct Cyc_String_pa_struct
_tmp54B;struct Cyc_String_pa_struct _tmp54A;return(struct _dyneither_ptr)((_tmp54A.tag=
0,((_tmp54A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2CA)),((_tmp54B.tag=0,((_tmp54B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp306),((_tmp54C.tag=0,((_tmp54C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DF->name)),((_tmp54D[0]=&
_tmp54C,((_tmp54D[1]=& _tmp54B,((_tmp54D[2]=& _tmp54A,Cyc_aprintf(((_tmp54E="%s{.%s = %s}",
_tag_dyneither(_tmp54E,sizeof(char),13))),_tag_dyneither(_tmp54D,sizeof(void*),3))))))))))))))))))));}
_LL162:;_LL163: goto _LL15F;_LL15F:;}{const char*_tmp553;void*_tmp552[2];struct Cyc_String_pa_struct
_tmp551;struct Cyc_String_pa_struct _tmp550;return(struct _dyneither_ptr)((_tmp550.tag=
0,((_tmp550.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2CA)),((_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DF->name)),((_tmp552[0]=& _tmp551,((
_tmp552[1]=& _tmp550,Cyc_aprintf(((_tmp553="%s{%s}",_tag_dyneither(_tmp553,
sizeof(char),7))),_tag_dyneither(_tmp552,sizeof(void*),2))))))))))))));};_LL14D: {
struct Cyc_Absyn_Datatype_p_struct*_tmp2E1=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp2CF;if(_tmp2E1->tag != 7)goto _LL14F;else{_tmp2E2=_tmp2E1->f2;}}_LL14E: if(
_tmp2CA == 0){const char*_tmp557;void*_tmp556[1];struct Cyc_String_pa_struct _tmp555;
return(struct _dyneither_ptr)((_tmp555.tag=0,((_tmp555.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2E2->name)),((_tmp556[0]=&
_tmp555,Cyc_aprintf(((_tmp557="%s",_tag_dyneither(_tmp557,sizeof(char),3))),
_tag_dyneither(_tmp556,sizeof(void*),1))))))));}else{const char*_tmp55C;void*
_tmp55B[2];struct Cyc_String_pa_struct _tmp55A;struct Cyc_String_pa_struct _tmp559;
return(struct _dyneither_ptr)((_tmp559.tag=0,((_tmp559.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA)),((_tmp55A.tag=0,((
_tmp55A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2E2->name)),((_tmp55B[0]=& _tmp55A,((_tmp55B[1]=& _tmp559,Cyc_aprintf(((
_tmp55C="%s(%s)",_tag_dyneither(_tmp55C,sizeof(char),7))),_tag_dyneither(_tmp55B,
sizeof(void*),2))))))))))))));}_LL14F: {struct Cyc_Absyn_Null_p_struct*_tmp2E3=(
struct Cyc_Absyn_Null_p_struct*)_tmp2CF;if(_tmp2E3->tag != 8)goto _LL151;}_LL150: {
const char*_tmp55D;return(_tmp55D="NULL",_tag_dyneither(_tmp55D,sizeof(char),5));}
_LL151: {struct Cyc_Absyn_Int_p_struct*_tmp2E4=(struct Cyc_Absyn_Int_p_struct*)
_tmp2CF;if(_tmp2E4->tag != 9)goto _LL153;else{_tmp2E5=_tmp2E4->f2;}}_LL152: {const
char*_tmp561;void*_tmp560[1];struct Cyc_Int_pa_struct _tmp55F;return(struct
_dyneither_ptr)((_tmp55F.tag=1,((_tmp55F.f1=(unsigned long)_tmp2E5,((_tmp560[0]=&
_tmp55F,Cyc_aprintf(((_tmp561="%d",_tag_dyneither(_tmp561,sizeof(char),3))),
_tag_dyneither(_tmp560,sizeof(void*),1))))))));}_LL153: {struct Cyc_Absyn_Char_p_struct*
_tmp2E6=(struct Cyc_Absyn_Char_p_struct*)_tmp2CF;if(_tmp2E6->tag != 10)goto _LL155;
else{_tmp2E7=_tmp2E6->f1;}}_LL154: {const char*_tmp565;void*_tmp564[1];struct Cyc_Int_pa_struct
_tmp563;return(struct _dyneither_ptr)((_tmp563.tag=1,((_tmp563.f1=(unsigned long)((
int)_tmp2E7),((_tmp564[0]=& _tmp563,Cyc_aprintf(((_tmp565="%d",_tag_dyneither(
_tmp565,sizeof(char),3))),_tag_dyneither(_tmp564,sizeof(void*),1))))))));}_LL155: {
struct Cyc_Absyn_Float_p_struct*_tmp2E8=(struct Cyc_Absyn_Float_p_struct*)_tmp2CF;
if(_tmp2E8->tag != 11)goto _LL157;else{_tmp2E9=_tmp2E8->f1;}}_LL156: return _tmp2E9;
_LL157: {struct Cyc_Absyn_Enum_p_struct*_tmp2EA=(struct Cyc_Absyn_Enum_p_struct*)
_tmp2CF;if(_tmp2EA->tag != 12)goto _LL159;else{_tmp2EB=_tmp2EA->f2;}}_LL158:
_tmp2ED=_tmp2EB;goto _LL15A;_LL159: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp2EC=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp2CF;if(_tmp2EC->tag != 13)goto _LL15B;else{
_tmp2ED=_tmp2EC->f2;}}_LL15A: return Cyc_Absynpp_qvar2string(_tmp2ED->name);_LL15B: {
struct Cyc_Absyn_Exp_p_struct*_tmp2EE=(struct Cyc_Absyn_Exp_p_struct*)_tmp2CF;if(
_tmp2EE->tag != 16)goto _LL15D;else{_tmp2EF=_tmp2EE->f1;}}_LL15C: return Cyc_Absynpp_exp2string(
_tmp2EF);_LL15D:;_LL15E:(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL13E:;};}
_LL13C: {struct Cyc_Tcpat_Neg_struct*_tmp2CB=(struct Cyc_Tcpat_Neg_struct*)_tmp2C7;
if(_tmp2CB->tag != 1)goto _LL139;else{_tmp2CC=_tmp2CB->f1;}}_LL13D: if(((int(*)(
struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2CC)){const char*_tmp566;return(
_tmp566="_",_tag_dyneither(_tmp566,sizeof(char),2));}{struct Cyc_Tcpat_Con_s*
_tmp31F=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2CC);
struct Cyc_Absyn_Pat*_tmp320=_tmp31F->orig_pat;if(_tmp320 == 0)(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);{void*_tmp321=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp320->topt))->v);enum Cyc_Absyn_Size_of _tmp323;struct Cyc_Absyn_PtrInfo
_tmp326;struct Cyc_Absyn_PtrAtts _tmp327;struct Cyc_Absyn_DatatypeInfo _tmp329;union
Cyc_Absyn_DatatypeInfoU _tmp32A;struct Cyc_Absyn_Datatypedecl**_tmp32B;struct Cyc_Absyn_Datatypedecl*
_tmp32C;struct Cyc_Absyn_AggrInfo _tmp32E;union Cyc_Absyn_AggrInfoU _tmp32F;struct
Cyc_Absyn_Aggrdecl**_tmp330;struct Cyc_Absyn_Aggrdecl*_tmp331;_LL165: {struct Cyc_Absyn_IntType_struct*
_tmp322=(struct Cyc_Absyn_IntType_struct*)_tmp321;if(_tmp322->tag != 6)goto _LL167;
else{_tmp323=_tmp322->f2;if(_tmp323 != Cyc_Absyn_Char_sz)goto _LL167;}}_LL166:{int
i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp332=Cyc_Tcpat_char_con(r,(char)i,(
struct Cyc_Absyn_Pat*)_tmp320);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2CC,_tmp332)){const char*_tmp56A;void*_tmp569[1];struct
Cyc_Int_pa_struct _tmp568;return(struct _dyneither_ptr)((_tmp568.tag=1,((_tmp568.f1=(
unsigned long)i,((_tmp569[0]=& _tmp568,Cyc_aprintf(((_tmp56A="%d",_tag_dyneither(
_tmp56A,sizeof(char),3))),_tag_dyneither(_tmp569,sizeof(void*),1))))))));}}}(int)
_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL167: {struct Cyc_Absyn_IntType_struct*
_tmp324=(struct Cyc_Absyn_IntType_struct*)_tmp321;if(_tmp324->tag != 6)goto _LL169;}
_LL168:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp336=Cyc_Tcpat_int_con(
r,(int)i,_tmp320);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp2CC,_tmp336)){const char*_tmp56E;void*_tmp56D[1];struct Cyc_Int_pa_struct
_tmp56C;return(struct _dyneither_ptr)((_tmp56C.tag=1,((_tmp56C.f1=(unsigned long)((
int)i),((_tmp56D[0]=& _tmp56C,Cyc_aprintf(((_tmp56E="%d",_tag_dyneither(_tmp56E,
sizeof(char),3))),_tag_dyneither(_tmp56D,sizeof(void*),1))))))));}}}(int)_throw((
void*)& Cyc_Tcpat_Desc2string_val);_LL169: {struct Cyc_Absyn_PointerType_struct*
_tmp325=(struct Cyc_Absyn_PointerType_struct*)_tmp321;if(_tmp325->tag != 5)goto
_LL16B;else{_tmp326=_tmp325->f1;_tmp327=_tmp326.ptr_atts;}}_LL16A: {union Cyc_Absyn_Constraint*
_tmp33A=_tmp327.nullable;int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp33A);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*
s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,Cyc_Tcpat_null_con(r,(
struct Cyc_Absyn_Pat*)_tmp320))){const char*_tmp56F;return(_tmp56F="NULL",
_tag_dyneither(_tmp56F,sizeof(char),5));}}{const char*_tmp570;return(_tmp570="&_",
_tag_dyneither(_tmp570,sizeof(char),3));};}_LL16B: {struct Cyc_Absyn_DatatypeType_struct*
_tmp328=(struct Cyc_Absyn_DatatypeType_struct*)_tmp321;if(_tmp328->tag != 3)goto
_LL16D;else{_tmp329=_tmp328->f1;_tmp32A=_tmp329.datatype_info;if((_tmp32A.KnownDatatype).tag
!= 2)goto _LL16D;_tmp32B=(struct Cyc_Absyn_Datatypedecl**)(_tmp32A.KnownDatatype).val;
_tmp32C=*_tmp32B;}}_LL16C: if(_tmp32C->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp33D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp32C->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp33D);for(0;(unsigned int)_tmp33D;_tmp33D=_tmp33D->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp33D->hd)->name).f2;struct Cyc_List_List*
_tmp33E=((struct Cyc_Absyn_Datatypefield*)_tmp33D->hd)->typs;struct Cyc_Tcpat_Con_s*
_tmp571;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2CC,(struct Cyc_Tcpat_Con_s*)((_tmp571=_cycalloc(sizeof(*_tmp571)),((_tmp571->name=
Cyc_Tcpat_Name_v(n),((_tmp571->arity=0,((_tmp571->span=0,((_tmp571->orig_pat=
_tmp320,_tmp571)))))))))))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp33E)== 0)return n;else{const char*_tmp575;void*_tmp574[1];struct Cyc_String_pa_struct
_tmp573;return(struct _dyneither_ptr)((_tmp573.tag=0,((_tmp573.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)n),((_tmp574[0]=& _tmp573,Cyc_aprintf(((
_tmp575="%s(...)",_tag_dyneither(_tmp575,sizeof(char),8))),_tag_dyneither(
_tmp574,sizeof(void*),1))))))));}}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};
_LL16D:{struct Cyc_Absyn_AggrType_struct*_tmp32D=(struct Cyc_Absyn_AggrType_struct*)
_tmp321;if(_tmp32D->tag != 12)goto _LL16F;else{_tmp32E=_tmp32D->f1;_tmp32F=_tmp32E.aggr_info;
if((_tmp32F.KnownAggr).tag != 2)goto _LL16F;_tmp330=(struct Cyc_Absyn_Aggrdecl**)(
_tmp32F.KnownAggr).val;_tmp331=*_tmp330;}}if(!(_tmp331->kind == Cyc_Absyn_UnionA))
goto _LL16F;_LL16E: {struct Cyc_List_List*_tmp343=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp331->impl))->fields;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp343);struct _tuple1 _tmp345;struct _dyneither_ptr*_tmp346;struct _dyneither_ptr
_tmp347;struct _tuple1*_tmp344=_tmp331->name;_tmp345=*_tmp344;_tmp346=_tmp345.f2;
_tmp347=*_tmp346;for(0;(unsigned int)_tmp343;_tmp343=_tmp343->tl){struct
_dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp343->hd)->name;struct Cyc_Tcpat_Con_s*
_tmp576;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2CC,(struct Cyc_Tcpat_Con_s*)((_tmp576=_cycalloc(sizeof(*_tmp576)),((_tmp576->name=
Cyc_Tcpat_Name_v(n),((_tmp576->arity=0,((_tmp576->span=0,((_tmp576->orig_pat=
_tmp320,_tmp576)))))))))))){const char*_tmp57B;void*_tmp57A[2];struct Cyc_String_pa_struct
_tmp579;struct Cyc_String_pa_struct _tmp578;return(struct _dyneither_ptr)((_tmp578.tag=
0,((_tmp578.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp579.tag=0,((
_tmp579.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp347),((_tmp57A[0]=&
_tmp579,((_tmp57A[1]=& _tmp578,Cyc_aprintf(((_tmp57B="%s{.%s = _}",_tag_dyneither(
_tmp57B,sizeof(char),12))),_tag_dyneither(_tmp57A,sizeof(void*),2))))))))))))));}}(
int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL16F:;_LL170:(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);_LL164:;};};_LL139:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(
struct _RegionHandle*r,void*d){struct _dyneither_ptr*_tmp57C;return(_tmp57C=
_cycalloc(sizeof(*_tmp57C)),((_tmp57C[0]=Cyc_Tcpat_desc2string(r,d),_tmp57C)));}
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",
sizeof(char),2);{struct Cyc_List_List*_tmp34F=_tmp34E;for(0;_tmp34F != 0;_tmp34F=((
struct Cyc_List_List*)_check_null(_tmp34F))->tl){if(_tmp34F->tl != 0){{struct Cyc_List_List*
_tmp57D;_tmp34F->tl=(struct Cyc_List_List*)((_tmp57D=_cycalloc(sizeof(*_tmp57D)),((
_tmp57D->hd=comma,((_tmp57D->tl=_tmp34F->tl,_tmp57D))))));}_tmp34F=_tmp34F->tl;}}}
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp34E);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp352;_push_handler(& _tmp352);{int _tmp354=0;if(setjmp(_tmp352.handler))
_tmp354=1;if(!_tmp354){{struct _dyneither_ptr _tmp355=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp581;void*_tmp580[1];struct Cyc_String_pa_struct _tmp57F;(_tmp57F.tag=
0,((_tmp57F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp355),((_tmp580[0]=&
_tmp57F,Cyc_Tcutil_terr(loc,((_tmp581="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp581,sizeof(char),53))),_tag_dyneither(_tmp580,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp353=(void*)_exn_thrown;void*_tmp35A=_tmp353;_LL172: {
struct Cyc_Tcpat_Desc2string_struct*_tmp35B=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp35A;if(_tmp35B->tag != Cyc_Tcpat_Desc2string)goto _LL174;}_LL173:{const char*
_tmp584;void*_tmp583;(_tmp583=0,Cyc_Tcutil_terr(loc,((_tmp584="patterns may not be exhaustive.",
_tag_dyneither(_tmp584,sizeof(char),32))),_tag_dyneither(_tmp583,sizeof(void*),0)));}
goto _LL171;_LL174:;_LL175:(void)_throw(_tmp35A);_LL171:;}};}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple20*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp35E=Cyc_Tcenv_get_fnrgn(te);int _tmp35F=0;struct _tuple22
_tmp585;struct _tuple22 _tmp360=(_tmp585.f1=_tmp35E,((_tmp585.f2=& _tmp35F,_tmp585)));
struct Cyc_List_List*_tmp361=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _tuple22*,struct Cyc_Absyn_Switch_clause*),struct
_tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp35E,Cyc_Tcpat_get_match,&
_tmp360,swcs);void*_tmp362=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp35E,_tmp361);int _tmp363=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple20*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp35E,_tmp362,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp363);for(0;_tmp361 != 0;_tmp361=_tmp361->tl){
struct _tuple21 _tmp365;struct _tuple20*_tmp366;struct _tuple20 _tmp367;int _tmp368;
struct Cyc_Position_Segment*_tmp369;struct _tuple21*_tmp364=(struct _tuple21*)
_tmp361->hd;_tmp365=*_tmp364;_tmp366=_tmp365.f2;_tmp367=*_tmp366;_tmp368=_tmp367.f1;
_tmp369=_tmp367.f2;if(!_tmp368){const char*_tmp588;void*_tmp587;(_tmp587=0,Cyc_Tcutil_terr(
_tmp369,((_tmp588="redundant pattern",_tag_dyneither(_tmp588,sizeof(char),18))),
_tag_dyneither(_tmp587,sizeof(void*),0)));}}}struct _tuple23{struct Cyc_Position_Segment*
f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct
_tuple23*pr,void*desc){(*pr).f2=0;{struct _handler_cons _tmp36D;_push_handler(&
_tmp36D);{int _tmp36F=0;if(setjmp(_tmp36D.handler))_tmp36F=1;if(!_tmp36F){{struct
_dyneither_ptr _tmp370=Cyc_Tcpat_desc2string(r,desc);const char*_tmp58C;void*
_tmp58B[1];struct Cyc_String_pa_struct _tmp58A;(_tmp58A.tag=0,((_tmp58A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp370),((_tmp58B[0]=& _tmp58A,Cyc_Tcutil_warn((*
pr).f1,((_tmp58C="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(
_tmp58C,sizeof(char),45))),_tag_dyneither(_tmp58B,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp36E=(void*)_exn_thrown;void*_tmp375=_tmp36E;_LL177: {
struct Cyc_Tcpat_Desc2string_struct*_tmp376=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp375;if(_tmp376->tag != Cyc_Tcpat_Desc2string)goto _LL179;}_LL178:{const char*
_tmp58F;void*_tmp58E;(_tmp58E=0,Cyc_Tcutil_warn((*pr).f1,((_tmp58F="pattern not exhaustive.",
_tag_dyneither(_tmp58F,sizeof(char),24))),_tag_dyneither(_tmp58E,sizeof(void*),0)));}
goto _LL176;_LL179:;_LL17A:(void)_throw(_tmp375);_LL176:;}};};}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple24{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle*_tmp379=Cyc_Tcenv_get_fnrgn(te);struct _tuple24*_tmp592;
struct Cyc_List_List*_tmp591;struct Cyc_List_List*_tmp37A=(_tmp591=_region_malloc(
_tmp379,sizeof(*_tmp591)),((_tmp591->hd=((_tmp592=_region_malloc(_tmp379,sizeof(*
_tmp592)),((_tmp592->f1=Cyc_Tcpat_compile_pat(_tmp379,p),((_tmp592->f2=0,_tmp592)))))),((
_tmp591->tl=0,_tmp591)))));void*_tmp37B=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp379,_tmp37A);struct
_tuple23 _tmp593;struct _tuple23 _tmp37C=(_tmp593.f1=loc,((_tmp593.f2=1,_tmp593)));
int _tmp37D=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct
_RegionHandle*,struct _tuple23*,void*),struct _tuple23*env1,void(*rhs_appears)(int,
int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp379,_tmp37B,
Cyc_Tcpat_not_exhaust_warn,& _tmp37C,Cyc_Tcpat_dummy_fn,0,& _tmp37D);return _tmp37C.f2;}
static struct _tuple21*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple20*_tmp594;
struct _tuple20*rhs=(_tmp594=_region_malloc(r,sizeof(*_tmp594)),((_tmp594->f1=0,((
_tmp594->f2=(swc->pattern)->loc,_tmp594)))));struct _tuple21*_tmp595;return(
_tmp595=_region_malloc(r,sizeof(*_tmp595)),((_tmp595->f1=sp0,((_tmp595->f2=rhs,
_tmp595)))));}static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){struct _RegionHandle*
_tmp383=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp384=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple21*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp383,Cyc_Tcpat_get_match2,
_tmp383,swcs);void*_tmp385=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp383,_tmp384);int _tmp386=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple20*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp383,_tmp385,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp386);for(0;_tmp384 != 0;_tmp384=_tmp384->tl){
struct _tuple21 _tmp388;struct _tuple20*_tmp389;struct _tuple20 _tmp38A;int _tmp38B;
struct Cyc_Position_Segment*_tmp38C;struct _tuple21*_tmp387=(struct _tuple21*)
_tmp384->hd;_tmp388=*_tmp387;_tmp389=_tmp388.f2;_tmp38A=*_tmp389;_tmp38B=_tmp38A.f1;
_tmp38C=_tmp38A.f2;if(!_tmp38B){const char*_tmp598;void*_tmp597;(_tmp597=0,Cyc_Tcutil_terr(
_tmp38C,((_tmp598="redundant pattern",_tag_dyneither(_tmp598,sizeof(char),18))),
_tag_dyneither(_tmp597,sizeof(void*),0)));}}}
