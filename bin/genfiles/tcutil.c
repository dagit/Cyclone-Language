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
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_printf(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;
void*f2;};void*Cyc_Core_fst(struct _tuple0*);int Cyc_Core_intcmp(int,int);extern
char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < 
dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(struct
Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct
_tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;extern struct Cyc_Absyn_FloatType_struct
Cyc_Absyn_FloatType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
_tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum 
Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div
 = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt
 = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 
12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift
 = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple2*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*
es,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int
volatile_kw,struct _dyneither_ptr body,struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*
Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct
_tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct
Cyc_Absyn_Kind*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
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
_tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple13{unsigned int f1;int f2;};
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(
struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(
void*t);int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple14
Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);extern int Cyc_Tcutil_warn_alias_coerce;
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct
_tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple15 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple15*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct
_tuple16*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int
allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple17{int f1;void*f2;
};struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmpFDC;void*_tmpFDB[2];const char*_tmpFDA;
const char*_tmpFD9;struct Cyc_String_pa_struct _tmpFD8;struct Cyc_String_pa_struct
_tmpFD7;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmpFD7.tag=0,((_tmpFD7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmpFD8.tag=0,((_tmpFD8.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(_tmpFD9="const ",
_tag_dyneither(_tmpFD9,sizeof(char),7)):((_tmpFDA="",_tag_dyneither(_tmpFDA,
sizeof(char),1)))),((_tmpFDB[0]=& _tmpFD8,((_tmpFDB[1]=& _tmpFD7,Cyc_aprintf(((
_tmpFDC="%s%s",_tag_dyneither(_tmpFDC,sizeof(char),5))),_tag_dyneither(_tmpFDB,
sizeof(void*),2))))))))))))));const char*_tmpFE5;void*_tmpFE4[2];const char*
_tmpFE3;const char*_tmpFE2;struct Cyc_String_pa_struct _tmpFE1;struct Cyc_String_pa_struct
_tmpFE0;struct _dyneither_ptr s2=(struct _dyneither_ptr)((_tmpFE0.tag=0,((_tmpFE0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((
_tmpFE1.tag=0,((_tmpFE1.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmpFE2="const ",
_tag_dyneither(_tmpFE2,sizeof(char),7)):((_tmpFE3="",_tag_dyneither(_tmpFE3,
sizeof(char),1)))),((_tmpFE4[0]=& _tmpFE1,((_tmpFE4[1]=& _tmpFE0,Cyc_aprintf(((
_tmpFE5="%s%s",_tag_dyneither(_tmpFE5,sizeof(char),5))),_tag_dyneither(_tmpFE4,
sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpFE9;void*_tmpFE8[1];struct
Cyc_String_pa_struct _tmpFE7;(_tmpFE7.tag=0,((_tmpFE7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s1),((_tmpFE8[0]=& _tmpFE7,Cyc_fprintf(Cyc_stderr,((_tmpFE9="  %s",
_tag_dyneither(_tmpFE9,sizeof(char),5))),_tag_dyneither(_tmpFE8,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));if(pos + 5 >= 80){{const char*_tmpFEC;
void*_tmpFEB;(_tmpFEB=0,Cyc_fprintf(Cyc_stderr,((_tmpFEC="\n\t",_tag_dyneither(
_tmpFEC,sizeof(char),3))),_tag_dyneither(_tmpFEB,sizeof(void*),0)));}pos=8;}
else{{const char*_tmpFEF;void*_tmpFEE;(_tmpFEE=0,Cyc_fprintf(Cyc_stderr,((_tmpFEF=" ",
_tag_dyneither(_tmpFEF,sizeof(char),2))),_tag_dyneither(_tmpFEE,sizeof(void*),0)));}
++ pos;}{const char*_tmpFF2;void*_tmpFF1;(_tmpFF1=0,Cyc_fprintf(Cyc_stderr,((
_tmpFF2="and ",_tag_dyneither(_tmpFF2,sizeof(char),5))),_tag_dyneither(_tmpFF1,
sizeof(void*),0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{
const char*_tmpFF5;void*_tmpFF4;(_tmpFF4=0,Cyc_fprintf(Cyc_stderr,((_tmpFF5="\n\t",
_tag_dyneither(_tmpFF5,sizeof(char),3))),_tag_dyneither(_tmpFF4,sizeof(void*),0)));}
pos=8;}{const char*_tmpFF9;void*_tmpFF8[1];struct Cyc_String_pa_struct _tmpFF7;(
_tmpFF7.tag=0,((_tmpFF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((
_tmpFF8[0]=& _tmpFF7,Cyc_fprintf(Cyc_stderr,((_tmpFF9="%s ",_tag_dyneither(
_tmpFF9,sizeof(char),4))),_tag_dyneither(_tmpFF8,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpFFC;void*
_tmpFFB;(_tmpFFB=0,Cyc_fprintf(Cyc_stderr,((_tmpFFC="\n\t",_tag_dyneither(
_tmpFFC,sizeof(char),3))),_tag_dyneither(_tmpFFB,sizeof(void*),0)));}pos=8;}{
const char*_tmpFFF;void*_tmpFFE;(_tmpFFE=0,Cyc_fprintf(Cyc_stderr,((_tmpFFF="are not compatible. ",
_tag_dyneither(_tmpFFF,sizeof(char),21))),_tag_dyneither(_tmpFFE,sizeof(void*),0)));}
pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const
char*_tmp1002;void*_tmp1001;(_tmp1001=0,Cyc_fprintf(Cyc_stderr,((_tmp1002="\n\t",
_tag_dyneither(_tmp1002,sizeof(char),3))),_tag_dyneither(_tmp1001,sizeof(void*),
0)));}{const char*_tmp1006;void*_tmp1005[1];struct Cyc_String_pa_struct _tmp1004;(
_tmp1004.tag=0,((_tmp1004.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmp1005[0]=& _tmp1004,Cyc_fprintf(Cyc_stderr,((_tmp1006="%s",_tag_dyneither(
_tmp1006,sizeof(char),3))),_tag_dyneither(_tmp1005,sizeof(void*),1)))))));};}{
const char*_tmp1009;void*_tmp1008;(_tmp1008=0,Cyc_fprintf(Cyc_stderr,((_tmp1009="\n",
_tag_dyneither(_tmp1009,sizeof(char),2))),_tag_dyneither(_tmp1008,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{const char*_tmp100D;void*_tmp100C[1];struct Cyc_String_pa_struct _tmp100B;(
_tmp100B.tag=0,((_tmp100B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp100C[0]=& _tmp100B,Cyc_fprintf(Cyc_stderr,((_tmp100D="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmp100D,sizeof(char),36))),_tag_dyneither(_tmp100C,sizeof(void*),
1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmp1010;struct Cyc_Core_Impossible_struct*_tmp100F;(int)_throw((void*)((_tmp100F=
_cycalloc(sizeof(*_tmp100F)),((_tmp100F[0]=((_tmp1010.tag=Cyc_Core_Impossible,((
_tmp1010.f1=msg,_tmp1010)))),_tmp100F)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp1015;void*_tmp1014[2];struct Cyc_String_pa_struct
_tmp1013;struct Cyc_String_pa_struct _tmp1012;(_tmp1012.tag=0,((_tmp1012.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp1013.tag=0,((_tmp1013.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp1014[0]=& _tmp1013,((_tmp1014[1]=& _tmp1012,Cyc_fprintf(Cyc_stderr,((
_tmp1015="%s::%s ",_tag_dyneither(_tmp1015,sizeof(char),8))),_tag_dyneither(
_tmp1014,sizeof(void*),2)))))))))))));}{const char*_tmp1018;void*_tmp1017;(
_tmp1017=0,Cyc_fprintf(Cyc_stderr,((_tmp1018="\n",_tag_dyneither(_tmp1018,
sizeof(char),2))),_tag_dyneither(_tmp1017,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp1019;Cyc_Tcutil_warning_segs=((_tmp1019=_cycalloc(
sizeof(*_tmp1019)),((_tmp1019->hd=sg,((_tmp1019->tl=Cyc_Tcutil_warning_segs,
_tmp1019))))));}{struct _dyneither_ptr*_tmp101C;struct Cyc_List_List*_tmp101B;Cyc_Tcutil_warning_msgs=((
_tmp101B=_cycalloc(sizeof(*_tmp101B)),((_tmp101B->hd=((_tmp101C=_cycalloc(
sizeof(*_tmp101C)),((_tmp101C[0]=msg,_tmp101C)))),((_tmp101B->tl=Cyc_Tcutil_warning_msgs,
_tmp101B))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp101F;void*_tmp101E;(_tmp101E=0,Cyc_fprintf(Cyc_stderr,((
_tmp101F="***Warnings***\n",_tag_dyneither(_tmp101F,sizeof(char),16))),
_tag_dyneither(_tmp101E,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp1024;void*_tmp1023[2];struct
Cyc_String_pa_struct _tmp1022;struct Cyc_String_pa_struct _tmp1021;(_tmp1021.tag=0,((
_tmp1021.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp1022.tag=
0,((_tmp1022.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp1023[0]=&
_tmp1022,((_tmp1023[1]=& _tmp1021,Cyc_fprintf(Cyc_stderr,((_tmp1024="%s: %s\n",
_tag_dyneither(_tmp1024,sizeof(char),8))),_tag_dyneither(_tmp1023,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp1027;void*_tmp1026;(
_tmp1026=0,Cyc_fprintf(Cyc_stderr,((_tmp1027="**************\n",_tag_dyneither(
_tmp1027,sizeof(char),16))),_tag_dyneither(_tmp1026,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t){void*
_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;void**_tmp43;void***_tmp44;
struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;struct Cyc_Absyn_Exp*_tmp49;
_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)_tmp3D;
if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}_LL2:
goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 18)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 18)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp1028;*_tmp44=((_tmp1028=
_cycalloc(sizeof(*_tmp1028)),((_tmp1028[0]=t2,_tmp1028))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp1029;*_tmp47=((_tmp1029=_cycalloc(sizeof(*
_tmp1029)),((_tmp1029->v=t2,_tmp1029))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 19)goto _LLB;
else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{void*_tmp4C=
_tmp49->r;void*_tmp4E;_LLE: {struct Cyc_Absyn_Valueof_e_struct*_tmp4D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp4C;if(_tmp4D->tag != 39)goto _LL10;else{_tmp4E=(void*)_tmp4D->f1;}}_LLF: return
_tmp4E;_LL10:;_LL11: return t;_LLD:;};_LLB:;_LLC: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp4F=cptr;union Cyc_Absyn_Constraint
_tmp50;int _tmp51;union Cyc_Absyn_Constraint _tmp52;void*_tmp53;union Cyc_Absyn_Constraint
_tmp54;union Cyc_Absyn_Constraint*_tmp55;_LL13: _tmp50=*_tmp4F;if((_tmp50.No_constr).tag
!= 3)goto _LL15;_tmp51=(int)(_tmp50.No_constr).val;_LL14: return Cyc_Absyn_empty_conref();
_LL15: _tmp52=*_tmp4F;if((_tmp52.Eq_constr).tag != 1)goto _LL17;_tmp53=(void*)(
_tmp52.Eq_constr).val;_LL16: return Cyc_Absyn_new_conref(_tmp53);_LL17: _tmp54=*
_tmp4F;if((_tmp54.Forward_constr).tag != 2)goto _LL12;_tmp55=(union Cyc_Absyn_Constraint*)(
_tmp54.Forward_constr).val;_LL18: return Cyc_Tcutil_copy_conref(_tmp55);_LL12:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb){void*_tmp56=Cyc_Absyn_compress_kb(
kb);struct Cyc_Absyn_Kind*_tmp59;_LL1A: {struct Cyc_Absyn_Unknown_kb_struct*_tmp57=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmp56;if(_tmp57->tag != 1)goto _LL1C;}_LL1B: {
struct Cyc_Absyn_Unknown_kb_struct _tmp102C;struct Cyc_Absyn_Unknown_kb_struct*
_tmp102B;return(void*)((_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B[0]=((
_tmp102C.tag=1,((_tmp102C.f1=0,_tmp102C)))),_tmp102B))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*
_tmp58=(struct Cyc_Absyn_Less_kb_struct*)_tmp56;if(_tmp58->tag != 2)goto _LL1E;
else{_tmp59=_tmp58->f2;}}_LL1D: {struct Cyc_Absyn_Less_kb_struct _tmp102F;struct
Cyc_Absyn_Less_kb_struct*_tmp102E;return(void*)((_tmp102E=_cycalloc(sizeof(*
_tmp102E)),((_tmp102E[0]=((_tmp102F.tag=2,((_tmp102F.f1=0,((_tmp102F.f2=_tmp59,
_tmp102F)))))),_tmp102E))));}_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp1030;
return(_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030->name=tv->name,((
_tmp1030->identity=- 1,((_tmp1030->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp1030)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmp1031;return(_tmp1031=_cycalloc(sizeof(*
_tmp1031)),((_tmp1031->f1=_tmp61,((_tmp1031->f2=_tmp62,((_tmp1031->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmp1031)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg){struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;
struct _tuple11*_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{
struct _tuple11*_tmp1032;return(_tmp1032=_cycalloc(sizeof(*_tmp1032)),((_tmp1032->f1=
_tmp67,((_tmp1032->f2=Cyc_Tcutil_copy_type(_tmp68),_tmp1032)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp1033;return(_tmp1033=_cycalloc(sizeof(*_tmp1033)),((_tmp1033->name=f->name,((
_tmp1033->tq=f->tq,((_tmp1033->type=Cyc_Tcutil_copy_type(f->type),((_tmp1033->width=
f->width,((_tmp1033->attributes=f->attributes,_tmp1033)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;
void*_tmp6E;struct _tuple0*_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{
struct _tuple0*_tmp1034;return(_tmp1034=_cycalloc(sizeof(*_tmp1034)),((_tmp1034->f1=
Cyc_Tcutil_copy_type(_tmp6D),((_tmp1034->f2=Cyc_Tcutil_copy_type(_tmp6E),
_tmp1034)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp1035;return(_tmp1035=
_cycalloc(sizeof(*_tmp1035)),((_tmp1035->name=f->name,((_tmp1035->tag=f->tag,((
_tmp1035->loc=f->loc,_tmp1035)))))));}void*Cyc_Tcutil_copy_type(void*t){void*
_tmp71=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp75;struct Cyc_Absyn_DatatypeInfo
_tmp77;union Cyc_Absyn_DatatypeInfoU _tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_DatatypeFieldInfo
_tmp7B;union Cyc_Absyn_DatatypeFieldInfoU _tmp7C;struct Cyc_List_List*_tmp7D;struct
Cyc_Absyn_PtrInfo _tmp7F;void*_tmp80;struct Cyc_Absyn_Tqual _tmp81;struct Cyc_Absyn_PtrAtts
_tmp82;void*_tmp83;union Cyc_Absyn_Constraint*_tmp84;union Cyc_Absyn_Constraint*
_tmp85;union Cyc_Absyn_Constraint*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp87;struct Cyc_Absyn_ArrayInfo
_tmp8C;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;struct Cyc_Absyn_Exp*_tmp8F;union
Cyc_Absyn_Constraint*_tmp90;struct Cyc_Position_Segment*_tmp91;struct Cyc_Absyn_FnInfo
_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_Core_Opt*_tmp95;void*_tmp96;struct
Cyc_List_List*_tmp97;int _tmp98;struct Cyc_Absyn_VarargInfo*_tmp99;struct Cyc_List_List*
_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_List_List*_tmp9D;struct Cyc_Absyn_AggrInfo
_tmp9F;union Cyc_Absyn_AggrInfoU _tmpA0;struct _tuple4 _tmpA1;enum Cyc_Absyn_AggrKind
_tmpA2;struct _tuple2*_tmpA3;struct Cyc_Core_Opt*_tmpA4;struct Cyc_List_List*_tmpA5;
struct Cyc_Absyn_AggrInfo _tmpA7;union Cyc_Absyn_AggrInfoU _tmpA8;struct Cyc_Absyn_Aggrdecl**
_tmpA9;struct Cyc_List_List*_tmpAA;enum Cyc_Absyn_AggrKind _tmpAC;struct Cyc_List_List*
_tmpAD;struct _tuple2*_tmpAF;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_List_List*
_tmpB2;void*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;void*_tmpB8;void*_tmpBA;void*
_tmpBB;struct _tuple2*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_Typedefdecl*
_tmpBF;void*_tmpC3;struct Cyc_List_List*_tmpC5;void*_tmpC7;_LL21: {struct Cyc_Absyn_VoidType_struct*
_tmp72=(struct Cyc_Absyn_VoidType_struct*)_tmp71;if(_tmp72->tag != 0)goto _LL23;}
_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Evar_struct*_tmp73=(struct Cyc_Absyn_Evar_struct*)
_tmp71;if(_tmp73->tag != 1)goto _LL25;}_LL24: return t;_LL25: {struct Cyc_Absyn_VarType_struct*
_tmp74=(struct Cyc_Absyn_VarType_struct*)_tmp71;if(_tmp74->tag != 2)goto _LL27;
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmp1038;struct
Cyc_Absyn_VarType_struct*_tmp1037;return(void*)((_tmp1037=_cycalloc(sizeof(*
_tmp1037)),((_tmp1037[0]=((_tmp1038.tag=2,((_tmp1038.f1=Cyc_Tcutil_copy_tvar(
_tmp75),_tmp1038)))),_tmp1037))));}_LL27: {struct Cyc_Absyn_DatatypeType_struct*
_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)_tmp71;if(_tmp76->tag != 3)goto _LL29;
else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;_tmp79=_tmp77.targs;}}_LL28: {
struct Cyc_Absyn_DatatypeType_struct _tmp103E;struct Cyc_Absyn_DatatypeInfo _tmp103D;
struct Cyc_Absyn_DatatypeType_struct*_tmp103C;return(void*)((_tmp103C=_cycalloc(
sizeof(*_tmp103C)),((_tmp103C[0]=((_tmp103E.tag=3,((_tmp103E.f1=((_tmp103D.datatype_info=
_tmp78,((_tmp103D.targs=Cyc_Tcutil_copy_types(_tmp79),_tmp103D)))),_tmp103E)))),
_tmp103C))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp71;if(_tmp7A->tag != 4)goto _LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;
_tmp7D=_tmp7B.targs;}}_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp1044;
struct Cyc_Absyn_DatatypeFieldInfo _tmp1043;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp1042;return(void*)((_tmp1042=_cycalloc(sizeof(*_tmp1042)),((_tmp1042[0]=((
_tmp1044.tag=4,((_tmp1044.f1=((_tmp1043.field_info=_tmp7C,((_tmp1043.targs=Cyc_Tcutil_copy_types(
_tmp7D),_tmp1043)))),_tmp1044)))),_tmp1042))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*
_tmp7E=(struct Cyc_Absyn_PointerType_struct*)_tmp71;if(_tmp7E->tag != 5)goto _LL2D;
else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;
_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;
_tmp87=_tmp82.ptrloc;}}_LL2C: {void*_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*
_tmpD1=Cyc_Tcutil_copy_type(_tmp83);union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual
_tmpD3=_tmp81;union Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*
_tmpD5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmp104E;struct Cyc_Absyn_PtrAtts
_tmp104D;struct Cyc_Absyn_PtrInfo _tmp104C;struct Cyc_Absyn_PointerType_struct*
_tmp104B;return(void*)((_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B[0]=((
_tmp104E.tag=5,((_tmp104E.f1=((_tmp104C.elt_typ=_tmpD0,((_tmp104C.elt_tq=_tmpD3,((
_tmp104C.ptr_atts=((_tmp104D.rgn=_tmpD1,((_tmp104D.nullable=_tmpD2,((_tmp104D.bounds=
_tmpD4,((_tmp104D.zero_term=_tmpD5,((_tmp104D.ptrloc=_tmp87,_tmp104D)))))))))),
_tmp104C)))))),_tmp104E)))),_tmp104B))));}_LL2D: {struct Cyc_Absyn_IntType_struct*
_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp71;if(_tmp88->tag != 6)goto _LL2F;}
_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*_tmp89=(struct Cyc_Absyn_FloatType_struct*)
_tmp71;if(_tmp89->tag != 7)goto _LL31;}_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*
_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)_tmp71;if(_tmp8A->tag != 8)goto _LL33;}
_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp71;if(_tmp8B->tag != 9)goto _LL35;else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;
_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}
_LL34: {struct Cyc_Absyn_ArrayType_struct _tmp1054;struct Cyc_Absyn_ArrayInfo
_tmp1053;struct Cyc_Absyn_ArrayType_struct*_tmp1052;return(void*)((_tmp1052=
_cycalloc(sizeof(*_tmp1052)),((_tmp1052[0]=((_tmp1054.tag=9,((_tmp1054.f1=((
_tmp1053.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((_tmp1053.tq=_tmp8E,((_tmp1053.num_elts=
_tmp8F,((_tmp1053.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmp1053.zt_loc=_tmp91,_tmp1053)))))))))),
_tmp1054)))),_tmp1052))));}_LL35: {struct Cyc_Absyn_FnType_struct*_tmp92=(struct
Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;else{_tmp93=_tmp92->f1;
_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;_tmp97=_tmp93.args;
_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=_tmp93.rgn_po;_tmp9B=
_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*_tmp1055;struct Cyc_Core_Opt*
_tmpDE=_tmp95 == 0?0:((_tmp1055=_cycalloc(sizeof(*_tmp1055)),((_tmp1055->v=Cyc_Tcutil_copy_type((
void*)_tmp95->v),_tmp1055))));void*_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*
_tmpE0=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp99 != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp99;struct Cyc_Absyn_VarargInfo*_tmp1056;cyc_varargs2=((_tmp1056=_cycalloc(
sizeof(*_tmp1056)),((_tmp1056->name=cv->name,((_tmp1056->tq=cv->tq,((_tmp1056->type=
Cyc_Tcutil_copy_type(cv->type),((_tmp1056->inject=cv->inject,_tmp1056))))))))));}{
struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9A);
struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmp105C;struct
Cyc_Absyn_FnInfo _tmp105B;struct Cyc_Absyn_FnType_struct*_tmp105A;return(void*)((
_tmp105A=_cycalloc(sizeof(*_tmp105A)),((_tmp105A[0]=((_tmp105C.tag=10,((_tmp105C.f1=((
_tmp105B.tvars=_tmpDD,((_tmp105B.effect=_tmpDE,((_tmp105B.ret_typ=_tmpDF,((
_tmp105B.args=_tmpE0,((_tmp105B.c_varargs=_tmpE1,((_tmp105B.cyc_varargs=
cyc_varargs2,((_tmp105B.rgn_po=_tmpE3,((_tmp105B.attributes=_tmpE4,_tmp105B)))))))))))))))),
_tmp105C)))),_tmp105A))));};}_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(
struct Cyc_Absyn_TupleType_struct*)_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{
_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct _tmp105F;struct Cyc_Absyn_TupleType_struct*
_tmp105E;return(void*)((_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E[0]=((
_tmp105F.tag=11,((_tmp105F.f1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),
_tmp105F)))),_tmp105E))));}_LL39: {struct Cyc_Absyn_AggrType_struct*_tmp9E=(
struct Cyc_Absyn_AggrType_struct*)_tmp71;if(_tmp9E->tag != 12)goto _LL3B;else{
_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((_tmpA0.UnknownAggr).tag != 1)goto
_LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;_tmpA2=_tmpA1.f1;_tmpA3=
_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {struct Cyc_Absyn_AggrType_struct
_tmp1065;struct Cyc_Absyn_AggrInfo _tmp1064;struct Cyc_Absyn_AggrType_struct*
_tmp1063;return(void*)((_tmp1063=_cycalloc(sizeof(*_tmp1063)),((_tmp1063[0]=((
_tmp1065.tag=12,((_tmp1065.f1=((_tmp1064.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmp1064.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmp1064)))),
_tmp1065)))),_tmp1063))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(
struct Cyc_Absyn_AggrType_struct*)_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{
_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;if((_tmpA8.KnownAggr).tag != 2)goto
_LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}
_LL3C: {struct Cyc_Absyn_AggrType_struct _tmp106B;struct Cyc_Absyn_AggrInfo _tmp106A;
struct Cyc_Absyn_AggrType_struct*_tmp1069;return(void*)((_tmp1069=_cycalloc(
sizeof(*_tmp1069)),((_tmp1069[0]=((_tmp106B.tag=12,((_tmp106B.f1=((_tmp106A.aggr_info=
Cyc_Absyn_KnownAggr(_tmpA9),((_tmp106A.targs=Cyc_Tcutil_copy_types(_tmpAA),
_tmp106A)))),_tmp106B)))),_tmp1069))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmp106E;struct Cyc_Absyn_AnonAggrType_struct*_tmp106D;return(void*)((_tmp106D=
_cycalloc(sizeof(*_tmp106D)),((_tmp106D[0]=((_tmp106E.tag=13,((_tmp106E.f1=
_tmpAC,((_tmp106E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpAD),_tmp106E)))))),_tmp106D))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmpAE=(struct Cyc_Absyn_EnumType_struct*)_tmp71;if(_tmpAE->tag != 14)goto _LL41;
else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL40: {struct Cyc_Absyn_EnumType_struct
_tmp1071;struct Cyc_Absyn_EnumType_struct*_tmp1070;return(void*)((_tmp1070=
_cycalloc(sizeof(*_tmp1070)),((_tmp1070[0]=((_tmp1071.tag=14,((_tmp1071.f1=
_tmpAF,((_tmp1071.f2=_tmpB0,_tmp1071)))))),_tmp1070))));}_LL41: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp71;if(_tmpB1->tag != 15)goto
_LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1074;struct Cyc_Absyn_AnonEnumType_struct*_tmp1073;return(void*)((_tmp1073=
_cycalloc(sizeof(*_tmp1073)),((_tmp1073[0]=((_tmp1074.tag=15,((_tmp1074.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmp1074)))),
_tmp1073))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmp1077;struct Cyc_Absyn_TagType_struct*_tmp1076;
return(void*)((_tmp1076=_cycalloc(sizeof(*_tmp1076)),((_tmp1076[0]=((_tmp1077.tag=
20,((_tmp1077.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmp1077)))),_tmp1076))));}
_LL45: {struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmp107A;struct Cyc_Absyn_ValueofType_struct*_tmp1079;return(void*)((_tmp1079=
_cycalloc(sizeof(*_tmp1079)),((_tmp1079[0]=((_tmp107A.tag=19,((_tmp107A.f1=
_tmpB6,_tmp107A)))),_tmp1079))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB7=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto
_LL49;else{_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct
_tmp107D;struct Cyc_Absyn_RgnHandleType_struct*_tmp107C;return(void*)((_tmp107C=
_cycalloc(sizeof(*_tmp107C)),((_tmp107C[0]=((_tmp107D.tag=16,((_tmp107D.f1=(void*)
Cyc_Tcutil_copy_type(_tmpB8),_tmp107D)))),_tmp107C))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmp1080;struct Cyc_Absyn_DynRgnType_struct*_tmp107F;return(void*)((_tmp107F=
_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=17,((_tmp1080.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmp1080.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmp1080)))))),_tmp107F))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmp1083;struct Cyc_Absyn_TypedefType_struct*_tmp1082;return(void*)((_tmp1082=
_cycalloc(sizeof(*_tmp1082)),((_tmp1082[0]=((_tmp1083.tag=18,((_tmp1083.f1=
_tmpBD,((_tmp1083.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmp1083.f3=_tmpBF,((
_tmp1083.f4=0,_tmp1083)))))))))),_tmp1082))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC0=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}
_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmp1086;
struct Cyc_Absyn_AccessEff_struct*_tmp1085;return(void*)((_tmp1085=_cycalloc(
sizeof(*_tmp1085)),((_tmp1085[0]=((_tmp1086.tag=23,((_tmp1086.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmp1086)))),_tmp1085))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmp1089;struct Cyc_Absyn_JoinEff_struct*
_tmp1088;return(void*)((_tmp1088=_cycalloc(sizeof(*_tmp1088)),((_tmp1088[0]=((
_tmp1089.tag=24,((_tmp1089.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmp1089)))),
_tmp1088))));}_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmp108C;struct Cyc_Absyn_RgnsEff_struct*_tmp108B;
return(void*)((_tmp108B=_cycalloc(sizeof(*_tmp108B)),((_tmp108B[0]=((_tmp108C.tag=
25,((_tmp108C.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmp108C)))),_tmp108B))));}
_LL20:;}static void*Cyc_Tcutil_copy_designator(void*d){void*_tmp107=d;struct Cyc_Absyn_Exp*
_tmp109;struct _dyneither_ptr*_tmp10B;_LL58: {struct Cyc_Absyn_ArrayElement_struct*
_tmp108=(struct Cyc_Absyn_ArrayElement_struct*)_tmp107;if(_tmp108->tag != 0)goto
_LL5A;else{_tmp109=_tmp108->f1;}}_LL59: {struct Cyc_Absyn_ArrayElement_struct
_tmp108F;struct Cyc_Absyn_ArrayElement_struct*_tmp108E;return(void*)((_tmp108E=
_cycalloc(sizeof(*_tmp108E)),((_tmp108E[0]=((_tmp108F.tag=0,((_tmp108F.f1=Cyc_Tcutil_deep_copy_exp(
_tmp109),_tmp108F)))),_tmp108E))));}_LL5A: {struct Cyc_Absyn_FieldName_struct*
_tmp10A=(struct Cyc_Absyn_FieldName_struct*)_tmp107;if(_tmp10A->tag != 1)goto _LL57;
else{_tmp10B=_tmp10A->f1;}}_LL5B: return d;_LL57:;}struct _tuple18{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple18*Cyc_Tcutil_copy_eds(struct
_tuple18*e){struct _tuple18*_tmp1090;return(_tmp1090=_cycalloc(sizeof(*_tmp1090)),((
_tmp1090->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp1090->f2=Cyc_Tcutil_deep_copy_exp((
e[0]).f2),_tmp1090)))));}struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*
e){void*_tmp10F=e->r;union Cyc_Absyn_Cnst _tmp111;struct _tuple2*_tmp113;void*
_tmp114;struct _tuple2*_tmp116;enum Cyc_Absyn_Primop _tmp118;struct Cyc_List_List*
_tmp119;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Core_Opt*_tmp11C;struct Cyc_Absyn_Exp*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11F;enum Cyc_Absyn_Incrementor _tmp120;struct Cyc_Absyn_Exp*
_tmp122;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*
_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*
_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*
_tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List*
_tmp133;struct Cyc_Absyn_VarargCallInfo*_tmp134;struct Cyc_Absyn_Exp*_tmp136;
struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_List_List*
_tmp13B;void*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;int _tmp13F;enum Cyc_Absyn_Coercion
_tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*
_tmp145;void*_tmp147;struct Cyc_Absyn_Exp*_tmp149;void*_tmp14B;void*_tmp14C;
struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct _dyneither_ptr*
_tmp151;int _tmp152;int _tmp153;struct Cyc_Absyn_Exp*_tmp155;struct _dyneither_ptr*
_tmp156;int _tmp157;int _tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*
_tmp15B;struct Cyc_List_List*_tmp15D;struct _tuple9*_tmp15F;struct _tuple9 _tmp160;
struct Cyc_Core_Opt*_tmp161;struct Cyc_Absyn_Tqual _tmp162;void*_tmp163;struct Cyc_List_List*
_tmp164;struct Cyc_List_List*_tmp166;struct Cyc_Absyn_Vardecl*_tmp168;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;int _tmp16B;struct _tuple2*_tmp16D;struct Cyc_List_List*
_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_Aggrdecl*_tmp170;void*
_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypedecl*
_tmp176;struct Cyc_Absyn_Datatypefield*_tmp177;struct _tuple2*_tmp179;struct Cyc_Absyn_Enumdecl*
_tmp17A;struct Cyc_Absyn_Enumfield*_tmp17B;struct _tuple2*_tmp17D;void*_tmp17E;
struct Cyc_Absyn_Enumfield*_tmp17F;struct Cyc_Absyn_MallocInfo _tmp181;int _tmp182;
struct Cyc_Absyn_Exp*_tmp183;void**_tmp184;struct Cyc_Absyn_Exp*_tmp185;int _tmp186;
struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Core_Opt*
_tmp18B;struct Cyc_List_List*_tmp18C;struct Cyc_Absyn_Exp*_tmp18F;struct
_dyneither_ptr*_tmp190;void*_tmp192;int _tmp194;struct _dyneither_ptr _tmp195;_LL5D: {
struct Cyc_Absyn_Const_e_struct*_tmp110=(struct Cyc_Absyn_Const_e_struct*)_tmp10F;
if(_tmp110->tag != 0)goto _LL5F;else{_tmp111=_tmp110->f1;}}_LL5E: return Cyc_Absyn_const_exp(
_tmp111,e->loc);_LL5F: {struct Cyc_Absyn_Var_e_struct*_tmp112=(struct Cyc_Absyn_Var_e_struct*)
_tmp10F;if(_tmp112->tag != 1)goto _LL61;else{_tmp113=_tmp112->f1;_tmp114=(void*)
_tmp112->f2;}}_LL60: return Cyc_Absyn_varb_exp(_tmp113,_tmp114,e->loc);_LL61: {
struct Cyc_Absyn_UnknownId_e_struct*_tmp115=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp10F;if(_tmp115->tag != 2)goto _LL63;else{_tmp116=_tmp115->f1;}}_LL62: return Cyc_Absyn_unknownid_exp(
_tmp116,e->loc);_LL63: {struct Cyc_Absyn_Primop_e_struct*_tmp117=(struct Cyc_Absyn_Primop_e_struct*)
_tmp10F;if(_tmp117->tag != 3)goto _LL65;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}
_LL64: return Cyc_Absyn_primop_exp(_tmp118,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp119),e->loc);_LL65: {struct Cyc_Absyn_AssignOp_e_struct*_tmp11A=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp10F;if(_tmp11A->tag != 4)goto _LL67;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;
_tmp11D=_tmp11A->f3;}}_LL66: {struct Cyc_Core_Opt*_tmp1091;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp11B),(unsigned int)_tmp11C?(_tmp1091=
_cycalloc_atomic(sizeof(*_tmp1091)),((_tmp1091->v=(void*)((enum Cyc_Absyn_Primop)
_tmp11C->v),_tmp1091))): 0,Cyc_Tcutil_deep_copy_exp(_tmp11D),e->loc);}_LL67: {
struct Cyc_Absyn_Increment_e_struct*_tmp11E=(struct Cyc_Absyn_Increment_e_struct*)
_tmp10F;if(_tmp11E->tag != 5)goto _LL69;else{_tmp11F=_tmp11E->f1;_tmp120=_tmp11E->f2;}}
_LL68: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp11F),_tmp120,e->loc);
_LL69: {struct Cyc_Absyn_Conditional_e_struct*_tmp121=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp10F;if(_tmp121->tag != 6)goto _LL6B;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;
_tmp124=_tmp121->f3;}}_LL6A: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp122),Cyc_Tcutil_deep_copy_exp(_tmp123),Cyc_Tcutil_deep_copy_exp(_tmp124),e->loc);
_LL6B: {struct Cyc_Absyn_And_e_struct*_tmp125=(struct Cyc_Absyn_And_e_struct*)
_tmp10F;if(_tmp125->tag != 7)goto _LL6D;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}
_LL6C: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp126),Cyc_Tcutil_deep_copy_exp(
_tmp127),e->loc);_LL6D: {struct Cyc_Absyn_Or_e_struct*_tmp128=(struct Cyc_Absyn_Or_e_struct*)
_tmp10F;if(_tmp128->tag != 8)goto _LL6F;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}
_LL6E: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp129),Cyc_Tcutil_deep_copy_exp(
_tmp12A),e->loc);_LL6F: {struct Cyc_Absyn_SeqExp_e_struct*_tmp12B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp10F;if(_tmp12B->tag != 9)goto _LL71;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}
_LL70: return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp12C),Cyc_Tcutil_deep_copy_exp(
_tmp12D),e->loc);_LL71: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp12E=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp10F;if(_tmp12E->tag != 10)goto _LL73;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;}}
_LL72: return Cyc_Absyn_unknowncall_exp(Cyc_Tcutil_deep_copy_exp(_tmp12F),((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp130),e->loc);_LL73: {struct Cyc_Absyn_FnCall_e_struct*
_tmp131=(struct Cyc_Absyn_FnCall_e_struct*)_tmp10F;if(_tmp131->tag != 11)goto _LL75;
else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;_tmp134=_tmp131->f3;}}_LL74: {
struct Cyc_Absyn_VarargCallInfo*_tmp197=_tmp134;struct Cyc_Absyn_VarargCallInfo
_tmp198;int _tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_Absyn_VarargInfo*
_tmp19B;_LLB0: if(_tmp197 == 0)goto _LLB2;_tmp198=*_tmp197;_tmp199=_tmp198.num_varargs;
_tmp19A=_tmp198.injectors;_tmp19B=_tmp198.vai;_LLB1: {struct Cyc_Absyn_VarargInfo
_tmp19D;struct Cyc_Core_Opt*_tmp19E;struct Cyc_Absyn_Tqual _tmp19F;void*_tmp1A0;int
_tmp1A1;struct Cyc_Absyn_VarargInfo*_tmp19C=_tmp19B;_tmp19D=*_tmp19C;_tmp19E=
_tmp19D.name;_tmp19F=_tmp19D.tq;_tmp1A0=_tmp19D.type;_tmp1A1=_tmp19D.inject;{
struct Cyc_Absyn_FnCall_e_struct _tmp109B;struct Cyc_Absyn_VarargInfo*_tmp109A;
struct Cyc_Absyn_VarargCallInfo*_tmp1099;struct Cyc_Absyn_FnCall_e_struct*_tmp1098;
return Cyc_Absyn_new_exp((void*)((_tmp1098=_cycalloc(sizeof(*_tmp1098)),((
_tmp1098[0]=((_tmp109B.tag=11,((_tmp109B.f1=Cyc_Tcutil_deep_copy_exp(_tmp132),((
_tmp109B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp133),((
_tmp109B.f3=((_tmp1099=_cycalloc(sizeof(*_tmp1099)),((_tmp1099->num_varargs=
_tmp199,((_tmp1099->injectors=_tmp19A,((_tmp1099->vai=((_tmp109A=_cycalloc(
sizeof(*_tmp109A)),((_tmp109A->name=_tmp19E,((_tmp109A->tq=_tmp19F,((_tmp109A->type=
Cyc_Tcutil_copy_type(_tmp1A0),((_tmp109A->inject=_tmp1A1,_tmp109A)))))))))),
_tmp1099)))))))),_tmp109B)))))))),_tmp1098)))),e->loc);};}_LLB2:;_LLB3: {struct
Cyc_Absyn_FnCall_e_struct _tmp109E;struct Cyc_Absyn_FnCall_e_struct*_tmp109D;
return Cyc_Absyn_new_exp((void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((
_tmp109D[0]=((_tmp109E.tag=11,((_tmp109E.f1=Cyc_Tcutil_deep_copy_exp(_tmp132),((
_tmp109E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp133),((
_tmp109E.f3=_tmp134,_tmp109E)))))))),_tmp109D)))),e->loc);}_LLAF:;}_LL75: {
struct Cyc_Absyn_Throw_e_struct*_tmp135=(struct Cyc_Absyn_Throw_e_struct*)_tmp10F;
if(_tmp135->tag != 12)goto _LL77;else{_tmp136=_tmp135->f1;}}_LL76: return Cyc_Absyn_throw_exp(
Cyc_Tcutil_deep_copy_exp(_tmp136),e->loc);_LL77: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp137=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp10F;if(_tmp137->tag != 13)
goto _LL79;else{_tmp138=_tmp137->f1;}}_LL78: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
_tmp138),e->loc);_LL79: {struct Cyc_Absyn_Instantiate_e_struct*_tmp139=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp10F;if(_tmp139->tag != 14)goto _LL7B;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;}}
_LL7A: return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp13A),((struct
Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp13B),e->loc);_LL7B: {struct Cyc_Absyn_Cast_e_struct*_tmp13C=(struct Cyc_Absyn_Cast_e_struct*)
_tmp10F;if(_tmp13C->tag != 15)goto _LL7D;else{_tmp13D=(void*)_tmp13C->f1;_tmp13E=
_tmp13C->f2;_tmp13F=_tmp13C->f3;_tmp140=_tmp13C->f4;}}_LL7C: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp13D),Cyc_Tcutil_deep_copy_exp(_tmp13E),_tmp13F,_tmp140,
e->loc);_LL7D: {struct Cyc_Absyn_Address_e_struct*_tmp141=(struct Cyc_Absyn_Address_e_struct*)
_tmp10F;if(_tmp141->tag != 16)goto _LL7F;else{_tmp142=_tmp141->f1;}}_LL7E: return
Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp142),e->loc);_LL7F: {struct
Cyc_Absyn_New_e_struct*_tmp143=(struct Cyc_Absyn_New_e_struct*)_tmp10F;if(_tmp143->tag
!= 17)goto _LL81;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;}}_LL80: {struct Cyc_Absyn_Exp*
eo1=_tmp144;if(_tmp144 != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp144);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp145),e->loc);}_LL81: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp146=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp10F;if(_tmp146->tag != 18)goto _LL83;else{_tmp147=(void*)_tmp146->f1;}}_LL82:
return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp147),e->loc);_LL83: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp148=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp10F;if(_tmp148->tag != 19)goto _LL85;else{_tmp149=_tmp148->f1;}}_LL84: return
Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),e->loc);_LL85: {struct
Cyc_Absyn_Offsetof_e_struct*_tmp14A=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp10F;
if(_tmp14A->tag != 20)goto _LL87;else{_tmp14B=(void*)_tmp14A->f1;_tmp14C=(void*)
_tmp14A->f2;}}_LL86: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp14B),
_tmp14C,e->loc);_LL87: {struct Cyc_Absyn_Deref_e_struct*_tmp14D=(struct Cyc_Absyn_Deref_e_struct*)
_tmp10F;if(_tmp14D->tag != 21)goto _LL89;else{_tmp14E=_tmp14D->f1;}}_LL88: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp14E),e->loc);_LL89: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp14F=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp10F;if(_tmp14F->tag != 22)goto
_LL8B;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;_tmp152=_tmp14F->f3;_tmp153=
_tmp14F->f4;}}_LL8A: {struct Cyc_Absyn_AggrMember_e_struct _tmp10A1;struct Cyc_Absyn_AggrMember_e_struct*
_tmp10A0;return Cyc_Absyn_new_exp((void*)((_tmp10A0=_cycalloc(sizeof(*_tmp10A0)),((
_tmp10A0[0]=((_tmp10A1.tag=22,((_tmp10A1.f1=Cyc_Tcutil_deep_copy_exp(_tmp150),((
_tmp10A1.f2=_tmp151,((_tmp10A1.f3=_tmp152,((_tmp10A1.f4=_tmp153,_tmp10A1)))))))))),
_tmp10A0)))),e->loc);}_LL8B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp154=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp10F;if(_tmp154->tag != 23)goto _LL8D;else{
_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;_tmp158=_tmp154->f4;}}
_LL8C: {struct Cyc_Absyn_AggrArrow_e_struct _tmp10A4;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp10A3;return Cyc_Absyn_new_exp((void*)((_tmp10A3=_cycalloc(sizeof(*_tmp10A3)),((
_tmp10A3[0]=((_tmp10A4.tag=23,((_tmp10A4.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp10A4.f2=_tmp156,((_tmp10A4.f3=_tmp157,((_tmp10A4.f4=_tmp158,_tmp10A4)))))))))),
_tmp10A3)))),e->loc);}_LL8D: {struct Cyc_Absyn_Subscript_e_struct*_tmp159=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp10F;if(_tmp159->tag != 24)goto _LL8F;else{
_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;}}_LL8E: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp15A),Cyc_Tcutil_deep_copy_exp(_tmp15B),e->loc);
_LL8F: {struct Cyc_Absyn_Tuple_e_struct*_tmp15C=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp10F;if(_tmp15C->tag != 25)goto _LL91;else{_tmp15D=_tmp15C->f1;}}_LL90: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp15D),e->loc);
_LL91: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp15E=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp10F;if(_tmp15E->tag != 26)goto _LL93;else{_tmp15F=_tmp15E->f1;_tmp160=*_tmp15F;
_tmp161=_tmp160.f1;_tmp162=_tmp160.f2;_tmp163=_tmp160.f3;_tmp164=_tmp15E->f2;}}
_LL92: {struct Cyc_Core_Opt*vopt1=_tmp161;if(_tmp161 != 0){struct Cyc_Core_Opt*
_tmp10A5;vopt1=((_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((_tmp10A5->v=(struct
_dyneither_ptr*)_tmp161->v,_tmp10A5))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp10AB;struct _tuple9*_tmp10AA;struct Cyc_Absyn_CompoundLit_e_struct*_tmp10A9;
return Cyc_Absyn_new_exp((void*)((_tmp10A9=_cycalloc(sizeof(*_tmp10A9)),((
_tmp10A9[0]=((_tmp10AB.tag=26,((_tmp10AB.f1=((_tmp10AA=_cycalloc(sizeof(*
_tmp10AA)),((_tmp10AA->f1=vopt1,((_tmp10AA->f2=_tmp162,((_tmp10AA->f3=Cyc_Tcutil_copy_type(
_tmp163),_tmp10AA)))))))),((_tmp10AB.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp164),_tmp10AB)))))),_tmp10A9)))),e->loc);};}_LL93: {struct Cyc_Absyn_Array_e_struct*
_tmp165=(struct Cyc_Absyn_Array_e_struct*)_tmp10F;if(_tmp165->tag != 27)goto _LL95;
else{_tmp166=_tmp165->f1;}}_LL94: {struct Cyc_Absyn_Array_e_struct _tmp10AE;struct
Cyc_Absyn_Array_e_struct*_tmp10AD;return Cyc_Absyn_new_exp((void*)((_tmp10AD=
_cycalloc(sizeof(*_tmp10AD)),((_tmp10AD[0]=((_tmp10AE.tag=27,((_tmp10AE.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp166),_tmp10AE)))),_tmp10AD)))),e->loc);}
_LL95: {struct Cyc_Absyn_Comprehension_e_struct*_tmp167=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp10F;if(_tmp167->tag != 28)goto _LL97;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;
_tmp16A=_tmp167->f3;_tmp16B=_tmp167->f4;}}_LL96: {struct Cyc_Absyn_Comprehension_e_struct
_tmp10B1;struct Cyc_Absyn_Comprehension_e_struct*_tmp10B0;return Cyc_Absyn_new_exp((
void*)((_tmp10B0=_cycalloc(sizeof(*_tmp10B0)),((_tmp10B0[0]=((_tmp10B1.tag=28,((
_tmp10B1.f1=_tmp168,((_tmp10B1.f2=Cyc_Tcutil_deep_copy_exp(_tmp169),((_tmp10B1.f3=
Cyc_Tcutil_deep_copy_exp(_tmp16A),((_tmp10B1.f4=_tmp16B,_tmp10B1)))))))))),
_tmp10B0)))),e->loc);}_LL97: {struct Cyc_Absyn_Aggregate_e_struct*_tmp16C=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp10F;if(_tmp16C->tag != 29)goto _LL99;else{
_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;_tmp16F=_tmp16C->f3;_tmp170=_tmp16C->f4;}}
_LL98: {struct Cyc_Absyn_Aggregate_e_struct _tmp10B4;struct Cyc_Absyn_Aggregate_e_struct*
_tmp10B3;return Cyc_Absyn_new_exp((void*)((_tmp10B3=_cycalloc(sizeof(*_tmp10B3)),((
_tmp10B3[0]=((_tmp10B4.tag=29,((_tmp10B4.f1=_tmp16D,((_tmp10B4.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp16E),((_tmp10B4.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp16F),((
_tmp10B4.f4=_tmp170,_tmp10B4)))))))))),_tmp10B3)))),e->loc);}_LL99: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp171=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp10F;if(_tmp171->tag != 30)goto
_LL9B;else{_tmp172=(void*)_tmp171->f1;_tmp173=_tmp171->f2;}}_LL9A: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp10B7;struct Cyc_Absyn_AnonStruct_e_struct*_tmp10B6;return Cyc_Absyn_new_exp((
void*)((_tmp10B6=_cycalloc(sizeof(*_tmp10B6)),((_tmp10B6[0]=((_tmp10B7.tag=30,((
_tmp10B7.f1=(void*)Cyc_Tcutil_copy_type(_tmp172),((_tmp10B7.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp173),_tmp10B7)))))),_tmp10B6)))),e->loc);}_LL9B: {struct Cyc_Absyn_Datatype_e_struct*
_tmp174=(struct Cyc_Absyn_Datatype_e_struct*)_tmp10F;if(_tmp174->tag != 31)goto
_LL9D;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;_tmp177=_tmp174->f3;}}_LL9C: {
struct Cyc_Absyn_Datatype_e_struct _tmp10BA;struct Cyc_Absyn_Datatype_e_struct*
_tmp10B9;return Cyc_Absyn_new_exp((void*)((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((
_tmp10B9[0]=((_tmp10BA.tag=31,((_tmp10BA.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp175),((_tmp10BA.f2=_tmp176,((_tmp10BA.f3=_tmp177,_tmp10BA)))))))),_tmp10B9)))),
e->loc);}_LL9D: {struct Cyc_Absyn_Enum_e_struct*_tmp178=(struct Cyc_Absyn_Enum_e_struct*)
_tmp10F;if(_tmp178->tag != 32)goto _LL9F;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;
_tmp17B=_tmp178->f3;}}_LL9E: return e;_LL9F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp17C=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp10F;if(_tmp17C->tag != 33)goto
_LLA1;else{_tmp17D=_tmp17C->f1;_tmp17E=(void*)_tmp17C->f2;_tmp17F=_tmp17C->f3;}}
_LLA0: {struct Cyc_Absyn_AnonEnum_e_struct _tmp10BD;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp10BC;return Cyc_Absyn_new_exp((void*)((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((
_tmp10BC[0]=((_tmp10BD.tag=33,((_tmp10BD.f1=_tmp17D,((_tmp10BD.f2=(void*)Cyc_Tcutil_copy_type(
_tmp17E),((_tmp10BD.f3=_tmp17F,_tmp10BD)))))))),_tmp10BC)))),e->loc);}_LLA1: {
struct Cyc_Absyn_Malloc_e_struct*_tmp180=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp10F;if(_tmp180->tag != 34)goto _LLA3;else{_tmp181=_tmp180->f1;_tmp182=_tmp181.is_calloc;
_tmp183=_tmp181.rgn;_tmp184=_tmp181.elt_type;_tmp185=_tmp181.num_elts;_tmp186=
_tmp181.fat_result;}}_LLA2: {struct Cyc_Absyn_Exp*_tmp1BC=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp183;if(_tmp183 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp183);{void**t1=_tmp184;if(_tmp184 != 0){void**_tmp10BE;
t1=((_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE[0]=Cyc_Tcutil_copy_type(*
_tmp184),_tmp10BE))));}{struct Cyc_Absyn_Malloc_e_struct _tmp10C4;struct Cyc_Absyn_MallocInfo
_tmp10C3;struct Cyc_Absyn_Malloc_e_struct*_tmp10C2;_tmp1BC->r=(void*)((_tmp10C2=
_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2[0]=((_tmp10C4.tag=34,((_tmp10C4.f1=((
_tmp10C3.is_calloc=_tmp182,((_tmp10C3.rgn=r1,((_tmp10C3.elt_type=t1,((_tmp10C3.num_elts=
_tmp185,((_tmp10C3.fat_result=_tmp186,_tmp10C3)))))))))),_tmp10C4)))),_tmp10C2))));}
return _tmp1BC;};}_LLA3: {struct Cyc_Absyn_Swap_e_struct*_tmp187=(struct Cyc_Absyn_Swap_e_struct*)
_tmp10F;if(_tmp187->tag != 35)goto _LLA5;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}
_LLA4: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp188),Cyc_Tcutil_deep_copy_exp(
_tmp189),e->loc);_LLA5: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp18A=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp10F;if(_tmp18A->tag != 36)goto _LLA7;else{
_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLA6: {struct Cyc_Core_Opt*nopt1=
_tmp18B;if(_tmp18B != 0){struct Cyc_Core_Opt*_tmp10C5;nopt1=((_tmp10C5=_cycalloc(
sizeof(*_tmp10C5)),((_tmp10C5->v=(struct _tuple2*)_tmp18B->v,_tmp10C5))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp10C8;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp10C7;return Cyc_Absyn_new_exp((void*)((_tmp10C7=_cycalloc(sizeof(*_tmp10C7)),((
_tmp10C7[0]=((_tmp10C8.tag=36,((_tmp10C8.f1=nopt1,((_tmp10C8.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp18C),_tmp10C8)))))),_tmp10C7)))),e->loc);};}_LLA7: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp18D=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp10F;if(_tmp18D->tag != 37)goto
_LLA9;}_LLA8: {struct Cyc_Core_Failure_struct _tmp10CE;const char*_tmp10CD;struct
Cyc_Core_Failure_struct*_tmp10CC;(int)_throw((void*)((_tmp10CC=_cycalloc(sizeof(*
_tmp10CC)),((_tmp10CC[0]=((_tmp10CE.tag=Cyc_Core_Failure,((_tmp10CE.f1=((
_tmp10CD="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp10CD,
sizeof(char),45))),_tmp10CE)))),_tmp10CC)))));}_LLA9: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp18E=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp10F;if(_tmp18E->tag != 38)goto
_LLAB;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LLAA: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp10D1;struct Cyc_Absyn_Tagcheck_e_struct*_tmp10D0;return Cyc_Absyn_new_exp((
void*)((_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((_tmp10D0[0]=((_tmp10D1.tag=38,((
_tmp10D1.f1=Cyc_Tcutil_deep_copy_exp(_tmp18F),((_tmp10D1.f2=_tmp190,_tmp10D1)))))),
_tmp10D0)))),e->loc);}_LLAB: {struct Cyc_Absyn_Valueof_e_struct*_tmp191=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp10F;if(_tmp191->tag != 39)goto _LLAD;else{_tmp192=(
void*)_tmp191->f1;}}_LLAC: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp192),e->loc);_LLAD: {struct Cyc_Absyn_Asm_e_struct*_tmp193=(struct Cyc_Absyn_Asm_e_struct*)
_tmp10F;if(_tmp193->tag != 40)goto _LL5C;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;}}
_LLAE: return Cyc_Absyn_asm_exp(_tmp194,_tmp195,e->loc);_LL5C:;}struct _tuple19{
enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual
f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,
struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1CA;enum Cyc_Absyn_KindQual
_tmp1CB;enum Cyc_Absyn_AliasQual _tmp1CC;struct Cyc_Absyn_Kind*_tmp1C9=ka1;_tmp1CA=*
_tmp1C9;_tmp1CB=_tmp1CA.kind;_tmp1CC=_tmp1CA.aliasqual;{struct Cyc_Absyn_Kind
_tmp1CE;enum Cyc_Absyn_KindQual _tmp1CF;enum Cyc_Absyn_AliasQual _tmp1D0;struct Cyc_Absyn_Kind*
_tmp1CD=ka2;_tmp1CE=*_tmp1CD;_tmp1CF=_tmp1CE.kind;_tmp1D0=_tmp1CE.aliasqual;if(
_tmp1CB != _tmp1CF){struct _tuple19 _tmp10D2;struct _tuple19 _tmp1D2=(_tmp10D2.f1=
_tmp1CB,((_tmp10D2.f2=_tmp1CF,_tmp10D2)));enum Cyc_Absyn_KindQual _tmp1D3;enum 
Cyc_Absyn_KindQual _tmp1D4;enum Cyc_Absyn_KindQual _tmp1D5;enum Cyc_Absyn_KindQual
_tmp1D6;enum Cyc_Absyn_KindQual _tmp1D7;enum Cyc_Absyn_KindQual _tmp1D8;_LLB5:
_tmp1D3=_tmp1D2.f1;if(_tmp1D3 != Cyc_Absyn_BoxKind)goto _LLB7;_tmp1D4=_tmp1D2.f2;
if(_tmp1D4 != Cyc_Absyn_MemKind)goto _LLB7;_LLB6: goto _LLB8;_LLB7: _tmp1D5=_tmp1D2.f1;
if(_tmp1D5 != Cyc_Absyn_BoxKind)goto _LLB9;_tmp1D6=_tmp1D2.f2;if(_tmp1D6 != Cyc_Absyn_AnyKind)
goto _LLB9;_LLB8: goto _LLBA;_LLB9: _tmp1D7=_tmp1D2.f1;if(_tmp1D7 != Cyc_Absyn_MemKind)
goto _LLBB;_tmp1D8=_tmp1D2.f2;if(_tmp1D8 != Cyc_Absyn_AnyKind)goto _LLBB;_LLBA: goto
_LLB4;_LLBB:;_LLBC: return 0;_LLB4:;}if(_tmp1CC != _tmp1D0){struct _tuple20 _tmp10D3;
struct _tuple20 _tmp1DA=(_tmp10D3.f1=_tmp1CC,((_tmp10D3.f2=_tmp1D0,_tmp10D3)));
enum Cyc_Absyn_AliasQual _tmp1DB;enum Cyc_Absyn_AliasQual _tmp1DC;enum Cyc_Absyn_AliasQual
_tmp1DD;enum Cyc_Absyn_AliasQual _tmp1DE;_LLBE: _tmp1DB=_tmp1DA.f1;if(_tmp1DB != 
Cyc_Absyn_Aliasable)goto _LLC0;_tmp1DC=_tmp1DA.f2;if(_tmp1DC != Cyc_Absyn_Top)goto
_LLC0;_LLBF: goto _LLC1;_LLC0: _tmp1DD=_tmp1DA.f1;if(_tmp1DD != Cyc_Absyn_Unique)
goto _LLC2;_tmp1DE=_tmp1DA.f2;if(_tmp1DE != Cyc_Absyn_Top)goto _LLC2;_LLC1: return 1;
_LLC2:;_LLC3: return 0;_LLBD:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp1DF=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Absyn_Kind*_tmp1E3;_LLC5: {
struct Cyc_Absyn_Eq_kb_struct*_tmp1E0=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1DF;if(
_tmp1E0->tag != 0)goto _LLC7;else{_tmp1E1=_tmp1E0->f1;}}_LLC6: return _tmp1E1;_LLC7: {
struct Cyc_Absyn_Less_kb_struct*_tmp1E2=(struct Cyc_Absyn_Less_kb_struct*)_tmp1DF;
if(_tmp1E2->tag != 2)goto _LLC9;else{_tmp1E3=_tmp1E2->f2;}}_LLC8: return _tmp1E3;
_LLC9:;_LLCA:{struct Cyc_Absyn_Less_kb_struct _tmp10D6;struct Cyc_Absyn_Less_kb_struct*
_tmp10D5;tv->kind=(void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((
_tmp10D6.tag=2,((_tmp10D6.f1=0,((_tmp10D6.f2=def,_tmp10D6)))))),_tmp10D5))));}
return def;_LLC4:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp10D7;struct _tuple0 _tmp1E7=(_tmp10D7.f1=Cyc_Absyn_compress_kb(kb1),((_tmp10D7.f2=
Cyc_Absyn_compress_kb(kb2),_tmp10D7)));void*_tmp1E8;struct Cyc_Absyn_Kind*_tmp1EA;
void*_tmp1EB;struct Cyc_Absyn_Kind*_tmp1ED;void*_tmp1EE;struct Cyc_Core_Opt*
_tmp1F0;struct Cyc_Core_Opt**_tmp1F1;struct Cyc_Absyn_Kind*_tmp1F2;void*_tmp1F3;
struct Cyc_Absyn_Kind*_tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Kind*_tmp1F8;void*
_tmp1F9;struct Cyc_Core_Opt*_tmp1FB;struct Cyc_Core_Opt**_tmp1FC;struct Cyc_Absyn_Kind*
_tmp1FD;void*_tmp1FE;struct Cyc_Core_Opt*_tmp200;struct Cyc_Core_Opt**_tmp201;
struct Cyc_Absyn_Kind*_tmp202;void*_tmp203;struct Cyc_Core_Opt*_tmp205;struct Cyc_Core_Opt**
_tmp206;struct Cyc_Absyn_Kind*_tmp207;void*_tmp208;struct Cyc_Core_Opt*_tmp20A;
struct Cyc_Core_Opt**_tmp20B;void*_tmp20C;void*_tmp20D;void*_tmp20E;struct Cyc_Core_Opt*
_tmp210;struct Cyc_Core_Opt**_tmp211;_LLCC: _tmp1E8=_tmp1E7.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1E9=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1E8;if(_tmp1E9->tag != 0)goto _LLCE;
else{_tmp1EA=_tmp1E9->f1;}};_tmp1EB=_tmp1E7.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1EC=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1EB;if(_tmp1EC->tag != 0)goto _LLCE;
else{_tmp1ED=_tmp1EC->f1;}};_LLCD: return _tmp1EA == _tmp1ED;_LLCE: _tmp1EE=_tmp1E7.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp1EF=(struct Cyc_Absyn_Less_kb_struct*)_tmp1EE;
if(_tmp1EF->tag != 2)goto _LLD0;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=(struct Cyc_Core_Opt**)&
_tmp1EF->f1;_tmp1F2=_tmp1EF->f2;}};_tmp1F3=_tmp1E7.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp1F4=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F3;if(_tmp1F4->tag != 0)goto _LLD0;
else{_tmp1F5=_tmp1F4->f1;}};_LLCF: if(Cyc_Tcutil_kind_leq(_tmp1F5,_tmp1F2)){{void**
_tmp10D8;*_tmp1F1=(struct Cyc_Core_Opt*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((
_tmp10D8[0]=kb2,_tmp10D8))));}return 1;}else{return 0;}_LLD0: _tmp1F6=_tmp1E7.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp1F7=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F6;if(
_tmp1F7->tag != 0)goto _LLD2;else{_tmp1F8=_tmp1F7->f1;}};_tmp1F9=_tmp1E7.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp1FA=(struct Cyc_Absyn_Less_kb_struct*)_tmp1F9;
if(_tmp1FA->tag != 2)goto _LLD2;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=(struct Cyc_Core_Opt**)&
_tmp1FA->f1;_tmp1FD=_tmp1FA->f2;}};_LLD1: if(Cyc_Tcutil_kind_leq(_tmp1F8,_tmp1FD)){{
void**_tmp10D9;*_tmp1FC=(struct Cyc_Core_Opt*)((_tmp10D9=_cycalloc(sizeof(*
_tmp10D9)),((_tmp10D9[0]=kb1,_tmp10D9))));}return 1;}else{return 0;}_LLD2: _tmp1FE=
_tmp1E7.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp1FF=(struct Cyc_Absyn_Less_kb_struct*)
_tmp1FE;if(_tmp1FF->tag != 2)goto _LLD4;else{_tmp200=_tmp1FF->f1;_tmp201=(struct
Cyc_Core_Opt**)& _tmp1FF->f1;_tmp202=_tmp1FF->f2;}};_tmp203=_tmp1E7.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp204=(struct Cyc_Absyn_Less_kb_struct*)_tmp203;if(_tmp204->tag != 2)goto _LLD4;
else{_tmp205=_tmp204->f1;_tmp206=(struct Cyc_Core_Opt**)& _tmp204->f1;_tmp207=
_tmp204->f2;}};_LLD3: if(Cyc_Tcutil_kind_leq(_tmp202,_tmp207)){{void**_tmp10DA;*
_tmp206=(struct Cyc_Core_Opt*)((_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((_tmp10DA[
0]=kb1,_tmp10DA))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp207,_tmp202)){{
void**_tmp10DB;*_tmp201=(struct Cyc_Core_Opt*)((_tmp10DB=_cycalloc(sizeof(*
_tmp10DB)),((_tmp10DB[0]=kb2,_tmp10DB))));}return 1;}else{return 0;}}_LLD4: _tmp208=
_tmp1E7.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp209=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp208;if(_tmp209->tag != 1)goto _LLD6;else{_tmp20A=_tmp209->f1;_tmp20B=(struct
Cyc_Core_Opt**)& _tmp209->f1;}};_tmp20C=_tmp1E7.f2;_LLD5: _tmp20D=_tmp20C;_tmp211=
_tmp20B;goto _LLD7;_LLD6: _tmp20D=_tmp1E7.f1;_tmp20E=_tmp1E7.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp20F=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp20E;if(_tmp20F->tag != 1)goto
_LLCB;else{_tmp210=_tmp20F->f1;_tmp211=(struct Cyc_Core_Opt**)& _tmp20F->f1;}};
_LLD7:{void**_tmp10DC;*_tmp211=(struct Cyc_Core_Opt*)((_tmp10DC=_cycalloc(sizeof(*
_tmp10DC)),((_tmp10DC[0]=_tmp20D,_tmp10DC))));}return 1;_LLCB:;}struct _tuple15 Cyc_Tcutil_swap_kind(
void*t,void*kb){void*_tmp217=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp219;
_LLD9: {struct Cyc_Absyn_VarType_struct*_tmp218=(struct Cyc_Absyn_VarType_struct*)
_tmp217;if(_tmp218->tag != 2)goto _LLDB;else{_tmp219=_tmp218->f1;}}_LLDA: {void*
_tmp21A=_tmp219->kind;_tmp219->kind=kb;{struct _tuple15 _tmp10DD;return(_tmp10DD.f1=
_tmp219,((_tmp10DD.f2=_tmp21A,_tmp10DD)));};}_LLDB:;_LLDC: {const char*_tmp10E1;
void*_tmp10E0[1];struct Cyc_String_pa_struct _tmp10DF;(_tmp10DF.tag=0,((_tmp10DF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp10E0[0]=& _tmp10DF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10E1="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp10E1,sizeof(char),40))),_tag_dyneither(_tmp10E0,sizeof(void*),
1)))))));}_LLD8:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp21F=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp221;struct Cyc_Absyn_Tvar*_tmp223;
enum Cyc_Absyn_Size_of _tmp226;struct Cyc_Absyn_DatatypeFieldInfo _tmp232;union Cyc_Absyn_DatatypeFieldInfoU
_tmp233;struct _tuple3 _tmp234;struct Cyc_Absyn_Datatypedecl*_tmp235;struct Cyc_Absyn_Datatypefield*
_tmp236;struct Cyc_Absyn_DatatypeFieldInfo _tmp238;union Cyc_Absyn_DatatypeFieldInfoU
_tmp239;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp23A;struct Cyc_Absyn_AggrInfo
_tmp23C;union Cyc_Absyn_AggrInfoU _tmp23D;struct _tuple4 _tmp23E;struct Cyc_Absyn_AggrInfo
_tmp240;union Cyc_Absyn_AggrInfoU _tmp241;struct Cyc_Absyn_Aggrdecl**_tmp242;struct
Cyc_Absyn_Aggrdecl*_tmp243;struct Cyc_Absyn_Aggrdecl _tmp244;struct Cyc_Absyn_AggrdeclImpl*
_tmp245;struct Cyc_Absyn_PtrInfo _tmp248;struct Cyc_Absyn_Typedefdecl*_tmp24E;_LLDE: {
struct Cyc_Absyn_Evar_struct*_tmp220=(struct Cyc_Absyn_Evar_struct*)_tmp21F;if(
_tmp220->tag != 1)goto _LLE0;else{_tmp221=_tmp220->f1;}}_LLDF: return(struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(_tmp221))->v;_LLE0: {struct Cyc_Absyn_VarType_struct*
_tmp222=(struct Cyc_Absyn_VarType_struct*)_tmp21F;if(_tmp222->tag != 2)goto _LLE2;
else{_tmp223=_tmp222->f1;}}_LLE1: return Cyc_Tcutil_tvar_kind(_tmp223,& Cyc_Tcutil_bk);
_LLE2: {struct Cyc_Absyn_VoidType_struct*_tmp224=(struct Cyc_Absyn_VoidType_struct*)
_tmp21F;if(_tmp224->tag != 0)goto _LLE4;}_LLE3: return& Cyc_Tcutil_mk;_LLE4: {struct
Cyc_Absyn_IntType_struct*_tmp225=(struct Cyc_Absyn_IntType_struct*)_tmp21F;if(
_tmp225->tag != 6)goto _LLE6;else{_tmp226=_tmp225->f2;}}_LLE5: return(_tmp226 == (
enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp226 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?&
Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLE6: {struct Cyc_Absyn_FloatType_struct*_tmp227=(
struct Cyc_Absyn_FloatType_struct*)_tmp21F;if(_tmp227->tag != 7)goto _LLE8;}_LLE7:
goto _LLE9;_LLE8: {struct Cyc_Absyn_DoubleType_struct*_tmp228=(struct Cyc_Absyn_DoubleType_struct*)
_tmp21F;if(_tmp228->tag != 8)goto _LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Absyn_FnType_struct*
_tmp229=(struct Cyc_Absyn_FnType_struct*)_tmp21F;if(_tmp229->tag != 10)goto _LLEC;}
_LLEB: return& Cyc_Tcutil_mk;_LLEC: {struct Cyc_Absyn_DynRgnType_struct*_tmp22A=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp21F;if(_tmp22A->tag != 17)goto _LLEE;}_LLED:
goto _LLEF;_LLEE: {struct Cyc_Absyn_EnumType_struct*_tmp22B=(struct Cyc_Absyn_EnumType_struct*)
_tmp21F;if(_tmp22B->tag != 14)goto _LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp22C=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp21F;if(_tmp22C->tag != 15)goto
_LLF2;}_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp22D=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp21F;if(_tmp22D->tag != 16)goto _LLF4;}
_LLF3: return& Cyc_Tcutil_bk;_LLF4: {struct Cyc_Absyn_UniqueRgn_struct*_tmp22E=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp21F;if(_tmp22E->tag != 22)goto _LLF6;}_LLF5:
return& Cyc_Tcutil_urk;_LLF6: {struct Cyc_Absyn_HeapRgn_struct*_tmp22F=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp21F;if(_tmp22F->tag != 21)goto _LLF8;}_LLF7: return& Cyc_Tcutil_rk;_LLF8: {
struct Cyc_Absyn_DatatypeType_struct*_tmp230=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp21F;if(_tmp230->tag != 3)goto _LLFA;}_LLF9: return& Cyc_Tcutil_ak;_LLFA: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp231=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp21F;if(_tmp231->tag != 4)goto _LLFC;else{_tmp232=_tmp231->f1;_tmp233=_tmp232.field_info;
if((_tmp233.KnownDatatypefield).tag != 2)goto _LLFC;_tmp234=(struct _tuple3)(
_tmp233.KnownDatatypefield).val;_tmp235=_tmp234.f1;_tmp236=_tmp234.f2;}}_LLFB:
return& Cyc_Tcutil_mk;_LLFC: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp237=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp21F;if(_tmp237->tag != 4)goto _LLFE;
else{_tmp238=_tmp237->f1;_tmp239=_tmp238.field_info;if((_tmp239.UnknownDatatypefield).tag
!= 1)goto _LLFE;_tmp23A=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp239.UnknownDatatypefield).val;}}
_LLFD: {const char*_tmp10E4;void*_tmp10E3;(_tmp10E3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E4="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp10E4,sizeof(char),39))),_tag_dyneither(_tmp10E3,sizeof(void*),
0)));}_LLFE: {struct Cyc_Absyn_AggrType_struct*_tmp23B=(struct Cyc_Absyn_AggrType_struct*)
_tmp21F;if(_tmp23B->tag != 12)goto _LL100;else{_tmp23C=_tmp23B->f1;_tmp23D=_tmp23C.aggr_info;
if((_tmp23D.UnknownAggr).tag != 1)goto _LL100;_tmp23E=(struct _tuple4)(_tmp23D.UnknownAggr).val;}}
_LLFF: return& Cyc_Tcutil_ak;_LL100: {struct Cyc_Absyn_AggrType_struct*_tmp23F=(
struct Cyc_Absyn_AggrType_struct*)_tmp21F;if(_tmp23F->tag != 12)goto _LL102;else{
_tmp240=_tmp23F->f1;_tmp241=_tmp240.aggr_info;if((_tmp241.KnownAggr).tag != 2)
goto _LL102;_tmp242=(struct Cyc_Absyn_Aggrdecl**)(_tmp241.KnownAggr).val;_tmp243=*
_tmp242;_tmp244=*_tmp243;_tmp245=_tmp244.impl;}}_LL101: return _tmp245 == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LL102: {struct Cyc_Absyn_AnonAggrType_struct*_tmp246=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp21F;if(_tmp246->tag != 13)goto _LL104;}_LL103: return& Cyc_Tcutil_mk;_LL104: {
struct Cyc_Absyn_PointerType_struct*_tmp247=(struct Cyc_Absyn_PointerType_struct*)
_tmp21F;if(_tmp247->tag != 5)goto _LL106;else{_tmp248=_tmp247->f1;}}_LL105: {void*
_tmp254=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp248.ptr_atts).bounds);_LL117: {struct Cyc_Absyn_DynEither_b_struct*
_tmp255=(struct Cyc_Absyn_DynEither_b_struct*)_tmp254;if(_tmp255->tag != 0)goto
_LL119;}_LL118: {struct Cyc_Absyn_Kind*_tmp10E5;return(_tmp10E5=_cycalloc_atomic(
sizeof(*_tmp10E5)),((_tmp10E5->kind=Cyc_Absyn_MemKind,((_tmp10E5->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp248.ptr_atts).rgn))->aliasqual,_tmp10E5)))));}_LL119: {struct Cyc_Absyn_Upper_b_struct*
_tmp256=(struct Cyc_Absyn_Upper_b_struct*)_tmp254;if(_tmp256->tag != 1)goto _LL116;}
_LL11A: {struct Cyc_Absyn_Kind*_tmp10E6;return(_tmp10E6=_cycalloc_atomic(sizeof(*
_tmp10E6)),((_tmp10E6->kind=Cyc_Absyn_BoxKind,((_tmp10E6->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp248.ptr_atts).rgn))->aliasqual,_tmp10E6)))));}_LL116:;}_LL106: {struct Cyc_Absyn_ValueofType_struct*
_tmp249=(struct Cyc_Absyn_ValueofType_struct*)_tmp21F;if(_tmp249->tag != 19)goto
_LL108;}_LL107: return& Cyc_Tcutil_ik;_LL108: {struct Cyc_Absyn_TagType_struct*
_tmp24A=(struct Cyc_Absyn_TagType_struct*)_tmp21F;if(_tmp24A->tag != 20)goto _LL10A;}
_LL109: return& Cyc_Tcutil_bk;_LL10A: {struct Cyc_Absyn_ArrayType_struct*_tmp24B=(
struct Cyc_Absyn_ArrayType_struct*)_tmp21F;if(_tmp24B->tag != 9)goto _LL10C;}_LL10B:
goto _LL10D;_LL10C: {struct Cyc_Absyn_TupleType_struct*_tmp24C=(struct Cyc_Absyn_TupleType_struct*)
_tmp21F;if(_tmp24C->tag != 11)goto _LL10E;}_LL10D: return& Cyc_Tcutil_mk;_LL10E: {
struct Cyc_Absyn_TypedefType_struct*_tmp24D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp21F;if(_tmp24D->tag != 18)goto _LL110;else{_tmp24E=_tmp24D->f3;}}_LL10F: if(
_tmp24E == 0  || _tmp24E->kind == 0){const char*_tmp10EA;void*_tmp10E9[1];struct Cyc_String_pa_struct
_tmp10E8;(_tmp10E8.tag=0,((_tmp10E8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10E9[0]=& _tmp10E8,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10EA="typ_kind: typedef found: %s",
_tag_dyneither(_tmp10EA,sizeof(char),28))),_tag_dyneither(_tmp10E9,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp24E->kind))->v;
_LL110: {struct Cyc_Absyn_AccessEff_struct*_tmp24F=(struct Cyc_Absyn_AccessEff_struct*)
_tmp21F;if(_tmp24F->tag != 23)goto _LL112;}_LL111: goto _LL113;_LL112: {struct Cyc_Absyn_JoinEff_struct*
_tmp250=(struct Cyc_Absyn_JoinEff_struct*)_tmp21F;if(_tmp250->tag != 24)goto _LL114;}
_LL113: goto _LL115;_LL114: {struct Cyc_Absyn_RgnsEff_struct*_tmp251=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp21F;if(_tmp251->tag != 25)goto _LLDD;}_LL115: return& Cyc_Tcutil_ek;_LLDD:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == 
k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp25C;_push_handler(& _tmp25C);{int _tmp25E=
0;if(setjmp(_tmp25C.handler))_tmp25E=1;if(!_tmp25E){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp25F=1;_npop_handler(0);return _tmp25F;};;_pop_handler();}else{void*_tmp25D=(
void*)_exn_thrown;void*_tmp261=_tmp25D;_LL11C: {struct Cyc_Tcutil_Unify_struct*
_tmp262=(struct Cyc_Tcutil_Unify_struct*)_tmp261;if(_tmp262->tag != Cyc_Tcutil_Unify)
goto _LL11E;}_LL11D: return 0;_LL11E:;_LL11F:(void)_throw(_tmp261);_LL11B:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*
_tmp263=t;struct Cyc_Absyn_Tvar*_tmp265;struct Cyc_Core_Opt*_tmp267;struct Cyc_Core_Opt*
_tmp268;struct Cyc_Core_Opt**_tmp269;struct Cyc_Absyn_PtrInfo _tmp26B;struct Cyc_Absyn_ArrayInfo
_tmp26D;void*_tmp26E;struct Cyc_Absyn_FnInfo _tmp270;struct Cyc_List_List*_tmp271;
struct Cyc_Core_Opt*_tmp272;void*_tmp273;struct Cyc_List_List*_tmp274;int _tmp275;
struct Cyc_Absyn_VarargInfo*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*
_tmp278;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_DatatypeInfo _tmp27C;struct
Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27F;struct Cyc_Absyn_DatatypeFieldInfo
_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_AggrInfo _tmp284;struct Cyc_List_List*
_tmp285;struct Cyc_List_List*_tmp287;void*_tmp289;void*_tmp28B;void*_tmp28D;void*
_tmp28F;struct Cyc_List_List*_tmp291;_LL121: {struct Cyc_Absyn_VarType_struct*
_tmp264=(struct Cyc_Absyn_VarType_struct*)_tmp263;if(_tmp264->tag != 2)goto _LL123;
else{_tmp265=_tmp264->f1;}}_LL122: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmp265)){{const char*_tmp10EB;Cyc_Tcutil_failure_reason=((
_tmp10EB="(type variable would escape scope)",_tag_dyneither(_tmp10EB,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL120;_LL123: {
struct Cyc_Absyn_Evar_struct*_tmp266=(struct Cyc_Absyn_Evar_struct*)_tmp263;if(
_tmp266->tag != 1)goto _LL125;else{_tmp267=_tmp266->f2;_tmp268=_tmp266->f4;_tmp269=(
struct Cyc_Core_Opt**)& _tmp266->f4;}}_LL124: if(t == evar){{const char*_tmp10EC;Cyc_Tcutil_failure_reason=((
_tmp10EC="(occurs check)",_tag_dyneither(_tmp10EC,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp267 != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp267->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp269))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp294=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp269))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp10ED;_tmp294=((
_tmp10ED=_cycalloc(sizeof(*_tmp10ED)),((_tmp10ED->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp10ED->tl=_tmp294,_tmp10ED))))));}}}{struct Cyc_Core_Opt*_tmp10EE;*_tmp269=((
_tmp10EE=_cycalloc(sizeof(*_tmp10EE)),((_tmp10EE->v=_tmp294,_tmp10EE))));};}}}
goto _LL120;_LL125: {struct Cyc_Absyn_PointerType_struct*_tmp26A=(struct Cyc_Absyn_PointerType_struct*)
_tmp263;if(_tmp26A->tag != 5)goto _LL127;else{_tmp26B=_tmp26A->f1;}}_LL126: Cyc_Tcutil_occurs(
evar,r,env,_tmp26B.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp26B.ptr_atts).rgn);
goto _LL120;_LL127: {struct Cyc_Absyn_ArrayType_struct*_tmp26C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp263;if(_tmp26C->tag != 9)goto _LL129;else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26D.elt_type;}}
_LL128: Cyc_Tcutil_occurs(evar,r,env,_tmp26E);goto _LL120;_LL129: {struct Cyc_Absyn_FnType_struct*
_tmp26F=(struct Cyc_Absyn_FnType_struct*)_tmp263;if(_tmp26F->tag != 10)goto _LL12B;
else{_tmp270=_tmp26F->f1;_tmp271=_tmp270.tvars;_tmp272=_tmp270.effect;_tmp273=
_tmp270.ret_typ;_tmp274=_tmp270.args;_tmp275=_tmp270.c_varargs;_tmp276=_tmp270.cyc_varargs;
_tmp277=_tmp270.rgn_po;_tmp278=_tmp270.attributes;}}_LL12A: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp271,env);if(_tmp272 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp272->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp273);for(0;_tmp274 != 0;_tmp274=_tmp274->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp274->hd)).f3);}if(_tmp276 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp276->type);for(0;_tmp277 != 0;_tmp277=_tmp277->tl){struct _tuple0
_tmp298;void*_tmp299;void*_tmp29A;struct _tuple0*_tmp297=(struct _tuple0*)_tmp277->hd;
_tmp298=*_tmp297;_tmp299=_tmp298.f1;_tmp29A=_tmp298.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp299);Cyc_Tcutil_occurs(evar,r,env,_tmp29A);}goto _LL120;_LL12B: {struct
Cyc_Absyn_TupleType_struct*_tmp279=(struct Cyc_Absyn_TupleType_struct*)_tmp263;
if(_tmp279->tag != 11)goto _LL12D;else{_tmp27A=_tmp279->f1;}}_LL12C: for(0;_tmp27A
!= 0;_tmp27A=_tmp27A->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)
_tmp27A->hd)).f2);}goto _LL120;_LL12D: {struct Cyc_Absyn_DatatypeType_struct*
_tmp27B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp263;if(_tmp27B->tag != 3)goto
_LL12F;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27C.targs;}}_LL12E: Cyc_Tcutil_occurslist(
evar,r,env,_tmp27D);goto _LL120;_LL12F: {struct Cyc_Absyn_TypedefType_struct*
_tmp27E=(struct Cyc_Absyn_TypedefType_struct*)_tmp263;if(_tmp27E->tag != 18)goto
_LL131;else{_tmp27F=_tmp27E->f2;}}_LL130: _tmp282=_tmp27F;goto _LL132;_LL131: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp280=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp263;if(_tmp280->tag != 4)goto _LL133;else{_tmp281=_tmp280->f1;_tmp282=_tmp281.targs;}}
_LL132: _tmp285=_tmp282;goto _LL134;_LL133: {struct Cyc_Absyn_AggrType_struct*
_tmp283=(struct Cyc_Absyn_AggrType_struct*)_tmp263;if(_tmp283->tag != 12)goto
_LL135;else{_tmp284=_tmp283->f1;_tmp285=_tmp284.targs;}}_LL134: Cyc_Tcutil_occurslist(
evar,r,env,_tmp285);goto _LL120;_LL135: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp286=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp263;if(_tmp286->tag != 13)goto
_LL137;else{_tmp287=_tmp286->f2;}}_LL136: for(0;_tmp287 != 0;_tmp287=_tmp287->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp287->hd)->type);}
goto _LL120;_LL137: {struct Cyc_Absyn_TagType_struct*_tmp288=(struct Cyc_Absyn_TagType_struct*)
_tmp263;if(_tmp288->tag != 20)goto _LL139;else{_tmp289=(void*)_tmp288->f1;}}_LL138:
_tmp28B=_tmp289;goto _LL13A;_LL139: {struct Cyc_Absyn_AccessEff_struct*_tmp28A=(
struct Cyc_Absyn_AccessEff_struct*)_tmp263;if(_tmp28A->tag != 23)goto _LL13B;else{
_tmp28B=(void*)_tmp28A->f1;}}_LL13A: _tmp28D=_tmp28B;goto _LL13C;_LL13B: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp28C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp263;if(_tmp28C->tag != 16)goto _LL13D;else{_tmp28D=(void*)_tmp28C->f1;}}_LL13C:
_tmp28F=_tmp28D;goto _LL13E;_LL13D: {struct Cyc_Absyn_RgnsEff_struct*_tmp28E=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp263;if(_tmp28E->tag != 25)goto _LL13F;else{
_tmp28F=(void*)_tmp28E->f1;}}_LL13E: Cyc_Tcutil_occurs(evar,r,env,_tmp28F);goto
_LL120;_LL13F: {struct Cyc_Absyn_JoinEff_struct*_tmp290=(struct Cyc_Absyn_JoinEff_struct*)
_tmp263;if(_tmp290->tag != 24)goto _LL141;else{_tmp291=_tmp290->f1;}}_LL140: Cyc_Tcutil_occurslist(
evar,r,env,_tmp291);goto _LL120;_LL141:;_LL142: goto _LL120;_LL120:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp10F1;void*
_tmp10F0;(_tmp10F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp10F1="tq1 real_const not set.",_tag_dyneither(_tmp10F1,
sizeof(char),24))),_tag_dyneither(_tmp10F0,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp10F4;void*_tmp10F3;(_tmp10F3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F4="tq2 real_const not set.",
_tag_dyneither(_tmp10F4,sizeof(char),24))),_tag_dyneither(_tmp10F3,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp10F5;Cyc_Tcutil_failure_reason=((_tmp10F5="(qualifiers don't match)",
_tag_dyneither(_tmp10F5,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2A0=x;union Cyc_Absyn_Constraint _tmp2A1;int _tmp2A2;union Cyc_Absyn_Constraint
_tmp2A3;void*_tmp2A4;union Cyc_Absyn_Constraint _tmp2A5;union Cyc_Absyn_Constraint*
_tmp2A6;_LL144: _tmp2A1=*_tmp2A0;if((_tmp2A1.No_constr).tag != 3)goto _LL146;
_tmp2A2=(int)(_tmp2A1.No_constr).val;_LL145:{union Cyc_Absyn_Constraint _tmp10F6;*
x=(((_tmp10F6.Forward_constr).val=y,(((_tmp10F6.Forward_constr).tag=2,_tmp10F6))));}
return;_LL146: _tmp2A3=*_tmp2A0;if((_tmp2A3.Eq_constr).tag != 1)goto _LL148;_tmp2A4=(
void*)(_tmp2A3.Eq_constr).val;_LL147: {union Cyc_Absyn_Constraint*_tmp2A8=y;union
Cyc_Absyn_Constraint _tmp2A9;int _tmp2AA;union Cyc_Absyn_Constraint _tmp2AB;void*
_tmp2AC;union Cyc_Absyn_Constraint _tmp2AD;union Cyc_Absyn_Constraint*_tmp2AE;
_LL14B: _tmp2A9=*_tmp2A8;if((_tmp2A9.No_constr).tag != 3)goto _LL14D;_tmp2AA=(int)(
_tmp2A9.No_constr).val;_LL14C:*y=*x;return;_LL14D: _tmp2AB=*_tmp2A8;if((_tmp2AB.Eq_constr).tag
!= 1)goto _LL14F;_tmp2AC=(void*)(_tmp2AB.Eq_constr).val;_LL14E: if(cmp(_tmp2A4,
_tmp2AC)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL14F: _tmp2AD=*_tmp2A8;if((_tmp2AD.Forward_constr).tag != 2)goto _LL14A;
_tmp2AE=(union Cyc_Absyn_Constraint*)(_tmp2AD.Forward_constr).val;_LL150: {const
char*_tmp10F9;void*_tmp10F8;(_tmp10F8=0,Cyc_Tcutil_impos(((_tmp10F9="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp10F9,sizeof(char),40))),_tag_dyneither(_tmp10F8,sizeof(void*),
0)));}_LL14A:;}_LL148: _tmp2A5=*_tmp2A0;if((_tmp2A5.Forward_constr).tag != 2)goto
_LL143;_tmp2A6=(union Cyc_Absyn_Constraint*)(_tmp2A5.Forward_constr).val;_LL149: {
const char*_tmp10FC;void*_tmp10FB;(_tmp10FB=0,Cyc_Tcutil_impos(((_tmp10FC="unify_conref: forward after compress",
_tag_dyneither(_tmp10FC,sizeof(char),37))),_tag_dyneither(_tmp10FB,sizeof(void*),
0)));}_LL143:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2B3;
_push_handler(& _tmp2B3);{int _tmp2B5=0;if(setjmp(_tmp2B3.handler))_tmp2B5=1;if(!
_tmp2B5){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2B6=1;_npop_handler(0);return _tmp2B6;};;
_pop_handler();}else{void*_tmp2B4=(void*)_exn_thrown;void*_tmp2B8=_tmp2B4;_LL152: {
struct Cyc_Tcutil_Unify_struct*_tmp2B9=(struct Cyc_Tcutil_Unify_struct*)_tmp2B8;
if(_tmp2B9->tag != Cyc_Tcutil_Unify)goto _LL154;}_LL153: return 0;_LL154:;_LL155:(
void)_throw(_tmp2B8);_LL151:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp10FD;struct _tuple0 _tmp2BB=(_tmp10FD.f1=b1,((_tmp10FD.f2=b2,
_tmp10FD)));void*_tmp2BC;void*_tmp2BE;void*_tmp2C0;void*_tmp2C2;void*_tmp2C4;
struct Cyc_Absyn_Exp*_tmp2C6;void*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C9;_LL157:
_tmp2BC=_tmp2BB.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2BD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2BC;if(_tmp2BD->tag != 0)goto _LL159;};_tmp2BE=_tmp2BB.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2BF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2BE;if(_tmp2BF->tag != 0)goto
_LL159;};_LL158: return 0;_LL159: _tmp2C0=_tmp2BB.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2C1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2C0;if(_tmp2C1->tag != 0)goto
_LL15B;};_LL15A: return - 1;_LL15B: _tmp2C2=_tmp2BB.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2C3=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2C2;if(_tmp2C3->tag != 0)goto
_LL15D;};_LL15C: return 1;_LL15D: _tmp2C4=_tmp2BB.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2C5=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C4;if(_tmp2C5->tag != 1)goto _LL156;
else{_tmp2C6=_tmp2C5->f1;}};_tmp2C7=_tmp2BB.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2C8=(struct Cyc_Absyn_Upper_b_struct*)_tmp2C7;if(_tmp2C8->tag != 1)goto _LL156;
else{_tmp2C9=_tmp2C8->f1;}};_LL15E: return Cyc_Evexp_const_exp_cmp(_tmp2C6,_tmp2C9);
_LL156:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0
_tmp10FE;struct _tuple0 _tmp2CB=(_tmp10FE.f1=b1,((_tmp10FE.f2=b2,_tmp10FE)));void*
_tmp2CC;void*_tmp2CE;void*_tmp2D0;void*_tmp2D2;void*_tmp2D4;struct Cyc_Absyn_Exp*
_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D9;_LL160: _tmp2CC=_tmp2CB.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp2CD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2CC;if(_tmp2CD->tag != 0)goto _LL162;};_tmp2CE=_tmp2CB.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2CF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2CE;if(_tmp2CF->tag != 0)goto
_LL162;};_LL161: return 0;_LL162: _tmp2D0=_tmp2CB.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2D1=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2D0;if(_tmp2D1->tag != 0)goto
_LL164;};_LL163: return - 1;_LL164: _tmp2D2=_tmp2CB.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2D3=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2D2;if(_tmp2D3->tag != 0)goto
_LL166;};_LL165: return 1;_LL166: _tmp2D4=_tmp2CB.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2D5=(struct Cyc_Absyn_Upper_b_struct*)_tmp2D4;if(_tmp2D5->tag != 1)goto _LL15F;
else{_tmp2D6=_tmp2D5->f1;}};_tmp2D7=_tmp2CB.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2D8=(struct Cyc_Absyn_Upper_b_struct*)_tmp2D7;if(_tmp2D8->tag != 1)goto _LL15F;
else{_tmp2D9=_tmp2D8->f1;}};_LL167: return Cyc_Evexp_const_exp_cmp(_tmp2D6,_tmp2D9);
_LL15F:;}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp2DA=att;
_LL169: {struct Cyc_Absyn_Regparm_att_struct*_tmp2DB=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp2DA;if(_tmp2DB->tag != 0)goto _LL16B;}_LL16A: return 0;_LL16B: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2DC=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2DA;if(_tmp2DC->tag != 1)goto
_LL16D;}_LL16C: return 1;_LL16D: {struct Cyc_Absyn_Cdecl_att_struct*_tmp2DD=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp2DA;if(_tmp2DD->tag != 2)goto _LL16F;}_LL16E:
return 2;_LL16F: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2DE=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2DA;if(_tmp2DE->tag != 3)goto _LL171;}_LL170: return 3;_LL171: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp2DF=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp2DA;if(_tmp2DF->tag != 4)goto
_LL173;}_LL172: return 4;_LL173: {struct Cyc_Absyn_Const_att_struct*_tmp2E0=(struct
Cyc_Absyn_Const_att_struct*)_tmp2DA;if(_tmp2E0->tag != 5)goto _LL175;}_LL174:
return 5;_LL175: {struct Cyc_Absyn_Aligned_att_struct*_tmp2E1=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp2DA;if(_tmp2E1->tag != 6)goto _LL177;}_LL176: return 6;_LL177: {struct Cyc_Absyn_Packed_att_struct*
_tmp2E2=(struct Cyc_Absyn_Packed_att_struct*)_tmp2DA;if(_tmp2E2->tag != 7)goto
_LL179;}_LL178: return 7;_LL179: {struct Cyc_Absyn_Section_att_struct*_tmp2E3=(
struct Cyc_Absyn_Section_att_struct*)_tmp2DA;if(_tmp2E3->tag != 8)goto _LL17B;}
_LL17A: return 8;_LL17B: {struct Cyc_Absyn_Nocommon_att_struct*_tmp2E4=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp2DA;if(_tmp2E4->tag != 9)goto _LL17D;}_LL17C: return 9;_LL17D: {struct Cyc_Absyn_Shared_att_struct*
_tmp2E5=(struct Cyc_Absyn_Shared_att_struct*)_tmp2DA;if(_tmp2E5->tag != 10)goto
_LL17F;}_LL17E: return 10;_LL17F: {struct Cyc_Absyn_Unused_att_struct*_tmp2E6=(
struct Cyc_Absyn_Unused_att_struct*)_tmp2DA;if(_tmp2E6->tag != 11)goto _LL181;}
_LL180: return 11;_LL181: {struct Cyc_Absyn_Weak_att_struct*_tmp2E7=(struct Cyc_Absyn_Weak_att_struct*)
_tmp2DA;if(_tmp2E7->tag != 12)goto _LL183;}_LL182: return 12;_LL183: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp2E8=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp2DA;if(_tmp2E8->tag != 13)goto
_LL185;}_LL184: return 13;_LL185: {struct Cyc_Absyn_Dllexport_att_struct*_tmp2E9=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp2DA;if(_tmp2E9->tag != 14)goto _LL187;}
_LL186: return 14;_LL187: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp2EA=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp2DA;if(_tmp2EA->tag
!= 15)goto _LL189;}_LL188: return 15;_LL189: {struct Cyc_Absyn_Constructor_att_struct*
_tmp2EB=(struct Cyc_Absyn_Constructor_att_struct*)_tmp2DA;if(_tmp2EB->tag != 16)
goto _LL18B;}_LL18A: return 16;_LL18B: {struct Cyc_Absyn_Destructor_att_struct*
_tmp2EC=(struct Cyc_Absyn_Destructor_att_struct*)_tmp2DA;if(_tmp2EC->tag != 17)
goto _LL18D;}_LL18C: return 17;_LL18D: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp2ED=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp2DA;if(_tmp2ED->tag
!= 18)goto _LL18F;}_LL18E: return 18;_LL18F: {struct Cyc_Absyn_Format_att_struct*
_tmp2EE=(struct Cyc_Absyn_Format_att_struct*)_tmp2DA;if(_tmp2EE->tag != 19)goto
_LL191;}_LL190: return 19;_LL191: {struct Cyc_Absyn_Initializes_att_struct*_tmp2EF=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp2DA;if(_tmp2EF->tag != 20)goto _LL193;}
_LL192: return 20;_LL193:;_LL194: return 21;_LL168:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp10FF;struct _tuple0 _tmp2F1=(_tmp10FF.f1=
att1,((_tmp10FF.f2=att2,_tmp10FF)));void*_tmp2F2;int _tmp2F4;void*_tmp2F5;int
_tmp2F7;void*_tmp2F8;int _tmp2FA;void*_tmp2FB;int _tmp2FD;void*_tmp2FE;int _tmp300;
void*_tmp301;int _tmp303;void*_tmp304;struct _dyneither_ptr _tmp306;void*_tmp307;
struct _dyneither_ptr _tmp309;void*_tmp30A;enum Cyc_Absyn_Format_Type _tmp30C;int
_tmp30D;int _tmp30E;void*_tmp30F;enum Cyc_Absyn_Format_Type _tmp311;int _tmp312;int
_tmp313;_LL196: _tmp2F2=_tmp2F1.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp2F3=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp2F2;if(_tmp2F3->tag != 0)goto _LL198;else{
_tmp2F4=_tmp2F3->f1;}};_tmp2F5=_tmp2F1.f2;{struct Cyc_Absyn_Regparm_att_struct*
_tmp2F6=(struct Cyc_Absyn_Regparm_att_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto
_LL198;else{_tmp2F7=_tmp2F6->f1;}};_LL197: _tmp2FA=_tmp2F4;_tmp2FD=_tmp2F7;goto
_LL199;_LL198: _tmp2F8=_tmp2F1.f1;{struct Cyc_Absyn_Initializes_att_struct*_tmp2F9=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp2F8;if(_tmp2F9->tag != 20)goto _LL19A;
else{_tmp2FA=_tmp2F9->f1;}};_tmp2FB=_tmp2F1.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp2FC=(struct Cyc_Absyn_Initializes_att_struct*)_tmp2FB;if(_tmp2FC->tag != 20)
goto _LL19A;else{_tmp2FD=_tmp2FC->f1;}};_LL199: _tmp300=_tmp2FA;_tmp303=_tmp2FD;
goto _LL19B;_LL19A: _tmp2FE=_tmp2F1.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp2FF=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp2FE;if(_tmp2FF->tag != 6)goto _LL19C;else{
_tmp300=_tmp2FF->f1;}};_tmp301=_tmp2F1.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp302=(struct Cyc_Absyn_Aligned_att_struct*)_tmp301;if(_tmp302->tag != 6)goto
_LL19C;else{_tmp303=_tmp302->f1;}};_LL19B: return Cyc_Core_intcmp(_tmp300,_tmp303);
_LL19C: _tmp304=_tmp2F1.f1;{struct Cyc_Absyn_Section_att_struct*_tmp305=(struct Cyc_Absyn_Section_att_struct*)
_tmp304;if(_tmp305->tag != 8)goto _LL19E;else{_tmp306=_tmp305->f1;}};_tmp307=
_tmp2F1.f2;{struct Cyc_Absyn_Section_att_struct*_tmp308=(struct Cyc_Absyn_Section_att_struct*)
_tmp307;if(_tmp308->tag != 8)goto _LL19E;else{_tmp309=_tmp308->f1;}};_LL19D: return
Cyc_strcmp((struct _dyneither_ptr)_tmp306,(struct _dyneither_ptr)_tmp309);_LL19E:
_tmp30A=_tmp2F1.f1;{struct Cyc_Absyn_Format_att_struct*_tmp30B=(struct Cyc_Absyn_Format_att_struct*)
_tmp30A;if(_tmp30B->tag != 19)goto _LL1A0;else{_tmp30C=_tmp30B->f1;_tmp30D=_tmp30B->f2;
_tmp30E=_tmp30B->f3;}};_tmp30F=_tmp2F1.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp310=(struct Cyc_Absyn_Format_att_struct*)_tmp30F;if(_tmp310->tag != 19)goto
_LL1A0;else{_tmp311=_tmp310->f1;_tmp312=_tmp310->f2;_tmp313=_tmp310->f3;}};
_LL19F: {int _tmp314=Cyc_Core_intcmp((int)((unsigned int)_tmp30C),(int)((
unsigned int)_tmp311));if(_tmp314 != 0)return _tmp314;{int _tmp315=Cyc_Core_intcmp(
_tmp30D,_tmp312);if(_tmp315 != 0)return _tmp315;return Cyc_Core_intcmp(_tmp30E,
_tmp313);};}_LL1A0:;_LL1A1: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL195:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct
Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return
0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp316=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp317;
enum Cyc_Absyn_KindQual _tmp318;enum Cyc_Absyn_AliasQual _tmp319;struct Cyc_Absyn_Kind
_tmp31A;enum Cyc_Absyn_KindQual _tmp31B;enum Cyc_Absyn_AliasQual _tmp31C;struct Cyc_Absyn_Kind
_tmp31D;enum Cyc_Absyn_KindQual _tmp31E;struct Cyc_Absyn_Kind _tmp31F;enum Cyc_Absyn_KindQual
_tmp320;_LL1A3: _tmp317=*_tmp316;_tmp318=_tmp317.kind;if(_tmp318 != Cyc_Absyn_RgnKind)
goto _LL1A5;_tmp319=_tmp317.aliasqual;if(_tmp319 != Cyc_Absyn_Unique)goto _LL1A5;
_LL1A4: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1A2;_LL1A5: _tmp31A=*_tmp316;
_tmp31B=_tmp31A.kind;if(_tmp31B != Cyc_Absyn_RgnKind)goto _LL1A7;_tmp31C=_tmp31A.aliasqual;
if(_tmp31C != Cyc_Absyn_Aliasable)goto _LL1A7;_LL1A6: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1A2;_LL1A7: _tmp31D=*_tmp316;_tmp31E=_tmp31D.kind;if(_tmp31E != Cyc_Absyn_EffKind)
goto _LL1A9;_LL1A8: t=Cyc_Absyn_empty_effect;goto _LL1A2;_LL1A9: _tmp31F=*_tmp316;
_tmp320=_tmp31F.kind;if(_tmp320 != Cyc_Absyn_IntKind)goto _LL1AB;_LL1AA:{struct Cyc_Absyn_ValueofType_struct
_tmp1102;struct Cyc_Absyn_ValueofType_struct*_tmp1101;t=(void*)((_tmp1101=
_cycalloc(sizeof(*_tmp1101)),((_tmp1101[0]=((_tmp1102.tag=19,((_tmp1102.f1=Cyc_Absyn_uint_exp(
0,0),_tmp1102)))),_tmp1101))));}goto _LL1A2;_LL1AB:;_LL1AC: t=Cyc_Absyn_sint_typ;
goto _LL1A2;_LL1A2:;}{struct _tuple15*_tmp1103;return(_tmp1103=_cycalloc(sizeof(*
_tmp1103)),((_tmp1103->f1=tv,((_tmp1103->f2=t,_tmp1103)))));};}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp324=Cyc_Tcutil_compress(t);void*_tmp32F;void*_tmp331;void*
_tmp332;struct Cyc_Absyn_DatatypeInfo _tmp334;struct Cyc_List_List*_tmp335;struct
Cyc_Absyn_PtrInfo _tmp337;void*_tmp338;struct Cyc_Absyn_PtrAtts _tmp339;void*
_tmp33A;struct Cyc_Absyn_ArrayInfo _tmp33C;void*_tmp33D;struct Cyc_List_List*
_tmp33F;struct Cyc_Absyn_DatatypeFieldInfo _tmp341;struct Cyc_List_List*_tmp342;
struct Cyc_Absyn_AggrInfo _tmp344;struct Cyc_List_List*_tmp345;struct Cyc_List_List*
_tmp347;struct Cyc_Absyn_FnInfo _tmp34A;struct Cyc_List_List*_tmp34B;struct Cyc_Core_Opt*
_tmp34C;void*_tmp34D;struct Cyc_List_List*_tmp34E;struct Cyc_Absyn_VarargInfo*
_tmp34F;struct Cyc_List_List*_tmp350;void*_tmp356;struct Cyc_List_List*_tmp358;
_LL1AE: {struct Cyc_Absyn_VoidType_struct*_tmp325=(struct Cyc_Absyn_VoidType_struct*)
_tmp324;if(_tmp325->tag != 0)goto _LL1B0;}_LL1AF: goto _LL1B1;_LL1B0: {struct Cyc_Absyn_FloatType_struct*
_tmp326=(struct Cyc_Absyn_FloatType_struct*)_tmp324;if(_tmp326->tag != 7)goto
_LL1B2;}_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_DoubleType_struct*_tmp327=(
struct Cyc_Absyn_DoubleType_struct*)_tmp324;if(_tmp327->tag != 8)goto _LL1B4;}
_LL1B3: goto _LL1B5;_LL1B4: {struct Cyc_Absyn_EnumType_struct*_tmp328=(struct Cyc_Absyn_EnumType_struct*)
_tmp324;if(_tmp328->tag != 14)goto _LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp329=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp324;if(_tmp329->tag != 15)goto
_LL1B8;}_LL1B7: goto _LL1B9;_LL1B8: {struct Cyc_Absyn_ValueofType_struct*_tmp32A=(
struct Cyc_Absyn_ValueofType_struct*)_tmp324;if(_tmp32A->tag != 19)goto _LL1BA;}
_LL1B9: goto _LL1BB;_LL1BA: {struct Cyc_Absyn_IntType_struct*_tmp32B=(struct Cyc_Absyn_IntType_struct*)
_tmp324;if(_tmp32B->tag != 6)goto _LL1BC;}_LL1BB: return Cyc_Absyn_empty_effect;
_LL1BC: {struct Cyc_Absyn_Evar_struct*_tmp32C=(struct Cyc_Absyn_Evar_struct*)
_tmp324;if(_tmp32C->tag != 1)goto _LL1BE;}_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_VarType_struct*
_tmp32D=(struct Cyc_Absyn_VarType_struct*)_tmp324;if(_tmp32D->tag != 2)goto _LL1C0;}
_LL1BF: {struct Cyc_Absyn_Kind*_tmp359=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind
_tmp35A;enum Cyc_Absyn_KindQual _tmp35B;struct Cyc_Absyn_Kind _tmp35C;enum Cyc_Absyn_KindQual
_tmp35D;struct Cyc_Absyn_Kind _tmp35E;enum Cyc_Absyn_KindQual _tmp35F;_LL1E3:
_tmp35A=*_tmp359;_tmp35B=_tmp35A.kind;if(_tmp35B != Cyc_Absyn_RgnKind)goto _LL1E5;
_LL1E4: {struct Cyc_Absyn_AccessEff_struct _tmp1106;struct Cyc_Absyn_AccessEff_struct*
_tmp1105;return(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((
_tmp1106.tag=23,((_tmp1106.f1=(void*)t,_tmp1106)))),_tmp1105))));}_LL1E5: _tmp35C=*
_tmp359;_tmp35D=_tmp35C.kind;if(_tmp35D != Cyc_Absyn_EffKind)goto _LL1E7;_LL1E6:
return t;_LL1E7: _tmp35E=*_tmp359;_tmp35F=_tmp35E.kind;if(_tmp35F != Cyc_Absyn_IntKind)
goto _LL1E9;_LL1E8: return Cyc_Absyn_empty_effect;_LL1E9:;_LL1EA: {struct Cyc_Absyn_RgnsEff_struct
_tmp1109;struct Cyc_Absyn_RgnsEff_struct*_tmp1108;return(void*)((_tmp1108=
_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=25,((_tmp1109.f1=(void*)
t,_tmp1109)))),_tmp1108))));}_LL1E2:;}_LL1C0: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp32E=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp324;if(_tmp32E->tag != 16)goto
_LL1C2;else{_tmp32F=(void*)_tmp32E->f1;}}_LL1C1: {struct Cyc_Absyn_AccessEff_struct
_tmp110C;struct Cyc_Absyn_AccessEff_struct*_tmp110B;return(void*)((_tmp110B=
_cycalloc(sizeof(*_tmp110B)),((_tmp110B[0]=((_tmp110C.tag=23,((_tmp110C.f1=(void*)
_tmp32F,_tmp110C)))),_tmp110B))));}_LL1C2: {struct Cyc_Absyn_DynRgnType_struct*
_tmp330=(struct Cyc_Absyn_DynRgnType_struct*)_tmp324;if(_tmp330->tag != 17)goto
_LL1C4;else{_tmp331=(void*)_tmp330->f1;_tmp332=(void*)_tmp330->f2;}}_LL1C3: {
struct Cyc_Absyn_AccessEff_struct _tmp110F;struct Cyc_Absyn_AccessEff_struct*
_tmp110E;return(void*)((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E[0]=((
_tmp110F.tag=23,((_tmp110F.f1=(void*)_tmp332,_tmp110F)))),_tmp110E))));}_LL1C4: {
struct Cyc_Absyn_DatatypeType_struct*_tmp333=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp324;if(_tmp333->tag != 3)goto _LL1C6;else{_tmp334=_tmp333->f1;_tmp335=_tmp334.targs;}}
_LL1C5: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp335);struct Cyc_Absyn_JoinEff_struct
_tmp1112;struct Cyc_Absyn_JoinEff_struct*_tmp1111;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111[0]=((_tmp1112.tag=24,((
_tmp1112.f1=ts,_tmp1112)))),_tmp1111)))));}_LL1C6: {struct Cyc_Absyn_PointerType_struct*
_tmp336=(struct Cyc_Absyn_PointerType_struct*)_tmp324;if(_tmp336->tag != 5)goto
_LL1C8;else{_tmp337=_tmp336->f1;_tmp338=_tmp337.elt_typ;_tmp339=_tmp337.ptr_atts;
_tmp33A=_tmp339.rgn;}}_LL1C7: {struct Cyc_Absyn_JoinEff_struct _tmp1121;struct Cyc_Absyn_AccessEff_struct*
_tmp1120;struct Cyc_Absyn_AccessEff_struct _tmp111F;void*_tmp111E[2];struct Cyc_Absyn_JoinEff_struct*
_tmp111D;return Cyc_Tcutil_normalize_effect((void*)((_tmp111D=_cycalloc(sizeof(*
_tmp111D)),((_tmp111D[0]=((_tmp1121.tag=24,((_tmp1121.f1=((_tmp111E[1]=Cyc_Tcutil_rgns_of(
_tmp338),((_tmp111E[0]=(void*)((_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120[
0]=((_tmp111F.tag=23,((_tmp111F.f1=(void*)_tmp33A,_tmp111F)))),_tmp1120)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp111E,sizeof(void*),2)))))),_tmp1121)))),_tmp111D)))));}_LL1C8: {struct Cyc_Absyn_ArrayType_struct*
_tmp33B=(struct Cyc_Absyn_ArrayType_struct*)_tmp324;if(_tmp33B->tag != 9)goto
_LL1CA;else{_tmp33C=_tmp33B->f1;_tmp33D=_tmp33C.elt_type;}}_LL1C9: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp33D));_LL1CA: {struct Cyc_Absyn_TupleType_struct*_tmp33E=(
struct Cyc_Absyn_TupleType_struct*)_tmp324;if(_tmp33E->tag != 11)goto _LL1CC;else{
_tmp33F=_tmp33E->f1;}}_LL1CB: {struct Cyc_List_List*_tmp36F=0;for(0;_tmp33F != 0;
_tmp33F=_tmp33F->tl){struct Cyc_List_List*_tmp1122;_tmp36F=((_tmp1122=_cycalloc(
sizeof(*_tmp1122)),((_tmp1122->hd=(*((struct _tuple11*)_tmp33F->hd)).f2,((
_tmp1122->tl=_tmp36F,_tmp1122))))));}_tmp342=_tmp36F;goto _LL1CD;}_LL1CC: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp340=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp324;if(_tmp340->tag != 4)goto _LL1CE;else{_tmp341=_tmp340->f1;_tmp342=_tmp341.targs;}}
_LL1CD: _tmp345=_tmp342;goto _LL1CF;_LL1CE: {struct Cyc_Absyn_AggrType_struct*
_tmp343=(struct Cyc_Absyn_AggrType_struct*)_tmp324;if(_tmp343->tag != 12)goto
_LL1D0;else{_tmp344=_tmp343->f1;_tmp345=_tmp344.targs;}}_LL1CF: {struct Cyc_Absyn_JoinEff_struct
_tmp1125;struct Cyc_Absyn_JoinEff_struct*_tmp1124;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1124=_cycalloc(sizeof(*_tmp1124)),((_tmp1124[0]=((_tmp1125.tag=24,((
_tmp1125.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp345),_tmp1125)))),_tmp1124)))));}_LL1D0: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp346=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp324;if(_tmp346->tag != 13)goto _LL1D2;else{_tmp347=_tmp346->f2;}}_LL1D1: {
struct Cyc_Absyn_JoinEff_struct _tmp1128;struct Cyc_Absyn_JoinEff_struct*_tmp1127;
return Cyc_Tcutil_normalize_effect((void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((
_tmp1127[0]=((_tmp1128.tag=24,((_tmp1128.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp347),_tmp1128)))),_tmp1127)))));}_LL1D2: {struct Cyc_Absyn_TagType_struct*
_tmp348=(struct Cyc_Absyn_TagType_struct*)_tmp324;if(_tmp348->tag != 20)goto _LL1D4;}
_LL1D3: return Cyc_Absyn_empty_effect;_LL1D4: {struct Cyc_Absyn_FnType_struct*
_tmp349=(struct Cyc_Absyn_FnType_struct*)_tmp324;if(_tmp349->tag != 10)goto _LL1D6;
else{_tmp34A=_tmp349->f1;_tmp34B=_tmp34A.tvars;_tmp34C=_tmp34A.effect;_tmp34D=
_tmp34A.ret_typ;_tmp34E=_tmp34A.args;_tmp34F=_tmp34A.cyc_varargs;_tmp350=_tmp34A.rgn_po;}}
_LL1D5: {void*_tmp375=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct
_tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp34B),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp34C))->v);return Cyc_Tcutil_normalize_effect(
_tmp375);}_LL1D6: {struct Cyc_Absyn_UniqueRgn_struct*_tmp351=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp324;if(_tmp351->tag != 22)goto _LL1D8;}_LL1D7: goto _LL1D9;_LL1D8: {struct Cyc_Absyn_HeapRgn_struct*
_tmp352=(struct Cyc_Absyn_HeapRgn_struct*)_tmp324;if(_tmp352->tag != 21)goto _LL1DA;}
_LL1D9: return Cyc_Absyn_empty_effect;_LL1DA: {struct Cyc_Absyn_AccessEff_struct*
_tmp353=(struct Cyc_Absyn_AccessEff_struct*)_tmp324;if(_tmp353->tag != 23)goto
_LL1DC;}_LL1DB: goto _LL1DD;_LL1DC: {struct Cyc_Absyn_JoinEff_struct*_tmp354=(
struct Cyc_Absyn_JoinEff_struct*)_tmp324;if(_tmp354->tag != 24)goto _LL1DE;}_LL1DD:
return t;_LL1DE: {struct Cyc_Absyn_RgnsEff_struct*_tmp355=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp324;if(_tmp355->tag != 25)goto _LL1E0;else{_tmp356=(void*)_tmp355->f1;}}_LL1DF:
return Cyc_Tcutil_rgns_of(_tmp356);_LL1E0: {struct Cyc_Absyn_TypedefType_struct*
_tmp357=(struct Cyc_Absyn_TypedefType_struct*)_tmp324;if(_tmp357->tag != 18)goto
_LL1AD;else{_tmp358=_tmp357->f2;}}_LL1E1: {struct Cyc_Absyn_JoinEff_struct
_tmp112B;struct Cyc_Absyn_JoinEff_struct*_tmp112A;return Cyc_Tcutil_normalize_effect((
void*)((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=24,((
_tmp112B.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp358),_tmp112B)))),_tmp112A)))));}_LL1AD:;}
void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*_tmp378=
e;struct Cyc_List_List*_tmp37A;struct Cyc_List_List**_tmp37B;void*_tmp37D;_LL1EC: {
struct Cyc_Absyn_JoinEff_struct*_tmp379=(struct Cyc_Absyn_JoinEff_struct*)_tmp378;
if(_tmp379->tag != 24)goto _LL1EE;else{_tmp37A=_tmp379->f1;_tmp37B=(struct Cyc_List_List**)&
_tmp379->f1;}}_LL1ED: {int redo_join=0;{struct Cyc_List_List*effs=*_tmp37B;for(0;
effs != 0;effs=effs->tl){void*_tmp37E=(void*)effs->hd;effs->hd=(void*)Cyc_Tcutil_compress(
Cyc_Tcutil_normalize_effect(_tmp37E));{void*_tmp37F=(void*)effs->hd;void*_tmp382;
_LL1F3: {struct Cyc_Absyn_JoinEff_struct*_tmp380=(struct Cyc_Absyn_JoinEff_struct*)
_tmp37F;if(_tmp380->tag != 24)goto _LL1F5;}_LL1F4: goto _LL1F6;_LL1F5: {struct Cyc_Absyn_AccessEff_struct*
_tmp381=(struct Cyc_Absyn_AccessEff_struct*)_tmp37F;if(_tmp381->tag != 23)goto
_LL1F7;else{_tmp382=(void*)_tmp381->f1;{struct Cyc_Absyn_HeapRgn_struct*_tmp383=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp382;if(_tmp383->tag != 21)goto _LL1F7;};}}
_LL1F6: redo_join=1;goto _LL1F2;_LL1F7:;_LL1F8: goto _LL1F2;_LL1F2:;};}}if(!
redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp37B;for(0;effs != 0;effs=effs->tl){void*_tmp384=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp386;void*_tmp388;_LL1FA: {struct Cyc_Absyn_JoinEff_struct*
_tmp385=(struct Cyc_Absyn_JoinEff_struct*)_tmp384;if(_tmp385->tag != 24)goto _LL1FC;
else{_tmp386=_tmp385->f1;}}_LL1FB: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp386,effects);goto _LL1F9;_LL1FC: {
struct Cyc_Absyn_AccessEff_struct*_tmp387=(struct Cyc_Absyn_AccessEff_struct*)
_tmp384;if(_tmp387->tag != 23)goto _LL1FE;else{_tmp388=(void*)_tmp387->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp389=(struct Cyc_Absyn_HeapRgn_struct*)_tmp388;if(
_tmp389->tag != 21)goto _LL1FE;};}}_LL1FD: goto _LL1F9;_LL1FE:;_LL1FF:{struct Cyc_List_List*
_tmp112C;effects=((_tmp112C=_cycalloc(sizeof(*_tmp112C)),((_tmp112C->hd=_tmp384,((
_tmp112C->tl=effects,_tmp112C))))));}goto _LL1F9;_LL1F9:;}}*_tmp37B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1EE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp37C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp378;if(_tmp37C->tag != 25)goto _LL1F0;
else{_tmp37D=(void*)_tmp37C->f1;}}_LL1EF: {void*_tmp38B=Cyc_Tcutil_compress(
_tmp37D);_LL201: {struct Cyc_Absyn_Evar_struct*_tmp38C=(struct Cyc_Absyn_Evar_struct*)
_tmp38B;if(_tmp38C->tag != 1)goto _LL203;}_LL202: goto _LL204;_LL203: {struct Cyc_Absyn_VarType_struct*
_tmp38D=(struct Cyc_Absyn_VarType_struct*)_tmp38B;if(_tmp38D->tag != 2)goto _LL205;}
_LL204: return e;_LL205:;_LL206: return Cyc_Tcutil_rgns_of(_tmp37D);_LL200:;}_LL1F0:;
_LL1F1: return e;_LL1EB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp1136;struct Cyc_Core_Opt*_tmp1135;struct Cyc_Absyn_FnInfo _tmp1134;struct Cyc_Absyn_FnType_struct*
_tmp1133;struct Cyc_Absyn_FnType_struct*_tmp38E=(_tmp1133=_cycalloc(sizeof(*
_tmp1133)),((_tmp1133[0]=((_tmp1136.tag=10,((_tmp1136.f1=((_tmp1134.tvars=0,((
_tmp1134.effect=((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135->v=eff,
_tmp1135)))),((_tmp1134.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1134.args=0,((
_tmp1134.c_varargs=0,((_tmp1134.cyc_varargs=0,((_tmp1134.rgn_po=0,((_tmp1134.attributes=
0,_tmp1134)))))))))))))))),_tmp1136)))),_tmp1133)));return Cyc_Absyn_atb_typ((
void*)_tmp38E,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)&
Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp393=Cyc_Tcutil_compress(e);void*
_tmp395;struct Cyc_List_List*_tmp397;void*_tmp399;struct Cyc_Core_Opt*_tmp39B;
struct Cyc_Core_Opt*_tmp39C;struct Cyc_Core_Opt**_tmp39D;struct Cyc_Core_Opt*
_tmp39E;_LL208: {struct Cyc_Absyn_AccessEff_struct*_tmp394=(struct Cyc_Absyn_AccessEff_struct*)
_tmp393;if(_tmp394->tag != 23)goto _LL20A;else{_tmp395=(void*)_tmp394->f1;}}_LL209:
if(constrain)return Cyc_Tcutil_unify(r,_tmp395);_tmp395=Cyc_Tcutil_compress(
_tmp395);if(r == _tmp395)return 1;{struct _tuple0 _tmp1137;struct _tuple0 _tmp3A0=(
_tmp1137.f1=r,((_tmp1137.f2=_tmp395,_tmp1137)));void*_tmp3A1;struct Cyc_Absyn_Tvar*
_tmp3A3;void*_tmp3A4;struct Cyc_Absyn_Tvar*_tmp3A6;_LL213: _tmp3A1=_tmp3A0.f1;{
struct Cyc_Absyn_VarType_struct*_tmp3A2=(struct Cyc_Absyn_VarType_struct*)_tmp3A1;
if(_tmp3A2->tag != 2)goto _LL215;else{_tmp3A3=_tmp3A2->f1;}};_tmp3A4=_tmp3A0.f2;{
struct Cyc_Absyn_VarType_struct*_tmp3A5=(struct Cyc_Absyn_VarType_struct*)_tmp3A4;
if(_tmp3A5->tag != 2)goto _LL215;else{_tmp3A6=_tmp3A5->f1;}};_LL214: return Cyc_Absyn_tvar_cmp(
_tmp3A3,_tmp3A6)== 0;_LL215:;_LL216: return 0;_LL212:;};_LL20A: {struct Cyc_Absyn_JoinEff_struct*
_tmp396=(struct Cyc_Absyn_JoinEff_struct*)_tmp393;if(_tmp396->tag != 24)goto _LL20C;
else{_tmp397=_tmp396->f1;}}_LL20B: for(0;_tmp397 != 0;_tmp397=_tmp397->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp397->hd))return 1;}return 0;_LL20C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp398=(struct Cyc_Absyn_RgnsEff_struct*)_tmp393;if(_tmp398->tag != 25)goto _LL20E;
else{_tmp399=(void*)_tmp398->f1;}}_LL20D: {void*_tmp3A7=Cyc_Tcutil_rgns_of(
_tmp399);void*_tmp3A9;_LL218: {struct Cyc_Absyn_RgnsEff_struct*_tmp3A8=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3A7;if(_tmp3A8->tag != 25)goto _LL21A;else{_tmp3A9=(void*)_tmp3A8->f1;}}_LL219:
if(!constrain)return 0;{void*_tmp3AA=Cyc_Tcutil_compress(_tmp3A9);struct Cyc_Core_Opt*
_tmp3AC;struct Cyc_Core_Opt*_tmp3AD;struct Cyc_Core_Opt**_tmp3AE;struct Cyc_Core_Opt*
_tmp3AF;_LL21D: {struct Cyc_Absyn_Evar_struct*_tmp3AB=(struct Cyc_Absyn_Evar_struct*)
_tmp3AA;if(_tmp3AB->tag != 1)goto _LL21F;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;
_tmp3AE=(struct Cyc_Core_Opt**)& _tmp3AB->f2;_tmp3AF=_tmp3AB->f4;}}_LL21E: {void*
_tmp3B0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3AF);Cyc_Tcutil_occurs(
_tmp3B0,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3AF))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1146;struct Cyc_Absyn_AccessEff_struct*
_tmp1145;struct Cyc_Absyn_AccessEff_struct _tmp1144;void*_tmp1143[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1142;void*_tmp3B1=Cyc_Tcutil_dummy_fntype((void*)((_tmp1142=_cycalloc(
sizeof(*_tmp1142)),((_tmp1142[0]=((_tmp1146.tag=24,((_tmp1146.f1=((_tmp1143[1]=(
void*)((_tmp1145=_cycalloc(sizeof(*_tmp1145)),((_tmp1145[0]=((_tmp1144.tag=23,((
_tmp1144.f1=(void*)r,_tmp1144)))),_tmp1145)))),((_tmp1143[0]=_tmp3B0,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1143,sizeof(void*),2)))))),
_tmp1146)))),_tmp1142)))));{struct Cyc_Core_Opt*_tmp1147;*_tmp3AE=((_tmp1147=
_cycalloc(sizeof(*_tmp1147)),((_tmp1147->v=_tmp3B1,_tmp1147))));}return 1;};}
_LL21F:;_LL220: return 0;_LL21C:;};_LL21A:;_LL21B: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3A7);_LL217:;}_LL20E: {struct Cyc_Absyn_Evar_struct*_tmp39A=(
struct Cyc_Absyn_Evar_struct*)_tmp393;if(_tmp39A->tag != 1)goto _LL210;else{_tmp39B=
_tmp39A->f1;_tmp39C=_tmp39A->f2;_tmp39D=(struct Cyc_Core_Opt**)& _tmp39A->f2;
_tmp39E=_tmp39A->f4;}}_LL20F: if(_tmp39B == 0  || ((struct Cyc_Absyn_Kind*)_tmp39B->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp114A;void*_tmp1149;(_tmp1149=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp114A="effect evar has wrong kind",
_tag_dyneither(_tmp114A,sizeof(char),27))),_tag_dyneither(_tmp1149,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3BA=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp39E);Cyc_Tcutil_occurs(_tmp3BA,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39E))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp115F;struct Cyc_List_List*_tmp115E;struct Cyc_Absyn_AccessEff_struct _tmp115D;
struct Cyc_Absyn_AccessEff_struct*_tmp115C;struct Cyc_List_List*_tmp115B;struct Cyc_Absyn_JoinEff_struct*
_tmp115A;struct Cyc_Absyn_JoinEff_struct*_tmp3BB=(_tmp115A=_cycalloc(sizeof(*
_tmp115A)),((_tmp115A[0]=((_tmp115F.tag=24,((_tmp115F.f1=((_tmp115B=_cycalloc(
sizeof(*_tmp115B)),((_tmp115B->hd=_tmp3BA,((_tmp115B->tl=((_tmp115E=_cycalloc(
sizeof(*_tmp115E)),((_tmp115E->hd=(void*)((_tmp115C=_cycalloc(sizeof(*_tmp115C)),((
_tmp115C[0]=((_tmp115D.tag=23,((_tmp115D.f1=(void*)r,_tmp115D)))),_tmp115C)))),((
_tmp115E->tl=0,_tmp115E)))))),_tmp115B)))))),_tmp115F)))),_tmp115A)));{struct Cyc_Core_Opt*
_tmp1160;*_tmp39D=((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160->v=(void*)
_tmp3BB,_tmp1160))));}return 1;};};_LL210:;_LL211: return 0;_LL207:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp3C3=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp3C6;void*_tmp3C8;struct Cyc_Core_Opt*
_tmp3CA;struct Cyc_Core_Opt*_tmp3CB;struct Cyc_Core_Opt**_tmp3CC;struct Cyc_Core_Opt*
_tmp3CD;_LL222: {struct Cyc_Absyn_AccessEff_struct*_tmp3C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C3;if(_tmp3C4->tag != 23)goto _LL224;}_LL223: return 0;_LL224: {struct Cyc_Absyn_JoinEff_struct*
_tmp3C5=(struct Cyc_Absyn_JoinEff_struct*)_tmp3C3;if(_tmp3C5->tag != 24)goto _LL226;
else{_tmp3C6=_tmp3C5->f1;}}_LL225: for(0;_tmp3C6 != 0;_tmp3C6=_tmp3C6->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp3C6->hd))return 1;}return 0;_LL226: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3C7=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3C3;if(_tmp3C7->tag != 25)goto _LL228;
else{_tmp3C8=(void*)_tmp3C7->f1;}}_LL227: _tmp3C8=Cyc_Tcutil_compress(_tmp3C8);
if(t == _tmp3C8)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3C8);{
void*_tmp3CE=Cyc_Tcutil_rgns_of(t);void*_tmp3D0;_LL22D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3CF=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3CE;if(_tmp3CF->tag != 25)goto _LL22F;
else{_tmp3D0=(void*)_tmp3CF->f1;}}_LL22E: {struct _tuple0 _tmp1161;struct _tuple0
_tmp3D2=(_tmp1161.f1=Cyc_Tcutil_compress(_tmp3D0),((_tmp1161.f2=_tmp3C8,_tmp1161)));
void*_tmp3D3;struct Cyc_Absyn_Tvar*_tmp3D5;void*_tmp3D6;struct Cyc_Absyn_Tvar*
_tmp3D8;_LL232: _tmp3D3=_tmp3D2.f1;{struct Cyc_Absyn_VarType_struct*_tmp3D4=(
struct Cyc_Absyn_VarType_struct*)_tmp3D3;if(_tmp3D4->tag != 2)goto _LL234;else{
_tmp3D5=_tmp3D4->f1;}};_tmp3D6=_tmp3D2.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3D7=(struct Cyc_Absyn_VarType_struct*)_tmp3D6;if(_tmp3D7->tag != 2)goto _LL234;
else{_tmp3D8=_tmp3D7->f1;}};_LL233: return Cyc_Tcutil_unify(t,_tmp3C8);_LL234:;
_LL235: return _tmp3D0 == _tmp3C8;_LL231:;}_LL22F:;_LL230: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp3CE);_LL22C:;};_LL228: {struct Cyc_Absyn_Evar_struct*
_tmp3C9=(struct Cyc_Absyn_Evar_struct*)_tmp3C3;if(_tmp3C9->tag != 1)goto _LL22A;
else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;_tmp3CC=(struct Cyc_Core_Opt**)&
_tmp3C9->f2;_tmp3CD=_tmp3C9->f4;}}_LL229: if(_tmp3CA == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3CA->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1164;void*_tmp1163;(
_tmp1163=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1164="effect evar has wrong kind",_tag_dyneither(_tmp1164,sizeof(char),27))),
_tag_dyneither(_tmp1163,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp3DB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3CD);
Cyc_Tcutil_occurs(_tmp3DB,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3CD))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1179;struct Cyc_List_List*
_tmp1178;struct Cyc_Absyn_RgnsEff_struct _tmp1177;struct Cyc_Absyn_RgnsEff_struct*
_tmp1176;struct Cyc_List_List*_tmp1175;struct Cyc_Absyn_JoinEff_struct*_tmp1174;
struct Cyc_Absyn_JoinEff_struct*_tmp3DC=(_tmp1174=_cycalloc(sizeof(*_tmp1174)),((
_tmp1174[0]=((_tmp1179.tag=24,((_tmp1179.f1=((_tmp1175=_cycalloc(sizeof(*
_tmp1175)),((_tmp1175->hd=_tmp3DB,((_tmp1175->tl=((_tmp1178=_cycalloc(sizeof(*
_tmp1178)),((_tmp1178->hd=(void*)((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((
_tmp1176[0]=((_tmp1177.tag=25,((_tmp1177.f1=(void*)t,_tmp1177)))),_tmp1176)))),((
_tmp1178->tl=0,_tmp1178)))))),_tmp1175)))))),_tmp1179)))),_tmp1174)));{struct Cyc_Core_Opt*
_tmp117A;*_tmp3CC=((_tmp117A=_cycalloc(sizeof(*_tmp117A)),((_tmp117A->v=(void*)
_tmp3DC,_tmp117A))));}return 1;};};_LL22A:;_LL22B: return 0;_LL221:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp3E4=e;struct Cyc_Absyn_Tvar*_tmp3E6;struct Cyc_List_List*_tmp3E8;void*
_tmp3EA;struct Cyc_Core_Opt*_tmp3EC;struct Cyc_Core_Opt*_tmp3ED;struct Cyc_Core_Opt**
_tmp3EE;struct Cyc_Core_Opt*_tmp3EF;_LL237: {struct Cyc_Absyn_VarType_struct*
_tmp3E5=(struct Cyc_Absyn_VarType_struct*)_tmp3E4;if(_tmp3E5->tag != 2)goto _LL239;
else{_tmp3E6=_tmp3E5->f1;}}_LL238: return Cyc_Absyn_tvar_cmp(v,_tmp3E6)== 0;_LL239: {
struct Cyc_Absyn_JoinEff_struct*_tmp3E7=(struct Cyc_Absyn_JoinEff_struct*)_tmp3E4;
if(_tmp3E7->tag != 24)goto _LL23B;else{_tmp3E8=_tmp3E7->f1;}}_LL23A: for(0;_tmp3E8
!= 0;_tmp3E8=_tmp3E8->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp3E8->hd))return 1;}return 0;_LL23B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3E9=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3E4;if(_tmp3E9->tag != 25)goto _LL23D;
else{_tmp3EA=(void*)_tmp3E9->f1;}}_LL23C: {void*_tmp3F0=Cyc_Tcutil_rgns_of(
_tmp3EA);void*_tmp3F2;_LL242: {struct Cyc_Absyn_RgnsEff_struct*_tmp3F1=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3F0;if(_tmp3F1->tag != 25)goto _LL244;else{_tmp3F2=(void*)_tmp3F1->f1;}}_LL243:
if(!may_constrain_evars)return 0;{void*_tmp3F3=Cyc_Tcutil_compress(_tmp3F2);
struct Cyc_Core_Opt*_tmp3F5;struct Cyc_Core_Opt*_tmp3F6;struct Cyc_Core_Opt**
_tmp3F7;struct Cyc_Core_Opt*_tmp3F8;_LL247: {struct Cyc_Absyn_Evar_struct*_tmp3F4=(
struct Cyc_Absyn_Evar_struct*)_tmp3F3;if(_tmp3F4->tag != 1)goto _LL249;else{_tmp3F5=
_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;_tmp3F7=(struct Cyc_Core_Opt**)& _tmp3F4->f2;
_tmp3F8=_tmp3F4->f4;}}_LL248: {void*_tmp3F9=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3F8);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3F8))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1189;struct Cyc_Absyn_VarType_struct*_tmp1188;
struct Cyc_Absyn_VarType_struct _tmp1187;void*_tmp1186[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1185;void*_tmp3FA=Cyc_Tcutil_dummy_fntype((void*)((_tmp1185=_cycalloc(
sizeof(*_tmp1185)),((_tmp1185[0]=((_tmp1189.tag=24,((_tmp1189.f1=((_tmp1186[1]=(
void*)((_tmp1188=_cycalloc(sizeof(*_tmp1188)),((_tmp1188[0]=((_tmp1187.tag=2,((
_tmp1187.f1=v,_tmp1187)))),_tmp1188)))),((_tmp1186[0]=_tmp3F9,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1186,sizeof(void*),2)))))),
_tmp1189)))),_tmp1185)))));{struct Cyc_Core_Opt*_tmp118A;*_tmp3F7=((_tmp118A=
_cycalloc(sizeof(*_tmp118A)),((_tmp118A->v=_tmp3FA,_tmp118A))));}return 1;};}
_LL249:;_LL24A: return 0;_LL246:;};_LL244:;_LL245: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp3F0);_LL241:;}_LL23D: {struct Cyc_Absyn_Evar_struct*
_tmp3EB=(struct Cyc_Absyn_Evar_struct*)_tmp3E4;if(_tmp3EB->tag != 1)goto _LL23F;
else{_tmp3EC=_tmp3EB->f1;_tmp3ED=_tmp3EB->f2;_tmp3EE=(struct Cyc_Core_Opt**)&
_tmp3EB->f2;_tmp3EF=_tmp3EB->f4;}}_LL23E: if(_tmp3EC == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3EC->v)->kind != Cyc_Absyn_EffKind){const char*_tmp118D;void*_tmp118C;(
_tmp118C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp118D="effect evar has wrong kind",_tag_dyneither(_tmp118D,sizeof(char),27))),
_tag_dyneither(_tmp118C,sizeof(void*),0)));}{void*_tmp403=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3EF);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EF))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp11A2;
struct Cyc_List_List*_tmp11A1;struct Cyc_Absyn_VarType_struct _tmp11A0;struct Cyc_Absyn_VarType_struct*
_tmp119F;struct Cyc_List_List*_tmp119E;struct Cyc_Absyn_JoinEff_struct*_tmp119D;
struct Cyc_Absyn_JoinEff_struct*_tmp404=(_tmp119D=_cycalloc(sizeof(*_tmp119D)),((
_tmp119D[0]=((_tmp11A2.tag=24,((_tmp11A2.f1=((_tmp119E=_cycalloc(sizeof(*
_tmp119E)),((_tmp119E->hd=_tmp403,((_tmp119E->tl=((_tmp11A1=_cycalloc(sizeof(*
_tmp11A1)),((_tmp11A1->hd=(void*)((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((
_tmp119F[0]=((_tmp11A0.tag=2,((_tmp11A0.f1=v,_tmp11A0)))),_tmp119F)))),((
_tmp11A1->tl=0,_tmp11A1)))))),_tmp119E)))))),_tmp11A2)))),_tmp119D)));{struct Cyc_Core_Opt*
_tmp11A3;*_tmp3EE=((_tmp11A3=_cycalloc(sizeof(*_tmp11A3)),((_tmp11A3->v=(void*)
_tmp404,_tmp11A3))));}return 1;};};_LL23F:;_LL240: return 0;_LL236:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp40C=e;struct Cyc_List_List*
_tmp40E;void*_tmp410;_LL24C: {struct Cyc_Absyn_JoinEff_struct*_tmp40D=(struct Cyc_Absyn_JoinEff_struct*)
_tmp40C;if(_tmp40D->tag != 24)goto _LL24E;else{_tmp40E=_tmp40D->f1;}}_LL24D: for(0;
_tmp40E != 0;_tmp40E=_tmp40E->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp40E->hd))return 1;}return 0;_LL24E: {struct Cyc_Absyn_RgnsEff_struct*_tmp40F=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp40C;if(_tmp40F->tag != 25)goto _LL250;else{
_tmp410=(void*)_tmp40F->f1;}}_LL24F: {void*_tmp412=Cyc_Tcutil_rgns_of(_tmp410);
void*_tmp414;_LL255: {struct Cyc_Absyn_RgnsEff_struct*_tmp413=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp412;if(_tmp413->tag != 25)goto _LL257;else{_tmp414=(void*)_tmp413->f1;}}_LL256:
return 0;_LL257:;_LL258: return Cyc_Tcutil_evar_in_effect(evar,_tmp412);_LL254:;}
_LL250: {struct Cyc_Absyn_Evar_struct*_tmp411=(struct Cyc_Absyn_Evar_struct*)
_tmp40C;if(_tmp411->tag != 1)goto _LL252;}_LL251: return evar == e;_LL252:;_LL253:
return 0;_LL24B:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2){void*_tmp415=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp417;void*
_tmp419;struct Cyc_Absyn_Tvar*_tmp41B;void*_tmp41D;struct Cyc_Core_Opt*_tmp41F;
struct Cyc_Core_Opt**_tmp420;struct Cyc_Core_Opt*_tmp421;_LL25A: {struct Cyc_Absyn_JoinEff_struct*
_tmp416=(struct Cyc_Absyn_JoinEff_struct*)_tmp415;if(_tmp416->tag != 24)goto _LL25C;
else{_tmp417=_tmp416->f1;}}_LL25B: for(0;_tmp417 != 0;_tmp417=_tmp417->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp417->hd,e2))return 0;}return 1;_LL25C: {struct Cyc_Absyn_AccessEff_struct*
_tmp418=(struct Cyc_Absyn_AccessEff_struct*)_tmp415;if(_tmp418->tag != 23)goto
_LL25E;else{_tmp419=(void*)_tmp418->f1;}}_LL25D: return Cyc_Tcutil_region_in_effect(
0,_tmp419,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp419,(void*)& Cyc_Absyn_HeapRgn_val);
_LL25E: {struct Cyc_Absyn_VarType_struct*_tmp41A=(struct Cyc_Absyn_VarType_struct*)
_tmp415;if(_tmp41A->tag != 2)goto _LL260;else{_tmp41B=_tmp41A->f1;}}_LL25F: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp41B,e2);_LL260: {struct Cyc_Absyn_RgnsEff_struct*
_tmp41C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp415;if(_tmp41C->tag != 25)goto _LL262;
else{_tmp41D=(void*)_tmp41C->f1;}}_LL261: {void*_tmp422=Cyc_Tcutil_rgns_of(
_tmp41D);void*_tmp424;_LL267: {struct Cyc_Absyn_RgnsEff_struct*_tmp423=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp422;if(_tmp423->tag != 25)goto _LL269;else{_tmp424=(void*)_tmp423->f1;}}_LL268:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp424,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp424,Cyc_Absyn_sint_typ);_LL269:;
_LL26A: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp422,e2);_LL266:;}
_LL262: {struct Cyc_Absyn_Evar_struct*_tmp41E=(struct Cyc_Absyn_Evar_struct*)
_tmp415;if(_tmp41E->tag != 1)goto _LL264;else{_tmp41F=_tmp41E->f2;_tmp420=(struct
Cyc_Core_Opt**)& _tmp41E->f2;_tmp421=_tmp41E->f4;}}_LL263: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp11A4;*_tmp420=((_tmp11A4=_cycalloc(sizeof(*
_tmp11A4)),((_tmp11A4->v=Cyc_Absyn_empty_effect,_tmp11A4))));}return 1;_LL264:;
_LL265: {const char*_tmp11A8;void*_tmp11A7[1];struct Cyc_String_pa_struct _tmp11A6;(
_tmp11A6.tag=0,((_tmp11A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp11A7[0]=& _tmp11A6,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A8="subset_effect: bad effect: %s",
_tag_dyneither(_tmp11A8,sizeof(char),30))),_tag_dyneither(_tmp11A7,sizeof(void*),
1)))))));}_LL259:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp42A;void*_tmp42B;void*_tmp42C;struct
_tuple0*_tmp429=(struct _tuple0*)r1->hd;_tmp42A=*_tmp429;_tmp42B=_tmp42A.f1;
_tmp42C=_tmp42A.f2;{int found=_tmp42B == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp42E;void*_tmp42F;void*
_tmp430;struct _tuple0*_tmp42D=(struct _tuple0*)r2->hd;_tmp42E=*_tmp42D;_tmp42F=
_tmp42E.f1;_tmp430=_tmp42E.f2;if(Cyc_Tcutil_unify(_tmp42B,_tmp42F) && Cyc_Tcutil_unify(
_tmp42C,_tmp430)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp431=t1;struct Cyc_Core_Opt*_tmp433;struct Cyc_Core_Opt*
_tmp434;struct Cyc_Core_Opt**_tmp435;struct Cyc_Core_Opt*_tmp436;_LL26C: {struct
Cyc_Absyn_Evar_struct*_tmp432=(struct Cyc_Absyn_Evar_struct*)_tmp431;if(_tmp432->tag
!= 1)goto _LL26E;else{_tmp433=_tmp432->f1;_tmp434=_tmp432->f2;_tmp435=(struct Cyc_Core_Opt**)&
_tmp432->f2;_tmp436=_tmp432->f4;}}_LL26D: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp436))->v,t2);{struct
Cyc_Absyn_Kind*_tmp437=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp437,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp433))->v)){{struct
Cyc_Core_Opt*_tmp11A9;*_tmp435=((_tmp11A9=_cycalloc(sizeof(*_tmp11A9)),((
_tmp11A9->v=t2,_tmp11A9))));}return;}else{{void*_tmp439=t2;struct Cyc_Core_Opt*
_tmp43B;struct Cyc_Core_Opt**_tmp43C;struct Cyc_Core_Opt*_tmp43D;struct Cyc_Absyn_PtrInfo
_tmp43F;_LL271: {struct Cyc_Absyn_Evar_struct*_tmp43A=(struct Cyc_Absyn_Evar_struct*)
_tmp439;if(_tmp43A->tag != 1)goto _LL273;else{_tmp43B=_tmp43A->f2;_tmp43C=(struct
Cyc_Core_Opt**)& _tmp43A->f2;_tmp43D=_tmp43A->f4;}}_LL272: {struct Cyc_List_List*
_tmp440=(struct Cyc_List_List*)_tmp436->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp43D))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp440,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp11AA;Cyc_Tcutil_failure_reason=((
_tmp11AA="(type variable would escape scope)",_tag_dyneither(_tmp11AA,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp433->v,_tmp437)){{struct Cyc_Core_Opt*_tmp11AB;*_tmp43C=((
_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB->v=t1,_tmp11AB))));}return;}{
const char*_tmp11AC;Cyc_Tcutil_failure_reason=((_tmp11AC="(kinds are incompatible)",
_tag_dyneither(_tmp11AC,sizeof(char),25)));}goto _LL270;}_LL273:{struct Cyc_Absyn_PointerType_struct*
_tmp43E=(struct Cyc_Absyn_PointerType_struct*)_tmp439;if(_tmp43E->tag != 5)goto
_LL275;else{_tmp43F=_tmp43E->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp433->v)->kind
== Cyc_Absyn_BoxKind))goto _LL275;_LL274: {union Cyc_Absyn_Constraint*_tmp444=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp43F.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp445=_tmp444;union Cyc_Absyn_Constraint
_tmp446;int _tmp447;_LL278: _tmp446=*_tmp445;if((_tmp446.No_constr).tag != 3)goto
_LL27A;_tmp447=(int)(_tmp446.No_constr).val;_LL279:{struct
_union_Constraint_Eq_constr*_tmp11AD;(_tmp11AD=& _tmp444->Eq_constr,((_tmp11AD->tag=
1,_tmp11AD->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp11AE;*_tmp435=((
_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE->v=t2,_tmp11AE))));}return;
_LL27A:;_LL27B: goto _LL277;_LL277:;}goto _LL270;}_LL275:;_LL276: goto _LL270;_LL270:;}{
const char*_tmp11AF;Cyc_Tcutil_failure_reason=((_tmp11AF="(kinds are incompatible)",
_tag_dyneither(_tmp11AF,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL26E:;_LL26F: goto _LL26B;_LL26B:;}{struct _tuple0 _tmp11B0;struct _tuple0 _tmp44C=(
_tmp11B0.f1=t2,((_tmp11B0.f2=t1,_tmp11B0)));void*_tmp44D;void*_tmp44F;void*
_tmp451;void*_tmp453;struct Cyc_Absyn_Tvar*_tmp455;void*_tmp456;struct Cyc_Absyn_Tvar*
_tmp458;void*_tmp459;struct Cyc_Absyn_AggrInfo _tmp45B;union Cyc_Absyn_AggrInfoU
_tmp45C;struct Cyc_List_List*_tmp45D;void*_tmp45E;struct Cyc_Absyn_AggrInfo _tmp460;
union Cyc_Absyn_AggrInfoU _tmp461;struct Cyc_List_List*_tmp462;void*_tmp463;struct
_tuple2*_tmp465;void*_tmp466;struct _tuple2*_tmp468;void*_tmp469;struct Cyc_List_List*
_tmp46B;void*_tmp46C;struct Cyc_List_List*_tmp46E;void*_tmp46F;struct Cyc_Absyn_DatatypeInfo
_tmp471;union Cyc_Absyn_DatatypeInfoU _tmp472;struct Cyc_Absyn_Datatypedecl**
_tmp473;struct Cyc_Absyn_Datatypedecl*_tmp474;struct Cyc_List_List*_tmp475;void*
_tmp476;struct Cyc_Absyn_DatatypeInfo _tmp478;union Cyc_Absyn_DatatypeInfoU _tmp479;
struct Cyc_Absyn_Datatypedecl**_tmp47A;struct Cyc_Absyn_Datatypedecl*_tmp47B;
struct Cyc_List_List*_tmp47C;void*_tmp47D;struct Cyc_Absyn_DatatypeFieldInfo
_tmp47F;union Cyc_Absyn_DatatypeFieldInfoU _tmp480;struct _tuple3 _tmp481;struct Cyc_Absyn_Datatypedecl*
_tmp482;struct Cyc_Absyn_Datatypefield*_tmp483;struct Cyc_List_List*_tmp484;void*
_tmp485;struct Cyc_Absyn_DatatypeFieldInfo _tmp487;union Cyc_Absyn_DatatypeFieldInfoU
_tmp488;struct _tuple3 _tmp489;struct Cyc_Absyn_Datatypedecl*_tmp48A;struct Cyc_Absyn_Datatypefield*
_tmp48B;struct Cyc_List_List*_tmp48C;void*_tmp48D;struct Cyc_Absyn_PtrInfo _tmp48F;
void*_tmp490;struct Cyc_Absyn_Tqual _tmp491;struct Cyc_Absyn_PtrAtts _tmp492;void*
_tmp493;union Cyc_Absyn_Constraint*_tmp494;union Cyc_Absyn_Constraint*_tmp495;
union Cyc_Absyn_Constraint*_tmp496;void*_tmp497;struct Cyc_Absyn_PtrInfo _tmp499;
void*_tmp49A;struct Cyc_Absyn_Tqual _tmp49B;struct Cyc_Absyn_PtrAtts _tmp49C;void*
_tmp49D;union Cyc_Absyn_Constraint*_tmp49E;union Cyc_Absyn_Constraint*_tmp49F;
union Cyc_Absyn_Constraint*_tmp4A0;void*_tmp4A1;enum Cyc_Absyn_Sign _tmp4A3;enum 
Cyc_Absyn_Size_of _tmp4A4;void*_tmp4A5;enum Cyc_Absyn_Sign _tmp4A7;enum Cyc_Absyn_Size_of
_tmp4A8;void*_tmp4A9;void*_tmp4AB;void*_tmp4AD;int _tmp4AF;void*_tmp4B0;int
_tmp4B2;void*_tmp4B3;void*_tmp4B5;void*_tmp4B6;void*_tmp4B8;void*_tmp4B9;struct
Cyc_Absyn_Exp*_tmp4BB;void*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;void*_tmp4BF;
struct Cyc_Absyn_ArrayInfo _tmp4C1;void*_tmp4C2;struct Cyc_Absyn_Tqual _tmp4C3;
struct Cyc_Absyn_Exp*_tmp4C4;union Cyc_Absyn_Constraint*_tmp4C5;void*_tmp4C6;
struct Cyc_Absyn_ArrayInfo _tmp4C8;void*_tmp4C9;struct Cyc_Absyn_Tqual _tmp4CA;
struct Cyc_Absyn_Exp*_tmp4CB;union Cyc_Absyn_Constraint*_tmp4CC;void*_tmp4CD;
struct Cyc_Absyn_FnInfo _tmp4CF;struct Cyc_List_List*_tmp4D0;struct Cyc_Core_Opt*
_tmp4D1;void*_tmp4D2;struct Cyc_List_List*_tmp4D3;int _tmp4D4;struct Cyc_Absyn_VarargInfo*
_tmp4D5;struct Cyc_List_List*_tmp4D6;struct Cyc_List_List*_tmp4D7;void*_tmp4D8;
struct Cyc_Absyn_FnInfo _tmp4DA;struct Cyc_List_List*_tmp4DB;struct Cyc_Core_Opt*
_tmp4DC;void*_tmp4DD;struct Cyc_List_List*_tmp4DE;int _tmp4DF;struct Cyc_Absyn_VarargInfo*
_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*_tmp4E2;void*_tmp4E3;
struct Cyc_List_List*_tmp4E5;void*_tmp4E6;struct Cyc_List_List*_tmp4E8;void*
_tmp4E9;enum Cyc_Absyn_AggrKind _tmp4EB;struct Cyc_List_List*_tmp4EC;void*_tmp4ED;
enum Cyc_Absyn_AggrKind _tmp4EF;struct Cyc_List_List*_tmp4F0;void*_tmp4F1;void*
_tmp4F3;void*_tmp4F5;void*_tmp4F7;void*_tmp4F9;void*_tmp4FB;void*_tmp4FC;void*
_tmp4FE;void*_tmp4FF;void*_tmp501;void*_tmp502;void*_tmp503;void*_tmp505;void*
_tmp506;void*_tmp507;void*_tmp509;void*_tmp50B;void*_tmp50D;void*_tmp50F;void*
_tmp511;_LL27D: _tmp44D=_tmp44C.f1;{struct Cyc_Absyn_Evar_struct*_tmp44E=(struct
Cyc_Absyn_Evar_struct*)_tmp44D;if(_tmp44E->tag != 1)goto _LL27F;};_LL27E: Cyc_Tcutil_unify_it(
t2,t1);return;_LL27F: _tmp44F=_tmp44C.f1;{struct Cyc_Absyn_VoidType_struct*_tmp450=(
struct Cyc_Absyn_VoidType_struct*)_tmp44F;if(_tmp450->tag != 0)goto _LL281;};
_tmp451=_tmp44C.f2;{struct Cyc_Absyn_VoidType_struct*_tmp452=(struct Cyc_Absyn_VoidType_struct*)
_tmp451;if(_tmp452->tag != 0)goto _LL281;};_LL280: return;_LL281: _tmp453=_tmp44C.f1;{
struct Cyc_Absyn_VarType_struct*_tmp454=(struct Cyc_Absyn_VarType_struct*)_tmp453;
if(_tmp454->tag != 2)goto _LL283;else{_tmp455=_tmp454->f1;}};_tmp456=_tmp44C.f2;{
struct Cyc_Absyn_VarType_struct*_tmp457=(struct Cyc_Absyn_VarType_struct*)_tmp456;
if(_tmp457->tag != 2)goto _LL283;else{_tmp458=_tmp457->f1;}};_LL282: {struct
_dyneither_ptr*_tmp513=_tmp455->name;struct _dyneither_ptr*_tmp514=_tmp458->name;
int _tmp515=_tmp455->identity;int _tmp516=_tmp458->identity;if(_tmp516 == _tmp515)
return;{const char*_tmp11B1;Cyc_Tcutil_failure_reason=((_tmp11B1="(variable types are not the same)",
_tag_dyneither(_tmp11B1,sizeof(char),34)));}goto _LL27C;}_LL283: _tmp459=_tmp44C.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp45A=(struct Cyc_Absyn_AggrType_struct*)
_tmp459;if(_tmp45A->tag != 12)goto _LL285;else{_tmp45B=_tmp45A->f1;_tmp45C=_tmp45B.aggr_info;
_tmp45D=_tmp45B.targs;}};_tmp45E=_tmp44C.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp45F=(struct Cyc_Absyn_AggrType_struct*)_tmp45E;if(_tmp45F->tag != 12)goto
_LL285;else{_tmp460=_tmp45F->f1;_tmp461=_tmp460.aggr_info;_tmp462=_tmp460.targs;}};
_LL284: {enum Cyc_Absyn_AggrKind _tmp519;struct _tuple2*_tmp51A;struct _tuple12
_tmp518=Cyc_Absyn_aggr_kinded_name(_tmp461);_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;{
enum Cyc_Absyn_AggrKind _tmp51C;struct _tuple2*_tmp51D;struct _tuple12 _tmp51B=Cyc_Absyn_aggr_kinded_name(
_tmp45C);_tmp51C=_tmp51B.f1;_tmp51D=_tmp51B.f2;if(_tmp519 != _tmp51C){{const char*
_tmp11B2;Cyc_Tcutil_failure_reason=((_tmp11B2="(struct and union type)",
_tag_dyneither(_tmp11B2,sizeof(char),24)));}goto _LL27C;}if(Cyc_Absyn_qvar_cmp(
_tmp51A,_tmp51D)!= 0){{const char*_tmp11B3;Cyc_Tcutil_failure_reason=((_tmp11B3="(different type name)",
_tag_dyneither(_tmp11B3,sizeof(char),22)));}goto _LL27C;}Cyc_Tcutil_unify_list(
_tmp462,_tmp45D);return;};}_LL285: _tmp463=_tmp44C.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp464=(struct Cyc_Absyn_EnumType_struct*)_tmp463;if(_tmp464->tag != 14)goto
_LL287;else{_tmp465=_tmp464->f1;}};_tmp466=_tmp44C.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp467=(struct Cyc_Absyn_EnumType_struct*)_tmp466;if(_tmp467->tag != 14)goto
_LL287;else{_tmp468=_tmp467->f1;}};_LL286: if(Cyc_Absyn_qvar_cmp(_tmp465,_tmp468)
== 0)return;{const char*_tmp11B4;Cyc_Tcutil_failure_reason=((_tmp11B4="(different enum types)",
_tag_dyneither(_tmp11B4,sizeof(char),23)));}goto _LL27C;_LL287: _tmp469=_tmp44C.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp46A=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp469;if(_tmp46A->tag != 15)goto _LL289;else{_tmp46B=_tmp46A->f1;}};_tmp46C=
_tmp44C.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp46D=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp46C;if(_tmp46D->tag != 15)goto _LL289;else{_tmp46E=_tmp46D->f1;}};_LL288: {int
bad=0;for(0;_tmp46B != 0  && _tmp46E != 0;(_tmp46B=_tmp46B->tl,_tmp46E=_tmp46E->tl)){
struct Cyc_Absyn_Enumfield*_tmp521=(struct Cyc_Absyn_Enumfield*)_tmp46B->hd;struct
Cyc_Absyn_Enumfield*_tmp522=(struct Cyc_Absyn_Enumfield*)_tmp46E->hd;if(Cyc_Absyn_qvar_cmp(
_tmp521->name,_tmp522->name)!= 0){{const char*_tmp11B5;Cyc_Tcutil_failure_reason=((
_tmp11B5="(different names for enum fields)",_tag_dyneither(_tmp11B5,sizeof(char),
34)));}bad=1;break;}if(_tmp521->tag == _tmp522->tag)continue;if(_tmp521->tag == 0
 || _tmp522->tag == 0){{const char*_tmp11B6;Cyc_Tcutil_failure_reason=((_tmp11B6="(different tag values for enum fields)",
_tag_dyneither(_tmp11B6,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp521->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp522->tag))){{const char*_tmp11B7;Cyc_Tcutil_failure_reason=((_tmp11B7="(different tag values for enum fields)",
_tag_dyneither(_tmp11B7,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL27C;if(
_tmp46B == 0  && _tmp46E == 0)return;{const char*_tmp11B8;Cyc_Tcutil_failure_reason=((
_tmp11B8="(different number of fields for enums)",_tag_dyneither(_tmp11B8,
sizeof(char),39)));}goto _LL27C;}_LL289: _tmp46F=_tmp44C.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp470=(struct Cyc_Absyn_DatatypeType_struct*)_tmp46F;if(_tmp470->tag != 3)goto
_LL28B;else{_tmp471=_tmp470->f1;_tmp472=_tmp471.datatype_info;if((_tmp472.KnownDatatype).tag
!= 2)goto _LL28B;_tmp473=(struct Cyc_Absyn_Datatypedecl**)(_tmp472.KnownDatatype).val;
_tmp474=*_tmp473;_tmp475=_tmp471.targs;}};_tmp476=_tmp44C.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp477=(struct Cyc_Absyn_DatatypeType_struct*)_tmp476;if(_tmp477->tag != 3)goto
_LL28B;else{_tmp478=_tmp477->f1;_tmp479=_tmp478.datatype_info;if((_tmp479.KnownDatatype).tag
!= 2)goto _LL28B;_tmp47A=(struct Cyc_Absyn_Datatypedecl**)(_tmp479.KnownDatatype).val;
_tmp47B=*_tmp47A;_tmp47C=_tmp478.targs;}};_LL28A: if(_tmp474 == _tmp47B  || Cyc_Absyn_qvar_cmp(
_tmp474->name,_tmp47B->name)== 0){Cyc_Tcutil_unify_list(_tmp47C,_tmp475);return;}
goto _LL27C;_LL28B: _tmp47D=_tmp44C.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp47E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp47D;if(_tmp47E->tag != 4)
goto _LL28D;else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47F.field_info;if((_tmp480.KnownDatatypefield).tag
!= 2)goto _LL28D;_tmp481=(struct _tuple3)(_tmp480.KnownDatatypefield).val;_tmp482=
_tmp481.f1;_tmp483=_tmp481.f2;_tmp484=_tmp47F.targs;}};_tmp485=_tmp44C.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp486=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp485;if(_tmp486->tag != 4)goto _LL28D;else{_tmp487=_tmp486->f1;_tmp488=_tmp487.field_info;
if((_tmp488.KnownDatatypefield).tag != 2)goto _LL28D;_tmp489=(struct _tuple3)(
_tmp488.KnownDatatypefield).val;_tmp48A=_tmp489.f1;_tmp48B=_tmp489.f2;_tmp48C=
_tmp487.targs;}};_LL28C: if((_tmp482 == _tmp48A  || Cyc_Absyn_qvar_cmp(_tmp482->name,
_tmp48A->name)== 0) && (_tmp483 == _tmp48B  || Cyc_Absyn_qvar_cmp(_tmp483->name,
_tmp48B->name)== 0)){Cyc_Tcutil_unify_list(_tmp48C,_tmp484);return;}{const char*
_tmp11B9;Cyc_Tcutil_failure_reason=((_tmp11B9="(different datatype field types)",
_tag_dyneither(_tmp11B9,sizeof(char),33)));}goto _LL27C;_LL28D: _tmp48D=_tmp44C.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp48E=(struct Cyc_Absyn_PointerType_struct*)
_tmp48D;if(_tmp48E->tag != 5)goto _LL28F;else{_tmp48F=_tmp48E->f1;_tmp490=_tmp48F.elt_typ;
_tmp491=_tmp48F.elt_tq;_tmp492=_tmp48F.ptr_atts;_tmp493=_tmp492.rgn;_tmp494=
_tmp492.nullable;_tmp495=_tmp492.bounds;_tmp496=_tmp492.zero_term;}};_tmp497=
_tmp44C.f2;{struct Cyc_Absyn_PointerType_struct*_tmp498=(struct Cyc_Absyn_PointerType_struct*)
_tmp497;if(_tmp498->tag != 5)goto _LL28F;else{_tmp499=_tmp498->f1;_tmp49A=_tmp499.elt_typ;
_tmp49B=_tmp499.elt_tq;_tmp49C=_tmp499.ptr_atts;_tmp49D=_tmp49C.rgn;_tmp49E=
_tmp49C.nullable;_tmp49F=_tmp49C.bounds;_tmp4A0=_tmp49C.zero_term;}};_LL28E: Cyc_Tcutil_unify_it(
_tmp49A,_tmp490);Cyc_Tcutil_unify_it(_tmp493,_tmp49D);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp11BA;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4A0,_tmp496,((_tmp11BA="(not both zero terminated)",
_tag_dyneither(_tmp11BA,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp49B,
_tmp49A,_tmp491,_tmp490);{const char*_tmp11BB;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp49F,_tmp495,((
_tmp11BB="(different pointer bounds)",_tag_dyneither(_tmp11BB,sizeof(char),27))));}{
void*_tmp52A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp49F);_LL2B8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp52B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp52A;if(_tmp52B->tag != 0)goto
_LL2BA;}_LL2B9: return;_LL2BA:;_LL2BB: goto _LL2B7;_LL2B7:;}{const char*_tmp11BC;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp49E,_tmp494,((_tmp11BC="(incompatible pointer types)",_tag_dyneither(
_tmp11BC,sizeof(char),29))));}return;_LL28F: _tmp4A1=_tmp44C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4A2=(struct Cyc_Absyn_IntType_struct*)_tmp4A1;if(_tmp4A2->tag != 6)goto _LL291;
else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;}};_tmp4A5=_tmp44C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4A6=(struct Cyc_Absyn_IntType_struct*)_tmp4A5;if(_tmp4A6->tag != 6)goto _LL291;
else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;}};_LL290: if(_tmp4A7 == _tmp4A3  && ((
_tmp4A8 == _tmp4A4  || _tmp4A8 == Cyc_Absyn_Int_sz  && _tmp4A4 == Cyc_Absyn_Long_sz)
 || _tmp4A8 == Cyc_Absyn_Long_sz  && _tmp4A4 == Cyc_Absyn_Int_sz))return;{const char*
_tmp11BD;Cyc_Tcutil_failure_reason=((_tmp11BD="(different integral types)",
_tag_dyneither(_tmp11BD,sizeof(char),27)));}goto _LL27C;_LL291: _tmp4A9=_tmp44C.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4AA=(struct Cyc_Absyn_FloatType_struct*)
_tmp4A9;if(_tmp4AA->tag != 7)goto _LL293;};_tmp4AB=_tmp44C.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4AC=(struct Cyc_Absyn_FloatType_struct*)_tmp4AB;if(_tmp4AC->tag != 7)goto
_LL293;};_LL292: return;_LL293: _tmp4AD=_tmp44C.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4AE=(struct Cyc_Absyn_DoubleType_struct*)_tmp4AD;if(_tmp4AE->tag != 8)goto
_LL295;else{_tmp4AF=_tmp4AE->f1;}};_tmp4B0=_tmp44C.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4B1=(struct Cyc_Absyn_DoubleType_struct*)_tmp4B0;if(_tmp4B1->tag != 8)goto
_LL295;else{_tmp4B2=_tmp4B1->f1;}};_LL294: if(_tmp4AF == _tmp4B2)return;goto _LL27C;
_LL295: _tmp4B3=_tmp44C.f1;{struct Cyc_Absyn_TagType_struct*_tmp4B4=(struct Cyc_Absyn_TagType_struct*)
_tmp4B3;if(_tmp4B4->tag != 20)goto _LL297;else{_tmp4B5=(void*)_tmp4B4->f1;}};
_tmp4B6=_tmp44C.f2;{struct Cyc_Absyn_TagType_struct*_tmp4B7=(struct Cyc_Absyn_TagType_struct*)
_tmp4B6;if(_tmp4B7->tag != 20)goto _LL297;else{_tmp4B8=(void*)_tmp4B7->f1;}};
_LL296: Cyc_Tcutil_unify_it(_tmp4B5,_tmp4B8);return;_LL297: _tmp4B9=_tmp44C.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp4BA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4B9;if(_tmp4BA->tag != 19)goto _LL299;else{_tmp4BB=_tmp4BA->f1;}};_tmp4BC=
_tmp44C.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp4BD=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4BC;if(_tmp4BD->tag != 19)goto _LL299;else{_tmp4BE=_tmp4BD->f1;}};_LL298: if(!
Cyc_Evexp_same_const_exp(_tmp4BB,_tmp4BE)){{const char*_tmp11BE;Cyc_Tcutil_failure_reason=((
_tmp11BE="(cannot prove expressions are the same)",_tag_dyneither(_tmp11BE,
sizeof(char),40)));}goto _LL27C;}return;_LL299: _tmp4BF=_tmp44C.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp4C0=(struct Cyc_Absyn_ArrayType_struct*)_tmp4BF;if(_tmp4C0->tag != 9)goto
_LL29B;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C1.elt_type;_tmp4C3=_tmp4C1.tq;
_tmp4C4=_tmp4C1.num_elts;_tmp4C5=_tmp4C1.zero_term;}};_tmp4C6=_tmp44C.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp4C7=(struct Cyc_Absyn_ArrayType_struct*)_tmp4C6;
if(_tmp4C7->tag != 9)goto _LL29B;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C8.elt_type;
_tmp4CA=_tmp4C8.tq;_tmp4CB=_tmp4C8.num_elts;_tmp4CC=_tmp4C8.zero_term;}};_LL29A:
Cyc_Tcutil_unify_it(_tmp4C9,_tmp4C2);Cyc_Tcutil_unify_tqual(_tmp4CA,_tmp4C9,
_tmp4C3,_tmp4C2);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp11BF;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4C5,_tmp4CC,((_tmp11BF="(not both zero terminated)",_tag_dyneither(_tmp11BF,
sizeof(char),27))));}if(_tmp4C4 == _tmp4CB)return;if(_tmp4C4 == 0  || _tmp4CB == 0)
goto _LL27C;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4C4,(struct Cyc_Absyn_Exp*)
_tmp4CB))return;{const char*_tmp11C0;Cyc_Tcutil_failure_reason=((_tmp11C0="(different array sizes)",
_tag_dyneither(_tmp11C0,sizeof(char),24)));}goto _LL27C;_LL29B: _tmp4CD=_tmp44C.f1;{
struct Cyc_Absyn_FnType_struct*_tmp4CE=(struct Cyc_Absyn_FnType_struct*)_tmp4CD;
if(_tmp4CE->tag != 10)goto _LL29D;else{_tmp4CF=_tmp4CE->f1;_tmp4D0=_tmp4CF.tvars;
_tmp4D1=_tmp4CF.effect;_tmp4D2=_tmp4CF.ret_typ;_tmp4D3=_tmp4CF.args;_tmp4D4=
_tmp4CF.c_varargs;_tmp4D5=_tmp4CF.cyc_varargs;_tmp4D6=_tmp4CF.rgn_po;_tmp4D7=
_tmp4CF.attributes;}};_tmp4D8=_tmp44C.f2;{struct Cyc_Absyn_FnType_struct*_tmp4D9=(
struct Cyc_Absyn_FnType_struct*)_tmp4D8;if(_tmp4D9->tag != 10)goto _LL29D;else{
_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4DA.tvars;_tmp4DC=_tmp4DA.effect;_tmp4DD=_tmp4DA.ret_typ;
_tmp4DE=_tmp4DA.args;_tmp4DF=_tmp4DA.c_varargs;_tmp4E0=_tmp4DA.cyc_varargs;
_tmp4E1=_tmp4DA.rgn_po;_tmp4E2=_tmp4DA.attributes;}};_LL29C: {int done=0;struct
_RegionHandle _tmp531=_new_region("rgn");struct _RegionHandle*rgn=& _tmp531;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp4DB != 0){if(_tmp4D0 == 0){{
const char*_tmp11C1;Cyc_Tcutil_failure_reason=((_tmp11C1="(second function type has too few type variables)",
_tag_dyneither(_tmp11C1,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp533=((struct Cyc_Absyn_Tvar*)_tmp4DB->hd)->kind;void*_tmp534=((struct Cyc_Absyn_Tvar*)
_tmp4D0->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp533,_tmp534)){{const char*
_tmp11C7;void*_tmp11C6[3];struct Cyc_String_pa_struct _tmp11C5;struct Cyc_String_pa_struct
_tmp11C4;struct Cyc_String_pa_struct _tmp11C3;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp11C3.tag=0,((_tmp11C3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp4D0->hd,& Cyc_Tcutil_bk))),((_tmp11C4.tag=0,((_tmp11C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp4DB->hd,& Cyc_Tcutil_bk))),((_tmp11C5.tag=0,((_tmp11C5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp4DB->hd)),((_tmp11C6[0]=& _tmp11C5,((_tmp11C6[1]=& _tmp11C4,((_tmp11C6[2]=&
_tmp11C3,Cyc_aprintf(((_tmp11C7="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp11C7,sizeof(char),44))),_tag_dyneither(_tmp11C6,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp11D1;struct Cyc_Absyn_VarType_struct _tmp11D0;struct Cyc_Absyn_VarType_struct*
_tmp11CF;struct Cyc_List_List*_tmp11CE;inst=((_tmp11CE=_region_malloc(rgn,sizeof(*
_tmp11CE)),((_tmp11CE->hd=((_tmp11D1=_region_malloc(rgn,sizeof(*_tmp11D1)),((
_tmp11D1->f1=(struct Cyc_Absyn_Tvar*)_tmp4D0->hd,((_tmp11D1->f2=(void*)((_tmp11CF=
_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF[0]=((_tmp11D0.tag=2,((_tmp11D0.f1=(
struct Cyc_Absyn_Tvar*)_tmp4DB->hd,_tmp11D0)))),_tmp11CF)))),_tmp11D1)))))),((
_tmp11CE->tl=inst,_tmp11CE))))));}_tmp4DB=_tmp4DB->tl;_tmp4D0=_tmp4D0->tl;};}if(
_tmp4D0 != 0){{const char*_tmp11D2;Cyc_Tcutil_failure_reason=((_tmp11D2="(second function type has too many type variables)",
_tag_dyneither(_tmp11D2,sizeof(char),51)));}_npop_handler(0);goto _LL27C;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp11DE;struct Cyc_Absyn_FnInfo _tmp11DD;
struct Cyc_Absyn_FnType_struct*_tmp11DC;struct Cyc_Absyn_FnType_struct _tmp11D8;
struct Cyc_Absyn_FnInfo _tmp11D7;struct Cyc_Absyn_FnType_struct*_tmp11D6;Cyc_Tcutil_unify_it((
void*)((_tmp11D6=_cycalloc(sizeof(*_tmp11D6)),((_tmp11D6[0]=((_tmp11D8.tag=10,((
_tmp11D8.f1=((_tmp11D7.tvars=0,((_tmp11D7.effect=_tmp4DC,((_tmp11D7.ret_typ=
_tmp4DD,((_tmp11D7.args=_tmp4DE,((_tmp11D7.c_varargs=_tmp4DF,((_tmp11D7.cyc_varargs=
_tmp4E0,((_tmp11D7.rgn_po=_tmp4E1,((_tmp11D7.attributes=_tmp4E2,_tmp11D7)))))))))))))))),
_tmp11D8)))),_tmp11D6)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp11DC=
_cycalloc(sizeof(*_tmp11DC)),((_tmp11DC[0]=((_tmp11DE.tag=10,((_tmp11DE.f1=((
_tmp11DD.tvars=0,((_tmp11DD.effect=_tmp4D1,((_tmp11DD.ret_typ=_tmp4D2,((_tmp11DD.args=
_tmp4D3,((_tmp11DD.c_varargs=_tmp4D4,((_tmp11DD.cyc_varargs=_tmp4D5,((_tmp11DD.rgn_po=
_tmp4D6,((_tmp11DD.attributes=_tmp4D7,_tmp11DD)))))))))))))))),_tmp11DE)))),
_tmp11DC))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp4DD,_tmp4D2);for(0;_tmp4DE != 0  && _tmp4D3 != 0;(_tmp4DE=_tmp4DE->tl,_tmp4D3=
_tmp4D3->tl)){struct Cyc_Absyn_Tqual _tmp546;void*_tmp547;struct _tuple9 _tmp545=*((
struct _tuple9*)_tmp4DE->hd);_tmp546=_tmp545.f2;_tmp547=_tmp545.f3;{struct Cyc_Absyn_Tqual
_tmp549;void*_tmp54A;struct _tuple9 _tmp548=*((struct _tuple9*)_tmp4D3->hd);_tmp549=
_tmp548.f2;_tmp54A=_tmp548.f3;Cyc_Tcutil_unify_it(_tmp547,_tmp54A);Cyc_Tcutil_unify_tqual(
_tmp546,_tmp547,_tmp549,_tmp54A);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp4DE != 0  || _tmp4D3 != 0){{const char*_tmp11DF;Cyc_Tcutil_failure_reason=((
_tmp11DF="(function types have different number of arguments)",_tag_dyneither(
_tmp11DF,sizeof(char),52)));}_npop_handler(0);goto _LL27C;}if(_tmp4DF != _tmp4D4){{
const char*_tmp11E0;Cyc_Tcutil_failure_reason=((_tmp11E0="(only one function type takes C varargs)",
_tag_dyneither(_tmp11E0,sizeof(char),41)));}_npop_handler(0);goto _LL27C;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp11E1;struct _tuple21 _tmp54E=(_tmp11E1.f1=
_tmp4E0,((_tmp11E1.f2=_tmp4D5,_tmp11E1)));struct Cyc_Absyn_VarargInfo*_tmp54F;
struct Cyc_Absyn_VarargInfo*_tmp550;struct Cyc_Absyn_VarargInfo*_tmp551;struct Cyc_Absyn_VarargInfo*
_tmp552;struct Cyc_Absyn_VarargInfo*_tmp553;struct Cyc_Absyn_VarargInfo _tmp554;
struct Cyc_Core_Opt*_tmp555;struct Cyc_Absyn_Tqual _tmp556;void*_tmp557;int _tmp558;
struct Cyc_Absyn_VarargInfo*_tmp559;struct Cyc_Absyn_VarargInfo _tmp55A;struct Cyc_Core_Opt*
_tmp55B;struct Cyc_Absyn_Tqual _tmp55C;void*_tmp55D;int _tmp55E;_LL2BD: _tmp54F=
_tmp54E.f1;if(_tmp54F != 0)goto _LL2BF;_tmp550=_tmp54E.f2;if(_tmp550 != 0)goto
_LL2BF;_LL2BE: goto _LL2BC;_LL2BF: _tmp551=_tmp54E.f1;if(_tmp551 != 0)goto _LL2C1;
_LL2C0: goto _LL2C2;_LL2C1: _tmp552=_tmp54E.f2;if(_tmp552 != 0)goto _LL2C3;_LL2C2:
bad_cyc_vararg=1;{const char*_tmp11E2;Cyc_Tcutil_failure_reason=((_tmp11E2="(only one function type takes varargs)",
_tag_dyneither(_tmp11E2,sizeof(char),39)));}goto _LL2BC;_LL2C3: _tmp553=_tmp54E.f1;
if(_tmp553 == 0)goto _LL2BC;_tmp554=*_tmp553;_tmp555=_tmp554.name;_tmp556=_tmp554.tq;
_tmp557=_tmp554.type;_tmp558=_tmp554.inject;_tmp559=_tmp54E.f2;if(_tmp559 == 0)
goto _LL2BC;_tmp55A=*_tmp559;_tmp55B=_tmp55A.name;_tmp55C=_tmp55A.tq;_tmp55D=
_tmp55A.type;_tmp55E=_tmp55A.inject;_LL2C4: Cyc_Tcutil_unify_it(_tmp557,_tmp55D);
Cyc_Tcutil_unify_tqual(_tmp556,_tmp557,_tmp55C,_tmp55D);if(_tmp558 != _tmp55E){
bad_cyc_vararg=1;{const char*_tmp11E3;Cyc_Tcutil_failure_reason=((_tmp11E3="(only one function type injects varargs)",
_tag_dyneither(_tmp11E3,sizeof(char),41)));};}goto _LL2BC;_LL2BC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL27C;}{int bad_effect=0;{struct _tuple22
_tmp11E4;struct _tuple22 _tmp562=(_tmp11E4.f1=_tmp4DC,((_tmp11E4.f2=_tmp4D1,
_tmp11E4)));struct Cyc_Core_Opt*_tmp563;struct Cyc_Core_Opt*_tmp564;struct Cyc_Core_Opt*
_tmp565;struct Cyc_Core_Opt*_tmp566;_LL2C6: _tmp563=_tmp562.f1;if(_tmp563 != 0)goto
_LL2C8;_tmp564=_tmp562.f2;if(_tmp564 != 0)goto _LL2C8;_LL2C7: goto _LL2C5;_LL2C8:
_tmp565=_tmp562.f1;if(_tmp565 != 0)goto _LL2CA;_LL2C9: goto _LL2CB;_LL2CA: _tmp566=
_tmp562.f2;if(_tmp566 != 0)goto _LL2CC;_LL2CB: bad_effect=1;goto _LL2C5;_LL2CC:;
_LL2CD: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4DC))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D1))->v);
goto _LL2C5;_LL2C5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp11E5;Cyc_Tcutil_failure_reason=((_tmp11E5="(function type effects do not unify)",
_tag_dyneither(_tmp11E5,sizeof(char),37)));}_npop_handler(0);goto _LL27C;}if(!Cyc_Tcutil_same_atts(
_tmp4D7,_tmp4E2)){{const char*_tmp11E6;Cyc_Tcutil_failure_reason=((_tmp11E6="(function types have different attributes)",
_tag_dyneither(_tmp11E6,sizeof(char),43)));}_npop_handler(0);goto _LL27C;}if(!Cyc_Tcutil_same_rgn_po(
_tmp4D6,_tmp4E1)){{const char*_tmp11E7;Cyc_Tcutil_failure_reason=((_tmp11E7="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp11E7,sizeof(char),58)));}_npop_handler(0);goto _LL27C;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL29D: _tmp4E3=_tmp44C.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp4E4=(struct Cyc_Absyn_TupleType_struct*)_tmp4E3;
if(_tmp4E4->tag != 11)goto _LL29F;else{_tmp4E5=_tmp4E4->f1;}};_tmp4E6=_tmp44C.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp4E7=(struct Cyc_Absyn_TupleType_struct*)
_tmp4E6;if(_tmp4E7->tag != 11)goto _LL29F;else{_tmp4E8=_tmp4E7->f1;}};_LL29E: for(0;
_tmp4E8 != 0  && _tmp4E5 != 0;(_tmp4E8=_tmp4E8->tl,_tmp4E5=_tmp4E5->tl)){struct Cyc_Absyn_Tqual
_tmp56B;void*_tmp56C;struct _tuple11 _tmp56A=*((struct _tuple11*)_tmp4E8->hd);
_tmp56B=_tmp56A.f1;_tmp56C=_tmp56A.f2;{struct Cyc_Absyn_Tqual _tmp56E;void*_tmp56F;
struct _tuple11 _tmp56D=*((struct _tuple11*)_tmp4E5->hd);_tmp56E=_tmp56D.f1;_tmp56F=
_tmp56D.f2;Cyc_Tcutil_unify_it(_tmp56C,_tmp56F);Cyc_Tcutil_unify_tqual(_tmp56B,
_tmp56C,_tmp56E,_tmp56F);};}if(_tmp4E8 == 0  && _tmp4E5 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp11E8;Cyc_Tcutil_failure_reason=((
_tmp11E8="(tuple types have different numbers of components)",_tag_dyneither(
_tmp11E8,sizeof(char),51)));}goto _LL27C;_LL29F: _tmp4E9=_tmp44C.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp4EA=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4E9;if(_tmp4EA->tag != 13)goto
_LL2A1;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;}};_tmp4ED=_tmp44C.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp4EE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4ED;if(_tmp4EE->tag != 13)goto _LL2A1;else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}};
_LL2A0: if(_tmp4EF != _tmp4EB){{const char*_tmp11E9;Cyc_Tcutil_failure_reason=((
_tmp11E9="(struct and union type)",_tag_dyneither(_tmp11E9,sizeof(char),24)));}
goto _LL27C;}for(0;_tmp4F0 != 0  && _tmp4EC != 0;(_tmp4F0=_tmp4F0->tl,_tmp4EC=
_tmp4EC->tl)){struct Cyc_Absyn_Aggrfield*_tmp572=(struct Cyc_Absyn_Aggrfield*)
_tmp4F0->hd;struct Cyc_Absyn_Aggrfield*_tmp573=(struct Cyc_Absyn_Aggrfield*)
_tmp4EC->hd;if(Cyc_strptrcmp(_tmp572->name,_tmp573->name)!= 0){{const char*
_tmp11EA;Cyc_Tcutil_failure_reason=((_tmp11EA="(different member names)",
_tag_dyneither(_tmp11EA,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp572->type,_tmp573->type);Cyc_Tcutil_unify_tqual(_tmp572->tq,
_tmp572->type,_tmp573->tq,_tmp573->type);if(!Cyc_Tcutil_same_atts(_tmp572->attributes,
_tmp573->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp11EB;Cyc_Tcutil_failure_reason=((_tmp11EB="(different attributes on member)",
_tag_dyneither(_tmp11EB,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp572->width != 0  && _tmp573->width == 0  || _tmp573->width != 0  && _tmp572->width
== 0) || (_tmp572->width != 0  && _tmp573->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp572->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp573->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp11EC;Cyc_Tcutil_failure_reason=((_tmp11EC="(different bitfield widths on member)",
_tag_dyneither(_tmp11EC,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp4F0 == 0  && _tmp4EC == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp11ED;Cyc_Tcutil_failure_reason=((_tmp11ED="(different number of members)",
_tag_dyneither(_tmp11ED,sizeof(char),30)));}goto _LL27C;_LL2A1: _tmp4F1=_tmp44C.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp4F2=(struct Cyc_Absyn_HeapRgn_struct*)_tmp4F1;
if(_tmp4F2->tag != 21)goto _LL2A3;};_tmp4F3=_tmp44C.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp4F4=(struct Cyc_Absyn_HeapRgn_struct*)_tmp4F3;if(_tmp4F4->tag != 21)goto _LL2A3;};
_LL2A2: return;_LL2A3: _tmp4F5=_tmp44C.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp4F6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp4F5;if(_tmp4F6->tag != 22)goto
_LL2A5;};_tmp4F7=_tmp44C.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp4F8=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp4F7;if(_tmp4F8->tag != 22)goto _LL2A5;};_LL2A4: return;_LL2A5: _tmp4F9=_tmp44C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp4FA=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4F9;if(_tmp4FA->tag != 16)goto _LL2A7;else{_tmp4FB=(void*)_tmp4FA->f1;}};
_tmp4FC=_tmp44C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp4FD=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4FC;if(_tmp4FD->tag != 16)goto _LL2A7;else{_tmp4FE=(void*)_tmp4FD->f1;}};
_LL2A6: Cyc_Tcutil_unify_it(_tmp4FB,_tmp4FE);return;_LL2A7: _tmp4FF=_tmp44C.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp500=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4FF;if(_tmp500->tag != 17)goto _LL2A9;else{_tmp501=(void*)_tmp500->f1;_tmp502=(
void*)_tmp500->f2;}};_tmp503=_tmp44C.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp504=(struct Cyc_Absyn_DynRgnType_struct*)_tmp503;if(_tmp504->tag != 17)goto
_LL2A9;else{_tmp505=(void*)_tmp504->f1;_tmp506=(void*)_tmp504->f2;}};_LL2A8: Cyc_Tcutil_unify_it(
_tmp501,_tmp505);Cyc_Tcutil_unify_it(_tmp502,_tmp506);return;_LL2A9: _tmp507=
_tmp44C.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp508=(struct Cyc_Absyn_JoinEff_struct*)
_tmp507;if(_tmp508->tag != 24)goto _LL2AB;};_LL2AA: goto _LL2AC;_LL2AB: _tmp509=
_tmp44C.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp50A=(struct Cyc_Absyn_JoinEff_struct*)
_tmp509;if(_tmp50A->tag != 24)goto _LL2AD;};_LL2AC: goto _LL2AE;_LL2AD: _tmp50B=
_tmp44C.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp50C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp50B;if(_tmp50C->tag != 23)goto _LL2AF;};_LL2AE: goto _LL2B0;_LL2AF: _tmp50D=
_tmp44C.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp50E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp50D;if(_tmp50E->tag != 25)goto _LL2B1;};_LL2B0: goto _LL2B2;_LL2B1: _tmp50F=
_tmp44C.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp510=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp50F;if(_tmp510->tag != 25)goto _LL2B3;};_LL2B2: goto _LL2B4;_LL2B3: _tmp511=
_tmp44C.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp512=(struct Cyc_Absyn_AccessEff_struct*)
_tmp511;if(_tmp512->tag != 23)goto _LL2B5;};_LL2B4: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp11EE;Cyc_Tcutil_failure_reason=((_tmp11EE="(effects don't unify)",
_tag_dyneither(_tmp11EE,sizeof(char),22)));}goto _LL27C;_LL2B5:;_LL2B6: goto _LL27C;
_LL27C:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return
- 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)
_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp579=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp57A=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp579,_tmp57A);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp57B=x;union Cyc_Absyn_Constraint _tmp57C;int _tmp57D;union Cyc_Absyn_Constraint
_tmp57E;void*_tmp57F;union Cyc_Absyn_Constraint _tmp580;union Cyc_Absyn_Constraint*
_tmp581;_LL2CF: _tmp57C=*_tmp57B;if((_tmp57C.No_constr).tag != 3)goto _LL2D1;
_tmp57D=(int)(_tmp57C.No_constr).val;_LL2D0: return - 1;_LL2D1: _tmp57E=*_tmp57B;if((
_tmp57E.Eq_constr).tag != 1)goto _LL2D3;_tmp57F=(void*)(_tmp57E.Eq_constr).val;
_LL2D2: {union Cyc_Absyn_Constraint*_tmp582=y;union Cyc_Absyn_Constraint _tmp583;
int _tmp584;union Cyc_Absyn_Constraint _tmp585;void*_tmp586;union Cyc_Absyn_Constraint
_tmp587;union Cyc_Absyn_Constraint*_tmp588;_LL2D6: _tmp583=*_tmp582;if((_tmp583.No_constr).tag
!= 3)goto _LL2D8;_tmp584=(int)(_tmp583.No_constr).val;_LL2D7: return 1;_LL2D8:
_tmp585=*_tmp582;if((_tmp585.Eq_constr).tag != 1)goto _LL2DA;_tmp586=(void*)(
_tmp585.Eq_constr).val;_LL2D9: return cmp(_tmp57F,_tmp586);_LL2DA: _tmp587=*_tmp582;
if((_tmp587.Forward_constr).tag != 2)goto _LL2D5;_tmp588=(union Cyc_Absyn_Constraint*)(
_tmp587.Forward_constr).val;_LL2DB: {const char*_tmp11F1;void*_tmp11F0;(_tmp11F0=
0,Cyc_Tcutil_impos(((_tmp11F1="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11F1,sizeof(char),40))),_tag_dyneither(_tmp11F0,sizeof(void*),
0)));}_LL2D5:;}_LL2D3: _tmp580=*_tmp57B;if((_tmp580.Forward_constr).tag != 2)goto
_LL2CE;_tmp581=(union Cyc_Absyn_Constraint*)(_tmp580.Forward_constr).val;_LL2D4: {
const char*_tmp11F4;void*_tmp11F3;(_tmp11F3=0,Cyc_Tcutil_impos(((_tmp11F4="unify_conref: forward after compress",
_tag_dyneither(_tmp11F4,sizeof(char),37))),_tag_dyneither(_tmp11F3,sizeof(void*),
0)));}_LL2CE:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp58E;struct Cyc_Absyn_Tqual _tmp58F;void*_tmp590;
struct _tuple11*_tmp58D=tqt1;_tmp58E=*_tmp58D;_tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;{
struct _tuple11 _tmp592;struct Cyc_Absyn_Tqual _tmp593;void*_tmp594;struct _tuple11*
_tmp591=tqt2;_tmp592=*_tmp591;_tmp593=_tmp592.f1;_tmp594=_tmp592.f2;{int _tmp595=
Cyc_Tcutil_tqual_cmp(_tmp58F,_tmp593);if(_tmp595 != 0)return _tmp595;return Cyc_Tcutil_typecmp(
_tmp590,_tmp594);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp596=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp596 != 0)return _tmp596;{int _tmp597=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp597 != 0)return _tmp597;{int _tmp598=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp598 != 0)return _tmp598;{int _tmp599=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp599 != 0)return _tmp599;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp59A=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp59A != 0)return _tmp59A;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp59B=t;_LL2DD: {struct Cyc_Absyn_VoidType_struct*_tmp59C=(struct
Cyc_Absyn_VoidType_struct*)_tmp59B;if(_tmp59C->tag != 0)goto _LL2DF;}_LL2DE: return
0;_LL2DF: {struct Cyc_Absyn_Evar_struct*_tmp59D=(struct Cyc_Absyn_Evar_struct*)
_tmp59B;if(_tmp59D->tag != 1)goto _LL2E1;}_LL2E0: return 1;_LL2E1: {struct Cyc_Absyn_VarType_struct*
_tmp59E=(struct Cyc_Absyn_VarType_struct*)_tmp59B;if(_tmp59E->tag != 2)goto _LL2E3;}
_LL2E2: return 2;_LL2E3: {struct Cyc_Absyn_DatatypeType_struct*_tmp59F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp59B;if(_tmp59F->tag != 3)goto _LL2E5;}_LL2E4: return 3;_LL2E5: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5A0=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp59B;if(_tmp5A0->tag != 4)
goto _LL2E7;}_LL2E6: return 4;_LL2E7: {struct Cyc_Absyn_PointerType_struct*_tmp5A1=(
struct Cyc_Absyn_PointerType_struct*)_tmp59B;if(_tmp5A1->tag != 5)goto _LL2E9;}
_LL2E8: return 5;_LL2E9: {struct Cyc_Absyn_IntType_struct*_tmp5A2=(struct Cyc_Absyn_IntType_struct*)
_tmp59B;if(_tmp5A2->tag != 6)goto _LL2EB;}_LL2EA: return 6;_LL2EB: {struct Cyc_Absyn_FloatType_struct*
_tmp5A3=(struct Cyc_Absyn_FloatType_struct*)_tmp59B;if(_tmp5A3->tag != 7)goto
_LL2ED;}_LL2EC: return 7;_LL2ED: {struct Cyc_Absyn_DoubleType_struct*_tmp5A4=(
struct Cyc_Absyn_DoubleType_struct*)_tmp59B;if(_tmp5A4->tag != 8)goto _LL2EF;}
_LL2EE: return 8;_LL2EF: {struct Cyc_Absyn_ArrayType_struct*_tmp5A5=(struct Cyc_Absyn_ArrayType_struct*)
_tmp59B;if(_tmp5A5->tag != 9)goto _LL2F1;}_LL2F0: return 9;_LL2F1: {struct Cyc_Absyn_FnType_struct*
_tmp5A6=(struct Cyc_Absyn_FnType_struct*)_tmp59B;if(_tmp5A6->tag != 10)goto _LL2F3;}
_LL2F2: return 10;_LL2F3: {struct Cyc_Absyn_TupleType_struct*_tmp5A7=(struct Cyc_Absyn_TupleType_struct*)
_tmp59B;if(_tmp5A7->tag != 11)goto _LL2F5;}_LL2F4: return 11;_LL2F5: {struct Cyc_Absyn_AggrType_struct*
_tmp5A8=(struct Cyc_Absyn_AggrType_struct*)_tmp59B;if(_tmp5A8->tag != 12)goto
_LL2F7;}_LL2F6: return 12;_LL2F7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5A9=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp59B;if(_tmp5A9->tag != 13)goto _LL2F9;}
_LL2F8: return 14;_LL2F9: {struct Cyc_Absyn_EnumType_struct*_tmp5AA=(struct Cyc_Absyn_EnumType_struct*)
_tmp59B;if(_tmp5AA->tag != 14)goto _LL2FB;}_LL2FA: return 16;_LL2FB: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp5AB=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp59B;if(_tmp5AB->tag != 15)goto
_LL2FD;}_LL2FC: return 17;_LL2FD: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5AC=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp59B;if(_tmp5AC->tag != 16)goto _LL2FF;}
_LL2FE: return 18;_LL2FF: {struct Cyc_Absyn_TypedefType_struct*_tmp5AD=(struct Cyc_Absyn_TypedefType_struct*)
_tmp59B;if(_tmp5AD->tag != 18)goto _LL301;}_LL300: return 19;_LL301: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp5AE=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp59B;if(_tmp5AE->tag != 22)goto
_LL303;}_LL302: return 20;_LL303: {struct Cyc_Absyn_HeapRgn_struct*_tmp5AF=(struct
Cyc_Absyn_HeapRgn_struct*)_tmp59B;if(_tmp5AF->tag != 21)goto _LL305;}_LL304: return
21;_LL305: {struct Cyc_Absyn_AccessEff_struct*_tmp5B0=(struct Cyc_Absyn_AccessEff_struct*)
_tmp59B;if(_tmp5B0->tag != 23)goto _LL307;}_LL306: return 22;_LL307: {struct Cyc_Absyn_JoinEff_struct*
_tmp5B1=(struct Cyc_Absyn_JoinEff_struct*)_tmp59B;if(_tmp5B1->tag != 24)goto _LL309;}
_LL308: return 23;_LL309: {struct Cyc_Absyn_RgnsEff_struct*_tmp5B2=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp59B;if(_tmp5B2->tag != 25)goto _LL30B;}_LL30A: return 24;_LL30B: {struct Cyc_Absyn_TagType_struct*
_tmp5B3=(struct Cyc_Absyn_TagType_struct*)_tmp59B;if(_tmp5B3->tag != 20)goto _LL30D;}
_LL30C: return 26;_LL30D: {struct Cyc_Absyn_DynRgnType_struct*_tmp5B4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp59B;if(_tmp5B4->tag != 17)goto _LL30F;}_LL30E: return 27;_LL30F: {struct Cyc_Absyn_ValueofType_struct*
_tmp5B5=(struct Cyc_Absyn_ValueofType_struct*)_tmp59B;if(_tmp5B5->tag != 19)goto
_LL2DC;}_LL310: return 28;_LL2DC:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp5B6=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp5B6 != 0)
return _tmp5B6;{struct _tuple0 _tmp11F5;struct _tuple0 _tmp5B8=(_tmp11F5.f1=t2,((
_tmp11F5.f2=t1,_tmp11F5)));void*_tmp5B9;void*_tmp5BB;void*_tmp5BD;struct Cyc_Absyn_Tvar*
_tmp5BF;void*_tmp5C0;struct Cyc_Absyn_Tvar*_tmp5C2;void*_tmp5C3;struct Cyc_Absyn_AggrInfo
_tmp5C5;union Cyc_Absyn_AggrInfoU _tmp5C6;struct Cyc_List_List*_tmp5C7;void*_tmp5C8;
struct Cyc_Absyn_AggrInfo _tmp5CA;union Cyc_Absyn_AggrInfoU _tmp5CB;struct Cyc_List_List*
_tmp5CC;void*_tmp5CD;struct _tuple2*_tmp5CF;void*_tmp5D0;struct _tuple2*_tmp5D2;
void*_tmp5D3;struct Cyc_List_List*_tmp5D5;void*_tmp5D6;struct Cyc_List_List*
_tmp5D8;void*_tmp5D9;struct Cyc_Absyn_DatatypeInfo _tmp5DB;union Cyc_Absyn_DatatypeInfoU
_tmp5DC;struct Cyc_Absyn_Datatypedecl**_tmp5DD;struct Cyc_Absyn_Datatypedecl*
_tmp5DE;struct Cyc_List_List*_tmp5DF;void*_tmp5E0;struct Cyc_Absyn_DatatypeInfo
_tmp5E2;union Cyc_Absyn_DatatypeInfoU _tmp5E3;struct Cyc_Absyn_Datatypedecl**
_tmp5E4;struct Cyc_Absyn_Datatypedecl*_tmp5E5;struct Cyc_List_List*_tmp5E6;void*
_tmp5E7;struct Cyc_Absyn_DatatypeFieldInfo _tmp5E9;union Cyc_Absyn_DatatypeFieldInfoU
_tmp5EA;struct _tuple3 _tmp5EB;struct Cyc_Absyn_Datatypedecl*_tmp5EC;struct Cyc_Absyn_Datatypefield*
_tmp5ED;struct Cyc_List_List*_tmp5EE;void*_tmp5EF;struct Cyc_Absyn_DatatypeFieldInfo
_tmp5F1;union Cyc_Absyn_DatatypeFieldInfoU _tmp5F2;struct _tuple3 _tmp5F3;struct Cyc_Absyn_Datatypedecl*
_tmp5F4;struct Cyc_Absyn_Datatypefield*_tmp5F5;struct Cyc_List_List*_tmp5F6;void*
_tmp5F7;struct Cyc_Absyn_PtrInfo _tmp5F9;void*_tmp5FA;struct Cyc_Absyn_Tqual _tmp5FB;
struct Cyc_Absyn_PtrAtts _tmp5FC;void*_tmp5FD;union Cyc_Absyn_Constraint*_tmp5FE;
union Cyc_Absyn_Constraint*_tmp5FF;union Cyc_Absyn_Constraint*_tmp600;void*_tmp601;
struct Cyc_Absyn_PtrInfo _tmp603;void*_tmp604;struct Cyc_Absyn_Tqual _tmp605;struct
Cyc_Absyn_PtrAtts _tmp606;void*_tmp607;union Cyc_Absyn_Constraint*_tmp608;union Cyc_Absyn_Constraint*
_tmp609;union Cyc_Absyn_Constraint*_tmp60A;void*_tmp60B;enum Cyc_Absyn_Sign
_tmp60D;enum Cyc_Absyn_Size_of _tmp60E;void*_tmp60F;enum Cyc_Absyn_Sign _tmp611;
enum Cyc_Absyn_Size_of _tmp612;void*_tmp613;void*_tmp615;void*_tmp617;int _tmp619;
void*_tmp61A;int _tmp61C;void*_tmp61D;struct Cyc_Absyn_ArrayInfo _tmp61F;void*
_tmp620;struct Cyc_Absyn_Tqual _tmp621;struct Cyc_Absyn_Exp*_tmp622;union Cyc_Absyn_Constraint*
_tmp623;void*_tmp624;struct Cyc_Absyn_ArrayInfo _tmp626;void*_tmp627;struct Cyc_Absyn_Tqual
_tmp628;struct Cyc_Absyn_Exp*_tmp629;union Cyc_Absyn_Constraint*_tmp62A;void*
_tmp62B;struct Cyc_Absyn_FnInfo _tmp62D;struct Cyc_List_List*_tmp62E;struct Cyc_Core_Opt*
_tmp62F;void*_tmp630;struct Cyc_List_List*_tmp631;int _tmp632;struct Cyc_Absyn_VarargInfo*
_tmp633;struct Cyc_List_List*_tmp634;struct Cyc_List_List*_tmp635;void*_tmp636;
struct Cyc_Absyn_FnInfo _tmp638;struct Cyc_List_List*_tmp639;struct Cyc_Core_Opt*
_tmp63A;void*_tmp63B;struct Cyc_List_List*_tmp63C;int _tmp63D;struct Cyc_Absyn_VarargInfo*
_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_List_List*_tmp640;void*_tmp641;
struct Cyc_List_List*_tmp643;void*_tmp644;struct Cyc_List_List*_tmp646;void*
_tmp647;enum Cyc_Absyn_AggrKind _tmp649;struct Cyc_List_List*_tmp64A;void*_tmp64B;
enum Cyc_Absyn_AggrKind _tmp64D;struct Cyc_List_List*_tmp64E;void*_tmp64F;void*
_tmp651;void*_tmp652;void*_tmp654;void*_tmp655;void*_tmp657;void*_tmp658;void*
_tmp659;void*_tmp65B;void*_tmp65C;void*_tmp65D;void*_tmp65F;void*_tmp660;void*
_tmp662;void*_tmp663;struct Cyc_Absyn_Exp*_tmp665;void*_tmp666;struct Cyc_Absyn_Exp*
_tmp668;void*_tmp669;void*_tmp66B;void*_tmp66D;void*_tmp66F;void*_tmp671;void*
_tmp673;_LL312: _tmp5B9=_tmp5B8.f1;{struct Cyc_Absyn_Evar_struct*_tmp5BA=(struct
Cyc_Absyn_Evar_struct*)_tmp5B9;if(_tmp5BA->tag != 1)goto _LL314;};_tmp5BB=_tmp5B8.f2;{
struct Cyc_Absyn_Evar_struct*_tmp5BC=(struct Cyc_Absyn_Evar_struct*)_tmp5BB;if(
_tmp5BC->tag != 1)goto _LL314;};_LL313: {const char*_tmp11F8;void*_tmp11F7;(
_tmp11F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11F8="typecmp: can only compare closed types",_tag_dyneither(_tmp11F8,
sizeof(char),39))),_tag_dyneither(_tmp11F7,sizeof(void*),0)));}_LL314: _tmp5BD=
_tmp5B8.f1;{struct Cyc_Absyn_VarType_struct*_tmp5BE=(struct Cyc_Absyn_VarType_struct*)
_tmp5BD;if(_tmp5BE->tag != 2)goto _LL316;else{_tmp5BF=_tmp5BE->f1;}};_tmp5C0=
_tmp5B8.f2;{struct Cyc_Absyn_VarType_struct*_tmp5C1=(struct Cyc_Absyn_VarType_struct*)
_tmp5C0;if(_tmp5C1->tag != 2)goto _LL316;else{_tmp5C2=_tmp5C1->f1;}};_LL315: return
Cyc_Core_intcmp(_tmp5C2->identity,_tmp5BF->identity);_LL316: _tmp5C3=_tmp5B8.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp5C4=(struct Cyc_Absyn_AggrType_struct*)
_tmp5C3;if(_tmp5C4->tag != 12)goto _LL318;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C5.aggr_info;
_tmp5C7=_tmp5C5.targs;}};_tmp5C8=_tmp5B8.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp5C9=(struct Cyc_Absyn_AggrType_struct*)_tmp5C8;if(_tmp5C9->tag != 12)goto
_LL318;else{_tmp5CA=_tmp5C9->f1;_tmp5CB=_tmp5CA.aggr_info;_tmp5CC=_tmp5CA.targs;}};
_LL317: {struct _tuple2*_tmp678;struct _tuple12 _tmp677=Cyc_Absyn_aggr_kinded_name(
_tmp5C6);_tmp678=_tmp677.f2;{struct _tuple2*_tmp67A;struct _tuple12 _tmp679=Cyc_Absyn_aggr_kinded_name(
_tmp5CB);_tmp67A=_tmp679.f2;{int _tmp67B=Cyc_Absyn_qvar_cmp(_tmp678,_tmp67A);if(
_tmp67B != 0)return _tmp67B;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5C7,_tmp5CC);}};};}
_LL318: _tmp5CD=_tmp5B8.f1;{struct Cyc_Absyn_EnumType_struct*_tmp5CE=(struct Cyc_Absyn_EnumType_struct*)
_tmp5CD;if(_tmp5CE->tag != 14)goto _LL31A;else{_tmp5CF=_tmp5CE->f1;}};_tmp5D0=
_tmp5B8.f2;{struct Cyc_Absyn_EnumType_struct*_tmp5D1=(struct Cyc_Absyn_EnumType_struct*)
_tmp5D0;if(_tmp5D1->tag != 14)goto _LL31A;else{_tmp5D2=_tmp5D1->f1;}};_LL319:
return Cyc_Absyn_qvar_cmp(_tmp5CF,_tmp5D2);_LL31A: _tmp5D3=_tmp5B8.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp5D4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D3;if(_tmp5D4->tag != 15)goto
_LL31C;else{_tmp5D5=_tmp5D4->f1;}};_tmp5D6=_tmp5B8.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp5D7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5D6;if(_tmp5D7->tag != 15)goto
_LL31C;else{_tmp5D8=_tmp5D7->f1;}};_LL31B: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp5D5,_tmp5D8);_LL31C: _tmp5D9=_tmp5B8.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp5DA=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5D9;if(_tmp5DA->tag != 3)goto
_LL31E;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=_tmp5DB.datatype_info;if((_tmp5DC.KnownDatatype).tag
!= 2)goto _LL31E;_tmp5DD=(struct Cyc_Absyn_Datatypedecl**)(_tmp5DC.KnownDatatype).val;
_tmp5DE=*_tmp5DD;_tmp5DF=_tmp5DB.targs;}};_tmp5E0=_tmp5B8.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp5E1=(struct Cyc_Absyn_DatatypeType_struct*)_tmp5E0;if(_tmp5E1->tag != 3)goto
_LL31E;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E2.datatype_info;if((_tmp5E3.KnownDatatype).tag
!= 2)goto _LL31E;_tmp5E4=(struct Cyc_Absyn_Datatypedecl**)(_tmp5E3.KnownDatatype).val;
_tmp5E5=*_tmp5E4;_tmp5E6=_tmp5E2.targs;}};_LL31D: if(_tmp5E5 == _tmp5DE)return 0;{
int _tmp67C=Cyc_Absyn_qvar_cmp(_tmp5E5->name,_tmp5DE->name);if(_tmp67C != 0)return
_tmp67C;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5E6,_tmp5DF);};_LL31E: _tmp5E7=
_tmp5B8.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp5E8=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp5E7;if(_tmp5E8->tag != 4)goto _LL320;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E9.field_info;
if((_tmp5EA.KnownDatatypefield).tag != 2)goto _LL320;_tmp5EB=(struct _tuple3)(
_tmp5EA.KnownDatatypefield).val;_tmp5EC=_tmp5EB.f1;_tmp5ED=_tmp5EB.f2;_tmp5EE=
_tmp5E9.targs;}};_tmp5EF=_tmp5B8.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5F0=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5EF;if(_tmp5F0->tag != 4)
goto _LL320;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F1.field_info;if((_tmp5F2.KnownDatatypefield).tag
!= 2)goto _LL320;_tmp5F3=(struct _tuple3)(_tmp5F2.KnownDatatypefield).val;_tmp5F4=
_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;_tmp5F6=_tmp5F1.targs;}};_LL31F: if(_tmp5F4 == 
_tmp5EC)return 0;{int _tmp67D=Cyc_Absyn_qvar_cmp(_tmp5EC->name,_tmp5F4->name);if(
_tmp67D != 0)return _tmp67D;{int _tmp67E=Cyc_Absyn_qvar_cmp(_tmp5ED->name,_tmp5F5->name);
if(_tmp67E != 0)return _tmp67E;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5F6,_tmp5EE);};};
_LL320: _tmp5F7=_tmp5B8.f1;{struct Cyc_Absyn_PointerType_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_struct*)
_tmp5F7;if(_tmp5F8->tag != 5)goto _LL322;else{_tmp5F9=_tmp5F8->f1;_tmp5FA=_tmp5F9.elt_typ;
_tmp5FB=_tmp5F9.elt_tq;_tmp5FC=_tmp5F9.ptr_atts;_tmp5FD=_tmp5FC.rgn;_tmp5FE=
_tmp5FC.nullable;_tmp5FF=_tmp5FC.bounds;_tmp600=_tmp5FC.zero_term;}};_tmp601=
_tmp5B8.f2;{struct Cyc_Absyn_PointerType_struct*_tmp602=(struct Cyc_Absyn_PointerType_struct*)
_tmp601;if(_tmp602->tag != 5)goto _LL322;else{_tmp603=_tmp602->f1;_tmp604=_tmp603.elt_typ;
_tmp605=_tmp603.elt_tq;_tmp606=_tmp603.ptr_atts;_tmp607=_tmp606.rgn;_tmp608=
_tmp606.nullable;_tmp609=_tmp606.bounds;_tmp60A=_tmp606.zero_term;}};_LL321: {
int _tmp67F=Cyc_Tcutil_typecmp(_tmp604,_tmp5FA);if(_tmp67F != 0)return _tmp67F;{int
_tmp680=Cyc_Tcutil_typecmp(_tmp607,_tmp5FD);if(_tmp680 != 0)return _tmp680;{int
_tmp681=Cyc_Tcutil_tqual_cmp(_tmp605,_tmp5FB);if(_tmp681 != 0)return _tmp681;{int
_tmp682=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp609,_tmp5FF);if(_tmp682 != 0)
return _tmp682;{int _tmp683=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp60A,
_tmp600);if(_tmp683 != 0)return _tmp683;{void*_tmp684=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp609);_LL347: {struct Cyc_Absyn_DynEither_b_struct*
_tmp685=(struct Cyc_Absyn_DynEither_b_struct*)_tmp684;if(_tmp685->tag != 0)goto
_LL349;}_LL348: return 0;_LL349:;_LL34A: goto _LL346;_LL346:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp608,_tmp5FE);};};};};}_LL322: _tmp60B=_tmp5B8.f1;{struct Cyc_Absyn_IntType_struct*
_tmp60C=(struct Cyc_Absyn_IntType_struct*)_tmp60B;if(_tmp60C->tag != 6)goto _LL324;
else{_tmp60D=_tmp60C->f1;_tmp60E=_tmp60C->f2;}};_tmp60F=_tmp5B8.f2;{struct Cyc_Absyn_IntType_struct*
_tmp610=(struct Cyc_Absyn_IntType_struct*)_tmp60F;if(_tmp610->tag != 6)goto _LL324;
else{_tmp611=_tmp610->f1;_tmp612=_tmp610->f2;}};_LL323: if(_tmp611 != _tmp60D)
return Cyc_Core_intcmp((int)((unsigned int)_tmp611),(int)((unsigned int)_tmp60D));
if(_tmp612 != _tmp60E)return Cyc_Core_intcmp((int)((unsigned int)_tmp612),(int)((
unsigned int)_tmp60E));return 0;_LL324: _tmp613=_tmp5B8.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp614=(struct Cyc_Absyn_FloatType_struct*)_tmp613;if(_tmp614->tag != 7)goto
_LL326;};_tmp615=_tmp5B8.f2;{struct Cyc_Absyn_FloatType_struct*_tmp616=(struct Cyc_Absyn_FloatType_struct*)
_tmp615;if(_tmp616->tag != 7)goto _LL326;};_LL325: return 0;_LL326: _tmp617=_tmp5B8.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp618=(struct Cyc_Absyn_DoubleType_struct*)
_tmp617;if(_tmp618->tag != 8)goto _LL328;else{_tmp619=_tmp618->f1;}};_tmp61A=
_tmp5B8.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp61B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp61A;if(_tmp61B->tag != 8)goto _LL328;else{_tmp61C=_tmp61B->f1;}};_LL327: if(
_tmp619 == _tmp61C)return 0;else{if(_tmp619)return - 1;else{return 1;}}_LL328: _tmp61D=
_tmp5B8.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp61E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp61D;if(_tmp61E->tag != 9)goto _LL32A;else{_tmp61F=_tmp61E->f1;_tmp620=_tmp61F.elt_type;
_tmp621=_tmp61F.tq;_tmp622=_tmp61F.num_elts;_tmp623=_tmp61F.zero_term;}};_tmp624=
_tmp5B8.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp625=(struct Cyc_Absyn_ArrayType_struct*)
_tmp624;if(_tmp625->tag != 9)goto _LL32A;else{_tmp626=_tmp625->f1;_tmp627=_tmp626.elt_type;
_tmp628=_tmp626.tq;_tmp629=_tmp626.num_elts;_tmp62A=_tmp626.zero_term;}};_LL329: {
int _tmp686=Cyc_Tcutil_tqual_cmp(_tmp628,_tmp621);if(_tmp686 != 0)return _tmp686;{
int _tmp687=Cyc_Tcutil_typecmp(_tmp627,_tmp620);if(_tmp687 != 0)return _tmp687;{int
_tmp688=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp623,_tmp62A);if(_tmp688 != 0)
return _tmp688;if(_tmp622 == _tmp629)return 0;if(_tmp622 == 0  || _tmp629 == 0){const
char*_tmp11FB;void*_tmp11FA;(_tmp11FA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11FB="missing expression in array index",
_tag_dyneither(_tmp11FB,sizeof(char),34))),_tag_dyneither(_tmp11FA,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp622,_tmp629);};};}_LL32A: _tmp62B=_tmp5B8.f1;{struct Cyc_Absyn_FnType_struct*
_tmp62C=(struct Cyc_Absyn_FnType_struct*)_tmp62B;if(_tmp62C->tag != 10)goto _LL32C;
else{_tmp62D=_tmp62C->f1;_tmp62E=_tmp62D.tvars;_tmp62F=_tmp62D.effect;_tmp630=
_tmp62D.ret_typ;_tmp631=_tmp62D.args;_tmp632=_tmp62D.c_varargs;_tmp633=_tmp62D.cyc_varargs;
_tmp634=_tmp62D.rgn_po;_tmp635=_tmp62D.attributes;}};_tmp636=_tmp5B8.f2;{struct
Cyc_Absyn_FnType_struct*_tmp637=(struct Cyc_Absyn_FnType_struct*)_tmp636;if(
_tmp637->tag != 10)goto _LL32C;else{_tmp638=_tmp637->f1;_tmp639=_tmp638.tvars;
_tmp63A=_tmp638.effect;_tmp63B=_tmp638.ret_typ;_tmp63C=_tmp638.args;_tmp63D=
_tmp638.c_varargs;_tmp63E=_tmp638.cyc_varargs;_tmp63F=_tmp638.rgn_po;_tmp640=
_tmp638.attributes;}};_LL32B: {const char*_tmp11FE;void*_tmp11FD;(_tmp11FD=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11FE="typecmp: function types not handled",_tag_dyneither(_tmp11FE,sizeof(
char),36))),_tag_dyneither(_tmp11FD,sizeof(void*),0)));}_LL32C: _tmp641=_tmp5B8.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp642=(struct Cyc_Absyn_TupleType_struct*)
_tmp641;if(_tmp642->tag != 11)goto _LL32E;else{_tmp643=_tmp642->f1;}};_tmp644=
_tmp5B8.f2;{struct Cyc_Absyn_TupleType_struct*_tmp645=(struct Cyc_Absyn_TupleType_struct*)
_tmp644;if(_tmp645->tag != 11)goto _LL32E;else{_tmp646=_tmp645->f1;}};_LL32D:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp646,
_tmp643);_LL32E: _tmp647=_tmp5B8.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp648=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp647;if(_tmp648->tag != 13)goto _LL330;
else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;}};_tmp64B=_tmp5B8.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp64C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp64B;if(_tmp64C->tag != 13)goto
_LL330;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;}};_LL32F: if(_tmp64D != 
_tmp649){if(_tmp64D == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp64E,
_tmp64A);_LL330: _tmp64F=_tmp5B8.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp650=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp64F;if(_tmp650->tag != 16)goto _LL332;
else{_tmp651=(void*)_tmp650->f1;}};_tmp652=_tmp5B8.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp653=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp652;if(_tmp653->tag != 16)goto
_LL332;else{_tmp654=(void*)_tmp653->f1;}};_LL331: return Cyc_Tcutil_typecmp(
_tmp651,_tmp654);_LL332: _tmp655=_tmp5B8.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp656=(struct Cyc_Absyn_DynRgnType_struct*)_tmp655;if(_tmp656->tag != 17)goto
_LL334;else{_tmp657=(void*)_tmp656->f1;_tmp658=(void*)_tmp656->f2;}};_tmp659=
_tmp5B8.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp65A=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp659;if(_tmp65A->tag != 17)goto _LL334;else{_tmp65B=(void*)_tmp65A->f1;_tmp65C=(
void*)_tmp65A->f2;}};_LL333: {int _tmp68D=Cyc_Tcutil_typecmp(_tmp657,_tmp65B);if(
_tmp68D != 0)return _tmp68D;else{return Cyc_Tcutil_typecmp(_tmp658,_tmp65C);}}
_LL334: _tmp65D=_tmp5B8.f1;{struct Cyc_Absyn_TagType_struct*_tmp65E=(struct Cyc_Absyn_TagType_struct*)
_tmp65D;if(_tmp65E->tag != 20)goto _LL336;else{_tmp65F=(void*)_tmp65E->f1;}};
_tmp660=_tmp5B8.f2;{struct Cyc_Absyn_TagType_struct*_tmp661=(struct Cyc_Absyn_TagType_struct*)
_tmp660;if(_tmp661->tag != 20)goto _LL336;else{_tmp662=(void*)_tmp661->f1;}};
_LL335: return Cyc_Tcutil_typecmp(_tmp65F,_tmp662);_LL336: _tmp663=_tmp5B8.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp664=(struct Cyc_Absyn_ValueofType_struct*)
_tmp663;if(_tmp664->tag != 19)goto _LL338;else{_tmp665=_tmp664->f1;}};_tmp666=
_tmp5B8.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp667=(struct Cyc_Absyn_ValueofType_struct*)
_tmp666;if(_tmp667->tag != 19)goto _LL338;else{_tmp668=_tmp667->f1;}};_LL337:
return Cyc_Evexp_const_exp_cmp(_tmp665,_tmp668);_LL338: _tmp669=_tmp5B8.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp66A=(struct Cyc_Absyn_JoinEff_struct*)_tmp669;if(
_tmp66A->tag != 24)goto _LL33A;};_LL339: goto _LL33B;_LL33A: _tmp66B=_tmp5B8.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp66C=(struct Cyc_Absyn_JoinEff_struct*)_tmp66B;
if(_tmp66C->tag != 24)goto _LL33C;};_LL33B: goto _LL33D;_LL33C: _tmp66D=_tmp5B8.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp66E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp66D;if(_tmp66E->tag != 23)goto _LL33E;};_LL33D: goto _LL33F;_LL33E: _tmp66F=
_tmp5B8.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp670=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp66F;if(_tmp670->tag != 25)goto _LL340;};_LL33F: goto _LL341;_LL340: _tmp671=
_tmp5B8.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp672=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp671;if(_tmp672->tag != 25)goto _LL342;};_LL341: goto _LL343;_LL342: _tmp673=
_tmp5B8.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp674=(struct Cyc_Absyn_AccessEff_struct*)
_tmp673;if(_tmp674->tag != 23)goto _LL344;};_LL343: {const char*_tmp1201;void*
_tmp1200;(_tmp1200=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1201="typecmp: effects not handled",_tag_dyneither(
_tmp1201,sizeof(char),29))),_tag_dyneither(_tmp1200,sizeof(void*),0)));}_LL344:;
_LL345: {const char*_tmp1204;void*_tmp1203;(_tmp1203=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1204="Unmatched case in typecmp",
_tag_dyneither(_tmp1204,sizeof(char),26))),_tag_dyneither(_tmp1203,sizeof(void*),
0)));}_LL311:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp692=Cyc_Tcutil_compress(
t);_LL34C: {struct Cyc_Absyn_IntType_struct*_tmp693=(struct Cyc_Absyn_IntType_struct*)
_tmp692;if(_tmp693->tag != 6)goto _LL34E;}_LL34D: goto _LL34F;_LL34E: {struct Cyc_Absyn_FloatType_struct*
_tmp694=(struct Cyc_Absyn_FloatType_struct*)_tmp692;if(_tmp694->tag != 7)goto
_LL350;}_LL34F: goto _LL351;_LL350: {struct Cyc_Absyn_DoubleType_struct*_tmp695=(
struct Cyc_Absyn_DoubleType_struct*)_tmp692;if(_tmp695->tag != 8)goto _LL352;}
_LL351: goto _LL353;_LL352: {struct Cyc_Absyn_EnumType_struct*_tmp696=(struct Cyc_Absyn_EnumType_struct*)
_tmp692;if(_tmp696->tag != 14)goto _LL354;}_LL353: goto _LL355;_LL354: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp697=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp692;if(_tmp697->tag != 15)goto
_LL356;}_LL355: return 1;_LL356:;_LL357: return 0;_LL34B:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1205;struct _tuple0 _tmp699=(_tmp1205.f1=t1,((_tmp1205.f2=t2,_tmp1205)));
void*_tmp69A;int _tmp69C;void*_tmp69D;int _tmp69F;void*_tmp6A0;void*_tmp6A2;void*
_tmp6A4;void*_tmp6A6;void*_tmp6A8;void*_tmp6AA;void*_tmp6AC;void*_tmp6AE;void*
_tmp6B0;void*_tmp6B2;void*_tmp6B4;enum Cyc_Absyn_Size_of _tmp6B6;void*_tmp6B7;
enum Cyc_Absyn_Size_of _tmp6B9;void*_tmp6BA;enum Cyc_Absyn_Size_of _tmp6BC;void*
_tmp6BD;enum Cyc_Absyn_Size_of _tmp6BF;void*_tmp6C0;enum Cyc_Absyn_Size_of _tmp6C2;
void*_tmp6C3;enum Cyc_Absyn_Size_of _tmp6C5;void*_tmp6C6;enum Cyc_Absyn_Size_of
_tmp6C8;void*_tmp6C9;enum Cyc_Absyn_Size_of _tmp6CB;void*_tmp6CC;void*_tmp6CE;
enum Cyc_Absyn_Size_of _tmp6D0;void*_tmp6D1;void*_tmp6D3;enum Cyc_Absyn_Size_of
_tmp6D5;void*_tmp6D6;enum Cyc_Absyn_Size_of _tmp6D8;void*_tmp6D9;enum Cyc_Absyn_Size_of
_tmp6DB;void*_tmp6DC;enum Cyc_Absyn_Size_of _tmp6DE;void*_tmp6DF;void*_tmp6E1;
enum Cyc_Absyn_Size_of _tmp6E3;void*_tmp6E4;enum Cyc_Absyn_Size_of _tmp6E6;void*
_tmp6E7;enum Cyc_Absyn_Size_of _tmp6E9;void*_tmp6EA;enum Cyc_Absyn_Size_of _tmp6EC;
void*_tmp6ED;enum Cyc_Absyn_Size_of _tmp6EF;void*_tmp6F0;enum Cyc_Absyn_Size_of
_tmp6F2;void*_tmp6F3;enum Cyc_Absyn_Size_of _tmp6F5;void*_tmp6F6;void*_tmp6F8;
enum Cyc_Absyn_Size_of _tmp6FA;_LL359: _tmp69A=_tmp699.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp69B=(struct Cyc_Absyn_DoubleType_struct*)_tmp69A;if(_tmp69B->tag != 8)goto
_LL35B;else{_tmp69C=_tmp69B->f1;}};_tmp69D=_tmp699.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp69E=(struct Cyc_Absyn_DoubleType_struct*)_tmp69D;if(_tmp69E->tag != 8)goto
_LL35B;else{_tmp69F=_tmp69E->f1;}};_LL35A: return !_tmp69F  && _tmp69C;_LL35B:
_tmp6A0=_tmp699.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6A1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6A0;if(_tmp6A1->tag != 8)goto _LL35D;};_tmp6A2=_tmp699.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6A3=(struct Cyc_Absyn_FloatType_struct*)_tmp6A2;if(_tmp6A3->tag != 7)goto
_LL35D;};_LL35C: goto _LL35E;_LL35D: _tmp6A4=_tmp699.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6A5=(struct Cyc_Absyn_DoubleType_struct*)_tmp6A4;if(_tmp6A5->tag != 8)goto
_LL35F;};_tmp6A6=_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6A7=(struct Cyc_Absyn_IntType_struct*)
_tmp6A6;if(_tmp6A7->tag != 6)goto _LL35F;};_LL35E: goto _LL360;_LL35F: _tmp6A8=
_tmp699.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6A9=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6A8;if(_tmp6A9->tag != 8)goto _LL361;};_tmp6AA=_tmp699.f2;{struct Cyc_Absyn_TagType_struct*
_tmp6AB=(struct Cyc_Absyn_TagType_struct*)_tmp6AA;if(_tmp6AB->tag != 20)goto _LL361;};
_LL360: goto _LL362;_LL361: _tmp6AC=_tmp699.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6AD=(struct Cyc_Absyn_FloatType_struct*)_tmp6AC;if(_tmp6AD->tag != 7)goto
_LL363;};_tmp6AE=_tmp699.f2;{struct Cyc_Absyn_TagType_struct*_tmp6AF=(struct Cyc_Absyn_TagType_struct*)
_tmp6AE;if(_tmp6AF->tag != 20)goto _LL363;};_LL362: goto _LL364;_LL363: _tmp6B0=
_tmp699.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6B1=(struct Cyc_Absyn_FloatType_struct*)
_tmp6B0;if(_tmp6B1->tag != 7)goto _LL365;};_tmp6B2=_tmp699.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6B3=(struct Cyc_Absyn_IntType_struct*)_tmp6B2;if(_tmp6B3->tag != 6)goto _LL365;};
_LL364: return 1;_LL365: _tmp6B4=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*_tmp6B5=(
struct Cyc_Absyn_IntType_struct*)_tmp6B4;if(_tmp6B5->tag != 6)goto _LL367;else{
_tmp6B6=_tmp6B5->f2;if(_tmp6B6 != Cyc_Absyn_LongLong_sz)goto _LL367;}};_tmp6B7=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6B8=(struct Cyc_Absyn_IntType_struct*)
_tmp6B7;if(_tmp6B8->tag != 6)goto _LL367;else{_tmp6B9=_tmp6B8->f2;if(_tmp6B9 != Cyc_Absyn_LongLong_sz)
goto _LL367;}};_LL366: return 0;_LL367: _tmp6BA=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6BB=(struct Cyc_Absyn_IntType_struct*)_tmp6BA;if(_tmp6BB->tag != 6)goto _LL369;
else{_tmp6BC=_tmp6BB->f2;if(_tmp6BC != Cyc_Absyn_LongLong_sz)goto _LL369;}};_LL368:
return 1;_LL369: _tmp6BD=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*_tmp6BE=(
struct Cyc_Absyn_IntType_struct*)_tmp6BD;if(_tmp6BE->tag != 6)goto _LL36B;else{
_tmp6BF=_tmp6BE->f2;if(_tmp6BF != Cyc_Absyn_Long_sz)goto _LL36B;}};_tmp6C0=_tmp699.f2;{
struct Cyc_Absyn_IntType_struct*_tmp6C1=(struct Cyc_Absyn_IntType_struct*)_tmp6C0;
if(_tmp6C1->tag != 6)goto _LL36B;else{_tmp6C2=_tmp6C1->f2;if(_tmp6C2 != Cyc_Absyn_Int_sz)
goto _LL36B;}};_LL36A: goto _LL36C;_LL36B: _tmp6C3=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6C4=(struct Cyc_Absyn_IntType_struct*)_tmp6C3;if(_tmp6C4->tag != 6)goto _LL36D;
else{_tmp6C5=_tmp6C4->f2;if(_tmp6C5 != Cyc_Absyn_Int_sz)goto _LL36D;}};_tmp6C6=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6C7=(struct Cyc_Absyn_IntType_struct*)
_tmp6C6;if(_tmp6C7->tag != 6)goto _LL36D;else{_tmp6C8=_tmp6C7->f2;if(_tmp6C8 != Cyc_Absyn_Long_sz)
goto _LL36D;}};_LL36C: return 0;_LL36D: _tmp6C9=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6CA=(struct Cyc_Absyn_IntType_struct*)_tmp6C9;if(_tmp6CA->tag != 6)goto _LL36F;
else{_tmp6CB=_tmp6CA->f2;if(_tmp6CB != Cyc_Absyn_Long_sz)goto _LL36F;}};_tmp6CC=
_tmp699.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6CD=(struct Cyc_Absyn_FloatType_struct*)
_tmp6CC;if(_tmp6CD->tag != 7)goto _LL36F;};_LL36E: goto _LL370;_LL36F: _tmp6CE=
_tmp699.f1;{struct Cyc_Absyn_IntType_struct*_tmp6CF=(struct Cyc_Absyn_IntType_struct*)
_tmp6CE;if(_tmp6CF->tag != 6)goto _LL371;else{_tmp6D0=_tmp6CF->f2;if(_tmp6D0 != Cyc_Absyn_Int_sz)
goto _LL371;}};_tmp6D1=_tmp699.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6D2=(
struct Cyc_Absyn_FloatType_struct*)_tmp6D1;if(_tmp6D2->tag != 7)goto _LL371;};
_LL370: goto _LL372;_LL371: _tmp6D3=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6D4=(struct Cyc_Absyn_IntType_struct*)_tmp6D3;if(_tmp6D4->tag != 6)goto _LL373;
else{_tmp6D5=_tmp6D4->f2;if(_tmp6D5 != Cyc_Absyn_Long_sz)goto _LL373;}};_tmp6D6=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6D7=(struct Cyc_Absyn_IntType_struct*)
_tmp6D6;if(_tmp6D7->tag != 6)goto _LL373;else{_tmp6D8=_tmp6D7->f2;if(_tmp6D8 != Cyc_Absyn_Short_sz)
goto _LL373;}};_LL372: goto _LL374;_LL373: _tmp6D9=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6DA=(struct Cyc_Absyn_IntType_struct*)_tmp6D9;if(_tmp6DA->tag != 6)goto _LL375;
else{_tmp6DB=_tmp6DA->f2;if(_tmp6DB != Cyc_Absyn_Int_sz)goto _LL375;}};_tmp6DC=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6DD=(struct Cyc_Absyn_IntType_struct*)
_tmp6DC;if(_tmp6DD->tag != 6)goto _LL375;else{_tmp6DE=_tmp6DD->f2;if(_tmp6DE != Cyc_Absyn_Short_sz)
goto _LL375;}};_LL374: goto _LL376;_LL375: _tmp6DF=_tmp699.f1;{struct Cyc_Absyn_TagType_struct*
_tmp6E0=(struct Cyc_Absyn_TagType_struct*)_tmp6DF;if(_tmp6E0->tag != 20)goto _LL377;};
_tmp6E1=_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6E2=(struct Cyc_Absyn_IntType_struct*)
_tmp6E1;if(_tmp6E2->tag != 6)goto _LL377;else{_tmp6E3=_tmp6E2->f2;if(_tmp6E3 != Cyc_Absyn_Short_sz)
goto _LL377;}};_LL376: goto _LL378;_LL377: _tmp6E4=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6E5=(struct Cyc_Absyn_IntType_struct*)_tmp6E4;if(_tmp6E5->tag != 6)goto _LL379;
else{_tmp6E6=_tmp6E5->f2;if(_tmp6E6 != Cyc_Absyn_Long_sz)goto _LL379;}};_tmp6E7=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6E8=(struct Cyc_Absyn_IntType_struct*)
_tmp6E7;if(_tmp6E8->tag != 6)goto _LL379;else{_tmp6E9=_tmp6E8->f2;if(_tmp6E9 != Cyc_Absyn_Char_sz)
goto _LL379;}};_LL378: goto _LL37A;_LL379: _tmp6EA=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6EB=(struct Cyc_Absyn_IntType_struct*)_tmp6EA;if(_tmp6EB->tag != 6)goto _LL37B;
else{_tmp6EC=_tmp6EB->f2;if(_tmp6EC != Cyc_Absyn_Int_sz)goto _LL37B;}};_tmp6ED=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6EE=(struct Cyc_Absyn_IntType_struct*)
_tmp6ED;if(_tmp6EE->tag != 6)goto _LL37B;else{_tmp6EF=_tmp6EE->f2;if(_tmp6EF != Cyc_Absyn_Char_sz)
goto _LL37B;}};_LL37A: goto _LL37C;_LL37B: _tmp6F0=_tmp699.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6F1=(struct Cyc_Absyn_IntType_struct*)_tmp6F0;if(_tmp6F1->tag != 6)goto _LL37D;
else{_tmp6F2=_tmp6F1->f2;if(_tmp6F2 != Cyc_Absyn_Short_sz)goto _LL37D;}};_tmp6F3=
_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F4=(struct Cyc_Absyn_IntType_struct*)
_tmp6F3;if(_tmp6F4->tag != 6)goto _LL37D;else{_tmp6F5=_tmp6F4->f2;if(_tmp6F5 != Cyc_Absyn_Char_sz)
goto _LL37D;}};_LL37C: goto _LL37E;_LL37D: _tmp6F6=_tmp699.f1;{struct Cyc_Absyn_TagType_struct*
_tmp6F7=(struct Cyc_Absyn_TagType_struct*)_tmp6F6;if(_tmp6F7->tag != 20)goto _LL37F;};
_tmp6F8=_tmp699.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F9=(struct Cyc_Absyn_IntType_struct*)
_tmp6F8;if(_tmp6F9->tag != 6)goto _LL37F;else{_tmp6FA=_tmp6F9->f2;if(_tmp6FA != Cyc_Absyn_Char_sz)
goto _LL37F;}};_LL37E: return 1;_LL37F:;_LL380: return 0;_LL358:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp6FB=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1206;max_arith_type=((_tmp1206=
_region_malloc(_tmp6FB,sizeof(*_tmp1206)),((_tmp1206->v=t1,_tmp1206))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp120B;void*_tmp120A[2];struct
Cyc_String_pa_struct _tmp1209;struct Cyc_String_pa_struct _tmp1208;(_tmp1208.tag=0,((
_tmp1208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp1209.tag=0,((_tmp1209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp120A[0]=& _tmp1209,((_tmp120A[1]=& _tmp1208,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp120B="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp120B,sizeof(char),41))),_tag_dyneither(_tmp120A,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp701=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL382: {struct Cyc_Absyn_PointerType_struct*
_tmp702=(struct Cyc_Absyn_PointerType_struct*)_tmp701;if(_tmp702->tag != 5)goto
_LL384;}_LL383: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL381;_LL384:;_LL385: return 0;_LL381:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp703=Cyc_Tcutil_compress(t);_LL387: {struct Cyc_Absyn_IntType_struct*
_tmp704=(struct Cyc_Absyn_IntType_struct*)_tmp703;if(_tmp704->tag != 6)goto _LL389;}
_LL388: goto _LL38A;_LL389: {struct Cyc_Absyn_TagType_struct*_tmp705=(struct Cyc_Absyn_TagType_struct*)
_tmp703;if(_tmp705->tag != 20)goto _LL38B;}_LL38A: goto _LL38C;_LL38B: {struct Cyc_Absyn_EnumType_struct*
_tmp706=(struct Cyc_Absyn_EnumType_struct*)_tmp703;if(_tmp706->tag != 14)goto
_LL38D;}_LL38C: goto _LL38E;_LL38D: {struct Cyc_Absyn_AnonEnumType_struct*_tmp707=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp703;if(_tmp707->tag != 15)goto _LL38F;}
_LL38E: return 1;_LL38F:;_LL390: return 0;_LL386:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp120E;void*_tmp120D;(_tmp120D=0,Cyc_Tcutil_warn(e->loc,((_tmp120E="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp120E,sizeof(char),44))),_tag_dyneither(_tmp120D,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp1211;void*_tmp1210;(
_tmp1210=0,Cyc_Tcutil_warn(e->loc,((_tmp1211="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp1211,sizeof(char),44))),_tag_dyneither(_tmp1210,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1212;struct _tuple0 _tmp70D=(_tmp1212.f1=
t1,((_tmp1212.f2=t2,_tmp1212)));void*_tmp70E;struct Cyc_Absyn_PtrInfo _tmp710;void*
_tmp711;struct Cyc_Absyn_PtrInfo _tmp713;void*_tmp714;struct Cyc_Absyn_ArrayInfo
_tmp716;void*_tmp717;struct Cyc_Absyn_Tqual _tmp718;struct Cyc_Absyn_Exp*_tmp719;
union Cyc_Absyn_Constraint*_tmp71A;void*_tmp71B;struct Cyc_Absyn_ArrayInfo _tmp71D;
void*_tmp71E;struct Cyc_Absyn_Tqual _tmp71F;struct Cyc_Absyn_Exp*_tmp720;union Cyc_Absyn_Constraint*
_tmp721;void*_tmp722;void*_tmp724;_LL392: _tmp70E=_tmp70D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp70F=(struct Cyc_Absyn_PointerType_struct*)_tmp70E;if(_tmp70F->tag != 5)goto
_LL394;else{_tmp710=_tmp70F->f1;}};_tmp711=_tmp70D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp712=(struct Cyc_Absyn_PointerType_struct*)_tmp711;if(_tmp712->tag != 5)goto
_LL394;else{_tmp713=_tmp712->f1;}};_LL393: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp710.ptr_atts).nullable,(_tmp713.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp710.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp710.ptr_atts).bounds,(
_tmp713.ptr_atts).bounds)){struct _tuple0 _tmp1213;struct _tuple0 _tmp727=(_tmp1213.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp710.ptr_atts).bounds),((_tmp1213.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp713.ptr_atts).bounds),
_tmp1213)));void*_tmp728;void*_tmp72A;void*_tmp72C;void*_tmp72E;void*_tmp730;
struct Cyc_Absyn_Exp*_tmp732;void*_tmp733;struct Cyc_Absyn_Exp*_tmp735;void*
_tmp736;void*_tmp738;struct Cyc_Absyn_Exp*_tmp73A;_LL39B: _tmp728=_tmp727.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp729=(struct Cyc_Absyn_Upper_b_struct*)_tmp728;
if(_tmp729->tag != 1)goto _LL39D;};_tmp72A=_tmp727.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp72B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp72A;if(_tmp72B->tag != 0)goto
_LL39D;};_LL39C: goto _LL39E;_LL39D: _tmp72C=_tmp727.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp72D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp72C;if(_tmp72D->tag != 0)goto
_LL39F;};_tmp72E=_tmp727.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp72F=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp72E;if(_tmp72F->tag != 0)goto _LL39F;};_LL39E:
okay=1;goto _LL39A;_LL39F: _tmp730=_tmp727.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp731=(struct Cyc_Absyn_Upper_b_struct*)_tmp730;if(_tmp731->tag != 1)goto _LL3A1;
else{_tmp732=_tmp731->f1;}};_tmp733=_tmp727.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp734=(struct Cyc_Absyn_Upper_b_struct*)_tmp733;if(_tmp734->tag != 1)goto _LL3A1;
else{_tmp735=_tmp734->f1;}};_LL3A0: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp735,
_tmp732);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp713.ptr_atts).zero_term)){const char*_tmp1216;void*_tmp1215;(_tmp1215=0,
Cyc_Tcutil_warn(loc,((_tmp1216="implicit cast to shorter array",_tag_dyneither(
_tmp1216,sizeof(char),31))),_tag_dyneither(_tmp1215,sizeof(void*),0)));}goto
_LL39A;_LL3A1: _tmp736=_tmp727.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp737=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp736;if(_tmp737->tag != 0)goto _LL39A;};
_tmp738=_tmp727.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp739=(struct Cyc_Absyn_Upper_b_struct*)
_tmp738;if(_tmp739->tag != 1)goto _LL39A;else{_tmp73A=_tmp739->f1;}};_LL3A2: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp710.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp713.ptr_atts).bounds))goto _LL39A;okay=0;goto
_LL39A;_LL39A:;}okay=okay  && (!(_tmp710.elt_tq).real_const  || (_tmp713.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp710.ptr_atts).rgn,(_tmp713.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp710.ptr_atts).rgn,(_tmp713.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp710.ptr_atts).zero_term,(
_tmp713.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp710.ptr_atts).zero_term) && (_tmp713.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp710.elt_typ,_tmp713.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp710.elt_typ,_tmp713.elt_typ));return okay;}_LL394: _tmp714=_tmp70D.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp715=(struct Cyc_Absyn_ArrayType_struct*)_tmp714;
if(_tmp715->tag != 9)goto _LL396;else{_tmp716=_tmp715->f1;_tmp717=_tmp716.elt_type;
_tmp718=_tmp716.tq;_tmp719=_tmp716.num_elts;_tmp71A=_tmp716.zero_term;}};_tmp71B=
_tmp70D.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp71C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp71B;if(_tmp71C->tag != 9)goto _LL396;else{_tmp71D=_tmp71C->f1;_tmp71E=_tmp71D.elt_type;
_tmp71F=_tmp71D.tq;_tmp720=_tmp71D.num_elts;_tmp721=_tmp71D.zero_term;}};_LL395: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp71A,_tmp721) && ((_tmp719 != 0
 && _tmp720 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp719,(
struct Cyc_Absyn_Exp*)_tmp720));return(okay  && Cyc_Tcutil_unify(_tmp717,_tmp71E))
 && (!_tmp718.real_const  || _tmp71F.real_const);}_LL396: _tmp722=_tmp70D.f1;{
struct Cyc_Absyn_TagType_struct*_tmp723=(struct Cyc_Absyn_TagType_struct*)_tmp722;
if(_tmp723->tag != 20)goto _LL398;};_tmp724=_tmp70D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp725=(struct Cyc_Absyn_IntType_struct*)_tmp724;if(_tmp725->tag != 6)goto _LL398;};
_LL397: return 0;_LL398:;_LL399: return Cyc_Tcutil_unify(t1,t2);_LL391:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp73D=Cyc_Tcutil_compress(t);_LL3A4: {struct Cyc_Absyn_PointerType_struct*
_tmp73E=(struct Cyc_Absyn_PointerType_struct*)_tmp73D;if(_tmp73E->tag != 5)goto
_LL3A6;}_LL3A5: return 1;_LL3A6:;_LL3A7: return 0;_LL3A3:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp73F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp741;void*
_tmp742;_LL3A9: {struct Cyc_Absyn_PointerType_struct*_tmp740=(struct Cyc_Absyn_PointerType_struct*)
_tmp73F;if(_tmp740->tag != 5)goto _LL3AB;else{_tmp741=_tmp740->f1;_tmp742=_tmp741.elt_typ;}}
_LL3AA: return _tmp742;_LL3AB:;_LL3AC: {const char*_tmp1219;void*_tmp1218;(_tmp1218=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1219="pointer_elt_type",_tag_dyneither(_tmp1219,sizeof(char),17))),
_tag_dyneither(_tmp1218,sizeof(void*),0)));}_LL3A8:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp745=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp747;
struct Cyc_Absyn_PtrAtts _tmp748;struct Cyc_Absyn_PtrAtts*_tmp749;_LL3AE: {struct
Cyc_Absyn_PointerType_struct*_tmp746=(struct Cyc_Absyn_PointerType_struct*)
_tmp745;if(_tmp746->tag != 5)goto _LL3B0;else{_tmp747=_tmp746->f1;_tmp748=_tmp747.ptr_atts;
_tmp749=(struct Cyc_Absyn_PtrAtts*)&(_tmp746->f1).ptr_atts;}}_LL3AF: return _tmp749->rgn;
_LL3B0:;_LL3B1: {const char*_tmp121C;void*_tmp121B;(_tmp121B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp121C="pointer_elt_type",
_tag_dyneither(_tmp121C,sizeof(char),17))),_tag_dyneither(_tmp121B,sizeof(void*),
0)));}_LL3AD:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp74C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp74E;struct Cyc_Absyn_PtrAtts _tmp74F;void*_tmp750;
_LL3B3: {struct Cyc_Absyn_PointerType_struct*_tmp74D=(struct Cyc_Absyn_PointerType_struct*)
_tmp74C;if(_tmp74D->tag != 5)goto _LL3B5;else{_tmp74E=_tmp74D->f1;_tmp74F=_tmp74E.ptr_atts;
_tmp750=_tmp74F.rgn;}}_LL3B4:*rgn=_tmp750;return 1;_LL3B5:;_LL3B6: return 0;_LL3B2:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp751=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp753;struct Cyc_Absyn_PtrAtts _tmp754;union Cyc_Absyn_Constraint*
_tmp755;_LL3B8: {struct Cyc_Absyn_PointerType_struct*_tmp752=(struct Cyc_Absyn_PointerType_struct*)
_tmp751;if(_tmp752->tag != 5)goto _LL3BA;else{_tmp753=_tmp752->f1;_tmp754=_tmp753.ptr_atts;
_tmp755=_tmp754.bounds;}}_LL3B9:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp755)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3BA:;_LL3BB: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3B7:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp756=e->r;union Cyc_Absyn_Cnst
_tmp758;struct _tuple7 _tmp759;int _tmp75A;union Cyc_Absyn_Cnst _tmp75C;struct _tuple5
_tmp75D;char _tmp75E;union Cyc_Absyn_Cnst _tmp760;struct _tuple6 _tmp761;short _tmp762;
union Cyc_Absyn_Cnst _tmp764;struct _tuple8 _tmp765;long long _tmp766;void*_tmp768;
struct Cyc_Absyn_Exp*_tmp769;_LL3BD: {struct Cyc_Absyn_Const_e_struct*_tmp757=(
struct Cyc_Absyn_Const_e_struct*)_tmp756;if(_tmp757->tag != 0)goto _LL3BF;else{
_tmp758=_tmp757->f1;if((_tmp758.Int_c).tag != 4)goto _LL3BF;_tmp759=(struct _tuple7)(
_tmp758.Int_c).val;_tmp75A=_tmp759.f2;if(_tmp75A != 0)goto _LL3BF;}}_LL3BE: goto
_LL3C0;_LL3BF: {struct Cyc_Absyn_Const_e_struct*_tmp75B=(struct Cyc_Absyn_Const_e_struct*)
_tmp756;if(_tmp75B->tag != 0)goto _LL3C1;else{_tmp75C=_tmp75B->f1;if((_tmp75C.Char_c).tag
!= 2)goto _LL3C1;_tmp75D=(struct _tuple5)(_tmp75C.Char_c).val;_tmp75E=_tmp75D.f2;
if(_tmp75E != 0)goto _LL3C1;}}_LL3C0: goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Const_e_struct*
_tmp75F=(struct Cyc_Absyn_Const_e_struct*)_tmp756;if(_tmp75F->tag != 0)goto _LL3C3;
else{_tmp760=_tmp75F->f1;if((_tmp760.Short_c).tag != 3)goto _LL3C3;_tmp761=(struct
_tuple6)(_tmp760.Short_c).val;_tmp762=_tmp761.f2;if(_tmp762 != 0)goto _LL3C3;}}
_LL3C2: goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Const_e_struct*_tmp763=(struct Cyc_Absyn_Const_e_struct*)
_tmp756;if(_tmp763->tag != 0)goto _LL3C5;else{_tmp764=_tmp763->f1;if((_tmp764.LongLong_c).tag
!= 5)goto _LL3C5;_tmp765=(struct _tuple8)(_tmp764.LongLong_c).val;_tmp766=_tmp765.f2;
if(_tmp766 != 0)goto _LL3C5;}}_LL3C4: return 1;_LL3C5: {struct Cyc_Absyn_Cast_e_struct*
_tmp767=(struct Cyc_Absyn_Cast_e_struct*)_tmp756;if(_tmp767->tag != 15)goto _LL3C7;
else{_tmp768=(void*)_tmp767->f1;_tmp769=_tmp767->f2;}}_LL3C6: return Cyc_Tcutil_is_zero(
_tmp769) && Cyc_Tcutil_admits_zero(_tmp768);_LL3C7:;_LL3C8: return 0;_LL3BC:;}
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};struct
Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind
Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_mk={
Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,
Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind
Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={
Cyc_Absyn_BoxKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,
Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};struct
Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind
Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};struct Cyc_Core_Opt Cyc_Tcutil_rko={(
void*)& Cyc_Tcutil_rk};struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};struct Cyc_Core_Opt Cyc_Tcutil_mko={(
void*)& Cyc_Tcutil_mk};struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};struct Cyc_Core_Opt Cyc_Tcutil_trko={(
void*)& Cyc_Tcutil_trk};struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};struct Cyc_Core_Opt Cyc_Tcutil_tmko={(
void*)& Cyc_Tcutil_tmk};struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};struct Cyc_Core_Opt Cyc_Tcutil_ubko={(
void*)& Cyc_Tcutil_ubk};struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind
_tmp76B;enum Cyc_Absyn_KindQual _tmp76C;enum Cyc_Absyn_AliasQual _tmp76D;struct Cyc_Absyn_Kind*
_tmp76A=ka;_tmp76B=*_tmp76A;_tmp76C=_tmp76B.kind;_tmp76D=_tmp76B.aliasqual;
switch(_tmp76D){case Cyc_Absyn_Aliasable: _LL3C9: switch(_tmp76C){case Cyc_Absyn_AnyKind:
_LL3CB: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3CC:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3CD: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3CE: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3CF: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3D0: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3CA: switch(_tmp76C){case Cyc_Absyn_AnyKind: _LL3D3: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3D4: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3D5: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3D6: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3D7: break;}break;case Cyc_Absyn_Top: _LL3D2: switch(_tmp76C){case Cyc_Absyn_AnyKind:
_LL3DA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3DB:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3DC: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3DD: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3DE: break;}break;}{const char*_tmp1220;
void*_tmp121F[1];struct Cyc_String_pa_struct _tmp121E;(_tmp121E.tag=0,((_tmp121E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp121F[0]=& _tmp121E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1220="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp1220,sizeof(char),26))),_tag_dyneither(_tmp121F,sizeof(void*),1)))))));};}
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,& Cyc_Tcutil_ak};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_struct bb_v={0,& Cyc_Tcutil_bk};static struct Cyc_Absyn_Eq_kb_struct
rb_v={0,& Cyc_Tcutil_rk};static struct Cyc_Absyn_Eq_kb_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_struct ib_v={0,& Cyc_Tcutil_ik};static void*ab=(void*)&
ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;static void*rb=(void*)&
rb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;static struct Cyc_Absyn_Eq_kb_struct
tab_v={0,& Cyc_Tcutil_tak};static struct Cyc_Absyn_Eq_kb_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_struct tbb_v={0,& Cyc_Tcutil_tbk};static struct Cyc_Absyn_Eq_kb_struct
trb_v={0,& Cyc_Tcutil_trk};static void*tab=(void*)& tab_v;static void*tmb=(void*)&
tmb_v;static void*tbb=(void*)& tbb_v;static void*trb=(void*)& trb_v;static struct Cyc_Absyn_Eq_kb_struct
uab_v={0,& Cyc_Tcutil_uak};static struct Cyc_Absyn_Eq_kb_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_struct ubb_v={0,& Cyc_Tcutil_ubk};static struct Cyc_Absyn_Eq_kb_struct
urb_v={0,& Cyc_Tcutil_urk};static void*uab=(void*)& uab_v;static void*umb=(void*)&
umb_v;static void*ubb=(void*)& ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind
_tmp772;enum Cyc_Absyn_KindQual _tmp773;enum Cyc_Absyn_AliasQual _tmp774;struct Cyc_Absyn_Kind*
_tmp771=ka;_tmp772=*_tmp771;_tmp773=_tmp772.kind;_tmp774=_tmp772.aliasqual;
switch(_tmp774){case Cyc_Absyn_Aliasable: _LL3E0: switch(_tmp773){case Cyc_Absyn_AnyKind:
_LL3E2: return& ab;case Cyc_Absyn_MemKind: _LL3E3: return& mb;case Cyc_Absyn_BoxKind:
_LL3E4: return& bb;case Cyc_Absyn_RgnKind: _LL3E5: return& rb;case Cyc_Absyn_EffKind:
_LL3E6: return& eb;case Cyc_Absyn_IntKind: _LL3E7: return& ib;}case Cyc_Absyn_Unique:
_LL3E1: switch(_tmp773){case Cyc_Absyn_AnyKind: _LL3EA: return& uab;case Cyc_Absyn_MemKind:
_LL3EB: return& umb;case Cyc_Absyn_BoxKind: _LL3EC: return& ubb;case Cyc_Absyn_RgnKind:
_LL3ED: return& urb;default: _LL3EE: break;}break;case Cyc_Absyn_Top: _LL3E9: switch(
_tmp773){case Cyc_Absyn_AnyKind: _LL3F1: return& tab;case Cyc_Absyn_MemKind: _LL3F2:
return& tmb;case Cyc_Absyn_BoxKind: _LL3F3: return& tbb;case Cyc_Absyn_RgnKind: _LL3F4:
return& trb;default: _LL3F5: break;}break;}{const char*_tmp1224;void*_tmp1223[1];
struct Cyc_String_pa_struct _tmp1222;(_tmp1222.tag=0,((_tmp1222.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1223[0]=&
_tmp1222,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1224="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp1224,sizeof(char),24))),
_tag_dyneither(_tmp1223,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int
Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct
_tmp1227;struct Cyc_Absyn_Const_e_struct*_tmp1226;e1->r=(void*)((_tmp1226=
_cycalloc(sizeof(*_tmp1226)),((_tmp1226[0]=((_tmp1227.tag=0,((_tmp1227.f1=Cyc_Absyn_Null_c,
_tmp1227)))),_tmp1226))));}{struct Cyc_Core_Opt*_tmp788=Cyc_Tcenv_lookup_opt_type_vars(
te);struct Cyc_Absyn_PointerType_struct _tmp1231;struct Cyc_Absyn_PtrAtts _tmp1230;
struct Cyc_Absyn_PtrInfo _tmp122F;struct Cyc_Absyn_PointerType_struct*_tmp122E;
struct Cyc_Absyn_PointerType_struct*_tmp789=(_tmp122E=_cycalloc(sizeof(*_tmp122E)),((
_tmp122E[0]=((_tmp1231.tag=5,((_tmp1231.f1=((_tmp122F.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp788),((_tmp122F.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp122F.ptr_atts=((_tmp1230.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,_tmp788),((_tmp1230.nullable=Cyc_Absyn_true_conref,((_tmp1230.bounds=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp1230.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp1230.ptrloc=0,
_tmp1230)))))))))),_tmp122F)))))),_tmp1231)))),_tmp122E)));((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp789);{int bogus=0;int retv=Cyc_Tcutil_coerce_arg(
te,e1,t2,& bogus);if(bogus != 0){const char*_tmp1236;void*_tmp1235[2];struct Cyc_String_pa_struct
_tmp1234;struct Cyc_String_pa_struct _tmp1233;(_tmp1233.tag=0,((_tmp1233.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((
_tmp1234.tag=0,((_tmp1234.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1235[0]=& _tmp1234,((_tmp1235[1]=& _tmp1233,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1236="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp1236,sizeof(char),56))),_tag_dyneither(_tmp1235,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL3F7: {
const char*_tmp1237;return(_tmp1237="unknown",_tag_dyneither(_tmp1237,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL3F8: {const char*_tmp1238;return(_tmp1238="no coercion",
_tag_dyneither(_tmp1238,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL3F9: {
const char*_tmp1239;return(_tmp1239="null check",_tag_dyneither(_tmp1239,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL3FA: {const char*_tmp123A;return(
_tmp123A="other coercion",_tag_dyneither(_tmp123A,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp1247;const char*_tmp1246;void*_tmp1245[1];struct Cyc_Int_pa_struct
_tmp1244;struct _tuple2*_tmp1243;struct _tuple2*v=(_tmp1243=_cycalloc(sizeof(*
_tmp1243)),((_tmp1243->f1=Cyc_Absyn_Loc_n,((_tmp1243->f2=(struct _dyneither_ptr*)((
_tmp1247=_cycalloc(sizeof(*_tmp1247)),((_tmp1247[0]=((_tmp1244.tag=1,((_tmp1244.f1=(
unsigned long)counter ++,((_tmp1245[0]=& _tmp1244,Cyc_aprintf(((_tmp1246="__aliasvar%d",
_tag_dyneither(_tmp1246,sizeof(char),13))),_tag_dyneither(_tmp1245,sizeof(void*),
1)))))))),_tmp1247)))),_tmp1243)))));struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(
v,(void*)& Cyc_Absyn_VoidType_val,0);struct Cyc_Absyn_Local_b_struct _tmp124A;
struct Cyc_Absyn_Local_b_struct*_tmp1249;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(
v,(void*)((_tmp1249=_cycalloc(sizeof(*_tmp1249)),((_tmp1249[0]=((_tmp124A.tag=4,((
_tmp124A.f1=vd,_tmp124A)))),_tmp1249)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((
void*)& rgn_kb);{void*_tmp796=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo
_tmp798;struct Cyc_Absyn_PtrAtts _tmp799;void*_tmp79A;void**_tmp79B;_LL3FD: {
struct Cyc_Absyn_PointerType_struct*_tmp797=(struct Cyc_Absyn_PointerType_struct*)
_tmp796;if(_tmp797->tag != 5)goto _LL3FF;else{_tmp798=_tmp797->f1;_tmp799=_tmp798.ptr_atts;
_tmp79A=_tmp799.rgn;_tmp79B=(void**)&((_tmp797->f1).ptr_atts).rgn;}}_LL3FE:{
struct Cyc_Absyn_VarType_struct _tmp124D;struct Cyc_Absyn_VarType_struct*_tmp124C;*
_tmp79B=(void*)((_tmp124C=_cycalloc(sizeof(*_tmp124C)),((_tmp124C[0]=((_tmp124D.tag=
2,((_tmp124D.f1=tv,_tmp124D)))),_tmp124C))));}goto _LL3FC;_LL3FF:;_LL400: goto
_LL3FC;_LL3FC:;}e->topt=0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,
e->loc);{struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct
_tuple14 _tmp124E;return(_tmp124E.f1=d,((_tmp124E.f2=ve,_tmp124E)));};}static int
Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,
struct Cyc_Position_Segment*loc){struct _RegionHandle _tmp7A7=_new_region("r");
struct _RegionHandle*r=& _tmp7A7;_push_region(r);if((Cyc_Tcutil_is_noalias_path(r,
e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(e_typ)){
void*_tmp7A8=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo _tmp7AA;
struct Cyc_Absyn_PtrAtts _tmp7AB;void*_tmp7AC;_LL402: {struct Cyc_Absyn_PointerType_struct*
_tmp7A9=(struct Cyc_Absyn_PointerType_struct*)_tmp7A8;if(_tmp7A9->tag != 5)goto
_LL404;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7AA.ptr_atts;_tmp7AC=_tmp7AB.rgn;}}
_LL403: {void*_tmp7AE=Cyc_Tcutil_compress(_tmp7AC);_LL409: {struct Cyc_Absyn_HeapRgn_struct*
_tmp7AF=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7AE;if(_tmp7AF->tag != 21)goto _LL40B;}
_LL40A: {int _tmp7B0=0;_npop_handler(0);return _tmp7B0;}_LL40B:;_LL40C: {struct Cyc_Absyn_Kind*
_tmp7B1=Cyc_Tcutil_typ_kind(_tmp7AC);int _tmp7B2=_tmp7B1->kind == Cyc_Absyn_RgnKind
 && _tmp7B1->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7B2;}
_LL408:;}_LL404: {struct Cyc_Absyn_TypedefType_struct*_tmp7AD=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7A8;if(_tmp7AD->tag != 18)goto _LL406;}_LL405: {const char*_tmp1253;void*
_tmp1252[2];struct Cyc_String_pa_struct _tmp1251;struct Cyc_String_pa_struct
_tmp1250;(_tmp1250.tag=0,((_tmp1250.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp1251.tag=0,((_tmp1251.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp1252[0]=& _tmp1251,((_tmp1252[1]=& _tmp1250,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1253="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp1253,sizeof(char),42))),_tag_dyneither(_tmp1252,sizeof(void*),
2)))))))))))));}_LL406:;_LL407: {int _tmp7B7=0;_npop_handler(0);return _tmp7B7;}
_LL401:;}{int _tmp7B8=0;_npop_handler(0);return _tmp7B8;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1258;void*_tmp1257[2];
struct Cyc_String_pa_struct _tmp1256;struct Cyc_String_pa_struct _tmp1255;(_tmp1255.tag=
0,((_tmp1255.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1256.tag=0,((_tmp1256.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1257[0]=& _tmp1256,((_tmp1257[1]=& _tmp1255,Cyc_Tcutil_warn(
e->loc,((_tmp1258="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1258,sizeof(char),53))),_tag_dyneither(_tmp1257,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp125E;void*_tmp125D[3];struct Cyc_String_pa_struct _tmp125C;struct Cyc_String_pa_struct
_tmp125B;struct Cyc_String_pa_struct _tmp125A;(_tmp125A.tag=0,((_tmp125A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp125B.tag=0,((_tmp125B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp125C.tag=0,((_tmp125C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp125D[0]=& _tmp125C,((_tmp125D[1]=& _tmp125B,((
_tmp125D[2]=& _tmp125A,Cyc_Tcutil_warn(e->loc,((_tmp125E="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp125E,sizeof(char),40))),_tag_dyneither(_tmp125D,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp1263;void*_tmp1262[2];
struct Cyc_String_pa_struct _tmp1261;struct Cyc_String_pa_struct _tmp1260;(_tmp1260.tag=
0,((_tmp1260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1261.tag=0,((_tmp1261.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1262[0]=& _tmp1261,((_tmp1262[1]=& _tmp1260,Cyc_Tcutil_warn(
e->loc,((_tmp1263="implicit cast from %s to %s",_tag_dyneither(_tmp1263,sizeof(
char),28))),_tag_dyneither(_tmp1262,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp7C6=Cyc_Tcutil_compress(t);_LL40E: {struct Cyc_Absyn_IntType_struct*
_tmp7C7=(struct Cyc_Absyn_IntType_struct*)_tmp7C6;if(_tmp7C7->tag != 6)goto _LL410;}
_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_FloatType_struct*_tmp7C8=(struct Cyc_Absyn_FloatType_struct*)
_tmp7C6;if(_tmp7C8->tag != 7)goto _LL412;}_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_DoubleType_struct*
_tmp7C9=(struct Cyc_Absyn_DoubleType_struct*)_tmp7C6;if(_tmp7C9->tag != 8)goto
_LL414;}_LL413: return 1;_LL414:;_LL415: return 0;_LL40D:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple16*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp7CB;
struct _RegionHandle*_tmp7CC;struct _tuple16 _tmp7CA=*env;_tmp7CB=_tmp7CA.f1;
_tmp7CC=_tmp7CA.f2;{struct _tuple11*_tmp1264;return(_tmp1264=_region_malloc(
_tmp7CC,sizeof(*_tmp1264)),((_tmp1264->f1=x->tq,((_tmp1264->f2=Cyc_Tcutil_rsubstitute(
_tmp7CC,_tmp7CB,x->type),_tmp1264)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp7CF;void*
_tmp7D0;struct _tuple11 _tmp7CE=*x;_tmp7CF=_tmp7CE.f1;_tmp7D0=_tmp7CE.f2;{struct
_tuple11*_tmp1265;return(_tmp1265=_region_malloc(r,sizeof(*_tmp1265)),((_tmp1265->f1=
_tmp7CF,((_tmp1265->f2=_tmp7D0,_tmp1265)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{
void*_tmp7D2=t1;struct Cyc_List_List*_tmp7D5;struct Cyc_Absyn_AggrInfo _tmp7D7;
union Cyc_Absyn_AggrInfoU _tmp7D8;struct Cyc_Absyn_Aggrdecl**_tmp7D9;struct Cyc_Absyn_Aggrdecl*
_tmp7DA;struct Cyc_List_List*_tmp7DB;enum Cyc_Absyn_AggrKind _tmp7DD;struct Cyc_List_List*
_tmp7DE;_LL417: {struct Cyc_Absyn_VoidType_struct*_tmp7D3=(struct Cyc_Absyn_VoidType_struct*)
_tmp7D2;if(_tmp7D3->tag != 0)goto _LL419;}_LL418: return 0;_LL419: {struct Cyc_Absyn_TupleType_struct*
_tmp7D4=(struct Cyc_Absyn_TupleType_struct*)_tmp7D2;if(_tmp7D4->tag != 11)goto
_LL41B;else{_tmp7D5=_tmp7D4->f1;}}_LL41A: return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _RegionHandle*,struct _tuple11*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,
r,_tmp7D5);_LL41B: {struct Cyc_Absyn_AggrType_struct*_tmp7D6=(struct Cyc_Absyn_AggrType_struct*)
_tmp7D2;if(_tmp7D6->tag != 12)goto _LL41D;else{_tmp7D7=_tmp7D6->f1;_tmp7D8=_tmp7D7.aggr_info;
if((_tmp7D8.KnownAggr).tag != 2)goto _LL41D;_tmp7D9=(struct Cyc_Absyn_Aggrdecl**)(
_tmp7D8.KnownAggr).val;_tmp7DA=*_tmp7D9;_tmp7DB=_tmp7D7.targs;}}_LL41C: if(((
_tmp7DA->kind == Cyc_Absyn_UnionA  || _tmp7DA->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7DA->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp7DA->impl))->rgn_po != 0){struct _tuple11*_tmp1268;struct Cyc_List_List*
_tmp1267;return(_tmp1267=_region_malloc(r,sizeof(*_tmp1267)),((_tmp1267->hd=((
_tmp1268=_region_malloc(r,sizeof(*_tmp1268)),((_tmp1268->f1=Cyc_Absyn_const_tqual(
0),((_tmp1268->f2=t1,_tmp1268)))))),((_tmp1267->tl=0,_tmp1267)))));}{struct Cyc_List_List*
_tmp7E1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp7DA->tvs,
_tmp7DB);struct _tuple16 _tmp1269;struct _tuple16 env=(_tmp1269.f1=_tmp7E1,((
_tmp1269.f2=r,_tmp1269)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple11*(*f)(struct _tuple16*,struct Cyc_Absyn_Aggrfield*),struct _tuple16*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7DA->impl))->fields);};_LL41D: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp7DC=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7D2;if(_tmp7DC->tag != 13)goto _LL41F;else{_tmp7DD=_tmp7DC->f1;if(_tmp7DD != 
Cyc_Absyn_StructA)goto _LL41F;_tmp7DE=_tmp7DC->f2;}}_LL41E: {struct _tuple16
_tmp126A;struct _tuple16 env=(_tmp126A.f1=0,((_tmp126A.f2=r,_tmp126A)));return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple16*,
struct Cyc_Absyn_Aggrfield*),struct _tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r,Cyc_Tcutil_flatten_typ_f,& env,_tmp7DE);}_LL41F:;_LL420: {struct _tuple11*
_tmp126D;struct Cyc_List_List*_tmp126C;return(_tmp126C=_region_malloc(r,sizeof(*
_tmp126C)),((_tmp126C->hd=((_tmp126D=_region_malloc(r,sizeof(*_tmp126D)),((
_tmp126D->f1=Cyc_Absyn_const_tqual(0),((_tmp126D->f2=t1,_tmp126D)))))),((
_tmp126C->tl=0,_tmp126C)))));}_LL416:;};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp7E6=(void*)t->hd;_LL422: {struct Cyc_Absyn_Pure_att_struct*
_tmp7E7=(struct Cyc_Absyn_Pure_att_struct*)_tmp7E6;if(_tmp7E7->tag != 22)goto
_LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Noreturn_att_struct*_tmp7E8=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp7E6;if(_tmp7E8->tag != 4)goto _LL426;}
_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_Initializes_att_struct*_tmp7E9=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp7E6;if(_tmp7E9->tag != 20)goto _LL428;}
_LL427: continue;_LL428:;_LL429: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL421:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{
struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((
struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp126E;struct _tuple0 _tmp7EB=(_tmp126E.f1=t1,((_tmp126E.f2=t2,_tmp126E)));void*
_tmp7EC;struct Cyc_Absyn_PtrInfo _tmp7EE;void*_tmp7EF;struct Cyc_Absyn_Tqual _tmp7F0;
struct Cyc_Absyn_PtrAtts _tmp7F1;void*_tmp7F2;union Cyc_Absyn_Constraint*_tmp7F3;
union Cyc_Absyn_Constraint*_tmp7F4;union Cyc_Absyn_Constraint*_tmp7F5;void*_tmp7F6;
struct Cyc_Absyn_PtrInfo _tmp7F8;void*_tmp7F9;struct Cyc_Absyn_Tqual _tmp7FA;struct
Cyc_Absyn_PtrAtts _tmp7FB;void*_tmp7FC;union Cyc_Absyn_Constraint*_tmp7FD;union Cyc_Absyn_Constraint*
_tmp7FE;union Cyc_Absyn_Constraint*_tmp7FF;void*_tmp800;struct Cyc_Absyn_DatatypeFieldInfo
_tmp802;union Cyc_Absyn_DatatypeFieldInfoU _tmp803;struct _tuple3 _tmp804;struct Cyc_Absyn_Datatypedecl*
_tmp805;struct Cyc_Absyn_Datatypefield*_tmp806;struct Cyc_List_List*_tmp807;void*
_tmp808;struct Cyc_Absyn_DatatypeInfo _tmp80A;union Cyc_Absyn_DatatypeInfoU _tmp80B;
struct Cyc_Absyn_Datatypedecl**_tmp80C;struct Cyc_Absyn_Datatypedecl*_tmp80D;
struct Cyc_List_List*_tmp80E;void*_tmp80F;struct Cyc_Absyn_FnInfo _tmp811;void*
_tmp812;struct Cyc_Absyn_FnInfo _tmp814;_LL42B: _tmp7EC=_tmp7EB.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp7ED=(struct Cyc_Absyn_PointerType_struct*)_tmp7EC;if(_tmp7ED->tag != 5)goto
_LL42D;else{_tmp7EE=_tmp7ED->f1;_tmp7EF=_tmp7EE.elt_typ;_tmp7F0=_tmp7EE.elt_tq;
_tmp7F1=_tmp7EE.ptr_atts;_tmp7F2=_tmp7F1.rgn;_tmp7F3=_tmp7F1.nullable;_tmp7F4=
_tmp7F1.bounds;_tmp7F5=_tmp7F1.zero_term;}};_tmp7F6=_tmp7EB.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp7F7=(struct Cyc_Absyn_PointerType_struct*)_tmp7F6;if(_tmp7F7->tag != 5)goto
_LL42D;else{_tmp7F8=_tmp7F7->f1;_tmp7F9=_tmp7F8.elt_typ;_tmp7FA=_tmp7F8.elt_tq;
_tmp7FB=_tmp7F8.ptr_atts;_tmp7FC=_tmp7FB.rgn;_tmp7FD=_tmp7FB.nullable;_tmp7FE=
_tmp7FB.bounds;_tmp7FF=_tmp7FB.zero_term;}};_LL42C: if(_tmp7F0.real_const  && !
_tmp7FA.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7F3,
_tmp7FD) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7F3)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7FD))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp7F5,
_tmp7FF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7F5)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp7FF))return 0;if(!Cyc_Tcutil_unify(_tmp7F2,_tmp7FC) && !Cyc_Tcenv_region_outlives(
te,_tmp7F2,_tmp7FC))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp7F4,_tmp7FE)){struct _tuple0 _tmp126F;struct _tuple0 _tmp816=(_tmp126F.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7F4),((_tmp126F.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp7FE),_tmp126F)));void*
_tmp817;void*_tmp819;void*_tmp81B;struct Cyc_Absyn_Exp*_tmp81D;void*_tmp81E;
struct Cyc_Absyn_Exp*_tmp820;_LL434: _tmp817=_tmp816.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp818=(struct Cyc_Absyn_Upper_b_struct*)_tmp817;if(_tmp818->tag != 1)goto _LL436;};
_tmp819=_tmp816.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp81A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp819;if(_tmp81A->tag != 0)goto _LL436;};_LL435: goto _LL433;_LL436: _tmp81B=
_tmp816.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp81C=(struct Cyc_Absyn_Upper_b_struct*)
_tmp81B;if(_tmp81C->tag != 1)goto _LL438;else{_tmp81D=_tmp81C->f1;}};_tmp81E=
_tmp816.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp81F=(struct Cyc_Absyn_Upper_b_struct*)
_tmp81E;if(_tmp81F->tag != 1)goto _LL438;else{_tmp820=_tmp81F->f1;}};_LL437: if(!
Cyc_Evexp_lte_const_exp(_tmp820,_tmp81D))return 0;goto _LL433;_LL438:;_LL439:
return 0;_LL433:;}{struct _tuple0*_tmp1272;struct Cyc_List_List*_tmp1271;return Cyc_Tcutil_ptrsubtype(
te,((_tmp1271=_cycalloc(sizeof(*_tmp1271)),((_tmp1271->hd=((_tmp1272=_cycalloc(
sizeof(*_tmp1272)),((_tmp1272->f1=t1,((_tmp1272->f2=t2,_tmp1272)))))),((_tmp1271->tl=
assume,_tmp1271)))))),_tmp7EF,_tmp7F9);};_LL42D: _tmp800=_tmp7EB.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp801=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp800;if(_tmp801->tag != 4)
goto _LL42F;else{_tmp802=_tmp801->f1;_tmp803=_tmp802.field_info;if((_tmp803.KnownDatatypefield).tag
!= 2)goto _LL42F;_tmp804=(struct _tuple3)(_tmp803.KnownDatatypefield).val;_tmp805=
_tmp804.f1;_tmp806=_tmp804.f2;_tmp807=_tmp802.targs;}};_tmp808=_tmp7EB.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp809=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp808;if(_tmp809->tag != 3)goto _LL42F;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp80A.datatype_info;
if((_tmp80B.KnownDatatype).tag != 2)goto _LL42F;_tmp80C=(struct Cyc_Absyn_Datatypedecl**)(
_tmp80B.KnownDatatype).val;_tmp80D=*_tmp80C;_tmp80E=_tmp80A.targs;}};_LL42E: if(
_tmp805 != _tmp80D  && Cyc_Absyn_qvar_cmp(_tmp805->name,_tmp80D->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp807)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp80E))return 0;for(0;_tmp807 != 0;(_tmp807=
_tmp807->tl,_tmp80E=_tmp80E->tl)){if(!Cyc_Tcutil_unify((void*)_tmp807->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp80E))->hd))return 0;}return 1;_LL42F: _tmp80F=
_tmp7EB.f1;{struct Cyc_Absyn_FnType_struct*_tmp810=(struct Cyc_Absyn_FnType_struct*)
_tmp80F;if(_tmp810->tag != 10)goto _LL431;else{_tmp811=_tmp810->f1;}};_tmp812=
_tmp7EB.f2;{struct Cyc_Absyn_FnType_struct*_tmp813=(struct Cyc_Absyn_FnType_struct*)
_tmp812;if(_tmp813->tag != 10)goto _LL431;else{_tmp814=_tmp813->f1;}};_LL430: if(
_tmp811.tvars != 0  || _tmp814.tvars != 0){struct Cyc_List_List*_tmp823=_tmp811.tvars;
struct Cyc_List_List*_tmp824=_tmp814.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp823)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp824))return 0;{
struct _RegionHandle*_tmp825=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp823 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp823->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp824))->hd)->kind))return 0;{struct _tuple15*_tmp127C;struct Cyc_Absyn_VarType_struct
_tmp127B;struct Cyc_Absyn_VarType_struct*_tmp127A;struct Cyc_List_List*_tmp1279;
inst=((_tmp1279=_region_malloc(_tmp825,sizeof(*_tmp1279)),((_tmp1279->hd=((
_tmp127C=_region_malloc(_tmp825,sizeof(*_tmp127C)),((_tmp127C->f1=(struct Cyc_Absyn_Tvar*)
_tmp824->hd,((_tmp127C->f2=(void*)((_tmp127A=_cycalloc(sizeof(*_tmp127A)),((
_tmp127A[0]=((_tmp127B.tag=2,((_tmp127B.f1=(struct Cyc_Absyn_Tvar*)_tmp823->hd,
_tmp127B)))),_tmp127A)))),_tmp127C)))))),((_tmp1279->tl=inst,_tmp1279))))));}
_tmp823=_tmp823->tl;_tmp824=_tmp824->tl;}if(inst != 0){_tmp811.tvars=0;_tmp814.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1282;struct Cyc_Absyn_FnType_struct*_tmp1281;
struct Cyc_Absyn_FnType_struct _tmp127F;struct Cyc_Absyn_FnType_struct*_tmp127E;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp127E=_cycalloc(sizeof(*_tmp127E)),((
_tmp127E[0]=((_tmp127F.tag=10,((_tmp127F.f1=_tmp811,_tmp127F)))),_tmp127E)))),(
void*)((_tmp1281=_cycalloc(sizeof(*_tmp1281)),((_tmp1281[0]=((_tmp1282.tag=10,((
_tmp1282.f1=_tmp814,_tmp1282)))),_tmp1281)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp811.ret_typ,_tmp814.ret_typ))return 0;{struct Cyc_List_List*_tmp82E=
_tmp811.args;struct Cyc_List_List*_tmp82F=_tmp814.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp82E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp82F))return 0;for(0;_tmp82E != 0;(_tmp82E=_tmp82E->tl,_tmp82F=_tmp82F->tl)){
struct Cyc_Absyn_Tqual _tmp831;void*_tmp832;struct _tuple9 _tmp830=*((struct _tuple9*)
_tmp82E->hd);_tmp831=_tmp830.f2;_tmp832=_tmp830.f3;{struct Cyc_Absyn_Tqual _tmp834;
void*_tmp835;struct _tuple9 _tmp833=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp82F))->hd);_tmp834=_tmp833.f2;_tmp835=_tmp833.f3;if(_tmp834.real_const
 && !_tmp831.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp835,_tmp832))return
0;};}if(_tmp811.c_varargs != _tmp814.c_varargs)return 0;if(_tmp811.cyc_varargs != 0
 && _tmp814.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp836=*_tmp811.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp837=*_tmp814.cyc_varargs;if((_tmp837.tq).real_const
 && !(_tmp836.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp837.type,
_tmp836.type))return 0;}else{if(_tmp811.cyc_varargs != 0  || _tmp814.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp811.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp814.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp811.rgn_po,_tmp814.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp811.attributes,_tmp814.attributes))return 0;return 1;};_LL431:;_LL432: return 0;
_LL42A:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp1283;
struct _tuple0 _tmp839=(_tmp1283.f1=Cyc_Tcutil_compress(t1),((_tmp1283.f2=Cyc_Tcutil_compress(
t2),_tmp1283)));void*_tmp83A;enum Cyc_Absyn_Size_of _tmp83C;void*_tmp83D;enum Cyc_Absyn_Size_of
_tmp83F;_LL43B: _tmp83A=_tmp839.f1;{struct Cyc_Absyn_IntType_struct*_tmp83B=(
struct Cyc_Absyn_IntType_struct*)_tmp83A;if(_tmp83B->tag != 6)goto _LL43D;else{
_tmp83C=_tmp83B->f2;}};_tmp83D=_tmp839.f2;{struct Cyc_Absyn_IntType_struct*
_tmp83E=(struct Cyc_Absyn_IntType_struct*)_tmp83D;if(_tmp83E->tag != 6)goto _LL43D;
else{_tmp83F=_tmp83E->f2;}};_LL43C: return(_tmp83C == _tmp83F  || _tmp83C == Cyc_Absyn_Int_sz
 && _tmp83F == Cyc_Absyn_Long_sz) || _tmp83C == Cyc_Absyn_Long_sz  && _tmp83F == Cyc_Absyn_Int_sz;
_LL43D:;_LL43E: return 0;_LL43A:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp840=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp840,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp840,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp842;struct Cyc_Absyn_Tqual
_tmp843;void*_tmp844;struct _tuple11*_tmp841=(struct _tuple11*)tqs1->hd;_tmp842=*
_tmp841;_tmp843=_tmp842.f1;_tmp844=_tmp842.f2;{struct _tuple11 _tmp846;struct Cyc_Absyn_Tqual
_tmp847;void*_tmp848;struct _tuple11*_tmp845=(struct _tuple11*)tqs2->hd;_tmp846=*
_tmp845;_tmp847=_tmp846.f1;_tmp848=_tmp846.f2;if(_tmp847.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp844,_tmp848))continue;else{if(Cyc_Tcutil_unify(_tmp844,_tmp848))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp844,_tmp848))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp849=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp84B;_LL440: {struct Cyc_Absyn_IntType_struct*_tmp84A=(
struct Cyc_Absyn_IntType_struct*)_tmp849;if(_tmp84A->tag != 6)goto _LL442;else{
_tmp84B=_tmp84A->f2;if(_tmp84B != Cyc_Absyn_Char_sz)goto _LL442;}}_LL441: return 1;
_LL442:;_LL443: return 0;_LL43F:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp84C=t2;enum Cyc_Absyn_Size_of _tmp84E;enum Cyc_Absyn_Size_of _tmp850;_LL445: {
struct Cyc_Absyn_IntType_struct*_tmp84D=(struct Cyc_Absyn_IntType_struct*)_tmp84C;
if(_tmp84D->tag != 6)goto _LL447;else{_tmp84E=_tmp84D->f2;if(_tmp84E != Cyc_Absyn_Int_sz)
goto _LL447;}}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_IntType_struct*_tmp84F=(
struct Cyc_Absyn_IntType_struct*)_tmp84C;if(_tmp84F->tag != 6)goto _LL449;else{
_tmp850=_tmp84F->f2;if(_tmp850 != Cyc_Absyn_Long_sz)goto _LL449;}}_LL448: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL444;_LL449:;
_LL44A: goto _LL444;_LL444:;}{void*_tmp851=t1;struct Cyc_Absyn_PtrInfo _tmp853;void*
_tmp854;struct Cyc_Absyn_Tqual _tmp855;struct Cyc_Absyn_PtrAtts _tmp856;void*_tmp857;
union Cyc_Absyn_Constraint*_tmp858;union Cyc_Absyn_Constraint*_tmp859;union Cyc_Absyn_Constraint*
_tmp85A;struct Cyc_Absyn_ArrayInfo _tmp85C;void*_tmp85D;struct Cyc_Absyn_Tqual
_tmp85E;struct Cyc_Absyn_Exp*_tmp85F;union Cyc_Absyn_Constraint*_tmp860;struct Cyc_Absyn_Enumdecl*
_tmp862;void*_tmp867;_LL44C: {struct Cyc_Absyn_PointerType_struct*_tmp852=(struct
Cyc_Absyn_PointerType_struct*)_tmp851;if(_tmp852->tag != 5)goto _LL44E;else{
_tmp853=_tmp852->f1;_tmp854=_tmp853.elt_typ;_tmp855=_tmp853.elt_tq;_tmp856=
_tmp853.ptr_atts;_tmp857=_tmp856.rgn;_tmp858=_tmp856.nullable;_tmp859=_tmp856.bounds;
_tmp85A=_tmp856.zero_term;}}_LL44D:{void*_tmp868=t2;struct Cyc_Absyn_PtrInfo
_tmp86A;void*_tmp86B;struct Cyc_Absyn_Tqual _tmp86C;struct Cyc_Absyn_PtrAtts _tmp86D;
void*_tmp86E;union Cyc_Absyn_Constraint*_tmp86F;union Cyc_Absyn_Constraint*_tmp870;
union Cyc_Absyn_Constraint*_tmp871;_LL45D: {struct Cyc_Absyn_PointerType_struct*
_tmp869=(struct Cyc_Absyn_PointerType_struct*)_tmp868;if(_tmp869->tag != 5)goto
_LL45F;else{_tmp86A=_tmp869->f1;_tmp86B=_tmp86A.elt_typ;_tmp86C=_tmp86A.elt_tq;
_tmp86D=_tmp86A.ptr_atts;_tmp86E=_tmp86D.rgn;_tmp86F=_tmp86D.nullable;_tmp870=
_tmp86D.bounds;_tmp871=_tmp86D.zero_term;}}_LL45E: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1286;struct Cyc_List_List*
_tmp1285;struct Cyc_List_List*_tmp872=(_tmp1285=_cycalloc(sizeof(*_tmp1285)),((
_tmp1285->hd=((_tmp1286=_cycalloc(sizeof(*_tmp1286)),((_tmp1286->f1=t1,((
_tmp1286->f2=t2,_tmp1286)))))),((_tmp1285->tl=0,_tmp1285)))));int _tmp873=Cyc_Tcutil_ptrsubtype(
te,_tmp872,_tmp854,_tmp86B) && (!_tmp855.real_const  || _tmp86C.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp85A,
_tmp871) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp871);
int _tmp874=_tmp873?0:((Cyc_Tcutil_bits_only(_tmp854) && Cyc_Tcutil_is_char_type(
_tmp86B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp871)) && (_tmp86C.real_const  || !_tmp855.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp859,_tmp870);if(!
bounds_ok  && !_tmp874){struct _tuple0 _tmp1287;struct _tuple0 _tmp876=(_tmp1287.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp859),((_tmp1287.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp870),_tmp1287)));
void*_tmp877;struct Cyc_Absyn_Exp*_tmp879;void*_tmp87A;struct Cyc_Absyn_Exp*
_tmp87C;_LL462: _tmp877=_tmp876.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp878=(
struct Cyc_Absyn_Upper_b_struct*)_tmp877;if(_tmp878->tag != 1)goto _LL464;else{
_tmp879=_tmp878->f1;}};_tmp87A=_tmp876.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp87B=(struct Cyc_Absyn_Upper_b_struct*)_tmp87A;if(_tmp87B->tag != 1)goto _LL464;
else{_tmp87C=_tmp87B->f1;}};_LL463: if(Cyc_Evexp_lte_const_exp(_tmp87C,_tmp879))
bounds_ok=1;goto _LL461;_LL464:;_LL465: bounds_ok=1;goto _LL461;_LL461:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp858) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp86F))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp873  || _tmp874)) && (Cyc_Tcutil_unify(
_tmp857,_tmp86E) || Cyc_Tcenv_region_outlives(te,_tmp857,_tmp86E)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL45F:;_LL460: goto _LL45C;
_LL45C:;}return Cyc_Absyn_Unknown_coercion;_LL44E: {struct Cyc_Absyn_ArrayType_struct*
_tmp85B=(struct Cyc_Absyn_ArrayType_struct*)_tmp851;if(_tmp85B->tag != 9)goto
_LL450;else{_tmp85C=_tmp85B->f1;_tmp85D=_tmp85C.elt_type;_tmp85E=_tmp85C.tq;
_tmp85F=_tmp85C.num_elts;_tmp860=_tmp85C.zero_term;}}_LL44F:{void*_tmp87F=t2;
struct Cyc_Absyn_ArrayInfo _tmp881;void*_tmp882;struct Cyc_Absyn_Tqual _tmp883;
struct Cyc_Absyn_Exp*_tmp884;union Cyc_Absyn_Constraint*_tmp885;_LL467: {struct Cyc_Absyn_ArrayType_struct*
_tmp880=(struct Cyc_Absyn_ArrayType_struct*)_tmp87F;if(_tmp880->tag != 9)goto
_LL469;else{_tmp881=_tmp880->f1;_tmp882=_tmp881.elt_type;_tmp883=_tmp881.tq;
_tmp884=_tmp881.num_elts;_tmp885=_tmp881.zero_term;}}_LL468: {int okay;okay=((
_tmp85F != 0  && _tmp884 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp860,
_tmp885)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp884,(struct Cyc_Absyn_Exp*)
_tmp85F);return(okay  && Cyc_Tcutil_unify(_tmp85D,_tmp882)) && (!_tmp85E.real_const
 || _tmp883.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL469:;_LL46A: return Cyc_Absyn_Unknown_coercion;_LL466:;}return Cyc_Absyn_Unknown_coercion;
_LL450: {struct Cyc_Absyn_EnumType_struct*_tmp861=(struct Cyc_Absyn_EnumType_struct*)
_tmp851;if(_tmp861->tag != 14)goto _LL452;else{_tmp862=_tmp861->f2;}}_LL451:{void*
_tmp886=t2;struct Cyc_Absyn_Enumdecl*_tmp888;_LL46C: {struct Cyc_Absyn_EnumType_struct*
_tmp887=(struct Cyc_Absyn_EnumType_struct*)_tmp886;if(_tmp887->tag != 14)goto
_LL46E;else{_tmp888=_tmp887->f2;}}_LL46D: if((_tmp862->fields != 0  && _tmp888->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp862->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp888->fields))->v))return Cyc_Absyn_No_coercion;goto _LL46B;_LL46E:;_LL46F: goto
_LL46B;_LL46B:;}goto _LL453;_LL452: {struct Cyc_Absyn_IntType_struct*_tmp863=(
struct Cyc_Absyn_IntType_struct*)_tmp851;if(_tmp863->tag != 6)goto _LL454;}_LL453:
goto _LL455;_LL454: {struct Cyc_Absyn_FloatType_struct*_tmp864=(struct Cyc_Absyn_FloatType_struct*)
_tmp851;if(_tmp864->tag != 7)goto _LL456;}_LL455: goto _LL457;_LL456: {struct Cyc_Absyn_DoubleType_struct*
_tmp865=(struct Cyc_Absyn_DoubleType_struct*)_tmp851;if(_tmp865->tag != 8)goto
_LL458;}_LL457: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL458: {struct Cyc_Absyn_RgnHandleType_struct*_tmp866=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp851;if(_tmp866->tag != 16)goto _LL45A;else{_tmp867=(void*)_tmp866->f1;}}_LL459:{
void*_tmp889=t2;void*_tmp88B;_LL471: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp88A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp889;if(_tmp88A->tag != 16)goto
_LL473;else{_tmp88B=(void*)_tmp88A->f1;}}_LL472: if(Cyc_Tcenv_region_outlives(te,
_tmp867,_tmp88B))return Cyc_Absyn_No_coercion;goto _LL470;_LL473:;_LL474: goto
_LL470;_LL470:;}return Cyc_Absyn_Unknown_coercion;_LL45A:;_LL45B: return Cyc_Absyn_Unknown_coercion;
_LL44B:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp88C=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp128A;struct Cyc_Absyn_Cast_e_struct*_tmp1289;e->r=(
void*)((_tmp1289=_cycalloc(sizeof(*_tmp1289)),((_tmp1289[0]=((_tmp128A.tag=15,((
_tmp128A.f1=(void*)t,((_tmp128A.f2=_tmp88C,((_tmp128A.f3=0,((_tmp128A.f4=c,
_tmp128A)))))))))),_tmp1289))));}{struct Cyc_Core_Opt*_tmp128B;e->topt=((_tmp128B=
_cycalloc(sizeof(*_tmp128B)),((_tmp128B->v=t,_tmp128B))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp890=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL476: {struct Cyc_Absyn_IntType_struct*_tmp891=(
struct Cyc_Absyn_IntType_struct*)_tmp890;if(_tmp891->tag != 6)goto _LL478;}_LL477:
goto _LL479;_LL478: {struct Cyc_Absyn_EnumType_struct*_tmp892=(struct Cyc_Absyn_EnumType_struct*)
_tmp890;if(_tmp892->tag != 14)goto _LL47A;}_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp893=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp890;if(_tmp893->tag != 15)goto
_LL47C;}_LL47B: goto _LL47D;_LL47C: {struct Cyc_Absyn_TagType_struct*_tmp894=(
struct Cyc_Absyn_TagType_struct*)_tmp890;if(_tmp894->tag != 20)goto _LL47E;}_LL47D:
return 1;_LL47E: {struct Cyc_Absyn_Evar_struct*_tmp895=(struct Cyc_Absyn_Evar_struct*)
_tmp890;if(_tmp895->tag != 1)goto _LL480;}_LL47F: return Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL480:;_LL481:
return 0;_LL475:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp896=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL483: {struct Cyc_Absyn_FloatType_struct*_tmp897=(
struct Cyc_Absyn_FloatType_struct*)_tmp896;if(_tmp897->tag != 7)goto _LL485;}_LL484:
goto _LL486;_LL485: {struct Cyc_Absyn_DoubleType_struct*_tmp898=(struct Cyc_Absyn_DoubleType_struct*)
_tmp896;if(_tmp898->tag != 8)goto _LL487;}_LL486: return 1;_LL487:;_LL488: return 0;
_LL482:;};}int Cyc_Tcutil_is_function_type(void*t){void*_tmp899=Cyc_Tcutil_compress(
t);_LL48A: {struct Cyc_Absyn_FnType_struct*_tmp89A=(struct Cyc_Absyn_FnType_struct*)
_tmp899;if(_tmp89A->tag != 10)goto _LL48C;}_LL48B: return 1;_LL48C:;_LL48D: return 0;
_LL489:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp128C;struct _tuple0 _tmp89C=(_tmp128C.f1=t1,((_tmp128C.f2=t2,_tmp128C)));void*
_tmp89D;int _tmp89F;void*_tmp8A0;int _tmp8A2;void*_tmp8A3;void*_tmp8A5;void*
_tmp8A7;void*_tmp8A9;void*_tmp8AB;enum Cyc_Absyn_Sign _tmp8AD;enum Cyc_Absyn_Size_of
_tmp8AE;void*_tmp8AF;enum Cyc_Absyn_Sign _tmp8B1;enum Cyc_Absyn_Size_of _tmp8B2;
void*_tmp8B3;enum Cyc_Absyn_Size_of _tmp8B5;void*_tmp8B6;enum Cyc_Absyn_Size_of
_tmp8B8;void*_tmp8B9;enum Cyc_Absyn_Sign _tmp8BB;enum Cyc_Absyn_Size_of _tmp8BC;
void*_tmp8BD;enum Cyc_Absyn_Sign _tmp8BF;enum Cyc_Absyn_Size_of _tmp8C0;void*
_tmp8C1;enum Cyc_Absyn_Sign _tmp8C3;enum Cyc_Absyn_Size_of _tmp8C4;void*_tmp8C5;
enum Cyc_Absyn_Sign _tmp8C7;enum Cyc_Absyn_Size_of _tmp8C8;void*_tmp8C9;enum Cyc_Absyn_Size_of
_tmp8CB;void*_tmp8CC;enum Cyc_Absyn_Size_of _tmp8CE;_LL48F: _tmp89D=_tmp89C.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp89E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp89D;if(_tmp89E->tag != 8)goto _LL491;else{_tmp89F=_tmp89E->f1;}};_tmp8A0=
_tmp89C.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp8A1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8A0;if(_tmp8A1->tag != 8)goto _LL491;else{_tmp8A2=_tmp8A1->f1;}};_LL490: if(
_tmp89F)return t1;else{return t2;}_LL491: _tmp8A3=_tmp89C.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp8A4=(struct Cyc_Absyn_DoubleType_struct*)_tmp8A3;if(_tmp8A4->tag != 8)goto
_LL493;};_LL492: return t1;_LL493: _tmp8A5=_tmp89C.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp8A6=(struct Cyc_Absyn_DoubleType_struct*)_tmp8A5;if(_tmp8A6->tag != 8)goto
_LL495;};_LL494: return t2;_LL495: _tmp8A7=_tmp89C.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp8A8=(struct Cyc_Absyn_FloatType_struct*)_tmp8A7;if(_tmp8A8->tag != 7)goto
_LL497;};_LL496: goto _LL498;_LL497: _tmp8A9=_tmp89C.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp8AA=(struct Cyc_Absyn_FloatType_struct*)_tmp8A9;if(_tmp8AA->tag != 7)goto
_LL499;};_LL498: return(void*)& Cyc_Absyn_FloatType_val;_LL499: _tmp8AB=_tmp89C.f1;{
struct Cyc_Absyn_IntType_struct*_tmp8AC=(struct Cyc_Absyn_IntType_struct*)_tmp8AB;
if(_tmp8AC->tag != 6)goto _LL49B;else{_tmp8AD=_tmp8AC->f1;if(_tmp8AD != Cyc_Absyn_Unsigned)
goto _LL49B;_tmp8AE=_tmp8AC->f2;if(_tmp8AE != Cyc_Absyn_LongLong_sz)goto _LL49B;}};
_LL49A: goto _LL49C;_LL49B: _tmp8AF=_tmp89C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8B0=(struct Cyc_Absyn_IntType_struct*)_tmp8AF;if(_tmp8B0->tag != 6)goto _LL49D;
else{_tmp8B1=_tmp8B0->f1;if(_tmp8B1 != Cyc_Absyn_Unsigned)goto _LL49D;_tmp8B2=
_tmp8B0->f2;if(_tmp8B2 != Cyc_Absyn_LongLong_sz)goto _LL49D;}};_LL49C: return Cyc_Absyn_ulonglong_typ;
_LL49D: _tmp8B3=_tmp89C.f1;{struct Cyc_Absyn_IntType_struct*_tmp8B4=(struct Cyc_Absyn_IntType_struct*)
_tmp8B3;if(_tmp8B4->tag != 6)goto _LL49F;else{_tmp8B5=_tmp8B4->f2;if(_tmp8B5 != Cyc_Absyn_LongLong_sz)
goto _LL49F;}};_LL49E: goto _LL4A0;_LL49F: _tmp8B6=_tmp89C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8B7=(struct Cyc_Absyn_IntType_struct*)_tmp8B6;if(_tmp8B7->tag != 6)goto _LL4A1;
else{_tmp8B8=_tmp8B7->f2;if(_tmp8B8 != Cyc_Absyn_LongLong_sz)goto _LL4A1;}};_LL4A0:
return Cyc_Absyn_slonglong_typ;_LL4A1: _tmp8B9=_tmp89C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8BA=(struct Cyc_Absyn_IntType_struct*)_tmp8B9;if(_tmp8BA->tag != 6)goto _LL4A3;
else{_tmp8BB=_tmp8BA->f1;if(_tmp8BB != Cyc_Absyn_Unsigned)goto _LL4A3;_tmp8BC=
_tmp8BA->f2;if(_tmp8BC != Cyc_Absyn_Long_sz)goto _LL4A3;}};_LL4A2: goto _LL4A4;
_LL4A3: _tmp8BD=_tmp89C.f2;{struct Cyc_Absyn_IntType_struct*_tmp8BE=(struct Cyc_Absyn_IntType_struct*)
_tmp8BD;if(_tmp8BE->tag != 6)goto _LL4A5;else{_tmp8BF=_tmp8BE->f1;if(_tmp8BF != Cyc_Absyn_Unsigned)
goto _LL4A5;_tmp8C0=_tmp8BE->f2;if(_tmp8C0 != Cyc_Absyn_Long_sz)goto _LL4A5;}};
_LL4A4: return Cyc_Absyn_ulong_typ;_LL4A5: _tmp8C1=_tmp89C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8C2=(struct Cyc_Absyn_IntType_struct*)_tmp8C1;if(_tmp8C2->tag != 6)goto _LL4A7;
else{_tmp8C3=_tmp8C2->f1;if(_tmp8C3 != Cyc_Absyn_Unsigned)goto _LL4A7;_tmp8C4=
_tmp8C2->f2;if(_tmp8C4 != Cyc_Absyn_Int_sz)goto _LL4A7;}};_LL4A6: goto _LL4A8;_LL4A7:
_tmp8C5=_tmp89C.f2;{struct Cyc_Absyn_IntType_struct*_tmp8C6=(struct Cyc_Absyn_IntType_struct*)
_tmp8C5;if(_tmp8C6->tag != 6)goto _LL4A9;else{_tmp8C7=_tmp8C6->f1;if(_tmp8C7 != Cyc_Absyn_Unsigned)
goto _LL4A9;_tmp8C8=_tmp8C6->f2;if(_tmp8C8 != Cyc_Absyn_Int_sz)goto _LL4A9;}};
_LL4A8: return Cyc_Absyn_uint_typ;_LL4A9: _tmp8C9=_tmp89C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8CA=(struct Cyc_Absyn_IntType_struct*)_tmp8C9;if(_tmp8CA->tag != 6)goto _LL4AB;
else{_tmp8CB=_tmp8CA->f2;if(_tmp8CB != Cyc_Absyn_Long_sz)goto _LL4AB;}};_LL4AA:
goto _LL4AC;_LL4AB: _tmp8CC=_tmp89C.f2;{struct Cyc_Absyn_IntType_struct*_tmp8CD=(
struct Cyc_Absyn_IntType_struct*)_tmp8CC;if(_tmp8CD->tag != 6)goto _LL4AD;else{
_tmp8CE=_tmp8CD->f2;if(_tmp8CE != Cyc_Absyn_Long_sz)goto _LL4AD;}};_LL4AC: return
Cyc_Absyn_slong_typ;_LL4AD:;_LL4AE: return Cyc_Absyn_sint_typ;_LL48E:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp8CF=e->r;struct Cyc_Core_Opt*_tmp8D1;_LL4B0: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp8D0=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp8CF;if(_tmp8D0->tag != 4)goto _LL4B2;else{_tmp8D1=_tmp8D0->f2;if(_tmp8D1 != 0)
goto _LL4B2;}}_LL4B1:{const char*_tmp128F;void*_tmp128E;(_tmp128E=0,Cyc_Tcutil_warn(
e->loc,((_tmp128F="assignment in test",_tag_dyneither(_tmp128F,sizeof(char),19))),
_tag_dyneither(_tmp128E,sizeof(void*),0)));}goto _LL4AF;_LL4B2:;_LL4B3: goto _LL4AF;
_LL4AF:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1290;struct _tuple0 _tmp8D5=(
_tmp1290.f1=c1,((_tmp1290.f2=c2,_tmp1290)));void*_tmp8D6;struct Cyc_Absyn_Kind*
_tmp8D8;void*_tmp8D9;struct Cyc_Absyn_Kind*_tmp8DB;void*_tmp8DC;struct Cyc_Core_Opt*
_tmp8DE;struct Cyc_Core_Opt**_tmp8DF;void*_tmp8E0;struct Cyc_Core_Opt*_tmp8E2;
struct Cyc_Core_Opt**_tmp8E3;void*_tmp8E4;struct Cyc_Core_Opt*_tmp8E6;struct Cyc_Core_Opt**
_tmp8E7;struct Cyc_Absyn_Kind*_tmp8E8;void*_tmp8E9;struct Cyc_Absyn_Kind*_tmp8EB;
void*_tmp8EC;struct Cyc_Absyn_Kind*_tmp8EE;void*_tmp8EF;struct Cyc_Core_Opt*
_tmp8F1;struct Cyc_Core_Opt**_tmp8F2;struct Cyc_Absyn_Kind*_tmp8F3;void*_tmp8F4;
struct Cyc_Core_Opt*_tmp8F6;struct Cyc_Core_Opt**_tmp8F7;struct Cyc_Absyn_Kind*
_tmp8F8;void*_tmp8F9;struct Cyc_Core_Opt*_tmp8FB;struct Cyc_Core_Opt**_tmp8FC;
struct Cyc_Absyn_Kind*_tmp8FD;_LL4B5: _tmp8D6=_tmp8D5.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8D7=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8D6;if(_tmp8D7->tag != 0)goto _LL4B7;
else{_tmp8D8=_tmp8D7->f1;}};_tmp8D9=_tmp8D5.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8DA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8D9;if(_tmp8DA->tag != 0)goto _LL4B7;
else{_tmp8DB=_tmp8DA->f1;}};_LL4B6: return _tmp8D8 == _tmp8DB;_LL4B7: _tmp8DC=
_tmp8D5.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp8DD=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8DC;if(_tmp8DD->tag != 1)goto _LL4B9;else{_tmp8DE=_tmp8DD->f1;_tmp8DF=(struct
Cyc_Core_Opt**)& _tmp8DD->f1;}};_LL4B8:{struct Cyc_Core_Opt*_tmp1291;*_tmp8DF=((
_tmp1291=_cycalloc(sizeof(*_tmp1291)),((_tmp1291->v=c1,_tmp1291))));}return 1;
_LL4B9: _tmp8E0=_tmp8D5.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp8E1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8E0;if(_tmp8E1->tag != 1)goto _LL4BB;else{_tmp8E2=_tmp8E1->f1;_tmp8E3=(struct
Cyc_Core_Opt**)& _tmp8E1->f1;}};_LL4BA:{struct Cyc_Core_Opt*_tmp1292;*_tmp8E3=((
_tmp1292=_cycalloc(sizeof(*_tmp1292)),((_tmp1292->v=c2,_tmp1292))));}return 1;
_LL4BB: _tmp8E4=_tmp8D5.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp8E5=(struct Cyc_Absyn_Less_kb_struct*)
_tmp8E4;if(_tmp8E5->tag != 2)goto _LL4BD;else{_tmp8E6=_tmp8E5->f1;_tmp8E7=(struct
Cyc_Core_Opt**)& _tmp8E5->f1;_tmp8E8=_tmp8E5->f2;}};_tmp8E9=_tmp8D5.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp8EA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8E9;if(_tmp8EA->tag != 0)goto _LL4BD;
else{_tmp8EB=_tmp8EA->f1;}};_LL4BC: if(Cyc_Tcutil_kind_leq(_tmp8EB,_tmp8E8)){{
struct Cyc_Core_Opt*_tmp1293;*_tmp8E7=((_tmp1293=_cycalloc(sizeof(*_tmp1293)),((
_tmp1293->v=c2,_tmp1293))));}return 1;}else{return 0;}_LL4BD: _tmp8EC=_tmp8D5.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp8ED=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8EC;if(
_tmp8ED->tag != 0)goto _LL4BF;else{_tmp8EE=_tmp8ED->f1;}};_tmp8EF=_tmp8D5.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp8F0=(struct Cyc_Absyn_Less_kb_struct*)_tmp8EF;
if(_tmp8F0->tag != 2)goto _LL4BF;else{_tmp8F1=_tmp8F0->f1;_tmp8F2=(struct Cyc_Core_Opt**)&
_tmp8F0->f1;_tmp8F3=_tmp8F0->f2;}};_LL4BE: if(Cyc_Tcutil_kind_leq(_tmp8EE,_tmp8F3)){{
struct Cyc_Core_Opt*_tmp1294;*_tmp8F2=((_tmp1294=_cycalloc(sizeof(*_tmp1294)),((
_tmp1294->v=c1,_tmp1294))));}return 1;}else{return 0;}_LL4BF: _tmp8F4=_tmp8D5.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp8F5=(struct Cyc_Absyn_Less_kb_struct*)_tmp8F4;
if(_tmp8F5->tag != 2)goto _LL4B4;else{_tmp8F6=_tmp8F5->f1;_tmp8F7=(struct Cyc_Core_Opt**)&
_tmp8F5->f1;_tmp8F8=_tmp8F5->f2;}};_tmp8F9=_tmp8D5.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp8FA=(struct Cyc_Absyn_Less_kb_struct*)_tmp8F9;if(_tmp8FA->tag != 2)goto _LL4B4;
else{_tmp8FB=_tmp8FA->f1;_tmp8FC=(struct Cyc_Core_Opt**)& _tmp8FA->f1;_tmp8FD=
_tmp8FA->f2;}};_LL4C0: if(Cyc_Tcutil_kind_leq(_tmp8F8,_tmp8FD)){{struct Cyc_Core_Opt*
_tmp1295;*_tmp8FC=((_tmp1295=_cycalloc(sizeof(*_tmp1295)),((_tmp1295->v=c1,
_tmp1295))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp8FD,_tmp8F8)){{struct Cyc_Core_Opt*
_tmp1296;*_tmp8F7=((_tmp1296=_cycalloc(sizeof(*_tmp1296)),((_tmp1296->v=c2,
_tmp1296))));}return 1;}else{return 0;}}_LL4B4:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp129A;void*_tmp1299[1];struct Cyc_Int_pa_struct _tmp1298;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1298.tag=1,((_tmp1298.f1=(
unsigned long)i,((_tmp1299[0]=& _tmp1298,Cyc_aprintf(((_tmp129A="#%d",
_tag_dyneither(_tmp129A,sizeof(char),4))),_tag_dyneither(_tmp1299,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp129D;struct Cyc_Absyn_Tvar*_tmp129C;return(
_tmp129C=_cycalloc(sizeof(*_tmp129C)),((_tmp129C->name=((_tmp129D=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp129D[0]=s,_tmp129D)))),((_tmp129C->identity=
- 1,((_tmp129C->kind=k,_tmp129C)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp909=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp909,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp12A1;void*_tmp12A0[1];struct Cyc_String_pa_struct
_tmp129F;(_tmp129F.tag=0,((_tmp129F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp12A0[0]=& _tmp129F,Cyc_printf(((_tmp12A1="%s",
_tag_dyneither(_tmp12A1,sizeof(char),3))),_tag_dyneither(_tmp12A0,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp12A2;struct
_dyneither_ptr _tmp90D=Cyc_strconcat(((_tmp12A2="`",_tag_dyneither(_tmp12A2,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp12A5;char _tmp12A4;
struct _dyneither_ptr _tmp12A3;(_tmp12A3=_dyneither_ptr_plus(_tmp90D,sizeof(char),
1),((_tmp12A4=*((char*)_check_dyneither_subscript(_tmp12A3,sizeof(char),0)),((
_tmp12A5='t',((_get_dyneither_size(_tmp12A3,sizeof(char))== 1  && (_tmp12A4 == '\000'
 && _tmp12A5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp12A3.curr)=_tmp12A5)))))));}{
struct _dyneither_ptr*_tmp12A6;t->name=((_tmp12A6=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp12A6[0]=(struct _dyneither_ptr)_tmp90D,_tmp12A6))));};};}
struct _tuple23{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple23*x){struct Cyc_Core_Opt*
_tmp12A9;struct _tuple9*_tmp12A8;return(_tmp12A8=_cycalloc(sizeof(*_tmp12A8)),((
_tmp12A8->f1=(struct Cyc_Core_Opt*)((_tmp12A9=_cycalloc(sizeof(*_tmp12A9)),((
_tmp12A9->v=(*x).f1,_tmp12A9)))),((_tmp12A8->f2=(*x).f2,((_tmp12A8->f3=(*x).f3,
_tmp12A8)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp915=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp12AA;_tmp915=((_tmp12AA=_cycalloc(sizeof(*_tmp12AA)),((
_tmp12AA->hd=(void*)atts->hd,((_tmp12AA->tl=_tmp915,_tmp12AA))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp12B0;struct Cyc_Absyn_FnInfo _tmp12AF;struct Cyc_Absyn_FnType_struct*_tmp12AE;
return(void*)((_tmp12AE=_cycalloc(sizeof(*_tmp12AE)),((_tmp12AE[0]=((_tmp12B0.tag=
10,((_tmp12B0.f1=((_tmp12AF.tvars=fd->tvs,((_tmp12AF.effect=fd->effect,((
_tmp12AF.ret_typ=fd->ret_type,((_tmp12AF.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp12AF.c_varargs=fd->c_varargs,((_tmp12AF.cyc_varargs=fd->cyc_varargs,((
_tmp12AF.rgn_po=fd->rgn_po,((_tmp12AF.attributes=_tmp915,_tmp12AF)))))))))))))))),
_tmp12B0)))),_tmp12AE))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple24{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple24*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){
return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp12B1;return(_tmp12B1=_cycalloc(sizeof(*_tmp12B1)),((_tmp12B1->f1=(*
pr).f1,((_tmp12B1->f2=t,_tmp12B1)))));}struct _tuple25{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};static
struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
struct _tuple25*_tmp12B4;struct _tuple26*_tmp12B3;return(_tmp12B3=_region_malloc(
rgn,sizeof(*_tmp12B3)),((_tmp12B3->f1=((_tmp12B4=_region_malloc(rgn,sizeof(*
_tmp12B4)),((_tmp12B4->f1=(*y).f1,((_tmp12B4->f2=(*y).f2,_tmp12B4)))))),((
_tmp12B3->f2=(*y).f3,_tmp12B3)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple26*w){struct _tuple25*_tmp91E;void*_tmp91F;struct _tuple26 _tmp91D=*w;
_tmp91E=_tmp91D.f1;_tmp91F=_tmp91D.f2;{struct Cyc_Core_Opt*_tmp921;struct Cyc_Absyn_Tqual
_tmp922;struct _tuple25 _tmp920=*_tmp91E;_tmp921=_tmp920.f1;_tmp922=_tmp920.f2;{
struct _tuple9*_tmp12B5;return(_tmp12B5=_cycalloc(sizeof(*_tmp12B5)),((_tmp12B5->f1=
_tmp921,((_tmp12B5->f2=_tmp922,((_tmp12B5->f3=_tmp91F,_tmp12B5)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){return f->type;}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*
f,void*t){struct Cyc_Absyn_Aggrfield*_tmp12B6;return(_tmp12B6=_cycalloc(sizeof(*
_tmp12B6)),((_tmp12B6->name=f->name,((_tmp12B6->tq=f->tq,((_tmp12B6->type=t,((
_tmp12B6->width=f->width,((_tmp12B6->attributes=f->attributes,_tmp12B6)))))))))));}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp12B7;return(_tmp12B7=_cycalloc(
sizeof(*_tmp12B7)),((_tmp12B7->topt=old->topt,((_tmp12B7->r=r,((_tmp12B7->loc=
old->loc,((_tmp12B7->annot=old->annot,_tmp12B7)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp926=e->r;enum Cyc_Absyn_Primop _tmp92C;struct Cyc_List_List*_tmp92D;
struct Cyc_Absyn_Exp*_tmp92F;struct Cyc_Absyn_Exp*_tmp930;struct Cyc_Absyn_Exp*
_tmp931;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Exp*_tmp934;struct Cyc_Absyn_Exp*
_tmp936;struct Cyc_Absyn_Exp*_tmp937;struct Cyc_Absyn_Exp*_tmp939;struct Cyc_Absyn_Exp*
_tmp93A;void*_tmp93C;struct Cyc_Absyn_Exp*_tmp93D;int _tmp93E;enum Cyc_Absyn_Coercion
_tmp93F;void*_tmp941;struct Cyc_Absyn_Exp*_tmp943;void*_tmp945;void*_tmp946;void*
_tmp948;_LL4C2: {struct Cyc_Absyn_Const_e_struct*_tmp927=(struct Cyc_Absyn_Const_e_struct*)
_tmp926;if(_tmp927->tag != 0)goto _LL4C4;}_LL4C3: goto _LL4C5;_LL4C4: {struct Cyc_Absyn_Enum_e_struct*
_tmp928=(struct Cyc_Absyn_Enum_e_struct*)_tmp926;if(_tmp928->tag != 32)goto _LL4C6;}
_LL4C5: goto _LL4C7;_LL4C6: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp929=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp926;if(_tmp929->tag != 33)goto _LL4C8;}_LL4C7: goto _LL4C9;_LL4C8: {struct Cyc_Absyn_Var_e_struct*
_tmp92A=(struct Cyc_Absyn_Var_e_struct*)_tmp926;if(_tmp92A->tag != 1)goto _LL4CA;}
_LL4C9: return e;_LL4CA: {struct Cyc_Absyn_Primop_e_struct*_tmp92B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp926;if(_tmp92B->tag != 3)goto _LL4CC;else{_tmp92C=_tmp92B->f1;_tmp92D=_tmp92B->f2;}}
_LL4CB: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp92D)== 1){struct
Cyc_Absyn_Exp*_tmp949=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp92D))->hd;struct Cyc_Absyn_Exp*_tmp94A=Cyc_Tcutil_rsubsexp(r,inst,_tmp949);
if(_tmp94A == _tmp949)return e;{struct Cyc_Absyn_Primop_e_struct _tmp12BD;struct Cyc_Absyn_Exp*
_tmp12BC[1];struct Cyc_Absyn_Primop_e_struct*_tmp12BB;return Cyc_Tcutil_copye(e,(
void*)((_tmp12BB=_cycalloc(sizeof(*_tmp12BB)),((_tmp12BB[0]=((_tmp12BD.tag=3,((
_tmp12BD.f1=_tmp92C,((_tmp12BD.f2=((_tmp12BC[0]=_tmp94A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp12BC,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp12BD)))))),_tmp12BB)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp92D)== 2){struct Cyc_Absyn_Exp*_tmp94E=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp92D))->hd;struct Cyc_Absyn_Exp*_tmp94F=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp92D->tl))->hd;struct
Cyc_Absyn_Exp*_tmp950=Cyc_Tcutil_rsubsexp(r,inst,_tmp94E);struct Cyc_Absyn_Exp*
_tmp951=Cyc_Tcutil_rsubsexp(r,inst,_tmp94F);if(_tmp950 == _tmp94E  && _tmp951 == 
_tmp94F)return e;{struct Cyc_Absyn_Primop_e_struct _tmp12C3;struct Cyc_Absyn_Exp*
_tmp12C2[2];struct Cyc_Absyn_Primop_e_struct*_tmp12C1;return Cyc_Tcutil_copye(e,(
void*)((_tmp12C1=_cycalloc(sizeof(*_tmp12C1)),((_tmp12C1[0]=((_tmp12C3.tag=3,((
_tmp12C3.f1=_tmp92C,((_tmp12C3.f2=((_tmp12C2[1]=_tmp951,((_tmp12C2[0]=_tmp950,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp12C2,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp12C3)))))),_tmp12C1)))));};}
else{const char*_tmp12C6;void*_tmp12C5;return(_tmp12C5=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C6="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp12C6,sizeof(char),34))),_tag_dyneither(_tmp12C5,sizeof(void*),
0)));}}_LL4CC: {struct Cyc_Absyn_Conditional_e_struct*_tmp92E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp926;if(_tmp92E->tag != 6)goto _LL4CE;else{_tmp92F=_tmp92E->f1;_tmp930=_tmp92E->f2;
_tmp931=_tmp92E->f3;}}_LL4CD: {struct Cyc_Absyn_Exp*_tmp957=Cyc_Tcutil_rsubsexp(r,
inst,_tmp92F);struct Cyc_Absyn_Exp*_tmp958=Cyc_Tcutil_rsubsexp(r,inst,_tmp930);
struct Cyc_Absyn_Exp*_tmp959=Cyc_Tcutil_rsubsexp(r,inst,_tmp931);if((_tmp957 == 
_tmp92F  && _tmp958 == _tmp930) && _tmp959 == _tmp931)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp12C9;struct Cyc_Absyn_Conditional_e_struct*_tmp12C8;return Cyc_Tcutil_copye(e,(
void*)((_tmp12C8=_cycalloc(sizeof(*_tmp12C8)),((_tmp12C8[0]=((_tmp12C9.tag=6,((
_tmp12C9.f1=_tmp957,((_tmp12C9.f2=_tmp958,((_tmp12C9.f3=_tmp959,_tmp12C9)))))))),
_tmp12C8)))));};}_LL4CE: {struct Cyc_Absyn_And_e_struct*_tmp932=(struct Cyc_Absyn_And_e_struct*)
_tmp926;if(_tmp932->tag != 7)goto _LL4D0;else{_tmp933=_tmp932->f1;_tmp934=_tmp932->f2;}}
_LL4CF: {struct Cyc_Absyn_Exp*_tmp95C=Cyc_Tcutil_rsubsexp(r,inst,_tmp933);struct
Cyc_Absyn_Exp*_tmp95D=Cyc_Tcutil_rsubsexp(r,inst,_tmp934);if(_tmp95C == _tmp933
 && _tmp95D == _tmp934)return e;{struct Cyc_Absyn_And_e_struct _tmp12CC;struct Cyc_Absyn_And_e_struct*
_tmp12CB;return Cyc_Tcutil_copye(e,(void*)((_tmp12CB=_cycalloc(sizeof(*_tmp12CB)),((
_tmp12CB[0]=((_tmp12CC.tag=7,((_tmp12CC.f1=_tmp95C,((_tmp12CC.f2=_tmp95D,
_tmp12CC)))))),_tmp12CB)))));};}_LL4D0: {struct Cyc_Absyn_Or_e_struct*_tmp935=(
struct Cyc_Absyn_Or_e_struct*)_tmp926;if(_tmp935->tag != 8)goto _LL4D2;else{_tmp936=
_tmp935->f1;_tmp937=_tmp935->f2;}}_LL4D1: {struct Cyc_Absyn_Exp*_tmp960=Cyc_Tcutil_rsubsexp(
r,inst,_tmp936);struct Cyc_Absyn_Exp*_tmp961=Cyc_Tcutil_rsubsexp(r,inst,_tmp937);
if(_tmp960 == _tmp936  && _tmp961 == _tmp937)return e;{struct Cyc_Absyn_Or_e_struct
_tmp12CF;struct Cyc_Absyn_Or_e_struct*_tmp12CE;return Cyc_Tcutil_copye(e,(void*)((
_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((_tmp12CE[0]=((_tmp12CF.tag=8,((_tmp12CF.f1=
_tmp960,((_tmp12CF.f2=_tmp961,_tmp12CF)))))),_tmp12CE)))));};}_LL4D2: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp938=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp926;if(_tmp938->tag != 9)goto _LL4D4;
else{_tmp939=_tmp938->f1;_tmp93A=_tmp938->f2;}}_LL4D3: {struct Cyc_Absyn_Exp*
_tmp964=Cyc_Tcutil_rsubsexp(r,inst,_tmp939);struct Cyc_Absyn_Exp*_tmp965=Cyc_Tcutil_rsubsexp(
r,inst,_tmp93A);if(_tmp964 == _tmp939  && _tmp965 == _tmp93A)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp12D2;struct Cyc_Absyn_SeqExp_e_struct*_tmp12D1;return Cyc_Tcutil_copye(e,(void*)((
_tmp12D1=_cycalloc(sizeof(*_tmp12D1)),((_tmp12D1[0]=((_tmp12D2.tag=9,((_tmp12D2.f1=
_tmp964,((_tmp12D2.f2=_tmp965,_tmp12D2)))))),_tmp12D1)))));};}_LL4D4: {struct Cyc_Absyn_Cast_e_struct*
_tmp93B=(struct Cyc_Absyn_Cast_e_struct*)_tmp926;if(_tmp93B->tag != 15)goto _LL4D6;
else{_tmp93C=(void*)_tmp93B->f1;_tmp93D=_tmp93B->f2;_tmp93E=_tmp93B->f3;_tmp93F=
_tmp93B->f4;}}_LL4D5: {struct Cyc_Absyn_Exp*_tmp968=Cyc_Tcutil_rsubsexp(r,inst,
_tmp93D);void*_tmp969=Cyc_Tcutil_rsubstitute(r,inst,_tmp93C);if(_tmp968 == 
_tmp93D  && _tmp969 == _tmp93C)return e;{struct Cyc_Absyn_Cast_e_struct _tmp12D5;
struct Cyc_Absyn_Cast_e_struct*_tmp12D4;return Cyc_Tcutil_copye(e,(void*)((
_tmp12D4=_cycalloc(sizeof(*_tmp12D4)),((_tmp12D4[0]=((_tmp12D5.tag=15,((_tmp12D5.f1=(
void*)_tmp969,((_tmp12D5.f2=_tmp968,((_tmp12D5.f3=_tmp93E,((_tmp12D5.f4=_tmp93F,
_tmp12D5)))))))))),_tmp12D4)))));};}_LL4D6: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp940=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp926;if(_tmp940->tag != 18)goto
_LL4D8;else{_tmp941=(void*)_tmp940->f1;}}_LL4D7: {void*_tmp96C=Cyc_Tcutil_rsubstitute(
r,inst,_tmp941);if(_tmp96C == _tmp941)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp12D8;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp12D7;return Cyc_Tcutil_copye(e,(
void*)((_tmp12D7=_cycalloc(sizeof(*_tmp12D7)),((_tmp12D7[0]=((_tmp12D8.tag=18,((
_tmp12D8.f1=(void*)_tmp96C,_tmp12D8)))),_tmp12D7)))));};}_LL4D8: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp942=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp926;if(_tmp942->tag != 19)goto
_LL4DA;else{_tmp943=_tmp942->f1;}}_LL4D9: {struct Cyc_Absyn_Exp*_tmp96F=Cyc_Tcutil_rsubsexp(
r,inst,_tmp943);if(_tmp96F == _tmp943)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp12DB;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp12DA;return Cyc_Tcutil_copye(e,(
void*)((_tmp12DA=_cycalloc(sizeof(*_tmp12DA)),((_tmp12DA[0]=((_tmp12DB.tag=19,((
_tmp12DB.f1=_tmp96F,_tmp12DB)))),_tmp12DA)))));};}_LL4DA: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp944=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp926;if(_tmp944->tag != 20)goto
_LL4DC;else{_tmp945=(void*)_tmp944->f1;_tmp946=(void*)_tmp944->f2;}}_LL4DB: {
void*_tmp972=Cyc_Tcutil_rsubstitute(r,inst,_tmp945);if(_tmp972 == _tmp945)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp12DE;struct Cyc_Absyn_Offsetof_e_struct*
_tmp12DD;return Cyc_Tcutil_copye(e,(void*)((_tmp12DD=_cycalloc(sizeof(*_tmp12DD)),((
_tmp12DD[0]=((_tmp12DE.tag=20,((_tmp12DE.f1=(void*)_tmp972,((_tmp12DE.f2=(void*)
_tmp946,_tmp12DE)))))),_tmp12DD)))));};}_LL4DC: {struct Cyc_Absyn_Valueof_e_struct*
_tmp947=(struct Cyc_Absyn_Valueof_e_struct*)_tmp926;if(_tmp947->tag != 39)goto
_LL4DE;else{_tmp948=(void*)_tmp947->f1;}}_LL4DD: {void*_tmp975=Cyc_Tcutil_rsubstitute(
r,inst,_tmp948);if(_tmp975 == _tmp948)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp12E1;struct Cyc_Absyn_Valueof_e_struct*_tmp12E0;return Cyc_Tcutil_copye(e,(
void*)((_tmp12E0=_cycalloc(sizeof(*_tmp12E0)),((_tmp12E0[0]=((_tmp12E1.tag=39,((
_tmp12E1.f1=(void*)_tmp975,_tmp12E1)))),_tmp12E0)))));};}_LL4DE:;_LL4DF: {const
char*_tmp12E4;void*_tmp12E3;return(_tmp12E3=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E4="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp12E4,sizeof(char),46))),_tag_dyneither(_tmp12E3,sizeof(void*),
0)));}_LL4C1:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp97A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp97C;
struct Cyc_Absyn_AggrInfo _tmp97E;union Cyc_Absyn_AggrInfoU _tmp97F;struct Cyc_List_List*
_tmp980;struct Cyc_Absyn_DatatypeInfo _tmp982;union Cyc_Absyn_DatatypeInfoU _tmp983;
struct Cyc_List_List*_tmp984;struct Cyc_Absyn_DatatypeFieldInfo _tmp986;union Cyc_Absyn_DatatypeFieldInfoU
_tmp987;struct Cyc_List_List*_tmp988;struct _tuple2*_tmp98A;struct Cyc_List_List*
_tmp98B;struct Cyc_Absyn_Typedefdecl*_tmp98C;void**_tmp98D;struct Cyc_Absyn_ArrayInfo
_tmp98F;void*_tmp990;struct Cyc_Absyn_Tqual _tmp991;struct Cyc_Absyn_Exp*_tmp992;
union Cyc_Absyn_Constraint*_tmp993;struct Cyc_Position_Segment*_tmp994;struct Cyc_Absyn_PtrInfo
_tmp996;void*_tmp997;struct Cyc_Absyn_Tqual _tmp998;struct Cyc_Absyn_PtrAtts _tmp999;
void*_tmp99A;union Cyc_Absyn_Constraint*_tmp99B;union Cyc_Absyn_Constraint*_tmp99C;
union Cyc_Absyn_Constraint*_tmp99D;struct Cyc_Absyn_FnInfo _tmp99F;struct Cyc_List_List*
_tmp9A0;struct Cyc_Core_Opt*_tmp9A1;void*_tmp9A2;struct Cyc_List_List*_tmp9A3;int
_tmp9A4;struct Cyc_Absyn_VarargInfo*_tmp9A5;struct Cyc_List_List*_tmp9A6;struct Cyc_List_List*
_tmp9A7;struct Cyc_List_List*_tmp9A9;enum Cyc_Absyn_AggrKind _tmp9AB;struct Cyc_List_List*
_tmp9AC;struct Cyc_Core_Opt*_tmp9AE;void*_tmp9B0;void*_tmp9B2;void*_tmp9B3;void*
_tmp9B5;struct Cyc_Absyn_Exp*_tmp9B7;void*_tmp9C1;void*_tmp9C3;struct Cyc_List_List*
_tmp9C5;_LL4E1: {struct Cyc_Absyn_VarType_struct*_tmp97B=(struct Cyc_Absyn_VarType_struct*)
_tmp97A;if(_tmp97B->tag != 2)goto _LL4E3;else{_tmp97C=_tmp97B->f1;}}_LL4E2: {
struct _handler_cons _tmp9C6;_push_handler(& _tmp9C6);{int _tmp9C8=0;if(setjmp(
_tmp9C6.handler))_tmp9C8=1;if(!_tmp9C8){{void*_tmp9C9=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp97C);_npop_handler(0);return
_tmp9C9;};_pop_handler();}else{void*_tmp9C7=(void*)_exn_thrown;void*_tmp9CB=
_tmp9C7;_LL516: {struct Cyc_Core_Not_found_struct*_tmp9CC=(struct Cyc_Core_Not_found_struct*)
_tmp9CB;if(_tmp9CC->tag != Cyc_Core_Not_found)goto _LL518;}_LL517: return t;_LL518:;
_LL519:(void)_throw(_tmp9CB);_LL515:;}};}_LL4E3: {struct Cyc_Absyn_AggrType_struct*
_tmp97D=(struct Cyc_Absyn_AggrType_struct*)_tmp97A;if(_tmp97D->tag != 12)goto
_LL4E5;else{_tmp97E=_tmp97D->f1;_tmp97F=_tmp97E.aggr_info;_tmp980=_tmp97E.targs;}}
_LL4E4: {struct Cyc_List_List*_tmp9CD=Cyc_Tcutil_substs(rgn,inst,_tmp980);struct
Cyc_Absyn_AggrType_struct _tmp12EA;struct Cyc_Absyn_AggrInfo _tmp12E9;struct Cyc_Absyn_AggrType_struct*
_tmp12E8;return _tmp9CD == _tmp980?t:(void*)((_tmp12E8=_cycalloc(sizeof(*_tmp12E8)),((
_tmp12E8[0]=((_tmp12EA.tag=12,((_tmp12EA.f1=((_tmp12E9.aggr_info=_tmp97F,((
_tmp12E9.targs=_tmp9CD,_tmp12E9)))),_tmp12EA)))),_tmp12E8))));}_LL4E5: {struct
Cyc_Absyn_DatatypeType_struct*_tmp981=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp97A;if(_tmp981->tag != 3)goto _LL4E7;else{_tmp982=_tmp981->f1;_tmp983=_tmp982.datatype_info;
_tmp984=_tmp982.targs;}}_LL4E6: {struct Cyc_List_List*_tmp9D1=Cyc_Tcutil_substs(
rgn,inst,_tmp984);struct Cyc_Absyn_DatatypeType_struct _tmp12F0;struct Cyc_Absyn_DatatypeInfo
_tmp12EF;struct Cyc_Absyn_DatatypeType_struct*_tmp12EE;return _tmp9D1 == _tmp984?t:(
void*)((_tmp12EE=_cycalloc(sizeof(*_tmp12EE)),((_tmp12EE[0]=((_tmp12F0.tag=3,((
_tmp12F0.f1=((_tmp12EF.datatype_info=_tmp983,((_tmp12EF.targs=_tmp9D1,_tmp12EF)))),
_tmp12F0)))),_tmp12EE))));}_LL4E7: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp985=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp97A;if(_tmp985->tag != 4)
goto _LL4E9;else{_tmp986=_tmp985->f1;_tmp987=_tmp986.field_info;_tmp988=_tmp986.targs;}}
_LL4E8: {struct Cyc_List_List*_tmp9D5=Cyc_Tcutil_substs(rgn,inst,_tmp988);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp12F6;struct Cyc_Absyn_DatatypeFieldInfo
_tmp12F5;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp12F4;return _tmp9D5 == 
_tmp988?t:(void*)((_tmp12F4=_cycalloc(sizeof(*_tmp12F4)),((_tmp12F4[0]=((
_tmp12F6.tag=4,((_tmp12F6.f1=((_tmp12F5.field_info=_tmp987,((_tmp12F5.targs=
_tmp9D5,_tmp12F5)))),_tmp12F6)))),_tmp12F4))));}_LL4E9: {struct Cyc_Absyn_TypedefType_struct*
_tmp989=(struct Cyc_Absyn_TypedefType_struct*)_tmp97A;if(_tmp989->tag != 18)goto
_LL4EB;else{_tmp98A=_tmp989->f1;_tmp98B=_tmp989->f2;_tmp98C=_tmp989->f3;_tmp98D=
_tmp989->f4;}}_LL4EA: {struct Cyc_List_List*_tmp9D9=Cyc_Tcutil_substs(rgn,inst,
_tmp98B);struct Cyc_Absyn_TypedefType_struct _tmp12F9;struct Cyc_Absyn_TypedefType_struct*
_tmp12F8;return _tmp9D9 == _tmp98B?t:(void*)((_tmp12F8=_cycalloc(sizeof(*_tmp12F8)),((
_tmp12F8[0]=((_tmp12F9.tag=18,((_tmp12F9.f1=_tmp98A,((_tmp12F9.f2=_tmp9D9,((
_tmp12F9.f3=_tmp98C,((_tmp12F9.f4=_tmp98D,_tmp12F9)))))))))),_tmp12F8))));}
_LL4EB: {struct Cyc_Absyn_ArrayType_struct*_tmp98E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp97A;if(_tmp98E->tag != 9)goto _LL4ED;else{_tmp98F=_tmp98E->f1;_tmp990=_tmp98F.elt_type;
_tmp991=_tmp98F.tq;_tmp992=_tmp98F.num_elts;_tmp993=_tmp98F.zero_term;_tmp994=
_tmp98F.zt_loc;}}_LL4EC: {void*_tmp9DC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp990);
struct Cyc_Absyn_ArrayType_struct _tmp12FF;struct Cyc_Absyn_ArrayInfo _tmp12FE;
struct Cyc_Absyn_ArrayType_struct*_tmp12FD;return _tmp9DC == _tmp990?t:(void*)((
_tmp12FD=_cycalloc(sizeof(*_tmp12FD)),((_tmp12FD[0]=((_tmp12FF.tag=9,((_tmp12FF.f1=((
_tmp12FE.elt_type=_tmp9DC,((_tmp12FE.tq=_tmp991,((_tmp12FE.num_elts=_tmp992,((
_tmp12FE.zero_term=_tmp993,((_tmp12FE.zt_loc=_tmp994,_tmp12FE)))))))))),_tmp12FF)))),
_tmp12FD))));}_LL4ED: {struct Cyc_Absyn_PointerType_struct*_tmp995=(struct Cyc_Absyn_PointerType_struct*)
_tmp97A;if(_tmp995->tag != 5)goto _LL4EF;else{_tmp996=_tmp995->f1;_tmp997=_tmp996.elt_typ;
_tmp998=_tmp996.elt_tq;_tmp999=_tmp996.ptr_atts;_tmp99A=_tmp999.rgn;_tmp99B=
_tmp999.nullable;_tmp99C=_tmp999.bounds;_tmp99D=_tmp999.zero_term;}}_LL4EE: {
void*_tmp9E0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp997);void*_tmp9E1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp99A);union Cyc_Absyn_Constraint*_tmp9E2=_tmp99C;{void*_tmp9E3=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp99C);struct Cyc_Absyn_Exp*_tmp9E5;_LL51B: {struct Cyc_Absyn_Upper_b_struct*
_tmp9E4=(struct Cyc_Absyn_Upper_b_struct*)_tmp9E3;if(_tmp9E4->tag != 1)goto _LL51D;
else{_tmp9E5=_tmp9E4->f1;}}_LL51C: {struct Cyc_Absyn_Exp*_tmp9E6=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp9E5);if(_tmp9E6 != _tmp9E5){struct Cyc_Absyn_Upper_b_struct _tmp1302;
struct Cyc_Absyn_Upper_b_struct*_tmp1301;_tmp9E2=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1301=_cycalloc(sizeof(*_tmp1301)),((
_tmp1301[0]=((_tmp1302.tag=1,((_tmp1302.f1=_tmp9E6,_tmp1302)))),_tmp1301)))));}
goto _LL51A;}_LL51D:;_LL51E: goto _LL51A;_LL51A:;}if((_tmp9E0 == _tmp997  && _tmp9E1
== _tmp99A) && _tmp9E2 == _tmp99C)return t;{struct Cyc_Absyn_PointerType_struct
_tmp130C;struct Cyc_Absyn_PtrAtts _tmp130B;struct Cyc_Absyn_PtrInfo _tmp130A;struct
Cyc_Absyn_PointerType_struct*_tmp1309;return(void*)((_tmp1309=_cycalloc(sizeof(*
_tmp1309)),((_tmp1309[0]=((_tmp130C.tag=5,((_tmp130C.f1=((_tmp130A.elt_typ=
_tmp9E0,((_tmp130A.elt_tq=_tmp998,((_tmp130A.ptr_atts=((_tmp130B.rgn=_tmp9E1,((
_tmp130B.nullable=_tmp99B,((_tmp130B.bounds=_tmp9E2,((_tmp130B.zero_term=_tmp99D,((
_tmp130B.ptrloc=0,_tmp130B)))))))))),_tmp130A)))))),_tmp130C)))),_tmp1309))));};}
_LL4EF: {struct Cyc_Absyn_FnType_struct*_tmp99E=(struct Cyc_Absyn_FnType_struct*)
_tmp97A;if(_tmp99E->tag != 10)goto _LL4F1;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99F.tvars;
_tmp9A1=_tmp99F.effect;_tmp9A2=_tmp99F.ret_typ;_tmp9A3=_tmp99F.args;_tmp9A4=
_tmp99F.c_varargs;_tmp9A5=_tmp99F.cyc_varargs;_tmp9A6=_tmp99F.rgn_po;_tmp9A7=
_tmp99F.attributes;}}_LL4F0:{struct Cyc_List_List*_tmp9ED=_tmp9A0;for(0;_tmp9ED != 
0;_tmp9ED=_tmp9ED->tl){struct _tuple15*_tmp1316;struct Cyc_Absyn_VarType_struct
_tmp1315;struct Cyc_Absyn_VarType_struct*_tmp1314;struct Cyc_List_List*_tmp1313;
inst=((_tmp1313=_region_malloc(rgn,sizeof(*_tmp1313)),((_tmp1313->hd=((_tmp1316=
_region_malloc(rgn,sizeof(*_tmp1316)),((_tmp1316->f1=(struct Cyc_Absyn_Tvar*)
_tmp9ED->hd,((_tmp1316->f2=(void*)((_tmp1314=_cycalloc(sizeof(*_tmp1314)),((
_tmp1314[0]=((_tmp1315.tag=2,((_tmp1315.f1=(struct Cyc_Absyn_Tvar*)_tmp9ED->hd,
_tmp1315)))),_tmp1314)))),_tmp1316)))))),((_tmp1313->tl=inst,_tmp1313))))));}}{
struct Cyc_List_List*_tmp9F3;struct Cyc_List_List*_tmp9F4;struct _tuple1 _tmp9F2=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp9A3));
_tmp9F3=_tmp9F2.f1;_tmp9F4=_tmp9F2.f2;{struct Cyc_List_List*_tmp9F5=Cyc_Tcutil_substs(
rgn,inst,_tmp9F4);struct Cyc_List_List*_tmp9F6=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp9F3,_tmp9F5));struct Cyc_Core_Opt*
eff2;if(_tmp9A1 == 0)eff2=0;else{void*_tmp9F7=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp9A1->v);if(_tmp9F7 == (void*)_tmp9A1->v)eff2=_tmp9A1;else{struct Cyc_Core_Opt*
_tmp1317;eff2=((_tmp1317=_cycalloc(sizeof(*_tmp1317)),((_tmp1317->v=_tmp9F7,
_tmp1317))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp9A5 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp9FA;struct Cyc_Absyn_Tqual _tmp9FB;void*
_tmp9FC;int _tmp9FD;struct Cyc_Absyn_VarargInfo _tmp9F9=*_tmp9A5;_tmp9FA=_tmp9F9.name;
_tmp9FB=_tmp9F9.tq;_tmp9FC=_tmp9F9.type;_tmp9FD=_tmp9F9.inject;{void*_tmp9FE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9FC);if(_tmp9FE == _tmp9FC)cyc_varargs2=_tmp9A5;else{struct Cyc_Absyn_VarargInfo*
_tmp1318;cyc_varargs2=((_tmp1318=_cycalloc(sizeof(*_tmp1318)),((_tmp1318->name=
_tmp9FA,((_tmp1318->tq=_tmp9FB,((_tmp1318->type=_tmp9FE,((_tmp1318->inject=
_tmp9FD,_tmp1318))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA01;struct Cyc_List_List*_tmpA02;struct _tuple1 _tmpA00=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp9A6);_tmpA01=_tmpA00.f1;_tmpA02=_tmpA00.f2;{struct Cyc_List_List*
_tmpA03=Cyc_Tcutil_substs(rgn,inst,_tmpA01);struct Cyc_List_List*_tmpA04=Cyc_Tcutil_substs(
rgn,inst,_tmpA02);if(_tmpA03 == _tmpA01  && _tmpA04 == _tmpA02)rgn_po2=_tmp9A6;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA03,_tmpA04);}{struct Cyc_Absyn_FnType_struct _tmp131E;struct
Cyc_Absyn_FnInfo _tmp131D;struct Cyc_Absyn_FnType_struct*_tmp131C;return(void*)((
_tmp131C=_cycalloc(sizeof(*_tmp131C)),((_tmp131C[0]=((_tmp131E.tag=10,((_tmp131E.f1=((
_tmp131D.tvars=_tmp9A0,((_tmp131D.effect=eff2,((_tmp131D.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9A2),((_tmp131D.args=_tmp9F6,((_tmp131D.c_varargs=_tmp9A4,((
_tmp131D.cyc_varargs=cyc_varargs2,((_tmp131D.rgn_po=rgn_po2,((_tmp131D.attributes=
_tmp9A7,_tmp131D)))))))))))))))),_tmp131E)))),_tmp131C))));};};};};};};_LL4F1: {
struct Cyc_Absyn_TupleType_struct*_tmp9A8=(struct Cyc_Absyn_TupleType_struct*)
_tmp97A;if(_tmp9A8->tag != 11)goto _LL4F3;else{_tmp9A9=_tmp9A8->f1;}}_LL4F2: {
struct Cyc_List_List*_tmpA08=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp9A9);struct Cyc_List_List*_tmpA09=Cyc_Tcutil_substs(rgn,inst,_tmpA08);if(
_tmpA09 == _tmpA08)return t;{struct Cyc_List_List*_tmpA0A=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp9A9,_tmpA09);struct Cyc_Absyn_TupleType_struct
_tmp1321;struct Cyc_Absyn_TupleType_struct*_tmp1320;return(void*)((_tmp1320=
_cycalloc(sizeof(*_tmp1320)),((_tmp1320[0]=((_tmp1321.tag=11,((_tmp1321.f1=
_tmpA0A,_tmp1321)))),_tmp1320))));};}_LL4F3: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9AA=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp97A;if(_tmp9AA->tag != 13)goto
_LL4F5;else{_tmp9AB=_tmp9AA->f1;_tmp9AC=_tmp9AA->f2;}}_LL4F4: {struct Cyc_List_List*
_tmpA0D=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp9AC);struct
Cyc_List_List*_tmpA0E=Cyc_Tcutil_substs(rgn,inst,_tmpA0D);if(_tmpA0E == _tmpA0D)
return t;{struct Cyc_List_List*_tmpA0F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp9AC,_tmpA0E);struct Cyc_Absyn_AnonAggrType_struct
_tmp1324;struct Cyc_Absyn_AnonAggrType_struct*_tmp1323;return(void*)((_tmp1323=
_cycalloc(sizeof(*_tmp1323)),((_tmp1323[0]=((_tmp1324.tag=13,((_tmp1324.f1=
_tmp9AB,((_tmp1324.f2=_tmpA0F,_tmp1324)))))),_tmp1323))));};}_LL4F5: {struct Cyc_Absyn_Evar_struct*
_tmp9AD=(struct Cyc_Absyn_Evar_struct*)_tmp97A;if(_tmp9AD->tag != 1)goto _LL4F7;
else{_tmp9AE=_tmp9AD->f2;}}_LL4F6: if(_tmp9AE != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp9AE->v);else{return t;}_LL4F7: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp9AF=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp97A;if(_tmp9AF->tag != 16)goto
_LL4F9;else{_tmp9B0=(void*)_tmp9AF->f1;}}_LL4F8: {void*_tmpA12=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9B0);struct Cyc_Absyn_RgnHandleType_struct _tmp1327;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1326;return _tmpA12 == _tmp9B0?t:(void*)((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((
_tmp1326[0]=((_tmp1327.tag=16,((_tmp1327.f1=(void*)_tmpA12,_tmp1327)))),_tmp1326))));}
_LL4F9: {struct Cyc_Absyn_DynRgnType_struct*_tmp9B1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp97A;if(_tmp9B1->tag != 17)goto _LL4FB;else{_tmp9B2=(void*)_tmp9B1->f1;_tmp9B3=(
void*)_tmp9B1->f2;}}_LL4FA: {void*_tmpA15=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp9B2);void*_tmpA16=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9B3);struct Cyc_Absyn_DynRgnType_struct
_tmp132A;struct Cyc_Absyn_DynRgnType_struct*_tmp1329;return _tmpA15 == _tmp9B2  && 
_tmpA16 == _tmp9B3?t:(void*)((_tmp1329=_cycalloc(sizeof(*_tmp1329)),((_tmp1329[0]=((
_tmp132A.tag=17,((_tmp132A.f1=(void*)_tmpA15,((_tmp132A.f2=(void*)_tmpA16,
_tmp132A)))))),_tmp1329))));}_LL4FB: {struct Cyc_Absyn_TagType_struct*_tmp9B4=(
struct Cyc_Absyn_TagType_struct*)_tmp97A;if(_tmp9B4->tag != 20)goto _LL4FD;else{
_tmp9B5=(void*)_tmp9B4->f1;}}_LL4FC: {void*_tmpA19=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp9B5);struct Cyc_Absyn_TagType_struct _tmp132D;struct Cyc_Absyn_TagType_struct*
_tmp132C;return _tmpA19 == _tmp9B5?t:(void*)((_tmp132C=_cycalloc(sizeof(*_tmp132C)),((
_tmp132C[0]=((_tmp132D.tag=20,((_tmp132D.f1=(void*)_tmpA19,_tmp132D)))),_tmp132C))));}
_LL4FD: {struct Cyc_Absyn_ValueofType_struct*_tmp9B6=(struct Cyc_Absyn_ValueofType_struct*)
_tmp97A;if(_tmp9B6->tag != 19)goto _LL4FF;else{_tmp9B7=_tmp9B6->f1;}}_LL4FE: {
struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp9B7);struct Cyc_Absyn_ValueofType_struct
_tmp1330;struct Cyc_Absyn_ValueofType_struct*_tmp132F;return _tmpA1C == _tmp9B7?t:(
void*)((_tmp132F=_cycalloc(sizeof(*_tmp132F)),((_tmp132F[0]=((_tmp1330.tag=19,((
_tmp1330.f1=_tmpA1C,_tmp1330)))),_tmp132F))));}_LL4FF: {struct Cyc_Absyn_EnumType_struct*
_tmp9B8=(struct Cyc_Absyn_EnumType_struct*)_tmp97A;if(_tmp9B8->tag != 14)goto
_LL501;}_LL500: goto _LL502;_LL501: {struct Cyc_Absyn_AnonEnumType_struct*_tmp9B9=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp97A;if(_tmp9B9->tag != 15)goto _LL503;}
_LL502: goto _LL504;_LL503: {struct Cyc_Absyn_VoidType_struct*_tmp9BA=(struct Cyc_Absyn_VoidType_struct*)
_tmp97A;if(_tmp9BA->tag != 0)goto _LL505;}_LL504: goto _LL506;_LL505: {struct Cyc_Absyn_IntType_struct*
_tmp9BB=(struct Cyc_Absyn_IntType_struct*)_tmp97A;if(_tmp9BB->tag != 6)goto _LL507;}
_LL506: goto _LL508;_LL507: {struct Cyc_Absyn_FloatType_struct*_tmp9BC=(struct Cyc_Absyn_FloatType_struct*)
_tmp97A;if(_tmp9BC->tag != 7)goto _LL509;}_LL508: goto _LL50A;_LL509: {struct Cyc_Absyn_DoubleType_struct*
_tmp9BD=(struct Cyc_Absyn_DoubleType_struct*)_tmp97A;if(_tmp9BD->tag != 8)goto
_LL50B;}_LL50A: goto _LL50C;_LL50B: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9BE=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp97A;if(_tmp9BE->tag != 22)goto _LL50D;}
_LL50C: goto _LL50E;_LL50D: {struct Cyc_Absyn_HeapRgn_struct*_tmp9BF=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp97A;if(_tmp9BF->tag != 21)goto _LL50F;}_LL50E: return t;_LL50F: {struct Cyc_Absyn_RgnsEff_struct*
_tmp9C0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp97A;if(_tmp9C0->tag != 25)goto _LL511;
else{_tmp9C1=(void*)_tmp9C0->f1;}}_LL510: {void*_tmpA1F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9C1);struct Cyc_Absyn_RgnsEff_struct _tmp1333;struct Cyc_Absyn_RgnsEff_struct*
_tmp1332;return _tmpA1F == _tmp9C1?t:(void*)((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((
_tmp1332[0]=((_tmp1333.tag=25,((_tmp1333.f1=(void*)_tmpA1F,_tmp1333)))),_tmp1332))));}
_LL511: {struct Cyc_Absyn_AccessEff_struct*_tmp9C2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp97A;if(_tmp9C2->tag != 23)goto _LL513;else{_tmp9C3=(void*)_tmp9C2->f1;}}_LL512: {
void*_tmpA22=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9C3);struct Cyc_Absyn_AccessEff_struct
_tmp1336;struct Cyc_Absyn_AccessEff_struct*_tmp1335;return _tmpA22 == _tmp9C3?t:(
void*)((_tmp1335=_cycalloc(sizeof(*_tmp1335)),((_tmp1335[0]=((_tmp1336.tag=23,((
_tmp1336.f1=(void*)_tmpA22,_tmp1336)))),_tmp1335))));}_LL513: {struct Cyc_Absyn_JoinEff_struct*
_tmp9C4=(struct Cyc_Absyn_JoinEff_struct*)_tmp97A;if(_tmp9C4->tag != 24)goto _LL4E0;
else{_tmp9C5=_tmp9C4->f1;}}_LL514: {struct Cyc_List_List*_tmpA25=Cyc_Tcutil_substs(
rgn,inst,_tmp9C5);struct Cyc_Absyn_JoinEff_struct _tmp1339;struct Cyc_Absyn_JoinEff_struct*
_tmp1338;return _tmpA25 == _tmp9C5?t:(void*)((_tmp1338=_cycalloc(sizeof(*_tmp1338)),((
_tmp1338[0]=((_tmp1339.tag=24,((_tmp1339.f1=_tmpA25,_tmp1339)))),_tmp1338))));}
_LL4E0:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmpA28=(
void*)ts->hd;struct Cyc_List_List*_tmpA29=ts->tl;void*_tmpA2A=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA28);struct Cyc_List_List*_tmpA2B=Cyc_Tcutil_substs(rgn,inst,_tmpA29);
if(_tmpA28 == _tmpA2A  && _tmpA29 == _tmpA2B)return ts;{struct Cyc_List_List*_tmp133A;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp133A=_cycalloc(sizeof(*
_tmp133A)),((_tmp133A->hd=_tmpA2A,((_tmp133A->tl=_tmpA2B,_tmp133A)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpA2D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp133D;struct _tuple15*_tmp133C;return(
_tmp133C=_cycalloc(sizeof(*_tmp133C)),((_tmp133C->f1=tv,((_tmp133C->f2=Cyc_Absyn_new_evar(
_tmpA2D,((_tmp133D=_cycalloc(sizeof(*_tmp133D)),((_tmp133D->v=s,_tmp133D))))),
_tmp133C)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpA31;struct Cyc_List_List*_tmpA32;
struct _RegionHandle*_tmpA33;struct _tuple16*_tmpA30=env;_tmpA31=*_tmpA30;_tmpA32=
_tmpA31.f1;_tmpA33=_tmpA31.f2;{struct Cyc_Core_Opt*_tmpA34=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1340;struct
_tuple15*_tmp133F;return(_tmp133F=_region_malloc(_tmpA33,sizeof(*_tmp133F)),((
_tmp133F->f1=tv,((_tmp133F->f2=Cyc_Absyn_new_evar(_tmpA34,((_tmp1340=_cycalloc(
sizeof(*_tmp1340)),((_tmp1340->v=_tmpA32,_tmp1340))))),_tmp133F)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp1346;void*_tmp1345[3];
struct Cyc_String_pa_struct _tmp1344;struct Cyc_String_pa_struct _tmp1343;struct Cyc_String_pa_struct
_tmp1342;(_tmp1342.tag=0,((_tmp1342.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp1343.tag=0,((_tmp1343.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp1344.tag=0,((_tmp1344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp1345[0]=& _tmp1344,((_tmp1345[1]=& _tmp1343,((_tmp1345[2]=& _tmp1342,Cyc_Tcutil_terr(
loc,((_tmp1346="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1346,sizeof(char),59))),_tag_dyneither(_tmp1345,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1349;void*_tmp1348;(_tmp1348=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1349="same type variable has different identity!",
_tag_dyneither(_tmp1349,sizeof(char),43))),_tag_dyneither(_tmp1348,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp134A;return(_tmp134A=_cycalloc(sizeof(*_tmp134A)),((_tmp134A->hd=tv,((
_tmp134A->tl=tvs,_tmp134A)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp134D;void*_tmp134C;(_tmp134C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134D="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp134D,sizeof(char),39))),_tag_dyneither(_tmp134C,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpA41=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpA41->identity == - 1){const char*
_tmp1350;void*_tmp134F;(_tmp134F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1350="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1350,sizeof(char),41))),_tag_dyneither(_tmp134F,sizeof(void*),
0)));}if(_tmpA41->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1351;return(_tmp1351=_cycalloc(sizeof(*_tmp1351)),((_tmp1351->hd=tv,((
_tmp1351->tl=tvs,_tmp1351)))));};}struct _tuple27{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity
== - 1){const char*_tmp1354;void*_tmp1353;(_tmp1353=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1354="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1354,sizeof(char),44))),_tag_dyneither(_tmp1353,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple27
_tmpA48;struct Cyc_Absyn_Tvar*_tmpA49;int _tmpA4A;int*_tmpA4B;struct _tuple27*
_tmpA47=(struct _tuple27*)tvs2->hd;_tmpA48=*_tmpA47;_tmpA49=_tmpA48.f1;_tmpA4A=
_tmpA48.f2;_tmpA4B=(int*)&(*_tmpA47).f2;if(_tmpA49->identity == - 1){const char*
_tmp1357;void*_tmp1356;(_tmp1356=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1357="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1357,sizeof(char),46))),_tag_dyneither(_tmp1356,sizeof(void*),
0)));}if(_tmpA49->identity == tv->identity){*_tmpA4B=*_tmpA4B  || b;return tvs;}}}{
struct _tuple27*_tmp135A;struct Cyc_List_List*_tmp1359;return(_tmp1359=
_region_malloc(r,sizeof(*_tmp1359)),((_tmp1359->hd=((_tmp135A=_region_malloc(r,
sizeof(*_tmp135A)),((_tmp135A->f1=tv,((_tmp135A->f2=b,_tmp135A)))))),((_tmp1359->tl=
tvs,_tmp1359)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp135E;void*_tmp135D[1];struct Cyc_String_pa_struct _tmp135C;(_tmp135C.tag=0,((
_tmp135C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp135D[0]=& _tmp135C,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135E="bound tvar id for %s is NULL",
_tag_dyneither(_tmp135E,sizeof(char),29))),_tag_dyneither(_tmp135D,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp135F;return(_tmp135F=_cycalloc(sizeof(*
_tmp135F)),((_tmp135F->hd=tv,((_tmp135F->tl=tvs,_tmp135F)))));};}struct _tuple28{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpA54=Cyc_Tcutil_compress(
e);int _tmpA56;_LL520: {struct Cyc_Absyn_Evar_struct*_tmpA55=(struct Cyc_Absyn_Evar_struct*)
_tmpA54;if(_tmpA55->tag != 1)goto _LL522;else{_tmpA56=_tmpA55->f3;}}_LL521:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple28 _tmpA58;void*
_tmpA59;int _tmpA5A;int*_tmpA5B;struct _tuple28*_tmpA57=(struct _tuple28*)es2->hd;
_tmpA58=*_tmpA57;_tmpA59=_tmpA58.f1;_tmpA5A=_tmpA58.f2;_tmpA5B=(int*)&(*_tmpA57).f2;{
void*_tmpA5C=Cyc_Tcutil_compress(_tmpA59);int _tmpA5E;_LL525: {struct Cyc_Absyn_Evar_struct*
_tmpA5D=(struct Cyc_Absyn_Evar_struct*)_tmpA5C;if(_tmpA5D->tag != 1)goto _LL527;
else{_tmpA5E=_tmpA5D->f3;}}_LL526: if(_tmpA56 == _tmpA5E){if(b != *_tmpA5B)*_tmpA5B=
1;return es;}goto _LL524;_LL527:;_LL528: goto _LL524;_LL524:;};}}{struct _tuple28*
_tmp1362;struct Cyc_List_List*_tmp1361;return(_tmp1361=_region_malloc(r,sizeof(*
_tmp1361)),((_tmp1361->hd=((_tmp1362=_region_malloc(r,sizeof(*_tmp1362)),((
_tmp1362->f1=e,((_tmp1362->f2=b,_tmp1362)))))),((_tmp1361->tl=es,_tmp1361)))));};
_LL522:;_LL523: return es;_LL51F:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1363;r=((_tmp1363=_region_malloc(rgn,sizeof(*_tmp1363)),((
_tmp1363->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1363->tl=r,_tmp1363))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpA63;int _tmpA64;struct
_tuple27 _tmpA62=*((struct _tuple27*)tvs->hd);_tmpA63=_tmpA62.f1;_tmpA64=_tmpA62.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpA63->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1364;res=((_tmp1364=_region_malloc(r,sizeof(*_tmp1364)),((
_tmp1364->hd=(struct _tuple27*)tvs->hd,((_tmp1364->tl=res,_tmp1364))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp1368;void*_tmp1367[1];struct Cyc_String_pa_struct _tmp1366;(_tmp1366.tag=
0,((_tmp1366.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1367[0]=&
_tmp1366,Cyc_Tcutil_terr(loc,((_tmp1368="bitfield %s does not have constant width",
_tag_dyneither(_tmp1368,sizeof(char),41))),_tag_dyneither(_tmp1367,sizeof(void*),
1)))))));}else{unsigned int _tmpA6A;int _tmpA6B;struct _tuple13 _tmpA69=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpA6A=_tmpA69.f1;_tmpA6B=_tmpA69.f2;if(!_tmpA6B){
const char*_tmp136B;void*_tmp136A;(_tmp136A=0,Cyc_Tcutil_terr(loc,((_tmp136B="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp136B,sizeof(char),45))),_tag_dyneither(_tmp136A,sizeof(void*),
0)));}w=_tmpA6A;}{void*_tmpA6E=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpA70;_LL52A: {struct Cyc_Absyn_IntType_struct*_tmpA6F=(struct Cyc_Absyn_IntType_struct*)
_tmpA6E;if(_tmpA6F->tag != 6)goto _LL52C;else{_tmpA70=_tmpA6F->f2;}}_LL52B: switch(
_tmpA70){case Cyc_Absyn_Char_sz: _LL52E: if(w > 8){const char*_tmp136E;void*_tmp136D;(
_tmp136D=0,Cyc_Tcutil_terr(loc,((_tmp136E="bitfield larger than type",
_tag_dyneither(_tmp136E,sizeof(char),26))),_tag_dyneither(_tmp136D,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL52F: if(w > 16){const char*_tmp1371;void*
_tmp1370;(_tmp1370=0,Cyc_Tcutil_terr(loc,((_tmp1371="bitfield larger than type",
_tag_dyneither(_tmp1371,sizeof(char),26))),_tag_dyneither(_tmp1370,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL530: goto _LL531;case Cyc_Absyn_Int_sz: _LL531:
if(w > 32){const char*_tmp1374;void*_tmp1373;(_tmp1373=0,Cyc_Tcutil_terr(loc,((
_tmp1374="bitfield larger than type",_tag_dyneither(_tmp1374,sizeof(char),26))),
_tag_dyneither(_tmp1373,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL532: if(w > 64){const char*_tmp1377;void*_tmp1376;(_tmp1376=0,Cyc_Tcutil_terr(
loc,((_tmp1377="bitfield larger than type",_tag_dyneither(_tmp1377,sizeof(char),
26))),_tag_dyneither(_tmp1376,sizeof(void*),0)));}break;}goto _LL529;_LL52C:;
_LL52D:{const char*_tmp137C;void*_tmp137B[2];struct Cyc_String_pa_struct _tmp137A;
struct Cyc_String_pa_struct _tmp1379;(_tmp1379.tag=0,((_tmp1379.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp137A.tag=0,((_tmp137A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp137B[0]=& _tmp137A,((_tmp137B[1]=& _tmp1379,Cyc_Tcutil_terr(loc,((_tmp137C="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp137C,sizeof(char),52))),_tag_dyneither(_tmp137B,sizeof(void*),
2)))))))))))));}goto _LL529;_LL529:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpA7D=(void*)atts->hd;_LL535: {struct Cyc_Absyn_Packed_att_struct*
_tmpA7E=(struct Cyc_Absyn_Packed_att_struct*)_tmpA7D;if(_tmpA7E->tag != 7)goto
_LL537;}_LL536: continue;_LL537: {struct Cyc_Absyn_Aligned_att_struct*_tmpA7F=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpA7D;if(_tmpA7F->tag != 6)goto _LL539;}
_LL538: continue;_LL539:;_LL53A: {const char*_tmp1381;void*_tmp1380[2];struct Cyc_String_pa_struct
_tmp137F;struct Cyc_String_pa_struct _tmp137E;(_tmp137E.tag=0,((_tmp137E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp137F.tag=0,((_tmp137F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1380[0]=& _tmp137F,((_tmp1380[1]=& _tmp137E,Cyc_Tcutil_terr(loc,((
_tmp1381="bad attribute %s on member %s",_tag_dyneither(_tmp1381,sizeof(char),30))),
_tag_dyneither(_tmp1380,sizeof(void*),2)))))))))))));}_LL534:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpA84=t;struct
Cyc_Absyn_Typedefdecl*_tmpA86;void**_tmpA87;_LL53C: {struct Cyc_Absyn_TypedefType_struct*
_tmpA85=(struct Cyc_Absyn_TypedefType_struct*)_tmpA84;if(_tmpA85->tag != 18)goto
_LL53E;else{_tmpA86=_tmpA85->f3;_tmpA87=_tmpA85->f4;}}_LL53D: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpA86))->tq).real_const  || (_tmpA86->tq).print_const){if(
declared_const){const char*_tmp1384;void*_tmp1383;(_tmp1383=0,Cyc_Tcutil_warn(loc,((
_tmp1384="extra const",_tag_dyneither(_tmp1384,sizeof(char),12))),_tag_dyneither(
_tmp1383,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpA87)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpA87);else{return declared_const;}_LL53E:;_LL53F: return
declared_const;_LL53B:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpA8A=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpA8C;struct Cyc_Absyn_PtrAtts _tmpA8D;void*_tmpA8E;
_LL541: {struct Cyc_Absyn_PointerType_struct*_tmpA8B=(struct Cyc_Absyn_PointerType_struct*)
_tmpA8A;if(_tmpA8B->tag != 5)goto _LL543;else{_tmpA8C=_tmpA8B->f1;_tmpA8D=_tmpA8C.ptr_atts;
_tmpA8E=_tmpA8D.rgn;}}_LL542:{void*_tmpA8F=Cyc_Tcutil_compress(_tmpA8E);struct
Cyc_Absyn_Tvar*_tmpA91;_LL546: {struct Cyc_Absyn_VarType_struct*_tmpA90=(struct
Cyc_Absyn_VarType_struct*)_tmpA8F;if(_tmpA90->tag != 2)goto _LL548;else{_tmpA91=
_tmpA90->f1;}}_LL547: return Cyc_Absyn_tvar_cmp(tvar,_tmpA91)== 0;_LL548:;_LL549:
goto _LL545;_LL545:;}goto _LL540;_LL543:;_LL544: goto _LL540;_LL540:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpA92=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpA94;
struct Cyc_Absyn_Kind _tmpA95;enum Cyc_Absyn_KindQual _tmpA96;enum Cyc_Absyn_AliasQual
_tmpA97;struct Cyc_Absyn_Kind*_tmpA99;struct Cyc_Absyn_Kind _tmpA9A;enum Cyc_Absyn_KindQual
_tmpA9B;enum Cyc_Absyn_AliasQual _tmpA9C;_LL54B: {struct Cyc_Absyn_Less_kb_struct*
_tmpA93=(struct Cyc_Absyn_Less_kb_struct*)_tmpA92;if(_tmpA93->tag != 2)goto _LL54D;
else{_tmpA94=_tmpA93->f2;_tmpA95=*_tmpA94;_tmpA96=_tmpA95.kind;if(_tmpA96 != Cyc_Absyn_RgnKind)
goto _LL54D;_tmpA97=_tmpA95.aliasqual;if(_tmpA97 != Cyc_Absyn_Top)goto _LL54D;}}
_LL54C: goto _LL54E;_LL54D: {struct Cyc_Absyn_Eq_kb_struct*_tmpA98=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpA92;if(_tmpA98->tag != 0)goto _LL54F;else{_tmpA99=_tmpA98->f1;_tmpA9A=*_tmpA99;
_tmpA9B=_tmpA9A.kind;if(_tmpA9B != Cyc_Absyn_RgnKind)goto _LL54F;_tmpA9C=_tmpA9A.aliasqual;
if(_tmpA9C != Cyc_Absyn_Top)goto _LL54F;}}_LL54E: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL54F:;_LL550: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL54A:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple29{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpA9D=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpAA0;struct Cyc_Core_Opt**
_tmpAA1;struct Cyc_Core_Opt*_tmpAA2;struct Cyc_Core_Opt**_tmpAA3;struct Cyc_Absyn_Tvar*
_tmpAA5;struct Cyc_List_List*_tmpAA7;struct _tuple2*_tmpAA9;struct Cyc_Absyn_Enumdecl*
_tmpAAA;struct Cyc_Absyn_Enumdecl**_tmpAAB;struct Cyc_Absyn_DatatypeInfo _tmpAAD;
union Cyc_Absyn_DatatypeInfoU _tmpAAE;union Cyc_Absyn_DatatypeInfoU*_tmpAAF;struct
Cyc_List_List*_tmpAB0;struct Cyc_List_List**_tmpAB1;struct Cyc_Absyn_DatatypeFieldInfo
_tmpAB3;union Cyc_Absyn_DatatypeFieldInfoU _tmpAB4;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpAB5;struct Cyc_List_List*_tmpAB6;struct Cyc_Absyn_PtrInfo _tmpAB8;void*_tmpAB9;
struct Cyc_Absyn_Tqual _tmpABA;struct Cyc_Absyn_Tqual*_tmpABB;struct Cyc_Absyn_PtrAtts
_tmpABC;void*_tmpABD;union Cyc_Absyn_Constraint*_tmpABE;union Cyc_Absyn_Constraint*
_tmpABF;union Cyc_Absyn_Constraint*_tmpAC0;void*_tmpAC2;struct Cyc_Absyn_Exp*
_tmpAC4;struct Cyc_Absyn_ArrayInfo _tmpAC9;void*_tmpACA;struct Cyc_Absyn_Tqual
_tmpACB;struct Cyc_Absyn_Tqual*_tmpACC;struct Cyc_Absyn_Exp*_tmpACD;struct Cyc_Absyn_Exp**
_tmpACE;union Cyc_Absyn_Constraint*_tmpACF;struct Cyc_Position_Segment*_tmpAD0;
struct Cyc_Absyn_FnInfo _tmpAD2;struct Cyc_List_List*_tmpAD3;struct Cyc_List_List**
_tmpAD4;struct Cyc_Core_Opt*_tmpAD5;struct Cyc_Core_Opt**_tmpAD6;void*_tmpAD7;
struct Cyc_List_List*_tmpAD8;int _tmpAD9;struct Cyc_Absyn_VarargInfo*_tmpADA;struct
Cyc_List_List*_tmpADB;struct Cyc_List_List*_tmpADC;struct Cyc_List_List*_tmpADE;
enum Cyc_Absyn_AggrKind _tmpAE0;struct Cyc_List_List*_tmpAE1;struct Cyc_Absyn_AggrInfo
_tmpAE3;union Cyc_Absyn_AggrInfoU _tmpAE4;union Cyc_Absyn_AggrInfoU*_tmpAE5;struct
Cyc_List_List*_tmpAE6;struct Cyc_List_List**_tmpAE7;struct _tuple2*_tmpAE9;struct
Cyc_List_List*_tmpAEA;struct Cyc_List_List**_tmpAEB;struct Cyc_Absyn_Typedefdecl*
_tmpAEC;struct Cyc_Absyn_Typedefdecl**_tmpAED;void**_tmpAEE;void***_tmpAEF;void*
_tmpAF3;void*_tmpAF5;void*_tmpAF6;void*_tmpAF8;void*_tmpAFA;struct Cyc_List_List*
_tmpAFC;_LL552: {struct Cyc_Absyn_VoidType_struct*_tmpA9E=(struct Cyc_Absyn_VoidType_struct*)
_tmpA9D;if(_tmpA9E->tag != 0)goto _LL554;}_LL553: goto _LL551;_LL554: {struct Cyc_Absyn_Evar_struct*
_tmpA9F=(struct Cyc_Absyn_Evar_struct*)_tmpA9D;if(_tmpA9F->tag != 1)goto _LL556;
else{_tmpAA0=_tmpA9F->f1;_tmpAA1=(struct Cyc_Core_Opt**)& _tmpA9F->f1;_tmpAA2=
_tmpA9F->f2;_tmpAA3=(struct Cyc_Core_Opt**)& _tmpA9F->f2;}}_LL555: if(*_tmpAA1 == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmpAA1))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpAA1))->v,expected_kind))*_tmpAA1=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpAA3=(
struct Cyc_Core_Opt*)& urgn;else{*_tmpAA3=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1387;struct Cyc_Absyn_Less_kb_struct*
_tmp1386;struct Cyc_Absyn_Tvar*_tmpAFD=Cyc_Tcutil_new_tvar((void*)((_tmp1386=
_cycalloc(sizeof(*_tmp1386)),((_tmp1386[0]=((_tmp1387.tag=2,((_tmp1387.f1=0,((
_tmp1387.f2=expected_kind,_tmp1387)))))),_tmp1386)))));{struct Cyc_Absyn_VarType_struct*
_tmp138D;struct Cyc_Absyn_VarType_struct _tmp138C;struct Cyc_Core_Opt*_tmp138B;*
_tmpAA3=((_tmp138B=_cycalloc(sizeof(*_tmp138B)),((_tmp138B->v=(void*)((_tmp138D=
_cycalloc(sizeof(*_tmp138D)),((_tmp138D[0]=((_tmp138C.tag=2,((_tmp138C.f1=
_tmpAFD,_tmp138C)))),_tmp138D)))),_tmp138B))));}_tmpAA5=_tmpAFD;goto _LL557;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL551;_LL556: {struct Cyc_Absyn_VarType_struct*_tmpAA4=(
struct Cyc_Absyn_VarType_struct*)_tmpA9D;if(_tmpAA4->tag != 2)goto _LL558;else{
_tmpAA5=_tmpAA4->f1;}}_LL557:{void*_tmpB03=Cyc_Absyn_compress_kb(_tmpAA5->kind);
struct Cyc_Core_Opt*_tmpB05;struct Cyc_Core_Opt**_tmpB06;_LL587: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB04=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB03;if(_tmpB04->tag != 1)goto
_LL589;else{_tmpB05=_tmpB04->f1;_tmpB06=(struct Cyc_Core_Opt**)& _tmpB04->f1;}}
_LL588:{struct Cyc_Absyn_Less_kb_struct*_tmp1393;struct Cyc_Absyn_Less_kb_struct
_tmp1392;struct Cyc_Core_Opt*_tmp1391;*_tmpB06=((_tmp1391=_cycalloc(sizeof(*
_tmp1391)),((_tmp1391->v=(void*)((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((
_tmp1393[0]=((_tmp1392.tag=2,((_tmp1392.f1=0,((_tmp1392.f2=expected_kind,
_tmp1392)))))),_tmp1393)))),_tmp1391))));}goto _LL586;_LL589:;_LL58A: goto _LL586;
_LL586:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAA5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpAA5,put_in_effect);{void*_tmpB0A=Cyc_Absyn_compress_kb(_tmpAA5->kind);struct
Cyc_Core_Opt*_tmpB0C;struct Cyc_Core_Opt**_tmpB0D;struct Cyc_Absyn_Kind*_tmpB0E;
_LL58C: {struct Cyc_Absyn_Less_kb_struct*_tmpB0B=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB0A;if(_tmpB0B->tag != 2)goto _LL58E;else{_tmpB0C=_tmpB0B->f1;_tmpB0D=(struct
Cyc_Core_Opt**)& _tmpB0B->f1;_tmpB0E=_tmpB0B->f2;}}_LL58D: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB0E)){struct Cyc_Absyn_Less_kb_struct*_tmp1399;struct Cyc_Absyn_Less_kb_struct
_tmp1398;struct Cyc_Core_Opt*_tmp1397;*_tmpB0D=((_tmp1397=_cycalloc(sizeof(*
_tmp1397)),((_tmp1397->v=(void*)((_tmp1399=_cycalloc(sizeof(*_tmp1399)),((
_tmp1399[0]=((_tmp1398.tag=2,((_tmp1398.f1=0,((_tmp1398.f2=expected_kind,
_tmp1398)))))),_tmp1399)))),_tmp1397))));}goto _LL58B;_LL58E:;_LL58F: goto _LL58B;
_LL58B:;}goto _LL551;_LL558: {struct Cyc_Absyn_AnonEnumType_struct*_tmpAA6=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmpA9D;if(_tmpAA6->tag != 15)goto _LL55A;else{
_tmpAA7=_tmpAA6->f1;}}_LL559: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB12=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpAA7 != 0;_tmpAA7=_tmpAA7->tl){
struct Cyc_Absyn_Enumfield*_tmpB13=(struct Cyc_Absyn_Enumfield*)_tmpAA7->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB13->name).f2)){
const char*_tmp139D;void*_tmp139C[1];struct Cyc_String_pa_struct _tmp139B;(_tmp139B.tag=
0,((_tmp139B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB13->name).f2),((
_tmp139C[0]=& _tmp139B,Cyc_Tcutil_terr(_tmpB13->loc,((_tmp139D="duplicate enum field name %s",
_tag_dyneither(_tmp139D,sizeof(char),29))),_tag_dyneither(_tmp139C,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp139E;prev_fields=((_tmp139E=
_region_malloc(_tmpB12,sizeof(*_tmp139E)),((_tmp139E->hd=(*_tmpB13->name).f2,((
_tmp139E->tl=prev_fields,_tmp139E))))));}if(_tmpB13->tag == 0)_tmpB13->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB13->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB13->tag))){const char*_tmp13A2;void*
_tmp13A1[1];struct Cyc_String_pa_struct _tmp13A0;(_tmp13A0.tag=0,((_tmp13A0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB13->name).f2),((_tmp13A1[0]=&
_tmp13A0,Cyc_Tcutil_terr(loc,((_tmp13A2="enum field %s: expression is not constant",
_tag_dyneither(_tmp13A2,sizeof(char),42))),_tag_dyneither(_tmp13A1,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB13->tag))).f1;tag_count=t1 + 1;(*_tmpB13->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp13A8;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp13A7;struct _tuple28*_tmp13A6;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple28*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB13->name).f2,(struct _tuple28*)((_tmp13A6=_cycalloc(sizeof(*_tmp13A6)),((
_tmp13A6->f1=(void*)((_tmp13A8=_cycalloc(sizeof(*_tmp13A8)),((_tmp13A8[0]=((
_tmp13A7.tag=4,((_tmp13A7.f1=(void*)t,((_tmp13A7.f2=_tmpB13,_tmp13A7)))))),
_tmp13A8)))),((_tmp13A6->f2=1,_tmp13A6)))))));};};}}goto _LL551;}_LL55A: {struct
Cyc_Absyn_EnumType_struct*_tmpAA8=(struct Cyc_Absyn_EnumType_struct*)_tmpA9D;if(
_tmpAA8->tag != 14)goto _LL55C;else{_tmpAA9=_tmpAA8->f1;_tmpAAA=_tmpAA8->f2;
_tmpAAB=(struct Cyc_Absyn_Enumdecl**)& _tmpAA8->f2;}}_LL55B: if(*_tmpAAB == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpAAB))->fields == 0){struct
_handler_cons _tmpB1E;_push_handler(& _tmpB1E);{int _tmpB20=0;if(setjmp(_tmpB1E.handler))
_tmpB20=1;if(!_tmpB20){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpAA9);*_tmpAAB=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB1F=(void*)_exn_thrown;void*_tmpB22=_tmpB1F;_LL591: {struct Cyc_Dict_Absent_struct*
_tmpB23=(struct Cyc_Dict_Absent_struct*)_tmpB22;if(_tmpB23->tag != Cyc_Dict_Absent)
goto _LL593;}_LL592: {struct Cyc_Tcenv_Genv*_tmpB24=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp13A9;struct Cyc_Absyn_Enumdecl*_tmpB25=(_tmp13A9=
_cycalloc(sizeof(*_tmp13A9)),((_tmp13A9->sc=Cyc_Absyn_Extern,((_tmp13A9->name=
_tmpAA9,((_tmp13A9->fields=0,_tmp13A9)))))));Cyc_Tc_tcEnumdecl(te,_tmpB24,loc,
_tmpB25);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpAA9);*
_tmpAAB=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL590;};}_LL593:;_LL594:(void)_throw(
_tmpB22);_LL590:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpAAB);*_tmpAA9=(ed->name)[0];goto _LL551;};_LL55C: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAAC=(struct Cyc_Absyn_DatatypeType_struct*)_tmpA9D;if(_tmpAAC->tag != 3)goto
_LL55E;else{_tmpAAD=_tmpAAC->f1;_tmpAAE=_tmpAAD.datatype_info;_tmpAAF=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpAAC->f1).datatype_info;_tmpAB0=_tmpAAD.targs;_tmpAB1=(struct Cyc_List_List**)&(
_tmpAAC->f1).targs;}}_LL55D: {struct Cyc_List_List*_tmpB27=*_tmpAB1;{union Cyc_Absyn_DatatypeInfoU
_tmpB28=*_tmpAAF;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB29;struct _tuple2*
_tmpB2A;int _tmpB2B;struct Cyc_Absyn_Datatypedecl**_tmpB2C;struct Cyc_Absyn_Datatypedecl*
_tmpB2D;_LL596: if((_tmpB28.UnknownDatatype).tag != 1)goto _LL598;_tmpB29=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpB28.UnknownDatatype).val;_tmpB2A=_tmpB29.name;
_tmpB2B=_tmpB29.is_extensible;_LL597: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpB2E;_push_handler(& _tmpB2E);{int _tmpB30=0;if(setjmp(
_tmpB2E.handler))_tmpB30=1;if(!_tmpB30){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpB2A);;_pop_handler();}else{void*_tmpB2F=(void*)_exn_thrown;void*_tmpB32=
_tmpB2F;_LL59B: {struct Cyc_Dict_Absent_struct*_tmpB33=(struct Cyc_Dict_Absent_struct*)
_tmpB32;if(_tmpB33->tag != Cyc_Dict_Absent)goto _LL59D;}_LL59C: {struct Cyc_Tcenv_Genv*
_tmpB34=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp13AA;struct Cyc_Absyn_Datatypedecl*
_tmpB35=(_tmp13AA=_cycalloc(sizeof(*_tmp13AA)),((_tmp13AA->sc=Cyc_Absyn_Extern,((
_tmp13AA->name=_tmpB2A,((_tmp13AA->tvs=0,((_tmp13AA->fields=0,((_tmp13AA->is_extensible=
_tmpB2B,_tmp13AA)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpB34,loc,_tmpB35);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpB2A);if(_tmpB27 != 0){{const char*
_tmp13AE;void*_tmp13AD[1];struct Cyc_String_pa_struct _tmp13AC;(_tmp13AC.tag=0,((
_tmp13AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB2A)),((_tmp13AD[0]=& _tmp13AC,Cyc_Tcutil_terr(loc,((_tmp13AE="declare parameterized datatype %s before using",
_tag_dyneither(_tmp13AE,sizeof(char),47))),_tag_dyneither(_tmp13AD,sizeof(void*),
1)))))));}return cvtenv;}goto _LL59A;}_LL59D:;_LL59E:(void)_throw(_tmpB32);_LL59A:;}};}
if(_tmpB2B  && !(*tudp)->is_extensible){const char*_tmp13B2;void*_tmp13B1[1];
struct Cyc_String_pa_struct _tmp13B0;(_tmp13B0.tag=0,((_tmp13B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB2A)),((
_tmp13B1[0]=& _tmp13B0,Cyc_Tcutil_terr(loc,((_tmp13B2="datatype %s was not declared @extensible",
_tag_dyneither(_tmp13B2,sizeof(char),41))),_tag_dyneither(_tmp13B1,sizeof(void*),
1)))))));}*_tmpAAF=Cyc_Absyn_KnownDatatype(tudp);_tmpB2D=*tudp;goto _LL599;}
_LL598: if((_tmpB28.KnownDatatype).tag != 2)goto _LL595;_tmpB2C=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB28.KnownDatatype).val;_tmpB2D=*_tmpB2C;_LL599: {struct Cyc_List_List*tvs=
_tmpB2D->tvs;for(0;_tmpB27 != 0  && tvs != 0;(_tmpB27=_tmpB27->tl,tvs=tvs->tl)){
void*t=(void*)_tmpB27->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp13B3;struct _tuple0 _tmpB3E=(_tmp13B3.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp13B3.f2=t,_tmp13B3)));void*_tmpB3F;void*_tmpB41;struct Cyc_Absyn_Tvar*_tmpB43;
_LL5A0: _tmpB3F=_tmpB3E.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB40=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB3F;if(_tmpB40->tag != 1)goto _LL5A2;};_tmpB41=_tmpB3E.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB42=(struct Cyc_Absyn_VarType_struct*)_tmpB41;if(_tmpB42->tag != 2)goto _LL5A2;
else{_tmpB43=_tmpB42->f1;}};_LL5A1: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB43);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB43,1);continue;_LL5A2:;_LL5A3: goto _LL59F;_LL59F:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpB27 != 0){const char*_tmp13B7;void*_tmp13B6[1];
struct Cyc_String_pa_struct _tmp13B5;(_tmp13B5.tag=0,((_tmp13B5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB2D->name)),((
_tmp13B6[0]=& _tmp13B5,Cyc_Tcutil_terr(loc,((_tmp13B7="too many type arguments for datatype %s",
_tag_dyneither(_tmp13B7,sizeof(char),40))),_tag_dyneither(_tmp13B6,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp13B8;hidden_ts=((_tmp13B8=_cycalloc(sizeof(*_tmp13B8)),((_tmp13B8->hd=e,((
_tmp13B8->tl=hidden_ts,_tmp13B8))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpAB1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAB1,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL595;}_LL595:;}goto
_LL551;}_LL55E: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpAB2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpA9D;if(_tmpAB2->tag != 4)goto _LL560;else{_tmpAB3=_tmpAB2->f1;_tmpAB4=_tmpAB3.field_info;
_tmpAB5=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpAB2->f1).field_info;_tmpAB6=
_tmpAB3.targs;}}_LL55F:{union Cyc_Absyn_DatatypeFieldInfoU _tmpB48=*_tmpAB5;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB49;struct _tuple2*_tmpB4A;struct _tuple2*
_tmpB4B;int _tmpB4C;struct _tuple3 _tmpB4D;struct Cyc_Absyn_Datatypedecl*_tmpB4E;
struct Cyc_Absyn_Datatypefield*_tmpB4F;_LL5A5: if((_tmpB48.UnknownDatatypefield).tag
!= 1)goto _LL5A7;_tmpB49=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpB48.UnknownDatatypefield).val;
_tmpB4A=_tmpB49.datatype_name;_tmpB4B=_tmpB49.field_name;_tmpB4C=_tmpB49.is_extensible;
_LL5A6: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpB50;_push_handler(& _tmpB50);{int _tmpB52=0;if(setjmp(
_tmpB50.handler))_tmpB52=1;if(!_tmpB52){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpB4A);;_pop_handler();}else{void*_tmpB51=(void*)_exn_thrown;void*_tmpB54=
_tmpB51;_LL5AA: {struct Cyc_Dict_Absent_struct*_tmpB55=(struct Cyc_Dict_Absent_struct*)
_tmpB54;if(_tmpB55->tag != Cyc_Dict_Absent)goto _LL5AC;}_LL5AB:{const char*_tmp13BC;
void*_tmp13BB[1];struct Cyc_String_pa_struct _tmp13BA;(_tmp13BA.tag=0,((_tmp13BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4A)),((
_tmp13BB[0]=& _tmp13BA,Cyc_Tcutil_terr(loc,((_tmp13BC="unbound datatype %s",
_tag_dyneither(_tmp13BC,sizeof(char),20))),_tag_dyneither(_tmp13BB,sizeof(void*),
1)))))));}return cvtenv;_LL5AC:;_LL5AD:(void)_throw(_tmpB54);_LL5A9:;}};}{struct
_handler_cons _tmpB59;_push_handler(& _tmpB59);{int _tmpB5B=0;if(setjmp(_tmpB59.handler))
_tmpB5B=1;if(!_tmpB5B){{struct _RegionHandle*_tmpB5C=Cyc_Tcenv_get_fnrgn(te);void*
_tmpB5D=Cyc_Tcenv_lookup_ordinary(_tmpB5C,te,loc,_tmpB4B);struct Cyc_Absyn_Datatypedecl*
_tmpB5F;struct Cyc_Absyn_Datatypefield*_tmpB60;_LL5AF: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpB5E=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpB5D;if(_tmpB5E->tag != 2)goto
_LL5B1;else{_tmpB5F=_tmpB5E->f1;_tmpB60=_tmpB5E->f2;}}_LL5B0: tuf=_tmpB60;tud=
_tmpB5F;if(_tmpB4C  && !tud->is_extensible){const char*_tmp13C0;void*_tmp13BF[1];
struct Cyc_String_pa_struct _tmp13BE;(_tmp13BE.tag=0,((_tmp13BE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4A)),((
_tmp13BF[0]=& _tmp13BE,Cyc_Tcutil_terr(loc,((_tmp13C0="datatype %s was not declared @extensible",
_tag_dyneither(_tmp13C0,sizeof(char),41))),_tag_dyneither(_tmp13BF,sizeof(void*),
1)))))));}goto _LL5AE;_LL5B1:;_LL5B2:{const char*_tmp13C5;void*_tmp13C4[2];struct
Cyc_String_pa_struct _tmp13C3;struct Cyc_String_pa_struct _tmp13C2;(_tmp13C2.tag=0,((
_tmp13C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4A)),((_tmp13C3.tag=0,((_tmp13C3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4B)),((_tmp13C4[0]=& _tmp13C3,((
_tmp13C4[1]=& _tmp13C2,Cyc_Tcutil_terr(loc,((_tmp13C5="unbound field %s in type datatype %s",
_tag_dyneither(_tmp13C5,sizeof(char),37))),_tag_dyneither(_tmp13C4,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpB68=cvtenv;_npop_handler(0);return
_tmpB68;};_LL5AE:;};_pop_handler();}else{void*_tmpB5A=(void*)_exn_thrown;void*
_tmpB6A=_tmpB5A;_LL5B4: {struct Cyc_Dict_Absent_struct*_tmpB6B=(struct Cyc_Dict_Absent_struct*)
_tmpB6A;if(_tmpB6B->tag != Cyc_Dict_Absent)goto _LL5B6;}_LL5B5:{const char*_tmp13CA;
void*_tmp13C9[2];struct Cyc_String_pa_struct _tmp13C8;struct Cyc_String_pa_struct
_tmp13C7;(_tmp13C7.tag=0,((_tmp13C7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4A)),((_tmp13C8.tag=0,((_tmp13C8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4B)),((
_tmp13C9[0]=& _tmp13C8,((_tmp13C9[1]=& _tmp13C7,Cyc_Tcutil_terr(loc,((_tmp13CA="unbound field %s in type datatype %s",
_tag_dyneither(_tmp13CA,sizeof(char),37))),_tag_dyneither(_tmp13C9,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5B6:;_LL5B7:(void)_throw(_tmpB6A);_LL5B3:;}};}*
_tmpAB5=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpB4E=tud;_tmpB4F=tuf;goto _LL5A8;}
_LL5A7: if((_tmpB48.KnownDatatypefield).tag != 2)goto _LL5A4;_tmpB4D=(struct _tuple3)(
_tmpB48.KnownDatatypefield).val;_tmpB4E=_tmpB4D.f1;_tmpB4F=_tmpB4D.f2;_LL5A8: {
struct Cyc_List_List*tvs=_tmpB4E->tvs;for(0;_tmpAB6 != 0  && tvs != 0;(_tmpAB6=
_tmpAB6->tl,tvs=tvs->tl)){void*t=(void*)_tmpAB6->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp13CB;struct _tuple0 _tmpB71=(
_tmp13CB.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp13CB.f2=t,_tmp13CB)));void*
_tmpB72;void*_tmpB74;struct Cyc_Absyn_Tvar*_tmpB76;_LL5B9: _tmpB72=_tmpB71.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpB73=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB72;if(_tmpB73->tag != 1)goto _LL5BB;};_tmpB74=_tmpB71.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB75=(struct Cyc_Absyn_VarType_struct*)_tmpB74;if(_tmpB75->tag != 2)goto _LL5BB;
else{_tmpB76=_tmpB75->f1;}};_LL5BA: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB76);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB76,1);continue;_LL5BB:;_LL5BC: goto _LL5B8;_LL5B8:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpAB6 != 0){const char*_tmp13D0;void*_tmp13CF[2];
struct Cyc_String_pa_struct _tmp13CE;struct Cyc_String_pa_struct _tmp13CD;(_tmp13CD.tag=
0,((_tmp13CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4F->name)),((_tmp13CE.tag=0,((_tmp13CE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4E->name)),((_tmp13CF[0]=& _tmp13CE,((
_tmp13CF[1]=& _tmp13CD,Cyc_Tcutil_terr(loc,((_tmp13D0="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp13D0,sizeof(char),43))),_tag_dyneither(_tmp13CF,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp13D5;void*_tmp13D4[2];struct Cyc_String_pa_struct
_tmp13D3;struct Cyc_String_pa_struct _tmp13D2;(_tmp13D2.tag=0,((_tmp13D2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4F->name)),((
_tmp13D3.tag=0,((_tmp13D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4E->name)),((_tmp13D4[0]=& _tmp13D3,((_tmp13D4[1]=& _tmp13D2,Cyc_Tcutil_terr(
loc,((_tmp13D5="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp13D5,sizeof(char),42))),_tag_dyneither(_tmp13D4,sizeof(void*),2)))))))))))));}
goto _LL5A4;}_LL5A4:;}goto _LL551;_LL560: {struct Cyc_Absyn_PointerType_struct*
_tmpAB7=(struct Cyc_Absyn_PointerType_struct*)_tmpA9D;if(_tmpAB7->tag != 5)goto
_LL562;else{_tmpAB8=_tmpAB7->f1;_tmpAB9=_tmpAB8.elt_typ;_tmpABA=_tmpAB8.elt_tq;
_tmpABB=(struct Cyc_Absyn_Tqual*)&(_tmpAB7->f1).elt_tq;_tmpABC=_tmpAB8.ptr_atts;
_tmpABD=_tmpABC.rgn;_tmpABE=_tmpABC.nullable;_tmpABF=_tmpABC.bounds;_tmpAC0=
_tmpABC.zero_term;}}_LL561: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAB9,1);_tmpABB->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpABB->print_const,_tmpAB9);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5BD: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5BE: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5BF: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpABD,1);{union Cyc_Absyn_Constraint*
_tmpB7F=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpAC0);union Cyc_Absyn_Constraint _tmpB80;int _tmpB81;union Cyc_Absyn_Constraint
_tmpB82;int _tmpB83;_LL5C2: _tmpB80=*_tmpB7F;if((_tmpB80.No_constr).tag != 3)goto
_LL5C4;_tmpB81=(int)(_tmpB80.No_constr).val;_LL5C3:{void*_tmpB84=Cyc_Tcutil_compress(
_tmpAB9);enum Cyc_Absyn_Size_of _tmpB86;_LL5C9: {struct Cyc_Absyn_IntType_struct*
_tmpB85=(struct Cyc_Absyn_IntType_struct*)_tmpB84;if(_tmpB85->tag != 6)goto _LL5CB;
else{_tmpB86=_tmpB85->f2;if(_tmpB86 != Cyc_Absyn_Char_sz)goto _LL5CB;}}_LL5CA:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpAC0,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5C8;_LL5CB:;_LL5CC:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpAC0,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5C8;
_LL5C8:;}goto _LL5C1;_LL5C4: _tmpB82=*_tmpB7F;if((_tmpB82.Eq_constr).tag != 1)goto
_LL5C6;_tmpB83=(int)(_tmpB82.Eq_constr).val;if(_tmpB83 != 1)goto _LL5C6;_LL5C5: if(
!Cyc_Tcutil_admits_zero(_tmpAB9)){const char*_tmp13D9;void*_tmp13D8[1];struct Cyc_String_pa_struct
_tmp13D7;(_tmp13D7.tag=0,((_tmp13D7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAB9)),((_tmp13D8[0]=& _tmp13D7,Cyc_Tcutil_terr(
loc,((_tmp13D9="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp13D9,sizeof(char),53))),_tag_dyneither(_tmp13D8,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5C1;_LL5C6:;_LL5C7: is_zero_terminated=0;
goto _LL5C1;_LL5C1:;}{void*_tmpB8A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpABF);struct Cyc_Absyn_Exp*
_tmpB8D;_LL5CE: {struct Cyc_Absyn_DynEither_b_struct*_tmpB8B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpB8A;if(_tmpB8B->tag != 0)goto _LL5D0;}_LL5CF: goto _LL5CD;_LL5D0: {struct Cyc_Absyn_Upper_b_struct*
_tmpB8C=(struct Cyc_Absyn_Upper_b_struct*)_tmpB8A;if(_tmpB8C->tag != 1)goto _LL5CD;
else{_tmpB8D=_tmpB8C->f1;}}_LL5D1: {struct _RegionHandle*_tmpB8E=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpB8F=Cyc_Tcenv_allow_valueof(_tmpB8E,te);Cyc_Tcexp_tcExp(
_tmpB8F,0,_tmpB8D);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB8D,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB8D)){const char*_tmp13DC;void*
_tmp13DB;(_tmp13DB=0,Cyc_Tcutil_terr(loc,((_tmp13DC="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp13DC,sizeof(char),49))),_tag_dyneither(_tmp13DB,sizeof(void*),
0)));}{unsigned int _tmpB93;int _tmpB94;struct _tuple13 _tmpB92=Cyc_Evexp_eval_const_uint_exp(
_tmpB8D);_tmpB93=_tmpB92.f1;_tmpB94=_tmpB92.f2;if(is_zero_terminated  && (!
_tmpB94  || _tmpB93 < 1)){const char*_tmp13DF;void*_tmp13DE;(_tmp13DE=0,Cyc_Tcutil_terr(
loc,((_tmp13DF="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp13DF,sizeof(char),55))),_tag_dyneither(_tmp13DE,sizeof(void*),
0)));}goto _LL5CD;};}_LL5CD:;}goto _LL551;};}_LL562: {struct Cyc_Absyn_TagType_struct*
_tmpAC1=(struct Cyc_Absyn_TagType_struct*)_tmpA9D;if(_tmpAC1->tag != 20)goto _LL564;
else{_tmpAC2=(void*)_tmpAC1->f1;}}_LL563: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpAC2,1);goto _LL551;_LL564: {struct Cyc_Absyn_ValueofType_struct*
_tmpAC3=(struct Cyc_Absyn_ValueofType_struct*)_tmpA9D;if(_tmpAC3->tag != 19)goto
_LL566;else{_tmpAC4=_tmpAC3->f1;}}_LL565: {struct _RegionHandle*_tmpB97=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpB98=Cyc_Tcenv_allow_valueof(_tmpB97,te);Cyc_Tcexp_tcExp(
_tmpB98,0,_tmpAC4);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAC4)){const char*
_tmp13E2;void*_tmp13E1;(_tmp13E1=0,Cyc_Tcutil_terr(loc,((_tmp13E2="valueof_t requires an int expression",
_tag_dyneither(_tmp13E2,sizeof(char),37))),_tag_dyneither(_tmp13E1,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC4,te,cvtenv);goto
_LL551;}_LL566: {struct Cyc_Absyn_IntType_struct*_tmpAC5=(struct Cyc_Absyn_IntType_struct*)
_tmpA9D;if(_tmpAC5->tag != 6)goto _LL568;}_LL567: goto _LL569;_LL568: {struct Cyc_Absyn_FloatType_struct*
_tmpAC6=(struct Cyc_Absyn_FloatType_struct*)_tmpA9D;if(_tmpAC6->tag != 7)goto
_LL56A;}_LL569: goto _LL56B;_LL56A: {struct Cyc_Absyn_DoubleType_struct*_tmpAC7=(
struct Cyc_Absyn_DoubleType_struct*)_tmpA9D;if(_tmpAC7->tag != 8)goto _LL56C;}
_LL56B: goto _LL551;_LL56C: {struct Cyc_Absyn_ArrayType_struct*_tmpAC8=(struct Cyc_Absyn_ArrayType_struct*)
_tmpA9D;if(_tmpAC8->tag != 9)goto _LL56E;else{_tmpAC9=_tmpAC8->f1;_tmpACA=_tmpAC9.elt_type;
_tmpACB=_tmpAC9.tq;_tmpACC=(struct Cyc_Absyn_Tqual*)&(_tmpAC8->f1).tq;_tmpACD=
_tmpAC9.num_elts;_tmpACE=(struct Cyc_Absyn_Exp**)&(_tmpAC8->f1).num_elts;_tmpACF=
_tmpAC9.zero_term;_tmpAD0=_tmpAC9.zt_loc;}}_LL56D: {struct Cyc_Absyn_Exp*_tmpB9B=*
_tmpACE;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpACA,1);_tmpACC->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpACC->print_const,
_tmpACA);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpB9C=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpACF);union Cyc_Absyn_Constraint
_tmpB9D;int _tmpB9E;union Cyc_Absyn_Constraint _tmpB9F;int _tmpBA0;_LL5D3: _tmpB9D=*
_tmpB9C;if((_tmpB9D.No_constr).tag != 3)goto _LL5D5;_tmpB9E=(int)(_tmpB9D.No_constr).val;
_LL5D4:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpACF,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5D2;_LL5D5: _tmpB9F=*_tmpB9C;if((_tmpB9F.Eq_constr).tag
!= 1)goto _LL5D7;_tmpBA0=(int)(_tmpB9F.Eq_constr).val;if(_tmpBA0 != 1)goto _LL5D7;
_LL5D6: if(!Cyc_Tcutil_admits_zero(_tmpACA)){const char*_tmp13E6;void*_tmp13E5[1];
struct Cyc_String_pa_struct _tmp13E4;(_tmp13E4.tag=0,((_tmp13E4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpACA)),((
_tmp13E5[0]=& _tmp13E4,Cyc_Tcutil_terr(loc,((_tmp13E6="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp13E6,sizeof(char),51))),_tag_dyneither(_tmp13E5,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5D2;_LL5D7:;_LL5D8: is_zero_terminated=0;
goto _LL5D2;_LL5D2:;}if(_tmpB9B == 0){if(is_zero_terminated)*_tmpACE=(_tmpB9B=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp13E9;void*
_tmp13E8;(_tmp13E8=0,Cyc_Tcutil_warn(loc,((_tmp13E9="array bound defaults to 1 here",
_tag_dyneither(_tmp13E9,sizeof(char),31))),_tag_dyneither(_tmp13E8,sizeof(void*),
0)));}*_tmpACE=(_tmpB9B=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpB9B);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpB9B)){const char*_tmp13EC;void*_tmp13EB;(_tmp13EB=0,Cyc_Tcutil_terr(loc,((
_tmp13EC="array bounds expression is not constant",_tag_dyneither(_tmp13EC,
sizeof(char),40))),_tag_dyneither(_tmp13EB,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpB9B)){const char*_tmp13EF;void*_tmp13EE;(_tmp13EE=0,
Cyc_Tcutil_terr(loc,((_tmp13EF="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp13EF,sizeof(char),47))),_tag_dyneither(_tmp13EE,sizeof(void*),
0)));}{unsigned int _tmpBAB;int _tmpBAC;struct _tuple13 _tmpBAA=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpB9B);_tmpBAB=_tmpBAA.f1;_tmpBAC=_tmpBAA.f2;if((
is_zero_terminated  && _tmpBAC) && _tmpBAB < 1){const char*_tmp13F2;void*_tmp13F1;(
_tmp13F1=0,Cyc_Tcutil_warn(loc,((_tmp13F2="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp13F2,sizeof(char),48))),_tag_dyneither(_tmp13F1,sizeof(void*),
0)));}if((_tmpBAC  && _tmpBAB < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp13F5;void*
_tmp13F4;(_tmp13F4=0,Cyc_Tcutil_warn(loc,((_tmp13F5="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp13F5,sizeof(char),75))),_tag_dyneither(_tmp13F4,sizeof(void*),
0)));}*_tmpACE=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL551;};};}
_LL56E: {struct Cyc_Absyn_FnType_struct*_tmpAD1=(struct Cyc_Absyn_FnType_struct*)
_tmpA9D;if(_tmpAD1->tag != 10)goto _LL570;else{_tmpAD2=_tmpAD1->f1;_tmpAD3=_tmpAD2.tvars;
_tmpAD4=(struct Cyc_List_List**)&(_tmpAD1->f1).tvars;_tmpAD5=_tmpAD2.effect;
_tmpAD6=(struct Cyc_Core_Opt**)&(_tmpAD1->f1).effect;_tmpAD7=_tmpAD2.ret_typ;
_tmpAD8=_tmpAD2.args;_tmpAD9=_tmpAD2.c_varargs;_tmpADA=_tmpAD2.cyc_varargs;
_tmpADB=_tmpAD2.rgn_po;_tmpADC=_tmpAD2.attributes;}}_LL56F: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpADC != 0;
_tmpADC=_tmpADC->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpADC->hd)){const char*
_tmp13F9;void*_tmp13F8[1];struct Cyc_String_pa_struct _tmp13F7;(_tmp13F7.tag=0,((
_tmp13F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpADC->hd)),((_tmp13F8[0]=& _tmp13F7,Cyc_Tcutil_terr(loc,((_tmp13F9="bad function type attribute %s",
_tag_dyneither(_tmp13F9,sizeof(char),31))),_tag_dyneither(_tmp13F8,sizeof(void*),
1)))))));}{void*_tmpBB4=(void*)_tmpADC->hd;enum Cyc_Absyn_Format_Type _tmpBB9;int
_tmpBBA;int _tmpBBB;_LL5DA: {struct Cyc_Absyn_Stdcall_att_struct*_tmpBB5=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpBB4;if(_tmpBB5->tag != 1)goto _LL5DC;}_LL5DB: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5D9;_LL5DC: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpBB6=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpBB4;if(_tmpBB6->tag != 2)goto
_LL5DE;}_LL5DD: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5D9;_LL5DE: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpBB7=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpBB4;if(_tmpBB7->tag != 3)goto _LL5E0;}_LL5DF: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5D9;_LL5E0: {struct Cyc_Absyn_Format_att_struct*_tmpBB8=(
struct Cyc_Absyn_Format_att_struct*)_tmpBB4;if(_tmpBB8->tag != 19)goto _LL5E2;else{
_tmpBB9=_tmpBB8->f1;_tmpBBA=_tmpBB8->f2;_tmpBBB=_tmpBB8->f3;}}_LL5E1: if(!
seen_format){seen_format=1;ft=_tmpBB9;fmt_desc_arg=_tmpBBA;fmt_arg_start=_tmpBBB;}
else{const char*_tmp13FC;void*_tmp13FB;(_tmp13FB=0,Cyc_Tcutil_terr(loc,((_tmp13FC="function can't have multiple format attributes",
_tag_dyneither(_tmp13FC,sizeof(char),47))),_tag_dyneither(_tmp13FB,sizeof(void*),
0)));}goto _LL5D9;_LL5E2:;_LL5E3: goto _LL5D9;_LL5D9:;};}if(num_convs > 1){const char*
_tmp13FF;void*_tmp13FE;(_tmp13FE=0,Cyc_Tcutil_terr(loc,((_tmp13FF="function can't have multiple calling conventions",
_tag_dyneither(_tmp13FF,sizeof(char),49))),_tag_dyneither(_tmp13FE,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpAD4);{struct Cyc_List_List*b=*
_tmpAD4;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpBC0=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpBC2;struct Cyc_Absyn_Kind _tmpBC3;enum Cyc_Absyn_KindQual
_tmpBC4;_LL5E5: {struct Cyc_Absyn_Eq_kb_struct*_tmpBC1=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpBC0;if(_tmpBC1->tag != 0)goto _LL5E7;else{_tmpBC2=_tmpBC1->f1;_tmpBC3=*_tmpBC2;
_tmpBC4=_tmpBC3.kind;if(_tmpBC4 != Cyc_Absyn_MemKind)goto _LL5E7;}}_LL5E6:{const
char*_tmp1403;void*_tmp1402[1];struct Cyc_String_pa_struct _tmp1401;(_tmp1401.tag=
0,((_tmp1401.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp1402[0]=& _tmp1401,Cyc_Tcutil_terr(loc,((_tmp1403="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1403,sizeof(char),51))),_tag_dyneither(_tmp1402,sizeof(void*),
1)))))));}goto _LL5E4;_LL5E7:;_LL5E8: goto _LL5E4;_LL5E4:;};}}{struct _RegionHandle*
_tmpBC8=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1404;struct Cyc_Tcutil_CVTEnv
_tmpBC9=(_tmp1404.r=_tmpBC8,((_tmp1404.kind_env=cvtenv.kind_env,((_tmp1404.free_vars=
0,((_tmp1404.free_evars=0,((_tmp1404.generalize_evars=cvtenv.generalize_evars,((
_tmp1404.fn_result=1,_tmp1404)))))))))));_tmpBC9=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC9,& Cyc_Tcutil_tmk,_tmpAD7,1);_tmpBC9.fn_result=0;{struct Cyc_List_List*
a=_tmpAD8;for(0;a != 0;a=a->tl){struct _tuple9*_tmpBCA=(struct _tuple9*)a->hd;void*
_tmpBCB=(*_tmpBCA).f3;_tmpBC9=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpBC9,& Cyc_Tcutil_tmk,
_tmpBCB,1);((*_tmpBCA).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpBCA).f2).print_const,_tmpBCB);}}if(_tmpADA != 0){if(_tmpAD9){const char*
_tmp1407;void*_tmp1406;(_tmp1406=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1407="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp1407,sizeof(char),29))),_tag_dyneither(_tmp1406,sizeof(void*),
0)));}{void*_tmpBCF;int _tmpBD0;struct Cyc_Absyn_VarargInfo _tmpBCE=*_tmpADA;
_tmpBCF=_tmpBCE.type;_tmpBD0=_tmpBCE.inject;_tmpBC9=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC9,& Cyc_Tcutil_tmk,_tmpBCF,1);(_tmpADA->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpADA->tq).print_const,_tmpBCF);if(_tmpBD0){void*_tmpBD1=Cyc_Tcutil_compress(
_tmpBCF);struct Cyc_Absyn_PtrInfo _tmpBD3;void*_tmpBD4;struct Cyc_Absyn_PtrAtts
_tmpBD5;union Cyc_Absyn_Constraint*_tmpBD6;union Cyc_Absyn_Constraint*_tmpBD7;
_LL5EA: {struct Cyc_Absyn_PointerType_struct*_tmpBD2=(struct Cyc_Absyn_PointerType_struct*)
_tmpBD1;if(_tmpBD2->tag != 5)goto _LL5EC;else{_tmpBD3=_tmpBD2->f1;_tmpBD4=_tmpBD3.elt_typ;
_tmpBD5=_tmpBD3.ptr_atts;_tmpBD6=_tmpBD5.bounds;_tmpBD7=_tmpBD5.zero_term;}}
_LL5EB:{void*_tmpBD8=Cyc_Tcutil_compress(_tmpBD4);_LL5EF: {struct Cyc_Absyn_DatatypeType_struct*
_tmpBD9=(struct Cyc_Absyn_DatatypeType_struct*)_tmpBD8;if(_tmpBD9->tag != 3)goto
_LL5F1;}_LL5F0: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpBD7)){const char*_tmp140A;void*_tmp1409;(_tmp1409=0,Cyc_Tcutil_terr(loc,((
_tmp140A="can't inject into a zeroterm pointer",_tag_dyneither(_tmp140A,sizeof(
char),37))),_tag_dyneither(_tmp1409,sizeof(void*),0)));}{void*_tmpBDC=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpBD6);_LL5F4: {struct Cyc_Absyn_DynEither_b_struct*_tmpBDD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBDC;if(_tmpBDD->tag != 0)goto _LL5F6;}_LL5F5:{const char*_tmp140D;void*_tmp140C;(
_tmp140C=0,Cyc_Tcutil_terr(loc,((_tmp140D="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp140D,sizeof(char),44))),_tag_dyneither(_tmp140C,sizeof(void*),
0)));}goto _LL5F3;_LL5F6:;_LL5F7: goto _LL5F3;_LL5F3:;}goto _LL5EE;_LL5F1:;_LL5F2:{
const char*_tmp1410;void*_tmp140F;(_tmp140F=0,Cyc_Tcutil_terr(loc,((_tmp1410="can't inject a non-datatype type",
_tag_dyneither(_tmp1410,sizeof(char),33))),_tag_dyneither(_tmp140F,sizeof(void*),
0)));}goto _LL5EE;_LL5EE:;}goto _LL5E9;_LL5EC:;_LL5ED:{const char*_tmp1413;void*
_tmp1412;(_tmp1412=0,Cyc_Tcutil_terr(loc,((_tmp1413="expecting a datatype pointer type",
_tag_dyneither(_tmp1413,sizeof(char),34))),_tag_dyneither(_tmp1412,sizeof(void*),
0)));}goto _LL5E9;_LL5E9:;}};}if(seen_format){int _tmpBE4=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpAD8);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpBE4) || 
fmt_arg_start < 0) || _tmpADA == 0  && fmt_arg_start != 0) || _tmpADA != 0  && 
fmt_arg_start != _tmpBE4 + 1){const char*_tmp1416;void*_tmp1415;(_tmp1415=0,Cyc_Tcutil_terr(
loc,((_tmp1416="bad format descriptor",_tag_dyneither(_tmp1416,sizeof(char),22))),
_tag_dyneither(_tmp1415,sizeof(void*),0)));}else{void*_tmpBE8;struct _tuple9
_tmpBE7=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpAD8,
fmt_desc_arg - 1);_tmpBE8=_tmpBE7.f3;{void*_tmpBE9=Cyc_Tcutil_compress(_tmpBE8);
struct Cyc_Absyn_PtrInfo _tmpBEB;void*_tmpBEC;struct Cyc_Absyn_PtrAtts _tmpBED;union
Cyc_Absyn_Constraint*_tmpBEE;union Cyc_Absyn_Constraint*_tmpBEF;_LL5F9: {struct
Cyc_Absyn_PointerType_struct*_tmpBEA=(struct Cyc_Absyn_PointerType_struct*)
_tmpBE9;if(_tmpBEA->tag != 5)goto _LL5FB;else{_tmpBEB=_tmpBEA->f1;_tmpBEC=_tmpBEB.elt_typ;
_tmpBED=_tmpBEB.ptr_atts;_tmpBEE=_tmpBED.bounds;_tmpBEF=_tmpBED.zero_term;}}
_LL5FA:{struct _tuple0 _tmp1417;struct _tuple0 _tmpBF1=(_tmp1417.f1=Cyc_Tcutil_compress(
_tmpBEC),((_tmp1417.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpBEE),_tmp1417)));void*_tmpBF2;enum Cyc_Absyn_Sign
_tmpBF4;enum Cyc_Absyn_Size_of _tmpBF5;void*_tmpBF6;_LL5FE: _tmpBF2=_tmpBF1.f1;{
struct Cyc_Absyn_IntType_struct*_tmpBF3=(struct Cyc_Absyn_IntType_struct*)_tmpBF2;
if(_tmpBF3->tag != 6)goto _LL600;else{_tmpBF4=_tmpBF3->f1;if(_tmpBF4 != Cyc_Absyn_None)
goto _LL600;_tmpBF5=_tmpBF3->f2;if(_tmpBF5 != Cyc_Absyn_Char_sz)goto _LL600;}};
_tmpBF6=_tmpBF1.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpBF7=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBF6;if(_tmpBF7->tag != 0)goto _LL600;};_LL5FF: goto _LL5FD;_LL600:;_LL601:{const
char*_tmp141A;void*_tmp1419;(_tmp1419=0,Cyc_Tcutil_terr(loc,((_tmp141A="format descriptor is not a char ? type",
_tag_dyneither(_tmp141A,sizeof(char),39))),_tag_dyneither(_tmp1419,sizeof(void*),
0)));}goto _LL5FD;_LL5FD:;}goto _LL5F8;_LL5FB:;_LL5FC:{const char*_tmp141D;void*
_tmp141C;(_tmp141C=0,Cyc_Tcutil_terr(loc,((_tmp141D="format descriptor is not a char ? type",
_tag_dyneither(_tmp141D,sizeof(char),39))),_tag_dyneither(_tmp141C,sizeof(void*),
0)));}goto _LL5F8;_LL5F8:;}if(fmt_arg_start != 0){int problem;{struct _tuple29
_tmp141E;struct _tuple29 _tmpBFD=(_tmp141E.f1=ft,((_tmp141E.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpADA))->type)),
_tmp141E)));enum Cyc_Absyn_Format_Type _tmpBFE;void*_tmpBFF;struct Cyc_Absyn_DatatypeInfo
_tmpC01;union Cyc_Absyn_DatatypeInfoU _tmpC02;struct Cyc_Absyn_Datatypedecl**
_tmpC03;struct Cyc_Absyn_Datatypedecl*_tmpC04;enum Cyc_Absyn_Format_Type _tmpC05;
void*_tmpC06;struct Cyc_Absyn_DatatypeInfo _tmpC08;union Cyc_Absyn_DatatypeInfoU
_tmpC09;struct Cyc_Absyn_Datatypedecl**_tmpC0A;struct Cyc_Absyn_Datatypedecl*
_tmpC0B;_LL603: _tmpBFE=_tmpBFD.f1;if(_tmpBFE != Cyc_Absyn_Printf_ft)goto _LL605;
_tmpBFF=_tmpBFD.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC00=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpBFF;if(_tmpC00->tag != 3)goto _LL605;else{_tmpC01=_tmpC00->f1;_tmpC02=_tmpC01.datatype_info;
if((_tmpC02.KnownDatatype).tag != 2)goto _LL605;_tmpC03=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC02.KnownDatatype).val;_tmpC04=*_tmpC03;}};_LL604: problem=Cyc_Absyn_qvar_cmp(
_tmpC04->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL602;_LL605: _tmpC05=
_tmpBFD.f1;if(_tmpC05 != Cyc_Absyn_Scanf_ft)goto _LL607;_tmpC06=_tmpBFD.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC07=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC06;if(_tmpC07->tag != 3)goto _LL607;else{_tmpC08=_tmpC07->f1;_tmpC09=_tmpC08.datatype_info;
if((_tmpC09.KnownDatatype).tag != 2)goto _LL607;_tmpC0A=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC09.KnownDatatype).val;_tmpC0B=*_tmpC0A;}};_LL606: problem=Cyc_Absyn_qvar_cmp(
_tmpC0B->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL602;_LL607:;_LL608:
problem=1;goto _LL602;_LL602:;}if(problem){const char*_tmp1421;void*_tmp1420;(
_tmp1420=0,Cyc_Tcutil_terr(loc,((_tmp1421="format attribute and vararg types don't match",
_tag_dyneither(_tmp1421,sizeof(char),46))),_tag_dyneither(_tmp1420,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpADB;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC0F;void*_tmpC10;void*_tmpC11;struct _tuple0*_tmpC0E=(struct _tuple0*)
rpo->hd;_tmpC0F=*_tmpC0E;_tmpC10=_tmpC0F.f1;_tmpC11=_tmpC0F.f2;_tmpBC9=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC9,& Cyc_Tcutil_ek,_tmpC10,1);_tmpBC9=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC9,& Cyc_Tcutil_trk,_tmpC11,1);}}if(*_tmpAD6 != 0)_tmpBC9=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpBC9,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpAD6))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpBC9.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC13;int _tmpC14;struct
_tuple27 _tmpC12=*((struct _tuple27*)tvs->hd);_tmpC13=_tmpC12.f1;_tmpC14=_tmpC12.f2;
if(!_tmpC14)continue;{void*_tmpC15=Cyc_Absyn_compress_kb(_tmpC13->kind);struct
Cyc_Core_Opt*_tmpC17;struct Cyc_Core_Opt**_tmpC18;struct Cyc_Absyn_Kind*_tmpC19;
struct Cyc_Absyn_Kind*_tmpC1B;struct Cyc_Absyn_Kind*_tmpC1D;struct Cyc_Absyn_Kind
_tmpC1E;enum Cyc_Absyn_KindQual _tmpC1F;struct Cyc_Absyn_Kind*_tmpC21;struct Cyc_Absyn_Kind
_tmpC22;enum Cyc_Absyn_KindQual _tmpC23;struct Cyc_Core_Opt*_tmpC25;struct Cyc_Core_Opt**
_tmpC26;struct Cyc_Absyn_Kind*_tmpC27;struct Cyc_Absyn_Kind _tmpC28;enum Cyc_Absyn_KindQual
_tmpC29;struct Cyc_Absyn_Kind*_tmpC2B;struct Cyc_Absyn_Kind _tmpC2C;enum Cyc_Absyn_KindQual
_tmpC2D;struct Cyc_Core_Opt*_tmpC2F;struct Cyc_Core_Opt**_tmpC30;_LL60A:{struct Cyc_Absyn_Less_kb_struct*
_tmpC16=(struct Cyc_Absyn_Less_kb_struct*)_tmpC15;if(_tmpC16->tag != 2)goto _LL60C;
else{_tmpC17=_tmpC16->f1;_tmpC18=(struct Cyc_Core_Opt**)& _tmpC16->f1;_tmpC19=
_tmpC16->f2;}}if(!(_tmpC19->kind == Cyc_Absyn_RgnKind))goto _LL60C;_LL60B: if(
_tmpC19->aliasqual == Cyc_Absyn_Top){*_tmpC18=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpC1B=_tmpC19;goto _LL60D;}*_tmpC18=Cyc_Tcutil_kind_to_bound_opt(_tmpC19);
_tmpC1B=_tmpC19;goto _LL60D;_LL60C:{struct Cyc_Absyn_Eq_kb_struct*_tmpC1A=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC15;if(_tmpC1A->tag != 0)goto _LL60E;else{_tmpC1B=
_tmpC1A->f1;}}if(!(_tmpC1B->kind == Cyc_Absyn_RgnKind))goto _LL60E;_LL60D:{struct
Cyc_Absyn_AccessEff_struct*_tmp1430;struct Cyc_Absyn_VarType_struct*_tmp142F;
struct Cyc_Absyn_VarType_struct _tmp142E;struct Cyc_Absyn_AccessEff_struct _tmp142D;
struct Cyc_List_List*_tmp142C;effect=((_tmp142C=_cycalloc(sizeof(*_tmp142C)),((
_tmp142C->hd=(void*)((_tmp1430=_cycalloc(sizeof(*_tmp1430)),((_tmp1430[0]=((
_tmp142D.tag=23,((_tmp142D.f1=(void*)((void*)((_tmp142F=_cycalloc(sizeof(*
_tmp142F)),((_tmp142F[0]=((_tmp142E.tag=2,((_tmp142E.f1=_tmpC13,_tmp142E)))),
_tmp142F))))),_tmp142D)))),_tmp1430)))),((_tmp142C->tl=effect,_tmp142C))))));}
goto _LL609;_LL60E: {struct Cyc_Absyn_Less_kb_struct*_tmpC1C=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC15;if(_tmpC1C->tag != 2)goto _LL610;else{_tmpC1D=_tmpC1C->f2;_tmpC1E=*_tmpC1D;
_tmpC1F=_tmpC1E.kind;if(_tmpC1F != Cyc_Absyn_IntKind)goto _LL610;}}_LL60F: goto
_LL611;_LL610: {struct Cyc_Absyn_Eq_kb_struct*_tmpC20=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC15;if(_tmpC20->tag != 0)goto _LL612;else{_tmpC21=_tmpC20->f1;_tmpC22=*_tmpC21;
_tmpC23=_tmpC22.kind;if(_tmpC23 != Cyc_Absyn_IntKind)goto _LL612;}}_LL611: goto
_LL609;_LL612: {struct Cyc_Absyn_Less_kb_struct*_tmpC24=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC15;if(_tmpC24->tag != 2)goto _LL614;else{_tmpC25=_tmpC24->f1;_tmpC26=(struct
Cyc_Core_Opt**)& _tmpC24->f1;_tmpC27=_tmpC24->f2;_tmpC28=*_tmpC27;_tmpC29=_tmpC28.kind;
if(_tmpC29 != Cyc_Absyn_EffKind)goto _LL614;}}_LL613:*_tmpC26=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL615;_LL614: {struct Cyc_Absyn_Eq_kb_struct*_tmpC2A=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC15;if(_tmpC2A->tag != 0)goto _LL616;else{_tmpC2B=
_tmpC2A->f1;_tmpC2C=*_tmpC2B;_tmpC2D=_tmpC2C.kind;if(_tmpC2D != Cyc_Absyn_EffKind)
goto _LL616;}}_LL615:{struct Cyc_Absyn_VarType_struct*_tmp1436;struct Cyc_Absyn_VarType_struct
_tmp1435;struct Cyc_List_List*_tmp1434;effect=((_tmp1434=_cycalloc(sizeof(*
_tmp1434)),((_tmp1434->hd=(void*)((_tmp1436=_cycalloc(sizeof(*_tmp1436)),((
_tmp1436[0]=((_tmp1435.tag=2,((_tmp1435.f1=_tmpC13,_tmp1435)))),_tmp1436)))),((
_tmp1434->tl=effect,_tmp1434))))));}goto _LL609;_LL616: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC2E=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC15;if(_tmpC2E->tag != 1)goto
_LL618;else{_tmpC2F=_tmpC2E->f1;_tmpC30=(struct Cyc_Core_Opt**)& _tmpC2E->f1;}}
_LL617:{struct Cyc_Absyn_Less_kb_struct*_tmp143C;struct Cyc_Absyn_Less_kb_struct
_tmp143B;struct Cyc_Core_Opt*_tmp143A;*_tmpC30=((_tmp143A=_cycalloc(sizeof(*
_tmp143A)),((_tmp143A->v=(void*)((_tmp143C=_cycalloc(sizeof(*_tmp143C)),((
_tmp143C[0]=((_tmp143B.tag=2,((_tmp143B.f1=0,((_tmp143B.f2=& Cyc_Tcutil_ak,
_tmp143B)))))),_tmp143C)))),_tmp143A))));}goto _LL619;_LL618:;_LL619:{struct Cyc_Absyn_RgnsEff_struct*
_tmp144B;struct Cyc_Absyn_VarType_struct*_tmp144A;struct Cyc_Absyn_VarType_struct
_tmp1449;struct Cyc_Absyn_RgnsEff_struct _tmp1448;struct Cyc_List_List*_tmp1447;
effect=((_tmp1447=_cycalloc(sizeof(*_tmp1447)),((_tmp1447->hd=(void*)((_tmp144B=
_cycalloc(sizeof(*_tmp144B)),((_tmp144B[0]=((_tmp1448.tag=25,((_tmp1448.f1=(void*)((
void*)((_tmp144A=_cycalloc(sizeof(*_tmp144A)),((_tmp144A[0]=((_tmp1449.tag=2,((
_tmp1449.f1=_tmpC13,_tmp1449)))),_tmp144A))))),_tmp1448)))),_tmp144B)))),((
_tmp1447->tl=effect,_tmp1447))))));}goto _LL609;_LL609:;};}}{struct Cyc_List_List*
ts=_tmpBC9.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpC42;int _tmpC43;struct
_tuple28 _tmpC41=*((struct _tuple28*)ts->hd);_tmpC42=_tmpC41.f1;_tmpC43=_tmpC41.f2;
if(!_tmpC43)continue;{struct Cyc_Absyn_Kind*_tmpC44=Cyc_Tcutil_typ_kind(_tmpC42);
struct Cyc_Absyn_Kind _tmpC45;enum Cyc_Absyn_KindQual _tmpC46;struct Cyc_Absyn_Kind
_tmpC47;enum Cyc_Absyn_KindQual _tmpC48;struct Cyc_Absyn_Kind _tmpC49;enum Cyc_Absyn_KindQual
_tmpC4A;_LL61B: _tmpC45=*_tmpC44;_tmpC46=_tmpC45.kind;if(_tmpC46 != Cyc_Absyn_RgnKind)
goto _LL61D;_LL61C:{struct Cyc_Absyn_AccessEff_struct*_tmp1451;struct Cyc_Absyn_AccessEff_struct
_tmp1450;struct Cyc_List_List*_tmp144F;effect=((_tmp144F=_cycalloc(sizeof(*
_tmp144F)),((_tmp144F->hd=(void*)((_tmp1451=_cycalloc(sizeof(*_tmp1451)),((
_tmp1451[0]=((_tmp1450.tag=23,((_tmp1450.f1=(void*)_tmpC42,_tmp1450)))),_tmp1451)))),((
_tmp144F->tl=effect,_tmp144F))))));}goto _LL61A;_LL61D: _tmpC47=*_tmpC44;_tmpC48=
_tmpC47.kind;if(_tmpC48 != Cyc_Absyn_EffKind)goto _LL61F;_LL61E:{struct Cyc_List_List*
_tmp1452;effect=((_tmp1452=_cycalloc(sizeof(*_tmp1452)),((_tmp1452->hd=_tmpC42,((
_tmp1452->tl=effect,_tmp1452))))));}goto _LL61A;_LL61F: _tmpC49=*_tmpC44;_tmpC4A=
_tmpC49.kind;if(_tmpC4A != Cyc_Absyn_IntKind)goto _LL621;_LL620: goto _LL61A;_LL621:;
_LL622:{struct Cyc_Absyn_RgnsEff_struct*_tmp1458;struct Cyc_Absyn_RgnsEff_struct
_tmp1457;struct Cyc_List_List*_tmp1456;effect=((_tmp1456=_cycalloc(sizeof(*
_tmp1456)),((_tmp1456->hd=(void*)((_tmp1458=_cycalloc(sizeof(*_tmp1458)),((
_tmp1458[0]=((_tmp1457.tag=25,((_tmp1457.f1=(void*)_tmpC42,_tmp1457)))),_tmp1458)))),((
_tmp1456->tl=effect,_tmp1456))))));}goto _LL61A;_LL61A:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp145E;struct Cyc_Absyn_JoinEff_struct _tmp145D;struct Cyc_Core_Opt*_tmp145C;*
_tmpAD6=((_tmp145C=_cycalloc(sizeof(*_tmp145C)),((_tmp145C->v=(void*)((_tmp145E=
_cycalloc(sizeof(*_tmp145E)),((_tmp145E[0]=((_tmp145D.tag=24,((_tmp145D.f1=
effect,_tmp145D)))),_tmp145E)))),_tmp145C))));};}if(*_tmpAD4 != 0){struct Cyc_List_List*
bs=*_tmpAD4;for(0;bs != 0;bs=bs->tl){void*_tmpC55=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpC57;struct Cyc_Core_Opt**
_tmpC58;struct Cyc_Core_Opt*_tmpC5A;struct Cyc_Core_Opt**_tmpC5B;struct Cyc_Absyn_Kind*
_tmpC5C;struct Cyc_Absyn_Kind _tmpC5D;enum Cyc_Absyn_KindQual _tmpC5E;enum Cyc_Absyn_AliasQual
_tmpC5F;struct Cyc_Core_Opt*_tmpC61;struct Cyc_Core_Opt**_tmpC62;struct Cyc_Absyn_Kind*
_tmpC63;struct Cyc_Absyn_Kind _tmpC64;enum Cyc_Absyn_KindQual _tmpC65;enum Cyc_Absyn_AliasQual
_tmpC66;struct Cyc_Core_Opt*_tmpC68;struct Cyc_Core_Opt**_tmpC69;struct Cyc_Absyn_Kind*
_tmpC6A;struct Cyc_Absyn_Kind _tmpC6B;enum Cyc_Absyn_KindQual _tmpC6C;enum Cyc_Absyn_AliasQual
_tmpC6D;struct Cyc_Core_Opt*_tmpC6F;struct Cyc_Core_Opt**_tmpC70;struct Cyc_Absyn_Kind*
_tmpC71;struct Cyc_Absyn_Kind _tmpC72;enum Cyc_Absyn_KindQual _tmpC73;enum Cyc_Absyn_AliasQual
_tmpC74;struct Cyc_Core_Opt*_tmpC76;struct Cyc_Core_Opt**_tmpC77;struct Cyc_Absyn_Kind*
_tmpC78;struct Cyc_Absyn_Kind _tmpC79;enum Cyc_Absyn_KindQual _tmpC7A;enum Cyc_Absyn_AliasQual
_tmpC7B;struct Cyc_Core_Opt*_tmpC7D;struct Cyc_Core_Opt**_tmpC7E;struct Cyc_Absyn_Kind*
_tmpC7F;struct Cyc_Absyn_Kind _tmpC80;enum Cyc_Absyn_KindQual _tmpC81;enum Cyc_Absyn_AliasQual
_tmpC82;struct Cyc_Core_Opt*_tmpC84;struct Cyc_Core_Opt**_tmpC85;struct Cyc_Absyn_Kind*
_tmpC86;struct Cyc_Absyn_Kind _tmpC87;enum Cyc_Absyn_KindQual _tmpC88;enum Cyc_Absyn_AliasQual
_tmpC89;struct Cyc_Core_Opt*_tmpC8B;struct Cyc_Core_Opt**_tmpC8C;struct Cyc_Absyn_Kind*
_tmpC8D;struct Cyc_Absyn_Kind _tmpC8E;enum Cyc_Absyn_KindQual _tmpC8F;enum Cyc_Absyn_AliasQual
_tmpC90;struct Cyc_Core_Opt*_tmpC92;struct Cyc_Core_Opt**_tmpC93;struct Cyc_Absyn_Kind*
_tmpC94;struct Cyc_Absyn_Kind*_tmpC96;struct Cyc_Absyn_Kind _tmpC97;enum Cyc_Absyn_KindQual
_tmpC98;_LL624: {struct Cyc_Absyn_Unknown_kb_struct*_tmpC56=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC55;if(_tmpC56->tag != 1)goto _LL626;else{_tmpC57=_tmpC56->f1;_tmpC58=(struct
Cyc_Core_Opt**)& _tmpC56->f1;}}_LL625:{const char*_tmp1462;void*_tmp1461[1];struct
Cyc_String_pa_struct _tmp1460;(_tmp1460.tag=0,((_tmp1460.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1461[0]=&
_tmp1460,Cyc_Tcutil_warn(loc,((_tmp1462="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1462,sizeof(char),47))),_tag_dyneither(_tmp1461,sizeof(void*),
1)))))));}_tmpC5B=_tmpC58;goto _LL627;_LL626: {struct Cyc_Absyn_Less_kb_struct*
_tmpC59=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;if(_tmpC59->tag != 2)goto _LL628;
else{_tmpC5A=_tmpC59->f1;_tmpC5B=(struct Cyc_Core_Opt**)& _tmpC59->f1;_tmpC5C=
_tmpC59->f2;_tmpC5D=*_tmpC5C;_tmpC5E=_tmpC5D.kind;if(_tmpC5E != Cyc_Absyn_BoxKind)
goto _LL628;_tmpC5F=_tmpC5D.aliasqual;if(_tmpC5F != Cyc_Absyn_Top)goto _LL628;}}
_LL627: _tmpC62=_tmpC5B;goto _LL629;_LL628: {struct Cyc_Absyn_Less_kb_struct*
_tmpC60=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;if(_tmpC60->tag != 2)goto _LL62A;
else{_tmpC61=_tmpC60->f1;_tmpC62=(struct Cyc_Core_Opt**)& _tmpC60->f1;_tmpC63=
_tmpC60->f2;_tmpC64=*_tmpC63;_tmpC65=_tmpC64.kind;if(_tmpC65 != Cyc_Absyn_MemKind)
goto _LL62A;_tmpC66=_tmpC64.aliasqual;if(_tmpC66 != Cyc_Absyn_Top)goto _LL62A;}}
_LL629: _tmpC69=_tmpC62;goto _LL62B;_LL62A: {struct Cyc_Absyn_Less_kb_struct*
_tmpC67=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;if(_tmpC67->tag != 2)goto _LL62C;
else{_tmpC68=_tmpC67->f1;_tmpC69=(struct Cyc_Core_Opt**)& _tmpC67->f1;_tmpC6A=
_tmpC67->f2;_tmpC6B=*_tmpC6A;_tmpC6C=_tmpC6B.kind;if(_tmpC6C != Cyc_Absyn_MemKind)
goto _LL62C;_tmpC6D=_tmpC6B.aliasqual;if(_tmpC6D != Cyc_Absyn_Aliasable)goto _LL62C;}}
_LL62B: _tmpC70=_tmpC69;goto _LL62D;_LL62C: {struct Cyc_Absyn_Less_kb_struct*
_tmpC6E=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;if(_tmpC6E->tag != 2)goto _LL62E;
else{_tmpC6F=_tmpC6E->f1;_tmpC70=(struct Cyc_Core_Opt**)& _tmpC6E->f1;_tmpC71=
_tmpC6E->f2;_tmpC72=*_tmpC71;_tmpC73=_tmpC72.kind;if(_tmpC73 != Cyc_Absyn_AnyKind)
goto _LL62E;_tmpC74=_tmpC72.aliasqual;if(_tmpC74 != Cyc_Absyn_Top)goto _LL62E;}}
_LL62D: _tmpC77=_tmpC70;goto _LL62F;_LL62E: {struct Cyc_Absyn_Less_kb_struct*
_tmpC75=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;if(_tmpC75->tag != 2)goto _LL630;
else{_tmpC76=_tmpC75->f1;_tmpC77=(struct Cyc_Core_Opt**)& _tmpC75->f1;_tmpC78=
_tmpC75->f2;_tmpC79=*_tmpC78;_tmpC7A=_tmpC79.kind;if(_tmpC7A != Cyc_Absyn_AnyKind)
goto _LL630;_tmpC7B=_tmpC79.aliasqual;if(_tmpC7B != Cyc_Absyn_Aliasable)goto _LL630;}}
_LL62F:*_tmpC77=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL623;_LL630: {
struct Cyc_Absyn_Less_kb_struct*_tmpC7C=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;
if(_tmpC7C->tag != 2)goto _LL632;else{_tmpC7D=_tmpC7C->f1;_tmpC7E=(struct Cyc_Core_Opt**)&
_tmpC7C->f1;_tmpC7F=_tmpC7C->f2;_tmpC80=*_tmpC7F;_tmpC81=_tmpC80.kind;if(_tmpC81
!= Cyc_Absyn_MemKind)goto _LL632;_tmpC82=_tmpC80.aliasqual;if(_tmpC82 != Cyc_Absyn_Unique)
goto _LL632;}}_LL631: _tmpC85=_tmpC7E;goto _LL633;_LL632: {struct Cyc_Absyn_Less_kb_struct*
_tmpC83=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;if(_tmpC83->tag != 2)goto _LL634;
else{_tmpC84=_tmpC83->f1;_tmpC85=(struct Cyc_Core_Opt**)& _tmpC83->f1;_tmpC86=
_tmpC83->f2;_tmpC87=*_tmpC86;_tmpC88=_tmpC87.kind;if(_tmpC88 != Cyc_Absyn_AnyKind)
goto _LL634;_tmpC89=_tmpC87.aliasqual;if(_tmpC89 != Cyc_Absyn_Unique)goto _LL634;}}
_LL633:*_tmpC85=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL623;_LL634: {
struct Cyc_Absyn_Less_kb_struct*_tmpC8A=(struct Cyc_Absyn_Less_kb_struct*)_tmpC55;
if(_tmpC8A->tag != 2)goto _LL636;else{_tmpC8B=_tmpC8A->f1;_tmpC8C=(struct Cyc_Core_Opt**)&
_tmpC8A->f1;_tmpC8D=_tmpC8A->f2;_tmpC8E=*_tmpC8D;_tmpC8F=_tmpC8E.kind;if(_tmpC8F
!= Cyc_Absyn_RgnKind)goto _LL636;_tmpC90=_tmpC8E.aliasqual;if(_tmpC90 != Cyc_Absyn_Top)
goto _LL636;}}_LL635:*_tmpC8C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL623;_LL636: {struct Cyc_Absyn_Less_kb_struct*_tmpC91=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC55;if(_tmpC91->tag != 2)goto _LL638;else{_tmpC92=_tmpC91->f1;_tmpC93=(struct
Cyc_Core_Opt**)& _tmpC91->f1;_tmpC94=_tmpC91->f2;}}_LL637:*_tmpC93=Cyc_Tcutil_kind_to_bound_opt(
_tmpC94);goto _LL623;_LL638: {struct Cyc_Absyn_Eq_kb_struct*_tmpC95=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC55;if(_tmpC95->tag != 0)goto _LL63A;else{_tmpC96=_tmpC95->f1;_tmpC97=*_tmpC96;
_tmpC98=_tmpC97.kind;if(_tmpC98 != Cyc_Absyn_MemKind)goto _LL63A;}}_LL639:{const
char*_tmp1465;void*_tmp1464;(_tmp1464=0,Cyc_Tcutil_terr(loc,((_tmp1465="functions can't abstract M types",
_tag_dyneither(_tmp1465,sizeof(char),33))),_tag_dyneither(_tmp1464,sizeof(void*),
0)));}goto _LL623;_LL63A:;_LL63B: goto _LL623;_LL623:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpBC9.kind_env,*_tmpAD4);_tmpBC9.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpBC9.r,_tmpBC9.free_vars,*_tmpAD4);{struct Cyc_List_List*tvs=_tmpBC9.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC9F;int _tmpCA0;struct
_tuple27 _tmpC9E=*((struct _tuple27*)tvs->hd);_tmpC9F=_tmpC9E.f1;_tmpCA0=_tmpC9E.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpC9F,_tmpCA0);}}{struct Cyc_List_List*evs=_tmpBC9.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpCA2;int _tmpCA3;struct _tuple28 _tmpCA1=*((struct _tuple28*)evs->hd);
_tmpCA2=_tmpCA1.f1;_tmpCA3=_tmpCA1.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpCA2,_tmpCA3);}};}goto _LL551;};}_LL570: {struct Cyc_Absyn_TupleType_struct*
_tmpADD=(struct Cyc_Absyn_TupleType_struct*)_tmpA9D;if(_tmpADD->tag != 11)goto
_LL572;else{_tmpADE=_tmpADD->f1;}}_LL571: for(0;_tmpADE != 0;_tmpADE=_tmpADE->tl){
struct _tuple11*_tmpCA5=(struct _tuple11*)_tmpADE->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpCA5).f2,1);((*_tmpCA5).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpCA5).f1).print_const,(*_tmpCA5).f2);}goto _LL551;_LL572: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpADF=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpA9D;if(_tmpADF->tag != 13)goto
_LL574;else{_tmpAE0=_tmpADF->f1;_tmpAE1=_tmpADF->f2;}}_LL573: {struct
_RegionHandle*_tmpCA6=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpAE1 != 0;_tmpAE1=_tmpAE1->tl){struct Cyc_Absyn_Aggrfield _tmpCA8;struct
_dyneither_ptr*_tmpCA9;struct Cyc_Absyn_Tqual _tmpCAA;struct Cyc_Absyn_Tqual*
_tmpCAB;void*_tmpCAC;struct Cyc_Absyn_Exp*_tmpCAD;struct Cyc_List_List*_tmpCAE;
struct Cyc_Absyn_Aggrfield*_tmpCA7=(struct Cyc_Absyn_Aggrfield*)_tmpAE1->hd;
_tmpCA8=*_tmpCA7;_tmpCA9=_tmpCA8.name;_tmpCAA=_tmpCA8.tq;_tmpCAB=(struct Cyc_Absyn_Tqual*)&(*
_tmpCA7).tq;_tmpCAC=_tmpCA8.type;_tmpCAD=_tmpCA8.width;_tmpCAE=_tmpCA8.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpCA9)){
const char*_tmp1469;void*_tmp1468[1];struct Cyc_String_pa_struct _tmp1467;(_tmp1467.tag=
0,((_tmp1467.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA9),((
_tmp1468[0]=& _tmp1467,Cyc_Tcutil_terr(loc,((_tmp1469="duplicate field %s",
_tag_dyneither(_tmp1469,sizeof(char),19))),_tag_dyneither(_tmp1468,sizeof(void*),
1)))))));}{const char*_tmp146A;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpCA9,((
_tmp146A="",_tag_dyneither(_tmp146A,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp146B;prev_fields=((_tmp146B=_region_malloc(_tmpCA6,sizeof(*_tmp146B)),((
_tmp146B->hd=_tmpCA9,((_tmp146B->tl=prev_fields,_tmp146B))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpCAC,1);_tmpCAB->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpCAB->print_const,_tmpCAC);if(_tmpAE0 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpCAC)){const char*_tmp146F;void*_tmp146E[1];struct Cyc_String_pa_struct _tmp146D;(
_tmp146D.tag=0,((_tmp146D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpCA9),((_tmp146E[0]=& _tmp146D,Cyc_Tcutil_warn(loc,((_tmp146F="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp146F,sizeof(char),74))),_tag_dyneither(_tmp146E,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpCAC,_tmpCAD,_tmpCA9);Cyc_Tcutil_check_field_atts(
loc,_tmpCA9,_tmpCAE);}}goto _LL551;}_LL574: {struct Cyc_Absyn_AggrType_struct*
_tmpAE2=(struct Cyc_Absyn_AggrType_struct*)_tmpA9D;if(_tmpAE2->tag != 12)goto
_LL576;else{_tmpAE3=_tmpAE2->f1;_tmpAE4=_tmpAE3.aggr_info;_tmpAE5=(union Cyc_Absyn_AggrInfoU*)&(
_tmpAE2->f1).aggr_info;_tmpAE6=_tmpAE3.targs;_tmpAE7=(struct Cyc_List_List**)&(
_tmpAE2->f1).targs;}}_LL575:{union Cyc_Absyn_AggrInfoU _tmpCB7=*_tmpAE5;struct
_tuple4 _tmpCB8;enum Cyc_Absyn_AggrKind _tmpCB9;struct _tuple2*_tmpCBA;struct Cyc_Core_Opt*
_tmpCBB;struct Cyc_Absyn_Aggrdecl**_tmpCBC;struct Cyc_Absyn_Aggrdecl*_tmpCBD;
_LL63D: if((_tmpCB7.UnknownAggr).tag != 1)goto _LL63F;_tmpCB8=(struct _tuple4)(
_tmpCB7.UnknownAggr).val;_tmpCB9=_tmpCB8.f1;_tmpCBA=_tmpCB8.f2;_tmpCBB=_tmpCB8.f3;
_LL63E: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpCBE;_push_handler(&
_tmpCBE);{int _tmpCC0=0;if(setjmp(_tmpCBE.handler))_tmpCC0=1;if(!_tmpCC0){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpCBA);{struct Cyc_Absyn_Aggrdecl*_tmpCC1=*adp;if(_tmpCC1->kind != 
_tmpCB9){if(_tmpCC1->kind == Cyc_Absyn_StructA){const char*_tmp1474;void*_tmp1473[
2];struct Cyc_String_pa_struct _tmp1472;struct Cyc_String_pa_struct _tmp1471;(
_tmp1471.tag=0,((_tmp1471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCBA)),((_tmp1472.tag=0,((_tmp1472.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCBA)),((_tmp1473[0]=& _tmp1472,((
_tmp1473[1]=& _tmp1471,Cyc_Tcutil_terr(loc,((_tmp1474="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1474,sizeof(char),40))),_tag_dyneither(_tmp1473,sizeof(void*),
2)))))))))))));}else{const char*_tmp1479;void*_tmp1478[2];struct Cyc_String_pa_struct
_tmp1477;struct Cyc_String_pa_struct _tmp1476;(_tmp1476.tag=0,((_tmp1476.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCBA)),((
_tmp1477.tag=0,((_tmp1477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCBA)),((_tmp1478[0]=& _tmp1477,((_tmp1478[1]=& _tmp1476,Cyc_Tcutil_terr(loc,((
_tmp1479="expecting union %s instead of struct %s",_tag_dyneither(_tmp1479,
sizeof(char),40))),_tag_dyneither(_tmp1478,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpCBB  && (int)_tmpCBB->v){if(!((unsigned int)_tmpCC1->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCC1->impl))->tagged){const char*
_tmp147D;void*_tmp147C[1];struct Cyc_String_pa_struct _tmp147B;(_tmp147B.tag=0,((
_tmp147B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCBA)),((_tmp147C[0]=& _tmp147B,Cyc_Tcutil_terr(loc,((_tmp147D="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp147D,sizeof(char),42))),_tag_dyneither(_tmp147C,sizeof(void*),
1)))))));}}*_tmpAE5=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpCBF=(void*)_exn_thrown;void*_tmpCCE=_tmpCBF;_LL642: {struct Cyc_Dict_Absent_struct*
_tmpCCF=(struct Cyc_Dict_Absent_struct*)_tmpCCE;if(_tmpCCF->tag != Cyc_Dict_Absent)
goto _LL644;}_LL643: {struct Cyc_Tcenv_Genv*_tmpCD0=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp147E;struct Cyc_Absyn_Aggrdecl*_tmpCD1=(_tmp147E=
_cycalloc(sizeof(*_tmp147E)),((_tmp147E->kind=_tmpCB9,((_tmp147E->sc=Cyc_Absyn_Extern,((
_tmp147E->name=_tmpCBA,((_tmp147E->tvs=0,((_tmp147E->impl=0,((_tmp147E->attributes=
0,_tmp147E)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpCD0,loc,_tmpCD1);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpCBA);*_tmpAE5=Cyc_Absyn_KnownAggr(adp);if(*_tmpAE7 != 0){{const char*
_tmp1482;void*_tmp1481[1];struct Cyc_String_pa_struct _tmp1480;(_tmp1480.tag=0,((
_tmp1480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCBA)),((_tmp1481[0]=& _tmp1480,Cyc_Tcutil_terr(loc,((_tmp1482="declare parameterized type %s before using",
_tag_dyneither(_tmp1482,sizeof(char),43))),_tag_dyneither(_tmp1481,sizeof(void*),
1)))))));}return cvtenv;}goto _LL641;}_LL644:;_LL645:(void)_throw(_tmpCCE);_LL641:;}};}
_tmpCBD=*adp;goto _LL640;}_LL63F: if((_tmpCB7.KnownAggr).tag != 2)goto _LL63C;
_tmpCBC=(struct Cyc_Absyn_Aggrdecl**)(_tmpCB7.KnownAggr).val;_tmpCBD=*_tmpCBC;
_LL640: {struct Cyc_List_List*tvs=_tmpCBD->tvs;struct Cyc_List_List*ts=*_tmpAE7;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpCD6=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpCD7=(void*)ts->hd;{struct _tuple0 _tmp1483;
struct _tuple0 _tmpCD9=(_tmp1483.f1=Cyc_Absyn_compress_kb(_tmpCD6->kind),((
_tmp1483.f2=_tmpCD7,_tmp1483)));void*_tmpCDA;void*_tmpCDC;struct Cyc_Absyn_Tvar*
_tmpCDE;_LL647: _tmpCDA=_tmpCD9.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpCDB=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpCDA;if(_tmpCDB->tag != 1)goto _LL649;};
_tmpCDC=_tmpCD9.f2;{struct Cyc_Absyn_VarType_struct*_tmpCDD=(struct Cyc_Absyn_VarType_struct*)
_tmpCDC;if(_tmpCDD->tag != 2)goto _LL649;else{_tmpCDE=_tmpCDD->f1;}};_LL648: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpCDE);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpCDE,1);continue;_LL649:;_LL64A: goto _LL646;_LL646:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp1487;void*_tmp1486[1];struct Cyc_String_pa_struct _tmp1485;(
_tmp1485.tag=0,((_tmp1485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpCBD->name)),((_tmp1486[0]=& _tmp1485,Cyc_Tcutil_terr(loc,((_tmp1487="too many parameters for type %s",
_tag_dyneither(_tmp1487,sizeof(char),32))),_tag_dyneither(_tmp1486,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1488;hidden_ts=((_tmp1488=_cycalloc(sizeof(*_tmp1488)),((_tmp1488->hd=e,((
_tmp1488->tl=hidden_ts,_tmp1488))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k,e,1);}*_tmpAE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAE7,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL63C:;}goto _LL551;
_LL576: {struct Cyc_Absyn_TypedefType_struct*_tmpAE8=(struct Cyc_Absyn_TypedefType_struct*)
_tmpA9D;if(_tmpAE8->tag != 18)goto _LL578;else{_tmpAE9=_tmpAE8->f1;_tmpAEA=_tmpAE8->f2;
_tmpAEB=(struct Cyc_List_List**)& _tmpAE8->f2;_tmpAEC=_tmpAE8->f3;_tmpAED=(struct
Cyc_Absyn_Typedefdecl**)& _tmpAE8->f3;_tmpAEE=_tmpAE8->f4;_tmpAEF=(void***)&
_tmpAE8->f4;}}_LL577: {struct Cyc_List_List*targs=*_tmpAEB;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpCE3;_push_handler(& _tmpCE3);{int _tmpCE5=0;if(setjmp(
_tmpCE3.handler))_tmpCE5=1;if(!_tmpCE5){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpAE9);;_pop_handler();}else{void*_tmpCE4=(void*)_exn_thrown;void*_tmpCE7=
_tmpCE4;_LL64C: {struct Cyc_Dict_Absent_struct*_tmpCE8=(struct Cyc_Dict_Absent_struct*)
_tmpCE7;if(_tmpCE8->tag != Cyc_Dict_Absent)goto _LL64E;}_LL64D:{const char*_tmp148C;
void*_tmp148B[1];struct Cyc_String_pa_struct _tmp148A;(_tmp148A.tag=0,((_tmp148A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAE9)),((
_tmp148B[0]=& _tmp148A,Cyc_Tcutil_terr(loc,((_tmp148C="unbound typedef name %s",
_tag_dyneither(_tmp148C,sizeof(char),24))),_tag_dyneither(_tmp148B,sizeof(void*),
1)))))));}return cvtenv;_LL64E:;_LL64F:(void)_throw(_tmpCE7);_LL64B:;}};}*_tmpAED=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpAE9[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpCEC=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple15*_tmp148F;struct Cyc_List_List*
_tmp148E;inst=((_tmp148E=_region_malloc(_tmpCEC,sizeof(*_tmp148E)),((_tmp148E->hd=((
_tmp148F=_region_malloc(_tmpCEC,sizeof(*_tmp148F)),((_tmp148F->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp148F->f2=(void*)ts->hd,_tmp148F)))))),((_tmp148E->tl=inst,_tmp148E))))));};}
if(ts != 0){const char*_tmp1493;void*_tmp1492[1];struct Cyc_String_pa_struct
_tmp1491;(_tmp1491.tag=0,((_tmp1491.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAE9)),((_tmp1492[0]=& _tmp1491,Cyc_Tcutil_terr(
loc,((_tmp1493="too many parameters for typedef %s",_tag_dyneither(_tmp1493,
sizeof(char),35))),_tag_dyneither(_tmp1492,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp1494;hidden_ts=((_tmp1494=_cycalloc(sizeof(*_tmp1494)),((
_tmp1494->hd=e,((_tmp1494->tl=hidden_ts,_tmp1494))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp1497;struct Cyc_List_List*_tmp1496;inst=(
struct Cyc_List_List*)((_tmp1496=_cycalloc(sizeof(*_tmp1496)),((_tmp1496->hd=(
struct _tuple15*)((_tmp1497=_cycalloc(sizeof(*_tmp1497)),((_tmp1497->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1497->f2=e,_tmp1497)))))),((_tmp1496->tl=inst,
_tmp1496))))));};}*_tmpAEB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpCEC,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1498;*_tmpAEF=((_tmp1498=_cycalloc(sizeof(*
_tmp1498)),((_tmp1498[0]=new_typ,_tmp1498))));}}goto _LL551;};}_LL578: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpAF0=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpA9D;if(_tmpAF0->tag != 22)goto
_LL57A;}_LL579: goto _LL57B;_LL57A: {struct Cyc_Absyn_HeapRgn_struct*_tmpAF1=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpA9D;if(_tmpAF1->tag != 21)goto _LL57C;}_LL57B:
goto _LL551;_LL57C: {struct Cyc_Absyn_RgnHandleType_struct*_tmpAF2=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpA9D;if(_tmpAF2->tag != 16)goto _LL57E;else{_tmpAF3=(void*)_tmpAF2->f1;}}_LL57D:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpAF3,1);
goto _LL551;_LL57E: {struct Cyc_Absyn_DynRgnType_struct*_tmpAF4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpA9D;if(_tmpAF4->tag != 17)goto _LL580;else{_tmpAF5=(void*)_tmpAF4->f1;_tmpAF6=(
void*)_tmpAF4->f2;}}_LL57F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpAF5,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpAF6,1);goto _LL551;_LL580: {struct Cyc_Absyn_AccessEff_struct*_tmpAF7=(struct
Cyc_Absyn_AccessEff_struct*)_tmpA9D;if(_tmpAF7->tag != 23)goto _LL582;else{_tmpAF8=(
void*)_tmpAF7->f1;}}_LL581: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpAF8,1);goto _LL551;_LL582: {struct Cyc_Absyn_RgnsEff_struct*
_tmpAF9=(struct Cyc_Absyn_RgnsEff_struct*)_tmpA9D;if(_tmpAF9->tag != 25)goto _LL584;
else{_tmpAFA=(void*)_tmpAF9->f1;}}_LL583: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAFA,1);goto _LL551;_LL584: {struct Cyc_Absyn_JoinEff_struct*
_tmpAFB=(struct Cyc_Absyn_JoinEff_struct*)_tmpA9D;if(_tmpAFB->tag != 24)goto _LL551;
else{_tmpAFC=_tmpAFB->f1;}}_LL585: for(0;_tmpAFC != 0;_tmpAFC=_tmpAFC->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpAFC->hd,1);}
goto _LL551;_LL551:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpCF6=t;struct Cyc_Core_Opt*_tmpCF8;struct Cyc_Core_Opt*_tmpCF9;_LL651: {
struct Cyc_Absyn_Evar_struct*_tmpCF7=(struct Cyc_Absyn_Evar_struct*)_tmpCF6;if(
_tmpCF7->tag != 1)goto _LL653;else{_tmpCF8=_tmpCF7->f1;_tmpCF9=_tmpCF7->f2;}}
_LL652: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpCFA=_tmpCF8;struct Cyc_Core_Opt
_tmpCFB;struct Cyc_Absyn_Kind*_tmpCFC;_LL656: if(_tmpCFA != 0)goto _LL658;_LL657:{
const char*_tmp1499;s=((_tmp1499="kind=NULL ",_tag_dyneither(_tmp1499,sizeof(char),
11)));}goto _LL655;_LL658: if(_tmpCFA == 0)goto _LL655;_tmpCFB=*_tmpCFA;_tmpCFC=(
struct Cyc_Absyn_Kind*)_tmpCFB.v;_LL659:{const char*_tmp149D;void*_tmp149C[1];
struct Cyc_String_pa_struct _tmp149B;s=(struct _dyneither_ptr)((_tmp149B.tag=0,((
_tmp149B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpCFC)),((_tmp149C[0]=& _tmp149B,Cyc_aprintf(((_tmp149D="kind=%s ",
_tag_dyneither(_tmp149D,sizeof(char),9))),_tag_dyneither(_tmp149C,sizeof(void*),
1))))))));}goto _LL655;_LL655:;}{struct Cyc_Core_Opt*_tmpD01=_tmpCF9;struct Cyc_Core_Opt
_tmpD02;void*_tmpD03;_LL65B: if(_tmpD01 != 0)goto _LL65D;_LL65C:{const char*_tmp14A1;
void*_tmp14A0[1];struct Cyc_String_pa_struct _tmp149F;s=(struct _dyneither_ptr)((
_tmp149F.tag=0,((_tmp149F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp14A0[0]=& _tmp149F,Cyc_aprintf(((_tmp14A1="%s ref=NULL",_tag_dyneither(
_tmp14A1,sizeof(char),12))),_tag_dyneither(_tmp14A0,sizeof(void*),1))))))));}
goto _LL65A;_LL65D: if(_tmpD01 == 0)goto _LL65A;_tmpD02=*_tmpD01;_tmpD03=(void*)
_tmpD02.v;_LL65E:{const char*_tmp14A6;void*_tmp14A5[2];struct Cyc_String_pa_struct
_tmp14A4;struct Cyc_String_pa_struct _tmp14A3;s=(struct _dyneither_ptr)((_tmp14A3.tag=
0,((_tmp14A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpD03)),((_tmp14A4.tag=0,((_tmp14A4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp14A5[0]=& _tmp14A4,((_tmp14A5[1]=& _tmp14A3,Cyc_aprintf(((
_tmp14A6="%s ref=%s",_tag_dyneither(_tmp14A6,sizeof(char),10))),_tag_dyneither(
_tmp14A5,sizeof(void*),2))))))))))))));}goto _LL65A;_LL65A:;}{const char*_tmp14AA;
void*_tmp14A9[1];struct Cyc_String_pa_struct _tmp14A8;(_tmp14A8.tag=0,((_tmp14A8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp14A9[0]=& _tmp14A8,Cyc_fprintf(
Cyc_stderr,((_tmp14AA="evar info: %s\n",_tag_dyneither(_tmp14AA,sizeof(char),15))),
_tag_dyneither(_tmp14A9,sizeof(void*),1)))))));}goto _LL650;}_LL653:;_LL654: goto
_LL650;_LL650:;}{const char*_tmp14B0;void*_tmp14AF[3];struct Cyc_String_pa_struct
_tmp14AE;struct Cyc_String_pa_struct _tmp14AD;struct Cyc_String_pa_struct _tmp14AC;(
_tmp14AC.tag=0,((_tmp14AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp14AD.tag=0,((_tmp14AD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp14AE.tag=0,((
_tmp14AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp14AF[0]=& _tmp14AE,((_tmp14AF[1]=& _tmp14AD,((_tmp14AF[2]=& _tmp14AC,Cyc_Tcutil_terr(
loc,((_tmp14B0="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp14B0,sizeof(char),51))),_tag_dyneither(_tmp14AF,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpD13=e->r;struct Cyc_List_List*_tmpD19;struct Cyc_Absyn_Exp*_tmpD1B;struct
Cyc_Absyn_Exp*_tmpD1C;struct Cyc_Absyn_Exp*_tmpD1D;struct Cyc_Absyn_Exp*_tmpD1F;
struct Cyc_Absyn_Exp*_tmpD20;struct Cyc_Absyn_Exp*_tmpD22;struct Cyc_Absyn_Exp*
_tmpD23;struct Cyc_Absyn_Exp*_tmpD25;struct Cyc_Absyn_Exp*_tmpD26;void*_tmpD28;
struct Cyc_Absyn_Exp*_tmpD29;void*_tmpD2B;void*_tmpD2D;void*_tmpD2F;struct Cyc_Absyn_Exp*
_tmpD31;_LL660: {struct Cyc_Absyn_Const_e_struct*_tmpD14=(struct Cyc_Absyn_Const_e_struct*)
_tmpD13;if(_tmpD14->tag != 0)goto _LL662;}_LL661: goto _LL663;_LL662: {struct Cyc_Absyn_Enum_e_struct*
_tmpD15=(struct Cyc_Absyn_Enum_e_struct*)_tmpD13;if(_tmpD15->tag != 32)goto _LL664;}
_LL663: goto _LL665;_LL664: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD16=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD13;if(_tmpD16->tag != 33)goto _LL666;}_LL665: goto _LL667;_LL666: {struct Cyc_Absyn_Var_e_struct*
_tmpD17=(struct Cyc_Absyn_Var_e_struct*)_tmpD13;if(_tmpD17->tag != 1)goto _LL668;}
_LL667: goto _LL65F;_LL668: {struct Cyc_Absyn_Primop_e_struct*_tmpD18=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD13;if(_tmpD18->tag != 3)goto _LL66A;else{_tmpD19=_tmpD18->f2;}}_LL669: for(0;
_tmpD19 != 0;_tmpD19=_tmpD19->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpD19->hd,te,cvtenv);}goto _LL65F;_LL66A: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD1A=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD13;if(_tmpD1A->tag != 6)goto
_LL66C;else{_tmpD1B=_tmpD1A->f1;_tmpD1C=_tmpD1A->f2;_tmpD1D=_tmpD1A->f3;}}_LL66B:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD1B,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD1C,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD1D,te,
cvtenv);goto _LL65F;_LL66C: {struct Cyc_Absyn_And_e_struct*_tmpD1E=(struct Cyc_Absyn_And_e_struct*)
_tmpD13;if(_tmpD1E->tag != 7)goto _LL66E;else{_tmpD1F=_tmpD1E->f1;_tmpD20=_tmpD1E->f2;}}
_LL66D: _tmpD22=_tmpD1F;_tmpD23=_tmpD20;goto _LL66F;_LL66E: {struct Cyc_Absyn_Or_e_struct*
_tmpD21=(struct Cyc_Absyn_Or_e_struct*)_tmpD13;if(_tmpD21->tag != 8)goto _LL670;
else{_tmpD22=_tmpD21->f1;_tmpD23=_tmpD21->f2;}}_LL66F: _tmpD25=_tmpD22;_tmpD26=
_tmpD23;goto _LL671;_LL670: {struct Cyc_Absyn_SeqExp_e_struct*_tmpD24=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD13;if(_tmpD24->tag != 9)goto _LL672;else{_tmpD25=_tmpD24->f1;_tmpD26=_tmpD24->f2;}}
_LL671: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD25,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD26,te,cvtenv);goto _LL65F;_LL672: {
struct Cyc_Absyn_Cast_e_struct*_tmpD27=(struct Cyc_Absyn_Cast_e_struct*)_tmpD13;
if(_tmpD27->tag != 15)goto _LL674;else{_tmpD28=(void*)_tmpD27->f1;_tmpD29=_tmpD27->f2;}}
_LL673: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD29,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpD28,1);goto
_LL65F;_LL674: {struct Cyc_Absyn_Offsetof_e_struct*_tmpD2A=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD13;if(_tmpD2A->tag != 20)goto _LL676;else{_tmpD2B=(void*)_tmpD2A->f1;}}_LL675:
_tmpD2D=_tmpD2B;goto _LL677;_LL676: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpD2C=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD13;if(_tmpD2C->tag != 18)goto _LL678;
else{_tmpD2D=(void*)_tmpD2C->f1;}}_LL677: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpD2D,1);goto _LL65F;_LL678: {struct Cyc_Absyn_Valueof_e_struct*
_tmpD2E=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD13;if(_tmpD2E->tag != 39)goto
_LL67A;else{_tmpD2F=(void*)_tmpD2E->f1;}}_LL679: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpD2F,1);goto _LL65F;_LL67A: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD30=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD13;if(_tmpD30->tag != 19)goto
_LL67C;else{_tmpD31=_tmpD30->f1;}}_LL67B: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD31,te,cvtenv);goto _LL65F;_LL67C:;_LL67D: {const char*_tmp14B3;void*_tmp14B2;(
_tmp14B2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp14B3="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp14B3,sizeof(char),66))),_tag_dyneither(_tmp14B2,sizeof(void*),
0)));}_LL65F:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpD34=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpD36;struct
_tuple27 _tmpD35=*((struct _tuple27*)vs->hd);_tmpD36=_tmpD35.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpD34,_tmpD36);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpD38;struct _tuple28 _tmpD37=*((struct _tuple28*)evs->hd);_tmpD38=_tmpD37.f1;{
void*_tmpD39=Cyc_Tcutil_compress(_tmpD38);struct Cyc_Core_Opt*_tmpD3B;struct Cyc_Core_Opt**
_tmpD3C;_LL67F: {struct Cyc_Absyn_Evar_struct*_tmpD3A=(struct Cyc_Absyn_Evar_struct*)
_tmpD39;if(_tmpD3A->tag != 1)goto _LL681;else{_tmpD3B=_tmpD3A->f4;_tmpD3C=(struct
Cyc_Core_Opt**)& _tmpD3A->f4;}}_LL680: if(*_tmpD3C == 0){struct Cyc_Core_Opt*
_tmp14B4;*_tmpD3C=((_tmp14B4=_cycalloc(sizeof(*_tmp14B4)),((_tmp14B4->v=_tmpD34,
_tmp14B4))));}else{struct Cyc_List_List*_tmpD3E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpD3C))->v;struct Cyc_List_List*_tmpD3F=0;for(0;_tmpD3E != 0;
_tmpD3E=_tmpD3E->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpD34,(struct Cyc_Absyn_Tvar*)_tmpD3E->hd)){struct Cyc_List_List*_tmp14B5;
_tmpD3F=((_tmp14B5=_cycalloc(sizeof(*_tmp14B5)),((_tmp14B5->hd=(struct Cyc_Absyn_Tvar*)
_tmpD3E->hd,((_tmp14B5->tl=_tmpD3F,_tmp14B5))))));}}{struct Cyc_Core_Opt*_tmp14B6;*
_tmpD3C=((_tmp14B6=_cycalloc(sizeof(*_tmp14B6)),((_tmp14B6->v=_tmpD3F,_tmp14B6))));};}
goto _LL67E;_LL681:;_LL682: goto _LL67E;_LL67E:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpD42=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpD43=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp14B7;struct Cyc_Tcutil_CVTEnv _tmpD44=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp14B7.r=_tmpD43,((_tmp14B7.kind_env=_tmpD42,((_tmp14B7.free_vars=0,((_tmp14B7.free_evars=
0,((_tmp14B7.generalize_evars=generalize_evars,((_tmp14B7.fn_result=0,_tmp14B7)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpD45=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple27*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpD43,(struct Cyc_Absyn_Tvar*(*)(struct _tuple27*))Cyc_Core_fst,
_tmpD44.free_vars);struct Cyc_List_List*_tmpD46=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpD43,(void*(*)(struct _tuple28*))Cyc_Core_fst,_tmpD44.free_evars);if(_tmpD42 != 
0){struct Cyc_List_List*_tmpD47=0;{struct Cyc_List_List*_tmpD48=_tmpD45;for(0;(
unsigned int)_tmpD48;_tmpD48=_tmpD48->tl){struct Cyc_Absyn_Tvar*_tmpD49=(struct
Cyc_Absyn_Tvar*)_tmpD48->hd;int found=0;{struct Cyc_List_List*_tmpD4A=_tmpD42;for(
0;(unsigned int)_tmpD4A;_tmpD4A=_tmpD4A->tl){if(Cyc_Absyn_tvar_cmp(_tmpD49,(
struct Cyc_Absyn_Tvar*)_tmpD4A->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp14B8;_tmpD47=((_tmp14B8=_region_malloc(_tmpD43,sizeof(*_tmp14B8)),((_tmp14B8->hd=(
struct Cyc_Absyn_Tvar*)_tmpD48->hd,((_tmp14B8->tl=_tmpD47,_tmp14B8))))));}}}
_tmpD45=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD47);}{struct Cyc_List_List*x=_tmpD45;for(0;x != 0;x=x->tl){void*_tmpD4C=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpD4E;struct Cyc_Core_Opt**
_tmpD4F;struct Cyc_Core_Opt*_tmpD51;struct Cyc_Core_Opt**_tmpD52;struct Cyc_Absyn_Kind*
_tmpD53;struct Cyc_Absyn_Kind _tmpD54;enum Cyc_Absyn_KindQual _tmpD55;enum Cyc_Absyn_AliasQual
_tmpD56;struct Cyc_Core_Opt*_tmpD58;struct Cyc_Core_Opt**_tmpD59;struct Cyc_Absyn_Kind*
_tmpD5A;struct Cyc_Absyn_Kind _tmpD5B;enum Cyc_Absyn_KindQual _tmpD5C;enum Cyc_Absyn_AliasQual
_tmpD5D;struct Cyc_Core_Opt*_tmpD5F;struct Cyc_Core_Opt**_tmpD60;struct Cyc_Absyn_Kind*
_tmpD61;struct Cyc_Absyn_Kind _tmpD62;enum Cyc_Absyn_KindQual _tmpD63;enum Cyc_Absyn_AliasQual
_tmpD64;struct Cyc_Core_Opt*_tmpD66;struct Cyc_Core_Opt**_tmpD67;struct Cyc_Absyn_Kind*
_tmpD68;struct Cyc_Absyn_Kind _tmpD69;enum Cyc_Absyn_KindQual _tmpD6A;enum Cyc_Absyn_AliasQual
_tmpD6B;struct Cyc_Core_Opt*_tmpD6D;struct Cyc_Core_Opt**_tmpD6E;struct Cyc_Absyn_Kind*
_tmpD6F;struct Cyc_Absyn_Kind _tmpD70;enum Cyc_Absyn_KindQual _tmpD71;enum Cyc_Absyn_AliasQual
_tmpD72;struct Cyc_Core_Opt*_tmpD74;struct Cyc_Core_Opt**_tmpD75;struct Cyc_Absyn_Kind*
_tmpD76;struct Cyc_Absyn_Kind*_tmpD78;struct Cyc_Absyn_Kind _tmpD79;enum Cyc_Absyn_KindQual
_tmpD7A;enum Cyc_Absyn_AliasQual _tmpD7B;_LL684: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpD4D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpD4C;if(_tmpD4D->tag != 1)goto
_LL686;else{_tmpD4E=_tmpD4D->f1;_tmpD4F=(struct Cyc_Core_Opt**)& _tmpD4D->f1;}}
_LL685: _tmpD52=_tmpD4F;goto _LL687;_LL686: {struct Cyc_Absyn_Less_kb_struct*
_tmpD50=(struct Cyc_Absyn_Less_kb_struct*)_tmpD4C;if(_tmpD50->tag != 2)goto _LL688;
else{_tmpD51=_tmpD50->f1;_tmpD52=(struct Cyc_Core_Opt**)& _tmpD50->f1;_tmpD53=
_tmpD50->f2;_tmpD54=*_tmpD53;_tmpD55=_tmpD54.kind;if(_tmpD55 != Cyc_Absyn_BoxKind)
goto _LL688;_tmpD56=_tmpD54.aliasqual;if(_tmpD56 != Cyc_Absyn_Top)goto _LL688;}}
_LL687: _tmpD59=_tmpD52;goto _LL689;_LL688: {struct Cyc_Absyn_Less_kb_struct*
_tmpD57=(struct Cyc_Absyn_Less_kb_struct*)_tmpD4C;if(_tmpD57->tag != 2)goto _LL68A;
else{_tmpD58=_tmpD57->f1;_tmpD59=(struct Cyc_Core_Opt**)& _tmpD57->f1;_tmpD5A=
_tmpD57->f2;_tmpD5B=*_tmpD5A;_tmpD5C=_tmpD5B.kind;if(_tmpD5C != Cyc_Absyn_MemKind)
goto _LL68A;_tmpD5D=_tmpD5B.aliasqual;if(_tmpD5D != Cyc_Absyn_Top)goto _LL68A;}}
_LL689: _tmpD60=_tmpD59;goto _LL68B;_LL68A: {struct Cyc_Absyn_Less_kb_struct*
_tmpD5E=(struct Cyc_Absyn_Less_kb_struct*)_tmpD4C;if(_tmpD5E->tag != 2)goto _LL68C;
else{_tmpD5F=_tmpD5E->f1;_tmpD60=(struct Cyc_Core_Opt**)& _tmpD5E->f1;_tmpD61=
_tmpD5E->f2;_tmpD62=*_tmpD61;_tmpD63=_tmpD62.kind;if(_tmpD63 != Cyc_Absyn_MemKind)
goto _LL68C;_tmpD64=_tmpD62.aliasqual;if(_tmpD64 != Cyc_Absyn_Aliasable)goto _LL68C;}}
_LL68B:*_tmpD60=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL683;_LL68C: {
struct Cyc_Absyn_Less_kb_struct*_tmpD65=(struct Cyc_Absyn_Less_kb_struct*)_tmpD4C;
if(_tmpD65->tag != 2)goto _LL68E;else{_tmpD66=_tmpD65->f1;_tmpD67=(struct Cyc_Core_Opt**)&
_tmpD65->f1;_tmpD68=_tmpD65->f2;_tmpD69=*_tmpD68;_tmpD6A=_tmpD69.kind;if(_tmpD6A
!= Cyc_Absyn_MemKind)goto _LL68E;_tmpD6B=_tmpD69.aliasqual;if(_tmpD6B != Cyc_Absyn_Unique)
goto _LL68E;}}_LL68D:*_tmpD67=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL683;_LL68E: {struct Cyc_Absyn_Less_kb_struct*_tmpD6C=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD4C;if(_tmpD6C->tag != 2)goto _LL690;else{_tmpD6D=_tmpD6C->f1;_tmpD6E=(struct
Cyc_Core_Opt**)& _tmpD6C->f1;_tmpD6F=_tmpD6C->f2;_tmpD70=*_tmpD6F;_tmpD71=_tmpD70.kind;
if(_tmpD71 != Cyc_Absyn_RgnKind)goto _LL690;_tmpD72=_tmpD70.aliasqual;if(_tmpD72 != 
Cyc_Absyn_Top)goto _LL690;}}_LL68F:*_tmpD6E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL683;_LL690: {struct Cyc_Absyn_Less_kb_struct*_tmpD73=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD4C;if(_tmpD73->tag != 2)goto _LL692;else{_tmpD74=_tmpD73->f1;_tmpD75=(struct
Cyc_Core_Opt**)& _tmpD73->f1;_tmpD76=_tmpD73->f2;}}_LL691:*_tmpD75=Cyc_Tcutil_kind_to_bound_opt(
_tmpD76);goto _LL683;_LL692: {struct Cyc_Absyn_Eq_kb_struct*_tmpD77=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpD4C;if(_tmpD77->tag != 0)goto _LL694;else{_tmpD78=_tmpD77->f1;_tmpD79=*_tmpD78;
_tmpD7A=_tmpD79.kind;if(_tmpD7A != Cyc_Absyn_MemKind)goto _LL694;_tmpD7B=_tmpD79.aliasqual;}}
_LL693:{const char*_tmp14BF;void*_tmp14BE[2];struct Cyc_String_pa_struct _tmp14BD;
struct Cyc_Absyn_Kind*_tmp14BC;struct Cyc_String_pa_struct _tmp14BB;(_tmp14BB.tag=0,((
_tmp14BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp14BC=_cycalloc_atomic(sizeof(*_tmp14BC)),((_tmp14BC->kind=Cyc_Absyn_MemKind,((
_tmp14BC->aliasqual=_tmpD7B,_tmp14BC)))))))),((_tmp14BD.tag=0,((_tmp14BD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp14BE[0]=& _tmp14BD,((_tmp14BE[1]=& _tmp14BB,Cyc_Tcutil_terr(loc,((
_tmp14BF="type variable %s cannot have kind %s",_tag_dyneither(_tmp14BF,sizeof(
char),37))),_tag_dyneither(_tmp14BE,sizeof(void*),2)))))))))))));}goto _LL683;
_LL694:;_LL695: goto _LL683;_LL683:;}}if(_tmpD45 != 0  || _tmpD46 != 0){{void*_tmpD81=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpD83;struct Cyc_List_List*_tmpD84;
struct Cyc_List_List**_tmpD85;struct Cyc_Core_Opt*_tmpD86;void*_tmpD87;struct Cyc_List_List*
_tmpD88;int _tmpD89;struct Cyc_Absyn_VarargInfo*_tmpD8A;struct Cyc_List_List*
_tmpD8B;struct Cyc_List_List*_tmpD8C;_LL697: {struct Cyc_Absyn_FnType_struct*
_tmpD82=(struct Cyc_Absyn_FnType_struct*)_tmpD81;if(_tmpD82->tag != 10)goto _LL699;
else{_tmpD83=_tmpD82->f1;_tmpD84=_tmpD83.tvars;_tmpD85=(struct Cyc_List_List**)&(
_tmpD82->f1).tvars;_tmpD86=_tmpD83.effect;_tmpD87=_tmpD83.ret_typ;_tmpD88=
_tmpD83.args;_tmpD89=_tmpD83.c_varargs;_tmpD8A=_tmpD83.cyc_varargs;_tmpD8B=
_tmpD83.rgn_po;_tmpD8C=_tmpD83.attributes;}}_LL698: if(*_tmpD85 == 0){*_tmpD85=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpD45);_tmpD45=0;}
goto _LL696;_LL699:;_LL69A: goto _LL696;_LL696:;}if(_tmpD45 != 0){const char*_tmp14C3;
void*_tmp14C2[1];struct Cyc_String_pa_struct _tmp14C1;(_tmp14C1.tag=0,((_tmp14C1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpD45->hd)->name),((
_tmp14C2[0]=& _tmp14C1,Cyc_Tcutil_terr(loc,((_tmp14C3="unbound type variable %s",
_tag_dyneither(_tmp14C3,sizeof(char),25))),_tag_dyneither(_tmp14C2,sizeof(void*),
1)))))));}if(_tmpD46 != 0)for(0;_tmpD46 != 0;_tmpD46=_tmpD46->tl){void*e=(void*)
_tmpD46->hd;struct Cyc_Absyn_Kind*_tmpD90=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpD91;enum Cyc_Absyn_KindQual _tmpD92;enum Cyc_Absyn_AliasQual _tmpD93;struct Cyc_Absyn_Kind
_tmpD94;enum Cyc_Absyn_KindQual _tmpD95;enum Cyc_Absyn_AliasQual _tmpD96;struct Cyc_Absyn_Kind
_tmpD97;enum Cyc_Absyn_KindQual _tmpD98;enum Cyc_Absyn_AliasQual _tmpD99;struct Cyc_Absyn_Kind
_tmpD9A;enum Cyc_Absyn_KindQual _tmpD9B;_LL69C: _tmpD91=*_tmpD90;_tmpD92=_tmpD91.kind;
if(_tmpD92 != Cyc_Absyn_RgnKind)goto _LL69E;_tmpD93=_tmpD91.aliasqual;if(_tmpD93 != 
Cyc_Absyn_Unique)goto _LL69E;_LL69D: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp14C6;void*_tmp14C5;(_tmp14C5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14C6="can't unify evar with unique region!",
_tag_dyneither(_tmp14C6,sizeof(char),37))),_tag_dyneither(_tmp14C5,sizeof(void*),
0)));}goto _LL69B;_LL69E: _tmpD94=*_tmpD90;_tmpD95=_tmpD94.kind;if(_tmpD95 != Cyc_Absyn_RgnKind)
goto _LL6A0;_tmpD96=_tmpD94.aliasqual;if(_tmpD96 != Cyc_Absyn_Aliasable)goto _LL6A0;
_LL69F: goto _LL6A1;_LL6A0: _tmpD97=*_tmpD90;_tmpD98=_tmpD97.kind;if(_tmpD98 != Cyc_Absyn_RgnKind)
goto _LL6A2;_tmpD99=_tmpD97.aliasqual;if(_tmpD99 != Cyc_Absyn_Top)goto _LL6A2;
_LL6A1: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp14C9;
void*_tmp14C8;(_tmp14C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp14C9="can't unify evar with heap!",_tag_dyneither(
_tmp14C9,sizeof(char),28))),_tag_dyneither(_tmp14C8,sizeof(void*),0)));}goto
_LL69B;_LL6A2: _tmpD9A=*_tmpD90;_tmpD9B=_tmpD9A.kind;if(_tmpD9B != Cyc_Absyn_EffKind)
goto _LL6A4;_LL6A3: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp14CC;void*_tmp14CB;(_tmp14CB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14CC="can't unify evar with {}!",
_tag_dyneither(_tmp14CC,sizeof(char),26))),_tag_dyneither(_tmp14CB,sizeof(void*),
0)));}goto _LL69B;_LL6A4:;_LL6A5:{const char*_tmp14D1;void*_tmp14D0[2];struct Cyc_String_pa_struct
_tmp14CF;struct Cyc_String_pa_struct _tmp14CE;(_tmp14CE.tag=0,((_tmp14CE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp14CF.tag=0,((_tmp14CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp14D0[0]=& _tmp14CF,((_tmp14D0[1]=& _tmp14CE,Cyc_Tcutil_terr(loc,((
_tmp14D1="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp14D1,sizeof(char),52))),_tag_dyneither(_tmp14D0,sizeof(void*),2)))))))))))));}
goto _LL69B;_LL69B:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpDA7=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpDA9;struct Cyc_List_List*_tmpDAA;struct Cyc_Core_Opt*
_tmpDAB;void*_tmpDAC;_LL6A7: {struct Cyc_Absyn_FnType_struct*_tmpDA8=(struct Cyc_Absyn_FnType_struct*)
_tmpDA7;if(_tmpDA8->tag != 10)goto _LL6A9;else{_tmpDA9=_tmpDA8->f1;_tmpDAA=_tmpDA9.tvars;
_tmpDAB=_tmpDA9.effect;_tmpDAC=_tmpDA9.ret_typ;}}_LL6A8: fd->tvs=_tmpDAA;fd->effect=
_tmpDAB;goto _LL6A6;_LL6A9:;_LL6AA: {const char*_tmp14D4;void*_tmp14D3;(_tmp14D3=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp14D4="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp14D4,sizeof(
char),38))),_tag_dyneither(_tmp14D3,sizeof(void*),0)));}_LL6A6:;}{struct
_RegionHandle*_tmpDAF=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp14D5;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDAF,(struct _dyneither_ptr*(*)(
struct _tuple23*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,((_tmp14D5="function declaration has repeated parameter",
_tag_dyneither(_tmp14D5,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp14D6;fd->cached_typ=((
_tmp14D6=_cycalloc(sizeof(*_tmp14D6)),((_tmp14D6->v=t,_tmp14D6))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpDB2=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp14D7;
struct Cyc_Tcutil_CVTEnv _tmpDB3=Cyc_Tcutil_check_valid_type(loc,te,((_tmp14D7.r=
_tmpDB2,((_tmp14D7.kind_env=bound_tvars,((_tmp14D7.free_vars=0,((_tmp14D7.free_evars=
0,((_tmp14D7.generalize_evars=0,((_tmp14D7.fn_result=0,_tmp14D7)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpDB4=Cyc_Tcutil_remove_bound_tvars(
_tmpDB2,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple27*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDB2,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple27*))Cyc_Core_fst,_tmpDB3.free_vars),bound_tvars);struct Cyc_List_List*
_tmpDB5=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple28*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDB2,(void*(*)(struct _tuple28*))Cyc_Core_fst,
_tmpDB3.free_evars);{struct Cyc_List_List*fs=_tmpDB4;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpDB6=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp14DC;void*_tmp14DB[2];struct Cyc_String_pa_struct _tmp14DA;struct Cyc_String_pa_struct
_tmp14D9;(_tmp14D9.tag=0,((_tmp14D9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp14DA.tag=0,((_tmp14DA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpDB6),((_tmp14DB[0]=& _tmp14DA,((
_tmp14DB[1]=& _tmp14D9,Cyc_Tcutil_terr(loc,((_tmp14DC="unbound type variable %s in type %s",
_tag_dyneither(_tmp14DC,sizeof(char),36))),_tag_dyneither(_tmp14DB,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpDB5 != 0)for(0;_tmpDB5 != 0;_tmpDB5=
_tmpDB5->tl){void*e=(void*)_tmpDB5->hd;struct Cyc_Absyn_Kind*_tmpDBB=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpDBC;enum Cyc_Absyn_KindQual _tmpDBD;enum Cyc_Absyn_AliasQual
_tmpDBE;struct Cyc_Absyn_Kind _tmpDBF;enum Cyc_Absyn_KindQual _tmpDC0;enum Cyc_Absyn_AliasQual
_tmpDC1;struct Cyc_Absyn_Kind _tmpDC2;enum Cyc_Absyn_KindQual _tmpDC3;enum Cyc_Absyn_AliasQual
_tmpDC4;struct Cyc_Absyn_Kind _tmpDC5;enum Cyc_Absyn_KindQual _tmpDC6;_LL6AC:
_tmpDBC=*_tmpDBB;_tmpDBD=_tmpDBC.kind;if(_tmpDBD != Cyc_Absyn_RgnKind)goto _LL6AE;
_tmpDBE=_tmpDBC.aliasqual;if(_tmpDBE != Cyc_Absyn_Unique)goto _LL6AE;_LL6AD: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp14DF;void*
_tmp14DE;(_tmp14DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14DF="can't unify evar with unique region!",
_tag_dyneither(_tmp14DF,sizeof(char),37))),_tag_dyneither(_tmp14DE,sizeof(void*),
0)));}goto _LL6AB;_LL6AE: _tmpDBF=*_tmpDBB;_tmpDC0=_tmpDBF.kind;if(_tmpDC0 != Cyc_Absyn_RgnKind)
goto _LL6B0;_tmpDC1=_tmpDBF.aliasqual;if(_tmpDC1 != Cyc_Absyn_Aliasable)goto _LL6B0;
_LL6AF: goto _LL6B1;_LL6B0: _tmpDC2=*_tmpDBB;_tmpDC3=_tmpDC2.kind;if(_tmpDC3 != Cyc_Absyn_RgnKind)
goto _LL6B2;_tmpDC4=_tmpDC2.aliasqual;if(_tmpDC4 != Cyc_Absyn_Top)goto _LL6B2;
_LL6B1: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp14E2;
void*_tmp14E1;(_tmp14E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp14E2="can't unify evar with heap!",_tag_dyneither(
_tmp14E2,sizeof(char),28))),_tag_dyneither(_tmp14E1,sizeof(void*),0)));}goto
_LL6AB;_LL6B2: _tmpDC5=*_tmpDBB;_tmpDC6=_tmpDC5.kind;if(_tmpDC6 != Cyc_Absyn_EffKind)
goto _LL6B4;_LL6B3: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp14E5;void*_tmp14E4;(_tmp14E4=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14E5="can't unify evar with {}!",
_tag_dyneither(_tmp14E5,sizeof(char),26))),_tag_dyneither(_tmp14E4,sizeof(void*),
0)));}goto _LL6AB;_LL6B4:;_LL6B5:{const char*_tmp14EA;void*_tmp14E9[2];struct Cyc_String_pa_struct
_tmp14E8;struct Cyc_String_pa_struct _tmp14E7;(_tmp14E7.tag=0,((_tmp14E7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp14E8.tag=0,((_tmp14E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp14E9[0]=& _tmp14E8,((_tmp14E9[1]=& _tmp14E7,Cyc_Tcutil_terr(loc,((
_tmp14EA="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp14EA,sizeof(char),52))),_tag_dyneither(_tmp14E9,sizeof(void*),2)))))))))))));}
goto _LL6AB;_LL6AB:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp14EF;void*_tmp14EE[2];struct Cyc_String_pa_struct
_tmp14ED;struct Cyc_String_pa_struct _tmp14EC;(_tmp14EC.tag=0,((_tmp14EC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp14ED.tag=
0,((_tmp14ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp14EE[0]=&
_tmp14ED,((_tmp14EE[1]=& _tmp14EC,Cyc_Tcutil_terr(loc,((_tmp14EF="%s: %s",
_tag_dyneither(_tmp14EF,sizeof(char),7))),_tag_dyneither(_tmp14EE,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp14F0;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp14F0="duplicate type variable",
_tag_dyneither(_tmp14F0,sizeof(char),24))));}struct _tuple30{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple31{struct Cyc_List_List*f1;void*f2;};struct _tuple32{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp14F1;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp14F1="",_tag_dyneither(_tmp14F1,
sizeof(char),1))))!= 0){struct _tuple30*_tmp14F4;struct Cyc_List_List*_tmp14F3;
fields=((_tmp14F3=_cycalloc(sizeof(*_tmp14F3)),((_tmp14F3->hd=((_tmp14F4=
_cycalloc(sizeof(*_tmp14F4)),((_tmp14F4->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp14F4->f2=0,_tmp14F4)))))),((_tmp14F3->tl=fields,_tmp14F3))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp14F6;const char*_tmp14F5;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp14F6="struct",_tag_dyneither(_tmp14F6,sizeof(char),7)):((
_tmp14F5="union",_tag_dyneither(_tmp14F5,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple31 _tmpDDB;struct Cyc_List_List*
_tmpDDC;void*_tmpDDD;struct _tuple31*_tmpDDA=(struct _tuple31*)des->hd;_tmpDDB=*
_tmpDDA;_tmpDDC=_tmpDDB.f1;_tmpDDD=_tmpDDB.f2;if(_tmpDDC == 0){struct Cyc_List_List*
_tmpDDE=fields;for(0;_tmpDDE != 0;_tmpDDE=_tmpDDE->tl){if(!(*((struct _tuple30*)
_tmpDDE->hd)).f2){(*((struct _tuple30*)_tmpDDE->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp14FC;struct Cyc_Absyn_FieldName_struct _tmp14FB;struct Cyc_List_List*_tmp14FA;(*((
struct _tuple31*)des->hd)).f1=(struct Cyc_List_List*)((_tmp14FA=_cycalloc(sizeof(*
_tmp14FA)),((_tmp14FA->hd=(void*)((_tmp14FC=_cycalloc(sizeof(*_tmp14FC)),((
_tmp14FC[0]=((_tmp14FB.tag=1,((_tmp14FB.f1=((*((struct _tuple30*)_tmpDDE->hd)).f1)->name,
_tmp14FB)))),_tmp14FC)))),((_tmp14FA->tl=0,_tmp14FA))))));}{struct _tuple32*
_tmp14FF;struct Cyc_List_List*_tmp14FE;ans=((_tmp14FE=_region_malloc(rgn,sizeof(*
_tmp14FE)),((_tmp14FE->hd=((_tmp14FF=_region_malloc(rgn,sizeof(*_tmp14FF)),((
_tmp14FF->f1=(*((struct _tuple30*)_tmpDDE->hd)).f1,((_tmp14FF->f2=_tmpDDD,
_tmp14FF)))))),((_tmp14FE->tl=ans,_tmp14FE))))));}break;}}if(_tmpDDE == 0){const
char*_tmp1503;void*_tmp1502[1];struct Cyc_String_pa_struct _tmp1501;(_tmp1501.tag=
0,((_tmp1501.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp1502[0]=& _tmp1501,Cyc_Tcutil_terr(loc,((_tmp1503="too many arguments to %s",
_tag_dyneither(_tmp1503,sizeof(char),25))),_tag_dyneither(_tmp1502,sizeof(void*),
1)))))));}}else{if(_tmpDDC->tl != 0){const char*_tmp1506;void*_tmp1505;(_tmp1505=0,
Cyc_Tcutil_terr(loc,((_tmp1506="multiple designators are not yet supported",
_tag_dyneither(_tmp1506,sizeof(char),43))),_tag_dyneither(_tmp1505,sizeof(void*),
0)));}else{void*_tmpDE9=(void*)_tmpDDC->hd;struct _dyneither_ptr*_tmpDEC;_LL6B7: {
struct Cyc_Absyn_ArrayElement_struct*_tmpDEA=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpDE9;if(_tmpDEA->tag != 0)goto _LL6B9;}_LL6B8:{const char*_tmp150A;void*_tmp1509[
1];struct Cyc_String_pa_struct _tmp1508;(_tmp1508.tag=0,((_tmp1508.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1509[0]=& _tmp1508,Cyc_Tcutil_terr(
loc,((_tmp150A="array designator used in argument to %s",_tag_dyneither(_tmp150A,
sizeof(char),40))),_tag_dyneither(_tmp1509,sizeof(void*),1)))))));}goto _LL6B6;
_LL6B9: {struct Cyc_Absyn_FieldName_struct*_tmpDEB=(struct Cyc_Absyn_FieldName_struct*)
_tmpDE9;if(_tmpDEB->tag != 1)goto _LL6B6;else{_tmpDEC=_tmpDEB->f1;}}_LL6BA: {
struct Cyc_List_List*_tmpDF0=fields;for(0;_tmpDF0 != 0;_tmpDF0=_tmpDF0->tl){if(Cyc_strptrcmp(
_tmpDEC,((*((struct _tuple30*)_tmpDF0->hd)).f1)->name)== 0){if((*((struct _tuple30*)
_tmpDF0->hd)).f2){const char*_tmp150E;void*_tmp150D[1];struct Cyc_String_pa_struct
_tmp150C;(_tmp150C.tag=0,((_tmp150C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpDEC),((_tmp150D[0]=& _tmp150C,Cyc_Tcutil_terr(loc,((_tmp150E="member %s has already been used as an argument",
_tag_dyneither(_tmp150E,sizeof(char),47))),_tag_dyneither(_tmp150D,sizeof(void*),
1)))))));}(*((struct _tuple30*)_tmpDF0->hd)).f2=1;{struct _tuple32*_tmp1511;struct
Cyc_List_List*_tmp1510;ans=((_tmp1510=_region_malloc(rgn,sizeof(*_tmp1510)),((
_tmp1510->hd=((_tmp1511=_region_malloc(rgn,sizeof(*_tmp1511)),((_tmp1511->f1=(*((
struct _tuple30*)_tmpDF0->hd)).f1,((_tmp1511->f2=_tmpDDD,_tmp1511)))))),((
_tmp1510->tl=ans,_tmp1510))))));}break;}}if(_tmpDF0 == 0){const char*_tmp1515;void*
_tmp1514[1];struct Cyc_String_pa_struct _tmp1513;(_tmp1513.tag=0,((_tmp1513.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpDEC),((_tmp1514[0]=& _tmp1513,
Cyc_Tcutil_terr(loc,((_tmp1515="bad field designator %s",_tag_dyneither(_tmp1515,
sizeof(char),24))),_tag_dyneither(_tmp1514,sizeof(void*),1)))))));}goto _LL6B6;}
_LL6B6:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple30*)fields->hd)).f2){{const char*_tmp1518;void*_tmp1517;(
_tmp1517=0,Cyc_Tcutil_terr(loc,((_tmp1518="too few arguments to struct",
_tag_dyneither(_tmp1518,sizeof(char),28))),_tag_dyneither(_tmp1517,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple30*)fields->hd)).f2){if(found){const char*_tmp151B;void*_tmp151A;(_tmp151A=
0,Cyc_Tcutil_terr(loc,((_tmp151B="only one member of a union is allowed",
_tag_dyneither(_tmp151B,sizeof(char),38))),_tag_dyneither(_tmp151A,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp151E;void*_tmp151D;(_tmp151D=0,Cyc_Tcutil_terr(
loc,((_tmp151E="missing member for union",_tag_dyneither(_tmp151E,sizeof(char),
25))),_tag_dyneither(_tmp151D,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE01=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE03;void*_tmpE04;struct Cyc_Absyn_PtrAtts _tmpE05;union Cyc_Absyn_Constraint*
_tmpE06;_LL6BC: {struct Cyc_Absyn_PointerType_struct*_tmpE02=(struct Cyc_Absyn_PointerType_struct*)
_tmpE01;if(_tmpE02->tag != 5)goto _LL6BE;else{_tmpE03=_tmpE02->f1;_tmpE04=_tmpE03.elt_typ;
_tmpE05=_tmpE03.ptr_atts;_tmpE06=_tmpE05.bounds;}}_LL6BD: {void*_tmpE07=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE06);_LL6C1: {struct Cyc_Absyn_DynEither_b_struct*_tmpE08=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE07;if(_tmpE08->tag != 0)goto _LL6C3;}_LL6C2:*elt_typ_dest=_tmpE04;return 1;
_LL6C3:;_LL6C4: return 0;_LL6C0:;}_LL6BE:;_LL6BF: return 0;_LL6BB:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE09=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE0B;void*_tmpE0C;struct Cyc_Absyn_PtrAtts _tmpE0D;union Cyc_Absyn_Constraint*
_tmpE0E;_LL6C6: {struct Cyc_Absyn_PointerType_struct*_tmpE0A=(struct Cyc_Absyn_PointerType_struct*)
_tmpE09;if(_tmpE0A->tag != 5)goto _LL6C8;else{_tmpE0B=_tmpE0A->f1;_tmpE0C=_tmpE0B.elt_typ;
_tmpE0D=_tmpE0B.ptr_atts;_tmpE0E=_tmpE0D.zero_term;}}_LL6C7:*elt_typ_dest=
_tmpE0C;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE0E);_LL6C8:;_LL6C9: return 0;_LL6C5:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE0F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE11;void*_tmpE12;struct Cyc_Absyn_PtrAtts _tmpE13;
union Cyc_Absyn_Constraint*_tmpE14;union Cyc_Absyn_Constraint*_tmpE15;struct Cyc_Absyn_ArrayInfo
_tmpE17;void*_tmpE18;struct Cyc_Absyn_Tqual _tmpE19;struct Cyc_Absyn_Exp*_tmpE1A;
union Cyc_Absyn_Constraint*_tmpE1B;_LL6CB: {struct Cyc_Absyn_PointerType_struct*
_tmpE10=(struct Cyc_Absyn_PointerType_struct*)_tmpE0F;if(_tmpE10->tag != 5)goto
_LL6CD;else{_tmpE11=_tmpE10->f1;_tmpE12=_tmpE11.elt_typ;_tmpE13=_tmpE11.ptr_atts;
_tmpE14=_tmpE13.bounds;_tmpE15=_tmpE13.zero_term;}}_LL6CC: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE15)){*ptr_type=t;*elt_type=
_tmpE12;{void*_tmpE1C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE14);_LL6D2: {struct Cyc_Absyn_DynEither_b_struct*
_tmpE1D=(struct Cyc_Absyn_DynEither_b_struct*)_tmpE1C;if(_tmpE1D->tag != 0)goto
_LL6D4;}_LL6D3:*is_dyneither=1;goto _LL6D1;_LL6D4:;_LL6D5:*is_dyneither=0;goto
_LL6D1;_LL6D1:;}return 1;}else{return 0;}_LL6CD: {struct Cyc_Absyn_ArrayType_struct*
_tmpE16=(struct Cyc_Absyn_ArrayType_struct*)_tmpE0F;if(_tmpE16->tag != 9)goto
_LL6CF;else{_tmpE17=_tmpE16->f1;_tmpE18=_tmpE17.elt_type;_tmpE19=_tmpE17.tq;
_tmpE1A=_tmpE17.num_elts;_tmpE1B=_tmpE17.zero_term;}}_LL6CE: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE1B)){*elt_type=_tmpE18;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1533;struct Cyc_Absyn_PtrAtts
_tmp1532;struct Cyc_Absyn_Upper_b_struct _tmp1531;struct Cyc_Absyn_Upper_b_struct*
_tmp1530;struct Cyc_Absyn_PtrInfo _tmp152F;struct Cyc_Absyn_PointerType_struct*
_tmp152E;*ptr_type=(void*)((_tmp152E=_cycalloc(sizeof(*_tmp152E)),((_tmp152E[0]=((
_tmp1533.tag=5,((_tmp1533.f1=((_tmp152F.elt_typ=_tmpE18,((_tmp152F.elt_tq=
_tmpE19,((_tmp152F.ptr_atts=((_tmp1532.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1532.nullable=Cyc_Absyn_false_conref,((_tmp1532.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1530=_cycalloc(sizeof(*_tmp1530)),((
_tmp1530[0]=((_tmp1531.tag=1,((_tmp1531.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpE1A),_tmp1531)))),_tmp1530))))),((_tmp1532.zero_term=_tmpE1B,((_tmp1532.ptrloc=
0,_tmp1532)))))))))),_tmp152F)))))),_tmp1533)))),_tmp152E))));}return 1;}else{
return 0;}_LL6CF:;_LL6D0: return 0;_LL6CA:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE24=e1->r;
struct Cyc_Absyn_Exp*_tmpE27;struct Cyc_Absyn_Exp*_tmpE29;struct Cyc_Absyn_Exp*
_tmpE2B;struct Cyc_Absyn_Exp*_tmpE2D;struct Cyc_Absyn_Exp*_tmpE2F;struct Cyc_Absyn_Exp*
_tmpE31;_LL6D7: {struct Cyc_Absyn_Cast_e_struct*_tmpE25=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE24;if(_tmpE25->tag != 15)goto _LL6D9;}_LL6D8: {const char*_tmp1537;void*
_tmp1536[1];struct Cyc_String_pa_struct _tmp1535;(_tmp1535.tag=0,((_tmp1535.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1536[0]=& _tmp1535,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1537="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1537,sizeof(char),29))),_tag_dyneither(_tmp1536,sizeof(void*),1)))))));}
_LL6D9: {struct Cyc_Absyn_Deref_e_struct*_tmpE26=(struct Cyc_Absyn_Deref_e_struct*)
_tmpE24;if(_tmpE26->tag != 21)goto _LL6DB;else{_tmpE27=_tmpE26->f1;}}_LL6DA:
_tmpE29=_tmpE27;goto _LL6DC;_LL6DB: {struct Cyc_Absyn_Subscript_e_struct*_tmpE28=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpE24;if(_tmpE28->tag != 24)goto _LL6DD;
else{_tmpE29=_tmpE28->f1;}}_LL6DC: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpE29->topt))->v,ptr_type,is_dyneither,
elt_type);_LL6DD: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpE2A=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE24;if(_tmpE2A->tag != 23)goto _LL6DF;else{_tmpE2B=_tmpE2A->f1;}}_LL6DE:
_tmpE2D=_tmpE2B;goto _LL6E0;_LL6DF: {struct Cyc_Absyn_AggrMember_e_struct*_tmpE2C=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpE24;if(_tmpE2C->tag != 22)goto _LL6E1;
else{_tmpE2D=_tmpE2C->f1;}}_LL6E0: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpE2D->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp153B;void*_tmp153A[1];struct Cyc_String_pa_struct _tmp1539;(_tmp1539.tag=
0,((_tmp1539.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp153A[0]=& _tmp1539,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp153B="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp153B,sizeof(char),51))),_tag_dyneither(_tmp153A,sizeof(void*),
1)))))));}return 0;_LL6E1: {struct Cyc_Absyn_Instantiate_e_struct*_tmpE2E=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpE24;if(_tmpE2E->tag != 14)goto _LL6E3;else{
_tmpE2F=_tmpE2E->f1;}}_LL6E2: _tmpE31=_tmpE2F;goto _LL6E4;_LL6E3: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpE30=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE24;if(_tmpE30->tag != 13)
goto _LL6E5;else{_tmpE31=_tmpE30->f1;}}_LL6E4: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE31->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp153F;void*_tmp153E[1];struct Cyc_String_pa_struct
_tmp153D;(_tmp153D.tag=0,((_tmp153D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp153E[0]=& _tmp153D,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp153F="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp153F,sizeof(char),49))),_tag_dyneither(_tmp153E,sizeof(void*),
1)))))));}return 0;_LL6E5: {struct Cyc_Absyn_Var_e_struct*_tmpE32=(struct Cyc_Absyn_Var_e_struct*)
_tmpE24;if(_tmpE32->tag != 1)goto _LL6E7;}_LL6E6: return 0;_LL6E7:;_LL6E8: {const
char*_tmp1543;void*_tmp1542[1];struct Cyc_String_pa_struct _tmp1541;(_tmp1541.tag=
0,((_tmp1541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1542[0]=& _tmp1541,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1543="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1543,sizeof(char),39))),_tag_dyneither(_tmp1542,sizeof(void*),
1)))))));}_LL6D6:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
static int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpE3F=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpE42;_LL6EA: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpE40=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpE3F;if(_tmpE40->tag != 22)goto
_LL6EC;}_LL6EB: return 1;_LL6EC: {struct Cyc_Absyn_VarType_struct*_tmpE41=(struct
Cyc_Absyn_VarType_struct*)_tmpE3F;if(_tmpE41->tag != 2)goto _LL6EE;else{_tmpE42=
_tmpE41->f1;}}_LL6ED: {struct Cyc_Absyn_Kind _tmpE44;enum Cyc_Absyn_KindQual
_tmpE45;enum Cyc_Absyn_AliasQual _tmpE46;struct Cyc_Absyn_Kind*_tmpE43=Cyc_Tcutil_tvar_kind(
_tmpE42,& Cyc_Tcutil_rk);_tmpE44=*_tmpE43;_tmpE45=_tmpE44.kind;_tmpE46=_tmpE44.aliasqual;
if(_tmpE45 == Cyc_Absyn_RgnKind  && (_tmpE46 == Cyc_Absyn_Unique  || _tmpE46 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpE47=Cyc_Absyn_compress_kb(_tmpE42->kind);struct
Cyc_Core_Opt*_tmpE49;struct Cyc_Core_Opt**_tmpE4A;struct Cyc_Absyn_Kind*_tmpE4B;
struct Cyc_Absyn_Kind _tmpE4C;enum Cyc_Absyn_KindQual _tmpE4D;enum Cyc_Absyn_AliasQual
_tmpE4E;_LL6F1: {struct Cyc_Absyn_Less_kb_struct*_tmpE48=(struct Cyc_Absyn_Less_kb_struct*)
_tmpE47;if(_tmpE48->tag != 2)goto _LL6F3;else{_tmpE49=_tmpE48->f1;_tmpE4A=(struct
Cyc_Core_Opt**)& _tmpE48->f1;_tmpE4B=_tmpE48->f2;_tmpE4C=*_tmpE4B;_tmpE4D=_tmpE4C.kind;
if(_tmpE4D != Cyc_Absyn_RgnKind)goto _LL6F3;_tmpE4E=_tmpE4C.aliasqual;if(_tmpE4E != 
Cyc_Absyn_Top)goto _LL6F3;}}_LL6F2:{struct Cyc_Absyn_Less_kb_struct*_tmp1549;
struct Cyc_Absyn_Less_kb_struct _tmp1548;struct Cyc_Core_Opt*_tmp1547;*_tmpE4A=((
_tmp1547=_cycalloc(sizeof(*_tmp1547)),((_tmp1547->v=(void*)((_tmp1549=_cycalloc(
sizeof(*_tmp1549)),((_tmp1549[0]=((_tmp1548.tag=2,((_tmp1548.f1=0,((_tmp1548.f2=&
Cyc_Tcutil_rk,_tmp1548)))))),_tmp1549)))),_tmp1547))));}return 0;_LL6F3:;_LL6F4:
return 1;_LL6F0:;}return 0;}_LL6EE:;_LL6EF: return 0;_LL6E9:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpE52=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE54;struct Cyc_Absyn_PtrAtts _tmpE55;void*_tmpE56;struct Cyc_Absyn_Tvar*_tmpE58;
_LL6F6: {struct Cyc_Absyn_PointerType_struct*_tmpE53=(struct Cyc_Absyn_PointerType_struct*)
_tmpE52;if(_tmpE53->tag != 5)goto _LL6F8;else{_tmpE54=_tmpE53->f1;_tmpE55=_tmpE54.ptr_atts;
_tmpE56=_tmpE55.rgn;}}_LL6F7: return Cyc_Tcutil_is_noalias_region(_tmpE56,
must_be_unique);_LL6F8: {struct Cyc_Absyn_VarType_struct*_tmpE57=(struct Cyc_Absyn_VarType_struct*)
_tmpE52;if(_tmpE57->tag != 2)goto _LL6FA;else{_tmpE58=_tmpE57->f1;}}_LL6F9: {
struct Cyc_Absyn_Kind _tmpE5A;enum Cyc_Absyn_KindQual _tmpE5B;enum Cyc_Absyn_AliasQual
_tmpE5C;struct Cyc_Absyn_Kind*_tmpE59=Cyc_Tcutil_tvar_kind(_tmpE58,& Cyc_Tcutil_bk);
_tmpE5A=*_tmpE59;_tmpE5B=_tmpE5A.kind;_tmpE5C=_tmpE5A.aliasqual;switch(_tmpE5B){
case Cyc_Absyn_BoxKind: _LL6FC: goto _LL6FD;case Cyc_Absyn_AnyKind: _LL6FD: goto _LL6FE;
case Cyc_Absyn_MemKind: _LL6FE: if(_tmpE5C == Cyc_Absyn_Unique  || _tmpE5C == Cyc_Absyn_Top){
void*_tmpE5D=Cyc_Absyn_compress_kb(_tmpE58->kind);struct Cyc_Core_Opt*_tmpE5F;
struct Cyc_Core_Opt**_tmpE60;struct Cyc_Absyn_Kind*_tmpE61;struct Cyc_Absyn_Kind
_tmpE62;enum Cyc_Absyn_KindQual _tmpE63;enum Cyc_Absyn_AliasQual _tmpE64;_LL701: {
struct Cyc_Absyn_Less_kb_struct*_tmpE5E=(struct Cyc_Absyn_Less_kb_struct*)_tmpE5D;
if(_tmpE5E->tag != 2)goto _LL703;else{_tmpE5F=_tmpE5E->f1;_tmpE60=(struct Cyc_Core_Opt**)&
_tmpE5E->f1;_tmpE61=_tmpE5E->f2;_tmpE62=*_tmpE61;_tmpE63=_tmpE62.kind;_tmpE64=
_tmpE62.aliasqual;if(_tmpE64 != Cyc_Absyn_Top)goto _LL703;}}_LL702:{struct Cyc_Absyn_Less_kb_struct*
_tmp1553;struct Cyc_Absyn_Kind*_tmp1552;struct Cyc_Absyn_Less_kb_struct _tmp1551;
struct Cyc_Core_Opt*_tmp1550;*_tmpE60=((_tmp1550=_cycalloc(sizeof(*_tmp1550)),((
_tmp1550->v=(void*)((_tmp1553=_cycalloc(sizeof(*_tmp1553)),((_tmp1553[0]=((
_tmp1551.tag=2,((_tmp1551.f1=0,((_tmp1551.f2=((_tmp1552=_cycalloc_atomic(sizeof(*
_tmp1552)),((_tmp1552->kind=_tmpE63,((_tmp1552->aliasqual=Cyc_Absyn_Aliasable,
_tmp1552)))))),_tmp1551)))))),_tmp1553)))),_tmp1550))));}return 0;_LL703:;_LL704:
return 1;_LL700:;}return 0;default: _LL6FF: return 0;}}_LL6FA:;_LL6FB: return 0;_LL6F5:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpE69=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpE69))
return 1;{void*_tmpE6A=_tmpE69;struct Cyc_List_List*_tmpE6C;struct Cyc_Absyn_AggrInfo
_tmpE6E;union Cyc_Absyn_AggrInfoU _tmpE6F;struct Cyc_Absyn_Aggrdecl**_tmpE70;struct
Cyc_List_List*_tmpE71;struct Cyc_List_List*_tmpE73;struct Cyc_Absyn_AggrInfo
_tmpE75;union Cyc_Absyn_AggrInfoU _tmpE76;struct _tuple4 _tmpE77;struct Cyc_Absyn_DatatypeInfo
_tmpE79;union Cyc_Absyn_DatatypeInfoU _tmpE7A;struct Cyc_List_List*_tmpE7B;struct
Cyc_Absyn_DatatypeFieldInfo _tmpE7D;union Cyc_Absyn_DatatypeFieldInfoU _tmpE7E;
struct Cyc_List_List*_tmpE7F;_LL707: {struct Cyc_Absyn_TupleType_struct*_tmpE6B=(
struct Cyc_Absyn_TupleType_struct*)_tmpE6A;if(_tmpE6B->tag != 11)goto _LL709;else{
_tmpE6C=_tmpE6B->f1;}}_LL708: while(_tmpE6C != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpE6C->hd)).f2))return 1;_tmpE6C=_tmpE6C->tl;}return 0;
_LL709: {struct Cyc_Absyn_AggrType_struct*_tmpE6D=(struct Cyc_Absyn_AggrType_struct*)
_tmpE6A;if(_tmpE6D->tag != 12)goto _LL70B;else{_tmpE6E=_tmpE6D->f1;_tmpE6F=_tmpE6E.aggr_info;
if((_tmpE6F.KnownAggr).tag != 2)goto _LL70B;_tmpE70=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE6F.KnownAggr).val;_tmpE71=_tmpE6E.targs;}}_LL70A: if((*_tmpE70)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpE80=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpE70)->tvs,_tmpE71);struct Cyc_List_List*_tmpE81=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpE70)->impl))->fields;void*t;
while(_tmpE81 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpE80,((struct Cyc_Absyn_Aggrfield*)
_tmpE81->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpE81=_tmpE81->tl;}return 0;}_LL70B: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpE72=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpE6A;if(_tmpE72->tag != 13)goto
_LL70D;else{_tmpE73=_tmpE72->f2;}}_LL70C: while(_tmpE73 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpE73->hd)->type))return 1;_tmpE73=_tmpE73->tl;}
return 0;_LL70D: {struct Cyc_Absyn_AggrType_struct*_tmpE74=(struct Cyc_Absyn_AggrType_struct*)
_tmpE6A;if(_tmpE74->tag != 12)goto _LL70F;else{_tmpE75=_tmpE74->f1;_tmpE76=_tmpE75.aggr_info;
if((_tmpE76.UnknownAggr).tag != 1)goto _LL70F;_tmpE77=(struct _tuple4)(_tmpE76.UnknownAggr).val;}}
_LL70E: {const char*_tmp1556;void*_tmp1555;(_tmp1555=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1556="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1556,sizeof(char),36))),_tag_dyneither(_tmp1555,sizeof(void*),
0)));}_LL70F: {struct Cyc_Absyn_DatatypeType_struct*_tmpE78=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpE6A;if(_tmpE78->tag != 3)goto _LL711;else{_tmpE79=_tmpE78->f1;_tmpE7A=_tmpE79.datatype_info;
_tmpE7B=_tmpE79.targs;}}_LL710: {union Cyc_Absyn_DatatypeInfoU _tmpE84=_tmpE7A;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpE85;struct _tuple2*_tmpE86;int _tmpE87;
struct Cyc_Absyn_Datatypedecl**_tmpE88;struct Cyc_Absyn_Datatypedecl*_tmpE89;
struct Cyc_Absyn_Datatypedecl _tmpE8A;struct Cyc_List_List*_tmpE8B;struct Cyc_Core_Opt*
_tmpE8C;_LL716: if((_tmpE84.UnknownDatatype).tag != 1)goto _LL718;_tmpE85=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpE84.UnknownDatatype).val;_tmpE86=_tmpE85.name;
_tmpE87=_tmpE85.is_extensible;_LL717: {const char*_tmp1559;void*_tmp1558;(
_tmp1558=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1559="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1559,
sizeof(char),40))),_tag_dyneither(_tmp1558,sizeof(void*),0)));}_LL718: if((
_tmpE84.KnownDatatype).tag != 2)goto _LL715;_tmpE88=(struct Cyc_Absyn_Datatypedecl**)(
_tmpE84.KnownDatatype).val;_tmpE89=*_tmpE88;_tmpE8A=*_tmpE89;_tmpE8B=_tmpE8A.tvs;
_tmpE8C=_tmpE8A.fields;_LL719: return 0;_LL715:;}_LL711: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpE7C=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpE6A;if(_tmpE7C->tag != 4)
goto _LL713;else{_tmpE7D=_tmpE7C->f1;_tmpE7E=_tmpE7D.field_info;_tmpE7F=_tmpE7D.targs;}}
_LL712: {union Cyc_Absyn_DatatypeFieldInfoU _tmpE8F=_tmpE7E;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpE90;struct _tuple3 _tmpE91;struct Cyc_Absyn_Datatypedecl*_tmpE92;struct Cyc_Absyn_Datatypefield*
_tmpE93;_LL71B: if((_tmpE8F.UnknownDatatypefield).tag != 1)goto _LL71D;_tmpE90=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpE8F.UnknownDatatypefield).val;
_LL71C: {const char*_tmp155C;void*_tmp155B;(_tmp155B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp155C="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp155C,sizeof(char),46))),_tag_dyneither(_tmp155B,sizeof(void*),
0)));}_LL71D: if((_tmpE8F.KnownDatatypefield).tag != 2)goto _LL71A;_tmpE91=(struct
_tuple3)(_tmpE8F.KnownDatatypefield).val;_tmpE92=_tmpE91.f1;_tmpE93=_tmpE91.f2;
_LL71E: {struct Cyc_List_List*_tmpE96=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpE92->tvs,_tmpE7F);struct Cyc_List_List*_tmpE97=
_tmpE93->typs;while(_tmpE97 != 0){_tmpE69=Cyc_Tcutil_rsubstitute(rgn,_tmpE96,(*((
struct _tuple11*)_tmpE97->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpE69))return 1;_tmpE97=_tmpE97->tl;}return 0;}_LL71A:;}_LL713:;_LL714: return 0;
_LL706:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpE98=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpE9A;union Cyc_Absyn_AggrInfoU _tmpE9B;struct Cyc_Absyn_Aggrdecl**_tmpE9C;struct
Cyc_Absyn_Aggrdecl*_tmpE9D;struct Cyc_List_List*_tmpE9E;struct Cyc_List_List*
_tmpEA0;_LL720: {struct Cyc_Absyn_AggrType_struct*_tmpE99=(struct Cyc_Absyn_AggrType_struct*)
_tmpE98;if(_tmpE99->tag != 12)goto _LL722;else{_tmpE9A=_tmpE99->f1;_tmpE9B=_tmpE9A.aggr_info;
if((_tmpE9B.KnownAggr).tag != 2)goto _LL722;_tmpE9C=(struct Cyc_Absyn_Aggrdecl**)(
_tmpE9B.KnownAggr).val;_tmpE9D=*_tmpE9C;_tmpE9E=_tmpE9A.targs;}}_LL721: _tmpEA0=
_tmpE9D->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE9D->impl))->fields;
goto _LL723;_LL722: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE9F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE98;if(_tmpE9F->tag != 13)goto _LL724;else{_tmpEA0=_tmpE9F->f2;}}_LL723: {
struct Cyc_Absyn_Aggrfield*_tmpEA1=Cyc_Absyn_lookup_field(_tmpEA0,f);{struct Cyc_Absyn_Aggrfield*
_tmpEA2=_tmpEA1;struct Cyc_Absyn_Aggrfield _tmpEA3;void*_tmpEA4;_LL727: if(_tmpEA2
!= 0)goto _LL729;_LL728: {const char*_tmp155F;void*_tmp155E;(_tmp155E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp155F="is_noalias_field: missing field",
_tag_dyneither(_tmp155F,sizeof(char),32))),_tag_dyneither(_tmp155E,sizeof(void*),
0)));}_LL729: if(_tmpEA2 == 0)goto _LL726;_tmpEA3=*_tmpEA2;_tmpEA4=_tmpEA3.type;
_LL72A: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpEA4);_LL726:;}return 0;}
_LL724:;_LL725: {const char*_tmp1563;void*_tmp1562[1];struct Cyc_String_pa_struct
_tmp1561;(_tmp1561.tag=0,((_tmp1561.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1562[0]=& _tmp1561,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1563="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1563,sizeof(char),36))),_tag_dyneither(_tmp1562,sizeof(void*),
1)))))));}_LL71F:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpEAA=e->r;void*_tmpEAC;struct Cyc_Absyn_Exp*
_tmpEAF;struct Cyc_Absyn_Exp*_tmpEB1;struct Cyc_Absyn_Exp*_tmpEB3;struct
_dyneither_ptr*_tmpEB4;struct Cyc_Absyn_Exp*_tmpEB6;struct Cyc_Absyn_Exp*_tmpEB7;
struct Cyc_Absyn_Exp*_tmpEB9;struct Cyc_Absyn_Exp*_tmpEBA;struct Cyc_Absyn_Exp*
_tmpEBC;struct Cyc_Absyn_Exp*_tmpEBE;struct Cyc_Absyn_Stmt*_tmpEC0;_LL72C: {struct
Cyc_Absyn_Var_e_struct*_tmpEAB=(struct Cyc_Absyn_Var_e_struct*)_tmpEAA;if(_tmpEAB->tag
!= 1)goto _LL72E;else{_tmpEAC=(void*)_tmpEAB->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpEAD=(struct Cyc_Absyn_Global_b_struct*)_tmpEAC;if(_tmpEAD->tag != 1)goto _LL72E;};}}
_LL72D: return 0;_LL72E: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEAE=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEAA;if(_tmpEAE->tag != 23)goto _LL730;else{_tmpEAF=_tmpEAE->f1;}}_LL72F:
_tmpEB1=_tmpEAF;goto _LL731;_LL730: {struct Cyc_Absyn_Deref_e_struct*_tmpEB0=(
struct Cyc_Absyn_Deref_e_struct*)_tmpEAA;if(_tmpEB0->tag != 21)goto _LL732;else{
_tmpEB1=_tmpEB0->f1;}}_LL731: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEB1->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEB1);_LL732: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEB2=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpEAA;if(_tmpEB2->tag != 22)goto _LL734;else{_tmpEB3=_tmpEB2->f1;_tmpEB4=_tmpEB2->f2;}}
_LL733: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB3);_LL734: {struct Cyc_Absyn_Subscript_e_struct*
_tmpEB5=(struct Cyc_Absyn_Subscript_e_struct*)_tmpEAA;if(_tmpEB5->tag != 24)goto
_LL736;else{_tmpEB6=_tmpEB5->f1;_tmpEB7=_tmpEB5->f2;}}_LL735: {void*_tmpEC1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEB6->topt))->v);_LL741: {struct Cyc_Absyn_TupleType_struct*
_tmpEC2=(struct Cyc_Absyn_TupleType_struct*)_tmpEC1;if(_tmpEC2->tag != 11)goto
_LL743;}_LL742: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEB6);_LL743:;_LL744:
return 0;_LL740:;}_LL736: {struct Cyc_Absyn_Conditional_e_struct*_tmpEB8=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpEAA;if(_tmpEB8->tag != 6)goto _LL738;else{
_tmpEB9=_tmpEB8->f2;_tmpEBA=_tmpEB8->f3;}}_LL737: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEB9) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpEBA);_LL738: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpEBB=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpEAA;if(_tmpEBB->tag != 9)goto _LL73A;
else{_tmpEBC=_tmpEBB->f2;}}_LL739: _tmpEBE=_tmpEBC;goto _LL73B;_LL73A: {struct Cyc_Absyn_Cast_e_struct*
_tmpEBD=(struct Cyc_Absyn_Cast_e_struct*)_tmpEAA;if(_tmpEBD->tag != 15)goto _LL73C;
else{_tmpEBE=_tmpEBD->f2;}}_LL73B: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEBE);
_LL73C: {struct Cyc_Absyn_StmtExp_e_struct*_tmpEBF=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpEAA;if(_tmpEBF->tag != 37)goto _LL73E;else{_tmpEC0=_tmpEBF->f1;}}_LL73D: while(
1){void*_tmpEC3=_tmpEC0->r;struct Cyc_Absyn_Stmt*_tmpEC5;struct Cyc_Absyn_Stmt*
_tmpEC6;struct Cyc_Absyn_Decl*_tmpEC8;struct Cyc_Absyn_Stmt*_tmpEC9;struct Cyc_Absyn_Exp*
_tmpECB;_LL746: {struct Cyc_Absyn_Seq_s_struct*_tmpEC4=(struct Cyc_Absyn_Seq_s_struct*)
_tmpEC3;if(_tmpEC4->tag != 2)goto _LL748;else{_tmpEC5=_tmpEC4->f1;_tmpEC6=_tmpEC4->f2;}}
_LL747: _tmpEC0=_tmpEC6;goto _LL745;_LL748: {struct Cyc_Absyn_Decl_s_struct*_tmpEC7=(
struct Cyc_Absyn_Decl_s_struct*)_tmpEC3;if(_tmpEC7->tag != 12)goto _LL74A;else{
_tmpEC8=_tmpEC7->f1;_tmpEC9=_tmpEC7->f2;}}_LL749: _tmpEC0=_tmpEC9;goto _LL745;
_LL74A: {struct Cyc_Absyn_Exp_s_struct*_tmpECA=(struct Cyc_Absyn_Exp_s_struct*)
_tmpEC3;if(_tmpECA->tag != 1)goto _LL74C;else{_tmpECB=_tmpECA->f1;}}_LL74B: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpECB);_LL74C:;_LL74D: {const char*_tmp1566;
void*_tmp1565;(_tmp1565=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1566="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1566,sizeof(char),40))),_tag_dyneither(_tmp1565,sizeof(void*),
0)));}_LL745:;}_LL73E:;_LL73F: return 1;_LL72B:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp1567;struct _tuple17 bogus_ans=(_tmp1567.f1=0,((_tmp1567.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1567)));void*_tmpECE=e->r;struct _tuple2*_tmpED0;
void*_tmpED1;struct Cyc_Absyn_Exp*_tmpED3;struct _dyneither_ptr*_tmpED4;int _tmpED5;
struct Cyc_Absyn_Exp*_tmpED7;struct _dyneither_ptr*_tmpED8;int _tmpED9;struct Cyc_Absyn_Exp*
_tmpEDB;struct Cyc_Absyn_Exp*_tmpEDD;struct Cyc_Absyn_Exp*_tmpEDE;_LL74F: {struct
Cyc_Absyn_Var_e_struct*_tmpECF=(struct Cyc_Absyn_Var_e_struct*)_tmpECE;if(_tmpECF->tag
!= 1)goto _LL751;else{_tmpED0=_tmpECF->f1;_tmpED1=(void*)_tmpECF->f2;}}_LL750: {
void*_tmpEDF=_tmpED1;struct Cyc_Absyn_Vardecl*_tmpEE3;struct Cyc_Absyn_Vardecl*
_tmpEE5;struct Cyc_Absyn_Vardecl*_tmpEE7;struct Cyc_Absyn_Vardecl*_tmpEE9;_LL75C: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpEE0=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpEDF;if(_tmpEE0->tag != 0)goto _LL75E;}_LL75D: goto _LL75F;_LL75E: {struct Cyc_Absyn_Funname_b_struct*
_tmpEE1=(struct Cyc_Absyn_Funname_b_struct*)_tmpEDF;if(_tmpEE1->tag != 2)goto
_LL760;}_LL75F: return bogus_ans;_LL760: {struct Cyc_Absyn_Global_b_struct*_tmpEE2=(
struct Cyc_Absyn_Global_b_struct*)_tmpEDF;if(_tmpEE2->tag != 1)goto _LL762;else{
_tmpEE3=_tmpEE2->f1;}}_LL761: {void*_tmpEEA=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL769: {struct Cyc_Absyn_ArrayType_struct*
_tmpEEB=(struct Cyc_Absyn_ArrayType_struct*)_tmpEEA;if(_tmpEEB->tag != 9)goto
_LL76B;}_LL76A: {struct _tuple17 _tmp1568;return(_tmp1568.f1=1,((_tmp1568.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp1568)));}_LL76B:;_LL76C: {struct _tuple17 _tmp1569;
return(_tmp1569.f1=(_tmpEE3->tq).real_const,((_tmp1569.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1569)));}_LL768:;}_LL762: {struct Cyc_Absyn_Local_b_struct*_tmpEE4=(struct Cyc_Absyn_Local_b_struct*)
_tmpEDF;if(_tmpEE4->tag != 4)goto _LL764;else{_tmpEE5=_tmpEE4->f1;}}_LL763: {void*
_tmpEEE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL76E: {struct Cyc_Absyn_ArrayType_struct*_tmpEEF=(struct Cyc_Absyn_ArrayType_struct*)
_tmpEEE;if(_tmpEEF->tag != 9)goto _LL770;}_LL76F: {struct _tuple17 _tmp156A;return(
_tmp156A.f1=1,((_tmp156A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpEE5->rgn))->v,
_tmp156A)));}_LL770:;_LL771: _tmpEE5->escapes=1;{struct _tuple17 _tmp156B;return(
_tmp156B.f1=(_tmpEE5->tq).real_const,((_tmp156B.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE5->rgn))->v,_tmp156B)));};_LL76D:;}_LL764: {struct Cyc_Absyn_Pat_b_struct*
_tmpEE6=(struct Cyc_Absyn_Pat_b_struct*)_tmpEDF;if(_tmpEE6->tag != 5)goto _LL766;
else{_tmpEE7=_tmpEE6->f1;}}_LL765: _tmpEE9=_tmpEE7;goto _LL767;_LL766: {struct Cyc_Absyn_Param_b_struct*
_tmpEE8=(struct Cyc_Absyn_Param_b_struct*)_tmpEDF;if(_tmpEE8->tag != 3)goto _LL75B;
else{_tmpEE9=_tmpEE8->f1;}}_LL767: _tmpEE9->escapes=1;{struct _tuple17 _tmp156C;
return(_tmp156C.f1=(_tmpEE9->tq).real_const,((_tmp156C.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE9->rgn))->v,_tmp156C)));};_LL75B:;}_LL751: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpED2=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpECE;if(_tmpED2->tag != 22)goto
_LL753;else{_tmpED3=_tmpED2->f1;_tmpED4=_tmpED2->f2;_tmpED5=_tmpED2->f3;}}_LL752:
if(_tmpED5)return bogus_ans;{void*_tmpEF3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpED3->topt))->v);struct Cyc_List_List*_tmpEF5;struct Cyc_Absyn_AggrInfo
_tmpEF7;union Cyc_Absyn_AggrInfoU _tmpEF8;struct Cyc_Absyn_Aggrdecl**_tmpEF9;struct
Cyc_Absyn_Aggrdecl*_tmpEFA;_LL773: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEF4=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpEF3;if(_tmpEF4->tag != 13)goto _LL775;
else{_tmpEF5=_tmpEF4->f2;}}_LL774: {struct Cyc_Absyn_Aggrfield*_tmpEFB=Cyc_Absyn_lookup_field(
_tmpEF5,_tmpED4);if(_tmpEFB != 0  && _tmpEFB->width != 0){struct _tuple17 _tmp156D;
return(_tmp156D.f1=(_tmpEFB->tq).real_const,((_tmp156D.f2=(Cyc_Tcutil_addressof_props(
te,_tmpED3)).f2,_tmp156D)));}return bogus_ans;}_LL775: {struct Cyc_Absyn_AggrType_struct*
_tmpEF6=(struct Cyc_Absyn_AggrType_struct*)_tmpEF3;if(_tmpEF6->tag != 12)goto
_LL777;else{_tmpEF7=_tmpEF6->f1;_tmpEF8=_tmpEF7.aggr_info;if((_tmpEF8.KnownAggr).tag
!= 2)goto _LL777;_tmpEF9=(struct Cyc_Absyn_Aggrdecl**)(_tmpEF8.KnownAggr).val;
_tmpEFA=*_tmpEF9;}}_LL776: {struct Cyc_Absyn_Aggrfield*_tmpEFD=Cyc_Absyn_lookup_decl_field(
_tmpEFA,_tmpED4);if(_tmpEFD != 0  && _tmpEFD->width != 0){struct _tuple17 _tmp156E;
return(_tmp156E.f1=(_tmpEFD->tq).real_const,((_tmp156E.f2=(Cyc_Tcutil_addressof_props(
te,_tmpED3)).f2,_tmp156E)));}return bogus_ans;}_LL777:;_LL778: return bogus_ans;
_LL772:;};_LL753: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpED6=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpECE;if(_tmpED6->tag != 23)goto _LL755;else{_tmpED7=_tmpED6->f1;_tmpED8=_tmpED6->f2;
_tmpED9=_tmpED6->f3;}}_LL754: if(_tmpED9)return bogus_ans;{void*_tmpEFF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED7->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF01;void*_tmpF02;struct Cyc_Absyn_PtrAtts _tmpF03;void*_tmpF04;_LL77A: {struct
Cyc_Absyn_PointerType_struct*_tmpF00=(struct Cyc_Absyn_PointerType_struct*)
_tmpEFF;if(_tmpF00->tag != 5)goto _LL77C;else{_tmpF01=_tmpF00->f1;_tmpF02=_tmpF01.elt_typ;
_tmpF03=_tmpF01.ptr_atts;_tmpF04=_tmpF03.rgn;}}_LL77B: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF05=Cyc_Tcutil_compress(_tmpF02);struct Cyc_List_List*_tmpF07;
struct Cyc_Absyn_AggrInfo _tmpF09;union Cyc_Absyn_AggrInfoU _tmpF0A;struct Cyc_Absyn_Aggrdecl**
_tmpF0B;struct Cyc_Absyn_Aggrdecl*_tmpF0C;_LL77F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF06=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF05;if(_tmpF06->tag != 13)goto
_LL781;else{_tmpF07=_tmpF06->f2;}}_LL780: finfo=Cyc_Absyn_lookup_field(_tmpF07,
_tmpED8);goto _LL77E;_LL781: {struct Cyc_Absyn_AggrType_struct*_tmpF08=(struct Cyc_Absyn_AggrType_struct*)
_tmpF05;if(_tmpF08->tag != 12)goto _LL783;else{_tmpF09=_tmpF08->f1;_tmpF0A=_tmpF09.aggr_info;
if((_tmpF0A.KnownAggr).tag != 2)goto _LL783;_tmpF0B=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF0A.KnownAggr).val;_tmpF0C=*_tmpF0B;}}_LL782: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF0C,_tmpED8);goto _LL77E;_LL783:;_LL784: return bogus_ans;_LL77E:;}if(finfo != 0
 && finfo->width != 0){struct _tuple17 _tmp156F;return(_tmp156F.f1=(finfo->tq).real_const,((
_tmp156F.f2=_tmpF04,_tmp156F)));}return bogus_ans;}_LL77C:;_LL77D: return bogus_ans;
_LL779:;};_LL755: {struct Cyc_Absyn_Deref_e_struct*_tmpEDA=(struct Cyc_Absyn_Deref_e_struct*)
_tmpECE;if(_tmpEDA->tag != 21)goto _LL757;else{_tmpEDB=_tmpEDA->f1;}}_LL756: {void*
_tmpF0E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpEDB->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF10;struct Cyc_Absyn_Tqual _tmpF11;struct Cyc_Absyn_PtrAtts
_tmpF12;void*_tmpF13;_LL786: {struct Cyc_Absyn_PointerType_struct*_tmpF0F=(struct
Cyc_Absyn_PointerType_struct*)_tmpF0E;if(_tmpF0F->tag != 5)goto _LL788;else{
_tmpF10=_tmpF0F->f1;_tmpF11=_tmpF10.elt_tq;_tmpF12=_tmpF10.ptr_atts;_tmpF13=
_tmpF12.rgn;}}_LL787: {struct _tuple17 _tmp1570;return(_tmp1570.f1=_tmpF11.real_const,((
_tmp1570.f2=_tmpF13,_tmp1570)));}_LL788:;_LL789: return bogus_ans;_LL785:;}_LL757: {
struct Cyc_Absyn_Subscript_e_struct*_tmpEDC=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpECE;if(_tmpEDC->tag != 24)goto _LL759;else{_tmpEDD=_tmpEDC->f1;_tmpEDE=_tmpEDC->f2;}}
_LL758: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpEDD->topt))->v);void*_tmpF15=t;struct Cyc_List_List*_tmpF17;struct Cyc_Absyn_PtrInfo
_tmpF19;struct Cyc_Absyn_Tqual _tmpF1A;struct Cyc_Absyn_PtrAtts _tmpF1B;void*_tmpF1C;
struct Cyc_Absyn_ArrayInfo _tmpF1E;struct Cyc_Absyn_Tqual _tmpF1F;_LL78B: {struct Cyc_Absyn_TupleType_struct*
_tmpF16=(struct Cyc_Absyn_TupleType_struct*)_tmpF15;if(_tmpF16->tag != 11)goto
_LL78D;else{_tmpF17=_tmpF16->f1;}}_LL78C: {unsigned int _tmpF21;int _tmpF22;struct
_tuple13 _tmpF20=Cyc_Evexp_eval_const_uint_exp(_tmpEDE);_tmpF21=_tmpF20.f1;
_tmpF22=_tmpF20.f2;if(!_tmpF22)return bogus_ans;{struct _tuple11*_tmpF23=Cyc_Absyn_lookup_tuple_field(
_tmpF17,(int)_tmpF21);if(_tmpF23 != 0){struct _tuple17 _tmp1571;return(_tmp1571.f1=((*
_tmpF23).f1).real_const,((_tmp1571.f2=(Cyc_Tcutil_addressof_props(te,_tmpEDD)).f2,
_tmp1571)));}return bogus_ans;};}_LL78D: {struct Cyc_Absyn_PointerType_struct*
_tmpF18=(struct Cyc_Absyn_PointerType_struct*)_tmpF15;if(_tmpF18->tag != 5)goto
_LL78F;else{_tmpF19=_tmpF18->f1;_tmpF1A=_tmpF19.elt_tq;_tmpF1B=_tmpF19.ptr_atts;
_tmpF1C=_tmpF1B.rgn;}}_LL78E: {struct _tuple17 _tmp1572;return(_tmp1572.f1=_tmpF1A.real_const,((
_tmp1572.f2=_tmpF1C,_tmp1572)));}_LL78F: {struct Cyc_Absyn_ArrayType_struct*
_tmpF1D=(struct Cyc_Absyn_ArrayType_struct*)_tmpF15;if(_tmpF1D->tag != 9)goto
_LL791;else{_tmpF1E=_tmpF1D->f1;_tmpF1F=_tmpF1E.tq;}}_LL790: {struct _tuple17
_tmp1573;return(_tmp1573.f1=_tmpF1F.real_const,((_tmp1573.f2=(Cyc_Tcutil_addressof_props(
te,_tmpEDD)).f2,_tmp1573)));}_LL791:;_LL792: return bogus_ans;_LL78A:;}_LL759:;
_LL75A:{const char*_tmp1576;void*_tmp1575;(_tmp1575=0,Cyc_Tcutil_terr(e->loc,((
_tmp1576="unary & applied to non-lvalue",_tag_dyneither(_tmp1576,sizeof(char),30))),
_tag_dyneither(_tmp1575,sizeof(void*),0)));}return bogus_ans;_LL74E:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpF2A=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpF2C;void*_tmpF2D;struct Cyc_Absyn_Tqual
_tmpF2E;union Cyc_Absyn_Constraint*_tmpF2F;_LL794: {struct Cyc_Absyn_ArrayType_struct*
_tmpF2B=(struct Cyc_Absyn_ArrayType_struct*)_tmpF2A;if(_tmpF2B->tag != 9)goto
_LL796;else{_tmpF2C=_tmpF2B->f1;_tmpF2D=_tmpF2C.elt_type;_tmpF2E=_tmpF2C.tq;
_tmpF2F=_tmpF2C.zero_term;}}_LL795: {void*_tmpF31;struct _tuple17 _tmpF30=Cyc_Tcutil_addressof_props(
te,e);_tmpF31=_tmpF30.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1579;struct Cyc_Absyn_Upper_b_struct*
_tmp1578;return Cyc_Absyn_atb_typ(_tmpF2D,_tmpF31,_tmpF2E,(void*)((_tmp1578=
_cycalloc(sizeof(*_tmp1578)),((_tmp1578[0]=((_tmp1579.tag=1,((_tmp1579.f1=e,
_tmp1579)))),_tmp1578)))),_tmpF2F);};}_LL796:;_LL797: return e_typ;_LL793:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpF34=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpF37;_LL799: {struct Cyc_Absyn_DynEither_b_struct*
_tmpF35=(struct Cyc_Absyn_DynEither_b_struct*)_tmpF34;if(_tmpF35->tag != 0)goto
_LL79B;}_LL79A: return;_LL79B: {struct Cyc_Absyn_Upper_b_struct*_tmpF36=(struct Cyc_Absyn_Upper_b_struct*)
_tmpF34;if(_tmpF36->tag != 1)goto _LL798;else{_tmpF37=_tmpF36->f1;}}_LL79C: {
unsigned int _tmpF39;int _tmpF3A;struct _tuple13 _tmpF38=Cyc_Evexp_eval_const_uint_exp(
_tmpF37);_tmpF39=_tmpF38.f1;_tmpF3A=_tmpF38.f2;if(_tmpF3A  && _tmpF39 <= i){const
char*_tmp157E;void*_tmp157D[2];struct Cyc_Int_pa_struct _tmp157C;struct Cyc_Int_pa_struct
_tmp157B;(_tmp157B.tag=1,((_tmp157B.f1=(unsigned long)((int)i),((_tmp157C.tag=1,((
_tmp157C.f1=(unsigned long)((int)_tmpF39),((_tmp157D[0]=& _tmp157C,((_tmp157D[1]=&
_tmp157B,Cyc_Tcutil_terr(loc,((_tmp157E="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp157E,sizeof(char),39))),_tag_dyneither(_tmp157D,sizeof(void*),
2)))))))))))));}return;}_LL798:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpF3F=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpF41;_LL79E: {struct Cyc_Absyn_Upper_b_struct*_tmpF40=(struct Cyc_Absyn_Upper_b_struct*)
_tmpF3F;if(_tmpF40->tag != 1)goto _LL7A0;else{_tmpF41=_tmpF40->f1;}}_LL79F: {
unsigned int _tmpF43;int _tmpF44;struct _tuple13 _tmpF42=Cyc_Evexp_eval_const_uint_exp(
_tmpF41);_tmpF43=_tmpF42.f1;_tmpF44=_tmpF42.f2;return _tmpF44  && _tmpF43 == 1;}
_LL7A0:;_LL7A1: return 0;_LL79D:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpF45=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpF4D;void*_tmpF4E;union Cyc_Absyn_Constraint*
_tmpF4F;struct Cyc_List_List*_tmpF51;struct Cyc_Absyn_AggrInfo _tmpF53;union Cyc_Absyn_AggrInfoU
_tmpF54;struct _tuple4 _tmpF55;struct Cyc_Absyn_AggrInfo _tmpF57;union Cyc_Absyn_AggrInfoU
_tmpF58;struct Cyc_Absyn_Aggrdecl**_tmpF59;struct Cyc_Absyn_Aggrdecl*_tmpF5A;
struct Cyc_List_List*_tmpF5B;struct Cyc_List_List*_tmpF5D;_LL7A3: {struct Cyc_Absyn_VoidType_struct*
_tmpF46=(struct Cyc_Absyn_VoidType_struct*)_tmpF45;if(_tmpF46->tag != 0)goto _LL7A5;}
_LL7A4: goto _LL7A6;_LL7A5: {struct Cyc_Absyn_IntType_struct*_tmpF47=(struct Cyc_Absyn_IntType_struct*)
_tmpF45;if(_tmpF47->tag != 6)goto _LL7A7;}_LL7A6: goto _LL7A8;_LL7A7: {struct Cyc_Absyn_FloatType_struct*
_tmpF48=(struct Cyc_Absyn_FloatType_struct*)_tmpF45;if(_tmpF48->tag != 7)goto
_LL7A9;}_LL7A8: goto _LL7AA;_LL7A9: {struct Cyc_Absyn_DoubleType_struct*_tmpF49=(
struct Cyc_Absyn_DoubleType_struct*)_tmpF45;if(_tmpF49->tag != 8)goto _LL7AB;}
_LL7AA: return 1;_LL7AB: {struct Cyc_Absyn_EnumType_struct*_tmpF4A=(struct Cyc_Absyn_EnumType_struct*)
_tmpF45;if(_tmpF4A->tag != 14)goto _LL7AD;}_LL7AC: goto _LL7AE;_LL7AD: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpF4B=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF45;if(_tmpF4B->tag != 15)goto
_LL7AF;}_LL7AE: return 0;_LL7AF: {struct Cyc_Absyn_ArrayType_struct*_tmpF4C=(struct
Cyc_Absyn_ArrayType_struct*)_tmpF45;if(_tmpF4C->tag != 9)goto _LL7B1;else{_tmpF4D=
_tmpF4C->f1;_tmpF4E=_tmpF4D.elt_type;_tmpF4F=_tmpF4D.zero_term;}}_LL7B0: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF4F) && Cyc_Tcutil_bits_only(
_tmpF4E);_LL7B1: {struct Cyc_Absyn_TupleType_struct*_tmpF50=(struct Cyc_Absyn_TupleType_struct*)
_tmpF45;if(_tmpF50->tag != 11)goto _LL7B3;else{_tmpF51=_tmpF50->f1;}}_LL7B2: for(0;
_tmpF51 != 0;_tmpF51=_tmpF51->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpF51->hd)).f2))return 0;}return 1;_LL7B3: {struct Cyc_Absyn_AggrType_struct*
_tmpF52=(struct Cyc_Absyn_AggrType_struct*)_tmpF45;if(_tmpF52->tag != 12)goto
_LL7B5;else{_tmpF53=_tmpF52->f1;_tmpF54=_tmpF53.aggr_info;if((_tmpF54.UnknownAggr).tag
!= 1)goto _LL7B5;_tmpF55=(struct _tuple4)(_tmpF54.UnknownAggr).val;}}_LL7B4: return
0;_LL7B5: {struct Cyc_Absyn_AggrType_struct*_tmpF56=(struct Cyc_Absyn_AggrType_struct*)
_tmpF45;if(_tmpF56->tag != 12)goto _LL7B7;else{_tmpF57=_tmpF56->f1;_tmpF58=_tmpF57.aggr_info;
if((_tmpF58.KnownAggr).tag != 2)goto _LL7B7;_tmpF59=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF58.KnownAggr).val;_tmpF5A=*_tmpF59;_tmpF5B=_tmpF57.targs;}}_LL7B6: if(
_tmpF5A->impl == 0)return 0;{struct _RegionHandle _tmpF5E=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpF5E;_push_region(rgn);{struct Cyc_List_List*_tmpF5F=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF5A->tvs,_tmpF5B);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF5A->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpF5F,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpF60=0;_npop_handler(0);return
_tmpF60;}}}{int _tmpF61=1;_npop_handler(0);return _tmpF61;};};_pop_region(rgn);};
_LL7B7: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF5C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF45;if(_tmpF5C->tag != 13)goto _LL7B9;else{_tmpF5D=_tmpF5C->f2;}}_LL7B8: for(0;
_tmpF5D != 0;_tmpF5D=_tmpF5D->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpF5D->hd)->type))return 0;}return 1;_LL7B9:;_LL7BA: return 0;_LL7A2:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpF62=e->r;struct _tuple2*_tmpF6A;void*_tmpF6B;struct Cyc_Absyn_Exp*_tmpF6D;
struct Cyc_Absyn_Exp*_tmpF6E;struct Cyc_Absyn_Exp*_tmpF6F;struct Cyc_Absyn_Exp*
_tmpF71;struct Cyc_Absyn_Exp*_tmpF72;struct Cyc_Absyn_Exp*_tmpF74;struct Cyc_Absyn_Exp*
_tmpF76;void*_tmpF78;struct Cyc_Absyn_Exp*_tmpF79;enum Cyc_Absyn_Coercion _tmpF7A;
void*_tmpF7C;struct Cyc_Absyn_Exp*_tmpF7D;struct Cyc_Absyn_Exp*_tmpF7F;struct Cyc_Absyn_Exp*
_tmpF81;struct Cyc_Absyn_Exp*_tmpF82;struct Cyc_List_List*_tmpF84;struct Cyc_List_List*
_tmpF86;struct Cyc_List_List*_tmpF88;enum Cyc_Absyn_Primop _tmpF8A;struct Cyc_List_List*
_tmpF8B;struct Cyc_List_List*_tmpF8D;struct Cyc_List_List*_tmpF8F;_LL7BC: {struct
Cyc_Absyn_Const_e_struct*_tmpF63=(struct Cyc_Absyn_Const_e_struct*)_tmpF62;if(
_tmpF63->tag != 0)goto _LL7BE;}_LL7BD: goto _LL7BF;_LL7BE: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpF64=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpF62;if(_tmpF64->tag != 18)goto
_LL7C0;}_LL7BF: goto _LL7C1;_LL7C0: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpF65=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpF62;if(_tmpF65->tag != 19)goto _LL7C2;}
_LL7C1: goto _LL7C3;_LL7C2: {struct Cyc_Absyn_Offsetof_e_struct*_tmpF66=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpF62;if(_tmpF66->tag != 20)goto _LL7C4;}_LL7C3: goto _LL7C5;_LL7C4: {struct Cyc_Absyn_Enum_e_struct*
_tmpF67=(struct Cyc_Absyn_Enum_e_struct*)_tmpF62;if(_tmpF67->tag != 32)goto _LL7C6;}
_LL7C5: goto _LL7C7;_LL7C6: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpF68=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpF62;if(_tmpF68->tag != 33)goto _LL7C8;}_LL7C7: return 1;_LL7C8: {struct Cyc_Absyn_Var_e_struct*
_tmpF69=(struct Cyc_Absyn_Var_e_struct*)_tmpF62;if(_tmpF69->tag != 1)goto _LL7CA;
else{_tmpF6A=_tmpF69->f1;_tmpF6B=(void*)_tmpF69->f2;}}_LL7C9: {void*_tmpF90=
_tmpF6B;struct Cyc_Absyn_Vardecl*_tmpF93;_LL7E9: {struct Cyc_Absyn_Funname_b_struct*
_tmpF91=(struct Cyc_Absyn_Funname_b_struct*)_tmpF90;if(_tmpF91->tag != 2)goto
_LL7EB;}_LL7EA: return 1;_LL7EB: {struct Cyc_Absyn_Global_b_struct*_tmpF92=(struct
Cyc_Absyn_Global_b_struct*)_tmpF90;if(_tmpF92->tag != 1)goto _LL7ED;else{_tmpF93=
_tmpF92->f1;}}_LL7EC: {void*_tmpF95=Cyc_Tcutil_compress(_tmpF93->type);_LL7F2: {
struct Cyc_Absyn_ArrayType_struct*_tmpF96=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF95;if(_tmpF96->tag != 9)goto _LL7F4;}_LL7F3: goto _LL7F5;_LL7F4: {struct Cyc_Absyn_FnType_struct*
_tmpF97=(struct Cyc_Absyn_FnType_struct*)_tmpF95;if(_tmpF97->tag != 10)goto _LL7F6;}
_LL7F5: return 1;_LL7F6:;_LL7F7: return var_okay;_LL7F1:;}_LL7ED: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpF94=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpF90;if(_tmpF94->tag != 0)goto
_LL7EF;}_LL7EE: return 0;_LL7EF:;_LL7F0: return var_okay;_LL7E8:;}_LL7CA: {struct Cyc_Absyn_Conditional_e_struct*
_tmpF6C=(struct Cyc_Absyn_Conditional_e_struct*)_tmpF62;if(_tmpF6C->tag != 6)goto
_LL7CC;else{_tmpF6D=_tmpF6C->f1;_tmpF6E=_tmpF6C->f2;_tmpF6F=_tmpF6C->f3;}}_LL7CB:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpF6D) && Cyc_Tcutil_cnst_exp(te,0,_tmpF6E))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpF6F);_LL7CC: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF70=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF62;if(_tmpF70->tag != 9)goto _LL7CE;
else{_tmpF71=_tmpF70->f1;_tmpF72=_tmpF70->f2;}}_LL7CD: return Cyc_Tcutil_cnst_exp(
te,0,_tmpF71) && Cyc_Tcutil_cnst_exp(te,0,_tmpF72);_LL7CE: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpF73=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpF62;if(_tmpF73->tag != 13)
goto _LL7D0;else{_tmpF74=_tmpF73->f1;}}_LL7CF: _tmpF76=_tmpF74;goto _LL7D1;_LL7D0: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpF75=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpF62;if(_tmpF75->tag != 14)goto _LL7D2;else{_tmpF76=_tmpF75->f1;}}_LL7D1: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpF76);_LL7D2: {struct Cyc_Absyn_Cast_e_struct*
_tmpF77=(struct Cyc_Absyn_Cast_e_struct*)_tmpF62;if(_tmpF77->tag != 15)goto _LL7D4;
else{_tmpF78=(void*)_tmpF77->f1;_tmpF79=_tmpF77->f2;_tmpF7A=_tmpF77->f4;if(
_tmpF7A != Cyc_Absyn_No_coercion)goto _LL7D4;}}_LL7D3: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpF79);_LL7D4: {struct Cyc_Absyn_Cast_e_struct*_tmpF7B=(struct Cyc_Absyn_Cast_e_struct*)
_tmpF62;if(_tmpF7B->tag != 15)goto _LL7D6;else{_tmpF7C=(void*)_tmpF7B->f1;_tmpF7D=
_tmpF7B->f2;}}_LL7D5: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpF7D);_LL7D6: {
struct Cyc_Absyn_Address_e_struct*_tmpF7E=(struct Cyc_Absyn_Address_e_struct*)
_tmpF62;if(_tmpF7E->tag != 16)goto _LL7D8;else{_tmpF7F=_tmpF7E->f1;}}_LL7D7: return
Cyc_Tcutil_cnst_exp(te,1,_tmpF7F);_LL7D8: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpF80=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpF62;if(_tmpF80->tag != 28)
goto _LL7DA;else{_tmpF81=_tmpF80->f2;_tmpF82=_tmpF80->f3;}}_LL7D9: return Cyc_Tcutil_cnst_exp(
te,0,_tmpF81) && Cyc_Tcutil_cnst_exp(te,0,_tmpF82);_LL7DA: {struct Cyc_Absyn_Array_e_struct*
_tmpF83=(struct Cyc_Absyn_Array_e_struct*)_tmpF62;if(_tmpF83->tag != 27)goto _LL7DC;
else{_tmpF84=_tmpF83->f1;}}_LL7DB: _tmpF86=_tmpF84;goto _LL7DD;_LL7DC: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpF85=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpF62;if(_tmpF85->tag != 30)goto
_LL7DE;else{_tmpF86=_tmpF85->f2;}}_LL7DD: _tmpF88=_tmpF86;goto _LL7DF;_LL7DE: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpF87=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpF62;if(_tmpF87->tag != 29)goto _LL7E0;else{_tmpF88=_tmpF87->f3;}}_LL7DF: for(0;
_tmpF88 != 0;_tmpF88=_tmpF88->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)
_tmpF88->hd)).f2))return 0;}return 1;_LL7E0: {struct Cyc_Absyn_Primop_e_struct*
_tmpF89=(struct Cyc_Absyn_Primop_e_struct*)_tmpF62;if(_tmpF89->tag != 3)goto _LL7E2;
else{_tmpF8A=_tmpF89->f1;_tmpF8B=_tmpF89->f2;}}_LL7E1: _tmpF8D=_tmpF8B;goto _LL7E3;
_LL7E2: {struct Cyc_Absyn_Tuple_e_struct*_tmpF8C=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpF62;if(_tmpF8C->tag != 25)goto _LL7E4;else{_tmpF8D=_tmpF8C->f1;}}_LL7E3:
_tmpF8F=_tmpF8D;goto _LL7E5;_LL7E4: {struct Cyc_Absyn_Datatype_e_struct*_tmpF8E=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpF62;if(_tmpF8E->tag != 31)goto _LL7E6;else{
_tmpF8F=_tmpF8E->f1;}}_LL7E5: for(0;_tmpF8F != 0;_tmpF8F=_tmpF8F->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpF8F->hd))return 0;}return 1;_LL7E6:;_LL7E7: return 0;
_LL7BB:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpF98=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpF9E;
struct Cyc_Absyn_PtrAtts _tmpF9F;union Cyc_Absyn_Constraint*_tmpFA0;union Cyc_Absyn_Constraint*
_tmpFA1;struct Cyc_Absyn_ArrayInfo _tmpFA3;void*_tmpFA4;struct Cyc_List_List*
_tmpFA6;struct Cyc_Absyn_AggrInfo _tmpFA8;union Cyc_Absyn_AggrInfoU _tmpFA9;struct
Cyc_List_List*_tmpFAA;struct Cyc_List_List*_tmpFAC;_LL7F9: {struct Cyc_Absyn_VoidType_struct*
_tmpF99=(struct Cyc_Absyn_VoidType_struct*)_tmpF98;if(_tmpF99->tag != 0)goto _LL7FB;}
_LL7FA: goto _LL7FC;_LL7FB: {struct Cyc_Absyn_IntType_struct*_tmpF9A=(struct Cyc_Absyn_IntType_struct*)
_tmpF98;if(_tmpF9A->tag != 6)goto _LL7FD;}_LL7FC: goto _LL7FE;_LL7FD: {struct Cyc_Absyn_FloatType_struct*
_tmpF9B=(struct Cyc_Absyn_FloatType_struct*)_tmpF98;if(_tmpF9B->tag != 7)goto
_LL7FF;}_LL7FE: goto _LL800;_LL7FF: {struct Cyc_Absyn_DoubleType_struct*_tmpF9C=(
struct Cyc_Absyn_DoubleType_struct*)_tmpF98;if(_tmpF9C->tag != 8)goto _LL801;}
_LL800: return 1;_LL801: {struct Cyc_Absyn_PointerType_struct*_tmpF9D=(struct Cyc_Absyn_PointerType_struct*)
_tmpF98;if(_tmpF9D->tag != 5)goto _LL803;else{_tmpF9E=_tmpF9D->f1;_tmpF9F=_tmpF9E.ptr_atts;
_tmpFA0=_tmpF9F.nullable;_tmpFA1=_tmpF9F.bounds;}}_LL802: {void*_tmpFAF=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpFA1);_LL812: {struct Cyc_Absyn_DynEither_b_struct*_tmpFB0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFAF;if(_tmpFB0->tag != 0)goto _LL814;}_LL813: return 1;_LL814:;_LL815: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpFA0);_LL811:;}
_LL803: {struct Cyc_Absyn_ArrayType_struct*_tmpFA2=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF98;if(_tmpFA2->tag != 9)goto _LL805;else{_tmpFA3=_tmpFA2->f1;_tmpFA4=_tmpFA3.elt_type;}}
_LL804: return Cyc_Tcutil_supports_default(_tmpFA4);_LL805: {struct Cyc_Absyn_TupleType_struct*
_tmpFA5=(struct Cyc_Absyn_TupleType_struct*)_tmpF98;if(_tmpFA5->tag != 11)goto
_LL807;else{_tmpFA6=_tmpFA5->f1;}}_LL806: for(0;_tmpFA6 != 0;_tmpFA6=_tmpFA6->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpFA6->hd)).f2))return 0;}
return 1;_LL807: {struct Cyc_Absyn_AggrType_struct*_tmpFA7=(struct Cyc_Absyn_AggrType_struct*)
_tmpF98;if(_tmpFA7->tag != 12)goto _LL809;else{_tmpFA8=_tmpFA7->f1;_tmpFA9=_tmpFA8.aggr_info;
_tmpFAA=_tmpFA8.targs;}}_LL808: {struct Cyc_Absyn_Aggrdecl*_tmpFB1=Cyc_Absyn_get_known_aggrdecl(
_tmpFA9);if(_tmpFB1->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpFB1->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpFB1->tvs,_tmpFAA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFB1->impl))->fields);}
_LL809: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFAB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF98;if(_tmpFAB->tag != 13)goto _LL80B;else{_tmpFAC=_tmpFAB->f2;}}_LL80A: return
Cyc_Tcutil_fields_support_default(0,0,_tmpFAC);_LL80B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFAD=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF98;if(_tmpFAD->tag != 15)goto
_LL80D;}_LL80C: goto _LL80E;_LL80D: {struct Cyc_Absyn_EnumType_struct*_tmpFAE=(
struct Cyc_Absyn_EnumType_struct*)_tmpF98;if(_tmpFAE->tag != 14)goto _LL80F;}_LL80E:
return 1;_LL80F:;_LL810: return 0;_LL7F8:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmpFB2=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFB2;
_push_region(rgn);{struct Cyc_List_List*_tmpFB3=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmpFB3,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmpFB4=0;_npop_handler(0);return _tmpFB4;}}}{int _tmpFB5=1;_npop_handler(0);
return _tmpFB5;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(void*t){void*
_tmpFB6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFBB;struct Cyc_Absyn_PtrAtts
_tmpFBC;union Cyc_Absyn_Constraint*_tmpFBD;union Cyc_Absyn_Constraint*_tmpFBE;
_LL817: {struct Cyc_Absyn_IntType_struct*_tmpFB7=(struct Cyc_Absyn_IntType_struct*)
_tmpFB6;if(_tmpFB7->tag != 6)goto _LL819;}_LL818: goto _LL81A;_LL819: {struct Cyc_Absyn_FloatType_struct*
_tmpFB8=(struct Cyc_Absyn_FloatType_struct*)_tmpFB6;if(_tmpFB8->tag != 7)goto
_LL81B;}_LL81A: goto _LL81C;_LL81B: {struct Cyc_Absyn_DoubleType_struct*_tmpFB9=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFB6;if(_tmpFB9->tag != 8)goto _LL81D;}
_LL81C: return 1;_LL81D: {struct Cyc_Absyn_PointerType_struct*_tmpFBA=(struct Cyc_Absyn_PointerType_struct*)
_tmpFB6;if(_tmpFBA->tag != 5)goto _LL81F;else{_tmpFBB=_tmpFBA->f1;_tmpFBC=_tmpFBB.ptr_atts;
_tmpFBD=_tmpFBC.nullable;_tmpFBE=_tmpFBC.bounds;}}_LL81E: {void*_tmpFBF=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpFBE);_LL822: {struct Cyc_Absyn_DynEither_b_struct*_tmpFC0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFBF;if(_tmpFC0->tag != 0)goto _LL824;}_LL823: return 0;_LL824:;_LL825: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFBD);_LL821:;}
_LL81F:;_LL820: return 0;_LL816:;}int Cyc_Tcutil_is_noreturn(void*t){{void*_tmpFC1=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFC3;void*_tmpFC4;struct Cyc_Absyn_FnInfo
_tmpFC6;struct Cyc_List_List*_tmpFC7;_LL827: {struct Cyc_Absyn_PointerType_struct*
_tmpFC2=(struct Cyc_Absyn_PointerType_struct*)_tmpFC1;if(_tmpFC2->tag != 5)goto
_LL829;else{_tmpFC3=_tmpFC2->f1;_tmpFC4=_tmpFC3.elt_typ;}}_LL828: return Cyc_Tcutil_is_noreturn(
_tmpFC4);_LL829: {struct Cyc_Absyn_FnType_struct*_tmpFC5=(struct Cyc_Absyn_FnType_struct*)
_tmpFC1;if(_tmpFC5->tag != 10)goto _LL82B;else{_tmpFC6=_tmpFC5->f1;_tmpFC7=_tmpFC6.attributes;}}
_LL82A: for(0;_tmpFC7 != 0;_tmpFC7=_tmpFC7->tl){void*_tmpFC8=(void*)_tmpFC7->hd;
_LL82E: {struct Cyc_Absyn_Noreturn_att_struct*_tmpFC9=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmpFC8;if(_tmpFC9->tag != 4)goto _LL830;}_LL82F: return 1;_LL830:;_LL831: continue;
_LL82D:;}goto _LL826;_LL82B:;_LL82C: goto _LL826;_LL826:;}return 0;}struct Cyc_List_List*
Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){void*_tmpFCA=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpFCC;struct Cyc_List_List*_tmpFCD;
struct Cyc_List_List**_tmpFCE;_LL833: {struct Cyc_Absyn_FnType_struct*_tmpFCB=(
struct Cyc_Absyn_FnType_struct*)_tmpFCA;if(_tmpFCB->tag != 10)goto _LL835;else{
_tmpFCC=_tmpFCB->f1;_tmpFCD=_tmpFCC.attributes;_tmpFCE=(struct Cyc_List_List**)&(
_tmpFCB->f1).attributes;}}_LL834: {struct Cyc_List_List*_tmpFCF=0;for(0;atts != 0;
atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp157F;*_tmpFCE=((_tmp157F=_cycalloc(sizeof(*_tmp157F)),((_tmp157F->hd=(void*)
atts->hd,((_tmp157F->tl=*_tmpFCE,_tmp157F))))));}else{struct Cyc_List_List*
_tmp1580;_tmpFCF=((_tmp1580=_cycalloc(sizeof(*_tmp1580)),((_tmp1580->hd=(void*)
atts->hd,((_tmp1580->tl=_tmpFCF,_tmp1580))))));}}return _tmpFCF;}_LL835:;_LL836: {
const char*_tmp1583;void*_tmp1582;(_tmp1582=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1583="transfer_fn_type_atts",
_tag_dyneither(_tmp1583,sizeof(char),22))),_tag_dyneither(_tmp1582,sizeof(void*),
0)));}_LL832:;}
