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
tag;struct _dyneither_ptr f1;};typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};typedef
unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
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
Cyc_List_list_t;typedef struct Cyc_List_List*Cyc_List_List_t;struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};typedef
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};typedef struct _dyneither_ptr*
Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_t;typedef struct
_dyneither_ptr*Cyc_Absyn_tvarname_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};typedef struct _tuple0*
Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;typedef struct _tuple0*
Cyc_Absyn_typedef_name_t;typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};
struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct
Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_struct{
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct _dyneither_ptr*f1;};extern
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};struct
Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
unsigned int);extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_exn_typ();void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp(unsigned int,unsigned int);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,unsigned int loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_no_regions;
struct Cyc_PP_Ppstate;typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int
add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;
int print_all_kinds;int print_all_effects;int print_using_stmts;int
print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;typedef struct Cyc_Set_Set*
Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
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
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*
old_fenv,struct Cyc_Absyn_Fndecl*new_fn);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const
struct Cyc_Tcenv_CList*f3;};const struct _tuple10*const Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_dyneither_ptr*,struct Cyc_Absyn_Stmt*);int Cyc_Tcenv_all_labels_resolved(struct
Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,
unsigned int,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*
name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);
struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**
oldtv,unsigned int loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int
resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
unsigned int,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,
unsigned int,void*rgn);void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*
te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);
void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_is_pointer_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple11*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);int
Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_extract_const_from_typedef(
unsigned int,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void Cyc_Tcutil_check_no_qual(unsigned int loc,
void*t);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
struct _tuple12{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple12*eq;int
isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct _tuple13{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};typedef struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcpat_result_t;struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(unsigned int,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 
1,Cyc_CfFlowInfo_AllIL  = 2};typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple14{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple14 val;};struct
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
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple15{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15
val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct
_union_FlowInfo_ReachableFL ReachableFL;};typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};typedef struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};
struct Cyc_CfFlowInfo_RefCntRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};typedef void*Cyc_CfFlowInfo_killrgn_t;void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);static void Cyc_Tcstmt_simplify_unused_result_exp(
struct Cyc_Absyn_Exp*e){void*_tmp0=e->r;struct Cyc_Absyn_Exp*_tmp2;enum Cyc_Absyn_Incrementor
_tmp3;struct Cyc_Absyn_Exp*_tmp5;enum Cyc_Absyn_Incrementor _tmp6;_LL1: {struct Cyc_Absyn_Increment_e_struct*
_tmp1=(struct Cyc_Absyn_Increment_e_struct*)_tmp0;if(_tmp1->tag != 5)goto _LL3;
else{_tmp2=_tmp1->f1;_tmp3=_tmp1->f2;if(_tmp3 != Cyc_Absyn_PostInc)goto _LL3;}}
_LL2:{struct Cyc_Absyn_Increment_e_struct _tmp182;struct Cyc_Absyn_Increment_e_struct*
_tmp181;e->r=(void*)((_tmp181=_cycalloc(sizeof(*_tmp181)),((_tmp181[0]=((_tmp182.tag=
5,((_tmp182.f1=_tmp2,((_tmp182.f2=Cyc_Absyn_PreInc,_tmp182)))))),_tmp181))));}
goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_struct*_tmp4=(struct Cyc_Absyn_Increment_e_struct*)
_tmp0;if(_tmp4->tag != 5)goto _LL5;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;if(_tmp6 != 
Cyc_Absyn_PostDec)goto _LL5;}}_LL4:{struct Cyc_Absyn_Increment_e_struct _tmp185;
struct Cyc_Absyn_Increment_e_struct*_tmp184;e->r=(void*)((_tmp184=_cycalloc(
sizeof(*_tmp184)),((_tmp184[0]=((_tmp185.tag=5,((_tmp185.f1=_tmp5,((_tmp185.f2=
Cyc_Absyn_PreDec,_tmp185)))))),_tmp184))));}goto _LL0;_LL5:;_LL6: goto _LL0;_LL0:;}
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));s->try_depth=
Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(unsigned int loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct _tuple13*_tmpC;struct Cyc_List_List*
_tmpD;struct Cyc_Tcpat_TcPatResult _tmpB=pat_res;_tmpC=_tmpB.tvars_and_bounds_opt;
_tmpD=_tmpB.patvars;{struct Cyc_List_List*_tmpE=_tmpC == 0?0:(*_tmpC).f1;struct Cyc_List_List*
_tmpF=_tmpC == 0?0:(*_tmpC).f2;struct _RegionHandle _tmp10=_new_region("r");struct
_RegionHandle*r=& _tmp10;_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
r,loc,te,_tmpE);te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpF,loc);if(
new_block)te2=Cyc_Tcenv_new_block(r,loc,te2);{struct Cyc_Core_Opt*_tmp186;struct
Cyc_Core_Opt*_tmp11=(_tmp186=_cycalloc(sizeof(*_tmp186)),((_tmp186->v=Cyc_Tcenv_curr_rgn(
te2),_tmp186)));{struct Cyc_List_List*_tmp12=_tmpD;for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp12->hd);((
struct Cyc_Absyn_Vardecl*)_tmp12->hd)->rgn=_tmp11;}}if(where_opt != 0){const char*
_tmp187;Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)where_opt,((_tmp187="switch clause guard",
_tag_dyneither(_tmp187,sizeof(char),20))));}if(_tmpD != 0)te2=Cyc_Tcenv_set_encloser(
r,te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);};}if(_tmpD != 0)Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));;_pop_region(r);};}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*
const x){if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{struct Cyc_List_List*
_tmp188;return(_tmp188=_region_malloc(r,sizeof(*_tmp188)),((_tmp188->hd=(void*)f(
env,(void*)x->hd),((_tmp188->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp188)))));}}
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s0,void*r1,void*r2,int new_block){struct _RegionHandle _tmp16=_new_region("r");
struct _RegionHandle*r=& _tmp16;_push_region(r);{struct _tuple11*tk=0;struct Cyc_Tcenv_Tenv*
_tmp17=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);Cyc_Tcstmt_tcStmt(
_tmp17,s0,new_block);if(tk != 0)((*((struct _tuple11*)_check_null(tk))).f1)->kind=(*((
struct _tuple11*)_check_null(tk))).f2;};_pop_region(r);}void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){Cyc_Tcstmt_decorate_stmt(
te,s0);{void*_tmp18=s0->r;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Stmt*_tmp1D;
struct Cyc_Absyn_Stmt*_tmp1E;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*
_tmp22;struct Cyc_Absyn_Stmt*_tmp23;struct Cyc_Absyn_Stmt*_tmp24;struct _tuple9
_tmp26;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Stmt*
_tmp29;struct Cyc_Absyn_Exp*_tmp2B;struct _tuple9 _tmp2C;struct Cyc_Absyn_Exp*_tmp2D;
struct Cyc_Absyn_Stmt*_tmp2E;struct _tuple9 _tmp2F;struct Cyc_Absyn_Exp*_tmp30;
struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_Absyn_Stmt*
_tmp34;struct _tuple9 _tmp35;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Stmt*
_tmp37;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Stmt**_tmp3A;struct Cyc_Absyn_Stmt*
_tmp3C;struct Cyc_Absyn_Stmt**_tmp3D;struct _dyneither_ptr*_tmp3F;struct Cyc_Absyn_Stmt*
_tmp40;struct Cyc_Absyn_Stmt**_tmp41;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_Switch_clause**
_tmp44;struct Cyc_Absyn_Switch_clause***_tmp45;struct _dyneither_ptr*_tmp47;struct
Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_List_List*_tmp4B;
struct Cyc_Absyn_Stmt*_tmp4D;struct Cyc_List_List*_tmp4E;struct Cyc_Absyn_Decl*
_tmp50;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Exp*_tmp53;_LL8: {struct Cyc_Absyn_Skip_s_struct*
_tmp19=(struct Cyc_Absyn_Skip_s_struct*)_tmp18;if(_tmp19->tag != 0)goto _LLA;}_LL9:
return;_LLA: {struct Cyc_Absyn_Exp_s_struct*_tmp1A=(struct Cyc_Absyn_Exp_s_struct*)
_tmp18;if(_tmp1A->tag != 1)goto _LLC;else{_tmp1B=_tmp1A->f1;}}_LLB: Cyc_Tcexp_tcExp(
te,0,_tmp1B);if(!Cyc_Tcexp_in_stmt_exp)Cyc_Tcstmt_simplify_unused_result_exp(
_tmp1B);return;_LLC: {struct Cyc_Absyn_Seq_s_struct*_tmp1C=(struct Cyc_Absyn_Seq_s_struct*)
_tmp18;if(_tmp1C->tag != 2)goto _LLE;else{_tmp1D=_tmp1C->f1;_tmp1E=_tmp1C->f2;}}
_LLD: {struct _RegionHandle _tmp54=_new_region("r");struct _RegionHandle*r=& _tmp54;
_push_region(r);{struct Cyc_Tcenv_Stmt_j_struct _tmp18B;struct Cyc_Tcenv_Stmt_j_struct*
_tmp18A;struct Cyc_Tcenv_Tenv*_tmp55=Cyc_Tcenv_set_next(r,te,(void*)((_tmp18A=
_cycalloc(sizeof(*_tmp18A)),((_tmp18A[0]=((_tmp18B.tag=3,((_tmp18B.f1=_tmp1E,
_tmp18B)))),_tmp18A)))));Cyc_Tcstmt_tcStmt(_tmp55,_tmp1D,1);}Cyc_Tcstmt_tcStmt(
te,_tmp1E,1);_npop_handler(0);return;;_pop_region(r);}_LLE: {struct Cyc_Absyn_Return_s_struct*
_tmp1F=(struct Cyc_Absyn_Return_s_struct*)_tmp18;if(_tmp1F->tag != 3)goto _LL10;
else{_tmp20=_tmp1F->f1;}}_LLF: {void*t=Cyc_Tcenv_return_typ(te);if(_tmp20 == 0){
void*_tmp58=Cyc_Tcutil_compress(t);_LL2B: {struct Cyc_Absyn_VoidType_struct*
_tmp59=(struct Cyc_Absyn_VoidType_struct*)_tmp58;if(_tmp59->tag != 0)goto _LL2D;}
_LL2C: goto _LL2A;_LL2D: {struct Cyc_Absyn_FloatType_struct*_tmp5A=(struct Cyc_Absyn_FloatType_struct*)
_tmp58;if(_tmp5A->tag != 7)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp5B=(struct Cyc_Absyn_IntType_struct*)_tmp58;if(_tmp5B->tag != 6)goto _LL31;}
_LL30:{const char*_tmp18F;void*_tmp18E[1];struct Cyc_String_pa_struct _tmp18D;(
_tmp18D.tag=0,((_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp18E[0]=& _tmp18D,Cyc_Tcutil_warn(s0->loc,((_tmp18F="should return a value of type %s",
_tag_dyneither(_tmp18F,sizeof(char),33))),_tag_dyneither(_tmp18E,sizeof(void*),1)))))));}
goto _LL2A;_LL31:;_LL32:{const char*_tmp193;void*_tmp192[1];struct Cyc_String_pa_struct
_tmp191;(_tmp191.tag=0,((_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp192[0]=& _tmp191,Cyc_Tcutil_terr(s0->loc,((
_tmp193="must return a value of type %s",_tag_dyneither(_tmp193,sizeof(char),31))),
_tag_dyneither(_tmp192,sizeof(void*),1)))))));}goto _LL2A;_LL2A:;}else{int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp20;Cyc_Tcexp_tcExp(te,(void**)& t,
e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){{const char*_tmp198;void*_tmp197[2];
struct Cyc_String_pa_struct _tmp196;struct Cyc_String_pa_struct _tmp195;(_tmp195.tag=
0,((_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp196.tag=0,((_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp197[0]=& _tmp196,((_tmp197[1]=& _tmp195,Cyc_Tcutil_terr(s0->loc,((_tmp198="returns value of type %s but requires %s",
_tag_dyneither(_tmp198,sizeof(char),41))),_tag_dyneither(_tmp197,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle _tmp66=_new_region("temp");
struct _RegionHandle*temp=& _tmp66;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){const char*_tmp19B;void*_tmp19A;(
_tmp19A=0,Cyc_Tcutil_terr(e->loc,((_tmp19B="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp19B,sizeof(char),49))),_tag_dyneither(_tmp19A,sizeof(void*),0)));};
_pop_region(temp);};}return;}_LL10: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp21=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp18;if(_tmp21->tag != 4)goto _LL12;else{
_tmp22=_tmp21->f1;_tmp23=_tmp21->f2;_tmp24=_tmp21->f3;}}_LL11: {struct _tuple12*
_tmp6B;int _tmp6C;const char*_tmp19C;struct Cyc_Tcexp_TestEnv _tmp6A=Cyc_Tcexp_tcTest(
te,_tmp22,((_tmp19C="if statement",_tag_dyneither(_tmp19C,sizeof(char),13))));
_tmp6B=_tmp6A.eq;_tmp6C=_tmp6A.isTrue;{struct _tuple12*_tmp6D=_tmp6B;struct
_tuple12 _tmp6E;void*_tmp6F;void*_tmp70;_LL34: if(_tmp6D == 0)goto _LL36;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.f1;_tmp70=_tmp6E.f2;_LL35: if(_tmp6C){Cyc_Tcstmt_tcStmtRefine(
te,_tmp23,_tmp6F,_tmp70,1);Cyc_Tcstmt_tcStmt(te,_tmp24,1);}else{Cyc_Tcstmt_tcStmt(
te,_tmp23,1);Cyc_Tcstmt_tcStmtRefine(te,_tmp24,_tmp6F,_tmp70,1);}goto _LL33;_LL36:;
_LL37: Cyc_Tcstmt_tcStmt(te,_tmp23,1);Cyc_Tcstmt_tcStmt(te,_tmp24,1);goto _LL33;
_LL33:;}return;}_LL12: {struct Cyc_Absyn_While_s_struct*_tmp25=(struct Cyc_Absyn_While_s_struct*)
_tmp18;if(_tmp25->tag != 5)goto _LL14;else{_tmp26=_tmp25->f1;_tmp27=_tmp26.f1;
_tmp28=_tmp26.f2;_tmp29=_tmp25->f2;}}_LL13: Cyc_Tcstmt_decorate_stmt(te,_tmp28);{
const char*_tmp19D;Cyc_Tcexp_tcTest(te,_tmp27,((_tmp19D="while loop",
_tag_dyneither(_tmp19D,sizeof(char),11))));}{struct _RegionHandle _tmp72=
_new_region("r");struct _RegionHandle*r=& _tmp72;_push_region(r);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop(r,te,_tmp28),_tmp29,1);_npop_handler(0);return;;
_pop_region(r);};_LL14: {struct Cyc_Absyn_For_s_struct*_tmp2A=(struct Cyc_Absyn_For_s_struct*)
_tmp18;if(_tmp2A->tag != 9)goto _LL16;else{_tmp2B=_tmp2A->f1;_tmp2C=_tmp2A->f2;
_tmp2D=_tmp2C.f1;_tmp2E=_tmp2C.f2;_tmp2F=_tmp2A->f3;_tmp30=_tmp2F.f1;_tmp31=
_tmp2F.f2;_tmp32=_tmp2A->f4;}}_LL15: Cyc_Tcstmt_decorate_stmt(te,_tmp2E);Cyc_Tcstmt_decorate_stmt(
te,_tmp31);Cyc_Tcexp_tcExp(te,0,_tmp2B);{const char*_tmp19E;Cyc_Tcexp_tcTest(te,
_tmp2D,((_tmp19E="for loop",_tag_dyneither(_tmp19E,sizeof(char),9))));}{struct
_RegionHandle _tmp74=_new_region("r");struct _RegionHandle*r=& _tmp74;_push_region(
r);{struct Cyc_Tcenv_Tenv*_tmp75=Cyc_Tcenv_set_in_loop(r,te,_tmp31);Cyc_Tcstmt_tcStmt(
_tmp75,_tmp32,1);Cyc_Tcexp_tcExp(_tmp75,0,_tmp30);Cyc_Tcstmt_simplify_unused_result_exp(
_tmp30);}_npop_handler(0);return;;_pop_region(r);};_LL16: {struct Cyc_Absyn_Do_s_struct*
_tmp33=(struct Cyc_Absyn_Do_s_struct*)_tmp18;if(_tmp33->tag != 14)goto _LL18;else{
_tmp34=_tmp33->f1;_tmp35=_tmp33->f2;_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;}}_LL17:
Cyc_Tcstmt_decorate_stmt(te,_tmp37);{struct _RegionHandle _tmp76=_new_region("r");
struct _RegionHandle*r=& _tmp76;_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
r,te,_tmp37),_tmp34,1);{const char*_tmp19F;Cyc_Tcexp_tcTest(te,_tmp36,((_tmp19F="do loop",
_tag_dyneither(_tmp19F,sizeof(char),8))));}_npop_handler(0);return;;_pop_region(
r);};_LL18: {struct Cyc_Absyn_Break_s_struct*_tmp38=(struct Cyc_Absyn_Break_s_struct*)
_tmp18;if(_tmp38->tag != 6)goto _LL1A;else{_tmp39=_tmp38->f1;_tmp3A=(struct Cyc_Absyn_Stmt**)&
_tmp38->f1;}}_LL19: Cyc_Tcenv_process_break(te,s0,_tmp3A);return;_LL1A: {struct
Cyc_Absyn_Continue_s_struct*_tmp3B=(struct Cyc_Absyn_Continue_s_struct*)_tmp18;
if(_tmp3B->tag != 7)goto _LL1C;else{_tmp3C=_tmp3B->f1;_tmp3D=(struct Cyc_Absyn_Stmt**)&
_tmp3B->f1;}}_LL1B: Cyc_Tcenv_process_continue(te,s0,_tmp3D);return;_LL1C: {
struct Cyc_Absyn_Goto_s_struct*_tmp3E=(struct Cyc_Absyn_Goto_s_struct*)_tmp18;if(
_tmp3E->tag != 8)goto _LL1E;else{_tmp3F=_tmp3E->f1;_tmp40=_tmp3E->f2;_tmp41=(
struct Cyc_Absyn_Stmt**)& _tmp3E->f2;}}_LL1D: Cyc_Tcenv_process_goto(te,s0,_tmp3F,
_tmp41);return;_LL1E: {struct Cyc_Absyn_Fallthru_s_struct*_tmp42=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp18;if(_tmp42->tag != 11)goto _LL20;else{_tmp43=_tmp42->f1;_tmp44=_tmp42->f2;
_tmp45=(struct Cyc_Absyn_Switch_clause***)& _tmp42->f2;}}_LL1F: {const struct
_tuple10*_tmp78=Cyc_Tcenv_process_fallthru(te,s0,_tmp45);if(_tmp78 == (const
struct _tuple10*)0){{const char*_tmp1A2;void*_tmp1A1;(_tmp1A1=0,Cyc_Tcutil_terr(s0->loc,((
_tmp1A2="fallthru not in a non-last case",_tag_dyneither(_tmp1A2,sizeof(char),32))),
_tag_dyneither(_tmp1A1,sizeof(void*),0)));}return;}{struct Cyc_List_List*_tmp7B=(*
_tmp78).f2;const struct Cyc_Tcenv_CList*_tmp7C=(*_tmp78).f3;struct Cyc_List_List*
instantiation=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp7B);struct Cyc_List_List*
_tmp7D=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,
void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(
Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp7C);for(0;_tmp7D != 
0  && _tmp43 != 0;(_tmp7D=_tmp7D->tl,_tmp43=_tmp43->tl)){int bogus=0;Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp43->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)
_tmp43->hd,(void*)_tmp7D->hd,& bogus)){{const char*_tmp1A7;void*_tmp1A6[2];struct
Cyc_String_pa_struct _tmp1A5;struct Cyc_String_pa_struct _tmp1A4;(_tmp1A4.tag=0,((
_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp7D->hd)),((_tmp1A5.tag=0,((_tmp1A5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp43->hd)->topt))->v)),((_tmp1A6[0]=& _tmp1A5,((_tmp1A6[1]=&
_tmp1A4,Cyc_Tcutil_terr(s0->loc,((_tmp1A7="fallthru argument has type %s but pattern variable has type %s",
_tag_dyneither(_tmp1A7,sizeof(char),63))),_tag_dyneither(_tmp1A6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle _tmp82=_new_region("temp");
struct _RegionHandle*temp=& _tmp82;_push_region(temp);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
temp,(void*)_tmp7D->hd) && !Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)
_tmp43->hd)){const char*_tmp1AA;void*_tmp1A9;(_tmp1A9=0,Cyc_Tcutil_terr(((struct
Cyc_Absyn_Exp*)_tmp43->hd)->loc,((_tmp1AA="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp1AA,sizeof(char),49))),_tag_dyneither(_tmp1A9,sizeof(void*),0)));};
_pop_region(temp);};}if(_tmp43 != 0){const char*_tmp1AD;void*_tmp1AC;(_tmp1AC=0,
Cyc_Tcutil_terr(s0->loc,((_tmp1AD="too many arguments to explicit fallthru",
_tag_dyneither(_tmp1AD,sizeof(char),40))),_tag_dyneither(_tmp1AC,sizeof(void*),0)));}
if(_tmp7D != 0){const char*_tmp1B0;void*_tmp1AF;(_tmp1AF=0,Cyc_Tcutil_terr(s0->loc,((
_tmp1B0="too few arguments to explicit fallthru",_tag_dyneither(_tmp1B0,sizeof(
char),39))),_tag_dyneither(_tmp1AF,sizeof(void*),0)));}return;};}_LL20: {struct
Cyc_Absyn_Label_s_struct*_tmp46=(struct Cyc_Absyn_Label_s_struct*)_tmp18;if(
_tmp46->tag != 13)goto _LL22;else{_tmp47=_tmp46->f1;_tmp48=_tmp46->f2;}}_LL21: {
struct _RegionHandle _tmp89=_new_region("r");struct _RegionHandle*r=& _tmp89;
_push_region(r);{struct _dyneither_ptr*_tmp1BD;const char*_tmp1BC;void*_tmp1BB[1];
struct Cyc_String_pa_struct _tmp1BA;struct Cyc_Absyn_Tvar*_tmp1B9;Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp47,_tmp48),((
_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((_tmp1B9->name=((_tmp1BD=_cycalloc(sizeof(*
_tmp1BD)),((_tmp1BD[0]=(struct _dyneither_ptr)((_tmp1BA.tag=0,((_tmp1BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1BB[0]=& _tmp1BA,Cyc_aprintf(((
_tmp1BC="`%s",_tag_dyneither(_tmp1BC,sizeof(char),4))),_tag_dyneither(_tmp1BB,
sizeof(void*),1)))))))),_tmp1BD)))),((_tmp1B9->identity=Cyc_Tcutil_new_tvar_id(),((
_tmp1B9->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1B9))))))))),_tmp48,0);}
_npop_handler(0);return;;_pop_region(r);}_LL22: {struct Cyc_Absyn_Switch_s_struct*
_tmp49=(struct Cyc_Absyn_Switch_s_struct*)_tmp18;if(_tmp49->tag != 10)goto _LL24;
else{_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;}}_LL23: Cyc_Tcexp_tcExp(te,0,_tmp4A);{
void*_tmp8F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A->topt))->v;struct
_RegionHandle _tmp90=_new_region("r");struct _RegionHandle*r=& _tmp90;_push_region(
r);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmp8F) && !Cyc_Tcutil_is_noalias_path(
r,_tmp4A)){const char*_tmp1C0;void*_tmp1BF;(_tmp1BF=0,Cyc_Tcutil_terr(_tmp4A->loc,((
_tmp1C0="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp1C0,sizeof(char),49))),_tag_dyneither(_tmp1BF,sizeof(void*),0)));}{struct Cyc_Tcenv_Tenv*
_tmp93=Cyc_Tcenv_set_in_switch(r,te);_tmp93=Cyc_Tcenv_clear_fallthru(r,_tmp93);{
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp4B);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp94=((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult _tmp95=Cyc_Tcpat_tcPat(_tmp93,
_tmp94,& _tmp8F);struct Cyc_List_List*_tmp96=_tmp95.tvars_and_bounds_opt == 0?0:(*
_tmp95.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp94->topt))->v,_tmp8F)){{const char*_tmp1C5;void*_tmp1C4[2];struct
Cyc_String_pa_struct _tmp1C3;struct Cyc_String_pa_struct _tmp1C2;(_tmp1C2.tag=0,((
_tmp1C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp94->topt))->v)),((_tmp1C3.tag=0,((
_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp8F)),((_tmp1C4[0]=& _tmp1C3,((_tmp1C4[1]=& _tmp1C2,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,((_tmp1C5="switch on type %s, but case expects type %s",
_tag_dyneither(_tmp1C5,sizeof(char),44))),_tag_dyneither(_tmp1C4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(_tmp93,_tmp94);}{
struct Cyc_Core_Opt*_tmp1C6;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((
_tmp1C6=_cycalloc(sizeof(*_tmp1C6)),((_tmp1C6->v=_tmp95.patvars,_tmp1C6))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp93,_tmp95,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp95.tvars_and_bounds_opt != 0  && (*_tmp95.tvars_and_bounds_opt).f2 != 0)_tmp93=
Cyc_Tcenv_clear_fallthru(r,_tmp93);else{_tmp93=Cyc_Tcenv_set_fallthru(r,_tmp93,
_tmp96,_tmp95.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};Cyc_Tcpat_check_switch_exhaustive(
s0->loc,te,_tmp4B);_npop_handler(0);return;;_pop_region(r);};_LL24: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp4C=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp18;if(_tmp4C->tag != 15)goto _LL26;
else{_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;}}_LL25:{struct Cyc_Absyn_Seq_s_struct
_tmp1C9;struct Cyc_Absyn_Seq_s_struct*_tmp1C8;_tmp4D->r=(Cyc_Absyn_new_stmt((void*)((
_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8[0]=((_tmp1C9.tag=2,((_tmp1C9.f1=
Cyc_Absyn_new_stmt(_tmp4D->r,_tmp4D->loc),((_tmp1C9.f2=Cyc_Absyn_skip_stmt(
_tmp4D->loc),_tmp1C9)))))),_tmp1C8)))),_tmp4D->loc))->r;}{struct _RegionHandle
_tmp9E=_new_region("r");struct _RegionHandle*r=& _tmp9E;_push_region(r);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp4D,1);{struct
_RegionHandle _tmp9F=_new_region("r2");struct _RegionHandle*r2=& _tmp9F;
_push_region(r2);{struct Cyc_Tcenv_Tenv*_tmpA0=Cyc_Tcenv_set_in_switch(r2,te);
_tmpA0=Cyc_Tcenv_clear_fallthru(r2,_tmpA0);{struct Cyc_List_List*_tmpA1=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp4E);for(0;_tmpA1 != 0;
_tmpA1=_tmpA1->tl){struct Cyc_Absyn_Pat*_tmpA2=((struct Cyc_Absyn_Switch_clause*)
_tmpA1->hd)->pattern;void*_tmpA3=Cyc_Absyn_exn_typ();struct Cyc_Tcpat_TcPatResult
_tmpA4=Cyc_Tcpat_tcPat(_tmpA0,_tmpA2,& _tmpA3);struct Cyc_List_List*_tmpA5=_tmpA4.tvars_and_bounds_opt
== 0?0:(*_tmpA4.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA2->topt))->v,_tmpA3)){const char*_tmp1CD;void*_tmp1CC[1];struct
Cyc_String_pa_struct _tmp1CB;(_tmp1CB.tag=0,((_tmp1CB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA2->topt))->v)),((_tmp1CC[0]=& _tmp1CB,Cyc_Tcutil_terr(((struct
Cyc_Absyn_Switch_clause*)_tmpA1->hd)->loc,((_tmp1CD="expected datatype exn but found %s",
_tag_dyneither(_tmp1CD,sizeof(char),35))),_tag_dyneither(_tmp1CC,sizeof(void*),1)))))));}
else{Cyc_Tcpat_check_pat_regions(_tmpA0,_tmpA2);}{struct Cyc_Core_Opt*_tmp1CE;((
struct Cyc_Absyn_Switch_clause*)_tmpA1->hd)->pat_vars=((_tmp1CE=_cycalloc(sizeof(*
_tmp1CE)),((_tmp1CE->v=_tmpA4.patvars,_tmp1CE))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmpA1->hd)->loc,_tmpA0,_tmpA4,((struct Cyc_Absyn_Switch_clause*)
_tmpA1->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpA1->hd)->where_clause,1);
if(_tmpA4.tvars_and_bounds_opt != 0  && (*_tmpA4.tvars_and_bounds_opt).f2 != 0)
_tmpA0=Cyc_Tcenv_clear_fallthru(r2,_tmpA0);else{_tmpA0=Cyc_Tcenv_set_fallthru(r2,
_tmpA0,_tmpA5,_tmpA4.patvars,(struct Cyc_Absyn_Switch_clause*)_tmpA1->hd);}}};}
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp4E);_npop_handler(1);return;;
_pop_region(r2);};;_pop_region(r);};_LL26: {struct Cyc_Absyn_Decl_s_struct*_tmp4F=(
struct Cyc_Absyn_Decl_s_struct*)_tmp18;if(_tmp4F->tag != 12)goto _LL28;else{_tmp50=
_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL27: {struct _dyneither_ptr unimp_msg_part;struct
_RegionHandle _tmpAA=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpAA;
_push_region(decl_rgn);{struct Cyc_Tcenv_Tenv*te2;if(new_block)te2=Cyc_Tcenv_new_block(
decl_rgn,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{struct Cyc_Tcenv_Tenv*
_tmpAB=te2;void*_tmpAC=_tmp50->r;struct Cyc_Absyn_Vardecl*_tmpAE;struct Cyc_Absyn_Pat*
_tmpB0;struct Cyc_Core_Opt*_tmpB1;struct Cyc_Core_Opt**_tmpB2;struct Cyc_Absyn_Exp*
_tmpB3;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_Tvar*_tmpB7;struct Cyc_Absyn_Vardecl*
_tmpB8;int _tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Tvar*
_tmpBD;struct Cyc_Absyn_Vardecl*_tmpBE;struct Cyc_Absyn_Fndecl*_tmpC0;struct
_dyneither_ptr*_tmpC2;struct Cyc_List_List*_tmpC3;struct _tuple0*_tmpC5;struct Cyc_List_List*
_tmpC6;_LL39: {struct Cyc_Absyn_Var_d_struct*_tmpAD=(struct Cyc_Absyn_Var_d_struct*)
_tmpAC;if(_tmpAD->tag != 0)goto _LL3B;else{_tmpAE=_tmpAD->f1;}}_LL3A: {struct Cyc_Absyn_Vardecl
_tmpD0;enum Cyc_Absyn_Scope _tmpD1;struct _tuple0*_tmpD2;struct _tuple0 _tmpD3;union
Cyc_Absyn_Nmspace _tmpD4;struct _dyneither_ptr*_tmpD5;struct Cyc_Absyn_Tqual _tmpD6;
void*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Core_Opt*_tmpD9;struct Cyc_Core_Opt**
_tmpDA;struct Cyc_List_List*_tmpDB;struct Cyc_Absyn_Vardecl*_tmpCF=_tmpAE;_tmpD0=*
_tmpCF;_tmpD1=_tmpD0.sc;_tmpD2=_tmpD0.name;_tmpD3=*_tmpD2;_tmpD4=_tmpD3.f1;
_tmpD5=_tmpD3.f2;_tmpD6=_tmpD0.tq;_tmpD7=_tmpD0.type;_tmpD8=_tmpD0.initializer;
_tmpD9=_tmpD0.rgn;_tmpDA=(struct Cyc_Core_Opt**)&(*_tmpCF).rgn;_tmpDB=_tmpD0.attributes;{
void*_tmpDC=Cyc_Tcenv_curr_rgn(_tmpAB);int is_local;switch(_tmpD1){case Cyc_Absyn_Static:
_LL59: goto _LL5A;case Cyc_Absyn_Extern: _LL5A: goto _LL5B;case Cyc_Absyn_ExternC: _LL5B:
is_local=0;break;case Cyc_Absyn_Abstract: _LL5C:{const char*_tmp1D1;void*_tmp1D0;(
_tmp1D0=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1D1="bad abstract scope for local variable",
_tag_dyneither(_tmp1D1,sizeof(char),38))),_tag_dyneither(_tmp1D0,sizeof(void*),0)));}
goto _LL5D;case Cyc_Absyn_Register: _LL5D: goto _LL5E;case Cyc_Absyn_Public: _LL5E:
is_local=1;break;}{struct Cyc_Core_Opt*_tmp1D3;struct Cyc_Core_Opt*_tmp1D2;*_tmpDA=
is_local?(_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3->v=_tmpDC,_tmp1D3))):((
_tmp1D2=_cycalloc(sizeof(*_tmp1D2)),((_tmp1D2->v=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1D2))));}{union Cyc_Absyn_Nmspace _tmpE1=_tmpD4;int _tmpE2;struct Cyc_List_List*
_tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE5;_LL61: if((_tmpE1.Loc_n).tag
!= 4)goto _LL63;_tmpE2=(int)(_tmpE1.Loc_n).val;_LL62: goto _LL60;_LL63: if((_tmpE1.Rel_n).tag
!= 1)goto _LL65;_tmpE3=(struct Cyc_List_List*)(_tmpE1.Rel_n).val;if(_tmpE3 != 0)
goto _LL65;_LL64:(*_tmpAE->name).f1=Cyc_Absyn_Loc_n;goto _LL60;_LL65: if((_tmpE1.C_n).tag
!= 3)goto _LL67;_tmpE4=(struct Cyc_List_List*)(_tmpE1.C_n).val;_LL66: {const char*
_tmp1D6;void*_tmp1D5;(int)_throw(((_tmp1D5=0,((void*(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1D6="tcstmt: C_n declaration",
_tag_dyneither(_tmp1D6,sizeof(char),24))),_tag_dyneither(_tmp1D5,sizeof(void*),0)))));}
_LL67: if((_tmpE1.Abs_n).tag != 2)goto _LL69;_tmpE5=(struct Cyc_List_List*)(_tmpE1.Abs_n).val;
_LL68: {const char*_tmp1D9;void*_tmp1D8;(int)_throw(((_tmp1D8=0,((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1D9="tcstmt: Abs_n declaration",
_tag_dyneither(_tmp1D9,sizeof(char),26))),_tag_dyneither(_tmp1D8,sizeof(void*),0)))));}
_LL69:;_LL6A:{const char*_tmp1DC;void*_tmp1DB;(_tmp1DB=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1DC="cannot declare a qualified local variable",_tag_dyneither(_tmp1DC,
sizeof(char),42))),_tag_dyneither(_tmp1DB,sizeof(void*),0)));}goto _LL60;_LL60:;}{
void*_tmpEC=Cyc_Tcutil_compress(_tmpD7);struct Cyc_Absyn_ArrayInfo _tmpEE;void*
_tmpEF;struct Cyc_Absyn_Tqual _tmpF0;struct Cyc_Absyn_Exp*_tmpF1;union Cyc_Absyn_Constraint*
_tmpF2;unsigned int _tmpF3;_LL6C:{struct Cyc_Absyn_ArrayType_struct*_tmpED=(struct
Cyc_Absyn_ArrayType_struct*)_tmpEC;if(_tmpED->tag != 8)goto _LL6E;else{_tmpEE=
_tmpED->f1;_tmpEF=_tmpEE.elt_type;_tmpF0=_tmpEE.tq;_tmpF1=_tmpEE.num_elts;if(
_tmpF1 != 0)goto _LL6E;_tmpF2=_tmpEE.zero_term;_tmpF3=_tmpEE.zt_loc;}}if(!(_tmpAE->initializer
!= 0))goto _LL6E;_LL6D:{void*_tmpF4=((struct Cyc_Absyn_Exp*)_check_null(_tmpAE->initializer))->r;
union Cyc_Absyn_Cnst _tmpF6;struct _dyneither_ptr _tmpF7;union Cyc_Absyn_Cnst _tmpF9;
struct _dyneither_ptr _tmpFA;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_List_List*_tmpFE;
struct Cyc_List_List*_tmp100;_LL71: {struct Cyc_Absyn_Const_e_struct*_tmpF5=(
struct Cyc_Absyn_Const_e_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL73;else{_tmpF6=
_tmpF5->f1;if((_tmpF6.String_c).tag != 8)goto _LL73;_tmpF7=(struct _dyneither_ptr)(
_tmpF6.String_c).val;}}_LL72: _tmpD7=(_tmpAE->type=Cyc_Absyn_array_typ(_tmpEF,
_tmpF0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpF7,
sizeof(char)),0),_tmpF2,_tmpF3));goto _LL70;_LL73: {struct Cyc_Absyn_Const_e_struct*
_tmpF8=(struct Cyc_Absyn_Const_e_struct*)_tmpF4;if(_tmpF8->tag != 0)goto _LL75;
else{_tmpF9=_tmpF8->f1;if((_tmpF9.Wstring_c).tag != 9)goto _LL75;_tmpFA=(struct
_dyneither_ptr)(_tmpF9.Wstring_c).val;}}_LL74: _tmpD7=(_tmpAE->type=Cyc_Absyn_array_typ(
_tmpEF,_tmpF0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0),_tmpF2,_tmpF3));goto
_LL70;_LL75: {struct Cyc_Absyn_Comprehension_e_struct*_tmpFB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmpF4;if(_tmpFB->tag != 28)goto _LL77;else{_tmpFC=_tmpFB->f2;}}_LL76: _tmpD7=(
_tmpAE->type=Cyc_Absyn_array_typ(_tmpEF,_tmpF0,(struct Cyc_Absyn_Exp*)_tmpFC,
_tmpF2,_tmpF3));goto _LL70;_LL77: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpFD=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpF4;if(_tmpFD->tag != 36)goto _LL79;
else{_tmpFE=_tmpFD->f2;}}_LL78: _tmp100=_tmpFE;goto _LL7A;_LL79: {struct Cyc_Absyn_Array_e_struct*
_tmpFF=(struct Cyc_Absyn_Array_e_struct*)_tmpF4;if(_tmpFF->tag != 27)goto _LL7B;
else{_tmp100=_tmpFF->f1;}}_LL7A: _tmpD7=(_tmpAE->type=Cyc_Absyn_array_typ(_tmpEF,
_tmpF0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp100),0),_tmpF2,_tmpF3));goto _LL70;_LL7B:;_LL7C: goto _LL70;
_LL70:;}goto _LL6B;_LL6E:;_LL6F: goto _LL6B;_LL6B:;}{struct Cyc_List_List*_tmp101=!
is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpAB);int _tmp102=!is_local?0: 1;Cyc_Tcutil_check_type(
s0->loc,_tmpAB,_tmp101,& Cyc_Tcutil_tmk,_tmp102,_tmpD7);(_tmpAE->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpAE->tq).print_const,_tmpD7);{
struct Cyc_Tcenv_Tenv*_tmp103=Cyc_Tcenv_add_local_var(decl_rgn,_tmp51->loc,_tmpAB,
_tmpAE);if(_tmpD1 == Cyc_Absyn_Extern  || _tmpD1 == Cyc_Absyn_ExternC){const char*
_tmp1DF;void*_tmp1DE;(_tmp1DE=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1DF="local extern declarations not yet supported",
_tag_dyneither(_tmp1DF,sizeof(char),44))),_tag_dyneither(_tmp1DE,sizeof(void*),0)));}
if(_tmpD8 != 0){Cyc_Tcexp_tcExpInitializer(_tmp103,(void**)& _tmpD7,(struct Cyc_Absyn_Exp*)
_tmpD8);if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpAB,(struct Cyc_Absyn_Exp*)
_tmpD8)){const char*_tmp1E2;void*_tmp1E1;(_tmp1E1=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1E2="initializer needs to be a constant expression",_tag_dyneither(_tmp1E2,
sizeof(char),46))),_tag_dyneither(_tmp1E1,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(
_tmp103,(struct Cyc_Absyn_Exp*)_tmpD8,_tmpD7)){struct _dyneither_ptr _tmp108=*
_tmpD5;struct _dyneither_ptr _tmp109=Cyc_Absynpp_typ2string(_tmpD7);struct
_dyneither_ptr _tmp10A=Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD8->topt))->v);if(((_get_dyneither_size(_tmp108,sizeof(char))+ 
_get_dyneither_size(_tmp109,sizeof(char)))+ _get_dyneither_size(_tmp10A,sizeof(
char)))+ 50 < 80){const char*_tmp1E8;void*_tmp1E7[3];struct Cyc_String_pa_struct
_tmp1E6;struct Cyc_String_pa_struct _tmp1E5;struct Cyc_String_pa_struct _tmp1E4;(
_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((
_tmp1E5.tag=0,((_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109),((
_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108),((
_tmp1E7[0]=& _tmp1E6,((_tmp1E7[1]=& _tmp1E5,((_tmp1E7[2]=& _tmp1E4,Cyc_Tcutil_terr(
_tmp50->loc,((_tmp1E8="%s declared with type %s but initialized with type %s.",
_tag_dyneither(_tmp1E8,sizeof(char),55))),_tag_dyneither(_tmp1E7,sizeof(void*),3)))))))))))))))))));}
else{if((_get_dyneither_size(_tmp108,sizeof(char))+ _get_dyneither_size(_tmp109,
sizeof(char)))+ 25 < 80  && _get_dyneither_size(_tmp10A,sizeof(char))+ 25 < 80){
const char*_tmp1EE;void*_tmp1ED[3];struct Cyc_String_pa_struct _tmp1EC;struct Cyc_String_pa_struct
_tmp1EB;struct Cyc_String_pa_struct _tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((_tmp1EB.tag=0,((_tmp1EB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109),((_tmp1EC.tag=0,((_tmp1EC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108),((_tmp1ED[0]=& _tmp1EC,((
_tmp1ED[1]=& _tmp1EB,((_tmp1ED[2]=& _tmp1EA,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1EE="%s declared with type %s\n but initialized with type %s.",
_tag_dyneither(_tmp1EE,sizeof(char),56))),_tag_dyneither(_tmp1ED,sizeof(void*),3)))))))))))))))))));}
else{const char*_tmp1F4;void*_tmp1F3[3];struct Cyc_String_pa_struct _tmp1F2;struct
Cyc_String_pa_struct _tmp1F1;struct Cyc_String_pa_struct _tmp1F0;(_tmp1F0.tag=0,((
_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((_tmp1F1.tag=0,((
_tmp1F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109),((_tmp1F2.tag=0,((
_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108),((_tmp1F3[0]=&
_tmp1F2,((_tmp1F3[1]=& _tmp1F1,((_tmp1F3[2]=& _tmp1F0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1F4="%s declared with type \n%s\n but initialized with type \n%s.",
_tag_dyneither(_tmp1F4,sizeof(char),58))),_tag_dyneither(_tmp1F3,sizeof(void*),3)))))))))))))))))));}}
Cyc_Tcutil_unify(_tmpD7,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD8->topt))->v);
Cyc_Tcutil_explain_failure();}}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,
_tmp103,s0),_tmp51,0);_npop_handler(0);return;};};};}_LL3B: {struct Cyc_Absyn_Let_d_struct*
_tmpAF=(struct Cyc_Absyn_Let_d_struct*)_tmpAC;if(_tmpAF->tag != 2)goto _LL3D;else{
_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;_tmpB2=(struct Cyc_Core_Opt**)& _tmpAF->f2;
_tmpB3=_tmpAF->f3;}}_LL3C: Cyc_Tcexp_tcExpInitializer(_tmpAB,0,_tmpB3);{struct Cyc_Tcpat_TcPatResult
_tmp11A=Cyc_Tcpat_tcPat(_tmpAB,_tmpB0,(void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(_tmpB3->topt))->v));{struct Cyc_Core_Opt*_tmp1F5;*_tmpB2=((_tmp1F5=
_cycalloc(sizeof(*_tmp1F5)),((_tmp1F5->v=_tmp11A.patvars,_tmp1F5))));}if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB0->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpB3->topt))->v) && !Cyc_Tcutil_coerce_assign(_tmpAB,_tmpB3,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB0->topt))->v)){{const char*_tmp1FA;void*
_tmp1F9[2];struct Cyc_String_pa_struct _tmp1F8;struct Cyc_String_pa_struct _tmp1F7;(
_tmp1F7.tag=0,((_tmp1F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB3->topt))->v)),((_tmp1F8.tag=0,((
_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB0->topt))->v)),((_tmp1F9[0]=&
_tmp1F8,((_tmp1F9[1]=& _tmp1F7,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1FA="pattern type %s does not match definition type %s",
_tag_dyneither(_tmp1FA,sizeof(char),50))),_tag_dyneither(_tmp1F9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpB0->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB3->topt))->v);Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmpAB,_tmpB0);}Cyc_Tcpat_check_let_pat_exhaustive(
_tmpB0->loc,_tmpAB,_tmpB0);Cyc_Tcstmt_pattern_synth(s0->loc,_tmpAB,_tmp11A,
_tmp51,0,0);_npop_handler(0);return;};_LL3D: {struct Cyc_Absyn_Letv_d_struct*
_tmpB4=(struct Cyc_Absyn_Letv_d_struct*)_tmpAC;if(_tmpB4->tag != 3)goto _LL3F;else{
_tmpB5=_tmpB4->f1;}}_LL3E: {void*_tmp120=Cyc_Tcenv_curr_rgn(_tmpAB);struct Cyc_Tcenv_Tenv*
_tmp121=_tmpAB;for(0;_tmpB5 != 0;_tmpB5=_tmpB5->tl){struct Cyc_Absyn_Vardecl*
_tmp122=(struct Cyc_Absyn_Vardecl*)_tmpB5->hd;struct Cyc_Absyn_Vardecl _tmp124;
struct _tuple0*_tmp125;struct _tuple0 _tmp126;union Cyc_Absyn_Nmspace _tmp127;void*
_tmp128;struct Cyc_Core_Opt*_tmp129;struct Cyc_Core_Opt**_tmp12A;struct Cyc_Absyn_Vardecl*
_tmp123=_tmp122;_tmp124=*_tmp123;_tmp125=_tmp124.name;_tmp126=*_tmp125;_tmp127=
_tmp126.f1;_tmp128=_tmp124.type;_tmp129=_tmp124.rgn;_tmp12A=(struct Cyc_Core_Opt**)&(*
_tmp123).rgn;{struct Cyc_Core_Opt*_tmp1FB;*_tmp12A=((_tmp1FB=_cycalloc(sizeof(*
_tmp1FB)),((_tmp1FB->v=_tmp120,_tmp1FB))));}{union Cyc_Absyn_Nmspace _tmp12C=
_tmp127;int _tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_List_List*_tmp12F;_LL7E:
if((_tmp12C.Loc_n).tag != 4)goto _LL80;_tmp12D=(int)(_tmp12C.Loc_n).val;_LL7F: goto
_LL7D;_LL80: if((_tmp12C.Rel_n).tag != 1)goto _LL82;_tmp12E=(struct Cyc_List_List*)(
_tmp12C.Rel_n).val;if(_tmp12E != 0)goto _LL82;_LL81:(*_tmp122->name).f1=Cyc_Absyn_Loc_n;
goto _LL7D;_LL82: if((_tmp12C.Abs_n).tag != 2)goto _LL84;_tmp12F=(struct Cyc_List_List*)(
_tmp12C.Abs_n).val;_LL83: {const char*_tmp1FE;void*_tmp1FD;(int)_throw(((_tmp1FD=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1FE="tcstmt: Abs_n declaration",_tag_dyneither(_tmp1FE,sizeof(char),26))),
_tag_dyneither(_tmp1FD,sizeof(void*),0)))));}_LL84:;_LL85:{const char*_tmp201;
void*_tmp200;(_tmp200=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp201="cannot declare a qualified local variable",
_tag_dyneither(_tmp201,sizeof(char),42))),_tag_dyneither(_tmp200,sizeof(void*),0)));}
goto _LL7D;_LL7D:;}Cyc_Tcutil_check_type(s0->loc,_tmp121,Cyc_Tcenv_lookup_type_vars(
_tmp121),& Cyc_Tcutil_tmk,1,_tmp128);_tmp121=Cyc_Tcenv_add_local_var(decl_rgn,
_tmp51->loc,_tmp121,_tmp122);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,
_tmp121,s0),_tmp51,0);_npop_handler(0);return;}_LL3F: {struct Cyc_Absyn_Region_d_struct*
_tmpB6=(struct Cyc_Absyn_Region_d_struct*)_tmpAC;if(_tmpB6->tag != 4)goto _LL41;
else{_tmpB7=_tmpB6->f1;_tmpB8=_tmpB6->f2;_tmpB9=_tmpB6->f3;_tmpBA=_tmpB6->f4;}}
_LL40:{struct Cyc_Absyn_Seq_s_struct _tmp204;struct Cyc_Absyn_Seq_s_struct*_tmp203;
_tmp51->r=(Cyc_Absyn_new_stmt((void*)((_tmp203=_cycalloc(sizeof(*_tmp203)),((
_tmp203[0]=((_tmp204.tag=2,((_tmp204.f1=Cyc_Absyn_new_stmt(_tmp51->r,_tmp51->loc),((
_tmp204.f2=Cyc_Absyn_skip_stmt(_tmp51->loc),_tmp204)))))),_tmp203)))),_tmp51->loc))->r;}{
struct Cyc_Tcenv_Tenv*_tmp136=_tmpAB;struct Cyc_Absyn_Vardecl _tmp138;void*_tmp139;
void**_tmp13A;struct Cyc_Core_Opt*_tmp13B;struct Cyc_Core_Opt**_tmp13C;struct Cyc_Absyn_Vardecl*
_tmp137=_tmpB8;_tmp138=*_tmp137;_tmp139=_tmp138.type;_tmp13A=(void**)&(*_tmp137).type;
_tmp13B=_tmp138.rgn;_tmp13C=(struct Cyc_Core_Opt**)&(*_tmp137).rgn;{void*_tmp13D=
Cyc_Tcenv_curr_rgn(_tmpAB);{struct Cyc_Core_Opt*_tmp205;*_tmp13C=((_tmp205=
_cycalloc(sizeof(*_tmp205)),((_tmp205->v=_tmp13D,_tmp205))));}{void*rgn_typ;if(
_tmpBA != 0){struct Cyc_Absyn_Exp*open_exp=(struct Cyc_Absyn_Exp*)_tmpBA;struct Cyc_List_List*
_tmp13F=Cyc_Tcenv_lookup_type_vars(_tmpAB);{struct Cyc_Core_Opt*_tmp206;rgn_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,((_tmp206=_cycalloc(
sizeof(*_tmp206)),((_tmp206->v=_tmp13F,_tmp206)))));}{struct Cyc_Core_Opt*_tmp207;
void*rgn2_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,((_tmp207=
_cycalloc(sizeof(*_tmp207)),((_tmp207->v=_tmp13F,_tmp207)))));struct Cyc_Absyn_DynRgnType_struct
_tmp20A;struct Cyc_Absyn_DynRgnType_struct*_tmp209;void*expected=(void*)((_tmp209=
_cycalloc(sizeof(*_tmp209)),((_tmp209[0]=((_tmp20A.tag=16,((_tmp20A.f1=(void*)
rgn_typ,((_tmp20A.f2=(void*)rgn2_typ,_tmp20A)))))),_tmp209))));{struct Cyc_Absyn_RgnHandleType_struct
_tmp20D;struct Cyc_Absyn_RgnHandleType_struct*_tmp20C;*_tmp13A=(void*)((_tmp20C=
_cycalloc(sizeof(*_tmp20C)),((_tmp20C[0]=((_tmp20D.tag=15,((_tmp20D.f1=(void*)
rgn_typ,_tmp20D)))),_tmp20C))));}if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(_tmpAB,(
void**)& expected,open_exp),expected)){const char*_tmp212;void*_tmp211[2];struct
Cyc_String_pa_struct _tmp210;struct Cyc_String_pa_struct _tmp20F;(_tmp20F.tag=0,((
_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(open_exp->topt))->v)),((_tmp210.tag=0,((
_tmp210.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
expected)),((_tmp211[0]=& _tmp210,((_tmp211[1]=& _tmp20F,Cyc_Tcutil_terr(open_exp->loc,((
_tmp212="expecting %s but found %s",_tag_dyneither(_tmp212,sizeof(char),26))),
_tag_dyneither(_tmp211,sizeof(void*),2)))))))))))));}Cyc_Tcenv_check_rgn_accessible(
_tmpAB,open_exp->loc,rgn2_typ);_tmp136=Cyc_Tcenv_add_region(decl_rgn,_tmp136,
rgn_typ,0,1);};}else{{struct Cyc_Absyn_VarType_struct _tmp215;struct Cyc_Absyn_VarType_struct*
_tmp214;rgn_typ=(void*)((_tmp214=_cycalloc(sizeof(*_tmp214)),((_tmp214[0]=((
_tmp215.tag=2,((_tmp215.f1=_tmpB7,_tmp215)))),_tmp214))));}{struct Cyc_List_List*
_tmp216;_tmp136=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp136,((_tmp216=
_cycalloc(sizeof(*_tmp216)),((_tmp216->hd=_tmpB7,((_tmp216->tl=0,_tmp216)))))));}
_tmp136=Cyc_Tcenv_add_region(decl_rgn,_tmp136,rgn_typ,_tmpB9,1);}Cyc_Tcutil_check_type(
s0->loc,_tmpAB,Cyc_Tcenv_lookup_type_vars(_tmp136),& Cyc_Tcutil_bk,1,*_tmp13A);{
struct Cyc_Absyn_RgnHandleType_struct _tmp219;struct Cyc_Absyn_RgnHandleType_struct*
_tmp218;if(!Cyc_Tcutil_unify(*_tmp13A,(void*)((_tmp218=_cycalloc(sizeof(*_tmp218)),((
_tmp218[0]=((_tmp219.tag=15,((_tmp219.f1=(void*)rgn_typ,_tmp219)))),_tmp218)))))){
const char*_tmp21C;void*_tmp21B;(_tmp21B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp21C="region stmt: type of region handle is wrong!",
_tag_dyneither(_tmp21C,sizeof(char),45))),_tag_dyneither(_tmp21B,sizeof(void*),0)));}}
if(!Cyc_Absyn_no_regions)_tmp136=Cyc_Tcenv_enter_try(decl_rgn,_tmp136);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp51->loc,
_tmp136,_tmpB8),s0),_tmp51,0);_npop_handler(0);return;};};};_LL41: {struct Cyc_Absyn_Alias_d_struct*
_tmpBB=(struct Cyc_Absyn_Alias_d_struct*)_tmpAC;if(_tmpBB->tag != 5)goto _LL43;
else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;_tmpBE=_tmpBB->f3;}}_LL42: {struct
_RegionHandle _tmp151=_new_region("r");struct _RegionHandle*r=& _tmp151;
_push_region(r);{struct Cyc_Absyn_Tvar*_tmp21D[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
r,s0->loc,_tmpAB,((_tmp21D[0]=_tmpBD,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp21D,
sizeof(struct Cyc_Absyn_Tvar*),1)))));{struct Cyc_Absyn_VarType_struct _tmp220;
struct Cyc_Absyn_VarType_struct*_tmp21F;te2=Cyc_Tcenv_add_region(r,te2,(void*)((
_tmp21F=_cycalloc(sizeof(*_tmp21F)),((_tmp21F[0]=((_tmp220.tag=2,((_tmp220.f1=
_tmpBD,_tmp220)))),_tmp21F)))),0,1);}Cyc_Tcexp_tcExpInitializer(te2,0,_tmpBC);
if(!Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmpBC->topt))->v)){{
const char*_tmp224;void*_tmp223[1];struct Cyc_String_pa_struct _tmp222;(_tmp222.tag=
0,((_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpBC->topt))->v)),((_tmp223[0]=&
_tmp222,Cyc_Tcutil_terr(_tmpBC->loc,((_tmp224="%s is not a pointer type",
_tag_dyneither(_tmp224,sizeof(char),25))),_tag_dyneither(_tmp223,sizeof(void*),1)))))));}
_npop_handler(1);return;}_tmpBE->type=Cyc_Tcutil_copy_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpBC->topt))->v);Cyc_Tcutil_check_type(_tmpBC->loc,_tmpAB,Cyc_Tcenv_lookup_type_vars(
te2),& Cyc_Tcutil_mk,1,_tmpBE->type);{struct Cyc_Core_Opt*_tmp225;_tmpBE->rgn=((
_tmp225=_cycalloc(sizeof(*_tmp225)),((_tmp225->v=Cyc_Tcenv_curr_rgn(te2),_tmp225))));}
te2=Cyc_Tcenv_add_local_var(r,s0->loc,te2,_tmpBE);te2=Cyc_Tcenv_set_encloser(r,
te2,_tmp51);{struct Cyc_List_List*_tmp158=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcstmt_tcStmt(
te2,_tmp51,0);};}Cyc_NewControlFlow_set_encloser(s0,Cyc_Tcenv_get_encloser(
_tmpAB));_npop_handler(1);return;;_pop_region(r);}_LL43: {struct Cyc_Absyn_Fn_d_struct*
_tmpBF=(struct Cyc_Absyn_Fn_d_struct*)_tmpAC;if(_tmpBF->tag != 1)goto _LL45;else{
_tmpC0=_tmpBF->f1;}}_LL44: {void*_tmp15A=Cyc_Tcenv_curr_rgn(_tmpAB);if(_tmpC0->sc
!= Cyc_Absyn_Public){const char*_tmp228;void*_tmp227;(_tmp227=0,Cyc_Tcutil_terr(
_tmp50->loc,((_tmp228="bad storage class for inner function",_tag_dyneither(
_tmp228,sizeof(char),37))),_tag_dyneither(_tmp227,sizeof(void*),0)));}{union Cyc_Absyn_Nmspace
_tmp15D=(*_tmpC0->name).f1;struct Cyc_List_List*_tmp15E;struct Cyc_List_List*
_tmp15F;_LL87: if((_tmp15D.Rel_n).tag != 1)goto _LL89;_tmp15E=(struct Cyc_List_List*)(
_tmp15D.Rel_n).val;if(_tmp15E != 0)goto _LL89;_LL88: goto _LL86;_LL89: if((_tmp15D.Abs_n).tag
!= 2)goto _LL8B;_tmp15F=(struct Cyc_List_List*)(_tmp15D.Abs_n).val;_LL8A: {const
char*_tmp22B;void*_tmp22A;(_tmp22A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp22B="tc: Abs_n in tcStmt var decl",
_tag_dyneither(_tmp22B,sizeof(char),29))),_tag_dyneither(_tmp22A,sizeof(void*),0)));}
_LL8B:;_LL8C: {const char*_tmp22E;void*_tmp22D;(_tmp22D=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp22E="explicit namespace not allowed on inner function declaration",
_tag_dyneither(_tmp22E,sizeof(char),61))),_tag_dyneither(_tmp22D,sizeof(void*),0)));}
_LL86:;}(*_tmpC0->name).f1=Cyc_Absyn_Loc_n;Cyc_Tcutil_check_fndecl_valid_type(
_tmp50->loc,_tmpAB,_tmpC0);{void*t=Cyc_Tcutil_fndecl2typ(_tmpC0);_tmpC0->attributes=
Cyc_Tcutil_transfer_fn_type_atts(t,_tmpC0->attributes);{struct Cyc_List_List*atts=
_tmpC0->attributes;for(0;(unsigned int)atts;atts=atts->tl){void*_tmp164=(void*)
atts->hd;_LL8E: {struct Cyc_Absyn_Packed_att_struct*_tmp165=(struct Cyc_Absyn_Packed_att_struct*)
_tmp164;if(_tmp165->tag != 7)goto _LL90;}_LL8F: goto _LL91;_LL90: {struct Cyc_Absyn_Aligned_att_struct*
_tmp166=(struct Cyc_Absyn_Aligned_att_struct*)_tmp164;if(_tmp166->tag != 6)goto
_LL92;}_LL91:{const char*_tmp232;void*_tmp231[1];struct Cyc_String_pa_struct
_tmp230;(_tmp230.tag=0,((_tmp230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp231[0]=& _tmp230,Cyc_Tcutil_terr(
_tmp50->loc,((_tmp232="bad attribute %s for function",_tag_dyneither(_tmp232,
sizeof(char),30))),_tag_dyneither(_tmp231,sizeof(void*),1)))))));}goto _LL8D;
_LL92:;_LL93: goto _LL8D;_LL8D:;}}{struct Cyc_Core_Opt*_tmp235;struct Cyc_Absyn_Vardecl*
_tmp234;struct Cyc_Absyn_Vardecl*vd=(_tmp234=_cycalloc(sizeof(*_tmp234)),((
_tmp234->sc=_tmpC0->sc,((_tmp234->name=_tmpC0->name,((_tmp234->tq=Cyc_Absyn_const_tqual(
0),((_tmp234->type=Cyc_Absyn_at_typ(t,_tmp15A,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((
_tmp234->initializer=0,((_tmp234->rgn=((_tmp235=_cycalloc(sizeof(*_tmp235)),((
_tmp235->v=_tmp15A,_tmp235)))),((_tmp234->attributes=0,((_tmp234->escapes=0,
_tmp234)))))))))))))))));_tmpC0->fn_vardecl=(struct Cyc_Absyn_Vardecl*)vd;_tmpAB=
Cyc_Tcenv_add_local_var(decl_rgn,_tmp50->loc,_tmpAB,vd);{struct Cyc_Tcenv_Fenv*
old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAB->le);_tmpAB->le=(struct Cyc_Tcenv_Fenv*)
Cyc_Tcenv_nested_fenv(_tmp50->loc,old_fenv,_tmpC0);Cyc_Tcstmt_tcStmt(_tmpAB,
_tmpC0->body,0);Cyc_Tcenv_check_delayed_effects(_tmpAB);Cyc_Tcenv_check_delayed_constraints(
_tmpAB);if(!Cyc_Tcenv_all_labels_resolved(_tmpAB)){const char*_tmp238;void*
_tmp237;(_tmp237=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp238="function has goto statements to undefined labels",
_tag_dyneither(_tmp238,sizeof(char),49))),_tag_dyneither(_tmp237,sizeof(void*),0)));}
_tmpAB->le=(struct Cyc_Tcenv_Fenv*)old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
decl_rgn,_tmpAB,s0),_tmp51,0);_npop_handler(0);return;};};};}_LL45: {struct Cyc_Absyn_Namespace_d_struct*
_tmpC1=(struct Cyc_Absyn_Namespace_d_struct*)_tmpAC;if(_tmpC1->tag != 10)goto _LL47;
else{_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;}}_LL46:{const char*_tmp239;
unimp_msg_part=((_tmp239="namespace",_tag_dyneither(_tmp239,sizeof(char),10)));}
goto _LL38;_LL47: {struct Cyc_Absyn_Using_d_struct*_tmpC4=(struct Cyc_Absyn_Using_d_struct*)
_tmpAC;if(_tmpC4->tag != 11)goto _LL49;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;}}
_LL48:{const char*_tmp23A;unimp_msg_part=((_tmp23A="using",_tag_dyneither(_tmp23A,
sizeof(char),6)));}goto _LL38;_LL49: {struct Cyc_Absyn_Aggr_d_struct*_tmpC7=(
struct Cyc_Absyn_Aggr_d_struct*)_tmpAC;if(_tmpC7->tag != 6)goto _LL4B;}_LL4A:{const
char*_tmp23B;unimp_msg_part=((_tmp23B="type",_tag_dyneither(_tmp23B,sizeof(char),
5)));}goto _LL38;_LL4B: {struct Cyc_Absyn_Datatype_d_struct*_tmpC8=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpAC;if(_tmpC8->tag != 7)goto _LL4D;}_LL4C:{const char*_tmp23C;unimp_msg_part=((
_tmp23C="datatype",_tag_dyneither(_tmp23C,sizeof(char),9)));}goto _LL38;_LL4D: {
struct Cyc_Absyn_Enum_d_struct*_tmpC9=(struct Cyc_Absyn_Enum_d_struct*)_tmpAC;if(
_tmpC9->tag != 8)goto _LL4F;}_LL4E:{const char*_tmp23D;unimp_msg_part=((_tmp23D="enum",
_tag_dyneither(_tmp23D,sizeof(char),5)));}goto _LL38;_LL4F: {struct Cyc_Absyn_Typedef_d_struct*
_tmpCA=(struct Cyc_Absyn_Typedef_d_struct*)_tmpAC;if(_tmpCA->tag != 9)goto _LL51;}
_LL50:{const char*_tmp23E;unimp_msg_part=((_tmp23E="typedef",_tag_dyneither(
_tmp23E,sizeof(char),8)));}goto _LL38;_LL51: {struct Cyc_Absyn_ExternC_d_struct*
_tmpCB=(struct Cyc_Absyn_ExternC_d_struct*)_tmpAC;if(_tmpCB->tag != 12)goto _LL53;}
_LL52:{const char*_tmp23F;unimp_msg_part=((_tmp23F="extern \"C\"",_tag_dyneither(
_tmp23F,sizeof(char),11)));}goto _LL38;_LL53: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpCC=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpAC;if(_tmpCC->tag != 13)goto
_LL55;}_LL54:{const char*_tmp240;unimp_msg_part=((_tmp240="extern \"C include\"",
_tag_dyneither(_tmp240,sizeof(char),19)));}goto _LL38;_LL55: {struct Cyc_Absyn_Porton_d_struct*
_tmpCD=(struct Cyc_Absyn_Porton_d_struct*)_tmpAC;if(_tmpCD->tag != 14)goto _LL57;}
_LL56:{const char*_tmp241;unimp_msg_part=((_tmp241="__cyclone_port_on__",
_tag_dyneither(_tmp241,sizeof(char),20)));}goto _LL38;_LL57: {struct Cyc_Absyn_Portoff_d_struct*
_tmpCE=(struct Cyc_Absyn_Portoff_d_struct*)_tmpAC;if(_tmpCE->tag != 15)goto _LL38;}
_LL58:{const char*_tmp242;unimp_msg_part=((_tmp242="__cyclone_port_off__",
_tag_dyneither(_tmp242,sizeof(char),21)));}goto _LL38;_LL38:;};}{const char*
_tmp246;void*_tmp245[1];struct Cyc_String_pa_struct _tmp244;(int)_throw(((_tmp244.tag=
0,((_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((
_tmp245[0]=& _tmp244,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp246="tcStmt: nested %s declarations unimplemented",
_tag_dyneither(_tmp246,sizeof(char),45))),_tag_dyneither(_tmp245,sizeof(void*),1)))))))));};;
_pop_region(decl_rgn);}_LL28: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp52=(
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp18;if(_tmp52->tag != 16)goto _LL7;else{
_tmp53=_tmp52->f1;}}_LL29: {void*rgn_type=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_RgnHandleType_struct
_tmp249;struct Cyc_Absyn_RgnHandleType_struct*_tmp248;void*etype=(void*)((_tmp248=
_cycalloc(sizeof(*_tmp248)),((_tmp248[0]=((_tmp249.tag=15,((_tmp249.f1=(void*)
rgn_type,_tmp249)))),_tmp248))));if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,(void**)&
etype,_tmp53),etype)){const char*_tmp24D;void*_tmp24C[1];struct Cyc_String_pa_struct
_tmp24B;(_tmp24B.tag=0,((_tmp24B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp53->topt))->v)),((
_tmp24C[0]=& _tmp24B,Cyc_Tcutil_terr(_tmp53->loc,((_tmp24D="expecting region_t but found %s",
_tag_dyneither(_tmp24D,sizeof(char),32))),_tag_dyneither(_tmp24C,sizeof(void*),1)))))));}
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);return;}_LL7:;};}
