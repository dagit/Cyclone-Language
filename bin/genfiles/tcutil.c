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
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
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
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};inline static void* arrcast(
struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){if(bd >> 20  || sz >> 12)
return 0;{unsigned char*ptrbd=dyn.curr + bd * sz;if(((ptrbd < dyn.curr  || dyn.curr == 
0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)return 0;return dyn.curr;};}
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);void*Cyc_List_hd(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,
int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
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
Cyc_Core_Opt*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_RefCntRgn_struct{
int tag;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_TypeDeclType_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;
};extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct
Cyc_Absyn_UniqueRgn_val;extern struct Cyc_Absyn_RefCntRgn_struct Cyc_Absyn_RefCntRgn_val;
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
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
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
struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*
x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(
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
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;
int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;
int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;
int print_externC_stmts;int print_full_evars;int print_zeroterm;int
generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_region(
void*r,int must_be_unique);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);struct _tuple17{int f1;void*f2;};struct _tuple17 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*loc,void*t);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{
struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void Cyc_Tc_tcAggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp1089;void*_tmp1088[2];const char*
_tmp1087;const char*_tmp1086;struct Cyc_String_pa_struct _tmp1085;struct Cyc_String_pa_struct
_tmp1084;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp1084.tag=0,((
_tmp1084.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp1085.tag=0,((_tmp1085.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp1086="const ",_tag_dyneither(_tmp1086,sizeof(char),7)):((
_tmp1087="",_tag_dyneither(_tmp1087,sizeof(char),1)))),((_tmp1088[0]=& _tmp1085,((
_tmp1088[1]=& _tmp1084,Cyc_aprintf(((_tmp1089="%s%s",_tag_dyneither(_tmp1089,
sizeof(char),5))),_tag_dyneither(_tmp1088,sizeof(void*),2))))))))))))));const
char*_tmp1092;void*_tmp1091[2];const char*_tmp1090;const char*_tmp108F;struct Cyc_String_pa_struct
_tmp108E;struct Cyc_String_pa_struct _tmp108D;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp108D.tag=0,((_tmp108D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp108E.tag=0,((
_tmp108E.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp108F="const ",
_tag_dyneither(_tmp108F,sizeof(char),7)):((_tmp1090="",_tag_dyneither(_tmp1090,
sizeof(char),1)))),((_tmp1091[0]=& _tmp108E,((_tmp1091[1]=& _tmp108D,Cyc_aprintf(((
_tmp1092="%s%s",_tag_dyneither(_tmp1092,sizeof(char),5))),_tag_dyneither(
_tmp1091,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp1096;void*
_tmp1095[1];struct Cyc_String_pa_struct _tmp1094;(_tmp1094.tag=0,((_tmp1094.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1095[0]=& _tmp1094,Cyc_fprintf(
Cyc_stderr,((_tmp1096="  %s",_tag_dyneither(_tmp1096,sizeof(char),5))),
_tag_dyneither(_tmp1095,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp1099;void*_tmp1098;(_tmp1098=0,Cyc_fprintf(
Cyc_stderr,((_tmp1099="\n\t",_tag_dyneither(_tmp1099,sizeof(char),3))),
_tag_dyneither(_tmp1098,sizeof(void*),0)));}pos=8;}else{{const char*_tmp109C;void*
_tmp109B;(_tmp109B=0,Cyc_fprintf(Cyc_stderr,((_tmp109C=" ",_tag_dyneither(
_tmp109C,sizeof(char),2))),_tag_dyneither(_tmp109B,sizeof(void*),0)));}++ pos;}{
const char*_tmp109F;void*_tmp109E;(_tmp109E=0,Cyc_fprintf(Cyc_stderr,((_tmp109F="and ",
_tag_dyneither(_tmp109F,sizeof(char),5))),_tag_dyneither(_tmp109E,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp10A2;void*_tmp10A1;(_tmp10A1=0,Cyc_fprintf(Cyc_stderr,((_tmp10A2="\n\t",
_tag_dyneither(_tmp10A2,sizeof(char),3))),_tag_dyneither(_tmp10A1,sizeof(void*),
0)));}pos=8;}{const char*_tmp10A6;void*_tmp10A5[1];struct Cyc_String_pa_struct
_tmp10A4;(_tmp10A4.tag=0,((_tmp10A4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp10A5[0]=& _tmp10A4,Cyc_fprintf(Cyc_stderr,((_tmp10A6="%s ",
_tag_dyneither(_tmp10A6,sizeof(char),4))),_tag_dyneither(_tmp10A5,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp10A9;void*_tmp10A8;(_tmp10A8=0,Cyc_fprintf(Cyc_stderr,((_tmp10A9="\n\t",
_tag_dyneither(_tmp10A9,sizeof(char),3))),_tag_dyneither(_tmp10A8,sizeof(void*),
0)));}pos=8;}{const char*_tmp10AC;void*_tmp10AB;(_tmp10AB=0,Cyc_fprintf(Cyc_stderr,((
_tmp10AC="are not compatible. ",_tag_dyneither(_tmp10AC,sizeof(char),21))),
_tag_dyneither(_tmp10AB,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp10AF;void*_tmp10AE;(_tmp10AE=0,
Cyc_fprintf(Cyc_stderr,((_tmp10AF="\n\t",_tag_dyneither(_tmp10AF,sizeof(char),3))),
_tag_dyneither(_tmp10AE,sizeof(void*),0)));}{const char*_tmp10B3;void*_tmp10B2[1];
struct Cyc_String_pa_struct _tmp10B1;(_tmp10B1.tag=0,((_tmp10B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp10B2[0]=&
_tmp10B1,Cyc_fprintf(Cyc_stderr,((_tmp10B3="%s",_tag_dyneither(_tmp10B3,sizeof(
char),3))),_tag_dyneither(_tmp10B2,sizeof(void*),1)))))));};}{const char*_tmp10B6;
void*_tmp10B5;(_tmp10B5=0,Cyc_fprintf(Cyc_stderr,((_tmp10B6="\n",_tag_dyneither(
_tmp10B6,sizeof(char),2))),_tag_dyneither(_tmp10B5,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp10BA;
void*_tmp10B9[1];struct Cyc_String_pa_struct _tmp10B8;(_tmp10B8.tag=0,((_tmp10B8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10B9[0]=& _tmp10B8,Cyc_fprintf(
Cyc_stderr,((_tmp10BA="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp10BA,sizeof(char),36))),_tag_dyneither(_tmp10B9,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp10BD;
struct Cyc_Core_Impossible_struct*_tmp10BC;(int)_throw((void*)((_tmp10BC=
_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BD.tag=Cyc_Core_Impossible,((
_tmp10BD.f1=msg,_tmp10BD)))),_tmp10BC)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp10C2;void*_tmp10C1[2];struct Cyc_String_pa_struct
_tmp10C0;struct Cyc_String_pa_struct _tmp10BF;(_tmp10BF.tag=0,((_tmp10BF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10C0.tag=0,((_tmp10C0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp10C1[0]=& _tmp10C0,((_tmp10C1[1]=& _tmp10BF,Cyc_fprintf(Cyc_stderr,((
_tmp10C2="%s::%s ",_tag_dyneither(_tmp10C2,sizeof(char),8))),_tag_dyneither(
_tmp10C1,sizeof(void*),2)))))))))))));}{const char*_tmp10C5;void*_tmp10C4;(
_tmp10C4=0,Cyc_fprintf(Cyc_stderr,((_tmp10C5="\n",_tag_dyneither(_tmp10C5,
sizeof(char),2))),_tag_dyneither(_tmp10C4,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp10C6;Cyc_Tcutil_warning_segs=((_tmp10C6=_cycalloc(
sizeof(*_tmp10C6)),((_tmp10C6->hd=sg,((_tmp10C6->tl=Cyc_Tcutil_warning_segs,
_tmp10C6))))));}{struct _dyneither_ptr*_tmp10C9;struct Cyc_List_List*_tmp10C8;Cyc_Tcutil_warning_msgs=((
_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8->hd=((_tmp10C9=_cycalloc(
sizeof(*_tmp10C9)),((_tmp10C9[0]=msg,_tmp10C9)))),((_tmp10C8->tl=Cyc_Tcutil_warning_msgs,
_tmp10C8))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp10CC;void*_tmp10CB;(_tmp10CB=0,Cyc_fprintf(Cyc_stderr,((
_tmp10CC="***Warnings***\n",_tag_dyneither(_tmp10CC,sizeof(char),16))),
_tag_dyneither(_tmp10CB,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp10D1;void*_tmp10D0[2];struct
Cyc_String_pa_struct _tmp10CF;struct Cyc_String_pa_struct _tmp10CE;(_tmp10CE.tag=0,((
_tmp10CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10CF.tag=
0,((_tmp10CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10D0[0]=&
_tmp10CF,((_tmp10D0[1]=& _tmp10CE,Cyc_fprintf(Cyc_stderr,((_tmp10D1="%s: %s\n",
_tag_dyneither(_tmp10D1,sizeof(char),8))),_tag_dyneither(_tmp10D0,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10D4;void*_tmp10D3;(
_tmp10D3=0,Cyc_fprintf(Cyc_stderr,((_tmp10D4="**************\n",_tag_dyneither(
_tmp10D4,sizeof(char),16))),_tag_dyneither(_tmp10D3,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t){void*
_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;void**_tmp43;void***_tmp44;
struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;struct Cyc_Absyn_Exp*_tmp49;
void**_tmp4B;void*_tmp4C;_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)
_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}
_LL2: goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 18)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 18)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10D5;*_tmp44=((_tmp10D5=
_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=t2,_tmp10D5))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10D6;*_tmp47=((_tmp10D6=_cycalloc(sizeof(*
_tmp10D6)),((_tmp10D6->v=t2,_tmp10D6))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 19)goto _LLB;
else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{void*_tmp4F=
_tmp49->r;void*_tmp51;_LL10: {struct Cyc_Absyn_Valueof_e_struct*_tmp50=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp4F;if(_tmp50->tag != 39)goto _LL12;else{_tmp51=(void*)_tmp50->f1;}}_LL11:
return Cyc_Tcutil_compress(_tmp51);_LL12:;_LL13: return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp4A=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp3D;if(_tmp4A->tag != 27)goto _LLD;
else{_tmp4B=_tmp4A->f2;if(_tmp4B == 0)goto _LLD;_tmp4C=*_tmp4B;}}_LLC: return Cyc_Tcutil_compress(
_tmp4C);_LLD:;_LLE: return t;_LL0:;}void*Cyc_Tcutil_copy_type(void*t);static struct
Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*
cptr){union Cyc_Absyn_Constraint*_tmp52=cptr;union Cyc_Absyn_Constraint _tmp53;int
_tmp54;union Cyc_Absyn_Constraint _tmp55;void*_tmp56;union Cyc_Absyn_Constraint
_tmp57;union Cyc_Absyn_Constraint*_tmp58;_LL15: _tmp53=*_tmp52;if((_tmp53.No_constr).tag
!= 3)goto _LL17;_tmp54=(int)(_tmp53.No_constr).val;_LL16: return Cyc_Absyn_empty_conref();
_LL17: _tmp55=*_tmp52;if((_tmp55.Eq_constr).tag != 1)goto _LL19;_tmp56=(void*)(
_tmp55.Eq_constr).val;_LL18: return Cyc_Absyn_new_conref(_tmp56);_LL19: _tmp57=*
_tmp52;if((_tmp57.Forward_constr).tag != 2)goto _LL14;_tmp58=(union Cyc_Absyn_Constraint*)(
_tmp57.Forward_constr).val;_LL1A: return Cyc_Tcutil_copy_conref(_tmp58);_LL14:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb){void*_tmp59=Cyc_Absyn_compress_kb(
kb);struct Cyc_Absyn_Kind*_tmp5C;_LL1C: {struct Cyc_Absyn_Unknown_kb_struct*_tmp5A=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmp59;if(_tmp5A->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_struct _tmp10D9;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10D8;return(void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((
_tmp10D9.tag=1,((_tmp10D9.f1=0,_tmp10D9)))),_tmp10D8))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp10DC;struct
Cyc_Absyn_Less_kb_struct*_tmp10DB;return(void*)((_tmp10DB=_cycalloc(sizeof(*
_tmp10DB)),((_tmp10DB[0]=((_tmp10DC.tag=2,((_tmp10DC.f1=0,((_tmp10DC.f2=_tmp5C,
_tmp10DC)))))),_tmp10DB))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10DD;
return(_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD->name=tv->name,((
_tmp10DD->identity=- 1,((_tmp10DD->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10DD)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple9*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple9*_tmp10DE;return(_tmp10DE=_cycalloc(sizeof(*
_tmp10DE)),((_tmp10DE->f1=_tmp64,((_tmp10DE->f2=_tmp65,((_tmp10DE->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10DE)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg){struct _tuple11 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple11*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple11*_tmp10DF;return(_tmp10DF=_cycalloc(sizeof(*_tmp10DF)),((_tmp10DF->f1=
_tmp6A,((_tmp10DF->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp10DF)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10E0;return(_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0->name=f->name,((
_tmp10E0->tq=f->tq,((_tmp10E0->type=Cyc_Tcutil_copy_type(f->type),((_tmp10E0->width=
f->width,((_tmp10E0->attributes=f->attributes,_tmp10E0)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10E1;return(_tmp10E1=_cycalloc(sizeof(*_tmp10E1)),((_tmp10E1->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp10E1->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp10E1)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10E2;return(_tmp10E2=
_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2->name=f->name,((_tmp10E2->tag=f->tag,((
_tmp10E2->loc=f->loc,_tmp10E2)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp10E5;struct Cyc_Absyn_VarType_struct*
_tmp10E4;return(void*)((_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4[0]=((
_tmp10E5.tag=2,((_tmp10E5.f1=Cyc_Tcutil_copy_tvar(t),_tmp10E5)))),_tmp10E4))));}
void*Cyc_Tcutil_copy_type(void*t){void*_tmp76=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp7A;struct Cyc_Absyn_DatatypeInfo _tmp7C;union Cyc_Absyn_DatatypeInfoU _tmp7D;
struct Cyc_List_List*_tmp7E;struct Cyc_Absyn_DatatypeFieldInfo _tmp80;union Cyc_Absyn_DatatypeFieldInfoU
_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_PtrInfo _tmp84;void*_tmp85;
struct Cyc_Absyn_Tqual _tmp86;struct Cyc_Absyn_PtrAtts _tmp87;void*_tmp88;union Cyc_Absyn_Constraint*
_tmp89;union Cyc_Absyn_Constraint*_tmp8A;union Cyc_Absyn_Constraint*_tmp8B;struct
Cyc_Absyn_PtrLoc*_tmp8C;struct Cyc_Absyn_ArrayInfo _tmp91;void*_tmp92;struct Cyc_Absyn_Tqual
_tmp93;struct Cyc_Absyn_Exp*_tmp94;union Cyc_Absyn_Constraint*_tmp95;struct Cyc_Position_Segment*
_tmp96;struct Cyc_Absyn_FnInfo _tmp98;struct Cyc_List_List*_tmp99;struct Cyc_Core_Opt*
_tmp9A;struct Cyc_Absyn_Tqual _tmp9B;void*_tmp9C;struct Cyc_List_List*_tmp9D;int
_tmp9E;struct Cyc_Absyn_VarargInfo*_tmp9F;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*
_tmpA1;struct Cyc_List_List*_tmpA3;struct Cyc_Absyn_AggrInfo _tmpA5;union Cyc_Absyn_AggrInfoU
_tmpA6;struct _tuple4 _tmpA7;enum Cyc_Absyn_AggrKind _tmpA8;struct _tuple2*_tmpA9;
struct Cyc_Core_Opt*_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_Absyn_AggrInfo
_tmpAD;union Cyc_Absyn_AggrInfoU _tmpAE;struct Cyc_Absyn_Aggrdecl**_tmpAF;struct Cyc_List_List*
_tmpB0;enum Cyc_Absyn_AggrKind _tmpB2;struct Cyc_List_List*_tmpB3;struct _tuple2*
_tmpB5;struct Cyc_Absyn_Enumdecl*_tmpB6;struct Cyc_List_List*_tmpB8;void*_tmpBA;
struct Cyc_Absyn_Exp*_tmpBC;void*_tmpBE;void*_tmpC0;void*_tmpC1;struct _tuple2*
_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Typedefdecl*_tmpC5;void*_tmpCA;
struct Cyc_List_List*_tmpCC;void*_tmpCE;struct Cyc_Absyn_TypeDecl*_tmpD0;struct Cyc_Absyn_TypeDecl
_tmpD1;void*_tmpD2;struct Cyc_Absyn_Aggrdecl*_tmpD4;struct Cyc_Absyn_TypeDecl*
_tmpD6;struct Cyc_Absyn_TypeDecl _tmpD7;void*_tmpD8;struct Cyc_Absyn_Enumdecl*
_tmpDA;struct Cyc_Absyn_TypeDecl*_tmpDC;struct Cyc_Absyn_TypeDecl _tmpDD;void*
_tmpDE;struct Cyc_Absyn_Datatypedecl*_tmpE0;_LL23: {struct Cyc_Absyn_VoidType_struct*
_tmp77=(struct Cyc_Absyn_VoidType_struct*)_tmp76;if(_tmp77->tag != 0)goto _LL25;}
_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Evar_struct*_tmp78=(struct Cyc_Absyn_Evar_struct*)
_tmp76;if(_tmp78->tag != 1)goto _LL27;}_LL26: return t;_LL27: {struct Cyc_Absyn_VarType_struct*
_tmp79=(struct Cyc_Absyn_VarType_struct*)_tmp76;if(_tmp79->tag != 2)goto _LL29;
else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct _tmp10E8;struct
Cyc_Absyn_VarType_struct*_tmp10E7;return(void*)((_tmp10E7=_cycalloc(sizeof(*
_tmp10E7)),((_tmp10E7[0]=((_tmp10E8.tag=2,((_tmp10E8.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10E8)))),_tmp10E7))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10EE;struct Cyc_Absyn_DatatypeInfo _tmp10ED;
struct Cyc_Absyn_DatatypeType_struct*_tmp10EC;return(void*)((_tmp10EC=_cycalloc(
sizeof(*_tmp10EC)),((_tmp10EC[0]=((_tmp10EE.tag=3,((_tmp10EE.f1=((_tmp10ED.datatype_info=
_tmp7D,((_tmp10ED.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp10ED)))),_tmp10EE)))),
_tmp10EC))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10F4;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10F3;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10F2;return(void*)((_tmp10F2=_cycalloc(sizeof(*_tmp10F2)),((_tmp10F2[0]=((
_tmp10F4.tag=4,((_tmp10F4.f1=((_tmp10F3.field_info=_tmp81,((_tmp10F3.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp10F3)))),_tmp10F4)))),_tmp10F2))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE9=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpEA=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEC=_tmp86;union Cyc_Absyn_Constraint*_tmpED=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpEE=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp10FE;struct Cyc_Absyn_PtrAtts
_tmp10FD;struct Cyc_Absyn_PtrInfo _tmp10FC;struct Cyc_Absyn_PointerType_struct*
_tmp10FB;return(void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((
_tmp10FE.tag=5,((_tmp10FE.f1=((_tmp10FC.elt_typ=_tmpE9,((_tmp10FC.elt_tq=_tmpEC,((
_tmp10FC.ptr_atts=((_tmp10FD.rgn=_tmpEA,((_tmp10FD.nullable=_tmpEB,((_tmp10FD.bounds=
_tmpED,((_tmp10FD.zero_term=_tmpEE,((_tmp10FD.ptrloc=_tmp8C,_tmp10FD)))))))))),
_tmp10FC)))))),_tmp10FE)))),_tmp10FB))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_DoubleType_struct*
_tmp8F=(struct Cyc_Absyn_DoubleType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;}
_LL34: return t;_LL35: {struct Cyc_Absyn_ArrayType_struct*_tmp90=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76;if(_tmp90->tag != 9)goto _LL37;else{_tmp91=_tmp90->f1;_tmp92=_tmp91.elt_type;
_tmp93=_tmp91.tq;_tmp94=_tmp91.num_elts;_tmp95=_tmp91.zero_term;_tmp96=_tmp91.zt_loc;}}
_LL36: {struct Cyc_Absyn_ArrayType_struct _tmp1104;struct Cyc_Absyn_ArrayInfo
_tmp1103;struct Cyc_Absyn_ArrayType_struct*_tmp1102;return(void*)((_tmp1102=
_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1104.tag=9,((_tmp1104.f1=((
_tmp1103.elt_type=Cyc_Tcutil_copy_type(_tmp92),((_tmp1103.tq=_tmp93,((_tmp1103.num_elts=
_tmp94,((_tmp1103.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp95),((_tmp1103.zt_loc=_tmp96,_tmp1103)))))))))),
_tmp1104)))),_tmp1102))));}_LL37: {struct Cyc_Absyn_FnType_struct*_tmp97=(struct
Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp97->tag != 10)goto _LL39;else{_tmp98=_tmp97->f1;
_tmp99=_tmp98.tvars;_tmp9A=_tmp98.effect;_tmp9B=_tmp98.ret_tqual;_tmp9C=_tmp98.ret_typ;
_tmp9D=_tmp98.args;_tmp9E=_tmp98.c_varargs;_tmp9F=_tmp98.cyc_varargs;_tmpA0=
_tmp98.rgn_po;_tmpA1=_tmp98.attributes;}}_LL38: {struct Cyc_List_List*_tmpF6=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp99);struct Cyc_Core_Opt*
_tmp1105;struct Cyc_Core_Opt*_tmpF7=_tmp9A == 0?0:((_tmp1105=_cycalloc(sizeof(*
_tmp1105)),((_tmp1105->v=Cyc_Tcutil_copy_type((void*)_tmp9A->v),_tmp1105))));
void*_tmpF8=Cyc_Tcutil_copy_type(_tmp9C);struct Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9D);int _tmpFA=_tmp9E;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9F != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9F;struct Cyc_Absyn_VarargInfo*
_tmp1106;cyc_varargs2=((_tmp1106=_cycalloc(sizeof(*_tmp1106)),((_tmp1106->name=
cv->name,((_tmp1106->tq=cv->tq,((_tmp1106->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp1106->inject=cv->inject,_tmp1106))))))))));}{struct Cyc_List_List*_tmpFC=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpA0);struct Cyc_List_List*_tmpFD=_tmpA1;
struct Cyc_Absyn_FnType_struct _tmp110C;struct Cyc_Absyn_FnInfo _tmp110B;struct Cyc_Absyn_FnType_struct*
_tmp110A;return(void*)((_tmp110A=_cycalloc(sizeof(*_tmp110A)),((_tmp110A[0]=((
_tmp110C.tag=10,((_tmp110C.f1=((_tmp110B.tvars=_tmpF6,((_tmp110B.effect=_tmpF7,((
_tmp110B.ret_tqual=_tmp9B,((_tmp110B.ret_typ=_tmpF8,((_tmp110B.args=_tmpF9,((
_tmp110B.c_varargs=_tmpFA,((_tmp110B.cyc_varargs=cyc_varargs2,((_tmp110B.rgn_po=
_tmpFC,((_tmp110B.attributes=_tmpFD,_tmp110B)))))))))))))))))),_tmp110C)))),
_tmp110A))));};}_LL39: {struct Cyc_Absyn_TupleType_struct*_tmpA2=(struct Cyc_Absyn_TupleType_struct*)
_tmp76;if(_tmpA2->tag != 11)goto _LL3B;else{_tmpA3=_tmpA2->f1;}}_LL3A: {struct Cyc_Absyn_TupleType_struct
_tmp110F;struct Cyc_Absyn_TupleType_struct*_tmp110E;return(void*)((_tmp110E=
_cycalloc(sizeof(*_tmp110E)),((_tmp110E[0]=((_tmp110F.tag=11,((_tmp110F.f1=((
struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA3),_tmp110F)))),_tmp110E))));}_LL3B: {
struct Cyc_Absyn_AggrType_struct*_tmpA4=(struct Cyc_Absyn_AggrType_struct*)_tmp76;
if(_tmpA4->tag != 12)goto _LL3D;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.aggr_info;if((
_tmpA6.UnknownAggr).tag != 1)goto _LL3D;_tmpA7=(struct _tuple4)(_tmpA6.UnknownAggr).val;
_tmpA8=_tmpA7.f1;_tmpA9=_tmpA7.f2;_tmpAA=_tmpA7.f3;_tmpAB=_tmpA5.targs;}}_LL3C: {
struct Cyc_Absyn_AggrType_struct _tmp1115;struct Cyc_Absyn_AggrInfo _tmp1114;struct
Cyc_Absyn_AggrType_struct*_tmp1113;return(void*)((_tmp1113=_cycalloc(sizeof(*
_tmp1113)),((_tmp1113[0]=((_tmp1115.tag=12,((_tmp1115.f1=((_tmp1114.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA8,_tmpA9,_tmpAA),((_tmp1114.targs=Cyc_Tcutil_copy_types(
_tmpAB),_tmp1114)))),_tmp1115)))),_tmp1113))));}_LL3D: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAC->tag != 12)goto _LL3F;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;if((_tmpAE.KnownAggr).tag != 2)
goto _LL3F;_tmpAF=(struct Cyc_Absyn_Aggrdecl**)(_tmpAE.KnownAggr).val;_tmpB0=
_tmpAD.targs;}}_LL3E: {struct Cyc_Absyn_AggrType_struct _tmp111B;struct Cyc_Absyn_AggrInfo
_tmp111A;struct Cyc_Absyn_AggrType_struct*_tmp1119;return(void*)((_tmp1119=
_cycalloc(sizeof(*_tmp1119)),((_tmp1119[0]=((_tmp111B.tag=12,((_tmp111B.f1=((
_tmp111A.aggr_info=Cyc_Absyn_KnownAggr(_tmpAF),((_tmp111A.targs=Cyc_Tcutil_copy_types(
_tmpB0),_tmp111A)))),_tmp111B)))),_tmp1119))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB1->tag != 13)goto
_LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL40: {struct Cyc_Absyn_AnonAggrType_struct
_tmp111E;struct Cyc_Absyn_AnonAggrType_struct*_tmp111D;return(void*)((_tmp111D=
_cycalloc(sizeof(*_tmp111D)),((_tmp111D[0]=((_tmp111E.tag=13,((_tmp111E.f1=
_tmpB2,((_tmp111E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB3),_tmp111E)))))),_tmp111D))));}_LL41: {struct Cyc_Absyn_EnumType_struct*
_tmpB4=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB4->tag != 14)goto _LL43;
else{_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;}}_LL42: {struct Cyc_Absyn_EnumType_struct
_tmp1121;struct Cyc_Absyn_EnumType_struct*_tmp1120;return(void*)((_tmp1120=
_cycalloc(sizeof(*_tmp1120)),((_tmp1120[0]=((_tmp1121.tag=14,((_tmp1121.f1=
_tmpB5,((_tmp1121.f2=_tmpB6,_tmp1121)))))),_tmp1120))));}_LL43: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB7->tag != 15)goto
_LL45;else{_tmpB8=_tmpB7->f1;}}_LL44: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1124;struct Cyc_Absyn_AnonEnumType_struct*_tmp1123;return(void*)((_tmp1123=
_cycalloc(sizeof(*_tmp1123)),((_tmp1123[0]=((_tmp1124.tag=15,((_tmp1124.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB8),_tmp1124)))),
_tmp1123))));}_LL45: {struct Cyc_Absyn_TagType_struct*_tmpB9=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB9->tag != 20)goto _LL47;else{_tmpBA=(void*)_tmpB9->f1;}}_LL46: {
struct Cyc_Absyn_TagType_struct _tmp1127;struct Cyc_Absyn_TagType_struct*_tmp1126;
return(void*)((_tmp1126=_cycalloc(sizeof(*_tmp1126)),((_tmp1126[0]=((_tmp1127.tag=
20,((_tmp1127.f1=(void*)Cyc_Tcutil_copy_type(_tmpBA),_tmp1127)))),_tmp1126))));}
_LL47: {struct Cyc_Absyn_ValueofType_struct*_tmpBB=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBB->tag != 19)goto _LL49;else{_tmpBC=_tmpBB->f1;}}_LL48: {struct Cyc_Absyn_ValueofType_struct
_tmp112A;struct Cyc_Absyn_ValueofType_struct*_tmp1129;return(void*)((_tmp1129=
_cycalloc(sizeof(*_tmp1129)),((_tmp1129[0]=((_tmp112A.tag=19,((_tmp112A.f1=
_tmpBC,_tmp112A)))),_tmp1129))));}_LL49: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBD=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBD->tag != 16)goto
_LL4B;else{_tmpBE=(void*)_tmpBD->f1;}}_LL4A: {struct Cyc_Absyn_RgnHandleType_struct
_tmp112D;struct Cyc_Absyn_RgnHandleType_struct*_tmp112C;return(void*)((_tmp112C=
_cycalloc(sizeof(*_tmp112C)),((_tmp112C[0]=((_tmp112D.tag=16,((_tmp112D.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBE),_tmp112D)))),_tmp112C))));}_LL4B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBF=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBF->tag != 17)goto _LL4D;
else{_tmpC0=(void*)_tmpBF->f1;_tmpC1=(void*)_tmpBF->f2;}}_LL4C: {struct Cyc_Absyn_DynRgnType_struct
_tmp1130;struct Cyc_Absyn_DynRgnType_struct*_tmp112F;return(void*)((_tmp112F=
_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1130.tag=17,((_tmp1130.f1=(void*)
Cyc_Tcutil_copy_type(_tmpC0),((_tmp1130.f2=(void*)Cyc_Tcutil_copy_type(_tmpC1),
_tmp1130)))))),_tmp112F))));}_LL4D: {struct Cyc_Absyn_TypedefType_struct*_tmpC2=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC2->tag != 18)goto _LL4F;else{
_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_tmpC5=_tmpC2->f3;}}_LL4E: {struct Cyc_Absyn_TypedefType_struct
_tmp1133;struct Cyc_Absyn_TypedefType_struct*_tmp1132;return(void*)((_tmp1132=
_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((_tmp1133.tag=18,((_tmp1133.f1=
_tmpC3,((_tmp1133.f2=Cyc_Tcutil_copy_types(_tmpC4),((_tmp1133.f3=_tmpC5,((
_tmp1133.f4=0,_tmp1133)))))))))),_tmp1132))));}_LL4F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC6->tag != 22)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_RefCntRgn_struct*_tmpC7=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp76;if(_tmpC7->tag != 23)goto _LL53;}_LL52: goto _LL54;_LL53: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC8=(struct Cyc_Absyn_HeapRgn_struct*)_tmp76;if(_tmpC8->tag != 21)goto _LL55;}
_LL54: return t;_LL55: {struct Cyc_Absyn_AccessEff_struct*_tmpC9=(struct Cyc_Absyn_AccessEff_struct*)
_tmp76;if(_tmpC9->tag != 24)goto _LL57;else{_tmpCA=(void*)_tmpC9->f1;}}_LL56: {
struct Cyc_Absyn_AccessEff_struct _tmp1136;struct Cyc_Absyn_AccessEff_struct*
_tmp1135;return(void*)((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135[0]=((
_tmp1136.tag=24,((_tmp1136.f1=(void*)Cyc_Tcutil_copy_type(_tmpCA),_tmp1136)))),
_tmp1135))));}_LL57: {struct Cyc_Absyn_JoinEff_struct*_tmpCB=(struct Cyc_Absyn_JoinEff_struct*)
_tmp76;if(_tmpCB->tag != 25)goto _LL59;else{_tmpCC=_tmpCB->f1;}}_LL58: {struct Cyc_Absyn_JoinEff_struct
_tmp1139;struct Cyc_Absyn_JoinEff_struct*_tmp1138;return(void*)((_tmp1138=
_cycalloc(sizeof(*_tmp1138)),((_tmp1138[0]=((_tmp1139.tag=25,((_tmp1139.f1=Cyc_Tcutil_copy_types(
_tmpCC),_tmp1139)))),_tmp1138))));}_LL59: {struct Cyc_Absyn_RgnsEff_struct*_tmpCD=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp76;if(_tmpCD->tag != 26)goto _LL5B;else{_tmpCE=(
void*)_tmpCD->f1;}}_LL5A: {struct Cyc_Absyn_RgnsEff_struct _tmp113C;struct Cyc_Absyn_RgnsEff_struct*
_tmp113B;return(void*)((_tmp113B=_cycalloc(sizeof(*_tmp113B)),((_tmp113B[0]=((
_tmp113C.tag=26,((_tmp113C.f1=(void*)Cyc_Tcutil_copy_type(_tmpCE),_tmp113C)))),
_tmp113B))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCF=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCF->tag != 27)goto _LL5D;else{_tmpD0=_tmpCF->f1;_tmpD1=*_tmpD0;
_tmpD2=_tmpD1.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD3=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL5D;else{_tmpD4=_tmpD3->f1;}};}}_LL5C: {struct
Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD4->tvs);struct Cyc_Absyn_AggrType_struct
_tmp1142;struct Cyc_Absyn_AggrInfo _tmp1141;struct Cyc_Absyn_AggrType_struct*
_tmp1140;return(void*)((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140[0]=((
_tmp1142.tag=12,((_tmp1142.f1=((_tmp1141.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD4->kind,
_tmpD4->name,0),((_tmp1141.targs=_tmp120,_tmp1141)))),_tmp1142)))),_tmp1140))));}
_LL5D: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD5=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD5->tag != 27)goto _LL5F;else{_tmpD6=_tmpD5->f1;_tmpD7=*_tmpD6;
_tmpD8=_tmpD7.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD9=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD8;if(_tmpD9->tag != 1)goto _LL5F;else{_tmpDA=_tmpD9->f1;}};}}_LL5E: {struct
Cyc_Absyn_EnumType_struct _tmp1145;struct Cyc_Absyn_EnumType_struct*_tmp1144;
return(void*)((_tmp1144=_cycalloc(sizeof(*_tmp1144)),((_tmp1144[0]=((_tmp1145.tag=
14,((_tmp1145.f1=_tmpDA->name,((_tmp1145.f2=0,_tmp1145)))))),_tmp1144))));}_LL5F: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpDB=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpDB->tag != 27)goto _LL22;else{_tmpDC=_tmpDB->f1;_tmpDD=*_tmpDC;
_tmpDE=_tmpDD.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDF=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDE;if(_tmpDF->tag != 2)goto _LL22;else{_tmpE0=_tmpDF->f1;}};}}_LL60: {struct
Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpE0->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp114F;struct Cyc_Absyn_UnknownDatatypeInfo _tmp114E;struct Cyc_Absyn_DatatypeInfo
_tmp114D;struct Cyc_Absyn_DatatypeType_struct*_tmp114C;return(void*)((_tmp114C=
_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=((_tmp114F.tag=3,((_tmp114F.f1=((
_tmp114D.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp114E.name=_tmpE0->name,((
_tmp114E.is_extensible=0,_tmp114E))))),((_tmp114D.targs=_tmp126,_tmp114D)))),
_tmp114F)))),_tmp114C))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp12B=d;struct Cyc_Absyn_Exp*_tmp12D;struct _dyneither_ptr*_tmp12F;_LL62: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12C=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp12B;if(_tmp12C->tag != 0)goto _LL64;else{_tmp12D=_tmp12C->f1;}}_LL63: {struct
Cyc_Absyn_ArrayElement_struct _tmp1152;struct Cyc_Absyn_ArrayElement_struct*
_tmp1151;return(void*)((_tmp1151=_cycalloc(sizeof(*_tmp1151)),((_tmp1151[0]=((
_tmp1152.tag=0,((_tmp1152.f1=Cyc_Tcutil_deep_copy_exp(_tmp12D),_tmp1152)))),
_tmp1151))));}_LL64: {struct Cyc_Absyn_FieldName_struct*_tmp12E=(struct Cyc_Absyn_FieldName_struct*)
_tmp12B;if(_tmp12E->tag != 1)goto _LL61;else{_tmp12F=_tmp12E->f1;}}_LL65: return d;
_LL61:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e){struct _tuple18*_tmp1153;
return(_tmp1153=_cycalloc(sizeof(*_tmp1153)),((_tmp1153->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp1153->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1153)))));}
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){void*_tmp133=
e->r;union Cyc_Absyn_Cnst _tmp135;struct _tuple2*_tmp137;void*_tmp138;struct _tuple2*
_tmp13A;enum Cyc_Absyn_Primop _tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Exp*
_tmp13F;struct Cyc_Core_Opt*_tmp140;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*
_tmp143;enum Cyc_Absyn_Incrementor _tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*
_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Exp*
_tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*
_tmp150;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_List_List*
_tmp154;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*_tmp157;struct Cyc_Absyn_VarargCallInfo*
_tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*
_tmp15E;struct Cyc_List_List*_tmp15F;void*_tmp161;struct Cyc_Absyn_Exp*_tmp162;int
_tmp163;enum Cyc_Absyn_Coercion _tmp164;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*
_tmp168;struct Cyc_Absyn_Exp*_tmp169;void*_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;
void*_tmp16F;void*_tmp170;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*
_tmp174;struct _dyneither_ptr*_tmp175;int _tmp176;int _tmp177;struct Cyc_Absyn_Exp*
_tmp179;struct _dyneither_ptr*_tmp17A;int _tmp17B;int _tmp17C;struct Cyc_Absyn_Exp*
_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_List_List*_tmp181;struct _tuple9*
_tmp183;struct _tuple9 _tmp184;struct Cyc_Core_Opt*_tmp185;struct Cyc_Absyn_Tqual
_tmp186;void*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp18A;
struct Cyc_Absyn_Vardecl*_tmp18C;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*
_tmp18E;int _tmp18F;struct _tuple2*_tmp191;struct Cyc_List_List*_tmp192;struct Cyc_List_List*
_tmp193;struct Cyc_Absyn_Aggrdecl*_tmp194;void*_tmp196;struct Cyc_List_List*
_tmp197;struct Cyc_List_List*_tmp199;struct Cyc_Absyn_Datatypedecl*_tmp19A;struct
Cyc_Absyn_Datatypefield*_tmp19B;struct _tuple2*_tmp19D;struct Cyc_Absyn_Enumdecl*
_tmp19E;struct Cyc_Absyn_Enumfield*_tmp19F;struct _tuple2*_tmp1A1;void*_tmp1A2;
struct Cyc_Absyn_Enumfield*_tmp1A3;struct Cyc_Absyn_MallocInfo _tmp1A5;int _tmp1A6;
struct Cyc_Absyn_Exp*_tmp1A7;void**_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;int _tmp1AA;
struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Core_Opt*
_tmp1AF;struct Cyc_List_List*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B3;struct
_dyneither_ptr*_tmp1B4;void*_tmp1B6;int _tmp1B8;struct _dyneither_ptr _tmp1B9;_LL67: {
struct Cyc_Absyn_Const_e_struct*_tmp134=(struct Cyc_Absyn_Const_e_struct*)_tmp133;
if(_tmp134->tag != 0)goto _LL69;else{_tmp135=_tmp134->f1;}}_LL68: return Cyc_Absyn_const_exp(
_tmp135,e->loc);_LL69: {struct Cyc_Absyn_Var_e_struct*_tmp136=(struct Cyc_Absyn_Var_e_struct*)
_tmp133;if(_tmp136->tag != 1)goto _LL6B;else{_tmp137=_tmp136->f1;_tmp138=(void*)
_tmp136->f2;}}_LL6A: return Cyc_Absyn_varb_exp(_tmp137,_tmp138,e->loc);_LL6B: {
struct Cyc_Absyn_UnknownId_e_struct*_tmp139=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp133;if(_tmp139->tag != 2)goto _LL6D;else{_tmp13A=_tmp139->f1;}}_LL6C: return Cyc_Absyn_unknownid_exp(
_tmp13A,e->loc);_LL6D: {struct Cyc_Absyn_Primop_e_struct*_tmp13B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp133;if(_tmp13B->tag != 3)goto _LL6F;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}
_LL6E: return Cyc_Absyn_primop_exp(_tmp13C,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp13D),e->loc);_LL6F: {struct Cyc_Absyn_AssignOp_e_struct*_tmp13E=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp133;if(_tmp13E->tag != 4)goto _LL71;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;
_tmp141=_tmp13E->f3;}}_LL70: {struct Cyc_Core_Opt*_tmp1154;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13F),(unsigned int)_tmp140?(_tmp1154=
_cycalloc_atomic(sizeof(*_tmp1154)),((_tmp1154->v=(void*)((enum Cyc_Absyn_Primop)
_tmp140->v),_tmp1154))): 0,Cyc_Tcutil_deep_copy_exp(_tmp141),e->loc);}_LL71: {
struct Cyc_Absyn_Increment_e_struct*_tmp142=(struct Cyc_Absyn_Increment_e_struct*)
_tmp133;if(_tmp142->tag != 5)goto _LL73;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL72: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp143),_tmp144,e->loc);
_LL73: {struct Cyc_Absyn_Conditional_e_struct*_tmp145=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp133;if(_tmp145->tag != 6)goto _LL75;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;
_tmp148=_tmp145->f3;}}_LL74: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp146),Cyc_Tcutil_deep_copy_exp(_tmp147),Cyc_Tcutil_deep_copy_exp(_tmp148),e->loc);
_LL75: {struct Cyc_Absyn_And_e_struct*_tmp149=(struct Cyc_Absyn_And_e_struct*)
_tmp133;if(_tmp149->tag != 7)goto _LL77;else{_tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;}}
_LL76: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp14A),Cyc_Tcutil_deep_copy_exp(
_tmp14B),e->loc);_LL77: {struct Cyc_Absyn_Or_e_struct*_tmp14C=(struct Cyc_Absyn_Or_e_struct*)
_tmp133;if(_tmp14C->tag != 8)goto _LL79;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}}
_LL78: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp14D),Cyc_Tcutil_deep_copy_exp(
_tmp14E),e->loc);_LL79: {struct Cyc_Absyn_SeqExp_e_struct*_tmp14F=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp133;if(_tmp14F->tag != 9)goto _LL7B;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;}}
_LL7A: return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp150),Cyc_Tcutil_deep_copy_exp(
_tmp151),e->loc);_LL7B: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp152=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp133;if(_tmp152->tag != 10)goto _LL7D;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;}}
_LL7C: return Cyc_Absyn_unknowncall_exp(Cyc_Tcutil_deep_copy_exp(_tmp153),((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp154),e->loc);_LL7D: {struct Cyc_Absyn_FnCall_e_struct*
_tmp155=(struct Cyc_Absyn_FnCall_e_struct*)_tmp133;if(_tmp155->tag != 11)goto _LL7F;
else{_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;_tmp158=_tmp155->f3;}}_LL7E: {
struct Cyc_Absyn_VarargCallInfo*_tmp1BB=_tmp158;struct Cyc_Absyn_VarargCallInfo
_tmp1BC;int _tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_VarargInfo*
_tmp1BF;_LLBA: if(_tmp1BB == 0)goto _LLBC;_tmp1BC=*_tmp1BB;_tmp1BD=_tmp1BC.num_varargs;
_tmp1BE=_tmp1BC.injectors;_tmp1BF=_tmp1BC.vai;_LLBB: {struct Cyc_Absyn_VarargInfo
_tmp1C1;struct Cyc_Core_Opt*_tmp1C2;struct Cyc_Absyn_Tqual _tmp1C3;void*_tmp1C4;int
_tmp1C5;struct Cyc_Absyn_VarargInfo*_tmp1C0=_tmp1BF;_tmp1C1=*_tmp1C0;_tmp1C2=
_tmp1C1.name;_tmp1C3=_tmp1C1.tq;_tmp1C4=_tmp1C1.type;_tmp1C5=_tmp1C1.inject;{
struct Cyc_Absyn_FnCall_e_struct _tmp115E;struct Cyc_Absyn_VarargInfo*_tmp115D;
struct Cyc_Absyn_VarargCallInfo*_tmp115C;struct Cyc_Absyn_FnCall_e_struct*_tmp115B;
return Cyc_Absyn_new_exp((void*)((_tmp115B=_cycalloc(sizeof(*_tmp115B)),((
_tmp115B[0]=((_tmp115E.tag=11,((_tmp115E.f1=Cyc_Tcutil_deep_copy_exp(_tmp156),((
_tmp115E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),((
_tmp115E.f3=((_tmp115C=_cycalloc(sizeof(*_tmp115C)),((_tmp115C->num_varargs=
_tmp1BD,((_tmp115C->injectors=_tmp1BE,((_tmp115C->vai=((_tmp115D=_cycalloc(
sizeof(*_tmp115D)),((_tmp115D->name=_tmp1C2,((_tmp115D->tq=_tmp1C3,((_tmp115D->type=
Cyc_Tcutil_copy_type(_tmp1C4),((_tmp115D->inject=_tmp1C5,_tmp115D)))))))))),
_tmp115C)))))))),_tmp115E)))))))),_tmp115B)))),e->loc);};}_LLBC:;_LLBD: {struct
Cyc_Absyn_FnCall_e_struct _tmp1161;struct Cyc_Absyn_FnCall_e_struct*_tmp1160;
return Cyc_Absyn_new_exp((void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((
_tmp1160[0]=((_tmp1161.tag=11,((_tmp1161.f1=Cyc_Tcutil_deep_copy_exp(_tmp156),((
_tmp1161.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),((
_tmp1161.f3=_tmp158,_tmp1161)))))))),_tmp1160)))),e->loc);}_LLB9:;}_LL7F: {
struct Cyc_Absyn_Throw_e_struct*_tmp159=(struct Cyc_Absyn_Throw_e_struct*)_tmp133;
if(_tmp159->tag != 12)goto _LL81;else{_tmp15A=_tmp159->f1;}}_LL80: return Cyc_Absyn_throw_exp(
Cyc_Tcutil_deep_copy_exp(_tmp15A),e->loc);_LL81: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp15B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp133;if(_tmp15B->tag != 13)
goto _LL83;else{_tmp15C=_tmp15B->f1;}}_LL82: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
_tmp15C),e->loc);_LL83: {struct Cyc_Absyn_Instantiate_e_struct*_tmp15D=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp133;if(_tmp15D->tag != 14)goto _LL85;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;}}
_LL84: return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp15E),((struct
Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp15F),e->loc);_LL85: {struct Cyc_Absyn_Cast_e_struct*_tmp160=(struct Cyc_Absyn_Cast_e_struct*)
_tmp133;if(_tmp160->tag != 15)goto _LL87;else{_tmp161=(void*)_tmp160->f1;_tmp162=
_tmp160->f2;_tmp163=_tmp160->f3;_tmp164=_tmp160->f4;}}_LL86: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp161),Cyc_Tcutil_deep_copy_exp(_tmp162),_tmp163,_tmp164,
e->loc);_LL87: {struct Cyc_Absyn_Address_e_struct*_tmp165=(struct Cyc_Absyn_Address_e_struct*)
_tmp133;if(_tmp165->tag != 16)goto _LL89;else{_tmp166=_tmp165->f1;}}_LL88: return
Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp166),e->loc);_LL89: {struct
Cyc_Absyn_New_e_struct*_tmp167=(struct Cyc_Absyn_New_e_struct*)_tmp133;if(_tmp167->tag
!= 17)goto _LL8B;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;}}_LL8A: {struct Cyc_Absyn_Exp*
eo1=_tmp168;if(_tmp168 != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp168);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp169),e->loc);}_LL8B: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp16A=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp133;if(_tmp16A->tag != 18)goto _LL8D;else{_tmp16B=(void*)_tmp16A->f1;}}_LL8C:
return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp16B),e->loc);_LL8D: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp16C=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp133;if(_tmp16C->tag != 19)goto _LL8F;else{_tmp16D=_tmp16C->f1;}}_LL8E: return
Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp16D),e->loc);_LL8F: {struct
Cyc_Absyn_Offsetof_e_struct*_tmp16E=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp133;
if(_tmp16E->tag != 20)goto _LL91;else{_tmp16F=(void*)_tmp16E->f1;_tmp170=(void*)
_tmp16E->f2;}}_LL90: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp16F),
_tmp170,e->loc);_LL91: {struct Cyc_Absyn_Deref_e_struct*_tmp171=(struct Cyc_Absyn_Deref_e_struct*)
_tmp133;if(_tmp171->tag != 21)goto _LL93;else{_tmp172=_tmp171->f1;}}_LL92: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp172),e->loc);_LL93: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp173=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp133;if(_tmp173->tag != 22)goto
_LL95;else{_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;_tmp176=_tmp173->f3;_tmp177=
_tmp173->f4;}}_LL94: {struct Cyc_Absyn_AggrMember_e_struct _tmp1164;struct Cyc_Absyn_AggrMember_e_struct*
_tmp1163;return Cyc_Absyn_new_exp((void*)((_tmp1163=_cycalloc(sizeof(*_tmp1163)),((
_tmp1163[0]=((_tmp1164.tag=22,((_tmp1164.f1=Cyc_Tcutil_deep_copy_exp(_tmp174),((
_tmp1164.f2=_tmp175,((_tmp1164.f3=_tmp176,((_tmp1164.f4=_tmp177,_tmp1164)))))))))),
_tmp1163)))),e->loc);}_LL95: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp178=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp133;if(_tmp178->tag != 23)goto _LL97;else{
_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;_tmp17B=_tmp178->f3;_tmp17C=_tmp178->f4;}}
_LL96: {struct Cyc_Absyn_AggrArrow_e_struct _tmp1167;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1166;return Cyc_Absyn_new_exp((void*)((_tmp1166=_cycalloc(sizeof(*_tmp1166)),((
_tmp1166[0]=((_tmp1167.tag=23,((_tmp1167.f1=Cyc_Tcutil_deep_copy_exp(_tmp179),((
_tmp1167.f2=_tmp17A,((_tmp1167.f3=_tmp17B,((_tmp1167.f4=_tmp17C,_tmp1167)))))))))),
_tmp1166)))),e->loc);}_LL97: {struct Cyc_Absyn_Subscript_e_struct*_tmp17D=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp133;if(_tmp17D->tag != 24)goto _LL99;else{
_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}}_LL98: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp17E),Cyc_Tcutil_deep_copy_exp(_tmp17F),e->loc);
_LL99: {struct Cyc_Absyn_Tuple_e_struct*_tmp180=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp133;if(_tmp180->tag != 25)goto _LL9B;else{_tmp181=_tmp180->f1;}}_LL9A: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp181),e->loc);
_LL9B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp182=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp133;if(_tmp182->tag != 26)goto _LL9D;else{_tmp183=_tmp182->f1;_tmp184=*_tmp183;
_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;_tmp187=_tmp184.f3;_tmp188=_tmp182->f2;}}
_LL9C: {struct Cyc_Core_Opt*vopt1=_tmp185;if(_tmp185 != 0){struct Cyc_Core_Opt*
_tmp1168;vopt1=((_tmp1168=_cycalloc(sizeof(*_tmp1168)),((_tmp1168->v=(struct
_dyneither_ptr*)_tmp185->v,_tmp1168))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp116E;struct _tuple9*_tmp116D;struct Cyc_Absyn_CompoundLit_e_struct*_tmp116C;
return Cyc_Absyn_new_exp((void*)((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((
_tmp116C[0]=((_tmp116E.tag=26,((_tmp116E.f1=((_tmp116D=_cycalloc(sizeof(*
_tmp116D)),((_tmp116D->f1=vopt1,((_tmp116D->f2=_tmp186,((_tmp116D->f3=Cyc_Tcutil_copy_type(
_tmp187),_tmp116D)))))))),((_tmp116E.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp188),_tmp116E)))))),_tmp116C)))),e->loc);};}_LL9D: {struct Cyc_Absyn_Array_e_struct*
_tmp189=(struct Cyc_Absyn_Array_e_struct*)_tmp133;if(_tmp189->tag != 27)goto _LL9F;
else{_tmp18A=_tmp189->f1;}}_LL9E: {struct Cyc_Absyn_Array_e_struct _tmp1171;struct
Cyc_Absyn_Array_e_struct*_tmp1170;return Cyc_Absyn_new_exp((void*)((_tmp1170=
_cycalloc(sizeof(*_tmp1170)),((_tmp1170[0]=((_tmp1171.tag=27,((_tmp1171.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18A),_tmp1171)))),_tmp1170)))),e->loc);}
_LL9F: {struct Cyc_Absyn_Comprehension_e_struct*_tmp18B=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp133;if(_tmp18B->tag != 28)goto _LLA1;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LLA0: {struct Cyc_Absyn_Comprehension_e_struct
_tmp1174;struct Cyc_Absyn_Comprehension_e_struct*_tmp1173;return Cyc_Absyn_new_exp((
void*)((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173[0]=((_tmp1174.tag=28,((
_tmp1174.f1=_tmp18C,((_tmp1174.f2=Cyc_Tcutil_deep_copy_exp(_tmp18D),((_tmp1174.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18E),((_tmp1174.f4=_tmp18F,_tmp1174)))))))))),
_tmp1173)))),e->loc);}_LLA1: {struct Cyc_Absyn_Aggregate_e_struct*_tmp190=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp133;if(_tmp190->tag != 29)goto _LLA3;else{
_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;_tmp193=_tmp190->f3;_tmp194=_tmp190->f4;}}
_LLA2: {struct Cyc_Absyn_Aggregate_e_struct _tmp1177;struct Cyc_Absyn_Aggregate_e_struct*
_tmp1176;return Cyc_Absyn_new_exp((void*)((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((
_tmp1176[0]=((_tmp1177.tag=29,((_tmp1177.f1=_tmp191,((_tmp1177.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp192),((_tmp1177.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp193),((
_tmp1177.f4=_tmp194,_tmp1177)))))))))),_tmp1176)))),e->loc);}_LLA3: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp195=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp133;if(_tmp195->tag != 30)goto
_LLA5;else{_tmp196=(void*)_tmp195->f1;_tmp197=_tmp195->f2;}}_LLA4: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp117A;struct Cyc_Absyn_AnonStruct_e_struct*_tmp1179;return Cyc_Absyn_new_exp((
void*)((_tmp1179=_cycalloc(sizeof(*_tmp1179)),((_tmp1179[0]=((_tmp117A.tag=30,((
_tmp117A.f1=(void*)Cyc_Tcutil_copy_type(_tmp196),((_tmp117A.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp197),_tmp117A)))))),_tmp1179)))),e->loc);}_LLA5: {struct Cyc_Absyn_Datatype_e_struct*
_tmp198=(struct Cyc_Absyn_Datatype_e_struct*)_tmp133;if(_tmp198->tag != 31)goto
_LLA7;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;_tmp19B=_tmp198->f3;}}_LLA6: {
struct Cyc_Absyn_Datatype_e_struct _tmp117D;struct Cyc_Absyn_Datatype_e_struct*
_tmp117C;return Cyc_Absyn_new_exp((void*)((_tmp117C=_cycalloc(sizeof(*_tmp117C)),((
_tmp117C[0]=((_tmp117D.tag=31,((_tmp117D.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp199),((_tmp117D.f2=_tmp19A,((_tmp117D.f3=_tmp19B,_tmp117D)))))))),_tmp117C)))),
e->loc);}_LLA7: {struct Cyc_Absyn_Enum_e_struct*_tmp19C=(struct Cyc_Absyn_Enum_e_struct*)
_tmp133;if(_tmp19C->tag != 32)goto _LLA9;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;
_tmp19F=_tmp19C->f3;}}_LLA8: return e;_LLA9: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1A0=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp133;if(_tmp1A0->tag != 33)goto
_LLAB;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=(void*)_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;}}
_LLAA: {struct Cyc_Absyn_AnonEnum_e_struct _tmp1180;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp117F;return Cyc_Absyn_new_exp((void*)((_tmp117F=_cycalloc(sizeof(*_tmp117F)),((
_tmp117F[0]=((_tmp1180.tag=33,((_tmp1180.f1=_tmp1A1,((_tmp1180.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A2),((_tmp1180.f3=_tmp1A3,_tmp1180)))))))),_tmp117F)))),e->loc);}_LLAB: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A4=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp133;if(_tmp1A4->tag != 34)goto _LLAD;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A5.is_calloc;
_tmp1A7=_tmp1A5.rgn;_tmp1A8=_tmp1A5.elt_type;_tmp1A9=_tmp1A5.num_elts;_tmp1AA=
_tmp1A5.fat_result;}}_LLAC: {struct Cyc_Absyn_Exp*_tmp1E0=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A7;if(_tmp1A7 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A7);{void**t1=_tmp1A8;if(_tmp1A8 != 0){void**_tmp1181;
t1=((_tmp1181=_cycalloc(sizeof(*_tmp1181)),((_tmp1181[0]=Cyc_Tcutil_copy_type(*
_tmp1A8),_tmp1181))));}{struct Cyc_Absyn_Malloc_e_struct _tmp1187;struct Cyc_Absyn_MallocInfo
_tmp1186;struct Cyc_Absyn_Malloc_e_struct*_tmp1185;_tmp1E0->r=(void*)((_tmp1185=
_cycalloc(sizeof(*_tmp1185)),((_tmp1185[0]=((_tmp1187.tag=34,((_tmp1187.f1=((
_tmp1186.is_calloc=_tmp1A6,((_tmp1186.rgn=r1,((_tmp1186.elt_type=t1,((_tmp1186.num_elts=
_tmp1A9,((_tmp1186.fat_result=_tmp1AA,_tmp1186)))))))))),_tmp1187)))),_tmp1185))));}
return _tmp1E0;};}_LLAD: {struct Cyc_Absyn_Swap_e_struct*_tmp1AB=(struct Cyc_Absyn_Swap_e_struct*)
_tmp133;if(_tmp1AB->tag != 35)goto _LLAF;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}
_LLAE: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AC),Cyc_Tcutil_deep_copy_exp(
_tmp1AD),e->loc);_LLAF: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AE=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp133;if(_tmp1AE->tag != 36)goto _LLB1;else{
_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLB0: {struct Cyc_Core_Opt*nopt1=
_tmp1AF;if(_tmp1AF != 0){struct Cyc_Core_Opt*_tmp1188;nopt1=((_tmp1188=_cycalloc(
sizeof(*_tmp1188)),((_tmp1188->v=(struct _tuple2*)_tmp1AF->v,_tmp1188))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp118B;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp118A;return Cyc_Absyn_new_exp((void*)((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((
_tmp118A[0]=((_tmp118B.tag=36,((_tmp118B.f1=nopt1,((_tmp118B.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1B0),_tmp118B)))))),_tmp118A)))),e->loc);};}_LLB1: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1B1=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp133;if(_tmp1B1->tag != 37)goto
_LLB3;}_LLB2: {struct Cyc_Core_Failure_struct _tmp1191;const char*_tmp1190;struct
Cyc_Core_Failure_struct*_tmp118F;(int)_throw((void*)((_tmp118F=_cycalloc(sizeof(*
_tmp118F)),((_tmp118F[0]=((_tmp1191.tag=Cyc_Core_Failure,((_tmp1191.f1=((
_tmp1190="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp1190,
sizeof(char),45))),_tmp1191)))),_tmp118F)))));}_LLB3: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B2=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp133;if(_tmp1B2->tag != 38)goto
_LLB5;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LLB4: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp1194;struct Cyc_Absyn_Tagcheck_e_struct*_tmp1193;return Cyc_Absyn_new_exp((
void*)((_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193[0]=((_tmp1194.tag=38,((
_tmp1194.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B3),((_tmp1194.f2=_tmp1B4,_tmp1194)))))),
_tmp1193)))),e->loc);}_LLB5: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B5=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp133;if(_tmp1B5->tag != 39)goto _LLB7;else{_tmp1B6=(
void*)_tmp1B5->f1;}}_LLB6: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp1B6),e->loc);_LLB7: {struct Cyc_Absyn_Asm_e_struct*_tmp1B7=(struct Cyc_Absyn_Asm_e_struct*)
_tmp133;if(_tmp1B7->tag != 40)goto _LL66;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}
_LLB8: return Cyc_Absyn_asm_exp(_tmp1B8,_tmp1B9,e->loc);_LL66:;}struct _tuple19{
enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual
f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,
struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1EE;enum Cyc_Absyn_KindQual
_tmp1EF;enum Cyc_Absyn_AliasQual _tmp1F0;struct Cyc_Absyn_Kind*_tmp1ED=ka1;_tmp1EE=*
_tmp1ED;_tmp1EF=_tmp1EE.kind;_tmp1F0=_tmp1EE.aliasqual;{struct Cyc_Absyn_Kind
_tmp1F2;enum Cyc_Absyn_KindQual _tmp1F3;enum Cyc_Absyn_AliasQual _tmp1F4;struct Cyc_Absyn_Kind*
_tmp1F1=ka2;_tmp1F2=*_tmp1F1;_tmp1F3=_tmp1F2.kind;_tmp1F4=_tmp1F2.aliasqual;if(
_tmp1EF != _tmp1F3){struct _tuple19 _tmp1195;struct _tuple19 _tmp1F6=(_tmp1195.f1=
_tmp1EF,((_tmp1195.f2=_tmp1F3,_tmp1195)));enum Cyc_Absyn_KindQual _tmp1F7;enum 
Cyc_Absyn_KindQual _tmp1F8;enum Cyc_Absyn_KindQual _tmp1F9;enum Cyc_Absyn_KindQual
_tmp1FA;enum Cyc_Absyn_KindQual _tmp1FB;enum Cyc_Absyn_KindQual _tmp1FC;_LLBF:
_tmp1F7=_tmp1F6.f1;if(_tmp1F7 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F8=_tmp1F6.f2;
if(_tmp1F8 != Cyc_Absyn_MemKind)goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1F9=_tmp1F6.f1;
if(_tmp1F9 != Cyc_Absyn_BoxKind)goto _LLC3;_tmp1FA=_tmp1F6.f2;if(_tmp1FA != Cyc_Absyn_AnyKind)
goto _LLC3;_LLC2: goto _LLC4;_LLC3: _tmp1FB=_tmp1F6.f1;if(_tmp1FB != Cyc_Absyn_MemKind)
goto _LLC5;_tmp1FC=_tmp1F6.f2;if(_tmp1FC != Cyc_Absyn_AnyKind)goto _LLC5;_LLC4: goto
_LLBE;_LLC5:;_LLC6: return 0;_LLBE:;}if(_tmp1F0 != _tmp1F4){struct _tuple20 _tmp1196;
struct _tuple20 _tmp1FE=(_tmp1196.f1=_tmp1F0,((_tmp1196.f2=_tmp1F4,_tmp1196)));
enum Cyc_Absyn_AliasQual _tmp1FF;enum Cyc_Absyn_AliasQual _tmp200;enum Cyc_Absyn_AliasQual
_tmp201;enum Cyc_Absyn_AliasQual _tmp202;_LLC8: _tmp1FF=_tmp1FE.f1;if(_tmp1FF != 
Cyc_Absyn_Aliasable)goto _LLCA;_tmp200=_tmp1FE.f2;if(_tmp200 != Cyc_Absyn_Top)goto
_LLCA;_LLC9: goto _LLCB;_LLCA: _tmp201=_tmp1FE.f1;if(_tmp201 != Cyc_Absyn_Unique)
goto _LLCC;_tmp202=_tmp1FE.f2;if(_tmp202 != Cyc_Absyn_Top)goto _LLCC;_LLCB: return 1;
_LLCC:;_LLCD: return 0;_LLC7:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp203=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp205;struct Cyc_Absyn_Kind*_tmp207;_LLCF: {
struct Cyc_Absyn_Eq_kb_struct*_tmp204=(struct Cyc_Absyn_Eq_kb_struct*)_tmp203;if(
_tmp204->tag != 0)goto _LLD1;else{_tmp205=_tmp204->f1;}}_LLD0: return _tmp205;_LLD1: {
struct Cyc_Absyn_Less_kb_struct*_tmp206=(struct Cyc_Absyn_Less_kb_struct*)_tmp203;
if(_tmp206->tag != 2)goto _LLD3;else{_tmp207=_tmp206->f2;}}_LLD2: return _tmp207;
_LLD3:;_LLD4:{struct Cyc_Absyn_Less_kb_struct _tmp1199;struct Cyc_Absyn_Less_kb_struct*
_tmp1198;tv->kind=(void*)((_tmp1198=_cycalloc(sizeof(*_tmp1198)),((_tmp1198[0]=((
_tmp1199.tag=2,((_tmp1199.f1=0,((_tmp1199.f2=def,_tmp1199)))))),_tmp1198))));}
return def;_LLCE:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp119A;struct _tuple0 _tmp20B=(_tmp119A.f1=Cyc_Absyn_compress_kb(kb1),((_tmp119A.f2=
Cyc_Absyn_compress_kb(kb2),_tmp119A)));void*_tmp20C;struct Cyc_Absyn_Kind*_tmp20E;
void*_tmp20F;struct Cyc_Absyn_Kind*_tmp211;void*_tmp212;struct Cyc_Core_Opt*
_tmp214;struct Cyc_Core_Opt**_tmp215;struct Cyc_Absyn_Kind*_tmp216;void*_tmp217;
struct Cyc_Absyn_Kind*_tmp219;void*_tmp21A;struct Cyc_Absyn_Kind*_tmp21C;void*
_tmp21D;struct Cyc_Core_Opt*_tmp21F;struct Cyc_Core_Opt**_tmp220;struct Cyc_Absyn_Kind*
_tmp221;void*_tmp222;struct Cyc_Core_Opt*_tmp224;struct Cyc_Core_Opt**_tmp225;
struct Cyc_Absyn_Kind*_tmp226;void*_tmp227;struct Cyc_Core_Opt*_tmp229;struct Cyc_Core_Opt**
_tmp22A;struct Cyc_Absyn_Kind*_tmp22B;void*_tmp22C;struct Cyc_Core_Opt*_tmp22E;
struct Cyc_Core_Opt**_tmp22F;void*_tmp230;void*_tmp231;void*_tmp232;struct Cyc_Core_Opt*
_tmp234;struct Cyc_Core_Opt**_tmp235;_LLD6: _tmp20C=_tmp20B.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp20D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp20C;if(_tmp20D->tag != 0)goto _LLD8;
else{_tmp20E=_tmp20D->f1;}};_tmp20F=_tmp20B.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp210=(struct Cyc_Absyn_Eq_kb_struct*)_tmp20F;if(_tmp210->tag != 0)goto _LLD8;
else{_tmp211=_tmp210->f1;}};_LLD7: return _tmp20E == _tmp211;_LLD8: _tmp212=_tmp20B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp213=(struct Cyc_Absyn_Less_kb_struct*)_tmp212;
if(_tmp213->tag != 2)goto _LLDA;else{_tmp214=_tmp213->f1;_tmp215=(struct Cyc_Core_Opt**)&
_tmp213->f1;_tmp216=_tmp213->f2;}};_tmp217=_tmp20B.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp218=(struct Cyc_Absyn_Eq_kb_struct*)_tmp217;if(_tmp218->tag != 0)goto _LLDA;
else{_tmp219=_tmp218->f1;}};_LLD9: if(Cyc_Tcutil_kind_leq(_tmp219,_tmp216)){{
struct Cyc_Core_Opt*_tmp119B;*_tmp215=((_tmp119B=_cycalloc(sizeof(*_tmp119B)),((
_tmp119B->v=kb2,_tmp119B))));}return 1;}else{return 0;}_LLDA: _tmp21A=_tmp20B.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp21B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp21A;if(
_tmp21B->tag != 0)goto _LLDC;else{_tmp21C=_tmp21B->f1;}};_tmp21D=_tmp20B.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21E=(struct Cyc_Absyn_Less_kb_struct*)_tmp21D;
if(_tmp21E->tag != 2)goto _LLDC;else{_tmp21F=_tmp21E->f1;_tmp220=(struct Cyc_Core_Opt**)&
_tmp21E->f1;_tmp221=_tmp21E->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp21C,_tmp221)){{
struct Cyc_Core_Opt*_tmp119C;*_tmp220=((_tmp119C=_cycalloc(sizeof(*_tmp119C)),((
_tmp119C->v=kb1,_tmp119C))));}return 1;}else{return 0;}_LLDC: _tmp222=_tmp20B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp223=(struct Cyc_Absyn_Less_kb_struct*)_tmp222;
if(_tmp223->tag != 2)goto _LLDE;else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)&
_tmp223->f1;_tmp226=_tmp223->f2;}};_tmp227=_tmp20B.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp228=(struct Cyc_Absyn_Less_kb_struct*)_tmp227;if(_tmp228->tag != 2)goto _LLDE;
else{_tmp229=_tmp228->f1;_tmp22A=(struct Cyc_Core_Opt**)& _tmp228->f1;_tmp22B=
_tmp228->f2;}};_LLDD: if(Cyc_Tcutil_kind_leq(_tmp226,_tmp22B)){{struct Cyc_Core_Opt*
_tmp119D;*_tmp22A=((_tmp119D=_cycalloc(sizeof(*_tmp119D)),((_tmp119D->v=kb1,
_tmp119D))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp22B,_tmp226)){{struct Cyc_Core_Opt*
_tmp119E;*_tmp225=((_tmp119E=_cycalloc(sizeof(*_tmp119E)),((_tmp119E->v=kb2,
_tmp119E))));}return 1;}else{return 0;}}_LLDE: _tmp22C=_tmp20B.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22C;if(_tmp22D->tag != 1)goto
_LLE0;else{_tmp22E=_tmp22D->f1;_tmp22F=(struct Cyc_Core_Opt**)& _tmp22D->f1;}};
_tmp230=_tmp20B.f2;_LLDF: _tmp231=_tmp230;_tmp235=_tmp22F;goto _LLE1;_LLE0: _tmp231=
_tmp20B.f1;_tmp232=_tmp20B.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp233=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp232;if(_tmp233->tag != 1)goto _LLD5;else{_tmp234=
_tmp233->f1;_tmp235=(struct Cyc_Core_Opt**)& _tmp233->f1;}};_LLE1:{struct Cyc_Core_Opt*
_tmp119F;*_tmp235=((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F->v=_tmp231,
_tmp119F))));}return 1;_LLD5:;}struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp23B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23D;_LLE3: {struct
Cyc_Absyn_VarType_struct*_tmp23C=(struct Cyc_Absyn_VarType_struct*)_tmp23B;if(
_tmp23C->tag != 2)goto _LLE5;else{_tmp23D=_tmp23C->f1;}}_LLE4: {void*_tmp23E=
_tmp23D->kind;_tmp23D->kind=kb;{struct _tuple15 _tmp11A0;return(_tmp11A0.f1=
_tmp23D,((_tmp11A0.f2=_tmp23E,_tmp11A0)));};}_LLE5:;_LLE6: {const char*_tmp11A4;
void*_tmp11A3[1];struct Cyc_String_pa_struct _tmp11A2;(_tmp11A2.tag=0,((_tmp11A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp11A3[0]=& _tmp11A2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11A4="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp11A4,sizeof(char),40))),_tag_dyneither(_tmp11A3,sizeof(void*),
1)))))));}_LLE2:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp243=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp245;struct Cyc_Absyn_Tvar*_tmp247;
enum Cyc_Absyn_Size_of _tmp24A;struct Cyc_Absyn_DatatypeFieldInfo _tmp257;union Cyc_Absyn_DatatypeFieldInfoU
_tmp258;struct _tuple3 _tmp259;struct Cyc_Absyn_Datatypedecl*_tmp25A;struct Cyc_Absyn_Datatypefield*
_tmp25B;struct Cyc_Absyn_DatatypeFieldInfo _tmp25D;union Cyc_Absyn_DatatypeFieldInfoU
_tmp25E;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp25F;struct Cyc_Absyn_AggrInfo
_tmp261;union Cyc_Absyn_AggrInfoU _tmp262;struct _tuple4 _tmp263;struct Cyc_Absyn_AggrInfo
_tmp265;union Cyc_Absyn_AggrInfoU _tmp266;struct Cyc_Absyn_Aggrdecl**_tmp267;struct
Cyc_Absyn_Aggrdecl*_tmp268;struct Cyc_Absyn_Aggrdecl _tmp269;struct Cyc_List_List*
_tmp26A;struct Cyc_Absyn_AggrdeclImpl*_tmp26B;struct Cyc_List_List*_tmp26C;struct
Cyc_Absyn_PtrInfo _tmp26F;struct Cyc_Absyn_Typedefdecl*_tmp275;struct Cyc_Absyn_TypeDecl*
_tmp277;struct Cyc_Absyn_TypeDecl _tmp278;void*_tmp279;struct Cyc_Absyn_TypeDecl*
_tmp27C;struct Cyc_Absyn_TypeDecl _tmp27D;void*_tmp27E;struct Cyc_Absyn_TypeDecl*
_tmp281;struct Cyc_Absyn_TypeDecl _tmp282;void*_tmp283;_LLE8: {struct Cyc_Absyn_Evar_struct*
_tmp244=(struct Cyc_Absyn_Evar_struct*)_tmp243;if(_tmp244->tag != 1)goto _LLEA;
else{_tmp245=_tmp244->f1;}}_LLE9: return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmp245))->v;_LLEA: {struct Cyc_Absyn_VarType_struct*_tmp246=(struct
Cyc_Absyn_VarType_struct*)_tmp243;if(_tmp246->tag != 2)goto _LLEC;else{_tmp247=
_tmp246->f1;}}_LLEB: return Cyc_Tcutil_tvar_kind(_tmp247,& Cyc_Tcutil_bk);_LLEC: {
struct Cyc_Absyn_VoidType_struct*_tmp248=(struct Cyc_Absyn_VoidType_struct*)
_tmp243;if(_tmp248->tag != 0)goto _LLEE;}_LLED: return& Cyc_Tcutil_mk;_LLEE: {struct
Cyc_Absyn_IntType_struct*_tmp249=(struct Cyc_Absyn_IntType_struct*)_tmp243;if(
_tmp249->tag != 6)goto _LLF0;else{_tmp24A=_tmp249->f2;}}_LLEF: return(_tmp24A == (
enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp24A == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?&
Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF0: {struct Cyc_Absyn_FloatType_struct*_tmp24B=(
struct Cyc_Absyn_FloatType_struct*)_tmp243;if(_tmp24B->tag != 7)goto _LLF2;}_LLF1:
goto _LLF3;_LLF2: {struct Cyc_Absyn_DoubleType_struct*_tmp24C=(struct Cyc_Absyn_DoubleType_struct*)
_tmp243;if(_tmp24C->tag != 8)goto _LLF4;}_LLF3: goto _LLF5;_LLF4: {struct Cyc_Absyn_FnType_struct*
_tmp24D=(struct Cyc_Absyn_FnType_struct*)_tmp243;if(_tmp24D->tag != 10)goto _LLF6;}
_LLF5: return& Cyc_Tcutil_mk;_LLF6: {struct Cyc_Absyn_DynRgnType_struct*_tmp24E=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp243;if(_tmp24E->tag != 17)goto _LLF8;}_LLF7:
goto _LLF9;_LLF8: {struct Cyc_Absyn_EnumType_struct*_tmp24F=(struct Cyc_Absyn_EnumType_struct*)
_tmp243;if(_tmp24F->tag != 14)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp250=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp243;if(_tmp250->tag != 15)goto
_LLFC;}_LLFB: goto _LLFD;_LLFC: {struct Cyc_Absyn_RgnHandleType_struct*_tmp251=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp243;if(_tmp251->tag != 16)goto _LLFE;}
_LLFD: return& Cyc_Tcutil_bk;_LLFE: {struct Cyc_Absyn_UniqueRgn_struct*_tmp252=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp243;if(_tmp252->tag != 22)goto _LL100;}_LLFF:
return& Cyc_Tcutil_urk;_LL100: {struct Cyc_Absyn_HeapRgn_struct*_tmp253=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp243;if(_tmp253->tag != 21)goto _LL102;}_LL101: return& Cyc_Tcutil_rk;_LL102: {
struct Cyc_Absyn_RefCntRgn_struct*_tmp254=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp243;if(_tmp254->tag != 23)goto _LL104;}_LL103: return& Cyc_Tcutil_trk;_LL104: {
struct Cyc_Absyn_DatatypeType_struct*_tmp255=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp243;if(_tmp255->tag != 3)goto _LL106;}_LL105: return& Cyc_Tcutil_ak;_LL106: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp256=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp243;if(_tmp256->tag != 4)goto _LL108;else{_tmp257=_tmp256->f1;_tmp258=_tmp257.field_info;
if((_tmp258.KnownDatatypefield).tag != 2)goto _LL108;_tmp259=(struct _tuple3)(
_tmp258.KnownDatatypefield).val;_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;}}_LL107:
return& Cyc_Tcutil_mk;_LL108: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp25C=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp243;if(_tmp25C->tag != 4)goto _LL10A;
else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25D.field_info;if((_tmp25E.UnknownDatatypefield).tag
!= 1)goto _LL10A;_tmp25F=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp25E.UnknownDatatypefield).val;}}
_LL109: {const char*_tmp11A7;void*_tmp11A6;(_tmp11A6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A7="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp11A7,sizeof(char),39))),_tag_dyneither(_tmp11A6,sizeof(void*),
0)));}_LL10A: {struct Cyc_Absyn_AggrType_struct*_tmp260=(struct Cyc_Absyn_AggrType_struct*)
_tmp243;if(_tmp260->tag != 12)goto _LL10C;else{_tmp261=_tmp260->f1;_tmp262=_tmp261.aggr_info;
if((_tmp262.UnknownAggr).tag != 1)goto _LL10C;_tmp263=(struct _tuple4)(_tmp262.UnknownAggr).val;}}
_LL10B: return& Cyc_Tcutil_ak;_LL10C: {struct Cyc_Absyn_AggrType_struct*_tmp264=(
struct Cyc_Absyn_AggrType_struct*)_tmp243;if(_tmp264->tag != 12)goto _LL10E;else{
_tmp265=_tmp264->f1;_tmp266=_tmp265.aggr_info;if((_tmp266.KnownAggr).tag != 2)
goto _LL10E;_tmp267=(struct Cyc_Absyn_Aggrdecl**)(_tmp266.KnownAggr).val;_tmp268=*
_tmp267;_tmp269=*_tmp268;_tmp26A=_tmp269.tvs;_tmp26B=_tmp269.impl;_tmp26C=
_tmp265.targs;}}_LL10D: if(_tmp26B == 0)return& Cyc_Tcutil_ak;{struct Cyc_List_List*
_tmp28A=_tmp26B->fields;if(_tmp28A == 0)return& Cyc_Tcutil_mk;for(0;_tmp28A->tl != 
0;_tmp28A=_tmp28A->tl){;}{void*_tmp28B=((struct Cyc_Absyn_Aggrfield*)_tmp28A->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp28B);if(_tmp26C != 0  && (k == & Cyc_Tcutil_ak
 || k == & Cyc_Tcutil_tak)){struct _RegionHandle _tmp28C=_new_region("temp");struct
_RegionHandle*temp=& _tmp28C;_push_region(temp);{struct Cyc_List_List*_tmp28D=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp26A,_tmp26C);_tmp28B=Cyc_Tcutil_rsubstitute(
temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp26A,
_tmp26C),_tmp28B);k=Cyc_Tcutil_typ_kind(_tmp28B);};_pop_region(temp);}if(k == &
Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)return k;return& Cyc_Tcutil_mk;};};_LL10E: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp26D=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp243;if(_tmp26D->tag != 13)goto _LL110;}_LL10F: return& Cyc_Tcutil_mk;_LL110: {
struct Cyc_Absyn_PointerType_struct*_tmp26E=(struct Cyc_Absyn_PointerType_struct*)
_tmp243;if(_tmp26E->tag != 5)goto _LL112;else{_tmp26F=_tmp26E->f1;}}_LL111: {void*
_tmp28E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp26F.ptr_atts).bounds);_LL129: {struct Cyc_Absyn_DynEither_b_struct*
_tmp28F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp28E;if(_tmp28F->tag != 0)goto
_LL12B;}_LL12A: {struct Cyc_Absyn_Kind*_tmp11A8;return(_tmp11A8=_cycalloc_atomic(
sizeof(*_tmp11A8)),((_tmp11A8->kind=Cyc_Absyn_MemKind,((_tmp11A8->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26F.ptr_atts).rgn))->aliasqual,_tmp11A8)))));}_LL12B: {struct Cyc_Absyn_Upper_b_struct*
_tmp290=(struct Cyc_Absyn_Upper_b_struct*)_tmp28E;if(_tmp290->tag != 1)goto _LL128;}
_LL12C: {struct Cyc_Absyn_Kind*_tmp11A9;return(_tmp11A9=_cycalloc_atomic(sizeof(*
_tmp11A9)),((_tmp11A9->kind=Cyc_Absyn_BoxKind,((_tmp11A9->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26F.ptr_atts).rgn))->aliasqual,_tmp11A9)))));}_LL128:;}_LL112: {struct Cyc_Absyn_ValueofType_struct*
_tmp270=(struct Cyc_Absyn_ValueofType_struct*)_tmp243;if(_tmp270->tag != 19)goto
_LL114;}_LL113: return& Cyc_Tcutil_ik;_LL114: {struct Cyc_Absyn_TagType_struct*
_tmp271=(struct Cyc_Absyn_TagType_struct*)_tmp243;if(_tmp271->tag != 20)goto _LL116;}
_LL115: return& Cyc_Tcutil_bk;_LL116: {struct Cyc_Absyn_ArrayType_struct*_tmp272=(
struct Cyc_Absyn_ArrayType_struct*)_tmp243;if(_tmp272->tag != 9)goto _LL118;}_LL117:
goto _LL119;_LL118: {struct Cyc_Absyn_TupleType_struct*_tmp273=(struct Cyc_Absyn_TupleType_struct*)
_tmp243;if(_tmp273->tag != 11)goto _LL11A;}_LL119: return& Cyc_Tcutil_mk;_LL11A: {
struct Cyc_Absyn_TypedefType_struct*_tmp274=(struct Cyc_Absyn_TypedefType_struct*)
_tmp243;if(_tmp274->tag != 18)goto _LL11C;else{_tmp275=_tmp274->f3;}}_LL11B: if(
_tmp275 == 0  || _tmp275->kind == 0){const char*_tmp11AD;void*_tmp11AC[1];struct Cyc_String_pa_struct
_tmp11AB;(_tmp11AB.tag=0,((_tmp11AB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp11AC[0]=& _tmp11AB,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AD="typ_kind: typedef found: %s",
_tag_dyneither(_tmp11AD,sizeof(char),28))),_tag_dyneither(_tmp11AC,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp275->kind))->v;
_LL11C: {struct Cyc_Absyn_TypeDeclType_struct*_tmp276=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp243;if(_tmp276->tag != 27)goto _LL11E;else{_tmp277=_tmp276->f1;_tmp278=*
_tmp277;_tmp279=_tmp278.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp27A=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp279;if(_tmp27A->tag != 0)goto _LL11E;};}}_LL11D: return& Cyc_Tcutil_ak;_LL11E: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp27B=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp243;if(_tmp27B->tag != 27)goto _LL120;else{_tmp27C=_tmp27B->f1;_tmp27D=*
_tmp27C;_tmp27E=_tmp27D.r;{struct Cyc_Absyn_Enum_td_struct*_tmp27F=(struct Cyc_Absyn_Enum_td_struct*)
_tmp27E;if(_tmp27F->tag != 1)goto _LL120;};}}_LL11F: return& Cyc_Tcutil_bk;_LL120: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp280=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp243;if(_tmp280->tag != 27)goto _LL122;else{_tmp281=_tmp280->f1;_tmp282=*
_tmp281;_tmp283=_tmp282.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp284=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp283;if(_tmp284->tag != 2)goto _LL122;};}}_LL121: return& Cyc_Tcutil_ak;_LL122: {
struct Cyc_Absyn_AccessEff_struct*_tmp285=(struct Cyc_Absyn_AccessEff_struct*)
_tmp243;if(_tmp285->tag != 24)goto _LL124;}_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_JoinEff_struct*
_tmp286=(struct Cyc_Absyn_JoinEff_struct*)_tmp243;if(_tmp286->tag != 25)goto _LL126;}
_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_RgnsEff_struct*_tmp287=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp243;if(_tmp287->tag != 26)goto _LLE7;}_LL127: return& Cyc_Tcutil_ek;_LLE7:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == 
k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp296;_push_handler(& _tmp296);{int _tmp298=
0;if(setjmp(_tmp296.handler))_tmp298=1;if(!_tmp298){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp299=1;_npop_handler(0);return _tmp299;};;_pop_handler();}else{void*_tmp297=(
void*)_exn_thrown;void*_tmp29B=_tmp297;_LL12E: {struct Cyc_Tcutil_Unify_struct*
_tmp29C=(struct Cyc_Tcutil_Unify_struct*)_tmp29B;if(_tmp29C->tag != Cyc_Tcutil_Unify)
goto _LL130;}_LL12F: return 0;_LL130:;_LL131:(void)_throw(_tmp29B);_LL12D:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*
_tmp29D=t;struct Cyc_Absyn_Tvar*_tmp29F;struct Cyc_Core_Opt*_tmp2A1;struct Cyc_Core_Opt*
_tmp2A2;struct Cyc_Core_Opt**_tmp2A3;struct Cyc_Absyn_PtrInfo _tmp2A5;struct Cyc_Absyn_ArrayInfo
_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_FnInfo _tmp2AA;struct Cyc_List_List*_tmp2AB;
struct Cyc_Core_Opt*_tmp2AC;struct Cyc_Absyn_Tqual _tmp2AD;void*_tmp2AE;struct Cyc_List_List*
_tmp2AF;int _tmp2B0;struct Cyc_Absyn_VarargInfo*_tmp2B1;struct Cyc_List_List*
_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_DatatypeInfo
_tmp2B7;struct Cyc_List_List*_tmp2B8;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_AggrInfo _tmp2BF;struct Cyc_List_List*
_tmp2C0;struct Cyc_List_List*_tmp2C2;void*_tmp2C4;void*_tmp2C6;void*_tmp2C8;void*
_tmp2CA;struct Cyc_List_List*_tmp2CC;_LL133: {struct Cyc_Absyn_VarType_struct*
_tmp29E=(struct Cyc_Absyn_VarType_struct*)_tmp29D;if(_tmp29E->tag != 2)goto _LL135;
else{_tmp29F=_tmp29E->f1;}}_LL134: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmp29F)){{const char*_tmp11AE;Cyc_Tcutil_failure_reason=((
_tmp11AE="(type variable would escape scope)",_tag_dyneither(_tmp11AE,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL132;_LL135: {
struct Cyc_Absyn_Evar_struct*_tmp2A0=(struct Cyc_Absyn_Evar_struct*)_tmp29D;if(
_tmp2A0->tag != 1)goto _LL137;else{_tmp2A1=_tmp2A0->f2;_tmp2A2=_tmp2A0->f4;_tmp2A3=(
struct Cyc_Core_Opt**)& _tmp2A0->f4;}}_LL136: if(t == evar){{const char*_tmp11AF;Cyc_Tcutil_failure_reason=((
_tmp11AF="(occurs check)",_tag_dyneither(_tmp11AF,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp2A1 != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp2A1->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp2A3))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2CF=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A3))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp11B0;_tmp2CF=((
_tmp11B0=_cycalloc(sizeof(*_tmp11B0)),((_tmp11B0->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp11B0->tl=_tmp2CF,_tmp11B0))))));}}}{struct Cyc_Core_Opt*_tmp11B1;*_tmp2A3=((
_tmp11B1=_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1->v=_tmp2CF,_tmp11B1))));};}}}
goto _LL132;_LL137: {struct Cyc_Absyn_PointerType_struct*_tmp2A4=(struct Cyc_Absyn_PointerType_struct*)
_tmp29D;if(_tmp2A4->tag != 5)goto _LL139;else{_tmp2A5=_tmp2A4->f1;}}_LL138: Cyc_Tcutil_occurs(
evar,r,env,_tmp2A5.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp2A5.ptr_atts).rgn);
goto _LL132;_LL139: {struct Cyc_Absyn_ArrayType_struct*_tmp2A6=(struct Cyc_Absyn_ArrayType_struct*)
_tmp29D;if(_tmp2A6->tag != 9)goto _LL13B;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A7.elt_type;}}
_LL13A: Cyc_Tcutil_occurs(evar,r,env,_tmp2A8);goto _LL132;_LL13B: {struct Cyc_Absyn_FnType_struct*
_tmp2A9=(struct Cyc_Absyn_FnType_struct*)_tmp29D;if(_tmp2A9->tag != 10)goto _LL13D;
else{_tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2AA.tvars;_tmp2AC=_tmp2AA.effect;_tmp2AD=
_tmp2AA.ret_tqual;_tmp2AE=_tmp2AA.ret_typ;_tmp2AF=_tmp2AA.args;_tmp2B0=_tmp2AA.c_varargs;
_tmp2B1=_tmp2AA.cyc_varargs;_tmp2B2=_tmp2AA.rgn_po;_tmp2B3=_tmp2AA.attributes;}}
_LL13C: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2AB,env);if(_tmp2AC != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp2AC->v);Cyc_Tcutil_occurs(evar,r,env,_tmp2AE);for(0;
_tmp2AF != 0;_tmp2AF=_tmp2AF->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple9*)
_tmp2AF->hd)).f3);}if(_tmp2B1 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp2B1->type);
for(0;_tmp2B2 != 0;_tmp2B2=_tmp2B2->tl){struct _tuple0 _tmp2D3;void*_tmp2D4;void*
_tmp2D5;struct _tuple0*_tmp2D2=(struct _tuple0*)_tmp2B2->hd;_tmp2D3=*_tmp2D2;
_tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;Cyc_Tcutil_occurs(evar,r,env,_tmp2D4);Cyc_Tcutil_occurs(
evar,r,env,_tmp2D5);}goto _LL132;_LL13D: {struct Cyc_Absyn_TupleType_struct*
_tmp2B4=(struct Cyc_Absyn_TupleType_struct*)_tmp29D;if(_tmp2B4->tag != 11)goto
_LL13F;else{_tmp2B5=_tmp2B4->f1;}}_LL13E: for(0;_tmp2B5 != 0;_tmp2B5=_tmp2B5->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp2B5->hd)).f2);}goto _LL132;
_LL13F: {struct Cyc_Absyn_DatatypeType_struct*_tmp2B6=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp29D;if(_tmp2B6->tag != 3)goto _LL141;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B7.targs;}}
_LL140: Cyc_Tcutil_occurslist(evar,r,env,_tmp2B8);goto _LL132;_LL141: {struct Cyc_Absyn_TypedefType_struct*
_tmp2B9=(struct Cyc_Absyn_TypedefType_struct*)_tmp29D;if(_tmp2B9->tag != 18)goto
_LL143;else{_tmp2BA=_tmp2B9->f2;}}_LL142: _tmp2BD=_tmp2BA;goto _LL144;_LL143: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2BB=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp29D;if(_tmp2BB->tag != 4)goto _LL145;else{_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BC.targs;}}
_LL144: _tmp2C0=_tmp2BD;goto _LL146;_LL145: {struct Cyc_Absyn_AggrType_struct*
_tmp2BE=(struct Cyc_Absyn_AggrType_struct*)_tmp29D;if(_tmp2BE->tag != 12)goto
_LL147;else{_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BF.targs;}}_LL146: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2C0);goto _LL132;_LL147: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2C1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp29D;if(_tmp2C1->tag != 13)goto
_LL149;else{_tmp2C2=_tmp2C1->f2;}}_LL148: for(0;_tmp2C2 != 0;_tmp2C2=_tmp2C2->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2C2->hd)->type);}
goto _LL132;_LL149: {struct Cyc_Absyn_TagType_struct*_tmp2C3=(struct Cyc_Absyn_TagType_struct*)
_tmp29D;if(_tmp2C3->tag != 20)goto _LL14B;else{_tmp2C4=(void*)_tmp2C3->f1;}}_LL14A:
_tmp2C6=_tmp2C4;goto _LL14C;_LL14B: {struct Cyc_Absyn_AccessEff_struct*_tmp2C5=(
struct Cyc_Absyn_AccessEff_struct*)_tmp29D;if(_tmp2C5->tag != 24)goto _LL14D;else{
_tmp2C6=(void*)_tmp2C5->f1;}}_LL14C: _tmp2C8=_tmp2C6;goto _LL14E;_LL14D: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp2C7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp29D;if(_tmp2C7->tag != 16)goto _LL14F;else{_tmp2C8=(void*)_tmp2C7->f1;}}_LL14E:
_tmp2CA=_tmp2C8;goto _LL150;_LL14F: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C9=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp29D;if(_tmp2C9->tag != 26)goto _LL151;else{
_tmp2CA=(void*)_tmp2C9->f1;}}_LL150: Cyc_Tcutil_occurs(evar,r,env,_tmp2CA);goto
_LL132;_LL151: {struct Cyc_Absyn_JoinEff_struct*_tmp2CB=(struct Cyc_Absyn_JoinEff_struct*)
_tmp29D;if(_tmp2CB->tag != 25)goto _LL153;else{_tmp2CC=_tmp2CB->f1;}}_LL152: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2CC);goto _LL132;_LL153:;_LL154: goto _LL132;_LL132:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp11B4;void*
_tmp11B3;(_tmp11B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11B4="tq1 real_const not set.",_tag_dyneither(_tmp11B4,
sizeof(char),24))),_tag_dyneither(_tmp11B3,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp11B7;void*_tmp11B6;(_tmp11B6=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11B7="tq2 real_const not set.",
_tag_dyneither(_tmp11B7,sizeof(char),24))),_tag_dyneither(_tmp11B6,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp11B8;Cyc_Tcutil_failure_reason=((_tmp11B8="(qualifiers don't match)",
_tag_dyneither(_tmp11B8,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2DB=x;union Cyc_Absyn_Constraint _tmp2DC;int _tmp2DD;union Cyc_Absyn_Constraint
_tmp2DE;void*_tmp2DF;union Cyc_Absyn_Constraint _tmp2E0;union Cyc_Absyn_Constraint*
_tmp2E1;_LL156: _tmp2DC=*_tmp2DB;if((_tmp2DC.No_constr).tag != 3)goto _LL158;
_tmp2DD=(int)(_tmp2DC.No_constr).val;_LL157:{union Cyc_Absyn_Constraint _tmp11B9;*
x=(((_tmp11B9.Forward_constr).val=y,(((_tmp11B9.Forward_constr).tag=2,_tmp11B9))));}
return;_LL158: _tmp2DE=*_tmp2DB;if((_tmp2DE.Eq_constr).tag != 1)goto _LL15A;_tmp2DF=(
void*)(_tmp2DE.Eq_constr).val;_LL159: {union Cyc_Absyn_Constraint*_tmp2E3=y;union
Cyc_Absyn_Constraint _tmp2E4;int _tmp2E5;union Cyc_Absyn_Constraint _tmp2E6;void*
_tmp2E7;union Cyc_Absyn_Constraint _tmp2E8;union Cyc_Absyn_Constraint*_tmp2E9;
_LL15D: _tmp2E4=*_tmp2E3;if((_tmp2E4.No_constr).tag != 3)goto _LL15F;_tmp2E5=(int)(
_tmp2E4.No_constr).val;_LL15E:*y=*x;return;_LL15F: _tmp2E6=*_tmp2E3;if((_tmp2E6.Eq_constr).tag
!= 1)goto _LL161;_tmp2E7=(void*)(_tmp2E6.Eq_constr).val;_LL160: if(cmp(_tmp2DF,
_tmp2E7)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL161: _tmp2E8=*_tmp2E3;if((_tmp2E8.Forward_constr).tag != 2)goto _LL15C;
_tmp2E9=(union Cyc_Absyn_Constraint*)(_tmp2E8.Forward_constr).val;_LL162: {const
char*_tmp11BC;void*_tmp11BB;(_tmp11BB=0,Cyc_Tcutil_impos(((_tmp11BC="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11BC,sizeof(char),40))),_tag_dyneither(_tmp11BB,sizeof(void*),
0)));}_LL15C:;}_LL15A: _tmp2E0=*_tmp2DB;if((_tmp2E0.Forward_constr).tag != 2)goto
_LL155;_tmp2E1=(union Cyc_Absyn_Constraint*)(_tmp2E0.Forward_constr).val;_LL15B: {
const char*_tmp11BF;void*_tmp11BE;(_tmp11BE=0,Cyc_Tcutil_impos(((_tmp11BF="unify_conref: forward after compress",
_tag_dyneither(_tmp11BF,sizeof(char),37))),_tag_dyneither(_tmp11BE,sizeof(void*),
0)));}_LL155:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2EE;
_push_handler(& _tmp2EE);{int _tmp2F0=0;if(setjmp(_tmp2EE.handler))_tmp2F0=1;if(!
_tmp2F0){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2F1=1;_npop_handler(0);return _tmp2F1;};;
_pop_handler();}else{void*_tmp2EF=(void*)_exn_thrown;void*_tmp2F3=_tmp2EF;_LL164: {
struct Cyc_Tcutil_Unify_struct*_tmp2F4=(struct Cyc_Tcutil_Unify_struct*)_tmp2F3;
if(_tmp2F4->tag != Cyc_Tcutil_Unify)goto _LL166;}_LL165: return 0;_LL166:;_LL167:(
void)_throw(_tmp2F3);_LL163:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11C0;struct _tuple0 _tmp2F6=(_tmp11C0.f1=b1,((_tmp11C0.f2=b2,
_tmp11C0)));void*_tmp2F7;void*_tmp2F9;void*_tmp2FB;void*_tmp2FD;void*_tmp2FF;
struct Cyc_Absyn_Exp*_tmp301;void*_tmp302;struct Cyc_Absyn_Exp*_tmp304;_LL169:
_tmp2F7=_tmp2F6.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2F8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2F7;if(_tmp2F8->tag != 0)goto _LL16B;};_tmp2F9=_tmp2F6.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2FA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F9;if(_tmp2FA->tag != 0)goto
_LL16B;};_LL16A: return 0;_LL16B: _tmp2FB=_tmp2F6.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2FC=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2FB;if(_tmp2FC->tag != 0)goto
_LL16D;};_LL16C: return - 1;_LL16D: _tmp2FD=_tmp2F6.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2FE=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2FD;if(_tmp2FE->tag != 0)goto
_LL16F;};_LL16E: return 1;_LL16F: _tmp2FF=_tmp2F6.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp300=(struct Cyc_Absyn_Upper_b_struct*)_tmp2FF;if(_tmp300->tag != 1)goto _LL168;
else{_tmp301=_tmp300->f1;}};_tmp302=_tmp2F6.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp303=(struct Cyc_Absyn_Upper_b_struct*)_tmp302;if(_tmp303->tag != 1)goto _LL168;
else{_tmp304=_tmp303->f1;}};_LL170: return Cyc_Evexp_const_exp_cmp(_tmp301,_tmp304);
_LL168:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0
_tmp11C1;struct _tuple0 _tmp306=(_tmp11C1.f1=b1,((_tmp11C1.f2=b2,_tmp11C1)));void*
_tmp307;void*_tmp309;void*_tmp30B;void*_tmp30D;void*_tmp30F;struct Cyc_Absyn_Exp*
_tmp311;void*_tmp312;struct Cyc_Absyn_Exp*_tmp314;_LL172: _tmp307=_tmp306.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp308=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp307;if(_tmp308->tag != 0)goto _LL174;};_tmp309=_tmp306.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp30A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp309;if(_tmp30A->tag != 0)goto
_LL174;};_LL173: return 0;_LL174: _tmp30B=_tmp306.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp30C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp30B;if(_tmp30C->tag != 0)goto
_LL176;};_LL175: return - 1;_LL176: _tmp30D=_tmp306.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp30E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp30D;if(_tmp30E->tag != 0)goto
_LL178;};_LL177: return 1;_LL178: _tmp30F=_tmp306.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp310=(struct Cyc_Absyn_Upper_b_struct*)_tmp30F;if(_tmp310->tag != 1)goto _LL171;
else{_tmp311=_tmp310->f1;}};_tmp312=_tmp306.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp313=(struct Cyc_Absyn_Upper_b_struct*)_tmp312;if(_tmp313->tag != 1)goto _LL171;
else{_tmp314=_tmp313->f1;}};_LL179: return Cyc_Evexp_const_exp_cmp(_tmp311,_tmp314);
_LL171:;}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp315=att;
_LL17B: {struct Cyc_Absyn_Regparm_att_struct*_tmp316=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp315;if(_tmp316->tag != 0)goto _LL17D;}_LL17C: return 0;_LL17D: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp317=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp315;if(_tmp317->tag != 1)goto
_LL17F;}_LL17E: return 1;_LL17F: {struct Cyc_Absyn_Cdecl_att_struct*_tmp318=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp315;if(_tmp318->tag != 2)goto _LL181;}_LL180:
return 2;_LL181: {struct Cyc_Absyn_Fastcall_att_struct*_tmp319=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp315;if(_tmp319->tag != 3)goto _LL183;}_LL182: return 3;_LL183: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp31A=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp315;if(_tmp31A->tag != 4)goto
_LL185;}_LL184: return 4;_LL185: {struct Cyc_Absyn_Const_att_struct*_tmp31B=(struct
Cyc_Absyn_Const_att_struct*)_tmp315;if(_tmp31B->tag != 5)goto _LL187;}_LL186:
return 5;_LL187: {struct Cyc_Absyn_Aligned_att_struct*_tmp31C=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp315;if(_tmp31C->tag != 6)goto _LL189;}_LL188: return 6;_LL189: {struct Cyc_Absyn_Packed_att_struct*
_tmp31D=(struct Cyc_Absyn_Packed_att_struct*)_tmp315;if(_tmp31D->tag != 7)goto
_LL18B;}_LL18A: return 7;_LL18B: {struct Cyc_Absyn_Section_att_struct*_tmp31E=(
struct Cyc_Absyn_Section_att_struct*)_tmp315;if(_tmp31E->tag != 8)goto _LL18D;}
_LL18C: return 8;_LL18D: {struct Cyc_Absyn_Nocommon_att_struct*_tmp31F=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp315;if(_tmp31F->tag != 9)goto _LL18F;}_LL18E: return 9;_LL18F: {struct Cyc_Absyn_Shared_att_struct*
_tmp320=(struct Cyc_Absyn_Shared_att_struct*)_tmp315;if(_tmp320->tag != 10)goto
_LL191;}_LL190: return 10;_LL191: {struct Cyc_Absyn_Unused_att_struct*_tmp321=(
struct Cyc_Absyn_Unused_att_struct*)_tmp315;if(_tmp321->tag != 11)goto _LL193;}
_LL192: return 11;_LL193: {struct Cyc_Absyn_Weak_att_struct*_tmp322=(struct Cyc_Absyn_Weak_att_struct*)
_tmp315;if(_tmp322->tag != 12)goto _LL195;}_LL194: return 12;_LL195: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp323=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp315;if(_tmp323->tag != 13)goto
_LL197;}_LL196: return 13;_LL197: {struct Cyc_Absyn_Dllexport_att_struct*_tmp324=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp315;if(_tmp324->tag != 14)goto _LL199;}
_LL198: return 14;_LL199: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp325=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp315;if(_tmp325->tag
!= 15)goto _LL19B;}_LL19A: return 15;_LL19B: {struct Cyc_Absyn_Constructor_att_struct*
_tmp326=(struct Cyc_Absyn_Constructor_att_struct*)_tmp315;if(_tmp326->tag != 16)
goto _LL19D;}_LL19C: return 16;_LL19D: {struct Cyc_Absyn_Destructor_att_struct*
_tmp327=(struct Cyc_Absyn_Destructor_att_struct*)_tmp315;if(_tmp327->tag != 17)
goto _LL19F;}_LL19E: return 17;_LL19F: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp328=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp315;if(_tmp328->tag
!= 18)goto _LL1A1;}_LL1A0: return 18;_LL1A1: {struct Cyc_Absyn_Format_att_struct*
_tmp329=(struct Cyc_Absyn_Format_att_struct*)_tmp315;if(_tmp329->tag != 19)goto
_LL1A3;}_LL1A2: return 19;_LL1A3: {struct Cyc_Absyn_Initializes_att_struct*_tmp32A=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp315;if(_tmp32A->tag != 20)goto _LL1A5;}
_LL1A4: return 20;_LL1A5:;_LL1A6: return 21;_LL17A:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp11C2;struct _tuple0 _tmp32C=(_tmp11C2.f1=
att1,((_tmp11C2.f2=att2,_tmp11C2)));void*_tmp32D;int _tmp32F;void*_tmp330;int
_tmp332;void*_tmp333;int _tmp335;void*_tmp336;int _tmp338;void*_tmp339;int _tmp33B;
void*_tmp33C;int _tmp33E;void*_tmp33F;struct _dyneither_ptr _tmp341;void*_tmp342;
struct _dyneither_ptr _tmp344;void*_tmp345;enum Cyc_Absyn_Format_Type _tmp347;int
_tmp348;int _tmp349;void*_tmp34A;enum Cyc_Absyn_Format_Type _tmp34C;int _tmp34D;int
_tmp34E;_LL1A8: _tmp32D=_tmp32C.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp32E=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp32D;if(_tmp32E->tag != 0)goto _LL1AA;else{
_tmp32F=_tmp32E->f1;}};_tmp330=_tmp32C.f2;{struct Cyc_Absyn_Regparm_att_struct*
_tmp331=(struct Cyc_Absyn_Regparm_att_struct*)_tmp330;if(_tmp331->tag != 0)goto
_LL1AA;else{_tmp332=_tmp331->f1;}};_LL1A9: _tmp335=_tmp32F;_tmp338=_tmp332;goto
_LL1AB;_LL1AA: _tmp333=_tmp32C.f1;{struct Cyc_Absyn_Initializes_att_struct*_tmp334=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp333;if(_tmp334->tag != 20)goto _LL1AC;
else{_tmp335=_tmp334->f1;}};_tmp336=_tmp32C.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp337=(struct Cyc_Absyn_Initializes_att_struct*)_tmp336;if(_tmp337->tag != 20)
goto _LL1AC;else{_tmp338=_tmp337->f1;}};_LL1AB: _tmp33B=_tmp335;_tmp33E=_tmp338;
goto _LL1AD;_LL1AC: _tmp339=_tmp32C.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp33A=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp339;if(_tmp33A->tag != 6)goto _LL1AE;else{
_tmp33B=_tmp33A->f1;}};_tmp33C=_tmp32C.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp33D=(struct Cyc_Absyn_Aligned_att_struct*)_tmp33C;if(_tmp33D->tag != 6)goto
_LL1AE;else{_tmp33E=_tmp33D->f1;}};_LL1AD: return Cyc_Core_intcmp(_tmp33B,_tmp33E);
_LL1AE: _tmp33F=_tmp32C.f1;{struct Cyc_Absyn_Section_att_struct*_tmp340=(struct Cyc_Absyn_Section_att_struct*)
_tmp33F;if(_tmp340->tag != 8)goto _LL1B0;else{_tmp341=_tmp340->f1;}};_tmp342=
_tmp32C.f2;{struct Cyc_Absyn_Section_att_struct*_tmp343=(struct Cyc_Absyn_Section_att_struct*)
_tmp342;if(_tmp343->tag != 8)goto _LL1B0;else{_tmp344=_tmp343->f1;}};_LL1AF: return
Cyc_strcmp((struct _dyneither_ptr)_tmp341,(struct _dyneither_ptr)_tmp344);_LL1B0:
_tmp345=_tmp32C.f1;{struct Cyc_Absyn_Format_att_struct*_tmp346=(struct Cyc_Absyn_Format_att_struct*)
_tmp345;if(_tmp346->tag != 19)goto _LL1B2;else{_tmp347=_tmp346->f1;_tmp348=_tmp346->f2;
_tmp349=_tmp346->f3;}};_tmp34A=_tmp32C.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp34B=(struct Cyc_Absyn_Format_att_struct*)_tmp34A;if(_tmp34B->tag != 19)goto
_LL1B2;else{_tmp34C=_tmp34B->f1;_tmp34D=_tmp34B->f2;_tmp34E=_tmp34B->f3;}};
_LL1B1: {int _tmp34F=Cyc_Core_intcmp((int)((unsigned int)_tmp347),(int)((
unsigned int)_tmp34C));if(_tmp34F != 0)return _tmp34F;{int _tmp350=Cyc_Core_intcmp(
_tmp348,_tmp34D);if(_tmp350 != 0)return _tmp350;return Cyc_Core_intcmp(_tmp349,
_tmp34E);};}_LL1B2:;_LL1B3: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL1A7:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct
Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return
0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp351=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp352;
enum Cyc_Absyn_KindQual _tmp353;enum Cyc_Absyn_AliasQual _tmp354;struct Cyc_Absyn_Kind
_tmp355;enum Cyc_Absyn_KindQual _tmp356;enum Cyc_Absyn_AliasQual _tmp357;struct Cyc_Absyn_Kind
_tmp358;enum Cyc_Absyn_KindQual _tmp359;struct Cyc_Absyn_Kind _tmp35A;enum Cyc_Absyn_KindQual
_tmp35B;_LL1B5: _tmp352=*_tmp351;_tmp353=_tmp352.kind;if(_tmp353 != Cyc_Absyn_RgnKind)
goto _LL1B7;_tmp354=_tmp352.aliasqual;if(_tmp354 != Cyc_Absyn_Unique)goto _LL1B7;
_LL1B6: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B4;_LL1B7: _tmp355=*_tmp351;
_tmp356=_tmp355.kind;if(_tmp356 != Cyc_Absyn_RgnKind)goto _LL1B9;_tmp357=_tmp355.aliasqual;
if(_tmp357 != Cyc_Absyn_Aliasable)goto _LL1B9;_LL1B8: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1B4;_LL1B9: _tmp358=*_tmp351;_tmp359=_tmp358.kind;if(_tmp359 != Cyc_Absyn_EffKind)
goto _LL1BB;_LL1BA: t=Cyc_Absyn_empty_effect;goto _LL1B4;_LL1BB: _tmp35A=*_tmp351;
_tmp35B=_tmp35A.kind;if(_tmp35B != Cyc_Absyn_IntKind)goto _LL1BD;_LL1BC:{struct Cyc_Absyn_ValueofType_struct
_tmp11C5;struct Cyc_Absyn_ValueofType_struct*_tmp11C4;t=(void*)((_tmp11C4=
_cycalloc(sizeof(*_tmp11C4)),((_tmp11C4[0]=((_tmp11C5.tag=19,((_tmp11C5.f1=Cyc_Absyn_uint_exp(
0,0),_tmp11C5)))),_tmp11C4))));}goto _LL1B4;_LL1BD:;_LL1BE: t=Cyc_Absyn_sint_typ;
goto _LL1B4;_LL1B4:;}{struct _tuple15*_tmp11C6;return(_tmp11C6=_cycalloc(sizeof(*
_tmp11C6)),((_tmp11C6->f1=tv,((_tmp11C6->f2=t,_tmp11C6)))));};}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp35F=Cyc_Tcutil_compress(t);void*_tmp36A;void*_tmp36C;void*
_tmp36D;struct Cyc_Absyn_DatatypeInfo _tmp36F;struct Cyc_List_List*_tmp370;struct
Cyc_Absyn_PtrInfo _tmp372;void*_tmp373;struct Cyc_Absyn_PtrAtts _tmp374;void*
_tmp375;struct Cyc_Absyn_ArrayInfo _tmp377;void*_tmp378;struct Cyc_List_List*
_tmp37A;struct Cyc_Absyn_DatatypeFieldInfo _tmp37C;struct Cyc_List_List*_tmp37D;
struct Cyc_Absyn_AggrInfo _tmp37F;struct Cyc_List_List*_tmp380;struct Cyc_List_List*
_tmp382;struct Cyc_Absyn_FnInfo _tmp385;struct Cyc_List_List*_tmp386;struct Cyc_Core_Opt*
_tmp387;struct Cyc_Absyn_Tqual _tmp388;void*_tmp389;struct Cyc_List_List*_tmp38A;
struct Cyc_Absyn_VarargInfo*_tmp38B;struct Cyc_List_List*_tmp38C;void*_tmp393;
struct Cyc_List_List*_tmp395;_LL1C0: {struct Cyc_Absyn_VoidType_struct*_tmp360=(
struct Cyc_Absyn_VoidType_struct*)_tmp35F;if(_tmp360->tag != 0)goto _LL1C2;}_LL1C1:
goto _LL1C3;_LL1C2: {struct Cyc_Absyn_FloatType_struct*_tmp361=(struct Cyc_Absyn_FloatType_struct*)
_tmp35F;if(_tmp361->tag != 7)goto _LL1C4;}_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_DoubleType_struct*
_tmp362=(struct Cyc_Absyn_DoubleType_struct*)_tmp35F;if(_tmp362->tag != 8)goto
_LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_EnumType_struct*_tmp363=(
struct Cyc_Absyn_EnumType_struct*)_tmp35F;if(_tmp363->tag != 14)goto _LL1C8;}_LL1C7:
goto _LL1C9;_LL1C8: {struct Cyc_Absyn_AnonEnumType_struct*_tmp364=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp35F;if(_tmp364->tag != 15)goto _LL1CA;}_LL1C9: goto _LL1CB;_LL1CA: {struct Cyc_Absyn_ValueofType_struct*
_tmp365=(struct Cyc_Absyn_ValueofType_struct*)_tmp35F;if(_tmp365->tag != 19)goto
_LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_IntType_struct*_tmp366=(
struct Cyc_Absyn_IntType_struct*)_tmp35F;if(_tmp366->tag != 6)goto _LL1CE;}_LL1CD:
return Cyc_Absyn_empty_effect;_LL1CE: {struct Cyc_Absyn_Evar_struct*_tmp367=(
struct Cyc_Absyn_Evar_struct*)_tmp35F;if(_tmp367->tag != 1)goto _LL1D0;}_LL1CF: goto
_LL1D1;_LL1D0: {struct Cyc_Absyn_VarType_struct*_tmp368=(struct Cyc_Absyn_VarType_struct*)
_tmp35F;if(_tmp368->tag != 2)goto _LL1D2;}_LL1D1: {struct Cyc_Absyn_Kind*_tmp397=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp398;enum Cyc_Absyn_KindQual
_tmp399;struct Cyc_Absyn_Kind _tmp39A;enum Cyc_Absyn_KindQual _tmp39B;struct Cyc_Absyn_Kind
_tmp39C;enum Cyc_Absyn_KindQual _tmp39D;_LL1F9: _tmp398=*_tmp397;_tmp399=_tmp398.kind;
if(_tmp399 != Cyc_Absyn_RgnKind)goto _LL1FB;_LL1FA: {struct Cyc_Absyn_AccessEff_struct
_tmp11C9;struct Cyc_Absyn_AccessEff_struct*_tmp11C8;return(void*)((_tmp11C8=
_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8[0]=((_tmp11C9.tag=24,((_tmp11C9.f1=(void*)
t,_tmp11C9)))),_tmp11C8))));}_LL1FB: _tmp39A=*_tmp397;_tmp39B=_tmp39A.kind;if(
_tmp39B != Cyc_Absyn_EffKind)goto _LL1FD;_LL1FC: return t;_LL1FD: _tmp39C=*_tmp397;
_tmp39D=_tmp39C.kind;if(_tmp39D != Cyc_Absyn_IntKind)goto _LL1FF;_LL1FE: return Cyc_Absyn_empty_effect;
_LL1FF:;_LL200: {struct Cyc_Absyn_RgnsEff_struct _tmp11CC;struct Cyc_Absyn_RgnsEff_struct*
_tmp11CB;return(void*)((_tmp11CB=_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB[0]=((
_tmp11CC.tag=26,((_tmp11CC.f1=(void*)t,_tmp11CC)))),_tmp11CB))));}_LL1F8:;}
_LL1D2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp369=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp35F;if(_tmp369->tag != 16)goto _LL1D4;else{_tmp36A=(void*)_tmp369->f1;}}_LL1D3: {
struct Cyc_Absyn_AccessEff_struct _tmp11CF;struct Cyc_Absyn_AccessEff_struct*
_tmp11CE;return(void*)((_tmp11CE=_cycalloc(sizeof(*_tmp11CE)),((_tmp11CE[0]=((
_tmp11CF.tag=24,((_tmp11CF.f1=(void*)_tmp36A,_tmp11CF)))),_tmp11CE))));}_LL1D4: {
struct Cyc_Absyn_DynRgnType_struct*_tmp36B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp35F;if(_tmp36B->tag != 17)goto _LL1D6;else{_tmp36C=(void*)_tmp36B->f1;_tmp36D=(
void*)_tmp36B->f2;}}_LL1D5: {struct Cyc_Absyn_AccessEff_struct _tmp11D2;struct Cyc_Absyn_AccessEff_struct*
_tmp11D1;return(void*)((_tmp11D1=_cycalloc(sizeof(*_tmp11D1)),((_tmp11D1[0]=((
_tmp11D2.tag=24,((_tmp11D2.f1=(void*)_tmp36D,_tmp11D2)))),_tmp11D1))));}_LL1D6: {
struct Cyc_Absyn_DatatypeType_struct*_tmp36E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp35F;if(_tmp36E->tag != 3)goto _LL1D8;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.targs;}}
_LL1D7: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp370);struct Cyc_Absyn_JoinEff_struct
_tmp11D5;struct Cyc_Absyn_JoinEff_struct*_tmp11D4;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4[0]=((_tmp11D5.tag=25,((
_tmp11D5.f1=ts,_tmp11D5)))),_tmp11D4)))));}_LL1D8: {struct Cyc_Absyn_PointerType_struct*
_tmp371=(struct Cyc_Absyn_PointerType_struct*)_tmp35F;if(_tmp371->tag != 5)goto
_LL1DA;else{_tmp372=_tmp371->f1;_tmp373=_tmp372.elt_typ;_tmp374=_tmp372.ptr_atts;
_tmp375=_tmp374.rgn;}}_LL1D9: {struct Cyc_Absyn_JoinEff_struct _tmp11E4;struct Cyc_Absyn_AccessEff_struct*
_tmp11E3;struct Cyc_Absyn_AccessEff_struct _tmp11E2;void*_tmp11E1[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11E0;return Cyc_Tcutil_normalize_effect((void*)((_tmp11E0=_cycalloc(sizeof(*
_tmp11E0)),((_tmp11E0[0]=((_tmp11E4.tag=25,((_tmp11E4.f1=((_tmp11E1[1]=Cyc_Tcutil_rgns_of(
_tmp373),((_tmp11E1[0]=(void*)((_tmp11E3=_cycalloc(sizeof(*_tmp11E3)),((_tmp11E3[
0]=((_tmp11E2.tag=24,((_tmp11E2.f1=(void*)_tmp375,_tmp11E2)))),_tmp11E3)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11E1,sizeof(void*),2)))))),_tmp11E4)))),_tmp11E0)))));}_LL1DA: {struct Cyc_Absyn_ArrayType_struct*
_tmp376=(struct Cyc_Absyn_ArrayType_struct*)_tmp35F;if(_tmp376->tag != 9)goto
_LL1DC;else{_tmp377=_tmp376->f1;_tmp378=_tmp377.elt_type;}}_LL1DB: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp378));_LL1DC: {struct Cyc_Absyn_TupleType_struct*_tmp379=(
struct Cyc_Absyn_TupleType_struct*)_tmp35F;if(_tmp379->tag != 11)goto _LL1DE;else{
_tmp37A=_tmp379->f1;}}_LL1DD: {struct Cyc_List_List*_tmp3AD=0;for(0;_tmp37A != 0;
_tmp37A=_tmp37A->tl){struct Cyc_List_List*_tmp11E5;_tmp3AD=((_tmp11E5=_cycalloc(
sizeof(*_tmp11E5)),((_tmp11E5->hd=(*((struct _tuple11*)_tmp37A->hd)).f2,((
_tmp11E5->tl=_tmp3AD,_tmp11E5))))));}_tmp37D=_tmp3AD;goto _LL1DF;}_LL1DE: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp37B=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp35F;if(_tmp37B->tag != 4)goto _LL1E0;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37C.targs;}}
_LL1DF: _tmp380=_tmp37D;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_AggrType_struct*
_tmp37E=(struct Cyc_Absyn_AggrType_struct*)_tmp35F;if(_tmp37E->tag != 12)goto
_LL1E2;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37F.targs;}}_LL1E1: {struct Cyc_Absyn_JoinEff_struct
_tmp11E8;struct Cyc_Absyn_JoinEff_struct*_tmp11E7;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11E7=_cycalloc(sizeof(*_tmp11E7)),((_tmp11E7[0]=((_tmp11E8.tag=25,((
_tmp11E8.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp380),_tmp11E8)))),_tmp11E7)))));}_LL1E2: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp381=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35F;if(_tmp381->tag != 13)goto _LL1E4;else{_tmp382=_tmp381->f2;}}_LL1E3: {
struct Cyc_Absyn_JoinEff_struct _tmp11EB;struct Cyc_Absyn_JoinEff_struct*_tmp11EA;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11EA=_cycalloc(sizeof(*_tmp11EA)),((
_tmp11EA[0]=((_tmp11EB.tag=25,((_tmp11EB.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp382),_tmp11EB)))),_tmp11EA)))));}_LL1E4: {struct Cyc_Absyn_TagType_struct*
_tmp383=(struct Cyc_Absyn_TagType_struct*)_tmp35F;if(_tmp383->tag != 20)goto _LL1E6;}
_LL1E5: return Cyc_Absyn_empty_effect;_LL1E6: {struct Cyc_Absyn_FnType_struct*
_tmp384=(struct Cyc_Absyn_FnType_struct*)_tmp35F;if(_tmp384->tag != 10)goto _LL1E8;
else{_tmp385=_tmp384->f1;_tmp386=_tmp385.tvars;_tmp387=_tmp385.effect;_tmp388=
_tmp385.ret_tqual;_tmp389=_tmp385.ret_typ;_tmp38A=_tmp385.args;_tmp38B=_tmp385.cyc_varargs;
_tmp38C=_tmp385.rgn_po;}}_LL1E7: {void*_tmp3B3=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_region_free_subst,_tmp386),(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp387))->v);return Cyc_Tcutil_normalize_effect(_tmp3B3);}_LL1E8: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp38D=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp35F;if(_tmp38D->tag != 22)goto
_LL1EA;}_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_RefCntRgn_struct*_tmp38E=(
struct Cyc_Absyn_RefCntRgn_struct*)_tmp35F;if(_tmp38E->tag != 23)goto _LL1EC;}
_LL1EB: goto _LL1ED;_LL1EC: {struct Cyc_Absyn_HeapRgn_struct*_tmp38F=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp35F;if(_tmp38F->tag != 21)goto _LL1EE;}_LL1ED: return Cyc_Absyn_empty_effect;
_LL1EE: {struct Cyc_Absyn_AccessEff_struct*_tmp390=(struct Cyc_Absyn_AccessEff_struct*)
_tmp35F;if(_tmp390->tag != 24)goto _LL1F0;}_LL1EF: goto _LL1F1;_LL1F0: {struct Cyc_Absyn_JoinEff_struct*
_tmp391=(struct Cyc_Absyn_JoinEff_struct*)_tmp35F;if(_tmp391->tag != 25)goto _LL1F2;}
_LL1F1: return t;_LL1F2: {struct Cyc_Absyn_RgnsEff_struct*_tmp392=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp35F;if(_tmp392->tag != 26)goto _LL1F4;else{_tmp393=(void*)_tmp392->f1;}}_LL1F3:
return Cyc_Tcutil_rgns_of(_tmp393);_LL1F4: {struct Cyc_Absyn_TypedefType_struct*
_tmp394=(struct Cyc_Absyn_TypedefType_struct*)_tmp35F;if(_tmp394->tag != 18)goto
_LL1F6;else{_tmp395=_tmp394->f2;}}_LL1F5: {struct Cyc_Absyn_JoinEff_struct
_tmp11EE;struct Cyc_Absyn_JoinEff_struct*_tmp11ED;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((_tmp11ED[0]=((_tmp11EE.tag=25,((
_tmp11EE.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp395),_tmp11EE)))),_tmp11ED)))));}_LL1F6: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp396=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp35F;if(_tmp396->tag != 27)goto _LL1BF;}_LL1F7: {const char*_tmp11F1;void*
_tmp11F0;(_tmp11F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11F1="typedecl in rgns_of",_tag_dyneither(_tmp11F1,
sizeof(char),20))),_tag_dyneither(_tmp11F0,sizeof(void*),0)));}_LL1BF:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp3B8=e;struct Cyc_List_List*_tmp3BA;
struct Cyc_List_List**_tmp3BB;void*_tmp3BD;_LL202: {struct Cyc_Absyn_JoinEff_struct*
_tmp3B9=(struct Cyc_Absyn_JoinEff_struct*)_tmp3B8;if(_tmp3B9->tag != 25)goto _LL204;
else{_tmp3BA=_tmp3B9->f1;_tmp3BB=(struct Cyc_List_List**)& _tmp3B9->f1;}}_LL203: {
int redo_join=0;{struct Cyc_List_List*effs=*_tmp3BB;for(0;effs != 0;effs=effs->tl){
void*_tmp3BE=(void*)effs->hd;effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp3BE));{void*_tmp3BF=(void*)effs->hd;void*_tmp3C2;void*_tmp3C5;void*_tmp3C8;
_LL209: {struct Cyc_Absyn_JoinEff_struct*_tmp3C0=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3BF;if(_tmp3C0->tag != 25)goto _LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_AccessEff_struct*
_tmp3C1=(struct Cyc_Absyn_AccessEff_struct*)_tmp3BF;if(_tmp3C1->tag != 24)goto
_LL20D;else{_tmp3C2=(void*)_tmp3C1->f1;{struct Cyc_Absyn_HeapRgn_struct*_tmp3C3=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp3C2;if(_tmp3C3->tag != 21)goto _LL20D;};}}
_LL20C: redo_join=1;goto _LL208;_LL20D: {struct Cyc_Absyn_AccessEff_struct*_tmp3C4=(
struct Cyc_Absyn_AccessEff_struct*)_tmp3BF;if(_tmp3C4->tag != 24)goto _LL20F;else{
_tmp3C5=(void*)_tmp3C4->f1;{struct Cyc_Absyn_RefCntRgn_struct*_tmp3C6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp3C5;if(_tmp3C6->tag != 23)goto _LL20F;};}}_LL20E: redo_join=1;goto _LL208;_LL20F: {
struct Cyc_Absyn_AccessEff_struct*_tmp3C7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3BF;if(_tmp3C7->tag != 24)goto _LL211;else{_tmp3C8=(void*)_tmp3C7->f1;{struct
Cyc_Absyn_UniqueRgn_struct*_tmp3C9=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp3C8;
if(_tmp3C9->tag != 22)goto _LL211;};}}_LL210: redo_join=1;goto _LL208;_LL211:;_LL212:
goto _LL208;_LL208:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp3BB;for(0;effs != 0;effs=effs->tl){void*_tmp3CA=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp3CC;void*_tmp3CE;void*_tmp3D1;void*
_tmp3D4;_LL214: {struct Cyc_Absyn_JoinEff_struct*_tmp3CB=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3CA;if(_tmp3CB->tag != 25)goto _LL216;else{_tmp3CC=_tmp3CB->f1;}}_LL215:
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_revappend)(_tmp3CC,effects);goto _LL213;_LL216: {struct Cyc_Absyn_AccessEff_struct*
_tmp3CD=(struct Cyc_Absyn_AccessEff_struct*)_tmp3CA;if(_tmp3CD->tag != 24)goto
_LL218;else{_tmp3CE=(void*)_tmp3CD->f1;{struct Cyc_Absyn_HeapRgn_struct*_tmp3CF=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp3CE;if(_tmp3CF->tag != 21)goto _LL218;};}}
_LL217: goto _LL213;_LL218: {struct Cyc_Absyn_AccessEff_struct*_tmp3D0=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3CA;if(_tmp3D0->tag != 24)goto _LL21A;else{_tmp3D1=(void*)_tmp3D0->f1;{struct
Cyc_Absyn_RefCntRgn_struct*_tmp3D2=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp3D1;
if(_tmp3D2->tag != 23)goto _LL21A;};}}_LL219: goto _LL213;_LL21A: {struct Cyc_Absyn_AccessEff_struct*
_tmp3D3=(struct Cyc_Absyn_AccessEff_struct*)_tmp3CA;if(_tmp3D3->tag != 24)goto
_LL21C;else{_tmp3D4=(void*)_tmp3D3->f1;{struct Cyc_Absyn_UniqueRgn_struct*_tmp3D5=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp3D4;if(_tmp3D5->tag != 22)goto _LL21C;};}}
_LL21B: goto _LL213;_LL21C:;_LL21D:{struct Cyc_List_List*_tmp11F2;effects=((
_tmp11F2=_cycalloc(sizeof(*_tmp11F2)),((_tmp11F2->hd=_tmp3CA,((_tmp11F2->tl=
effects,_tmp11F2))))));}goto _LL213;_LL213:;}}*_tmp3BB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL204: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3BC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3B8;if(_tmp3BC->tag != 26)goto _LL206;
else{_tmp3BD=(void*)_tmp3BC->f1;}}_LL205: {void*_tmp3D7=Cyc_Tcutil_compress(
_tmp3BD);_LL21F: {struct Cyc_Absyn_Evar_struct*_tmp3D8=(struct Cyc_Absyn_Evar_struct*)
_tmp3D7;if(_tmp3D8->tag != 1)goto _LL221;}_LL220: goto _LL222;_LL221: {struct Cyc_Absyn_VarType_struct*
_tmp3D9=(struct Cyc_Absyn_VarType_struct*)_tmp3D7;if(_tmp3D9->tag != 2)goto _LL223;}
_LL222: return e;_LL223:;_LL224: return Cyc_Tcutil_rgns_of(_tmp3BD);_LL21E:;}_LL206:;
_LL207: return e;_LL201:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp11FC;struct Cyc_Core_Opt*_tmp11FB;struct Cyc_Absyn_FnInfo _tmp11FA;struct Cyc_Absyn_FnType_struct*
_tmp11F9;struct Cyc_Absyn_FnType_struct*_tmp3DA=(_tmp11F9=_cycalloc(sizeof(*
_tmp11F9)),((_tmp11F9[0]=((_tmp11FC.tag=10,((_tmp11FC.f1=((_tmp11FA.tvars=0,((
_tmp11FA.effect=((_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB->v=eff,
_tmp11FB)))),((_tmp11FA.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp11FA.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmp11FA.args=0,((_tmp11FA.c_varargs=0,((
_tmp11FA.cyc_varargs=0,((_tmp11FA.rgn_po=0,((_tmp11FA.attributes=0,_tmp11FA)))))))))))))))))),
_tmp11FC)))),_tmp11F9)));return Cyc_Absyn_atb_typ((void*)_tmp3DA,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{void*_tmp3DF=Cyc_Tcutil_compress(e);void*_tmp3E1;struct Cyc_List_List*
_tmp3E3;void*_tmp3E5;struct Cyc_Core_Opt*_tmp3E7;struct Cyc_Core_Opt*_tmp3E8;
struct Cyc_Core_Opt**_tmp3E9;struct Cyc_Core_Opt*_tmp3EA;_LL226: {struct Cyc_Absyn_AccessEff_struct*
_tmp3E0=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DF;if(_tmp3E0->tag != 24)goto
_LL228;else{_tmp3E1=(void*)_tmp3E0->f1;}}_LL227: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3E1);_tmp3E1=Cyc_Tcutil_compress(_tmp3E1);if(r == _tmp3E1)return 1;{struct
_tuple0 _tmp11FD;struct _tuple0 _tmp3EC=(_tmp11FD.f1=r,((_tmp11FD.f2=_tmp3E1,
_tmp11FD)));void*_tmp3ED;struct Cyc_Absyn_Tvar*_tmp3EF;void*_tmp3F0;struct Cyc_Absyn_Tvar*
_tmp3F2;_LL231: _tmp3ED=_tmp3EC.f1;{struct Cyc_Absyn_VarType_struct*_tmp3EE=(
struct Cyc_Absyn_VarType_struct*)_tmp3ED;if(_tmp3EE->tag != 2)goto _LL233;else{
_tmp3EF=_tmp3EE->f1;}};_tmp3F0=_tmp3EC.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3F1=(struct Cyc_Absyn_VarType_struct*)_tmp3F0;if(_tmp3F1->tag != 2)goto _LL233;
else{_tmp3F2=_tmp3F1->f1;}};_LL232: return Cyc_Absyn_tvar_cmp(_tmp3EF,_tmp3F2)== 0;
_LL233:;_LL234: return 0;_LL230:;};_LL228: {struct Cyc_Absyn_JoinEff_struct*_tmp3E2=(
struct Cyc_Absyn_JoinEff_struct*)_tmp3DF;if(_tmp3E2->tag != 25)goto _LL22A;else{
_tmp3E3=_tmp3E2->f1;}}_LL229: for(0;_tmp3E3 != 0;_tmp3E3=_tmp3E3->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp3E3->hd))return 1;}return 0;_LL22A: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3E4=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3DF;if(_tmp3E4->tag != 26)goto _LL22C;
else{_tmp3E5=(void*)_tmp3E4->f1;}}_LL22B: {void*_tmp3F3=Cyc_Tcutil_rgns_of(
_tmp3E5);void*_tmp3F5;_LL236: {struct Cyc_Absyn_RgnsEff_struct*_tmp3F4=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3F3;if(_tmp3F4->tag != 26)goto _LL238;else{_tmp3F5=(void*)_tmp3F4->f1;}}_LL237:
if(!constrain)return 0;{void*_tmp3F6=Cyc_Tcutil_compress(_tmp3F5);struct Cyc_Core_Opt*
_tmp3F8;struct Cyc_Core_Opt*_tmp3F9;struct Cyc_Core_Opt**_tmp3FA;struct Cyc_Core_Opt*
_tmp3FB;_LL23B: {struct Cyc_Absyn_Evar_struct*_tmp3F7=(struct Cyc_Absyn_Evar_struct*)
_tmp3F6;if(_tmp3F7->tag != 1)goto _LL23D;else{_tmp3F8=_tmp3F7->f1;_tmp3F9=_tmp3F7->f2;
_tmp3FA=(struct Cyc_Core_Opt**)& _tmp3F7->f2;_tmp3FB=_tmp3F7->f4;}}_LL23C: {void*
_tmp3FC=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3FB);Cyc_Tcutil_occurs(
_tmp3FC,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3FB))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp120C;struct Cyc_Absyn_AccessEff_struct*
_tmp120B;struct Cyc_Absyn_AccessEff_struct _tmp120A;void*_tmp1209[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1208;void*_tmp3FD=Cyc_Tcutil_dummy_fntype((void*)((_tmp1208=_cycalloc(
sizeof(*_tmp1208)),((_tmp1208[0]=((_tmp120C.tag=25,((_tmp120C.f1=((_tmp1209[1]=(
void*)((_tmp120B=_cycalloc(sizeof(*_tmp120B)),((_tmp120B[0]=((_tmp120A.tag=24,((
_tmp120A.f1=(void*)r,_tmp120A)))),_tmp120B)))),((_tmp1209[0]=_tmp3FC,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1209,sizeof(void*),2)))))),
_tmp120C)))),_tmp1208)))));{struct Cyc_Core_Opt*_tmp120D;*_tmp3FA=((_tmp120D=
_cycalloc(sizeof(*_tmp120D)),((_tmp120D->v=_tmp3FD,_tmp120D))));}return 1;};}
_LL23D:;_LL23E: return 0;_LL23A:;};_LL238:;_LL239: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3F3);_LL235:;}_LL22C: {struct Cyc_Absyn_Evar_struct*_tmp3E6=(
struct Cyc_Absyn_Evar_struct*)_tmp3DF;if(_tmp3E6->tag != 1)goto _LL22E;else{_tmp3E7=
_tmp3E6->f1;_tmp3E8=_tmp3E6->f2;_tmp3E9=(struct Cyc_Core_Opt**)& _tmp3E6->f2;
_tmp3EA=_tmp3E6->f4;}}_LL22D: if(_tmp3E7 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E7->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp1210;void*_tmp120F;(_tmp120F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1210="effect evar has wrong kind",
_tag_dyneither(_tmp1210,sizeof(char),27))),_tag_dyneither(_tmp120F,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp406=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3EA);Cyc_Tcutil_occurs(_tmp406,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3EA))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp1225;struct Cyc_List_List*_tmp1224;struct Cyc_Absyn_AccessEff_struct _tmp1223;
struct Cyc_Absyn_AccessEff_struct*_tmp1222;struct Cyc_List_List*_tmp1221;struct Cyc_Absyn_JoinEff_struct*
_tmp1220;struct Cyc_Absyn_JoinEff_struct*_tmp407=(_tmp1220=_cycalloc(sizeof(*
_tmp1220)),((_tmp1220[0]=((_tmp1225.tag=25,((_tmp1225.f1=((_tmp1221=_cycalloc(
sizeof(*_tmp1221)),((_tmp1221->hd=_tmp406,((_tmp1221->tl=((_tmp1224=_cycalloc(
sizeof(*_tmp1224)),((_tmp1224->hd=(void*)((_tmp1222=_cycalloc(sizeof(*_tmp1222)),((
_tmp1222[0]=((_tmp1223.tag=24,((_tmp1223.f1=(void*)r,_tmp1223)))),_tmp1222)))),((
_tmp1224->tl=0,_tmp1224)))))),_tmp1221)))))),_tmp1225)))),_tmp1220)));{struct Cyc_Core_Opt*
_tmp1226;*_tmp3E9=((_tmp1226=_cycalloc(sizeof(*_tmp1226)),((_tmp1226->v=(void*)
_tmp407,_tmp1226))));}return 1;};};_LL22E:;_LL22F: return 0;_LL225:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp40F=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp412;void*_tmp414;struct Cyc_Core_Opt*
_tmp416;struct Cyc_Core_Opt*_tmp417;struct Cyc_Core_Opt**_tmp418;struct Cyc_Core_Opt*
_tmp419;_LL240: {struct Cyc_Absyn_AccessEff_struct*_tmp410=(struct Cyc_Absyn_AccessEff_struct*)
_tmp40F;if(_tmp410->tag != 24)goto _LL242;}_LL241: return 0;_LL242: {struct Cyc_Absyn_JoinEff_struct*
_tmp411=(struct Cyc_Absyn_JoinEff_struct*)_tmp40F;if(_tmp411->tag != 25)goto _LL244;
else{_tmp412=_tmp411->f1;}}_LL243: for(0;_tmp412 != 0;_tmp412=_tmp412->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp412->hd))return 1;}return 0;_LL244: {struct Cyc_Absyn_RgnsEff_struct*
_tmp413=(struct Cyc_Absyn_RgnsEff_struct*)_tmp40F;if(_tmp413->tag != 26)goto _LL246;
else{_tmp414=(void*)_tmp413->f1;}}_LL245: _tmp414=Cyc_Tcutil_compress(_tmp414);
if(t == _tmp414)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp414);{
void*_tmp41A=Cyc_Tcutil_rgns_of(t);void*_tmp41C;_LL24B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp41B=(struct Cyc_Absyn_RgnsEff_struct*)_tmp41A;if(_tmp41B->tag != 26)goto _LL24D;
else{_tmp41C=(void*)_tmp41B->f1;}}_LL24C: {struct _tuple0 _tmp1227;struct _tuple0
_tmp41E=(_tmp1227.f1=Cyc_Tcutil_compress(_tmp41C),((_tmp1227.f2=_tmp414,_tmp1227)));
void*_tmp41F;struct Cyc_Absyn_Tvar*_tmp421;void*_tmp422;struct Cyc_Absyn_Tvar*
_tmp424;_LL250: _tmp41F=_tmp41E.f1;{struct Cyc_Absyn_VarType_struct*_tmp420=(
struct Cyc_Absyn_VarType_struct*)_tmp41F;if(_tmp420->tag != 2)goto _LL252;else{
_tmp421=_tmp420->f1;}};_tmp422=_tmp41E.f2;{struct Cyc_Absyn_VarType_struct*
_tmp423=(struct Cyc_Absyn_VarType_struct*)_tmp422;if(_tmp423->tag != 2)goto _LL252;
else{_tmp424=_tmp423->f1;}};_LL251: return Cyc_Tcutil_unify(t,_tmp414);_LL252:;
_LL253: return _tmp41C == _tmp414;_LL24F:;}_LL24D:;_LL24E: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp41A);_LL24A:;};_LL246: {struct Cyc_Absyn_Evar_struct*
_tmp415=(struct Cyc_Absyn_Evar_struct*)_tmp40F;if(_tmp415->tag != 1)goto _LL248;
else{_tmp416=_tmp415->f1;_tmp417=_tmp415->f2;_tmp418=(struct Cyc_Core_Opt**)&
_tmp415->f2;_tmp419=_tmp415->f4;}}_LL247: if(_tmp416 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp416->v)->kind != Cyc_Absyn_EffKind){const char*_tmp122A;void*_tmp1229;(
_tmp1229=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp122A="effect evar has wrong kind",_tag_dyneither(_tmp122A,sizeof(char),27))),
_tag_dyneither(_tmp1229,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp427=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp419);
Cyc_Tcutil_occurs(_tmp427,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp419))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp123F;struct Cyc_List_List*
_tmp123E;struct Cyc_Absyn_RgnsEff_struct _tmp123D;struct Cyc_Absyn_RgnsEff_struct*
_tmp123C;struct Cyc_List_List*_tmp123B;struct Cyc_Absyn_JoinEff_struct*_tmp123A;
struct Cyc_Absyn_JoinEff_struct*_tmp428=(_tmp123A=_cycalloc(sizeof(*_tmp123A)),((
_tmp123A[0]=((_tmp123F.tag=25,((_tmp123F.f1=((_tmp123B=_cycalloc(sizeof(*
_tmp123B)),((_tmp123B->hd=_tmp427,((_tmp123B->tl=((_tmp123E=_cycalloc(sizeof(*
_tmp123E)),((_tmp123E->hd=(void*)((_tmp123C=_cycalloc(sizeof(*_tmp123C)),((
_tmp123C[0]=((_tmp123D.tag=26,((_tmp123D.f1=(void*)t,_tmp123D)))),_tmp123C)))),((
_tmp123E->tl=0,_tmp123E)))))),_tmp123B)))))),_tmp123F)))),_tmp123A)));{struct Cyc_Core_Opt*
_tmp1240;*_tmp418=((_tmp1240=_cycalloc(sizeof(*_tmp1240)),((_tmp1240->v=(void*)
_tmp428,_tmp1240))));}return 1;};};_LL248:;_LL249: return 0;_LL23F:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp430=e;struct Cyc_Absyn_Tvar*_tmp432;struct Cyc_List_List*_tmp434;void*
_tmp436;struct Cyc_Core_Opt*_tmp438;struct Cyc_Core_Opt*_tmp439;struct Cyc_Core_Opt**
_tmp43A;struct Cyc_Core_Opt*_tmp43B;_LL255: {struct Cyc_Absyn_VarType_struct*
_tmp431=(struct Cyc_Absyn_VarType_struct*)_tmp430;if(_tmp431->tag != 2)goto _LL257;
else{_tmp432=_tmp431->f1;}}_LL256: return Cyc_Absyn_tvar_cmp(v,_tmp432)== 0;_LL257: {
struct Cyc_Absyn_JoinEff_struct*_tmp433=(struct Cyc_Absyn_JoinEff_struct*)_tmp430;
if(_tmp433->tag != 25)goto _LL259;else{_tmp434=_tmp433->f1;}}_LL258: for(0;_tmp434
!= 0;_tmp434=_tmp434->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp434->hd))return 1;}return 0;_LL259: {struct Cyc_Absyn_RgnsEff_struct*
_tmp435=(struct Cyc_Absyn_RgnsEff_struct*)_tmp430;if(_tmp435->tag != 26)goto _LL25B;
else{_tmp436=(void*)_tmp435->f1;}}_LL25A: {void*_tmp43C=Cyc_Tcutil_rgns_of(
_tmp436);void*_tmp43E;_LL260: {struct Cyc_Absyn_RgnsEff_struct*_tmp43D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp43C;if(_tmp43D->tag != 26)goto _LL262;else{_tmp43E=(void*)_tmp43D->f1;}}_LL261:
if(!may_constrain_evars)return 0;{void*_tmp43F=Cyc_Tcutil_compress(_tmp43E);
struct Cyc_Core_Opt*_tmp441;struct Cyc_Core_Opt*_tmp442;struct Cyc_Core_Opt**
_tmp443;struct Cyc_Core_Opt*_tmp444;_LL265: {struct Cyc_Absyn_Evar_struct*_tmp440=(
struct Cyc_Absyn_Evar_struct*)_tmp43F;if(_tmp440->tag != 1)goto _LL267;else{_tmp441=
_tmp440->f1;_tmp442=_tmp440->f2;_tmp443=(struct Cyc_Core_Opt**)& _tmp440->f2;
_tmp444=_tmp440->f4;}}_LL266: {void*_tmp445=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp444);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp444))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp124F;struct Cyc_Absyn_VarType_struct*_tmp124E;
struct Cyc_Absyn_VarType_struct _tmp124D;void*_tmp124C[2];struct Cyc_Absyn_JoinEff_struct*
_tmp124B;void*_tmp446=Cyc_Tcutil_dummy_fntype((void*)((_tmp124B=_cycalloc(
sizeof(*_tmp124B)),((_tmp124B[0]=((_tmp124F.tag=25,((_tmp124F.f1=((_tmp124C[1]=(
void*)((_tmp124E=_cycalloc(sizeof(*_tmp124E)),((_tmp124E[0]=((_tmp124D.tag=2,((
_tmp124D.f1=v,_tmp124D)))),_tmp124E)))),((_tmp124C[0]=_tmp445,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp124C,sizeof(void*),2)))))),
_tmp124F)))),_tmp124B)))));{struct Cyc_Core_Opt*_tmp1250;*_tmp443=((_tmp1250=
_cycalloc(sizeof(*_tmp1250)),((_tmp1250->v=_tmp446,_tmp1250))));}return 1;};}
_LL267:;_LL268: return 0;_LL264:;};_LL262:;_LL263: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp43C);_LL25F:;}_LL25B: {struct Cyc_Absyn_Evar_struct*
_tmp437=(struct Cyc_Absyn_Evar_struct*)_tmp430;if(_tmp437->tag != 1)goto _LL25D;
else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;_tmp43A=(struct Cyc_Core_Opt**)&
_tmp437->f2;_tmp43B=_tmp437->f4;}}_LL25C: if(_tmp438 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp438->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1253;void*_tmp1252;(
_tmp1252=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1253="effect evar has wrong kind",_tag_dyneither(_tmp1253,sizeof(char),27))),
_tag_dyneither(_tmp1252,sizeof(void*),0)));}{void*_tmp44F=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp43B);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp43B))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp1268;
struct Cyc_List_List*_tmp1267;struct Cyc_Absyn_VarType_struct _tmp1266;struct Cyc_Absyn_VarType_struct*
_tmp1265;struct Cyc_List_List*_tmp1264;struct Cyc_Absyn_JoinEff_struct*_tmp1263;
struct Cyc_Absyn_JoinEff_struct*_tmp450=(_tmp1263=_cycalloc(sizeof(*_tmp1263)),((
_tmp1263[0]=((_tmp1268.tag=25,((_tmp1268.f1=((_tmp1264=_cycalloc(sizeof(*
_tmp1264)),((_tmp1264->hd=_tmp44F,((_tmp1264->tl=((_tmp1267=_cycalloc(sizeof(*
_tmp1267)),((_tmp1267->hd=(void*)((_tmp1265=_cycalloc(sizeof(*_tmp1265)),((
_tmp1265[0]=((_tmp1266.tag=2,((_tmp1266.f1=v,_tmp1266)))),_tmp1265)))),((
_tmp1267->tl=0,_tmp1267)))))),_tmp1264)))))),_tmp1268)))),_tmp1263)));{struct Cyc_Core_Opt*
_tmp1269;*_tmp43A=((_tmp1269=_cycalloc(sizeof(*_tmp1269)),((_tmp1269->v=(void*)
_tmp450,_tmp1269))));}return 1;};};_LL25D:;_LL25E: return 0;_LL254:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp458=e;struct Cyc_List_List*
_tmp45A;void*_tmp45C;_LL26A: {struct Cyc_Absyn_JoinEff_struct*_tmp459=(struct Cyc_Absyn_JoinEff_struct*)
_tmp458;if(_tmp459->tag != 25)goto _LL26C;else{_tmp45A=_tmp459->f1;}}_LL26B: for(0;
_tmp45A != 0;_tmp45A=_tmp45A->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp45A->hd))return 1;}return 0;_LL26C: {struct Cyc_Absyn_RgnsEff_struct*_tmp45B=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp458;if(_tmp45B->tag != 26)goto _LL26E;else{
_tmp45C=(void*)_tmp45B->f1;}}_LL26D: {void*_tmp45E=Cyc_Tcutil_rgns_of(_tmp45C);
void*_tmp460;_LL273: {struct Cyc_Absyn_RgnsEff_struct*_tmp45F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp45E;if(_tmp45F->tag != 26)goto _LL275;else{_tmp460=(void*)_tmp45F->f1;}}_LL274:
return 0;_LL275:;_LL276: return Cyc_Tcutil_evar_in_effect(evar,_tmp45E);_LL272:;}
_LL26E: {struct Cyc_Absyn_Evar_struct*_tmp45D=(struct Cyc_Absyn_Evar_struct*)
_tmp458;if(_tmp45D->tag != 1)goto _LL270;}_LL26F: return evar == e;_LL270:;_LL271:
return 0;_LL269:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2){void*_tmp461=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp463;void*
_tmp465;struct Cyc_Absyn_Tvar*_tmp467;void*_tmp469;struct Cyc_Core_Opt*_tmp46B;
struct Cyc_Core_Opt**_tmp46C;struct Cyc_Core_Opt*_tmp46D;_LL278: {struct Cyc_Absyn_JoinEff_struct*
_tmp462=(struct Cyc_Absyn_JoinEff_struct*)_tmp461;if(_tmp462->tag != 25)goto _LL27A;
else{_tmp463=_tmp462->f1;}}_LL279: for(0;_tmp463 != 0;_tmp463=_tmp463->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp463->hd,e2))return 0;}return 1;_LL27A: {struct Cyc_Absyn_AccessEff_struct*
_tmp464=(struct Cyc_Absyn_AccessEff_struct*)_tmp461;if(_tmp464->tag != 24)goto
_LL27C;else{_tmp465=(void*)_tmp464->f1;}}_LL27B: return Cyc_Tcutil_region_in_effect(
0,_tmp465,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp465,(void*)& Cyc_Absyn_HeapRgn_val);
_LL27C: {struct Cyc_Absyn_VarType_struct*_tmp466=(struct Cyc_Absyn_VarType_struct*)
_tmp461;if(_tmp466->tag != 2)goto _LL27E;else{_tmp467=_tmp466->f1;}}_LL27D: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp467,e2);_LL27E: {struct Cyc_Absyn_RgnsEff_struct*
_tmp468=(struct Cyc_Absyn_RgnsEff_struct*)_tmp461;if(_tmp468->tag != 26)goto _LL280;
else{_tmp469=(void*)_tmp468->f1;}}_LL27F: {void*_tmp46E=Cyc_Tcutil_rgns_of(
_tmp469);void*_tmp470;_LL285: {struct Cyc_Absyn_RgnsEff_struct*_tmp46F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp46E;if(_tmp46F->tag != 26)goto _LL287;else{_tmp470=(void*)_tmp46F->f1;}}_LL286:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp470,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp470,Cyc_Absyn_sint_typ);_LL287:;
_LL288: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp46E,e2);_LL284:;}
_LL280: {struct Cyc_Absyn_Evar_struct*_tmp46A=(struct Cyc_Absyn_Evar_struct*)
_tmp461;if(_tmp46A->tag != 1)goto _LL282;else{_tmp46B=_tmp46A->f2;_tmp46C=(struct
Cyc_Core_Opt**)& _tmp46A->f2;_tmp46D=_tmp46A->f4;}}_LL281: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp126A;*_tmp46C=((_tmp126A=_cycalloc(sizeof(*
_tmp126A)),((_tmp126A->v=Cyc_Absyn_empty_effect,_tmp126A))));}return 1;_LL282:;
_LL283: {const char*_tmp126E;void*_tmp126D[1];struct Cyc_String_pa_struct _tmp126C;(
_tmp126C.tag=0,((_tmp126C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp126D[0]=& _tmp126C,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp126E="subset_effect: bad effect: %s",
_tag_dyneither(_tmp126E,sizeof(char),30))),_tag_dyneither(_tmp126D,sizeof(void*),
1)))))));}_LL277:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp476;void*_tmp477;void*_tmp478;struct
_tuple0*_tmp475=(struct _tuple0*)r1->hd;_tmp476=*_tmp475;_tmp477=_tmp476.f1;
_tmp478=_tmp476.f2;{int found=_tmp477 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp47A;void*_tmp47B;void*
_tmp47C;struct _tuple0*_tmp479=(struct _tuple0*)r2->hd;_tmp47A=*_tmp479;_tmp47B=
_tmp47A.f1;_tmp47C=_tmp47A.f2;if(Cyc_Tcutil_unify(_tmp477,_tmp47B) && Cyc_Tcutil_unify(
_tmp478,_tmp47C)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp47D=t1;struct Cyc_Core_Opt*_tmp47F;struct Cyc_Core_Opt*
_tmp480;struct Cyc_Core_Opt**_tmp481;struct Cyc_Core_Opt*_tmp482;_LL28A: {struct
Cyc_Absyn_Evar_struct*_tmp47E=(struct Cyc_Absyn_Evar_struct*)_tmp47D;if(_tmp47E->tag
!= 1)goto _LL28C;else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47E->f2;_tmp481=(struct Cyc_Core_Opt**)&
_tmp47E->f2;_tmp482=_tmp47E->f4;}}_LL28B: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp482))->v,t2);{struct
Cyc_Absyn_Kind*_tmp483=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp483,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp47F))->v)){{struct
Cyc_Core_Opt*_tmp126F;*_tmp481=((_tmp126F=_cycalloc(sizeof(*_tmp126F)),((
_tmp126F->v=t2,_tmp126F))));}return;}else{{void*_tmp485=t2;struct Cyc_Core_Opt*
_tmp487;struct Cyc_Core_Opt**_tmp488;struct Cyc_Core_Opt*_tmp489;struct Cyc_Absyn_PtrInfo
_tmp48B;_LL28F: {struct Cyc_Absyn_Evar_struct*_tmp486=(struct Cyc_Absyn_Evar_struct*)
_tmp485;if(_tmp486->tag != 1)goto _LL291;else{_tmp487=_tmp486->f2;_tmp488=(struct
Cyc_Core_Opt**)& _tmp486->f2;_tmp489=_tmp486->f4;}}_LL290: {struct Cyc_List_List*
_tmp48C=(struct Cyc_List_List*)_tmp482->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp489))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp48C,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1270;Cyc_Tcutil_failure_reason=((
_tmp1270="(type variable would escape scope)",_tag_dyneither(_tmp1270,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp47F->v,_tmp483)){{struct Cyc_Core_Opt*_tmp1271;*_tmp488=((
_tmp1271=_cycalloc(sizeof(*_tmp1271)),((_tmp1271->v=t1,_tmp1271))));}return;}{
const char*_tmp1272;Cyc_Tcutil_failure_reason=((_tmp1272="(kinds are incompatible)",
_tag_dyneither(_tmp1272,sizeof(char),25)));}goto _LL28E;}_LL291:{struct Cyc_Absyn_PointerType_struct*
_tmp48A=(struct Cyc_Absyn_PointerType_struct*)_tmp485;if(_tmp48A->tag != 5)goto
_LL293;else{_tmp48B=_tmp48A->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp47F->v)->kind
== Cyc_Absyn_BoxKind))goto _LL293;_LL292: {union Cyc_Absyn_Constraint*_tmp490=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp48B.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp491=_tmp490;union Cyc_Absyn_Constraint
_tmp492;int _tmp493;_LL296: _tmp492=*_tmp491;if((_tmp492.No_constr).tag != 3)goto
_LL298;_tmp493=(int)(_tmp492.No_constr).val;_LL297:{struct
_union_Constraint_Eq_constr*_tmp1273;(_tmp1273=& _tmp490->Eq_constr,((_tmp1273->tag=
1,_tmp1273->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp1274;*_tmp481=((
_tmp1274=_cycalloc(sizeof(*_tmp1274)),((_tmp1274->v=t2,_tmp1274))));}return;
_LL298:;_LL299: goto _LL295;_LL295:;}goto _LL28E;}_LL293:;_LL294: goto _LL28E;_LL28E:;}{
const char*_tmp1275;Cyc_Tcutil_failure_reason=((_tmp1275="(kinds are incompatible)",
_tag_dyneither(_tmp1275,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL28C:;_LL28D: goto _LL289;_LL289:;}{struct _tuple0 _tmp1276;struct _tuple0 _tmp498=(
_tmp1276.f1=t2,((_tmp1276.f2=t1,_tmp1276)));void*_tmp499;void*_tmp49B;void*
_tmp49D;void*_tmp49F;struct Cyc_Absyn_Tvar*_tmp4A1;void*_tmp4A2;struct Cyc_Absyn_Tvar*
_tmp4A4;void*_tmp4A5;struct Cyc_Absyn_AggrInfo _tmp4A7;union Cyc_Absyn_AggrInfoU
_tmp4A8;struct Cyc_List_List*_tmp4A9;void*_tmp4AA;struct Cyc_Absyn_AggrInfo _tmp4AC;
union Cyc_Absyn_AggrInfoU _tmp4AD;struct Cyc_List_List*_tmp4AE;void*_tmp4AF;struct
_tuple2*_tmp4B1;void*_tmp4B2;struct _tuple2*_tmp4B4;void*_tmp4B5;struct Cyc_List_List*
_tmp4B7;void*_tmp4B8;struct Cyc_List_List*_tmp4BA;void*_tmp4BB;struct Cyc_Absyn_DatatypeInfo
_tmp4BD;union Cyc_Absyn_DatatypeInfoU _tmp4BE;struct Cyc_Absyn_Datatypedecl**
_tmp4BF;struct Cyc_Absyn_Datatypedecl*_tmp4C0;struct Cyc_List_List*_tmp4C1;void*
_tmp4C2;struct Cyc_Absyn_DatatypeInfo _tmp4C4;union Cyc_Absyn_DatatypeInfoU _tmp4C5;
struct Cyc_Absyn_Datatypedecl**_tmp4C6;struct Cyc_Absyn_Datatypedecl*_tmp4C7;
struct Cyc_List_List*_tmp4C8;void*_tmp4C9;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4CB;union Cyc_Absyn_DatatypeFieldInfoU _tmp4CC;struct _tuple3 _tmp4CD;struct Cyc_Absyn_Datatypedecl*
_tmp4CE;struct Cyc_Absyn_Datatypefield*_tmp4CF;struct Cyc_List_List*_tmp4D0;void*
_tmp4D1;struct Cyc_Absyn_DatatypeFieldInfo _tmp4D3;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4D4;struct _tuple3 _tmp4D5;struct Cyc_Absyn_Datatypedecl*_tmp4D6;struct Cyc_Absyn_Datatypefield*
_tmp4D7;struct Cyc_List_List*_tmp4D8;void*_tmp4D9;struct Cyc_Absyn_PtrInfo _tmp4DB;
void*_tmp4DC;struct Cyc_Absyn_Tqual _tmp4DD;struct Cyc_Absyn_PtrAtts _tmp4DE;void*
_tmp4DF;union Cyc_Absyn_Constraint*_tmp4E0;union Cyc_Absyn_Constraint*_tmp4E1;
union Cyc_Absyn_Constraint*_tmp4E2;void*_tmp4E3;struct Cyc_Absyn_PtrInfo _tmp4E5;
void*_tmp4E6;struct Cyc_Absyn_Tqual _tmp4E7;struct Cyc_Absyn_PtrAtts _tmp4E8;void*
_tmp4E9;union Cyc_Absyn_Constraint*_tmp4EA;union Cyc_Absyn_Constraint*_tmp4EB;
union Cyc_Absyn_Constraint*_tmp4EC;void*_tmp4ED;enum Cyc_Absyn_Sign _tmp4EF;enum 
Cyc_Absyn_Size_of _tmp4F0;void*_tmp4F1;enum Cyc_Absyn_Sign _tmp4F3;enum Cyc_Absyn_Size_of
_tmp4F4;void*_tmp4F5;void*_tmp4F7;void*_tmp4F9;int _tmp4FB;void*_tmp4FC;int
_tmp4FE;void*_tmp4FF;void*_tmp501;void*_tmp502;void*_tmp504;void*_tmp505;struct
Cyc_Absyn_Exp*_tmp507;void*_tmp508;struct Cyc_Absyn_Exp*_tmp50A;void*_tmp50B;
struct Cyc_Absyn_ArrayInfo _tmp50D;void*_tmp50E;struct Cyc_Absyn_Tqual _tmp50F;
struct Cyc_Absyn_Exp*_tmp510;union Cyc_Absyn_Constraint*_tmp511;void*_tmp512;
struct Cyc_Absyn_ArrayInfo _tmp514;void*_tmp515;struct Cyc_Absyn_Tqual _tmp516;
struct Cyc_Absyn_Exp*_tmp517;union Cyc_Absyn_Constraint*_tmp518;void*_tmp519;
struct Cyc_Absyn_FnInfo _tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_Core_Opt*
_tmp51D;struct Cyc_Absyn_Tqual _tmp51E;void*_tmp51F;struct Cyc_List_List*_tmp520;
int _tmp521;struct Cyc_Absyn_VarargInfo*_tmp522;struct Cyc_List_List*_tmp523;struct
Cyc_List_List*_tmp524;void*_tmp525;struct Cyc_Absyn_FnInfo _tmp527;struct Cyc_List_List*
_tmp528;struct Cyc_Core_Opt*_tmp529;struct Cyc_Absyn_Tqual _tmp52A;void*_tmp52B;
struct Cyc_List_List*_tmp52C;int _tmp52D;struct Cyc_Absyn_VarargInfo*_tmp52E;struct
Cyc_List_List*_tmp52F;struct Cyc_List_List*_tmp530;void*_tmp531;struct Cyc_List_List*
_tmp533;void*_tmp534;struct Cyc_List_List*_tmp536;void*_tmp537;enum Cyc_Absyn_AggrKind
_tmp539;struct Cyc_List_List*_tmp53A;void*_tmp53B;enum Cyc_Absyn_AggrKind _tmp53D;
struct Cyc_List_List*_tmp53E;void*_tmp53F;void*_tmp541;void*_tmp543;void*_tmp545;
void*_tmp547;void*_tmp549;void*_tmp54B;void*_tmp54D;void*_tmp54E;void*_tmp550;
void*_tmp551;void*_tmp553;void*_tmp554;void*_tmp555;void*_tmp557;void*_tmp558;
void*_tmp559;void*_tmp55B;void*_tmp55D;void*_tmp55F;void*_tmp561;void*_tmp563;
_LL29B: _tmp499=_tmp498.f1;{struct Cyc_Absyn_Evar_struct*_tmp49A=(struct Cyc_Absyn_Evar_struct*)
_tmp499;if(_tmp49A->tag != 1)goto _LL29D;};_LL29C: Cyc_Tcutil_unify_it(t2,t1);
return;_LL29D: _tmp49B=_tmp498.f1;{struct Cyc_Absyn_VoidType_struct*_tmp49C=(
struct Cyc_Absyn_VoidType_struct*)_tmp49B;if(_tmp49C->tag != 0)goto _LL29F;};
_tmp49D=_tmp498.f2;{struct Cyc_Absyn_VoidType_struct*_tmp49E=(struct Cyc_Absyn_VoidType_struct*)
_tmp49D;if(_tmp49E->tag != 0)goto _LL29F;};_LL29E: return;_LL29F: _tmp49F=_tmp498.f1;{
struct Cyc_Absyn_VarType_struct*_tmp4A0=(struct Cyc_Absyn_VarType_struct*)_tmp49F;
if(_tmp4A0->tag != 2)goto _LL2A1;else{_tmp4A1=_tmp4A0->f1;}};_tmp4A2=_tmp498.f2;{
struct Cyc_Absyn_VarType_struct*_tmp4A3=(struct Cyc_Absyn_VarType_struct*)_tmp4A2;
if(_tmp4A3->tag != 2)goto _LL2A1;else{_tmp4A4=_tmp4A3->f1;}};_LL2A0: {struct
_dyneither_ptr*_tmp565=_tmp4A1->name;struct _dyneither_ptr*_tmp566=_tmp4A4->name;
int _tmp567=_tmp4A1->identity;int _tmp568=_tmp4A4->identity;if(_tmp568 == _tmp567)
return;{const char*_tmp1277;Cyc_Tcutil_failure_reason=((_tmp1277="(variable types are not the same)",
_tag_dyneither(_tmp1277,sizeof(char),34)));}goto _LL29A;}_LL2A1: _tmp4A5=_tmp498.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4A6=(struct Cyc_Absyn_AggrType_struct*)
_tmp4A5;if(_tmp4A6->tag != 12)goto _LL2A3;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A7.aggr_info;
_tmp4A9=_tmp4A7.targs;}};_tmp4AA=_tmp498.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4AB=(struct Cyc_Absyn_AggrType_struct*)_tmp4AA;if(_tmp4AB->tag != 12)goto
_LL2A3;else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AC.aggr_info;_tmp4AE=_tmp4AC.targs;}};
_LL2A2: {enum Cyc_Absyn_AggrKind _tmp56B;struct _tuple2*_tmp56C;struct _tuple12
_tmp56A=Cyc_Absyn_aggr_kinded_name(_tmp4AD);_tmp56B=_tmp56A.f1;_tmp56C=_tmp56A.f2;{
enum Cyc_Absyn_AggrKind _tmp56E;struct _tuple2*_tmp56F;struct _tuple12 _tmp56D=Cyc_Absyn_aggr_kinded_name(
_tmp4A8);_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;if(_tmp56B != _tmp56E){{const char*
_tmp1278;Cyc_Tcutil_failure_reason=((_tmp1278="(struct and union type)",
_tag_dyneither(_tmp1278,sizeof(char),24)));}goto _LL29A;}if(Cyc_Absyn_qvar_cmp(
_tmp56C,_tmp56F)!= 0){{const char*_tmp1279;Cyc_Tcutil_failure_reason=((_tmp1279="(different type name)",
_tag_dyneither(_tmp1279,sizeof(char),22)));}goto _LL29A;}Cyc_Tcutil_unify_list(
_tmp4AE,_tmp4A9);return;};}_LL2A3: _tmp4AF=_tmp498.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp4B0=(struct Cyc_Absyn_EnumType_struct*)_tmp4AF;if(_tmp4B0->tag != 14)goto
_LL2A5;else{_tmp4B1=_tmp4B0->f1;}};_tmp4B2=_tmp498.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp4B3=(struct Cyc_Absyn_EnumType_struct*)_tmp4B2;if(_tmp4B3->tag != 14)goto
_LL2A5;else{_tmp4B4=_tmp4B3->f1;}};_LL2A4: if(Cyc_Absyn_qvar_cmp(_tmp4B1,_tmp4B4)
== 0)return;{const char*_tmp127A;Cyc_Tcutil_failure_reason=((_tmp127A="(different enum types)",
_tag_dyneither(_tmp127A,sizeof(char),23)));}goto _LL29A;_LL2A5: _tmp4B5=_tmp498.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp4B6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B5;if(_tmp4B6->tag != 15)goto _LL2A7;else{_tmp4B7=_tmp4B6->f1;}};_tmp4B8=
_tmp498.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4B9=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B8;if(_tmp4B9->tag != 15)goto _LL2A7;else{_tmp4BA=_tmp4B9->f1;}};_LL2A6: {int
bad=0;for(0;_tmp4B7 != 0  && _tmp4BA != 0;(_tmp4B7=_tmp4B7->tl,_tmp4BA=_tmp4BA->tl)){
struct Cyc_Absyn_Enumfield*_tmp573=(struct Cyc_Absyn_Enumfield*)_tmp4B7->hd;struct
Cyc_Absyn_Enumfield*_tmp574=(struct Cyc_Absyn_Enumfield*)_tmp4BA->hd;if(Cyc_Absyn_qvar_cmp(
_tmp573->name,_tmp574->name)!= 0){{const char*_tmp127B;Cyc_Tcutil_failure_reason=((
_tmp127B="(different names for enum fields)",_tag_dyneither(_tmp127B,sizeof(char),
34)));}bad=1;break;}if(_tmp573->tag == _tmp574->tag)continue;if(_tmp573->tag == 0
 || _tmp574->tag == 0){{const char*_tmp127C;Cyc_Tcutil_failure_reason=((_tmp127C="(different tag values for enum fields)",
_tag_dyneither(_tmp127C,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp573->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp574->tag))){{const char*_tmp127D;Cyc_Tcutil_failure_reason=((_tmp127D="(different tag values for enum fields)",
_tag_dyneither(_tmp127D,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL29A;if(
_tmp4B7 == 0  && _tmp4BA == 0)return;{const char*_tmp127E;Cyc_Tcutil_failure_reason=((
_tmp127E="(different number of fields for enums)",_tag_dyneither(_tmp127E,
sizeof(char),39)));}goto _LL29A;}_LL2A7: _tmp4BB=_tmp498.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4BC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4BB;if(_tmp4BC->tag != 3)goto
_LL2A9;else{_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BD.datatype_info;if((_tmp4BE.KnownDatatype).tag
!= 2)goto _LL2A9;_tmp4BF=(struct Cyc_Absyn_Datatypedecl**)(_tmp4BE.KnownDatatype).val;
_tmp4C0=*_tmp4BF;_tmp4C1=_tmp4BD.targs;}};_tmp4C2=_tmp498.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4C3=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4C2;if(_tmp4C3->tag != 3)goto
_LL2A9;else{_tmp4C4=_tmp4C3->f1;_tmp4C5=_tmp4C4.datatype_info;if((_tmp4C5.KnownDatatype).tag
!= 2)goto _LL2A9;_tmp4C6=(struct Cyc_Absyn_Datatypedecl**)(_tmp4C5.KnownDatatype).val;
_tmp4C7=*_tmp4C6;_tmp4C8=_tmp4C4.targs;}};_LL2A8: if(_tmp4C0 == _tmp4C7  || Cyc_Absyn_qvar_cmp(
_tmp4C0->name,_tmp4C7->name)== 0){Cyc_Tcutil_unify_list(_tmp4C8,_tmp4C1);return;}
goto _LL29A;_LL2A9: _tmp4C9=_tmp498.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4CA=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4C9;if(_tmp4CA->tag != 4)
goto _LL2AB;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CB.field_info;if((_tmp4CC.KnownDatatypefield).tag
!= 2)goto _LL2AB;_tmp4CD=(struct _tuple3)(_tmp4CC.KnownDatatypefield).val;_tmp4CE=
_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;_tmp4D0=_tmp4CB.targs;}};_tmp4D1=_tmp498.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4D2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4D1;if(_tmp4D2->tag != 4)goto _LL2AB;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D3.field_info;
if((_tmp4D4.KnownDatatypefield).tag != 2)goto _LL2AB;_tmp4D5=(struct _tuple3)(
_tmp4D4.KnownDatatypefield).val;_tmp4D6=_tmp4D5.f1;_tmp4D7=_tmp4D5.f2;_tmp4D8=
_tmp4D3.targs;}};_LL2AA: if((_tmp4CE == _tmp4D6  || Cyc_Absyn_qvar_cmp(_tmp4CE->name,
_tmp4D6->name)== 0) && (_tmp4CF == _tmp4D7  || Cyc_Absyn_qvar_cmp(_tmp4CF->name,
_tmp4D7->name)== 0)){Cyc_Tcutil_unify_list(_tmp4D8,_tmp4D0);return;}{const char*
_tmp127F;Cyc_Tcutil_failure_reason=((_tmp127F="(different datatype field types)",
_tag_dyneither(_tmp127F,sizeof(char),33)));}goto _LL29A;_LL2AB: _tmp4D9=_tmp498.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4DA=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D9;if(_tmp4DA->tag != 5)goto _LL2AD;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DB.elt_typ;
_tmp4DD=_tmp4DB.elt_tq;_tmp4DE=_tmp4DB.ptr_atts;_tmp4DF=_tmp4DE.rgn;_tmp4E0=
_tmp4DE.nullable;_tmp4E1=_tmp4DE.bounds;_tmp4E2=_tmp4DE.zero_term;}};_tmp4E3=
_tmp498.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4E4=(struct Cyc_Absyn_PointerType_struct*)
_tmp4E3;if(_tmp4E4->tag != 5)goto _LL2AD;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E5.elt_typ;
_tmp4E7=_tmp4E5.elt_tq;_tmp4E8=_tmp4E5.ptr_atts;_tmp4E9=_tmp4E8.rgn;_tmp4EA=
_tmp4E8.nullable;_tmp4EB=_tmp4E8.bounds;_tmp4EC=_tmp4E8.zero_term;}};_LL2AC: Cyc_Tcutil_unify_it(
_tmp4E6,_tmp4DC);Cyc_Tcutil_unify_it(_tmp4DF,_tmp4E9);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1280;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4EC,_tmp4E2,((_tmp1280="(not both zero terminated)",
_tag_dyneither(_tmp1280,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4E7,
_tmp4E6,_tmp4DD,_tmp4DC);{const char*_tmp1281;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4EB,_tmp4E1,((
_tmp1281="(different pointer bounds)",_tag_dyneither(_tmp1281,sizeof(char),27))));}{
void*_tmp57C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4EB);_LL2D8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp57D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp57C;if(_tmp57D->tag != 0)goto
_LL2DA;}_LL2D9: return;_LL2DA:;_LL2DB: goto _LL2D7;_LL2D7:;}{const char*_tmp1282;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4EA,_tmp4E0,((_tmp1282="(incompatible pointer types)",_tag_dyneither(
_tmp1282,sizeof(char),29))));}return;_LL2AD: _tmp4ED=_tmp498.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4EE=(struct Cyc_Absyn_IntType_struct*)_tmp4ED;if(_tmp4EE->tag != 6)goto _LL2AF;
else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}};_tmp4F1=_tmp498.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4F2=(struct Cyc_Absyn_IntType_struct*)_tmp4F1;if(_tmp4F2->tag != 6)goto _LL2AF;
else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;}};_LL2AE: if(_tmp4F3 == _tmp4EF  && ((
_tmp4F4 == _tmp4F0  || _tmp4F4 == Cyc_Absyn_Int_sz  && _tmp4F0 == Cyc_Absyn_Long_sz)
 || _tmp4F4 == Cyc_Absyn_Long_sz  && _tmp4F0 == Cyc_Absyn_Int_sz))return;{const char*
_tmp1283;Cyc_Tcutil_failure_reason=((_tmp1283="(different integral types)",
_tag_dyneither(_tmp1283,sizeof(char),27)));}goto _LL29A;_LL2AF: _tmp4F5=_tmp498.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4F6=(struct Cyc_Absyn_FloatType_struct*)
_tmp4F5;if(_tmp4F6->tag != 7)goto _LL2B1;};_tmp4F7=_tmp498.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4F8=(struct Cyc_Absyn_FloatType_struct*)_tmp4F7;if(_tmp4F8->tag != 7)goto
_LL2B1;};_LL2B0: return;_LL2B1: _tmp4F9=_tmp498.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4FA=(struct Cyc_Absyn_DoubleType_struct*)_tmp4F9;if(_tmp4FA->tag != 8)goto
_LL2B3;else{_tmp4FB=_tmp4FA->f1;}};_tmp4FC=_tmp498.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4FD=(struct Cyc_Absyn_DoubleType_struct*)_tmp4FC;if(_tmp4FD->tag != 8)goto
_LL2B3;else{_tmp4FE=_tmp4FD->f1;}};_LL2B2: if(_tmp4FB == _tmp4FE)return;goto _LL29A;
_LL2B3: _tmp4FF=_tmp498.f1;{struct Cyc_Absyn_TagType_struct*_tmp500=(struct Cyc_Absyn_TagType_struct*)
_tmp4FF;if(_tmp500->tag != 20)goto _LL2B5;else{_tmp501=(void*)_tmp500->f1;}};
_tmp502=_tmp498.f2;{struct Cyc_Absyn_TagType_struct*_tmp503=(struct Cyc_Absyn_TagType_struct*)
_tmp502;if(_tmp503->tag != 20)goto _LL2B5;else{_tmp504=(void*)_tmp503->f1;}};
_LL2B4: Cyc_Tcutil_unify_it(_tmp501,_tmp504);return;_LL2B5: _tmp505=_tmp498.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp506=(struct Cyc_Absyn_ValueofType_struct*)
_tmp505;if(_tmp506->tag != 19)goto _LL2B7;else{_tmp507=_tmp506->f1;}};_tmp508=
_tmp498.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp509=(struct Cyc_Absyn_ValueofType_struct*)
_tmp508;if(_tmp509->tag != 19)goto _LL2B7;else{_tmp50A=_tmp509->f1;}};_LL2B6: if(!
Cyc_Evexp_same_const_exp(_tmp507,_tmp50A)){{const char*_tmp1284;Cyc_Tcutil_failure_reason=((
_tmp1284="(cannot prove expressions are the same)",_tag_dyneither(_tmp1284,
sizeof(char),40)));}goto _LL29A;}return;_LL2B7: _tmp50B=_tmp498.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp50C=(struct Cyc_Absyn_ArrayType_struct*)_tmp50B;if(_tmp50C->tag != 9)goto
_LL2B9;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.elt_type;_tmp50F=_tmp50D.tq;
_tmp510=_tmp50D.num_elts;_tmp511=_tmp50D.zero_term;}};_tmp512=_tmp498.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp513=(struct Cyc_Absyn_ArrayType_struct*)_tmp512;
if(_tmp513->tag != 9)goto _LL2B9;else{_tmp514=_tmp513->f1;_tmp515=_tmp514.elt_type;
_tmp516=_tmp514.tq;_tmp517=_tmp514.num_elts;_tmp518=_tmp514.zero_term;}};_LL2B8:
Cyc_Tcutil_unify_it(_tmp515,_tmp50E);Cyc_Tcutil_unify_tqual(_tmp516,_tmp515,
_tmp50F,_tmp50E);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp1285;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp511,_tmp518,((_tmp1285="(not both zero terminated)",_tag_dyneither(_tmp1285,
sizeof(char),27))));}if(_tmp510 == _tmp517)return;if(_tmp510 == 0  || _tmp517 == 0)
goto _LL29A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp510,(struct Cyc_Absyn_Exp*)
_tmp517))return;{const char*_tmp1286;Cyc_Tcutil_failure_reason=((_tmp1286="(different array sizes)",
_tag_dyneither(_tmp1286,sizeof(char),24)));}goto _LL29A;_LL2B9: _tmp519=_tmp498.f1;{
struct Cyc_Absyn_FnType_struct*_tmp51A=(struct Cyc_Absyn_FnType_struct*)_tmp519;
if(_tmp51A->tag != 10)goto _LL2BB;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51B.tvars;
_tmp51D=_tmp51B.effect;_tmp51E=_tmp51B.ret_tqual;_tmp51F=_tmp51B.ret_typ;_tmp520=
_tmp51B.args;_tmp521=_tmp51B.c_varargs;_tmp522=_tmp51B.cyc_varargs;_tmp523=
_tmp51B.rgn_po;_tmp524=_tmp51B.attributes;}};_tmp525=_tmp498.f2;{struct Cyc_Absyn_FnType_struct*
_tmp526=(struct Cyc_Absyn_FnType_struct*)_tmp525;if(_tmp526->tag != 10)goto _LL2BB;
else{_tmp527=_tmp526->f1;_tmp528=_tmp527.tvars;_tmp529=_tmp527.effect;_tmp52A=
_tmp527.ret_tqual;_tmp52B=_tmp527.ret_typ;_tmp52C=_tmp527.args;_tmp52D=_tmp527.c_varargs;
_tmp52E=_tmp527.cyc_varargs;_tmp52F=_tmp527.rgn_po;_tmp530=_tmp527.attributes;}};
_LL2BA: {int done=0;struct _RegionHandle _tmp583=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp583;_push_region(rgn);{struct Cyc_List_List*inst=0;while(
_tmp528 != 0){if(_tmp51C == 0){{const char*_tmp1287;Cyc_Tcutil_failure_reason=((
_tmp1287="(second function type has too few type variables)",_tag_dyneither(
_tmp1287,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp585=((struct Cyc_Absyn_Tvar*)_tmp528->hd)->kind;void*_tmp586=((struct Cyc_Absyn_Tvar*)
_tmp51C->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp585,_tmp586)){{const char*
_tmp128D;void*_tmp128C[3];struct Cyc_String_pa_struct _tmp128B;struct Cyc_String_pa_struct
_tmp128A;struct Cyc_String_pa_struct _tmp1289;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1289.tag=0,((_tmp1289.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp51C->hd,& Cyc_Tcutil_bk))),((_tmp128A.tag=0,((_tmp128A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp528->hd,& Cyc_Tcutil_bk))),((_tmp128B.tag=0,((_tmp128B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp528->hd)),((_tmp128C[0]=& _tmp128B,((_tmp128C[1]=& _tmp128A,((_tmp128C[2]=&
_tmp1289,Cyc_aprintf(((_tmp128D="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp128D,sizeof(char),44))),_tag_dyneither(_tmp128C,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp1297;struct Cyc_Absyn_VarType_struct _tmp1296;struct Cyc_Absyn_VarType_struct*
_tmp1295;struct Cyc_List_List*_tmp1294;inst=((_tmp1294=_region_malloc(rgn,sizeof(*
_tmp1294)),((_tmp1294->hd=((_tmp1297=_region_malloc(rgn,sizeof(*_tmp1297)),((
_tmp1297->f1=(struct Cyc_Absyn_Tvar*)_tmp51C->hd,((_tmp1297->f2=(void*)((_tmp1295=
_cycalloc(sizeof(*_tmp1295)),((_tmp1295[0]=((_tmp1296.tag=2,((_tmp1296.f1=(
struct Cyc_Absyn_Tvar*)_tmp528->hd,_tmp1296)))),_tmp1295)))),_tmp1297)))))),((
_tmp1294->tl=inst,_tmp1294))))));}_tmp528=_tmp528->tl;_tmp51C=_tmp51C->tl;};}if(
_tmp51C != 0){{const char*_tmp1298;Cyc_Tcutil_failure_reason=((_tmp1298="(second function type has too many type variables)",
_tag_dyneither(_tmp1298,sizeof(char),51)));}_npop_handler(0);goto _LL29A;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp12A4;struct Cyc_Absyn_FnInfo _tmp12A3;
struct Cyc_Absyn_FnType_struct*_tmp12A2;struct Cyc_Absyn_FnType_struct _tmp129E;
struct Cyc_Absyn_FnInfo _tmp129D;struct Cyc_Absyn_FnType_struct*_tmp129C;Cyc_Tcutil_unify_it((
void*)((_tmp129C=_cycalloc(sizeof(*_tmp129C)),((_tmp129C[0]=((_tmp129E.tag=10,((
_tmp129E.f1=((_tmp129D.tvars=0,((_tmp129D.effect=_tmp529,((_tmp129D.ret_tqual=
_tmp52A,((_tmp129D.ret_typ=_tmp52B,((_tmp129D.args=_tmp52C,((_tmp129D.c_varargs=
_tmp52D,((_tmp129D.cyc_varargs=_tmp52E,((_tmp129D.rgn_po=_tmp52F,((_tmp129D.attributes=
_tmp530,_tmp129D)))))))))))))))))),_tmp129E)))),_tmp129C)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp12A2=_cycalloc(sizeof(*_tmp12A2)),((_tmp12A2[0]=((_tmp12A4.tag=
10,((_tmp12A4.f1=((_tmp12A3.tvars=0,((_tmp12A3.effect=_tmp51D,((_tmp12A3.ret_tqual=
_tmp51E,((_tmp12A3.ret_typ=_tmp51F,((_tmp12A3.args=_tmp520,((_tmp12A3.c_varargs=
_tmp521,((_tmp12A3.cyc_varargs=_tmp522,((_tmp12A3.rgn_po=_tmp523,((_tmp12A3.attributes=
_tmp524,_tmp12A3)))))))))))))))))),_tmp12A4)))),_tmp12A2))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp52B,_tmp51F);Cyc_Tcutil_unify_tqual(
_tmp52A,_tmp52B,_tmp51E,_tmp51F);for(0;_tmp52C != 0  && _tmp520 != 0;(_tmp52C=
_tmp52C->tl,_tmp520=_tmp520->tl)){struct Cyc_Absyn_Tqual _tmp598;void*_tmp599;
struct _tuple9 _tmp597=*((struct _tuple9*)_tmp52C->hd);_tmp598=_tmp597.f2;_tmp599=
_tmp597.f3;{struct Cyc_Absyn_Tqual _tmp59B;void*_tmp59C;struct _tuple9 _tmp59A=*((
struct _tuple9*)_tmp520->hd);_tmp59B=_tmp59A.f2;_tmp59C=_tmp59A.f3;Cyc_Tcutil_unify_it(
_tmp599,_tmp59C);Cyc_Tcutil_unify_tqual(_tmp598,_tmp599,_tmp59B,_tmp59C);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp52C != 0  || _tmp520 != 0){{const char*_tmp12A5;
Cyc_Tcutil_failure_reason=((_tmp12A5="(function types have different number of arguments)",
_tag_dyneither(_tmp12A5,sizeof(char),52)));}_npop_handler(0);goto _LL29A;}if(
_tmp52D != _tmp521){{const char*_tmp12A6;Cyc_Tcutil_failure_reason=((_tmp12A6="(only one function type takes C varargs)",
_tag_dyneither(_tmp12A6,sizeof(char),41)));}_npop_handler(0);goto _LL29A;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp12A7;struct _tuple21 _tmp5A0=(_tmp12A7.f1=
_tmp52E,((_tmp12A7.f2=_tmp522,_tmp12A7)));struct Cyc_Absyn_VarargInfo*_tmp5A1;
struct Cyc_Absyn_VarargInfo*_tmp5A2;struct Cyc_Absyn_VarargInfo*_tmp5A3;struct Cyc_Absyn_VarargInfo*
_tmp5A4;struct Cyc_Absyn_VarargInfo*_tmp5A5;struct Cyc_Absyn_VarargInfo _tmp5A6;
struct Cyc_Core_Opt*_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A9;int _tmp5AA;
struct Cyc_Absyn_VarargInfo*_tmp5AB;struct Cyc_Absyn_VarargInfo _tmp5AC;struct Cyc_Core_Opt*
_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AE;void*_tmp5AF;int _tmp5B0;_LL2DD: _tmp5A1=
_tmp5A0.f1;if(_tmp5A1 != 0)goto _LL2DF;_tmp5A2=_tmp5A0.f2;if(_tmp5A2 != 0)goto
_LL2DF;_LL2DE: goto _LL2DC;_LL2DF: _tmp5A3=_tmp5A0.f1;if(_tmp5A3 != 0)goto _LL2E1;
_LL2E0: goto _LL2E2;_LL2E1: _tmp5A4=_tmp5A0.f2;if(_tmp5A4 != 0)goto _LL2E3;_LL2E2:
bad_cyc_vararg=1;{const char*_tmp12A8;Cyc_Tcutil_failure_reason=((_tmp12A8="(only one function type takes varargs)",
_tag_dyneither(_tmp12A8,sizeof(char),39)));}goto _LL2DC;_LL2E3: _tmp5A5=_tmp5A0.f1;
if(_tmp5A5 == 0)goto _LL2DC;_tmp5A6=*_tmp5A5;_tmp5A7=_tmp5A6.name;_tmp5A8=_tmp5A6.tq;
_tmp5A9=_tmp5A6.type;_tmp5AA=_tmp5A6.inject;_tmp5AB=_tmp5A0.f2;if(_tmp5AB == 0)
goto _LL2DC;_tmp5AC=*_tmp5AB;_tmp5AD=_tmp5AC.name;_tmp5AE=_tmp5AC.tq;_tmp5AF=
_tmp5AC.type;_tmp5B0=_tmp5AC.inject;_LL2E4: Cyc_Tcutil_unify_it(_tmp5A9,_tmp5AF);
Cyc_Tcutil_unify_tqual(_tmp5A8,_tmp5A9,_tmp5AE,_tmp5AF);if(_tmp5AA != _tmp5B0){
bad_cyc_vararg=1;{const char*_tmp12A9;Cyc_Tcutil_failure_reason=((_tmp12A9="(only one function type injects varargs)",
_tag_dyneither(_tmp12A9,sizeof(char),41)));};}goto _LL2DC;_LL2DC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL29A;}{int bad_effect=0;{struct _tuple22
_tmp12AA;struct _tuple22 _tmp5B4=(_tmp12AA.f1=_tmp529,((_tmp12AA.f2=_tmp51D,
_tmp12AA)));struct Cyc_Core_Opt*_tmp5B5;struct Cyc_Core_Opt*_tmp5B6;struct Cyc_Core_Opt*
_tmp5B7;struct Cyc_Core_Opt*_tmp5B8;_LL2E6: _tmp5B5=_tmp5B4.f1;if(_tmp5B5 != 0)goto
_LL2E8;_tmp5B6=_tmp5B4.f2;if(_tmp5B6 != 0)goto _LL2E8;_LL2E7: goto _LL2E5;_LL2E8:
_tmp5B7=_tmp5B4.f1;if(_tmp5B7 != 0)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp5B8=
_tmp5B4.f2;if(_tmp5B8 != 0)goto _LL2EC;_LL2EB: bad_effect=1;goto _LL2E5;_LL2EC:;
_LL2ED: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp529))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp51D))->v);
goto _LL2E5;_LL2E5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp12AB;Cyc_Tcutil_failure_reason=((_tmp12AB="(function type effects do not unify)",
_tag_dyneither(_tmp12AB,sizeof(char),37)));}_npop_handler(0);goto _LL29A;}if(!Cyc_Tcutil_same_atts(
_tmp524,_tmp530)){{const char*_tmp12AC;Cyc_Tcutil_failure_reason=((_tmp12AC="(function types have different attributes)",
_tag_dyneither(_tmp12AC,sizeof(char),43)));}_npop_handler(0);goto _LL29A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp523,_tmp52F)){{const char*_tmp12AD;Cyc_Tcutil_failure_reason=((_tmp12AD="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp12AD,sizeof(char),58)));}_npop_handler(0);goto _LL29A;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2BB: _tmp531=_tmp498.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp532=(struct Cyc_Absyn_TupleType_struct*)_tmp531;
if(_tmp532->tag != 11)goto _LL2BD;else{_tmp533=_tmp532->f1;}};_tmp534=_tmp498.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp535=(struct Cyc_Absyn_TupleType_struct*)
_tmp534;if(_tmp535->tag != 11)goto _LL2BD;else{_tmp536=_tmp535->f1;}};_LL2BC: for(0;
_tmp536 != 0  && _tmp533 != 0;(_tmp536=_tmp536->tl,_tmp533=_tmp533->tl)){struct Cyc_Absyn_Tqual
_tmp5BD;void*_tmp5BE;struct _tuple11 _tmp5BC=*((struct _tuple11*)_tmp536->hd);
_tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;{struct Cyc_Absyn_Tqual _tmp5C0;void*_tmp5C1;
struct _tuple11 _tmp5BF=*((struct _tuple11*)_tmp533->hd);_tmp5C0=_tmp5BF.f1;_tmp5C1=
_tmp5BF.f2;Cyc_Tcutil_unify_it(_tmp5BE,_tmp5C1);Cyc_Tcutil_unify_tqual(_tmp5BD,
_tmp5BE,_tmp5C0,_tmp5C1);};}if(_tmp536 == 0  && _tmp533 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp12AE;Cyc_Tcutil_failure_reason=((
_tmp12AE="(tuple types have different numbers of components)",_tag_dyneither(
_tmp12AE,sizeof(char),51)));}goto _LL29A;_LL2BD: _tmp537=_tmp498.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp538=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp537;if(_tmp538->tag != 13)goto
_LL2BF;else{_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;}};_tmp53B=_tmp498.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp53C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp53B;if(_tmp53C->tag != 13)goto _LL2BF;else{_tmp53D=_tmp53C->f1;_tmp53E=_tmp53C->f2;}};
_LL2BE: if(_tmp53D != _tmp539){{const char*_tmp12AF;Cyc_Tcutil_failure_reason=((
_tmp12AF="(struct and union type)",_tag_dyneither(_tmp12AF,sizeof(char),24)));}
goto _LL29A;}for(0;_tmp53E != 0  && _tmp53A != 0;(_tmp53E=_tmp53E->tl,_tmp53A=
_tmp53A->tl)){struct Cyc_Absyn_Aggrfield*_tmp5C4=(struct Cyc_Absyn_Aggrfield*)
_tmp53E->hd;struct Cyc_Absyn_Aggrfield*_tmp5C5=(struct Cyc_Absyn_Aggrfield*)
_tmp53A->hd;if(Cyc_strptrcmp(_tmp5C4->name,_tmp5C5->name)!= 0){{const char*
_tmp12B0;Cyc_Tcutil_failure_reason=((_tmp12B0="(different member names)",
_tag_dyneither(_tmp12B0,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5C4->type,_tmp5C5->type);Cyc_Tcutil_unify_tqual(_tmp5C4->tq,
_tmp5C4->type,_tmp5C5->tq,_tmp5C5->type);if(!Cyc_Tcutil_same_atts(_tmp5C4->attributes,
_tmp5C5->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp12B1;Cyc_Tcutil_failure_reason=((_tmp12B1="(different attributes on member)",
_tag_dyneither(_tmp12B1,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5C4->width != 0  && _tmp5C5->width == 0  || _tmp5C5->width != 0  && _tmp5C4->width
== 0) || (_tmp5C4->width != 0  && _tmp5C5->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5C4->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5C5->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp12B2;Cyc_Tcutil_failure_reason=((_tmp12B2="(different bitfield widths on member)",
_tag_dyneither(_tmp12B2,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp53E == 0  && _tmp53A == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp12B3;Cyc_Tcutil_failure_reason=((_tmp12B3="(different number of members)",
_tag_dyneither(_tmp12B3,sizeof(char),30)));}goto _LL29A;_LL2BF: _tmp53F=_tmp498.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp540=(struct Cyc_Absyn_HeapRgn_struct*)_tmp53F;
if(_tmp540->tag != 21)goto _LL2C1;};_tmp541=_tmp498.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp542=(struct Cyc_Absyn_HeapRgn_struct*)_tmp541;if(_tmp542->tag != 21)goto _LL2C1;};
_LL2C0: return;_LL2C1: _tmp543=_tmp498.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp544=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp543;if(_tmp544->tag != 22)goto
_LL2C3;};_tmp545=_tmp498.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp546=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp545;if(_tmp546->tag != 22)goto _LL2C3;};_LL2C2: return;_LL2C3: _tmp547=_tmp498.f1;{
struct Cyc_Absyn_RefCntRgn_struct*_tmp548=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp547;if(_tmp548->tag != 23)goto _LL2C5;};_tmp549=_tmp498.f2;{struct Cyc_Absyn_RefCntRgn_struct*
_tmp54A=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp549;if(_tmp54A->tag != 23)goto
_LL2C5;};_LL2C4: return;_LL2C5: _tmp54B=_tmp498.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp54C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp54B;if(_tmp54C->tag != 16)goto
_LL2C7;else{_tmp54D=(void*)_tmp54C->f1;}};_tmp54E=_tmp498.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp54F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp54E;if(_tmp54F->tag != 16)goto
_LL2C7;else{_tmp550=(void*)_tmp54F->f1;}};_LL2C6: Cyc_Tcutil_unify_it(_tmp54D,
_tmp550);return;_LL2C7: _tmp551=_tmp498.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp552=(struct Cyc_Absyn_DynRgnType_struct*)_tmp551;if(_tmp552->tag != 17)goto
_LL2C9;else{_tmp553=(void*)_tmp552->f1;_tmp554=(void*)_tmp552->f2;}};_tmp555=
_tmp498.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp556=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp555;if(_tmp556->tag != 17)goto _LL2C9;else{_tmp557=(void*)_tmp556->f1;_tmp558=(
void*)_tmp556->f2;}};_LL2C8: Cyc_Tcutil_unify_it(_tmp553,_tmp557);Cyc_Tcutil_unify_it(
_tmp554,_tmp558);return;_LL2C9: _tmp559=_tmp498.f1;{struct Cyc_Absyn_JoinEff_struct*
_tmp55A=(struct Cyc_Absyn_JoinEff_struct*)_tmp559;if(_tmp55A->tag != 25)goto _LL2CB;};
_LL2CA: goto _LL2CC;_LL2CB: _tmp55B=_tmp498.f2;{struct Cyc_Absyn_JoinEff_struct*
_tmp55C=(struct Cyc_Absyn_JoinEff_struct*)_tmp55B;if(_tmp55C->tag != 25)goto _LL2CD;};
_LL2CC: goto _LL2CE;_LL2CD: _tmp55D=_tmp498.f1;{struct Cyc_Absyn_AccessEff_struct*
_tmp55E=(struct Cyc_Absyn_AccessEff_struct*)_tmp55D;if(_tmp55E->tag != 24)goto
_LL2CF;};_LL2CE: goto _LL2D0;_LL2CF: _tmp55F=_tmp498.f1;{struct Cyc_Absyn_RgnsEff_struct*
_tmp560=(struct Cyc_Absyn_RgnsEff_struct*)_tmp55F;if(_tmp560->tag != 26)goto _LL2D1;};
_LL2D0: goto _LL2D2;_LL2D1: _tmp561=_tmp498.f2;{struct Cyc_Absyn_RgnsEff_struct*
_tmp562=(struct Cyc_Absyn_RgnsEff_struct*)_tmp561;if(_tmp562->tag != 26)goto _LL2D3;};
_LL2D2: goto _LL2D4;_LL2D3: _tmp563=_tmp498.f2;{struct Cyc_Absyn_AccessEff_struct*
_tmp564=(struct Cyc_Absyn_AccessEff_struct*)_tmp563;if(_tmp564->tag != 24)goto
_LL2D5;};_LL2D4: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmp12B4;Cyc_Tcutil_failure_reason=((
_tmp12B4="(effects don't unify)",_tag_dyneither(_tmp12B4,sizeof(char),22)));}
goto _LL29A;_LL2D5:;_LL2D6: goto _LL29A;_LL29A:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp5CB=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp5CC=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp5CB,_tmp5CC);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp5CD=x;union Cyc_Absyn_Constraint _tmp5CE;int _tmp5CF;
union Cyc_Absyn_Constraint _tmp5D0;void*_tmp5D1;union Cyc_Absyn_Constraint _tmp5D2;
union Cyc_Absyn_Constraint*_tmp5D3;_LL2EF: _tmp5CE=*_tmp5CD;if((_tmp5CE.No_constr).tag
!= 3)goto _LL2F1;_tmp5CF=(int)(_tmp5CE.No_constr).val;_LL2F0: return - 1;_LL2F1:
_tmp5D0=*_tmp5CD;if((_tmp5D0.Eq_constr).tag != 1)goto _LL2F3;_tmp5D1=(void*)(
_tmp5D0.Eq_constr).val;_LL2F2: {union Cyc_Absyn_Constraint*_tmp5D4=y;union Cyc_Absyn_Constraint
_tmp5D5;int _tmp5D6;union Cyc_Absyn_Constraint _tmp5D7;void*_tmp5D8;union Cyc_Absyn_Constraint
_tmp5D9;union Cyc_Absyn_Constraint*_tmp5DA;_LL2F6: _tmp5D5=*_tmp5D4;if((_tmp5D5.No_constr).tag
!= 3)goto _LL2F8;_tmp5D6=(int)(_tmp5D5.No_constr).val;_LL2F7: return 1;_LL2F8:
_tmp5D7=*_tmp5D4;if((_tmp5D7.Eq_constr).tag != 1)goto _LL2FA;_tmp5D8=(void*)(
_tmp5D7.Eq_constr).val;_LL2F9: return cmp(_tmp5D1,_tmp5D8);_LL2FA: _tmp5D9=*_tmp5D4;
if((_tmp5D9.Forward_constr).tag != 2)goto _LL2F5;_tmp5DA=(union Cyc_Absyn_Constraint*)(
_tmp5D9.Forward_constr).val;_LL2FB: {const char*_tmp12B7;void*_tmp12B6;(_tmp12B6=
0,Cyc_Tcutil_impos(((_tmp12B7="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp12B7,sizeof(char),40))),_tag_dyneither(_tmp12B6,sizeof(void*),
0)));}_LL2F5:;}_LL2F3: _tmp5D2=*_tmp5CD;if((_tmp5D2.Forward_constr).tag != 2)goto
_LL2EE;_tmp5D3=(union Cyc_Absyn_Constraint*)(_tmp5D2.Forward_constr).val;_LL2F4: {
const char*_tmp12BA;void*_tmp12B9;(_tmp12B9=0,Cyc_Tcutil_impos(((_tmp12BA="unify_conref: forward after compress",
_tag_dyneither(_tmp12BA,sizeof(char),37))),_tag_dyneither(_tmp12B9,sizeof(void*),
0)));}_LL2EE:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp5E0;struct Cyc_Absyn_Tqual _tmp5E1;void*_tmp5E2;
struct _tuple11*_tmp5DF=tqt1;_tmp5E0=*_tmp5DF;_tmp5E1=_tmp5E0.f1;_tmp5E2=_tmp5E0.f2;{
struct _tuple11 _tmp5E4;struct Cyc_Absyn_Tqual _tmp5E5;void*_tmp5E6;struct _tuple11*
_tmp5E3=tqt2;_tmp5E4=*_tmp5E3;_tmp5E5=_tmp5E4.f1;_tmp5E6=_tmp5E4.f2;{int _tmp5E7=
Cyc_Tcutil_tqual_cmp(_tmp5E1,_tmp5E5);if(_tmp5E7 != 0)return _tmp5E7;return Cyc_Tcutil_typecmp(
_tmp5E2,_tmp5E6);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5E8=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5E8 != 0)return _tmp5E8;{int _tmp5E9=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5E9 != 0)return _tmp5E9;{int _tmp5EA=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5EA != 0)return _tmp5EA;{int _tmp5EB=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5EB != 0)return _tmp5EB;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5EC=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5EC != 0)return _tmp5EC;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5ED=t;_LL2FD: {struct Cyc_Absyn_VoidType_struct*_tmp5EE=(struct
Cyc_Absyn_VoidType_struct*)_tmp5ED;if(_tmp5EE->tag != 0)goto _LL2FF;}_LL2FE: return
0;_LL2FF: {struct Cyc_Absyn_Evar_struct*_tmp5EF=(struct Cyc_Absyn_Evar_struct*)
_tmp5ED;if(_tmp5EF->tag != 1)goto _LL301;}_LL300: return 1;_LL301: {struct Cyc_Absyn_VarType_struct*
_tmp5F0=(struct Cyc_Absyn_VarType_struct*)_tmp5ED;if(_tmp5F0->tag != 2)goto _LL303;}
_LL302: return 2;_LL303: {struct Cyc_Absyn_DatatypeType_struct*_tmp5F1=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5ED;if(_tmp5F1->tag != 3)goto _LL305;}_LL304: return 3;_LL305: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5F2=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5ED;if(_tmp5F2->tag != 4)
goto _LL307;}_LL306: return 4;_LL307: {struct Cyc_Absyn_PointerType_struct*_tmp5F3=(
struct Cyc_Absyn_PointerType_struct*)_tmp5ED;if(_tmp5F3->tag != 5)goto _LL309;}
_LL308: return 5;_LL309: {struct Cyc_Absyn_IntType_struct*_tmp5F4=(struct Cyc_Absyn_IntType_struct*)
_tmp5ED;if(_tmp5F4->tag != 6)goto _LL30B;}_LL30A: return 6;_LL30B: {struct Cyc_Absyn_FloatType_struct*
_tmp5F5=(struct Cyc_Absyn_FloatType_struct*)_tmp5ED;if(_tmp5F5->tag != 7)goto
_LL30D;}_LL30C: return 7;_LL30D: {struct Cyc_Absyn_DoubleType_struct*_tmp5F6=(
struct Cyc_Absyn_DoubleType_struct*)_tmp5ED;if(_tmp5F6->tag != 8)goto _LL30F;}
_LL30E: return 8;_LL30F: {struct Cyc_Absyn_ArrayType_struct*_tmp5F7=(struct Cyc_Absyn_ArrayType_struct*)
_tmp5ED;if(_tmp5F7->tag != 9)goto _LL311;}_LL310: return 9;_LL311: {struct Cyc_Absyn_FnType_struct*
_tmp5F8=(struct Cyc_Absyn_FnType_struct*)_tmp5ED;if(_tmp5F8->tag != 10)goto _LL313;}
_LL312: return 10;_LL313: {struct Cyc_Absyn_TupleType_struct*_tmp5F9=(struct Cyc_Absyn_TupleType_struct*)
_tmp5ED;if(_tmp5F9->tag != 11)goto _LL315;}_LL314: return 11;_LL315: {struct Cyc_Absyn_AggrType_struct*
_tmp5FA=(struct Cyc_Absyn_AggrType_struct*)_tmp5ED;if(_tmp5FA->tag != 12)goto
_LL317;}_LL316: return 12;_LL317: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5FB=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp5ED;if(_tmp5FB->tag != 13)goto _LL319;}
_LL318: return 14;_LL319: {struct Cyc_Absyn_EnumType_struct*_tmp5FC=(struct Cyc_Absyn_EnumType_struct*)
_tmp5ED;if(_tmp5FC->tag != 14)goto _LL31B;}_LL31A: return 16;_LL31B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp5FD=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5ED;if(_tmp5FD->tag != 15)goto
_LL31D;}_LL31C: return 17;_LL31D: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5FE=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5ED;if(_tmp5FE->tag != 16)goto _LL31F;}
_LL31E: return 18;_LL31F: {struct Cyc_Absyn_TypedefType_struct*_tmp5FF=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5ED;if(_tmp5FF->tag != 18)goto _LL321;}_LL320: return 19;_LL321: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp600=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp5ED;if(_tmp600->tag != 22)goto
_LL323;}_LL322: return 20;_LL323: {struct Cyc_Absyn_HeapRgn_struct*_tmp601=(struct
Cyc_Absyn_HeapRgn_struct*)_tmp5ED;if(_tmp601->tag != 21)goto _LL325;}_LL324: return
21;_LL325: {struct Cyc_Absyn_AccessEff_struct*_tmp602=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5ED;if(_tmp602->tag != 24)goto _LL327;}_LL326: return 22;_LL327: {struct Cyc_Absyn_JoinEff_struct*
_tmp603=(struct Cyc_Absyn_JoinEff_struct*)_tmp5ED;if(_tmp603->tag != 25)goto _LL329;}
_LL328: return 23;_LL329: {struct Cyc_Absyn_RgnsEff_struct*_tmp604=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5ED;if(_tmp604->tag != 26)goto _LL32B;}_LL32A: return 24;_LL32B: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp605=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp5ED;if(_tmp605->tag != 23)goto
_LL32D;}_LL32C: return 25;_LL32D: {struct Cyc_Absyn_TagType_struct*_tmp606=(struct
Cyc_Absyn_TagType_struct*)_tmp5ED;if(_tmp606->tag != 20)goto _LL32F;}_LL32E: return
26;_LL32F: {struct Cyc_Absyn_DynRgnType_struct*_tmp607=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5ED;if(_tmp607->tag != 17)goto _LL331;}_LL330: return 27;_LL331: {struct Cyc_Absyn_ValueofType_struct*
_tmp608=(struct Cyc_Absyn_ValueofType_struct*)_tmp5ED;if(_tmp608->tag != 19)goto
_LL333;}_LL332: return 28;_LL333: {struct Cyc_Absyn_TypeDeclType_struct*_tmp609=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp5ED;if(_tmp609->tag != 27)goto _LL2FC;}
_LL334: return 29;_LL2FC:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp60A=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp60A != 0)
return _tmp60A;{struct _tuple0 _tmp12BB;struct _tuple0 _tmp60C=(_tmp12BB.f1=t2,((
_tmp12BB.f2=t1,_tmp12BB)));void*_tmp60D;void*_tmp60F;void*_tmp611;struct Cyc_Absyn_Tvar*
_tmp613;void*_tmp614;struct Cyc_Absyn_Tvar*_tmp616;void*_tmp617;struct Cyc_Absyn_AggrInfo
_tmp619;union Cyc_Absyn_AggrInfoU _tmp61A;struct Cyc_List_List*_tmp61B;void*_tmp61C;
struct Cyc_Absyn_AggrInfo _tmp61E;union Cyc_Absyn_AggrInfoU _tmp61F;struct Cyc_List_List*
_tmp620;void*_tmp621;struct _tuple2*_tmp623;void*_tmp624;struct _tuple2*_tmp626;
void*_tmp627;struct Cyc_List_List*_tmp629;void*_tmp62A;struct Cyc_List_List*
_tmp62C;void*_tmp62D;struct Cyc_Absyn_DatatypeInfo _tmp62F;union Cyc_Absyn_DatatypeInfoU
_tmp630;struct Cyc_Absyn_Datatypedecl**_tmp631;struct Cyc_Absyn_Datatypedecl*
_tmp632;struct Cyc_List_List*_tmp633;void*_tmp634;struct Cyc_Absyn_DatatypeInfo
_tmp636;union Cyc_Absyn_DatatypeInfoU _tmp637;struct Cyc_Absyn_Datatypedecl**
_tmp638;struct Cyc_Absyn_Datatypedecl*_tmp639;struct Cyc_List_List*_tmp63A;void*
_tmp63B;struct Cyc_Absyn_DatatypeFieldInfo _tmp63D;union Cyc_Absyn_DatatypeFieldInfoU
_tmp63E;struct _tuple3 _tmp63F;struct Cyc_Absyn_Datatypedecl*_tmp640;struct Cyc_Absyn_Datatypefield*
_tmp641;struct Cyc_List_List*_tmp642;void*_tmp643;struct Cyc_Absyn_DatatypeFieldInfo
_tmp645;union Cyc_Absyn_DatatypeFieldInfoU _tmp646;struct _tuple3 _tmp647;struct Cyc_Absyn_Datatypedecl*
_tmp648;struct Cyc_Absyn_Datatypefield*_tmp649;struct Cyc_List_List*_tmp64A;void*
_tmp64B;struct Cyc_Absyn_PtrInfo _tmp64D;void*_tmp64E;struct Cyc_Absyn_Tqual _tmp64F;
struct Cyc_Absyn_PtrAtts _tmp650;void*_tmp651;union Cyc_Absyn_Constraint*_tmp652;
union Cyc_Absyn_Constraint*_tmp653;union Cyc_Absyn_Constraint*_tmp654;void*_tmp655;
struct Cyc_Absyn_PtrInfo _tmp657;void*_tmp658;struct Cyc_Absyn_Tqual _tmp659;struct
Cyc_Absyn_PtrAtts _tmp65A;void*_tmp65B;union Cyc_Absyn_Constraint*_tmp65C;union Cyc_Absyn_Constraint*
_tmp65D;union Cyc_Absyn_Constraint*_tmp65E;void*_tmp65F;enum Cyc_Absyn_Sign
_tmp661;enum Cyc_Absyn_Size_of _tmp662;void*_tmp663;enum Cyc_Absyn_Sign _tmp665;
enum Cyc_Absyn_Size_of _tmp666;void*_tmp667;void*_tmp669;void*_tmp66B;int _tmp66D;
void*_tmp66E;int _tmp670;void*_tmp671;struct Cyc_Absyn_ArrayInfo _tmp673;void*
_tmp674;struct Cyc_Absyn_Tqual _tmp675;struct Cyc_Absyn_Exp*_tmp676;union Cyc_Absyn_Constraint*
_tmp677;void*_tmp678;struct Cyc_Absyn_ArrayInfo _tmp67A;void*_tmp67B;struct Cyc_Absyn_Tqual
_tmp67C;struct Cyc_Absyn_Exp*_tmp67D;union Cyc_Absyn_Constraint*_tmp67E;void*
_tmp67F;struct Cyc_Absyn_FnInfo _tmp681;struct Cyc_List_List*_tmp682;struct Cyc_Core_Opt*
_tmp683;struct Cyc_Absyn_Tqual _tmp684;void*_tmp685;struct Cyc_List_List*_tmp686;
int _tmp687;struct Cyc_Absyn_VarargInfo*_tmp688;struct Cyc_List_List*_tmp689;struct
Cyc_List_List*_tmp68A;void*_tmp68B;struct Cyc_Absyn_FnInfo _tmp68D;struct Cyc_List_List*
_tmp68E;struct Cyc_Core_Opt*_tmp68F;struct Cyc_Absyn_Tqual _tmp690;void*_tmp691;
struct Cyc_List_List*_tmp692;int _tmp693;struct Cyc_Absyn_VarargInfo*_tmp694;struct
Cyc_List_List*_tmp695;struct Cyc_List_List*_tmp696;void*_tmp697;struct Cyc_List_List*
_tmp699;void*_tmp69A;struct Cyc_List_List*_tmp69C;void*_tmp69D;enum Cyc_Absyn_AggrKind
_tmp69F;struct Cyc_List_List*_tmp6A0;void*_tmp6A1;enum Cyc_Absyn_AggrKind _tmp6A3;
struct Cyc_List_List*_tmp6A4;void*_tmp6A5;void*_tmp6A7;void*_tmp6A8;void*_tmp6AA;
void*_tmp6AB;void*_tmp6AD;void*_tmp6AE;void*_tmp6AF;void*_tmp6B1;void*_tmp6B2;
void*_tmp6B3;void*_tmp6B5;void*_tmp6B6;void*_tmp6B8;void*_tmp6B9;struct Cyc_Absyn_Exp*
_tmp6BB;void*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BE;void*_tmp6BF;void*_tmp6C1;void*
_tmp6C3;void*_tmp6C5;void*_tmp6C7;void*_tmp6C9;_LL336: _tmp60D=_tmp60C.f1;{struct
Cyc_Absyn_Evar_struct*_tmp60E=(struct Cyc_Absyn_Evar_struct*)_tmp60D;if(_tmp60E->tag
!= 1)goto _LL338;};_tmp60F=_tmp60C.f2;{struct Cyc_Absyn_Evar_struct*_tmp610=(
struct Cyc_Absyn_Evar_struct*)_tmp60F;if(_tmp610->tag != 1)goto _LL338;};_LL337: {
const char*_tmp12BE;void*_tmp12BD;(_tmp12BD=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12BE="typecmp: can only compare closed types",
_tag_dyneither(_tmp12BE,sizeof(char),39))),_tag_dyneither(_tmp12BD,sizeof(void*),
0)));}_LL338: _tmp611=_tmp60C.f1;{struct Cyc_Absyn_VarType_struct*_tmp612=(struct
Cyc_Absyn_VarType_struct*)_tmp611;if(_tmp612->tag != 2)goto _LL33A;else{_tmp613=
_tmp612->f1;}};_tmp614=_tmp60C.f2;{struct Cyc_Absyn_VarType_struct*_tmp615=(
struct Cyc_Absyn_VarType_struct*)_tmp614;if(_tmp615->tag != 2)goto _LL33A;else{
_tmp616=_tmp615->f1;}};_LL339: return Cyc_Core_intcmp(_tmp616->identity,_tmp613->identity);
_LL33A: _tmp617=_tmp60C.f1;{struct Cyc_Absyn_AggrType_struct*_tmp618=(struct Cyc_Absyn_AggrType_struct*)
_tmp617;if(_tmp618->tag != 12)goto _LL33C;else{_tmp619=_tmp618->f1;_tmp61A=_tmp619.aggr_info;
_tmp61B=_tmp619.targs;}};_tmp61C=_tmp60C.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp61D=(struct Cyc_Absyn_AggrType_struct*)_tmp61C;if(_tmp61D->tag != 12)goto
_LL33C;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61E.aggr_info;_tmp620=_tmp61E.targs;}};
_LL33B: {struct _tuple2*_tmp6CE;struct _tuple12 _tmp6CD=Cyc_Absyn_aggr_kinded_name(
_tmp61A);_tmp6CE=_tmp6CD.f2;{struct _tuple2*_tmp6D0;struct _tuple12 _tmp6CF=Cyc_Absyn_aggr_kinded_name(
_tmp61F);_tmp6D0=_tmp6CF.f2;{int _tmp6D1=Cyc_Absyn_qvar_cmp(_tmp6CE,_tmp6D0);if(
_tmp6D1 != 0)return _tmp6D1;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp61B,_tmp620);}};};}
_LL33C: _tmp621=_tmp60C.f1;{struct Cyc_Absyn_EnumType_struct*_tmp622=(struct Cyc_Absyn_EnumType_struct*)
_tmp621;if(_tmp622->tag != 14)goto _LL33E;else{_tmp623=_tmp622->f1;}};_tmp624=
_tmp60C.f2;{struct Cyc_Absyn_EnumType_struct*_tmp625=(struct Cyc_Absyn_EnumType_struct*)
_tmp624;if(_tmp625->tag != 14)goto _LL33E;else{_tmp626=_tmp625->f1;}};_LL33D:
return Cyc_Absyn_qvar_cmp(_tmp623,_tmp626);_LL33E: _tmp627=_tmp60C.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp628=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp627;if(_tmp628->tag != 15)goto
_LL340;else{_tmp629=_tmp628->f1;}};_tmp62A=_tmp60C.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp62B=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp62A;if(_tmp62B->tag != 15)goto
_LL340;else{_tmp62C=_tmp62B->f1;}};_LL33F: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp629,_tmp62C);_LL340: _tmp62D=_tmp60C.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp62E=(struct Cyc_Absyn_DatatypeType_struct*)_tmp62D;if(_tmp62E->tag != 3)goto
_LL342;else{_tmp62F=_tmp62E->f1;_tmp630=_tmp62F.datatype_info;if((_tmp630.KnownDatatype).tag
!= 2)goto _LL342;_tmp631=(struct Cyc_Absyn_Datatypedecl**)(_tmp630.KnownDatatype).val;
_tmp632=*_tmp631;_tmp633=_tmp62F.targs;}};_tmp634=_tmp60C.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp635=(struct Cyc_Absyn_DatatypeType_struct*)_tmp634;if(_tmp635->tag != 3)goto
_LL342;else{_tmp636=_tmp635->f1;_tmp637=_tmp636.datatype_info;if((_tmp637.KnownDatatype).tag
!= 2)goto _LL342;_tmp638=(struct Cyc_Absyn_Datatypedecl**)(_tmp637.KnownDatatype).val;
_tmp639=*_tmp638;_tmp63A=_tmp636.targs;}};_LL341: if(_tmp639 == _tmp632)return 0;{
int _tmp6D2=Cyc_Absyn_qvar_cmp(_tmp639->name,_tmp632->name);if(_tmp6D2 != 0)return
_tmp6D2;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp63A,_tmp633);};_LL342: _tmp63B=
_tmp60C.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp63C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp63B;if(_tmp63C->tag != 4)goto _LL344;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63D.field_info;
if((_tmp63E.KnownDatatypefield).tag != 2)goto _LL344;_tmp63F=(struct _tuple3)(
_tmp63E.KnownDatatypefield).val;_tmp640=_tmp63F.f1;_tmp641=_tmp63F.f2;_tmp642=
_tmp63D.targs;}};_tmp643=_tmp60C.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp644=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp643;if(_tmp644->tag != 4)
goto _LL344;else{_tmp645=_tmp644->f1;_tmp646=_tmp645.field_info;if((_tmp646.KnownDatatypefield).tag
!= 2)goto _LL344;_tmp647=(struct _tuple3)(_tmp646.KnownDatatypefield).val;_tmp648=
_tmp647.f1;_tmp649=_tmp647.f2;_tmp64A=_tmp645.targs;}};_LL343: if(_tmp648 == 
_tmp640)return 0;{int _tmp6D3=Cyc_Absyn_qvar_cmp(_tmp640->name,_tmp648->name);if(
_tmp6D3 != 0)return _tmp6D3;{int _tmp6D4=Cyc_Absyn_qvar_cmp(_tmp641->name,_tmp649->name);
if(_tmp6D4 != 0)return _tmp6D4;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp64A,_tmp642);};};
_LL344: _tmp64B=_tmp60C.f1;{struct Cyc_Absyn_PointerType_struct*_tmp64C=(struct Cyc_Absyn_PointerType_struct*)
_tmp64B;if(_tmp64C->tag != 5)goto _LL346;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64D.elt_typ;
_tmp64F=_tmp64D.elt_tq;_tmp650=_tmp64D.ptr_atts;_tmp651=_tmp650.rgn;_tmp652=
_tmp650.nullable;_tmp653=_tmp650.bounds;_tmp654=_tmp650.zero_term;}};_tmp655=
_tmp60C.f2;{struct Cyc_Absyn_PointerType_struct*_tmp656=(struct Cyc_Absyn_PointerType_struct*)
_tmp655;if(_tmp656->tag != 5)goto _LL346;else{_tmp657=_tmp656->f1;_tmp658=_tmp657.elt_typ;
_tmp659=_tmp657.elt_tq;_tmp65A=_tmp657.ptr_atts;_tmp65B=_tmp65A.rgn;_tmp65C=
_tmp65A.nullable;_tmp65D=_tmp65A.bounds;_tmp65E=_tmp65A.zero_term;}};_LL345: {
int _tmp6D5=Cyc_Tcutil_typecmp(_tmp658,_tmp64E);if(_tmp6D5 != 0)return _tmp6D5;{int
_tmp6D6=Cyc_Tcutil_typecmp(_tmp65B,_tmp651);if(_tmp6D6 != 0)return _tmp6D6;{int
_tmp6D7=Cyc_Tcutil_tqual_cmp(_tmp659,_tmp64F);if(_tmp6D7 != 0)return _tmp6D7;{int
_tmp6D8=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp65D,_tmp653);if(_tmp6D8 != 0)
return _tmp6D8;{int _tmp6D9=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp65E,
_tmp654);if(_tmp6D9 != 0)return _tmp6D9;{void*_tmp6DA=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp65D);_LL36B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6DB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6DA;if(_tmp6DB->tag != 0)goto
_LL36D;}_LL36C: return 0;_LL36D:;_LL36E: goto _LL36A;_LL36A:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp65C,_tmp652);};};};};}_LL346: _tmp65F=_tmp60C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp660=(struct Cyc_Absyn_IntType_struct*)_tmp65F;if(_tmp660->tag != 6)goto _LL348;
else{_tmp661=_tmp660->f1;_tmp662=_tmp660->f2;}};_tmp663=_tmp60C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp664=(struct Cyc_Absyn_IntType_struct*)_tmp663;if(_tmp664->tag != 6)goto _LL348;
else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;}};_LL347: if(_tmp665 != _tmp661)
return Cyc_Core_intcmp((int)((unsigned int)_tmp665),(int)((unsigned int)_tmp661));
if(_tmp666 != _tmp662)return Cyc_Core_intcmp((int)((unsigned int)_tmp666),(int)((
unsigned int)_tmp662));return 0;_LL348: _tmp667=_tmp60C.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp668=(struct Cyc_Absyn_FloatType_struct*)_tmp667;if(_tmp668->tag != 7)goto
_LL34A;};_tmp669=_tmp60C.f2;{struct Cyc_Absyn_FloatType_struct*_tmp66A=(struct Cyc_Absyn_FloatType_struct*)
_tmp669;if(_tmp66A->tag != 7)goto _LL34A;};_LL349: return 0;_LL34A: _tmp66B=_tmp60C.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp66C=(struct Cyc_Absyn_DoubleType_struct*)
_tmp66B;if(_tmp66C->tag != 8)goto _LL34C;else{_tmp66D=_tmp66C->f1;}};_tmp66E=
_tmp60C.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp66F=(struct Cyc_Absyn_DoubleType_struct*)
_tmp66E;if(_tmp66F->tag != 8)goto _LL34C;else{_tmp670=_tmp66F->f1;}};_LL34B: if(
_tmp66D == _tmp670)return 0;else{if(_tmp66D)return - 1;else{return 1;}}_LL34C: _tmp671=
_tmp60C.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp672=(struct Cyc_Absyn_ArrayType_struct*)
_tmp671;if(_tmp672->tag != 9)goto _LL34E;else{_tmp673=_tmp672->f1;_tmp674=_tmp673.elt_type;
_tmp675=_tmp673.tq;_tmp676=_tmp673.num_elts;_tmp677=_tmp673.zero_term;}};_tmp678=
_tmp60C.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp679=(struct Cyc_Absyn_ArrayType_struct*)
_tmp678;if(_tmp679->tag != 9)goto _LL34E;else{_tmp67A=_tmp679->f1;_tmp67B=_tmp67A.elt_type;
_tmp67C=_tmp67A.tq;_tmp67D=_tmp67A.num_elts;_tmp67E=_tmp67A.zero_term;}};_LL34D: {
int _tmp6DC=Cyc_Tcutil_tqual_cmp(_tmp67C,_tmp675);if(_tmp6DC != 0)return _tmp6DC;{
int _tmp6DD=Cyc_Tcutil_typecmp(_tmp67B,_tmp674);if(_tmp6DD != 0)return _tmp6DD;{int
_tmp6DE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp677,_tmp67E);if(_tmp6DE != 0)
return _tmp6DE;if(_tmp676 == _tmp67D)return 0;if(_tmp676 == 0  || _tmp67D == 0){const
char*_tmp12C1;void*_tmp12C0;(_tmp12C0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C1="missing expression in array index",
_tag_dyneither(_tmp12C1,sizeof(char),34))),_tag_dyneither(_tmp12C0,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp676,_tmp67D);};};}_LL34E: _tmp67F=_tmp60C.f1;{struct Cyc_Absyn_FnType_struct*
_tmp680=(struct Cyc_Absyn_FnType_struct*)_tmp67F;if(_tmp680->tag != 10)goto _LL350;
else{_tmp681=_tmp680->f1;_tmp682=_tmp681.tvars;_tmp683=_tmp681.effect;_tmp684=
_tmp681.ret_tqual;_tmp685=_tmp681.ret_typ;_tmp686=_tmp681.args;_tmp687=_tmp681.c_varargs;
_tmp688=_tmp681.cyc_varargs;_tmp689=_tmp681.rgn_po;_tmp68A=_tmp681.attributes;}};
_tmp68B=_tmp60C.f2;{struct Cyc_Absyn_FnType_struct*_tmp68C=(struct Cyc_Absyn_FnType_struct*)
_tmp68B;if(_tmp68C->tag != 10)goto _LL350;else{_tmp68D=_tmp68C->f1;_tmp68E=_tmp68D.tvars;
_tmp68F=_tmp68D.effect;_tmp690=_tmp68D.ret_tqual;_tmp691=_tmp68D.ret_typ;_tmp692=
_tmp68D.args;_tmp693=_tmp68D.c_varargs;_tmp694=_tmp68D.cyc_varargs;_tmp695=
_tmp68D.rgn_po;_tmp696=_tmp68D.attributes;}};_LL34F: {const char*_tmp12C4;void*
_tmp12C3;(_tmp12C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C4="typecmp: function types not handled",
_tag_dyneither(_tmp12C4,sizeof(char),36))),_tag_dyneither(_tmp12C3,sizeof(void*),
0)));}_LL350: _tmp697=_tmp60C.f1;{struct Cyc_Absyn_TupleType_struct*_tmp698=(
struct Cyc_Absyn_TupleType_struct*)_tmp697;if(_tmp698->tag != 11)goto _LL352;else{
_tmp699=_tmp698->f1;}};_tmp69A=_tmp60C.f2;{struct Cyc_Absyn_TupleType_struct*
_tmp69B=(struct Cyc_Absyn_TupleType_struct*)_tmp69A;if(_tmp69B->tag != 11)goto
_LL352;else{_tmp69C=_tmp69B->f1;}};_LL351: return((int(*)(int(*cmp)(struct
_tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_tqual_type_cmp,_tmp69C,_tmp699);_LL352: _tmp69D=_tmp60C.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp69E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp69D;if(_tmp69E->tag != 13)goto
_LL354;else{_tmp69F=_tmp69E->f1;_tmp6A0=_tmp69E->f2;}};_tmp6A1=_tmp60C.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp6A2=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6A1;if(_tmp6A2->tag != 13)goto _LL354;else{_tmp6A3=_tmp6A2->f1;_tmp6A4=_tmp6A2->f2;}};
_LL353: if(_tmp6A3 != _tmp69F){if(_tmp6A3 == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp6A4,_tmp6A0);_LL354: _tmp6A5=_tmp60C.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A6=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A5;if(_tmp6A6->tag != 16)goto
_LL356;else{_tmp6A7=(void*)_tmp6A6->f1;}};_tmp6A8=_tmp60C.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A9=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A8;if(_tmp6A9->tag != 16)goto
_LL356;else{_tmp6AA=(void*)_tmp6A9->f1;}};_LL355: return Cyc_Tcutil_typecmp(
_tmp6A7,_tmp6AA);_LL356: _tmp6AB=_tmp60C.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp6AC=(struct Cyc_Absyn_DynRgnType_struct*)_tmp6AB;if(_tmp6AC->tag != 17)goto
_LL358;else{_tmp6AD=(void*)_tmp6AC->f1;_tmp6AE=(void*)_tmp6AC->f2;}};_tmp6AF=
_tmp60C.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp6B0=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp6AF;if(_tmp6B0->tag != 17)goto _LL358;else{_tmp6B1=(void*)_tmp6B0->f1;_tmp6B2=(
void*)_tmp6B0->f2;}};_LL357: {int _tmp6E3=Cyc_Tcutil_typecmp(_tmp6AD,_tmp6B1);if(
_tmp6E3 != 0)return _tmp6E3;else{return Cyc_Tcutil_typecmp(_tmp6AE,_tmp6B2);}}
_LL358: _tmp6B3=_tmp60C.f1;{struct Cyc_Absyn_TagType_struct*_tmp6B4=(struct Cyc_Absyn_TagType_struct*)
_tmp6B3;if(_tmp6B4->tag != 20)goto _LL35A;else{_tmp6B5=(void*)_tmp6B4->f1;}};
_tmp6B6=_tmp60C.f2;{struct Cyc_Absyn_TagType_struct*_tmp6B7=(struct Cyc_Absyn_TagType_struct*)
_tmp6B6;if(_tmp6B7->tag != 20)goto _LL35A;else{_tmp6B8=(void*)_tmp6B7->f1;}};
_LL359: return Cyc_Tcutil_typecmp(_tmp6B5,_tmp6B8);_LL35A: _tmp6B9=_tmp60C.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp6BA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6B9;if(_tmp6BA->tag != 19)goto _LL35C;else{_tmp6BB=_tmp6BA->f1;}};_tmp6BC=
_tmp60C.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp6BD=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6BC;if(_tmp6BD->tag != 19)goto _LL35C;else{_tmp6BE=_tmp6BD->f1;}};_LL35B:
return Cyc_Evexp_const_exp_cmp(_tmp6BB,_tmp6BE);_LL35C: _tmp6BF=_tmp60C.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp6C0=(struct Cyc_Absyn_JoinEff_struct*)_tmp6BF;if(
_tmp6C0->tag != 25)goto _LL35E;};_LL35D: goto _LL35F;_LL35E: _tmp6C1=_tmp60C.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp6C2=(struct Cyc_Absyn_JoinEff_struct*)_tmp6C1;
if(_tmp6C2->tag != 25)goto _LL360;};_LL35F: goto _LL361;_LL360: _tmp6C3=_tmp60C.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp6C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6C3;if(_tmp6C4->tag != 24)goto _LL362;};_LL361: goto _LL363;_LL362: _tmp6C5=
_tmp60C.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp6C6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6C5;if(_tmp6C6->tag != 26)goto _LL364;};_LL363: goto _LL365;_LL364: _tmp6C7=
_tmp60C.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp6C8=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6C7;if(_tmp6C8->tag != 26)goto _LL366;};_LL365: goto _LL367;_LL366: _tmp6C9=
_tmp60C.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp6CA=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6C9;if(_tmp6CA->tag != 24)goto _LL368;};_LL367: {const char*_tmp12C7;void*
_tmp12C6;(_tmp12C6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C7="typecmp: effects not handled",_tag_dyneither(
_tmp12C7,sizeof(char),29))),_tag_dyneither(_tmp12C6,sizeof(void*),0)));}_LL368:;
_LL369: {const char*_tmp12CA;void*_tmp12C9;(_tmp12C9=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CA="Unmatched case in typecmp",
_tag_dyneither(_tmp12CA,sizeof(char),26))),_tag_dyneither(_tmp12C9,sizeof(void*),
0)));}_LL335:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6E8=Cyc_Tcutil_compress(
t);_LL370: {struct Cyc_Absyn_IntType_struct*_tmp6E9=(struct Cyc_Absyn_IntType_struct*)
_tmp6E8;if(_tmp6E9->tag != 6)goto _LL372;}_LL371: goto _LL373;_LL372: {struct Cyc_Absyn_FloatType_struct*
_tmp6EA=(struct Cyc_Absyn_FloatType_struct*)_tmp6E8;if(_tmp6EA->tag != 7)goto
_LL374;}_LL373: goto _LL375;_LL374: {struct Cyc_Absyn_DoubleType_struct*_tmp6EB=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6E8;if(_tmp6EB->tag != 8)goto _LL376;}
_LL375: goto _LL377;_LL376: {struct Cyc_Absyn_EnumType_struct*_tmp6EC=(struct Cyc_Absyn_EnumType_struct*)
_tmp6E8;if(_tmp6EC->tag != 14)goto _LL378;}_LL377: goto _LL379;_LL378: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp6ED=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp6E8;if(_tmp6ED->tag != 15)goto
_LL37A;}_LL379: return 1;_LL37A:;_LL37B: return 0;_LL36F:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp12CB;struct _tuple0 _tmp6EF=(_tmp12CB.f1=t1,((_tmp12CB.f2=t2,_tmp12CB)));
void*_tmp6F0;int _tmp6F2;void*_tmp6F3;int _tmp6F5;void*_tmp6F6;void*_tmp6F8;void*
_tmp6FA;void*_tmp6FC;void*_tmp6FE;void*_tmp700;void*_tmp702;void*_tmp704;void*
_tmp706;void*_tmp708;void*_tmp70A;enum Cyc_Absyn_Size_of _tmp70C;void*_tmp70D;
enum Cyc_Absyn_Size_of _tmp70F;void*_tmp710;enum Cyc_Absyn_Size_of _tmp712;void*
_tmp713;enum Cyc_Absyn_Size_of _tmp715;void*_tmp716;enum Cyc_Absyn_Size_of _tmp718;
void*_tmp719;enum Cyc_Absyn_Size_of _tmp71B;void*_tmp71C;enum Cyc_Absyn_Size_of
_tmp71E;void*_tmp71F;enum Cyc_Absyn_Size_of _tmp721;void*_tmp722;void*_tmp724;
enum Cyc_Absyn_Size_of _tmp726;void*_tmp727;void*_tmp729;enum Cyc_Absyn_Size_of
_tmp72B;void*_tmp72C;enum Cyc_Absyn_Size_of _tmp72E;void*_tmp72F;enum Cyc_Absyn_Size_of
_tmp731;void*_tmp732;enum Cyc_Absyn_Size_of _tmp734;void*_tmp735;void*_tmp737;
enum Cyc_Absyn_Size_of _tmp739;void*_tmp73A;enum Cyc_Absyn_Size_of _tmp73C;void*
_tmp73D;enum Cyc_Absyn_Size_of _tmp73F;void*_tmp740;enum Cyc_Absyn_Size_of _tmp742;
void*_tmp743;enum Cyc_Absyn_Size_of _tmp745;void*_tmp746;enum Cyc_Absyn_Size_of
_tmp748;void*_tmp749;enum Cyc_Absyn_Size_of _tmp74B;void*_tmp74C;void*_tmp74E;
enum Cyc_Absyn_Size_of _tmp750;_LL37D: _tmp6F0=_tmp6EF.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6F1=(struct Cyc_Absyn_DoubleType_struct*)_tmp6F0;if(_tmp6F1->tag != 8)goto
_LL37F;else{_tmp6F2=_tmp6F1->f1;}};_tmp6F3=_tmp6EF.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp6F4=(struct Cyc_Absyn_DoubleType_struct*)_tmp6F3;if(_tmp6F4->tag != 8)goto
_LL37F;else{_tmp6F5=_tmp6F4->f1;}};_LL37E: return !_tmp6F5  && _tmp6F2;_LL37F:
_tmp6F6=_tmp6EF.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6F7=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6F6;if(_tmp6F7->tag != 8)goto _LL381;};_tmp6F8=_tmp6EF.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6F9=(struct Cyc_Absyn_FloatType_struct*)_tmp6F8;if(_tmp6F9->tag != 7)goto
_LL381;};_LL380: goto _LL382;_LL381: _tmp6FA=_tmp6EF.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6FB=(struct Cyc_Absyn_DoubleType_struct*)_tmp6FA;if(_tmp6FB->tag != 8)goto
_LL383;};_tmp6FC=_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp6FD=(struct Cyc_Absyn_IntType_struct*)
_tmp6FC;if(_tmp6FD->tag != 6)goto _LL383;};_LL382: goto _LL384;_LL383: _tmp6FE=
_tmp6EF.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6FF=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6FE;if(_tmp6FF->tag != 8)goto _LL385;};_tmp700=_tmp6EF.f2;{struct Cyc_Absyn_TagType_struct*
_tmp701=(struct Cyc_Absyn_TagType_struct*)_tmp700;if(_tmp701->tag != 20)goto _LL385;};
_LL384: goto _LL386;_LL385: _tmp702=_tmp6EF.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp703=(struct Cyc_Absyn_FloatType_struct*)_tmp702;if(_tmp703->tag != 7)goto
_LL387;};_tmp704=_tmp6EF.f2;{struct Cyc_Absyn_TagType_struct*_tmp705=(struct Cyc_Absyn_TagType_struct*)
_tmp704;if(_tmp705->tag != 20)goto _LL387;};_LL386: goto _LL388;_LL387: _tmp706=
_tmp6EF.f1;{struct Cyc_Absyn_FloatType_struct*_tmp707=(struct Cyc_Absyn_FloatType_struct*)
_tmp706;if(_tmp707->tag != 7)goto _LL389;};_tmp708=_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*
_tmp709=(struct Cyc_Absyn_IntType_struct*)_tmp708;if(_tmp709->tag != 6)goto _LL389;};
_LL388: return 1;_LL389: _tmp70A=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*_tmp70B=(
struct Cyc_Absyn_IntType_struct*)_tmp70A;if(_tmp70B->tag != 6)goto _LL38B;else{
_tmp70C=_tmp70B->f2;if(_tmp70C != Cyc_Absyn_LongLong_sz)goto _LL38B;}};_tmp70D=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp70E=(struct Cyc_Absyn_IntType_struct*)
_tmp70D;if(_tmp70E->tag != 6)goto _LL38B;else{_tmp70F=_tmp70E->f2;if(_tmp70F != Cyc_Absyn_LongLong_sz)
goto _LL38B;}};_LL38A: return 0;_LL38B: _tmp710=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp711=(struct Cyc_Absyn_IntType_struct*)_tmp710;if(_tmp711->tag != 6)goto _LL38D;
else{_tmp712=_tmp711->f2;if(_tmp712 != Cyc_Absyn_LongLong_sz)goto _LL38D;}};_LL38C:
return 1;_LL38D: _tmp713=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*_tmp714=(
struct Cyc_Absyn_IntType_struct*)_tmp713;if(_tmp714->tag != 6)goto _LL38F;else{
_tmp715=_tmp714->f2;if(_tmp715 != Cyc_Absyn_Long_sz)goto _LL38F;}};_tmp716=_tmp6EF.f2;{
struct Cyc_Absyn_IntType_struct*_tmp717=(struct Cyc_Absyn_IntType_struct*)_tmp716;
if(_tmp717->tag != 6)goto _LL38F;else{_tmp718=_tmp717->f2;if(_tmp718 != Cyc_Absyn_Int_sz)
goto _LL38F;}};_LL38E: goto _LL390;_LL38F: _tmp719=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp71A=(struct Cyc_Absyn_IntType_struct*)_tmp719;if(_tmp71A->tag != 6)goto _LL391;
else{_tmp71B=_tmp71A->f2;if(_tmp71B != Cyc_Absyn_Int_sz)goto _LL391;}};_tmp71C=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp71D=(struct Cyc_Absyn_IntType_struct*)
_tmp71C;if(_tmp71D->tag != 6)goto _LL391;else{_tmp71E=_tmp71D->f2;if(_tmp71E != Cyc_Absyn_Long_sz)
goto _LL391;}};_LL390: return 0;_LL391: _tmp71F=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp720=(struct Cyc_Absyn_IntType_struct*)_tmp71F;if(_tmp720->tag != 6)goto _LL393;
else{_tmp721=_tmp720->f2;if(_tmp721 != Cyc_Absyn_Long_sz)goto _LL393;}};_tmp722=
_tmp6EF.f2;{struct Cyc_Absyn_FloatType_struct*_tmp723=(struct Cyc_Absyn_FloatType_struct*)
_tmp722;if(_tmp723->tag != 7)goto _LL393;};_LL392: goto _LL394;_LL393: _tmp724=
_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*_tmp725=(struct Cyc_Absyn_IntType_struct*)
_tmp724;if(_tmp725->tag != 6)goto _LL395;else{_tmp726=_tmp725->f2;if(_tmp726 != Cyc_Absyn_Int_sz)
goto _LL395;}};_tmp727=_tmp6EF.f2;{struct Cyc_Absyn_FloatType_struct*_tmp728=(
struct Cyc_Absyn_FloatType_struct*)_tmp727;if(_tmp728->tag != 7)goto _LL395;};
_LL394: goto _LL396;_LL395: _tmp729=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp72A=(struct Cyc_Absyn_IntType_struct*)_tmp729;if(_tmp72A->tag != 6)goto _LL397;
else{_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Long_sz)goto _LL397;}};_tmp72C=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp72D=(struct Cyc_Absyn_IntType_struct*)
_tmp72C;if(_tmp72D->tag != 6)goto _LL397;else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Short_sz)
goto _LL397;}};_LL396: goto _LL398;_LL397: _tmp72F=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp730=(struct Cyc_Absyn_IntType_struct*)_tmp72F;if(_tmp730->tag != 6)goto _LL399;
else{_tmp731=_tmp730->f2;if(_tmp731 != Cyc_Absyn_Int_sz)goto _LL399;}};_tmp732=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp733=(struct Cyc_Absyn_IntType_struct*)
_tmp732;if(_tmp733->tag != 6)goto _LL399;else{_tmp734=_tmp733->f2;if(_tmp734 != Cyc_Absyn_Short_sz)
goto _LL399;}};_LL398: goto _LL39A;_LL399: _tmp735=_tmp6EF.f1;{struct Cyc_Absyn_TagType_struct*
_tmp736=(struct Cyc_Absyn_TagType_struct*)_tmp735;if(_tmp736->tag != 20)goto _LL39B;};
_tmp737=_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp738=(struct Cyc_Absyn_IntType_struct*)
_tmp737;if(_tmp738->tag != 6)goto _LL39B;else{_tmp739=_tmp738->f2;if(_tmp739 != Cyc_Absyn_Short_sz)
goto _LL39B;}};_LL39A: goto _LL39C;_LL39B: _tmp73A=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp73B=(struct Cyc_Absyn_IntType_struct*)_tmp73A;if(_tmp73B->tag != 6)goto _LL39D;
else{_tmp73C=_tmp73B->f2;if(_tmp73C != Cyc_Absyn_Long_sz)goto _LL39D;}};_tmp73D=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp73E=(struct Cyc_Absyn_IntType_struct*)
_tmp73D;if(_tmp73E->tag != 6)goto _LL39D;else{_tmp73F=_tmp73E->f2;if(_tmp73F != Cyc_Absyn_Char_sz)
goto _LL39D;}};_LL39C: goto _LL39E;_LL39D: _tmp740=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp741=(struct Cyc_Absyn_IntType_struct*)_tmp740;if(_tmp741->tag != 6)goto _LL39F;
else{_tmp742=_tmp741->f2;if(_tmp742 != Cyc_Absyn_Int_sz)goto _LL39F;}};_tmp743=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp744=(struct Cyc_Absyn_IntType_struct*)
_tmp743;if(_tmp744->tag != 6)goto _LL39F;else{_tmp745=_tmp744->f2;if(_tmp745 != Cyc_Absyn_Char_sz)
goto _LL39F;}};_LL39E: goto _LL3A0;_LL39F: _tmp746=_tmp6EF.f1;{struct Cyc_Absyn_IntType_struct*
_tmp747=(struct Cyc_Absyn_IntType_struct*)_tmp746;if(_tmp747->tag != 6)goto _LL3A1;
else{_tmp748=_tmp747->f2;if(_tmp748 != Cyc_Absyn_Short_sz)goto _LL3A1;}};_tmp749=
_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp74A=(struct Cyc_Absyn_IntType_struct*)
_tmp749;if(_tmp74A->tag != 6)goto _LL3A1;else{_tmp74B=_tmp74A->f2;if(_tmp74B != Cyc_Absyn_Char_sz)
goto _LL3A1;}};_LL3A0: goto _LL3A2;_LL3A1: _tmp74C=_tmp6EF.f1;{struct Cyc_Absyn_TagType_struct*
_tmp74D=(struct Cyc_Absyn_TagType_struct*)_tmp74C;if(_tmp74D->tag != 20)goto _LL3A3;};
_tmp74E=_tmp6EF.f2;{struct Cyc_Absyn_IntType_struct*_tmp74F=(struct Cyc_Absyn_IntType_struct*)
_tmp74E;if(_tmp74F->tag != 6)goto _LL3A3;else{_tmp750=_tmp74F->f2;if(_tmp750 != Cyc_Absyn_Char_sz)
goto _LL3A3;}};_LL3A2: return 1;_LL3A3:;_LL3A4: return 0;_LL37C:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp751=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp12CC;max_arith_type=((_tmp12CC=
_region_malloc(_tmp751,sizeof(*_tmp12CC)),((_tmp12CC->v=t1,_tmp12CC))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp12D1;void*_tmp12D0[2];struct
Cyc_String_pa_struct _tmp12CF;struct Cyc_String_pa_struct _tmp12CE;(_tmp12CE.tag=0,((
_tmp12CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp12CF.tag=0,((_tmp12CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12D0[0]=& _tmp12CF,((_tmp12D0[1]=& _tmp12CE,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12D1="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12D1,sizeof(char),41))),_tag_dyneither(_tmp12D0,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp757=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL3A6: {struct Cyc_Absyn_PointerType_struct*
_tmp758=(struct Cyc_Absyn_PointerType_struct*)_tmp757;if(_tmp758->tag != 5)goto
_LL3A8;}_LL3A7: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL3A5;_LL3A8:;_LL3A9: return 0;_LL3A5:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp759=Cyc_Tcutil_compress(t);_LL3AB: {struct Cyc_Absyn_IntType_struct*
_tmp75A=(struct Cyc_Absyn_IntType_struct*)_tmp759;if(_tmp75A->tag != 6)goto _LL3AD;}
_LL3AC: goto _LL3AE;_LL3AD: {struct Cyc_Absyn_TagType_struct*_tmp75B=(struct Cyc_Absyn_TagType_struct*)
_tmp759;if(_tmp75B->tag != 20)goto _LL3AF;}_LL3AE: goto _LL3B0;_LL3AF: {struct Cyc_Absyn_EnumType_struct*
_tmp75C=(struct Cyc_Absyn_EnumType_struct*)_tmp759;if(_tmp75C->tag != 14)goto
_LL3B1;}_LL3B0: goto _LL3B2;_LL3B1: {struct Cyc_Absyn_AnonEnumType_struct*_tmp75D=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp759;if(_tmp75D->tag != 15)goto _LL3B3;}
_LL3B2: return 1;_LL3B3:;_LL3B4: return 0;_LL3AA:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp12D4;void*_tmp12D3;(_tmp12D3=0,Cyc_Tcutil_warn(e->loc,((_tmp12D4="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12D4,sizeof(char),44))),_tag_dyneither(_tmp12D3,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12D7;void*_tmp12D6;(
_tmp12D6=0,Cyc_Tcutil_warn(e->loc,((_tmp12D7="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12D7,sizeof(char),44))),_tag_dyneither(_tmp12D6,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12D8;struct _tuple0 _tmp763=(_tmp12D8.f1=
t1,((_tmp12D8.f2=t2,_tmp12D8)));void*_tmp764;struct Cyc_Absyn_PtrInfo _tmp766;void*
_tmp767;struct Cyc_Absyn_PtrInfo _tmp769;void*_tmp76A;struct Cyc_Absyn_ArrayInfo
_tmp76C;void*_tmp76D;struct Cyc_Absyn_Tqual _tmp76E;struct Cyc_Absyn_Exp*_tmp76F;
union Cyc_Absyn_Constraint*_tmp770;void*_tmp771;struct Cyc_Absyn_ArrayInfo _tmp773;
void*_tmp774;struct Cyc_Absyn_Tqual _tmp775;struct Cyc_Absyn_Exp*_tmp776;union Cyc_Absyn_Constraint*
_tmp777;void*_tmp778;void*_tmp77A;_LL3B6: _tmp764=_tmp763.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp765=(struct Cyc_Absyn_PointerType_struct*)_tmp764;if(_tmp765->tag != 5)goto
_LL3B8;else{_tmp766=_tmp765->f1;}};_tmp767=_tmp763.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp768=(struct Cyc_Absyn_PointerType_struct*)_tmp767;if(_tmp768->tag != 5)goto
_LL3B8;else{_tmp769=_tmp768->f1;}};_LL3B7: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp766.ptr_atts).nullable,(_tmp769.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp766.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp766.ptr_atts).bounds,(
_tmp769.ptr_atts).bounds)){struct _tuple0 _tmp12D9;struct _tuple0 _tmp77D=(_tmp12D9.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp766.ptr_atts).bounds),((_tmp12D9.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp769.ptr_atts).bounds),
_tmp12D9)));void*_tmp77E;void*_tmp780;void*_tmp782;void*_tmp784;void*_tmp786;
struct Cyc_Absyn_Exp*_tmp788;void*_tmp789;struct Cyc_Absyn_Exp*_tmp78B;void*
_tmp78C;void*_tmp78E;struct Cyc_Absyn_Exp*_tmp790;_LL3BF: _tmp77E=_tmp77D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp77F=(struct Cyc_Absyn_Upper_b_struct*)_tmp77E;
if(_tmp77F->tag != 1)goto _LL3C1;};_tmp780=_tmp77D.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp781=(struct Cyc_Absyn_DynEither_b_struct*)_tmp780;if(_tmp781->tag != 0)goto
_LL3C1;};_LL3C0: goto _LL3C2;_LL3C1: _tmp782=_tmp77D.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp783=(struct Cyc_Absyn_DynEither_b_struct*)_tmp782;if(_tmp783->tag != 0)goto
_LL3C3;};_tmp784=_tmp77D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp785=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp784;if(_tmp785->tag != 0)goto _LL3C3;};_LL3C2:
okay=1;goto _LL3BE;_LL3C3: _tmp786=_tmp77D.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp787=(struct Cyc_Absyn_Upper_b_struct*)_tmp786;if(_tmp787->tag != 1)goto _LL3C5;
else{_tmp788=_tmp787->f1;}};_tmp789=_tmp77D.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp78A=(struct Cyc_Absyn_Upper_b_struct*)_tmp789;if(_tmp78A->tag != 1)goto _LL3C5;
else{_tmp78B=_tmp78A->f1;}};_LL3C4: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp78B,
_tmp788);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp769.ptr_atts).zero_term)){const char*_tmp12DC;void*_tmp12DB;(_tmp12DB=0,
Cyc_Tcutil_warn(loc,((_tmp12DC="implicit cast to shorter array",_tag_dyneither(
_tmp12DC,sizeof(char),31))),_tag_dyneither(_tmp12DB,sizeof(void*),0)));}goto
_LL3BE;_LL3C5: _tmp78C=_tmp77D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp78D=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp78C;if(_tmp78D->tag != 0)goto _LL3BE;};
_tmp78E=_tmp77D.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp78F=(struct Cyc_Absyn_Upper_b_struct*)
_tmp78E;if(_tmp78F->tag != 1)goto _LL3BE;else{_tmp790=_tmp78F->f1;}};_LL3C6: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp766.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp769.ptr_atts).bounds))goto _LL3BE;okay=0;goto
_LL3BE;_LL3BE:;}okay=okay  && (!(_tmp766.elt_tq).real_const  || (_tmp769.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp766.ptr_atts).rgn,(_tmp769.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp766.ptr_atts).rgn,(_tmp769.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp766.ptr_atts).zero_term,(
_tmp769.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp766.ptr_atts).zero_term) && (_tmp769.elt_tq).real_const);{int _tmp793=((
int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp769.ptr_atts).bounds,
Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp769.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp766.elt_typ,
_tmp769.elt_typ) || (_tmp793  && ((_tmp769.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp769.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp766.elt_typ,_tmp769.elt_typ));return okay;};}_LL3B8: _tmp76A=_tmp763.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp76B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76A;if(_tmp76B->tag != 9)goto _LL3BA;else{_tmp76C=_tmp76B->f1;_tmp76D=_tmp76C.elt_type;
_tmp76E=_tmp76C.tq;_tmp76F=_tmp76C.num_elts;_tmp770=_tmp76C.zero_term;}};_tmp771=
_tmp763.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp772=(struct Cyc_Absyn_ArrayType_struct*)
_tmp771;if(_tmp772->tag != 9)goto _LL3BA;else{_tmp773=_tmp772->f1;_tmp774=_tmp773.elt_type;
_tmp775=_tmp773.tq;_tmp776=_tmp773.num_elts;_tmp777=_tmp773.zero_term;}};_LL3B9: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp770,_tmp777) && ((_tmp76F != 0
 && _tmp776 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp76F,(
struct Cyc_Absyn_Exp*)_tmp776));return(okay  && Cyc_Tcutil_unify(_tmp76D,_tmp774))
 && (!_tmp76E.real_const  || _tmp775.real_const);}_LL3BA: _tmp778=_tmp763.f1;{
struct Cyc_Absyn_TagType_struct*_tmp779=(struct Cyc_Absyn_TagType_struct*)_tmp778;
if(_tmp779->tag != 20)goto _LL3BC;};_tmp77A=_tmp763.f2;{struct Cyc_Absyn_IntType_struct*
_tmp77B=(struct Cyc_Absyn_IntType_struct*)_tmp77A;if(_tmp77B->tag != 6)goto _LL3BC;};
_LL3BB: return 0;_LL3BC:;_LL3BD: return Cyc_Tcutil_unify(t1,t2);_LL3B5:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp794=Cyc_Tcutil_compress(t);_LL3C8: {struct Cyc_Absyn_PointerType_struct*
_tmp795=(struct Cyc_Absyn_PointerType_struct*)_tmp794;if(_tmp795->tag != 5)goto
_LL3CA;}_LL3C9: return 1;_LL3CA:;_LL3CB: return 0;_LL3C7:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp796=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp798;void*
_tmp799;_LL3CD: {struct Cyc_Absyn_PointerType_struct*_tmp797=(struct Cyc_Absyn_PointerType_struct*)
_tmp796;if(_tmp797->tag != 5)goto _LL3CF;else{_tmp798=_tmp797->f1;_tmp799=_tmp798.elt_typ;}}
_LL3CE: return _tmp799;_LL3CF:;_LL3D0: {const char*_tmp12DF;void*_tmp12DE;(_tmp12DE=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12DF="pointer_elt_type",_tag_dyneither(_tmp12DF,sizeof(char),17))),
_tag_dyneither(_tmp12DE,sizeof(void*),0)));}_LL3CC:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp79C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp79E;
struct Cyc_Absyn_PtrAtts _tmp79F;struct Cyc_Absyn_PtrAtts*_tmp7A0;_LL3D2: {struct
Cyc_Absyn_PointerType_struct*_tmp79D=(struct Cyc_Absyn_PointerType_struct*)
_tmp79C;if(_tmp79D->tag != 5)goto _LL3D4;else{_tmp79E=_tmp79D->f1;_tmp79F=_tmp79E.ptr_atts;
_tmp7A0=(struct Cyc_Absyn_PtrAtts*)&(_tmp79D->f1).ptr_atts;}}_LL3D3: return _tmp7A0->rgn;
_LL3D4:;_LL3D5: {const char*_tmp12E2;void*_tmp12E1;(_tmp12E1=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E2="pointer_elt_type",
_tag_dyneither(_tmp12E2,sizeof(char),17))),_tag_dyneither(_tmp12E1,sizeof(void*),
0)));}_LL3D1:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp7A3=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp7A5;struct Cyc_Absyn_PtrAtts _tmp7A6;void*_tmp7A7;
_LL3D7: {struct Cyc_Absyn_PointerType_struct*_tmp7A4=(struct Cyc_Absyn_PointerType_struct*)
_tmp7A3;if(_tmp7A4->tag != 5)goto _LL3D9;else{_tmp7A5=_tmp7A4->f1;_tmp7A6=_tmp7A5.ptr_atts;
_tmp7A7=_tmp7A6.rgn;}}_LL3D8:*rgn=_tmp7A7;return 1;_LL3D9:;_LL3DA: return 0;_LL3D6:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp7A8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp7AA;struct Cyc_Absyn_PtrAtts _tmp7AB;union Cyc_Absyn_Constraint*
_tmp7AC;_LL3DC: {struct Cyc_Absyn_PointerType_struct*_tmp7A9=(struct Cyc_Absyn_PointerType_struct*)
_tmp7A8;if(_tmp7A9->tag != 5)goto _LL3DE;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7AA.ptr_atts;
_tmp7AC=_tmp7AB.bounds;}}_LL3DD:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7AC)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DE:;_LL3DF: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3DB:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp7AD=e->r;union Cyc_Absyn_Cnst
_tmp7AF;struct _tuple7 _tmp7B0;int _tmp7B1;union Cyc_Absyn_Cnst _tmp7B3;struct _tuple5
_tmp7B4;char _tmp7B5;union Cyc_Absyn_Cnst _tmp7B7;struct _tuple6 _tmp7B8;short _tmp7B9;
union Cyc_Absyn_Cnst _tmp7BB;struct _tuple8 _tmp7BC;long long _tmp7BD;void*_tmp7BF;
struct Cyc_Absyn_Exp*_tmp7C0;_LL3E1: {struct Cyc_Absyn_Const_e_struct*_tmp7AE=(
struct Cyc_Absyn_Const_e_struct*)_tmp7AD;if(_tmp7AE->tag != 0)goto _LL3E3;else{
_tmp7AF=_tmp7AE->f1;if((_tmp7AF.Int_c).tag != 4)goto _LL3E3;_tmp7B0=(struct _tuple7)(
_tmp7AF.Int_c).val;_tmp7B1=_tmp7B0.f2;if(_tmp7B1 != 0)goto _LL3E3;}}_LL3E2: goto
_LL3E4;_LL3E3: {struct Cyc_Absyn_Const_e_struct*_tmp7B2=(struct Cyc_Absyn_Const_e_struct*)
_tmp7AD;if(_tmp7B2->tag != 0)goto _LL3E5;else{_tmp7B3=_tmp7B2->f1;if((_tmp7B3.Char_c).tag
!= 2)goto _LL3E5;_tmp7B4=(struct _tuple5)(_tmp7B3.Char_c).val;_tmp7B5=_tmp7B4.f2;
if(_tmp7B5 != 0)goto _LL3E5;}}_LL3E4: goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Const_e_struct*
_tmp7B6=(struct Cyc_Absyn_Const_e_struct*)_tmp7AD;if(_tmp7B6->tag != 0)goto _LL3E7;
else{_tmp7B7=_tmp7B6->f1;if((_tmp7B7.Short_c).tag != 3)goto _LL3E7;_tmp7B8=(struct
_tuple6)(_tmp7B7.Short_c).val;_tmp7B9=_tmp7B8.f2;if(_tmp7B9 != 0)goto _LL3E7;}}
_LL3E6: goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Const_e_struct*_tmp7BA=(struct Cyc_Absyn_Const_e_struct*)
_tmp7AD;if(_tmp7BA->tag != 0)goto _LL3E9;else{_tmp7BB=_tmp7BA->f1;if((_tmp7BB.LongLong_c).tag
!= 5)goto _LL3E9;_tmp7BC=(struct _tuple8)(_tmp7BB.LongLong_c).val;_tmp7BD=_tmp7BC.f2;
if(_tmp7BD != 0)goto _LL3E9;}}_LL3E8: return 1;_LL3E9: {struct Cyc_Absyn_Cast_e_struct*
_tmp7BE=(struct Cyc_Absyn_Cast_e_struct*)_tmp7AD;if(_tmp7BE->tag != 15)goto _LL3EB;
else{_tmp7BF=(void*)_tmp7BE->f1;_tmp7C0=_tmp7BE->f2;}}_LL3EA: return Cyc_Tcutil_is_zero(
_tmp7C0) && Cyc_Tcutil_admits_zero(_tmp7BF);_LL3EB:;_LL3EC: return 0;_LL3E0:;}
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
_tmp7C2;enum Cyc_Absyn_KindQual _tmp7C3;enum Cyc_Absyn_AliasQual _tmp7C4;struct Cyc_Absyn_Kind*
_tmp7C1=ka;_tmp7C2=*_tmp7C1;_tmp7C3=_tmp7C2.kind;_tmp7C4=_tmp7C2.aliasqual;
switch(_tmp7C4){case Cyc_Absyn_Aliasable: _LL3ED: switch(_tmp7C3){case Cyc_Absyn_AnyKind:
_LL3EF: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3F0:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F1: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F2: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F3: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F4: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3EE: switch(_tmp7C3){case Cyc_Absyn_AnyKind: _LL3F7: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F8: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F9: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3FA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3FB: break;}break;case Cyc_Absyn_Top: _LL3F6: switch(_tmp7C3){case Cyc_Absyn_AnyKind:
_LL3FE: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FF:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL400: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL401: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL402: break;}break;}{const char*_tmp12E6;
void*_tmp12E5[1];struct Cyc_String_pa_struct _tmp12E4;(_tmp12E4.tag=0,((_tmp12E4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp12E5[0]=& _tmp12E4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12E6="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp12E6,sizeof(char),26))),_tag_dyneither(_tmp12E5,sizeof(void*),1)))))));};}
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
_tmp7C9;enum Cyc_Absyn_KindQual _tmp7CA;enum Cyc_Absyn_AliasQual _tmp7CB;struct Cyc_Absyn_Kind*
_tmp7C8=ka;_tmp7C9=*_tmp7C8;_tmp7CA=_tmp7C9.kind;_tmp7CB=_tmp7C9.aliasqual;
switch(_tmp7CB){case Cyc_Absyn_Aliasable: _LL404: switch(_tmp7CA){case Cyc_Absyn_AnyKind:
_LL406: return& ab;case Cyc_Absyn_MemKind: _LL407: return& mb;case Cyc_Absyn_BoxKind:
_LL408: return& bb;case Cyc_Absyn_RgnKind: _LL409: return& rb;case Cyc_Absyn_EffKind:
_LL40A: return& eb;case Cyc_Absyn_IntKind: _LL40B: return& ib;}case Cyc_Absyn_Unique:
_LL405: switch(_tmp7CA){case Cyc_Absyn_AnyKind: _LL40E: return& uab;case Cyc_Absyn_MemKind:
_LL40F: return& umb;case Cyc_Absyn_BoxKind: _LL410: return& ubb;case Cyc_Absyn_RgnKind:
_LL411: return& urb;default: _LL412: break;}break;case Cyc_Absyn_Top: _LL40D: switch(
_tmp7CA){case Cyc_Absyn_AnyKind: _LL415: return& tab;case Cyc_Absyn_MemKind: _LL416:
return& tmb;case Cyc_Absyn_BoxKind: _LL417: return& tbb;case Cyc_Absyn_RgnKind: _LL418:
return& trb;default: _LL419: break;}break;}{const char*_tmp12EA;void*_tmp12E9[1];
struct Cyc_String_pa_struct _tmp12E8;(_tmp12E8.tag=0,((_tmp12E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12E9[0]=&
_tmp12E8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12EA="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12EA,sizeof(char),24))),
_tag_dyneither(_tmp12E9,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){struct Cyc_Core_Opt*_tmp12EB;return(_tmp12EB=_cycalloc(
sizeof(*_tmp12EB)),((_tmp12EB->v=Cyc_Tcutil_kind_to_bound(k),_tmp12EB)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12EE;struct Cyc_Absyn_Const_e_struct*
_tmp12ED;e1->r=(void*)((_tmp12ED=_cycalloc(sizeof(*_tmp12ED)),((_tmp12ED[0]=((
_tmp12EE.tag=0,((_tmp12EE.f1=Cyc_Absyn_Null_c,_tmp12EE)))),_tmp12ED))));}{struct
Cyc_Core_Opt*_tmp7E0=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12F8;struct Cyc_Absyn_PtrAtts _tmp12F7;struct Cyc_Absyn_PtrInfo _tmp12F6;struct
Cyc_Absyn_PointerType_struct*_tmp12F5;struct Cyc_Absyn_PointerType_struct*_tmp7E1=(
_tmp12F5=_cycalloc(sizeof(*_tmp12F5)),((_tmp12F5[0]=((_tmp12F8.tag=5,((_tmp12F8.f1=((
_tmp12F6.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7E0),((
_tmp12F6.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12F6.ptr_atts=((_tmp12F7.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7E0),((_tmp12F7.nullable=Cyc_Absyn_true_conref,((
_tmp12F7.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12F7.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12F7.ptrloc=0,_tmp12F7)))))))))),_tmp12F6)))))),_tmp12F8)))),_tmp12F5)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7E1);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp12FD;void*_tmp12FC[2];struct Cyc_String_pa_struct _tmp12FB;struct Cyc_String_pa_struct
_tmp12FA;(_tmp12FA.tag=0,((_tmp12FA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12FB.tag=0,((
_tmp12FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12FC[0]=& _tmp12FB,((_tmp12FC[1]=& _tmp12FA,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FD="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12FD,sizeof(char),56))),_tag_dyneither(_tmp12FC,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL41B: {
const char*_tmp12FE;return(_tmp12FE="unknown",_tag_dyneither(_tmp12FE,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL41C: {const char*_tmp12FF;return(_tmp12FF="no coercion",
_tag_dyneither(_tmp12FF,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41D: {
const char*_tmp1300;return(_tmp1300="null check",_tag_dyneither(_tmp1300,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL41E: {const char*_tmp1301;return(
_tmp1301="other coercion",_tag_dyneither(_tmp1301,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp130E;const char*_tmp130D;void*_tmp130C[1];struct Cyc_Int_pa_struct
_tmp130B;struct _tuple2*_tmp130A;struct _tuple2*v=(_tmp130A=_cycalloc(sizeof(*
_tmp130A)),((_tmp130A->f1=Cyc_Absyn_Loc_n,((_tmp130A->f2=((_tmp130E=_cycalloc(
sizeof(*_tmp130E)),((_tmp130E[0]=(struct _dyneither_ptr)((_tmp130B.tag=1,((
_tmp130B.f1=(unsigned long)counter ++,((_tmp130C[0]=& _tmp130B,Cyc_aprintf(((
_tmp130D="__aliasvar%d",_tag_dyneither(_tmp130D,sizeof(char),13))),
_tag_dyneither(_tmp130C,sizeof(void*),1)))))))),_tmp130E)))),_tmp130A)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp1311;struct Cyc_Absyn_Local_b_struct*
_tmp1310;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp1310=_cycalloc(
sizeof(*_tmp1310)),((_tmp1310[0]=((_tmp1311.tag=4,((_tmp1311.f1=vd,_tmp1311)))),
_tmp1310)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7EE=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7F0;struct Cyc_Absyn_PtrAtts
_tmp7F1;void*_tmp7F2;void**_tmp7F3;_LL421: {struct Cyc_Absyn_PointerType_struct*
_tmp7EF=(struct Cyc_Absyn_PointerType_struct*)_tmp7EE;if(_tmp7EF->tag != 5)goto
_LL423;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=_tmp7F0.ptr_atts;_tmp7F2=_tmp7F1.rgn;
_tmp7F3=(void**)&((_tmp7EF->f1).ptr_atts).rgn;}}_LL422:{struct Cyc_Absyn_VarType_struct
_tmp1314;struct Cyc_Absyn_VarType_struct*_tmp1313;*_tmp7F3=(void*)((_tmp1313=
_cycalloc(sizeof(*_tmp1313)),((_tmp1313[0]=((_tmp1314.tag=2,((_tmp1314.f1=tv,
_tmp1314)))),_tmp1313))));}goto _LL420;_LL423:;_LL424: goto _LL420;_LL420:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple14 _tmp1315;return(_tmp1315.f1=
d,((_tmp1315.f2=ve,_tmp1315)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7FF=_new_region("r");struct _RegionHandle*r=& _tmp7FF;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp800=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp802;struct Cyc_Absyn_PtrAtts _tmp803;void*_tmp804;_LL426: {struct Cyc_Absyn_PointerType_struct*
_tmp801=(struct Cyc_Absyn_PointerType_struct*)_tmp800;if(_tmp801->tag != 5)goto
_LL428;else{_tmp802=_tmp801->f1;_tmp803=_tmp802.ptr_atts;_tmp804=_tmp803.rgn;}}
_LL427: {void*_tmp806=Cyc_Tcutil_compress(_tmp804);_LL42D: {struct Cyc_Absyn_HeapRgn_struct*
_tmp807=(struct Cyc_Absyn_HeapRgn_struct*)_tmp806;if(_tmp807->tag != 21)goto _LL42F;}
_LL42E: {int _tmp808=0;_npop_handler(0);return _tmp808;}_LL42F:;_LL430: {struct Cyc_Absyn_Kind*
_tmp809=Cyc_Tcutil_typ_kind(_tmp804);int _tmp80A=_tmp809->kind == Cyc_Absyn_RgnKind
 && _tmp809->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp80A;}
_LL42C:;}_LL428: {struct Cyc_Absyn_TypedefType_struct*_tmp805=(struct Cyc_Absyn_TypedefType_struct*)
_tmp800;if(_tmp805->tag != 18)goto _LL42A;}_LL429: {const char*_tmp131A;void*
_tmp1319[2];struct Cyc_String_pa_struct _tmp1318;struct Cyc_String_pa_struct
_tmp1317;(_tmp1317.tag=0,((_tmp1317.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp1318.tag=0,((_tmp1318.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp1319[0]=& _tmp1318,((_tmp1319[1]=& _tmp1317,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp131A="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp131A,sizeof(char),42))),_tag_dyneither(_tmp1319,sizeof(void*),
2)))))))))))));}_LL42A:;_LL42B: {int _tmp80F=0;_npop_handler(0);return _tmp80F;}
_LL425:;}{int _tmp810=0;_npop_handler(0);return _tmp810;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp131F;void*_tmp131E[2];
struct Cyc_String_pa_struct _tmp131D;struct Cyc_String_pa_struct _tmp131C;(_tmp131C.tag=
0,((_tmp131C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp131D.tag=0,((_tmp131D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp131E[0]=& _tmp131D,((_tmp131E[1]=& _tmp131C,Cyc_Tcutil_warn(
e->loc,((_tmp131F="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp131F,sizeof(char),53))),_tag_dyneither(_tmp131E,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1325;void*_tmp1324[3];struct Cyc_String_pa_struct _tmp1323;struct Cyc_String_pa_struct
_tmp1322;struct Cyc_String_pa_struct _tmp1321;(_tmp1321.tag=0,((_tmp1321.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1322.tag=0,((_tmp1322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1323.tag=0,((_tmp1323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp1324[0]=& _tmp1323,((_tmp1324[1]=& _tmp1322,((
_tmp1324[2]=& _tmp1321,Cyc_Tcutil_warn(e->loc,((_tmp1325="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp1325,sizeof(char),40))),_tag_dyneither(_tmp1324,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp132A;void*_tmp1329[2];
struct Cyc_String_pa_struct _tmp1328;struct Cyc_String_pa_struct _tmp1327;(_tmp1327.tag=
0,((_tmp1327.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1328.tag=0,((_tmp1328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1329[0]=& _tmp1328,((_tmp1329[1]=& _tmp1327,Cyc_Tcutil_warn(
e->loc,((_tmp132A="implicit cast from %s to %s",_tag_dyneither(_tmp132A,sizeof(
char),28))),_tag_dyneither(_tmp1329,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp81E=Cyc_Tcutil_compress(t);_LL432: {struct Cyc_Absyn_IntType_struct*
_tmp81F=(struct Cyc_Absyn_IntType_struct*)_tmp81E;if(_tmp81F->tag != 6)goto _LL434;}
_LL433: goto _LL435;_LL434: {struct Cyc_Absyn_FloatType_struct*_tmp820=(struct Cyc_Absyn_FloatType_struct*)
_tmp81E;if(_tmp820->tag != 7)goto _LL436;}_LL435: goto _LL437;_LL436: {struct Cyc_Absyn_DoubleType_struct*
_tmp821=(struct Cyc_Absyn_DoubleType_struct*)_tmp81E;if(_tmp821->tag != 8)goto
_LL438;}_LL437: return 1;_LL438:;_LL439: return 0;_LL431:;}static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,
void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*
f3;int f4;};static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,
struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp823;struct _RegionHandle*
_tmp824;struct Cyc_Tcenv_Tenv*_tmp825;int _tmp826;struct _tuple23 _tmp822=*env;
_tmp823=_tmp822.f1;_tmp824=_tmp822.f2;_tmp825=_tmp822.f3;_tmp826=_tmp822.f4;{
void*_tmp827=Cyc_Tcutil_rsubstitute(_tmp824,_tmp823,x->type);struct Cyc_List_List*
_tmp828=Cyc_Tcutil_flatten_typ(_tmp824,_tmp826,_tmp825,_tmp827);if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp828)== 1){struct _tuple11*_tmp132D;
struct Cyc_List_List*_tmp132C;return(_tmp132C=_region_malloc(_tmp824,sizeof(*
_tmp132C)),((_tmp132C->hd=((_tmp132D=_region_malloc(_tmp824,sizeof(*_tmp132D)),((
_tmp132D->f1=x->tq,((_tmp132D->f2=_tmp827,_tmp132D)))))),((_tmp132C->tl=0,
_tmp132C)))));}else{return _tmp828;}};}struct _tuple24{struct _RegionHandle*f1;
struct Cyc_Tcenv_Tenv*f2;int f3;};static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(
struct _tuple24*env,struct _tuple11*x){struct _RegionHandle*_tmp82C;struct Cyc_Tcenv_Tenv*
_tmp82D;int _tmp82E;struct _tuple24 _tmp82B=*env;_tmp82C=_tmp82B.f1;_tmp82D=_tmp82B.f2;
_tmp82E=_tmp82B.f3;{struct Cyc_Absyn_Tqual _tmp830;void*_tmp831;struct _tuple11
_tmp82F=*x;_tmp830=_tmp82F.f1;_tmp831=_tmp82F.f2;{struct Cyc_List_List*_tmp832=
Cyc_Tcutil_flatten_typ(_tmp82C,_tmp82E,_tmp82D,_tmp831);if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp832)== 1){struct _tuple11*_tmp1330;struct Cyc_List_List*
_tmp132F;return(_tmp132F=_region_malloc(_tmp82C,sizeof(*_tmp132F)),((_tmp132F->hd=((
_tmp1330=_region_malloc(_tmp82C,sizeof(*_tmp1330)),((_tmp1330->f1=_tmp830,((
_tmp1330->f2=_tmp831,_tmp1330)))))),((_tmp132F->tl=0,_tmp132F)))));}else{return
_tmp832;}};};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct
_RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(
t1);{void*_tmp835=t1;struct Cyc_List_List*_tmp838;struct Cyc_Absyn_AggrInfo _tmp83A;
union Cyc_Absyn_AggrInfoU _tmp83B;struct Cyc_Absyn_Aggrdecl**_tmp83C;struct Cyc_Absyn_Aggrdecl*
_tmp83D;struct Cyc_List_List*_tmp83E;enum Cyc_Absyn_AggrKind _tmp840;struct Cyc_List_List*
_tmp841;_LL43B: {struct Cyc_Absyn_VoidType_struct*_tmp836=(struct Cyc_Absyn_VoidType_struct*)
_tmp835;if(_tmp836->tag != 0)goto _LL43D;}_LL43C: return 0;_LL43D: {struct Cyc_Absyn_TupleType_struct*
_tmp837=(struct Cyc_Absyn_TupleType_struct*)_tmp835;if(_tmp837->tag != 11)goto
_LL43F;else{_tmp838=_tmp837->f1;}}_LL43E: {struct _tuple24 _tmp1331;struct _tuple24
_tmp842=(_tmp1331.f1=r,((_tmp1331.f2=te,((_tmp1331.f3=flatten,_tmp1331)))));
struct Cyc_List_List*_tmp843=_tmp838;struct Cyc_List_List _tmp844;struct _tuple11*
_tmp845;struct Cyc_List_List*_tmp846;_LL446: if(_tmp843 != 0)goto _LL448;_LL447:
return 0;_LL448: if(_tmp843 == 0)goto _LL445;_tmp844=*_tmp843;_tmp845=(struct
_tuple11*)_tmp844.hd;_tmp846=_tmp844.tl;_LL449: {struct Cyc_List_List*_tmp847=Cyc_Tcutil_rcopy_tqt(&
_tmp842,_tmp845);_tmp842.f3=0;{struct Cyc_List_List*_tmp848=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp842,_tmp838);struct Cyc_List_List*_tmp1332;struct Cyc_List_List*_tmp849=(
_tmp1332=_region_malloc(r,sizeof(*_tmp1332)),((_tmp1332->hd=_tmp847,((_tmp1332->tl=
_tmp848,_tmp1332)))));struct Cyc_List_List*_tmp1333;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1333=
_region_malloc(r,sizeof(*_tmp1333)),((_tmp1333->hd=_tmp847,((_tmp1333->tl=
_tmp848,_tmp1333)))))));};}_LL445:;}_LL43F: {struct Cyc_Absyn_AggrType_struct*
_tmp839=(struct Cyc_Absyn_AggrType_struct*)_tmp835;if(_tmp839->tag != 12)goto
_LL441;else{_tmp83A=_tmp839->f1;_tmp83B=_tmp83A.aggr_info;if((_tmp83B.KnownAggr).tag
!= 2)goto _LL441;_tmp83C=(struct Cyc_Absyn_Aggrdecl**)(_tmp83B.KnownAggr).val;
_tmp83D=*_tmp83C;_tmp83E=_tmp83A.targs;}}_LL440: if(((_tmp83D->kind == Cyc_Absyn_UnionA
 || _tmp83D->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp83D->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp83D->impl))->rgn_po != 0){
struct _tuple11*_tmp1336;struct Cyc_List_List*_tmp1335;return(_tmp1335=
_region_malloc(r,sizeof(*_tmp1335)),((_tmp1335->hd=((_tmp1336=_region_malloc(r,
sizeof(*_tmp1336)),((_tmp1336->f1=Cyc_Absyn_empty_tqual(0),((_tmp1336->f2=t1,
_tmp1336)))))),((_tmp1335->tl=0,_tmp1335)))));}{struct Cyc_List_List*_tmp84F=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp83D->tvs,_tmp83E);struct _tuple23
_tmp1337;struct _tuple23 env=(_tmp1337.f1=_tmp84F,((_tmp1337.f2=r,((_tmp1337.f3=te,((
_tmp1337.f4=flatten,_tmp1337)))))));struct Cyc_List_List*_tmp850=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp83D->impl))->fields;struct Cyc_List_List _tmp851;struct Cyc_Absyn_Aggrfield*
_tmp852;struct Cyc_List_List*_tmp853;_LL44B: if(_tmp850 != 0)goto _LL44D;_LL44C:
return 0;_LL44D: if(_tmp850 == 0)goto _LL44A;_tmp851=*_tmp850;_tmp852=(struct Cyc_Absyn_Aggrfield*)
_tmp851.hd;_tmp853=_tmp851.tl;_LL44E: {struct Cyc_List_List*_tmp854=Cyc_Tcutil_flatten_typ_f(&
env,_tmp852);env.f4=0;{struct Cyc_List_List*_tmp855=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp853);struct Cyc_List_List*_tmp1338;struct Cyc_List_List*_tmp856=(_tmp1338=
_region_malloc(r,sizeof(*_tmp1338)),((_tmp1338->hd=_tmp854,((_tmp1338->tl=
_tmp855,_tmp1338)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp856);};}_LL44A:;};_LL441: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp83F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp835;if(_tmp83F->tag != 13)goto _LL443;else{_tmp840=_tmp83F->f1;if(_tmp840 != 
Cyc_Absyn_StructA)goto _LL443;_tmp841=_tmp83F->f2;}}_LL442: {struct _tuple23
_tmp1339;struct _tuple23 env=(_tmp1339.f1=0,((_tmp1339.f2=r,((_tmp1339.f3=te,((
_tmp1339.f4=flatten,_tmp1339)))))));struct Cyc_List_List*_tmp859=_tmp841;struct
Cyc_List_List _tmp85A;struct Cyc_Absyn_Aggrfield*_tmp85B;struct Cyc_List_List*
_tmp85C;_LL450: if(_tmp859 != 0)goto _LL452;_LL451: return 0;_LL452: if(_tmp859 == 0)
goto _LL44F;_tmp85A=*_tmp859;_tmp85B=(struct Cyc_Absyn_Aggrfield*)_tmp85A.hd;
_tmp85C=_tmp85A.tl;_LL453: {struct Cyc_List_List*_tmp85D=Cyc_Tcutil_flatten_typ_f(&
env,_tmp85B);env.f4=0;{struct Cyc_List_List*_tmp85E=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp85C);struct Cyc_List_List*_tmp133A;struct Cyc_List_List*_tmp85F=(_tmp133A=
_region_malloc(r,sizeof(*_tmp133A)),((_tmp133A->hd=_tmp85D,((_tmp133A->tl=
_tmp85E,_tmp133A)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp85F);};}_LL44F:;}_LL443:;_LL444:
goto _LL43A;_LL43A:;};}{struct _tuple11*_tmp133D;struct Cyc_List_List*_tmp133C;
return(_tmp133C=_region_malloc(r,sizeof(*_tmp133C)),((_tmp133C->hd=((_tmp133D=
_region_malloc(r,sizeof(*_tmp133D)),((_tmp133D->f1=Cyc_Absyn_empty_tqual(0),((
_tmp133D->f2=t1,_tmp133D)))))),((_tmp133C->tl=0,_tmp133C)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp864=(void*)t->hd;_LL455: {struct Cyc_Absyn_Pure_att_struct*
_tmp865=(struct Cyc_Absyn_Pure_att_struct*)_tmp864;if(_tmp865->tag != 22)goto
_LL457;}_LL456: goto _LL458;_LL457: {struct Cyc_Absyn_Noreturn_att_struct*_tmp866=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp864;if(_tmp866->tag != 4)goto _LL459;}
_LL458: goto _LL45A;_LL459: {struct Cyc_Absyn_Initializes_att_struct*_tmp867=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp864;if(_tmp867->tag != 20)goto _LL45B;}
_LL45A: continue;_LL45B:;_LL45C: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL454:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp133E;struct _tuple0 _tmp869=(_tmp133E.f1=Cyc_Tcutil_compress(t1),((
_tmp133E.f2=Cyc_Tcutil_compress(t2),_tmp133E)));void*_tmp86A;enum Cyc_Absyn_Size_of
_tmp86C;void*_tmp86D;enum Cyc_Absyn_Size_of _tmp86F;_LL45E: _tmp86A=_tmp869.f1;{
struct Cyc_Absyn_IntType_struct*_tmp86B=(struct Cyc_Absyn_IntType_struct*)_tmp86A;
if(_tmp86B->tag != 6)goto _LL460;else{_tmp86C=_tmp86B->f2;}};_tmp86D=_tmp869.f2;{
struct Cyc_Absyn_IntType_struct*_tmp86E=(struct Cyc_Absyn_IntType_struct*)_tmp86D;
if(_tmp86E->tag != 6)goto _LL460;else{_tmp86F=_tmp86E->f2;}};_LL45F: return(_tmp86C
== _tmp86F  || _tmp86C == Cyc_Absyn_Int_sz  && _tmp86F == Cyc_Absyn_Long_sz) || 
_tmp86C == Cyc_Absyn_Long_sz  && _tmp86F == Cyc_Absyn_Int_sz;_LL460:;_LL461: return 0;
_LL45D:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp133F;struct _tuple0 _tmp871=(
_tmp133F.f1=t1,((_tmp133F.f2=t2,_tmp133F)));void*_tmp872;struct Cyc_Absyn_PtrInfo
_tmp874;void*_tmp875;struct Cyc_Absyn_Tqual _tmp876;struct Cyc_Absyn_PtrAtts _tmp877;
void*_tmp878;union Cyc_Absyn_Constraint*_tmp879;union Cyc_Absyn_Constraint*_tmp87A;
union Cyc_Absyn_Constraint*_tmp87B;void*_tmp87C;struct Cyc_Absyn_PtrInfo _tmp87E;
void*_tmp87F;struct Cyc_Absyn_Tqual _tmp880;struct Cyc_Absyn_PtrAtts _tmp881;void*
_tmp882;union Cyc_Absyn_Constraint*_tmp883;union Cyc_Absyn_Constraint*_tmp884;
union Cyc_Absyn_Constraint*_tmp885;void*_tmp886;struct Cyc_Absyn_DatatypeFieldInfo
_tmp888;union Cyc_Absyn_DatatypeFieldInfoU _tmp889;struct _tuple3 _tmp88A;struct Cyc_Absyn_Datatypedecl*
_tmp88B;struct Cyc_Absyn_Datatypefield*_tmp88C;struct Cyc_List_List*_tmp88D;void*
_tmp88E;struct Cyc_Absyn_DatatypeInfo _tmp890;union Cyc_Absyn_DatatypeInfoU _tmp891;
struct Cyc_Absyn_Datatypedecl**_tmp892;struct Cyc_Absyn_Datatypedecl*_tmp893;
struct Cyc_List_List*_tmp894;void*_tmp895;struct Cyc_Absyn_FnInfo _tmp897;void*
_tmp898;struct Cyc_Absyn_FnInfo _tmp89A;_LL463: _tmp872=_tmp871.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp873=(struct Cyc_Absyn_PointerType_struct*)_tmp872;if(_tmp873->tag != 5)goto
_LL465;else{_tmp874=_tmp873->f1;_tmp875=_tmp874.elt_typ;_tmp876=_tmp874.elt_tq;
_tmp877=_tmp874.ptr_atts;_tmp878=_tmp877.rgn;_tmp879=_tmp877.nullable;_tmp87A=
_tmp877.bounds;_tmp87B=_tmp877.zero_term;}};_tmp87C=_tmp871.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp87D=(struct Cyc_Absyn_PointerType_struct*)_tmp87C;if(_tmp87D->tag != 5)goto
_LL465;else{_tmp87E=_tmp87D->f1;_tmp87F=_tmp87E.elt_typ;_tmp880=_tmp87E.elt_tq;
_tmp881=_tmp87E.ptr_atts;_tmp882=_tmp881.rgn;_tmp883=_tmp881.nullable;_tmp884=
_tmp881.bounds;_tmp885=_tmp881.zero_term;}};_LL464: if(_tmp876.real_const  && !
_tmp880.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp879,
_tmp883) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp879)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp883))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp87B,
_tmp885) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87B)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp885))return 0;if(!Cyc_Tcutil_unify(_tmp878,_tmp882) && !Cyc_Tcenv_region_outlives(
te,_tmp878,_tmp882))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp87A,_tmp884)){struct _tuple0 _tmp1340;struct _tuple0 _tmp89C=(_tmp1340.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp87A),((_tmp1340.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp884),_tmp1340)));void*
_tmp89D;void*_tmp89F;void*_tmp8A1;struct Cyc_Absyn_Exp*_tmp8A3;void*_tmp8A4;
struct Cyc_Absyn_Exp*_tmp8A6;_LL46C: _tmp89D=_tmp89C.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp89E=(struct Cyc_Absyn_Upper_b_struct*)_tmp89D;if(_tmp89E->tag != 1)goto _LL46E;};
_tmp89F=_tmp89C.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp8A0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp89F;if(_tmp8A0->tag != 0)goto _LL46E;};_LL46D: goto _LL46B;_LL46E: _tmp8A1=
_tmp89C.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8A2=(struct Cyc_Absyn_Upper_b_struct*)
_tmp8A1;if(_tmp8A2->tag != 1)goto _LL470;else{_tmp8A3=_tmp8A2->f1;}};_tmp8A4=
_tmp89C.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp8A5=(struct Cyc_Absyn_Upper_b_struct*)
_tmp8A4;if(_tmp8A5->tag != 1)goto _LL470;else{_tmp8A6=_tmp8A5->f1;}};_LL46F: if(!
Cyc_Evexp_lte_const_exp(_tmp8A6,_tmp8A3))return 0;goto _LL46B;_LL470:;_LL471:
return 0;_LL46B:;}if(!_tmp880.real_const  && _tmp876.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp87F)))return 0;}{int _tmp8A7=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp884,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp885);struct _tuple0*
_tmp1343;struct Cyc_List_List*_tmp1342;return(_tmp8A7  && Cyc_Tcutil_ptrsubtype(te,((
_tmp1342=_cycalloc(sizeof(*_tmp1342)),((_tmp1342->hd=((_tmp1343=_cycalloc(
sizeof(*_tmp1343)),((_tmp1343->f1=t1,((_tmp1343->f2=t2,_tmp1343)))))),((_tmp1342->tl=
assume,_tmp1342)))))),_tmp875,_tmp87F) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL465: _tmp886=_tmp871.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp887=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp886;if(_tmp887->tag != 4)
goto _LL467;else{_tmp888=_tmp887->f1;_tmp889=_tmp888.field_info;if((_tmp889.KnownDatatypefield).tag
!= 2)goto _LL467;_tmp88A=(struct _tuple3)(_tmp889.KnownDatatypefield).val;_tmp88B=
_tmp88A.f1;_tmp88C=_tmp88A.f2;_tmp88D=_tmp888.targs;}};_tmp88E=_tmp871.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp88F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp88E;if(_tmp88F->tag != 3)goto _LL467;else{_tmp890=_tmp88F->f1;_tmp891=_tmp890.datatype_info;
if((_tmp891.KnownDatatype).tag != 2)goto _LL467;_tmp892=(struct Cyc_Absyn_Datatypedecl**)(
_tmp891.KnownDatatype).val;_tmp893=*_tmp892;_tmp894=_tmp890.targs;}};_LL466: if(
_tmp88B != _tmp893  && Cyc_Absyn_qvar_cmp(_tmp88B->name,_tmp893->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp88D)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp894))return 0;for(0;_tmp88D != 0;(_tmp88D=
_tmp88D->tl,_tmp894=_tmp894->tl)){if(!Cyc_Tcutil_unify((void*)_tmp88D->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp894))->hd))return 0;}return 1;_LL467: _tmp895=
_tmp871.f1;{struct Cyc_Absyn_FnType_struct*_tmp896=(struct Cyc_Absyn_FnType_struct*)
_tmp895;if(_tmp896->tag != 10)goto _LL469;else{_tmp897=_tmp896->f1;}};_tmp898=
_tmp871.f2;{struct Cyc_Absyn_FnType_struct*_tmp899=(struct Cyc_Absyn_FnType_struct*)
_tmp898;if(_tmp899->tag != 10)goto _LL469;else{_tmp89A=_tmp899->f1;}};_LL468: if(
_tmp897.tvars != 0  || _tmp89A.tvars != 0){struct Cyc_List_List*_tmp8AA=_tmp897.tvars;
struct Cyc_List_List*_tmp8AB=_tmp89A.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8AA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AB))return 0;{
struct _RegionHandle*_tmp8AC=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp8AA != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp8AA->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp8AB))->hd)->kind))return 0;{struct _tuple15*_tmp134D;struct Cyc_Absyn_VarType_struct
_tmp134C;struct Cyc_Absyn_VarType_struct*_tmp134B;struct Cyc_List_List*_tmp134A;
inst=((_tmp134A=_region_malloc(_tmp8AC,sizeof(*_tmp134A)),((_tmp134A->hd=((
_tmp134D=_region_malloc(_tmp8AC,sizeof(*_tmp134D)),((_tmp134D->f1=(struct Cyc_Absyn_Tvar*)
_tmp8AB->hd,((_tmp134D->f2=(void*)((_tmp134B=_cycalloc(sizeof(*_tmp134B)),((
_tmp134B[0]=((_tmp134C.tag=2,((_tmp134C.f1=(struct Cyc_Absyn_Tvar*)_tmp8AA->hd,
_tmp134C)))),_tmp134B)))),_tmp134D)))))),((_tmp134A->tl=inst,_tmp134A))))));}
_tmp8AA=_tmp8AA->tl;_tmp8AB=_tmp8AB->tl;}if(inst != 0){_tmp897.tvars=0;_tmp89A.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1353;struct Cyc_Absyn_FnType_struct*_tmp1352;
struct Cyc_Absyn_FnType_struct _tmp1350;struct Cyc_Absyn_FnType_struct*_tmp134F;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp134F=_cycalloc(sizeof(*_tmp134F)),((
_tmp134F[0]=((_tmp1350.tag=10,((_tmp1350.f1=_tmp897,_tmp1350)))),_tmp134F)))),(
void*)((_tmp1352=_cycalloc(sizeof(*_tmp1352)),((_tmp1352[0]=((_tmp1353.tag=10,((
_tmp1353.f1=_tmp89A,_tmp1353)))),_tmp1352)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp897.ret_typ,_tmp89A.ret_typ))return 0;{struct Cyc_List_List*_tmp8B5=
_tmp897.args;struct Cyc_List_List*_tmp8B6=_tmp89A.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp8B5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8B6))return 0;for(0;_tmp8B5 != 0;(_tmp8B5=_tmp8B5->tl,_tmp8B6=_tmp8B6->tl)){
struct Cyc_Absyn_Tqual _tmp8B8;void*_tmp8B9;struct _tuple9 _tmp8B7=*((struct _tuple9*)
_tmp8B5->hd);_tmp8B8=_tmp8B7.f2;_tmp8B9=_tmp8B7.f3;{struct Cyc_Absyn_Tqual _tmp8BB;
void*_tmp8BC;struct _tuple9 _tmp8BA=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp8B6))->hd);_tmp8BB=_tmp8BA.f2;_tmp8BC=_tmp8BA.f3;if(_tmp8BB.real_const
 && !_tmp8B8.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8BC,_tmp8B9))return
0;};}if(_tmp897.c_varargs != _tmp89A.c_varargs)return 0;if(_tmp897.cyc_varargs != 0
 && _tmp89A.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8BD=*_tmp897.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8BE=*_tmp89A.cyc_varargs;if((_tmp8BE.tq).real_const
 && !(_tmp8BD.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8BE.type,
_tmp8BD.type))return 0;}else{if(_tmp897.cyc_varargs != 0  || _tmp89A.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp897.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp89A.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp897.rgn_po,_tmp89A.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp897.attributes,_tmp89A.attributes))return 0;return 1;};_LL469:;_LL46A: return 0;
_LL462:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp8BF=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp8BF,1,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp8BF,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp8C1;struct Cyc_Absyn_Tqual
_tmp8C2;void*_tmp8C3;struct _tuple11*_tmp8C0=(struct _tuple11*)tqs1->hd;_tmp8C1=*
_tmp8C0;_tmp8C2=_tmp8C1.f1;_tmp8C3=_tmp8C1.f2;{struct _tuple11 _tmp8C5;struct Cyc_Absyn_Tqual
_tmp8C6;void*_tmp8C7;struct _tuple11*_tmp8C4=(struct _tuple11*)tqs2->hd;_tmp8C5=*
_tmp8C4;_tmp8C6=_tmp8C5.f1;_tmp8C7=_tmp8C5.f2;if(_tmp8C2.real_const  && !_tmp8C6.real_const)
return 0;if((_tmp8C6.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8C7))) && Cyc_Tcutil_subtype(te,assume,_tmp8C3,_tmp8C7))continue;if(Cyc_Tcutil_unify(
_tmp8C3,_tmp8C7))continue;if(Cyc_Tcutil_isomorphic(_tmp8C3,_tmp8C7))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8C8=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8CA;_LL473: {struct Cyc_Absyn_IntType_struct*_tmp8C9=(
struct Cyc_Absyn_IntType_struct*)_tmp8C8;if(_tmp8C9->tag != 6)goto _LL475;else{
_tmp8CA=_tmp8C9->f2;if(_tmp8CA != Cyc_Absyn_Char_sz)goto _LL475;}}_LL474: return 1;
_LL475:;_LL476: return 0;_LL472:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8CB=t2;enum Cyc_Absyn_Size_of _tmp8CD;enum Cyc_Absyn_Size_of _tmp8CF;_LL478: {
struct Cyc_Absyn_IntType_struct*_tmp8CC=(struct Cyc_Absyn_IntType_struct*)_tmp8CB;
if(_tmp8CC->tag != 6)goto _LL47A;else{_tmp8CD=_tmp8CC->f2;if(_tmp8CD != Cyc_Absyn_Int_sz)
goto _LL47A;}}_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_IntType_struct*_tmp8CE=(
struct Cyc_Absyn_IntType_struct*)_tmp8CB;if(_tmp8CE->tag != 6)goto _LL47C;else{
_tmp8CF=_tmp8CE->f2;if(_tmp8CF != Cyc_Absyn_Long_sz)goto _LL47C;}}_LL47B: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL477;_LL47C:;
_LL47D: goto _LL477;_LL477:;}{void*_tmp8D0=t1;struct Cyc_Absyn_PtrInfo _tmp8D2;void*
_tmp8D3;struct Cyc_Absyn_Tqual _tmp8D4;struct Cyc_Absyn_PtrAtts _tmp8D5;void*_tmp8D6;
union Cyc_Absyn_Constraint*_tmp8D7;union Cyc_Absyn_Constraint*_tmp8D8;union Cyc_Absyn_Constraint*
_tmp8D9;struct Cyc_Absyn_ArrayInfo _tmp8DB;void*_tmp8DC;struct Cyc_Absyn_Tqual
_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DE;union Cyc_Absyn_Constraint*_tmp8DF;struct Cyc_Absyn_Enumdecl*
_tmp8E1;void*_tmp8E6;_LL47F: {struct Cyc_Absyn_PointerType_struct*_tmp8D1=(struct
Cyc_Absyn_PointerType_struct*)_tmp8D0;if(_tmp8D1->tag != 5)goto _LL481;else{
_tmp8D2=_tmp8D1->f1;_tmp8D3=_tmp8D2.elt_typ;_tmp8D4=_tmp8D2.elt_tq;_tmp8D5=
_tmp8D2.ptr_atts;_tmp8D6=_tmp8D5.rgn;_tmp8D7=_tmp8D5.nullable;_tmp8D8=_tmp8D5.bounds;
_tmp8D9=_tmp8D5.zero_term;}}_LL480:{void*_tmp8E7=t2;struct Cyc_Absyn_PtrInfo
_tmp8E9;void*_tmp8EA;struct Cyc_Absyn_Tqual _tmp8EB;struct Cyc_Absyn_PtrAtts _tmp8EC;
void*_tmp8ED;union Cyc_Absyn_Constraint*_tmp8EE;union Cyc_Absyn_Constraint*_tmp8EF;
union Cyc_Absyn_Constraint*_tmp8F0;_LL490: {struct Cyc_Absyn_PointerType_struct*
_tmp8E8=(struct Cyc_Absyn_PointerType_struct*)_tmp8E7;if(_tmp8E8->tag != 5)goto
_LL492;else{_tmp8E9=_tmp8E8->f1;_tmp8EA=_tmp8E9.elt_typ;_tmp8EB=_tmp8E9.elt_tq;
_tmp8EC=_tmp8E9.ptr_atts;_tmp8ED=_tmp8EC.rgn;_tmp8EE=_tmp8EC.nullable;_tmp8EF=
_tmp8EC.bounds;_tmp8F0=_tmp8EC.zero_term;}}_LL491: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1356;struct Cyc_List_List*
_tmp1355;struct Cyc_List_List*_tmp8F1=(_tmp1355=_cycalloc(sizeof(*_tmp1355)),((
_tmp1355->hd=((_tmp1356=_cycalloc(sizeof(*_tmp1356)),((_tmp1356->f1=t1,((
_tmp1356->f2=t2,_tmp1356)))))),((_tmp1355->tl=0,_tmp1355)))));int _tmp8F2=_tmp8EB.real_const
 || !_tmp8D4.real_const;int _tmp8F3=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8EF,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8F0);int _tmp8F4=_tmp8F2  && (((_tmp8F3  && Cyc_Tcutil_ptrsubtype(
te,_tmp8F1,_tmp8D3,_tmp8EA) || Cyc_Tcutil_unify(_tmp8D3,_tmp8EA)) || Cyc_Tcutil_isomorphic(
_tmp8D3,_tmp8EA)) || Cyc_Tcutil_unify(_tmp8EA,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8D9,_tmp8F0) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8F0);int _tmp8F5=_tmp8F4?0:((Cyc_Tcutil_bits_only(_tmp8D3) && Cyc_Tcutil_is_char_type(
_tmp8EA)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8F0)) && (_tmp8EB.real_const  || !_tmp8D4.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D8,_tmp8EF);if(!
bounds_ok  && !_tmp8F5){struct _tuple0 _tmp1357;struct _tuple0 _tmp8F7=(_tmp1357.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D8),((_tmp1357.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8EF),_tmp1357)));
void*_tmp8F8;struct Cyc_Absyn_Exp*_tmp8FA;void*_tmp8FB;struct Cyc_Absyn_Exp*
_tmp8FD;_LL495: _tmp8F8=_tmp8F7.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8F9=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8F8;if(_tmp8F9->tag != 1)goto _LL497;else{
_tmp8FA=_tmp8F9->f1;}};_tmp8FB=_tmp8F7.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8FC=(struct Cyc_Absyn_Upper_b_struct*)_tmp8FB;if(_tmp8FC->tag != 1)goto _LL497;
else{_tmp8FD=_tmp8FC->f1;}};_LL496: if(Cyc_Evexp_lte_const_exp(_tmp8FD,_tmp8FA))
bounds_ok=1;goto _LL494;_LL497:;_LL498: bounds_ok=1;goto _LL494;_LL494:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8D7) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8EE))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8F4  || _tmp8F5)) && (Cyc_Tcutil_unify(
_tmp8D6,_tmp8ED) || Cyc_Tcenv_region_outlives(te,_tmp8D6,_tmp8ED)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493: goto _LL48F;
_LL48F:;}return Cyc_Absyn_Unknown_coercion;_LL481: {struct Cyc_Absyn_ArrayType_struct*
_tmp8DA=(struct Cyc_Absyn_ArrayType_struct*)_tmp8D0;if(_tmp8DA->tag != 9)goto
_LL483;else{_tmp8DB=_tmp8DA->f1;_tmp8DC=_tmp8DB.elt_type;_tmp8DD=_tmp8DB.tq;
_tmp8DE=_tmp8DB.num_elts;_tmp8DF=_tmp8DB.zero_term;}}_LL482:{void*_tmp900=t2;
struct Cyc_Absyn_ArrayInfo _tmp902;void*_tmp903;struct Cyc_Absyn_Tqual _tmp904;
struct Cyc_Absyn_Exp*_tmp905;union Cyc_Absyn_Constraint*_tmp906;_LL49A: {struct Cyc_Absyn_ArrayType_struct*
_tmp901=(struct Cyc_Absyn_ArrayType_struct*)_tmp900;if(_tmp901->tag != 9)goto
_LL49C;else{_tmp902=_tmp901->f1;_tmp903=_tmp902.elt_type;_tmp904=_tmp902.tq;
_tmp905=_tmp902.num_elts;_tmp906=_tmp902.zero_term;}}_LL49B: {int okay;okay=((
_tmp8DE != 0  && _tmp905 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8DF,
_tmp906)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp905,(struct Cyc_Absyn_Exp*)
_tmp8DE);return(okay  && Cyc_Tcutil_unify(_tmp8DC,_tmp903)) && (!_tmp8DD.real_const
 || _tmp904.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL49C:;_LL49D: return Cyc_Absyn_Unknown_coercion;_LL499:;}return Cyc_Absyn_Unknown_coercion;
_LL483: {struct Cyc_Absyn_EnumType_struct*_tmp8E0=(struct Cyc_Absyn_EnumType_struct*)
_tmp8D0;if(_tmp8E0->tag != 14)goto _LL485;else{_tmp8E1=_tmp8E0->f2;}}_LL484:{void*
_tmp907=t2;struct Cyc_Absyn_Enumdecl*_tmp909;_LL49F: {struct Cyc_Absyn_EnumType_struct*
_tmp908=(struct Cyc_Absyn_EnumType_struct*)_tmp907;if(_tmp908->tag != 14)goto
_LL4A1;else{_tmp909=_tmp908->f2;}}_LL4A0: if((_tmp8E1->fields != 0  && _tmp909->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8E1->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp909->fields))->v))return Cyc_Absyn_No_coercion;goto _LL49E;_LL4A1:;_LL4A2: goto
_LL49E;_LL49E:;}goto _LL486;_LL485: {struct Cyc_Absyn_IntType_struct*_tmp8E2=(
struct Cyc_Absyn_IntType_struct*)_tmp8D0;if(_tmp8E2->tag != 6)goto _LL487;}_LL486:
goto _LL488;_LL487: {struct Cyc_Absyn_FloatType_struct*_tmp8E3=(struct Cyc_Absyn_FloatType_struct*)
_tmp8D0;if(_tmp8E3->tag != 7)goto _LL489;}_LL488: goto _LL48A;_LL489: {struct Cyc_Absyn_DoubleType_struct*
_tmp8E4=(struct Cyc_Absyn_DoubleType_struct*)_tmp8D0;if(_tmp8E4->tag != 8)goto
_LL48B;}_LL48A: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL48B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp8E5=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8D0;if(_tmp8E5->tag != 16)goto _LL48D;else{_tmp8E6=(void*)_tmp8E5->f1;}}_LL48C:{
void*_tmp90A=t2;void*_tmp90C;_LL4A4: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp90B=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp90A;if(_tmp90B->tag != 16)goto
_LL4A6;else{_tmp90C=(void*)_tmp90B->f1;}}_LL4A5: if(Cyc_Tcenv_region_outlives(te,
_tmp8E6,_tmp90C))return Cyc_Absyn_No_coercion;goto _LL4A3;_LL4A6:;_LL4A7: goto
_LL4A3;_LL4A3:;}return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E: return Cyc_Absyn_Unknown_coercion;
_LL47E:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp90D=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp135A;struct Cyc_Absyn_Cast_e_struct*_tmp1359;e->r=(
void*)((_tmp1359=_cycalloc(sizeof(*_tmp1359)),((_tmp1359[0]=((_tmp135A.tag=15,((
_tmp135A.f1=(void*)t,((_tmp135A.f2=_tmp90D,((_tmp135A.f3=0,((_tmp135A.f4=c,
_tmp135A)))))))))),_tmp1359))));}{struct Cyc_Core_Opt*_tmp135B;e->topt=((_tmp135B=
_cycalloc(sizeof(*_tmp135B)),((_tmp135B->v=t,_tmp135B))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp911=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL4A9: {struct Cyc_Absyn_IntType_struct*_tmp912=(
struct Cyc_Absyn_IntType_struct*)_tmp911;if(_tmp912->tag != 6)goto _LL4AB;}_LL4AA:
goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_struct*_tmp913=(struct Cyc_Absyn_EnumType_struct*)
_tmp911;if(_tmp913->tag != 14)goto _LL4AD;}_LL4AC: goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp914=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp911;if(_tmp914->tag != 15)goto
_LL4AF;}_LL4AE: goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_struct*_tmp915=(
struct Cyc_Absyn_TagType_struct*)_tmp911;if(_tmp915->tag != 20)goto _LL4B1;}_LL4B0:
return 1;_LL4B1: {struct Cyc_Absyn_Evar_struct*_tmp916=(struct Cyc_Absyn_Evar_struct*)
_tmp911;if(_tmp916->tag != 1)goto _LL4B3;}_LL4B2: return Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
return 0;_LL4A8:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp917=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL4B6: {struct Cyc_Absyn_FloatType_struct*_tmp918=(
struct Cyc_Absyn_FloatType_struct*)_tmp917;if(_tmp918->tag != 7)goto _LL4B8;}_LL4B7:
goto _LL4B9;_LL4B8: {struct Cyc_Absyn_DoubleType_struct*_tmp919=(struct Cyc_Absyn_DoubleType_struct*)
_tmp917;if(_tmp919->tag != 8)goto _LL4BA;}_LL4B9: return 1;_LL4BA:;_LL4BB: return 0;
_LL4B5:;};}int Cyc_Tcutil_is_function_type(void*t){void*_tmp91A=Cyc_Tcutil_compress(
t);_LL4BD: {struct Cyc_Absyn_FnType_struct*_tmp91B=(struct Cyc_Absyn_FnType_struct*)
_tmp91A;if(_tmp91B->tag != 10)goto _LL4BF;}_LL4BE: return 1;_LL4BF:;_LL4C0: return 0;
_LL4BC:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp135C;struct _tuple0 _tmp91D=(_tmp135C.f1=t1,((_tmp135C.f2=t2,_tmp135C)));void*
_tmp91E;int _tmp920;void*_tmp921;int _tmp923;void*_tmp924;void*_tmp926;void*
_tmp928;void*_tmp92A;void*_tmp92C;enum Cyc_Absyn_Sign _tmp92E;enum Cyc_Absyn_Size_of
_tmp92F;void*_tmp930;enum Cyc_Absyn_Sign _tmp932;enum Cyc_Absyn_Size_of _tmp933;
void*_tmp934;enum Cyc_Absyn_Size_of _tmp936;void*_tmp937;enum Cyc_Absyn_Size_of
_tmp939;void*_tmp93A;enum Cyc_Absyn_Sign _tmp93C;enum Cyc_Absyn_Size_of _tmp93D;
void*_tmp93E;enum Cyc_Absyn_Sign _tmp940;enum Cyc_Absyn_Size_of _tmp941;void*
_tmp942;enum Cyc_Absyn_Sign _tmp944;enum Cyc_Absyn_Size_of _tmp945;void*_tmp946;
enum Cyc_Absyn_Sign _tmp948;enum Cyc_Absyn_Size_of _tmp949;void*_tmp94A;enum Cyc_Absyn_Size_of
_tmp94C;void*_tmp94D;enum Cyc_Absyn_Size_of _tmp94F;_LL4C2: _tmp91E=_tmp91D.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp91F=(struct Cyc_Absyn_DoubleType_struct*)
_tmp91E;if(_tmp91F->tag != 8)goto _LL4C4;else{_tmp920=_tmp91F->f1;}};_tmp921=
_tmp91D.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp922=(struct Cyc_Absyn_DoubleType_struct*)
_tmp921;if(_tmp922->tag != 8)goto _LL4C4;else{_tmp923=_tmp922->f1;}};_LL4C3: if(
_tmp920)return t1;else{return t2;}_LL4C4: _tmp924=_tmp91D.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp925=(struct Cyc_Absyn_DoubleType_struct*)_tmp924;if(_tmp925->tag != 8)goto
_LL4C6;};_LL4C5: return t1;_LL4C6: _tmp926=_tmp91D.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp927=(struct Cyc_Absyn_DoubleType_struct*)_tmp926;if(_tmp927->tag != 8)goto
_LL4C8;};_LL4C7: return t2;_LL4C8: _tmp928=_tmp91D.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp929=(struct Cyc_Absyn_FloatType_struct*)_tmp928;if(_tmp929->tag != 7)goto
_LL4CA;};_LL4C9: goto _LL4CB;_LL4CA: _tmp92A=_tmp91D.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp92B=(struct Cyc_Absyn_FloatType_struct*)_tmp92A;if(_tmp92B->tag != 7)goto
_LL4CC;};_LL4CB: return(void*)& Cyc_Absyn_FloatType_val;_LL4CC: _tmp92C=_tmp91D.f1;{
struct Cyc_Absyn_IntType_struct*_tmp92D=(struct Cyc_Absyn_IntType_struct*)_tmp92C;
if(_tmp92D->tag != 6)goto _LL4CE;else{_tmp92E=_tmp92D->f1;if(_tmp92E != Cyc_Absyn_Unsigned)
goto _LL4CE;_tmp92F=_tmp92D->f2;if(_tmp92F != Cyc_Absyn_LongLong_sz)goto _LL4CE;}};
_LL4CD: goto _LL4CF;_LL4CE: _tmp930=_tmp91D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp931=(struct Cyc_Absyn_IntType_struct*)_tmp930;if(_tmp931->tag != 6)goto _LL4D0;
else{_tmp932=_tmp931->f1;if(_tmp932 != Cyc_Absyn_Unsigned)goto _LL4D0;_tmp933=
_tmp931->f2;if(_tmp933 != Cyc_Absyn_LongLong_sz)goto _LL4D0;}};_LL4CF: return Cyc_Absyn_ulonglong_typ;
_LL4D0: _tmp934=_tmp91D.f1;{struct Cyc_Absyn_IntType_struct*_tmp935=(struct Cyc_Absyn_IntType_struct*)
_tmp934;if(_tmp935->tag != 6)goto _LL4D2;else{_tmp936=_tmp935->f2;if(_tmp936 != Cyc_Absyn_LongLong_sz)
goto _LL4D2;}};_LL4D1: goto _LL4D3;_LL4D2: _tmp937=_tmp91D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp938=(struct Cyc_Absyn_IntType_struct*)_tmp937;if(_tmp938->tag != 6)goto _LL4D4;
else{_tmp939=_tmp938->f2;if(_tmp939 != Cyc_Absyn_LongLong_sz)goto _LL4D4;}};_LL4D3:
return Cyc_Absyn_slonglong_typ;_LL4D4: _tmp93A=_tmp91D.f1;{struct Cyc_Absyn_IntType_struct*
_tmp93B=(struct Cyc_Absyn_IntType_struct*)_tmp93A;if(_tmp93B->tag != 6)goto _LL4D6;
else{_tmp93C=_tmp93B->f1;if(_tmp93C != Cyc_Absyn_Unsigned)goto _LL4D6;_tmp93D=
_tmp93B->f2;if(_tmp93D != Cyc_Absyn_Long_sz)goto _LL4D6;}};_LL4D5: goto _LL4D7;
_LL4D6: _tmp93E=_tmp91D.f2;{struct Cyc_Absyn_IntType_struct*_tmp93F=(struct Cyc_Absyn_IntType_struct*)
_tmp93E;if(_tmp93F->tag != 6)goto _LL4D8;else{_tmp940=_tmp93F->f1;if(_tmp940 != Cyc_Absyn_Unsigned)
goto _LL4D8;_tmp941=_tmp93F->f2;if(_tmp941 != Cyc_Absyn_Long_sz)goto _LL4D8;}};
_LL4D7: return Cyc_Absyn_ulong_typ;_LL4D8: _tmp942=_tmp91D.f1;{struct Cyc_Absyn_IntType_struct*
_tmp943=(struct Cyc_Absyn_IntType_struct*)_tmp942;if(_tmp943->tag != 6)goto _LL4DA;
else{_tmp944=_tmp943->f1;if(_tmp944 != Cyc_Absyn_Unsigned)goto _LL4DA;_tmp945=
_tmp943->f2;if(_tmp945 != Cyc_Absyn_Int_sz)goto _LL4DA;}};_LL4D9: goto _LL4DB;_LL4DA:
_tmp946=_tmp91D.f2;{struct Cyc_Absyn_IntType_struct*_tmp947=(struct Cyc_Absyn_IntType_struct*)
_tmp946;if(_tmp947->tag != 6)goto _LL4DC;else{_tmp948=_tmp947->f1;if(_tmp948 != Cyc_Absyn_Unsigned)
goto _LL4DC;_tmp949=_tmp947->f2;if(_tmp949 != Cyc_Absyn_Int_sz)goto _LL4DC;}};
_LL4DB: return Cyc_Absyn_uint_typ;_LL4DC: _tmp94A=_tmp91D.f1;{struct Cyc_Absyn_IntType_struct*
_tmp94B=(struct Cyc_Absyn_IntType_struct*)_tmp94A;if(_tmp94B->tag != 6)goto _LL4DE;
else{_tmp94C=_tmp94B->f2;if(_tmp94C != Cyc_Absyn_Long_sz)goto _LL4DE;}};_LL4DD:
goto _LL4DF;_LL4DE: _tmp94D=_tmp91D.f2;{struct Cyc_Absyn_IntType_struct*_tmp94E=(
struct Cyc_Absyn_IntType_struct*)_tmp94D;if(_tmp94E->tag != 6)goto _LL4E0;else{
_tmp94F=_tmp94E->f2;if(_tmp94F != Cyc_Absyn_Long_sz)goto _LL4E0;}};_LL4DF: return
Cyc_Absyn_slong_typ;_LL4E0:;_LL4E1: return Cyc_Absyn_sint_typ;_LL4C1:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp950=e->r;struct Cyc_Core_Opt*_tmp952;_LL4E3: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp951=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp950;if(_tmp951->tag != 4)goto _LL4E5;else{_tmp952=_tmp951->f2;if(_tmp952 != 0)
goto _LL4E5;}}_LL4E4:{const char*_tmp135F;void*_tmp135E;(_tmp135E=0,Cyc_Tcutil_warn(
e->loc,((_tmp135F="assignment in test",_tag_dyneither(_tmp135F,sizeof(char),19))),
_tag_dyneither(_tmp135E,sizeof(void*),0)));}goto _LL4E2;_LL4E5:;_LL4E6: goto _LL4E2;
_LL4E2:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1360;struct _tuple0 _tmp956=(
_tmp1360.f1=c1,((_tmp1360.f2=c2,_tmp1360)));void*_tmp957;struct Cyc_Absyn_Kind*
_tmp959;void*_tmp95A;struct Cyc_Absyn_Kind*_tmp95C;void*_tmp95D;struct Cyc_Core_Opt*
_tmp95F;struct Cyc_Core_Opt**_tmp960;void*_tmp961;struct Cyc_Core_Opt*_tmp963;
struct Cyc_Core_Opt**_tmp964;void*_tmp965;struct Cyc_Core_Opt*_tmp967;struct Cyc_Core_Opt**
_tmp968;struct Cyc_Absyn_Kind*_tmp969;void*_tmp96A;struct Cyc_Absyn_Kind*_tmp96C;
void*_tmp96D;struct Cyc_Absyn_Kind*_tmp96F;void*_tmp970;struct Cyc_Core_Opt*
_tmp972;struct Cyc_Core_Opt**_tmp973;struct Cyc_Absyn_Kind*_tmp974;void*_tmp975;
struct Cyc_Core_Opt*_tmp977;struct Cyc_Core_Opt**_tmp978;struct Cyc_Absyn_Kind*
_tmp979;void*_tmp97A;struct Cyc_Core_Opt*_tmp97C;struct Cyc_Core_Opt**_tmp97D;
struct Cyc_Absyn_Kind*_tmp97E;_LL4E8: _tmp957=_tmp956.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp958=(struct Cyc_Absyn_Eq_kb_struct*)_tmp957;if(_tmp958->tag != 0)goto _LL4EA;
else{_tmp959=_tmp958->f1;}};_tmp95A=_tmp956.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp95B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95A;if(_tmp95B->tag != 0)goto _LL4EA;
else{_tmp95C=_tmp95B->f1;}};_LL4E9: return _tmp959 == _tmp95C;_LL4EA: _tmp95D=
_tmp956.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp95E=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp95D;if(_tmp95E->tag != 1)goto _LL4EC;else{_tmp95F=_tmp95E->f1;_tmp960=(struct
Cyc_Core_Opt**)& _tmp95E->f1;}};_LL4EB:{struct Cyc_Core_Opt*_tmp1361;*_tmp960=((
_tmp1361=_cycalloc(sizeof(*_tmp1361)),((_tmp1361->v=c1,_tmp1361))));}return 1;
_LL4EC: _tmp961=_tmp956.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp962=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp961;if(_tmp962->tag != 1)goto _LL4EE;else{_tmp963=_tmp962->f1;_tmp964=(struct
Cyc_Core_Opt**)& _tmp962->f1;}};_LL4ED:{struct Cyc_Core_Opt*_tmp1362;*_tmp964=((
_tmp1362=_cycalloc(sizeof(*_tmp1362)),((_tmp1362->v=c2,_tmp1362))));}return 1;
_LL4EE: _tmp965=_tmp956.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp966=(struct Cyc_Absyn_Less_kb_struct*)
_tmp965;if(_tmp966->tag != 2)goto _LL4F0;else{_tmp967=_tmp966->f1;_tmp968=(struct
Cyc_Core_Opt**)& _tmp966->f1;_tmp969=_tmp966->f2;}};_tmp96A=_tmp956.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp96B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp96A;if(_tmp96B->tag != 0)goto _LL4F0;
else{_tmp96C=_tmp96B->f1;}};_LL4EF: if(Cyc_Tcutil_kind_leq(_tmp96C,_tmp969)){{
struct Cyc_Core_Opt*_tmp1363;*_tmp968=((_tmp1363=_cycalloc(sizeof(*_tmp1363)),((
_tmp1363->v=c2,_tmp1363))));}return 1;}else{return 0;}_LL4F0: _tmp96D=_tmp956.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp96E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp96D;if(
_tmp96E->tag != 0)goto _LL4F2;else{_tmp96F=_tmp96E->f1;}};_tmp970=_tmp956.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp971=(struct Cyc_Absyn_Less_kb_struct*)_tmp970;
if(_tmp971->tag != 2)goto _LL4F2;else{_tmp972=_tmp971->f1;_tmp973=(struct Cyc_Core_Opt**)&
_tmp971->f1;_tmp974=_tmp971->f2;}};_LL4F1: if(Cyc_Tcutil_kind_leq(_tmp96F,_tmp974)){{
struct Cyc_Core_Opt*_tmp1364;*_tmp973=((_tmp1364=_cycalloc(sizeof(*_tmp1364)),((
_tmp1364->v=c1,_tmp1364))));}return 1;}else{return 0;}_LL4F2: _tmp975=_tmp956.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp976=(struct Cyc_Absyn_Less_kb_struct*)_tmp975;
if(_tmp976->tag != 2)goto _LL4E7;else{_tmp977=_tmp976->f1;_tmp978=(struct Cyc_Core_Opt**)&
_tmp976->f1;_tmp979=_tmp976->f2;}};_tmp97A=_tmp956.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp97B=(struct Cyc_Absyn_Less_kb_struct*)_tmp97A;if(_tmp97B->tag != 2)goto _LL4E7;
else{_tmp97C=_tmp97B->f1;_tmp97D=(struct Cyc_Core_Opt**)& _tmp97B->f1;_tmp97E=
_tmp97B->f2;}};_LL4F3: if(Cyc_Tcutil_kind_leq(_tmp979,_tmp97E)){{struct Cyc_Core_Opt*
_tmp1365;*_tmp97D=((_tmp1365=_cycalloc(sizeof(*_tmp1365)),((_tmp1365->v=c1,
_tmp1365))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp97E,_tmp979)){{struct Cyc_Core_Opt*
_tmp1366;*_tmp978=((_tmp1366=_cycalloc(sizeof(*_tmp1366)),((_tmp1366->v=c2,
_tmp1366))));}return 1;}else{return 0;}}_LL4E7:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp136A;void*_tmp1369[1];struct Cyc_Int_pa_struct _tmp1368;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1368.tag=1,((_tmp1368.f1=(
unsigned long)i,((_tmp1369[0]=& _tmp1368,Cyc_aprintf(((_tmp136A="#%d",
_tag_dyneither(_tmp136A,sizeof(char),4))),_tag_dyneither(_tmp1369,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp136D;struct Cyc_Absyn_Tvar*_tmp136C;return(
_tmp136C=_cycalloc(sizeof(*_tmp136C)),((_tmp136C->name=((_tmp136D=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp136D[0]=s,_tmp136D)))),((_tmp136C->identity=
- 1,((_tmp136C->kind=k,_tmp136C)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp98A=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp98A,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1371;void*_tmp1370[1];struct Cyc_String_pa_struct
_tmp136F;(_tmp136F.tag=0,((_tmp136F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp1370[0]=& _tmp136F,Cyc_printf(((_tmp1371="%s",
_tag_dyneither(_tmp1371,sizeof(char),3))),_tag_dyneither(_tmp1370,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1372;struct
_dyneither_ptr _tmp98E=Cyc_strconcat(((_tmp1372="`",_tag_dyneither(_tmp1372,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1375;char _tmp1374;
struct _dyneither_ptr _tmp1373;(_tmp1373=_dyneither_ptr_plus(_tmp98E,sizeof(char),
1),((_tmp1374=*((char*)_check_dyneither_subscript(_tmp1373,sizeof(char),0)),((
_tmp1375='t',((_get_dyneither_size(_tmp1373,sizeof(char))== 1  && (_tmp1374 == '\000'
 && _tmp1375 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1373.curr)=_tmp1375)))))));}{
struct _dyneither_ptr*_tmp1376;t->name=((_tmp1376=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1376[0]=(struct _dyneither_ptr)_tmp98E,_tmp1376))));};};}
struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple25*x){struct Cyc_Core_Opt*
_tmp1379;struct _tuple9*_tmp1378;return(_tmp1378=_cycalloc(sizeof(*_tmp1378)),((
_tmp1378->f1=(struct Cyc_Core_Opt*)((_tmp1379=_cycalloc(sizeof(*_tmp1379)),((
_tmp1379->v=(*x).f1,_tmp1379)))),((_tmp1378->f2=(*x).f2,((_tmp1378->f3=(*x).f3,
_tmp1378)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp996=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp137A;_tmp996=((_tmp137A=_cycalloc(sizeof(*_tmp137A)),((
_tmp137A->hd=(void*)atts->hd,((_tmp137A->tl=_tmp996,_tmp137A))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp1380;struct Cyc_Absyn_FnInfo _tmp137F;struct Cyc_Absyn_FnType_struct*_tmp137E;
return(void*)((_tmp137E=_cycalloc(sizeof(*_tmp137E)),((_tmp137E[0]=((_tmp1380.tag=
10,((_tmp1380.f1=((_tmp137F.tvars=fd->tvs,((_tmp137F.effect=fd->effect,((
_tmp137F.ret_tqual=fd->ret_tqual,((_tmp137F.ret_typ=fd->ret_type,((_tmp137F.args=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple25*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp137F.c_varargs=fd->c_varargs,((
_tmp137F.cyc_varargs=fd->cyc_varargs,((_tmp137F.rgn_po=fd->rgn_po,((_tmp137F.attributes=
_tmp996,_tmp137F)))))))))))))))))),_tmp1380)))),_tmp137E))));};}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple26{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple26*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct _tuple11*
_tmp1381;return(_tmp1381=_cycalloc(sizeof(*_tmp1381)),((_tmp1381->f1=(*pr).f1,((
_tmp1381->f2=t,_tmp1381)))));}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple28{struct _tuple27*f1;void*f2;};static struct _tuple28*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple9*y){struct _tuple27*_tmp1384;struct _tuple28*
_tmp1383;return(_tmp1383=_region_malloc(rgn,sizeof(*_tmp1383)),((_tmp1383->f1=((
_tmp1384=_region_malloc(rgn,sizeof(*_tmp1384)),((_tmp1384->f1=(*y).f1,((_tmp1384->f2=(*
y).f2,_tmp1384)))))),((_tmp1383->f2=(*y).f3,_tmp1383)))));}static struct _tuple9*
Cyc_Tcutil_substitute_f2(struct _tuple28*w){struct _tuple27*_tmp99F;void*_tmp9A0;
struct _tuple28 _tmp99E=*w;_tmp99F=_tmp99E.f1;_tmp9A0=_tmp99E.f2;{struct Cyc_Core_Opt*
_tmp9A2;struct Cyc_Absyn_Tqual _tmp9A3;struct _tuple27 _tmp9A1=*_tmp99F;_tmp9A2=
_tmp9A1.f1;_tmp9A3=_tmp9A1.f2;{struct _tuple9*_tmp1385;return(_tmp1385=_cycalloc(
sizeof(*_tmp1385)),((_tmp1385->f1=_tmp9A2,((_tmp1385->f2=_tmp9A3,((_tmp1385->f3=
_tmp9A0,_tmp1385)))))));};};}static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1386;return(
_tmp1386=_cycalloc(sizeof(*_tmp1386)),((_tmp1386->name=f->name,((_tmp1386->tq=f->tq,((
_tmp1386->type=t,((_tmp1386->width=f->width,((_tmp1386->attributes=f->attributes,
_tmp1386)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*
_tmp1387;return(_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387->topt=old->topt,((
_tmp1387->r=r,((_tmp1387->loc=old->loc,((_tmp1387->annot=old->annot,_tmp1387)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e){void*_tmp9A7=e->r;enum Cyc_Absyn_Primop _tmp9AD;
struct Cyc_List_List*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_Absyn_Exp*
_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B2;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_Absyn_Exp*
_tmp9B5;struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B8;struct Cyc_Absyn_Exp*
_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BB;void*_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BE;int
_tmp9BF;enum Cyc_Absyn_Coercion _tmp9C0;void*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C4;
void*_tmp9C6;void*_tmp9C7;void*_tmp9C9;_LL4F5: {struct Cyc_Absyn_Const_e_struct*
_tmp9A8=(struct Cyc_Absyn_Const_e_struct*)_tmp9A7;if(_tmp9A8->tag != 0)goto _LL4F7;}
_LL4F6: goto _LL4F8;_LL4F7: {struct Cyc_Absyn_Enum_e_struct*_tmp9A9=(struct Cyc_Absyn_Enum_e_struct*)
_tmp9A7;if(_tmp9A9->tag != 32)goto _LL4F9;}_LL4F8: goto _LL4FA;_LL4F9: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9AA=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9A7;if(_tmp9AA->tag != 33)goto
_LL4FB;}_LL4FA: goto _LL4FC;_LL4FB: {struct Cyc_Absyn_Var_e_struct*_tmp9AB=(struct
Cyc_Absyn_Var_e_struct*)_tmp9A7;if(_tmp9AB->tag != 1)goto _LL4FD;}_LL4FC: return e;
_LL4FD: {struct Cyc_Absyn_Primop_e_struct*_tmp9AC=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9A7;if(_tmp9AC->tag != 3)goto _LL4FF;else{_tmp9AD=_tmp9AC->f1;_tmp9AE=_tmp9AC->f2;}}
_LL4FE: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9AE)== 1){struct
Cyc_Absyn_Exp*_tmp9CA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp9AE))->hd;struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9CA);
if(_tmp9CB == _tmp9CA)return e;{struct Cyc_Absyn_Primop_e_struct _tmp138D;struct Cyc_Absyn_Exp*
_tmp138C[1];struct Cyc_Absyn_Primop_e_struct*_tmp138B;return Cyc_Tcutil_copye(e,(
void*)((_tmp138B=_cycalloc(sizeof(*_tmp138B)),((_tmp138B[0]=((_tmp138D.tag=3,((
_tmp138D.f1=_tmp9AD,((_tmp138D.f2=((_tmp138C[0]=_tmp9CB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp138C,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp138D)))))),_tmp138B)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp9AE)== 2){struct Cyc_Absyn_Exp*_tmp9CF=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9AE))->hd;struct Cyc_Absyn_Exp*_tmp9D0=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9AE->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9D1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9CF);struct Cyc_Absyn_Exp*
_tmp9D2=Cyc_Tcutil_rsubsexp(r,inst,_tmp9D0);if(_tmp9D1 == _tmp9CF  && _tmp9D2 == 
_tmp9D0)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1393;struct Cyc_Absyn_Exp*
_tmp1392[2];struct Cyc_Absyn_Primop_e_struct*_tmp1391;return Cyc_Tcutil_copye(e,(
void*)((_tmp1391=_cycalloc(sizeof(*_tmp1391)),((_tmp1391[0]=((_tmp1393.tag=3,((
_tmp1393.f1=_tmp9AD,((_tmp1393.f2=((_tmp1392[1]=_tmp9D2,((_tmp1392[0]=_tmp9D1,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1392,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1393)))))),_tmp1391)))));};}
else{const char*_tmp1396;void*_tmp1395;return(_tmp1395=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1396="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1396,sizeof(char),34))),_tag_dyneither(_tmp1395,sizeof(void*),
0)));}}_LL4FF: {struct Cyc_Absyn_Conditional_e_struct*_tmp9AF=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9A7;if(_tmp9AF->tag != 6)goto _LL501;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9AF->f2;
_tmp9B2=_tmp9AF->f3;}}_LL500: {struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(r,
inst,_tmp9B0);struct Cyc_Absyn_Exp*_tmp9D9=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B1);
struct Cyc_Absyn_Exp*_tmp9DA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B2);if((_tmp9D8 == 
_tmp9B0  && _tmp9D9 == _tmp9B1) && _tmp9DA == _tmp9B2)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1399;struct Cyc_Absyn_Conditional_e_struct*_tmp1398;return Cyc_Tcutil_copye(e,(
void*)((_tmp1398=_cycalloc(sizeof(*_tmp1398)),((_tmp1398[0]=((_tmp1399.tag=6,((
_tmp1399.f1=_tmp9D8,((_tmp1399.f2=_tmp9D9,((_tmp1399.f3=_tmp9DA,_tmp1399)))))))),
_tmp1398)))));};}_LL501: {struct Cyc_Absyn_And_e_struct*_tmp9B3=(struct Cyc_Absyn_And_e_struct*)
_tmp9A7;if(_tmp9B3->tag != 7)goto _LL503;else{_tmp9B4=_tmp9B3->f1;_tmp9B5=_tmp9B3->f2;}}
_LL502: {struct Cyc_Absyn_Exp*_tmp9DD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B4);struct
Cyc_Absyn_Exp*_tmp9DE=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B5);if(_tmp9DD == _tmp9B4
 && _tmp9DE == _tmp9B5)return e;{struct Cyc_Absyn_And_e_struct _tmp139C;struct Cyc_Absyn_And_e_struct*
_tmp139B;return Cyc_Tcutil_copye(e,(void*)((_tmp139B=_cycalloc(sizeof(*_tmp139B)),((
_tmp139B[0]=((_tmp139C.tag=7,((_tmp139C.f1=_tmp9DD,((_tmp139C.f2=_tmp9DE,
_tmp139C)))))),_tmp139B)))));};}_LL503: {struct Cyc_Absyn_Or_e_struct*_tmp9B6=(
struct Cyc_Absyn_Or_e_struct*)_tmp9A7;if(_tmp9B6->tag != 8)goto _LL505;else{_tmp9B7=
_tmp9B6->f1;_tmp9B8=_tmp9B6->f2;}}_LL504: {struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9B7);struct Cyc_Absyn_Exp*_tmp9E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B8);
if(_tmp9E1 == _tmp9B7  && _tmp9E2 == _tmp9B8)return e;{struct Cyc_Absyn_Or_e_struct
_tmp139F;struct Cyc_Absyn_Or_e_struct*_tmp139E;return Cyc_Tcutil_copye(e,(void*)((
_tmp139E=_cycalloc(sizeof(*_tmp139E)),((_tmp139E[0]=((_tmp139F.tag=8,((_tmp139F.f1=
_tmp9E1,((_tmp139F.f2=_tmp9E2,_tmp139F)))))),_tmp139E)))));};}_LL505: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9B9=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp9A7;if(_tmp9B9->tag != 9)goto _LL507;
else{_tmp9BA=_tmp9B9->f1;_tmp9BB=_tmp9B9->f2;}}_LL506: {struct Cyc_Absyn_Exp*
_tmp9E5=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BA);struct Cyc_Absyn_Exp*_tmp9E6=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9BB);if(_tmp9E5 == _tmp9BA  && _tmp9E6 == _tmp9BB)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp13A2;struct Cyc_Absyn_SeqExp_e_struct*_tmp13A1;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A1=_cycalloc(sizeof(*_tmp13A1)),((_tmp13A1[0]=((_tmp13A2.tag=9,((_tmp13A2.f1=
_tmp9E5,((_tmp13A2.f2=_tmp9E6,_tmp13A2)))))),_tmp13A1)))));};}_LL507: {struct Cyc_Absyn_Cast_e_struct*
_tmp9BC=(struct Cyc_Absyn_Cast_e_struct*)_tmp9A7;if(_tmp9BC->tag != 15)goto _LL509;
else{_tmp9BD=(void*)_tmp9BC->f1;_tmp9BE=_tmp9BC->f2;_tmp9BF=_tmp9BC->f3;_tmp9C0=
_tmp9BC->f4;}}_LL508: {struct Cyc_Absyn_Exp*_tmp9E9=Cyc_Tcutil_rsubsexp(r,inst,
_tmp9BE);void*_tmp9EA=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BD);if(_tmp9E9 == 
_tmp9BE  && _tmp9EA == _tmp9BD)return e;{struct Cyc_Absyn_Cast_e_struct _tmp13A5;
struct Cyc_Absyn_Cast_e_struct*_tmp13A4;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A4=_cycalloc(sizeof(*_tmp13A4)),((_tmp13A4[0]=((_tmp13A5.tag=15,((_tmp13A5.f1=(
void*)_tmp9EA,((_tmp13A5.f2=_tmp9E9,((_tmp13A5.f3=_tmp9BF,((_tmp13A5.f4=_tmp9C0,
_tmp13A5)))))))))),_tmp13A4)))));};}_LL509: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9C1=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9A7;if(_tmp9C1->tag != 18)goto
_LL50B;else{_tmp9C2=(void*)_tmp9C1->f1;}}_LL50A: {void*_tmp9ED=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9C2);if(_tmp9ED == _tmp9C2)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp13A8;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp13A7;return Cyc_Tcutil_copye(e,(
void*)((_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((_tmp13A7[0]=((_tmp13A8.tag=18,((
_tmp13A8.f1=(void*)_tmp9ED,_tmp13A8)))),_tmp13A7)))));};}_LL50B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9C3=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp9A7;if(_tmp9C3->tag != 19)goto
_LL50D;else{_tmp9C4=_tmp9C3->f1;}}_LL50C: {struct Cyc_Absyn_Exp*_tmp9F0=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9C4);if(_tmp9F0 == _tmp9C4)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp13AB;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp13AA;return Cyc_Tcutil_copye(e,(
void*)((_tmp13AA=_cycalloc(sizeof(*_tmp13AA)),((_tmp13AA[0]=((_tmp13AB.tag=19,((
_tmp13AB.f1=_tmp9F0,_tmp13AB)))),_tmp13AA)))));};}_LL50D: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9C5=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp9A7;if(_tmp9C5->tag != 20)goto
_LL50F;else{_tmp9C6=(void*)_tmp9C5->f1;_tmp9C7=(void*)_tmp9C5->f2;}}_LL50E: {
void*_tmp9F3=Cyc_Tcutil_rsubstitute(r,inst,_tmp9C6);if(_tmp9F3 == _tmp9C6)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp13AE;struct Cyc_Absyn_Offsetof_e_struct*
_tmp13AD;return Cyc_Tcutil_copye(e,(void*)((_tmp13AD=_cycalloc(sizeof(*_tmp13AD)),((
_tmp13AD[0]=((_tmp13AE.tag=20,((_tmp13AE.f1=(void*)_tmp9F3,((_tmp13AE.f2=(void*)
_tmp9C7,_tmp13AE)))))),_tmp13AD)))));};}_LL50F: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9C8=(struct Cyc_Absyn_Valueof_e_struct*)_tmp9A7;if(_tmp9C8->tag != 39)goto
_LL511;else{_tmp9C9=(void*)_tmp9C8->f1;}}_LL510: {void*_tmp9F6=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9C9);if(_tmp9F6 == _tmp9C9)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp13B1;struct Cyc_Absyn_Valueof_e_struct*_tmp13B0;return Cyc_Tcutil_copye(e,(
void*)((_tmp13B0=_cycalloc(sizeof(*_tmp13B0)),((_tmp13B0[0]=((_tmp13B1.tag=39,((
_tmp13B1.f1=(void*)_tmp9F6,_tmp13B1)))),_tmp13B0)))));};}_LL511:;_LL512: {const
char*_tmp13B4;void*_tmp13B3;return(_tmp13B3=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13B4="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp13B4,sizeof(char),46))),_tag_dyneither(_tmp13B3,sizeof(void*),
0)));}_LL4F4:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9FB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9FD;
struct Cyc_Absyn_AggrInfo _tmp9FF;union Cyc_Absyn_AggrInfoU _tmpA00;struct Cyc_List_List*
_tmpA01;struct Cyc_Absyn_DatatypeInfo _tmpA03;union Cyc_Absyn_DatatypeInfoU _tmpA04;
struct Cyc_List_List*_tmpA05;struct Cyc_Absyn_DatatypeFieldInfo _tmpA07;union Cyc_Absyn_DatatypeFieldInfoU
_tmpA08;struct Cyc_List_List*_tmpA09;struct _tuple2*_tmpA0B;struct Cyc_List_List*
_tmpA0C;struct Cyc_Absyn_Typedefdecl*_tmpA0D;void**_tmpA0E;struct Cyc_Absyn_ArrayInfo
_tmpA10;void*_tmpA11;struct Cyc_Absyn_Tqual _tmpA12;struct Cyc_Absyn_Exp*_tmpA13;
union Cyc_Absyn_Constraint*_tmpA14;struct Cyc_Position_Segment*_tmpA15;struct Cyc_Absyn_PtrInfo
_tmpA17;void*_tmpA18;struct Cyc_Absyn_Tqual _tmpA19;struct Cyc_Absyn_PtrAtts _tmpA1A;
void*_tmpA1B;union Cyc_Absyn_Constraint*_tmpA1C;union Cyc_Absyn_Constraint*_tmpA1D;
union Cyc_Absyn_Constraint*_tmpA1E;struct Cyc_Absyn_FnInfo _tmpA20;struct Cyc_List_List*
_tmpA21;struct Cyc_Core_Opt*_tmpA22;struct Cyc_Absyn_Tqual _tmpA23;void*_tmpA24;
struct Cyc_List_List*_tmpA25;int _tmpA26;struct Cyc_Absyn_VarargInfo*_tmpA27;struct
Cyc_List_List*_tmpA28;struct Cyc_List_List*_tmpA29;struct Cyc_List_List*_tmpA2B;
enum Cyc_Absyn_AggrKind _tmpA2D;struct Cyc_List_List*_tmpA2E;struct Cyc_Core_Opt*
_tmpA30;void*_tmpA32;void*_tmpA34;void*_tmpA35;void*_tmpA37;struct Cyc_Absyn_Exp*
_tmpA39;void*_tmpA44;void*_tmpA46;struct Cyc_List_List*_tmpA48;_LL514: {struct Cyc_Absyn_VarType_struct*
_tmp9FC=(struct Cyc_Absyn_VarType_struct*)_tmp9FB;if(_tmp9FC->tag != 2)goto _LL516;
else{_tmp9FD=_tmp9FC->f1;}}_LL515: {struct _handler_cons _tmpA4A;_push_handler(&
_tmpA4A);{int _tmpA4C=0;if(setjmp(_tmpA4A.handler))_tmpA4C=1;if(!_tmpA4C){{void*
_tmpA4D=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9FD);_npop_handler(0);return _tmpA4D;};_pop_handler();}else{void*_tmpA4B=(
void*)_exn_thrown;void*_tmpA4F=_tmpA4B;_LL54D: {struct Cyc_Core_Not_found_struct*
_tmpA50=(struct Cyc_Core_Not_found_struct*)_tmpA4F;if(_tmpA50->tag != Cyc_Core_Not_found)
goto _LL54F;}_LL54E: return t;_LL54F:;_LL550:(void)_throw(_tmpA4F);_LL54C:;}};}
_LL516: {struct Cyc_Absyn_AggrType_struct*_tmp9FE=(struct Cyc_Absyn_AggrType_struct*)
_tmp9FB;if(_tmp9FE->tag != 12)goto _LL518;else{_tmp9FF=_tmp9FE->f1;_tmpA00=_tmp9FF.aggr_info;
_tmpA01=_tmp9FF.targs;}}_LL517: {struct Cyc_List_List*_tmpA51=Cyc_Tcutil_substs(
rgn,inst,_tmpA01);struct Cyc_Absyn_AggrType_struct _tmp13BA;struct Cyc_Absyn_AggrInfo
_tmp13B9;struct Cyc_Absyn_AggrType_struct*_tmp13B8;return _tmpA51 == _tmpA01?t:(
void*)((_tmp13B8=_cycalloc(sizeof(*_tmp13B8)),((_tmp13B8[0]=((_tmp13BA.tag=12,((
_tmp13BA.f1=((_tmp13B9.aggr_info=_tmpA00,((_tmp13B9.targs=_tmpA51,_tmp13B9)))),
_tmp13BA)))),_tmp13B8))));}_LL518: {struct Cyc_Absyn_DatatypeType_struct*_tmpA02=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp9FB;if(_tmpA02->tag != 3)goto _LL51A;
else{_tmpA03=_tmpA02->f1;_tmpA04=_tmpA03.datatype_info;_tmpA05=_tmpA03.targs;}}
_LL519: {struct Cyc_List_List*_tmpA55=Cyc_Tcutil_substs(rgn,inst,_tmpA05);struct
Cyc_Absyn_DatatypeType_struct _tmp13C0;struct Cyc_Absyn_DatatypeInfo _tmp13BF;
struct Cyc_Absyn_DatatypeType_struct*_tmp13BE;return _tmpA55 == _tmpA05?t:(void*)((
_tmp13BE=_cycalloc(sizeof(*_tmp13BE)),((_tmp13BE[0]=((_tmp13C0.tag=3,((_tmp13C0.f1=((
_tmp13BF.datatype_info=_tmpA04,((_tmp13BF.targs=_tmpA55,_tmp13BF)))),_tmp13C0)))),
_tmp13BE))));}_LL51A: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpA06=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp9FB;if(_tmpA06->tag != 4)goto _LL51C;else{
_tmpA07=_tmpA06->f1;_tmpA08=_tmpA07.field_info;_tmpA09=_tmpA07.targs;}}_LL51B: {
struct Cyc_List_List*_tmpA59=Cyc_Tcutil_substs(rgn,inst,_tmpA09);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp13C6;struct Cyc_Absyn_DatatypeFieldInfo _tmp13C5;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13C4;return _tmpA59 == _tmpA09?t:(void*)((_tmp13C4=_cycalloc(sizeof(*_tmp13C4)),((
_tmp13C4[0]=((_tmp13C6.tag=4,((_tmp13C6.f1=((_tmp13C5.field_info=_tmpA08,((
_tmp13C5.targs=_tmpA59,_tmp13C5)))),_tmp13C6)))),_tmp13C4))));}_LL51C: {struct
Cyc_Absyn_TypedefType_struct*_tmpA0A=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9FB;if(_tmpA0A->tag != 18)goto _LL51E;else{_tmpA0B=_tmpA0A->f1;_tmpA0C=_tmpA0A->f2;
_tmpA0D=_tmpA0A->f3;_tmpA0E=_tmpA0A->f4;}}_LL51D: {struct Cyc_List_List*_tmpA5D=
Cyc_Tcutil_substs(rgn,inst,_tmpA0C);struct Cyc_Absyn_TypedefType_struct _tmp13C9;
struct Cyc_Absyn_TypedefType_struct*_tmp13C8;return _tmpA5D == _tmpA0C?t:(void*)((
_tmp13C8=_cycalloc(sizeof(*_tmp13C8)),((_tmp13C8[0]=((_tmp13C9.tag=18,((_tmp13C9.f1=
_tmpA0B,((_tmp13C9.f2=_tmpA5D,((_tmp13C9.f3=_tmpA0D,((_tmp13C9.f4=_tmpA0E,
_tmp13C9)))))))))),_tmp13C8))));}_LL51E: {struct Cyc_Absyn_ArrayType_struct*
_tmpA0F=(struct Cyc_Absyn_ArrayType_struct*)_tmp9FB;if(_tmpA0F->tag != 9)goto
_LL520;else{_tmpA10=_tmpA0F->f1;_tmpA11=_tmpA10.elt_type;_tmpA12=_tmpA10.tq;
_tmpA13=_tmpA10.num_elts;_tmpA14=_tmpA10.zero_term;_tmpA15=_tmpA10.zt_loc;}}
_LL51F: {void*_tmpA60=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA11);struct Cyc_Absyn_ArrayType_struct
_tmp13CF;struct Cyc_Absyn_ArrayInfo _tmp13CE;struct Cyc_Absyn_ArrayType_struct*
_tmp13CD;return _tmpA60 == _tmpA11?t:(void*)((_tmp13CD=_cycalloc(sizeof(*_tmp13CD)),((
_tmp13CD[0]=((_tmp13CF.tag=9,((_tmp13CF.f1=((_tmp13CE.elt_type=_tmpA60,((
_tmp13CE.tq=_tmpA12,((_tmp13CE.num_elts=_tmpA13,((_tmp13CE.zero_term=_tmpA14,((
_tmp13CE.zt_loc=_tmpA15,_tmp13CE)))))))))),_tmp13CF)))),_tmp13CD))));}_LL520: {
struct Cyc_Absyn_PointerType_struct*_tmpA16=(struct Cyc_Absyn_PointerType_struct*)
_tmp9FB;if(_tmpA16->tag != 5)goto _LL522;else{_tmpA17=_tmpA16->f1;_tmpA18=_tmpA17.elt_typ;
_tmpA19=_tmpA17.elt_tq;_tmpA1A=_tmpA17.ptr_atts;_tmpA1B=_tmpA1A.rgn;_tmpA1C=
_tmpA1A.nullable;_tmpA1D=_tmpA1A.bounds;_tmpA1E=_tmpA1A.zero_term;}}_LL521: {
void*_tmpA64=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA18);void*_tmpA65=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA1B);union Cyc_Absyn_Constraint*_tmpA66=_tmpA1D;{void*_tmpA67=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA1D);struct Cyc_Absyn_Exp*_tmpA69;_LL552: {struct Cyc_Absyn_Upper_b_struct*
_tmpA68=(struct Cyc_Absyn_Upper_b_struct*)_tmpA67;if(_tmpA68->tag != 1)goto _LL554;
else{_tmpA69=_tmpA68->f1;}}_LL553: {struct Cyc_Absyn_Exp*_tmpA6A=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA69);if(_tmpA6A != _tmpA69){struct Cyc_Absyn_Upper_b_struct _tmp13D2;
struct Cyc_Absyn_Upper_b_struct*_tmp13D1;_tmpA66=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13D1=_cycalloc(sizeof(*_tmp13D1)),((
_tmp13D1[0]=((_tmp13D2.tag=1,((_tmp13D2.f1=_tmpA6A,_tmp13D2)))),_tmp13D1)))));}
goto _LL551;}_LL554:;_LL555: goto _LL551;_LL551:;}if((_tmpA64 == _tmpA18  && _tmpA65
== _tmpA1B) && _tmpA66 == _tmpA1D)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13DC;struct Cyc_Absyn_PtrAtts _tmp13DB;struct Cyc_Absyn_PtrInfo _tmp13DA;struct
Cyc_Absyn_PointerType_struct*_tmp13D9;return(void*)((_tmp13D9=_cycalloc(sizeof(*
_tmp13D9)),((_tmp13D9[0]=((_tmp13DC.tag=5,((_tmp13DC.f1=((_tmp13DA.elt_typ=
_tmpA64,((_tmp13DA.elt_tq=_tmpA19,((_tmp13DA.ptr_atts=((_tmp13DB.rgn=_tmpA65,((
_tmp13DB.nullable=_tmpA1C,((_tmp13DB.bounds=_tmpA66,((_tmp13DB.zero_term=_tmpA1E,((
_tmp13DB.ptrloc=0,_tmp13DB)))))))))),_tmp13DA)))))),_tmp13DC)))),_tmp13D9))));};}
_LL522: {struct Cyc_Absyn_FnType_struct*_tmpA1F=(struct Cyc_Absyn_FnType_struct*)
_tmp9FB;if(_tmpA1F->tag != 10)goto _LL524;else{_tmpA20=_tmpA1F->f1;_tmpA21=_tmpA20.tvars;
_tmpA22=_tmpA20.effect;_tmpA23=_tmpA20.ret_tqual;_tmpA24=_tmpA20.ret_typ;_tmpA25=
_tmpA20.args;_tmpA26=_tmpA20.c_varargs;_tmpA27=_tmpA20.cyc_varargs;_tmpA28=
_tmpA20.rgn_po;_tmpA29=_tmpA20.attributes;}}_LL523:{struct Cyc_List_List*_tmpA71=
_tmpA21;for(0;_tmpA71 != 0;_tmpA71=_tmpA71->tl){struct _tuple15*_tmp13E6;struct Cyc_Absyn_VarType_struct
_tmp13E5;struct Cyc_Absyn_VarType_struct*_tmp13E4;struct Cyc_List_List*_tmp13E3;
inst=((_tmp13E3=_region_malloc(rgn,sizeof(*_tmp13E3)),((_tmp13E3->hd=((_tmp13E6=
_region_malloc(rgn,sizeof(*_tmp13E6)),((_tmp13E6->f1=(struct Cyc_Absyn_Tvar*)
_tmpA71->hd,((_tmp13E6->f2=(void*)((_tmp13E4=_cycalloc(sizeof(*_tmp13E4)),((
_tmp13E4[0]=((_tmp13E5.tag=2,((_tmp13E5.f1=(struct Cyc_Absyn_Tvar*)_tmpA71->hd,
_tmp13E5)))),_tmp13E4)))),_tmp13E6)))))),((_tmp13E3->tl=inst,_tmp13E3))))));}}{
struct Cyc_List_List*_tmpA77;struct Cyc_List_List*_tmpA78;struct _tuple1 _tmpA76=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA25));
_tmpA77=_tmpA76.f1;_tmpA78=_tmpA76.f2;{struct Cyc_List_List*_tmpA79=Cyc_Tcutil_substs(
rgn,inst,_tmpA78);struct Cyc_List_List*_tmpA7A=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA77,_tmpA79));struct Cyc_Core_Opt*
eff2;if(_tmpA22 == 0)eff2=0;else{void*_tmpA7B=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA22->v);if(_tmpA7B == (void*)_tmpA22->v)eff2=_tmpA22;else{struct Cyc_Core_Opt*
_tmp13E7;eff2=((_tmp13E7=_cycalloc(sizeof(*_tmp13E7)),((_tmp13E7->v=_tmpA7B,
_tmp13E7))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA27 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA7E;struct Cyc_Absyn_Tqual _tmpA7F;void*
_tmpA80;int _tmpA81;struct Cyc_Absyn_VarargInfo _tmpA7D=*_tmpA27;_tmpA7E=_tmpA7D.name;
_tmpA7F=_tmpA7D.tq;_tmpA80=_tmpA7D.type;_tmpA81=_tmpA7D.inject;{void*_tmpA82=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA80);if(_tmpA82 == _tmpA80)cyc_varargs2=_tmpA27;else{struct Cyc_Absyn_VarargInfo*
_tmp13E8;cyc_varargs2=((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((_tmp13E8->name=
_tmpA7E,((_tmp13E8->tq=_tmpA7F,((_tmp13E8->type=_tmpA82,((_tmp13E8->inject=
_tmpA81,_tmp13E8))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA85;struct Cyc_List_List*_tmpA86;struct _tuple1 _tmpA84=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA28);_tmpA85=_tmpA84.f1;_tmpA86=_tmpA84.f2;{struct Cyc_List_List*
_tmpA87=Cyc_Tcutil_substs(rgn,inst,_tmpA85);struct Cyc_List_List*_tmpA88=Cyc_Tcutil_substs(
rgn,inst,_tmpA86);if(_tmpA87 == _tmpA85  && _tmpA88 == _tmpA86)rgn_po2=_tmpA28;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA87,_tmpA88);}{struct Cyc_Absyn_FnType_struct _tmp13EE;struct
Cyc_Absyn_FnInfo _tmp13ED;struct Cyc_Absyn_FnType_struct*_tmp13EC;return(void*)((
_tmp13EC=_cycalloc(sizeof(*_tmp13EC)),((_tmp13EC[0]=((_tmp13EE.tag=10,((_tmp13EE.f1=((
_tmp13ED.tvars=_tmpA21,((_tmp13ED.effect=eff2,((_tmp13ED.ret_tqual=_tmpA23,((
_tmp13ED.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA24),((_tmp13ED.args=
_tmpA7A,((_tmp13ED.c_varargs=_tmpA26,((_tmp13ED.cyc_varargs=cyc_varargs2,((
_tmp13ED.rgn_po=rgn_po2,((_tmp13ED.attributes=_tmpA29,_tmp13ED)))))))))))))))))),
_tmp13EE)))),_tmp13EC))));};};};};};};_LL524: {struct Cyc_Absyn_TupleType_struct*
_tmpA2A=(struct Cyc_Absyn_TupleType_struct*)_tmp9FB;if(_tmpA2A->tag != 11)goto
_LL526;else{_tmpA2B=_tmpA2A->f1;}}_LL525: {struct Cyc_List_List*_tmpA8C=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA2B);struct Cyc_List_List*_tmpA8D=
Cyc_Tcutil_substs(rgn,inst,_tmpA8C);if(_tmpA8D == _tmpA8C)return t;{struct Cyc_List_List*
_tmpA8E=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA2B,_tmpA8D);struct Cyc_Absyn_TupleType_struct _tmp13F1;struct Cyc_Absyn_TupleType_struct*
_tmp13F0;return(void*)((_tmp13F0=_cycalloc(sizeof(*_tmp13F0)),((_tmp13F0[0]=((
_tmp13F1.tag=11,((_tmp13F1.f1=_tmpA8E,_tmp13F1)))),_tmp13F0))));};}_LL526: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA2C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9FB;if(_tmpA2C->tag != 13)goto _LL528;else{_tmpA2D=_tmpA2C->f1;_tmpA2E=_tmpA2C->f2;}}
_LL527: {struct Cyc_List_List*_tmpA91=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA2E);struct Cyc_List_List*_tmpA92=Cyc_Tcutil_substs(
rgn,inst,_tmpA91);if(_tmpA92 == _tmpA91)return t;{struct Cyc_List_List*_tmpA93=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA2E,_tmpA92);struct Cyc_Absyn_AnonAggrType_struct _tmp13F4;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13F3;return(void*)((_tmp13F3=_cycalloc(sizeof(*_tmp13F3)),((_tmp13F3[0]=((
_tmp13F4.tag=13,((_tmp13F4.f1=_tmpA2D,((_tmp13F4.f2=_tmpA93,_tmp13F4)))))),
_tmp13F3))));};}_LL528: {struct Cyc_Absyn_Evar_struct*_tmpA2F=(struct Cyc_Absyn_Evar_struct*)
_tmp9FB;if(_tmpA2F->tag != 1)goto _LL52A;else{_tmpA30=_tmpA2F->f2;}}_LL529: if(
_tmpA30 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA30->v);else{return
t;}_LL52A: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA31=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9FB;if(_tmpA31->tag != 16)goto _LL52C;else{_tmpA32=(void*)_tmpA31->f1;}}_LL52B: {
void*_tmpA96=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA32);struct Cyc_Absyn_RgnHandleType_struct
_tmp13F7;struct Cyc_Absyn_RgnHandleType_struct*_tmp13F6;return _tmpA96 == _tmpA32?t:(
void*)((_tmp13F6=_cycalloc(sizeof(*_tmp13F6)),((_tmp13F6[0]=((_tmp13F7.tag=16,((
_tmp13F7.f1=(void*)_tmpA96,_tmp13F7)))),_tmp13F6))));}_LL52C: {struct Cyc_Absyn_DynRgnType_struct*
_tmpA33=(struct Cyc_Absyn_DynRgnType_struct*)_tmp9FB;if(_tmpA33->tag != 17)goto
_LL52E;else{_tmpA34=(void*)_tmpA33->f1;_tmpA35=(void*)_tmpA33->f2;}}_LL52D: {
void*_tmpA99=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA34);void*_tmpA9A=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA35);struct Cyc_Absyn_DynRgnType_struct _tmp13FA;struct Cyc_Absyn_DynRgnType_struct*
_tmp13F9;return _tmpA99 == _tmpA34  && _tmpA9A == _tmpA35?t:(void*)((_tmp13F9=
_cycalloc(sizeof(*_tmp13F9)),((_tmp13F9[0]=((_tmp13FA.tag=17,((_tmp13FA.f1=(void*)
_tmpA99,((_tmp13FA.f2=(void*)_tmpA9A,_tmp13FA)))))),_tmp13F9))));}_LL52E: {
struct Cyc_Absyn_TagType_struct*_tmpA36=(struct Cyc_Absyn_TagType_struct*)_tmp9FB;
if(_tmpA36->tag != 20)goto _LL530;else{_tmpA37=(void*)_tmpA36->f1;}}_LL52F: {void*
_tmpA9D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA37);struct Cyc_Absyn_TagType_struct
_tmp13FD;struct Cyc_Absyn_TagType_struct*_tmp13FC;return _tmpA9D == _tmpA37?t:(void*)((
_tmp13FC=_cycalloc(sizeof(*_tmp13FC)),((_tmp13FC[0]=((_tmp13FD.tag=20,((_tmp13FD.f1=(
void*)_tmpA9D,_tmp13FD)))),_tmp13FC))));}_LL530: {struct Cyc_Absyn_ValueofType_struct*
_tmpA38=(struct Cyc_Absyn_ValueofType_struct*)_tmp9FB;if(_tmpA38->tag != 19)goto
_LL532;else{_tmpA39=_tmpA38->f1;}}_LL531: {struct Cyc_Absyn_Exp*_tmpAA0=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA39);struct Cyc_Absyn_ValueofType_struct _tmp1400;struct Cyc_Absyn_ValueofType_struct*
_tmp13FF;return _tmpAA0 == _tmpA39?t:(void*)((_tmp13FF=_cycalloc(sizeof(*_tmp13FF)),((
_tmp13FF[0]=((_tmp1400.tag=19,((_tmp1400.f1=_tmpAA0,_tmp1400)))),_tmp13FF))));}
_LL532: {struct Cyc_Absyn_EnumType_struct*_tmpA3A=(struct Cyc_Absyn_EnumType_struct*)
_tmp9FB;if(_tmpA3A->tag != 14)goto _LL534;}_LL533: goto _LL535;_LL534: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpA3B=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp9FB;if(_tmpA3B->tag != 15)goto
_LL536;}_LL535: goto _LL537;_LL536: {struct Cyc_Absyn_VoidType_struct*_tmpA3C=(
struct Cyc_Absyn_VoidType_struct*)_tmp9FB;if(_tmpA3C->tag != 0)goto _LL538;}_LL537:
goto _LL539;_LL538: {struct Cyc_Absyn_IntType_struct*_tmpA3D=(struct Cyc_Absyn_IntType_struct*)
_tmp9FB;if(_tmpA3D->tag != 6)goto _LL53A;}_LL539: goto _LL53B;_LL53A: {struct Cyc_Absyn_FloatType_struct*
_tmpA3E=(struct Cyc_Absyn_FloatType_struct*)_tmp9FB;if(_tmpA3E->tag != 7)goto
_LL53C;}_LL53B: goto _LL53D;_LL53C: {struct Cyc_Absyn_DoubleType_struct*_tmpA3F=(
struct Cyc_Absyn_DoubleType_struct*)_tmp9FB;if(_tmpA3F->tag != 8)goto _LL53E;}
_LL53D: goto _LL53F;_LL53E: {struct Cyc_Absyn_RefCntRgn_struct*_tmpA40=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp9FB;if(_tmpA40->tag != 23)goto _LL540;}_LL53F: goto _LL541;_LL540: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpA41=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp9FB;if(_tmpA41->tag != 22)goto
_LL542;}_LL541: goto _LL543;_LL542: {struct Cyc_Absyn_HeapRgn_struct*_tmpA42=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp9FB;if(_tmpA42->tag != 21)goto _LL544;}_LL543:
return t;_LL544: {struct Cyc_Absyn_RgnsEff_struct*_tmpA43=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9FB;if(_tmpA43->tag != 26)goto _LL546;else{_tmpA44=(void*)_tmpA43->f1;}}_LL545: {
void*_tmpAA3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA44);struct Cyc_Absyn_RgnsEff_struct
_tmp1403;struct Cyc_Absyn_RgnsEff_struct*_tmp1402;return _tmpAA3 == _tmpA44?t:(void*)((
_tmp1402=_cycalloc(sizeof(*_tmp1402)),((_tmp1402[0]=((_tmp1403.tag=26,((_tmp1403.f1=(
void*)_tmpAA3,_tmp1403)))),_tmp1402))));}_LL546: {struct Cyc_Absyn_AccessEff_struct*
_tmpA45=(struct Cyc_Absyn_AccessEff_struct*)_tmp9FB;if(_tmpA45->tag != 24)goto
_LL548;else{_tmpA46=(void*)_tmpA45->f1;}}_LL547: {void*_tmpAA6=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA46);struct Cyc_Absyn_AccessEff_struct _tmp1406;struct Cyc_Absyn_AccessEff_struct*
_tmp1405;return _tmpAA6 == _tmpA46?t:(void*)((_tmp1405=_cycalloc(sizeof(*_tmp1405)),((
_tmp1405[0]=((_tmp1406.tag=24,((_tmp1406.f1=(void*)_tmpAA6,_tmp1406)))),_tmp1405))));}
_LL548: {struct Cyc_Absyn_JoinEff_struct*_tmpA47=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9FB;if(_tmpA47->tag != 25)goto _LL54A;else{_tmpA48=_tmpA47->f1;}}_LL549: {
struct Cyc_List_List*_tmpAA9=Cyc_Tcutil_substs(rgn,inst,_tmpA48);struct Cyc_Absyn_JoinEff_struct
_tmp1409;struct Cyc_Absyn_JoinEff_struct*_tmp1408;return _tmpAA9 == _tmpA48?t:(void*)((
_tmp1408=_cycalloc(sizeof(*_tmp1408)),((_tmp1408[0]=((_tmp1409.tag=25,((_tmp1409.f1=
_tmpAA9,_tmp1409)))),_tmp1408))));}_LL54A: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpA49=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp9FB;if(_tmpA49->tag != 27)goto
_LL513;}_LL54B: {const char*_tmp140C;void*_tmp140B;(_tmp140B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140C="found typedecltype in rsubs",
_tag_dyneither(_tmp140C,sizeof(char),28))),_tag_dyneither(_tmp140B,sizeof(void*),
0)));}_LL513:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*
rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*
_tmpAAE=(void*)ts->hd;struct Cyc_List_List*_tmpAAF=ts->tl;void*_tmpAB0=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpAAE);struct Cyc_List_List*_tmpAB1=Cyc_Tcutil_substs(rgn,inst,_tmpAAF);
if(_tmpAAE == _tmpAB0  && _tmpAAF == _tmpAB1)return ts;{struct Cyc_List_List*_tmp140D;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp140D=_cycalloc(sizeof(*
_tmp140D)),((_tmp140D->hd=_tmpAB0,((_tmp140D->tl=_tmpAB1,_tmp140D)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpAB3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1410;struct _tuple15*_tmp140F;return(
_tmp140F=_cycalloc(sizeof(*_tmp140F)),((_tmp140F->f1=tv,((_tmp140F->f2=Cyc_Absyn_new_evar(
_tmpAB3,((_tmp1410=_cycalloc(sizeof(*_tmp1410)),((_tmp1410->v=s,_tmp1410))))),
_tmp140F)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpAB7;struct Cyc_List_List*_tmpAB8;
struct _RegionHandle*_tmpAB9;struct _tuple16*_tmpAB6=env;_tmpAB7=*_tmpAB6;_tmpAB8=
_tmpAB7.f1;_tmpAB9=_tmpAB7.f2;{struct Cyc_Core_Opt*_tmpABA=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1413;struct
_tuple15*_tmp1412;return(_tmp1412=_region_malloc(_tmpAB9,sizeof(*_tmp1412)),((
_tmp1412->f1=tv,((_tmp1412->f2=Cyc_Absyn_new_evar(_tmpABA,((_tmp1413=_cycalloc(
sizeof(*_tmp1413)),((_tmp1413->v=_tmpAB8,_tmp1413))))),_tmp1412)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp1419;void*_tmp1418[3];
struct Cyc_String_pa_struct _tmp1417;struct Cyc_String_pa_struct _tmp1416;struct Cyc_String_pa_struct
_tmp1415;(_tmp1415.tag=0,((_tmp1415.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp1416.tag=0,((_tmp1416.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp1417.tag=0,((_tmp1417.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp1418[0]=& _tmp1417,((_tmp1418[1]=& _tmp1416,((_tmp1418[2]=& _tmp1415,Cyc_Tcutil_terr(
loc,((_tmp1419="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1419,sizeof(char),59))),_tag_dyneither(_tmp1418,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp141C;void*_tmp141B;(_tmp141B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141C="same type variable has different identity!",
_tag_dyneither(_tmp141C,sizeof(char),43))),_tag_dyneither(_tmp141B,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp141D;return(_tmp141D=_cycalloc(sizeof(*_tmp141D)),((_tmp141D->hd=tv,((
_tmp141D->tl=tvs,_tmp141D)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1420;void*_tmp141F;(_tmp141F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1420="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp1420,sizeof(char),39))),_tag_dyneither(_tmp141F,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAC7=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAC7->identity == - 1){const char*
_tmp1423;void*_tmp1422;(_tmp1422=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1423="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1423,sizeof(char),41))),_tag_dyneither(_tmp1422,sizeof(void*),
0)));}if(_tmpAC7->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1424;return(_tmp1424=_cycalloc(sizeof(*_tmp1424)),((_tmp1424->hd=tv,((
_tmp1424->tl=tvs,_tmp1424)))));};}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1427;void*_tmp1426;(_tmp1426=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1427="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1427,sizeof(char),44))),_tag_dyneither(_tmp1426,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple29
_tmpACE;struct Cyc_Absyn_Tvar*_tmpACF;int _tmpAD0;int*_tmpAD1;struct _tuple29*
_tmpACD=(struct _tuple29*)tvs2->hd;_tmpACE=*_tmpACD;_tmpACF=_tmpACE.f1;_tmpAD0=
_tmpACE.f2;_tmpAD1=(int*)&(*_tmpACD).f2;if(_tmpACF->identity == - 1){const char*
_tmp142A;void*_tmp1429;(_tmp1429=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp142A="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp142A,sizeof(char),46))),_tag_dyneither(_tmp1429,sizeof(void*),
0)));}if(_tmpACF->identity == tv->identity){*_tmpAD1=*_tmpAD1  || b;return tvs;}}}{
struct _tuple29*_tmp142D;struct Cyc_List_List*_tmp142C;return(_tmp142C=
_region_malloc(r,sizeof(*_tmp142C)),((_tmp142C->hd=((_tmp142D=_region_malloc(r,
sizeof(*_tmp142D)),((_tmp142D->f1=tv,((_tmp142D->f2=b,_tmp142D)))))),((_tmp142C->tl=
tvs,_tmp142C)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1431;void*_tmp1430[1];struct Cyc_String_pa_struct _tmp142F;(_tmp142F.tag=0,((
_tmp142F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp1430[0]=& _tmp142F,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1431="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1431,sizeof(char),29))),_tag_dyneither(_tmp1430,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1432;return(_tmp1432=_cycalloc(sizeof(*
_tmp1432)),((_tmp1432->hd=tv,((_tmp1432->tl=tvs,_tmp1432)))));};}struct _tuple30{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpADA=Cyc_Tcutil_compress(
e);int _tmpADC;_LL557: {struct Cyc_Absyn_Evar_struct*_tmpADB=(struct Cyc_Absyn_Evar_struct*)
_tmpADA;if(_tmpADB->tag != 1)goto _LL559;else{_tmpADC=_tmpADB->f3;}}_LL558:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple30 _tmpADE;void*
_tmpADF;int _tmpAE0;int*_tmpAE1;struct _tuple30*_tmpADD=(struct _tuple30*)es2->hd;
_tmpADE=*_tmpADD;_tmpADF=_tmpADE.f1;_tmpAE0=_tmpADE.f2;_tmpAE1=(int*)&(*_tmpADD).f2;{
void*_tmpAE2=Cyc_Tcutil_compress(_tmpADF);int _tmpAE4;_LL55C: {struct Cyc_Absyn_Evar_struct*
_tmpAE3=(struct Cyc_Absyn_Evar_struct*)_tmpAE2;if(_tmpAE3->tag != 1)goto _LL55E;
else{_tmpAE4=_tmpAE3->f3;}}_LL55D: if(_tmpADC == _tmpAE4){if(b != *_tmpAE1)*_tmpAE1=
1;return es;}goto _LL55B;_LL55E:;_LL55F: goto _LL55B;_LL55B:;};}}{struct _tuple30*
_tmp1435;struct Cyc_List_List*_tmp1434;return(_tmp1434=_region_malloc(r,sizeof(*
_tmp1434)),((_tmp1434->hd=((_tmp1435=_region_malloc(r,sizeof(*_tmp1435)),((
_tmp1435->f1=e,((_tmp1435->f2=b,_tmp1435)))))),((_tmp1434->tl=es,_tmp1434)))));};
_LL559:;_LL55A: return es;_LL556:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1436;r=((_tmp1436=_region_malloc(rgn,sizeof(*_tmp1436)),((
_tmp1436->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1436->tl=r,_tmp1436))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAE9;int _tmpAEA;struct
_tuple29 _tmpAE8=*((struct _tuple29*)tvs->hd);_tmpAE9=_tmpAE8.f1;_tmpAEA=_tmpAE8.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpAE9->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1437;res=((_tmp1437=_region_malloc(r,sizeof(*_tmp1437)),((
_tmp1437->hd=(struct _tuple29*)tvs->hd,((_tmp1437->tl=res,_tmp1437))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp143B;void*_tmp143A[1];struct Cyc_String_pa_struct _tmp1439;(_tmp1439.tag=
0,((_tmp1439.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp143A[0]=&
_tmp1439,Cyc_Tcutil_terr(loc,((_tmp143B="bitfield %s does not have constant width",
_tag_dyneither(_tmp143B,sizeof(char),41))),_tag_dyneither(_tmp143A,sizeof(void*),
1)))))));}else{unsigned int _tmpAF0;int _tmpAF1;struct _tuple13 _tmpAEF=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAF0=_tmpAEF.f1;_tmpAF1=_tmpAEF.f2;if(!_tmpAF1){
const char*_tmp143E;void*_tmp143D;(_tmp143D=0,Cyc_Tcutil_terr(loc,((_tmp143E="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp143E,sizeof(char),45))),_tag_dyneither(_tmp143D,sizeof(void*),
0)));}w=_tmpAF0;}{void*_tmpAF4=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAF6;_LL561: {struct Cyc_Absyn_IntType_struct*_tmpAF5=(struct Cyc_Absyn_IntType_struct*)
_tmpAF4;if(_tmpAF5->tag != 6)goto _LL563;else{_tmpAF6=_tmpAF5->f2;}}_LL562: switch(
_tmpAF6){case Cyc_Absyn_Char_sz: _LL565: if(w > 8){const char*_tmp1441;void*_tmp1440;(
_tmp1440=0,Cyc_Tcutil_terr(loc,((_tmp1441="bitfield larger than type",
_tag_dyneither(_tmp1441,sizeof(char),26))),_tag_dyneither(_tmp1440,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL566: if(w > 16){const char*_tmp1444;void*
_tmp1443;(_tmp1443=0,Cyc_Tcutil_terr(loc,((_tmp1444="bitfield larger than type",
_tag_dyneither(_tmp1444,sizeof(char),26))),_tag_dyneither(_tmp1443,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL567: goto _LL568;case Cyc_Absyn_Int_sz: _LL568:
if(w > 32){const char*_tmp1447;void*_tmp1446;(_tmp1446=0,Cyc_Tcutil_terr(loc,((
_tmp1447="bitfield larger than type",_tag_dyneither(_tmp1447,sizeof(char),26))),
_tag_dyneither(_tmp1446,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL569: if(w > 64){const char*_tmp144A;void*_tmp1449;(_tmp1449=0,Cyc_Tcutil_terr(
loc,((_tmp144A="bitfield larger than type",_tag_dyneither(_tmp144A,sizeof(char),
26))),_tag_dyneither(_tmp1449,sizeof(void*),0)));}break;}goto _LL560;_LL563:;
_LL564:{const char*_tmp144F;void*_tmp144E[2];struct Cyc_String_pa_struct _tmp144D;
struct Cyc_String_pa_struct _tmp144C;(_tmp144C.tag=0,((_tmp144C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp144D.tag=0,((_tmp144D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp144E[0]=& _tmp144D,((_tmp144E[1]=& _tmp144C,Cyc_Tcutil_terr(loc,((_tmp144F="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp144F,sizeof(char),52))),_tag_dyneither(_tmp144E,sizeof(void*),
2)))))))))))));}goto _LL560;_LL560:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpB03=(void*)atts->hd;_LL56C: {struct Cyc_Absyn_Packed_att_struct*
_tmpB04=(struct Cyc_Absyn_Packed_att_struct*)_tmpB03;if(_tmpB04->tag != 7)goto
_LL56E;}_LL56D: continue;_LL56E: {struct Cyc_Absyn_Aligned_att_struct*_tmpB05=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpB03;if(_tmpB05->tag != 6)goto _LL570;}
_LL56F: continue;_LL570:;_LL571: {const char*_tmp1454;void*_tmp1453[2];struct Cyc_String_pa_struct
_tmp1452;struct Cyc_String_pa_struct _tmp1451;(_tmp1451.tag=0,((_tmp1451.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1452.tag=0,((_tmp1452.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1453[0]=& _tmp1452,((_tmp1453[1]=& _tmp1451,Cyc_Tcutil_terr(loc,((
_tmp1454="bad attribute %s on member %s",_tag_dyneither(_tmp1454,sizeof(char),30))),
_tag_dyneither(_tmp1453,sizeof(void*),2)))))))))))));}_LL56B:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpB0A=t;struct
Cyc_Absyn_Typedefdecl*_tmpB0C;void**_tmpB0D;_LL573: {struct Cyc_Absyn_TypedefType_struct*
_tmpB0B=(struct Cyc_Absyn_TypedefType_struct*)_tmpB0A;if(_tmpB0B->tag != 18)goto
_LL575;else{_tmpB0C=_tmpB0B->f3;_tmpB0D=_tmpB0B->f4;}}_LL574: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpB0C))->tq).real_const  || (_tmpB0C->tq).print_const){if(
declared_const){const char*_tmp1457;void*_tmp1456;(_tmp1456=0,Cyc_Tcutil_warn(loc,((
_tmp1457="extra const",_tag_dyneither(_tmp1457,sizeof(char),12))),_tag_dyneither(
_tmp1456,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpB0D)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpB0D);else{return declared_const;}_LL575:;_LL576: return
declared_const;_LL572:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpB10=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpB12;struct Cyc_Absyn_PtrAtts _tmpB13;void*_tmpB14;
_LL578: {struct Cyc_Absyn_PointerType_struct*_tmpB11=(struct Cyc_Absyn_PointerType_struct*)
_tmpB10;if(_tmpB11->tag != 5)goto _LL57A;else{_tmpB12=_tmpB11->f1;_tmpB13=_tmpB12.ptr_atts;
_tmpB14=_tmpB13.rgn;}}_LL579:{void*_tmpB15=Cyc_Tcutil_compress(_tmpB14);struct
Cyc_Absyn_Tvar*_tmpB17;_LL57D: {struct Cyc_Absyn_VarType_struct*_tmpB16=(struct
Cyc_Absyn_VarType_struct*)_tmpB15;if(_tmpB16->tag != 2)goto _LL57F;else{_tmpB17=
_tmpB16->f1;}}_LL57E: return Cyc_Absyn_tvar_cmp(tvar,_tmpB17)== 0;_LL57F:;_LL580:
goto _LL57C;_LL57C:;}goto _LL577;_LL57A:;_LL57B: goto _LL577;_LL577:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpB18=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB1A;
struct Cyc_Absyn_Kind _tmpB1B;enum Cyc_Absyn_KindQual _tmpB1C;enum Cyc_Absyn_AliasQual
_tmpB1D;struct Cyc_Absyn_Kind*_tmpB1F;struct Cyc_Absyn_Kind _tmpB20;enum Cyc_Absyn_KindQual
_tmpB21;enum Cyc_Absyn_AliasQual _tmpB22;_LL582: {struct Cyc_Absyn_Less_kb_struct*
_tmpB19=(struct Cyc_Absyn_Less_kb_struct*)_tmpB18;if(_tmpB19->tag != 2)goto _LL584;
else{_tmpB1A=_tmpB19->f2;_tmpB1B=*_tmpB1A;_tmpB1C=_tmpB1B.kind;if(_tmpB1C != Cyc_Absyn_RgnKind)
goto _LL584;_tmpB1D=_tmpB1B.aliasqual;if(_tmpB1D != Cyc_Absyn_Top)goto _LL584;}}
_LL583: goto _LL585;_LL584: {struct Cyc_Absyn_Eq_kb_struct*_tmpB1E=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB18;if(_tmpB1E->tag != 0)goto _LL586;else{_tmpB1F=_tmpB1E->f1;_tmpB20=*_tmpB1F;
_tmpB21=_tmpB20.kind;if(_tmpB21 != Cyc_Absyn_RgnKind)goto _LL586;_tmpB22=_tmpB20.aliasqual;
if(_tmpB22 != Cyc_Absyn_Top)goto _LL586;}}_LL585: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL586:;_LL587: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL581:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpB23=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB26;struct Cyc_Core_Opt**
_tmpB27;struct Cyc_Core_Opt*_tmpB28;struct Cyc_Core_Opt**_tmpB29;struct Cyc_Absyn_Tvar*
_tmpB2B;struct Cyc_Absyn_TypeDecl*_tmpB2D;struct Cyc_Absyn_TypeDecl _tmpB2E;void*
_tmpB2F;void**_tmpB30;void***_tmpB31;struct Cyc_List_List*_tmpB33;struct _tuple2*
_tmpB35;struct Cyc_Absyn_Enumdecl*_tmpB36;struct Cyc_Absyn_Enumdecl**_tmpB37;
struct Cyc_Absyn_DatatypeInfo _tmpB39;union Cyc_Absyn_DatatypeInfoU _tmpB3A;union Cyc_Absyn_DatatypeInfoU*
_tmpB3B;struct Cyc_List_List*_tmpB3C;struct Cyc_List_List**_tmpB3D;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB3F;union Cyc_Absyn_DatatypeFieldInfoU _tmpB40;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB41;struct Cyc_List_List*_tmpB42;struct Cyc_Absyn_PtrInfo _tmpB44;void*_tmpB45;
struct Cyc_Absyn_Tqual _tmpB46;struct Cyc_Absyn_Tqual*_tmpB47;struct Cyc_Absyn_PtrAtts
_tmpB48;void*_tmpB49;union Cyc_Absyn_Constraint*_tmpB4A;union Cyc_Absyn_Constraint*
_tmpB4B;union Cyc_Absyn_Constraint*_tmpB4C;void*_tmpB4E;struct Cyc_Absyn_Exp*
_tmpB50;struct Cyc_Absyn_ArrayInfo _tmpB55;void*_tmpB56;struct Cyc_Absyn_Tqual
_tmpB57;struct Cyc_Absyn_Tqual*_tmpB58;struct Cyc_Absyn_Exp*_tmpB59;struct Cyc_Absyn_Exp**
_tmpB5A;union Cyc_Absyn_Constraint*_tmpB5B;struct Cyc_Position_Segment*_tmpB5C;
struct Cyc_Absyn_FnInfo _tmpB5E;struct Cyc_List_List*_tmpB5F;struct Cyc_List_List**
_tmpB60;struct Cyc_Core_Opt*_tmpB61;struct Cyc_Core_Opt**_tmpB62;struct Cyc_Absyn_Tqual
_tmpB63;struct Cyc_Absyn_Tqual*_tmpB64;void*_tmpB65;struct Cyc_List_List*_tmpB66;
int _tmpB67;struct Cyc_Absyn_VarargInfo*_tmpB68;struct Cyc_List_List*_tmpB69;struct
Cyc_List_List*_tmpB6A;struct Cyc_List_List*_tmpB6C;enum Cyc_Absyn_AggrKind _tmpB6E;
struct Cyc_List_List*_tmpB6F;struct Cyc_Absyn_AggrInfo _tmpB71;union Cyc_Absyn_AggrInfoU
_tmpB72;union Cyc_Absyn_AggrInfoU*_tmpB73;struct Cyc_List_List*_tmpB74;struct Cyc_List_List**
_tmpB75;struct _tuple2*_tmpB77;struct Cyc_List_List*_tmpB78;struct Cyc_List_List**
_tmpB79;struct Cyc_Absyn_Typedefdecl*_tmpB7A;struct Cyc_Absyn_Typedefdecl**_tmpB7B;
void**_tmpB7C;void***_tmpB7D;void*_tmpB82;void*_tmpB84;void*_tmpB85;void*_tmpB87;
void*_tmpB89;struct Cyc_List_List*_tmpB8B;_LL589: {struct Cyc_Absyn_VoidType_struct*
_tmpB24=(struct Cyc_Absyn_VoidType_struct*)_tmpB23;if(_tmpB24->tag != 0)goto _LL58B;}
_LL58A: goto _LL588;_LL58B: {struct Cyc_Absyn_Evar_struct*_tmpB25=(struct Cyc_Absyn_Evar_struct*)
_tmpB23;if(_tmpB25->tag != 1)goto _LL58D;else{_tmpB26=_tmpB25->f1;_tmpB27=(struct
Cyc_Core_Opt**)& _tmpB25->f1;_tmpB28=_tmpB25->f2;_tmpB29=(struct Cyc_Core_Opt**)&
_tmpB25->f2;}}_LL58C: if(*_tmpB27 == 0  || Cyc_Tcutil_kind_leq(expected_kind,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB27))->v) && !Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB27))->v,
expected_kind))*_tmpB27=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result
 && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){if(
expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB29=(struct Cyc_Core_Opt*)& urgn;
else{*_tmpB29=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmp145A;struct Cyc_Absyn_Less_kb_struct*_tmp1459;
struct Cyc_Absyn_Tvar*_tmpB8C=Cyc_Tcutil_new_tvar((void*)((_tmp1459=_cycalloc(
sizeof(*_tmp1459)),((_tmp1459[0]=((_tmp145A.tag=2,((_tmp145A.f1=0,((_tmp145A.f2=
expected_kind,_tmp145A)))))),_tmp1459)))));{struct Cyc_Absyn_VarType_struct*
_tmp1460;struct Cyc_Absyn_VarType_struct _tmp145F;struct Cyc_Core_Opt*_tmp145E;*
_tmpB29=((_tmp145E=_cycalloc(sizeof(*_tmp145E)),((_tmp145E->v=(void*)((_tmp1460=
_cycalloc(sizeof(*_tmp1460)),((_tmp1460[0]=((_tmp145F.tag=2,((_tmp145F.f1=
_tmpB8C,_tmp145F)))),_tmp1460)))),_tmp145E))));}_tmpB2B=_tmpB8C;goto _LL58E;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL588;_LL58D: {struct Cyc_Absyn_VarType_struct*_tmpB2A=(
struct Cyc_Absyn_VarType_struct*)_tmpB23;if(_tmpB2A->tag != 2)goto _LL58F;else{
_tmpB2B=_tmpB2A->f1;}}_LL58E:{void*_tmpB92=Cyc_Absyn_compress_kb(_tmpB2B->kind);
struct Cyc_Core_Opt*_tmpB94;struct Cyc_Core_Opt**_tmpB95;_LL5C2: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB93=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB92;if(_tmpB93->tag != 1)goto
_LL5C4;else{_tmpB94=_tmpB93->f1;_tmpB95=(struct Cyc_Core_Opt**)& _tmpB93->f1;}}
_LL5C3:{struct Cyc_Absyn_Less_kb_struct*_tmp1466;struct Cyc_Absyn_Less_kb_struct
_tmp1465;struct Cyc_Core_Opt*_tmp1464;*_tmpB95=((_tmp1464=_cycalloc(sizeof(*
_tmp1464)),((_tmp1464->v=(void*)((_tmp1466=_cycalloc(sizeof(*_tmp1466)),((
_tmp1466[0]=((_tmp1465.tag=2,((_tmp1465.f1=0,((_tmp1465.f2=expected_kind,
_tmp1465)))))),_tmp1466)))),_tmp1464))));}goto _LL5C1;_LL5C4:;_LL5C5: goto _LL5C1;
_LL5C1:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB2B);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB2B,put_in_effect);{void*_tmpB99=Cyc_Absyn_compress_kb(_tmpB2B->kind);struct
Cyc_Core_Opt*_tmpB9B;struct Cyc_Core_Opt**_tmpB9C;struct Cyc_Absyn_Kind*_tmpB9D;
_LL5C7: {struct Cyc_Absyn_Less_kb_struct*_tmpB9A=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB99;if(_tmpB9A->tag != 2)goto _LL5C9;else{_tmpB9B=_tmpB9A->f1;_tmpB9C=(struct
Cyc_Core_Opt**)& _tmpB9A->f1;_tmpB9D=_tmpB9A->f2;}}_LL5C8: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB9D)){struct Cyc_Absyn_Less_kb_struct*_tmp146C;struct Cyc_Absyn_Less_kb_struct
_tmp146B;struct Cyc_Core_Opt*_tmp146A;*_tmpB9C=((_tmp146A=_cycalloc(sizeof(*
_tmp146A)),((_tmp146A->v=(void*)((_tmp146C=_cycalloc(sizeof(*_tmp146C)),((
_tmp146C[0]=((_tmp146B.tag=2,((_tmp146B.f1=0,((_tmp146B.f2=expected_kind,
_tmp146B)))))),_tmp146C)))),_tmp146A))));}goto _LL5C6;_LL5C9:;_LL5CA: goto _LL5C6;
_LL5C6:;}goto _LL588;_LL58F: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB2C=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB23;if(_tmpB2C->tag != 27)goto _LL591;else{
_tmpB2D=_tmpB2C->f1;_tmpB2E=*_tmpB2D;_tmpB2F=_tmpB2E.r;_tmpB30=_tmpB2C->f2;
_tmpB31=(void***)& _tmpB2C->f2;}}_LL590: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpBA1=_tmpB2F;struct Cyc_Absyn_Aggrdecl*_tmpBA3;struct Cyc_Absyn_Enumdecl*
_tmpBA5;struct Cyc_Absyn_Datatypedecl*_tmpBA7;_LL5CC: {struct Cyc_Absyn_Aggr_td_struct*
_tmpBA2=(struct Cyc_Absyn_Aggr_td_struct*)_tmpBA1;if(_tmpBA2->tag != 0)goto _LL5CE;
else{_tmpBA3=_tmpBA2->f1;}}_LL5CD: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBA3);goto _LL5CB;_LL5CE: {struct Cyc_Absyn_Enum_td_struct*_tmpBA4=(struct Cyc_Absyn_Enum_td_struct*)
_tmpBA1;if(_tmpBA4->tag != 1)goto _LL5D0;else{_tmpBA5=_tmpBA4->f1;}}_LL5CF: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpBA5);goto _LL5CB;_LL5D0: {struct Cyc_Absyn_Datatype_td_struct*
_tmpBA6=(struct Cyc_Absyn_Datatype_td_struct*)_tmpBA1;if(_tmpBA6->tag != 2)goto
_LL5CB;else{_tmpBA7=_tmpBA6->f1;}}_LL5D1: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBA7);goto _LL5CB;_LL5CB:;}{void**_tmp146D;*_tmpB31=((_tmp146D=_cycalloc(
sizeof(*_tmp146D)),((_tmp146D[0]=new_t,_tmp146D))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL591: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB32=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB23;if(_tmpB32->tag != 15)goto
_LL593;else{_tmpB33=_tmpB32->f1;}}_LL592: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpBA9=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpB33 != 0;_tmpB33=_tmpB33->tl){
struct Cyc_Absyn_Enumfield*_tmpBAA=(struct Cyc_Absyn_Enumfield*)_tmpB33->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpBAA->name).f2)){
const char*_tmp1471;void*_tmp1470[1];struct Cyc_String_pa_struct _tmp146F;(_tmp146F.tag=
0,((_tmp146F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBAA->name).f2),((
_tmp1470[0]=& _tmp146F,Cyc_Tcutil_terr(_tmpBAA->loc,((_tmp1471="duplicate enum field name %s",
_tag_dyneither(_tmp1471,sizeof(char),29))),_tag_dyneither(_tmp1470,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1472;prev_fields=((_tmp1472=
_region_malloc(_tmpBA9,sizeof(*_tmp1472)),((_tmp1472->hd=(*_tmpBAA->name).f2,((
_tmp1472->tl=prev_fields,_tmp1472))))));}if(_tmpBAA->tag == 0)_tmpBAA->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpBAA->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpBAA->tag))){const char*_tmp1476;void*
_tmp1475[1];struct Cyc_String_pa_struct _tmp1474;(_tmp1474.tag=0,((_tmp1474.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBAA->name).f2),((_tmp1475[0]=&
_tmp1474,Cyc_Tcutil_terr(loc,((_tmp1476="enum field %s: expression is not constant",
_tag_dyneither(_tmp1476,sizeof(char),42))),_tag_dyneither(_tmp1475,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpBAA->tag))).f1;tag_count=t1 + 1;(*_tmpBAA->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp147C;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp147B;struct _tuple30*_tmp147A;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple30*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpBAA->name).f2,(struct _tuple30*)((_tmp147A=_cycalloc(sizeof(*_tmp147A)),((
_tmp147A->f1=(void*)((_tmp147C=_cycalloc(sizeof(*_tmp147C)),((_tmp147C[0]=((
_tmp147B.tag=4,((_tmp147B.f1=(void*)t,((_tmp147B.f2=_tmpBAA,_tmp147B)))))),
_tmp147C)))),((_tmp147A->f2=1,_tmp147A)))))));};};}}goto _LL588;}_LL593: {struct
Cyc_Absyn_EnumType_struct*_tmpB34=(struct Cyc_Absyn_EnumType_struct*)_tmpB23;if(
_tmpB34->tag != 14)goto _LL595;else{_tmpB35=_tmpB34->f1;_tmpB36=_tmpB34->f2;
_tmpB37=(struct Cyc_Absyn_Enumdecl**)& _tmpB34->f2;}}_LL594: if(*_tmpB37 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB37))->fields == 0){struct
_handler_cons _tmpBB5;_push_handler(& _tmpBB5);{int _tmpBB7=0;if(setjmp(_tmpBB5.handler))
_tmpBB7=1;if(!_tmpBB7){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB35);*_tmpB37=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpBB6=(void*)_exn_thrown;void*_tmpBB9=_tmpBB6;_LL5D3: {struct Cyc_Dict_Absent_struct*
_tmpBBA=(struct Cyc_Dict_Absent_struct*)_tmpBB9;if(_tmpBBA->tag != Cyc_Dict_Absent)
goto _LL5D5;}_LL5D4: {struct Cyc_Tcenv_Genv*_tmpBBB=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp147D;struct Cyc_Absyn_Enumdecl*_tmpBBC=(_tmp147D=
_cycalloc(sizeof(*_tmp147D)),((_tmp147D->sc=Cyc_Absyn_Extern,((_tmp147D->name=
_tmpB35,((_tmp147D->fields=0,_tmp147D)))))));Cyc_Tc_tcEnumdecl(te,_tmpBBB,loc,
_tmpBBC);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB35);*
_tmpB37=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5D2;};}_LL5D5:;_LL5D6:(void)_throw(
_tmpBB9);_LL5D2:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB37);*_tmpB35=(ed->name)[0];goto _LL588;};_LL595: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB38=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB23;if(_tmpB38->tag != 3)goto
_LL597;else{_tmpB39=_tmpB38->f1;_tmpB3A=_tmpB39.datatype_info;_tmpB3B=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB38->f1).datatype_info;_tmpB3C=_tmpB39.targs;_tmpB3D=(struct Cyc_List_List**)&(
_tmpB38->f1).targs;}}_LL596: {struct Cyc_List_List*_tmpBBE=*_tmpB3D;{union Cyc_Absyn_DatatypeInfoU
_tmpBBF=*_tmpB3B;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBC0;struct _tuple2*
_tmpBC1;int _tmpBC2;struct Cyc_Absyn_Datatypedecl**_tmpBC3;struct Cyc_Absyn_Datatypedecl*
_tmpBC4;_LL5D8: if((_tmpBBF.UnknownDatatype).tag != 1)goto _LL5DA;_tmpBC0=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBBF.UnknownDatatype).val;_tmpBC1=_tmpBC0.name;
_tmpBC2=_tmpBC0.is_extensible;_LL5D9: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBC5;_push_handler(& _tmpBC5);{int _tmpBC7=0;if(setjmp(
_tmpBC5.handler))_tmpBC7=1;if(!_tmpBC7){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBC1);;_pop_handler();}else{void*_tmpBC6=(void*)_exn_thrown;void*_tmpBC9=
_tmpBC6;_LL5DD: {struct Cyc_Dict_Absent_struct*_tmpBCA=(struct Cyc_Dict_Absent_struct*)
_tmpBC9;if(_tmpBCA->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {struct Cyc_Tcenv_Genv*
_tmpBCB=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp147E;struct Cyc_Absyn_Datatypedecl*
_tmpBCC=(_tmp147E=_cycalloc(sizeof(*_tmp147E)),((_tmp147E->sc=Cyc_Absyn_Extern,((
_tmp147E->name=_tmpBC1,((_tmp147E->tvs=0,((_tmp147E->fields=0,((_tmp147E->is_extensible=
_tmpBC2,_tmp147E)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBCB,loc,_tmpBCC);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBC1);if(_tmpBBE != 0){{const char*
_tmp1482;void*_tmp1481[1];struct Cyc_String_pa_struct _tmp1480;(_tmp1480.tag=0,((
_tmp1480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC1)),((_tmp1481[0]=& _tmp1480,Cyc_Tcutil_terr(loc,((_tmp1482="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1482,sizeof(char),47))),_tag_dyneither(_tmp1481,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5DC;}_LL5DF:;_LL5E0:(void)_throw(_tmpBC9);_LL5DC:;}};}
if(_tmpBC2  && !(*tudp)->is_extensible){const char*_tmp1486;void*_tmp1485[1];
struct Cyc_String_pa_struct _tmp1484;(_tmp1484.tag=0,((_tmp1484.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC1)),((
_tmp1485[0]=& _tmp1484,Cyc_Tcutil_terr(loc,((_tmp1486="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1486,sizeof(char),41))),_tag_dyneither(_tmp1485,sizeof(void*),
1)))))));}*_tmpB3B=Cyc_Absyn_KnownDatatype(tudp);_tmpBC4=*tudp;goto _LL5DB;}
_LL5DA: if((_tmpBBF.KnownDatatype).tag != 2)goto _LL5D7;_tmpBC3=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBBF.KnownDatatype).val;_tmpBC4=*_tmpBC3;_LL5DB: {struct Cyc_List_List*tvs=
_tmpBC4->tvs;for(0;_tmpBBE != 0  && tvs != 0;(_tmpBBE=_tmpBBE->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBBE->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1487;struct _tuple0 _tmpBD5=(_tmp1487.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1487.f2=t,_tmp1487)));void*_tmpBD6;void*_tmpBD8;struct Cyc_Absyn_Tvar*_tmpBDA;
_LL5E2: _tmpBD6=_tmpBD5.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBD7=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBD6;if(_tmpBD7->tag != 1)goto _LL5E4;};_tmpBD8=_tmpBD5.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBD9=(struct Cyc_Absyn_VarType_struct*)_tmpBD8;if(_tmpBD9->tag != 2)goto _LL5E4;
else{_tmpBDA=_tmpBD9->f1;}};_LL5E3: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBDA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBDA,1);continue;_LL5E4:;_LL5E5: goto _LL5E1;_LL5E1:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBBE != 0){const
char*_tmp148B;void*_tmp148A[1];struct Cyc_String_pa_struct _tmp1489;(_tmp1489.tag=
0,((_tmp1489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC4->name)),((_tmp148A[0]=& _tmp1489,Cyc_Tcutil_terr(loc,((_tmp148B="too many type arguments for datatype %s",
_tag_dyneither(_tmp148B,sizeof(char),40))),_tag_dyneither(_tmp148A,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp148C;hidden_ts=((_tmp148C=_cycalloc(sizeof(*_tmp148C)),((_tmp148C->hd=e,((
_tmp148C->tl=hidden_ts,_tmp148C))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB3D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB3D,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5D7;}_LL5D7:;}goto
_LL588;}_LL597: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB3E=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB23;if(_tmpB3E->tag != 4)goto _LL599;else{_tmpB3F=_tmpB3E->f1;_tmpB40=_tmpB3F.field_info;
_tmpB41=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB3E->f1).field_info;_tmpB42=
_tmpB3F.targs;}}_LL598:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBDF=*_tmpB41;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBE0;struct _tuple2*_tmpBE1;struct _tuple2*
_tmpBE2;int _tmpBE3;struct _tuple3 _tmpBE4;struct Cyc_Absyn_Datatypedecl*_tmpBE5;
struct Cyc_Absyn_Datatypefield*_tmpBE6;_LL5E7: if((_tmpBDF.UnknownDatatypefield).tag
!= 1)goto _LL5E9;_tmpBE0=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBDF.UnknownDatatypefield).val;
_tmpBE1=_tmpBE0.datatype_name;_tmpBE2=_tmpBE0.field_name;_tmpBE3=_tmpBE0.is_extensible;
_LL5E8: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBE7;_push_handler(& _tmpBE7);{int _tmpBE9=0;if(setjmp(
_tmpBE7.handler))_tmpBE9=1;if(!_tmpBE9){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBE1);;_pop_handler();}else{void*_tmpBE8=(void*)_exn_thrown;void*_tmpBEB=
_tmpBE8;_LL5EC: {struct Cyc_Dict_Absent_struct*_tmpBEC=(struct Cyc_Dict_Absent_struct*)
_tmpBEB;if(_tmpBEC->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:{const char*_tmp1490;
void*_tmp148F[1];struct Cyc_String_pa_struct _tmp148E;(_tmp148E.tag=0,((_tmp148E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE1)),((
_tmp148F[0]=& _tmp148E,Cyc_Tcutil_terr(loc,((_tmp1490="unbound datatype %s",
_tag_dyneither(_tmp1490,sizeof(char),20))),_tag_dyneither(_tmp148F,sizeof(void*),
1)))))));}return cvtenv;_LL5EE:;_LL5EF:(void)_throw(_tmpBEB);_LL5EB:;}};}{struct
_handler_cons _tmpBF0;_push_handler(& _tmpBF0);{int _tmpBF2=0;if(setjmp(_tmpBF0.handler))
_tmpBF2=1;if(!_tmpBF2){{struct _RegionHandle*_tmpBF3=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBF4=Cyc_Tcenv_lookup_ordinary(_tmpBF3,te,loc,_tmpBE2);struct Cyc_Absyn_Datatypedecl*
_tmpBF6;struct Cyc_Absyn_Datatypefield*_tmpBF7;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBF5=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBF4;if(_tmpBF5->tag != 2)goto
_LL5F3;else{_tmpBF6=_tmpBF5->f1;_tmpBF7=_tmpBF5->f2;}}_LL5F2: tuf=_tmpBF7;tud=
_tmpBF6;if(_tmpBE3  && !tud->is_extensible){const char*_tmp1494;void*_tmp1493[1];
struct Cyc_String_pa_struct _tmp1492;(_tmp1492.tag=0,((_tmp1492.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE1)),((
_tmp1493[0]=& _tmp1492,Cyc_Tcutil_terr(loc,((_tmp1494="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1494,sizeof(char),41))),_tag_dyneither(_tmp1493,sizeof(void*),
1)))))));}goto _LL5F0;_LL5F3:;_LL5F4:{const char*_tmp1499;void*_tmp1498[2];struct
Cyc_String_pa_struct _tmp1497;struct Cyc_String_pa_struct _tmp1496;(_tmp1496.tag=0,((
_tmp1496.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE1)),((_tmp1497.tag=0,((_tmp1497.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE2)),((_tmp1498[0]=& _tmp1497,((
_tmp1498[1]=& _tmp1496,Cyc_Tcutil_terr(loc,((_tmp1499="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1499,sizeof(char),37))),_tag_dyneither(_tmp1498,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBFF=cvtenv;_npop_handler(0);return
_tmpBFF;};_LL5F0:;};_pop_handler();}else{void*_tmpBF1=(void*)_exn_thrown;void*
_tmpC01=_tmpBF1;_LL5F6: {struct Cyc_Dict_Absent_struct*_tmpC02=(struct Cyc_Dict_Absent_struct*)
_tmpC01;if(_tmpC02->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:{const char*_tmp149E;
void*_tmp149D[2];struct Cyc_String_pa_struct _tmp149C;struct Cyc_String_pa_struct
_tmp149B;(_tmp149B.tag=0,((_tmp149B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE1)),((_tmp149C.tag=0,((_tmp149C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE2)),((
_tmp149D[0]=& _tmp149C,((_tmp149D[1]=& _tmp149B,Cyc_Tcutil_terr(loc,((_tmp149E="unbound field %s in type datatype %s",
_tag_dyneither(_tmp149E,sizeof(char),37))),_tag_dyneither(_tmp149D,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5F8:;_LL5F9:(void)_throw(_tmpC01);_LL5F5:;}};}*
_tmpB41=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBE5=tud;_tmpBE6=tuf;goto _LL5EA;}
_LL5E9: if((_tmpBDF.KnownDatatypefield).tag != 2)goto _LL5E6;_tmpBE4=(struct _tuple3)(
_tmpBDF.KnownDatatypefield).val;_tmpBE5=_tmpBE4.f1;_tmpBE6=_tmpBE4.f2;_LL5EA: {
struct Cyc_List_List*tvs=_tmpBE5->tvs;for(0;_tmpB42 != 0  && tvs != 0;(_tmpB42=
_tmpB42->tl,tvs=tvs->tl)){void*t=(void*)_tmpB42->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp149F;struct _tuple0 _tmpC08=(
_tmp149F.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp149F.f2=t,_tmp149F)));void*
_tmpC09;void*_tmpC0B;struct Cyc_Absyn_Tvar*_tmpC0D;_LL5FB: _tmpC09=_tmpC08.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpC0A=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC09;if(_tmpC0A->tag != 1)goto _LL5FD;};_tmpC0B=_tmpC08.f2;{struct Cyc_Absyn_VarType_struct*
_tmpC0C=(struct Cyc_Absyn_VarType_struct*)_tmpC0B;if(_tmpC0C->tag != 2)goto _LL5FD;
else{_tmpC0D=_tmpC0C->f1;}};_LL5FC: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpC0D);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpC0D,1);continue;_LL5FD:;_LL5FE: goto _LL5FA;_LL5FA:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB42 != 0){const
char*_tmp14A4;void*_tmp14A3[2];struct Cyc_String_pa_struct _tmp14A2;struct Cyc_String_pa_struct
_tmp14A1;(_tmp14A1.tag=0,((_tmp14A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE6->name)),((_tmp14A2.tag=0,((
_tmp14A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE5->name)),((_tmp14A3[0]=& _tmp14A2,((_tmp14A3[1]=& _tmp14A1,Cyc_Tcutil_terr(
loc,((_tmp14A4="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp14A4,sizeof(char),43))),_tag_dyneither(_tmp14A3,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp14A9;void*_tmp14A8[2];struct Cyc_String_pa_struct
_tmp14A7;struct Cyc_String_pa_struct _tmp14A6;(_tmp14A6.tag=0,((_tmp14A6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE6->name)),((
_tmp14A7.tag=0,((_tmp14A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE5->name)),((_tmp14A8[0]=& _tmp14A7,((_tmp14A8[1]=& _tmp14A6,Cyc_Tcutil_terr(
loc,((_tmp14A9="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp14A9,sizeof(char),42))),_tag_dyneither(_tmp14A8,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}goto _LL588;_LL599: {struct Cyc_Absyn_PointerType_struct*
_tmpB43=(struct Cyc_Absyn_PointerType_struct*)_tmpB23;if(_tmpB43->tag != 5)goto
_LL59B;else{_tmpB44=_tmpB43->f1;_tmpB45=_tmpB44.elt_typ;_tmpB46=_tmpB44.elt_tq;
_tmpB47=(struct Cyc_Absyn_Tqual*)&(_tmpB43->f1).elt_tq;_tmpB48=_tmpB44.ptr_atts;
_tmpB49=_tmpB48.rgn;_tmpB4A=_tmpB48.nullable;_tmpB4B=_tmpB48.bounds;_tmpB4C=
_tmpB48.zero_term;}}_LL59A: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB45,1);_tmpB47->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB47->print_const,_tmpB45);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5FF: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL600: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB49,1);{union Cyc_Absyn_Constraint*
_tmpC16=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB4C);union Cyc_Absyn_Constraint _tmpC17;int _tmpC18;union Cyc_Absyn_Constraint
_tmpC19;int _tmpC1A;_LL604: _tmpC17=*_tmpC16;if((_tmpC17.No_constr).tag != 3)goto
_LL606;_tmpC18=(int)(_tmpC17.No_constr).val;_LL605:{void*_tmpC1B=Cyc_Tcutil_compress(
_tmpB45);enum Cyc_Absyn_Size_of _tmpC1D;_LL60B: {struct Cyc_Absyn_IntType_struct*
_tmpC1C=(struct Cyc_Absyn_IntType_struct*)_tmpC1B;if(_tmpC1C->tag != 6)goto _LL60D;
else{_tmpC1D=_tmpC1C->f2;if(_tmpC1D != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB4C,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL60A;_LL60D:;_LL60E:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB4C,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL60A;
_LL60A:;}goto _LL603;_LL606: _tmpC19=*_tmpC16;if((_tmpC19.Eq_constr).tag != 1)goto
_LL608;_tmpC1A=(int)(_tmpC19.Eq_constr).val;if(_tmpC1A != 1)goto _LL608;_LL607: if(
!Cyc_Tcutil_admits_zero(_tmpB45)){const char*_tmp14AD;void*_tmp14AC[1];struct Cyc_String_pa_struct
_tmp14AB;(_tmp14AB.tag=0,((_tmp14AB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB45)),((_tmp14AC[0]=& _tmp14AB,Cyc_Tcutil_terr(
loc,((_tmp14AD="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp14AD,sizeof(char),53))),_tag_dyneither(_tmp14AC,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL603;_LL608:;_LL609: is_zero_terminated=0;
goto _LL603;_LL603:;}{void*_tmpC21=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB4B);struct Cyc_Absyn_Exp*
_tmpC24;_LL610: {struct Cyc_Absyn_DynEither_b_struct*_tmpC22=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC21;if(_tmpC22->tag != 0)goto _LL612;}_LL611: goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_struct*
_tmpC23=(struct Cyc_Absyn_Upper_b_struct*)_tmpC21;if(_tmpC23->tag != 1)goto _LL60F;
else{_tmpC24=_tmpC23->f1;}}_LL613: {struct _RegionHandle*_tmpC25=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC26=Cyc_Tcenv_allow_valueof(_tmpC25,te);Cyc_Tcexp_tcExp(
_tmpC26,0,_tmpC24);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC24,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC24)){const char*_tmp14B0;void*
_tmp14AF;(_tmp14AF=0,Cyc_Tcutil_terr(loc,((_tmp14B0="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp14B0,sizeof(char),49))),_tag_dyneither(_tmp14AF,sizeof(void*),
0)));}{unsigned int _tmpC2A;int _tmpC2B;struct _tuple13 _tmpC29=Cyc_Evexp_eval_const_uint_exp(
_tmpC24);_tmpC2A=_tmpC29.f1;_tmpC2B=_tmpC29.f2;if(is_zero_terminated  && (!
_tmpC2B  || _tmpC2A < 1)){const char*_tmp14B3;void*_tmp14B2;(_tmp14B2=0,Cyc_Tcutil_terr(
loc,((_tmp14B3="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp14B3,sizeof(char),55))),_tag_dyneither(_tmp14B2,sizeof(void*),
0)));}goto _LL60F;};}_LL60F:;}goto _LL588;};}_LL59B: {struct Cyc_Absyn_TagType_struct*
_tmpB4D=(struct Cyc_Absyn_TagType_struct*)_tmpB23;if(_tmpB4D->tag != 20)goto _LL59D;
else{_tmpB4E=(void*)_tmpB4D->f1;}}_LL59C: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB4E,1);goto _LL588;_LL59D: {struct Cyc_Absyn_ValueofType_struct*
_tmpB4F=(struct Cyc_Absyn_ValueofType_struct*)_tmpB23;if(_tmpB4F->tag != 19)goto
_LL59F;else{_tmpB50=_tmpB4F->f1;}}_LL59E: {struct _RegionHandle*_tmpC2E=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC2F=Cyc_Tcenv_allow_valueof(_tmpC2E,te);Cyc_Tcexp_tcExp(
_tmpC2F,0,_tmpB50);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB50)){const char*
_tmp14B6;void*_tmp14B5;(_tmp14B5=0,Cyc_Tcutil_terr(loc,((_tmp14B6="valueof_t requires an int expression",
_tag_dyneither(_tmp14B6,sizeof(char),37))),_tag_dyneither(_tmp14B5,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB50,te,cvtenv);goto
_LL588;}_LL59F: {struct Cyc_Absyn_IntType_struct*_tmpB51=(struct Cyc_Absyn_IntType_struct*)
_tmpB23;if(_tmpB51->tag != 6)goto _LL5A1;}_LL5A0: goto _LL5A2;_LL5A1: {struct Cyc_Absyn_FloatType_struct*
_tmpB52=(struct Cyc_Absyn_FloatType_struct*)_tmpB23;if(_tmpB52->tag != 7)goto
_LL5A3;}_LL5A2: goto _LL5A4;_LL5A3: {struct Cyc_Absyn_DoubleType_struct*_tmpB53=(
struct Cyc_Absyn_DoubleType_struct*)_tmpB23;if(_tmpB53->tag != 8)goto _LL5A5;}
_LL5A4: goto _LL588;_LL5A5: {struct Cyc_Absyn_ArrayType_struct*_tmpB54=(struct Cyc_Absyn_ArrayType_struct*)
_tmpB23;if(_tmpB54->tag != 9)goto _LL5A7;else{_tmpB55=_tmpB54->f1;_tmpB56=_tmpB55.elt_type;
_tmpB57=_tmpB55.tq;_tmpB58=(struct Cyc_Absyn_Tqual*)&(_tmpB54->f1).tq;_tmpB59=
_tmpB55.num_elts;_tmpB5A=(struct Cyc_Absyn_Exp**)&(_tmpB54->f1).num_elts;_tmpB5B=
_tmpB55.zero_term;_tmpB5C=_tmpB55.zt_loc;}}_LL5A6: {struct Cyc_Absyn_Exp*_tmpC32=*
_tmpB5A;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpB56,1);_tmpB58->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB58->print_const,
_tmpB56);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC33=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB5B);union Cyc_Absyn_Constraint
_tmpC34;int _tmpC35;union Cyc_Absyn_Constraint _tmpC36;int _tmpC37;_LL615: _tmpC34=*
_tmpC33;if((_tmpC34.No_constr).tag != 3)goto _LL617;_tmpC35=(int)(_tmpC34.No_constr).val;
_LL616:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB5B,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL614;_LL617: _tmpC36=*_tmpC33;if((_tmpC36.Eq_constr).tag
!= 1)goto _LL619;_tmpC37=(int)(_tmpC36.Eq_constr).val;if(_tmpC37 != 1)goto _LL619;
_LL618: if(!Cyc_Tcutil_admits_zero(_tmpB56)){const char*_tmp14BA;void*_tmp14B9[1];
struct Cyc_String_pa_struct _tmp14B8;(_tmp14B8.tag=0,((_tmp14B8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB56)),((
_tmp14B9[0]=& _tmp14B8,Cyc_Tcutil_terr(loc,((_tmp14BA="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp14BA,sizeof(char),51))),_tag_dyneither(_tmp14B9,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL614;_LL619:;_LL61A: is_zero_terminated=0;
goto _LL614;_LL614:;}if(_tmpC32 == 0){if(is_zero_terminated)*_tmpB5A=(_tmpC32=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14BD;void*
_tmp14BC;(_tmp14BC=0,Cyc_Tcutil_warn(loc,((_tmp14BD="array bound defaults to 1 here",
_tag_dyneither(_tmp14BD,sizeof(char),31))),_tag_dyneither(_tmp14BC,sizeof(void*),
0)));}*_tmpB5A=(_tmpC32=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC32);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC32)){const char*_tmp14C0;void*_tmp14BF;(_tmp14BF=0,Cyc_Tcutil_terr(loc,((
_tmp14C0="array bounds expression is not constant",_tag_dyneither(_tmp14C0,
sizeof(char),40))),_tag_dyneither(_tmp14BF,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC32)){const char*_tmp14C3;void*_tmp14C2;(_tmp14C2=0,
Cyc_Tcutil_terr(loc,((_tmp14C3="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14C3,sizeof(char),47))),_tag_dyneither(_tmp14C2,sizeof(void*),
0)));}{unsigned int _tmpC42;int _tmpC43;struct _tuple13 _tmpC41=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC32);_tmpC42=_tmpC41.f1;_tmpC43=_tmpC41.f2;if((
is_zero_terminated  && _tmpC43) && _tmpC42 < 1){const char*_tmp14C6;void*_tmp14C5;(
_tmp14C5=0,Cyc_Tcutil_warn(loc,((_tmp14C6="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14C6,sizeof(char),48))),_tag_dyneither(_tmp14C5,sizeof(void*),
0)));}if((_tmpC43  && _tmpC42 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14C9;void*
_tmp14C8;(_tmp14C8=0,Cyc_Tcutil_warn(loc,((_tmp14C9="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14C9,sizeof(char),75))),_tag_dyneither(_tmp14C8,sizeof(void*),
0)));}*_tmpB5A=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL588;};};}
_LL5A7: {struct Cyc_Absyn_FnType_struct*_tmpB5D=(struct Cyc_Absyn_FnType_struct*)
_tmpB23;if(_tmpB5D->tag != 10)goto _LL5A9;else{_tmpB5E=_tmpB5D->f1;_tmpB5F=_tmpB5E.tvars;
_tmpB60=(struct Cyc_List_List**)&(_tmpB5D->f1).tvars;_tmpB61=_tmpB5E.effect;
_tmpB62=(struct Cyc_Core_Opt**)&(_tmpB5D->f1).effect;_tmpB63=_tmpB5E.ret_tqual;
_tmpB64=(struct Cyc_Absyn_Tqual*)&(_tmpB5D->f1).ret_tqual;_tmpB65=_tmpB5E.ret_typ;
_tmpB66=_tmpB5E.args;_tmpB67=_tmpB5E.c_varargs;_tmpB68=_tmpB5E.cyc_varargs;
_tmpB69=_tmpB5E.rgn_po;_tmpB6A=_tmpB5E.attributes;}}_LL5A8: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB6A != 0;
_tmpB6A=_tmpB6A->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB6A->hd)){const char*
_tmp14CD;void*_tmp14CC[1];struct Cyc_String_pa_struct _tmp14CB;(_tmp14CB.tag=0,((
_tmp14CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB6A->hd)),((_tmp14CC[0]=& _tmp14CB,Cyc_Tcutil_terr(loc,((_tmp14CD="bad function type attribute %s",
_tag_dyneither(_tmp14CD,sizeof(char),31))),_tag_dyneither(_tmp14CC,sizeof(void*),
1)))))));}{void*_tmpC4B=(void*)_tmpB6A->hd;enum Cyc_Absyn_Format_Type _tmpC50;int
_tmpC51;int _tmpC52;_LL61C: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC4C=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC4B;if(_tmpC4C->tag != 1)goto _LL61E;}_LL61D: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC4D=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC4B;if(_tmpC4D->tag != 2)goto
_LL620;}_LL61F: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC4E=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC4B;if(_tmpC4E->tag != 3)goto _LL622;}_LL621: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_struct*_tmpC4F=(
struct Cyc_Absyn_Format_att_struct*)_tmpC4B;if(_tmpC4F->tag != 19)goto _LL624;else{
_tmpC50=_tmpC4F->f1;_tmpC51=_tmpC4F->f2;_tmpC52=_tmpC4F->f3;}}_LL623: if(!
seen_format){seen_format=1;ft=_tmpC50;fmt_desc_arg=_tmpC51;fmt_arg_start=_tmpC52;}
else{const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(loc,((_tmp14D0="function can't have multiple format attributes",
_tag_dyneither(_tmp14D0,sizeof(char),47))),_tag_dyneither(_tmp14CF,sizeof(void*),
0)));}goto _LL61B;_LL624:;_LL625: goto _LL61B;_LL61B:;};}if(num_convs > 1){const char*
_tmp14D3;void*_tmp14D2;(_tmp14D2=0,Cyc_Tcutil_terr(loc,((_tmp14D3="function can't have multiple calling conventions",
_tag_dyneither(_tmp14D3,sizeof(char),49))),_tag_dyneither(_tmp14D2,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB60);{struct Cyc_List_List*b=*
_tmpB60;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC57=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC59;struct Cyc_Absyn_Kind _tmpC5A;enum Cyc_Absyn_KindQual
_tmpC5B;_LL627: {struct Cyc_Absyn_Eq_kb_struct*_tmpC58=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC57;if(_tmpC58->tag != 0)goto _LL629;else{_tmpC59=_tmpC58->f1;_tmpC5A=*_tmpC59;
_tmpC5B=_tmpC5A.kind;if(_tmpC5B != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:{const
char*_tmp14D7;void*_tmp14D6[1];struct Cyc_String_pa_struct _tmp14D5;(_tmp14D5.tag=
0,((_tmp14D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14D6[0]=& _tmp14D5,Cyc_Tcutil_terr(loc,((_tmp14D7="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14D7,sizeof(char),51))),_tag_dyneither(_tmp14D6,sizeof(void*),
1)))))));}goto _LL626;_LL629:;_LL62A: goto _LL626;_LL626:;};}}{struct _RegionHandle*
_tmpC5F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14D8;struct Cyc_Tcutil_CVTEnv
_tmpC60=(_tmp14D8.r=_tmpC5F,((_tmp14D8.kind_env=cvtenv.kind_env,((_tmp14D8.free_vars=
0,((_tmp14D8.free_evars=0,((_tmp14D8.generalize_evars=cvtenv.generalize_evars,((
_tmp14D8.fn_result=1,_tmp14D8)))))))))));_tmpC60=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC60,& Cyc_Tcutil_tmk,_tmpB65,1);_tmpB64->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB64->print_const,_tmpB65);_tmpC60.fn_result=0;{struct Cyc_List_List*a=
_tmpB66;for(0;a != 0;a=a->tl){struct _tuple9*_tmpC61=(struct _tuple9*)a->hd;void*
_tmpC62=(*_tmpC61).f3;_tmpC60=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC60,& Cyc_Tcutil_tmk,
_tmpC62,1);{int _tmpC63=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC61).f2).print_const,
_tmpC62);((*_tmpC61).f2).real_const=_tmpC63;{void*_tmpC64=Cyc_Tcutil_compress(
_tmpC62);struct Cyc_Absyn_ArrayInfo _tmpC66;void*_tmpC67;struct Cyc_Absyn_Tqual
_tmpC68;struct Cyc_Absyn_Exp*_tmpC69;union Cyc_Absyn_Constraint*_tmpC6A;struct Cyc_Position_Segment*
_tmpC6B;_LL62C: {struct Cyc_Absyn_ArrayType_struct*_tmpC65=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC64;if(_tmpC65->tag != 9)goto _LL62E;else{_tmpC66=_tmpC65->f1;_tmpC67=_tmpC66.elt_type;
_tmpC68=_tmpC66.tq;_tmpC69=_tmpC66.num_elts;_tmpC6A=_tmpC66.zero_term;_tmpC6B=
_tmpC66.zt_loc;}}_LL62D: {void*_tmpC6C=Cyc_Absyn_new_evar(0,0);_tmpC60=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC60,& Cyc_Tcutil_rk,_tmpC6C,1);{struct Cyc_Absyn_Upper_b_struct _tmp14DB;
struct Cyc_Absyn_Upper_b_struct*_tmp14DA;void*_tmpC6D=Cyc_Absyn_atb_typ(_tmpC67,
_tmpC6C,_tmpC68,(void*)((_tmp14DA=_cycalloc(sizeof(*_tmp14DA)),((_tmp14DA[0]=((
_tmp14DB.tag=1,((_tmp14DB.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC69),_tmp14DB)))),
_tmp14DA)))),_tmpC6A);(*_tmpC61).f3=_tmpC6D;goto _LL62B;};}_LL62E:;_LL62F: goto
_LL62B;_LL62B:;};};}}if(_tmpB68 != 0){if(_tmpB67){const char*_tmp14DE;void*
_tmp14DD;(_tmp14DD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14DE="both c_vararg and cyc_vararg",_tag_dyneither(
_tmp14DE,sizeof(char),29))),_tag_dyneither(_tmp14DD,sizeof(void*),0)));}{void*
_tmpC73;int _tmpC74;struct Cyc_Absyn_VarargInfo _tmpC72=*_tmpB68;_tmpC73=_tmpC72.type;
_tmpC74=_tmpC72.inject;_tmpC60=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC60,& Cyc_Tcutil_tmk,
_tmpC73,1);(_tmpB68->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(
_tmpB68->tq).print_const,_tmpC73);if(_tmpC74){void*_tmpC75=Cyc_Tcutil_compress(
_tmpC73);struct Cyc_Absyn_PtrInfo _tmpC77;void*_tmpC78;struct Cyc_Absyn_PtrAtts
_tmpC79;union Cyc_Absyn_Constraint*_tmpC7A;union Cyc_Absyn_Constraint*_tmpC7B;
_LL631: {struct Cyc_Absyn_PointerType_struct*_tmpC76=(struct Cyc_Absyn_PointerType_struct*)
_tmpC75;if(_tmpC76->tag != 5)goto _LL633;else{_tmpC77=_tmpC76->f1;_tmpC78=_tmpC77.elt_typ;
_tmpC79=_tmpC77.ptr_atts;_tmpC7A=_tmpC79.bounds;_tmpC7B=_tmpC79.zero_term;}}
_LL632:{void*_tmpC7C=Cyc_Tcutil_compress(_tmpC78);_LL636: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC7D=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC7C;if(_tmpC7D->tag != 3)goto
_LL638;}_LL637: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC7B)){const char*_tmp14E1;void*_tmp14E0;(_tmp14E0=0,Cyc_Tcutil_terr(loc,((
_tmp14E1="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14E1,sizeof(
char),37))),_tag_dyneither(_tmp14E0,sizeof(void*),0)));}{void*_tmpC80=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC7A);_LL63B: {struct Cyc_Absyn_DynEither_b_struct*_tmpC81=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC80;if(_tmpC81->tag != 0)goto _LL63D;}_LL63C:{const char*_tmp14E4;void*_tmp14E3;(
_tmp14E3=0,Cyc_Tcutil_terr(loc,((_tmp14E4="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14E4,sizeof(char),44))),_tag_dyneither(_tmp14E3,sizeof(void*),
0)));}goto _LL63A;_LL63D:;_LL63E: goto _LL63A;_LL63A:;}goto _LL635;_LL638:;_LL639:{
const char*_tmp14E7;void*_tmp14E6;(_tmp14E6=0,Cyc_Tcutil_terr(loc,((_tmp14E7="can't inject a non-datatype type",
_tag_dyneither(_tmp14E7,sizeof(char),33))),_tag_dyneither(_tmp14E6,sizeof(void*),
0)));}goto _LL635;_LL635:;}goto _LL630;_LL633:;_LL634:{const char*_tmp14EA;void*
_tmp14E9;(_tmp14E9=0,Cyc_Tcutil_terr(loc,((_tmp14EA="expecting a datatype pointer type",
_tag_dyneither(_tmp14EA,sizeof(char),34))),_tag_dyneither(_tmp14E9,sizeof(void*),
0)));}goto _LL630;_LL630:;}};}if(seen_format){int _tmpC88=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB66);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC88) || 
fmt_arg_start < 0) || _tmpB68 == 0  && fmt_arg_start != 0) || _tmpB68 != 0  && 
fmt_arg_start != _tmpC88 + 1){const char*_tmp14ED;void*_tmp14EC;(_tmp14EC=0,Cyc_Tcutil_terr(
loc,((_tmp14ED="bad format descriptor",_tag_dyneither(_tmp14ED,sizeof(char),22))),
_tag_dyneither(_tmp14EC,sizeof(void*),0)));}else{void*_tmpC8C;struct _tuple9
_tmpC8B=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB66,
fmt_desc_arg - 1);_tmpC8C=_tmpC8B.f3;{void*_tmpC8D=Cyc_Tcutil_compress(_tmpC8C);
struct Cyc_Absyn_PtrInfo _tmpC8F;void*_tmpC90;struct Cyc_Absyn_PtrAtts _tmpC91;union
Cyc_Absyn_Constraint*_tmpC92;union Cyc_Absyn_Constraint*_tmpC93;_LL640: {struct
Cyc_Absyn_PointerType_struct*_tmpC8E=(struct Cyc_Absyn_PointerType_struct*)
_tmpC8D;if(_tmpC8E->tag != 5)goto _LL642;else{_tmpC8F=_tmpC8E->f1;_tmpC90=_tmpC8F.elt_typ;
_tmpC91=_tmpC8F.ptr_atts;_tmpC92=_tmpC91.bounds;_tmpC93=_tmpC91.zero_term;}}
_LL641:{struct _tuple0 _tmp14EE;struct _tuple0 _tmpC95=(_tmp14EE.f1=Cyc_Tcutil_compress(
_tmpC90),((_tmp14EE.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC92),_tmp14EE)));void*_tmpC96;enum Cyc_Absyn_Sign
_tmpC98;enum Cyc_Absyn_Size_of _tmpC99;void*_tmpC9A;_LL645: _tmpC96=_tmpC95.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC97=(struct Cyc_Absyn_IntType_struct*)_tmpC96;
if(_tmpC97->tag != 6)goto _LL647;else{_tmpC98=_tmpC97->f1;if(_tmpC98 != Cyc_Absyn_None)
goto _LL647;_tmpC99=_tmpC97->f2;if(_tmpC99 != Cyc_Absyn_Char_sz)goto _LL647;}};
_tmpC9A=_tmpC95.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC9B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC9A;if(_tmpC9B->tag != 0)goto _LL647;};_LL646: goto _LL644;_LL647:;_LL648:{const
char*_tmp14F1;void*_tmp14F0;(_tmp14F0=0,Cyc_Tcutil_terr(loc,((_tmp14F1="format descriptor is not a char ? type",
_tag_dyneither(_tmp14F1,sizeof(char),39))),_tag_dyneither(_tmp14F0,sizeof(void*),
0)));}goto _LL644;_LL644:;}goto _LL63F;_LL642:;_LL643:{const char*_tmp14F4;void*
_tmp14F3;(_tmp14F3=0,Cyc_Tcutil_terr(loc,((_tmp14F4="format descriptor is not a char ? type",
_tag_dyneither(_tmp14F4,sizeof(char),39))),_tag_dyneither(_tmp14F3,sizeof(void*),
0)));}goto _LL63F;_LL63F:;}if(fmt_arg_start != 0){int problem;{struct _tuple31
_tmp14F5;struct _tuple31 _tmpCA1=(_tmp14F5.f1=ft,((_tmp14F5.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB68))->type)),
_tmp14F5)));enum Cyc_Absyn_Format_Type _tmpCA2;void*_tmpCA3;struct Cyc_Absyn_DatatypeInfo
_tmpCA5;union Cyc_Absyn_DatatypeInfoU _tmpCA6;struct Cyc_Absyn_Datatypedecl**
_tmpCA7;struct Cyc_Absyn_Datatypedecl*_tmpCA8;enum Cyc_Absyn_Format_Type _tmpCA9;
void*_tmpCAA;struct Cyc_Absyn_DatatypeInfo _tmpCAC;union Cyc_Absyn_DatatypeInfoU
_tmpCAD;struct Cyc_Absyn_Datatypedecl**_tmpCAE;struct Cyc_Absyn_Datatypedecl*
_tmpCAF;_LL64A: _tmpCA2=_tmpCA1.f1;if(_tmpCA2 != Cyc_Absyn_Printf_ft)goto _LL64C;
_tmpCA3=_tmpCA1.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpCA4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCA3;if(_tmpCA4->tag != 3)goto _LL64C;else{_tmpCA5=_tmpCA4->f1;_tmpCA6=_tmpCA5.datatype_info;
if((_tmpCA6.KnownDatatype).tag != 2)goto _LL64C;_tmpCA7=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCA6.KnownDatatype).val;_tmpCA8=*_tmpCA7;}};_LL64B: problem=Cyc_Absyn_qvar_cmp(
_tmpCA8->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL649;_LL64C: _tmpCA9=
_tmpCA1.f1;if(_tmpCA9 != Cyc_Absyn_Scanf_ft)goto _LL64E;_tmpCAA=_tmpCA1.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpCAB=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCAA;if(_tmpCAB->tag != 3)goto _LL64E;else{_tmpCAC=_tmpCAB->f1;_tmpCAD=_tmpCAC.datatype_info;
if((_tmpCAD.KnownDatatype).tag != 2)goto _LL64E;_tmpCAE=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCAD.KnownDatatype).val;_tmpCAF=*_tmpCAE;}};_LL64D: problem=Cyc_Absyn_qvar_cmp(
_tmpCAF->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL649;_LL64E:;_LL64F:
problem=1;goto _LL649;_LL649:;}if(problem){const char*_tmp14F8;void*_tmp14F7;(
_tmp14F7=0,Cyc_Tcutil_terr(loc,((_tmp14F8="format attribute and vararg types don't match",
_tag_dyneither(_tmp14F8,sizeof(char),46))),_tag_dyneither(_tmp14F7,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB69;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpCB3;void*_tmpCB4;void*_tmpCB5;struct _tuple0*_tmpCB2=(struct _tuple0*)
rpo->hd;_tmpCB3=*_tmpCB2;_tmpCB4=_tmpCB3.f1;_tmpCB5=_tmpCB3.f2;_tmpC60=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC60,& Cyc_Tcutil_ek,_tmpCB4,1);_tmpC60=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC60,& Cyc_Tcutil_trk,_tmpCB5,1);}}if(*_tmpB62 != 0)_tmpC60=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC60,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB62))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC60.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpCB7;int _tmpCB8;struct
_tuple29 _tmpCB6=*((struct _tuple29*)tvs->hd);_tmpCB7=_tmpCB6.f1;_tmpCB8=_tmpCB6.f2;
if(!_tmpCB8)continue;{void*_tmpCB9=Cyc_Absyn_compress_kb(_tmpCB7->kind);struct
Cyc_Core_Opt*_tmpCBB;struct Cyc_Core_Opt**_tmpCBC;struct Cyc_Absyn_Kind*_tmpCBD;
struct Cyc_Absyn_Kind*_tmpCBF;struct Cyc_Absyn_Kind*_tmpCC1;struct Cyc_Absyn_Kind
_tmpCC2;enum Cyc_Absyn_KindQual _tmpCC3;struct Cyc_Absyn_Kind*_tmpCC5;struct Cyc_Absyn_Kind
_tmpCC6;enum Cyc_Absyn_KindQual _tmpCC7;struct Cyc_Core_Opt*_tmpCC9;struct Cyc_Core_Opt**
_tmpCCA;struct Cyc_Absyn_Kind*_tmpCCB;struct Cyc_Absyn_Kind _tmpCCC;enum Cyc_Absyn_KindQual
_tmpCCD;struct Cyc_Absyn_Kind*_tmpCCF;struct Cyc_Absyn_Kind _tmpCD0;enum Cyc_Absyn_KindQual
_tmpCD1;struct Cyc_Core_Opt*_tmpCD3;struct Cyc_Core_Opt**_tmpCD4;_LL651:{struct Cyc_Absyn_Less_kb_struct*
_tmpCBA=(struct Cyc_Absyn_Less_kb_struct*)_tmpCB9;if(_tmpCBA->tag != 2)goto _LL653;
else{_tmpCBB=_tmpCBA->f1;_tmpCBC=(struct Cyc_Core_Opt**)& _tmpCBA->f1;_tmpCBD=
_tmpCBA->f2;}}if(!(_tmpCBD->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652: if(
_tmpCBD->aliasqual == Cyc_Absyn_Top){*_tmpCBC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCBF=_tmpCBD;goto _LL654;}*_tmpCBC=Cyc_Tcutil_kind_to_bound_opt(_tmpCBD);
_tmpCBF=_tmpCBD;goto _LL654;_LL653:{struct Cyc_Absyn_Eq_kb_struct*_tmpCBE=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCB9;if(_tmpCBE->tag != 0)goto _LL655;else{_tmpCBF=
_tmpCBE->f1;}}if(!(_tmpCBF->kind == Cyc_Absyn_RgnKind))goto _LL655;_LL654:{struct
Cyc_Absyn_AccessEff_struct*_tmp1507;struct Cyc_Absyn_VarType_struct*_tmp1506;
struct Cyc_Absyn_VarType_struct _tmp1505;struct Cyc_Absyn_AccessEff_struct _tmp1504;
struct Cyc_List_List*_tmp1503;effect=((_tmp1503=_cycalloc(sizeof(*_tmp1503)),((
_tmp1503->hd=(void*)((_tmp1507=_cycalloc(sizeof(*_tmp1507)),((_tmp1507[0]=((
_tmp1504.tag=24,((_tmp1504.f1=(void*)((void*)((_tmp1506=_cycalloc(sizeof(*
_tmp1506)),((_tmp1506[0]=((_tmp1505.tag=2,((_tmp1505.f1=_tmpCB7,_tmp1505)))),
_tmp1506))))),_tmp1504)))),_tmp1507)))),((_tmp1503->tl=effect,_tmp1503))))));}
goto _LL650;_LL655: {struct Cyc_Absyn_Less_kb_struct*_tmpCC0=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCB9;if(_tmpCC0->tag != 2)goto _LL657;else{_tmpCC1=_tmpCC0->f2;_tmpCC2=*_tmpCC1;
_tmpCC3=_tmpCC2.kind;if(_tmpCC3 != Cyc_Absyn_IntKind)goto _LL657;}}_LL656: goto
_LL658;_LL657: {struct Cyc_Absyn_Eq_kb_struct*_tmpCC4=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCB9;if(_tmpCC4->tag != 0)goto _LL659;else{_tmpCC5=_tmpCC4->f1;_tmpCC6=*_tmpCC5;
_tmpCC7=_tmpCC6.kind;if(_tmpCC7 != Cyc_Absyn_IntKind)goto _LL659;}}_LL658: goto
_LL650;_LL659: {struct Cyc_Absyn_Less_kb_struct*_tmpCC8=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCB9;if(_tmpCC8->tag != 2)goto _LL65B;else{_tmpCC9=_tmpCC8->f1;_tmpCCA=(struct
Cyc_Core_Opt**)& _tmpCC8->f1;_tmpCCB=_tmpCC8->f2;_tmpCCC=*_tmpCCB;_tmpCCD=_tmpCCC.kind;
if(_tmpCCD != Cyc_Absyn_EffKind)goto _LL65B;}}_LL65A:*_tmpCCA=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL65C;_LL65B: {struct Cyc_Absyn_Eq_kb_struct*_tmpCCE=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCB9;if(_tmpCCE->tag != 0)goto _LL65D;else{_tmpCCF=
_tmpCCE->f1;_tmpCD0=*_tmpCCF;_tmpCD1=_tmpCD0.kind;if(_tmpCD1 != Cyc_Absyn_EffKind)
goto _LL65D;}}_LL65C:{struct Cyc_Absyn_VarType_struct*_tmp150D;struct Cyc_Absyn_VarType_struct
_tmp150C;struct Cyc_List_List*_tmp150B;effect=((_tmp150B=_cycalloc(sizeof(*
_tmp150B)),((_tmp150B->hd=(void*)((_tmp150D=_cycalloc(sizeof(*_tmp150D)),((
_tmp150D[0]=((_tmp150C.tag=2,((_tmp150C.f1=_tmpCB7,_tmp150C)))),_tmp150D)))),((
_tmp150B->tl=effect,_tmp150B))))));}goto _LL650;_LL65D: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCD2=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpCB9;if(_tmpCD2->tag != 1)goto
_LL65F;else{_tmpCD3=_tmpCD2->f1;_tmpCD4=(struct Cyc_Core_Opt**)& _tmpCD2->f1;}}
_LL65E:{struct Cyc_Absyn_Less_kb_struct*_tmp1513;struct Cyc_Absyn_Less_kb_struct
_tmp1512;struct Cyc_Core_Opt*_tmp1511;*_tmpCD4=((_tmp1511=_cycalloc(sizeof(*
_tmp1511)),((_tmp1511->v=(void*)((_tmp1513=_cycalloc(sizeof(*_tmp1513)),((
_tmp1513[0]=((_tmp1512.tag=2,((_tmp1512.f1=0,((_tmp1512.f2=& Cyc_Tcutil_ak,
_tmp1512)))))),_tmp1513)))),_tmp1511))));}goto _LL660;_LL65F:;_LL660:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1522;struct Cyc_Absyn_VarType_struct*_tmp1521;struct Cyc_Absyn_VarType_struct
_tmp1520;struct Cyc_Absyn_RgnsEff_struct _tmp151F;struct Cyc_List_List*_tmp151E;
effect=((_tmp151E=_cycalloc(sizeof(*_tmp151E)),((_tmp151E->hd=(void*)((_tmp1522=
_cycalloc(sizeof(*_tmp1522)),((_tmp1522[0]=((_tmp151F.tag=26,((_tmp151F.f1=(void*)((
void*)((_tmp1521=_cycalloc(sizeof(*_tmp1521)),((_tmp1521[0]=((_tmp1520.tag=2,((
_tmp1520.f1=_tmpCB7,_tmp1520)))),_tmp1521))))),_tmp151F)))),_tmp1522)))),((
_tmp151E->tl=effect,_tmp151E))))));}goto _LL650;_LL650:;};}}{struct Cyc_List_List*
ts=_tmpC60.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCE6;int _tmpCE7;struct
_tuple30 _tmpCE5=*((struct _tuple30*)ts->hd);_tmpCE6=_tmpCE5.f1;_tmpCE7=_tmpCE5.f2;
if(!_tmpCE7)continue;{struct Cyc_Absyn_Kind*_tmpCE8=Cyc_Tcutil_typ_kind(_tmpCE6);
struct Cyc_Absyn_Kind _tmpCE9;enum Cyc_Absyn_KindQual _tmpCEA;struct Cyc_Absyn_Kind
_tmpCEB;enum Cyc_Absyn_KindQual _tmpCEC;struct Cyc_Absyn_Kind _tmpCED;enum Cyc_Absyn_KindQual
_tmpCEE;_LL662: _tmpCE9=*_tmpCE8;_tmpCEA=_tmpCE9.kind;if(_tmpCEA != Cyc_Absyn_RgnKind)
goto _LL664;_LL663:{struct Cyc_Absyn_AccessEff_struct*_tmp1528;struct Cyc_Absyn_AccessEff_struct
_tmp1527;struct Cyc_List_List*_tmp1526;effect=((_tmp1526=_cycalloc(sizeof(*
_tmp1526)),((_tmp1526->hd=(void*)((_tmp1528=_cycalloc(sizeof(*_tmp1528)),((
_tmp1528[0]=((_tmp1527.tag=24,((_tmp1527.f1=(void*)_tmpCE6,_tmp1527)))),_tmp1528)))),((
_tmp1526->tl=effect,_tmp1526))))));}goto _LL661;_LL664: _tmpCEB=*_tmpCE8;_tmpCEC=
_tmpCEB.kind;if(_tmpCEC != Cyc_Absyn_EffKind)goto _LL666;_LL665:{struct Cyc_List_List*
_tmp1529;effect=((_tmp1529=_cycalloc(sizeof(*_tmp1529)),((_tmp1529->hd=_tmpCE6,((
_tmp1529->tl=effect,_tmp1529))))));}goto _LL661;_LL666: _tmpCED=*_tmpCE8;_tmpCEE=
_tmpCED.kind;if(_tmpCEE != Cyc_Absyn_IntKind)goto _LL668;_LL667: goto _LL661;_LL668:;
_LL669:{struct Cyc_Absyn_RgnsEff_struct*_tmp152F;struct Cyc_Absyn_RgnsEff_struct
_tmp152E;struct Cyc_List_List*_tmp152D;effect=((_tmp152D=_cycalloc(sizeof(*
_tmp152D)),((_tmp152D->hd=(void*)((_tmp152F=_cycalloc(sizeof(*_tmp152F)),((
_tmp152F[0]=((_tmp152E.tag=26,((_tmp152E.f1=(void*)_tmpCE6,_tmp152E)))),_tmp152F)))),((
_tmp152D->tl=effect,_tmp152D))))));}goto _LL661;_LL661:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1535;struct Cyc_Absyn_JoinEff_struct _tmp1534;struct Cyc_Core_Opt*_tmp1533;*
_tmpB62=((_tmp1533=_cycalloc(sizeof(*_tmp1533)),((_tmp1533->v=(void*)((_tmp1535=
_cycalloc(sizeof(*_tmp1535)),((_tmp1535[0]=((_tmp1534.tag=25,((_tmp1534.f1=
effect,_tmp1534)))),_tmp1535)))),_tmp1533))));};}if(*_tmpB60 != 0){struct Cyc_List_List*
bs=*_tmpB60;for(0;bs != 0;bs=bs->tl){void*_tmpCF9=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCFB;struct Cyc_Core_Opt**
_tmpCFC;struct Cyc_Core_Opt*_tmpCFE;struct Cyc_Core_Opt**_tmpCFF;struct Cyc_Absyn_Kind*
_tmpD00;struct Cyc_Absyn_Kind _tmpD01;enum Cyc_Absyn_KindQual _tmpD02;enum Cyc_Absyn_AliasQual
_tmpD03;struct Cyc_Core_Opt*_tmpD05;struct Cyc_Core_Opt**_tmpD06;struct Cyc_Absyn_Kind*
_tmpD07;struct Cyc_Absyn_Kind _tmpD08;enum Cyc_Absyn_KindQual _tmpD09;enum Cyc_Absyn_AliasQual
_tmpD0A;struct Cyc_Core_Opt*_tmpD0C;struct Cyc_Core_Opt**_tmpD0D;struct Cyc_Absyn_Kind*
_tmpD0E;struct Cyc_Absyn_Kind _tmpD0F;enum Cyc_Absyn_KindQual _tmpD10;enum Cyc_Absyn_AliasQual
_tmpD11;struct Cyc_Core_Opt*_tmpD13;struct Cyc_Core_Opt**_tmpD14;struct Cyc_Absyn_Kind*
_tmpD15;struct Cyc_Absyn_Kind _tmpD16;enum Cyc_Absyn_KindQual _tmpD17;enum Cyc_Absyn_AliasQual
_tmpD18;struct Cyc_Core_Opt*_tmpD1A;struct Cyc_Core_Opt**_tmpD1B;struct Cyc_Absyn_Kind*
_tmpD1C;struct Cyc_Absyn_Kind _tmpD1D;enum Cyc_Absyn_KindQual _tmpD1E;enum Cyc_Absyn_AliasQual
_tmpD1F;struct Cyc_Core_Opt*_tmpD21;struct Cyc_Core_Opt**_tmpD22;struct Cyc_Absyn_Kind*
_tmpD23;struct Cyc_Absyn_Kind _tmpD24;enum Cyc_Absyn_KindQual _tmpD25;enum Cyc_Absyn_AliasQual
_tmpD26;struct Cyc_Core_Opt*_tmpD28;struct Cyc_Core_Opt**_tmpD29;struct Cyc_Absyn_Kind*
_tmpD2A;struct Cyc_Absyn_Kind _tmpD2B;enum Cyc_Absyn_KindQual _tmpD2C;enum Cyc_Absyn_AliasQual
_tmpD2D;struct Cyc_Core_Opt*_tmpD2F;struct Cyc_Core_Opt**_tmpD30;struct Cyc_Absyn_Kind*
_tmpD31;struct Cyc_Absyn_Kind _tmpD32;enum Cyc_Absyn_KindQual _tmpD33;enum Cyc_Absyn_AliasQual
_tmpD34;struct Cyc_Core_Opt*_tmpD36;struct Cyc_Core_Opt**_tmpD37;struct Cyc_Absyn_Kind*
_tmpD38;struct Cyc_Absyn_Kind*_tmpD3A;struct Cyc_Absyn_Kind _tmpD3B;enum Cyc_Absyn_KindQual
_tmpD3C;_LL66B: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCFA=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCF9;if(_tmpCFA->tag != 1)goto _LL66D;else{_tmpCFB=_tmpCFA->f1;_tmpCFC=(struct
Cyc_Core_Opt**)& _tmpCFA->f1;}}_LL66C:{const char*_tmp1539;void*_tmp1538[1];struct
Cyc_String_pa_struct _tmp1537;(_tmp1537.tag=0,((_tmp1537.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1538[0]=&
_tmp1537,Cyc_Tcutil_warn(loc,((_tmp1539="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1539,sizeof(char),47))),_tag_dyneither(_tmp1538,sizeof(void*),
1)))))));}_tmpCFF=_tmpCFC;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_struct*
_tmpCFD=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;if(_tmpCFD->tag != 2)goto _LL66F;
else{_tmpCFE=_tmpCFD->f1;_tmpCFF=(struct Cyc_Core_Opt**)& _tmpCFD->f1;_tmpD00=
_tmpCFD->f2;_tmpD01=*_tmpD00;_tmpD02=_tmpD01.kind;if(_tmpD02 != Cyc_Absyn_BoxKind)
goto _LL66F;_tmpD03=_tmpD01.aliasqual;if(_tmpD03 != Cyc_Absyn_Top)goto _LL66F;}}
_LL66E: _tmpD06=_tmpCFF;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_struct*
_tmpD04=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;if(_tmpD04->tag != 2)goto _LL671;
else{_tmpD05=_tmpD04->f1;_tmpD06=(struct Cyc_Core_Opt**)& _tmpD04->f1;_tmpD07=
_tmpD04->f2;_tmpD08=*_tmpD07;_tmpD09=_tmpD08.kind;if(_tmpD09 != Cyc_Absyn_MemKind)
goto _LL671;_tmpD0A=_tmpD08.aliasqual;if(_tmpD0A != Cyc_Absyn_Top)goto _LL671;}}
_LL670: _tmpD0D=_tmpD06;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_struct*
_tmpD0B=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;if(_tmpD0B->tag != 2)goto _LL673;
else{_tmpD0C=_tmpD0B->f1;_tmpD0D=(struct Cyc_Core_Opt**)& _tmpD0B->f1;_tmpD0E=
_tmpD0B->f2;_tmpD0F=*_tmpD0E;_tmpD10=_tmpD0F.kind;if(_tmpD10 != Cyc_Absyn_MemKind)
goto _LL673;_tmpD11=_tmpD0F.aliasqual;if(_tmpD11 != Cyc_Absyn_Aliasable)goto _LL673;}}
_LL672: _tmpD14=_tmpD0D;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_struct*
_tmpD12=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;if(_tmpD12->tag != 2)goto _LL675;
else{_tmpD13=_tmpD12->f1;_tmpD14=(struct Cyc_Core_Opt**)& _tmpD12->f1;_tmpD15=
_tmpD12->f2;_tmpD16=*_tmpD15;_tmpD17=_tmpD16.kind;if(_tmpD17 != Cyc_Absyn_AnyKind)
goto _LL675;_tmpD18=_tmpD16.aliasqual;if(_tmpD18 != Cyc_Absyn_Top)goto _LL675;}}
_LL674: _tmpD1B=_tmpD14;goto _LL676;_LL675: {struct Cyc_Absyn_Less_kb_struct*
_tmpD19=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;if(_tmpD19->tag != 2)goto _LL677;
else{_tmpD1A=_tmpD19->f1;_tmpD1B=(struct Cyc_Core_Opt**)& _tmpD19->f1;_tmpD1C=
_tmpD19->f2;_tmpD1D=*_tmpD1C;_tmpD1E=_tmpD1D.kind;if(_tmpD1E != Cyc_Absyn_AnyKind)
goto _LL677;_tmpD1F=_tmpD1D.aliasqual;if(_tmpD1F != Cyc_Absyn_Aliasable)goto _LL677;}}
_LL676:*_tmpD1B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL66A;_LL677: {
struct Cyc_Absyn_Less_kb_struct*_tmpD20=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;
if(_tmpD20->tag != 2)goto _LL679;else{_tmpD21=_tmpD20->f1;_tmpD22=(struct Cyc_Core_Opt**)&
_tmpD20->f1;_tmpD23=_tmpD20->f2;_tmpD24=*_tmpD23;_tmpD25=_tmpD24.kind;if(_tmpD25
!= Cyc_Absyn_MemKind)goto _LL679;_tmpD26=_tmpD24.aliasqual;if(_tmpD26 != Cyc_Absyn_Unique)
goto _LL679;}}_LL678: _tmpD29=_tmpD22;goto _LL67A;_LL679: {struct Cyc_Absyn_Less_kb_struct*
_tmpD27=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;if(_tmpD27->tag != 2)goto _LL67B;
else{_tmpD28=_tmpD27->f1;_tmpD29=(struct Cyc_Core_Opt**)& _tmpD27->f1;_tmpD2A=
_tmpD27->f2;_tmpD2B=*_tmpD2A;_tmpD2C=_tmpD2B.kind;if(_tmpD2C != Cyc_Absyn_AnyKind)
goto _LL67B;_tmpD2D=_tmpD2B.aliasqual;if(_tmpD2D != Cyc_Absyn_Unique)goto _LL67B;}}
_LL67A:*_tmpD29=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL66A;_LL67B: {
struct Cyc_Absyn_Less_kb_struct*_tmpD2E=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF9;
if(_tmpD2E->tag != 2)goto _LL67D;else{_tmpD2F=_tmpD2E->f1;_tmpD30=(struct Cyc_Core_Opt**)&
_tmpD2E->f1;_tmpD31=_tmpD2E->f2;_tmpD32=*_tmpD31;_tmpD33=_tmpD32.kind;if(_tmpD33
!= Cyc_Absyn_RgnKind)goto _LL67D;_tmpD34=_tmpD32.aliasqual;if(_tmpD34 != Cyc_Absyn_Top)
goto _LL67D;}}_LL67C:*_tmpD30=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL66A;_LL67D: {struct Cyc_Absyn_Less_kb_struct*_tmpD35=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCF9;if(_tmpD35->tag != 2)goto _LL67F;else{_tmpD36=_tmpD35->f1;_tmpD37=(struct
Cyc_Core_Opt**)& _tmpD35->f1;_tmpD38=_tmpD35->f2;}}_LL67E:*_tmpD37=Cyc_Tcutil_kind_to_bound_opt(
_tmpD38);goto _LL66A;_LL67F: {struct Cyc_Absyn_Eq_kb_struct*_tmpD39=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCF9;if(_tmpD39->tag != 0)goto _LL681;else{_tmpD3A=_tmpD39->f1;_tmpD3B=*_tmpD3A;
_tmpD3C=_tmpD3B.kind;if(_tmpD3C != Cyc_Absyn_MemKind)goto _LL681;}}_LL680:{const
char*_tmp153C;void*_tmp153B;(_tmp153B=0,Cyc_Tcutil_terr(loc,((_tmp153C="functions can't abstract M types",
_tag_dyneither(_tmp153C,sizeof(char),33))),_tag_dyneither(_tmp153B,sizeof(void*),
0)));}goto _LL66A;_LL681:;_LL682: goto _LL66A;_LL66A:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC60.kind_env,*_tmpB60);_tmpC60.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC60.r,_tmpC60.free_vars,*_tmpB60);{struct Cyc_List_List*tvs=_tmpC60.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD43;int _tmpD44;struct
_tuple29 _tmpD42=*((struct _tuple29*)tvs->hd);_tmpD43=_tmpD42.f1;_tmpD44=_tmpD42.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD43,_tmpD44);}}{struct Cyc_List_List*evs=_tmpC60.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD46;int _tmpD47;struct _tuple30 _tmpD45=*((struct _tuple30*)evs->hd);
_tmpD46=_tmpD45.f1;_tmpD47=_tmpD45.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD46,_tmpD47);}};}goto _LL588;};}_LL5A9: {struct Cyc_Absyn_TupleType_struct*
_tmpB6B=(struct Cyc_Absyn_TupleType_struct*)_tmpB23;if(_tmpB6B->tag != 11)goto
_LL5AB;else{_tmpB6C=_tmpB6B->f1;}}_LL5AA: for(0;_tmpB6C != 0;_tmpB6C=_tmpB6C->tl){
struct _tuple11*_tmpD49=(struct _tuple11*)_tmpB6C->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD49).f2,1);((*_tmpD49).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD49).f1).print_const,(*_tmpD49).f2);}goto _LL588;_LL5AB: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB6D=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB23;if(_tmpB6D->tag != 13)goto
_LL5AD;else{_tmpB6E=_tmpB6D->f1;_tmpB6F=_tmpB6D->f2;}}_LL5AC: {struct
_RegionHandle*_tmpD4A=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB6F != 0;_tmpB6F=_tmpB6F->tl){struct Cyc_Absyn_Aggrfield _tmpD4C;struct
_dyneither_ptr*_tmpD4D;struct Cyc_Absyn_Tqual _tmpD4E;struct Cyc_Absyn_Tqual*
_tmpD4F;void*_tmpD50;struct Cyc_Absyn_Exp*_tmpD51;struct Cyc_List_List*_tmpD52;
struct Cyc_Absyn_Aggrfield*_tmpD4B=(struct Cyc_Absyn_Aggrfield*)_tmpB6F->hd;
_tmpD4C=*_tmpD4B;_tmpD4D=_tmpD4C.name;_tmpD4E=_tmpD4C.tq;_tmpD4F=(struct Cyc_Absyn_Tqual*)&(*
_tmpD4B).tq;_tmpD50=_tmpD4C.type;_tmpD51=_tmpD4C.width;_tmpD52=_tmpD4C.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD4D)){
const char*_tmp1540;void*_tmp153F[1];struct Cyc_String_pa_struct _tmp153E;(_tmp153E.tag=
0,((_tmp153E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD4D),((
_tmp153F[0]=& _tmp153E,Cyc_Tcutil_terr(loc,((_tmp1540="duplicate field %s",
_tag_dyneither(_tmp1540,sizeof(char),19))),_tag_dyneither(_tmp153F,sizeof(void*),
1)))))));}{const char*_tmp1541;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD4D,((
_tmp1541="",_tag_dyneither(_tmp1541,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1542;prev_fields=((_tmp1542=_region_malloc(_tmpD4A,sizeof(*_tmp1542)),((
_tmp1542->hd=_tmpD4D,((_tmp1542->tl=prev_fields,_tmp1542))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD50,1);_tmpD4F->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD4F->print_const,_tmpD50);if(_tmpB6E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD50)){const char*_tmp1546;void*_tmp1545[1];struct Cyc_String_pa_struct _tmp1544;(
_tmp1544.tag=0,((_tmp1544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD4D),((_tmp1545[0]=& _tmp1544,Cyc_Tcutil_warn(loc,((_tmp1546="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1546,sizeof(char),74))),_tag_dyneither(_tmp1545,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD50,_tmpD51,_tmpD4D);Cyc_Tcutil_check_field_atts(
loc,_tmpD4D,_tmpD52);}}goto _LL588;}_LL5AD: {struct Cyc_Absyn_AggrType_struct*
_tmpB70=(struct Cyc_Absyn_AggrType_struct*)_tmpB23;if(_tmpB70->tag != 12)goto
_LL5AF;else{_tmpB71=_tmpB70->f1;_tmpB72=_tmpB71.aggr_info;_tmpB73=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB70->f1).aggr_info;_tmpB74=_tmpB71.targs;_tmpB75=(struct Cyc_List_List**)&(
_tmpB70->f1).targs;}}_LL5AE:{union Cyc_Absyn_AggrInfoU _tmpD5B=*_tmpB73;struct
_tuple4 _tmpD5C;enum Cyc_Absyn_AggrKind _tmpD5D;struct _tuple2*_tmpD5E;struct Cyc_Core_Opt*
_tmpD5F;struct Cyc_Absyn_Aggrdecl**_tmpD60;struct Cyc_Absyn_Aggrdecl*_tmpD61;
_LL684: if((_tmpD5B.UnknownAggr).tag != 1)goto _LL686;_tmpD5C=(struct _tuple4)(
_tmpD5B.UnknownAggr).val;_tmpD5D=_tmpD5C.f1;_tmpD5E=_tmpD5C.f2;_tmpD5F=_tmpD5C.f3;
_LL685: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD62;_push_handler(&
_tmpD62);{int _tmpD64=0;if(setjmp(_tmpD62.handler))_tmpD64=1;if(!_tmpD64){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD5E);{struct Cyc_Absyn_Aggrdecl*_tmpD65=*adp;if(_tmpD65->kind != 
_tmpD5D){if(_tmpD65->kind == Cyc_Absyn_StructA){const char*_tmp154B;void*_tmp154A[
2];struct Cyc_String_pa_struct _tmp1549;struct Cyc_String_pa_struct _tmp1548;(
_tmp1548.tag=0,((_tmp1548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5E)),((_tmp1549.tag=0,((_tmp1549.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5E)),((_tmp154A[0]=& _tmp1549,((
_tmp154A[1]=& _tmp1548,Cyc_Tcutil_terr(loc,((_tmp154B="expecting struct %s instead of union %s",
_tag_dyneither(_tmp154B,sizeof(char),40))),_tag_dyneither(_tmp154A,sizeof(void*),
2)))))))))))));}else{const char*_tmp1550;void*_tmp154F[2];struct Cyc_String_pa_struct
_tmp154E;struct Cyc_String_pa_struct _tmp154D;(_tmp154D.tag=0,((_tmp154D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5E)),((
_tmp154E.tag=0,((_tmp154E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5E)),((_tmp154F[0]=& _tmp154E,((_tmp154F[1]=& _tmp154D,Cyc_Tcutil_terr(loc,((
_tmp1550="expecting union %s instead of struct %s",_tag_dyneither(_tmp1550,
sizeof(char),40))),_tag_dyneither(_tmp154F,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD5F  && (int)_tmpD5F->v){if(!((unsigned int)_tmpD65->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD65->impl))->tagged){const char*
_tmp1554;void*_tmp1553[1];struct Cyc_String_pa_struct _tmp1552;(_tmp1552.tag=0,((
_tmp1552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5E)),((_tmp1553[0]=& _tmp1552,Cyc_Tcutil_terr(loc,((_tmp1554="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1554,sizeof(char),42))),_tag_dyneither(_tmp1553,sizeof(void*),
1)))))));}}*_tmpB73=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD63=(void*)_exn_thrown;void*_tmpD72=_tmpD63;_LL689: {struct Cyc_Dict_Absent_struct*
_tmpD73=(struct Cyc_Dict_Absent_struct*)_tmpD72;if(_tmpD73->tag != Cyc_Dict_Absent)
goto _LL68B;}_LL68A: {struct Cyc_Tcenv_Genv*_tmpD74=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1555;struct Cyc_Absyn_Aggrdecl*_tmpD75=(_tmp1555=
_cycalloc(sizeof(*_tmp1555)),((_tmp1555->kind=_tmpD5D,((_tmp1555->sc=Cyc_Absyn_Extern,((
_tmp1555->name=_tmpD5E,((_tmp1555->tvs=0,((_tmp1555->impl=0,((_tmp1555->attributes=
0,_tmp1555)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD74,loc,_tmpD75);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD5E);*_tmpB73=Cyc_Absyn_KnownAggr(adp);if(*_tmpB75 != 0){{const char*
_tmp1559;void*_tmp1558[1];struct Cyc_String_pa_struct _tmp1557;(_tmp1557.tag=0,((
_tmp1557.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5E)),((_tmp1558[0]=& _tmp1557,Cyc_Tcutil_terr(loc,((_tmp1559="declare parameterized type %s before using",
_tag_dyneither(_tmp1559,sizeof(char),43))),_tag_dyneither(_tmp1558,sizeof(void*),
1)))))));}return cvtenv;}goto _LL688;}_LL68B:;_LL68C:(void)_throw(_tmpD72);_LL688:;}};}
_tmpD61=*adp;goto _LL687;}_LL686: if((_tmpD5B.KnownAggr).tag != 2)goto _LL683;
_tmpD60=(struct Cyc_Absyn_Aggrdecl**)(_tmpD5B.KnownAggr).val;_tmpD61=*_tmpD60;
_LL687: {struct Cyc_List_List*tvs=_tmpD61->tvs;struct Cyc_List_List*ts=*_tmpB75;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD7A=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD7B=(void*)ts->hd;{struct _tuple0 _tmp155A;
struct _tuple0 _tmpD7D=(_tmp155A.f1=Cyc_Absyn_compress_kb(_tmpD7A->kind),((
_tmp155A.f2=_tmpD7B,_tmp155A)));void*_tmpD7E;void*_tmpD80;struct Cyc_Absyn_Tvar*
_tmpD82;_LL68E: _tmpD7E=_tmpD7D.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD7F=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD7E;if(_tmpD7F->tag != 1)goto _LL690;};
_tmpD80=_tmpD7D.f2;{struct Cyc_Absyn_VarType_struct*_tmpD81=(struct Cyc_Absyn_VarType_struct*)
_tmpD80;if(_tmpD81->tag != 2)goto _LL690;else{_tmpD82=_tmpD81->f1;}};_LL68F: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD82);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD82,1);continue;_LL690:;_LL691: goto _LL68D;_LL68D:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp155E;void*_tmp155D[1];struct Cyc_String_pa_struct
_tmp155C;(_tmp155C.tag=0,((_tmp155C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD61->name)),((_tmp155D[0]=& _tmp155C,
Cyc_Tcutil_terr(loc,((_tmp155E="too many parameters for type %s",_tag_dyneither(
_tmp155E,sizeof(char),32))),_tag_dyneither(_tmp155D,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp155F;
hidden_ts=((_tmp155F=_cycalloc(sizeof(*_tmp155F)),((_tmp155F->hd=e,((_tmp155F->tl=
hidden_ts,_tmp155F))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB75=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB75,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL683:;}goto _LL588;_LL5AF: {struct Cyc_Absyn_TypedefType_struct*
_tmpB76=(struct Cyc_Absyn_TypedefType_struct*)_tmpB23;if(_tmpB76->tag != 18)goto
_LL5B1;else{_tmpB77=_tmpB76->f1;_tmpB78=_tmpB76->f2;_tmpB79=(struct Cyc_List_List**)&
_tmpB76->f2;_tmpB7A=_tmpB76->f3;_tmpB7B=(struct Cyc_Absyn_Typedefdecl**)& _tmpB76->f3;
_tmpB7C=_tmpB76->f4;_tmpB7D=(void***)& _tmpB76->f4;}}_LL5B0: {struct Cyc_List_List*
targs=*_tmpB79;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpD87;
_push_handler(& _tmpD87);{int _tmpD89=0;if(setjmp(_tmpD87.handler))_tmpD89=1;if(!
_tmpD89){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB77);;_pop_handler();}else{
void*_tmpD88=(void*)_exn_thrown;void*_tmpD8B=_tmpD88;_LL693: {struct Cyc_Dict_Absent_struct*
_tmpD8C=(struct Cyc_Dict_Absent_struct*)_tmpD8B;if(_tmpD8C->tag != Cyc_Dict_Absent)
goto _LL695;}_LL694:{const char*_tmp1563;void*_tmp1562[1];struct Cyc_String_pa_struct
_tmp1561;(_tmp1561.tag=0,((_tmp1561.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB77)),((_tmp1562[0]=& _tmp1561,Cyc_Tcutil_terr(
loc,((_tmp1563="unbound typedef name %s",_tag_dyneither(_tmp1563,sizeof(char),24))),
_tag_dyneither(_tmp1562,sizeof(void*),1)))))));}return cvtenv;_LL695:;_LL696:(
void)_throw(_tmpD8B);_LL692:;}};}*_tmpB7B=(struct Cyc_Absyn_Typedefdecl*)td;
_tmpB77[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpD90=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=
Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple15*_tmp1566;struct Cyc_List_List*_tmp1565;inst=((_tmp1565=
_region_malloc(_tmpD90,sizeof(*_tmp1565)),((_tmp1565->hd=((_tmp1566=
_region_malloc(_tmpD90,sizeof(*_tmp1566)),((_tmp1566->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1566->f2=(void*)ts->hd,_tmp1566)))))),((_tmp1565->tl=inst,_tmp1565))))));};}
if(ts != 0){const char*_tmp156A;void*_tmp1569[1];struct Cyc_String_pa_struct
_tmp1568;(_tmp1568.tag=0,((_tmp1568.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB77)),((_tmp1569[0]=& _tmp1568,Cyc_Tcutil_terr(
loc,((_tmp156A="too many parameters for typedef %s",_tag_dyneither(_tmp156A,
sizeof(char),35))),_tag_dyneither(_tmp1569,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp156B;hidden_ts=((_tmp156B=_cycalloc(sizeof(*_tmp156B)),((
_tmp156B->hd=e,((_tmp156B->tl=hidden_ts,_tmp156B))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp156E;struct Cyc_List_List*_tmp156D;inst=(
struct Cyc_List_List*)((_tmp156D=_cycalloc(sizeof(*_tmp156D)),((_tmp156D->hd=(
struct _tuple15*)((_tmp156E=_cycalloc(sizeof(*_tmp156E)),((_tmp156E->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp156E->f2=e,_tmp156E)))))),((_tmp156D->tl=inst,
_tmp156D))))));};}*_tmpB79=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD90,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp156F;*_tmpB7D=((_tmp156F=_cycalloc(sizeof(*
_tmp156F)),((_tmp156F[0]=new_typ,_tmp156F))));}}goto _LL588;};}_LL5B1: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpB7E=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpB23;if(_tmpB7E->tag != 23)goto
_LL5B3;}_LL5B2: goto _LL5B4;_LL5B3: {struct Cyc_Absyn_UniqueRgn_struct*_tmpB7F=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmpB23;if(_tmpB7F->tag != 22)goto _LL5B5;}
_LL5B4: goto _LL5B6;_LL5B5: {struct Cyc_Absyn_HeapRgn_struct*_tmpB80=(struct Cyc_Absyn_HeapRgn_struct*)
_tmpB23;if(_tmpB80->tag != 21)goto _LL5B7;}_LL5B6: goto _LL588;_LL5B7: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB81=(struct Cyc_Absyn_RgnHandleType_struct*)_tmpB23;if(_tmpB81->tag != 16)goto
_LL5B9;else{_tmpB82=(void*)_tmpB81->f1;}}_LL5B8: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB82,1);goto _LL588;_LL5B9: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB83=(struct Cyc_Absyn_DynRgnType_struct*)_tmpB23;if(_tmpB83->tag != 17)goto
_LL5BB;else{_tmpB84=(void*)_tmpB83->f1;_tmpB85=(void*)_tmpB83->f2;}}_LL5BA:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB84,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB85,1);
goto _LL588;_LL5BB: {struct Cyc_Absyn_AccessEff_struct*_tmpB86=(struct Cyc_Absyn_AccessEff_struct*)
_tmpB23;if(_tmpB86->tag != 24)goto _LL5BD;else{_tmpB87=(void*)_tmpB86->f1;}}_LL5BC:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB87,1);
goto _LL588;_LL5BD: {struct Cyc_Absyn_RgnsEff_struct*_tmpB88=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpB23;if(_tmpB88->tag != 26)goto _LL5BF;else{_tmpB89=(void*)_tmpB88->f1;}}_LL5BE:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB89,1);
goto _LL588;_LL5BF: {struct Cyc_Absyn_JoinEff_struct*_tmpB8A=(struct Cyc_Absyn_JoinEff_struct*)
_tmpB23;if(_tmpB8A->tag != 25)goto _LL588;else{_tmpB8B=_tmpB8A->f1;}}_LL5C0: for(0;
_tmpB8B != 0;_tmpB8B=_tmpB8B->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB8B->hd,1);}goto _LL588;_LL588:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpD9A=t;struct Cyc_Core_Opt*
_tmpD9C;struct Cyc_Core_Opt*_tmpD9D;_LL698: {struct Cyc_Absyn_Evar_struct*_tmpD9B=(
struct Cyc_Absyn_Evar_struct*)_tmpD9A;if(_tmpD9B->tag != 1)goto _LL69A;else{_tmpD9C=
_tmpD9B->f1;_tmpD9D=_tmpD9B->f2;}}_LL699: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpD9E=_tmpD9C;struct Cyc_Core_Opt _tmpD9F;struct Cyc_Absyn_Kind*_tmpDA0;_LL69D:
if(_tmpD9E != 0)goto _LL69F;_LL69E:{const char*_tmp1570;s=((_tmp1570="kind=NULL ",
_tag_dyneither(_tmp1570,sizeof(char),11)));}goto _LL69C;_LL69F: if(_tmpD9E == 0)
goto _LL69C;_tmpD9F=*_tmpD9E;_tmpDA0=(struct Cyc_Absyn_Kind*)_tmpD9F.v;_LL6A0:{
const char*_tmp1574;void*_tmp1573[1];struct Cyc_String_pa_struct _tmp1572;s=(struct
_dyneither_ptr)((_tmp1572.tag=0,((_tmp1572.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpDA0)),((_tmp1573[0]=& _tmp1572,Cyc_aprintf(((
_tmp1574="kind=%s ",_tag_dyneither(_tmp1574,sizeof(char),9))),_tag_dyneither(
_tmp1573,sizeof(void*),1))))))));}goto _LL69C;_LL69C:;}{struct Cyc_Core_Opt*
_tmpDA5=_tmpD9D;struct Cyc_Core_Opt _tmpDA6;void*_tmpDA7;_LL6A2: if(_tmpDA5 != 0)
goto _LL6A4;_LL6A3:{const char*_tmp1578;void*_tmp1577[1];struct Cyc_String_pa_struct
_tmp1576;s=(struct _dyneither_ptr)((_tmp1576.tag=0,((_tmp1576.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1577[0]=& _tmp1576,Cyc_aprintf(((
_tmp1578="%s ref=NULL",_tag_dyneither(_tmp1578,sizeof(char),12))),_tag_dyneither(
_tmp1577,sizeof(void*),1))))))));}goto _LL6A1;_LL6A4: if(_tmpDA5 == 0)goto _LL6A1;
_tmpDA6=*_tmpDA5;_tmpDA7=(void*)_tmpDA6.v;_LL6A5:{const char*_tmp157D;void*
_tmp157C[2];struct Cyc_String_pa_struct _tmp157B;struct Cyc_String_pa_struct
_tmp157A;s=(struct _dyneither_ptr)((_tmp157A.tag=0,((_tmp157A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpDA7)),((
_tmp157B.tag=0,((_tmp157B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp157C[0]=& _tmp157B,((_tmp157C[1]=& _tmp157A,Cyc_aprintf(((_tmp157D="%s ref=%s",
_tag_dyneither(_tmp157D,sizeof(char),10))),_tag_dyneither(_tmp157C,sizeof(void*),
2))))))))))))));}goto _LL6A1;_LL6A1:;}goto _LL697;}_LL69A:;_LL69B: goto _LL697;
_LL697:;}{const char*_tmp1583;void*_tmp1582[3];struct Cyc_String_pa_struct _tmp1581;
struct Cyc_String_pa_struct _tmp1580;struct Cyc_String_pa_struct _tmp157F;(_tmp157F.tag=
0,((_tmp157F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp1580.tag=0,((_tmp1580.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1581.tag=0,((
_tmp1581.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1582[0]=& _tmp1581,((_tmp1582[1]=& _tmp1580,((_tmp1582[2]=& _tmp157F,Cyc_Tcutil_terr(
loc,((_tmp1583="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1583,sizeof(char),51))),_tag_dyneither(_tmp1582,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpDB4=e->r;struct Cyc_List_List*_tmpDBA;struct Cyc_Absyn_Exp*_tmpDBC;struct
Cyc_Absyn_Exp*_tmpDBD;struct Cyc_Absyn_Exp*_tmpDBE;struct Cyc_Absyn_Exp*_tmpDC0;
struct Cyc_Absyn_Exp*_tmpDC1;struct Cyc_Absyn_Exp*_tmpDC3;struct Cyc_Absyn_Exp*
_tmpDC4;struct Cyc_Absyn_Exp*_tmpDC6;struct Cyc_Absyn_Exp*_tmpDC7;void*_tmpDC9;
struct Cyc_Absyn_Exp*_tmpDCA;void*_tmpDCC;void*_tmpDCE;void*_tmpDD0;struct Cyc_Absyn_Exp*
_tmpDD2;_LL6A7: {struct Cyc_Absyn_Const_e_struct*_tmpDB5=(struct Cyc_Absyn_Const_e_struct*)
_tmpDB4;if(_tmpDB5->tag != 0)goto _LL6A9;}_LL6A8: goto _LL6AA;_LL6A9: {struct Cyc_Absyn_Enum_e_struct*
_tmpDB6=(struct Cyc_Absyn_Enum_e_struct*)_tmpDB4;if(_tmpDB6->tag != 32)goto _LL6AB;}
_LL6AA: goto _LL6AC;_LL6AB: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDB7=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDB4;if(_tmpDB7->tag != 33)goto _LL6AD;}_LL6AC: goto _LL6AE;_LL6AD: {struct Cyc_Absyn_Var_e_struct*
_tmpDB8=(struct Cyc_Absyn_Var_e_struct*)_tmpDB4;if(_tmpDB8->tag != 1)goto _LL6AF;}
_LL6AE: goto _LL6A6;_LL6AF: {struct Cyc_Absyn_Primop_e_struct*_tmpDB9=(struct Cyc_Absyn_Primop_e_struct*)
_tmpDB4;if(_tmpDB9->tag != 3)goto _LL6B1;else{_tmpDBA=_tmpDB9->f2;}}_LL6B0: for(0;
_tmpDBA != 0;_tmpDBA=_tmpDBA->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpDBA->hd,te,cvtenv);}goto _LL6A6;_LL6B1: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDBB=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDB4;if(_tmpDBB->tag != 6)goto
_LL6B3;else{_tmpDBC=_tmpDBB->f1;_tmpDBD=_tmpDBB->f2;_tmpDBE=_tmpDBB->f3;}}_LL6B2:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBC,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDBD,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBE,te,
cvtenv);goto _LL6A6;_LL6B3: {struct Cyc_Absyn_And_e_struct*_tmpDBF=(struct Cyc_Absyn_And_e_struct*)
_tmpDB4;if(_tmpDBF->tag != 7)goto _LL6B5;else{_tmpDC0=_tmpDBF->f1;_tmpDC1=_tmpDBF->f2;}}
_LL6B4: _tmpDC3=_tmpDC0;_tmpDC4=_tmpDC1;goto _LL6B6;_LL6B5: {struct Cyc_Absyn_Or_e_struct*
_tmpDC2=(struct Cyc_Absyn_Or_e_struct*)_tmpDB4;if(_tmpDC2->tag != 8)goto _LL6B7;
else{_tmpDC3=_tmpDC2->f1;_tmpDC4=_tmpDC2->f2;}}_LL6B6: _tmpDC6=_tmpDC3;_tmpDC7=
_tmpDC4;goto _LL6B8;_LL6B7: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDC5=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDB4;if(_tmpDC5->tag != 9)goto _LL6B9;else{_tmpDC6=_tmpDC5->f1;_tmpDC7=_tmpDC5->f2;}}
_LL6B8: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC6,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC7,te,cvtenv);goto _LL6A6;_LL6B9: {
struct Cyc_Absyn_Cast_e_struct*_tmpDC8=(struct Cyc_Absyn_Cast_e_struct*)_tmpDB4;
if(_tmpDC8->tag != 15)goto _LL6BB;else{_tmpDC9=(void*)_tmpDC8->f1;_tmpDCA=_tmpDC8->f2;}}
_LL6BA: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDCA,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDC9,1);goto
_LL6A6;_LL6BB: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDCB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDB4;if(_tmpDCB->tag != 20)goto _LL6BD;else{_tmpDCC=(void*)_tmpDCB->f1;}}_LL6BC:
_tmpDCE=_tmpDCC;goto _LL6BE;_LL6BD: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDCD=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDB4;if(_tmpDCD->tag != 18)goto _LL6BF;
else{_tmpDCE=(void*)_tmpDCD->f1;}}_LL6BE: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDCE,1);goto _LL6A6;_LL6BF: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDCF=(struct Cyc_Absyn_Valueof_e_struct*)_tmpDB4;if(_tmpDCF->tag != 39)goto
_LL6C1;else{_tmpDD0=(void*)_tmpDCF->f1;}}_LL6C0: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDD0,1);goto _LL6A6;_LL6C1: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDD1=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDB4;if(_tmpDD1->tag != 19)goto
_LL6C3;else{_tmpDD2=_tmpDD1->f1;}}_LL6C2: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDD2,te,cvtenv);goto _LL6A6;_LL6C3:;_LL6C4: {const char*_tmp1586;void*_tmp1585;(
_tmp1585=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1586="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1586,sizeof(char),66))),_tag_dyneither(_tmp1585,sizeof(void*),
0)));}_LL6A6:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDD5=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDD7;struct
_tuple29 _tmpDD6=*((struct _tuple29*)vs->hd);_tmpDD7=_tmpDD6.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDD5,_tmpDD7);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDD9;struct _tuple30 _tmpDD8=*((struct _tuple30*)evs->hd);_tmpDD9=_tmpDD8.f1;{
void*_tmpDDA=Cyc_Tcutil_compress(_tmpDD9);struct Cyc_Core_Opt*_tmpDDC;struct Cyc_Core_Opt**
_tmpDDD;_LL6C6: {struct Cyc_Absyn_Evar_struct*_tmpDDB=(struct Cyc_Absyn_Evar_struct*)
_tmpDDA;if(_tmpDDB->tag != 1)goto _LL6C8;else{_tmpDDC=_tmpDDB->f4;_tmpDDD=(struct
Cyc_Core_Opt**)& _tmpDDB->f4;}}_LL6C7: if(*_tmpDDD == 0){struct Cyc_Core_Opt*
_tmp1587;*_tmpDDD=((_tmp1587=_cycalloc(sizeof(*_tmp1587)),((_tmp1587->v=_tmpDD5,
_tmp1587))));}else{struct Cyc_List_List*_tmpDDF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDDD))->v;struct Cyc_List_List*_tmpDE0=0;for(0;_tmpDDF != 0;
_tmpDDF=_tmpDDF->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDD5,(struct Cyc_Absyn_Tvar*)_tmpDDF->hd)){struct Cyc_List_List*_tmp1588;
_tmpDE0=((_tmp1588=_cycalloc(sizeof(*_tmp1588)),((_tmp1588->hd=(struct Cyc_Absyn_Tvar*)
_tmpDDF->hd,((_tmp1588->tl=_tmpDE0,_tmp1588))))));}}{struct Cyc_Core_Opt*_tmp1589;*
_tmpDDD=((_tmp1589=_cycalloc(sizeof(*_tmp1589)),((_tmp1589->v=_tmpDE0,_tmp1589))));};}
goto _LL6C5;_LL6C8:;_LL6C9: goto _LL6C5;_LL6C5:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpDE3=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpDE4=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp158A;struct Cyc_Tcutil_CVTEnv _tmpDE5=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp158A.r=_tmpDE4,((_tmp158A.kind_env=_tmpDE3,((_tmp158A.free_vars=0,((_tmp158A.free_evars=
0,((_tmp158A.generalize_evars=generalize_evars,((_tmp158A.fn_result=0,_tmp158A)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDE6=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDE4,(struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,
_tmpDE5.free_vars);struct Cyc_List_List*_tmpDE7=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpDE4,(void*(*)(struct _tuple30*))Cyc_Core_fst,_tmpDE5.free_evars);if(_tmpDE3 != 
0){struct Cyc_List_List*_tmpDE8=0;{struct Cyc_List_List*_tmpDE9=_tmpDE6;for(0;(
unsigned int)_tmpDE9;_tmpDE9=_tmpDE9->tl){struct Cyc_Absyn_Tvar*_tmpDEA=(struct
Cyc_Absyn_Tvar*)_tmpDE9->hd;int found=0;{struct Cyc_List_List*_tmpDEB=_tmpDE3;for(
0;(unsigned int)_tmpDEB;_tmpDEB=_tmpDEB->tl){if(Cyc_Absyn_tvar_cmp(_tmpDEA,(
struct Cyc_Absyn_Tvar*)_tmpDEB->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp158B;_tmpDE8=((_tmp158B=_region_malloc(_tmpDE4,sizeof(*_tmp158B)),((_tmp158B->hd=(
struct Cyc_Absyn_Tvar*)_tmpDE9->hd,((_tmp158B->tl=_tmpDE8,_tmp158B))))));}}}
_tmpDE6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpDE8);}{struct Cyc_List_List*x=_tmpDE6;for(0;x != 0;x=x->tl){void*_tmpDED=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDEF;struct Cyc_Core_Opt**
_tmpDF0;struct Cyc_Core_Opt*_tmpDF2;struct Cyc_Core_Opt**_tmpDF3;struct Cyc_Absyn_Kind*
_tmpDF4;struct Cyc_Absyn_Kind _tmpDF5;enum Cyc_Absyn_KindQual _tmpDF6;enum Cyc_Absyn_AliasQual
_tmpDF7;struct Cyc_Core_Opt*_tmpDF9;struct Cyc_Core_Opt**_tmpDFA;struct Cyc_Absyn_Kind*
_tmpDFB;struct Cyc_Absyn_Kind _tmpDFC;enum Cyc_Absyn_KindQual _tmpDFD;enum Cyc_Absyn_AliasQual
_tmpDFE;struct Cyc_Core_Opt*_tmpE00;struct Cyc_Core_Opt**_tmpE01;struct Cyc_Absyn_Kind*
_tmpE02;struct Cyc_Absyn_Kind _tmpE03;enum Cyc_Absyn_KindQual _tmpE04;enum Cyc_Absyn_AliasQual
_tmpE05;struct Cyc_Core_Opt*_tmpE07;struct Cyc_Core_Opt**_tmpE08;struct Cyc_Absyn_Kind*
_tmpE09;struct Cyc_Absyn_Kind _tmpE0A;enum Cyc_Absyn_KindQual _tmpE0B;enum Cyc_Absyn_AliasQual
_tmpE0C;struct Cyc_Core_Opt*_tmpE0E;struct Cyc_Core_Opt**_tmpE0F;struct Cyc_Absyn_Kind*
_tmpE10;struct Cyc_Absyn_Kind _tmpE11;enum Cyc_Absyn_KindQual _tmpE12;enum Cyc_Absyn_AliasQual
_tmpE13;struct Cyc_Core_Opt*_tmpE15;struct Cyc_Core_Opt**_tmpE16;struct Cyc_Absyn_Kind*
_tmpE17;struct Cyc_Absyn_Kind*_tmpE19;struct Cyc_Absyn_Kind _tmpE1A;enum Cyc_Absyn_KindQual
_tmpE1B;enum Cyc_Absyn_AliasQual _tmpE1C;_LL6CB: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDEE=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDED;if(_tmpDEE->tag != 1)goto
_LL6CD;else{_tmpDEF=_tmpDEE->f1;_tmpDF0=(struct Cyc_Core_Opt**)& _tmpDEE->f1;}}
_LL6CC: _tmpDF3=_tmpDF0;goto _LL6CE;_LL6CD: {struct Cyc_Absyn_Less_kb_struct*
_tmpDF1=(struct Cyc_Absyn_Less_kb_struct*)_tmpDED;if(_tmpDF1->tag != 2)goto _LL6CF;
else{_tmpDF2=_tmpDF1->f1;_tmpDF3=(struct Cyc_Core_Opt**)& _tmpDF1->f1;_tmpDF4=
_tmpDF1->f2;_tmpDF5=*_tmpDF4;_tmpDF6=_tmpDF5.kind;if(_tmpDF6 != Cyc_Absyn_BoxKind)
goto _LL6CF;_tmpDF7=_tmpDF5.aliasqual;if(_tmpDF7 != Cyc_Absyn_Top)goto _LL6CF;}}
_LL6CE: _tmpDFA=_tmpDF3;goto _LL6D0;_LL6CF: {struct Cyc_Absyn_Less_kb_struct*
_tmpDF8=(struct Cyc_Absyn_Less_kb_struct*)_tmpDED;if(_tmpDF8->tag != 2)goto _LL6D1;
else{_tmpDF9=_tmpDF8->f1;_tmpDFA=(struct Cyc_Core_Opt**)& _tmpDF8->f1;_tmpDFB=
_tmpDF8->f2;_tmpDFC=*_tmpDFB;_tmpDFD=_tmpDFC.kind;if(_tmpDFD != Cyc_Absyn_MemKind)
goto _LL6D1;_tmpDFE=_tmpDFC.aliasqual;if(_tmpDFE != Cyc_Absyn_Top)goto _LL6D1;}}
_LL6D0: _tmpE01=_tmpDFA;goto _LL6D2;_LL6D1: {struct Cyc_Absyn_Less_kb_struct*
_tmpDFF=(struct Cyc_Absyn_Less_kb_struct*)_tmpDED;if(_tmpDFF->tag != 2)goto _LL6D3;
else{_tmpE00=_tmpDFF->f1;_tmpE01=(struct Cyc_Core_Opt**)& _tmpDFF->f1;_tmpE02=
_tmpDFF->f2;_tmpE03=*_tmpE02;_tmpE04=_tmpE03.kind;if(_tmpE04 != Cyc_Absyn_MemKind)
goto _LL6D3;_tmpE05=_tmpE03.aliasqual;if(_tmpE05 != Cyc_Absyn_Aliasable)goto _LL6D3;}}
_LL6D2:*_tmpE01=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6CA;_LL6D3: {
struct Cyc_Absyn_Less_kb_struct*_tmpE06=(struct Cyc_Absyn_Less_kb_struct*)_tmpDED;
if(_tmpE06->tag != 2)goto _LL6D5;else{_tmpE07=_tmpE06->f1;_tmpE08=(struct Cyc_Core_Opt**)&
_tmpE06->f1;_tmpE09=_tmpE06->f2;_tmpE0A=*_tmpE09;_tmpE0B=_tmpE0A.kind;if(_tmpE0B
!= Cyc_Absyn_MemKind)goto _LL6D5;_tmpE0C=_tmpE0A.aliasqual;if(_tmpE0C != Cyc_Absyn_Unique)
goto _LL6D5;}}_LL6D4:*_tmpE08=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6CA;_LL6D5: {struct Cyc_Absyn_Less_kb_struct*_tmpE0D=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDED;if(_tmpE0D->tag != 2)goto _LL6D7;else{_tmpE0E=_tmpE0D->f1;_tmpE0F=(struct
Cyc_Core_Opt**)& _tmpE0D->f1;_tmpE10=_tmpE0D->f2;_tmpE11=*_tmpE10;_tmpE12=_tmpE11.kind;
if(_tmpE12 != Cyc_Absyn_RgnKind)goto _LL6D7;_tmpE13=_tmpE11.aliasqual;if(_tmpE13 != 
Cyc_Absyn_Top)goto _LL6D7;}}_LL6D6:*_tmpE0F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6CA;_LL6D7: {struct Cyc_Absyn_Less_kb_struct*_tmpE14=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDED;if(_tmpE14->tag != 2)goto _LL6D9;else{_tmpE15=_tmpE14->f1;_tmpE16=(struct
Cyc_Core_Opt**)& _tmpE14->f1;_tmpE17=_tmpE14->f2;}}_LL6D8:*_tmpE16=Cyc_Tcutil_kind_to_bound_opt(
_tmpE17);goto _LL6CA;_LL6D9: {struct Cyc_Absyn_Eq_kb_struct*_tmpE18=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDED;if(_tmpE18->tag != 0)goto _LL6DB;else{_tmpE19=_tmpE18->f1;_tmpE1A=*_tmpE19;
_tmpE1B=_tmpE1A.kind;if(_tmpE1B != Cyc_Absyn_MemKind)goto _LL6DB;_tmpE1C=_tmpE1A.aliasqual;}}
_LL6DA:{const char*_tmp1592;void*_tmp1591[2];struct Cyc_String_pa_struct _tmp1590;
struct Cyc_Absyn_Kind*_tmp158F;struct Cyc_String_pa_struct _tmp158E;(_tmp158E.tag=0,((
_tmp158E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp158F=_cycalloc_atomic(sizeof(*_tmp158F)),((_tmp158F->kind=Cyc_Absyn_MemKind,((
_tmp158F->aliasqual=_tmpE1C,_tmp158F)))))))),((_tmp1590.tag=0,((_tmp1590.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1591[0]=& _tmp1590,((_tmp1591[1]=& _tmp158E,Cyc_Tcutil_terr(loc,((
_tmp1592="type variable %s cannot have kind %s",_tag_dyneither(_tmp1592,sizeof(
char),37))),_tag_dyneither(_tmp1591,sizeof(void*),2)))))))))))));}goto _LL6CA;
_LL6DB:;_LL6DC: goto _LL6CA;_LL6CA:;}}if(_tmpDE6 != 0  || _tmpDE7 != 0){{void*_tmpE22=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE24;struct Cyc_List_List*_tmpE25;
struct Cyc_List_List**_tmpE26;struct Cyc_Core_Opt*_tmpE27;struct Cyc_Absyn_Tqual
_tmpE28;void*_tmpE29;struct Cyc_List_List*_tmpE2A;int _tmpE2B;struct Cyc_Absyn_VarargInfo*
_tmpE2C;struct Cyc_List_List*_tmpE2D;struct Cyc_List_List*_tmpE2E;_LL6DE: {struct
Cyc_Absyn_FnType_struct*_tmpE23=(struct Cyc_Absyn_FnType_struct*)_tmpE22;if(
_tmpE23->tag != 10)goto _LL6E0;else{_tmpE24=_tmpE23->f1;_tmpE25=_tmpE24.tvars;
_tmpE26=(struct Cyc_List_List**)&(_tmpE23->f1).tvars;_tmpE27=_tmpE24.effect;
_tmpE28=_tmpE24.ret_tqual;_tmpE29=_tmpE24.ret_typ;_tmpE2A=_tmpE24.args;_tmpE2B=
_tmpE24.c_varargs;_tmpE2C=_tmpE24.cyc_varargs;_tmpE2D=_tmpE24.rgn_po;_tmpE2E=
_tmpE24.attributes;}}_LL6DF: if(*_tmpE26 == 0){*_tmpE26=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDE6);_tmpDE6=0;}goto _LL6DD;_LL6E0:;
_LL6E1: goto _LL6DD;_LL6DD:;}if(_tmpDE6 != 0){const char*_tmp1596;void*_tmp1595[1];
struct Cyc_String_pa_struct _tmp1594;(_tmp1594.tag=0,((_tmp1594.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDE6->hd)->name),((
_tmp1595[0]=& _tmp1594,Cyc_Tcutil_terr(loc,((_tmp1596="unbound type variable %s",
_tag_dyneither(_tmp1596,sizeof(char),25))),_tag_dyneither(_tmp1595,sizeof(void*),
1)))))));}if(_tmpDE7 != 0)for(0;_tmpDE7 != 0;_tmpDE7=_tmpDE7->tl){void*e=(void*)
_tmpDE7->hd;struct Cyc_Absyn_Kind*_tmpE32=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE33;enum Cyc_Absyn_KindQual _tmpE34;enum Cyc_Absyn_AliasQual _tmpE35;struct Cyc_Absyn_Kind
_tmpE36;enum Cyc_Absyn_KindQual _tmpE37;enum Cyc_Absyn_AliasQual _tmpE38;struct Cyc_Absyn_Kind
_tmpE39;enum Cyc_Absyn_KindQual _tmpE3A;enum Cyc_Absyn_AliasQual _tmpE3B;struct Cyc_Absyn_Kind
_tmpE3C;enum Cyc_Absyn_KindQual _tmpE3D;_LL6E3: _tmpE33=*_tmpE32;_tmpE34=_tmpE33.kind;
if(_tmpE34 != Cyc_Absyn_RgnKind)goto _LL6E5;_tmpE35=_tmpE33.aliasqual;if(_tmpE35 != 
Cyc_Absyn_Unique)goto _LL6E5;_LL6E4: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1599;void*_tmp1598;(_tmp1598=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1599="can't unify evar with unique region!",
_tag_dyneither(_tmp1599,sizeof(char),37))),_tag_dyneither(_tmp1598,sizeof(void*),
0)));}goto _LL6E2;_LL6E5: _tmpE36=*_tmpE32;_tmpE37=_tmpE36.kind;if(_tmpE37 != Cyc_Absyn_RgnKind)
goto _LL6E7;_tmpE38=_tmpE36.aliasqual;if(_tmpE38 != Cyc_Absyn_Aliasable)goto _LL6E7;
_LL6E6: goto _LL6E8;_LL6E7: _tmpE39=*_tmpE32;_tmpE3A=_tmpE39.kind;if(_tmpE3A != Cyc_Absyn_RgnKind)
goto _LL6E9;_tmpE3B=_tmpE39.aliasqual;if(_tmpE3B != Cyc_Absyn_Top)goto _LL6E9;
_LL6E8: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp159C;
void*_tmp159B;(_tmp159B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp159C="can't unify evar with heap!",_tag_dyneither(
_tmp159C,sizeof(char),28))),_tag_dyneither(_tmp159B,sizeof(void*),0)));}goto
_LL6E2;_LL6E9: _tmpE3C=*_tmpE32;_tmpE3D=_tmpE3C.kind;if(_tmpE3D != Cyc_Absyn_EffKind)
goto _LL6EB;_LL6EA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp159F;void*_tmp159E;(_tmp159E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159F="can't unify evar with {}!",
_tag_dyneither(_tmp159F,sizeof(char),26))),_tag_dyneither(_tmp159E,sizeof(void*),
0)));}goto _LL6E2;_LL6EB:;_LL6EC:{const char*_tmp15A4;void*_tmp15A3[2];struct Cyc_String_pa_struct
_tmp15A2;struct Cyc_String_pa_struct _tmp15A1;(_tmp15A1.tag=0,((_tmp15A1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15A2.tag=0,((_tmp15A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15A3[0]=& _tmp15A2,((_tmp15A3[1]=& _tmp15A1,Cyc_Tcutil_terr(loc,((
_tmp15A4="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15A4,sizeof(char),52))),_tag_dyneither(_tmp15A3,sizeof(void*),2)))))))))))));}
goto _LL6E2;_LL6E2:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE49=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE4B;struct Cyc_List_List*_tmpE4C;struct Cyc_Core_Opt*
_tmpE4D;struct Cyc_Absyn_Tqual _tmpE4E;void*_tmpE4F;struct Cyc_List_List*_tmpE50;
_LL6EE: {struct Cyc_Absyn_FnType_struct*_tmpE4A=(struct Cyc_Absyn_FnType_struct*)
_tmpE49;if(_tmpE4A->tag != 10)goto _LL6F0;else{_tmpE4B=_tmpE4A->f1;_tmpE4C=_tmpE4B.tvars;
_tmpE4D=_tmpE4B.effect;_tmpE4E=_tmpE4B.ret_tqual;_tmpE4F=_tmpE4B.ret_typ;_tmpE50=
_tmpE4B.args;}}_LL6EF: fd->tvs=_tmpE4C;fd->effect=_tmpE4D;{struct Cyc_List_List*
_tmpE51=fd->args;for(0;_tmpE51 != 0;(_tmpE51=_tmpE51->tl,_tmpE50=_tmpE50->tl)){(*((
struct _tuple25*)_tmpE51->hd)).f2=(*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmpE50))->hd)).f2;(*((struct _tuple25*)_tmpE51->hd)).f3=(*((struct
_tuple9*)_tmpE50->hd)).f3;}}fd->ret_tqual=_tmpE4E;fd->ret_type=_tmpE4F;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE4F);
goto _LL6ED;_LL6F0:;_LL6F1: {const char*_tmp15A7;void*_tmp15A6;(_tmp15A6=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A7="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp15A7,sizeof(char),38))),_tag_dyneither(_tmp15A6,sizeof(void*),
0)));}_LL6ED:;}{struct _RegionHandle*_tmpE54=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp15A8;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple25*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpE54,(struct _dyneither_ptr*(*)(struct _tuple25*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp15A8="function declaration has repeated parameter",
_tag_dyneither(_tmp15A8,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp15A9;fd->cached_typ=((
_tmp15A9=_cycalloc(sizeof(*_tmp15A9)),((_tmp15A9->v=t,_tmp15A9))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpE57=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp15AA;
struct Cyc_Tcutil_CVTEnv _tmpE58=Cyc_Tcutil_check_valid_type(loc,te,((_tmp15AA.r=
_tmpE57,((_tmp15AA.kind_env=bound_tvars,((_tmp15AA.free_vars=0,((_tmp15AA.free_evars=
0,((_tmp15AA.generalize_evars=0,((_tmp15AA.fn_result=0,_tmp15AA)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE59=Cyc_Tcutil_remove_bound_tvars(
_tmpE57,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE57,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple29*))Cyc_Core_fst,_tmpE58.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE5A=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple30*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE57,(void*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpE58.free_evars);{struct Cyc_List_List*fs=_tmpE59;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE5B=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp15AF;void*_tmp15AE[2];struct Cyc_String_pa_struct _tmp15AD;struct Cyc_String_pa_struct
_tmp15AC;(_tmp15AC.tag=0,((_tmp15AC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp15AD.tag=0,((_tmp15AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE5B),((_tmp15AE[0]=& _tmp15AD,((
_tmp15AE[1]=& _tmp15AC,Cyc_Tcutil_terr(loc,((_tmp15AF="unbound type variable %s in type %s",
_tag_dyneither(_tmp15AF,sizeof(char),36))),_tag_dyneither(_tmp15AE,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE5A != 0)for(0;_tmpE5A != 0;_tmpE5A=
_tmpE5A->tl){void*e=(void*)_tmpE5A->hd;struct Cyc_Absyn_Kind*_tmpE60=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE61;enum Cyc_Absyn_KindQual _tmpE62;enum Cyc_Absyn_AliasQual
_tmpE63;struct Cyc_Absyn_Kind _tmpE64;enum Cyc_Absyn_KindQual _tmpE65;enum Cyc_Absyn_AliasQual
_tmpE66;struct Cyc_Absyn_Kind _tmpE67;enum Cyc_Absyn_KindQual _tmpE68;enum Cyc_Absyn_AliasQual
_tmpE69;struct Cyc_Absyn_Kind _tmpE6A;enum Cyc_Absyn_KindQual _tmpE6B;_LL6F3:
_tmpE61=*_tmpE60;_tmpE62=_tmpE61.kind;if(_tmpE62 != Cyc_Absyn_RgnKind)goto _LL6F5;
_tmpE63=_tmpE61.aliasqual;if(_tmpE63 != Cyc_Absyn_Unique)goto _LL6F5;_LL6F4: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp15B2;void*
_tmp15B1;(_tmp15B1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15B2="can't unify evar with unique region!",
_tag_dyneither(_tmp15B2,sizeof(char),37))),_tag_dyneither(_tmp15B1,sizeof(void*),
0)));}goto _LL6F2;_LL6F5: _tmpE64=*_tmpE60;_tmpE65=_tmpE64.kind;if(_tmpE65 != Cyc_Absyn_RgnKind)
goto _LL6F7;_tmpE66=_tmpE64.aliasqual;if(_tmpE66 != Cyc_Absyn_Aliasable)goto _LL6F7;
_LL6F6: goto _LL6F8;_LL6F7: _tmpE67=*_tmpE60;_tmpE68=_tmpE67.kind;if(_tmpE68 != Cyc_Absyn_RgnKind)
goto _LL6F9;_tmpE69=_tmpE67.aliasqual;if(_tmpE69 != Cyc_Absyn_Top)goto _LL6F9;
_LL6F8: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp15B5;
void*_tmp15B4;(_tmp15B4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15B5="can't unify evar with heap!",_tag_dyneither(
_tmp15B5,sizeof(char),28))),_tag_dyneither(_tmp15B4,sizeof(void*),0)));}goto
_LL6F2;_LL6F9: _tmpE6A=*_tmpE60;_tmpE6B=_tmpE6A.kind;if(_tmpE6B != Cyc_Absyn_EffKind)
goto _LL6FB;_LL6FA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15B8;void*_tmp15B7;(_tmp15B7=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15B8="can't unify evar with {}!",
_tag_dyneither(_tmp15B8,sizeof(char),26))),_tag_dyneither(_tmp15B7,sizeof(void*),
0)));}goto _LL6F2;_LL6FB:;_LL6FC:{const char*_tmp15BD;void*_tmp15BC[2];struct Cyc_String_pa_struct
_tmp15BB;struct Cyc_String_pa_struct _tmp15BA;(_tmp15BA.tag=0,((_tmp15BA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15BB.tag=0,((_tmp15BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15BC[0]=& _tmp15BB,((_tmp15BC[1]=& _tmp15BA,Cyc_Tcutil_terr(loc,((
_tmp15BD="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15BD,sizeof(char),52))),_tag_dyneither(_tmp15BC,sizeof(void*),2)))))))))))));}
goto _LL6F2;_LL6F2:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp15C2;void*_tmp15C1[2];struct Cyc_String_pa_struct
_tmp15C0;struct Cyc_String_pa_struct _tmp15BF;(_tmp15BF.tag=0,((_tmp15BF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15C0.tag=
0,((_tmp15C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15C1[0]=&
_tmp15C0,((_tmp15C1[1]=& _tmp15BF,Cyc_Tcutil_terr(loc,((_tmp15C2="%s: %s",
_tag_dyneither(_tmp15C2,sizeof(char),7))),_tag_dyneither(_tmp15C1,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp15C3;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15C3="duplicate type variable",
_tag_dyneither(_tmp15C3,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp15C4;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15C4="",_tag_dyneither(_tmp15C4,
sizeof(char),1))))!= 0){struct _tuple32*_tmp15C7;struct Cyc_List_List*_tmp15C6;
fields=((_tmp15C6=_cycalloc(sizeof(*_tmp15C6)),((_tmp15C6->hd=((_tmp15C7=
_cycalloc(sizeof(*_tmp15C7)),((_tmp15C7->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp15C7->f2=0,_tmp15C7)))))),((_tmp15C6->tl=fields,_tmp15C6))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp15C9;const char*_tmp15C8;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp15C9="struct",_tag_dyneither(_tmp15C9,sizeof(char),7)):((
_tmp15C8="union",_tag_dyneither(_tmp15C8,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple33 _tmpE80;struct Cyc_List_List*
_tmpE81;void*_tmpE82;struct _tuple33*_tmpE7F=(struct _tuple33*)des->hd;_tmpE80=*
_tmpE7F;_tmpE81=_tmpE80.f1;_tmpE82=_tmpE80.f2;if(_tmpE81 == 0){struct Cyc_List_List*
_tmpE83=fields;for(0;_tmpE83 != 0;_tmpE83=_tmpE83->tl){if(!(*((struct _tuple32*)
_tmpE83->hd)).f2){(*((struct _tuple32*)_tmpE83->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp15CF;struct Cyc_Absyn_FieldName_struct _tmp15CE;struct Cyc_List_List*_tmp15CD;(*((
struct _tuple33*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15CD=_cycalloc(sizeof(*
_tmp15CD)),((_tmp15CD->hd=(void*)((_tmp15CF=_cycalloc(sizeof(*_tmp15CF)),((
_tmp15CF[0]=((_tmp15CE.tag=1,((_tmp15CE.f1=((*((struct _tuple32*)_tmpE83->hd)).f1)->name,
_tmp15CE)))),_tmp15CF)))),((_tmp15CD->tl=0,_tmp15CD))))));}{struct _tuple34*
_tmp15D2;struct Cyc_List_List*_tmp15D1;ans=((_tmp15D1=_region_malloc(rgn,sizeof(*
_tmp15D1)),((_tmp15D1->hd=((_tmp15D2=_region_malloc(rgn,sizeof(*_tmp15D2)),((
_tmp15D2->f1=(*((struct _tuple32*)_tmpE83->hd)).f1,((_tmp15D2->f2=_tmpE82,
_tmp15D2)))))),((_tmp15D1->tl=ans,_tmp15D1))))));}break;}}if(_tmpE83 == 0){const
char*_tmp15D6;void*_tmp15D5[1];struct Cyc_String_pa_struct _tmp15D4;(_tmp15D4.tag=
0,((_tmp15D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15D5[0]=& _tmp15D4,Cyc_Tcutil_terr(loc,((_tmp15D6="too many arguments to %s",
_tag_dyneither(_tmp15D6,sizeof(char),25))),_tag_dyneither(_tmp15D5,sizeof(void*),
1)))))));}}else{if(_tmpE81->tl != 0){const char*_tmp15D9;void*_tmp15D8;(_tmp15D8=0,
Cyc_Tcutil_terr(loc,((_tmp15D9="multiple designators are not yet supported",
_tag_dyneither(_tmp15D9,sizeof(char),43))),_tag_dyneither(_tmp15D8,sizeof(void*),
0)));}else{void*_tmpE8E=(void*)_tmpE81->hd;struct _dyneither_ptr*_tmpE91;_LL6FE: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE8F=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE8E;if(_tmpE8F->tag != 0)goto _LL700;}_LL6FF:{const char*_tmp15DD;void*_tmp15DC[
1];struct Cyc_String_pa_struct _tmp15DB;(_tmp15DB.tag=0,((_tmp15DB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15DC[0]=& _tmp15DB,Cyc_Tcutil_terr(
loc,((_tmp15DD="array designator used in argument to %s",_tag_dyneither(_tmp15DD,
sizeof(char),40))),_tag_dyneither(_tmp15DC,sizeof(void*),1)))))));}goto _LL6FD;
_LL700: {struct Cyc_Absyn_FieldName_struct*_tmpE90=(struct Cyc_Absyn_FieldName_struct*)
_tmpE8E;if(_tmpE90->tag != 1)goto _LL6FD;else{_tmpE91=_tmpE90->f1;}}_LL701: {
struct Cyc_List_List*_tmpE95=fields;for(0;_tmpE95 != 0;_tmpE95=_tmpE95->tl){if(Cyc_strptrcmp(
_tmpE91,((*((struct _tuple32*)_tmpE95->hd)).f1)->name)== 0){if((*((struct _tuple32*)
_tmpE95->hd)).f2){const char*_tmp15E1;void*_tmp15E0[1];struct Cyc_String_pa_struct
_tmp15DF;(_tmp15DF.tag=0,((_tmp15DF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE91),((_tmp15E0[0]=& _tmp15DF,Cyc_Tcutil_terr(loc,((_tmp15E1="member %s has already been used as an argument",
_tag_dyneither(_tmp15E1,sizeof(char),47))),_tag_dyneither(_tmp15E0,sizeof(void*),
1)))))));}(*((struct _tuple32*)_tmpE95->hd)).f2=1;{struct _tuple34*_tmp15E4;struct
Cyc_List_List*_tmp15E3;ans=((_tmp15E3=_region_malloc(rgn,sizeof(*_tmp15E3)),((
_tmp15E3->hd=((_tmp15E4=_region_malloc(rgn,sizeof(*_tmp15E4)),((_tmp15E4->f1=(*((
struct _tuple32*)_tmpE95->hd)).f1,((_tmp15E4->f2=_tmpE82,_tmp15E4)))))),((
_tmp15E3->tl=ans,_tmp15E3))))));}break;}}if(_tmpE95 == 0){const char*_tmp15E8;void*
_tmp15E7[1];struct Cyc_String_pa_struct _tmp15E6;(_tmp15E6.tag=0,((_tmp15E6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE91),((_tmp15E7[0]=& _tmp15E6,
Cyc_Tcutil_terr(loc,((_tmp15E8="bad field designator %s",_tag_dyneither(_tmp15E8,
sizeof(char),24))),_tag_dyneither(_tmp15E7,sizeof(void*),1)))))));}goto _LL6FD;}
_LL6FD:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){{const char*_tmp15EB;void*_tmp15EA;(
_tmp15EA=0,Cyc_Tcutil_terr(loc,((_tmp15EB="too few arguments to struct",
_tag_dyneither(_tmp15EB,sizeof(char),28))),_tag_dyneither(_tmp15EA,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple32*)fields->hd)).f2){if(found){const char*_tmp15EE;void*_tmp15ED;(_tmp15ED=
0,Cyc_Tcutil_terr(loc,((_tmp15EE="only one member of a union is allowed",
_tag_dyneither(_tmp15EE,sizeof(char),38))),_tag_dyneither(_tmp15ED,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15F1;void*_tmp15F0;(_tmp15F0=0,Cyc_Tcutil_terr(
loc,((_tmp15F1="missing member for union",_tag_dyneither(_tmp15F1,sizeof(char),
25))),_tag_dyneither(_tmp15F0,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEA6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEA8;void*_tmpEA9;struct Cyc_Absyn_PtrAtts _tmpEAA;union Cyc_Absyn_Constraint*
_tmpEAB;_LL703: {struct Cyc_Absyn_PointerType_struct*_tmpEA7=(struct Cyc_Absyn_PointerType_struct*)
_tmpEA6;if(_tmpEA7->tag != 5)goto _LL705;else{_tmpEA8=_tmpEA7->f1;_tmpEA9=_tmpEA8.elt_typ;
_tmpEAA=_tmpEA8.ptr_atts;_tmpEAB=_tmpEAA.bounds;}}_LL704: {void*_tmpEAC=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpEAB);_LL708: {struct Cyc_Absyn_DynEither_b_struct*_tmpEAD=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEAC;if(_tmpEAD->tag != 0)goto _LL70A;}_LL709:*elt_typ_dest=_tmpEA9;return 1;
_LL70A:;_LL70B: return 0;_LL707:;}_LL705:;_LL706: return 0;_LL702:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEAE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEB0;void*_tmpEB1;struct Cyc_Absyn_PtrAtts _tmpEB2;union Cyc_Absyn_Constraint*
_tmpEB3;_LL70D: {struct Cyc_Absyn_PointerType_struct*_tmpEAF=(struct Cyc_Absyn_PointerType_struct*)
_tmpEAE;if(_tmpEAF->tag != 5)goto _LL70F;else{_tmpEB0=_tmpEAF->f1;_tmpEB1=_tmpEB0.elt_typ;
_tmpEB2=_tmpEB0.ptr_atts;_tmpEB3=_tmpEB2.zero_term;}}_LL70E:*elt_typ_dest=
_tmpEB1;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpEB3);_LL70F:;_LL710: return 0;_LL70C:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEB4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEB6;void*_tmpEB7;struct Cyc_Absyn_PtrAtts _tmpEB8;
union Cyc_Absyn_Constraint*_tmpEB9;union Cyc_Absyn_Constraint*_tmpEBA;struct Cyc_Absyn_ArrayInfo
_tmpEBC;void*_tmpEBD;struct Cyc_Absyn_Tqual _tmpEBE;struct Cyc_Absyn_Exp*_tmpEBF;
union Cyc_Absyn_Constraint*_tmpEC0;_LL712: {struct Cyc_Absyn_PointerType_struct*
_tmpEB5=(struct Cyc_Absyn_PointerType_struct*)_tmpEB4;if(_tmpEB5->tag != 5)goto
_LL714;else{_tmpEB6=_tmpEB5->f1;_tmpEB7=_tmpEB6.elt_typ;_tmpEB8=_tmpEB6.ptr_atts;
_tmpEB9=_tmpEB8.bounds;_tmpEBA=_tmpEB8.zero_term;}}_LL713: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEBA)){*ptr_type=t;*elt_type=
_tmpEB7;{void*_tmpEC1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpEB9);_LL719: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEC2=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEC1;if(_tmpEC2->tag != 0)goto
_LL71B;}_LL71A:*is_dyneither=1;goto _LL718;_LL71B:;_LL71C:*is_dyneither=0;goto
_LL718;_LL718:;}return 1;}else{return 0;}_LL714: {struct Cyc_Absyn_ArrayType_struct*
_tmpEBB=(struct Cyc_Absyn_ArrayType_struct*)_tmpEB4;if(_tmpEBB->tag != 9)goto
_LL716;else{_tmpEBC=_tmpEBB->f1;_tmpEBD=_tmpEBC.elt_type;_tmpEBE=_tmpEBC.tq;
_tmpEBF=_tmpEBC.num_elts;_tmpEC0=_tmpEBC.zero_term;}}_LL715: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEC0)){*elt_type=_tmpEBD;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1606;struct Cyc_Absyn_PtrAtts
_tmp1605;struct Cyc_Absyn_Upper_b_struct _tmp1604;struct Cyc_Absyn_Upper_b_struct*
_tmp1603;struct Cyc_Absyn_PtrInfo _tmp1602;struct Cyc_Absyn_PointerType_struct*
_tmp1601;*ptr_type=(void*)((_tmp1601=_cycalloc(sizeof(*_tmp1601)),((_tmp1601[0]=((
_tmp1606.tag=5,((_tmp1606.f1=((_tmp1602.elt_typ=_tmpEBD,((_tmp1602.elt_tq=
_tmpEBE,((_tmp1602.ptr_atts=((_tmp1605.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1605.nullable=Cyc_Absyn_false_conref,((_tmp1605.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1603=_cycalloc(sizeof(*_tmp1603)),((
_tmp1603[0]=((_tmp1604.tag=1,((_tmp1604.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEBF),_tmp1604)))),_tmp1603))))),((_tmp1605.zero_term=_tmpEC0,((_tmp1605.ptrloc=
0,_tmp1605)))))))))),_tmp1602)))))),_tmp1606)))),_tmp1601))));}return 1;}else{
return 0;}_LL716:;_LL717: return 0;_LL711:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEC9=e1->r;
struct Cyc_Absyn_Exp*_tmpECC;struct Cyc_Absyn_Exp*_tmpECE;struct Cyc_Absyn_Exp*
_tmpED0;struct Cyc_Absyn_Exp*_tmpED2;struct Cyc_Absyn_Exp*_tmpED4;struct Cyc_Absyn_Exp*
_tmpED6;_LL71E: {struct Cyc_Absyn_Cast_e_struct*_tmpECA=(struct Cyc_Absyn_Cast_e_struct*)
_tmpEC9;if(_tmpECA->tag != 15)goto _LL720;}_LL71F: {const char*_tmp160A;void*
_tmp1609[1];struct Cyc_String_pa_struct _tmp1608;(_tmp1608.tag=0,((_tmp1608.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1609[0]=& _tmp1608,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp160A="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp160A,sizeof(char),29))),_tag_dyneither(_tmp1609,sizeof(void*),1)))))));}
_LL720: {struct Cyc_Absyn_Deref_e_struct*_tmpECB=(struct Cyc_Absyn_Deref_e_struct*)
_tmpEC9;if(_tmpECB->tag != 21)goto _LL722;else{_tmpECC=_tmpECB->f1;}}_LL721:
_tmpECE=_tmpECC;goto _LL723;_LL722: {struct Cyc_Absyn_Subscript_e_struct*_tmpECD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpEC9;if(_tmpECD->tag != 24)goto _LL724;
else{_tmpECE=_tmpECD->f1;}}_LL723: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpECE->topt))->v,ptr_type,is_dyneither,
elt_type);_LL724: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpECF=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEC9;if(_tmpECF->tag != 23)goto _LL726;else{_tmpED0=_tmpECF->f1;}}_LL725:
_tmpED2=_tmpED0;goto _LL727;_LL726: {struct Cyc_Absyn_AggrMember_e_struct*_tmpED1=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpEC9;if(_tmpED1->tag != 22)goto _LL728;
else{_tmpED2=_tmpED1->f1;}}_LL727: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpED2->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp160E;void*_tmp160D[1];struct Cyc_String_pa_struct _tmp160C;(_tmp160C.tag=
0,((_tmp160C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp160D[0]=& _tmp160C,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160E="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp160E,sizeof(char),51))),_tag_dyneither(_tmp160D,sizeof(void*),
1)))))));}return 0;_LL728: {struct Cyc_Absyn_Instantiate_e_struct*_tmpED3=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpEC9;if(_tmpED3->tag != 14)goto _LL72A;else{
_tmpED4=_tmpED3->f1;}}_LL729: _tmpED6=_tmpED4;goto _LL72B;_LL72A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpED5=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpEC9;if(_tmpED5->tag != 13)
goto _LL72C;else{_tmpED6=_tmpED5->f1;}}_LL72B: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED6->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1612;void*_tmp1611[1];struct Cyc_String_pa_struct
_tmp1610;(_tmp1610.tag=0,((_tmp1610.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1611[0]=& _tmp1610,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1612="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1612,sizeof(char),49))),_tag_dyneither(_tmp1611,sizeof(void*),
1)))))));}return 0;_LL72C: {struct Cyc_Absyn_Var_e_struct*_tmpED7=(struct Cyc_Absyn_Var_e_struct*)
_tmpEC9;if(_tmpED7->tag != 1)goto _LL72E;}_LL72D: return 0;_LL72E:;_LL72F: {const
char*_tmp1616;void*_tmp1615[1];struct Cyc_String_pa_struct _tmp1614;(_tmp1614.tag=
0,((_tmp1614.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1615[0]=& _tmp1614,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1616="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1616,sizeof(char),39))),_tag_dyneither(_tmp1615,sizeof(void*),
1)))))));}_LL71D:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpEE4=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpEE8;_LL731: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpEE5=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpEE4;if(_tmpEE5->tag != 23)goto
_LL733;}_LL732: return !must_be_unique;_LL733: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpEE6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpEE4;if(_tmpEE6->tag != 22)goto
_LL735;}_LL734: return 1;_LL735: {struct Cyc_Absyn_VarType_struct*_tmpEE7=(struct
Cyc_Absyn_VarType_struct*)_tmpEE4;if(_tmpEE7->tag != 2)goto _LL737;else{_tmpEE8=
_tmpEE7->f1;}}_LL736: {struct Cyc_Absyn_Kind _tmpEEA;enum Cyc_Absyn_KindQual
_tmpEEB;enum Cyc_Absyn_AliasQual _tmpEEC;struct Cyc_Absyn_Kind*_tmpEE9=Cyc_Tcutil_tvar_kind(
_tmpEE8,& Cyc_Tcutil_rk);_tmpEEA=*_tmpEE9;_tmpEEB=_tmpEEA.kind;_tmpEEC=_tmpEEA.aliasqual;
if(_tmpEEB == Cyc_Absyn_RgnKind  && (_tmpEEC == Cyc_Absyn_Unique  || _tmpEEC == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpEED=Cyc_Absyn_compress_kb(_tmpEE8->kind);struct
Cyc_Core_Opt*_tmpEEF;struct Cyc_Core_Opt**_tmpEF0;struct Cyc_Absyn_Kind*_tmpEF1;
struct Cyc_Absyn_Kind _tmpEF2;enum Cyc_Absyn_KindQual _tmpEF3;enum Cyc_Absyn_AliasQual
_tmpEF4;_LL73A: {struct Cyc_Absyn_Less_kb_struct*_tmpEEE=(struct Cyc_Absyn_Less_kb_struct*)
_tmpEED;if(_tmpEEE->tag != 2)goto _LL73C;else{_tmpEEF=_tmpEEE->f1;_tmpEF0=(struct
Cyc_Core_Opt**)& _tmpEEE->f1;_tmpEF1=_tmpEEE->f2;_tmpEF2=*_tmpEF1;_tmpEF3=_tmpEF2.kind;
if(_tmpEF3 != Cyc_Absyn_RgnKind)goto _LL73C;_tmpEF4=_tmpEF2.aliasqual;if(_tmpEF4 != 
Cyc_Absyn_Top)goto _LL73C;}}_LL73B:{struct Cyc_Absyn_Less_kb_struct*_tmp161C;
struct Cyc_Absyn_Less_kb_struct _tmp161B;struct Cyc_Core_Opt*_tmp161A;*_tmpEF0=((
_tmp161A=_cycalloc(sizeof(*_tmp161A)),((_tmp161A->v=(void*)((_tmp161C=_cycalloc(
sizeof(*_tmp161C)),((_tmp161C[0]=((_tmp161B.tag=2,((_tmp161B.f1=0,((_tmp161B.f2=&
Cyc_Tcutil_rk,_tmp161B)))))),_tmp161C)))),_tmp161A))));}return 0;_LL73C:;_LL73D:
return 1;_LL739:;}return 0;}_LL737:;_LL738: return 0;_LL730:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEF8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEFA;struct Cyc_Absyn_PtrAtts _tmpEFB;void*_tmpEFC;struct Cyc_Absyn_Tvar*_tmpEFE;
_LL73F: {struct Cyc_Absyn_PointerType_struct*_tmpEF9=(struct Cyc_Absyn_PointerType_struct*)
_tmpEF8;if(_tmpEF9->tag != 5)goto _LL741;else{_tmpEFA=_tmpEF9->f1;_tmpEFB=_tmpEFA.ptr_atts;
_tmpEFC=_tmpEFB.rgn;}}_LL740: return Cyc_Tcutil_is_noalias_region(_tmpEFC,
must_be_unique);_LL741: {struct Cyc_Absyn_VarType_struct*_tmpEFD=(struct Cyc_Absyn_VarType_struct*)
_tmpEF8;if(_tmpEFD->tag != 2)goto _LL743;else{_tmpEFE=_tmpEFD->f1;}}_LL742: {
struct Cyc_Absyn_Kind _tmpF00;enum Cyc_Absyn_KindQual _tmpF01;enum Cyc_Absyn_AliasQual
_tmpF02;struct Cyc_Absyn_Kind*_tmpEFF=Cyc_Tcutil_tvar_kind(_tmpEFE,& Cyc_Tcutil_bk);
_tmpF00=*_tmpEFF;_tmpF01=_tmpF00.kind;_tmpF02=_tmpF00.aliasqual;switch(_tmpF01){
case Cyc_Absyn_BoxKind: _LL745: goto _LL746;case Cyc_Absyn_AnyKind: _LL746: goto _LL747;
case Cyc_Absyn_MemKind: _LL747: if(_tmpF02 == Cyc_Absyn_Unique  || _tmpF02 == Cyc_Absyn_Top){
void*_tmpF03=Cyc_Absyn_compress_kb(_tmpEFE->kind);struct Cyc_Core_Opt*_tmpF05;
struct Cyc_Core_Opt**_tmpF06;struct Cyc_Absyn_Kind*_tmpF07;struct Cyc_Absyn_Kind
_tmpF08;enum Cyc_Absyn_KindQual _tmpF09;enum Cyc_Absyn_AliasQual _tmpF0A;_LL74A: {
struct Cyc_Absyn_Less_kb_struct*_tmpF04=(struct Cyc_Absyn_Less_kb_struct*)_tmpF03;
if(_tmpF04->tag != 2)goto _LL74C;else{_tmpF05=_tmpF04->f1;_tmpF06=(struct Cyc_Core_Opt**)&
_tmpF04->f1;_tmpF07=_tmpF04->f2;_tmpF08=*_tmpF07;_tmpF09=_tmpF08.kind;_tmpF0A=
_tmpF08.aliasqual;if(_tmpF0A != Cyc_Absyn_Top)goto _LL74C;}}_LL74B:{struct Cyc_Absyn_Less_kb_struct*
_tmp1626;struct Cyc_Absyn_Kind*_tmp1625;struct Cyc_Absyn_Less_kb_struct _tmp1624;
struct Cyc_Core_Opt*_tmp1623;*_tmpF06=((_tmp1623=_cycalloc(sizeof(*_tmp1623)),((
_tmp1623->v=(void*)((_tmp1626=_cycalloc(sizeof(*_tmp1626)),((_tmp1626[0]=((
_tmp1624.tag=2,((_tmp1624.f1=0,((_tmp1624.f2=((_tmp1625=_cycalloc_atomic(sizeof(*
_tmp1625)),((_tmp1625->kind=_tmpF09,((_tmp1625->aliasqual=Cyc_Absyn_Aliasable,
_tmp1625)))))),_tmp1624)))))),_tmp1626)))),_tmp1623))));}return 0;_LL74C:;_LL74D:
return 1;_LL749:;}return 0;default: _LL748: return 0;}}_LL743:;_LL744: return 0;_LL73E:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpF0F=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpF0F))
return 1;{void*_tmpF10=_tmpF0F;struct Cyc_List_List*_tmpF12;struct Cyc_Absyn_AggrInfo
_tmpF14;union Cyc_Absyn_AggrInfoU _tmpF15;struct Cyc_Absyn_Aggrdecl**_tmpF16;struct
Cyc_List_List*_tmpF17;struct Cyc_List_List*_tmpF19;struct Cyc_Absyn_AggrInfo
_tmpF1B;union Cyc_Absyn_AggrInfoU _tmpF1C;struct _tuple4 _tmpF1D;struct Cyc_Absyn_DatatypeInfo
_tmpF1F;union Cyc_Absyn_DatatypeInfoU _tmpF20;struct Cyc_List_List*_tmpF21;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF23;union Cyc_Absyn_DatatypeFieldInfoU _tmpF24;
struct Cyc_List_List*_tmpF25;_LL750: {struct Cyc_Absyn_TupleType_struct*_tmpF11=(
struct Cyc_Absyn_TupleType_struct*)_tmpF10;if(_tmpF11->tag != 11)goto _LL752;else{
_tmpF12=_tmpF11->f1;}}_LL751: while(_tmpF12 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpF12->hd)).f2))return 1;_tmpF12=_tmpF12->tl;}return 0;
_LL752: {struct Cyc_Absyn_AggrType_struct*_tmpF13=(struct Cyc_Absyn_AggrType_struct*)
_tmpF10;if(_tmpF13->tag != 12)goto _LL754;else{_tmpF14=_tmpF13->f1;_tmpF15=_tmpF14.aggr_info;
if((_tmpF15.KnownAggr).tag != 2)goto _LL754;_tmpF16=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF15.KnownAggr).val;_tmpF17=_tmpF14.targs;}}_LL753: if((*_tmpF16)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF26=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpF16)->tvs,_tmpF17);struct Cyc_List_List*_tmpF27=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF16)->impl))->fields;void*t;
while(_tmpF27 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF26,((struct Cyc_Absyn_Aggrfield*)
_tmpF27->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF27=_tmpF27->tl;}return 0;}_LL754: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF18=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF10;if(_tmpF18->tag != 13)goto
_LL756;else{_tmpF19=_tmpF18->f2;}}_LL755: while(_tmpF19 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF19->hd)->type))return 1;_tmpF19=_tmpF19->tl;}
return 0;_LL756: {struct Cyc_Absyn_AggrType_struct*_tmpF1A=(struct Cyc_Absyn_AggrType_struct*)
_tmpF10;if(_tmpF1A->tag != 12)goto _LL758;else{_tmpF1B=_tmpF1A->f1;_tmpF1C=_tmpF1B.aggr_info;
if((_tmpF1C.UnknownAggr).tag != 1)goto _LL758;_tmpF1D=(struct _tuple4)(_tmpF1C.UnknownAggr).val;}}
_LL757: {const char*_tmp1629;void*_tmp1628;(_tmp1628=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1629="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1629,sizeof(char),36))),_tag_dyneither(_tmp1628,sizeof(void*),
0)));}_LL758: {struct Cyc_Absyn_DatatypeType_struct*_tmpF1E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpF10;if(_tmpF1E->tag != 3)goto _LL75A;else{_tmpF1F=_tmpF1E->f1;_tmpF20=_tmpF1F.datatype_info;
_tmpF21=_tmpF1F.targs;}}_LL759: {union Cyc_Absyn_DatatypeInfoU _tmpF2A=_tmpF20;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF2B;struct _tuple2*_tmpF2C;int _tmpF2D;
struct Cyc_Absyn_Datatypedecl**_tmpF2E;struct Cyc_Absyn_Datatypedecl*_tmpF2F;
struct Cyc_Absyn_Datatypedecl _tmpF30;struct Cyc_List_List*_tmpF31;struct Cyc_Core_Opt*
_tmpF32;_LL75F: if((_tmpF2A.UnknownDatatype).tag != 1)goto _LL761;_tmpF2B=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF2A.UnknownDatatype).val;_tmpF2C=_tmpF2B.name;
_tmpF2D=_tmpF2B.is_extensible;_LL760: {const char*_tmp162C;void*_tmp162B;(
_tmp162B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp162C="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp162C,
sizeof(char),40))),_tag_dyneither(_tmp162B,sizeof(void*),0)));}_LL761: if((
_tmpF2A.KnownDatatype).tag != 2)goto _LL75E;_tmpF2E=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF2A.KnownDatatype).val;_tmpF2F=*_tmpF2E;_tmpF30=*_tmpF2F;_tmpF31=_tmpF30.tvs;
_tmpF32=_tmpF30.fields;_LL762: return 0;_LL75E:;}_LL75A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF22=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpF10;if(_tmpF22->tag != 4)
goto _LL75C;else{_tmpF23=_tmpF22->f1;_tmpF24=_tmpF23.field_info;_tmpF25=_tmpF23.targs;}}
_LL75B: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF35=_tmpF24;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF36;struct _tuple3 _tmpF37;struct Cyc_Absyn_Datatypedecl*_tmpF38;struct Cyc_Absyn_Datatypefield*
_tmpF39;_LL764: if((_tmpF35.UnknownDatatypefield).tag != 1)goto _LL766;_tmpF36=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF35.UnknownDatatypefield).val;
_LL765: {const char*_tmp162F;void*_tmp162E;(_tmp162E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp162F="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp162F,sizeof(char),46))),_tag_dyneither(_tmp162E,sizeof(void*),
0)));}_LL766: if((_tmpF35.KnownDatatypefield).tag != 2)goto _LL763;_tmpF37=(struct
_tuple3)(_tmpF35.KnownDatatypefield).val;_tmpF38=_tmpF37.f1;_tmpF39=_tmpF37.f2;
_LL767: {struct Cyc_List_List*_tmpF3C=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF38->tvs,_tmpF25);struct Cyc_List_List*_tmpF3D=
_tmpF39->typs;while(_tmpF3D != 0){_tmpF0F=Cyc_Tcutil_rsubstitute(rgn,_tmpF3C,(*((
struct _tuple11*)_tmpF3D->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpF0F))return 1;_tmpF3D=_tmpF3D->tl;}return 0;}_LL763:;}_LL75C:;_LL75D: return 0;
_LL74F:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF40;union Cyc_Absyn_AggrInfoU _tmpF41;struct Cyc_Absyn_Aggrdecl**_tmpF42;struct
Cyc_Absyn_Aggrdecl*_tmpF43;struct Cyc_List_List*_tmpF44;struct Cyc_List_List*
_tmpF46;_LL769: {struct Cyc_Absyn_AggrType_struct*_tmpF3F=(struct Cyc_Absyn_AggrType_struct*)
_tmpF3E;if(_tmpF3F->tag != 12)goto _LL76B;else{_tmpF40=_tmpF3F->f1;_tmpF41=_tmpF40.aggr_info;
if((_tmpF41.KnownAggr).tag != 2)goto _LL76B;_tmpF42=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF41.KnownAggr).val;_tmpF43=*_tmpF42;_tmpF44=_tmpF40.targs;}}_LL76A: _tmpF46=
_tmpF43->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF43->impl))->fields;
goto _LL76C;_LL76B: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF45=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF3E;if(_tmpF45->tag != 13)goto _LL76D;else{_tmpF46=_tmpF45->f2;}}_LL76C: {
struct Cyc_Absyn_Aggrfield*_tmpF47=Cyc_Absyn_lookup_field(_tmpF46,f);{struct Cyc_Absyn_Aggrfield*
_tmpF48=_tmpF47;struct Cyc_Absyn_Aggrfield _tmpF49;void*_tmpF4A;_LL770: if(_tmpF48
!= 0)goto _LL772;_LL771: {const char*_tmp1632;void*_tmp1631;(_tmp1631=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1632="is_noalias_field: missing field",
_tag_dyneither(_tmp1632,sizeof(char),32))),_tag_dyneither(_tmp1631,sizeof(void*),
0)));}_LL772: if(_tmpF48 == 0)goto _LL76F;_tmpF49=*_tmpF48;_tmpF4A=_tmpF49.type;
_LL773: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF4A);_LL76F:;}return 0;}
_LL76D:;_LL76E: {const char*_tmp1636;void*_tmp1635[1];struct Cyc_String_pa_struct
_tmp1634;(_tmp1634.tag=0,((_tmp1634.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1635[0]=& _tmp1634,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1636="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1636,sizeof(char),36))),_tag_dyneither(_tmp1635,sizeof(void*),
1)))))));}_LL768:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF50=e->r;void*_tmpF52;struct Cyc_Absyn_Exp*
_tmpF55;struct Cyc_Absyn_Exp*_tmpF57;struct Cyc_Absyn_Exp*_tmpF59;struct
_dyneither_ptr*_tmpF5A;struct Cyc_Absyn_Exp*_tmpF5C;struct Cyc_Absyn_Exp*_tmpF5D;
struct Cyc_Absyn_Exp*_tmpF5F;struct Cyc_Absyn_Exp*_tmpF60;struct Cyc_Absyn_Exp*
_tmpF62;struct Cyc_Absyn_Exp*_tmpF64;struct Cyc_Absyn_Stmt*_tmpF66;_LL775: {struct
Cyc_Absyn_Var_e_struct*_tmpF51=(struct Cyc_Absyn_Var_e_struct*)_tmpF50;if(_tmpF51->tag
!= 1)goto _LL777;else{_tmpF52=(void*)_tmpF51->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF53=(struct Cyc_Absyn_Global_b_struct*)_tmpF52;if(_tmpF53->tag != 1)goto _LL777;};}}
_LL776: return 0;_LL777: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF54=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF50;if(_tmpF54->tag != 23)goto _LL779;else{_tmpF55=_tmpF54->f1;}}_LL778:
_tmpF57=_tmpF55;goto _LL77A;_LL779: {struct Cyc_Absyn_Deref_e_struct*_tmpF56=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF50;if(_tmpF56->tag != 21)goto _LL77B;else{
_tmpF57=_tmpF56->f1;}}_LL77A: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF57->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF57);_LL77B: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF58=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF50;if(_tmpF58->tag != 22)goto _LL77D;else{_tmpF59=_tmpF58->f1;_tmpF5A=_tmpF58->f2;}}
_LL77C: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF59);_LL77D: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF5B=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF50;if(_tmpF5B->tag != 24)goto
_LL77F;else{_tmpF5C=_tmpF5B->f1;_tmpF5D=_tmpF5B->f2;}}_LL77E: {void*_tmpF67=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF5C->topt))->v);_LL78A: {struct Cyc_Absyn_TupleType_struct*
_tmpF68=(struct Cyc_Absyn_TupleType_struct*)_tmpF67;if(_tmpF68->tag != 11)goto
_LL78C;}_LL78B: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF5C);_LL78C:;_LL78D:
return 0;_LL789:;}_LL77F: {struct Cyc_Absyn_Conditional_e_struct*_tmpF5E=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF50;if(_tmpF5E->tag != 6)goto _LL781;else{
_tmpF5F=_tmpF5E->f2;_tmpF60=_tmpF5E->f3;}}_LL780: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF5F) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF60);_LL781: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF61=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF50;if(_tmpF61->tag != 9)goto _LL783;
else{_tmpF62=_tmpF61->f2;}}_LL782: _tmpF64=_tmpF62;goto _LL784;_LL783: {struct Cyc_Absyn_Cast_e_struct*
_tmpF63=(struct Cyc_Absyn_Cast_e_struct*)_tmpF50;if(_tmpF63->tag != 15)goto _LL785;
else{_tmpF64=_tmpF63->f2;}}_LL784: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF64);
_LL785: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF65=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF50;if(_tmpF65->tag != 37)goto _LL787;else{_tmpF66=_tmpF65->f1;}}_LL786: while(
1){void*_tmpF69=_tmpF66->r;struct Cyc_Absyn_Stmt*_tmpF6B;struct Cyc_Absyn_Stmt*
_tmpF6C;struct Cyc_Absyn_Decl*_tmpF6E;struct Cyc_Absyn_Stmt*_tmpF6F;struct Cyc_Absyn_Exp*
_tmpF71;_LL78F: {struct Cyc_Absyn_Seq_s_struct*_tmpF6A=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF69;if(_tmpF6A->tag != 2)goto _LL791;else{_tmpF6B=_tmpF6A->f1;_tmpF6C=_tmpF6A->f2;}}
_LL790: _tmpF66=_tmpF6C;goto _LL78E;_LL791: {struct Cyc_Absyn_Decl_s_struct*_tmpF6D=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF69;if(_tmpF6D->tag != 12)goto _LL793;else{
_tmpF6E=_tmpF6D->f1;_tmpF6F=_tmpF6D->f2;}}_LL792: _tmpF66=_tmpF6F;goto _LL78E;
_LL793: {struct Cyc_Absyn_Exp_s_struct*_tmpF70=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF69;if(_tmpF70->tag != 1)goto _LL795;else{_tmpF71=_tmpF70->f1;}}_LL794: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF71);_LL795:;_LL796: {const char*_tmp1639;
void*_tmp1638;(_tmp1638=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1639="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1639,sizeof(char),40))),_tag_dyneither(_tmp1638,sizeof(void*),
0)));}_LL78E:;}_LL787:;_LL788: return 1;_LL774:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp163A;struct _tuple17 bogus_ans=(_tmp163A.f1=0,((_tmp163A.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp163A)));void*_tmpF74=e->r;struct _tuple2*_tmpF76;
void*_tmpF77;struct Cyc_Absyn_Exp*_tmpF79;struct _dyneither_ptr*_tmpF7A;int _tmpF7B;
struct Cyc_Absyn_Exp*_tmpF7D;struct _dyneither_ptr*_tmpF7E;int _tmpF7F;struct Cyc_Absyn_Exp*
_tmpF81;struct Cyc_Absyn_Exp*_tmpF83;struct Cyc_Absyn_Exp*_tmpF84;_LL798: {struct
Cyc_Absyn_Var_e_struct*_tmpF75=(struct Cyc_Absyn_Var_e_struct*)_tmpF74;if(_tmpF75->tag
!= 1)goto _LL79A;else{_tmpF76=_tmpF75->f1;_tmpF77=(void*)_tmpF75->f2;}}_LL799: {
void*_tmpF85=_tmpF77;struct Cyc_Absyn_Vardecl*_tmpF89;struct Cyc_Absyn_Vardecl*
_tmpF8B;struct Cyc_Absyn_Vardecl*_tmpF8D;struct Cyc_Absyn_Vardecl*_tmpF8F;_LL7A5: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF86=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF85;if(_tmpF86->tag != 0)goto _LL7A7;}_LL7A6: goto _LL7A8;_LL7A7: {struct Cyc_Absyn_Funname_b_struct*
_tmpF87=(struct Cyc_Absyn_Funname_b_struct*)_tmpF85;if(_tmpF87->tag != 2)goto
_LL7A9;}_LL7A8: return bogus_ans;_LL7A9: {struct Cyc_Absyn_Global_b_struct*_tmpF88=(
struct Cyc_Absyn_Global_b_struct*)_tmpF85;if(_tmpF88->tag != 1)goto _LL7AB;else{
_tmpF89=_tmpF88->f1;}}_LL7AA: {void*_tmpF90=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL7B2: {struct Cyc_Absyn_ArrayType_struct*
_tmpF91=(struct Cyc_Absyn_ArrayType_struct*)_tmpF90;if(_tmpF91->tag != 9)goto
_LL7B4;}_LL7B3: {struct _tuple17 _tmp163B;return(_tmp163B.f1=1,((_tmp163B.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp163B)));}_LL7B4:;_LL7B5: {struct _tuple17 _tmp163C;
return(_tmp163C.f1=(_tmpF89->tq).real_const,((_tmp163C.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp163C)));}_LL7B1:;}_LL7AB: {struct Cyc_Absyn_Local_b_struct*_tmpF8A=(struct Cyc_Absyn_Local_b_struct*)
_tmpF85;if(_tmpF8A->tag != 4)goto _LL7AD;else{_tmpF8B=_tmpF8A->f1;}}_LL7AC: {void*
_tmpF94=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL7B7: {struct Cyc_Absyn_ArrayType_struct*_tmpF95=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF94;if(_tmpF95->tag != 9)goto _LL7B9;}_LL7B8: {struct _tuple17 _tmp163D;return(
_tmp163D.f1=1,((_tmp163D.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF8B->rgn))->v,
_tmp163D)));}_LL7B9:;_LL7BA: _tmpF8B->escapes=1;{struct _tuple17 _tmp163E;return(
_tmp163E.f1=(_tmpF8B->tq).real_const,((_tmp163E.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8B->rgn))->v,_tmp163E)));};_LL7B6:;}_LL7AD: {struct Cyc_Absyn_Pat_b_struct*
_tmpF8C=(struct Cyc_Absyn_Pat_b_struct*)_tmpF85;if(_tmpF8C->tag != 5)goto _LL7AF;
else{_tmpF8D=_tmpF8C->f1;}}_LL7AE: _tmpF8F=_tmpF8D;goto _LL7B0;_LL7AF: {struct Cyc_Absyn_Param_b_struct*
_tmpF8E=(struct Cyc_Absyn_Param_b_struct*)_tmpF85;if(_tmpF8E->tag != 3)goto _LL7A4;
else{_tmpF8F=_tmpF8E->f1;}}_LL7B0: _tmpF8F->escapes=1;{struct _tuple17 _tmp163F;
return(_tmp163F.f1=(_tmpF8F->tq).real_const,((_tmp163F.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8F->rgn))->v,_tmp163F)));};_LL7A4:;}_LL79A: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF78=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF74;if(_tmpF78->tag != 22)goto
_LL79C;else{_tmpF79=_tmpF78->f1;_tmpF7A=_tmpF78->f2;_tmpF7B=_tmpF78->f3;}}_LL79B:
if(_tmpF7B)return bogus_ans;{void*_tmpF99=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF79->topt))->v);struct Cyc_List_List*_tmpF9B;struct Cyc_Absyn_AggrInfo
_tmpF9D;union Cyc_Absyn_AggrInfoU _tmpF9E;struct Cyc_Absyn_Aggrdecl**_tmpF9F;struct
Cyc_Absyn_Aggrdecl*_tmpFA0;_LL7BC: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF9A=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF99;if(_tmpF9A->tag != 13)goto _LL7BE;
else{_tmpF9B=_tmpF9A->f2;}}_LL7BD: {struct Cyc_Absyn_Aggrfield*_tmpFA1=Cyc_Absyn_lookup_field(
_tmpF9B,_tmpF7A);if(_tmpFA1 != 0  && _tmpFA1->width == 0){struct _tuple17 _tmp1640;
return(_tmp1640.f1=(_tmpFA1->tq).real_const,((_tmp1640.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF79)).f2,_tmp1640)));}return bogus_ans;}_LL7BE: {struct Cyc_Absyn_AggrType_struct*
_tmpF9C=(struct Cyc_Absyn_AggrType_struct*)_tmpF99;if(_tmpF9C->tag != 12)goto
_LL7C0;else{_tmpF9D=_tmpF9C->f1;_tmpF9E=_tmpF9D.aggr_info;if((_tmpF9E.KnownAggr).tag
!= 2)goto _LL7C0;_tmpF9F=(struct Cyc_Absyn_Aggrdecl**)(_tmpF9E.KnownAggr).val;
_tmpFA0=*_tmpF9F;}}_LL7BF: {struct Cyc_Absyn_Aggrfield*_tmpFA3=Cyc_Absyn_lookup_decl_field(
_tmpFA0,_tmpF7A);if(_tmpFA3 != 0  && _tmpFA3->width == 0){struct _tuple17 _tmp1641;
return(_tmp1641.f1=(_tmpFA3->tq).real_const,((_tmp1641.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF79)).f2,_tmp1641)));}return bogus_ans;}_LL7C0:;_LL7C1: return bogus_ans;
_LL7BB:;};_LL79C: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF7C=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF74;if(_tmpF7C->tag != 23)goto _LL79E;else{_tmpF7D=_tmpF7C->f1;_tmpF7E=_tmpF7C->f2;
_tmpF7F=_tmpF7C->f3;}}_LL79D: if(_tmpF7F)return bogus_ans;{void*_tmpFA5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF7D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpFA7;void*_tmpFA8;struct Cyc_Absyn_PtrAtts _tmpFA9;void*_tmpFAA;_LL7C3: {struct
Cyc_Absyn_PointerType_struct*_tmpFA6=(struct Cyc_Absyn_PointerType_struct*)
_tmpFA5;if(_tmpFA6->tag != 5)goto _LL7C5;else{_tmpFA7=_tmpFA6->f1;_tmpFA8=_tmpFA7.elt_typ;
_tmpFA9=_tmpFA7.ptr_atts;_tmpFAA=_tmpFA9.rgn;}}_LL7C4: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpFAB=Cyc_Tcutil_compress(_tmpFA8);struct Cyc_List_List*_tmpFAD;
struct Cyc_Absyn_AggrInfo _tmpFAF;union Cyc_Absyn_AggrInfoU _tmpFB0;struct Cyc_Absyn_Aggrdecl**
_tmpFB1;struct Cyc_Absyn_Aggrdecl*_tmpFB2;_LL7C8: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpFAC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFAB;if(_tmpFAC->tag != 13)goto
_LL7CA;else{_tmpFAD=_tmpFAC->f2;}}_LL7C9: finfo=Cyc_Absyn_lookup_field(_tmpFAD,
_tmpF7E);goto _LL7C7;_LL7CA: {struct Cyc_Absyn_AggrType_struct*_tmpFAE=(struct Cyc_Absyn_AggrType_struct*)
_tmpFAB;if(_tmpFAE->tag != 12)goto _LL7CC;else{_tmpFAF=_tmpFAE->f1;_tmpFB0=_tmpFAF.aggr_info;
if((_tmpFB0.KnownAggr).tag != 2)goto _LL7CC;_tmpFB1=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFB0.KnownAggr).val;_tmpFB2=*_tmpFB1;}}_LL7CB: finfo=Cyc_Absyn_lookup_decl_field(
_tmpFB2,_tmpF7E);goto _LL7C7;_LL7CC:;_LL7CD: return bogus_ans;_LL7C7:;}if(finfo != 0
 && finfo->width == 0){struct _tuple17 _tmp1642;return(_tmp1642.f1=(finfo->tq).real_const,((
_tmp1642.f2=_tmpFAA,_tmp1642)));}return bogus_ans;}_LL7C5:;_LL7C6: return bogus_ans;
_LL7C2:;};_LL79E: {struct Cyc_Absyn_Deref_e_struct*_tmpF80=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF74;if(_tmpF80->tag != 21)goto _LL7A0;else{_tmpF81=_tmpF80->f1;}}_LL79F: {void*
_tmpFB4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF81->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpFB6;struct Cyc_Absyn_Tqual _tmpFB7;struct Cyc_Absyn_PtrAtts
_tmpFB8;void*_tmpFB9;_LL7CF: {struct Cyc_Absyn_PointerType_struct*_tmpFB5=(struct
Cyc_Absyn_PointerType_struct*)_tmpFB4;if(_tmpFB5->tag != 5)goto _LL7D1;else{
_tmpFB6=_tmpFB5->f1;_tmpFB7=_tmpFB6.elt_tq;_tmpFB8=_tmpFB6.ptr_atts;_tmpFB9=
_tmpFB8.rgn;}}_LL7D0: {struct _tuple17 _tmp1643;return(_tmp1643.f1=_tmpFB7.real_const,((
_tmp1643.f2=_tmpFB9,_tmp1643)));}_LL7D1:;_LL7D2: return bogus_ans;_LL7CE:;}_LL7A0: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF82=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF74;if(_tmpF82->tag != 24)goto _LL7A2;else{_tmpF83=_tmpF82->f1;_tmpF84=_tmpF82->f2;}}
_LL7A1: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF83->topt))->v);void*_tmpFBB=t;struct Cyc_List_List*_tmpFBD;struct Cyc_Absyn_PtrInfo
_tmpFBF;struct Cyc_Absyn_Tqual _tmpFC0;struct Cyc_Absyn_PtrAtts _tmpFC1;void*_tmpFC2;
struct Cyc_Absyn_ArrayInfo _tmpFC4;struct Cyc_Absyn_Tqual _tmpFC5;_LL7D4: {struct Cyc_Absyn_TupleType_struct*
_tmpFBC=(struct Cyc_Absyn_TupleType_struct*)_tmpFBB;if(_tmpFBC->tag != 11)goto
_LL7D6;else{_tmpFBD=_tmpFBC->f1;}}_LL7D5: {unsigned int _tmpFC7;int _tmpFC8;struct
_tuple13 _tmpFC6=Cyc_Evexp_eval_const_uint_exp(_tmpF84);_tmpFC7=_tmpFC6.f1;
_tmpFC8=_tmpFC6.f2;if(!_tmpFC8)return bogus_ans;{struct _tuple11*_tmpFC9=Cyc_Absyn_lookup_tuple_field(
_tmpFBD,(int)_tmpFC7);if(_tmpFC9 != 0){struct _tuple17 _tmp1644;return(_tmp1644.f1=((*
_tmpFC9).f1).real_const,((_tmp1644.f2=(Cyc_Tcutil_addressof_props(te,_tmpF83)).f2,
_tmp1644)));}return bogus_ans;};}_LL7D6: {struct Cyc_Absyn_PointerType_struct*
_tmpFBE=(struct Cyc_Absyn_PointerType_struct*)_tmpFBB;if(_tmpFBE->tag != 5)goto
_LL7D8;else{_tmpFBF=_tmpFBE->f1;_tmpFC0=_tmpFBF.elt_tq;_tmpFC1=_tmpFBF.ptr_atts;
_tmpFC2=_tmpFC1.rgn;}}_LL7D7: {struct _tuple17 _tmp1645;return(_tmp1645.f1=_tmpFC0.real_const,((
_tmp1645.f2=_tmpFC2,_tmp1645)));}_LL7D8: {struct Cyc_Absyn_ArrayType_struct*
_tmpFC3=(struct Cyc_Absyn_ArrayType_struct*)_tmpFBB;if(_tmpFC3->tag != 9)goto
_LL7DA;else{_tmpFC4=_tmpFC3->f1;_tmpFC5=_tmpFC4.tq;}}_LL7D9: {struct _tuple17
_tmp1646;return(_tmp1646.f1=_tmpFC5.real_const,((_tmp1646.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF83)).f2,_tmp1646)));}_LL7DA:;_LL7DB: return bogus_ans;_LL7D3:;}_LL7A2:;
_LL7A3:{const char*_tmp1649;void*_tmp1648;(_tmp1648=0,Cyc_Tcutil_terr(e->loc,((
_tmp1649="unary & applied to non-lvalue",_tag_dyneither(_tmp1649,sizeof(char),30))),
_tag_dyneither(_tmp1648,sizeof(void*),0)));}return bogus_ans;_LL797:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFD0=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFD2;void*_tmpFD3;struct Cyc_Absyn_Tqual
_tmpFD4;union Cyc_Absyn_Constraint*_tmpFD5;_LL7DD: {struct Cyc_Absyn_ArrayType_struct*
_tmpFD1=(struct Cyc_Absyn_ArrayType_struct*)_tmpFD0;if(_tmpFD1->tag != 9)goto
_LL7DF;else{_tmpFD2=_tmpFD1->f1;_tmpFD3=_tmpFD2.elt_type;_tmpFD4=_tmpFD2.tq;
_tmpFD5=_tmpFD2.zero_term;}}_LL7DE: {void*_tmpFD7;struct _tuple17 _tmpFD6=Cyc_Tcutil_addressof_props(
te,e);_tmpFD7=_tmpFD6.f2;{struct Cyc_Absyn_Upper_b_struct _tmp164C;struct Cyc_Absyn_Upper_b_struct*
_tmp164B;return Cyc_Absyn_atb_typ(_tmpFD3,_tmpFD7,_tmpFD4,(void*)((_tmp164B=
_cycalloc(sizeof(*_tmp164B)),((_tmp164B[0]=((_tmp164C.tag=1,((_tmp164C.f1=e,
_tmp164C)))),_tmp164B)))),_tmpFD5);};}_LL7DF:;_LL7E0: return e_typ;_LL7DC:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFDA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFDD;_LL7E2: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFDB=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFDA;if(_tmpFDB->tag != 0)goto
_LL7E4;}_LL7E3: return;_LL7E4: {struct Cyc_Absyn_Upper_b_struct*_tmpFDC=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFDA;if(_tmpFDC->tag != 1)goto _LL7E1;else{_tmpFDD=_tmpFDC->f1;}}_LL7E5: {
unsigned int _tmpFDF;int _tmpFE0;struct _tuple13 _tmpFDE=Cyc_Evexp_eval_const_uint_exp(
_tmpFDD);_tmpFDF=_tmpFDE.f1;_tmpFE0=_tmpFDE.f2;if(_tmpFE0  && _tmpFDF <= i){const
char*_tmp1651;void*_tmp1650[2];struct Cyc_Int_pa_struct _tmp164F;struct Cyc_Int_pa_struct
_tmp164E;(_tmp164E.tag=1,((_tmp164E.f1=(unsigned long)((int)i),((_tmp164F.tag=1,((
_tmp164F.f1=(unsigned long)((int)_tmpFDF),((_tmp1650[0]=& _tmp164F,((_tmp1650[1]=&
_tmp164E,Cyc_Tcutil_terr(loc,((_tmp1651="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1651,sizeof(char),39))),_tag_dyneither(_tmp1650,sizeof(void*),
2)))))))))))));}return;}_LL7E1:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFE5=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFE7;_LL7E7: {struct Cyc_Absyn_Upper_b_struct*_tmpFE6=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFE5;if(_tmpFE6->tag != 1)goto _LL7E9;else{_tmpFE7=_tmpFE6->f1;}}_LL7E8: {
unsigned int _tmpFE9;int _tmpFEA;struct _tuple13 _tmpFE8=Cyc_Evexp_eval_const_uint_exp(
_tmpFE7);_tmpFE9=_tmpFE8.f1;_tmpFEA=_tmpFE8.f2;return _tmpFEA  && _tmpFE9 == 1;}
_LL7E9:;_LL7EA: return 0;_LL7E6:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFEB=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFF3;void*_tmpFF4;union Cyc_Absyn_Constraint*
_tmpFF5;struct Cyc_List_List*_tmpFF7;struct Cyc_Absyn_AggrInfo _tmpFF9;union Cyc_Absyn_AggrInfoU
_tmpFFA;struct _tuple4 _tmpFFB;struct Cyc_Absyn_AggrInfo _tmpFFD;union Cyc_Absyn_AggrInfoU
_tmpFFE;struct Cyc_Absyn_Aggrdecl**_tmpFFF;struct Cyc_Absyn_Aggrdecl*_tmp1000;
struct Cyc_List_List*_tmp1001;struct Cyc_List_List*_tmp1003;_LL7EC: {struct Cyc_Absyn_VoidType_struct*
_tmpFEC=(struct Cyc_Absyn_VoidType_struct*)_tmpFEB;if(_tmpFEC->tag != 0)goto _LL7EE;}
_LL7ED: goto _LL7EF;_LL7EE: {struct Cyc_Absyn_IntType_struct*_tmpFED=(struct Cyc_Absyn_IntType_struct*)
_tmpFEB;if(_tmpFED->tag != 6)goto _LL7F0;}_LL7EF: goto _LL7F1;_LL7F0: {struct Cyc_Absyn_FloatType_struct*
_tmpFEE=(struct Cyc_Absyn_FloatType_struct*)_tmpFEB;if(_tmpFEE->tag != 7)goto
_LL7F2;}_LL7F1: goto _LL7F3;_LL7F2: {struct Cyc_Absyn_DoubleType_struct*_tmpFEF=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFEB;if(_tmpFEF->tag != 8)goto _LL7F4;}
_LL7F3: return 1;_LL7F4: {struct Cyc_Absyn_EnumType_struct*_tmpFF0=(struct Cyc_Absyn_EnumType_struct*)
_tmpFEB;if(_tmpFF0->tag != 14)goto _LL7F6;}_LL7F5: goto _LL7F7;_LL7F6: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFF1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpFEB;if(_tmpFF1->tag != 15)goto
_LL7F8;}_LL7F7: return 0;_LL7F8: {struct Cyc_Absyn_ArrayType_struct*_tmpFF2=(struct
Cyc_Absyn_ArrayType_struct*)_tmpFEB;if(_tmpFF2->tag != 9)goto _LL7FA;else{_tmpFF3=
_tmpFF2->f1;_tmpFF4=_tmpFF3.elt_type;_tmpFF5=_tmpFF3.zero_term;}}_LL7F9: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFF5) && Cyc_Tcutil_bits_only(
_tmpFF4);_LL7FA: {struct Cyc_Absyn_TupleType_struct*_tmpFF6=(struct Cyc_Absyn_TupleType_struct*)
_tmpFEB;if(_tmpFF6->tag != 11)goto _LL7FC;else{_tmpFF7=_tmpFF6->f1;}}_LL7FB: for(0;
_tmpFF7 != 0;_tmpFF7=_tmpFF7->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpFF7->hd)).f2))return 0;}return 1;_LL7FC: {struct Cyc_Absyn_AggrType_struct*
_tmpFF8=(struct Cyc_Absyn_AggrType_struct*)_tmpFEB;if(_tmpFF8->tag != 12)goto
_LL7FE;else{_tmpFF9=_tmpFF8->f1;_tmpFFA=_tmpFF9.aggr_info;if((_tmpFFA.UnknownAggr).tag
!= 1)goto _LL7FE;_tmpFFB=(struct _tuple4)(_tmpFFA.UnknownAggr).val;}}_LL7FD: return
0;_LL7FE: {struct Cyc_Absyn_AggrType_struct*_tmpFFC=(struct Cyc_Absyn_AggrType_struct*)
_tmpFEB;if(_tmpFFC->tag != 12)goto _LL800;else{_tmpFFD=_tmpFFC->f1;_tmpFFE=_tmpFFD.aggr_info;
if((_tmpFFE.KnownAggr).tag != 2)goto _LL800;_tmpFFF=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFFE.KnownAggr).val;_tmp1000=*_tmpFFF;_tmp1001=_tmpFFD.targs;}}_LL7FF: if(
_tmp1000->impl == 0)return 0;{struct _RegionHandle _tmp1004=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp1004;_push_region(rgn);{struct Cyc_List_List*
_tmp1005=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1000->tvs,
_tmp1001);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1000->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(
rgn,_tmp1005,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp1006=0;
_npop_handler(0);return _tmp1006;}}}{int _tmp1007=1;_npop_handler(0);return
_tmp1007;};};_pop_region(rgn);};_LL800: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1002=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFEB;if(_tmp1002->tag != 13)
goto _LL802;else{_tmp1003=_tmp1002->f2;}}_LL801: for(0;_tmp1003 != 0;_tmp1003=
_tmp1003->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmp1003->hd)->type))
return 0;}return 1;_LL802:;_LL803: return 0;_LL7EB:;}static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp1008=e->r;
struct _tuple2*_tmp1010;void*_tmp1011;struct Cyc_Absyn_Exp*_tmp1013;struct Cyc_Absyn_Exp*
_tmp1014;struct Cyc_Absyn_Exp*_tmp1015;struct Cyc_Absyn_Exp*_tmp1017;struct Cyc_Absyn_Exp*
_tmp1018;struct Cyc_Absyn_Exp*_tmp101A;struct Cyc_Absyn_Exp*_tmp101C;void*_tmp101E;
struct Cyc_Absyn_Exp*_tmp101F;enum Cyc_Absyn_Coercion _tmp1020;void*_tmp1022;
struct Cyc_Absyn_Exp*_tmp1023;struct Cyc_Absyn_Exp*_tmp1025;struct Cyc_Absyn_Exp*
_tmp1027;struct Cyc_Absyn_Exp*_tmp1028;struct Cyc_List_List*_tmp102A;struct Cyc_List_List*
_tmp102C;struct Cyc_List_List*_tmp102E;enum Cyc_Absyn_Primop _tmp1030;struct Cyc_List_List*
_tmp1031;struct Cyc_List_List*_tmp1033;struct Cyc_List_List*_tmp1035;_LL805: {
struct Cyc_Absyn_Const_e_struct*_tmp1009=(struct Cyc_Absyn_Const_e_struct*)
_tmp1008;if(_tmp1009->tag != 0)goto _LL807;}_LL806: goto _LL808;_LL807: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp100A=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1008;if(_tmp100A->tag != 18)
goto _LL809;}_LL808: goto _LL80A;_LL809: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp100B=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1008;if(_tmp100B->tag != 19)
goto _LL80B;}_LL80A: goto _LL80C;_LL80B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp100C=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp1008;if(_tmp100C->tag != 20)goto
_LL80D;}_LL80C: goto _LL80E;_LL80D: {struct Cyc_Absyn_Enum_e_struct*_tmp100D=(
struct Cyc_Absyn_Enum_e_struct*)_tmp1008;if(_tmp100D->tag != 32)goto _LL80F;}_LL80E:
goto _LL810;_LL80F: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp100E=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp1008;if(_tmp100E->tag != 33)goto _LL811;}_LL810: return 1;_LL811: {struct Cyc_Absyn_Var_e_struct*
_tmp100F=(struct Cyc_Absyn_Var_e_struct*)_tmp1008;if(_tmp100F->tag != 1)goto _LL813;
else{_tmp1010=_tmp100F->f1;_tmp1011=(void*)_tmp100F->f2;}}_LL812: {void*_tmp1036=
_tmp1011;struct Cyc_Absyn_Vardecl*_tmp1039;_LL832: {struct Cyc_Absyn_Funname_b_struct*
_tmp1037=(struct Cyc_Absyn_Funname_b_struct*)_tmp1036;if(_tmp1037->tag != 2)goto
_LL834;}_LL833: return 1;_LL834: {struct Cyc_Absyn_Global_b_struct*_tmp1038=(struct
Cyc_Absyn_Global_b_struct*)_tmp1036;if(_tmp1038->tag != 1)goto _LL836;else{
_tmp1039=_tmp1038->f1;}}_LL835: {void*_tmp103B=Cyc_Tcutil_compress(_tmp1039->type);
_LL83B: {struct Cyc_Absyn_ArrayType_struct*_tmp103C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp103B;if(_tmp103C->tag != 9)goto _LL83D;}_LL83C: goto _LL83E;_LL83D: {struct Cyc_Absyn_FnType_struct*
_tmp103D=(struct Cyc_Absyn_FnType_struct*)_tmp103B;if(_tmp103D->tag != 10)goto
_LL83F;}_LL83E: return 1;_LL83F:;_LL840: return var_okay;_LL83A:;}_LL836: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp103A=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp1036;if(_tmp103A->tag != 0)
goto _LL838;}_LL837: return 0;_LL838:;_LL839: return var_okay;_LL831:;}_LL813: {
struct Cyc_Absyn_Conditional_e_struct*_tmp1012=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp1008;if(_tmp1012->tag != 6)goto _LL815;else{_tmp1013=_tmp1012->f1;_tmp1014=
_tmp1012->f2;_tmp1015=_tmp1012->f3;}}_LL814: return(Cyc_Tcutil_cnst_exp(te,0,
_tmp1013) && Cyc_Tcutil_cnst_exp(te,0,_tmp1014)) && Cyc_Tcutil_cnst_exp(te,0,
_tmp1015);_LL815: {struct Cyc_Absyn_SeqExp_e_struct*_tmp1016=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1008;if(_tmp1016->tag != 9)goto _LL817;else{_tmp1017=_tmp1016->f1;_tmp1018=
_tmp1016->f2;}}_LL816: return Cyc_Tcutil_cnst_exp(te,0,_tmp1017) && Cyc_Tcutil_cnst_exp(
te,0,_tmp1018);_LL817: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp1019=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp1008;if(_tmp1019->tag != 13)goto _LL819;
else{_tmp101A=_tmp1019->f1;}}_LL818: _tmp101C=_tmp101A;goto _LL81A;_LL819: {struct
Cyc_Absyn_Instantiate_e_struct*_tmp101B=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1008;if(_tmp101B->tag != 14)goto _LL81B;else{_tmp101C=_tmp101B->f1;}}_LL81A:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp101C);_LL81B: {struct Cyc_Absyn_Cast_e_struct*
_tmp101D=(struct Cyc_Absyn_Cast_e_struct*)_tmp1008;if(_tmp101D->tag != 15)goto
_LL81D;else{_tmp101E=(void*)_tmp101D->f1;_tmp101F=_tmp101D->f2;_tmp1020=_tmp101D->f4;
if(_tmp1020 != Cyc_Absyn_No_coercion)goto _LL81D;}}_LL81C: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp101F);_LL81D: {struct Cyc_Absyn_Cast_e_struct*_tmp1021=(struct Cyc_Absyn_Cast_e_struct*)
_tmp1008;if(_tmp1021->tag != 15)goto _LL81F;else{_tmp1022=(void*)_tmp1021->f1;
_tmp1023=_tmp1021->f2;}}_LL81E: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1023);
_LL81F: {struct Cyc_Absyn_Address_e_struct*_tmp1024=(struct Cyc_Absyn_Address_e_struct*)
_tmp1008;if(_tmp1024->tag != 16)goto _LL821;else{_tmp1025=_tmp1024->f1;}}_LL820:
return Cyc_Tcutil_cnst_exp(te,1,_tmp1025);_LL821: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1026=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp1008;if(_tmp1026->tag != 28)
goto _LL823;else{_tmp1027=_tmp1026->f2;_tmp1028=_tmp1026->f3;}}_LL822: return Cyc_Tcutil_cnst_exp(
te,0,_tmp1027) && Cyc_Tcutil_cnst_exp(te,0,_tmp1028);_LL823: {struct Cyc_Absyn_Array_e_struct*
_tmp1029=(struct Cyc_Absyn_Array_e_struct*)_tmp1008;if(_tmp1029->tag != 27)goto
_LL825;else{_tmp102A=_tmp1029->f1;}}_LL824: _tmp102C=_tmp102A;goto _LL826;_LL825: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp102B=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1008;if(_tmp102B->tag != 30)goto _LL827;else{_tmp102C=_tmp102B->f2;}}_LL826:
_tmp102E=_tmp102C;goto _LL828;_LL827: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp102D=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp1008;if(_tmp102D->tag != 29)
goto _LL829;else{_tmp102E=_tmp102D->f3;}}_LL828: for(0;_tmp102E != 0;_tmp102E=
_tmp102E->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)_tmp102E->hd)).f2))
return 0;}return 1;_LL829: {struct Cyc_Absyn_Primop_e_struct*_tmp102F=(struct Cyc_Absyn_Primop_e_struct*)
_tmp1008;if(_tmp102F->tag != 3)goto _LL82B;else{_tmp1030=_tmp102F->f1;_tmp1031=
_tmp102F->f2;}}_LL82A: _tmp1033=_tmp1031;goto _LL82C;_LL82B: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1032=(struct Cyc_Absyn_Tuple_e_struct*)_tmp1008;if(_tmp1032->tag != 25)goto
_LL82D;else{_tmp1033=_tmp1032->f1;}}_LL82C: _tmp1035=_tmp1033;goto _LL82E;_LL82D: {
struct Cyc_Absyn_Datatype_e_struct*_tmp1034=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp1008;if(_tmp1034->tag != 31)goto _LL82F;else{_tmp1035=_tmp1034->f1;}}_LL82E:
for(0;_tmp1035 != 0;_tmp1035=_tmp1035->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp1035->hd))return 0;}return 1;_LL82F:;_LL830: return 0;_LL804:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp103E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1044;
struct Cyc_Absyn_PtrAtts _tmp1045;union Cyc_Absyn_Constraint*_tmp1046;union Cyc_Absyn_Constraint*
_tmp1047;struct Cyc_Absyn_ArrayInfo _tmp1049;void*_tmp104A;struct Cyc_List_List*
_tmp104C;struct Cyc_Absyn_AggrInfo _tmp104E;union Cyc_Absyn_AggrInfoU _tmp104F;
struct Cyc_List_List*_tmp1050;struct Cyc_List_List*_tmp1052;_LL842: {struct Cyc_Absyn_VoidType_struct*
_tmp103F=(struct Cyc_Absyn_VoidType_struct*)_tmp103E;if(_tmp103F->tag != 0)goto
_LL844;}_LL843: goto _LL845;_LL844: {struct Cyc_Absyn_IntType_struct*_tmp1040=(
struct Cyc_Absyn_IntType_struct*)_tmp103E;if(_tmp1040->tag != 6)goto _LL846;}_LL845:
goto _LL847;_LL846: {struct Cyc_Absyn_FloatType_struct*_tmp1041=(struct Cyc_Absyn_FloatType_struct*)
_tmp103E;if(_tmp1041->tag != 7)goto _LL848;}_LL847: goto _LL849;_LL848: {struct Cyc_Absyn_DoubleType_struct*
_tmp1042=(struct Cyc_Absyn_DoubleType_struct*)_tmp103E;if(_tmp1042->tag != 8)goto
_LL84A;}_LL849: return 1;_LL84A: {struct Cyc_Absyn_PointerType_struct*_tmp1043=(
struct Cyc_Absyn_PointerType_struct*)_tmp103E;if(_tmp1043->tag != 5)goto _LL84C;
else{_tmp1044=_tmp1043->f1;_tmp1045=_tmp1044.ptr_atts;_tmp1046=_tmp1045.nullable;
_tmp1047=_tmp1045.bounds;}}_LL84B: {void*_tmp1055=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1047);_LL85B: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1056=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1055;if(_tmp1056->tag != 0)goto _LL85D;}_LL85C: return 1;_LL85D:;_LL85E: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp1046);
_LL85A:;}_LL84C: {struct Cyc_Absyn_ArrayType_struct*_tmp1048=(struct Cyc_Absyn_ArrayType_struct*)
_tmp103E;if(_tmp1048->tag != 9)goto _LL84E;else{_tmp1049=_tmp1048->f1;_tmp104A=
_tmp1049.elt_type;}}_LL84D: return Cyc_Tcutil_supports_default(_tmp104A);_LL84E: {
struct Cyc_Absyn_TupleType_struct*_tmp104B=(struct Cyc_Absyn_TupleType_struct*)
_tmp103E;if(_tmp104B->tag != 11)goto _LL850;else{_tmp104C=_tmp104B->f1;}}_LL84F:
for(0;_tmp104C != 0;_tmp104C=_tmp104C->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple11*)_tmp104C->hd)).f2))return 0;}return 1;_LL850: {struct Cyc_Absyn_AggrType_struct*
_tmp104D=(struct Cyc_Absyn_AggrType_struct*)_tmp103E;if(_tmp104D->tag != 12)goto
_LL852;else{_tmp104E=_tmp104D->f1;_tmp104F=_tmp104E.aggr_info;_tmp1050=_tmp104E.targs;}}
_LL851: {struct Cyc_Absyn_Aggrdecl*_tmp1057=Cyc_Absyn_get_known_aggrdecl(_tmp104F);
if(_tmp1057->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1057->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp1057->tvs,_tmp1050,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1057->impl))->fields);}
_LL852: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1051=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp103E;if(_tmp1051->tag != 13)goto _LL854;else{_tmp1052=_tmp1051->f2;}}_LL853:
return Cyc_Tcutil_fields_support_default(0,0,_tmp1052);_LL854: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1053=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp103E;if(_tmp1053->tag != 15)
goto _LL856;}_LL855: goto _LL857;_LL856: {struct Cyc_Absyn_EnumType_struct*_tmp1054=(
struct Cyc_Absyn_EnumType_struct*)_tmp103E;if(_tmp1054->tag != 14)goto _LL858;}
_LL857: return 1;_LL858:;_LL859: return 0;_LL841:;}void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t){void*_tmp1058=t;struct Cyc_Absyn_Typedefdecl*
_tmp105A;_LL860: {struct Cyc_Absyn_TypedefType_struct*_tmp1059=(struct Cyc_Absyn_TypedefType_struct*)
_tmp1058;if(_tmp1059->tag != 18)goto _LL862;else{_tmp105A=_tmp1059->f3;}}_LL861:
if(_tmp105A != 0){struct Cyc_Absyn_Tqual _tmp105B=_tmp105A->tq;if(((_tmp105B.print_const
 || _tmp105B.q_volatile) || _tmp105B.q_restrict) || _tmp105B.real_const){const
char*_tmp1655;void*_tmp1654[1];struct Cyc_String_pa_struct _tmp1653;(_tmp1653.tag=
0,((_tmp1653.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1654[0]=& _tmp1653,Cyc_Tcutil_warn(loc,((_tmp1655="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp1655,sizeof(char),44))),_tag_dyneither(_tmp1654,sizeof(void*),
1)))))));}}goto _LL85F;_LL862:;_LL863: goto _LL85F;_LL85F:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp105F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp105F;
_push_region(rgn);{struct Cyc_List_List*_tmp1060=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp1060,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp1061=0;_npop_handler(0);return _tmp1061;}}}{int _tmp1062=1;
_npop_handler(0);return _tmp1062;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp1063=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1068;
struct Cyc_Absyn_PtrAtts _tmp1069;union Cyc_Absyn_Constraint*_tmp106A;union Cyc_Absyn_Constraint*
_tmp106B;_LL865: {struct Cyc_Absyn_IntType_struct*_tmp1064=(struct Cyc_Absyn_IntType_struct*)
_tmp1063;if(_tmp1064->tag != 6)goto _LL867;}_LL866: goto _LL868;_LL867: {struct Cyc_Absyn_FloatType_struct*
_tmp1065=(struct Cyc_Absyn_FloatType_struct*)_tmp1063;if(_tmp1065->tag != 7)goto
_LL869;}_LL868: goto _LL86A;_LL869: {struct Cyc_Absyn_DoubleType_struct*_tmp1066=(
struct Cyc_Absyn_DoubleType_struct*)_tmp1063;if(_tmp1066->tag != 8)goto _LL86B;}
_LL86A: return 1;_LL86B: {struct Cyc_Absyn_PointerType_struct*_tmp1067=(struct Cyc_Absyn_PointerType_struct*)
_tmp1063;if(_tmp1067->tag != 5)goto _LL86D;else{_tmp1068=_tmp1067->f1;_tmp1069=
_tmp1068.ptr_atts;_tmp106A=_tmp1069.nullable;_tmp106B=_tmp1069.bounds;}}_LL86C: {
void*_tmp106C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp106B);_LL870: {struct Cyc_Absyn_DynEither_b_struct*
_tmp106D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp106C;if(_tmp106D->tag != 0)goto
_LL872;}_LL871: return 0;_LL872:;_LL873: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp106A);_LL86F:;}_LL86D:;_LL86E: return 0;_LL864:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp106E=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp1070;void*_tmp1071;struct Cyc_Absyn_FnInfo _tmp1073;struct Cyc_List_List*
_tmp1074;_LL875: {struct Cyc_Absyn_PointerType_struct*_tmp106F=(struct Cyc_Absyn_PointerType_struct*)
_tmp106E;if(_tmp106F->tag != 5)goto _LL877;else{_tmp1070=_tmp106F->f1;_tmp1071=
_tmp1070.elt_typ;}}_LL876: return Cyc_Tcutil_is_noreturn(_tmp1071);_LL877: {struct
Cyc_Absyn_FnType_struct*_tmp1072=(struct Cyc_Absyn_FnType_struct*)_tmp106E;if(
_tmp1072->tag != 10)goto _LL879;else{_tmp1073=_tmp1072->f1;_tmp1074=_tmp1073.attributes;}}
_LL878: for(0;_tmp1074 != 0;_tmp1074=_tmp1074->tl){void*_tmp1075=(void*)_tmp1074->hd;
_LL87C: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1076=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1075;if(_tmp1076->tag != 4)goto _LL87E;}_LL87D: return 1;_LL87E:;_LL87F:
continue;_LL87B:;}goto _LL874;_LL879:;_LL87A: goto _LL874;_LL874:;}return 0;}struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp1077=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp1079;struct Cyc_List_List*
_tmp107A;struct Cyc_List_List**_tmp107B;_LL881: {struct Cyc_Absyn_FnType_struct*
_tmp1078=(struct Cyc_Absyn_FnType_struct*)_tmp1077;if(_tmp1078->tag != 10)goto
_LL883;else{_tmp1079=_tmp1078->f1;_tmp107A=_tmp1079.attributes;_tmp107B=(struct
Cyc_List_List**)&(_tmp1078->f1).attributes;}}_LL882: {struct Cyc_List_List*
_tmp107C=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1656;*_tmp107B=((_tmp1656=_cycalloc(sizeof(*_tmp1656)),((
_tmp1656->hd=(void*)atts->hd,((_tmp1656->tl=*_tmp107B,_tmp1656))))));}else{
struct Cyc_List_List*_tmp1657;_tmp107C=((_tmp1657=_cycalloc(sizeof(*_tmp1657)),((
_tmp1657->hd=(void*)atts->hd,((_tmp1657->tl=_tmp107C,_tmp1657))))));}}return
_tmp107C;}_LL883:;_LL884: {const char*_tmp165A;void*_tmp1659;(_tmp1659=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp165A="transfer_fn_type_atts",
_tag_dyneither(_tmp165A,sizeof(char),22))),_tag_dyneither(_tmp1659,sizeof(void*),
0)));}_LL880:;}
