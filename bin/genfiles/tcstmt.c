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
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
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
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr
line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;};
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
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
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;struct
Cyc_Position_Segment*loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};
struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_struct{
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
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
topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{
int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_new_evar(struct
Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_exn_typ();void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_no_regions;
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
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
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,
struct Cyc_Absyn_Fndecl*new_fn);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
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
struct _tuple9{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const
struct Cyc_Tcenv_CList*f3;};const struct _tuple9*Cyc_Tcenv_process_fallthru(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);struct
Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_dyneither_ptr*,struct Cyc_Absyn_Stmt*);int Cyc_Tcenv_all_labels_resolved(struct
Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*
name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct _tuple10{struct Cyc_Absyn_Tvar*f1;void*f2;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple10**
oldtv,struct Cyc_Position_Segment*loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,void*r,int resetable,int opened);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_is_pointer_type(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple10*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int
allow_evars,void*);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);int Cyc_Tcutil_new_tvar_id();
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple11{void*f1;void*
f2;};struct Cyc_Tcexp_TestEnv{struct _tuple11*eq;int isTrue;};struct Cyc_Tcexp_TestEnv
Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct _tuple12{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple12*
tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple13 val;};struct _union_RelnOp_LessNumelts{
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
f1;int f2;void*f3;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{
int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct
_union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
int new_block);static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*
e){void*_tmp0=e->r;struct Cyc_Absyn_Exp*_tmp2;enum Cyc_Absyn_Incrementor _tmp3;
struct Cyc_Absyn_Exp*_tmp5;enum Cyc_Absyn_Incrementor _tmp6;_LL1: {struct Cyc_Absyn_Increment_e_struct*
_tmp1=(struct Cyc_Absyn_Increment_e_struct*)_tmp0;if(_tmp1->tag != 5)goto _LL3;
else{_tmp2=_tmp1->f1;_tmp3=_tmp1->f2;if(_tmp3 != Cyc_Absyn_PostInc)goto _LL3;}}
_LL2:{struct Cyc_Absyn_Increment_e_struct _tmp180;struct Cyc_Absyn_Increment_e_struct*
_tmp17F;e->r=(void*)((_tmp17F=_cycalloc(sizeof(*_tmp17F)),((_tmp17F[0]=((_tmp180.tag=
5,((_tmp180.f1=_tmp2,((_tmp180.f2=Cyc_Absyn_PreInc,_tmp180)))))),_tmp17F))));}
goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_struct*_tmp4=(struct Cyc_Absyn_Increment_e_struct*)
_tmp0;if(_tmp4->tag != 5)goto _LL5;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;if(_tmp6 != 
Cyc_Absyn_PostDec)goto _LL5;}}_LL4:{struct Cyc_Absyn_Increment_e_struct _tmp183;
struct Cyc_Absyn_Increment_e_struct*_tmp182;e->r=(void*)((_tmp182=_cycalloc(
sizeof(*_tmp182)),((_tmp182[0]=((_tmp183.tag=5,((_tmp183.f1=_tmp5,((_tmp183.f2=
Cyc_Absyn_PreDec,_tmp183)))))),_tmp182))));}goto _LL0;_LL5:;_LL6: goto _LL0;_LL0:;}
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));s->try_depth=
Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct _tuple12*_tmpC;struct Cyc_List_List*
_tmpD;struct Cyc_Tcpat_TcPatResult _tmpB=pat_res;_tmpC=_tmpB.tvars_and_bounds_opt;
_tmpD=_tmpB.patvars;{struct Cyc_List_List*_tmpE=_tmpC == 0?0:(*_tmpC).f1;struct Cyc_List_List*
_tmpF=_tmpC == 0?0:(*_tmpC).f2;struct _RegionHandle*_tmp10=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_tmp10,loc,te,_tmpE);te2=Cyc_Tcenv_new_outlives_constraints(
_tmp10,te2,_tmpF,loc);if(new_block)te2=Cyc_Tcenv_new_block(_tmp10,loc,te2);{
struct Cyc_Core_Opt*_tmp184;struct Cyc_Core_Opt*_tmp11=(_tmp184=_cycalloc(sizeof(*
_tmp184)),((_tmp184->v=Cyc_Tcenv_curr_rgn(te2),_tmp184)));{struct Cyc_List_List*
_tmp12=_tmpD;for(0;_tmp12 != 0;_tmp12=_tmp12->tl){te2=Cyc_Tcenv_add_pat_var(
_tmp10,loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp12->hd);((struct Cyc_Absyn_Vardecl*)
_tmp12->hd)->rgn=_tmp11;}}if(where_opt != 0){const char*_tmp185;Cyc_Tcexp_tcTest(
te2,(struct Cyc_Absyn_Exp*)where_opt,((_tmp185="switch clause guard",
_tag_dyneither(_tmp185,sizeof(char),20))));}if(_tmpD != 0)te2=Cyc_Tcenv_set_encloser(
_tmp10,te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);};}if(_tmpD != 0)Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));};}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*
x){if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{struct Cyc_List_List*
_tmp186;return(_tmp186=_region_malloc(r,sizeof(*_tmp186)),((_tmp186->hd=(void*)f(
env,(void*)x->hd),((_tmp186->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp186)))));}}
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s0,void*r1,void*r2,int new_block){struct _RegionHandle*_tmp16=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10*tk=0;struct Cyc_Tcenv_Tenv*_tmp17=Cyc_Tcenv_add_region_equality(
_tmp16,te,r1,r2,& tk,s0->loc);Cyc_Tcstmt_tcStmt(_tmp17,s0,new_block);if(tk != 0)((*((
struct _tuple10*)_check_null(tk))).f1)->kind=(*((struct _tuple10*)_check_null(tk))).f2;}
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int
new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmp18=s0->r;struct Cyc_Absyn_Exp*
_tmp1B;struct Cyc_Absyn_Stmt*_tmp1D;struct Cyc_Absyn_Stmt*_tmp1E;struct Cyc_Absyn_Exp*
_tmp20;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Stmt*_tmp23;struct Cyc_Absyn_Stmt*
_tmp24;struct _tuple8 _tmp26;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Stmt*
_tmp28;struct Cyc_Absyn_Stmt*_tmp29;struct Cyc_Absyn_Exp*_tmp2B;struct _tuple8
_tmp2C;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;struct _tuple8
_tmp2F;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_Absyn_Stmt*
_tmp32;struct Cyc_Absyn_Stmt*_tmp34;struct _tuple8 _tmp35;struct Cyc_Absyn_Exp*
_tmp36;struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Stmt**
_tmp3A;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Stmt**_tmp3D;struct
_dyneither_ptr*_tmp3F;struct Cyc_Absyn_Stmt*_tmp40;struct Cyc_Absyn_Stmt**_tmp41;
struct Cyc_List_List*_tmp43;struct Cyc_Absyn_Switch_clause**_tmp44;struct Cyc_Absyn_Switch_clause***
_tmp45;struct _dyneither_ptr*_tmp47;struct Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Exp*
_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_Stmt*_tmp4D;struct Cyc_List_List*
_tmp4E;struct Cyc_Absyn_Decl*_tmp50;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Exp*
_tmp53;_LL8: {struct Cyc_Absyn_Skip_s_struct*_tmp19=(struct Cyc_Absyn_Skip_s_struct*)
_tmp18;if(_tmp19->tag != 0)goto _LLA;}_LL9: return;_LLA: {struct Cyc_Absyn_Exp_s_struct*
_tmp1A=(struct Cyc_Absyn_Exp_s_struct*)_tmp18;if(_tmp1A->tag != 1)goto _LLC;else{
_tmp1B=_tmp1A->f1;}}_LLB: Cyc_Tcexp_tcExp(te,0,_tmp1B);if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp1B);return;_LLC: {struct Cyc_Absyn_Seq_s_struct*
_tmp1C=(struct Cyc_Absyn_Seq_s_struct*)_tmp18;if(_tmp1C->tag != 2)goto _LLE;else{
_tmp1D=_tmp1C->f1;_tmp1E=_tmp1C->f2;}}_LLD: {struct _RegionHandle*_tmp54=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Stmt_j_struct _tmp189;struct Cyc_Tcenv_Stmt_j_struct*_tmp188;
struct Cyc_Tcenv_Tenv*_tmp55=Cyc_Tcenv_set_next(_tmp54,te,(void*)((_tmp188=
_cycalloc(sizeof(*_tmp188)),((_tmp188[0]=((_tmp189.tag=3,((_tmp189.f1=_tmp1E,
_tmp189)))),_tmp188)))));Cyc_Tcstmt_tcStmt(_tmp55,_tmp1D,1);}Cyc_Tcstmt_tcStmt(
te,_tmp1E,1);return;}_LLE: {struct Cyc_Absyn_Return_s_struct*_tmp1F=(struct Cyc_Absyn_Return_s_struct*)
_tmp18;if(_tmp1F->tag != 3)goto _LL10;else{_tmp20=_tmp1F->f1;}}_LLF: {void*t=Cyc_Tcenv_return_typ(
te);if(_tmp20 == 0){void*_tmp58=Cyc_Tcutil_compress(t);_LL2B: {struct Cyc_Absyn_VoidType_struct*
_tmp59=(struct Cyc_Absyn_VoidType_struct*)_tmp58;if(_tmp59->tag != 0)goto _LL2D;}
_LL2C: goto _LL2A;_LL2D: {struct Cyc_Absyn_DoubleType_struct*_tmp5A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp58;if(_tmp5A->tag != 8)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*
_tmp5B=(struct Cyc_Absyn_FloatType_struct*)_tmp58;if(_tmp5B->tag != 7)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_IntType_struct*_tmp5C=(struct Cyc_Absyn_IntType_struct*)
_tmp58;if(_tmp5C->tag != 6)goto _LL33;}_LL32:{const char*_tmp18D;void*_tmp18C[1];
struct Cyc_String_pa_struct _tmp18B;(_tmp18B.tag=0,((_tmp18B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp18C[0]=&
_tmp18B,Cyc_Tcutil_warn(s0->loc,((_tmp18D="should return a value of type %s",
_tag_dyneither(_tmp18D,sizeof(char),33))),_tag_dyneither(_tmp18C,sizeof(void*),1)))))));}
goto _LL2A;_LL33:;_LL34:{const char*_tmp191;void*_tmp190[1];struct Cyc_String_pa_struct
_tmp18F;(_tmp18F.tag=0,((_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp190[0]=& _tmp18F,Cyc_Tcutil_terr(s0->loc,((
_tmp191="must return a value of type %s",_tag_dyneither(_tmp191,sizeof(char),31))),
_tag_dyneither(_tmp190,sizeof(void*),1)))))));}goto _LL2A;_LL2A:;}else{int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp20;Cyc_Tcexp_tcExp(te,(void**)& t,
e);if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){{const char*_tmp196;void*_tmp195[2];
struct Cyc_String_pa_struct _tmp194;struct Cyc_String_pa_struct _tmp193;(_tmp193.tag=
0,((_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp194.tag=0,((_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp195[0]=& _tmp194,((_tmp195[1]=& _tmp193,Cyc_Tcutil_terr(s0->loc,((_tmp196="returns value of type %s but requires %s",
_tag_dyneither(_tmp196,sizeof(char),41))),_tag_dyneither(_tmp195,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmp67=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp67,t) && !Cyc_Tcutil_is_noalias_path(
_tmp67,e)){const char*_tmp199;void*_tmp198;(_tmp198=0,Cyc_Tcutil_terr(e->loc,((
_tmp199="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp199,sizeof(char),49))),_tag_dyneither(_tmp198,sizeof(void*),0)));}};}return;}
_LL10: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp21=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp18;if(_tmp21->tag != 4)goto _LL12;else{_tmp22=_tmp21->f1;_tmp23=_tmp21->f2;
_tmp24=_tmp21->f3;}}_LL11: {struct _tuple11*_tmp6C;int _tmp6D;const char*_tmp19A;
struct Cyc_Tcexp_TestEnv _tmp6B=Cyc_Tcexp_tcTest(te,_tmp22,((_tmp19A="if statement",
_tag_dyneither(_tmp19A,sizeof(char),13))));_tmp6C=_tmp6B.eq;_tmp6D=_tmp6B.isTrue;{
struct _tuple11*_tmp6E=_tmp6C;struct _tuple11 _tmp6F;void*_tmp70;void*_tmp71;_LL36:
if(_tmp6E == 0)goto _LL38;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;_LL37:
if(_tmp6D){Cyc_Tcstmt_tcStmtRefine(te,_tmp23,_tmp70,_tmp71,1);Cyc_Tcstmt_tcStmt(
te,_tmp24,1);}else{Cyc_Tcstmt_tcStmt(te,_tmp23,1);Cyc_Tcstmt_tcStmtRefine(te,
_tmp24,_tmp70,_tmp71,1);}goto _LL35;_LL38:;_LL39: Cyc_Tcstmt_tcStmt(te,_tmp23,1);
Cyc_Tcstmt_tcStmt(te,_tmp24,1);goto _LL35;_LL35:;}return;}_LL12: {struct Cyc_Absyn_While_s_struct*
_tmp25=(struct Cyc_Absyn_While_s_struct*)_tmp18;if(_tmp25->tag != 5)goto _LL14;
else{_tmp26=_tmp25->f1;_tmp27=_tmp26.f1;_tmp28=_tmp26.f2;_tmp29=_tmp25->f2;}}
_LL13: Cyc_Tcstmt_decorate_stmt(te,_tmp28);{const char*_tmp19B;Cyc_Tcexp_tcTest(te,
_tmp27,((_tmp19B="while loop",_tag_dyneither(_tmp19B,sizeof(char),11))));}{
struct _RegionHandle*_tmp73=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
_tmp73,te,_tmp28),_tmp29,1);return;};_LL14: {struct Cyc_Absyn_For_s_struct*_tmp2A=(
struct Cyc_Absyn_For_s_struct*)_tmp18;if(_tmp2A->tag != 9)goto _LL16;else{_tmp2B=
_tmp2A->f1;_tmp2C=_tmp2A->f2;_tmp2D=_tmp2C.f1;_tmp2E=_tmp2C.f2;_tmp2F=_tmp2A->f3;
_tmp30=_tmp2F.f1;_tmp31=_tmp2F.f2;_tmp32=_tmp2A->f4;}}_LL15: Cyc_Tcstmt_decorate_stmt(
te,_tmp2E);Cyc_Tcstmt_decorate_stmt(te,_tmp31);Cyc_Tcexp_tcExp(te,0,_tmp2B);{
const char*_tmp19C;Cyc_Tcexp_tcTest(te,_tmp2D,((_tmp19C="for loop",_tag_dyneither(
_tmp19C,sizeof(char),9))));}{struct _RegionHandle*_tmp75=Cyc_Tcenv_get_fnrgn(te);{
struct Cyc_Tcenv_Tenv*_tmp76=Cyc_Tcenv_set_in_loop(_tmp75,te,_tmp31);Cyc_Tcstmt_tcStmt(
_tmp76,_tmp32,1);Cyc_Tcexp_tcExp(_tmp76,0,_tmp30);Cyc_Tcstmt_simplify_unused_result_exp(
_tmp30);}return;};_LL16: {struct Cyc_Absyn_Do_s_struct*_tmp33=(struct Cyc_Absyn_Do_s_struct*)
_tmp18;if(_tmp33->tag != 14)goto _LL18;else{_tmp34=_tmp33->f1;_tmp35=_tmp33->f2;
_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;}}_LL17: Cyc_Tcstmt_decorate_stmt(te,_tmp37);{
struct _RegionHandle*_tmp77=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
_tmp77,te,_tmp37),_tmp34,1);{const char*_tmp19D;Cyc_Tcexp_tcTest(te,_tmp36,((
_tmp19D="do loop",_tag_dyneither(_tmp19D,sizeof(char),8))));}return;};_LL18: {
struct Cyc_Absyn_Break_s_struct*_tmp38=(struct Cyc_Absyn_Break_s_struct*)_tmp18;
if(_tmp38->tag != 6)goto _LL1A;else{_tmp39=_tmp38->f1;_tmp3A=(struct Cyc_Absyn_Stmt**)&
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
_tuple9*_tmp79=Cyc_Tcenv_process_fallthru(te,s0,_tmp45);if(_tmp79 == (const struct
_tuple9*)0){{const char*_tmp1A0;void*_tmp19F;(_tmp19F=0,Cyc_Tcutil_terr(s0->loc,((
_tmp1A0="fallthru not in a non-last case",_tag_dyneither(_tmp1A0,sizeof(char),32))),
_tag_dyneither(_tmp19F,sizeof(void*),0)));}return;}{struct Cyc_List_List*_tmp7C=(*
_tmp79).f2;const struct Cyc_Tcenv_CList*_tmp7D=(*_tmp79).f3;struct Cyc_List_List*
instantiation=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp7C);struct Cyc_List_List*
_tmp7E=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,
void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(
Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp7D);for(0;_tmp7E != 
0  && _tmp43 != 0;(_tmp7E=_tmp7E->tl,_tmp43=_tmp43->tl)){int bogus=0;Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp43->hd);if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)
_tmp43->hd,(void*)_tmp7E->hd,& bogus)){{const char*_tmp1A5;void*_tmp1A4[2];struct
Cyc_String_pa_struct _tmp1A3;struct Cyc_String_pa_struct _tmp1A2;(_tmp1A2.tag=0,((
_tmp1A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp7E->hd)),((_tmp1A3.tag=0,((_tmp1A3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp43->hd)->topt))->v)),((_tmp1A4[0]=& _tmp1A3,((_tmp1A4[1]=&
_tmp1A2,Cyc_Tcutil_terr(s0->loc,((_tmp1A5="fallthru argument has type %s but pattern variable has type %s",
_tag_dyneither(_tmp1A5,sizeof(char),63))),_tag_dyneither(_tmp1A4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmp83=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp83,(void*)_tmp7E->hd) && !Cyc_Tcutil_is_noalias_path(
_tmp83,(struct Cyc_Absyn_Exp*)_tmp43->hd)){const char*_tmp1A8;void*_tmp1A7;(
_tmp1A7=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp43->hd)->loc,((_tmp1A8="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp1A8,sizeof(char),49))),_tag_dyneither(_tmp1A7,sizeof(void*),0)));}};}
if(_tmp43 != 0){const char*_tmp1AB;void*_tmp1AA;(_tmp1AA=0,Cyc_Tcutil_terr(s0->loc,((
_tmp1AB="too many arguments to explicit fallthru",_tag_dyneither(_tmp1AB,sizeof(
char),40))),_tag_dyneither(_tmp1AA,sizeof(void*),0)));}if(_tmp7E != 0){const char*
_tmp1AE;void*_tmp1AD;(_tmp1AD=0,Cyc_Tcutil_terr(s0->loc,((_tmp1AE="too few arguments to explicit fallthru",
_tag_dyneither(_tmp1AE,sizeof(char),39))),_tag_dyneither(_tmp1AD,sizeof(void*),0)));}
return;};}_LL20: {struct Cyc_Absyn_Label_s_struct*_tmp46=(struct Cyc_Absyn_Label_s_struct*)
_tmp18;if(_tmp46->tag != 13)goto _LL22;else{_tmp47=_tmp46->f1;_tmp48=_tmp46->f2;}}
_LL21: {struct _RegionHandle*_tmp8A=Cyc_Tcenv_get_fnrgn(te);{struct _dyneither_ptr*
_tmp1BB;const char*_tmp1BA;void*_tmp1B9[1];struct Cyc_String_pa_struct _tmp1B8;
struct Cyc_Absyn_Tvar*_tmp1B7;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(_tmp8A,
s0->loc,Cyc_Tcenv_add_label(te,_tmp47,_tmp48),((_tmp1B7=_cycalloc(sizeof(*
_tmp1B7)),((_tmp1B7->name=((_tmp1BB=_cycalloc(sizeof(*_tmp1BB)),((_tmp1BB[0]=(
struct _dyneither_ptr)((_tmp1B8.tag=0,((_tmp1B8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp47),((_tmp1B9[0]=& _tmp1B8,Cyc_aprintf(((_tmp1BA="`%s",
_tag_dyneither(_tmp1BA,sizeof(char),4))),_tag_dyneither(_tmp1B9,sizeof(void*),1)))))))),
_tmp1BB)))),((_tmp1B7->identity=Cyc_Tcutil_new_tvar_id(),((_tmp1B7->kind=Cyc_Tcutil_kind_to_bound(&
Cyc_Tcutil_rk),_tmp1B7))))))))),_tmp48,0);}return;}_LL22: {struct Cyc_Absyn_Switch_s_struct*
_tmp49=(struct Cyc_Absyn_Switch_s_struct*)_tmp18;if(_tmp49->tag != 10)goto _LL24;
else{_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;}}_LL23: Cyc_Tcexp_tcExp(te,0,_tmp4A);{
void*_tmp90=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A->topt))->v;struct
_RegionHandle*_tmp91=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp91,_tmp90) && !Cyc_Tcutil_is_noalias_path(_tmp91,_tmp4A)){const char*_tmp1BE;
void*_tmp1BD;(_tmp1BD=0,Cyc_Tcutil_terr(_tmp4A->loc,((_tmp1BE="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp1BE,sizeof(char),49))),_tag_dyneither(_tmp1BD,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmp94=Cyc_Tcenv_set_in_switch(_tmp91,te);_tmp94=Cyc_Tcenv_clear_fallthru(
_tmp91,_tmp94);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp4B);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmp95=((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp96=Cyc_Tcpat_tcPat(_tmp94,_tmp95,& _tmp90);struct Cyc_List_List*_tmp97=_tmp96.tvars_and_bounds_opt
== 0?0:(*_tmp96.tvars_and_bounds_opt).f1;if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp95->topt))->v,_tmp90)){{const char*_tmp1C3;void*_tmp1C2[2];struct
Cyc_String_pa_struct _tmp1C1;struct Cyc_String_pa_struct _tmp1C0;(_tmp1C0.tag=0,((
_tmp1C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp95->topt))->v)),((_tmp1C1.tag=0,((
_tmp1C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp90)),((_tmp1C2[0]=& _tmp1C1,((_tmp1C2[1]=& _tmp1C0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,((_tmp1C3="switch on type %s, but case expects type %s",
_tag_dyneither(_tmp1C3,sizeof(char),44))),_tag_dyneither(_tmp1C2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(_tmp94,_tmp95);}{
struct Cyc_Core_Opt*_tmp1C4;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((
_tmp1C4=_cycalloc(sizeof(*_tmp1C4)),((_tmp1C4->v=_tmp96.patvars,_tmp1C4))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp94,_tmp96,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp96.tvars_and_bounds_opt != 0  && (*_tmp96.tvars_and_bounds_opt).f2 != 0)_tmp94=
Cyc_Tcenv_clear_fallthru(_tmp91,_tmp94);else{_tmp94=Cyc_Tcenv_set_fallthru(
_tmp91,_tmp94,_tmp97,_tmp96.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp4B);return;};_LL24: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp4C=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp18;if(_tmp4C->tag != 15)goto _LL26;
else{_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;}}_LL25:{struct Cyc_Absyn_Seq_s_struct
_tmp1C7;struct Cyc_Absyn_Seq_s_struct*_tmp1C6;_tmp4D->r=(Cyc_Absyn_new_stmt((void*)((
_tmp1C6=_cycalloc(sizeof(*_tmp1C6)),((_tmp1C6[0]=((_tmp1C7.tag=2,((_tmp1C7.f1=
Cyc_Absyn_new_stmt(_tmp4D->r,_tmp4D->loc),((_tmp1C7.f2=Cyc_Absyn_skip_stmt(
_tmp4D->loc),_tmp1C7)))))),_tmp1C6)))),_tmp4D->loc))->r;}{struct _RegionHandle*
_tmp9F=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp9F,
Cyc_Tcenv_enter_try(_tmp9F,te),s0),_tmp4D,1);{struct _RegionHandle*_tmpA0=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA1=Cyc_Tcenv_set_in_switch(_tmpA0,te);_tmpA1=Cyc_Tcenv_clear_fallthru(
_tmpA0,_tmpA1);{struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp4E);for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){struct Cyc_Absyn_Pat*
_tmpA3=((struct Cyc_Absyn_Switch_clause*)_tmpA2->hd)->pattern;void*_tmpA4=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmpA5=Cyc_Tcpat_tcPat(_tmpA1,_tmpA3,& _tmpA4);struct
Cyc_List_List*_tmpA6=_tmpA5.tvars_and_bounds_opt == 0?0:(*_tmpA5.tvars_and_bounds_opt).f1;
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA3->topt))->v,
_tmpA4)){const char*_tmp1CB;void*_tmp1CA[1];struct Cyc_String_pa_struct _tmp1C9;(
_tmp1C9.tag=0,((_tmp1C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA3->topt))->v)),((_tmp1CA[0]=&
_tmp1C9,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpA2->hd)->loc,((
_tmp1CB="expected datatype exn but found %s",_tag_dyneither(_tmp1CB,sizeof(char),
35))),_tag_dyneither(_tmp1CA,sizeof(void*),1)))))));}else{Cyc_Tcpat_check_pat_regions(
_tmpA1,_tmpA3);}{struct Cyc_Core_Opt*_tmp1CC;((struct Cyc_Absyn_Switch_clause*)
_tmpA2->hd)->pat_vars=((_tmp1CC=_cycalloc(sizeof(*_tmp1CC)),((_tmp1CC->v=_tmpA5.patvars,
_tmp1CC))));}Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmpA2->hd)->loc,
_tmpA1,_tmpA5,((struct Cyc_Absyn_Switch_clause*)_tmpA2->hd)->body,((struct Cyc_Absyn_Switch_clause*)
_tmpA2->hd)->where_clause,1);if(_tmpA5.tvars_and_bounds_opt != 0  && (*_tmpA5.tvars_and_bounds_opt).f2
!= 0)_tmpA1=Cyc_Tcenv_clear_fallthru(_tmpA0,_tmpA1);else{_tmpA1=Cyc_Tcenv_set_fallthru(
_tmpA0,_tmpA1,_tmpA6,_tmpA5.patvars,(struct Cyc_Absyn_Switch_clause*)_tmpA2->hd);}}};}
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp4E);return;};};_LL26: {struct Cyc_Absyn_Decl_s_struct*
_tmp4F=(struct Cyc_Absyn_Decl_s_struct*)_tmp18;if(_tmp4F->tag != 12)goto _LL28;
else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL27: {struct _dyneither_ptr
unimp_msg_part;struct _RegionHandle*_tmpAB=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcenv_Tenv*
te2;if(new_block)te2=Cyc_Tcenv_new_block(_tmpAB,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(
_tmpAB,te);}{struct Cyc_Tcenv_Tenv*_tmpAC=te2;void*_tmpAD=_tmp50->r;struct Cyc_Absyn_Vardecl*
_tmpAF;struct Cyc_Absyn_Pat*_tmpB1;struct Cyc_Core_Opt*_tmpB2;struct Cyc_Core_Opt**
_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_List_List*_tmpB6;struct Cyc_Absyn_Tvar*
_tmpB8;struct Cyc_Absyn_Vardecl*_tmpB9;int _tmpBA;struct Cyc_Absyn_Exp*_tmpBB;
struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Tvar*_tmpBE;struct Cyc_Absyn_Vardecl*
_tmpBF;struct Cyc_Absyn_Fndecl*_tmpC1;struct _dyneither_ptr*_tmpC3;struct Cyc_List_List*
_tmpC4;struct _tuple0*_tmpC6;struct Cyc_List_List*_tmpC7;_LL3B: {struct Cyc_Absyn_Var_d_struct*
_tmpAE=(struct Cyc_Absyn_Var_d_struct*)_tmpAD;if(_tmpAE->tag != 0)goto _LL3D;else{
_tmpAF=_tmpAE->f1;}}_LL3C: {struct Cyc_Absyn_Vardecl _tmpD1;enum Cyc_Absyn_Scope
_tmpD2;struct _tuple0*_tmpD3;struct _tuple0 _tmpD4;union Cyc_Absyn_Nmspace _tmpD5;
struct _dyneither_ptr*_tmpD6;struct Cyc_Absyn_Tqual _tmpD7;void*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;struct Cyc_Core_Opt*_tmpDA;struct Cyc_Core_Opt**_tmpDB;struct Cyc_List_List*
_tmpDC;struct Cyc_Absyn_Vardecl*_tmpD0=_tmpAF;_tmpD1=*_tmpD0;_tmpD2=_tmpD1.sc;
_tmpD3=_tmpD1.name;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;_tmpD7=
_tmpD1.tq;_tmpD8=_tmpD1.type;_tmpD9=_tmpD1.initializer;_tmpDA=_tmpD1.rgn;_tmpDB=(
struct Cyc_Core_Opt**)&(*_tmpD0).rgn;_tmpDC=_tmpD1.attributes;{void*_tmpDD=Cyc_Tcenv_curr_rgn(
_tmpAC);int is_local;switch(_tmpD2){case Cyc_Absyn_Static: _LL5B: goto _LL5C;case Cyc_Absyn_Extern:
_LL5C: goto _LL5D;case Cyc_Absyn_ExternC: _LL5D: is_local=0;break;case Cyc_Absyn_Abstract:
_LL5E:{const char*_tmp1CF;void*_tmp1CE;(_tmp1CE=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1CF="bad abstract scope for local variable",_tag_dyneither(_tmp1CF,sizeof(
char),38))),_tag_dyneither(_tmp1CE,sizeof(void*),0)));}goto _LL5F;case Cyc_Absyn_Register:
_LL5F: goto _LL60;case Cyc_Absyn_Public: _LL60: is_local=1;break;}{struct Cyc_Core_Opt*
_tmp1D1;struct Cyc_Core_Opt*_tmp1D0;*_tmpDB=is_local?(_tmp1D1=_cycalloc(sizeof(*
_tmp1D1)),((_tmp1D1->v=_tmpDD,_tmp1D1))):((_tmp1D0=_cycalloc(sizeof(*_tmp1D0)),((
_tmp1D0->v=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1D0))));}{union Cyc_Absyn_Nmspace
_tmpE2=_tmpD5;int _tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE5;
struct Cyc_List_List*_tmpE6;_LL63: if((_tmpE2.Loc_n).tag != 4)goto _LL65;_tmpE3=(int)(
_tmpE2.Loc_n).val;_LL64: goto _LL62;_LL65: if((_tmpE2.Rel_n).tag != 1)goto _LL67;
_tmpE4=(struct Cyc_List_List*)(_tmpE2.Rel_n).val;if(_tmpE4 != 0)goto _LL67;_LL66:(*
_tmpAF->name).f1=Cyc_Absyn_Loc_n;goto _LL62;_LL67: if((_tmpE2.C_n).tag != 3)goto
_LL69;_tmpE5=(struct Cyc_List_List*)(_tmpE2.C_n).val;_LL68: {const char*_tmp1D4;
void*_tmp1D3;(int)_throw(((_tmp1D3=0,((void*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1D4="tcstmt: C_n declaration",
_tag_dyneither(_tmp1D4,sizeof(char),24))),_tag_dyneither(_tmp1D3,sizeof(void*),0)))));}
_LL69: if((_tmpE2.Abs_n).tag != 2)goto _LL6B;_tmpE6=(struct Cyc_List_List*)(_tmpE2.Abs_n).val;
_LL6A: {const char*_tmp1D7;void*_tmp1D6;(int)_throw(((_tmp1D6=0,((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1D7="tcstmt: Abs_n declaration",
_tag_dyneither(_tmp1D7,sizeof(char),26))),_tag_dyneither(_tmp1D6,sizeof(void*),0)))));}
_LL6B:;_LL6C:{const char*_tmp1DA;void*_tmp1D9;(_tmp1D9=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1DA="cannot declare a qualified local variable",_tag_dyneither(_tmp1DA,
sizeof(char),42))),_tag_dyneither(_tmp1D9,sizeof(void*),0)));}goto _LL62;_LL62:;}{
void*_tmpED=Cyc_Tcutil_compress(_tmpD8);struct Cyc_Absyn_ArrayInfo _tmpEF;void*
_tmpF0;struct Cyc_Absyn_Tqual _tmpF1;struct Cyc_Absyn_Exp*_tmpF2;union Cyc_Absyn_Constraint*
_tmpF3;struct Cyc_Position_Segment*_tmpF4;_LL6E:{struct Cyc_Absyn_ArrayType_struct*
_tmpEE=(struct Cyc_Absyn_ArrayType_struct*)_tmpED;if(_tmpEE->tag != 9)goto _LL70;
else{_tmpEF=_tmpEE->f1;_tmpF0=_tmpEF.elt_type;_tmpF1=_tmpEF.tq;_tmpF2=_tmpEF.num_elts;
if(_tmpF2 != 0)goto _LL70;_tmpF3=_tmpEF.zero_term;_tmpF4=_tmpEF.zt_loc;}}if(!(
_tmpAF->initializer != 0))goto _LL70;_LL6F:{void*_tmpF5=((struct Cyc_Absyn_Exp*)
_check_null(_tmpAF->initializer))->r;union Cyc_Absyn_Cnst _tmpF7;struct
_dyneither_ptr _tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_List_List*_tmpFC;
struct Cyc_List_List*_tmpFE;_LL73: {struct Cyc_Absyn_Const_e_struct*_tmpF6=(struct
Cyc_Absyn_Const_e_struct*)_tmpF5;if(_tmpF6->tag != 0)goto _LL75;else{_tmpF7=_tmpF6->f1;
if((_tmpF7.String_c).tag != 7)goto _LL75;_tmpF8=(struct _dyneither_ptr)(_tmpF7.String_c).val;}}
_LL74: _tmpD8=(_tmpAF->type=Cyc_Absyn_array_typ(_tmpF0,_tmpF1,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpF8,sizeof(char)),0),_tmpF3,_tmpF4));
goto _LL72;_LL75: {struct Cyc_Absyn_Comprehension_e_struct*_tmpF9=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmpF5;if(_tmpF9->tag != 28)goto _LL77;else{_tmpFA=_tmpF9->f2;}}_LL76: _tmpD8=(
_tmpAF->type=Cyc_Absyn_array_typ(_tmpF0,_tmpF1,(struct Cyc_Absyn_Exp*)_tmpFA,
_tmpF3,_tmpF4));goto _LL72;_LL77: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpFB=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpF5;if(_tmpFB->tag != 36)goto _LL79;
else{_tmpFC=_tmpFB->f2;}}_LL78: _tmpFE=_tmpFC;goto _LL7A;_LL79: {struct Cyc_Absyn_Array_e_struct*
_tmpFD=(struct Cyc_Absyn_Array_e_struct*)_tmpF5;if(_tmpFD->tag != 27)goto _LL7B;
else{_tmpFE=_tmpFD->f1;}}_LL7A: _tmpD8=(_tmpAF->type=Cyc_Absyn_array_typ(_tmpF0,
_tmpF1,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpFE),0),_tmpF3,_tmpF4));goto _LL72;_LL7B:;_LL7C: goto _LL72;
_LL72:;}goto _LL6D;_LL70:;_LL71: goto _LL6D;_LL6D:;}{struct Cyc_List_List*_tmpFF=!
is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpAC);int _tmp100=!is_local?0: 1;Cyc_Tcutil_check_type(
s0->loc,_tmpAC,_tmpFF,& Cyc_Tcutil_tmk,_tmp100,_tmpD8);{struct Cyc_Tcenv_Tenv*
_tmp101=Cyc_Tcenv_add_local_var(_tmpAB,_tmp51->loc,_tmpAC,_tmpAF);if(_tmpD2 == 
Cyc_Absyn_Extern  || _tmpD2 == Cyc_Absyn_ExternC){const char*_tmp1DD;void*_tmp1DC;(
_tmp1DC=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1DD="local extern declarations not yet supported",
_tag_dyneither(_tmp1DD,sizeof(char),44))),_tag_dyneither(_tmp1DC,sizeof(void*),0)));}
if(_tmpD9 != 0){Cyc_Tcexp_tcExpInitializer(_tmp101,(void**)& _tmpD8,(struct Cyc_Absyn_Exp*)
_tmpD9);if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpAC,(struct Cyc_Absyn_Exp*)
_tmpD9)){const char*_tmp1E0;void*_tmp1DF;(_tmp1DF=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1E0="initializer needs to be a constant expression",_tag_dyneither(_tmp1E0,
sizeof(char),46))),_tag_dyneither(_tmp1DF,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(
_tmp101,(struct Cyc_Absyn_Exp*)_tmpD9,_tmpD8)){struct _dyneither_ptr _tmp106=*
_tmpD6;struct _dyneither_ptr _tmp107=Cyc_Absynpp_typ2string(_tmpD8);struct
_dyneither_ptr _tmp108=Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD9->topt))->v);if(((_get_dyneither_size(_tmp106,sizeof(char))+ 
_get_dyneither_size(_tmp107,sizeof(char)))+ _get_dyneither_size(_tmp108,sizeof(
char)))+ 50 < 80){const char*_tmp1E6;void*_tmp1E5[3];struct Cyc_String_pa_struct
_tmp1E4;struct Cyc_String_pa_struct _tmp1E3;struct Cyc_String_pa_struct _tmp1E2;(
_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108),((
_tmp1E3.tag=0,((_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107),((
_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((
_tmp1E5[0]=& _tmp1E4,((_tmp1E5[1]=& _tmp1E3,((_tmp1E5[2]=& _tmp1E2,Cyc_Tcutil_terr(
_tmp50->loc,((_tmp1E6="%s declared with type %s but initialized with type %s.",
_tag_dyneither(_tmp1E6,sizeof(char),55))),_tag_dyneither(_tmp1E5,sizeof(void*),3)))))))))))))))))));}
else{if((_get_dyneither_size(_tmp106,sizeof(char))+ _get_dyneither_size(_tmp107,
sizeof(char)))+ 25 < 80  && _get_dyneither_size(_tmp108,sizeof(char))+ 25 < 80){
const char*_tmp1EC;void*_tmp1EB[3];struct Cyc_String_pa_struct _tmp1EA;struct Cyc_String_pa_struct
_tmp1E9;struct Cyc_String_pa_struct _tmp1E8;(_tmp1E8.tag=0,((_tmp1E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp108),((_tmp1E9.tag=0,((_tmp1E9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107),((_tmp1EA.tag=0,((_tmp1EA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((_tmp1EB[0]=& _tmp1EA,((
_tmp1EB[1]=& _tmp1E9,((_tmp1EB[2]=& _tmp1E8,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1EC="%s declared with type %s\n but initialized with type %s.",
_tag_dyneither(_tmp1EC,sizeof(char),56))),_tag_dyneither(_tmp1EB,sizeof(void*),3)))))))))))))))))));}
else{const char*_tmp1F2;void*_tmp1F1[3];struct Cyc_String_pa_struct _tmp1F0;struct
Cyc_String_pa_struct _tmp1EF;struct Cyc_String_pa_struct _tmp1EE;(_tmp1EE.tag=0,((
_tmp1EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108),((_tmp1EF.tag=0,((
_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107),((_tmp1F0.tag=0,((
_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp106),((_tmp1F1[0]=&
_tmp1F0,((_tmp1F1[1]=& _tmp1EF,((_tmp1F1[2]=& _tmp1EE,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1F2="%s declared with type \n%s\n but initialized with type \n%s.",
_tag_dyneither(_tmp1F2,sizeof(char),58))),_tag_dyneither(_tmp1F1,sizeof(void*),3)))))))))))))))))));}}
Cyc_Tcutil_unify(_tmpD8,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD9->topt))->v);
Cyc_Tcutil_explain_failure();}}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpAB,
_tmp101,s0),_tmp51,0);return;};};};}_LL3D: {struct Cyc_Absyn_Let_d_struct*_tmpB0=(
struct Cyc_Absyn_Let_d_struct*)_tmpAD;if(_tmpB0->tag != 2)goto _LL3F;else{_tmpB1=
_tmpB0->f1;_tmpB2=_tmpB0->f2;_tmpB3=(struct Cyc_Core_Opt**)& _tmpB0->f2;_tmpB4=
_tmpB0->f3;}}_LL3E: Cyc_Tcexp_tcExpInitializer(_tmpAC,0,_tmpB4);{struct Cyc_Tcpat_TcPatResult
_tmp118=Cyc_Tcpat_tcPat(_tmpAC,_tmpB1,(void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(_tmpB4->topt))->v));{struct Cyc_Core_Opt*_tmp1F3;*_tmpB3=((_tmp1F3=
_cycalloc(sizeof(*_tmp1F3)),((_tmp1F3->v=_tmp118.patvars,_tmp1F3))));}if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB1->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpB4->topt))->v) && !Cyc_Tcutil_coerce_assign(_tmpAC,_tmpB4,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpB1->topt))->v)){{const char*_tmp1F8;void*
_tmp1F7[2];struct Cyc_String_pa_struct _tmp1F6;struct Cyc_String_pa_struct _tmp1F5;(
_tmp1F5.tag=0,((_tmp1F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB4->topt))->v)),((_tmp1F6.tag=0,((
_tmp1F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB1->topt))->v)),((_tmp1F7[0]=&
_tmp1F6,((_tmp1F7[1]=& _tmp1F5,Cyc_Tcutil_terr(_tmp50->loc,((_tmp1F8="pattern type %s does not match definition type %s",
_tag_dyneither(_tmp1F8,sizeof(char),50))),_tag_dyneither(_tmp1F7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpB1->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpB4->topt))->v);Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmpAC,_tmpB1);}Cyc_Tcpat_check_let_pat_exhaustive(
_tmpB1->loc,_tmpAC,_tmpB1);Cyc_Tcstmt_pattern_synth(s0->loc,_tmpAC,_tmp118,
_tmp51,0,0);return;};_LL3F: {struct Cyc_Absyn_Letv_d_struct*_tmpB5=(struct Cyc_Absyn_Letv_d_struct*)
_tmpAD;if(_tmpB5->tag != 3)goto _LL41;else{_tmpB6=_tmpB5->f1;}}_LL40: {void*
_tmp11E=Cyc_Tcenv_curr_rgn(_tmpAC);struct Cyc_Tcenv_Tenv*_tmp11F=_tmpAC;for(0;
_tmpB6 != 0;_tmpB6=_tmpB6->tl){struct Cyc_Absyn_Vardecl*_tmp120=(struct Cyc_Absyn_Vardecl*)
_tmpB6->hd;struct Cyc_Absyn_Vardecl _tmp122;struct _tuple0*_tmp123;struct _tuple0
_tmp124;union Cyc_Absyn_Nmspace _tmp125;void*_tmp126;struct Cyc_Core_Opt*_tmp127;
struct Cyc_Core_Opt**_tmp128;struct Cyc_Absyn_Vardecl*_tmp121=_tmp120;_tmp122=*
_tmp121;_tmp123=_tmp122.name;_tmp124=*_tmp123;_tmp125=_tmp124.f1;_tmp126=_tmp122.type;
_tmp127=_tmp122.rgn;_tmp128=(struct Cyc_Core_Opt**)&(*_tmp121).rgn;{struct Cyc_Core_Opt*
_tmp1F9;*_tmp128=((_tmp1F9=_cycalloc(sizeof(*_tmp1F9)),((_tmp1F9->v=_tmp11E,
_tmp1F9))));}{union Cyc_Absyn_Nmspace _tmp12A=_tmp125;int _tmp12B;struct Cyc_List_List*
_tmp12C;struct Cyc_List_List*_tmp12D;_LL7E: if((_tmp12A.Loc_n).tag != 4)goto _LL80;
_tmp12B=(int)(_tmp12A.Loc_n).val;_LL7F: goto _LL7D;_LL80: if((_tmp12A.Rel_n).tag != 
1)goto _LL82;_tmp12C=(struct Cyc_List_List*)(_tmp12A.Rel_n).val;if(_tmp12C != 0)
goto _LL82;_LL81:(*_tmp120->name).f1=Cyc_Absyn_Loc_n;goto _LL7D;_LL82: if((_tmp12A.Abs_n).tag
!= 2)goto _LL84;_tmp12D=(struct Cyc_List_List*)(_tmp12A.Abs_n).val;_LL83: {const
char*_tmp1FC;void*_tmp1FB;(int)_throw(((_tmp1FB=0,((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1FC="tcstmt: Abs_n declaration",
_tag_dyneither(_tmp1FC,sizeof(char),26))),_tag_dyneither(_tmp1FB,sizeof(void*),0)))));}
_LL84:;_LL85:{const char*_tmp1FF;void*_tmp1FE;(_tmp1FE=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp1FF="cannot declare a qualified local variable",_tag_dyneither(_tmp1FF,
sizeof(char),42))),_tag_dyneither(_tmp1FE,sizeof(void*),0)));}goto _LL7D;_LL7D:;}
Cyc_Tcutil_check_type(s0->loc,_tmp11F,Cyc_Tcenv_lookup_type_vars(_tmp11F),& Cyc_Tcutil_tmk,
1,_tmp126);_tmp11F=Cyc_Tcenv_add_local_var(_tmpAB,_tmp51->loc,_tmp11F,_tmp120);}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmpAB,_tmp11F,s0),_tmp51,0);return;}
_LL41: {struct Cyc_Absyn_Region_d_struct*_tmpB7=(struct Cyc_Absyn_Region_d_struct*)
_tmpAD;if(_tmpB7->tag != 4)goto _LL43;else{_tmpB8=_tmpB7->f1;_tmpB9=_tmpB7->f2;
_tmpBA=_tmpB7->f3;_tmpBB=_tmpB7->f4;}}_LL42:{struct Cyc_Absyn_Seq_s_struct _tmp202;
struct Cyc_Absyn_Seq_s_struct*_tmp201;_tmp51->r=(Cyc_Absyn_new_stmt((void*)((
_tmp201=_cycalloc(sizeof(*_tmp201)),((_tmp201[0]=((_tmp202.tag=2,((_tmp202.f1=
Cyc_Absyn_new_stmt(_tmp51->r,_tmp51->loc),((_tmp202.f2=Cyc_Absyn_skip_stmt(
_tmp51->loc),_tmp202)))))),_tmp201)))),_tmp51->loc))->r;}{struct Cyc_Tcenv_Tenv*
_tmp134=_tmpAC;struct Cyc_Absyn_Vardecl _tmp136;void*_tmp137;void**_tmp138;struct
Cyc_Core_Opt*_tmp139;struct Cyc_Core_Opt**_tmp13A;struct Cyc_Absyn_Vardecl*_tmp135=
_tmpB9;_tmp136=*_tmp135;_tmp137=_tmp136.type;_tmp138=(void**)&(*_tmp135).type;
_tmp139=_tmp136.rgn;_tmp13A=(struct Cyc_Core_Opt**)&(*_tmp135).rgn;{void*_tmp13B=
Cyc_Tcenv_curr_rgn(_tmpAC);{struct Cyc_Core_Opt*_tmp203;*_tmp13A=((_tmp203=
_cycalloc(sizeof(*_tmp203)),((_tmp203->v=_tmp13B,_tmp203))));}{void*rgn_typ;if(
_tmpBB != 0){struct Cyc_Absyn_Exp*open_exp=(struct Cyc_Absyn_Exp*)_tmpBB;struct Cyc_List_List*
_tmp13D=Cyc_Tcenv_lookup_type_vars(_tmpAC);{struct Cyc_Core_Opt*_tmp204;rgn_typ=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,((_tmp204=_cycalloc(
sizeof(*_tmp204)),((_tmp204->v=_tmp13D,_tmp204)))));}{struct Cyc_Core_Opt*_tmp205;
void*rgn2_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,((_tmp205=
_cycalloc(sizeof(*_tmp205)),((_tmp205->v=_tmp13D,_tmp205)))));struct Cyc_Absyn_DynRgnType_struct
_tmp208;struct Cyc_Absyn_DynRgnType_struct*_tmp207;void*expected=(void*)((_tmp207=
_cycalloc(sizeof(*_tmp207)),((_tmp207[0]=((_tmp208.tag=17,((_tmp208.f1=(void*)
rgn_typ,((_tmp208.f2=(void*)rgn2_typ,_tmp208)))))),_tmp207))));{struct Cyc_Absyn_RgnHandleType_struct
_tmp20B;struct Cyc_Absyn_RgnHandleType_struct*_tmp20A;*_tmp138=(void*)((_tmp20A=
_cycalloc(sizeof(*_tmp20A)),((_tmp20A[0]=((_tmp20B.tag=16,((_tmp20B.f1=(void*)
rgn_typ,_tmp20B)))),_tmp20A))));}if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(_tmpAC,(
void**)& expected,open_exp),expected)){const char*_tmp210;void*_tmp20F[2];struct
Cyc_String_pa_struct _tmp20E;struct Cyc_String_pa_struct _tmp20D;(_tmp20D.tag=0,((
_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(open_exp->topt))->v)),((_tmp20E.tag=0,((
_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
expected)),((_tmp20F[0]=& _tmp20E,((_tmp20F[1]=& _tmp20D,Cyc_Tcutil_terr(open_exp->loc,((
_tmp210="expecting %s but found %s",_tag_dyneither(_tmp210,sizeof(char),26))),
_tag_dyneither(_tmp20F,sizeof(void*),2)))))))))))));}Cyc_Tcenv_check_rgn_accessible(
_tmpAC,open_exp->loc,rgn2_typ);_tmp134=Cyc_Tcenv_add_region(_tmpAB,_tmp134,
rgn_typ,0,1);};}else{{struct Cyc_Absyn_VarType_struct _tmp213;struct Cyc_Absyn_VarType_struct*
_tmp212;rgn_typ=(void*)((_tmp212=_cycalloc(sizeof(*_tmp212)),((_tmp212[0]=((
_tmp213.tag=2,((_tmp213.f1=_tmpB8,_tmp213)))),_tmp212))));}{struct Cyc_List_List*
_tmp214;_tmp134=Cyc_Tcenv_add_type_vars(_tmpAB,s0->loc,_tmp134,((_tmp214=
_cycalloc(sizeof(*_tmp214)),((_tmp214->hd=_tmpB8,((_tmp214->tl=0,_tmp214)))))));}
_tmp134=Cyc_Tcenv_add_region(_tmpAB,_tmp134,rgn_typ,_tmpBA,1);}Cyc_Tcutil_check_type(
s0->loc,_tmpAC,Cyc_Tcenv_lookup_type_vars(_tmp134),& Cyc_Tcutil_bk,1,*_tmp138);{
struct Cyc_Absyn_RgnHandleType_struct _tmp217;struct Cyc_Absyn_RgnHandleType_struct*
_tmp216;if(!Cyc_Tcutil_unify(*_tmp138,(void*)((_tmp216=_cycalloc(sizeof(*_tmp216)),((
_tmp216[0]=((_tmp217.tag=16,((_tmp217.f1=(void*)rgn_typ,_tmp217)))),_tmp216)))))){
const char*_tmp21A;void*_tmp219;(_tmp219=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp21A="region stmt: type of region handle is wrong!",
_tag_dyneither(_tmp21A,sizeof(char),45))),_tag_dyneither(_tmp219,sizeof(void*),0)));}}
if(!Cyc_Absyn_no_regions)_tmp134=Cyc_Tcenv_enter_try(_tmpAB,_tmp134);Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(_tmpAB,Cyc_Tcenv_add_local_var(_tmpAB,_tmp51->loc,_tmp134,
_tmpB9),s0),_tmp51,0);return;};};};_LL43: {struct Cyc_Absyn_Alias_d_struct*_tmpBC=(
struct Cyc_Absyn_Alias_d_struct*)_tmpAD;if(_tmpBC->tag != 5)goto _LL45;else{_tmpBD=
_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL44: {struct _RegionHandle*
_tmp14F=Cyc_Tcenv_get_fnrgn(_tmpAC);{struct Cyc_Absyn_Tvar*_tmp21B[1];struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_add_type_vars(_tmp14F,s0->loc,_tmpAC,((_tmp21B[0]=_tmpBE,((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp14F,_tag_dyneither(_tmp21B,sizeof(struct Cyc_Absyn_Tvar*),1)))));{struct Cyc_Absyn_VarType_struct
_tmp21E;struct Cyc_Absyn_VarType_struct*_tmp21D;te2=Cyc_Tcenv_add_region(_tmp14F,
te2,(void*)((_tmp21D=_cycalloc(sizeof(*_tmp21D)),((_tmp21D[0]=((_tmp21E.tag=2,((
_tmp21E.f1=_tmpBE,_tmp21E)))),_tmp21D)))),0,1);}Cyc_Tcexp_tcExpInitializer(te2,0,
_tmpBD);if(!Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpBD->topt))->v)){{const char*_tmp222;void*_tmp221[1];struct Cyc_String_pa_struct
_tmp220;(_tmp220.tag=0,((_tmp220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmpBD->topt))->v)),((
_tmp221[0]=& _tmp220,Cyc_Tcutil_terr(_tmpBD->loc,((_tmp222="%s is not a pointer type",
_tag_dyneither(_tmp222,sizeof(char),25))),_tag_dyneither(_tmp221,sizeof(void*),1)))))));}
return;}_tmpBF->type=Cyc_Tcutil_copy_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpBD->topt))->v);Cyc_Tcutil_check_type(_tmpBD->loc,_tmpAC,Cyc_Tcenv_lookup_type_vars(
te2),& Cyc_Tcutil_mk,1,_tmpBF->type);{struct Cyc_Core_Opt*_tmp223;_tmpBF->rgn=((
_tmp223=_cycalloc(sizeof(*_tmp223)),((_tmp223->v=Cyc_Tcenv_curr_rgn(te2),_tmp223))));}
te2=Cyc_Tcenv_add_local_var(_tmp14F,s0->loc,te2,_tmpBF);te2=Cyc_Tcenv_set_encloser(
_tmp14F,te2,_tmp51);{struct Cyc_List_List*_tmp156=Cyc_Tcenv_lookup_type_vars(te2);
Cyc_Tcstmt_tcStmt(te2,_tmp51,0);};}Cyc_NewControlFlow_set_encloser(s0,Cyc_Tcenv_get_encloser(
_tmpAC));return;}_LL45: {struct Cyc_Absyn_Fn_d_struct*_tmpC0=(struct Cyc_Absyn_Fn_d_struct*)
_tmpAD;if(_tmpC0->tag != 1)goto _LL47;else{_tmpC1=_tmpC0->f1;}}_LL46: {void*
_tmp158=Cyc_Tcenv_curr_rgn(_tmpAC);if(_tmpC1->sc != Cyc_Absyn_Public){const char*
_tmp226;void*_tmp225;(_tmp225=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp226="bad storage class for inner function",
_tag_dyneither(_tmp226,sizeof(char),37))),_tag_dyneither(_tmp225,sizeof(void*),0)));}{
union Cyc_Absyn_Nmspace _tmp15B=(*_tmpC1->name).f1;struct Cyc_List_List*_tmp15C;
struct Cyc_List_List*_tmp15D;_LL87: if((_tmp15B.Rel_n).tag != 1)goto _LL89;_tmp15C=(
struct Cyc_List_List*)(_tmp15B.Rel_n).val;if(_tmp15C != 0)goto _LL89;_LL88: goto
_LL86;_LL89: if((_tmp15B.Abs_n).tag != 2)goto _LL8B;_tmp15D=(struct Cyc_List_List*)(
_tmp15B.Abs_n).val;_LL8A: {const char*_tmp229;void*_tmp228;(_tmp228=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp229="tc: Abs_n in tcStmt var decl",
_tag_dyneither(_tmp229,sizeof(char),29))),_tag_dyneither(_tmp228,sizeof(void*),0)));}
_LL8B:;_LL8C: {const char*_tmp22C;void*_tmp22B;(_tmp22B=0,Cyc_Tcutil_terr(_tmp50->loc,((
_tmp22C="explicit namespace not allowed on inner function declaration",
_tag_dyneither(_tmp22C,sizeof(char),61))),_tag_dyneither(_tmp22B,sizeof(void*),0)));}
_LL86:;}(*_tmpC1->name).f1=Cyc_Absyn_Loc_n;Cyc_Tcutil_check_fndecl_valid_type(
_tmp50->loc,_tmpAC,_tmpC1);{void*t=Cyc_Tcutil_fndecl2typ(_tmpC1);_tmpC1->attributes=
Cyc_Tcutil_transfer_fn_type_atts(t,_tmpC1->attributes);{struct Cyc_List_List*atts=
_tmpC1->attributes;for(0;(unsigned int)atts;atts=atts->tl){void*_tmp162=(void*)
atts->hd;_LL8E: {struct Cyc_Absyn_Packed_att_struct*_tmp163=(struct Cyc_Absyn_Packed_att_struct*)
_tmp162;if(_tmp163->tag != 7)goto _LL90;}_LL8F: goto _LL91;_LL90: {struct Cyc_Absyn_Aligned_att_struct*
_tmp164=(struct Cyc_Absyn_Aligned_att_struct*)_tmp162;if(_tmp164->tag != 6)goto
_LL92;}_LL91:{const char*_tmp230;void*_tmp22F[1];struct Cyc_String_pa_struct
_tmp22E;(_tmp22E.tag=0,((_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp22F[0]=& _tmp22E,Cyc_Tcutil_terr(
_tmp50->loc,((_tmp230="bad attribute %s for function",_tag_dyneither(_tmp230,
sizeof(char),30))),_tag_dyneither(_tmp22F,sizeof(void*),1)))))));}goto _LL8D;
_LL92:;_LL93: goto _LL8D;_LL8D:;}}{struct Cyc_Core_Opt*_tmp233;struct Cyc_Absyn_Vardecl*
_tmp232;struct Cyc_Absyn_Vardecl*vd=(_tmp232=_cycalloc(sizeof(*_tmp232)),((
_tmp232->sc=_tmpC1->sc,((_tmp232->name=_tmpC1->name,((_tmp232->tq=Cyc_Absyn_const_tqual(
0),((_tmp232->type=Cyc_Absyn_at_typ(t,_tmp158,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((
_tmp232->initializer=0,((_tmp232->rgn=((_tmp233=_cycalloc(sizeof(*_tmp233)),((
_tmp233->v=_tmp158,_tmp233)))),((_tmp232->attributes=0,((_tmp232->escapes=0,
_tmp232)))))))))))))))));_tmpC1->fn_vardecl=(struct Cyc_Absyn_Vardecl*)vd;_tmpAC=
Cyc_Tcenv_add_local_var(_tmpAB,_tmp50->loc,_tmpAC,vd);{struct Cyc_Tcenv_Fenv*
old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpAC->le);_tmpAC->le=(struct Cyc_Tcenv_Fenv*)
Cyc_Tcenv_nested_fenv(_tmp50->loc,old_fenv,_tmpC1);Cyc_Tcstmt_tcStmt(_tmpAC,
_tmpC1->body,0);Cyc_Tcenv_check_delayed_effects(_tmpAC);Cyc_Tcenv_check_delayed_constraints(
_tmpAC);if(!Cyc_Tcenv_all_labels_resolved(_tmpAC)){const char*_tmp236;void*
_tmp235;(_tmp235=0,Cyc_Tcutil_terr(_tmp50->loc,((_tmp236="function has goto statements to undefined labels",
_tag_dyneither(_tmp236,sizeof(char),49))),_tag_dyneither(_tmp235,sizeof(void*),0)));}
_tmpAC->le=(struct Cyc_Tcenv_Fenv*)old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmpAB,_tmpAC,s0),_tmp51,0);return;};};};}_LL47: {struct Cyc_Absyn_Namespace_d_struct*
_tmpC2=(struct Cyc_Absyn_Namespace_d_struct*)_tmpAD;if(_tmpC2->tag != 10)goto _LL49;
else{_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;}}_LL48:{const char*_tmp237;
unimp_msg_part=((_tmp237="namespace",_tag_dyneither(_tmp237,sizeof(char),10)));}
goto _LL3A;_LL49: {struct Cyc_Absyn_Using_d_struct*_tmpC5=(struct Cyc_Absyn_Using_d_struct*)
_tmpAD;if(_tmpC5->tag != 11)goto _LL4B;else{_tmpC6=_tmpC5->f1;_tmpC7=_tmpC5->f2;}}
_LL4A:{const char*_tmp238;unimp_msg_part=((_tmp238="using",_tag_dyneither(_tmp238,
sizeof(char),6)));}goto _LL3A;_LL4B: {struct Cyc_Absyn_Aggr_d_struct*_tmpC8=(
struct Cyc_Absyn_Aggr_d_struct*)_tmpAD;if(_tmpC8->tag != 6)goto _LL4D;}_LL4C:{const
char*_tmp239;unimp_msg_part=((_tmp239="type",_tag_dyneither(_tmp239,sizeof(char),
5)));}goto _LL3A;_LL4D: {struct Cyc_Absyn_Datatype_d_struct*_tmpC9=(struct Cyc_Absyn_Datatype_d_struct*)
_tmpAD;if(_tmpC9->tag != 7)goto _LL4F;}_LL4E:{const char*_tmp23A;unimp_msg_part=((
_tmp23A="datatype",_tag_dyneither(_tmp23A,sizeof(char),9)));}goto _LL3A;_LL4F: {
struct Cyc_Absyn_Enum_d_struct*_tmpCA=(struct Cyc_Absyn_Enum_d_struct*)_tmpAD;if(
_tmpCA->tag != 8)goto _LL51;}_LL50:{const char*_tmp23B;unimp_msg_part=((_tmp23B="enum",
_tag_dyneither(_tmp23B,sizeof(char),5)));}goto _LL3A;_LL51: {struct Cyc_Absyn_Typedef_d_struct*
_tmpCB=(struct Cyc_Absyn_Typedef_d_struct*)_tmpAD;if(_tmpCB->tag != 9)goto _LL53;}
_LL52:{const char*_tmp23C;unimp_msg_part=((_tmp23C="typedef",_tag_dyneither(
_tmp23C,sizeof(char),8)));}goto _LL3A;_LL53: {struct Cyc_Absyn_ExternC_d_struct*
_tmpCC=(struct Cyc_Absyn_ExternC_d_struct*)_tmpAD;if(_tmpCC->tag != 12)goto _LL55;}
_LL54:{const char*_tmp23D;unimp_msg_part=((_tmp23D="extern \"C\"",_tag_dyneither(
_tmp23D,sizeof(char),11)));}goto _LL3A;_LL55: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpCD=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmpAD;if(_tmpCD->tag != 13)goto
_LL57;}_LL56:{const char*_tmp23E;unimp_msg_part=((_tmp23E="extern \"C include\"",
_tag_dyneither(_tmp23E,sizeof(char),19)));}goto _LL3A;_LL57: {struct Cyc_Absyn_Porton_d_struct*
_tmpCE=(struct Cyc_Absyn_Porton_d_struct*)_tmpAD;if(_tmpCE->tag != 14)goto _LL59;}
_LL58:{const char*_tmp23F;unimp_msg_part=((_tmp23F="__cyclone_port_on__",
_tag_dyneither(_tmp23F,sizeof(char),20)));}goto _LL3A;_LL59: {struct Cyc_Absyn_Portoff_d_struct*
_tmpCF=(struct Cyc_Absyn_Portoff_d_struct*)_tmpAD;if(_tmpCF->tag != 15)goto _LL3A;}
_LL5A:{const char*_tmp240;unimp_msg_part=((_tmp240="__cyclone_port_off__",
_tag_dyneither(_tmp240,sizeof(char),21)));}goto _LL3A;_LL3A:;};}{const char*
_tmp244;void*_tmp243[1];struct Cyc_String_pa_struct _tmp242;(int)_throw(((_tmp242.tag=
0,((_tmp242.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((
_tmp243[0]=& _tmp242,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp244="tcStmt: nested %s declarations unimplemented",
_tag_dyneither(_tmp244,sizeof(char),45))),_tag_dyneither(_tmp243,sizeof(void*),1)))))))));};}
_LL28: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp52=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp18;if(_tmp52->tag != 16)goto _LL7;else{_tmp53=_tmp52->f1;}}_LL29: {void*
rgn_type=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_Absyn_RgnHandleType_struct _tmp247;struct Cyc_Absyn_RgnHandleType_struct*
_tmp246;void*etype=(void*)((_tmp246=_cycalloc(sizeof(*_tmp246)),((_tmp246[0]=((
_tmp247.tag=16,((_tmp247.f1=(void*)rgn_type,_tmp247)))),_tmp246))));if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& etype,_tmp53),etype)){const char*_tmp24B;void*_tmp24A[
1];struct Cyc_String_pa_struct _tmp249;(_tmp249.tag=0,((_tmp249.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp53->topt))->v)),((_tmp24A[0]=& _tmp249,Cyc_Tcutil_terr(_tmp53->loc,((
_tmp24B="expecting region_t but found %s",_tag_dyneither(_tmp24B,sizeof(char),32))),
_tag_dyneither(_tmp24A,sizeof(void*),1)))))));}Cyc_Tcenv_check_rgn_resetable(te,
s0->loc,rgn_type);return;}_LL7:;};}
