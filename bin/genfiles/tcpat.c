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
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
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
k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Absyn_Kind
Cyc_Tcutil_tmk;extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt
Cyc_Tcutil_ako;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;extern struct Cyc_Core_Opt
Cyc_Tcutil_trko;int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple10{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct _tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
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
void*)& Cyc_Absyn_Wild_p_val;{const char*_tmp3B9;void*_tmp3B8;(_tmp3B8=0,Cyc_Tcutil_terr(
p->loc,((_tmp3B9="struct tag used without arguments in pattern",_tag_dyneither(
_tmp3B9,sizeof(char),45))),_tag_dyneither(_tmp3B8,sizeof(void*),0)));}
_npop_handler(0);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp20=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp1E;if(_tmp20->tag != 2)goto _LL12;else{
_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL11:{struct Cyc_Absyn_Datatype_p_struct
_tmp3BC;struct Cyc_Absyn_Datatype_p_struct*_tmp3BB;p->r=(void*)((_tmp3BB=
_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[0]=((_tmp3BC.tag=7,((_tmp3BC.f1=_tmp21,((
_tmp3BC.f2=_tmp22,((_tmp3BC.f3=0,((_tmp3BC.f4=0,_tmp3BC)))))))))),_tmp3BB))));}
_npop_handler(0);return;_LL12: {struct Cyc_Tcenv_EnumRes_struct*_tmp23=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp1E;if(_tmp23->tag != 3)goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}
_LL13:{struct Cyc_Absyn_Enum_p_struct _tmp3BF;struct Cyc_Absyn_Enum_p_struct*
_tmp3BE;p->r=(void*)((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=((_tmp3BF.tag=
12,((_tmp3BF.f1=_tmp24,((_tmp3BF.f2=_tmp25,_tmp3BF)))))),_tmp3BE))));}
_npop_handler(0);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp26=(
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp1E;if(_tmp26->tag != 4)goto _LL16;else{
_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}_LL15:{struct Cyc_Absyn_AnonEnum_p_struct
_tmp3C2;struct Cyc_Absyn_AnonEnum_p_struct*_tmp3C1;p->r=(void*)((_tmp3C1=
_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=((_tmp3C2.tag=13,((_tmp3C2.f1=(void*)
_tmp27,((_tmp3C2.f2=_tmp28,_tmp3C2)))))),_tmp3C1))));}_npop_handler(0);return;
_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp29=(struct Cyc_Tcenv_VarRes_struct*)
_tmp1E;if(_tmp29->tag != 0)goto _LLD;}_LL17: goto _LLD;_LLD:;};_pop_handler();}else{
void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {struct Cyc_Dict_Absent_struct*
_tmp34=(struct Cyc_Dict_Absent_struct*)_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)
goto _LL1B;}_LL1A: goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{union
Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;_LL1E:
if((_tmp35.Loc_n).tag != 4)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F: goto
_LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(
_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:(*_tmp2).f1=Cyc_Absyn_Loc_n;{
struct Cyc_Absyn_Var_p_struct _tmp3C5;struct Cyc_Absyn_Var_p_struct*_tmp3C4;p->r=(
void*)((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4[0]=((_tmp3C5.tag=1,((
_tmp3C5.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((
_tmp3C5.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3C5)))))),
_tmp3C4))));}return;_LL22:;_LL23:{const char*_tmp3C8;void*_tmp3C7;(_tmp3C7=0,Cyc_Tcutil_terr(
p->loc,((_tmp3C8="qualified variable in pattern",_tag_dyneither(_tmp3C8,sizeof(
char),30))),_tag_dyneither(_tmp3C7,sizeof(void*),0)));}return;_LL1D:;};_LL3: {
struct Cyc_Absyn_UnknownCall_p_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=
_tmp3->f3;}}_LL4:{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;
if(setjmp(_tmp3C.handler))_tmp3E=1;if(!_tmp3E){{struct _RegionHandle*_tmp3F=Cyc_Tcenv_get_fnrgn(
te);void*_tmp40=Cyc_Tcenv_lookup_ordinary(_tmp3F,te,p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*
_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;
_LL25: {struct Cyc_Tcenv_AggrRes_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {struct Cyc_List_List*
_tmp49=0;for(0;_tmp5 != 0;_tmp5=_tmp5->tl){struct _tuple14*_tmp3CB;struct Cyc_List_List*
_tmp3CA;_tmp49=((_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->hd=((_tmp3CB=
_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB->f1=0,((_tmp3CB->f2=(struct Cyc_Absyn_Pat*)
_tmp5->hd,_tmp3CB)))))),((_tmp3CA->tl=_tmp49,_tmp3CA))))));}{struct Cyc_Absyn_Aggr_p_struct
_tmp3D5;struct Cyc_Absyn_Aggrdecl**_tmp3D4;struct Cyc_Absyn_AggrInfo*_tmp3D3;
struct Cyc_Absyn_Aggr_p_struct*_tmp3D2;p->r=(void*)((_tmp3D2=_cycalloc(sizeof(*
_tmp3D2)),((_tmp3D2[0]=((_tmp3D5.tag=6,((_tmp3D5.f1=((_tmp3D3=_cycalloc(sizeof(*
_tmp3D3)),((_tmp3D3->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3D4=_cycalloc(sizeof(*
_tmp3D4)),((_tmp3D4[0]=_tmp42,_tmp3D4))))),((_tmp3D3->targs=0,_tmp3D3)))))),((
_tmp3D5.f2=0,((_tmp3D5.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp49),((_tmp3D5.f4=_tmp6,_tmp3D5)))))))))),_tmp3D2))));}_npop_handler(0);
return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp43=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp40;if(_tmp43->tag != 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}
_LL28:{struct Cyc_Absyn_Datatype_p_struct _tmp3D8;struct Cyc_Absyn_Datatype_p_struct*
_tmp3D7;p->r=(void*)((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7[0]=((_tmp3D8.tag=
7,((_tmp3D8.f1=_tmp44,((_tmp3D8.f2=_tmp45,((_tmp3D8.f3=_tmp5,((_tmp3D8.f4=_tmp6,
_tmp3D8)))))))))),_tmp3D7))));}_npop_handler(0);return;_LL29: {struct Cyc_Tcenv_EnumRes_struct*
_tmp46=(struct Cyc_Tcenv_EnumRes_struct*)_tmp40;if(_tmp46->tag != 3)goto _LL2B;}
_LL2A: goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmp47=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp40;if(_tmp47->tag != 4)goto _LL2D;}_LL2C:{const char*_tmp3DB;void*_tmp3DA;(
_tmp3DA=0,Cyc_Tcutil_terr(p->loc,((_tmp3DB="enum tag used with arguments in pattern",
_tag_dyneither(_tmp3DB,sizeof(char),40))),_tag_dyneither(_tmp3DA,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;_npop_handler(0);return;_LL2D: {struct Cyc_Tcenv_VarRes_struct*
_tmp48=(struct Cyc_Tcenv_VarRes_struct*)_tmp40;if(_tmp48->tag != 0)goto _LL24;}
_LL2E: goto _LL24;_LL24:;};_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;
void*_tmp55=_tmp3D;_LL30: {struct Cyc_Dict_Absent_struct*_tmp56=(struct Cyc_Dict_Absent_struct*)
_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31: goto _LL2F;_LL32:;_LL33:(
void)_throw(_tmp55);_LL2F:;}};}{const char*_tmp3DF;void*_tmp3DE[1];struct Cyc_String_pa_struct
_tmp3DD;(_tmp3DD.tag=0,((_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3DE[0]=& _tmp3DD,Cyc_Tcutil_terr(p->loc,((
_tmp3DF="%s is not a constructor in pattern",_tag_dyneither(_tmp3DF,sizeof(char),
35))),_tag_dyneither(_tmp3DE,sizeof(void*),1)))))));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmp7->tag != 6)goto _LL7;else{_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;
_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=_tmp7->f4;}}_LL6: if(topt == 0){const char*
_tmp3E2;void*_tmp3E1;(_tmp3E1=0,Cyc_Tcutil_terr(p->loc,((_tmp3E2="cannot determine pattern type",
_tag_dyneither(_tmp3E2,sizeof(char),30))),_tag_dyneither(_tmp3E1,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));{void*_tmp5C=t;struct
Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct Cyc_Absyn_AggrType_struct*_tmp5D=(struct
Cyc_Absyn_AggrType_struct*)_tmp5C;if(_tmp5D->tag != 12)goto _LL37;else{_tmp5E=
_tmp5D->f1;}}_LL36:{struct Cyc_Absyn_Aggr_p_struct _tmp3E8;struct Cyc_Absyn_AggrInfo*
_tmp3E7;struct Cyc_Absyn_Aggr_p_struct*_tmp3E6;p->r=(void*)((_tmp3E6=_cycalloc(
sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E8.tag=6,((_tmp3E8.f1=((_tmp3E7=_cycalloc(
sizeof(*_tmp3E7)),((_tmp3E7[0]=_tmp5E,_tmp3E7)))),((_tmp3E8.f2=_tmp9,((_tmp3E8.f3=
_tmpA,((_tmp3E8.f4=_tmpB,_tmp3E8)))))))))),_tmp3E6))));}Cyc_Tcpat_resolve_pat(te,
topt,p);goto _LL34;_LL37:;_LL38:{const char*_tmp3EC;void*_tmp3EB[1];struct Cyc_String_pa_struct
_tmp3EA;(_tmp3EA.tag=0,((_tmp3EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3EB[0]=& _tmp3EA,Cyc_Tcutil_terr(p->loc,((
_tmp3EC="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3EC,
sizeof(char),45))),_tag_dyneither(_tmp3EB,sizeof(void*),1)))))));}goto _LL34;
_LL34:;}return;};_LL7: {struct Cyc_Absyn_Aggr_p_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;
_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;
_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;
_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;
_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:{struct _handler_cons _tmp65;
_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))_tmp67=1;if(!
_tmp67){{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmp12);struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;if(_tmp69->impl == 0){{const char*
_tmp3F4;void*_tmp3F3[1];const char*_tmp3F2;const char*_tmp3F1;struct Cyc_String_pa_struct
_tmp3F0;(_tmp3F0.tag=0,((_tmp3F0.f1=(struct _dyneither_ptr)(_tmp69->kind == Cyc_Absyn_StructA?(
_tmp3F1="struct",_tag_dyneither(_tmp3F1,sizeof(char),7)):((_tmp3F2="union",
_tag_dyneither(_tmp3F2,sizeof(char),6)))),((_tmp3F3[0]=& _tmp3F0,Cyc_Tcutil_terr(
p->loc,((_tmp3F4="can't destructure an abstract %s",_tag_dyneither(_tmp3F4,
sizeof(char),33))),_tag_dyneither(_tmp3F3,sizeof(void*),1)))))));}p->r=(void*)&
Cyc_Absyn_Wild_p_val;_npop_handler(0);return;}{int more_exists=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars)
- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp15);if(more_exists < 0){{
const char*_tmp3F7;void*_tmp3F6;(_tmp3F6=0,Cyc_Tcutil_terr(p->loc,((_tmp3F7="too many existentially bound type variables in pattern",
_tag_dyneither(_tmp3F7,sizeof(char),55))),_tag_dyneither(_tmp3F6,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp71=_tmp15;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars);for(0;n != 
0;-- n){_tmp71=&((struct Cyc_List_List*)_check_null(*_tmp71))->tl;}}*_tmp71=0;};}
else{if(more_exists > 0){struct Cyc_List_List*_tmp72=0;for(0;more_exists != 0;--
more_exists){struct Cyc_Absyn_Unknown_kb_struct*_tmp3FD;struct Cyc_Absyn_Unknown_kb_struct
_tmp3FC;struct Cyc_List_List*_tmp3FB;_tmp72=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((
_tmp3FB->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((
_tmp3FD[0]=((_tmp3FC.tag=1,((_tmp3FC.f1=0,_tmp3FC)))),_tmp3FD))))),((_tmp3FB->tl=
_tmp72,_tmp3FB))))));}*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp15,_tmp72);}}{struct Cyc_Absyn_Aggr_p_struct
_tmp403;struct Cyc_Absyn_AggrInfo*_tmp402;struct Cyc_Absyn_Aggr_p_struct*_tmp401;p->r=(
void*)((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401[0]=((_tmp403.tag=6,((
_tmp403.f1=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402->aggr_info=Cyc_Absyn_KnownAggr(
_tmp68),((_tmp402->targs=_tmp13,_tmp402)))))),((_tmp403.f2=*_tmp15,((_tmp403.f3=
_tmp16,((_tmp403.f4=_tmp17,_tmp403)))))))))),_tmp401))));};};};_pop_handler();}
else{void*_tmp66=(void*)_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {struct Cyc_Dict_Absent_struct*
_tmp7B=(struct Cyc_Dict_Absent_struct*)_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)
goto _LL3C;}_LL3B:{const char*_tmp406;void*_tmp405;(_tmp405=0,Cyc_Tcutil_terr(p->loc,((
_tmp406="Non-aggregate name has designator patterns",_tag_dyneither(_tmp406,
sizeof(char),43))),_tag_dyneither(_tmp405,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}return;_LL9: {struct Cyc_Absyn_Exp_p_struct*
_tmp18=(struct Cyc_Absyn_Exp_p_struct*)_tmp0;if(_tmp18->tag != 16)goto _LLB;else{
_tmp19=_tmp18->f1;}}_LLA: Cyc_Tcexp_tcExp(te,0,_tmp19);if(!Cyc_Tcutil_is_const_exp(
te,_tmp19)){{const char*_tmp409;void*_tmp408;(_tmp408=0,Cyc_Tcutil_terr(p->loc,((
_tmp409="non-constant expression in case pattern",_tag_dyneither(_tmp409,sizeof(
char),40))),_tag_dyneither(_tmp408,sizeof(void*),0)));}p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
unsigned int _tmp81;int _tmp82;struct _tuple13 _tmp80=Cyc_Evexp_eval_const_uint_exp(
_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;{struct Cyc_Absyn_Int_p_struct _tmp40C;
struct Cyc_Absyn_Int_p_struct*_tmp40B;p->r=(void*)((_tmp40B=_cycalloc_atomic(
sizeof(*_tmp40B)),((_tmp40B[0]=((_tmp40C.tag=9,((_tmp40C.f1=Cyc_Absyn_None,((
_tmp40C.f2=(int)_tmp81,_tmp40C)))))),_tmp40B))));}return;};_LLB:;_LLC: return;
_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**
topt){if(topt != 0)return*topt;{struct Cyc_Core_Opt*_tmp40D;return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((
_tmp40D->v=s,_tmp40D)))));};}static void*Cyc_Tcpat_num_type(void**topt,void*numt){
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp86=Cyc_Tcutil_compress(
numt);_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmp87=(struct Cyc_Absyn_EnumType_struct*)
_tmp86;if(_tmp87->tag != 14)goto _LL41;}_LL40: if(topt != 0)return*topt;goto _LL3E;
_LL41:;_LL42: goto _LL3E;_LL3E:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_List_List**v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(
0);{struct Cyc_List_List*_tmp40E;*v_result_ptr=(struct Cyc_List_List*)((_tmp40E=
_cycalloc(sizeof(*_tmp40E)),((_tmp40E->hd=vd,((_tmp40E->tl=*v_result_ptr,_tmp40E))))));};}
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult
res1,struct Cyc_Tcpat_TcPatResult res2){struct _tuple12*_tmp8A;struct Cyc_List_List*
_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;
_tmp8B=_tmp89.patvars;{struct _tuple12*_tmp8D;struct Cyc_List_List*_tmp8E;struct
Cyc_Tcpat_TcPatResult _tmp8C=res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=
_tmp8C.patvars;if(_tmp8A != 0  || _tmp8D != 0){if(_tmp8A == 0){struct _tuple12*
_tmp40F;_tmp8A=((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F->f1=0,((_tmp40F->f2=
0,_tmp40F))))));}if(_tmp8D == 0){struct _tuple12*_tmp410;_tmp8D=((_tmp410=
_cycalloc(sizeof(*_tmp410)),((_tmp410->f1=0,((_tmp410->f2=0,_tmp410))))));}{
struct _tuple12*_tmp413;struct Cyc_Tcpat_TcPatResult _tmp412;return(_tmp412.tvars_and_bounds_opt=((
_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f1,(*
_tmp8D).f1),((_tmp413->f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f2,(*_tmp8D).f2),_tmp413)))))),((
_tmp412.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp412)));};}{struct Cyc_Tcpat_TcPatResult
_tmp414;return(_tmp414.tvars_and_bounds_opt=0,((_tmp414.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E),
_tmp414)));};};}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Pat*_tmp415;return(_tmp415=_cycalloc(sizeof(*_tmp415)),((
_tmp415->loc=loc,((_tmp415->topt=0,((_tmp415->r=(void*)& Cyc_Absyn_Wild_p_val,
_tmp415)))))));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple16{
struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**
rgn_opt){Cyc_Tcpat_resolve_pat(te,topt,p);{void*t;struct Cyc_Tcpat_TcPatResult
_tmp416;struct Cyc_Tcpat_TcPatResult res=(_tmp416.tvars_and_bounds_opt=0,((_tmp416.patvars=
0,_tmp416)));{void*_tmp95=p->r;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Pat*
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
topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp99->topt))->v;if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){const char*_tmp419;
void*_tmp418;(_tmp418=0,Cyc_Tcutil_terr(p->loc,((_tmp419="pattern would point to an abstract member",
_tag_dyneither(_tmp419,sizeof(char),42))),_tag_dyneither(_tmp418,sizeof(void*),0)));}
Cyc_Tcpat_set_vd(_tmp98,& res.patvars,t);goto _LL43;_LL48: {struct Cyc_Absyn_Reference_p_struct*
_tmp9A=(struct Cyc_Absyn_Reference_p_struct*)_tmp95;if(_tmp9A->tag != 2)goto _LL4A;
else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL49: res=Cyc_Tcpat_tcPatRec(te,
_tmp9C,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->topt))->v;
if(rgn_opt == 0){{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_Tcutil_terr(p->loc,((
_tmp41C="* pattern would point into an unknown/unallowed region",_tag_dyneither(
_tmp41C,sizeof(char),55))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}goto _LL43;}
else{if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp41F;void*_tmp41E;(
_tmp41E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp41F="* pattern cannot take the address of an alias-free path",_tag_dyneither(
_tmp41F,sizeof(char),56))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}}{struct
Cyc_Absyn_PointerType_struct _tmp429;struct Cyc_Absyn_PtrAtts _tmp428;struct Cyc_Absyn_PtrInfo
_tmp427;struct Cyc_Absyn_PointerType_struct*_tmp426;Cyc_Tcpat_set_vd(_tmp9B,& res.patvars,(
void*)((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=((_tmp429.tag=5,((
_tmp429.f1=((_tmp427.elt_typ=t,((_tmp427.elt_tq=Cyc_Absyn_empty_tqual(0),((
_tmp427.ptr_atts=((_tmp428.rgn=*rgn_opt,((_tmp428.nullable=Cyc_Absyn_false_conref,((
_tmp428.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp428.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp428.ptrloc=0,_tmp428)))))))))),_tmp427)))))),_tmp429)))),_tmp426)))));}goto
_LL43;_LL4A: {struct Cyc_Absyn_TagInt_p_struct*_tmp9D=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp95;if(_tmp9D->tag != 3)goto _LL4C;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}
_LL4B: Cyc_Tcpat_set_vd(_tmp9F,& res.patvars,_tmp9F->type);{struct _RegionHandle*
_tmpD9=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Absyn_Tvar*_tmp42A[1];Cyc_Tcenv_add_type_vars(
_tmpD9,p->loc,te,((_tmp42A[0]=_tmp9E,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42A,sizeof(struct Cyc_Absyn_Tvar*),
1)))));}if(res.tvars_and_bounds_opt == 0){struct _tuple12*_tmp42B;res.tvars_and_bounds_opt=((
_tmp42B=_cycalloc(sizeof(*_tmp42B)),((_tmp42B->f1=0,((_tmp42B->f2=0,_tmp42B))))));}{
struct Cyc_List_List*_tmp42C;(*res.tvars_and_bounds_opt).f1=((_tmp42C=_cycalloc(
sizeof(*_tmp42C)),((_tmp42C->hd=_tmp9E,((_tmp42C->tl=(*res.tvars_and_bounds_opt).f1,
_tmp42C))))));}t=Cyc_Absyn_uint_typ;goto _LL43;};_LL4C: {struct Cyc_Absyn_Int_p_struct*
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
else{_tmpA9=_tmpA8->f1;}}_LL57:{struct Cyc_Absyn_EnumType_struct _tmp42F;struct Cyc_Absyn_EnumType_struct*
_tmp42E;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((
_tmp42E[0]=((_tmp42F.tag=14,((_tmp42F.f1=_tmpA9->name,((_tmp42F.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpA9,_tmp42F)))))),_tmp42E)))));}goto _LL43;_LL58: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmpAA=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp95;if(_tmpAA->tag != 13)goto _LL5A;
else{_tmpAB=(void*)_tmpAA->f1;}}_LL59: t=Cyc_Tcpat_num_type(topt,_tmpAB);goto
_LL43;_LL5A: {struct Cyc_Absyn_Null_p_struct*_tmpAC=(struct Cyc_Absyn_Null_p_struct*)
_tmp95;if(_tmpAC->tag != 8)goto _LL5C;}_LL5B: if(topt != 0){void*_tmpDF=Cyc_Tcutil_compress(*
topt);_LL6F: {struct Cyc_Absyn_PointerType_struct*_tmpE0=(struct Cyc_Absyn_PointerType_struct*)
_tmpDF;if(_tmpE0->tag != 5)goto _LL71;}_LL70: t=*topt;goto tcpat_end;_LL71:;_LL72:
goto _LL6E;_LL6E:;}{struct Cyc_Core_Opt*_tmpE1=Cyc_Tcenv_lookup_opt_type_vars(te);{
struct Cyc_Absyn_PointerType_struct _tmp439;struct Cyc_Absyn_PtrAtts _tmp438;struct
Cyc_Absyn_PtrInfo _tmp437;struct Cyc_Absyn_PointerType_struct*_tmp436;t=(void*)((
_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436[0]=((_tmp439.tag=5,((_tmp439.f1=((
_tmp437.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmpE1),((
_tmp437.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp437.ptr_atts=((_tmp438.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmpE1),((_tmp438.nullable=Cyc_Absyn_true_conref,((
_tmp438.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp438.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp438.ptrloc=0,_tmp438)))))))))),_tmp437)))))),_tmp439)))),_tmp436))));}goto
_LL43;};_LL5C: {struct Cyc_Absyn_Pointer_p_struct*_tmpAD=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp95;if(_tmpAD->tag != 5)goto _LL5E;else{_tmpAE=_tmpAD->f1;}}_LL5D: {void*
inner_typ=(void*)& Cyc_Absyn_VoidType_val;void**_tmpE6=0;int elt_const=0;if(topt != 
0){void*_tmpE7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmpE9;void*
_tmpEA;struct Cyc_Absyn_Tqual _tmpEB;_LL74: {struct Cyc_Absyn_PointerType_struct*
_tmpE8=(struct Cyc_Absyn_PointerType_struct*)_tmpE7;if(_tmpE8->tag != 5)goto _LL76;
else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE9.elt_typ;_tmpEB=_tmpE9.elt_tq;}}_LL75:
inner_typ=_tmpEA;_tmpE6=(void**)& inner_typ;elt_const=_tmpEB.real_const;goto _LL73;
_LL76:;_LL77: goto _LL73;_LL73:;}{void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmpAE,_tmpE6,(void**)& ptr_rgn));{void*_tmpEC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v);struct Cyc_Absyn_DatatypeFieldInfo
_tmpEE;union Cyc_Absyn_DatatypeFieldInfoU _tmpEF;struct _tuple2 _tmpF0;struct Cyc_Absyn_Datatypedecl*
_tmpF1;struct Cyc_Absyn_Datatypefield*_tmpF2;struct Cyc_List_List*_tmpF3;_LL79: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmpED=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpEC;if(_tmpED->tag != 4)goto _LL7B;else{_tmpEE=_tmpED->f1;_tmpEF=_tmpEE.field_info;
if((_tmpEF.KnownDatatypefield).tag != 2)goto _LL7B;_tmpF0=(struct _tuple2)(_tmpEF.KnownDatatypefield).val;
_tmpF1=_tmpF0.f1;_tmpF2=_tmpF0.f2;_tmpF3=_tmpEE.targs;}}_LL7A:{void*_tmpF4=Cyc_Tcutil_compress(
inner_typ);_LL7E: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpF5=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF4;if(_tmpF5->tag != 4)goto _LL80;}_LL7F: goto DONT_PROMOTE;_LL80:;_LL81: goto
_LL7D;_LL7D:;}{struct Cyc_Absyn_DatatypeType_struct _tmp443;struct Cyc_Absyn_Datatypedecl**
_tmp442;struct Cyc_Absyn_DatatypeInfo _tmp441;struct Cyc_Absyn_DatatypeType_struct*
_tmp440;struct Cyc_Absyn_DatatypeType_struct*_tmpF6=(_tmp440=_cycalloc(sizeof(*
_tmp440)),((_tmp440[0]=((_tmp443.tag=3,((_tmp443.f1=((_tmp441.datatype_info=Cyc_Absyn_KnownDatatype(((
_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=_tmpF1,_tmp442))))),((_tmp441.targs=
_tmpF3,_tmp441)))),_tmp443)))),_tmp440)));((struct Cyc_Core_Opt*)_check_null(
_tmpAE->topt))->v=(void*)((void*)_tmpF6);{struct Cyc_Absyn_PointerType_struct
_tmp44D;struct Cyc_Absyn_PtrAtts _tmp44C;struct Cyc_Absyn_PtrInfo _tmp44B;struct Cyc_Absyn_PointerType_struct*
_tmp44A;t=(void*)((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=((_tmp44D.tag=
5,((_tmp44D.f1=((_tmp44B.elt_typ=(void*)_tmpF6,((_tmp44B.elt_tq=elt_const?Cyc_Absyn_const_tqual(
0): Cyc_Absyn_empty_tqual(0),((_tmp44B.ptr_atts=((_tmp44C.rgn=ptr_rgn,((_tmp44C.nullable=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp44C.bounds=Cyc_Absyn_bounds_one_conref,((
_tmp44C.zero_term=Cyc_Absyn_false_conref,((_tmp44C.ptrloc=0,_tmp44C)))))))))),
_tmp44B)))))),_tmp44D)))),_tmp44A))));}goto _LL78;};_LL7B:;_LL7C: DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_struct _tmp457;struct Cyc_Absyn_PtrAtts _tmp456;struct
Cyc_Absyn_PtrInfo _tmp455;struct Cyc_Absyn_PointerType_struct*_tmp454;t=(void*)((
_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454[0]=((_tmp457.tag=5,((_tmp457.f1=((
_tmp455.elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v,((
_tmp455.elt_tq=elt_const?Cyc_Absyn_const_tqual(0): Cyc_Absyn_empty_tqual(0),((
_tmp455.ptr_atts=((_tmp456.rgn=ptr_rgn,((_tmp456.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)(),((_tmp456.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp456.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp456.ptrloc=0,_tmp456)))))))))),_tmp455)))))),_tmp457)))),_tmp454))));}_LL78:;}
goto _LL43;};}_LL5E: {struct Cyc_Absyn_Tuple_p_struct*_tmpAF=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp95;if(_tmpAF->tag != 4)goto _LL60;else{_tmpB0=_tmpAF->f1;_tmpB1=(struct Cyc_List_List**)&
_tmpAF->f1;_tmpB2=_tmpAF->f2;}}_LL5F: {struct Cyc_List_List*_tmp103=*_tmpB1;
struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*
_tmp104=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp106;_LL83: {struct Cyc_Absyn_TupleType_struct*
_tmp105=(struct Cyc_Absyn_TupleType_struct*)_tmp104;if(_tmp105->tag != 11)goto
_LL85;else{_tmp106=_tmp105->f1;}}_LL84: topt_ts=_tmp106;if(_tmpB2){int _tmp107=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp103);int _tmp108=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp106);if(_tmp107 < _tmp108){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp108 - _tmp107;++ i){struct Cyc_List_List*_tmp458;
wild_ps=((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp458->tl=wild_ps,_tmp458))))));}}*_tmpB1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp103,wild_ps);_tmp103=*_tmpB1;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp103)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp106)){const char*_tmp45B;void*_tmp45A;(
_tmp45A=0,Cyc_Tcutil_warn(p->loc,((_tmp45B="unnecessary ... in tuple pattern",
_tag_dyneither(_tmp45B,sizeof(char),33))),_tag_dyneither(_tmp45A,sizeof(void*),0)));}}}
goto _LL82;_LL85:;_LL86: goto _LL82;_LL82:;}else{if(_tmpB2){const char*_tmp45E;void*
_tmp45D;(_tmp45D=0,Cyc_Tcutil_terr(p->loc,((_tmp45E="cannot determine missing fields for ... in tuple pattern",
_tag_dyneither(_tmp45E,sizeof(char),57))),_tag_dyneither(_tmp45D,sizeof(void*),0)));}}
for(0;_tmp103 != 0;_tmp103=_tmp103->tl){void**_tmp10E=0;if(topt_ts != 0){_tmp10E=(
void**)&(*((struct _tuple15*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp103->hd,_tmp10E,rgn_opt));{
struct _tuple15*_tmp461;struct Cyc_List_List*_tmp460;pat_ts=((_tmp460=_cycalloc(
sizeof(*_tmp460)),((_tmp460->hd=((_tmp461=_cycalloc(sizeof(*_tmp461)),((_tmp461->f1=
Cyc_Absyn_empty_tqual(0),((_tmp461->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmp103->hd)->topt))->v,_tmp461)))))),((_tmp460->tl=pat_ts,
_tmp460))))));};}{struct Cyc_Absyn_TupleType_struct _tmp464;struct Cyc_Absyn_TupleType_struct*
_tmp463;t=(void*)((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463[0]=((_tmp464.tag=
11,((_tmp464.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts),_tmp464)))),_tmp463))));}goto _LL43;}_LL60: {struct Cyc_Absyn_Aggr_p_struct*
_tmpB3=(struct Cyc_Absyn_Aggr_p_struct*)_tmp95;if(_tmpB3->tag != 6)goto _LL62;else{
_tmpB4=_tmpB3->f1;if(_tmpB4 == 0)goto _LL62;_tmpB5=*_tmpB4;_tmpB6=_tmpB5.aggr_info;
if((_tmpB6.KnownAggr).tag != 2)goto _LL62;_tmpB7=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB6.KnownAggr).val;_tmpB8=*_tmpB7;_tmpB9=_tmpB5.targs;_tmpBA=(struct Cyc_List_List**)&(*
_tmpB3->f1).targs;_tmpBB=_tmpB3->f2;_tmpBC=_tmpB3->f3;_tmpBD=(struct Cyc_List_List**)&
_tmpB3->f3;_tmpBE=_tmpB3->f4;}}_LL61: {struct Cyc_List_List*_tmp113=*_tmpBD;const
char*_tmp466;const char*_tmp465;struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?(
_tmp466="struct",_tag_dyneither(_tmp466,sizeof(char),7)):((_tmp465="union",
_tag_dyneither(_tmp465,sizeof(char),6)));if(_tmpB8->impl == 0){{const char*_tmp46A;
void*_tmp469[1];struct Cyc_String_pa_struct _tmp468;(_tmp468.tag=0,((_tmp468.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp469[0]=& _tmp468,Cyc_Tcutil_terr(
p->loc,((_tmp46A="can't destructure an abstract %s",_tag_dyneither(_tmp46A,
sizeof(char),33))),_tag_dyneither(_tmp469,sizeof(void*),1)))))));}t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL43;}if(_tmpBB != 0  || _tmpB8->kind == 
Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->tagged)
rgn_opt=0;{struct _RegionHandle _tmp117=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp117;_push_region(rgn);{struct Cyc_List_List*_tmp118=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmp119=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->exist_vars;{struct Cyc_List_List*t=_tmpBB;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp119))->hd;
_tmp119=_tmp119->tl;{void*_tmp11A=Cyc_Absyn_compress_kb(tv->kind);void*_tmp11B=
Cyc_Absyn_compress_kb(uv->kind);int error=0;struct Cyc_Absyn_Kind*k2;{void*_tmp11C=
_tmp11B;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Absyn_Kind*_tmp120;_LL88: {struct
Cyc_Absyn_Less_kb_struct*_tmp11D=(struct Cyc_Absyn_Less_kb_struct*)_tmp11C;if(
_tmp11D->tag != 2)goto _LL8A;else{_tmp11E=_tmp11D->f2;}}_LL89: _tmp120=_tmp11E;goto
_LL8B;_LL8A: {struct Cyc_Absyn_Eq_kb_struct*_tmp11F=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp11C;if(_tmp11F->tag != 0)goto _LL8C;else{_tmp120=_tmp11F->f1;}}_LL8B: k2=
_tmp120;goto _LL87;_LL8C:;_LL8D: {const char*_tmp46D;void*_tmp46C;(_tmp46C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp46D="unconstrained existential type variable in aggregate",
_tag_dyneither(_tmp46D,sizeof(char),53))),_tag_dyneither(_tmp46C,sizeof(void*),0)));}
_LL87:;}{void*_tmp123=_tmp11A;struct Cyc_Absyn_Kind*_tmp125;struct Cyc_Core_Opt*
_tmp127;struct Cyc_Core_Opt**_tmp128;struct Cyc_Absyn_Kind*_tmp129;struct Cyc_Core_Opt*
_tmp12B;struct Cyc_Core_Opt**_tmp12C;_LL8F: {struct Cyc_Absyn_Eq_kb_struct*_tmp124=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp123;if(_tmp124->tag != 0)goto _LL91;else{_tmp125=
_tmp124->f1;}}_LL90: if(!Cyc_Tcutil_kind_leq(k2,_tmp125))error=1;goto _LL8E;_LL91: {
struct Cyc_Absyn_Less_kb_struct*_tmp126=(struct Cyc_Absyn_Less_kb_struct*)_tmp123;
if(_tmp126->tag != 2)goto _LL93;else{_tmp127=_tmp126->f1;_tmp128=(struct Cyc_Core_Opt**)&
_tmp126->f1;_tmp129=_tmp126->f2;}}_LL92: _tmp12C=_tmp128;goto _LL94;_LL93: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp12A=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp123;
if(_tmp12A->tag != 1)goto _LL8E;else{_tmp12B=_tmp12A->f1;_tmp12C=(struct Cyc_Core_Opt**)&
_tmp12A->f1;}}_LL94:{struct Cyc_Core_Opt*_tmp46E;*_tmp12C=((_tmp46E=_cycalloc(
sizeof(*_tmp46E)),((_tmp46E->v=_tmp11B,_tmp46E))));}goto _LL8E;_LL8E:;}if(error){
const char*_tmp474;void*_tmp473[3];struct Cyc_String_pa_struct _tmp472;struct Cyc_String_pa_struct
_tmp471;struct Cyc_String_pa_struct _tmp470;(_tmp470.tag=0,((_tmp470.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(k2)),((_tmp471.tag=
0,((_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
_tmp11A)),((_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*tv->name),((_tmp473[0]=& _tmp472,((_tmp473[1]=& _tmp471,((_tmp473[
2]=& _tmp470,Cyc_Tcutil_terr(p->loc,((_tmp474="type variable %s has kind %s but must have at least kind %s",
_tag_dyneither(_tmp474,sizeof(char),60))),_tag_dyneither(_tmp473,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_Absyn_VarType_struct _tmp477;struct Cyc_Absyn_VarType_struct*_tmp476;
void*vartype=(void*)((_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476[0]=((_tmp477.tag=
2,((_tmp477.f1=tv,_tmp477)))),_tmp476))));{struct Cyc_List_List*_tmp478;_tmp118=((
_tmp478=_region_malloc(rgn,sizeof(*_tmp478)),((_tmp478->hd=(void*)vartype,((
_tmp478->tl=_tmp118,_tmp478))))));}if(k2->kind == Cyc_Absyn_RgnKind){if(k2->aliasqual
== Cyc_Absyn_Aliasable){struct _tuple0*_tmp47B;struct Cyc_List_List*_tmp47A;
outlives_constraints=((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->hd=((
_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->f1=Cyc_Absyn_empty_effect,((
_tmp47B->f2=vartype,_tmp47B)))))),((_tmp47A->tl=outlives_constraints,_tmp47A))))));}
else{const char*_tmp47E;void*_tmp47D;(_tmp47D=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp47E="opened existential had unique or top region kind",
_tag_dyneither(_tmp47E,sizeof(char),49))),_tag_dyneither(_tmp47D,sizeof(void*),0)));}}};};}}
_tmp118=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp118);{struct _RegionHandle*_tmp13A=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_add_type_vars(_tmp13A,p->loc,te,_tmpBB);struct Cyc_List_List*
_tmp13B=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple10 _tmp47F;struct _tuple10
_tmp13C=(_tmp47F.f1=_tmp13B,((_tmp47F.f2=rgn,_tmp47F)));struct Cyc_List_List*
_tmp13D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(
struct _tuple10*,struct Cyc_Absyn_Tvar*),struct _tuple10*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13C,_tmpB8->tvs);struct Cyc_List_List*
_tmp13E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->exist_vars,_tmp118);struct Cyc_List_List*_tmp13F=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple11*))Cyc_Core_snd,_tmp13D);struct Cyc_List_List*_tmp140=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple11*))Cyc_Core_snd,_tmp13E);struct Cyc_List_List*_tmp141=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmp13D,_tmp13E);if(_tmpBB != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0){struct
_tuple12*_tmp480;res.tvars_and_bounds_opt=((_tmp480=_cycalloc(sizeof(*_tmp480)),((
_tmp480->f1=0,((_tmp480->f2=0,_tmp480))))));}(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmpBB);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp143=0;{struct Cyc_List_List*
_tmp144=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po;for(0;
_tmp144 != 0;_tmp144=_tmp144->tl){struct _tuple0*_tmp483;struct Cyc_List_List*
_tmp482;_tmp143=((_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482->hd=((_tmp483=
_cycalloc(sizeof(*_tmp483)),((_tmp483->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp141,(*((
struct _tuple0*)_tmp144->hd)).f1),((_tmp483->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp141,(*((struct _tuple0*)_tmp144->hd)).f2),_tmp483)))))),((_tmp482->tl=_tmp143,
_tmp482))))));}}_tmp143=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp143);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp143);};}*
_tmpBA=_tmp13F;{struct Cyc_Absyn_AggrType_struct _tmp48D;struct Cyc_Absyn_Aggrdecl**
_tmp48C;struct Cyc_Absyn_AggrInfo _tmp48B;struct Cyc_Absyn_AggrType_struct*_tmp48A;
t=(void*)((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp48D.tag=12,((
_tmp48D.f1=((_tmp48B.aggr_info=Cyc_Absyn_KnownAggr(((_tmp48C=_cycalloc(sizeof(*
_tmp48C)),((_tmp48C[0]=_tmpB8,_tmp48C))))),((_tmp48B.targs=*_tmpBA,_tmp48B)))),
_tmp48D)))),_tmp48A))));}if(_tmpBE  && _tmpB8->kind == Cyc_Absyn_UnionA){const char*
_tmp490;void*_tmp48F;(_tmp48F=0,Cyc_Tcutil_warn(p->loc,((_tmp490="`...' pattern not allowed in union pattern",
_tag_dyneither(_tmp490,sizeof(char),43))),_tag_dyneither(_tmp48F,sizeof(void*),0)));}
else{if(_tmpBE){int _tmp14D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp113);int _tmp14E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->fields);if(_tmp14D < _tmp14E){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp14E - _tmp14D;++ i){struct _tuple14*_tmp493;struct
Cyc_List_List*_tmp492;wild_dps=((_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492->hd=((
_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->f1=0,((_tmp493->f2=Cyc_Tcpat_wild_pat(
p->loc),_tmp493)))))),((_tmp492->tl=wild_dps,_tmp492))))));}}*_tmpBD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp113,
wild_dps);_tmp113=*_tmpBD;}else{if(_tmp14D == _tmp14E){const char*_tmp496;void*
_tmp495;(_tmp495=0,Cyc_Tcutil_warn(p->loc,((_tmp496="unnecessary ... in struct pattern",
_tag_dyneither(_tmp496,sizeof(char),34))),_tag_dyneither(_tmp495,sizeof(void*),0)));}}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,
_tmp113,_tmpB8->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple16 _tmp154;struct Cyc_Absyn_Aggrfield*
_tmp155;struct Cyc_Absyn_Pat*_tmp156;struct _tuple16*_tmp153=(struct _tuple16*)
fields->hd;_tmp154=*_tmp153;_tmp155=_tmp154.f1;_tmp156=_tmp154.f2;{void*_tmp157=
Cyc_Tcutil_rsubstitute(rgn,_tmp141,_tmp155->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp156,(void**)& _tmp157,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp156->topt))->v,_tmp157)){const char*
_tmp49D;void*_tmp49C[4];struct Cyc_String_pa_struct _tmp49B;struct Cyc_String_pa_struct
_tmp49A;struct Cyc_String_pa_struct _tmp499;struct Cyc_String_pa_struct _tmp498;(
_tmp498.tag=0,((_tmp498.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp156->topt))->v)),((_tmp499.tag=0,((
_tmp499.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp157)),((_tmp49A.tag=0,((_tmp49A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp49B.tag=0,((_tmp49B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp155->name),((_tmp49C[0]=& _tmp49B,((_tmp49C[1]=& _tmp49A,((
_tmp49C[2]=& _tmp499,((_tmp49C[3]=& _tmp498,Cyc_Tcutil_terr(p->loc,((_tmp49D="field %s of %s pattern expects type %s != %s",
_tag_dyneither(_tmp49D,sizeof(char),45))),_tag_dyneither(_tmp49C,sizeof(void*),4)))))))))))))))))))))))));}};}};};}
_npop_handler(0);goto _LL43;;_pop_region(rgn);};}_LL62: {struct Cyc_Absyn_Datatype_p_struct*
_tmpBF=(struct Cyc_Absyn_Datatype_p_struct*)_tmp95;if(_tmpBF->tag != 7)goto _LL64;
else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=_tmpBF->f3;_tmpC3=(struct Cyc_List_List**)&
_tmpBF->f3;_tmpC4=_tmpBF->f4;}}_LL63: {struct Cyc_List_List*_tmp161=*_tmpC3;
struct _RegionHandle*_tmp162=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*tqts=
_tmpC1->typs;struct Cyc_List_List*_tmp163=Cyc_Tcenv_lookup_type_vars(te);struct
_tuple10 _tmp49E;struct _tuple10 _tmp164=(_tmp49E.f1=_tmp163,((_tmp49E.f2=_tmp162,
_tmp49E)));struct Cyc_List_List*_tmp165=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp162,Cyc_Tcutil_r_make_inst_var,&
_tmp164,_tmpC0->tvs);struct Cyc_List_List*_tmp166=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple11*))Cyc_Core_snd,_tmp165);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmp4A4;struct Cyc_Absyn_DatatypeFieldInfo _tmp4A3;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4A2;t=(void*)((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2[0]=((_tmp4A4.tag=
4,((_tmp4A4.f1=((_tmp4A3.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1),((
_tmp4A3.targs=_tmp166,_tmp4A3)))),_tmp4A4)))),_tmp4A2))));}if(_tmpC4){int _tmp16A=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp161);int _tmp16B=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp16A < _tmp16B){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp16B - _tmp16A;++ i){struct Cyc_List_List*_tmp4A5;
wild_ps=((_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5->hd=Cyc_Tcpat_wild_pat(p->loc),((
_tmp4A5->tl=wild_ps,_tmp4A5))))));}}*_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp161,wild_ps);_tmp161=*_tmpC3;}
else{if(_tmp16A == _tmp16B){const char*_tmp4A9;void*_tmp4A8[1];struct Cyc_String_pa_struct
_tmp4A7;(_tmp4A7.tag=0,((_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC0->name)),((_tmp4A8[0]=& _tmp4A7,Cyc_Tcutil_warn(p->loc,((
_tmp4A9="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4A9,sizeof(
char),37))),_tag_dyneither(_tmp4A8,sizeof(void*),1)))))));}}}for(0;_tmp161 != 0
 && tqts != 0;(_tmp161=_tmp161->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp170=(
struct Cyc_Absyn_Pat*)_tmp161->hd;void*_tmp171=Cyc_Tcutil_rsubstitute(_tmp162,
_tmp165,(*((struct _tuple15*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp170,(void**)& _tmp171,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp170->topt))->v,_tmp171)){const char*_tmp4AF;void*_tmp4AE[3];
struct Cyc_String_pa_struct _tmp4AD;struct Cyc_String_pa_struct _tmp4AC;struct Cyc_String_pa_struct
_tmp4AB;(_tmp4AB.tag=0,((_tmp4AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp170->topt))->v)),((
_tmp4AC.tag=0,((_tmp4AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp171)),((_tmp4AD.tag=0,((_tmp4AD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4AE[0]=& _tmp4AD,((
_tmp4AE[1]=& _tmp4AC,((_tmp4AE[2]=& _tmp4AB,Cyc_Tcutil_terr(_tmp170->loc,((_tmp4AF="%s expects argument type %s, not %s",
_tag_dyneither(_tmp4AF,sizeof(char),36))),_tag_dyneither(_tmp4AE,sizeof(void*),3)))))))))))))))))));}}
if(_tmp161 != 0){const char*_tmp4B3;void*_tmp4B2[1];struct Cyc_String_pa_struct
_tmp4B1;(_tmp4B1.tag=0,((_tmp4B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4B2[0]=& _tmp4B1,Cyc_Tcutil_terr(p->loc,((
_tmp4B3="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4B3,
sizeof(char),47))),_tag_dyneither(_tmp4B2,sizeof(void*),1)))))));}if(tqts != 0){
const char*_tmp4B7;void*_tmp4B6[1];struct Cyc_String_pa_struct _tmp4B5;(_tmp4B5.tag=
0,((_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC1->name)),((_tmp4B6[0]=& _tmp4B5,Cyc_Tcutil_terr(p->loc,((_tmp4B7="too few arguments for datatype constructor %s",
_tag_dyneither(_tmp4B7,sizeof(char),46))),_tag_dyneither(_tmp4B6,sizeof(void*),1)))))));}}
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
te));goto _LL43;_LL43:;}tcpat_end: {struct Cyc_Core_Opt*_tmp4B8;p->topt=((_tmp4B8=
_cycalloc(sizeof(*_tmp4B8)),((_tmp4B8->v=t,_tmp4B8))));}return res;};}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp180=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct
_RegionHandle*_tmp181=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp4B9;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp181,Cyc_Tcpat_get_name,_tmp180.patvars),
p->loc,((_tmp4B9="pattern contains a repeated variable",_tag_dyneither(_tmp4B9,
sizeof(char),37))));}return _tmp180;}void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p){void*_tmp183=p->r;struct Cyc_Absyn_Pat*_tmp185;struct
Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18B;
_LL96: {struct Cyc_Absyn_Pointer_p_struct*_tmp184=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp183;if(_tmp184->tag != 5)goto _LL98;else{_tmp185=_tmp184->f1;}}_LL97: Cyc_Tcpat_check_pat_regions(
te,_tmp185);{struct _RegionHandle*_tmp18C=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp18C,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp185->topt))->v)){{const char*
_tmp4BC;void*_tmp4BB;(_tmp4BB=0,Cyc_Tcutil_terr(p->loc,((_tmp4BC="Pattern dereferences to a non-aliased pointer; use swap",
_tag_dyneither(_tmp4BC,sizeof(char),56))),_tag_dyneither(_tmp4BB,sizeof(void*),0)));}
return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp18C,(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){{const char*_tmp4BF;void*_tmp4BE;(_tmp4BE=0,Cyc_Tcutil_terr(
p->loc,((_tmp4BF="Pattern dereferences a non-aliased pointer; use swap",
_tag_dyneither(_tmp4BF,sizeof(char),53))),_tag_dyneither(_tmp4BE,sizeof(void*),0)));}
return;}{void*_tmp191=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp193;struct Cyc_Absyn_PtrAtts _tmp194;void*_tmp195;_LLA1: {
struct Cyc_Absyn_PointerType_struct*_tmp192=(struct Cyc_Absyn_PointerType_struct*)
_tmp191;if(_tmp192->tag != 5)goto _LLA3;else{_tmp193=_tmp192->f1;_tmp194=_tmp193.ptr_atts;
_tmp195=_tmp194.rgn;}}_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp195);
return;_LLA3:;_LLA4: {const char*_tmp4C2;void*_tmp4C1;(_tmp4C1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C2="check_pat_regions: bad pointer type",
_tag_dyneither(_tmp4C2,sizeof(char),36))),_tag_dyneither(_tmp4C1,sizeof(void*),0)));}
_LLA0:;};};_LL98: {struct Cyc_Absyn_Aggr_p_struct*_tmp186=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp183;if(_tmp186->tag != 6)goto _LL9A;else{_tmp187=_tmp186->f3;}}_LL99: for(0;
_tmp187 != 0;_tmp187=_tmp187->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple14*)_tmp187->hd)).f2);}return;_LL9A: {struct Cyc_Absyn_Datatype_p_struct*
_tmp188=(struct Cyc_Absyn_Datatype_p_struct*)_tmp183;if(_tmp188->tag != 7)goto
_LL9C;else{_tmp189=_tmp188->f3;}}_LL9B: _tmp18B=_tmp189;goto _LL9D;_LL9C: {struct
Cyc_Absyn_Tuple_p_struct*_tmp18A=(struct Cyc_Absyn_Tuple_p_struct*)_tmp183;if(
_tmp18A->tag != 4)goto _LL9E;else{_tmp18B=_tmp18A->f1;}}_LL9D: for(0;_tmp18B != 0;
_tmp18B=_tmp18B->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp18B->hd);}return;_LL9E:;_LL9F: return;_LL95:;}struct _union_Name_value_Name_v{
int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};
union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct
_union_Name_value_Int_v Int_v;};union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct
_dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp4C3;return((_tmp4C3.Name_v).val=s,(((
_tmp4C3.Name_v).tag=1,_tmp4C3)));}union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){
union Cyc_Tcpat_Name_value _tmp4C4;return((_tmp4C4.Int_v).val=i,(((_tmp4C4.Int_v).tag=
2,_tmp4C4)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Any_struct{int tag;};struct
Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp19A=c1->name;struct _dyneither_ptr _tmp19B;int _tmp19C;
_LLA6: if((_tmp19A.Name_v).tag != 1)goto _LLA8;_tmp19B=(struct _dyneither_ptr)(
_tmp19A.Name_v).val;_LLA7: {union Cyc_Tcpat_Name_value _tmp19D=c2->name;struct
_dyneither_ptr _tmp19E;int _tmp19F;_LLAB: if((_tmp19D.Name_v).tag != 1)goto _LLAD;
_tmp19E=(struct _dyneither_ptr)(_tmp19D.Name_v).val;_LLAC: return Cyc_strcmp((
struct _dyneither_ptr)_tmp19B,(struct _dyneither_ptr)_tmp19E);_LLAD: if((_tmp19D.Int_v).tag
!= 2)goto _LLAA;_tmp19F=(int)(_tmp19D.Int_v).val;_LLAE: return - 1;_LLAA:;}_LLA8: if((
_tmp19A.Int_v).tag != 2)goto _LLA5;_tmp19C=(int)(_tmp19A.Int_v).val;_LLA9: {union
Cyc_Tcpat_Name_value _tmp1A0=c2->name;struct _dyneither_ptr _tmp1A1;int _tmp1A2;
_LLB0: if((_tmp1A0.Name_v).tag != 1)goto _LLB2;_tmp1A1=(struct _dyneither_ptr)(
_tmp1A0.Name_v).val;_LLB1: return 1;_LLB2: if((_tmp1A0.Int_v).tag != 2)goto _LLAF;
_tmp1A2=(int)(_tmp1A0.Int_v).val;_LLB3: return _tmp19C - _tmp1A2;_LLAF:;}_LLA5:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static int Cyc_Tcpat_one_opt=
1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=256;static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){const char*_tmp4C7;struct Cyc_Tcpat_Con_s*_tmp4C6;return(_tmp4C6=_region_malloc(
r,sizeof(*_tmp4C6)),((_tmp4C6->name=Cyc_Tcpat_Name_v(((_tmp4C7="NULL",
_tag_dyneither(_tmp4C7,sizeof(char),5)))),((_tmp4C6->arity=0,((_tmp4C6->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4C6->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C6)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){const char*_tmp4CA;struct Cyc_Tcpat_Con_s*_tmp4C9;return(_tmp4C9=
_region_malloc(r,sizeof(*_tmp4C9)),((_tmp4C9->name=Cyc_Tcpat_Name_v(((_tmp4CA="&",
_tag_dyneither(_tmp4CA,sizeof(char),2)))),((_tmp4C9->arity=1,((_tmp4C9->span=(
int*)& Cyc_Tcpat_two_opt,((_tmp4C9->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4C9)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){const char*_tmp4CD;struct Cyc_Tcpat_Con_s*_tmp4CC;return(_tmp4CC=_region_malloc(
r,sizeof(*_tmp4CC)),((_tmp4CC->name=Cyc_Tcpat_Name_v(((_tmp4CD="&",
_tag_dyneither(_tmp4CD,sizeof(char),2)))),((_tmp4CC->arity=1,((_tmp4CC->span=(
int*)& Cyc_Tcpat_one_opt,((_tmp4CC->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4CC)))))))));}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4CE;return(_tmp4CE=_region_malloc(r,
sizeof(*_tmp4CE)),((_tmp4CE->name=Cyc_Tcpat_Int_v(i),((_tmp4CE->arity=0,((
_tmp4CE->span=0,((_tmp4CE->orig_pat=p,_tmp4CE)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4CF;return(_tmp4CF=_region_malloc(r,sizeof(*_tmp4CF)),((
_tmp4CF->name=Cyc_Tcpat_Name_v(f),((_tmp4CF->arity=0,((_tmp4CF->span=0,((_tmp4CF->orig_pat=(
struct Cyc_Absyn_Pat*)p,_tmp4CF)))))))));}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_tmp4D0;
return(_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0->name=Cyc_Tcpat_Int_v((
int)c),((_tmp4D0->arity=0,((_tmp4D0->span=(int*)& Cyc_Tcpat_twofiftysix_opt,((
_tmp4D0->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4D0)))))))));}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){const char*
_tmp4D3;struct Cyc_Tcpat_Con_s*_tmp4D2;return(_tmp4D2=_region_malloc(r,sizeof(*
_tmp4D2)),((_tmp4D2->name=Cyc_Tcpat_Name_v(((_tmp4D3="$",_tag_dyneither(_tmp4D3,
sizeof(char),2)))),((_tmp4D2->arity=i,((_tmp4D2->span=(int*)& Cyc_Tcpat_one_opt,((
_tmp4D2->orig_pat=p,_tmp4D2)))))))));}static void*Cyc_Tcpat_null_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4D6;struct
Cyc_Tcpat_Con_struct*_tmp4D5;return(void*)((_tmp4D5=_region_malloc(r,sizeof(*
_tmp4D5)),((_tmp4D5[0]=((_tmp4D6.tag=1,((_tmp4D6.f1=Cyc_Tcpat_null_con(r,p),((
_tmp4D6.f2=0,_tmp4D6)))))),_tmp4D5))));}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4D9;
struct Cyc_Tcpat_Con_struct*_tmp4D8;return(void*)((_tmp4D8=_region_malloc(r,
sizeof(*_tmp4D8)),((_tmp4D8[0]=((_tmp4D9.tag=1,((_tmp4D9.f1=Cyc_Tcpat_int_con(r,
i,p),((_tmp4D9.f2=0,_tmp4D9)))))),_tmp4D8))));}static void*Cyc_Tcpat_char_pat(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4DC;struct Cyc_Tcpat_Con_struct*_tmp4DB;return(void*)((_tmp4DB=_region_malloc(
r,sizeof(*_tmp4DB)),((_tmp4DB[0]=((_tmp4DC.tag=1,((_tmp4DC.f1=Cyc_Tcpat_char_con(
r,c,p),((_tmp4DC.f2=0,_tmp4DC)))))),_tmp4DB))));}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct
_tmp4DF;struct Cyc_Tcpat_Con_struct*_tmp4DE;return(void*)((_tmp4DE=_region_malloc(
r,sizeof(*_tmp4DE)),((_tmp4DE[0]=((_tmp4DF.tag=1,((_tmp4DF.f1=Cyc_Tcpat_float_con(
r,f,p),((_tmp4DF.f2=0,_tmp4DF)))))),_tmp4DE))));}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_struct
_tmp4E5;struct Cyc_List_List*_tmp4E4;struct Cyc_Tcpat_Con_struct*_tmp4E3;return(
void*)((_tmp4E3=_region_malloc(r,sizeof(*_tmp4E3)),((_tmp4E3[0]=((_tmp4E5.tag=1,((
_tmp4E5.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4E5.f2=((_tmp4E4=_region_malloc(r,
sizeof(*_tmp4E4)),((_tmp4E4->hd=p,((_tmp4E4->tl=0,_tmp4E4)))))),_tmp4E5)))))),
_tmp4E3))));}static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*
p0){struct Cyc_Tcpat_Con_struct _tmp4EB;struct Cyc_List_List*_tmp4EA;struct Cyc_Tcpat_Con_struct*
_tmp4E9;return(void*)((_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9)),((_tmp4E9[0]=((
_tmp4EB.tag=1,((_tmp4EB.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4EB.f2=((_tmp4EA=
_region_malloc(r,sizeof(*_tmp4EA)),((_tmp4EA->hd=p,((_tmp4EA->tl=0,_tmp4EA)))))),
_tmp4EB)))))),_tmp4E9))));}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_struct _tmp4EE;
struct Cyc_Tcpat_Con_struct*_tmp4ED;return(void*)((_tmp4ED=_region_malloc(r,
sizeof(*_tmp4ED)),((_tmp4ED[0]=((_tmp4EE.tag=1,((_tmp4EE.f1=Cyc_Tcpat_tuple_con(
r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4EE.f2=ss,
_tmp4EE)))))),_tmp4ED))));}static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,
struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*
p){struct Cyc_Tcpat_Con_s*_tmp4EF;struct Cyc_Tcpat_Con_s*c=(_tmp4EF=_region_malloc(
r,sizeof(*_tmp4EF)),((_tmp4EF->name=Cyc_Tcpat_Name_v(con_name),((_tmp4EF->arity=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4EF->span=span,((
_tmp4EF->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4EF)))))))));struct Cyc_Tcpat_Con_struct
_tmp4F2;struct Cyc_Tcpat_Con_struct*_tmp4F1;return(void*)((_tmp4F1=_region_malloc(
r,sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F2.tag=1,((_tmp4F2.f1=c,((_tmp4F2.f2=ps,
_tmp4F2)))))),_tmp4F1))));}static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*
r,struct Cyc_Absyn_Pat*p){void*s;{void*_tmp1C1=p->r;enum Cyc_Absyn_Sign _tmp1C6;
int _tmp1C7;char _tmp1C9;struct _dyneither_ptr _tmp1CB;struct Cyc_Absyn_Pat*_tmp1CD;
struct Cyc_Absyn_Pat*_tmp1CF;struct Cyc_Absyn_Pat*_tmp1D1;struct Cyc_Absyn_Datatypedecl*
_tmp1D3;struct Cyc_Absyn_Datatypefield*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct
Cyc_List_List*_tmp1D7;struct Cyc_Absyn_AggrInfo*_tmp1D9;struct Cyc_Absyn_AggrInfo
_tmp1DA;union Cyc_Absyn_AggrInfoU _tmp1DB;struct Cyc_Absyn_Aggrdecl**_tmp1DC;struct
Cyc_Absyn_Aggrdecl*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Enumdecl*
_tmp1E0;struct Cyc_Absyn_Enumfield*_tmp1E1;void*_tmp1E3;struct Cyc_Absyn_Enumfield*
_tmp1E4;_LLB5: {struct Cyc_Absyn_Wild_p_struct*_tmp1C2=(struct Cyc_Absyn_Wild_p_struct*)
_tmp1C1;if(_tmp1C2->tag != 0)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_TagInt_p_struct*
_tmp1C3=(struct Cyc_Absyn_TagInt_p_struct*)_tmp1C1;if(_tmp1C3->tag != 3)goto _LLB9;}
_LLB8:{struct Cyc_Tcpat_Any_struct _tmp4F5;struct Cyc_Tcpat_Any_struct*_tmp4F4;s=(
void*)((_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4F5.tag=0,
_tmp4F5)),_tmp4F4))));}goto _LLB4;_LLB9: {struct Cyc_Absyn_Null_p_struct*_tmp1C4=(
struct Cyc_Absyn_Null_p_struct*)_tmp1C1;if(_tmp1C4->tag != 8)goto _LLBB;}_LLBA: s=
Cyc_Tcpat_null_pat(r,p);goto _LLB4;_LLBB: {struct Cyc_Absyn_Int_p_struct*_tmp1C5=(
struct Cyc_Absyn_Int_p_struct*)_tmp1C1;if(_tmp1C5->tag != 9)goto _LLBD;else{_tmp1C6=
_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;}}_LLBC: s=Cyc_Tcpat_int_pat(r,_tmp1C7,(struct Cyc_Absyn_Pat*)
p);goto _LLB4;_LLBD: {struct Cyc_Absyn_Char_p_struct*_tmp1C8=(struct Cyc_Absyn_Char_p_struct*)
_tmp1C1;if(_tmp1C8->tag != 10)goto _LLBF;else{_tmp1C9=_tmp1C8->f1;}}_LLBE: s=Cyc_Tcpat_char_pat(
r,_tmp1C9,p);goto _LLB4;_LLBF: {struct Cyc_Absyn_Float_p_struct*_tmp1CA=(struct Cyc_Absyn_Float_p_struct*)
_tmp1C1;if(_tmp1CA->tag != 11)goto _LLC1;else{_tmp1CB=_tmp1CA->f1;}}_LLC0: s=Cyc_Tcpat_float_pat(
r,_tmp1CB,p);goto _LLB4;_LLC1: {struct Cyc_Absyn_Var_p_struct*_tmp1CC=(struct Cyc_Absyn_Var_p_struct*)
_tmp1C1;if(_tmp1CC->tag != 1)goto _LLC3;else{_tmp1CD=_tmp1CC->f2;}}_LLC2: s=Cyc_Tcpat_compile_pat(
r,_tmp1CD);goto _LLB4;_LLC3: {struct Cyc_Absyn_Reference_p_struct*_tmp1CE=(struct
Cyc_Absyn_Reference_p_struct*)_tmp1C1;if(_tmp1CE->tag != 2)goto _LLC5;else{_tmp1CF=
_tmp1CE->f2;}}_LLC4: s=Cyc_Tcpat_compile_pat(r,_tmp1CF);goto _LLB4;_LLC5: {struct
Cyc_Absyn_Pointer_p_struct*_tmp1D0=(struct Cyc_Absyn_Pointer_p_struct*)_tmp1C1;
if(_tmp1D0->tag != 5)goto _LLC7;else{_tmp1D1=_tmp1D0->f1;}}_LLC6:{void*_tmp1EB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1ED;struct Cyc_Absyn_PtrAtts _tmp1EE;union Cyc_Absyn_Constraint*_tmp1EF;_LLDA: {
struct Cyc_Absyn_PointerType_struct*_tmp1EC=(struct Cyc_Absyn_PointerType_struct*)
_tmp1EB;if(_tmp1EC->tag != 5)goto _LLDC;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1ED.ptr_atts;
_tmp1EF=_tmp1EE.nullable;}}_LLDB: {int is_nullable=0;int still_working=1;while(
still_working){union Cyc_Absyn_Constraint*_tmp1F0=_tmp1EF;union Cyc_Absyn_Constraint
_tmp1F1;union Cyc_Absyn_Constraint*_tmp1F2;union Cyc_Absyn_Constraint _tmp1F3;int
_tmp1F4;union Cyc_Absyn_Constraint _tmp1F5;int _tmp1F6;_LLDF: _tmp1F1=*_tmp1F0;if((
_tmp1F1.Forward_constr).tag != 2)goto _LLE1;_tmp1F2=(union Cyc_Absyn_Constraint*)(
_tmp1F1.Forward_constr).val;_LLE0:*_tmp1EF=*_tmp1F2;continue;_LLE1: _tmp1F3=*
_tmp1F0;if((_tmp1F3.No_constr).tag != 3)goto _LLE3;_tmp1F4=(int)(_tmp1F3.No_constr).val;
_LLE2:{struct _union_Constraint_Eq_constr*_tmp4F6;(_tmp4F6=& _tmp1EF->Eq_constr,((
_tmp4F6->tag=1,_tmp4F6->val=0)));}is_nullable=0;still_working=0;goto _LLDE;_LLE3:
_tmp1F5=*_tmp1F0;if((_tmp1F5.Eq_constr).tag != 1)goto _LLDE;_tmp1F6=(int)(_tmp1F5.Eq_constr).val;
_LLE4: is_nullable=(int)_tmp1F6;still_working=0;goto _LLDE;_LLDE:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp1D1);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLD9;};}_LLDC:;_LLDD: {const char*_tmp4F9;void*_tmp4F8;(_tmp4F8=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp4F9="expecting pointertype for pattern!",_tag_dyneither(_tmp4F9,sizeof(char),
35))),_tag_dyneither(_tmp4F8,sizeof(void*),0)));}_LLD9:;}goto _LLB4;_LLC7: {
struct Cyc_Absyn_Datatype_p_struct*_tmp1D2=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp1C1;if(_tmp1D2->tag != 7)goto _LLC9;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;
_tmp1D5=_tmp1D2->f3;}}_LLC8: {int*span;{void*_tmp1FA=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LLE6: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1FB=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1FA;if(_tmp1FB->tag != 3)goto
_LLE8;}_LLE7: if(_tmp1D3->is_extensible)span=0;else{int*_tmp4FA;span=((_tmp4FA=
_region_malloc(r,sizeof(*_tmp4FA)),((_tmp4FA[0]=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D3->fields))->v),
_tmp4FA))));}goto _LLE5;_LLE8: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1FC=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1FA;if(_tmp1FC->tag != 4)goto _LLEA;}
_LLE9: span=(int*)& Cyc_Tcpat_one_opt;goto _LLE5;_LLEA:;_LLEB:{const char*_tmp4FD;
void*_tmp4FC;span=((_tmp4FC=0,((int*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FD="void datatype pattern has bad type",
_tag_dyneither(_tmp4FD,sizeof(char),35))),_tag_dyneither(_tmp4FC,sizeof(void*),0))));}
goto _LLE5;_LLE5:;}s=Cyc_Tcpat_con_pat(r,*(*_tmp1D4->name).f2,span,((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D5),p);goto _LLB4;}_LLC9: {struct Cyc_Absyn_Tuple_p_struct*_tmp1D6=(struct
Cyc_Absyn_Tuple_p_struct*)_tmp1C1;if(_tmp1D6->tag != 4)goto _LLCB;else{_tmp1D7=
_tmp1D6->f1;}}_LLCA: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D7),(struct Cyc_Absyn_Pat*)p);goto _LLB4;_LLCB: {struct Cyc_Absyn_Aggr_p_struct*
_tmp1D8=(struct Cyc_Absyn_Aggr_p_struct*)_tmp1C1;if(_tmp1D8->tag != 6)goto _LLCD;
else{_tmp1D9=_tmp1D8->f1;if(_tmp1D9 == 0)goto _LLCD;_tmp1DA=*_tmp1D9;_tmp1DB=
_tmp1DA.aggr_info;if((_tmp1DB.KnownAggr).tag != 2)goto _LLCD;_tmp1DC=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1DB.KnownAggr).val;_tmp1DD=*_tmp1DC;_tmp1DE=_tmp1D8->f3;}}_LLCC: if(_tmp1DD->kind
== Cyc_Absyn_StructA){struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->fields;for(0;fields != 
0;fields=fields->tl){const char*_tmp4FE;int found=Cyc_strcmp((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4FE="",_tag_dyneither(_tmp4FE,
sizeof(char),1))))== 0;{struct Cyc_List_List*dlps0=_tmp1DE;for(0;!found  && dlps0
!= 0;dlps0=dlps0->tl){struct _tuple14 _tmp201;struct Cyc_List_List*_tmp202;struct
Cyc_Absyn_Pat*_tmp203;struct _tuple14*_tmp200=(struct _tuple14*)dlps0->hd;_tmp201=*
_tmp200;_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;{struct Cyc_List_List*_tmp204=
_tmp202;struct Cyc_List_List _tmp205;void*_tmp206;struct _dyneither_ptr*_tmp208;
struct Cyc_List_List*_tmp209;_LLED: if(_tmp204 == 0)goto _LLEF;_tmp205=*_tmp204;
_tmp206=(void*)_tmp205.hd;{struct Cyc_Absyn_FieldName_struct*_tmp207=(struct Cyc_Absyn_FieldName_struct*)
_tmp206;if(_tmp207->tag != 1)goto _LLEF;else{_tmp208=_tmp207->f1;}};_tmp209=
_tmp205.tl;if(_tmp209 != 0)goto _LLEF;_LLEE: if(Cyc_strptrcmp(_tmp208,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){{struct Cyc_List_List*_tmp4FF;ps=((_tmp4FF=_region_malloc(
r,sizeof(*_tmp4FF)),((_tmp4FF->hd=Cyc_Tcpat_compile_pat(r,_tmp203),((_tmp4FF->tl=
ps,_tmp4FF))))));}found=1;}goto _LLEC;_LLEF:;_LLF0: {const char*_tmp502;void*
_tmp501;(_tmp501=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp502="bad designator(s)",_tag_dyneither(_tmp502,sizeof(char),18))),
_tag_dyneither(_tmp501,sizeof(void*),0)));}_LLEC:;};}}if(!found){const char*
_tmp505;void*_tmp504;(_tmp504=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp505="bad designator",_tag_dyneither(
_tmp505,sizeof(char),15))),_tag_dyneither(_tmp504,sizeof(void*),0)));}}}s=Cyc_Tcpat_tuple_pat(
r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DD->impl))->tagged){const char*_tmp508;void*_tmp507;(_tmp507=0,
Cyc_Tcutil_terr(p->loc,((_tmp508="patterns on untagged unions not yet supported.",
_tag_dyneither(_tmp508,sizeof(char),47))),_tag_dyneither(_tmp507,sizeof(void*),0)));}{
int*_tmp509;int*span=(_tmp509=_region_malloc(r,sizeof(*_tmp509)),((_tmp509[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DD->impl))->fields),_tmp509)));int field_name;struct Cyc_List_List*
_tmp212=_tmp1DE;struct Cyc_List_List _tmp213;struct _tuple14*_tmp214;struct _tuple14
_tmp215;struct Cyc_List_List*_tmp216;struct Cyc_List_List _tmp217;void*_tmp218;
struct _dyneither_ptr*_tmp21A;struct Cyc_List_List*_tmp21B;struct Cyc_Absyn_Pat*
_tmp21C;struct Cyc_List_List*_tmp21D;_LLF2: if(_tmp212 == 0)goto _LLF4;_tmp213=*
_tmp212;_tmp214=(struct _tuple14*)_tmp213.hd;_tmp215=*_tmp214;_tmp216=_tmp215.f1;
if(_tmp216 == 0)goto _LLF4;_tmp217=*_tmp216;_tmp218=(void*)_tmp217.hd;{struct Cyc_Absyn_FieldName_struct*
_tmp219=(struct Cyc_Absyn_FieldName_struct*)_tmp218;if(_tmp219->tag != 1)goto _LLF4;
else{_tmp21A=_tmp219->f1;}};_tmp21B=_tmp217.tl;if(_tmp21B != 0)goto _LLF4;_tmp21C=
_tmp215.f2;_tmp21D=_tmp213.tl;if(_tmp21D != 0)goto _LLF4;_LLF3:{struct Cyc_List_List*
_tmp50A;s=Cyc_Tcpat_con_pat(r,*_tmp21A,span,((_tmp50A=_region_malloc(r,sizeof(*
_tmp50A)),((_tmp50A->hd=Cyc_Tcpat_compile_pat(r,_tmp21C),((_tmp50A->tl=0,_tmp50A)))))),
p);}goto _LLF1;_LLF4:;_LLF5: {const char*_tmp50D;void*_tmp50C;(_tmp50C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50D="bad union pattern",
_tag_dyneither(_tmp50D,sizeof(char),18))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}
_LLF1:;};}goto _LLB4;_LLCD: {struct Cyc_Absyn_Enum_p_struct*_tmp1DF=(struct Cyc_Absyn_Enum_p_struct*)
_tmp1C1;if(_tmp1DF->tag != 12)goto _LLCF;else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}
_LLCE: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1E0->fields))->v);{int*_tmp50E;s=Cyc_Tcpat_con_pat(
r,*(*_tmp1E1->name).f2,((_tmp50E=_region_malloc(r,sizeof(*_tmp50E)),((_tmp50E[0]=
span,_tmp50E)))),0,p);}goto _LLB4;}_LLCF: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E2=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1C1;if(_tmp1E2->tag != 13)goto
_LLD1;else{_tmp1E3=(void*)_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LLD0: {struct Cyc_List_List*
fields;{void*_tmp223=Cyc_Tcutil_compress(_tmp1E3);struct Cyc_List_List*_tmp225;
_LLF7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp224=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp223;if(_tmp224->tag != 15)goto _LLF9;else{_tmp225=_tmp224->f1;}}_LLF8: fields=
_tmp225;goto _LLF6;_LLF9:;_LLFA: {const char*_tmp511;void*_tmp510;(_tmp510=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp511="bad type in AnonEnum_p",
_tag_dyneither(_tmp511,sizeof(char),23))),_tag_dyneither(_tmp510,sizeof(void*),0)));}
_LLF6:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);{int*
_tmp512;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E4->name).f2,((_tmp512=_region_malloc(r,
sizeof(*_tmp512)),((_tmp512[0]=span,_tmp512)))),0,p);}goto _LLB4;};}_LLD1: {
struct Cyc_Absyn_UnknownId_p_struct*_tmp1E5=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1C1;if(_tmp1E5->tag != 14)goto _LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_UnknownCall_p_struct*
_tmp1E6=(struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1C1;if(_tmp1E6->tag != 15)goto
_LLD5;}_LLD4: goto _LLD6;_LLD5: {struct Cyc_Absyn_Aggr_p_struct*_tmp1E7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp1C1;if(_tmp1E7->tag != 6)goto _LLD7;}_LLD6: goto _LLD8;_LLD7: {struct Cyc_Absyn_Exp_p_struct*
_tmp1E8=(struct Cyc_Absyn_Exp_p_struct*)_tmp1C1;if(_tmp1E8->tag != 16)goto _LLB4;}
_LLD8: {struct Cyc_Tcpat_Any_struct _tmp515;struct Cyc_Tcpat_Any_struct*_tmp514;s=(
void*)((_tmp514=_region_malloc(r,sizeof(*_tmp514)),((_tmp514[0]=((_tmp515.tag=0,
_tmp515)),_tmp514))));}_LLB4:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;
struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes
 = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){const char*_tmp518;void*_tmp517;(_tmp517=0,Cyc_fprintf(Cyc_stderr,((
_tmp518=" ",_tag_dyneither(_tmp518,sizeof(char),2))),_tag_dyneither(_tmp517,
sizeof(void*),0)));}}static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp22D=c->name;union Cyc_Tcpat_Name_value _tmp22E=
_tmp22D;struct _dyneither_ptr _tmp22F;int _tmp230;_LLFC: if((_tmp22E.Name_v).tag != 1)
goto _LLFE;_tmp22F=(struct _dyneither_ptr)(_tmp22E.Name_v).val;_LLFD:{const char*
_tmp51C;void*_tmp51B[1];struct Cyc_String_pa_struct _tmp51A;(_tmp51A.tag=0,((
_tmp51A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22F),((_tmp51B[0]=&
_tmp51A,Cyc_fprintf(Cyc_stderr,((_tmp51C="%s",_tag_dyneither(_tmp51C,sizeof(char),
3))),_tag_dyneither(_tmp51B,sizeof(void*),1)))))));}goto _LLFB;_LLFE: if((_tmp22E.Int_v).tag
!= 2)goto _LLFB;_tmp230=(int)(_tmp22E.Int_v).val;_LLFF:{const char*_tmp520;void*
_tmp51F[1];struct Cyc_Int_pa_struct _tmp51E;(_tmp51E.tag=1,((_tmp51E.f1=(
unsigned long)_tmp230,((_tmp51F[0]=& _tmp51E,Cyc_fprintf(Cyc_stderr,((_tmp520="%d",
_tag_dyneither(_tmp520,sizeof(char),3))),_tag_dyneither(_tmp51F,sizeof(void*),1)))))));}
goto _LLFB;_LLFB:;}static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(
void*),int tab){void*_tmp237=d;void*_tmp239;struct Cyc_List_List*_tmp23C;struct Cyc_Tcpat_Con_s*
_tmp23D;void*_tmp23E;void*_tmp23F;_LL101: {struct Cyc_Tcpat_Success_struct*
_tmp238=(struct Cyc_Tcpat_Success_struct*)_tmp237;if(_tmp238->tag != 1)goto _LL103;
else{_tmp239=(void*)_tmp238->f1;}}_LL102: Cyc_Tcpat_print_tab(tab);{const char*
_tmp523;void*_tmp522;(_tmp522=0,Cyc_fprintf(Cyc_stderr,((_tmp523="Success(",
_tag_dyneither(_tmp523,sizeof(char),9))),_tag_dyneither(_tmp522,sizeof(void*),0)));}
print_rhs(_tmp239);{const char*_tmp526;void*_tmp525;(_tmp525=0,Cyc_fprintf(Cyc_stderr,((
_tmp526=")\n",_tag_dyneither(_tmp526,sizeof(char),3))),_tag_dyneither(_tmp525,
sizeof(void*),0)));}goto _LL100;_LL103: {struct Cyc_Tcpat_Failure_struct*_tmp23A=(
struct Cyc_Tcpat_Failure_struct*)_tmp237;if(_tmp23A->tag != 0)goto _LL105;}_LL104:{
const char*_tmp529;void*_tmp528;(_tmp528=0,Cyc_fprintf(Cyc_stderr,((_tmp529="Failure\n",
_tag_dyneither(_tmp529,sizeof(char),9))),_tag_dyneither(_tmp528,sizeof(void*),0)));}
goto _LL100;_LL105: {struct Cyc_Tcpat_IfEq_struct*_tmp23B=(struct Cyc_Tcpat_IfEq_struct*)
_tmp237;if(_tmp23B->tag != 2)goto _LL100;else{_tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;
_tmp23E=(void*)_tmp23B->f3;_tmp23F=(void*)_tmp23B->f4;}}_LL106: Cyc_Tcpat_print_tab(
tab);{const char*_tmp52C;void*_tmp52B;(_tmp52B=0,Cyc_fprintf(Cyc_stderr,((_tmp52C="Access[",
_tag_dyneither(_tmp52C,sizeof(char),8))),_tag_dyneither(_tmp52B,sizeof(void*),0)));}
for(0;_tmp23C != 0;_tmp23C=_tmp23C->tl){{const char*_tmp530;void*_tmp52F[1];struct
Cyc_Int_pa_struct _tmp52E;(_tmp52E.tag=1,((_tmp52E.f1=(unsigned long)((int)
_tmp23C->hd),((_tmp52F[0]=& _tmp52E,Cyc_fprintf(Cyc_stderr,((_tmp530="%d",
_tag_dyneither(_tmp530,sizeof(char),3))),_tag_dyneither(_tmp52F,sizeof(void*),1)))))));}
if(_tmp23C->tl != 0){const char*_tmp533;void*_tmp532;(_tmp532=0,Cyc_fprintf(Cyc_stderr,((
_tmp533=",",_tag_dyneither(_tmp533,sizeof(char),2))),_tag_dyneither(_tmp532,
sizeof(void*),0)));}}{const char*_tmp536;void*_tmp535;(_tmp535=0,Cyc_fprintf(Cyc_stderr,((
_tmp536="],",_tag_dyneither(_tmp536,sizeof(char),3))),_tag_dyneither(_tmp535,
sizeof(void*),0)));}Cyc_Tcpat_print_con(_tmp23D);{const char*_tmp539;void*_tmp538;(
_tmp538=0,Cyc_fprintf(Cyc_stderr,((_tmp539=",\n",_tag_dyneither(_tmp539,sizeof(
char),3))),_tag_dyneither(_tmp538,sizeof(void*),0)));}Cyc_Tcpat_print_dec_tree(
_tmp23E,print_rhs,tab + 7);Cyc_Tcpat_print_dec_tree(_tmp23F,print_rhs,tab + 7);
_LL100:;}struct _tuple18{int f1;struct Cyc_Position_Segment*f2;};static void Cyc_Tcpat_print_swrhs(
struct _tuple18*x){return;}static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp251=td;struct Cyc_Set_Set*_tmp253;
_LL108: {struct Cyc_Tcpat_Neg_struct*_tmp252=(struct Cyc_Tcpat_Neg_struct*)_tmp251;
if(_tmp252->tag != 1)goto _LL10A;else{_tmp253=_tmp252->f1;}}_LL109: {struct Cyc_Tcpat_Neg_struct
_tmp53C;struct Cyc_Tcpat_Neg_struct*_tmp53B;return(void*)((_tmp53B=_region_malloc(
r,sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53C.tag=1,((_tmp53C.f1=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(
r,_tmp253,c),_tmp53C)))),_tmp53B))));}_LL10A: {struct Cyc_Tcpat_Pos_struct*
_tmp254=(struct Cyc_Tcpat_Pos_struct*)_tmp251;if(_tmp254->tag != 0)goto _LL107;}
_LL10B: {const char*_tmp53F;void*_tmp53E;(_tmp53E=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp53F="add_neg called when td is Positive",
_tag_dyneither(_tmp53F,sizeof(char),35))),_tag_dyneither(_tmp53E,sizeof(void*),0)));}
_LL107:;}static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp259=td;struct Cyc_Tcpat_Con_s*_tmp25B;struct Cyc_Set_Set*
_tmp25D;_LL10D: {struct Cyc_Tcpat_Pos_struct*_tmp25A=(struct Cyc_Tcpat_Pos_struct*)
_tmp259;if(_tmp25A->tag != 0)goto _LL10F;else{_tmp25B=_tmp25A->f1;}}_LL10E: if(Cyc_Tcpat_compare_con(
c,_tmp25B)== 0)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_No;}_LL10F: {struct Cyc_Tcpat_Neg_struct*
_tmp25C=(struct Cyc_Tcpat_Neg_struct*)_tmp259;if(_tmp25C->tag != 1)goto _LL10C;
else{_tmp25D=_tmp25C->f1;}}_LL110: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp25D,c))return Cyc_Tcpat_No;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp25D)+ 1)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_Maybe;}}_LL10C:;}struct
_tuple19{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp25E=ctxt;struct Cyc_List_List _tmp25F;struct _tuple19*
_tmp260;struct _tuple19 _tmp261;struct Cyc_Tcpat_Con_s*_tmp262;struct Cyc_List_List*
_tmp263;struct Cyc_List_List*_tmp264;_LL112: if(_tmp25E != 0)goto _LL114;_LL113:
return 0;_LL114: if(_tmp25E == 0)goto _LL111;_tmp25F=*_tmp25E;_tmp260=(struct
_tuple19*)_tmp25F.hd;_tmp261=*_tmp260;_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;
_tmp264=_tmp25F.tl;_LL115: {struct _tuple19*_tmp545;struct Cyc_List_List*_tmp544;
struct Cyc_List_List*_tmp543;return(_tmp543=_region_malloc(r,sizeof(*_tmp543)),((
_tmp543->hd=((_tmp545=_region_malloc(r,sizeof(*_tmp545)),((_tmp545->f1=_tmp262,((
_tmp545->f2=(struct Cyc_List_List*)((_tmp544=_region_malloc(r,sizeof(*_tmp544)),((
_tmp544->hd=dsc,((_tmp544->tl=_tmp263,_tmp544)))))),_tmp545)))))),((_tmp543->tl=
_tmp264,_tmp543)))));}_LL111:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(
struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp268=ctxt;
struct Cyc_List_List _tmp269;struct _tuple19*_tmp26A;struct _tuple19 _tmp26B;struct
Cyc_Tcpat_Con_s*_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26E;
_LL117: if(_tmp268 != 0)goto _LL119;_LL118: {const char*_tmp548;void*_tmp547;(
_tmp547=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp548="norm_context: empty context",_tag_dyneither(_tmp548,sizeof(char),28))),
_tag_dyneither(_tmp547,sizeof(void*),0)));}_LL119: if(_tmp268 == 0)goto _LL116;
_tmp269=*_tmp268;_tmp26A=(struct _tuple19*)_tmp269.hd;_tmp26B=*_tmp26A;_tmp26C=
_tmp26B.f1;_tmp26D=_tmp26B.f2;_tmp26E=_tmp269.tl;_LL11A: {struct Cyc_Tcpat_Pos_struct
_tmp54B;struct Cyc_Tcpat_Pos_struct*_tmp54A;return Cyc_Tcpat_augment(r,_tmp26E,(
void*)((_tmp54A=_region_malloc(r,sizeof(*_tmp54A)),((_tmp54A[0]=((_tmp54B.tag=0,((
_tmp54B.f1=_tmp26C,((_tmp54B.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp26D),_tmp54B)))))),_tmp54A)))));}
_LL116:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple12 _tmp54C;struct _tuple12
_tmp274=(_tmp54C.f1=ctxt,((_tmp54C.f2=work,_tmp54C)));struct Cyc_List_List*
_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*
_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List _tmp27A;struct _tuple19*
_tmp27B;struct _tuple19 _tmp27C;struct Cyc_Tcpat_Con_s*_tmp27D;struct Cyc_List_List*
_tmp27E;struct Cyc_List_List*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_List_List
_tmp281;struct _tuple17*_tmp282;struct _tuple17 _tmp283;struct Cyc_List_List*_tmp284;
struct Cyc_List_List*_tmp285;_LL11C: _tmp275=_tmp274.f1;if(_tmp275 != 0)goto _LL11E;
_tmp276=_tmp274.f2;if(_tmp276 != 0)goto _LL11E;_LL11D: return dsc;_LL11E: _tmp277=
_tmp274.f1;if(_tmp277 != 0)goto _LL120;_LL11F: goto _LL121;_LL120: _tmp278=_tmp274.f2;
if(_tmp278 != 0)goto _LL122;_LL121: {const char*_tmp54F;void*_tmp54E;(_tmp54E=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp54F="build_desc: ctxt and work don't match",_tag_dyneither(_tmp54F,sizeof(
char),38))),_tag_dyneither(_tmp54E,sizeof(void*),0)));}_LL122: _tmp279=_tmp274.f1;
if(_tmp279 == 0)goto _LL11B;_tmp27A=*_tmp279;_tmp27B=(struct _tuple19*)_tmp27A.hd;
_tmp27C=*_tmp27B;_tmp27D=_tmp27C.f1;_tmp27E=_tmp27C.f2;_tmp27F=_tmp27A.tl;
_tmp280=_tmp274.f2;if(_tmp280 == 0)goto _LL11B;_tmp281=*_tmp280;_tmp282=(struct
_tuple17*)_tmp281.hd;_tmp283=*_tmp282;_tmp284=_tmp283.f3;_tmp285=_tmp281.tl;
_LL123: {struct Cyc_Tcpat_Pos_struct _tmp555;struct Cyc_List_List*_tmp554;struct Cyc_Tcpat_Pos_struct*
_tmp553;struct Cyc_Tcpat_Pos_struct*_tmp288=(_tmp553=_region_malloc(r,sizeof(*
_tmp553)),((_tmp553[0]=((_tmp555.tag=0,((_tmp555.f1=_tmp27D,((_tmp555.f2=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rrev)(r,_tmp27E),((_tmp554=_region_malloc(r,sizeof(*_tmp554)),((
_tmp554->hd=dsc,((_tmp554->tl=_tmp284,_tmp554))))))),_tmp555)))))),_tmp553)));
return Cyc_Tcpat_build_desc(r,_tmp27F,(void*)_tmp288,_tmp285);}_LL11B:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp28C=allmrules;struct
Cyc_List_List _tmp28D;struct _tuple0*_tmp28E;struct _tuple0 _tmp28F;void*_tmp290;
void*_tmp291;struct Cyc_List_List*_tmp292;_LL125: if(_tmp28C != 0)goto _LL127;_LL126: {
struct Cyc_Tcpat_Failure_struct _tmp558;struct Cyc_Tcpat_Failure_struct*_tmp557;
return(void*)((_tmp557=_cycalloc(sizeof(*_tmp557)),((_tmp557[0]=((_tmp558.tag=0,((
_tmp558.f1=(void*)dsc,_tmp558)))),_tmp557))));}_LL127: if(_tmp28C == 0)goto _LL124;
_tmp28D=*_tmp28C;_tmp28E=(struct _tuple0*)_tmp28D.hd;_tmp28F=*_tmp28E;_tmp290=
_tmp28F.f1;_tmp291=_tmp28F.f2;_tmp292=_tmp28D.tl;_LL128: return Cyc_Tcpat_match(r,
_tmp290,0,dsc,0,0,_tmp291,_tmp292);_LL124:;}static void*Cyc_Tcpat_match_compile(
struct _RegionHandle*r,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_struct
_tmp55B;struct Cyc_Tcpat_Neg_struct*_tmp55A;return Cyc_Tcpat_or_match(r,(void*)((
_tmp55A=_region_malloc(r,sizeof(*_tmp55A)),((_tmp55A[0]=((_tmp55B.tag=1,((
_tmp55B.f1=Cyc_Tcpat_empty_con_set(r),_tmp55B)))),_tmp55A)))),allmrules);}static
void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp297=
work;struct Cyc_List_List _tmp298;struct _tuple17*_tmp299;struct _tuple17 _tmp29A;
struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29C;struct Cyc_List_List*
_tmp29D;struct Cyc_List_List*_tmp29E;struct Cyc_List_List _tmp29F;struct _tuple17*
_tmp2A0;struct _tuple17 _tmp2A1;struct Cyc_List_List*_tmp2A2;struct Cyc_List_List*
_tmp2A3;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*_tmp2A5;_LL12A: if(
_tmp297 != 0)goto _LL12C;_LL12B: {struct Cyc_Tcpat_Success_struct _tmp55E;struct Cyc_Tcpat_Success_struct*
_tmp55D;return(void*)((_tmp55D=_region_malloc(r,sizeof(*_tmp55D)),((_tmp55D[0]=((
_tmp55E.tag=1,((_tmp55E.f1=(void*)right_hand_side,_tmp55E)))),_tmp55D))));}
_LL12C: if(_tmp297 == 0)goto _LL12E;_tmp298=*_tmp297;_tmp299=(struct _tuple17*)
_tmp298.hd;_tmp29A=*_tmp299;_tmp29B=_tmp29A.f1;if(_tmp29B != 0)goto _LL12E;_tmp29C=
_tmp29A.f2;if(_tmp29C != 0)goto _LL12E;_tmp29D=_tmp29A.f3;if(_tmp29D != 0)goto
_LL12E;_tmp29E=_tmp298.tl;_LL12D: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp29E,right_hand_side,rules);_LL12E: if(_tmp297 == 0)goto _LL129;_tmp29F=*
_tmp297;_tmp2A0=(struct _tuple17*)_tmp29F.hd;_tmp2A1=*_tmp2A0;_tmp2A2=_tmp2A1.f1;
_tmp2A3=_tmp2A1.f2;_tmp2A4=_tmp2A1.f3;_tmp2A5=_tmp29F.tl;_LL12F: if((_tmp2A2 == 0
 || _tmp2A3 == 0) || _tmp2A4 == 0){const char*_tmp561;void*_tmp560;(_tmp560=0,Cyc_Tcutil_impos(((
_tmp561="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp561,sizeof(
char),38))),_tag_dyneither(_tmp560,sizeof(void*),0)));}{struct Cyc_List_List
_tmp2AB;void*_tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AA=(
struct Cyc_List_List*)_tmp2A2;_tmp2AB=*_tmp2AA;_tmp2AC=(void*)_tmp2AB.hd;_tmp2AD=
_tmp2AB.tl;{struct Cyc_List_List _tmp2AF;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*
_tmp2B1;struct Cyc_List_List*_tmp2AE=(struct Cyc_List_List*)_tmp2A3;_tmp2AF=*
_tmp2AE;_tmp2B0=(struct Cyc_List_List*)_tmp2AF.hd;_tmp2B1=_tmp2AF.tl;{struct Cyc_List_List
_tmp2B3;void*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*_tmp2B2=(
struct Cyc_List_List*)_tmp2A4;_tmp2B3=*_tmp2B2;_tmp2B4=(void*)_tmp2B3.hd;_tmp2B5=
_tmp2B3.tl;{struct _tuple17*_tmp562;struct _tuple17*_tmp2B6=(_tmp562=
_region_malloc(r,sizeof(*_tmp562)),((_tmp562->f1=_tmp2AD,((_tmp562->f2=_tmp2B1,((
_tmp562->f3=_tmp2B5,_tmp562)))))));struct Cyc_List_List*_tmp563;return Cyc_Tcpat_match(
r,_tmp2AC,_tmp2B0,_tmp2B4,ctx,((_tmp563=_region_malloc(r,sizeof(*_tmp563)),((
_tmp563->hd=_tmp2B6,((_tmp563->tl=_tmp2A5,_tmp563)))))),right_hand_side,rules);};};};};
_LL129:;}static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,
struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_tmp2B9=dsc;struct Cyc_Set_Set*_tmp2BB;
struct Cyc_List_List*_tmp2BD;_LL131: {struct Cyc_Tcpat_Neg_struct*_tmp2BA=(struct
Cyc_Tcpat_Neg_struct*)_tmp2B9;if(_tmp2BA->tag != 1)goto _LL133;else{_tmp2BB=
_tmp2BA->f1;}}_LL132: {struct Cyc_Tcpat_Neg_struct _tmp566;struct Cyc_Tcpat_Neg_struct*
_tmp565;void*any=(void*)((_tmp565=_region_malloc(r,sizeof(*_tmp565)),((_tmp565[0]=((
_tmp566.tag=1,((_tmp566.f1=Cyc_Tcpat_empty_con_set(r),_tmp566)))),_tmp565))));
struct Cyc_List_List*_tmp2BE=0;{int i=0;for(0;i < pcon->arity;++ i){struct Cyc_List_List*
_tmp567;_tmp2BE=((_tmp567=_region_malloc(r,sizeof(*_tmp567)),((_tmp567->hd=any,((
_tmp567->tl=_tmp2BE,_tmp567))))));}}return _tmp2BE;}_LL133: {struct Cyc_Tcpat_Pos_struct*
_tmp2BC=(struct Cyc_Tcpat_Pos_struct*)_tmp2B9;if(_tmp2BC->tag != 0)goto _LL130;
else{_tmp2BD=_tmp2BC->f2;}}_LL134: return _tmp2BD;_LL130:;}struct _tuple20{struct
_RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*Cyc_Tcpat_getoarg(
struct _tuple20*env,int i){struct _tuple20 _tmp2C3;struct _RegionHandle*_tmp2C4;
struct Cyc_List_List*_tmp2C5;struct _tuple20*_tmp2C2=env;_tmp2C3=*_tmp2C2;_tmp2C4=
_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;{struct Cyc_List_List*_tmp568;return(_tmp568=
_region_malloc(_tmp2C4,sizeof(*_tmp568)),((_tmp568->hd=(void*)(i + 1),((_tmp568->tl=
_tmp2C5,_tmp568)))));};}static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct
_RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple20 _tmp569;struct _tuple20 _tmp2C7=(_tmp569.f1=r,((_tmp569.f2=obj,_tmp569)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple20*,int),struct _tuple20*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp2C7);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp2C9=p;struct Cyc_Tcpat_Con_s*
_tmp2CC;struct Cyc_List_List*_tmp2CD;_LL136: {struct Cyc_Tcpat_Any_struct*_tmp2CA=(
struct Cyc_Tcpat_Any_struct*)_tmp2C9;if(_tmp2CA->tag != 0)goto _LL138;}_LL137:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL138: {struct Cyc_Tcpat_Con_struct*_tmp2CB=(struct Cyc_Tcpat_Con_struct*)
_tmp2C9;if(_tmp2CB->tag != 1)goto _LL135;else{_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CB->f2;}}
_LL139: switch(Cyc_Tcpat_static_match(_tmp2CC,dsc)){case Cyc_Tcpat_Yes: _LL13A: {
struct _tuple19*_tmp56C;struct Cyc_List_List*_tmp56B;struct Cyc_List_List*ctx2=(
_tmp56B=_region_malloc(r,sizeof(*_tmp56B)),((_tmp56B->hd=((_tmp56C=
_region_malloc(r,sizeof(*_tmp56C)),((_tmp56C->f1=_tmp2CC,((_tmp56C->f2=0,_tmp56C)))))),((
_tmp56B->tl=ctx,_tmp56B)))));struct _tuple17*_tmp56D;struct _tuple17*work_frame=(
_tmp56D=_region_malloc(r,sizeof(*_tmp56D)),((_tmp56D->f1=_tmp2CD,((_tmp56D->f2=
Cyc_Tcpat_getoargs(r,_tmp2CC,obj),((_tmp56D->f3=Cyc_Tcpat_getdargs(r,_tmp2CC,dsc),
_tmp56D)))))));struct Cyc_List_List*_tmp56E;struct Cyc_List_List*work2=(_tmp56E=
_region_malloc(r,sizeof(*_tmp56E)),((_tmp56E->hd=work_frame,((_tmp56E->tl=work,
_tmp56E)))));return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case
Cyc_Tcpat_No: _LL13B: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,
work),rules);case Cyc_Tcpat_Maybe: _LL13C: {struct _tuple19*_tmp571;struct Cyc_List_List*
_tmp570;struct Cyc_List_List*ctx2=(_tmp570=_region_malloc(r,sizeof(*_tmp570)),((
_tmp570->hd=((_tmp571=_region_malloc(r,sizeof(*_tmp571)),((_tmp571->f1=_tmp2CC,((
_tmp571->f2=0,_tmp571)))))),((_tmp570->tl=ctx,_tmp570)))));struct _tuple17*
_tmp572;struct _tuple17*work_frame=(_tmp572=_region_malloc(r,sizeof(*_tmp572)),((
_tmp572->f1=_tmp2CD,((_tmp572->f2=Cyc_Tcpat_getoargs(r,_tmp2CC,obj),((_tmp572->f3=
Cyc_Tcpat_getdargs(r,_tmp2CC,dsc),_tmp572)))))));struct Cyc_List_List*_tmp573;
struct Cyc_List_List*work2=(_tmp573=_region_malloc(r,sizeof(*_tmp573)),((_tmp573->hd=
work_frame,((_tmp573->tl=work,_tmp573)))));void*_tmp2D2=Cyc_Tcpat_and_match(r,
ctx2,work2,right_hand_side,rules);void*_tmp2D3=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2CC),work),rules);struct Cyc_Tcpat_IfEq_struct
_tmp576;struct Cyc_Tcpat_IfEq_struct*_tmp575;return(void*)((_tmp575=
_region_malloc(r,sizeof(*_tmp575)),((_tmp575[0]=((_tmp576.tag=2,((_tmp576.f1=obj,((
_tmp576.f2=_tmp2CC,((_tmp576.f3=(void*)_tmp2D2,((_tmp576.f4=(void*)_tmp2D3,
_tmp576)))))))))),_tmp575))));}}_LL135:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp2DA=d;void*_tmp2DC;void*_tmp2DE;void*_tmp2E0;void*_tmp2E1;_LL13F: {struct Cyc_Tcpat_Failure_struct*
_tmp2DB=(struct Cyc_Tcpat_Failure_struct*)_tmp2DA;if(_tmp2DB->tag != 0)goto _LL141;
else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL140: if(!(*exhaust_done)){not_exhaust(r,env1,
_tmp2DC);*exhaust_done=1;}goto _LL13E;_LL141: {struct Cyc_Tcpat_Success_struct*
_tmp2DD=(struct Cyc_Tcpat_Success_struct*)_tmp2DA;if(_tmp2DD->tag != 1)goto _LL143;
else{_tmp2DE=(void*)_tmp2DD->f1;}}_LL142: rhs_appears(env2,_tmp2DE);goto _LL13E;
_LL143: {struct Cyc_Tcpat_IfEq_struct*_tmp2DF=(struct Cyc_Tcpat_IfEq_struct*)
_tmp2DA;if(_tmp2DF->tag != 2)goto _LL13E;else{_tmp2E0=(void*)_tmp2DF->f3;_tmp2E1=(
void*)_tmp2DF->f4;}}_LL144: Cyc_Tcpat_check_exhaust_overlap(r,_tmp2E0,not_exhaust,
env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(r,_tmp2E1,
not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13E;_LL13E:;}struct
_tuple21{void*f1;struct _tuple18*f2;};struct _tuple22{struct _RegionHandle*f1;int*
f2;};static struct _tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple22 _tmp2E3;struct _RegionHandle*_tmp2E4;int*_tmp2E5;struct
_tuple22*_tmp2E2=env;_tmp2E3=*_tmp2E2;_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2E4,swc->pattern);struct _tuple18*_tmp577;
struct _tuple18*rhs=(_tmp577=_region_malloc(_tmp2E4,sizeof(*_tmp577)),((_tmp577->f1=
0,((_tmp577->f2=(swc->pattern)->loc,_tmp577)))));void*sp;if(swc->where_clause != 
0){{struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp579;sp=Cyc_Tcpat_tuple_pat(
_tmp2E4,((_tmp579=_region_malloc(_tmp2E4,sizeof(*_tmp579)),((_tmp579->hd=sp0,((
_tmp579->tl=((_tmp57A=_region_malloc(_tmp2E4,sizeof(*_tmp57A)),((_tmp57A->hd=Cyc_Tcpat_int_pat(
_tmp2E4,*_tmp2E5,0),((_tmp57A->tl=0,_tmp57A)))))),_tmp579)))))),0);}*_tmp2E5=*
_tmp2E5 + 1;}else{struct Cyc_List_List*_tmp584;struct Cyc_Tcpat_Any_struct _tmp583;
struct Cyc_Tcpat_Any_struct*_tmp582;struct Cyc_List_List*_tmp581;sp=Cyc_Tcpat_tuple_pat(
_tmp2E4,((_tmp581=_region_malloc(_tmp2E4,sizeof(*_tmp581)),((_tmp581->hd=sp0,((
_tmp581->tl=((_tmp584=_region_malloc(_tmp2E4,sizeof(*_tmp584)),((_tmp584->hd=(
void*)((_tmp582=_region_malloc(_tmp2E4,sizeof(*_tmp582)),((_tmp582[0]=((_tmp583.tag=
0,_tmp583)),_tmp582)))),((_tmp584->tl=0,_tmp584)))))),_tmp581)))))),0);}{struct
_tuple21*_tmp585;return(_tmp585=_region_malloc(_tmp2E4,sizeof(*_tmp585)),((
_tmp585->f1=sp,((_tmp585->f2=rhs,_tmp585)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";
struct Cyc_Tcpat_Desc2string_struct{char*tag;};struct Cyc_Tcpat_Desc2string_struct
Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp2EF=d;struct Cyc_Tcpat_Con_s*_tmp2F1;
struct Cyc_List_List*_tmp2F2;struct Cyc_Set_Set*_tmp2F4;_LL146: {struct Cyc_Tcpat_Pos_struct*
_tmp2F0=(struct Cyc_Tcpat_Pos_struct*)_tmp2EF;if(_tmp2F0->tag != 0)goto _LL148;
else{_tmp2F1=_tmp2F0->f1;_tmp2F2=_tmp2F0->f2;}}_LL147: {union Cyc_Tcpat_Name_value
_tmp2F5=_tmp2F1->name;struct Cyc_Absyn_Pat*_tmp2F6=_tmp2F1->orig_pat;if(_tmp2F6 == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp2F2))->hd);{void*_tmp2F7=_tmp2F6->r;struct Cyc_Absyn_Vardecl*_tmp2FA;struct
Cyc_Absyn_Vardecl*_tmp2FC;struct Cyc_Absyn_Tvar*_tmp2FE;struct Cyc_Absyn_Vardecl*
_tmp2FF;struct Cyc_Absyn_AggrInfo*_tmp303;struct Cyc_Absyn_AggrInfo _tmp304;union
Cyc_Absyn_AggrInfoU _tmp305;struct Cyc_Absyn_Aggrdecl**_tmp306;struct Cyc_Absyn_Aggrdecl*
_tmp307;struct Cyc_List_List*_tmp308;struct Cyc_Absyn_Datatypefield*_tmp30A;int
_tmp30D;char _tmp30F;struct _dyneither_ptr _tmp311;struct Cyc_Absyn_Enumfield*
_tmp313;struct Cyc_Absyn_Enumfield*_tmp315;struct Cyc_Absyn_Exp*_tmp317;_LL14B: {
struct Cyc_Absyn_Wild_p_struct*_tmp2F8=(struct Cyc_Absyn_Wild_p_struct*)_tmp2F7;
if(_tmp2F8->tag != 0)goto _LL14D;}_LL14C: {const char*_tmp586;return(_tmp586="_",
_tag_dyneither(_tmp586,sizeof(char),2));}_LL14D: {struct Cyc_Absyn_Var_p_struct*
_tmp2F9=(struct Cyc_Absyn_Var_p_struct*)_tmp2F7;if(_tmp2F9->tag != 1)goto _LL14F;
else{_tmp2FA=_tmp2F9->f1;}}_LL14E: return Cyc_Absynpp_qvar2string(_tmp2FA->name);
_LL14F: {struct Cyc_Absyn_Reference_p_struct*_tmp2FB=(struct Cyc_Absyn_Reference_p_struct*)
_tmp2F7;if(_tmp2FB->tag != 2)goto _LL151;else{_tmp2FC=_tmp2FB->f1;}}_LL150: {const
char*_tmp58A;void*_tmp589[1];struct Cyc_String_pa_struct _tmp588;return(struct
_dyneither_ptr)((_tmp588.tag=0,((_tmp588.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FC->name)),((_tmp589[0]=& _tmp588,Cyc_aprintf(((
_tmp58A="*%s",_tag_dyneither(_tmp58A,sizeof(char),4))),_tag_dyneither(_tmp589,
sizeof(void*),1))))))));}_LL151: {struct Cyc_Absyn_TagInt_p_struct*_tmp2FD=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp2F7;if(_tmp2FD->tag != 3)goto _LL153;else{
_tmp2FE=_tmp2FD->f1;_tmp2FF=_tmp2FD->f2;}}_LL152: {const char*_tmp58F;void*
_tmp58E[2];struct Cyc_String_pa_struct _tmp58D;struct Cyc_String_pa_struct _tmp58C;
return(struct _dyneither_ptr)((_tmp58C.tag=0,((_tmp58C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2FE->name),((_tmp58D.tag=0,((_tmp58D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FF->name)),((
_tmp58E[0]=& _tmp58D,((_tmp58E[1]=& _tmp58C,Cyc_aprintf(((_tmp58F="%s<`%s>",
_tag_dyneither(_tmp58F,sizeof(char),8))),_tag_dyneither(_tmp58E,sizeof(void*),2))))))))))))));}
_LL153: {struct Cyc_Absyn_Tuple_p_struct*_tmp300=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp2F7;if(_tmp300->tag != 4)goto _LL155;}_LL154: {const char*_tmp593;void*_tmp592[
1];struct Cyc_String_pa_struct _tmp591;return(struct _dyneither_ptr)((_tmp591.tag=0,((
_tmp591.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2F2)),((_tmp592[0]=& _tmp591,Cyc_aprintf(((_tmp593="$(%s)",_tag_dyneither(
_tmp593,sizeof(char),6))),_tag_dyneither(_tmp592,sizeof(void*),1))))))));}_LL155: {
struct Cyc_Absyn_Pointer_p_struct*_tmp301=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp2F7;if(_tmp301->tag != 5)goto _LL157;}_LL156: {const char*_tmp597;void*_tmp596[
1];struct Cyc_String_pa_struct _tmp595;return(struct _dyneither_ptr)((_tmp595.tag=0,((
_tmp595.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,
_tmp2F2)),((_tmp596[0]=& _tmp595,Cyc_aprintf(((_tmp597="&%s",_tag_dyneither(
_tmp597,sizeof(char),4))),_tag_dyneither(_tmp596,sizeof(void*),1))))))));}_LL157: {
struct Cyc_Absyn_Aggr_p_struct*_tmp302=(struct Cyc_Absyn_Aggr_p_struct*)_tmp2F7;
if(_tmp302->tag != 6)goto _LL159;else{_tmp303=_tmp302->f1;if(_tmp303 == 0)goto
_LL159;_tmp304=*_tmp303;_tmp305=_tmp304.aggr_info;if((_tmp305.KnownAggr).tag != 2)
goto _LL159;_tmp306=(struct Cyc_Absyn_Aggrdecl**)(_tmp305.KnownAggr).val;_tmp307=*
_tmp306;_tmp308=_tmp302->f3;}}_LL158: if(_tmp307->kind == Cyc_Absyn_UnionA){struct
Cyc_List_List*_tmp326=_tmp308;struct Cyc_List_List _tmp327;struct _tuple14*_tmp328;
struct _tuple14 _tmp329;struct Cyc_List_List*_tmp32A;struct Cyc_List_List _tmp32B;
void*_tmp32C;struct _dyneither_ptr*_tmp32E;_LL16C: if(_tmp326 == 0)goto _LL16E;
_tmp327=*_tmp326;_tmp328=(struct _tuple14*)_tmp327.hd;_tmp329=*_tmp328;_tmp32A=
_tmp329.f1;if(_tmp32A == 0)goto _LL16E;_tmp32B=*_tmp32A;_tmp32C=(void*)_tmp32B.hd;{
struct Cyc_Absyn_FieldName_struct*_tmp32D=(struct Cyc_Absyn_FieldName_struct*)
_tmp32C;if(_tmp32D->tag != 1)goto _LL16E;else{_tmp32E=_tmp32D->f1;}};_LL16D: {
const char*_tmp59D;void*_tmp59C[3];struct Cyc_String_pa_struct _tmp59B;struct Cyc_String_pa_struct
_tmp59A;struct Cyc_String_pa_struct _tmp599;return(struct _dyneither_ptr)((_tmp599.tag=
0,((_tmp599.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F2)),((_tmp59A.tag=0,((_tmp59A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp32E),((_tmp59B.tag=0,((_tmp59B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp307->name)),((_tmp59C[0]=&
_tmp59B,((_tmp59C[1]=& _tmp59A,((_tmp59C[2]=& _tmp599,Cyc_aprintf(((_tmp59D="%s{.%s = %s}",
_tag_dyneither(_tmp59D,sizeof(char),13))),_tag_dyneither(_tmp59C,sizeof(void*),3))))))))))))))))))));}
_LL16E:;_LL16F: goto _LL16B;_LL16B:;}{const char*_tmp5A2;void*_tmp5A1[2];struct Cyc_String_pa_struct
_tmp5A0;struct Cyc_String_pa_struct _tmp59F;return(struct _dyneither_ptr)((_tmp59F.tag=
0,((_tmp59F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2F2)),((_tmp5A0.tag=0,((_tmp5A0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp307->name)),((_tmp5A1[0]=& _tmp5A0,((
_tmp5A1[1]=& _tmp59F,Cyc_aprintf(((_tmp5A2="%s{%s}",_tag_dyneither(_tmp5A2,
sizeof(char),7))),_tag_dyneither(_tmp5A1,sizeof(void*),2))))))))))))));};_LL159: {
struct Cyc_Absyn_Datatype_p_struct*_tmp309=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp2F7;if(_tmp309->tag != 7)goto _LL15B;else{_tmp30A=_tmp309->f2;}}_LL15A: if(
_tmp2F2 == 0){const char*_tmp5A6;void*_tmp5A5[1];struct Cyc_String_pa_struct _tmp5A4;
return(struct _dyneither_ptr)((_tmp5A4.tag=0,((_tmp5A4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp30A->name)),((_tmp5A5[0]=&
_tmp5A4,Cyc_aprintf(((_tmp5A6="%s",_tag_dyneither(_tmp5A6,sizeof(char),3))),
_tag_dyneither(_tmp5A5,sizeof(void*),1))))))));}else{const char*_tmp5AB;void*
_tmp5AA[2];struct Cyc_String_pa_struct _tmp5A9;struct Cyc_String_pa_struct _tmp5A8;
return(struct _dyneither_ptr)((_tmp5A8.tag=0,((_tmp5A8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2F2)),((_tmp5A9.tag=0,((
_tmp5A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp30A->name)),((_tmp5AA[0]=& _tmp5A9,((_tmp5AA[1]=& _tmp5A8,Cyc_aprintf(((
_tmp5AB="%s(%s)",_tag_dyneither(_tmp5AB,sizeof(char),7))),_tag_dyneither(_tmp5AA,
sizeof(void*),2))))))))))))));}_LL15B: {struct Cyc_Absyn_Null_p_struct*_tmp30B=(
struct Cyc_Absyn_Null_p_struct*)_tmp2F7;if(_tmp30B->tag != 8)goto _LL15D;}_LL15C: {
const char*_tmp5AC;return(_tmp5AC="NULL",_tag_dyneither(_tmp5AC,sizeof(char),5));}
_LL15D: {struct Cyc_Absyn_Int_p_struct*_tmp30C=(struct Cyc_Absyn_Int_p_struct*)
_tmp2F7;if(_tmp30C->tag != 9)goto _LL15F;else{_tmp30D=_tmp30C->f2;}}_LL15E: {const
char*_tmp5B0;void*_tmp5AF[1];struct Cyc_Int_pa_struct _tmp5AE;return(struct
_dyneither_ptr)((_tmp5AE.tag=1,((_tmp5AE.f1=(unsigned long)_tmp30D,((_tmp5AF[0]=&
_tmp5AE,Cyc_aprintf(((_tmp5B0="%d",_tag_dyneither(_tmp5B0,sizeof(char),3))),
_tag_dyneither(_tmp5AF,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_Char_p_struct*
_tmp30E=(struct Cyc_Absyn_Char_p_struct*)_tmp2F7;if(_tmp30E->tag != 10)goto _LL161;
else{_tmp30F=_tmp30E->f1;}}_LL160: {const char*_tmp5B4;void*_tmp5B3[1];struct Cyc_Int_pa_struct
_tmp5B2;return(struct _dyneither_ptr)((_tmp5B2.tag=1,((_tmp5B2.f1=(unsigned long)((
int)_tmp30F),((_tmp5B3[0]=& _tmp5B2,Cyc_aprintf(((_tmp5B4="%d",_tag_dyneither(
_tmp5B4,sizeof(char),3))),_tag_dyneither(_tmp5B3,sizeof(void*),1))))))));}_LL161: {
struct Cyc_Absyn_Float_p_struct*_tmp310=(struct Cyc_Absyn_Float_p_struct*)_tmp2F7;
if(_tmp310->tag != 11)goto _LL163;else{_tmp311=_tmp310->f1;}}_LL162: return _tmp311;
_LL163: {struct Cyc_Absyn_Enum_p_struct*_tmp312=(struct Cyc_Absyn_Enum_p_struct*)
_tmp2F7;if(_tmp312->tag != 12)goto _LL165;else{_tmp313=_tmp312->f2;}}_LL164:
_tmp315=_tmp313;goto _LL166;_LL165: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp314=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp2F7;if(_tmp314->tag != 13)goto _LL167;else{
_tmp315=_tmp314->f2;}}_LL166: return Cyc_Absynpp_qvar2string(_tmp315->name);_LL167: {
struct Cyc_Absyn_Exp_p_struct*_tmp316=(struct Cyc_Absyn_Exp_p_struct*)_tmp2F7;if(
_tmp316->tag != 16)goto _LL169;else{_tmp317=_tmp316->f1;}}_LL168: return Cyc_Absynpp_exp2string(
_tmp317);_LL169:;_LL16A:(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL14A:;};}
_LL148: {struct Cyc_Tcpat_Neg_struct*_tmp2F3=(struct Cyc_Tcpat_Neg_struct*)_tmp2EF;
if(_tmp2F3->tag != 1)goto _LL145;else{_tmp2F4=_tmp2F3->f1;}}_LL149: if(((int(*)(
struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2F4)){const char*_tmp5B5;return(
_tmp5B5="_",_tag_dyneither(_tmp5B5,sizeof(char),2));}{struct Cyc_Tcpat_Con_s*
_tmp347=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2F4);
struct Cyc_Absyn_Pat*_tmp348=_tmp347->orig_pat;if(_tmp348 == 0)(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);{void*_tmp349=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp348->topt))->v);enum Cyc_Absyn_Size_of _tmp34B;struct Cyc_Absyn_PtrInfo
_tmp34E;struct Cyc_Absyn_PtrAtts _tmp34F;struct Cyc_Absyn_DatatypeInfo _tmp351;union
Cyc_Absyn_DatatypeInfoU _tmp352;struct Cyc_Absyn_Datatypedecl**_tmp353;struct Cyc_Absyn_Datatypedecl*
_tmp354;struct Cyc_Absyn_AggrInfo _tmp356;union Cyc_Absyn_AggrInfoU _tmp357;struct
Cyc_Absyn_Aggrdecl**_tmp358;struct Cyc_Absyn_Aggrdecl*_tmp359;_LL171: {struct Cyc_Absyn_IntType_struct*
_tmp34A=(struct Cyc_Absyn_IntType_struct*)_tmp349;if(_tmp34A->tag != 6)goto _LL173;
else{_tmp34B=_tmp34A->f2;if(_tmp34B != Cyc_Absyn_Char_sz)goto _LL173;}}_LL172:{int
i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp35A=Cyc_Tcpat_char_con(r,(char)i,(
struct Cyc_Absyn_Pat*)_tmp348);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2F4,_tmp35A)){const char*_tmp5B9;void*_tmp5B8[1];struct
Cyc_Int_pa_struct _tmp5B7;return(struct _dyneither_ptr)((_tmp5B7.tag=1,((_tmp5B7.f1=(
unsigned long)i,((_tmp5B8[0]=& _tmp5B7,Cyc_aprintf(((_tmp5B9="%d",_tag_dyneither(
_tmp5B9,sizeof(char),3))),_tag_dyneither(_tmp5B8,sizeof(void*),1))))))));}}}(int)
_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL173: {struct Cyc_Absyn_IntType_struct*
_tmp34C=(struct Cyc_Absyn_IntType_struct*)_tmp349;if(_tmp34C->tag != 6)goto _LL175;}
_LL174:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp35E=Cyc_Tcpat_int_con(
r,(int)i,_tmp348);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp2F4,_tmp35E)){const char*_tmp5BD;void*_tmp5BC[1];struct Cyc_Int_pa_struct
_tmp5BB;return(struct _dyneither_ptr)((_tmp5BB.tag=1,((_tmp5BB.f1=(unsigned long)((
int)i),((_tmp5BC[0]=& _tmp5BB,Cyc_aprintf(((_tmp5BD="%d",_tag_dyneither(_tmp5BD,
sizeof(char),3))),_tag_dyneither(_tmp5BC,sizeof(void*),1))))))));}}}(int)_throw((
void*)& Cyc_Tcpat_Desc2string_val);_LL175: {struct Cyc_Absyn_PointerType_struct*
_tmp34D=(struct Cyc_Absyn_PointerType_struct*)_tmp349;if(_tmp34D->tag != 5)goto
_LL177;else{_tmp34E=_tmp34D->f1;_tmp34F=_tmp34E.ptr_atts;}}_LL176: {union Cyc_Absyn_Constraint*
_tmp362=_tmp34F.nullable;int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp362);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*
s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F4,Cyc_Tcpat_null_con(r,(
struct Cyc_Absyn_Pat*)_tmp348))){const char*_tmp5BE;return(_tmp5BE="NULL",
_tag_dyneither(_tmp5BE,sizeof(char),5));}}{const char*_tmp5BF;return(_tmp5BF="&_",
_tag_dyneither(_tmp5BF,sizeof(char),3));};}_LL177: {struct Cyc_Absyn_DatatypeType_struct*
_tmp350=(struct Cyc_Absyn_DatatypeType_struct*)_tmp349;if(_tmp350->tag != 3)goto
_LL179;else{_tmp351=_tmp350->f1;_tmp352=_tmp351.datatype_info;if((_tmp352.KnownDatatype).tag
!= 2)goto _LL179;_tmp353=(struct Cyc_Absyn_Datatypedecl**)(_tmp352.KnownDatatype).val;
_tmp354=*_tmp353;}}_LL178: if(_tmp354->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp365=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp354->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp365);for(0;(unsigned int)_tmp365;_tmp365=_tmp365->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp365->hd)->name).f2;struct Cyc_List_List*
_tmp366=((struct Cyc_Absyn_Datatypefield*)_tmp365->hd)->typs;struct Cyc_Tcpat_Con_s*
_tmp5C0;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F4,(struct Cyc_Tcpat_Con_s*)((_tmp5C0=_cycalloc(sizeof(*_tmp5C0)),((_tmp5C0->name=
Cyc_Tcpat_Name_v(n),((_tmp5C0->arity=0,((_tmp5C0->span=0,((_tmp5C0->orig_pat=
_tmp348,_tmp5C0)))))))))))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp366)== 0)return n;else{const char*_tmp5C4;void*_tmp5C3[1];struct Cyc_String_pa_struct
_tmp5C2;return(struct _dyneither_ptr)((_tmp5C2.tag=0,((_tmp5C2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5C3[0]=& _tmp5C2,Cyc_aprintf(((
_tmp5C4="%s(...)",_tag_dyneither(_tmp5C4,sizeof(char),8))),_tag_dyneither(
_tmp5C3,sizeof(void*),1))))))));}}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};
_LL179:{struct Cyc_Absyn_AggrType_struct*_tmp355=(struct Cyc_Absyn_AggrType_struct*)
_tmp349;if(_tmp355->tag != 12)goto _LL17B;else{_tmp356=_tmp355->f1;_tmp357=_tmp356.aggr_info;
if((_tmp357.KnownAggr).tag != 2)goto _LL17B;_tmp358=(struct Cyc_Absyn_Aggrdecl**)(
_tmp357.KnownAggr).val;_tmp359=*_tmp358;}}if(!(_tmp359->kind == Cyc_Absyn_UnionA))
goto _LL17B;_LL17A: {struct Cyc_List_List*_tmp36B=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp359->impl))->fields;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp36B);struct _tuple1 _tmp36D;struct _dyneither_ptr*_tmp36E;struct _dyneither_ptr
_tmp36F;struct _tuple1*_tmp36C=_tmp359->name;_tmp36D=*_tmp36C;_tmp36E=_tmp36D.f2;
_tmp36F=*_tmp36E;for(0;(unsigned int)_tmp36B;_tmp36B=_tmp36B->tl){struct
_dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp36B->hd)->name;struct Cyc_Tcpat_Con_s*
_tmp5C5;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2F4,(struct Cyc_Tcpat_Con_s*)((_tmp5C5=_cycalloc(sizeof(*_tmp5C5)),((_tmp5C5->name=
Cyc_Tcpat_Name_v(n),((_tmp5C5->arity=0,((_tmp5C5->span=0,((_tmp5C5->orig_pat=
_tmp348,_tmp5C5)))))))))))){const char*_tmp5CA;void*_tmp5C9[2];struct Cyc_String_pa_struct
_tmp5C8;struct Cyc_String_pa_struct _tmp5C7;return(struct _dyneither_ptr)((_tmp5C7.tag=
0,((_tmp5C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5C8.tag=0,((
_tmp5C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36F),((_tmp5C9[0]=&
_tmp5C8,((_tmp5C9[1]=& _tmp5C7,Cyc_aprintf(((_tmp5CA="%s{.%s = _}",_tag_dyneither(
_tmp5CA,sizeof(char),12))),_tag_dyneither(_tmp5C9,sizeof(void*),2))))))))))))));}}(
int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL17B:;_LL17C:(int)_throw((void*)&
Cyc_Tcpat_Desc2string_val);_LL170:;};};_LL145:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(
struct _RegionHandle*r,void*d){struct _dyneither_ptr*_tmp5CB;return(_tmp5CB=
_cycalloc(sizeof(*_tmp5CB)),((_tmp5CB[0]=Cyc_Tcpat_desc2string(r,d),_tmp5CB)));}
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp376=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",
sizeof(char),2);{struct Cyc_List_List*_tmp377=_tmp376;for(0;_tmp377 != 0;_tmp377=((
struct Cyc_List_List*)_check_null(_tmp377))->tl){if(_tmp377->tl != 0){{struct Cyc_List_List*
_tmp5CC;_tmp377->tl=(struct Cyc_List_List*)((_tmp5CC=_cycalloc(sizeof(*_tmp5CC)),((
_tmp5CC->hd=comma,((_tmp5CC->tl=_tmp377->tl,_tmp5CC))))));}_tmp377=_tmp377->tl;}}}
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp376);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp37A;_push_handler(& _tmp37A);{int _tmp37C=0;if(setjmp(_tmp37A.handler))
_tmp37C=1;if(!_tmp37C){{struct _dyneither_ptr _tmp37D=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5D0;void*_tmp5CF[1];struct Cyc_String_pa_struct _tmp5CE;(_tmp5CE.tag=
0,((_tmp5CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37D),((_tmp5CF[0]=&
_tmp5CE,Cyc_Tcutil_terr(loc,((_tmp5D0="patterns may not be exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5D0,sizeof(char),53))),_tag_dyneither(_tmp5CF,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp37B=(void*)_exn_thrown;void*_tmp382=_tmp37B;_LL17E: {
struct Cyc_Tcpat_Desc2string_struct*_tmp383=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp382;if(_tmp383->tag != Cyc_Tcpat_Desc2string)goto _LL180;}_LL17F:{const char*
_tmp5D3;void*_tmp5D2;(_tmp5D2=0,Cyc_Tcutil_terr(loc,((_tmp5D3="patterns may not be exhaustive.",
_tag_dyneither(_tmp5D3,sizeof(char),32))),_tag_dyneither(_tmp5D2,sizeof(void*),0)));}
goto _LL17D;_LL180:;_LL181:(void)_throw(_tmp382);_LL17D:;}};}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple18*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp386=Cyc_Tcenv_get_fnrgn(te);int _tmp387=0;struct _tuple22
_tmp5D4;struct _tuple22 _tmp388=(_tmp5D4.f1=_tmp386,((_tmp5D4.f2=& _tmp387,_tmp5D4)));
struct Cyc_List_List*_tmp389=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _tuple22*,struct Cyc_Absyn_Switch_clause*),struct
_tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp386,Cyc_Tcpat_get_match,&
_tmp388,swcs);void*_tmp38A=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp386,_tmp389);int _tmp38B=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple18*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp386,_tmp38A,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp38B);for(0;_tmp389 != 0;_tmp389=_tmp389->tl){
struct _tuple21 _tmp38D;struct _tuple18*_tmp38E;struct _tuple18 _tmp38F;int _tmp390;
struct Cyc_Position_Segment*_tmp391;struct _tuple21*_tmp38C=(struct _tuple21*)
_tmp389->hd;_tmp38D=*_tmp38C;_tmp38E=_tmp38D.f2;_tmp38F=*_tmp38E;_tmp390=_tmp38F.f1;
_tmp391=_tmp38F.f2;if(!_tmp390){{const char*_tmp5D7;void*_tmp5D6;(_tmp5D6=0,Cyc_Tcutil_terr(
_tmp391,((_tmp5D7="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5D7,sizeof(char),74))),_tag_dyneither(_tmp5D6,sizeof(void*),0)));}
break;}}}struct _tuple23{struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _RegionHandle*r,struct _tuple23*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp395;_push_handler(& _tmp395);{int _tmp397=0;if(setjmp(_tmp395.handler))
_tmp397=1;if(!_tmp397){{struct _dyneither_ptr _tmp398=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5DB;void*_tmp5DA[1];struct Cyc_String_pa_struct _tmp5D9;(_tmp5D9.tag=
0,((_tmp5D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp398),((_tmp5DA[0]=&
_tmp5D9,Cyc_Tcutil_warn((*pr).f1,((_tmp5DB="pattern not exhaustive.\n\tmissing case for %s",
_tag_dyneither(_tmp5DB,sizeof(char),45))),_tag_dyneither(_tmp5DA,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmp396=(void*)_exn_thrown;void*_tmp39D=_tmp396;_LL183: {
struct Cyc_Tcpat_Desc2string_struct*_tmp39E=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp39D;if(_tmp39E->tag != Cyc_Tcpat_Desc2string)goto _LL185;}_LL184:{const char*
_tmp5DE;void*_tmp5DD;(_tmp5DD=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5DE="pattern not exhaustive.",
_tag_dyneither(_tmp5DE,sizeof(char),24))),_tag_dyneither(_tmp5DD,sizeof(void*),0)));}
goto _LL182;_LL185:;_LL186:(void)_throw(_tmp39D);_LL182:;}};};}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple24{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle*_tmp3A1=Cyc_Tcenv_get_fnrgn(te);struct _tuple24*_tmp5E1;
struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp3A2=(_tmp5E0=_region_malloc(
_tmp3A1,sizeof(*_tmp5E0)),((_tmp5E0->hd=((_tmp5E1=_region_malloc(_tmp3A1,sizeof(*
_tmp5E1)),((_tmp5E1->f1=Cyc_Tcpat_compile_pat(_tmp3A1,p),((_tmp5E1->f2=0,_tmp5E1)))))),((
_tmp5E0->tl=0,_tmp5E0)))));void*_tmp3A3=((void*(*)(struct _RegionHandle*r,struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp3A1,_tmp3A2);struct
_tuple23 _tmp5E2;struct _tuple23 _tmp3A4=(_tmp5E2.f1=loc,((_tmp5E2.f2=1,_tmp5E2)));
int _tmp3A5=0;((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct
_RegionHandle*,struct _tuple23*,void*),struct _tuple23*env1,void(*rhs_appears)(int,
int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3A1,_tmp3A3,
Cyc_Tcpat_not_exhaust_warn,& _tmp3A4,Cyc_Tcpat_dummy_fn,0,& _tmp3A5);return _tmp3A4.f2;}
static struct _tuple21*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple18*_tmp5E3;
struct _tuple18*rhs=(_tmp5E3=_region_malloc(r,sizeof(*_tmp5E3)),((_tmp5E3->f1=0,((
_tmp5E3->f2=(swc->pattern)->loc,_tmp5E3)))));struct _tuple21*_tmp5E4;return(
_tmp5E4=_region_malloc(r,sizeof(*_tmp5E4)),((_tmp5E4->f1=sp0,((_tmp5E4->f2=rhs,
_tmp5E4)))));}static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,struct
Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){struct _RegionHandle*
_tmp3AB=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp3AC=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple21*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3AB,Cyc_Tcpat_get_match2,
_tmp3AB,swcs);void*_tmp3AD=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp3AB,_tmp3AC);int _tmp3AE=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple18*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3AB,_tmp3AD,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp3AE);for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){
struct _tuple21 _tmp3B0;struct _tuple18*_tmp3B1;struct _tuple18 _tmp3B2;int _tmp3B3;
struct Cyc_Position_Segment*_tmp3B4;struct _tuple21*_tmp3AF=(struct _tuple21*)
_tmp3AC->hd;_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3B0.f2;_tmp3B2=*_tmp3B1;_tmp3B3=_tmp3B2.f1;
_tmp3B4=_tmp3B2.f2;if(!_tmp3B3){{const char*_tmp5E7;void*_tmp5E6;(_tmp5E6=0,Cyc_Tcutil_terr(
_tmp3B4,((_tmp5E7="redundant pattern (check for misspelled constructors in earlier patterns)",
_tag_dyneither(_tmp5E7,sizeof(char),74))),_tag_dyneither(_tmp5E6,sizeof(void*),0)));}
break;}}}
