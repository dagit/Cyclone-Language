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
_tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct
_tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct
_tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{
int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};
struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _union_Cnst_Float_c{
int tag;struct _dyneither_ptr val;};struct _union_Cnst_String_c{int tag;struct
_dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};
union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c
Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum 
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
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp108D;void*_tmp108C[2];const char*
_tmp108B;const char*_tmp108A;struct Cyc_String_pa_struct _tmp1089;struct Cyc_String_pa_struct
_tmp1088;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp1088.tag=0,((
_tmp1088.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp1089.tag=0,((_tmp1089.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp108A="const ",_tag_dyneither(_tmp108A,sizeof(char),7)):((
_tmp108B="",_tag_dyneither(_tmp108B,sizeof(char),1)))),((_tmp108C[0]=& _tmp1089,((
_tmp108C[1]=& _tmp1088,Cyc_aprintf(((_tmp108D="%s%s",_tag_dyneither(_tmp108D,
sizeof(char),5))),_tag_dyneither(_tmp108C,sizeof(void*),2))))))))))))));const
char*_tmp1096;void*_tmp1095[2];const char*_tmp1094;const char*_tmp1093;struct Cyc_String_pa_struct
_tmp1092;struct Cyc_String_pa_struct _tmp1091;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp1091.tag=0,((_tmp1091.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1092.tag=0,((
_tmp1092.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp1093="const ",
_tag_dyneither(_tmp1093,sizeof(char),7)):((_tmp1094="",_tag_dyneither(_tmp1094,
sizeof(char),1)))),((_tmp1095[0]=& _tmp1092,((_tmp1095[1]=& _tmp1091,Cyc_aprintf(((
_tmp1096="%s%s",_tag_dyneither(_tmp1096,sizeof(char),5))),_tag_dyneither(
_tmp1095,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp109A;void*
_tmp1099[1];struct Cyc_String_pa_struct _tmp1098;(_tmp1098.tag=0,((_tmp1098.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1099[0]=& _tmp1098,Cyc_fprintf(
Cyc_stderr,((_tmp109A="  %s",_tag_dyneither(_tmp109A,sizeof(char),5))),
_tag_dyneither(_tmp1099,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp109D;void*_tmp109C;(_tmp109C=0,Cyc_fprintf(
Cyc_stderr,((_tmp109D="\n\t",_tag_dyneither(_tmp109D,sizeof(char),3))),
_tag_dyneither(_tmp109C,sizeof(void*),0)));}pos=8;}else{{const char*_tmp10A0;void*
_tmp109F;(_tmp109F=0,Cyc_fprintf(Cyc_stderr,((_tmp10A0=" ",_tag_dyneither(
_tmp10A0,sizeof(char),2))),_tag_dyneither(_tmp109F,sizeof(void*),0)));}++ pos;}{
const char*_tmp10A3;void*_tmp10A2;(_tmp10A2=0,Cyc_fprintf(Cyc_stderr,((_tmp10A3="and ",
_tag_dyneither(_tmp10A3,sizeof(char),5))),_tag_dyneither(_tmp10A2,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp10A6;void*_tmp10A5;(_tmp10A5=0,Cyc_fprintf(Cyc_stderr,((_tmp10A6="\n\t",
_tag_dyneither(_tmp10A6,sizeof(char),3))),_tag_dyneither(_tmp10A5,sizeof(void*),
0)));}pos=8;}{const char*_tmp10AA;void*_tmp10A9[1];struct Cyc_String_pa_struct
_tmp10A8;(_tmp10A8.tag=0,((_tmp10A8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp10A9[0]=& _tmp10A8,Cyc_fprintf(Cyc_stderr,((_tmp10AA="%s ",
_tag_dyneither(_tmp10AA,sizeof(char),4))),_tag_dyneither(_tmp10A9,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp10AD;void*_tmp10AC;(_tmp10AC=0,Cyc_fprintf(Cyc_stderr,((_tmp10AD="\n\t",
_tag_dyneither(_tmp10AD,sizeof(char),3))),_tag_dyneither(_tmp10AC,sizeof(void*),
0)));}pos=8;}{const char*_tmp10B0;void*_tmp10AF;(_tmp10AF=0,Cyc_fprintf(Cyc_stderr,((
_tmp10B0="are not compatible. ",_tag_dyneither(_tmp10B0,sizeof(char),21))),
_tag_dyneither(_tmp10AF,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp10B3;void*_tmp10B2;(_tmp10B2=0,
Cyc_fprintf(Cyc_stderr,((_tmp10B3="\n\t",_tag_dyneither(_tmp10B3,sizeof(char),3))),
_tag_dyneither(_tmp10B2,sizeof(void*),0)));}{const char*_tmp10B7;void*_tmp10B6[1];
struct Cyc_String_pa_struct _tmp10B5;(_tmp10B5.tag=0,((_tmp10B5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp10B6[0]=&
_tmp10B5,Cyc_fprintf(Cyc_stderr,((_tmp10B7="%s",_tag_dyneither(_tmp10B7,sizeof(
char),3))),_tag_dyneither(_tmp10B6,sizeof(void*),1)))))));};}{const char*_tmp10BA;
void*_tmp10B9;(_tmp10B9=0,Cyc_fprintf(Cyc_stderr,((_tmp10BA="\n",_tag_dyneither(
_tmp10BA,sizeof(char),2))),_tag_dyneither(_tmp10B9,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp10BE;
void*_tmp10BD[1];struct Cyc_String_pa_struct _tmp10BC;(_tmp10BC.tag=0,((_tmp10BC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10BD[0]=& _tmp10BC,Cyc_fprintf(
Cyc_stderr,((_tmp10BE="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp10BE,sizeof(char),36))),_tag_dyneither(_tmp10BD,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp10C1;
struct Cyc_Core_Impossible_struct*_tmp10C0;(int)_throw((void*)((_tmp10C0=
_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0[0]=((_tmp10C1.tag=Cyc_Core_Impossible,((
_tmp10C1.f1=msg,_tmp10C1)))),_tmp10C0)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp10C6;void*_tmp10C5[2];struct Cyc_String_pa_struct
_tmp10C4;struct Cyc_String_pa_struct _tmp10C3;(_tmp10C3.tag=0,((_tmp10C3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10C4.tag=0,((_tmp10C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp10C5[0]=& _tmp10C4,((_tmp10C5[1]=& _tmp10C3,Cyc_fprintf(Cyc_stderr,((
_tmp10C6="%s::%s ",_tag_dyneither(_tmp10C6,sizeof(char),8))),_tag_dyneither(
_tmp10C5,sizeof(void*),2)))))))))))));}{const char*_tmp10C9;void*_tmp10C8;(
_tmp10C8=0,Cyc_fprintf(Cyc_stderr,((_tmp10C9="\n",_tag_dyneither(_tmp10C9,
sizeof(char),2))),_tag_dyneither(_tmp10C8,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp10CA;Cyc_Tcutil_warning_segs=((_tmp10CA=_cycalloc(
sizeof(*_tmp10CA)),((_tmp10CA->hd=sg,((_tmp10CA->tl=Cyc_Tcutil_warning_segs,
_tmp10CA))))));}{struct _dyneither_ptr*_tmp10CD;struct Cyc_List_List*_tmp10CC;Cyc_Tcutil_warning_msgs=((
_tmp10CC=_cycalloc(sizeof(*_tmp10CC)),((_tmp10CC->hd=((_tmp10CD=_cycalloc(
sizeof(*_tmp10CD)),((_tmp10CD[0]=msg,_tmp10CD)))),((_tmp10CC->tl=Cyc_Tcutil_warning_msgs,
_tmp10CC))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp10D0;void*_tmp10CF;(_tmp10CF=0,Cyc_fprintf(Cyc_stderr,((
_tmp10D0="***Warnings***\n",_tag_dyneither(_tmp10D0,sizeof(char),16))),
_tag_dyneither(_tmp10CF,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp10D5;void*_tmp10D4[2];struct
Cyc_String_pa_struct _tmp10D3;struct Cyc_String_pa_struct _tmp10D2;(_tmp10D2.tag=0,((
_tmp10D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10D3.tag=
0,((_tmp10D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10D4[0]=&
_tmp10D3,((_tmp10D4[1]=& _tmp10D2,Cyc_fprintf(Cyc_stderr,((_tmp10D5="%s: %s\n",
_tag_dyneither(_tmp10D5,sizeof(char),8))),_tag_dyneither(_tmp10D4,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10D8;void*_tmp10D7;(
_tmp10D7=0,Cyc_fprintf(Cyc_stderr,((_tmp10D8="**************\n",_tag_dyneither(
_tmp10D8,sizeof(char),16))),_tag_dyneither(_tmp10D7,sizeof(void*),0)));}Cyc_fflush((
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10D9;*_tmp44=((_tmp10D9=
_cycalloc(sizeof(*_tmp10D9)),((_tmp10D9[0]=t2,_tmp10D9))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10DA;*_tmp47=((_tmp10DA=_cycalloc(sizeof(*
_tmp10DA)),((_tmp10DA->v=t2,_tmp10DA))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
struct Cyc_Absyn_Unknown_kb_struct _tmp10DD;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10DC;return(void*)((_tmp10DC=_cycalloc(sizeof(*_tmp10DC)),((_tmp10DC[0]=((
_tmp10DD.tag=1,((_tmp10DD.f1=0,_tmp10DD)))),_tmp10DC))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp10E0;struct
Cyc_Absyn_Less_kb_struct*_tmp10DF;return(void*)((_tmp10DF=_cycalloc(sizeof(*
_tmp10DF)),((_tmp10DF[0]=((_tmp10E0.tag=2,((_tmp10E0.f1=0,((_tmp10E0.f2=_tmp5C,
_tmp10E0)))))),_tmp10DF))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10E1;
return(_tmp10E1=_cycalloc(sizeof(*_tmp10E1)),((_tmp10E1->name=tv->name,((
_tmp10E1->identity=- 1,((_tmp10E1->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10E1)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple9*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple9*_tmp10E2;return(_tmp10E2=_cycalloc(sizeof(*
_tmp10E2)),((_tmp10E2->f1=_tmp64,((_tmp10E2->f2=_tmp65,((_tmp10E2->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10E2)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg){struct _tuple11 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple11*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple11*_tmp10E3;return(_tmp10E3=_cycalloc(sizeof(*_tmp10E3)),((_tmp10E3->f1=
_tmp6A,((_tmp10E3->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp10E3)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10E4;return(_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4->name=f->name,((
_tmp10E4->tq=f->tq,((_tmp10E4->type=Cyc_Tcutil_copy_type(f->type),((_tmp10E4->width=
f->width,((_tmp10E4->attributes=f->attributes,_tmp10E4)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10E5;return(_tmp10E5=_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp10E5->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp10E5)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10E6;return(_tmp10E6=
_cycalloc(sizeof(*_tmp10E6)),((_tmp10E6->name=f->name,((_tmp10E6->tag=f->tag,((
_tmp10E6->loc=f->loc,_tmp10E6)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp10E9;struct Cyc_Absyn_VarType_struct*
_tmp10E8;return(void*)((_tmp10E8=_cycalloc(sizeof(*_tmp10E8)),((_tmp10E8[0]=((
_tmp10E9.tag=2,((_tmp10E9.f1=Cyc_Tcutil_copy_tvar(t),_tmp10E9)))),_tmp10E8))));}
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
else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct _tmp10EC;struct
Cyc_Absyn_VarType_struct*_tmp10EB;return(void*)((_tmp10EB=_cycalloc(sizeof(*
_tmp10EB)),((_tmp10EB[0]=((_tmp10EC.tag=2,((_tmp10EC.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10EC)))),_tmp10EB))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10F2;struct Cyc_Absyn_DatatypeInfo _tmp10F1;
struct Cyc_Absyn_DatatypeType_struct*_tmp10F0;return(void*)((_tmp10F0=_cycalloc(
sizeof(*_tmp10F0)),((_tmp10F0[0]=((_tmp10F2.tag=3,((_tmp10F2.f1=((_tmp10F1.datatype_info=
_tmp7D,((_tmp10F1.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp10F1)))),_tmp10F2)))),
_tmp10F0))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10F8;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10F7;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10F6;return(void*)((_tmp10F6=_cycalloc(sizeof(*_tmp10F6)),((_tmp10F6[0]=((
_tmp10F8.tag=4,((_tmp10F8.f1=((_tmp10F7.field_info=_tmp81,((_tmp10F7.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp10F7)))),_tmp10F8)))),_tmp10F6))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE9=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpEA=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEC=_tmp86;union Cyc_Absyn_Constraint*_tmpED=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpEE=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp1102;struct Cyc_Absyn_PtrAtts
_tmp1101;struct Cyc_Absyn_PtrInfo _tmp1100;struct Cyc_Absyn_PointerType_struct*
_tmp10FF;return(void*)((_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF[0]=((
_tmp1102.tag=5,((_tmp1102.f1=((_tmp1100.elt_typ=_tmpE9,((_tmp1100.elt_tq=_tmpEC,((
_tmp1100.ptr_atts=((_tmp1101.rgn=_tmpEA,((_tmp1101.nullable=_tmpEB,((_tmp1101.bounds=
_tmpED,((_tmp1101.zero_term=_tmpEE,((_tmp1101.ptrloc=_tmp8C,_tmp1101)))))))))),
_tmp1100)))))),_tmp1102)))),_tmp10FF))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_DoubleType_struct*
_tmp8F=(struct Cyc_Absyn_DoubleType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;}
_LL34: return t;_LL35: {struct Cyc_Absyn_ArrayType_struct*_tmp90=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76;if(_tmp90->tag != 9)goto _LL37;else{_tmp91=_tmp90->f1;_tmp92=_tmp91.elt_type;
_tmp93=_tmp91.tq;_tmp94=_tmp91.num_elts;_tmp95=_tmp91.zero_term;_tmp96=_tmp91.zt_loc;}}
_LL36: {struct Cyc_Absyn_ArrayType_struct _tmp1108;struct Cyc_Absyn_ArrayInfo
_tmp1107;struct Cyc_Absyn_ArrayType_struct*_tmp1106;return(void*)((_tmp1106=
_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((_tmp1108.tag=9,((_tmp1108.f1=((
_tmp1107.elt_type=Cyc_Tcutil_copy_type(_tmp92),((_tmp1107.tq=_tmp93,((_tmp1107.num_elts=
_tmp94,((_tmp1107.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp95),((_tmp1107.zt_loc=_tmp96,_tmp1107)))))))))),
_tmp1108)))),_tmp1106))));}_LL37: {struct Cyc_Absyn_FnType_struct*_tmp97=(struct
Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp97->tag != 10)goto _LL39;else{_tmp98=_tmp97->f1;
_tmp99=_tmp98.tvars;_tmp9A=_tmp98.effect;_tmp9B=_tmp98.ret_tqual;_tmp9C=_tmp98.ret_typ;
_tmp9D=_tmp98.args;_tmp9E=_tmp98.c_varargs;_tmp9F=_tmp98.cyc_varargs;_tmpA0=
_tmp98.rgn_po;_tmpA1=_tmp98.attributes;}}_LL38: {struct Cyc_List_List*_tmpF6=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp99);struct Cyc_Core_Opt*
_tmp1109;struct Cyc_Core_Opt*_tmpF7=_tmp9A == 0?0:((_tmp1109=_cycalloc(sizeof(*
_tmp1109)),((_tmp1109->v=Cyc_Tcutil_copy_type((void*)_tmp9A->v),_tmp1109))));
void*_tmpF8=Cyc_Tcutil_copy_type(_tmp9C);struct Cyc_List_List*_tmpF9=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9D);int _tmpFA=_tmp9E;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9F != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9F;struct Cyc_Absyn_VarargInfo*
_tmp110A;cyc_varargs2=((_tmp110A=_cycalloc(sizeof(*_tmp110A)),((_tmp110A->name=
cv->name,((_tmp110A->tq=cv->tq,((_tmp110A->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp110A->inject=cv->inject,_tmp110A))))))))));}{struct Cyc_List_List*_tmpFC=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpA0);struct Cyc_List_List*_tmpFD=_tmpA1;
struct Cyc_Absyn_FnType_struct _tmp1110;struct Cyc_Absyn_FnInfo _tmp110F;struct Cyc_Absyn_FnType_struct*
_tmp110E;return(void*)((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E[0]=((
_tmp1110.tag=10,((_tmp1110.f1=((_tmp110F.tvars=_tmpF6,((_tmp110F.effect=_tmpF7,((
_tmp110F.ret_tqual=_tmp9B,((_tmp110F.ret_typ=_tmpF8,((_tmp110F.args=_tmpF9,((
_tmp110F.c_varargs=_tmpFA,((_tmp110F.cyc_varargs=cyc_varargs2,((_tmp110F.rgn_po=
_tmpFC,((_tmp110F.attributes=_tmpFD,_tmp110F)))))))))))))))))),_tmp1110)))),
_tmp110E))));};}_LL39: {struct Cyc_Absyn_TupleType_struct*_tmpA2=(struct Cyc_Absyn_TupleType_struct*)
_tmp76;if(_tmpA2->tag != 11)goto _LL3B;else{_tmpA3=_tmpA2->f1;}}_LL3A: {struct Cyc_Absyn_TupleType_struct
_tmp1113;struct Cyc_Absyn_TupleType_struct*_tmp1112;return(void*)((_tmp1112=
_cycalloc(sizeof(*_tmp1112)),((_tmp1112[0]=((_tmp1113.tag=11,((_tmp1113.f1=((
struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA3),_tmp1113)))),_tmp1112))));}_LL3B: {
struct Cyc_Absyn_AggrType_struct*_tmpA4=(struct Cyc_Absyn_AggrType_struct*)_tmp76;
if(_tmpA4->tag != 12)goto _LL3D;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.aggr_info;if((
_tmpA6.UnknownAggr).tag != 1)goto _LL3D;_tmpA7=(struct _tuple4)(_tmpA6.UnknownAggr).val;
_tmpA8=_tmpA7.f1;_tmpA9=_tmpA7.f2;_tmpAA=_tmpA7.f3;_tmpAB=_tmpA5.targs;}}_LL3C: {
struct Cyc_Absyn_AggrType_struct _tmp1119;struct Cyc_Absyn_AggrInfo _tmp1118;struct
Cyc_Absyn_AggrType_struct*_tmp1117;return(void*)((_tmp1117=_cycalloc(sizeof(*
_tmp1117)),((_tmp1117[0]=((_tmp1119.tag=12,((_tmp1119.f1=((_tmp1118.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA8,_tmpA9,_tmpAA),((_tmp1118.targs=Cyc_Tcutil_copy_types(
_tmpAB),_tmp1118)))),_tmp1119)))),_tmp1117))));}_LL3D: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAC->tag != 12)goto _LL3F;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;if((_tmpAE.KnownAggr).tag != 2)
goto _LL3F;_tmpAF=(struct Cyc_Absyn_Aggrdecl**)(_tmpAE.KnownAggr).val;_tmpB0=
_tmpAD.targs;}}_LL3E: {struct Cyc_Absyn_AggrType_struct _tmp111F;struct Cyc_Absyn_AggrInfo
_tmp111E;struct Cyc_Absyn_AggrType_struct*_tmp111D;return(void*)((_tmp111D=
_cycalloc(sizeof(*_tmp111D)),((_tmp111D[0]=((_tmp111F.tag=12,((_tmp111F.f1=((
_tmp111E.aggr_info=Cyc_Absyn_KnownAggr(_tmpAF),((_tmp111E.targs=Cyc_Tcutil_copy_types(
_tmpB0),_tmp111E)))),_tmp111F)))),_tmp111D))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB1->tag != 13)goto
_LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL40: {struct Cyc_Absyn_AnonAggrType_struct
_tmp1122;struct Cyc_Absyn_AnonAggrType_struct*_tmp1121;return(void*)((_tmp1121=
_cycalloc(sizeof(*_tmp1121)),((_tmp1121[0]=((_tmp1122.tag=13,((_tmp1122.f1=
_tmpB2,((_tmp1122.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB3),_tmp1122)))))),_tmp1121))));}_LL41: {struct Cyc_Absyn_EnumType_struct*
_tmpB4=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB4->tag != 14)goto _LL43;
else{_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;}}_LL42: {struct Cyc_Absyn_EnumType_struct
_tmp1125;struct Cyc_Absyn_EnumType_struct*_tmp1124;return(void*)((_tmp1124=
_cycalloc(sizeof(*_tmp1124)),((_tmp1124[0]=((_tmp1125.tag=14,((_tmp1125.f1=
_tmpB5,((_tmp1125.f2=_tmpB6,_tmp1125)))))),_tmp1124))));}_LL43: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB7->tag != 15)goto
_LL45;else{_tmpB8=_tmpB7->f1;}}_LL44: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1128;struct Cyc_Absyn_AnonEnumType_struct*_tmp1127;return(void*)((_tmp1127=
_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1128.tag=15,((_tmp1128.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB8),_tmp1128)))),
_tmp1127))));}_LL45: {struct Cyc_Absyn_TagType_struct*_tmpB9=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB9->tag != 20)goto _LL47;else{_tmpBA=(void*)_tmpB9->f1;}}_LL46: {
struct Cyc_Absyn_TagType_struct _tmp112B;struct Cyc_Absyn_TagType_struct*_tmp112A;
return(void*)((_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A[0]=((_tmp112B.tag=
20,((_tmp112B.f1=(void*)Cyc_Tcutil_copy_type(_tmpBA),_tmp112B)))),_tmp112A))));}
_LL47: {struct Cyc_Absyn_ValueofType_struct*_tmpBB=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBB->tag != 19)goto _LL49;else{_tmpBC=_tmpBB->f1;}}_LL48: {struct Cyc_Absyn_ValueofType_struct
_tmp112E;struct Cyc_Absyn_ValueofType_struct*_tmp112D;return(void*)((_tmp112D=
_cycalloc(sizeof(*_tmp112D)),((_tmp112D[0]=((_tmp112E.tag=19,((_tmp112E.f1=
_tmpBC,_tmp112E)))),_tmp112D))));}_LL49: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBD=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBD->tag != 16)goto
_LL4B;else{_tmpBE=(void*)_tmpBD->f1;}}_LL4A: {struct Cyc_Absyn_RgnHandleType_struct
_tmp1131;struct Cyc_Absyn_RgnHandleType_struct*_tmp1130;return(void*)((_tmp1130=
_cycalloc(sizeof(*_tmp1130)),((_tmp1130[0]=((_tmp1131.tag=16,((_tmp1131.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBE),_tmp1131)))),_tmp1130))));}_LL4B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBF=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBF->tag != 17)goto _LL4D;
else{_tmpC0=(void*)_tmpBF->f1;_tmpC1=(void*)_tmpBF->f2;}}_LL4C: {struct Cyc_Absyn_DynRgnType_struct
_tmp1134;struct Cyc_Absyn_DynRgnType_struct*_tmp1133;return(void*)((_tmp1133=
_cycalloc(sizeof(*_tmp1133)),((_tmp1133[0]=((_tmp1134.tag=17,((_tmp1134.f1=(void*)
Cyc_Tcutil_copy_type(_tmpC0),((_tmp1134.f2=(void*)Cyc_Tcutil_copy_type(_tmpC1),
_tmp1134)))))),_tmp1133))));}_LL4D: {struct Cyc_Absyn_TypedefType_struct*_tmpC2=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC2->tag != 18)goto _LL4F;else{
_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_tmpC5=_tmpC2->f3;}}_LL4E: {struct Cyc_Absyn_TypedefType_struct
_tmp1137;struct Cyc_Absyn_TypedefType_struct*_tmp1136;return(void*)((_tmp1136=
_cycalloc(sizeof(*_tmp1136)),((_tmp1136[0]=((_tmp1137.tag=18,((_tmp1137.f1=
_tmpC3,((_tmp1137.f2=Cyc_Tcutil_copy_types(_tmpC4),((_tmp1137.f3=_tmpC5,((
_tmp1137.f4=0,_tmp1137)))))))))),_tmp1136))));}_LL4F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC6->tag != 22)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_RefCntRgn_struct*_tmpC7=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp76;if(_tmpC7->tag != 23)goto _LL53;}_LL52: goto _LL54;_LL53: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC8=(struct Cyc_Absyn_HeapRgn_struct*)_tmp76;if(_tmpC8->tag != 21)goto _LL55;}
_LL54: return t;_LL55: {struct Cyc_Absyn_AccessEff_struct*_tmpC9=(struct Cyc_Absyn_AccessEff_struct*)
_tmp76;if(_tmpC9->tag != 24)goto _LL57;else{_tmpCA=(void*)_tmpC9->f1;}}_LL56: {
struct Cyc_Absyn_AccessEff_struct _tmp113A;struct Cyc_Absyn_AccessEff_struct*
_tmp1139;return(void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((
_tmp113A.tag=24,((_tmp113A.f1=(void*)Cyc_Tcutil_copy_type(_tmpCA),_tmp113A)))),
_tmp1139))));}_LL57: {struct Cyc_Absyn_JoinEff_struct*_tmpCB=(struct Cyc_Absyn_JoinEff_struct*)
_tmp76;if(_tmpCB->tag != 25)goto _LL59;else{_tmpCC=_tmpCB->f1;}}_LL58: {struct Cyc_Absyn_JoinEff_struct
_tmp113D;struct Cyc_Absyn_JoinEff_struct*_tmp113C;return(void*)((_tmp113C=
_cycalloc(sizeof(*_tmp113C)),((_tmp113C[0]=((_tmp113D.tag=25,((_tmp113D.f1=Cyc_Tcutil_copy_types(
_tmpCC),_tmp113D)))),_tmp113C))));}_LL59: {struct Cyc_Absyn_RgnsEff_struct*_tmpCD=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp76;if(_tmpCD->tag != 26)goto _LL5B;else{_tmpCE=(
void*)_tmpCD->f1;}}_LL5A: {struct Cyc_Absyn_RgnsEff_struct _tmp1140;struct Cyc_Absyn_RgnsEff_struct*
_tmp113F;return(void*)((_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F[0]=((
_tmp1140.tag=26,((_tmp1140.f1=(void*)Cyc_Tcutil_copy_type(_tmpCE),_tmp1140)))),
_tmp113F))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCF=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCF->tag != 27)goto _LL5D;else{_tmpD0=_tmpCF->f1;_tmpD1=*_tmpD0;
_tmpD2=_tmpD1.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD3=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL5D;else{_tmpD4=_tmpD3->f1;}};}}_LL5C: {struct
Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD4->tvs);struct Cyc_Absyn_AggrType_struct
_tmp1146;struct Cyc_Absyn_AggrInfo _tmp1145;struct Cyc_Absyn_AggrType_struct*
_tmp1144;return(void*)((_tmp1144=_cycalloc(sizeof(*_tmp1144)),((_tmp1144[0]=((
_tmp1146.tag=12,((_tmp1146.f1=((_tmp1145.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD4->kind,
_tmpD4->name,0),((_tmp1145.targs=_tmp120,_tmp1145)))),_tmp1146)))),_tmp1144))));}
_LL5D: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD5=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD5->tag != 27)goto _LL5F;else{_tmpD6=_tmpD5->f1;_tmpD7=*_tmpD6;
_tmpD8=_tmpD7.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD9=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD8;if(_tmpD9->tag != 1)goto _LL5F;else{_tmpDA=_tmpD9->f1;}};}}_LL5E: {struct
Cyc_Absyn_EnumType_struct _tmp1149;struct Cyc_Absyn_EnumType_struct*_tmp1148;
return(void*)((_tmp1148=_cycalloc(sizeof(*_tmp1148)),((_tmp1148[0]=((_tmp1149.tag=
14,((_tmp1149.f1=_tmpDA->name,((_tmp1149.f2=0,_tmp1149)))))),_tmp1148))));}_LL5F: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpDB=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpDB->tag != 27)goto _LL22;else{_tmpDC=_tmpDB->f1;_tmpDD=*_tmpDC;
_tmpDE=_tmpDD.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDF=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDE;if(_tmpDF->tag != 2)goto _LL22;else{_tmpE0=_tmpDF->f1;}};}}_LL60: {struct
Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpE0->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp1153;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1152;struct Cyc_Absyn_DatatypeInfo
_tmp1151;struct Cyc_Absyn_DatatypeType_struct*_tmp1150;return(void*)((_tmp1150=
_cycalloc(sizeof(*_tmp1150)),((_tmp1150[0]=((_tmp1153.tag=3,((_tmp1153.f1=((
_tmp1151.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp1152.name=_tmpE0->name,((
_tmp1152.is_extensible=0,_tmp1152))))),((_tmp1151.targs=_tmp126,_tmp1151)))),
_tmp1153)))),_tmp1150))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp12B=d;struct Cyc_Absyn_Exp*_tmp12D;struct _dyneither_ptr*_tmp12F;_LL62: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12C=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp12B;if(_tmp12C->tag != 0)goto _LL64;else{_tmp12D=_tmp12C->f1;}}_LL63: {struct
Cyc_Absyn_ArrayElement_struct _tmp1156;struct Cyc_Absyn_ArrayElement_struct*
_tmp1155;return(void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155[0]=((
_tmp1156.tag=0,((_tmp1156.f1=Cyc_Tcutil_deep_copy_exp(_tmp12D),_tmp1156)))),
_tmp1155))));}_LL64: {struct Cyc_Absyn_FieldName_struct*_tmp12E=(struct Cyc_Absyn_FieldName_struct*)
_tmp12B;if(_tmp12E->tag != 1)goto _LL61;else{_tmp12F=_tmp12E->f1;}}_LL65: return d;
_LL61:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e){struct _tuple18*_tmp1157;
return(_tmp1157=_cycalloc(sizeof(*_tmp1157)),((_tmp1157->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp1157->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1157)))));}
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
_tmp141=_tmp13E->f3;}}_LL70: {struct Cyc_Core_Opt*_tmp1158;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13F),(unsigned int)_tmp140?(_tmp1158=
_cycalloc_atomic(sizeof(*_tmp1158)),((_tmp1158->v=(void*)((enum Cyc_Absyn_Primop)
_tmp140->v),_tmp1158))): 0,Cyc_Tcutil_deep_copy_exp(_tmp141),e->loc);}_LL71: {
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
struct Cyc_Absyn_FnCall_e_struct _tmp1162;struct Cyc_Absyn_VarargInfo*_tmp1161;
struct Cyc_Absyn_VarargCallInfo*_tmp1160;struct Cyc_Absyn_FnCall_e_struct*_tmp115F;
return Cyc_Absyn_new_exp((void*)((_tmp115F=_cycalloc(sizeof(*_tmp115F)),((
_tmp115F[0]=((_tmp1162.tag=11,((_tmp1162.f1=Cyc_Tcutil_deep_copy_exp(_tmp156),((
_tmp1162.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),((
_tmp1162.f3=((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160->num_varargs=
_tmp1BD,((_tmp1160->injectors=_tmp1BE,((_tmp1160->vai=((_tmp1161=_cycalloc(
sizeof(*_tmp1161)),((_tmp1161->name=_tmp1C2,((_tmp1161->tq=_tmp1C3,((_tmp1161->type=
Cyc_Tcutil_copy_type(_tmp1C4),((_tmp1161->inject=_tmp1C5,_tmp1161)))))))))),
_tmp1160)))))))),_tmp1162)))))))),_tmp115F)))),e->loc);};}_LLBC:;_LLBD: {struct
Cyc_Absyn_FnCall_e_struct _tmp1165;struct Cyc_Absyn_FnCall_e_struct*_tmp1164;
return Cyc_Absyn_new_exp((void*)((_tmp1164=_cycalloc(sizeof(*_tmp1164)),((
_tmp1164[0]=((_tmp1165.tag=11,((_tmp1165.f1=Cyc_Tcutil_deep_copy_exp(_tmp156),((
_tmp1165.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),((
_tmp1165.f3=_tmp158,_tmp1165)))))))),_tmp1164)))),e->loc);}_LLB9:;}_LL7F: {
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
_tmp173->f4;}}_LL94: {struct Cyc_Absyn_AggrMember_e_struct _tmp1168;struct Cyc_Absyn_AggrMember_e_struct*
_tmp1167;return Cyc_Absyn_new_exp((void*)((_tmp1167=_cycalloc(sizeof(*_tmp1167)),((
_tmp1167[0]=((_tmp1168.tag=22,((_tmp1168.f1=Cyc_Tcutil_deep_copy_exp(_tmp174),((
_tmp1168.f2=_tmp175,((_tmp1168.f3=_tmp176,((_tmp1168.f4=_tmp177,_tmp1168)))))))))),
_tmp1167)))),e->loc);}_LL95: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp178=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp133;if(_tmp178->tag != 23)goto _LL97;else{
_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;_tmp17B=_tmp178->f3;_tmp17C=_tmp178->f4;}}
_LL96: {struct Cyc_Absyn_AggrArrow_e_struct _tmp116B;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp116A;return Cyc_Absyn_new_exp((void*)((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((
_tmp116A[0]=((_tmp116B.tag=23,((_tmp116B.f1=Cyc_Tcutil_deep_copy_exp(_tmp179),((
_tmp116B.f2=_tmp17A,((_tmp116B.f3=_tmp17B,((_tmp116B.f4=_tmp17C,_tmp116B)))))))))),
_tmp116A)))),e->loc);}_LL97: {struct Cyc_Absyn_Subscript_e_struct*_tmp17D=(struct
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
_tmp116C;vopt1=((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((_tmp116C->v=(struct
_dyneither_ptr*)_tmp185->v,_tmp116C))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp1172;struct _tuple9*_tmp1171;struct Cyc_Absyn_CompoundLit_e_struct*_tmp1170;
return Cyc_Absyn_new_exp((void*)((_tmp1170=_cycalloc(sizeof(*_tmp1170)),((
_tmp1170[0]=((_tmp1172.tag=26,((_tmp1172.f1=((_tmp1171=_cycalloc(sizeof(*
_tmp1171)),((_tmp1171->f1=vopt1,((_tmp1171->f2=_tmp186,((_tmp1171->f3=Cyc_Tcutil_copy_type(
_tmp187),_tmp1171)))))))),((_tmp1172.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp188),_tmp1172)))))),_tmp1170)))),e->loc);};}_LL9D: {struct Cyc_Absyn_Array_e_struct*
_tmp189=(struct Cyc_Absyn_Array_e_struct*)_tmp133;if(_tmp189->tag != 27)goto _LL9F;
else{_tmp18A=_tmp189->f1;}}_LL9E: {struct Cyc_Absyn_Array_e_struct _tmp1175;struct
Cyc_Absyn_Array_e_struct*_tmp1174;return Cyc_Absyn_new_exp((void*)((_tmp1174=
_cycalloc(sizeof(*_tmp1174)),((_tmp1174[0]=((_tmp1175.tag=27,((_tmp1175.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp18A),_tmp1175)))),_tmp1174)))),e->loc);}
_LL9F: {struct Cyc_Absyn_Comprehension_e_struct*_tmp18B=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp133;if(_tmp18B->tag != 28)goto _LLA1;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LLA0: {struct Cyc_Absyn_Comprehension_e_struct
_tmp1178;struct Cyc_Absyn_Comprehension_e_struct*_tmp1177;return Cyc_Absyn_new_exp((
void*)((_tmp1177=_cycalloc(sizeof(*_tmp1177)),((_tmp1177[0]=((_tmp1178.tag=28,((
_tmp1178.f1=_tmp18C,((_tmp1178.f2=Cyc_Tcutil_deep_copy_exp(_tmp18D),((_tmp1178.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18E),((_tmp1178.f4=_tmp18F,_tmp1178)))))))))),
_tmp1177)))),e->loc);}_LLA1: {struct Cyc_Absyn_Aggregate_e_struct*_tmp190=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp133;if(_tmp190->tag != 29)goto _LLA3;else{
_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;_tmp193=_tmp190->f3;_tmp194=_tmp190->f4;}}
_LLA2: {struct Cyc_Absyn_Aggregate_e_struct _tmp117B;struct Cyc_Absyn_Aggregate_e_struct*
_tmp117A;return Cyc_Absyn_new_exp((void*)((_tmp117A=_cycalloc(sizeof(*_tmp117A)),((
_tmp117A[0]=((_tmp117B.tag=29,((_tmp117B.f1=_tmp191,((_tmp117B.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp192),((_tmp117B.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp193),((
_tmp117B.f4=_tmp194,_tmp117B)))))))))),_tmp117A)))),e->loc);}_LLA3: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp195=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp133;if(_tmp195->tag != 30)goto
_LLA5;else{_tmp196=(void*)_tmp195->f1;_tmp197=_tmp195->f2;}}_LLA4: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp117E;struct Cyc_Absyn_AnonStruct_e_struct*_tmp117D;return Cyc_Absyn_new_exp((
void*)((_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D[0]=((_tmp117E.tag=30,((
_tmp117E.f1=(void*)Cyc_Tcutil_copy_type(_tmp196),((_tmp117E.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp197),_tmp117E)))))),_tmp117D)))),e->loc);}_LLA5: {struct Cyc_Absyn_Datatype_e_struct*
_tmp198=(struct Cyc_Absyn_Datatype_e_struct*)_tmp133;if(_tmp198->tag != 31)goto
_LLA7;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;_tmp19B=_tmp198->f3;}}_LLA6: {
struct Cyc_Absyn_Datatype_e_struct _tmp1181;struct Cyc_Absyn_Datatype_e_struct*
_tmp1180;return Cyc_Absyn_new_exp((void*)((_tmp1180=_cycalloc(sizeof(*_tmp1180)),((
_tmp1180[0]=((_tmp1181.tag=31,((_tmp1181.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp199),((_tmp1181.f2=_tmp19A,((_tmp1181.f3=_tmp19B,_tmp1181)))))))),_tmp1180)))),
e->loc);}_LLA7: {struct Cyc_Absyn_Enum_e_struct*_tmp19C=(struct Cyc_Absyn_Enum_e_struct*)
_tmp133;if(_tmp19C->tag != 32)goto _LLA9;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;
_tmp19F=_tmp19C->f3;}}_LLA8: return e;_LLA9: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1A0=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp133;if(_tmp1A0->tag != 33)goto
_LLAB;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=(void*)_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;}}
_LLAA: {struct Cyc_Absyn_AnonEnum_e_struct _tmp1184;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1183;return Cyc_Absyn_new_exp((void*)((_tmp1183=_cycalloc(sizeof(*_tmp1183)),((
_tmp1183[0]=((_tmp1184.tag=33,((_tmp1184.f1=_tmp1A1,((_tmp1184.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A2),((_tmp1184.f3=_tmp1A3,_tmp1184)))))))),_tmp1183)))),e->loc);}_LLAB: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A4=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp133;if(_tmp1A4->tag != 34)goto _LLAD;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A5.is_calloc;
_tmp1A7=_tmp1A5.rgn;_tmp1A8=_tmp1A5.elt_type;_tmp1A9=_tmp1A5.num_elts;_tmp1AA=
_tmp1A5.fat_result;}}_LLAC: {struct Cyc_Absyn_Exp*_tmp1E0=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A7;if(_tmp1A7 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A7);{void**t1=_tmp1A8;if(_tmp1A8 != 0){void**_tmp1185;
t1=((_tmp1185=_cycalloc(sizeof(*_tmp1185)),((_tmp1185[0]=Cyc_Tcutil_copy_type(*
_tmp1A8),_tmp1185))));}{struct Cyc_Absyn_Malloc_e_struct _tmp118B;struct Cyc_Absyn_MallocInfo
_tmp118A;struct Cyc_Absyn_Malloc_e_struct*_tmp1189;_tmp1E0->r=(void*)((_tmp1189=
_cycalloc(sizeof(*_tmp1189)),((_tmp1189[0]=((_tmp118B.tag=34,((_tmp118B.f1=((
_tmp118A.is_calloc=_tmp1A6,((_tmp118A.rgn=r1,((_tmp118A.elt_type=t1,((_tmp118A.num_elts=
_tmp1A9,((_tmp118A.fat_result=_tmp1AA,_tmp118A)))))))))),_tmp118B)))),_tmp1189))));}
return _tmp1E0;};}_LLAD: {struct Cyc_Absyn_Swap_e_struct*_tmp1AB=(struct Cyc_Absyn_Swap_e_struct*)
_tmp133;if(_tmp1AB->tag != 35)goto _LLAF;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}
_LLAE: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AC),Cyc_Tcutil_deep_copy_exp(
_tmp1AD),e->loc);_LLAF: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AE=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp133;if(_tmp1AE->tag != 36)goto _LLB1;else{
_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLB0: {struct Cyc_Core_Opt*nopt1=
_tmp1AF;if(_tmp1AF != 0){struct Cyc_Core_Opt*_tmp118C;nopt1=((_tmp118C=_cycalloc(
sizeof(*_tmp118C)),((_tmp118C->v=(struct _tuple2*)_tmp1AF->v,_tmp118C))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp118F;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp118E;return Cyc_Absyn_new_exp((void*)((_tmp118E=_cycalloc(sizeof(*_tmp118E)),((
_tmp118E[0]=((_tmp118F.tag=36,((_tmp118F.f1=nopt1,((_tmp118F.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1B0),_tmp118F)))))),_tmp118E)))),e->loc);};}_LLB1: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1B1=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp133;if(_tmp1B1->tag != 37)goto
_LLB3;}_LLB2: {struct Cyc_Core_Failure_struct _tmp1195;const char*_tmp1194;struct
Cyc_Core_Failure_struct*_tmp1193;(int)_throw((void*)((_tmp1193=_cycalloc(sizeof(*
_tmp1193)),((_tmp1193[0]=((_tmp1195.tag=Cyc_Core_Failure,((_tmp1195.f1=((
_tmp1194="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp1194,
sizeof(char),45))),_tmp1195)))),_tmp1193)))));}_LLB3: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B2=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp133;if(_tmp1B2->tag != 38)goto
_LLB5;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LLB4: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp1198;struct Cyc_Absyn_Tagcheck_e_struct*_tmp1197;return Cyc_Absyn_new_exp((
void*)((_tmp1197=_cycalloc(sizeof(*_tmp1197)),((_tmp1197[0]=((_tmp1198.tag=38,((
_tmp1198.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B3),((_tmp1198.f2=_tmp1B4,_tmp1198)))))),
_tmp1197)))),e->loc);}_LLB5: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B5=(struct
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
_tmp1EF != _tmp1F3){struct _tuple19 _tmp1199;struct _tuple19 _tmp1F6=(_tmp1199.f1=
_tmp1EF,((_tmp1199.f2=_tmp1F3,_tmp1199)));enum Cyc_Absyn_KindQual _tmp1F7;enum 
Cyc_Absyn_KindQual _tmp1F8;enum Cyc_Absyn_KindQual _tmp1F9;enum Cyc_Absyn_KindQual
_tmp1FA;enum Cyc_Absyn_KindQual _tmp1FB;enum Cyc_Absyn_KindQual _tmp1FC;_LLBF:
_tmp1F7=_tmp1F6.f1;if(_tmp1F7 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F8=_tmp1F6.f2;
if(_tmp1F8 != Cyc_Absyn_MemKind)goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1F9=_tmp1F6.f1;
if(_tmp1F9 != Cyc_Absyn_BoxKind)goto _LLC3;_tmp1FA=_tmp1F6.f2;if(_tmp1FA != Cyc_Absyn_AnyKind)
goto _LLC3;_LLC2: goto _LLC4;_LLC3: _tmp1FB=_tmp1F6.f1;if(_tmp1FB != Cyc_Absyn_MemKind)
goto _LLC5;_tmp1FC=_tmp1F6.f2;if(_tmp1FC != Cyc_Absyn_AnyKind)goto _LLC5;_LLC4: goto
_LLBE;_LLC5:;_LLC6: return 0;_LLBE:;}if(_tmp1F0 != _tmp1F4){struct _tuple20 _tmp119A;
struct _tuple20 _tmp1FE=(_tmp119A.f1=_tmp1F0,((_tmp119A.f2=_tmp1F4,_tmp119A)));
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
_LLD3:;_LLD4:{struct Cyc_Absyn_Less_kb_struct _tmp119D;struct Cyc_Absyn_Less_kb_struct*
_tmp119C;tv->kind=(void*)((_tmp119C=_cycalloc(sizeof(*_tmp119C)),((_tmp119C[0]=((
_tmp119D.tag=2,((_tmp119D.f1=0,((_tmp119D.f2=def,_tmp119D)))))),_tmp119C))));}
return def;_LLCE:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp119E;struct _tuple0 _tmp20B=(_tmp119E.f1=Cyc_Absyn_compress_kb(kb1),((_tmp119E.f2=
Cyc_Absyn_compress_kb(kb2),_tmp119E)));void*_tmp20C;struct Cyc_Absyn_Kind*_tmp20E;
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
struct Cyc_Core_Opt*_tmp119F;*_tmp215=((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((
_tmp119F->v=kb2,_tmp119F))));}return 1;}else{return 0;}_LLDA: _tmp21A=_tmp20B.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp21B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp21A;if(
_tmp21B->tag != 0)goto _LLDC;else{_tmp21C=_tmp21B->f1;}};_tmp21D=_tmp20B.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21E=(struct Cyc_Absyn_Less_kb_struct*)_tmp21D;
if(_tmp21E->tag != 2)goto _LLDC;else{_tmp21F=_tmp21E->f1;_tmp220=(struct Cyc_Core_Opt**)&
_tmp21E->f1;_tmp221=_tmp21E->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp21C,_tmp221)){{
struct Cyc_Core_Opt*_tmp11A0;*_tmp220=((_tmp11A0=_cycalloc(sizeof(*_tmp11A0)),((
_tmp11A0->v=kb1,_tmp11A0))));}return 1;}else{return 0;}_LLDC: _tmp222=_tmp20B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp223=(struct Cyc_Absyn_Less_kb_struct*)_tmp222;
if(_tmp223->tag != 2)goto _LLDE;else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)&
_tmp223->f1;_tmp226=_tmp223->f2;}};_tmp227=_tmp20B.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp228=(struct Cyc_Absyn_Less_kb_struct*)_tmp227;if(_tmp228->tag != 2)goto _LLDE;
else{_tmp229=_tmp228->f1;_tmp22A=(struct Cyc_Core_Opt**)& _tmp228->f1;_tmp22B=
_tmp228->f2;}};_LLDD: if(Cyc_Tcutil_kind_leq(_tmp226,_tmp22B)){{struct Cyc_Core_Opt*
_tmp11A1;*_tmp22A=((_tmp11A1=_cycalloc(sizeof(*_tmp11A1)),((_tmp11A1->v=kb1,
_tmp11A1))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp22B,_tmp226)){{struct Cyc_Core_Opt*
_tmp11A2;*_tmp225=((_tmp11A2=_cycalloc(sizeof(*_tmp11A2)),((_tmp11A2->v=kb2,
_tmp11A2))));}return 1;}else{return 0;}}_LLDE: _tmp22C=_tmp20B.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22C;if(_tmp22D->tag != 1)goto
_LLE0;else{_tmp22E=_tmp22D->f1;_tmp22F=(struct Cyc_Core_Opt**)& _tmp22D->f1;}};
_tmp230=_tmp20B.f2;_LLDF: _tmp231=_tmp230;_tmp235=_tmp22F;goto _LLE1;_LLE0: _tmp231=
_tmp20B.f1;_tmp232=_tmp20B.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp233=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp232;if(_tmp233->tag != 1)goto _LLD5;else{_tmp234=
_tmp233->f1;_tmp235=(struct Cyc_Core_Opt**)& _tmp233->f1;}};_LLE1:{struct Cyc_Core_Opt*
_tmp11A3;*_tmp235=((_tmp11A3=_cycalloc(sizeof(*_tmp11A3)),((_tmp11A3->v=_tmp231,
_tmp11A3))));}return 1;_LLD5:;}struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp23B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23D;_LLE3: {struct
Cyc_Absyn_VarType_struct*_tmp23C=(struct Cyc_Absyn_VarType_struct*)_tmp23B;if(
_tmp23C->tag != 2)goto _LLE5;else{_tmp23D=_tmp23C->f1;}}_LLE4: {void*_tmp23E=
_tmp23D->kind;_tmp23D->kind=kb;{struct _tuple15 _tmp11A4;return(_tmp11A4.f1=
_tmp23D,((_tmp11A4.f2=_tmp23E,_tmp11A4)));};}_LLE5:;_LLE6: {const char*_tmp11A8;
void*_tmp11A7[1];struct Cyc_String_pa_struct _tmp11A6;(_tmp11A6.tag=0,((_tmp11A6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp11A7[0]=& _tmp11A6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11A8="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp11A8,sizeof(char),40))),_tag_dyneither(_tmp11A7,sizeof(void*),
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
_LL109: {const char*_tmp11AB;void*_tmp11AA;(_tmp11AA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AB="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp11AB,sizeof(char),39))),_tag_dyneither(_tmp11AA,sizeof(void*),
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
_LL12B;}_LL12A: {struct Cyc_Absyn_Kind*_tmp11AC;return(_tmp11AC=_cycalloc_atomic(
sizeof(*_tmp11AC)),((_tmp11AC->kind=Cyc_Absyn_MemKind,((_tmp11AC->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26F.ptr_atts).rgn))->aliasqual,_tmp11AC)))));}_LL12B: {struct Cyc_Absyn_Upper_b_struct*
_tmp290=(struct Cyc_Absyn_Upper_b_struct*)_tmp28E;if(_tmp290->tag != 1)goto _LL128;}
_LL12C: {struct Cyc_Absyn_Kind*_tmp11AD;return(_tmp11AD=_cycalloc_atomic(sizeof(*
_tmp11AD)),((_tmp11AD->kind=Cyc_Absyn_BoxKind,((_tmp11AD->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26F.ptr_atts).rgn))->aliasqual,_tmp11AD)))));}_LL128:;}_LL112: {struct Cyc_Absyn_ValueofType_struct*
_tmp270=(struct Cyc_Absyn_ValueofType_struct*)_tmp243;if(_tmp270->tag != 19)goto
_LL114;}_LL113: return& Cyc_Tcutil_ik;_LL114: {struct Cyc_Absyn_TagType_struct*
_tmp271=(struct Cyc_Absyn_TagType_struct*)_tmp243;if(_tmp271->tag != 20)goto _LL116;}
_LL115: return& Cyc_Tcutil_bk;_LL116: {struct Cyc_Absyn_ArrayType_struct*_tmp272=(
struct Cyc_Absyn_ArrayType_struct*)_tmp243;if(_tmp272->tag != 9)goto _LL118;}_LL117:
goto _LL119;_LL118: {struct Cyc_Absyn_TupleType_struct*_tmp273=(struct Cyc_Absyn_TupleType_struct*)
_tmp243;if(_tmp273->tag != 11)goto _LL11A;}_LL119: return& Cyc_Tcutil_mk;_LL11A: {
struct Cyc_Absyn_TypedefType_struct*_tmp274=(struct Cyc_Absyn_TypedefType_struct*)
_tmp243;if(_tmp274->tag != 18)goto _LL11C;else{_tmp275=_tmp274->f3;}}_LL11B: if(
_tmp275 == 0  || _tmp275->kind == 0){const char*_tmp11B1;void*_tmp11B0[1];struct Cyc_String_pa_struct
_tmp11AF;(_tmp11AF.tag=0,((_tmp11AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp11B0[0]=& _tmp11AF,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11B1="typ_kind: typedef found: %s",
_tag_dyneither(_tmp11B1,sizeof(char),28))),_tag_dyneither(_tmp11B0,sizeof(void*),
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
Cyc_Tcutil_fast_tvar_cmp,env,_tmp29F)){{const char*_tmp11B2;Cyc_Tcutil_failure_reason=((
_tmp11B2="(type variable would escape scope)",_tag_dyneither(_tmp11B2,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL132;_LL135: {
struct Cyc_Absyn_Evar_struct*_tmp2A0=(struct Cyc_Absyn_Evar_struct*)_tmp29D;if(
_tmp2A0->tag != 1)goto _LL137;else{_tmp2A1=_tmp2A0->f2;_tmp2A2=_tmp2A0->f4;_tmp2A3=(
struct Cyc_Core_Opt**)& _tmp2A0->f4;}}_LL136: if(t == evar){{const char*_tmp11B3;Cyc_Tcutil_failure_reason=((
_tmp11B3="(occurs check)",_tag_dyneither(_tmp11B3,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp2A1 != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp2A1->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp2A3))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2CF=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A3))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp11B4;_tmp2CF=((
_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp11B4->tl=_tmp2CF,_tmp11B4))))));}}}{struct Cyc_Core_Opt*_tmp11B5;*_tmp2A3=((
_tmp11B5=_cycalloc(sizeof(*_tmp11B5)),((_tmp11B5->v=_tmp2CF,_tmp11B5))));};}}}
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
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp11B8;void*
_tmp11B7;(_tmp11B7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11B8="tq1 real_const not set.",_tag_dyneither(_tmp11B8,
sizeof(char),24))),_tag_dyneither(_tmp11B7,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp11BB;void*_tmp11BA;(_tmp11BA=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11BB="tq2 real_const not set.",
_tag_dyneither(_tmp11BB,sizeof(char),24))),_tag_dyneither(_tmp11BA,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp11BC;Cyc_Tcutil_failure_reason=((_tmp11BC="(qualifiers don't match)",
_tag_dyneither(_tmp11BC,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2DB=x;union Cyc_Absyn_Constraint _tmp2DC;int _tmp2DD;union Cyc_Absyn_Constraint
_tmp2DE;void*_tmp2DF;union Cyc_Absyn_Constraint _tmp2E0;union Cyc_Absyn_Constraint*
_tmp2E1;_LL156: _tmp2DC=*_tmp2DB;if((_tmp2DC.No_constr).tag != 3)goto _LL158;
_tmp2DD=(int)(_tmp2DC.No_constr).val;_LL157:{union Cyc_Absyn_Constraint _tmp11BD;*
x=(((_tmp11BD.Forward_constr).val=y,(((_tmp11BD.Forward_constr).tag=2,_tmp11BD))));}
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
char*_tmp11C0;void*_tmp11BF;(_tmp11BF=0,Cyc_Tcutil_impos(((_tmp11C0="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11C0,sizeof(char),40))),_tag_dyneither(_tmp11BF,sizeof(void*),
0)));}_LL15C:;}_LL15A: _tmp2E0=*_tmp2DB;if((_tmp2E0.Forward_constr).tag != 2)goto
_LL155;_tmp2E1=(union Cyc_Absyn_Constraint*)(_tmp2E0.Forward_constr).val;_LL15B: {
const char*_tmp11C3;void*_tmp11C2;(_tmp11C2=0,Cyc_Tcutil_impos(((_tmp11C3="unify_conref: forward after compress",
_tag_dyneither(_tmp11C3,sizeof(char),37))),_tag_dyneither(_tmp11C2,sizeof(void*),
0)));}_LL155:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2EE;
_push_handler(& _tmp2EE);{int _tmp2F0=0;if(setjmp(_tmp2EE.handler))_tmp2F0=1;if(!
_tmp2F0){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2F1=1;_npop_handler(0);return _tmp2F1;};;
_pop_handler();}else{void*_tmp2EF=(void*)_exn_thrown;void*_tmp2F3=_tmp2EF;_LL164: {
struct Cyc_Tcutil_Unify_struct*_tmp2F4=(struct Cyc_Tcutil_Unify_struct*)_tmp2F3;
if(_tmp2F4->tag != Cyc_Tcutil_Unify)goto _LL166;}_LL165: return 0;_LL166:;_LL167:(
void)_throw(_tmp2F3);_LL163:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11C4;struct _tuple0 _tmp2F6=(_tmp11C4.f1=b1,((_tmp11C4.f2=b2,
_tmp11C4)));void*_tmp2F7;void*_tmp2F9;void*_tmp2FB;void*_tmp2FD;void*_tmp2FF;
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
_tmp11C5;struct _tuple0 _tmp306=(_tmp11C5.f1=b1,((_tmp11C5.f2=b2,_tmp11C5)));void*
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
void*att1,void*att2){struct _tuple0 _tmp11C6;struct _tuple0 _tmp32C=(_tmp11C6.f1=
att1,((_tmp11C6.f2=att2,_tmp11C6)));void*_tmp32D;int _tmp32F;void*_tmp330;int
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
_tmp11C9;struct Cyc_Absyn_ValueofType_struct*_tmp11C8;t=(void*)((_tmp11C8=
_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8[0]=((_tmp11C9.tag=19,((_tmp11C9.f1=Cyc_Absyn_uint_exp(
0,0),_tmp11C9)))),_tmp11C8))));}goto _LL1B4;_LL1BD:;_LL1BE: t=Cyc_Absyn_sint_typ;
goto _LL1B4;_LL1B4:;}{struct _tuple15*_tmp11CA;return(_tmp11CA=_cycalloc(sizeof(*
_tmp11CA)),((_tmp11CA->f1=tv,((_tmp11CA->f2=t,_tmp11CA)))));};}static void*Cyc_Tcutil_rgns_of(
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
_tmp11CD;struct Cyc_Absyn_AccessEff_struct*_tmp11CC;return(void*)((_tmp11CC=
_cycalloc(sizeof(*_tmp11CC)),((_tmp11CC[0]=((_tmp11CD.tag=24,((_tmp11CD.f1=(void*)
t,_tmp11CD)))),_tmp11CC))));}_LL1FB: _tmp39A=*_tmp397;_tmp39B=_tmp39A.kind;if(
_tmp39B != Cyc_Absyn_EffKind)goto _LL1FD;_LL1FC: return t;_LL1FD: _tmp39C=*_tmp397;
_tmp39D=_tmp39C.kind;if(_tmp39D != Cyc_Absyn_IntKind)goto _LL1FF;_LL1FE: return Cyc_Absyn_empty_effect;
_LL1FF:;_LL200: {struct Cyc_Absyn_RgnsEff_struct _tmp11D0;struct Cyc_Absyn_RgnsEff_struct*
_tmp11CF;return(void*)((_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF[0]=((
_tmp11D0.tag=26,((_tmp11D0.f1=(void*)t,_tmp11D0)))),_tmp11CF))));}_LL1F8:;}
_LL1D2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp369=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp35F;if(_tmp369->tag != 16)goto _LL1D4;else{_tmp36A=(void*)_tmp369->f1;}}_LL1D3: {
struct Cyc_Absyn_AccessEff_struct _tmp11D3;struct Cyc_Absyn_AccessEff_struct*
_tmp11D2;return(void*)((_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((
_tmp11D3.tag=24,((_tmp11D3.f1=(void*)_tmp36A,_tmp11D3)))),_tmp11D2))));}_LL1D4: {
struct Cyc_Absyn_DynRgnType_struct*_tmp36B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp35F;if(_tmp36B->tag != 17)goto _LL1D6;else{_tmp36C=(void*)_tmp36B->f1;_tmp36D=(
void*)_tmp36B->f2;}}_LL1D5: {struct Cyc_Absyn_AccessEff_struct _tmp11D6;struct Cyc_Absyn_AccessEff_struct*
_tmp11D5;return(void*)((_tmp11D5=_cycalloc(sizeof(*_tmp11D5)),((_tmp11D5[0]=((
_tmp11D6.tag=24,((_tmp11D6.f1=(void*)_tmp36D,_tmp11D6)))),_tmp11D5))));}_LL1D6: {
struct Cyc_Absyn_DatatypeType_struct*_tmp36E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp35F;if(_tmp36E->tag != 3)goto _LL1D8;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.targs;}}
_LL1D7: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp370);struct Cyc_Absyn_JoinEff_struct
_tmp11D9;struct Cyc_Absyn_JoinEff_struct*_tmp11D8;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8[0]=((_tmp11D9.tag=25,((
_tmp11D9.f1=ts,_tmp11D9)))),_tmp11D8)))));}_LL1D8: {struct Cyc_Absyn_PointerType_struct*
_tmp371=(struct Cyc_Absyn_PointerType_struct*)_tmp35F;if(_tmp371->tag != 5)goto
_LL1DA;else{_tmp372=_tmp371->f1;_tmp373=_tmp372.elt_typ;_tmp374=_tmp372.ptr_atts;
_tmp375=_tmp374.rgn;}}_LL1D9: {struct Cyc_Absyn_JoinEff_struct _tmp11E8;struct Cyc_Absyn_AccessEff_struct*
_tmp11E7;struct Cyc_Absyn_AccessEff_struct _tmp11E6;void*_tmp11E5[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11E4;return Cyc_Tcutil_normalize_effect((void*)((_tmp11E4=_cycalloc(sizeof(*
_tmp11E4)),((_tmp11E4[0]=((_tmp11E8.tag=25,((_tmp11E8.f1=((_tmp11E5[1]=Cyc_Tcutil_rgns_of(
_tmp373),((_tmp11E5[0]=(void*)((_tmp11E7=_cycalloc(sizeof(*_tmp11E7)),((_tmp11E7[
0]=((_tmp11E6.tag=24,((_tmp11E6.f1=(void*)_tmp375,_tmp11E6)))),_tmp11E7)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11E5,sizeof(void*),2)))))),_tmp11E8)))),_tmp11E4)))));}_LL1DA: {struct Cyc_Absyn_ArrayType_struct*
_tmp376=(struct Cyc_Absyn_ArrayType_struct*)_tmp35F;if(_tmp376->tag != 9)goto
_LL1DC;else{_tmp377=_tmp376->f1;_tmp378=_tmp377.elt_type;}}_LL1DB: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp378));_LL1DC: {struct Cyc_Absyn_TupleType_struct*_tmp379=(
struct Cyc_Absyn_TupleType_struct*)_tmp35F;if(_tmp379->tag != 11)goto _LL1DE;else{
_tmp37A=_tmp379->f1;}}_LL1DD: {struct Cyc_List_List*_tmp3AD=0;for(0;_tmp37A != 0;
_tmp37A=_tmp37A->tl){struct Cyc_List_List*_tmp11E9;_tmp3AD=((_tmp11E9=_cycalloc(
sizeof(*_tmp11E9)),((_tmp11E9->hd=(*((struct _tuple11*)_tmp37A->hd)).f2,((
_tmp11E9->tl=_tmp3AD,_tmp11E9))))));}_tmp37D=_tmp3AD;goto _LL1DF;}_LL1DE: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp37B=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp35F;if(_tmp37B->tag != 4)goto _LL1E0;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37C.targs;}}
_LL1DF: _tmp380=_tmp37D;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_AggrType_struct*
_tmp37E=(struct Cyc_Absyn_AggrType_struct*)_tmp35F;if(_tmp37E->tag != 12)goto
_LL1E2;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37F.targs;}}_LL1E1: {struct Cyc_Absyn_JoinEff_struct
_tmp11EC;struct Cyc_Absyn_JoinEff_struct*_tmp11EB;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11EB=_cycalloc(sizeof(*_tmp11EB)),((_tmp11EB[0]=((_tmp11EC.tag=25,((
_tmp11EC.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp380),_tmp11EC)))),_tmp11EB)))));}_LL1E2: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp381=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35F;if(_tmp381->tag != 13)goto _LL1E4;else{_tmp382=_tmp381->f2;}}_LL1E3: {
struct Cyc_Absyn_JoinEff_struct _tmp11EF;struct Cyc_Absyn_JoinEff_struct*_tmp11EE;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((
_tmp11EE[0]=((_tmp11EF.tag=25,((_tmp11EF.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp382),_tmp11EF)))),_tmp11EE)))));}_LL1E4: {struct Cyc_Absyn_TagType_struct*
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
_tmp11F2;struct Cyc_Absyn_JoinEff_struct*_tmp11F1;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1[0]=((_tmp11F2.tag=25,((
_tmp11F2.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp395),_tmp11F2)))),_tmp11F1)))));}_LL1F6: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp396=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp35F;if(_tmp396->tag != 27)goto _LL1BF;}_LL1F7: {const char*_tmp11F5;void*
_tmp11F4;(_tmp11F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11F5="typedecl in rgns_of",_tag_dyneither(_tmp11F5,
sizeof(char),20))),_tag_dyneither(_tmp11F4,sizeof(void*),0)));}_LL1BF:;}void*Cyc_Tcutil_normalize_effect(
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
_LL21B: goto _LL213;_LL21C:;_LL21D:{struct Cyc_List_List*_tmp11F6;effects=((
_tmp11F6=_cycalloc(sizeof(*_tmp11F6)),((_tmp11F6->hd=_tmp3CA,((_tmp11F6->tl=
effects,_tmp11F6))))));}goto _LL213;_LL213:;}}*_tmp3BB=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL204: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3BC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3B8;if(_tmp3BC->tag != 26)goto _LL206;
else{_tmp3BD=(void*)_tmp3BC->f1;}}_LL205: {void*_tmp3D7=Cyc_Tcutil_compress(
_tmp3BD);_LL21F: {struct Cyc_Absyn_Evar_struct*_tmp3D8=(struct Cyc_Absyn_Evar_struct*)
_tmp3D7;if(_tmp3D8->tag != 1)goto _LL221;}_LL220: goto _LL222;_LL221: {struct Cyc_Absyn_VarType_struct*
_tmp3D9=(struct Cyc_Absyn_VarType_struct*)_tmp3D7;if(_tmp3D9->tag != 2)goto _LL223;}
_LL222: return e;_LL223:;_LL224: return Cyc_Tcutil_rgns_of(_tmp3BD);_LL21E:;}_LL206:;
_LL207: return e;_LL201:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp1200;struct Cyc_Core_Opt*_tmp11FF;struct Cyc_Absyn_FnInfo _tmp11FE;struct Cyc_Absyn_FnType_struct*
_tmp11FD;struct Cyc_Absyn_FnType_struct*_tmp3DA=(_tmp11FD=_cycalloc(sizeof(*
_tmp11FD)),((_tmp11FD[0]=((_tmp1200.tag=10,((_tmp1200.f1=((_tmp11FE.tvars=0,((
_tmp11FE.effect=((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF->v=eff,
_tmp11FF)))),((_tmp11FE.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp11FE.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmp11FE.args=0,((_tmp11FE.c_varargs=0,((
_tmp11FE.cyc_varargs=0,((_tmp11FE.rgn_po=0,((_tmp11FE.attributes=0,_tmp11FE)))))))))))))))))),
_tmp1200)))),_tmp11FD)));return Cyc_Absyn_atb_typ((void*)_tmp3DA,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{void*_tmp3DF=Cyc_Tcutil_compress(e);void*_tmp3E1;struct Cyc_List_List*
_tmp3E3;void*_tmp3E5;struct Cyc_Core_Opt*_tmp3E7;struct Cyc_Core_Opt*_tmp3E8;
struct Cyc_Core_Opt**_tmp3E9;struct Cyc_Core_Opt*_tmp3EA;_LL226: {struct Cyc_Absyn_AccessEff_struct*
_tmp3E0=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DF;if(_tmp3E0->tag != 24)goto
_LL228;else{_tmp3E1=(void*)_tmp3E0->f1;}}_LL227: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3E1);_tmp3E1=Cyc_Tcutil_compress(_tmp3E1);if(r == _tmp3E1)return 1;{struct
_tuple0 _tmp1201;struct _tuple0 _tmp3EC=(_tmp1201.f1=r,((_tmp1201.f2=_tmp3E1,
_tmp1201)));void*_tmp3ED;struct Cyc_Absyn_Tvar*_tmp3EF;void*_tmp3F0;struct Cyc_Absyn_Tvar*
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
_check_null(_tmp3FB))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1210;struct Cyc_Absyn_AccessEff_struct*
_tmp120F;struct Cyc_Absyn_AccessEff_struct _tmp120E;void*_tmp120D[2];struct Cyc_Absyn_JoinEff_struct*
_tmp120C;void*_tmp3FD=Cyc_Tcutil_dummy_fntype((void*)((_tmp120C=_cycalloc(
sizeof(*_tmp120C)),((_tmp120C[0]=((_tmp1210.tag=25,((_tmp1210.f1=((_tmp120D[1]=(
void*)((_tmp120F=_cycalloc(sizeof(*_tmp120F)),((_tmp120F[0]=((_tmp120E.tag=24,((
_tmp120E.f1=(void*)r,_tmp120E)))),_tmp120F)))),((_tmp120D[0]=_tmp3FC,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp120D,sizeof(void*),2)))))),
_tmp1210)))),_tmp120C)))));{struct Cyc_Core_Opt*_tmp1211;*_tmp3FA=((_tmp1211=
_cycalloc(sizeof(*_tmp1211)),((_tmp1211->v=_tmp3FD,_tmp1211))));}return 1;};}
_LL23D:;_LL23E: return 0;_LL23A:;};_LL238:;_LL239: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3F3);_LL235:;}_LL22C: {struct Cyc_Absyn_Evar_struct*_tmp3E6=(
struct Cyc_Absyn_Evar_struct*)_tmp3DF;if(_tmp3E6->tag != 1)goto _LL22E;else{_tmp3E7=
_tmp3E6->f1;_tmp3E8=_tmp3E6->f2;_tmp3E9=(struct Cyc_Core_Opt**)& _tmp3E6->f2;
_tmp3EA=_tmp3E6->f4;}}_LL22D: if(_tmp3E7 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E7->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp1214;void*_tmp1213;(_tmp1213=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1214="effect evar has wrong kind",
_tag_dyneither(_tmp1214,sizeof(char),27))),_tag_dyneither(_tmp1213,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp406=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3EA);Cyc_Tcutil_occurs(_tmp406,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3EA))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp1229;struct Cyc_List_List*_tmp1228;struct Cyc_Absyn_AccessEff_struct _tmp1227;
struct Cyc_Absyn_AccessEff_struct*_tmp1226;struct Cyc_List_List*_tmp1225;struct Cyc_Absyn_JoinEff_struct*
_tmp1224;struct Cyc_Absyn_JoinEff_struct*_tmp407=(_tmp1224=_cycalloc(sizeof(*
_tmp1224)),((_tmp1224[0]=((_tmp1229.tag=25,((_tmp1229.f1=((_tmp1225=_cycalloc(
sizeof(*_tmp1225)),((_tmp1225->hd=_tmp406,((_tmp1225->tl=((_tmp1228=_cycalloc(
sizeof(*_tmp1228)),((_tmp1228->hd=(void*)((_tmp1226=_cycalloc(sizeof(*_tmp1226)),((
_tmp1226[0]=((_tmp1227.tag=24,((_tmp1227.f1=(void*)r,_tmp1227)))),_tmp1226)))),((
_tmp1228->tl=0,_tmp1228)))))),_tmp1225)))))),_tmp1229)))),_tmp1224)));{struct Cyc_Core_Opt*
_tmp122A;*_tmp3E9=((_tmp122A=_cycalloc(sizeof(*_tmp122A)),((_tmp122A->v=(void*)
_tmp407,_tmp122A))));}return 1;};};_LL22E:;_LL22F: return 0;_LL225:;};}static int Cyc_Tcutil_type_in_effect(
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
else{_tmp41C=(void*)_tmp41B->f1;}}_LL24C: {struct _tuple0 _tmp122B;struct _tuple0
_tmp41E=(_tmp122B.f1=Cyc_Tcutil_compress(_tmp41C),((_tmp122B.f2=_tmp414,_tmp122B)));
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
_tmp416->v)->kind != Cyc_Absyn_EffKind){const char*_tmp122E;void*_tmp122D;(
_tmp122D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp122E="effect evar has wrong kind",_tag_dyneither(_tmp122E,sizeof(char),27))),
_tag_dyneither(_tmp122D,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp427=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp419);
Cyc_Tcutil_occurs(_tmp427,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp419))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1243;struct Cyc_List_List*
_tmp1242;struct Cyc_Absyn_RgnsEff_struct _tmp1241;struct Cyc_Absyn_RgnsEff_struct*
_tmp1240;struct Cyc_List_List*_tmp123F;struct Cyc_Absyn_JoinEff_struct*_tmp123E;
struct Cyc_Absyn_JoinEff_struct*_tmp428=(_tmp123E=_cycalloc(sizeof(*_tmp123E)),((
_tmp123E[0]=((_tmp1243.tag=25,((_tmp1243.f1=((_tmp123F=_cycalloc(sizeof(*
_tmp123F)),((_tmp123F->hd=_tmp427,((_tmp123F->tl=((_tmp1242=_cycalloc(sizeof(*
_tmp1242)),((_tmp1242->hd=(void*)((_tmp1240=_cycalloc(sizeof(*_tmp1240)),((
_tmp1240[0]=((_tmp1241.tag=26,((_tmp1241.f1=(void*)t,_tmp1241)))),_tmp1240)))),((
_tmp1242->tl=0,_tmp1242)))))),_tmp123F)))))),_tmp1243)))),_tmp123E)));{struct Cyc_Core_Opt*
_tmp1244;*_tmp418=((_tmp1244=_cycalloc(sizeof(*_tmp1244)),((_tmp1244->v=(void*)
_tmp428,_tmp1244))));}return 1;};};_LL248:;_LL249: return 0;_LL23F:;};}static int Cyc_Tcutil_variable_in_effect(
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
struct Cyc_Absyn_JoinEff_struct _tmp1253;struct Cyc_Absyn_VarType_struct*_tmp1252;
struct Cyc_Absyn_VarType_struct _tmp1251;void*_tmp1250[2];struct Cyc_Absyn_JoinEff_struct*
_tmp124F;void*_tmp446=Cyc_Tcutil_dummy_fntype((void*)((_tmp124F=_cycalloc(
sizeof(*_tmp124F)),((_tmp124F[0]=((_tmp1253.tag=25,((_tmp1253.f1=((_tmp1250[1]=(
void*)((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((_tmp1252[0]=((_tmp1251.tag=2,((
_tmp1251.f1=v,_tmp1251)))),_tmp1252)))),((_tmp1250[0]=_tmp445,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1250,sizeof(void*),2)))))),
_tmp1253)))),_tmp124F)))));{struct Cyc_Core_Opt*_tmp1254;*_tmp443=((_tmp1254=
_cycalloc(sizeof(*_tmp1254)),((_tmp1254->v=_tmp446,_tmp1254))));}return 1;};}
_LL267:;_LL268: return 0;_LL264:;};_LL262:;_LL263: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp43C);_LL25F:;}_LL25B: {struct Cyc_Absyn_Evar_struct*
_tmp437=(struct Cyc_Absyn_Evar_struct*)_tmp430;if(_tmp437->tag != 1)goto _LL25D;
else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;_tmp43A=(struct Cyc_Core_Opt**)&
_tmp437->f2;_tmp43B=_tmp437->f4;}}_LL25C: if(_tmp438 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp438->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1257;void*_tmp1256;(
_tmp1256=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1257="effect evar has wrong kind",_tag_dyneither(_tmp1257,sizeof(char),27))),
_tag_dyneither(_tmp1256,sizeof(void*),0)));}{void*_tmp44F=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp43B);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp43B))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp126C;
struct Cyc_List_List*_tmp126B;struct Cyc_Absyn_VarType_struct _tmp126A;struct Cyc_Absyn_VarType_struct*
_tmp1269;struct Cyc_List_List*_tmp1268;struct Cyc_Absyn_JoinEff_struct*_tmp1267;
struct Cyc_Absyn_JoinEff_struct*_tmp450=(_tmp1267=_cycalloc(sizeof(*_tmp1267)),((
_tmp1267[0]=((_tmp126C.tag=25,((_tmp126C.f1=((_tmp1268=_cycalloc(sizeof(*
_tmp1268)),((_tmp1268->hd=_tmp44F,((_tmp1268->tl=((_tmp126B=_cycalloc(sizeof(*
_tmp126B)),((_tmp126B->hd=(void*)((_tmp1269=_cycalloc(sizeof(*_tmp1269)),((
_tmp1269[0]=((_tmp126A.tag=2,((_tmp126A.f1=v,_tmp126A)))),_tmp1269)))),((
_tmp126B->tl=0,_tmp126B)))))),_tmp1268)))))),_tmp126C)))),_tmp1267)));{struct Cyc_Core_Opt*
_tmp126D;*_tmp43A=((_tmp126D=_cycalloc(sizeof(*_tmp126D)),((_tmp126D->v=(void*)
_tmp450,_tmp126D))));}return 1;};};_LL25D:;_LL25E: return 0;_LL254:;};}static int Cyc_Tcutil_evar_in_effect(
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
e1,e2)){struct Cyc_Core_Opt*_tmp126E;*_tmp46C=((_tmp126E=_cycalloc(sizeof(*
_tmp126E)),((_tmp126E->v=Cyc_Absyn_empty_effect,_tmp126E))));}return 1;_LL282:;
_LL283: {const char*_tmp1272;void*_tmp1271[1];struct Cyc_String_pa_struct _tmp1270;(
_tmp1270.tag=0,((_tmp1270.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1271[0]=& _tmp1270,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1272="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1272,sizeof(char),30))),_tag_dyneither(_tmp1271,sizeof(void*),
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
Cyc_Core_Opt*_tmp1273;*_tmp481=((_tmp1273=_cycalloc(sizeof(*_tmp1273)),((
_tmp1273->v=t2,_tmp1273))));}return;}else{{void*_tmp485=t2;struct Cyc_Core_Opt*
_tmp487;struct Cyc_Core_Opt**_tmp488;struct Cyc_Core_Opt*_tmp489;struct Cyc_Absyn_PtrInfo
_tmp48B;_LL28F: {struct Cyc_Absyn_Evar_struct*_tmp486=(struct Cyc_Absyn_Evar_struct*)
_tmp485;if(_tmp486->tag != 1)goto _LL291;else{_tmp487=_tmp486->f2;_tmp488=(struct
Cyc_Core_Opt**)& _tmp486->f2;_tmp489=_tmp486->f4;}}_LL290: {struct Cyc_List_List*
_tmp48C=(struct Cyc_List_List*)_tmp482->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp489))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp48C,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1274;Cyc_Tcutil_failure_reason=((
_tmp1274="(type variable would escape scope)",_tag_dyneither(_tmp1274,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp47F->v,_tmp483)){{struct Cyc_Core_Opt*_tmp1275;*_tmp488=((
_tmp1275=_cycalloc(sizeof(*_tmp1275)),((_tmp1275->v=t1,_tmp1275))));}return;}{
const char*_tmp1276;Cyc_Tcutil_failure_reason=((_tmp1276="(kinds are incompatible)",
_tag_dyneither(_tmp1276,sizeof(char),25)));}goto _LL28E;}_LL291:{struct Cyc_Absyn_PointerType_struct*
_tmp48A=(struct Cyc_Absyn_PointerType_struct*)_tmp485;if(_tmp48A->tag != 5)goto
_LL293;else{_tmp48B=_tmp48A->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp47F->v)->kind
== Cyc_Absyn_BoxKind))goto _LL293;_LL292: {union Cyc_Absyn_Constraint*_tmp490=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp48B.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp491=_tmp490;union Cyc_Absyn_Constraint
_tmp492;int _tmp493;_LL296: _tmp492=*_tmp491;if((_tmp492.No_constr).tag != 3)goto
_LL298;_tmp493=(int)(_tmp492.No_constr).val;_LL297:{struct
_union_Constraint_Eq_constr*_tmp1277;(_tmp1277=& _tmp490->Eq_constr,((_tmp1277->tag=
1,_tmp1277->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp1278;*_tmp481=((
_tmp1278=_cycalloc(sizeof(*_tmp1278)),((_tmp1278->v=t2,_tmp1278))));}return;
_LL298:;_LL299: goto _LL295;_LL295:;}goto _LL28E;}_LL293:;_LL294: goto _LL28E;_LL28E:;}{
const char*_tmp1279;Cyc_Tcutil_failure_reason=((_tmp1279="(kinds are incompatible)",
_tag_dyneither(_tmp1279,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL28C:;_LL28D: goto _LL289;_LL289:;}{struct _tuple0 _tmp127A;struct _tuple0 _tmp498=(
_tmp127A.f1=t2,((_tmp127A.f2=t1,_tmp127A)));void*_tmp499;void*_tmp49B;void*
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
return;{const char*_tmp127B;Cyc_Tcutil_failure_reason=((_tmp127B="(variable types are not the same)",
_tag_dyneither(_tmp127B,sizeof(char),34)));}goto _LL29A;}_LL2A1: _tmp4A5=_tmp498.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4A6=(struct Cyc_Absyn_AggrType_struct*)
_tmp4A5;if(_tmp4A6->tag != 12)goto _LL2A3;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A7.aggr_info;
_tmp4A9=_tmp4A7.targs;}};_tmp4AA=_tmp498.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4AB=(struct Cyc_Absyn_AggrType_struct*)_tmp4AA;if(_tmp4AB->tag != 12)goto
_LL2A3;else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AC.aggr_info;_tmp4AE=_tmp4AC.targs;}};
_LL2A2: {enum Cyc_Absyn_AggrKind _tmp56B;struct _tuple2*_tmp56C;struct _tuple12
_tmp56A=Cyc_Absyn_aggr_kinded_name(_tmp4AD);_tmp56B=_tmp56A.f1;_tmp56C=_tmp56A.f2;{
enum Cyc_Absyn_AggrKind _tmp56E;struct _tuple2*_tmp56F;struct _tuple12 _tmp56D=Cyc_Absyn_aggr_kinded_name(
_tmp4A8);_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;if(_tmp56B != _tmp56E){{const char*
_tmp127C;Cyc_Tcutil_failure_reason=((_tmp127C="(struct and union type)",
_tag_dyneither(_tmp127C,sizeof(char),24)));}goto _LL29A;}if(Cyc_Absyn_qvar_cmp(
_tmp56C,_tmp56F)!= 0){{const char*_tmp127D;Cyc_Tcutil_failure_reason=((_tmp127D="(different type name)",
_tag_dyneither(_tmp127D,sizeof(char),22)));}goto _LL29A;}Cyc_Tcutil_unify_list(
_tmp4AE,_tmp4A9);return;};}_LL2A3: _tmp4AF=_tmp498.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp4B0=(struct Cyc_Absyn_EnumType_struct*)_tmp4AF;if(_tmp4B0->tag != 14)goto
_LL2A5;else{_tmp4B1=_tmp4B0->f1;}};_tmp4B2=_tmp498.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp4B3=(struct Cyc_Absyn_EnumType_struct*)_tmp4B2;if(_tmp4B3->tag != 14)goto
_LL2A5;else{_tmp4B4=_tmp4B3->f1;}};_LL2A4: if(Cyc_Absyn_qvar_cmp(_tmp4B1,_tmp4B4)
== 0)return;{const char*_tmp127E;Cyc_Tcutil_failure_reason=((_tmp127E="(different enum types)",
_tag_dyneither(_tmp127E,sizeof(char),23)));}goto _LL29A;_LL2A5: _tmp4B5=_tmp498.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp4B6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B5;if(_tmp4B6->tag != 15)goto _LL2A7;else{_tmp4B7=_tmp4B6->f1;}};_tmp4B8=
_tmp498.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4B9=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B8;if(_tmp4B9->tag != 15)goto _LL2A7;else{_tmp4BA=_tmp4B9->f1;}};_LL2A6: {int
bad=0;for(0;_tmp4B7 != 0  && _tmp4BA != 0;(_tmp4B7=_tmp4B7->tl,_tmp4BA=_tmp4BA->tl)){
struct Cyc_Absyn_Enumfield*_tmp573=(struct Cyc_Absyn_Enumfield*)_tmp4B7->hd;struct
Cyc_Absyn_Enumfield*_tmp574=(struct Cyc_Absyn_Enumfield*)_tmp4BA->hd;if(Cyc_Absyn_qvar_cmp(
_tmp573->name,_tmp574->name)!= 0){{const char*_tmp127F;Cyc_Tcutil_failure_reason=((
_tmp127F="(different names for enum fields)",_tag_dyneither(_tmp127F,sizeof(char),
34)));}bad=1;break;}if(_tmp573->tag == _tmp574->tag)continue;if(_tmp573->tag == 0
 || _tmp574->tag == 0){{const char*_tmp1280;Cyc_Tcutil_failure_reason=((_tmp1280="(different tag values for enum fields)",
_tag_dyneither(_tmp1280,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp573->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp574->tag))){{const char*_tmp1281;Cyc_Tcutil_failure_reason=((_tmp1281="(different tag values for enum fields)",
_tag_dyneither(_tmp1281,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL29A;if(
_tmp4B7 == 0  && _tmp4BA == 0)return;{const char*_tmp1282;Cyc_Tcutil_failure_reason=((
_tmp1282="(different number of fields for enums)",_tag_dyneither(_tmp1282,
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
_tmp1283;Cyc_Tcutil_failure_reason=((_tmp1283="(different datatype field types)",
_tag_dyneither(_tmp1283,sizeof(char),33)));}goto _LL29A;_LL2AB: _tmp4D9=_tmp498.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4DA=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D9;if(_tmp4DA->tag != 5)goto _LL2AD;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DB.elt_typ;
_tmp4DD=_tmp4DB.elt_tq;_tmp4DE=_tmp4DB.ptr_atts;_tmp4DF=_tmp4DE.rgn;_tmp4E0=
_tmp4DE.nullable;_tmp4E1=_tmp4DE.bounds;_tmp4E2=_tmp4DE.zero_term;}};_tmp4E3=
_tmp498.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4E4=(struct Cyc_Absyn_PointerType_struct*)
_tmp4E3;if(_tmp4E4->tag != 5)goto _LL2AD;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E5.elt_typ;
_tmp4E7=_tmp4E5.elt_tq;_tmp4E8=_tmp4E5.ptr_atts;_tmp4E9=_tmp4E8.rgn;_tmp4EA=
_tmp4E8.nullable;_tmp4EB=_tmp4E8.bounds;_tmp4EC=_tmp4E8.zero_term;}};_LL2AC: Cyc_Tcutil_unify_it(
_tmp4E6,_tmp4DC);Cyc_Tcutil_unify_it(_tmp4DF,_tmp4E9);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1284;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4EC,_tmp4E2,((_tmp1284="(not both zero terminated)",
_tag_dyneither(_tmp1284,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4E7,
_tmp4E6,_tmp4DD,_tmp4DC);{const char*_tmp1285;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4EB,_tmp4E1,((
_tmp1285="(different pointer bounds)",_tag_dyneither(_tmp1285,sizeof(char),27))));}{
void*_tmp57C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4EB);_LL2D8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp57D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp57C;if(_tmp57D->tag != 0)goto
_LL2DA;}_LL2D9: return;_LL2DA:;_LL2DB: goto _LL2D7;_LL2D7:;}{const char*_tmp1286;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4EA,_tmp4E0,((_tmp1286="(incompatible pointer types)",_tag_dyneither(
_tmp1286,sizeof(char),29))));}return;_LL2AD: _tmp4ED=_tmp498.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4EE=(struct Cyc_Absyn_IntType_struct*)_tmp4ED;if(_tmp4EE->tag != 6)goto _LL2AF;
else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}};_tmp4F1=_tmp498.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4F2=(struct Cyc_Absyn_IntType_struct*)_tmp4F1;if(_tmp4F2->tag != 6)goto _LL2AF;
else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;}};_LL2AE: if(_tmp4F3 == _tmp4EF  && ((
_tmp4F4 == _tmp4F0  || _tmp4F4 == Cyc_Absyn_Int_sz  && _tmp4F0 == Cyc_Absyn_Long_sz)
 || _tmp4F4 == Cyc_Absyn_Long_sz  && _tmp4F0 == Cyc_Absyn_Int_sz))return;{const char*
_tmp1287;Cyc_Tcutil_failure_reason=((_tmp1287="(different integral types)",
_tag_dyneither(_tmp1287,sizeof(char),27)));}goto _LL29A;_LL2AF: _tmp4F5=_tmp498.f1;{
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
Cyc_Evexp_same_const_exp(_tmp507,_tmp50A)){{const char*_tmp1288;Cyc_Tcutil_failure_reason=((
_tmp1288="(cannot prove expressions are the same)",_tag_dyneither(_tmp1288,
sizeof(char),40)));}goto _LL29A;}return;_LL2B7: _tmp50B=_tmp498.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp50C=(struct Cyc_Absyn_ArrayType_struct*)_tmp50B;if(_tmp50C->tag != 9)goto
_LL2B9;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.elt_type;_tmp50F=_tmp50D.tq;
_tmp510=_tmp50D.num_elts;_tmp511=_tmp50D.zero_term;}};_tmp512=_tmp498.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp513=(struct Cyc_Absyn_ArrayType_struct*)_tmp512;
if(_tmp513->tag != 9)goto _LL2B9;else{_tmp514=_tmp513->f1;_tmp515=_tmp514.elt_type;
_tmp516=_tmp514.tq;_tmp517=_tmp514.num_elts;_tmp518=_tmp514.zero_term;}};_LL2B8:
Cyc_Tcutil_unify_it(_tmp515,_tmp50E);Cyc_Tcutil_unify_tqual(_tmp516,_tmp515,
_tmp50F,_tmp50E);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp1289;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp511,_tmp518,((_tmp1289="(not both zero terminated)",_tag_dyneither(_tmp1289,
sizeof(char),27))));}if(_tmp510 == _tmp517)return;if(_tmp510 == 0  || _tmp517 == 0)
goto _LL29A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp510,(struct Cyc_Absyn_Exp*)
_tmp517))return;{const char*_tmp128A;Cyc_Tcutil_failure_reason=((_tmp128A="(different array sizes)",
_tag_dyneither(_tmp128A,sizeof(char),24)));}goto _LL29A;_LL2B9: _tmp519=_tmp498.f1;{
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
_tmp528 != 0){if(_tmp51C == 0){{const char*_tmp128B;Cyc_Tcutil_failure_reason=((
_tmp128B="(second function type has too few type variables)",_tag_dyneither(
_tmp128B,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp585=((struct Cyc_Absyn_Tvar*)_tmp528->hd)->kind;void*_tmp586=((struct Cyc_Absyn_Tvar*)
_tmp51C->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp585,_tmp586)){{const char*
_tmp1291;void*_tmp1290[3];struct Cyc_String_pa_struct _tmp128F;struct Cyc_String_pa_struct
_tmp128E;struct Cyc_String_pa_struct _tmp128D;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp128D.tag=0,((_tmp128D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp51C->hd,& Cyc_Tcutil_bk))),((_tmp128E.tag=0,((_tmp128E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp528->hd,& Cyc_Tcutil_bk))),((_tmp128F.tag=0,((_tmp128F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp528->hd)),((_tmp1290[0]=& _tmp128F,((_tmp1290[1]=& _tmp128E,((_tmp1290[2]=&
_tmp128D,Cyc_aprintf(((_tmp1291="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1291,sizeof(char),44))),_tag_dyneither(_tmp1290,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp129B;struct Cyc_Absyn_VarType_struct _tmp129A;struct Cyc_Absyn_VarType_struct*
_tmp1299;struct Cyc_List_List*_tmp1298;inst=((_tmp1298=_region_malloc(rgn,sizeof(*
_tmp1298)),((_tmp1298->hd=((_tmp129B=_region_malloc(rgn,sizeof(*_tmp129B)),((
_tmp129B->f1=(struct Cyc_Absyn_Tvar*)_tmp51C->hd,((_tmp129B->f2=(void*)((_tmp1299=
_cycalloc(sizeof(*_tmp1299)),((_tmp1299[0]=((_tmp129A.tag=2,((_tmp129A.f1=(
struct Cyc_Absyn_Tvar*)_tmp528->hd,_tmp129A)))),_tmp1299)))),_tmp129B)))))),((
_tmp1298->tl=inst,_tmp1298))))));}_tmp528=_tmp528->tl;_tmp51C=_tmp51C->tl;};}if(
_tmp51C != 0){{const char*_tmp129C;Cyc_Tcutil_failure_reason=((_tmp129C="(second function type has too many type variables)",
_tag_dyneither(_tmp129C,sizeof(char),51)));}_npop_handler(0);goto _LL29A;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp12A8;struct Cyc_Absyn_FnInfo _tmp12A7;
struct Cyc_Absyn_FnType_struct*_tmp12A6;struct Cyc_Absyn_FnType_struct _tmp12A2;
struct Cyc_Absyn_FnInfo _tmp12A1;struct Cyc_Absyn_FnType_struct*_tmp12A0;Cyc_Tcutil_unify_it((
void*)((_tmp12A0=_cycalloc(sizeof(*_tmp12A0)),((_tmp12A0[0]=((_tmp12A2.tag=10,((
_tmp12A2.f1=((_tmp12A1.tvars=0,((_tmp12A1.effect=_tmp529,((_tmp12A1.ret_tqual=
_tmp52A,((_tmp12A1.ret_typ=_tmp52B,((_tmp12A1.args=_tmp52C,((_tmp12A1.c_varargs=
_tmp52D,((_tmp12A1.cyc_varargs=_tmp52E,((_tmp12A1.rgn_po=_tmp52F,((_tmp12A1.attributes=
_tmp530,_tmp12A1)))))))))))))))))),_tmp12A2)))),_tmp12A0)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp12A6=_cycalloc(sizeof(*_tmp12A6)),((_tmp12A6[0]=((_tmp12A8.tag=
10,((_tmp12A8.f1=((_tmp12A7.tvars=0,((_tmp12A7.effect=_tmp51D,((_tmp12A7.ret_tqual=
_tmp51E,((_tmp12A7.ret_typ=_tmp51F,((_tmp12A7.args=_tmp520,((_tmp12A7.c_varargs=
_tmp521,((_tmp12A7.cyc_varargs=_tmp522,((_tmp12A7.rgn_po=_tmp523,((_tmp12A7.attributes=
_tmp524,_tmp12A7)))))))))))))))))),_tmp12A8)))),_tmp12A6))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp52B,_tmp51F);Cyc_Tcutil_unify_tqual(
_tmp52A,_tmp52B,_tmp51E,_tmp51F);for(0;_tmp52C != 0  && _tmp520 != 0;(_tmp52C=
_tmp52C->tl,_tmp520=_tmp520->tl)){struct Cyc_Absyn_Tqual _tmp598;void*_tmp599;
struct _tuple9 _tmp597=*((struct _tuple9*)_tmp52C->hd);_tmp598=_tmp597.f2;_tmp599=
_tmp597.f3;{struct Cyc_Absyn_Tqual _tmp59B;void*_tmp59C;struct _tuple9 _tmp59A=*((
struct _tuple9*)_tmp520->hd);_tmp59B=_tmp59A.f2;_tmp59C=_tmp59A.f3;Cyc_Tcutil_unify_it(
_tmp599,_tmp59C);Cyc_Tcutil_unify_tqual(_tmp598,_tmp599,_tmp59B,_tmp59C);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp52C != 0  || _tmp520 != 0){{const char*_tmp12A9;
Cyc_Tcutil_failure_reason=((_tmp12A9="(function types have different number of arguments)",
_tag_dyneither(_tmp12A9,sizeof(char),52)));}_npop_handler(0);goto _LL29A;}if(
_tmp52D != _tmp521){{const char*_tmp12AA;Cyc_Tcutil_failure_reason=((_tmp12AA="(only one function type takes C varargs)",
_tag_dyneither(_tmp12AA,sizeof(char),41)));}_npop_handler(0);goto _LL29A;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp12AB;struct _tuple21 _tmp5A0=(_tmp12AB.f1=
_tmp52E,((_tmp12AB.f2=_tmp522,_tmp12AB)));struct Cyc_Absyn_VarargInfo*_tmp5A1;
struct Cyc_Absyn_VarargInfo*_tmp5A2;struct Cyc_Absyn_VarargInfo*_tmp5A3;struct Cyc_Absyn_VarargInfo*
_tmp5A4;struct Cyc_Absyn_VarargInfo*_tmp5A5;struct Cyc_Absyn_VarargInfo _tmp5A6;
struct Cyc_Core_Opt*_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A9;int _tmp5AA;
struct Cyc_Absyn_VarargInfo*_tmp5AB;struct Cyc_Absyn_VarargInfo _tmp5AC;struct Cyc_Core_Opt*
_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AE;void*_tmp5AF;int _tmp5B0;_LL2DD: _tmp5A1=
_tmp5A0.f1;if(_tmp5A1 != 0)goto _LL2DF;_tmp5A2=_tmp5A0.f2;if(_tmp5A2 != 0)goto
_LL2DF;_LL2DE: goto _LL2DC;_LL2DF: _tmp5A3=_tmp5A0.f1;if(_tmp5A3 != 0)goto _LL2E1;
_LL2E0: goto _LL2E2;_LL2E1: _tmp5A4=_tmp5A0.f2;if(_tmp5A4 != 0)goto _LL2E3;_LL2E2:
bad_cyc_vararg=1;{const char*_tmp12AC;Cyc_Tcutil_failure_reason=((_tmp12AC="(only one function type takes varargs)",
_tag_dyneither(_tmp12AC,sizeof(char),39)));}goto _LL2DC;_LL2E3: _tmp5A5=_tmp5A0.f1;
if(_tmp5A5 == 0)goto _LL2DC;_tmp5A6=*_tmp5A5;_tmp5A7=_tmp5A6.name;_tmp5A8=_tmp5A6.tq;
_tmp5A9=_tmp5A6.type;_tmp5AA=_tmp5A6.inject;_tmp5AB=_tmp5A0.f2;if(_tmp5AB == 0)
goto _LL2DC;_tmp5AC=*_tmp5AB;_tmp5AD=_tmp5AC.name;_tmp5AE=_tmp5AC.tq;_tmp5AF=
_tmp5AC.type;_tmp5B0=_tmp5AC.inject;_LL2E4: Cyc_Tcutil_unify_it(_tmp5A9,_tmp5AF);
Cyc_Tcutil_unify_tqual(_tmp5A8,_tmp5A9,_tmp5AE,_tmp5AF);if(_tmp5AA != _tmp5B0){
bad_cyc_vararg=1;{const char*_tmp12AD;Cyc_Tcutil_failure_reason=((_tmp12AD="(only one function type injects varargs)",
_tag_dyneither(_tmp12AD,sizeof(char),41)));};}goto _LL2DC;_LL2DC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL29A;}{int bad_effect=0;{struct _tuple22
_tmp12AE;struct _tuple22 _tmp5B4=(_tmp12AE.f1=_tmp529,((_tmp12AE.f2=_tmp51D,
_tmp12AE)));struct Cyc_Core_Opt*_tmp5B5;struct Cyc_Core_Opt*_tmp5B6;struct Cyc_Core_Opt*
_tmp5B7;struct Cyc_Core_Opt*_tmp5B8;_LL2E6: _tmp5B5=_tmp5B4.f1;if(_tmp5B5 != 0)goto
_LL2E8;_tmp5B6=_tmp5B4.f2;if(_tmp5B6 != 0)goto _LL2E8;_LL2E7: goto _LL2E5;_LL2E8:
_tmp5B7=_tmp5B4.f1;if(_tmp5B7 != 0)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp5B8=
_tmp5B4.f2;if(_tmp5B8 != 0)goto _LL2EC;_LL2EB: bad_effect=1;goto _LL2E5;_LL2EC:;
_LL2ED: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp529))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp51D))->v);
goto _LL2E5;_LL2E5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp12AF;Cyc_Tcutil_failure_reason=((_tmp12AF="(function type effects do not unify)",
_tag_dyneither(_tmp12AF,sizeof(char),37)));}_npop_handler(0);goto _LL29A;}if(!Cyc_Tcutil_same_atts(
_tmp524,_tmp530)){{const char*_tmp12B0;Cyc_Tcutil_failure_reason=((_tmp12B0="(function types have different attributes)",
_tag_dyneither(_tmp12B0,sizeof(char),43)));}_npop_handler(0);goto _LL29A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp523,_tmp52F)){{const char*_tmp12B1;Cyc_Tcutil_failure_reason=((_tmp12B1="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp12B1,sizeof(char),58)));}_npop_handler(0);goto _LL29A;}
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
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp12B2;Cyc_Tcutil_failure_reason=((
_tmp12B2="(tuple types have different numbers of components)",_tag_dyneither(
_tmp12B2,sizeof(char),51)));}goto _LL29A;_LL2BD: _tmp537=_tmp498.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp538=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp537;if(_tmp538->tag != 13)goto
_LL2BF;else{_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;}};_tmp53B=_tmp498.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp53C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp53B;if(_tmp53C->tag != 13)goto _LL2BF;else{_tmp53D=_tmp53C->f1;_tmp53E=_tmp53C->f2;}};
_LL2BE: if(_tmp53D != _tmp539){{const char*_tmp12B3;Cyc_Tcutil_failure_reason=((
_tmp12B3="(struct and union type)",_tag_dyneither(_tmp12B3,sizeof(char),24)));}
goto _LL29A;}for(0;_tmp53E != 0  && _tmp53A != 0;(_tmp53E=_tmp53E->tl,_tmp53A=
_tmp53A->tl)){struct Cyc_Absyn_Aggrfield*_tmp5C4=(struct Cyc_Absyn_Aggrfield*)
_tmp53E->hd;struct Cyc_Absyn_Aggrfield*_tmp5C5=(struct Cyc_Absyn_Aggrfield*)
_tmp53A->hd;if(Cyc_strptrcmp(_tmp5C4->name,_tmp5C5->name)!= 0){{const char*
_tmp12B4;Cyc_Tcutil_failure_reason=((_tmp12B4="(different member names)",
_tag_dyneither(_tmp12B4,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5C4->type,_tmp5C5->type);Cyc_Tcutil_unify_tqual(_tmp5C4->tq,
_tmp5C4->type,_tmp5C5->tq,_tmp5C5->type);if(!Cyc_Tcutil_same_atts(_tmp5C4->attributes,
_tmp5C5->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp12B5;Cyc_Tcutil_failure_reason=((_tmp12B5="(different attributes on member)",
_tag_dyneither(_tmp12B5,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5C4->width != 0  && _tmp5C5->width == 0  || _tmp5C5->width != 0  && _tmp5C4->width
== 0) || (_tmp5C4->width != 0  && _tmp5C5->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5C4->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5C5->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp12B6;Cyc_Tcutil_failure_reason=((_tmp12B6="(different bitfield widths on member)",
_tag_dyneither(_tmp12B6,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp53E == 0  && _tmp53A == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp12B7;Cyc_Tcutil_failure_reason=((_tmp12B7="(different number of members)",
_tag_dyneither(_tmp12B7,sizeof(char),30)));}goto _LL29A;_LL2BF: _tmp53F=_tmp498.f1;{
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
_LL2D5;};_LL2D4: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmp12B8;Cyc_Tcutil_failure_reason=((
_tmp12B8="(effects don't unify)",_tag_dyneither(_tmp12B8,sizeof(char),22)));}
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
_tmp5D9.Forward_constr).val;_LL2FB: {const char*_tmp12BB;void*_tmp12BA;(_tmp12BA=
0,Cyc_Tcutil_impos(((_tmp12BB="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp12BB,sizeof(char),40))),_tag_dyneither(_tmp12BA,sizeof(void*),
0)));}_LL2F5:;}_LL2F3: _tmp5D2=*_tmp5CD;if((_tmp5D2.Forward_constr).tag != 2)goto
_LL2EE;_tmp5D3=(union Cyc_Absyn_Constraint*)(_tmp5D2.Forward_constr).val;_LL2F4: {
const char*_tmp12BE;void*_tmp12BD;(_tmp12BD=0,Cyc_Tcutil_impos(((_tmp12BE="unify_conref: forward after compress",
_tag_dyneither(_tmp12BE,sizeof(char),37))),_tag_dyneither(_tmp12BD,sizeof(void*),
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
return _tmp60A;{struct _tuple0 _tmp12BF;struct _tuple0 _tmp60C=(_tmp12BF.f1=t2,((
_tmp12BF.f2=t1,_tmp12BF)));void*_tmp60D;void*_tmp60F;void*_tmp611;struct Cyc_Absyn_Tvar*
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
const char*_tmp12C2;void*_tmp12C1;(_tmp12C1=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C2="typecmp: can only compare closed types",
_tag_dyneither(_tmp12C2,sizeof(char),39))),_tag_dyneither(_tmp12C1,sizeof(void*),
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
char*_tmp12C5;void*_tmp12C4;(_tmp12C4=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C5="missing expression in array index",
_tag_dyneither(_tmp12C5,sizeof(char),34))),_tag_dyneither(_tmp12C4,sizeof(void*),
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
_tmp68D.rgn_po;_tmp696=_tmp68D.attributes;}};_LL34F: {const char*_tmp12C8;void*
_tmp12C7;(_tmp12C7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C8="typecmp: function types not handled",
_tag_dyneither(_tmp12C8,sizeof(char),36))),_tag_dyneither(_tmp12C7,sizeof(void*),
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
_tmp6C9;if(_tmp6CA->tag != 24)goto _LL368;};_LL367: {const char*_tmp12CB;void*
_tmp12CA;(_tmp12CA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12CB="typecmp: effects not handled",_tag_dyneither(
_tmp12CB,sizeof(char),29))),_tag_dyneither(_tmp12CA,sizeof(void*),0)));}_LL368:;
_LL369: {const char*_tmp12CE;void*_tmp12CD;(_tmp12CD=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CE="Unmatched case in typecmp",
_tag_dyneither(_tmp12CE,sizeof(char),26))),_tag_dyneither(_tmp12CD,sizeof(void*),
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
_tuple0 _tmp12CF;struct _tuple0 _tmp6EF=(_tmp12CF.f1=t1,((_tmp12CF.f2=t2,_tmp12CF)));
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
max_arith_type->v)){struct Cyc_Core_Opt*_tmp12D0;max_arith_type=((_tmp12D0=
_region_malloc(_tmp751,sizeof(*_tmp12D0)),((_tmp12D0->v=t1,_tmp12D0))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp12D5;void*_tmp12D4[2];struct
Cyc_String_pa_struct _tmp12D3;struct Cyc_String_pa_struct _tmp12D2;(_tmp12D2.tag=0,((
_tmp12D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp12D3.tag=0,((_tmp12D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12D4[0]=& _tmp12D3,((_tmp12D4[1]=& _tmp12D2,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12D5="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12D5,sizeof(char),41))),_tag_dyneither(_tmp12D4,sizeof(void*),
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
char*_tmp12D8;void*_tmp12D7;(_tmp12D7=0,Cyc_Tcutil_warn(e->loc,((_tmp12D8="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12D8,sizeof(char),44))),_tag_dyneither(_tmp12D7,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12DB;void*_tmp12DA;(
_tmp12DA=0,Cyc_Tcutil_warn(e->loc,((_tmp12DB="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12DB,sizeof(char),44))),_tag_dyneither(_tmp12DA,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12DC;struct _tuple0 _tmp763=(_tmp12DC.f1=
t1,((_tmp12DC.f2=t2,_tmp12DC)));void*_tmp764;struct Cyc_Absyn_PtrInfo _tmp766;void*
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
_tmp769.ptr_atts).bounds)){struct _tuple0 _tmp12DD;struct _tuple0 _tmp77D=(_tmp12DD.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp766.ptr_atts).bounds),((_tmp12DD.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp769.ptr_atts).bounds),
_tmp12DD)));void*_tmp77E;void*_tmp780;void*_tmp782;void*_tmp784;void*_tmp786;
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
0,(_tmp769.ptr_atts).zero_term)){const char*_tmp12E0;void*_tmp12DF;(_tmp12DF=0,
Cyc_Tcutil_warn(loc,((_tmp12E0="implicit cast to shorter array",_tag_dyneither(
_tmp12E0,sizeof(char),31))),_tag_dyneither(_tmp12DF,sizeof(void*),0)));}goto
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
_LL3CE: return _tmp799;_LL3CF:;_LL3D0: {const char*_tmp12E3;void*_tmp12E2;(_tmp12E2=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12E3="pointer_elt_type",_tag_dyneither(_tmp12E3,sizeof(char),17))),
_tag_dyneither(_tmp12E2,sizeof(void*),0)));}_LL3CC:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp79C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp79E;
struct Cyc_Absyn_PtrAtts _tmp79F;struct Cyc_Absyn_PtrAtts*_tmp7A0;_LL3D2: {struct
Cyc_Absyn_PointerType_struct*_tmp79D=(struct Cyc_Absyn_PointerType_struct*)
_tmp79C;if(_tmp79D->tag != 5)goto _LL3D4;else{_tmp79E=_tmp79D->f1;_tmp79F=_tmp79E.ptr_atts;
_tmp7A0=(struct Cyc_Absyn_PtrAtts*)&(_tmp79D->f1).ptr_atts;}}_LL3D3: return _tmp7A0->rgn;
_LL3D4:;_LL3D5: {const char*_tmp12E6;void*_tmp12E5;(_tmp12E5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E6="pointer_elt_type",
_tag_dyneither(_tmp12E6,sizeof(char),17))),_tag_dyneither(_tmp12E5,sizeof(void*),
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
union Cyc_Absyn_Cnst _tmp7BB;struct _tuple8 _tmp7BC;long long _tmp7BD;union Cyc_Absyn_Cnst
_tmp7BF;struct _dyneither_ptr _tmp7C0;void*_tmp7C2;struct Cyc_Absyn_Exp*_tmp7C3;
_LL3E1: {struct Cyc_Absyn_Const_e_struct*_tmp7AE=(struct Cyc_Absyn_Const_e_struct*)
_tmp7AD;if(_tmp7AE->tag != 0)goto _LL3E3;else{_tmp7AF=_tmp7AE->f1;if((_tmp7AF.Int_c).tag
!= 5)goto _LL3E3;_tmp7B0=(struct _tuple7)(_tmp7AF.Int_c).val;_tmp7B1=_tmp7B0.f2;
if(_tmp7B1 != 0)goto _LL3E3;}}_LL3E2: goto _LL3E4;_LL3E3: {struct Cyc_Absyn_Const_e_struct*
_tmp7B2=(struct Cyc_Absyn_Const_e_struct*)_tmp7AD;if(_tmp7B2->tag != 0)goto _LL3E5;
else{_tmp7B3=_tmp7B2->f1;if((_tmp7B3.Char_c).tag != 2)goto _LL3E5;_tmp7B4=(struct
_tuple5)(_tmp7B3.Char_c).val;_tmp7B5=_tmp7B4.f2;if(_tmp7B5 != 0)goto _LL3E5;}}
_LL3E4: goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Const_e_struct*_tmp7B6=(struct Cyc_Absyn_Const_e_struct*)
_tmp7AD;if(_tmp7B6->tag != 0)goto _LL3E7;else{_tmp7B7=_tmp7B6->f1;if((_tmp7B7.Short_c).tag
!= 4)goto _LL3E7;_tmp7B8=(struct _tuple6)(_tmp7B7.Short_c).val;_tmp7B9=_tmp7B8.f2;
if(_tmp7B9 != 0)goto _LL3E7;}}_LL3E6: goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Const_e_struct*
_tmp7BA=(struct Cyc_Absyn_Const_e_struct*)_tmp7AD;if(_tmp7BA->tag != 0)goto _LL3E9;
else{_tmp7BB=_tmp7BA->f1;if((_tmp7BB.LongLong_c).tag != 6)goto _LL3E9;_tmp7BC=(
struct _tuple8)(_tmp7BB.LongLong_c).val;_tmp7BD=_tmp7BC.f2;if(_tmp7BD != 0)goto
_LL3E9;}}_LL3E8: return 1;_LL3E9: {struct Cyc_Absyn_Const_e_struct*_tmp7BE=(struct
Cyc_Absyn_Const_e_struct*)_tmp7AD;if(_tmp7BE->tag != 0)goto _LL3EB;else{_tmp7BF=
_tmp7BE->f1;if((_tmp7BF.Wchar_c).tag != 3)goto _LL3EB;_tmp7C0=(struct
_dyneither_ptr)(_tmp7BF.Wchar_c).val;}}_LL3EA: {unsigned long _tmp7C4=Cyc_strlen((
struct _dyneither_ptr)_tmp7C0);int i=0;if(_tmp7C4 >= 2  && *((const char*)
_check_dyneither_subscript(_tmp7C0,sizeof(char),0))== '\\'){if(*((const char*)
_check_dyneither_subscript(_tmp7C0,sizeof(char),1))== '0')i=2;else{if((*((const
char*)_check_dyneither_subscript(_tmp7C0,sizeof(char),1))== 'x'  && _tmp7C4 >= 3)
 && *((const char*)_check_dyneither_subscript(_tmp7C0,sizeof(char),2))== '0')i=3;
else{return 0;}}for(0;i < _tmp7C4;++ i){if(*((const char*)_check_dyneither_subscript(
_tmp7C0,sizeof(char),i))!= '0')return 0;}return 1;}else{return 0;}}_LL3EB: {struct
Cyc_Absyn_Cast_e_struct*_tmp7C1=(struct Cyc_Absyn_Cast_e_struct*)_tmp7AD;if(
_tmp7C1->tag != 15)goto _LL3ED;else{_tmp7C2=(void*)_tmp7C1->f1;_tmp7C3=_tmp7C1->f2;}}
_LL3EC: return Cyc_Tcutil_is_zero(_tmp7C3) && Cyc_Tcutil_admits_zero(_tmp7C2);
_LL3ED:;_LL3EE: return 0;_LL3E0:;}struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,
Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};struct
Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind
Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_ek={
Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,
Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind
Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};struct Cyc_Absyn_Kind Cyc_Tcutil_urk={
Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,
Cyc_Absyn_Unique};struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};struct
Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};struct Cyc_Core_Opt Cyc_Tcutil_ako={(
void*)& Cyc_Tcutil_ak};struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};struct Cyc_Core_Opt Cyc_Tcutil_iko={(
void*)& Cyc_Tcutil_ik};struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};struct Cyc_Core_Opt Cyc_Tcutil_tako={(
void*)& Cyc_Tcutil_tak};struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};struct Cyc_Core_Opt Cyc_Tcutil_urko={(
void*)& Cyc_Tcutil_urk};struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};struct Cyc_Core_Opt Cyc_Tcutil_umko={(
void*)& Cyc_Tcutil_umk};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*
ka){struct Cyc_Absyn_Kind _tmp7C6;enum Cyc_Absyn_KindQual _tmp7C7;enum Cyc_Absyn_AliasQual
_tmp7C8;struct Cyc_Absyn_Kind*_tmp7C5=ka;_tmp7C6=*_tmp7C5;_tmp7C7=_tmp7C6.kind;
_tmp7C8=_tmp7C6.aliasqual;switch(_tmp7C8){case Cyc_Absyn_Aliasable: _LL3EF: switch(
_tmp7C7){case Cyc_Absyn_AnyKind: _LL3F1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;
case Cyc_Absyn_MemKind: _LL3F2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind:
_LL3F3: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F4:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F5: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F6: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3F0: switch(_tmp7C7){case
Cyc_Absyn_AnyKind: _LL3F9: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind:
_LL3FA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3FB:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3FC: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;default: _LL3FD: break;}break;case Cyc_Absyn_Top:
_LL3F8: switch(_tmp7C7){case Cyc_Absyn_AnyKind: _LL400: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL401: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;
case Cyc_Absyn_BoxKind: _LL402: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind:
_LL403: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL404: break;}break;}{
const char*_tmp12EA;void*_tmp12E9[1];struct Cyc_String_pa_struct _tmp12E8;(_tmp12E8.tag=
0,((_tmp12E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12E9[0]=& _tmp12E8,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EA="kind_to_opt: bad kind %s\n",
_tag_dyneither(_tmp12EA,sizeof(char),26))),_tag_dyneither(_tmp12E9,sizeof(void*),
1)))))));};}static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){static
struct Cyc_Absyn_Eq_kb_struct ab_v={0,& Cyc_Tcutil_ak};static struct Cyc_Absyn_Eq_kb_struct
mb_v={0,& Cyc_Tcutil_mk};static struct Cyc_Absyn_Eq_kb_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_struct rb_v={0,& Cyc_Tcutil_rk};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,& Cyc_Tcutil_ek};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,& Cyc_Tcutil_ik};
static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;
static struct Cyc_Absyn_Eq_kb_struct tab_v={0,& Cyc_Tcutil_tak};static struct Cyc_Absyn_Eq_kb_struct
tmb_v={0,& Cyc_Tcutil_tmk};static struct Cyc_Absyn_Eq_kb_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_struct trb_v={0,& Cyc_Tcutil_trk};static void*tab=(void*)&
tab_v;static void*tmb=(void*)& tmb_v;static void*tbb=(void*)& tbb_v;static void*trb=(
void*)& trb_v;static struct Cyc_Absyn_Eq_kb_struct uab_v={0,& Cyc_Tcutil_uak};static
struct Cyc_Absyn_Eq_kb_struct umb_v={0,& Cyc_Tcutil_umk};static struct Cyc_Absyn_Eq_kb_struct
ubb_v={0,& Cyc_Tcutil_ubk};static struct Cyc_Absyn_Eq_kb_struct urb_v={0,& Cyc_Tcutil_urk};
static void*uab=(void*)& uab_v;static void*umb=(void*)& umb_v;static void*ubb=(void*)&
ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind _tmp7CD;enum Cyc_Absyn_KindQual
_tmp7CE;enum Cyc_Absyn_AliasQual _tmp7CF;struct Cyc_Absyn_Kind*_tmp7CC=ka;_tmp7CD=*
_tmp7CC;_tmp7CE=_tmp7CD.kind;_tmp7CF=_tmp7CD.aliasqual;switch(_tmp7CF){case Cyc_Absyn_Aliasable:
_LL406: switch(_tmp7CE){case Cyc_Absyn_AnyKind: _LL408: return& ab;case Cyc_Absyn_MemKind:
_LL409: return& mb;case Cyc_Absyn_BoxKind: _LL40A: return& bb;case Cyc_Absyn_RgnKind:
_LL40B: return& rb;case Cyc_Absyn_EffKind: _LL40C: return& eb;case Cyc_Absyn_IntKind:
_LL40D: return& ib;}case Cyc_Absyn_Unique: _LL407: switch(_tmp7CE){case Cyc_Absyn_AnyKind:
_LL410: return& uab;case Cyc_Absyn_MemKind: _LL411: return& umb;case Cyc_Absyn_BoxKind:
_LL412: return& ubb;case Cyc_Absyn_RgnKind: _LL413: return& urb;default: _LL414: break;}
break;case Cyc_Absyn_Top: _LL40F: switch(_tmp7CE){case Cyc_Absyn_AnyKind: _LL417:
return& tab;case Cyc_Absyn_MemKind: _LL418: return& tmb;case Cyc_Absyn_BoxKind: _LL419:
return& tbb;case Cyc_Absyn_RgnKind: _LL41A: return& trb;default: _LL41B: break;}break;}{
const char*_tmp12EE;void*_tmp12ED[1];struct Cyc_String_pa_struct _tmp12EC;(_tmp12EC.tag=
0,((_tmp12EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12ED[0]=& _tmp12EC,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EE="kind_to_b: bad kind %s\n",
_tag_dyneither(_tmp12EE,sizeof(char),24))),_tag_dyneither(_tmp12ED,sizeof(void*),
1)))))));};}void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
struct Cyc_Core_Opt*_tmp12EF;return(_tmp12EF=_cycalloc(sizeof(*_tmp12EF)),((
_tmp12EF->v=Cyc_Tcutil_kind_to_bound(k),_tmp12EF)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12F2;struct Cyc_Absyn_Const_e_struct*
_tmp12F1;e1->r=(void*)((_tmp12F1=_cycalloc(sizeof(*_tmp12F1)),((_tmp12F1[0]=((
_tmp12F2.tag=0,((_tmp12F2.f1=Cyc_Absyn_Null_c,_tmp12F2)))),_tmp12F1))));}{struct
Cyc_Core_Opt*_tmp7E4=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12FC;struct Cyc_Absyn_PtrAtts _tmp12FB;struct Cyc_Absyn_PtrInfo _tmp12FA;struct
Cyc_Absyn_PointerType_struct*_tmp12F9;struct Cyc_Absyn_PointerType_struct*_tmp7E5=(
_tmp12F9=_cycalloc(sizeof(*_tmp12F9)),((_tmp12F9[0]=((_tmp12FC.tag=5,((_tmp12FC.f1=((
_tmp12FA.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7E4),((
_tmp12FA.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12FA.ptr_atts=((_tmp12FB.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7E4),((_tmp12FB.nullable=Cyc_Absyn_true_conref,((
_tmp12FB.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12FB.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12FB.ptrloc=0,_tmp12FB)))))))))),_tmp12FA)))))),_tmp12FC)))),_tmp12F9)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7E5);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp1301;void*_tmp1300[2];struct Cyc_String_pa_struct _tmp12FF;struct Cyc_String_pa_struct
_tmp12FE;(_tmp12FE.tag=0,((_tmp12FE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12FF.tag=0,((
_tmp12FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1300[0]=& _tmp12FF,((_tmp1300[1]=& _tmp12FE,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1301="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp1301,sizeof(char),56))),_tag_dyneither(_tmp1300,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL41D: {
const char*_tmp1302;return(_tmp1302="unknown",_tag_dyneither(_tmp1302,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL41E: {const char*_tmp1303;return(_tmp1303="no coercion",
_tag_dyneither(_tmp1303,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41F: {
const char*_tmp1304;return(_tmp1304="null check",_tag_dyneither(_tmp1304,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL420: {const char*_tmp1305;return(
_tmp1305="other coercion",_tag_dyneither(_tmp1305,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp1312;const char*_tmp1311;void*_tmp1310[1];struct Cyc_Int_pa_struct
_tmp130F;struct _tuple2*_tmp130E;struct _tuple2*v=(_tmp130E=_cycalloc(sizeof(*
_tmp130E)),((_tmp130E->f1=Cyc_Absyn_Loc_n,((_tmp130E->f2=((_tmp1312=_cycalloc(
sizeof(*_tmp1312)),((_tmp1312[0]=(struct _dyneither_ptr)((_tmp130F.tag=1,((
_tmp130F.f1=(unsigned long)counter ++,((_tmp1310[0]=& _tmp130F,Cyc_aprintf(((
_tmp1311="__aliasvar%d",_tag_dyneither(_tmp1311,sizeof(char),13))),
_tag_dyneither(_tmp1310,sizeof(void*),1)))))))),_tmp1312)))),_tmp130E)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp1315;struct Cyc_Absyn_Local_b_struct*
_tmp1314;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp1314=_cycalloc(
sizeof(*_tmp1314)),((_tmp1314[0]=((_tmp1315.tag=4,((_tmp1315.f1=vd,_tmp1315)))),
_tmp1314)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7F2=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7F4;struct Cyc_Absyn_PtrAtts
_tmp7F5;void*_tmp7F6;void**_tmp7F7;_LL423: {struct Cyc_Absyn_PointerType_struct*
_tmp7F3=(struct Cyc_Absyn_PointerType_struct*)_tmp7F2;if(_tmp7F3->tag != 5)goto
_LL425;else{_tmp7F4=_tmp7F3->f1;_tmp7F5=_tmp7F4.ptr_atts;_tmp7F6=_tmp7F5.rgn;
_tmp7F7=(void**)&((_tmp7F3->f1).ptr_atts).rgn;}}_LL424:{struct Cyc_Absyn_VarType_struct
_tmp1318;struct Cyc_Absyn_VarType_struct*_tmp1317;*_tmp7F7=(void*)((_tmp1317=
_cycalloc(sizeof(*_tmp1317)),((_tmp1317[0]=((_tmp1318.tag=2,((_tmp1318.f1=tv,
_tmp1318)))),_tmp1317))));}goto _LL422;_LL425:;_LL426: goto _LL422;_LL422:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple14 _tmp1319;return(_tmp1319.f1=
d,((_tmp1319.f2=ve,_tmp1319)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp803=_new_region("r");struct _RegionHandle*r=& _tmp803;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp804=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp806;struct Cyc_Absyn_PtrAtts _tmp807;void*_tmp808;_LL428: {struct Cyc_Absyn_PointerType_struct*
_tmp805=(struct Cyc_Absyn_PointerType_struct*)_tmp804;if(_tmp805->tag != 5)goto
_LL42A;else{_tmp806=_tmp805->f1;_tmp807=_tmp806.ptr_atts;_tmp808=_tmp807.rgn;}}
_LL429: {void*_tmp80A=Cyc_Tcutil_compress(_tmp808);_LL42F: {struct Cyc_Absyn_HeapRgn_struct*
_tmp80B=(struct Cyc_Absyn_HeapRgn_struct*)_tmp80A;if(_tmp80B->tag != 21)goto _LL431;}
_LL430: {int _tmp80C=0;_npop_handler(0);return _tmp80C;}_LL431:;_LL432: {struct Cyc_Absyn_Kind*
_tmp80D=Cyc_Tcutil_typ_kind(_tmp808);int _tmp80E=_tmp80D->kind == Cyc_Absyn_RgnKind
 && _tmp80D->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp80E;}
_LL42E:;}_LL42A: {struct Cyc_Absyn_TypedefType_struct*_tmp809=(struct Cyc_Absyn_TypedefType_struct*)
_tmp804;if(_tmp809->tag != 18)goto _LL42C;}_LL42B: {const char*_tmp131E;void*
_tmp131D[2];struct Cyc_String_pa_struct _tmp131C;struct Cyc_String_pa_struct
_tmp131B;(_tmp131B.tag=0,((_tmp131B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp131C.tag=0,((_tmp131C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp131D[0]=& _tmp131C,((_tmp131D[1]=& _tmp131B,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp131E="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp131E,sizeof(char),42))),_tag_dyneither(_tmp131D,sizeof(void*),
2)))))))))))));}_LL42C:;_LL42D: {int _tmp813=0;_npop_handler(0);return _tmp813;}
_LL427:;}{int _tmp814=0;_npop_handler(0);return _tmp814;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1323;void*_tmp1322[2];
struct Cyc_String_pa_struct _tmp1321;struct Cyc_String_pa_struct _tmp1320;(_tmp1320.tag=
0,((_tmp1320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1321.tag=0,((_tmp1321.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1322[0]=& _tmp1321,((_tmp1322[1]=& _tmp1320,Cyc_Tcutil_warn(
e->loc,((_tmp1323="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1323,sizeof(char),53))),_tag_dyneither(_tmp1322,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1329;void*_tmp1328[3];struct Cyc_String_pa_struct _tmp1327;struct Cyc_String_pa_struct
_tmp1326;struct Cyc_String_pa_struct _tmp1325;(_tmp1325.tag=0,((_tmp1325.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1326.tag=0,((_tmp1326.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1327.tag=0,((_tmp1327.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp1328[0]=& _tmp1327,((_tmp1328[1]=& _tmp1326,((
_tmp1328[2]=& _tmp1325,Cyc_Tcutil_warn(e->loc,((_tmp1329="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp1329,sizeof(char),40))),_tag_dyneither(_tmp1328,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp132E;void*_tmp132D[2];
struct Cyc_String_pa_struct _tmp132C;struct Cyc_String_pa_struct _tmp132B;(_tmp132B.tag=
0,((_tmp132B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp132C.tag=0,((_tmp132C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp132D[0]=& _tmp132C,((_tmp132D[1]=& _tmp132B,Cyc_Tcutil_warn(
e->loc,((_tmp132E="implicit cast from %s to %s",_tag_dyneither(_tmp132E,sizeof(
char),28))),_tag_dyneither(_tmp132D,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp822=Cyc_Tcutil_compress(t);_LL434: {struct Cyc_Absyn_IntType_struct*
_tmp823=(struct Cyc_Absyn_IntType_struct*)_tmp822;if(_tmp823->tag != 6)goto _LL436;}
_LL435: goto _LL437;_LL436: {struct Cyc_Absyn_FloatType_struct*_tmp824=(struct Cyc_Absyn_FloatType_struct*)
_tmp822;if(_tmp824->tag != 7)goto _LL438;}_LL437: goto _LL439;_LL438: {struct Cyc_Absyn_DoubleType_struct*
_tmp825=(struct Cyc_Absyn_DoubleType_struct*)_tmp822;if(_tmp825->tag != 8)goto
_LL43A;}_LL439: return 1;_LL43A:;_LL43B: return 0;_LL433:;}static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,
void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*
f3;int f4;};static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,
struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp827;struct _RegionHandle*
_tmp828;struct Cyc_Tcenv_Tenv*_tmp829;int _tmp82A;struct _tuple23 _tmp826=*env;
_tmp827=_tmp826.f1;_tmp828=_tmp826.f2;_tmp829=_tmp826.f3;_tmp82A=_tmp826.f4;{
void*_tmp82B=Cyc_Tcutil_rsubstitute(_tmp828,_tmp827,x->type);struct Cyc_List_List*
_tmp82C=Cyc_Tcutil_flatten_typ(_tmp828,_tmp82A,_tmp829,_tmp82B);if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp82C)== 1){struct _tuple11*_tmp1331;
struct Cyc_List_List*_tmp1330;return(_tmp1330=_region_malloc(_tmp828,sizeof(*
_tmp1330)),((_tmp1330->hd=((_tmp1331=_region_malloc(_tmp828,sizeof(*_tmp1331)),((
_tmp1331->f1=x->tq,((_tmp1331->f2=_tmp82B,_tmp1331)))))),((_tmp1330->tl=0,
_tmp1330)))));}else{return _tmp82C;}};}struct _tuple24{struct _RegionHandle*f1;
struct Cyc_Tcenv_Tenv*f2;int f3;};static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(
struct _tuple24*env,struct _tuple11*x){struct _RegionHandle*_tmp830;struct Cyc_Tcenv_Tenv*
_tmp831;int _tmp832;struct _tuple24 _tmp82F=*env;_tmp830=_tmp82F.f1;_tmp831=_tmp82F.f2;
_tmp832=_tmp82F.f3;{struct Cyc_Absyn_Tqual _tmp834;void*_tmp835;struct _tuple11
_tmp833=*x;_tmp834=_tmp833.f1;_tmp835=_tmp833.f2;{struct Cyc_List_List*_tmp836=
Cyc_Tcutil_flatten_typ(_tmp830,_tmp832,_tmp831,_tmp835);if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp836)== 1){struct _tuple11*_tmp1334;struct Cyc_List_List*
_tmp1333;return(_tmp1333=_region_malloc(_tmp830,sizeof(*_tmp1333)),((_tmp1333->hd=((
_tmp1334=_region_malloc(_tmp830,sizeof(*_tmp1334)),((_tmp1334->f1=_tmp834,((
_tmp1334->f2=_tmp835,_tmp1334)))))),((_tmp1333->tl=0,_tmp1333)))));}else{return
_tmp836;}};};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct
_RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(
t1);{void*_tmp839=t1;struct Cyc_List_List*_tmp83C;struct Cyc_Absyn_AggrInfo _tmp83E;
union Cyc_Absyn_AggrInfoU _tmp83F;struct Cyc_Absyn_Aggrdecl**_tmp840;struct Cyc_Absyn_Aggrdecl*
_tmp841;struct Cyc_List_List*_tmp842;enum Cyc_Absyn_AggrKind _tmp844;struct Cyc_List_List*
_tmp845;_LL43D: {struct Cyc_Absyn_VoidType_struct*_tmp83A=(struct Cyc_Absyn_VoidType_struct*)
_tmp839;if(_tmp83A->tag != 0)goto _LL43F;}_LL43E: return 0;_LL43F: {struct Cyc_Absyn_TupleType_struct*
_tmp83B=(struct Cyc_Absyn_TupleType_struct*)_tmp839;if(_tmp83B->tag != 11)goto
_LL441;else{_tmp83C=_tmp83B->f1;}}_LL440: {struct _tuple24 _tmp1335;struct _tuple24
_tmp846=(_tmp1335.f1=r,((_tmp1335.f2=te,((_tmp1335.f3=flatten,_tmp1335)))));
struct Cyc_List_List*_tmp847=_tmp83C;struct Cyc_List_List _tmp848;struct _tuple11*
_tmp849;struct Cyc_List_List*_tmp84A;_LL448: if(_tmp847 != 0)goto _LL44A;_LL449:
return 0;_LL44A: if(_tmp847 == 0)goto _LL447;_tmp848=*_tmp847;_tmp849=(struct
_tuple11*)_tmp848.hd;_tmp84A=_tmp848.tl;_LL44B: {struct Cyc_List_List*_tmp84B=Cyc_Tcutil_rcopy_tqt(&
_tmp846,_tmp849);_tmp846.f3=0;{struct Cyc_List_List*_tmp84C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp846,_tmp83C);struct Cyc_List_List*_tmp1336;struct Cyc_List_List*_tmp84D=(
_tmp1336=_region_malloc(r,sizeof(*_tmp1336)),((_tmp1336->hd=_tmp84B,((_tmp1336->tl=
_tmp84C,_tmp1336)))));struct Cyc_List_List*_tmp1337;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1337=
_region_malloc(r,sizeof(*_tmp1337)),((_tmp1337->hd=_tmp84B,((_tmp1337->tl=
_tmp84C,_tmp1337)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_struct*
_tmp83D=(struct Cyc_Absyn_AggrType_struct*)_tmp839;if(_tmp83D->tag != 12)goto
_LL443;else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83E.aggr_info;if((_tmp83F.KnownAggr).tag
!= 2)goto _LL443;_tmp840=(struct Cyc_Absyn_Aggrdecl**)(_tmp83F.KnownAggr).val;
_tmp841=*_tmp840;_tmp842=_tmp83E.targs;}}_LL442: if(((_tmp841->kind == Cyc_Absyn_UnionA
 || _tmp841->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp841->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp841->impl))->rgn_po != 0){
struct _tuple11*_tmp133A;struct Cyc_List_List*_tmp1339;return(_tmp1339=
_region_malloc(r,sizeof(*_tmp1339)),((_tmp1339->hd=((_tmp133A=_region_malloc(r,
sizeof(*_tmp133A)),((_tmp133A->f1=Cyc_Absyn_empty_tqual(0),((_tmp133A->f2=t1,
_tmp133A)))))),((_tmp1339->tl=0,_tmp1339)))));}{struct Cyc_List_List*_tmp853=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp841->tvs,_tmp842);struct _tuple23
_tmp133B;struct _tuple23 env=(_tmp133B.f1=_tmp853,((_tmp133B.f2=r,((_tmp133B.f3=te,((
_tmp133B.f4=flatten,_tmp133B)))))));struct Cyc_List_List*_tmp854=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp841->impl))->fields;struct Cyc_List_List _tmp855;struct Cyc_Absyn_Aggrfield*
_tmp856;struct Cyc_List_List*_tmp857;_LL44D: if(_tmp854 != 0)goto _LL44F;_LL44E:
return 0;_LL44F: if(_tmp854 == 0)goto _LL44C;_tmp855=*_tmp854;_tmp856=(struct Cyc_Absyn_Aggrfield*)
_tmp855.hd;_tmp857=_tmp855.tl;_LL450: {struct Cyc_List_List*_tmp858=Cyc_Tcutil_flatten_typ_f(&
env,_tmp856);env.f4=0;{struct Cyc_List_List*_tmp859=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp857);struct Cyc_List_List*_tmp133C;struct Cyc_List_List*_tmp85A=(_tmp133C=
_region_malloc(r,sizeof(*_tmp133C)),((_tmp133C->hd=_tmp858,((_tmp133C->tl=
_tmp859,_tmp133C)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp85A);};}_LL44C:;};_LL443: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp843=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp839;if(_tmp843->tag != 13)goto _LL445;else{_tmp844=_tmp843->f1;if(_tmp844 != 
Cyc_Absyn_StructA)goto _LL445;_tmp845=_tmp843->f2;}}_LL444: {struct _tuple23
_tmp133D;struct _tuple23 env=(_tmp133D.f1=0,((_tmp133D.f2=r,((_tmp133D.f3=te,((
_tmp133D.f4=flatten,_tmp133D)))))));struct Cyc_List_List*_tmp85D=_tmp845;struct
Cyc_List_List _tmp85E;struct Cyc_Absyn_Aggrfield*_tmp85F;struct Cyc_List_List*
_tmp860;_LL452: if(_tmp85D != 0)goto _LL454;_LL453: return 0;_LL454: if(_tmp85D == 0)
goto _LL451;_tmp85E=*_tmp85D;_tmp85F=(struct Cyc_Absyn_Aggrfield*)_tmp85E.hd;
_tmp860=_tmp85E.tl;_LL455: {struct Cyc_List_List*_tmp861=Cyc_Tcutil_flatten_typ_f(&
env,_tmp85F);env.f4=0;{struct Cyc_List_List*_tmp862=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp860);struct Cyc_List_List*_tmp133E;struct Cyc_List_List*_tmp863=(_tmp133E=
_region_malloc(r,sizeof(*_tmp133E)),((_tmp133E->hd=_tmp861,((_tmp133E->tl=
_tmp862,_tmp133E)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp863);};}_LL451:;}_LL445:;_LL446:
goto _LL43C;_LL43C:;};}{struct _tuple11*_tmp1341;struct Cyc_List_List*_tmp1340;
return(_tmp1340=_region_malloc(r,sizeof(*_tmp1340)),((_tmp1340->hd=((_tmp1341=
_region_malloc(r,sizeof(*_tmp1341)),((_tmp1341->f1=Cyc_Absyn_empty_tqual(0),((
_tmp1341->f2=t1,_tmp1341)))))),((_tmp1340->tl=0,_tmp1340)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp868=(void*)t->hd;_LL457: {struct Cyc_Absyn_Pure_att_struct*
_tmp869=(struct Cyc_Absyn_Pure_att_struct*)_tmp868;if(_tmp869->tag != 22)goto
_LL459;}_LL458: goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_struct*_tmp86A=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp868;if(_tmp86A->tag != 4)goto _LL45B;}
_LL45A: goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_struct*_tmp86B=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp868;if(_tmp86B->tag != 20)goto _LL45D;}
_LL45C: continue;_LL45D:;_LL45E: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL456:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1342;struct _tuple0 _tmp86D=(_tmp1342.f1=Cyc_Tcutil_compress(t1),((
_tmp1342.f2=Cyc_Tcutil_compress(t2),_tmp1342)));void*_tmp86E;enum Cyc_Absyn_Size_of
_tmp870;void*_tmp871;enum Cyc_Absyn_Size_of _tmp873;_LL460: _tmp86E=_tmp86D.f1;{
struct Cyc_Absyn_IntType_struct*_tmp86F=(struct Cyc_Absyn_IntType_struct*)_tmp86E;
if(_tmp86F->tag != 6)goto _LL462;else{_tmp870=_tmp86F->f2;}};_tmp871=_tmp86D.f2;{
struct Cyc_Absyn_IntType_struct*_tmp872=(struct Cyc_Absyn_IntType_struct*)_tmp871;
if(_tmp872->tag != 6)goto _LL462;else{_tmp873=_tmp872->f2;}};_LL461: return(_tmp870
== _tmp873  || _tmp870 == Cyc_Absyn_Int_sz  && _tmp873 == Cyc_Absyn_Long_sz) || 
_tmp870 == Cyc_Absyn_Long_sz  && _tmp873 == Cyc_Absyn_Int_sz;_LL462:;_LL463: return 0;
_LL45F:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1343;struct _tuple0 _tmp875=(
_tmp1343.f1=t1,((_tmp1343.f2=t2,_tmp1343)));void*_tmp876;struct Cyc_Absyn_PtrInfo
_tmp878;void*_tmp879;struct Cyc_Absyn_Tqual _tmp87A;struct Cyc_Absyn_PtrAtts _tmp87B;
void*_tmp87C;union Cyc_Absyn_Constraint*_tmp87D;union Cyc_Absyn_Constraint*_tmp87E;
union Cyc_Absyn_Constraint*_tmp87F;void*_tmp880;struct Cyc_Absyn_PtrInfo _tmp882;
void*_tmp883;struct Cyc_Absyn_Tqual _tmp884;struct Cyc_Absyn_PtrAtts _tmp885;void*
_tmp886;union Cyc_Absyn_Constraint*_tmp887;union Cyc_Absyn_Constraint*_tmp888;
union Cyc_Absyn_Constraint*_tmp889;void*_tmp88A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp88C;union Cyc_Absyn_DatatypeFieldInfoU _tmp88D;struct _tuple3 _tmp88E;struct Cyc_Absyn_Datatypedecl*
_tmp88F;struct Cyc_Absyn_Datatypefield*_tmp890;struct Cyc_List_List*_tmp891;void*
_tmp892;struct Cyc_Absyn_DatatypeInfo _tmp894;union Cyc_Absyn_DatatypeInfoU _tmp895;
struct Cyc_Absyn_Datatypedecl**_tmp896;struct Cyc_Absyn_Datatypedecl*_tmp897;
struct Cyc_List_List*_tmp898;void*_tmp899;struct Cyc_Absyn_FnInfo _tmp89B;void*
_tmp89C;struct Cyc_Absyn_FnInfo _tmp89E;_LL465: _tmp876=_tmp875.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp877=(struct Cyc_Absyn_PointerType_struct*)_tmp876;if(_tmp877->tag != 5)goto
_LL467;else{_tmp878=_tmp877->f1;_tmp879=_tmp878.elt_typ;_tmp87A=_tmp878.elt_tq;
_tmp87B=_tmp878.ptr_atts;_tmp87C=_tmp87B.rgn;_tmp87D=_tmp87B.nullable;_tmp87E=
_tmp87B.bounds;_tmp87F=_tmp87B.zero_term;}};_tmp880=_tmp875.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp881=(struct Cyc_Absyn_PointerType_struct*)_tmp880;if(_tmp881->tag != 5)goto
_LL467;else{_tmp882=_tmp881->f1;_tmp883=_tmp882.elt_typ;_tmp884=_tmp882.elt_tq;
_tmp885=_tmp882.ptr_atts;_tmp886=_tmp885.rgn;_tmp887=_tmp885.nullable;_tmp888=
_tmp885.bounds;_tmp889=_tmp885.zero_term;}};_LL466: if(_tmp87A.real_const  && !
_tmp884.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp87D,
_tmp887) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87D)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp887))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp87F,
_tmp889) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87F)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp889))return 0;if(!Cyc_Tcutil_unify(_tmp87C,_tmp886) && !Cyc_Tcenv_region_outlives(
te,_tmp87C,_tmp886))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp87E,_tmp888)){struct _tuple0 _tmp1344;struct _tuple0 _tmp8A0=(_tmp1344.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp87E),((_tmp1344.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp888),_tmp1344)));void*
_tmp8A1;void*_tmp8A3;void*_tmp8A5;struct Cyc_Absyn_Exp*_tmp8A7;void*_tmp8A8;
struct Cyc_Absyn_Exp*_tmp8AA;_LL46E: _tmp8A1=_tmp8A0.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp8A2=(struct Cyc_Absyn_Upper_b_struct*)_tmp8A1;if(_tmp8A2->tag != 1)goto _LL470;};
_tmp8A3=_tmp8A0.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp8A4=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp8A3;if(_tmp8A4->tag != 0)goto _LL470;};_LL46F: goto _LL46D;_LL470: _tmp8A5=
_tmp8A0.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8A6=(struct Cyc_Absyn_Upper_b_struct*)
_tmp8A5;if(_tmp8A6->tag != 1)goto _LL472;else{_tmp8A7=_tmp8A6->f1;}};_tmp8A8=
_tmp8A0.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp8A9=(struct Cyc_Absyn_Upper_b_struct*)
_tmp8A8;if(_tmp8A9->tag != 1)goto _LL472;else{_tmp8AA=_tmp8A9->f1;}};_LL471: if(!
Cyc_Evexp_lte_const_exp(_tmp8AA,_tmp8A7))return 0;goto _LL46D;_LL472:;_LL473:
return 0;_LL46D:;}if(!_tmp884.real_const  && _tmp87A.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp883)))return 0;}{int _tmp8AB=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp888,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp889);struct _tuple0*
_tmp1347;struct Cyc_List_List*_tmp1346;return(_tmp8AB  && Cyc_Tcutil_ptrsubtype(te,((
_tmp1346=_cycalloc(sizeof(*_tmp1346)),((_tmp1346->hd=((_tmp1347=_cycalloc(
sizeof(*_tmp1347)),((_tmp1347->f1=t1,((_tmp1347->f2=t2,_tmp1347)))))),((_tmp1346->tl=
assume,_tmp1346)))))),_tmp879,_tmp883) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL467: _tmp88A=_tmp875.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp88B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp88A;if(_tmp88B->tag != 4)
goto _LL469;else{_tmp88C=_tmp88B->f1;_tmp88D=_tmp88C.field_info;if((_tmp88D.KnownDatatypefield).tag
!= 2)goto _LL469;_tmp88E=(struct _tuple3)(_tmp88D.KnownDatatypefield).val;_tmp88F=
_tmp88E.f1;_tmp890=_tmp88E.f2;_tmp891=_tmp88C.targs;}};_tmp892=_tmp875.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp893=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp892;if(_tmp893->tag != 3)goto _LL469;else{_tmp894=_tmp893->f1;_tmp895=_tmp894.datatype_info;
if((_tmp895.KnownDatatype).tag != 2)goto _LL469;_tmp896=(struct Cyc_Absyn_Datatypedecl**)(
_tmp895.KnownDatatype).val;_tmp897=*_tmp896;_tmp898=_tmp894.targs;}};_LL468: if(
_tmp88F != _tmp897  && Cyc_Absyn_qvar_cmp(_tmp88F->name,_tmp897->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp891)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp898))return 0;for(0;_tmp891 != 0;(_tmp891=
_tmp891->tl,_tmp898=_tmp898->tl)){if(!Cyc_Tcutil_unify((void*)_tmp891->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp898))->hd))return 0;}return 1;_LL469: _tmp899=
_tmp875.f1;{struct Cyc_Absyn_FnType_struct*_tmp89A=(struct Cyc_Absyn_FnType_struct*)
_tmp899;if(_tmp89A->tag != 10)goto _LL46B;else{_tmp89B=_tmp89A->f1;}};_tmp89C=
_tmp875.f2;{struct Cyc_Absyn_FnType_struct*_tmp89D=(struct Cyc_Absyn_FnType_struct*)
_tmp89C;if(_tmp89D->tag != 10)goto _LL46B;else{_tmp89E=_tmp89D->f1;}};_LL46A: if(
_tmp89B.tvars != 0  || _tmp89E.tvars != 0){struct Cyc_List_List*_tmp8AE=_tmp89B.tvars;
struct Cyc_List_List*_tmp8AF=_tmp89E.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8AE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AF))return 0;{
struct _RegionHandle*_tmp8B0=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp8AE != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp8AE->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp8AF))->hd)->kind))return 0;{struct _tuple15*_tmp1351;struct Cyc_Absyn_VarType_struct
_tmp1350;struct Cyc_Absyn_VarType_struct*_tmp134F;struct Cyc_List_List*_tmp134E;
inst=((_tmp134E=_region_malloc(_tmp8B0,sizeof(*_tmp134E)),((_tmp134E->hd=((
_tmp1351=_region_malloc(_tmp8B0,sizeof(*_tmp1351)),((_tmp1351->f1=(struct Cyc_Absyn_Tvar*)
_tmp8AF->hd,((_tmp1351->f2=(void*)((_tmp134F=_cycalloc(sizeof(*_tmp134F)),((
_tmp134F[0]=((_tmp1350.tag=2,((_tmp1350.f1=(struct Cyc_Absyn_Tvar*)_tmp8AE->hd,
_tmp1350)))),_tmp134F)))),_tmp1351)))))),((_tmp134E->tl=inst,_tmp134E))))));}
_tmp8AE=_tmp8AE->tl;_tmp8AF=_tmp8AF->tl;}if(inst != 0){_tmp89B.tvars=0;_tmp89E.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1357;struct Cyc_Absyn_FnType_struct*_tmp1356;
struct Cyc_Absyn_FnType_struct _tmp1354;struct Cyc_Absyn_FnType_struct*_tmp1353;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1353=_cycalloc(sizeof(*_tmp1353)),((
_tmp1353[0]=((_tmp1354.tag=10,((_tmp1354.f1=_tmp89B,_tmp1354)))),_tmp1353)))),(
void*)((_tmp1356=_cycalloc(sizeof(*_tmp1356)),((_tmp1356[0]=((_tmp1357.tag=10,((
_tmp1357.f1=_tmp89E,_tmp1357)))),_tmp1356)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp89B.ret_typ,_tmp89E.ret_typ))return 0;{struct Cyc_List_List*_tmp8B9=
_tmp89B.args;struct Cyc_List_List*_tmp8BA=_tmp89E.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp8B9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8BA))return 0;for(0;_tmp8B9 != 0;(_tmp8B9=_tmp8B9->tl,_tmp8BA=_tmp8BA->tl)){
struct Cyc_Absyn_Tqual _tmp8BC;void*_tmp8BD;struct _tuple9 _tmp8BB=*((struct _tuple9*)
_tmp8B9->hd);_tmp8BC=_tmp8BB.f2;_tmp8BD=_tmp8BB.f3;{struct Cyc_Absyn_Tqual _tmp8BF;
void*_tmp8C0;struct _tuple9 _tmp8BE=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp8BA))->hd);_tmp8BF=_tmp8BE.f2;_tmp8C0=_tmp8BE.f3;if(_tmp8BF.real_const
 && !_tmp8BC.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8C0,_tmp8BD))return
0;};}if(_tmp89B.c_varargs != _tmp89E.c_varargs)return 0;if(_tmp89B.cyc_varargs != 0
 && _tmp89E.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8C1=*_tmp89B.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8C2=*_tmp89E.cyc_varargs;if((_tmp8C2.tq).real_const
 && !(_tmp8C1.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8C2.type,
_tmp8C1.type))return 0;}else{if(_tmp89B.cyc_varargs != 0  || _tmp89E.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp89B.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp89E.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp89B.rgn_po,_tmp89E.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp89B.attributes,_tmp89E.attributes))return 0;return 1;};_LL46B:;_LL46C: return 0;
_LL464:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp8C3=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp8C3,1,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp8C3,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp8C5;struct Cyc_Absyn_Tqual
_tmp8C6;void*_tmp8C7;struct _tuple11*_tmp8C4=(struct _tuple11*)tqs1->hd;_tmp8C5=*
_tmp8C4;_tmp8C6=_tmp8C5.f1;_tmp8C7=_tmp8C5.f2;{struct _tuple11 _tmp8C9;struct Cyc_Absyn_Tqual
_tmp8CA;void*_tmp8CB;struct _tuple11*_tmp8C8=(struct _tuple11*)tqs2->hd;_tmp8C9=*
_tmp8C8;_tmp8CA=_tmp8C9.f1;_tmp8CB=_tmp8C9.f2;if(_tmp8C6.real_const  && !_tmp8CA.real_const)
return 0;if((_tmp8CA.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8CB))) && Cyc_Tcutil_subtype(te,assume,_tmp8C7,_tmp8CB))continue;if(Cyc_Tcutil_unify(
_tmp8C7,_tmp8CB))continue;if(Cyc_Tcutil_isomorphic(_tmp8C7,_tmp8CB))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8CC=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8CE;_LL475: {struct Cyc_Absyn_IntType_struct*_tmp8CD=(
struct Cyc_Absyn_IntType_struct*)_tmp8CC;if(_tmp8CD->tag != 6)goto _LL477;else{
_tmp8CE=_tmp8CD->f2;if(_tmp8CE != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476: return 1;
_LL477:;_LL478: return 0;_LL474:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8CF=t2;enum Cyc_Absyn_Size_of _tmp8D1;enum Cyc_Absyn_Size_of _tmp8D3;_LL47A: {
struct Cyc_Absyn_IntType_struct*_tmp8D0=(struct Cyc_Absyn_IntType_struct*)_tmp8CF;
if(_tmp8D0->tag != 6)goto _LL47C;else{_tmp8D1=_tmp8D0->f2;if(_tmp8D1 != Cyc_Absyn_Int_sz)
goto _LL47C;}}_LL47B: goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_struct*_tmp8D2=(
struct Cyc_Absyn_IntType_struct*)_tmp8CF;if(_tmp8D2->tag != 6)goto _LL47E;else{
_tmp8D3=_tmp8D2->f2;if(_tmp8D3 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL479;_LL47E:;
_LL47F: goto _LL479;_LL479:;}{void*_tmp8D4=t1;struct Cyc_Absyn_PtrInfo _tmp8D6;void*
_tmp8D7;struct Cyc_Absyn_Tqual _tmp8D8;struct Cyc_Absyn_PtrAtts _tmp8D9;void*_tmp8DA;
union Cyc_Absyn_Constraint*_tmp8DB;union Cyc_Absyn_Constraint*_tmp8DC;union Cyc_Absyn_Constraint*
_tmp8DD;struct Cyc_Absyn_ArrayInfo _tmp8DF;void*_tmp8E0;struct Cyc_Absyn_Tqual
_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E2;union Cyc_Absyn_Constraint*_tmp8E3;struct Cyc_Absyn_Enumdecl*
_tmp8E5;void*_tmp8EA;_LL481: {struct Cyc_Absyn_PointerType_struct*_tmp8D5=(struct
Cyc_Absyn_PointerType_struct*)_tmp8D4;if(_tmp8D5->tag != 5)goto _LL483;else{
_tmp8D6=_tmp8D5->f1;_tmp8D7=_tmp8D6.elt_typ;_tmp8D8=_tmp8D6.elt_tq;_tmp8D9=
_tmp8D6.ptr_atts;_tmp8DA=_tmp8D9.rgn;_tmp8DB=_tmp8D9.nullable;_tmp8DC=_tmp8D9.bounds;
_tmp8DD=_tmp8D9.zero_term;}}_LL482:{void*_tmp8EB=t2;struct Cyc_Absyn_PtrInfo
_tmp8ED;void*_tmp8EE;struct Cyc_Absyn_Tqual _tmp8EF;struct Cyc_Absyn_PtrAtts _tmp8F0;
void*_tmp8F1;union Cyc_Absyn_Constraint*_tmp8F2;union Cyc_Absyn_Constraint*_tmp8F3;
union Cyc_Absyn_Constraint*_tmp8F4;_LL492: {struct Cyc_Absyn_PointerType_struct*
_tmp8EC=(struct Cyc_Absyn_PointerType_struct*)_tmp8EB;if(_tmp8EC->tag != 5)goto
_LL494;else{_tmp8ED=_tmp8EC->f1;_tmp8EE=_tmp8ED.elt_typ;_tmp8EF=_tmp8ED.elt_tq;
_tmp8F0=_tmp8ED.ptr_atts;_tmp8F1=_tmp8F0.rgn;_tmp8F2=_tmp8F0.nullable;_tmp8F3=
_tmp8F0.bounds;_tmp8F4=_tmp8F0.zero_term;}}_LL493: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp135A;struct Cyc_List_List*
_tmp1359;struct Cyc_List_List*_tmp8F5=(_tmp1359=_cycalloc(sizeof(*_tmp1359)),((
_tmp1359->hd=((_tmp135A=_cycalloc(sizeof(*_tmp135A)),((_tmp135A->f1=t1,((
_tmp135A->f2=t2,_tmp135A)))))),((_tmp1359->tl=0,_tmp1359)))));int _tmp8F6=_tmp8EF.real_const
 || !_tmp8D8.real_const;int _tmp8F7=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8F3,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8F4);int _tmp8F8=_tmp8F6  && (((_tmp8F7  && Cyc_Tcutil_ptrsubtype(
te,_tmp8F5,_tmp8D7,_tmp8EE) || Cyc_Tcutil_unify(_tmp8D7,_tmp8EE)) || Cyc_Tcutil_isomorphic(
_tmp8D7,_tmp8EE)) || Cyc_Tcutil_unify(_tmp8EE,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8DD,_tmp8F4) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8F4);int _tmp8F9=_tmp8F8?0:((Cyc_Tcutil_bits_only(_tmp8D7) && Cyc_Tcutil_is_char_type(
_tmp8EE)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8F4)) && (_tmp8EF.real_const  || !_tmp8D8.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8DC,_tmp8F3);if(!
bounds_ok  && !_tmp8F9){struct _tuple0 _tmp135B;struct _tuple0 _tmp8FB=(_tmp135B.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8DC),((_tmp135B.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8F3),_tmp135B)));
void*_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FE;void*_tmp8FF;struct Cyc_Absyn_Exp*
_tmp901;_LL497: _tmp8FC=_tmp8FB.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8FD=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8FC;if(_tmp8FD->tag != 1)goto _LL499;else{
_tmp8FE=_tmp8FD->f1;}};_tmp8FF=_tmp8FB.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp900=(struct Cyc_Absyn_Upper_b_struct*)_tmp8FF;if(_tmp900->tag != 1)goto _LL499;
else{_tmp901=_tmp900->f1;}};_LL498: if(Cyc_Evexp_lte_const_exp(_tmp901,_tmp8FE))
bounds_ok=1;goto _LL496;_LL499:;_LL49A: bounds_ok=1;goto _LL496;_LL496:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8DB) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8F2))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8F8  || _tmp8F9)) && (Cyc_Tcutil_unify(
_tmp8DA,_tmp8F1) || Cyc_Tcenv_region_outlives(te,_tmp8DA,_tmp8F1)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL494:;_LL495: goto _LL491;
_LL491:;}return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_struct*
_tmp8DE=(struct Cyc_Absyn_ArrayType_struct*)_tmp8D4;if(_tmp8DE->tag != 9)goto
_LL485;else{_tmp8DF=_tmp8DE->f1;_tmp8E0=_tmp8DF.elt_type;_tmp8E1=_tmp8DF.tq;
_tmp8E2=_tmp8DF.num_elts;_tmp8E3=_tmp8DF.zero_term;}}_LL484:{void*_tmp904=t2;
struct Cyc_Absyn_ArrayInfo _tmp906;void*_tmp907;struct Cyc_Absyn_Tqual _tmp908;
struct Cyc_Absyn_Exp*_tmp909;union Cyc_Absyn_Constraint*_tmp90A;_LL49C: {struct Cyc_Absyn_ArrayType_struct*
_tmp905=(struct Cyc_Absyn_ArrayType_struct*)_tmp904;if(_tmp905->tag != 9)goto
_LL49E;else{_tmp906=_tmp905->f1;_tmp907=_tmp906.elt_type;_tmp908=_tmp906.tq;
_tmp909=_tmp906.num_elts;_tmp90A=_tmp906.zero_term;}}_LL49D: {int okay;okay=((
_tmp8E2 != 0  && _tmp909 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E3,
_tmp90A)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp909,(struct Cyc_Absyn_Exp*)
_tmp8E2);return(okay  && Cyc_Tcutil_unify(_tmp8E0,_tmp907)) && (!_tmp8E1.real_const
 || _tmp908.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL49E:;_LL49F: return Cyc_Absyn_Unknown_coercion;_LL49B:;}return Cyc_Absyn_Unknown_coercion;
_LL485: {struct Cyc_Absyn_EnumType_struct*_tmp8E4=(struct Cyc_Absyn_EnumType_struct*)
_tmp8D4;if(_tmp8E4->tag != 14)goto _LL487;else{_tmp8E5=_tmp8E4->f2;}}_LL486:{void*
_tmp90B=t2;struct Cyc_Absyn_Enumdecl*_tmp90D;_LL4A1: {struct Cyc_Absyn_EnumType_struct*
_tmp90C=(struct Cyc_Absyn_EnumType_struct*)_tmp90B;if(_tmp90C->tag != 14)goto
_LL4A3;else{_tmp90D=_tmp90C->f2;}}_LL4A2: if((_tmp8E5->fields != 0  && _tmp90D->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8E5->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp90D->fields))->v))return Cyc_Absyn_No_coercion;goto _LL4A0;_LL4A3:;_LL4A4: goto
_LL4A0;_LL4A0:;}goto _LL488;_LL487: {struct Cyc_Absyn_IntType_struct*_tmp8E6=(
struct Cyc_Absyn_IntType_struct*)_tmp8D4;if(_tmp8E6->tag != 6)goto _LL489;}_LL488:
goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_struct*_tmp8E7=(struct Cyc_Absyn_FloatType_struct*)
_tmp8D4;if(_tmp8E7->tag != 7)goto _LL48B;}_LL48A: goto _LL48C;_LL48B: {struct Cyc_Absyn_DoubleType_struct*
_tmp8E8=(struct Cyc_Absyn_DoubleType_struct*)_tmp8D4;if(_tmp8E8->tag != 8)goto
_LL48D;}_LL48C: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL48D: {struct Cyc_Absyn_RgnHandleType_struct*_tmp8E9=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8D4;if(_tmp8E9->tag != 16)goto _LL48F;else{_tmp8EA=(void*)_tmp8E9->f1;}}_LL48E:{
void*_tmp90E=t2;void*_tmp910;_LL4A6: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp90F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp90E;if(_tmp90F->tag != 16)goto
_LL4A8;else{_tmp910=(void*)_tmp90F->f1;}}_LL4A7: if(Cyc_Tcenv_region_outlives(te,
_tmp8EA,_tmp910))return Cyc_Absyn_No_coercion;goto _LL4A5;_LL4A8:;_LL4A9: goto
_LL4A5;_LL4A5:;}return Cyc_Absyn_Unknown_coercion;_LL48F:;_LL490: return Cyc_Absyn_Unknown_coercion;
_LL480:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp135E;struct Cyc_Absyn_Cast_e_struct*_tmp135D;e->r=(
void*)((_tmp135D=_cycalloc(sizeof(*_tmp135D)),((_tmp135D[0]=((_tmp135E.tag=15,((
_tmp135E.f1=(void*)t,((_tmp135E.f2=_tmp911,((_tmp135E.f3=0,((_tmp135E.f4=c,
_tmp135E)))))))))),_tmp135D))));}{struct Cyc_Core_Opt*_tmp135F;e->topt=((_tmp135F=
_cycalloc(sizeof(*_tmp135F)),((_tmp135F->v=t,_tmp135F))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp915=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL4AB: {struct Cyc_Absyn_IntType_struct*_tmp916=(
struct Cyc_Absyn_IntType_struct*)_tmp915;if(_tmp916->tag != 6)goto _LL4AD;}_LL4AC:
goto _LL4AE;_LL4AD: {struct Cyc_Absyn_EnumType_struct*_tmp917=(struct Cyc_Absyn_EnumType_struct*)
_tmp915;if(_tmp917->tag != 14)goto _LL4AF;}_LL4AE: goto _LL4B0;_LL4AF: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp918=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp915;if(_tmp918->tag != 15)goto
_LL4B1;}_LL4B0: goto _LL4B2;_LL4B1: {struct Cyc_Absyn_TagType_struct*_tmp919=(
struct Cyc_Absyn_TagType_struct*)_tmp915;if(_tmp919->tag != 20)goto _LL4B3;}_LL4B2:
return 1;_LL4B3: {struct Cyc_Absyn_Evar_struct*_tmp91A=(struct Cyc_Absyn_Evar_struct*)
_tmp915;if(_tmp91A->tag != 1)goto _LL4B5;}_LL4B4: return Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL4B5:;_LL4B6:
return 0;_LL4AA:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp91B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL4B8: {struct Cyc_Absyn_FloatType_struct*_tmp91C=(
struct Cyc_Absyn_FloatType_struct*)_tmp91B;if(_tmp91C->tag != 7)goto _LL4BA;}_LL4B9:
goto _LL4BB;_LL4BA: {struct Cyc_Absyn_DoubleType_struct*_tmp91D=(struct Cyc_Absyn_DoubleType_struct*)
_tmp91B;if(_tmp91D->tag != 8)goto _LL4BC;}_LL4BB: return 1;_LL4BC:;_LL4BD: return 0;
_LL4B7:;};}int Cyc_Tcutil_is_function_type(void*t){void*_tmp91E=Cyc_Tcutil_compress(
t);_LL4BF: {struct Cyc_Absyn_FnType_struct*_tmp91F=(struct Cyc_Absyn_FnType_struct*)
_tmp91E;if(_tmp91F->tag != 10)goto _LL4C1;}_LL4C0: return 1;_LL4C1:;_LL4C2: return 0;
_LL4BE:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp1360;struct _tuple0 _tmp921=(_tmp1360.f1=t1,((_tmp1360.f2=t2,_tmp1360)));void*
_tmp922;int _tmp924;void*_tmp925;int _tmp927;void*_tmp928;void*_tmp92A;void*
_tmp92C;void*_tmp92E;void*_tmp930;enum Cyc_Absyn_Sign _tmp932;enum Cyc_Absyn_Size_of
_tmp933;void*_tmp934;enum Cyc_Absyn_Sign _tmp936;enum Cyc_Absyn_Size_of _tmp937;
void*_tmp938;enum Cyc_Absyn_Size_of _tmp93A;void*_tmp93B;enum Cyc_Absyn_Size_of
_tmp93D;void*_tmp93E;enum Cyc_Absyn_Sign _tmp940;enum Cyc_Absyn_Size_of _tmp941;
void*_tmp942;enum Cyc_Absyn_Sign _tmp944;enum Cyc_Absyn_Size_of _tmp945;void*
_tmp946;enum Cyc_Absyn_Sign _tmp948;enum Cyc_Absyn_Size_of _tmp949;void*_tmp94A;
enum Cyc_Absyn_Sign _tmp94C;enum Cyc_Absyn_Size_of _tmp94D;void*_tmp94E;enum Cyc_Absyn_Size_of
_tmp950;void*_tmp951;enum Cyc_Absyn_Size_of _tmp953;_LL4C4: _tmp922=_tmp921.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp923=(struct Cyc_Absyn_DoubleType_struct*)
_tmp922;if(_tmp923->tag != 8)goto _LL4C6;else{_tmp924=_tmp923->f1;}};_tmp925=
_tmp921.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp926=(struct Cyc_Absyn_DoubleType_struct*)
_tmp925;if(_tmp926->tag != 8)goto _LL4C6;else{_tmp927=_tmp926->f1;}};_LL4C5: if(
_tmp924)return t1;else{return t2;}_LL4C6: _tmp928=_tmp921.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp929=(struct Cyc_Absyn_DoubleType_struct*)_tmp928;if(_tmp929->tag != 8)goto
_LL4C8;};_LL4C7: return t1;_LL4C8: _tmp92A=_tmp921.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp92B=(struct Cyc_Absyn_DoubleType_struct*)_tmp92A;if(_tmp92B->tag != 8)goto
_LL4CA;};_LL4C9: return t2;_LL4CA: _tmp92C=_tmp921.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp92D=(struct Cyc_Absyn_FloatType_struct*)_tmp92C;if(_tmp92D->tag != 7)goto
_LL4CC;};_LL4CB: goto _LL4CD;_LL4CC: _tmp92E=_tmp921.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp92F=(struct Cyc_Absyn_FloatType_struct*)_tmp92E;if(_tmp92F->tag != 7)goto
_LL4CE;};_LL4CD: return(void*)& Cyc_Absyn_FloatType_val;_LL4CE: _tmp930=_tmp921.f1;{
struct Cyc_Absyn_IntType_struct*_tmp931=(struct Cyc_Absyn_IntType_struct*)_tmp930;
if(_tmp931->tag != 6)goto _LL4D0;else{_tmp932=_tmp931->f1;if(_tmp932 != Cyc_Absyn_Unsigned)
goto _LL4D0;_tmp933=_tmp931->f2;if(_tmp933 != Cyc_Absyn_LongLong_sz)goto _LL4D0;}};
_LL4CF: goto _LL4D1;_LL4D0: _tmp934=_tmp921.f2;{struct Cyc_Absyn_IntType_struct*
_tmp935=(struct Cyc_Absyn_IntType_struct*)_tmp934;if(_tmp935->tag != 6)goto _LL4D2;
else{_tmp936=_tmp935->f1;if(_tmp936 != Cyc_Absyn_Unsigned)goto _LL4D2;_tmp937=
_tmp935->f2;if(_tmp937 != Cyc_Absyn_LongLong_sz)goto _LL4D2;}};_LL4D1: return Cyc_Absyn_ulonglong_typ;
_LL4D2: _tmp938=_tmp921.f1;{struct Cyc_Absyn_IntType_struct*_tmp939=(struct Cyc_Absyn_IntType_struct*)
_tmp938;if(_tmp939->tag != 6)goto _LL4D4;else{_tmp93A=_tmp939->f2;if(_tmp93A != Cyc_Absyn_LongLong_sz)
goto _LL4D4;}};_LL4D3: goto _LL4D5;_LL4D4: _tmp93B=_tmp921.f2;{struct Cyc_Absyn_IntType_struct*
_tmp93C=(struct Cyc_Absyn_IntType_struct*)_tmp93B;if(_tmp93C->tag != 6)goto _LL4D6;
else{_tmp93D=_tmp93C->f2;if(_tmp93D != Cyc_Absyn_LongLong_sz)goto _LL4D6;}};_LL4D5:
return Cyc_Absyn_slonglong_typ;_LL4D6: _tmp93E=_tmp921.f1;{struct Cyc_Absyn_IntType_struct*
_tmp93F=(struct Cyc_Absyn_IntType_struct*)_tmp93E;if(_tmp93F->tag != 6)goto _LL4D8;
else{_tmp940=_tmp93F->f1;if(_tmp940 != Cyc_Absyn_Unsigned)goto _LL4D8;_tmp941=
_tmp93F->f2;if(_tmp941 != Cyc_Absyn_Long_sz)goto _LL4D8;}};_LL4D7: goto _LL4D9;
_LL4D8: _tmp942=_tmp921.f2;{struct Cyc_Absyn_IntType_struct*_tmp943=(struct Cyc_Absyn_IntType_struct*)
_tmp942;if(_tmp943->tag != 6)goto _LL4DA;else{_tmp944=_tmp943->f1;if(_tmp944 != Cyc_Absyn_Unsigned)
goto _LL4DA;_tmp945=_tmp943->f2;if(_tmp945 != Cyc_Absyn_Long_sz)goto _LL4DA;}};
_LL4D9: return Cyc_Absyn_ulong_typ;_LL4DA: _tmp946=_tmp921.f1;{struct Cyc_Absyn_IntType_struct*
_tmp947=(struct Cyc_Absyn_IntType_struct*)_tmp946;if(_tmp947->tag != 6)goto _LL4DC;
else{_tmp948=_tmp947->f1;if(_tmp948 != Cyc_Absyn_Unsigned)goto _LL4DC;_tmp949=
_tmp947->f2;if(_tmp949 != Cyc_Absyn_Int_sz)goto _LL4DC;}};_LL4DB: goto _LL4DD;_LL4DC:
_tmp94A=_tmp921.f2;{struct Cyc_Absyn_IntType_struct*_tmp94B=(struct Cyc_Absyn_IntType_struct*)
_tmp94A;if(_tmp94B->tag != 6)goto _LL4DE;else{_tmp94C=_tmp94B->f1;if(_tmp94C != Cyc_Absyn_Unsigned)
goto _LL4DE;_tmp94D=_tmp94B->f2;if(_tmp94D != Cyc_Absyn_Int_sz)goto _LL4DE;}};
_LL4DD: return Cyc_Absyn_uint_typ;_LL4DE: _tmp94E=_tmp921.f1;{struct Cyc_Absyn_IntType_struct*
_tmp94F=(struct Cyc_Absyn_IntType_struct*)_tmp94E;if(_tmp94F->tag != 6)goto _LL4E0;
else{_tmp950=_tmp94F->f2;if(_tmp950 != Cyc_Absyn_Long_sz)goto _LL4E0;}};_LL4DF:
goto _LL4E1;_LL4E0: _tmp951=_tmp921.f2;{struct Cyc_Absyn_IntType_struct*_tmp952=(
struct Cyc_Absyn_IntType_struct*)_tmp951;if(_tmp952->tag != 6)goto _LL4E2;else{
_tmp953=_tmp952->f2;if(_tmp953 != Cyc_Absyn_Long_sz)goto _LL4E2;}};_LL4E1: return
Cyc_Absyn_slong_typ;_LL4E2:;_LL4E3: return Cyc_Absyn_sint_typ;_LL4C3:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp954=e->r;struct Cyc_Core_Opt*_tmp956;_LL4E5: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp955=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp954;if(_tmp955->tag != 4)goto _LL4E7;else{_tmp956=_tmp955->f2;if(_tmp956 != 0)
goto _LL4E7;}}_LL4E6:{const char*_tmp1363;void*_tmp1362;(_tmp1362=0,Cyc_Tcutil_warn(
e->loc,((_tmp1363="assignment in test",_tag_dyneither(_tmp1363,sizeof(char),19))),
_tag_dyneither(_tmp1362,sizeof(void*),0)));}goto _LL4E4;_LL4E7:;_LL4E8: goto _LL4E4;
_LL4E4:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1364;struct _tuple0 _tmp95A=(
_tmp1364.f1=c1,((_tmp1364.f2=c2,_tmp1364)));void*_tmp95B;struct Cyc_Absyn_Kind*
_tmp95D;void*_tmp95E;struct Cyc_Absyn_Kind*_tmp960;void*_tmp961;struct Cyc_Core_Opt*
_tmp963;struct Cyc_Core_Opt**_tmp964;void*_tmp965;struct Cyc_Core_Opt*_tmp967;
struct Cyc_Core_Opt**_tmp968;void*_tmp969;struct Cyc_Core_Opt*_tmp96B;struct Cyc_Core_Opt**
_tmp96C;struct Cyc_Absyn_Kind*_tmp96D;void*_tmp96E;struct Cyc_Absyn_Kind*_tmp970;
void*_tmp971;struct Cyc_Absyn_Kind*_tmp973;void*_tmp974;struct Cyc_Core_Opt*
_tmp976;struct Cyc_Core_Opt**_tmp977;struct Cyc_Absyn_Kind*_tmp978;void*_tmp979;
struct Cyc_Core_Opt*_tmp97B;struct Cyc_Core_Opt**_tmp97C;struct Cyc_Absyn_Kind*
_tmp97D;void*_tmp97E;struct Cyc_Core_Opt*_tmp980;struct Cyc_Core_Opt**_tmp981;
struct Cyc_Absyn_Kind*_tmp982;_LL4EA: _tmp95B=_tmp95A.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp95C=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95B;if(_tmp95C->tag != 0)goto _LL4EC;
else{_tmp95D=_tmp95C->f1;}};_tmp95E=_tmp95A.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp95F=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95E;if(_tmp95F->tag != 0)goto _LL4EC;
else{_tmp960=_tmp95F->f1;}};_LL4EB: return _tmp95D == _tmp960;_LL4EC: _tmp961=
_tmp95A.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp962=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp961;if(_tmp962->tag != 1)goto _LL4EE;else{_tmp963=_tmp962->f1;_tmp964=(struct
Cyc_Core_Opt**)& _tmp962->f1;}};_LL4ED:{struct Cyc_Core_Opt*_tmp1365;*_tmp964=((
_tmp1365=_cycalloc(sizeof(*_tmp1365)),((_tmp1365->v=c1,_tmp1365))));}return 1;
_LL4EE: _tmp965=_tmp95A.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp966=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp965;if(_tmp966->tag != 1)goto _LL4F0;else{_tmp967=_tmp966->f1;_tmp968=(struct
Cyc_Core_Opt**)& _tmp966->f1;}};_LL4EF:{struct Cyc_Core_Opt*_tmp1366;*_tmp968=((
_tmp1366=_cycalloc(sizeof(*_tmp1366)),((_tmp1366->v=c2,_tmp1366))));}return 1;
_LL4F0: _tmp969=_tmp95A.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp96A=(struct Cyc_Absyn_Less_kb_struct*)
_tmp969;if(_tmp96A->tag != 2)goto _LL4F2;else{_tmp96B=_tmp96A->f1;_tmp96C=(struct
Cyc_Core_Opt**)& _tmp96A->f1;_tmp96D=_tmp96A->f2;}};_tmp96E=_tmp95A.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp96F=(struct Cyc_Absyn_Eq_kb_struct*)_tmp96E;if(_tmp96F->tag != 0)goto _LL4F2;
else{_tmp970=_tmp96F->f1;}};_LL4F1: if(Cyc_Tcutil_kind_leq(_tmp970,_tmp96D)){{
struct Cyc_Core_Opt*_tmp1367;*_tmp96C=((_tmp1367=_cycalloc(sizeof(*_tmp1367)),((
_tmp1367->v=c2,_tmp1367))));}return 1;}else{return 0;}_LL4F2: _tmp971=_tmp95A.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp972=(struct Cyc_Absyn_Eq_kb_struct*)_tmp971;if(
_tmp972->tag != 0)goto _LL4F4;else{_tmp973=_tmp972->f1;}};_tmp974=_tmp95A.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp975=(struct Cyc_Absyn_Less_kb_struct*)_tmp974;
if(_tmp975->tag != 2)goto _LL4F4;else{_tmp976=_tmp975->f1;_tmp977=(struct Cyc_Core_Opt**)&
_tmp975->f1;_tmp978=_tmp975->f2;}};_LL4F3: if(Cyc_Tcutil_kind_leq(_tmp973,_tmp978)){{
struct Cyc_Core_Opt*_tmp1368;*_tmp977=((_tmp1368=_cycalloc(sizeof(*_tmp1368)),((
_tmp1368->v=c1,_tmp1368))));}return 1;}else{return 0;}_LL4F4: _tmp979=_tmp95A.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp97A=(struct Cyc_Absyn_Less_kb_struct*)_tmp979;
if(_tmp97A->tag != 2)goto _LL4E9;else{_tmp97B=_tmp97A->f1;_tmp97C=(struct Cyc_Core_Opt**)&
_tmp97A->f1;_tmp97D=_tmp97A->f2;}};_tmp97E=_tmp95A.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp97F=(struct Cyc_Absyn_Less_kb_struct*)_tmp97E;if(_tmp97F->tag != 2)goto _LL4E9;
else{_tmp980=_tmp97F->f1;_tmp981=(struct Cyc_Core_Opt**)& _tmp97F->f1;_tmp982=
_tmp97F->f2;}};_LL4F5: if(Cyc_Tcutil_kind_leq(_tmp97D,_tmp982)){{struct Cyc_Core_Opt*
_tmp1369;*_tmp981=((_tmp1369=_cycalloc(sizeof(*_tmp1369)),((_tmp1369->v=c1,
_tmp1369))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp982,_tmp97D)){{struct Cyc_Core_Opt*
_tmp136A;*_tmp97C=((_tmp136A=_cycalloc(sizeof(*_tmp136A)),((_tmp136A->v=c2,
_tmp136A))));}return 1;}else{return 0;}}_LL4E9:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp136E;void*_tmp136D[1];struct Cyc_Int_pa_struct _tmp136C;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp136C.tag=1,((_tmp136C.f1=(
unsigned long)i,((_tmp136D[0]=& _tmp136C,Cyc_aprintf(((_tmp136E="#%d",
_tag_dyneither(_tmp136E,sizeof(char),4))),_tag_dyneither(_tmp136D,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp1371;struct Cyc_Absyn_Tvar*_tmp1370;return(
_tmp1370=_cycalloc(sizeof(*_tmp1370)),((_tmp1370->name=((_tmp1371=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp1371[0]=s,_tmp1371)))),((_tmp1370->identity=
- 1,((_tmp1370->kind=k,_tmp1370)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp98E=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp98E,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1375;void*_tmp1374[1];struct Cyc_String_pa_struct
_tmp1373;(_tmp1373.tag=0,((_tmp1373.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp1374[0]=& _tmp1373,Cyc_printf(((_tmp1375="%s",
_tag_dyneither(_tmp1375,sizeof(char),3))),_tag_dyneither(_tmp1374,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1376;struct
_dyneither_ptr _tmp992=Cyc_strconcat(((_tmp1376="`",_tag_dyneither(_tmp1376,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1379;char _tmp1378;
struct _dyneither_ptr _tmp1377;(_tmp1377=_dyneither_ptr_plus(_tmp992,sizeof(char),
1),((_tmp1378=*((char*)_check_dyneither_subscript(_tmp1377,sizeof(char),0)),((
_tmp1379='t',((_get_dyneither_size(_tmp1377,sizeof(char))== 1  && (_tmp1378 == '\000'
 && _tmp1379 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1377.curr)=_tmp1379)))))));}{
struct _dyneither_ptr*_tmp137A;t->name=((_tmp137A=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp137A[0]=(struct _dyneither_ptr)_tmp992,_tmp137A))));};};}
struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple25*x){struct Cyc_Core_Opt*
_tmp137D;struct _tuple9*_tmp137C;return(_tmp137C=_cycalloc(sizeof(*_tmp137C)),((
_tmp137C->f1=(struct Cyc_Core_Opt*)((_tmp137D=_cycalloc(sizeof(*_tmp137D)),((
_tmp137D->v=(*x).f1,_tmp137D)))),((_tmp137C->f2=(*x).f2,((_tmp137C->f3=(*x).f3,
_tmp137C)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp99A=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp137E;_tmp99A=((_tmp137E=_cycalloc(sizeof(*_tmp137E)),((
_tmp137E->hd=(void*)atts->hd,((_tmp137E->tl=_tmp99A,_tmp137E))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp1384;struct Cyc_Absyn_FnInfo _tmp1383;struct Cyc_Absyn_FnType_struct*_tmp1382;
return(void*)((_tmp1382=_cycalloc(sizeof(*_tmp1382)),((_tmp1382[0]=((_tmp1384.tag=
10,((_tmp1384.f1=((_tmp1383.tvars=fd->tvs,((_tmp1383.effect=fd->effect,((
_tmp1383.ret_tqual=fd->ret_tqual,((_tmp1383.ret_typ=fd->ret_type,((_tmp1383.args=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple25*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1383.c_varargs=fd->c_varargs,((
_tmp1383.cyc_varargs=fd->cyc_varargs,((_tmp1383.rgn_po=fd->rgn_po,((_tmp1383.attributes=
_tmp99A,_tmp1383)))))))))))))))))),_tmp1384)))),_tmp1382))));};}return(void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}struct _tuple26{void*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(struct _tuple26*
t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct _tuple11*
_tmp1385;return(_tmp1385=_cycalloc(sizeof(*_tmp1385)),((_tmp1385->f1=(*pr).f1,((
_tmp1385->f2=t,_tmp1385)))));}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple28{struct _tuple27*f1;void*f2;};static struct _tuple28*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple9*y){struct _tuple27*_tmp1388;struct _tuple28*
_tmp1387;return(_tmp1387=_region_malloc(rgn,sizeof(*_tmp1387)),((_tmp1387->f1=((
_tmp1388=_region_malloc(rgn,sizeof(*_tmp1388)),((_tmp1388->f1=(*y).f1,((_tmp1388->f2=(*
y).f2,_tmp1388)))))),((_tmp1387->f2=(*y).f3,_tmp1387)))));}static struct _tuple9*
Cyc_Tcutil_substitute_f2(struct _tuple28*w){struct _tuple27*_tmp9A3;void*_tmp9A4;
struct _tuple28 _tmp9A2=*w;_tmp9A3=_tmp9A2.f1;_tmp9A4=_tmp9A2.f2;{struct Cyc_Core_Opt*
_tmp9A6;struct Cyc_Absyn_Tqual _tmp9A7;struct _tuple27 _tmp9A5=*_tmp9A3;_tmp9A6=
_tmp9A5.f1;_tmp9A7=_tmp9A5.f2;{struct _tuple9*_tmp1389;return(_tmp1389=_cycalloc(
sizeof(*_tmp1389)),((_tmp1389->f1=_tmp9A6,((_tmp1389->f2=_tmp9A7,((_tmp1389->f3=
_tmp9A4,_tmp1389)))))));};};}static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp138A;return(
_tmp138A=_cycalloc(sizeof(*_tmp138A)),((_tmp138A->name=f->name,((_tmp138A->tq=f->tq,((
_tmp138A->type=t,((_tmp138A->width=f->width,((_tmp138A->attributes=f->attributes,
_tmp138A)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*
_tmp138B;return(_tmp138B=_cycalloc(sizeof(*_tmp138B)),((_tmp138B->topt=old->topt,((
_tmp138B->r=r,((_tmp138B->loc=old->loc,((_tmp138B->annot=old->annot,_tmp138B)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e){void*_tmp9AB=e->r;enum Cyc_Absyn_Primop _tmp9B1;
struct Cyc_List_List*_tmp9B2;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_Absyn_Exp*
_tmp9B5;struct Cyc_Absyn_Exp*_tmp9B6;struct Cyc_Absyn_Exp*_tmp9B8;struct Cyc_Absyn_Exp*
_tmp9B9;struct Cyc_Absyn_Exp*_tmp9BB;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_Absyn_Exp*
_tmp9BE;struct Cyc_Absyn_Exp*_tmp9BF;void*_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C2;int
_tmp9C3;enum Cyc_Absyn_Coercion _tmp9C4;void*_tmp9C6;struct Cyc_Absyn_Exp*_tmp9C8;
void*_tmp9CA;void*_tmp9CB;void*_tmp9CD;_LL4F7: {struct Cyc_Absyn_Const_e_struct*
_tmp9AC=(struct Cyc_Absyn_Const_e_struct*)_tmp9AB;if(_tmp9AC->tag != 0)goto _LL4F9;}
_LL4F8: goto _LL4FA;_LL4F9: {struct Cyc_Absyn_Enum_e_struct*_tmp9AD=(struct Cyc_Absyn_Enum_e_struct*)
_tmp9AB;if(_tmp9AD->tag != 32)goto _LL4FB;}_LL4FA: goto _LL4FC;_LL4FB: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp9AE=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp9AB;if(_tmp9AE->tag != 33)goto
_LL4FD;}_LL4FC: goto _LL4FE;_LL4FD: {struct Cyc_Absyn_Var_e_struct*_tmp9AF=(struct
Cyc_Absyn_Var_e_struct*)_tmp9AB;if(_tmp9AF->tag != 1)goto _LL4FF;}_LL4FE: return e;
_LL4FF: {struct Cyc_Absyn_Primop_e_struct*_tmp9B0=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9AB;if(_tmp9B0->tag != 3)goto _LL501;else{_tmp9B1=_tmp9B0->f1;_tmp9B2=_tmp9B0->f2;}}
_LL500: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9B2)== 1){struct
Cyc_Absyn_Exp*_tmp9CE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp9B2))->hd;struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9CE);
if(_tmp9CF == _tmp9CE)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1391;struct Cyc_Absyn_Exp*
_tmp1390[1];struct Cyc_Absyn_Primop_e_struct*_tmp138F;return Cyc_Tcutil_copye(e,(
void*)((_tmp138F=_cycalloc(sizeof(*_tmp138F)),((_tmp138F[0]=((_tmp1391.tag=3,((
_tmp1391.f1=_tmp9B1,((_tmp1391.f2=((_tmp1390[0]=_tmp9CF,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1390,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp1391)))))),_tmp138F)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp9B2)== 2){struct Cyc_Absyn_Exp*_tmp9D3=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9B2))->hd;struct Cyc_Absyn_Exp*_tmp9D4=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9B2->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9D5=Cyc_Tcutil_rsubsexp(r,inst,_tmp9D3);struct Cyc_Absyn_Exp*
_tmp9D6=Cyc_Tcutil_rsubsexp(r,inst,_tmp9D4);if(_tmp9D5 == _tmp9D3  && _tmp9D6 == 
_tmp9D4)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1397;struct Cyc_Absyn_Exp*
_tmp1396[2];struct Cyc_Absyn_Primop_e_struct*_tmp1395;return Cyc_Tcutil_copye(e,(
void*)((_tmp1395=_cycalloc(sizeof(*_tmp1395)),((_tmp1395[0]=((_tmp1397.tag=3,((
_tmp1397.f1=_tmp9B1,((_tmp1397.f2=((_tmp1396[1]=_tmp9D6,((_tmp1396[0]=_tmp9D5,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1396,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1397)))))),_tmp1395)))));};}
else{const char*_tmp139A;void*_tmp1399;return(_tmp1399=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp139A="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp139A,sizeof(char),34))),_tag_dyneither(_tmp1399,sizeof(void*),
0)));}}_LL501: {struct Cyc_Absyn_Conditional_e_struct*_tmp9B3=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9AB;if(_tmp9B3->tag != 6)goto _LL503;else{_tmp9B4=_tmp9B3->f1;_tmp9B5=_tmp9B3->f2;
_tmp9B6=_tmp9B3->f3;}}_LL502: {struct Cyc_Absyn_Exp*_tmp9DC=Cyc_Tcutil_rsubsexp(r,
inst,_tmp9B4);struct Cyc_Absyn_Exp*_tmp9DD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B5);
struct Cyc_Absyn_Exp*_tmp9DE=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B6);if((_tmp9DC == 
_tmp9B4  && _tmp9DD == _tmp9B5) && _tmp9DE == _tmp9B6)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp139D;struct Cyc_Absyn_Conditional_e_struct*_tmp139C;return Cyc_Tcutil_copye(e,(
void*)((_tmp139C=_cycalloc(sizeof(*_tmp139C)),((_tmp139C[0]=((_tmp139D.tag=6,((
_tmp139D.f1=_tmp9DC,((_tmp139D.f2=_tmp9DD,((_tmp139D.f3=_tmp9DE,_tmp139D)))))))),
_tmp139C)))));};}_LL503: {struct Cyc_Absyn_And_e_struct*_tmp9B7=(struct Cyc_Absyn_And_e_struct*)
_tmp9AB;if(_tmp9B7->tag != 7)goto _LL505;else{_tmp9B8=_tmp9B7->f1;_tmp9B9=_tmp9B7->f2;}}
_LL504: {struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B8);struct
Cyc_Absyn_Exp*_tmp9E2=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B9);if(_tmp9E1 == _tmp9B8
 && _tmp9E2 == _tmp9B9)return e;{struct Cyc_Absyn_And_e_struct _tmp13A0;struct Cyc_Absyn_And_e_struct*
_tmp139F;return Cyc_Tcutil_copye(e,(void*)((_tmp139F=_cycalloc(sizeof(*_tmp139F)),((
_tmp139F[0]=((_tmp13A0.tag=7,((_tmp13A0.f1=_tmp9E1,((_tmp13A0.f2=_tmp9E2,
_tmp13A0)))))),_tmp139F)))));};}_LL505: {struct Cyc_Absyn_Or_e_struct*_tmp9BA=(
struct Cyc_Absyn_Or_e_struct*)_tmp9AB;if(_tmp9BA->tag != 8)goto _LL507;else{_tmp9BB=
_tmp9BA->f1;_tmp9BC=_tmp9BA->f2;}}_LL506: {struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9BB);struct Cyc_Absyn_Exp*_tmp9E6=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BC);
if(_tmp9E5 == _tmp9BB  && _tmp9E6 == _tmp9BC)return e;{struct Cyc_Absyn_Or_e_struct
_tmp13A3;struct Cyc_Absyn_Or_e_struct*_tmp13A2;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A2=_cycalloc(sizeof(*_tmp13A2)),((_tmp13A2[0]=((_tmp13A3.tag=8,((_tmp13A3.f1=
_tmp9E5,((_tmp13A3.f2=_tmp9E6,_tmp13A3)))))),_tmp13A2)))));};}_LL507: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9BD=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp9AB;if(_tmp9BD->tag != 9)goto _LL509;
else{_tmp9BE=_tmp9BD->f1;_tmp9BF=_tmp9BD->f2;}}_LL508: {struct Cyc_Absyn_Exp*
_tmp9E9=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BE);struct Cyc_Absyn_Exp*_tmp9EA=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9BF);if(_tmp9E9 == _tmp9BE  && _tmp9EA == _tmp9BF)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp13A6;struct Cyc_Absyn_SeqExp_e_struct*_tmp13A5;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A5=_cycalloc(sizeof(*_tmp13A5)),((_tmp13A5[0]=((_tmp13A6.tag=9,((_tmp13A6.f1=
_tmp9E9,((_tmp13A6.f2=_tmp9EA,_tmp13A6)))))),_tmp13A5)))));};}_LL509: {struct Cyc_Absyn_Cast_e_struct*
_tmp9C0=(struct Cyc_Absyn_Cast_e_struct*)_tmp9AB;if(_tmp9C0->tag != 15)goto _LL50B;
else{_tmp9C1=(void*)_tmp9C0->f1;_tmp9C2=_tmp9C0->f2;_tmp9C3=_tmp9C0->f3;_tmp9C4=
_tmp9C0->f4;}}_LL50A: {struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Tcutil_rsubsexp(r,inst,
_tmp9C2);void*_tmp9EE=Cyc_Tcutil_rsubstitute(r,inst,_tmp9C1);if(_tmp9ED == 
_tmp9C2  && _tmp9EE == _tmp9C1)return e;{struct Cyc_Absyn_Cast_e_struct _tmp13A9;
struct Cyc_Absyn_Cast_e_struct*_tmp13A8;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A8=_cycalloc(sizeof(*_tmp13A8)),((_tmp13A8[0]=((_tmp13A9.tag=15,((_tmp13A9.f1=(
void*)_tmp9EE,((_tmp13A9.f2=_tmp9ED,((_tmp13A9.f3=_tmp9C3,((_tmp13A9.f4=_tmp9C4,
_tmp13A9)))))))))),_tmp13A8)))));};}_LL50B: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9C5=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9AB;if(_tmp9C5->tag != 18)goto
_LL50D;else{_tmp9C6=(void*)_tmp9C5->f1;}}_LL50C: {void*_tmp9F1=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9C6);if(_tmp9F1 == _tmp9C6)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp13AC;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp13AB;return Cyc_Tcutil_copye(e,(
void*)((_tmp13AB=_cycalloc(sizeof(*_tmp13AB)),((_tmp13AB[0]=((_tmp13AC.tag=18,((
_tmp13AC.f1=(void*)_tmp9F1,_tmp13AC)))),_tmp13AB)))));};}_LL50D: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9C7=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp9AB;if(_tmp9C7->tag != 19)goto
_LL50F;else{_tmp9C8=_tmp9C7->f1;}}_LL50E: {struct Cyc_Absyn_Exp*_tmp9F4=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9C8);if(_tmp9F4 == _tmp9C8)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp13AF;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp13AE;return Cyc_Tcutil_copye(e,(
void*)((_tmp13AE=_cycalloc(sizeof(*_tmp13AE)),((_tmp13AE[0]=((_tmp13AF.tag=19,((
_tmp13AF.f1=_tmp9F4,_tmp13AF)))),_tmp13AE)))));};}_LL50F: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9C9=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp9AB;if(_tmp9C9->tag != 20)goto
_LL511;else{_tmp9CA=(void*)_tmp9C9->f1;_tmp9CB=(void*)_tmp9C9->f2;}}_LL510: {
void*_tmp9F7=Cyc_Tcutil_rsubstitute(r,inst,_tmp9CA);if(_tmp9F7 == _tmp9CA)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp13B2;struct Cyc_Absyn_Offsetof_e_struct*
_tmp13B1;return Cyc_Tcutil_copye(e,(void*)((_tmp13B1=_cycalloc(sizeof(*_tmp13B1)),((
_tmp13B1[0]=((_tmp13B2.tag=20,((_tmp13B2.f1=(void*)_tmp9F7,((_tmp13B2.f2=(void*)
_tmp9CB,_tmp13B2)))))),_tmp13B1)))));};}_LL511: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9CC=(struct Cyc_Absyn_Valueof_e_struct*)_tmp9AB;if(_tmp9CC->tag != 39)goto
_LL513;else{_tmp9CD=(void*)_tmp9CC->f1;}}_LL512: {void*_tmp9FA=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9CD);if(_tmp9FA == _tmp9CD)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp13B5;struct Cyc_Absyn_Valueof_e_struct*_tmp13B4;return Cyc_Tcutil_copye(e,(
void*)((_tmp13B4=_cycalloc(sizeof(*_tmp13B4)),((_tmp13B4[0]=((_tmp13B5.tag=39,((
_tmp13B5.f1=(void*)_tmp9FA,_tmp13B5)))),_tmp13B4)))));};}_LL513:;_LL514: {const
char*_tmp13B8;void*_tmp13B7;return(_tmp13B7=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13B8="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp13B8,sizeof(char),46))),_tag_dyneither(_tmp13B7,sizeof(void*),
0)));}_LL4F6:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9FF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpA01;
struct Cyc_Absyn_AggrInfo _tmpA03;union Cyc_Absyn_AggrInfoU _tmpA04;struct Cyc_List_List*
_tmpA05;struct Cyc_Absyn_DatatypeInfo _tmpA07;union Cyc_Absyn_DatatypeInfoU _tmpA08;
struct Cyc_List_List*_tmpA09;struct Cyc_Absyn_DatatypeFieldInfo _tmpA0B;union Cyc_Absyn_DatatypeFieldInfoU
_tmpA0C;struct Cyc_List_List*_tmpA0D;struct _tuple2*_tmpA0F;struct Cyc_List_List*
_tmpA10;struct Cyc_Absyn_Typedefdecl*_tmpA11;void**_tmpA12;struct Cyc_Absyn_ArrayInfo
_tmpA14;void*_tmpA15;struct Cyc_Absyn_Tqual _tmpA16;struct Cyc_Absyn_Exp*_tmpA17;
union Cyc_Absyn_Constraint*_tmpA18;struct Cyc_Position_Segment*_tmpA19;struct Cyc_Absyn_PtrInfo
_tmpA1B;void*_tmpA1C;struct Cyc_Absyn_Tqual _tmpA1D;struct Cyc_Absyn_PtrAtts _tmpA1E;
void*_tmpA1F;union Cyc_Absyn_Constraint*_tmpA20;union Cyc_Absyn_Constraint*_tmpA21;
union Cyc_Absyn_Constraint*_tmpA22;struct Cyc_Absyn_FnInfo _tmpA24;struct Cyc_List_List*
_tmpA25;struct Cyc_Core_Opt*_tmpA26;struct Cyc_Absyn_Tqual _tmpA27;void*_tmpA28;
struct Cyc_List_List*_tmpA29;int _tmpA2A;struct Cyc_Absyn_VarargInfo*_tmpA2B;struct
Cyc_List_List*_tmpA2C;struct Cyc_List_List*_tmpA2D;struct Cyc_List_List*_tmpA2F;
enum Cyc_Absyn_AggrKind _tmpA31;struct Cyc_List_List*_tmpA32;struct Cyc_Core_Opt*
_tmpA34;void*_tmpA36;void*_tmpA38;void*_tmpA39;void*_tmpA3B;struct Cyc_Absyn_Exp*
_tmpA3D;void*_tmpA48;void*_tmpA4A;struct Cyc_List_List*_tmpA4C;_LL516: {struct Cyc_Absyn_VarType_struct*
_tmpA00=(struct Cyc_Absyn_VarType_struct*)_tmp9FF;if(_tmpA00->tag != 2)goto _LL518;
else{_tmpA01=_tmpA00->f1;}}_LL517: {struct _handler_cons _tmpA4E;_push_handler(&
_tmpA4E);{int _tmpA50=0;if(setjmp(_tmpA4E.handler))_tmpA50=1;if(!_tmpA50){{void*
_tmpA51=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmpA01);_npop_handler(0);return _tmpA51;};_pop_handler();}else{void*_tmpA4F=(
void*)_exn_thrown;void*_tmpA53=_tmpA4F;_LL54F: {struct Cyc_Core_Not_found_struct*
_tmpA54=(struct Cyc_Core_Not_found_struct*)_tmpA53;if(_tmpA54->tag != Cyc_Core_Not_found)
goto _LL551;}_LL550: return t;_LL551:;_LL552:(void)_throw(_tmpA53);_LL54E:;}};}
_LL518: {struct Cyc_Absyn_AggrType_struct*_tmpA02=(struct Cyc_Absyn_AggrType_struct*)
_tmp9FF;if(_tmpA02->tag != 12)goto _LL51A;else{_tmpA03=_tmpA02->f1;_tmpA04=_tmpA03.aggr_info;
_tmpA05=_tmpA03.targs;}}_LL519: {struct Cyc_List_List*_tmpA55=Cyc_Tcutil_substs(
rgn,inst,_tmpA05);struct Cyc_Absyn_AggrType_struct _tmp13BE;struct Cyc_Absyn_AggrInfo
_tmp13BD;struct Cyc_Absyn_AggrType_struct*_tmp13BC;return _tmpA55 == _tmpA05?t:(
void*)((_tmp13BC=_cycalloc(sizeof(*_tmp13BC)),((_tmp13BC[0]=((_tmp13BE.tag=12,((
_tmp13BE.f1=((_tmp13BD.aggr_info=_tmpA04,((_tmp13BD.targs=_tmpA55,_tmp13BD)))),
_tmp13BE)))),_tmp13BC))));}_LL51A: {struct Cyc_Absyn_DatatypeType_struct*_tmpA06=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp9FF;if(_tmpA06->tag != 3)goto _LL51C;
else{_tmpA07=_tmpA06->f1;_tmpA08=_tmpA07.datatype_info;_tmpA09=_tmpA07.targs;}}
_LL51B: {struct Cyc_List_List*_tmpA59=Cyc_Tcutil_substs(rgn,inst,_tmpA09);struct
Cyc_Absyn_DatatypeType_struct _tmp13C4;struct Cyc_Absyn_DatatypeInfo _tmp13C3;
struct Cyc_Absyn_DatatypeType_struct*_tmp13C2;return _tmpA59 == _tmpA09?t:(void*)((
_tmp13C2=_cycalloc(sizeof(*_tmp13C2)),((_tmp13C2[0]=((_tmp13C4.tag=3,((_tmp13C4.f1=((
_tmp13C3.datatype_info=_tmpA08,((_tmp13C3.targs=_tmpA59,_tmp13C3)))),_tmp13C4)))),
_tmp13C2))));}_LL51C: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpA0A=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp9FF;if(_tmpA0A->tag != 4)goto _LL51E;else{
_tmpA0B=_tmpA0A->f1;_tmpA0C=_tmpA0B.field_info;_tmpA0D=_tmpA0B.targs;}}_LL51D: {
struct Cyc_List_List*_tmpA5D=Cyc_Tcutil_substs(rgn,inst,_tmpA0D);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp13CA;struct Cyc_Absyn_DatatypeFieldInfo _tmp13C9;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13C8;return _tmpA5D == _tmpA0D?t:(void*)((_tmp13C8=_cycalloc(sizeof(*_tmp13C8)),((
_tmp13C8[0]=((_tmp13CA.tag=4,((_tmp13CA.f1=((_tmp13C9.field_info=_tmpA0C,((
_tmp13C9.targs=_tmpA5D,_tmp13C9)))),_tmp13CA)))),_tmp13C8))));}_LL51E: {struct
Cyc_Absyn_TypedefType_struct*_tmpA0E=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9FF;if(_tmpA0E->tag != 18)goto _LL520;else{_tmpA0F=_tmpA0E->f1;_tmpA10=_tmpA0E->f2;
_tmpA11=_tmpA0E->f3;_tmpA12=_tmpA0E->f4;}}_LL51F: {struct Cyc_List_List*_tmpA61=
Cyc_Tcutil_substs(rgn,inst,_tmpA10);struct Cyc_Absyn_TypedefType_struct _tmp13CD;
struct Cyc_Absyn_TypedefType_struct*_tmp13CC;return _tmpA61 == _tmpA10?t:(void*)((
_tmp13CC=_cycalloc(sizeof(*_tmp13CC)),((_tmp13CC[0]=((_tmp13CD.tag=18,((_tmp13CD.f1=
_tmpA0F,((_tmp13CD.f2=_tmpA61,((_tmp13CD.f3=_tmpA11,((_tmp13CD.f4=_tmpA12,
_tmp13CD)))))))))),_tmp13CC))));}_LL520: {struct Cyc_Absyn_ArrayType_struct*
_tmpA13=(struct Cyc_Absyn_ArrayType_struct*)_tmp9FF;if(_tmpA13->tag != 9)goto
_LL522;else{_tmpA14=_tmpA13->f1;_tmpA15=_tmpA14.elt_type;_tmpA16=_tmpA14.tq;
_tmpA17=_tmpA14.num_elts;_tmpA18=_tmpA14.zero_term;_tmpA19=_tmpA14.zt_loc;}}
_LL521: {void*_tmpA64=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA15);struct Cyc_Absyn_ArrayType_struct
_tmp13D3;struct Cyc_Absyn_ArrayInfo _tmp13D2;struct Cyc_Absyn_ArrayType_struct*
_tmp13D1;return _tmpA64 == _tmpA15?t:(void*)((_tmp13D1=_cycalloc(sizeof(*_tmp13D1)),((
_tmp13D1[0]=((_tmp13D3.tag=9,((_tmp13D3.f1=((_tmp13D2.elt_type=_tmpA64,((
_tmp13D2.tq=_tmpA16,((_tmp13D2.num_elts=_tmpA17,((_tmp13D2.zero_term=_tmpA18,((
_tmp13D2.zt_loc=_tmpA19,_tmp13D2)))))))))),_tmp13D3)))),_tmp13D1))));}_LL522: {
struct Cyc_Absyn_PointerType_struct*_tmpA1A=(struct Cyc_Absyn_PointerType_struct*)
_tmp9FF;if(_tmpA1A->tag != 5)goto _LL524;else{_tmpA1B=_tmpA1A->f1;_tmpA1C=_tmpA1B.elt_typ;
_tmpA1D=_tmpA1B.elt_tq;_tmpA1E=_tmpA1B.ptr_atts;_tmpA1F=_tmpA1E.rgn;_tmpA20=
_tmpA1E.nullable;_tmpA21=_tmpA1E.bounds;_tmpA22=_tmpA1E.zero_term;}}_LL523: {
void*_tmpA68=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA1C);void*_tmpA69=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA1F);union Cyc_Absyn_Constraint*_tmpA6A=_tmpA21;{void*_tmpA6B=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA21);struct Cyc_Absyn_Exp*_tmpA6D;_LL554: {struct Cyc_Absyn_Upper_b_struct*
_tmpA6C=(struct Cyc_Absyn_Upper_b_struct*)_tmpA6B;if(_tmpA6C->tag != 1)goto _LL556;
else{_tmpA6D=_tmpA6C->f1;}}_LL555: {struct Cyc_Absyn_Exp*_tmpA6E=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA6D);if(_tmpA6E != _tmpA6D){struct Cyc_Absyn_Upper_b_struct _tmp13D6;
struct Cyc_Absyn_Upper_b_struct*_tmp13D5;_tmpA6A=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13D5=_cycalloc(sizeof(*_tmp13D5)),((
_tmp13D5[0]=((_tmp13D6.tag=1,((_tmp13D6.f1=_tmpA6E,_tmp13D6)))),_tmp13D5)))));}
goto _LL553;}_LL556:;_LL557: goto _LL553;_LL553:;}if((_tmpA68 == _tmpA1C  && _tmpA69
== _tmpA1F) && _tmpA6A == _tmpA21)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13E0;struct Cyc_Absyn_PtrAtts _tmp13DF;struct Cyc_Absyn_PtrInfo _tmp13DE;struct
Cyc_Absyn_PointerType_struct*_tmp13DD;return(void*)((_tmp13DD=_cycalloc(sizeof(*
_tmp13DD)),((_tmp13DD[0]=((_tmp13E0.tag=5,((_tmp13E0.f1=((_tmp13DE.elt_typ=
_tmpA68,((_tmp13DE.elt_tq=_tmpA1D,((_tmp13DE.ptr_atts=((_tmp13DF.rgn=_tmpA69,((
_tmp13DF.nullable=_tmpA20,((_tmp13DF.bounds=_tmpA6A,((_tmp13DF.zero_term=_tmpA22,((
_tmp13DF.ptrloc=0,_tmp13DF)))))))))),_tmp13DE)))))),_tmp13E0)))),_tmp13DD))));};}
_LL524: {struct Cyc_Absyn_FnType_struct*_tmpA23=(struct Cyc_Absyn_FnType_struct*)
_tmp9FF;if(_tmpA23->tag != 10)goto _LL526;else{_tmpA24=_tmpA23->f1;_tmpA25=_tmpA24.tvars;
_tmpA26=_tmpA24.effect;_tmpA27=_tmpA24.ret_tqual;_tmpA28=_tmpA24.ret_typ;_tmpA29=
_tmpA24.args;_tmpA2A=_tmpA24.c_varargs;_tmpA2B=_tmpA24.cyc_varargs;_tmpA2C=
_tmpA24.rgn_po;_tmpA2D=_tmpA24.attributes;}}_LL525:{struct Cyc_List_List*_tmpA75=
_tmpA25;for(0;_tmpA75 != 0;_tmpA75=_tmpA75->tl){struct _tuple15*_tmp13EA;struct Cyc_Absyn_VarType_struct
_tmp13E9;struct Cyc_Absyn_VarType_struct*_tmp13E8;struct Cyc_List_List*_tmp13E7;
inst=((_tmp13E7=_region_malloc(rgn,sizeof(*_tmp13E7)),((_tmp13E7->hd=((_tmp13EA=
_region_malloc(rgn,sizeof(*_tmp13EA)),((_tmp13EA->f1=(struct Cyc_Absyn_Tvar*)
_tmpA75->hd,((_tmp13EA->f2=(void*)((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((
_tmp13E8[0]=((_tmp13E9.tag=2,((_tmp13E9.f1=(struct Cyc_Absyn_Tvar*)_tmpA75->hd,
_tmp13E9)))),_tmp13E8)))),_tmp13EA)))))),((_tmp13E7->tl=inst,_tmp13E7))))));}}{
struct Cyc_List_List*_tmpA7B;struct Cyc_List_List*_tmpA7C;struct _tuple1 _tmpA7A=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA29));
_tmpA7B=_tmpA7A.f1;_tmpA7C=_tmpA7A.f2;{struct Cyc_List_List*_tmpA7D=Cyc_Tcutil_substs(
rgn,inst,_tmpA7C);struct Cyc_List_List*_tmpA7E=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA7B,_tmpA7D));struct Cyc_Core_Opt*
eff2;if(_tmpA26 == 0)eff2=0;else{void*_tmpA7F=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA26->v);if(_tmpA7F == (void*)_tmpA26->v)eff2=_tmpA26;else{struct Cyc_Core_Opt*
_tmp13EB;eff2=((_tmp13EB=_cycalloc(sizeof(*_tmp13EB)),((_tmp13EB->v=_tmpA7F,
_tmp13EB))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA2B == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA82;struct Cyc_Absyn_Tqual _tmpA83;void*
_tmpA84;int _tmpA85;struct Cyc_Absyn_VarargInfo _tmpA81=*_tmpA2B;_tmpA82=_tmpA81.name;
_tmpA83=_tmpA81.tq;_tmpA84=_tmpA81.type;_tmpA85=_tmpA81.inject;{void*_tmpA86=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA84);if(_tmpA86 == _tmpA84)cyc_varargs2=_tmpA2B;else{struct Cyc_Absyn_VarargInfo*
_tmp13EC;cyc_varargs2=((_tmp13EC=_cycalloc(sizeof(*_tmp13EC)),((_tmp13EC->name=
_tmpA82,((_tmp13EC->tq=_tmpA83,((_tmp13EC->type=_tmpA86,((_tmp13EC->inject=
_tmpA85,_tmp13EC))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA89;struct Cyc_List_List*_tmpA8A;struct _tuple1 _tmpA88=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA2C);_tmpA89=_tmpA88.f1;_tmpA8A=_tmpA88.f2;{struct Cyc_List_List*
_tmpA8B=Cyc_Tcutil_substs(rgn,inst,_tmpA89);struct Cyc_List_List*_tmpA8C=Cyc_Tcutil_substs(
rgn,inst,_tmpA8A);if(_tmpA8B == _tmpA89  && _tmpA8C == _tmpA8A)rgn_po2=_tmpA2C;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA8B,_tmpA8C);}{struct Cyc_Absyn_FnType_struct _tmp13F2;struct
Cyc_Absyn_FnInfo _tmp13F1;struct Cyc_Absyn_FnType_struct*_tmp13F0;return(void*)((
_tmp13F0=_cycalloc(sizeof(*_tmp13F0)),((_tmp13F0[0]=((_tmp13F2.tag=10,((_tmp13F2.f1=((
_tmp13F1.tvars=_tmpA25,((_tmp13F1.effect=eff2,((_tmp13F1.ret_tqual=_tmpA27,((
_tmp13F1.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA28),((_tmp13F1.args=
_tmpA7E,((_tmp13F1.c_varargs=_tmpA2A,((_tmp13F1.cyc_varargs=cyc_varargs2,((
_tmp13F1.rgn_po=rgn_po2,((_tmp13F1.attributes=_tmpA2D,_tmp13F1)))))))))))))))))),
_tmp13F2)))),_tmp13F0))));};};};};};};_LL526: {struct Cyc_Absyn_TupleType_struct*
_tmpA2E=(struct Cyc_Absyn_TupleType_struct*)_tmp9FF;if(_tmpA2E->tag != 11)goto
_LL528;else{_tmpA2F=_tmpA2E->f1;}}_LL527: {struct Cyc_List_List*_tmpA90=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA2F);struct Cyc_List_List*_tmpA91=
Cyc_Tcutil_substs(rgn,inst,_tmpA90);if(_tmpA91 == _tmpA90)return t;{struct Cyc_List_List*
_tmpA92=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA2F,_tmpA91);struct Cyc_Absyn_TupleType_struct _tmp13F5;struct Cyc_Absyn_TupleType_struct*
_tmp13F4;return(void*)((_tmp13F4=_cycalloc(sizeof(*_tmp13F4)),((_tmp13F4[0]=((
_tmp13F5.tag=11,((_tmp13F5.f1=_tmpA92,_tmp13F5)))),_tmp13F4))));};}_LL528: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA30=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9FF;if(_tmpA30->tag != 13)goto _LL52A;else{_tmpA31=_tmpA30->f1;_tmpA32=_tmpA30->f2;}}
_LL529: {struct Cyc_List_List*_tmpA95=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA32);struct Cyc_List_List*_tmpA96=Cyc_Tcutil_substs(
rgn,inst,_tmpA95);if(_tmpA96 == _tmpA95)return t;{struct Cyc_List_List*_tmpA97=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA32,_tmpA96);struct Cyc_Absyn_AnonAggrType_struct _tmp13F8;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13F7;return(void*)((_tmp13F7=_cycalloc(sizeof(*_tmp13F7)),((_tmp13F7[0]=((
_tmp13F8.tag=13,((_tmp13F8.f1=_tmpA31,((_tmp13F8.f2=_tmpA97,_tmp13F8)))))),
_tmp13F7))));};}_LL52A: {struct Cyc_Absyn_Evar_struct*_tmpA33=(struct Cyc_Absyn_Evar_struct*)
_tmp9FF;if(_tmpA33->tag != 1)goto _LL52C;else{_tmpA34=_tmpA33->f2;}}_LL52B: if(
_tmpA34 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA34->v);else{return
t;}_LL52C: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA35=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9FF;if(_tmpA35->tag != 16)goto _LL52E;else{_tmpA36=(void*)_tmpA35->f1;}}_LL52D: {
void*_tmpA9A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA36);struct Cyc_Absyn_RgnHandleType_struct
_tmp13FB;struct Cyc_Absyn_RgnHandleType_struct*_tmp13FA;return _tmpA9A == _tmpA36?t:(
void*)((_tmp13FA=_cycalloc(sizeof(*_tmp13FA)),((_tmp13FA[0]=((_tmp13FB.tag=16,((
_tmp13FB.f1=(void*)_tmpA9A,_tmp13FB)))),_tmp13FA))));}_LL52E: {struct Cyc_Absyn_DynRgnType_struct*
_tmpA37=(struct Cyc_Absyn_DynRgnType_struct*)_tmp9FF;if(_tmpA37->tag != 17)goto
_LL530;else{_tmpA38=(void*)_tmpA37->f1;_tmpA39=(void*)_tmpA37->f2;}}_LL52F: {
void*_tmpA9D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA38);void*_tmpA9E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA39);struct Cyc_Absyn_DynRgnType_struct _tmp13FE;struct Cyc_Absyn_DynRgnType_struct*
_tmp13FD;return _tmpA9D == _tmpA38  && _tmpA9E == _tmpA39?t:(void*)((_tmp13FD=
_cycalloc(sizeof(*_tmp13FD)),((_tmp13FD[0]=((_tmp13FE.tag=17,((_tmp13FE.f1=(void*)
_tmpA9D,((_tmp13FE.f2=(void*)_tmpA9E,_tmp13FE)))))),_tmp13FD))));}_LL530: {
struct Cyc_Absyn_TagType_struct*_tmpA3A=(struct Cyc_Absyn_TagType_struct*)_tmp9FF;
if(_tmpA3A->tag != 20)goto _LL532;else{_tmpA3B=(void*)_tmpA3A->f1;}}_LL531: {void*
_tmpAA1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA3B);struct Cyc_Absyn_TagType_struct
_tmp1401;struct Cyc_Absyn_TagType_struct*_tmp1400;return _tmpAA1 == _tmpA3B?t:(void*)((
_tmp1400=_cycalloc(sizeof(*_tmp1400)),((_tmp1400[0]=((_tmp1401.tag=20,((_tmp1401.f1=(
void*)_tmpAA1,_tmp1401)))),_tmp1400))));}_LL532: {struct Cyc_Absyn_ValueofType_struct*
_tmpA3C=(struct Cyc_Absyn_ValueofType_struct*)_tmp9FF;if(_tmpA3C->tag != 19)goto
_LL534;else{_tmpA3D=_tmpA3C->f1;}}_LL533: {struct Cyc_Absyn_Exp*_tmpAA4=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA3D);struct Cyc_Absyn_ValueofType_struct _tmp1404;struct Cyc_Absyn_ValueofType_struct*
_tmp1403;return _tmpAA4 == _tmpA3D?t:(void*)((_tmp1403=_cycalloc(sizeof(*_tmp1403)),((
_tmp1403[0]=((_tmp1404.tag=19,((_tmp1404.f1=_tmpAA4,_tmp1404)))),_tmp1403))));}
_LL534: {struct Cyc_Absyn_EnumType_struct*_tmpA3E=(struct Cyc_Absyn_EnumType_struct*)
_tmp9FF;if(_tmpA3E->tag != 14)goto _LL536;}_LL535: goto _LL537;_LL536: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpA3F=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp9FF;if(_tmpA3F->tag != 15)goto
_LL538;}_LL537: goto _LL539;_LL538: {struct Cyc_Absyn_VoidType_struct*_tmpA40=(
struct Cyc_Absyn_VoidType_struct*)_tmp9FF;if(_tmpA40->tag != 0)goto _LL53A;}_LL539:
goto _LL53B;_LL53A: {struct Cyc_Absyn_IntType_struct*_tmpA41=(struct Cyc_Absyn_IntType_struct*)
_tmp9FF;if(_tmpA41->tag != 6)goto _LL53C;}_LL53B: goto _LL53D;_LL53C: {struct Cyc_Absyn_FloatType_struct*
_tmpA42=(struct Cyc_Absyn_FloatType_struct*)_tmp9FF;if(_tmpA42->tag != 7)goto
_LL53E;}_LL53D: goto _LL53F;_LL53E: {struct Cyc_Absyn_DoubleType_struct*_tmpA43=(
struct Cyc_Absyn_DoubleType_struct*)_tmp9FF;if(_tmpA43->tag != 8)goto _LL540;}
_LL53F: goto _LL541;_LL540: {struct Cyc_Absyn_RefCntRgn_struct*_tmpA44=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp9FF;if(_tmpA44->tag != 23)goto _LL542;}_LL541: goto _LL543;_LL542: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpA45=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp9FF;if(_tmpA45->tag != 22)goto
_LL544;}_LL543: goto _LL545;_LL544: {struct Cyc_Absyn_HeapRgn_struct*_tmpA46=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp9FF;if(_tmpA46->tag != 21)goto _LL546;}_LL545:
return t;_LL546: {struct Cyc_Absyn_RgnsEff_struct*_tmpA47=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9FF;if(_tmpA47->tag != 26)goto _LL548;else{_tmpA48=(void*)_tmpA47->f1;}}_LL547: {
void*_tmpAA7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA48);struct Cyc_Absyn_RgnsEff_struct
_tmp1407;struct Cyc_Absyn_RgnsEff_struct*_tmp1406;return _tmpAA7 == _tmpA48?t:(void*)((
_tmp1406=_cycalloc(sizeof(*_tmp1406)),((_tmp1406[0]=((_tmp1407.tag=26,((_tmp1407.f1=(
void*)_tmpAA7,_tmp1407)))),_tmp1406))));}_LL548: {struct Cyc_Absyn_AccessEff_struct*
_tmpA49=(struct Cyc_Absyn_AccessEff_struct*)_tmp9FF;if(_tmpA49->tag != 24)goto
_LL54A;else{_tmpA4A=(void*)_tmpA49->f1;}}_LL549: {void*_tmpAAA=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA4A);struct Cyc_Absyn_AccessEff_struct _tmp140A;struct Cyc_Absyn_AccessEff_struct*
_tmp1409;return _tmpAAA == _tmpA4A?t:(void*)((_tmp1409=_cycalloc(sizeof(*_tmp1409)),((
_tmp1409[0]=((_tmp140A.tag=24,((_tmp140A.f1=(void*)_tmpAAA,_tmp140A)))),_tmp1409))));}
_LL54A: {struct Cyc_Absyn_JoinEff_struct*_tmpA4B=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9FF;if(_tmpA4B->tag != 25)goto _LL54C;else{_tmpA4C=_tmpA4B->f1;}}_LL54B: {
struct Cyc_List_List*_tmpAAD=Cyc_Tcutil_substs(rgn,inst,_tmpA4C);struct Cyc_Absyn_JoinEff_struct
_tmp140D;struct Cyc_Absyn_JoinEff_struct*_tmp140C;return _tmpAAD == _tmpA4C?t:(void*)((
_tmp140C=_cycalloc(sizeof(*_tmp140C)),((_tmp140C[0]=((_tmp140D.tag=25,((_tmp140D.f1=
_tmpAAD,_tmp140D)))),_tmp140C))));}_LL54C: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpA4D=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp9FF;if(_tmpA4D->tag != 27)goto
_LL515;}_LL54D: {const char*_tmp1410;void*_tmp140F;(_tmp140F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1410="found typedecltype in rsubs",
_tag_dyneither(_tmp1410,sizeof(char),28))),_tag_dyneither(_tmp140F,sizeof(void*),
0)));}_LL515:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*
rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*
_tmpAB2=(void*)ts->hd;struct Cyc_List_List*_tmpAB3=ts->tl;void*_tmpAB4=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpAB2);struct Cyc_List_List*_tmpAB5=Cyc_Tcutil_substs(rgn,inst,_tmpAB3);
if(_tmpAB2 == _tmpAB4  && _tmpAB3 == _tmpAB5)return ts;{struct Cyc_List_List*_tmp1411;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp1411=_cycalloc(sizeof(*
_tmp1411)),((_tmp1411->hd=_tmpAB4,((_tmp1411->tl=_tmpAB5,_tmp1411)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpAB7=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1414;struct _tuple15*_tmp1413;return(
_tmp1413=_cycalloc(sizeof(*_tmp1413)),((_tmp1413->f1=tv,((_tmp1413->f2=Cyc_Absyn_new_evar(
_tmpAB7,((_tmp1414=_cycalloc(sizeof(*_tmp1414)),((_tmp1414->v=s,_tmp1414))))),
_tmp1413)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpABB;struct Cyc_List_List*_tmpABC;
struct _RegionHandle*_tmpABD;struct _tuple16*_tmpABA=env;_tmpABB=*_tmpABA;_tmpABC=
_tmpABB.f1;_tmpABD=_tmpABB.f2;{struct Cyc_Core_Opt*_tmpABE=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1417;struct
_tuple15*_tmp1416;return(_tmp1416=_region_malloc(_tmpABD,sizeof(*_tmp1416)),((
_tmp1416->f1=tv,((_tmp1416->f2=Cyc_Absyn_new_evar(_tmpABE,((_tmp1417=_cycalloc(
sizeof(*_tmp1417)),((_tmp1417->v=_tmpABC,_tmp1417))))),_tmp1416)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp141D;void*_tmp141C[3];
struct Cyc_String_pa_struct _tmp141B;struct Cyc_String_pa_struct _tmp141A;struct Cyc_String_pa_struct
_tmp1419;(_tmp1419.tag=0,((_tmp1419.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp141A.tag=0,((_tmp141A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp141B.tag=0,((_tmp141B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp141C[0]=& _tmp141B,((_tmp141C[1]=& _tmp141A,((_tmp141C[2]=& _tmp1419,Cyc_Tcutil_terr(
loc,((_tmp141D="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp141D,sizeof(char),59))),_tag_dyneither(_tmp141C,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1420;void*_tmp141F;(_tmp141F=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1420="same type variable has different identity!",
_tag_dyneither(_tmp1420,sizeof(char),43))),_tag_dyneither(_tmp141F,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp1421;return(_tmp1421=_cycalloc(sizeof(*_tmp1421)),((_tmp1421->hd=tv,((
_tmp1421->tl=tvs,_tmp1421)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1424;void*_tmp1423;(_tmp1423=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1424="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp1424,sizeof(char),39))),_tag_dyneither(_tmp1423,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpACB=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpACB->identity == - 1){const char*
_tmp1427;void*_tmp1426;(_tmp1426=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1427="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1427,sizeof(char),41))),_tag_dyneither(_tmp1426,sizeof(void*),
0)));}if(_tmpACB->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1428;return(_tmp1428=_cycalloc(sizeof(*_tmp1428)),((_tmp1428->hd=tv,((
_tmp1428->tl=tvs,_tmp1428)))));};}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp142B;void*_tmp142A;(_tmp142A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp142B="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp142B,sizeof(char),44))),_tag_dyneither(_tmp142A,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple29
_tmpAD2;struct Cyc_Absyn_Tvar*_tmpAD3;int _tmpAD4;int*_tmpAD5;struct _tuple29*
_tmpAD1=(struct _tuple29*)tvs2->hd;_tmpAD2=*_tmpAD1;_tmpAD3=_tmpAD2.f1;_tmpAD4=
_tmpAD2.f2;_tmpAD5=(int*)&(*_tmpAD1).f2;if(_tmpAD3->identity == - 1){const char*
_tmp142E;void*_tmp142D;(_tmp142D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp142E="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp142E,sizeof(char),46))),_tag_dyneither(_tmp142D,sizeof(void*),
0)));}if(_tmpAD3->identity == tv->identity){*_tmpAD5=*_tmpAD5  || b;return tvs;}}}{
struct _tuple29*_tmp1431;struct Cyc_List_List*_tmp1430;return(_tmp1430=
_region_malloc(r,sizeof(*_tmp1430)),((_tmp1430->hd=((_tmp1431=_region_malloc(r,
sizeof(*_tmp1431)),((_tmp1431->f1=tv,((_tmp1431->f2=b,_tmp1431)))))),((_tmp1430->tl=
tvs,_tmp1430)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1435;void*_tmp1434[1];struct Cyc_String_pa_struct _tmp1433;(_tmp1433.tag=0,((
_tmp1433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp1434[0]=& _tmp1433,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1435="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1435,sizeof(char),29))),_tag_dyneither(_tmp1434,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1436;return(_tmp1436=_cycalloc(sizeof(*
_tmp1436)),((_tmp1436->hd=tv,((_tmp1436->tl=tvs,_tmp1436)))));};}struct _tuple30{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpADE=Cyc_Tcutil_compress(
e);int _tmpAE0;_LL559: {struct Cyc_Absyn_Evar_struct*_tmpADF=(struct Cyc_Absyn_Evar_struct*)
_tmpADE;if(_tmpADF->tag != 1)goto _LL55B;else{_tmpAE0=_tmpADF->f3;}}_LL55A:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple30 _tmpAE2;void*
_tmpAE3;int _tmpAE4;int*_tmpAE5;struct _tuple30*_tmpAE1=(struct _tuple30*)es2->hd;
_tmpAE2=*_tmpAE1;_tmpAE3=_tmpAE2.f1;_tmpAE4=_tmpAE2.f2;_tmpAE5=(int*)&(*_tmpAE1).f2;{
void*_tmpAE6=Cyc_Tcutil_compress(_tmpAE3);int _tmpAE8;_LL55E: {struct Cyc_Absyn_Evar_struct*
_tmpAE7=(struct Cyc_Absyn_Evar_struct*)_tmpAE6;if(_tmpAE7->tag != 1)goto _LL560;
else{_tmpAE8=_tmpAE7->f3;}}_LL55F: if(_tmpAE0 == _tmpAE8){if(b != *_tmpAE5)*_tmpAE5=
1;return es;}goto _LL55D;_LL560:;_LL561: goto _LL55D;_LL55D:;};}}{struct _tuple30*
_tmp1439;struct Cyc_List_List*_tmp1438;return(_tmp1438=_region_malloc(r,sizeof(*
_tmp1438)),((_tmp1438->hd=((_tmp1439=_region_malloc(r,sizeof(*_tmp1439)),((
_tmp1439->f1=e,((_tmp1439->f2=b,_tmp1439)))))),((_tmp1438->tl=es,_tmp1438)))));};
_LL55B:;_LL55C: return es;_LL558:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp143A;r=((_tmp143A=_region_malloc(rgn,sizeof(*_tmp143A)),((
_tmp143A->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp143A->tl=r,_tmp143A))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAED;int _tmpAEE;struct
_tuple29 _tmpAEC=*((struct _tuple29*)tvs->hd);_tmpAED=_tmpAEC.f1;_tmpAEE=_tmpAEC.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpAED->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp143B;res=((_tmp143B=_region_malloc(r,sizeof(*_tmp143B)),((
_tmp143B->hd=(struct _tuple29*)tvs->hd,((_tmp143B->tl=res,_tmp143B))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp143F;void*_tmp143E[1];struct Cyc_String_pa_struct _tmp143D;(_tmp143D.tag=
0,((_tmp143D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp143E[0]=&
_tmp143D,Cyc_Tcutil_terr(loc,((_tmp143F="bitfield %s does not have constant width",
_tag_dyneither(_tmp143F,sizeof(char),41))),_tag_dyneither(_tmp143E,sizeof(void*),
1)))))));}else{unsigned int _tmpAF4;int _tmpAF5;struct _tuple13 _tmpAF3=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAF4=_tmpAF3.f1;_tmpAF5=_tmpAF3.f2;if(!_tmpAF5){
const char*_tmp1442;void*_tmp1441;(_tmp1441=0,Cyc_Tcutil_terr(loc,((_tmp1442="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp1442,sizeof(char),45))),_tag_dyneither(_tmp1441,sizeof(void*),
0)));}w=_tmpAF4;}{void*_tmpAF8=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAFA;_LL563: {struct Cyc_Absyn_IntType_struct*_tmpAF9=(struct Cyc_Absyn_IntType_struct*)
_tmpAF8;if(_tmpAF9->tag != 6)goto _LL565;else{_tmpAFA=_tmpAF9->f2;}}_LL564: switch(
_tmpAFA){case Cyc_Absyn_Char_sz: _LL567: if(w > 8){const char*_tmp1445;void*_tmp1444;(
_tmp1444=0,Cyc_Tcutil_terr(loc,((_tmp1445="bitfield larger than type",
_tag_dyneither(_tmp1445,sizeof(char),26))),_tag_dyneither(_tmp1444,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL568: if(w > 16){const char*_tmp1448;void*
_tmp1447;(_tmp1447=0,Cyc_Tcutil_terr(loc,((_tmp1448="bitfield larger than type",
_tag_dyneither(_tmp1448,sizeof(char),26))),_tag_dyneither(_tmp1447,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL569: goto _LL56A;case Cyc_Absyn_Int_sz: _LL56A:
if(w > 32){const char*_tmp144B;void*_tmp144A;(_tmp144A=0,Cyc_Tcutil_terr(loc,((
_tmp144B="bitfield larger than type",_tag_dyneither(_tmp144B,sizeof(char),26))),
_tag_dyneither(_tmp144A,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL56B: if(w > 64){const char*_tmp144E;void*_tmp144D;(_tmp144D=0,Cyc_Tcutil_terr(
loc,((_tmp144E="bitfield larger than type",_tag_dyneither(_tmp144E,sizeof(char),
26))),_tag_dyneither(_tmp144D,sizeof(void*),0)));}break;}goto _LL562;_LL565:;
_LL566:{const char*_tmp1453;void*_tmp1452[2];struct Cyc_String_pa_struct _tmp1451;
struct Cyc_String_pa_struct _tmp1450;(_tmp1450.tag=0,((_tmp1450.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1451.tag=0,((_tmp1451.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1452[0]=& _tmp1451,((_tmp1452[1]=& _tmp1450,Cyc_Tcutil_terr(loc,((_tmp1453="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1453,sizeof(char),52))),_tag_dyneither(_tmp1452,sizeof(void*),
2)))))))))))));}goto _LL562;_LL562:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpB07=(void*)atts->hd;_LL56E: {struct Cyc_Absyn_Packed_att_struct*
_tmpB08=(struct Cyc_Absyn_Packed_att_struct*)_tmpB07;if(_tmpB08->tag != 7)goto
_LL570;}_LL56F: continue;_LL570: {struct Cyc_Absyn_Aligned_att_struct*_tmpB09=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpB07;if(_tmpB09->tag != 6)goto _LL572;}
_LL571: continue;_LL572:;_LL573: {const char*_tmp1458;void*_tmp1457[2];struct Cyc_String_pa_struct
_tmp1456;struct Cyc_String_pa_struct _tmp1455;(_tmp1455.tag=0,((_tmp1455.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1456.tag=0,((_tmp1456.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1457[0]=& _tmp1456,((_tmp1457[1]=& _tmp1455,Cyc_Tcutil_terr(loc,((
_tmp1458="bad attribute %s on member %s",_tag_dyneither(_tmp1458,sizeof(char),30))),
_tag_dyneither(_tmp1457,sizeof(void*),2)))))))))))));}_LL56D:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpB0E=t;struct
Cyc_Absyn_Typedefdecl*_tmpB10;void**_tmpB11;_LL575: {struct Cyc_Absyn_TypedefType_struct*
_tmpB0F=(struct Cyc_Absyn_TypedefType_struct*)_tmpB0E;if(_tmpB0F->tag != 18)goto
_LL577;else{_tmpB10=_tmpB0F->f3;_tmpB11=_tmpB0F->f4;}}_LL576: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpB10))->tq).real_const  || (_tmpB10->tq).print_const){if(
declared_const){const char*_tmp145B;void*_tmp145A;(_tmp145A=0,Cyc_Tcutil_warn(loc,((
_tmp145B="extra const",_tag_dyneither(_tmp145B,sizeof(char),12))),_tag_dyneither(
_tmp145A,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpB11)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpB11);else{return declared_const;}_LL577:;_LL578: return
declared_const;_LL574:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpB14=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpB16;struct Cyc_Absyn_PtrAtts _tmpB17;void*_tmpB18;
_LL57A: {struct Cyc_Absyn_PointerType_struct*_tmpB15=(struct Cyc_Absyn_PointerType_struct*)
_tmpB14;if(_tmpB15->tag != 5)goto _LL57C;else{_tmpB16=_tmpB15->f1;_tmpB17=_tmpB16.ptr_atts;
_tmpB18=_tmpB17.rgn;}}_LL57B:{void*_tmpB19=Cyc_Tcutil_compress(_tmpB18);struct
Cyc_Absyn_Tvar*_tmpB1B;_LL57F: {struct Cyc_Absyn_VarType_struct*_tmpB1A=(struct
Cyc_Absyn_VarType_struct*)_tmpB19;if(_tmpB1A->tag != 2)goto _LL581;else{_tmpB1B=
_tmpB1A->f1;}}_LL580: return Cyc_Absyn_tvar_cmp(tvar,_tmpB1B)== 0;_LL581:;_LL582:
goto _LL57E;_LL57E:;}goto _LL579;_LL57C:;_LL57D: goto _LL579;_LL579:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpB1C=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB1E;
struct Cyc_Absyn_Kind _tmpB1F;enum Cyc_Absyn_KindQual _tmpB20;enum Cyc_Absyn_AliasQual
_tmpB21;struct Cyc_Absyn_Kind*_tmpB23;struct Cyc_Absyn_Kind _tmpB24;enum Cyc_Absyn_KindQual
_tmpB25;enum Cyc_Absyn_AliasQual _tmpB26;_LL584: {struct Cyc_Absyn_Less_kb_struct*
_tmpB1D=(struct Cyc_Absyn_Less_kb_struct*)_tmpB1C;if(_tmpB1D->tag != 2)goto _LL586;
else{_tmpB1E=_tmpB1D->f2;_tmpB1F=*_tmpB1E;_tmpB20=_tmpB1F.kind;if(_tmpB20 != Cyc_Absyn_RgnKind)
goto _LL586;_tmpB21=_tmpB1F.aliasqual;if(_tmpB21 != Cyc_Absyn_Top)goto _LL586;}}
_LL585: goto _LL587;_LL586: {struct Cyc_Absyn_Eq_kb_struct*_tmpB22=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB1C;if(_tmpB22->tag != 0)goto _LL588;else{_tmpB23=_tmpB22->f1;_tmpB24=*_tmpB23;
_tmpB25=_tmpB24.kind;if(_tmpB25 != Cyc_Absyn_RgnKind)goto _LL588;_tmpB26=_tmpB24.aliasqual;
if(_tmpB26 != Cyc_Absyn_Top)goto _LL588;}}_LL587: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL588:;_LL589: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL583:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpB27=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB2A;struct Cyc_Core_Opt**
_tmpB2B;struct Cyc_Core_Opt*_tmpB2C;struct Cyc_Core_Opt**_tmpB2D;struct Cyc_Absyn_Tvar*
_tmpB2F;struct Cyc_Absyn_TypeDecl*_tmpB31;struct Cyc_Absyn_TypeDecl _tmpB32;void*
_tmpB33;void**_tmpB34;void***_tmpB35;struct Cyc_List_List*_tmpB37;struct _tuple2*
_tmpB39;struct Cyc_Absyn_Enumdecl*_tmpB3A;struct Cyc_Absyn_Enumdecl**_tmpB3B;
struct Cyc_Absyn_DatatypeInfo _tmpB3D;union Cyc_Absyn_DatatypeInfoU _tmpB3E;union Cyc_Absyn_DatatypeInfoU*
_tmpB3F;struct Cyc_List_List*_tmpB40;struct Cyc_List_List**_tmpB41;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB43;union Cyc_Absyn_DatatypeFieldInfoU _tmpB44;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB45;struct Cyc_List_List*_tmpB46;struct Cyc_Absyn_PtrInfo _tmpB48;void*_tmpB49;
struct Cyc_Absyn_Tqual _tmpB4A;struct Cyc_Absyn_Tqual*_tmpB4B;struct Cyc_Absyn_PtrAtts
_tmpB4C;void*_tmpB4D;union Cyc_Absyn_Constraint*_tmpB4E;union Cyc_Absyn_Constraint*
_tmpB4F;union Cyc_Absyn_Constraint*_tmpB50;void*_tmpB52;struct Cyc_Absyn_Exp*
_tmpB54;struct Cyc_Absyn_ArrayInfo _tmpB59;void*_tmpB5A;struct Cyc_Absyn_Tqual
_tmpB5B;struct Cyc_Absyn_Tqual*_tmpB5C;struct Cyc_Absyn_Exp*_tmpB5D;struct Cyc_Absyn_Exp**
_tmpB5E;union Cyc_Absyn_Constraint*_tmpB5F;struct Cyc_Position_Segment*_tmpB60;
struct Cyc_Absyn_FnInfo _tmpB62;struct Cyc_List_List*_tmpB63;struct Cyc_List_List**
_tmpB64;struct Cyc_Core_Opt*_tmpB65;struct Cyc_Core_Opt**_tmpB66;struct Cyc_Absyn_Tqual
_tmpB67;struct Cyc_Absyn_Tqual*_tmpB68;void*_tmpB69;struct Cyc_List_List*_tmpB6A;
int _tmpB6B;struct Cyc_Absyn_VarargInfo*_tmpB6C;struct Cyc_List_List*_tmpB6D;struct
Cyc_List_List*_tmpB6E;struct Cyc_List_List*_tmpB70;enum Cyc_Absyn_AggrKind _tmpB72;
struct Cyc_List_List*_tmpB73;struct Cyc_Absyn_AggrInfo _tmpB75;union Cyc_Absyn_AggrInfoU
_tmpB76;union Cyc_Absyn_AggrInfoU*_tmpB77;struct Cyc_List_List*_tmpB78;struct Cyc_List_List**
_tmpB79;struct _tuple2*_tmpB7B;struct Cyc_List_List*_tmpB7C;struct Cyc_List_List**
_tmpB7D;struct Cyc_Absyn_Typedefdecl*_tmpB7E;struct Cyc_Absyn_Typedefdecl**_tmpB7F;
void**_tmpB80;void***_tmpB81;void*_tmpB86;void*_tmpB88;void*_tmpB89;void*_tmpB8B;
void*_tmpB8D;struct Cyc_List_List*_tmpB8F;_LL58B: {struct Cyc_Absyn_VoidType_struct*
_tmpB28=(struct Cyc_Absyn_VoidType_struct*)_tmpB27;if(_tmpB28->tag != 0)goto _LL58D;}
_LL58C: goto _LL58A;_LL58D: {struct Cyc_Absyn_Evar_struct*_tmpB29=(struct Cyc_Absyn_Evar_struct*)
_tmpB27;if(_tmpB29->tag != 1)goto _LL58F;else{_tmpB2A=_tmpB29->f1;_tmpB2B=(struct
Cyc_Core_Opt**)& _tmpB29->f1;_tmpB2C=_tmpB29->f2;_tmpB2D=(struct Cyc_Core_Opt**)&
_tmpB29->f2;}}_LL58E: if(*_tmpB2B == 0  || Cyc_Tcutil_kind_leq(expected_kind,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB2B))->v) && !Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB2B))->v,
expected_kind))*_tmpB2B=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result
 && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){if(
expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB2D=(struct Cyc_Core_Opt*)& urgn;
else{*_tmpB2D=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmp145E;struct Cyc_Absyn_Less_kb_struct*_tmp145D;
struct Cyc_Absyn_Tvar*_tmpB90=Cyc_Tcutil_new_tvar((void*)((_tmp145D=_cycalloc(
sizeof(*_tmp145D)),((_tmp145D[0]=((_tmp145E.tag=2,((_tmp145E.f1=0,((_tmp145E.f2=
expected_kind,_tmp145E)))))),_tmp145D)))));{struct Cyc_Absyn_VarType_struct*
_tmp1464;struct Cyc_Absyn_VarType_struct _tmp1463;struct Cyc_Core_Opt*_tmp1462;*
_tmpB2D=((_tmp1462=_cycalloc(sizeof(*_tmp1462)),((_tmp1462->v=(void*)((_tmp1464=
_cycalloc(sizeof(*_tmp1464)),((_tmp1464[0]=((_tmp1463.tag=2,((_tmp1463.f1=
_tmpB90,_tmp1463)))),_tmp1464)))),_tmp1462))));}_tmpB2F=_tmpB90;goto _LL590;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL58A;_LL58F: {struct Cyc_Absyn_VarType_struct*_tmpB2E=(
struct Cyc_Absyn_VarType_struct*)_tmpB27;if(_tmpB2E->tag != 2)goto _LL591;else{
_tmpB2F=_tmpB2E->f1;}}_LL590:{void*_tmpB96=Cyc_Absyn_compress_kb(_tmpB2F->kind);
struct Cyc_Core_Opt*_tmpB98;struct Cyc_Core_Opt**_tmpB99;_LL5C4: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB97=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB96;if(_tmpB97->tag != 1)goto
_LL5C6;else{_tmpB98=_tmpB97->f1;_tmpB99=(struct Cyc_Core_Opt**)& _tmpB97->f1;}}
_LL5C5:{struct Cyc_Absyn_Less_kb_struct*_tmp146A;struct Cyc_Absyn_Less_kb_struct
_tmp1469;struct Cyc_Core_Opt*_tmp1468;*_tmpB99=((_tmp1468=_cycalloc(sizeof(*
_tmp1468)),((_tmp1468->v=(void*)((_tmp146A=_cycalloc(sizeof(*_tmp146A)),((
_tmp146A[0]=((_tmp1469.tag=2,((_tmp1469.f1=0,((_tmp1469.f2=expected_kind,
_tmp1469)))))),_tmp146A)))),_tmp1468))));}goto _LL5C3;_LL5C6:;_LL5C7: goto _LL5C3;
_LL5C3:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB2F);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB2F,put_in_effect);{void*_tmpB9D=Cyc_Absyn_compress_kb(_tmpB2F->kind);struct
Cyc_Core_Opt*_tmpB9F;struct Cyc_Core_Opt**_tmpBA0;struct Cyc_Absyn_Kind*_tmpBA1;
_LL5C9: {struct Cyc_Absyn_Less_kb_struct*_tmpB9E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB9D;if(_tmpB9E->tag != 2)goto _LL5CB;else{_tmpB9F=_tmpB9E->f1;_tmpBA0=(struct
Cyc_Core_Opt**)& _tmpB9E->f1;_tmpBA1=_tmpB9E->f2;}}_LL5CA: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpBA1)){struct Cyc_Absyn_Less_kb_struct*_tmp1470;struct Cyc_Absyn_Less_kb_struct
_tmp146F;struct Cyc_Core_Opt*_tmp146E;*_tmpBA0=((_tmp146E=_cycalloc(sizeof(*
_tmp146E)),((_tmp146E->v=(void*)((_tmp1470=_cycalloc(sizeof(*_tmp1470)),((
_tmp1470[0]=((_tmp146F.tag=2,((_tmp146F.f1=0,((_tmp146F.f2=expected_kind,
_tmp146F)))))),_tmp1470)))),_tmp146E))));}goto _LL5C8;_LL5CB:;_LL5CC: goto _LL5C8;
_LL5C8:;}goto _LL58A;_LL591: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB30=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB27;if(_tmpB30->tag != 27)goto _LL593;else{
_tmpB31=_tmpB30->f1;_tmpB32=*_tmpB31;_tmpB33=_tmpB32.r;_tmpB34=_tmpB30->f2;
_tmpB35=(void***)& _tmpB30->f2;}}_LL592: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpBA5=_tmpB33;struct Cyc_Absyn_Aggrdecl*_tmpBA7;struct Cyc_Absyn_Enumdecl*
_tmpBA9;struct Cyc_Absyn_Datatypedecl*_tmpBAB;_LL5CE: {struct Cyc_Absyn_Aggr_td_struct*
_tmpBA6=(struct Cyc_Absyn_Aggr_td_struct*)_tmpBA5;if(_tmpBA6->tag != 0)goto _LL5D0;
else{_tmpBA7=_tmpBA6->f1;}}_LL5CF: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBA7);goto _LL5CD;_LL5D0: {struct Cyc_Absyn_Enum_td_struct*_tmpBA8=(struct Cyc_Absyn_Enum_td_struct*)
_tmpBA5;if(_tmpBA8->tag != 1)goto _LL5D2;else{_tmpBA9=_tmpBA8->f1;}}_LL5D1: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpBA9);goto _LL5CD;_LL5D2: {struct Cyc_Absyn_Datatype_td_struct*
_tmpBAA=(struct Cyc_Absyn_Datatype_td_struct*)_tmpBA5;if(_tmpBAA->tag != 2)goto
_LL5CD;else{_tmpBAB=_tmpBAA->f1;}}_LL5D3: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBAB);goto _LL5CD;_LL5CD:;}{void**_tmp1471;*_tmpB35=((_tmp1471=_cycalloc(
sizeof(*_tmp1471)),((_tmp1471[0]=new_t,_tmp1471))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL593: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB36=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB27;if(_tmpB36->tag != 15)goto
_LL595;else{_tmpB37=_tmpB36->f1;}}_LL594: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpBAD=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpB37 != 0;_tmpB37=_tmpB37->tl){
struct Cyc_Absyn_Enumfield*_tmpBAE=(struct Cyc_Absyn_Enumfield*)_tmpB37->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpBAE->name).f2)){
const char*_tmp1475;void*_tmp1474[1];struct Cyc_String_pa_struct _tmp1473;(_tmp1473.tag=
0,((_tmp1473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBAE->name).f2),((
_tmp1474[0]=& _tmp1473,Cyc_Tcutil_terr(_tmpBAE->loc,((_tmp1475="duplicate enum field name %s",
_tag_dyneither(_tmp1475,sizeof(char),29))),_tag_dyneither(_tmp1474,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1476;prev_fields=((_tmp1476=
_region_malloc(_tmpBAD,sizeof(*_tmp1476)),((_tmp1476->hd=(*_tmpBAE->name).f2,((
_tmp1476->tl=prev_fields,_tmp1476))))));}if(_tmpBAE->tag == 0)_tmpBAE->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpBAE->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpBAE->tag))){const char*_tmp147A;void*
_tmp1479[1];struct Cyc_String_pa_struct _tmp1478;(_tmp1478.tag=0,((_tmp1478.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBAE->name).f2),((_tmp1479[0]=&
_tmp1478,Cyc_Tcutil_terr(loc,((_tmp147A="enum field %s: expression is not constant",
_tag_dyneither(_tmp147A,sizeof(char),42))),_tag_dyneither(_tmp1479,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpBAE->tag))).f1;tag_count=t1 + 1;(*_tmpBAE->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp1480;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp147F;struct _tuple30*_tmp147E;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple30*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpBAE->name).f2,(struct _tuple30*)((_tmp147E=_cycalloc(sizeof(*_tmp147E)),((
_tmp147E->f1=(void*)((_tmp1480=_cycalloc(sizeof(*_tmp1480)),((_tmp1480[0]=((
_tmp147F.tag=4,((_tmp147F.f1=(void*)t,((_tmp147F.f2=_tmpBAE,_tmp147F)))))),
_tmp1480)))),((_tmp147E->f2=1,_tmp147E)))))));};};}}goto _LL58A;}_LL595: {struct
Cyc_Absyn_EnumType_struct*_tmpB38=(struct Cyc_Absyn_EnumType_struct*)_tmpB27;if(
_tmpB38->tag != 14)goto _LL597;else{_tmpB39=_tmpB38->f1;_tmpB3A=_tmpB38->f2;
_tmpB3B=(struct Cyc_Absyn_Enumdecl**)& _tmpB38->f2;}}_LL596: if(*_tmpB3B == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB3B))->fields == 0){struct
_handler_cons _tmpBB9;_push_handler(& _tmpBB9);{int _tmpBBB=0;if(setjmp(_tmpBB9.handler))
_tmpBBB=1;if(!_tmpBBB){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB39);*_tmpB3B=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpBBA=(void*)_exn_thrown;void*_tmpBBD=_tmpBBA;_LL5D5: {struct Cyc_Dict_Absent_struct*
_tmpBBE=(struct Cyc_Dict_Absent_struct*)_tmpBBD;if(_tmpBBE->tag != Cyc_Dict_Absent)
goto _LL5D7;}_LL5D6: {struct Cyc_Tcenv_Genv*_tmpBBF=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1481;struct Cyc_Absyn_Enumdecl*_tmpBC0=(_tmp1481=
_cycalloc(sizeof(*_tmp1481)),((_tmp1481->sc=Cyc_Absyn_Extern,((_tmp1481->name=
_tmpB39,((_tmp1481->fields=0,_tmp1481)))))));Cyc_Tc_tcEnumdecl(te,_tmpBBF,loc,
_tmpBC0);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB39);*
_tmpB3B=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5D4;};}_LL5D7:;_LL5D8:(void)_throw(
_tmpBBD);_LL5D4:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB3B);*_tmpB39=(ed->name)[0];goto _LL58A;};_LL597: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB3C=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB27;if(_tmpB3C->tag != 3)goto
_LL599;else{_tmpB3D=_tmpB3C->f1;_tmpB3E=_tmpB3D.datatype_info;_tmpB3F=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB3C->f1).datatype_info;_tmpB40=_tmpB3D.targs;_tmpB41=(struct Cyc_List_List**)&(
_tmpB3C->f1).targs;}}_LL598: {struct Cyc_List_List*_tmpBC2=*_tmpB41;{union Cyc_Absyn_DatatypeInfoU
_tmpBC3=*_tmpB3F;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBC4;struct _tuple2*
_tmpBC5;int _tmpBC6;struct Cyc_Absyn_Datatypedecl**_tmpBC7;struct Cyc_Absyn_Datatypedecl*
_tmpBC8;_LL5DA: if((_tmpBC3.UnknownDatatype).tag != 1)goto _LL5DC;_tmpBC4=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBC3.UnknownDatatype).val;_tmpBC5=_tmpBC4.name;
_tmpBC6=_tmpBC4.is_extensible;_LL5DB: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBC9;_push_handler(& _tmpBC9);{int _tmpBCB=0;if(setjmp(
_tmpBC9.handler))_tmpBCB=1;if(!_tmpBCB){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBC5);;_pop_handler();}else{void*_tmpBCA=(void*)_exn_thrown;void*_tmpBCD=
_tmpBCA;_LL5DF: {struct Cyc_Dict_Absent_struct*_tmpBCE=(struct Cyc_Dict_Absent_struct*)
_tmpBCD;if(_tmpBCE->tag != Cyc_Dict_Absent)goto _LL5E1;}_LL5E0: {struct Cyc_Tcenv_Genv*
_tmpBCF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1482;struct Cyc_Absyn_Datatypedecl*
_tmpBD0=(_tmp1482=_cycalloc(sizeof(*_tmp1482)),((_tmp1482->sc=Cyc_Absyn_Extern,((
_tmp1482->name=_tmpBC5,((_tmp1482->tvs=0,((_tmp1482->fields=0,((_tmp1482->is_extensible=
_tmpBC6,_tmp1482)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBCF,loc,_tmpBD0);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBC5);if(_tmpBC2 != 0){{const char*
_tmp1486;void*_tmp1485[1];struct Cyc_String_pa_struct _tmp1484;(_tmp1484.tag=0,((
_tmp1484.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC5)),((_tmp1485[0]=& _tmp1484,Cyc_Tcutil_terr(loc,((_tmp1486="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1486,sizeof(char),47))),_tag_dyneither(_tmp1485,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5DE;}_LL5E1:;_LL5E2:(void)_throw(_tmpBCD);_LL5DE:;}};}
if(_tmpBC6  && !(*tudp)->is_extensible){const char*_tmp148A;void*_tmp1489[1];
struct Cyc_String_pa_struct _tmp1488;(_tmp1488.tag=0,((_tmp1488.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC5)),((
_tmp1489[0]=& _tmp1488,Cyc_Tcutil_terr(loc,((_tmp148A="datatype %s was not declared @extensible",
_tag_dyneither(_tmp148A,sizeof(char),41))),_tag_dyneither(_tmp1489,sizeof(void*),
1)))))));}*_tmpB3F=Cyc_Absyn_KnownDatatype(tudp);_tmpBC8=*tudp;goto _LL5DD;}
_LL5DC: if((_tmpBC3.KnownDatatype).tag != 2)goto _LL5D9;_tmpBC7=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBC3.KnownDatatype).val;_tmpBC8=*_tmpBC7;_LL5DD: {struct Cyc_List_List*tvs=
_tmpBC8->tvs;for(0;_tmpBC2 != 0  && tvs != 0;(_tmpBC2=_tmpBC2->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBC2->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp148B;struct _tuple0 _tmpBD9=(_tmp148B.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp148B.f2=t,_tmp148B)));void*_tmpBDA;void*_tmpBDC;struct Cyc_Absyn_Tvar*_tmpBDE;
_LL5E4: _tmpBDA=_tmpBD9.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBDB=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBDA;if(_tmpBDB->tag != 1)goto _LL5E6;};_tmpBDC=_tmpBD9.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBDD=(struct Cyc_Absyn_VarType_struct*)_tmpBDC;if(_tmpBDD->tag != 2)goto _LL5E6;
else{_tmpBDE=_tmpBDD->f1;}};_LL5E5: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBDE);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBDE,1);continue;_LL5E6:;_LL5E7: goto _LL5E3;_LL5E3:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBC2 != 0){const
char*_tmp148F;void*_tmp148E[1];struct Cyc_String_pa_struct _tmp148D;(_tmp148D.tag=
0,((_tmp148D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC8->name)),((_tmp148E[0]=& _tmp148D,Cyc_Tcutil_terr(loc,((_tmp148F="too many type arguments for datatype %s",
_tag_dyneither(_tmp148F,sizeof(char),40))),_tag_dyneither(_tmp148E,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1490;hidden_ts=((_tmp1490=_cycalloc(sizeof(*_tmp1490)),((_tmp1490->hd=e,((
_tmp1490->tl=hidden_ts,_tmp1490))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB41=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB41,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5D9;}_LL5D9:;}goto
_LL58A;}_LL599: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB42=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB27;if(_tmpB42->tag != 4)goto _LL59B;else{_tmpB43=_tmpB42->f1;_tmpB44=_tmpB43.field_info;
_tmpB45=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB42->f1).field_info;_tmpB46=
_tmpB43.targs;}}_LL59A:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBE3=*_tmpB45;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBE4;struct _tuple2*_tmpBE5;struct _tuple2*
_tmpBE6;int _tmpBE7;struct _tuple3 _tmpBE8;struct Cyc_Absyn_Datatypedecl*_tmpBE9;
struct Cyc_Absyn_Datatypefield*_tmpBEA;_LL5E9: if((_tmpBE3.UnknownDatatypefield).tag
!= 1)goto _LL5EB;_tmpBE4=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBE3.UnknownDatatypefield).val;
_tmpBE5=_tmpBE4.datatype_name;_tmpBE6=_tmpBE4.field_name;_tmpBE7=_tmpBE4.is_extensible;
_LL5EA: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBEB;_push_handler(& _tmpBEB);{int _tmpBED=0;if(setjmp(
_tmpBEB.handler))_tmpBED=1;if(!_tmpBED){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBE5);;_pop_handler();}else{void*_tmpBEC=(void*)_exn_thrown;void*_tmpBEF=
_tmpBEC;_LL5EE: {struct Cyc_Dict_Absent_struct*_tmpBF0=(struct Cyc_Dict_Absent_struct*)
_tmpBEF;if(_tmpBF0->tag != Cyc_Dict_Absent)goto _LL5F0;}_LL5EF:{const char*_tmp1494;
void*_tmp1493[1];struct Cyc_String_pa_struct _tmp1492;(_tmp1492.tag=0,((_tmp1492.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE5)),((
_tmp1493[0]=& _tmp1492,Cyc_Tcutil_terr(loc,((_tmp1494="unbound datatype %s",
_tag_dyneither(_tmp1494,sizeof(char),20))),_tag_dyneither(_tmp1493,sizeof(void*),
1)))))));}return cvtenv;_LL5F0:;_LL5F1:(void)_throw(_tmpBEF);_LL5ED:;}};}{struct
_handler_cons _tmpBF4;_push_handler(& _tmpBF4);{int _tmpBF6=0;if(setjmp(_tmpBF4.handler))
_tmpBF6=1;if(!_tmpBF6){{struct _RegionHandle*_tmpBF7=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBF8=Cyc_Tcenv_lookup_ordinary(_tmpBF7,te,loc,_tmpBE6);struct Cyc_Absyn_Datatypedecl*
_tmpBFA;struct Cyc_Absyn_Datatypefield*_tmpBFB;_LL5F3: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBF9=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBF8;if(_tmpBF9->tag != 2)goto
_LL5F5;else{_tmpBFA=_tmpBF9->f1;_tmpBFB=_tmpBF9->f2;}}_LL5F4: tuf=_tmpBFB;tud=
_tmpBFA;if(_tmpBE7  && !tud->is_extensible){const char*_tmp1498;void*_tmp1497[1];
struct Cyc_String_pa_struct _tmp1496;(_tmp1496.tag=0,((_tmp1496.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE5)),((
_tmp1497[0]=& _tmp1496,Cyc_Tcutil_terr(loc,((_tmp1498="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1498,sizeof(char),41))),_tag_dyneither(_tmp1497,sizeof(void*),
1)))))));}goto _LL5F2;_LL5F5:;_LL5F6:{const char*_tmp149D;void*_tmp149C[2];struct
Cyc_String_pa_struct _tmp149B;struct Cyc_String_pa_struct _tmp149A;(_tmp149A.tag=0,((
_tmp149A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE5)),((_tmp149B.tag=0,((_tmp149B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE6)),((_tmp149C[0]=& _tmp149B,((
_tmp149C[1]=& _tmp149A,Cyc_Tcutil_terr(loc,((_tmp149D="unbound field %s in type datatype %s",
_tag_dyneither(_tmp149D,sizeof(char),37))),_tag_dyneither(_tmp149C,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpC03=cvtenv;_npop_handler(0);return
_tmpC03;};_LL5F2:;};_pop_handler();}else{void*_tmpBF5=(void*)_exn_thrown;void*
_tmpC05=_tmpBF5;_LL5F8: {struct Cyc_Dict_Absent_struct*_tmpC06=(struct Cyc_Dict_Absent_struct*)
_tmpC05;if(_tmpC06->tag != Cyc_Dict_Absent)goto _LL5FA;}_LL5F9:{const char*_tmp14A2;
void*_tmp14A1[2];struct Cyc_String_pa_struct _tmp14A0;struct Cyc_String_pa_struct
_tmp149F;(_tmp149F.tag=0,((_tmp149F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE5)),((_tmp14A0.tag=0,((_tmp14A0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE6)),((
_tmp14A1[0]=& _tmp14A0,((_tmp14A1[1]=& _tmp149F,Cyc_Tcutil_terr(loc,((_tmp14A2="unbound field %s in type datatype %s",
_tag_dyneither(_tmp14A2,sizeof(char),37))),_tag_dyneither(_tmp14A1,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5FA:;_LL5FB:(void)_throw(_tmpC05);_LL5F7:;}};}*
_tmpB45=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBE9=tud;_tmpBEA=tuf;goto _LL5EC;}
_LL5EB: if((_tmpBE3.KnownDatatypefield).tag != 2)goto _LL5E8;_tmpBE8=(struct _tuple3)(
_tmpBE3.KnownDatatypefield).val;_tmpBE9=_tmpBE8.f1;_tmpBEA=_tmpBE8.f2;_LL5EC: {
struct Cyc_List_List*tvs=_tmpBE9->tvs;for(0;_tmpB46 != 0  && tvs != 0;(_tmpB46=
_tmpB46->tl,tvs=tvs->tl)){void*t=(void*)_tmpB46->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp14A3;struct _tuple0 _tmpC0C=(
_tmp14A3.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp14A3.f2=t,_tmp14A3)));void*
_tmpC0D;void*_tmpC0F;struct Cyc_Absyn_Tvar*_tmpC11;_LL5FD: _tmpC0D=_tmpC0C.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpC0E=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC0D;if(_tmpC0E->tag != 1)goto _LL5FF;};_tmpC0F=_tmpC0C.f2;{struct Cyc_Absyn_VarType_struct*
_tmpC10=(struct Cyc_Absyn_VarType_struct*)_tmpC0F;if(_tmpC10->tag != 2)goto _LL5FF;
else{_tmpC11=_tmpC10->f1;}};_LL5FE: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpC11);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpC11,1);continue;_LL5FF:;_LL600: goto _LL5FC;_LL5FC:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB46 != 0){const
char*_tmp14A8;void*_tmp14A7[2];struct Cyc_String_pa_struct _tmp14A6;struct Cyc_String_pa_struct
_tmp14A5;(_tmp14A5.tag=0,((_tmp14A5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBEA->name)),((_tmp14A6.tag=0,((
_tmp14A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE9->name)),((_tmp14A7[0]=& _tmp14A6,((_tmp14A7[1]=& _tmp14A5,Cyc_Tcutil_terr(
loc,((_tmp14A8="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp14A8,sizeof(char),43))),_tag_dyneither(_tmp14A7,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp14AD;void*_tmp14AC[2];struct Cyc_String_pa_struct
_tmp14AB;struct Cyc_String_pa_struct _tmp14AA;(_tmp14AA.tag=0,((_tmp14AA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBEA->name)),((
_tmp14AB.tag=0,((_tmp14AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE9->name)),((_tmp14AC[0]=& _tmp14AB,((_tmp14AC[1]=& _tmp14AA,Cyc_Tcutil_terr(
loc,((_tmp14AD="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp14AD,sizeof(char),42))),_tag_dyneither(_tmp14AC,sizeof(void*),2)))))))))))));}
goto _LL5E8;}_LL5E8:;}goto _LL58A;_LL59B: {struct Cyc_Absyn_PointerType_struct*
_tmpB47=(struct Cyc_Absyn_PointerType_struct*)_tmpB27;if(_tmpB47->tag != 5)goto
_LL59D;else{_tmpB48=_tmpB47->f1;_tmpB49=_tmpB48.elt_typ;_tmpB4A=_tmpB48.elt_tq;
_tmpB4B=(struct Cyc_Absyn_Tqual*)&(_tmpB47->f1).elt_tq;_tmpB4C=_tmpB48.ptr_atts;
_tmpB4D=_tmpB4C.rgn;_tmpB4E=_tmpB4C.nullable;_tmpB4F=_tmpB4C.bounds;_tmpB50=
_tmpB4C.zero_term;}}_LL59C: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB49,1);_tmpB4B->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB4B->print_const,_tmpB49);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL601: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL602: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL603: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB4D,1);{union Cyc_Absyn_Constraint*
_tmpC1A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB50);union Cyc_Absyn_Constraint _tmpC1B;int _tmpC1C;union Cyc_Absyn_Constraint
_tmpC1D;int _tmpC1E;_LL606: _tmpC1B=*_tmpC1A;if((_tmpC1B.No_constr).tag != 3)goto
_LL608;_tmpC1C=(int)(_tmpC1B.No_constr).val;_LL607:{void*_tmpC1F=Cyc_Tcutil_compress(
_tmpB49);enum Cyc_Absyn_Size_of _tmpC21;_LL60D: {struct Cyc_Absyn_IntType_struct*
_tmpC20=(struct Cyc_Absyn_IntType_struct*)_tmpC1F;if(_tmpC20->tag != 6)goto _LL60F;
else{_tmpC21=_tmpC20->f2;if(_tmpC21 != Cyc_Absyn_Char_sz)goto _LL60F;}}_LL60E:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB50,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL60C;_LL60F:;_LL610:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB50,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL60C;
_LL60C:;}goto _LL605;_LL608: _tmpC1D=*_tmpC1A;if((_tmpC1D.Eq_constr).tag != 1)goto
_LL60A;_tmpC1E=(int)(_tmpC1D.Eq_constr).val;if(_tmpC1E != 1)goto _LL60A;_LL609: if(
!Cyc_Tcutil_admits_zero(_tmpB49)){const char*_tmp14B1;void*_tmp14B0[1];struct Cyc_String_pa_struct
_tmp14AF;(_tmp14AF.tag=0,((_tmp14AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB49)),((_tmp14B0[0]=& _tmp14AF,Cyc_Tcutil_terr(
loc,((_tmp14B1="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp14B1,sizeof(char),53))),_tag_dyneither(_tmp14B0,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL605;_LL60A:;_LL60B: is_zero_terminated=0;
goto _LL605;_LL605:;}{void*_tmpC25=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB4F);struct Cyc_Absyn_Exp*
_tmpC28;_LL612: {struct Cyc_Absyn_DynEither_b_struct*_tmpC26=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC25;if(_tmpC26->tag != 0)goto _LL614;}_LL613: goto _LL611;_LL614: {struct Cyc_Absyn_Upper_b_struct*
_tmpC27=(struct Cyc_Absyn_Upper_b_struct*)_tmpC25;if(_tmpC27->tag != 1)goto _LL611;
else{_tmpC28=_tmpC27->f1;}}_LL615: {struct _RegionHandle*_tmpC29=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC2A=Cyc_Tcenv_allow_valueof(_tmpC29,te);Cyc_Tcexp_tcExp(
_tmpC2A,0,_tmpC28);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC28,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC28)){const char*_tmp14B4;void*
_tmp14B3;(_tmp14B3=0,Cyc_Tcutil_terr(loc,((_tmp14B4="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp14B4,sizeof(char),49))),_tag_dyneither(_tmp14B3,sizeof(void*),
0)));}{unsigned int _tmpC2E;int _tmpC2F;struct _tuple13 _tmpC2D=Cyc_Evexp_eval_const_uint_exp(
_tmpC28);_tmpC2E=_tmpC2D.f1;_tmpC2F=_tmpC2D.f2;if(is_zero_terminated  && (!
_tmpC2F  || _tmpC2E < 1)){const char*_tmp14B7;void*_tmp14B6;(_tmp14B6=0,Cyc_Tcutil_terr(
loc,((_tmp14B7="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp14B7,sizeof(char),55))),_tag_dyneither(_tmp14B6,sizeof(void*),
0)));}goto _LL611;};}_LL611:;}goto _LL58A;};}_LL59D: {struct Cyc_Absyn_TagType_struct*
_tmpB51=(struct Cyc_Absyn_TagType_struct*)_tmpB27;if(_tmpB51->tag != 20)goto _LL59F;
else{_tmpB52=(void*)_tmpB51->f1;}}_LL59E: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB52,1);goto _LL58A;_LL59F: {struct Cyc_Absyn_ValueofType_struct*
_tmpB53=(struct Cyc_Absyn_ValueofType_struct*)_tmpB27;if(_tmpB53->tag != 19)goto
_LL5A1;else{_tmpB54=_tmpB53->f1;}}_LL5A0: {struct _RegionHandle*_tmpC32=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC33=Cyc_Tcenv_allow_valueof(_tmpC32,te);Cyc_Tcexp_tcExp(
_tmpC33,0,_tmpB54);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB54)){const char*
_tmp14BA;void*_tmp14B9;(_tmp14B9=0,Cyc_Tcutil_terr(loc,((_tmp14BA="valueof_t requires an int expression",
_tag_dyneither(_tmp14BA,sizeof(char),37))),_tag_dyneither(_tmp14B9,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB54,te,cvtenv);goto
_LL58A;}_LL5A1: {struct Cyc_Absyn_IntType_struct*_tmpB55=(struct Cyc_Absyn_IntType_struct*)
_tmpB27;if(_tmpB55->tag != 6)goto _LL5A3;}_LL5A2: goto _LL5A4;_LL5A3: {struct Cyc_Absyn_FloatType_struct*
_tmpB56=(struct Cyc_Absyn_FloatType_struct*)_tmpB27;if(_tmpB56->tag != 7)goto
_LL5A5;}_LL5A4: goto _LL5A6;_LL5A5: {struct Cyc_Absyn_DoubleType_struct*_tmpB57=(
struct Cyc_Absyn_DoubleType_struct*)_tmpB27;if(_tmpB57->tag != 8)goto _LL5A7;}
_LL5A6: goto _LL58A;_LL5A7: {struct Cyc_Absyn_ArrayType_struct*_tmpB58=(struct Cyc_Absyn_ArrayType_struct*)
_tmpB27;if(_tmpB58->tag != 9)goto _LL5A9;else{_tmpB59=_tmpB58->f1;_tmpB5A=_tmpB59.elt_type;
_tmpB5B=_tmpB59.tq;_tmpB5C=(struct Cyc_Absyn_Tqual*)&(_tmpB58->f1).tq;_tmpB5D=
_tmpB59.num_elts;_tmpB5E=(struct Cyc_Absyn_Exp**)&(_tmpB58->f1).num_elts;_tmpB5F=
_tmpB59.zero_term;_tmpB60=_tmpB59.zt_loc;}}_LL5A8: {struct Cyc_Absyn_Exp*_tmpC36=*
_tmpB5E;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpB5A,1);_tmpB5C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB5C->print_const,
_tmpB5A);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC37=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB5F);union Cyc_Absyn_Constraint
_tmpC38;int _tmpC39;union Cyc_Absyn_Constraint _tmpC3A;int _tmpC3B;_LL617: _tmpC38=*
_tmpC37;if((_tmpC38.No_constr).tag != 3)goto _LL619;_tmpC39=(int)(_tmpC38.No_constr).val;
_LL618:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB5F,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL616;_LL619: _tmpC3A=*_tmpC37;if((_tmpC3A.Eq_constr).tag
!= 1)goto _LL61B;_tmpC3B=(int)(_tmpC3A.Eq_constr).val;if(_tmpC3B != 1)goto _LL61B;
_LL61A: if(!Cyc_Tcutil_admits_zero(_tmpB5A)){const char*_tmp14BE;void*_tmp14BD[1];
struct Cyc_String_pa_struct _tmp14BC;(_tmp14BC.tag=0,((_tmp14BC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB5A)),((
_tmp14BD[0]=& _tmp14BC,Cyc_Tcutil_terr(loc,((_tmp14BE="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp14BE,sizeof(char),51))),_tag_dyneither(_tmp14BD,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL616;_LL61B:;_LL61C: is_zero_terminated=0;
goto _LL616;_LL616:;}if(_tmpC36 == 0){if(is_zero_terminated)*_tmpB5E=(_tmpC36=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14C1;void*
_tmp14C0;(_tmp14C0=0,Cyc_Tcutil_warn(loc,((_tmp14C1="array bound defaults to 1 here",
_tag_dyneither(_tmp14C1,sizeof(char),31))),_tag_dyneither(_tmp14C0,sizeof(void*),
0)));}*_tmpB5E=(_tmpC36=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC36);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC36)){const char*_tmp14C4;void*_tmp14C3;(_tmp14C3=0,Cyc_Tcutil_terr(loc,((
_tmp14C4="array bounds expression is not constant",_tag_dyneither(_tmp14C4,
sizeof(char),40))),_tag_dyneither(_tmp14C3,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC36)){const char*_tmp14C7;void*_tmp14C6;(_tmp14C6=0,
Cyc_Tcutil_terr(loc,((_tmp14C7="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14C7,sizeof(char),47))),_tag_dyneither(_tmp14C6,sizeof(void*),
0)));}{unsigned int _tmpC46;int _tmpC47;struct _tuple13 _tmpC45=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC36);_tmpC46=_tmpC45.f1;_tmpC47=_tmpC45.f2;if((
is_zero_terminated  && _tmpC47) && _tmpC46 < 1){const char*_tmp14CA;void*_tmp14C9;(
_tmp14C9=0,Cyc_Tcutil_warn(loc,((_tmp14CA="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14CA,sizeof(char),48))),_tag_dyneither(_tmp14C9,sizeof(void*),
0)));}if((_tmpC47  && _tmpC46 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14CD;void*
_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_warn(loc,((_tmp14CD="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14CD,sizeof(char),75))),_tag_dyneither(_tmp14CC,sizeof(void*),
0)));}*_tmpB5E=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL58A;};};}
_LL5A9: {struct Cyc_Absyn_FnType_struct*_tmpB61=(struct Cyc_Absyn_FnType_struct*)
_tmpB27;if(_tmpB61->tag != 10)goto _LL5AB;else{_tmpB62=_tmpB61->f1;_tmpB63=_tmpB62.tvars;
_tmpB64=(struct Cyc_List_List**)&(_tmpB61->f1).tvars;_tmpB65=_tmpB62.effect;
_tmpB66=(struct Cyc_Core_Opt**)&(_tmpB61->f1).effect;_tmpB67=_tmpB62.ret_tqual;
_tmpB68=(struct Cyc_Absyn_Tqual*)&(_tmpB61->f1).ret_tqual;_tmpB69=_tmpB62.ret_typ;
_tmpB6A=_tmpB62.args;_tmpB6B=_tmpB62.c_varargs;_tmpB6C=_tmpB62.cyc_varargs;
_tmpB6D=_tmpB62.rgn_po;_tmpB6E=_tmpB62.attributes;}}_LL5AA: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB6E != 0;
_tmpB6E=_tmpB6E->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB6E->hd)){const char*
_tmp14D1;void*_tmp14D0[1];struct Cyc_String_pa_struct _tmp14CF;(_tmp14CF.tag=0,((
_tmp14CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB6E->hd)),((_tmp14D0[0]=& _tmp14CF,Cyc_Tcutil_terr(loc,((_tmp14D1="bad function type attribute %s",
_tag_dyneither(_tmp14D1,sizeof(char),31))),_tag_dyneither(_tmp14D0,sizeof(void*),
1)))))));}{void*_tmpC4F=(void*)_tmpB6E->hd;enum Cyc_Absyn_Format_Type _tmpC54;int
_tmpC55;int _tmpC56;_LL61E: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC50=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC4F;if(_tmpC50->tag != 1)goto _LL620;}_LL61F: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61D;_LL620: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC51=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC4F;if(_tmpC51->tag != 2)goto
_LL622;}_LL621: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61D;_LL622: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC52=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC4F;if(_tmpC52->tag != 3)goto _LL624;}_LL623: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL61D;_LL624: {struct Cyc_Absyn_Format_att_struct*_tmpC53=(
struct Cyc_Absyn_Format_att_struct*)_tmpC4F;if(_tmpC53->tag != 19)goto _LL626;else{
_tmpC54=_tmpC53->f1;_tmpC55=_tmpC53->f2;_tmpC56=_tmpC53->f3;}}_LL625: if(!
seen_format){seen_format=1;ft=_tmpC54;fmt_desc_arg=_tmpC55;fmt_arg_start=_tmpC56;}
else{const char*_tmp14D4;void*_tmp14D3;(_tmp14D3=0,Cyc_Tcutil_terr(loc,((_tmp14D4="function can't have multiple format attributes",
_tag_dyneither(_tmp14D4,sizeof(char),47))),_tag_dyneither(_tmp14D3,sizeof(void*),
0)));}goto _LL61D;_LL626:;_LL627: goto _LL61D;_LL61D:;};}if(num_convs > 1){const char*
_tmp14D7;void*_tmp14D6;(_tmp14D6=0,Cyc_Tcutil_terr(loc,((_tmp14D7="function can't have multiple calling conventions",
_tag_dyneither(_tmp14D7,sizeof(char),49))),_tag_dyneither(_tmp14D6,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB64);{struct Cyc_List_List*b=*
_tmpB64;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC5B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC5D;struct Cyc_Absyn_Kind _tmpC5E;enum Cyc_Absyn_KindQual
_tmpC5F;_LL629: {struct Cyc_Absyn_Eq_kb_struct*_tmpC5C=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC5B;if(_tmpC5C->tag != 0)goto _LL62B;else{_tmpC5D=_tmpC5C->f1;_tmpC5E=*_tmpC5D;
_tmpC5F=_tmpC5E.kind;if(_tmpC5F != Cyc_Absyn_MemKind)goto _LL62B;}}_LL62A:{const
char*_tmp14DB;void*_tmp14DA[1];struct Cyc_String_pa_struct _tmp14D9;(_tmp14D9.tag=
0,((_tmp14D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14DA[0]=& _tmp14D9,Cyc_Tcutil_terr(loc,((_tmp14DB="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14DB,sizeof(char),51))),_tag_dyneither(_tmp14DA,sizeof(void*),
1)))))));}goto _LL628;_LL62B:;_LL62C: goto _LL628;_LL628:;};}}{struct _RegionHandle*
_tmpC63=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14DC;struct Cyc_Tcutil_CVTEnv
_tmpC64=(_tmp14DC.r=_tmpC63,((_tmp14DC.kind_env=cvtenv.kind_env,((_tmp14DC.free_vars=
0,((_tmp14DC.free_evars=0,((_tmp14DC.generalize_evars=cvtenv.generalize_evars,((
_tmp14DC.fn_result=1,_tmp14DC)))))))))));_tmpC64=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC64,& Cyc_Tcutil_tmk,_tmpB69,1);_tmpB68->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB68->print_const,_tmpB69);_tmpC64.fn_result=0;{struct Cyc_List_List*a=
_tmpB6A;for(0;a != 0;a=a->tl){struct _tuple9*_tmpC65=(struct _tuple9*)a->hd;void*
_tmpC66=(*_tmpC65).f3;_tmpC64=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC64,& Cyc_Tcutil_tmk,
_tmpC66,1);{int _tmpC67=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC65).f2).print_const,
_tmpC66);((*_tmpC65).f2).real_const=_tmpC67;{void*_tmpC68=Cyc_Tcutil_compress(
_tmpC66);struct Cyc_Absyn_ArrayInfo _tmpC6A;void*_tmpC6B;struct Cyc_Absyn_Tqual
_tmpC6C;struct Cyc_Absyn_Exp*_tmpC6D;union Cyc_Absyn_Constraint*_tmpC6E;struct Cyc_Position_Segment*
_tmpC6F;_LL62E: {struct Cyc_Absyn_ArrayType_struct*_tmpC69=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC68;if(_tmpC69->tag != 9)goto _LL630;else{_tmpC6A=_tmpC69->f1;_tmpC6B=_tmpC6A.elt_type;
_tmpC6C=_tmpC6A.tq;_tmpC6D=_tmpC6A.num_elts;_tmpC6E=_tmpC6A.zero_term;_tmpC6F=
_tmpC6A.zt_loc;}}_LL62F: {void*_tmpC70=Cyc_Absyn_new_evar(0,0);_tmpC64=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC64,& Cyc_Tcutil_rk,_tmpC70,1);{struct Cyc_Absyn_Upper_b_struct _tmp14DF;
struct Cyc_Absyn_Upper_b_struct*_tmp14DE;void*_tmpC71=Cyc_Absyn_atb_typ(_tmpC6B,
_tmpC70,_tmpC6C,(void*)((_tmp14DE=_cycalloc(sizeof(*_tmp14DE)),((_tmp14DE[0]=((
_tmp14DF.tag=1,((_tmp14DF.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC6D),_tmp14DF)))),
_tmp14DE)))),_tmpC6E);(*_tmpC65).f3=_tmpC71;goto _LL62D;};}_LL630:;_LL631: goto
_LL62D;_LL62D:;};};}}if(_tmpB6C != 0){if(_tmpB6B){const char*_tmp14E2;void*
_tmp14E1;(_tmp14E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14E2="both c_vararg and cyc_vararg",_tag_dyneither(
_tmp14E2,sizeof(char),29))),_tag_dyneither(_tmp14E1,sizeof(void*),0)));}{void*
_tmpC77;int _tmpC78;struct Cyc_Absyn_VarargInfo _tmpC76=*_tmpB6C;_tmpC77=_tmpC76.type;
_tmpC78=_tmpC76.inject;_tmpC64=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC64,& Cyc_Tcutil_tmk,
_tmpC77,1);(_tmpB6C->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(
_tmpB6C->tq).print_const,_tmpC77);if(_tmpC78){void*_tmpC79=Cyc_Tcutil_compress(
_tmpC77);struct Cyc_Absyn_PtrInfo _tmpC7B;void*_tmpC7C;struct Cyc_Absyn_PtrAtts
_tmpC7D;union Cyc_Absyn_Constraint*_tmpC7E;union Cyc_Absyn_Constraint*_tmpC7F;
_LL633: {struct Cyc_Absyn_PointerType_struct*_tmpC7A=(struct Cyc_Absyn_PointerType_struct*)
_tmpC79;if(_tmpC7A->tag != 5)goto _LL635;else{_tmpC7B=_tmpC7A->f1;_tmpC7C=_tmpC7B.elt_typ;
_tmpC7D=_tmpC7B.ptr_atts;_tmpC7E=_tmpC7D.bounds;_tmpC7F=_tmpC7D.zero_term;}}
_LL634:{void*_tmpC80=Cyc_Tcutil_compress(_tmpC7C);_LL638: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC81=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC80;if(_tmpC81->tag != 3)goto
_LL63A;}_LL639: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC7F)){const char*_tmp14E5;void*_tmp14E4;(_tmp14E4=0,Cyc_Tcutil_terr(loc,((
_tmp14E5="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14E5,sizeof(
char),37))),_tag_dyneither(_tmp14E4,sizeof(void*),0)));}{void*_tmpC84=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC7E);_LL63D: {struct Cyc_Absyn_DynEither_b_struct*_tmpC85=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC84;if(_tmpC85->tag != 0)goto _LL63F;}_LL63E:{const char*_tmp14E8;void*_tmp14E7;(
_tmp14E7=0,Cyc_Tcutil_terr(loc,((_tmp14E8="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14E8,sizeof(char),44))),_tag_dyneither(_tmp14E7,sizeof(void*),
0)));}goto _LL63C;_LL63F:;_LL640: goto _LL63C;_LL63C:;}goto _LL637;_LL63A:;_LL63B:{
const char*_tmp14EB;void*_tmp14EA;(_tmp14EA=0,Cyc_Tcutil_terr(loc,((_tmp14EB="can't inject a non-datatype type",
_tag_dyneither(_tmp14EB,sizeof(char),33))),_tag_dyneither(_tmp14EA,sizeof(void*),
0)));}goto _LL637;_LL637:;}goto _LL632;_LL635:;_LL636:{const char*_tmp14EE;void*
_tmp14ED;(_tmp14ED=0,Cyc_Tcutil_terr(loc,((_tmp14EE="expecting a datatype pointer type",
_tag_dyneither(_tmp14EE,sizeof(char),34))),_tag_dyneither(_tmp14ED,sizeof(void*),
0)));}goto _LL632;_LL632:;}};}if(seen_format){int _tmpC8C=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB6A);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC8C) || 
fmt_arg_start < 0) || _tmpB6C == 0  && fmt_arg_start != 0) || _tmpB6C != 0  && 
fmt_arg_start != _tmpC8C + 1){const char*_tmp14F1;void*_tmp14F0;(_tmp14F0=0,Cyc_Tcutil_terr(
loc,((_tmp14F1="bad format descriptor",_tag_dyneither(_tmp14F1,sizeof(char),22))),
_tag_dyneither(_tmp14F0,sizeof(void*),0)));}else{void*_tmpC90;struct _tuple9
_tmpC8F=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB6A,
fmt_desc_arg - 1);_tmpC90=_tmpC8F.f3;{void*_tmpC91=Cyc_Tcutil_compress(_tmpC90);
struct Cyc_Absyn_PtrInfo _tmpC93;void*_tmpC94;struct Cyc_Absyn_PtrAtts _tmpC95;union
Cyc_Absyn_Constraint*_tmpC96;union Cyc_Absyn_Constraint*_tmpC97;_LL642: {struct
Cyc_Absyn_PointerType_struct*_tmpC92=(struct Cyc_Absyn_PointerType_struct*)
_tmpC91;if(_tmpC92->tag != 5)goto _LL644;else{_tmpC93=_tmpC92->f1;_tmpC94=_tmpC93.elt_typ;
_tmpC95=_tmpC93.ptr_atts;_tmpC96=_tmpC95.bounds;_tmpC97=_tmpC95.zero_term;}}
_LL643:{struct _tuple0 _tmp14F2;struct _tuple0 _tmpC99=(_tmp14F2.f1=Cyc_Tcutil_compress(
_tmpC94),((_tmp14F2.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC96),_tmp14F2)));void*_tmpC9A;enum Cyc_Absyn_Sign
_tmpC9C;enum Cyc_Absyn_Size_of _tmpC9D;void*_tmpC9E;_LL647: _tmpC9A=_tmpC99.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC9B=(struct Cyc_Absyn_IntType_struct*)_tmpC9A;
if(_tmpC9B->tag != 6)goto _LL649;else{_tmpC9C=_tmpC9B->f1;if(_tmpC9C != Cyc_Absyn_None)
goto _LL649;_tmpC9D=_tmpC9B->f2;if(_tmpC9D != Cyc_Absyn_Char_sz)goto _LL649;}};
_tmpC9E=_tmpC99.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC9F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC9E;if(_tmpC9F->tag != 0)goto _LL649;};_LL648: goto _LL646;_LL649:;_LL64A:{const
char*_tmp14F5;void*_tmp14F4;(_tmp14F4=0,Cyc_Tcutil_terr(loc,((_tmp14F5="format descriptor is not a char ? type",
_tag_dyneither(_tmp14F5,sizeof(char),39))),_tag_dyneither(_tmp14F4,sizeof(void*),
0)));}goto _LL646;_LL646:;}goto _LL641;_LL644:;_LL645:{const char*_tmp14F8;void*
_tmp14F7;(_tmp14F7=0,Cyc_Tcutil_terr(loc,((_tmp14F8="format descriptor is not a char ? type",
_tag_dyneither(_tmp14F8,sizeof(char),39))),_tag_dyneither(_tmp14F7,sizeof(void*),
0)));}goto _LL641;_LL641:;}if(fmt_arg_start != 0){int problem;{struct _tuple31
_tmp14F9;struct _tuple31 _tmpCA5=(_tmp14F9.f1=ft,((_tmp14F9.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB6C))->type)),
_tmp14F9)));enum Cyc_Absyn_Format_Type _tmpCA6;void*_tmpCA7;struct Cyc_Absyn_DatatypeInfo
_tmpCA9;union Cyc_Absyn_DatatypeInfoU _tmpCAA;struct Cyc_Absyn_Datatypedecl**
_tmpCAB;struct Cyc_Absyn_Datatypedecl*_tmpCAC;enum Cyc_Absyn_Format_Type _tmpCAD;
void*_tmpCAE;struct Cyc_Absyn_DatatypeInfo _tmpCB0;union Cyc_Absyn_DatatypeInfoU
_tmpCB1;struct Cyc_Absyn_Datatypedecl**_tmpCB2;struct Cyc_Absyn_Datatypedecl*
_tmpCB3;_LL64C: _tmpCA6=_tmpCA5.f1;if(_tmpCA6 != Cyc_Absyn_Printf_ft)goto _LL64E;
_tmpCA7=_tmpCA5.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpCA8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCA7;if(_tmpCA8->tag != 3)goto _LL64E;else{_tmpCA9=_tmpCA8->f1;_tmpCAA=_tmpCA9.datatype_info;
if((_tmpCAA.KnownDatatype).tag != 2)goto _LL64E;_tmpCAB=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCAA.KnownDatatype).val;_tmpCAC=*_tmpCAB;}};_LL64D: problem=Cyc_Absyn_qvar_cmp(
_tmpCAC->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL64B;_LL64E: _tmpCAD=
_tmpCA5.f1;if(_tmpCAD != Cyc_Absyn_Scanf_ft)goto _LL650;_tmpCAE=_tmpCA5.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpCAF=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCAE;if(_tmpCAF->tag != 3)goto _LL650;else{_tmpCB0=_tmpCAF->f1;_tmpCB1=_tmpCB0.datatype_info;
if((_tmpCB1.KnownDatatype).tag != 2)goto _LL650;_tmpCB2=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCB1.KnownDatatype).val;_tmpCB3=*_tmpCB2;}};_LL64F: problem=Cyc_Absyn_qvar_cmp(
_tmpCB3->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL64B;_LL650:;_LL651:
problem=1;goto _LL64B;_LL64B:;}if(problem){const char*_tmp14FC;void*_tmp14FB;(
_tmp14FB=0,Cyc_Tcutil_terr(loc,((_tmp14FC="format attribute and vararg types don't match",
_tag_dyneither(_tmp14FC,sizeof(char),46))),_tag_dyneither(_tmp14FB,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB6D;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpCB7;void*_tmpCB8;void*_tmpCB9;struct _tuple0*_tmpCB6=(struct _tuple0*)
rpo->hd;_tmpCB7=*_tmpCB6;_tmpCB8=_tmpCB7.f1;_tmpCB9=_tmpCB7.f2;_tmpC64=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC64,& Cyc_Tcutil_ek,_tmpCB8,1);_tmpC64=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC64,& Cyc_Tcutil_trk,_tmpCB9,1);}}if(*_tmpB66 != 0)_tmpC64=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC64,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB66))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC64.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpCBB;int _tmpCBC;struct
_tuple29 _tmpCBA=*((struct _tuple29*)tvs->hd);_tmpCBB=_tmpCBA.f1;_tmpCBC=_tmpCBA.f2;
if(!_tmpCBC)continue;{void*_tmpCBD=Cyc_Absyn_compress_kb(_tmpCBB->kind);struct
Cyc_Core_Opt*_tmpCBF;struct Cyc_Core_Opt**_tmpCC0;struct Cyc_Absyn_Kind*_tmpCC1;
struct Cyc_Absyn_Kind*_tmpCC3;struct Cyc_Absyn_Kind*_tmpCC5;struct Cyc_Absyn_Kind
_tmpCC6;enum Cyc_Absyn_KindQual _tmpCC7;struct Cyc_Absyn_Kind*_tmpCC9;struct Cyc_Absyn_Kind
_tmpCCA;enum Cyc_Absyn_KindQual _tmpCCB;struct Cyc_Core_Opt*_tmpCCD;struct Cyc_Core_Opt**
_tmpCCE;struct Cyc_Absyn_Kind*_tmpCCF;struct Cyc_Absyn_Kind _tmpCD0;enum Cyc_Absyn_KindQual
_tmpCD1;struct Cyc_Absyn_Kind*_tmpCD3;struct Cyc_Absyn_Kind _tmpCD4;enum Cyc_Absyn_KindQual
_tmpCD5;struct Cyc_Core_Opt*_tmpCD7;struct Cyc_Core_Opt**_tmpCD8;_LL653:{struct Cyc_Absyn_Less_kb_struct*
_tmpCBE=(struct Cyc_Absyn_Less_kb_struct*)_tmpCBD;if(_tmpCBE->tag != 2)goto _LL655;
else{_tmpCBF=_tmpCBE->f1;_tmpCC0=(struct Cyc_Core_Opt**)& _tmpCBE->f1;_tmpCC1=
_tmpCBE->f2;}}if(!(_tmpCC1->kind == Cyc_Absyn_RgnKind))goto _LL655;_LL654: if(
_tmpCC1->aliasqual == Cyc_Absyn_Top){*_tmpCC0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCC3=_tmpCC1;goto _LL656;}*_tmpCC0=Cyc_Tcutil_kind_to_bound_opt(_tmpCC1);
_tmpCC3=_tmpCC1;goto _LL656;_LL655:{struct Cyc_Absyn_Eq_kb_struct*_tmpCC2=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCBD;if(_tmpCC2->tag != 0)goto _LL657;else{_tmpCC3=
_tmpCC2->f1;}}if(!(_tmpCC3->kind == Cyc_Absyn_RgnKind))goto _LL657;_LL656:{struct
Cyc_Absyn_AccessEff_struct*_tmp150B;struct Cyc_Absyn_VarType_struct*_tmp150A;
struct Cyc_Absyn_VarType_struct _tmp1509;struct Cyc_Absyn_AccessEff_struct _tmp1508;
struct Cyc_List_List*_tmp1507;effect=((_tmp1507=_cycalloc(sizeof(*_tmp1507)),((
_tmp1507->hd=(void*)((_tmp150B=_cycalloc(sizeof(*_tmp150B)),((_tmp150B[0]=((
_tmp1508.tag=24,((_tmp1508.f1=(void*)((void*)((_tmp150A=_cycalloc(sizeof(*
_tmp150A)),((_tmp150A[0]=((_tmp1509.tag=2,((_tmp1509.f1=_tmpCBB,_tmp1509)))),
_tmp150A))))),_tmp1508)))),_tmp150B)))),((_tmp1507->tl=effect,_tmp1507))))));}
goto _LL652;_LL657: {struct Cyc_Absyn_Less_kb_struct*_tmpCC4=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCBD;if(_tmpCC4->tag != 2)goto _LL659;else{_tmpCC5=_tmpCC4->f2;_tmpCC6=*_tmpCC5;
_tmpCC7=_tmpCC6.kind;if(_tmpCC7 != Cyc_Absyn_IntKind)goto _LL659;}}_LL658: goto
_LL65A;_LL659: {struct Cyc_Absyn_Eq_kb_struct*_tmpCC8=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCBD;if(_tmpCC8->tag != 0)goto _LL65B;else{_tmpCC9=_tmpCC8->f1;_tmpCCA=*_tmpCC9;
_tmpCCB=_tmpCCA.kind;if(_tmpCCB != Cyc_Absyn_IntKind)goto _LL65B;}}_LL65A: goto
_LL652;_LL65B: {struct Cyc_Absyn_Less_kb_struct*_tmpCCC=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCBD;if(_tmpCCC->tag != 2)goto _LL65D;else{_tmpCCD=_tmpCCC->f1;_tmpCCE=(struct
Cyc_Core_Opt**)& _tmpCCC->f1;_tmpCCF=_tmpCCC->f2;_tmpCD0=*_tmpCCF;_tmpCD1=_tmpCD0.kind;
if(_tmpCD1 != Cyc_Absyn_EffKind)goto _LL65D;}}_LL65C:*_tmpCCE=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL65E;_LL65D: {struct Cyc_Absyn_Eq_kb_struct*_tmpCD2=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCBD;if(_tmpCD2->tag != 0)goto _LL65F;else{_tmpCD3=
_tmpCD2->f1;_tmpCD4=*_tmpCD3;_tmpCD5=_tmpCD4.kind;if(_tmpCD5 != Cyc_Absyn_EffKind)
goto _LL65F;}}_LL65E:{struct Cyc_Absyn_VarType_struct*_tmp1511;struct Cyc_Absyn_VarType_struct
_tmp1510;struct Cyc_List_List*_tmp150F;effect=((_tmp150F=_cycalloc(sizeof(*
_tmp150F)),((_tmp150F->hd=(void*)((_tmp1511=_cycalloc(sizeof(*_tmp1511)),((
_tmp1511[0]=((_tmp1510.tag=2,((_tmp1510.f1=_tmpCBB,_tmp1510)))),_tmp1511)))),((
_tmp150F->tl=effect,_tmp150F))))));}goto _LL652;_LL65F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCD6=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpCBD;if(_tmpCD6->tag != 1)goto
_LL661;else{_tmpCD7=_tmpCD6->f1;_tmpCD8=(struct Cyc_Core_Opt**)& _tmpCD6->f1;}}
_LL660:{struct Cyc_Absyn_Less_kb_struct*_tmp1517;struct Cyc_Absyn_Less_kb_struct
_tmp1516;struct Cyc_Core_Opt*_tmp1515;*_tmpCD8=((_tmp1515=_cycalloc(sizeof(*
_tmp1515)),((_tmp1515->v=(void*)((_tmp1517=_cycalloc(sizeof(*_tmp1517)),((
_tmp1517[0]=((_tmp1516.tag=2,((_tmp1516.f1=0,((_tmp1516.f2=& Cyc_Tcutil_ak,
_tmp1516)))))),_tmp1517)))),_tmp1515))));}goto _LL662;_LL661:;_LL662:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1526;struct Cyc_Absyn_VarType_struct*_tmp1525;struct Cyc_Absyn_VarType_struct
_tmp1524;struct Cyc_Absyn_RgnsEff_struct _tmp1523;struct Cyc_List_List*_tmp1522;
effect=((_tmp1522=_cycalloc(sizeof(*_tmp1522)),((_tmp1522->hd=(void*)((_tmp1526=
_cycalloc(sizeof(*_tmp1526)),((_tmp1526[0]=((_tmp1523.tag=26,((_tmp1523.f1=(void*)((
void*)((_tmp1525=_cycalloc(sizeof(*_tmp1525)),((_tmp1525[0]=((_tmp1524.tag=2,((
_tmp1524.f1=_tmpCBB,_tmp1524)))),_tmp1525))))),_tmp1523)))),_tmp1526)))),((
_tmp1522->tl=effect,_tmp1522))))));}goto _LL652;_LL652:;};}}{struct Cyc_List_List*
ts=_tmpC64.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCEA;int _tmpCEB;struct
_tuple30 _tmpCE9=*((struct _tuple30*)ts->hd);_tmpCEA=_tmpCE9.f1;_tmpCEB=_tmpCE9.f2;
if(!_tmpCEB)continue;{struct Cyc_Absyn_Kind*_tmpCEC=Cyc_Tcutil_typ_kind(_tmpCEA);
struct Cyc_Absyn_Kind _tmpCED;enum Cyc_Absyn_KindQual _tmpCEE;struct Cyc_Absyn_Kind
_tmpCEF;enum Cyc_Absyn_KindQual _tmpCF0;struct Cyc_Absyn_Kind _tmpCF1;enum Cyc_Absyn_KindQual
_tmpCF2;_LL664: _tmpCED=*_tmpCEC;_tmpCEE=_tmpCED.kind;if(_tmpCEE != Cyc_Absyn_RgnKind)
goto _LL666;_LL665:{struct Cyc_Absyn_AccessEff_struct*_tmp152C;struct Cyc_Absyn_AccessEff_struct
_tmp152B;struct Cyc_List_List*_tmp152A;effect=((_tmp152A=_cycalloc(sizeof(*
_tmp152A)),((_tmp152A->hd=(void*)((_tmp152C=_cycalloc(sizeof(*_tmp152C)),((
_tmp152C[0]=((_tmp152B.tag=24,((_tmp152B.f1=(void*)_tmpCEA,_tmp152B)))),_tmp152C)))),((
_tmp152A->tl=effect,_tmp152A))))));}goto _LL663;_LL666: _tmpCEF=*_tmpCEC;_tmpCF0=
_tmpCEF.kind;if(_tmpCF0 != Cyc_Absyn_EffKind)goto _LL668;_LL667:{struct Cyc_List_List*
_tmp152D;effect=((_tmp152D=_cycalloc(sizeof(*_tmp152D)),((_tmp152D->hd=_tmpCEA,((
_tmp152D->tl=effect,_tmp152D))))));}goto _LL663;_LL668: _tmpCF1=*_tmpCEC;_tmpCF2=
_tmpCF1.kind;if(_tmpCF2 != Cyc_Absyn_IntKind)goto _LL66A;_LL669: goto _LL663;_LL66A:;
_LL66B:{struct Cyc_Absyn_RgnsEff_struct*_tmp1533;struct Cyc_Absyn_RgnsEff_struct
_tmp1532;struct Cyc_List_List*_tmp1531;effect=((_tmp1531=_cycalloc(sizeof(*
_tmp1531)),((_tmp1531->hd=(void*)((_tmp1533=_cycalloc(sizeof(*_tmp1533)),((
_tmp1533[0]=((_tmp1532.tag=26,((_tmp1532.f1=(void*)_tmpCEA,_tmp1532)))),_tmp1533)))),((
_tmp1531->tl=effect,_tmp1531))))));}goto _LL663;_LL663:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1539;struct Cyc_Absyn_JoinEff_struct _tmp1538;struct Cyc_Core_Opt*_tmp1537;*
_tmpB66=((_tmp1537=_cycalloc(sizeof(*_tmp1537)),((_tmp1537->v=(void*)((_tmp1539=
_cycalloc(sizeof(*_tmp1539)),((_tmp1539[0]=((_tmp1538.tag=25,((_tmp1538.f1=
effect,_tmp1538)))),_tmp1539)))),_tmp1537))));};}if(*_tmpB64 != 0){struct Cyc_List_List*
bs=*_tmpB64;for(0;bs != 0;bs=bs->tl){void*_tmpCFD=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCFF;struct Cyc_Core_Opt**
_tmpD00;struct Cyc_Core_Opt*_tmpD02;struct Cyc_Core_Opt**_tmpD03;struct Cyc_Absyn_Kind*
_tmpD04;struct Cyc_Absyn_Kind _tmpD05;enum Cyc_Absyn_KindQual _tmpD06;enum Cyc_Absyn_AliasQual
_tmpD07;struct Cyc_Core_Opt*_tmpD09;struct Cyc_Core_Opt**_tmpD0A;struct Cyc_Absyn_Kind*
_tmpD0B;struct Cyc_Absyn_Kind _tmpD0C;enum Cyc_Absyn_KindQual _tmpD0D;enum Cyc_Absyn_AliasQual
_tmpD0E;struct Cyc_Core_Opt*_tmpD10;struct Cyc_Core_Opt**_tmpD11;struct Cyc_Absyn_Kind*
_tmpD12;struct Cyc_Absyn_Kind _tmpD13;enum Cyc_Absyn_KindQual _tmpD14;enum Cyc_Absyn_AliasQual
_tmpD15;struct Cyc_Core_Opt*_tmpD17;struct Cyc_Core_Opt**_tmpD18;struct Cyc_Absyn_Kind*
_tmpD19;struct Cyc_Absyn_Kind _tmpD1A;enum Cyc_Absyn_KindQual _tmpD1B;enum Cyc_Absyn_AliasQual
_tmpD1C;struct Cyc_Core_Opt*_tmpD1E;struct Cyc_Core_Opt**_tmpD1F;struct Cyc_Absyn_Kind*
_tmpD20;struct Cyc_Absyn_Kind _tmpD21;enum Cyc_Absyn_KindQual _tmpD22;enum Cyc_Absyn_AliasQual
_tmpD23;struct Cyc_Core_Opt*_tmpD25;struct Cyc_Core_Opt**_tmpD26;struct Cyc_Absyn_Kind*
_tmpD27;struct Cyc_Absyn_Kind _tmpD28;enum Cyc_Absyn_KindQual _tmpD29;enum Cyc_Absyn_AliasQual
_tmpD2A;struct Cyc_Core_Opt*_tmpD2C;struct Cyc_Core_Opt**_tmpD2D;struct Cyc_Absyn_Kind*
_tmpD2E;struct Cyc_Absyn_Kind _tmpD2F;enum Cyc_Absyn_KindQual _tmpD30;enum Cyc_Absyn_AliasQual
_tmpD31;struct Cyc_Core_Opt*_tmpD33;struct Cyc_Core_Opt**_tmpD34;struct Cyc_Absyn_Kind*
_tmpD35;struct Cyc_Absyn_Kind _tmpD36;enum Cyc_Absyn_KindQual _tmpD37;enum Cyc_Absyn_AliasQual
_tmpD38;struct Cyc_Core_Opt*_tmpD3A;struct Cyc_Core_Opt**_tmpD3B;struct Cyc_Absyn_Kind*
_tmpD3C;struct Cyc_Absyn_Kind*_tmpD3E;struct Cyc_Absyn_Kind _tmpD3F;enum Cyc_Absyn_KindQual
_tmpD40;_LL66D: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCFE=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCFD;if(_tmpCFE->tag != 1)goto _LL66F;else{_tmpCFF=_tmpCFE->f1;_tmpD00=(struct
Cyc_Core_Opt**)& _tmpCFE->f1;}}_LL66E:{const char*_tmp153D;void*_tmp153C[1];struct
Cyc_String_pa_struct _tmp153B;(_tmp153B.tag=0,((_tmp153B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp153C[0]=&
_tmp153B,Cyc_Tcutil_warn(loc,((_tmp153D="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp153D,sizeof(char),47))),_tag_dyneither(_tmp153C,sizeof(void*),
1)))))));}_tmpD03=_tmpD00;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_struct*
_tmpD01=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;if(_tmpD01->tag != 2)goto _LL671;
else{_tmpD02=_tmpD01->f1;_tmpD03=(struct Cyc_Core_Opt**)& _tmpD01->f1;_tmpD04=
_tmpD01->f2;_tmpD05=*_tmpD04;_tmpD06=_tmpD05.kind;if(_tmpD06 != Cyc_Absyn_BoxKind)
goto _LL671;_tmpD07=_tmpD05.aliasqual;if(_tmpD07 != Cyc_Absyn_Top)goto _LL671;}}
_LL670: _tmpD0A=_tmpD03;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_struct*
_tmpD08=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;if(_tmpD08->tag != 2)goto _LL673;
else{_tmpD09=_tmpD08->f1;_tmpD0A=(struct Cyc_Core_Opt**)& _tmpD08->f1;_tmpD0B=
_tmpD08->f2;_tmpD0C=*_tmpD0B;_tmpD0D=_tmpD0C.kind;if(_tmpD0D != Cyc_Absyn_MemKind)
goto _LL673;_tmpD0E=_tmpD0C.aliasqual;if(_tmpD0E != Cyc_Absyn_Top)goto _LL673;}}
_LL672: _tmpD11=_tmpD0A;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_struct*
_tmpD0F=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;if(_tmpD0F->tag != 2)goto _LL675;
else{_tmpD10=_tmpD0F->f1;_tmpD11=(struct Cyc_Core_Opt**)& _tmpD0F->f1;_tmpD12=
_tmpD0F->f2;_tmpD13=*_tmpD12;_tmpD14=_tmpD13.kind;if(_tmpD14 != Cyc_Absyn_MemKind)
goto _LL675;_tmpD15=_tmpD13.aliasqual;if(_tmpD15 != Cyc_Absyn_Aliasable)goto _LL675;}}
_LL674: _tmpD18=_tmpD11;goto _LL676;_LL675: {struct Cyc_Absyn_Less_kb_struct*
_tmpD16=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;if(_tmpD16->tag != 2)goto _LL677;
else{_tmpD17=_tmpD16->f1;_tmpD18=(struct Cyc_Core_Opt**)& _tmpD16->f1;_tmpD19=
_tmpD16->f2;_tmpD1A=*_tmpD19;_tmpD1B=_tmpD1A.kind;if(_tmpD1B != Cyc_Absyn_AnyKind)
goto _LL677;_tmpD1C=_tmpD1A.aliasqual;if(_tmpD1C != Cyc_Absyn_Top)goto _LL677;}}
_LL676: _tmpD1F=_tmpD18;goto _LL678;_LL677: {struct Cyc_Absyn_Less_kb_struct*
_tmpD1D=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;if(_tmpD1D->tag != 2)goto _LL679;
else{_tmpD1E=_tmpD1D->f1;_tmpD1F=(struct Cyc_Core_Opt**)& _tmpD1D->f1;_tmpD20=
_tmpD1D->f2;_tmpD21=*_tmpD20;_tmpD22=_tmpD21.kind;if(_tmpD22 != Cyc_Absyn_AnyKind)
goto _LL679;_tmpD23=_tmpD21.aliasqual;if(_tmpD23 != Cyc_Absyn_Aliasable)goto _LL679;}}
_LL678:*_tmpD1F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL66C;_LL679: {
struct Cyc_Absyn_Less_kb_struct*_tmpD24=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;
if(_tmpD24->tag != 2)goto _LL67B;else{_tmpD25=_tmpD24->f1;_tmpD26=(struct Cyc_Core_Opt**)&
_tmpD24->f1;_tmpD27=_tmpD24->f2;_tmpD28=*_tmpD27;_tmpD29=_tmpD28.kind;if(_tmpD29
!= Cyc_Absyn_MemKind)goto _LL67B;_tmpD2A=_tmpD28.aliasqual;if(_tmpD2A != Cyc_Absyn_Unique)
goto _LL67B;}}_LL67A: _tmpD2D=_tmpD26;goto _LL67C;_LL67B: {struct Cyc_Absyn_Less_kb_struct*
_tmpD2B=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;if(_tmpD2B->tag != 2)goto _LL67D;
else{_tmpD2C=_tmpD2B->f1;_tmpD2D=(struct Cyc_Core_Opt**)& _tmpD2B->f1;_tmpD2E=
_tmpD2B->f2;_tmpD2F=*_tmpD2E;_tmpD30=_tmpD2F.kind;if(_tmpD30 != Cyc_Absyn_AnyKind)
goto _LL67D;_tmpD31=_tmpD2F.aliasqual;if(_tmpD31 != Cyc_Absyn_Unique)goto _LL67D;}}
_LL67C:*_tmpD2D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL66C;_LL67D: {
struct Cyc_Absyn_Less_kb_struct*_tmpD32=(struct Cyc_Absyn_Less_kb_struct*)_tmpCFD;
if(_tmpD32->tag != 2)goto _LL67F;else{_tmpD33=_tmpD32->f1;_tmpD34=(struct Cyc_Core_Opt**)&
_tmpD32->f1;_tmpD35=_tmpD32->f2;_tmpD36=*_tmpD35;_tmpD37=_tmpD36.kind;if(_tmpD37
!= Cyc_Absyn_RgnKind)goto _LL67F;_tmpD38=_tmpD36.aliasqual;if(_tmpD38 != Cyc_Absyn_Top)
goto _LL67F;}}_LL67E:*_tmpD34=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL66C;_LL67F: {struct Cyc_Absyn_Less_kb_struct*_tmpD39=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCFD;if(_tmpD39->tag != 2)goto _LL681;else{_tmpD3A=_tmpD39->f1;_tmpD3B=(struct
Cyc_Core_Opt**)& _tmpD39->f1;_tmpD3C=_tmpD39->f2;}}_LL680:*_tmpD3B=Cyc_Tcutil_kind_to_bound_opt(
_tmpD3C);goto _LL66C;_LL681: {struct Cyc_Absyn_Eq_kb_struct*_tmpD3D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCFD;if(_tmpD3D->tag != 0)goto _LL683;else{_tmpD3E=_tmpD3D->f1;_tmpD3F=*_tmpD3E;
_tmpD40=_tmpD3F.kind;if(_tmpD40 != Cyc_Absyn_MemKind)goto _LL683;}}_LL682:{const
char*_tmp1540;void*_tmp153F;(_tmp153F=0,Cyc_Tcutil_terr(loc,((_tmp1540="functions can't abstract M types",
_tag_dyneither(_tmp1540,sizeof(char),33))),_tag_dyneither(_tmp153F,sizeof(void*),
0)));}goto _LL66C;_LL683:;_LL684: goto _LL66C;_LL66C:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC64.kind_env,*_tmpB64);_tmpC64.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC64.r,_tmpC64.free_vars,*_tmpB64);{struct Cyc_List_List*tvs=_tmpC64.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD47;int _tmpD48;struct
_tuple29 _tmpD46=*((struct _tuple29*)tvs->hd);_tmpD47=_tmpD46.f1;_tmpD48=_tmpD46.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD47,_tmpD48);}}{struct Cyc_List_List*evs=_tmpC64.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD4A;int _tmpD4B;struct _tuple30 _tmpD49=*((struct _tuple30*)evs->hd);
_tmpD4A=_tmpD49.f1;_tmpD4B=_tmpD49.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD4A,_tmpD4B);}};}goto _LL58A;};}_LL5AB: {struct Cyc_Absyn_TupleType_struct*
_tmpB6F=(struct Cyc_Absyn_TupleType_struct*)_tmpB27;if(_tmpB6F->tag != 11)goto
_LL5AD;else{_tmpB70=_tmpB6F->f1;}}_LL5AC: for(0;_tmpB70 != 0;_tmpB70=_tmpB70->tl){
struct _tuple11*_tmpD4D=(struct _tuple11*)_tmpB70->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD4D).f2,1);((*_tmpD4D).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD4D).f1).print_const,(*_tmpD4D).f2);}goto _LL58A;_LL5AD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB71=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB27;if(_tmpB71->tag != 13)goto
_LL5AF;else{_tmpB72=_tmpB71->f1;_tmpB73=_tmpB71->f2;}}_LL5AE: {struct
_RegionHandle*_tmpD4E=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB73 != 0;_tmpB73=_tmpB73->tl){struct Cyc_Absyn_Aggrfield _tmpD50;struct
_dyneither_ptr*_tmpD51;struct Cyc_Absyn_Tqual _tmpD52;struct Cyc_Absyn_Tqual*
_tmpD53;void*_tmpD54;struct Cyc_Absyn_Exp*_tmpD55;struct Cyc_List_List*_tmpD56;
struct Cyc_Absyn_Aggrfield*_tmpD4F=(struct Cyc_Absyn_Aggrfield*)_tmpB73->hd;
_tmpD50=*_tmpD4F;_tmpD51=_tmpD50.name;_tmpD52=_tmpD50.tq;_tmpD53=(struct Cyc_Absyn_Tqual*)&(*
_tmpD4F).tq;_tmpD54=_tmpD50.type;_tmpD55=_tmpD50.width;_tmpD56=_tmpD50.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD51)){
const char*_tmp1544;void*_tmp1543[1];struct Cyc_String_pa_struct _tmp1542;(_tmp1542.tag=
0,((_tmp1542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD51),((
_tmp1543[0]=& _tmp1542,Cyc_Tcutil_terr(loc,((_tmp1544="duplicate field %s",
_tag_dyneither(_tmp1544,sizeof(char),19))),_tag_dyneither(_tmp1543,sizeof(void*),
1)))))));}{const char*_tmp1545;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD51,((
_tmp1545="",_tag_dyneither(_tmp1545,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1546;prev_fields=((_tmp1546=_region_malloc(_tmpD4E,sizeof(*_tmp1546)),((
_tmp1546->hd=_tmpD51,((_tmp1546->tl=prev_fields,_tmp1546))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD54,1);_tmpD53->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD53->print_const,_tmpD54);if(_tmpB72 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD54)){const char*_tmp154A;void*_tmp1549[1];struct Cyc_String_pa_struct _tmp1548;(
_tmp1548.tag=0,((_tmp1548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD51),((_tmp1549[0]=& _tmp1548,Cyc_Tcutil_warn(loc,((_tmp154A="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp154A,sizeof(char),74))),_tag_dyneither(_tmp1549,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD54,_tmpD55,_tmpD51);Cyc_Tcutil_check_field_atts(
loc,_tmpD51,_tmpD56);}}goto _LL58A;}_LL5AF: {struct Cyc_Absyn_AggrType_struct*
_tmpB74=(struct Cyc_Absyn_AggrType_struct*)_tmpB27;if(_tmpB74->tag != 12)goto
_LL5B1;else{_tmpB75=_tmpB74->f1;_tmpB76=_tmpB75.aggr_info;_tmpB77=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB74->f1).aggr_info;_tmpB78=_tmpB75.targs;_tmpB79=(struct Cyc_List_List**)&(
_tmpB74->f1).targs;}}_LL5B0:{union Cyc_Absyn_AggrInfoU _tmpD5F=*_tmpB77;struct
_tuple4 _tmpD60;enum Cyc_Absyn_AggrKind _tmpD61;struct _tuple2*_tmpD62;struct Cyc_Core_Opt*
_tmpD63;struct Cyc_Absyn_Aggrdecl**_tmpD64;struct Cyc_Absyn_Aggrdecl*_tmpD65;
_LL686: if((_tmpD5F.UnknownAggr).tag != 1)goto _LL688;_tmpD60=(struct _tuple4)(
_tmpD5F.UnknownAggr).val;_tmpD61=_tmpD60.f1;_tmpD62=_tmpD60.f2;_tmpD63=_tmpD60.f3;
_LL687: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD66;_push_handler(&
_tmpD66);{int _tmpD68=0;if(setjmp(_tmpD66.handler))_tmpD68=1;if(!_tmpD68){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD62);{struct Cyc_Absyn_Aggrdecl*_tmpD69=*adp;if(_tmpD69->kind != 
_tmpD61){if(_tmpD69->kind == Cyc_Absyn_StructA){const char*_tmp154F;void*_tmp154E[
2];struct Cyc_String_pa_struct _tmp154D;struct Cyc_String_pa_struct _tmp154C;(
_tmp154C.tag=0,((_tmp154C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD62)),((_tmp154D.tag=0,((_tmp154D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD62)),((_tmp154E[0]=& _tmp154D,((
_tmp154E[1]=& _tmp154C,Cyc_Tcutil_terr(loc,((_tmp154F="expecting struct %s instead of union %s",
_tag_dyneither(_tmp154F,sizeof(char),40))),_tag_dyneither(_tmp154E,sizeof(void*),
2)))))))))))));}else{const char*_tmp1554;void*_tmp1553[2];struct Cyc_String_pa_struct
_tmp1552;struct Cyc_String_pa_struct _tmp1551;(_tmp1551.tag=0,((_tmp1551.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD62)),((
_tmp1552.tag=0,((_tmp1552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD62)),((_tmp1553[0]=& _tmp1552,((_tmp1553[1]=& _tmp1551,Cyc_Tcutil_terr(loc,((
_tmp1554="expecting union %s instead of struct %s",_tag_dyneither(_tmp1554,
sizeof(char),40))),_tag_dyneither(_tmp1553,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD63  && (int)_tmpD63->v){if(!((unsigned int)_tmpD69->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD69->impl))->tagged){const char*
_tmp1558;void*_tmp1557[1];struct Cyc_String_pa_struct _tmp1556;(_tmp1556.tag=0,((
_tmp1556.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD62)),((_tmp1557[0]=& _tmp1556,Cyc_Tcutil_terr(loc,((_tmp1558="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1558,sizeof(char),42))),_tag_dyneither(_tmp1557,sizeof(void*),
1)))))));}}*_tmpB77=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD67=(void*)_exn_thrown;void*_tmpD76=_tmpD67;_LL68B: {struct Cyc_Dict_Absent_struct*
_tmpD77=(struct Cyc_Dict_Absent_struct*)_tmpD76;if(_tmpD77->tag != Cyc_Dict_Absent)
goto _LL68D;}_LL68C: {struct Cyc_Tcenv_Genv*_tmpD78=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1559;struct Cyc_Absyn_Aggrdecl*_tmpD79=(_tmp1559=
_cycalloc(sizeof(*_tmp1559)),((_tmp1559->kind=_tmpD61,((_tmp1559->sc=Cyc_Absyn_Extern,((
_tmp1559->name=_tmpD62,((_tmp1559->tvs=0,((_tmp1559->impl=0,((_tmp1559->attributes=
0,_tmp1559)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD78,loc,_tmpD79);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD62);*_tmpB77=Cyc_Absyn_KnownAggr(adp);if(*_tmpB79 != 0){{const char*
_tmp155D;void*_tmp155C[1];struct Cyc_String_pa_struct _tmp155B;(_tmp155B.tag=0,((
_tmp155B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD62)),((_tmp155C[0]=& _tmp155B,Cyc_Tcutil_terr(loc,((_tmp155D="declare parameterized type %s before using",
_tag_dyneither(_tmp155D,sizeof(char),43))),_tag_dyneither(_tmp155C,sizeof(void*),
1)))))));}return cvtenv;}goto _LL68A;}_LL68D:;_LL68E:(void)_throw(_tmpD76);_LL68A:;}};}
_tmpD65=*adp;goto _LL689;}_LL688: if((_tmpD5F.KnownAggr).tag != 2)goto _LL685;
_tmpD64=(struct Cyc_Absyn_Aggrdecl**)(_tmpD5F.KnownAggr).val;_tmpD65=*_tmpD64;
_LL689: {struct Cyc_List_List*tvs=_tmpD65->tvs;struct Cyc_List_List*ts=*_tmpB79;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD7E=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD7F=(void*)ts->hd;{struct _tuple0 _tmp155E;
struct _tuple0 _tmpD81=(_tmp155E.f1=Cyc_Absyn_compress_kb(_tmpD7E->kind),((
_tmp155E.f2=_tmpD7F,_tmp155E)));void*_tmpD82;void*_tmpD84;struct Cyc_Absyn_Tvar*
_tmpD86;_LL690: _tmpD82=_tmpD81.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD83=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD82;if(_tmpD83->tag != 1)goto _LL692;};
_tmpD84=_tmpD81.f2;{struct Cyc_Absyn_VarType_struct*_tmpD85=(struct Cyc_Absyn_VarType_struct*)
_tmpD84;if(_tmpD85->tag != 2)goto _LL692;else{_tmpD86=_tmpD85->f1;}};_LL691: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD86);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD86,1);continue;_LL692:;_LL693: goto _LL68F;_LL68F:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp1562;void*_tmp1561[1];struct Cyc_String_pa_struct
_tmp1560;(_tmp1560.tag=0,((_tmp1560.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD65->name)),((_tmp1561[0]=& _tmp1560,
Cyc_Tcutil_terr(loc,((_tmp1562="too many parameters for type %s",_tag_dyneither(
_tmp1562,sizeof(char),32))),_tag_dyneither(_tmp1561,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1563;
hidden_ts=((_tmp1563=_cycalloc(sizeof(*_tmp1563)),((_tmp1563->hd=e,((_tmp1563->tl=
hidden_ts,_tmp1563))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB79=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB79,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL685:;}goto _LL58A;_LL5B1: {struct Cyc_Absyn_TypedefType_struct*
_tmpB7A=(struct Cyc_Absyn_TypedefType_struct*)_tmpB27;if(_tmpB7A->tag != 18)goto
_LL5B3;else{_tmpB7B=_tmpB7A->f1;_tmpB7C=_tmpB7A->f2;_tmpB7D=(struct Cyc_List_List**)&
_tmpB7A->f2;_tmpB7E=_tmpB7A->f3;_tmpB7F=(struct Cyc_Absyn_Typedefdecl**)& _tmpB7A->f3;
_tmpB80=_tmpB7A->f4;_tmpB81=(void***)& _tmpB7A->f4;}}_LL5B2: {struct Cyc_List_List*
targs=*_tmpB7D;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpD8B;
_push_handler(& _tmpD8B);{int _tmpD8D=0;if(setjmp(_tmpD8B.handler))_tmpD8D=1;if(!
_tmpD8D){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB7B);;_pop_handler();}else{
void*_tmpD8C=(void*)_exn_thrown;void*_tmpD8F=_tmpD8C;_LL695: {struct Cyc_Dict_Absent_struct*
_tmpD90=(struct Cyc_Dict_Absent_struct*)_tmpD8F;if(_tmpD90->tag != Cyc_Dict_Absent)
goto _LL697;}_LL696:{const char*_tmp1567;void*_tmp1566[1];struct Cyc_String_pa_struct
_tmp1565;(_tmp1565.tag=0,((_tmp1565.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB7B)),((_tmp1566[0]=& _tmp1565,Cyc_Tcutil_terr(
loc,((_tmp1567="unbound typedef name %s",_tag_dyneither(_tmp1567,sizeof(char),24))),
_tag_dyneither(_tmp1566,sizeof(void*),1)))))));}return cvtenv;_LL697:;_LL698:(
void)_throw(_tmpD8F);_LL694:;}};}*_tmpB7F=(struct Cyc_Absyn_Typedefdecl*)td;
_tmpB7B[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpD94=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=
Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple15*_tmp156A;struct Cyc_List_List*_tmp1569;inst=((_tmp1569=
_region_malloc(_tmpD94,sizeof(*_tmp1569)),((_tmp1569->hd=((_tmp156A=
_region_malloc(_tmpD94,sizeof(*_tmp156A)),((_tmp156A->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp156A->f2=(void*)ts->hd,_tmp156A)))))),((_tmp1569->tl=inst,_tmp1569))))));};}
if(ts != 0){const char*_tmp156E;void*_tmp156D[1];struct Cyc_String_pa_struct
_tmp156C;(_tmp156C.tag=0,((_tmp156C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB7B)),((_tmp156D[0]=& _tmp156C,Cyc_Tcutil_terr(
loc,((_tmp156E="too many parameters for typedef %s",_tag_dyneither(_tmp156E,
sizeof(char),35))),_tag_dyneither(_tmp156D,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp156F;hidden_ts=((_tmp156F=_cycalloc(sizeof(*_tmp156F)),((
_tmp156F->hd=e,((_tmp156F->tl=hidden_ts,_tmp156F))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp1572;struct Cyc_List_List*_tmp1571;inst=(
struct Cyc_List_List*)((_tmp1571=_cycalloc(sizeof(*_tmp1571)),((_tmp1571->hd=(
struct _tuple15*)((_tmp1572=_cycalloc(sizeof(*_tmp1572)),((_tmp1572->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp1572->f2=e,_tmp1572)))))),((_tmp1571->tl=inst,
_tmp1571))))));};}*_tmpB7D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD94,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1573;*_tmpB81=((_tmp1573=_cycalloc(sizeof(*
_tmp1573)),((_tmp1573[0]=new_typ,_tmp1573))));}}goto _LL58A;};}_LL5B3: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpB82=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpB27;if(_tmpB82->tag != 23)goto
_LL5B5;}_LL5B4: goto _LL5B6;_LL5B5: {struct Cyc_Absyn_UniqueRgn_struct*_tmpB83=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmpB27;if(_tmpB83->tag != 22)goto _LL5B7;}
_LL5B6: goto _LL5B8;_LL5B7: {struct Cyc_Absyn_HeapRgn_struct*_tmpB84=(struct Cyc_Absyn_HeapRgn_struct*)
_tmpB27;if(_tmpB84->tag != 21)goto _LL5B9;}_LL5B8: goto _LL58A;_LL5B9: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB85=(struct Cyc_Absyn_RgnHandleType_struct*)_tmpB27;if(_tmpB85->tag != 16)goto
_LL5BB;else{_tmpB86=(void*)_tmpB85->f1;}}_LL5BA: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB86,1);goto _LL58A;_LL5BB: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB87=(struct Cyc_Absyn_DynRgnType_struct*)_tmpB27;if(_tmpB87->tag != 17)goto
_LL5BD;else{_tmpB88=(void*)_tmpB87->f1;_tmpB89=(void*)_tmpB87->f2;}}_LL5BC:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB88,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB89,1);
goto _LL58A;_LL5BD: {struct Cyc_Absyn_AccessEff_struct*_tmpB8A=(struct Cyc_Absyn_AccessEff_struct*)
_tmpB27;if(_tmpB8A->tag != 24)goto _LL5BF;else{_tmpB8B=(void*)_tmpB8A->f1;}}_LL5BE:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB8B,1);
goto _LL58A;_LL5BF: {struct Cyc_Absyn_RgnsEff_struct*_tmpB8C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpB27;if(_tmpB8C->tag != 26)goto _LL5C1;else{_tmpB8D=(void*)_tmpB8C->f1;}}_LL5C0:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB8D,1);
goto _LL58A;_LL5C1: {struct Cyc_Absyn_JoinEff_struct*_tmpB8E=(struct Cyc_Absyn_JoinEff_struct*)
_tmpB27;if(_tmpB8E->tag != 25)goto _LL58A;else{_tmpB8F=_tmpB8E->f1;}}_LL5C2: for(0;
_tmpB8F != 0;_tmpB8F=_tmpB8F->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB8F->hd,1);}goto _LL58A;_LL58A:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpD9E=t;struct Cyc_Core_Opt*
_tmpDA0;struct Cyc_Core_Opt*_tmpDA1;_LL69A: {struct Cyc_Absyn_Evar_struct*_tmpD9F=(
struct Cyc_Absyn_Evar_struct*)_tmpD9E;if(_tmpD9F->tag != 1)goto _LL69C;else{_tmpDA0=
_tmpD9F->f1;_tmpDA1=_tmpD9F->f2;}}_LL69B: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpDA2=_tmpDA0;struct Cyc_Core_Opt _tmpDA3;struct Cyc_Absyn_Kind*_tmpDA4;_LL69F:
if(_tmpDA2 != 0)goto _LL6A1;_LL6A0:{const char*_tmp1574;s=((_tmp1574="kind=NULL ",
_tag_dyneither(_tmp1574,sizeof(char),11)));}goto _LL69E;_LL6A1: if(_tmpDA2 == 0)
goto _LL69E;_tmpDA3=*_tmpDA2;_tmpDA4=(struct Cyc_Absyn_Kind*)_tmpDA3.v;_LL6A2:{
const char*_tmp1578;void*_tmp1577[1];struct Cyc_String_pa_struct _tmp1576;s=(struct
_dyneither_ptr)((_tmp1576.tag=0,((_tmp1576.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpDA4)),((_tmp1577[0]=& _tmp1576,Cyc_aprintf(((
_tmp1578="kind=%s ",_tag_dyneither(_tmp1578,sizeof(char),9))),_tag_dyneither(
_tmp1577,sizeof(void*),1))))))));}goto _LL69E;_LL69E:;}{struct Cyc_Core_Opt*
_tmpDA9=_tmpDA1;struct Cyc_Core_Opt _tmpDAA;void*_tmpDAB;_LL6A4: if(_tmpDA9 != 0)
goto _LL6A6;_LL6A5:{const char*_tmp157C;void*_tmp157B[1];struct Cyc_String_pa_struct
_tmp157A;s=(struct _dyneither_ptr)((_tmp157A.tag=0,((_tmp157A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp157B[0]=& _tmp157A,Cyc_aprintf(((
_tmp157C="%s ref=NULL",_tag_dyneither(_tmp157C,sizeof(char),12))),_tag_dyneither(
_tmp157B,sizeof(void*),1))))))));}goto _LL6A3;_LL6A6: if(_tmpDA9 == 0)goto _LL6A3;
_tmpDAA=*_tmpDA9;_tmpDAB=(void*)_tmpDAA.v;_LL6A7:{const char*_tmp1581;void*
_tmp1580[2];struct Cyc_String_pa_struct _tmp157F;struct Cyc_String_pa_struct
_tmp157E;s=(struct _dyneither_ptr)((_tmp157E.tag=0,((_tmp157E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpDAB)),((
_tmp157F.tag=0,((_tmp157F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp1580[0]=& _tmp157F,((_tmp1580[1]=& _tmp157E,Cyc_aprintf(((_tmp1581="%s ref=%s",
_tag_dyneither(_tmp1581,sizeof(char),10))),_tag_dyneither(_tmp1580,sizeof(void*),
2))))))))))))));}goto _LL6A3;_LL6A3:;}goto _LL699;}_LL69C:;_LL69D: goto _LL699;
_LL699:;}{const char*_tmp1587;void*_tmp1586[3];struct Cyc_String_pa_struct _tmp1585;
struct Cyc_String_pa_struct _tmp1584;struct Cyc_String_pa_struct _tmp1583;(_tmp1583.tag=
0,((_tmp1583.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp1584.tag=0,((_tmp1584.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1585.tag=0,((
_tmp1585.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1586[0]=& _tmp1585,((_tmp1586[1]=& _tmp1584,((_tmp1586[2]=& _tmp1583,Cyc_Tcutil_terr(
loc,((_tmp1587="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1587,sizeof(char),51))),_tag_dyneither(_tmp1586,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpDB8=e->r;struct Cyc_List_List*_tmpDBE;struct Cyc_Absyn_Exp*_tmpDC0;struct
Cyc_Absyn_Exp*_tmpDC1;struct Cyc_Absyn_Exp*_tmpDC2;struct Cyc_Absyn_Exp*_tmpDC4;
struct Cyc_Absyn_Exp*_tmpDC5;struct Cyc_Absyn_Exp*_tmpDC7;struct Cyc_Absyn_Exp*
_tmpDC8;struct Cyc_Absyn_Exp*_tmpDCA;struct Cyc_Absyn_Exp*_tmpDCB;void*_tmpDCD;
struct Cyc_Absyn_Exp*_tmpDCE;void*_tmpDD0;void*_tmpDD2;void*_tmpDD4;struct Cyc_Absyn_Exp*
_tmpDD6;_LL6A9: {struct Cyc_Absyn_Const_e_struct*_tmpDB9=(struct Cyc_Absyn_Const_e_struct*)
_tmpDB8;if(_tmpDB9->tag != 0)goto _LL6AB;}_LL6AA: goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Enum_e_struct*
_tmpDBA=(struct Cyc_Absyn_Enum_e_struct*)_tmpDB8;if(_tmpDBA->tag != 32)goto _LL6AD;}
_LL6AC: goto _LL6AE;_LL6AD: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDBB=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDB8;if(_tmpDBB->tag != 33)goto _LL6AF;}_LL6AE: goto _LL6B0;_LL6AF: {struct Cyc_Absyn_Var_e_struct*
_tmpDBC=(struct Cyc_Absyn_Var_e_struct*)_tmpDB8;if(_tmpDBC->tag != 1)goto _LL6B1;}
_LL6B0: goto _LL6A8;_LL6B1: {struct Cyc_Absyn_Primop_e_struct*_tmpDBD=(struct Cyc_Absyn_Primop_e_struct*)
_tmpDB8;if(_tmpDBD->tag != 3)goto _LL6B3;else{_tmpDBE=_tmpDBD->f2;}}_LL6B2: for(0;
_tmpDBE != 0;_tmpDBE=_tmpDBE->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpDBE->hd,te,cvtenv);}goto _LL6A8;_LL6B3: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDBF=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDB8;if(_tmpDBF->tag != 6)goto
_LL6B5;else{_tmpDC0=_tmpDBF->f1;_tmpDC1=_tmpDBF->f2;_tmpDC2=_tmpDBF->f3;}}_LL6B4:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC0,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDC1,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC2,te,
cvtenv);goto _LL6A8;_LL6B5: {struct Cyc_Absyn_And_e_struct*_tmpDC3=(struct Cyc_Absyn_And_e_struct*)
_tmpDB8;if(_tmpDC3->tag != 7)goto _LL6B7;else{_tmpDC4=_tmpDC3->f1;_tmpDC5=_tmpDC3->f2;}}
_LL6B6: _tmpDC7=_tmpDC4;_tmpDC8=_tmpDC5;goto _LL6B8;_LL6B7: {struct Cyc_Absyn_Or_e_struct*
_tmpDC6=(struct Cyc_Absyn_Or_e_struct*)_tmpDB8;if(_tmpDC6->tag != 8)goto _LL6B9;
else{_tmpDC7=_tmpDC6->f1;_tmpDC8=_tmpDC6->f2;}}_LL6B8: _tmpDCA=_tmpDC7;_tmpDCB=
_tmpDC8;goto _LL6BA;_LL6B9: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDC9=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDB8;if(_tmpDC9->tag != 9)goto _LL6BB;else{_tmpDCA=_tmpDC9->f1;_tmpDCB=_tmpDC9->f2;}}
_LL6BA: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDCA,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDCB,te,cvtenv);goto _LL6A8;_LL6BB: {
struct Cyc_Absyn_Cast_e_struct*_tmpDCC=(struct Cyc_Absyn_Cast_e_struct*)_tmpDB8;
if(_tmpDCC->tag != 15)goto _LL6BD;else{_tmpDCD=(void*)_tmpDCC->f1;_tmpDCE=_tmpDCC->f2;}}
_LL6BC: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDCE,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDCD,1);goto
_LL6A8;_LL6BD: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDCF=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDB8;if(_tmpDCF->tag != 20)goto _LL6BF;else{_tmpDD0=(void*)_tmpDCF->f1;}}_LL6BE:
_tmpDD2=_tmpDD0;goto _LL6C0;_LL6BF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDD1=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDB8;if(_tmpDD1->tag != 18)goto _LL6C1;
else{_tmpDD2=(void*)_tmpDD1->f1;}}_LL6C0: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDD2,1);goto _LL6A8;_LL6C1: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDD3=(struct Cyc_Absyn_Valueof_e_struct*)_tmpDB8;if(_tmpDD3->tag != 39)goto
_LL6C3;else{_tmpDD4=(void*)_tmpDD3->f1;}}_LL6C2: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDD4,1);goto _LL6A8;_LL6C3: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDD5=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDB8;if(_tmpDD5->tag != 19)goto
_LL6C5;else{_tmpDD6=_tmpDD5->f1;}}_LL6C4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDD6,te,cvtenv);goto _LL6A8;_LL6C5:;_LL6C6: {const char*_tmp158A;void*_tmp1589;(
_tmp1589=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp158A="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp158A,sizeof(char),66))),_tag_dyneither(_tmp1589,sizeof(void*),
0)));}_LL6A8:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDD9=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDDB;struct
_tuple29 _tmpDDA=*((struct _tuple29*)vs->hd);_tmpDDB=_tmpDDA.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDD9,_tmpDDB);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDDD;struct _tuple30 _tmpDDC=*((struct _tuple30*)evs->hd);_tmpDDD=_tmpDDC.f1;{
void*_tmpDDE=Cyc_Tcutil_compress(_tmpDDD);struct Cyc_Core_Opt*_tmpDE0;struct Cyc_Core_Opt**
_tmpDE1;_LL6C8: {struct Cyc_Absyn_Evar_struct*_tmpDDF=(struct Cyc_Absyn_Evar_struct*)
_tmpDDE;if(_tmpDDF->tag != 1)goto _LL6CA;else{_tmpDE0=_tmpDDF->f4;_tmpDE1=(struct
Cyc_Core_Opt**)& _tmpDDF->f4;}}_LL6C9: if(*_tmpDE1 == 0){struct Cyc_Core_Opt*
_tmp158B;*_tmpDE1=((_tmp158B=_cycalloc(sizeof(*_tmp158B)),((_tmp158B->v=_tmpDD9,
_tmp158B))));}else{struct Cyc_List_List*_tmpDE3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDE1))->v;struct Cyc_List_List*_tmpDE4=0;for(0;_tmpDE3 != 0;
_tmpDE3=_tmpDE3->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDD9,(struct Cyc_Absyn_Tvar*)_tmpDE3->hd)){struct Cyc_List_List*_tmp158C;
_tmpDE4=((_tmp158C=_cycalloc(sizeof(*_tmp158C)),((_tmp158C->hd=(struct Cyc_Absyn_Tvar*)
_tmpDE3->hd,((_tmp158C->tl=_tmpDE4,_tmp158C))))));}}{struct Cyc_Core_Opt*_tmp158D;*
_tmpDE1=((_tmp158D=_cycalloc(sizeof(*_tmp158D)),((_tmp158D->v=_tmpDE4,_tmp158D))));};}
goto _LL6C7;_LL6CA:;_LL6CB: goto _LL6C7;_LL6C7:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpDE7=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpDE8=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp158E;struct Cyc_Tcutil_CVTEnv _tmpDE9=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp158E.r=_tmpDE8,((_tmp158E.kind_env=_tmpDE7,((_tmp158E.free_vars=0,((_tmp158E.free_evars=
0,((_tmp158E.generalize_evars=generalize_evars,((_tmp158E.fn_result=0,_tmp158E)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDEA=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDE8,(struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,
_tmpDE9.free_vars);struct Cyc_List_List*_tmpDEB=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpDE8,(void*(*)(struct _tuple30*))Cyc_Core_fst,_tmpDE9.free_evars);if(_tmpDE7 != 
0){struct Cyc_List_List*_tmpDEC=0;{struct Cyc_List_List*_tmpDED=_tmpDEA;for(0;(
unsigned int)_tmpDED;_tmpDED=_tmpDED->tl){struct Cyc_Absyn_Tvar*_tmpDEE=(struct
Cyc_Absyn_Tvar*)_tmpDED->hd;int found=0;{struct Cyc_List_List*_tmpDEF=_tmpDE7;for(
0;(unsigned int)_tmpDEF;_tmpDEF=_tmpDEF->tl){if(Cyc_Absyn_tvar_cmp(_tmpDEE,(
struct Cyc_Absyn_Tvar*)_tmpDEF->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp158F;_tmpDEC=((_tmp158F=_region_malloc(_tmpDE8,sizeof(*_tmp158F)),((_tmp158F->hd=(
struct Cyc_Absyn_Tvar*)_tmpDED->hd,((_tmp158F->tl=_tmpDEC,_tmp158F))))));}}}
_tmpDEA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpDEC);}{struct Cyc_List_List*x=_tmpDEA;for(0;x != 0;x=x->tl){void*_tmpDF1=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDF3;struct Cyc_Core_Opt**
_tmpDF4;struct Cyc_Core_Opt*_tmpDF6;struct Cyc_Core_Opt**_tmpDF7;struct Cyc_Absyn_Kind*
_tmpDF8;struct Cyc_Absyn_Kind _tmpDF9;enum Cyc_Absyn_KindQual _tmpDFA;enum Cyc_Absyn_AliasQual
_tmpDFB;struct Cyc_Core_Opt*_tmpDFD;struct Cyc_Core_Opt**_tmpDFE;struct Cyc_Absyn_Kind*
_tmpDFF;struct Cyc_Absyn_Kind _tmpE00;enum Cyc_Absyn_KindQual _tmpE01;enum Cyc_Absyn_AliasQual
_tmpE02;struct Cyc_Core_Opt*_tmpE04;struct Cyc_Core_Opt**_tmpE05;struct Cyc_Absyn_Kind*
_tmpE06;struct Cyc_Absyn_Kind _tmpE07;enum Cyc_Absyn_KindQual _tmpE08;enum Cyc_Absyn_AliasQual
_tmpE09;struct Cyc_Core_Opt*_tmpE0B;struct Cyc_Core_Opt**_tmpE0C;struct Cyc_Absyn_Kind*
_tmpE0D;struct Cyc_Absyn_Kind _tmpE0E;enum Cyc_Absyn_KindQual _tmpE0F;enum Cyc_Absyn_AliasQual
_tmpE10;struct Cyc_Core_Opt*_tmpE12;struct Cyc_Core_Opt**_tmpE13;struct Cyc_Absyn_Kind*
_tmpE14;struct Cyc_Absyn_Kind _tmpE15;enum Cyc_Absyn_KindQual _tmpE16;enum Cyc_Absyn_AliasQual
_tmpE17;struct Cyc_Core_Opt*_tmpE19;struct Cyc_Core_Opt**_tmpE1A;struct Cyc_Absyn_Kind*
_tmpE1B;struct Cyc_Absyn_Kind*_tmpE1D;struct Cyc_Absyn_Kind _tmpE1E;enum Cyc_Absyn_KindQual
_tmpE1F;enum Cyc_Absyn_AliasQual _tmpE20;_LL6CD: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDF2=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDF1;if(_tmpDF2->tag != 1)goto
_LL6CF;else{_tmpDF3=_tmpDF2->f1;_tmpDF4=(struct Cyc_Core_Opt**)& _tmpDF2->f1;}}
_LL6CE: _tmpDF7=_tmpDF4;goto _LL6D0;_LL6CF: {struct Cyc_Absyn_Less_kb_struct*
_tmpDF5=(struct Cyc_Absyn_Less_kb_struct*)_tmpDF1;if(_tmpDF5->tag != 2)goto _LL6D1;
else{_tmpDF6=_tmpDF5->f1;_tmpDF7=(struct Cyc_Core_Opt**)& _tmpDF5->f1;_tmpDF8=
_tmpDF5->f2;_tmpDF9=*_tmpDF8;_tmpDFA=_tmpDF9.kind;if(_tmpDFA != Cyc_Absyn_BoxKind)
goto _LL6D1;_tmpDFB=_tmpDF9.aliasqual;if(_tmpDFB != Cyc_Absyn_Top)goto _LL6D1;}}
_LL6D0: _tmpDFE=_tmpDF7;goto _LL6D2;_LL6D1: {struct Cyc_Absyn_Less_kb_struct*
_tmpDFC=(struct Cyc_Absyn_Less_kb_struct*)_tmpDF1;if(_tmpDFC->tag != 2)goto _LL6D3;
else{_tmpDFD=_tmpDFC->f1;_tmpDFE=(struct Cyc_Core_Opt**)& _tmpDFC->f1;_tmpDFF=
_tmpDFC->f2;_tmpE00=*_tmpDFF;_tmpE01=_tmpE00.kind;if(_tmpE01 != Cyc_Absyn_MemKind)
goto _LL6D3;_tmpE02=_tmpE00.aliasqual;if(_tmpE02 != Cyc_Absyn_Top)goto _LL6D3;}}
_LL6D2: _tmpE05=_tmpDFE;goto _LL6D4;_LL6D3: {struct Cyc_Absyn_Less_kb_struct*
_tmpE03=(struct Cyc_Absyn_Less_kb_struct*)_tmpDF1;if(_tmpE03->tag != 2)goto _LL6D5;
else{_tmpE04=_tmpE03->f1;_tmpE05=(struct Cyc_Core_Opt**)& _tmpE03->f1;_tmpE06=
_tmpE03->f2;_tmpE07=*_tmpE06;_tmpE08=_tmpE07.kind;if(_tmpE08 != Cyc_Absyn_MemKind)
goto _LL6D5;_tmpE09=_tmpE07.aliasqual;if(_tmpE09 != Cyc_Absyn_Aliasable)goto _LL6D5;}}
_LL6D4:*_tmpE05=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6CC;_LL6D5: {
struct Cyc_Absyn_Less_kb_struct*_tmpE0A=(struct Cyc_Absyn_Less_kb_struct*)_tmpDF1;
if(_tmpE0A->tag != 2)goto _LL6D7;else{_tmpE0B=_tmpE0A->f1;_tmpE0C=(struct Cyc_Core_Opt**)&
_tmpE0A->f1;_tmpE0D=_tmpE0A->f2;_tmpE0E=*_tmpE0D;_tmpE0F=_tmpE0E.kind;if(_tmpE0F
!= Cyc_Absyn_MemKind)goto _LL6D7;_tmpE10=_tmpE0E.aliasqual;if(_tmpE10 != Cyc_Absyn_Unique)
goto _LL6D7;}}_LL6D6:*_tmpE0C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6CC;_LL6D7: {struct Cyc_Absyn_Less_kb_struct*_tmpE11=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDF1;if(_tmpE11->tag != 2)goto _LL6D9;else{_tmpE12=_tmpE11->f1;_tmpE13=(struct
Cyc_Core_Opt**)& _tmpE11->f1;_tmpE14=_tmpE11->f2;_tmpE15=*_tmpE14;_tmpE16=_tmpE15.kind;
if(_tmpE16 != Cyc_Absyn_RgnKind)goto _LL6D9;_tmpE17=_tmpE15.aliasqual;if(_tmpE17 != 
Cyc_Absyn_Top)goto _LL6D9;}}_LL6D8:*_tmpE13=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6CC;_LL6D9: {struct Cyc_Absyn_Less_kb_struct*_tmpE18=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDF1;if(_tmpE18->tag != 2)goto _LL6DB;else{_tmpE19=_tmpE18->f1;_tmpE1A=(struct
Cyc_Core_Opt**)& _tmpE18->f1;_tmpE1B=_tmpE18->f2;}}_LL6DA:*_tmpE1A=Cyc_Tcutil_kind_to_bound_opt(
_tmpE1B);goto _LL6CC;_LL6DB: {struct Cyc_Absyn_Eq_kb_struct*_tmpE1C=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDF1;if(_tmpE1C->tag != 0)goto _LL6DD;else{_tmpE1D=_tmpE1C->f1;_tmpE1E=*_tmpE1D;
_tmpE1F=_tmpE1E.kind;if(_tmpE1F != Cyc_Absyn_MemKind)goto _LL6DD;_tmpE20=_tmpE1E.aliasqual;}}
_LL6DC:{const char*_tmp1596;void*_tmp1595[2];struct Cyc_String_pa_struct _tmp1594;
struct Cyc_Absyn_Kind*_tmp1593;struct Cyc_String_pa_struct _tmp1592;(_tmp1592.tag=0,((
_tmp1592.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp1593=_cycalloc_atomic(sizeof(*_tmp1593)),((_tmp1593->kind=Cyc_Absyn_MemKind,((
_tmp1593->aliasqual=_tmpE20,_tmp1593)))))))),((_tmp1594.tag=0,((_tmp1594.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1595[0]=& _tmp1594,((_tmp1595[1]=& _tmp1592,Cyc_Tcutil_terr(loc,((
_tmp1596="type variable %s cannot have kind %s",_tag_dyneither(_tmp1596,sizeof(
char),37))),_tag_dyneither(_tmp1595,sizeof(void*),2)))))))))))));}goto _LL6CC;
_LL6DD:;_LL6DE: goto _LL6CC;_LL6CC:;}}if(_tmpDEA != 0  || _tmpDEB != 0){{void*_tmpE26=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE28;struct Cyc_List_List*_tmpE29;
struct Cyc_List_List**_tmpE2A;struct Cyc_Core_Opt*_tmpE2B;struct Cyc_Absyn_Tqual
_tmpE2C;void*_tmpE2D;struct Cyc_List_List*_tmpE2E;int _tmpE2F;struct Cyc_Absyn_VarargInfo*
_tmpE30;struct Cyc_List_List*_tmpE31;struct Cyc_List_List*_tmpE32;_LL6E0: {struct
Cyc_Absyn_FnType_struct*_tmpE27=(struct Cyc_Absyn_FnType_struct*)_tmpE26;if(
_tmpE27->tag != 10)goto _LL6E2;else{_tmpE28=_tmpE27->f1;_tmpE29=_tmpE28.tvars;
_tmpE2A=(struct Cyc_List_List**)&(_tmpE27->f1).tvars;_tmpE2B=_tmpE28.effect;
_tmpE2C=_tmpE28.ret_tqual;_tmpE2D=_tmpE28.ret_typ;_tmpE2E=_tmpE28.args;_tmpE2F=
_tmpE28.c_varargs;_tmpE30=_tmpE28.cyc_varargs;_tmpE31=_tmpE28.rgn_po;_tmpE32=
_tmpE28.attributes;}}_LL6E1: if(*_tmpE2A == 0){*_tmpE2A=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDEA);_tmpDEA=0;}goto _LL6DF;_LL6E2:;
_LL6E3: goto _LL6DF;_LL6DF:;}if(_tmpDEA != 0){const char*_tmp159A;void*_tmp1599[1];
struct Cyc_String_pa_struct _tmp1598;(_tmp1598.tag=0,((_tmp1598.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDEA->hd)->name),((
_tmp1599[0]=& _tmp1598,Cyc_Tcutil_terr(loc,((_tmp159A="unbound type variable %s",
_tag_dyneither(_tmp159A,sizeof(char),25))),_tag_dyneither(_tmp1599,sizeof(void*),
1)))))));}if(_tmpDEB != 0)for(0;_tmpDEB != 0;_tmpDEB=_tmpDEB->tl){void*e=(void*)
_tmpDEB->hd;struct Cyc_Absyn_Kind*_tmpE36=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE37;enum Cyc_Absyn_KindQual _tmpE38;enum Cyc_Absyn_AliasQual _tmpE39;struct Cyc_Absyn_Kind
_tmpE3A;enum Cyc_Absyn_KindQual _tmpE3B;enum Cyc_Absyn_AliasQual _tmpE3C;struct Cyc_Absyn_Kind
_tmpE3D;enum Cyc_Absyn_KindQual _tmpE3E;enum Cyc_Absyn_AliasQual _tmpE3F;struct Cyc_Absyn_Kind
_tmpE40;enum Cyc_Absyn_KindQual _tmpE41;_LL6E5: _tmpE37=*_tmpE36;_tmpE38=_tmpE37.kind;
if(_tmpE38 != Cyc_Absyn_RgnKind)goto _LL6E7;_tmpE39=_tmpE37.aliasqual;if(_tmpE39 != 
Cyc_Absyn_Unique)goto _LL6E7;_LL6E6: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp159D;void*_tmp159C;(_tmp159C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159D="can't unify evar with unique region!",
_tag_dyneither(_tmp159D,sizeof(char),37))),_tag_dyneither(_tmp159C,sizeof(void*),
0)));}goto _LL6E4;_LL6E7: _tmpE3A=*_tmpE36;_tmpE3B=_tmpE3A.kind;if(_tmpE3B != Cyc_Absyn_RgnKind)
goto _LL6E9;_tmpE3C=_tmpE3A.aliasqual;if(_tmpE3C != Cyc_Absyn_Aliasable)goto _LL6E9;
_LL6E8: goto _LL6EA;_LL6E9: _tmpE3D=*_tmpE36;_tmpE3E=_tmpE3D.kind;if(_tmpE3E != Cyc_Absyn_RgnKind)
goto _LL6EB;_tmpE3F=_tmpE3D.aliasqual;if(_tmpE3F != Cyc_Absyn_Top)goto _LL6EB;
_LL6EA: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp15A0;
void*_tmp159F;(_tmp159F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15A0="can't unify evar with heap!",_tag_dyneither(
_tmp15A0,sizeof(char),28))),_tag_dyneither(_tmp159F,sizeof(void*),0)));}goto
_LL6E4;_LL6EB: _tmpE40=*_tmpE36;_tmpE41=_tmpE40.kind;if(_tmpE41 != Cyc_Absyn_EffKind)
goto _LL6ED;_LL6EC: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15A3;void*_tmp15A2;(_tmp15A2=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A3="can't unify evar with {}!",
_tag_dyneither(_tmp15A3,sizeof(char),26))),_tag_dyneither(_tmp15A2,sizeof(void*),
0)));}goto _LL6E4;_LL6ED:;_LL6EE:{const char*_tmp15A8;void*_tmp15A7[2];struct Cyc_String_pa_struct
_tmp15A6;struct Cyc_String_pa_struct _tmp15A5;(_tmp15A5.tag=0,((_tmp15A5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15A6.tag=0,((_tmp15A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15A7[0]=& _tmp15A6,((_tmp15A7[1]=& _tmp15A5,Cyc_Tcutil_terr(loc,((
_tmp15A8="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15A8,sizeof(char),52))),_tag_dyneither(_tmp15A7,sizeof(void*),2)))))))))))));}
goto _LL6E4;_LL6E4:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE4D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE4F;struct Cyc_List_List*_tmpE50;struct Cyc_Core_Opt*
_tmpE51;struct Cyc_Absyn_Tqual _tmpE52;void*_tmpE53;struct Cyc_List_List*_tmpE54;
_LL6F0: {struct Cyc_Absyn_FnType_struct*_tmpE4E=(struct Cyc_Absyn_FnType_struct*)
_tmpE4D;if(_tmpE4E->tag != 10)goto _LL6F2;else{_tmpE4F=_tmpE4E->f1;_tmpE50=_tmpE4F.tvars;
_tmpE51=_tmpE4F.effect;_tmpE52=_tmpE4F.ret_tqual;_tmpE53=_tmpE4F.ret_typ;_tmpE54=
_tmpE4F.args;}}_LL6F1: fd->tvs=_tmpE50;fd->effect=_tmpE51;{struct Cyc_List_List*
_tmpE55=fd->args;for(0;_tmpE55 != 0;(_tmpE55=_tmpE55->tl,_tmpE54=_tmpE54->tl)){(*((
struct _tuple25*)_tmpE55->hd)).f2=(*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmpE54))->hd)).f2;(*((struct _tuple25*)_tmpE55->hd)).f3=(*((struct
_tuple9*)_tmpE54->hd)).f3;}}fd->ret_tqual=_tmpE52;fd->ret_type=_tmpE53;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE53);
goto _LL6EF;_LL6F2:;_LL6F3: {const char*_tmp15AB;void*_tmp15AA;(_tmp15AA=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15AB="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp15AB,sizeof(char),38))),_tag_dyneither(_tmp15AA,sizeof(void*),
0)));}_LL6EF:;}{struct _RegionHandle*_tmpE58=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp15AC;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple25*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpE58,(struct _dyneither_ptr*(*)(struct _tuple25*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp15AC="function declaration has repeated parameter",
_tag_dyneither(_tmp15AC,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp15AD;fd->cached_typ=((
_tmp15AD=_cycalloc(sizeof(*_tmp15AD)),((_tmp15AD->v=t,_tmp15AD))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpE5B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp15AE;
struct Cyc_Tcutil_CVTEnv _tmpE5C=Cyc_Tcutil_check_valid_type(loc,te,((_tmp15AE.r=
_tmpE5B,((_tmp15AE.kind_env=bound_tvars,((_tmp15AE.free_vars=0,((_tmp15AE.free_evars=
0,((_tmp15AE.generalize_evars=0,((_tmp15AE.fn_result=0,_tmp15AE)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE5D=Cyc_Tcutil_remove_bound_tvars(
_tmpE5B,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE5B,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple29*))Cyc_Core_fst,_tmpE5C.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE5E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple30*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE5B,(void*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpE5C.free_evars);{struct Cyc_List_List*fs=_tmpE5D;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE5F=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp15B3;void*_tmp15B2[2];struct Cyc_String_pa_struct _tmp15B1;struct Cyc_String_pa_struct
_tmp15B0;(_tmp15B0.tag=0,((_tmp15B0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp15B1.tag=0,((_tmp15B1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE5F),((_tmp15B2[0]=& _tmp15B1,((
_tmp15B2[1]=& _tmp15B0,Cyc_Tcutil_terr(loc,((_tmp15B3="unbound type variable %s in type %s",
_tag_dyneither(_tmp15B3,sizeof(char),36))),_tag_dyneither(_tmp15B2,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE5E != 0)for(0;_tmpE5E != 0;_tmpE5E=
_tmpE5E->tl){void*e=(void*)_tmpE5E->hd;struct Cyc_Absyn_Kind*_tmpE64=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE65;enum Cyc_Absyn_KindQual _tmpE66;enum Cyc_Absyn_AliasQual
_tmpE67;struct Cyc_Absyn_Kind _tmpE68;enum Cyc_Absyn_KindQual _tmpE69;enum Cyc_Absyn_AliasQual
_tmpE6A;struct Cyc_Absyn_Kind _tmpE6B;enum Cyc_Absyn_KindQual _tmpE6C;enum Cyc_Absyn_AliasQual
_tmpE6D;struct Cyc_Absyn_Kind _tmpE6E;enum Cyc_Absyn_KindQual _tmpE6F;_LL6F5:
_tmpE65=*_tmpE64;_tmpE66=_tmpE65.kind;if(_tmpE66 != Cyc_Absyn_RgnKind)goto _LL6F7;
_tmpE67=_tmpE65.aliasqual;if(_tmpE67 != Cyc_Absyn_Unique)goto _LL6F7;_LL6F6: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp15B6;void*
_tmp15B5;(_tmp15B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15B6="can't unify evar with unique region!",
_tag_dyneither(_tmp15B6,sizeof(char),37))),_tag_dyneither(_tmp15B5,sizeof(void*),
0)));}goto _LL6F4;_LL6F7: _tmpE68=*_tmpE64;_tmpE69=_tmpE68.kind;if(_tmpE69 != Cyc_Absyn_RgnKind)
goto _LL6F9;_tmpE6A=_tmpE68.aliasqual;if(_tmpE6A != Cyc_Absyn_Aliasable)goto _LL6F9;
_LL6F8: goto _LL6FA;_LL6F9: _tmpE6B=*_tmpE64;_tmpE6C=_tmpE6B.kind;if(_tmpE6C != Cyc_Absyn_RgnKind)
goto _LL6FB;_tmpE6D=_tmpE6B.aliasqual;if(_tmpE6D != Cyc_Absyn_Top)goto _LL6FB;
_LL6FA: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp15B9;
void*_tmp15B8;(_tmp15B8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15B9="can't unify evar with heap!",_tag_dyneither(
_tmp15B9,sizeof(char),28))),_tag_dyneither(_tmp15B8,sizeof(void*),0)));}goto
_LL6F4;_LL6FB: _tmpE6E=*_tmpE64;_tmpE6F=_tmpE6E.kind;if(_tmpE6F != Cyc_Absyn_EffKind)
goto _LL6FD;_LL6FC: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15BC;void*_tmp15BB;(_tmp15BB=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15BC="can't unify evar with {}!",
_tag_dyneither(_tmp15BC,sizeof(char),26))),_tag_dyneither(_tmp15BB,sizeof(void*),
0)));}goto _LL6F4;_LL6FD:;_LL6FE:{const char*_tmp15C1;void*_tmp15C0[2];struct Cyc_String_pa_struct
_tmp15BF;struct Cyc_String_pa_struct _tmp15BE;(_tmp15BE.tag=0,((_tmp15BE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15BF.tag=0,((_tmp15BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15C0[0]=& _tmp15BF,((_tmp15C0[1]=& _tmp15BE,Cyc_Tcutil_terr(loc,((
_tmp15C1="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15C1,sizeof(char),52))),_tag_dyneither(_tmp15C0,sizeof(void*),2)))))))))))));}
goto _LL6F4;_LL6F4:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp15C6;void*_tmp15C5[2];struct Cyc_String_pa_struct
_tmp15C4;struct Cyc_String_pa_struct _tmp15C3;(_tmp15C3.tag=0,((_tmp15C3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15C4.tag=
0,((_tmp15C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15C5[0]=&
_tmp15C4,((_tmp15C5[1]=& _tmp15C3,Cyc_Tcutil_terr(loc,((_tmp15C6="%s: %s",
_tag_dyneither(_tmp15C6,sizeof(char),7))),_tag_dyneither(_tmp15C5,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp15C7;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15C7="duplicate type variable",
_tag_dyneither(_tmp15C7,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp15C8;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15C8="",_tag_dyneither(_tmp15C8,
sizeof(char),1))))!= 0){struct _tuple32*_tmp15CB;struct Cyc_List_List*_tmp15CA;
fields=((_tmp15CA=_cycalloc(sizeof(*_tmp15CA)),((_tmp15CA->hd=((_tmp15CB=
_cycalloc(sizeof(*_tmp15CB)),((_tmp15CB->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp15CB->f2=0,_tmp15CB)))))),((_tmp15CA->tl=fields,_tmp15CA))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp15CD;const char*_tmp15CC;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp15CD="struct",_tag_dyneither(_tmp15CD,sizeof(char),7)):((
_tmp15CC="union",_tag_dyneither(_tmp15CC,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple33 _tmpE84;struct Cyc_List_List*
_tmpE85;void*_tmpE86;struct _tuple33*_tmpE83=(struct _tuple33*)des->hd;_tmpE84=*
_tmpE83;_tmpE85=_tmpE84.f1;_tmpE86=_tmpE84.f2;if(_tmpE85 == 0){struct Cyc_List_List*
_tmpE87=fields;for(0;_tmpE87 != 0;_tmpE87=_tmpE87->tl){if(!(*((struct _tuple32*)
_tmpE87->hd)).f2){(*((struct _tuple32*)_tmpE87->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp15D3;struct Cyc_Absyn_FieldName_struct _tmp15D2;struct Cyc_List_List*_tmp15D1;(*((
struct _tuple33*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15D1=_cycalloc(sizeof(*
_tmp15D1)),((_tmp15D1->hd=(void*)((_tmp15D3=_cycalloc(sizeof(*_tmp15D3)),((
_tmp15D3[0]=((_tmp15D2.tag=1,((_tmp15D2.f1=((*((struct _tuple32*)_tmpE87->hd)).f1)->name,
_tmp15D2)))),_tmp15D3)))),((_tmp15D1->tl=0,_tmp15D1))))));}{struct _tuple34*
_tmp15D6;struct Cyc_List_List*_tmp15D5;ans=((_tmp15D5=_region_malloc(rgn,sizeof(*
_tmp15D5)),((_tmp15D5->hd=((_tmp15D6=_region_malloc(rgn,sizeof(*_tmp15D6)),((
_tmp15D6->f1=(*((struct _tuple32*)_tmpE87->hd)).f1,((_tmp15D6->f2=_tmpE86,
_tmp15D6)))))),((_tmp15D5->tl=ans,_tmp15D5))))));}break;}}if(_tmpE87 == 0){const
char*_tmp15DA;void*_tmp15D9[1];struct Cyc_String_pa_struct _tmp15D8;(_tmp15D8.tag=
0,((_tmp15D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15D9[0]=& _tmp15D8,Cyc_Tcutil_terr(loc,((_tmp15DA="too many arguments to %s",
_tag_dyneither(_tmp15DA,sizeof(char),25))),_tag_dyneither(_tmp15D9,sizeof(void*),
1)))))));}}else{if(_tmpE85->tl != 0){const char*_tmp15DD;void*_tmp15DC;(_tmp15DC=0,
Cyc_Tcutil_terr(loc,((_tmp15DD="multiple designators are not yet supported",
_tag_dyneither(_tmp15DD,sizeof(char),43))),_tag_dyneither(_tmp15DC,sizeof(void*),
0)));}else{void*_tmpE92=(void*)_tmpE85->hd;struct _dyneither_ptr*_tmpE95;_LL700: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE93=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE92;if(_tmpE93->tag != 0)goto _LL702;}_LL701:{const char*_tmp15E1;void*_tmp15E0[
1];struct Cyc_String_pa_struct _tmp15DF;(_tmp15DF.tag=0,((_tmp15DF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15E0[0]=& _tmp15DF,Cyc_Tcutil_terr(
loc,((_tmp15E1="array designator used in argument to %s",_tag_dyneither(_tmp15E1,
sizeof(char),40))),_tag_dyneither(_tmp15E0,sizeof(void*),1)))))));}goto _LL6FF;
_LL702: {struct Cyc_Absyn_FieldName_struct*_tmpE94=(struct Cyc_Absyn_FieldName_struct*)
_tmpE92;if(_tmpE94->tag != 1)goto _LL6FF;else{_tmpE95=_tmpE94->f1;}}_LL703: {
struct Cyc_List_List*_tmpE99=fields;for(0;_tmpE99 != 0;_tmpE99=_tmpE99->tl){if(Cyc_strptrcmp(
_tmpE95,((*((struct _tuple32*)_tmpE99->hd)).f1)->name)== 0){if((*((struct _tuple32*)
_tmpE99->hd)).f2){const char*_tmp15E5;void*_tmp15E4[1];struct Cyc_String_pa_struct
_tmp15E3;(_tmp15E3.tag=0,((_tmp15E3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE95),((_tmp15E4[0]=& _tmp15E3,Cyc_Tcutil_terr(loc,((_tmp15E5="member %s has already been used as an argument",
_tag_dyneither(_tmp15E5,sizeof(char),47))),_tag_dyneither(_tmp15E4,sizeof(void*),
1)))))));}(*((struct _tuple32*)_tmpE99->hd)).f2=1;{struct _tuple34*_tmp15E8;struct
Cyc_List_List*_tmp15E7;ans=((_tmp15E7=_region_malloc(rgn,sizeof(*_tmp15E7)),((
_tmp15E7->hd=((_tmp15E8=_region_malloc(rgn,sizeof(*_tmp15E8)),((_tmp15E8->f1=(*((
struct _tuple32*)_tmpE99->hd)).f1,((_tmp15E8->f2=_tmpE86,_tmp15E8)))))),((
_tmp15E7->tl=ans,_tmp15E7))))));}break;}}if(_tmpE99 == 0){const char*_tmp15EC;void*
_tmp15EB[1];struct Cyc_String_pa_struct _tmp15EA;(_tmp15EA.tag=0,((_tmp15EA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE95),((_tmp15EB[0]=& _tmp15EA,
Cyc_Tcutil_terr(loc,((_tmp15EC="bad field designator %s",_tag_dyneither(_tmp15EC,
sizeof(char),24))),_tag_dyneither(_tmp15EB,sizeof(void*),1)))))));}goto _LL6FF;}
_LL6FF:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){{const char*_tmp15EF;void*_tmp15EE;(
_tmp15EE=0,Cyc_Tcutil_terr(loc,((_tmp15EF="too few arguments to struct",
_tag_dyneither(_tmp15EF,sizeof(char),28))),_tag_dyneither(_tmp15EE,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple32*)fields->hd)).f2){if(found){const char*_tmp15F2;void*_tmp15F1;(_tmp15F1=
0,Cyc_Tcutil_terr(loc,((_tmp15F2="only one member of a union is allowed",
_tag_dyneither(_tmp15F2,sizeof(char),38))),_tag_dyneither(_tmp15F1,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15F5;void*_tmp15F4;(_tmp15F4=0,Cyc_Tcutil_terr(
loc,((_tmp15F5="missing member for union",_tag_dyneither(_tmp15F5,sizeof(char),
25))),_tag_dyneither(_tmp15F4,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEAA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEAC;void*_tmpEAD;struct Cyc_Absyn_PtrAtts _tmpEAE;union Cyc_Absyn_Constraint*
_tmpEAF;_LL705: {struct Cyc_Absyn_PointerType_struct*_tmpEAB=(struct Cyc_Absyn_PointerType_struct*)
_tmpEAA;if(_tmpEAB->tag != 5)goto _LL707;else{_tmpEAC=_tmpEAB->f1;_tmpEAD=_tmpEAC.elt_typ;
_tmpEAE=_tmpEAC.ptr_atts;_tmpEAF=_tmpEAE.bounds;}}_LL706: {void*_tmpEB0=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpEAF);_LL70A: {struct Cyc_Absyn_DynEither_b_struct*_tmpEB1=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEB0;if(_tmpEB1->tag != 0)goto _LL70C;}_LL70B:*elt_typ_dest=_tmpEAD;return 1;
_LL70C:;_LL70D: return 0;_LL709:;}_LL707:;_LL708: return 0;_LL704:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEB2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEB4;void*_tmpEB5;struct Cyc_Absyn_PtrAtts _tmpEB6;union Cyc_Absyn_Constraint*
_tmpEB7;_LL70F: {struct Cyc_Absyn_PointerType_struct*_tmpEB3=(struct Cyc_Absyn_PointerType_struct*)
_tmpEB2;if(_tmpEB3->tag != 5)goto _LL711;else{_tmpEB4=_tmpEB3->f1;_tmpEB5=_tmpEB4.elt_typ;
_tmpEB6=_tmpEB4.ptr_atts;_tmpEB7=_tmpEB6.zero_term;}}_LL710:*elt_typ_dest=
_tmpEB5;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpEB7);_LL711:;_LL712: return 0;_LL70E:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEB8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEBA;void*_tmpEBB;struct Cyc_Absyn_PtrAtts _tmpEBC;
union Cyc_Absyn_Constraint*_tmpEBD;union Cyc_Absyn_Constraint*_tmpEBE;struct Cyc_Absyn_ArrayInfo
_tmpEC0;void*_tmpEC1;struct Cyc_Absyn_Tqual _tmpEC2;struct Cyc_Absyn_Exp*_tmpEC3;
union Cyc_Absyn_Constraint*_tmpEC4;_LL714: {struct Cyc_Absyn_PointerType_struct*
_tmpEB9=(struct Cyc_Absyn_PointerType_struct*)_tmpEB8;if(_tmpEB9->tag != 5)goto
_LL716;else{_tmpEBA=_tmpEB9->f1;_tmpEBB=_tmpEBA.elt_typ;_tmpEBC=_tmpEBA.ptr_atts;
_tmpEBD=_tmpEBC.bounds;_tmpEBE=_tmpEBC.zero_term;}}_LL715: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEBE)){*ptr_type=t;*elt_type=
_tmpEBB;{void*_tmpEC5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpEBD);_LL71B: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEC6=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEC5;if(_tmpEC6->tag != 0)goto
_LL71D;}_LL71C:*is_dyneither=1;goto _LL71A;_LL71D:;_LL71E:*is_dyneither=0;goto
_LL71A;_LL71A:;}return 1;}else{return 0;}_LL716: {struct Cyc_Absyn_ArrayType_struct*
_tmpEBF=(struct Cyc_Absyn_ArrayType_struct*)_tmpEB8;if(_tmpEBF->tag != 9)goto
_LL718;else{_tmpEC0=_tmpEBF->f1;_tmpEC1=_tmpEC0.elt_type;_tmpEC2=_tmpEC0.tq;
_tmpEC3=_tmpEC0.num_elts;_tmpEC4=_tmpEC0.zero_term;}}_LL717: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEC4)){*elt_type=_tmpEC1;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp160A;struct Cyc_Absyn_PtrAtts
_tmp1609;struct Cyc_Absyn_Upper_b_struct _tmp1608;struct Cyc_Absyn_Upper_b_struct*
_tmp1607;struct Cyc_Absyn_PtrInfo _tmp1606;struct Cyc_Absyn_PointerType_struct*
_tmp1605;*ptr_type=(void*)((_tmp1605=_cycalloc(sizeof(*_tmp1605)),((_tmp1605[0]=((
_tmp160A.tag=5,((_tmp160A.f1=((_tmp1606.elt_typ=_tmpEC1,((_tmp1606.elt_tq=
_tmpEC2,((_tmp1606.ptr_atts=((_tmp1609.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1609.nullable=Cyc_Absyn_false_conref,((_tmp1609.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1607=_cycalloc(sizeof(*_tmp1607)),((
_tmp1607[0]=((_tmp1608.tag=1,((_tmp1608.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEC3),_tmp1608)))),_tmp1607))))),((_tmp1609.zero_term=_tmpEC4,((_tmp1609.ptrloc=
0,_tmp1609)))))))))),_tmp1606)))))),_tmp160A)))),_tmp1605))));}return 1;}else{
return 0;}_LL718:;_LL719: return 0;_LL713:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpECD=e1->r;
struct Cyc_Absyn_Exp*_tmpED0;struct Cyc_Absyn_Exp*_tmpED2;struct Cyc_Absyn_Exp*
_tmpED4;struct Cyc_Absyn_Exp*_tmpED6;struct Cyc_Absyn_Exp*_tmpED8;struct Cyc_Absyn_Exp*
_tmpEDA;_LL720: {struct Cyc_Absyn_Cast_e_struct*_tmpECE=(struct Cyc_Absyn_Cast_e_struct*)
_tmpECD;if(_tmpECE->tag != 15)goto _LL722;}_LL721: {const char*_tmp160E;void*
_tmp160D[1];struct Cyc_String_pa_struct _tmp160C;(_tmp160C.tag=0,((_tmp160C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp160D[0]=& _tmp160C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp160E="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp160E,sizeof(char),29))),_tag_dyneither(_tmp160D,sizeof(void*),1)))))));}
_LL722: {struct Cyc_Absyn_Deref_e_struct*_tmpECF=(struct Cyc_Absyn_Deref_e_struct*)
_tmpECD;if(_tmpECF->tag != 21)goto _LL724;else{_tmpED0=_tmpECF->f1;}}_LL723:
_tmpED2=_tmpED0;goto _LL725;_LL724: {struct Cyc_Absyn_Subscript_e_struct*_tmpED1=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpECD;if(_tmpED1->tag != 24)goto _LL726;
else{_tmpED2=_tmpED1->f1;}}_LL725: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpED2->topt))->v,ptr_type,is_dyneither,
elt_type);_LL726: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpED3=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpECD;if(_tmpED3->tag != 23)goto _LL728;else{_tmpED4=_tmpED3->f1;}}_LL727:
_tmpED6=_tmpED4;goto _LL729;_LL728: {struct Cyc_Absyn_AggrMember_e_struct*_tmpED5=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpECD;if(_tmpED5->tag != 22)goto _LL72A;
else{_tmpED6=_tmpED5->f1;}}_LL729: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpED6->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp1612;void*_tmp1611[1];struct Cyc_String_pa_struct _tmp1610;(_tmp1610.tag=
0,((_tmp1610.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1611[0]=& _tmp1610,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1612="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1612,sizeof(char),51))),_tag_dyneither(_tmp1611,sizeof(void*),
1)))))));}return 0;_LL72A: {struct Cyc_Absyn_Instantiate_e_struct*_tmpED7=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpECD;if(_tmpED7->tag != 14)goto _LL72C;else{
_tmpED8=_tmpED7->f1;}}_LL72B: _tmpEDA=_tmpED8;goto _LL72D;_LL72C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpED9=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpECD;if(_tmpED9->tag != 13)
goto _LL72E;else{_tmpEDA=_tmpED9->f1;}}_LL72D: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEDA->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1616;void*_tmp1615[1];struct Cyc_String_pa_struct
_tmp1614;(_tmp1614.tag=0,((_tmp1614.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1615[0]=& _tmp1614,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1616="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1616,sizeof(char),49))),_tag_dyneither(_tmp1615,sizeof(void*),
1)))))));}return 0;_LL72E: {struct Cyc_Absyn_Var_e_struct*_tmpEDB=(struct Cyc_Absyn_Var_e_struct*)
_tmpECD;if(_tmpEDB->tag != 1)goto _LL730;}_LL72F: return 0;_LL730:;_LL731: {const
char*_tmp161A;void*_tmp1619[1];struct Cyc_String_pa_struct _tmp1618;(_tmp1618.tag=
0,((_tmp1618.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1619[0]=& _tmp1618,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp161A="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp161A,sizeof(char),39))),_tag_dyneither(_tmp1619,sizeof(void*),
1)))))));}_LL71F:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpEE8=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpEEC;_LL733: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpEE9=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpEE8;if(_tmpEE9->tag != 23)goto
_LL735;}_LL734: return !must_be_unique;_LL735: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpEEA=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpEE8;if(_tmpEEA->tag != 22)goto
_LL737;}_LL736: return 1;_LL737: {struct Cyc_Absyn_VarType_struct*_tmpEEB=(struct
Cyc_Absyn_VarType_struct*)_tmpEE8;if(_tmpEEB->tag != 2)goto _LL739;else{_tmpEEC=
_tmpEEB->f1;}}_LL738: {struct Cyc_Absyn_Kind _tmpEEE;enum Cyc_Absyn_KindQual
_tmpEEF;enum Cyc_Absyn_AliasQual _tmpEF0;struct Cyc_Absyn_Kind*_tmpEED=Cyc_Tcutil_tvar_kind(
_tmpEEC,& Cyc_Tcutil_rk);_tmpEEE=*_tmpEED;_tmpEEF=_tmpEEE.kind;_tmpEF0=_tmpEEE.aliasqual;
if(_tmpEEF == Cyc_Absyn_RgnKind  && (_tmpEF0 == Cyc_Absyn_Unique  || _tmpEF0 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpEF1=Cyc_Absyn_compress_kb(_tmpEEC->kind);struct
Cyc_Core_Opt*_tmpEF3;struct Cyc_Core_Opt**_tmpEF4;struct Cyc_Absyn_Kind*_tmpEF5;
struct Cyc_Absyn_Kind _tmpEF6;enum Cyc_Absyn_KindQual _tmpEF7;enum Cyc_Absyn_AliasQual
_tmpEF8;_LL73C: {struct Cyc_Absyn_Less_kb_struct*_tmpEF2=(struct Cyc_Absyn_Less_kb_struct*)
_tmpEF1;if(_tmpEF2->tag != 2)goto _LL73E;else{_tmpEF3=_tmpEF2->f1;_tmpEF4=(struct
Cyc_Core_Opt**)& _tmpEF2->f1;_tmpEF5=_tmpEF2->f2;_tmpEF6=*_tmpEF5;_tmpEF7=_tmpEF6.kind;
if(_tmpEF7 != Cyc_Absyn_RgnKind)goto _LL73E;_tmpEF8=_tmpEF6.aliasqual;if(_tmpEF8 != 
Cyc_Absyn_Top)goto _LL73E;}}_LL73D:{struct Cyc_Absyn_Less_kb_struct*_tmp1620;
struct Cyc_Absyn_Less_kb_struct _tmp161F;struct Cyc_Core_Opt*_tmp161E;*_tmpEF4=((
_tmp161E=_cycalloc(sizeof(*_tmp161E)),((_tmp161E->v=(void*)((_tmp1620=_cycalloc(
sizeof(*_tmp1620)),((_tmp1620[0]=((_tmp161F.tag=2,((_tmp161F.f1=0,((_tmp161F.f2=&
Cyc_Tcutil_rk,_tmp161F)))))),_tmp1620)))),_tmp161E))));}return 0;_LL73E:;_LL73F:
return 1;_LL73B:;}return 0;}_LL739:;_LL73A: return 0;_LL732:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEFC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEFE;struct Cyc_Absyn_PtrAtts _tmpEFF;void*_tmpF00;struct Cyc_Absyn_Tvar*_tmpF02;
_LL741: {struct Cyc_Absyn_PointerType_struct*_tmpEFD=(struct Cyc_Absyn_PointerType_struct*)
_tmpEFC;if(_tmpEFD->tag != 5)goto _LL743;else{_tmpEFE=_tmpEFD->f1;_tmpEFF=_tmpEFE.ptr_atts;
_tmpF00=_tmpEFF.rgn;}}_LL742: return Cyc_Tcutil_is_noalias_region(_tmpF00,
must_be_unique);_LL743: {struct Cyc_Absyn_VarType_struct*_tmpF01=(struct Cyc_Absyn_VarType_struct*)
_tmpEFC;if(_tmpF01->tag != 2)goto _LL745;else{_tmpF02=_tmpF01->f1;}}_LL744: {
struct Cyc_Absyn_Kind _tmpF04;enum Cyc_Absyn_KindQual _tmpF05;enum Cyc_Absyn_AliasQual
_tmpF06;struct Cyc_Absyn_Kind*_tmpF03=Cyc_Tcutil_tvar_kind(_tmpF02,& Cyc_Tcutil_bk);
_tmpF04=*_tmpF03;_tmpF05=_tmpF04.kind;_tmpF06=_tmpF04.aliasqual;switch(_tmpF05){
case Cyc_Absyn_BoxKind: _LL747: goto _LL748;case Cyc_Absyn_AnyKind: _LL748: goto _LL749;
case Cyc_Absyn_MemKind: _LL749: if(_tmpF06 == Cyc_Absyn_Unique  || _tmpF06 == Cyc_Absyn_Top){
void*_tmpF07=Cyc_Absyn_compress_kb(_tmpF02->kind);struct Cyc_Core_Opt*_tmpF09;
struct Cyc_Core_Opt**_tmpF0A;struct Cyc_Absyn_Kind*_tmpF0B;struct Cyc_Absyn_Kind
_tmpF0C;enum Cyc_Absyn_KindQual _tmpF0D;enum Cyc_Absyn_AliasQual _tmpF0E;_LL74C: {
struct Cyc_Absyn_Less_kb_struct*_tmpF08=(struct Cyc_Absyn_Less_kb_struct*)_tmpF07;
if(_tmpF08->tag != 2)goto _LL74E;else{_tmpF09=_tmpF08->f1;_tmpF0A=(struct Cyc_Core_Opt**)&
_tmpF08->f1;_tmpF0B=_tmpF08->f2;_tmpF0C=*_tmpF0B;_tmpF0D=_tmpF0C.kind;_tmpF0E=
_tmpF0C.aliasqual;if(_tmpF0E != Cyc_Absyn_Top)goto _LL74E;}}_LL74D:{struct Cyc_Absyn_Less_kb_struct*
_tmp162A;struct Cyc_Absyn_Kind*_tmp1629;struct Cyc_Absyn_Less_kb_struct _tmp1628;
struct Cyc_Core_Opt*_tmp1627;*_tmpF0A=((_tmp1627=_cycalloc(sizeof(*_tmp1627)),((
_tmp1627->v=(void*)((_tmp162A=_cycalloc(sizeof(*_tmp162A)),((_tmp162A[0]=((
_tmp1628.tag=2,((_tmp1628.f1=0,((_tmp1628.f2=((_tmp1629=_cycalloc_atomic(sizeof(*
_tmp1629)),((_tmp1629->kind=_tmpF0D,((_tmp1629->aliasqual=Cyc_Absyn_Aliasable,
_tmp1629)))))),_tmp1628)))))),_tmp162A)))),_tmp1627))));}return 0;_LL74E:;_LL74F:
return 1;_LL74B:;}return 0;default: _LL74A: return 0;}}_LL745:;_LL746: return 0;_LL740:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpF13=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpF13))
return 1;{void*_tmpF14=_tmpF13;struct Cyc_List_List*_tmpF16;struct Cyc_Absyn_AggrInfo
_tmpF18;union Cyc_Absyn_AggrInfoU _tmpF19;struct Cyc_Absyn_Aggrdecl**_tmpF1A;struct
Cyc_List_List*_tmpF1B;struct Cyc_List_List*_tmpF1D;struct Cyc_Absyn_AggrInfo
_tmpF1F;union Cyc_Absyn_AggrInfoU _tmpF20;struct _tuple4 _tmpF21;struct Cyc_Absyn_DatatypeInfo
_tmpF23;union Cyc_Absyn_DatatypeInfoU _tmpF24;struct Cyc_List_List*_tmpF25;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF27;union Cyc_Absyn_DatatypeFieldInfoU _tmpF28;
struct Cyc_List_List*_tmpF29;_LL752: {struct Cyc_Absyn_TupleType_struct*_tmpF15=(
struct Cyc_Absyn_TupleType_struct*)_tmpF14;if(_tmpF15->tag != 11)goto _LL754;else{
_tmpF16=_tmpF15->f1;}}_LL753: while(_tmpF16 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpF16->hd)).f2))return 1;_tmpF16=_tmpF16->tl;}return 0;
_LL754: {struct Cyc_Absyn_AggrType_struct*_tmpF17=(struct Cyc_Absyn_AggrType_struct*)
_tmpF14;if(_tmpF17->tag != 12)goto _LL756;else{_tmpF18=_tmpF17->f1;_tmpF19=_tmpF18.aggr_info;
if((_tmpF19.KnownAggr).tag != 2)goto _LL756;_tmpF1A=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF19.KnownAggr).val;_tmpF1B=_tmpF18.targs;}}_LL755: if((*_tmpF1A)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF2A=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpF1A)->tvs,_tmpF1B);struct Cyc_List_List*_tmpF2B=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF1A)->impl))->fields;void*t;
while(_tmpF2B != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF2A,((struct Cyc_Absyn_Aggrfield*)
_tmpF2B->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF2B=_tmpF2B->tl;}return 0;}_LL756: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF1C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF14;if(_tmpF1C->tag != 13)goto
_LL758;else{_tmpF1D=_tmpF1C->f2;}}_LL757: while(_tmpF1D != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF1D->hd)->type))return 1;_tmpF1D=_tmpF1D->tl;}
return 0;_LL758: {struct Cyc_Absyn_AggrType_struct*_tmpF1E=(struct Cyc_Absyn_AggrType_struct*)
_tmpF14;if(_tmpF1E->tag != 12)goto _LL75A;else{_tmpF1F=_tmpF1E->f1;_tmpF20=_tmpF1F.aggr_info;
if((_tmpF20.UnknownAggr).tag != 1)goto _LL75A;_tmpF21=(struct _tuple4)(_tmpF20.UnknownAggr).val;}}
_LL759: {const char*_tmp162D;void*_tmp162C;(_tmp162C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp162D="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp162D,sizeof(char),36))),_tag_dyneither(_tmp162C,sizeof(void*),
0)));}_LL75A: {struct Cyc_Absyn_DatatypeType_struct*_tmpF22=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpF14;if(_tmpF22->tag != 3)goto _LL75C;else{_tmpF23=_tmpF22->f1;_tmpF24=_tmpF23.datatype_info;
_tmpF25=_tmpF23.targs;}}_LL75B: {union Cyc_Absyn_DatatypeInfoU _tmpF2E=_tmpF24;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF2F;struct _tuple2*_tmpF30;int _tmpF31;
struct Cyc_Absyn_Datatypedecl**_tmpF32;struct Cyc_Absyn_Datatypedecl*_tmpF33;
struct Cyc_Absyn_Datatypedecl _tmpF34;struct Cyc_List_List*_tmpF35;struct Cyc_Core_Opt*
_tmpF36;_LL761: if((_tmpF2E.UnknownDatatype).tag != 1)goto _LL763;_tmpF2F=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF2E.UnknownDatatype).val;_tmpF30=_tmpF2F.name;
_tmpF31=_tmpF2F.is_extensible;_LL762: {const char*_tmp1630;void*_tmp162F;(
_tmp162F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1630="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1630,
sizeof(char),40))),_tag_dyneither(_tmp162F,sizeof(void*),0)));}_LL763: if((
_tmpF2E.KnownDatatype).tag != 2)goto _LL760;_tmpF32=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF2E.KnownDatatype).val;_tmpF33=*_tmpF32;_tmpF34=*_tmpF33;_tmpF35=_tmpF34.tvs;
_tmpF36=_tmpF34.fields;_LL764: return 0;_LL760:;}_LL75C: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF26=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpF14;if(_tmpF26->tag != 4)
goto _LL75E;else{_tmpF27=_tmpF26->f1;_tmpF28=_tmpF27.field_info;_tmpF29=_tmpF27.targs;}}
_LL75D: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF39=_tmpF28;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF3A;struct _tuple3 _tmpF3B;struct Cyc_Absyn_Datatypedecl*_tmpF3C;struct Cyc_Absyn_Datatypefield*
_tmpF3D;_LL766: if((_tmpF39.UnknownDatatypefield).tag != 1)goto _LL768;_tmpF3A=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF39.UnknownDatatypefield).val;
_LL767: {const char*_tmp1633;void*_tmp1632;(_tmp1632=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1633="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1633,sizeof(char),46))),_tag_dyneither(_tmp1632,sizeof(void*),
0)));}_LL768: if((_tmpF39.KnownDatatypefield).tag != 2)goto _LL765;_tmpF3B=(struct
_tuple3)(_tmpF39.KnownDatatypefield).val;_tmpF3C=_tmpF3B.f1;_tmpF3D=_tmpF3B.f2;
_LL769: {struct Cyc_List_List*_tmpF40=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF3C->tvs,_tmpF29);struct Cyc_List_List*_tmpF41=
_tmpF3D->typs;while(_tmpF41 != 0){_tmpF13=Cyc_Tcutil_rsubstitute(rgn,_tmpF40,(*((
struct _tuple11*)_tmpF41->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpF13))return 1;_tmpF41=_tmpF41->tl;}return 0;}_LL765:;}_LL75E:;_LL75F: return 0;
_LL751:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF42=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF44;union Cyc_Absyn_AggrInfoU _tmpF45;struct Cyc_Absyn_Aggrdecl**_tmpF46;struct
Cyc_Absyn_Aggrdecl*_tmpF47;struct Cyc_List_List*_tmpF48;struct Cyc_List_List*
_tmpF4A;_LL76B: {struct Cyc_Absyn_AggrType_struct*_tmpF43=(struct Cyc_Absyn_AggrType_struct*)
_tmpF42;if(_tmpF43->tag != 12)goto _LL76D;else{_tmpF44=_tmpF43->f1;_tmpF45=_tmpF44.aggr_info;
if((_tmpF45.KnownAggr).tag != 2)goto _LL76D;_tmpF46=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF45.KnownAggr).val;_tmpF47=*_tmpF46;_tmpF48=_tmpF44.targs;}}_LL76C: _tmpF4A=
_tmpF47->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF47->impl))->fields;
goto _LL76E;_LL76D: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF49=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF42;if(_tmpF49->tag != 13)goto _LL76F;else{_tmpF4A=_tmpF49->f2;}}_LL76E: {
struct Cyc_Absyn_Aggrfield*_tmpF4B=Cyc_Absyn_lookup_field(_tmpF4A,f);{struct Cyc_Absyn_Aggrfield*
_tmpF4C=_tmpF4B;struct Cyc_Absyn_Aggrfield _tmpF4D;void*_tmpF4E;_LL772: if(_tmpF4C
!= 0)goto _LL774;_LL773: {const char*_tmp1636;void*_tmp1635;(_tmp1635=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1636="is_noalias_field: missing field",
_tag_dyneither(_tmp1636,sizeof(char),32))),_tag_dyneither(_tmp1635,sizeof(void*),
0)));}_LL774: if(_tmpF4C == 0)goto _LL771;_tmpF4D=*_tmpF4C;_tmpF4E=_tmpF4D.type;
_LL775: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF4E);_LL771:;}return 0;}
_LL76F:;_LL770: {const char*_tmp163A;void*_tmp1639[1];struct Cyc_String_pa_struct
_tmp1638;(_tmp1638.tag=0,((_tmp1638.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1639[0]=& _tmp1638,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp163A="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp163A,sizeof(char),36))),_tag_dyneither(_tmp1639,sizeof(void*),
1)))))));}_LL76A:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF54=e->r;void*_tmpF56;struct Cyc_Absyn_Exp*
_tmpF59;struct Cyc_Absyn_Exp*_tmpF5B;struct Cyc_Absyn_Exp*_tmpF5D;struct
_dyneither_ptr*_tmpF5E;struct Cyc_Absyn_Exp*_tmpF60;struct Cyc_Absyn_Exp*_tmpF61;
struct Cyc_Absyn_Exp*_tmpF63;struct Cyc_Absyn_Exp*_tmpF64;struct Cyc_Absyn_Exp*
_tmpF66;struct Cyc_Absyn_Exp*_tmpF68;struct Cyc_Absyn_Stmt*_tmpF6A;_LL777: {struct
Cyc_Absyn_Var_e_struct*_tmpF55=(struct Cyc_Absyn_Var_e_struct*)_tmpF54;if(_tmpF55->tag
!= 1)goto _LL779;else{_tmpF56=(void*)_tmpF55->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF57=(struct Cyc_Absyn_Global_b_struct*)_tmpF56;if(_tmpF57->tag != 1)goto _LL779;};}}
_LL778: return 0;_LL779: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF58=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF54;if(_tmpF58->tag != 23)goto _LL77B;else{_tmpF59=_tmpF58->f1;}}_LL77A:
_tmpF5B=_tmpF59;goto _LL77C;_LL77B: {struct Cyc_Absyn_Deref_e_struct*_tmpF5A=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF54;if(_tmpF5A->tag != 21)goto _LL77D;else{
_tmpF5B=_tmpF5A->f1;}}_LL77C: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF5B->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF5B);_LL77D: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF5C=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF54;if(_tmpF5C->tag != 22)goto _LL77F;else{_tmpF5D=_tmpF5C->f1;_tmpF5E=_tmpF5C->f2;}}
_LL77E: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF5D);_LL77F: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF5F=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF54;if(_tmpF5F->tag != 24)goto
_LL781;else{_tmpF60=_tmpF5F->f1;_tmpF61=_tmpF5F->f2;}}_LL780: {void*_tmpF6B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF60->topt))->v);_LL78C: {struct Cyc_Absyn_TupleType_struct*
_tmpF6C=(struct Cyc_Absyn_TupleType_struct*)_tmpF6B;if(_tmpF6C->tag != 11)goto
_LL78E;}_LL78D: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF60);_LL78E:;_LL78F:
return 0;_LL78B:;}_LL781: {struct Cyc_Absyn_Conditional_e_struct*_tmpF62=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF54;if(_tmpF62->tag != 6)goto _LL783;else{
_tmpF63=_tmpF62->f2;_tmpF64=_tmpF62->f3;}}_LL782: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF63) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF64);_LL783: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF65=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF54;if(_tmpF65->tag != 9)goto _LL785;
else{_tmpF66=_tmpF65->f2;}}_LL784: _tmpF68=_tmpF66;goto _LL786;_LL785: {struct Cyc_Absyn_Cast_e_struct*
_tmpF67=(struct Cyc_Absyn_Cast_e_struct*)_tmpF54;if(_tmpF67->tag != 15)goto _LL787;
else{_tmpF68=_tmpF67->f2;}}_LL786: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF68);
_LL787: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF69=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF54;if(_tmpF69->tag != 37)goto _LL789;else{_tmpF6A=_tmpF69->f1;}}_LL788: while(
1){void*_tmpF6D=_tmpF6A->r;struct Cyc_Absyn_Stmt*_tmpF6F;struct Cyc_Absyn_Stmt*
_tmpF70;struct Cyc_Absyn_Decl*_tmpF72;struct Cyc_Absyn_Stmt*_tmpF73;struct Cyc_Absyn_Exp*
_tmpF75;_LL791: {struct Cyc_Absyn_Seq_s_struct*_tmpF6E=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF6D;if(_tmpF6E->tag != 2)goto _LL793;else{_tmpF6F=_tmpF6E->f1;_tmpF70=_tmpF6E->f2;}}
_LL792: _tmpF6A=_tmpF70;goto _LL790;_LL793: {struct Cyc_Absyn_Decl_s_struct*_tmpF71=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF6D;if(_tmpF71->tag != 12)goto _LL795;else{
_tmpF72=_tmpF71->f1;_tmpF73=_tmpF71->f2;}}_LL794: _tmpF6A=_tmpF73;goto _LL790;
_LL795: {struct Cyc_Absyn_Exp_s_struct*_tmpF74=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF6D;if(_tmpF74->tag != 1)goto _LL797;else{_tmpF75=_tmpF74->f1;}}_LL796: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF75);_LL797:;_LL798: {const char*_tmp163D;
void*_tmp163C;(_tmp163C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp163D="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp163D,sizeof(char),40))),_tag_dyneither(_tmp163C,sizeof(void*),
0)));}_LL790:;}_LL789:;_LL78A: return 1;_LL776:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp163E;struct _tuple17 bogus_ans=(_tmp163E.f1=0,((_tmp163E.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp163E)));void*_tmpF78=e->r;struct _tuple2*_tmpF7A;
void*_tmpF7B;struct Cyc_Absyn_Exp*_tmpF7D;struct _dyneither_ptr*_tmpF7E;int _tmpF7F;
struct Cyc_Absyn_Exp*_tmpF81;struct _dyneither_ptr*_tmpF82;int _tmpF83;struct Cyc_Absyn_Exp*
_tmpF85;struct Cyc_Absyn_Exp*_tmpF87;struct Cyc_Absyn_Exp*_tmpF88;_LL79A: {struct
Cyc_Absyn_Var_e_struct*_tmpF79=(struct Cyc_Absyn_Var_e_struct*)_tmpF78;if(_tmpF79->tag
!= 1)goto _LL79C;else{_tmpF7A=_tmpF79->f1;_tmpF7B=(void*)_tmpF79->f2;}}_LL79B: {
void*_tmpF89=_tmpF7B;struct Cyc_Absyn_Vardecl*_tmpF8D;struct Cyc_Absyn_Vardecl*
_tmpF8F;struct Cyc_Absyn_Vardecl*_tmpF91;struct Cyc_Absyn_Vardecl*_tmpF93;_LL7A7: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF8A=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF89;if(_tmpF8A->tag != 0)goto _LL7A9;}_LL7A8: goto _LL7AA;_LL7A9: {struct Cyc_Absyn_Funname_b_struct*
_tmpF8B=(struct Cyc_Absyn_Funname_b_struct*)_tmpF89;if(_tmpF8B->tag != 2)goto
_LL7AB;}_LL7AA: return bogus_ans;_LL7AB: {struct Cyc_Absyn_Global_b_struct*_tmpF8C=(
struct Cyc_Absyn_Global_b_struct*)_tmpF89;if(_tmpF8C->tag != 1)goto _LL7AD;else{
_tmpF8D=_tmpF8C->f1;}}_LL7AC: {void*_tmpF94=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL7B4: {struct Cyc_Absyn_ArrayType_struct*
_tmpF95=(struct Cyc_Absyn_ArrayType_struct*)_tmpF94;if(_tmpF95->tag != 9)goto
_LL7B6;}_LL7B5: {struct _tuple17 _tmp163F;return(_tmp163F.f1=1,((_tmp163F.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp163F)));}_LL7B6:;_LL7B7: {struct _tuple17 _tmp1640;
return(_tmp1640.f1=(_tmpF8D->tq).real_const,((_tmp1640.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1640)));}_LL7B3:;}_LL7AD: {struct Cyc_Absyn_Local_b_struct*_tmpF8E=(struct Cyc_Absyn_Local_b_struct*)
_tmpF89;if(_tmpF8E->tag != 4)goto _LL7AF;else{_tmpF8F=_tmpF8E->f1;}}_LL7AE: {void*
_tmpF98=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL7B9: {struct Cyc_Absyn_ArrayType_struct*_tmpF99=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF98;if(_tmpF99->tag != 9)goto _LL7BB;}_LL7BA: {struct _tuple17 _tmp1641;return(
_tmp1641.f1=1,((_tmp1641.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF8F->rgn))->v,
_tmp1641)));}_LL7BB:;_LL7BC: _tmpF8F->escapes=1;{struct _tuple17 _tmp1642;return(
_tmp1642.f1=(_tmpF8F->tq).real_const,((_tmp1642.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8F->rgn))->v,_tmp1642)));};_LL7B8:;}_LL7AF: {struct Cyc_Absyn_Pat_b_struct*
_tmpF90=(struct Cyc_Absyn_Pat_b_struct*)_tmpF89;if(_tmpF90->tag != 5)goto _LL7B1;
else{_tmpF91=_tmpF90->f1;}}_LL7B0: _tmpF93=_tmpF91;goto _LL7B2;_LL7B1: {struct Cyc_Absyn_Param_b_struct*
_tmpF92=(struct Cyc_Absyn_Param_b_struct*)_tmpF89;if(_tmpF92->tag != 3)goto _LL7A6;
else{_tmpF93=_tmpF92->f1;}}_LL7B2: _tmpF93->escapes=1;{struct _tuple17 _tmp1643;
return(_tmp1643.f1=(_tmpF93->tq).real_const,((_tmp1643.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF93->rgn))->v,_tmp1643)));};_LL7A6:;}_LL79C: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF7C=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF78;if(_tmpF7C->tag != 22)goto
_LL79E;else{_tmpF7D=_tmpF7C->f1;_tmpF7E=_tmpF7C->f2;_tmpF7F=_tmpF7C->f3;}}_LL79D:
if(_tmpF7F)return bogus_ans;{void*_tmpF9D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF7D->topt))->v);struct Cyc_List_List*_tmpF9F;struct Cyc_Absyn_AggrInfo
_tmpFA1;union Cyc_Absyn_AggrInfoU _tmpFA2;struct Cyc_Absyn_Aggrdecl**_tmpFA3;struct
Cyc_Absyn_Aggrdecl*_tmpFA4;_LL7BE: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF9E=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF9D;if(_tmpF9E->tag != 13)goto _LL7C0;
else{_tmpF9F=_tmpF9E->f2;}}_LL7BF: {struct Cyc_Absyn_Aggrfield*_tmpFA5=Cyc_Absyn_lookup_field(
_tmpF9F,_tmpF7E);if(_tmpFA5 != 0  && _tmpFA5->width == 0){struct _tuple17 _tmp1644;
return(_tmp1644.f1=(_tmpFA5->tq).real_const,((_tmp1644.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF7D)).f2,_tmp1644)));}return bogus_ans;}_LL7C0: {struct Cyc_Absyn_AggrType_struct*
_tmpFA0=(struct Cyc_Absyn_AggrType_struct*)_tmpF9D;if(_tmpFA0->tag != 12)goto
_LL7C2;else{_tmpFA1=_tmpFA0->f1;_tmpFA2=_tmpFA1.aggr_info;if((_tmpFA2.KnownAggr).tag
!= 2)goto _LL7C2;_tmpFA3=(struct Cyc_Absyn_Aggrdecl**)(_tmpFA2.KnownAggr).val;
_tmpFA4=*_tmpFA3;}}_LL7C1: {struct Cyc_Absyn_Aggrfield*_tmpFA7=Cyc_Absyn_lookup_decl_field(
_tmpFA4,_tmpF7E);if(_tmpFA7 != 0  && _tmpFA7->width == 0){struct _tuple17 _tmp1645;
return(_tmp1645.f1=(_tmpFA7->tq).real_const,((_tmp1645.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF7D)).f2,_tmp1645)));}return bogus_ans;}_LL7C2:;_LL7C3: return bogus_ans;
_LL7BD:;};_LL79E: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF80=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF78;if(_tmpF80->tag != 23)goto _LL7A0;else{_tmpF81=_tmpF80->f1;_tmpF82=_tmpF80->f2;
_tmpF83=_tmpF80->f3;}}_LL79F: if(_tmpF83)return bogus_ans;{void*_tmpFA9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF81->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpFAB;void*_tmpFAC;struct Cyc_Absyn_PtrAtts _tmpFAD;void*_tmpFAE;_LL7C5: {struct
Cyc_Absyn_PointerType_struct*_tmpFAA=(struct Cyc_Absyn_PointerType_struct*)
_tmpFA9;if(_tmpFAA->tag != 5)goto _LL7C7;else{_tmpFAB=_tmpFAA->f1;_tmpFAC=_tmpFAB.elt_typ;
_tmpFAD=_tmpFAB.ptr_atts;_tmpFAE=_tmpFAD.rgn;}}_LL7C6: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpFAF=Cyc_Tcutil_compress(_tmpFAC);struct Cyc_List_List*_tmpFB1;
struct Cyc_Absyn_AggrInfo _tmpFB3;union Cyc_Absyn_AggrInfoU _tmpFB4;struct Cyc_Absyn_Aggrdecl**
_tmpFB5;struct Cyc_Absyn_Aggrdecl*_tmpFB6;_LL7CA: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpFB0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFAF;if(_tmpFB0->tag != 13)goto
_LL7CC;else{_tmpFB1=_tmpFB0->f2;}}_LL7CB: finfo=Cyc_Absyn_lookup_field(_tmpFB1,
_tmpF82);goto _LL7C9;_LL7CC: {struct Cyc_Absyn_AggrType_struct*_tmpFB2=(struct Cyc_Absyn_AggrType_struct*)
_tmpFAF;if(_tmpFB2->tag != 12)goto _LL7CE;else{_tmpFB3=_tmpFB2->f1;_tmpFB4=_tmpFB3.aggr_info;
if((_tmpFB4.KnownAggr).tag != 2)goto _LL7CE;_tmpFB5=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFB4.KnownAggr).val;_tmpFB6=*_tmpFB5;}}_LL7CD: finfo=Cyc_Absyn_lookup_decl_field(
_tmpFB6,_tmpF82);goto _LL7C9;_LL7CE:;_LL7CF: return bogus_ans;_LL7C9:;}if(finfo != 0
 && finfo->width == 0){struct _tuple17 _tmp1646;return(_tmp1646.f1=(finfo->tq).real_const,((
_tmp1646.f2=_tmpFAE,_tmp1646)));}return bogus_ans;}_LL7C7:;_LL7C8: return bogus_ans;
_LL7C4:;};_LL7A0: {struct Cyc_Absyn_Deref_e_struct*_tmpF84=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF78;if(_tmpF84->tag != 21)goto _LL7A2;else{_tmpF85=_tmpF84->f1;}}_LL7A1: {void*
_tmpFB8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF85->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpFBA;struct Cyc_Absyn_Tqual _tmpFBB;struct Cyc_Absyn_PtrAtts
_tmpFBC;void*_tmpFBD;_LL7D1: {struct Cyc_Absyn_PointerType_struct*_tmpFB9=(struct
Cyc_Absyn_PointerType_struct*)_tmpFB8;if(_tmpFB9->tag != 5)goto _LL7D3;else{
_tmpFBA=_tmpFB9->f1;_tmpFBB=_tmpFBA.elt_tq;_tmpFBC=_tmpFBA.ptr_atts;_tmpFBD=
_tmpFBC.rgn;}}_LL7D2: {struct _tuple17 _tmp1647;return(_tmp1647.f1=_tmpFBB.real_const,((
_tmp1647.f2=_tmpFBD,_tmp1647)));}_LL7D3:;_LL7D4: return bogus_ans;_LL7D0:;}_LL7A2: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF86=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF78;if(_tmpF86->tag != 24)goto _LL7A4;else{_tmpF87=_tmpF86->f1;_tmpF88=_tmpF86->f2;}}
_LL7A3: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF87->topt))->v);void*_tmpFBF=t;struct Cyc_List_List*_tmpFC1;struct Cyc_Absyn_PtrInfo
_tmpFC3;struct Cyc_Absyn_Tqual _tmpFC4;struct Cyc_Absyn_PtrAtts _tmpFC5;void*_tmpFC6;
struct Cyc_Absyn_ArrayInfo _tmpFC8;struct Cyc_Absyn_Tqual _tmpFC9;_LL7D6: {struct Cyc_Absyn_TupleType_struct*
_tmpFC0=(struct Cyc_Absyn_TupleType_struct*)_tmpFBF;if(_tmpFC0->tag != 11)goto
_LL7D8;else{_tmpFC1=_tmpFC0->f1;}}_LL7D7: {unsigned int _tmpFCB;int _tmpFCC;struct
_tuple13 _tmpFCA=Cyc_Evexp_eval_const_uint_exp(_tmpF88);_tmpFCB=_tmpFCA.f1;
_tmpFCC=_tmpFCA.f2;if(!_tmpFCC)return bogus_ans;{struct _tuple11*_tmpFCD=Cyc_Absyn_lookup_tuple_field(
_tmpFC1,(int)_tmpFCB);if(_tmpFCD != 0){struct _tuple17 _tmp1648;return(_tmp1648.f1=((*
_tmpFCD).f1).real_const,((_tmp1648.f2=(Cyc_Tcutil_addressof_props(te,_tmpF87)).f2,
_tmp1648)));}return bogus_ans;};}_LL7D8: {struct Cyc_Absyn_PointerType_struct*
_tmpFC2=(struct Cyc_Absyn_PointerType_struct*)_tmpFBF;if(_tmpFC2->tag != 5)goto
_LL7DA;else{_tmpFC3=_tmpFC2->f1;_tmpFC4=_tmpFC3.elt_tq;_tmpFC5=_tmpFC3.ptr_atts;
_tmpFC6=_tmpFC5.rgn;}}_LL7D9: {struct _tuple17 _tmp1649;return(_tmp1649.f1=_tmpFC4.real_const,((
_tmp1649.f2=_tmpFC6,_tmp1649)));}_LL7DA: {struct Cyc_Absyn_ArrayType_struct*
_tmpFC7=(struct Cyc_Absyn_ArrayType_struct*)_tmpFBF;if(_tmpFC7->tag != 9)goto
_LL7DC;else{_tmpFC8=_tmpFC7->f1;_tmpFC9=_tmpFC8.tq;}}_LL7DB: {struct _tuple17
_tmp164A;return(_tmp164A.f1=_tmpFC9.real_const,((_tmp164A.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF87)).f2,_tmp164A)));}_LL7DC:;_LL7DD: return bogus_ans;_LL7D5:;}_LL7A4:;
_LL7A5:{const char*_tmp164D;void*_tmp164C;(_tmp164C=0,Cyc_Tcutil_terr(e->loc,((
_tmp164D="unary & applied to non-lvalue",_tag_dyneither(_tmp164D,sizeof(char),30))),
_tag_dyneither(_tmp164C,sizeof(void*),0)));}return bogus_ans;_LL799:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFD4=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFD6;void*_tmpFD7;struct Cyc_Absyn_Tqual
_tmpFD8;union Cyc_Absyn_Constraint*_tmpFD9;_LL7DF: {struct Cyc_Absyn_ArrayType_struct*
_tmpFD5=(struct Cyc_Absyn_ArrayType_struct*)_tmpFD4;if(_tmpFD5->tag != 9)goto
_LL7E1;else{_tmpFD6=_tmpFD5->f1;_tmpFD7=_tmpFD6.elt_type;_tmpFD8=_tmpFD6.tq;
_tmpFD9=_tmpFD6.zero_term;}}_LL7E0: {void*_tmpFDB;struct _tuple17 _tmpFDA=Cyc_Tcutil_addressof_props(
te,e);_tmpFDB=_tmpFDA.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1650;struct Cyc_Absyn_Upper_b_struct*
_tmp164F;return Cyc_Absyn_atb_typ(_tmpFD7,_tmpFDB,_tmpFD8,(void*)((_tmp164F=
_cycalloc(sizeof(*_tmp164F)),((_tmp164F[0]=((_tmp1650.tag=1,((_tmp1650.f1=e,
_tmp1650)))),_tmp164F)))),_tmpFD9);};}_LL7E1:;_LL7E2: return e_typ;_LL7DE:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFDE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFE1;_LL7E4: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFDF=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFDE;if(_tmpFDF->tag != 0)goto
_LL7E6;}_LL7E5: return;_LL7E6: {struct Cyc_Absyn_Upper_b_struct*_tmpFE0=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFDE;if(_tmpFE0->tag != 1)goto _LL7E3;else{_tmpFE1=_tmpFE0->f1;}}_LL7E7: {
unsigned int _tmpFE3;int _tmpFE4;struct _tuple13 _tmpFE2=Cyc_Evexp_eval_const_uint_exp(
_tmpFE1);_tmpFE3=_tmpFE2.f1;_tmpFE4=_tmpFE2.f2;if(_tmpFE4  && _tmpFE3 <= i){const
char*_tmp1655;void*_tmp1654[2];struct Cyc_Int_pa_struct _tmp1653;struct Cyc_Int_pa_struct
_tmp1652;(_tmp1652.tag=1,((_tmp1652.f1=(unsigned long)((int)i),((_tmp1653.tag=1,((
_tmp1653.f1=(unsigned long)((int)_tmpFE3),((_tmp1654[0]=& _tmp1653,((_tmp1654[1]=&
_tmp1652,Cyc_Tcutil_terr(loc,((_tmp1655="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1655,sizeof(char),39))),_tag_dyneither(_tmp1654,sizeof(void*),
2)))))))))))));}return;}_LL7E3:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFE9=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFEB;_LL7E9: {struct Cyc_Absyn_Upper_b_struct*_tmpFEA=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFE9;if(_tmpFEA->tag != 1)goto _LL7EB;else{_tmpFEB=_tmpFEA->f1;}}_LL7EA: {
unsigned int _tmpFED;int _tmpFEE;struct _tuple13 _tmpFEC=Cyc_Evexp_eval_const_uint_exp(
_tmpFEB);_tmpFED=_tmpFEC.f1;_tmpFEE=_tmpFEC.f2;return _tmpFEE  && _tmpFED == 1;}
_LL7EB:;_LL7EC: return 0;_LL7E8:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFEF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFF7;void*_tmpFF8;union Cyc_Absyn_Constraint*
_tmpFF9;struct Cyc_List_List*_tmpFFB;struct Cyc_Absyn_AggrInfo _tmpFFD;union Cyc_Absyn_AggrInfoU
_tmpFFE;struct _tuple4 _tmpFFF;struct Cyc_Absyn_AggrInfo _tmp1001;union Cyc_Absyn_AggrInfoU
_tmp1002;struct Cyc_Absyn_Aggrdecl**_tmp1003;struct Cyc_Absyn_Aggrdecl*_tmp1004;
struct Cyc_List_List*_tmp1005;struct Cyc_List_List*_tmp1007;_LL7EE: {struct Cyc_Absyn_VoidType_struct*
_tmpFF0=(struct Cyc_Absyn_VoidType_struct*)_tmpFEF;if(_tmpFF0->tag != 0)goto _LL7F0;}
_LL7EF: goto _LL7F1;_LL7F0: {struct Cyc_Absyn_IntType_struct*_tmpFF1=(struct Cyc_Absyn_IntType_struct*)
_tmpFEF;if(_tmpFF1->tag != 6)goto _LL7F2;}_LL7F1: goto _LL7F3;_LL7F2: {struct Cyc_Absyn_FloatType_struct*
_tmpFF2=(struct Cyc_Absyn_FloatType_struct*)_tmpFEF;if(_tmpFF2->tag != 7)goto
_LL7F4;}_LL7F3: goto _LL7F5;_LL7F4: {struct Cyc_Absyn_DoubleType_struct*_tmpFF3=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFEF;if(_tmpFF3->tag != 8)goto _LL7F6;}
_LL7F5: return 1;_LL7F6: {struct Cyc_Absyn_EnumType_struct*_tmpFF4=(struct Cyc_Absyn_EnumType_struct*)
_tmpFEF;if(_tmpFF4->tag != 14)goto _LL7F8;}_LL7F7: goto _LL7F9;_LL7F8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFF5=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpFEF;if(_tmpFF5->tag != 15)goto
_LL7FA;}_LL7F9: return 0;_LL7FA: {struct Cyc_Absyn_ArrayType_struct*_tmpFF6=(struct
Cyc_Absyn_ArrayType_struct*)_tmpFEF;if(_tmpFF6->tag != 9)goto _LL7FC;else{_tmpFF7=
_tmpFF6->f1;_tmpFF8=_tmpFF7.elt_type;_tmpFF9=_tmpFF7.zero_term;}}_LL7FB: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFF9) && Cyc_Tcutil_bits_only(
_tmpFF8);_LL7FC: {struct Cyc_Absyn_TupleType_struct*_tmpFFA=(struct Cyc_Absyn_TupleType_struct*)
_tmpFEF;if(_tmpFFA->tag != 11)goto _LL7FE;else{_tmpFFB=_tmpFFA->f1;}}_LL7FD: for(0;
_tmpFFB != 0;_tmpFFB=_tmpFFB->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpFFB->hd)).f2))return 0;}return 1;_LL7FE: {struct Cyc_Absyn_AggrType_struct*
_tmpFFC=(struct Cyc_Absyn_AggrType_struct*)_tmpFEF;if(_tmpFFC->tag != 12)goto
_LL800;else{_tmpFFD=_tmpFFC->f1;_tmpFFE=_tmpFFD.aggr_info;if((_tmpFFE.UnknownAggr).tag
!= 1)goto _LL800;_tmpFFF=(struct _tuple4)(_tmpFFE.UnknownAggr).val;}}_LL7FF: return
0;_LL800: {struct Cyc_Absyn_AggrType_struct*_tmp1000=(struct Cyc_Absyn_AggrType_struct*)
_tmpFEF;if(_tmp1000->tag != 12)goto _LL802;else{_tmp1001=_tmp1000->f1;_tmp1002=
_tmp1001.aggr_info;if((_tmp1002.KnownAggr).tag != 2)goto _LL802;_tmp1003=(struct
Cyc_Absyn_Aggrdecl**)(_tmp1002.KnownAggr).val;_tmp1004=*_tmp1003;_tmp1005=
_tmp1001.targs;}}_LL801: if(_tmp1004->impl == 0)return 0;{struct _RegionHandle
_tmp1008=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1008;_push_region(rgn);{
struct Cyc_List_List*_tmp1009=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp1004->tvs,_tmp1005);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1004->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute(rgn,_tmp1009,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp100A=0;_npop_handler(0);return _tmp100A;}}}{int _tmp100B=1;_npop_handler(0);
return _tmp100B;};};_pop_region(rgn);};_LL802: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1006=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFEF;if(_tmp1006->tag != 13)
goto _LL804;else{_tmp1007=_tmp1006->f2;}}_LL803: for(0;_tmp1007 != 0;_tmp1007=
_tmp1007->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmp1007->hd)->type))
return 0;}return 1;_LL804:;_LL805: return 0;_LL7ED:;}static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp100C=e->r;
struct _tuple2*_tmp1014;void*_tmp1015;struct Cyc_Absyn_Exp*_tmp1017;struct Cyc_Absyn_Exp*
_tmp1018;struct Cyc_Absyn_Exp*_tmp1019;struct Cyc_Absyn_Exp*_tmp101B;struct Cyc_Absyn_Exp*
_tmp101C;struct Cyc_Absyn_Exp*_tmp101E;struct Cyc_Absyn_Exp*_tmp1020;void*_tmp1022;
struct Cyc_Absyn_Exp*_tmp1023;enum Cyc_Absyn_Coercion _tmp1024;void*_tmp1026;
struct Cyc_Absyn_Exp*_tmp1027;struct Cyc_Absyn_Exp*_tmp1029;struct Cyc_Absyn_Exp*
_tmp102B;struct Cyc_Absyn_Exp*_tmp102C;struct Cyc_List_List*_tmp102E;struct Cyc_List_List*
_tmp1030;struct Cyc_List_List*_tmp1032;enum Cyc_Absyn_Primop _tmp1034;struct Cyc_List_List*
_tmp1035;struct Cyc_List_List*_tmp1037;struct Cyc_List_List*_tmp1039;_LL807: {
struct Cyc_Absyn_Const_e_struct*_tmp100D=(struct Cyc_Absyn_Const_e_struct*)
_tmp100C;if(_tmp100D->tag != 0)goto _LL809;}_LL808: goto _LL80A;_LL809: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp100E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp100C;if(_tmp100E->tag != 18)
goto _LL80B;}_LL80A: goto _LL80C;_LL80B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp100F=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp100C;if(_tmp100F->tag != 19)
goto _LL80D;}_LL80C: goto _LL80E;_LL80D: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp1010=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp100C;if(_tmp1010->tag != 20)goto
_LL80F;}_LL80E: goto _LL810;_LL80F: {struct Cyc_Absyn_Enum_e_struct*_tmp1011=(
struct Cyc_Absyn_Enum_e_struct*)_tmp100C;if(_tmp1011->tag != 32)goto _LL811;}_LL810:
goto _LL812;_LL811: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1012=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp100C;if(_tmp1012->tag != 33)goto _LL813;}_LL812: return 1;_LL813: {struct Cyc_Absyn_Var_e_struct*
_tmp1013=(struct Cyc_Absyn_Var_e_struct*)_tmp100C;if(_tmp1013->tag != 1)goto _LL815;
else{_tmp1014=_tmp1013->f1;_tmp1015=(void*)_tmp1013->f2;}}_LL814: {void*_tmp103A=
_tmp1015;struct Cyc_Absyn_Vardecl*_tmp103D;_LL834: {struct Cyc_Absyn_Funname_b_struct*
_tmp103B=(struct Cyc_Absyn_Funname_b_struct*)_tmp103A;if(_tmp103B->tag != 2)goto
_LL836;}_LL835: return 1;_LL836: {struct Cyc_Absyn_Global_b_struct*_tmp103C=(struct
Cyc_Absyn_Global_b_struct*)_tmp103A;if(_tmp103C->tag != 1)goto _LL838;else{
_tmp103D=_tmp103C->f1;}}_LL837: {void*_tmp103F=Cyc_Tcutil_compress(_tmp103D->type);
_LL83D: {struct Cyc_Absyn_ArrayType_struct*_tmp1040=(struct Cyc_Absyn_ArrayType_struct*)
_tmp103F;if(_tmp1040->tag != 9)goto _LL83F;}_LL83E: goto _LL840;_LL83F: {struct Cyc_Absyn_FnType_struct*
_tmp1041=(struct Cyc_Absyn_FnType_struct*)_tmp103F;if(_tmp1041->tag != 10)goto
_LL841;}_LL840: return 1;_LL841:;_LL842: return var_okay;_LL83C:;}_LL838: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp103E=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp103A;if(_tmp103E->tag != 0)
goto _LL83A;}_LL839: return 0;_LL83A:;_LL83B: return var_okay;_LL833:;}_LL815: {
struct Cyc_Absyn_Conditional_e_struct*_tmp1016=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp100C;if(_tmp1016->tag != 6)goto _LL817;else{_tmp1017=_tmp1016->f1;_tmp1018=
_tmp1016->f2;_tmp1019=_tmp1016->f3;}}_LL816: return(Cyc_Tcutil_cnst_exp(te,0,
_tmp1017) && Cyc_Tcutil_cnst_exp(te,0,_tmp1018)) && Cyc_Tcutil_cnst_exp(te,0,
_tmp1019);_LL817: {struct Cyc_Absyn_SeqExp_e_struct*_tmp101A=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp100C;if(_tmp101A->tag != 9)goto _LL819;else{_tmp101B=_tmp101A->f1;_tmp101C=
_tmp101A->f2;}}_LL818: return Cyc_Tcutil_cnst_exp(te,0,_tmp101B) && Cyc_Tcutil_cnst_exp(
te,0,_tmp101C);_LL819: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp101D=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp100C;if(_tmp101D->tag != 13)goto _LL81B;
else{_tmp101E=_tmp101D->f1;}}_LL81A: _tmp1020=_tmp101E;goto _LL81C;_LL81B: {struct
Cyc_Absyn_Instantiate_e_struct*_tmp101F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp100C;if(_tmp101F->tag != 14)goto _LL81D;else{_tmp1020=_tmp101F->f1;}}_LL81C:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1020);_LL81D: {struct Cyc_Absyn_Cast_e_struct*
_tmp1021=(struct Cyc_Absyn_Cast_e_struct*)_tmp100C;if(_tmp1021->tag != 15)goto
_LL81F;else{_tmp1022=(void*)_tmp1021->f1;_tmp1023=_tmp1021->f2;_tmp1024=_tmp1021->f4;
if(_tmp1024 != Cyc_Absyn_No_coercion)goto _LL81F;}}_LL81E: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp1023);_LL81F: {struct Cyc_Absyn_Cast_e_struct*_tmp1025=(struct Cyc_Absyn_Cast_e_struct*)
_tmp100C;if(_tmp1025->tag != 15)goto _LL821;else{_tmp1026=(void*)_tmp1025->f1;
_tmp1027=_tmp1025->f2;}}_LL820: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1027);
_LL821: {struct Cyc_Absyn_Address_e_struct*_tmp1028=(struct Cyc_Absyn_Address_e_struct*)
_tmp100C;if(_tmp1028->tag != 16)goto _LL823;else{_tmp1029=_tmp1028->f1;}}_LL822:
return Cyc_Tcutil_cnst_exp(te,1,_tmp1029);_LL823: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp102A=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp100C;if(_tmp102A->tag != 28)
goto _LL825;else{_tmp102B=_tmp102A->f2;_tmp102C=_tmp102A->f3;}}_LL824: return Cyc_Tcutil_cnst_exp(
te,0,_tmp102B) && Cyc_Tcutil_cnst_exp(te,0,_tmp102C);_LL825: {struct Cyc_Absyn_Array_e_struct*
_tmp102D=(struct Cyc_Absyn_Array_e_struct*)_tmp100C;if(_tmp102D->tag != 27)goto
_LL827;else{_tmp102E=_tmp102D->f1;}}_LL826: _tmp1030=_tmp102E;goto _LL828;_LL827: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp102F=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp100C;if(_tmp102F->tag != 30)goto _LL829;else{_tmp1030=_tmp102F->f2;}}_LL828:
_tmp1032=_tmp1030;goto _LL82A;_LL829: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1031=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp100C;if(_tmp1031->tag != 29)
goto _LL82B;else{_tmp1032=_tmp1031->f3;}}_LL82A: for(0;_tmp1032 != 0;_tmp1032=
_tmp1032->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)_tmp1032->hd)).f2))
return 0;}return 1;_LL82B: {struct Cyc_Absyn_Primop_e_struct*_tmp1033=(struct Cyc_Absyn_Primop_e_struct*)
_tmp100C;if(_tmp1033->tag != 3)goto _LL82D;else{_tmp1034=_tmp1033->f1;_tmp1035=
_tmp1033->f2;}}_LL82C: _tmp1037=_tmp1035;goto _LL82E;_LL82D: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1036=(struct Cyc_Absyn_Tuple_e_struct*)_tmp100C;if(_tmp1036->tag != 25)goto
_LL82F;else{_tmp1037=_tmp1036->f1;}}_LL82E: _tmp1039=_tmp1037;goto _LL830;_LL82F: {
struct Cyc_Absyn_Datatype_e_struct*_tmp1038=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp100C;if(_tmp1038->tag != 31)goto _LL831;else{_tmp1039=_tmp1038->f1;}}_LL830:
for(0;_tmp1039 != 0;_tmp1039=_tmp1039->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp1039->hd))return 0;}return 1;_LL831:;_LL832: return 0;_LL806:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp1042=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1048;
struct Cyc_Absyn_PtrAtts _tmp1049;union Cyc_Absyn_Constraint*_tmp104A;union Cyc_Absyn_Constraint*
_tmp104B;struct Cyc_Absyn_ArrayInfo _tmp104D;void*_tmp104E;struct Cyc_List_List*
_tmp1050;struct Cyc_Absyn_AggrInfo _tmp1052;union Cyc_Absyn_AggrInfoU _tmp1053;
struct Cyc_List_List*_tmp1054;struct Cyc_List_List*_tmp1056;_LL844: {struct Cyc_Absyn_VoidType_struct*
_tmp1043=(struct Cyc_Absyn_VoidType_struct*)_tmp1042;if(_tmp1043->tag != 0)goto
_LL846;}_LL845: goto _LL847;_LL846: {struct Cyc_Absyn_IntType_struct*_tmp1044=(
struct Cyc_Absyn_IntType_struct*)_tmp1042;if(_tmp1044->tag != 6)goto _LL848;}_LL847:
goto _LL849;_LL848: {struct Cyc_Absyn_FloatType_struct*_tmp1045=(struct Cyc_Absyn_FloatType_struct*)
_tmp1042;if(_tmp1045->tag != 7)goto _LL84A;}_LL849: goto _LL84B;_LL84A: {struct Cyc_Absyn_DoubleType_struct*
_tmp1046=(struct Cyc_Absyn_DoubleType_struct*)_tmp1042;if(_tmp1046->tag != 8)goto
_LL84C;}_LL84B: return 1;_LL84C: {struct Cyc_Absyn_PointerType_struct*_tmp1047=(
struct Cyc_Absyn_PointerType_struct*)_tmp1042;if(_tmp1047->tag != 5)goto _LL84E;
else{_tmp1048=_tmp1047->f1;_tmp1049=_tmp1048.ptr_atts;_tmp104A=_tmp1049.nullable;
_tmp104B=_tmp1049.bounds;}}_LL84D: {void*_tmp1059=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp104B);_LL85D: {
struct Cyc_Absyn_DynEither_b_struct*_tmp105A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1059;if(_tmp105A->tag != 0)goto _LL85F;}_LL85E: return 1;_LL85F:;_LL860: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp104A);
_LL85C:;}_LL84E: {struct Cyc_Absyn_ArrayType_struct*_tmp104C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1042;if(_tmp104C->tag != 9)goto _LL850;else{_tmp104D=_tmp104C->f1;_tmp104E=
_tmp104D.elt_type;}}_LL84F: return Cyc_Tcutil_supports_default(_tmp104E);_LL850: {
struct Cyc_Absyn_TupleType_struct*_tmp104F=(struct Cyc_Absyn_TupleType_struct*)
_tmp1042;if(_tmp104F->tag != 11)goto _LL852;else{_tmp1050=_tmp104F->f1;}}_LL851:
for(0;_tmp1050 != 0;_tmp1050=_tmp1050->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple11*)_tmp1050->hd)).f2))return 0;}return 1;_LL852: {struct Cyc_Absyn_AggrType_struct*
_tmp1051=(struct Cyc_Absyn_AggrType_struct*)_tmp1042;if(_tmp1051->tag != 12)goto
_LL854;else{_tmp1052=_tmp1051->f1;_tmp1053=_tmp1052.aggr_info;_tmp1054=_tmp1052.targs;}}
_LL853: {struct Cyc_Absyn_Aggrdecl*_tmp105B=Cyc_Absyn_get_known_aggrdecl(_tmp1053);
if(_tmp105B->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp105B->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp105B->tvs,_tmp1054,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp105B->impl))->fields);}
_LL854: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1055=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1042;if(_tmp1055->tag != 13)goto _LL856;else{_tmp1056=_tmp1055->f2;}}_LL855:
return Cyc_Tcutil_fields_support_default(0,0,_tmp1056);_LL856: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1057=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1042;if(_tmp1057->tag != 15)
goto _LL858;}_LL857: goto _LL859;_LL858: {struct Cyc_Absyn_EnumType_struct*_tmp1058=(
struct Cyc_Absyn_EnumType_struct*)_tmp1042;if(_tmp1058->tag != 14)goto _LL85A;}
_LL859: return 1;_LL85A:;_LL85B: return 0;_LL843:;}void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t){void*_tmp105C=t;struct Cyc_Absyn_Typedefdecl*
_tmp105E;_LL862: {struct Cyc_Absyn_TypedefType_struct*_tmp105D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp105C;if(_tmp105D->tag != 18)goto _LL864;else{_tmp105E=_tmp105D->f3;}}_LL863:
if(_tmp105E != 0){struct Cyc_Absyn_Tqual _tmp105F=_tmp105E->tq;if(((_tmp105F.print_const
 || _tmp105F.q_volatile) || _tmp105F.q_restrict) || _tmp105F.real_const){const
char*_tmp1659;void*_tmp1658[1];struct Cyc_String_pa_struct _tmp1657;(_tmp1657.tag=
0,((_tmp1657.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1658[0]=& _tmp1657,Cyc_Tcutil_warn(loc,((_tmp1659="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp1659,sizeof(char),44))),_tag_dyneither(_tmp1658,sizeof(void*),
1)))))));}}goto _LL861;_LL864:;_LL865: goto _LL861;_LL861:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp1063=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1063;
_push_region(rgn);{struct Cyc_List_List*_tmp1064=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp1064,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp1065=0;_npop_handler(0);return _tmp1065;}}}{int _tmp1066=1;
_npop_handler(0);return _tmp1066;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp1067=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp106C;
struct Cyc_Absyn_PtrAtts _tmp106D;union Cyc_Absyn_Constraint*_tmp106E;union Cyc_Absyn_Constraint*
_tmp106F;_LL867: {struct Cyc_Absyn_IntType_struct*_tmp1068=(struct Cyc_Absyn_IntType_struct*)
_tmp1067;if(_tmp1068->tag != 6)goto _LL869;}_LL868: goto _LL86A;_LL869: {struct Cyc_Absyn_FloatType_struct*
_tmp1069=(struct Cyc_Absyn_FloatType_struct*)_tmp1067;if(_tmp1069->tag != 7)goto
_LL86B;}_LL86A: goto _LL86C;_LL86B: {struct Cyc_Absyn_DoubleType_struct*_tmp106A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp1067;if(_tmp106A->tag != 8)goto _LL86D;}
_LL86C: return 1;_LL86D: {struct Cyc_Absyn_PointerType_struct*_tmp106B=(struct Cyc_Absyn_PointerType_struct*)
_tmp1067;if(_tmp106B->tag != 5)goto _LL86F;else{_tmp106C=_tmp106B->f1;_tmp106D=
_tmp106C.ptr_atts;_tmp106E=_tmp106D.nullable;_tmp106F=_tmp106D.bounds;}}_LL86E: {
void*_tmp1070=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp106F);_LL872: {struct Cyc_Absyn_DynEither_b_struct*
_tmp1071=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1070;if(_tmp1071->tag != 0)goto
_LL874;}_LL873: return 0;_LL874:;_LL875: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp106E);_LL871:;}_LL86F:;_LL870: return 0;_LL866:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp1072=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp1074;void*_tmp1075;struct Cyc_Absyn_FnInfo _tmp1077;struct Cyc_List_List*
_tmp1078;_LL877: {struct Cyc_Absyn_PointerType_struct*_tmp1073=(struct Cyc_Absyn_PointerType_struct*)
_tmp1072;if(_tmp1073->tag != 5)goto _LL879;else{_tmp1074=_tmp1073->f1;_tmp1075=
_tmp1074.elt_typ;}}_LL878: return Cyc_Tcutil_is_noreturn(_tmp1075);_LL879: {struct
Cyc_Absyn_FnType_struct*_tmp1076=(struct Cyc_Absyn_FnType_struct*)_tmp1072;if(
_tmp1076->tag != 10)goto _LL87B;else{_tmp1077=_tmp1076->f1;_tmp1078=_tmp1077.attributes;}}
_LL87A: for(0;_tmp1078 != 0;_tmp1078=_tmp1078->tl){void*_tmp1079=(void*)_tmp1078->hd;
_LL87E: {struct Cyc_Absyn_Noreturn_att_struct*_tmp107A=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1079;if(_tmp107A->tag != 4)goto _LL880;}_LL87F: return 1;_LL880:;_LL881:
continue;_LL87D:;}goto _LL876;_LL87B:;_LL87C: goto _LL876;_LL876:;}return 0;}struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp107B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp107D;struct Cyc_List_List*
_tmp107E;struct Cyc_List_List**_tmp107F;_LL883: {struct Cyc_Absyn_FnType_struct*
_tmp107C=(struct Cyc_Absyn_FnType_struct*)_tmp107B;if(_tmp107C->tag != 10)goto
_LL885;else{_tmp107D=_tmp107C->f1;_tmp107E=_tmp107D.attributes;_tmp107F=(struct
Cyc_List_List**)&(_tmp107C->f1).attributes;}}_LL884: {struct Cyc_List_List*
_tmp1080=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp165A;*_tmp107F=((_tmp165A=_cycalloc(sizeof(*_tmp165A)),((
_tmp165A->hd=(void*)atts->hd,((_tmp165A->tl=*_tmp107F,_tmp165A))))));}else{
struct Cyc_List_List*_tmp165B;_tmp1080=((_tmp165B=_cycalloc(sizeof(*_tmp165B)),((
_tmp165B->hd=(void*)atts->hd,((_tmp165B->tl=_tmp1080,_tmp165B))))));}}return
_tmp1080;}_LL885:;_LL886: {const char*_tmp165E;void*_tmp165D;(_tmp165D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp165E="transfer_fn_type_atts",
_tag_dyneither(_tmp165E,sizeof(char),22))),_tag_dyneither(_tmp165D,sizeof(void*),
0)));}_LL882:;}
