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
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;
int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;
int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;
int print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
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
void*)& Cyc_Absyn_Wild_p_val;{const char*_tmp3B7;void*_tmp3B6;(_tmp3B6=0,Cyc_Tcutil_terr(
p->loc,((_tmp3B7="struct tag used without arguments in pattern",_tag_dyneither(
_tmp3B7,sizeof(char),45))),_tag_dyneither(_tmp3B6,sizeof(void*),0)));}
_npop_handler(0);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp20=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp1E;if(_tmp20->tag != 2)goto _LL12;else{
_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL11:{struct Cyc_Absyn_Datatype_p_struct
_tmp3BA;struct Cyc_Absyn_Datatype_p_struct*_tmp3B9;p->r=(void*)((_tmp3B9=
_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3BA.tag=7,((_tmp3BA.f1=_tmp21,((
_tmp3BA.f2=_tmp22,((_tmp3BA.f3=0,((_tmp3BA.f4=0,_tmp3BA)))))))))),_tmp3B9))));}
_npop_handler(0);return;_LL12: {struct Cyc_Tcenv_EnumRes_struct*_tmp23=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp1E;if(_tmp23->tag != 3)goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}
_LL13:{struct Cyc_Absyn_Enum_p_struct _tmp3BD;struct Cyc_Absyn_Enum_p_struct*
_tmp3BC;p->r=(void*)((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC[0]=((_tmp3BD.tag=
12,((_tmp3BD.f1=_tmp24,((_tmp3BD.f2=_tmp25,_tmp3BD)))))),_tmp3BC))));}
_npop_handler(0);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp26=(
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp1E;if(_tmp26->tag != 4)goto _LL16;else{
_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}_LL15:{struct Cyc_Absyn_AnonEnum_p_struct
_tmp3C0;struct Cyc_Absyn_AnonEnum_p_struct*_tmp3BF;p->r=(void*)((_tmp3BF=
_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF[0]=((_tmp3C0.tag=13,((_tmp3C0.f1=(void*)
_tmp27,((_tmp3C0.f2=_tmp28,_tmp3C0)))))),_tmp3BF))));}_npop_handler(0);return;
_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp29=(struct Cyc_Tcenv_VarRes_struct*)
_tmp1E;if(_tmp29->tag != 0)goto _LLD;}_LL17: goto _LLD;_LLD:;};_pop_handler();}else{
void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {struct Cyc_Dict_Absent_struct*
_tmp34=(struct Cyc_Dict_Absent_struct*)_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)
goto _LL1B;}_LL1A: goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{union
Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;_LL1E:
if((_tmp35.Loc_n).tag != 4)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F: goto
_LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(
_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:(*_tmp2).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_Var_p_struct _tmp3C3;struct Cyc_Absyn_Var_p_struct*_tmp3C2;p->r=(
void*)((_tmp3C2=_cycalloc(sizeof(*_tmp3C2)),((_tmp3C2[0]=((_tmp3C3.tag=1,((
_tmp3C3.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((
_tmp3C3.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3C3)))))),
_tmp3C2))));}return;_LL22:;_LL23:{const char*_tmp3C6;void*_tmp3C5;(_tmp3C5=0,Cyc_Tcutil_terr(
p->loc,((_tmp3C6="qualified variable in pattern",_tag_dyneither(_tmp3C6,sizeof(
char),30))),_tag_dyneither(_tmp3C5,sizeof(void*),0)));}return;_LL1D:;};_LL3: {
struct Cyc_Absyn_UnknownCall_p_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=
_tmp3->f3;}}_LL4:{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;
if(setjmp(_tmp3C.handler))_tmp3E=1;if(!_tmp3E){{struct _RegionHandle*_tmp3F=Cyc_Tcenv_get_fnrgn(
te);void*_tmp40=Cyc_Tcenv_lookup_ordinary(_tmp3F,te,p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*
_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;
_LL25: {struct Cyc_Tcenv_AggrRes_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {struct Cyc_List_List*
_tmp49=0;for(0;_tmp5 != 0;_tmp5=_tmp5->tl){struct _tuple14*_tmp3C9;struct Cyc_List_List*
_tmp3C8;_tmp49=((_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8->hd=((_tmp3C9=
_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9->f1=0,((_tmp3C9->f2=(struct Cyc_Absyn_Pat*)
_tmp5->hd,_tmp3C9)))))),((_tmp3C8->tl=_tmp49,_tmp3C8))))));}{struct Cyc_Absyn_Aggr_p_struct
_tmp3D3;struct Cyc_Absyn_Aggrdecl**_tmp3D2;struct Cyc_Absyn_AggrInfo*_tmp3D1;
struct Cyc_Absyn_Aggr_p_struct*_tmp3D0;p->r=(void*)((_tmp3D0=_cycalloc(sizeof(*
_tmp3D0)),((_tmp3D0[0]=((_tmp3D3.tag=6,((_tmp3D3.f1=((_tmp3D1=_cycalloc(sizeof(*
_tmp3D1)),((_tmp3D1->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3D2=_cycalloc(sizeof(*
_tmp3D2)),((_tmp3D2[0]=_tmp42,_tmp3D2))))),((_tmp3D1->targs=0,_tmp3D1)))))),((
_tmp3D3.f2=0,((_tmp3D3.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp49),((_tmp3D3.f4=_tmp6,_tmp3D3)))))))))),_tmp3D0))));}_npop_handler(0);
return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp43=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp40;if(_tmp43->tag != 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}
_LL28:{struct Cyc_Absyn_Datatype_p_struct _tmp3D6;struct Cyc_Absyn_Datatype_p_struct*
_tmp3D5;p->r=(void*)((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=((_tmp3D6.tag=
7,((_tmp3D6.f1=_tmp44,((_tmp3D6.f2=_tmp45,((_tmp3D6.f3=_tmp5,((_tmp3D6.f4=_tmp6,
_tmp3D6)))))))))),_tmp3D5))));}_npop_handler(0);return;_LL29: {struct Cyc_Tcenv_EnumRes_struct*
_tmp46=(struct Cyc_Tcenv_EnumRes_struct*)_tmp40;if(_tmp46->tag != 3)goto _LL2B;}
_LL2A: goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp47=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp40;if(_tmp47->tag != 4)goto _LL2D;}_LL2C:{const char*_tmp3D9;void*_tmp3D8;(
_tmp3D8=0,Cyc_Tcutil_terr(p->loc,((_tmp3D9="enum tag used with arguments in pattern",
_tag_dyneither(_tmp3D9,sizeof(char),40))),_tag_dyneither(_tmp3D8,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;_npop_handler(0);return;_LL2D: {struct Cyc_Tcenv_VarRes_struct*
_tmp48=(struct Cyc_Tcenv_VarRes_struct*)_tmp40;if(_tmp48->tag != 0)goto _LL24;}
_LL2E: goto _LL24;_LL24:;};_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;
void*_tmp55=_tmp3D;_LL30: {struct Cyc_Dict_Absent_struct*_tmp56=(struct Cyc_Dict_Absent_struct*)
_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31: goto _LL2F;_LL32:;_LL33:(
void)_throw(_tmp55);_LL2F:;}};}{const char*_tmp3DD;void*_tmp3DC[1];struct Cyc_String_pa_struct
_tmp3DB;(_tmp3DB.tag=0,((_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3DC[0]=& _tmp3DB,Cyc_Tcutil_terr(p->loc,((
_tmp3DD="%s is not a constructor in pattern",_tag_dyneither(_tmp3DD,sizeof(char),
35))),_tag_dyneither(_tmp3DC,sizeof(void*),1)))))));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmp7->tag != 6)goto _LL7;else{_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;
_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=_tmp7->f4;}}_LL6: if(topt == 0){const char*
_tmp3E0;void*_tmp3DF;(_tmp3DF=0,Cyc_Tcutil_terr(p->loc,((_tmp3E0="cannot determine pattern type",
_tag_dyneither(_tmp3E0,sizeof(char),30))),_tag_dyneither(_tmp3DF,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));{void*_tmp5C=t;struct
Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct Cyc_Absyn_AggrType_struct*_tmp5D=(struct
Cyc_Absyn_AggrType_struct*)_tmp5C;if(_tmp5D->tag != 12)goto _LL37;else{_tmp5E=
_tmp5D->f1;}}_LL36:{struct Cyc_Absyn_Aggr_p_struct _tmp3E6;struct Cyc_Absyn_AggrInfo*
_tmp3E5;struct Cyc_Absyn_Aggr_p_struct*_tmp3E4;p->r=(void*)((_tmp3E4=_cycalloc(
sizeof(*_tmp3E4)),((_tmp3E4[0]=((_tmp3E6.tag=6,((_tmp3E6.f1=((_tmp3E5=_cycalloc(
sizeof(*_tmp3E5)),((_tmp3E5[0]=_tmp5E,_tmp3E5)))),((_tmp3E6.f2=_tmp9,((_tmp3E6.f3=
_tmpA,((_tmp3E6.f4=_tmpB,_tmp3E6)))))))))),_tmp3E4))));}Cyc_Tcpat_resolve_pat(te,
topt,p);goto _LL34;_LL37:;_LL38:{const char*_tmp3EA;void*_tmp3E9[1];struct Cyc_String_pa_struct
_tmp3E8;(_tmp3E8.tag=0,((_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3E9[0]=& _tmp3E8,Cyc_Tcutil_terr(p->loc,((
_tmp3EA="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3EA,
sizeof(char),45))),_tag_dyneither(_tmp3E9,sizeof(void*),1)))))));}goto _LL34;
_LL34:;}return;};_LL7: {struct Cyc_Absyn_Aggr_p_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;
_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;
_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;
_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;
_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:{struct _handler_cons _tmp65;
_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))_tmp67=1;if(!
_tmp67){{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmp12);struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;if(_tmp69->impl == 0){{const char*
_tmp3F2;void*_tmp3F1[1];const char*_tmp3F0;const char*_tmp3EF;struct Cyc_String_pa_struct
_tmp3EE;(_tmp3EE.tag=0,((_tmp3EE.f1=(struct _dyneither_ptr)(_tmp69->kind == Cyc_Absyn_StructA?(
_tmp3EF="struct",_tag_dyneither(_tmp3EF,sizeof(char),7)):((_tmp3F0="union",
_tag_dyneither(_tmp3F0,sizeof(char),6)))),((_tmp3F1[0]=& _tmp3EE,Cyc_Tcutil_terr(
p->loc,((_tmp3F2="can't destructure an abstract %s",_tag_dyneither(_tmp3F2,
sizeof(char),33))),_tag_dyneither(_tmp3F1,sizeof(void*),1)))))));}p->r=(void*)&
Cyc_Absyn_Wild_p_val;_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp15);if(more_exists < 0){{
const char*_tmp3F5;void*_tmp3F4;(_tmp3F4=0,Cyc_Tcutil_terr(p->loc,((_tmp3F5="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp3F5,sizeof(char),55))),_tag_dyneither(_tmp3F4,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp71=_tmp15;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars);for(0;n != 
0;-- n){_tmp71=&((struct Cyc_List_List*)_check_null(*_tmp71))->tl;}}*_tmp71=0;};}
else{if(more_exists > 0){struct Cyc_List_List*_tmp72=0;for(0;more_exists != 0;--
more_exists){struct Cyc_Absyn_Unknown_kb_struct*_tmp3FB;struct Cyc_Absyn_Unknown_kb_struct
_tmp3FA;struct Cyc_List_List*_tmp3F9;_tmp72=((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((
_tmp3F9->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((
_tmp3FB[0]=((_tmp3FA.tag=1,((_tmp3FA.f1=0,_tmp3FA)))),_tmp3FB))))),((_tmp3F9->tl=
_tmp72,_tmp3F9))))));}*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp15,_tmp72);}}{struct Cyc_Absyn_Aggr_p_struct
_tmp401;struct Cyc_Absyn_AggrInfo*_tmp400;struct Cyc_Absyn_Aggr_p_struct*_tmp3FF;p->r=(
void*)((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF[0]=((_tmp401.tag=6,((
_tmp401.f1=((_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400->aggr_info=Cyc_Absyn_KnownAggr(
_tmp68),((_tmp400->targs=_tmp13,_tmp400)))))),((_tmp401.f2=*_tmp15,((_tmp401.f3=
_tmp16,((_tmp401.f4=_tmp17,_tmp401)))))))))),_tmp3FF))));};};};_pop_handler();}
else{void*_tmp66=(void*)_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {struct Cyc_Dict_Absent_struct*
_tmp7B=(struct Cyc_Dict_Absent_struct*)_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)
goto _LL3C;}_LL3B:{const char*_tmp404;void*_tmp403;(_tmp403=0,Cyc_Tcutil_terr(p->loc,((
_tmp404="Non-aggregate name has designator patterns",_tag_dyneither(_tmp404,
sizeof(char),43))),_tag_dyneither(_tmp403,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}return;_LL9: {struct Cyc_Absyn_Exp_p_struct*
_tmp18=(struct Cyc_Absyn_Exp_p_struct*)_tmp0;if(_tmp18->tag != 16)goto _LLB;else{
_tmp19=_tmp18->f1;}}_LLA: Cyc_Tcexp_tcExp(te,0,_tmp19);if(!Cyc_Tcutil_is_const_exp(
te,_tmp19)){{const char*_tmp407;void*_tmp406;(_tmp406=0,Cyc_Tcutil_terr(p->loc,((
_tmp407="non-constant expression in case pattern",_tag_dyneither(_tmp407,sizeof(
char),40))),_tag_dyneither(_tmp406,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
unsigned int _tmp81;int _tmp82;struct _tuple13 _tmp80=Cyc_Evexp_eval_const_uint_exp(
_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{struct Cyc_Absyn_Int_p_struct _tmp40A;
struct Cyc_Absyn_Int_p_struct*_tmp409;p->r=(void*)((_tmp409=_cycalloc_atomic(
sizeof(*_tmp409)),((_tmp409[0]=((_tmp40A.tag=9,((_tmp40A.f1=Cyc_Absyn_None,((
_tmp40A.f2=(int)_tmp81,_tmp40A)))))),_tmp409))));}return;};_LLB:;_LLC: return;
_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**
topt){if(topt != 0)return*topt;{struct Cyc_Core_Opt*_tmp40B;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((
_tmp40B->v=s,_tmp40B)))));};}static void*Cyc_Tcpat_num_type(void**topt,void*numt){
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp86=Cyc_Tcutil_compress(
numt);_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmp87=(struct Cyc_Absyn_EnumType_struct*)
_tmp86;if(_tmp87->tag != 14)goto _LL41;}_LL40: if(topt != 0)return*topt;goto _LL3E;
_LL41:;_LL42: goto _LL3E;_LL3E:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_List_List**v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(
0);{struct Cyc_List_List*_tmp40C;*v_result_ptr=(struct Cyc_List_List*)((_tmp40C=
_cycalloc(sizeof(*_tmp40C)),((_tmp40C->hd=vd,((_tmp40C->tl=*v_result_ptr,_tmp40C))))));};}
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple12*_tmp8A;struct Cyc_List_List*
_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;
_tmp8B=_tmp89.patvars;{struct _tuple12*_tmp8D;struct Cyc_List_List*_tmp8E;struct
Cyc_Tcpat_TcPatResult _tmp8C=res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=
_tmp8C.patvars;if(_tmp8A != 0  || _tmp8D != 0){if(_tmp8A == 0){struct _tuple12*
_tmp40D;_tmp8A=((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D->f1=0,((_tmp40D->f2=
0,_tmp40D))))));}if(_tmp8D == 0){struct _tuple12*_tmp40E;_tmp8D=((_tmp40E=
_cycalloc(sizeof(*_tmp40E)),((_tmp40E->f1=0,((_tmp40E->f2=0,_tmp40E))))));}{
struct _tuple12*_tmp411;struct Cyc_Tcpat_TcPatResult _tmp410;return(_tmp410.tvars_and_bounds_opt=((
_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f1,(*
_tmp8D).f1),((_tmp411->f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f2,(*_tmp8D).f2),_tmp411)))))),((
_tmp410.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp410)));};}{struct Cyc_Tcpat_TcPatResult
_tmp412;return(_tmp412.tvars_and_bounds_opt=0,((_tmp412.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E),
_tmp412)));};};}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Pat*_tmp413;return(_tmp413=_cycalloc(sizeof(*_tmp413)),((
_tmp413->loc=loc,((_tmp413->topt=0,((_tmp413->r=(void*)& Cyc_Absyn_Wild_p_val,
_tmp413)))))));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple16{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,topt,p);{void*t;struct Cyc_Tcpat_TcPatResult
_tmp414;struct Cyc_Tcpat_TcPatResult res=(_tmp414.tvars_and_bounds_opt=0,((_tmp414.patvars=
0,_tmp414)));{void*_tmp95=p->r;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Pat*
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
if(rgn_opt == 0){{const char*_tmp417;void*_tmp416;(_tmp416=0,Cyc_Tcutil_terr(p->loc,((
_tmp417="* pattern would point into an unknown/unallowed region",_tag_dyneither(
_tmp417,sizeof(char),55))),_tag_dyneither(_tmp416,sizeof(void*),0)));}goto _LL43;}
else{if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp41A;void*_tmp419;(
_tmp419=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp41A="* pattern cannot take the address of an alias-free path",_tag_dyneither(
_tmp41A,sizeof(char),56))),_tag_dyneither(_tmp419,sizeof(void*),0)));}}{struct
Cyc_Absyn_PointerType_struct _tmp424;struct Cyc_Absyn_PtrAtts _tmp423;struct Cyc_Absyn_PtrInfo
_tmp422;struct Cyc_Absyn_PointerType_struct*_tmp421;Cyc_Tcpat_set_vd(_tmp9B,& res.patvars,(
void*)((_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421[0]=((_tmp424.tag=5,((
_tmp424.f1=((_tmp422.elt_typ=t,((_tmp422.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp422.ptr_atts=((_tmp423.rgn=*rgn_opt,((_tmp423.nullable=Cyc_Absyn_false_conref,((
_tmp423.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp423.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp423.ptrloc=0,_tmp423)))))))))),_tmp422)))))),_tmp424)))),_tmp421)))));}goto
_LL43;_LL4A: {struct Cyc_Absyn_TagInt_p_struct*_tmp9D=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp95;if(_tmp9D->tag != 3)goto _LL4C;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}
_LL4B: Cyc_Tcpat_set_vd(_tmp9F,& res.patvars,_tmp9F->type);{struct _RegionHandle*
_tmpD7=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Absyn_Tvar*_tmp425[1];Cyc_Tcenv_add_type_vars(
_tmpD7,p->loc,te,((_tmp425[0]=_tmp9E,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp425,sizeof(struct Cyc_Absyn_Tvar*),
1)))));}if(res.tvars_and_bounds_opt == 0){struct _tuple12*_tmp426;res.tvars_and_bounds_opt=((
_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426->f1=0,((_tmp426->f2=0,_tmp426))))));}{
struct Cyc_List_List*_tmp427;(*res.tvars_and_bounds_opt).f1=((_tmp427=_cycalloc(
sizeof(*_tmp427)),((_tmp427->hd=_tmp9E,((_tmp427->tl=(*res.tvars_and_bounds_opt).f1,
_tmp427))))));}t=Cyc_Absyn_uint_typ;goto _LL43;};_LL4C: {struct Cyc_Absyn_Int_p_struct*
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
else{_tmpA9=_tmpA8->f1;}}_LL57:{struct Cyc_Absyn_EnumType_struct _tmp42A;struct Cyc_Absyn_EnumType_struct*
_tmp429;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp429=_cycalloc(sizeof(*_tmp429)),((
_tmp429[0]=((_tmp42A.tag=14,((_tmp42A.f1=_tmpA9->name,((_tmp42A.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpA9,_tmp42A)))))),_tmp429)))));}goto _LL43;_LL58: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmpAA=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp95;if(_tmpAA->tag != 13)goto _LL5A;
else{_tmpAB=(void*)_tmpAA->f1;}}_LL59: t=Cyc_Tcpat_num_type(topt,_tmpAB);goto
_LL43;_LL5A: {struct Cyc_Absyn_Null_p_struct*_tmpAC=(struct Cyc_Absyn_Null_p_struct*)
_tmp95;if(_tmpAC->tag != 8)goto _LL5C;}_LL5B: if(topt != 0){void*_tmpDD=Cyc_Tcutil_compress(*
topt);_LL6F: {struct Cyc_Absyn_PointerType_struct*_tmpDE=(struct Cyc_Absyn_PointerType_struct*)
_tmpDD;if(_tmpDE->tag != 5)goto _LL71;}_LL70: t=*topt;goto tcpat_end;_LL71:;_LL72:
goto _LL6E;_LL6E:;}{struct Cyc_Core_Opt*_tmpDF=Cyc_Tcenv_lookup_opt_type_vars(te);{
struct Cyc_Absyn_PointerType_struct _tmp434;struct Cyc_Absyn_PtrAtts _tmp433;struct
Cyc_Absyn_PtrInfo _tmp432;struct Cyc_Absyn_PointerType_struct*_tmp431;t=(void*)((
_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431[0]=((_tmp434.tag=5,((_tmp434.f1=((
_tmp432.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmpDF),((
_tmp432.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp432.ptr_atts=((_tmp433.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmpDF),((_tmp433.nullable=Cyc_Absyn_true_conref,((
_tmp433.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp433.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp433.ptrloc=0,_tmp433)))))))))),_tmp432)))))),_tmp434)))),_tmp431))));}goto
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
_LL7D;_LL7D:;}{struct Cyc_Absyn_DatatypeType_struct _tmp43E;struct Cyc_Absyn_Datatypedecl**
_tmp43D;struct Cyc_Absyn_DatatypeInfo _tmp43C;struct Cyc_Absyn_DatatypeType_struct*
_tmp43B;struct Cyc_Absyn_DatatypeType_struct*_tmpF4=(_tmp43B=_cycalloc(sizeof(*
_tmp43B)),((_tmp43B[0]=((_tmp43E.tag=3,((_tmp43E.f1=((_tmp43C.datatype_info=Cyc_Absyn_KnownDatatype(((
_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D[0]=_tmpEF,_tmp43D))))),((_tmp43C.targs=
_tmpF1,_tmp43C)))),_tmp43E)))),_tmp43B)));((struct Cyc_Core_Opt*)_check_null(
_tmpAE->topt))->v=(void*)((void*)_tmpF4);{struct Cyc_Absyn_PointerType_struct
_tmp448;struct Cyc_Absyn_PtrAtts _tmp447;struct Cyc_Absyn_PtrInfo _tmp446;struct Cyc_Absyn_PointerType_struct*
_tmp445;t=(void*)((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445[0]=((_tmp448.tag=
5,((_tmp448.f1=((_tmp446.elt_typ=(void*)_tmpF4,((_tmp446.elt_tq=elt_const?Cyc_Absyn_const_tqual(
0): Cyc_Absyn_empty_tqual(0),((_tmp446.ptr_atts=((_tmp447.rgn=ptr_rgn,((_tmp447.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp447.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp447.zero_term=Cyc_Absyn_false_conref,((_tmp447.ptrloc=0,_tmp447)))))))))),
_tmp446)))))),_tmp448)))),_tmp445))));}goto _LL78;};_LL7B:;_LL7C: DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_struct _tmp452;struct Cyc_Absyn_PtrAtts _tmp451;struct
Cyc_Absyn_PtrInfo _tmp450;struct Cyc_Absyn_PointerType_struct*_tmp44F;t=(void*)((
_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=((_tmp452.tag=5,((_tmp452.f1=((
_tmp450.elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v,((
_tmp450.elt_tq=elt_const?Cyc_Absyn_const_tqual(0): Cyc_Absyn_empty_tqual(0),((
_tmp450.ptr_atts=((_tmp451.rgn=ptr_rgn,((_tmp451.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp451.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp451.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp451.ptrloc=0,_tmp451)))))))))),_tmp450)))))),_tmp452)))),_tmp44F))));}_LL78:;}
goto _LL43;};}_LL5E: {struct Cyc_Absyn_Tuple_p_struct*_tmpAF=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp95;if(_tmpAF->tag != 4)goto _LL60;else{_tmpB0=_tmpAF->f1;_tmpB1=(struct Cyc_List_List**)&
_tmpAF->f1;_tmpB2=_tmpAF->f2;}}_LL5F: {struct Cyc_List_List*_tmp101=*_tmpB1;
struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*
_tmp102=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp104;_LL83: {struct Cyc_Absyn_TupleType_struct*
_tmp103=(struct Cyc_Absyn_TupleType_struct*)_tmp102;if(_tmp103->tag != 11)goto
_LL85;else{_tmp104=_tmp103->f1;}}_LL84: topt_ts=_tmp104;if(_tmpB2){int _tmp105=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101);int _tmp106=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp104);if(_tmp105 < _tmp106){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp106 - _tmp105;++ i){struct Cyc_List_List*_tmp453;
wild_ps=((_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp453->tl=wild_ps,_tmp453))))));}}*_tmpB1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp101,wild_ps);_tmp101=*_tmpB1;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp104)){const char*_tmp456;void*_tmp455;(
_tmp455=0,Cyc_Tcutil_warn(p->loc,((_tmp456="unnecessary ... in tuple pattern",
_tag_dyneither(_tmp456,sizeof(char),33))),_tag_dyneither(_tmp455,sizeof(void*),0)));}}}
goto _LL82;_LL85:;_LL86: goto _LL82;_LL82:;}else{if(_tmpB2){const char*_tmp459;void*
_tmp458;(_tmp458=0,Cyc_Tcutil_terr(p->loc,((_tmp459="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp459,sizeof(char),57))),_tag_dyneither(_tmp458,sizeof(void*),0)));}}
for(0;_tmp101 != 0;_tmp101=_tmp101->tl){void**_tmp10C=0;if(topt_ts != 0){_tmp10C=(
void**)&(*((struct _tuple15*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp101->hd,_tmp10C,rgn_opt));{
struct _tuple15*_tmp45C;struct Cyc_List_List*_tmp45B;pat_ts=((_tmp45B=_cycalloc(
sizeof(*_tmp45B)),((_tmp45B->hd=((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C->f1=
Cyc_Absyn_empty_tqual(0),((_tmp45C->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmp101->hd)->topt))->v,_tmp45C)))))),((_tmp45B->tl=pat_ts,
_tmp45B))))));};}{struct Cyc_Absyn_TupleType_struct _tmp45F;struct Cyc_Absyn_TupleType_struct*
_tmp45E;t=(void*)((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp45F.tag=
11,((_tmp45F.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp45F)))),_tmp45E))));}goto _LL43;}_LL60: {struct Cyc_Absyn_Aggr_p_struct*
_tmpB3=(struct Cyc_Absyn_Aggr_p_struct*)_tmp95;if(_tmpB3->tag != 6)goto _LL62;else{
_tmpB4=_tmpB3->f1;if(_tmpB4 == 0)goto _LL62;_tmpB5=*_tmpB4;_tmpB6=_tmpB5.aggr_info;
if((_tmpB6.KnownAggr).tag != 2)goto _LL62;_tmpB7=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB6.KnownAggr).val;_tmpB8=*_tmpB7;_tmpB9=_tmpB5.targs;_tmpBA=(struct Cyc_List_List**)&(*
_tmpB3->f1).targs;_tmpBB=_tmpB3->f2;_tmpBC=_tmpB3->f3;_tmpBD=(struct Cyc_List_List**)&
_tmpB3->f3;_tmpBE=_tmpB3->f4;}}_LL61: {struct Cyc_List_List*_tmp111=*_tmpBD;const
char*_tmp461;const char*_tmp460;struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?(
_tmp461="struct",_tag_dyneither(_tmp461,sizeof(char),7)):((_tmp460="union",
_tag_dyneither(_tmp460,sizeof(char),6)));if(_tmpB8->impl == 0){{const char*_tmp465;
void*_tmp464[1];struct Cyc_String_pa_struct _tmp463;(_tmp463.tag=0,((_tmp463.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp464[0]=& _tmp463,Cyc_Tcutil_terr(
p->loc,((_tmp465="can't destructure an abstract %s",_tag_dyneither(_tmp465,
sizeof(char),33))),_tag_dyneither(_tmp464,sizeof(void*),1)))))));}t=Cyc_Absyn_wildtyp(
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
_tmp11E;goto _LL87;_LL8C:;_LL8D: {const char*_tmp468;void*_tmp467;(_tmp467=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp468="unconstrained existential type variable in aggregate",
_tag_dyneither(_tmp468,sizeof(char),53))),_tag_dyneither(_tmp467,sizeof(void*),0)));}
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
_tmp128->f1;}}_LL94:{struct Cyc_Core_Opt*_tmp469;*_tmp12A=((_tmp469=_cycalloc(
sizeof(*_tmp469)),((_tmp469->v=_tmp119,_tmp469))));}goto _LL8E;_LL8E:;}if(error){
const char*_tmp46F;void*_tmp46E[3];struct Cyc_String_pa_struct _tmp46D;struct Cyc_String_pa_struct
_tmp46C;struct Cyc_String_pa_struct _tmp46B;(_tmp46B.tag=0,((_tmp46B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(k2)),((_tmp46C.tag=
0,((_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
_tmp118)),((_tmp46D.tag=0,((_tmp46D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*tv->name),((_tmp46E[0]=& _tmp46D,((_tmp46E[1]=& _tmp46C,((_tmp46E[
2]=& _tmp46B,Cyc_Tcutil_terr(p->loc,((_tmp46F="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp46F,sizeof(char),60))),_tag_dyneither(_tmp46E,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_Absyn_VarType_struct _tmp472;struct Cyc_Absyn_VarType_struct*_tmp471;
void*vartype=(void*)((_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471[0]=((_tmp472.tag=
2,((_tmp472.f1=tv,_tmp472)))),_tmp471))));{struct Cyc_List_List*_tmp473;_tmp116=((
_tmp473=_region_malloc(rgn,sizeof(*_tmp473)),((_tmp473->hd=(void*)vartype,((
_tmp473->tl=_tmp116,_tmp473))))));}if(k2->kind == Cyc_Absyn_RgnKind){if(k2->aliasqual
== Cyc_Absyn_Aliasable){struct _tuple0*_tmp476;struct Cyc_List_List*_tmp475;
outlives_constraints=((_tmp475=_cycalloc(sizeof(*_tmp475)),((_tmp475->hd=((
_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476->f1=Cyc_Absyn_empty_effect,((
_tmp476->f2=vartype,_tmp476)))))),((_tmp475->tl=outlives_constraints,_tmp475))))));}
else{const char*_tmp479;void*_tmp478;(_tmp478=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp479="opened existential had unique or top region kind",
_tag_dyneither(_tmp479,sizeof(char),49))),_tag_dyneither(_tmp478,sizeof(void*),0)));}}};};}}
_tmp116=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp116);{struct _RegionHandle*_tmp138=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_add_type_vars(_tmp138,p->loc,te,_tmpBB);struct Cyc_List_List*
_tmp139=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple10 _tmp47A;struct _tuple10
_tmp13A=(_tmp47A.f1=_tmp139,((_tmp47A.f2=rgn,_tmp47A)));struct Cyc_List_List*
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
_tuple12*_tmp47B;res.tvars_and_bounds_opt=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((
_tmp47B->f1=0,((_tmp47B->f2=0,_tmp47B))))));}(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmpBB);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp141=0;{struct Cyc_List_List*
_tmp142=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po;for(0;
_tmp142 != 0;_tmp142=_tmp142->tl){struct _tuple0*_tmp47E;struct Cyc_List_List*
_tmp47D;_tmp141=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->hd=((_tmp47E=
_cycalloc(sizeof(*_tmp47E)),((_tmp47E->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp13F,(*((
struct _tuple0*)_tmp142->hd)).f1),((_tmp47E->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp13F,(*((struct _tuple0*)_tmp142->hd)).f2),_tmp47E)))))),((_tmp47D->tl=_tmp141,
_tmp47D))))));}}_tmp141=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp141);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp141);};}*
_tmpBA=_tmp13D;{struct Cyc_Absyn_AggrType_struct _tmp488;struct Cyc_Absyn_Aggrdecl**
_tmp487;struct Cyc_Absyn_AggrInfo _tmp486;struct Cyc_Absyn_AggrType_struct*_tmp485;
t=(void*)((_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485[0]=((_tmp488.tag=12,((
_tmp488.f1=((_tmp486.aggr_info=Cyc_Absyn_KnownAggr(((_tmp487=_cycalloc(sizeof(*
_tmp487)),((_tmp487[0]=_tmpB8,_tmp487))))),((_tmp486.targs=*_tmpBA,_tmp486)))),
_tmp488)))),_tmp485))));}if(_tmpBE  && _tmpB8->kind == Cyc_Absyn_UnionA){const char*
_tmp48B;void*_tmp48A;(_tmp48A=0,Cyc_Tcutil_warn(p->loc,((_tmp48B="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp48B,sizeof(char),43))),_tag_dyneither(_tmp48A,sizeof(void*),0)));}
else{if(_tmpBE){int _tmp14B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp111);int _tmp14C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->fields);if(_tmp14B < _tmp14C){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp14C - _tmp14B;++ i){struct _tuple14*_tmp48E;struct
Cyc_List_List*_tmp48D;wild_dps=((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D->hd=((
_tmp48E=_cycalloc(sizeof(*_tmp48E)),((_tmp48E->f1=0,((_tmp48E->f2=Cyc_Tcpat_wild_pat(
p->loc),_tmp48E)))))),((_tmp48D->tl=wild_dps,_tmp48D))))));}}*_tmpBD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp111,
wild_dps);_tmp111=*_tmpBD;}else{if(_tmp14B == _tmp14C){const char*_tmp491;void*
_tmp490;(_tmp490=0,Cyc_Tcutil_warn(p->loc,((_tmp491="unnecessary ... in struct pattern",
_tag_dyneither(_tmp491,sizeof(char),34))),_tag_dyneither(_tmp490,sizeof(void*),0)));}}}}{
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
_tmp498;void*_tmp497[4];struct Cyc_String_pa_struct _tmp496;struct Cyc_String_pa_struct
_tmp495;struct Cyc_String_pa_struct _tmp494;struct Cyc_String_pa_struct _tmp493;(
_tmp493.tag=0,((_tmp493.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp154->topt))->v)),((_tmp494.tag=0,((
_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp155)),((_tmp495.tag=0,((_tmp495.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp496.tag=0,((_tmp496.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp153->name),((_tmp497[0]=& _tmp496,((_tmp497[1]=& _tmp495,((
_tmp497[2]=& _tmp494,((_tmp497[3]=& _tmp493,Cyc_Tcutil_terr(p->loc,((_tmp498="field %s of %s pattern expects type %s != %s",
_tag_dyneither(_tmp498,sizeof(char),45))),_tag_dyneither(_tmp497,sizeof(void*),4)))))))))))))))))))))))));}};}};};}
_npop_handler(0);goto _LL43;;_pop_region(rgn);};}_LL62: {struct Cyc_Absyn_Datatype_p_struct*
_tmpBF=(struct Cyc_Absyn_Datatype_p_struct*)_tmp95;if(_tmpBF->tag != 7)goto _LL64;
else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=_tmpBF->f3;_tmpC3=(struct Cyc_List_List**)&
_tmpBF->f3;_tmpC4=_tmpBF->f4;}}_LL63: {struct Cyc_List_List*_tmp15F=*_tmpC3;
struct _RegionHandle*_tmp160=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*tqts=
_tmpC1->typs;struct Cyc_List_List*_tmp161=Cyc_Tcenv_lookup_type_vars(te);struct
_tuple10 _tmp499;struct _tuple10 _tmp162=(_tmp499.f1=_tmp161,((_tmp499.f2=_tmp160,
_tmp499)));struct Cyc_List_List*_tmp163=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp160,Cyc_Tcutil_r_make_inst_var,&
_tmp162,_tmpC0->tvs);struct Cyc_List_List*_tmp164=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple11*))Cyc_Core_snd,_tmp163);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmp49F;struct Cyc_Absyn_DatatypeFieldInfo _tmp49E;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp49D;t=(void*)((_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp49F.tag=
4,((_tmp49F.f1=((_tmp49E.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1),((
_tmp49E.targs=_tmp164,_tmp49E)))),_tmp49F)))),_tmp49D))));}if(_tmpC4){int _tmp168=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp15F);int _tmp169=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp168 < _tmp169){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp169 - _tmp168;++ i){struct Cyc_List_List*_tmp4A0;
wild_ps=((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp4A0->tl=wild_ps,_tmp4A0))))));}}*_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp15F,wild_ps);_tmp15F=*_tmpC3;}
else{if(_tmp168 == _tmp169){const char*_tmp4A4;void*_tmp4A3[1];struct Cyc_String_pa_struct
_tmp4A2;(_tmp4A2.tag=0,((_tmp4A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC0->name)),((_tmp4A3[0]=& _tmp4A2,Cyc_Tcutil_warn(p->loc,((
_tmp4A4="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4A4,sizeof(
char),37))),_tag_dyneither(_tmp4A3,sizeof(void*),1)))))));}}}for(0;_tmp15F != 0
 && tqts != 0;(_tmp15F=_tmp15F->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp16E=(
struct Cyc_Absyn_Pat*)_tmp15F->hd;void*_tmp16F=Cyc_Tcutil_rsubstitute(_tmp160,
_tmp163,(*((struct _tuple15*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp16E,(void**)& _tmp16F,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp16E->topt))->v,_tmp16F)){const char*_tmp4AA;void*_tmp4A9[3];
struct Cyc_String_pa_struct _tmp4A8;struct Cyc_String_pa_struct _tmp4A7;struct Cyc_String_pa_struct
_tmp4A6;(_tmp4A6.tag=0,((_tmp4A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp16E->topt))->v)),((
_tmp4A7.tag=0,((_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp16F)),((_tmp4A8.tag=0,((_tmp4A8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4A9[0]=& _tmp4A8,((
_tmp4A9[1]=& _tmp4A7,((_tmp4A9[2]=& _tmp4A6,Cyc_Tcutil_terr(_tmp16E->loc,((_tmp4AA="%s expects argument type %s, not %s",
_tag_dyneither(_tmp4AA,sizeof(char),36))),_tag_dyneither(_tmp4A9,sizeof(void*),3)))))))))))))))))));}}
if(_tmp15F != 0){const char*_tmp4AE;void*_tmp4AD[1];struct Cyc_String_pa_struct
_tmp4AC;(_tmp4AC.tag=0,((_tmp4AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4AD[0]=& _tmp4AC,Cyc_Tcutil_terr(p->loc,((
_tmp4AE="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4AE,
sizeof(char),47))),_tag_dyneither(_tmp4AD,sizeof(void*),1)))))));}if(tqts != 0){
const char*_tmp4B2;void*_tmp4B1[1];struct Cyc_String_pa_struct _tmp4B0;(_tmp4B0.tag=
0,((_tmp4B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC1->name)),((_tmp4B1[0]=& _tmp4B0,Cyc_Tcutil_terr(p->loc,((_tmp4B2="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp4B2,sizeof(char),46))),_tag_dyneither(_tmp4B1,sizeof(void*),1)))))));}}
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
te));goto _LL43;_LL43:;}tcpat_end: {struct Cyc_Core_Opt*_tmp4B3;p->topt=((_tmp4B3=
_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->v=t,_tmp4B3))));}return res;};}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp17E=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct
_RegionHandle*_tmp17F=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp4B4;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp17F,Cyc_Tcpat_get_name,_tmp17E.patvars),
p->loc,((_tmp4B4="pattern contains a repeated variable",_tag_dyneither(_tmp4B4,
sizeof(char),37))));}return _tmp17E;}void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p){void*_tmp181=p->r;struct Cyc_Absyn_Pat*_tmp183;struct
Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;
_LL96: {struct Cyc_Absyn_Pointer_p_struct*_tmp182=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp181;if(_tmp182->tag != 5)goto _LL98;else{_tmp183=_tmp182->f1;}}_LL97: Cyc_Tcpat_check_pat_regions(
te,_tmp183);{struct _RegionHandle*_tmp18A=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp18A,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp183->topt))->v)){{const char*
_tmp4B7;void*_tmp4B6;(_tmp4B6=0,Cyc_Tcutil_terr(p->loc,((_tmp4B7="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp4B7,sizeof(char),56))),_tag_dyneither(_tmp4B6,sizeof(void*),0)));}
return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp18A,(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){{const char*_tmp4BA;void*_tmp4B9;(_tmp4B9=0,Cyc_Tcutil_terr(
p->loc,((_tmp4BA="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp4BA,sizeof(char),53))),_tag_dyneither(_tmp4B9,sizeof(void*),0)));}
return;}{void*_tmp18F=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp191;struct Cyc_Absyn_PtrAtts _tmp192;void*_tmp193;_LLA1: {
struct Cyc_Absyn_PointerType_struct*_tmp190=(struct Cyc_Absyn_PointerType_struct*)
_tmp18F;if(_tmp190->tag != 5)goto _LLA3;else{_tmp191=_tmp190->f1;_tmp192=_tmp191.ptr_atts;
_tmp193=_tmp192.rgn;}}_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp193);
return;_LLA3:;_LLA4: {const char*_tmp4BD;void*_tmp4BC;(_tmp4BC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BD="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp4BD,sizeof(char),36))),_tag_dyneither(_tmp4BC,sizeof(void*),0)));}
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
_dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp4BE;return((_tmp4BE.Name_v).val=s,(((
_tmp4BE.Name_v).tag=1,_tmp4BE)));}union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){
union Cyc_Tcpat_Name_value _tmp4BF;return((_tmp4BF.Int_v).val=i,(((_tmp4BF.Int_v).tag=
2,_tmp4BF)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*
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
p){const char*_tmp4C2;struct Cyc_Tcpat_Con_s*_tmp4C1;return(_tmp4C1=_region_malloc(
r,sizeof(*_tmp4C1)),((_tmp4C1->name=Cyc_Tcpat_Name_v(((_tmp4C2="NULL",
_tag_dyneither(_tmp4C2,sizeof(char),5)))),((_tmp4C1->arity=0,((_tmp4C1->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4C1->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C1)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){const char*_tmp4C5;struct Cyc_Tcpat_Con_s*_tmp4C4;return(_tmp4C4=
_region_malloc(r,sizeof(*_tmp4C4)),((_tmp4C4->name=Cyc_Tcpat_Name_v(((_tmp4C5="&",
_tag_dyneither(_tmp4C5,sizeof(char),2)))),((_tmp4C4->arity=1,((_tmp4C4->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4C4->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C4)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){const char*_tmp4C8;struct Cyc_Tcpat_Con_s*_tmp4C7;return(_tmp4C7=_region_malloc(
r,sizeof(*_tmp4C7)),((_tmp4C7->name=Cyc_Tcpat_Name_v(((_tmp4C8="&",
_tag_dyneither(_tmp4C8,sizeof(char),2)))),((_tmp4C7->arity=1,((_tmp4C7->span=(
int*)& Cyc_Tcpat_one_opt,((_tmp4C7->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C7)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4C9;return(_tmp4C9=_region_malloc(r,
sizeof(*_tmp4C9)),((_tmp4C9->name=Cyc_Tcpat_Int_v(i),((_tmp4C9->arity=0,((
_tmp4C9->span=0,((_tmp4C9->orig_pat=p,_tmp4C9)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4CA;return(_tmp4CA=_region_malloc(r,sizeof(*_tmp4CA)),((
_tmp4CA->name=Cyc_Tcpat_Name_v(f),((_tmp4CA->arity=0,((_tmp4CA->span=0,((_tmp4CA->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp4CA)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4CB;
return(_tmp4CB=_region_malloc(r,sizeof(*_tmp4CB)),((_tmp4CB->name=Cyc_Tcpat_Int_v((
int)c),((_tmp4CB->arity=0,((_tmp4CB->span=(int*)& Cyc_Tcpat_twofiftysix_opt,((
_tmp4CB->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4CB)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){const char*
_tmp4CE;struct Cyc_Tcpat_Con_s*_tmp4CD;return(_tmp4CD=_region_malloc(r,sizeof(*
_tmp4CD)),((_tmp4CD->name=Cyc_Tcpat_Name_v(((_tmp4CE="$",_tag_dyneither(_tmp4CE,
sizeof(char),2)))),((_tmp4CD->arity=i,((_tmp4CD->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp4CD->orig_pat=p,_tmp4CD)))))))));}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4D1;struct
Cyc_Tcpat_Con_struct*_tmp4D0;return(void*)((_tmp4D0=_region_malloc(r,sizeof(*
_tmp4D0)),((_tmp4D0[0]=((_tmp4D1.tag=1,((_tmp4D1.f1=Cyc_Tcpat_null_con(r,p),((
_tmp4D1.f2=0,_tmp4D1)))))),_tmp4D0))));}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4D4;
struct Cyc_Tcpat_Con_struct*_tmp4D3;return(void*)((_tmp4D3=_region_malloc(r,
sizeof(*_tmp4D3)),((_tmp4D3[0]=((_tmp4D4.tag=1,((_tmp4D4.f1=Cyc_Tcpat_int_con(r,
i,p),((_tmp4D4.f2=0,_tmp4D4)))))),_tmp4D3))));}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4D7;struct Cyc_Tcpat_Con_struct*_tmp4D6;return(void*)((_tmp4D6=_region_malloc(
r,sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D7.tag=1,((_tmp4D7.f1=Cyc_Tcpat_char_con(
r,c,p),((_tmp4D7.f2=0,_tmp4D7)))))),_tmp4D6))));}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4DA;struct Cyc_Tcpat_Con_struct*_tmp4D9;return(void*)((_tmp4D9=_region_malloc(
r,sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4DA.tag=1,((_tmp4DA.f1=Cyc_Tcpat_float_con(
r,f,p),((_tmp4DA.f2=0,_tmp4DA)))))),_tmp4D9))));}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_struct
_tmp4E0;struct Cyc_List_List*_tmp4DF;struct Cyc_Tcpat_Con_struct*_tmp4DE;return(
void*)((_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE[0]=((_tmp4E0.tag=1,((
_tmp4E0.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4E0.f2=((_tmp4DF=_region_malloc(r,
sizeof(*_tmp4DF)),((_tmp4DF->hd=p,((_tmp4DF->tl=0,_tmp4DF)))))),_tmp4E0)))))),
_tmp4DE))));}static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){struct Cyc_Tcpat_Con_struct _tmp4E6;struct Cyc_List_List*_tmp4E5;struct Cyc_Tcpat_Con_struct*
_tmp4E4;return(void*)((_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4[0]=((
_tmp4E6.tag=1,((_tmp4E6.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4E6.f2=((_tmp4E5=
_region_malloc(r,sizeof(*_tmp4E5)),((_tmp4E5->hd=p,((_tmp4E5->tl=0,_tmp4E5)))))),
_tmp4E6)))))),_tmp4E4))));}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4E9;
struct Cyc_Tcpat_Con_struct*_tmp4E8;return(void*)((_tmp4E8=_region_malloc(r,
sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4E9.tag=1,((_tmp4E9.f1=Cyc_Tcpat_tuple_con(
r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4E9.f2=ss,
_tmp4E9)))))),_tmp4E8))));}static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,
struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4EA;struct Cyc_Tcpat_Con_s*c=(_tmp4EA=_region_malloc(
r,sizeof(*_tmp4EA)),((_tmp4EA->name=Cyc_Tcpat_Name_v(con_name),((_tmp4EA->arity=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4EA->span=span,((
_tmp4EA->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4EA)))))))));struct Cyc_Tcpat_Con_struct
_tmp4ED;struct Cyc_Tcpat_Con_struct*_tmp4EC;return(void*)((_tmp4EC=_region_malloc(
r,sizeof(*_tmp4EC)),((_tmp4EC[0]=((_tmp4ED.tag=1,((_tmp4ED.f1=c,((_tmp4ED.f2=ps,
_tmp4ED)))))),_tmp4EC))));}static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*
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
_LLB8:{struct Cyc_Tcpat_Any_struct _tmp4F0;struct Cyc_Tcpat_Any_struct*_tmp4EF;s=(
void*)((_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF)),((_tmp4EF[0]=((_tmp4F0.tag=0,
_tmp4F0)),_tmp4EF))));}goto _LLB4;_LLB9: {struct Cyc_Absyn_Null_p_struct*_tmp1C2=(
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
_LLE2:{struct _union_Constraint_Eq_constr*_tmp4F1;(_tmp4F1=& _tmp1ED->Eq_constr,((
_tmp4F1->tag=1,_tmp4F1->val=0)));}is_nullable=0;still_working=0;goto _LLDE;_LLE3:
_tmp1F3=*_tmp1EE;if((_tmp1F3.Eq_constr).tag != 1)goto _LLDE;_tmp1F4=(int)(_tmp1F3.Eq_constr).val;
_LLE4: is_nullable=(int)_tmp1F4;still_working=0;goto _LLDE;_LLDE:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp1CF);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLD9;};}_LLDC:;_LLDD: {const char*_tmp4F4;void*_tmp4F3;(_tmp4F3=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4F4="expecting pointertype for pattern!",_tag_dyneither(_tmp4F4,sizeof(char),
35))),_tag_dyneither(_tmp4F3,sizeof(void*),0)));}_LLD9:;}goto _LLB4;_LLC7: {
struct Cyc_Absyn_Datatype_p_struct*_tmp1D0=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp1BF;if(_tmp1D0->tag != 7)goto _LLC9;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;
_tmp1D3=_tmp1D0->f3;}}_LLC8: {int*span;{void*_tmp1F8=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLE6: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1F9=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1F8;if(_tmp1F9->tag != 3)goto
_LLE8;}_LLE7: if(_tmp1D1->is_extensible)span=0;else{int*_tmp4F5;span=((_tmp4F5=
_region_malloc(r,sizeof(*_tmp4F5)),((_tmp4F5[0]=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D1->fields))->v),
_tmp4F5))));}goto _LLE5;_LLE8: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1FA=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1F8;if(_tmp1FA->tag != 4)goto _LLEA;}
_LLE9: span=(int*)& Cyc_Tcpat_one_opt;goto _LLE5;_LLEA:;_LLEB:{const char*_tmp4F8;
void*_tmp4F7;span=((_tmp4F7=0,((int*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F8="void datatype pattern has bad type",
_tag_dyneither(_tmp4F8,sizeof(char),35))),_tag_dyneither(_tmp4F7,sizeof(void*),0))));}
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
0;fields=fields->tl){const char*_tmp4F9;int found=Cyc_strcmp((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4F9="",_tag_dyneither(_tmp4F9,
sizeof(char),1))))== 0;{struct Cyc_List_List*dlps0=_tmp1DC;for(0;!found  && dlps0
!= 0;dlps0=dlps0->tl){struct _tuple14 _tmp1FF;struct Cyc_List_List*_tmp200;struct
Cyc_Absyn_Pat*_tmp201;struct _tuple14*_tmp1FE=(struct _tuple14*)dlps0->hd;_tmp1FF=*
_tmp1FE;_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;{struct Cyc_List_List*_tmp202=
_tmp200;struct Cyc_List_List _tmp203;void*_tmp204;struct _dyneither_ptr*_tmp206;
struct Cyc_List_List*_tmp207;_LLED: if(_tmp202 == 0)goto _LLEF;_tmp203=*_tmp202;
_tmp204=(void*)_tmp203.hd;{struct Cyc_Absyn_FieldName_struct*_tmp205=(struct Cyc_Absyn_FieldName_struct*)
_tmp204;if(_tmp205->tag != 1)goto _LLEF;else{_tmp206=_tmp205->f1;}};_tmp207=
_tmp203.tl;if(_tmp207 != 0)goto _LLEF;_LLEE: if(Cyc_strptrcmp(_tmp206,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){{struct Cyc_List_List*_tmp4FA;ps=((_tmp4FA=_region_malloc(
r,sizeof(*_tmp4FA)),((_tmp4FA->hd=Cyc_Tcpat_compile_pat(r,_tmp201),((_tmp4FA->tl=
ps,_tmp4FA))))));}found=1;}goto _LLEC;_LLEF:;_LLF0: {const char*_tmp4FD;void*
_tmp4FC;(_tmp4FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4FD="bad designator(s)",_tag_dyneither(_tmp4FD,sizeof(char),18))),
_tag_dyneither(_tmp4FC,sizeof(void*),0)));}_LLEC:;};}}if(!found){const char*
_tmp500;void*_tmp4FF;(_tmp4FF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp500="bad designator",_tag_dyneither(
_tmp500,sizeof(char),15))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}}}s=Cyc_Tcpat_tuple_pat(
r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DB->impl))->tagged){const char*_tmp503;void*_tmp502;(_tmp502=0,
Cyc_Tcutil_terr(p->loc,((_tmp503="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp503,sizeof(char),47))),_tag_dyneither(_tmp502,sizeof(void*),0)));}{
int*_tmp504;int*span=(_tmp504=_region_malloc(r,sizeof(*_tmp504)),((_tmp504[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DB->impl))->fields),_tmp504)));int field_name;struct Cyc_List_List*
_tmp210=_tmp1DC;struct Cyc_List_List _tmp211;struct _tuple14*_tmp212;struct _tuple14
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List _tmp215;void*_tmp216;
struct _dyneither_ptr*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_Absyn_Pat*
_tmp21A;struct Cyc_List_List*_tmp21B;_LLF2: if(_tmp210 == 0)goto _LLF4;_tmp211=*
_tmp210;_tmp212=(struct _tuple14*)_tmp211.hd;_tmp213=*_tmp212;_tmp214=_tmp213.f1;
if(_tmp214 == 0)goto _LLF4;_tmp215=*_tmp214;_tmp216=(void*)_tmp215.hd;{struct Cyc_Absyn_FieldName_struct*
_tmp217=(struct Cyc_Absyn_FieldName_struct*)_tmp216;if(_tmp217->tag != 1)goto _LLF4;
else{_tmp218=_tmp217->f1;}};_tmp219=_tmp215.tl;if(_tmp219 != 0)goto _LLF4;_tmp21A=
_tmp213.f2;_tmp21B=_tmp211.tl;if(_tmp21B != 0)goto _LLF4;_LLF3:{struct Cyc_List_List*
_tmp505;s=Cyc_Tcpat_con_pat(r,*_tmp218,span,((_tmp505=_region_malloc(r,sizeof(*
_tmp505)),((_tmp505->hd=Cyc_Tcpat_compile_pat(r,_tmp21A),((_tmp505->tl=0,_tmp505)))))),
p);}goto _LLF1;_LLF4:;_LLF5: {const char*_tmp508;void*_tmp507;(_tmp507=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp508="bad union pattern",
_tag_dyneither(_tmp508,sizeof(char),18))),_tag_dyneither(_tmp507,sizeof(void*),0)));}
_LLF1:;};}goto _LLB4;_LLCD: {struct Cyc_Absyn_Enum_p_struct*_tmp1DD=(struct Cyc_Absyn_Enum_p_struct*)
_tmp1BF;if(_tmp1DD->tag != 12)goto _LLCF;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}
_LLCE: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1DE->fields))->v);{int*_tmp509;s=Cyc_Tcpat_con_pat(
r,*(*_tmp1DF->name).f2,((_tmp509=_region_malloc(r,sizeof(*_tmp509)),((_tmp509[0]=
span,_tmp509)))),0,p);}goto _LLB4;}_LLCF: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E0=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1BF;if(_tmp1E0->tag != 13)goto
_LLD1;else{_tmp1E1=(void*)_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LLD0: {struct Cyc_List_List*
fields;{void*_tmp221=Cyc_Tcutil_compress(_tmp1E1);struct Cyc_List_List*_tmp223;
_LLF7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp222=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp221;if(_tmp222->tag != 15)goto _LLF9;else{_tmp223=_tmp222->f1;}}_LLF8: fields=
_tmp223;goto _LLF6;_LLF9:;_LLFA: {const char*_tmp50C;void*_tmp50B;(_tmp50B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50C="bad type in AnonEnum_p",
_tag_dyneither(_tmp50C,sizeof(char),23))),_tag_dyneither(_tmp50B,sizeof(void*),0)));}
_LLF6:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);{int*
_tmp50D;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E2->name).f2,((_tmp50D=_region_malloc(r,
sizeof(*_tmp50D)),((_tmp50D[0]=span,_tmp50D)))),0,p);}goto _LLB4;};}_LLD1: {
struct Cyc_Absyn_UnknownId_p_struct*_tmp1E3=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1BF;if(_tmp1E3->tag != 14)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp1E4=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1BF;if(_tmp1E4->tag != 15)goto
_LLD5;}_LLD4: goto _LLD6;_LLD5: {struct Cyc_Absyn_Aggr_p_struct*_tmp1E5=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp1BF;if(_tmp1E5->tag != 6)goto _LLD7;}_LLD6: goto _LLD8;_LLD7: {struct Cyc_Absyn_Exp_p_struct*
_tmp1E6=(struct Cyc_Absyn_Exp_p_struct*)_tmp1BF;if(_tmp1E6->tag != 16)goto _LLB4;}
_LLD8: {struct Cyc_Tcpat_Any_struct _tmp510;struct Cyc_Tcpat_Any_struct*_tmp50F;s=(
void*)((_tmp50F=_region_malloc(r,sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp510.tag=0,
_tmp510)),_tmp50F))));}_LLB4:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;
struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes
 = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){const char*_tmp513;void*_tmp512;(_tmp512=0,Cyc_fprintf(Cyc_stderr,((
_tmp513=" ",_tag_dyneither(_tmp513,sizeof(char),2))),_tag_dyneither(_tmp512,
sizeof(void*),0)));}}static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp22B=c->name;union Cyc_Tcpat_Name_value _tmp22C=
_tmp22B;struct _dyneither_ptr _tmp22D;int _tmp22E;_LLFC: if((_tmp22C.Name_v).tag != 1)
goto _LLFE;_tmp22D=(struct _dyneither_ptr)(_tmp22C.Name_v).val;_LLFD:{const char*
_tmp517;void*_tmp516[1];struct Cyc_String_pa_struct _tmp515;(_tmp515.tag=0,((
_tmp515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22D),((_tmp516[0]=&
_tmp515,Cyc_fprintf(Cyc_stderr,((_tmp517="%s",_tag_dyneither(_tmp517,sizeof(char),
3))),_tag_dyneither(_tmp516,sizeof(void*),1)))))));}goto _LLFB;_LLFE: if((_tmp22C.Int_v).tag
!= 2)goto _LLFB;_tmp22E=(int)(_tmp22C.Int_v).val;_LLFF:{const char*_tmp51B;void*
_tmp51A[1];struct Cyc_Int_pa_struct _tmp519;(_tmp519.tag=1,((_tmp519.f1=(
unsigned long)_tmp22E,((_tmp51A[0]=& _tmp519,Cyc_fprintf(Cyc_stderr,((_tmp51B="%d",
_tag_dyneither(_tmp51B,sizeof(char),3))),_tag_dyneither(_tmp51A,sizeof(void*),1)))))));}
goto _LLFB;_LLFB:;}static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(
void*),int tab){void*_tmp235=d;void*_tmp237;struct Cyc_List_List*_tmp23A;struct Cyc_Tcpat_Con_s*
_tmp23B;void*_tmp23C;void*_tmp23D;_LL101: {struct Cyc_Tcpat_Success_struct*
_tmp236=(struct Cyc_Tcpat_Success_struct*)_tmp235;if(_tmp236->tag != 1)goto _LL103;
else{_tmp237=(void*)_tmp236->f1;}}_LL102: Cyc_Tcpat_print_tab(tab);{const char*
_tmp51E;void*_tmp51D;(_tmp51D=0,Cyc_fprintf(Cyc_stderr,((_tmp51E="Success(",
_tag_dyneither(_tmp51E,sizeof(char),9))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}
print_rhs(_tmp237);{const char*_tmp521;void*_tmp520;(_tmp520=0,Cyc_fprintf(Cyc_stderr,((
_tmp521=")\n",_tag_dyneither(_tmp521,sizeof(char),3))),_tag_dyneither(_tmp520,
sizeof(void*),0)));}goto _LL100;_LL103: {struct Cyc_Tcpat_Failure_struct*_tmp238=(
struct Cyc_Tcpat_Failure_struct*)_tmp235;if(_tmp238->tag != 0)goto _LL105;}_LL104:{
const char*_tmp524;void*_tmp523;(_tmp523=0,Cyc_fprintf(Cyc_stderr,((_tmp524="Failure\n",
_tag_dyneither(_tmp524,sizeof(char),9))),_tag_dyneither(_tmp523,sizeof(void*),0)));}
goto _LL100;_LL105: {struct Cyc_Tcpat_IfEq_struct*_tmp239=(struct Cyc_Tcpat_IfEq_struct*)
_tmp235;if(_tmp239->tag != 2)goto _LL100;else{_tmp23A=_tmp239->f1;_tmp23B=_tmp239->f2;
_tmp23C=(void*)_tmp239->f3;_tmp23D=(void*)_tmp239->f4;}}_LL106: Cyc_Tcpat_print_tab(
tab);{const char*_tmp527;void*_tmp526;(_tmp526=0,Cyc_fprintf(Cyc_stderr,((_tmp527="Access[",
_tag_dyneither(_tmp527,sizeof(char),8))),_tag_dyneither(_tmp526,sizeof(void*),0)));}
for(0;_tmp23A != 0;_tmp23A=_tmp23A->tl){{const char*_tmp52B;void*_tmp52A[1];struct
Cyc_Int_pa_struct _tmp529;(_tmp529.tag=1,((_tmp529.f1=(unsigned long)((int)
_tmp23A->hd),((_tmp52A[0]=& _tmp529,Cyc_fprintf(Cyc_stderr,((_tmp52B="%d",
_tag_dyneither(_tmp52B,sizeof(char),3))),_tag_dyneither(_tmp52A,sizeof(void*),1)))))));}
if(_tmp23A->tl != 0){const char*_tmp52E;void*_tmp52D;(_tmp52D=0,Cyc_fprintf(Cyc_stderr,((
_tmp52E=",",_tag_dyneither(_tmp52E,sizeof(char),2))),_tag_dyneither(_tmp52D,
sizeof(void*),0)));}}{const char*_tmp531;void*_tmp530;(_tmp530=0,Cyc_fprintf(Cyc_stderr,((
_tmp531="],",_tag_dyneither(_tmp531,sizeof(char),3))),_tag_dyneither(_tmp530,
sizeof(void*),0)));}Cyc_Tcpat_print_con(_tmp23B);{const char*_tmp534;void*_tmp533;(
_tmp533=0,Cyc_fprintf(Cyc_stderr,((_tmp534=",\n",_tag_dyneither(_tmp534,sizeof(
char),3))),_tag_dyneither(_tmp533,sizeof(void*),0)));}Cyc_Tcpat_print_dec_tree(
_tmp23C,print_rhs,tab + 7);Cyc_Tcpat_print_dec_tree(_tmp23D,print_rhs,tab + 7);
_LL100:;}struct _tuple18{int f1;struct Cyc_Position_Segment*f2;};static void Cyc_Tcpat_print_swrhs(
struct _tuple18*x){return;}static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp24F=td;struct Cyc_Set_Set*_tmp251;
_LL108: {struct Cyc_Tcpat_Neg_struct*_tmp250=(struct Cyc_Tcpat_Neg_struct*)_tmp24F;
if(_tmp250->tag != 1)goto _LL10A;else{_tmp251=_tmp250->f1;}}_LL109: {struct Cyc_Tcpat_Neg_struct
_tmp537;struct Cyc_Tcpat_Neg_struct*_tmp536;return(void*)((_tmp536=_region_malloc(
r,sizeof(*_tmp536)),((_tmp536[0]=((_tmp537.tag=1,((_tmp537.f1=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(
r,_tmp251,c),_tmp537)))),_tmp536))));}_LL10A: {struct Cyc_Tcpat_Pos_struct*
_tmp252=(struct Cyc_Tcpat_Pos_struct*)_tmp24F;if(_tmp252->tag != 0)goto _LL107;}
_LL10B: {const char*_tmp53A;void*_tmp539;(_tmp539=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp53A="add_neg called when td is Positive",
_tag_dyneither(_tmp53A,sizeof(char),35))),_tag_dyneither(_tmp539,sizeof(void*),0)));}
_LL107:;}static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp257=td;struct Cyc_Tcpat_Con_s*_tmp259;struct Cyc_Set_Set*
_tmp25B;_LL10D: {struct Cyc_Tcpat_Pos_struct*_tmp258=(struct Cyc_Tcpat_Pos_struct*)
_tmp257;if(_tmp258->tag != 0)goto _LL10F;else{_tmp259=_tmp258->f1;}}_LL10E: if(Cyc_Tcpat_compare_con(
c,_tmp259)== 0)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_No;}_LL10F: {struct Cyc_Tcpat_Neg_struct*
_tmp25A=(struct Cyc_Tcpat_Neg_struct*)_tmp257;if(_tmp25A->tag != 1)goto _LL10C;
else{_tmp25B=_tmp25A->f1;}}_LL110: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp25B,c))return Cyc_Tcpat_No;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp25B)+ 1)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_Maybe;}}_LL10C:;}struct
_tuple19{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp25C=ctxt;struct Cyc_List_List _tmp25D;struct _tuple19*
_tmp25E;struct _tuple19 _tmp25F;struct Cyc_Tcpat_Con_s*_tmp260;struct Cyc_List_List*
_tmp261;struct Cyc_List_List*_tmp262;_LL112: if(_tmp25C != 0)goto _LL114;_LL113:
return 0;_LL114: if(_tmp25C == 0)goto _LL111;_tmp25D=*_tmp25C;_tmp25E=(struct
_tuple19*)_tmp25D.hd;_tmp25F=*_tmp25E;_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;
_tmp262=_tmp25D.tl;_LL115: {struct _tuple19*_tmp540;struct Cyc_List_List*_tmp53F;
struct Cyc_List_List*_tmp53E;return(_tmp53E=_region_malloc(r,sizeof(*_tmp53E)),((
_tmp53E->hd=((_tmp540=_region_malloc(r,sizeof(*_tmp540)),((_tmp540->f1=_tmp260,((
_tmp540->f2=(struct Cyc_List_List*)((_tmp53F=_region_malloc(r,sizeof(*_tmp53F)),((
_tmp53F->hd=dsc,((_tmp53F->tl=_tmp261,_tmp53F)))))),_tmp540)))))),((_tmp53E->tl=
_tmp262,_tmp53E)))));}_LL111:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp266=ctxt;
struct Cyc_List_List _tmp267;struct _tuple19*_tmp268;struct _tuple19 _tmp269;struct
Cyc_Tcpat_Con_s*_tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*_tmp26C;
_LL117: if(_tmp266 != 0)goto _LL119;_LL118: {const char*_tmp543;void*_tmp542;(
_tmp542=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp543="norm_context: empty context",_tag_dyneither(_tmp543,sizeof(char),28))),
_tag_dyneither(_tmp542,sizeof(void*),0)));}_LL119: if(_tmp266 == 0)goto _LL116;
_tmp267=*_tmp266;_tmp268=(struct _tuple19*)_tmp267.hd;_tmp269=*_tmp268;_tmp26A=
_tmp269.f1;_tmp26B=_tmp269.f2;_tmp26C=_tmp267.tl;_LL11A: {struct Cyc_Tcpat_Pos_struct
_tmp546;struct Cyc_Tcpat_Pos_struct*_tmp545;return Cyc_Tcpat_augment(r,_tmp26C,(
void*)((_tmp545=_region_malloc(r,sizeof(*_tmp545)),((_tmp545[0]=((_tmp546.tag=0,((
_tmp546.f1=_tmp26A,((_tmp546.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp26B),_tmp546)))))),_tmp545)))));}
_LL116:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple12 _tmp547;struct _tuple12
_tmp272=(_tmp547.f1=ctxt,((_tmp547.f2=work,_tmp547)));struct Cyc_List_List*
_tmp273;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*
_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List _tmp278;struct _tuple19*
_tmp279;struct _tuple19 _tmp27A;struct Cyc_Tcpat_Con_s*_tmp27B;struct Cyc_List_List*
_tmp27C;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_List_List
_tmp27F;struct _tuple17*_tmp280;struct _tuple17 _tmp281;struct Cyc_List_List*_tmp282;
struct Cyc_List_List*_tmp283;_LL11C: _tmp273=_tmp272.f1;if(_tmp273 != 0)goto _LL11E;
_tmp274=_tmp272.f2;if(_tmp274 != 0)goto _LL11E;_LL11D: return dsc;_LL11E: _tmp275=
_tmp272.f1;if(_tmp275 != 0)goto _LL120;_LL11F: goto _LL121;_LL120: _tmp276=_tmp272.f2;
if(_tmp276 != 0)goto _LL122;_LL121: {const char*_tmp54A;void*_tmp549;(_tmp549=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp54A="build_desc: ctxt and work don't match",_tag_dyneither(_tmp54A,sizeof(
char),38))),_tag_dyneither(_tmp549,sizeof(void*),0)));}_LL122: _tmp277=_tmp272.f1;
if(_tmp277 == 0)goto _LL11B;_tmp278=*_tmp277;_tmp279=(struct _tuple19*)_tmp278.hd;
_tmp27A=*_tmp279;_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;_tmp27D=_tmp278.tl;
_tmp27E=_tmp272.f2;if(_tmp27E == 0)goto _LL11B;_tmp27F=*_tmp27E;_tmp280=(struct
_tuple17*)_tmp27F.hd;_tmp281=*_tmp280;_tmp282=_tmp281.f3;_tmp283=_tmp27F.tl;
_LL123: {struct Cyc_Tcpat_Pos_struct _tmp550;struct Cyc_List_List*_tmp54F;struct Cyc_Tcpat_Pos_struct*
_tmp54E;struct Cyc_Tcpat_Pos_struct*_tmp286=(_tmp54E=_region_malloc(r,sizeof(*
_tmp54E)),((_tmp54E[0]=((_tmp550.tag=0,((_tmp550.f1=_tmp27B,((_tmp550.f2=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,_tmp27C),((_tmp54F=_region_malloc(r,sizeof(*_tmp54F)),((
_tmp54F->hd=dsc,((_tmp54F->tl=_tmp282,_tmp54F))))))),_tmp550)))))),_tmp54E)));
return Cyc_Tcpat_build_desc(r,_tmp27D,(void*)_tmp286,_tmp283);}_LL11B:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp28A=allmrules;struct
Cyc_List_List _tmp28B;struct _tuple0*_tmp28C;struct _tuple0 _tmp28D;void*_tmp28E;
void*_tmp28F;struct Cyc_List_List*_tmp290;_LL125: if(_tmp28A != 0)goto _LL127;_LL126: {
struct Cyc_Tcpat_Failure_struct _tmp553;struct Cyc_Tcpat_Failure_struct*_tmp552;
return(void*)((_tmp552=_cycalloc(sizeof(*_tmp552)),((_tmp552[0]=((_tmp553.tag=0,((
_tmp553.f1=(void*)dsc,_tmp553)))),_tmp552))));}_LL127: if(_tmp28A == 0)goto _LL124;
_tmp28B=*_tmp28A;_tmp28C=(struct _tuple0*)_tmp28B.hd;_tmp28D=*_tmp28C;_tmp28E=
_tmp28D.f1;_tmp28F=_tmp28D.f2;_tmp290=_tmp28B.tl;_LL128: return Cyc_Tcpat_match(r,
_tmp28E,0,dsc,0,0,_tmp28F,_tmp290);_LL124:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_struct
_tmp556;struct Cyc_Tcpat_Neg_struct*_tmp555;return Cyc_Tcpat_or_match(r,(void*)((
_tmp555=_region_malloc(r,sizeof(*_tmp555)),((_tmp555[0]=((_tmp556.tag=1,((
_tmp556.f1=Cyc_Tcpat_empty_con_set(r),_tmp556)))),_tmp555)))),allmrules);}static
void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp295=
work;struct Cyc_List_List _tmp296;struct _tuple17*_tmp297;struct _tuple17 _tmp298;
struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*
_tmp29B;struct Cyc_List_List*_tmp29C;struct Cyc_List_List _tmp29D;struct _tuple17*
_tmp29E;struct _tuple17 _tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*
_tmp2A1;struct Cyc_List_List*_tmp2A2;struct Cyc_List_List*_tmp2A3;_LL12A: if(
_tmp295 != 0)goto _LL12C;_LL12B: {struct Cyc_Tcpat_Success_struct _tmp559;struct Cyc_Tcpat_Success_struct*
_tmp558;return(void*)((_tmp558=_region_malloc(r,sizeof(*_tmp558)),((_tmp558[0]=((
_tmp559.tag=1,((_tmp559.f1=(void*)right_hand_side,_tmp559)))),_tmp558))));}
_LL12C: if(_tmp295 == 0)goto _LL12E;_tmp296=*_tmp295;_tmp297=(struct _tuple17*)
_tmp296.hd;_tmp298=*_tmp297;_tmp299=_tmp298.f1;if(_tmp299 != 0)goto _LL12E;_tmp29A=
_tmp298.f2;if(_tmp29A != 0)goto _LL12E;_tmp29B=_tmp298.f3;if(_tmp29B != 0)goto
_LL12E;_tmp29C=_tmp296.tl;_LL12D: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp29C,right_hand_side,rules);_LL12E: if(_tmp295 == 0)goto _LL129;_tmp29D=*
_tmp295;_tmp29E=(struct _tuple17*)_tmp29D.hd;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.f1;
_tmp2A1=_tmp29F.f2;_tmp2A2=_tmp29F.f3;_tmp2A3=_tmp29D.tl;_LL12F: if((_tmp2A0 == 0
 || _tmp2A1 == 0) || _tmp2A2 == 0){const char*_tmp55C;void*_tmp55B;(_tmp55B=0,Cyc_Tcutil_impos(((
_tmp55C="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp55C,sizeof(
char),38))),_tag_dyneither(_tmp55B,sizeof(void*),0)));}{struct Cyc_List_List
_tmp2A9;void*_tmp2AA;struct Cyc_List_List*_tmp2AB;struct Cyc_List_List*_tmp2A8=(
struct Cyc_List_List*)_tmp2A0;_tmp2A9=*_tmp2A8;_tmp2AA=(void*)_tmp2A9.hd;_tmp2AB=
_tmp2A9.tl;{struct Cyc_List_List _tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*
_tmp2AF;struct Cyc_List_List*_tmp2AC=(struct Cyc_List_List*)_tmp2A1;_tmp2AD=*
_tmp2AC;_tmp2AE=(struct Cyc_List_List*)_tmp2AD.hd;_tmp2AF=_tmp2AD.tl;{struct Cyc_List_List
_tmp2B1;void*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B0=(
struct Cyc_List_List*)_tmp2A2;_tmp2B1=*_tmp2B0;_tmp2B2=(void*)_tmp2B1.hd;_tmp2B3=
_tmp2B1.tl;{struct _tuple17*_tmp55D;struct _tuple17*_tmp2B4=(_tmp55D=
_region_malloc(r,sizeof(*_tmp55D)),((_tmp55D->f1=_tmp2AB,((_tmp55D->f2=_tmp2AF,((
_tmp55D->f3=_tmp2B3,_tmp55D)))))));struct Cyc_List_List*_tmp55E;return Cyc_Tcpat_match(
r,_tmp2AA,_tmp2AE,_tmp2B2,ctx,((_tmp55E=_region_malloc(r,sizeof(*_tmp55E)),((
_tmp55E->hd=_tmp2B4,((_tmp55E->tl=_tmp2A3,_tmp55E)))))),right_hand_side,rules);};};};};
_LL129:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp2B7=dsc;struct Cyc_Set_Set*_tmp2B9;
struct Cyc_List_List*_tmp2BB;_LL131: {struct Cyc_Tcpat_Neg_struct*_tmp2B8=(struct
Cyc_Tcpat_Neg_struct*)_tmp2B7;if(_tmp2B8->tag != 1)goto _LL133;else{_tmp2B9=
_tmp2B8->f1;}}_LL132: {struct Cyc_Tcpat_Neg_struct _tmp561;struct Cyc_Tcpat_Neg_struct*
_tmp560;void*any=(void*)((_tmp560=_region_malloc(r,sizeof(*_tmp560)),((_tmp560[0]=((
_tmp561.tag=1,((_tmp561.f1=Cyc_Tcpat_empty_con_set(r),_tmp561)))),_tmp560))));
struct Cyc_List_List*_tmp2BC=0;{int i=0;for(0;i < pcon->arity;++ i){struct Cyc_List_List*
_tmp562;_tmp2BC=((_tmp562=_region_malloc(r,sizeof(*_tmp562)),((_tmp562->hd=any,((
_tmp562->tl=_tmp2BC,_tmp562))))));}}return _tmp2BC;}_LL133: {struct Cyc_Tcpat_Pos_struct*
_tmp2BA=(struct Cyc_Tcpat_Pos_struct*)_tmp2B7;if(_tmp2BA->tag != 0)goto _LL130;
else{_tmp2BB=_tmp2BA->f2;}}_LL134: return _tmp2BB;_LL130:;}struct _tuple20{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(
struct _tuple20*env,int i){struct _tuple20 _tmp2C1;struct _RegionHandle*_tmp2C2;
struct Cyc_List_List*_tmp2C3;struct _tuple20*_tmp2C0=env;_tmp2C1=*_tmp2C0;_tmp2C2=
_tmp2C1.f1;_tmp2C3=_tmp2C1.f2;{struct Cyc_List_List*_tmp563;return(_tmp563=
_region_malloc(_tmp2C2,sizeof(*_tmp563)),((_tmp563->hd=(void*)(i + 1),((_tmp563->tl=
_tmp2C3,_tmp563)))));};}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct
_RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple20 _tmp564;struct _tuple20 _tmp2C5=(_tmp564.f1=r,((_tmp564.f2=obj,_tmp564)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple20*,int),struct _tuple20*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp2C5);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp2C7=p;struct Cyc_Tcpat_Con_s*
_tmp2CA;struct Cyc_List_List*_tmp2CB;_LL136: {struct Cyc_Tcpat_Any_struct*_tmp2C8=(
struct Cyc_Tcpat_Any_struct*)_tmp2C7;if(_tmp2C8->tag != 0)goto _LL138;}_LL137:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL138: {struct Cyc_Tcpat_Con_struct*_tmp2C9=(struct Cyc_Tcpat_Con_struct*)
_tmp2C7;if(_tmp2C9->tag != 1)goto _LL135;else{_tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2C9->f2;}}
_LL139: switch(Cyc_Tcpat_static_match(_tmp2CA,dsc)){case Cyc_Tcpat_Yes: _LL13A: {
struct _tuple19*_tmp567;struct Cyc_List_List*_tmp566;struct Cyc_List_List*ctx2=(
_tmp566=_region_malloc(r,sizeof(*_tmp566)),((_tmp566->hd=((_tmp567=
_region_malloc(r,sizeof(*_tmp567)),((_tmp567->f1=_tmp2CA,((_tmp567->f2=0,_tmp567)))))),((
_tmp566->tl=ctx,_tmp566)))));struct _tuple17*_tmp568;struct _tuple17*work_frame=(
_tmp568=_region_malloc(r,sizeof(*_tmp568)),((_tmp568->f1=_tmp2CB,((_tmp568->f2=
Cyc_Tcpat_getoargs(r,_tmp2CA,obj),((_tmp568->f3=Cyc_Tcpat_getdargs(r,_tmp2CA,dsc),
_tmp568)))))));struct Cyc_List_List*_tmp569;struct Cyc_List_List*work2=(_tmp569=
_region_malloc(r,sizeof(*_tmp569)),((_tmp569->hd=work_frame,((_tmp569->tl=work,
_tmp569)))));return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case
Cyc_Tcpat_No: _LL13B: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,
work),rules);case Cyc_Tcpat_Maybe: _LL13C: {struct _tuple19*_tmp56C;struct Cyc_List_List*
_tmp56B;struct Cyc_List_List*ctx2=(_tmp56B=_region_malloc(r,sizeof(*_tmp56B)),((
_tmp56B->hd=((_tmp56C=_region_malloc(r,sizeof(*_tmp56C)),((_tmp56C->f1=_tmp2CA,((
_tmp56C->f2=0,_tmp56C)))))),((_tmp56B->tl=ctx,_tmp56B)))));struct _tuple17*
_tmp56D;struct _tuple17*work_frame=(_tmp56D=_region_malloc(r,sizeof(*_tmp56D)),((
_tmp56D->f1=_tmp2CB,((_tmp56D->f2=Cyc_Tcpat_getoargs(r,_tmp2CA,obj),((_tmp56D->f3=
Cyc_Tcpat_getdargs(r,_tmp2CA,dsc),_tmp56D)))))));struct Cyc_List_List*_tmp56E;
struct Cyc_List_List*work2=(_tmp56E=_region_malloc(r,sizeof(*_tmp56E)),((_tmp56E->hd=
work_frame,((_tmp56E->tl=work,_tmp56E)))));void*_tmp2D0=Cyc_Tcpat_and_match(r,
ctx2,work2,right_hand_side,rules);void*_tmp2D1=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2CA),work),rules);struct Cyc_Tcpat_IfEq_struct
_tmp571;struct Cyc_Tcpat_IfEq_struct*_tmp570;return(void*)((_tmp570=
_region_malloc(r,sizeof(*_tmp570)),((_tmp570[0]=((_tmp571.tag=2,((_tmp571.f1=obj,((
_tmp571.f2=_tmp2CA,((_tmp571.f3=(void*)_tmp2D0,((_tmp571.f4=(void*)_tmp2D1,
_tmp571)))))))))),_tmp570))));}}_LL135:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp2D8=d;void*_tmp2DA;void*_tmp2DC;void*_tmp2DE;void*_tmp2DF;_LL13F: {struct Cyc_Tcpat_Failure_struct*
_tmp2D9=(struct Cyc_Tcpat_Failure_struct*)_tmp2D8;if(_tmp2D9->tag != 0)goto _LL141;
else{_tmp2DA=(void*)_tmp2D9->f1;}}_LL140: if(!(*exhaust_done)){not_exhaust(r,env1,
_tmp2DA);*exhaust_done=1;}goto _LL13E;_LL141: {struct Cyc_Tcpat_Success_struct*
_tmp2DB=(struct Cyc_Tcpat_Success_struct*)_tmp2D8;if(_tmp2DB->tag != 1)goto _LL143;
else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL142: rhs_appears(env2,_tmp2DC);goto _LL13E;
_LL143: {struct Cyc_Tcpat_IfEq_struct*_tmp2DD=(struct Cyc_Tcpat_IfEq_struct*)
_tmp2D8;if(_tmp2DD->tag != 2)goto _LL13E;else{_tmp2DE=(void*)_tmp2DD->f3;_tmp2DF=(
void*)_tmp2DD->f4;}}_LL144: Cyc_Tcpat_check_exhaust_overlap(r,_tmp2DE,not_exhaust,
env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(r,_tmp2DF,
not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13E;_LL13E:;}struct
_tuple21{void*f1;struct _tuple18*f2;};struct _tuple22{struct _RegionHandle*f1;int*
f2;};static struct _tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple22 _tmp2E1;struct _RegionHandle*_tmp2E2;int*_tmp2E3;struct
_tuple22*_tmp2E0=env;_tmp2E1=*_tmp2E0;_tmp2E2=_tmp2E1.f1;_tmp2E3=_tmp2E1.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2E2,swc->pattern);struct _tuple18*_tmp572;
struct _tuple18*rhs=(_tmp572=_region_malloc(_tmp2E2,sizeof(*_tmp572)),((_tmp572->f1=
0,((_tmp572->f2=(swc->pattern)->loc,_tmp572)))));void*sp;if(swc->where_clause != 
0){{struct Cyc_List_List*_tmp575;struct Cyc_List_List*_tmp574;sp=Cyc_Tcpat_tuple_pat(
_tmp2E2,((_tmp574=_region_malloc(_tmp2E2,sizeof(*_tmp574)),((_tmp574->hd=sp0,((
_tmp574->tl=((_tmp575=_region_malloc(_tmp2E2,sizeof(*_tmp575)),((_tmp575->hd=Cyc_Tcpat_int_pat(
_tmp2E2,*_tmp2E3,0),((_tmp575->tl=0,_tmp575)))))),_tmp574)))))),0);}*_tmp2E3=*
_tmp2E3 + 1;}else{struct Cyc_List_List*_tmp57F;struct Cyc_Tcpat_Any_struct _tmp57E;
struct Cyc_Tcpat_Any_struct*_tmp57D;struct Cyc_List_List*_tmp57C;sp=Cyc_Tcpat_tuple_pat(
_tmp2E2,((_tmp57C=_region_malloc(_tmp2E2,sizeof(*_tmp57C)),((_tmp57C->hd=sp0,((
_tmp57C->tl=((_tmp57F=_region_malloc(_tmp2E2,sizeof(*_tmp57F)),((_tmp57F->hd=(
void*)((_tmp57D=_region_malloc(_tmp2E2,sizeof(*_tmp57D)),((_tmp57D[0]=((_tmp57E.tag=
0,_tmp57E)),_tmp57D)))),((_tmp57F->tl=0,_tmp57F)))))),_tmp57C)))))),0);}{struct
_tuple21*_tmp580;return(_tmp580=_region_malloc(_tmp2E2,sizeof(*_tmp580)),((
_tmp580->f1=sp,((_tmp580->f2=rhs,_tmp580)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";
struct Cyc_Tcpat_Desc2string_struct{char*tag;};struct Cyc_Tcpat_Desc2string_struct
Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp2ED=d;struct Cyc_Tcpat_Con_s*_tmp2EF;
struct Cyc_List_List*_tmp2F0;struct Cyc_Set_Set*_tmp2F2;_LL146: {struct Cyc_Tcpat_Pos_struct*
_tmp2EE=(struct Cyc_Tcpat_Pos_struct*)_tmp2ED;if(_tmp2EE->tag != 0)goto _LL148;
else{_tmp2EF=_tmp2EE->f1;_tmp2F0=_tmp2EE->f2;}}_LL147: {union Cyc_Tcpat_Name_value
_tmp2F3=_tmp2EF->name;struct Cyc_Absyn_Pat*_tmp2F4=_tmp2EF->orig_pat;if(_tmp2F4 == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp2F0))->hd);{void*_tmp2F5=_tmp2F4->r;struct Cyc_Absyn_Vardecl*_tmp2F8;struct
Cyc_Absyn_Vardecl*_tmp2FA;struct Cyc_Absyn_Tvar*_tmp2FC;struct Cyc_Absyn_Vardecl*
_tmp2FD;struct Cyc_Absyn_AggrInfo*_tmp301;struct Cyc_Absyn_AggrInfo _tmp302;union
Cyc_Absyn_AggrInfoU _tmp303;struct Cyc_Absyn_Aggrdecl**_tmp304;struct Cyc_Absyn_Aggrdecl*
_tmp305;struct Cyc_List_List*_tmp306;struct Cyc_Absyn_Datatypefield*_tmp308;int
_tmp30B;char _tmp30D;struct _dyneither_ptr _tmp30F;struct Cyc_Absyn_Enumfield*
_tmp311;struct Cyc_Absyn_Enumfield*_tmp313;struct Cyc_Absyn_Exp*_tmp315;_LL14B: {
struct Cyc_Absyn_Wild_p_struct*_tmp2F6=(struct Cyc_Absyn_Wild_p_struct*)_tmp2F5;
if(_tmp2F6->tag != 0)goto _LL14D;}_LL14C: {const char*_tmp581;return(_tmp581="_",
_tag_dyneither(_tmp581,sizeof(char),2));}_LL14D: {struct Cyc_Absyn_Var_p_struct*
_tmp2F7=(struct Cyc_Absyn_Var_p_struct*)_tmp2F5;if(_tmp2F7->tag != 1)goto _LL14F;
else{_tmp2F8=_tmp2F7->f1;}}_LL14E: return Cyc_Absynpp_qvar2string(_tmp2F8->name);
_LL14F: {struct Cyc_Absyn_Reference_p_struct*_tmp2F9=(struct Cyc_Absyn_Reference_p_struct*)
_tmp2F5;if(_tmp2F9->tag != 2)goto _LL151;else{_tmp2FA=_tmp2F9->f1;}}_LL150: {const
char*_tmp585;void*_tmp584[1];struct Cyc_String_pa_struct _tmp583;return(struct
_dyneither_ptr)((_tmp583.tag=0,((_tmp583.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FA->name)),((_tmp584[0]=& _tmp583,Cyc_aprintf(((
_tmp585="*%s",_tag_dyneither(_tmp585,sizeof(char),4))),_tag_dyneither(_tmp584,
sizeof(void*),1))))))));}_LL151: {struct Cyc_Absyn_TagInt_p_struct*_tmp2FB=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp2F5;if(_tmp2FB->tag != 3)goto _LL153;else{
_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;}}_LL152: {const char*_tmp58A;void*
_tmp589[2];struct Cyc_String_pa_struct _tmp588;struct Cyc_String_pa_struct _tmp587;
return(struct _dyneither_ptr)((_tmp587.tag=0,((_tmp587.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2FC->name),((_tmp588.tag=0,((_tmp588.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FD->name)),((
_tmp589[0]=& _tmp588,((_tmp589[1]=& _tmp587,Cyc_aprintf(((_tmp58A="%s<`%s>",
_tag_dyneither(_tmp58A,sizeof(char),8))),_tag_dyneither(_tmp589,sizeof(void*),2))))))))))))));}
_LL153: {struct Cyc_Absyn_Tuple_p_struct*_tmp2FE=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp2F5;if(_tmp2FE->tag != 4)goto _LL155;}_LL154: {const char*_tmp58E;void*_tmp58D[
1];struct Cyc_String_pa_struct _tmp58C;return(struct _dyneither_ptr)((_tmp58C.tag=0,((
_tmp58C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2F0)),((_tmp58D[0]=& _tmp58C,Cyc_aprintf(((_tmp58E="$(%s)",_tag_dyneither(
_tmp58E,sizeof(char),6))),_tag_dyneither(_tmp58D,sizeof(void*),1))))))));}_LL155: {
struct Cyc_Absyn_Pointer_p_struct*_tmp2FF=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp2F5;if(_tmp2FF->tag != 5)goto _LL157;}_LL156: {const char*_tmp592;void*_tmp591[
1];struct Cyc_String_pa_struct _tmp590;return(struct _dyneither_ptr)((_tmp590.tag=0,((
_tmp590.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2F0)),((_tmp591[0]=& _tmp590,Cyc_aprintf(((_tmp592="&%s",_tag_dyneither(
_tmp592,sizeof(char),4))),_tag_dyneither(_tmp591,sizeof(void*),1))))))));}_LL157: {
struct Cyc_Absyn_Aggr_p_struct*_tmp300=(struct Cyc_Absyn_Aggr_p_struct*)_tmp2F5;
if(_tmp300->tag != 6)goto _LL159;else{_tmp301=_tmp300->f1;if(_tmp301 == 0)goto
_LL159;_tmp302=*_tmp301;_tmp303=_tmp302.aggr_info;if((_tmp303.KnownAggr).tag != 2)
goto _LL159;_tmp304=(struct Cyc_Absyn_Aggrdecl**)(_tmp303.KnownAggr).val;_tmp305=*
_tmp304;_tmp306=_tmp300->f3;}}_LL158: if(_tmp305->kind == Cyc_Absyn_UnionA){struct
Cyc_List_List*_tmp324=_tmp306;struct Cyc_List_List _tmp325;struct _tuple14*_tmp326;
struct _tuple14 _tmp327;struct Cyc_List_List*_tmp328;struct Cyc_List_List _tmp329;
void*_tmp32A;struct _dyneither_ptr*_tmp32C;_LL16C: if(_tmp324 == 0)goto _LL16E;
_tmp325=*_tmp324;_tmp326=(struct _tuple14*)_tmp325.hd;_tmp327=*_tmp326;_tmp328=
_tmp327.f1;if(_tmp328 == 0)goto _LL16E;_tmp329=*_tmp328;_tmp32A=(void*)_tmp329.hd;{
struct Cyc_Absyn_FieldName_struct*_tmp32B=(struct Cyc_Absyn_FieldName_struct*)
_tmp32A;if(_tmp32B->tag != 1)goto _LL16E;else{_tmp32C=_tmp32B->f1;}};_LL16D: {
const char*_tmp598;void*_tmp597[3];struct Cyc_String_pa_struct _tmp596;struct Cyc_String_pa_struct
_tmp595;struct Cyc_String_pa_struct _tmp594;return(struct _dyneither_ptr)((_tmp594.tag=
0,((_tmp594.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F0)),((_tmp595.tag=0,((_tmp595.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp32C),((_tmp596.tag=0,((_tmp596.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp305->name)),((_tmp597[0]=&
_tmp596,((_tmp597[1]=& _tmp595,((_tmp597[2]=& _tmp594,Cyc_aprintf(((_tmp598="%s{.%s = %s}",
_tag_dyneither(_tmp598,sizeof(char),13))),_tag_dyneither(_tmp597,sizeof(void*),3))))))))))))))))))));}
_LL16E:;_LL16F: goto _LL16B;_LL16B:;}{const char*_tmp59D;void*_tmp59C[2];struct Cyc_String_pa_struct
_tmp59B;struct Cyc_String_pa_struct _tmp59A;return(struct _dyneither_ptr)((_tmp59A.tag=
0,((_tmp59A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F0)),((_tmp59B.tag=0,((_tmp59B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp305->name)),((_tmp59C[0]=& _tmp59B,((
_tmp59C[1]=& _tmp59A,Cyc_aprintf(((_tmp59D="%s{%s}",_tag_dyneither(_tmp59D,
sizeof(char),7))),_tag_dyneither(_tmp59C,sizeof(void*),2))))))))))))));};_LL159: {
struct Cyc_Absyn_Datatype_p_struct*_tmp307=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp2F5;if(_tmp307->tag != 7)goto _LL15B;else{_tmp308=_tmp307->f2;}}_LL15A: if(
_tmp2F0 == 0){const char*_tmp5A1;void*_tmp5A0[1];struct Cyc_String_pa_struct _tmp59F;
return(struct _dyneither_ptr)((_tmp59F.tag=0,((_tmp59F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp308->name)),((_tmp5A0[0]=&
_tmp59F,Cyc_aprintf(((_tmp5A1="%s",_tag_dyneither(_tmp5A1,sizeof(char),3))),
_tag_dyneither(_tmp5A0,sizeof(void*),1))))))));}else{const char*_tmp5A6;void*
_tmp5A5[2];struct Cyc_String_pa_struct _tmp5A4;struct Cyc_String_pa_struct _tmp5A3;
return(struct _dyneither_ptr)((_tmp5A3.tag=0,((_tmp5A3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2F0)),((_tmp5A4.tag=0,((
_tmp5A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp308->name)),((_tmp5A5[0]=& _tmp5A4,((_tmp5A5[1]=& _tmp5A3,Cyc_aprintf(((
_tmp5A6="%s(%s)",_tag_dyneither(_tmp5A6,sizeof(char),7))),_tag_dyneither(_tmp5A5,
sizeof(void*),2))))))))))))));}_LL15B: {struct Cyc_Absyn_Null_p_struct*_tmp309=(
struct Cyc_Absyn_Null_p_struct*)_tmp2F5;if(_tmp309->tag != 8)goto _LL15D;}_LL15C: {
const char*_tmp5A7;return(_tmp5A7="NULL",_tag_dyneither(_tmp5A7,sizeof(char),5));}
_LL15D: {struct Cyc_Absyn_Int_p_struct*_tmp30A=(struct Cyc_Absyn_Int_p_struct*)
_tmp2F5;if(_tmp30A->tag != 9)goto _LL15F;else{_tmp30B=_tmp30A->f2;}}_LL15E: {const
char*_tmp5AB;void*_tmp5AA[1];struct Cyc_Int_pa_struct _tmp5A9;return(struct
_dyneither_ptr)((_tmp5A9.tag=1,((_tmp5A9.f1=(unsigned long)_tmp30B,((_tmp5AA[0]=&
_tmp5A9,Cyc_aprintf(((_tmp5AB="%d",_tag_dyneither(_tmp5AB,sizeof(char),3))),
_tag_dyneither(_tmp5AA,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_Char_p_struct*
_tmp30C=(struct Cyc_Absyn_Char_p_struct*)_tmp2F5;if(_tmp30C->tag != 10)goto _LL161;
else{_tmp30D=_tmp30C->f1;}}_LL160: {const char*_tmp5AF;void*_tmp5AE[1];struct Cyc_Int_pa_struct
_tmp5AD;return(struct _dyneither_ptr)((_tmp5AD.tag=1,((_tmp5AD.f1=(unsigned long)((
int)_tmp30D),((_tmp5AE[0]=& _tmp5AD,Cyc_aprintf(((_tmp5AF="%d",_tag_dyneither(
_tmp5AF,sizeof(char),3))),_tag_dyneither(_tmp5AE,sizeof(void*),1))))))));}_LL161: {
struct Cyc_Absyn_Float_p_struct*_tmp30E=(struct Cyc_Absyn_Float_p_struct*)_tmp2F5;
if(_tmp30E->tag != 11)goto _LL163;else{_tmp30F=_tmp30E->f1;}}_LL162: return _tmp30F;
_LL163: {struct Cyc_Absyn_Enum_p_struct*_tmp310=(struct Cyc_Absyn_Enum_p_struct*)
_tmp2F5;if(_tmp310->tag != 12)goto _LL165;else{_tmp311=_tmp310->f2;}}_LL164:
_tmp313=_tmp311;goto _LL166;_LL165: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp312=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp2F5;if(_tmp312->tag != 13)goto _LL167;else{
_tmp313=_tmp312->f2;}}_LL166: return Cyc_Absynpp_qvar2string(_tmp313->name);_LL167: {
struct Cyc_Absyn_Exp_p_struct*_tmp314=(struct Cyc_Absyn_Exp_p_struct*)_tmp2F5;if(
_tmp314->tag != 16)goto _LL169;else{_tmp315=_tmp314->f1;}}_LL168: return Cyc_Absynpp_exp2string(
_tmp315);_LL169:;_LL16A:(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL14A:;};}
_LL148: {struct Cyc_Tcpat_Neg_struct*_tmp2F1=(struct Cyc_Tcpat_Neg_struct*)_tmp2ED;
if(_tmp2F1->tag != 1)goto _LL145;else{_tmp2F2=_tmp2F1->f1;}}_LL149: if(((int(*)(
struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2F2)){const char*_tmp5B0;return(
_tmp5B0="_",_tag_dyneither(_tmp5B0,sizeof(char),2));}{struct Cyc_Tcpat_Con_s*
_tmp345=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2F2);
struct Cyc_Absyn_Pat*_tmp346=_tmp345->orig_pat;if(_tmp346 == 0)(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);{void*_tmp347=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp346->topt))->v);enum Cyc_Absyn_Size_of _tmp349;struct Cyc_Absyn_PtrInfo
_tmp34C;struct Cyc_Absyn_PtrAtts _tmp34D;struct Cyc_Absyn_DatatypeInfo _tmp34F;union
Cyc_Absyn_DatatypeInfoU _tmp350;struct Cyc_Absyn_Datatypedecl**_tmp351;struct Cyc_Absyn_Datatypedecl*
_tmp352;struct Cyc_Absyn_AggrInfo _tmp354;union Cyc_Absyn_AggrInfoU _tmp355;struct
Cyc_Absyn_Aggrdecl**_tmp356;struct Cyc_Absyn_Aggrdecl*_tmp357;_LL171: {struct Cyc_Absyn_IntType_struct*
_tmp348=(struct Cyc_Absyn_IntType_struct*)_tmp347;if(_tmp348->tag != 6)goto _LL173;
else{_tmp349=_tmp348->f2;if(_tmp349 != Cyc_Absyn_Char_sz)goto _LL173;}}_LL172:{int
i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp358=Cyc_Tcpat_char_con(r,(char)i,(
struct Cyc_Absyn_Pat*)_tmp346);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2F2,_tmp358)){const char*_tmp5B4;void*_tmp5B3[1];struct
Cyc_Int_pa_struct _tmp5B2;return(struct _dyneither_ptr)((_tmp5B2.tag=1,((_tmp5B2.f1=(
unsigned long)i,((_tmp5B3[0]=& _tmp5B2,Cyc_aprintf(((_tmp5B4="%d",_tag_dyneither(
_tmp5B4,sizeof(char),3))),_tag_dyneither(_tmp5B3,sizeof(void*),1))))))));}}}(int)
_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL173: {struct Cyc_Absyn_IntType_struct*
_tmp34A=(struct Cyc_Absyn_IntType_struct*)_tmp347;if(_tmp34A->tag != 6)goto _LL175;}
_LL174:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp35C=Cyc_Tcpat_int_con(
r,(int)i,_tmp346);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp2F2,_tmp35C)){const char*_tmp5B8;void*_tmp5B7[1];struct Cyc_Int_pa_struct
_tmp5B6;return(struct _dyneither_ptr)((_tmp5B6.tag=1,((_tmp5B6.f1=(unsigned long)((
int)i),((_tmp5B7[0]=& _tmp5B6,Cyc_aprintf(((_tmp5B8="%d",_tag_dyneither(_tmp5B8,
sizeof(char),3))),_tag_dyneither(_tmp5B7,sizeof(void*),1))))))));}}}(int)_throw((
void*)& Cyc_Tcpat_Desc2string_val);_LL175: {struct Cyc_Absyn_PointerType_struct*
_tmp34B=(struct Cyc_Absyn_PointerType_struct*)_tmp347;if(_tmp34B->tag != 5)goto
_LL177;else{_tmp34C=_tmp34B->f1;_tmp34D=_tmp34C.ptr_atts;}}_LL176: {union Cyc_Absyn_Constraint*
_tmp360=_tmp34D.nullable;int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp360);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*
s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F2,Cyc_Tcpat_null_con(r,(
struct Cyc_Absyn_Pat*)_tmp346))){const char*_tmp5B9;return(_tmp5B9="NULL",
_tag_dyneither(_tmp5B9,sizeof(char),5));}}{const char*_tmp5BA;return(_tmp5BA="&_",
_tag_dyneither(_tmp5BA,sizeof(char),3));};}_LL177: {struct Cyc_Absyn_DatatypeType_struct*
_tmp34E=(struct Cyc_Absyn_DatatypeType_struct*)_tmp347;if(_tmp34E->tag != 3)goto
_LL179;else{_tmp34F=_tmp34E->f1;_tmp350=_tmp34F.datatype_info;if((_tmp350.KnownDatatype).tag
!= 2)goto _LL179;_tmp351=(struct Cyc_Absyn_Datatypedecl**)(_tmp350.KnownDatatype).val;
_tmp352=*_tmp351;}}_LL178: if(_tmp352->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp363=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp352->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp363);for(0;(unsigned int)_tmp363;_tmp363=_tmp363->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp363->hd)->name).f2;struct Cyc_List_List*
_tmp364=((struct Cyc_Absyn_Datatypefield*)_tmp363->hd)->typs;struct Cyc_Tcpat_Con_s*
_tmp5BB;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F2,(struct Cyc_Tcpat_Con_s*)((_tmp5BB=_cycalloc(sizeof(*_tmp5BB)),((_tmp5BB->name=
Cyc_Tcpat_Name_v(n),((_tmp5BB->arity=0,((_tmp5BB->span=0,((_tmp5BB->orig_pat=
_tmp346,_tmp5BB)))))))))))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp364)== 0)return n;else{const char*_tmp5BF;void*_tmp5BE[1];struct Cyc_String_pa_struct
_tmp5BD;return(struct _dyneither_ptr)((_tmp5BD.tag=0,((_tmp5BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5BE[0]=& _tmp5BD,Cyc_aprintf(((
_tmp5BF="%s(...)",_tag_dyneither(_tmp5BF,sizeof(char),8))),_tag_dyneither(
_tmp5BE,sizeof(void*),1))))))));}}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};
_LL179:{struct Cyc_Absyn_AggrType_struct*_tmp353=(struct Cyc_Absyn_AggrType_struct*)
_tmp347;if(_tmp353->tag != 12)goto _LL17B;else{_tmp354=_tmp353->f1;_tmp355=_tmp354.aggr_info;
if((_tmp355.KnownAggr).tag != 2)goto _LL17B;_tmp356=(struct Cyc_Absyn_Aggrdecl**)(
_tmp355.KnownAggr).val;_tmp357=*_tmp356;}}if(!(_tmp357->kind == Cyc_Absyn_UnionA))
goto _LL17B;_LL17A: {struct Cyc_List_List*_tmp369=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp357->impl))->fields;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp369);struct _tuple1 _tmp36B;struct _dyneither_ptr*_tmp36C;struct _dyneither_ptr
_tmp36D;struct _tuple1*_tmp36A=_tmp357->name;_tmp36B=*_tmp36A;_tmp36C=_tmp36B.f2;
_tmp36D=*_tmp36C;for(0;(unsigned int)_tmp369;_tmp369=_tmp369->tl){struct
_dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp369->hd)->name;struct Cyc_Tcpat_Con_s*
_tmp5C0;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F2,(struct Cyc_Tcpat_Con_s*)((_tmp5C0=_cycalloc(sizeof(*_tmp5C0)),((_tmp5C0->name=
Cyc_Tcpat_Name_v(n),((_tmp5C0->arity=0,((_tmp5C0->span=0,((_tmp5C0->orig_pat=
_tmp346,_tmp5C0)))))))))))){const char*_tmp5C5;void*_tmp5C4[2];struct Cyc_String_pa_struct
_tmp5C3;struct Cyc_String_pa_struct _tmp5C2;return(struct _dyneither_ptr)((_tmp5C2.tag=
0,((_tmp5C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5C3.tag=0,((
_tmp5C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36D),((_tmp5C4[0]=&
_tmp5C3,((_tmp5C4[1]=& _tmp5C2,Cyc_aprintf(((_tmp5C5="%s{.%s = _}",_tag_dyneither(
_tmp5C5,sizeof(char),12))),_tag_dyneither(_tmp5C4,sizeof(void*),2))))))))))))));}}(
int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL17B:;_LL17C:(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);_LL170:;};};_LL145:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(
struct _RegionHandle*r,void*d){struct _dyneither_ptr*_tmp5C6;return(_tmp5C6=
_cycalloc(sizeof(*_tmp5C6)),((_tmp5C6[0]=Cyc_Tcpat_desc2string(r,d),_tmp5C6)));}
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",
sizeof(char),2);{struct Cyc_List_List*_tmp375=_tmp374;for(0;_tmp375 != 0;_tmp375=((
struct Cyc_List_List*)_check_null(_tmp375))->tl){if(_tmp375->tl != 0){{struct Cyc_List_List*
_tmp5C7;_tmp375->tl=(struct Cyc_List_List*)((_tmp5C7=_cycalloc(sizeof(*_tmp5C7)),((
_tmp5C7->hd=comma,((_tmp5C7->tl=_tmp375->tl,_tmp5C7))))));}_tmp375=_tmp375->tl;}}}
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp374);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp378;_push_handler(& _tmp378);{int _tmp37A=0;if(setjmp(_tmp378.handler))
_tmp37A=1;if(!_tmp37A){{struct _dyneither_ptr _tmp37B=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5CB;void*_tmp5CA[1];struct Cyc_String_pa_struct _tmp5C9;(_tmp5C9.tag=
0,((_tmp5C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37B),((_tmp5CA[0]=&
_tmp5C9,Cyc_Tcutil_terr(loc,((_tmp5CB="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5CB,sizeof(char),53))),_tag_dyneither(_tmp5CA,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp379=(void*)_exn_thrown;void*_tmp380=_tmp379;_LL17E: {
struct Cyc_Tcpat_Desc2string_struct*_tmp381=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp380;if(_tmp381->tag != Cyc_Tcpat_Desc2string)goto _LL180;}_LL17F:{const char*
_tmp5CE;void*_tmp5CD;(_tmp5CD=0,Cyc_Tcutil_terr(loc,((_tmp5CE="patterns may not be exhaustive.",
_tag_dyneither(_tmp5CE,sizeof(char),32))),_tag_dyneither(_tmp5CD,sizeof(void*),0)));}
goto _LL17D;_LL180:;_LL181:(void)_throw(_tmp380);_LL17D:;}};}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple18*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp384=Cyc_Tcenv_get_fnrgn(te);int _tmp385=0;struct _tuple22
_tmp5CF;struct _tuple22 _tmp386=(_tmp5CF.f1=_tmp384,((_tmp5CF.f2=& _tmp385,_tmp5CF)));
struct Cyc_List_List*_tmp387=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _tuple22*,struct Cyc_Absyn_Switch_clause*),struct
_tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp384,Cyc_Tcpat_get_match,&
_tmp386,swcs);void*_tmp388=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp384,_tmp387);int _tmp389=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple18*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp384,_tmp388,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp389);for(0;_tmp387 != 0;_tmp387=_tmp387->tl){
struct _tuple21 _tmp38B;struct _tuple18*_tmp38C;struct _tuple18 _tmp38D;int _tmp38E;
struct Cyc_Position_Segment*_tmp38F;struct _tuple21*_tmp38A=(struct _tuple21*)
_tmp387->hd;_tmp38B=*_tmp38A;_tmp38C=_tmp38B.f2;_tmp38D=*_tmp38C;_tmp38E=_tmp38D.f1;
_tmp38F=_tmp38D.f2;if(!_tmp38E){{const char*_tmp5D2;void*_tmp5D1;(_tmp5D1=0,Cyc_Tcutil_terr(
_tmp38F,((_tmp5D2="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5D2,sizeof(char),74))),_tag_dyneither(_tmp5D1,sizeof(void*),0)));}
break;}}}struct _tuple23{struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _RegionHandle*r,struct _tuple23*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp393;_push_handler(& _tmp393);{int _tmp395=0;if(setjmp(_tmp393.handler))
_tmp395=1;if(!_tmp395){{struct _dyneither_ptr _tmp396=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5D6;void*_tmp5D5[1];struct Cyc_String_pa_struct _tmp5D4;(_tmp5D4.tag=
0,((_tmp5D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp396),((_tmp5D5[0]=&
_tmp5D4,Cyc_Tcutil_warn((*pr).f1,((_tmp5D6="pattern not exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5D6,sizeof(char),45))),_tag_dyneither(_tmp5D5,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp394=(void*)_exn_thrown;void*_tmp39B=_tmp394;_LL183: {
struct Cyc_Tcpat_Desc2string_struct*_tmp39C=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp39B;if(_tmp39C->tag != Cyc_Tcpat_Desc2string)goto _LL185;}_LL184:{const char*
_tmp5D9;void*_tmp5D8;(_tmp5D8=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5D9="pattern not exhaustive.",
_tag_dyneither(_tmp5D9,sizeof(char),24))),_tag_dyneither(_tmp5D8,sizeof(void*),0)));}
goto _LL182;_LL185:;_LL186:(void)_throw(_tmp39B);_LL182:;}};};}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple24{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle*_tmp39F=Cyc_Tcenv_get_fnrgn(te);struct _tuple24*_tmp5DC;
struct Cyc_List_List*_tmp5DB;struct Cyc_List_List*_tmp3A0=(_tmp5DB=_region_malloc(
_tmp39F,sizeof(*_tmp5DB)),((_tmp5DB->hd=((_tmp5DC=_region_malloc(_tmp39F,sizeof(*
_tmp5DC)),((_tmp5DC->f1=Cyc_Tcpat_compile_pat(_tmp39F,p),((_tmp5DC->f2=0,_tmp5DC)))))),((
_tmp5DB->tl=0,_tmp5DB)))));void*_tmp3A1=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp39F,_tmp3A0);struct
_tuple23 _tmp5DD;struct _tuple23 _tmp3A2=(_tmp5DD.f1=loc,((_tmp5DD.f2=1,_tmp5DD)));
int _tmp3A3=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct
_RegionHandle*,struct _tuple23*,void*),struct _tuple23*env1,void(*rhs_appears)(int,
int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp39F,_tmp3A1,
Cyc_Tcpat_not_exhaust_warn,& _tmp3A2,Cyc_Tcpat_dummy_fn,0,& _tmp3A3);return _tmp3A2.f2;}
static struct _tuple21*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple18*_tmp5DE;
struct _tuple18*rhs=(_tmp5DE=_region_malloc(r,sizeof(*_tmp5DE)),((_tmp5DE->f1=0,((
_tmp5DE->f2=(swc->pattern)->loc,_tmp5DE)))));struct _tuple21*_tmp5DF;return(
_tmp5DF=_region_malloc(r,sizeof(*_tmp5DF)),((_tmp5DF->f1=sp0,((_tmp5DF->f2=rhs,
_tmp5DF)))));}static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){struct _RegionHandle*
_tmp3A9=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp3AA=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple21*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3A9,Cyc_Tcpat_get_match2,
_tmp3A9,swcs);void*_tmp3AB=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp3A9,_tmp3AA);int _tmp3AC=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple18*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3A9,_tmp3AB,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp3AC);for(0;_tmp3AA != 0;_tmp3AA=_tmp3AA->tl){
struct _tuple21 _tmp3AE;struct _tuple18*_tmp3AF;struct _tuple18 _tmp3B0;int _tmp3B1;
struct Cyc_Position_Segment*_tmp3B2;struct _tuple21*_tmp3AD=(struct _tuple21*)
_tmp3AA->hd;_tmp3AE=*_tmp3AD;_tmp3AF=_tmp3AE.f2;_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3B0.f1;
_tmp3B2=_tmp3B0.f2;if(!_tmp3B1){{const char*_tmp5E2;void*_tmp5E1;(_tmp5E1=0,Cyc_Tcutil_terr(
_tmp3B2,((_tmp5E2="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5E2,sizeof(char),74))),_tag_dyneither(_tmp5E1,sizeof(void*),0)));}
break;}}}
