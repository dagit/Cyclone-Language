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
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_unique_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
typedef struct Cyc_List_List*Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};struct
Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(
void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);
};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Set_Set;typedef struct Cyc_Set_Set*Cyc_Set_set_t;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};void*Cyc_Set_choose(struct Cyc_Set_Set*s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;typedef
struct Cyc_Position_Segment*Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{int
tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern
char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*tag;};
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*
Cyc_Absyn_var_t;typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};typedef union Cyc_Absyn_Nmspace
Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*
Cyc_Absyn_qvar_opt_t;typedef struct _tuple1*Cyc_Absyn_typedef_name_t;typedef struct
_tuple1*Cyc_Absyn_typedef_name_opt_t;typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;
struct Cyc_Position_Segment*loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;
struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;
};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
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
};extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
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
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;
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
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
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
Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
void*Cyc_Absyn_float_typ(int);extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
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
k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Absyn_Kind
Cyc_Tcutil_tmk;extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt
Cyc_Tcutil_ako;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;extern struct Cyc_Core_Opt
Cyc_Tcutil_trko;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple11{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple13*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};typedef struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple14{unsigned int f1;int f2;};struct _tuple14 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
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
void*)& Cyc_Absyn_Wild_p_val;{const char*_tmp3BB;void*_tmp3BA;(_tmp3BA=0,Cyc_Tcutil_terr(
p->loc,((_tmp3BB="struct tag used without arguments in pattern",_tag_dyneither(
_tmp3BB,sizeof(char),45))),_tag_dyneither(_tmp3BA,sizeof(void*),0)));}
_npop_handler(0);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp20=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp1E;if(_tmp20->tag != 2)goto _LL12;else{
_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL11:{struct Cyc_Absyn_Datatype_p_struct
_tmp3BE;struct Cyc_Absyn_Datatype_p_struct*_tmp3BD;p->r=(void*)((_tmp3BD=
_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BE.tag=7,((_tmp3BE.f1=_tmp21,((
_tmp3BE.f2=_tmp22,((_tmp3BE.f3=0,((_tmp3BE.f4=0,_tmp3BE)))))))))),_tmp3BD))));}
_npop_handler(0);return;_LL12: {struct Cyc_Tcenv_EnumRes_struct*_tmp23=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp1E;if(_tmp23->tag != 3)goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}
_LL13:{struct Cyc_Absyn_Enum_p_struct _tmp3C1;struct Cyc_Absyn_Enum_p_struct*
_tmp3C0;p->r=(void*)((_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0[0]=((_tmp3C1.tag=
12,((_tmp3C1.f1=_tmp24,((_tmp3C1.f2=_tmp25,_tmp3C1)))))),_tmp3C0))));}
_npop_handler(0);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp26=(
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp1E;if(_tmp26->tag != 4)goto _LL16;else{
_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}_LL15:{struct Cyc_Absyn_AnonEnum_p_struct
_tmp3C4;struct Cyc_Absyn_AnonEnum_p_struct*_tmp3C3;p->r=(void*)((_tmp3C3=
_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C4.tag=13,((_tmp3C4.f1=(void*)
_tmp27,((_tmp3C4.f2=_tmp28,_tmp3C4)))))),_tmp3C3))));}_npop_handler(0);return;
_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp29=(struct Cyc_Tcenv_VarRes_struct*)
_tmp1E;if(_tmp29->tag != 0)goto _LLD;}_LL17: goto _LLD;_LLD:;};_pop_handler();}else{
void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {struct Cyc_Dict_Absent_struct*
_tmp34=(struct Cyc_Dict_Absent_struct*)_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)
goto _LL1B;}_LL1A: goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{union
Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;_LL1E:
if((_tmp35.Loc_n).tag != 4)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F: goto
_LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(
_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:(*_tmp2).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_Var_p_struct _tmp3C7;struct Cyc_Absyn_Var_p_struct*_tmp3C6;p->r=(
void*)((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6[0]=((_tmp3C7.tag=1,((
_tmp3C7.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((
_tmp3C7.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3C7)))))),
_tmp3C6))));}return;_LL22:;_LL23:{const char*_tmp3CA;void*_tmp3C9;(_tmp3C9=0,Cyc_Tcutil_terr(
p->loc,((_tmp3CA="qualified variable in pattern",_tag_dyneither(_tmp3CA,sizeof(
char),30))),_tag_dyneither(_tmp3C9,sizeof(void*),0)));}return;_LL1D:;};_LL3: {
struct Cyc_Absyn_UnknownCall_p_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=
_tmp3->f3;}}_LL4:{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;
if(setjmp(_tmp3C.handler))_tmp3E=1;if(!_tmp3E){{struct _RegionHandle*_tmp3F=Cyc_Tcenv_get_fnrgn(
te);void*_tmp40=Cyc_Tcenv_lookup_ordinary(_tmp3F,te,p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*
_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;
_LL25: {struct Cyc_Tcenv_AggrRes_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {struct Cyc_List_List*
_tmp49=0;for(0;_tmp5 != 0;_tmp5=_tmp5->tl){struct _tuple15*_tmp3CD;struct Cyc_List_List*
_tmp3CC;_tmp49=((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC->hd=((_tmp3CD=
_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD->f1=0,((_tmp3CD->f2=(struct Cyc_Absyn_Pat*)
_tmp5->hd,_tmp3CD)))))),((_tmp3CC->tl=_tmp49,_tmp3CC))))));}{struct Cyc_Absyn_Aggr_p_struct
_tmp3D7;struct Cyc_Absyn_Aggrdecl**_tmp3D6;struct Cyc_Absyn_AggrInfo*_tmp3D5;
struct Cyc_Absyn_Aggr_p_struct*_tmp3D4;p->r=(void*)((_tmp3D4=_cycalloc(sizeof(*
_tmp3D4)),((_tmp3D4[0]=((_tmp3D7.tag=6,((_tmp3D7.f1=((_tmp3D5=_cycalloc(sizeof(*
_tmp3D5)),((_tmp3D5->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3D6=_cycalloc(sizeof(*
_tmp3D6)),((_tmp3D6[0]=_tmp42,_tmp3D6))))),((_tmp3D5->targs=0,_tmp3D5)))))),((
_tmp3D7.f2=0,((_tmp3D7.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp49),((_tmp3D7.f4=_tmp6,_tmp3D7)))))))))),_tmp3D4))));}_npop_handler(0);
return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp43=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp40;if(_tmp43->tag != 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}
_LL28:{struct Cyc_Absyn_Datatype_p_struct _tmp3DA;struct Cyc_Absyn_Datatype_p_struct*
_tmp3D9;p->r=(void*)((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DA.tag=
7,((_tmp3DA.f1=_tmp44,((_tmp3DA.f2=_tmp45,((_tmp3DA.f3=_tmp5,((_tmp3DA.f4=_tmp6,
_tmp3DA)))))))))),_tmp3D9))));}_npop_handler(0);return;_LL29: {struct Cyc_Tcenv_EnumRes_struct*
_tmp46=(struct Cyc_Tcenv_EnumRes_struct*)_tmp40;if(_tmp46->tag != 3)goto _LL2B;}
_LL2A: goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp47=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp40;if(_tmp47->tag != 4)goto _LL2D;}_LL2C:{const char*_tmp3DD;void*_tmp3DC;(
_tmp3DC=0,Cyc_Tcutil_terr(p->loc,((_tmp3DD="enum tag used with arguments in pattern",
_tag_dyneither(_tmp3DD,sizeof(char),40))),_tag_dyneither(_tmp3DC,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;_npop_handler(0);return;_LL2D: {struct Cyc_Tcenv_VarRes_struct*
_tmp48=(struct Cyc_Tcenv_VarRes_struct*)_tmp40;if(_tmp48->tag != 0)goto _LL24;}
_LL2E: goto _LL24;_LL24:;};_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;
void*_tmp55=_tmp3D;_LL30: {struct Cyc_Dict_Absent_struct*_tmp56=(struct Cyc_Dict_Absent_struct*)
_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31: goto _LL2F;_LL32:;_LL33:(
void)_throw(_tmp55);_LL2F:;}};}{const char*_tmp3E1;void*_tmp3E0[1];struct Cyc_String_pa_struct
_tmp3DF;(_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3E0[0]=& _tmp3DF,Cyc_Tcutil_terr(p->loc,((
_tmp3E1="%s is not a constructor in pattern",_tag_dyneither(_tmp3E1,sizeof(char),
35))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmp7->tag != 6)goto _LL7;else{_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;
_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=_tmp7->f4;}}_LL6: if(topt == 0){const char*
_tmp3E4;void*_tmp3E3;(_tmp3E3=0,Cyc_Tcutil_terr(p->loc,((_tmp3E4="cannot determine pattern type",
_tag_dyneither(_tmp3E4,sizeof(char),30))),_tag_dyneither(_tmp3E3,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));{void*_tmp5C=t;struct
Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct Cyc_Absyn_AggrType_struct*_tmp5D=(struct
Cyc_Absyn_AggrType_struct*)_tmp5C;if(_tmp5D->tag != 11)goto _LL37;else{_tmp5E=
_tmp5D->f1;}}_LL36:{struct Cyc_Absyn_Aggr_p_struct _tmp3EA;struct Cyc_Absyn_AggrInfo*
_tmp3E9;struct Cyc_Absyn_Aggr_p_struct*_tmp3E8;p->r=(void*)((_tmp3E8=_cycalloc(
sizeof(*_tmp3E8)),((_tmp3E8[0]=((_tmp3EA.tag=6,((_tmp3EA.f1=((_tmp3E9=_cycalloc(
sizeof(*_tmp3E9)),((_tmp3E9[0]=_tmp5E,_tmp3E9)))),((_tmp3EA.f2=_tmp9,((_tmp3EA.f3=
_tmpA,((_tmp3EA.f4=_tmpB,_tmp3EA)))))))))),_tmp3E8))));}Cyc_Tcpat_resolve_pat(te,
topt,p);goto _LL34;_LL37:;_LL38:{const char*_tmp3EE;void*_tmp3ED[1];struct Cyc_String_pa_struct
_tmp3EC;(_tmp3EC.tag=0,((_tmp3EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3ED[0]=& _tmp3EC,Cyc_Tcutil_terr(p->loc,((
_tmp3EE="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3EE,
sizeof(char),45))),_tag_dyneither(_tmp3ED,sizeof(void*),1)))))));}goto _LL34;
_LL34:;}return;};_LL7: {struct Cyc_Absyn_Aggr_p_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;
_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;
_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;
_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;
_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:{struct _handler_cons _tmp65;
_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))_tmp67=1;if(!
_tmp67){{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmp12);struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;if(_tmp69->impl == 0){{const char*
_tmp3F6;void*_tmp3F5[1];const char*_tmp3F4;const char*_tmp3F3;struct Cyc_String_pa_struct
_tmp3F2;(_tmp3F2.tag=0,((_tmp3F2.f1=(struct _dyneither_ptr)(_tmp69->kind == Cyc_Absyn_StructA?(
_tmp3F3="struct",_tag_dyneither(_tmp3F3,sizeof(char),7)):((_tmp3F4="union",
_tag_dyneither(_tmp3F4,sizeof(char),6)))),((_tmp3F5[0]=& _tmp3F2,Cyc_Tcutil_terr(
p->loc,((_tmp3F6="can't destructure an abstract %s",_tag_dyneither(_tmp3F6,
sizeof(char),33))),_tag_dyneither(_tmp3F5,sizeof(void*),1)))))));}p->r=(void*)&
Cyc_Absyn_Wild_p_val;_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp15);if(more_exists < 0){{
const char*_tmp3F9;void*_tmp3F8;(_tmp3F8=0,Cyc_Tcutil_terr(p->loc,((_tmp3F9="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp3F9,sizeof(char),55))),_tag_dyneither(_tmp3F8,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp71=_tmp15;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars);for(0;n != 
0;-- n){_tmp71=&((struct Cyc_List_List*)_check_null(*_tmp71))->tl;}}*_tmp71=0;};}
else{if(more_exists > 0){struct Cyc_List_List*_tmp72=0;for(0;more_exists != 0;--
more_exists){struct Cyc_Absyn_Unknown_kb_struct*_tmp3FF;struct Cyc_Absyn_Unknown_kb_struct
_tmp3FE;struct Cyc_List_List*_tmp3FD;_tmp72=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((
_tmp3FD->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((
_tmp3FF[0]=((_tmp3FE.tag=1,((_tmp3FE.f1=0,_tmp3FE)))),_tmp3FF))))),((_tmp3FD->tl=
_tmp72,_tmp3FD))))));}*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp15,_tmp72);}}{struct Cyc_Absyn_Aggr_p_struct
_tmp405;struct Cyc_Absyn_AggrInfo*_tmp404;struct Cyc_Absyn_Aggr_p_struct*_tmp403;p->r=(
void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp405.tag=6,((
_tmp405.f1=((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404->aggr_info=Cyc_Absyn_KnownAggr(
_tmp68),((_tmp404->targs=_tmp13,_tmp404)))))),((_tmp405.f2=*_tmp15,((_tmp405.f3=
_tmp16,((_tmp405.f4=_tmp17,_tmp405)))))))))),_tmp403))));};};};_pop_handler();}
else{void*_tmp66=(void*)_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {struct Cyc_Dict_Absent_struct*
_tmp7B=(struct Cyc_Dict_Absent_struct*)_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)
goto _LL3C;}_LL3B:{const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_terr(p->loc,((
_tmp408="Non-aggregate name has designator patterns",_tag_dyneither(_tmp408,
sizeof(char),43))),_tag_dyneither(_tmp407,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}return;_LL9: {struct Cyc_Absyn_Exp_p_struct*
_tmp18=(struct Cyc_Absyn_Exp_p_struct*)_tmp0;if(_tmp18->tag != 16)goto _LLB;else{
_tmp19=_tmp18->f1;}}_LLA: Cyc_Tcexp_tcExp(te,0,_tmp19);if(!Cyc_Tcutil_is_const_exp(
te,_tmp19)){{const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_Tcutil_terr(p->loc,((
_tmp40B="non-constant expression in case pattern",_tag_dyneither(_tmp40B,sizeof(
char),40))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=Cyc_Evexp_eval_const_uint_exp(
_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{struct Cyc_Absyn_Int_p_struct _tmp40E;
struct Cyc_Absyn_Int_p_struct*_tmp40D;p->r=(void*)((_tmp40D=_cycalloc_atomic(
sizeof(*_tmp40D)),((_tmp40D[0]=((_tmp40E.tag=9,((_tmp40E.f1=Cyc_Absyn_None,((
_tmp40E.f2=(int)_tmp81,_tmp40E)))))),_tmp40D))));}return;};_LLB:;_LLC: return;
_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**
topt){if(topt != 0)return*topt;{struct Cyc_Core_Opt*_tmp40F;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((
_tmp40F->v=s,_tmp40F)))));};}static void*Cyc_Tcpat_num_type(void**topt,void*numt){
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp86=Cyc_Tcutil_compress(
numt);_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmp87=(struct Cyc_Absyn_EnumType_struct*)
_tmp86;if(_tmp87->tag != 13)goto _LL41;}_LL40: if(topt != 0)return*topt;goto _LL3E;
_LL41:;_LL42: goto _LL3E;_LL3E:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_List_List**v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(
0);{struct Cyc_List_List*_tmp410;*v_result_ptr=(struct Cyc_List_List*)((_tmp410=
_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=vd,((_tmp410->tl=*v_result_ptr,_tmp410))))));};}
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple13*_tmp8A;struct Cyc_List_List*
_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;
_tmp8B=_tmp89.patvars;{struct _tuple13*_tmp8D;struct Cyc_List_List*_tmp8E;struct
Cyc_Tcpat_TcPatResult _tmp8C=res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=
_tmp8C.patvars;if(_tmp8A != 0  || _tmp8D != 0){if(_tmp8A == 0){struct _tuple13*
_tmp411;_tmp8A=((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411->f1=0,((_tmp411->f2=
0,_tmp411))))));}if(_tmp8D == 0){struct _tuple13*_tmp412;_tmp8D=((_tmp412=
_cycalloc(sizeof(*_tmp412)),((_tmp412->f1=0,((_tmp412->f2=0,_tmp412))))));}{
struct _tuple13*_tmp415;struct Cyc_Tcpat_TcPatResult _tmp414;return(_tmp414.tvars_and_bounds_opt=((
_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f1,(*
_tmp8D).f1),((_tmp415->f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f2,(*_tmp8D).f2),_tmp415)))))),((
_tmp414.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp414)));};}{struct Cyc_Tcpat_TcPatResult
_tmp416;return(_tmp416.tvars_and_bounds_opt=0,((_tmp416.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E),
_tmp416)));};};}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Pat*_tmp417;return(_tmp417=_cycalloc(sizeof(*_tmp417)),((
_tmp417->loc=loc,((_tmp417->topt=0,((_tmp417->r=(void*)& Cyc_Absyn_Wild_p_val,
_tmp417)))))));}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,topt,p);{void*t;struct Cyc_Tcpat_TcPatResult
_tmp418;struct Cyc_Tcpat_TcPatResult res=(_tmp418.tvars_and_bounds_opt=0,((_tmp418.patvars=
0,_tmp418)));{void*_tmp95=p->r;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Pat*
_tmp99;struct Cyc_Absyn_Vardecl*_tmp9B;struct Cyc_Absyn_Pat*_tmp9C;struct Cyc_Absyn_Tvar*
_tmp9E;struct Cyc_Absyn_Vardecl*_tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Sign
_tmpA3;enum Cyc_Absyn_Sign _tmpA5;int _tmpA8;struct Cyc_Absyn_Enumdecl*_tmpAA;void*
_tmpAC;struct Cyc_Absyn_Pat*_tmpAF;struct Cyc_List_List*_tmpB1;struct Cyc_List_List**
_tmpB2;int _tmpB3;struct Cyc_Absyn_AggrInfo*_tmpB5;struct Cyc_Absyn_AggrInfo _tmpB6;
union Cyc_Absyn_AggrInfoU _tmpB7;struct Cyc_Absyn_Aggrdecl**_tmpB8;struct Cyc_Absyn_Aggrdecl*
_tmpB9;struct Cyc_List_List*_tmpBA;struct Cyc_List_List**_tmpBB;struct Cyc_List_List*
_tmpBC;struct Cyc_List_List*_tmpBD;struct Cyc_List_List**_tmpBE;int _tmpBF;struct
Cyc_Absyn_Datatypedecl*_tmpC1;struct Cyc_Absyn_Datatypefield*_tmpC2;struct Cyc_List_List*
_tmpC3;struct Cyc_List_List**_tmpC4;int _tmpC5;struct Cyc_Absyn_AggrInfo*_tmpC7;
struct Cyc_Absyn_AggrInfo*_tmpC9;struct Cyc_Absyn_AggrInfo _tmpCA;union Cyc_Absyn_AggrInfoU
_tmpCB;struct _tuple3 _tmpCC;_LL44: {struct Cyc_Absyn_Wild_p_struct*_tmp96=(struct
Cyc_Absyn_Wild_p_struct*)_tmp95;if(_tmp96->tag != 0)goto _LL46;}_LL45: t=Cyc_Tcpat_any_type(
Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL43;_LL46: {struct Cyc_Absyn_Var_p_struct*
_tmp97=(struct Cyc_Absyn_Var_p_struct*)_tmp95;if(_tmp97->tag != 1)goto _LL48;else{
_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL47: res=Cyc_Tcpat_tcPatRec(te,_tmp99,
topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp99->topt))->v;if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){const char*_tmp41B;
void*_tmp41A;(_tmp41A=0,Cyc_Tcutil_terr(p->loc,((_tmp41B="pattern would point to an abstract member",
_tag_dyneither(_tmp41B,sizeof(char),42))),_tag_dyneither(_tmp41A,sizeof(void*),0)));}
Cyc_Tcpat_set_vd(_tmp98,& res.patvars,t);goto _LL43;_LL48: {struct Cyc_Absyn_Reference_p_struct*
_tmp9A=(struct Cyc_Absyn_Reference_p_struct*)_tmp95;if(_tmp9A->tag != 2)goto _LL4A;
else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL49: res=Cyc_Tcpat_tcPatRec(te,
_tmp9C,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->topt))->v;
if(rgn_opt == 0){{const char*_tmp41E;void*_tmp41D;(_tmp41D=0,Cyc_Tcutil_terr(p->loc,((
_tmp41E="* pattern would point into an unknown/unallowed region",_tag_dyneither(
_tmp41E,sizeof(char),55))),_tag_dyneither(_tmp41D,sizeof(void*),0)));}goto _LL43;}
else{if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp421;void*_tmp420;(
_tmp420=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp421="* pattern cannot take the address of an alias-free path",_tag_dyneither(
_tmp421,sizeof(char),56))),_tag_dyneither(_tmp420,sizeof(void*),0)));}}{struct
Cyc_Absyn_PointerType_struct _tmp42B;struct Cyc_Absyn_PtrAtts _tmp42A;struct Cyc_Absyn_PtrInfo
_tmp429;struct Cyc_Absyn_PointerType_struct*_tmp428;Cyc_Tcpat_set_vd(_tmp9B,& res.patvars,(
void*)((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=((_tmp42B.tag=5,((
_tmp42B.f1=((_tmp429.elt_typ=t,((_tmp429.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp429.ptr_atts=((_tmp42A.rgn=*rgn_opt,((_tmp42A.nullable=Cyc_Absyn_false_conref,((
_tmp42A.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp42A.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp42A.ptrloc=0,_tmp42A)))))))))),_tmp429)))))),_tmp42B)))),_tmp428)))));}goto
_LL43;_LL4A: {struct Cyc_Absyn_TagInt_p_struct*_tmp9D=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp95;if(_tmp9D->tag != 3)goto _LL4C;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}
_LL4B: Cyc_Tcpat_set_vd(_tmp9F,& res.patvars,_tmp9F->type);{struct _RegionHandle*
_tmpDA=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Absyn_Tvar*_tmp42C[1];Cyc_Tcenv_add_type_vars(
_tmpDA,p->loc,te,((_tmp42C[0]=_tmp9E,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42C,sizeof(struct Cyc_Absyn_Tvar*),
1)))));}if(res.tvars_and_bounds_opt == 0){struct _tuple13*_tmp42D;res.tvars_and_bounds_opt=((
_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->f1=0,((_tmp42D->f2=0,_tmp42D))))));}{
struct Cyc_List_List*_tmp42E;(*res.tvars_and_bounds_opt).f1=((_tmp42E=_cycalloc(
sizeof(*_tmp42E)),((_tmp42E->hd=_tmp9E,((_tmp42E->tl=(*res.tvars_and_bounds_opt).f1,
_tmp42E))))));}t=Cyc_Absyn_uint_typ;goto _LL43;};_LL4C: {struct Cyc_Absyn_Int_p_struct*
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
_tmpA7=(struct Cyc_Absyn_Float_p_struct*)_tmp95;if(_tmpA7->tag != 11)goto _LL56;
else{_tmpA8=_tmpA7->f2;}}_LL55: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(
_tmpA8));goto _LL43;_LL56: {struct Cyc_Absyn_Enum_p_struct*_tmpA9=(struct Cyc_Absyn_Enum_p_struct*)
_tmp95;if(_tmpA9->tag != 12)goto _LL58;else{_tmpAA=_tmpA9->f1;}}_LL57:{struct Cyc_Absyn_EnumType_struct
_tmp431;struct Cyc_Absyn_EnumType_struct*_tmp430;t=Cyc_Tcpat_num_type(topt,(void*)((
_tmp430=_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=((_tmp431.tag=13,((_tmp431.f1=
_tmpAA->name,((_tmp431.f2=(struct Cyc_Absyn_Enumdecl*)_tmpAA,_tmp431)))))),
_tmp430)))));}goto _LL43;_LL58: {struct Cyc_Absyn_AnonEnum_p_struct*_tmpAB=(struct
Cyc_Absyn_AnonEnum_p_struct*)_tmp95;if(_tmpAB->tag != 13)goto _LL5A;else{_tmpAC=(
void*)_tmpAB->f1;}}_LL59: t=Cyc_Tcpat_num_type(topt,_tmpAC);goto _LL43;_LL5A: {
struct Cyc_Absyn_Null_p_struct*_tmpAD=(struct Cyc_Absyn_Null_p_struct*)_tmp95;if(
_tmpAD->tag != 8)goto _LL5C;}_LL5B: if(topt != 0){void*_tmpE0=Cyc_Tcutil_compress(*
topt);_LL6F: {struct Cyc_Absyn_PointerType_struct*_tmpE1=(struct Cyc_Absyn_PointerType_struct*)
_tmpE0;if(_tmpE1->tag != 5)goto _LL71;}_LL70: t=*topt;goto tcpat_end;_LL71:;_LL72:
goto _LL6E;_LL6E:;}{struct Cyc_Core_Opt*_tmpE2=Cyc_Tcenv_lookup_opt_type_vars(te);{
struct Cyc_Absyn_PointerType_struct _tmp43B;struct Cyc_Absyn_PtrAtts _tmp43A;struct
Cyc_Absyn_PtrInfo _tmp439;struct Cyc_Absyn_PointerType_struct*_tmp438;t=(void*)((
_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp43B.tag=5,((_tmp43B.f1=((
_tmp439.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmpE2),((
_tmp439.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp439.ptr_atts=((_tmp43A.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmpE2),((_tmp43A.nullable=Cyc_Absyn_true_conref,((
_tmp43A.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp43A.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp43A.ptrloc=0,_tmp43A)))))))))),_tmp439)))))),_tmp43B)))),_tmp438))));}goto
_LL43;};_LL5C: {struct Cyc_Absyn_Pointer_p_struct*_tmpAE=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp95;if(_tmpAE->tag != 5)goto _LL5E;else{_tmpAF=_tmpAE->f1;}}_LL5D: {void*
inner_typ=(void*)& Cyc_Absyn_VoidType_val;void**_tmpE7=0;int elt_const=0;if(topt != 
0){void*_tmpE8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmpEA;void*
_tmpEB;struct Cyc_Absyn_Tqual _tmpEC;_LL74: {struct Cyc_Absyn_PointerType_struct*
_tmpE9=(struct Cyc_Absyn_PointerType_struct*)_tmpE8;if(_tmpE9->tag != 5)goto _LL76;
else{_tmpEA=_tmpE9->f1;_tmpEB=_tmpEA.elt_typ;_tmpEC=_tmpEA.elt_tq;}}_LL75:
inner_typ=_tmpEB;_tmpE7=(void**)& inner_typ;elt_const=_tmpEC.real_const;goto _LL73;
_LL76:;_LL77: goto _LL73;_LL73:;}{void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmpAF,_tmpE7,(void**)& ptr_rgn));{void*_tmpED=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAF->topt))->v);struct Cyc_Absyn_DatatypeFieldInfo
_tmpEF;union Cyc_Absyn_DatatypeFieldInfoU _tmpF0;struct _tuple2 _tmpF1;struct Cyc_Absyn_Datatypedecl*
_tmpF2;struct Cyc_Absyn_Datatypefield*_tmpF3;struct Cyc_List_List*_tmpF4;_LL79: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmpEE=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpED;if(_tmpEE->tag != 4)goto _LL7B;else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEF.field_info;
if((_tmpF0.KnownDatatypefield).tag != 2)goto _LL7B;_tmpF1=(struct _tuple2)(_tmpF0.KnownDatatypefield).val;
_tmpF2=_tmpF1.f1;_tmpF3=_tmpF1.f2;_tmpF4=_tmpEF.targs;}}_LL7A:{void*_tmpF5=Cyc_Tcutil_compress(
inner_typ);_LL7E: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpF6=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF5;if(_tmpF6->tag != 4)goto _LL80;}_LL7F: goto DONT_PROMOTE;_LL80:;_LL81: goto
_LL7D;_LL7D:;}{struct Cyc_Absyn_DatatypeType_struct _tmp445;struct Cyc_Absyn_Datatypedecl**
_tmp444;struct Cyc_Absyn_DatatypeInfo _tmp443;struct Cyc_Absyn_DatatypeType_struct*
_tmp442;struct Cyc_Absyn_DatatypeType_struct*_tmpF7=(_tmp442=_cycalloc(sizeof(*
_tmp442)),((_tmp442[0]=((_tmp445.tag=3,((_tmp445.f1=((_tmp443.datatype_info=Cyc_Absyn_KnownDatatype(((
_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=_tmpF2,_tmp444))))),((_tmp443.targs=
_tmpF4,_tmp443)))),_tmp445)))),_tmp442)));((struct Cyc_Core_Opt*)_check_null(
_tmpAF->topt))->v=(void*)((void*)_tmpF7);{struct Cyc_Absyn_PointerType_struct
_tmp44F;struct Cyc_Absyn_PtrAtts _tmp44E;struct Cyc_Absyn_PtrInfo _tmp44D;struct Cyc_Absyn_PointerType_struct*
_tmp44C;t=(void*)((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C[0]=((_tmp44F.tag=
5,((_tmp44F.f1=((_tmp44D.elt_typ=(void*)_tmpF7,((_tmp44D.elt_tq=elt_const?Cyc_Absyn_const_tqual(
0): Cyc_Absyn_empty_tqual(0),((_tmp44D.ptr_atts=((_tmp44E.rgn=ptr_rgn,((_tmp44E.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp44E.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp44E.zero_term=Cyc_Absyn_false_conref,((_tmp44E.ptrloc=0,_tmp44E)))))))))),
_tmp44D)))))),_tmp44F)))),_tmp44C))));}goto _LL78;};_LL7B:;_LL7C: DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_struct _tmp459;struct Cyc_Absyn_PtrAtts _tmp458;struct
Cyc_Absyn_PtrInfo _tmp457;struct Cyc_Absyn_PointerType_struct*_tmp456;t=(void*)((
_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456[0]=((_tmp459.tag=5,((_tmp459.f1=((
_tmp457.elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAF->topt))->v,((
_tmp457.elt_tq=elt_const?Cyc_Absyn_const_tqual(0): Cyc_Absyn_empty_tqual(0),((
_tmp457.ptr_atts=((_tmp458.rgn=ptr_rgn,((_tmp458.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp458.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp458.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp458.ptrloc=0,_tmp458)))))))))),_tmp457)))))),_tmp459)))),_tmp456))));}_LL78:;}
goto _LL43;};}_LL5E: {struct Cyc_Absyn_Tuple_p_struct*_tmpB0=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp95;if(_tmpB0->tag != 4)goto _LL60;else{_tmpB1=_tmpB0->f1;_tmpB2=(struct Cyc_List_List**)&
_tmpB0->f1;_tmpB3=_tmpB0->f2;}}_LL5F: {struct Cyc_List_List*_tmp104=*_tmpB2;
struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*
_tmp105=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp107;_LL83: {struct Cyc_Absyn_TupleType_struct*
_tmp106=(struct Cyc_Absyn_TupleType_struct*)_tmp105;if(_tmp106->tag != 10)goto
_LL85;else{_tmp107=_tmp106->f1;}}_LL84: topt_ts=_tmp107;if(_tmpB3){int _tmp108=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);int _tmp109=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp107);if(_tmp108 < _tmp109){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp109 - _tmp108;++ i){struct Cyc_List_List*_tmp45A;
wild_ps=((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp45A->tl=wild_ps,_tmp45A))))));}}*_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp104,wild_ps);_tmp104=*_tmpB2;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp107)){const char*_tmp45D;void*_tmp45C;(
_tmp45C=0,Cyc_Tcutil_warn(p->loc,((_tmp45D="unnecessary ... in tuple pattern",
_tag_dyneither(_tmp45D,sizeof(char),33))),_tag_dyneither(_tmp45C,sizeof(void*),0)));}}}
goto _LL82;_LL85:;_LL86: goto _LL82;_LL82:;}else{if(_tmpB3){const char*_tmp460;void*
_tmp45F;(_tmp45F=0,Cyc_Tcutil_terr(p->loc,((_tmp460="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp460,sizeof(char),57))),_tag_dyneither(_tmp45F,sizeof(void*),0)));}}
for(0;_tmp104 != 0;_tmp104=_tmp104->tl){void**_tmp10F=0;if(topt_ts != 0){_tmp10F=(
void**)&(*((struct _tuple16*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp104->hd,_tmp10F,rgn_opt));{
struct _tuple16*_tmp463;struct Cyc_List_List*_tmp462;pat_ts=((_tmp462=_cycalloc(
sizeof(*_tmp462)),((_tmp462->hd=((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463->f1=
Cyc_Absyn_empty_tqual(0),((_tmp463->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmp104->hd)->topt))->v,_tmp463)))))),((_tmp462->tl=pat_ts,
_tmp462))))));};}{struct Cyc_Absyn_TupleType_struct _tmp466;struct Cyc_Absyn_TupleType_struct*
_tmp465;t=(void*)((_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465[0]=((_tmp466.tag=
10,((_tmp466.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp466)))),_tmp465))));}goto _LL43;}_LL60: {struct Cyc_Absyn_Aggr_p_struct*
_tmpB4=(struct Cyc_Absyn_Aggr_p_struct*)_tmp95;if(_tmpB4->tag != 6)goto _LL62;else{
_tmpB5=_tmpB4->f1;if(_tmpB5 == 0)goto _LL62;_tmpB6=*_tmpB5;_tmpB7=_tmpB6.aggr_info;
if((_tmpB7.KnownAggr).tag != 2)goto _LL62;_tmpB8=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB7.KnownAggr).val;_tmpB9=*_tmpB8;_tmpBA=_tmpB6.targs;_tmpBB=(struct Cyc_List_List**)&(*
_tmpB4->f1).targs;_tmpBC=_tmpB4->f2;_tmpBD=_tmpB4->f3;_tmpBE=(struct Cyc_List_List**)&
_tmpB4->f3;_tmpBF=_tmpB4->f4;}}_LL61: {struct Cyc_List_List*_tmp114=*_tmpBE;const
char*_tmp468;const char*_tmp467;struct _dyneither_ptr aggr_str=_tmpB9->kind == Cyc_Absyn_StructA?(
_tmp468="struct",_tag_dyneither(_tmp468,sizeof(char),7)):((_tmp467="union",
_tag_dyneither(_tmp467,sizeof(char),6)));if(_tmpB9->impl == 0){{const char*_tmp46C;
void*_tmp46B[1];struct Cyc_String_pa_struct _tmp46A;(_tmp46A.tag=0,((_tmp46A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp46B[0]=& _tmp46A,Cyc_Tcutil_terr(
p->loc,((_tmp46C="can't destructure an abstract %s",_tag_dyneither(_tmp46C,
sizeof(char),33))),_tag_dyneither(_tmp46B,sizeof(void*),1)))))));}t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;}if(_tmpBC != 0  || _tmpB9->kind == 
Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->tagged)
rgn_opt=0;{struct _RegionHandle _tmp118=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp118;_push_region(rgn);{struct Cyc_List_List*_tmp119=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmp11A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB9->impl))->exist_vars;{struct Cyc_List_List*t=_tmpBC;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp11A))->hd;
_tmp11A=_tmp11A->tl;{void*_tmp11B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp11C=
Cyc_Absyn_compress_kb(uv->kind);int error=0;struct Cyc_Absyn_Kind*k2;{void*_tmp11D=
_tmp11C;struct Cyc_Absyn_Kind*_tmp11F;struct Cyc_Absyn_Kind*_tmp121;_LL88: {struct
Cyc_Absyn_Less_kb_struct*_tmp11E=(struct Cyc_Absyn_Less_kb_struct*)_tmp11D;if(
_tmp11E->tag != 2)goto _LL8A;else{_tmp11F=_tmp11E->f2;}}_LL89: _tmp121=_tmp11F;goto
_LL8B;_LL8A: {struct Cyc_Absyn_Eq_kb_struct*_tmp120=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp11D;if(_tmp120->tag != 0)goto _LL8C;else{_tmp121=_tmp120->f1;}}_LL8B: k2=
_tmp121;goto _LL87;_LL8C:;_LL8D: {const char*_tmp46F;void*_tmp46E;(_tmp46E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp46F="unconstrained existential type variable in aggregate",
_tag_dyneither(_tmp46F,sizeof(char),53))),_tag_dyneither(_tmp46E,sizeof(void*),0)));}
_LL87:;}{void*_tmp124=_tmp11B;struct Cyc_Absyn_Kind*_tmp126;struct Cyc_Core_Opt*
_tmp128;struct Cyc_Core_Opt**_tmp129;struct Cyc_Absyn_Kind*_tmp12A;struct Cyc_Core_Opt*
_tmp12C;struct Cyc_Core_Opt**_tmp12D;_LL8F: {struct Cyc_Absyn_Eq_kb_struct*_tmp125=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp124;if(_tmp125->tag != 0)goto _LL91;else{_tmp126=
_tmp125->f1;}}_LL90: if(!Cyc_Tcutil_kind_leq(k2,_tmp126))error=1;goto _LL8E;_LL91: {
struct Cyc_Absyn_Less_kb_struct*_tmp127=(struct Cyc_Absyn_Less_kb_struct*)_tmp124;
if(_tmp127->tag != 2)goto _LL93;else{_tmp128=_tmp127->f1;_tmp129=(struct Cyc_Core_Opt**)&
_tmp127->f1;_tmp12A=_tmp127->f2;}}_LL92: _tmp12D=_tmp129;goto _LL94;_LL93: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp12B=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp124;
if(_tmp12B->tag != 1)goto _LL8E;else{_tmp12C=_tmp12B->f1;_tmp12D=(struct Cyc_Core_Opt**)&
_tmp12B->f1;}}_LL94:{struct Cyc_Core_Opt*_tmp470;*_tmp12D=((_tmp470=_cycalloc(
sizeof(*_tmp470)),((_tmp470->v=_tmp11C,_tmp470))));}goto _LL8E;_LL8E:;}if(error){
const char*_tmp476;void*_tmp475[3];struct Cyc_String_pa_struct _tmp474;struct Cyc_String_pa_struct
_tmp473;struct Cyc_String_pa_struct _tmp472;(_tmp472.tag=0,((_tmp472.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(k2)),((_tmp473.tag=
0,((_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
_tmp11B)),((_tmp474.tag=0,((_tmp474.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*tv->name),((_tmp475[0]=& _tmp474,((_tmp475[1]=& _tmp473,((_tmp475[
2]=& _tmp472,Cyc_Tcutil_terr(p->loc,((_tmp476="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp476,sizeof(char),60))),_tag_dyneither(_tmp475,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_Absyn_VarType_struct _tmp479;struct Cyc_Absyn_VarType_struct*_tmp478;
void*vartype=(void*)((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478[0]=((_tmp479.tag=
2,((_tmp479.f1=tv,_tmp479)))),_tmp478))));{struct Cyc_List_List*_tmp47A;_tmp119=((
_tmp47A=_region_malloc(rgn,sizeof(*_tmp47A)),((_tmp47A->hd=(void*)vartype,((
_tmp47A->tl=_tmp119,_tmp47A))))));}if(k2->kind == Cyc_Absyn_RgnKind){if(k2->aliasqual
== Cyc_Absyn_Aliasable){struct _tuple0*_tmp47D;struct Cyc_List_List*_tmp47C;
outlives_constraints=((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->hd=((
_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->f1=Cyc_Absyn_empty_effect,((
_tmp47D->f2=vartype,_tmp47D)))))),((_tmp47C->tl=outlives_constraints,_tmp47C))))));}
else{const char*_tmp480;void*_tmp47F;(_tmp47F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp480="opened existential had unique or top region kind",
_tag_dyneither(_tmp480,sizeof(char),49))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}}};};}}
_tmp119=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp119);{struct _RegionHandle*_tmp13B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_add_type_vars(_tmp13B,p->loc,te,_tmpBC);struct Cyc_List_List*
_tmp13C=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple11 _tmp481;struct _tuple11
_tmp13D=(_tmp481.f1=_tmp13C,((_tmp481.f2=rgn,_tmp481)));struct Cyc_List_List*
_tmp13E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(
struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13D,_tmpB9->tvs);struct Cyc_List_List*
_tmp13F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB9->impl))->exist_vars,_tmp119);struct Cyc_List_List*_tmp140=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp13E);struct Cyc_List_List*_tmp141=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp13F);struct Cyc_List_List*_tmp142=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmp13E,_tmp13F);if(_tmpBC != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB9->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0){struct
_tuple13*_tmp482;res.tvars_and_bounds_opt=((_tmp482=_cycalloc(sizeof(*_tmp482)),((
_tmp482->f1=0,((_tmp482->f2=0,_tmp482))))));}(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmpBC);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp144=0;{struct Cyc_List_List*
_tmp145=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->rgn_po;for(0;
_tmp145 != 0;_tmp145=_tmp145->tl){struct _tuple0*_tmp485;struct Cyc_List_List*
_tmp484;_tmp144=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->hd=((_tmp485=
_cycalloc(sizeof(*_tmp485)),((_tmp485->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp142,(*((
struct _tuple0*)_tmp145->hd)).f1),((_tmp485->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp142,(*((struct _tuple0*)_tmp145->hd)).f2),_tmp485)))))),((_tmp484->tl=_tmp144,
_tmp484))))));}}_tmp144=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp144);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp144);};}*
_tmpBB=_tmp140;{struct Cyc_Absyn_AggrType_struct _tmp48F;struct Cyc_Absyn_Aggrdecl**
_tmp48E;struct Cyc_Absyn_AggrInfo _tmp48D;struct Cyc_Absyn_AggrType_struct*_tmp48C;
t=(void*)((_tmp48C=_cycalloc(sizeof(*_tmp48C)),((_tmp48C[0]=((_tmp48F.tag=11,((
_tmp48F.f1=((_tmp48D.aggr_info=Cyc_Absyn_KnownAggr(((_tmp48E=_cycalloc(sizeof(*
_tmp48E)),((_tmp48E[0]=_tmpB9,_tmp48E))))),((_tmp48D.targs=*_tmpBB,_tmp48D)))),
_tmp48F)))),_tmp48C))));}if(_tmpBF  && _tmpB9->kind == Cyc_Absyn_UnionA){const char*
_tmp492;void*_tmp491;(_tmp491=0,Cyc_Tcutil_warn(p->loc,((_tmp492="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp492,sizeof(char),43))),_tag_dyneither(_tmp491,sizeof(void*),0)));}
else{if(_tmpBF){int _tmp14E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp114);int _tmp14F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB9->impl))->fields);if(_tmp14E < _tmp14F){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp14F - _tmp14E;++ i){struct _tuple15*_tmp495;struct
Cyc_List_List*_tmp494;wild_dps=((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494->hd=((
_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495->f1=0,((_tmp495->f2=Cyc_Tcpat_wild_pat(
p->loc),_tmp495)))))),((_tmp494->tl=wild_dps,_tmp494))))));}}*_tmpBE=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp114,
wild_dps);_tmp114=*_tmpBE;}else{if(_tmp14E == _tmp14F){const char*_tmp498;void*
_tmp497;(_tmp497=0,Cyc_Tcutil_warn(p->loc,((_tmp498="unnecessary ... in struct pattern",
_tag_dyneither(_tmp498,sizeof(char),34))),_tag_dyneither(_tmp497,sizeof(void*),0)));}}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,
_tmp114,_tmpB9->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB9->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple17 _tmp155;struct Cyc_Absyn_Aggrfield*
_tmp156;struct Cyc_Absyn_Pat*_tmp157;struct _tuple17*_tmp154=(struct _tuple17*)
fields->hd;_tmp155=*_tmp154;_tmp156=_tmp155.f1;_tmp157=_tmp155.f2;{void*_tmp158=
Cyc_Tcutil_rsubstitute(rgn,_tmp142,_tmp156->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp157,(void**)& _tmp158,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp157->topt))->v,_tmp158)){const char*
_tmp49F;void*_tmp49E[4];struct Cyc_String_pa_struct _tmp49D;struct Cyc_String_pa_struct
_tmp49C;struct Cyc_String_pa_struct _tmp49B;struct Cyc_String_pa_struct _tmp49A;(
_tmp49A.tag=0,((_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp157->topt))->v)),((_tmp49B.tag=0,((
_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp158)),((_tmp49C.tag=0,((_tmp49C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp49D.tag=0,((_tmp49D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp156->name),((_tmp49E[0]=& _tmp49D,((_tmp49E[1]=& _tmp49C,((
_tmp49E[2]=& _tmp49B,((_tmp49E[3]=& _tmp49A,Cyc_Tcutil_terr(p->loc,((_tmp49F="field %s of %s pattern expects type %s != %s",
_tag_dyneither(_tmp49F,sizeof(char),45))),_tag_dyneither(_tmp49E,sizeof(void*),4)))))))))))))))))))))))));}};}};};}
_npop_handler(0);goto _LL43;;_pop_region(rgn);};}_LL62: {struct Cyc_Absyn_Datatype_p_struct*
_tmpC0=(struct Cyc_Absyn_Datatype_p_struct*)_tmp95;if(_tmpC0->tag != 7)goto _LL64;
else{_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;_tmpC3=_tmpC0->f3;_tmpC4=(struct Cyc_List_List**)&
_tmpC0->f3;_tmpC5=_tmpC0->f4;}}_LL63: {struct Cyc_List_List*_tmp162=*_tmpC4;
struct _RegionHandle*_tmp163=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*tqts=
_tmpC2->typs;struct Cyc_List_List*_tmp164=Cyc_Tcenv_lookup_type_vars(te);struct
_tuple11 _tmp4A0;struct _tuple11 _tmp165=(_tmp4A0.f1=_tmp164,((_tmp4A0.f2=_tmp163,
_tmp4A0)));struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct
_tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp163,Cyc_Tcutil_r_make_inst_var,&
_tmp165,_tmpC1->tvs);struct Cyc_List_List*_tmp167=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple12*))Cyc_Core_snd,_tmp166);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmp4A6;struct Cyc_Absyn_DatatypeFieldInfo _tmp4A5;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4A4;t=(void*)((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A6.tag=
4,((_tmp4A6.f1=((_tmp4A5.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC1,_tmpC2),((
_tmp4A5.targs=_tmp167,_tmp4A5)))),_tmp4A6)))),_tmp4A4))));}if(_tmpC5){int _tmp16B=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp162);int _tmp16C=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp16B < _tmp16C){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp16C - _tmp16B;++ i){struct Cyc_List_List*_tmp4A7;
wild_ps=((_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((_tmp4A7->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp4A7->tl=wild_ps,_tmp4A7))))));}}*_tmpC4=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp162,wild_ps);_tmp162=*_tmpC4;}
else{if(_tmp16B == _tmp16C){const char*_tmp4AB;void*_tmp4AA[1];struct Cyc_String_pa_struct
_tmp4A9;(_tmp4A9.tag=0,((_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4AA[0]=& _tmp4A9,Cyc_Tcutil_warn(p->loc,((
_tmp4AB="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4AB,sizeof(
char),37))),_tag_dyneither(_tmp4AA,sizeof(void*),1)))))));}}}for(0;_tmp162 != 0
 && tqts != 0;(_tmp162=_tmp162->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp171=(
struct Cyc_Absyn_Pat*)_tmp162->hd;void*_tmp172=Cyc_Tcutil_rsubstitute(_tmp163,
_tmp166,(*((struct _tuple16*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp171,(void**)& _tmp172,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp171->topt))->v,_tmp172)){const char*_tmp4B1;void*_tmp4B0[3];
struct Cyc_String_pa_struct _tmp4AF;struct Cyc_String_pa_struct _tmp4AE;struct Cyc_String_pa_struct
_tmp4AD;(_tmp4AD.tag=0,((_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp171->topt))->v)),((
_tmp4AE.tag=0,((_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp172)),((_tmp4AF.tag=0,((_tmp4AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC2->name)),((_tmp4B0[0]=& _tmp4AF,((
_tmp4B0[1]=& _tmp4AE,((_tmp4B0[2]=& _tmp4AD,Cyc_Tcutil_terr(_tmp171->loc,((_tmp4B1="%s expects argument type %s, not %s",
_tag_dyneither(_tmp4B1,sizeof(char),36))),_tag_dyneither(_tmp4B0,sizeof(void*),3)))))))))))))))))));}}
if(_tmp162 != 0){const char*_tmp4B5;void*_tmp4B4[1];struct Cyc_String_pa_struct
_tmp4B3;(_tmp4B3.tag=0,((_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC2->name)),((_tmp4B4[0]=& _tmp4B3,Cyc_Tcutil_terr(p->loc,((
_tmp4B5="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4B5,
sizeof(char),47))),_tag_dyneither(_tmp4B4,sizeof(void*),1)))))));}if(tqts != 0){
const char*_tmp4B9;void*_tmp4B8[1];struct Cyc_String_pa_struct _tmp4B7;(_tmp4B7.tag=
0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC2->name)),((_tmp4B8[0]=& _tmp4B7,Cyc_Tcutil_terr(p->loc,((_tmp4B9="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp4B9,sizeof(char),46))),_tag_dyneither(_tmp4B8,sizeof(void*),1)))))));}}
goto _LL43;}_LL64: {struct Cyc_Absyn_Aggr_p_struct*_tmpC6=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp95;if(_tmpC6->tag != 6)goto _LL66;else{_tmpC7=_tmpC6->f1;if(_tmpC7 != 0)goto
_LL66;}}_LL65: goto _LL67;_LL66: {struct Cyc_Absyn_Aggr_p_struct*_tmpC8=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp95;if(_tmpC8->tag != 6)goto _LL68;else{_tmpC9=_tmpC8->f1;if(_tmpC9 == 0)goto
_LL68;_tmpCA=*_tmpC9;_tmpCB=_tmpCA.aggr_info;if((_tmpCB.UnknownAggr).tag != 1)
goto _LL68;_tmpCC=(struct _tuple3)(_tmpCB.UnknownAggr).val;}}_LL67: goto _LL69;_LL68: {
struct Cyc_Absyn_UnknownId_p_struct*_tmpCD=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp95;if(_tmpCD->tag != 14)goto _LL6A;}_LL69: goto _LL6B;_LL6A: {struct Cyc_Absyn_Exp_p_struct*
_tmpCE=(struct Cyc_Absyn_Exp_p_struct*)_tmp95;if(_tmpCE->tag != 16)goto _LL6C;}
_LL6B: goto _LL6D;_LL6C: {struct Cyc_Absyn_UnknownCall_p_struct*_tmpCF=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp95;if(_tmpCF->tag != 15)goto _LL43;}_LL6D: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL43;_LL43:;}tcpat_end: {struct Cyc_Core_Opt*_tmp4BA;p->topt=((_tmp4BA=
_cycalloc(sizeof(*_tmp4BA)),((_tmp4BA->v=t,_tmp4BA))));}return res;};}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp181=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct
_RegionHandle*_tmp182=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp4BB;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp182,Cyc_Tcpat_get_name,_tmp181.patvars),
p->loc,((_tmp4BB="pattern contains a repeated variable",_tag_dyneither(_tmp4BB,
sizeof(char),37))));}return _tmp181;}void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p){void*_tmp184=p->r;struct Cyc_Absyn_Pat*_tmp186;struct
Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp18A;struct Cyc_List_List*_tmp18C;
_LL96: {struct Cyc_Absyn_Pointer_p_struct*_tmp185=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp184;if(_tmp185->tag != 5)goto _LL98;else{_tmp186=_tmp185->f1;}}_LL97: Cyc_Tcpat_check_pat_regions(
te,_tmp186);{struct _RegionHandle*_tmp18D=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp18D,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp186->topt))->v)){{const char*
_tmp4BE;void*_tmp4BD;(_tmp4BD=0,Cyc_Tcutil_terr(p->loc,((_tmp4BE="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp4BE,sizeof(char),56))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}
return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp18D,(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){{const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,Cyc_Tcutil_terr(
p->loc,((_tmp4C1="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp4C1,sizeof(char),53))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}
return;}{void*_tmp192=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp194;struct Cyc_Absyn_PtrAtts _tmp195;void*_tmp196;_LLA1: {
struct Cyc_Absyn_PointerType_struct*_tmp193=(struct Cyc_Absyn_PointerType_struct*)
_tmp192;if(_tmp193->tag != 5)goto _LLA3;else{_tmp194=_tmp193->f1;_tmp195=_tmp194.ptr_atts;
_tmp196=_tmp195.rgn;}}_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp196);
return;_LLA3:;_LLA4: {const char*_tmp4C4;void*_tmp4C3;(_tmp4C3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C4="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp4C4,sizeof(char),36))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}
_LLA0:;};};_LL98: {struct Cyc_Absyn_Aggr_p_struct*_tmp187=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp184;if(_tmp187->tag != 6)goto _LL9A;else{_tmp188=_tmp187->f3;}}_LL99: for(0;
_tmp188 != 0;_tmp188=_tmp188->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple15*)_tmp188->hd)).f2);}return;_LL9A: {struct Cyc_Absyn_Datatype_p_struct*
_tmp189=(struct Cyc_Absyn_Datatype_p_struct*)_tmp184;if(_tmp189->tag != 7)goto
_LL9C;else{_tmp18A=_tmp189->f3;}}_LL9B: _tmp18C=_tmp18A;goto _LL9D;_LL9C: {struct
Cyc_Absyn_Tuple_p_struct*_tmp18B=(struct Cyc_Absyn_Tuple_p_struct*)_tmp184;if(
_tmp18B->tag != 4)goto _LL9E;else{_tmp18C=_tmp18B->f1;}}_LL9D: for(0;_tmp18C != 0;
_tmp18C=_tmp18C->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp18C->hd);}return;_LL9E:;_LL9F: return;_LL95:;}struct _union_Name_value_Name_v{
int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};
union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct
_union_Name_value_Int_v Int_v;};typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value
_tmp4C5;return((_tmp4C5.Name_v).val=s,(((_tmp4C5.Name_v).tag=1,_tmp4C5)));}union
Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp4C6;
return((_tmp4C6.Int_v).val=i,(((_tmp4C6.Int_v).tag=2,_tmp4C6)));}struct Cyc_Tcpat_Con_s{
union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_struct{int tag;};
struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};typedef void*Cyc_Tcpat_simple_pat_t;static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value _tmp19B=c1->name;struct
_dyneither_ptr _tmp19C;int _tmp19D;_LLA6: if((_tmp19B.Name_v).tag != 1)goto _LLA8;
_tmp19C=(struct _dyneither_ptr)(_tmp19B.Name_v).val;_LLA7: {union Cyc_Tcpat_Name_value
_tmp19E=c2->name;struct _dyneither_ptr _tmp19F;int _tmp1A0;_LLAB: if((_tmp19E.Name_v).tag
!= 1)goto _LLAD;_tmp19F=(struct _dyneither_ptr)(_tmp19E.Name_v).val;_LLAC: return
Cyc_strcmp((struct _dyneither_ptr)_tmp19C,(struct _dyneither_ptr)_tmp19F);_LLAD:
if((_tmp19E.Int_v).tag != 2)goto _LLAA;_tmp1A0=(int)(_tmp19E.Int_v).val;_LLAE:
return - 1;_LLAA:;}_LLA8: if((_tmp19B.Int_v).tag != 2)goto _LLA5;_tmp19D=(int)(
_tmp19B.Int_v).val;_LLA9: {union Cyc_Tcpat_Name_value _tmp1A1=c2->name;struct
_dyneither_ptr _tmp1A2;int _tmp1A3;_LLB0: if((_tmp1A1.Name_v).tag != 1)goto _LLB2;
_tmp1A2=(struct _dyneither_ptr)(_tmp1A1.Name_v).val;_LLB1: return 1;_LLB2: if((
_tmp1A1.Int_v).tag != 2)goto _LLAF;_tmp1A3=(int)(_tmp1A1.Int_v).val;_LLB3: return
_tmp19D - _tmp1A3;_LLAF:;}_LLA5:;}static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(
struct _RegionHandle*r){return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*
cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
static int Cyc_Tcpat_one_opt=1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=
256;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){const char*_tmp4C9;struct Cyc_Tcpat_Con_s*_tmp4C8;return(_tmp4C8=
_region_malloc(r,sizeof(*_tmp4C8)),((_tmp4C8->name=Cyc_Tcpat_Name_v(((_tmp4C9="NULL",
_tag_dyneither(_tmp4C9,sizeof(char),5)))),((_tmp4C8->arity=0,((_tmp4C8->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4C8->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C8)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){const char*_tmp4CC;struct Cyc_Tcpat_Con_s*_tmp4CB;return(_tmp4CB=
_region_malloc(r,sizeof(*_tmp4CB)),((_tmp4CB->name=Cyc_Tcpat_Name_v(((_tmp4CC="&",
_tag_dyneither(_tmp4CC,sizeof(char),2)))),((_tmp4CB->arity=1,((_tmp4CB->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4CB->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4CB)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){const char*_tmp4CF;struct Cyc_Tcpat_Con_s*_tmp4CE;return(_tmp4CE=_region_malloc(
r,sizeof(*_tmp4CE)),((_tmp4CE->name=Cyc_Tcpat_Name_v(((_tmp4CF="&",
_tag_dyneither(_tmp4CF,sizeof(char),2)))),((_tmp4CE->arity=1,((_tmp4CE->span=(
int*)& Cyc_Tcpat_one_opt,((_tmp4CE->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4CE)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4D0;return(_tmp4D0=_region_malloc(r,
sizeof(*_tmp4D0)),((_tmp4D0->name=Cyc_Tcpat_Int_v(i),((_tmp4D0->arity=0,((
_tmp4D0->span=0,((_tmp4D0->orig_pat=p,_tmp4D0)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4D1;return(_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1)),((
_tmp4D1->name=Cyc_Tcpat_Name_v(f),((_tmp4D1->arity=0,((_tmp4D1->span=0,((_tmp4D1->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp4D1)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4D2;
return(_tmp4D2=_region_malloc(r,sizeof(*_tmp4D2)),((_tmp4D2->name=Cyc_Tcpat_Int_v((
int)c),((_tmp4D2->arity=0,((_tmp4D2->span=(int*)& Cyc_Tcpat_twofiftysix_opt,((
_tmp4D2->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4D2)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){const char*
_tmp4D5;struct Cyc_Tcpat_Con_s*_tmp4D4;return(_tmp4D4=_region_malloc(r,sizeof(*
_tmp4D4)),((_tmp4D4->name=Cyc_Tcpat_Name_v(((_tmp4D5="$",_tag_dyneither(_tmp4D5,
sizeof(char),2)))),((_tmp4D4->arity=i,((_tmp4D4->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp4D4->orig_pat=p,_tmp4D4)))))))));}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4D8;struct
Cyc_Tcpat_Con_struct*_tmp4D7;return(void*)((_tmp4D7=_region_malloc(r,sizeof(*
_tmp4D7)),((_tmp4D7[0]=((_tmp4D8.tag=1,((_tmp4D8.f1=Cyc_Tcpat_null_con(r,p),((
_tmp4D8.f2=0,_tmp4D8)))))),_tmp4D7))));}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4DB;
struct Cyc_Tcpat_Con_struct*_tmp4DA;return(void*)((_tmp4DA=_region_malloc(r,
sizeof(*_tmp4DA)),((_tmp4DA[0]=((_tmp4DB.tag=1,((_tmp4DB.f1=Cyc_Tcpat_int_con(r,
i,p),((_tmp4DB.f2=0,_tmp4DB)))))),_tmp4DA))));}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4DE;struct Cyc_Tcpat_Con_struct*_tmp4DD;return(void*)((_tmp4DD=_region_malloc(
r,sizeof(*_tmp4DD)),((_tmp4DD[0]=((_tmp4DE.tag=1,((_tmp4DE.f1=Cyc_Tcpat_char_con(
r,c,p),((_tmp4DE.f2=0,_tmp4DE)))))),_tmp4DD))));}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4E1;struct Cyc_Tcpat_Con_struct*_tmp4E0;return(void*)((_tmp4E0=_region_malloc(
r,sizeof(*_tmp4E0)),((_tmp4E0[0]=((_tmp4E1.tag=1,((_tmp4E1.f1=Cyc_Tcpat_float_con(
r,f,p),((_tmp4E1.f2=0,_tmp4E1)))))),_tmp4E0))));}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_struct
_tmp4E7;struct Cyc_List_List*_tmp4E6;struct Cyc_Tcpat_Con_struct*_tmp4E5;return(
void*)((_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5)),((_tmp4E5[0]=((_tmp4E7.tag=1,((
_tmp4E7.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4E7.f2=((_tmp4E6=_region_malloc(r,
sizeof(*_tmp4E6)),((_tmp4E6->hd=p,((_tmp4E6->tl=0,_tmp4E6)))))),_tmp4E7)))))),
_tmp4E5))));}static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){struct Cyc_Tcpat_Con_struct _tmp4ED;struct Cyc_List_List*_tmp4EC;struct Cyc_Tcpat_Con_struct*
_tmp4EB;return(void*)((_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB)),((_tmp4EB[0]=((
_tmp4ED.tag=1,((_tmp4ED.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4ED.f2=((_tmp4EC=
_region_malloc(r,sizeof(*_tmp4EC)),((_tmp4EC->hd=p,((_tmp4EC->tl=0,_tmp4EC)))))),
_tmp4ED)))))),_tmp4EB))));}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4F0;
struct Cyc_Tcpat_Con_struct*_tmp4EF;return(void*)((_tmp4EF=_region_malloc(r,
sizeof(*_tmp4EF)),((_tmp4EF[0]=((_tmp4F0.tag=1,((_tmp4F0.f1=Cyc_Tcpat_tuple_con(
r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4F0.f2=ss,
_tmp4F0)))))),_tmp4EF))));}static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,
struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4F1;struct Cyc_Tcpat_Con_s*c=(_tmp4F1=_region_malloc(
r,sizeof(*_tmp4F1)),((_tmp4F1->name=Cyc_Tcpat_Name_v(con_name),((_tmp4F1->arity=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4F1->span=span,((
_tmp4F1->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4F1)))))))));struct Cyc_Tcpat_Con_struct
_tmp4F4;struct Cyc_Tcpat_Con_struct*_tmp4F3;return(void*)((_tmp4F3=_region_malloc(
r,sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F4.tag=1,((_tmp4F4.f1=c,((_tmp4F4.f2=ps,
_tmp4F4)))))),_tmp4F3))));}static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*
r,struct Cyc_Absyn_Pat*p){void*s;{void*_tmp1C2=p->r;enum Cyc_Absyn_Sign _tmp1C7;
int _tmp1C8;char _tmp1CA;struct _dyneither_ptr _tmp1CC;struct Cyc_Absyn_Pat*_tmp1CE;
struct Cyc_Absyn_Pat*_tmp1D0;struct Cyc_Absyn_Pat*_tmp1D2;struct Cyc_Absyn_Datatypedecl*
_tmp1D4;struct Cyc_Absyn_Datatypefield*_tmp1D5;struct Cyc_List_List*_tmp1D6;struct
Cyc_List_List*_tmp1D8;struct Cyc_Absyn_AggrInfo*_tmp1DA;struct Cyc_Absyn_AggrInfo
_tmp1DB;union Cyc_Absyn_AggrInfoU _tmp1DC;struct Cyc_Absyn_Aggrdecl**_tmp1DD;struct
Cyc_Absyn_Aggrdecl*_tmp1DE;struct Cyc_List_List*_tmp1DF;struct Cyc_Absyn_Enumdecl*
_tmp1E1;struct Cyc_Absyn_Enumfield*_tmp1E2;void*_tmp1E4;struct Cyc_Absyn_Enumfield*
_tmp1E5;_LLB5: {struct Cyc_Absyn_Wild_p_struct*_tmp1C3=(struct Cyc_Absyn_Wild_p_struct*)
_tmp1C2;if(_tmp1C3->tag != 0)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_TagInt_p_struct*
_tmp1C4=(struct Cyc_Absyn_TagInt_p_struct*)_tmp1C2;if(_tmp1C4->tag != 3)goto _LLB9;}
_LLB8:{struct Cyc_Tcpat_Any_struct _tmp4F7;struct Cyc_Tcpat_Any_struct*_tmp4F6;s=(
void*)((_tmp4F6=_region_malloc(r,sizeof(*_tmp4F6)),((_tmp4F6[0]=((_tmp4F7.tag=0,
_tmp4F7)),_tmp4F6))));}goto _LLB4;_LLB9: {struct Cyc_Absyn_Null_p_struct*_tmp1C5=(
struct Cyc_Absyn_Null_p_struct*)_tmp1C2;if(_tmp1C5->tag != 8)goto _LLBB;}_LLBA: s=
Cyc_Tcpat_null_pat(r,p);goto _LLB4;_LLBB: {struct Cyc_Absyn_Int_p_struct*_tmp1C6=(
struct Cyc_Absyn_Int_p_struct*)_tmp1C2;if(_tmp1C6->tag != 9)goto _LLBD;else{_tmp1C7=
_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;}}_LLBC: s=Cyc_Tcpat_int_pat(r,_tmp1C8,(struct Cyc_Absyn_Pat*)
p);goto _LLB4;_LLBD: {struct Cyc_Absyn_Char_p_struct*_tmp1C9=(struct Cyc_Absyn_Char_p_struct*)
_tmp1C2;if(_tmp1C9->tag != 10)goto _LLBF;else{_tmp1CA=_tmp1C9->f1;}}_LLBE: s=Cyc_Tcpat_char_pat(
r,_tmp1CA,p);goto _LLB4;_LLBF: {struct Cyc_Absyn_Float_p_struct*_tmp1CB=(struct Cyc_Absyn_Float_p_struct*)
_tmp1C2;if(_tmp1CB->tag != 11)goto _LLC1;else{_tmp1CC=_tmp1CB->f1;}}_LLC0: s=Cyc_Tcpat_float_pat(
r,_tmp1CC,p);goto _LLB4;_LLC1: {struct Cyc_Absyn_Var_p_struct*_tmp1CD=(struct Cyc_Absyn_Var_p_struct*)
_tmp1C2;if(_tmp1CD->tag != 1)goto _LLC3;else{_tmp1CE=_tmp1CD->f2;}}_LLC2: s=Cyc_Tcpat_compile_pat(
r,_tmp1CE);goto _LLB4;_LLC3: {struct Cyc_Absyn_Reference_p_struct*_tmp1CF=(struct
Cyc_Absyn_Reference_p_struct*)_tmp1C2;if(_tmp1CF->tag != 2)goto _LLC5;else{_tmp1D0=
_tmp1CF->f2;}}_LLC4: s=Cyc_Tcpat_compile_pat(r,_tmp1D0);goto _LLB4;_LLC5: {struct
Cyc_Absyn_Pointer_p_struct*_tmp1D1=(struct Cyc_Absyn_Pointer_p_struct*)_tmp1C2;
if(_tmp1D1->tag != 5)goto _LLC7;else{_tmp1D2=_tmp1D1->f1;}}_LLC6:{void*_tmp1EC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1EE;struct Cyc_Absyn_PtrAtts _tmp1EF;union Cyc_Absyn_Constraint*_tmp1F0;_LLDA: {
struct Cyc_Absyn_PointerType_struct*_tmp1ED=(struct Cyc_Absyn_PointerType_struct*)
_tmp1EC;if(_tmp1ED->tag != 5)goto _LLDC;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1EE.ptr_atts;
_tmp1F0=_tmp1EF.nullable;}}_LLDB: {int is_nullable=0;int still_working=1;while(
still_working){union Cyc_Absyn_Constraint*_tmp1F1=_tmp1F0;union Cyc_Absyn_Constraint
_tmp1F2;union Cyc_Absyn_Constraint*_tmp1F3;union Cyc_Absyn_Constraint _tmp1F4;int
_tmp1F5;union Cyc_Absyn_Constraint _tmp1F6;int _tmp1F7;_LLDF: _tmp1F2=*_tmp1F1;if((
_tmp1F2.Forward_constr).tag != 2)goto _LLE1;_tmp1F3=(union Cyc_Absyn_Constraint*)(
_tmp1F2.Forward_constr).val;_LLE0:*_tmp1F0=*_tmp1F3;continue;_LLE1: _tmp1F4=*
_tmp1F1;if((_tmp1F4.No_constr).tag != 3)goto _LLE3;_tmp1F5=(int)(_tmp1F4.No_constr).val;
_LLE2:{struct _union_Constraint_Eq_constr*_tmp4F8;(_tmp4F8=& _tmp1F0->Eq_constr,((
_tmp4F8->tag=1,_tmp4F8->val=0)));}is_nullable=0;still_working=0;goto _LLDE;_LLE3:
_tmp1F6=*_tmp1F1;if((_tmp1F6.Eq_constr).tag != 1)goto _LLDE;_tmp1F7=(int)(_tmp1F6.Eq_constr).val;
_LLE4: is_nullable=(int)_tmp1F7;still_working=0;goto _LLDE;_LLDE:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp1D2);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLD9;};}_LLDC:;_LLDD: {const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4FB="expecting pointertype for pattern!",_tag_dyneither(_tmp4FB,sizeof(char),
35))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}_LLD9:;}goto _LLB4;_LLC7: {
struct Cyc_Absyn_Datatype_p_struct*_tmp1D3=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp1C2;if(_tmp1D3->tag != 7)goto _LLC9;else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D3->f2;
_tmp1D6=_tmp1D3->f3;}}_LLC8: {int*span;{void*_tmp1FB=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLE6: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1FC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1FB;if(_tmp1FC->tag != 3)goto
_LLE8;}_LLE7: if(_tmp1D4->is_extensible)span=0;else{int*_tmp4FC;span=((_tmp4FC=
_region_malloc(r,sizeof(*_tmp4FC)),((_tmp4FC[0]=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D4->fields))->v),
_tmp4FC))));}goto _LLE5;_LLE8: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1FD=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1FB;if(_tmp1FD->tag != 4)goto _LLEA;}
_LLE9: span=(int*)& Cyc_Tcpat_one_opt;goto _LLE5;_LLEA:;_LLEB:{const char*_tmp4FF;
void*_tmp4FE;span=((_tmp4FE=0,((int*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FF="void datatype pattern has bad type",
_tag_dyneither(_tmp4FF,sizeof(char),35))),_tag_dyneither(_tmp4FE,sizeof(void*),0))));}
goto _LLE5;_LLE5:;}s=Cyc_Tcpat_con_pat(r,*(*_tmp1D5->name).f2,span,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D6),p);goto _LLB4;}_LLC9: {struct Cyc_Absyn_Tuple_p_struct*_tmp1D7=(struct
Cyc_Absyn_Tuple_p_struct*)_tmp1C2;if(_tmp1D7->tag != 4)goto _LLCB;else{_tmp1D8=
_tmp1D7->f1;}}_LLCA: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D8),(struct Cyc_Absyn_Pat*)p);goto _LLB4;_LLCB: {struct Cyc_Absyn_Aggr_p_struct*
_tmp1D9=(struct Cyc_Absyn_Aggr_p_struct*)_tmp1C2;if(_tmp1D9->tag != 6)goto _LLCD;
else{_tmp1DA=_tmp1D9->f1;if(_tmp1DA == 0)goto _LLCD;_tmp1DB=*_tmp1DA;_tmp1DC=
_tmp1DB.aggr_info;if((_tmp1DC.KnownAggr).tag != 2)goto _LLCD;_tmp1DD=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1DC.KnownAggr).val;_tmp1DE=*_tmp1DD;_tmp1DF=_tmp1D9->f3;}}_LLCC: if(_tmp1DE->kind
== Cyc_Absyn_StructA){struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DE->impl))->fields;for(0;fields != 
0;fields=fields->tl){const char*_tmp500;int found=Cyc_strcmp((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp500="",_tag_dyneither(_tmp500,
sizeof(char),1))))== 0;{struct Cyc_List_List*dlps0=_tmp1DF;for(0;!found  && dlps0
!= 0;dlps0=dlps0->tl){struct _tuple15 _tmp202;struct Cyc_List_List*_tmp203;struct
Cyc_Absyn_Pat*_tmp204;struct _tuple15*_tmp201=(struct _tuple15*)dlps0->hd;_tmp202=*
_tmp201;_tmp203=_tmp202.f1;_tmp204=_tmp202.f2;{struct Cyc_List_List*_tmp205=
_tmp203;struct Cyc_List_List _tmp206;void*_tmp207;struct _dyneither_ptr*_tmp209;
struct Cyc_List_List*_tmp20A;_LLED: if(_tmp205 == 0)goto _LLEF;_tmp206=*_tmp205;
_tmp207=(void*)_tmp206.hd;{struct Cyc_Absyn_FieldName_struct*_tmp208=(struct Cyc_Absyn_FieldName_struct*)
_tmp207;if(_tmp208->tag != 1)goto _LLEF;else{_tmp209=_tmp208->f1;}};_tmp20A=
_tmp206.tl;if(_tmp20A != 0)goto _LLEF;_LLEE: if(Cyc_strptrcmp(_tmp209,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){{struct Cyc_List_List*_tmp501;ps=((_tmp501=_region_malloc(
r,sizeof(*_tmp501)),((_tmp501->hd=Cyc_Tcpat_compile_pat(r,_tmp204),((_tmp501->tl=
ps,_tmp501))))));}found=1;}goto _LLEC;_LLEF:;_LLF0: {const char*_tmp504;void*
_tmp503;(_tmp503=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp504="bad designator(s)",_tag_dyneither(_tmp504,sizeof(char),18))),
_tag_dyneither(_tmp503,sizeof(void*),0)));}_LLEC:;};}}if(!found){const char*
_tmp507;void*_tmp506;(_tmp506=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp507="bad designator",_tag_dyneither(
_tmp507,sizeof(char),15))),_tag_dyneither(_tmp506,sizeof(void*),0)));}}}s=Cyc_Tcpat_tuple_pat(
r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DE->impl))->tagged){const char*_tmp50A;void*_tmp509;(_tmp509=0,
Cyc_Tcutil_terr(p->loc,((_tmp50A="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp50A,sizeof(char),47))),_tag_dyneither(_tmp509,sizeof(void*),0)));}{
int*_tmp50B;int*span=(_tmp50B=_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DE->impl))->fields),_tmp50B)));int field_name;struct Cyc_List_List*
_tmp213=_tmp1DF;struct Cyc_List_List _tmp214;struct _tuple15*_tmp215;struct _tuple15
_tmp216;struct Cyc_List_List*_tmp217;struct Cyc_List_List _tmp218;void*_tmp219;
struct _dyneither_ptr*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_Absyn_Pat*
_tmp21D;struct Cyc_List_List*_tmp21E;_LLF2: if(_tmp213 == 0)goto _LLF4;_tmp214=*
_tmp213;_tmp215=(struct _tuple15*)_tmp214.hd;_tmp216=*_tmp215;_tmp217=_tmp216.f1;
if(_tmp217 == 0)goto _LLF4;_tmp218=*_tmp217;_tmp219=(void*)_tmp218.hd;{struct Cyc_Absyn_FieldName_struct*
_tmp21A=(struct Cyc_Absyn_FieldName_struct*)_tmp219;if(_tmp21A->tag != 1)goto _LLF4;
else{_tmp21B=_tmp21A->f1;}};_tmp21C=_tmp218.tl;if(_tmp21C != 0)goto _LLF4;_tmp21D=
_tmp216.f2;_tmp21E=_tmp214.tl;if(_tmp21E != 0)goto _LLF4;_LLF3:{struct Cyc_List_List*
_tmp50C;s=Cyc_Tcpat_con_pat(r,*_tmp21B,span,((_tmp50C=_region_malloc(r,sizeof(*
_tmp50C)),((_tmp50C->hd=Cyc_Tcpat_compile_pat(r,_tmp21D),((_tmp50C->tl=0,_tmp50C)))))),
p);}goto _LLF1;_LLF4:;_LLF5: {const char*_tmp50F;void*_tmp50E;(_tmp50E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50F="bad union pattern",
_tag_dyneither(_tmp50F,sizeof(char),18))),_tag_dyneither(_tmp50E,sizeof(void*),0)));}
_LLF1:;};}goto _LLB4;_LLCD: {struct Cyc_Absyn_Enum_p_struct*_tmp1E0=(struct Cyc_Absyn_Enum_p_struct*)
_tmp1C2;if(_tmp1E0->tag != 12)goto _LLCF;else{_tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}
_LLCE: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E1->fields))->v);{int*_tmp510;s=Cyc_Tcpat_con_pat(
r,*(*_tmp1E2->name).f2,((_tmp510=_region_malloc(r,sizeof(*_tmp510)),((_tmp510[0]=
span,_tmp510)))),0,p);}goto _LLB4;}_LLCF: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E3=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1C2;if(_tmp1E3->tag != 13)goto
_LLD1;else{_tmp1E4=(void*)_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;}}_LLD0: {struct Cyc_List_List*
fields;{void*_tmp224=Cyc_Tcutil_compress(_tmp1E4);struct Cyc_List_List*_tmp226;
_LLF7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp225=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp224;if(_tmp225->tag != 14)goto _LLF9;else{_tmp226=_tmp225->f1;}}_LLF8: fields=
_tmp226;goto _LLF6;_LLF9:;_LLFA: {const char*_tmp513;void*_tmp512;(_tmp512=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp513="bad type in AnonEnum_p",
_tag_dyneither(_tmp513,sizeof(char),23))),_tag_dyneither(_tmp512,sizeof(void*),0)));}
_LLF6:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);{int*
_tmp514;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E5->name).f2,((_tmp514=_region_malloc(r,
sizeof(*_tmp514)),((_tmp514[0]=span,_tmp514)))),0,p);}goto _LLB4;};}_LLD1: {
struct Cyc_Absyn_UnknownId_p_struct*_tmp1E6=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1C2;if(_tmp1E6->tag != 14)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp1E7=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1C2;if(_tmp1E7->tag != 15)goto
_LLD5;}_LLD4: goto _LLD6;_LLD5: {struct Cyc_Absyn_Aggr_p_struct*_tmp1E8=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp1C2;if(_tmp1E8->tag != 6)goto _LLD7;}_LLD6: goto _LLD8;_LLD7: {struct Cyc_Absyn_Exp_p_struct*
_tmp1E9=(struct Cyc_Absyn_Exp_p_struct*)_tmp1C2;if(_tmp1E9->tag != 16)goto _LLB4;}
_LLD8: {struct Cyc_Tcpat_Any_struct _tmp517;struct Cyc_Tcpat_Any_struct*_tmp516;s=(
void*)((_tmp516=_region_malloc(r,sizeof(*_tmp516)),((_tmp516[0]=((_tmp517.tag=0,
_tmp517)),_tmp516))));}_LLB4:;}return s;}typedef struct Cyc_List_List*Cyc_Tcpat_match_t;
struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*f1;};typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;typedef struct Cyc_List_List*Cyc_Tcpat_access_t;
struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};typedef void*Cyc_Tcpat_decision_t;
struct _tuple18{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};typedef struct _tuple18*Cyc_Tcpat_work_stack_frame_t;typedef struct Cyc_List_List*
Cyc_Tcpat_work_stack_t;enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,
Cyc_Tcpat_Maybe  = 2};typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;static void
Cyc_Tcpat_print_tab(int i){for(0;i != 0;-- i){const char*_tmp51A;void*_tmp519;(
_tmp519=0,Cyc_fprintf(Cyc_stderr,((_tmp51A=" ",_tag_dyneither(_tmp51A,sizeof(
char),2))),_tag_dyneither(_tmp519,sizeof(void*),0)));}}static void Cyc_Tcpat_print_con(
struct Cyc_Tcpat_Con_s*c){union Cyc_Tcpat_Name_value _tmp22E=c->name;union Cyc_Tcpat_Name_value
_tmp22F=_tmp22E;struct _dyneither_ptr _tmp230;int _tmp231;_LLFC: if((_tmp22F.Name_v).tag
!= 1)goto _LLFE;_tmp230=(struct _dyneither_ptr)(_tmp22F.Name_v).val;_LLFD:{const
char*_tmp51E;void*_tmp51D[1];struct Cyc_String_pa_struct _tmp51C;(_tmp51C.tag=0,((
_tmp51C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp230),((_tmp51D[0]=&
_tmp51C,Cyc_fprintf(Cyc_stderr,((_tmp51E="%s",_tag_dyneither(_tmp51E,sizeof(char),
3))),_tag_dyneither(_tmp51D,sizeof(void*),1)))))));}goto _LLFB;_LLFE: if((_tmp22F.Int_v).tag
!= 2)goto _LLFB;_tmp231=(int)(_tmp22F.Int_v).val;_LLFF:{const char*_tmp522;void*
_tmp521[1];struct Cyc_Int_pa_struct _tmp520;(_tmp520.tag=1,((_tmp520.f1=(
unsigned long)_tmp231,((_tmp521[0]=& _tmp520,Cyc_fprintf(Cyc_stderr,((_tmp522="%d",
_tag_dyneither(_tmp522,sizeof(char),3))),_tag_dyneither(_tmp521,sizeof(void*),1)))))));}
goto _LLFB;_LLFB:;}static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(
void*),int tab){void*_tmp238=d;void*_tmp23A;struct Cyc_List_List*_tmp23D;struct Cyc_Tcpat_Con_s*
_tmp23E;void*_tmp23F;void*_tmp240;_LL101: {struct Cyc_Tcpat_Success_struct*
_tmp239=(struct Cyc_Tcpat_Success_struct*)_tmp238;if(_tmp239->tag != 1)goto _LL103;
else{_tmp23A=(void*)_tmp239->f1;}}_LL102: Cyc_Tcpat_print_tab(tab);{const char*
_tmp525;void*_tmp524;(_tmp524=0,Cyc_fprintf(Cyc_stderr,((_tmp525="Success(",
_tag_dyneither(_tmp525,sizeof(char),9))),_tag_dyneither(_tmp524,sizeof(void*),0)));}
print_rhs(_tmp23A);{const char*_tmp528;void*_tmp527;(_tmp527=0,Cyc_fprintf(Cyc_stderr,((
_tmp528=")\n",_tag_dyneither(_tmp528,sizeof(char),3))),_tag_dyneither(_tmp527,
sizeof(void*),0)));}goto _LL100;_LL103: {struct Cyc_Tcpat_Failure_struct*_tmp23B=(
struct Cyc_Tcpat_Failure_struct*)_tmp238;if(_tmp23B->tag != 0)goto _LL105;}_LL104:{
const char*_tmp52B;void*_tmp52A;(_tmp52A=0,Cyc_fprintf(Cyc_stderr,((_tmp52B="Failure\n",
_tag_dyneither(_tmp52B,sizeof(char),9))),_tag_dyneither(_tmp52A,sizeof(void*),0)));}
goto _LL100;_LL105: {struct Cyc_Tcpat_IfEq_struct*_tmp23C=(struct Cyc_Tcpat_IfEq_struct*)
_tmp238;if(_tmp23C->tag != 2)goto _LL100;else{_tmp23D=_tmp23C->f1;_tmp23E=_tmp23C->f2;
_tmp23F=(void*)_tmp23C->f3;_tmp240=(void*)_tmp23C->f4;}}_LL106: Cyc_Tcpat_print_tab(
tab);{const char*_tmp52E;void*_tmp52D;(_tmp52D=0,Cyc_fprintf(Cyc_stderr,((_tmp52E="Access[",
_tag_dyneither(_tmp52E,sizeof(char),8))),_tag_dyneither(_tmp52D,sizeof(void*),0)));}
for(0;_tmp23D != 0;_tmp23D=_tmp23D->tl){{const char*_tmp532;void*_tmp531[1];struct
Cyc_Int_pa_struct _tmp530;(_tmp530.tag=1,((_tmp530.f1=(unsigned long)((int)
_tmp23D->hd),((_tmp531[0]=& _tmp530,Cyc_fprintf(Cyc_stderr,((_tmp532="%d",
_tag_dyneither(_tmp532,sizeof(char),3))),_tag_dyneither(_tmp531,sizeof(void*),1)))))));}
if(_tmp23D->tl != 0){const char*_tmp535;void*_tmp534;(_tmp534=0,Cyc_fprintf(Cyc_stderr,((
_tmp535=",",_tag_dyneither(_tmp535,sizeof(char),2))),_tag_dyneither(_tmp534,
sizeof(void*),0)));}}{const char*_tmp538;void*_tmp537;(_tmp537=0,Cyc_fprintf(Cyc_stderr,((
_tmp538="],",_tag_dyneither(_tmp538,sizeof(char),3))),_tag_dyneither(_tmp537,
sizeof(void*),0)));}Cyc_Tcpat_print_con(_tmp23E);{const char*_tmp53B;void*_tmp53A;(
_tmp53A=0,Cyc_fprintf(Cyc_stderr,((_tmp53B=",\n",_tag_dyneither(_tmp53B,sizeof(
char),3))),_tag_dyneither(_tmp53A,sizeof(void*),0)));}Cyc_Tcpat_print_dec_tree(
_tmp23F,print_rhs,tab + 7);Cyc_Tcpat_print_dec_tree(_tmp240,print_rhs,tab + 7);
_LL100:;}struct _tuple19{int f1;struct Cyc_Position_Segment*f2;};static void Cyc_Tcpat_print_swrhs(
struct _tuple19*x){return;}static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp252=td;struct Cyc_Set_Set*_tmp254;
_LL108: {struct Cyc_Tcpat_Neg_struct*_tmp253=(struct Cyc_Tcpat_Neg_struct*)_tmp252;
if(_tmp253->tag != 1)goto _LL10A;else{_tmp254=_tmp253->f1;}}_LL109: {struct Cyc_Tcpat_Neg_struct
_tmp53E;struct Cyc_Tcpat_Neg_struct*_tmp53D;return(void*)((_tmp53D=_region_malloc(
r,sizeof(*_tmp53D)),((_tmp53D[0]=((_tmp53E.tag=1,((_tmp53E.f1=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(
r,_tmp254,c),_tmp53E)))),_tmp53D))));}_LL10A: {struct Cyc_Tcpat_Pos_struct*
_tmp255=(struct Cyc_Tcpat_Pos_struct*)_tmp252;if(_tmp255->tag != 0)goto _LL107;}
_LL10B: {const char*_tmp541;void*_tmp540;(_tmp540=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp541="add_neg called when td is Positive",
_tag_dyneither(_tmp541,sizeof(char),35))),_tag_dyneither(_tmp540,sizeof(void*),0)));}
_LL107:;}static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp25A=td;struct Cyc_Tcpat_Con_s*_tmp25C;struct Cyc_Set_Set*
_tmp25E;_LL10D: {struct Cyc_Tcpat_Pos_struct*_tmp25B=(struct Cyc_Tcpat_Pos_struct*)
_tmp25A;if(_tmp25B->tag != 0)goto _LL10F;else{_tmp25C=_tmp25B->f1;}}_LL10E: if(Cyc_Tcpat_compare_con(
c,_tmp25C)== 0)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_No;}_LL10F: {struct Cyc_Tcpat_Neg_struct*
_tmp25D=(struct Cyc_Tcpat_Neg_struct*)_tmp25A;if(_tmp25D->tag != 1)goto _LL10C;
else{_tmp25E=_tmp25D->f1;}}_LL110: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp25E,c))return Cyc_Tcpat_No;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp25E)+ 1)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_Maybe;}}_LL10C:;}struct
_tuple20{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp25F=ctxt;struct Cyc_List_List _tmp260;struct _tuple20*
_tmp261;struct _tuple20 _tmp262;struct Cyc_Tcpat_Con_s*_tmp263;struct Cyc_List_List*
_tmp264;struct Cyc_List_List*_tmp265;_LL112: if(_tmp25F != 0)goto _LL114;_LL113:
return 0;_LL114: if(_tmp25F == 0)goto _LL111;_tmp260=*_tmp25F;_tmp261=(struct
_tuple20*)_tmp260.hd;_tmp262=*_tmp261;_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;
_tmp265=_tmp260.tl;_LL115: {struct _tuple20*_tmp547;struct Cyc_List_List*_tmp546;
struct Cyc_List_List*_tmp545;return(_tmp545=_region_malloc(r,sizeof(*_tmp545)),((
_tmp545->hd=((_tmp547=_region_malloc(r,sizeof(*_tmp547)),((_tmp547->f1=_tmp263,((
_tmp547->f2=(struct Cyc_List_List*)((_tmp546=_region_malloc(r,sizeof(*_tmp546)),((
_tmp546->hd=dsc,((_tmp546->tl=_tmp264,_tmp546)))))),_tmp547)))))),((_tmp545->tl=
_tmp265,_tmp545)))));}_LL111:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp269=ctxt;
struct Cyc_List_List _tmp26A;struct _tuple20*_tmp26B;struct _tuple20 _tmp26C;struct
Cyc_Tcpat_Con_s*_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp26F;
_LL117: if(_tmp269 != 0)goto _LL119;_LL118: {const char*_tmp54A;void*_tmp549;(
_tmp549=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp54A="norm_context: empty context",_tag_dyneither(_tmp54A,sizeof(char),28))),
_tag_dyneither(_tmp549,sizeof(void*),0)));}_LL119: if(_tmp269 == 0)goto _LL116;
_tmp26A=*_tmp269;_tmp26B=(struct _tuple20*)_tmp26A.hd;_tmp26C=*_tmp26B;_tmp26D=
_tmp26C.f1;_tmp26E=_tmp26C.f2;_tmp26F=_tmp26A.tl;_LL11A: {struct Cyc_Tcpat_Pos_struct
_tmp54D;struct Cyc_Tcpat_Pos_struct*_tmp54C;return Cyc_Tcpat_augment(r,_tmp26F,(
void*)((_tmp54C=_region_malloc(r,sizeof(*_tmp54C)),((_tmp54C[0]=((_tmp54D.tag=0,((
_tmp54D.f1=_tmp26D,((_tmp54D.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp26E),_tmp54D)))))),_tmp54C)))));}
_LL116:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple13 _tmp54E;struct _tuple13
_tmp275=(_tmp54E.f1=ctxt,((_tmp54E.f2=work,_tmp54E)));struct Cyc_List_List*
_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_List_List*
_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List _tmp27B;struct _tuple20*
_tmp27C;struct _tuple20 _tmp27D;struct Cyc_Tcpat_Con_s*_tmp27E;struct Cyc_List_List*
_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_List_List
_tmp282;struct _tuple18*_tmp283;struct _tuple18 _tmp284;struct Cyc_List_List*_tmp285;
struct Cyc_List_List*_tmp286;_LL11C: _tmp276=_tmp275.f1;if(_tmp276 != 0)goto _LL11E;
_tmp277=_tmp275.f2;if(_tmp277 != 0)goto _LL11E;_LL11D: return dsc;_LL11E: _tmp278=
_tmp275.f1;if(_tmp278 != 0)goto _LL120;_LL11F: goto _LL121;_LL120: _tmp279=_tmp275.f2;
if(_tmp279 != 0)goto _LL122;_LL121: {const char*_tmp551;void*_tmp550;(_tmp550=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp551="build_desc: ctxt and work don't match",_tag_dyneither(_tmp551,sizeof(
char),38))),_tag_dyneither(_tmp550,sizeof(void*),0)));}_LL122: _tmp27A=_tmp275.f1;
if(_tmp27A == 0)goto _LL11B;_tmp27B=*_tmp27A;_tmp27C=(struct _tuple20*)_tmp27B.hd;
_tmp27D=*_tmp27C;_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;_tmp280=_tmp27B.tl;
_tmp281=_tmp275.f2;if(_tmp281 == 0)goto _LL11B;_tmp282=*_tmp281;_tmp283=(struct
_tuple18*)_tmp282.hd;_tmp284=*_tmp283;_tmp285=_tmp284.f3;_tmp286=_tmp282.tl;
_LL123: {struct Cyc_Tcpat_Pos_struct _tmp557;struct Cyc_List_List*_tmp556;struct Cyc_Tcpat_Pos_struct*
_tmp555;struct Cyc_Tcpat_Pos_struct*_tmp289=(_tmp555=_region_malloc(r,sizeof(*
_tmp555)),((_tmp555[0]=((_tmp557.tag=0,((_tmp557.f1=_tmp27E,((_tmp557.f2=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,_tmp27F),((_tmp556=_region_malloc(r,sizeof(*_tmp556)),((
_tmp556->hd=dsc,((_tmp556->tl=_tmp285,_tmp556))))))),_tmp557)))))),_tmp555)));
return Cyc_Tcpat_build_desc(r,_tmp280,(void*)_tmp289,_tmp286);}_LL11B:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp28D=allmrules;struct
Cyc_List_List _tmp28E;struct _tuple0*_tmp28F;struct _tuple0 _tmp290;void*_tmp291;
void*_tmp292;struct Cyc_List_List*_tmp293;_LL125: if(_tmp28D != 0)goto _LL127;_LL126: {
struct Cyc_Tcpat_Failure_struct _tmp55A;struct Cyc_Tcpat_Failure_struct*_tmp559;
return(void*)((_tmp559=_cycalloc(sizeof(*_tmp559)),((_tmp559[0]=((_tmp55A.tag=0,((
_tmp55A.f1=(void*)dsc,_tmp55A)))),_tmp559))));}_LL127: if(_tmp28D == 0)goto _LL124;
_tmp28E=*_tmp28D;_tmp28F=(struct _tuple0*)_tmp28E.hd;_tmp290=*_tmp28F;_tmp291=
_tmp290.f1;_tmp292=_tmp290.f2;_tmp293=_tmp28E.tl;_LL128: return Cyc_Tcpat_match(r,
_tmp291,0,dsc,0,0,_tmp292,_tmp293);_LL124:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_struct
_tmp55D;struct Cyc_Tcpat_Neg_struct*_tmp55C;return Cyc_Tcpat_or_match(r,(void*)((
_tmp55C=_region_malloc(r,sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55D.tag=1,((
_tmp55D.f1=Cyc_Tcpat_empty_con_set(r),_tmp55D)))),_tmp55C)))),allmrules);}static
void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp298=
work;struct Cyc_List_List _tmp299;struct _tuple18*_tmp29A;struct _tuple18 _tmp29B;
struct Cyc_List_List*_tmp29C;struct Cyc_List_List*_tmp29D;struct Cyc_List_List*
_tmp29E;struct Cyc_List_List*_tmp29F;struct Cyc_List_List _tmp2A0;struct _tuple18*
_tmp2A1;struct _tuple18 _tmp2A2;struct Cyc_List_List*_tmp2A3;struct Cyc_List_List*
_tmp2A4;struct Cyc_List_List*_tmp2A5;struct Cyc_List_List*_tmp2A6;_LL12A: if(
_tmp298 != 0)goto _LL12C;_LL12B: {struct Cyc_Tcpat_Success_struct _tmp560;struct Cyc_Tcpat_Success_struct*
_tmp55F;return(void*)((_tmp55F=_region_malloc(r,sizeof(*_tmp55F)),((_tmp55F[0]=((
_tmp560.tag=1,((_tmp560.f1=(void*)right_hand_side,_tmp560)))),_tmp55F))));}
_LL12C: if(_tmp298 == 0)goto _LL12E;_tmp299=*_tmp298;_tmp29A=(struct _tuple18*)
_tmp299.hd;_tmp29B=*_tmp29A;_tmp29C=_tmp29B.f1;if(_tmp29C != 0)goto _LL12E;_tmp29D=
_tmp29B.f2;if(_tmp29D != 0)goto _LL12E;_tmp29E=_tmp29B.f3;if(_tmp29E != 0)goto
_LL12E;_tmp29F=_tmp299.tl;_LL12D: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp29F,right_hand_side,rules);_LL12E: if(_tmp298 == 0)goto _LL129;_tmp2A0=*
_tmp298;_tmp2A1=(struct _tuple18*)_tmp2A0.hd;_tmp2A2=*_tmp2A1;_tmp2A3=_tmp2A2.f1;
_tmp2A4=_tmp2A2.f2;_tmp2A5=_tmp2A2.f3;_tmp2A6=_tmp2A0.tl;_LL12F: if((_tmp2A3 == 0
 || _tmp2A4 == 0) || _tmp2A5 == 0){const char*_tmp563;void*_tmp562;(_tmp562=0,Cyc_Tcutil_impos(((
_tmp563="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp563,sizeof(
char),38))),_tag_dyneither(_tmp562,sizeof(void*),0)));}{struct Cyc_List_List
_tmp2AC;void*_tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2AB=(
struct Cyc_List_List*)_tmp2A3;_tmp2AC=*_tmp2AB;_tmp2AD=(void*)_tmp2AC.hd;_tmp2AE=
_tmp2AC.tl;{struct Cyc_List_List _tmp2B0;struct Cyc_List_List*_tmp2B1;struct Cyc_List_List*
_tmp2B2;struct Cyc_List_List*_tmp2AF=(struct Cyc_List_List*)_tmp2A4;_tmp2B0=*
_tmp2AF;_tmp2B1=(struct Cyc_List_List*)_tmp2B0.hd;_tmp2B2=_tmp2B0.tl;{struct Cyc_List_List
_tmp2B4;void*_tmp2B5;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B3=(
struct Cyc_List_List*)_tmp2A5;_tmp2B4=*_tmp2B3;_tmp2B5=(void*)_tmp2B4.hd;_tmp2B6=
_tmp2B4.tl;{struct _tuple18*_tmp564;struct _tuple18*_tmp2B7=(_tmp564=
_region_malloc(r,sizeof(*_tmp564)),((_tmp564->f1=_tmp2AE,((_tmp564->f2=_tmp2B2,((
_tmp564->f3=_tmp2B6,_tmp564)))))));struct Cyc_List_List*_tmp565;return Cyc_Tcpat_match(
r,_tmp2AD,_tmp2B1,_tmp2B5,ctx,((_tmp565=_region_malloc(r,sizeof(*_tmp565)),((
_tmp565->hd=_tmp2B7,((_tmp565->tl=_tmp2A6,_tmp565)))))),right_hand_side,rules);};};};};
_LL129:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp2BA=dsc;struct Cyc_Set_Set*_tmp2BC;
struct Cyc_List_List*_tmp2BE;_LL131: {struct Cyc_Tcpat_Neg_struct*_tmp2BB=(struct
Cyc_Tcpat_Neg_struct*)_tmp2BA;if(_tmp2BB->tag != 1)goto _LL133;else{_tmp2BC=
_tmp2BB->f1;}}_LL132: {struct Cyc_Tcpat_Neg_struct _tmp568;struct Cyc_Tcpat_Neg_struct*
_tmp567;void*any=(void*)((_tmp567=_region_malloc(r,sizeof(*_tmp567)),((_tmp567[0]=((
_tmp568.tag=1,((_tmp568.f1=Cyc_Tcpat_empty_con_set(r),_tmp568)))),_tmp567))));
struct Cyc_List_List*_tmp2BF=0;{int i=0;for(0;i < pcon->arity;++ i){struct Cyc_List_List*
_tmp569;_tmp2BF=((_tmp569=_region_malloc(r,sizeof(*_tmp569)),((_tmp569->hd=any,((
_tmp569->tl=_tmp2BF,_tmp569))))));}}return _tmp2BF;}_LL133: {struct Cyc_Tcpat_Pos_struct*
_tmp2BD=(struct Cyc_Tcpat_Pos_struct*)_tmp2BA;if(_tmp2BD->tag != 0)goto _LL130;
else{_tmp2BE=_tmp2BD->f2;}}_LL134: return _tmp2BE;_LL130:;}struct _tuple21{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(
struct _tuple21*env,int i){struct _tuple21 _tmp2C4;struct _RegionHandle*_tmp2C5;
struct Cyc_List_List*_tmp2C6;struct _tuple21*_tmp2C3=env;_tmp2C4=*_tmp2C3;_tmp2C5=
_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;{struct Cyc_List_List*_tmp56A;return(_tmp56A=
_region_malloc(_tmp2C5,sizeof(*_tmp56A)),((_tmp56A->hd=(void*)(i + 1),((_tmp56A->tl=
_tmp2C6,_tmp56A)))));};}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct
_RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple21 _tmp56B;struct _tuple21 _tmp2C8=(_tmp56B.f1=r,((_tmp56B.f2=obj,_tmp56B)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple21*,int),struct _tuple21*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp2C8);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp2CA=p;struct Cyc_Tcpat_Con_s*
_tmp2CD;struct Cyc_List_List*_tmp2CE;_LL136: {struct Cyc_Tcpat_Any_struct*_tmp2CB=(
struct Cyc_Tcpat_Any_struct*)_tmp2CA;if(_tmp2CB->tag != 0)goto _LL138;}_LL137:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL138: {struct Cyc_Tcpat_Con_struct*_tmp2CC=(struct Cyc_Tcpat_Con_struct*)
_tmp2CA;if(_tmp2CC->tag != 1)goto _LL135;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;}}
_LL139: switch(Cyc_Tcpat_static_match(_tmp2CD,dsc)){case Cyc_Tcpat_Yes: _LL13A: {
struct _tuple20*_tmp56E;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*ctx2=(
_tmp56D=_region_malloc(r,sizeof(*_tmp56D)),((_tmp56D->hd=((_tmp56E=
_region_malloc(r,sizeof(*_tmp56E)),((_tmp56E->f1=_tmp2CD,((_tmp56E->f2=0,_tmp56E)))))),((
_tmp56D->tl=ctx,_tmp56D)))));struct _tuple18*_tmp56F;struct _tuple18*work_frame=(
_tmp56F=_region_malloc(r,sizeof(*_tmp56F)),((_tmp56F->f1=_tmp2CE,((_tmp56F->f2=
Cyc_Tcpat_getoargs(r,_tmp2CD,obj),((_tmp56F->f3=Cyc_Tcpat_getdargs(r,_tmp2CD,dsc),
_tmp56F)))))));struct Cyc_List_List*_tmp570;struct Cyc_List_List*work2=(_tmp570=
_region_malloc(r,sizeof(*_tmp570)),((_tmp570->hd=work_frame,((_tmp570->tl=work,
_tmp570)))));return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case
Cyc_Tcpat_No: _LL13B: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,
work),rules);case Cyc_Tcpat_Maybe: _LL13C: {struct _tuple20*_tmp573;struct Cyc_List_List*
_tmp572;struct Cyc_List_List*ctx2=(_tmp572=_region_malloc(r,sizeof(*_tmp572)),((
_tmp572->hd=((_tmp573=_region_malloc(r,sizeof(*_tmp573)),((_tmp573->f1=_tmp2CD,((
_tmp573->f2=0,_tmp573)))))),((_tmp572->tl=ctx,_tmp572)))));struct _tuple18*
_tmp574;struct _tuple18*work_frame=(_tmp574=_region_malloc(r,sizeof(*_tmp574)),((
_tmp574->f1=_tmp2CE,((_tmp574->f2=Cyc_Tcpat_getoargs(r,_tmp2CD,obj),((_tmp574->f3=
Cyc_Tcpat_getdargs(r,_tmp2CD,dsc),_tmp574)))))));struct Cyc_List_List*_tmp575;
struct Cyc_List_List*work2=(_tmp575=_region_malloc(r,sizeof(*_tmp575)),((_tmp575->hd=
work_frame,((_tmp575->tl=work,_tmp575)))));void*_tmp2D3=Cyc_Tcpat_and_match(r,
ctx2,work2,right_hand_side,rules);void*_tmp2D4=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2CD),work),rules);struct Cyc_Tcpat_IfEq_struct
_tmp578;struct Cyc_Tcpat_IfEq_struct*_tmp577;return(void*)((_tmp577=
_region_malloc(r,sizeof(*_tmp577)),((_tmp577[0]=((_tmp578.tag=2,((_tmp578.f1=obj,((
_tmp578.f2=_tmp2CD,((_tmp578.f3=(void*)_tmp2D3,((_tmp578.f4=(void*)_tmp2D4,
_tmp578)))))))))),_tmp577))));}}_LL135:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp2DB=d;void*_tmp2DD;void*_tmp2DF;void*_tmp2E1;void*_tmp2E2;_LL13F: {struct Cyc_Tcpat_Failure_struct*
_tmp2DC=(struct Cyc_Tcpat_Failure_struct*)_tmp2DB;if(_tmp2DC->tag != 0)goto _LL141;
else{_tmp2DD=(void*)_tmp2DC->f1;}}_LL140: if(!(*exhaust_done)){not_exhaust(r,env1,
_tmp2DD);*exhaust_done=1;}goto _LL13E;_LL141: {struct Cyc_Tcpat_Success_struct*
_tmp2DE=(struct Cyc_Tcpat_Success_struct*)_tmp2DB;if(_tmp2DE->tag != 1)goto _LL143;
else{_tmp2DF=(void*)_tmp2DE->f1;}}_LL142: rhs_appears(env2,_tmp2DF);goto _LL13E;
_LL143: {struct Cyc_Tcpat_IfEq_struct*_tmp2E0=(struct Cyc_Tcpat_IfEq_struct*)
_tmp2DB;if(_tmp2E0->tag != 2)goto _LL13E;else{_tmp2E1=(void*)_tmp2E0->f3;_tmp2E2=(
void*)_tmp2E0->f4;}}_LL144: Cyc_Tcpat_check_exhaust_overlap(r,_tmp2E1,not_exhaust,
env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(r,_tmp2E2,
not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13E;_LL13E:;}struct
_tuple22{void*f1;struct _tuple19*f2;};struct _tuple23{struct _RegionHandle*f1;int*
f2;};static struct _tuple22*Cyc_Tcpat_get_match(struct _tuple23*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple23 _tmp2E4;struct _RegionHandle*_tmp2E5;int*_tmp2E6;struct
_tuple23*_tmp2E3=env;_tmp2E4=*_tmp2E3;_tmp2E5=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2E5,swc->pattern);struct _tuple19*_tmp579;
struct _tuple19*rhs=(_tmp579=_region_malloc(_tmp2E5,sizeof(*_tmp579)),((_tmp579->f1=
0,((_tmp579->f2=(swc->pattern)->loc,_tmp579)))));void*sp;if(swc->where_clause != 
0){{struct Cyc_List_List*_tmp57C;struct Cyc_List_List*_tmp57B;sp=Cyc_Tcpat_tuple_pat(
_tmp2E5,((_tmp57B=_region_malloc(_tmp2E5,sizeof(*_tmp57B)),((_tmp57B->hd=sp0,((
_tmp57B->tl=((_tmp57C=_region_malloc(_tmp2E5,sizeof(*_tmp57C)),((_tmp57C->hd=Cyc_Tcpat_int_pat(
_tmp2E5,*_tmp2E6,0),((_tmp57C->tl=0,_tmp57C)))))),_tmp57B)))))),0);}*_tmp2E6=*
_tmp2E6 + 1;}else{struct Cyc_List_List*_tmp586;struct Cyc_Tcpat_Any_struct _tmp585;
struct Cyc_Tcpat_Any_struct*_tmp584;struct Cyc_List_List*_tmp583;sp=Cyc_Tcpat_tuple_pat(
_tmp2E5,((_tmp583=_region_malloc(_tmp2E5,sizeof(*_tmp583)),((_tmp583->hd=sp0,((
_tmp583->tl=((_tmp586=_region_malloc(_tmp2E5,sizeof(*_tmp586)),((_tmp586->hd=(
void*)((_tmp584=_region_malloc(_tmp2E5,sizeof(*_tmp584)),((_tmp584[0]=((_tmp585.tag=
0,_tmp585)),_tmp584)))),((_tmp586->tl=0,_tmp586)))))),_tmp583)))))),0);}{struct
_tuple22*_tmp587;return(_tmp587=_region_malloc(_tmp2E5,sizeof(*_tmp587)),((
_tmp587->f1=sp,((_tmp587->f2=rhs,_tmp587)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";
struct Cyc_Tcpat_Desc2string_struct{char*tag;};struct Cyc_Tcpat_Desc2string_struct
Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp2F0=d;struct Cyc_Tcpat_Con_s*_tmp2F2;
struct Cyc_List_List*_tmp2F3;struct Cyc_Set_Set*_tmp2F5;_LL146: {struct Cyc_Tcpat_Pos_struct*
_tmp2F1=(struct Cyc_Tcpat_Pos_struct*)_tmp2F0;if(_tmp2F1->tag != 0)goto _LL148;
else{_tmp2F2=_tmp2F1->f1;_tmp2F3=_tmp2F1->f2;}}_LL147: {union Cyc_Tcpat_Name_value
_tmp2F6=_tmp2F2->name;struct Cyc_Absyn_Pat*_tmp2F7=_tmp2F2->orig_pat;if(_tmp2F7 == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp2F3))->hd);{void*_tmp2F8=_tmp2F7->r;struct Cyc_Absyn_Vardecl*_tmp2FB;struct
Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Tvar*_tmp2FF;struct Cyc_Absyn_Vardecl*
_tmp300;struct Cyc_Absyn_AggrInfo*_tmp304;struct Cyc_Absyn_AggrInfo _tmp305;union
Cyc_Absyn_AggrInfoU _tmp306;struct Cyc_Absyn_Aggrdecl**_tmp307;struct Cyc_Absyn_Aggrdecl*
_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_Absyn_Datatypefield*_tmp30B;int
_tmp30E;char _tmp310;struct _dyneither_ptr _tmp312;int _tmp313;struct Cyc_Absyn_Enumfield*
_tmp315;struct Cyc_Absyn_Enumfield*_tmp317;struct Cyc_Absyn_Exp*_tmp319;_LL14B: {
struct Cyc_Absyn_Wild_p_struct*_tmp2F9=(struct Cyc_Absyn_Wild_p_struct*)_tmp2F8;
if(_tmp2F9->tag != 0)goto _LL14D;}_LL14C: {const char*_tmp588;return(_tmp588="_",
_tag_dyneither(_tmp588,sizeof(char),2));}_LL14D: {struct Cyc_Absyn_Var_p_struct*
_tmp2FA=(struct Cyc_Absyn_Var_p_struct*)_tmp2F8;if(_tmp2FA->tag != 1)goto _LL14F;
else{_tmp2FB=_tmp2FA->f1;}}_LL14E: return Cyc_Absynpp_qvar2string(_tmp2FB->name);
_LL14F: {struct Cyc_Absyn_Reference_p_struct*_tmp2FC=(struct Cyc_Absyn_Reference_p_struct*)
_tmp2F8;if(_tmp2FC->tag != 2)goto _LL151;else{_tmp2FD=_tmp2FC->f1;}}_LL150: {const
char*_tmp58C;void*_tmp58B[1];struct Cyc_String_pa_struct _tmp58A;return(struct
_dyneither_ptr)((_tmp58A.tag=0,((_tmp58A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FD->name)),((_tmp58B[0]=& _tmp58A,Cyc_aprintf(((
_tmp58C="*%s",_tag_dyneither(_tmp58C,sizeof(char),4))),_tag_dyneither(_tmp58B,
sizeof(void*),1))))))));}_LL151: {struct Cyc_Absyn_TagInt_p_struct*_tmp2FE=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp2F8;if(_tmp2FE->tag != 3)goto _LL153;else{
_tmp2FF=_tmp2FE->f1;_tmp300=_tmp2FE->f2;}}_LL152: {const char*_tmp591;void*
_tmp590[2];struct Cyc_String_pa_struct _tmp58F;struct Cyc_String_pa_struct _tmp58E;
return(struct _dyneither_ptr)((_tmp58E.tag=0,((_tmp58E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2FF->name),((_tmp58F.tag=0,((_tmp58F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp300->name)),((
_tmp590[0]=& _tmp58F,((_tmp590[1]=& _tmp58E,Cyc_aprintf(((_tmp591="%s<`%s>",
_tag_dyneither(_tmp591,sizeof(char),8))),_tag_dyneither(_tmp590,sizeof(void*),2))))))))))))));}
_LL153: {struct Cyc_Absyn_Tuple_p_struct*_tmp301=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp2F8;if(_tmp301->tag != 4)goto _LL155;}_LL154: {const char*_tmp595;void*_tmp594[
1];struct Cyc_String_pa_struct _tmp593;return(struct _dyneither_ptr)((_tmp593.tag=0,((
_tmp593.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2F3)),((_tmp594[0]=& _tmp593,Cyc_aprintf(((_tmp595="$(%s)",_tag_dyneither(
_tmp595,sizeof(char),6))),_tag_dyneither(_tmp594,sizeof(void*),1))))))));}_LL155: {
struct Cyc_Absyn_Pointer_p_struct*_tmp302=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp2F8;if(_tmp302->tag != 5)goto _LL157;}_LL156: {const char*_tmp599;void*_tmp598[
1];struct Cyc_String_pa_struct _tmp597;return(struct _dyneither_ptr)((_tmp597.tag=0,((
_tmp597.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2F3)),((_tmp598[0]=& _tmp597,Cyc_aprintf(((_tmp599="&%s",_tag_dyneither(
_tmp599,sizeof(char),4))),_tag_dyneither(_tmp598,sizeof(void*),1))))))));}_LL157: {
struct Cyc_Absyn_Aggr_p_struct*_tmp303=(struct Cyc_Absyn_Aggr_p_struct*)_tmp2F8;
if(_tmp303->tag != 6)goto _LL159;else{_tmp304=_tmp303->f1;if(_tmp304 == 0)goto
_LL159;_tmp305=*_tmp304;_tmp306=_tmp305.aggr_info;if((_tmp306.KnownAggr).tag != 2)
goto _LL159;_tmp307=(struct Cyc_Absyn_Aggrdecl**)(_tmp306.KnownAggr).val;_tmp308=*
_tmp307;_tmp309=_tmp303->f3;}}_LL158: if(_tmp308->kind == Cyc_Absyn_UnionA){struct
Cyc_List_List*_tmp328=_tmp309;struct Cyc_List_List _tmp329;struct _tuple15*_tmp32A;
struct _tuple15 _tmp32B;struct Cyc_List_List*_tmp32C;struct Cyc_List_List _tmp32D;
void*_tmp32E;struct _dyneither_ptr*_tmp330;_LL16C: if(_tmp328 == 0)goto _LL16E;
_tmp329=*_tmp328;_tmp32A=(struct _tuple15*)_tmp329.hd;_tmp32B=*_tmp32A;_tmp32C=
_tmp32B.f1;if(_tmp32C == 0)goto _LL16E;_tmp32D=*_tmp32C;_tmp32E=(void*)_tmp32D.hd;{
struct Cyc_Absyn_FieldName_struct*_tmp32F=(struct Cyc_Absyn_FieldName_struct*)
_tmp32E;if(_tmp32F->tag != 1)goto _LL16E;else{_tmp330=_tmp32F->f1;}};_LL16D: {
const char*_tmp59F;void*_tmp59E[3];struct Cyc_String_pa_struct _tmp59D;struct Cyc_String_pa_struct
_tmp59C;struct Cyc_String_pa_struct _tmp59B;return(struct _dyneither_ptr)((_tmp59B.tag=
0,((_tmp59B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F3)),((_tmp59C.tag=0,((_tmp59C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp330),((_tmp59D.tag=0,((_tmp59D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp308->name)),((_tmp59E[0]=&
_tmp59D,((_tmp59E[1]=& _tmp59C,((_tmp59E[2]=& _tmp59B,Cyc_aprintf(((_tmp59F="%s{.%s = %s}",
_tag_dyneither(_tmp59F,sizeof(char),13))),_tag_dyneither(_tmp59E,sizeof(void*),3))))))))))))))))))));}
_LL16E:;_LL16F: goto _LL16B;_LL16B:;}{const char*_tmp5A4;void*_tmp5A3[2];struct Cyc_String_pa_struct
_tmp5A2;struct Cyc_String_pa_struct _tmp5A1;return(struct _dyneither_ptr)((_tmp5A1.tag=
0,((_tmp5A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F3)),((_tmp5A2.tag=0,((_tmp5A2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp308->name)),((_tmp5A3[0]=& _tmp5A2,((
_tmp5A3[1]=& _tmp5A1,Cyc_aprintf(((_tmp5A4="%s{%s}",_tag_dyneither(_tmp5A4,
sizeof(char),7))),_tag_dyneither(_tmp5A3,sizeof(void*),2))))))))))))));};_LL159: {
struct Cyc_Absyn_Datatype_p_struct*_tmp30A=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp2F8;if(_tmp30A->tag != 7)goto _LL15B;else{_tmp30B=_tmp30A->f2;}}_LL15A: if(
_tmp2F3 == 0){const char*_tmp5A8;void*_tmp5A7[1];struct Cyc_String_pa_struct _tmp5A6;
return(struct _dyneither_ptr)((_tmp5A6.tag=0,((_tmp5A6.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp30B->name)),((_tmp5A7[0]=&
_tmp5A6,Cyc_aprintf(((_tmp5A8="%s",_tag_dyneither(_tmp5A8,sizeof(char),3))),
_tag_dyneither(_tmp5A7,sizeof(void*),1))))))));}else{const char*_tmp5AD;void*
_tmp5AC[2];struct Cyc_String_pa_struct _tmp5AB;struct Cyc_String_pa_struct _tmp5AA;
return(struct _dyneither_ptr)((_tmp5AA.tag=0,((_tmp5AA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2F3)),((_tmp5AB.tag=0,((
_tmp5AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp30B->name)),((_tmp5AC[0]=& _tmp5AB,((_tmp5AC[1]=& _tmp5AA,Cyc_aprintf(((
_tmp5AD="%s(%s)",_tag_dyneither(_tmp5AD,sizeof(char),7))),_tag_dyneither(_tmp5AC,
sizeof(void*),2))))))))))))));}_LL15B: {struct Cyc_Absyn_Null_p_struct*_tmp30C=(
struct Cyc_Absyn_Null_p_struct*)_tmp2F8;if(_tmp30C->tag != 8)goto _LL15D;}_LL15C: {
const char*_tmp5AE;return(_tmp5AE="NULL",_tag_dyneither(_tmp5AE,sizeof(char),5));}
_LL15D: {struct Cyc_Absyn_Int_p_struct*_tmp30D=(struct Cyc_Absyn_Int_p_struct*)
_tmp2F8;if(_tmp30D->tag != 9)goto _LL15F;else{_tmp30E=_tmp30D->f2;}}_LL15E: {const
char*_tmp5B2;void*_tmp5B1[1];struct Cyc_Int_pa_struct _tmp5B0;return(struct
_dyneither_ptr)((_tmp5B0.tag=1,((_tmp5B0.f1=(unsigned long)_tmp30E,((_tmp5B1[0]=&
_tmp5B0,Cyc_aprintf(((_tmp5B2="%d",_tag_dyneither(_tmp5B2,sizeof(char),3))),
_tag_dyneither(_tmp5B1,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_Char_p_struct*
_tmp30F=(struct Cyc_Absyn_Char_p_struct*)_tmp2F8;if(_tmp30F->tag != 10)goto _LL161;
else{_tmp310=_tmp30F->f1;}}_LL160: {const char*_tmp5B6;void*_tmp5B5[1];struct Cyc_Int_pa_struct
_tmp5B4;return(struct _dyneither_ptr)((_tmp5B4.tag=1,((_tmp5B4.f1=(unsigned long)((
int)_tmp310),((_tmp5B5[0]=& _tmp5B4,Cyc_aprintf(((_tmp5B6="%d",_tag_dyneither(
_tmp5B6,sizeof(char),3))),_tag_dyneither(_tmp5B5,sizeof(void*),1))))))));}_LL161: {
struct Cyc_Absyn_Float_p_struct*_tmp311=(struct Cyc_Absyn_Float_p_struct*)_tmp2F8;
if(_tmp311->tag != 11)goto _LL163;else{_tmp312=_tmp311->f1;_tmp313=_tmp311->f2;}}
_LL162: return _tmp312;_LL163: {struct Cyc_Absyn_Enum_p_struct*_tmp314=(struct Cyc_Absyn_Enum_p_struct*)
_tmp2F8;if(_tmp314->tag != 12)goto _LL165;else{_tmp315=_tmp314->f2;}}_LL164:
_tmp317=_tmp315;goto _LL166;_LL165: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp316=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp2F8;if(_tmp316->tag != 13)goto _LL167;else{
_tmp317=_tmp316->f2;}}_LL166: return Cyc_Absynpp_qvar2string(_tmp317->name);_LL167: {
struct Cyc_Absyn_Exp_p_struct*_tmp318=(struct Cyc_Absyn_Exp_p_struct*)_tmp2F8;if(
_tmp318->tag != 16)goto _LL169;else{_tmp319=_tmp318->f1;}}_LL168: return Cyc_Absynpp_exp2string(
_tmp319);_LL169:;_LL16A:(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL14A:;};}
_LL148: {struct Cyc_Tcpat_Neg_struct*_tmp2F4=(struct Cyc_Tcpat_Neg_struct*)_tmp2F0;
if(_tmp2F4->tag != 1)goto _LL145;else{_tmp2F5=_tmp2F4->f1;}}_LL149: if(((int(*)(
struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2F5)){const char*_tmp5B7;return(
_tmp5B7="_",_tag_dyneither(_tmp5B7,sizeof(char),2));}{struct Cyc_Tcpat_Con_s*
_tmp349=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2F5);
struct Cyc_Absyn_Pat*_tmp34A=_tmp349->orig_pat;if(_tmp34A == 0)(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);{void*_tmp34B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp34A->topt))->v);enum Cyc_Absyn_Size_of _tmp34D;struct Cyc_Absyn_PtrInfo
_tmp350;struct Cyc_Absyn_PtrAtts _tmp351;struct Cyc_Absyn_DatatypeInfo _tmp353;union
Cyc_Absyn_DatatypeInfoU _tmp354;struct Cyc_Absyn_Datatypedecl**_tmp355;struct Cyc_Absyn_Datatypedecl*
_tmp356;struct Cyc_Absyn_AggrInfo _tmp358;union Cyc_Absyn_AggrInfoU _tmp359;struct
Cyc_Absyn_Aggrdecl**_tmp35A;struct Cyc_Absyn_Aggrdecl*_tmp35B;_LL171: {struct Cyc_Absyn_IntType_struct*
_tmp34C=(struct Cyc_Absyn_IntType_struct*)_tmp34B;if(_tmp34C->tag != 6)goto _LL173;
else{_tmp34D=_tmp34C->f2;if(_tmp34D != Cyc_Absyn_Char_sz)goto _LL173;}}_LL172:{int
i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp35C=Cyc_Tcpat_char_con(r,(char)i,(
struct Cyc_Absyn_Pat*)_tmp34A);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2F5,_tmp35C)){const char*_tmp5BB;void*_tmp5BA[1];struct
Cyc_Int_pa_struct _tmp5B9;return(struct _dyneither_ptr)((_tmp5B9.tag=1,((_tmp5B9.f1=(
unsigned long)i,((_tmp5BA[0]=& _tmp5B9,Cyc_aprintf(((_tmp5BB="%d",_tag_dyneither(
_tmp5BB,sizeof(char),3))),_tag_dyneither(_tmp5BA,sizeof(void*),1))))))));}}}(int)
_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL173: {struct Cyc_Absyn_IntType_struct*
_tmp34E=(struct Cyc_Absyn_IntType_struct*)_tmp34B;if(_tmp34E->tag != 6)goto _LL175;}
_LL174:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp360=Cyc_Tcpat_int_con(
r,(int)i,_tmp34A);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp2F5,_tmp360)){const char*_tmp5BF;void*_tmp5BE[1];struct Cyc_Int_pa_struct
_tmp5BD;return(struct _dyneither_ptr)((_tmp5BD.tag=1,((_tmp5BD.f1=(unsigned long)((
int)i),((_tmp5BE[0]=& _tmp5BD,Cyc_aprintf(((_tmp5BF="%d",_tag_dyneither(_tmp5BF,
sizeof(char),3))),_tag_dyneither(_tmp5BE,sizeof(void*),1))))))));}}}(int)_throw((
void*)& Cyc_Tcpat_Desc2string_val);_LL175: {struct Cyc_Absyn_PointerType_struct*
_tmp34F=(struct Cyc_Absyn_PointerType_struct*)_tmp34B;if(_tmp34F->tag != 5)goto
_LL177;else{_tmp350=_tmp34F->f1;_tmp351=_tmp350.ptr_atts;}}_LL176: {union Cyc_Absyn_Constraint*
_tmp364=_tmp351.nullable;int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp364);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*
s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F5,Cyc_Tcpat_null_con(r,(
struct Cyc_Absyn_Pat*)_tmp34A))){const char*_tmp5C0;return(_tmp5C0="NULL",
_tag_dyneither(_tmp5C0,sizeof(char),5));}}{const char*_tmp5C1;return(_tmp5C1="&_",
_tag_dyneither(_tmp5C1,sizeof(char),3));};}_LL177: {struct Cyc_Absyn_DatatypeType_struct*
_tmp352=(struct Cyc_Absyn_DatatypeType_struct*)_tmp34B;if(_tmp352->tag != 3)goto
_LL179;else{_tmp353=_tmp352->f1;_tmp354=_tmp353.datatype_info;if((_tmp354.KnownDatatype).tag
!= 2)goto _LL179;_tmp355=(struct Cyc_Absyn_Datatypedecl**)(_tmp354.KnownDatatype).val;
_tmp356=*_tmp355;}}_LL178: if(_tmp356->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp367=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp356->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp367);for(0;(unsigned int)_tmp367;_tmp367=_tmp367->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp367->hd)->name).f2;struct Cyc_List_List*
_tmp368=((struct Cyc_Absyn_Datatypefield*)_tmp367->hd)->typs;struct Cyc_Tcpat_Con_s*
_tmp5C2;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F5,(struct Cyc_Tcpat_Con_s*)((_tmp5C2=_cycalloc(sizeof(*_tmp5C2)),((_tmp5C2->name=
Cyc_Tcpat_Name_v(n),((_tmp5C2->arity=0,((_tmp5C2->span=0,((_tmp5C2->orig_pat=
_tmp34A,_tmp5C2)))))))))))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp368)== 0)return n;else{const char*_tmp5C6;void*_tmp5C5[1];struct Cyc_String_pa_struct
_tmp5C4;return(struct _dyneither_ptr)((_tmp5C4.tag=0,((_tmp5C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5C5[0]=& _tmp5C4,Cyc_aprintf(((
_tmp5C6="%s(...)",_tag_dyneither(_tmp5C6,sizeof(char),8))),_tag_dyneither(
_tmp5C5,sizeof(void*),1))))))));}}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};
_LL179:{struct Cyc_Absyn_AggrType_struct*_tmp357=(struct Cyc_Absyn_AggrType_struct*)
_tmp34B;if(_tmp357->tag != 11)goto _LL17B;else{_tmp358=_tmp357->f1;_tmp359=_tmp358.aggr_info;
if((_tmp359.KnownAggr).tag != 2)goto _LL17B;_tmp35A=(struct Cyc_Absyn_Aggrdecl**)(
_tmp359.KnownAggr).val;_tmp35B=*_tmp35A;}}if(!(_tmp35B->kind == Cyc_Absyn_UnionA))
goto _LL17B;_LL17A: {struct Cyc_List_List*_tmp36D=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp35B->impl))->fields;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp36D);struct _tuple1 _tmp36F;struct _dyneither_ptr*_tmp370;struct _dyneither_ptr
_tmp371;struct _tuple1*_tmp36E=_tmp35B->name;_tmp36F=*_tmp36E;_tmp370=_tmp36F.f2;
_tmp371=*_tmp370;for(0;(unsigned int)_tmp36D;_tmp36D=_tmp36D->tl){struct
_dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp36D->hd)->name;struct Cyc_Tcpat_Con_s*
_tmp5C7;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F5,(struct Cyc_Tcpat_Con_s*)((_tmp5C7=_cycalloc(sizeof(*_tmp5C7)),((_tmp5C7->name=
Cyc_Tcpat_Name_v(n),((_tmp5C7->arity=0,((_tmp5C7->span=0,((_tmp5C7->orig_pat=
_tmp34A,_tmp5C7)))))))))))){const char*_tmp5CC;void*_tmp5CB[2];struct Cyc_String_pa_struct
_tmp5CA;struct Cyc_String_pa_struct _tmp5C9;return(struct _dyneither_ptr)((_tmp5C9.tag=
0,((_tmp5C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5CA.tag=0,((
_tmp5CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp371),((_tmp5CB[0]=&
_tmp5CA,((_tmp5CB[1]=& _tmp5C9,Cyc_aprintf(((_tmp5CC="%s{.%s = _}",_tag_dyneither(
_tmp5CC,sizeof(char),12))),_tag_dyneither(_tmp5CB,sizeof(void*),2))))))))))))));}}(
int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL17B:;_LL17C:(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);_LL170:;};};_LL145:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(
struct _RegionHandle*r,void*d){struct _dyneither_ptr*_tmp5CD;return(_tmp5CD=
_cycalloc(sizeof(*_tmp5CD)),((_tmp5CD[0]=Cyc_Tcpat_desc2string(r,d),_tmp5CD)));}
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",
sizeof(char),2);{struct Cyc_List_List*_tmp379=_tmp378;for(0;_tmp379 != 0;_tmp379=((
struct Cyc_List_List*)_check_null(_tmp379))->tl){if(_tmp379->tl != 0){{struct Cyc_List_List*
_tmp5CE;_tmp379->tl=(struct Cyc_List_List*)((_tmp5CE=_cycalloc(sizeof(*_tmp5CE)),((
_tmp5CE->hd=comma,((_tmp5CE->tl=_tmp379->tl,_tmp5CE))))));}_tmp379=_tmp379->tl;}}}
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp378);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp37C;_push_handler(& _tmp37C);{int _tmp37E=0;if(setjmp(_tmp37C.handler))
_tmp37E=1;if(!_tmp37E){{struct _dyneither_ptr _tmp37F=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5D2;void*_tmp5D1[1];struct Cyc_String_pa_struct _tmp5D0;(_tmp5D0.tag=
0,((_tmp5D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37F),((_tmp5D1[0]=&
_tmp5D0,Cyc_Tcutil_terr(loc,((_tmp5D2="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5D2,sizeof(char),53))),_tag_dyneither(_tmp5D1,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp37D=(void*)_exn_thrown;void*_tmp384=_tmp37D;_LL17E: {
struct Cyc_Tcpat_Desc2string_struct*_tmp385=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp384;if(_tmp385->tag != Cyc_Tcpat_Desc2string)goto _LL180;}_LL17F:{const char*
_tmp5D5;void*_tmp5D4;(_tmp5D4=0,Cyc_Tcutil_terr(loc,((_tmp5D5="patterns may not be exhaustive.",
_tag_dyneither(_tmp5D5,sizeof(char),32))),_tag_dyneither(_tmp5D4,sizeof(void*),0)));}
goto _LL17D;_LL180:;_LL181:(void)_throw(_tmp384);_LL17D:;}};}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple19*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp388=Cyc_Tcenv_get_fnrgn(te);int _tmp389=0;struct _tuple23
_tmp5D6;struct _tuple23 _tmp38A=(_tmp5D6.f1=_tmp388,((_tmp5D6.f2=& _tmp389,_tmp5D6)));
struct Cyc_List_List*_tmp38B=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple22*(*f)(struct _tuple23*,struct Cyc_Absyn_Switch_clause*),struct
_tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp388,Cyc_Tcpat_get_match,&
_tmp38A,swcs);void*_tmp38C=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp388,_tmp38B);int _tmp38D=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple19*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp388,_tmp38C,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp38D);for(0;_tmp38B != 0;_tmp38B=_tmp38B->tl){
struct _tuple22 _tmp38F;struct _tuple19*_tmp390;struct _tuple19 _tmp391;int _tmp392;
struct Cyc_Position_Segment*_tmp393;struct _tuple22*_tmp38E=(struct _tuple22*)
_tmp38B->hd;_tmp38F=*_tmp38E;_tmp390=_tmp38F.f2;_tmp391=*_tmp390;_tmp392=_tmp391.f1;
_tmp393=_tmp391.f2;if(!_tmp392){{const char*_tmp5D9;void*_tmp5D8;(_tmp5D8=0,Cyc_Tcutil_terr(
_tmp393,((_tmp5D9="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5D9,sizeof(char),74))),_tag_dyneither(_tmp5D8,sizeof(void*),0)));}
break;}}}struct _tuple24{struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _RegionHandle*r,struct _tuple24*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp397;_push_handler(& _tmp397);{int _tmp399=0;if(setjmp(_tmp397.handler))
_tmp399=1;if(!_tmp399){{struct _dyneither_ptr _tmp39A=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5DD;void*_tmp5DC[1];struct Cyc_String_pa_struct _tmp5DB;(_tmp5DB.tag=
0,((_tmp5DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39A),((_tmp5DC[0]=&
_tmp5DB,Cyc_Tcutil_warn((*pr).f1,((_tmp5DD="pattern not exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5DD,sizeof(char),45))),_tag_dyneither(_tmp5DC,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp398=(void*)_exn_thrown;void*_tmp39F=_tmp398;_LL183: {
struct Cyc_Tcpat_Desc2string_struct*_tmp3A0=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp39F;if(_tmp3A0->tag != Cyc_Tcpat_Desc2string)goto _LL185;}_LL184:{const char*
_tmp5E0;void*_tmp5DF;(_tmp5DF=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5E0="pattern not exhaustive.",
_tag_dyneither(_tmp5E0,sizeof(char),24))),_tag_dyneither(_tmp5DF,sizeof(void*),0)));}
goto _LL182;_LL185:;_LL186:(void)_throw(_tmp39F);_LL182:;}};};}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple25{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle*_tmp3A3=Cyc_Tcenv_get_fnrgn(te);struct _tuple25*_tmp5E3;
struct Cyc_List_List*_tmp5E2;struct Cyc_List_List*_tmp3A4=(_tmp5E2=_region_malloc(
_tmp3A3,sizeof(*_tmp5E2)),((_tmp5E2->hd=((_tmp5E3=_region_malloc(_tmp3A3,sizeof(*
_tmp5E3)),((_tmp5E3->f1=Cyc_Tcpat_compile_pat(_tmp3A3,p),((_tmp5E3->f2=0,_tmp5E3)))))),((
_tmp5E2->tl=0,_tmp5E2)))));void*_tmp3A5=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp3A3,_tmp3A4);struct
_tuple24 _tmp5E4;struct _tuple24 _tmp3A6=(_tmp5E4.f1=loc,((_tmp5E4.f2=1,_tmp5E4)));
int _tmp3A7=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct
_RegionHandle*,struct _tuple24*,void*),struct _tuple24*env1,void(*rhs_appears)(int,
int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3A3,_tmp3A5,
Cyc_Tcpat_not_exhaust_warn,& _tmp3A6,Cyc_Tcpat_dummy_fn,0,& _tmp3A7);return _tmp3A6.f2;}
static struct _tuple22*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple19*_tmp5E5;
struct _tuple19*rhs=(_tmp5E5=_region_malloc(r,sizeof(*_tmp5E5)),((_tmp5E5->f1=0,((
_tmp5E5->f2=(swc->pattern)->loc,_tmp5E5)))));struct _tuple22*_tmp5E6;return(
_tmp5E6=_region_malloc(r,sizeof(*_tmp5E6)),((_tmp5E6->f1=sp0,((_tmp5E6->f2=rhs,
_tmp5E6)))));}static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){struct _RegionHandle*
_tmp3AD=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple22*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3AD,Cyc_Tcpat_get_match2,
_tmp3AD,swcs);void*_tmp3AF=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp3AD,_tmp3AE);int _tmp3B0=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple19*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3AD,_tmp3AF,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp3B0);for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){
struct _tuple22 _tmp3B2;struct _tuple19*_tmp3B3;struct _tuple19 _tmp3B4;int _tmp3B5;
struct Cyc_Position_Segment*_tmp3B6;struct _tuple22*_tmp3B1=(struct _tuple22*)
_tmp3AE->hd;_tmp3B2=*_tmp3B1;_tmp3B3=_tmp3B2.f2;_tmp3B4=*_tmp3B3;_tmp3B5=_tmp3B4.f1;
_tmp3B6=_tmp3B4.f2;if(!_tmp3B5){{const char*_tmp5E9;void*_tmp5E8;(_tmp5E8=0,Cyc_Tcutil_terr(
_tmp3B6,((_tmp5E9="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5E9,sizeof(char),74))),_tag_dyneither(_tmp5E8,sizeof(void*),0)));}
break;}}}
