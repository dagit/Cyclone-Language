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
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
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
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*
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
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp1056;void*_tmp1055[2];const char*
_tmp1054;const char*_tmp1053;struct Cyc_String_pa_struct _tmp1052;struct Cyc_String_pa_struct
_tmp1051;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp1051.tag=0,((
_tmp1051.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp1052.tag=0,((_tmp1052.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp1053="const ",_tag_dyneither(_tmp1053,sizeof(char),7)):((
_tmp1054="",_tag_dyneither(_tmp1054,sizeof(char),1)))),((_tmp1055[0]=& _tmp1052,((
_tmp1055[1]=& _tmp1051,Cyc_aprintf(((_tmp1056="%s%s",_tag_dyneither(_tmp1056,
sizeof(char),5))),_tag_dyneither(_tmp1055,sizeof(void*),2))))))))))))));const
char*_tmp105F;void*_tmp105E[2];const char*_tmp105D;const char*_tmp105C;struct Cyc_String_pa_struct
_tmp105B;struct Cyc_String_pa_struct _tmp105A;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp105A.tag=0,((_tmp105A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp105B.tag=0,((
_tmp105B.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp105C="const ",
_tag_dyneither(_tmp105C,sizeof(char),7)):((_tmp105D="",_tag_dyneither(_tmp105D,
sizeof(char),1)))),((_tmp105E[0]=& _tmp105B,((_tmp105E[1]=& _tmp105A,Cyc_aprintf(((
_tmp105F="%s%s",_tag_dyneither(_tmp105F,sizeof(char),5))),_tag_dyneither(
_tmp105E,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp1063;void*
_tmp1062[1];struct Cyc_String_pa_struct _tmp1061;(_tmp1061.tag=0,((_tmp1061.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1062[0]=& _tmp1061,Cyc_fprintf(
Cyc_stderr,((_tmp1063="  %s",_tag_dyneither(_tmp1063,sizeof(char),5))),
_tag_dyneither(_tmp1062,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp1066;void*_tmp1065;(_tmp1065=0,Cyc_fprintf(
Cyc_stderr,((_tmp1066="\n\t",_tag_dyneither(_tmp1066,sizeof(char),3))),
_tag_dyneither(_tmp1065,sizeof(void*),0)));}pos=8;}else{{const char*_tmp1069;void*
_tmp1068;(_tmp1068=0,Cyc_fprintf(Cyc_stderr,((_tmp1069=" ",_tag_dyneither(
_tmp1069,sizeof(char),2))),_tag_dyneither(_tmp1068,sizeof(void*),0)));}++ pos;}{
const char*_tmp106C;void*_tmp106B;(_tmp106B=0,Cyc_fprintf(Cyc_stderr,((_tmp106C="and ",
_tag_dyneither(_tmp106C,sizeof(char),5))),_tag_dyneither(_tmp106B,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp106F;void*_tmp106E;(_tmp106E=0,Cyc_fprintf(Cyc_stderr,((_tmp106F="\n\t",
_tag_dyneither(_tmp106F,sizeof(char),3))),_tag_dyneither(_tmp106E,sizeof(void*),
0)));}pos=8;}{const char*_tmp1073;void*_tmp1072[1];struct Cyc_String_pa_struct
_tmp1071;(_tmp1071.tag=0,((_tmp1071.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp1072[0]=& _tmp1071,Cyc_fprintf(Cyc_stderr,((_tmp1073="%s ",
_tag_dyneither(_tmp1073,sizeof(char),4))),_tag_dyneither(_tmp1072,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp1076;void*_tmp1075;(_tmp1075=0,Cyc_fprintf(Cyc_stderr,((_tmp1076="\n\t",
_tag_dyneither(_tmp1076,sizeof(char),3))),_tag_dyneither(_tmp1075,sizeof(void*),
0)));}pos=8;}{const char*_tmp1079;void*_tmp1078;(_tmp1078=0,Cyc_fprintf(Cyc_stderr,((
_tmp1079="are not compatible. ",_tag_dyneither(_tmp1079,sizeof(char),21))),
_tag_dyneither(_tmp1078,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp107C;void*_tmp107B;(_tmp107B=0,
Cyc_fprintf(Cyc_stderr,((_tmp107C="\n\t",_tag_dyneither(_tmp107C,sizeof(char),3))),
_tag_dyneither(_tmp107B,sizeof(void*),0)));}{const char*_tmp1080;void*_tmp107F[1];
struct Cyc_String_pa_struct _tmp107E;(_tmp107E.tag=0,((_tmp107E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp107F[0]=&
_tmp107E,Cyc_fprintf(Cyc_stderr,((_tmp1080="%s",_tag_dyneither(_tmp1080,sizeof(
char),3))),_tag_dyneither(_tmp107F,sizeof(void*),1)))))));};}{const char*_tmp1083;
void*_tmp1082;(_tmp1082=0,Cyc_fprintf(Cyc_stderr,((_tmp1083="\n",_tag_dyneither(
_tmp1083,sizeof(char),2))),_tag_dyneither(_tmp1082,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp1087;
void*_tmp1086[1];struct Cyc_String_pa_struct _tmp1085;(_tmp1085.tag=0,((_tmp1085.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1086[0]=& _tmp1085,Cyc_fprintf(
Cyc_stderr,((_tmp1087="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp1087,sizeof(char),36))),_tag_dyneither(_tmp1086,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp108A;
struct Cyc_Core_Impossible_struct*_tmp1089;(int)_throw((void*)((_tmp1089=
_cycalloc(sizeof(*_tmp1089)),((_tmp1089[0]=((_tmp108A.tag=Cyc_Core_Impossible,((
_tmp108A.f1=msg,_tmp108A)))),_tmp1089)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp108F;void*_tmp108E[2];struct Cyc_String_pa_struct
_tmp108D;struct Cyc_String_pa_struct _tmp108C;(_tmp108C.tag=0,((_tmp108C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp108D.tag=0,((_tmp108D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp108E[0]=& _tmp108D,((_tmp108E[1]=& _tmp108C,Cyc_fprintf(Cyc_stderr,((
_tmp108F="%s::%s ",_tag_dyneither(_tmp108F,sizeof(char),8))),_tag_dyneither(
_tmp108E,sizeof(void*),2)))))))))))));}{const char*_tmp1092;void*_tmp1091;(
_tmp1091=0,Cyc_fprintf(Cyc_stderr,((_tmp1092="\n",_tag_dyneither(_tmp1092,
sizeof(char),2))),_tag_dyneither(_tmp1091,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp1093;Cyc_Tcutil_warning_segs=((_tmp1093=_cycalloc(
sizeof(*_tmp1093)),((_tmp1093->hd=sg,((_tmp1093->tl=Cyc_Tcutil_warning_segs,
_tmp1093))))));}{struct _dyneither_ptr*_tmp1096;struct Cyc_List_List*_tmp1095;Cyc_Tcutil_warning_msgs=((
_tmp1095=_cycalloc(sizeof(*_tmp1095)),((_tmp1095->hd=((_tmp1096=_cycalloc(
sizeof(*_tmp1096)),((_tmp1096[0]=msg,_tmp1096)))),((_tmp1095->tl=Cyc_Tcutil_warning_msgs,
_tmp1095))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp1099;void*_tmp1098;(_tmp1098=0,Cyc_fprintf(Cyc_stderr,((
_tmp1099="***Warnings***\n",_tag_dyneither(_tmp1099,sizeof(char),16))),
_tag_dyneither(_tmp1098,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp109E;void*_tmp109D[2];struct
Cyc_String_pa_struct _tmp109C;struct Cyc_String_pa_struct _tmp109B;(_tmp109B.tag=0,((
_tmp109B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp109C.tag=
0,((_tmp109C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp109D[0]=&
_tmp109C,((_tmp109D[1]=& _tmp109B,Cyc_fprintf(Cyc_stderr,((_tmp109E="%s: %s\n",
_tag_dyneither(_tmp109E,sizeof(char),8))),_tag_dyneither(_tmp109D,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10A1;void*_tmp10A0;(
_tmp10A0=0,Cyc_fprintf(Cyc_stderr,((_tmp10A1="**************\n",_tag_dyneither(
_tmp10A1,sizeof(char),16))),_tag_dyneither(_tmp10A0,sizeof(void*),0)));}Cyc_fflush((
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10A2;*_tmp44=((_tmp10A2=
_cycalloc(sizeof(*_tmp10A2)),((_tmp10A2[0]=t2,_tmp10A2))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10A3;*_tmp47=((_tmp10A3=_cycalloc(sizeof(*
_tmp10A3)),((_tmp10A3->v=t2,_tmp10A3))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 19)goto _LLB;
else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{void*_tmp4F=
_tmp49->r;void*_tmp51;_LL10: {struct Cyc_Absyn_Valueof_e_struct*_tmp50=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp4F;if(_tmp50->tag != 39)goto _LL12;else{_tmp51=(void*)_tmp50->f1;}}_LL11:
return Cyc_Tcutil_compress(_tmp51);_LL12:;_LL13: return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_struct*
_tmp4A=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp3D;if(_tmp4A->tag != 26)goto _LLD;
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
struct Cyc_Absyn_Unknown_kb_struct _tmp10A6;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10A5;return(void*)((_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((_tmp10A5[0]=((
_tmp10A6.tag=1,((_tmp10A6.f1=0,_tmp10A6)))),_tmp10A5))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp10A9;struct
Cyc_Absyn_Less_kb_struct*_tmp10A8;return(void*)((_tmp10A8=_cycalloc(sizeof(*
_tmp10A8)),((_tmp10A8[0]=((_tmp10A9.tag=2,((_tmp10A9.f1=0,((_tmp10A9.f2=_tmp5C,
_tmp10A9)))))),_tmp10A8))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10AA;
return(_tmp10AA=_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA->name=tv->name,((
_tmp10AA->identity=- 1,((_tmp10AA->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10AA)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple9*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple9*_tmp10AB;return(_tmp10AB=_cycalloc(sizeof(*
_tmp10AB)),((_tmp10AB->f1=_tmp64,((_tmp10AB->f2=_tmp65,((_tmp10AB->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10AB)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg){struct _tuple11 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple11*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple11*_tmp10AC;return(_tmp10AC=_cycalloc(sizeof(*_tmp10AC)),((_tmp10AC->f1=
_tmp6A,((_tmp10AC->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp10AC)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10AD;return(_tmp10AD=_cycalloc(sizeof(*_tmp10AD)),((_tmp10AD->name=f->name,((
_tmp10AD->tq=f->tq,((_tmp10AD->type=Cyc_Tcutil_copy_type(f->type),((_tmp10AD->width=
f->width,((_tmp10AD->attributes=f->attributes,_tmp10AD)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10AE;return(_tmp10AE=_cycalloc(sizeof(*_tmp10AE)),((_tmp10AE->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp10AE->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp10AE)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10AF;return(_tmp10AF=
_cycalloc(sizeof(*_tmp10AF)),((_tmp10AF->name=f->name,((_tmp10AF->tag=f->tag,((
_tmp10AF->loc=f->loc,_tmp10AF)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp10B2;struct Cyc_Absyn_VarType_struct*
_tmp10B1;return(void*)((_tmp10B1=_cycalloc(sizeof(*_tmp10B1)),((_tmp10B1[0]=((
_tmp10B2.tag=2,((_tmp10B2.f1=Cyc_Tcutil_copy_tvar(t),_tmp10B2)))),_tmp10B1))));}
void*Cyc_Tcutil_copy_type(void*t){void*_tmp76=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp7A;struct Cyc_Absyn_DatatypeInfo _tmp7C;union Cyc_Absyn_DatatypeInfoU _tmp7D;
struct Cyc_List_List*_tmp7E;struct Cyc_Absyn_DatatypeFieldInfo _tmp80;union Cyc_Absyn_DatatypeFieldInfoU
_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_PtrInfo _tmp84;void*_tmp85;
struct Cyc_Absyn_Tqual _tmp86;struct Cyc_Absyn_PtrAtts _tmp87;void*_tmp88;union Cyc_Absyn_Constraint*
_tmp89;union Cyc_Absyn_Constraint*_tmp8A;union Cyc_Absyn_Constraint*_tmp8B;struct
Cyc_Absyn_PtrLoc*_tmp8C;struct Cyc_Absyn_ArrayInfo _tmp91;void*_tmp92;struct Cyc_Absyn_Tqual
_tmp93;struct Cyc_Absyn_Exp*_tmp94;union Cyc_Absyn_Constraint*_tmp95;struct Cyc_Position_Segment*
_tmp96;struct Cyc_Absyn_FnInfo _tmp98;struct Cyc_List_List*_tmp99;struct Cyc_Core_Opt*
_tmp9A;void*_tmp9B;struct Cyc_List_List*_tmp9C;int _tmp9D;struct Cyc_Absyn_VarargInfo*
_tmp9E;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmpA0;struct Cyc_List_List*
_tmpA2;struct Cyc_Absyn_AggrInfo _tmpA4;union Cyc_Absyn_AggrInfoU _tmpA5;struct
_tuple4 _tmpA6;enum Cyc_Absyn_AggrKind _tmpA7;struct _tuple2*_tmpA8;struct Cyc_Core_Opt*
_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_AggrInfo _tmpAC;union Cyc_Absyn_AggrInfoU
_tmpAD;struct Cyc_Absyn_Aggrdecl**_tmpAE;struct Cyc_List_List*_tmpAF;enum Cyc_Absyn_AggrKind
_tmpB1;struct Cyc_List_List*_tmpB2;struct _tuple2*_tmpB4;struct Cyc_Absyn_Enumdecl*
_tmpB5;struct Cyc_List_List*_tmpB7;void*_tmpB9;struct Cyc_Absyn_Exp*_tmpBB;void*
_tmpBD;void*_tmpBF;void*_tmpC0;struct _tuple2*_tmpC2;struct Cyc_List_List*_tmpC3;
struct Cyc_Absyn_Typedefdecl*_tmpC4;void*_tmpC8;struct Cyc_List_List*_tmpCA;void*
_tmpCC;struct Cyc_Absyn_TypeDecl*_tmpCE;struct Cyc_Absyn_TypeDecl _tmpCF;void*
_tmpD0;struct Cyc_Absyn_Aggrdecl*_tmpD2;struct Cyc_Absyn_TypeDecl*_tmpD4;struct Cyc_Absyn_TypeDecl
_tmpD5;void*_tmpD6;struct Cyc_Absyn_Enumdecl*_tmpD8;struct Cyc_Absyn_TypeDecl*
_tmpDA;struct Cyc_Absyn_TypeDecl _tmpDB;void*_tmpDC;struct Cyc_Absyn_Datatypedecl*
_tmpDE;_LL23: {struct Cyc_Absyn_VoidType_struct*_tmp77=(struct Cyc_Absyn_VoidType_struct*)
_tmp76;if(_tmp77->tag != 0)goto _LL25;}_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Evar_struct*
_tmp78=(struct Cyc_Absyn_Evar_struct*)_tmp76;if(_tmp78->tag != 1)goto _LL27;}_LL26:
return t;_LL27: {struct Cyc_Absyn_VarType_struct*_tmp79=(struct Cyc_Absyn_VarType_struct*)
_tmp76;if(_tmp79->tag != 2)goto _LL29;else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct
_tmp10B5;struct Cyc_Absyn_VarType_struct*_tmp10B4;return(void*)((_tmp10B4=
_cycalloc(sizeof(*_tmp10B4)),((_tmp10B4[0]=((_tmp10B5.tag=2,((_tmp10B5.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10B5)))),_tmp10B4))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10BB;struct Cyc_Absyn_DatatypeInfo _tmp10BA;
struct Cyc_Absyn_DatatypeType_struct*_tmp10B9;return(void*)((_tmp10B9=_cycalloc(
sizeof(*_tmp10B9)),((_tmp10B9[0]=((_tmp10BB.tag=3,((_tmp10BB.f1=((_tmp10BA.datatype_info=
_tmp7D,((_tmp10BA.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp10BA)))),_tmp10BB)))),
_tmp10B9))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10C1;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10C0;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10BF;return(void*)((_tmp10BF=_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=((
_tmp10C1.tag=4,((_tmp10C1.f1=((_tmp10C0.field_info=_tmp81,((_tmp10C0.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp10C0)))),_tmp10C1)))),_tmp10BF))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE7=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpE8=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpE9=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEA=_tmp86;union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpEC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp10CB;struct Cyc_Absyn_PtrAtts
_tmp10CA;struct Cyc_Absyn_PtrInfo _tmp10C9;struct Cyc_Absyn_PointerType_struct*
_tmp10C8;return(void*)((_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8[0]=((
_tmp10CB.tag=5,((_tmp10CB.f1=((_tmp10C9.elt_typ=_tmpE7,((_tmp10C9.elt_tq=_tmpEA,((
_tmp10C9.ptr_atts=((_tmp10CA.rgn=_tmpE8,((_tmp10CA.nullable=_tmpE9,((_tmp10CA.bounds=
_tmpEB,((_tmp10CA.zero_term=_tmpEC,((_tmp10CA.ptrloc=_tmp8C,_tmp10CA)))))))))),
_tmp10C9)))))),_tmp10CB)))),_tmp10C8))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_DoubleType_struct*
_tmp8F=(struct Cyc_Absyn_DoubleType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;}
_LL34: return t;_LL35: {struct Cyc_Absyn_ArrayType_struct*_tmp90=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76;if(_tmp90->tag != 9)goto _LL37;else{_tmp91=_tmp90->f1;_tmp92=_tmp91.elt_type;
_tmp93=_tmp91.tq;_tmp94=_tmp91.num_elts;_tmp95=_tmp91.zero_term;_tmp96=_tmp91.zt_loc;}}
_LL36: {struct Cyc_Absyn_ArrayType_struct _tmp10D1;struct Cyc_Absyn_ArrayInfo
_tmp10D0;struct Cyc_Absyn_ArrayType_struct*_tmp10CF;return(void*)((_tmp10CF=
_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF[0]=((_tmp10D1.tag=9,((_tmp10D1.f1=((
_tmp10D0.elt_type=Cyc_Tcutil_copy_type(_tmp92),((_tmp10D0.tq=_tmp93,((_tmp10D0.num_elts=
_tmp94,((_tmp10D0.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp95),((_tmp10D0.zt_loc=_tmp96,_tmp10D0)))))))))),
_tmp10D1)))),_tmp10CF))));}_LL37: {struct Cyc_Absyn_FnType_struct*_tmp97=(struct
Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp97->tag != 10)goto _LL39;else{_tmp98=_tmp97->f1;
_tmp99=_tmp98.tvars;_tmp9A=_tmp98.effect;_tmp9B=_tmp98.ret_typ;_tmp9C=_tmp98.args;
_tmp9D=_tmp98.c_varargs;_tmp9E=_tmp98.cyc_varargs;_tmp9F=_tmp98.rgn_po;_tmpA0=
_tmp98.attributes;}}_LL38: {struct Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp99);struct Cyc_Core_Opt*_tmp10D2;struct Cyc_Core_Opt*
_tmpF5=_tmp9A == 0?0:((_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2->v=Cyc_Tcutil_copy_type((
void*)_tmp9A->v),_tmp10D2))));void*_tmpF6=Cyc_Tcutil_copy_type(_tmp9B);struct Cyc_List_List*
_tmpF7=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp9C);int _tmpF8=_tmp9D;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp9E != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp9E;struct Cyc_Absyn_VarargInfo*_tmp10D3;cyc_varargs2=((_tmp10D3=_cycalloc(
sizeof(*_tmp10D3)),((_tmp10D3->name=cv->name,((_tmp10D3->tq=cv->tq,((_tmp10D3->type=
Cyc_Tcutil_copy_type(cv->type),((_tmp10D3->inject=cv->inject,_tmp10D3))))))))));}{
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);
struct Cyc_List_List*_tmpFB=_tmpA0;struct Cyc_Absyn_FnType_struct _tmp10D9;struct
Cyc_Absyn_FnInfo _tmp10D8;struct Cyc_Absyn_FnType_struct*_tmp10D7;return(void*)((
_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7[0]=((_tmp10D9.tag=10,((_tmp10D9.f1=((
_tmp10D8.tvars=_tmpF4,((_tmp10D8.effect=_tmpF5,((_tmp10D8.ret_typ=_tmpF6,((
_tmp10D8.args=_tmpF7,((_tmp10D8.c_varargs=_tmpF8,((_tmp10D8.cyc_varargs=
cyc_varargs2,((_tmp10D8.rgn_po=_tmpFA,((_tmp10D8.attributes=_tmpFB,_tmp10D8)))))))))))))))),
_tmp10D9)))),_tmp10D7))));};}_LL39: {struct Cyc_Absyn_TupleType_struct*_tmpA1=(
struct Cyc_Absyn_TupleType_struct*)_tmp76;if(_tmpA1->tag != 11)goto _LL3B;else{
_tmpA2=_tmpA1->f1;}}_LL3A: {struct Cyc_Absyn_TupleType_struct _tmp10DC;struct Cyc_Absyn_TupleType_struct*
_tmp10DB;return(void*)((_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB[0]=((
_tmp10DC.tag=11,((_tmp10DC.f1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA2),
_tmp10DC)))),_tmp10DB))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA3=(
struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpA3->tag != 12)goto _LL3D;else{
_tmpA4=_tmpA3->f1;_tmpA5=_tmpA4.aggr_info;if((_tmpA5.UnknownAggr).tag != 1)goto
_LL3D;_tmpA6=(struct _tuple4)(_tmpA5.UnknownAggr).val;_tmpA7=_tmpA6.f1;_tmpA8=
_tmpA6.f2;_tmpA9=_tmpA6.f3;_tmpAA=_tmpA4.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmp10E2;struct Cyc_Absyn_AggrInfo _tmp10E1;struct Cyc_Absyn_AggrType_struct*
_tmp10E0;return(void*)((_tmp10E0=_cycalloc(sizeof(*_tmp10E0)),((_tmp10E0[0]=((
_tmp10E2.tag=12,((_tmp10E2.f1=((_tmp10E1.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA7,
_tmpA8,_tmpA9),((_tmp10E1.targs=Cyc_Tcutil_copy_types(_tmpAA),_tmp10E1)))),
_tmp10E2)))),_tmp10E0))));}_LL3D: {struct Cyc_Absyn_AggrType_struct*_tmpAB=(
struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAB->tag != 12)goto _LL3F;else{
_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;if((_tmpAD.KnownAggr).tag != 2)goto
_LL3F;_tmpAE=(struct Cyc_Absyn_Aggrdecl**)(_tmpAD.KnownAggr).val;_tmpAF=_tmpAC.targs;}}
_LL3E: {struct Cyc_Absyn_AggrType_struct _tmp10E8;struct Cyc_Absyn_AggrInfo _tmp10E7;
struct Cyc_Absyn_AggrType_struct*_tmp10E6;return(void*)((_tmp10E6=_cycalloc(
sizeof(*_tmp10E6)),((_tmp10E6[0]=((_tmp10E8.tag=12,((_tmp10E8.f1=((_tmp10E7.aggr_info=
Cyc_Absyn_KnownAggr(_tmpAE),((_tmp10E7.targs=Cyc_Tcutil_copy_types(_tmpAF),
_tmp10E7)))),_tmp10E8)))),_tmp10E6))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB0->tag != 13)goto
_LL41;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}_LL40: {struct Cyc_Absyn_AnonAggrType_struct
_tmp10EB;struct Cyc_Absyn_AnonAggrType_struct*_tmp10EA;return(void*)((_tmp10EA=
_cycalloc(sizeof(*_tmp10EA)),((_tmp10EA[0]=((_tmp10EB.tag=13,((_tmp10EB.f1=
_tmpB1,((_tmp10EB.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB2),_tmp10EB)))))),_tmp10EA))));}_LL41: {struct Cyc_Absyn_EnumType_struct*
_tmpB3=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB3->tag != 14)goto _LL43;
else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL42: {struct Cyc_Absyn_EnumType_struct
_tmp10EE;struct Cyc_Absyn_EnumType_struct*_tmp10ED;return(void*)((_tmp10ED=
_cycalloc(sizeof(*_tmp10ED)),((_tmp10ED[0]=((_tmp10EE.tag=14,((_tmp10EE.f1=
_tmpB4,((_tmp10EE.f2=_tmpB5,_tmp10EE)))))),_tmp10ED))));}_LL43: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB6->tag != 15)goto
_LL45;else{_tmpB7=_tmpB6->f1;}}_LL44: {struct Cyc_Absyn_AnonEnumType_struct
_tmp10F1;struct Cyc_Absyn_AnonEnumType_struct*_tmp10F0;return(void*)((_tmp10F0=
_cycalloc(sizeof(*_tmp10F0)),((_tmp10F0[0]=((_tmp10F1.tag=15,((_tmp10F1.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB7),_tmp10F1)))),
_tmp10F0))));}_LL45: {struct Cyc_Absyn_TagType_struct*_tmpB8=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB8->tag != 20)goto _LL47;else{_tmpB9=(void*)_tmpB8->f1;}}_LL46: {
struct Cyc_Absyn_TagType_struct _tmp10F4;struct Cyc_Absyn_TagType_struct*_tmp10F3;
return(void*)((_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((_tmp10F3[0]=((_tmp10F4.tag=
20,((_tmp10F4.f1=(void*)Cyc_Tcutil_copy_type(_tmpB9),_tmp10F4)))),_tmp10F3))));}
_LL47: {struct Cyc_Absyn_ValueofType_struct*_tmpBA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBA->tag != 19)goto _LL49;else{_tmpBB=_tmpBA->f1;}}_LL48: {struct Cyc_Absyn_ValueofType_struct
_tmp10F7;struct Cyc_Absyn_ValueofType_struct*_tmp10F6;return(void*)((_tmp10F6=
_cycalloc(sizeof(*_tmp10F6)),((_tmp10F6[0]=((_tmp10F7.tag=19,((_tmp10F7.f1=
_tmpBB,_tmp10F7)))),_tmp10F6))));}_LL49: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBC->tag != 16)goto
_LL4B;else{_tmpBD=(void*)_tmpBC->f1;}}_LL4A: {struct Cyc_Absyn_RgnHandleType_struct
_tmp10FA;struct Cyc_Absyn_RgnHandleType_struct*_tmp10F9;return(void*)((_tmp10F9=
_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9[0]=((_tmp10FA.tag=16,((_tmp10FA.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBD),_tmp10FA)))),_tmp10F9))));}_LL4B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBE=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBE->tag != 17)goto _LL4D;
else{_tmpBF=(void*)_tmpBE->f1;_tmpC0=(void*)_tmpBE->f2;}}_LL4C: {struct Cyc_Absyn_DynRgnType_struct
_tmp10FD;struct Cyc_Absyn_DynRgnType_struct*_tmp10FC;return(void*)((_tmp10FC=
_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((_tmp10FD.tag=17,((_tmp10FD.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBF),((_tmp10FD.f2=(void*)Cyc_Tcutil_copy_type(_tmpC0),
_tmp10FD)))))),_tmp10FC))));}_LL4D: {struct Cyc_Absyn_TypedefType_struct*_tmpC1=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC1->tag != 18)goto _LL4F;else{
_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;_tmpC4=_tmpC1->f3;}}_LL4E: {struct Cyc_Absyn_TypedefType_struct
_tmp1100;struct Cyc_Absyn_TypedefType_struct*_tmp10FF;return(void*)((_tmp10FF=
_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF[0]=((_tmp1100.tag=18,((_tmp1100.f1=
_tmpC2,((_tmp1100.f2=Cyc_Tcutil_copy_types(_tmpC3),((_tmp1100.f3=_tmpC4,((
_tmp1100.f4=0,_tmp1100)))))))))),_tmp10FF))));}_LL4F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC5=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC5->tag != 22)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_struct*_tmpC6=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp76;if(_tmpC6->tag != 21)goto _LL53;}_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_struct*
_tmpC7=(struct Cyc_Absyn_AccessEff_struct*)_tmp76;if(_tmpC7->tag != 23)goto _LL55;
else{_tmpC8=(void*)_tmpC7->f1;}}_LL54: {struct Cyc_Absyn_AccessEff_struct _tmp1103;
struct Cyc_Absyn_AccessEff_struct*_tmp1102;return(void*)((_tmp1102=_cycalloc(
sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1103.tag=23,((_tmp1103.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC8),_tmp1103)))),_tmp1102))));}_LL55: {struct Cyc_Absyn_JoinEff_struct*_tmpC9=(
struct Cyc_Absyn_JoinEff_struct*)_tmp76;if(_tmpC9->tag != 24)goto _LL57;else{_tmpCA=
_tmpC9->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_struct _tmp1106;struct Cyc_Absyn_JoinEff_struct*
_tmp1105;return(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((
_tmp1106.tag=24,((_tmp1106.f1=Cyc_Tcutil_copy_types(_tmpCA),_tmp1106)))),
_tmp1105))));}_LL57: {struct Cyc_Absyn_RgnsEff_struct*_tmpCB=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp76;if(_tmpCB->tag != 25)goto _LL59;else{_tmpCC=(void*)_tmpCB->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_struct _tmp1109;struct Cyc_Absyn_RgnsEff_struct*_tmp1108;
return(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=
25,((_tmp1109.f1=(void*)Cyc_Tcutil_copy_type(_tmpCC),_tmp1109)))),_tmp1108))));}
_LL59: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCD=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCD->tag != 26)goto _LL5B;else{_tmpCE=_tmpCD->f1;_tmpCF=*_tmpCE;
_tmpD0=_tmpCF.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD1=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD0;if(_tmpD1->tag != 0)goto _LL5B;else{_tmpD2=_tmpD1->f1;}};}}_LL5A: {struct
Cyc_List_List*_tmp11E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);struct Cyc_Absyn_AggrType_struct
_tmp110F;struct Cyc_Absyn_AggrInfo _tmp110E;struct Cyc_Absyn_AggrType_struct*
_tmp110D;return(void*)((_tmp110D=_cycalloc(sizeof(*_tmp110D)),((_tmp110D[0]=((
_tmp110F.tag=12,((_tmp110F.f1=((_tmp110E.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD2->kind,
_tmpD2->name,0),((_tmp110E.targs=_tmp11E,_tmp110E)))),_tmp110F)))),_tmp110D))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD3=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD3->tag != 26)goto _LL5D;else{_tmpD4=_tmpD3->f1;_tmpD5=*_tmpD4;
_tmpD6=_tmpD5.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD7=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD6;if(_tmpD7->tag != 1)goto _LL5D;else{_tmpD8=_tmpD7->f1;}};}}_LL5C: {struct
Cyc_Absyn_EnumType_struct _tmp1112;struct Cyc_Absyn_EnumType_struct*_tmp1111;
return(void*)((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111[0]=((_tmp1112.tag=
14,((_tmp1112.f1=_tmpD8->name,((_tmp1112.f2=0,_tmp1112)))))),_tmp1111))));}_LL5D: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpD9=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD9->tag != 26)goto _LL22;else{_tmpDA=_tmpD9->f1;_tmpDB=*_tmpDA;
_tmpDC=_tmpDB.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDD=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDC;if(_tmpDD->tag != 2)goto _LL22;else{_tmpDE=_tmpDD->f1;}};}}_LL5E: {struct
Cyc_List_List*_tmp124=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDE->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp111C;struct Cyc_Absyn_UnknownDatatypeInfo _tmp111B;struct Cyc_Absyn_DatatypeInfo
_tmp111A;struct Cyc_Absyn_DatatypeType_struct*_tmp1119;return(void*)((_tmp1119=
_cycalloc(sizeof(*_tmp1119)),((_tmp1119[0]=((_tmp111C.tag=3,((_tmp111C.f1=((
_tmp111A.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp111B.name=_tmpDE->name,((
_tmp111B.is_extensible=0,_tmp111B))))),((_tmp111A.targs=_tmp124,_tmp111A)))),
_tmp111C)))),_tmp1119))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp129=d;struct Cyc_Absyn_Exp*_tmp12B;struct _dyneither_ptr*_tmp12D;_LL60: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp129;if(_tmp12A->tag != 0)goto _LL62;else{_tmp12B=_tmp12A->f1;}}_LL61: {struct
Cyc_Absyn_ArrayElement_struct _tmp111F;struct Cyc_Absyn_ArrayElement_struct*
_tmp111E;return(void*)((_tmp111E=_cycalloc(sizeof(*_tmp111E)),((_tmp111E[0]=((
_tmp111F.tag=0,((_tmp111F.f1=Cyc_Tcutil_deep_copy_exp(_tmp12B),_tmp111F)))),
_tmp111E))));}_LL62: {struct Cyc_Absyn_FieldName_struct*_tmp12C=(struct Cyc_Absyn_FieldName_struct*)
_tmp129;if(_tmp12C->tag != 1)goto _LL5F;else{_tmp12D=_tmp12C->f1;}}_LL63: return d;
_LL5F:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e){struct _tuple18*_tmp1120;
return(_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp1120->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1120)))));}
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){void*_tmp131=
e->r;union Cyc_Absyn_Cnst _tmp133;struct _tuple2*_tmp135;void*_tmp136;struct _tuple2*
_tmp138;enum Cyc_Absyn_Primop _tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_Absyn_Exp*
_tmp13D;struct Cyc_Core_Opt*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*
_tmp141;enum Cyc_Absyn_Incrementor _tmp142;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*
_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*
_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*
_tmp14E;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_List_List*
_tmp152;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_Absyn_VarargCallInfo*
_tmp156;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*
_tmp15C;struct Cyc_List_List*_tmp15D;void*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;int
_tmp161;enum Cyc_Absyn_Coercion _tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*
_tmp166;struct Cyc_Absyn_Exp*_tmp167;void*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;
void*_tmp16D;void*_tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*
_tmp172;struct _dyneither_ptr*_tmp173;int _tmp174;int _tmp175;struct Cyc_Absyn_Exp*
_tmp177;struct _dyneither_ptr*_tmp178;int _tmp179;int _tmp17A;struct Cyc_Absyn_Exp*
_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_List_List*_tmp17F;struct _tuple9*
_tmp181;struct _tuple9 _tmp182;struct Cyc_Core_Opt*_tmp183;struct Cyc_Absyn_Tqual
_tmp184;void*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_List_List*_tmp188;
struct Cyc_Absyn_Vardecl*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Absyn_Exp*
_tmp18C;int _tmp18D;struct _tuple2*_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_List_List*
_tmp191;struct Cyc_Absyn_Aggrdecl*_tmp192;void*_tmp194;struct Cyc_List_List*
_tmp195;struct Cyc_List_List*_tmp197;struct Cyc_Absyn_Datatypedecl*_tmp198;struct
Cyc_Absyn_Datatypefield*_tmp199;struct _tuple2*_tmp19B;struct Cyc_Absyn_Enumdecl*
_tmp19C;struct Cyc_Absyn_Enumfield*_tmp19D;struct _tuple2*_tmp19F;void*_tmp1A0;
struct Cyc_Absyn_Enumfield*_tmp1A1;struct Cyc_Absyn_MallocInfo _tmp1A3;int _tmp1A4;
struct Cyc_Absyn_Exp*_tmp1A5;void**_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;int _tmp1A8;
struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Core_Opt*
_tmp1AD;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Exp*_tmp1B1;struct
_dyneither_ptr*_tmp1B2;void*_tmp1B4;int _tmp1B6;struct _dyneither_ptr _tmp1B7;_LL65: {
struct Cyc_Absyn_Const_e_struct*_tmp132=(struct Cyc_Absyn_Const_e_struct*)_tmp131;
if(_tmp132->tag != 0)goto _LL67;else{_tmp133=_tmp132->f1;}}_LL66: return Cyc_Absyn_const_exp(
_tmp133,e->loc);_LL67: {struct Cyc_Absyn_Var_e_struct*_tmp134=(struct Cyc_Absyn_Var_e_struct*)
_tmp131;if(_tmp134->tag != 1)goto _LL69;else{_tmp135=_tmp134->f1;_tmp136=(void*)
_tmp134->f2;}}_LL68: return Cyc_Absyn_varb_exp(_tmp135,_tmp136,e->loc);_LL69: {
struct Cyc_Absyn_UnknownId_e_struct*_tmp137=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp131;if(_tmp137->tag != 2)goto _LL6B;else{_tmp138=_tmp137->f1;}}_LL6A: return Cyc_Absyn_unknownid_exp(
_tmp138,e->loc);_LL6B: {struct Cyc_Absyn_Primop_e_struct*_tmp139=(struct Cyc_Absyn_Primop_e_struct*)
_tmp131;if(_tmp139->tag != 3)goto _LL6D;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;}}
_LL6C: return Cyc_Absyn_primop_exp(_tmp13A,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp13B),e->loc);_LL6D: {struct Cyc_Absyn_AssignOp_e_struct*_tmp13C=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp131;if(_tmp13C->tag != 4)goto _LL6F;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;
_tmp13F=_tmp13C->f3;}}_LL6E: {struct Cyc_Core_Opt*_tmp1121;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13D),(unsigned int)_tmp13E?(_tmp1121=
_cycalloc_atomic(sizeof(*_tmp1121)),((_tmp1121->v=(void*)((enum Cyc_Absyn_Primop)
_tmp13E->v),_tmp1121))): 0,Cyc_Tcutil_deep_copy_exp(_tmp13F),e->loc);}_LL6F: {
struct Cyc_Absyn_Increment_e_struct*_tmp140=(struct Cyc_Absyn_Increment_e_struct*)
_tmp131;if(_tmp140->tag != 5)goto _LL71;else{_tmp141=_tmp140->f1;_tmp142=_tmp140->f2;}}
_LL70: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp141),_tmp142,e->loc);
_LL71: {struct Cyc_Absyn_Conditional_e_struct*_tmp143=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp131;if(_tmp143->tag != 6)goto _LL73;else{_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;
_tmp146=_tmp143->f3;}}_LL72: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp144),Cyc_Tcutil_deep_copy_exp(_tmp145),Cyc_Tcutil_deep_copy_exp(_tmp146),e->loc);
_LL73: {struct Cyc_Absyn_And_e_struct*_tmp147=(struct Cyc_Absyn_And_e_struct*)
_tmp131;if(_tmp147->tag != 7)goto _LL75;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}
_LL74: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp148),Cyc_Tcutil_deep_copy_exp(
_tmp149),e->loc);_LL75: {struct Cyc_Absyn_Or_e_struct*_tmp14A=(struct Cyc_Absyn_Or_e_struct*)
_tmp131;if(_tmp14A->tag != 8)goto _LL77;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;}}
_LL76: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp14B),Cyc_Tcutil_deep_copy_exp(
_tmp14C),e->loc);_LL77: {struct Cyc_Absyn_SeqExp_e_struct*_tmp14D=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp131;if(_tmp14D->tag != 9)goto _LL79;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}
_LL78: return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp14E),Cyc_Tcutil_deep_copy_exp(
_tmp14F),e->loc);_LL79: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp150=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp131;if(_tmp150->tag != 10)goto _LL7B;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}
_LL7A: return Cyc_Absyn_unknowncall_exp(Cyc_Tcutil_deep_copy_exp(_tmp151),((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp152),e->loc);_LL7B: {struct Cyc_Absyn_FnCall_e_struct*
_tmp153=(struct Cyc_Absyn_FnCall_e_struct*)_tmp131;if(_tmp153->tag != 11)goto _LL7D;
else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;_tmp156=_tmp153->f3;}}_LL7C: {
struct Cyc_Absyn_VarargCallInfo*_tmp1B9=_tmp156;struct Cyc_Absyn_VarargCallInfo
_tmp1BA;int _tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_VarargInfo*
_tmp1BD;_LLB8: if(_tmp1B9 == 0)goto _LLBA;_tmp1BA=*_tmp1B9;_tmp1BB=_tmp1BA.num_varargs;
_tmp1BC=_tmp1BA.injectors;_tmp1BD=_tmp1BA.vai;_LLB9: {struct Cyc_Absyn_VarargInfo
_tmp1BF;struct Cyc_Core_Opt*_tmp1C0;struct Cyc_Absyn_Tqual _tmp1C1;void*_tmp1C2;int
_tmp1C3;struct Cyc_Absyn_VarargInfo*_tmp1BE=_tmp1BD;_tmp1BF=*_tmp1BE;_tmp1C0=
_tmp1BF.name;_tmp1C1=_tmp1BF.tq;_tmp1C2=_tmp1BF.type;_tmp1C3=_tmp1BF.inject;{
struct Cyc_Absyn_FnCall_e_struct _tmp112B;struct Cyc_Absyn_VarargInfo*_tmp112A;
struct Cyc_Absyn_VarargCallInfo*_tmp1129;struct Cyc_Absyn_FnCall_e_struct*_tmp1128;
return Cyc_Absyn_new_exp((void*)((_tmp1128=_cycalloc(sizeof(*_tmp1128)),((
_tmp1128[0]=((_tmp112B.tag=11,((_tmp112B.f1=Cyc_Tcutil_deep_copy_exp(_tmp154),((
_tmp112B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp155),((
_tmp112B.f3=((_tmp1129=_cycalloc(sizeof(*_tmp1129)),((_tmp1129->num_varargs=
_tmp1BB,((_tmp1129->injectors=_tmp1BC,((_tmp1129->vai=((_tmp112A=_cycalloc(
sizeof(*_tmp112A)),((_tmp112A->name=_tmp1C0,((_tmp112A->tq=_tmp1C1,((_tmp112A->type=
Cyc_Tcutil_copy_type(_tmp1C2),((_tmp112A->inject=_tmp1C3,_tmp112A)))))))))),
_tmp1129)))))))),_tmp112B)))))))),_tmp1128)))),e->loc);};}_LLBA:;_LLBB: {struct
Cyc_Absyn_FnCall_e_struct _tmp112E;struct Cyc_Absyn_FnCall_e_struct*_tmp112D;
return Cyc_Absyn_new_exp((void*)((_tmp112D=_cycalloc(sizeof(*_tmp112D)),((
_tmp112D[0]=((_tmp112E.tag=11,((_tmp112E.f1=Cyc_Tcutil_deep_copy_exp(_tmp154),((
_tmp112E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp155),((
_tmp112E.f3=_tmp156,_tmp112E)))))))),_tmp112D)))),e->loc);}_LLB7:;}_LL7D: {
struct Cyc_Absyn_Throw_e_struct*_tmp157=(struct Cyc_Absyn_Throw_e_struct*)_tmp131;
if(_tmp157->tag != 12)goto _LL7F;else{_tmp158=_tmp157->f1;}}_LL7E: return Cyc_Absyn_throw_exp(
Cyc_Tcutil_deep_copy_exp(_tmp158),e->loc);_LL7F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp159=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp131;if(_tmp159->tag != 13)
goto _LL81;else{_tmp15A=_tmp159->f1;}}_LL80: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
_tmp15A),e->loc);_LL81: {struct Cyc_Absyn_Instantiate_e_struct*_tmp15B=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp131;if(_tmp15B->tag != 14)goto _LL83;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;}}
_LL82: return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp15C),((struct
Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp15D),e->loc);_LL83: {struct Cyc_Absyn_Cast_e_struct*_tmp15E=(struct Cyc_Absyn_Cast_e_struct*)
_tmp131;if(_tmp15E->tag != 15)goto _LL85;else{_tmp15F=(void*)_tmp15E->f1;_tmp160=
_tmp15E->f2;_tmp161=_tmp15E->f3;_tmp162=_tmp15E->f4;}}_LL84: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp15F),Cyc_Tcutil_deep_copy_exp(_tmp160),_tmp161,_tmp162,
e->loc);_LL85: {struct Cyc_Absyn_Address_e_struct*_tmp163=(struct Cyc_Absyn_Address_e_struct*)
_tmp131;if(_tmp163->tag != 16)goto _LL87;else{_tmp164=_tmp163->f1;}}_LL86: return
Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp164),e->loc);_LL87: {struct
Cyc_Absyn_New_e_struct*_tmp165=(struct Cyc_Absyn_New_e_struct*)_tmp131;if(_tmp165->tag
!= 17)goto _LL89;else{_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;}}_LL88: {struct Cyc_Absyn_Exp*
eo1=_tmp166;if(_tmp166 != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp166);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp167),e->loc);}_LL89: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp168=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp131;if(_tmp168->tag != 18)goto _LL8B;else{_tmp169=(void*)_tmp168->f1;}}_LL8A:
return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp169),e->loc);_LL8B: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp16A=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp131;if(_tmp16A->tag != 19)goto _LL8D;else{_tmp16B=_tmp16A->f1;}}_LL8C: return
Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp16B),e->loc);_LL8D: {struct
Cyc_Absyn_Offsetof_e_struct*_tmp16C=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp131;
if(_tmp16C->tag != 20)goto _LL8F;else{_tmp16D=(void*)_tmp16C->f1;_tmp16E=(void*)
_tmp16C->f2;}}_LL8E: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp16D),
_tmp16E,e->loc);_LL8F: {struct Cyc_Absyn_Deref_e_struct*_tmp16F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp131;if(_tmp16F->tag != 21)goto _LL91;else{_tmp170=_tmp16F->f1;}}_LL90: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp170),e->loc);_LL91: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp171=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp131;if(_tmp171->tag != 22)goto
_LL93;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;_tmp174=_tmp171->f3;_tmp175=
_tmp171->f4;}}_LL92: {struct Cyc_Absyn_AggrMember_e_struct _tmp1131;struct Cyc_Absyn_AggrMember_e_struct*
_tmp1130;return Cyc_Absyn_new_exp((void*)((_tmp1130=_cycalloc(sizeof(*_tmp1130)),((
_tmp1130[0]=((_tmp1131.tag=22,((_tmp1131.f1=Cyc_Tcutil_deep_copy_exp(_tmp172),((
_tmp1131.f2=_tmp173,((_tmp1131.f3=_tmp174,((_tmp1131.f4=_tmp175,_tmp1131)))))))))),
_tmp1130)))),e->loc);}_LL93: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp176=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp131;if(_tmp176->tag != 23)goto _LL95;else{
_tmp177=_tmp176->f1;_tmp178=_tmp176->f2;_tmp179=_tmp176->f3;_tmp17A=_tmp176->f4;}}
_LL94: {struct Cyc_Absyn_AggrArrow_e_struct _tmp1134;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1133;return Cyc_Absyn_new_exp((void*)((_tmp1133=_cycalloc(sizeof(*_tmp1133)),((
_tmp1133[0]=((_tmp1134.tag=23,((_tmp1134.f1=Cyc_Tcutil_deep_copy_exp(_tmp177),((
_tmp1134.f2=_tmp178,((_tmp1134.f3=_tmp179,((_tmp1134.f4=_tmp17A,_tmp1134)))))))))),
_tmp1133)))),e->loc);}_LL95: {struct Cyc_Absyn_Subscript_e_struct*_tmp17B=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp131;if(_tmp17B->tag != 24)goto _LL97;else{
_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LL96: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp17C),Cyc_Tcutil_deep_copy_exp(_tmp17D),e->loc);
_LL97: {struct Cyc_Absyn_Tuple_e_struct*_tmp17E=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp131;if(_tmp17E->tag != 25)goto _LL99;else{_tmp17F=_tmp17E->f1;}}_LL98: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp17F),e->loc);
_LL99: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp180=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp131;if(_tmp180->tag != 26)goto _LL9B;else{_tmp181=_tmp180->f1;_tmp182=*_tmp181;
_tmp183=_tmp182.f1;_tmp184=_tmp182.f2;_tmp185=_tmp182.f3;_tmp186=_tmp180->f2;}}
_LL9A: {struct Cyc_Core_Opt*vopt1=_tmp183;if(_tmp183 != 0){struct Cyc_Core_Opt*
_tmp1135;vopt1=((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135->v=(struct
_dyneither_ptr*)_tmp183->v,_tmp1135))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp113B;struct _tuple9*_tmp113A;struct Cyc_Absyn_CompoundLit_e_struct*_tmp1139;
return Cyc_Absyn_new_exp((void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((
_tmp1139[0]=((_tmp113B.tag=26,((_tmp113B.f1=((_tmp113A=_cycalloc(sizeof(*
_tmp113A)),((_tmp113A->f1=vopt1,((_tmp113A->f2=_tmp184,((_tmp113A->f3=Cyc_Tcutil_copy_type(
_tmp185),_tmp113A)))))))),((_tmp113B.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp186),_tmp113B)))))),_tmp1139)))),e->loc);};}_LL9B: {struct Cyc_Absyn_Array_e_struct*
_tmp187=(struct Cyc_Absyn_Array_e_struct*)_tmp131;if(_tmp187->tag != 27)goto _LL9D;
else{_tmp188=_tmp187->f1;}}_LL9C: {struct Cyc_Absyn_Array_e_struct _tmp113E;struct
Cyc_Absyn_Array_e_struct*_tmp113D;return Cyc_Absyn_new_exp((void*)((_tmp113D=
_cycalloc(sizeof(*_tmp113D)),((_tmp113D[0]=((_tmp113E.tag=27,((_tmp113E.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp188),_tmp113E)))),_tmp113D)))),e->loc);}
_LL9D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp189=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp131;if(_tmp189->tag != 28)goto _LL9F;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;
_tmp18C=_tmp189->f3;_tmp18D=_tmp189->f4;}}_LL9E: {struct Cyc_Absyn_Comprehension_e_struct
_tmp1141;struct Cyc_Absyn_Comprehension_e_struct*_tmp1140;return Cyc_Absyn_new_exp((
void*)((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140[0]=((_tmp1141.tag=28,((
_tmp1141.f1=_tmp18A,((_tmp1141.f2=Cyc_Tcutil_deep_copy_exp(_tmp18B),((_tmp1141.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18C),((_tmp1141.f4=_tmp18D,_tmp1141)))))))))),
_tmp1140)))),e->loc);}_LL9F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp18E=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp131;if(_tmp18E->tag != 29)goto _LLA1;else{
_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;_tmp191=_tmp18E->f3;_tmp192=_tmp18E->f4;}}
_LLA0: {struct Cyc_Absyn_Aggregate_e_struct _tmp1144;struct Cyc_Absyn_Aggregate_e_struct*
_tmp1143;return Cyc_Absyn_new_exp((void*)((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((
_tmp1143[0]=((_tmp1144.tag=29,((_tmp1144.f1=_tmp18F,((_tmp1144.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp190),((_tmp1144.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp191),((
_tmp1144.f4=_tmp192,_tmp1144)))))))))),_tmp1143)))),e->loc);}_LLA1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp193=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp131;if(_tmp193->tag != 30)goto
_LLA3;else{_tmp194=(void*)_tmp193->f1;_tmp195=_tmp193->f2;}}_LLA2: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp1147;struct Cyc_Absyn_AnonStruct_e_struct*_tmp1146;return Cyc_Absyn_new_exp((
void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1147.tag=30,((
_tmp1147.f1=(void*)Cyc_Tcutil_copy_type(_tmp194),((_tmp1147.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp195),_tmp1147)))))),_tmp1146)))),e->loc);}_LLA3: {struct Cyc_Absyn_Datatype_e_struct*
_tmp196=(struct Cyc_Absyn_Datatype_e_struct*)_tmp131;if(_tmp196->tag != 31)goto
_LLA5;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;_tmp199=_tmp196->f3;}}_LLA4: {
struct Cyc_Absyn_Datatype_e_struct _tmp114A;struct Cyc_Absyn_Datatype_e_struct*
_tmp1149;return Cyc_Absyn_new_exp((void*)((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((
_tmp1149[0]=((_tmp114A.tag=31,((_tmp114A.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp197),((_tmp114A.f2=_tmp198,((_tmp114A.f3=_tmp199,_tmp114A)))))))),_tmp1149)))),
e->loc);}_LLA5: {struct Cyc_Absyn_Enum_e_struct*_tmp19A=(struct Cyc_Absyn_Enum_e_struct*)
_tmp131;if(_tmp19A->tag != 32)goto _LLA7;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;
_tmp19D=_tmp19A->f3;}}_LLA6: return e;_LLA7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp19E=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp131;if(_tmp19E->tag != 33)goto
_LLA9;else{_tmp19F=_tmp19E->f1;_tmp1A0=(void*)_tmp19E->f2;_tmp1A1=_tmp19E->f3;}}
_LLA8: {struct Cyc_Absyn_AnonEnum_e_struct _tmp114D;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp114C;return Cyc_Absyn_new_exp((void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((
_tmp114C[0]=((_tmp114D.tag=33,((_tmp114D.f1=_tmp19F,((_tmp114D.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A0),((_tmp114D.f3=_tmp1A1,_tmp114D)))))))),_tmp114C)))),e->loc);}_LLA9: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A2=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp131;if(_tmp1A2->tag != 34)goto _LLAB;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A3.is_calloc;
_tmp1A5=_tmp1A3.rgn;_tmp1A6=_tmp1A3.elt_type;_tmp1A7=_tmp1A3.num_elts;_tmp1A8=
_tmp1A3.fat_result;}}_LLAA: {struct Cyc_Absyn_Exp*_tmp1DE=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A5;if(_tmp1A5 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A5);{void**t1=_tmp1A6;if(_tmp1A6 != 0){void**_tmp114E;
t1=((_tmp114E=_cycalloc(sizeof(*_tmp114E)),((_tmp114E[0]=Cyc_Tcutil_copy_type(*
_tmp1A6),_tmp114E))));}{struct Cyc_Absyn_Malloc_e_struct _tmp1154;struct Cyc_Absyn_MallocInfo
_tmp1153;struct Cyc_Absyn_Malloc_e_struct*_tmp1152;_tmp1DE->r=(void*)((_tmp1152=
_cycalloc(sizeof(*_tmp1152)),((_tmp1152[0]=((_tmp1154.tag=34,((_tmp1154.f1=((
_tmp1153.is_calloc=_tmp1A4,((_tmp1153.rgn=r1,((_tmp1153.elt_type=t1,((_tmp1153.num_elts=
_tmp1A7,((_tmp1153.fat_result=_tmp1A8,_tmp1153)))))))))),_tmp1154)))),_tmp1152))));}
return _tmp1DE;};}_LLAB: {struct Cyc_Absyn_Swap_e_struct*_tmp1A9=(struct Cyc_Absyn_Swap_e_struct*)
_tmp131;if(_tmp1A9->tag != 35)goto _LLAD;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}
_LLAC: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AA),Cyc_Tcutil_deep_copy_exp(
_tmp1AB),e->loc);_LLAD: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AC=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp131;if(_tmp1AC->tag != 36)goto _LLAF;else{
_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LLAE: {struct Cyc_Core_Opt*nopt1=
_tmp1AD;if(_tmp1AD != 0){struct Cyc_Core_Opt*_tmp1155;nopt1=((_tmp1155=_cycalloc(
sizeof(*_tmp1155)),((_tmp1155->v=(struct _tuple2*)_tmp1AD->v,_tmp1155))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp1158;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1157;return Cyc_Absyn_new_exp((void*)((_tmp1157=_cycalloc(sizeof(*_tmp1157)),((
_tmp1157[0]=((_tmp1158.tag=36,((_tmp1158.f1=nopt1,((_tmp1158.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1AE),_tmp1158)))))),_tmp1157)))),e->loc);};}_LLAF: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1AF=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp131;if(_tmp1AF->tag != 37)goto
_LLB1;}_LLB0: {struct Cyc_Core_Failure_struct _tmp115E;const char*_tmp115D;struct
Cyc_Core_Failure_struct*_tmp115C;(int)_throw((void*)((_tmp115C=_cycalloc(sizeof(*
_tmp115C)),((_tmp115C[0]=((_tmp115E.tag=Cyc_Core_Failure,((_tmp115E.f1=((
_tmp115D="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp115D,
sizeof(char),45))),_tmp115E)))),_tmp115C)))));}_LLB1: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B0=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp131;if(_tmp1B0->tag != 38)goto
_LLB3;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LLB2: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp1161;struct Cyc_Absyn_Tagcheck_e_struct*_tmp1160;return Cyc_Absyn_new_exp((
void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160[0]=((_tmp1161.tag=38,((
_tmp1161.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B1),((_tmp1161.f2=_tmp1B2,_tmp1161)))))),
_tmp1160)))),e->loc);}_LLB3: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B3=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp131;if(_tmp1B3->tag != 39)goto _LLB5;else{_tmp1B4=(
void*)_tmp1B3->f1;}}_LLB4: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp1B4),e->loc);_LLB5: {struct Cyc_Absyn_Asm_e_struct*_tmp1B5=(struct Cyc_Absyn_Asm_e_struct*)
_tmp131;if(_tmp1B5->tag != 40)goto _LL64;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}
_LLB6: return Cyc_Absyn_asm_exp(_tmp1B6,_tmp1B7,e->loc);_LL64:;}struct _tuple19{
enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual
f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,
struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1EC;enum Cyc_Absyn_KindQual
_tmp1ED;enum Cyc_Absyn_AliasQual _tmp1EE;struct Cyc_Absyn_Kind*_tmp1EB=ka1;_tmp1EC=*
_tmp1EB;_tmp1ED=_tmp1EC.kind;_tmp1EE=_tmp1EC.aliasqual;{struct Cyc_Absyn_Kind
_tmp1F0;enum Cyc_Absyn_KindQual _tmp1F1;enum Cyc_Absyn_AliasQual _tmp1F2;struct Cyc_Absyn_Kind*
_tmp1EF=ka2;_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1F0.kind;_tmp1F2=_tmp1F0.aliasqual;if(
_tmp1ED != _tmp1F1){struct _tuple19 _tmp1162;struct _tuple19 _tmp1F4=(_tmp1162.f1=
_tmp1ED,((_tmp1162.f2=_tmp1F1,_tmp1162)));enum Cyc_Absyn_KindQual _tmp1F5;enum 
Cyc_Absyn_KindQual _tmp1F6;enum Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual
_tmp1F8;enum Cyc_Absyn_KindQual _tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;_LLBD:
_tmp1F5=_tmp1F4.f1;if(_tmp1F5 != Cyc_Absyn_BoxKind)goto _LLBF;_tmp1F6=_tmp1F4.f2;
if(_tmp1F6 != Cyc_Absyn_MemKind)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1F7=_tmp1F4.f1;
if(_tmp1F7 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F8=_tmp1F4.f2;if(_tmp1F8 != Cyc_Absyn_AnyKind)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1F9=_tmp1F4.f1;if(_tmp1F9 != Cyc_Absyn_MemKind)
goto _LLC3;_tmp1FA=_tmp1F4.f2;if(_tmp1FA != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto
_LLBC;_LLC3:;_LLC4: return 0;_LLBC:;}if(_tmp1EE != _tmp1F2){struct _tuple20 _tmp1163;
struct _tuple20 _tmp1FC=(_tmp1163.f1=_tmp1EE,((_tmp1163.f2=_tmp1F2,_tmp1163)));
enum Cyc_Absyn_AliasQual _tmp1FD;enum Cyc_Absyn_AliasQual _tmp1FE;enum Cyc_Absyn_AliasQual
_tmp1FF;enum Cyc_Absyn_AliasQual _tmp200;_LLC6: _tmp1FD=_tmp1FC.f1;if(_tmp1FD != 
Cyc_Absyn_Aliasable)goto _LLC8;_tmp1FE=_tmp1FC.f2;if(_tmp1FE != Cyc_Absyn_Top)goto
_LLC8;_LLC7: goto _LLC9;_LLC8: _tmp1FF=_tmp1FC.f1;if(_tmp1FF != Cyc_Absyn_Unique)
goto _LLCA;_tmp200=_tmp1FC.f2;if(_tmp200 != Cyc_Absyn_Top)goto _LLCA;_LLC9: return 1;
_LLCA:;_LLCB: return 0;_LLC5:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp201=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp203;struct Cyc_Absyn_Kind*_tmp205;_LLCD: {
struct Cyc_Absyn_Eq_kb_struct*_tmp202=(struct Cyc_Absyn_Eq_kb_struct*)_tmp201;if(
_tmp202->tag != 0)goto _LLCF;else{_tmp203=_tmp202->f1;}}_LLCE: return _tmp203;_LLCF: {
struct Cyc_Absyn_Less_kb_struct*_tmp204=(struct Cyc_Absyn_Less_kb_struct*)_tmp201;
if(_tmp204->tag != 2)goto _LLD1;else{_tmp205=_tmp204->f2;}}_LLD0: return _tmp205;
_LLD1:;_LLD2:{struct Cyc_Absyn_Less_kb_struct _tmp1166;struct Cyc_Absyn_Less_kb_struct*
_tmp1165;tv->kind=(void*)((_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165[0]=((
_tmp1166.tag=2,((_tmp1166.f1=0,((_tmp1166.f2=def,_tmp1166)))))),_tmp1165))));}
return def;_LLCC:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp1167;struct _tuple0 _tmp209=(_tmp1167.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1167.f2=
Cyc_Absyn_compress_kb(kb2),_tmp1167)));void*_tmp20A;struct Cyc_Absyn_Kind*_tmp20C;
void*_tmp20D;struct Cyc_Absyn_Kind*_tmp20F;void*_tmp210;struct Cyc_Core_Opt*
_tmp212;struct Cyc_Core_Opt**_tmp213;struct Cyc_Absyn_Kind*_tmp214;void*_tmp215;
struct Cyc_Absyn_Kind*_tmp217;void*_tmp218;struct Cyc_Absyn_Kind*_tmp21A;void*
_tmp21B;struct Cyc_Core_Opt*_tmp21D;struct Cyc_Core_Opt**_tmp21E;struct Cyc_Absyn_Kind*
_tmp21F;void*_tmp220;struct Cyc_Core_Opt*_tmp222;struct Cyc_Core_Opt**_tmp223;
struct Cyc_Absyn_Kind*_tmp224;void*_tmp225;struct Cyc_Core_Opt*_tmp227;struct Cyc_Core_Opt**
_tmp228;struct Cyc_Absyn_Kind*_tmp229;void*_tmp22A;struct Cyc_Core_Opt*_tmp22C;
struct Cyc_Core_Opt**_tmp22D;void*_tmp22E;void*_tmp22F;void*_tmp230;struct Cyc_Core_Opt*
_tmp232;struct Cyc_Core_Opt**_tmp233;_LLD4: _tmp20A=_tmp209.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp20B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp20A;if(_tmp20B->tag != 0)goto _LLD6;
else{_tmp20C=_tmp20B->f1;}};_tmp20D=_tmp209.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp20E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp20D;if(_tmp20E->tag != 0)goto _LLD6;
else{_tmp20F=_tmp20E->f1;}};_LLD5: return _tmp20C == _tmp20F;_LLD6: _tmp210=_tmp209.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp211=(struct Cyc_Absyn_Less_kb_struct*)_tmp210;
if(_tmp211->tag != 2)goto _LLD8;else{_tmp212=_tmp211->f1;_tmp213=(struct Cyc_Core_Opt**)&
_tmp211->f1;_tmp214=_tmp211->f2;}};_tmp215=_tmp209.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp216=(struct Cyc_Absyn_Eq_kb_struct*)_tmp215;if(_tmp216->tag != 0)goto _LLD8;
else{_tmp217=_tmp216->f1;}};_LLD7: if(Cyc_Tcutil_kind_leq(_tmp217,_tmp214)){{
struct Cyc_Core_Opt*_tmp1168;*_tmp213=((_tmp1168=_cycalloc(sizeof(*_tmp1168)),((
_tmp1168->v=kb2,_tmp1168))));}return 1;}else{return 0;}_LLD8: _tmp218=_tmp209.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp219=(struct Cyc_Absyn_Eq_kb_struct*)_tmp218;if(
_tmp219->tag != 0)goto _LLDA;else{_tmp21A=_tmp219->f1;}};_tmp21B=_tmp209.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21C=(struct Cyc_Absyn_Less_kb_struct*)_tmp21B;
if(_tmp21C->tag != 2)goto _LLDA;else{_tmp21D=_tmp21C->f1;_tmp21E=(struct Cyc_Core_Opt**)&
_tmp21C->f1;_tmp21F=_tmp21C->f2;}};_LLD9: if(Cyc_Tcutil_kind_leq(_tmp21A,_tmp21F)){{
struct Cyc_Core_Opt*_tmp1169;*_tmp21E=((_tmp1169=_cycalloc(sizeof(*_tmp1169)),((
_tmp1169->v=kb1,_tmp1169))));}return 1;}else{return 0;}_LLDA: _tmp220=_tmp209.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp221=(struct Cyc_Absyn_Less_kb_struct*)_tmp220;
if(_tmp221->tag != 2)goto _LLDC;else{_tmp222=_tmp221->f1;_tmp223=(struct Cyc_Core_Opt**)&
_tmp221->f1;_tmp224=_tmp221->f2;}};_tmp225=_tmp209.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp226=(struct Cyc_Absyn_Less_kb_struct*)_tmp225;if(_tmp226->tag != 2)goto _LLDC;
else{_tmp227=_tmp226->f1;_tmp228=(struct Cyc_Core_Opt**)& _tmp226->f1;_tmp229=
_tmp226->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp224,_tmp229)){{struct Cyc_Core_Opt*
_tmp116A;*_tmp228=((_tmp116A=_cycalloc(sizeof(*_tmp116A)),((_tmp116A->v=kb1,
_tmp116A))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp229,_tmp224)){{struct Cyc_Core_Opt*
_tmp116B;*_tmp223=((_tmp116B=_cycalloc(sizeof(*_tmp116B)),((_tmp116B->v=kb2,
_tmp116B))));}return 1;}else{return 0;}}_LLDC: _tmp22A=_tmp209.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22B=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22A;if(_tmp22B->tag != 1)goto
_LLDE;else{_tmp22C=_tmp22B->f1;_tmp22D=(struct Cyc_Core_Opt**)& _tmp22B->f1;}};
_tmp22E=_tmp209.f2;_LLDD: _tmp22F=_tmp22E;_tmp233=_tmp22D;goto _LLDF;_LLDE: _tmp22F=
_tmp209.f1;_tmp230=_tmp209.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp231=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp230;if(_tmp231->tag != 1)goto _LLD3;else{_tmp232=
_tmp231->f1;_tmp233=(struct Cyc_Core_Opt**)& _tmp231->f1;}};_LLDF:{struct Cyc_Core_Opt*
_tmp116C;*_tmp233=((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((_tmp116C->v=_tmp22F,
_tmp116C))));}return 1;_LLD3:;}struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp239=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23B;_LLE1: {struct
Cyc_Absyn_VarType_struct*_tmp23A=(struct Cyc_Absyn_VarType_struct*)_tmp239;if(
_tmp23A->tag != 2)goto _LLE3;else{_tmp23B=_tmp23A->f1;}}_LLE2: {void*_tmp23C=
_tmp23B->kind;_tmp23B->kind=kb;{struct _tuple15 _tmp116D;return(_tmp116D.f1=
_tmp23B,((_tmp116D.f2=_tmp23C,_tmp116D)));};}_LLE3:;_LLE4: {const char*_tmp1171;
void*_tmp1170[1];struct Cyc_String_pa_struct _tmp116F;(_tmp116F.tag=0,((_tmp116F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1170[0]=& _tmp116F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1171="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp1171,sizeof(char),40))),_tag_dyneither(_tmp1170,sizeof(void*),
1)))))));}_LLE0:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp241=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp243;struct Cyc_Absyn_Tvar*_tmp245;
enum Cyc_Absyn_Size_of _tmp248;struct Cyc_Absyn_DatatypeFieldInfo _tmp254;union Cyc_Absyn_DatatypeFieldInfoU
_tmp255;struct _tuple3 _tmp256;struct Cyc_Absyn_Datatypedecl*_tmp257;struct Cyc_Absyn_Datatypefield*
_tmp258;struct Cyc_Absyn_DatatypeFieldInfo _tmp25A;union Cyc_Absyn_DatatypeFieldInfoU
_tmp25B;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp25C;struct Cyc_Absyn_AggrInfo
_tmp25E;union Cyc_Absyn_AggrInfoU _tmp25F;struct _tuple4 _tmp260;struct Cyc_Absyn_AggrInfo
_tmp262;union Cyc_Absyn_AggrInfoU _tmp263;struct Cyc_Absyn_Aggrdecl**_tmp264;struct
Cyc_Absyn_Aggrdecl*_tmp265;struct Cyc_Absyn_Aggrdecl _tmp266;struct Cyc_Absyn_AggrdeclImpl*
_tmp267;struct Cyc_Absyn_PtrInfo _tmp26A;struct Cyc_Absyn_Typedefdecl*_tmp270;
struct Cyc_Absyn_TypeDecl*_tmp272;struct Cyc_Absyn_TypeDecl _tmp273;void*_tmp274;
struct Cyc_Absyn_TypeDecl*_tmp277;struct Cyc_Absyn_TypeDecl _tmp278;void*_tmp279;
struct Cyc_Absyn_TypeDecl*_tmp27C;struct Cyc_Absyn_TypeDecl _tmp27D;void*_tmp27E;
_LLE6: {struct Cyc_Absyn_Evar_struct*_tmp242=(struct Cyc_Absyn_Evar_struct*)
_tmp241;if(_tmp242->tag != 1)goto _LLE8;else{_tmp243=_tmp242->f1;}}_LLE7: return(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp243))->v;_LLE8: {
struct Cyc_Absyn_VarType_struct*_tmp244=(struct Cyc_Absyn_VarType_struct*)_tmp241;
if(_tmp244->tag != 2)goto _LLEA;else{_tmp245=_tmp244->f1;}}_LLE9: return Cyc_Tcutil_tvar_kind(
_tmp245,& Cyc_Tcutil_bk);_LLEA: {struct Cyc_Absyn_VoidType_struct*_tmp246=(struct
Cyc_Absyn_VoidType_struct*)_tmp241;if(_tmp246->tag != 0)goto _LLEC;}_LLEB: return&
Cyc_Tcutil_mk;_LLEC: {struct Cyc_Absyn_IntType_struct*_tmp247=(struct Cyc_Absyn_IntType_struct*)
_tmp241;if(_tmp247->tag != 6)goto _LLEE;else{_tmp248=_tmp247->f2;}}_LLED: return(
_tmp248 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp248 == (enum Cyc_Absyn_Size_of)
Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_FloatType_struct*
_tmp249=(struct Cyc_Absyn_FloatType_struct*)_tmp241;if(_tmp249->tag != 7)goto _LLF0;}
_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_DoubleType_struct*_tmp24A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp241;if(_tmp24A->tag != 8)goto _LLF2;}_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_FnType_struct*
_tmp24B=(struct Cyc_Absyn_FnType_struct*)_tmp241;if(_tmp24B->tag != 10)goto _LLF4;}
_LLF3: return& Cyc_Tcutil_mk;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*_tmp24C=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp241;if(_tmp24C->tag != 17)goto _LLF6;}_LLF5:
goto _LLF7;_LLF6: {struct Cyc_Absyn_EnumType_struct*_tmp24D=(struct Cyc_Absyn_EnumType_struct*)
_tmp241;if(_tmp24D->tag != 14)goto _LLF8;}_LLF7: goto _LLF9;_LLF8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp24E=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp241;if(_tmp24E->tag != 15)goto
_LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_RgnHandleType_struct*_tmp24F=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp241;if(_tmp24F->tag != 16)goto _LLFC;}
_LLFB: return& Cyc_Tcutil_bk;_LLFC: {struct Cyc_Absyn_UniqueRgn_struct*_tmp250=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp241;if(_tmp250->tag != 22)goto _LLFE;}_LLFD:
return& Cyc_Tcutil_urk;_LLFE: {struct Cyc_Absyn_HeapRgn_struct*_tmp251=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp241;if(_tmp251->tag != 21)goto _LL100;}_LLFF: return& Cyc_Tcutil_rk;_LL100: {
struct Cyc_Absyn_DatatypeType_struct*_tmp252=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp241;if(_tmp252->tag != 3)goto _LL102;}_LL101: return& Cyc_Tcutil_ak;_LL102: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp253=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp241;if(_tmp253->tag != 4)goto _LL104;else{_tmp254=_tmp253->f1;_tmp255=_tmp254.field_info;
if((_tmp255.KnownDatatypefield).tag != 2)goto _LL104;_tmp256=(struct _tuple3)(
_tmp255.KnownDatatypefield).val;_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;}}_LL103:
return& Cyc_Tcutil_mk;_LL104: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp259=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp241;if(_tmp259->tag != 4)goto _LL106;
else{_tmp25A=_tmp259->f1;_tmp25B=_tmp25A.field_info;if((_tmp25B.UnknownDatatypefield).tag
!= 1)goto _LL106;_tmp25C=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp25B.UnknownDatatypefield).val;}}
_LL105: {const char*_tmp1174;void*_tmp1173;(_tmp1173=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1174="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp1174,sizeof(char),39))),_tag_dyneither(_tmp1173,sizeof(void*),
0)));}_LL106: {struct Cyc_Absyn_AggrType_struct*_tmp25D=(struct Cyc_Absyn_AggrType_struct*)
_tmp241;if(_tmp25D->tag != 12)goto _LL108;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25E.aggr_info;
if((_tmp25F.UnknownAggr).tag != 1)goto _LL108;_tmp260=(struct _tuple4)(_tmp25F.UnknownAggr).val;}}
_LL107: return& Cyc_Tcutil_ak;_LL108: {struct Cyc_Absyn_AggrType_struct*_tmp261=(
struct Cyc_Absyn_AggrType_struct*)_tmp241;if(_tmp261->tag != 12)goto _LL10A;else{
_tmp262=_tmp261->f1;_tmp263=_tmp262.aggr_info;if((_tmp263.KnownAggr).tag != 2)
goto _LL10A;_tmp264=(struct Cyc_Absyn_Aggrdecl**)(_tmp263.KnownAggr).val;_tmp265=*
_tmp264;_tmp266=*_tmp265;_tmp267=_tmp266.impl;}}_LL109: return _tmp267 == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LL10A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp268=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp241;if(_tmp268->tag != 13)goto _LL10C;}_LL10B: return& Cyc_Tcutil_mk;_LL10C: {
struct Cyc_Absyn_PointerType_struct*_tmp269=(struct Cyc_Absyn_PointerType_struct*)
_tmp241;if(_tmp269->tag != 5)goto _LL10E;else{_tmp26A=_tmp269->f1;}}_LL10D: {void*
_tmp285=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp26A.ptr_atts).bounds);_LL125: {struct Cyc_Absyn_DynEither_b_struct*
_tmp286=(struct Cyc_Absyn_DynEither_b_struct*)_tmp285;if(_tmp286->tag != 0)goto
_LL127;}_LL126: {struct Cyc_Absyn_Kind*_tmp1175;return(_tmp1175=_cycalloc_atomic(
sizeof(*_tmp1175)),((_tmp1175->kind=Cyc_Absyn_MemKind,((_tmp1175->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26A.ptr_atts).rgn))->aliasqual,_tmp1175)))));}_LL127: {struct Cyc_Absyn_Upper_b_struct*
_tmp287=(struct Cyc_Absyn_Upper_b_struct*)_tmp285;if(_tmp287->tag != 1)goto _LL124;}
_LL128: {struct Cyc_Absyn_Kind*_tmp1176;return(_tmp1176=_cycalloc_atomic(sizeof(*
_tmp1176)),((_tmp1176->kind=Cyc_Absyn_BoxKind,((_tmp1176->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26A.ptr_atts).rgn))->aliasqual,_tmp1176)))));}_LL124:;}_LL10E: {struct Cyc_Absyn_ValueofType_struct*
_tmp26B=(struct Cyc_Absyn_ValueofType_struct*)_tmp241;if(_tmp26B->tag != 19)goto
_LL110;}_LL10F: return& Cyc_Tcutil_ik;_LL110: {struct Cyc_Absyn_TagType_struct*
_tmp26C=(struct Cyc_Absyn_TagType_struct*)_tmp241;if(_tmp26C->tag != 20)goto _LL112;}
_LL111: return& Cyc_Tcutil_bk;_LL112: {struct Cyc_Absyn_ArrayType_struct*_tmp26D=(
struct Cyc_Absyn_ArrayType_struct*)_tmp241;if(_tmp26D->tag != 9)goto _LL114;}_LL113:
goto _LL115;_LL114: {struct Cyc_Absyn_TupleType_struct*_tmp26E=(struct Cyc_Absyn_TupleType_struct*)
_tmp241;if(_tmp26E->tag != 11)goto _LL116;}_LL115: return& Cyc_Tcutil_mk;_LL116: {
struct Cyc_Absyn_TypedefType_struct*_tmp26F=(struct Cyc_Absyn_TypedefType_struct*)
_tmp241;if(_tmp26F->tag != 18)goto _LL118;else{_tmp270=_tmp26F->f3;}}_LL117: if(
_tmp270 == 0  || _tmp270->kind == 0){const char*_tmp117A;void*_tmp1179[1];struct Cyc_String_pa_struct
_tmp1178;(_tmp1178.tag=0,((_tmp1178.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1179[0]=& _tmp1178,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp117A="typ_kind: typedef found: %s",
_tag_dyneither(_tmp117A,sizeof(char),28))),_tag_dyneither(_tmp1179,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp270->kind))->v;
_LL118: {struct Cyc_Absyn_TypeDeclType_struct*_tmp271=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp241;if(_tmp271->tag != 26)goto _LL11A;else{_tmp272=_tmp271->f1;_tmp273=*
_tmp272;_tmp274=_tmp273.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp275=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp274;if(_tmp275->tag != 0)goto _LL11A;};}}_LL119: return& Cyc_Tcutil_ak;_LL11A: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp276=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp241;if(_tmp276->tag != 26)goto _LL11C;else{_tmp277=_tmp276->f1;_tmp278=*
_tmp277;_tmp279=_tmp278.r;{struct Cyc_Absyn_Enum_td_struct*_tmp27A=(struct Cyc_Absyn_Enum_td_struct*)
_tmp279;if(_tmp27A->tag != 1)goto _LL11C;};}}_LL11B: return& Cyc_Tcutil_bk;_LL11C: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp27B=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp241;if(_tmp27B->tag != 26)goto _LL11E;else{_tmp27C=_tmp27B->f1;_tmp27D=*
_tmp27C;_tmp27E=_tmp27D.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp27F=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp27E;if(_tmp27F->tag != 2)goto _LL11E;};}}_LL11D: return& Cyc_Tcutil_ak;_LL11E: {
struct Cyc_Absyn_AccessEff_struct*_tmp280=(struct Cyc_Absyn_AccessEff_struct*)
_tmp241;if(_tmp280->tag != 23)goto _LL120;}_LL11F: goto _LL121;_LL120: {struct Cyc_Absyn_JoinEff_struct*
_tmp281=(struct Cyc_Absyn_JoinEff_struct*)_tmp241;if(_tmp281->tag != 24)goto _LL122;}
_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_RgnsEff_struct*_tmp282=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp241;if(_tmp282->tag != 25)goto _LLE5;}_LL123: return& Cyc_Tcutil_ek;_LLE5:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == 
k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp28D;_push_handler(& _tmp28D);{int _tmp28F=
0;if(setjmp(_tmp28D.handler))_tmp28F=1;if(!_tmp28F){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp290=1;_npop_handler(0);return _tmp290;};;_pop_handler();}else{void*_tmp28E=(
void*)_exn_thrown;void*_tmp292=_tmp28E;_LL12A: {struct Cyc_Tcutil_Unify_struct*
_tmp293=(struct Cyc_Tcutil_Unify_struct*)_tmp292;if(_tmp293->tag != Cyc_Tcutil_Unify)
goto _LL12C;}_LL12B: return 0;_LL12C:;_LL12D:(void)_throw(_tmp292);_LL129:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*
_tmp294=t;struct Cyc_Absyn_Tvar*_tmp296;struct Cyc_Core_Opt*_tmp298;struct Cyc_Core_Opt*
_tmp299;struct Cyc_Core_Opt**_tmp29A;struct Cyc_Absyn_PtrInfo _tmp29C;struct Cyc_Absyn_ArrayInfo
_tmp29E;void*_tmp29F;struct Cyc_Absyn_FnInfo _tmp2A1;struct Cyc_List_List*_tmp2A2;
struct Cyc_Core_Opt*_tmp2A3;void*_tmp2A4;struct Cyc_List_List*_tmp2A5;int _tmp2A6;
struct Cyc_Absyn_VarargInfo*_tmp2A7;struct Cyc_List_List*_tmp2A8;struct Cyc_List_List*
_tmp2A9;struct Cyc_List_List*_tmp2AB;struct Cyc_Absyn_DatatypeInfo _tmp2AD;struct
Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2B0;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_AggrInfo _tmp2B5;struct Cyc_List_List*
_tmp2B6;struct Cyc_List_List*_tmp2B8;void*_tmp2BA;void*_tmp2BC;void*_tmp2BE;void*
_tmp2C0;struct Cyc_List_List*_tmp2C2;_LL12F: {struct Cyc_Absyn_VarType_struct*
_tmp295=(struct Cyc_Absyn_VarType_struct*)_tmp294;if(_tmp295->tag != 2)goto _LL131;
else{_tmp296=_tmp295->f1;}}_LL130: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmp296)){{const char*_tmp117B;Cyc_Tcutil_failure_reason=((
_tmp117B="(type variable would escape scope)",_tag_dyneither(_tmp117B,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL12E;_LL131: {
struct Cyc_Absyn_Evar_struct*_tmp297=(struct Cyc_Absyn_Evar_struct*)_tmp294;if(
_tmp297->tag != 1)goto _LL133;else{_tmp298=_tmp297->f2;_tmp299=_tmp297->f4;_tmp29A=(
struct Cyc_Core_Opt**)& _tmp297->f4;}}_LL132: if(t == evar){{const char*_tmp117C;Cyc_Tcutil_failure_reason=((
_tmp117C="(occurs check)",_tag_dyneither(_tmp117C,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp298 != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp298->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp29A))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2C5=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp29A))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp117D;_tmp2C5=((
_tmp117D=_cycalloc(sizeof(*_tmp117D)),((_tmp117D->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp117D->tl=_tmp2C5,_tmp117D))))));}}}{struct Cyc_Core_Opt*_tmp117E;*_tmp29A=((
_tmp117E=_cycalloc(sizeof(*_tmp117E)),((_tmp117E->v=_tmp2C5,_tmp117E))));};}}}
goto _LL12E;_LL133: {struct Cyc_Absyn_PointerType_struct*_tmp29B=(struct Cyc_Absyn_PointerType_struct*)
_tmp294;if(_tmp29B->tag != 5)goto _LL135;else{_tmp29C=_tmp29B->f1;}}_LL134: Cyc_Tcutil_occurs(
evar,r,env,_tmp29C.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp29C.ptr_atts).rgn);
goto _LL12E;_LL135: {struct Cyc_Absyn_ArrayType_struct*_tmp29D=(struct Cyc_Absyn_ArrayType_struct*)
_tmp294;if(_tmp29D->tag != 9)goto _LL137;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29E.elt_type;}}
_LL136: Cyc_Tcutil_occurs(evar,r,env,_tmp29F);goto _LL12E;_LL137: {struct Cyc_Absyn_FnType_struct*
_tmp2A0=(struct Cyc_Absyn_FnType_struct*)_tmp294;if(_tmp2A0->tag != 10)goto _LL139;
else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A1.tvars;_tmp2A3=_tmp2A1.effect;_tmp2A4=
_tmp2A1.ret_typ;_tmp2A5=_tmp2A1.args;_tmp2A6=_tmp2A1.c_varargs;_tmp2A7=_tmp2A1.cyc_varargs;
_tmp2A8=_tmp2A1.rgn_po;_tmp2A9=_tmp2A1.attributes;}}_LL138: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp2A2,env);if(_tmp2A3 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp2A3->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp2A4);for(0;_tmp2A5 != 0;_tmp2A5=_tmp2A5->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp2A5->hd)).f3);}if(_tmp2A7 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp2A7->type);for(0;_tmp2A8 != 0;_tmp2A8=_tmp2A8->tl){struct _tuple0
_tmp2C9;void*_tmp2CA;void*_tmp2CB;struct _tuple0*_tmp2C8=(struct _tuple0*)_tmp2A8->hd;
_tmp2C9=*_tmp2C8;_tmp2CA=_tmp2C9.f1;_tmp2CB=_tmp2C9.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp2CA);Cyc_Tcutil_occurs(evar,r,env,_tmp2CB);}goto _LL12E;_LL139: {struct
Cyc_Absyn_TupleType_struct*_tmp2AA=(struct Cyc_Absyn_TupleType_struct*)_tmp294;
if(_tmp2AA->tag != 11)goto _LL13B;else{_tmp2AB=_tmp2AA->f1;}}_LL13A: for(0;_tmp2AB
!= 0;_tmp2AB=_tmp2AB->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)
_tmp2AB->hd)).f2);}goto _LL12E;_LL13B: {struct Cyc_Absyn_DatatypeType_struct*
_tmp2AC=(struct Cyc_Absyn_DatatypeType_struct*)_tmp294;if(_tmp2AC->tag != 3)goto
_LL13D;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AD.targs;}}_LL13C: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2AE);goto _LL12E;_LL13D: {struct Cyc_Absyn_TypedefType_struct*
_tmp2AF=(struct Cyc_Absyn_TypedefType_struct*)_tmp294;if(_tmp2AF->tag != 18)goto
_LL13F;else{_tmp2B0=_tmp2AF->f2;}}_LL13E: _tmp2B3=_tmp2B0;goto _LL140;_LL13F: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2B1=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp294;if(_tmp2B1->tag != 4)goto _LL141;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B2.targs;}}
_LL140: _tmp2B6=_tmp2B3;goto _LL142;_LL141: {struct Cyc_Absyn_AggrType_struct*
_tmp2B4=(struct Cyc_Absyn_AggrType_struct*)_tmp294;if(_tmp2B4->tag != 12)goto
_LL143;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B5.targs;}}_LL142: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2B6);goto _LL12E;_LL143: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2B7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp294;if(_tmp2B7->tag != 13)goto
_LL145;else{_tmp2B8=_tmp2B7->f2;}}_LL144: for(0;_tmp2B8 != 0;_tmp2B8=_tmp2B8->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2B8->hd)->type);}
goto _LL12E;_LL145: {struct Cyc_Absyn_TagType_struct*_tmp2B9=(struct Cyc_Absyn_TagType_struct*)
_tmp294;if(_tmp2B9->tag != 20)goto _LL147;else{_tmp2BA=(void*)_tmp2B9->f1;}}_LL146:
_tmp2BC=_tmp2BA;goto _LL148;_LL147: {struct Cyc_Absyn_AccessEff_struct*_tmp2BB=(
struct Cyc_Absyn_AccessEff_struct*)_tmp294;if(_tmp2BB->tag != 23)goto _LL149;else{
_tmp2BC=(void*)_tmp2BB->f1;}}_LL148: _tmp2BE=_tmp2BC;goto _LL14A;_LL149: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp2BD=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp294;if(_tmp2BD->tag != 16)goto _LL14B;else{_tmp2BE=(void*)_tmp2BD->f1;}}_LL14A:
_tmp2C0=_tmp2BE;goto _LL14C;_LL14B: {struct Cyc_Absyn_RgnsEff_struct*_tmp2BF=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp294;if(_tmp2BF->tag != 25)goto _LL14D;else{
_tmp2C0=(void*)_tmp2BF->f1;}}_LL14C: Cyc_Tcutil_occurs(evar,r,env,_tmp2C0);goto
_LL12E;_LL14D: {struct Cyc_Absyn_JoinEff_struct*_tmp2C1=(struct Cyc_Absyn_JoinEff_struct*)
_tmp294;if(_tmp2C1->tag != 24)goto _LL14F;else{_tmp2C2=_tmp2C1->f1;}}_LL14E: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2C2);goto _LL12E;_LL14F:;_LL150: goto _LL12E;_LL12E:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp1181;void*
_tmp1180;(_tmp1180=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1181="tq1 real_const not set.",_tag_dyneither(_tmp1181,
sizeof(char),24))),_tag_dyneither(_tmp1180,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp1184;void*_tmp1183;(_tmp1183=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1184="tq2 real_const not set.",
_tag_dyneither(_tmp1184,sizeof(char),24))),_tag_dyneither(_tmp1183,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp1185;Cyc_Tcutil_failure_reason=((_tmp1185="(qualifiers don't match)",
_tag_dyneither(_tmp1185,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2D1=x;union Cyc_Absyn_Constraint _tmp2D2;int _tmp2D3;union Cyc_Absyn_Constraint
_tmp2D4;void*_tmp2D5;union Cyc_Absyn_Constraint _tmp2D6;union Cyc_Absyn_Constraint*
_tmp2D7;_LL152: _tmp2D2=*_tmp2D1;if((_tmp2D2.No_constr).tag != 3)goto _LL154;
_tmp2D3=(int)(_tmp2D2.No_constr).val;_LL153:{union Cyc_Absyn_Constraint _tmp1186;*
x=(((_tmp1186.Forward_constr).val=y,(((_tmp1186.Forward_constr).tag=2,_tmp1186))));}
return;_LL154: _tmp2D4=*_tmp2D1;if((_tmp2D4.Eq_constr).tag != 1)goto _LL156;_tmp2D5=(
void*)(_tmp2D4.Eq_constr).val;_LL155: {union Cyc_Absyn_Constraint*_tmp2D9=y;union
Cyc_Absyn_Constraint _tmp2DA;int _tmp2DB;union Cyc_Absyn_Constraint _tmp2DC;void*
_tmp2DD;union Cyc_Absyn_Constraint _tmp2DE;union Cyc_Absyn_Constraint*_tmp2DF;
_LL159: _tmp2DA=*_tmp2D9;if((_tmp2DA.No_constr).tag != 3)goto _LL15B;_tmp2DB=(int)(
_tmp2DA.No_constr).val;_LL15A:*y=*x;return;_LL15B: _tmp2DC=*_tmp2D9;if((_tmp2DC.Eq_constr).tag
!= 1)goto _LL15D;_tmp2DD=(void*)(_tmp2DC.Eq_constr).val;_LL15C: if(cmp(_tmp2D5,
_tmp2DD)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL15D: _tmp2DE=*_tmp2D9;if((_tmp2DE.Forward_constr).tag != 2)goto _LL158;
_tmp2DF=(union Cyc_Absyn_Constraint*)(_tmp2DE.Forward_constr).val;_LL15E: {const
char*_tmp1189;void*_tmp1188;(_tmp1188=0,Cyc_Tcutil_impos(((_tmp1189="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp1189,sizeof(char),40))),_tag_dyneither(_tmp1188,sizeof(void*),
0)));}_LL158:;}_LL156: _tmp2D6=*_tmp2D1;if((_tmp2D6.Forward_constr).tag != 2)goto
_LL151;_tmp2D7=(union Cyc_Absyn_Constraint*)(_tmp2D6.Forward_constr).val;_LL157: {
const char*_tmp118C;void*_tmp118B;(_tmp118B=0,Cyc_Tcutil_impos(((_tmp118C="unify_conref: forward after compress",
_tag_dyneither(_tmp118C,sizeof(char),37))),_tag_dyneither(_tmp118B,sizeof(void*),
0)));}_LL151:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2E4;
_push_handler(& _tmp2E4);{int _tmp2E6=0;if(setjmp(_tmp2E4.handler))_tmp2E6=1;if(!
_tmp2E6){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2E7=1;_npop_handler(0);return _tmp2E7;};;
_pop_handler();}else{void*_tmp2E5=(void*)_exn_thrown;void*_tmp2E9=_tmp2E5;_LL160: {
struct Cyc_Tcutil_Unify_struct*_tmp2EA=(struct Cyc_Tcutil_Unify_struct*)_tmp2E9;
if(_tmp2EA->tag != Cyc_Tcutil_Unify)goto _LL162;}_LL161: return 0;_LL162:;_LL163:(
void)_throw(_tmp2E9);_LL15F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp118D;struct _tuple0 _tmp2EC=(_tmp118D.f1=b1,((_tmp118D.f2=b2,
_tmp118D)));void*_tmp2ED;void*_tmp2EF;void*_tmp2F1;void*_tmp2F3;void*_tmp2F5;
struct Cyc_Absyn_Exp*_tmp2F7;void*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2FA;_LL165:
_tmp2ED=_tmp2EC.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2EE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2ED;if(_tmp2EE->tag != 0)goto _LL167;};_tmp2EF=_tmp2EC.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F0=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2EF;if(_tmp2F0->tag != 0)goto
_LL167;};_LL166: return 0;_LL167: _tmp2F1=_tmp2EC.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F2=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F1;if(_tmp2F2->tag != 0)goto
_LL169;};_LL168: return - 1;_LL169: _tmp2F3=_tmp2EC.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F4=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F3;if(_tmp2F4->tag != 0)goto
_LL16B;};_LL16A: return 1;_LL16B: _tmp2F5=_tmp2EC.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2F6=(struct Cyc_Absyn_Upper_b_struct*)_tmp2F5;if(_tmp2F6->tag != 1)goto _LL164;
else{_tmp2F7=_tmp2F6->f1;}};_tmp2F8=_tmp2EC.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2F9=(struct Cyc_Absyn_Upper_b_struct*)_tmp2F8;if(_tmp2F9->tag != 1)goto _LL164;
else{_tmp2FA=_tmp2F9->f1;}};_LL16C: return Cyc_Evexp_const_exp_cmp(_tmp2F7,_tmp2FA);
_LL164:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0
_tmp118E;struct _tuple0 _tmp2FC=(_tmp118E.f1=b1,((_tmp118E.f2=b2,_tmp118E)));void*
_tmp2FD;void*_tmp2FF;void*_tmp301;void*_tmp303;void*_tmp305;struct Cyc_Absyn_Exp*
_tmp307;void*_tmp308;struct Cyc_Absyn_Exp*_tmp30A;_LL16E: _tmp2FD=_tmp2FC.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp2FE=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2FD;if(_tmp2FE->tag != 0)goto _LL170;};_tmp2FF=_tmp2FC.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp300=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2FF;if(_tmp300->tag != 0)goto
_LL170;};_LL16F: return 0;_LL170: _tmp301=_tmp2FC.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp302=(struct Cyc_Absyn_DynEither_b_struct*)_tmp301;if(_tmp302->tag != 0)goto
_LL172;};_LL171: return - 1;_LL172: _tmp303=_tmp2FC.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp304=(struct Cyc_Absyn_DynEither_b_struct*)_tmp303;if(_tmp304->tag != 0)goto
_LL174;};_LL173: return 1;_LL174: _tmp305=_tmp2FC.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp306=(struct Cyc_Absyn_Upper_b_struct*)_tmp305;if(_tmp306->tag != 1)goto _LL16D;
else{_tmp307=_tmp306->f1;}};_tmp308=_tmp2FC.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp309=(struct Cyc_Absyn_Upper_b_struct*)_tmp308;if(_tmp309->tag != 1)goto _LL16D;
else{_tmp30A=_tmp309->f1;}};_LL175: return Cyc_Evexp_const_exp_cmp(_tmp307,_tmp30A);
_LL16D:;}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp30B=att;
_LL177: {struct Cyc_Absyn_Regparm_att_struct*_tmp30C=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp30B;if(_tmp30C->tag != 0)goto _LL179;}_LL178: return 0;_LL179: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp30D=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp30B;if(_tmp30D->tag != 1)goto
_LL17B;}_LL17A: return 1;_LL17B: {struct Cyc_Absyn_Cdecl_att_struct*_tmp30E=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp30B;if(_tmp30E->tag != 2)goto _LL17D;}_LL17C:
return 2;_LL17D: {struct Cyc_Absyn_Fastcall_att_struct*_tmp30F=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp30B;if(_tmp30F->tag != 3)goto _LL17F;}_LL17E: return 3;_LL17F: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp310=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp30B;if(_tmp310->tag != 4)goto
_LL181;}_LL180: return 4;_LL181: {struct Cyc_Absyn_Const_att_struct*_tmp311=(struct
Cyc_Absyn_Const_att_struct*)_tmp30B;if(_tmp311->tag != 5)goto _LL183;}_LL182:
return 5;_LL183: {struct Cyc_Absyn_Aligned_att_struct*_tmp312=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp30B;if(_tmp312->tag != 6)goto _LL185;}_LL184: return 6;_LL185: {struct Cyc_Absyn_Packed_att_struct*
_tmp313=(struct Cyc_Absyn_Packed_att_struct*)_tmp30B;if(_tmp313->tag != 7)goto
_LL187;}_LL186: return 7;_LL187: {struct Cyc_Absyn_Section_att_struct*_tmp314=(
struct Cyc_Absyn_Section_att_struct*)_tmp30B;if(_tmp314->tag != 8)goto _LL189;}
_LL188: return 8;_LL189: {struct Cyc_Absyn_Nocommon_att_struct*_tmp315=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp30B;if(_tmp315->tag != 9)goto _LL18B;}_LL18A: return 9;_LL18B: {struct Cyc_Absyn_Shared_att_struct*
_tmp316=(struct Cyc_Absyn_Shared_att_struct*)_tmp30B;if(_tmp316->tag != 10)goto
_LL18D;}_LL18C: return 10;_LL18D: {struct Cyc_Absyn_Unused_att_struct*_tmp317=(
struct Cyc_Absyn_Unused_att_struct*)_tmp30B;if(_tmp317->tag != 11)goto _LL18F;}
_LL18E: return 11;_LL18F: {struct Cyc_Absyn_Weak_att_struct*_tmp318=(struct Cyc_Absyn_Weak_att_struct*)
_tmp30B;if(_tmp318->tag != 12)goto _LL191;}_LL190: return 12;_LL191: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp319=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp30B;if(_tmp319->tag != 13)goto
_LL193;}_LL192: return 13;_LL193: {struct Cyc_Absyn_Dllexport_att_struct*_tmp31A=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp30B;if(_tmp31A->tag != 14)goto _LL195;}
_LL194: return 14;_LL195: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp31B=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp30B;if(_tmp31B->tag
!= 15)goto _LL197;}_LL196: return 15;_LL197: {struct Cyc_Absyn_Constructor_att_struct*
_tmp31C=(struct Cyc_Absyn_Constructor_att_struct*)_tmp30B;if(_tmp31C->tag != 16)
goto _LL199;}_LL198: return 16;_LL199: {struct Cyc_Absyn_Destructor_att_struct*
_tmp31D=(struct Cyc_Absyn_Destructor_att_struct*)_tmp30B;if(_tmp31D->tag != 17)
goto _LL19B;}_LL19A: return 17;_LL19B: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp31E=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp30B;if(_tmp31E->tag
!= 18)goto _LL19D;}_LL19C: return 18;_LL19D: {struct Cyc_Absyn_Format_att_struct*
_tmp31F=(struct Cyc_Absyn_Format_att_struct*)_tmp30B;if(_tmp31F->tag != 19)goto
_LL19F;}_LL19E: return 19;_LL19F: {struct Cyc_Absyn_Initializes_att_struct*_tmp320=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp30B;if(_tmp320->tag != 20)goto _LL1A1;}
_LL1A0: return 20;_LL1A1:;_LL1A2: return 21;_LL176:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp118F;struct _tuple0 _tmp322=(_tmp118F.f1=
att1,((_tmp118F.f2=att2,_tmp118F)));void*_tmp323;int _tmp325;void*_tmp326;int
_tmp328;void*_tmp329;int _tmp32B;void*_tmp32C;int _tmp32E;void*_tmp32F;int _tmp331;
void*_tmp332;int _tmp334;void*_tmp335;struct _dyneither_ptr _tmp337;void*_tmp338;
struct _dyneither_ptr _tmp33A;void*_tmp33B;enum Cyc_Absyn_Format_Type _tmp33D;int
_tmp33E;int _tmp33F;void*_tmp340;enum Cyc_Absyn_Format_Type _tmp342;int _tmp343;int
_tmp344;_LL1A4: _tmp323=_tmp322.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp324=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp323;if(_tmp324->tag != 0)goto _LL1A6;else{
_tmp325=_tmp324->f1;}};_tmp326=_tmp322.f2;{struct Cyc_Absyn_Regparm_att_struct*
_tmp327=(struct Cyc_Absyn_Regparm_att_struct*)_tmp326;if(_tmp327->tag != 0)goto
_LL1A6;else{_tmp328=_tmp327->f1;}};_LL1A5: _tmp32B=_tmp325;_tmp32E=_tmp328;goto
_LL1A7;_LL1A6: _tmp329=_tmp322.f1;{struct Cyc_Absyn_Initializes_att_struct*_tmp32A=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp329;if(_tmp32A->tag != 20)goto _LL1A8;
else{_tmp32B=_tmp32A->f1;}};_tmp32C=_tmp322.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp32D=(struct Cyc_Absyn_Initializes_att_struct*)_tmp32C;if(_tmp32D->tag != 20)
goto _LL1A8;else{_tmp32E=_tmp32D->f1;}};_LL1A7: _tmp331=_tmp32B;_tmp334=_tmp32E;
goto _LL1A9;_LL1A8: _tmp32F=_tmp322.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp330=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp32F;if(_tmp330->tag != 6)goto _LL1AA;else{
_tmp331=_tmp330->f1;}};_tmp332=_tmp322.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp333=(struct Cyc_Absyn_Aligned_att_struct*)_tmp332;if(_tmp333->tag != 6)goto
_LL1AA;else{_tmp334=_tmp333->f1;}};_LL1A9: return Cyc_Core_intcmp(_tmp331,_tmp334);
_LL1AA: _tmp335=_tmp322.f1;{struct Cyc_Absyn_Section_att_struct*_tmp336=(struct Cyc_Absyn_Section_att_struct*)
_tmp335;if(_tmp336->tag != 8)goto _LL1AC;else{_tmp337=_tmp336->f1;}};_tmp338=
_tmp322.f2;{struct Cyc_Absyn_Section_att_struct*_tmp339=(struct Cyc_Absyn_Section_att_struct*)
_tmp338;if(_tmp339->tag != 8)goto _LL1AC;else{_tmp33A=_tmp339->f1;}};_LL1AB: return
Cyc_strcmp((struct _dyneither_ptr)_tmp337,(struct _dyneither_ptr)_tmp33A);_LL1AC:
_tmp33B=_tmp322.f1;{struct Cyc_Absyn_Format_att_struct*_tmp33C=(struct Cyc_Absyn_Format_att_struct*)
_tmp33B;if(_tmp33C->tag != 19)goto _LL1AE;else{_tmp33D=_tmp33C->f1;_tmp33E=_tmp33C->f2;
_tmp33F=_tmp33C->f3;}};_tmp340=_tmp322.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp341=(struct Cyc_Absyn_Format_att_struct*)_tmp340;if(_tmp341->tag != 19)goto
_LL1AE;else{_tmp342=_tmp341->f1;_tmp343=_tmp341->f2;_tmp344=_tmp341->f3;}};
_LL1AD: {int _tmp345=Cyc_Core_intcmp((int)((unsigned int)_tmp33D),(int)((
unsigned int)_tmp342));if(_tmp345 != 0)return _tmp345;{int _tmp346=Cyc_Core_intcmp(
_tmp33E,_tmp343);if(_tmp346 != 0)return _tmp346;return Cyc_Core_intcmp(_tmp33F,
_tmp344);};}_LL1AE:;_LL1AF: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL1A3:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct
Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return
0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp347=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp348;
enum Cyc_Absyn_KindQual _tmp349;enum Cyc_Absyn_AliasQual _tmp34A;struct Cyc_Absyn_Kind
_tmp34B;enum Cyc_Absyn_KindQual _tmp34C;enum Cyc_Absyn_AliasQual _tmp34D;struct Cyc_Absyn_Kind
_tmp34E;enum Cyc_Absyn_KindQual _tmp34F;struct Cyc_Absyn_Kind _tmp350;enum Cyc_Absyn_KindQual
_tmp351;_LL1B1: _tmp348=*_tmp347;_tmp349=_tmp348.kind;if(_tmp349 != Cyc_Absyn_RgnKind)
goto _LL1B3;_tmp34A=_tmp348.aliasqual;if(_tmp34A != Cyc_Absyn_Unique)goto _LL1B3;
_LL1B2: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B0;_LL1B3: _tmp34B=*_tmp347;
_tmp34C=_tmp34B.kind;if(_tmp34C != Cyc_Absyn_RgnKind)goto _LL1B5;_tmp34D=_tmp34B.aliasqual;
if(_tmp34D != Cyc_Absyn_Aliasable)goto _LL1B5;_LL1B4: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1B0;_LL1B5: _tmp34E=*_tmp347;_tmp34F=_tmp34E.kind;if(_tmp34F != Cyc_Absyn_EffKind)
goto _LL1B7;_LL1B6: t=Cyc_Absyn_empty_effect;goto _LL1B0;_LL1B7: _tmp350=*_tmp347;
_tmp351=_tmp350.kind;if(_tmp351 != Cyc_Absyn_IntKind)goto _LL1B9;_LL1B8:{struct Cyc_Absyn_ValueofType_struct
_tmp1192;struct Cyc_Absyn_ValueofType_struct*_tmp1191;t=(void*)((_tmp1191=
_cycalloc(sizeof(*_tmp1191)),((_tmp1191[0]=((_tmp1192.tag=19,((_tmp1192.f1=Cyc_Absyn_uint_exp(
0,0),_tmp1192)))),_tmp1191))));}goto _LL1B0;_LL1B9:;_LL1BA: t=Cyc_Absyn_sint_typ;
goto _LL1B0;_LL1B0:;}{struct _tuple15*_tmp1193;return(_tmp1193=_cycalloc(sizeof(*
_tmp1193)),((_tmp1193->f1=tv,((_tmp1193->f2=t,_tmp1193)))));};}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp355=Cyc_Tcutil_compress(t);void*_tmp360;void*_tmp362;void*
_tmp363;struct Cyc_Absyn_DatatypeInfo _tmp365;struct Cyc_List_List*_tmp366;struct
Cyc_Absyn_PtrInfo _tmp368;void*_tmp369;struct Cyc_Absyn_PtrAtts _tmp36A;void*
_tmp36B;struct Cyc_Absyn_ArrayInfo _tmp36D;void*_tmp36E;struct Cyc_List_List*
_tmp370;struct Cyc_Absyn_DatatypeFieldInfo _tmp372;struct Cyc_List_List*_tmp373;
struct Cyc_Absyn_AggrInfo _tmp375;struct Cyc_List_List*_tmp376;struct Cyc_List_List*
_tmp378;struct Cyc_Absyn_FnInfo _tmp37B;struct Cyc_List_List*_tmp37C;struct Cyc_Core_Opt*
_tmp37D;void*_tmp37E;struct Cyc_List_List*_tmp37F;struct Cyc_Absyn_VarargInfo*
_tmp380;struct Cyc_List_List*_tmp381;void*_tmp387;struct Cyc_List_List*_tmp389;
_LL1BC: {struct Cyc_Absyn_VoidType_struct*_tmp356=(struct Cyc_Absyn_VoidType_struct*)
_tmp355;if(_tmp356->tag != 0)goto _LL1BE;}_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_FloatType_struct*
_tmp357=(struct Cyc_Absyn_FloatType_struct*)_tmp355;if(_tmp357->tag != 7)goto
_LL1C0;}_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_DoubleType_struct*_tmp358=(
struct Cyc_Absyn_DoubleType_struct*)_tmp355;if(_tmp358->tag != 8)goto _LL1C2;}
_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_EnumType_struct*_tmp359=(struct Cyc_Absyn_EnumType_struct*)
_tmp355;if(_tmp359->tag != 14)goto _LL1C4;}_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp35A=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp355;if(_tmp35A->tag != 15)goto
_LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_ValueofType_struct*_tmp35B=(
struct Cyc_Absyn_ValueofType_struct*)_tmp355;if(_tmp35B->tag != 19)goto _LL1C8;}
_LL1C7: goto _LL1C9;_LL1C8: {struct Cyc_Absyn_IntType_struct*_tmp35C=(struct Cyc_Absyn_IntType_struct*)
_tmp355;if(_tmp35C->tag != 6)goto _LL1CA;}_LL1C9: return Cyc_Absyn_empty_effect;
_LL1CA: {struct Cyc_Absyn_Evar_struct*_tmp35D=(struct Cyc_Absyn_Evar_struct*)
_tmp355;if(_tmp35D->tag != 1)goto _LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_VarType_struct*
_tmp35E=(struct Cyc_Absyn_VarType_struct*)_tmp355;if(_tmp35E->tag != 2)goto _LL1CE;}
_LL1CD: {struct Cyc_Absyn_Kind*_tmp38B=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind
_tmp38C;enum Cyc_Absyn_KindQual _tmp38D;struct Cyc_Absyn_Kind _tmp38E;enum Cyc_Absyn_KindQual
_tmp38F;struct Cyc_Absyn_Kind _tmp390;enum Cyc_Absyn_KindQual _tmp391;_LL1F3:
_tmp38C=*_tmp38B;_tmp38D=_tmp38C.kind;if(_tmp38D != Cyc_Absyn_RgnKind)goto _LL1F5;
_LL1F4: {struct Cyc_Absyn_AccessEff_struct _tmp1196;struct Cyc_Absyn_AccessEff_struct*
_tmp1195;return(void*)((_tmp1195=_cycalloc(sizeof(*_tmp1195)),((_tmp1195[0]=((
_tmp1196.tag=23,((_tmp1196.f1=(void*)t,_tmp1196)))),_tmp1195))));}_LL1F5: _tmp38E=*
_tmp38B;_tmp38F=_tmp38E.kind;if(_tmp38F != Cyc_Absyn_EffKind)goto _LL1F7;_LL1F6:
return t;_LL1F7: _tmp390=*_tmp38B;_tmp391=_tmp390.kind;if(_tmp391 != Cyc_Absyn_IntKind)
goto _LL1F9;_LL1F8: return Cyc_Absyn_empty_effect;_LL1F9:;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct
_tmp1199;struct Cyc_Absyn_RgnsEff_struct*_tmp1198;return(void*)((_tmp1198=
_cycalloc(sizeof(*_tmp1198)),((_tmp1198[0]=((_tmp1199.tag=25,((_tmp1199.f1=(void*)
t,_tmp1199)))),_tmp1198))));}_LL1F2:;}_LL1CE: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp35F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp355;if(_tmp35F->tag != 16)goto
_LL1D0;else{_tmp360=(void*)_tmp35F->f1;}}_LL1CF: {struct Cyc_Absyn_AccessEff_struct
_tmp119C;struct Cyc_Absyn_AccessEff_struct*_tmp119B;return(void*)((_tmp119B=
_cycalloc(sizeof(*_tmp119B)),((_tmp119B[0]=((_tmp119C.tag=23,((_tmp119C.f1=(void*)
_tmp360,_tmp119C)))),_tmp119B))));}_LL1D0: {struct Cyc_Absyn_DynRgnType_struct*
_tmp361=(struct Cyc_Absyn_DynRgnType_struct*)_tmp355;if(_tmp361->tag != 17)goto
_LL1D2;else{_tmp362=(void*)_tmp361->f1;_tmp363=(void*)_tmp361->f2;}}_LL1D1: {
struct Cyc_Absyn_AccessEff_struct _tmp119F;struct Cyc_Absyn_AccessEff_struct*
_tmp119E;return(void*)((_tmp119E=_cycalloc(sizeof(*_tmp119E)),((_tmp119E[0]=((
_tmp119F.tag=23,((_tmp119F.f1=(void*)_tmp363,_tmp119F)))),_tmp119E))));}_LL1D2: {
struct Cyc_Absyn_DatatypeType_struct*_tmp364=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp355;if(_tmp364->tag != 3)goto _LL1D4;else{_tmp365=_tmp364->f1;_tmp366=_tmp365.targs;}}
_LL1D3: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp366);struct Cyc_Absyn_JoinEff_struct
_tmp11A2;struct Cyc_Absyn_JoinEff_struct*_tmp11A1;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11A1=_cycalloc(sizeof(*_tmp11A1)),((_tmp11A1[0]=((_tmp11A2.tag=24,((
_tmp11A2.f1=ts,_tmp11A2)))),_tmp11A1)))));}_LL1D4: {struct Cyc_Absyn_PointerType_struct*
_tmp367=(struct Cyc_Absyn_PointerType_struct*)_tmp355;if(_tmp367->tag != 5)goto
_LL1D6;else{_tmp368=_tmp367->f1;_tmp369=_tmp368.elt_typ;_tmp36A=_tmp368.ptr_atts;
_tmp36B=_tmp36A.rgn;}}_LL1D5: {struct Cyc_Absyn_JoinEff_struct _tmp11B1;struct Cyc_Absyn_AccessEff_struct*
_tmp11B0;struct Cyc_Absyn_AccessEff_struct _tmp11AF;void*_tmp11AE[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11AD;return Cyc_Tcutil_normalize_effect((void*)((_tmp11AD=_cycalloc(sizeof(*
_tmp11AD)),((_tmp11AD[0]=((_tmp11B1.tag=24,((_tmp11B1.f1=((_tmp11AE[1]=Cyc_Tcutil_rgns_of(
_tmp369),((_tmp11AE[0]=(void*)((_tmp11B0=_cycalloc(sizeof(*_tmp11B0)),((_tmp11B0[
0]=((_tmp11AF.tag=23,((_tmp11AF.f1=(void*)_tmp36B,_tmp11AF)))),_tmp11B0)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11AE,sizeof(void*),2)))))),_tmp11B1)))),_tmp11AD)))));}_LL1D6: {struct Cyc_Absyn_ArrayType_struct*
_tmp36C=(struct Cyc_Absyn_ArrayType_struct*)_tmp355;if(_tmp36C->tag != 9)goto
_LL1D8;else{_tmp36D=_tmp36C->f1;_tmp36E=_tmp36D.elt_type;}}_LL1D7: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp36E));_LL1D8: {struct Cyc_Absyn_TupleType_struct*_tmp36F=(
struct Cyc_Absyn_TupleType_struct*)_tmp355;if(_tmp36F->tag != 11)goto _LL1DA;else{
_tmp370=_tmp36F->f1;}}_LL1D9: {struct Cyc_List_List*_tmp3A1=0;for(0;_tmp370 != 0;
_tmp370=_tmp370->tl){struct Cyc_List_List*_tmp11B2;_tmp3A1=((_tmp11B2=_cycalloc(
sizeof(*_tmp11B2)),((_tmp11B2->hd=(*((struct _tuple11*)_tmp370->hd)).f2,((
_tmp11B2->tl=_tmp3A1,_tmp11B2))))));}_tmp373=_tmp3A1;goto _LL1DB;}_LL1DA: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp371=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp355;if(_tmp371->tag != 4)goto _LL1DC;else{_tmp372=_tmp371->f1;_tmp373=_tmp372.targs;}}
_LL1DB: _tmp376=_tmp373;goto _LL1DD;_LL1DC: {struct Cyc_Absyn_AggrType_struct*
_tmp374=(struct Cyc_Absyn_AggrType_struct*)_tmp355;if(_tmp374->tag != 12)goto
_LL1DE;else{_tmp375=_tmp374->f1;_tmp376=_tmp375.targs;}}_LL1DD: {struct Cyc_Absyn_JoinEff_struct
_tmp11B5;struct Cyc_Absyn_JoinEff_struct*_tmp11B4;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11B4=_cycalloc(sizeof(*_tmp11B4)),((_tmp11B4[0]=((_tmp11B5.tag=24,((
_tmp11B5.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp376),_tmp11B5)))),_tmp11B4)))));}_LL1DE: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp377=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp355;if(_tmp377->tag != 13)goto _LL1E0;else{_tmp378=_tmp377->f2;}}_LL1DF: {
struct Cyc_Absyn_JoinEff_struct _tmp11B8;struct Cyc_Absyn_JoinEff_struct*_tmp11B7;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11B7=_cycalloc(sizeof(*_tmp11B7)),((
_tmp11B7[0]=((_tmp11B8.tag=24,((_tmp11B8.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp378),_tmp11B8)))),_tmp11B7)))));}_LL1E0: {struct Cyc_Absyn_TagType_struct*
_tmp379=(struct Cyc_Absyn_TagType_struct*)_tmp355;if(_tmp379->tag != 20)goto _LL1E2;}
_LL1E1: return Cyc_Absyn_empty_effect;_LL1E2: {struct Cyc_Absyn_FnType_struct*
_tmp37A=(struct Cyc_Absyn_FnType_struct*)_tmp355;if(_tmp37A->tag != 10)goto _LL1E4;
else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37B.tvars;_tmp37D=_tmp37B.effect;_tmp37E=
_tmp37B.ret_typ;_tmp37F=_tmp37B.args;_tmp380=_tmp37B.cyc_varargs;_tmp381=_tmp37B.rgn_po;}}
_LL1E3: {void*_tmp3A7=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct
_tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp37C),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp37D))->v);return Cyc_Tcutil_normalize_effect(
_tmp3A7);}_LL1E4: {struct Cyc_Absyn_UniqueRgn_struct*_tmp382=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp355;if(_tmp382->tag != 22)goto _LL1E6;}_LL1E5: goto _LL1E7;_LL1E6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp383=(struct Cyc_Absyn_HeapRgn_struct*)_tmp355;if(_tmp383->tag != 21)goto _LL1E8;}
_LL1E7: return Cyc_Absyn_empty_effect;_LL1E8: {struct Cyc_Absyn_AccessEff_struct*
_tmp384=(struct Cyc_Absyn_AccessEff_struct*)_tmp355;if(_tmp384->tag != 23)goto
_LL1EA;}_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*_tmp385=(
struct Cyc_Absyn_JoinEff_struct*)_tmp355;if(_tmp385->tag != 24)goto _LL1EC;}_LL1EB:
return t;_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*_tmp386=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp355;if(_tmp386->tag != 25)goto _LL1EE;else{_tmp387=(void*)_tmp386->f1;}}_LL1ED:
return Cyc_Tcutil_rgns_of(_tmp387);_LL1EE: {struct Cyc_Absyn_TypedefType_struct*
_tmp388=(struct Cyc_Absyn_TypedefType_struct*)_tmp355;if(_tmp388->tag != 18)goto
_LL1F0;else{_tmp389=_tmp388->f2;}}_LL1EF: {struct Cyc_Absyn_JoinEff_struct
_tmp11BB;struct Cyc_Absyn_JoinEff_struct*_tmp11BA;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((_tmp11BA[0]=((_tmp11BB.tag=24,((
_tmp11BB.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp389),_tmp11BB)))),_tmp11BA)))));}_LL1F0: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp38A=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp355;if(_tmp38A->tag != 26)goto _LL1BB;}_LL1F1: {const char*_tmp11BE;void*
_tmp11BD;(_tmp11BD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11BE="typedecl in rgns_of",_tag_dyneither(_tmp11BE,
sizeof(char),20))),_tag_dyneither(_tmp11BD,sizeof(void*),0)));}_LL1BB:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp3AC=e;struct Cyc_List_List*_tmp3AE;
struct Cyc_List_List**_tmp3AF;void*_tmp3B1;_LL1FC: {struct Cyc_Absyn_JoinEff_struct*
_tmp3AD=(struct Cyc_Absyn_JoinEff_struct*)_tmp3AC;if(_tmp3AD->tag != 24)goto _LL1FE;
else{_tmp3AE=_tmp3AD->f1;_tmp3AF=(struct Cyc_List_List**)& _tmp3AD->f1;}}_LL1FD: {
int redo_join=0;{struct Cyc_List_List*effs=*_tmp3AF;for(0;effs != 0;effs=effs->tl){
void*_tmp3B2=(void*)effs->hd;effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp3B2));{void*_tmp3B3=(void*)effs->hd;void*_tmp3B6;_LL203: {struct Cyc_Absyn_JoinEff_struct*
_tmp3B4=(struct Cyc_Absyn_JoinEff_struct*)_tmp3B3;if(_tmp3B4->tag != 24)goto _LL205;}
_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_AccessEff_struct*_tmp3B5=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3B3;if(_tmp3B5->tag != 23)goto _LL207;else{_tmp3B6=(void*)_tmp3B5->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp3B7=(struct Cyc_Absyn_HeapRgn_struct*)_tmp3B6;if(
_tmp3B7->tag != 21)goto _LL207;};}}_LL206: redo_join=1;goto _LL202;_LL207:;_LL208:
goto _LL202;_LL202:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp3AF;for(0;effs != 0;effs=effs->tl){void*_tmp3B8=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp3BA;void*_tmp3BC;_LL20A: {struct Cyc_Absyn_JoinEff_struct*
_tmp3B9=(struct Cyc_Absyn_JoinEff_struct*)_tmp3B8;if(_tmp3B9->tag != 24)goto _LL20C;
else{_tmp3BA=_tmp3B9->f1;}}_LL20B: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp3BA,effects);goto _LL209;_LL20C: {
struct Cyc_Absyn_AccessEff_struct*_tmp3BB=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3B8;if(_tmp3BB->tag != 23)goto _LL20E;else{_tmp3BC=(void*)_tmp3BB->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp3BD=(struct Cyc_Absyn_HeapRgn_struct*)_tmp3BC;if(
_tmp3BD->tag != 21)goto _LL20E;};}}_LL20D: goto _LL209;_LL20E:;_LL20F:{struct Cyc_List_List*
_tmp11BF;effects=((_tmp11BF=_cycalloc(sizeof(*_tmp11BF)),((_tmp11BF->hd=_tmp3B8,((
_tmp11BF->tl=effects,_tmp11BF))))));}goto _LL209;_LL209:;}}*_tmp3AF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3B0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3AC;if(_tmp3B0->tag != 25)goto _LL200;
else{_tmp3B1=(void*)_tmp3B0->f1;}}_LL1FF: {void*_tmp3BF=Cyc_Tcutil_compress(
_tmp3B1);_LL211: {struct Cyc_Absyn_Evar_struct*_tmp3C0=(struct Cyc_Absyn_Evar_struct*)
_tmp3BF;if(_tmp3C0->tag != 1)goto _LL213;}_LL212: goto _LL214;_LL213: {struct Cyc_Absyn_VarType_struct*
_tmp3C1=(struct Cyc_Absyn_VarType_struct*)_tmp3BF;if(_tmp3C1->tag != 2)goto _LL215;}
_LL214: return e;_LL215:;_LL216: return Cyc_Tcutil_rgns_of(_tmp3B1);_LL210:;}_LL200:;
_LL201: return e;_LL1FB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp11C9;struct Cyc_Core_Opt*_tmp11C8;struct Cyc_Absyn_FnInfo _tmp11C7;struct Cyc_Absyn_FnType_struct*
_tmp11C6;struct Cyc_Absyn_FnType_struct*_tmp3C2=(_tmp11C6=_cycalloc(sizeof(*
_tmp11C6)),((_tmp11C6[0]=((_tmp11C9.tag=10,((_tmp11C9.f1=((_tmp11C7.tvars=0,((
_tmp11C7.effect=((_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8->v=eff,
_tmp11C8)))),((_tmp11C7.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp11C7.args=0,((
_tmp11C7.c_varargs=0,((_tmp11C7.cyc_varargs=0,((_tmp11C7.rgn_po=0,((_tmp11C7.attributes=
0,_tmp11C7)))))))))))))))),_tmp11C9)))),_tmp11C6)));return Cyc_Absyn_atb_typ((
void*)_tmp3C2,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)&
Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp3C7=Cyc_Tcutil_compress(e);void*
_tmp3C9;struct Cyc_List_List*_tmp3CB;void*_tmp3CD;struct Cyc_Core_Opt*_tmp3CF;
struct Cyc_Core_Opt*_tmp3D0;struct Cyc_Core_Opt**_tmp3D1;struct Cyc_Core_Opt*
_tmp3D2;_LL218: {struct Cyc_Absyn_AccessEff_struct*_tmp3C8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C7;if(_tmp3C8->tag != 23)goto _LL21A;else{_tmp3C9=(void*)_tmp3C8->f1;}}_LL219:
if(constrain)return Cyc_Tcutil_unify(r,_tmp3C9);_tmp3C9=Cyc_Tcutil_compress(
_tmp3C9);if(r == _tmp3C9)return 1;{struct _tuple0 _tmp11CA;struct _tuple0 _tmp3D4=(
_tmp11CA.f1=r,((_tmp11CA.f2=_tmp3C9,_tmp11CA)));void*_tmp3D5;struct Cyc_Absyn_Tvar*
_tmp3D7;void*_tmp3D8;struct Cyc_Absyn_Tvar*_tmp3DA;_LL223: _tmp3D5=_tmp3D4.f1;{
struct Cyc_Absyn_VarType_struct*_tmp3D6=(struct Cyc_Absyn_VarType_struct*)_tmp3D5;
if(_tmp3D6->tag != 2)goto _LL225;else{_tmp3D7=_tmp3D6->f1;}};_tmp3D8=_tmp3D4.f2;{
struct Cyc_Absyn_VarType_struct*_tmp3D9=(struct Cyc_Absyn_VarType_struct*)_tmp3D8;
if(_tmp3D9->tag != 2)goto _LL225;else{_tmp3DA=_tmp3D9->f1;}};_LL224: return Cyc_Absyn_tvar_cmp(
_tmp3D7,_tmp3DA)== 0;_LL225:;_LL226: return 0;_LL222:;};_LL21A: {struct Cyc_Absyn_JoinEff_struct*
_tmp3CA=(struct Cyc_Absyn_JoinEff_struct*)_tmp3C7;if(_tmp3CA->tag != 24)goto _LL21C;
else{_tmp3CB=_tmp3CA->f1;}}_LL21B: for(0;_tmp3CB != 0;_tmp3CB=_tmp3CB->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp3CB->hd))return 1;}return 0;_LL21C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3CC=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3C7;if(_tmp3CC->tag != 25)goto _LL21E;
else{_tmp3CD=(void*)_tmp3CC->f1;}}_LL21D: {void*_tmp3DB=Cyc_Tcutil_rgns_of(
_tmp3CD);void*_tmp3DD;_LL228: {struct Cyc_Absyn_RgnsEff_struct*_tmp3DC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3DB;if(_tmp3DC->tag != 25)goto _LL22A;else{_tmp3DD=(void*)_tmp3DC->f1;}}_LL229:
if(!constrain)return 0;{void*_tmp3DE=Cyc_Tcutil_compress(_tmp3DD);struct Cyc_Core_Opt*
_tmp3E0;struct Cyc_Core_Opt*_tmp3E1;struct Cyc_Core_Opt**_tmp3E2;struct Cyc_Core_Opt*
_tmp3E3;_LL22D: {struct Cyc_Absyn_Evar_struct*_tmp3DF=(struct Cyc_Absyn_Evar_struct*)
_tmp3DE;if(_tmp3DF->tag != 1)goto _LL22F;else{_tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;
_tmp3E2=(struct Cyc_Core_Opt**)& _tmp3DF->f2;_tmp3E3=_tmp3DF->f4;}}_LL22E: {void*
_tmp3E4=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3E3);Cyc_Tcutil_occurs(
_tmp3E4,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3E3))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp11D9;struct Cyc_Absyn_AccessEff_struct*
_tmp11D8;struct Cyc_Absyn_AccessEff_struct _tmp11D7;void*_tmp11D6[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11D5;void*_tmp3E5=Cyc_Tcutil_dummy_fntype((void*)((_tmp11D5=_cycalloc(
sizeof(*_tmp11D5)),((_tmp11D5[0]=((_tmp11D9.tag=24,((_tmp11D9.f1=((_tmp11D6[1]=(
void*)((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8[0]=((_tmp11D7.tag=23,((
_tmp11D7.f1=(void*)r,_tmp11D7)))),_tmp11D8)))),((_tmp11D6[0]=_tmp3E4,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11D6,sizeof(void*),2)))))),
_tmp11D9)))),_tmp11D5)))));{struct Cyc_Core_Opt*_tmp11DA;*_tmp3E2=((_tmp11DA=
_cycalloc(sizeof(*_tmp11DA)),((_tmp11DA->v=_tmp3E5,_tmp11DA))));}return 1;};}
_LL22F:;_LL230: return 0;_LL22C:;};_LL22A:;_LL22B: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3DB);_LL227:;}_LL21E: {struct Cyc_Absyn_Evar_struct*_tmp3CE=(
struct Cyc_Absyn_Evar_struct*)_tmp3C7;if(_tmp3CE->tag != 1)goto _LL220;else{_tmp3CF=
_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;_tmp3D1=(struct Cyc_Core_Opt**)& _tmp3CE->f2;
_tmp3D2=_tmp3CE->f4;}}_LL21F: if(_tmp3CF == 0  || ((struct Cyc_Absyn_Kind*)_tmp3CF->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp11DD;void*_tmp11DC;(_tmp11DC=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11DD="effect evar has wrong kind",
_tag_dyneither(_tmp11DD,sizeof(char),27))),_tag_dyneither(_tmp11DC,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3D2);Cyc_Tcutil_occurs(_tmp3EE,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D2))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp11F2;struct Cyc_List_List*_tmp11F1;struct Cyc_Absyn_AccessEff_struct _tmp11F0;
struct Cyc_Absyn_AccessEff_struct*_tmp11EF;struct Cyc_List_List*_tmp11EE;struct Cyc_Absyn_JoinEff_struct*
_tmp11ED;struct Cyc_Absyn_JoinEff_struct*_tmp3EF=(_tmp11ED=_cycalloc(sizeof(*
_tmp11ED)),((_tmp11ED[0]=((_tmp11F2.tag=24,((_tmp11F2.f1=((_tmp11EE=_cycalloc(
sizeof(*_tmp11EE)),((_tmp11EE->hd=_tmp3EE,((_tmp11EE->tl=((_tmp11F1=_cycalloc(
sizeof(*_tmp11F1)),((_tmp11F1->hd=(void*)((_tmp11EF=_cycalloc(sizeof(*_tmp11EF)),((
_tmp11EF[0]=((_tmp11F0.tag=23,((_tmp11F0.f1=(void*)r,_tmp11F0)))),_tmp11EF)))),((
_tmp11F1->tl=0,_tmp11F1)))))),_tmp11EE)))))),_tmp11F2)))),_tmp11ED)));{struct Cyc_Core_Opt*
_tmp11F3;*_tmp3D1=((_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3->v=(void*)
_tmp3EF,_tmp11F3))));}return 1;};};_LL220:;_LL221: return 0;_LL217:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp3F7=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp3FA;void*_tmp3FC;struct Cyc_Core_Opt*
_tmp3FE;struct Cyc_Core_Opt*_tmp3FF;struct Cyc_Core_Opt**_tmp400;struct Cyc_Core_Opt*
_tmp401;_LL232: {struct Cyc_Absyn_AccessEff_struct*_tmp3F8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3F7;if(_tmp3F8->tag != 23)goto _LL234;}_LL233: return 0;_LL234: {struct Cyc_Absyn_JoinEff_struct*
_tmp3F9=(struct Cyc_Absyn_JoinEff_struct*)_tmp3F7;if(_tmp3F9->tag != 24)goto _LL236;
else{_tmp3FA=_tmp3F9->f1;}}_LL235: for(0;_tmp3FA != 0;_tmp3FA=_tmp3FA->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp3FA->hd))return 1;}return 0;_LL236: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3FB=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3F7;if(_tmp3FB->tag != 25)goto _LL238;
else{_tmp3FC=(void*)_tmp3FB->f1;}}_LL237: _tmp3FC=Cyc_Tcutil_compress(_tmp3FC);
if(t == _tmp3FC)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3FC);{
void*_tmp402=Cyc_Tcutil_rgns_of(t);void*_tmp404;_LL23D: {struct Cyc_Absyn_RgnsEff_struct*
_tmp403=(struct Cyc_Absyn_RgnsEff_struct*)_tmp402;if(_tmp403->tag != 25)goto _LL23F;
else{_tmp404=(void*)_tmp403->f1;}}_LL23E: {struct _tuple0 _tmp11F4;struct _tuple0
_tmp406=(_tmp11F4.f1=Cyc_Tcutil_compress(_tmp404),((_tmp11F4.f2=_tmp3FC,_tmp11F4)));
void*_tmp407;struct Cyc_Absyn_Tvar*_tmp409;void*_tmp40A;struct Cyc_Absyn_Tvar*
_tmp40C;_LL242: _tmp407=_tmp406.f1;{struct Cyc_Absyn_VarType_struct*_tmp408=(
struct Cyc_Absyn_VarType_struct*)_tmp407;if(_tmp408->tag != 2)goto _LL244;else{
_tmp409=_tmp408->f1;}};_tmp40A=_tmp406.f2;{struct Cyc_Absyn_VarType_struct*
_tmp40B=(struct Cyc_Absyn_VarType_struct*)_tmp40A;if(_tmp40B->tag != 2)goto _LL244;
else{_tmp40C=_tmp40B->f1;}};_LL243: return Cyc_Tcutil_unify(t,_tmp3FC);_LL244:;
_LL245: return _tmp404 == _tmp3FC;_LL241:;}_LL23F:;_LL240: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp402);_LL23C:;};_LL238: {struct Cyc_Absyn_Evar_struct*
_tmp3FD=(struct Cyc_Absyn_Evar_struct*)_tmp3F7;if(_tmp3FD->tag != 1)goto _LL23A;
else{_tmp3FE=_tmp3FD->f1;_tmp3FF=_tmp3FD->f2;_tmp400=(struct Cyc_Core_Opt**)&
_tmp3FD->f2;_tmp401=_tmp3FD->f4;}}_LL239: if(_tmp3FE == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3FE->v)->kind != Cyc_Absyn_EffKind){const char*_tmp11F7;void*_tmp11F6;(
_tmp11F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11F7="effect evar has wrong kind",_tag_dyneither(_tmp11F7,sizeof(char),27))),
_tag_dyneither(_tmp11F6,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp40F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp401);
Cyc_Tcutil_occurs(_tmp40F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp401))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp120C;struct Cyc_List_List*
_tmp120B;struct Cyc_Absyn_RgnsEff_struct _tmp120A;struct Cyc_Absyn_RgnsEff_struct*
_tmp1209;struct Cyc_List_List*_tmp1208;struct Cyc_Absyn_JoinEff_struct*_tmp1207;
struct Cyc_Absyn_JoinEff_struct*_tmp410=(_tmp1207=_cycalloc(sizeof(*_tmp1207)),((
_tmp1207[0]=((_tmp120C.tag=24,((_tmp120C.f1=((_tmp1208=_cycalloc(sizeof(*
_tmp1208)),((_tmp1208->hd=_tmp40F,((_tmp1208->tl=((_tmp120B=_cycalloc(sizeof(*
_tmp120B)),((_tmp120B->hd=(void*)((_tmp1209=_cycalloc(sizeof(*_tmp1209)),((
_tmp1209[0]=((_tmp120A.tag=25,((_tmp120A.f1=(void*)t,_tmp120A)))),_tmp1209)))),((
_tmp120B->tl=0,_tmp120B)))))),_tmp1208)))))),_tmp120C)))),_tmp1207)));{struct Cyc_Core_Opt*
_tmp120D;*_tmp400=((_tmp120D=_cycalloc(sizeof(*_tmp120D)),((_tmp120D->v=(void*)
_tmp410,_tmp120D))));}return 1;};};_LL23A:;_LL23B: return 0;_LL231:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp418=e;struct Cyc_Absyn_Tvar*_tmp41A;struct Cyc_List_List*_tmp41C;void*
_tmp41E;struct Cyc_Core_Opt*_tmp420;struct Cyc_Core_Opt*_tmp421;struct Cyc_Core_Opt**
_tmp422;struct Cyc_Core_Opt*_tmp423;_LL247: {struct Cyc_Absyn_VarType_struct*
_tmp419=(struct Cyc_Absyn_VarType_struct*)_tmp418;if(_tmp419->tag != 2)goto _LL249;
else{_tmp41A=_tmp419->f1;}}_LL248: return Cyc_Absyn_tvar_cmp(v,_tmp41A)== 0;_LL249: {
struct Cyc_Absyn_JoinEff_struct*_tmp41B=(struct Cyc_Absyn_JoinEff_struct*)_tmp418;
if(_tmp41B->tag != 24)goto _LL24B;else{_tmp41C=_tmp41B->f1;}}_LL24A: for(0;_tmp41C
!= 0;_tmp41C=_tmp41C->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp41C->hd))return 1;}return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp41D=(struct Cyc_Absyn_RgnsEff_struct*)_tmp418;if(_tmp41D->tag != 25)goto _LL24D;
else{_tmp41E=(void*)_tmp41D->f1;}}_LL24C: {void*_tmp424=Cyc_Tcutil_rgns_of(
_tmp41E);void*_tmp426;_LL252: {struct Cyc_Absyn_RgnsEff_struct*_tmp425=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp424;if(_tmp425->tag != 25)goto _LL254;else{_tmp426=(void*)_tmp425->f1;}}_LL253:
if(!may_constrain_evars)return 0;{void*_tmp427=Cyc_Tcutil_compress(_tmp426);
struct Cyc_Core_Opt*_tmp429;struct Cyc_Core_Opt*_tmp42A;struct Cyc_Core_Opt**
_tmp42B;struct Cyc_Core_Opt*_tmp42C;_LL257: {struct Cyc_Absyn_Evar_struct*_tmp428=(
struct Cyc_Absyn_Evar_struct*)_tmp427;if(_tmp428->tag != 1)goto _LL259;else{_tmp429=
_tmp428->f1;_tmp42A=_tmp428->f2;_tmp42B=(struct Cyc_Core_Opt**)& _tmp428->f2;
_tmp42C=_tmp428->f4;}}_LL258: {void*_tmp42D=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp42C);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp42C))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp121C;struct Cyc_Absyn_VarType_struct*_tmp121B;
struct Cyc_Absyn_VarType_struct _tmp121A;void*_tmp1219[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1218;void*_tmp42E=Cyc_Tcutil_dummy_fntype((void*)((_tmp1218=_cycalloc(
sizeof(*_tmp1218)),((_tmp1218[0]=((_tmp121C.tag=24,((_tmp121C.f1=((_tmp1219[1]=(
void*)((_tmp121B=_cycalloc(sizeof(*_tmp121B)),((_tmp121B[0]=((_tmp121A.tag=2,((
_tmp121A.f1=v,_tmp121A)))),_tmp121B)))),((_tmp1219[0]=_tmp42D,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1219,sizeof(void*),2)))))),
_tmp121C)))),_tmp1218)))));{struct Cyc_Core_Opt*_tmp121D;*_tmp42B=((_tmp121D=
_cycalloc(sizeof(*_tmp121D)),((_tmp121D->v=_tmp42E,_tmp121D))));}return 1;};}
_LL259:;_LL25A: return 0;_LL256:;};_LL254:;_LL255: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp424);_LL251:;}_LL24D: {struct Cyc_Absyn_Evar_struct*
_tmp41F=(struct Cyc_Absyn_Evar_struct*)_tmp418;if(_tmp41F->tag != 1)goto _LL24F;
else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;_tmp422=(struct Cyc_Core_Opt**)&
_tmp41F->f2;_tmp423=_tmp41F->f4;}}_LL24E: if(_tmp420 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp420->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1220;void*_tmp121F;(
_tmp121F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1220="effect evar has wrong kind",_tag_dyneither(_tmp1220,sizeof(char),27))),
_tag_dyneither(_tmp121F,sizeof(void*),0)));}{void*_tmp437=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp423);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp423))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp1235;
struct Cyc_List_List*_tmp1234;struct Cyc_Absyn_VarType_struct _tmp1233;struct Cyc_Absyn_VarType_struct*
_tmp1232;struct Cyc_List_List*_tmp1231;struct Cyc_Absyn_JoinEff_struct*_tmp1230;
struct Cyc_Absyn_JoinEff_struct*_tmp438=(_tmp1230=_cycalloc(sizeof(*_tmp1230)),((
_tmp1230[0]=((_tmp1235.tag=24,((_tmp1235.f1=((_tmp1231=_cycalloc(sizeof(*
_tmp1231)),((_tmp1231->hd=_tmp437,((_tmp1231->tl=((_tmp1234=_cycalloc(sizeof(*
_tmp1234)),((_tmp1234->hd=(void*)((_tmp1232=_cycalloc(sizeof(*_tmp1232)),((
_tmp1232[0]=((_tmp1233.tag=2,((_tmp1233.f1=v,_tmp1233)))),_tmp1232)))),((
_tmp1234->tl=0,_tmp1234)))))),_tmp1231)))))),_tmp1235)))),_tmp1230)));{struct Cyc_Core_Opt*
_tmp1236;*_tmp422=((_tmp1236=_cycalloc(sizeof(*_tmp1236)),((_tmp1236->v=(void*)
_tmp438,_tmp1236))));}return 1;};};_LL24F:;_LL250: return 0;_LL246:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp440=e;struct Cyc_List_List*
_tmp442;void*_tmp444;_LL25C: {struct Cyc_Absyn_JoinEff_struct*_tmp441=(struct Cyc_Absyn_JoinEff_struct*)
_tmp440;if(_tmp441->tag != 24)goto _LL25E;else{_tmp442=_tmp441->f1;}}_LL25D: for(0;
_tmp442 != 0;_tmp442=_tmp442->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp442->hd))return 1;}return 0;_LL25E: {struct Cyc_Absyn_RgnsEff_struct*_tmp443=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp440;if(_tmp443->tag != 25)goto _LL260;else{
_tmp444=(void*)_tmp443->f1;}}_LL25F: {void*_tmp446=Cyc_Tcutil_rgns_of(_tmp444);
void*_tmp448;_LL265: {struct Cyc_Absyn_RgnsEff_struct*_tmp447=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp446;if(_tmp447->tag != 25)goto _LL267;else{_tmp448=(void*)_tmp447->f1;}}_LL266:
return 0;_LL267:;_LL268: return Cyc_Tcutil_evar_in_effect(evar,_tmp446);_LL264:;}
_LL260: {struct Cyc_Absyn_Evar_struct*_tmp445=(struct Cyc_Absyn_Evar_struct*)
_tmp440;if(_tmp445->tag != 1)goto _LL262;}_LL261: return evar == e;_LL262:;_LL263:
return 0;_LL25B:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2){void*_tmp449=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp44B;void*
_tmp44D;struct Cyc_Absyn_Tvar*_tmp44F;void*_tmp451;struct Cyc_Core_Opt*_tmp453;
struct Cyc_Core_Opt**_tmp454;struct Cyc_Core_Opt*_tmp455;_LL26A: {struct Cyc_Absyn_JoinEff_struct*
_tmp44A=(struct Cyc_Absyn_JoinEff_struct*)_tmp449;if(_tmp44A->tag != 24)goto _LL26C;
else{_tmp44B=_tmp44A->f1;}}_LL26B: for(0;_tmp44B != 0;_tmp44B=_tmp44B->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp44B->hd,e2))return 0;}return 1;_LL26C: {struct Cyc_Absyn_AccessEff_struct*
_tmp44C=(struct Cyc_Absyn_AccessEff_struct*)_tmp449;if(_tmp44C->tag != 23)goto
_LL26E;else{_tmp44D=(void*)_tmp44C->f1;}}_LL26D: return Cyc_Tcutil_region_in_effect(
0,_tmp44D,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp44D,(void*)& Cyc_Absyn_HeapRgn_val);
_LL26E: {struct Cyc_Absyn_VarType_struct*_tmp44E=(struct Cyc_Absyn_VarType_struct*)
_tmp449;if(_tmp44E->tag != 2)goto _LL270;else{_tmp44F=_tmp44E->f1;}}_LL26F: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp44F,e2);_LL270: {struct Cyc_Absyn_RgnsEff_struct*
_tmp450=(struct Cyc_Absyn_RgnsEff_struct*)_tmp449;if(_tmp450->tag != 25)goto _LL272;
else{_tmp451=(void*)_tmp450->f1;}}_LL271: {void*_tmp456=Cyc_Tcutil_rgns_of(
_tmp451);void*_tmp458;_LL277: {struct Cyc_Absyn_RgnsEff_struct*_tmp457=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp456;if(_tmp457->tag != 25)goto _LL279;else{_tmp458=(void*)_tmp457->f1;}}_LL278:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp458,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp458,Cyc_Absyn_sint_typ);_LL279:;
_LL27A: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp456,e2);_LL276:;}
_LL272: {struct Cyc_Absyn_Evar_struct*_tmp452=(struct Cyc_Absyn_Evar_struct*)
_tmp449;if(_tmp452->tag != 1)goto _LL274;else{_tmp453=_tmp452->f2;_tmp454=(struct
Cyc_Core_Opt**)& _tmp452->f2;_tmp455=_tmp452->f4;}}_LL273: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp1237;*_tmp454=((_tmp1237=_cycalloc(sizeof(*
_tmp1237)),((_tmp1237->v=Cyc_Absyn_empty_effect,_tmp1237))));}return 1;_LL274:;
_LL275: {const char*_tmp123B;void*_tmp123A[1];struct Cyc_String_pa_struct _tmp1239;(
_tmp1239.tag=0,((_tmp1239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp123A[0]=& _tmp1239,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp123B="subset_effect: bad effect: %s",
_tag_dyneither(_tmp123B,sizeof(char),30))),_tag_dyneither(_tmp123A,sizeof(void*),
1)))))));}_LL269:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp45E;void*_tmp45F;void*_tmp460;struct
_tuple0*_tmp45D=(struct _tuple0*)r1->hd;_tmp45E=*_tmp45D;_tmp45F=_tmp45E.f1;
_tmp460=_tmp45E.f2;{int found=_tmp45F == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp462;void*_tmp463;void*
_tmp464;struct _tuple0*_tmp461=(struct _tuple0*)r2->hd;_tmp462=*_tmp461;_tmp463=
_tmp462.f1;_tmp464=_tmp462.f2;if(Cyc_Tcutil_unify(_tmp45F,_tmp463) && Cyc_Tcutil_unify(
_tmp460,_tmp464)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp465=t1;struct Cyc_Core_Opt*_tmp467;struct Cyc_Core_Opt*
_tmp468;struct Cyc_Core_Opt**_tmp469;struct Cyc_Core_Opt*_tmp46A;_LL27C: {struct
Cyc_Absyn_Evar_struct*_tmp466=(struct Cyc_Absyn_Evar_struct*)_tmp465;if(_tmp466->tag
!= 1)goto _LL27E;else{_tmp467=_tmp466->f1;_tmp468=_tmp466->f2;_tmp469=(struct Cyc_Core_Opt**)&
_tmp466->f2;_tmp46A=_tmp466->f4;}}_LL27D: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp46A))->v,t2);{struct
Cyc_Absyn_Kind*_tmp46B=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp46B,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp467))->v)){{struct
Cyc_Core_Opt*_tmp123C;*_tmp469=((_tmp123C=_cycalloc(sizeof(*_tmp123C)),((
_tmp123C->v=t2,_tmp123C))));}return;}else{{void*_tmp46D=t2;struct Cyc_Core_Opt*
_tmp46F;struct Cyc_Core_Opt**_tmp470;struct Cyc_Core_Opt*_tmp471;struct Cyc_Absyn_PtrInfo
_tmp473;_LL281: {struct Cyc_Absyn_Evar_struct*_tmp46E=(struct Cyc_Absyn_Evar_struct*)
_tmp46D;if(_tmp46E->tag != 1)goto _LL283;else{_tmp46F=_tmp46E->f2;_tmp470=(struct
Cyc_Core_Opt**)& _tmp46E->f2;_tmp471=_tmp46E->f4;}}_LL282: {struct Cyc_List_List*
_tmp474=(struct Cyc_List_List*)_tmp46A->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp471))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp474,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp123D;Cyc_Tcutil_failure_reason=((
_tmp123D="(type variable would escape scope)",_tag_dyneither(_tmp123D,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp467->v,_tmp46B)){{struct Cyc_Core_Opt*_tmp123E;*_tmp470=((
_tmp123E=_cycalloc(sizeof(*_tmp123E)),((_tmp123E->v=t1,_tmp123E))));}return;}{
const char*_tmp123F;Cyc_Tcutil_failure_reason=((_tmp123F="(kinds are incompatible)",
_tag_dyneither(_tmp123F,sizeof(char),25)));}goto _LL280;}_LL283:{struct Cyc_Absyn_PointerType_struct*
_tmp472=(struct Cyc_Absyn_PointerType_struct*)_tmp46D;if(_tmp472->tag != 5)goto
_LL285;else{_tmp473=_tmp472->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp467->v)->kind
== Cyc_Absyn_BoxKind))goto _LL285;_LL284: {union Cyc_Absyn_Constraint*_tmp478=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp473.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp479=_tmp478;union Cyc_Absyn_Constraint
_tmp47A;int _tmp47B;_LL288: _tmp47A=*_tmp479;if((_tmp47A.No_constr).tag != 3)goto
_LL28A;_tmp47B=(int)(_tmp47A.No_constr).val;_LL289:{struct
_union_Constraint_Eq_constr*_tmp1240;(_tmp1240=& _tmp478->Eq_constr,((_tmp1240->tag=
1,_tmp1240->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp1241;*_tmp469=((
_tmp1241=_cycalloc(sizeof(*_tmp1241)),((_tmp1241->v=t2,_tmp1241))));}return;
_LL28A:;_LL28B: goto _LL287;_LL287:;}goto _LL280;}_LL285:;_LL286: goto _LL280;_LL280:;}{
const char*_tmp1242;Cyc_Tcutil_failure_reason=((_tmp1242="(kinds are incompatible)",
_tag_dyneither(_tmp1242,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL27E:;_LL27F: goto _LL27B;_LL27B:;}{struct _tuple0 _tmp1243;struct _tuple0 _tmp480=(
_tmp1243.f1=t2,((_tmp1243.f2=t1,_tmp1243)));void*_tmp481;void*_tmp483;void*
_tmp485;void*_tmp487;struct Cyc_Absyn_Tvar*_tmp489;void*_tmp48A;struct Cyc_Absyn_Tvar*
_tmp48C;void*_tmp48D;struct Cyc_Absyn_AggrInfo _tmp48F;union Cyc_Absyn_AggrInfoU
_tmp490;struct Cyc_List_List*_tmp491;void*_tmp492;struct Cyc_Absyn_AggrInfo _tmp494;
union Cyc_Absyn_AggrInfoU _tmp495;struct Cyc_List_List*_tmp496;void*_tmp497;struct
_tuple2*_tmp499;void*_tmp49A;struct _tuple2*_tmp49C;void*_tmp49D;struct Cyc_List_List*
_tmp49F;void*_tmp4A0;struct Cyc_List_List*_tmp4A2;void*_tmp4A3;struct Cyc_Absyn_DatatypeInfo
_tmp4A5;union Cyc_Absyn_DatatypeInfoU _tmp4A6;struct Cyc_Absyn_Datatypedecl**
_tmp4A7;struct Cyc_Absyn_Datatypedecl*_tmp4A8;struct Cyc_List_List*_tmp4A9;void*
_tmp4AA;struct Cyc_Absyn_DatatypeInfo _tmp4AC;union Cyc_Absyn_DatatypeInfoU _tmp4AD;
struct Cyc_Absyn_Datatypedecl**_tmp4AE;struct Cyc_Absyn_Datatypedecl*_tmp4AF;
struct Cyc_List_List*_tmp4B0;void*_tmp4B1;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4B3;union Cyc_Absyn_DatatypeFieldInfoU _tmp4B4;struct _tuple3 _tmp4B5;struct Cyc_Absyn_Datatypedecl*
_tmp4B6;struct Cyc_Absyn_Datatypefield*_tmp4B7;struct Cyc_List_List*_tmp4B8;void*
_tmp4B9;struct Cyc_Absyn_DatatypeFieldInfo _tmp4BB;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4BC;struct _tuple3 _tmp4BD;struct Cyc_Absyn_Datatypedecl*_tmp4BE;struct Cyc_Absyn_Datatypefield*
_tmp4BF;struct Cyc_List_List*_tmp4C0;void*_tmp4C1;struct Cyc_Absyn_PtrInfo _tmp4C3;
void*_tmp4C4;struct Cyc_Absyn_Tqual _tmp4C5;struct Cyc_Absyn_PtrAtts _tmp4C6;void*
_tmp4C7;union Cyc_Absyn_Constraint*_tmp4C8;union Cyc_Absyn_Constraint*_tmp4C9;
union Cyc_Absyn_Constraint*_tmp4CA;void*_tmp4CB;struct Cyc_Absyn_PtrInfo _tmp4CD;
void*_tmp4CE;struct Cyc_Absyn_Tqual _tmp4CF;struct Cyc_Absyn_PtrAtts _tmp4D0;void*
_tmp4D1;union Cyc_Absyn_Constraint*_tmp4D2;union Cyc_Absyn_Constraint*_tmp4D3;
union Cyc_Absyn_Constraint*_tmp4D4;void*_tmp4D5;enum Cyc_Absyn_Sign _tmp4D7;enum 
Cyc_Absyn_Size_of _tmp4D8;void*_tmp4D9;enum Cyc_Absyn_Sign _tmp4DB;enum Cyc_Absyn_Size_of
_tmp4DC;void*_tmp4DD;void*_tmp4DF;void*_tmp4E1;int _tmp4E3;void*_tmp4E4;int
_tmp4E6;void*_tmp4E7;void*_tmp4E9;void*_tmp4EA;void*_tmp4EC;void*_tmp4ED;struct
Cyc_Absyn_Exp*_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F2;void*_tmp4F3;
struct Cyc_Absyn_ArrayInfo _tmp4F5;void*_tmp4F6;struct Cyc_Absyn_Tqual _tmp4F7;
struct Cyc_Absyn_Exp*_tmp4F8;union Cyc_Absyn_Constraint*_tmp4F9;void*_tmp4FA;
struct Cyc_Absyn_ArrayInfo _tmp4FC;void*_tmp4FD;struct Cyc_Absyn_Tqual _tmp4FE;
struct Cyc_Absyn_Exp*_tmp4FF;union Cyc_Absyn_Constraint*_tmp500;void*_tmp501;
struct Cyc_Absyn_FnInfo _tmp503;struct Cyc_List_List*_tmp504;struct Cyc_Core_Opt*
_tmp505;void*_tmp506;struct Cyc_List_List*_tmp507;int _tmp508;struct Cyc_Absyn_VarargInfo*
_tmp509;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*_tmp50B;void*_tmp50C;
struct Cyc_Absyn_FnInfo _tmp50E;struct Cyc_List_List*_tmp50F;struct Cyc_Core_Opt*
_tmp510;void*_tmp511;struct Cyc_List_List*_tmp512;int _tmp513;struct Cyc_Absyn_VarargInfo*
_tmp514;struct Cyc_List_List*_tmp515;struct Cyc_List_List*_tmp516;void*_tmp517;
struct Cyc_List_List*_tmp519;void*_tmp51A;struct Cyc_List_List*_tmp51C;void*
_tmp51D;enum Cyc_Absyn_AggrKind _tmp51F;struct Cyc_List_List*_tmp520;void*_tmp521;
enum Cyc_Absyn_AggrKind _tmp523;struct Cyc_List_List*_tmp524;void*_tmp525;void*
_tmp527;void*_tmp529;void*_tmp52B;void*_tmp52D;void*_tmp52F;void*_tmp530;void*
_tmp532;void*_tmp533;void*_tmp535;void*_tmp536;void*_tmp537;void*_tmp539;void*
_tmp53A;void*_tmp53B;void*_tmp53D;void*_tmp53F;void*_tmp541;void*_tmp543;void*
_tmp545;_LL28D: _tmp481=_tmp480.f1;{struct Cyc_Absyn_Evar_struct*_tmp482=(struct
Cyc_Absyn_Evar_struct*)_tmp481;if(_tmp482->tag != 1)goto _LL28F;};_LL28E: Cyc_Tcutil_unify_it(
t2,t1);return;_LL28F: _tmp483=_tmp480.f1;{struct Cyc_Absyn_VoidType_struct*_tmp484=(
struct Cyc_Absyn_VoidType_struct*)_tmp483;if(_tmp484->tag != 0)goto _LL291;};
_tmp485=_tmp480.f2;{struct Cyc_Absyn_VoidType_struct*_tmp486=(struct Cyc_Absyn_VoidType_struct*)
_tmp485;if(_tmp486->tag != 0)goto _LL291;};_LL290: return;_LL291: _tmp487=_tmp480.f1;{
struct Cyc_Absyn_VarType_struct*_tmp488=(struct Cyc_Absyn_VarType_struct*)_tmp487;
if(_tmp488->tag != 2)goto _LL293;else{_tmp489=_tmp488->f1;}};_tmp48A=_tmp480.f2;{
struct Cyc_Absyn_VarType_struct*_tmp48B=(struct Cyc_Absyn_VarType_struct*)_tmp48A;
if(_tmp48B->tag != 2)goto _LL293;else{_tmp48C=_tmp48B->f1;}};_LL292: {struct
_dyneither_ptr*_tmp547=_tmp489->name;struct _dyneither_ptr*_tmp548=_tmp48C->name;
int _tmp549=_tmp489->identity;int _tmp54A=_tmp48C->identity;if(_tmp54A == _tmp549)
return;{const char*_tmp1244;Cyc_Tcutil_failure_reason=((_tmp1244="(variable types are not the same)",
_tag_dyneither(_tmp1244,sizeof(char),34)));}goto _LL28C;}_LL293: _tmp48D=_tmp480.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp48E=(struct Cyc_Absyn_AggrType_struct*)
_tmp48D;if(_tmp48E->tag != 12)goto _LL295;else{_tmp48F=_tmp48E->f1;_tmp490=_tmp48F.aggr_info;
_tmp491=_tmp48F.targs;}};_tmp492=_tmp480.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp493=(struct Cyc_Absyn_AggrType_struct*)_tmp492;if(_tmp493->tag != 12)goto
_LL295;else{_tmp494=_tmp493->f1;_tmp495=_tmp494.aggr_info;_tmp496=_tmp494.targs;}};
_LL294: {enum Cyc_Absyn_AggrKind _tmp54D;struct _tuple2*_tmp54E;struct _tuple12
_tmp54C=Cyc_Absyn_aggr_kinded_name(_tmp495);_tmp54D=_tmp54C.f1;_tmp54E=_tmp54C.f2;{
enum Cyc_Absyn_AggrKind _tmp550;struct _tuple2*_tmp551;struct _tuple12 _tmp54F=Cyc_Absyn_aggr_kinded_name(
_tmp490);_tmp550=_tmp54F.f1;_tmp551=_tmp54F.f2;if(_tmp54D != _tmp550){{const char*
_tmp1245;Cyc_Tcutil_failure_reason=((_tmp1245="(struct and union type)",
_tag_dyneither(_tmp1245,sizeof(char),24)));}goto _LL28C;}if(Cyc_Absyn_qvar_cmp(
_tmp54E,_tmp551)!= 0){{const char*_tmp1246;Cyc_Tcutil_failure_reason=((_tmp1246="(different type name)",
_tag_dyneither(_tmp1246,sizeof(char),22)));}goto _LL28C;}Cyc_Tcutil_unify_list(
_tmp496,_tmp491);return;};}_LL295: _tmp497=_tmp480.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp498=(struct Cyc_Absyn_EnumType_struct*)_tmp497;if(_tmp498->tag != 14)goto
_LL297;else{_tmp499=_tmp498->f1;}};_tmp49A=_tmp480.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp49B=(struct Cyc_Absyn_EnumType_struct*)_tmp49A;if(_tmp49B->tag != 14)goto
_LL297;else{_tmp49C=_tmp49B->f1;}};_LL296: if(Cyc_Absyn_qvar_cmp(_tmp499,_tmp49C)
== 0)return;{const char*_tmp1247;Cyc_Tcutil_failure_reason=((_tmp1247="(different enum types)",
_tag_dyneither(_tmp1247,sizeof(char),23)));}goto _LL28C;_LL297: _tmp49D=_tmp480.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp49E=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp49D;if(_tmp49E->tag != 15)goto _LL299;else{_tmp49F=_tmp49E->f1;}};_tmp4A0=
_tmp480.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4A1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4A0;if(_tmp4A1->tag != 15)goto _LL299;else{_tmp4A2=_tmp4A1->f1;}};_LL298: {int
bad=0;for(0;_tmp49F != 0  && _tmp4A2 != 0;(_tmp49F=_tmp49F->tl,_tmp4A2=_tmp4A2->tl)){
struct Cyc_Absyn_Enumfield*_tmp555=(struct Cyc_Absyn_Enumfield*)_tmp49F->hd;struct
Cyc_Absyn_Enumfield*_tmp556=(struct Cyc_Absyn_Enumfield*)_tmp4A2->hd;if(Cyc_Absyn_qvar_cmp(
_tmp555->name,_tmp556->name)!= 0){{const char*_tmp1248;Cyc_Tcutil_failure_reason=((
_tmp1248="(different names for enum fields)",_tag_dyneither(_tmp1248,sizeof(char),
34)));}bad=1;break;}if(_tmp555->tag == _tmp556->tag)continue;if(_tmp555->tag == 0
 || _tmp556->tag == 0){{const char*_tmp1249;Cyc_Tcutil_failure_reason=((_tmp1249="(different tag values for enum fields)",
_tag_dyneither(_tmp1249,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp555->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp556->tag))){{const char*_tmp124A;Cyc_Tcutil_failure_reason=((_tmp124A="(different tag values for enum fields)",
_tag_dyneither(_tmp124A,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL28C;if(
_tmp49F == 0  && _tmp4A2 == 0)return;{const char*_tmp124B;Cyc_Tcutil_failure_reason=((
_tmp124B="(different number of fields for enums)",_tag_dyneither(_tmp124B,
sizeof(char),39)));}goto _LL28C;}_LL299: _tmp4A3=_tmp480.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4A4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4A3;if(_tmp4A4->tag != 3)goto
_LL29B;else{_tmp4A5=_tmp4A4->f1;_tmp4A6=_tmp4A5.datatype_info;if((_tmp4A6.KnownDatatype).tag
!= 2)goto _LL29B;_tmp4A7=(struct Cyc_Absyn_Datatypedecl**)(_tmp4A6.KnownDatatype).val;
_tmp4A8=*_tmp4A7;_tmp4A9=_tmp4A5.targs;}};_tmp4AA=_tmp480.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4AB=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4AA;if(_tmp4AB->tag != 3)goto
_LL29B;else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AC.datatype_info;if((_tmp4AD.KnownDatatype).tag
!= 2)goto _LL29B;_tmp4AE=(struct Cyc_Absyn_Datatypedecl**)(_tmp4AD.KnownDatatype).val;
_tmp4AF=*_tmp4AE;_tmp4B0=_tmp4AC.targs;}};_LL29A: if(_tmp4A8 == _tmp4AF  || Cyc_Absyn_qvar_cmp(
_tmp4A8->name,_tmp4AF->name)== 0){Cyc_Tcutil_unify_list(_tmp4B0,_tmp4A9);return;}
goto _LL28C;_LL29B: _tmp4B1=_tmp480.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4B2=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4B1;if(_tmp4B2->tag != 4)
goto _LL29D;else{_tmp4B3=_tmp4B2->f1;_tmp4B4=_tmp4B3.field_info;if((_tmp4B4.KnownDatatypefield).tag
!= 2)goto _LL29D;_tmp4B5=(struct _tuple3)(_tmp4B4.KnownDatatypefield).val;_tmp4B6=
_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;_tmp4B8=_tmp4B3.targs;}};_tmp4B9=_tmp480.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4BA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4B9;if(_tmp4BA->tag != 4)goto _LL29D;else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BB.field_info;
if((_tmp4BC.KnownDatatypefield).tag != 2)goto _LL29D;_tmp4BD=(struct _tuple3)(
_tmp4BC.KnownDatatypefield).val;_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;_tmp4C0=
_tmp4BB.targs;}};_LL29C: if((_tmp4B6 == _tmp4BE  || Cyc_Absyn_qvar_cmp(_tmp4B6->name,
_tmp4BE->name)== 0) && (_tmp4B7 == _tmp4BF  || Cyc_Absyn_qvar_cmp(_tmp4B7->name,
_tmp4BF->name)== 0)){Cyc_Tcutil_unify_list(_tmp4C0,_tmp4B8);return;}{const char*
_tmp124C;Cyc_Tcutil_failure_reason=((_tmp124C="(different datatype field types)",
_tag_dyneither(_tmp124C,sizeof(char),33)));}goto _LL28C;_LL29D: _tmp4C1=_tmp480.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4C2=(struct Cyc_Absyn_PointerType_struct*)
_tmp4C1;if(_tmp4C2->tag != 5)goto _LL29F;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C3.elt_typ;
_tmp4C5=_tmp4C3.elt_tq;_tmp4C6=_tmp4C3.ptr_atts;_tmp4C7=_tmp4C6.rgn;_tmp4C8=
_tmp4C6.nullable;_tmp4C9=_tmp4C6.bounds;_tmp4CA=_tmp4C6.zero_term;}};_tmp4CB=
_tmp480.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4CC=(struct Cyc_Absyn_PointerType_struct*)
_tmp4CB;if(_tmp4CC->tag != 5)goto _LL29F;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CD.elt_typ;
_tmp4CF=_tmp4CD.elt_tq;_tmp4D0=_tmp4CD.ptr_atts;_tmp4D1=_tmp4D0.rgn;_tmp4D2=
_tmp4D0.nullable;_tmp4D3=_tmp4D0.bounds;_tmp4D4=_tmp4D0.zero_term;}};_LL29E: Cyc_Tcutil_unify_it(
_tmp4CE,_tmp4C4);Cyc_Tcutil_unify_it(_tmp4C7,_tmp4D1);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp124D;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4D4,_tmp4CA,((_tmp124D="(not both zero terminated)",
_tag_dyneither(_tmp124D,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4CF,
_tmp4CE,_tmp4C5,_tmp4C4);{const char*_tmp124E;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4D3,_tmp4C9,((
_tmp124E="(different pointer bounds)",_tag_dyneither(_tmp124E,sizeof(char),27))));}{
void*_tmp55E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4D3);_LL2C8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp55F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp55E;if(_tmp55F->tag != 0)goto
_LL2CA;}_LL2C9: return;_LL2CA:;_LL2CB: goto _LL2C7;_LL2C7:;}{const char*_tmp124F;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4D2,_tmp4C8,((_tmp124F="(incompatible pointer types)",_tag_dyneither(
_tmp124F,sizeof(char),29))));}return;_LL29F: _tmp4D5=_tmp480.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4D6=(struct Cyc_Absyn_IntType_struct*)_tmp4D5;if(_tmp4D6->tag != 6)goto _LL2A1;
else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;}};_tmp4D9=_tmp480.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4DA=(struct Cyc_Absyn_IntType_struct*)_tmp4D9;if(_tmp4DA->tag != 6)goto _LL2A1;
else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;}};_LL2A0: if(_tmp4DB == _tmp4D7  && ((
_tmp4DC == _tmp4D8  || _tmp4DC == Cyc_Absyn_Int_sz  && _tmp4D8 == Cyc_Absyn_Long_sz)
 || _tmp4DC == Cyc_Absyn_Long_sz  && _tmp4D8 == Cyc_Absyn_Int_sz))return;{const char*
_tmp1250;Cyc_Tcutil_failure_reason=((_tmp1250="(different integral types)",
_tag_dyneither(_tmp1250,sizeof(char),27)));}goto _LL28C;_LL2A1: _tmp4DD=_tmp480.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4DE=(struct Cyc_Absyn_FloatType_struct*)
_tmp4DD;if(_tmp4DE->tag != 7)goto _LL2A3;};_tmp4DF=_tmp480.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4E0=(struct Cyc_Absyn_FloatType_struct*)_tmp4DF;if(_tmp4E0->tag != 7)goto
_LL2A3;};_LL2A2: return;_LL2A3: _tmp4E1=_tmp480.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4E2=(struct Cyc_Absyn_DoubleType_struct*)_tmp4E1;if(_tmp4E2->tag != 8)goto
_LL2A5;else{_tmp4E3=_tmp4E2->f1;}};_tmp4E4=_tmp480.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4E5=(struct Cyc_Absyn_DoubleType_struct*)_tmp4E4;if(_tmp4E5->tag != 8)goto
_LL2A5;else{_tmp4E6=_tmp4E5->f1;}};_LL2A4: if(_tmp4E3 == _tmp4E6)return;goto _LL28C;
_LL2A5: _tmp4E7=_tmp480.f1;{struct Cyc_Absyn_TagType_struct*_tmp4E8=(struct Cyc_Absyn_TagType_struct*)
_tmp4E7;if(_tmp4E8->tag != 20)goto _LL2A7;else{_tmp4E9=(void*)_tmp4E8->f1;}};
_tmp4EA=_tmp480.f2;{struct Cyc_Absyn_TagType_struct*_tmp4EB=(struct Cyc_Absyn_TagType_struct*)
_tmp4EA;if(_tmp4EB->tag != 20)goto _LL2A7;else{_tmp4EC=(void*)_tmp4EB->f1;}};
_LL2A6: Cyc_Tcutil_unify_it(_tmp4E9,_tmp4EC);return;_LL2A7: _tmp4ED=_tmp480.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp4EE=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4ED;if(_tmp4EE->tag != 19)goto _LL2A9;else{_tmp4EF=_tmp4EE->f1;}};_tmp4F0=
_tmp480.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp4F1=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4F0;if(_tmp4F1->tag != 19)goto _LL2A9;else{_tmp4F2=_tmp4F1->f1;}};_LL2A8: if(!
Cyc_Evexp_same_const_exp(_tmp4EF,_tmp4F2)){{const char*_tmp1251;Cyc_Tcutil_failure_reason=((
_tmp1251="(cannot prove expressions are the same)",_tag_dyneither(_tmp1251,
sizeof(char),40)));}goto _LL28C;}return;_LL2A9: _tmp4F3=_tmp480.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp4F4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4F3;if(_tmp4F4->tag != 9)goto
_LL2AB;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F5.elt_type;_tmp4F7=_tmp4F5.tq;
_tmp4F8=_tmp4F5.num_elts;_tmp4F9=_tmp4F5.zero_term;}};_tmp4FA=_tmp480.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp4FB=(struct Cyc_Absyn_ArrayType_struct*)_tmp4FA;
if(_tmp4FB->tag != 9)goto _LL2AB;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FC.elt_type;
_tmp4FE=_tmp4FC.tq;_tmp4FF=_tmp4FC.num_elts;_tmp500=_tmp4FC.zero_term;}};_LL2AA:
Cyc_Tcutil_unify_it(_tmp4FD,_tmp4F6);Cyc_Tcutil_unify_tqual(_tmp4FE,_tmp4FD,
_tmp4F7,_tmp4F6);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp1252;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4F9,_tmp500,((_tmp1252="(not both zero terminated)",_tag_dyneither(_tmp1252,
sizeof(char),27))));}if(_tmp4F8 == _tmp4FF)return;if(_tmp4F8 == 0  || _tmp4FF == 0)
goto _LL28C;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4F8,(struct Cyc_Absyn_Exp*)
_tmp4FF))return;{const char*_tmp1253;Cyc_Tcutil_failure_reason=((_tmp1253="(different array sizes)",
_tag_dyneither(_tmp1253,sizeof(char),24)));}goto _LL28C;_LL2AB: _tmp501=_tmp480.f1;{
struct Cyc_Absyn_FnType_struct*_tmp502=(struct Cyc_Absyn_FnType_struct*)_tmp501;
if(_tmp502->tag != 10)goto _LL2AD;else{_tmp503=_tmp502->f1;_tmp504=_tmp503.tvars;
_tmp505=_tmp503.effect;_tmp506=_tmp503.ret_typ;_tmp507=_tmp503.args;_tmp508=
_tmp503.c_varargs;_tmp509=_tmp503.cyc_varargs;_tmp50A=_tmp503.rgn_po;_tmp50B=
_tmp503.attributes;}};_tmp50C=_tmp480.f2;{struct Cyc_Absyn_FnType_struct*_tmp50D=(
struct Cyc_Absyn_FnType_struct*)_tmp50C;if(_tmp50D->tag != 10)goto _LL2AD;else{
_tmp50E=_tmp50D->f1;_tmp50F=_tmp50E.tvars;_tmp510=_tmp50E.effect;_tmp511=_tmp50E.ret_typ;
_tmp512=_tmp50E.args;_tmp513=_tmp50E.c_varargs;_tmp514=_tmp50E.cyc_varargs;
_tmp515=_tmp50E.rgn_po;_tmp516=_tmp50E.attributes;}};_LL2AC: {int done=0;struct
_RegionHandle _tmp565=_new_region("rgn");struct _RegionHandle*rgn=& _tmp565;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp50F != 0){if(_tmp504 == 0){{
const char*_tmp1254;Cyc_Tcutil_failure_reason=((_tmp1254="(second function type has too few type variables)",
_tag_dyneither(_tmp1254,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp567=((struct Cyc_Absyn_Tvar*)_tmp50F->hd)->kind;void*_tmp568=((struct Cyc_Absyn_Tvar*)
_tmp504->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp567,_tmp568)){{const char*
_tmp125A;void*_tmp1259[3];struct Cyc_String_pa_struct _tmp1258;struct Cyc_String_pa_struct
_tmp1257;struct Cyc_String_pa_struct _tmp1256;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1256.tag=0,((_tmp1256.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp504->hd,& Cyc_Tcutil_bk))),((_tmp1257.tag=0,((_tmp1257.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp50F->hd,& Cyc_Tcutil_bk))),((_tmp1258.tag=0,((_tmp1258.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp50F->hd)),((_tmp1259[0]=& _tmp1258,((_tmp1259[1]=& _tmp1257,((_tmp1259[2]=&
_tmp1256,Cyc_aprintf(((_tmp125A="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp125A,sizeof(char),44))),_tag_dyneither(_tmp1259,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp1264;struct Cyc_Absyn_VarType_struct _tmp1263;struct Cyc_Absyn_VarType_struct*
_tmp1262;struct Cyc_List_List*_tmp1261;inst=((_tmp1261=_region_malloc(rgn,sizeof(*
_tmp1261)),((_tmp1261->hd=((_tmp1264=_region_malloc(rgn,sizeof(*_tmp1264)),((
_tmp1264->f1=(struct Cyc_Absyn_Tvar*)_tmp504->hd,((_tmp1264->f2=(void*)((_tmp1262=
_cycalloc(sizeof(*_tmp1262)),((_tmp1262[0]=((_tmp1263.tag=2,((_tmp1263.f1=(
struct Cyc_Absyn_Tvar*)_tmp50F->hd,_tmp1263)))),_tmp1262)))),_tmp1264)))))),((
_tmp1261->tl=inst,_tmp1261))))));}_tmp50F=_tmp50F->tl;_tmp504=_tmp504->tl;};}if(
_tmp504 != 0){{const char*_tmp1265;Cyc_Tcutil_failure_reason=((_tmp1265="(second function type has too many type variables)",
_tag_dyneither(_tmp1265,sizeof(char),51)));}_npop_handler(0);goto _LL28C;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp1271;struct Cyc_Absyn_FnInfo _tmp1270;
struct Cyc_Absyn_FnType_struct*_tmp126F;struct Cyc_Absyn_FnType_struct _tmp126B;
struct Cyc_Absyn_FnInfo _tmp126A;struct Cyc_Absyn_FnType_struct*_tmp1269;Cyc_Tcutil_unify_it((
void*)((_tmp1269=_cycalloc(sizeof(*_tmp1269)),((_tmp1269[0]=((_tmp126B.tag=10,((
_tmp126B.f1=((_tmp126A.tvars=0,((_tmp126A.effect=_tmp510,((_tmp126A.ret_typ=
_tmp511,((_tmp126A.args=_tmp512,((_tmp126A.c_varargs=_tmp513,((_tmp126A.cyc_varargs=
_tmp514,((_tmp126A.rgn_po=_tmp515,((_tmp126A.attributes=_tmp516,_tmp126A)))))))))))))))),
_tmp126B)))),_tmp1269)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp126F=
_cycalloc(sizeof(*_tmp126F)),((_tmp126F[0]=((_tmp1271.tag=10,((_tmp1271.f1=((
_tmp1270.tvars=0,((_tmp1270.effect=_tmp505,((_tmp1270.ret_typ=_tmp506,((_tmp1270.args=
_tmp507,((_tmp1270.c_varargs=_tmp508,((_tmp1270.cyc_varargs=_tmp509,((_tmp1270.rgn_po=
_tmp50A,((_tmp1270.attributes=_tmp50B,_tmp1270)))))))))))))))),_tmp1271)))),
_tmp126F))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp511,_tmp506);for(0;_tmp512 != 0  && _tmp507 != 0;(_tmp512=_tmp512->tl,_tmp507=
_tmp507->tl)){struct Cyc_Absyn_Tqual _tmp57A;void*_tmp57B;struct _tuple9 _tmp579=*((
struct _tuple9*)_tmp512->hd);_tmp57A=_tmp579.f2;_tmp57B=_tmp579.f3;{struct Cyc_Absyn_Tqual
_tmp57D;void*_tmp57E;struct _tuple9 _tmp57C=*((struct _tuple9*)_tmp507->hd);_tmp57D=
_tmp57C.f2;_tmp57E=_tmp57C.f3;Cyc_Tcutil_unify_it(_tmp57B,_tmp57E);Cyc_Tcutil_unify_tqual(
_tmp57A,_tmp57B,_tmp57D,_tmp57E);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp512 != 0  || _tmp507 != 0){{const char*_tmp1272;Cyc_Tcutil_failure_reason=((
_tmp1272="(function types have different number of arguments)",_tag_dyneither(
_tmp1272,sizeof(char),52)));}_npop_handler(0);goto _LL28C;}if(_tmp513 != _tmp508){{
const char*_tmp1273;Cyc_Tcutil_failure_reason=((_tmp1273="(only one function type takes C varargs)",
_tag_dyneither(_tmp1273,sizeof(char),41)));}_npop_handler(0);goto _LL28C;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp1274;struct _tuple21 _tmp582=(_tmp1274.f1=
_tmp514,((_tmp1274.f2=_tmp509,_tmp1274)));struct Cyc_Absyn_VarargInfo*_tmp583;
struct Cyc_Absyn_VarargInfo*_tmp584;struct Cyc_Absyn_VarargInfo*_tmp585;struct Cyc_Absyn_VarargInfo*
_tmp586;struct Cyc_Absyn_VarargInfo*_tmp587;struct Cyc_Absyn_VarargInfo _tmp588;
struct Cyc_Core_Opt*_tmp589;struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58B;int _tmp58C;
struct Cyc_Absyn_VarargInfo*_tmp58D;struct Cyc_Absyn_VarargInfo _tmp58E;struct Cyc_Core_Opt*
_tmp58F;struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;int _tmp592;_LL2CD: _tmp583=
_tmp582.f1;if(_tmp583 != 0)goto _LL2CF;_tmp584=_tmp582.f2;if(_tmp584 != 0)goto
_LL2CF;_LL2CE: goto _LL2CC;_LL2CF: _tmp585=_tmp582.f1;if(_tmp585 != 0)goto _LL2D1;
_LL2D0: goto _LL2D2;_LL2D1: _tmp586=_tmp582.f2;if(_tmp586 != 0)goto _LL2D3;_LL2D2:
bad_cyc_vararg=1;{const char*_tmp1275;Cyc_Tcutil_failure_reason=((_tmp1275="(only one function type takes varargs)",
_tag_dyneither(_tmp1275,sizeof(char),39)));}goto _LL2CC;_LL2D3: _tmp587=_tmp582.f1;
if(_tmp587 == 0)goto _LL2CC;_tmp588=*_tmp587;_tmp589=_tmp588.name;_tmp58A=_tmp588.tq;
_tmp58B=_tmp588.type;_tmp58C=_tmp588.inject;_tmp58D=_tmp582.f2;if(_tmp58D == 0)
goto _LL2CC;_tmp58E=*_tmp58D;_tmp58F=_tmp58E.name;_tmp590=_tmp58E.tq;_tmp591=
_tmp58E.type;_tmp592=_tmp58E.inject;_LL2D4: Cyc_Tcutil_unify_it(_tmp58B,_tmp591);
Cyc_Tcutil_unify_tqual(_tmp58A,_tmp58B,_tmp590,_tmp591);if(_tmp58C != _tmp592){
bad_cyc_vararg=1;{const char*_tmp1276;Cyc_Tcutil_failure_reason=((_tmp1276="(only one function type injects varargs)",
_tag_dyneither(_tmp1276,sizeof(char),41)));};}goto _LL2CC;_LL2CC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL28C;}{int bad_effect=0;{struct _tuple22
_tmp1277;struct _tuple22 _tmp596=(_tmp1277.f1=_tmp510,((_tmp1277.f2=_tmp505,
_tmp1277)));struct Cyc_Core_Opt*_tmp597;struct Cyc_Core_Opt*_tmp598;struct Cyc_Core_Opt*
_tmp599;struct Cyc_Core_Opt*_tmp59A;_LL2D6: _tmp597=_tmp596.f1;if(_tmp597 != 0)goto
_LL2D8;_tmp598=_tmp596.f2;if(_tmp598 != 0)goto _LL2D8;_LL2D7: goto _LL2D5;_LL2D8:
_tmp599=_tmp596.f1;if(_tmp599 != 0)goto _LL2DA;_LL2D9: goto _LL2DB;_LL2DA: _tmp59A=
_tmp596.f2;if(_tmp59A != 0)goto _LL2DC;_LL2DB: bad_effect=1;goto _LL2D5;_LL2DC:;
_LL2DD: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp510))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp505))->v);
goto _LL2D5;_LL2D5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1278;Cyc_Tcutil_failure_reason=((_tmp1278="(function type effects do not unify)",
_tag_dyneither(_tmp1278,sizeof(char),37)));}_npop_handler(0);goto _LL28C;}if(!Cyc_Tcutil_same_atts(
_tmp50B,_tmp516)){{const char*_tmp1279;Cyc_Tcutil_failure_reason=((_tmp1279="(function types have different attributes)",
_tag_dyneither(_tmp1279,sizeof(char),43)));}_npop_handler(0);goto _LL28C;}if(!Cyc_Tcutil_same_rgn_po(
_tmp50A,_tmp515)){{const char*_tmp127A;Cyc_Tcutil_failure_reason=((_tmp127A="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp127A,sizeof(char),58)));}_npop_handler(0);goto _LL28C;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2AD: _tmp517=_tmp480.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp518=(struct Cyc_Absyn_TupleType_struct*)_tmp517;
if(_tmp518->tag != 11)goto _LL2AF;else{_tmp519=_tmp518->f1;}};_tmp51A=_tmp480.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp51B=(struct Cyc_Absyn_TupleType_struct*)
_tmp51A;if(_tmp51B->tag != 11)goto _LL2AF;else{_tmp51C=_tmp51B->f1;}};_LL2AE: for(0;
_tmp51C != 0  && _tmp519 != 0;(_tmp51C=_tmp51C->tl,_tmp519=_tmp519->tl)){struct Cyc_Absyn_Tqual
_tmp59F;void*_tmp5A0;struct _tuple11 _tmp59E=*((struct _tuple11*)_tmp51C->hd);
_tmp59F=_tmp59E.f1;_tmp5A0=_tmp59E.f2;{struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;
struct _tuple11 _tmp5A1=*((struct _tuple11*)_tmp519->hd);_tmp5A2=_tmp5A1.f1;_tmp5A3=
_tmp5A1.f2;Cyc_Tcutil_unify_it(_tmp5A0,_tmp5A3);Cyc_Tcutil_unify_tqual(_tmp59F,
_tmp5A0,_tmp5A2,_tmp5A3);};}if(_tmp51C == 0  && _tmp519 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp127B;Cyc_Tcutil_failure_reason=((
_tmp127B="(tuple types have different numbers of components)",_tag_dyneither(
_tmp127B,sizeof(char),51)));}goto _LL28C;_LL2AF: _tmp51D=_tmp480.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp51E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp51D;if(_tmp51E->tag != 13)goto
_LL2B1;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}};_tmp521=_tmp480.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp522=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp521;if(_tmp522->tag != 13)goto _LL2B1;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}};
_LL2B0: if(_tmp523 != _tmp51F){{const char*_tmp127C;Cyc_Tcutil_failure_reason=((
_tmp127C="(struct and union type)",_tag_dyneither(_tmp127C,sizeof(char),24)));}
goto _LL28C;}for(0;_tmp524 != 0  && _tmp520 != 0;(_tmp524=_tmp524->tl,_tmp520=
_tmp520->tl)){struct Cyc_Absyn_Aggrfield*_tmp5A6=(struct Cyc_Absyn_Aggrfield*)
_tmp524->hd;struct Cyc_Absyn_Aggrfield*_tmp5A7=(struct Cyc_Absyn_Aggrfield*)
_tmp520->hd;if(Cyc_strptrcmp(_tmp5A6->name,_tmp5A7->name)!= 0){{const char*
_tmp127D;Cyc_Tcutil_failure_reason=((_tmp127D="(different member names)",
_tag_dyneither(_tmp127D,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5A6->type,_tmp5A7->type);Cyc_Tcutil_unify_tqual(_tmp5A6->tq,
_tmp5A6->type,_tmp5A7->tq,_tmp5A7->type);if(!Cyc_Tcutil_same_atts(_tmp5A6->attributes,
_tmp5A7->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp127E;Cyc_Tcutil_failure_reason=((_tmp127E="(different attributes on member)",
_tag_dyneither(_tmp127E,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5A6->width != 0  && _tmp5A7->width == 0  || _tmp5A7->width != 0  && _tmp5A6->width
== 0) || (_tmp5A6->width != 0  && _tmp5A7->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5A6->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5A7->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp127F;Cyc_Tcutil_failure_reason=((_tmp127F="(different bitfield widths on member)",
_tag_dyneither(_tmp127F,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp524 == 0  && _tmp520 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp1280;Cyc_Tcutil_failure_reason=((_tmp1280="(different number of members)",
_tag_dyneither(_tmp1280,sizeof(char),30)));}goto _LL28C;_LL2B1: _tmp525=_tmp480.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp526=(struct Cyc_Absyn_HeapRgn_struct*)_tmp525;
if(_tmp526->tag != 21)goto _LL2B3;};_tmp527=_tmp480.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp528=(struct Cyc_Absyn_HeapRgn_struct*)_tmp527;if(_tmp528->tag != 21)goto _LL2B3;};
_LL2B2: return;_LL2B3: _tmp529=_tmp480.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp52A=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp529;if(_tmp52A->tag != 22)goto
_LL2B5;};_tmp52B=_tmp480.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp52C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp52B;if(_tmp52C->tag != 22)goto _LL2B5;};_LL2B4: return;_LL2B5: _tmp52D=_tmp480.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp52E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp52D;if(_tmp52E->tag != 16)goto _LL2B7;else{_tmp52F=(void*)_tmp52E->f1;}};
_tmp530=_tmp480.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp531=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp530;if(_tmp531->tag != 16)goto _LL2B7;else{_tmp532=(void*)_tmp531->f1;}};
_LL2B6: Cyc_Tcutil_unify_it(_tmp52F,_tmp532);return;_LL2B7: _tmp533=_tmp480.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp534=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp533;if(_tmp534->tag != 17)goto _LL2B9;else{_tmp535=(void*)_tmp534->f1;_tmp536=(
void*)_tmp534->f2;}};_tmp537=_tmp480.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp538=(struct Cyc_Absyn_DynRgnType_struct*)_tmp537;if(_tmp538->tag != 17)goto
_LL2B9;else{_tmp539=(void*)_tmp538->f1;_tmp53A=(void*)_tmp538->f2;}};_LL2B8: Cyc_Tcutil_unify_it(
_tmp535,_tmp539);Cyc_Tcutil_unify_it(_tmp536,_tmp53A);return;_LL2B9: _tmp53B=
_tmp480.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp53C=(struct Cyc_Absyn_JoinEff_struct*)
_tmp53B;if(_tmp53C->tag != 24)goto _LL2BB;};_LL2BA: goto _LL2BC;_LL2BB: _tmp53D=
_tmp480.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp53E=(struct Cyc_Absyn_JoinEff_struct*)
_tmp53D;if(_tmp53E->tag != 24)goto _LL2BD;};_LL2BC: goto _LL2BE;_LL2BD: _tmp53F=
_tmp480.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp540=(struct Cyc_Absyn_AccessEff_struct*)
_tmp53F;if(_tmp540->tag != 23)goto _LL2BF;};_LL2BE: goto _LL2C0;_LL2BF: _tmp541=
_tmp480.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp542=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp541;if(_tmp542->tag != 25)goto _LL2C1;};_LL2C0: goto _LL2C2;_LL2C1: _tmp543=
_tmp480.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp544=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp543;if(_tmp544->tag != 25)goto _LL2C3;};_LL2C2: goto _LL2C4;_LL2C3: _tmp545=
_tmp480.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp546=(struct Cyc_Absyn_AccessEff_struct*)
_tmp545;if(_tmp546->tag != 23)goto _LL2C5;};_LL2C4: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp1281;Cyc_Tcutil_failure_reason=((_tmp1281="(effects don't unify)",
_tag_dyneither(_tmp1281,sizeof(char),22)));}goto _LL28C;_LL2C5:;_LL2C6: goto _LL28C;
_LL28C:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return
- 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)
_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp5AD=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp5AE=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp5AD,_tmp5AE);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp5AF=x;union Cyc_Absyn_Constraint _tmp5B0;int _tmp5B1;union Cyc_Absyn_Constraint
_tmp5B2;void*_tmp5B3;union Cyc_Absyn_Constraint _tmp5B4;union Cyc_Absyn_Constraint*
_tmp5B5;_LL2DF: _tmp5B0=*_tmp5AF;if((_tmp5B0.No_constr).tag != 3)goto _LL2E1;
_tmp5B1=(int)(_tmp5B0.No_constr).val;_LL2E0: return - 1;_LL2E1: _tmp5B2=*_tmp5AF;if((
_tmp5B2.Eq_constr).tag != 1)goto _LL2E3;_tmp5B3=(void*)(_tmp5B2.Eq_constr).val;
_LL2E2: {union Cyc_Absyn_Constraint*_tmp5B6=y;union Cyc_Absyn_Constraint _tmp5B7;
int _tmp5B8;union Cyc_Absyn_Constraint _tmp5B9;void*_tmp5BA;union Cyc_Absyn_Constraint
_tmp5BB;union Cyc_Absyn_Constraint*_tmp5BC;_LL2E6: _tmp5B7=*_tmp5B6;if((_tmp5B7.No_constr).tag
!= 3)goto _LL2E8;_tmp5B8=(int)(_tmp5B7.No_constr).val;_LL2E7: return 1;_LL2E8:
_tmp5B9=*_tmp5B6;if((_tmp5B9.Eq_constr).tag != 1)goto _LL2EA;_tmp5BA=(void*)(
_tmp5B9.Eq_constr).val;_LL2E9: return cmp(_tmp5B3,_tmp5BA);_LL2EA: _tmp5BB=*_tmp5B6;
if((_tmp5BB.Forward_constr).tag != 2)goto _LL2E5;_tmp5BC=(union Cyc_Absyn_Constraint*)(
_tmp5BB.Forward_constr).val;_LL2EB: {const char*_tmp1284;void*_tmp1283;(_tmp1283=
0,Cyc_Tcutil_impos(((_tmp1284="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp1284,sizeof(char),40))),_tag_dyneither(_tmp1283,sizeof(void*),
0)));}_LL2E5:;}_LL2E3: _tmp5B4=*_tmp5AF;if((_tmp5B4.Forward_constr).tag != 2)goto
_LL2DE;_tmp5B5=(union Cyc_Absyn_Constraint*)(_tmp5B4.Forward_constr).val;_LL2E4: {
const char*_tmp1287;void*_tmp1286;(_tmp1286=0,Cyc_Tcutil_impos(((_tmp1287="unify_conref: forward after compress",
_tag_dyneither(_tmp1287,sizeof(char),37))),_tag_dyneither(_tmp1286,sizeof(void*),
0)));}_LL2DE:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp5C2;struct Cyc_Absyn_Tqual _tmp5C3;void*_tmp5C4;
struct _tuple11*_tmp5C1=tqt1;_tmp5C2=*_tmp5C1;_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;{
struct _tuple11 _tmp5C6;struct Cyc_Absyn_Tqual _tmp5C7;void*_tmp5C8;struct _tuple11*
_tmp5C5=tqt2;_tmp5C6=*_tmp5C5;_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;{int _tmp5C9=
Cyc_Tcutil_tqual_cmp(_tmp5C3,_tmp5C7);if(_tmp5C9 != 0)return _tmp5C9;return Cyc_Tcutil_typecmp(
_tmp5C4,_tmp5C8);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5CA=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5CA != 0)return _tmp5CA;{int _tmp5CB=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5CB != 0)return _tmp5CB;{int _tmp5CC=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5CC != 0)return _tmp5CC;{int _tmp5CD=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5CD != 0)return _tmp5CD;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5CE=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5CE != 0)return _tmp5CE;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5CF=t;_LL2ED: {struct Cyc_Absyn_VoidType_struct*_tmp5D0=(struct
Cyc_Absyn_VoidType_struct*)_tmp5CF;if(_tmp5D0->tag != 0)goto _LL2EF;}_LL2EE: return
0;_LL2EF: {struct Cyc_Absyn_Evar_struct*_tmp5D1=(struct Cyc_Absyn_Evar_struct*)
_tmp5CF;if(_tmp5D1->tag != 1)goto _LL2F1;}_LL2F0: return 1;_LL2F1: {struct Cyc_Absyn_VarType_struct*
_tmp5D2=(struct Cyc_Absyn_VarType_struct*)_tmp5CF;if(_tmp5D2->tag != 2)goto _LL2F3;}
_LL2F2: return 2;_LL2F3: {struct Cyc_Absyn_DatatypeType_struct*_tmp5D3=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5CF;if(_tmp5D3->tag != 3)goto _LL2F5;}_LL2F4: return 3;_LL2F5: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5D4=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5CF;if(_tmp5D4->tag != 4)
goto _LL2F7;}_LL2F6: return 4;_LL2F7: {struct Cyc_Absyn_PointerType_struct*_tmp5D5=(
struct Cyc_Absyn_PointerType_struct*)_tmp5CF;if(_tmp5D5->tag != 5)goto _LL2F9;}
_LL2F8: return 5;_LL2F9: {struct Cyc_Absyn_IntType_struct*_tmp5D6=(struct Cyc_Absyn_IntType_struct*)
_tmp5CF;if(_tmp5D6->tag != 6)goto _LL2FB;}_LL2FA: return 6;_LL2FB: {struct Cyc_Absyn_FloatType_struct*
_tmp5D7=(struct Cyc_Absyn_FloatType_struct*)_tmp5CF;if(_tmp5D7->tag != 7)goto
_LL2FD;}_LL2FC: return 7;_LL2FD: {struct Cyc_Absyn_DoubleType_struct*_tmp5D8=(
struct Cyc_Absyn_DoubleType_struct*)_tmp5CF;if(_tmp5D8->tag != 8)goto _LL2FF;}
_LL2FE: return 8;_LL2FF: {struct Cyc_Absyn_ArrayType_struct*_tmp5D9=(struct Cyc_Absyn_ArrayType_struct*)
_tmp5CF;if(_tmp5D9->tag != 9)goto _LL301;}_LL300: return 9;_LL301: {struct Cyc_Absyn_FnType_struct*
_tmp5DA=(struct Cyc_Absyn_FnType_struct*)_tmp5CF;if(_tmp5DA->tag != 10)goto _LL303;}
_LL302: return 10;_LL303: {struct Cyc_Absyn_TupleType_struct*_tmp5DB=(struct Cyc_Absyn_TupleType_struct*)
_tmp5CF;if(_tmp5DB->tag != 11)goto _LL305;}_LL304: return 11;_LL305: {struct Cyc_Absyn_AggrType_struct*
_tmp5DC=(struct Cyc_Absyn_AggrType_struct*)_tmp5CF;if(_tmp5DC->tag != 12)goto
_LL307;}_LL306: return 12;_LL307: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5DD=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp5CF;if(_tmp5DD->tag != 13)goto _LL309;}
_LL308: return 14;_LL309: {struct Cyc_Absyn_EnumType_struct*_tmp5DE=(struct Cyc_Absyn_EnumType_struct*)
_tmp5CF;if(_tmp5DE->tag != 14)goto _LL30B;}_LL30A: return 16;_LL30B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp5DF=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5CF;if(_tmp5DF->tag != 15)goto
_LL30D;}_LL30C: return 17;_LL30D: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5E0=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5CF;if(_tmp5E0->tag != 16)goto _LL30F;}
_LL30E: return 18;_LL30F: {struct Cyc_Absyn_TypedefType_struct*_tmp5E1=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5CF;if(_tmp5E1->tag != 18)goto _LL311;}_LL310: return 19;_LL311: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp5E2=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp5CF;if(_tmp5E2->tag != 22)goto
_LL313;}_LL312: return 20;_LL313: {struct Cyc_Absyn_HeapRgn_struct*_tmp5E3=(struct
Cyc_Absyn_HeapRgn_struct*)_tmp5CF;if(_tmp5E3->tag != 21)goto _LL315;}_LL314: return
21;_LL315: {struct Cyc_Absyn_AccessEff_struct*_tmp5E4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5CF;if(_tmp5E4->tag != 23)goto _LL317;}_LL316: return 22;_LL317: {struct Cyc_Absyn_JoinEff_struct*
_tmp5E5=(struct Cyc_Absyn_JoinEff_struct*)_tmp5CF;if(_tmp5E5->tag != 24)goto _LL319;}
_LL318: return 23;_LL319: {struct Cyc_Absyn_RgnsEff_struct*_tmp5E6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5CF;if(_tmp5E6->tag != 25)goto _LL31B;}_LL31A: return 24;_LL31B: {struct Cyc_Absyn_TagType_struct*
_tmp5E7=(struct Cyc_Absyn_TagType_struct*)_tmp5CF;if(_tmp5E7->tag != 20)goto _LL31D;}
_LL31C: return 26;_LL31D: {struct Cyc_Absyn_DynRgnType_struct*_tmp5E8=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5CF;if(_tmp5E8->tag != 17)goto _LL31F;}_LL31E: return 27;_LL31F: {struct Cyc_Absyn_ValueofType_struct*
_tmp5E9=(struct Cyc_Absyn_ValueofType_struct*)_tmp5CF;if(_tmp5E9->tag != 19)goto
_LL321;}_LL320: return 28;_LL321: {struct Cyc_Absyn_TypeDeclType_struct*_tmp5EA=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp5CF;if(_tmp5EA->tag != 26)goto _LL2EC;}
_LL322: return 29;_LL2EC:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp5EB=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp5EB != 0)
return _tmp5EB;{struct _tuple0 _tmp1288;struct _tuple0 _tmp5ED=(_tmp1288.f1=t2,((
_tmp1288.f2=t1,_tmp1288)));void*_tmp5EE;void*_tmp5F0;void*_tmp5F2;struct Cyc_Absyn_Tvar*
_tmp5F4;void*_tmp5F5;struct Cyc_Absyn_Tvar*_tmp5F7;void*_tmp5F8;struct Cyc_Absyn_AggrInfo
_tmp5FA;union Cyc_Absyn_AggrInfoU _tmp5FB;struct Cyc_List_List*_tmp5FC;void*_tmp5FD;
struct Cyc_Absyn_AggrInfo _tmp5FF;union Cyc_Absyn_AggrInfoU _tmp600;struct Cyc_List_List*
_tmp601;void*_tmp602;struct _tuple2*_tmp604;void*_tmp605;struct _tuple2*_tmp607;
void*_tmp608;struct Cyc_List_List*_tmp60A;void*_tmp60B;struct Cyc_List_List*
_tmp60D;void*_tmp60E;struct Cyc_Absyn_DatatypeInfo _tmp610;union Cyc_Absyn_DatatypeInfoU
_tmp611;struct Cyc_Absyn_Datatypedecl**_tmp612;struct Cyc_Absyn_Datatypedecl*
_tmp613;struct Cyc_List_List*_tmp614;void*_tmp615;struct Cyc_Absyn_DatatypeInfo
_tmp617;union Cyc_Absyn_DatatypeInfoU _tmp618;struct Cyc_Absyn_Datatypedecl**
_tmp619;struct Cyc_Absyn_Datatypedecl*_tmp61A;struct Cyc_List_List*_tmp61B;void*
_tmp61C;struct Cyc_Absyn_DatatypeFieldInfo _tmp61E;union Cyc_Absyn_DatatypeFieldInfoU
_tmp61F;struct _tuple3 _tmp620;struct Cyc_Absyn_Datatypedecl*_tmp621;struct Cyc_Absyn_Datatypefield*
_tmp622;struct Cyc_List_List*_tmp623;void*_tmp624;struct Cyc_Absyn_DatatypeFieldInfo
_tmp626;union Cyc_Absyn_DatatypeFieldInfoU _tmp627;struct _tuple3 _tmp628;struct Cyc_Absyn_Datatypedecl*
_tmp629;struct Cyc_Absyn_Datatypefield*_tmp62A;struct Cyc_List_List*_tmp62B;void*
_tmp62C;struct Cyc_Absyn_PtrInfo _tmp62E;void*_tmp62F;struct Cyc_Absyn_Tqual _tmp630;
struct Cyc_Absyn_PtrAtts _tmp631;void*_tmp632;union Cyc_Absyn_Constraint*_tmp633;
union Cyc_Absyn_Constraint*_tmp634;union Cyc_Absyn_Constraint*_tmp635;void*_tmp636;
struct Cyc_Absyn_PtrInfo _tmp638;void*_tmp639;struct Cyc_Absyn_Tqual _tmp63A;struct
Cyc_Absyn_PtrAtts _tmp63B;void*_tmp63C;union Cyc_Absyn_Constraint*_tmp63D;union Cyc_Absyn_Constraint*
_tmp63E;union Cyc_Absyn_Constraint*_tmp63F;void*_tmp640;enum Cyc_Absyn_Sign
_tmp642;enum Cyc_Absyn_Size_of _tmp643;void*_tmp644;enum Cyc_Absyn_Sign _tmp646;
enum Cyc_Absyn_Size_of _tmp647;void*_tmp648;void*_tmp64A;void*_tmp64C;int _tmp64E;
void*_tmp64F;int _tmp651;void*_tmp652;struct Cyc_Absyn_ArrayInfo _tmp654;void*
_tmp655;struct Cyc_Absyn_Tqual _tmp656;struct Cyc_Absyn_Exp*_tmp657;union Cyc_Absyn_Constraint*
_tmp658;void*_tmp659;struct Cyc_Absyn_ArrayInfo _tmp65B;void*_tmp65C;struct Cyc_Absyn_Tqual
_tmp65D;struct Cyc_Absyn_Exp*_tmp65E;union Cyc_Absyn_Constraint*_tmp65F;void*
_tmp660;struct Cyc_Absyn_FnInfo _tmp662;struct Cyc_List_List*_tmp663;struct Cyc_Core_Opt*
_tmp664;void*_tmp665;struct Cyc_List_List*_tmp666;int _tmp667;struct Cyc_Absyn_VarargInfo*
_tmp668;struct Cyc_List_List*_tmp669;struct Cyc_List_List*_tmp66A;void*_tmp66B;
struct Cyc_Absyn_FnInfo _tmp66D;struct Cyc_List_List*_tmp66E;struct Cyc_Core_Opt*
_tmp66F;void*_tmp670;struct Cyc_List_List*_tmp671;int _tmp672;struct Cyc_Absyn_VarargInfo*
_tmp673;struct Cyc_List_List*_tmp674;struct Cyc_List_List*_tmp675;void*_tmp676;
struct Cyc_List_List*_tmp678;void*_tmp679;struct Cyc_List_List*_tmp67B;void*
_tmp67C;enum Cyc_Absyn_AggrKind _tmp67E;struct Cyc_List_List*_tmp67F;void*_tmp680;
enum Cyc_Absyn_AggrKind _tmp682;struct Cyc_List_List*_tmp683;void*_tmp684;void*
_tmp686;void*_tmp687;void*_tmp689;void*_tmp68A;void*_tmp68C;void*_tmp68D;void*
_tmp68E;void*_tmp690;void*_tmp691;void*_tmp692;void*_tmp694;void*_tmp695;void*
_tmp697;void*_tmp698;struct Cyc_Absyn_Exp*_tmp69A;void*_tmp69B;struct Cyc_Absyn_Exp*
_tmp69D;void*_tmp69E;void*_tmp6A0;void*_tmp6A2;void*_tmp6A4;void*_tmp6A6;void*
_tmp6A8;_LL324: _tmp5EE=_tmp5ED.f1;{struct Cyc_Absyn_Evar_struct*_tmp5EF=(struct
Cyc_Absyn_Evar_struct*)_tmp5EE;if(_tmp5EF->tag != 1)goto _LL326;};_tmp5F0=_tmp5ED.f2;{
struct Cyc_Absyn_Evar_struct*_tmp5F1=(struct Cyc_Absyn_Evar_struct*)_tmp5F0;if(
_tmp5F1->tag != 1)goto _LL326;};_LL325: {const char*_tmp128B;void*_tmp128A;(
_tmp128A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp128B="typecmp: can only compare closed types",_tag_dyneither(_tmp128B,
sizeof(char),39))),_tag_dyneither(_tmp128A,sizeof(void*),0)));}_LL326: _tmp5F2=
_tmp5ED.f1;{struct Cyc_Absyn_VarType_struct*_tmp5F3=(struct Cyc_Absyn_VarType_struct*)
_tmp5F2;if(_tmp5F3->tag != 2)goto _LL328;else{_tmp5F4=_tmp5F3->f1;}};_tmp5F5=
_tmp5ED.f2;{struct Cyc_Absyn_VarType_struct*_tmp5F6=(struct Cyc_Absyn_VarType_struct*)
_tmp5F5;if(_tmp5F6->tag != 2)goto _LL328;else{_tmp5F7=_tmp5F6->f1;}};_LL327: return
Cyc_Core_intcmp(_tmp5F7->identity,_tmp5F4->identity);_LL328: _tmp5F8=_tmp5ED.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp5F9=(struct Cyc_Absyn_AggrType_struct*)
_tmp5F8;if(_tmp5F9->tag != 12)goto _LL32A;else{_tmp5FA=_tmp5F9->f1;_tmp5FB=_tmp5FA.aggr_info;
_tmp5FC=_tmp5FA.targs;}};_tmp5FD=_tmp5ED.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp5FE=(struct Cyc_Absyn_AggrType_struct*)_tmp5FD;if(_tmp5FE->tag != 12)goto
_LL32A;else{_tmp5FF=_tmp5FE->f1;_tmp600=_tmp5FF.aggr_info;_tmp601=_tmp5FF.targs;}};
_LL329: {struct _tuple2*_tmp6AD;struct _tuple12 _tmp6AC=Cyc_Absyn_aggr_kinded_name(
_tmp5FB);_tmp6AD=_tmp6AC.f2;{struct _tuple2*_tmp6AF;struct _tuple12 _tmp6AE=Cyc_Absyn_aggr_kinded_name(
_tmp600);_tmp6AF=_tmp6AE.f2;{int _tmp6B0=Cyc_Absyn_qvar_cmp(_tmp6AD,_tmp6AF);if(
_tmp6B0 != 0)return _tmp6B0;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp5FC,_tmp601);}};};}
_LL32A: _tmp602=_tmp5ED.f1;{struct Cyc_Absyn_EnumType_struct*_tmp603=(struct Cyc_Absyn_EnumType_struct*)
_tmp602;if(_tmp603->tag != 14)goto _LL32C;else{_tmp604=_tmp603->f1;}};_tmp605=
_tmp5ED.f2;{struct Cyc_Absyn_EnumType_struct*_tmp606=(struct Cyc_Absyn_EnumType_struct*)
_tmp605;if(_tmp606->tag != 14)goto _LL32C;else{_tmp607=_tmp606->f1;}};_LL32B:
return Cyc_Absyn_qvar_cmp(_tmp604,_tmp607);_LL32C: _tmp608=_tmp5ED.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp609=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp608;if(_tmp609->tag != 15)goto
_LL32E;else{_tmp60A=_tmp609->f1;}};_tmp60B=_tmp5ED.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp60C=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp60B;if(_tmp60C->tag != 15)goto
_LL32E;else{_tmp60D=_tmp60C->f1;}};_LL32D: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp60A,_tmp60D);_LL32E: _tmp60E=_tmp5ED.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp60F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp60E;if(_tmp60F->tag != 3)goto
_LL330;else{_tmp610=_tmp60F->f1;_tmp611=_tmp610.datatype_info;if((_tmp611.KnownDatatype).tag
!= 2)goto _LL330;_tmp612=(struct Cyc_Absyn_Datatypedecl**)(_tmp611.KnownDatatype).val;
_tmp613=*_tmp612;_tmp614=_tmp610.targs;}};_tmp615=_tmp5ED.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp616=(struct Cyc_Absyn_DatatypeType_struct*)_tmp615;if(_tmp616->tag != 3)goto
_LL330;else{_tmp617=_tmp616->f1;_tmp618=_tmp617.datatype_info;if((_tmp618.KnownDatatype).tag
!= 2)goto _LL330;_tmp619=(struct Cyc_Absyn_Datatypedecl**)(_tmp618.KnownDatatype).val;
_tmp61A=*_tmp619;_tmp61B=_tmp617.targs;}};_LL32F: if(_tmp61A == _tmp613)return 0;{
int _tmp6B1=Cyc_Absyn_qvar_cmp(_tmp61A->name,_tmp613->name);if(_tmp6B1 != 0)return
_tmp6B1;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp61B,_tmp614);};_LL330: _tmp61C=
_tmp5ED.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp61D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp61C;if(_tmp61D->tag != 4)goto _LL332;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61E.field_info;
if((_tmp61F.KnownDatatypefield).tag != 2)goto _LL332;_tmp620=(struct _tuple3)(
_tmp61F.KnownDatatypefield).val;_tmp621=_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=
_tmp61E.targs;}};_tmp624=_tmp5ED.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp625=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp624;if(_tmp625->tag != 4)
goto _LL332;else{_tmp626=_tmp625->f1;_tmp627=_tmp626.field_info;if((_tmp627.KnownDatatypefield).tag
!= 2)goto _LL332;_tmp628=(struct _tuple3)(_tmp627.KnownDatatypefield).val;_tmp629=
_tmp628.f1;_tmp62A=_tmp628.f2;_tmp62B=_tmp626.targs;}};_LL331: if(_tmp629 == 
_tmp621)return 0;{int _tmp6B2=Cyc_Absyn_qvar_cmp(_tmp621->name,_tmp629->name);if(
_tmp6B2 != 0)return _tmp6B2;{int _tmp6B3=Cyc_Absyn_qvar_cmp(_tmp622->name,_tmp62A->name);
if(_tmp6B3 != 0)return _tmp6B3;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp62B,_tmp623);};};
_LL332: _tmp62C=_tmp5ED.f1;{struct Cyc_Absyn_PointerType_struct*_tmp62D=(struct Cyc_Absyn_PointerType_struct*)
_tmp62C;if(_tmp62D->tag != 5)goto _LL334;else{_tmp62E=_tmp62D->f1;_tmp62F=_tmp62E.elt_typ;
_tmp630=_tmp62E.elt_tq;_tmp631=_tmp62E.ptr_atts;_tmp632=_tmp631.rgn;_tmp633=
_tmp631.nullable;_tmp634=_tmp631.bounds;_tmp635=_tmp631.zero_term;}};_tmp636=
_tmp5ED.f2;{struct Cyc_Absyn_PointerType_struct*_tmp637=(struct Cyc_Absyn_PointerType_struct*)
_tmp636;if(_tmp637->tag != 5)goto _LL334;else{_tmp638=_tmp637->f1;_tmp639=_tmp638.elt_typ;
_tmp63A=_tmp638.elt_tq;_tmp63B=_tmp638.ptr_atts;_tmp63C=_tmp63B.rgn;_tmp63D=
_tmp63B.nullable;_tmp63E=_tmp63B.bounds;_tmp63F=_tmp63B.zero_term;}};_LL333: {
int _tmp6B4=Cyc_Tcutil_typecmp(_tmp639,_tmp62F);if(_tmp6B4 != 0)return _tmp6B4;{int
_tmp6B5=Cyc_Tcutil_typecmp(_tmp63C,_tmp632);if(_tmp6B5 != 0)return _tmp6B5;{int
_tmp6B6=Cyc_Tcutil_tqual_cmp(_tmp63A,_tmp630);if(_tmp6B6 != 0)return _tmp6B6;{int
_tmp6B7=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp63E,_tmp634);if(_tmp6B7 != 0)
return _tmp6B7;{int _tmp6B8=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp63F,
_tmp635);if(_tmp6B8 != 0)return _tmp6B8;{void*_tmp6B9=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp63E);_LL359: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6BA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6B9;if(_tmp6BA->tag != 0)goto
_LL35B;}_LL35A: return 0;_LL35B:;_LL35C: goto _LL358;_LL358:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp63D,_tmp633);};};};};}_LL334: _tmp640=_tmp5ED.f1;{struct Cyc_Absyn_IntType_struct*
_tmp641=(struct Cyc_Absyn_IntType_struct*)_tmp640;if(_tmp641->tag != 6)goto _LL336;
else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;}};_tmp644=_tmp5ED.f2;{struct Cyc_Absyn_IntType_struct*
_tmp645=(struct Cyc_Absyn_IntType_struct*)_tmp644;if(_tmp645->tag != 6)goto _LL336;
else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;}};_LL335: if(_tmp646 != _tmp642)
return Cyc_Core_intcmp((int)((unsigned int)_tmp646),(int)((unsigned int)_tmp642));
if(_tmp647 != _tmp643)return Cyc_Core_intcmp((int)((unsigned int)_tmp647),(int)((
unsigned int)_tmp643));return 0;_LL336: _tmp648=_tmp5ED.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp649=(struct Cyc_Absyn_FloatType_struct*)_tmp648;if(_tmp649->tag != 7)goto
_LL338;};_tmp64A=_tmp5ED.f2;{struct Cyc_Absyn_FloatType_struct*_tmp64B=(struct Cyc_Absyn_FloatType_struct*)
_tmp64A;if(_tmp64B->tag != 7)goto _LL338;};_LL337: return 0;_LL338: _tmp64C=_tmp5ED.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp64D=(struct Cyc_Absyn_DoubleType_struct*)
_tmp64C;if(_tmp64D->tag != 8)goto _LL33A;else{_tmp64E=_tmp64D->f1;}};_tmp64F=
_tmp5ED.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp650=(struct Cyc_Absyn_DoubleType_struct*)
_tmp64F;if(_tmp650->tag != 8)goto _LL33A;else{_tmp651=_tmp650->f1;}};_LL339: if(
_tmp64E == _tmp651)return 0;else{if(_tmp64E)return - 1;else{return 1;}}_LL33A: _tmp652=
_tmp5ED.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp653=(struct Cyc_Absyn_ArrayType_struct*)
_tmp652;if(_tmp653->tag != 9)goto _LL33C;else{_tmp654=_tmp653->f1;_tmp655=_tmp654.elt_type;
_tmp656=_tmp654.tq;_tmp657=_tmp654.num_elts;_tmp658=_tmp654.zero_term;}};_tmp659=
_tmp5ED.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp65A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp659;if(_tmp65A->tag != 9)goto _LL33C;else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65B.elt_type;
_tmp65D=_tmp65B.tq;_tmp65E=_tmp65B.num_elts;_tmp65F=_tmp65B.zero_term;}};_LL33B: {
int _tmp6BB=Cyc_Tcutil_tqual_cmp(_tmp65D,_tmp656);if(_tmp6BB != 0)return _tmp6BB;{
int _tmp6BC=Cyc_Tcutil_typecmp(_tmp65C,_tmp655);if(_tmp6BC != 0)return _tmp6BC;{int
_tmp6BD=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp658,_tmp65F);if(_tmp6BD != 0)
return _tmp6BD;if(_tmp657 == _tmp65E)return 0;if(_tmp657 == 0  || _tmp65E == 0){const
char*_tmp128E;void*_tmp128D;(_tmp128D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp128E="missing expression in array index",
_tag_dyneither(_tmp128E,sizeof(char),34))),_tag_dyneither(_tmp128D,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp657,_tmp65E);};};}_LL33C: _tmp660=_tmp5ED.f1;{struct Cyc_Absyn_FnType_struct*
_tmp661=(struct Cyc_Absyn_FnType_struct*)_tmp660;if(_tmp661->tag != 10)goto _LL33E;
else{_tmp662=_tmp661->f1;_tmp663=_tmp662.tvars;_tmp664=_tmp662.effect;_tmp665=
_tmp662.ret_typ;_tmp666=_tmp662.args;_tmp667=_tmp662.c_varargs;_tmp668=_tmp662.cyc_varargs;
_tmp669=_tmp662.rgn_po;_tmp66A=_tmp662.attributes;}};_tmp66B=_tmp5ED.f2;{struct
Cyc_Absyn_FnType_struct*_tmp66C=(struct Cyc_Absyn_FnType_struct*)_tmp66B;if(
_tmp66C->tag != 10)goto _LL33E;else{_tmp66D=_tmp66C->f1;_tmp66E=_tmp66D.tvars;
_tmp66F=_tmp66D.effect;_tmp670=_tmp66D.ret_typ;_tmp671=_tmp66D.args;_tmp672=
_tmp66D.c_varargs;_tmp673=_tmp66D.cyc_varargs;_tmp674=_tmp66D.rgn_po;_tmp675=
_tmp66D.attributes;}};_LL33D: {const char*_tmp1291;void*_tmp1290;(_tmp1290=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1291="typecmp: function types not handled",_tag_dyneither(_tmp1291,sizeof(
char),36))),_tag_dyneither(_tmp1290,sizeof(void*),0)));}_LL33E: _tmp676=_tmp5ED.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp677=(struct Cyc_Absyn_TupleType_struct*)
_tmp676;if(_tmp677->tag != 11)goto _LL340;else{_tmp678=_tmp677->f1;}};_tmp679=
_tmp5ED.f2;{struct Cyc_Absyn_TupleType_struct*_tmp67A=(struct Cyc_Absyn_TupleType_struct*)
_tmp679;if(_tmp67A->tag != 11)goto _LL340;else{_tmp67B=_tmp67A->f1;}};_LL33F:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp67B,
_tmp678);_LL340: _tmp67C=_tmp5ED.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp67D=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp67C;if(_tmp67D->tag != 13)goto _LL342;
else{_tmp67E=_tmp67D->f1;_tmp67F=_tmp67D->f2;}};_tmp680=_tmp5ED.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp681=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp680;if(_tmp681->tag != 13)goto
_LL342;else{_tmp682=_tmp681->f1;_tmp683=_tmp681->f2;}};_LL341: if(_tmp682 != 
_tmp67E){if(_tmp682 == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp683,
_tmp67F);_LL342: _tmp684=_tmp5ED.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp685=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp684;if(_tmp685->tag != 16)goto _LL344;
else{_tmp686=(void*)_tmp685->f1;}};_tmp687=_tmp5ED.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp688=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp687;if(_tmp688->tag != 16)goto
_LL344;else{_tmp689=(void*)_tmp688->f1;}};_LL343: return Cyc_Tcutil_typecmp(
_tmp686,_tmp689);_LL344: _tmp68A=_tmp5ED.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp68B=(struct Cyc_Absyn_DynRgnType_struct*)_tmp68A;if(_tmp68B->tag != 17)goto
_LL346;else{_tmp68C=(void*)_tmp68B->f1;_tmp68D=(void*)_tmp68B->f2;}};_tmp68E=
_tmp5ED.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp68F=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp68E;if(_tmp68F->tag != 17)goto _LL346;else{_tmp690=(void*)_tmp68F->f1;_tmp691=(
void*)_tmp68F->f2;}};_LL345: {int _tmp6C2=Cyc_Tcutil_typecmp(_tmp68C,_tmp690);if(
_tmp6C2 != 0)return _tmp6C2;else{return Cyc_Tcutil_typecmp(_tmp68D,_tmp691);}}
_LL346: _tmp692=_tmp5ED.f1;{struct Cyc_Absyn_TagType_struct*_tmp693=(struct Cyc_Absyn_TagType_struct*)
_tmp692;if(_tmp693->tag != 20)goto _LL348;else{_tmp694=(void*)_tmp693->f1;}};
_tmp695=_tmp5ED.f2;{struct Cyc_Absyn_TagType_struct*_tmp696=(struct Cyc_Absyn_TagType_struct*)
_tmp695;if(_tmp696->tag != 20)goto _LL348;else{_tmp697=(void*)_tmp696->f1;}};
_LL347: return Cyc_Tcutil_typecmp(_tmp694,_tmp697);_LL348: _tmp698=_tmp5ED.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp699=(struct Cyc_Absyn_ValueofType_struct*)
_tmp698;if(_tmp699->tag != 19)goto _LL34A;else{_tmp69A=_tmp699->f1;}};_tmp69B=
_tmp5ED.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp69C=(struct Cyc_Absyn_ValueofType_struct*)
_tmp69B;if(_tmp69C->tag != 19)goto _LL34A;else{_tmp69D=_tmp69C->f1;}};_LL349:
return Cyc_Evexp_const_exp_cmp(_tmp69A,_tmp69D);_LL34A: _tmp69E=_tmp5ED.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp69F=(struct Cyc_Absyn_JoinEff_struct*)_tmp69E;if(
_tmp69F->tag != 24)goto _LL34C;};_LL34B: goto _LL34D;_LL34C: _tmp6A0=_tmp5ED.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp6A1=(struct Cyc_Absyn_JoinEff_struct*)_tmp6A0;
if(_tmp6A1->tag != 24)goto _LL34E;};_LL34D: goto _LL34F;_LL34E: _tmp6A2=_tmp5ED.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp6A3=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6A2;if(_tmp6A3->tag != 23)goto _LL350;};_LL34F: goto _LL351;_LL350: _tmp6A4=
_tmp5ED.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp6A5=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6A4;if(_tmp6A5->tag != 25)goto _LL352;};_LL351: goto _LL353;_LL352: _tmp6A6=
_tmp5ED.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp6A7=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6A6;if(_tmp6A7->tag != 25)goto _LL354;};_LL353: goto _LL355;_LL354: _tmp6A8=
_tmp5ED.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp6A9=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6A8;if(_tmp6A9->tag != 23)goto _LL356;};_LL355: {const char*_tmp1294;void*
_tmp1293;(_tmp1293=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1294="typecmp: effects not handled",_tag_dyneither(
_tmp1294,sizeof(char),29))),_tag_dyneither(_tmp1293,sizeof(void*),0)));}_LL356:;
_LL357: {const char*_tmp1297;void*_tmp1296;(_tmp1296=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1297="Unmatched case in typecmp",
_tag_dyneither(_tmp1297,sizeof(char),26))),_tag_dyneither(_tmp1296,sizeof(void*),
0)));}_LL323:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6C7=Cyc_Tcutil_compress(
t);_LL35E: {struct Cyc_Absyn_IntType_struct*_tmp6C8=(struct Cyc_Absyn_IntType_struct*)
_tmp6C7;if(_tmp6C8->tag != 6)goto _LL360;}_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_FloatType_struct*
_tmp6C9=(struct Cyc_Absyn_FloatType_struct*)_tmp6C7;if(_tmp6C9->tag != 7)goto
_LL362;}_LL361: goto _LL363;_LL362: {struct Cyc_Absyn_DoubleType_struct*_tmp6CA=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6C7;if(_tmp6CA->tag != 8)goto _LL364;}
_LL363: goto _LL365;_LL364: {struct Cyc_Absyn_EnumType_struct*_tmp6CB=(struct Cyc_Absyn_EnumType_struct*)
_tmp6C7;if(_tmp6CB->tag != 14)goto _LL366;}_LL365: goto _LL367;_LL366: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp6CC=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp6C7;if(_tmp6CC->tag != 15)goto
_LL368;}_LL367: return 1;_LL368:;_LL369: return 0;_LL35D:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1298;struct _tuple0 _tmp6CE=(_tmp1298.f1=t1,((_tmp1298.f2=t2,_tmp1298)));
void*_tmp6CF;int _tmp6D1;void*_tmp6D2;int _tmp6D4;void*_tmp6D5;void*_tmp6D7;void*
_tmp6D9;void*_tmp6DB;void*_tmp6DD;void*_tmp6DF;void*_tmp6E1;void*_tmp6E3;void*
_tmp6E5;void*_tmp6E7;void*_tmp6E9;enum Cyc_Absyn_Size_of _tmp6EB;void*_tmp6EC;
enum Cyc_Absyn_Size_of _tmp6EE;void*_tmp6EF;enum Cyc_Absyn_Size_of _tmp6F1;void*
_tmp6F2;enum Cyc_Absyn_Size_of _tmp6F4;void*_tmp6F5;enum Cyc_Absyn_Size_of _tmp6F7;
void*_tmp6F8;enum Cyc_Absyn_Size_of _tmp6FA;void*_tmp6FB;enum Cyc_Absyn_Size_of
_tmp6FD;void*_tmp6FE;enum Cyc_Absyn_Size_of _tmp700;void*_tmp701;void*_tmp703;
enum Cyc_Absyn_Size_of _tmp705;void*_tmp706;void*_tmp708;enum Cyc_Absyn_Size_of
_tmp70A;void*_tmp70B;enum Cyc_Absyn_Size_of _tmp70D;void*_tmp70E;enum Cyc_Absyn_Size_of
_tmp710;void*_tmp711;enum Cyc_Absyn_Size_of _tmp713;void*_tmp714;void*_tmp716;
enum Cyc_Absyn_Size_of _tmp718;void*_tmp719;enum Cyc_Absyn_Size_of _tmp71B;void*
_tmp71C;enum Cyc_Absyn_Size_of _tmp71E;void*_tmp71F;enum Cyc_Absyn_Size_of _tmp721;
void*_tmp722;enum Cyc_Absyn_Size_of _tmp724;void*_tmp725;enum Cyc_Absyn_Size_of
_tmp727;void*_tmp728;enum Cyc_Absyn_Size_of _tmp72A;void*_tmp72B;void*_tmp72D;
enum Cyc_Absyn_Size_of _tmp72F;_LL36B: _tmp6CF=_tmp6CE.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6D0=(struct Cyc_Absyn_DoubleType_struct*)_tmp6CF;if(_tmp6D0->tag != 8)goto
_LL36D;else{_tmp6D1=_tmp6D0->f1;}};_tmp6D2=_tmp6CE.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp6D3=(struct Cyc_Absyn_DoubleType_struct*)_tmp6D2;if(_tmp6D3->tag != 8)goto
_LL36D;else{_tmp6D4=_tmp6D3->f1;}};_LL36C: return !_tmp6D4  && _tmp6D1;_LL36D:
_tmp6D5=_tmp6CE.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6D6=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6D5;if(_tmp6D6->tag != 8)goto _LL36F;};_tmp6D7=_tmp6CE.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6D8=(struct Cyc_Absyn_FloatType_struct*)_tmp6D7;if(_tmp6D8->tag != 7)goto
_LL36F;};_LL36E: goto _LL370;_LL36F: _tmp6D9=_tmp6CE.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6DA=(struct Cyc_Absyn_DoubleType_struct*)_tmp6D9;if(_tmp6DA->tag != 8)goto
_LL371;};_tmp6DB=_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp6DC=(struct Cyc_Absyn_IntType_struct*)
_tmp6DB;if(_tmp6DC->tag != 6)goto _LL371;};_LL370: goto _LL372;_LL371: _tmp6DD=
_tmp6CE.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6DE=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6DD;if(_tmp6DE->tag != 8)goto _LL373;};_tmp6DF=_tmp6CE.f2;{struct Cyc_Absyn_TagType_struct*
_tmp6E0=(struct Cyc_Absyn_TagType_struct*)_tmp6DF;if(_tmp6E0->tag != 20)goto _LL373;};
_LL372: goto _LL374;_LL373: _tmp6E1=_tmp6CE.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6E2=(struct Cyc_Absyn_FloatType_struct*)_tmp6E1;if(_tmp6E2->tag != 7)goto
_LL375;};_tmp6E3=_tmp6CE.f2;{struct Cyc_Absyn_TagType_struct*_tmp6E4=(struct Cyc_Absyn_TagType_struct*)
_tmp6E3;if(_tmp6E4->tag != 20)goto _LL375;};_LL374: goto _LL376;_LL375: _tmp6E5=
_tmp6CE.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6E6=(struct Cyc_Absyn_FloatType_struct*)
_tmp6E5;if(_tmp6E6->tag != 7)goto _LL377;};_tmp6E7=_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6E8=(struct Cyc_Absyn_IntType_struct*)_tmp6E7;if(_tmp6E8->tag != 6)goto _LL377;};
_LL376: return 1;_LL377: _tmp6E9=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*_tmp6EA=(
struct Cyc_Absyn_IntType_struct*)_tmp6E9;if(_tmp6EA->tag != 6)goto _LL379;else{
_tmp6EB=_tmp6EA->f2;if(_tmp6EB != Cyc_Absyn_LongLong_sz)goto _LL379;}};_tmp6EC=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp6ED=(struct Cyc_Absyn_IntType_struct*)
_tmp6EC;if(_tmp6ED->tag != 6)goto _LL379;else{_tmp6EE=_tmp6ED->f2;if(_tmp6EE != Cyc_Absyn_LongLong_sz)
goto _LL379;}};_LL378: return 0;_LL379: _tmp6EF=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6F0=(struct Cyc_Absyn_IntType_struct*)_tmp6EF;if(_tmp6F0->tag != 6)goto _LL37B;
else{_tmp6F1=_tmp6F0->f2;if(_tmp6F1 != Cyc_Absyn_LongLong_sz)goto _LL37B;}};_LL37A:
return 1;_LL37B: _tmp6F2=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*_tmp6F3=(
struct Cyc_Absyn_IntType_struct*)_tmp6F2;if(_tmp6F3->tag != 6)goto _LL37D;else{
_tmp6F4=_tmp6F3->f2;if(_tmp6F4 != Cyc_Absyn_Long_sz)goto _LL37D;}};_tmp6F5=_tmp6CE.f2;{
struct Cyc_Absyn_IntType_struct*_tmp6F6=(struct Cyc_Absyn_IntType_struct*)_tmp6F5;
if(_tmp6F6->tag != 6)goto _LL37D;else{_tmp6F7=_tmp6F6->f2;if(_tmp6F7 != Cyc_Absyn_Int_sz)
goto _LL37D;}};_LL37C: goto _LL37E;_LL37D: _tmp6F8=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6F9=(struct Cyc_Absyn_IntType_struct*)_tmp6F8;if(_tmp6F9->tag != 6)goto _LL37F;
else{_tmp6FA=_tmp6F9->f2;if(_tmp6FA != Cyc_Absyn_Int_sz)goto _LL37F;}};_tmp6FB=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp6FC=(struct Cyc_Absyn_IntType_struct*)
_tmp6FB;if(_tmp6FC->tag != 6)goto _LL37F;else{_tmp6FD=_tmp6FC->f2;if(_tmp6FD != Cyc_Absyn_Long_sz)
goto _LL37F;}};_LL37E: return 0;_LL37F: _tmp6FE=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp6FF=(struct Cyc_Absyn_IntType_struct*)_tmp6FE;if(_tmp6FF->tag != 6)goto _LL381;
else{_tmp700=_tmp6FF->f2;if(_tmp700 != Cyc_Absyn_Long_sz)goto _LL381;}};_tmp701=
_tmp6CE.f2;{struct Cyc_Absyn_FloatType_struct*_tmp702=(struct Cyc_Absyn_FloatType_struct*)
_tmp701;if(_tmp702->tag != 7)goto _LL381;};_LL380: goto _LL382;_LL381: _tmp703=
_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*_tmp704=(struct Cyc_Absyn_IntType_struct*)
_tmp703;if(_tmp704->tag != 6)goto _LL383;else{_tmp705=_tmp704->f2;if(_tmp705 != Cyc_Absyn_Int_sz)
goto _LL383;}};_tmp706=_tmp6CE.f2;{struct Cyc_Absyn_FloatType_struct*_tmp707=(
struct Cyc_Absyn_FloatType_struct*)_tmp706;if(_tmp707->tag != 7)goto _LL383;};
_LL382: goto _LL384;_LL383: _tmp708=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp709=(struct Cyc_Absyn_IntType_struct*)_tmp708;if(_tmp709->tag != 6)goto _LL385;
else{_tmp70A=_tmp709->f2;if(_tmp70A != Cyc_Absyn_Long_sz)goto _LL385;}};_tmp70B=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp70C=(struct Cyc_Absyn_IntType_struct*)
_tmp70B;if(_tmp70C->tag != 6)goto _LL385;else{_tmp70D=_tmp70C->f2;if(_tmp70D != Cyc_Absyn_Short_sz)
goto _LL385;}};_LL384: goto _LL386;_LL385: _tmp70E=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp70F=(struct Cyc_Absyn_IntType_struct*)_tmp70E;if(_tmp70F->tag != 6)goto _LL387;
else{_tmp710=_tmp70F->f2;if(_tmp710 != Cyc_Absyn_Int_sz)goto _LL387;}};_tmp711=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp712=(struct Cyc_Absyn_IntType_struct*)
_tmp711;if(_tmp712->tag != 6)goto _LL387;else{_tmp713=_tmp712->f2;if(_tmp713 != Cyc_Absyn_Short_sz)
goto _LL387;}};_LL386: goto _LL388;_LL387: _tmp714=_tmp6CE.f1;{struct Cyc_Absyn_TagType_struct*
_tmp715=(struct Cyc_Absyn_TagType_struct*)_tmp714;if(_tmp715->tag != 20)goto _LL389;};
_tmp716=_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp717=(struct Cyc_Absyn_IntType_struct*)
_tmp716;if(_tmp717->tag != 6)goto _LL389;else{_tmp718=_tmp717->f2;if(_tmp718 != Cyc_Absyn_Short_sz)
goto _LL389;}};_LL388: goto _LL38A;_LL389: _tmp719=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp71A=(struct Cyc_Absyn_IntType_struct*)_tmp719;if(_tmp71A->tag != 6)goto _LL38B;
else{_tmp71B=_tmp71A->f2;if(_tmp71B != Cyc_Absyn_Long_sz)goto _LL38B;}};_tmp71C=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp71D=(struct Cyc_Absyn_IntType_struct*)
_tmp71C;if(_tmp71D->tag != 6)goto _LL38B;else{_tmp71E=_tmp71D->f2;if(_tmp71E != Cyc_Absyn_Char_sz)
goto _LL38B;}};_LL38A: goto _LL38C;_LL38B: _tmp71F=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp720=(struct Cyc_Absyn_IntType_struct*)_tmp71F;if(_tmp720->tag != 6)goto _LL38D;
else{_tmp721=_tmp720->f2;if(_tmp721 != Cyc_Absyn_Int_sz)goto _LL38D;}};_tmp722=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp723=(struct Cyc_Absyn_IntType_struct*)
_tmp722;if(_tmp723->tag != 6)goto _LL38D;else{_tmp724=_tmp723->f2;if(_tmp724 != Cyc_Absyn_Char_sz)
goto _LL38D;}};_LL38C: goto _LL38E;_LL38D: _tmp725=_tmp6CE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp726=(struct Cyc_Absyn_IntType_struct*)_tmp725;if(_tmp726->tag != 6)goto _LL38F;
else{_tmp727=_tmp726->f2;if(_tmp727 != Cyc_Absyn_Short_sz)goto _LL38F;}};_tmp728=
_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp729=(struct Cyc_Absyn_IntType_struct*)
_tmp728;if(_tmp729->tag != 6)goto _LL38F;else{_tmp72A=_tmp729->f2;if(_tmp72A != Cyc_Absyn_Char_sz)
goto _LL38F;}};_LL38E: goto _LL390;_LL38F: _tmp72B=_tmp6CE.f1;{struct Cyc_Absyn_TagType_struct*
_tmp72C=(struct Cyc_Absyn_TagType_struct*)_tmp72B;if(_tmp72C->tag != 20)goto _LL391;};
_tmp72D=_tmp6CE.f2;{struct Cyc_Absyn_IntType_struct*_tmp72E=(struct Cyc_Absyn_IntType_struct*)
_tmp72D;if(_tmp72E->tag != 6)goto _LL391;else{_tmp72F=_tmp72E->f2;if(_tmp72F != Cyc_Absyn_Char_sz)
goto _LL391;}};_LL390: return 1;_LL391:;_LL392: return 0;_LL36A:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp730=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1299;max_arith_type=((_tmp1299=
_region_malloc(_tmp730,sizeof(*_tmp1299)),((_tmp1299->v=t1,_tmp1299))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp129E;void*_tmp129D[2];struct
Cyc_String_pa_struct _tmp129C;struct Cyc_String_pa_struct _tmp129B;(_tmp129B.tag=0,((
_tmp129B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp129C.tag=0,((_tmp129C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp129D[0]=& _tmp129C,((_tmp129D[1]=& _tmp129B,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp129E="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp129E,sizeof(char),41))),_tag_dyneither(_tmp129D,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp736=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL394: {struct Cyc_Absyn_PointerType_struct*
_tmp737=(struct Cyc_Absyn_PointerType_struct*)_tmp736;if(_tmp737->tag != 5)goto
_LL396;}_LL395: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL393;_LL396:;_LL397: return 0;_LL393:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp738=Cyc_Tcutil_compress(t);_LL399: {struct Cyc_Absyn_IntType_struct*
_tmp739=(struct Cyc_Absyn_IntType_struct*)_tmp738;if(_tmp739->tag != 6)goto _LL39B;}
_LL39A: goto _LL39C;_LL39B: {struct Cyc_Absyn_TagType_struct*_tmp73A=(struct Cyc_Absyn_TagType_struct*)
_tmp738;if(_tmp73A->tag != 20)goto _LL39D;}_LL39C: goto _LL39E;_LL39D: {struct Cyc_Absyn_EnumType_struct*
_tmp73B=(struct Cyc_Absyn_EnumType_struct*)_tmp738;if(_tmp73B->tag != 14)goto
_LL39F;}_LL39E: goto _LL3A0;_LL39F: {struct Cyc_Absyn_AnonEnumType_struct*_tmp73C=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp738;if(_tmp73C->tag != 15)goto _LL3A1;}
_LL3A0: return 1;_LL3A1:;_LL3A2: return 0;_LL398:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp12A1;void*_tmp12A0;(_tmp12A0=0,Cyc_Tcutil_warn(e->loc,((_tmp12A1="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12A1,sizeof(char),44))),_tag_dyneither(_tmp12A0,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12A4;void*_tmp12A3;(
_tmp12A3=0,Cyc_Tcutil_warn(e->loc,((_tmp12A4="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12A4,sizeof(char),44))),_tag_dyneither(_tmp12A3,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12A5;struct _tuple0 _tmp742=(_tmp12A5.f1=
t1,((_tmp12A5.f2=t2,_tmp12A5)));void*_tmp743;struct Cyc_Absyn_PtrInfo _tmp745;void*
_tmp746;struct Cyc_Absyn_PtrInfo _tmp748;void*_tmp749;struct Cyc_Absyn_ArrayInfo
_tmp74B;void*_tmp74C;struct Cyc_Absyn_Tqual _tmp74D;struct Cyc_Absyn_Exp*_tmp74E;
union Cyc_Absyn_Constraint*_tmp74F;void*_tmp750;struct Cyc_Absyn_ArrayInfo _tmp752;
void*_tmp753;struct Cyc_Absyn_Tqual _tmp754;struct Cyc_Absyn_Exp*_tmp755;union Cyc_Absyn_Constraint*
_tmp756;void*_tmp757;void*_tmp759;_LL3A4: _tmp743=_tmp742.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp744=(struct Cyc_Absyn_PointerType_struct*)_tmp743;if(_tmp744->tag != 5)goto
_LL3A6;else{_tmp745=_tmp744->f1;}};_tmp746=_tmp742.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp747=(struct Cyc_Absyn_PointerType_struct*)_tmp746;if(_tmp747->tag != 5)goto
_LL3A6;else{_tmp748=_tmp747->f1;}};_LL3A5: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp745.ptr_atts).nullable,(_tmp748.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp745.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp745.ptr_atts).bounds,(
_tmp748.ptr_atts).bounds)){struct _tuple0 _tmp12A6;struct _tuple0 _tmp75C=(_tmp12A6.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp745.ptr_atts).bounds),((_tmp12A6.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp748.ptr_atts).bounds),
_tmp12A6)));void*_tmp75D;void*_tmp75F;void*_tmp761;void*_tmp763;void*_tmp765;
struct Cyc_Absyn_Exp*_tmp767;void*_tmp768;struct Cyc_Absyn_Exp*_tmp76A;void*
_tmp76B;void*_tmp76D;struct Cyc_Absyn_Exp*_tmp76F;_LL3AD: _tmp75D=_tmp75C.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp75E=(struct Cyc_Absyn_Upper_b_struct*)_tmp75D;
if(_tmp75E->tag != 1)goto _LL3AF;};_tmp75F=_tmp75C.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp760=(struct Cyc_Absyn_DynEither_b_struct*)_tmp75F;if(_tmp760->tag != 0)goto
_LL3AF;};_LL3AE: goto _LL3B0;_LL3AF: _tmp761=_tmp75C.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp762=(struct Cyc_Absyn_DynEither_b_struct*)_tmp761;if(_tmp762->tag != 0)goto
_LL3B1;};_tmp763=_tmp75C.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp764=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp763;if(_tmp764->tag != 0)goto _LL3B1;};_LL3B0:
okay=1;goto _LL3AC;_LL3B1: _tmp765=_tmp75C.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp766=(struct Cyc_Absyn_Upper_b_struct*)_tmp765;if(_tmp766->tag != 1)goto _LL3B3;
else{_tmp767=_tmp766->f1;}};_tmp768=_tmp75C.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp769=(struct Cyc_Absyn_Upper_b_struct*)_tmp768;if(_tmp769->tag != 1)goto _LL3B3;
else{_tmp76A=_tmp769->f1;}};_LL3B2: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp76A,
_tmp767);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp748.ptr_atts).zero_term)){const char*_tmp12A9;void*_tmp12A8;(_tmp12A8=0,
Cyc_Tcutil_warn(loc,((_tmp12A9="implicit cast to shorter array",_tag_dyneither(
_tmp12A9,sizeof(char),31))),_tag_dyneither(_tmp12A8,sizeof(void*),0)));}goto
_LL3AC;_LL3B3: _tmp76B=_tmp75C.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp76C=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp76B;if(_tmp76C->tag != 0)goto _LL3AC;};
_tmp76D=_tmp75C.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp76E=(struct Cyc_Absyn_Upper_b_struct*)
_tmp76D;if(_tmp76E->tag != 1)goto _LL3AC;else{_tmp76F=_tmp76E->f1;}};_LL3B4: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp745.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp748.ptr_atts).bounds))goto _LL3AC;okay=0;goto
_LL3AC;_LL3AC:;}okay=okay  && (!(_tmp745.elt_tq).real_const  || (_tmp748.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp745.ptr_atts).rgn,(_tmp748.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp745.ptr_atts).rgn,(_tmp748.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp745.ptr_atts).zero_term,(
_tmp748.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp745.ptr_atts).zero_term) && (_tmp748.elt_tq).real_const);{int _tmp772=((
int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp748.ptr_atts).bounds,
Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp748.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp745.elt_typ,
_tmp748.elt_typ) || (_tmp772  && ((_tmp748.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp748.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp745.elt_typ,_tmp748.elt_typ));return okay;};}_LL3A6: _tmp749=_tmp742.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp74A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp749;if(_tmp74A->tag != 9)goto _LL3A8;else{_tmp74B=_tmp74A->f1;_tmp74C=_tmp74B.elt_type;
_tmp74D=_tmp74B.tq;_tmp74E=_tmp74B.num_elts;_tmp74F=_tmp74B.zero_term;}};_tmp750=
_tmp742.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp751=(struct Cyc_Absyn_ArrayType_struct*)
_tmp750;if(_tmp751->tag != 9)goto _LL3A8;else{_tmp752=_tmp751->f1;_tmp753=_tmp752.elt_type;
_tmp754=_tmp752.tq;_tmp755=_tmp752.num_elts;_tmp756=_tmp752.zero_term;}};_LL3A7: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp74F,_tmp756) && ((_tmp74E != 0
 && _tmp755 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp74E,(
struct Cyc_Absyn_Exp*)_tmp755));return(okay  && Cyc_Tcutil_unify(_tmp74C,_tmp753))
 && (!_tmp74D.real_const  || _tmp754.real_const);}_LL3A8: _tmp757=_tmp742.f1;{
struct Cyc_Absyn_TagType_struct*_tmp758=(struct Cyc_Absyn_TagType_struct*)_tmp757;
if(_tmp758->tag != 20)goto _LL3AA;};_tmp759=_tmp742.f2;{struct Cyc_Absyn_IntType_struct*
_tmp75A=(struct Cyc_Absyn_IntType_struct*)_tmp759;if(_tmp75A->tag != 6)goto _LL3AA;};
_LL3A9: return 0;_LL3AA:;_LL3AB: return Cyc_Tcutil_unify(t1,t2);_LL3A3:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp773=Cyc_Tcutil_compress(t);_LL3B6: {struct Cyc_Absyn_PointerType_struct*
_tmp774=(struct Cyc_Absyn_PointerType_struct*)_tmp773;if(_tmp774->tag != 5)goto
_LL3B8;}_LL3B7: return 1;_LL3B8:;_LL3B9: return 0;_LL3B5:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp775=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp777;void*
_tmp778;_LL3BB: {struct Cyc_Absyn_PointerType_struct*_tmp776=(struct Cyc_Absyn_PointerType_struct*)
_tmp775;if(_tmp776->tag != 5)goto _LL3BD;else{_tmp777=_tmp776->f1;_tmp778=_tmp777.elt_typ;}}
_LL3BC: return _tmp778;_LL3BD:;_LL3BE: {const char*_tmp12AC;void*_tmp12AB;(_tmp12AB=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12AC="pointer_elt_type",_tag_dyneither(_tmp12AC,sizeof(char),17))),
_tag_dyneither(_tmp12AB,sizeof(void*),0)));}_LL3BA:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp77B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp77D;
struct Cyc_Absyn_PtrAtts _tmp77E;struct Cyc_Absyn_PtrAtts*_tmp77F;_LL3C0: {struct
Cyc_Absyn_PointerType_struct*_tmp77C=(struct Cyc_Absyn_PointerType_struct*)
_tmp77B;if(_tmp77C->tag != 5)goto _LL3C2;else{_tmp77D=_tmp77C->f1;_tmp77E=_tmp77D.ptr_atts;
_tmp77F=(struct Cyc_Absyn_PtrAtts*)&(_tmp77C->f1).ptr_atts;}}_LL3C1: return _tmp77F->rgn;
_LL3C2:;_LL3C3: {const char*_tmp12AF;void*_tmp12AE;(_tmp12AE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AF="pointer_elt_type",
_tag_dyneither(_tmp12AF,sizeof(char),17))),_tag_dyneither(_tmp12AE,sizeof(void*),
0)));}_LL3BF:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp782=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp784;struct Cyc_Absyn_PtrAtts _tmp785;void*_tmp786;
_LL3C5: {struct Cyc_Absyn_PointerType_struct*_tmp783=(struct Cyc_Absyn_PointerType_struct*)
_tmp782;if(_tmp783->tag != 5)goto _LL3C7;else{_tmp784=_tmp783->f1;_tmp785=_tmp784.ptr_atts;
_tmp786=_tmp785.rgn;}}_LL3C6:*rgn=_tmp786;return 1;_LL3C7:;_LL3C8: return 0;_LL3C4:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp787=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp789;struct Cyc_Absyn_PtrAtts _tmp78A;union Cyc_Absyn_Constraint*
_tmp78B;_LL3CA: {struct Cyc_Absyn_PointerType_struct*_tmp788=(struct Cyc_Absyn_PointerType_struct*)
_tmp787;if(_tmp788->tag != 5)goto _LL3CC;else{_tmp789=_tmp788->f1;_tmp78A=_tmp789.ptr_atts;
_tmp78B=_tmp78A.bounds;}}_LL3CB:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp78B)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3CC:;_LL3CD: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3C9:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp78C=e->r;union Cyc_Absyn_Cnst
_tmp78E;struct _tuple7 _tmp78F;int _tmp790;union Cyc_Absyn_Cnst _tmp792;struct _tuple5
_tmp793;char _tmp794;union Cyc_Absyn_Cnst _tmp796;struct _tuple6 _tmp797;short _tmp798;
union Cyc_Absyn_Cnst _tmp79A;struct _tuple8 _tmp79B;long long _tmp79C;void*_tmp79E;
struct Cyc_Absyn_Exp*_tmp79F;_LL3CF: {struct Cyc_Absyn_Const_e_struct*_tmp78D=(
struct Cyc_Absyn_Const_e_struct*)_tmp78C;if(_tmp78D->tag != 0)goto _LL3D1;else{
_tmp78E=_tmp78D->f1;if((_tmp78E.Int_c).tag != 4)goto _LL3D1;_tmp78F=(struct _tuple7)(
_tmp78E.Int_c).val;_tmp790=_tmp78F.f2;if(_tmp790 != 0)goto _LL3D1;}}_LL3D0: goto
_LL3D2;_LL3D1: {struct Cyc_Absyn_Const_e_struct*_tmp791=(struct Cyc_Absyn_Const_e_struct*)
_tmp78C;if(_tmp791->tag != 0)goto _LL3D3;else{_tmp792=_tmp791->f1;if((_tmp792.Char_c).tag
!= 2)goto _LL3D3;_tmp793=(struct _tuple5)(_tmp792.Char_c).val;_tmp794=_tmp793.f2;
if(_tmp794 != 0)goto _LL3D3;}}_LL3D2: goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Const_e_struct*
_tmp795=(struct Cyc_Absyn_Const_e_struct*)_tmp78C;if(_tmp795->tag != 0)goto _LL3D5;
else{_tmp796=_tmp795->f1;if((_tmp796.Short_c).tag != 3)goto _LL3D5;_tmp797=(struct
_tuple6)(_tmp796.Short_c).val;_tmp798=_tmp797.f2;if(_tmp798 != 0)goto _LL3D5;}}
_LL3D4: goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Const_e_struct*_tmp799=(struct Cyc_Absyn_Const_e_struct*)
_tmp78C;if(_tmp799->tag != 0)goto _LL3D7;else{_tmp79A=_tmp799->f1;if((_tmp79A.LongLong_c).tag
!= 5)goto _LL3D7;_tmp79B=(struct _tuple8)(_tmp79A.LongLong_c).val;_tmp79C=_tmp79B.f2;
if(_tmp79C != 0)goto _LL3D7;}}_LL3D6: return 1;_LL3D7: {struct Cyc_Absyn_Cast_e_struct*
_tmp79D=(struct Cyc_Absyn_Cast_e_struct*)_tmp78C;if(_tmp79D->tag != 15)goto _LL3D9;
else{_tmp79E=(void*)_tmp79D->f1;_tmp79F=_tmp79D->f2;}}_LL3D8: return Cyc_Tcutil_is_zero(
_tmp79F) && Cyc_Tcutil_admits_zero(_tmp79E);_LL3D9:;_LL3DA: return 0;_LL3CE:;}
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
_tmp7A1;enum Cyc_Absyn_KindQual _tmp7A2;enum Cyc_Absyn_AliasQual _tmp7A3;struct Cyc_Absyn_Kind*
_tmp7A0=ka;_tmp7A1=*_tmp7A0;_tmp7A2=_tmp7A1.kind;_tmp7A3=_tmp7A1.aliasqual;
switch(_tmp7A3){case Cyc_Absyn_Aliasable: _LL3DB: switch(_tmp7A2){case Cyc_Absyn_AnyKind:
_LL3DD: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3DE:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3DF: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3E0: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3E1: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3E2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3DC: switch(_tmp7A2){case Cyc_Absyn_AnyKind: _LL3E5: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3E6: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3E7: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3E8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3E9: break;}break;case Cyc_Absyn_Top: _LL3E4: switch(_tmp7A2){case Cyc_Absyn_AnyKind:
_LL3EC: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3ED:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3EE: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3EF: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3F0: break;}break;}{const char*_tmp12B3;
void*_tmp12B2[1];struct Cyc_String_pa_struct _tmp12B1;(_tmp12B1.tag=0,((_tmp12B1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp12B2[0]=& _tmp12B1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12B3="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp12B3,sizeof(char),26))),_tag_dyneither(_tmp12B2,sizeof(void*),1)))))));};}
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
_tmp7A8;enum Cyc_Absyn_KindQual _tmp7A9;enum Cyc_Absyn_AliasQual _tmp7AA;struct Cyc_Absyn_Kind*
_tmp7A7=ka;_tmp7A8=*_tmp7A7;_tmp7A9=_tmp7A8.kind;_tmp7AA=_tmp7A8.aliasqual;
switch(_tmp7AA){case Cyc_Absyn_Aliasable: _LL3F2: switch(_tmp7A9){case Cyc_Absyn_AnyKind:
_LL3F4: return& ab;case Cyc_Absyn_MemKind: _LL3F5: return& mb;case Cyc_Absyn_BoxKind:
_LL3F6: return& bb;case Cyc_Absyn_RgnKind: _LL3F7: return& rb;case Cyc_Absyn_EffKind:
_LL3F8: return& eb;case Cyc_Absyn_IntKind: _LL3F9: return& ib;}case Cyc_Absyn_Unique:
_LL3F3: switch(_tmp7A9){case Cyc_Absyn_AnyKind: _LL3FC: return& uab;case Cyc_Absyn_MemKind:
_LL3FD: return& umb;case Cyc_Absyn_BoxKind: _LL3FE: return& ubb;case Cyc_Absyn_RgnKind:
_LL3FF: return& urb;default: _LL400: break;}break;case Cyc_Absyn_Top: _LL3FB: switch(
_tmp7A9){case Cyc_Absyn_AnyKind: _LL403: return& tab;case Cyc_Absyn_MemKind: _LL404:
return& tmb;case Cyc_Absyn_BoxKind: _LL405: return& tbb;case Cyc_Absyn_RgnKind: _LL406:
return& trb;default: _LL407: break;}break;}{const char*_tmp12B7;void*_tmp12B6[1];
struct Cyc_String_pa_struct _tmp12B5;(_tmp12B5.tag=0,((_tmp12B5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12B6[0]=&
_tmp12B5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12B7="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12B7,sizeof(char),24))),
_tag_dyneither(_tmp12B6,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){struct Cyc_Core_Opt*_tmp12B8;return(_tmp12B8=_cycalloc(
sizeof(*_tmp12B8)),((_tmp12B8->v=Cyc_Tcutil_kind_to_bound(k),_tmp12B8)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12BB;struct Cyc_Absyn_Const_e_struct*
_tmp12BA;e1->r=(void*)((_tmp12BA=_cycalloc(sizeof(*_tmp12BA)),((_tmp12BA[0]=((
_tmp12BB.tag=0,((_tmp12BB.f1=Cyc_Absyn_Null_c,_tmp12BB)))),_tmp12BA))));}{struct
Cyc_Core_Opt*_tmp7BF=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12C5;struct Cyc_Absyn_PtrAtts _tmp12C4;struct Cyc_Absyn_PtrInfo _tmp12C3;struct
Cyc_Absyn_PointerType_struct*_tmp12C2;struct Cyc_Absyn_PointerType_struct*_tmp7C0=(
_tmp12C2=_cycalloc(sizeof(*_tmp12C2)),((_tmp12C2[0]=((_tmp12C5.tag=5,((_tmp12C5.f1=((
_tmp12C3.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7BF),((
_tmp12C3.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12C3.ptr_atts=((_tmp12C4.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7BF),((_tmp12C4.nullable=Cyc_Absyn_true_conref,((
_tmp12C4.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12C4.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12C4.ptrloc=0,_tmp12C4)))))))))),_tmp12C3)))))),_tmp12C5)))),_tmp12C2)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7C0);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp12CA;void*_tmp12C9[2];struct Cyc_String_pa_struct _tmp12C8;struct Cyc_String_pa_struct
_tmp12C7;(_tmp12C7.tag=0,((_tmp12C7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12C8.tag=0,((
_tmp12C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12C9[0]=& _tmp12C8,((_tmp12C9[1]=& _tmp12C7,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12CA="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12CA,sizeof(char),56))),_tag_dyneither(_tmp12C9,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL409: {
const char*_tmp12CB;return(_tmp12CB="unknown",_tag_dyneither(_tmp12CB,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL40A: {const char*_tmp12CC;return(_tmp12CC="no coercion",
_tag_dyneither(_tmp12CC,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL40B: {
const char*_tmp12CD;return(_tmp12CD="null check",_tag_dyneither(_tmp12CD,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL40C: {const char*_tmp12CE;return(
_tmp12CE="other coercion",_tag_dyneither(_tmp12CE,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp12DB;const char*_tmp12DA;void*_tmp12D9[1];struct Cyc_Int_pa_struct
_tmp12D8;struct _tuple2*_tmp12D7;struct _tuple2*v=(_tmp12D7=_cycalloc(sizeof(*
_tmp12D7)),((_tmp12D7->f1=Cyc_Absyn_Loc_n,((_tmp12D7->f2=((_tmp12DB=_cycalloc(
sizeof(*_tmp12DB)),((_tmp12DB[0]=(struct _dyneither_ptr)((_tmp12D8.tag=1,((
_tmp12D8.f1=(unsigned long)counter ++,((_tmp12D9[0]=& _tmp12D8,Cyc_aprintf(((
_tmp12DA="__aliasvar%d",_tag_dyneither(_tmp12DA,sizeof(char),13))),
_tag_dyneither(_tmp12D9,sizeof(void*),1)))))))),_tmp12DB)))),_tmp12D7)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp12DE;struct Cyc_Absyn_Local_b_struct*
_tmp12DD;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12DD=_cycalloc(
sizeof(*_tmp12DD)),((_tmp12DD[0]=((_tmp12DE.tag=4,((_tmp12DE.f1=vd,_tmp12DE)))),
_tmp12DD)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7CD=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7CF;struct Cyc_Absyn_PtrAtts
_tmp7D0;void*_tmp7D1;void**_tmp7D2;_LL40F: {struct Cyc_Absyn_PointerType_struct*
_tmp7CE=(struct Cyc_Absyn_PointerType_struct*)_tmp7CD;if(_tmp7CE->tag != 5)goto
_LL411;else{_tmp7CF=_tmp7CE->f1;_tmp7D0=_tmp7CF.ptr_atts;_tmp7D1=_tmp7D0.rgn;
_tmp7D2=(void**)&((_tmp7CE->f1).ptr_atts).rgn;}}_LL410:{struct Cyc_Absyn_VarType_struct
_tmp12E1;struct Cyc_Absyn_VarType_struct*_tmp12E0;*_tmp7D2=(void*)((_tmp12E0=
_cycalloc(sizeof(*_tmp12E0)),((_tmp12E0[0]=((_tmp12E1.tag=2,((_tmp12E1.f1=tv,
_tmp12E1)))),_tmp12E0))));}goto _LL40E;_LL411:;_LL412: goto _LL40E;_LL40E:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple14 _tmp12E2;return(_tmp12E2.f1=
d,((_tmp12E2.f2=ve,_tmp12E2)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7DE=_new_region("r");struct _RegionHandle*r=& _tmp7DE;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7DF=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7E1;struct Cyc_Absyn_PtrAtts _tmp7E2;void*_tmp7E3;_LL414: {struct Cyc_Absyn_PointerType_struct*
_tmp7E0=(struct Cyc_Absyn_PointerType_struct*)_tmp7DF;if(_tmp7E0->tag != 5)goto
_LL416;else{_tmp7E1=_tmp7E0->f1;_tmp7E2=_tmp7E1.ptr_atts;_tmp7E3=_tmp7E2.rgn;}}
_LL415: {void*_tmp7E5=Cyc_Tcutil_compress(_tmp7E3);_LL41B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp7E6=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7E5;if(_tmp7E6->tag != 21)goto _LL41D;}
_LL41C: {int _tmp7E7=0;_npop_handler(0);return _tmp7E7;}_LL41D:;_LL41E: {struct Cyc_Absyn_Kind*
_tmp7E8=Cyc_Tcutil_typ_kind(_tmp7E3);int _tmp7E9=_tmp7E8->kind == Cyc_Absyn_RgnKind
 && _tmp7E8->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7E9;}
_LL41A:;}_LL416: {struct Cyc_Absyn_TypedefType_struct*_tmp7E4=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7DF;if(_tmp7E4->tag != 18)goto _LL418;}_LL417: {const char*_tmp12E7;void*
_tmp12E6[2];struct Cyc_String_pa_struct _tmp12E5;struct Cyc_String_pa_struct
_tmp12E4;(_tmp12E4.tag=0,((_tmp12E4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp12E5.tag=0,((_tmp12E5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp12E6[0]=& _tmp12E5,((_tmp12E6[1]=& _tmp12E4,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E7="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp12E7,sizeof(char),42))),_tag_dyneither(_tmp12E6,sizeof(void*),
2)))))))))))));}_LL418:;_LL419: {int _tmp7EE=0;_npop_handler(0);return _tmp7EE;}
_LL413:;}{int _tmp7EF=0;_npop_handler(0);return _tmp7EF;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp12EC;void*_tmp12EB[2];
struct Cyc_String_pa_struct _tmp12EA;struct Cyc_String_pa_struct _tmp12E9;(_tmp12E9.tag=
0,((_tmp12E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp12EA.tag=0,((_tmp12EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp12EB[0]=& _tmp12EA,((_tmp12EB[1]=& _tmp12E9,Cyc_Tcutil_warn(
e->loc,((_tmp12EC="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp12EC,sizeof(char),53))),_tag_dyneither(_tmp12EB,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp12F2;void*_tmp12F1[3];struct Cyc_String_pa_struct _tmp12F0;struct Cyc_String_pa_struct
_tmp12EF;struct Cyc_String_pa_struct _tmp12EE;(_tmp12EE.tag=0,((_tmp12EE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp12EF.tag=0,((_tmp12EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp12F0.tag=0,((_tmp12F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp12F1[0]=& _tmp12F0,((_tmp12F1[1]=& _tmp12EF,((
_tmp12F1[2]=& _tmp12EE,Cyc_Tcutil_warn(e->loc,((_tmp12F2="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp12F2,sizeof(char),40))),_tag_dyneither(_tmp12F1,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp12F7;void*_tmp12F6[2];
struct Cyc_String_pa_struct _tmp12F5;struct Cyc_String_pa_struct _tmp12F4;(_tmp12F4.tag=
0,((_tmp12F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp12F5.tag=0,((_tmp12F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp12F6[0]=& _tmp12F5,((_tmp12F6[1]=& _tmp12F4,Cyc_Tcutil_warn(
e->loc,((_tmp12F7="implicit cast from %s to %s",_tag_dyneither(_tmp12F7,sizeof(
char),28))),_tag_dyneither(_tmp12F6,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp7FD=Cyc_Tcutil_compress(t);_LL420: {struct Cyc_Absyn_IntType_struct*
_tmp7FE=(struct Cyc_Absyn_IntType_struct*)_tmp7FD;if(_tmp7FE->tag != 6)goto _LL422;}
_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_FloatType_struct*_tmp7FF=(struct Cyc_Absyn_FloatType_struct*)
_tmp7FD;if(_tmp7FF->tag != 7)goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_DoubleType_struct*
_tmp800=(struct Cyc_Absyn_DoubleType_struct*)_tmp7FD;if(_tmp800->tag != 8)goto
_LL426;}_LL425: return 1;_LL426:;_LL427: return 0;_LL41F:;}static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,
void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*
f3;int f4;};static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,
struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp802;struct _RegionHandle*
_tmp803;struct Cyc_Tcenv_Tenv*_tmp804;int _tmp805;struct _tuple23 _tmp801=*env;
_tmp802=_tmp801.f1;_tmp803=_tmp801.f2;_tmp804=_tmp801.f3;_tmp805=_tmp801.f4;{
void*_tmp806=Cyc_Tcutil_rsubstitute(_tmp803,_tmp802,x->type);struct Cyc_List_List*
_tmp807=Cyc_Tcutil_flatten_typ(_tmp803,_tmp805,_tmp804,_tmp806);if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp807)== 1){struct _tuple11*_tmp12FA;
struct Cyc_List_List*_tmp12F9;return(_tmp12F9=_region_malloc(_tmp803,sizeof(*
_tmp12F9)),((_tmp12F9->hd=((_tmp12FA=_region_malloc(_tmp803,sizeof(*_tmp12FA)),((
_tmp12FA->f1=x->tq,((_tmp12FA->f2=_tmp806,_tmp12FA)))))),((_tmp12F9->tl=0,
_tmp12F9)))));}else{return _tmp807;}};}struct _tuple24{struct _RegionHandle*f1;
struct Cyc_Tcenv_Tenv*f2;int f3;};static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(
struct _tuple24*env,struct _tuple11*x){struct _RegionHandle*_tmp80B;struct Cyc_Tcenv_Tenv*
_tmp80C;int _tmp80D;struct _tuple24 _tmp80A=*env;_tmp80B=_tmp80A.f1;_tmp80C=_tmp80A.f2;
_tmp80D=_tmp80A.f3;{struct Cyc_Absyn_Tqual _tmp80F;void*_tmp810;struct _tuple11
_tmp80E=*x;_tmp80F=_tmp80E.f1;_tmp810=_tmp80E.f2;{struct Cyc_List_List*_tmp811=
Cyc_Tcutil_flatten_typ(_tmp80B,_tmp80D,_tmp80C,_tmp810);if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp811)== 1){struct _tuple11*_tmp12FD;struct Cyc_List_List*
_tmp12FC;return(_tmp12FC=_region_malloc(_tmp80B,sizeof(*_tmp12FC)),((_tmp12FC->hd=((
_tmp12FD=_region_malloc(_tmp80B,sizeof(*_tmp12FD)),((_tmp12FD->f1=_tmp80F,((
_tmp12FD->f2=_tmp810,_tmp12FD)))))),((_tmp12FC->tl=0,_tmp12FC)))));}else{return
_tmp811;}};};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct
_RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(
t1);{void*_tmp814=t1;struct Cyc_List_List*_tmp817;struct Cyc_Absyn_AggrInfo _tmp819;
union Cyc_Absyn_AggrInfoU _tmp81A;struct Cyc_Absyn_Aggrdecl**_tmp81B;struct Cyc_Absyn_Aggrdecl*
_tmp81C;struct Cyc_List_List*_tmp81D;enum Cyc_Absyn_AggrKind _tmp81F;struct Cyc_List_List*
_tmp820;_LL429: {struct Cyc_Absyn_VoidType_struct*_tmp815=(struct Cyc_Absyn_VoidType_struct*)
_tmp814;if(_tmp815->tag != 0)goto _LL42B;}_LL42A: return 0;_LL42B: {struct Cyc_Absyn_TupleType_struct*
_tmp816=(struct Cyc_Absyn_TupleType_struct*)_tmp814;if(_tmp816->tag != 11)goto
_LL42D;else{_tmp817=_tmp816->f1;}}_LL42C: {struct _tuple24 _tmp12FE;struct _tuple24
_tmp821=(_tmp12FE.f1=r,((_tmp12FE.f2=te,((_tmp12FE.f3=flatten,_tmp12FE)))));
struct Cyc_List_List*_tmp822=_tmp817;struct Cyc_List_List _tmp823;struct _tuple11*
_tmp824;struct Cyc_List_List*_tmp825;_LL434: if(_tmp822 != 0)goto _LL436;_LL435:
return 0;_LL436: if(_tmp822 == 0)goto _LL433;_tmp823=*_tmp822;_tmp824=(struct
_tuple11*)_tmp823.hd;_tmp825=_tmp823.tl;_LL437: {struct Cyc_List_List*_tmp826=Cyc_Tcutil_rcopy_tqt(&
_tmp821,_tmp824);_tmp821.f3=0;{struct Cyc_List_List*_tmp827=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp821,_tmp817);struct Cyc_List_List*_tmp12FF;struct Cyc_List_List*_tmp828=(
_tmp12FF=_region_malloc(r,sizeof(*_tmp12FF)),((_tmp12FF->hd=_tmp826,((_tmp12FF->tl=
_tmp827,_tmp12FF)))));struct Cyc_List_List*_tmp1300;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1300=
_region_malloc(r,sizeof(*_tmp1300)),((_tmp1300->hd=_tmp826,((_tmp1300->tl=
_tmp827,_tmp1300)))))));};}_LL433:;}_LL42D: {struct Cyc_Absyn_AggrType_struct*
_tmp818=(struct Cyc_Absyn_AggrType_struct*)_tmp814;if(_tmp818->tag != 12)goto
_LL42F;else{_tmp819=_tmp818->f1;_tmp81A=_tmp819.aggr_info;if((_tmp81A.KnownAggr).tag
!= 2)goto _LL42F;_tmp81B=(struct Cyc_Absyn_Aggrdecl**)(_tmp81A.KnownAggr).val;
_tmp81C=*_tmp81B;_tmp81D=_tmp819.targs;}}_LL42E: if(((_tmp81C->kind == Cyc_Absyn_UnionA
 || _tmp81C->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp81C->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp81C->impl))->rgn_po != 0){
struct _tuple11*_tmp1303;struct Cyc_List_List*_tmp1302;return(_tmp1302=
_region_malloc(r,sizeof(*_tmp1302)),((_tmp1302->hd=((_tmp1303=_region_malloc(r,
sizeof(*_tmp1303)),((_tmp1303->f1=Cyc_Absyn_empty_tqual(0),((_tmp1303->f2=t1,
_tmp1303)))))),((_tmp1302->tl=0,_tmp1302)))));}{struct Cyc_List_List*_tmp82E=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp81C->tvs,_tmp81D);struct _tuple23
_tmp1304;struct _tuple23 env=(_tmp1304.f1=_tmp82E,((_tmp1304.f2=r,((_tmp1304.f3=te,((
_tmp1304.f4=flatten,_tmp1304)))))));struct Cyc_List_List*_tmp82F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp81C->impl))->fields;struct Cyc_List_List _tmp830;struct Cyc_Absyn_Aggrfield*
_tmp831;struct Cyc_List_List*_tmp832;_LL439: if(_tmp82F != 0)goto _LL43B;_LL43A:
return 0;_LL43B: if(_tmp82F == 0)goto _LL438;_tmp830=*_tmp82F;_tmp831=(struct Cyc_Absyn_Aggrfield*)
_tmp830.hd;_tmp832=_tmp830.tl;_LL43C: {struct Cyc_List_List*_tmp833=Cyc_Tcutil_flatten_typ_f(&
env,_tmp831);env.f4=0;{struct Cyc_List_List*_tmp834=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp832);struct Cyc_List_List*_tmp1305;struct Cyc_List_List*_tmp835=(_tmp1305=
_region_malloc(r,sizeof(*_tmp1305)),((_tmp1305->hd=_tmp833,((_tmp1305->tl=
_tmp834,_tmp1305)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp835);};}_LL438:;};_LL42F: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp81E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp814;if(_tmp81E->tag != 13)goto _LL431;else{_tmp81F=_tmp81E->f1;if(_tmp81F != 
Cyc_Absyn_StructA)goto _LL431;_tmp820=_tmp81E->f2;}}_LL430: {struct _tuple23
_tmp1306;struct _tuple23 env=(_tmp1306.f1=0,((_tmp1306.f2=r,((_tmp1306.f3=te,((
_tmp1306.f4=flatten,_tmp1306)))))));struct Cyc_List_List*_tmp838=_tmp820;struct
Cyc_List_List _tmp839;struct Cyc_Absyn_Aggrfield*_tmp83A;struct Cyc_List_List*
_tmp83B;_LL43E: if(_tmp838 != 0)goto _LL440;_LL43F: return 0;_LL440: if(_tmp838 == 0)
goto _LL43D;_tmp839=*_tmp838;_tmp83A=(struct Cyc_Absyn_Aggrfield*)_tmp839.hd;
_tmp83B=_tmp839.tl;_LL441: {struct Cyc_List_List*_tmp83C=Cyc_Tcutil_flatten_typ_f(&
env,_tmp83A);env.f4=0;{struct Cyc_List_List*_tmp83D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp83B);struct Cyc_List_List*_tmp1307;struct Cyc_List_List*_tmp83E=(_tmp1307=
_region_malloc(r,sizeof(*_tmp1307)),((_tmp1307->hd=_tmp83C,((_tmp1307->tl=
_tmp83D,_tmp1307)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp83E);};}_LL43D:;}_LL431:;_LL432:
goto _LL428;_LL428:;};}{struct _tuple11*_tmp130A;struct Cyc_List_List*_tmp1309;
return(_tmp1309=_region_malloc(r,sizeof(*_tmp1309)),((_tmp1309->hd=((_tmp130A=
_region_malloc(r,sizeof(*_tmp130A)),((_tmp130A->f1=Cyc_Absyn_empty_tqual(0),((
_tmp130A->f2=t1,_tmp130A)))))),((_tmp1309->tl=0,_tmp1309)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp843=(void*)t->hd;_LL443: {struct Cyc_Absyn_Pure_att_struct*
_tmp844=(struct Cyc_Absyn_Pure_att_struct*)_tmp843;if(_tmp844->tag != 22)goto
_LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_Noreturn_att_struct*_tmp845=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp843;if(_tmp845->tag != 4)goto _LL447;}
_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Initializes_att_struct*_tmp846=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp843;if(_tmp846->tag != 20)goto _LL449;}
_LL448: continue;_LL449:;_LL44A: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL442:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp130B;struct _tuple0 _tmp848=(_tmp130B.f1=Cyc_Tcutil_compress(t1),((
_tmp130B.f2=Cyc_Tcutil_compress(t2),_tmp130B)));void*_tmp849;enum Cyc_Absyn_Size_of
_tmp84B;void*_tmp84C;enum Cyc_Absyn_Size_of _tmp84E;_LL44C: _tmp849=_tmp848.f1;{
struct Cyc_Absyn_IntType_struct*_tmp84A=(struct Cyc_Absyn_IntType_struct*)_tmp849;
if(_tmp84A->tag != 6)goto _LL44E;else{_tmp84B=_tmp84A->f2;}};_tmp84C=_tmp848.f2;{
struct Cyc_Absyn_IntType_struct*_tmp84D=(struct Cyc_Absyn_IntType_struct*)_tmp84C;
if(_tmp84D->tag != 6)goto _LL44E;else{_tmp84E=_tmp84D->f2;}};_LL44D: return(_tmp84B
== _tmp84E  || _tmp84B == Cyc_Absyn_Int_sz  && _tmp84E == Cyc_Absyn_Long_sz) || 
_tmp84B == Cyc_Absyn_Long_sz  && _tmp84E == Cyc_Absyn_Int_sz;_LL44E:;_LL44F: return 0;
_LL44B:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp130C;struct _tuple0 _tmp850=(
_tmp130C.f1=t1,((_tmp130C.f2=t2,_tmp130C)));void*_tmp851;struct Cyc_Absyn_PtrInfo
_tmp853;void*_tmp854;struct Cyc_Absyn_Tqual _tmp855;struct Cyc_Absyn_PtrAtts _tmp856;
void*_tmp857;union Cyc_Absyn_Constraint*_tmp858;union Cyc_Absyn_Constraint*_tmp859;
union Cyc_Absyn_Constraint*_tmp85A;void*_tmp85B;struct Cyc_Absyn_PtrInfo _tmp85D;
void*_tmp85E;struct Cyc_Absyn_Tqual _tmp85F;struct Cyc_Absyn_PtrAtts _tmp860;void*
_tmp861;union Cyc_Absyn_Constraint*_tmp862;union Cyc_Absyn_Constraint*_tmp863;
union Cyc_Absyn_Constraint*_tmp864;void*_tmp865;struct Cyc_Absyn_DatatypeFieldInfo
_tmp867;union Cyc_Absyn_DatatypeFieldInfoU _tmp868;struct _tuple3 _tmp869;struct Cyc_Absyn_Datatypedecl*
_tmp86A;struct Cyc_Absyn_Datatypefield*_tmp86B;struct Cyc_List_List*_tmp86C;void*
_tmp86D;struct Cyc_Absyn_DatatypeInfo _tmp86F;union Cyc_Absyn_DatatypeInfoU _tmp870;
struct Cyc_Absyn_Datatypedecl**_tmp871;struct Cyc_Absyn_Datatypedecl*_tmp872;
struct Cyc_List_List*_tmp873;void*_tmp874;struct Cyc_Absyn_FnInfo _tmp876;void*
_tmp877;struct Cyc_Absyn_FnInfo _tmp879;_LL451: _tmp851=_tmp850.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp852=(struct Cyc_Absyn_PointerType_struct*)_tmp851;if(_tmp852->tag != 5)goto
_LL453;else{_tmp853=_tmp852->f1;_tmp854=_tmp853.elt_typ;_tmp855=_tmp853.elt_tq;
_tmp856=_tmp853.ptr_atts;_tmp857=_tmp856.rgn;_tmp858=_tmp856.nullable;_tmp859=
_tmp856.bounds;_tmp85A=_tmp856.zero_term;}};_tmp85B=_tmp850.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp85C=(struct Cyc_Absyn_PointerType_struct*)_tmp85B;if(_tmp85C->tag != 5)goto
_LL453;else{_tmp85D=_tmp85C->f1;_tmp85E=_tmp85D.elt_typ;_tmp85F=_tmp85D.elt_tq;
_tmp860=_tmp85D.ptr_atts;_tmp861=_tmp860.rgn;_tmp862=_tmp860.nullable;_tmp863=
_tmp860.bounds;_tmp864=_tmp860.zero_term;}};_LL452: if(_tmp855.real_const  && !
_tmp85F.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp858,
_tmp862) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp858)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp862))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp85A,
_tmp864) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp85A)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp864))return 0;if(!Cyc_Tcutil_unify(_tmp857,_tmp861) && !Cyc_Tcenv_region_outlives(
te,_tmp857,_tmp861))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp859,_tmp863)){struct _tuple0 _tmp130D;struct _tuple0 _tmp87B=(_tmp130D.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp859),((_tmp130D.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp863),_tmp130D)));void*
_tmp87C;void*_tmp87E;void*_tmp880;struct Cyc_Absyn_Exp*_tmp882;void*_tmp883;
struct Cyc_Absyn_Exp*_tmp885;_LL45A: _tmp87C=_tmp87B.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp87D=(struct Cyc_Absyn_Upper_b_struct*)_tmp87C;if(_tmp87D->tag != 1)goto _LL45C;};
_tmp87E=_tmp87B.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp87F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp87E;if(_tmp87F->tag != 0)goto _LL45C;};_LL45B: goto _LL459;_LL45C: _tmp880=
_tmp87B.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp881=(struct Cyc_Absyn_Upper_b_struct*)
_tmp880;if(_tmp881->tag != 1)goto _LL45E;else{_tmp882=_tmp881->f1;}};_tmp883=
_tmp87B.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp884=(struct Cyc_Absyn_Upper_b_struct*)
_tmp883;if(_tmp884->tag != 1)goto _LL45E;else{_tmp885=_tmp884->f1;}};_LL45D: if(!
Cyc_Evexp_lte_const_exp(_tmp885,_tmp882))return 0;goto _LL459;_LL45E:;_LL45F:
return 0;_LL459:;}if(!_tmp85F.real_const  && _tmp855.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp85E)))return 0;}{int _tmp886=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp863,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp864);struct _tuple0*
_tmp1310;struct Cyc_List_List*_tmp130F;return(_tmp886  && Cyc_Tcutil_ptrsubtype(te,((
_tmp130F=_cycalloc(sizeof(*_tmp130F)),((_tmp130F->hd=((_tmp1310=_cycalloc(
sizeof(*_tmp1310)),((_tmp1310->f1=t1,((_tmp1310->f2=t2,_tmp1310)))))),((_tmp130F->tl=
assume,_tmp130F)))))),_tmp854,_tmp85E) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL453: _tmp865=_tmp850.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp866=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp865;if(_tmp866->tag != 4)
goto _LL455;else{_tmp867=_tmp866->f1;_tmp868=_tmp867.field_info;if((_tmp868.KnownDatatypefield).tag
!= 2)goto _LL455;_tmp869=(struct _tuple3)(_tmp868.KnownDatatypefield).val;_tmp86A=
_tmp869.f1;_tmp86B=_tmp869.f2;_tmp86C=_tmp867.targs;}};_tmp86D=_tmp850.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp86E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp86D;if(_tmp86E->tag != 3)goto _LL455;else{_tmp86F=_tmp86E->f1;_tmp870=_tmp86F.datatype_info;
if((_tmp870.KnownDatatype).tag != 2)goto _LL455;_tmp871=(struct Cyc_Absyn_Datatypedecl**)(
_tmp870.KnownDatatype).val;_tmp872=*_tmp871;_tmp873=_tmp86F.targs;}};_LL454: if(
_tmp86A != _tmp872  && Cyc_Absyn_qvar_cmp(_tmp86A->name,_tmp872->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp86C)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp873))return 0;for(0;_tmp86C != 0;(_tmp86C=
_tmp86C->tl,_tmp873=_tmp873->tl)){if(!Cyc_Tcutil_unify((void*)_tmp86C->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp873))->hd))return 0;}return 1;_LL455: _tmp874=
_tmp850.f1;{struct Cyc_Absyn_FnType_struct*_tmp875=(struct Cyc_Absyn_FnType_struct*)
_tmp874;if(_tmp875->tag != 10)goto _LL457;else{_tmp876=_tmp875->f1;}};_tmp877=
_tmp850.f2;{struct Cyc_Absyn_FnType_struct*_tmp878=(struct Cyc_Absyn_FnType_struct*)
_tmp877;if(_tmp878->tag != 10)goto _LL457;else{_tmp879=_tmp878->f1;}};_LL456: if(
_tmp876.tvars != 0  || _tmp879.tvars != 0){struct Cyc_List_List*_tmp889=_tmp876.tvars;
struct Cyc_List_List*_tmp88A=_tmp879.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp889)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp88A))return 0;{
struct _RegionHandle*_tmp88B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp889 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp889->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp88A))->hd)->kind))return 0;{struct _tuple15*_tmp131A;struct Cyc_Absyn_VarType_struct
_tmp1319;struct Cyc_Absyn_VarType_struct*_tmp1318;struct Cyc_List_List*_tmp1317;
inst=((_tmp1317=_region_malloc(_tmp88B,sizeof(*_tmp1317)),((_tmp1317->hd=((
_tmp131A=_region_malloc(_tmp88B,sizeof(*_tmp131A)),((_tmp131A->f1=(struct Cyc_Absyn_Tvar*)
_tmp88A->hd,((_tmp131A->f2=(void*)((_tmp1318=_cycalloc(sizeof(*_tmp1318)),((
_tmp1318[0]=((_tmp1319.tag=2,((_tmp1319.f1=(struct Cyc_Absyn_Tvar*)_tmp889->hd,
_tmp1319)))),_tmp1318)))),_tmp131A)))))),((_tmp1317->tl=inst,_tmp1317))))));}
_tmp889=_tmp889->tl;_tmp88A=_tmp88A->tl;}if(inst != 0){_tmp876.tvars=0;_tmp879.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1320;struct Cyc_Absyn_FnType_struct*_tmp131F;
struct Cyc_Absyn_FnType_struct _tmp131D;struct Cyc_Absyn_FnType_struct*_tmp131C;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp131C=_cycalloc(sizeof(*_tmp131C)),((
_tmp131C[0]=((_tmp131D.tag=10,((_tmp131D.f1=_tmp876,_tmp131D)))),_tmp131C)))),(
void*)((_tmp131F=_cycalloc(sizeof(*_tmp131F)),((_tmp131F[0]=((_tmp1320.tag=10,((
_tmp1320.f1=_tmp879,_tmp1320)))),_tmp131F)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp876.ret_typ,_tmp879.ret_typ))return 0;{struct Cyc_List_List*_tmp894=
_tmp876.args;struct Cyc_List_List*_tmp895=_tmp879.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp894)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp895))return 0;for(0;_tmp894 != 0;(_tmp894=_tmp894->tl,_tmp895=_tmp895->tl)){
struct Cyc_Absyn_Tqual _tmp897;void*_tmp898;struct _tuple9 _tmp896=*((struct _tuple9*)
_tmp894->hd);_tmp897=_tmp896.f2;_tmp898=_tmp896.f3;{struct Cyc_Absyn_Tqual _tmp89A;
void*_tmp89B;struct _tuple9 _tmp899=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp895))->hd);_tmp89A=_tmp899.f2;_tmp89B=_tmp899.f3;if(_tmp89A.real_const
 && !_tmp897.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp89B,_tmp898))return
0;};}if(_tmp876.c_varargs != _tmp879.c_varargs)return 0;if(_tmp876.cyc_varargs != 0
 && _tmp879.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp89C=*_tmp876.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp89D=*_tmp879.cyc_varargs;if((_tmp89D.tq).real_const
 && !(_tmp89C.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp89D.type,
_tmp89C.type))return 0;}else{if(_tmp876.cyc_varargs != 0  || _tmp879.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp876.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp879.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp876.rgn_po,_tmp879.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp876.attributes,_tmp879.attributes))return 0;return 1;};_LL457:;_LL458: return 0;
_LL450:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp89E=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp89E,1,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp89E,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp8A0;struct Cyc_Absyn_Tqual
_tmp8A1;void*_tmp8A2;struct _tuple11*_tmp89F=(struct _tuple11*)tqs1->hd;_tmp8A0=*
_tmp89F;_tmp8A1=_tmp8A0.f1;_tmp8A2=_tmp8A0.f2;{struct _tuple11 _tmp8A4;struct Cyc_Absyn_Tqual
_tmp8A5;void*_tmp8A6;struct _tuple11*_tmp8A3=(struct _tuple11*)tqs2->hd;_tmp8A4=*
_tmp8A3;_tmp8A5=_tmp8A4.f1;_tmp8A6=_tmp8A4.f2;if(_tmp8A1.real_const  && !_tmp8A5.real_const)
return 0;if((_tmp8A5.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8A6))) && Cyc_Tcutil_subtype(te,assume,_tmp8A2,_tmp8A6))continue;if(Cyc_Tcutil_unify(
_tmp8A2,_tmp8A6))continue;if(Cyc_Tcutil_isomorphic(_tmp8A2,_tmp8A6))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8A7=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8A9;_LL461: {struct Cyc_Absyn_IntType_struct*_tmp8A8=(
struct Cyc_Absyn_IntType_struct*)_tmp8A7;if(_tmp8A8->tag != 6)goto _LL463;else{
_tmp8A9=_tmp8A8->f2;if(_tmp8A9 != Cyc_Absyn_Char_sz)goto _LL463;}}_LL462: return 1;
_LL463:;_LL464: return 0;_LL460:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8AA=t2;enum Cyc_Absyn_Size_of _tmp8AC;enum Cyc_Absyn_Size_of _tmp8AE;_LL466: {
struct Cyc_Absyn_IntType_struct*_tmp8AB=(struct Cyc_Absyn_IntType_struct*)_tmp8AA;
if(_tmp8AB->tag != 6)goto _LL468;else{_tmp8AC=_tmp8AB->f2;if(_tmp8AC != Cyc_Absyn_Int_sz)
goto _LL468;}}_LL467: goto _LL469;_LL468: {struct Cyc_Absyn_IntType_struct*_tmp8AD=(
struct Cyc_Absyn_IntType_struct*)_tmp8AA;if(_tmp8AD->tag != 6)goto _LL46A;else{
_tmp8AE=_tmp8AD->f2;if(_tmp8AE != Cyc_Absyn_Long_sz)goto _LL46A;}}_LL469: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL465;_LL46A:;
_LL46B: goto _LL465;_LL465:;}{void*_tmp8AF=t1;struct Cyc_Absyn_PtrInfo _tmp8B1;void*
_tmp8B2;struct Cyc_Absyn_Tqual _tmp8B3;struct Cyc_Absyn_PtrAtts _tmp8B4;void*_tmp8B5;
union Cyc_Absyn_Constraint*_tmp8B6;union Cyc_Absyn_Constraint*_tmp8B7;union Cyc_Absyn_Constraint*
_tmp8B8;struct Cyc_Absyn_ArrayInfo _tmp8BA;void*_tmp8BB;struct Cyc_Absyn_Tqual
_tmp8BC;struct Cyc_Absyn_Exp*_tmp8BD;union Cyc_Absyn_Constraint*_tmp8BE;struct Cyc_Absyn_Enumdecl*
_tmp8C0;void*_tmp8C5;_LL46D: {struct Cyc_Absyn_PointerType_struct*_tmp8B0=(struct
Cyc_Absyn_PointerType_struct*)_tmp8AF;if(_tmp8B0->tag != 5)goto _LL46F;else{
_tmp8B1=_tmp8B0->f1;_tmp8B2=_tmp8B1.elt_typ;_tmp8B3=_tmp8B1.elt_tq;_tmp8B4=
_tmp8B1.ptr_atts;_tmp8B5=_tmp8B4.rgn;_tmp8B6=_tmp8B4.nullable;_tmp8B7=_tmp8B4.bounds;
_tmp8B8=_tmp8B4.zero_term;}}_LL46E:{void*_tmp8C6=t2;struct Cyc_Absyn_PtrInfo
_tmp8C8;void*_tmp8C9;struct Cyc_Absyn_Tqual _tmp8CA;struct Cyc_Absyn_PtrAtts _tmp8CB;
void*_tmp8CC;union Cyc_Absyn_Constraint*_tmp8CD;union Cyc_Absyn_Constraint*_tmp8CE;
union Cyc_Absyn_Constraint*_tmp8CF;_LL47E: {struct Cyc_Absyn_PointerType_struct*
_tmp8C7=(struct Cyc_Absyn_PointerType_struct*)_tmp8C6;if(_tmp8C7->tag != 5)goto
_LL480;else{_tmp8C8=_tmp8C7->f1;_tmp8C9=_tmp8C8.elt_typ;_tmp8CA=_tmp8C8.elt_tq;
_tmp8CB=_tmp8C8.ptr_atts;_tmp8CC=_tmp8CB.rgn;_tmp8CD=_tmp8CB.nullable;_tmp8CE=
_tmp8CB.bounds;_tmp8CF=_tmp8CB.zero_term;}}_LL47F: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1323;struct Cyc_List_List*
_tmp1322;struct Cyc_List_List*_tmp8D0=(_tmp1322=_cycalloc(sizeof(*_tmp1322)),((
_tmp1322->hd=((_tmp1323=_cycalloc(sizeof(*_tmp1323)),((_tmp1323->f1=t1,((
_tmp1323->f2=t2,_tmp1323)))))),((_tmp1322->tl=0,_tmp1322)))));int _tmp8D1=_tmp8CA.real_const
 || !_tmp8B3.real_const;int _tmp8D2=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8CE,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8CF);int _tmp8D3=_tmp8D1  && (((_tmp8D2  && Cyc_Tcutil_ptrsubtype(
te,_tmp8D0,_tmp8B2,_tmp8C9) || Cyc_Tcutil_unify(_tmp8B2,_tmp8C9)) || Cyc_Tcutil_isomorphic(
_tmp8B2,_tmp8C9)) || Cyc_Tcutil_unify(_tmp8C9,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8B8,_tmp8CF) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8CF);int _tmp8D4=_tmp8D3?0:((Cyc_Tcutil_bits_only(_tmp8B2) && Cyc_Tcutil_is_char_type(
_tmp8C9)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8CF)) && (_tmp8CA.real_const  || !_tmp8B3.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8B7,_tmp8CE);if(!
bounds_ok  && !_tmp8D4){struct _tuple0 _tmp1324;struct _tuple0 _tmp8D6=(_tmp1324.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8B7),((_tmp1324.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8CE),_tmp1324)));
void*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D9;void*_tmp8DA;struct Cyc_Absyn_Exp*
_tmp8DC;_LL483: _tmp8D7=_tmp8D6.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8D8=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8D7;if(_tmp8D8->tag != 1)goto _LL485;else{
_tmp8D9=_tmp8D8->f1;}};_tmp8DA=_tmp8D6.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8DB=(struct Cyc_Absyn_Upper_b_struct*)_tmp8DA;if(_tmp8DB->tag != 1)goto _LL485;
else{_tmp8DC=_tmp8DB->f1;}};_LL484: if(Cyc_Evexp_lte_const_exp(_tmp8DC,_tmp8D9))
bounds_ok=1;goto _LL482;_LL485:;_LL486: bounds_ok=1;goto _LL482;_LL482:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8B6) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8CD))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8D3  || _tmp8D4)) && (Cyc_Tcutil_unify(
_tmp8B5,_tmp8CC) || Cyc_Tcenv_region_outlives(te,_tmp8B5,_tmp8CC)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL480:;_LL481: goto _LL47D;
_LL47D:;}return Cyc_Absyn_Unknown_coercion;_LL46F: {struct Cyc_Absyn_ArrayType_struct*
_tmp8B9=(struct Cyc_Absyn_ArrayType_struct*)_tmp8AF;if(_tmp8B9->tag != 9)goto
_LL471;else{_tmp8BA=_tmp8B9->f1;_tmp8BB=_tmp8BA.elt_type;_tmp8BC=_tmp8BA.tq;
_tmp8BD=_tmp8BA.num_elts;_tmp8BE=_tmp8BA.zero_term;}}_LL470:{void*_tmp8DF=t2;
struct Cyc_Absyn_ArrayInfo _tmp8E1;void*_tmp8E2;struct Cyc_Absyn_Tqual _tmp8E3;
struct Cyc_Absyn_Exp*_tmp8E4;union Cyc_Absyn_Constraint*_tmp8E5;_LL488: {struct Cyc_Absyn_ArrayType_struct*
_tmp8E0=(struct Cyc_Absyn_ArrayType_struct*)_tmp8DF;if(_tmp8E0->tag != 9)goto
_LL48A;else{_tmp8E1=_tmp8E0->f1;_tmp8E2=_tmp8E1.elt_type;_tmp8E3=_tmp8E1.tq;
_tmp8E4=_tmp8E1.num_elts;_tmp8E5=_tmp8E1.zero_term;}}_LL489: {int okay;okay=((
_tmp8BD != 0  && _tmp8E4 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8BE,
_tmp8E5)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8E4,(struct Cyc_Absyn_Exp*)
_tmp8BD);return(okay  && Cyc_Tcutil_unify(_tmp8BB,_tmp8E2)) && (!_tmp8BC.real_const
 || _tmp8E3.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL48A:;_LL48B: return Cyc_Absyn_Unknown_coercion;_LL487:;}return Cyc_Absyn_Unknown_coercion;
_LL471: {struct Cyc_Absyn_EnumType_struct*_tmp8BF=(struct Cyc_Absyn_EnumType_struct*)
_tmp8AF;if(_tmp8BF->tag != 14)goto _LL473;else{_tmp8C0=_tmp8BF->f2;}}_LL472:{void*
_tmp8E6=t2;struct Cyc_Absyn_Enumdecl*_tmp8E8;_LL48D: {struct Cyc_Absyn_EnumType_struct*
_tmp8E7=(struct Cyc_Absyn_EnumType_struct*)_tmp8E6;if(_tmp8E7->tag != 14)goto
_LL48F;else{_tmp8E8=_tmp8E7->f2;}}_LL48E: if((_tmp8C0->fields != 0  && _tmp8E8->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8C0->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp8E8->fields))->v))return Cyc_Absyn_No_coercion;goto _LL48C;_LL48F:;_LL490: goto
_LL48C;_LL48C:;}goto _LL474;_LL473: {struct Cyc_Absyn_IntType_struct*_tmp8C1=(
struct Cyc_Absyn_IntType_struct*)_tmp8AF;if(_tmp8C1->tag != 6)goto _LL475;}_LL474:
goto _LL476;_LL475: {struct Cyc_Absyn_FloatType_struct*_tmp8C2=(struct Cyc_Absyn_FloatType_struct*)
_tmp8AF;if(_tmp8C2->tag != 7)goto _LL477;}_LL476: goto _LL478;_LL477: {struct Cyc_Absyn_DoubleType_struct*
_tmp8C3=(struct Cyc_Absyn_DoubleType_struct*)_tmp8AF;if(_tmp8C3->tag != 8)goto
_LL479;}_LL478: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL479: {struct Cyc_Absyn_RgnHandleType_struct*_tmp8C4=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8AF;if(_tmp8C4->tag != 16)goto _LL47B;else{_tmp8C5=(void*)_tmp8C4->f1;}}_LL47A:{
void*_tmp8E9=t2;void*_tmp8EB;_LL492: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8EA=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp8E9;if(_tmp8EA->tag != 16)goto
_LL494;else{_tmp8EB=(void*)_tmp8EA->f1;}}_LL493: if(Cyc_Tcenv_region_outlives(te,
_tmp8C5,_tmp8EB))return Cyc_Absyn_No_coercion;goto _LL491;_LL494:;_LL495: goto
_LL491;_LL491:;}return Cyc_Absyn_Unknown_coercion;_LL47B:;_LL47C: return Cyc_Absyn_Unknown_coercion;
_LL46C:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp8EC=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp1327;struct Cyc_Absyn_Cast_e_struct*_tmp1326;e->r=(
void*)((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((_tmp1326[0]=((_tmp1327.tag=15,((
_tmp1327.f1=(void*)t,((_tmp1327.f2=_tmp8EC,((_tmp1327.f3=0,((_tmp1327.f4=c,
_tmp1327)))))))))),_tmp1326))));}{struct Cyc_Core_Opt*_tmp1328;e->topt=((_tmp1328=
_cycalloc(sizeof(*_tmp1328)),((_tmp1328->v=t,_tmp1328))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp8F0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL497: {struct Cyc_Absyn_IntType_struct*_tmp8F1=(
struct Cyc_Absyn_IntType_struct*)_tmp8F0;if(_tmp8F1->tag != 6)goto _LL499;}_LL498:
goto _LL49A;_LL499: {struct Cyc_Absyn_EnumType_struct*_tmp8F2=(struct Cyc_Absyn_EnumType_struct*)
_tmp8F0;if(_tmp8F2->tag != 14)goto _LL49B;}_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp8F3=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp8F0;if(_tmp8F3->tag != 15)goto
_LL49D;}_LL49C: goto _LL49E;_LL49D: {struct Cyc_Absyn_TagType_struct*_tmp8F4=(
struct Cyc_Absyn_TagType_struct*)_tmp8F0;if(_tmp8F4->tag != 20)goto _LL49F;}_LL49E:
return 1;_LL49F: {struct Cyc_Absyn_Evar_struct*_tmp8F5=(struct Cyc_Absyn_Evar_struct*)
_tmp8F0;if(_tmp8F5->tag != 1)goto _LL4A1;}_LL4A0: return Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL4A1:;_LL4A2:
return 0;_LL496:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp8F6=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL4A4: {struct Cyc_Absyn_FloatType_struct*_tmp8F7=(
struct Cyc_Absyn_FloatType_struct*)_tmp8F6;if(_tmp8F7->tag != 7)goto _LL4A6;}_LL4A5:
goto _LL4A7;_LL4A6: {struct Cyc_Absyn_DoubleType_struct*_tmp8F8=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8F6;if(_tmp8F8->tag != 8)goto _LL4A8;}_LL4A7: return 1;_LL4A8:;_LL4A9: return 0;
_LL4A3:;};}int Cyc_Tcutil_is_function_type(void*t){void*_tmp8F9=Cyc_Tcutil_compress(
t);_LL4AB: {struct Cyc_Absyn_FnType_struct*_tmp8FA=(struct Cyc_Absyn_FnType_struct*)
_tmp8F9;if(_tmp8FA->tag != 10)goto _LL4AD;}_LL4AC: return 1;_LL4AD:;_LL4AE: return 0;
_LL4AA:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp1329;struct _tuple0 _tmp8FC=(_tmp1329.f1=t1,((_tmp1329.f2=t2,_tmp1329)));void*
_tmp8FD;int _tmp8FF;void*_tmp900;int _tmp902;void*_tmp903;void*_tmp905;void*
_tmp907;void*_tmp909;void*_tmp90B;enum Cyc_Absyn_Sign _tmp90D;enum Cyc_Absyn_Size_of
_tmp90E;void*_tmp90F;enum Cyc_Absyn_Sign _tmp911;enum Cyc_Absyn_Size_of _tmp912;
void*_tmp913;enum Cyc_Absyn_Size_of _tmp915;void*_tmp916;enum Cyc_Absyn_Size_of
_tmp918;void*_tmp919;enum Cyc_Absyn_Sign _tmp91B;enum Cyc_Absyn_Size_of _tmp91C;
void*_tmp91D;enum Cyc_Absyn_Sign _tmp91F;enum Cyc_Absyn_Size_of _tmp920;void*
_tmp921;enum Cyc_Absyn_Sign _tmp923;enum Cyc_Absyn_Size_of _tmp924;void*_tmp925;
enum Cyc_Absyn_Sign _tmp927;enum Cyc_Absyn_Size_of _tmp928;void*_tmp929;enum Cyc_Absyn_Size_of
_tmp92B;void*_tmp92C;enum Cyc_Absyn_Size_of _tmp92E;_LL4B0: _tmp8FD=_tmp8FC.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp8FE=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8FD;if(_tmp8FE->tag != 8)goto _LL4B2;else{_tmp8FF=_tmp8FE->f1;}};_tmp900=
_tmp8FC.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp901=(struct Cyc_Absyn_DoubleType_struct*)
_tmp900;if(_tmp901->tag != 8)goto _LL4B2;else{_tmp902=_tmp901->f1;}};_LL4B1: if(
_tmp8FF)return t1;else{return t2;}_LL4B2: _tmp903=_tmp8FC.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp904=(struct Cyc_Absyn_DoubleType_struct*)_tmp903;if(_tmp904->tag != 8)goto
_LL4B4;};_LL4B3: return t1;_LL4B4: _tmp905=_tmp8FC.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp906=(struct Cyc_Absyn_DoubleType_struct*)_tmp905;if(_tmp906->tag != 8)goto
_LL4B6;};_LL4B5: return t2;_LL4B6: _tmp907=_tmp8FC.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp908=(struct Cyc_Absyn_FloatType_struct*)_tmp907;if(_tmp908->tag != 7)goto
_LL4B8;};_LL4B7: goto _LL4B9;_LL4B8: _tmp909=_tmp8FC.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp90A=(struct Cyc_Absyn_FloatType_struct*)_tmp909;if(_tmp90A->tag != 7)goto
_LL4BA;};_LL4B9: return(void*)& Cyc_Absyn_FloatType_val;_LL4BA: _tmp90B=_tmp8FC.f1;{
struct Cyc_Absyn_IntType_struct*_tmp90C=(struct Cyc_Absyn_IntType_struct*)_tmp90B;
if(_tmp90C->tag != 6)goto _LL4BC;else{_tmp90D=_tmp90C->f1;if(_tmp90D != Cyc_Absyn_Unsigned)
goto _LL4BC;_tmp90E=_tmp90C->f2;if(_tmp90E != Cyc_Absyn_LongLong_sz)goto _LL4BC;}};
_LL4BB: goto _LL4BD;_LL4BC: _tmp90F=_tmp8FC.f2;{struct Cyc_Absyn_IntType_struct*
_tmp910=(struct Cyc_Absyn_IntType_struct*)_tmp90F;if(_tmp910->tag != 6)goto _LL4BE;
else{_tmp911=_tmp910->f1;if(_tmp911 != Cyc_Absyn_Unsigned)goto _LL4BE;_tmp912=
_tmp910->f2;if(_tmp912 != Cyc_Absyn_LongLong_sz)goto _LL4BE;}};_LL4BD: return Cyc_Absyn_ulonglong_typ;
_LL4BE: _tmp913=_tmp8FC.f1;{struct Cyc_Absyn_IntType_struct*_tmp914=(struct Cyc_Absyn_IntType_struct*)
_tmp913;if(_tmp914->tag != 6)goto _LL4C0;else{_tmp915=_tmp914->f2;if(_tmp915 != Cyc_Absyn_LongLong_sz)
goto _LL4C0;}};_LL4BF: goto _LL4C1;_LL4C0: _tmp916=_tmp8FC.f2;{struct Cyc_Absyn_IntType_struct*
_tmp917=(struct Cyc_Absyn_IntType_struct*)_tmp916;if(_tmp917->tag != 6)goto _LL4C2;
else{_tmp918=_tmp917->f2;if(_tmp918 != Cyc_Absyn_LongLong_sz)goto _LL4C2;}};_LL4C1:
return Cyc_Absyn_slonglong_typ;_LL4C2: _tmp919=_tmp8FC.f1;{struct Cyc_Absyn_IntType_struct*
_tmp91A=(struct Cyc_Absyn_IntType_struct*)_tmp919;if(_tmp91A->tag != 6)goto _LL4C4;
else{_tmp91B=_tmp91A->f1;if(_tmp91B != Cyc_Absyn_Unsigned)goto _LL4C4;_tmp91C=
_tmp91A->f2;if(_tmp91C != Cyc_Absyn_Long_sz)goto _LL4C4;}};_LL4C3: goto _LL4C5;
_LL4C4: _tmp91D=_tmp8FC.f2;{struct Cyc_Absyn_IntType_struct*_tmp91E=(struct Cyc_Absyn_IntType_struct*)
_tmp91D;if(_tmp91E->tag != 6)goto _LL4C6;else{_tmp91F=_tmp91E->f1;if(_tmp91F != Cyc_Absyn_Unsigned)
goto _LL4C6;_tmp920=_tmp91E->f2;if(_tmp920 != Cyc_Absyn_Long_sz)goto _LL4C6;}};
_LL4C5: return Cyc_Absyn_ulong_typ;_LL4C6: _tmp921=_tmp8FC.f1;{struct Cyc_Absyn_IntType_struct*
_tmp922=(struct Cyc_Absyn_IntType_struct*)_tmp921;if(_tmp922->tag != 6)goto _LL4C8;
else{_tmp923=_tmp922->f1;if(_tmp923 != Cyc_Absyn_Unsigned)goto _LL4C8;_tmp924=
_tmp922->f2;if(_tmp924 != Cyc_Absyn_Int_sz)goto _LL4C8;}};_LL4C7: goto _LL4C9;_LL4C8:
_tmp925=_tmp8FC.f2;{struct Cyc_Absyn_IntType_struct*_tmp926=(struct Cyc_Absyn_IntType_struct*)
_tmp925;if(_tmp926->tag != 6)goto _LL4CA;else{_tmp927=_tmp926->f1;if(_tmp927 != Cyc_Absyn_Unsigned)
goto _LL4CA;_tmp928=_tmp926->f2;if(_tmp928 != Cyc_Absyn_Int_sz)goto _LL4CA;}};
_LL4C9: return Cyc_Absyn_uint_typ;_LL4CA: _tmp929=_tmp8FC.f1;{struct Cyc_Absyn_IntType_struct*
_tmp92A=(struct Cyc_Absyn_IntType_struct*)_tmp929;if(_tmp92A->tag != 6)goto _LL4CC;
else{_tmp92B=_tmp92A->f2;if(_tmp92B != Cyc_Absyn_Long_sz)goto _LL4CC;}};_LL4CB:
goto _LL4CD;_LL4CC: _tmp92C=_tmp8FC.f2;{struct Cyc_Absyn_IntType_struct*_tmp92D=(
struct Cyc_Absyn_IntType_struct*)_tmp92C;if(_tmp92D->tag != 6)goto _LL4CE;else{
_tmp92E=_tmp92D->f2;if(_tmp92E != Cyc_Absyn_Long_sz)goto _LL4CE;}};_LL4CD: return
Cyc_Absyn_slong_typ;_LL4CE:;_LL4CF: return Cyc_Absyn_sint_typ;_LL4AF:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp92F=e->r;struct Cyc_Core_Opt*_tmp931;_LL4D1: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp930=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp92F;if(_tmp930->tag != 4)goto _LL4D3;else{_tmp931=_tmp930->f2;if(_tmp931 != 0)
goto _LL4D3;}}_LL4D2:{const char*_tmp132C;void*_tmp132B;(_tmp132B=0,Cyc_Tcutil_warn(
e->loc,((_tmp132C="assignment in test",_tag_dyneither(_tmp132C,sizeof(char),19))),
_tag_dyneither(_tmp132B,sizeof(void*),0)));}goto _LL4D0;_LL4D3:;_LL4D4: goto _LL4D0;
_LL4D0:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp132D;struct _tuple0 _tmp935=(
_tmp132D.f1=c1,((_tmp132D.f2=c2,_tmp132D)));void*_tmp936;struct Cyc_Absyn_Kind*
_tmp938;void*_tmp939;struct Cyc_Absyn_Kind*_tmp93B;void*_tmp93C;struct Cyc_Core_Opt*
_tmp93E;struct Cyc_Core_Opt**_tmp93F;void*_tmp940;struct Cyc_Core_Opt*_tmp942;
struct Cyc_Core_Opt**_tmp943;void*_tmp944;struct Cyc_Core_Opt*_tmp946;struct Cyc_Core_Opt**
_tmp947;struct Cyc_Absyn_Kind*_tmp948;void*_tmp949;struct Cyc_Absyn_Kind*_tmp94B;
void*_tmp94C;struct Cyc_Absyn_Kind*_tmp94E;void*_tmp94F;struct Cyc_Core_Opt*
_tmp951;struct Cyc_Core_Opt**_tmp952;struct Cyc_Absyn_Kind*_tmp953;void*_tmp954;
struct Cyc_Core_Opt*_tmp956;struct Cyc_Core_Opt**_tmp957;struct Cyc_Absyn_Kind*
_tmp958;void*_tmp959;struct Cyc_Core_Opt*_tmp95B;struct Cyc_Core_Opt**_tmp95C;
struct Cyc_Absyn_Kind*_tmp95D;_LL4D6: _tmp936=_tmp935.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp937=(struct Cyc_Absyn_Eq_kb_struct*)_tmp936;if(_tmp937->tag != 0)goto _LL4D8;
else{_tmp938=_tmp937->f1;}};_tmp939=_tmp935.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp93A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp939;if(_tmp93A->tag != 0)goto _LL4D8;
else{_tmp93B=_tmp93A->f1;}};_LL4D7: return _tmp938 == _tmp93B;_LL4D8: _tmp93C=
_tmp935.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp93D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp93C;if(_tmp93D->tag != 1)goto _LL4DA;else{_tmp93E=_tmp93D->f1;_tmp93F=(struct
Cyc_Core_Opt**)& _tmp93D->f1;}};_LL4D9:{struct Cyc_Core_Opt*_tmp132E;*_tmp93F=((
_tmp132E=_cycalloc(sizeof(*_tmp132E)),((_tmp132E->v=c1,_tmp132E))));}return 1;
_LL4DA: _tmp940=_tmp935.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp941=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp940;if(_tmp941->tag != 1)goto _LL4DC;else{_tmp942=_tmp941->f1;_tmp943=(struct
Cyc_Core_Opt**)& _tmp941->f1;}};_LL4DB:{struct Cyc_Core_Opt*_tmp132F;*_tmp943=((
_tmp132F=_cycalloc(sizeof(*_tmp132F)),((_tmp132F->v=c2,_tmp132F))));}return 1;
_LL4DC: _tmp944=_tmp935.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp945=(struct Cyc_Absyn_Less_kb_struct*)
_tmp944;if(_tmp945->tag != 2)goto _LL4DE;else{_tmp946=_tmp945->f1;_tmp947=(struct
Cyc_Core_Opt**)& _tmp945->f1;_tmp948=_tmp945->f2;}};_tmp949=_tmp935.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp94A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp949;if(_tmp94A->tag != 0)goto _LL4DE;
else{_tmp94B=_tmp94A->f1;}};_LL4DD: if(Cyc_Tcutil_kind_leq(_tmp94B,_tmp948)){{
struct Cyc_Core_Opt*_tmp1330;*_tmp947=((_tmp1330=_cycalloc(sizeof(*_tmp1330)),((
_tmp1330->v=c2,_tmp1330))));}return 1;}else{return 0;}_LL4DE: _tmp94C=_tmp935.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp94D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp94C;if(
_tmp94D->tag != 0)goto _LL4E0;else{_tmp94E=_tmp94D->f1;}};_tmp94F=_tmp935.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp950=(struct Cyc_Absyn_Less_kb_struct*)_tmp94F;
if(_tmp950->tag != 2)goto _LL4E0;else{_tmp951=_tmp950->f1;_tmp952=(struct Cyc_Core_Opt**)&
_tmp950->f1;_tmp953=_tmp950->f2;}};_LL4DF: if(Cyc_Tcutil_kind_leq(_tmp94E,_tmp953)){{
struct Cyc_Core_Opt*_tmp1331;*_tmp952=((_tmp1331=_cycalloc(sizeof(*_tmp1331)),((
_tmp1331->v=c1,_tmp1331))));}return 1;}else{return 0;}_LL4E0: _tmp954=_tmp935.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp955=(struct Cyc_Absyn_Less_kb_struct*)_tmp954;
if(_tmp955->tag != 2)goto _LL4D5;else{_tmp956=_tmp955->f1;_tmp957=(struct Cyc_Core_Opt**)&
_tmp955->f1;_tmp958=_tmp955->f2;}};_tmp959=_tmp935.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp95A=(struct Cyc_Absyn_Less_kb_struct*)_tmp959;if(_tmp95A->tag != 2)goto _LL4D5;
else{_tmp95B=_tmp95A->f1;_tmp95C=(struct Cyc_Core_Opt**)& _tmp95A->f1;_tmp95D=
_tmp95A->f2;}};_LL4E1: if(Cyc_Tcutil_kind_leq(_tmp958,_tmp95D)){{struct Cyc_Core_Opt*
_tmp1332;*_tmp95C=((_tmp1332=_cycalloc(sizeof(*_tmp1332)),((_tmp1332->v=c1,
_tmp1332))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp95D,_tmp958)){{struct Cyc_Core_Opt*
_tmp1333;*_tmp957=((_tmp1333=_cycalloc(sizeof(*_tmp1333)),((_tmp1333->v=c2,
_tmp1333))));}return 1;}else{return 0;}}_LL4D5:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp1337;void*_tmp1336[1];struct Cyc_Int_pa_struct _tmp1335;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1335.tag=1,((_tmp1335.f1=(
unsigned long)i,((_tmp1336[0]=& _tmp1335,Cyc_aprintf(((_tmp1337="#%d",
_tag_dyneither(_tmp1337,sizeof(char),4))),_tag_dyneither(_tmp1336,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp133A;struct Cyc_Absyn_Tvar*_tmp1339;return(
_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339->name=((_tmp133A=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp133A[0]=s,_tmp133A)))),((_tmp1339->identity=
- 1,((_tmp1339->kind=k,_tmp1339)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp969=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp969,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp133E;void*_tmp133D[1];struct Cyc_String_pa_struct
_tmp133C;(_tmp133C.tag=0,((_tmp133C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp133D[0]=& _tmp133C,Cyc_printf(((_tmp133E="%s",
_tag_dyneither(_tmp133E,sizeof(char),3))),_tag_dyneither(_tmp133D,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp133F;struct
_dyneither_ptr _tmp96D=Cyc_strconcat(((_tmp133F="`",_tag_dyneither(_tmp133F,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1342;char _tmp1341;
struct _dyneither_ptr _tmp1340;(_tmp1340=_dyneither_ptr_plus(_tmp96D,sizeof(char),
1),((_tmp1341=*((char*)_check_dyneither_subscript(_tmp1340,sizeof(char),0)),((
_tmp1342='t',((_get_dyneither_size(_tmp1340,sizeof(char))== 1  && (_tmp1341 == '\000'
 && _tmp1342 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1340.curr)=_tmp1342)))))));}{
struct _dyneither_ptr*_tmp1343;t->name=((_tmp1343=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1343[0]=(struct _dyneither_ptr)_tmp96D,_tmp1343))));};};}
struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple25*x){struct Cyc_Core_Opt*
_tmp1346;struct _tuple9*_tmp1345;return(_tmp1345=_cycalloc(sizeof(*_tmp1345)),((
_tmp1345->f1=(struct Cyc_Core_Opt*)((_tmp1346=_cycalloc(sizeof(*_tmp1346)),((
_tmp1346->v=(*x).f1,_tmp1346)))),((_tmp1345->f2=(*x).f2,((_tmp1345->f3=(*x).f3,
_tmp1345)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp975=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp1347;_tmp975=((_tmp1347=_cycalloc(sizeof(*_tmp1347)),((
_tmp1347->hd=(void*)atts->hd,((_tmp1347->tl=_tmp975,_tmp1347))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp134D;struct Cyc_Absyn_FnInfo _tmp134C;struct Cyc_Absyn_FnType_struct*_tmp134B;
return(void*)((_tmp134B=_cycalloc(sizeof(*_tmp134B)),((_tmp134B[0]=((_tmp134D.tag=
10,((_tmp134D.f1=((_tmp134C.tvars=fd->tvs,((_tmp134C.effect=fd->effect,((
_tmp134C.ret_typ=fd->ret_type,((_tmp134C.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp134C.c_varargs=fd->c_varargs,((_tmp134C.cyc_varargs=fd->cyc_varargs,((
_tmp134C.rgn_po=fd->rgn_po,((_tmp134C.attributes=_tmp975,_tmp134C)))))))))))))))),
_tmp134D)))),_tmp134B))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple26{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple26*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){
return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp134E;return(_tmp134E=_cycalloc(sizeof(*_tmp134E)),((_tmp134E->f1=(*
pr).f1,((_tmp134E->f2=t,_tmp134E)))));}struct _tuple27{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple28{struct _tuple27*f1;void*f2;};static
struct _tuple28*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
struct _tuple27*_tmp1351;struct _tuple28*_tmp1350;return(_tmp1350=_region_malloc(
rgn,sizeof(*_tmp1350)),((_tmp1350->f1=((_tmp1351=_region_malloc(rgn,sizeof(*
_tmp1351)),((_tmp1351->f1=(*y).f1,((_tmp1351->f2=(*y).f2,_tmp1351)))))),((
_tmp1350->f2=(*y).f3,_tmp1350)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple28*w){struct _tuple27*_tmp97E;void*_tmp97F;struct _tuple28 _tmp97D=*w;
_tmp97E=_tmp97D.f1;_tmp97F=_tmp97D.f2;{struct Cyc_Core_Opt*_tmp981;struct Cyc_Absyn_Tqual
_tmp982;struct _tuple27 _tmp980=*_tmp97E;_tmp981=_tmp980.f1;_tmp982=_tmp980.f2;{
struct _tuple9*_tmp1352;return(_tmp1352=_cycalloc(sizeof(*_tmp1352)),((_tmp1352->f1=
_tmp981,((_tmp1352->f2=_tmp982,((_tmp1352->f3=_tmp97F,_tmp1352)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){return f->type;}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*
f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1353;return(_tmp1353=_cycalloc(sizeof(*
_tmp1353)),((_tmp1353->name=f->name,((_tmp1353->tq=f->tq,((_tmp1353->type=t,((
_tmp1353->width=f->width,((_tmp1353->attributes=f->attributes,_tmp1353)))))))))));}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp1354;return(_tmp1354=_cycalloc(
sizeof(*_tmp1354)),((_tmp1354->topt=old->topt,((_tmp1354->r=r,((_tmp1354->loc=
old->loc,((_tmp1354->annot=old->annot,_tmp1354)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp986=e->r;enum Cyc_Absyn_Primop _tmp98C;struct Cyc_List_List*_tmp98D;
struct Cyc_Absyn_Exp*_tmp98F;struct Cyc_Absyn_Exp*_tmp990;struct Cyc_Absyn_Exp*
_tmp991;struct Cyc_Absyn_Exp*_tmp993;struct Cyc_Absyn_Exp*_tmp994;struct Cyc_Absyn_Exp*
_tmp996;struct Cyc_Absyn_Exp*_tmp997;struct Cyc_Absyn_Exp*_tmp999;struct Cyc_Absyn_Exp*
_tmp99A;void*_tmp99C;struct Cyc_Absyn_Exp*_tmp99D;int _tmp99E;enum Cyc_Absyn_Coercion
_tmp99F;void*_tmp9A1;struct Cyc_Absyn_Exp*_tmp9A3;void*_tmp9A5;void*_tmp9A6;void*
_tmp9A8;_LL4E3: {struct Cyc_Absyn_Const_e_struct*_tmp987=(struct Cyc_Absyn_Const_e_struct*)
_tmp986;if(_tmp987->tag != 0)goto _LL4E5;}_LL4E4: goto _LL4E6;_LL4E5: {struct Cyc_Absyn_Enum_e_struct*
_tmp988=(struct Cyc_Absyn_Enum_e_struct*)_tmp986;if(_tmp988->tag != 32)goto _LL4E7;}
_LL4E6: goto _LL4E8;_LL4E7: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp989=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp986;if(_tmp989->tag != 33)goto _LL4E9;}_LL4E8: goto _LL4EA;_LL4E9: {struct Cyc_Absyn_Var_e_struct*
_tmp98A=(struct Cyc_Absyn_Var_e_struct*)_tmp986;if(_tmp98A->tag != 1)goto _LL4EB;}
_LL4EA: return e;_LL4EB: {struct Cyc_Absyn_Primop_e_struct*_tmp98B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp986;if(_tmp98B->tag != 3)goto _LL4ED;else{_tmp98C=_tmp98B->f1;_tmp98D=_tmp98B->f2;}}
_LL4EC: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp98D)== 1){struct
Cyc_Absyn_Exp*_tmp9A9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp98D))->hd;struct Cyc_Absyn_Exp*_tmp9AA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A9);
if(_tmp9AA == _tmp9A9)return e;{struct Cyc_Absyn_Primop_e_struct _tmp135A;struct Cyc_Absyn_Exp*
_tmp1359[1];struct Cyc_Absyn_Primop_e_struct*_tmp1358;return Cyc_Tcutil_copye(e,(
void*)((_tmp1358=_cycalloc(sizeof(*_tmp1358)),((_tmp1358[0]=((_tmp135A.tag=3,((
_tmp135A.f1=_tmp98C,((_tmp135A.f2=((_tmp1359[0]=_tmp9AA,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1359,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp135A)))))),_tmp1358)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp98D)== 2){struct Cyc_Absyn_Exp*_tmp9AE=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp98D))->hd;struct Cyc_Absyn_Exp*_tmp9AF=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp98D->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9B0=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AE);struct Cyc_Absyn_Exp*
_tmp9B1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AF);if(_tmp9B0 == _tmp9AE  && _tmp9B1 == 
_tmp9AF)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1360;struct Cyc_Absyn_Exp*
_tmp135F[2];struct Cyc_Absyn_Primop_e_struct*_tmp135E;return Cyc_Tcutil_copye(e,(
void*)((_tmp135E=_cycalloc(sizeof(*_tmp135E)),((_tmp135E[0]=((_tmp1360.tag=3,((
_tmp1360.f1=_tmp98C,((_tmp1360.f2=((_tmp135F[1]=_tmp9B1,((_tmp135F[0]=_tmp9B0,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp135F,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1360)))))),_tmp135E)))));};}
else{const char*_tmp1363;void*_tmp1362;return(_tmp1362=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1363="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1363,sizeof(char),34))),_tag_dyneither(_tmp1362,sizeof(void*),
0)));}}_LL4ED: {struct Cyc_Absyn_Conditional_e_struct*_tmp98E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp986;if(_tmp98E->tag != 6)goto _LL4EF;else{_tmp98F=_tmp98E->f1;_tmp990=_tmp98E->f2;
_tmp991=_tmp98E->f3;}}_LL4EE: {struct Cyc_Absyn_Exp*_tmp9B7=Cyc_Tcutil_rsubsexp(r,
inst,_tmp98F);struct Cyc_Absyn_Exp*_tmp9B8=Cyc_Tcutil_rsubsexp(r,inst,_tmp990);
struct Cyc_Absyn_Exp*_tmp9B9=Cyc_Tcutil_rsubsexp(r,inst,_tmp991);if((_tmp9B7 == 
_tmp98F  && _tmp9B8 == _tmp990) && _tmp9B9 == _tmp991)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1366;struct Cyc_Absyn_Conditional_e_struct*_tmp1365;return Cyc_Tcutil_copye(e,(
void*)((_tmp1365=_cycalloc(sizeof(*_tmp1365)),((_tmp1365[0]=((_tmp1366.tag=6,((
_tmp1366.f1=_tmp9B7,((_tmp1366.f2=_tmp9B8,((_tmp1366.f3=_tmp9B9,_tmp1366)))))))),
_tmp1365)))));};}_LL4EF: {struct Cyc_Absyn_And_e_struct*_tmp992=(struct Cyc_Absyn_And_e_struct*)
_tmp986;if(_tmp992->tag != 7)goto _LL4F1;else{_tmp993=_tmp992->f1;_tmp994=_tmp992->f2;}}
_LL4F0: {struct Cyc_Absyn_Exp*_tmp9BC=Cyc_Tcutil_rsubsexp(r,inst,_tmp993);struct
Cyc_Absyn_Exp*_tmp9BD=Cyc_Tcutil_rsubsexp(r,inst,_tmp994);if(_tmp9BC == _tmp993
 && _tmp9BD == _tmp994)return e;{struct Cyc_Absyn_And_e_struct _tmp1369;struct Cyc_Absyn_And_e_struct*
_tmp1368;return Cyc_Tcutil_copye(e,(void*)((_tmp1368=_cycalloc(sizeof(*_tmp1368)),((
_tmp1368[0]=((_tmp1369.tag=7,((_tmp1369.f1=_tmp9BC,((_tmp1369.f2=_tmp9BD,
_tmp1369)))))),_tmp1368)))));};}_LL4F1: {struct Cyc_Absyn_Or_e_struct*_tmp995=(
struct Cyc_Absyn_Or_e_struct*)_tmp986;if(_tmp995->tag != 8)goto _LL4F3;else{_tmp996=
_tmp995->f1;_tmp997=_tmp995->f2;}}_LL4F2: {struct Cyc_Absyn_Exp*_tmp9C0=Cyc_Tcutil_rsubsexp(
r,inst,_tmp996);struct Cyc_Absyn_Exp*_tmp9C1=Cyc_Tcutil_rsubsexp(r,inst,_tmp997);
if(_tmp9C0 == _tmp996  && _tmp9C1 == _tmp997)return e;{struct Cyc_Absyn_Or_e_struct
_tmp136C;struct Cyc_Absyn_Or_e_struct*_tmp136B;return Cyc_Tcutil_copye(e,(void*)((
_tmp136B=_cycalloc(sizeof(*_tmp136B)),((_tmp136B[0]=((_tmp136C.tag=8,((_tmp136C.f1=
_tmp9C0,((_tmp136C.f2=_tmp9C1,_tmp136C)))))),_tmp136B)))));};}_LL4F3: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp998=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp986;if(_tmp998->tag != 9)goto _LL4F5;
else{_tmp999=_tmp998->f1;_tmp99A=_tmp998->f2;}}_LL4F4: {struct Cyc_Absyn_Exp*
_tmp9C4=Cyc_Tcutil_rsubsexp(r,inst,_tmp999);struct Cyc_Absyn_Exp*_tmp9C5=Cyc_Tcutil_rsubsexp(
r,inst,_tmp99A);if(_tmp9C4 == _tmp999  && _tmp9C5 == _tmp99A)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp136F;struct Cyc_Absyn_SeqExp_e_struct*_tmp136E;return Cyc_Tcutil_copye(e,(void*)((
_tmp136E=_cycalloc(sizeof(*_tmp136E)),((_tmp136E[0]=((_tmp136F.tag=9,((_tmp136F.f1=
_tmp9C4,((_tmp136F.f2=_tmp9C5,_tmp136F)))))),_tmp136E)))));};}_LL4F5: {struct Cyc_Absyn_Cast_e_struct*
_tmp99B=(struct Cyc_Absyn_Cast_e_struct*)_tmp986;if(_tmp99B->tag != 15)goto _LL4F7;
else{_tmp99C=(void*)_tmp99B->f1;_tmp99D=_tmp99B->f2;_tmp99E=_tmp99B->f3;_tmp99F=
_tmp99B->f4;}}_LL4F6: {struct Cyc_Absyn_Exp*_tmp9C8=Cyc_Tcutil_rsubsexp(r,inst,
_tmp99D);void*_tmp9C9=Cyc_Tcutil_rsubstitute(r,inst,_tmp99C);if(_tmp9C8 == 
_tmp99D  && _tmp9C9 == _tmp99C)return e;{struct Cyc_Absyn_Cast_e_struct _tmp1372;
struct Cyc_Absyn_Cast_e_struct*_tmp1371;return Cyc_Tcutil_copye(e,(void*)((
_tmp1371=_cycalloc(sizeof(*_tmp1371)),((_tmp1371[0]=((_tmp1372.tag=15,((_tmp1372.f1=(
void*)_tmp9C9,((_tmp1372.f2=_tmp9C8,((_tmp1372.f3=_tmp99E,((_tmp1372.f4=_tmp99F,
_tmp1372)))))))))),_tmp1371)))));};}_LL4F7: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9A0=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp986;if(_tmp9A0->tag != 18)goto
_LL4F9;else{_tmp9A1=(void*)_tmp9A0->f1;}}_LL4F8: {void*_tmp9CC=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9A1);if(_tmp9CC == _tmp9A1)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp1375;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1374;return Cyc_Tcutil_copye(e,(
void*)((_tmp1374=_cycalloc(sizeof(*_tmp1374)),((_tmp1374[0]=((_tmp1375.tag=18,((
_tmp1375.f1=(void*)_tmp9CC,_tmp1375)))),_tmp1374)))));};}_LL4F9: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9A2=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp986;if(_tmp9A2->tag != 19)goto
_LL4FB;else{_tmp9A3=_tmp9A2->f1;}}_LL4FA: {struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9A3);if(_tmp9CF == _tmp9A3)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp1378;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1377;return Cyc_Tcutil_copye(e,(
void*)((_tmp1377=_cycalloc(sizeof(*_tmp1377)),((_tmp1377[0]=((_tmp1378.tag=19,((
_tmp1378.f1=_tmp9CF,_tmp1378)))),_tmp1377)))));};}_LL4FB: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9A4=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp986;if(_tmp9A4->tag != 20)goto
_LL4FD;else{_tmp9A5=(void*)_tmp9A4->f1;_tmp9A6=(void*)_tmp9A4->f2;}}_LL4FC: {
void*_tmp9D2=Cyc_Tcutil_rsubstitute(r,inst,_tmp9A5);if(_tmp9D2 == _tmp9A5)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp137B;struct Cyc_Absyn_Offsetof_e_struct*
_tmp137A;return Cyc_Tcutil_copye(e,(void*)((_tmp137A=_cycalloc(sizeof(*_tmp137A)),((
_tmp137A[0]=((_tmp137B.tag=20,((_tmp137B.f1=(void*)_tmp9D2,((_tmp137B.f2=(void*)
_tmp9A6,_tmp137B)))))),_tmp137A)))));};}_LL4FD: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9A7=(struct Cyc_Absyn_Valueof_e_struct*)_tmp986;if(_tmp9A7->tag != 39)goto
_LL4FF;else{_tmp9A8=(void*)_tmp9A7->f1;}}_LL4FE: {void*_tmp9D5=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9A8);if(_tmp9D5 == _tmp9A8)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp137E;struct Cyc_Absyn_Valueof_e_struct*_tmp137D;return Cyc_Tcutil_copye(e,(
void*)((_tmp137D=_cycalloc(sizeof(*_tmp137D)),((_tmp137D[0]=((_tmp137E.tag=39,((
_tmp137E.f1=(void*)_tmp9D5,_tmp137E)))),_tmp137D)))));};}_LL4FF:;_LL500: {const
char*_tmp1381;void*_tmp1380;return(_tmp1380=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1381="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp1381,sizeof(char),46))),_tag_dyneither(_tmp1380,sizeof(void*),
0)));}_LL4E2:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9DA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9DC;
struct Cyc_Absyn_AggrInfo _tmp9DE;union Cyc_Absyn_AggrInfoU _tmp9DF;struct Cyc_List_List*
_tmp9E0;struct Cyc_Absyn_DatatypeInfo _tmp9E2;union Cyc_Absyn_DatatypeInfoU _tmp9E3;
struct Cyc_List_List*_tmp9E4;struct Cyc_Absyn_DatatypeFieldInfo _tmp9E6;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9E7;struct Cyc_List_List*_tmp9E8;struct _tuple2*_tmp9EA;struct Cyc_List_List*
_tmp9EB;struct Cyc_Absyn_Typedefdecl*_tmp9EC;void**_tmp9ED;struct Cyc_Absyn_ArrayInfo
_tmp9EF;void*_tmp9F0;struct Cyc_Absyn_Tqual _tmp9F1;struct Cyc_Absyn_Exp*_tmp9F2;
union Cyc_Absyn_Constraint*_tmp9F3;struct Cyc_Position_Segment*_tmp9F4;struct Cyc_Absyn_PtrInfo
_tmp9F6;void*_tmp9F7;struct Cyc_Absyn_Tqual _tmp9F8;struct Cyc_Absyn_PtrAtts _tmp9F9;
void*_tmp9FA;union Cyc_Absyn_Constraint*_tmp9FB;union Cyc_Absyn_Constraint*_tmp9FC;
union Cyc_Absyn_Constraint*_tmp9FD;struct Cyc_Absyn_FnInfo _tmp9FF;struct Cyc_List_List*
_tmpA00;struct Cyc_Core_Opt*_tmpA01;void*_tmpA02;struct Cyc_List_List*_tmpA03;int
_tmpA04;struct Cyc_Absyn_VarargInfo*_tmpA05;struct Cyc_List_List*_tmpA06;struct Cyc_List_List*
_tmpA07;struct Cyc_List_List*_tmpA09;enum Cyc_Absyn_AggrKind _tmpA0B;struct Cyc_List_List*
_tmpA0C;struct Cyc_Core_Opt*_tmpA0E;void*_tmpA10;void*_tmpA12;void*_tmpA13;void*
_tmpA15;struct Cyc_Absyn_Exp*_tmpA17;void*_tmpA21;void*_tmpA23;struct Cyc_List_List*
_tmpA25;_LL502: {struct Cyc_Absyn_VarType_struct*_tmp9DB=(struct Cyc_Absyn_VarType_struct*)
_tmp9DA;if(_tmp9DB->tag != 2)goto _LL504;else{_tmp9DC=_tmp9DB->f1;}}_LL503: {
struct _handler_cons _tmpA27;_push_handler(& _tmpA27);{int _tmpA29=0;if(setjmp(
_tmpA27.handler))_tmpA29=1;if(!_tmpA29){{void*_tmpA2A=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp9DC);_npop_handler(0);return
_tmpA2A;};_pop_handler();}else{void*_tmpA28=(void*)_exn_thrown;void*_tmpA2C=
_tmpA28;_LL539: {struct Cyc_Core_Not_found_struct*_tmpA2D=(struct Cyc_Core_Not_found_struct*)
_tmpA2C;if(_tmpA2D->tag != Cyc_Core_Not_found)goto _LL53B;}_LL53A: return t;_LL53B:;
_LL53C:(void)_throw(_tmpA2C);_LL538:;}};}_LL504: {struct Cyc_Absyn_AggrType_struct*
_tmp9DD=(struct Cyc_Absyn_AggrType_struct*)_tmp9DA;if(_tmp9DD->tag != 12)goto
_LL506;else{_tmp9DE=_tmp9DD->f1;_tmp9DF=_tmp9DE.aggr_info;_tmp9E0=_tmp9DE.targs;}}
_LL505: {struct Cyc_List_List*_tmpA2E=Cyc_Tcutil_substs(rgn,inst,_tmp9E0);struct
Cyc_Absyn_AggrType_struct _tmp1387;struct Cyc_Absyn_AggrInfo _tmp1386;struct Cyc_Absyn_AggrType_struct*
_tmp1385;return _tmpA2E == _tmp9E0?t:(void*)((_tmp1385=_cycalloc(sizeof(*_tmp1385)),((
_tmp1385[0]=((_tmp1387.tag=12,((_tmp1387.f1=((_tmp1386.aggr_info=_tmp9DF,((
_tmp1386.targs=_tmpA2E,_tmp1386)))),_tmp1387)))),_tmp1385))));}_LL506: {struct
Cyc_Absyn_DatatypeType_struct*_tmp9E1=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp9DA;if(_tmp9E1->tag != 3)goto _LL508;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=_tmp9E2.datatype_info;
_tmp9E4=_tmp9E2.targs;}}_LL507: {struct Cyc_List_List*_tmpA32=Cyc_Tcutil_substs(
rgn,inst,_tmp9E4);struct Cyc_Absyn_DatatypeType_struct _tmp138D;struct Cyc_Absyn_DatatypeInfo
_tmp138C;struct Cyc_Absyn_DatatypeType_struct*_tmp138B;return _tmpA32 == _tmp9E4?t:(
void*)((_tmp138B=_cycalloc(sizeof(*_tmp138B)),((_tmp138B[0]=((_tmp138D.tag=3,((
_tmp138D.f1=((_tmp138C.datatype_info=_tmp9E3,((_tmp138C.targs=_tmpA32,_tmp138C)))),
_tmp138D)))),_tmp138B))));}_LL508: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp9E5=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp9DA;if(_tmp9E5->tag != 4)
goto _LL50A;else{_tmp9E6=_tmp9E5->f1;_tmp9E7=_tmp9E6.field_info;_tmp9E8=_tmp9E6.targs;}}
_LL509: {struct Cyc_List_List*_tmpA36=Cyc_Tcutil_substs(rgn,inst,_tmp9E8);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp1393;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1392;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1391;return _tmpA36 == 
_tmp9E8?t:(void*)((_tmp1391=_cycalloc(sizeof(*_tmp1391)),((_tmp1391[0]=((
_tmp1393.tag=4,((_tmp1393.f1=((_tmp1392.field_info=_tmp9E7,((_tmp1392.targs=
_tmpA36,_tmp1392)))),_tmp1393)))),_tmp1391))));}_LL50A: {struct Cyc_Absyn_TypedefType_struct*
_tmp9E9=(struct Cyc_Absyn_TypedefType_struct*)_tmp9DA;if(_tmp9E9->tag != 18)goto
_LL50C;else{_tmp9EA=_tmp9E9->f1;_tmp9EB=_tmp9E9->f2;_tmp9EC=_tmp9E9->f3;_tmp9ED=
_tmp9E9->f4;}}_LL50B: {struct Cyc_List_List*_tmpA3A=Cyc_Tcutil_substs(rgn,inst,
_tmp9EB);struct Cyc_Absyn_TypedefType_struct _tmp1396;struct Cyc_Absyn_TypedefType_struct*
_tmp1395;return _tmpA3A == _tmp9EB?t:(void*)((_tmp1395=_cycalloc(sizeof(*_tmp1395)),((
_tmp1395[0]=((_tmp1396.tag=18,((_tmp1396.f1=_tmp9EA,((_tmp1396.f2=_tmpA3A,((
_tmp1396.f3=_tmp9EC,((_tmp1396.f4=_tmp9ED,_tmp1396)))))))))),_tmp1395))));}
_LL50C: {struct Cyc_Absyn_ArrayType_struct*_tmp9EE=(struct Cyc_Absyn_ArrayType_struct*)
_tmp9DA;if(_tmp9EE->tag != 9)goto _LL50E;else{_tmp9EF=_tmp9EE->f1;_tmp9F0=_tmp9EF.elt_type;
_tmp9F1=_tmp9EF.tq;_tmp9F2=_tmp9EF.num_elts;_tmp9F3=_tmp9EF.zero_term;_tmp9F4=
_tmp9EF.zt_loc;}}_LL50D: {void*_tmpA3D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9F0);
struct Cyc_Absyn_ArrayType_struct _tmp139C;struct Cyc_Absyn_ArrayInfo _tmp139B;
struct Cyc_Absyn_ArrayType_struct*_tmp139A;return _tmpA3D == _tmp9F0?t:(void*)((
_tmp139A=_cycalloc(sizeof(*_tmp139A)),((_tmp139A[0]=((_tmp139C.tag=9,((_tmp139C.f1=((
_tmp139B.elt_type=_tmpA3D,((_tmp139B.tq=_tmp9F1,((_tmp139B.num_elts=_tmp9F2,((
_tmp139B.zero_term=_tmp9F3,((_tmp139B.zt_loc=_tmp9F4,_tmp139B)))))))))),_tmp139C)))),
_tmp139A))));}_LL50E: {struct Cyc_Absyn_PointerType_struct*_tmp9F5=(struct Cyc_Absyn_PointerType_struct*)
_tmp9DA;if(_tmp9F5->tag != 5)goto _LL510;else{_tmp9F6=_tmp9F5->f1;_tmp9F7=_tmp9F6.elt_typ;
_tmp9F8=_tmp9F6.elt_tq;_tmp9F9=_tmp9F6.ptr_atts;_tmp9FA=_tmp9F9.rgn;_tmp9FB=
_tmp9F9.nullable;_tmp9FC=_tmp9F9.bounds;_tmp9FD=_tmp9F9.zero_term;}}_LL50F: {
void*_tmpA41=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9F7);void*_tmpA42=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9FA);union Cyc_Absyn_Constraint*_tmpA43=_tmp9FC;{void*_tmpA44=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp9FC);struct Cyc_Absyn_Exp*_tmpA46;_LL53E: {struct Cyc_Absyn_Upper_b_struct*
_tmpA45=(struct Cyc_Absyn_Upper_b_struct*)_tmpA44;if(_tmpA45->tag != 1)goto _LL540;
else{_tmpA46=_tmpA45->f1;}}_LL53F: {struct Cyc_Absyn_Exp*_tmpA47=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA46);if(_tmpA47 != _tmpA46){struct Cyc_Absyn_Upper_b_struct _tmp139F;
struct Cyc_Absyn_Upper_b_struct*_tmp139E;_tmpA43=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp139E=_cycalloc(sizeof(*_tmp139E)),((
_tmp139E[0]=((_tmp139F.tag=1,((_tmp139F.f1=_tmpA47,_tmp139F)))),_tmp139E)))));}
goto _LL53D;}_LL540:;_LL541: goto _LL53D;_LL53D:;}if((_tmpA41 == _tmp9F7  && _tmpA42
== _tmp9FA) && _tmpA43 == _tmp9FC)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13A9;struct Cyc_Absyn_PtrAtts _tmp13A8;struct Cyc_Absyn_PtrInfo _tmp13A7;struct
Cyc_Absyn_PointerType_struct*_tmp13A6;return(void*)((_tmp13A6=_cycalloc(sizeof(*
_tmp13A6)),((_tmp13A6[0]=((_tmp13A9.tag=5,((_tmp13A9.f1=((_tmp13A7.elt_typ=
_tmpA41,((_tmp13A7.elt_tq=_tmp9F8,((_tmp13A7.ptr_atts=((_tmp13A8.rgn=_tmpA42,((
_tmp13A8.nullable=_tmp9FB,((_tmp13A8.bounds=_tmpA43,((_tmp13A8.zero_term=_tmp9FD,((
_tmp13A8.ptrloc=0,_tmp13A8)))))))))),_tmp13A7)))))),_tmp13A9)))),_tmp13A6))));};}
_LL510: {struct Cyc_Absyn_FnType_struct*_tmp9FE=(struct Cyc_Absyn_FnType_struct*)
_tmp9DA;if(_tmp9FE->tag != 10)goto _LL512;else{_tmp9FF=_tmp9FE->f1;_tmpA00=_tmp9FF.tvars;
_tmpA01=_tmp9FF.effect;_tmpA02=_tmp9FF.ret_typ;_tmpA03=_tmp9FF.args;_tmpA04=
_tmp9FF.c_varargs;_tmpA05=_tmp9FF.cyc_varargs;_tmpA06=_tmp9FF.rgn_po;_tmpA07=
_tmp9FF.attributes;}}_LL511:{struct Cyc_List_List*_tmpA4E=_tmpA00;for(0;_tmpA4E != 
0;_tmpA4E=_tmpA4E->tl){struct _tuple15*_tmp13B3;struct Cyc_Absyn_VarType_struct
_tmp13B2;struct Cyc_Absyn_VarType_struct*_tmp13B1;struct Cyc_List_List*_tmp13B0;
inst=((_tmp13B0=_region_malloc(rgn,sizeof(*_tmp13B0)),((_tmp13B0->hd=((_tmp13B3=
_region_malloc(rgn,sizeof(*_tmp13B3)),((_tmp13B3->f1=(struct Cyc_Absyn_Tvar*)
_tmpA4E->hd,((_tmp13B3->f2=(void*)((_tmp13B1=_cycalloc(sizeof(*_tmp13B1)),((
_tmp13B1[0]=((_tmp13B2.tag=2,((_tmp13B2.f1=(struct Cyc_Absyn_Tvar*)_tmpA4E->hd,
_tmp13B2)))),_tmp13B1)))),_tmp13B3)))))),((_tmp13B0->tl=inst,_tmp13B0))))));}}{
struct Cyc_List_List*_tmpA54;struct Cyc_List_List*_tmpA55;struct _tuple1 _tmpA53=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpA03));
_tmpA54=_tmpA53.f1;_tmpA55=_tmpA53.f2;{struct Cyc_List_List*_tmpA56=Cyc_Tcutil_substs(
rgn,inst,_tmpA55);struct Cyc_List_List*_tmpA57=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA54,_tmpA56));struct Cyc_Core_Opt*
eff2;if(_tmpA01 == 0)eff2=0;else{void*_tmpA58=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA01->v);if(_tmpA58 == (void*)_tmpA01->v)eff2=_tmpA01;else{struct Cyc_Core_Opt*
_tmp13B4;eff2=((_tmp13B4=_cycalloc(sizeof(*_tmp13B4)),((_tmp13B4->v=_tmpA58,
_tmp13B4))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA05 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA5B;struct Cyc_Absyn_Tqual _tmpA5C;void*
_tmpA5D;int _tmpA5E;struct Cyc_Absyn_VarargInfo _tmpA5A=*_tmpA05;_tmpA5B=_tmpA5A.name;
_tmpA5C=_tmpA5A.tq;_tmpA5D=_tmpA5A.type;_tmpA5E=_tmpA5A.inject;{void*_tmpA5F=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA5D);if(_tmpA5F == _tmpA5D)cyc_varargs2=_tmpA05;else{struct Cyc_Absyn_VarargInfo*
_tmp13B5;cyc_varargs2=((_tmp13B5=_cycalloc(sizeof(*_tmp13B5)),((_tmp13B5->name=
_tmpA5B,((_tmp13B5->tq=_tmpA5C,((_tmp13B5->type=_tmpA5F,((_tmp13B5->inject=
_tmpA5E,_tmp13B5))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA62;struct Cyc_List_List*_tmpA63;struct _tuple1 _tmpA61=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA06);_tmpA62=_tmpA61.f1;_tmpA63=_tmpA61.f2;{struct Cyc_List_List*
_tmpA64=Cyc_Tcutil_substs(rgn,inst,_tmpA62);struct Cyc_List_List*_tmpA65=Cyc_Tcutil_substs(
rgn,inst,_tmpA63);if(_tmpA64 == _tmpA62  && _tmpA65 == _tmpA63)rgn_po2=_tmpA06;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA64,_tmpA65);}{struct Cyc_Absyn_FnType_struct _tmp13BB;struct
Cyc_Absyn_FnInfo _tmp13BA;struct Cyc_Absyn_FnType_struct*_tmp13B9;return(void*)((
_tmp13B9=_cycalloc(sizeof(*_tmp13B9)),((_tmp13B9[0]=((_tmp13BB.tag=10,((_tmp13BB.f1=((
_tmp13BA.tvars=_tmpA00,((_tmp13BA.effect=eff2,((_tmp13BA.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA02),((_tmp13BA.args=_tmpA57,((_tmp13BA.c_varargs=_tmpA04,((
_tmp13BA.cyc_varargs=cyc_varargs2,((_tmp13BA.rgn_po=rgn_po2,((_tmp13BA.attributes=
_tmpA07,_tmp13BA)))))))))))))))),_tmp13BB)))),_tmp13B9))));};};};};};};_LL512: {
struct Cyc_Absyn_TupleType_struct*_tmpA08=(struct Cyc_Absyn_TupleType_struct*)
_tmp9DA;if(_tmpA08->tag != 11)goto _LL514;else{_tmpA09=_tmpA08->f1;}}_LL513: {
struct Cyc_List_List*_tmpA69=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmpA09);struct Cyc_List_List*_tmpA6A=Cyc_Tcutil_substs(rgn,inst,_tmpA69);if(
_tmpA6A == _tmpA69)return t;{struct Cyc_List_List*_tmpA6B=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpA09,_tmpA6A);struct Cyc_Absyn_TupleType_struct
_tmp13BE;struct Cyc_Absyn_TupleType_struct*_tmp13BD;return(void*)((_tmp13BD=
_cycalloc(sizeof(*_tmp13BD)),((_tmp13BD[0]=((_tmp13BE.tag=11,((_tmp13BE.f1=
_tmpA6B,_tmp13BE)))),_tmp13BD))));};}_LL514: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpA0A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp9DA;if(_tmpA0A->tag != 13)goto
_LL516;else{_tmpA0B=_tmpA0A->f1;_tmpA0C=_tmpA0A->f2;}}_LL515: {struct Cyc_List_List*
_tmpA6E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmpA0C);struct
Cyc_List_List*_tmpA6F=Cyc_Tcutil_substs(rgn,inst,_tmpA6E);if(_tmpA6F == _tmpA6E)
return t;{struct Cyc_List_List*_tmpA70=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmpA0C,_tmpA6F);struct Cyc_Absyn_AnonAggrType_struct
_tmp13C1;struct Cyc_Absyn_AnonAggrType_struct*_tmp13C0;return(void*)((_tmp13C0=
_cycalloc(sizeof(*_tmp13C0)),((_tmp13C0[0]=((_tmp13C1.tag=13,((_tmp13C1.f1=
_tmpA0B,((_tmp13C1.f2=_tmpA70,_tmp13C1)))))),_tmp13C0))));};}_LL516: {struct Cyc_Absyn_Evar_struct*
_tmpA0D=(struct Cyc_Absyn_Evar_struct*)_tmp9DA;if(_tmpA0D->tag != 1)goto _LL518;
else{_tmpA0E=_tmpA0D->f2;}}_LL517: if(_tmpA0E != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmpA0E->v);else{return t;}_LL518: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpA0F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp9DA;if(_tmpA0F->tag != 16)goto
_LL51A;else{_tmpA10=(void*)_tmpA0F->f1;}}_LL519: {void*_tmpA73=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA10);struct Cyc_Absyn_RgnHandleType_struct _tmp13C4;struct Cyc_Absyn_RgnHandleType_struct*
_tmp13C3;return _tmpA73 == _tmpA10?t:(void*)((_tmp13C3=_cycalloc(sizeof(*_tmp13C3)),((
_tmp13C3[0]=((_tmp13C4.tag=16,((_tmp13C4.f1=(void*)_tmpA73,_tmp13C4)))),_tmp13C3))));}
_LL51A: {struct Cyc_Absyn_DynRgnType_struct*_tmpA11=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp9DA;if(_tmpA11->tag != 17)goto _LL51C;else{_tmpA12=(void*)_tmpA11->f1;_tmpA13=(
void*)_tmpA11->f2;}}_LL51B: {void*_tmpA76=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpA12);void*_tmpA77=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA13);struct Cyc_Absyn_DynRgnType_struct
_tmp13C7;struct Cyc_Absyn_DynRgnType_struct*_tmp13C6;return _tmpA76 == _tmpA12  && 
_tmpA77 == _tmpA13?t:(void*)((_tmp13C6=_cycalloc(sizeof(*_tmp13C6)),((_tmp13C6[0]=((
_tmp13C7.tag=17,((_tmp13C7.f1=(void*)_tmpA76,((_tmp13C7.f2=(void*)_tmpA77,
_tmp13C7)))))),_tmp13C6))));}_LL51C: {struct Cyc_Absyn_TagType_struct*_tmpA14=(
struct Cyc_Absyn_TagType_struct*)_tmp9DA;if(_tmpA14->tag != 20)goto _LL51E;else{
_tmpA15=(void*)_tmpA14->f1;}}_LL51D: {void*_tmpA7A=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmpA15);struct Cyc_Absyn_TagType_struct _tmp13CA;struct Cyc_Absyn_TagType_struct*
_tmp13C9;return _tmpA7A == _tmpA15?t:(void*)((_tmp13C9=_cycalloc(sizeof(*_tmp13C9)),((
_tmp13C9[0]=((_tmp13CA.tag=20,((_tmp13CA.f1=(void*)_tmpA7A,_tmp13CA)))),_tmp13C9))));}
_LL51E: {struct Cyc_Absyn_ValueofType_struct*_tmpA16=(struct Cyc_Absyn_ValueofType_struct*)
_tmp9DA;if(_tmpA16->tag != 19)goto _LL520;else{_tmpA17=_tmpA16->f1;}}_LL51F: {
struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA17);struct Cyc_Absyn_ValueofType_struct
_tmp13CD;struct Cyc_Absyn_ValueofType_struct*_tmp13CC;return _tmpA7D == _tmpA17?t:(
void*)((_tmp13CC=_cycalloc(sizeof(*_tmp13CC)),((_tmp13CC[0]=((_tmp13CD.tag=19,((
_tmp13CD.f1=_tmpA7D,_tmp13CD)))),_tmp13CC))));}_LL520: {struct Cyc_Absyn_EnumType_struct*
_tmpA18=(struct Cyc_Absyn_EnumType_struct*)_tmp9DA;if(_tmpA18->tag != 14)goto
_LL522;}_LL521: goto _LL523;_LL522: {struct Cyc_Absyn_AnonEnumType_struct*_tmpA19=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp9DA;if(_tmpA19->tag != 15)goto _LL524;}
_LL523: goto _LL525;_LL524: {struct Cyc_Absyn_VoidType_struct*_tmpA1A=(struct Cyc_Absyn_VoidType_struct*)
_tmp9DA;if(_tmpA1A->tag != 0)goto _LL526;}_LL525: goto _LL527;_LL526: {struct Cyc_Absyn_IntType_struct*
_tmpA1B=(struct Cyc_Absyn_IntType_struct*)_tmp9DA;if(_tmpA1B->tag != 6)goto _LL528;}
_LL527: goto _LL529;_LL528: {struct Cyc_Absyn_FloatType_struct*_tmpA1C=(struct Cyc_Absyn_FloatType_struct*)
_tmp9DA;if(_tmpA1C->tag != 7)goto _LL52A;}_LL529: goto _LL52B;_LL52A: {struct Cyc_Absyn_DoubleType_struct*
_tmpA1D=(struct Cyc_Absyn_DoubleType_struct*)_tmp9DA;if(_tmpA1D->tag != 8)goto
_LL52C;}_LL52B: goto _LL52D;_LL52C: {struct Cyc_Absyn_UniqueRgn_struct*_tmpA1E=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp9DA;if(_tmpA1E->tag != 22)goto _LL52E;}
_LL52D: goto _LL52F;_LL52E: {struct Cyc_Absyn_HeapRgn_struct*_tmpA1F=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp9DA;if(_tmpA1F->tag != 21)goto _LL530;}_LL52F: return t;_LL530: {struct Cyc_Absyn_RgnsEff_struct*
_tmpA20=(struct Cyc_Absyn_RgnsEff_struct*)_tmp9DA;if(_tmpA20->tag != 25)goto _LL532;
else{_tmpA21=(void*)_tmpA20->f1;}}_LL531: {void*_tmpA80=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA21);struct Cyc_Absyn_RgnsEff_struct _tmp13D0;struct Cyc_Absyn_RgnsEff_struct*
_tmp13CF;return _tmpA80 == _tmpA21?t:(void*)((_tmp13CF=_cycalloc(sizeof(*_tmp13CF)),((
_tmp13CF[0]=((_tmp13D0.tag=25,((_tmp13D0.f1=(void*)_tmpA80,_tmp13D0)))),_tmp13CF))));}
_LL532: {struct Cyc_Absyn_AccessEff_struct*_tmpA22=(struct Cyc_Absyn_AccessEff_struct*)
_tmp9DA;if(_tmpA22->tag != 23)goto _LL534;else{_tmpA23=(void*)_tmpA22->f1;}}_LL533: {
void*_tmpA83=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA23);struct Cyc_Absyn_AccessEff_struct
_tmp13D3;struct Cyc_Absyn_AccessEff_struct*_tmp13D2;return _tmpA83 == _tmpA23?t:(
void*)((_tmp13D2=_cycalloc(sizeof(*_tmp13D2)),((_tmp13D2[0]=((_tmp13D3.tag=23,((
_tmp13D3.f1=(void*)_tmpA83,_tmp13D3)))),_tmp13D2))));}_LL534: {struct Cyc_Absyn_JoinEff_struct*
_tmpA24=(struct Cyc_Absyn_JoinEff_struct*)_tmp9DA;if(_tmpA24->tag != 24)goto _LL536;
else{_tmpA25=_tmpA24->f1;}}_LL535: {struct Cyc_List_List*_tmpA86=Cyc_Tcutil_substs(
rgn,inst,_tmpA25);struct Cyc_Absyn_JoinEff_struct _tmp13D6;struct Cyc_Absyn_JoinEff_struct*
_tmp13D5;return _tmpA86 == _tmpA25?t:(void*)((_tmp13D5=_cycalloc(sizeof(*_tmp13D5)),((
_tmp13D5[0]=((_tmp13D6.tag=24,((_tmp13D6.f1=_tmpA86,_tmp13D6)))),_tmp13D5))));}
_LL536: {struct Cyc_Absyn_TypeDeclType_struct*_tmpA26=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp9DA;if(_tmpA26->tag != 26)goto _LL501;}_LL537: {const char*_tmp13D9;void*
_tmp13D8;(_tmp13D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp13D9="found typedecltype in rsubs",_tag_dyneither(
_tmp13D9,sizeof(char),28))),_tag_dyneither(_tmp13D8,sizeof(void*),0)));}_LL501:;}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmpA8B=(void*)ts->hd;
struct Cyc_List_List*_tmpA8C=ts->tl;void*_tmpA8D=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpA8B);struct Cyc_List_List*_tmpA8E=Cyc_Tcutil_substs(rgn,inst,_tmpA8C);if(
_tmpA8B == _tmpA8D  && _tmpA8C == _tmpA8E)return ts;{struct Cyc_List_List*_tmp13DA;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13DA=_cycalloc(sizeof(*
_tmp13DA)),((_tmp13DA->hd=_tmpA8D,((_tmp13DA->tl=_tmpA8E,_tmp13DA)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpA90=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13DD;struct _tuple15*_tmp13DC;return(
_tmp13DC=_cycalloc(sizeof(*_tmp13DC)),((_tmp13DC->f1=tv,((_tmp13DC->f2=Cyc_Absyn_new_evar(
_tmpA90,((_tmp13DD=_cycalloc(sizeof(*_tmp13DD)),((_tmp13DD->v=s,_tmp13DD))))),
_tmp13DC)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpA94;struct Cyc_List_List*_tmpA95;
struct _RegionHandle*_tmpA96;struct _tuple16*_tmpA93=env;_tmpA94=*_tmpA93;_tmpA95=
_tmpA94.f1;_tmpA96=_tmpA94.f2;{struct Cyc_Core_Opt*_tmpA97=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13E0;struct
_tuple15*_tmp13DF;return(_tmp13DF=_region_malloc(_tmpA96,sizeof(*_tmp13DF)),((
_tmp13DF->f1=tv,((_tmp13DF->f2=Cyc_Absyn_new_evar(_tmpA97,((_tmp13E0=_cycalloc(
sizeof(*_tmp13E0)),((_tmp13E0->v=_tmpA95,_tmp13E0))))),_tmp13DF)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp13E6;void*_tmp13E5[3];
struct Cyc_String_pa_struct _tmp13E4;struct Cyc_String_pa_struct _tmp13E3;struct Cyc_String_pa_struct
_tmp13E2;(_tmp13E2.tag=0,((_tmp13E2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp13E3.tag=0,((_tmp13E3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp13E4.tag=0,((_tmp13E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp13E5[0]=& _tmp13E4,((_tmp13E5[1]=& _tmp13E3,((_tmp13E5[2]=& _tmp13E2,Cyc_Tcutil_terr(
loc,((_tmp13E6="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp13E6,sizeof(char),59))),_tag_dyneither(_tmp13E5,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp13E9;void*_tmp13E8;(_tmp13E8=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13E9="same type variable has different identity!",
_tag_dyneither(_tmp13E9,sizeof(char),43))),_tag_dyneither(_tmp13E8,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp13EA;return(_tmp13EA=_cycalloc(sizeof(*_tmp13EA)),((_tmp13EA->hd=tv,((
_tmp13EA->tl=tvs,_tmp13EA)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp13ED;void*_tmp13EC;(_tmp13EC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13ED="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp13ED,sizeof(char),39))),_tag_dyneither(_tmp13EC,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAA4=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAA4->identity == - 1){const char*
_tmp13F0;void*_tmp13EF;(_tmp13EF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F0="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp13F0,sizeof(char),41))),_tag_dyneither(_tmp13EF,sizeof(void*),
0)));}if(_tmpAA4->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp13F1;return(_tmp13F1=_cycalloc(sizeof(*_tmp13F1)),((_tmp13F1->hd=tv,((
_tmp13F1->tl=tvs,_tmp13F1)))));};}struct _tuple29{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp13F4;void*_tmp13F3;(_tmp13F3=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F4="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp13F4,sizeof(char),44))),_tag_dyneither(_tmp13F3,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple29
_tmpAAB;struct Cyc_Absyn_Tvar*_tmpAAC;int _tmpAAD;int*_tmpAAE;struct _tuple29*
_tmpAAA=(struct _tuple29*)tvs2->hd;_tmpAAB=*_tmpAAA;_tmpAAC=_tmpAAB.f1;_tmpAAD=
_tmpAAB.f2;_tmpAAE=(int*)&(*_tmpAAA).f2;if(_tmpAAC->identity == - 1){const char*
_tmp13F7;void*_tmp13F6;(_tmp13F6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13F7="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp13F7,sizeof(char),46))),_tag_dyneither(_tmp13F6,sizeof(void*),
0)));}if(_tmpAAC->identity == tv->identity){*_tmpAAE=*_tmpAAE  || b;return tvs;}}}{
struct _tuple29*_tmp13FA;struct Cyc_List_List*_tmp13F9;return(_tmp13F9=
_region_malloc(r,sizeof(*_tmp13F9)),((_tmp13F9->hd=((_tmp13FA=_region_malloc(r,
sizeof(*_tmp13FA)),((_tmp13FA->f1=tv,((_tmp13FA->f2=b,_tmp13FA)))))),((_tmp13F9->tl=
tvs,_tmp13F9)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp13FE;void*_tmp13FD[1];struct Cyc_String_pa_struct _tmp13FC;(_tmp13FC.tag=0,((
_tmp13FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp13FD[0]=& _tmp13FC,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FE="bound tvar id for %s is NULL",
_tag_dyneither(_tmp13FE,sizeof(char),29))),_tag_dyneither(_tmp13FD,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp13FF;return(_tmp13FF=_cycalloc(sizeof(*
_tmp13FF)),((_tmp13FF->hd=tv,((_tmp13FF->tl=tvs,_tmp13FF)))));};}struct _tuple30{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpAB7=Cyc_Tcutil_compress(
e);int _tmpAB9;_LL543: {struct Cyc_Absyn_Evar_struct*_tmpAB8=(struct Cyc_Absyn_Evar_struct*)
_tmpAB7;if(_tmpAB8->tag != 1)goto _LL545;else{_tmpAB9=_tmpAB8->f3;}}_LL544:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple30 _tmpABB;void*
_tmpABC;int _tmpABD;int*_tmpABE;struct _tuple30*_tmpABA=(struct _tuple30*)es2->hd;
_tmpABB=*_tmpABA;_tmpABC=_tmpABB.f1;_tmpABD=_tmpABB.f2;_tmpABE=(int*)&(*_tmpABA).f2;{
void*_tmpABF=Cyc_Tcutil_compress(_tmpABC);int _tmpAC1;_LL548: {struct Cyc_Absyn_Evar_struct*
_tmpAC0=(struct Cyc_Absyn_Evar_struct*)_tmpABF;if(_tmpAC0->tag != 1)goto _LL54A;
else{_tmpAC1=_tmpAC0->f3;}}_LL549: if(_tmpAB9 == _tmpAC1){if(b != *_tmpABE)*_tmpABE=
1;return es;}goto _LL547;_LL54A:;_LL54B: goto _LL547;_LL547:;};}}{struct _tuple30*
_tmp1402;struct Cyc_List_List*_tmp1401;return(_tmp1401=_region_malloc(r,sizeof(*
_tmp1401)),((_tmp1401->hd=((_tmp1402=_region_malloc(r,sizeof(*_tmp1402)),((
_tmp1402->f1=e,((_tmp1402->f2=b,_tmp1402)))))),((_tmp1401->tl=es,_tmp1401)))));};
_LL545:;_LL546: return es;_LL542:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1403;r=((_tmp1403=_region_malloc(rgn,sizeof(*_tmp1403)),((
_tmp1403->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1403->tl=r,_tmp1403))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAC6;int _tmpAC7;struct
_tuple29 _tmpAC5=*((struct _tuple29*)tvs->hd);_tmpAC6=_tmpAC5.f1;_tmpAC7=_tmpAC5.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpAC6->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1404;res=((_tmp1404=_region_malloc(r,sizeof(*_tmp1404)),((
_tmp1404->hd=(struct _tuple29*)tvs->hd,((_tmp1404->tl=res,_tmp1404))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp1408;void*_tmp1407[1];struct Cyc_String_pa_struct _tmp1406;(_tmp1406.tag=
0,((_tmp1406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1407[0]=&
_tmp1406,Cyc_Tcutil_terr(loc,((_tmp1408="bitfield %s does not have constant width",
_tag_dyneither(_tmp1408,sizeof(char),41))),_tag_dyneither(_tmp1407,sizeof(void*),
1)))))));}else{unsigned int _tmpACD;int _tmpACE;struct _tuple13 _tmpACC=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpACD=_tmpACC.f1;_tmpACE=_tmpACC.f2;if(!_tmpACE){
const char*_tmp140B;void*_tmp140A;(_tmp140A=0,Cyc_Tcutil_terr(loc,((_tmp140B="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp140B,sizeof(char),45))),_tag_dyneither(_tmp140A,sizeof(void*),
0)));}w=_tmpACD;}{void*_tmpAD1=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAD3;_LL54D: {struct Cyc_Absyn_IntType_struct*_tmpAD2=(struct Cyc_Absyn_IntType_struct*)
_tmpAD1;if(_tmpAD2->tag != 6)goto _LL54F;else{_tmpAD3=_tmpAD2->f2;}}_LL54E: switch(
_tmpAD3){case Cyc_Absyn_Char_sz: _LL551: if(w > 8){const char*_tmp140E;void*_tmp140D;(
_tmp140D=0,Cyc_Tcutil_terr(loc,((_tmp140E="bitfield larger than type",
_tag_dyneither(_tmp140E,sizeof(char),26))),_tag_dyneither(_tmp140D,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL552: if(w > 16){const char*_tmp1411;void*
_tmp1410;(_tmp1410=0,Cyc_Tcutil_terr(loc,((_tmp1411="bitfield larger than type",
_tag_dyneither(_tmp1411,sizeof(char),26))),_tag_dyneither(_tmp1410,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL553: goto _LL554;case Cyc_Absyn_Int_sz: _LL554:
if(w > 32){const char*_tmp1414;void*_tmp1413;(_tmp1413=0,Cyc_Tcutil_terr(loc,((
_tmp1414="bitfield larger than type",_tag_dyneither(_tmp1414,sizeof(char),26))),
_tag_dyneither(_tmp1413,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL555: if(w > 64){const char*_tmp1417;void*_tmp1416;(_tmp1416=0,Cyc_Tcutil_terr(
loc,((_tmp1417="bitfield larger than type",_tag_dyneither(_tmp1417,sizeof(char),
26))),_tag_dyneither(_tmp1416,sizeof(void*),0)));}break;}goto _LL54C;_LL54F:;
_LL550:{const char*_tmp141C;void*_tmp141B[2];struct Cyc_String_pa_struct _tmp141A;
struct Cyc_String_pa_struct _tmp1419;(_tmp1419.tag=0,((_tmp1419.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp141A.tag=0,((_tmp141A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp141B[0]=& _tmp141A,((_tmp141B[1]=& _tmp1419,Cyc_Tcutil_terr(loc,((_tmp141C="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp141C,sizeof(char),52))),_tag_dyneither(_tmp141B,sizeof(void*),
2)))))))))))));}goto _LL54C;_LL54C:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpAE0=(void*)atts->hd;_LL558: {struct Cyc_Absyn_Packed_att_struct*
_tmpAE1=(struct Cyc_Absyn_Packed_att_struct*)_tmpAE0;if(_tmpAE1->tag != 7)goto
_LL55A;}_LL559: continue;_LL55A: {struct Cyc_Absyn_Aligned_att_struct*_tmpAE2=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpAE0;if(_tmpAE2->tag != 6)goto _LL55C;}
_LL55B: continue;_LL55C:;_LL55D: {const char*_tmp1421;void*_tmp1420[2];struct Cyc_String_pa_struct
_tmp141F;struct Cyc_String_pa_struct _tmp141E;(_tmp141E.tag=0,((_tmp141E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp141F.tag=0,((_tmp141F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1420[0]=& _tmp141F,((_tmp1420[1]=& _tmp141E,Cyc_Tcutil_terr(loc,((
_tmp1421="bad attribute %s on member %s",_tag_dyneither(_tmp1421,sizeof(char),30))),
_tag_dyneither(_tmp1420,sizeof(void*),2)))))))))))));}_LL557:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpAE7=t;struct
Cyc_Absyn_Typedefdecl*_tmpAE9;void**_tmpAEA;_LL55F: {struct Cyc_Absyn_TypedefType_struct*
_tmpAE8=(struct Cyc_Absyn_TypedefType_struct*)_tmpAE7;if(_tmpAE8->tag != 18)goto
_LL561;else{_tmpAE9=_tmpAE8->f3;_tmpAEA=_tmpAE8->f4;}}_LL560: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpAE9))->tq).real_const  || (_tmpAE9->tq).print_const){if(
declared_const){const char*_tmp1424;void*_tmp1423;(_tmp1423=0,Cyc_Tcutil_warn(loc,((
_tmp1424="extra const",_tag_dyneither(_tmp1424,sizeof(char),12))),_tag_dyneither(
_tmp1423,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpAEA)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpAEA);else{return declared_const;}_LL561:;_LL562: return
declared_const;_LL55E:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpAED=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpAEF;struct Cyc_Absyn_PtrAtts _tmpAF0;void*_tmpAF1;
_LL564: {struct Cyc_Absyn_PointerType_struct*_tmpAEE=(struct Cyc_Absyn_PointerType_struct*)
_tmpAED;if(_tmpAEE->tag != 5)goto _LL566;else{_tmpAEF=_tmpAEE->f1;_tmpAF0=_tmpAEF.ptr_atts;
_tmpAF1=_tmpAF0.rgn;}}_LL565:{void*_tmpAF2=Cyc_Tcutil_compress(_tmpAF1);struct
Cyc_Absyn_Tvar*_tmpAF4;_LL569: {struct Cyc_Absyn_VarType_struct*_tmpAF3=(struct
Cyc_Absyn_VarType_struct*)_tmpAF2;if(_tmpAF3->tag != 2)goto _LL56B;else{_tmpAF4=
_tmpAF3->f1;}}_LL56A: return Cyc_Absyn_tvar_cmp(tvar,_tmpAF4)== 0;_LL56B:;_LL56C:
goto _LL568;_LL568:;}goto _LL563;_LL566:;_LL567: goto _LL563;_LL563:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpAF5=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpAF7;
struct Cyc_Absyn_Kind _tmpAF8;enum Cyc_Absyn_KindQual _tmpAF9;enum Cyc_Absyn_AliasQual
_tmpAFA;struct Cyc_Absyn_Kind*_tmpAFC;struct Cyc_Absyn_Kind _tmpAFD;enum Cyc_Absyn_KindQual
_tmpAFE;enum Cyc_Absyn_AliasQual _tmpAFF;_LL56E: {struct Cyc_Absyn_Less_kb_struct*
_tmpAF6=(struct Cyc_Absyn_Less_kb_struct*)_tmpAF5;if(_tmpAF6->tag != 2)goto _LL570;
else{_tmpAF7=_tmpAF6->f2;_tmpAF8=*_tmpAF7;_tmpAF9=_tmpAF8.kind;if(_tmpAF9 != Cyc_Absyn_RgnKind)
goto _LL570;_tmpAFA=_tmpAF8.aliasqual;if(_tmpAFA != Cyc_Absyn_Top)goto _LL570;}}
_LL56F: goto _LL571;_LL570: {struct Cyc_Absyn_Eq_kb_struct*_tmpAFB=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpAF5;if(_tmpAFB->tag != 0)goto _LL572;else{_tmpAFC=_tmpAFB->f1;_tmpAFD=*_tmpAFC;
_tmpAFE=_tmpAFD.kind;if(_tmpAFE != Cyc_Absyn_RgnKind)goto _LL572;_tmpAFF=_tmpAFD.aliasqual;
if(_tmpAFF != Cyc_Absyn_Top)goto _LL572;}}_LL571: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL572:;_LL573: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL56D:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpB00=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB03;struct Cyc_Core_Opt**
_tmpB04;struct Cyc_Core_Opt*_tmpB05;struct Cyc_Core_Opt**_tmpB06;struct Cyc_Absyn_Tvar*
_tmpB08;struct Cyc_Absyn_TypeDecl*_tmpB0A;struct Cyc_Absyn_TypeDecl _tmpB0B;void*
_tmpB0C;void**_tmpB0D;void***_tmpB0E;struct Cyc_List_List*_tmpB10;struct _tuple2*
_tmpB12;struct Cyc_Absyn_Enumdecl*_tmpB13;struct Cyc_Absyn_Enumdecl**_tmpB14;
struct Cyc_Absyn_DatatypeInfo _tmpB16;union Cyc_Absyn_DatatypeInfoU _tmpB17;union Cyc_Absyn_DatatypeInfoU*
_tmpB18;struct Cyc_List_List*_tmpB19;struct Cyc_List_List**_tmpB1A;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB1C;union Cyc_Absyn_DatatypeFieldInfoU _tmpB1D;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB1E;struct Cyc_List_List*_tmpB1F;struct Cyc_Absyn_PtrInfo _tmpB21;void*_tmpB22;
struct Cyc_Absyn_Tqual _tmpB23;struct Cyc_Absyn_Tqual*_tmpB24;struct Cyc_Absyn_PtrAtts
_tmpB25;void*_tmpB26;union Cyc_Absyn_Constraint*_tmpB27;union Cyc_Absyn_Constraint*
_tmpB28;union Cyc_Absyn_Constraint*_tmpB29;void*_tmpB2B;struct Cyc_Absyn_Exp*
_tmpB2D;struct Cyc_Absyn_ArrayInfo _tmpB32;void*_tmpB33;struct Cyc_Absyn_Tqual
_tmpB34;struct Cyc_Absyn_Tqual*_tmpB35;struct Cyc_Absyn_Exp*_tmpB36;struct Cyc_Absyn_Exp**
_tmpB37;union Cyc_Absyn_Constraint*_tmpB38;struct Cyc_Position_Segment*_tmpB39;
struct Cyc_Absyn_FnInfo _tmpB3B;struct Cyc_List_List*_tmpB3C;struct Cyc_List_List**
_tmpB3D;struct Cyc_Core_Opt*_tmpB3E;struct Cyc_Core_Opt**_tmpB3F;void*_tmpB40;
struct Cyc_List_List*_tmpB41;int _tmpB42;struct Cyc_Absyn_VarargInfo*_tmpB43;struct
Cyc_List_List*_tmpB44;struct Cyc_List_List*_tmpB45;struct Cyc_List_List*_tmpB47;
enum Cyc_Absyn_AggrKind _tmpB49;struct Cyc_List_List*_tmpB4A;struct Cyc_Absyn_AggrInfo
_tmpB4C;union Cyc_Absyn_AggrInfoU _tmpB4D;union Cyc_Absyn_AggrInfoU*_tmpB4E;struct
Cyc_List_List*_tmpB4F;struct Cyc_List_List**_tmpB50;struct _tuple2*_tmpB52;struct
Cyc_List_List*_tmpB53;struct Cyc_List_List**_tmpB54;struct Cyc_Absyn_Typedefdecl*
_tmpB55;struct Cyc_Absyn_Typedefdecl**_tmpB56;void**_tmpB57;void***_tmpB58;void*
_tmpB5C;void*_tmpB5E;void*_tmpB5F;void*_tmpB61;void*_tmpB63;struct Cyc_List_List*
_tmpB65;_LL575: {struct Cyc_Absyn_VoidType_struct*_tmpB01=(struct Cyc_Absyn_VoidType_struct*)
_tmpB00;if(_tmpB01->tag != 0)goto _LL577;}_LL576: goto _LL574;_LL577: {struct Cyc_Absyn_Evar_struct*
_tmpB02=(struct Cyc_Absyn_Evar_struct*)_tmpB00;if(_tmpB02->tag != 1)goto _LL579;
else{_tmpB03=_tmpB02->f1;_tmpB04=(struct Cyc_Core_Opt**)& _tmpB02->f1;_tmpB05=
_tmpB02->f2;_tmpB06=(struct Cyc_Core_Opt**)& _tmpB02->f2;}}_LL578: if(*_tmpB04 == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmpB04))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpB04))->v,expected_kind))*_tmpB04=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB06=(
struct Cyc_Core_Opt*)& urgn;else{*_tmpB06=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp1427;struct Cyc_Absyn_Less_kb_struct*
_tmp1426;struct Cyc_Absyn_Tvar*_tmpB66=Cyc_Tcutil_new_tvar((void*)((_tmp1426=
_cycalloc(sizeof(*_tmp1426)),((_tmp1426[0]=((_tmp1427.tag=2,((_tmp1427.f1=0,((
_tmp1427.f2=expected_kind,_tmp1427)))))),_tmp1426)))));{struct Cyc_Absyn_VarType_struct*
_tmp142D;struct Cyc_Absyn_VarType_struct _tmp142C;struct Cyc_Core_Opt*_tmp142B;*
_tmpB06=((_tmp142B=_cycalloc(sizeof(*_tmp142B)),((_tmp142B->v=(void*)((_tmp142D=
_cycalloc(sizeof(*_tmp142D)),((_tmp142D[0]=((_tmp142C.tag=2,((_tmp142C.f1=
_tmpB66,_tmp142C)))),_tmp142D)))),_tmp142B))));}_tmpB08=_tmpB66;goto _LL57A;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL574;_LL579: {struct Cyc_Absyn_VarType_struct*_tmpB07=(
struct Cyc_Absyn_VarType_struct*)_tmpB00;if(_tmpB07->tag != 2)goto _LL57B;else{
_tmpB08=_tmpB07->f1;}}_LL57A:{void*_tmpB6C=Cyc_Absyn_compress_kb(_tmpB08->kind);
struct Cyc_Core_Opt*_tmpB6E;struct Cyc_Core_Opt**_tmpB6F;_LL5AC: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB6D=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB6C;if(_tmpB6D->tag != 1)goto
_LL5AE;else{_tmpB6E=_tmpB6D->f1;_tmpB6F=(struct Cyc_Core_Opt**)& _tmpB6D->f1;}}
_LL5AD:{struct Cyc_Absyn_Less_kb_struct*_tmp1433;struct Cyc_Absyn_Less_kb_struct
_tmp1432;struct Cyc_Core_Opt*_tmp1431;*_tmpB6F=((_tmp1431=_cycalloc(sizeof(*
_tmp1431)),((_tmp1431->v=(void*)((_tmp1433=_cycalloc(sizeof(*_tmp1433)),((
_tmp1433[0]=((_tmp1432.tag=2,((_tmp1432.f1=0,((_tmp1432.f2=expected_kind,
_tmp1432)))))),_tmp1433)))),_tmp1431))));}goto _LL5AB;_LL5AE:;_LL5AF: goto _LL5AB;
_LL5AB:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB08);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB08,put_in_effect);{void*_tmpB73=Cyc_Absyn_compress_kb(_tmpB08->kind);struct
Cyc_Core_Opt*_tmpB75;struct Cyc_Core_Opt**_tmpB76;struct Cyc_Absyn_Kind*_tmpB77;
_LL5B1: {struct Cyc_Absyn_Less_kb_struct*_tmpB74=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB73;if(_tmpB74->tag != 2)goto _LL5B3;else{_tmpB75=_tmpB74->f1;_tmpB76=(struct
Cyc_Core_Opt**)& _tmpB74->f1;_tmpB77=_tmpB74->f2;}}_LL5B2: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB77)){struct Cyc_Absyn_Less_kb_struct*_tmp1439;struct Cyc_Absyn_Less_kb_struct
_tmp1438;struct Cyc_Core_Opt*_tmp1437;*_tmpB76=((_tmp1437=_cycalloc(sizeof(*
_tmp1437)),((_tmp1437->v=(void*)((_tmp1439=_cycalloc(sizeof(*_tmp1439)),((
_tmp1439[0]=((_tmp1438.tag=2,((_tmp1438.f1=0,((_tmp1438.f2=expected_kind,
_tmp1438)))))),_tmp1439)))),_tmp1437))));}goto _LL5B0;_LL5B3:;_LL5B4: goto _LL5B0;
_LL5B0:;}goto _LL574;_LL57B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB09=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB00;if(_tmpB09->tag != 26)goto _LL57D;else{
_tmpB0A=_tmpB09->f1;_tmpB0B=*_tmpB0A;_tmpB0C=_tmpB0B.r;_tmpB0D=_tmpB09->f2;
_tmpB0E=(void***)& _tmpB09->f2;}}_LL57C: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpB7B=_tmpB0C;struct Cyc_Absyn_Aggrdecl*_tmpB7D;struct Cyc_Absyn_Enumdecl*
_tmpB7F;struct Cyc_Absyn_Datatypedecl*_tmpB81;_LL5B6: {struct Cyc_Absyn_Aggr_td_struct*
_tmpB7C=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB7B;if(_tmpB7C->tag != 0)goto _LL5B8;
else{_tmpB7D=_tmpB7C->f1;}}_LL5B7: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB7D);goto _LL5B5;_LL5B8: {struct Cyc_Absyn_Enum_td_struct*_tmpB7E=(struct Cyc_Absyn_Enum_td_struct*)
_tmpB7B;if(_tmpB7E->tag != 1)goto _LL5BA;else{_tmpB7F=_tmpB7E->f1;}}_LL5B9: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB7F);goto _LL5B5;_LL5BA: {struct Cyc_Absyn_Datatype_td_struct*
_tmpB80=(struct Cyc_Absyn_Datatype_td_struct*)_tmpB7B;if(_tmpB80->tag != 2)goto
_LL5B5;else{_tmpB81=_tmpB80->f1;}}_LL5BB: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB81);goto _LL5B5;_LL5B5:;}{void**_tmp143A;*_tmpB0E=((_tmp143A=_cycalloc(
sizeof(*_tmp143A)),((_tmp143A[0]=new_t,_tmp143A))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL57D: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB0F=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB00;if(_tmpB0F->tag != 15)goto
_LL57F;else{_tmpB10=_tmpB0F->f1;}}_LL57E: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB83=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpB10 != 0;_tmpB10=_tmpB10->tl){
struct Cyc_Absyn_Enumfield*_tmpB84=(struct Cyc_Absyn_Enumfield*)_tmpB10->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB84->name).f2)){
const char*_tmp143E;void*_tmp143D[1];struct Cyc_String_pa_struct _tmp143C;(_tmp143C.tag=
0,((_tmp143C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB84->name).f2),((
_tmp143D[0]=& _tmp143C,Cyc_Tcutil_terr(_tmpB84->loc,((_tmp143E="duplicate enum field name %s",
_tag_dyneither(_tmp143E,sizeof(char),29))),_tag_dyneither(_tmp143D,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp143F;prev_fields=((_tmp143F=
_region_malloc(_tmpB83,sizeof(*_tmp143F)),((_tmp143F->hd=(*_tmpB84->name).f2,((
_tmp143F->tl=prev_fields,_tmp143F))))));}if(_tmpB84->tag == 0)_tmpB84->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB84->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB84->tag))){const char*_tmp1443;void*
_tmp1442[1];struct Cyc_String_pa_struct _tmp1441;(_tmp1441.tag=0,((_tmp1441.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB84->name).f2),((_tmp1442[0]=&
_tmp1441,Cyc_Tcutil_terr(loc,((_tmp1443="enum field %s: expression is not constant",
_tag_dyneither(_tmp1443,sizeof(char),42))),_tag_dyneither(_tmp1442,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB84->tag))).f1;tag_count=t1 + 1;(*_tmpB84->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp1449;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp1448;struct _tuple30*_tmp1447;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple30*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB84->name).f2,(struct _tuple30*)((_tmp1447=_cycalloc(sizeof(*_tmp1447)),((
_tmp1447->f1=(void*)((_tmp1449=_cycalloc(sizeof(*_tmp1449)),((_tmp1449[0]=((
_tmp1448.tag=4,((_tmp1448.f1=(void*)t,((_tmp1448.f2=_tmpB84,_tmp1448)))))),
_tmp1449)))),((_tmp1447->f2=1,_tmp1447)))))));};};}}goto _LL574;}_LL57F: {struct
Cyc_Absyn_EnumType_struct*_tmpB11=(struct Cyc_Absyn_EnumType_struct*)_tmpB00;if(
_tmpB11->tag != 14)goto _LL581;else{_tmpB12=_tmpB11->f1;_tmpB13=_tmpB11->f2;
_tmpB14=(struct Cyc_Absyn_Enumdecl**)& _tmpB11->f2;}}_LL580: if(*_tmpB14 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB14))->fields == 0){struct
_handler_cons _tmpB8F;_push_handler(& _tmpB8F);{int _tmpB91=0;if(setjmp(_tmpB8F.handler))
_tmpB91=1;if(!_tmpB91){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB12);*_tmpB14=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB90=(void*)_exn_thrown;void*_tmpB93=_tmpB90;_LL5BD: {struct Cyc_Dict_Absent_struct*
_tmpB94=(struct Cyc_Dict_Absent_struct*)_tmpB93;if(_tmpB94->tag != Cyc_Dict_Absent)
goto _LL5BF;}_LL5BE: {struct Cyc_Tcenv_Genv*_tmpB95=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp144A;struct Cyc_Absyn_Enumdecl*_tmpB96=(_tmp144A=
_cycalloc(sizeof(*_tmp144A)),((_tmp144A->sc=Cyc_Absyn_Extern,((_tmp144A->name=
_tmpB12,((_tmp144A->fields=0,_tmp144A)))))));Cyc_Tc_tcEnumdecl(te,_tmpB95,loc,
_tmpB96);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB12);*
_tmpB14=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5BC;};}_LL5BF:;_LL5C0:(void)_throw(
_tmpB93);_LL5BC:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB14);*_tmpB12=(ed->name)[0];goto _LL574;};_LL581: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB15=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB00;if(_tmpB15->tag != 3)goto
_LL583;else{_tmpB16=_tmpB15->f1;_tmpB17=_tmpB16.datatype_info;_tmpB18=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB15->f1).datatype_info;_tmpB19=_tmpB16.targs;_tmpB1A=(struct Cyc_List_List**)&(
_tmpB15->f1).targs;}}_LL582: {struct Cyc_List_List*_tmpB98=*_tmpB1A;{union Cyc_Absyn_DatatypeInfoU
_tmpB99=*_tmpB18;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB9A;struct _tuple2*
_tmpB9B;int _tmpB9C;struct Cyc_Absyn_Datatypedecl**_tmpB9D;struct Cyc_Absyn_Datatypedecl*
_tmpB9E;_LL5C2: if((_tmpB99.UnknownDatatype).tag != 1)goto _LL5C4;_tmpB9A=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpB99.UnknownDatatype).val;_tmpB9B=_tmpB9A.name;
_tmpB9C=_tmpB9A.is_extensible;_LL5C3: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpB9F;_push_handler(& _tmpB9F);{int _tmpBA1=0;if(setjmp(
_tmpB9F.handler))_tmpBA1=1;if(!_tmpBA1){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpB9B);;_pop_handler();}else{void*_tmpBA0=(void*)_exn_thrown;void*_tmpBA3=
_tmpBA0;_LL5C7: {struct Cyc_Dict_Absent_struct*_tmpBA4=(struct Cyc_Dict_Absent_struct*)
_tmpBA3;if(_tmpBA4->tag != Cyc_Dict_Absent)goto _LL5C9;}_LL5C8: {struct Cyc_Tcenv_Genv*
_tmpBA5=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp144B;struct Cyc_Absyn_Datatypedecl*
_tmpBA6=(_tmp144B=_cycalloc(sizeof(*_tmp144B)),((_tmp144B->sc=Cyc_Absyn_Extern,((
_tmp144B->name=_tmpB9B,((_tmp144B->tvs=0,((_tmp144B->fields=0,((_tmp144B->is_extensible=
_tmpB9C,_tmp144B)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBA5,loc,_tmpBA6);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpB9B);if(_tmpB98 != 0){{const char*
_tmp144F;void*_tmp144E[1];struct Cyc_String_pa_struct _tmp144D;(_tmp144D.tag=0,((
_tmp144D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB9B)),((_tmp144E[0]=& _tmp144D,Cyc_Tcutil_terr(loc,((_tmp144F="declare parameterized datatype %s before using",
_tag_dyneither(_tmp144F,sizeof(char),47))),_tag_dyneither(_tmp144E,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5C6;}_LL5C9:;_LL5CA:(void)_throw(_tmpBA3);_LL5C6:;}};}
if(_tmpB9C  && !(*tudp)->is_extensible){const char*_tmp1453;void*_tmp1452[1];
struct Cyc_String_pa_struct _tmp1451;(_tmp1451.tag=0,((_tmp1451.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB9B)),((
_tmp1452[0]=& _tmp1451,Cyc_Tcutil_terr(loc,((_tmp1453="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1453,sizeof(char),41))),_tag_dyneither(_tmp1452,sizeof(void*),
1)))))));}*_tmpB18=Cyc_Absyn_KnownDatatype(tudp);_tmpB9E=*tudp;goto _LL5C5;}
_LL5C4: if((_tmpB99.KnownDatatype).tag != 2)goto _LL5C1;_tmpB9D=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB99.KnownDatatype).val;_tmpB9E=*_tmpB9D;_LL5C5: {struct Cyc_List_List*tvs=
_tmpB9E->tvs;for(0;_tmpB98 != 0  && tvs != 0;(_tmpB98=_tmpB98->tl,tvs=tvs->tl)){
void*t=(void*)_tmpB98->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1454;struct _tuple0 _tmpBAF=(_tmp1454.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1454.f2=t,_tmp1454)));void*_tmpBB0;void*_tmpBB2;struct Cyc_Absyn_Tvar*_tmpBB4;
_LL5CC: _tmpBB0=_tmpBAF.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBB1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBB0;if(_tmpBB1->tag != 1)goto _LL5CE;};_tmpBB2=_tmpBAF.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBB3=(struct Cyc_Absyn_VarType_struct*)_tmpBB2;if(_tmpBB3->tag != 2)goto _LL5CE;
else{_tmpBB4=_tmpBB3->f1;}};_LL5CD: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBB4);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBB4,1);continue;_LL5CE:;_LL5CF: goto _LL5CB;_LL5CB:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpB98 != 0){const char*_tmp1458;void*_tmp1457[1];
struct Cyc_String_pa_struct _tmp1456;(_tmp1456.tag=0,((_tmp1456.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB9E->name)),((
_tmp1457[0]=& _tmp1456,Cyc_Tcutil_terr(loc,((_tmp1458="too many type arguments for datatype %s",
_tag_dyneither(_tmp1458,sizeof(char),40))),_tag_dyneither(_tmp1457,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1459;hidden_ts=((_tmp1459=_cycalloc(sizeof(*_tmp1459)),((_tmp1459->hd=e,((
_tmp1459->tl=hidden_ts,_tmp1459))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB1A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB1A,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5C1;}_LL5C1:;}goto
_LL574;}_LL583: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB1B=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB00;if(_tmpB1B->tag != 4)goto _LL585;else{_tmpB1C=_tmpB1B->f1;_tmpB1D=_tmpB1C.field_info;
_tmpB1E=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB1B->f1).field_info;_tmpB1F=
_tmpB1C.targs;}}_LL584:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBB9=*_tmpB1E;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBBA;struct _tuple2*_tmpBBB;struct _tuple2*
_tmpBBC;int _tmpBBD;struct _tuple3 _tmpBBE;struct Cyc_Absyn_Datatypedecl*_tmpBBF;
struct Cyc_Absyn_Datatypefield*_tmpBC0;_LL5D1: if((_tmpBB9.UnknownDatatypefield).tag
!= 1)goto _LL5D3;_tmpBBA=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBB9.UnknownDatatypefield).val;
_tmpBBB=_tmpBBA.datatype_name;_tmpBBC=_tmpBBA.field_name;_tmpBBD=_tmpBBA.is_extensible;
_LL5D2: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBC1;_push_handler(& _tmpBC1);{int _tmpBC3=0;if(setjmp(
_tmpBC1.handler))_tmpBC3=1;if(!_tmpBC3){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBBB);;_pop_handler();}else{void*_tmpBC2=(void*)_exn_thrown;void*_tmpBC5=
_tmpBC2;_LL5D6: {struct Cyc_Dict_Absent_struct*_tmpBC6=(struct Cyc_Dict_Absent_struct*)
_tmpBC5;if(_tmpBC6->tag != Cyc_Dict_Absent)goto _LL5D8;}_LL5D7:{const char*_tmp145D;
void*_tmp145C[1];struct Cyc_String_pa_struct _tmp145B;(_tmp145B.tag=0,((_tmp145B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBB)),((
_tmp145C[0]=& _tmp145B,Cyc_Tcutil_terr(loc,((_tmp145D="unbound datatype %s",
_tag_dyneither(_tmp145D,sizeof(char),20))),_tag_dyneither(_tmp145C,sizeof(void*),
1)))))));}return cvtenv;_LL5D8:;_LL5D9:(void)_throw(_tmpBC5);_LL5D5:;}};}{struct
_handler_cons _tmpBCA;_push_handler(& _tmpBCA);{int _tmpBCC=0;if(setjmp(_tmpBCA.handler))
_tmpBCC=1;if(!_tmpBCC){{struct _RegionHandle*_tmpBCD=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBCE=Cyc_Tcenv_lookup_ordinary(_tmpBCD,te,loc,_tmpBBC);struct Cyc_Absyn_Datatypedecl*
_tmpBD0;struct Cyc_Absyn_Datatypefield*_tmpBD1;_LL5DB: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBCF=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBCE;if(_tmpBCF->tag != 2)goto
_LL5DD;else{_tmpBD0=_tmpBCF->f1;_tmpBD1=_tmpBCF->f2;}}_LL5DC: tuf=_tmpBD1;tud=
_tmpBD0;if(_tmpBBD  && !tud->is_extensible){const char*_tmp1461;void*_tmp1460[1];
struct Cyc_String_pa_struct _tmp145F;(_tmp145F.tag=0,((_tmp145F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBB)),((
_tmp1460[0]=& _tmp145F,Cyc_Tcutil_terr(loc,((_tmp1461="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1461,sizeof(char),41))),_tag_dyneither(_tmp1460,sizeof(void*),
1)))))));}goto _LL5DA;_LL5DD:;_LL5DE:{const char*_tmp1466;void*_tmp1465[2];struct
Cyc_String_pa_struct _tmp1464;struct Cyc_String_pa_struct _tmp1463;(_tmp1463.tag=0,((
_tmp1463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBBB)),((_tmp1464.tag=0,((_tmp1464.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBC)),((_tmp1465[0]=& _tmp1464,((
_tmp1465[1]=& _tmp1463,Cyc_Tcutil_terr(loc,((_tmp1466="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1466,sizeof(char),37))),_tag_dyneither(_tmp1465,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBD9=cvtenv;_npop_handler(0);return
_tmpBD9;};_LL5DA:;};_pop_handler();}else{void*_tmpBCB=(void*)_exn_thrown;void*
_tmpBDB=_tmpBCB;_LL5E0: {struct Cyc_Dict_Absent_struct*_tmpBDC=(struct Cyc_Dict_Absent_struct*)
_tmpBDB;if(_tmpBDC->tag != Cyc_Dict_Absent)goto _LL5E2;}_LL5E1:{const char*_tmp146B;
void*_tmp146A[2];struct Cyc_String_pa_struct _tmp1469;struct Cyc_String_pa_struct
_tmp1468;(_tmp1468.tag=0,((_tmp1468.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBB)),((_tmp1469.tag=0,((_tmp1469.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBC)),((
_tmp146A[0]=& _tmp1469,((_tmp146A[1]=& _tmp1468,Cyc_Tcutil_terr(loc,((_tmp146B="unbound field %s in type datatype %s",
_tag_dyneither(_tmp146B,sizeof(char),37))),_tag_dyneither(_tmp146A,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5E2:;_LL5E3:(void)_throw(_tmpBDB);_LL5DF:;}};}*
_tmpB1E=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBBF=tud;_tmpBC0=tuf;goto _LL5D4;}
_LL5D3: if((_tmpBB9.KnownDatatypefield).tag != 2)goto _LL5D0;_tmpBBE=(struct _tuple3)(
_tmpBB9.KnownDatatypefield).val;_tmpBBF=_tmpBBE.f1;_tmpBC0=_tmpBBE.f2;_LL5D4: {
struct Cyc_List_List*tvs=_tmpBBF->tvs;for(0;_tmpB1F != 0  && tvs != 0;(_tmpB1F=
_tmpB1F->tl,tvs=tvs->tl)){void*t=(void*)_tmpB1F->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp146C;struct _tuple0 _tmpBE2=(
_tmp146C.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp146C.f2=t,_tmp146C)));void*
_tmpBE3;void*_tmpBE5;struct Cyc_Absyn_Tvar*_tmpBE7;_LL5E5: _tmpBE3=_tmpBE2.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpBE4=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBE3;if(_tmpBE4->tag != 1)goto _LL5E7;};_tmpBE5=_tmpBE2.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBE6=(struct Cyc_Absyn_VarType_struct*)_tmpBE5;if(_tmpBE6->tag != 2)goto _LL5E7;
else{_tmpBE7=_tmpBE6->f1;}};_LL5E6: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBE7);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBE7,1);continue;_LL5E7:;_LL5E8: goto _LL5E4;_LL5E4:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpB1F != 0){const char*_tmp1471;void*_tmp1470[2];
struct Cyc_String_pa_struct _tmp146F;struct Cyc_String_pa_struct _tmp146E;(_tmp146E.tag=
0,((_tmp146E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC0->name)),((_tmp146F.tag=0,((_tmp146F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBF->name)),((_tmp1470[0]=& _tmp146F,((
_tmp1470[1]=& _tmp146E,Cyc_Tcutil_terr(loc,((_tmp1471="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp1471,sizeof(char),43))),_tag_dyneither(_tmp1470,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp1476;void*_tmp1475[2];struct Cyc_String_pa_struct
_tmp1474;struct Cyc_String_pa_struct _tmp1473;(_tmp1473.tag=0,((_tmp1473.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBC0->name)),((
_tmp1474.tag=0,((_tmp1474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBBF->name)),((_tmp1475[0]=& _tmp1474,((_tmp1475[1]=& _tmp1473,Cyc_Tcutil_terr(
loc,((_tmp1476="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp1476,sizeof(char),42))),_tag_dyneither(_tmp1475,sizeof(void*),2)))))))))))));}
goto _LL5D0;}_LL5D0:;}goto _LL574;_LL585: {struct Cyc_Absyn_PointerType_struct*
_tmpB20=(struct Cyc_Absyn_PointerType_struct*)_tmpB00;if(_tmpB20->tag != 5)goto
_LL587;else{_tmpB21=_tmpB20->f1;_tmpB22=_tmpB21.elt_typ;_tmpB23=_tmpB21.elt_tq;
_tmpB24=(struct Cyc_Absyn_Tqual*)&(_tmpB20->f1).elt_tq;_tmpB25=_tmpB21.ptr_atts;
_tmpB26=_tmpB25.rgn;_tmpB27=_tmpB25.nullable;_tmpB28=_tmpB25.bounds;_tmpB29=
_tmpB25.zero_term;}}_LL586: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB22,1);_tmpB24->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB24->print_const,_tmpB22);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5E9: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5EA: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5EB: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB26,1);{union Cyc_Absyn_Constraint*
_tmpBF0=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB29);union Cyc_Absyn_Constraint _tmpBF1;int _tmpBF2;union Cyc_Absyn_Constraint
_tmpBF3;int _tmpBF4;_LL5EE: _tmpBF1=*_tmpBF0;if((_tmpBF1.No_constr).tag != 3)goto
_LL5F0;_tmpBF2=(int)(_tmpBF1.No_constr).val;_LL5EF:{void*_tmpBF5=Cyc_Tcutil_compress(
_tmpB22);enum Cyc_Absyn_Size_of _tmpBF7;_LL5F5: {struct Cyc_Absyn_IntType_struct*
_tmpBF6=(struct Cyc_Absyn_IntType_struct*)_tmpBF5;if(_tmpBF6->tag != 6)goto _LL5F7;
else{_tmpBF7=_tmpBF6->f2;if(_tmpBF7 != Cyc_Absyn_Char_sz)goto _LL5F7;}}_LL5F6:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB29,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5F4;_LL5F7:;_LL5F8:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB29,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5F4;
_LL5F4:;}goto _LL5ED;_LL5F0: _tmpBF3=*_tmpBF0;if((_tmpBF3.Eq_constr).tag != 1)goto
_LL5F2;_tmpBF4=(int)(_tmpBF3.Eq_constr).val;if(_tmpBF4 != 1)goto _LL5F2;_LL5F1: if(
!Cyc_Tcutil_admits_zero(_tmpB22)){const char*_tmp147A;void*_tmp1479[1];struct Cyc_String_pa_struct
_tmp1478;(_tmp1478.tag=0,((_tmp1478.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB22)),((_tmp1479[0]=& _tmp1478,Cyc_Tcutil_terr(
loc,((_tmp147A="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp147A,sizeof(char),53))),_tag_dyneither(_tmp1479,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5ED;_LL5F2:;_LL5F3: is_zero_terminated=0;
goto _LL5ED;_LL5ED:;}{void*_tmpBFB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB28);struct Cyc_Absyn_Exp*
_tmpBFE;_LL5FA: {struct Cyc_Absyn_DynEither_b_struct*_tmpBFC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBFB;if(_tmpBFC->tag != 0)goto _LL5FC;}_LL5FB: goto _LL5F9;_LL5FC: {struct Cyc_Absyn_Upper_b_struct*
_tmpBFD=(struct Cyc_Absyn_Upper_b_struct*)_tmpBFB;if(_tmpBFD->tag != 1)goto _LL5F9;
else{_tmpBFE=_tmpBFD->f1;}}_LL5FD: {struct _RegionHandle*_tmpBFF=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC00=Cyc_Tcenv_allow_valueof(_tmpBFF,te);Cyc_Tcexp_tcExp(
_tmpC00,0,_tmpBFE);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBFE,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpBFE)){const char*_tmp147D;void*
_tmp147C;(_tmp147C=0,Cyc_Tcutil_terr(loc,((_tmp147D="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp147D,sizeof(char),49))),_tag_dyneither(_tmp147C,sizeof(void*),
0)));}{unsigned int _tmpC04;int _tmpC05;struct _tuple13 _tmpC03=Cyc_Evexp_eval_const_uint_exp(
_tmpBFE);_tmpC04=_tmpC03.f1;_tmpC05=_tmpC03.f2;if(is_zero_terminated  && (!
_tmpC05  || _tmpC04 < 1)){const char*_tmp1480;void*_tmp147F;(_tmp147F=0,Cyc_Tcutil_terr(
loc,((_tmp1480="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp1480,sizeof(char),55))),_tag_dyneither(_tmp147F,sizeof(void*),
0)));}goto _LL5F9;};}_LL5F9:;}goto _LL574;};}_LL587: {struct Cyc_Absyn_TagType_struct*
_tmpB2A=(struct Cyc_Absyn_TagType_struct*)_tmpB00;if(_tmpB2A->tag != 20)goto _LL589;
else{_tmpB2B=(void*)_tmpB2A->f1;}}_LL588: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB2B,1);goto _LL574;_LL589: {struct Cyc_Absyn_ValueofType_struct*
_tmpB2C=(struct Cyc_Absyn_ValueofType_struct*)_tmpB00;if(_tmpB2C->tag != 19)goto
_LL58B;else{_tmpB2D=_tmpB2C->f1;}}_LL58A: {struct _RegionHandle*_tmpC08=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC09=Cyc_Tcenv_allow_valueof(_tmpC08,te);Cyc_Tcexp_tcExp(
_tmpC09,0,_tmpB2D);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB2D)){const char*
_tmp1483;void*_tmp1482;(_tmp1482=0,Cyc_Tcutil_terr(loc,((_tmp1483="valueof_t requires an int expression",
_tag_dyneither(_tmp1483,sizeof(char),37))),_tag_dyneither(_tmp1482,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB2D,te,cvtenv);goto
_LL574;}_LL58B: {struct Cyc_Absyn_IntType_struct*_tmpB2E=(struct Cyc_Absyn_IntType_struct*)
_tmpB00;if(_tmpB2E->tag != 6)goto _LL58D;}_LL58C: goto _LL58E;_LL58D: {struct Cyc_Absyn_FloatType_struct*
_tmpB2F=(struct Cyc_Absyn_FloatType_struct*)_tmpB00;if(_tmpB2F->tag != 7)goto
_LL58F;}_LL58E: goto _LL590;_LL58F: {struct Cyc_Absyn_DoubleType_struct*_tmpB30=(
struct Cyc_Absyn_DoubleType_struct*)_tmpB00;if(_tmpB30->tag != 8)goto _LL591;}
_LL590: goto _LL574;_LL591: {struct Cyc_Absyn_ArrayType_struct*_tmpB31=(struct Cyc_Absyn_ArrayType_struct*)
_tmpB00;if(_tmpB31->tag != 9)goto _LL593;else{_tmpB32=_tmpB31->f1;_tmpB33=_tmpB32.elt_type;
_tmpB34=_tmpB32.tq;_tmpB35=(struct Cyc_Absyn_Tqual*)&(_tmpB31->f1).tq;_tmpB36=
_tmpB32.num_elts;_tmpB37=(struct Cyc_Absyn_Exp**)&(_tmpB31->f1).num_elts;_tmpB38=
_tmpB32.zero_term;_tmpB39=_tmpB32.zt_loc;}}_LL592: {struct Cyc_Absyn_Exp*_tmpC0C=*
_tmpB37;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpB33,1);_tmpB35->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB35->print_const,
_tmpB33);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC0D=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB38);union Cyc_Absyn_Constraint
_tmpC0E;int _tmpC0F;union Cyc_Absyn_Constraint _tmpC10;int _tmpC11;_LL5FF: _tmpC0E=*
_tmpC0D;if((_tmpC0E.No_constr).tag != 3)goto _LL601;_tmpC0F=(int)(_tmpC0E.No_constr).val;
_LL600:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB38,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5FE;_LL601: _tmpC10=*_tmpC0D;if((_tmpC10.Eq_constr).tag
!= 1)goto _LL603;_tmpC11=(int)(_tmpC10.Eq_constr).val;if(_tmpC11 != 1)goto _LL603;
_LL602: if(!Cyc_Tcutil_admits_zero(_tmpB33)){const char*_tmp1487;void*_tmp1486[1];
struct Cyc_String_pa_struct _tmp1485;(_tmp1485.tag=0,((_tmp1485.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB33)),((
_tmp1486[0]=& _tmp1485,Cyc_Tcutil_terr(loc,((_tmp1487="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp1487,sizeof(char),51))),_tag_dyneither(_tmp1486,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5FE;_LL603:;_LL604: is_zero_terminated=0;
goto _LL5FE;_LL5FE:;}if(_tmpC0C == 0){if(is_zero_terminated)*_tmpB37=(_tmpC0C=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp148A;void*
_tmp1489;(_tmp1489=0,Cyc_Tcutil_warn(loc,((_tmp148A="array bound defaults to 1 here",
_tag_dyneither(_tmp148A,sizeof(char),31))),_tag_dyneither(_tmp1489,sizeof(void*),
0)));}*_tmpB37=(_tmpC0C=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC0C);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC0C)){const char*_tmp148D;void*_tmp148C;(_tmp148C=0,Cyc_Tcutil_terr(loc,((
_tmp148D="array bounds expression is not constant",_tag_dyneither(_tmp148D,
sizeof(char),40))),_tag_dyneither(_tmp148C,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC0C)){const char*_tmp1490;void*_tmp148F;(_tmp148F=0,
Cyc_Tcutil_terr(loc,((_tmp1490="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp1490,sizeof(char),47))),_tag_dyneither(_tmp148F,sizeof(void*),
0)));}{unsigned int _tmpC1C;int _tmpC1D;struct _tuple13 _tmpC1B=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC0C);_tmpC1C=_tmpC1B.f1;_tmpC1D=_tmpC1B.f2;if((
is_zero_terminated  && _tmpC1D) && _tmpC1C < 1){const char*_tmp1493;void*_tmp1492;(
_tmp1492=0,Cyc_Tcutil_warn(loc,((_tmp1493="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp1493,sizeof(char),48))),_tag_dyneither(_tmp1492,sizeof(void*),
0)));}if((_tmpC1D  && _tmpC1C < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp1496;void*
_tmp1495;(_tmp1495=0,Cyc_Tcutil_warn(loc,((_tmp1496="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp1496,sizeof(char),75))),_tag_dyneither(_tmp1495,sizeof(void*),
0)));}*_tmpB37=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL574;};};}
_LL593: {struct Cyc_Absyn_FnType_struct*_tmpB3A=(struct Cyc_Absyn_FnType_struct*)
_tmpB00;if(_tmpB3A->tag != 10)goto _LL595;else{_tmpB3B=_tmpB3A->f1;_tmpB3C=_tmpB3B.tvars;
_tmpB3D=(struct Cyc_List_List**)&(_tmpB3A->f1).tvars;_tmpB3E=_tmpB3B.effect;
_tmpB3F=(struct Cyc_Core_Opt**)&(_tmpB3A->f1).effect;_tmpB40=_tmpB3B.ret_typ;
_tmpB41=_tmpB3B.args;_tmpB42=_tmpB3B.c_varargs;_tmpB43=_tmpB3B.cyc_varargs;
_tmpB44=_tmpB3B.rgn_po;_tmpB45=_tmpB3B.attributes;}}_LL594: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB45 != 0;
_tmpB45=_tmpB45->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB45->hd)){const char*
_tmp149A;void*_tmp1499[1];struct Cyc_String_pa_struct _tmp1498;(_tmp1498.tag=0,((
_tmp1498.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB45->hd)),((_tmp1499[0]=& _tmp1498,Cyc_Tcutil_terr(loc,((_tmp149A="bad function type attribute %s",
_tag_dyneither(_tmp149A,sizeof(char),31))),_tag_dyneither(_tmp1499,sizeof(void*),
1)))))));}{void*_tmpC25=(void*)_tmpB45->hd;enum Cyc_Absyn_Format_Type _tmpC2A;int
_tmpC2B;int _tmpC2C;_LL606: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC26=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC25;if(_tmpC26->tag != 1)goto _LL608;}_LL607: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL605;_LL608: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC27=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC25;if(_tmpC27->tag != 2)goto
_LL60A;}_LL609: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL605;_LL60A: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC28=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC25;if(_tmpC28->tag != 3)goto _LL60C;}_LL60B: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL605;_LL60C: {struct Cyc_Absyn_Format_att_struct*_tmpC29=(
struct Cyc_Absyn_Format_att_struct*)_tmpC25;if(_tmpC29->tag != 19)goto _LL60E;else{
_tmpC2A=_tmpC29->f1;_tmpC2B=_tmpC29->f2;_tmpC2C=_tmpC29->f3;}}_LL60D: if(!
seen_format){seen_format=1;ft=_tmpC2A;fmt_desc_arg=_tmpC2B;fmt_arg_start=_tmpC2C;}
else{const char*_tmp149D;void*_tmp149C;(_tmp149C=0,Cyc_Tcutil_terr(loc,((_tmp149D="function can't have multiple format attributes",
_tag_dyneither(_tmp149D,sizeof(char),47))),_tag_dyneither(_tmp149C,sizeof(void*),
0)));}goto _LL605;_LL60E:;_LL60F: goto _LL605;_LL605:;};}if(num_convs > 1){const char*
_tmp14A0;void*_tmp149F;(_tmp149F=0,Cyc_Tcutil_terr(loc,((_tmp14A0="function can't have multiple calling conventions",
_tag_dyneither(_tmp14A0,sizeof(char),49))),_tag_dyneither(_tmp149F,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB3D);{struct Cyc_List_List*b=*
_tmpB3D;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC31=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC33;struct Cyc_Absyn_Kind _tmpC34;enum Cyc_Absyn_KindQual
_tmpC35;_LL611: {struct Cyc_Absyn_Eq_kb_struct*_tmpC32=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC31;if(_tmpC32->tag != 0)goto _LL613;else{_tmpC33=_tmpC32->f1;_tmpC34=*_tmpC33;
_tmpC35=_tmpC34.kind;if(_tmpC35 != Cyc_Absyn_MemKind)goto _LL613;}}_LL612:{const
char*_tmp14A4;void*_tmp14A3[1];struct Cyc_String_pa_struct _tmp14A2;(_tmp14A2.tag=
0,((_tmp14A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14A3[0]=& _tmp14A2,Cyc_Tcutil_terr(loc,((_tmp14A4="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14A4,sizeof(char),51))),_tag_dyneither(_tmp14A3,sizeof(void*),
1)))))));}goto _LL610;_LL613:;_LL614: goto _LL610;_LL610:;};}}{struct _RegionHandle*
_tmpC39=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14A5;struct Cyc_Tcutil_CVTEnv
_tmpC3A=(_tmp14A5.r=_tmpC39,((_tmp14A5.kind_env=cvtenv.kind_env,((_tmp14A5.free_vars=
0,((_tmp14A5.free_evars=0,((_tmp14A5.generalize_evars=cvtenv.generalize_evars,((
_tmp14A5.fn_result=1,_tmp14A5)))))))))));_tmpC3A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC3A,& Cyc_Tcutil_tmk,_tmpB40,1);_tmpC3A.fn_result=0;{struct Cyc_List_List*
a=_tmpB41;for(0;a != 0;a=a->tl){struct _tuple9*_tmpC3B=(struct _tuple9*)a->hd;void*
_tmpC3C=(*_tmpC3B).f3;_tmpC3A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC3A,& Cyc_Tcutil_tmk,
_tmpC3C,1);((*_tmpC3B).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpC3B).f2).print_const,_tmpC3C);{void*_tmpC3D=Cyc_Tcutil_compress(_tmpC3C);
struct Cyc_Absyn_ArrayInfo _tmpC3F;void*_tmpC40;struct Cyc_Absyn_Tqual _tmpC41;
struct Cyc_Absyn_Exp*_tmpC42;union Cyc_Absyn_Constraint*_tmpC43;struct Cyc_Position_Segment*
_tmpC44;_LL616: {struct Cyc_Absyn_ArrayType_struct*_tmpC3E=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC3D;if(_tmpC3E->tag != 9)goto _LL618;else{_tmpC3F=_tmpC3E->f1;_tmpC40=_tmpC3F.elt_type;
_tmpC41=_tmpC3F.tq;_tmpC42=_tmpC3F.num_elts;_tmpC43=_tmpC3F.zero_term;_tmpC44=
_tmpC3F.zt_loc;}}_LL617: {void*_tmpC45=Cyc_Absyn_new_evar(0,0);_tmpC3A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC3A,& Cyc_Tcutil_rk,_tmpC45,1);{struct Cyc_Absyn_Upper_b_struct _tmp14A8;
struct Cyc_Absyn_Upper_b_struct*_tmp14A7;void*_tmpC46=Cyc_Absyn_atb_typ(_tmpC40,
_tmpC45,_tmpC41,(void*)((_tmp14A7=_cycalloc(sizeof(*_tmp14A7)),((_tmp14A7[0]=((
_tmp14A8.tag=1,((_tmp14A8.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC42),_tmp14A8)))),
_tmp14A7)))),_tmpC43);(*_tmpC3B).f3=_tmpC46;goto _LL615;};}_LL618:;_LL619: goto
_LL615;_LL615:;};}}if(_tmpB43 != 0){if(_tmpB42){const char*_tmp14AB;void*_tmp14AA;(
_tmp14AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp14AB="both c_vararg and cyc_vararg",_tag_dyneither(_tmp14AB,sizeof(char),29))),
_tag_dyneither(_tmp14AA,sizeof(void*),0)));}{void*_tmpC4C;int _tmpC4D;struct Cyc_Absyn_VarargInfo
_tmpC4B=*_tmpB43;_tmpC4C=_tmpC4B.type;_tmpC4D=_tmpC4B.inject;_tmpC3A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC3A,& Cyc_Tcutil_tmk,_tmpC4C,1);(_tmpB43->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpB43->tq).print_const,_tmpC4C);if(_tmpC4D){void*_tmpC4E=Cyc_Tcutil_compress(
_tmpC4C);struct Cyc_Absyn_PtrInfo _tmpC50;void*_tmpC51;struct Cyc_Absyn_PtrAtts
_tmpC52;union Cyc_Absyn_Constraint*_tmpC53;union Cyc_Absyn_Constraint*_tmpC54;
_LL61B: {struct Cyc_Absyn_PointerType_struct*_tmpC4F=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4E;if(_tmpC4F->tag != 5)goto _LL61D;else{_tmpC50=_tmpC4F->f1;_tmpC51=_tmpC50.elt_typ;
_tmpC52=_tmpC50.ptr_atts;_tmpC53=_tmpC52.bounds;_tmpC54=_tmpC52.zero_term;}}
_LL61C:{void*_tmpC55=Cyc_Tcutil_compress(_tmpC51);_LL620: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC56=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC55;if(_tmpC56->tag != 3)goto
_LL622;}_LL621: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC54)){const char*_tmp14AE;void*_tmp14AD;(_tmp14AD=0,Cyc_Tcutil_terr(loc,((
_tmp14AE="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14AE,sizeof(
char),37))),_tag_dyneither(_tmp14AD,sizeof(void*),0)));}{void*_tmpC59=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC53);_LL625: {struct Cyc_Absyn_DynEither_b_struct*_tmpC5A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC59;if(_tmpC5A->tag != 0)goto _LL627;}_LL626:{const char*_tmp14B1;void*_tmp14B0;(
_tmp14B0=0,Cyc_Tcutil_terr(loc,((_tmp14B1="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14B1,sizeof(char),44))),_tag_dyneither(_tmp14B0,sizeof(void*),
0)));}goto _LL624;_LL627:;_LL628: goto _LL624;_LL624:;}goto _LL61F;_LL622:;_LL623:{
const char*_tmp14B4;void*_tmp14B3;(_tmp14B3=0,Cyc_Tcutil_terr(loc,((_tmp14B4="can't inject a non-datatype type",
_tag_dyneither(_tmp14B4,sizeof(char),33))),_tag_dyneither(_tmp14B3,sizeof(void*),
0)));}goto _LL61F;_LL61F:;}goto _LL61A;_LL61D:;_LL61E:{const char*_tmp14B7;void*
_tmp14B6;(_tmp14B6=0,Cyc_Tcutil_terr(loc,((_tmp14B7="expecting a datatype pointer type",
_tag_dyneither(_tmp14B7,sizeof(char),34))),_tag_dyneither(_tmp14B6,sizeof(void*),
0)));}goto _LL61A;_LL61A:;}};}if(seen_format){int _tmpC61=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB41);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC61) || 
fmt_arg_start < 0) || _tmpB43 == 0  && fmt_arg_start != 0) || _tmpB43 != 0  && 
fmt_arg_start != _tmpC61 + 1){const char*_tmp14BA;void*_tmp14B9;(_tmp14B9=0,Cyc_Tcutil_terr(
loc,((_tmp14BA="bad format descriptor",_tag_dyneither(_tmp14BA,sizeof(char),22))),
_tag_dyneither(_tmp14B9,sizeof(void*),0)));}else{void*_tmpC65;struct _tuple9
_tmpC64=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB41,
fmt_desc_arg - 1);_tmpC65=_tmpC64.f3;{void*_tmpC66=Cyc_Tcutil_compress(_tmpC65);
struct Cyc_Absyn_PtrInfo _tmpC68;void*_tmpC69;struct Cyc_Absyn_PtrAtts _tmpC6A;union
Cyc_Absyn_Constraint*_tmpC6B;union Cyc_Absyn_Constraint*_tmpC6C;_LL62A: {struct
Cyc_Absyn_PointerType_struct*_tmpC67=(struct Cyc_Absyn_PointerType_struct*)
_tmpC66;if(_tmpC67->tag != 5)goto _LL62C;else{_tmpC68=_tmpC67->f1;_tmpC69=_tmpC68.elt_typ;
_tmpC6A=_tmpC68.ptr_atts;_tmpC6B=_tmpC6A.bounds;_tmpC6C=_tmpC6A.zero_term;}}
_LL62B:{struct _tuple0 _tmp14BB;struct _tuple0 _tmpC6E=(_tmp14BB.f1=Cyc_Tcutil_compress(
_tmpC69),((_tmp14BB.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC6B),_tmp14BB)));void*_tmpC6F;enum Cyc_Absyn_Sign
_tmpC71;enum Cyc_Absyn_Size_of _tmpC72;void*_tmpC73;_LL62F: _tmpC6F=_tmpC6E.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC70=(struct Cyc_Absyn_IntType_struct*)_tmpC6F;
if(_tmpC70->tag != 6)goto _LL631;else{_tmpC71=_tmpC70->f1;if(_tmpC71 != Cyc_Absyn_None)
goto _LL631;_tmpC72=_tmpC70->f2;if(_tmpC72 != Cyc_Absyn_Char_sz)goto _LL631;}};
_tmpC73=_tmpC6E.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC74=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC73;if(_tmpC74->tag != 0)goto _LL631;};_LL630: goto _LL62E;_LL631:;_LL632:{const
char*_tmp14BE;void*_tmp14BD;(_tmp14BD=0,Cyc_Tcutil_terr(loc,((_tmp14BE="format descriptor is not a char ? type",
_tag_dyneither(_tmp14BE,sizeof(char),39))),_tag_dyneither(_tmp14BD,sizeof(void*),
0)));}goto _LL62E;_LL62E:;}goto _LL629;_LL62C:;_LL62D:{const char*_tmp14C1;void*
_tmp14C0;(_tmp14C0=0,Cyc_Tcutil_terr(loc,((_tmp14C1="format descriptor is not a char ? type",
_tag_dyneither(_tmp14C1,sizeof(char),39))),_tag_dyneither(_tmp14C0,sizeof(void*),
0)));}goto _LL629;_LL629:;}if(fmt_arg_start != 0){int problem;{struct _tuple31
_tmp14C2;struct _tuple31 _tmpC7A=(_tmp14C2.f1=ft,((_tmp14C2.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB43))->type)),
_tmp14C2)));enum Cyc_Absyn_Format_Type _tmpC7B;void*_tmpC7C;struct Cyc_Absyn_DatatypeInfo
_tmpC7E;union Cyc_Absyn_DatatypeInfoU _tmpC7F;struct Cyc_Absyn_Datatypedecl**
_tmpC80;struct Cyc_Absyn_Datatypedecl*_tmpC81;enum Cyc_Absyn_Format_Type _tmpC82;
void*_tmpC83;struct Cyc_Absyn_DatatypeInfo _tmpC85;union Cyc_Absyn_DatatypeInfoU
_tmpC86;struct Cyc_Absyn_Datatypedecl**_tmpC87;struct Cyc_Absyn_Datatypedecl*
_tmpC88;_LL634: _tmpC7B=_tmpC7A.f1;if(_tmpC7B != Cyc_Absyn_Printf_ft)goto _LL636;
_tmpC7C=_tmpC7A.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC7D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC7C;if(_tmpC7D->tag != 3)goto _LL636;else{_tmpC7E=_tmpC7D->f1;_tmpC7F=_tmpC7E.datatype_info;
if((_tmpC7F.KnownDatatype).tag != 2)goto _LL636;_tmpC80=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC7F.KnownDatatype).val;_tmpC81=*_tmpC80;}};_LL635: problem=Cyc_Absyn_qvar_cmp(
_tmpC81->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL633;_LL636: _tmpC82=
_tmpC7A.f1;if(_tmpC82 != Cyc_Absyn_Scanf_ft)goto _LL638;_tmpC83=_tmpC7A.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC84=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC83;if(_tmpC84->tag != 3)goto _LL638;else{_tmpC85=_tmpC84->f1;_tmpC86=_tmpC85.datatype_info;
if((_tmpC86.KnownDatatype).tag != 2)goto _LL638;_tmpC87=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC86.KnownDatatype).val;_tmpC88=*_tmpC87;}};_LL637: problem=Cyc_Absyn_qvar_cmp(
_tmpC88->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL633;_LL638:;_LL639:
problem=1;goto _LL633;_LL633:;}if(problem){const char*_tmp14C5;void*_tmp14C4;(
_tmp14C4=0,Cyc_Tcutil_terr(loc,((_tmp14C5="format attribute and vararg types don't match",
_tag_dyneither(_tmp14C5,sizeof(char),46))),_tag_dyneither(_tmp14C4,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB44;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC8C;void*_tmpC8D;void*_tmpC8E;struct _tuple0*_tmpC8B=(struct _tuple0*)
rpo->hd;_tmpC8C=*_tmpC8B;_tmpC8D=_tmpC8C.f1;_tmpC8E=_tmpC8C.f2;_tmpC3A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC3A,& Cyc_Tcutil_ek,_tmpC8D,1);_tmpC3A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC3A,& Cyc_Tcutil_trk,_tmpC8E,1);}}if(*_tmpB3F != 0)_tmpC3A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC3A,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB3F))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC3A.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC90;int _tmpC91;struct
_tuple29 _tmpC8F=*((struct _tuple29*)tvs->hd);_tmpC90=_tmpC8F.f1;_tmpC91=_tmpC8F.f2;
if(!_tmpC91)continue;{void*_tmpC92=Cyc_Absyn_compress_kb(_tmpC90->kind);struct
Cyc_Core_Opt*_tmpC94;struct Cyc_Core_Opt**_tmpC95;struct Cyc_Absyn_Kind*_tmpC96;
struct Cyc_Absyn_Kind*_tmpC98;struct Cyc_Absyn_Kind*_tmpC9A;struct Cyc_Absyn_Kind
_tmpC9B;enum Cyc_Absyn_KindQual _tmpC9C;struct Cyc_Absyn_Kind*_tmpC9E;struct Cyc_Absyn_Kind
_tmpC9F;enum Cyc_Absyn_KindQual _tmpCA0;struct Cyc_Core_Opt*_tmpCA2;struct Cyc_Core_Opt**
_tmpCA3;struct Cyc_Absyn_Kind*_tmpCA4;struct Cyc_Absyn_Kind _tmpCA5;enum Cyc_Absyn_KindQual
_tmpCA6;struct Cyc_Absyn_Kind*_tmpCA8;struct Cyc_Absyn_Kind _tmpCA9;enum Cyc_Absyn_KindQual
_tmpCAA;struct Cyc_Core_Opt*_tmpCAC;struct Cyc_Core_Opt**_tmpCAD;_LL63B:{struct Cyc_Absyn_Less_kb_struct*
_tmpC93=(struct Cyc_Absyn_Less_kb_struct*)_tmpC92;if(_tmpC93->tag != 2)goto _LL63D;
else{_tmpC94=_tmpC93->f1;_tmpC95=(struct Cyc_Core_Opt**)& _tmpC93->f1;_tmpC96=
_tmpC93->f2;}}if(!(_tmpC96->kind == Cyc_Absyn_RgnKind))goto _LL63D;_LL63C: if(
_tmpC96->aliasqual == Cyc_Absyn_Top){*_tmpC95=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpC98=_tmpC96;goto _LL63E;}*_tmpC95=Cyc_Tcutil_kind_to_bound_opt(_tmpC96);
_tmpC98=_tmpC96;goto _LL63E;_LL63D:{struct Cyc_Absyn_Eq_kb_struct*_tmpC97=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC92;if(_tmpC97->tag != 0)goto _LL63F;else{_tmpC98=
_tmpC97->f1;}}if(!(_tmpC98->kind == Cyc_Absyn_RgnKind))goto _LL63F;_LL63E:{struct
Cyc_Absyn_AccessEff_struct*_tmp14D4;struct Cyc_Absyn_VarType_struct*_tmp14D3;
struct Cyc_Absyn_VarType_struct _tmp14D2;struct Cyc_Absyn_AccessEff_struct _tmp14D1;
struct Cyc_List_List*_tmp14D0;effect=((_tmp14D0=_cycalloc(sizeof(*_tmp14D0)),((
_tmp14D0->hd=(void*)((_tmp14D4=_cycalloc(sizeof(*_tmp14D4)),((_tmp14D4[0]=((
_tmp14D1.tag=23,((_tmp14D1.f1=(void*)((void*)((_tmp14D3=_cycalloc(sizeof(*
_tmp14D3)),((_tmp14D3[0]=((_tmp14D2.tag=2,((_tmp14D2.f1=_tmpC90,_tmp14D2)))),
_tmp14D3))))),_tmp14D1)))),_tmp14D4)))),((_tmp14D0->tl=effect,_tmp14D0))))));}
goto _LL63A;_LL63F: {struct Cyc_Absyn_Less_kb_struct*_tmpC99=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC92;if(_tmpC99->tag != 2)goto _LL641;else{_tmpC9A=_tmpC99->f2;_tmpC9B=*_tmpC9A;
_tmpC9C=_tmpC9B.kind;if(_tmpC9C != Cyc_Absyn_IntKind)goto _LL641;}}_LL640: goto
_LL642;_LL641: {struct Cyc_Absyn_Eq_kb_struct*_tmpC9D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC92;if(_tmpC9D->tag != 0)goto _LL643;else{_tmpC9E=_tmpC9D->f1;_tmpC9F=*_tmpC9E;
_tmpCA0=_tmpC9F.kind;if(_tmpCA0 != Cyc_Absyn_IntKind)goto _LL643;}}_LL642: goto
_LL63A;_LL643: {struct Cyc_Absyn_Less_kb_struct*_tmpCA1=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC92;if(_tmpCA1->tag != 2)goto _LL645;else{_tmpCA2=_tmpCA1->f1;_tmpCA3=(struct
Cyc_Core_Opt**)& _tmpCA1->f1;_tmpCA4=_tmpCA1->f2;_tmpCA5=*_tmpCA4;_tmpCA6=_tmpCA5.kind;
if(_tmpCA6 != Cyc_Absyn_EffKind)goto _LL645;}}_LL644:*_tmpCA3=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL646;_LL645: {struct Cyc_Absyn_Eq_kb_struct*_tmpCA7=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC92;if(_tmpCA7->tag != 0)goto _LL647;else{_tmpCA8=
_tmpCA7->f1;_tmpCA9=*_tmpCA8;_tmpCAA=_tmpCA9.kind;if(_tmpCAA != Cyc_Absyn_EffKind)
goto _LL647;}}_LL646:{struct Cyc_Absyn_VarType_struct*_tmp14DA;struct Cyc_Absyn_VarType_struct
_tmp14D9;struct Cyc_List_List*_tmp14D8;effect=((_tmp14D8=_cycalloc(sizeof(*
_tmp14D8)),((_tmp14D8->hd=(void*)((_tmp14DA=_cycalloc(sizeof(*_tmp14DA)),((
_tmp14DA[0]=((_tmp14D9.tag=2,((_tmp14D9.f1=_tmpC90,_tmp14D9)))),_tmp14DA)))),((
_tmp14D8->tl=effect,_tmp14D8))))));}goto _LL63A;_LL647: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCAB=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC92;if(_tmpCAB->tag != 1)goto
_LL649;else{_tmpCAC=_tmpCAB->f1;_tmpCAD=(struct Cyc_Core_Opt**)& _tmpCAB->f1;}}
_LL648:{struct Cyc_Absyn_Less_kb_struct*_tmp14E0;struct Cyc_Absyn_Less_kb_struct
_tmp14DF;struct Cyc_Core_Opt*_tmp14DE;*_tmpCAD=((_tmp14DE=_cycalloc(sizeof(*
_tmp14DE)),((_tmp14DE->v=(void*)((_tmp14E0=_cycalloc(sizeof(*_tmp14E0)),((
_tmp14E0[0]=((_tmp14DF.tag=2,((_tmp14DF.f1=0,((_tmp14DF.f2=& Cyc_Tcutil_ak,
_tmp14DF)))))),_tmp14E0)))),_tmp14DE))));}goto _LL64A;_LL649:;_LL64A:{struct Cyc_Absyn_RgnsEff_struct*
_tmp14EF;struct Cyc_Absyn_VarType_struct*_tmp14EE;struct Cyc_Absyn_VarType_struct
_tmp14ED;struct Cyc_Absyn_RgnsEff_struct _tmp14EC;struct Cyc_List_List*_tmp14EB;
effect=((_tmp14EB=_cycalloc(sizeof(*_tmp14EB)),((_tmp14EB->hd=(void*)((_tmp14EF=
_cycalloc(sizeof(*_tmp14EF)),((_tmp14EF[0]=((_tmp14EC.tag=25,((_tmp14EC.f1=(void*)((
void*)((_tmp14EE=_cycalloc(sizeof(*_tmp14EE)),((_tmp14EE[0]=((_tmp14ED.tag=2,((
_tmp14ED.f1=_tmpC90,_tmp14ED)))),_tmp14EE))))),_tmp14EC)))),_tmp14EF)))),((
_tmp14EB->tl=effect,_tmp14EB))))));}goto _LL63A;_LL63A:;};}}{struct Cyc_List_List*
ts=_tmpC3A.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCBF;int _tmpCC0;struct
_tuple30 _tmpCBE=*((struct _tuple30*)ts->hd);_tmpCBF=_tmpCBE.f1;_tmpCC0=_tmpCBE.f2;
if(!_tmpCC0)continue;{struct Cyc_Absyn_Kind*_tmpCC1=Cyc_Tcutil_typ_kind(_tmpCBF);
struct Cyc_Absyn_Kind _tmpCC2;enum Cyc_Absyn_KindQual _tmpCC3;struct Cyc_Absyn_Kind
_tmpCC4;enum Cyc_Absyn_KindQual _tmpCC5;struct Cyc_Absyn_Kind _tmpCC6;enum Cyc_Absyn_KindQual
_tmpCC7;_LL64C: _tmpCC2=*_tmpCC1;_tmpCC3=_tmpCC2.kind;if(_tmpCC3 != Cyc_Absyn_RgnKind)
goto _LL64E;_LL64D:{struct Cyc_Absyn_AccessEff_struct*_tmp14F5;struct Cyc_Absyn_AccessEff_struct
_tmp14F4;struct Cyc_List_List*_tmp14F3;effect=((_tmp14F3=_cycalloc(sizeof(*
_tmp14F3)),((_tmp14F3->hd=(void*)((_tmp14F5=_cycalloc(sizeof(*_tmp14F5)),((
_tmp14F5[0]=((_tmp14F4.tag=23,((_tmp14F4.f1=(void*)_tmpCBF,_tmp14F4)))),_tmp14F5)))),((
_tmp14F3->tl=effect,_tmp14F3))))));}goto _LL64B;_LL64E: _tmpCC4=*_tmpCC1;_tmpCC5=
_tmpCC4.kind;if(_tmpCC5 != Cyc_Absyn_EffKind)goto _LL650;_LL64F:{struct Cyc_List_List*
_tmp14F6;effect=((_tmp14F6=_cycalloc(sizeof(*_tmp14F6)),((_tmp14F6->hd=_tmpCBF,((
_tmp14F6->tl=effect,_tmp14F6))))));}goto _LL64B;_LL650: _tmpCC6=*_tmpCC1;_tmpCC7=
_tmpCC6.kind;if(_tmpCC7 != Cyc_Absyn_IntKind)goto _LL652;_LL651: goto _LL64B;_LL652:;
_LL653:{struct Cyc_Absyn_RgnsEff_struct*_tmp14FC;struct Cyc_Absyn_RgnsEff_struct
_tmp14FB;struct Cyc_List_List*_tmp14FA;effect=((_tmp14FA=_cycalloc(sizeof(*
_tmp14FA)),((_tmp14FA->hd=(void*)((_tmp14FC=_cycalloc(sizeof(*_tmp14FC)),((
_tmp14FC[0]=((_tmp14FB.tag=25,((_tmp14FB.f1=(void*)_tmpCBF,_tmp14FB)))),_tmp14FC)))),((
_tmp14FA->tl=effect,_tmp14FA))))));}goto _LL64B;_LL64B:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1502;struct Cyc_Absyn_JoinEff_struct _tmp1501;struct Cyc_Core_Opt*_tmp1500;*
_tmpB3F=((_tmp1500=_cycalloc(sizeof(*_tmp1500)),((_tmp1500->v=(void*)((_tmp1502=
_cycalloc(sizeof(*_tmp1502)),((_tmp1502[0]=((_tmp1501.tag=24,((_tmp1501.f1=
effect,_tmp1501)))),_tmp1502)))),_tmp1500))));};}if(*_tmpB3D != 0){struct Cyc_List_List*
bs=*_tmpB3D;for(0;bs != 0;bs=bs->tl){void*_tmpCD2=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCD4;struct Cyc_Core_Opt**
_tmpCD5;struct Cyc_Core_Opt*_tmpCD7;struct Cyc_Core_Opt**_tmpCD8;struct Cyc_Absyn_Kind*
_tmpCD9;struct Cyc_Absyn_Kind _tmpCDA;enum Cyc_Absyn_KindQual _tmpCDB;enum Cyc_Absyn_AliasQual
_tmpCDC;struct Cyc_Core_Opt*_tmpCDE;struct Cyc_Core_Opt**_tmpCDF;struct Cyc_Absyn_Kind*
_tmpCE0;struct Cyc_Absyn_Kind _tmpCE1;enum Cyc_Absyn_KindQual _tmpCE2;enum Cyc_Absyn_AliasQual
_tmpCE3;struct Cyc_Core_Opt*_tmpCE5;struct Cyc_Core_Opt**_tmpCE6;struct Cyc_Absyn_Kind*
_tmpCE7;struct Cyc_Absyn_Kind _tmpCE8;enum Cyc_Absyn_KindQual _tmpCE9;enum Cyc_Absyn_AliasQual
_tmpCEA;struct Cyc_Core_Opt*_tmpCEC;struct Cyc_Core_Opt**_tmpCED;struct Cyc_Absyn_Kind*
_tmpCEE;struct Cyc_Absyn_Kind _tmpCEF;enum Cyc_Absyn_KindQual _tmpCF0;enum Cyc_Absyn_AliasQual
_tmpCF1;struct Cyc_Core_Opt*_tmpCF3;struct Cyc_Core_Opt**_tmpCF4;struct Cyc_Absyn_Kind*
_tmpCF5;struct Cyc_Absyn_Kind _tmpCF6;enum Cyc_Absyn_KindQual _tmpCF7;enum Cyc_Absyn_AliasQual
_tmpCF8;struct Cyc_Core_Opt*_tmpCFA;struct Cyc_Core_Opt**_tmpCFB;struct Cyc_Absyn_Kind*
_tmpCFC;struct Cyc_Absyn_Kind _tmpCFD;enum Cyc_Absyn_KindQual _tmpCFE;enum Cyc_Absyn_AliasQual
_tmpCFF;struct Cyc_Core_Opt*_tmpD01;struct Cyc_Core_Opt**_tmpD02;struct Cyc_Absyn_Kind*
_tmpD03;struct Cyc_Absyn_Kind _tmpD04;enum Cyc_Absyn_KindQual _tmpD05;enum Cyc_Absyn_AliasQual
_tmpD06;struct Cyc_Core_Opt*_tmpD08;struct Cyc_Core_Opt**_tmpD09;struct Cyc_Absyn_Kind*
_tmpD0A;struct Cyc_Absyn_Kind _tmpD0B;enum Cyc_Absyn_KindQual _tmpD0C;enum Cyc_Absyn_AliasQual
_tmpD0D;struct Cyc_Core_Opt*_tmpD0F;struct Cyc_Core_Opt**_tmpD10;struct Cyc_Absyn_Kind*
_tmpD11;struct Cyc_Absyn_Kind*_tmpD13;struct Cyc_Absyn_Kind _tmpD14;enum Cyc_Absyn_KindQual
_tmpD15;_LL655: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCD3=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCD2;if(_tmpCD3->tag != 1)goto _LL657;else{_tmpCD4=_tmpCD3->f1;_tmpCD5=(struct
Cyc_Core_Opt**)& _tmpCD3->f1;}}_LL656:{const char*_tmp1506;void*_tmp1505[1];struct
Cyc_String_pa_struct _tmp1504;(_tmp1504.tag=0,((_tmp1504.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1505[0]=&
_tmp1504,Cyc_Tcutil_warn(loc,((_tmp1506="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1506,sizeof(char),47))),_tag_dyneither(_tmp1505,sizeof(void*),
1)))))));}_tmpCD8=_tmpCD5;goto _LL658;_LL657: {struct Cyc_Absyn_Less_kb_struct*
_tmpCD6=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;if(_tmpCD6->tag != 2)goto _LL659;
else{_tmpCD7=_tmpCD6->f1;_tmpCD8=(struct Cyc_Core_Opt**)& _tmpCD6->f1;_tmpCD9=
_tmpCD6->f2;_tmpCDA=*_tmpCD9;_tmpCDB=_tmpCDA.kind;if(_tmpCDB != Cyc_Absyn_BoxKind)
goto _LL659;_tmpCDC=_tmpCDA.aliasqual;if(_tmpCDC != Cyc_Absyn_Top)goto _LL659;}}
_LL658: _tmpCDF=_tmpCD8;goto _LL65A;_LL659: {struct Cyc_Absyn_Less_kb_struct*
_tmpCDD=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;if(_tmpCDD->tag != 2)goto _LL65B;
else{_tmpCDE=_tmpCDD->f1;_tmpCDF=(struct Cyc_Core_Opt**)& _tmpCDD->f1;_tmpCE0=
_tmpCDD->f2;_tmpCE1=*_tmpCE0;_tmpCE2=_tmpCE1.kind;if(_tmpCE2 != Cyc_Absyn_MemKind)
goto _LL65B;_tmpCE3=_tmpCE1.aliasqual;if(_tmpCE3 != Cyc_Absyn_Top)goto _LL65B;}}
_LL65A: _tmpCE6=_tmpCDF;goto _LL65C;_LL65B: {struct Cyc_Absyn_Less_kb_struct*
_tmpCE4=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;if(_tmpCE4->tag != 2)goto _LL65D;
else{_tmpCE5=_tmpCE4->f1;_tmpCE6=(struct Cyc_Core_Opt**)& _tmpCE4->f1;_tmpCE7=
_tmpCE4->f2;_tmpCE8=*_tmpCE7;_tmpCE9=_tmpCE8.kind;if(_tmpCE9 != Cyc_Absyn_MemKind)
goto _LL65D;_tmpCEA=_tmpCE8.aliasqual;if(_tmpCEA != Cyc_Absyn_Aliasable)goto _LL65D;}}
_LL65C: _tmpCED=_tmpCE6;goto _LL65E;_LL65D: {struct Cyc_Absyn_Less_kb_struct*
_tmpCEB=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;if(_tmpCEB->tag != 2)goto _LL65F;
else{_tmpCEC=_tmpCEB->f1;_tmpCED=(struct Cyc_Core_Opt**)& _tmpCEB->f1;_tmpCEE=
_tmpCEB->f2;_tmpCEF=*_tmpCEE;_tmpCF0=_tmpCEF.kind;if(_tmpCF0 != Cyc_Absyn_AnyKind)
goto _LL65F;_tmpCF1=_tmpCEF.aliasqual;if(_tmpCF1 != Cyc_Absyn_Top)goto _LL65F;}}
_LL65E: _tmpCF4=_tmpCED;goto _LL660;_LL65F: {struct Cyc_Absyn_Less_kb_struct*
_tmpCF2=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;if(_tmpCF2->tag != 2)goto _LL661;
else{_tmpCF3=_tmpCF2->f1;_tmpCF4=(struct Cyc_Core_Opt**)& _tmpCF2->f1;_tmpCF5=
_tmpCF2->f2;_tmpCF6=*_tmpCF5;_tmpCF7=_tmpCF6.kind;if(_tmpCF7 != Cyc_Absyn_AnyKind)
goto _LL661;_tmpCF8=_tmpCF6.aliasqual;if(_tmpCF8 != Cyc_Absyn_Aliasable)goto _LL661;}}
_LL660:*_tmpCF4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL654;_LL661: {
struct Cyc_Absyn_Less_kb_struct*_tmpCF9=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;
if(_tmpCF9->tag != 2)goto _LL663;else{_tmpCFA=_tmpCF9->f1;_tmpCFB=(struct Cyc_Core_Opt**)&
_tmpCF9->f1;_tmpCFC=_tmpCF9->f2;_tmpCFD=*_tmpCFC;_tmpCFE=_tmpCFD.kind;if(_tmpCFE
!= Cyc_Absyn_MemKind)goto _LL663;_tmpCFF=_tmpCFD.aliasqual;if(_tmpCFF != Cyc_Absyn_Unique)
goto _LL663;}}_LL662: _tmpD02=_tmpCFB;goto _LL664;_LL663: {struct Cyc_Absyn_Less_kb_struct*
_tmpD00=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;if(_tmpD00->tag != 2)goto _LL665;
else{_tmpD01=_tmpD00->f1;_tmpD02=(struct Cyc_Core_Opt**)& _tmpD00->f1;_tmpD03=
_tmpD00->f2;_tmpD04=*_tmpD03;_tmpD05=_tmpD04.kind;if(_tmpD05 != Cyc_Absyn_AnyKind)
goto _LL665;_tmpD06=_tmpD04.aliasqual;if(_tmpD06 != Cyc_Absyn_Unique)goto _LL665;}}
_LL664:*_tmpD02=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL654;_LL665: {
struct Cyc_Absyn_Less_kb_struct*_tmpD07=(struct Cyc_Absyn_Less_kb_struct*)_tmpCD2;
if(_tmpD07->tag != 2)goto _LL667;else{_tmpD08=_tmpD07->f1;_tmpD09=(struct Cyc_Core_Opt**)&
_tmpD07->f1;_tmpD0A=_tmpD07->f2;_tmpD0B=*_tmpD0A;_tmpD0C=_tmpD0B.kind;if(_tmpD0C
!= Cyc_Absyn_RgnKind)goto _LL667;_tmpD0D=_tmpD0B.aliasqual;if(_tmpD0D != Cyc_Absyn_Top)
goto _LL667;}}_LL666:*_tmpD09=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL654;_LL667: {struct Cyc_Absyn_Less_kb_struct*_tmpD0E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCD2;if(_tmpD0E->tag != 2)goto _LL669;else{_tmpD0F=_tmpD0E->f1;_tmpD10=(struct
Cyc_Core_Opt**)& _tmpD0E->f1;_tmpD11=_tmpD0E->f2;}}_LL668:*_tmpD10=Cyc_Tcutil_kind_to_bound_opt(
_tmpD11);goto _LL654;_LL669: {struct Cyc_Absyn_Eq_kb_struct*_tmpD12=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCD2;if(_tmpD12->tag != 0)goto _LL66B;else{_tmpD13=_tmpD12->f1;_tmpD14=*_tmpD13;
_tmpD15=_tmpD14.kind;if(_tmpD15 != Cyc_Absyn_MemKind)goto _LL66B;}}_LL66A:{const
char*_tmp1509;void*_tmp1508;(_tmp1508=0,Cyc_Tcutil_terr(loc,((_tmp1509="functions can't abstract M types",
_tag_dyneither(_tmp1509,sizeof(char),33))),_tag_dyneither(_tmp1508,sizeof(void*),
0)));}goto _LL654;_LL66B:;_LL66C: goto _LL654;_LL654:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC3A.kind_env,*_tmpB3D);_tmpC3A.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC3A.r,_tmpC3A.free_vars,*_tmpB3D);{struct Cyc_List_List*tvs=_tmpC3A.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD1C;int _tmpD1D;struct
_tuple29 _tmpD1B=*((struct _tuple29*)tvs->hd);_tmpD1C=_tmpD1B.f1;_tmpD1D=_tmpD1B.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD1C,_tmpD1D);}}{struct Cyc_List_List*evs=_tmpC3A.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD1F;int _tmpD20;struct _tuple30 _tmpD1E=*((struct _tuple30*)evs->hd);
_tmpD1F=_tmpD1E.f1;_tmpD20=_tmpD1E.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD1F,_tmpD20);}};}goto _LL574;};}_LL595: {struct Cyc_Absyn_TupleType_struct*
_tmpB46=(struct Cyc_Absyn_TupleType_struct*)_tmpB00;if(_tmpB46->tag != 11)goto
_LL597;else{_tmpB47=_tmpB46->f1;}}_LL596: for(0;_tmpB47 != 0;_tmpB47=_tmpB47->tl){
struct _tuple11*_tmpD22=(struct _tuple11*)_tmpB47->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD22).f2,1);((*_tmpD22).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD22).f1).print_const,(*_tmpD22).f2);}goto _LL574;_LL597: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB48=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB00;if(_tmpB48->tag != 13)goto
_LL599;else{_tmpB49=_tmpB48->f1;_tmpB4A=_tmpB48->f2;}}_LL598: {struct
_RegionHandle*_tmpD23=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB4A != 0;_tmpB4A=_tmpB4A->tl){struct Cyc_Absyn_Aggrfield _tmpD25;struct
_dyneither_ptr*_tmpD26;struct Cyc_Absyn_Tqual _tmpD27;struct Cyc_Absyn_Tqual*
_tmpD28;void*_tmpD29;struct Cyc_Absyn_Exp*_tmpD2A;struct Cyc_List_List*_tmpD2B;
struct Cyc_Absyn_Aggrfield*_tmpD24=(struct Cyc_Absyn_Aggrfield*)_tmpB4A->hd;
_tmpD25=*_tmpD24;_tmpD26=_tmpD25.name;_tmpD27=_tmpD25.tq;_tmpD28=(struct Cyc_Absyn_Tqual*)&(*
_tmpD24).tq;_tmpD29=_tmpD25.type;_tmpD2A=_tmpD25.width;_tmpD2B=_tmpD25.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD26)){
const char*_tmp150D;void*_tmp150C[1];struct Cyc_String_pa_struct _tmp150B;(_tmp150B.tag=
0,((_tmp150B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD26),((
_tmp150C[0]=& _tmp150B,Cyc_Tcutil_terr(loc,((_tmp150D="duplicate field %s",
_tag_dyneither(_tmp150D,sizeof(char),19))),_tag_dyneither(_tmp150C,sizeof(void*),
1)))))));}{const char*_tmp150E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD26,((
_tmp150E="",_tag_dyneither(_tmp150E,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp150F;prev_fields=((_tmp150F=_region_malloc(_tmpD23,sizeof(*_tmp150F)),((
_tmp150F->hd=_tmpD26,((_tmp150F->tl=prev_fields,_tmp150F))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD29,1);_tmpD28->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD28->print_const,_tmpD29);if(_tmpB49 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD29)){const char*_tmp1513;void*_tmp1512[1];struct Cyc_String_pa_struct _tmp1511;(
_tmp1511.tag=0,((_tmp1511.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD26),((_tmp1512[0]=& _tmp1511,Cyc_Tcutil_warn(loc,((_tmp1513="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1513,sizeof(char),74))),_tag_dyneither(_tmp1512,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD29,_tmpD2A,_tmpD26);Cyc_Tcutil_check_field_atts(
loc,_tmpD26,_tmpD2B);}}goto _LL574;}_LL599: {struct Cyc_Absyn_AggrType_struct*
_tmpB4B=(struct Cyc_Absyn_AggrType_struct*)_tmpB00;if(_tmpB4B->tag != 12)goto
_LL59B;else{_tmpB4C=_tmpB4B->f1;_tmpB4D=_tmpB4C.aggr_info;_tmpB4E=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB4B->f1).aggr_info;_tmpB4F=_tmpB4C.targs;_tmpB50=(struct Cyc_List_List**)&(
_tmpB4B->f1).targs;}}_LL59A:{union Cyc_Absyn_AggrInfoU _tmpD34=*_tmpB4E;struct
_tuple4 _tmpD35;enum Cyc_Absyn_AggrKind _tmpD36;struct _tuple2*_tmpD37;struct Cyc_Core_Opt*
_tmpD38;struct Cyc_Absyn_Aggrdecl**_tmpD39;struct Cyc_Absyn_Aggrdecl*_tmpD3A;
_LL66E: if((_tmpD34.UnknownAggr).tag != 1)goto _LL670;_tmpD35=(struct _tuple4)(
_tmpD34.UnknownAggr).val;_tmpD36=_tmpD35.f1;_tmpD37=_tmpD35.f2;_tmpD38=_tmpD35.f3;
_LL66F: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD3B;_push_handler(&
_tmpD3B);{int _tmpD3D=0;if(setjmp(_tmpD3B.handler))_tmpD3D=1;if(!_tmpD3D){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD37);{struct Cyc_Absyn_Aggrdecl*_tmpD3E=*adp;if(_tmpD3E->kind != 
_tmpD36){if(_tmpD3E->kind == Cyc_Absyn_StructA){const char*_tmp1518;void*_tmp1517[
2];struct Cyc_String_pa_struct _tmp1516;struct Cyc_String_pa_struct _tmp1515;(
_tmp1515.tag=0,((_tmp1515.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD37)),((_tmp1516.tag=0,((_tmp1516.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD37)),((_tmp1517[0]=& _tmp1516,((
_tmp1517[1]=& _tmp1515,Cyc_Tcutil_terr(loc,((_tmp1518="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1518,sizeof(char),40))),_tag_dyneither(_tmp1517,sizeof(void*),
2)))))))))))));}else{const char*_tmp151D;void*_tmp151C[2];struct Cyc_String_pa_struct
_tmp151B;struct Cyc_String_pa_struct _tmp151A;(_tmp151A.tag=0,((_tmp151A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD37)),((
_tmp151B.tag=0,((_tmp151B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD37)),((_tmp151C[0]=& _tmp151B,((_tmp151C[1]=& _tmp151A,Cyc_Tcutil_terr(loc,((
_tmp151D="expecting union %s instead of struct %s",_tag_dyneither(_tmp151D,
sizeof(char),40))),_tag_dyneither(_tmp151C,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD38  && (int)_tmpD38->v){if(!((unsigned int)_tmpD3E->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD3E->impl))->tagged){const char*
_tmp1521;void*_tmp1520[1];struct Cyc_String_pa_struct _tmp151F;(_tmp151F.tag=0,((
_tmp151F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD37)),((_tmp1520[0]=& _tmp151F,Cyc_Tcutil_terr(loc,((_tmp1521="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1521,sizeof(char),42))),_tag_dyneither(_tmp1520,sizeof(void*),
1)))))));}}*_tmpB4E=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD3C=(void*)_exn_thrown;void*_tmpD4B=_tmpD3C;_LL673: {struct Cyc_Dict_Absent_struct*
_tmpD4C=(struct Cyc_Dict_Absent_struct*)_tmpD4B;if(_tmpD4C->tag != Cyc_Dict_Absent)
goto _LL675;}_LL674: {struct Cyc_Tcenv_Genv*_tmpD4D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1522;struct Cyc_Absyn_Aggrdecl*_tmpD4E=(_tmp1522=
_cycalloc(sizeof(*_tmp1522)),((_tmp1522->kind=_tmpD36,((_tmp1522->sc=Cyc_Absyn_Extern,((
_tmp1522->name=_tmpD37,((_tmp1522->tvs=0,((_tmp1522->impl=0,((_tmp1522->attributes=
0,_tmp1522)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD4D,loc,_tmpD4E);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD37);*_tmpB4E=Cyc_Absyn_KnownAggr(adp);if(*_tmpB50 != 0){{const char*
_tmp1526;void*_tmp1525[1];struct Cyc_String_pa_struct _tmp1524;(_tmp1524.tag=0,((
_tmp1524.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD37)),((_tmp1525[0]=& _tmp1524,Cyc_Tcutil_terr(loc,((_tmp1526="declare parameterized type %s before using",
_tag_dyneither(_tmp1526,sizeof(char),43))),_tag_dyneither(_tmp1525,sizeof(void*),
1)))))));}return cvtenv;}goto _LL672;}_LL675:;_LL676:(void)_throw(_tmpD4B);_LL672:;}};}
_tmpD3A=*adp;goto _LL671;}_LL670: if((_tmpD34.KnownAggr).tag != 2)goto _LL66D;
_tmpD39=(struct Cyc_Absyn_Aggrdecl**)(_tmpD34.KnownAggr).val;_tmpD3A=*_tmpD39;
_LL671: {struct Cyc_List_List*tvs=_tmpD3A->tvs;struct Cyc_List_List*ts=*_tmpB50;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD53=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD54=(void*)ts->hd;{struct _tuple0 _tmp1527;
struct _tuple0 _tmpD56=(_tmp1527.f1=Cyc_Absyn_compress_kb(_tmpD53->kind),((
_tmp1527.f2=_tmpD54,_tmp1527)));void*_tmpD57;void*_tmpD59;struct Cyc_Absyn_Tvar*
_tmpD5B;_LL678: _tmpD57=_tmpD56.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD58=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD57;if(_tmpD58->tag != 1)goto _LL67A;};
_tmpD59=_tmpD56.f2;{struct Cyc_Absyn_VarType_struct*_tmpD5A=(struct Cyc_Absyn_VarType_struct*)
_tmpD59;if(_tmpD5A->tag != 2)goto _LL67A;else{_tmpD5B=_tmpD5A->f1;}};_LL679: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD5B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD5B,1);continue;_LL67A:;_LL67B: goto _LL677;_LL677:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp152B;void*_tmp152A[1];struct Cyc_String_pa_struct _tmp1529;(
_tmp1529.tag=0,((_tmp1529.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD3A->name)),((_tmp152A[0]=& _tmp1529,Cyc_Tcutil_terr(loc,((_tmp152B="too many parameters for type %s",
_tag_dyneither(_tmp152B,sizeof(char),32))),_tag_dyneither(_tmp152A,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp152C;hidden_ts=((_tmp152C=_cycalloc(sizeof(*_tmp152C)),((_tmp152C->hd=e,((
_tmp152C->tl=hidden_ts,_tmp152C))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k,e,1);}*_tmpB50=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB50,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL66D:;}goto _LL574;
_LL59B: {struct Cyc_Absyn_TypedefType_struct*_tmpB51=(struct Cyc_Absyn_TypedefType_struct*)
_tmpB00;if(_tmpB51->tag != 18)goto _LL59D;else{_tmpB52=_tmpB51->f1;_tmpB53=_tmpB51->f2;
_tmpB54=(struct Cyc_List_List**)& _tmpB51->f2;_tmpB55=_tmpB51->f3;_tmpB56=(struct
Cyc_Absyn_Typedefdecl**)& _tmpB51->f3;_tmpB57=_tmpB51->f4;_tmpB58=(void***)&
_tmpB51->f4;}}_LL59C: {struct Cyc_List_List*targs=*_tmpB54;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpD60;_push_handler(& _tmpD60);{int _tmpD62=0;if(setjmp(
_tmpD60.handler))_tmpD62=1;if(!_tmpD62){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpB52);;_pop_handler();}else{void*_tmpD61=(void*)_exn_thrown;void*_tmpD64=
_tmpD61;_LL67D: {struct Cyc_Dict_Absent_struct*_tmpD65=(struct Cyc_Dict_Absent_struct*)
_tmpD64;if(_tmpD65->tag != Cyc_Dict_Absent)goto _LL67F;}_LL67E:{const char*_tmp1530;
void*_tmp152F[1];struct Cyc_String_pa_struct _tmp152E;(_tmp152E.tag=0,((_tmp152E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB52)),((
_tmp152F[0]=& _tmp152E,Cyc_Tcutil_terr(loc,((_tmp1530="unbound typedef name %s",
_tag_dyneither(_tmp1530,sizeof(char),24))),_tag_dyneither(_tmp152F,sizeof(void*),
1)))))));}return cvtenv;_LL67F:;_LL680:(void)_throw(_tmpD64);_LL67C:;}};}*_tmpB56=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpB52[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpD69=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple15*_tmp1533;struct Cyc_List_List*
_tmp1532;inst=((_tmp1532=_region_malloc(_tmpD69,sizeof(*_tmp1532)),((_tmp1532->hd=((
_tmp1533=_region_malloc(_tmpD69,sizeof(*_tmp1533)),((_tmp1533->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1533->f2=(void*)ts->hd,_tmp1533)))))),((_tmp1532->tl=inst,_tmp1532))))));};}
if(ts != 0){const char*_tmp1537;void*_tmp1536[1];struct Cyc_String_pa_struct
_tmp1535;(_tmp1535.tag=0,((_tmp1535.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB52)),((_tmp1536[0]=& _tmp1535,Cyc_Tcutil_terr(
loc,((_tmp1537="too many parameters for typedef %s",_tag_dyneither(_tmp1537,
sizeof(char),35))),_tag_dyneither(_tmp1536,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp1538;hidden_ts=((_tmp1538=_cycalloc(sizeof(*_tmp1538)),((
_tmp1538->hd=e,((_tmp1538->tl=hidden_ts,_tmp1538))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp153B;struct Cyc_List_List*_tmp153A;inst=(
struct Cyc_List_List*)((_tmp153A=_cycalloc(sizeof(*_tmp153A)),((_tmp153A->hd=(
struct _tuple15*)((_tmp153B=_cycalloc(sizeof(*_tmp153B)),((_tmp153B->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp153B->f2=e,_tmp153B)))))),((_tmp153A->tl=inst,
_tmp153A))))));};}*_tmpB54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD69,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp153C;*_tmpB58=((_tmp153C=_cycalloc(sizeof(*
_tmp153C)),((_tmp153C[0]=new_typ,_tmp153C))));}}goto _LL574;};}_LL59D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpB59=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpB00;if(_tmpB59->tag != 22)goto
_LL59F;}_LL59E: goto _LL5A0;_LL59F: {struct Cyc_Absyn_HeapRgn_struct*_tmpB5A=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpB00;if(_tmpB5A->tag != 21)goto _LL5A1;}_LL5A0:
goto _LL574;_LL5A1: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB5B=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpB00;if(_tmpB5B->tag != 16)goto _LL5A3;else{_tmpB5C=(void*)_tmpB5B->f1;}}_LL5A2:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB5C,1);
goto _LL574;_LL5A3: {struct Cyc_Absyn_DynRgnType_struct*_tmpB5D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpB00;if(_tmpB5D->tag != 17)goto _LL5A5;else{_tmpB5E=(void*)_tmpB5D->f1;_tmpB5F=(
void*)_tmpB5D->f2;}}_LL5A4: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpB5E,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpB5F,1);goto _LL574;_LL5A5: {struct Cyc_Absyn_AccessEff_struct*_tmpB60=(struct
Cyc_Absyn_AccessEff_struct*)_tmpB00;if(_tmpB60->tag != 23)goto _LL5A7;else{_tmpB61=(
void*)_tmpB60->f1;}}_LL5A6: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpB61,1);goto _LL574;_LL5A7: {struct Cyc_Absyn_RgnsEff_struct*
_tmpB62=(struct Cyc_Absyn_RgnsEff_struct*)_tmpB00;if(_tmpB62->tag != 25)goto _LL5A9;
else{_tmpB63=(void*)_tmpB62->f1;}}_LL5A8: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB63,1);goto _LL574;_LL5A9: {struct Cyc_Absyn_JoinEff_struct*
_tmpB64=(struct Cyc_Absyn_JoinEff_struct*)_tmpB00;if(_tmpB64->tag != 24)goto _LL574;
else{_tmpB65=_tmpB64->f1;}}_LL5AA: for(0;_tmpB65 != 0;_tmpB65=_tmpB65->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB65->hd,1);}
goto _LL574;_LL574:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpD73=t;struct Cyc_Core_Opt*_tmpD75;struct Cyc_Core_Opt*_tmpD76;_LL682: {
struct Cyc_Absyn_Evar_struct*_tmpD74=(struct Cyc_Absyn_Evar_struct*)_tmpD73;if(
_tmpD74->tag != 1)goto _LL684;else{_tmpD75=_tmpD74->f1;_tmpD76=_tmpD74->f2;}}
_LL683: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpD77=_tmpD75;struct Cyc_Core_Opt
_tmpD78;struct Cyc_Absyn_Kind*_tmpD79;_LL687: if(_tmpD77 != 0)goto _LL689;_LL688:{
const char*_tmp153D;s=((_tmp153D="kind=NULL ",_tag_dyneither(_tmp153D,sizeof(char),
11)));}goto _LL686;_LL689: if(_tmpD77 == 0)goto _LL686;_tmpD78=*_tmpD77;_tmpD79=(
struct Cyc_Absyn_Kind*)_tmpD78.v;_LL68A:{const char*_tmp1541;void*_tmp1540[1];
struct Cyc_String_pa_struct _tmp153F;s=(struct _dyneither_ptr)((_tmp153F.tag=0,((
_tmp153F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpD79)),((_tmp1540[0]=& _tmp153F,Cyc_aprintf(((_tmp1541="kind=%s ",
_tag_dyneither(_tmp1541,sizeof(char),9))),_tag_dyneither(_tmp1540,sizeof(void*),
1))))))));}goto _LL686;_LL686:;}{struct Cyc_Core_Opt*_tmpD7E=_tmpD76;struct Cyc_Core_Opt
_tmpD7F;void*_tmpD80;_LL68C: if(_tmpD7E != 0)goto _LL68E;_LL68D:{const char*_tmp1545;
void*_tmp1544[1];struct Cyc_String_pa_struct _tmp1543;s=(struct _dyneither_ptr)((
_tmp1543.tag=0,((_tmp1543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp1544[0]=& _tmp1543,Cyc_aprintf(((_tmp1545="%s ref=NULL",_tag_dyneither(
_tmp1545,sizeof(char),12))),_tag_dyneither(_tmp1544,sizeof(void*),1))))))));}
goto _LL68B;_LL68E: if(_tmpD7E == 0)goto _LL68B;_tmpD7F=*_tmpD7E;_tmpD80=(void*)
_tmpD7F.v;_LL68F:{const char*_tmp154A;void*_tmp1549[2];struct Cyc_String_pa_struct
_tmp1548;struct Cyc_String_pa_struct _tmp1547;s=(struct _dyneither_ptr)((_tmp1547.tag=
0,((_tmp1547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpD80)),((_tmp1548.tag=0,((_tmp1548.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp1549[0]=& _tmp1548,((_tmp1549[1]=& _tmp1547,Cyc_aprintf(((
_tmp154A="%s ref=%s",_tag_dyneither(_tmp154A,sizeof(char),10))),_tag_dyneither(
_tmp1549,sizeof(void*),2))))))))))))));}goto _LL68B;_LL68B:;}goto _LL681;}_LL684:;
_LL685: goto _LL681;_LL681:;}{const char*_tmp1550;void*_tmp154F[3];struct Cyc_String_pa_struct
_tmp154E;struct Cyc_String_pa_struct _tmp154D;struct Cyc_String_pa_struct _tmp154C;(
_tmp154C.tag=0,((_tmp154C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp154D.tag=0,((_tmp154D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp154E.tag=0,((
_tmp154E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp154F[0]=& _tmp154E,((_tmp154F[1]=& _tmp154D,((_tmp154F[2]=& _tmp154C,Cyc_Tcutil_terr(
loc,((_tmp1550="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1550,sizeof(char),51))),_tag_dyneither(_tmp154F,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpD8D=e->r;struct Cyc_List_List*_tmpD93;struct Cyc_Absyn_Exp*_tmpD95;struct
Cyc_Absyn_Exp*_tmpD96;struct Cyc_Absyn_Exp*_tmpD97;struct Cyc_Absyn_Exp*_tmpD99;
struct Cyc_Absyn_Exp*_tmpD9A;struct Cyc_Absyn_Exp*_tmpD9C;struct Cyc_Absyn_Exp*
_tmpD9D;struct Cyc_Absyn_Exp*_tmpD9F;struct Cyc_Absyn_Exp*_tmpDA0;void*_tmpDA2;
struct Cyc_Absyn_Exp*_tmpDA3;void*_tmpDA5;void*_tmpDA7;void*_tmpDA9;struct Cyc_Absyn_Exp*
_tmpDAB;_LL691: {struct Cyc_Absyn_Const_e_struct*_tmpD8E=(struct Cyc_Absyn_Const_e_struct*)
_tmpD8D;if(_tmpD8E->tag != 0)goto _LL693;}_LL692: goto _LL694;_LL693: {struct Cyc_Absyn_Enum_e_struct*
_tmpD8F=(struct Cyc_Absyn_Enum_e_struct*)_tmpD8D;if(_tmpD8F->tag != 32)goto _LL695;}
_LL694: goto _LL696;_LL695: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD90=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD8D;if(_tmpD90->tag != 33)goto _LL697;}_LL696: goto _LL698;_LL697: {struct Cyc_Absyn_Var_e_struct*
_tmpD91=(struct Cyc_Absyn_Var_e_struct*)_tmpD8D;if(_tmpD91->tag != 1)goto _LL699;}
_LL698: goto _LL690;_LL699: {struct Cyc_Absyn_Primop_e_struct*_tmpD92=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD8D;if(_tmpD92->tag != 3)goto _LL69B;else{_tmpD93=_tmpD92->f2;}}_LL69A: for(0;
_tmpD93 != 0;_tmpD93=_tmpD93->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpD93->hd,te,cvtenv);}goto _LL690;_LL69B: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD94=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD8D;if(_tmpD94->tag != 6)goto
_LL69D;else{_tmpD95=_tmpD94->f1;_tmpD96=_tmpD94->f2;_tmpD97=_tmpD94->f3;}}_LL69C:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD95,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD96,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD97,te,
cvtenv);goto _LL690;_LL69D: {struct Cyc_Absyn_And_e_struct*_tmpD98=(struct Cyc_Absyn_And_e_struct*)
_tmpD8D;if(_tmpD98->tag != 7)goto _LL69F;else{_tmpD99=_tmpD98->f1;_tmpD9A=_tmpD98->f2;}}
_LL69E: _tmpD9C=_tmpD99;_tmpD9D=_tmpD9A;goto _LL6A0;_LL69F: {struct Cyc_Absyn_Or_e_struct*
_tmpD9B=(struct Cyc_Absyn_Or_e_struct*)_tmpD8D;if(_tmpD9B->tag != 8)goto _LL6A1;
else{_tmpD9C=_tmpD9B->f1;_tmpD9D=_tmpD9B->f2;}}_LL6A0: _tmpD9F=_tmpD9C;_tmpDA0=
_tmpD9D;goto _LL6A2;_LL6A1: {struct Cyc_Absyn_SeqExp_e_struct*_tmpD9E=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD8D;if(_tmpD9E->tag != 9)goto _LL6A3;else{_tmpD9F=_tmpD9E->f1;_tmpDA0=_tmpD9E->f2;}}
_LL6A2: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD9F,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA0,te,cvtenv);goto _LL690;_LL6A3: {
struct Cyc_Absyn_Cast_e_struct*_tmpDA1=(struct Cyc_Absyn_Cast_e_struct*)_tmpD8D;
if(_tmpDA1->tag != 15)goto _LL6A5;else{_tmpDA2=(void*)_tmpDA1->f1;_tmpDA3=_tmpDA1->f2;}}
_LL6A4: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDA3,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDA2,1);goto
_LL690;_LL6A5: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDA4=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD8D;if(_tmpDA4->tag != 20)goto _LL6A7;else{_tmpDA5=(void*)_tmpDA4->f1;}}_LL6A6:
_tmpDA7=_tmpDA5;goto _LL6A8;_LL6A7: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDA6=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD8D;if(_tmpDA6->tag != 18)goto _LL6A9;
else{_tmpDA7=(void*)_tmpDA6->f1;}}_LL6A8: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDA7,1);goto _LL690;_LL6A9: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDA8=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD8D;if(_tmpDA8->tag != 39)goto
_LL6AB;else{_tmpDA9=(void*)_tmpDA8->f1;}}_LL6AA: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDA9,1);goto _LL690;_LL6AB: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDAA=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD8D;if(_tmpDAA->tag != 19)goto
_LL6AD;else{_tmpDAB=_tmpDAA->f1;}}_LL6AC: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDAB,te,cvtenv);goto _LL690;_LL6AD:;_LL6AE: {const char*_tmp1553;void*_tmp1552;(
_tmp1552=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1553="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1553,sizeof(char),66))),_tag_dyneither(_tmp1552,sizeof(void*),
0)));}_LL690:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDAE=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDB0;struct
_tuple29 _tmpDAF=*((struct _tuple29*)vs->hd);_tmpDB0=_tmpDAF.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDAE,_tmpDB0);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDB2;struct _tuple30 _tmpDB1=*((struct _tuple30*)evs->hd);_tmpDB2=_tmpDB1.f1;{
void*_tmpDB3=Cyc_Tcutil_compress(_tmpDB2);struct Cyc_Core_Opt*_tmpDB5;struct Cyc_Core_Opt**
_tmpDB6;_LL6B0: {struct Cyc_Absyn_Evar_struct*_tmpDB4=(struct Cyc_Absyn_Evar_struct*)
_tmpDB3;if(_tmpDB4->tag != 1)goto _LL6B2;else{_tmpDB5=_tmpDB4->f4;_tmpDB6=(struct
Cyc_Core_Opt**)& _tmpDB4->f4;}}_LL6B1: if(*_tmpDB6 == 0){struct Cyc_Core_Opt*
_tmp1554;*_tmpDB6=((_tmp1554=_cycalloc(sizeof(*_tmp1554)),((_tmp1554->v=_tmpDAE,
_tmp1554))));}else{struct Cyc_List_List*_tmpDB8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDB6))->v;struct Cyc_List_List*_tmpDB9=0;for(0;_tmpDB8 != 0;
_tmpDB8=_tmpDB8->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDAE,(struct Cyc_Absyn_Tvar*)_tmpDB8->hd)){struct Cyc_List_List*_tmp1555;
_tmpDB9=((_tmp1555=_cycalloc(sizeof(*_tmp1555)),((_tmp1555->hd=(struct Cyc_Absyn_Tvar*)
_tmpDB8->hd,((_tmp1555->tl=_tmpDB9,_tmp1555))))));}}{struct Cyc_Core_Opt*_tmp1556;*
_tmpDB6=((_tmp1556=_cycalloc(sizeof(*_tmp1556)),((_tmp1556->v=_tmpDB9,_tmp1556))));};}
goto _LL6AF;_LL6B2:;_LL6B3: goto _LL6AF;_LL6AF:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpDBC=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpDBD=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp1557;struct Cyc_Tcutil_CVTEnv _tmpDBE=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp1557.r=_tmpDBD,((_tmp1557.kind_env=_tmpDBC,((_tmp1557.free_vars=0,((_tmp1557.free_evars=
0,((_tmp1557.generalize_evars=generalize_evars,((_tmp1557.fn_result=0,_tmp1557)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDBF=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDBD,(struct Cyc_Absyn_Tvar*(*)(struct _tuple29*))Cyc_Core_fst,
_tmpDBE.free_vars);struct Cyc_List_List*_tmpDC0=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpDBD,(void*(*)(struct _tuple30*))Cyc_Core_fst,_tmpDBE.free_evars);if(_tmpDBC != 
0){struct Cyc_List_List*_tmpDC1=0;{struct Cyc_List_List*_tmpDC2=_tmpDBF;for(0;(
unsigned int)_tmpDC2;_tmpDC2=_tmpDC2->tl){struct Cyc_Absyn_Tvar*_tmpDC3=(struct
Cyc_Absyn_Tvar*)_tmpDC2->hd;int found=0;{struct Cyc_List_List*_tmpDC4=_tmpDBC;for(
0;(unsigned int)_tmpDC4;_tmpDC4=_tmpDC4->tl){if(Cyc_Absyn_tvar_cmp(_tmpDC3,(
struct Cyc_Absyn_Tvar*)_tmpDC4->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp1558;_tmpDC1=((_tmp1558=_region_malloc(_tmpDBD,sizeof(*_tmp1558)),((_tmp1558->hd=(
struct Cyc_Absyn_Tvar*)_tmpDC2->hd,((_tmp1558->tl=_tmpDC1,_tmp1558))))));}}}
_tmpDBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpDC1);}{struct Cyc_List_List*x=_tmpDBF;for(0;x != 0;x=x->tl){void*_tmpDC6=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDC8;struct Cyc_Core_Opt**
_tmpDC9;struct Cyc_Core_Opt*_tmpDCB;struct Cyc_Core_Opt**_tmpDCC;struct Cyc_Absyn_Kind*
_tmpDCD;struct Cyc_Absyn_Kind _tmpDCE;enum Cyc_Absyn_KindQual _tmpDCF;enum Cyc_Absyn_AliasQual
_tmpDD0;struct Cyc_Core_Opt*_tmpDD2;struct Cyc_Core_Opt**_tmpDD3;struct Cyc_Absyn_Kind*
_tmpDD4;struct Cyc_Absyn_Kind _tmpDD5;enum Cyc_Absyn_KindQual _tmpDD6;enum Cyc_Absyn_AliasQual
_tmpDD7;struct Cyc_Core_Opt*_tmpDD9;struct Cyc_Core_Opt**_tmpDDA;struct Cyc_Absyn_Kind*
_tmpDDB;struct Cyc_Absyn_Kind _tmpDDC;enum Cyc_Absyn_KindQual _tmpDDD;enum Cyc_Absyn_AliasQual
_tmpDDE;struct Cyc_Core_Opt*_tmpDE0;struct Cyc_Core_Opt**_tmpDE1;struct Cyc_Absyn_Kind*
_tmpDE2;struct Cyc_Absyn_Kind _tmpDE3;enum Cyc_Absyn_KindQual _tmpDE4;enum Cyc_Absyn_AliasQual
_tmpDE5;struct Cyc_Core_Opt*_tmpDE7;struct Cyc_Core_Opt**_tmpDE8;struct Cyc_Absyn_Kind*
_tmpDE9;struct Cyc_Absyn_Kind _tmpDEA;enum Cyc_Absyn_KindQual _tmpDEB;enum Cyc_Absyn_AliasQual
_tmpDEC;struct Cyc_Core_Opt*_tmpDEE;struct Cyc_Core_Opt**_tmpDEF;struct Cyc_Absyn_Kind*
_tmpDF0;struct Cyc_Absyn_Kind*_tmpDF2;struct Cyc_Absyn_Kind _tmpDF3;enum Cyc_Absyn_KindQual
_tmpDF4;enum Cyc_Absyn_AliasQual _tmpDF5;_LL6B5: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDC7=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDC6;if(_tmpDC7->tag != 1)goto
_LL6B7;else{_tmpDC8=_tmpDC7->f1;_tmpDC9=(struct Cyc_Core_Opt**)& _tmpDC7->f1;}}
_LL6B6: _tmpDCC=_tmpDC9;goto _LL6B8;_LL6B7: {struct Cyc_Absyn_Less_kb_struct*
_tmpDCA=(struct Cyc_Absyn_Less_kb_struct*)_tmpDC6;if(_tmpDCA->tag != 2)goto _LL6B9;
else{_tmpDCB=_tmpDCA->f1;_tmpDCC=(struct Cyc_Core_Opt**)& _tmpDCA->f1;_tmpDCD=
_tmpDCA->f2;_tmpDCE=*_tmpDCD;_tmpDCF=_tmpDCE.kind;if(_tmpDCF != Cyc_Absyn_BoxKind)
goto _LL6B9;_tmpDD0=_tmpDCE.aliasqual;if(_tmpDD0 != Cyc_Absyn_Top)goto _LL6B9;}}
_LL6B8: _tmpDD3=_tmpDCC;goto _LL6BA;_LL6B9: {struct Cyc_Absyn_Less_kb_struct*
_tmpDD1=(struct Cyc_Absyn_Less_kb_struct*)_tmpDC6;if(_tmpDD1->tag != 2)goto _LL6BB;
else{_tmpDD2=_tmpDD1->f1;_tmpDD3=(struct Cyc_Core_Opt**)& _tmpDD1->f1;_tmpDD4=
_tmpDD1->f2;_tmpDD5=*_tmpDD4;_tmpDD6=_tmpDD5.kind;if(_tmpDD6 != Cyc_Absyn_MemKind)
goto _LL6BB;_tmpDD7=_tmpDD5.aliasqual;if(_tmpDD7 != Cyc_Absyn_Top)goto _LL6BB;}}
_LL6BA: _tmpDDA=_tmpDD3;goto _LL6BC;_LL6BB: {struct Cyc_Absyn_Less_kb_struct*
_tmpDD8=(struct Cyc_Absyn_Less_kb_struct*)_tmpDC6;if(_tmpDD8->tag != 2)goto _LL6BD;
else{_tmpDD9=_tmpDD8->f1;_tmpDDA=(struct Cyc_Core_Opt**)& _tmpDD8->f1;_tmpDDB=
_tmpDD8->f2;_tmpDDC=*_tmpDDB;_tmpDDD=_tmpDDC.kind;if(_tmpDDD != Cyc_Absyn_MemKind)
goto _LL6BD;_tmpDDE=_tmpDDC.aliasqual;if(_tmpDDE != Cyc_Absyn_Aliasable)goto _LL6BD;}}
_LL6BC:*_tmpDDA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6B4;_LL6BD: {
struct Cyc_Absyn_Less_kb_struct*_tmpDDF=(struct Cyc_Absyn_Less_kb_struct*)_tmpDC6;
if(_tmpDDF->tag != 2)goto _LL6BF;else{_tmpDE0=_tmpDDF->f1;_tmpDE1=(struct Cyc_Core_Opt**)&
_tmpDDF->f1;_tmpDE2=_tmpDDF->f2;_tmpDE3=*_tmpDE2;_tmpDE4=_tmpDE3.kind;if(_tmpDE4
!= Cyc_Absyn_MemKind)goto _LL6BF;_tmpDE5=_tmpDE3.aliasqual;if(_tmpDE5 != Cyc_Absyn_Unique)
goto _LL6BF;}}_LL6BE:*_tmpDE1=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6B4;_LL6BF: {struct Cyc_Absyn_Less_kb_struct*_tmpDE6=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDC6;if(_tmpDE6->tag != 2)goto _LL6C1;else{_tmpDE7=_tmpDE6->f1;_tmpDE8=(struct
Cyc_Core_Opt**)& _tmpDE6->f1;_tmpDE9=_tmpDE6->f2;_tmpDEA=*_tmpDE9;_tmpDEB=_tmpDEA.kind;
if(_tmpDEB != Cyc_Absyn_RgnKind)goto _LL6C1;_tmpDEC=_tmpDEA.aliasqual;if(_tmpDEC != 
Cyc_Absyn_Top)goto _LL6C1;}}_LL6C0:*_tmpDE8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6B4;_LL6C1: {struct Cyc_Absyn_Less_kb_struct*_tmpDED=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDC6;if(_tmpDED->tag != 2)goto _LL6C3;else{_tmpDEE=_tmpDED->f1;_tmpDEF=(struct
Cyc_Core_Opt**)& _tmpDED->f1;_tmpDF0=_tmpDED->f2;}}_LL6C2:*_tmpDEF=Cyc_Tcutil_kind_to_bound_opt(
_tmpDF0);goto _LL6B4;_LL6C3: {struct Cyc_Absyn_Eq_kb_struct*_tmpDF1=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDC6;if(_tmpDF1->tag != 0)goto _LL6C5;else{_tmpDF2=_tmpDF1->f1;_tmpDF3=*_tmpDF2;
_tmpDF4=_tmpDF3.kind;if(_tmpDF4 != Cyc_Absyn_MemKind)goto _LL6C5;_tmpDF5=_tmpDF3.aliasqual;}}
_LL6C4:{const char*_tmp155F;void*_tmp155E[2];struct Cyc_String_pa_struct _tmp155D;
struct Cyc_Absyn_Kind*_tmp155C;struct Cyc_String_pa_struct _tmp155B;(_tmp155B.tag=0,((
_tmp155B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp155C=_cycalloc_atomic(sizeof(*_tmp155C)),((_tmp155C->kind=Cyc_Absyn_MemKind,((
_tmp155C->aliasqual=_tmpDF5,_tmp155C)))))))),((_tmp155D.tag=0,((_tmp155D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp155E[0]=& _tmp155D,((_tmp155E[1]=& _tmp155B,Cyc_Tcutil_terr(loc,((
_tmp155F="type variable %s cannot have kind %s",_tag_dyneither(_tmp155F,sizeof(
char),37))),_tag_dyneither(_tmp155E,sizeof(void*),2)))))))))))));}goto _LL6B4;
_LL6C5:;_LL6C6: goto _LL6B4;_LL6B4:;}}if(_tmpDBF != 0  || _tmpDC0 != 0){{void*_tmpDFB=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpDFD;struct Cyc_List_List*_tmpDFE;
struct Cyc_List_List**_tmpDFF;struct Cyc_Core_Opt*_tmpE00;void*_tmpE01;struct Cyc_List_List*
_tmpE02;int _tmpE03;struct Cyc_Absyn_VarargInfo*_tmpE04;struct Cyc_List_List*
_tmpE05;struct Cyc_List_List*_tmpE06;_LL6C8: {struct Cyc_Absyn_FnType_struct*
_tmpDFC=(struct Cyc_Absyn_FnType_struct*)_tmpDFB;if(_tmpDFC->tag != 10)goto _LL6CA;
else{_tmpDFD=_tmpDFC->f1;_tmpDFE=_tmpDFD.tvars;_tmpDFF=(struct Cyc_List_List**)&(
_tmpDFC->f1).tvars;_tmpE00=_tmpDFD.effect;_tmpE01=_tmpDFD.ret_typ;_tmpE02=
_tmpDFD.args;_tmpE03=_tmpDFD.c_varargs;_tmpE04=_tmpDFD.cyc_varargs;_tmpE05=
_tmpDFD.rgn_po;_tmpE06=_tmpDFD.attributes;}}_LL6C9: if(*_tmpDFF == 0){*_tmpDFF=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpDBF);_tmpDBF=0;}
goto _LL6C7;_LL6CA:;_LL6CB: goto _LL6C7;_LL6C7:;}if(_tmpDBF != 0){const char*_tmp1563;
void*_tmp1562[1];struct Cyc_String_pa_struct _tmp1561;(_tmp1561.tag=0,((_tmp1561.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDBF->hd)->name),((
_tmp1562[0]=& _tmp1561,Cyc_Tcutil_terr(loc,((_tmp1563="unbound type variable %s",
_tag_dyneither(_tmp1563,sizeof(char),25))),_tag_dyneither(_tmp1562,sizeof(void*),
1)))))));}if(_tmpDC0 != 0)for(0;_tmpDC0 != 0;_tmpDC0=_tmpDC0->tl){void*e=(void*)
_tmpDC0->hd;struct Cyc_Absyn_Kind*_tmpE0A=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE0B;enum Cyc_Absyn_KindQual _tmpE0C;enum Cyc_Absyn_AliasQual _tmpE0D;struct Cyc_Absyn_Kind
_tmpE0E;enum Cyc_Absyn_KindQual _tmpE0F;enum Cyc_Absyn_AliasQual _tmpE10;struct Cyc_Absyn_Kind
_tmpE11;enum Cyc_Absyn_KindQual _tmpE12;enum Cyc_Absyn_AliasQual _tmpE13;struct Cyc_Absyn_Kind
_tmpE14;enum Cyc_Absyn_KindQual _tmpE15;_LL6CD: _tmpE0B=*_tmpE0A;_tmpE0C=_tmpE0B.kind;
if(_tmpE0C != Cyc_Absyn_RgnKind)goto _LL6CF;_tmpE0D=_tmpE0B.aliasqual;if(_tmpE0D != 
Cyc_Absyn_Unique)goto _LL6CF;_LL6CE: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1566;void*_tmp1565;(_tmp1565=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1566="can't unify evar with unique region!",
_tag_dyneither(_tmp1566,sizeof(char),37))),_tag_dyneither(_tmp1565,sizeof(void*),
0)));}goto _LL6CC;_LL6CF: _tmpE0E=*_tmpE0A;_tmpE0F=_tmpE0E.kind;if(_tmpE0F != Cyc_Absyn_RgnKind)
goto _LL6D1;_tmpE10=_tmpE0E.aliasqual;if(_tmpE10 != Cyc_Absyn_Aliasable)goto _LL6D1;
_LL6D0: goto _LL6D2;_LL6D1: _tmpE11=*_tmpE0A;_tmpE12=_tmpE11.kind;if(_tmpE12 != Cyc_Absyn_RgnKind)
goto _LL6D3;_tmpE13=_tmpE11.aliasqual;if(_tmpE13 != Cyc_Absyn_Top)goto _LL6D3;
_LL6D2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1569;
void*_tmp1568;(_tmp1568=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1569="can't unify evar with heap!",_tag_dyneither(
_tmp1569,sizeof(char),28))),_tag_dyneither(_tmp1568,sizeof(void*),0)));}goto
_LL6CC;_LL6D3: _tmpE14=*_tmpE0A;_tmpE15=_tmpE14.kind;if(_tmpE15 != Cyc_Absyn_EffKind)
goto _LL6D5;_LL6D4: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp156C;void*_tmp156B;(_tmp156B=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp156C="can't unify evar with {}!",
_tag_dyneither(_tmp156C,sizeof(char),26))),_tag_dyneither(_tmp156B,sizeof(void*),
0)));}goto _LL6CC;_LL6D5:;_LL6D6:{const char*_tmp1571;void*_tmp1570[2];struct Cyc_String_pa_struct
_tmp156F;struct Cyc_String_pa_struct _tmp156E;(_tmp156E.tag=0,((_tmp156E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp156F.tag=0,((_tmp156F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1570[0]=& _tmp156F,((_tmp1570[1]=& _tmp156E,Cyc_Tcutil_terr(loc,((
_tmp1571="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1571,sizeof(char),52))),_tag_dyneither(_tmp1570,sizeof(void*),2)))))))))))));}
goto _LL6CC;_LL6CC:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE21=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE23;struct Cyc_List_List*_tmpE24;struct Cyc_Core_Opt*
_tmpE25;void*_tmpE26;_LL6D8: {struct Cyc_Absyn_FnType_struct*_tmpE22=(struct Cyc_Absyn_FnType_struct*)
_tmpE21;if(_tmpE22->tag != 10)goto _LL6DA;else{_tmpE23=_tmpE22->f1;_tmpE24=_tmpE23.tvars;
_tmpE25=_tmpE23.effect;_tmpE26=_tmpE23.ret_typ;}}_LL6D9: fd->tvs=_tmpE24;fd->effect=
_tmpE25;goto _LL6D7;_LL6DA:;_LL6DB: {const char*_tmp1574;void*_tmp1573;(_tmp1573=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1574="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1574,sizeof(
char),38))),_tag_dyneither(_tmp1573,sizeof(void*),0)));}_LL6D7:;}{struct
_RegionHandle*_tmpE29=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp1575;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE29,(struct _dyneither_ptr*(*)(
struct _tuple25*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,((_tmp1575="function declaration has repeated parameter",
_tag_dyneither(_tmp1575,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp1576;fd->cached_typ=((
_tmp1576=_cycalloc(sizeof(*_tmp1576)),((_tmp1576->v=t,_tmp1576))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpE2C=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp1577;
struct Cyc_Tcutil_CVTEnv _tmpE2D=Cyc_Tcutil_check_valid_type(loc,te,((_tmp1577.r=
_tmpE2C,((_tmp1577.kind_env=bound_tvars,((_tmp1577.free_vars=0,((_tmp1577.free_evars=
0,((_tmp1577.generalize_evars=0,((_tmp1577.fn_result=0,_tmp1577)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE2E=Cyc_Tcutil_remove_bound_tvars(
_tmpE2C,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE2C,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple29*))Cyc_Core_fst,_tmpE2D.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE2F=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple30*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE2C,(void*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpE2D.free_evars);{struct Cyc_List_List*fs=_tmpE2E;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE30=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp157C;void*_tmp157B[2];struct Cyc_String_pa_struct _tmp157A;struct Cyc_String_pa_struct
_tmp1579;(_tmp1579.tag=0,((_tmp1579.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp157A.tag=0,((_tmp157A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE30),((_tmp157B[0]=& _tmp157A,((
_tmp157B[1]=& _tmp1579,Cyc_Tcutil_terr(loc,((_tmp157C="unbound type variable %s in type %s",
_tag_dyneither(_tmp157C,sizeof(char),36))),_tag_dyneither(_tmp157B,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE2F != 0)for(0;_tmpE2F != 0;_tmpE2F=
_tmpE2F->tl){void*e=(void*)_tmpE2F->hd;struct Cyc_Absyn_Kind*_tmpE35=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE36;enum Cyc_Absyn_KindQual _tmpE37;enum Cyc_Absyn_AliasQual
_tmpE38;struct Cyc_Absyn_Kind _tmpE39;enum Cyc_Absyn_KindQual _tmpE3A;enum Cyc_Absyn_AliasQual
_tmpE3B;struct Cyc_Absyn_Kind _tmpE3C;enum Cyc_Absyn_KindQual _tmpE3D;enum Cyc_Absyn_AliasQual
_tmpE3E;struct Cyc_Absyn_Kind _tmpE3F;enum Cyc_Absyn_KindQual _tmpE40;_LL6DD:
_tmpE36=*_tmpE35;_tmpE37=_tmpE36.kind;if(_tmpE37 != Cyc_Absyn_RgnKind)goto _LL6DF;
_tmpE38=_tmpE36.aliasqual;if(_tmpE38 != Cyc_Absyn_Unique)goto _LL6DF;_LL6DE: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp157F;void*
_tmp157E;(_tmp157E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp157F="can't unify evar with unique region!",
_tag_dyneither(_tmp157F,sizeof(char),37))),_tag_dyneither(_tmp157E,sizeof(void*),
0)));}goto _LL6DC;_LL6DF: _tmpE39=*_tmpE35;_tmpE3A=_tmpE39.kind;if(_tmpE3A != Cyc_Absyn_RgnKind)
goto _LL6E1;_tmpE3B=_tmpE39.aliasqual;if(_tmpE3B != Cyc_Absyn_Aliasable)goto _LL6E1;
_LL6E0: goto _LL6E2;_LL6E1: _tmpE3C=*_tmpE35;_tmpE3D=_tmpE3C.kind;if(_tmpE3D != Cyc_Absyn_RgnKind)
goto _LL6E3;_tmpE3E=_tmpE3C.aliasqual;if(_tmpE3E != Cyc_Absyn_Top)goto _LL6E3;
_LL6E2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1582;
void*_tmp1581;(_tmp1581=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1582="can't unify evar with heap!",_tag_dyneither(
_tmp1582,sizeof(char),28))),_tag_dyneither(_tmp1581,sizeof(void*),0)));}goto
_LL6DC;_LL6E3: _tmpE3F=*_tmpE35;_tmpE40=_tmpE3F.kind;if(_tmpE40 != Cyc_Absyn_EffKind)
goto _LL6E5;_LL6E4: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1585;void*_tmp1584;(_tmp1584=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1585="can't unify evar with {}!",
_tag_dyneither(_tmp1585,sizeof(char),26))),_tag_dyneither(_tmp1584,sizeof(void*),
0)));}goto _LL6DC;_LL6E5:;_LL6E6:{const char*_tmp158A;void*_tmp1589[2];struct Cyc_String_pa_struct
_tmp1588;struct Cyc_String_pa_struct _tmp1587;(_tmp1587.tag=0,((_tmp1587.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1588.tag=0,((_tmp1588.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1589[0]=& _tmp1588,((_tmp1589[1]=& _tmp1587,Cyc_Tcutil_terr(loc,((
_tmp158A="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp158A,sizeof(char),52))),_tag_dyneither(_tmp1589,sizeof(void*),2)))))))))))));}
goto _LL6DC;_LL6DC:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp158F;void*_tmp158E[2];struct Cyc_String_pa_struct
_tmp158D;struct Cyc_String_pa_struct _tmp158C;(_tmp158C.tag=0,((_tmp158C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp158D.tag=
0,((_tmp158D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp158E[0]=&
_tmp158D,((_tmp158E[1]=& _tmp158C,Cyc_Tcutil_terr(loc,((_tmp158F="%s: %s",
_tag_dyneither(_tmp158F,sizeof(char),7))),_tag_dyneither(_tmp158E,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp1590;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1590="duplicate type variable",
_tag_dyneither(_tmp1590,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp1591;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1591="",_tag_dyneither(_tmp1591,
sizeof(char),1))))!= 0){struct _tuple32*_tmp1594;struct Cyc_List_List*_tmp1593;
fields=((_tmp1593=_cycalloc(sizeof(*_tmp1593)),((_tmp1593->hd=((_tmp1594=
_cycalloc(sizeof(*_tmp1594)),((_tmp1594->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp1594->f2=0,_tmp1594)))))),((_tmp1593->tl=fields,_tmp1593))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp1596;const char*_tmp1595;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp1596="struct",_tag_dyneither(_tmp1596,sizeof(char),7)):((
_tmp1595="union",_tag_dyneither(_tmp1595,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple33 _tmpE55;struct Cyc_List_List*
_tmpE56;void*_tmpE57;struct _tuple33*_tmpE54=(struct _tuple33*)des->hd;_tmpE55=*
_tmpE54;_tmpE56=_tmpE55.f1;_tmpE57=_tmpE55.f2;if(_tmpE56 == 0){struct Cyc_List_List*
_tmpE58=fields;for(0;_tmpE58 != 0;_tmpE58=_tmpE58->tl){if(!(*((struct _tuple32*)
_tmpE58->hd)).f2){(*((struct _tuple32*)_tmpE58->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp159C;struct Cyc_Absyn_FieldName_struct _tmp159B;struct Cyc_List_List*_tmp159A;(*((
struct _tuple33*)des->hd)).f1=(struct Cyc_List_List*)((_tmp159A=_cycalloc(sizeof(*
_tmp159A)),((_tmp159A->hd=(void*)((_tmp159C=_cycalloc(sizeof(*_tmp159C)),((
_tmp159C[0]=((_tmp159B.tag=1,((_tmp159B.f1=((*((struct _tuple32*)_tmpE58->hd)).f1)->name,
_tmp159B)))),_tmp159C)))),((_tmp159A->tl=0,_tmp159A))))));}{struct _tuple34*
_tmp159F;struct Cyc_List_List*_tmp159E;ans=((_tmp159E=_region_malloc(rgn,sizeof(*
_tmp159E)),((_tmp159E->hd=((_tmp159F=_region_malloc(rgn,sizeof(*_tmp159F)),((
_tmp159F->f1=(*((struct _tuple32*)_tmpE58->hd)).f1,((_tmp159F->f2=_tmpE57,
_tmp159F)))))),((_tmp159E->tl=ans,_tmp159E))))));}break;}}if(_tmpE58 == 0){const
char*_tmp15A3;void*_tmp15A2[1];struct Cyc_String_pa_struct _tmp15A1;(_tmp15A1.tag=
0,((_tmp15A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15A2[0]=& _tmp15A1,Cyc_Tcutil_terr(loc,((_tmp15A3="too many arguments to %s",
_tag_dyneither(_tmp15A3,sizeof(char),25))),_tag_dyneither(_tmp15A2,sizeof(void*),
1)))))));}}else{if(_tmpE56->tl != 0){const char*_tmp15A6;void*_tmp15A5;(_tmp15A5=0,
Cyc_Tcutil_terr(loc,((_tmp15A6="multiple designators are not yet supported",
_tag_dyneither(_tmp15A6,sizeof(char),43))),_tag_dyneither(_tmp15A5,sizeof(void*),
0)));}else{void*_tmpE63=(void*)_tmpE56->hd;struct _dyneither_ptr*_tmpE66;_LL6E8: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE64=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE63;if(_tmpE64->tag != 0)goto _LL6EA;}_LL6E9:{const char*_tmp15AA;void*_tmp15A9[
1];struct Cyc_String_pa_struct _tmp15A8;(_tmp15A8.tag=0,((_tmp15A8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15A9[0]=& _tmp15A8,Cyc_Tcutil_terr(
loc,((_tmp15AA="array designator used in argument to %s",_tag_dyneither(_tmp15AA,
sizeof(char),40))),_tag_dyneither(_tmp15A9,sizeof(void*),1)))))));}goto _LL6E7;
_LL6EA: {struct Cyc_Absyn_FieldName_struct*_tmpE65=(struct Cyc_Absyn_FieldName_struct*)
_tmpE63;if(_tmpE65->tag != 1)goto _LL6E7;else{_tmpE66=_tmpE65->f1;}}_LL6EB: {
struct Cyc_List_List*_tmpE6A=fields;for(0;_tmpE6A != 0;_tmpE6A=_tmpE6A->tl){if(Cyc_strptrcmp(
_tmpE66,((*((struct _tuple32*)_tmpE6A->hd)).f1)->name)== 0){if((*((struct _tuple32*)
_tmpE6A->hd)).f2){const char*_tmp15AE;void*_tmp15AD[1];struct Cyc_String_pa_struct
_tmp15AC;(_tmp15AC.tag=0,((_tmp15AC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE66),((_tmp15AD[0]=& _tmp15AC,Cyc_Tcutil_terr(loc,((_tmp15AE="member %s has already been used as an argument",
_tag_dyneither(_tmp15AE,sizeof(char),47))),_tag_dyneither(_tmp15AD,sizeof(void*),
1)))))));}(*((struct _tuple32*)_tmpE6A->hd)).f2=1;{struct _tuple34*_tmp15B1;struct
Cyc_List_List*_tmp15B0;ans=((_tmp15B0=_region_malloc(rgn,sizeof(*_tmp15B0)),((
_tmp15B0->hd=((_tmp15B1=_region_malloc(rgn,sizeof(*_tmp15B1)),((_tmp15B1->f1=(*((
struct _tuple32*)_tmpE6A->hd)).f1,((_tmp15B1->f2=_tmpE57,_tmp15B1)))))),((
_tmp15B0->tl=ans,_tmp15B0))))));}break;}}if(_tmpE6A == 0){const char*_tmp15B5;void*
_tmp15B4[1];struct Cyc_String_pa_struct _tmp15B3;(_tmp15B3.tag=0,((_tmp15B3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE66),((_tmp15B4[0]=& _tmp15B3,
Cyc_Tcutil_terr(loc,((_tmp15B5="bad field designator %s",_tag_dyneither(_tmp15B5,
sizeof(char),24))),_tag_dyneither(_tmp15B4,sizeof(void*),1)))))));}goto _LL6E7;}
_LL6E7:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){{const char*_tmp15B8;void*_tmp15B7;(
_tmp15B7=0,Cyc_Tcutil_terr(loc,((_tmp15B8="too few arguments to struct",
_tag_dyneither(_tmp15B8,sizeof(char),28))),_tag_dyneither(_tmp15B7,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple32*)fields->hd)).f2){if(found){const char*_tmp15BB;void*_tmp15BA;(_tmp15BA=
0,Cyc_Tcutil_terr(loc,((_tmp15BB="only one member of a union is allowed",
_tag_dyneither(_tmp15BB,sizeof(char),38))),_tag_dyneither(_tmp15BA,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15BE;void*_tmp15BD;(_tmp15BD=0,Cyc_Tcutil_terr(
loc,((_tmp15BE="missing member for union",_tag_dyneither(_tmp15BE,sizeof(char),
25))),_tag_dyneither(_tmp15BD,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE7B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE7D;void*_tmpE7E;struct Cyc_Absyn_PtrAtts _tmpE7F;union Cyc_Absyn_Constraint*
_tmpE80;_LL6ED: {struct Cyc_Absyn_PointerType_struct*_tmpE7C=(struct Cyc_Absyn_PointerType_struct*)
_tmpE7B;if(_tmpE7C->tag != 5)goto _LL6EF;else{_tmpE7D=_tmpE7C->f1;_tmpE7E=_tmpE7D.elt_typ;
_tmpE7F=_tmpE7D.ptr_atts;_tmpE80=_tmpE7F.bounds;}}_LL6EE: {void*_tmpE81=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE80);_LL6F2: {struct Cyc_Absyn_DynEither_b_struct*_tmpE82=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE81;if(_tmpE82->tag != 0)goto _LL6F4;}_LL6F3:*elt_typ_dest=_tmpE7E;return 1;
_LL6F4:;_LL6F5: return 0;_LL6F1:;}_LL6EF:;_LL6F0: return 0;_LL6EC:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE83=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE85;void*_tmpE86;struct Cyc_Absyn_PtrAtts _tmpE87;union Cyc_Absyn_Constraint*
_tmpE88;_LL6F7: {struct Cyc_Absyn_PointerType_struct*_tmpE84=(struct Cyc_Absyn_PointerType_struct*)
_tmpE83;if(_tmpE84->tag != 5)goto _LL6F9;else{_tmpE85=_tmpE84->f1;_tmpE86=_tmpE85.elt_typ;
_tmpE87=_tmpE85.ptr_atts;_tmpE88=_tmpE87.zero_term;}}_LL6F8:*elt_typ_dest=
_tmpE86;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE88);_LL6F9:;_LL6FA: return 0;_LL6F6:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE89=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE8B;void*_tmpE8C;struct Cyc_Absyn_PtrAtts _tmpE8D;
union Cyc_Absyn_Constraint*_tmpE8E;union Cyc_Absyn_Constraint*_tmpE8F;struct Cyc_Absyn_ArrayInfo
_tmpE91;void*_tmpE92;struct Cyc_Absyn_Tqual _tmpE93;struct Cyc_Absyn_Exp*_tmpE94;
union Cyc_Absyn_Constraint*_tmpE95;_LL6FC: {struct Cyc_Absyn_PointerType_struct*
_tmpE8A=(struct Cyc_Absyn_PointerType_struct*)_tmpE89;if(_tmpE8A->tag != 5)goto
_LL6FE;else{_tmpE8B=_tmpE8A->f1;_tmpE8C=_tmpE8B.elt_typ;_tmpE8D=_tmpE8B.ptr_atts;
_tmpE8E=_tmpE8D.bounds;_tmpE8F=_tmpE8D.zero_term;}}_LL6FD: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE8F)){*ptr_type=t;*elt_type=
_tmpE8C;{void*_tmpE96=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE8E);_LL703: {struct Cyc_Absyn_DynEither_b_struct*
_tmpE97=(struct Cyc_Absyn_DynEither_b_struct*)_tmpE96;if(_tmpE97->tag != 0)goto
_LL705;}_LL704:*is_dyneither=1;goto _LL702;_LL705:;_LL706:*is_dyneither=0;goto
_LL702;_LL702:;}return 1;}else{return 0;}_LL6FE: {struct Cyc_Absyn_ArrayType_struct*
_tmpE90=(struct Cyc_Absyn_ArrayType_struct*)_tmpE89;if(_tmpE90->tag != 9)goto
_LL700;else{_tmpE91=_tmpE90->f1;_tmpE92=_tmpE91.elt_type;_tmpE93=_tmpE91.tq;
_tmpE94=_tmpE91.num_elts;_tmpE95=_tmpE91.zero_term;}}_LL6FF: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE95)){*elt_type=_tmpE92;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp15D3;struct Cyc_Absyn_PtrAtts
_tmp15D2;struct Cyc_Absyn_Upper_b_struct _tmp15D1;struct Cyc_Absyn_Upper_b_struct*
_tmp15D0;struct Cyc_Absyn_PtrInfo _tmp15CF;struct Cyc_Absyn_PointerType_struct*
_tmp15CE;*ptr_type=(void*)((_tmp15CE=_cycalloc(sizeof(*_tmp15CE)),((_tmp15CE[0]=((
_tmp15D3.tag=5,((_tmp15D3.f1=((_tmp15CF.elt_typ=_tmpE92,((_tmp15CF.elt_tq=
_tmpE93,((_tmp15CF.ptr_atts=((_tmp15D2.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp15D2.nullable=Cyc_Absyn_false_conref,((_tmp15D2.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp15D0=_cycalloc(sizeof(*_tmp15D0)),((
_tmp15D0[0]=((_tmp15D1.tag=1,((_tmp15D1.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpE94),_tmp15D1)))),_tmp15D0))))),((_tmp15D2.zero_term=_tmpE95,((_tmp15D2.ptrloc=
0,_tmp15D2)))))))))),_tmp15CF)))))),_tmp15D3)))),_tmp15CE))));}return 1;}else{
return 0;}_LL700:;_LL701: return 0;_LL6FB:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE9E=e1->r;
struct Cyc_Absyn_Exp*_tmpEA1;struct Cyc_Absyn_Exp*_tmpEA3;struct Cyc_Absyn_Exp*
_tmpEA5;struct Cyc_Absyn_Exp*_tmpEA7;struct Cyc_Absyn_Exp*_tmpEA9;struct Cyc_Absyn_Exp*
_tmpEAB;_LL708: {struct Cyc_Absyn_Cast_e_struct*_tmpE9F=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE9E;if(_tmpE9F->tag != 15)goto _LL70A;}_LL709: {const char*_tmp15D7;void*
_tmp15D6[1];struct Cyc_String_pa_struct _tmp15D5;(_tmp15D5.tag=0,((_tmp15D5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp15D6[0]=& _tmp15D5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15D7="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp15D7,sizeof(char),29))),_tag_dyneither(_tmp15D6,sizeof(void*),1)))))));}
_LL70A: {struct Cyc_Absyn_Deref_e_struct*_tmpEA0=(struct Cyc_Absyn_Deref_e_struct*)
_tmpE9E;if(_tmpEA0->tag != 21)goto _LL70C;else{_tmpEA1=_tmpEA0->f1;}}_LL70B:
_tmpEA3=_tmpEA1;goto _LL70D;_LL70C: {struct Cyc_Absyn_Subscript_e_struct*_tmpEA2=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpE9E;if(_tmpEA2->tag != 24)goto _LL70E;
else{_tmpEA3=_tmpEA2->f1;}}_LL70D: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEA3->topt))->v,ptr_type,is_dyneither,
elt_type);_LL70E: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEA4=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE9E;if(_tmpEA4->tag != 23)goto _LL710;else{_tmpEA5=_tmpEA4->f1;}}_LL70F:
_tmpEA7=_tmpEA5;goto _LL711;_LL710: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEA6=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpE9E;if(_tmpEA6->tag != 22)goto _LL712;
else{_tmpEA7=_tmpEA6->f1;}}_LL711: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpEA7->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp15DB;void*_tmp15DA[1];struct Cyc_String_pa_struct _tmp15D9;(_tmp15D9.tag=
0,((_tmp15D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15DA[0]=& _tmp15D9,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15DB="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp15DB,sizeof(char),51))),_tag_dyneither(_tmp15DA,sizeof(void*),
1)))))));}return 0;_LL712: {struct Cyc_Absyn_Instantiate_e_struct*_tmpEA8=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpE9E;if(_tmpEA8->tag != 14)goto _LL714;else{
_tmpEA9=_tmpEA8->f1;}}_LL713: _tmpEAB=_tmpEA9;goto _LL715;_LL714: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpEAA=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE9E;if(_tmpEAA->tag != 13)
goto _LL716;else{_tmpEAB=_tmpEAA->f1;}}_LL715: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEAB->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp15DF;void*_tmp15DE[1];struct Cyc_String_pa_struct
_tmp15DD;(_tmp15DD.tag=0,((_tmp15DD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp15DE[0]=& _tmp15DD,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15DF="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp15DF,sizeof(char),49))),_tag_dyneither(_tmp15DE,sizeof(void*),
1)))))));}return 0;_LL716: {struct Cyc_Absyn_Var_e_struct*_tmpEAC=(struct Cyc_Absyn_Var_e_struct*)
_tmpE9E;if(_tmpEAC->tag != 1)goto _LL718;}_LL717: return 0;_LL718:;_LL719: {const
char*_tmp15E3;void*_tmp15E2[1];struct Cyc_String_pa_struct _tmp15E1;(_tmp15E1.tag=
0,((_tmp15E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15E2[0]=& _tmp15E1,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E3="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp15E3,sizeof(char),39))),_tag_dyneither(_tmp15E2,sizeof(void*),
1)))))));}_LL707:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
static int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpEB9=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpEBC;_LL71B: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpEBA=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpEB9;if(_tmpEBA->tag != 22)goto
_LL71D;}_LL71C: return 1;_LL71D: {struct Cyc_Absyn_VarType_struct*_tmpEBB=(struct
Cyc_Absyn_VarType_struct*)_tmpEB9;if(_tmpEBB->tag != 2)goto _LL71F;else{_tmpEBC=
_tmpEBB->f1;}}_LL71E: {struct Cyc_Absyn_Kind _tmpEBE;enum Cyc_Absyn_KindQual
_tmpEBF;enum Cyc_Absyn_AliasQual _tmpEC0;struct Cyc_Absyn_Kind*_tmpEBD=Cyc_Tcutil_tvar_kind(
_tmpEBC,& Cyc_Tcutil_rk);_tmpEBE=*_tmpEBD;_tmpEBF=_tmpEBE.kind;_tmpEC0=_tmpEBE.aliasqual;
if(_tmpEBF == Cyc_Absyn_RgnKind  && (_tmpEC0 == Cyc_Absyn_Unique  || _tmpEC0 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpEC1=Cyc_Absyn_compress_kb(_tmpEBC->kind);struct
Cyc_Core_Opt*_tmpEC3;struct Cyc_Core_Opt**_tmpEC4;struct Cyc_Absyn_Kind*_tmpEC5;
struct Cyc_Absyn_Kind _tmpEC6;enum Cyc_Absyn_KindQual _tmpEC7;enum Cyc_Absyn_AliasQual
_tmpEC8;_LL722: {struct Cyc_Absyn_Less_kb_struct*_tmpEC2=(struct Cyc_Absyn_Less_kb_struct*)
_tmpEC1;if(_tmpEC2->tag != 2)goto _LL724;else{_tmpEC3=_tmpEC2->f1;_tmpEC4=(struct
Cyc_Core_Opt**)& _tmpEC2->f1;_tmpEC5=_tmpEC2->f2;_tmpEC6=*_tmpEC5;_tmpEC7=_tmpEC6.kind;
if(_tmpEC7 != Cyc_Absyn_RgnKind)goto _LL724;_tmpEC8=_tmpEC6.aliasqual;if(_tmpEC8 != 
Cyc_Absyn_Top)goto _LL724;}}_LL723:{struct Cyc_Absyn_Less_kb_struct*_tmp15E9;
struct Cyc_Absyn_Less_kb_struct _tmp15E8;struct Cyc_Core_Opt*_tmp15E7;*_tmpEC4=((
_tmp15E7=_cycalloc(sizeof(*_tmp15E7)),((_tmp15E7->v=(void*)((_tmp15E9=_cycalloc(
sizeof(*_tmp15E9)),((_tmp15E9[0]=((_tmp15E8.tag=2,((_tmp15E8.f1=0,((_tmp15E8.f2=&
Cyc_Tcutil_rk,_tmp15E8)))))),_tmp15E9)))),_tmp15E7))));}return 0;_LL724:;_LL725:
return 1;_LL721:;}return 0;}_LL71F:;_LL720: return 0;_LL71A:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpECC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpECE;struct Cyc_Absyn_PtrAtts _tmpECF;void*_tmpED0;struct Cyc_Absyn_Tvar*_tmpED2;
_LL727: {struct Cyc_Absyn_PointerType_struct*_tmpECD=(struct Cyc_Absyn_PointerType_struct*)
_tmpECC;if(_tmpECD->tag != 5)goto _LL729;else{_tmpECE=_tmpECD->f1;_tmpECF=_tmpECE.ptr_atts;
_tmpED0=_tmpECF.rgn;}}_LL728: return Cyc_Tcutil_is_noalias_region(_tmpED0,
must_be_unique);_LL729: {struct Cyc_Absyn_VarType_struct*_tmpED1=(struct Cyc_Absyn_VarType_struct*)
_tmpECC;if(_tmpED1->tag != 2)goto _LL72B;else{_tmpED2=_tmpED1->f1;}}_LL72A: {
struct Cyc_Absyn_Kind _tmpED4;enum Cyc_Absyn_KindQual _tmpED5;enum Cyc_Absyn_AliasQual
_tmpED6;struct Cyc_Absyn_Kind*_tmpED3=Cyc_Tcutil_tvar_kind(_tmpED2,& Cyc_Tcutil_bk);
_tmpED4=*_tmpED3;_tmpED5=_tmpED4.kind;_tmpED6=_tmpED4.aliasqual;switch(_tmpED5){
case Cyc_Absyn_BoxKind: _LL72D: goto _LL72E;case Cyc_Absyn_AnyKind: _LL72E: goto _LL72F;
case Cyc_Absyn_MemKind: _LL72F: if(_tmpED6 == Cyc_Absyn_Unique  || _tmpED6 == Cyc_Absyn_Top){
void*_tmpED7=Cyc_Absyn_compress_kb(_tmpED2->kind);struct Cyc_Core_Opt*_tmpED9;
struct Cyc_Core_Opt**_tmpEDA;struct Cyc_Absyn_Kind*_tmpEDB;struct Cyc_Absyn_Kind
_tmpEDC;enum Cyc_Absyn_KindQual _tmpEDD;enum Cyc_Absyn_AliasQual _tmpEDE;_LL732: {
struct Cyc_Absyn_Less_kb_struct*_tmpED8=(struct Cyc_Absyn_Less_kb_struct*)_tmpED7;
if(_tmpED8->tag != 2)goto _LL734;else{_tmpED9=_tmpED8->f1;_tmpEDA=(struct Cyc_Core_Opt**)&
_tmpED8->f1;_tmpEDB=_tmpED8->f2;_tmpEDC=*_tmpEDB;_tmpEDD=_tmpEDC.kind;_tmpEDE=
_tmpEDC.aliasqual;if(_tmpEDE != Cyc_Absyn_Top)goto _LL734;}}_LL733:{struct Cyc_Absyn_Less_kb_struct*
_tmp15F3;struct Cyc_Absyn_Kind*_tmp15F2;struct Cyc_Absyn_Less_kb_struct _tmp15F1;
struct Cyc_Core_Opt*_tmp15F0;*_tmpEDA=((_tmp15F0=_cycalloc(sizeof(*_tmp15F0)),((
_tmp15F0->v=(void*)((_tmp15F3=_cycalloc(sizeof(*_tmp15F3)),((_tmp15F3[0]=((
_tmp15F1.tag=2,((_tmp15F1.f1=0,((_tmp15F1.f2=((_tmp15F2=_cycalloc_atomic(sizeof(*
_tmp15F2)),((_tmp15F2->kind=_tmpEDD,((_tmp15F2->aliasqual=Cyc_Absyn_Aliasable,
_tmp15F2)))))),_tmp15F1)))))),_tmp15F3)))),_tmp15F0))));}return 0;_LL734:;_LL735:
return 1;_LL731:;}return 0;default: _LL730: return 0;}}_LL72B:;_LL72C: return 0;_LL726:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpEE3=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpEE3))
return 1;{void*_tmpEE4=_tmpEE3;struct Cyc_List_List*_tmpEE6;struct Cyc_Absyn_AggrInfo
_tmpEE8;union Cyc_Absyn_AggrInfoU _tmpEE9;struct Cyc_Absyn_Aggrdecl**_tmpEEA;struct
Cyc_List_List*_tmpEEB;struct Cyc_List_List*_tmpEED;struct Cyc_Absyn_AggrInfo
_tmpEEF;union Cyc_Absyn_AggrInfoU _tmpEF0;struct _tuple4 _tmpEF1;struct Cyc_Absyn_DatatypeInfo
_tmpEF3;union Cyc_Absyn_DatatypeInfoU _tmpEF4;struct Cyc_List_List*_tmpEF5;struct
Cyc_Absyn_DatatypeFieldInfo _tmpEF7;union Cyc_Absyn_DatatypeFieldInfoU _tmpEF8;
struct Cyc_List_List*_tmpEF9;_LL738: {struct Cyc_Absyn_TupleType_struct*_tmpEE5=(
struct Cyc_Absyn_TupleType_struct*)_tmpEE4;if(_tmpEE5->tag != 11)goto _LL73A;else{
_tmpEE6=_tmpEE5->f1;}}_LL739: while(_tmpEE6 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpEE6->hd)).f2))return 1;_tmpEE6=_tmpEE6->tl;}return 0;
_LL73A: {struct Cyc_Absyn_AggrType_struct*_tmpEE7=(struct Cyc_Absyn_AggrType_struct*)
_tmpEE4;if(_tmpEE7->tag != 12)goto _LL73C;else{_tmpEE8=_tmpEE7->f1;_tmpEE9=_tmpEE8.aggr_info;
if((_tmpEE9.KnownAggr).tag != 2)goto _LL73C;_tmpEEA=(struct Cyc_Absyn_Aggrdecl**)(
_tmpEE9.KnownAggr).val;_tmpEEB=_tmpEE8.targs;}}_LL73B: if((*_tmpEEA)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpEFA=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpEEA)->tvs,_tmpEEB);struct Cyc_List_List*_tmpEFB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpEEA)->impl))->fields;void*t;
while(_tmpEFB != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpEFA,((struct Cyc_Absyn_Aggrfield*)
_tmpEFB->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpEFB=_tmpEFB->tl;}return 0;}_LL73C: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpEEC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpEE4;if(_tmpEEC->tag != 13)goto
_LL73E;else{_tmpEED=_tmpEEC->f2;}}_LL73D: while(_tmpEED != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpEED->hd)->type))return 1;_tmpEED=_tmpEED->tl;}
return 0;_LL73E: {struct Cyc_Absyn_AggrType_struct*_tmpEEE=(struct Cyc_Absyn_AggrType_struct*)
_tmpEE4;if(_tmpEEE->tag != 12)goto _LL740;else{_tmpEEF=_tmpEEE->f1;_tmpEF0=_tmpEEF.aggr_info;
if((_tmpEF0.UnknownAggr).tag != 1)goto _LL740;_tmpEF1=(struct _tuple4)(_tmpEF0.UnknownAggr).val;}}
_LL73F: {const char*_tmp15F6;void*_tmp15F5;(_tmp15F5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15F6="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp15F6,sizeof(char),36))),_tag_dyneither(_tmp15F5,sizeof(void*),
0)));}_LL740: {struct Cyc_Absyn_DatatypeType_struct*_tmpEF2=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpEE4;if(_tmpEF2->tag != 3)goto _LL742;else{_tmpEF3=_tmpEF2->f1;_tmpEF4=_tmpEF3.datatype_info;
_tmpEF5=_tmpEF3.targs;}}_LL741: {union Cyc_Absyn_DatatypeInfoU _tmpEFE=_tmpEF4;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpEFF;struct _tuple2*_tmpF00;int _tmpF01;
struct Cyc_Absyn_Datatypedecl**_tmpF02;struct Cyc_Absyn_Datatypedecl*_tmpF03;
struct Cyc_Absyn_Datatypedecl _tmpF04;struct Cyc_List_List*_tmpF05;struct Cyc_Core_Opt*
_tmpF06;_LL747: if((_tmpEFE.UnknownDatatype).tag != 1)goto _LL749;_tmpEFF=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpEFE.UnknownDatatype).val;_tmpF00=_tmpEFF.name;
_tmpF01=_tmpEFF.is_extensible;_LL748: {const char*_tmp15F9;void*_tmp15F8;(
_tmp15F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp15F9="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp15F9,
sizeof(char),40))),_tag_dyneither(_tmp15F8,sizeof(void*),0)));}_LL749: if((
_tmpEFE.KnownDatatype).tag != 2)goto _LL746;_tmpF02=(struct Cyc_Absyn_Datatypedecl**)(
_tmpEFE.KnownDatatype).val;_tmpF03=*_tmpF02;_tmpF04=*_tmpF03;_tmpF05=_tmpF04.tvs;
_tmpF06=_tmpF04.fields;_LL74A: return 0;_LL746:;}_LL742: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpEF6=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpEE4;if(_tmpEF6->tag != 4)
goto _LL744;else{_tmpEF7=_tmpEF6->f1;_tmpEF8=_tmpEF7.field_info;_tmpEF9=_tmpEF7.targs;}}
_LL743: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF09=_tmpEF8;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF0A;struct _tuple3 _tmpF0B;struct Cyc_Absyn_Datatypedecl*_tmpF0C;struct Cyc_Absyn_Datatypefield*
_tmpF0D;_LL74C: if((_tmpF09.UnknownDatatypefield).tag != 1)goto _LL74E;_tmpF0A=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF09.UnknownDatatypefield).val;
_LL74D: {const char*_tmp15FC;void*_tmp15FB;(_tmp15FB=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15FC="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp15FC,sizeof(char),46))),_tag_dyneither(_tmp15FB,sizeof(void*),
0)));}_LL74E: if((_tmpF09.KnownDatatypefield).tag != 2)goto _LL74B;_tmpF0B=(struct
_tuple3)(_tmpF09.KnownDatatypefield).val;_tmpF0C=_tmpF0B.f1;_tmpF0D=_tmpF0B.f2;
_LL74F: {struct Cyc_List_List*_tmpF10=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF0C->tvs,_tmpEF9);struct Cyc_List_List*_tmpF11=
_tmpF0D->typs;while(_tmpF11 != 0){_tmpEE3=Cyc_Tcutil_rsubstitute(rgn,_tmpF10,(*((
struct _tuple11*)_tmpF11->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpEE3))return 1;_tmpF11=_tmpF11->tl;}return 0;}_LL74B:;}_LL744:;_LL745: return 0;
_LL737:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF12=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF14;union Cyc_Absyn_AggrInfoU _tmpF15;struct Cyc_Absyn_Aggrdecl**_tmpF16;struct
Cyc_Absyn_Aggrdecl*_tmpF17;struct Cyc_List_List*_tmpF18;struct Cyc_List_List*
_tmpF1A;_LL751: {struct Cyc_Absyn_AggrType_struct*_tmpF13=(struct Cyc_Absyn_AggrType_struct*)
_tmpF12;if(_tmpF13->tag != 12)goto _LL753;else{_tmpF14=_tmpF13->f1;_tmpF15=_tmpF14.aggr_info;
if((_tmpF15.KnownAggr).tag != 2)goto _LL753;_tmpF16=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF15.KnownAggr).val;_tmpF17=*_tmpF16;_tmpF18=_tmpF14.targs;}}_LL752: _tmpF1A=
_tmpF17->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF17->impl))->fields;
goto _LL754;_LL753: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF19=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF12;if(_tmpF19->tag != 13)goto _LL755;else{_tmpF1A=_tmpF19->f2;}}_LL754: {
struct Cyc_Absyn_Aggrfield*_tmpF1B=Cyc_Absyn_lookup_field(_tmpF1A,f);{struct Cyc_Absyn_Aggrfield*
_tmpF1C=_tmpF1B;struct Cyc_Absyn_Aggrfield _tmpF1D;void*_tmpF1E;_LL758: if(_tmpF1C
!= 0)goto _LL75A;_LL759: {const char*_tmp15FF;void*_tmp15FE;(_tmp15FE=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15FF="is_noalias_field: missing field",
_tag_dyneither(_tmp15FF,sizeof(char),32))),_tag_dyneither(_tmp15FE,sizeof(void*),
0)));}_LL75A: if(_tmpF1C == 0)goto _LL757;_tmpF1D=*_tmpF1C;_tmpF1E=_tmpF1D.type;
_LL75B: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF1E);_LL757:;}return 0;}
_LL755:;_LL756: {const char*_tmp1603;void*_tmp1602[1];struct Cyc_String_pa_struct
_tmp1601;(_tmp1601.tag=0,((_tmp1601.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1602[0]=& _tmp1601,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1603="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1603,sizeof(char),36))),_tag_dyneither(_tmp1602,sizeof(void*),
1)))))));}_LL750:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF24=e->r;void*_tmpF26;struct Cyc_Absyn_Exp*
_tmpF29;struct Cyc_Absyn_Exp*_tmpF2B;struct Cyc_Absyn_Exp*_tmpF2D;struct
_dyneither_ptr*_tmpF2E;struct Cyc_Absyn_Exp*_tmpF30;struct Cyc_Absyn_Exp*_tmpF31;
struct Cyc_Absyn_Exp*_tmpF33;struct Cyc_Absyn_Exp*_tmpF34;struct Cyc_Absyn_Exp*
_tmpF36;struct Cyc_Absyn_Exp*_tmpF38;struct Cyc_Absyn_Stmt*_tmpF3A;_LL75D: {struct
Cyc_Absyn_Var_e_struct*_tmpF25=(struct Cyc_Absyn_Var_e_struct*)_tmpF24;if(_tmpF25->tag
!= 1)goto _LL75F;else{_tmpF26=(void*)_tmpF25->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF27=(struct Cyc_Absyn_Global_b_struct*)_tmpF26;if(_tmpF27->tag != 1)goto _LL75F;};}}
_LL75E: return 0;_LL75F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF28=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF24;if(_tmpF28->tag != 23)goto _LL761;else{_tmpF29=_tmpF28->f1;}}_LL760:
_tmpF2B=_tmpF29;goto _LL762;_LL761: {struct Cyc_Absyn_Deref_e_struct*_tmpF2A=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF24;if(_tmpF2A->tag != 21)goto _LL763;else{
_tmpF2B=_tmpF2A->f1;}}_LL762: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF2B->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF2B);_LL763: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF2C=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF24;if(_tmpF2C->tag != 22)goto _LL765;else{_tmpF2D=_tmpF2C->f1;_tmpF2E=_tmpF2C->f2;}}
_LL764: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF2D);_LL765: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF2F=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF24;if(_tmpF2F->tag != 24)goto
_LL767;else{_tmpF30=_tmpF2F->f1;_tmpF31=_tmpF2F->f2;}}_LL766: {void*_tmpF3B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF30->topt))->v);_LL772: {struct Cyc_Absyn_TupleType_struct*
_tmpF3C=(struct Cyc_Absyn_TupleType_struct*)_tmpF3B;if(_tmpF3C->tag != 11)goto
_LL774;}_LL773: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF30);_LL774:;_LL775:
return 0;_LL771:;}_LL767: {struct Cyc_Absyn_Conditional_e_struct*_tmpF32=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF24;if(_tmpF32->tag != 6)goto _LL769;else{
_tmpF33=_tmpF32->f2;_tmpF34=_tmpF32->f3;}}_LL768: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF33) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF34);_LL769: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF35=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF24;if(_tmpF35->tag != 9)goto _LL76B;
else{_tmpF36=_tmpF35->f2;}}_LL76A: _tmpF38=_tmpF36;goto _LL76C;_LL76B: {struct Cyc_Absyn_Cast_e_struct*
_tmpF37=(struct Cyc_Absyn_Cast_e_struct*)_tmpF24;if(_tmpF37->tag != 15)goto _LL76D;
else{_tmpF38=_tmpF37->f2;}}_LL76C: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF38);
_LL76D: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF39=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF24;if(_tmpF39->tag != 37)goto _LL76F;else{_tmpF3A=_tmpF39->f1;}}_LL76E: while(
1){void*_tmpF3D=_tmpF3A->r;struct Cyc_Absyn_Stmt*_tmpF3F;struct Cyc_Absyn_Stmt*
_tmpF40;struct Cyc_Absyn_Decl*_tmpF42;struct Cyc_Absyn_Stmt*_tmpF43;struct Cyc_Absyn_Exp*
_tmpF45;_LL777: {struct Cyc_Absyn_Seq_s_struct*_tmpF3E=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF3D;if(_tmpF3E->tag != 2)goto _LL779;else{_tmpF3F=_tmpF3E->f1;_tmpF40=_tmpF3E->f2;}}
_LL778: _tmpF3A=_tmpF40;goto _LL776;_LL779: {struct Cyc_Absyn_Decl_s_struct*_tmpF41=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF3D;if(_tmpF41->tag != 12)goto _LL77B;else{
_tmpF42=_tmpF41->f1;_tmpF43=_tmpF41->f2;}}_LL77A: _tmpF3A=_tmpF43;goto _LL776;
_LL77B: {struct Cyc_Absyn_Exp_s_struct*_tmpF44=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF3D;if(_tmpF44->tag != 1)goto _LL77D;else{_tmpF45=_tmpF44->f1;}}_LL77C: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF45);_LL77D:;_LL77E: {const char*_tmp1606;
void*_tmp1605;(_tmp1605=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1606="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1606,sizeof(char),40))),_tag_dyneither(_tmp1605,sizeof(void*),
0)));}_LL776:;}_LL76F:;_LL770: return 1;_LL75C:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp1607;struct _tuple17 bogus_ans=(_tmp1607.f1=0,((_tmp1607.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1607)));void*_tmpF48=e->r;struct _tuple2*_tmpF4A;
void*_tmpF4B;struct Cyc_Absyn_Exp*_tmpF4D;struct _dyneither_ptr*_tmpF4E;int _tmpF4F;
struct Cyc_Absyn_Exp*_tmpF51;struct _dyneither_ptr*_tmpF52;int _tmpF53;struct Cyc_Absyn_Exp*
_tmpF55;struct Cyc_Absyn_Exp*_tmpF57;struct Cyc_Absyn_Exp*_tmpF58;_LL780: {struct
Cyc_Absyn_Var_e_struct*_tmpF49=(struct Cyc_Absyn_Var_e_struct*)_tmpF48;if(_tmpF49->tag
!= 1)goto _LL782;else{_tmpF4A=_tmpF49->f1;_tmpF4B=(void*)_tmpF49->f2;}}_LL781: {
void*_tmpF59=_tmpF4B;struct Cyc_Absyn_Vardecl*_tmpF5D;struct Cyc_Absyn_Vardecl*
_tmpF5F;struct Cyc_Absyn_Vardecl*_tmpF61;struct Cyc_Absyn_Vardecl*_tmpF63;_LL78D: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF5A=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF59;if(_tmpF5A->tag != 0)goto _LL78F;}_LL78E: goto _LL790;_LL78F: {struct Cyc_Absyn_Funname_b_struct*
_tmpF5B=(struct Cyc_Absyn_Funname_b_struct*)_tmpF59;if(_tmpF5B->tag != 2)goto
_LL791;}_LL790: return bogus_ans;_LL791: {struct Cyc_Absyn_Global_b_struct*_tmpF5C=(
struct Cyc_Absyn_Global_b_struct*)_tmpF59;if(_tmpF5C->tag != 1)goto _LL793;else{
_tmpF5D=_tmpF5C->f1;}}_LL792: {void*_tmpF64=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL79A: {struct Cyc_Absyn_ArrayType_struct*
_tmpF65=(struct Cyc_Absyn_ArrayType_struct*)_tmpF64;if(_tmpF65->tag != 9)goto
_LL79C;}_LL79B: {struct _tuple17 _tmp1608;return(_tmp1608.f1=1,((_tmp1608.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp1608)));}_LL79C:;_LL79D: {struct _tuple17 _tmp1609;
return(_tmp1609.f1=(_tmpF5D->tq).real_const,((_tmp1609.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1609)));}_LL799:;}_LL793: {struct Cyc_Absyn_Local_b_struct*_tmpF5E=(struct Cyc_Absyn_Local_b_struct*)
_tmpF59;if(_tmpF5E->tag != 4)goto _LL795;else{_tmpF5F=_tmpF5E->f1;}}_LL794: {void*
_tmpF68=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL79F: {struct Cyc_Absyn_ArrayType_struct*_tmpF69=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF68;if(_tmpF69->tag != 9)goto _LL7A1;}_LL7A0: {struct _tuple17 _tmp160A;return(
_tmp160A.f1=1,((_tmp160A.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF5F->rgn))->v,
_tmp160A)));}_LL7A1:;_LL7A2: _tmpF5F->escapes=1;{struct _tuple17 _tmp160B;return(
_tmp160B.f1=(_tmpF5F->tq).real_const,((_tmp160B.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF5F->rgn))->v,_tmp160B)));};_LL79E:;}_LL795: {struct Cyc_Absyn_Pat_b_struct*
_tmpF60=(struct Cyc_Absyn_Pat_b_struct*)_tmpF59;if(_tmpF60->tag != 5)goto _LL797;
else{_tmpF61=_tmpF60->f1;}}_LL796: _tmpF63=_tmpF61;goto _LL798;_LL797: {struct Cyc_Absyn_Param_b_struct*
_tmpF62=(struct Cyc_Absyn_Param_b_struct*)_tmpF59;if(_tmpF62->tag != 3)goto _LL78C;
else{_tmpF63=_tmpF62->f1;}}_LL798: _tmpF63->escapes=1;{struct _tuple17 _tmp160C;
return(_tmp160C.f1=(_tmpF63->tq).real_const,((_tmp160C.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF63->rgn))->v,_tmp160C)));};_LL78C:;}_LL782: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF4C=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF48;if(_tmpF4C->tag != 22)goto
_LL784;else{_tmpF4D=_tmpF4C->f1;_tmpF4E=_tmpF4C->f2;_tmpF4F=_tmpF4C->f3;}}_LL783:
if(_tmpF4F)return bogus_ans;{void*_tmpF6D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF4D->topt))->v);struct Cyc_List_List*_tmpF6F;struct Cyc_Absyn_AggrInfo
_tmpF71;union Cyc_Absyn_AggrInfoU _tmpF72;struct Cyc_Absyn_Aggrdecl**_tmpF73;struct
Cyc_Absyn_Aggrdecl*_tmpF74;_LL7A4: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF6E=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF6D;if(_tmpF6E->tag != 13)goto _LL7A6;
else{_tmpF6F=_tmpF6E->f2;}}_LL7A5: {struct Cyc_Absyn_Aggrfield*_tmpF75=Cyc_Absyn_lookup_field(
_tmpF6F,_tmpF4E);if(_tmpF75 != 0  && _tmpF75->width != 0){struct _tuple17 _tmp160D;
return(_tmp160D.f1=(_tmpF75->tq).real_const,((_tmp160D.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF4D)).f2,_tmp160D)));}return bogus_ans;}_LL7A6: {struct Cyc_Absyn_AggrType_struct*
_tmpF70=(struct Cyc_Absyn_AggrType_struct*)_tmpF6D;if(_tmpF70->tag != 12)goto
_LL7A8;else{_tmpF71=_tmpF70->f1;_tmpF72=_tmpF71.aggr_info;if((_tmpF72.KnownAggr).tag
!= 2)goto _LL7A8;_tmpF73=(struct Cyc_Absyn_Aggrdecl**)(_tmpF72.KnownAggr).val;
_tmpF74=*_tmpF73;}}_LL7A7: {struct Cyc_Absyn_Aggrfield*_tmpF77=Cyc_Absyn_lookup_decl_field(
_tmpF74,_tmpF4E);if(_tmpF77 != 0  && _tmpF77->width != 0){struct _tuple17 _tmp160E;
return(_tmp160E.f1=(_tmpF77->tq).real_const,((_tmp160E.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF4D)).f2,_tmp160E)));}return bogus_ans;}_LL7A8:;_LL7A9: return bogus_ans;
_LL7A3:;};_LL784: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF50=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF48;if(_tmpF50->tag != 23)goto _LL786;else{_tmpF51=_tmpF50->f1;_tmpF52=_tmpF50->f2;
_tmpF53=_tmpF50->f3;}}_LL785: if(_tmpF53)return bogus_ans;{void*_tmpF79=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF51->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF7B;void*_tmpF7C;struct Cyc_Absyn_PtrAtts _tmpF7D;void*_tmpF7E;_LL7AB: {struct
Cyc_Absyn_PointerType_struct*_tmpF7A=(struct Cyc_Absyn_PointerType_struct*)
_tmpF79;if(_tmpF7A->tag != 5)goto _LL7AD;else{_tmpF7B=_tmpF7A->f1;_tmpF7C=_tmpF7B.elt_typ;
_tmpF7D=_tmpF7B.ptr_atts;_tmpF7E=_tmpF7D.rgn;}}_LL7AC: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF7F=Cyc_Tcutil_compress(_tmpF7C);struct Cyc_List_List*_tmpF81;
struct Cyc_Absyn_AggrInfo _tmpF83;union Cyc_Absyn_AggrInfoU _tmpF84;struct Cyc_Absyn_Aggrdecl**
_tmpF85;struct Cyc_Absyn_Aggrdecl*_tmpF86;_LL7B0: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF80=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF7F;if(_tmpF80->tag != 13)goto
_LL7B2;else{_tmpF81=_tmpF80->f2;}}_LL7B1: finfo=Cyc_Absyn_lookup_field(_tmpF81,
_tmpF52);goto _LL7AF;_LL7B2: {struct Cyc_Absyn_AggrType_struct*_tmpF82=(struct Cyc_Absyn_AggrType_struct*)
_tmpF7F;if(_tmpF82->tag != 12)goto _LL7B4;else{_tmpF83=_tmpF82->f1;_tmpF84=_tmpF83.aggr_info;
if((_tmpF84.KnownAggr).tag != 2)goto _LL7B4;_tmpF85=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF84.KnownAggr).val;_tmpF86=*_tmpF85;}}_LL7B3: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF86,_tmpF52);goto _LL7AF;_LL7B4:;_LL7B5: return bogus_ans;_LL7AF:;}if(finfo != 0
 && finfo->width != 0){struct _tuple17 _tmp160F;return(_tmp160F.f1=(finfo->tq).real_const,((
_tmp160F.f2=_tmpF7E,_tmp160F)));}return bogus_ans;}_LL7AD:;_LL7AE: return bogus_ans;
_LL7AA:;};_LL786: {struct Cyc_Absyn_Deref_e_struct*_tmpF54=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF48;if(_tmpF54->tag != 21)goto _LL788;else{_tmpF55=_tmpF54->f1;}}_LL787: {void*
_tmpF88=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF55->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF8A;struct Cyc_Absyn_Tqual _tmpF8B;struct Cyc_Absyn_PtrAtts
_tmpF8C;void*_tmpF8D;_LL7B7: {struct Cyc_Absyn_PointerType_struct*_tmpF89=(struct
Cyc_Absyn_PointerType_struct*)_tmpF88;if(_tmpF89->tag != 5)goto _LL7B9;else{
_tmpF8A=_tmpF89->f1;_tmpF8B=_tmpF8A.elt_tq;_tmpF8C=_tmpF8A.ptr_atts;_tmpF8D=
_tmpF8C.rgn;}}_LL7B8: {struct _tuple17 _tmp1610;return(_tmp1610.f1=_tmpF8B.real_const,((
_tmp1610.f2=_tmpF8D,_tmp1610)));}_LL7B9:;_LL7BA: return bogus_ans;_LL7B6:;}_LL788: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF56=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF48;if(_tmpF56->tag != 24)goto _LL78A;else{_tmpF57=_tmpF56->f1;_tmpF58=_tmpF56->f2;}}
_LL789: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF57->topt))->v);void*_tmpF8F=t;struct Cyc_List_List*_tmpF91;struct Cyc_Absyn_PtrInfo
_tmpF93;struct Cyc_Absyn_Tqual _tmpF94;struct Cyc_Absyn_PtrAtts _tmpF95;void*_tmpF96;
struct Cyc_Absyn_ArrayInfo _tmpF98;struct Cyc_Absyn_Tqual _tmpF99;_LL7BC: {struct Cyc_Absyn_TupleType_struct*
_tmpF90=(struct Cyc_Absyn_TupleType_struct*)_tmpF8F;if(_tmpF90->tag != 11)goto
_LL7BE;else{_tmpF91=_tmpF90->f1;}}_LL7BD: {unsigned int _tmpF9B;int _tmpF9C;struct
_tuple13 _tmpF9A=Cyc_Evexp_eval_const_uint_exp(_tmpF58);_tmpF9B=_tmpF9A.f1;
_tmpF9C=_tmpF9A.f2;if(!_tmpF9C)return bogus_ans;{struct _tuple11*_tmpF9D=Cyc_Absyn_lookup_tuple_field(
_tmpF91,(int)_tmpF9B);if(_tmpF9D != 0){struct _tuple17 _tmp1611;return(_tmp1611.f1=((*
_tmpF9D).f1).real_const,((_tmp1611.f2=(Cyc_Tcutil_addressof_props(te,_tmpF57)).f2,
_tmp1611)));}return bogus_ans;};}_LL7BE: {struct Cyc_Absyn_PointerType_struct*
_tmpF92=(struct Cyc_Absyn_PointerType_struct*)_tmpF8F;if(_tmpF92->tag != 5)goto
_LL7C0;else{_tmpF93=_tmpF92->f1;_tmpF94=_tmpF93.elt_tq;_tmpF95=_tmpF93.ptr_atts;
_tmpF96=_tmpF95.rgn;}}_LL7BF: {struct _tuple17 _tmp1612;return(_tmp1612.f1=_tmpF94.real_const,((
_tmp1612.f2=_tmpF96,_tmp1612)));}_LL7C0: {struct Cyc_Absyn_ArrayType_struct*
_tmpF97=(struct Cyc_Absyn_ArrayType_struct*)_tmpF8F;if(_tmpF97->tag != 9)goto
_LL7C2;else{_tmpF98=_tmpF97->f1;_tmpF99=_tmpF98.tq;}}_LL7C1: {struct _tuple17
_tmp1613;return(_tmp1613.f1=_tmpF99.real_const,((_tmp1613.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF57)).f2,_tmp1613)));}_LL7C2:;_LL7C3: return bogus_ans;_LL7BB:;}_LL78A:;
_LL78B:{const char*_tmp1616;void*_tmp1615;(_tmp1615=0,Cyc_Tcutil_terr(e->loc,((
_tmp1616="unary & applied to non-lvalue",_tag_dyneither(_tmp1616,sizeof(char),30))),
_tag_dyneither(_tmp1615,sizeof(void*),0)));}return bogus_ans;_LL77F:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFA4=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFA6;void*_tmpFA7;struct Cyc_Absyn_Tqual
_tmpFA8;union Cyc_Absyn_Constraint*_tmpFA9;_LL7C5: {struct Cyc_Absyn_ArrayType_struct*
_tmpFA5=(struct Cyc_Absyn_ArrayType_struct*)_tmpFA4;if(_tmpFA5->tag != 9)goto
_LL7C7;else{_tmpFA6=_tmpFA5->f1;_tmpFA7=_tmpFA6.elt_type;_tmpFA8=_tmpFA6.tq;
_tmpFA9=_tmpFA6.zero_term;}}_LL7C6: {void*_tmpFAB;struct _tuple17 _tmpFAA=Cyc_Tcutil_addressof_props(
te,e);_tmpFAB=_tmpFAA.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1619;struct Cyc_Absyn_Upper_b_struct*
_tmp1618;return Cyc_Absyn_atb_typ(_tmpFA7,_tmpFAB,_tmpFA8,(void*)((_tmp1618=
_cycalloc(sizeof(*_tmp1618)),((_tmp1618[0]=((_tmp1619.tag=1,((_tmp1619.f1=e,
_tmp1619)))),_tmp1618)))),_tmpFA9);};}_LL7C7:;_LL7C8: return e_typ;_LL7C4:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFAE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFB1;_LL7CA: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFAF=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFAE;if(_tmpFAF->tag != 0)goto
_LL7CC;}_LL7CB: return;_LL7CC: {struct Cyc_Absyn_Upper_b_struct*_tmpFB0=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFAE;if(_tmpFB0->tag != 1)goto _LL7C9;else{_tmpFB1=_tmpFB0->f1;}}_LL7CD: {
unsigned int _tmpFB3;int _tmpFB4;struct _tuple13 _tmpFB2=Cyc_Evexp_eval_const_uint_exp(
_tmpFB1);_tmpFB3=_tmpFB2.f1;_tmpFB4=_tmpFB2.f2;if(_tmpFB4  && _tmpFB3 <= i){const
char*_tmp161E;void*_tmp161D[2];struct Cyc_Int_pa_struct _tmp161C;struct Cyc_Int_pa_struct
_tmp161B;(_tmp161B.tag=1,((_tmp161B.f1=(unsigned long)((int)i),((_tmp161C.tag=1,((
_tmp161C.f1=(unsigned long)((int)_tmpFB3),((_tmp161D[0]=& _tmp161C,((_tmp161D[1]=&
_tmp161B,Cyc_Tcutil_terr(loc,((_tmp161E="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp161E,sizeof(char),39))),_tag_dyneither(_tmp161D,sizeof(void*),
2)))))))))))));}return;}_LL7C9:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFB9=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFBB;_LL7CF: {struct Cyc_Absyn_Upper_b_struct*_tmpFBA=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFB9;if(_tmpFBA->tag != 1)goto _LL7D1;else{_tmpFBB=_tmpFBA->f1;}}_LL7D0: {
unsigned int _tmpFBD;int _tmpFBE;struct _tuple13 _tmpFBC=Cyc_Evexp_eval_const_uint_exp(
_tmpFBB);_tmpFBD=_tmpFBC.f1;_tmpFBE=_tmpFBC.f2;return _tmpFBE  && _tmpFBD == 1;}
_LL7D1:;_LL7D2: return 0;_LL7CE:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFBF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFC7;void*_tmpFC8;union Cyc_Absyn_Constraint*
_tmpFC9;struct Cyc_List_List*_tmpFCB;struct Cyc_Absyn_AggrInfo _tmpFCD;union Cyc_Absyn_AggrInfoU
_tmpFCE;struct _tuple4 _tmpFCF;struct Cyc_Absyn_AggrInfo _tmpFD1;union Cyc_Absyn_AggrInfoU
_tmpFD2;struct Cyc_Absyn_Aggrdecl**_tmpFD3;struct Cyc_Absyn_Aggrdecl*_tmpFD4;
struct Cyc_List_List*_tmpFD5;struct Cyc_List_List*_tmpFD7;_LL7D4: {struct Cyc_Absyn_VoidType_struct*
_tmpFC0=(struct Cyc_Absyn_VoidType_struct*)_tmpFBF;if(_tmpFC0->tag != 0)goto _LL7D6;}
_LL7D5: goto _LL7D7;_LL7D6: {struct Cyc_Absyn_IntType_struct*_tmpFC1=(struct Cyc_Absyn_IntType_struct*)
_tmpFBF;if(_tmpFC1->tag != 6)goto _LL7D8;}_LL7D7: goto _LL7D9;_LL7D8: {struct Cyc_Absyn_FloatType_struct*
_tmpFC2=(struct Cyc_Absyn_FloatType_struct*)_tmpFBF;if(_tmpFC2->tag != 7)goto
_LL7DA;}_LL7D9: goto _LL7DB;_LL7DA: {struct Cyc_Absyn_DoubleType_struct*_tmpFC3=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFBF;if(_tmpFC3->tag != 8)goto _LL7DC;}
_LL7DB: return 1;_LL7DC: {struct Cyc_Absyn_EnumType_struct*_tmpFC4=(struct Cyc_Absyn_EnumType_struct*)
_tmpFBF;if(_tmpFC4->tag != 14)goto _LL7DE;}_LL7DD: goto _LL7DF;_LL7DE: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFC5=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpFBF;if(_tmpFC5->tag != 15)goto
_LL7E0;}_LL7DF: return 0;_LL7E0: {struct Cyc_Absyn_ArrayType_struct*_tmpFC6=(struct
Cyc_Absyn_ArrayType_struct*)_tmpFBF;if(_tmpFC6->tag != 9)goto _LL7E2;else{_tmpFC7=
_tmpFC6->f1;_tmpFC8=_tmpFC7.elt_type;_tmpFC9=_tmpFC7.zero_term;}}_LL7E1: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFC9) && Cyc_Tcutil_bits_only(
_tmpFC8);_LL7E2: {struct Cyc_Absyn_TupleType_struct*_tmpFCA=(struct Cyc_Absyn_TupleType_struct*)
_tmpFBF;if(_tmpFCA->tag != 11)goto _LL7E4;else{_tmpFCB=_tmpFCA->f1;}}_LL7E3: for(0;
_tmpFCB != 0;_tmpFCB=_tmpFCB->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpFCB->hd)).f2))return 0;}return 1;_LL7E4: {struct Cyc_Absyn_AggrType_struct*
_tmpFCC=(struct Cyc_Absyn_AggrType_struct*)_tmpFBF;if(_tmpFCC->tag != 12)goto
_LL7E6;else{_tmpFCD=_tmpFCC->f1;_tmpFCE=_tmpFCD.aggr_info;if((_tmpFCE.UnknownAggr).tag
!= 1)goto _LL7E6;_tmpFCF=(struct _tuple4)(_tmpFCE.UnknownAggr).val;}}_LL7E5: return
0;_LL7E6: {struct Cyc_Absyn_AggrType_struct*_tmpFD0=(struct Cyc_Absyn_AggrType_struct*)
_tmpFBF;if(_tmpFD0->tag != 12)goto _LL7E8;else{_tmpFD1=_tmpFD0->f1;_tmpFD2=_tmpFD1.aggr_info;
if((_tmpFD2.KnownAggr).tag != 2)goto _LL7E8;_tmpFD3=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFD2.KnownAggr).val;_tmpFD4=*_tmpFD3;_tmpFD5=_tmpFD1.targs;}}_LL7E7: if(
_tmpFD4->impl == 0)return 0;{struct _RegionHandle _tmpFD8=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFD8;_push_region(rgn);{struct Cyc_List_List*_tmpFD9=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFD4->tvs,_tmpFD5);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFD4->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFD9,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFDA=0;_npop_handler(0);return
_tmpFDA;}}}{int _tmpFDB=1;_npop_handler(0);return _tmpFDB;};};_pop_region(rgn);};
_LL7E8: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFD6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFBF;if(_tmpFD6->tag != 13)goto _LL7EA;else{_tmpFD7=_tmpFD6->f2;}}_LL7E9: for(0;
_tmpFD7 != 0;_tmpFD7=_tmpFD7->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFD7->hd)->type))return 0;}return 1;_LL7EA:;_LL7EB: return 0;_LL7D3:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpFDC=e->r;struct _tuple2*_tmpFE4;void*_tmpFE5;struct Cyc_Absyn_Exp*_tmpFE7;
struct Cyc_Absyn_Exp*_tmpFE8;struct Cyc_Absyn_Exp*_tmpFE9;struct Cyc_Absyn_Exp*
_tmpFEB;struct Cyc_Absyn_Exp*_tmpFEC;struct Cyc_Absyn_Exp*_tmpFEE;struct Cyc_Absyn_Exp*
_tmpFF0;void*_tmpFF2;struct Cyc_Absyn_Exp*_tmpFF3;enum Cyc_Absyn_Coercion _tmpFF4;
void*_tmpFF6;struct Cyc_Absyn_Exp*_tmpFF7;struct Cyc_Absyn_Exp*_tmpFF9;struct Cyc_Absyn_Exp*
_tmpFFB;struct Cyc_Absyn_Exp*_tmpFFC;struct Cyc_List_List*_tmpFFE;struct Cyc_List_List*
_tmp1000;struct Cyc_List_List*_tmp1002;enum Cyc_Absyn_Primop _tmp1004;struct Cyc_List_List*
_tmp1005;struct Cyc_List_List*_tmp1007;struct Cyc_List_List*_tmp1009;_LL7ED: {
struct Cyc_Absyn_Const_e_struct*_tmpFDD=(struct Cyc_Absyn_Const_e_struct*)_tmpFDC;
if(_tmpFDD->tag != 0)goto _LL7EF;}_LL7EE: goto _LL7F0;_LL7EF: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpFDE=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpFDC;if(_tmpFDE->tag != 18)goto
_LL7F1;}_LL7F0: goto _LL7F2;_LL7F1: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpFDF=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFDC;if(_tmpFDF->tag != 19)goto _LL7F3;}
_LL7F2: goto _LL7F4;_LL7F3: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFE0=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpFDC;if(_tmpFE0->tag != 20)goto _LL7F5;}_LL7F4: goto _LL7F6;_LL7F5: {struct Cyc_Absyn_Enum_e_struct*
_tmpFE1=(struct Cyc_Absyn_Enum_e_struct*)_tmpFDC;if(_tmpFE1->tag != 32)goto _LL7F7;}
_LL7F6: goto _LL7F8;_LL7F7: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpFE2=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpFDC;if(_tmpFE2->tag != 33)goto _LL7F9;}_LL7F8: return 1;_LL7F9: {struct Cyc_Absyn_Var_e_struct*
_tmpFE3=(struct Cyc_Absyn_Var_e_struct*)_tmpFDC;if(_tmpFE3->tag != 1)goto _LL7FB;
else{_tmpFE4=_tmpFE3->f1;_tmpFE5=(void*)_tmpFE3->f2;}}_LL7FA: {void*_tmp100A=
_tmpFE5;struct Cyc_Absyn_Vardecl*_tmp100D;_LL81A: {struct Cyc_Absyn_Funname_b_struct*
_tmp100B=(struct Cyc_Absyn_Funname_b_struct*)_tmp100A;if(_tmp100B->tag != 2)goto
_LL81C;}_LL81B: return 1;_LL81C: {struct Cyc_Absyn_Global_b_struct*_tmp100C=(struct
Cyc_Absyn_Global_b_struct*)_tmp100A;if(_tmp100C->tag != 1)goto _LL81E;else{
_tmp100D=_tmp100C->f1;}}_LL81D: {void*_tmp100F=Cyc_Tcutil_compress(_tmp100D->type);
_LL823: {struct Cyc_Absyn_ArrayType_struct*_tmp1010=(struct Cyc_Absyn_ArrayType_struct*)
_tmp100F;if(_tmp1010->tag != 9)goto _LL825;}_LL824: goto _LL826;_LL825: {struct Cyc_Absyn_FnType_struct*
_tmp1011=(struct Cyc_Absyn_FnType_struct*)_tmp100F;if(_tmp1011->tag != 10)goto
_LL827;}_LL826: return 1;_LL827:;_LL828: return var_okay;_LL822:;}_LL81E: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp100E=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp100A;if(_tmp100E->tag != 0)
goto _LL820;}_LL81F: return 0;_LL820:;_LL821: return var_okay;_LL819:;}_LL7FB: {
struct Cyc_Absyn_Conditional_e_struct*_tmpFE6=(struct Cyc_Absyn_Conditional_e_struct*)
_tmpFDC;if(_tmpFE6->tag != 6)goto _LL7FD;else{_tmpFE7=_tmpFE6->f1;_tmpFE8=_tmpFE6->f2;
_tmpFE9=_tmpFE6->f3;}}_LL7FC: return(Cyc_Tcutil_cnst_exp(te,0,_tmpFE7) && Cyc_Tcutil_cnst_exp(
te,0,_tmpFE8)) && Cyc_Tcutil_cnst_exp(te,0,_tmpFE9);_LL7FD: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpFEA=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpFDC;if(_tmpFEA->tag != 9)goto _LL7FF;
else{_tmpFEB=_tmpFEA->f1;_tmpFEC=_tmpFEA->f2;}}_LL7FE: return Cyc_Tcutil_cnst_exp(
te,0,_tmpFEB) && Cyc_Tcutil_cnst_exp(te,0,_tmpFEC);_LL7FF: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpFED=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpFDC;if(_tmpFED->tag != 13)
goto _LL801;else{_tmpFEE=_tmpFED->f1;}}_LL800: _tmpFF0=_tmpFEE;goto _LL802;_LL801: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpFEF=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpFDC;if(_tmpFEF->tag != 14)goto _LL803;else{_tmpFF0=_tmpFEF->f1;}}_LL802: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpFF0);_LL803: {struct Cyc_Absyn_Cast_e_struct*
_tmpFF1=(struct Cyc_Absyn_Cast_e_struct*)_tmpFDC;if(_tmpFF1->tag != 15)goto _LL805;
else{_tmpFF2=(void*)_tmpFF1->f1;_tmpFF3=_tmpFF1->f2;_tmpFF4=_tmpFF1->f4;if(
_tmpFF4 != Cyc_Absyn_No_coercion)goto _LL805;}}_LL804: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpFF3);_LL805: {struct Cyc_Absyn_Cast_e_struct*_tmpFF5=(struct Cyc_Absyn_Cast_e_struct*)
_tmpFDC;if(_tmpFF5->tag != 15)goto _LL807;else{_tmpFF6=(void*)_tmpFF5->f1;_tmpFF7=
_tmpFF5->f2;}}_LL806: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpFF7);_LL807: {
struct Cyc_Absyn_Address_e_struct*_tmpFF8=(struct Cyc_Absyn_Address_e_struct*)
_tmpFDC;if(_tmpFF8->tag != 16)goto _LL809;else{_tmpFF9=_tmpFF8->f1;}}_LL808: return
Cyc_Tcutil_cnst_exp(te,1,_tmpFF9);_LL809: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpFFA=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpFDC;if(_tmpFFA->tag != 28)
goto _LL80B;else{_tmpFFB=_tmpFFA->f2;_tmpFFC=_tmpFFA->f3;}}_LL80A: return Cyc_Tcutil_cnst_exp(
te,0,_tmpFFB) && Cyc_Tcutil_cnst_exp(te,0,_tmpFFC);_LL80B: {struct Cyc_Absyn_Array_e_struct*
_tmpFFD=(struct Cyc_Absyn_Array_e_struct*)_tmpFDC;if(_tmpFFD->tag != 27)goto _LL80D;
else{_tmpFFE=_tmpFFD->f1;}}_LL80C: _tmp1000=_tmpFFE;goto _LL80E;_LL80D: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpFFF=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpFDC;if(_tmpFFF->tag != 30)goto
_LL80F;else{_tmp1000=_tmpFFF->f2;}}_LL80E: _tmp1002=_tmp1000;goto _LL810;_LL80F: {
struct Cyc_Absyn_Aggregate_e_struct*_tmp1001=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpFDC;if(_tmp1001->tag != 29)goto _LL811;else{_tmp1002=_tmp1001->f3;}}_LL810:
for(0;_tmp1002 != 0;_tmp1002=_tmp1002->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((
struct _tuple18*)_tmp1002->hd)).f2))return 0;}return 1;_LL811: {struct Cyc_Absyn_Primop_e_struct*
_tmp1003=(struct Cyc_Absyn_Primop_e_struct*)_tmpFDC;if(_tmp1003->tag != 3)goto
_LL813;else{_tmp1004=_tmp1003->f1;_tmp1005=_tmp1003->f2;}}_LL812: _tmp1007=
_tmp1005;goto _LL814;_LL813: {struct Cyc_Absyn_Tuple_e_struct*_tmp1006=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpFDC;if(_tmp1006->tag != 25)goto _LL815;else{_tmp1007=_tmp1006->f1;}}_LL814:
_tmp1009=_tmp1007;goto _LL816;_LL815: {struct Cyc_Absyn_Datatype_e_struct*_tmp1008=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpFDC;if(_tmp1008->tag != 31)goto _LL817;
else{_tmp1009=_tmp1008->f1;}}_LL816: for(0;_tmp1009 != 0;_tmp1009=_tmp1009->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmp1009->hd))return 0;}return
1;_LL817:;_LL818: return 0;_LL7EC:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp1012=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1018;
struct Cyc_Absyn_PtrAtts _tmp1019;union Cyc_Absyn_Constraint*_tmp101A;union Cyc_Absyn_Constraint*
_tmp101B;struct Cyc_Absyn_ArrayInfo _tmp101D;void*_tmp101E;struct Cyc_List_List*
_tmp1020;struct Cyc_Absyn_AggrInfo _tmp1022;union Cyc_Absyn_AggrInfoU _tmp1023;
struct Cyc_List_List*_tmp1024;struct Cyc_List_List*_tmp1026;_LL82A: {struct Cyc_Absyn_VoidType_struct*
_tmp1013=(struct Cyc_Absyn_VoidType_struct*)_tmp1012;if(_tmp1013->tag != 0)goto
_LL82C;}_LL82B: goto _LL82D;_LL82C: {struct Cyc_Absyn_IntType_struct*_tmp1014=(
struct Cyc_Absyn_IntType_struct*)_tmp1012;if(_tmp1014->tag != 6)goto _LL82E;}_LL82D:
goto _LL82F;_LL82E: {struct Cyc_Absyn_FloatType_struct*_tmp1015=(struct Cyc_Absyn_FloatType_struct*)
_tmp1012;if(_tmp1015->tag != 7)goto _LL830;}_LL82F: goto _LL831;_LL830: {struct Cyc_Absyn_DoubleType_struct*
_tmp1016=(struct Cyc_Absyn_DoubleType_struct*)_tmp1012;if(_tmp1016->tag != 8)goto
_LL832;}_LL831: return 1;_LL832: {struct Cyc_Absyn_PointerType_struct*_tmp1017=(
struct Cyc_Absyn_PointerType_struct*)_tmp1012;if(_tmp1017->tag != 5)goto _LL834;
else{_tmp1018=_tmp1017->f1;_tmp1019=_tmp1018.ptr_atts;_tmp101A=_tmp1019.nullable;
_tmp101B=_tmp1019.bounds;}}_LL833: {void*_tmp1029=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp101B);_LL843: {
struct Cyc_Absyn_DynEither_b_struct*_tmp102A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1029;if(_tmp102A->tag != 0)goto _LL845;}_LL844: return 1;_LL845:;_LL846: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp101A);
_LL842:;}_LL834: {struct Cyc_Absyn_ArrayType_struct*_tmp101C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1012;if(_tmp101C->tag != 9)goto _LL836;else{_tmp101D=_tmp101C->f1;_tmp101E=
_tmp101D.elt_type;}}_LL835: return Cyc_Tcutil_supports_default(_tmp101E);_LL836: {
struct Cyc_Absyn_TupleType_struct*_tmp101F=(struct Cyc_Absyn_TupleType_struct*)
_tmp1012;if(_tmp101F->tag != 11)goto _LL838;else{_tmp1020=_tmp101F->f1;}}_LL837:
for(0;_tmp1020 != 0;_tmp1020=_tmp1020->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple11*)_tmp1020->hd)).f2))return 0;}return 1;_LL838: {struct Cyc_Absyn_AggrType_struct*
_tmp1021=(struct Cyc_Absyn_AggrType_struct*)_tmp1012;if(_tmp1021->tag != 12)goto
_LL83A;else{_tmp1022=_tmp1021->f1;_tmp1023=_tmp1022.aggr_info;_tmp1024=_tmp1022.targs;}}
_LL839: {struct Cyc_Absyn_Aggrdecl*_tmp102B=Cyc_Absyn_get_known_aggrdecl(_tmp1023);
if(_tmp102B->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp102B->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp102B->tvs,_tmp1024,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp102B->impl))->fields);}
_LL83A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1025=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1012;if(_tmp1025->tag != 13)goto _LL83C;else{_tmp1026=_tmp1025->f2;}}_LL83B:
return Cyc_Tcutil_fields_support_default(0,0,_tmp1026);_LL83C: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1027=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1012;if(_tmp1027->tag != 15)
goto _LL83E;}_LL83D: goto _LL83F;_LL83E: {struct Cyc_Absyn_EnumType_struct*_tmp1028=(
struct Cyc_Absyn_EnumType_struct*)_tmp1012;if(_tmp1028->tag != 14)goto _LL840;}
_LL83F: return 1;_LL840:;_LL841: return 0;_LL829:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp102C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp102C;
_push_region(rgn);{struct Cyc_List_List*_tmp102D=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp102D,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp102E=0;_npop_handler(0);return _tmp102E;}}}{int _tmp102F=1;
_npop_handler(0);return _tmp102F;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp1030=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1035;
struct Cyc_Absyn_PtrAtts _tmp1036;union Cyc_Absyn_Constraint*_tmp1037;union Cyc_Absyn_Constraint*
_tmp1038;_LL848: {struct Cyc_Absyn_IntType_struct*_tmp1031=(struct Cyc_Absyn_IntType_struct*)
_tmp1030;if(_tmp1031->tag != 6)goto _LL84A;}_LL849: goto _LL84B;_LL84A: {struct Cyc_Absyn_FloatType_struct*
_tmp1032=(struct Cyc_Absyn_FloatType_struct*)_tmp1030;if(_tmp1032->tag != 7)goto
_LL84C;}_LL84B: goto _LL84D;_LL84C: {struct Cyc_Absyn_DoubleType_struct*_tmp1033=(
struct Cyc_Absyn_DoubleType_struct*)_tmp1030;if(_tmp1033->tag != 8)goto _LL84E;}
_LL84D: return 1;_LL84E: {struct Cyc_Absyn_PointerType_struct*_tmp1034=(struct Cyc_Absyn_PointerType_struct*)
_tmp1030;if(_tmp1034->tag != 5)goto _LL850;else{_tmp1035=_tmp1034->f1;_tmp1036=
_tmp1035.ptr_atts;_tmp1037=_tmp1036.nullable;_tmp1038=_tmp1036.bounds;}}_LL84F: {
void*_tmp1039=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp1038);_LL853: {struct Cyc_Absyn_DynEither_b_struct*
_tmp103A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1039;if(_tmp103A->tag != 0)goto
_LL855;}_LL854: return 0;_LL855:;_LL856: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp1037);_LL852:;}_LL850:;_LL851: return 0;_LL847:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp103B=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp103D;void*_tmp103E;struct Cyc_Absyn_FnInfo _tmp1040;struct Cyc_List_List*
_tmp1041;_LL858: {struct Cyc_Absyn_PointerType_struct*_tmp103C=(struct Cyc_Absyn_PointerType_struct*)
_tmp103B;if(_tmp103C->tag != 5)goto _LL85A;else{_tmp103D=_tmp103C->f1;_tmp103E=
_tmp103D.elt_typ;}}_LL859: return Cyc_Tcutil_is_noreturn(_tmp103E);_LL85A: {struct
Cyc_Absyn_FnType_struct*_tmp103F=(struct Cyc_Absyn_FnType_struct*)_tmp103B;if(
_tmp103F->tag != 10)goto _LL85C;else{_tmp1040=_tmp103F->f1;_tmp1041=_tmp1040.attributes;}}
_LL85B: for(0;_tmp1041 != 0;_tmp1041=_tmp1041->tl){void*_tmp1042=(void*)_tmp1041->hd;
_LL85F: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1043=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp1042;if(_tmp1043->tag != 4)goto _LL861;}_LL860: return 1;_LL861:;_LL862:
continue;_LL85E:;}goto _LL857;_LL85C:;_LL85D: goto _LL857;_LL857:;}return 0;}struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp1044=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp1046;struct Cyc_List_List*
_tmp1047;struct Cyc_List_List**_tmp1048;_LL864: {struct Cyc_Absyn_FnType_struct*
_tmp1045=(struct Cyc_Absyn_FnType_struct*)_tmp1044;if(_tmp1045->tag != 10)goto
_LL866;else{_tmp1046=_tmp1045->f1;_tmp1047=_tmp1046.attributes;_tmp1048=(struct
Cyc_List_List**)&(_tmp1045->f1).attributes;}}_LL865: {struct Cyc_List_List*
_tmp1049=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp161F;*_tmp1048=((_tmp161F=_cycalloc(sizeof(*_tmp161F)),((
_tmp161F->hd=(void*)atts->hd,((_tmp161F->tl=*_tmp1048,_tmp161F))))));}else{
struct Cyc_List_List*_tmp1620;_tmp1049=((_tmp1620=_cycalloc(sizeof(*_tmp1620)),((
_tmp1620->hd=(void*)atts->hd,((_tmp1620->tl=_tmp1049,_tmp1620))))));}}return
_tmp1049;}_LL866:;_LL867: {const char*_tmp1623;void*_tmp1622;(_tmp1622=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1623="transfer_fn_type_atts",
_tag_dyneither(_tmp1623,sizeof(char),22))),_tag_dyneither(_tmp1622,sizeof(void*),
0)));}_LL863:;}
