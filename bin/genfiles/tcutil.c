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
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp1021;void*_tmp1020[2];const char*
_tmp101F;const char*_tmp101E;struct Cyc_String_pa_struct _tmp101D;struct Cyc_String_pa_struct
_tmp101C;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp101C.tag=0,((
_tmp101C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp101D.tag=0,((_tmp101D.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp101E="const ",_tag_dyneither(_tmp101E,sizeof(char),7)):((
_tmp101F="",_tag_dyneither(_tmp101F,sizeof(char),1)))),((_tmp1020[0]=& _tmp101D,((
_tmp1020[1]=& _tmp101C,Cyc_aprintf(((_tmp1021="%s%s",_tag_dyneither(_tmp1021,
sizeof(char),5))),_tag_dyneither(_tmp1020,sizeof(void*),2))))))))))))));const
char*_tmp102A;void*_tmp1029[2];const char*_tmp1028;const char*_tmp1027;struct Cyc_String_pa_struct
_tmp1026;struct Cyc_String_pa_struct _tmp1025;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp1025.tag=0,((_tmp1025.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1026.tag=0,((
_tmp1026.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp1027="const ",
_tag_dyneither(_tmp1027,sizeof(char),7)):((_tmp1028="",_tag_dyneither(_tmp1028,
sizeof(char),1)))),((_tmp1029[0]=& _tmp1026,((_tmp1029[1]=& _tmp1025,Cyc_aprintf(((
_tmp102A="%s%s",_tag_dyneither(_tmp102A,sizeof(char),5))),_tag_dyneither(
_tmp1029,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp102E;void*
_tmp102D[1];struct Cyc_String_pa_struct _tmp102C;(_tmp102C.tag=0,((_tmp102C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp102D[0]=& _tmp102C,Cyc_fprintf(
Cyc_stderr,((_tmp102E="  %s",_tag_dyneither(_tmp102E,sizeof(char),5))),
_tag_dyneither(_tmp102D,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp1031;void*_tmp1030;(_tmp1030=0,Cyc_fprintf(
Cyc_stderr,((_tmp1031="\n\t",_tag_dyneither(_tmp1031,sizeof(char),3))),
_tag_dyneither(_tmp1030,sizeof(void*),0)));}pos=8;}else{{const char*_tmp1034;void*
_tmp1033;(_tmp1033=0,Cyc_fprintf(Cyc_stderr,((_tmp1034=" ",_tag_dyneither(
_tmp1034,sizeof(char),2))),_tag_dyneither(_tmp1033,sizeof(void*),0)));}++ pos;}{
const char*_tmp1037;void*_tmp1036;(_tmp1036=0,Cyc_fprintf(Cyc_stderr,((_tmp1037="and ",
_tag_dyneither(_tmp1037,sizeof(char),5))),_tag_dyneither(_tmp1036,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp103A;void*_tmp1039;(_tmp1039=0,Cyc_fprintf(Cyc_stderr,((_tmp103A="\n\t",
_tag_dyneither(_tmp103A,sizeof(char),3))),_tag_dyneither(_tmp1039,sizeof(void*),
0)));}pos=8;}{const char*_tmp103E;void*_tmp103D[1];struct Cyc_String_pa_struct
_tmp103C;(_tmp103C.tag=0,((_tmp103C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp103D[0]=& _tmp103C,Cyc_fprintf(Cyc_stderr,((_tmp103E="%s ",
_tag_dyneither(_tmp103E,sizeof(char),4))),_tag_dyneither(_tmp103D,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp1041;void*_tmp1040;(_tmp1040=0,Cyc_fprintf(Cyc_stderr,((_tmp1041="\n\t",
_tag_dyneither(_tmp1041,sizeof(char),3))),_tag_dyneither(_tmp1040,sizeof(void*),
0)));}pos=8;}{const char*_tmp1044;void*_tmp1043;(_tmp1043=0,Cyc_fprintf(Cyc_stderr,((
_tmp1044="are not compatible. ",_tag_dyneither(_tmp1044,sizeof(char),21))),
_tag_dyneither(_tmp1043,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp1047;void*_tmp1046;(_tmp1046=0,
Cyc_fprintf(Cyc_stderr,((_tmp1047="\n\t",_tag_dyneither(_tmp1047,sizeof(char),3))),
_tag_dyneither(_tmp1046,sizeof(void*),0)));}{const char*_tmp104B;void*_tmp104A[1];
struct Cyc_String_pa_struct _tmp1049;(_tmp1049.tag=0,((_tmp1049.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp104A[0]=&
_tmp1049,Cyc_fprintf(Cyc_stderr,((_tmp104B="%s",_tag_dyneither(_tmp104B,sizeof(
char),3))),_tag_dyneither(_tmp104A,sizeof(void*),1)))))));};}{const char*_tmp104E;
void*_tmp104D;(_tmp104D=0,Cyc_fprintf(Cyc_stderr,((_tmp104E="\n",_tag_dyneither(
_tmp104E,sizeof(char),2))),_tag_dyneither(_tmp104D,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp1052;
void*_tmp1051[1];struct Cyc_String_pa_struct _tmp1050;(_tmp1050.tag=0,((_tmp1050.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1051[0]=& _tmp1050,Cyc_fprintf(
Cyc_stderr,((_tmp1052="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp1052,sizeof(char),36))),_tag_dyneither(_tmp1051,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp1055;
struct Cyc_Core_Impossible_struct*_tmp1054;(int)_throw((void*)((_tmp1054=
_cycalloc(sizeof(*_tmp1054)),((_tmp1054[0]=((_tmp1055.tag=Cyc_Core_Impossible,((
_tmp1055.f1=msg,_tmp1055)))),_tmp1054)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp105A;void*_tmp1059[2];struct Cyc_String_pa_struct
_tmp1058;struct Cyc_String_pa_struct _tmp1057;(_tmp1057.tag=0,((_tmp1057.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp1058.tag=0,((_tmp1058.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp1059[0]=& _tmp1058,((_tmp1059[1]=& _tmp1057,Cyc_fprintf(Cyc_stderr,((
_tmp105A="%s::%s ",_tag_dyneither(_tmp105A,sizeof(char),8))),_tag_dyneither(
_tmp1059,sizeof(void*),2)))))))))))));}{const char*_tmp105D;void*_tmp105C;(
_tmp105C=0,Cyc_fprintf(Cyc_stderr,((_tmp105D="\n",_tag_dyneither(_tmp105D,
sizeof(char),2))),_tag_dyneither(_tmp105C,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp105E;Cyc_Tcutil_warning_segs=((_tmp105E=_cycalloc(
sizeof(*_tmp105E)),((_tmp105E->hd=sg,((_tmp105E->tl=Cyc_Tcutil_warning_segs,
_tmp105E))))));}{struct _dyneither_ptr*_tmp1061;struct Cyc_List_List*_tmp1060;Cyc_Tcutil_warning_msgs=((
_tmp1060=_cycalloc(sizeof(*_tmp1060)),((_tmp1060->hd=((_tmp1061=_cycalloc(
sizeof(*_tmp1061)),((_tmp1061[0]=msg,_tmp1061)))),((_tmp1060->tl=Cyc_Tcutil_warning_msgs,
_tmp1060))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp1064;void*_tmp1063;(_tmp1063=0,Cyc_fprintf(Cyc_stderr,((
_tmp1064="***Warnings***\n",_tag_dyneither(_tmp1064,sizeof(char),16))),
_tag_dyneither(_tmp1063,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp1069;void*_tmp1068[2];struct
Cyc_String_pa_struct _tmp1067;struct Cyc_String_pa_struct _tmp1066;(_tmp1066.tag=0,((
_tmp1066.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp1067.tag=
0,((_tmp1067.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp1068[0]=&
_tmp1067,((_tmp1068[1]=& _tmp1066,Cyc_fprintf(Cyc_stderr,((_tmp1069="%s: %s\n",
_tag_dyneither(_tmp1069,sizeof(char),8))),_tag_dyneither(_tmp1068,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp106C;void*_tmp106B;(
_tmp106B=0,Cyc_fprintf(Cyc_stderr,((_tmp106C="**************\n",_tag_dyneither(
_tmp106C,sizeof(char),16))),_tag_dyneither(_tmp106B,sizeof(void*),0)));}Cyc_fflush((
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp106D;*_tmp44=((_tmp106D=
_cycalloc(sizeof(*_tmp106D)),((_tmp106D[0]=t2,_tmp106D))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp106E;*_tmp47=((_tmp106E=_cycalloc(sizeof(*
_tmp106E)),((_tmp106E->v=t2,_tmp106E))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
struct Cyc_Absyn_Unknown_kb_struct _tmp1071;struct Cyc_Absyn_Unknown_kb_struct*
_tmp1070;return(void*)((_tmp1070=_cycalloc(sizeof(*_tmp1070)),((_tmp1070[0]=((
_tmp1071.tag=1,((_tmp1071.f1=0,_tmp1071)))),_tmp1070))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp1074;struct
Cyc_Absyn_Less_kb_struct*_tmp1073;return(void*)((_tmp1073=_cycalloc(sizeof(*
_tmp1073)),((_tmp1073[0]=((_tmp1074.tag=2,((_tmp1074.f1=0,((_tmp1074.f2=_tmp5C,
_tmp1074)))))),_tmp1073))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp1075;
return(_tmp1075=_cycalloc(sizeof(*_tmp1075)),((_tmp1075->name=tv->name,((
_tmp1075->identity=- 1,((_tmp1075->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp1075)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple9*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple9*_tmp1076;return(_tmp1076=_cycalloc(sizeof(*
_tmp1076)),((_tmp1076->f1=_tmp64,((_tmp1076->f2=_tmp65,((_tmp1076->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp1076)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg){struct _tuple11 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple11*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple11*_tmp1077;return(_tmp1077=_cycalloc(sizeof(*_tmp1077)),((_tmp1077->f1=
_tmp6A,((_tmp1077->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp1077)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp1078;return(_tmp1078=_cycalloc(sizeof(*_tmp1078)),((_tmp1078->name=f->name,((
_tmp1078->tq=f->tq,((_tmp1078->type=Cyc_Tcutil_copy_type(f->type),((_tmp1078->width=
f->width,((_tmp1078->attributes=f->attributes,_tmp1078)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp1079;return(_tmp1079=_cycalloc(sizeof(*_tmp1079)),((_tmp1079->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp1079->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp1079)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp107A;return(_tmp107A=
_cycalloc(sizeof(*_tmp107A)),((_tmp107A->name=f->name,((_tmp107A->tag=f->tag,((
_tmp107A->loc=f->loc,_tmp107A)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp107D;struct Cyc_Absyn_VarType_struct*
_tmp107C;return(void*)((_tmp107C=_cycalloc(sizeof(*_tmp107C)),((_tmp107C[0]=((
_tmp107D.tag=2,((_tmp107D.f1=Cyc_Tcutil_copy_tvar(t),_tmp107D)))),_tmp107C))));}
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
_tmp1080;struct Cyc_Absyn_VarType_struct*_tmp107F;return(void*)((_tmp107F=
_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=2,((_tmp1080.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp1080)))),_tmp107F))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp1086;struct Cyc_Absyn_DatatypeInfo _tmp1085;
struct Cyc_Absyn_DatatypeType_struct*_tmp1084;return(void*)((_tmp1084=_cycalloc(
sizeof(*_tmp1084)),((_tmp1084[0]=((_tmp1086.tag=3,((_tmp1086.f1=((_tmp1085.datatype_info=
_tmp7D,((_tmp1085.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp1085)))),_tmp1086)))),
_tmp1084))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp108C;
struct Cyc_Absyn_DatatypeFieldInfo _tmp108B;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp108A;return(void*)((_tmp108A=_cycalloc(sizeof(*_tmp108A)),((_tmp108A[0]=((
_tmp108C.tag=4,((_tmp108C.f1=((_tmp108B.field_info=_tmp81,((_tmp108B.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp108B)))),_tmp108C)))),_tmp108A))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE7=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpE8=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpE9=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEA=_tmp86;union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpEC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp1096;struct Cyc_Absyn_PtrAtts
_tmp1095;struct Cyc_Absyn_PtrInfo _tmp1094;struct Cyc_Absyn_PointerType_struct*
_tmp1093;return(void*)((_tmp1093=_cycalloc(sizeof(*_tmp1093)),((_tmp1093[0]=((
_tmp1096.tag=5,((_tmp1096.f1=((_tmp1094.elt_typ=_tmpE7,((_tmp1094.elt_tq=_tmpEA,((
_tmp1094.ptr_atts=((_tmp1095.rgn=_tmpE8,((_tmp1095.nullable=_tmpE9,((_tmp1095.bounds=
_tmpEB,((_tmp1095.zero_term=_tmpEC,((_tmp1095.ptrloc=_tmp8C,_tmp1095)))))))))),
_tmp1094)))))),_tmp1096)))),_tmp1093))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_DoubleType_struct*
_tmp8F=(struct Cyc_Absyn_DoubleType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;}
_LL34: return t;_LL35: {struct Cyc_Absyn_ArrayType_struct*_tmp90=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76;if(_tmp90->tag != 9)goto _LL37;else{_tmp91=_tmp90->f1;_tmp92=_tmp91.elt_type;
_tmp93=_tmp91.tq;_tmp94=_tmp91.num_elts;_tmp95=_tmp91.zero_term;_tmp96=_tmp91.zt_loc;}}
_LL36: {struct Cyc_Absyn_ArrayType_struct _tmp109C;struct Cyc_Absyn_ArrayInfo
_tmp109B;struct Cyc_Absyn_ArrayType_struct*_tmp109A;return(void*)((_tmp109A=
_cycalloc(sizeof(*_tmp109A)),((_tmp109A[0]=((_tmp109C.tag=9,((_tmp109C.f1=((
_tmp109B.elt_type=Cyc_Tcutil_copy_type(_tmp92),((_tmp109B.tq=_tmp93,((_tmp109B.num_elts=
_tmp94,((_tmp109B.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp95),((_tmp109B.zt_loc=_tmp96,_tmp109B)))))))))),
_tmp109C)))),_tmp109A))));}_LL37: {struct Cyc_Absyn_FnType_struct*_tmp97=(struct
Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp97->tag != 10)goto _LL39;else{_tmp98=_tmp97->f1;
_tmp99=_tmp98.tvars;_tmp9A=_tmp98.effect;_tmp9B=_tmp98.ret_typ;_tmp9C=_tmp98.args;
_tmp9D=_tmp98.c_varargs;_tmp9E=_tmp98.cyc_varargs;_tmp9F=_tmp98.rgn_po;_tmpA0=
_tmp98.attributes;}}_LL38: {struct Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp99);struct Cyc_Core_Opt*_tmp109D;struct Cyc_Core_Opt*
_tmpF5=_tmp9A == 0?0:((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D->v=Cyc_Tcutil_copy_type((
void*)_tmp9A->v),_tmp109D))));void*_tmpF6=Cyc_Tcutil_copy_type(_tmp9B);struct Cyc_List_List*
_tmpF7=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp9C);int _tmpF8=_tmp9D;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp9E != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp9E;struct Cyc_Absyn_VarargInfo*_tmp109E;cyc_varargs2=((_tmp109E=_cycalloc(
sizeof(*_tmp109E)),((_tmp109E->name=cv->name,((_tmp109E->tq=cv->tq,((_tmp109E->type=
Cyc_Tcutil_copy_type(cv->type),((_tmp109E->inject=cv->inject,_tmp109E))))))))));}{
struct Cyc_List_List*_tmpFA=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);
struct Cyc_List_List*_tmpFB=_tmpA0;struct Cyc_Absyn_FnType_struct _tmp10A4;struct
Cyc_Absyn_FnInfo _tmp10A3;struct Cyc_Absyn_FnType_struct*_tmp10A2;return(void*)((
_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((_tmp10A2[0]=((_tmp10A4.tag=10,((_tmp10A4.f1=((
_tmp10A3.tvars=_tmpF4,((_tmp10A3.effect=_tmpF5,((_tmp10A3.ret_typ=_tmpF6,((
_tmp10A3.args=_tmpF7,((_tmp10A3.c_varargs=_tmpF8,((_tmp10A3.cyc_varargs=
cyc_varargs2,((_tmp10A3.rgn_po=_tmpFA,((_tmp10A3.attributes=_tmpFB,_tmp10A3)))))))))))))))),
_tmp10A4)))),_tmp10A2))));};}_LL39: {struct Cyc_Absyn_TupleType_struct*_tmpA1=(
struct Cyc_Absyn_TupleType_struct*)_tmp76;if(_tmpA1->tag != 11)goto _LL3B;else{
_tmpA2=_tmpA1->f1;}}_LL3A: {struct Cyc_Absyn_TupleType_struct _tmp10A7;struct Cyc_Absyn_TupleType_struct*
_tmp10A6;return(void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((
_tmp10A7.tag=11,((_tmp10A7.f1=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA2),
_tmp10A7)))),_tmp10A6))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA3=(
struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpA3->tag != 12)goto _LL3D;else{
_tmpA4=_tmpA3->f1;_tmpA5=_tmpA4.aggr_info;if((_tmpA5.UnknownAggr).tag != 1)goto
_LL3D;_tmpA6=(struct _tuple4)(_tmpA5.UnknownAggr).val;_tmpA7=_tmpA6.f1;_tmpA8=
_tmpA6.f2;_tmpA9=_tmpA6.f3;_tmpAA=_tmpA4.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmp10AD;struct Cyc_Absyn_AggrInfo _tmp10AC;struct Cyc_Absyn_AggrType_struct*
_tmp10AB;return(void*)((_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB[0]=((
_tmp10AD.tag=12,((_tmp10AD.f1=((_tmp10AC.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA7,
_tmpA8,_tmpA9),((_tmp10AC.targs=Cyc_Tcutil_copy_types(_tmpAA),_tmp10AC)))),
_tmp10AD)))),_tmp10AB))));}_LL3D: {struct Cyc_Absyn_AggrType_struct*_tmpAB=(
struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAB->tag != 12)goto _LL3F;else{
_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;if((_tmpAD.KnownAggr).tag != 2)goto
_LL3F;_tmpAE=(struct Cyc_Absyn_Aggrdecl**)(_tmpAD.KnownAggr).val;_tmpAF=_tmpAC.targs;}}
_LL3E: {struct Cyc_Absyn_AggrType_struct _tmp10B3;struct Cyc_Absyn_AggrInfo _tmp10B2;
struct Cyc_Absyn_AggrType_struct*_tmp10B1;return(void*)((_tmp10B1=_cycalloc(
sizeof(*_tmp10B1)),((_tmp10B1[0]=((_tmp10B3.tag=12,((_tmp10B3.f1=((_tmp10B2.aggr_info=
Cyc_Absyn_KnownAggr(_tmpAE),((_tmp10B2.targs=Cyc_Tcutil_copy_types(_tmpAF),
_tmp10B2)))),_tmp10B3)))),_tmp10B1))));}_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB0->tag != 13)goto
_LL41;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}_LL40: {struct Cyc_Absyn_AnonAggrType_struct
_tmp10B6;struct Cyc_Absyn_AnonAggrType_struct*_tmp10B5;return(void*)((_tmp10B5=
_cycalloc(sizeof(*_tmp10B5)),((_tmp10B5[0]=((_tmp10B6.tag=13,((_tmp10B6.f1=
_tmpB1,((_tmp10B6.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB2),_tmp10B6)))))),_tmp10B5))));}_LL41: {struct Cyc_Absyn_EnumType_struct*
_tmpB3=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB3->tag != 14)goto _LL43;
else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL42: {struct Cyc_Absyn_EnumType_struct
_tmp10B9;struct Cyc_Absyn_EnumType_struct*_tmp10B8;return(void*)((_tmp10B8=
_cycalloc(sizeof(*_tmp10B8)),((_tmp10B8[0]=((_tmp10B9.tag=14,((_tmp10B9.f1=
_tmpB4,((_tmp10B9.f2=_tmpB5,_tmp10B9)))))),_tmp10B8))));}_LL43: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB6->tag != 15)goto
_LL45;else{_tmpB7=_tmpB6->f1;}}_LL44: {struct Cyc_Absyn_AnonEnumType_struct
_tmp10BC;struct Cyc_Absyn_AnonEnumType_struct*_tmp10BB;return(void*)((_tmp10BB=
_cycalloc(sizeof(*_tmp10BB)),((_tmp10BB[0]=((_tmp10BC.tag=15,((_tmp10BC.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB7),_tmp10BC)))),
_tmp10BB))));}_LL45: {struct Cyc_Absyn_TagType_struct*_tmpB8=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB8->tag != 20)goto _LL47;else{_tmpB9=(void*)_tmpB8->f1;}}_LL46: {
struct Cyc_Absyn_TagType_struct _tmp10BF;struct Cyc_Absyn_TagType_struct*_tmp10BE;
return(void*)((_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE[0]=((_tmp10BF.tag=
20,((_tmp10BF.f1=(void*)Cyc_Tcutil_copy_type(_tmpB9),_tmp10BF)))),_tmp10BE))));}
_LL47: {struct Cyc_Absyn_ValueofType_struct*_tmpBA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBA->tag != 19)goto _LL49;else{_tmpBB=_tmpBA->f1;}}_LL48: {struct Cyc_Absyn_ValueofType_struct
_tmp10C2;struct Cyc_Absyn_ValueofType_struct*_tmp10C1;return(void*)((_tmp10C1=
_cycalloc(sizeof(*_tmp10C1)),((_tmp10C1[0]=((_tmp10C2.tag=19,((_tmp10C2.f1=
_tmpBB,_tmp10C2)))),_tmp10C1))));}_LL49: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBC->tag != 16)goto
_LL4B;else{_tmpBD=(void*)_tmpBC->f1;}}_LL4A: {struct Cyc_Absyn_RgnHandleType_struct
_tmp10C5;struct Cyc_Absyn_RgnHandleType_struct*_tmp10C4;return(void*)((_tmp10C4=
_cycalloc(sizeof(*_tmp10C4)),((_tmp10C4[0]=((_tmp10C5.tag=16,((_tmp10C5.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBD),_tmp10C5)))),_tmp10C4))));}_LL4B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBE=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBE->tag != 17)goto _LL4D;
else{_tmpBF=(void*)_tmpBE->f1;_tmpC0=(void*)_tmpBE->f2;}}_LL4C: {struct Cyc_Absyn_DynRgnType_struct
_tmp10C8;struct Cyc_Absyn_DynRgnType_struct*_tmp10C7;return(void*)((_tmp10C7=
_cycalloc(sizeof(*_tmp10C7)),((_tmp10C7[0]=((_tmp10C8.tag=17,((_tmp10C8.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBF),((_tmp10C8.f2=(void*)Cyc_Tcutil_copy_type(_tmpC0),
_tmp10C8)))))),_tmp10C7))));}_LL4D: {struct Cyc_Absyn_TypedefType_struct*_tmpC1=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC1->tag != 18)goto _LL4F;else{
_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;_tmpC4=_tmpC1->f3;}}_LL4E: {struct Cyc_Absyn_TypedefType_struct
_tmp10CB;struct Cyc_Absyn_TypedefType_struct*_tmp10CA;return(void*)((_tmp10CA=
_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA[0]=((_tmp10CB.tag=18,((_tmp10CB.f1=
_tmpC2,((_tmp10CB.f2=Cyc_Tcutil_copy_types(_tmpC3),((_tmp10CB.f3=_tmpC4,((
_tmp10CB.f4=0,_tmp10CB)))))))))),_tmp10CA))));}_LL4F: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC5=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC5->tag != 22)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_struct*_tmpC6=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp76;if(_tmpC6->tag != 21)goto _LL53;}_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_struct*
_tmpC7=(struct Cyc_Absyn_AccessEff_struct*)_tmp76;if(_tmpC7->tag != 23)goto _LL55;
else{_tmpC8=(void*)_tmpC7->f1;}}_LL54: {struct Cyc_Absyn_AccessEff_struct _tmp10CE;
struct Cyc_Absyn_AccessEff_struct*_tmp10CD;return(void*)((_tmp10CD=_cycalloc(
sizeof(*_tmp10CD)),((_tmp10CD[0]=((_tmp10CE.tag=23,((_tmp10CE.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC8),_tmp10CE)))),_tmp10CD))));}_LL55: {struct Cyc_Absyn_JoinEff_struct*_tmpC9=(
struct Cyc_Absyn_JoinEff_struct*)_tmp76;if(_tmpC9->tag != 24)goto _LL57;else{_tmpCA=
_tmpC9->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_struct _tmp10D1;struct Cyc_Absyn_JoinEff_struct*
_tmp10D0;return(void*)((_tmp10D0=_cycalloc(sizeof(*_tmp10D0)),((_tmp10D0[0]=((
_tmp10D1.tag=24,((_tmp10D1.f1=Cyc_Tcutil_copy_types(_tmpCA),_tmp10D1)))),
_tmp10D0))));}_LL57: {struct Cyc_Absyn_RgnsEff_struct*_tmpCB=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp76;if(_tmpCB->tag != 25)goto _LL59;else{_tmpCC=(void*)_tmpCB->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_struct _tmp10D4;struct Cyc_Absyn_RgnsEff_struct*_tmp10D3;
return(void*)((_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((_tmp10D3[0]=((_tmp10D4.tag=
25,((_tmp10D4.f1=(void*)Cyc_Tcutil_copy_type(_tmpCC),_tmp10D4)))),_tmp10D3))));}
_LL59: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCD=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCD->tag != 26)goto _LL5B;else{_tmpCE=_tmpCD->f1;_tmpCF=*_tmpCE;
_tmpD0=_tmpCF.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD1=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD0;if(_tmpD1->tag != 0)goto _LL5B;else{_tmpD2=_tmpD1->f1;}};}}_LL5A: {struct
Cyc_List_List*_tmp11E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);struct Cyc_Absyn_AggrType_struct
_tmp10DA;struct Cyc_Absyn_AggrInfo _tmp10D9;struct Cyc_Absyn_AggrType_struct*
_tmp10D8;return(void*)((_tmp10D8=_cycalloc(sizeof(*_tmp10D8)),((_tmp10D8[0]=((
_tmp10DA.tag=12,((_tmp10DA.f1=((_tmp10D9.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD2->kind,
_tmpD2->name,0),((_tmp10D9.targs=_tmp11E,_tmp10D9)))),_tmp10DA)))),_tmp10D8))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD3=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD3->tag != 26)goto _LL5D;else{_tmpD4=_tmpD3->f1;_tmpD5=*_tmpD4;
_tmpD6=_tmpD5.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD7=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD6;if(_tmpD7->tag != 1)goto _LL5D;else{_tmpD8=_tmpD7->f1;}};}}_LL5C: {struct
Cyc_Absyn_EnumType_struct _tmp10DD;struct Cyc_Absyn_EnumType_struct*_tmp10DC;
return(void*)((_tmp10DC=_cycalloc(sizeof(*_tmp10DC)),((_tmp10DC[0]=((_tmp10DD.tag=
14,((_tmp10DD.f1=_tmpD8->name,((_tmp10DD.f2=0,_tmp10DD)))))),_tmp10DC))));}_LL5D: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpD9=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD9->tag != 26)goto _LL22;else{_tmpDA=_tmpD9->f1;_tmpDB=*_tmpDA;
_tmpDC=_tmpDB.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDD=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDC;if(_tmpDD->tag != 2)goto _LL22;else{_tmpDE=_tmpDD->f1;}};}}_LL5E: {struct
Cyc_List_List*_tmp124=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDE->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp10E7;struct Cyc_Absyn_UnknownDatatypeInfo _tmp10E6;struct Cyc_Absyn_DatatypeInfo
_tmp10E5;struct Cyc_Absyn_DatatypeType_struct*_tmp10E4;return(void*)((_tmp10E4=
_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4[0]=((_tmp10E7.tag=3,((_tmp10E7.f1=((
_tmp10E5.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp10E6.name=_tmpDE->name,((
_tmp10E6.is_extensible=0,_tmp10E6))))),((_tmp10E5.targs=_tmp124,_tmp10E5)))),
_tmp10E7)))),_tmp10E4))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp129=d;struct Cyc_Absyn_Exp*_tmp12B;struct _dyneither_ptr*_tmp12D;_LL60: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp129;if(_tmp12A->tag != 0)goto _LL62;else{_tmp12B=_tmp12A->f1;}}_LL61: {struct
Cyc_Absyn_ArrayElement_struct _tmp10EA;struct Cyc_Absyn_ArrayElement_struct*
_tmp10E9;return(void*)((_tmp10E9=_cycalloc(sizeof(*_tmp10E9)),((_tmp10E9[0]=((
_tmp10EA.tag=0,((_tmp10EA.f1=Cyc_Tcutil_deep_copy_exp(_tmp12B),_tmp10EA)))),
_tmp10E9))));}_LL62: {struct Cyc_Absyn_FieldName_struct*_tmp12C=(struct Cyc_Absyn_FieldName_struct*)
_tmp129;if(_tmp12C->tag != 1)goto _LL5F;else{_tmp12D=_tmp12C->f1;}}_LL63: return d;
_LL5F:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e){struct _tuple18*_tmp10EB;
return(_tmp10EB=_cycalloc(sizeof(*_tmp10EB)),((_tmp10EB->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp10EB->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp10EB)))));}
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
_tmp13F=_tmp13C->f3;}}_LL6E: {struct Cyc_Core_Opt*_tmp10EC;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13D),(unsigned int)_tmp13E?(_tmp10EC=
_cycalloc_atomic(sizeof(*_tmp10EC)),((_tmp10EC->v=(void*)((enum Cyc_Absyn_Primop)
_tmp13E->v),_tmp10EC))): 0,Cyc_Tcutil_deep_copy_exp(_tmp13F),e->loc);}_LL6F: {
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
struct Cyc_Absyn_FnCall_e_struct _tmp10F6;struct Cyc_Absyn_VarargInfo*_tmp10F5;
struct Cyc_Absyn_VarargCallInfo*_tmp10F4;struct Cyc_Absyn_FnCall_e_struct*_tmp10F3;
return Cyc_Absyn_new_exp((void*)((_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((
_tmp10F3[0]=((_tmp10F6.tag=11,((_tmp10F6.f1=Cyc_Tcutil_deep_copy_exp(_tmp154),((
_tmp10F6.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp155),((
_tmp10F6.f3=((_tmp10F4=_cycalloc(sizeof(*_tmp10F4)),((_tmp10F4->num_varargs=
_tmp1BB,((_tmp10F4->injectors=_tmp1BC,((_tmp10F4->vai=((_tmp10F5=_cycalloc(
sizeof(*_tmp10F5)),((_tmp10F5->name=_tmp1C0,((_tmp10F5->tq=_tmp1C1,((_tmp10F5->type=
Cyc_Tcutil_copy_type(_tmp1C2),((_tmp10F5->inject=_tmp1C3,_tmp10F5)))))))))),
_tmp10F4)))))))),_tmp10F6)))))))),_tmp10F3)))),e->loc);};}_LLBA:;_LLBB: {struct
Cyc_Absyn_FnCall_e_struct _tmp10F9;struct Cyc_Absyn_FnCall_e_struct*_tmp10F8;
return Cyc_Absyn_new_exp((void*)((_tmp10F8=_cycalloc(sizeof(*_tmp10F8)),((
_tmp10F8[0]=((_tmp10F9.tag=11,((_tmp10F9.f1=Cyc_Tcutil_deep_copy_exp(_tmp154),((
_tmp10F9.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp155),((
_tmp10F9.f3=_tmp156,_tmp10F9)))))))),_tmp10F8)))),e->loc);}_LLB7:;}_LL7D: {
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
_tmp171->f4;}}_LL92: {struct Cyc_Absyn_AggrMember_e_struct _tmp10FC;struct Cyc_Absyn_AggrMember_e_struct*
_tmp10FB;return Cyc_Absyn_new_exp((void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((
_tmp10FB[0]=((_tmp10FC.tag=22,((_tmp10FC.f1=Cyc_Tcutil_deep_copy_exp(_tmp172),((
_tmp10FC.f2=_tmp173,((_tmp10FC.f3=_tmp174,((_tmp10FC.f4=_tmp175,_tmp10FC)))))))))),
_tmp10FB)))),e->loc);}_LL93: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp176=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp131;if(_tmp176->tag != 23)goto _LL95;else{
_tmp177=_tmp176->f1;_tmp178=_tmp176->f2;_tmp179=_tmp176->f3;_tmp17A=_tmp176->f4;}}
_LL94: {struct Cyc_Absyn_AggrArrow_e_struct _tmp10FF;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp10FE;return Cyc_Absyn_new_exp((void*)((_tmp10FE=_cycalloc(sizeof(*_tmp10FE)),((
_tmp10FE[0]=((_tmp10FF.tag=23,((_tmp10FF.f1=Cyc_Tcutil_deep_copy_exp(_tmp177),((
_tmp10FF.f2=_tmp178,((_tmp10FF.f3=_tmp179,((_tmp10FF.f4=_tmp17A,_tmp10FF)))))))))),
_tmp10FE)))),e->loc);}_LL95: {struct Cyc_Absyn_Subscript_e_struct*_tmp17B=(struct
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
_tmp1100;vopt1=((_tmp1100=_cycalloc(sizeof(*_tmp1100)),((_tmp1100->v=(struct
_dyneither_ptr*)_tmp183->v,_tmp1100))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp1106;struct _tuple9*_tmp1105;struct Cyc_Absyn_CompoundLit_e_struct*_tmp1104;
return Cyc_Absyn_new_exp((void*)((_tmp1104=_cycalloc(sizeof(*_tmp1104)),((
_tmp1104[0]=((_tmp1106.tag=26,((_tmp1106.f1=((_tmp1105=_cycalloc(sizeof(*
_tmp1105)),((_tmp1105->f1=vopt1,((_tmp1105->f2=_tmp184,((_tmp1105->f3=Cyc_Tcutil_copy_type(
_tmp185),_tmp1105)))))))),((_tmp1106.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp186),_tmp1106)))))),_tmp1104)))),e->loc);};}_LL9B: {struct Cyc_Absyn_Array_e_struct*
_tmp187=(struct Cyc_Absyn_Array_e_struct*)_tmp131;if(_tmp187->tag != 27)goto _LL9D;
else{_tmp188=_tmp187->f1;}}_LL9C: {struct Cyc_Absyn_Array_e_struct _tmp1109;struct
Cyc_Absyn_Array_e_struct*_tmp1108;return Cyc_Absyn_new_exp((void*)((_tmp1108=
_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=27,((_tmp1109.f1=((
struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp188),_tmp1109)))),_tmp1108)))),e->loc);}
_LL9D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp189=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp131;if(_tmp189->tag != 28)goto _LL9F;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;
_tmp18C=_tmp189->f3;_tmp18D=_tmp189->f4;}}_LL9E: {struct Cyc_Absyn_Comprehension_e_struct
_tmp110C;struct Cyc_Absyn_Comprehension_e_struct*_tmp110B;return Cyc_Absyn_new_exp((
void*)((_tmp110B=_cycalloc(sizeof(*_tmp110B)),((_tmp110B[0]=((_tmp110C.tag=28,((
_tmp110C.f1=_tmp18A,((_tmp110C.f2=Cyc_Tcutil_deep_copy_exp(_tmp18B),((_tmp110C.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18C),((_tmp110C.f4=_tmp18D,_tmp110C)))))))))),
_tmp110B)))),e->loc);}_LL9F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp18E=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp131;if(_tmp18E->tag != 29)goto _LLA1;else{
_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;_tmp191=_tmp18E->f3;_tmp192=_tmp18E->f4;}}
_LLA0: {struct Cyc_Absyn_Aggregate_e_struct _tmp110F;struct Cyc_Absyn_Aggregate_e_struct*
_tmp110E;return Cyc_Absyn_new_exp((void*)((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((
_tmp110E[0]=((_tmp110F.tag=29,((_tmp110F.f1=_tmp18F,((_tmp110F.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp190),((_tmp110F.f3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp191),((
_tmp110F.f4=_tmp192,_tmp110F)))))))))),_tmp110E)))),e->loc);}_LLA1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp193=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp131;if(_tmp193->tag != 30)goto
_LLA3;else{_tmp194=(void*)_tmp193->f1;_tmp195=_tmp193->f2;}}_LLA2: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp1112;struct Cyc_Absyn_AnonStruct_e_struct*_tmp1111;return Cyc_Absyn_new_exp((
void*)((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111[0]=((_tmp1112.tag=30,((
_tmp1112.f1=(void*)Cyc_Tcutil_copy_type(_tmp194),((_tmp1112.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp195),_tmp1112)))))),_tmp1111)))),e->loc);}_LLA3: {struct Cyc_Absyn_Datatype_e_struct*
_tmp196=(struct Cyc_Absyn_Datatype_e_struct*)_tmp131;if(_tmp196->tag != 31)goto
_LLA5;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;_tmp199=_tmp196->f3;}}_LLA4: {
struct Cyc_Absyn_Datatype_e_struct _tmp1115;struct Cyc_Absyn_Datatype_e_struct*
_tmp1114;return Cyc_Absyn_new_exp((void*)((_tmp1114=_cycalloc(sizeof(*_tmp1114)),((
_tmp1114[0]=((_tmp1115.tag=31,((_tmp1115.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp197),((_tmp1115.f2=_tmp198,((_tmp1115.f3=_tmp199,_tmp1115)))))))),_tmp1114)))),
e->loc);}_LLA5: {struct Cyc_Absyn_Enum_e_struct*_tmp19A=(struct Cyc_Absyn_Enum_e_struct*)
_tmp131;if(_tmp19A->tag != 32)goto _LLA7;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;
_tmp19D=_tmp19A->f3;}}_LLA6: return e;_LLA7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp19E=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp131;if(_tmp19E->tag != 33)goto
_LLA9;else{_tmp19F=_tmp19E->f1;_tmp1A0=(void*)_tmp19E->f2;_tmp1A1=_tmp19E->f3;}}
_LLA8: {struct Cyc_Absyn_AnonEnum_e_struct _tmp1118;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1117;return Cyc_Absyn_new_exp((void*)((_tmp1117=_cycalloc(sizeof(*_tmp1117)),((
_tmp1117[0]=((_tmp1118.tag=33,((_tmp1118.f1=_tmp19F,((_tmp1118.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A0),((_tmp1118.f3=_tmp1A1,_tmp1118)))))))),_tmp1117)))),e->loc);}_LLA9: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A2=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp131;if(_tmp1A2->tag != 34)goto _LLAB;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A3.is_calloc;
_tmp1A5=_tmp1A3.rgn;_tmp1A6=_tmp1A3.elt_type;_tmp1A7=_tmp1A3.num_elts;_tmp1A8=
_tmp1A3.fat_result;}}_LLAA: {struct Cyc_Absyn_Exp*_tmp1DE=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A5;if(_tmp1A5 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A5);{void**t1=_tmp1A6;if(_tmp1A6 != 0){void**_tmp1119;
t1=((_tmp1119=_cycalloc(sizeof(*_tmp1119)),((_tmp1119[0]=Cyc_Tcutil_copy_type(*
_tmp1A6),_tmp1119))));}{struct Cyc_Absyn_Malloc_e_struct _tmp111F;struct Cyc_Absyn_MallocInfo
_tmp111E;struct Cyc_Absyn_Malloc_e_struct*_tmp111D;_tmp1DE->r=(void*)((_tmp111D=
_cycalloc(sizeof(*_tmp111D)),((_tmp111D[0]=((_tmp111F.tag=34,((_tmp111F.f1=((
_tmp111E.is_calloc=_tmp1A4,((_tmp111E.rgn=r1,((_tmp111E.elt_type=t1,((_tmp111E.num_elts=
_tmp1A7,((_tmp111E.fat_result=_tmp1A8,_tmp111E)))))))))),_tmp111F)))),_tmp111D))));}
return _tmp1DE;};}_LLAB: {struct Cyc_Absyn_Swap_e_struct*_tmp1A9=(struct Cyc_Absyn_Swap_e_struct*)
_tmp131;if(_tmp1A9->tag != 35)goto _LLAD;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}
_LLAC: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AA),Cyc_Tcutil_deep_copy_exp(
_tmp1AB),e->loc);_LLAD: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AC=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp131;if(_tmp1AC->tag != 36)goto _LLAF;else{
_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LLAE: {struct Cyc_Core_Opt*nopt1=
_tmp1AD;if(_tmp1AD != 0){struct Cyc_Core_Opt*_tmp1120;nopt1=((_tmp1120=_cycalloc(
sizeof(*_tmp1120)),((_tmp1120->v=(struct _tuple2*)_tmp1AD->v,_tmp1120))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp1123;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1122;return Cyc_Absyn_new_exp((void*)((_tmp1122=_cycalloc(sizeof(*_tmp1122)),((
_tmp1122[0]=((_tmp1123.tag=36,((_tmp1123.f1=nopt1,((_tmp1123.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1AE),_tmp1123)))))),_tmp1122)))),e->loc);};}_LLAF: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1AF=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp131;if(_tmp1AF->tag != 37)goto
_LLB1;}_LLB0: {struct Cyc_Core_Failure_struct _tmp1129;const char*_tmp1128;struct
Cyc_Core_Failure_struct*_tmp1127;(int)_throw((void*)((_tmp1127=_cycalloc(sizeof(*
_tmp1127)),((_tmp1127[0]=((_tmp1129.tag=Cyc_Core_Failure,((_tmp1129.f1=((
_tmp1128="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp1128,
sizeof(char),45))),_tmp1129)))),_tmp1127)))));}_LLB1: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B0=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp131;if(_tmp1B0->tag != 38)goto
_LLB3;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LLB2: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp112C;struct Cyc_Absyn_Tagcheck_e_struct*_tmp112B;return Cyc_Absyn_new_exp((
void*)((_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B[0]=((_tmp112C.tag=38,((
_tmp112C.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B1),((_tmp112C.f2=_tmp1B2,_tmp112C)))))),
_tmp112B)))),e->loc);}_LLB3: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B3=(struct
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
_tmp1ED != _tmp1F1){struct _tuple19 _tmp112D;struct _tuple19 _tmp1F4=(_tmp112D.f1=
_tmp1ED,((_tmp112D.f2=_tmp1F1,_tmp112D)));enum Cyc_Absyn_KindQual _tmp1F5;enum 
Cyc_Absyn_KindQual _tmp1F6;enum Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual
_tmp1F8;enum Cyc_Absyn_KindQual _tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;_LLBD:
_tmp1F5=_tmp1F4.f1;if(_tmp1F5 != Cyc_Absyn_BoxKind)goto _LLBF;_tmp1F6=_tmp1F4.f2;
if(_tmp1F6 != Cyc_Absyn_MemKind)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1F7=_tmp1F4.f1;
if(_tmp1F7 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F8=_tmp1F4.f2;if(_tmp1F8 != Cyc_Absyn_AnyKind)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1F9=_tmp1F4.f1;if(_tmp1F9 != Cyc_Absyn_MemKind)
goto _LLC3;_tmp1FA=_tmp1F4.f2;if(_tmp1FA != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto
_LLBC;_LLC3:;_LLC4: return 0;_LLBC:;}if(_tmp1EE != _tmp1F2){struct _tuple20 _tmp112E;
struct _tuple20 _tmp1FC=(_tmp112E.f1=_tmp1EE,((_tmp112E.f2=_tmp1F2,_tmp112E)));
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
_LLD1:;_LLD2:{struct Cyc_Absyn_Less_kb_struct _tmp1131;struct Cyc_Absyn_Less_kb_struct*
_tmp1130;tv->kind=(void*)((_tmp1130=_cycalloc(sizeof(*_tmp1130)),((_tmp1130[0]=((
_tmp1131.tag=2,((_tmp1131.f1=0,((_tmp1131.f2=def,_tmp1131)))))),_tmp1130))));}
return def;_LLCC:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp1132;struct _tuple0 _tmp209=(_tmp1132.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1132.f2=
Cyc_Absyn_compress_kb(kb2),_tmp1132)));void*_tmp20A;struct Cyc_Absyn_Kind*_tmp20C;
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
struct Cyc_Core_Opt*_tmp1133;*_tmp213=((_tmp1133=_cycalloc(sizeof(*_tmp1133)),((
_tmp1133->v=kb2,_tmp1133))));}return 1;}else{return 0;}_LLD8: _tmp218=_tmp209.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp219=(struct Cyc_Absyn_Eq_kb_struct*)_tmp218;if(
_tmp219->tag != 0)goto _LLDA;else{_tmp21A=_tmp219->f1;}};_tmp21B=_tmp209.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21C=(struct Cyc_Absyn_Less_kb_struct*)_tmp21B;
if(_tmp21C->tag != 2)goto _LLDA;else{_tmp21D=_tmp21C->f1;_tmp21E=(struct Cyc_Core_Opt**)&
_tmp21C->f1;_tmp21F=_tmp21C->f2;}};_LLD9: if(Cyc_Tcutil_kind_leq(_tmp21A,_tmp21F)){{
struct Cyc_Core_Opt*_tmp1134;*_tmp21E=((_tmp1134=_cycalloc(sizeof(*_tmp1134)),((
_tmp1134->v=kb1,_tmp1134))));}return 1;}else{return 0;}_LLDA: _tmp220=_tmp209.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp221=(struct Cyc_Absyn_Less_kb_struct*)_tmp220;
if(_tmp221->tag != 2)goto _LLDC;else{_tmp222=_tmp221->f1;_tmp223=(struct Cyc_Core_Opt**)&
_tmp221->f1;_tmp224=_tmp221->f2;}};_tmp225=_tmp209.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp226=(struct Cyc_Absyn_Less_kb_struct*)_tmp225;if(_tmp226->tag != 2)goto _LLDC;
else{_tmp227=_tmp226->f1;_tmp228=(struct Cyc_Core_Opt**)& _tmp226->f1;_tmp229=
_tmp226->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp224,_tmp229)){{struct Cyc_Core_Opt*
_tmp1135;*_tmp228=((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135->v=kb1,
_tmp1135))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp229,_tmp224)){{struct Cyc_Core_Opt*
_tmp1136;*_tmp223=((_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136->v=kb2,
_tmp1136))));}return 1;}else{return 0;}}_LLDC: _tmp22A=_tmp209.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22B=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22A;if(_tmp22B->tag != 1)goto
_LLDE;else{_tmp22C=_tmp22B->f1;_tmp22D=(struct Cyc_Core_Opt**)& _tmp22B->f1;}};
_tmp22E=_tmp209.f2;_LLDD: _tmp22F=_tmp22E;_tmp233=_tmp22D;goto _LLDF;_LLDE: _tmp22F=
_tmp209.f1;_tmp230=_tmp209.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp231=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp230;if(_tmp231->tag != 1)goto _LLD3;else{_tmp232=
_tmp231->f1;_tmp233=(struct Cyc_Core_Opt**)& _tmp231->f1;}};_LLDF:{struct Cyc_Core_Opt*
_tmp1137;*_tmp233=((_tmp1137=_cycalloc(sizeof(*_tmp1137)),((_tmp1137->v=_tmp22F,
_tmp1137))));}return 1;_LLD3:;}struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp239=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23B;_LLE1: {struct
Cyc_Absyn_VarType_struct*_tmp23A=(struct Cyc_Absyn_VarType_struct*)_tmp239;if(
_tmp23A->tag != 2)goto _LLE3;else{_tmp23B=_tmp23A->f1;}}_LLE2: {void*_tmp23C=
_tmp23B->kind;_tmp23B->kind=kb;{struct _tuple15 _tmp1138;return(_tmp1138.f1=
_tmp23B,((_tmp1138.f2=_tmp23C,_tmp1138)));};}_LLE3:;_LLE4: {const char*_tmp113C;
void*_tmp113B[1];struct Cyc_String_pa_struct _tmp113A;(_tmp113A.tag=0,((_tmp113A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp113B[0]=& _tmp113A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp113C="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp113C,sizeof(char),40))),_tag_dyneither(_tmp113B,sizeof(void*),
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
_LL105: {const char*_tmp113F;void*_tmp113E;(_tmp113E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp113F="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp113F,sizeof(char),39))),_tag_dyneither(_tmp113E,sizeof(void*),
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
_LL127;}_LL126: {struct Cyc_Absyn_Kind*_tmp1140;return(_tmp1140=_cycalloc_atomic(
sizeof(*_tmp1140)),((_tmp1140->kind=Cyc_Absyn_MemKind,((_tmp1140->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26A.ptr_atts).rgn))->aliasqual,_tmp1140)))));}_LL127: {struct Cyc_Absyn_Upper_b_struct*
_tmp287=(struct Cyc_Absyn_Upper_b_struct*)_tmp285;if(_tmp287->tag != 1)goto _LL124;}
_LL128: {struct Cyc_Absyn_Kind*_tmp1141;return(_tmp1141=_cycalloc_atomic(sizeof(*
_tmp1141)),((_tmp1141->kind=Cyc_Absyn_BoxKind,((_tmp1141->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26A.ptr_atts).rgn))->aliasqual,_tmp1141)))));}_LL124:;}_LL10E: {struct Cyc_Absyn_ValueofType_struct*
_tmp26B=(struct Cyc_Absyn_ValueofType_struct*)_tmp241;if(_tmp26B->tag != 19)goto
_LL110;}_LL10F: return& Cyc_Tcutil_ik;_LL110: {struct Cyc_Absyn_TagType_struct*
_tmp26C=(struct Cyc_Absyn_TagType_struct*)_tmp241;if(_tmp26C->tag != 20)goto _LL112;}
_LL111: return& Cyc_Tcutil_bk;_LL112: {struct Cyc_Absyn_ArrayType_struct*_tmp26D=(
struct Cyc_Absyn_ArrayType_struct*)_tmp241;if(_tmp26D->tag != 9)goto _LL114;}_LL113:
goto _LL115;_LL114: {struct Cyc_Absyn_TupleType_struct*_tmp26E=(struct Cyc_Absyn_TupleType_struct*)
_tmp241;if(_tmp26E->tag != 11)goto _LL116;}_LL115: return& Cyc_Tcutil_mk;_LL116: {
struct Cyc_Absyn_TypedefType_struct*_tmp26F=(struct Cyc_Absyn_TypedefType_struct*)
_tmp241;if(_tmp26F->tag != 18)goto _LL118;else{_tmp270=_tmp26F->f3;}}_LL117: if(
_tmp270 == 0  || _tmp270->kind == 0){const char*_tmp1145;void*_tmp1144[1];struct Cyc_String_pa_struct
_tmp1143;(_tmp1143.tag=0,((_tmp1143.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1144[0]=& _tmp1143,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1145="typ_kind: typedef found: %s",
_tag_dyneither(_tmp1145,sizeof(char),28))),_tag_dyneither(_tmp1144,sizeof(void*),
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
Cyc_Tcutil_fast_tvar_cmp,env,_tmp296)){{const char*_tmp1146;Cyc_Tcutil_failure_reason=((
_tmp1146="(type variable would escape scope)",_tag_dyneither(_tmp1146,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL12E;_LL131: {
struct Cyc_Absyn_Evar_struct*_tmp297=(struct Cyc_Absyn_Evar_struct*)_tmp294;if(
_tmp297->tag != 1)goto _LL133;else{_tmp298=_tmp297->f2;_tmp299=_tmp297->f4;_tmp29A=(
struct Cyc_Core_Opt**)& _tmp297->f4;}}_LL132: if(t == evar){{const char*_tmp1147;Cyc_Tcutil_failure_reason=((
_tmp1147="(occurs check)",_tag_dyneither(_tmp1147,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp298 != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp298->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp29A))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2C5=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp29A))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp1148;_tmp2C5=((
_tmp1148=_cycalloc(sizeof(*_tmp1148)),((_tmp1148->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp1148->tl=_tmp2C5,_tmp1148))))));}}}{struct Cyc_Core_Opt*_tmp1149;*_tmp29A=((
_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149->v=_tmp2C5,_tmp1149))));};}}}
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
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp114C;void*
_tmp114B;(_tmp114B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp114C="tq1 real_const not set.",_tag_dyneither(_tmp114C,
sizeof(char),24))),_tag_dyneither(_tmp114B,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp114F;void*_tmp114E;(_tmp114E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp114F="tq2 real_const not set.",
_tag_dyneither(_tmp114F,sizeof(char),24))),_tag_dyneither(_tmp114E,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp1150;Cyc_Tcutil_failure_reason=((_tmp1150="(qualifiers don't match)",
_tag_dyneither(_tmp1150,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2D1=x;union Cyc_Absyn_Constraint _tmp2D2;int _tmp2D3;union Cyc_Absyn_Constraint
_tmp2D4;void*_tmp2D5;union Cyc_Absyn_Constraint _tmp2D6;union Cyc_Absyn_Constraint*
_tmp2D7;_LL152: _tmp2D2=*_tmp2D1;if((_tmp2D2.No_constr).tag != 3)goto _LL154;
_tmp2D3=(int)(_tmp2D2.No_constr).val;_LL153:{union Cyc_Absyn_Constraint _tmp1151;*
x=(((_tmp1151.Forward_constr).val=y,(((_tmp1151.Forward_constr).tag=2,_tmp1151))));}
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
char*_tmp1154;void*_tmp1153;(_tmp1153=0,Cyc_Tcutil_impos(((_tmp1154="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp1154,sizeof(char),40))),_tag_dyneither(_tmp1153,sizeof(void*),
0)));}_LL158:;}_LL156: _tmp2D6=*_tmp2D1;if((_tmp2D6.Forward_constr).tag != 2)goto
_LL151;_tmp2D7=(union Cyc_Absyn_Constraint*)(_tmp2D6.Forward_constr).val;_LL157: {
const char*_tmp1157;void*_tmp1156;(_tmp1156=0,Cyc_Tcutil_impos(((_tmp1157="unify_conref: forward after compress",
_tag_dyneither(_tmp1157,sizeof(char),37))),_tag_dyneither(_tmp1156,sizeof(void*),
0)));}_LL151:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2E4;
_push_handler(& _tmp2E4);{int _tmp2E6=0;if(setjmp(_tmp2E4.handler))_tmp2E6=1;if(!
_tmp2E6){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2E7=1;_npop_handler(0);return _tmp2E7;};;
_pop_handler();}else{void*_tmp2E5=(void*)_exn_thrown;void*_tmp2E9=_tmp2E5;_LL160: {
struct Cyc_Tcutil_Unify_struct*_tmp2EA=(struct Cyc_Tcutil_Unify_struct*)_tmp2E9;
if(_tmp2EA->tag != Cyc_Tcutil_Unify)goto _LL162;}_LL161: return 0;_LL162:;_LL163:(
void)_throw(_tmp2E9);_LL15F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp1158;struct _tuple0 _tmp2EC=(_tmp1158.f1=b1,((_tmp1158.f2=b2,
_tmp1158)));void*_tmp2ED;void*_tmp2EF;void*_tmp2F1;void*_tmp2F3;void*_tmp2F5;
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
_tmp1159;struct _tuple0 _tmp2FC=(_tmp1159.f1=b1,((_tmp1159.f2=b2,_tmp1159)));void*
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
void*att1,void*att2){struct _tuple0 _tmp115A;struct _tuple0 _tmp322=(_tmp115A.f1=
att1,((_tmp115A.f2=att2,_tmp115A)));void*_tmp323;int _tmp325;void*_tmp326;int
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
_tmp115D;struct Cyc_Absyn_ValueofType_struct*_tmp115C;t=(void*)((_tmp115C=
_cycalloc(sizeof(*_tmp115C)),((_tmp115C[0]=((_tmp115D.tag=19,((_tmp115D.f1=Cyc_Absyn_uint_exp(
0,0),_tmp115D)))),_tmp115C))));}goto _LL1B0;_LL1B9:;_LL1BA: t=Cyc_Absyn_sint_typ;
goto _LL1B0;_LL1B0:;}{struct _tuple15*_tmp115E;return(_tmp115E=_cycalloc(sizeof(*
_tmp115E)),((_tmp115E->f1=tv,((_tmp115E->f2=t,_tmp115E)))));};}static void*Cyc_Tcutil_rgns_of(
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
_LL1F4: {struct Cyc_Absyn_AccessEff_struct _tmp1161;struct Cyc_Absyn_AccessEff_struct*
_tmp1160;return(void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160[0]=((
_tmp1161.tag=23,((_tmp1161.f1=(void*)t,_tmp1161)))),_tmp1160))));}_LL1F5: _tmp38E=*
_tmp38B;_tmp38F=_tmp38E.kind;if(_tmp38F != Cyc_Absyn_EffKind)goto _LL1F7;_LL1F6:
return t;_LL1F7: _tmp390=*_tmp38B;_tmp391=_tmp390.kind;if(_tmp391 != Cyc_Absyn_IntKind)
goto _LL1F9;_LL1F8: return Cyc_Absyn_empty_effect;_LL1F9:;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct
_tmp1164;struct Cyc_Absyn_RgnsEff_struct*_tmp1163;return(void*)((_tmp1163=
_cycalloc(sizeof(*_tmp1163)),((_tmp1163[0]=((_tmp1164.tag=25,((_tmp1164.f1=(void*)
t,_tmp1164)))),_tmp1163))));}_LL1F2:;}_LL1CE: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp35F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp355;if(_tmp35F->tag != 16)goto
_LL1D0;else{_tmp360=(void*)_tmp35F->f1;}}_LL1CF: {struct Cyc_Absyn_AccessEff_struct
_tmp1167;struct Cyc_Absyn_AccessEff_struct*_tmp1166;return(void*)((_tmp1166=
_cycalloc(sizeof(*_tmp1166)),((_tmp1166[0]=((_tmp1167.tag=23,((_tmp1167.f1=(void*)
_tmp360,_tmp1167)))),_tmp1166))));}_LL1D0: {struct Cyc_Absyn_DynRgnType_struct*
_tmp361=(struct Cyc_Absyn_DynRgnType_struct*)_tmp355;if(_tmp361->tag != 17)goto
_LL1D2;else{_tmp362=(void*)_tmp361->f1;_tmp363=(void*)_tmp361->f2;}}_LL1D1: {
struct Cyc_Absyn_AccessEff_struct _tmp116A;struct Cyc_Absyn_AccessEff_struct*
_tmp1169;return(void*)((_tmp1169=_cycalloc(sizeof(*_tmp1169)),((_tmp1169[0]=((
_tmp116A.tag=23,((_tmp116A.f1=(void*)_tmp363,_tmp116A)))),_tmp1169))));}_LL1D2: {
struct Cyc_Absyn_DatatypeType_struct*_tmp364=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp355;if(_tmp364->tag != 3)goto _LL1D4;else{_tmp365=_tmp364->f1;_tmp366=_tmp365.targs;}}
_LL1D3: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp366);struct Cyc_Absyn_JoinEff_struct
_tmp116D;struct Cyc_Absyn_JoinEff_struct*_tmp116C;return Cyc_Tcutil_normalize_effect((
void*)((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((_tmp116C[0]=((_tmp116D.tag=24,((
_tmp116D.f1=ts,_tmp116D)))),_tmp116C)))));}_LL1D4: {struct Cyc_Absyn_PointerType_struct*
_tmp367=(struct Cyc_Absyn_PointerType_struct*)_tmp355;if(_tmp367->tag != 5)goto
_LL1D6;else{_tmp368=_tmp367->f1;_tmp369=_tmp368.elt_typ;_tmp36A=_tmp368.ptr_atts;
_tmp36B=_tmp36A.rgn;}}_LL1D5: {struct Cyc_Absyn_JoinEff_struct _tmp117C;struct Cyc_Absyn_AccessEff_struct*
_tmp117B;struct Cyc_Absyn_AccessEff_struct _tmp117A;void*_tmp1179[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1178;return Cyc_Tcutil_normalize_effect((void*)((_tmp1178=_cycalloc(sizeof(*
_tmp1178)),((_tmp1178[0]=((_tmp117C.tag=24,((_tmp117C.f1=((_tmp1179[1]=Cyc_Tcutil_rgns_of(
_tmp369),((_tmp1179[0]=(void*)((_tmp117B=_cycalloc(sizeof(*_tmp117B)),((_tmp117B[
0]=((_tmp117A.tag=23,((_tmp117A.f1=(void*)_tmp36B,_tmp117A)))),_tmp117B)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1179,sizeof(void*),2)))))),_tmp117C)))),_tmp1178)))));}_LL1D6: {struct Cyc_Absyn_ArrayType_struct*
_tmp36C=(struct Cyc_Absyn_ArrayType_struct*)_tmp355;if(_tmp36C->tag != 9)goto
_LL1D8;else{_tmp36D=_tmp36C->f1;_tmp36E=_tmp36D.elt_type;}}_LL1D7: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp36E));_LL1D8: {struct Cyc_Absyn_TupleType_struct*_tmp36F=(
struct Cyc_Absyn_TupleType_struct*)_tmp355;if(_tmp36F->tag != 11)goto _LL1DA;else{
_tmp370=_tmp36F->f1;}}_LL1D9: {struct Cyc_List_List*_tmp3A1=0;for(0;_tmp370 != 0;
_tmp370=_tmp370->tl){struct Cyc_List_List*_tmp117D;_tmp3A1=((_tmp117D=_cycalloc(
sizeof(*_tmp117D)),((_tmp117D->hd=(*((struct _tuple11*)_tmp370->hd)).f2,((
_tmp117D->tl=_tmp3A1,_tmp117D))))));}_tmp373=_tmp3A1;goto _LL1DB;}_LL1DA: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp371=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp355;if(_tmp371->tag != 4)goto _LL1DC;else{_tmp372=_tmp371->f1;_tmp373=_tmp372.targs;}}
_LL1DB: _tmp376=_tmp373;goto _LL1DD;_LL1DC: {struct Cyc_Absyn_AggrType_struct*
_tmp374=(struct Cyc_Absyn_AggrType_struct*)_tmp355;if(_tmp374->tag != 12)goto
_LL1DE;else{_tmp375=_tmp374->f1;_tmp376=_tmp375.targs;}}_LL1DD: {struct Cyc_Absyn_JoinEff_struct
_tmp1180;struct Cyc_Absyn_JoinEff_struct*_tmp117F;return Cyc_Tcutil_normalize_effect((
void*)((_tmp117F=_cycalloc(sizeof(*_tmp117F)),((_tmp117F[0]=((_tmp1180.tag=24,((
_tmp1180.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp376),_tmp1180)))),_tmp117F)))));}_LL1DE: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp377=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp355;if(_tmp377->tag != 13)goto _LL1E0;else{_tmp378=_tmp377->f2;}}_LL1DF: {
struct Cyc_Absyn_JoinEff_struct _tmp1183;struct Cyc_Absyn_JoinEff_struct*_tmp1182;
return Cyc_Tcutil_normalize_effect((void*)((_tmp1182=_cycalloc(sizeof(*_tmp1182)),((
_tmp1182[0]=((_tmp1183.tag=24,((_tmp1183.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp378),_tmp1183)))),_tmp1182)))));}_LL1E0: {struct Cyc_Absyn_TagType_struct*
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
_tmp1186;struct Cyc_Absyn_JoinEff_struct*_tmp1185;return Cyc_Tcutil_normalize_effect((
void*)((_tmp1185=_cycalloc(sizeof(*_tmp1185)),((_tmp1185[0]=((_tmp1186.tag=24,((
_tmp1186.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp389),_tmp1186)))),_tmp1185)))));}_LL1F0: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp38A=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp355;if(_tmp38A->tag != 26)goto _LL1BB;}_LL1F1: {const char*_tmp1189;void*
_tmp1188;(_tmp1188=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1189="typedecl in rgns_of",_tag_dyneither(_tmp1189,
sizeof(char),20))),_tag_dyneither(_tmp1188,sizeof(void*),0)));}_LL1BB:;}void*Cyc_Tcutil_normalize_effect(
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
_tmp118A;effects=((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((_tmp118A->hd=_tmp3B8,((
_tmp118A->tl=effects,_tmp118A))))));}goto _LL209;_LL209:;}}*_tmp3AF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3B0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3AC;if(_tmp3B0->tag != 25)goto _LL200;
else{_tmp3B1=(void*)_tmp3B0->f1;}}_LL1FF: {void*_tmp3BF=Cyc_Tcutil_compress(
_tmp3B1);_LL211: {struct Cyc_Absyn_Evar_struct*_tmp3C0=(struct Cyc_Absyn_Evar_struct*)
_tmp3BF;if(_tmp3C0->tag != 1)goto _LL213;}_LL212: goto _LL214;_LL213: {struct Cyc_Absyn_VarType_struct*
_tmp3C1=(struct Cyc_Absyn_VarType_struct*)_tmp3BF;if(_tmp3C1->tag != 2)goto _LL215;}
_LL214: return e;_LL215:;_LL216: return Cyc_Tcutil_rgns_of(_tmp3B1);_LL210:;}_LL200:;
_LL201: return e;_LL1FB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp1194;struct Cyc_Core_Opt*_tmp1193;struct Cyc_Absyn_FnInfo _tmp1192;struct Cyc_Absyn_FnType_struct*
_tmp1191;struct Cyc_Absyn_FnType_struct*_tmp3C2=(_tmp1191=_cycalloc(sizeof(*
_tmp1191)),((_tmp1191[0]=((_tmp1194.tag=10,((_tmp1194.f1=((_tmp1192.tvars=0,((
_tmp1192.effect=((_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->v=eff,
_tmp1193)))),((_tmp1192.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp1192.args=0,((
_tmp1192.c_varargs=0,((_tmp1192.cyc_varargs=0,((_tmp1192.rgn_po=0,((_tmp1192.attributes=
0,_tmp1192)))))))))))))))),_tmp1194)))),_tmp1191)));return Cyc_Absyn_atb_typ((
void*)_tmp3C2,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)&
Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp3C7=Cyc_Tcutil_compress(e);void*
_tmp3C9;struct Cyc_List_List*_tmp3CB;void*_tmp3CD;struct Cyc_Core_Opt*_tmp3CF;
struct Cyc_Core_Opt*_tmp3D0;struct Cyc_Core_Opt**_tmp3D1;struct Cyc_Core_Opt*
_tmp3D2;_LL218: {struct Cyc_Absyn_AccessEff_struct*_tmp3C8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C7;if(_tmp3C8->tag != 23)goto _LL21A;else{_tmp3C9=(void*)_tmp3C8->f1;}}_LL219:
if(constrain)return Cyc_Tcutil_unify(r,_tmp3C9);_tmp3C9=Cyc_Tcutil_compress(
_tmp3C9);if(r == _tmp3C9)return 1;{struct _tuple0 _tmp1195;struct _tuple0 _tmp3D4=(
_tmp1195.f1=r,((_tmp1195.f2=_tmp3C9,_tmp1195)));void*_tmp3D5;struct Cyc_Absyn_Tvar*
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
_check_null(_tmp3E3))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp11A4;struct Cyc_Absyn_AccessEff_struct*
_tmp11A3;struct Cyc_Absyn_AccessEff_struct _tmp11A2;void*_tmp11A1[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11A0;void*_tmp3E5=Cyc_Tcutil_dummy_fntype((void*)((_tmp11A0=_cycalloc(
sizeof(*_tmp11A0)),((_tmp11A0[0]=((_tmp11A4.tag=24,((_tmp11A4.f1=((_tmp11A1[1]=(
void*)((_tmp11A3=_cycalloc(sizeof(*_tmp11A3)),((_tmp11A3[0]=((_tmp11A2.tag=23,((
_tmp11A2.f1=(void*)r,_tmp11A2)))),_tmp11A3)))),((_tmp11A1[0]=_tmp3E4,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11A1,sizeof(void*),2)))))),
_tmp11A4)))),_tmp11A0)))));{struct Cyc_Core_Opt*_tmp11A5;*_tmp3E2=((_tmp11A5=
_cycalloc(sizeof(*_tmp11A5)),((_tmp11A5->v=_tmp3E5,_tmp11A5))));}return 1;};}
_LL22F:;_LL230: return 0;_LL22C:;};_LL22A:;_LL22B: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3DB);_LL227:;}_LL21E: {struct Cyc_Absyn_Evar_struct*_tmp3CE=(
struct Cyc_Absyn_Evar_struct*)_tmp3C7;if(_tmp3CE->tag != 1)goto _LL220;else{_tmp3CF=
_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;_tmp3D1=(struct Cyc_Core_Opt**)& _tmp3CE->f2;
_tmp3D2=_tmp3CE->f4;}}_LL21F: if(_tmp3CF == 0  || ((struct Cyc_Absyn_Kind*)_tmp3CF->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp11A8;void*_tmp11A7;(_tmp11A7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A8="effect evar has wrong kind",
_tag_dyneither(_tmp11A8,sizeof(char),27))),_tag_dyneither(_tmp11A7,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp3EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3D2);Cyc_Tcutil_occurs(_tmp3EE,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D2))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp11BD;struct Cyc_List_List*_tmp11BC;struct Cyc_Absyn_AccessEff_struct _tmp11BB;
struct Cyc_Absyn_AccessEff_struct*_tmp11BA;struct Cyc_List_List*_tmp11B9;struct Cyc_Absyn_JoinEff_struct*
_tmp11B8;struct Cyc_Absyn_JoinEff_struct*_tmp3EF=(_tmp11B8=_cycalloc(sizeof(*
_tmp11B8)),((_tmp11B8[0]=((_tmp11BD.tag=24,((_tmp11BD.f1=((_tmp11B9=_cycalloc(
sizeof(*_tmp11B9)),((_tmp11B9->hd=_tmp3EE,((_tmp11B9->tl=((_tmp11BC=_cycalloc(
sizeof(*_tmp11BC)),((_tmp11BC->hd=(void*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((
_tmp11BA[0]=((_tmp11BB.tag=23,((_tmp11BB.f1=(void*)r,_tmp11BB)))),_tmp11BA)))),((
_tmp11BC->tl=0,_tmp11BC)))))),_tmp11B9)))))),_tmp11BD)))),_tmp11B8)));{struct Cyc_Core_Opt*
_tmp11BE;*_tmp3D1=((_tmp11BE=_cycalloc(sizeof(*_tmp11BE)),((_tmp11BE->v=(void*)
_tmp3EF,_tmp11BE))));}return 1;};};_LL220:;_LL221: return 0;_LL217:;};}static int Cyc_Tcutil_type_in_effect(
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
else{_tmp404=(void*)_tmp403->f1;}}_LL23E: {struct _tuple0 _tmp11BF;struct _tuple0
_tmp406=(_tmp11BF.f1=Cyc_Tcutil_compress(_tmp404),((_tmp11BF.f2=_tmp3FC,_tmp11BF)));
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
_tmp3FE->v)->kind != Cyc_Absyn_EffKind){const char*_tmp11C2;void*_tmp11C1;(
_tmp11C1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11C2="effect evar has wrong kind",_tag_dyneither(_tmp11C2,sizeof(char),27))),
_tag_dyneither(_tmp11C1,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp40F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp401);
Cyc_Tcutil_occurs(_tmp40F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp401))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp11D7;struct Cyc_List_List*
_tmp11D6;struct Cyc_Absyn_RgnsEff_struct _tmp11D5;struct Cyc_Absyn_RgnsEff_struct*
_tmp11D4;struct Cyc_List_List*_tmp11D3;struct Cyc_Absyn_JoinEff_struct*_tmp11D2;
struct Cyc_Absyn_JoinEff_struct*_tmp410=(_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((
_tmp11D2[0]=((_tmp11D7.tag=24,((_tmp11D7.f1=((_tmp11D3=_cycalloc(sizeof(*
_tmp11D3)),((_tmp11D3->hd=_tmp40F,((_tmp11D3->tl=((_tmp11D6=_cycalloc(sizeof(*
_tmp11D6)),((_tmp11D6->hd=(void*)((_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((
_tmp11D4[0]=((_tmp11D5.tag=25,((_tmp11D5.f1=(void*)t,_tmp11D5)))),_tmp11D4)))),((
_tmp11D6->tl=0,_tmp11D6)))))),_tmp11D3)))))),_tmp11D7)))),_tmp11D2)));{struct Cyc_Core_Opt*
_tmp11D8;*_tmp400=((_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8->v=(void*)
_tmp410,_tmp11D8))));}return 1;};};_LL23A:;_LL23B: return 0;_LL231:;};}static int Cyc_Tcutil_variable_in_effect(
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
struct Cyc_Absyn_JoinEff_struct _tmp11E7;struct Cyc_Absyn_VarType_struct*_tmp11E6;
struct Cyc_Absyn_VarType_struct _tmp11E5;void*_tmp11E4[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11E3;void*_tmp42E=Cyc_Tcutil_dummy_fntype((void*)((_tmp11E3=_cycalloc(
sizeof(*_tmp11E3)),((_tmp11E3[0]=((_tmp11E7.tag=24,((_tmp11E7.f1=((_tmp11E4[1]=(
void*)((_tmp11E6=_cycalloc(sizeof(*_tmp11E6)),((_tmp11E6[0]=((_tmp11E5.tag=2,((
_tmp11E5.f1=v,_tmp11E5)))),_tmp11E6)))),((_tmp11E4[0]=_tmp42D,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11E4,sizeof(void*),2)))))),
_tmp11E7)))),_tmp11E3)))));{struct Cyc_Core_Opt*_tmp11E8;*_tmp42B=((_tmp11E8=
_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8->v=_tmp42E,_tmp11E8))));}return 1;};}
_LL259:;_LL25A: return 0;_LL256:;};_LL254:;_LL255: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp424);_LL251:;}_LL24D: {struct Cyc_Absyn_Evar_struct*
_tmp41F=(struct Cyc_Absyn_Evar_struct*)_tmp418;if(_tmp41F->tag != 1)goto _LL24F;
else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;_tmp422=(struct Cyc_Core_Opt**)&
_tmp41F->f2;_tmp423=_tmp41F->f4;}}_LL24E: if(_tmp420 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp420->v)->kind != Cyc_Absyn_EffKind){const char*_tmp11EB;void*_tmp11EA;(
_tmp11EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp11EB="effect evar has wrong kind",_tag_dyneither(_tmp11EB,sizeof(char),27))),
_tag_dyneither(_tmp11EA,sizeof(void*),0)));}{void*_tmp437=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp423);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp423))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp1200;
struct Cyc_List_List*_tmp11FF;struct Cyc_Absyn_VarType_struct _tmp11FE;struct Cyc_Absyn_VarType_struct*
_tmp11FD;struct Cyc_List_List*_tmp11FC;struct Cyc_Absyn_JoinEff_struct*_tmp11FB;
struct Cyc_Absyn_JoinEff_struct*_tmp438=(_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((
_tmp11FB[0]=((_tmp1200.tag=24,((_tmp1200.f1=((_tmp11FC=_cycalloc(sizeof(*
_tmp11FC)),((_tmp11FC->hd=_tmp437,((_tmp11FC->tl=((_tmp11FF=_cycalloc(sizeof(*
_tmp11FF)),((_tmp11FF->hd=(void*)((_tmp11FD=_cycalloc(sizeof(*_tmp11FD)),((
_tmp11FD[0]=((_tmp11FE.tag=2,((_tmp11FE.f1=v,_tmp11FE)))),_tmp11FD)))),((
_tmp11FF->tl=0,_tmp11FF)))))),_tmp11FC)))))),_tmp1200)))),_tmp11FB)));{struct Cyc_Core_Opt*
_tmp1201;*_tmp422=((_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201->v=(void*)
_tmp438,_tmp1201))));}return 1;};};_LL24F:;_LL250: return 0;_LL246:;};}static int Cyc_Tcutil_evar_in_effect(
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
e1,e2)){struct Cyc_Core_Opt*_tmp1202;*_tmp454=((_tmp1202=_cycalloc(sizeof(*
_tmp1202)),((_tmp1202->v=Cyc_Absyn_empty_effect,_tmp1202))));}return 1;_LL274:;
_LL275: {const char*_tmp1206;void*_tmp1205[1];struct Cyc_String_pa_struct _tmp1204;(
_tmp1204.tag=0,((_tmp1204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1205[0]=& _tmp1204,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1206="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1206,sizeof(char),30))),_tag_dyneither(_tmp1205,sizeof(void*),
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
Cyc_Core_Opt*_tmp1207;*_tmp469=((_tmp1207=_cycalloc(sizeof(*_tmp1207)),((
_tmp1207->v=t2,_tmp1207))));}return;}else{{void*_tmp46D=t2;struct Cyc_Core_Opt*
_tmp46F;struct Cyc_Core_Opt**_tmp470;struct Cyc_Core_Opt*_tmp471;struct Cyc_Absyn_PtrInfo
_tmp473;_LL281: {struct Cyc_Absyn_Evar_struct*_tmp46E=(struct Cyc_Absyn_Evar_struct*)
_tmp46D;if(_tmp46E->tag != 1)goto _LL283;else{_tmp46F=_tmp46E->f2;_tmp470=(struct
Cyc_Core_Opt**)& _tmp46E->f2;_tmp471=_tmp46E->f4;}}_LL282: {struct Cyc_List_List*
_tmp474=(struct Cyc_List_List*)_tmp46A->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp471))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp474,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp1208;Cyc_Tcutil_failure_reason=((
_tmp1208="(type variable would escape scope)",_tag_dyneither(_tmp1208,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp467->v,_tmp46B)){{struct Cyc_Core_Opt*_tmp1209;*_tmp470=((
_tmp1209=_cycalloc(sizeof(*_tmp1209)),((_tmp1209->v=t1,_tmp1209))));}return;}{
const char*_tmp120A;Cyc_Tcutil_failure_reason=((_tmp120A="(kinds are incompatible)",
_tag_dyneither(_tmp120A,sizeof(char),25)));}goto _LL280;}_LL283:{struct Cyc_Absyn_PointerType_struct*
_tmp472=(struct Cyc_Absyn_PointerType_struct*)_tmp46D;if(_tmp472->tag != 5)goto
_LL285;else{_tmp473=_tmp472->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp467->v)->kind
== Cyc_Absyn_BoxKind))goto _LL285;_LL284: {union Cyc_Absyn_Constraint*_tmp478=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp473.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp479=_tmp478;union Cyc_Absyn_Constraint
_tmp47A;int _tmp47B;_LL288: _tmp47A=*_tmp479;if((_tmp47A.No_constr).tag != 3)goto
_LL28A;_tmp47B=(int)(_tmp47A.No_constr).val;_LL289:{struct
_union_Constraint_Eq_constr*_tmp120B;(_tmp120B=& _tmp478->Eq_constr,((_tmp120B->tag=
1,_tmp120B->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp120C;*_tmp469=((
_tmp120C=_cycalloc(sizeof(*_tmp120C)),((_tmp120C->v=t2,_tmp120C))));}return;
_LL28A:;_LL28B: goto _LL287;_LL287:;}goto _LL280;}_LL285:;_LL286: goto _LL280;_LL280:;}{
const char*_tmp120D;Cyc_Tcutil_failure_reason=((_tmp120D="(kinds are incompatible)",
_tag_dyneither(_tmp120D,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL27E:;_LL27F: goto _LL27B;_LL27B:;}{struct _tuple0 _tmp120E;struct _tuple0 _tmp480=(
_tmp120E.f1=t2,((_tmp120E.f2=t1,_tmp120E)));void*_tmp481;void*_tmp483;void*
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
return;{const char*_tmp120F;Cyc_Tcutil_failure_reason=((_tmp120F="(variable types are not the same)",
_tag_dyneither(_tmp120F,sizeof(char),34)));}goto _LL28C;}_LL293: _tmp48D=_tmp480.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp48E=(struct Cyc_Absyn_AggrType_struct*)
_tmp48D;if(_tmp48E->tag != 12)goto _LL295;else{_tmp48F=_tmp48E->f1;_tmp490=_tmp48F.aggr_info;
_tmp491=_tmp48F.targs;}};_tmp492=_tmp480.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp493=(struct Cyc_Absyn_AggrType_struct*)_tmp492;if(_tmp493->tag != 12)goto
_LL295;else{_tmp494=_tmp493->f1;_tmp495=_tmp494.aggr_info;_tmp496=_tmp494.targs;}};
_LL294: {enum Cyc_Absyn_AggrKind _tmp54D;struct _tuple2*_tmp54E;struct _tuple12
_tmp54C=Cyc_Absyn_aggr_kinded_name(_tmp495);_tmp54D=_tmp54C.f1;_tmp54E=_tmp54C.f2;{
enum Cyc_Absyn_AggrKind _tmp550;struct _tuple2*_tmp551;struct _tuple12 _tmp54F=Cyc_Absyn_aggr_kinded_name(
_tmp490);_tmp550=_tmp54F.f1;_tmp551=_tmp54F.f2;if(_tmp54D != _tmp550){{const char*
_tmp1210;Cyc_Tcutil_failure_reason=((_tmp1210="(struct and union type)",
_tag_dyneither(_tmp1210,sizeof(char),24)));}goto _LL28C;}if(Cyc_Absyn_qvar_cmp(
_tmp54E,_tmp551)!= 0){{const char*_tmp1211;Cyc_Tcutil_failure_reason=((_tmp1211="(different type name)",
_tag_dyneither(_tmp1211,sizeof(char),22)));}goto _LL28C;}Cyc_Tcutil_unify_list(
_tmp496,_tmp491);return;};}_LL295: _tmp497=_tmp480.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp498=(struct Cyc_Absyn_EnumType_struct*)_tmp497;if(_tmp498->tag != 14)goto
_LL297;else{_tmp499=_tmp498->f1;}};_tmp49A=_tmp480.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp49B=(struct Cyc_Absyn_EnumType_struct*)_tmp49A;if(_tmp49B->tag != 14)goto
_LL297;else{_tmp49C=_tmp49B->f1;}};_LL296: if(Cyc_Absyn_qvar_cmp(_tmp499,_tmp49C)
== 0)return;{const char*_tmp1212;Cyc_Tcutil_failure_reason=((_tmp1212="(different enum types)",
_tag_dyneither(_tmp1212,sizeof(char),23)));}goto _LL28C;_LL297: _tmp49D=_tmp480.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp49E=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp49D;if(_tmp49E->tag != 15)goto _LL299;else{_tmp49F=_tmp49E->f1;}};_tmp4A0=
_tmp480.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4A1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4A0;if(_tmp4A1->tag != 15)goto _LL299;else{_tmp4A2=_tmp4A1->f1;}};_LL298: {int
bad=0;for(0;_tmp49F != 0  && _tmp4A2 != 0;(_tmp49F=_tmp49F->tl,_tmp4A2=_tmp4A2->tl)){
struct Cyc_Absyn_Enumfield*_tmp555=(struct Cyc_Absyn_Enumfield*)_tmp49F->hd;struct
Cyc_Absyn_Enumfield*_tmp556=(struct Cyc_Absyn_Enumfield*)_tmp4A2->hd;if(Cyc_Absyn_qvar_cmp(
_tmp555->name,_tmp556->name)!= 0){{const char*_tmp1213;Cyc_Tcutil_failure_reason=((
_tmp1213="(different names for enum fields)",_tag_dyneither(_tmp1213,sizeof(char),
34)));}bad=1;break;}if(_tmp555->tag == _tmp556->tag)continue;if(_tmp555->tag == 0
 || _tmp556->tag == 0){{const char*_tmp1214;Cyc_Tcutil_failure_reason=((_tmp1214="(different tag values for enum fields)",
_tag_dyneither(_tmp1214,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp555->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp556->tag))){{const char*_tmp1215;Cyc_Tcutil_failure_reason=((_tmp1215="(different tag values for enum fields)",
_tag_dyneither(_tmp1215,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL28C;if(
_tmp49F == 0  && _tmp4A2 == 0)return;{const char*_tmp1216;Cyc_Tcutil_failure_reason=((
_tmp1216="(different number of fields for enums)",_tag_dyneither(_tmp1216,
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
_tmp1217;Cyc_Tcutil_failure_reason=((_tmp1217="(different datatype field types)",
_tag_dyneither(_tmp1217,sizeof(char),33)));}goto _LL28C;_LL29D: _tmp4C1=_tmp480.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4C2=(struct Cyc_Absyn_PointerType_struct*)
_tmp4C1;if(_tmp4C2->tag != 5)goto _LL29F;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C3.elt_typ;
_tmp4C5=_tmp4C3.elt_tq;_tmp4C6=_tmp4C3.ptr_atts;_tmp4C7=_tmp4C6.rgn;_tmp4C8=
_tmp4C6.nullable;_tmp4C9=_tmp4C6.bounds;_tmp4CA=_tmp4C6.zero_term;}};_tmp4CB=
_tmp480.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4CC=(struct Cyc_Absyn_PointerType_struct*)
_tmp4CB;if(_tmp4CC->tag != 5)goto _LL29F;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CD.elt_typ;
_tmp4CF=_tmp4CD.elt_tq;_tmp4D0=_tmp4CD.ptr_atts;_tmp4D1=_tmp4D0.rgn;_tmp4D2=
_tmp4D0.nullable;_tmp4D3=_tmp4D0.bounds;_tmp4D4=_tmp4D0.zero_term;}};_LL29E: Cyc_Tcutil_unify_it(
_tmp4CE,_tmp4C4);Cyc_Tcutil_unify_it(_tmp4C7,_tmp4D1);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp1218;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4D4,_tmp4CA,((_tmp1218="(not both zero terminated)",
_tag_dyneither(_tmp1218,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4CF,
_tmp4CE,_tmp4C5,_tmp4C4);{const char*_tmp1219;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4D3,_tmp4C9,((
_tmp1219="(different pointer bounds)",_tag_dyneither(_tmp1219,sizeof(char),27))));}{
void*_tmp55E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4D3);_LL2C8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp55F=(struct Cyc_Absyn_DynEither_b_struct*)_tmp55E;if(_tmp55F->tag != 0)goto
_LL2CA;}_LL2C9: return;_LL2CA:;_LL2CB: goto _LL2C7;_LL2C7:;}{const char*_tmp121A;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4D2,_tmp4C8,((_tmp121A="(incompatible pointer types)",_tag_dyneither(
_tmp121A,sizeof(char),29))));}return;_LL29F: _tmp4D5=_tmp480.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4D6=(struct Cyc_Absyn_IntType_struct*)_tmp4D5;if(_tmp4D6->tag != 6)goto _LL2A1;
else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;}};_tmp4D9=_tmp480.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4DA=(struct Cyc_Absyn_IntType_struct*)_tmp4D9;if(_tmp4DA->tag != 6)goto _LL2A1;
else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;}};_LL2A0: if(_tmp4DB == _tmp4D7  && ((
_tmp4DC == _tmp4D8  || _tmp4DC == Cyc_Absyn_Int_sz  && _tmp4D8 == Cyc_Absyn_Long_sz)
 || _tmp4DC == Cyc_Absyn_Long_sz  && _tmp4D8 == Cyc_Absyn_Int_sz))return;{const char*
_tmp121B;Cyc_Tcutil_failure_reason=((_tmp121B="(different integral types)",
_tag_dyneither(_tmp121B,sizeof(char),27)));}goto _LL28C;_LL2A1: _tmp4DD=_tmp480.f1;{
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
Cyc_Evexp_same_const_exp(_tmp4EF,_tmp4F2)){{const char*_tmp121C;Cyc_Tcutil_failure_reason=((
_tmp121C="(cannot prove expressions are the same)",_tag_dyneither(_tmp121C,
sizeof(char),40)));}goto _LL28C;}return;_LL2A9: _tmp4F3=_tmp480.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp4F4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4F3;if(_tmp4F4->tag != 9)goto
_LL2AB;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F5.elt_type;_tmp4F7=_tmp4F5.tq;
_tmp4F8=_tmp4F5.num_elts;_tmp4F9=_tmp4F5.zero_term;}};_tmp4FA=_tmp480.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp4FB=(struct Cyc_Absyn_ArrayType_struct*)_tmp4FA;
if(_tmp4FB->tag != 9)goto _LL2AB;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FC.elt_type;
_tmp4FE=_tmp4FC.tq;_tmp4FF=_tmp4FC.num_elts;_tmp500=_tmp4FC.zero_term;}};_LL2AA:
Cyc_Tcutil_unify_it(_tmp4FD,_tmp4F6);Cyc_Tcutil_unify_tqual(_tmp4FE,_tmp4FD,
_tmp4F7,_tmp4F6);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp121D;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4F9,_tmp500,((_tmp121D="(not both zero terminated)",_tag_dyneither(_tmp121D,
sizeof(char),27))));}if(_tmp4F8 == _tmp4FF)return;if(_tmp4F8 == 0  || _tmp4FF == 0)
goto _LL28C;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4F8,(struct Cyc_Absyn_Exp*)
_tmp4FF))return;{const char*_tmp121E;Cyc_Tcutil_failure_reason=((_tmp121E="(different array sizes)",
_tag_dyneither(_tmp121E,sizeof(char),24)));}goto _LL28C;_LL2AB: _tmp501=_tmp480.f1;{
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
const char*_tmp121F;Cyc_Tcutil_failure_reason=((_tmp121F="(second function type has too few type variables)",
_tag_dyneither(_tmp121F,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp567=((struct Cyc_Absyn_Tvar*)_tmp50F->hd)->kind;void*_tmp568=((struct Cyc_Absyn_Tvar*)
_tmp504->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp567,_tmp568)){{const char*
_tmp1225;void*_tmp1224[3];struct Cyc_String_pa_struct _tmp1223;struct Cyc_String_pa_struct
_tmp1222;struct Cyc_String_pa_struct _tmp1221;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1221.tag=0,((_tmp1221.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp504->hd,& Cyc_Tcutil_bk))),((_tmp1222.tag=0,((_tmp1222.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp50F->hd,& Cyc_Tcutil_bk))),((_tmp1223.tag=0,((_tmp1223.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp50F->hd)),((_tmp1224[0]=& _tmp1223,((_tmp1224[1]=& _tmp1222,((_tmp1224[2]=&
_tmp1221,Cyc_aprintf(((_tmp1225="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1225,sizeof(char),44))),_tag_dyneither(_tmp1224,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp122F;struct Cyc_Absyn_VarType_struct _tmp122E;struct Cyc_Absyn_VarType_struct*
_tmp122D;struct Cyc_List_List*_tmp122C;inst=((_tmp122C=_region_malloc(rgn,sizeof(*
_tmp122C)),((_tmp122C->hd=((_tmp122F=_region_malloc(rgn,sizeof(*_tmp122F)),((
_tmp122F->f1=(struct Cyc_Absyn_Tvar*)_tmp504->hd,((_tmp122F->f2=(void*)((_tmp122D=
_cycalloc(sizeof(*_tmp122D)),((_tmp122D[0]=((_tmp122E.tag=2,((_tmp122E.f1=(
struct Cyc_Absyn_Tvar*)_tmp50F->hd,_tmp122E)))),_tmp122D)))),_tmp122F)))))),((
_tmp122C->tl=inst,_tmp122C))))));}_tmp50F=_tmp50F->tl;_tmp504=_tmp504->tl;};}if(
_tmp504 != 0){{const char*_tmp1230;Cyc_Tcutil_failure_reason=((_tmp1230="(second function type has too many type variables)",
_tag_dyneither(_tmp1230,sizeof(char),51)));}_npop_handler(0);goto _LL28C;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp123C;struct Cyc_Absyn_FnInfo _tmp123B;
struct Cyc_Absyn_FnType_struct*_tmp123A;struct Cyc_Absyn_FnType_struct _tmp1236;
struct Cyc_Absyn_FnInfo _tmp1235;struct Cyc_Absyn_FnType_struct*_tmp1234;Cyc_Tcutil_unify_it((
void*)((_tmp1234=_cycalloc(sizeof(*_tmp1234)),((_tmp1234[0]=((_tmp1236.tag=10,((
_tmp1236.f1=((_tmp1235.tvars=0,((_tmp1235.effect=_tmp510,((_tmp1235.ret_typ=
_tmp511,((_tmp1235.args=_tmp512,((_tmp1235.c_varargs=_tmp513,((_tmp1235.cyc_varargs=
_tmp514,((_tmp1235.rgn_po=_tmp515,((_tmp1235.attributes=_tmp516,_tmp1235)))))))))))))))),
_tmp1236)))),_tmp1234)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp123A=
_cycalloc(sizeof(*_tmp123A)),((_tmp123A[0]=((_tmp123C.tag=10,((_tmp123C.f1=((
_tmp123B.tvars=0,((_tmp123B.effect=_tmp505,((_tmp123B.ret_typ=_tmp506,((_tmp123B.args=
_tmp507,((_tmp123B.c_varargs=_tmp508,((_tmp123B.cyc_varargs=_tmp509,((_tmp123B.rgn_po=
_tmp50A,((_tmp123B.attributes=_tmp50B,_tmp123B)))))))))))))))),_tmp123C)))),
_tmp123A))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp511,_tmp506);for(0;_tmp512 != 0  && _tmp507 != 0;(_tmp512=_tmp512->tl,_tmp507=
_tmp507->tl)){struct Cyc_Absyn_Tqual _tmp57A;void*_tmp57B;struct _tuple9 _tmp579=*((
struct _tuple9*)_tmp512->hd);_tmp57A=_tmp579.f2;_tmp57B=_tmp579.f3;{struct Cyc_Absyn_Tqual
_tmp57D;void*_tmp57E;struct _tuple9 _tmp57C=*((struct _tuple9*)_tmp507->hd);_tmp57D=
_tmp57C.f2;_tmp57E=_tmp57C.f3;Cyc_Tcutil_unify_it(_tmp57B,_tmp57E);Cyc_Tcutil_unify_tqual(
_tmp57A,_tmp57B,_tmp57D,_tmp57E);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp512 != 0  || _tmp507 != 0){{const char*_tmp123D;Cyc_Tcutil_failure_reason=((
_tmp123D="(function types have different number of arguments)",_tag_dyneither(
_tmp123D,sizeof(char),52)));}_npop_handler(0);goto _LL28C;}if(_tmp513 != _tmp508){{
const char*_tmp123E;Cyc_Tcutil_failure_reason=((_tmp123E="(only one function type takes C varargs)",
_tag_dyneither(_tmp123E,sizeof(char),41)));}_npop_handler(0);goto _LL28C;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp123F;struct _tuple21 _tmp582=(_tmp123F.f1=
_tmp514,((_tmp123F.f2=_tmp509,_tmp123F)));struct Cyc_Absyn_VarargInfo*_tmp583;
struct Cyc_Absyn_VarargInfo*_tmp584;struct Cyc_Absyn_VarargInfo*_tmp585;struct Cyc_Absyn_VarargInfo*
_tmp586;struct Cyc_Absyn_VarargInfo*_tmp587;struct Cyc_Absyn_VarargInfo _tmp588;
struct Cyc_Core_Opt*_tmp589;struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58B;int _tmp58C;
struct Cyc_Absyn_VarargInfo*_tmp58D;struct Cyc_Absyn_VarargInfo _tmp58E;struct Cyc_Core_Opt*
_tmp58F;struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;int _tmp592;_LL2CD: _tmp583=
_tmp582.f1;if(_tmp583 != 0)goto _LL2CF;_tmp584=_tmp582.f2;if(_tmp584 != 0)goto
_LL2CF;_LL2CE: goto _LL2CC;_LL2CF: _tmp585=_tmp582.f1;if(_tmp585 != 0)goto _LL2D1;
_LL2D0: goto _LL2D2;_LL2D1: _tmp586=_tmp582.f2;if(_tmp586 != 0)goto _LL2D3;_LL2D2:
bad_cyc_vararg=1;{const char*_tmp1240;Cyc_Tcutil_failure_reason=((_tmp1240="(only one function type takes varargs)",
_tag_dyneither(_tmp1240,sizeof(char),39)));}goto _LL2CC;_LL2D3: _tmp587=_tmp582.f1;
if(_tmp587 == 0)goto _LL2CC;_tmp588=*_tmp587;_tmp589=_tmp588.name;_tmp58A=_tmp588.tq;
_tmp58B=_tmp588.type;_tmp58C=_tmp588.inject;_tmp58D=_tmp582.f2;if(_tmp58D == 0)
goto _LL2CC;_tmp58E=*_tmp58D;_tmp58F=_tmp58E.name;_tmp590=_tmp58E.tq;_tmp591=
_tmp58E.type;_tmp592=_tmp58E.inject;_LL2D4: Cyc_Tcutil_unify_it(_tmp58B,_tmp591);
Cyc_Tcutil_unify_tqual(_tmp58A,_tmp58B,_tmp590,_tmp591);if(_tmp58C != _tmp592){
bad_cyc_vararg=1;{const char*_tmp1241;Cyc_Tcutil_failure_reason=((_tmp1241="(only one function type injects varargs)",
_tag_dyneither(_tmp1241,sizeof(char),41)));};}goto _LL2CC;_LL2CC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL28C;}{int bad_effect=0;{struct _tuple22
_tmp1242;struct _tuple22 _tmp596=(_tmp1242.f1=_tmp510,((_tmp1242.f2=_tmp505,
_tmp1242)));struct Cyc_Core_Opt*_tmp597;struct Cyc_Core_Opt*_tmp598;struct Cyc_Core_Opt*
_tmp599;struct Cyc_Core_Opt*_tmp59A;_LL2D6: _tmp597=_tmp596.f1;if(_tmp597 != 0)goto
_LL2D8;_tmp598=_tmp596.f2;if(_tmp598 != 0)goto _LL2D8;_LL2D7: goto _LL2D5;_LL2D8:
_tmp599=_tmp596.f1;if(_tmp599 != 0)goto _LL2DA;_LL2D9: goto _LL2DB;_LL2DA: _tmp59A=
_tmp596.f2;if(_tmp59A != 0)goto _LL2DC;_LL2DB: bad_effect=1;goto _LL2D5;_LL2DC:;
_LL2DD: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp510))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp505))->v);
goto _LL2D5;_LL2D5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1243;Cyc_Tcutil_failure_reason=((_tmp1243="(function type effects do not unify)",
_tag_dyneither(_tmp1243,sizeof(char),37)));}_npop_handler(0);goto _LL28C;}if(!Cyc_Tcutil_same_atts(
_tmp50B,_tmp516)){{const char*_tmp1244;Cyc_Tcutil_failure_reason=((_tmp1244="(function types have different attributes)",
_tag_dyneither(_tmp1244,sizeof(char),43)));}_npop_handler(0);goto _LL28C;}if(!Cyc_Tcutil_same_rgn_po(
_tmp50A,_tmp515)){{const char*_tmp1245;Cyc_Tcutil_failure_reason=((_tmp1245="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1245,sizeof(char),58)));}_npop_handler(0);goto _LL28C;}
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
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1246;Cyc_Tcutil_failure_reason=((
_tmp1246="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1246,sizeof(char),51)));}goto _LL28C;_LL2AF: _tmp51D=_tmp480.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp51E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp51D;if(_tmp51E->tag != 13)goto
_LL2B1;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;}};_tmp521=_tmp480.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp522=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp521;if(_tmp522->tag != 13)goto _LL2B1;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}};
_LL2B0: if(_tmp523 != _tmp51F){{const char*_tmp1247;Cyc_Tcutil_failure_reason=((
_tmp1247="(struct and union type)",_tag_dyneither(_tmp1247,sizeof(char),24)));}
goto _LL28C;}for(0;_tmp524 != 0  && _tmp520 != 0;(_tmp524=_tmp524->tl,_tmp520=
_tmp520->tl)){struct Cyc_Absyn_Aggrfield*_tmp5A6=(struct Cyc_Absyn_Aggrfield*)
_tmp524->hd;struct Cyc_Absyn_Aggrfield*_tmp5A7=(struct Cyc_Absyn_Aggrfield*)
_tmp520->hd;if(Cyc_strptrcmp(_tmp5A6->name,_tmp5A7->name)!= 0){{const char*
_tmp1248;Cyc_Tcutil_failure_reason=((_tmp1248="(different member names)",
_tag_dyneither(_tmp1248,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5A6->type,_tmp5A7->type);Cyc_Tcutil_unify_tqual(_tmp5A6->tq,
_tmp5A6->type,_tmp5A7->tq,_tmp5A7->type);if(!Cyc_Tcutil_same_atts(_tmp5A6->attributes,
_tmp5A7->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp1249;Cyc_Tcutil_failure_reason=((_tmp1249="(different attributes on member)",
_tag_dyneither(_tmp1249,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5A6->width != 0  && _tmp5A7->width == 0  || _tmp5A7->width != 0  && _tmp5A6->width
== 0) || (_tmp5A6->width != 0  && _tmp5A7->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5A6->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5A7->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp124A;Cyc_Tcutil_failure_reason=((_tmp124A="(different bitfield widths on member)",
_tag_dyneither(_tmp124A,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp524 == 0  && _tmp520 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp124B;Cyc_Tcutil_failure_reason=((_tmp124B="(different number of members)",
_tag_dyneither(_tmp124B,sizeof(char),30)));}goto _LL28C;_LL2B1: _tmp525=_tmp480.f1;{
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
t2))return;{const char*_tmp124C;Cyc_Tcutil_failure_reason=((_tmp124C="(effects don't unify)",
_tag_dyneither(_tmp124C,sizeof(char),22)));}goto _LL28C;_LL2C5:;_LL2C6: goto _LL28C;
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
_tmp5BB.Forward_constr).val;_LL2EB: {const char*_tmp124F;void*_tmp124E;(_tmp124E=
0,Cyc_Tcutil_impos(((_tmp124F="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp124F,sizeof(char),40))),_tag_dyneither(_tmp124E,sizeof(void*),
0)));}_LL2E5:;}_LL2E3: _tmp5B4=*_tmp5AF;if((_tmp5B4.Forward_constr).tag != 2)goto
_LL2DE;_tmp5B5=(union Cyc_Absyn_Constraint*)(_tmp5B4.Forward_constr).val;_LL2E4: {
const char*_tmp1252;void*_tmp1251;(_tmp1251=0,Cyc_Tcutil_impos(((_tmp1252="unify_conref: forward after compress",
_tag_dyneither(_tmp1252,sizeof(char),37))),_tag_dyneither(_tmp1251,sizeof(void*),
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
return _tmp5EB;{struct _tuple0 _tmp1253;struct _tuple0 _tmp5ED=(_tmp1253.f1=t2,((
_tmp1253.f2=t1,_tmp1253)));void*_tmp5EE;void*_tmp5F0;void*_tmp5F2;struct Cyc_Absyn_Tvar*
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
_tmp5F1->tag != 1)goto _LL326;};_LL325: {const char*_tmp1256;void*_tmp1255;(
_tmp1255=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1256="typecmp: can only compare closed types",_tag_dyneither(_tmp1256,
sizeof(char),39))),_tag_dyneither(_tmp1255,sizeof(void*),0)));}_LL326: _tmp5F2=
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
char*_tmp1259;void*_tmp1258;(_tmp1258=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1259="missing expression in array index",
_tag_dyneither(_tmp1259,sizeof(char),34))),_tag_dyneither(_tmp1258,sizeof(void*),
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
_tmp66D.attributes;}};_LL33D: {const char*_tmp125C;void*_tmp125B;(_tmp125B=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp125C="typecmp: function types not handled",_tag_dyneither(_tmp125C,sizeof(
char),36))),_tag_dyneither(_tmp125B,sizeof(void*),0)));}_LL33E: _tmp676=_tmp5ED.f1;{
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
_tmp6A8;if(_tmp6A9->tag != 23)goto _LL356;};_LL355: {const char*_tmp125F;void*
_tmp125E;(_tmp125E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp125F="typecmp: effects not handled",_tag_dyneither(
_tmp125F,sizeof(char),29))),_tag_dyneither(_tmp125E,sizeof(void*),0)));}_LL356:;
_LL357: {const char*_tmp1262;void*_tmp1261;(_tmp1261=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1262="Unmatched case in typecmp",
_tag_dyneither(_tmp1262,sizeof(char),26))),_tag_dyneither(_tmp1261,sizeof(void*),
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
_tuple0 _tmp1263;struct _tuple0 _tmp6CE=(_tmp1263.f1=t1,((_tmp1263.f2=t2,_tmp1263)));
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
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1264;max_arith_type=((_tmp1264=
_region_malloc(_tmp730,sizeof(*_tmp1264)),((_tmp1264->v=t1,_tmp1264))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp1269;void*_tmp1268[2];struct
Cyc_String_pa_struct _tmp1267;struct Cyc_String_pa_struct _tmp1266;(_tmp1266.tag=0,((
_tmp1266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp1267.tag=0,((_tmp1267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1268[0]=& _tmp1267,((_tmp1268[1]=& _tmp1266,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp1269="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp1269,sizeof(char),41))),_tag_dyneither(_tmp1268,sizeof(void*),
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
char*_tmp126C;void*_tmp126B;(_tmp126B=0,Cyc_Tcutil_warn(e->loc,((_tmp126C="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp126C,sizeof(char),44))),_tag_dyneither(_tmp126B,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp126F;void*_tmp126E;(
_tmp126E=0,Cyc_Tcutil_warn(e->loc,((_tmp126F="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp126F,sizeof(char),44))),_tag_dyneither(_tmp126E,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1270;struct _tuple0 _tmp742=(_tmp1270.f1=
t1,((_tmp1270.f2=t2,_tmp1270)));void*_tmp743;struct Cyc_Absyn_PtrInfo _tmp745;void*
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
_tmp748.ptr_atts).bounds)){struct _tuple0 _tmp1271;struct _tuple0 _tmp75C=(_tmp1271.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp745.ptr_atts).bounds),((_tmp1271.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp748.ptr_atts).bounds),
_tmp1271)));void*_tmp75D;void*_tmp75F;void*_tmp761;void*_tmp763;void*_tmp765;
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
0,(_tmp748.ptr_atts).zero_term)){const char*_tmp1274;void*_tmp1273;(_tmp1273=0,
Cyc_Tcutil_warn(loc,((_tmp1274="implicit cast to shorter array",_tag_dyneither(
_tmp1274,sizeof(char),31))),_tag_dyneither(_tmp1273,sizeof(void*),0)));}goto
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
1,(_tmp745.ptr_atts).zero_term) && (_tmp748.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp745.elt_typ,_tmp748.elt_typ) || ((_tmp748.elt_tq).real_const
 || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp748.elt_typ)))
 && Cyc_Tcutil_ptrsubtype(te,0,_tmp745.elt_typ,_tmp748.elt_typ));return okay;}
_LL3A6: _tmp749=_tmp742.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp74A=(struct Cyc_Absyn_ArrayType_struct*)
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
void*t){void*_tmp772=Cyc_Tcutil_compress(t);_LL3B6: {struct Cyc_Absyn_PointerType_struct*
_tmp773=(struct Cyc_Absyn_PointerType_struct*)_tmp772;if(_tmp773->tag != 5)goto
_LL3B8;}_LL3B7: return 1;_LL3B8:;_LL3B9: return 0;_LL3B5:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp774=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp776;void*
_tmp777;_LL3BB: {struct Cyc_Absyn_PointerType_struct*_tmp775=(struct Cyc_Absyn_PointerType_struct*)
_tmp774;if(_tmp775->tag != 5)goto _LL3BD;else{_tmp776=_tmp775->f1;_tmp777=_tmp776.elt_typ;}}
_LL3BC: return _tmp777;_LL3BD:;_LL3BE: {const char*_tmp1277;void*_tmp1276;(_tmp1276=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1277="pointer_elt_type",_tag_dyneither(_tmp1277,sizeof(char),17))),
_tag_dyneither(_tmp1276,sizeof(void*),0)));}_LL3BA:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp77A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp77C;
struct Cyc_Absyn_PtrAtts _tmp77D;struct Cyc_Absyn_PtrAtts*_tmp77E;_LL3C0: {struct
Cyc_Absyn_PointerType_struct*_tmp77B=(struct Cyc_Absyn_PointerType_struct*)
_tmp77A;if(_tmp77B->tag != 5)goto _LL3C2;else{_tmp77C=_tmp77B->f1;_tmp77D=_tmp77C.ptr_atts;
_tmp77E=(struct Cyc_Absyn_PtrAtts*)&(_tmp77B->f1).ptr_atts;}}_LL3C1: return _tmp77E->rgn;
_LL3C2:;_LL3C3: {const char*_tmp127A;void*_tmp1279;(_tmp1279=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127A="pointer_elt_type",
_tag_dyneither(_tmp127A,sizeof(char),17))),_tag_dyneither(_tmp1279,sizeof(void*),
0)));}_LL3BF:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp781=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp783;struct Cyc_Absyn_PtrAtts _tmp784;void*_tmp785;
_LL3C5: {struct Cyc_Absyn_PointerType_struct*_tmp782=(struct Cyc_Absyn_PointerType_struct*)
_tmp781;if(_tmp782->tag != 5)goto _LL3C7;else{_tmp783=_tmp782->f1;_tmp784=_tmp783.ptr_atts;
_tmp785=_tmp784.rgn;}}_LL3C6:*rgn=_tmp785;return 1;_LL3C7:;_LL3C8: return 0;_LL3C4:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp786=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp788;struct Cyc_Absyn_PtrAtts _tmp789;union Cyc_Absyn_Constraint*
_tmp78A;_LL3CA: {struct Cyc_Absyn_PointerType_struct*_tmp787=(struct Cyc_Absyn_PointerType_struct*)
_tmp786;if(_tmp787->tag != 5)goto _LL3CC;else{_tmp788=_tmp787->f1;_tmp789=_tmp788.ptr_atts;
_tmp78A=_tmp789.bounds;}}_LL3CB:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp78A)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3CC:;_LL3CD: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3C9:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp78B=e->r;union Cyc_Absyn_Cnst
_tmp78D;struct _tuple7 _tmp78E;int _tmp78F;union Cyc_Absyn_Cnst _tmp791;struct _tuple5
_tmp792;char _tmp793;union Cyc_Absyn_Cnst _tmp795;struct _tuple6 _tmp796;short _tmp797;
union Cyc_Absyn_Cnst _tmp799;struct _tuple8 _tmp79A;long long _tmp79B;void*_tmp79D;
struct Cyc_Absyn_Exp*_tmp79E;_LL3CF: {struct Cyc_Absyn_Const_e_struct*_tmp78C=(
struct Cyc_Absyn_Const_e_struct*)_tmp78B;if(_tmp78C->tag != 0)goto _LL3D1;else{
_tmp78D=_tmp78C->f1;if((_tmp78D.Int_c).tag != 4)goto _LL3D1;_tmp78E=(struct _tuple7)(
_tmp78D.Int_c).val;_tmp78F=_tmp78E.f2;if(_tmp78F != 0)goto _LL3D1;}}_LL3D0: goto
_LL3D2;_LL3D1: {struct Cyc_Absyn_Const_e_struct*_tmp790=(struct Cyc_Absyn_Const_e_struct*)
_tmp78B;if(_tmp790->tag != 0)goto _LL3D3;else{_tmp791=_tmp790->f1;if((_tmp791.Char_c).tag
!= 2)goto _LL3D3;_tmp792=(struct _tuple5)(_tmp791.Char_c).val;_tmp793=_tmp792.f2;
if(_tmp793 != 0)goto _LL3D3;}}_LL3D2: goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Const_e_struct*
_tmp794=(struct Cyc_Absyn_Const_e_struct*)_tmp78B;if(_tmp794->tag != 0)goto _LL3D5;
else{_tmp795=_tmp794->f1;if((_tmp795.Short_c).tag != 3)goto _LL3D5;_tmp796=(struct
_tuple6)(_tmp795.Short_c).val;_tmp797=_tmp796.f2;if(_tmp797 != 0)goto _LL3D5;}}
_LL3D4: goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Const_e_struct*_tmp798=(struct Cyc_Absyn_Const_e_struct*)
_tmp78B;if(_tmp798->tag != 0)goto _LL3D7;else{_tmp799=_tmp798->f1;if((_tmp799.LongLong_c).tag
!= 5)goto _LL3D7;_tmp79A=(struct _tuple8)(_tmp799.LongLong_c).val;_tmp79B=_tmp79A.f2;
if(_tmp79B != 0)goto _LL3D7;}}_LL3D6: return 1;_LL3D7: {struct Cyc_Absyn_Cast_e_struct*
_tmp79C=(struct Cyc_Absyn_Cast_e_struct*)_tmp78B;if(_tmp79C->tag != 15)goto _LL3D9;
else{_tmp79D=(void*)_tmp79C->f1;_tmp79E=_tmp79C->f2;}}_LL3D8: return Cyc_Tcutil_is_zero(
_tmp79E) && Cyc_Tcutil_admits_zero(_tmp79D);_LL3D9:;_LL3DA: return 0;_LL3CE:;}
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
_tmp7A0;enum Cyc_Absyn_KindQual _tmp7A1;enum Cyc_Absyn_AliasQual _tmp7A2;struct Cyc_Absyn_Kind*
_tmp79F=ka;_tmp7A0=*_tmp79F;_tmp7A1=_tmp7A0.kind;_tmp7A2=_tmp7A0.aliasqual;
switch(_tmp7A2){case Cyc_Absyn_Aliasable: _LL3DB: switch(_tmp7A1){case Cyc_Absyn_AnyKind:
_LL3DD: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3DE:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3DF: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3E0: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3E1: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3E2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3DC: switch(_tmp7A1){case Cyc_Absyn_AnyKind: _LL3E5: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3E6: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3E7: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3E8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3E9: break;}break;case Cyc_Absyn_Top: _LL3E4: switch(_tmp7A1){case Cyc_Absyn_AnyKind:
_LL3EC: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3ED:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3EE: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL3EF: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3F0: break;}break;}{const char*_tmp127E;
void*_tmp127D[1];struct Cyc_String_pa_struct _tmp127C;(_tmp127C.tag=0,((_tmp127C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp127D[0]=& _tmp127C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp127E="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp127E,sizeof(char),26))),_tag_dyneither(_tmp127D,sizeof(void*),1)))))));};}
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
_tmp7A7;enum Cyc_Absyn_KindQual _tmp7A8;enum Cyc_Absyn_AliasQual _tmp7A9;struct Cyc_Absyn_Kind*
_tmp7A6=ka;_tmp7A7=*_tmp7A6;_tmp7A8=_tmp7A7.kind;_tmp7A9=_tmp7A7.aliasqual;
switch(_tmp7A9){case Cyc_Absyn_Aliasable: _LL3F2: switch(_tmp7A8){case Cyc_Absyn_AnyKind:
_LL3F4: return& ab;case Cyc_Absyn_MemKind: _LL3F5: return& mb;case Cyc_Absyn_BoxKind:
_LL3F6: return& bb;case Cyc_Absyn_RgnKind: _LL3F7: return& rb;case Cyc_Absyn_EffKind:
_LL3F8: return& eb;case Cyc_Absyn_IntKind: _LL3F9: return& ib;}case Cyc_Absyn_Unique:
_LL3F3: switch(_tmp7A8){case Cyc_Absyn_AnyKind: _LL3FC: return& uab;case Cyc_Absyn_MemKind:
_LL3FD: return& umb;case Cyc_Absyn_BoxKind: _LL3FE: return& ubb;case Cyc_Absyn_RgnKind:
_LL3FF: return& urb;default: _LL400: break;}break;case Cyc_Absyn_Top: _LL3FB: switch(
_tmp7A8){case Cyc_Absyn_AnyKind: _LL403: return& tab;case Cyc_Absyn_MemKind: _LL404:
return& tmb;case Cyc_Absyn_BoxKind: _LL405: return& tbb;case Cyc_Absyn_RgnKind: _LL406:
return& trb;default: _LL407: break;}break;}{const char*_tmp1282;void*_tmp1281[1];
struct Cyc_String_pa_struct _tmp1280;(_tmp1280.tag=0,((_tmp1280.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1281[0]=&
_tmp1280,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1282="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp1282,sizeof(char),24))),
_tag_dyneither(_tmp1281,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){struct Cyc_Core_Opt*_tmp1283;return(_tmp1283=_cycalloc(
sizeof(*_tmp1283)),((_tmp1283->v=Cyc_Tcutil_kind_to_bound(k),_tmp1283)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp1286;struct Cyc_Absyn_Const_e_struct*
_tmp1285;e1->r=(void*)((_tmp1285=_cycalloc(sizeof(*_tmp1285)),((_tmp1285[0]=((
_tmp1286.tag=0,((_tmp1286.f1=Cyc_Absyn_Null_c,_tmp1286)))),_tmp1285))));}{struct
Cyc_Core_Opt*_tmp7BE=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp1290;struct Cyc_Absyn_PtrAtts _tmp128F;struct Cyc_Absyn_PtrInfo _tmp128E;struct
Cyc_Absyn_PointerType_struct*_tmp128D;struct Cyc_Absyn_PointerType_struct*_tmp7BF=(
_tmp128D=_cycalloc(sizeof(*_tmp128D)),((_tmp128D[0]=((_tmp1290.tag=5,((_tmp1290.f1=((
_tmp128E.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7BE),((
_tmp128E.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp128E.ptr_atts=((_tmp128F.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7BE),((_tmp128F.nullable=Cyc_Absyn_true_conref,((
_tmp128F.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp128F.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp128F.ptrloc=0,_tmp128F)))))))))),_tmp128E)))))),_tmp1290)))),_tmp128D)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7BF);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp1295;void*_tmp1294[2];struct Cyc_String_pa_struct _tmp1293;struct Cyc_String_pa_struct
_tmp1292;(_tmp1292.tag=0,((_tmp1292.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp1293.tag=0,((
_tmp1293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1294[0]=& _tmp1293,((_tmp1294[1]=& _tmp1292,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1295="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp1295,sizeof(char),56))),_tag_dyneither(_tmp1294,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL409: {
const char*_tmp1296;return(_tmp1296="unknown",_tag_dyneither(_tmp1296,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL40A: {const char*_tmp1297;return(_tmp1297="no coercion",
_tag_dyneither(_tmp1297,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL40B: {
const char*_tmp1298;return(_tmp1298="null check",_tag_dyneither(_tmp1298,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL40C: {const char*_tmp1299;return(
_tmp1299="other coercion",_tag_dyneither(_tmp1299,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp12A6;const char*_tmp12A5;void*_tmp12A4[1];struct Cyc_Int_pa_struct
_tmp12A3;struct _tuple2*_tmp12A2;struct _tuple2*v=(_tmp12A2=_cycalloc(sizeof(*
_tmp12A2)),((_tmp12A2->f1=Cyc_Absyn_Loc_n,((_tmp12A2->f2=((_tmp12A6=_cycalloc(
sizeof(*_tmp12A6)),((_tmp12A6[0]=(struct _dyneither_ptr)((_tmp12A3.tag=1,((
_tmp12A3.f1=(unsigned long)counter ++,((_tmp12A4[0]=& _tmp12A3,Cyc_aprintf(((
_tmp12A5="__aliasvar%d",_tag_dyneither(_tmp12A5,sizeof(char),13))),
_tag_dyneither(_tmp12A4,sizeof(void*),1)))))))),_tmp12A6)))),_tmp12A2)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp12A9;struct Cyc_Absyn_Local_b_struct*
_tmp12A8;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12A8=_cycalloc(
sizeof(*_tmp12A8)),((_tmp12A8[0]=((_tmp12A9.tag=4,((_tmp12A9.f1=vd,_tmp12A9)))),
_tmp12A8)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7CC=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7CE;struct Cyc_Absyn_PtrAtts
_tmp7CF;void*_tmp7D0;void**_tmp7D1;_LL40F: {struct Cyc_Absyn_PointerType_struct*
_tmp7CD=(struct Cyc_Absyn_PointerType_struct*)_tmp7CC;if(_tmp7CD->tag != 5)goto
_LL411;else{_tmp7CE=_tmp7CD->f1;_tmp7CF=_tmp7CE.ptr_atts;_tmp7D0=_tmp7CF.rgn;
_tmp7D1=(void**)&((_tmp7CD->f1).ptr_atts).rgn;}}_LL410:{struct Cyc_Absyn_VarType_struct
_tmp12AC;struct Cyc_Absyn_VarType_struct*_tmp12AB;*_tmp7D1=(void*)((_tmp12AB=
_cycalloc(sizeof(*_tmp12AB)),((_tmp12AB[0]=((_tmp12AC.tag=2,((_tmp12AC.f1=tv,
_tmp12AC)))),_tmp12AB))));}goto _LL40E;_LL411:;_LL412: goto _LL40E;_LL40E:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple14 _tmp12AD;return(_tmp12AD.f1=
d,((_tmp12AD.f2=ve,_tmp12AD)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7DD=_new_region("r");struct _RegionHandle*r=& _tmp7DD;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7DE=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7E0;struct Cyc_Absyn_PtrAtts _tmp7E1;void*_tmp7E2;_LL414: {struct Cyc_Absyn_PointerType_struct*
_tmp7DF=(struct Cyc_Absyn_PointerType_struct*)_tmp7DE;if(_tmp7DF->tag != 5)goto
_LL416;else{_tmp7E0=_tmp7DF->f1;_tmp7E1=_tmp7E0.ptr_atts;_tmp7E2=_tmp7E1.rgn;}}
_LL415: {void*_tmp7E4=Cyc_Tcutil_compress(_tmp7E2);_LL41B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp7E5=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7E4;if(_tmp7E5->tag != 21)goto _LL41D;}
_LL41C: {int _tmp7E6=0;_npop_handler(0);return _tmp7E6;}_LL41D:;_LL41E: {struct Cyc_Absyn_Kind*
_tmp7E7=Cyc_Tcutil_typ_kind(_tmp7E2);int _tmp7E8=_tmp7E7->kind == Cyc_Absyn_RgnKind
 && _tmp7E7->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp7E8;}
_LL41A:;}_LL416: {struct Cyc_Absyn_TypedefType_struct*_tmp7E3=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7DE;if(_tmp7E3->tag != 18)goto _LL418;}_LL417: {const char*_tmp12B2;void*
_tmp12B1[2];struct Cyc_String_pa_struct _tmp12B0;struct Cyc_String_pa_struct
_tmp12AF;(_tmp12AF.tag=0,((_tmp12AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp12B0.tag=0,((_tmp12B0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp12B1[0]=& _tmp12B0,((_tmp12B1[1]=& _tmp12AF,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B2="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp12B2,sizeof(char),42))),_tag_dyneither(_tmp12B1,sizeof(void*),
2)))))))))))));}_LL418:;_LL419: {int _tmp7ED=0;_npop_handler(0);return _tmp7ED;}
_LL413:;}{int _tmp7EE=0;_npop_handler(0);return _tmp7EE;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp12B7;void*_tmp12B6[2];
struct Cyc_String_pa_struct _tmp12B5;struct Cyc_String_pa_struct _tmp12B4;(_tmp12B4.tag=
0,((_tmp12B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp12B5.tag=0,((_tmp12B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp12B6[0]=& _tmp12B5,((_tmp12B6[1]=& _tmp12B4,Cyc_Tcutil_warn(
e->loc,((_tmp12B7="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp12B7,sizeof(char),53))),_tag_dyneither(_tmp12B6,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp12BD;void*_tmp12BC[3];struct Cyc_String_pa_struct _tmp12BB;struct Cyc_String_pa_struct
_tmp12BA;struct Cyc_String_pa_struct _tmp12B9;(_tmp12B9.tag=0,((_tmp12B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp12BA.tag=0,((_tmp12BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp12BB.tag=0,((_tmp12BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp12BC[0]=& _tmp12BB,((_tmp12BC[1]=& _tmp12BA,((
_tmp12BC[2]=& _tmp12B9,Cyc_Tcutil_warn(e->loc,((_tmp12BD="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp12BD,sizeof(char),40))),_tag_dyneither(_tmp12BC,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp12C2;void*_tmp12C1[2];
struct Cyc_String_pa_struct _tmp12C0;struct Cyc_String_pa_struct _tmp12BF;(_tmp12BF.tag=
0,((_tmp12BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp12C0.tag=0,((_tmp12C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp12C1[0]=& _tmp12C0,((_tmp12C1[1]=& _tmp12BF,Cyc_Tcutil_warn(
e->loc,((_tmp12C2="implicit cast from %s to %s",_tag_dyneither(_tmp12C2,sizeof(
char),28))),_tag_dyneither(_tmp12C1,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp7FC=Cyc_Tcutil_compress(t);_LL420: {struct Cyc_Absyn_IntType_struct*
_tmp7FD=(struct Cyc_Absyn_IntType_struct*)_tmp7FC;if(_tmp7FD->tag != 6)goto _LL422;}
_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_FloatType_struct*_tmp7FE=(struct Cyc_Absyn_FloatType_struct*)
_tmp7FC;if(_tmp7FE->tag != 7)goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_DoubleType_struct*
_tmp7FF=(struct Cyc_Absyn_DoubleType_struct*)_tmp7FC;if(_tmp7FF->tag != 8)goto
_LL426;}_LL425: return 1;_LL426:;_LL427: return 0;_LL41F:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple16*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp801;
struct _RegionHandle*_tmp802;struct _tuple16 _tmp800=*env;_tmp801=_tmp800.f1;
_tmp802=_tmp800.f2;{struct _tuple11*_tmp12C3;return(_tmp12C3=_region_malloc(
_tmp802,sizeof(*_tmp12C3)),((_tmp12C3->f1=x->tq,((_tmp12C3->f2=Cyc_Tcutil_rsubstitute(
_tmp802,_tmp801,x->type),_tmp12C3)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp805;void*
_tmp806;struct _tuple11 _tmp804=*x;_tmp805=_tmp804.f1;_tmp806=_tmp804.f2;{struct
_tuple11*_tmp12C4;return(_tmp12C4=_region_malloc(r,sizeof(*_tmp12C4)),((_tmp12C4->f1=
_tmp805,((_tmp12C4->f2=_tmp806,_tmp12C4)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{
void*_tmp808=t1;struct Cyc_List_List*_tmp80B;struct Cyc_Absyn_AggrInfo _tmp80D;
union Cyc_Absyn_AggrInfoU _tmp80E;struct Cyc_Absyn_Aggrdecl**_tmp80F;struct Cyc_Absyn_Aggrdecl*
_tmp810;struct Cyc_List_List*_tmp811;enum Cyc_Absyn_AggrKind _tmp813;struct Cyc_List_List*
_tmp814;_LL429: {struct Cyc_Absyn_VoidType_struct*_tmp809=(struct Cyc_Absyn_VoidType_struct*)
_tmp808;if(_tmp809->tag != 0)goto _LL42B;}_LL42A: return 0;_LL42B: {struct Cyc_Absyn_TupleType_struct*
_tmp80A=(struct Cyc_Absyn_TupleType_struct*)_tmp808;if(_tmp80A->tag != 11)goto
_LL42D;else{_tmp80B=_tmp80A->f1;}}_LL42C: return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _RegionHandle*,struct _tuple11*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,
r,_tmp80B);_LL42D: {struct Cyc_Absyn_AggrType_struct*_tmp80C=(struct Cyc_Absyn_AggrType_struct*)
_tmp808;if(_tmp80C->tag != 12)goto _LL42F;else{_tmp80D=_tmp80C->f1;_tmp80E=_tmp80D.aggr_info;
if((_tmp80E.KnownAggr).tag != 2)goto _LL42F;_tmp80F=(struct Cyc_Absyn_Aggrdecl**)(
_tmp80E.KnownAggr).val;_tmp810=*_tmp80F;_tmp811=_tmp80D.targs;}}_LL42E: if(((
_tmp810->kind == Cyc_Absyn_UnionA  || _tmp810->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp810->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp810->impl))->rgn_po != 0){struct _tuple11*_tmp12C7;struct Cyc_List_List*
_tmp12C6;return(_tmp12C6=_region_malloc(r,sizeof(*_tmp12C6)),((_tmp12C6->hd=((
_tmp12C7=_region_malloc(r,sizeof(*_tmp12C7)),((_tmp12C7->f1=Cyc_Absyn_const_tqual(
0),((_tmp12C7->f2=t1,_tmp12C7)))))),((_tmp12C6->tl=0,_tmp12C6)))));}{struct Cyc_List_List*
_tmp817=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp810->tvs,
_tmp811);struct _tuple16 _tmp12C8;struct _tuple16 env=(_tmp12C8.f1=_tmp817,((
_tmp12C8.f2=r,_tmp12C8)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple11*(*f)(struct _tuple16*,struct Cyc_Absyn_Aggrfield*),struct _tuple16*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp810->impl))->fields);};_LL42F: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp812=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp808;if(_tmp812->tag != 13)goto _LL431;else{_tmp813=_tmp812->f1;if(_tmp813 != 
Cyc_Absyn_StructA)goto _LL431;_tmp814=_tmp812->f2;}}_LL430: {struct _tuple16
_tmp12C9;struct _tuple16 env=(_tmp12C9.f1=0,((_tmp12C9.f2=r,_tmp12C9)));return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple16*,
struct Cyc_Absyn_Aggrfield*),struct _tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r,Cyc_Tcutil_flatten_typ_f,& env,_tmp814);}_LL431:;_LL432: {struct _tuple11*
_tmp12CC;struct Cyc_List_List*_tmp12CB;return(_tmp12CB=_region_malloc(r,sizeof(*
_tmp12CB)),((_tmp12CB->hd=((_tmp12CC=_region_malloc(r,sizeof(*_tmp12CC)),((
_tmp12CC->f1=Cyc_Absyn_const_tqual(0),((_tmp12CC->f2=t1,_tmp12CC)))))),((
_tmp12CB->tl=0,_tmp12CB)))));}_LL428:;};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp81C=(void*)t->hd;_LL434: {struct Cyc_Absyn_Pure_att_struct*
_tmp81D=(struct Cyc_Absyn_Pure_att_struct*)_tmp81C;if(_tmp81D->tag != 22)goto
_LL436;}_LL435: goto _LL437;_LL436: {struct Cyc_Absyn_Noreturn_att_struct*_tmp81E=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp81C;if(_tmp81E->tag != 4)goto _LL438;}
_LL437: goto _LL439;_LL438: {struct Cyc_Absyn_Initializes_att_struct*_tmp81F=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp81C;if(_tmp81F->tag != 20)goto _LL43A;}
_LL439: continue;_LL43A:;_LL43B: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL433:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{
struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((
struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp12CD;struct _tuple0 _tmp821=(_tmp12CD.f1=t1,((_tmp12CD.f2=t2,_tmp12CD)));void*
_tmp822;struct Cyc_Absyn_PtrInfo _tmp824;void*_tmp825;struct Cyc_Absyn_Tqual _tmp826;
struct Cyc_Absyn_PtrAtts _tmp827;void*_tmp828;union Cyc_Absyn_Constraint*_tmp829;
union Cyc_Absyn_Constraint*_tmp82A;union Cyc_Absyn_Constraint*_tmp82B;void*_tmp82C;
struct Cyc_Absyn_PtrInfo _tmp82E;void*_tmp82F;struct Cyc_Absyn_Tqual _tmp830;struct
Cyc_Absyn_PtrAtts _tmp831;void*_tmp832;union Cyc_Absyn_Constraint*_tmp833;union Cyc_Absyn_Constraint*
_tmp834;union Cyc_Absyn_Constraint*_tmp835;void*_tmp836;struct Cyc_Absyn_DatatypeFieldInfo
_tmp838;union Cyc_Absyn_DatatypeFieldInfoU _tmp839;struct _tuple3 _tmp83A;struct Cyc_Absyn_Datatypedecl*
_tmp83B;struct Cyc_Absyn_Datatypefield*_tmp83C;struct Cyc_List_List*_tmp83D;void*
_tmp83E;struct Cyc_Absyn_DatatypeInfo _tmp840;union Cyc_Absyn_DatatypeInfoU _tmp841;
struct Cyc_Absyn_Datatypedecl**_tmp842;struct Cyc_Absyn_Datatypedecl*_tmp843;
struct Cyc_List_List*_tmp844;void*_tmp845;struct Cyc_Absyn_FnInfo _tmp847;void*
_tmp848;struct Cyc_Absyn_FnInfo _tmp84A;_LL43D: _tmp822=_tmp821.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp823=(struct Cyc_Absyn_PointerType_struct*)_tmp822;if(_tmp823->tag != 5)goto
_LL43F;else{_tmp824=_tmp823->f1;_tmp825=_tmp824.elt_typ;_tmp826=_tmp824.elt_tq;
_tmp827=_tmp824.ptr_atts;_tmp828=_tmp827.rgn;_tmp829=_tmp827.nullable;_tmp82A=
_tmp827.bounds;_tmp82B=_tmp827.zero_term;}};_tmp82C=_tmp821.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp82D=(struct Cyc_Absyn_PointerType_struct*)_tmp82C;if(_tmp82D->tag != 5)goto
_LL43F;else{_tmp82E=_tmp82D->f1;_tmp82F=_tmp82E.elt_typ;_tmp830=_tmp82E.elt_tq;
_tmp831=_tmp82E.ptr_atts;_tmp832=_tmp831.rgn;_tmp833=_tmp831.nullable;_tmp834=
_tmp831.bounds;_tmp835=_tmp831.zero_term;}};_LL43E: if(_tmp826.real_const  && !
_tmp830.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp829,
_tmp833) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp829)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp833))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp82B,
_tmp835) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp82B)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp835))return 0;if(!Cyc_Tcutil_unify(_tmp828,_tmp832) && !Cyc_Tcenv_region_outlives(
te,_tmp828,_tmp832))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp82A,_tmp834)){struct _tuple0 _tmp12CE;struct _tuple0 _tmp84C=(_tmp12CE.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp82A),((_tmp12CE.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp834),_tmp12CE)));void*
_tmp84D;void*_tmp84F;void*_tmp851;struct Cyc_Absyn_Exp*_tmp853;void*_tmp854;
struct Cyc_Absyn_Exp*_tmp856;_LL446: _tmp84D=_tmp84C.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp84E=(struct Cyc_Absyn_Upper_b_struct*)_tmp84D;if(_tmp84E->tag != 1)goto _LL448;};
_tmp84F=_tmp84C.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp850=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp84F;if(_tmp850->tag != 0)goto _LL448;};_LL447: goto _LL445;_LL448: _tmp851=
_tmp84C.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp852=(struct Cyc_Absyn_Upper_b_struct*)
_tmp851;if(_tmp852->tag != 1)goto _LL44A;else{_tmp853=_tmp852->f1;}};_tmp854=
_tmp84C.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp855=(struct Cyc_Absyn_Upper_b_struct*)
_tmp854;if(_tmp855->tag != 1)goto _LL44A;else{_tmp856=_tmp855->f1;}};_LL449: if(!
Cyc_Evexp_lte_const_exp(_tmp856,_tmp853))return 0;goto _LL445;_LL44A:;_LL44B:
return 0;_LL445:;}if(!_tmp830.real_const  && _tmp826.real_const)return 0;{struct
_tuple0*_tmp12D1;struct Cyc_List_List*_tmp12D0;return Cyc_Tcutil_ptrsubtype(te,((
_tmp12D0=_cycalloc(sizeof(*_tmp12D0)),((_tmp12D0->hd=((_tmp12D1=_cycalloc(
sizeof(*_tmp12D1)),((_tmp12D1->f1=t1,((_tmp12D1->f2=t2,_tmp12D1)))))),((_tmp12D0->tl=
assume,_tmp12D0)))))),_tmp825,_tmp82F);};_LL43F: _tmp836=_tmp821.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp837=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp836;if(_tmp837->tag != 4)
goto _LL441;else{_tmp838=_tmp837->f1;_tmp839=_tmp838.field_info;if((_tmp839.KnownDatatypefield).tag
!= 2)goto _LL441;_tmp83A=(struct _tuple3)(_tmp839.KnownDatatypefield).val;_tmp83B=
_tmp83A.f1;_tmp83C=_tmp83A.f2;_tmp83D=_tmp838.targs;}};_tmp83E=_tmp821.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp83F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp83E;if(_tmp83F->tag != 3)goto _LL441;else{_tmp840=_tmp83F->f1;_tmp841=_tmp840.datatype_info;
if((_tmp841.KnownDatatype).tag != 2)goto _LL441;_tmp842=(struct Cyc_Absyn_Datatypedecl**)(
_tmp841.KnownDatatype).val;_tmp843=*_tmp842;_tmp844=_tmp840.targs;}};_LL440: if(
_tmp83B != _tmp843  && Cyc_Absyn_qvar_cmp(_tmp83B->name,_tmp843->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83D)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp844))return 0;for(0;_tmp83D != 0;(_tmp83D=
_tmp83D->tl,_tmp844=_tmp844->tl)){if(!Cyc_Tcutil_unify((void*)_tmp83D->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp844))->hd))return 0;}return 1;_LL441: _tmp845=
_tmp821.f1;{struct Cyc_Absyn_FnType_struct*_tmp846=(struct Cyc_Absyn_FnType_struct*)
_tmp845;if(_tmp846->tag != 10)goto _LL443;else{_tmp847=_tmp846->f1;}};_tmp848=
_tmp821.f2;{struct Cyc_Absyn_FnType_struct*_tmp849=(struct Cyc_Absyn_FnType_struct*)
_tmp848;if(_tmp849->tag != 10)goto _LL443;else{_tmp84A=_tmp849->f1;}};_LL442: if(
_tmp847.tvars != 0  || _tmp84A.tvars != 0){struct Cyc_List_List*_tmp859=_tmp847.tvars;
struct Cyc_List_List*_tmp85A=_tmp84A.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp859)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp85A))return 0;{
struct _RegionHandle*_tmp85B=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp859 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp859->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp85A))->hd)->kind))return 0;{struct _tuple15*_tmp12DB;struct Cyc_Absyn_VarType_struct
_tmp12DA;struct Cyc_Absyn_VarType_struct*_tmp12D9;struct Cyc_List_List*_tmp12D8;
inst=((_tmp12D8=_region_malloc(_tmp85B,sizeof(*_tmp12D8)),((_tmp12D8->hd=((
_tmp12DB=_region_malloc(_tmp85B,sizeof(*_tmp12DB)),((_tmp12DB->f1=(struct Cyc_Absyn_Tvar*)
_tmp85A->hd,((_tmp12DB->f2=(void*)((_tmp12D9=_cycalloc(sizeof(*_tmp12D9)),((
_tmp12D9[0]=((_tmp12DA.tag=2,((_tmp12DA.f1=(struct Cyc_Absyn_Tvar*)_tmp859->hd,
_tmp12DA)))),_tmp12D9)))),_tmp12DB)))))),((_tmp12D8->tl=inst,_tmp12D8))))));}
_tmp859=_tmp859->tl;_tmp85A=_tmp85A->tl;}if(inst != 0){_tmp847.tvars=0;_tmp84A.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp12E1;struct Cyc_Absyn_FnType_struct*_tmp12E0;
struct Cyc_Absyn_FnType_struct _tmp12DE;struct Cyc_Absyn_FnType_struct*_tmp12DD;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp12DD=_cycalloc(sizeof(*_tmp12DD)),((
_tmp12DD[0]=((_tmp12DE.tag=10,((_tmp12DE.f1=_tmp847,_tmp12DE)))),_tmp12DD)))),(
void*)((_tmp12E0=_cycalloc(sizeof(*_tmp12E0)),((_tmp12E0[0]=((_tmp12E1.tag=10,((
_tmp12E1.f1=_tmp84A,_tmp12E1)))),_tmp12E0)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp847.ret_typ,_tmp84A.ret_typ))return 0;{struct Cyc_List_List*_tmp864=
_tmp847.args;struct Cyc_List_List*_tmp865=_tmp84A.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp864)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp865))return 0;for(0;_tmp864 != 0;(_tmp864=_tmp864->tl,_tmp865=_tmp865->tl)){
struct Cyc_Absyn_Tqual _tmp867;void*_tmp868;struct _tuple9 _tmp866=*((struct _tuple9*)
_tmp864->hd);_tmp867=_tmp866.f2;_tmp868=_tmp866.f3;{struct Cyc_Absyn_Tqual _tmp86A;
void*_tmp86B;struct _tuple9 _tmp869=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp865))->hd);_tmp86A=_tmp869.f2;_tmp86B=_tmp869.f3;if(_tmp86A.real_const
 && !_tmp867.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp86B,_tmp868))return
0;};}if(_tmp847.c_varargs != _tmp84A.c_varargs)return 0;if(_tmp847.cyc_varargs != 0
 && _tmp84A.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp86C=*_tmp847.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp86D=*_tmp84A.cyc_varargs;if((_tmp86D.tq).real_const
 && !(_tmp86C.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp86D.type,
_tmp86C.type))return 0;}else{if(_tmp847.cyc_varargs != 0  || _tmp84A.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp847.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp84A.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp847.rgn_po,_tmp84A.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp847.attributes,_tmp84A.attributes))return 0;return 1;};_LL443:;_LL444: return 0;
_LL43C:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple0 _tmp12E2;
struct _tuple0 _tmp86F=(_tmp12E2.f1=Cyc_Tcutil_compress(t1),((_tmp12E2.f2=Cyc_Tcutil_compress(
t2),_tmp12E2)));void*_tmp870;enum Cyc_Absyn_Size_of _tmp872;void*_tmp873;enum Cyc_Absyn_Size_of
_tmp875;_LL44D: _tmp870=_tmp86F.f1;{struct Cyc_Absyn_IntType_struct*_tmp871=(
struct Cyc_Absyn_IntType_struct*)_tmp870;if(_tmp871->tag != 6)goto _LL44F;else{
_tmp872=_tmp871->f2;}};_tmp873=_tmp86F.f2;{struct Cyc_Absyn_IntType_struct*
_tmp874=(struct Cyc_Absyn_IntType_struct*)_tmp873;if(_tmp874->tag != 6)goto _LL44F;
else{_tmp875=_tmp874->f2;}};_LL44E: return(_tmp872 == _tmp875  || _tmp872 == Cyc_Absyn_Int_sz
 && _tmp875 == Cyc_Absyn_Long_sz) || _tmp872 == Cyc_Absyn_Long_sz  && _tmp875 == Cyc_Absyn_Int_sz;
_LL44F:;_LL450: return 0;_LL44C:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp876=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp876,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp876,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp878;struct Cyc_Absyn_Tqual
_tmp879;void*_tmp87A;struct _tuple11*_tmp877=(struct _tuple11*)tqs1->hd;_tmp878=*
_tmp877;_tmp879=_tmp878.f1;_tmp87A=_tmp878.f2;{struct _tuple11 _tmp87C;struct Cyc_Absyn_Tqual
_tmp87D;void*_tmp87E;struct _tuple11*_tmp87B=(struct _tuple11*)tqs2->hd;_tmp87C=*
_tmp87B;_tmp87D=_tmp87C.f1;_tmp87E=_tmp87C.f2;if(_tmp879.real_const  && !_tmp87D.real_const)
return 0;if((_tmp87D.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp87A))) && Cyc_Tcutil_subtype(te,assume,_tmp87A,_tmp87E))continue;if(Cyc_Tcutil_unify(
_tmp87A,_tmp87E))continue;if(Cyc_Tcutil_isomorphic(_tmp87A,_tmp87E))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp87F=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp881;_LL452: {struct Cyc_Absyn_IntType_struct*_tmp880=(
struct Cyc_Absyn_IntType_struct*)_tmp87F;if(_tmp880->tag != 6)goto _LL454;else{
_tmp881=_tmp880->f2;if(_tmp881 != Cyc_Absyn_Char_sz)goto _LL454;}}_LL453: return 1;
_LL454:;_LL455: return 0;_LL451:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp882=t2;enum Cyc_Absyn_Size_of _tmp884;enum Cyc_Absyn_Size_of _tmp886;_LL457: {
struct Cyc_Absyn_IntType_struct*_tmp883=(struct Cyc_Absyn_IntType_struct*)_tmp882;
if(_tmp883->tag != 6)goto _LL459;else{_tmp884=_tmp883->f2;if(_tmp884 != Cyc_Absyn_Int_sz)
goto _LL459;}}_LL458: goto _LL45A;_LL459: {struct Cyc_Absyn_IntType_struct*_tmp885=(
struct Cyc_Absyn_IntType_struct*)_tmp882;if(_tmp885->tag != 6)goto _LL45B;else{
_tmp886=_tmp885->f2;if(_tmp886 != Cyc_Absyn_Long_sz)goto _LL45B;}}_LL45A: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL456;_LL45B:;
_LL45C: goto _LL456;_LL456:;}{void*_tmp887=t1;struct Cyc_Absyn_PtrInfo _tmp889;void*
_tmp88A;struct Cyc_Absyn_Tqual _tmp88B;struct Cyc_Absyn_PtrAtts _tmp88C;void*_tmp88D;
union Cyc_Absyn_Constraint*_tmp88E;union Cyc_Absyn_Constraint*_tmp88F;union Cyc_Absyn_Constraint*
_tmp890;struct Cyc_Absyn_ArrayInfo _tmp892;void*_tmp893;struct Cyc_Absyn_Tqual
_tmp894;struct Cyc_Absyn_Exp*_tmp895;union Cyc_Absyn_Constraint*_tmp896;struct Cyc_Absyn_Enumdecl*
_tmp898;void*_tmp89D;_LL45E: {struct Cyc_Absyn_PointerType_struct*_tmp888=(struct
Cyc_Absyn_PointerType_struct*)_tmp887;if(_tmp888->tag != 5)goto _LL460;else{
_tmp889=_tmp888->f1;_tmp88A=_tmp889.elt_typ;_tmp88B=_tmp889.elt_tq;_tmp88C=
_tmp889.ptr_atts;_tmp88D=_tmp88C.rgn;_tmp88E=_tmp88C.nullable;_tmp88F=_tmp88C.bounds;
_tmp890=_tmp88C.zero_term;}}_LL45F:{void*_tmp89E=t2;struct Cyc_Absyn_PtrInfo
_tmp8A0;void*_tmp8A1;struct Cyc_Absyn_Tqual _tmp8A2;struct Cyc_Absyn_PtrAtts _tmp8A3;
void*_tmp8A4;union Cyc_Absyn_Constraint*_tmp8A5;union Cyc_Absyn_Constraint*_tmp8A6;
union Cyc_Absyn_Constraint*_tmp8A7;_LL46F: {struct Cyc_Absyn_PointerType_struct*
_tmp89F=(struct Cyc_Absyn_PointerType_struct*)_tmp89E;if(_tmp89F->tag != 5)goto
_LL471;else{_tmp8A0=_tmp89F->f1;_tmp8A1=_tmp8A0.elt_typ;_tmp8A2=_tmp8A0.elt_tq;
_tmp8A3=_tmp8A0.ptr_atts;_tmp8A4=_tmp8A3.rgn;_tmp8A5=_tmp8A3.nullable;_tmp8A6=
_tmp8A3.bounds;_tmp8A7=_tmp8A3.zero_term;}}_LL470: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp12E5;struct Cyc_List_List*
_tmp12E4;struct Cyc_List_List*_tmp8A8=(_tmp12E4=_cycalloc(sizeof(*_tmp12E4)),((
_tmp12E4->hd=((_tmp12E5=_cycalloc(sizeof(*_tmp12E5)),((_tmp12E5->f1=t1,((
_tmp12E5->f2=t2,_tmp12E5)))))),((_tmp12E4->tl=0,_tmp12E4)))));int _tmp8A9=_tmp8A2.real_const
 || !_tmp88B.real_const;int _tmp8AA=_tmp8A9  && ((((_tmp8A2.real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8A1))) && Cyc_Tcutil_ptrsubtype(te,_tmp8A8,
_tmp88A,_tmp8A1) || Cyc_Tcutil_unify(_tmp88A,_tmp8A1)) || Cyc_Tcutil_isomorphic(
_tmp88A,_tmp8A1)) || Cyc_Tcutil_unify(_tmp8A1,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp890,_tmp8A7) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8A7);int _tmp8AB=_tmp8AA?0:((Cyc_Tcutil_bits_only(_tmp88A) && Cyc_Tcutil_is_char_type(
_tmp8A1)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8A7)) && (_tmp8A2.real_const  || !_tmp88B.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp88F,_tmp8A6);if(!
bounds_ok  && !_tmp8AB){struct _tuple0 _tmp12E6;struct _tuple0 _tmp8AD=(_tmp12E6.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp88F),((_tmp12E6.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8A6),_tmp12E6)));
void*_tmp8AE;struct Cyc_Absyn_Exp*_tmp8B0;void*_tmp8B1;struct Cyc_Absyn_Exp*
_tmp8B3;_LL474: _tmp8AE=_tmp8AD.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8AF=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8AE;if(_tmp8AF->tag != 1)goto _LL476;else{
_tmp8B0=_tmp8AF->f1;}};_tmp8B1=_tmp8AD.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8B2=(struct Cyc_Absyn_Upper_b_struct*)_tmp8B1;if(_tmp8B2->tag != 1)goto _LL476;
else{_tmp8B3=_tmp8B2->f1;}};_LL475: if(Cyc_Evexp_lte_const_exp(_tmp8B3,_tmp8B0))
bounds_ok=1;goto _LL473;_LL476:;_LL477: bounds_ok=1;goto _LL473;_LL473:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp88E) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8A5))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8AA  || _tmp8AB)) && (Cyc_Tcutil_unify(
_tmp88D,_tmp8A4) || Cyc_Tcenv_region_outlives(te,_tmp88D,_tmp8A4)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL471:;_LL472: goto _LL46E;
_LL46E:;}return Cyc_Absyn_Unknown_coercion;_LL460: {struct Cyc_Absyn_ArrayType_struct*
_tmp891=(struct Cyc_Absyn_ArrayType_struct*)_tmp887;if(_tmp891->tag != 9)goto
_LL462;else{_tmp892=_tmp891->f1;_tmp893=_tmp892.elt_type;_tmp894=_tmp892.tq;
_tmp895=_tmp892.num_elts;_tmp896=_tmp892.zero_term;}}_LL461:{void*_tmp8B6=t2;
struct Cyc_Absyn_ArrayInfo _tmp8B8;void*_tmp8B9;struct Cyc_Absyn_Tqual _tmp8BA;
struct Cyc_Absyn_Exp*_tmp8BB;union Cyc_Absyn_Constraint*_tmp8BC;_LL479: {struct Cyc_Absyn_ArrayType_struct*
_tmp8B7=(struct Cyc_Absyn_ArrayType_struct*)_tmp8B6;if(_tmp8B7->tag != 9)goto
_LL47B;else{_tmp8B8=_tmp8B7->f1;_tmp8B9=_tmp8B8.elt_type;_tmp8BA=_tmp8B8.tq;
_tmp8BB=_tmp8B8.num_elts;_tmp8BC=_tmp8B8.zero_term;}}_LL47A: {int okay;okay=((
_tmp895 != 0  && _tmp8BB != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp896,
_tmp8BC)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8BB,(struct Cyc_Absyn_Exp*)
_tmp895);return(okay  && Cyc_Tcutil_unify(_tmp893,_tmp8B9)) && (!_tmp894.real_const
 || _tmp8BA.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL47B:;_LL47C: return Cyc_Absyn_Unknown_coercion;_LL478:;}return Cyc_Absyn_Unknown_coercion;
_LL462: {struct Cyc_Absyn_EnumType_struct*_tmp897=(struct Cyc_Absyn_EnumType_struct*)
_tmp887;if(_tmp897->tag != 14)goto _LL464;else{_tmp898=_tmp897->f2;}}_LL463:{void*
_tmp8BD=t2;struct Cyc_Absyn_Enumdecl*_tmp8BF;_LL47E: {struct Cyc_Absyn_EnumType_struct*
_tmp8BE=(struct Cyc_Absyn_EnumType_struct*)_tmp8BD;if(_tmp8BE->tag != 14)goto
_LL480;else{_tmp8BF=_tmp8BE->f2;}}_LL47F: if((_tmp898->fields != 0  && _tmp8BF->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp898->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp8BF->fields))->v))return Cyc_Absyn_No_coercion;goto _LL47D;_LL480:;_LL481: goto
_LL47D;_LL47D:;}goto _LL465;_LL464: {struct Cyc_Absyn_IntType_struct*_tmp899=(
struct Cyc_Absyn_IntType_struct*)_tmp887;if(_tmp899->tag != 6)goto _LL466;}_LL465:
goto _LL467;_LL466: {struct Cyc_Absyn_FloatType_struct*_tmp89A=(struct Cyc_Absyn_FloatType_struct*)
_tmp887;if(_tmp89A->tag != 7)goto _LL468;}_LL467: goto _LL469;_LL468: {struct Cyc_Absyn_DoubleType_struct*
_tmp89B=(struct Cyc_Absyn_DoubleType_struct*)_tmp887;if(_tmp89B->tag != 8)goto
_LL46A;}_LL469: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL46A: {struct Cyc_Absyn_RgnHandleType_struct*_tmp89C=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp887;if(_tmp89C->tag != 16)goto _LL46C;else{_tmp89D=(void*)_tmp89C->f1;}}_LL46B:{
void*_tmp8C0=t2;void*_tmp8C2;_LL483: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8C1=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp8C0;if(_tmp8C1->tag != 16)goto
_LL485;else{_tmp8C2=(void*)_tmp8C1->f1;}}_LL484: if(Cyc_Tcenv_region_outlives(te,
_tmp89D,_tmp8C2))return Cyc_Absyn_No_coercion;goto _LL482;_LL485:;_LL486: goto
_LL482;_LL482:;}return Cyc_Absyn_Unknown_coercion;_LL46C:;_LL46D: return Cyc_Absyn_Unknown_coercion;
_LL45D:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp8C3=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp12E9;struct Cyc_Absyn_Cast_e_struct*_tmp12E8;e->r=(
void*)((_tmp12E8=_cycalloc(sizeof(*_tmp12E8)),((_tmp12E8[0]=((_tmp12E9.tag=15,((
_tmp12E9.f1=(void*)t,((_tmp12E9.f2=_tmp8C3,((_tmp12E9.f3=0,((_tmp12E9.f4=c,
_tmp12E9)))))))))),_tmp12E8))));}{struct Cyc_Core_Opt*_tmp12EA;e->topt=((_tmp12EA=
_cycalloc(sizeof(*_tmp12EA)),((_tmp12EA->v=t,_tmp12EA))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmp8C7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL488: {struct Cyc_Absyn_IntType_struct*_tmp8C8=(
struct Cyc_Absyn_IntType_struct*)_tmp8C7;if(_tmp8C8->tag != 6)goto _LL48A;}_LL489:
goto _LL48B;_LL48A: {struct Cyc_Absyn_EnumType_struct*_tmp8C9=(struct Cyc_Absyn_EnumType_struct*)
_tmp8C7;if(_tmp8C9->tag != 14)goto _LL48C;}_LL48B: goto _LL48D;_LL48C: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp8CA=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp8C7;if(_tmp8CA->tag != 15)goto
_LL48E;}_LL48D: goto _LL48F;_LL48E: {struct Cyc_Absyn_TagType_struct*_tmp8CB=(
struct Cyc_Absyn_TagType_struct*)_tmp8C7;if(_tmp8CB->tag != 20)goto _LL490;}_LL48F:
return 1;_LL490: {struct Cyc_Absyn_Evar_struct*_tmp8CC=(struct Cyc_Absyn_Evar_struct*)
_tmp8C7;if(_tmp8CC->tag != 1)goto _LL492;}_LL491: return Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL492:;_LL493:
return 0;_LL487:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp8CD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL495: {struct Cyc_Absyn_FloatType_struct*_tmp8CE=(
struct Cyc_Absyn_FloatType_struct*)_tmp8CD;if(_tmp8CE->tag != 7)goto _LL497;}_LL496:
goto _LL498;_LL497: {struct Cyc_Absyn_DoubleType_struct*_tmp8CF=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8CD;if(_tmp8CF->tag != 8)goto _LL499;}_LL498: return 1;_LL499:;_LL49A: return 0;
_LL494:;};}int Cyc_Tcutil_is_function_type(void*t){void*_tmp8D0=Cyc_Tcutil_compress(
t);_LL49C: {struct Cyc_Absyn_FnType_struct*_tmp8D1=(struct Cyc_Absyn_FnType_struct*)
_tmp8D0;if(_tmp8D1->tag != 10)goto _LL49E;}_LL49D: return 1;_LL49E:;_LL49F: return 0;
_LL49B:;}void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct _tuple0
_tmp12EB;struct _tuple0 _tmp8D3=(_tmp12EB.f1=t1,((_tmp12EB.f2=t2,_tmp12EB)));void*
_tmp8D4;int _tmp8D6;void*_tmp8D7;int _tmp8D9;void*_tmp8DA;void*_tmp8DC;void*
_tmp8DE;void*_tmp8E0;void*_tmp8E2;enum Cyc_Absyn_Sign _tmp8E4;enum Cyc_Absyn_Size_of
_tmp8E5;void*_tmp8E6;enum Cyc_Absyn_Sign _tmp8E8;enum Cyc_Absyn_Size_of _tmp8E9;
void*_tmp8EA;enum Cyc_Absyn_Size_of _tmp8EC;void*_tmp8ED;enum Cyc_Absyn_Size_of
_tmp8EF;void*_tmp8F0;enum Cyc_Absyn_Sign _tmp8F2;enum Cyc_Absyn_Size_of _tmp8F3;
void*_tmp8F4;enum Cyc_Absyn_Sign _tmp8F6;enum Cyc_Absyn_Size_of _tmp8F7;void*
_tmp8F8;enum Cyc_Absyn_Sign _tmp8FA;enum Cyc_Absyn_Size_of _tmp8FB;void*_tmp8FC;
enum Cyc_Absyn_Sign _tmp8FE;enum Cyc_Absyn_Size_of _tmp8FF;void*_tmp900;enum Cyc_Absyn_Size_of
_tmp902;void*_tmp903;enum Cyc_Absyn_Size_of _tmp905;_LL4A1: _tmp8D4=_tmp8D3.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp8D5=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8D4;if(_tmp8D5->tag != 8)goto _LL4A3;else{_tmp8D6=_tmp8D5->f1;}};_tmp8D7=
_tmp8D3.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp8D8=(struct Cyc_Absyn_DoubleType_struct*)
_tmp8D7;if(_tmp8D8->tag != 8)goto _LL4A3;else{_tmp8D9=_tmp8D8->f1;}};_LL4A2: if(
_tmp8D6)return t1;else{return t2;}_LL4A3: _tmp8DA=_tmp8D3.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp8DB=(struct Cyc_Absyn_DoubleType_struct*)_tmp8DA;if(_tmp8DB->tag != 8)goto
_LL4A5;};_LL4A4: return t1;_LL4A5: _tmp8DC=_tmp8D3.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp8DD=(struct Cyc_Absyn_DoubleType_struct*)_tmp8DC;if(_tmp8DD->tag != 8)goto
_LL4A7;};_LL4A6: return t2;_LL4A7: _tmp8DE=_tmp8D3.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp8DF=(struct Cyc_Absyn_FloatType_struct*)_tmp8DE;if(_tmp8DF->tag != 7)goto
_LL4A9;};_LL4A8: goto _LL4AA;_LL4A9: _tmp8E0=_tmp8D3.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp8E1=(struct Cyc_Absyn_FloatType_struct*)_tmp8E0;if(_tmp8E1->tag != 7)goto
_LL4AB;};_LL4AA: return(void*)& Cyc_Absyn_FloatType_val;_LL4AB: _tmp8E2=_tmp8D3.f1;{
struct Cyc_Absyn_IntType_struct*_tmp8E3=(struct Cyc_Absyn_IntType_struct*)_tmp8E2;
if(_tmp8E3->tag != 6)goto _LL4AD;else{_tmp8E4=_tmp8E3->f1;if(_tmp8E4 != Cyc_Absyn_Unsigned)
goto _LL4AD;_tmp8E5=_tmp8E3->f2;if(_tmp8E5 != Cyc_Absyn_LongLong_sz)goto _LL4AD;}};
_LL4AC: goto _LL4AE;_LL4AD: _tmp8E6=_tmp8D3.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8E7=(struct Cyc_Absyn_IntType_struct*)_tmp8E6;if(_tmp8E7->tag != 6)goto _LL4AF;
else{_tmp8E8=_tmp8E7->f1;if(_tmp8E8 != Cyc_Absyn_Unsigned)goto _LL4AF;_tmp8E9=
_tmp8E7->f2;if(_tmp8E9 != Cyc_Absyn_LongLong_sz)goto _LL4AF;}};_LL4AE: return Cyc_Absyn_ulonglong_typ;
_LL4AF: _tmp8EA=_tmp8D3.f1;{struct Cyc_Absyn_IntType_struct*_tmp8EB=(struct Cyc_Absyn_IntType_struct*)
_tmp8EA;if(_tmp8EB->tag != 6)goto _LL4B1;else{_tmp8EC=_tmp8EB->f2;if(_tmp8EC != Cyc_Absyn_LongLong_sz)
goto _LL4B1;}};_LL4B0: goto _LL4B2;_LL4B1: _tmp8ED=_tmp8D3.f2;{struct Cyc_Absyn_IntType_struct*
_tmp8EE=(struct Cyc_Absyn_IntType_struct*)_tmp8ED;if(_tmp8EE->tag != 6)goto _LL4B3;
else{_tmp8EF=_tmp8EE->f2;if(_tmp8EF != Cyc_Absyn_LongLong_sz)goto _LL4B3;}};_LL4B2:
return Cyc_Absyn_slonglong_typ;_LL4B3: _tmp8F0=_tmp8D3.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8F1=(struct Cyc_Absyn_IntType_struct*)_tmp8F0;if(_tmp8F1->tag != 6)goto _LL4B5;
else{_tmp8F2=_tmp8F1->f1;if(_tmp8F2 != Cyc_Absyn_Unsigned)goto _LL4B5;_tmp8F3=
_tmp8F1->f2;if(_tmp8F3 != Cyc_Absyn_Long_sz)goto _LL4B5;}};_LL4B4: goto _LL4B6;
_LL4B5: _tmp8F4=_tmp8D3.f2;{struct Cyc_Absyn_IntType_struct*_tmp8F5=(struct Cyc_Absyn_IntType_struct*)
_tmp8F4;if(_tmp8F5->tag != 6)goto _LL4B7;else{_tmp8F6=_tmp8F5->f1;if(_tmp8F6 != Cyc_Absyn_Unsigned)
goto _LL4B7;_tmp8F7=_tmp8F5->f2;if(_tmp8F7 != Cyc_Absyn_Long_sz)goto _LL4B7;}};
_LL4B6: return Cyc_Absyn_ulong_typ;_LL4B7: _tmp8F8=_tmp8D3.f1;{struct Cyc_Absyn_IntType_struct*
_tmp8F9=(struct Cyc_Absyn_IntType_struct*)_tmp8F8;if(_tmp8F9->tag != 6)goto _LL4B9;
else{_tmp8FA=_tmp8F9->f1;if(_tmp8FA != Cyc_Absyn_Unsigned)goto _LL4B9;_tmp8FB=
_tmp8F9->f2;if(_tmp8FB != Cyc_Absyn_Int_sz)goto _LL4B9;}};_LL4B8: goto _LL4BA;_LL4B9:
_tmp8FC=_tmp8D3.f2;{struct Cyc_Absyn_IntType_struct*_tmp8FD=(struct Cyc_Absyn_IntType_struct*)
_tmp8FC;if(_tmp8FD->tag != 6)goto _LL4BB;else{_tmp8FE=_tmp8FD->f1;if(_tmp8FE != Cyc_Absyn_Unsigned)
goto _LL4BB;_tmp8FF=_tmp8FD->f2;if(_tmp8FF != Cyc_Absyn_Int_sz)goto _LL4BB;}};
_LL4BA: return Cyc_Absyn_uint_typ;_LL4BB: _tmp900=_tmp8D3.f1;{struct Cyc_Absyn_IntType_struct*
_tmp901=(struct Cyc_Absyn_IntType_struct*)_tmp900;if(_tmp901->tag != 6)goto _LL4BD;
else{_tmp902=_tmp901->f2;if(_tmp902 != Cyc_Absyn_Long_sz)goto _LL4BD;}};_LL4BC:
goto _LL4BE;_LL4BD: _tmp903=_tmp8D3.f2;{struct Cyc_Absyn_IntType_struct*_tmp904=(
struct Cyc_Absyn_IntType_struct*)_tmp903;if(_tmp904->tag != 6)goto _LL4BF;else{
_tmp905=_tmp904->f2;if(_tmp905 != Cyc_Absyn_Long_sz)goto _LL4BF;}};_LL4BE: return
Cyc_Absyn_slong_typ;_LL4BF:;_LL4C0: return Cyc_Absyn_sint_typ;_LL4A0:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp906=e->r;struct Cyc_Core_Opt*_tmp908;_LL4C2: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp907=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp906;if(_tmp907->tag != 4)goto _LL4C4;else{_tmp908=_tmp907->f2;if(_tmp908 != 0)
goto _LL4C4;}}_LL4C3:{const char*_tmp12EE;void*_tmp12ED;(_tmp12ED=0,Cyc_Tcutil_warn(
e->loc,((_tmp12EE="assignment in test",_tag_dyneither(_tmp12EE,sizeof(char),19))),
_tag_dyneither(_tmp12ED,sizeof(void*),0)));}goto _LL4C1;_LL4C4:;_LL4C5: goto _LL4C1;
_LL4C1:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp12EF;struct _tuple0 _tmp90C=(
_tmp12EF.f1=c1,((_tmp12EF.f2=c2,_tmp12EF)));void*_tmp90D;struct Cyc_Absyn_Kind*
_tmp90F;void*_tmp910;struct Cyc_Absyn_Kind*_tmp912;void*_tmp913;struct Cyc_Core_Opt*
_tmp915;struct Cyc_Core_Opt**_tmp916;void*_tmp917;struct Cyc_Core_Opt*_tmp919;
struct Cyc_Core_Opt**_tmp91A;void*_tmp91B;struct Cyc_Core_Opt*_tmp91D;struct Cyc_Core_Opt**
_tmp91E;struct Cyc_Absyn_Kind*_tmp91F;void*_tmp920;struct Cyc_Absyn_Kind*_tmp922;
void*_tmp923;struct Cyc_Absyn_Kind*_tmp925;void*_tmp926;struct Cyc_Core_Opt*
_tmp928;struct Cyc_Core_Opt**_tmp929;struct Cyc_Absyn_Kind*_tmp92A;void*_tmp92B;
struct Cyc_Core_Opt*_tmp92D;struct Cyc_Core_Opt**_tmp92E;struct Cyc_Absyn_Kind*
_tmp92F;void*_tmp930;struct Cyc_Core_Opt*_tmp932;struct Cyc_Core_Opt**_tmp933;
struct Cyc_Absyn_Kind*_tmp934;_LL4C7: _tmp90D=_tmp90C.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp90E=(struct Cyc_Absyn_Eq_kb_struct*)_tmp90D;if(_tmp90E->tag != 0)goto _LL4C9;
else{_tmp90F=_tmp90E->f1;}};_tmp910=_tmp90C.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp911=(struct Cyc_Absyn_Eq_kb_struct*)_tmp910;if(_tmp911->tag != 0)goto _LL4C9;
else{_tmp912=_tmp911->f1;}};_LL4C8: return _tmp90F == _tmp912;_LL4C9: _tmp913=
_tmp90C.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp914=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp913;if(_tmp914->tag != 1)goto _LL4CB;else{_tmp915=_tmp914->f1;_tmp916=(struct
Cyc_Core_Opt**)& _tmp914->f1;}};_LL4CA:{struct Cyc_Core_Opt*_tmp12F0;*_tmp916=((
_tmp12F0=_cycalloc(sizeof(*_tmp12F0)),((_tmp12F0->v=c1,_tmp12F0))));}return 1;
_LL4CB: _tmp917=_tmp90C.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp918=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp917;if(_tmp918->tag != 1)goto _LL4CD;else{_tmp919=_tmp918->f1;_tmp91A=(struct
Cyc_Core_Opt**)& _tmp918->f1;}};_LL4CC:{struct Cyc_Core_Opt*_tmp12F1;*_tmp91A=((
_tmp12F1=_cycalloc(sizeof(*_tmp12F1)),((_tmp12F1->v=c2,_tmp12F1))));}return 1;
_LL4CD: _tmp91B=_tmp90C.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp91C=(struct Cyc_Absyn_Less_kb_struct*)
_tmp91B;if(_tmp91C->tag != 2)goto _LL4CF;else{_tmp91D=_tmp91C->f1;_tmp91E=(struct
Cyc_Core_Opt**)& _tmp91C->f1;_tmp91F=_tmp91C->f2;}};_tmp920=_tmp90C.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp921=(struct Cyc_Absyn_Eq_kb_struct*)_tmp920;if(_tmp921->tag != 0)goto _LL4CF;
else{_tmp922=_tmp921->f1;}};_LL4CE: if(Cyc_Tcutil_kind_leq(_tmp922,_tmp91F)){{
struct Cyc_Core_Opt*_tmp12F2;*_tmp91E=((_tmp12F2=_cycalloc(sizeof(*_tmp12F2)),((
_tmp12F2->v=c2,_tmp12F2))));}return 1;}else{return 0;}_LL4CF: _tmp923=_tmp90C.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp924=(struct Cyc_Absyn_Eq_kb_struct*)_tmp923;if(
_tmp924->tag != 0)goto _LL4D1;else{_tmp925=_tmp924->f1;}};_tmp926=_tmp90C.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp927=(struct Cyc_Absyn_Less_kb_struct*)_tmp926;
if(_tmp927->tag != 2)goto _LL4D1;else{_tmp928=_tmp927->f1;_tmp929=(struct Cyc_Core_Opt**)&
_tmp927->f1;_tmp92A=_tmp927->f2;}};_LL4D0: if(Cyc_Tcutil_kind_leq(_tmp925,_tmp92A)){{
struct Cyc_Core_Opt*_tmp12F3;*_tmp929=((_tmp12F3=_cycalloc(sizeof(*_tmp12F3)),((
_tmp12F3->v=c1,_tmp12F3))));}return 1;}else{return 0;}_LL4D1: _tmp92B=_tmp90C.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp92C=(struct Cyc_Absyn_Less_kb_struct*)_tmp92B;
if(_tmp92C->tag != 2)goto _LL4C6;else{_tmp92D=_tmp92C->f1;_tmp92E=(struct Cyc_Core_Opt**)&
_tmp92C->f1;_tmp92F=_tmp92C->f2;}};_tmp930=_tmp90C.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp931=(struct Cyc_Absyn_Less_kb_struct*)_tmp930;if(_tmp931->tag != 2)goto _LL4C6;
else{_tmp932=_tmp931->f1;_tmp933=(struct Cyc_Core_Opt**)& _tmp931->f1;_tmp934=
_tmp931->f2;}};_LL4D2: if(Cyc_Tcutil_kind_leq(_tmp92F,_tmp934)){{struct Cyc_Core_Opt*
_tmp12F4;*_tmp933=((_tmp12F4=_cycalloc(sizeof(*_tmp12F4)),((_tmp12F4->v=c1,
_tmp12F4))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp934,_tmp92F)){{struct Cyc_Core_Opt*
_tmp12F5;*_tmp92E=((_tmp12F5=_cycalloc(sizeof(*_tmp12F5)),((_tmp12F5->v=c2,
_tmp12F5))));}return 1;}else{return 0;}}_LL4C6:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp12F9;void*_tmp12F8[1];struct Cyc_Int_pa_struct _tmp12F7;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp12F7.tag=1,((_tmp12F7.f1=(
unsigned long)i,((_tmp12F8[0]=& _tmp12F7,Cyc_aprintf(((_tmp12F9="#%d",
_tag_dyneither(_tmp12F9,sizeof(char),4))),_tag_dyneither(_tmp12F8,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp12FC;struct Cyc_Absyn_Tvar*_tmp12FB;return(
_tmp12FB=_cycalloc(sizeof(*_tmp12FB)),((_tmp12FB->name=((_tmp12FC=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp12FC[0]=s,_tmp12FC)))),((_tmp12FB->identity=
- 1,((_tmp12FB->kind=k,_tmp12FB)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp940=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp940,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1300;void*_tmp12FF[1];struct Cyc_String_pa_struct
_tmp12FE;(_tmp12FE.tag=0,((_tmp12FE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp12FF[0]=& _tmp12FE,Cyc_printf(((_tmp1300="%s",
_tag_dyneither(_tmp1300,sizeof(char),3))),_tag_dyneither(_tmp12FF,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1301;struct
_dyneither_ptr _tmp944=Cyc_strconcat(((_tmp1301="`",_tag_dyneither(_tmp1301,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1304;char _tmp1303;
struct _dyneither_ptr _tmp1302;(_tmp1302=_dyneither_ptr_plus(_tmp944,sizeof(char),
1),((_tmp1303=*((char*)_check_dyneither_subscript(_tmp1302,sizeof(char),0)),((
_tmp1304='t',((_get_dyneither_size(_tmp1302,sizeof(char))== 1  && (_tmp1303 == '\000'
 && _tmp1304 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1302.curr)=_tmp1304)))))));}{
struct _dyneither_ptr*_tmp1305;t->name=((_tmp1305=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1305[0]=(struct _dyneither_ptr)_tmp944,_tmp1305))));};};}
struct _tuple23{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple23*x){struct Cyc_Core_Opt*
_tmp1308;struct _tuple9*_tmp1307;return(_tmp1307=_cycalloc(sizeof(*_tmp1307)),((
_tmp1307->f1=(struct Cyc_Core_Opt*)((_tmp1308=_cycalloc(sizeof(*_tmp1308)),((
_tmp1308->v=(*x).f1,_tmp1308)))),((_tmp1307->f2=(*x).f2,((_tmp1307->f3=(*x).f3,
_tmp1307)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp94C=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp1309;_tmp94C=((_tmp1309=_cycalloc(sizeof(*_tmp1309)),((
_tmp1309->hd=(void*)atts->hd,((_tmp1309->tl=_tmp94C,_tmp1309))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp130F;struct Cyc_Absyn_FnInfo _tmp130E;struct Cyc_Absyn_FnType_struct*_tmp130D;
return(void*)((_tmp130D=_cycalloc(sizeof(*_tmp130D)),((_tmp130D[0]=((_tmp130F.tag=
10,((_tmp130F.f1=((_tmp130E.tvars=fd->tvs,((_tmp130E.effect=fd->effect,((
_tmp130E.ret_typ=fd->ret_type,((_tmp130E.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp130E.c_varargs=fd->c_varargs,((_tmp130E.cyc_varargs=fd->cyc_varargs,((
_tmp130E.rgn_po=fd->rgn_po,((_tmp130E.attributes=_tmp94C,_tmp130E)))))))))))))))),
_tmp130F)))),_tmp130D))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple24{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple24*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){
return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp1310;return(_tmp1310=_cycalloc(sizeof(*_tmp1310)),((_tmp1310->f1=(*
pr).f1,((_tmp1310->f2=t,_tmp1310)))));}struct _tuple25{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};static
struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
struct _tuple25*_tmp1313;struct _tuple26*_tmp1312;return(_tmp1312=_region_malloc(
rgn,sizeof(*_tmp1312)),((_tmp1312->f1=((_tmp1313=_region_malloc(rgn,sizeof(*
_tmp1313)),((_tmp1313->f1=(*y).f1,((_tmp1313->f2=(*y).f2,_tmp1313)))))),((
_tmp1312->f2=(*y).f3,_tmp1312)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple26*w){struct _tuple25*_tmp955;void*_tmp956;struct _tuple26 _tmp954=*w;
_tmp955=_tmp954.f1;_tmp956=_tmp954.f2;{struct Cyc_Core_Opt*_tmp958;struct Cyc_Absyn_Tqual
_tmp959;struct _tuple25 _tmp957=*_tmp955;_tmp958=_tmp957.f1;_tmp959=_tmp957.f2;{
struct _tuple9*_tmp1314;return(_tmp1314=_cycalloc(sizeof(*_tmp1314)),((_tmp1314->f1=
_tmp958,((_tmp1314->f2=_tmp959,((_tmp1314->f3=_tmp956,_tmp1314)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){return f->type;}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*
f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1315;return(_tmp1315=_cycalloc(sizeof(*
_tmp1315)),((_tmp1315->name=f->name,((_tmp1315->tq=f->tq,((_tmp1315->type=t,((
_tmp1315->width=f->width,((_tmp1315->attributes=f->attributes,_tmp1315)))))))))));}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp1316;return(_tmp1316=_cycalloc(
sizeof(*_tmp1316)),((_tmp1316->topt=old->topt,((_tmp1316->r=r,((_tmp1316->loc=
old->loc,((_tmp1316->annot=old->annot,_tmp1316)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp95D=e->r;enum Cyc_Absyn_Primop _tmp963;struct Cyc_List_List*_tmp964;
struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*
_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*
_tmp96D;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*
_tmp971;void*_tmp973;struct Cyc_Absyn_Exp*_tmp974;int _tmp975;enum Cyc_Absyn_Coercion
_tmp976;void*_tmp978;struct Cyc_Absyn_Exp*_tmp97A;void*_tmp97C;void*_tmp97D;void*
_tmp97F;_LL4D4: {struct Cyc_Absyn_Const_e_struct*_tmp95E=(struct Cyc_Absyn_Const_e_struct*)
_tmp95D;if(_tmp95E->tag != 0)goto _LL4D6;}_LL4D5: goto _LL4D7;_LL4D6: {struct Cyc_Absyn_Enum_e_struct*
_tmp95F=(struct Cyc_Absyn_Enum_e_struct*)_tmp95D;if(_tmp95F->tag != 32)goto _LL4D8;}
_LL4D7: goto _LL4D9;_LL4D8: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp960=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp95D;if(_tmp960->tag != 33)goto _LL4DA;}_LL4D9: goto _LL4DB;_LL4DA: {struct Cyc_Absyn_Var_e_struct*
_tmp961=(struct Cyc_Absyn_Var_e_struct*)_tmp95D;if(_tmp961->tag != 1)goto _LL4DC;}
_LL4DB: return e;_LL4DC: {struct Cyc_Absyn_Primop_e_struct*_tmp962=(struct Cyc_Absyn_Primop_e_struct*)
_tmp95D;if(_tmp962->tag != 3)goto _LL4DE;else{_tmp963=_tmp962->f1;_tmp964=_tmp962->f2;}}
_LL4DD: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp964)== 1){struct
Cyc_Absyn_Exp*_tmp980=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp964))->hd;struct Cyc_Absyn_Exp*_tmp981=Cyc_Tcutil_rsubsexp(r,inst,_tmp980);
if(_tmp981 == _tmp980)return e;{struct Cyc_Absyn_Primop_e_struct _tmp131C;struct Cyc_Absyn_Exp*
_tmp131B[1];struct Cyc_Absyn_Primop_e_struct*_tmp131A;return Cyc_Tcutil_copye(e,(
void*)((_tmp131A=_cycalloc(sizeof(*_tmp131A)),((_tmp131A[0]=((_tmp131C.tag=3,((
_tmp131C.f1=_tmp963,((_tmp131C.f2=((_tmp131B[0]=_tmp981,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp131B,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp131C)))))),_tmp131A)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp964)== 2){struct Cyc_Absyn_Exp*_tmp985=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp964))->hd;struct Cyc_Absyn_Exp*_tmp986=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp964->tl))->hd;struct
Cyc_Absyn_Exp*_tmp987=Cyc_Tcutil_rsubsexp(r,inst,_tmp985);struct Cyc_Absyn_Exp*
_tmp988=Cyc_Tcutil_rsubsexp(r,inst,_tmp986);if(_tmp987 == _tmp985  && _tmp988 == 
_tmp986)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1322;struct Cyc_Absyn_Exp*
_tmp1321[2];struct Cyc_Absyn_Primop_e_struct*_tmp1320;return Cyc_Tcutil_copye(e,(
void*)((_tmp1320=_cycalloc(sizeof(*_tmp1320)),((_tmp1320[0]=((_tmp1322.tag=3,((
_tmp1322.f1=_tmp963,((_tmp1322.f2=((_tmp1321[1]=_tmp988,((_tmp1321[0]=_tmp987,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1321,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1322)))))),_tmp1320)))));};}
else{const char*_tmp1325;void*_tmp1324;return(_tmp1324=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1325="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1325,sizeof(char),34))),_tag_dyneither(_tmp1324,sizeof(void*),
0)));}}_LL4DE: {struct Cyc_Absyn_Conditional_e_struct*_tmp965=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp95D;if(_tmp965->tag != 6)goto _LL4E0;else{_tmp966=_tmp965->f1;_tmp967=_tmp965->f2;
_tmp968=_tmp965->f3;}}_LL4DF: {struct Cyc_Absyn_Exp*_tmp98E=Cyc_Tcutil_rsubsexp(r,
inst,_tmp966);struct Cyc_Absyn_Exp*_tmp98F=Cyc_Tcutil_rsubsexp(r,inst,_tmp967);
struct Cyc_Absyn_Exp*_tmp990=Cyc_Tcutil_rsubsexp(r,inst,_tmp968);if((_tmp98E == 
_tmp966  && _tmp98F == _tmp967) && _tmp990 == _tmp968)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1328;struct Cyc_Absyn_Conditional_e_struct*_tmp1327;return Cyc_Tcutil_copye(e,(
void*)((_tmp1327=_cycalloc(sizeof(*_tmp1327)),((_tmp1327[0]=((_tmp1328.tag=6,((
_tmp1328.f1=_tmp98E,((_tmp1328.f2=_tmp98F,((_tmp1328.f3=_tmp990,_tmp1328)))))))),
_tmp1327)))));};}_LL4E0: {struct Cyc_Absyn_And_e_struct*_tmp969=(struct Cyc_Absyn_And_e_struct*)
_tmp95D;if(_tmp969->tag != 7)goto _LL4E2;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}
_LL4E1: {struct Cyc_Absyn_Exp*_tmp993=Cyc_Tcutil_rsubsexp(r,inst,_tmp96A);struct
Cyc_Absyn_Exp*_tmp994=Cyc_Tcutil_rsubsexp(r,inst,_tmp96B);if(_tmp993 == _tmp96A
 && _tmp994 == _tmp96B)return e;{struct Cyc_Absyn_And_e_struct _tmp132B;struct Cyc_Absyn_And_e_struct*
_tmp132A;return Cyc_Tcutil_copye(e,(void*)((_tmp132A=_cycalloc(sizeof(*_tmp132A)),((
_tmp132A[0]=((_tmp132B.tag=7,((_tmp132B.f1=_tmp993,((_tmp132B.f2=_tmp994,
_tmp132B)))))),_tmp132A)))));};}_LL4E2: {struct Cyc_Absyn_Or_e_struct*_tmp96C=(
struct Cyc_Absyn_Or_e_struct*)_tmp95D;if(_tmp96C->tag != 8)goto _LL4E4;else{_tmp96D=
_tmp96C->f1;_tmp96E=_tmp96C->f2;}}_LL4E3: {struct Cyc_Absyn_Exp*_tmp997=Cyc_Tcutil_rsubsexp(
r,inst,_tmp96D);struct Cyc_Absyn_Exp*_tmp998=Cyc_Tcutil_rsubsexp(r,inst,_tmp96E);
if(_tmp997 == _tmp96D  && _tmp998 == _tmp96E)return e;{struct Cyc_Absyn_Or_e_struct
_tmp132E;struct Cyc_Absyn_Or_e_struct*_tmp132D;return Cyc_Tcutil_copye(e,(void*)((
_tmp132D=_cycalloc(sizeof(*_tmp132D)),((_tmp132D[0]=((_tmp132E.tag=8,((_tmp132E.f1=
_tmp997,((_tmp132E.f2=_tmp998,_tmp132E)))))),_tmp132D)))));};}_LL4E4: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp96F=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp95D;if(_tmp96F->tag != 9)goto _LL4E6;
else{_tmp970=_tmp96F->f1;_tmp971=_tmp96F->f2;}}_LL4E5: {struct Cyc_Absyn_Exp*
_tmp99B=Cyc_Tcutil_rsubsexp(r,inst,_tmp970);struct Cyc_Absyn_Exp*_tmp99C=Cyc_Tcutil_rsubsexp(
r,inst,_tmp971);if(_tmp99B == _tmp970  && _tmp99C == _tmp971)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp1331;struct Cyc_Absyn_SeqExp_e_struct*_tmp1330;return Cyc_Tcutil_copye(e,(void*)((
_tmp1330=_cycalloc(sizeof(*_tmp1330)),((_tmp1330[0]=((_tmp1331.tag=9,((_tmp1331.f1=
_tmp99B,((_tmp1331.f2=_tmp99C,_tmp1331)))))),_tmp1330)))));};}_LL4E6: {struct Cyc_Absyn_Cast_e_struct*
_tmp972=(struct Cyc_Absyn_Cast_e_struct*)_tmp95D;if(_tmp972->tag != 15)goto _LL4E8;
else{_tmp973=(void*)_tmp972->f1;_tmp974=_tmp972->f2;_tmp975=_tmp972->f3;_tmp976=
_tmp972->f4;}}_LL4E7: {struct Cyc_Absyn_Exp*_tmp99F=Cyc_Tcutil_rsubsexp(r,inst,
_tmp974);void*_tmp9A0=Cyc_Tcutil_rsubstitute(r,inst,_tmp973);if(_tmp99F == 
_tmp974  && _tmp9A0 == _tmp973)return e;{struct Cyc_Absyn_Cast_e_struct _tmp1334;
struct Cyc_Absyn_Cast_e_struct*_tmp1333;return Cyc_Tcutil_copye(e,(void*)((
_tmp1333=_cycalloc(sizeof(*_tmp1333)),((_tmp1333[0]=((_tmp1334.tag=15,((_tmp1334.f1=(
void*)_tmp9A0,((_tmp1334.f2=_tmp99F,((_tmp1334.f3=_tmp975,((_tmp1334.f4=_tmp976,
_tmp1334)))))))))),_tmp1333)))));};}_LL4E8: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp977=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp95D;if(_tmp977->tag != 18)goto
_LL4EA;else{_tmp978=(void*)_tmp977->f1;}}_LL4E9: {void*_tmp9A3=Cyc_Tcutil_rsubstitute(
r,inst,_tmp978);if(_tmp9A3 == _tmp978)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp1337;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1336;return Cyc_Tcutil_copye(e,(
void*)((_tmp1336=_cycalloc(sizeof(*_tmp1336)),((_tmp1336[0]=((_tmp1337.tag=18,((
_tmp1337.f1=(void*)_tmp9A3,_tmp1337)))),_tmp1336)))));};}_LL4EA: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp979=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp95D;if(_tmp979->tag != 19)goto
_LL4EC;else{_tmp97A=_tmp979->f1;}}_LL4EB: {struct Cyc_Absyn_Exp*_tmp9A6=Cyc_Tcutil_rsubsexp(
r,inst,_tmp97A);if(_tmp9A6 == _tmp97A)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp133A;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1339;return Cyc_Tcutil_copye(e,(
void*)((_tmp1339=_cycalloc(sizeof(*_tmp1339)),((_tmp1339[0]=((_tmp133A.tag=19,((
_tmp133A.f1=_tmp9A6,_tmp133A)))),_tmp1339)))));};}_LL4EC: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp97B=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp95D;if(_tmp97B->tag != 20)goto
_LL4EE;else{_tmp97C=(void*)_tmp97B->f1;_tmp97D=(void*)_tmp97B->f2;}}_LL4ED: {
void*_tmp9A9=Cyc_Tcutil_rsubstitute(r,inst,_tmp97C);if(_tmp9A9 == _tmp97C)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp133D;struct Cyc_Absyn_Offsetof_e_struct*
_tmp133C;return Cyc_Tcutil_copye(e,(void*)((_tmp133C=_cycalloc(sizeof(*_tmp133C)),((
_tmp133C[0]=((_tmp133D.tag=20,((_tmp133D.f1=(void*)_tmp9A9,((_tmp133D.f2=(void*)
_tmp97D,_tmp133D)))))),_tmp133C)))));};}_LL4EE: {struct Cyc_Absyn_Valueof_e_struct*
_tmp97E=(struct Cyc_Absyn_Valueof_e_struct*)_tmp95D;if(_tmp97E->tag != 39)goto
_LL4F0;else{_tmp97F=(void*)_tmp97E->f1;}}_LL4EF: {void*_tmp9AC=Cyc_Tcutil_rsubstitute(
r,inst,_tmp97F);if(_tmp9AC == _tmp97F)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp1340;struct Cyc_Absyn_Valueof_e_struct*_tmp133F;return Cyc_Tcutil_copye(e,(
void*)((_tmp133F=_cycalloc(sizeof(*_tmp133F)),((_tmp133F[0]=((_tmp1340.tag=39,((
_tmp1340.f1=(void*)_tmp9AC,_tmp1340)))),_tmp133F)))));};}_LL4F0:;_LL4F1: {const
char*_tmp1343;void*_tmp1342;return(_tmp1342=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1343="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp1343,sizeof(char),46))),_tag_dyneither(_tmp1342,sizeof(void*),
0)));}_LL4D3:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9B1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9B3;
struct Cyc_Absyn_AggrInfo _tmp9B5;union Cyc_Absyn_AggrInfoU _tmp9B6;struct Cyc_List_List*
_tmp9B7;struct Cyc_Absyn_DatatypeInfo _tmp9B9;union Cyc_Absyn_DatatypeInfoU _tmp9BA;
struct Cyc_List_List*_tmp9BB;struct Cyc_Absyn_DatatypeFieldInfo _tmp9BD;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9BE;struct Cyc_List_List*_tmp9BF;struct _tuple2*_tmp9C1;struct Cyc_List_List*
_tmp9C2;struct Cyc_Absyn_Typedefdecl*_tmp9C3;void**_tmp9C4;struct Cyc_Absyn_ArrayInfo
_tmp9C6;void*_tmp9C7;struct Cyc_Absyn_Tqual _tmp9C8;struct Cyc_Absyn_Exp*_tmp9C9;
union Cyc_Absyn_Constraint*_tmp9CA;struct Cyc_Position_Segment*_tmp9CB;struct Cyc_Absyn_PtrInfo
_tmp9CD;void*_tmp9CE;struct Cyc_Absyn_Tqual _tmp9CF;struct Cyc_Absyn_PtrAtts _tmp9D0;
void*_tmp9D1;union Cyc_Absyn_Constraint*_tmp9D2;union Cyc_Absyn_Constraint*_tmp9D3;
union Cyc_Absyn_Constraint*_tmp9D4;struct Cyc_Absyn_FnInfo _tmp9D6;struct Cyc_List_List*
_tmp9D7;struct Cyc_Core_Opt*_tmp9D8;void*_tmp9D9;struct Cyc_List_List*_tmp9DA;int
_tmp9DB;struct Cyc_Absyn_VarargInfo*_tmp9DC;struct Cyc_List_List*_tmp9DD;struct Cyc_List_List*
_tmp9DE;struct Cyc_List_List*_tmp9E0;enum Cyc_Absyn_AggrKind _tmp9E2;struct Cyc_List_List*
_tmp9E3;struct Cyc_Core_Opt*_tmp9E5;void*_tmp9E7;void*_tmp9E9;void*_tmp9EA;void*
_tmp9EC;struct Cyc_Absyn_Exp*_tmp9EE;void*_tmp9F8;void*_tmp9FA;struct Cyc_List_List*
_tmp9FC;_LL4F3: {struct Cyc_Absyn_VarType_struct*_tmp9B2=(struct Cyc_Absyn_VarType_struct*)
_tmp9B1;if(_tmp9B2->tag != 2)goto _LL4F5;else{_tmp9B3=_tmp9B2->f1;}}_LL4F4: {
struct _handler_cons _tmp9FE;_push_handler(& _tmp9FE);{int _tmpA00=0;if(setjmp(
_tmp9FE.handler))_tmpA00=1;if(!_tmpA00){{void*_tmpA01=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp9B3);_npop_handler(0);return
_tmpA01;};_pop_handler();}else{void*_tmp9FF=(void*)_exn_thrown;void*_tmpA03=
_tmp9FF;_LL52A: {struct Cyc_Core_Not_found_struct*_tmpA04=(struct Cyc_Core_Not_found_struct*)
_tmpA03;if(_tmpA04->tag != Cyc_Core_Not_found)goto _LL52C;}_LL52B: return t;_LL52C:;
_LL52D:(void)_throw(_tmpA03);_LL529:;}};}_LL4F5: {struct Cyc_Absyn_AggrType_struct*
_tmp9B4=(struct Cyc_Absyn_AggrType_struct*)_tmp9B1;if(_tmp9B4->tag != 12)goto
_LL4F7;else{_tmp9B5=_tmp9B4->f1;_tmp9B6=_tmp9B5.aggr_info;_tmp9B7=_tmp9B5.targs;}}
_LL4F6: {struct Cyc_List_List*_tmpA05=Cyc_Tcutil_substs(rgn,inst,_tmp9B7);struct
Cyc_Absyn_AggrType_struct _tmp1349;struct Cyc_Absyn_AggrInfo _tmp1348;struct Cyc_Absyn_AggrType_struct*
_tmp1347;return _tmpA05 == _tmp9B7?t:(void*)((_tmp1347=_cycalloc(sizeof(*_tmp1347)),((
_tmp1347[0]=((_tmp1349.tag=12,((_tmp1349.f1=((_tmp1348.aggr_info=_tmp9B6,((
_tmp1348.targs=_tmpA05,_tmp1348)))),_tmp1349)))),_tmp1347))));}_LL4F7: {struct
Cyc_Absyn_DatatypeType_struct*_tmp9B8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp9B1;if(_tmp9B8->tag != 3)goto _LL4F9;else{_tmp9B9=_tmp9B8->f1;_tmp9BA=_tmp9B9.datatype_info;
_tmp9BB=_tmp9B9.targs;}}_LL4F8: {struct Cyc_List_List*_tmpA09=Cyc_Tcutil_substs(
rgn,inst,_tmp9BB);struct Cyc_Absyn_DatatypeType_struct _tmp134F;struct Cyc_Absyn_DatatypeInfo
_tmp134E;struct Cyc_Absyn_DatatypeType_struct*_tmp134D;return _tmpA09 == _tmp9BB?t:(
void*)((_tmp134D=_cycalloc(sizeof(*_tmp134D)),((_tmp134D[0]=((_tmp134F.tag=3,((
_tmp134F.f1=((_tmp134E.datatype_info=_tmp9BA,((_tmp134E.targs=_tmpA09,_tmp134E)))),
_tmp134F)))),_tmp134D))));}_LL4F9: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp9BC=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp9B1;if(_tmp9BC->tag != 4)
goto _LL4FB;else{_tmp9BD=_tmp9BC->f1;_tmp9BE=_tmp9BD.field_info;_tmp9BF=_tmp9BD.targs;}}
_LL4FA: {struct Cyc_List_List*_tmpA0D=Cyc_Tcutil_substs(rgn,inst,_tmp9BF);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp1355;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1354;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1353;return _tmpA0D == 
_tmp9BF?t:(void*)((_tmp1353=_cycalloc(sizeof(*_tmp1353)),((_tmp1353[0]=((
_tmp1355.tag=4,((_tmp1355.f1=((_tmp1354.field_info=_tmp9BE,((_tmp1354.targs=
_tmpA0D,_tmp1354)))),_tmp1355)))),_tmp1353))));}_LL4FB: {struct Cyc_Absyn_TypedefType_struct*
_tmp9C0=(struct Cyc_Absyn_TypedefType_struct*)_tmp9B1;if(_tmp9C0->tag != 18)goto
_LL4FD;else{_tmp9C1=_tmp9C0->f1;_tmp9C2=_tmp9C0->f2;_tmp9C3=_tmp9C0->f3;_tmp9C4=
_tmp9C0->f4;}}_LL4FC: {struct Cyc_List_List*_tmpA11=Cyc_Tcutil_substs(rgn,inst,
_tmp9C2);struct Cyc_Absyn_TypedefType_struct _tmp1358;struct Cyc_Absyn_TypedefType_struct*
_tmp1357;return _tmpA11 == _tmp9C2?t:(void*)((_tmp1357=_cycalloc(sizeof(*_tmp1357)),((
_tmp1357[0]=((_tmp1358.tag=18,((_tmp1358.f1=_tmp9C1,((_tmp1358.f2=_tmpA11,((
_tmp1358.f3=_tmp9C3,((_tmp1358.f4=_tmp9C4,_tmp1358)))))))))),_tmp1357))));}
_LL4FD: {struct Cyc_Absyn_ArrayType_struct*_tmp9C5=(struct Cyc_Absyn_ArrayType_struct*)
_tmp9B1;if(_tmp9C5->tag != 9)goto _LL4FF;else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C6.elt_type;
_tmp9C8=_tmp9C6.tq;_tmp9C9=_tmp9C6.num_elts;_tmp9CA=_tmp9C6.zero_term;_tmp9CB=
_tmp9C6.zt_loc;}}_LL4FE: {void*_tmpA14=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9C7);
struct Cyc_Absyn_ArrayType_struct _tmp135E;struct Cyc_Absyn_ArrayInfo _tmp135D;
struct Cyc_Absyn_ArrayType_struct*_tmp135C;return _tmpA14 == _tmp9C7?t:(void*)((
_tmp135C=_cycalloc(sizeof(*_tmp135C)),((_tmp135C[0]=((_tmp135E.tag=9,((_tmp135E.f1=((
_tmp135D.elt_type=_tmpA14,((_tmp135D.tq=_tmp9C8,((_tmp135D.num_elts=_tmp9C9,((
_tmp135D.zero_term=_tmp9CA,((_tmp135D.zt_loc=_tmp9CB,_tmp135D)))))))))),_tmp135E)))),
_tmp135C))));}_LL4FF: {struct Cyc_Absyn_PointerType_struct*_tmp9CC=(struct Cyc_Absyn_PointerType_struct*)
_tmp9B1;if(_tmp9CC->tag != 5)goto _LL501;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CD.elt_typ;
_tmp9CF=_tmp9CD.elt_tq;_tmp9D0=_tmp9CD.ptr_atts;_tmp9D1=_tmp9D0.rgn;_tmp9D2=
_tmp9D0.nullable;_tmp9D3=_tmp9D0.bounds;_tmp9D4=_tmp9D0.zero_term;}}_LL500: {
void*_tmpA18=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9CE);void*_tmpA19=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9D1);union Cyc_Absyn_Constraint*_tmpA1A=_tmp9D3;{void*_tmpA1B=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp9D3);struct Cyc_Absyn_Exp*_tmpA1D;_LL52F: {struct Cyc_Absyn_Upper_b_struct*
_tmpA1C=(struct Cyc_Absyn_Upper_b_struct*)_tmpA1B;if(_tmpA1C->tag != 1)goto _LL531;
else{_tmpA1D=_tmpA1C->f1;}}_LL530: {struct Cyc_Absyn_Exp*_tmpA1E=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA1D);if(_tmpA1E != _tmpA1D){struct Cyc_Absyn_Upper_b_struct _tmp1361;
struct Cyc_Absyn_Upper_b_struct*_tmp1360;_tmpA1A=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1360=_cycalloc(sizeof(*_tmp1360)),((
_tmp1360[0]=((_tmp1361.tag=1,((_tmp1361.f1=_tmpA1E,_tmp1361)))),_tmp1360)))));}
goto _LL52E;}_LL531:;_LL532: goto _LL52E;_LL52E:;}if((_tmpA18 == _tmp9CE  && _tmpA19
== _tmp9D1) && _tmpA1A == _tmp9D3)return t;{struct Cyc_Absyn_PointerType_struct
_tmp136B;struct Cyc_Absyn_PtrAtts _tmp136A;struct Cyc_Absyn_PtrInfo _tmp1369;struct
Cyc_Absyn_PointerType_struct*_tmp1368;return(void*)((_tmp1368=_cycalloc(sizeof(*
_tmp1368)),((_tmp1368[0]=((_tmp136B.tag=5,((_tmp136B.f1=((_tmp1369.elt_typ=
_tmpA18,((_tmp1369.elt_tq=_tmp9CF,((_tmp1369.ptr_atts=((_tmp136A.rgn=_tmpA19,((
_tmp136A.nullable=_tmp9D2,((_tmp136A.bounds=_tmpA1A,((_tmp136A.zero_term=_tmp9D4,((
_tmp136A.ptrloc=0,_tmp136A)))))))))),_tmp1369)))))),_tmp136B)))),_tmp1368))));};}
_LL501: {struct Cyc_Absyn_FnType_struct*_tmp9D5=(struct Cyc_Absyn_FnType_struct*)
_tmp9B1;if(_tmp9D5->tag != 10)goto _LL503;else{_tmp9D6=_tmp9D5->f1;_tmp9D7=_tmp9D6.tvars;
_tmp9D8=_tmp9D6.effect;_tmp9D9=_tmp9D6.ret_typ;_tmp9DA=_tmp9D6.args;_tmp9DB=
_tmp9D6.c_varargs;_tmp9DC=_tmp9D6.cyc_varargs;_tmp9DD=_tmp9D6.rgn_po;_tmp9DE=
_tmp9D6.attributes;}}_LL502:{struct Cyc_List_List*_tmpA25=_tmp9D7;for(0;_tmpA25 != 
0;_tmpA25=_tmpA25->tl){struct _tuple15*_tmp1375;struct Cyc_Absyn_VarType_struct
_tmp1374;struct Cyc_Absyn_VarType_struct*_tmp1373;struct Cyc_List_List*_tmp1372;
inst=((_tmp1372=_region_malloc(rgn,sizeof(*_tmp1372)),((_tmp1372->hd=((_tmp1375=
_region_malloc(rgn,sizeof(*_tmp1375)),((_tmp1375->f1=(struct Cyc_Absyn_Tvar*)
_tmpA25->hd,((_tmp1375->f2=(void*)((_tmp1373=_cycalloc(sizeof(*_tmp1373)),((
_tmp1373[0]=((_tmp1374.tag=2,((_tmp1374.f1=(struct Cyc_Absyn_Tvar*)_tmpA25->hd,
_tmp1374)))),_tmp1373)))),_tmp1375)))))),((_tmp1372->tl=inst,_tmp1372))))));}}{
struct Cyc_List_List*_tmpA2B;struct Cyc_List_List*_tmpA2C;struct _tuple1 _tmpA2A=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp9DA));
_tmpA2B=_tmpA2A.f1;_tmpA2C=_tmpA2A.f2;{struct Cyc_List_List*_tmpA2D=Cyc_Tcutil_substs(
rgn,inst,_tmpA2C);struct Cyc_List_List*_tmpA2E=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA2B,_tmpA2D));struct Cyc_Core_Opt*
eff2;if(_tmp9D8 == 0)eff2=0;else{void*_tmpA2F=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp9D8->v);if(_tmpA2F == (void*)_tmp9D8->v)eff2=_tmp9D8;else{struct Cyc_Core_Opt*
_tmp1376;eff2=((_tmp1376=_cycalloc(sizeof(*_tmp1376)),((_tmp1376->v=_tmpA2F,
_tmp1376))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp9DC == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA32;struct Cyc_Absyn_Tqual _tmpA33;void*
_tmpA34;int _tmpA35;struct Cyc_Absyn_VarargInfo _tmpA31=*_tmp9DC;_tmpA32=_tmpA31.name;
_tmpA33=_tmpA31.tq;_tmpA34=_tmpA31.type;_tmpA35=_tmpA31.inject;{void*_tmpA36=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA34);if(_tmpA36 == _tmpA34)cyc_varargs2=_tmp9DC;else{struct Cyc_Absyn_VarargInfo*
_tmp1377;cyc_varargs2=((_tmp1377=_cycalloc(sizeof(*_tmp1377)),((_tmp1377->name=
_tmpA32,((_tmp1377->tq=_tmpA33,((_tmp1377->type=_tmpA36,((_tmp1377->inject=
_tmpA35,_tmp1377))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA39;struct Cyc_List_List*_tmpA3A;struct _tuple1 _tmpA38=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp9DD);_tmpA39=_tmpA38.f1;_tmpA3A=_tmpA38.f2;{struct Cyc_List_List*
_tmpA3B=Cyc_Tcutil_substs(rgn,inst,_tmpA39);struct Cyc_List_List*_tmpA3C=Cyc_Tcutil_substs(
rgn,inst,_tmpA3A);if(_tmpA3B == _tmpA39  && _tmpA3C == _tmpA3A)rgn_po2=_tmp9DD;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA3B,_tmpA3C);}{struct Cyc_Absyn_FnType_struct _tmp137D;struct
Cyc_Absyn_FnInfo _tmp137C;struct Cyc_Absyn_FnType_struct*_tmp137B;return(void*)((
_tmp137B=_cycalloc(sizeof(*_tmp137B)),((_tmp137B[0]=((_tmp137D.tag=10,((_tmp137D.f1=((
_tmp137C.tvars=_tmp9D7,((_tmp137C.effect=eff2,((_tmp137C.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9D9),((_tmp137C.args=_tmpA2E,((_tmp137C.c_varargs=_tmp9DB,((
_tmp137C.cyc_varargs=cyc_varargs2,((_tmp137C.rgn_po=rgn_po2,((_tmp137C.attributes=
_tmp9DE,_tmp137C)))))))))))))))),_tmp137D)))),_tmp137B))));};};};};};};_LL503: {
struct Cyc_Absyn_TupleType_struct*_tmp9DF=(struct Cyc_Absyn_TupleType_struct*)
_tmp9B1;if(_tmp9DF->tag != 11)goto _LL505;else{_tmp9E0=_tmp9DF->f1;}}_LL504: {
struct Cyc_List_List*_tmpA40=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp9E0);struct Cyc_List_List*_tmpA41=Cyc_Tcutil_substs(rgn,inst,_tmpA40);if(
_tmpA41 == _tmpA40)return t;{struct Cyc_List_List*_tmpA42=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp9E0,_tmpA41);struct Cyc_Absyn_TupleType_struct
_tmp1380;struct Cyc_Absyn_TupleType_struct*_tmp137F;return(void*)((_tmp137F=
_cycalloc(sizeof(*_tmp137F)),((_tmp137F[0]=((_tmp1380.tag=11,((_tmp1380.f1=
_tmpA42,_tmp1380)))),_tmp137F))));};}_LL505: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9E1=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp9B1;if(_tmp9E1->tag != 13)goto
_LL507;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=_tmp9E1->f2;}}_LL506: {struct Cyc_List_List*
_tmpA45=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp9E3);struct
Cyc_List_List*_tmpA46=Cyc_Tcutil_substs(rgn,inst,_tmpA45);if(_tmpA46 == _tmpA45)
return t;{struct Cyc_List_List*_tmpA47=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp9E3,_tmpA46);struct Cyc_Absyn_AnonAggrType_struct
_tmp1383;struct Cyc_Absyn_AnonAggrType_struct*_tmp1382;return(void*)((_tmp1382=
_cycalloc(sizeof(*_tmp1382)),((_tmp1382[0]=((_tmp1383.tag=13,((_tmp1383.f1=
_tmp9E2,((_tmp1383.f2=_tmpA47,_tmp1383)))))),_tmp1382))));};}_LL507: {struct Cyc_Absyn_Evar_struct*
_tmp9E4=(struct Cyc_Absyn_Evar_struct*)_tmp9B1;if(_tmp9E4->tag != 1)goto _LL509;
else{_tmp9E5=_tmp9E4->f2;}}_LL508: if(_tmp9E5 != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp9E5->v);else{return t;}_LL509: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp9E6=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp9B1;if(_tmp9E6->tag != 16)goto
_LL50B;else{_tmp9E7=(void*)_tmp9E6->f1;}}_LL50A: {void*_tmpA4A=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9E7);struct Cyc_Absyn_RgnHandleType_struct _tmp1386;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1385;return _tmpA4A == _tmp9E7?t:(void*)((_tmp1385=_cycalloc(sizeof(*_tmp1385)),((
_tmp1385[0]=((_tmp1386.tag=16,((_tmp1386.f1=(void*)_tmpA4A,_tmp1386)))),_tmp1385))));}
_LL50B: {struct Cyc_Absyn_DynRgnType_struct*_tmp9E8=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp9B1;if(_tmp9E8->tag != 17)goto _LL50D;else{_tmp9E9=(void*)_tmp9E8->f1;_tmp9EA=(
void*)_tmp9E8->f2;}}_LL50C: {void*_tmpA4D=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp9E9);void*_tmpA4E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9EA);struct Cyc_Absyn_DynRgnType_struct
_tmp1389;struct Cyc_Absyn_DynRgnType_struct*_tmp1388;return _tmpA4D == _tmp9E9  && 
_tmpA4E == _tmp9EA?t:(void*)((_tmp1388=_cycalloc(sizeof(*_tmp1388)),((_tmp1388[0]=((
_tmp1389.tag=17,((_tmp1389.f1=(void*)_tmpA4D,((_tmp1389.f2=(void*)_tmpA4E,
_tmp1389)))))),_tmp1388))));}_LL50D: {struct Cyc_Absyn_TagType_struct*_tmp9EB=(
struct Cyc_Absyn_TagType_struct*)_tmp9B1;if(_tmp9EB->tag != 20)goto _LL50F;else{
_tmp9EC=(void*)_tmp9EB->f1;}}_LL50E: {void*_tmpA51=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp9EC);struct Cyc_Absyn_TagType_struct _tmp138C;struct Cyc_Absyn_TagType_struct*
_tmp138B;return _tmpA51 == _tmp9EC?t:(void*)((_tmp138B=_cycalloc(sizeof(*_tmp138B)),((
_tmp138B[0]=((_tmp138C.tag=20,((_tmp138C.f1=(void*)_tmpA51,_tmp138C)))),_tmp138B))));}
_LL50F: {struct Cyc_Absyn_ValueofType_struct*_tmp9ED=(struct Cyc_Absyn_ValueofType_struct*)
_tmp9B1;if(_tmp9ED->tag != 19)goto _LL511;else{_tmp9EE=_tmp9ED->f1;}}_LL510: {
struct Cyc_Absyn_Exp*_tmpA54=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp9EE);struct Cyc_Absyn_ValueofType_struct
_tmp138F;struct Cyc_Absyn_ValueofType_struct*_tmp138E;return _tmpA54 == _tmp9EE?t:(
void*)((_tmp138E=_cycalloc(sizeof(*_tmp138E)),((_tmp138E[0]=((_tmp138F.tag=19,((
_tmp138F.f1=_tmpA54,_tmp138F)))),_tmp138E))));}_LL511: {struct Cyc_Absyn_EnumType_struct*
_tmp9EF=(struct Cyc_Absyn_EnumType_struct*)_tmp9B1;if(_tmp9EF->tag != 14)goto
_LL513;}_LL512: goto _LL514;_LL513: {struct Cyc_Absyn_AnonEnumType_struct*_tmp9F0=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp9B1;if(_tmp9F0->tag != 15)goto _LL515;}
_LL514: goto _LL516;_LL515: {struct Cyc_Absyn_VoidType_struct*_tmp9F1=(struct Cyc_Absyn_VoidType_struct*)
_tmp9B1;if(_tmp9F1->tag != 0)goto _LL517;}_LL516: goto _LL518;_LL517: {struct Cyc_Absyn_IntType_struct*
_tmp9F2=(struct Cyc_Absyn_IntType_struct*)_tmp9B1;if(_tmp9F2->tag != 6)goto _LL519;}
_LL518: goto _LL51A;_LL519: {struct Cyc_Absyn_FloatType_struct*_tmp9F3=(struct Cyc_Absyn_FloatType_struct*)
_tmp9B1;if(_tmp9F3->tag != 7)goto _LL51B;}_LL51A: goto _LL51C;_LL51B: {struct Cyc_Absyn_DoubleType_struct*
_tmp9F4=(struct Cyc_Absyn_DoubleType_struct*)_tmp9B1;if(_tmp9F4->tag != 8)goto
_LL51D;}_LL51C: goto _LL51E;_LL51D: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9F5=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp9B1;if(_tmp9F5->tag != 22)goto _LL51F;}
_LL51E: goto _LL520;_LL51F: {struct Cyc_Absyn_HeapRgn_struct*_tmp9F6=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp9B1;if(_tmp9F6->tag != 21)goto _LL521;}_LL520: return t;_LL521: {struct Cyc_Absyn_RgnsEff_struct*
_tmp9F7=(struct Cyc_Absyn_RgnsEff_struct*)_tmp9B1;if(_tmp9F7->tag != 25)goto _LL523;
else{_tmp9F8=(void*)_tmp9F7->f1;}}_LL522: {void*_tmpA57=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp9F8);struct Cyc_Absyn_RgnsEff_struct _tmp1392;struct Cyc_Absyn_RgnsEff_struct*
_tmp1391;return _tmpA57 == _tmp9F8?t:(void*)((_tmp1391=_cycalloc(sizeof(*_tmp1391)),((
_tmp1391[0]=((_tmp1392.tag=25,((_tmp1392.f1=(void*)_tmpA57,_tmp1392)))),_tmp1391))));}
_LL523: {struct Cyc_Absyn_AccessEff_struct*_tmp9F9=(struct Cyc_Absyn_AccessEff_struct*)
_tmp9B1;if(_tmp9F9->tag != 23)goto _LL525;else{_tmp9FA=(void*)_tmp9F9->f1;}}_LL524: {
void*_tmpA5A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp9FA);struct Cyc_Absyn_AccessEff_struct
_tmp1395;struct Cyc_Absyn_AccessEff_struct*_tmp1394;return _tmpA5A == _tmp9FA?t:(
void*)((_tmp1394=_cycalloc(sizeof(*_tmp1394)),((_tmp1394[0]=((_tmp1395.tag=23,((
_tmp1395.f1=(void*)_tmpA5A,_tmp1395)))),_tmp1394))));}_LL525: {struct Cyc_Absyn_JoinEff_struct*
_tmp9FB=(struct Cyc_Absyn_JoinEff_struct*)_tmp9B1;if(_tmp9FB->tag != 24)goto _LL527;
else{_tmp9FC=_tmp9FB->f1;}}_LL526: {struct Cyc_List_List*_tmpA5D=Cyc_Tcutil_substs(
rgn,inst,_tmp9FC);struct Cyc_Absyn_JoinEff_struct _tmp1398;struct Cyc_Absyn_JoinEff_struct*
_tmp1397;return _tmpA5D == _tmp9FC?t:(void*)((_tmp1397=_cycalloc(sizeof(*_tmp1397)),((
_tmp1397[0]=((_tmp1398.tag=24,((_tmp1398.f1=_tmpA5D,_tmp1398)))),_tmp1397))));}
_LL527: {struct Cyc_Absyn_TypeDeclType_struct*_tmp9FD=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp9B1;if(_tmp9FD->tag != 26)goto _LL4F2;}_LL528: {const char*_tmp139B;void*
_tmp139A;(_tmp139A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp139B="found typedecltype in rsubs",_tag_dyneither(
_tmp139B,sizeof(char),28))),_tag_dyneither(_tmp139A,sizeof(void*),0)));}_LL4F2:;}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmpA62=(void*)ts->hd;
struct Cyc_List_List*_tmpA63=ts->tl;void*_tmpA64=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpA62);struct Cyc_List_List*_tmpA65=Cyc_Tcutil_substs(rgn,inst,_tmpA63);if(
_tmpA62 == _tmpA64  && _tmpA63 == _tmpA65)return ts;{struct Cyc_List_List*_tmp139C;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp139C=_cycalloc(sizeof(*
_tmp139C)),((_tmp139C->hd=_tmpA64,((_tmp139C->tl=_tmpA65,_tmp139C)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpA67=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp139F;struct _tuple15*_tmp139E;return(
_tmp139E=_cycalloc(sizeof(*_tmp139E)),((_tmp139E->f1=tv,((_tmp139E->f2=Cyc_Absyn_new_evar(
_tmpA67,((_tmp139F=_cycalloc(sizeof(*_tmp139F)),((_tmp139F->v=s,_tmp139F))))),
_tmp139E)))));}struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple16 _tmpA6B;struct Cyc_List_List*_tmpA6C;
struct _RegionHandle*_tmpA6D;struct _tuple16*_tmpA6A=env;_tmpA6B=*_tmpA6A;_tmpA6C=
_tmpA6B.f1;_tmpA6D=_tmpA6B.f2;{struct Cyc_Core_Opt*_tmpA6E=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13A2;struct
_tuple15*_tmp13A1;return(_tmp13A1=_region_malloc(_tmpA6D,sizeof(*_tmp13A1)),((
_tmp13A1->f1=tv,((_tmp13A1->f2=Cyc_Absyn_new_evar(_tmpA6E,((_tmp13A2=_cycalloc(
sizeof(*_tmp13A2)),((_tmp13A2->v=_tmpA6C,_tmp13A2))))),_tmp13A1)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp13A8;void*_tmp13A7[3];
struct Cyc_String_pa_struct _tmp13A6;struct Cyc_String_pa_struct _tmp13A5;struct Cyc_String_pa_struct
_tmp13A4;(_tmp13A4.tag=0,((_tmp13A4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp13A5.tag=0,((_tmp13A5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp13A6.tag=0,((_tmp13A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp13A7[0]=& _tmp13A6,((_tmp13A7[1]=& _tmp13A5,((_tmp13A7[2]=& _tmp13A4,Cyc_Tcutil_terr(
loc,((_tmp13A8="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp13A8,sizeof(char),59))),_tag_dyneither(_tmp13A7,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp13AB;void*_tmp13AA;(_tmp13AA=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13AB="same type variable has different identity!",
_tag_dyneither(_tmp13AB,sizeof(char),43))),_tag_dyneither(_tmp13AA,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp13AC;return(_tmp13AC=_cycalloc(sizeof(*_tmp13AC)),((_tmp13AC->hd=tv,((
_tmp13AC->tl=tvs,_tmp13AC)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp13AF;void*_tmp13AE;(_tmp13AE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13AF="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp13AF,sizeof(char),39))),_tag_dyneither(_tmp13AE,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpA7B=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpA7B->identity == - 1){const char*
_tmp13B2;void*_tmp13B1;(_tmp13B1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13B2="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp13B2,sizeof(char),41))),_tag_dyneither(_tmp13B1,sizeof(void*),
0)));}if(_tmpA7B->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp13B3;return(_tmp13B3=_cycalloc(sizeof(*_tmp13B3)),((_tmp13B3->hd=tv,((
_tmp13B3->tl=tvs,_tmp13B3)))));};}struct _tuple27{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp13B6;void*_tmp13B5;(_tmp13B5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13B6="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp13B6,sizeof(char),44))),_tag_dyneither(_tmp13B5,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple27
_tmpA82;struct Cyc_Absyn_Tvar*_tmpA83;int _tmpA84;int*_tmpA85;struct _tuple27*
_tmpA81=(struct _tuple27*)tvs2->hd;_tmpA82=*_tmpA81;_tmpA83=_tmpA82.f1;_tmpA84=
_tmpA82.f2;_tmpA85=(int*)&(*_tmpA81).f2;if(_tmpA83->identity == - 1){const char*
_tmp13B9;void*_tmp13B8;(_tmp13B8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13B9="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp13B9,sizeof(char),46))),_tag_dyneither(_tmp13B8,sizeof(void*),
0)));}if(_tmpA83->identity == tv->identity){*_tmpA85=*_tmpA85  || b;return tvs;}}}{
struct _tuple27*_tmp13BC;struct Cyc_List_List*_tmp13BB;return(_tmp13BB=
_region_malloc(r,sizeof(*_tmp13BB)),((_tmp13BB->hd=((_tmp13BC=_region_malloc(r,
sizeof(*_tmp13BC)),((_tmp13BC->f1=tv,((_tmp13BC->f2=b,_tmp13BC)))))),((_tmp13BB->tl=
tvs,_tmp13BB)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp13C0;void*_tmp13BF[1];struct Cyc_String_pa_struct _tmp13BE;(_tmp13BE.tag=0,((
_tmp13BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp13BF[0]=& _tmp13BE,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13C0="bound tvar id for %s is NULL",
_tag_dyneither(_tmp13C0,sizeof(char),29))),_tag_dyneither(_tmp13BF,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp13C1;return(_tmp13C1=_cycalloc(sizeof(*
_tmp13C1)),((_tmp13C1->hd=tv,((_tmp13C1->tl=tvs,_tmp13C1)))));};}struct _tuple28{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpA8E=Cyc_Tcutil_compress(
e);int _tmpA90;_LL534: {struct Cyc_Absyn_Evar_struct*_tmpA8F=(struct Cyc_Absyn_Evar_struct*)
_tmpA8E;if(_tmpA8F->tag != 1)goto _LL536;else{_tmpA90=_tmpA8F->f3;}}_LL535:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple28 _tmpA92;void*
_tmpA93;int _tmpA94;int*_tmpA95;struct _tuple28*_tmpA91=(struct _tuple28*)es2->hd;
_tmpA92=*_tmpA91;_tmpA93=_tmpA92.f1;_tmpA94=_tmpA92.f2;_tmpA95=(int*)&(*_tmpA91).f2;{
void*_tmpA96=Cyc_Tcutil_compress(_tmpA93);int _tmpA98;_LL539: {struct Cyc_Absyn_Evar_struct*
_tmpA97=(struct Cyc_Absyn_Evar_struct*)_tmpA96;if(_tmpA97->tag != 1)goto _LL53B;
else{_tmpA98=_tmpA97->f3;}}_LL53A: if(_tmpA90 == _tmpA98){if(b != *_tmpA95)*_tmpA95=
1;return es;}goto _LL538;_LL53B:;_LL53C: goto _LL538;_LL538:;};}}{struct _tuple28*
_tmp13C4;struct Cyc_List_List*_tmp13C3;return(_tmp13C3=_region_malloc(r,sizeof(*
_tmp13C3)),((_tmp13C3->hd=((_tmp13C4=_region_malloc(r,sizeof(*_tmp13C4)),((
_tmp13C4->f1=e,((_tmp13C4->f2=b,_tmp13C4)))))),((_tmp13C3->tl=es,_tmp13C3)))));};
_LL536:;_LL537: return es;_LL533:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp13C5;r=((_tmp13C5=_region_malloc(rgn,sizeof(*_tmp13C5)),((
_tmp13C5->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp13C5->tl=r,_tmp13C5))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpA9D;int _tmpA9E;struct
_tuple27 _tmpA9C=*((struct _tuple27*)tvs->hd);_tmpA9D=_tmpA9C.f1;_tmpA9E=_tmpA9C.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpA9D->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp13C6;res=((_tmp13C6=_region_malloc(r,sizeof(*_tmp13C6)),((
_tmp13C6->hd=(struct _tuple27*)tvs->hd,((_tmp13C6->tl=res,_tmp13C6))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp13CA;void*_tmp13C9[1];struct Cyc_String_pa_struct _tmp13C8;(_tmp13C8.tag=
0,((_tmp13C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp13C9[0]=&
_tmp13C8,Cyc_Tcutil_terr(loc,((_tmp13CA="bitfield %s does not have constant width",
_tag_dyneither(_tmp13CA,sizeof(char),41))),_tag_dyneither(_tmp13C9,sizeof(void*),
1)))))));}else{unsigned int _tmpAA4;int _tmpAA5;struct _tuple13 _tmpAA3=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAA4=_tmpAA3.f1;_tmpAA5=_tmpAA3.f2;if(!_tmpAA5){
const char*_tmp13CD;void*_tmp13CC;(_tmp13CC=0,Cyc_Tcutil_terr(loc,((_tmp13CD="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp13CD,sizeof(char),45))),_tag_dyneither(_tmp13CC,sizeof(void*),
0)));}w=_tmpAA4;}{void*_tmpAA8=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAAA;_LL53E: {struct Cyc_Absyn_IntType_struct*_tmpAA9=(struct Cyc_Absyn_IntType_struct*)
_tmpAA8;if(_tmpAA9->tag != 6)goto _LL540;else{_tmpAAA=_tmpAA9->f2;}}_LL53F: switch(
_tmpAAA){case Cyc_Absyn_Char_sz: _LL542: if(w > 8){const char*_tmp13D0;void*_tmp13CF;(
_tmp13CF=0,Cyc_Tcutil_terr(loc,((_tmp13D0="bitfield larger than type",
_tag_dyneither(_tmp13D0,sizeof(char),26))),_tag_dyneither(_tmp13CF,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL543: if(w > 16){const char*_tmp13D3;void*
_tmp13D2;(_tmp13D2=0,Cyc_Tcutil_terr(loc,((_tmp13D3="bitfield larger than type",
_tag_dyneither(_tmp13D3,sizeof(char),26))),_tag_dyneither(_tmp13D2,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL544: goto _LL545;case Cyc_Absyn_Int_sz: _LL545:
if(w > 32){const char*_tmp13D6;void*_tmp13D5;(_tmp13D5=0,Cyc_Tcutil_terr(loc,((
_tmp13D6="bitfield larger than type",_tag_dyneither(_tmp13D6,sizeof(char),26))),
_tag_dyneither(_tmp13D5,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL546: if(w > 64){const char*_tmp13D9;void*_tmp13D8;(_tmp13D8=0,Cyc_Tcutil_terr(
loc,((_tmp13D9="bitfield larger than type",_tag_dyneither(_tmp13D9,sizeof(char),
26))),_tag_dyneither(_tmp13D8,sizeof(void*),0)));}break;}goto _LL53D;_LL540:;
_LL541:{const char*_tmp13DE;void*_tmp13DD[2];struct Cyc_String_pa_struct _tmp13DC;
struct Cyc_String_pa_struct _tmp13DB;(_tmp13DB.tag=0,((_tmp13DB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp13DC.tag=0,((_tmp13DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp13DD[0]=& _tmp13DC,((_tmp13DD[1]=& _tmp13DB,Cyc_Tcutil_terr(loc,((_tmp13DE="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp13DE,sizeof(char),52))),_tag_dyneither(_tmp13DD,sizeof(void*),
2)))))))))))));}goto _LL53D;_LL53D:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpAB7=(void*)atts->hd;_LL549: {struct Cyc_Absyn_Packed_att_struct*
_tmpAB8=(struct Cyc_Absyn_Packed_att_struct*)_tmpAB7;if(_tmpAB8->tag != 7)goto
_LL54B;}_LL54A: continue;_LL54B: {struct Cyc_Absyn_Aligned_att_struct*_tmpAB9=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpAB7;if(_tmpAB9->tag != 6)goto _LL54D;}
_LL54C: continue;_LL54D:;_LL54E: {const char*_tmp13E3;void*_tmp13E2[2];struct Cyc_String_pa_struct
_tmp13E1;struct Cyc_String_pa_struct _tmp13E0;(_tmp13E0.tag=0,((_tmp13E0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp13E1.tag=0,((_tmp13E1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp13E2[0]=& _tmp13E1,((_tmp13E2[1]=& _tmp13E0,Cyc_Tcutil_terr(loc,((
_tmp13E3="bad attribute %s on member %s",_tag_dyneither(_tmp13E3,sizeof(char),30))),
_tag_dyneither(_tmp13E2,sizeof(void*),2)))))))))))));}_LL548:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpABE=t;struct
Cyc_Absyn_Typedefdecl*_tmpAC0;void**_tmpAC1;_LL550: {struct Cyc_Absyn_TypedefType_struct*
_tmpABF=(struct Cyc_Absyn_TypedefType_struct*)_tmpABE;if(_tmpABF->tag != 18)goto
_LL552;else{_tmpAC0=_tmpABF->f3;_tmpAC1=_tmpABF->f4;}}_LL551: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpAC0))->tq).real_const  || (_tmpAC0->tq).print_const){if(
declared_const){const char*_tmp13E6;void*_tmp13E5;(_tmp13E5=0,Cyc_Tcutil_warn(loc,((
_tmp13E6="extra const",_tag_dyneither(_tmp13E6,sizeof(char),12))),_tag_dyneither(
_tmp13E5,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpAC1)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpAC1);else{return declared_const;}_LL552:;_LL553: return
declared_const;_LL54F:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpAC4=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpAC6;struct Cyc_Absyn_PtrAtts _tmpAC7;void*_tmpAC8;
_LL555: {struct Cyc_Absyn_PointerType_struct*_tmpAC5=(struct Cyc_Absyn_PointerType_struct*)
_tmpAC4;if(_tmpAC5->tag != 5)goto _LL557;else{_tmpAC6=_tmpAC5->f1;_tmpAC7=_tmpAC6.ptr_atts;
_tmpAC8=_tmpAC7.rgn;}}_LL556:{void*_tmpAC9=Cyc_Tcutil_compress(_tmpAC8);struct
Cyc_Absyn_Tvar*_tmpACB;_LL55A: {struct Cyc_Absyn_VarType_struct*_tmpACA=(struct
Cyc_Absyn_VarType_struct*)_tmpAC9;if(_tmpACA->tag != 2)goto _LL55C;else{_tmpACB=
_tmpACA->f1;}}_LL55B: return Cyc_Absyn_tvar_cmp(tvar,_tmpACB)== 0;_LL55C:;_LL55D:
goto _LL559;_LL559:;}goto _LL554;_LL557:;_LL558: goto _LL554;_LL554:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpACC=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpACE;
struct Cyc_Absyn_Kind _tmpACF;enum Cyc_Absyn_KindQual _tmpAD0;enum Cyc_Absyn_AliasQual
_tmpAD1;struct Cyc_Absyn_Kind*_tmpAD3;struct Cyc_Absyn_Kind _tmpAD4;enum Cyc_Absyn_KindQual
_tmpAD5;enum Cyc_Absyn_AliasQual _tmpAD6;_LL55F: {struct Cyc_Absyn_Less_kb_struct*
_tmpACD=(struct Cyc_Absyn_Less_kb_struct*)_tmpACC;if(_tmpACD->tag != 2)goto _LL561;
else{_tmpACE=_tmpACD->f2;_tmpACF=*_tmpACE;_tmpAD0=_tmpACF.kind;if(_tmpAD0 != Cyc_Absyn_RgnKind)
goto _LL561;_tmpAD1=_tmpACF.aliasqual;if(_tmpAD1 != Cyc_Absyn_Top)goto _LL561;}}
_LL560: goto _LL562;_LL561: {struct Cyc_Absyn_Eq_kb_struct*_tmpAD2=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpACC;if(_tmpAD2->tag != 0)goto _LL563;else{_tmpAD3=_tmpAD2->f1;_tmpAD4=*_tmpAD3;
_tmpAD5=_tmpAD4.kind;if(_tmpAD5 != Cyc_Absyn_RgnKind)goto _LL563;_tmpAD6=_tmpAD4.aliasqual;
if(_tmpAD6 != Cyc_Absyn_Top)goto _LL563;}}_LL562: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL563:;_LL564: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL55E:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple29{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpAD7=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpADA;struct Cyc_Core_Opt**
_tmpADB;struct Cyc_Core_Opt*_tmpADC;struct Cyc_Core_Opt**_tmpADD;struct Cyc_Absyn_Tvar*
_tmpADF;struct Cyc_Absyn_TypeDecl*_tmpAE1;struct Cyc_Absyn_TypeDecl _tmpAE2;void*
_tmpAE3;void**_tmpAE4;void***_tmpAE5;struct Cyc_List_List*_tmpAE7;struct _tuple2*
_tmpAE9;struct Cyc_Absyn_Enumdecl*_tmpAEA;struct Cyc_Absyn_Enumdecl**_tmpAEB;
struct Cyc_Absyn_DatatypeInfo _tmpAED;union Cyc_Absyn_DatatypeInfoU _tmpAEE;union Cyc_Absyn_DatatypeInfoU*
_tmpAEF;struct Cyc_List_List*_tmpAF0;struct Cyc_List_List**_tmpAF1;struct Cyc_Absyn_DatatypeFieldInfo
_tmpAF3;union Cyc_Absyn_DatatypeFieldInfoU _tmpAF4;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpAF5;struct Cyc_List_List*_tmpAF6;struct Cyc_Absyn_PtrInfo _tmpAF8;void*_tmpAF9;
struct Cyc_Absyn_Tqual _tmpAFA;struct Cyc_Absyn_Tqual*_tmpAFB;struct Cyc_Absyn_PtrAtts
_tmpAFC;void*_tmpAFD;union Cyc_Absyn_Constraint*_tmpAFE;union Cyc_Absyn_Constraint*
_tmpAFF;union Cyc_Absyn_Constraint*_tmpB00;void*_tmpB02;struct Cyc_Absyn_Exp*
_tmpB04;struct Cyc_Absyn_ArrayInfo _tmpB09;void*_tmpB0A;struct Cyc_Absyn_Tqual
_tmpB0B;struct Cyc_Absyn_Tqual*_tmpB0C;struct Cyc_Absyn_Exp*_tmpB0D;struct Cyc_Absyn_Exp**
_tmpB0E;union Cyc_Absyn_Constraint*_tmpB0F;struct Cyc_Position_Segment*_tmpB10;
struct Cyc_Absyn_FnInfo _tmpB12;struct Cyc_List_List*_tmpB13;struct Cyc_List_List**
_tmpB14;struct Cyc_Core_Opt*_tmpB15;struct Cyc_Core_Opt**_tmpB16;void*_tmpB17;
struct Cyc_List_List*_tmpB18;int _tmpB19;struct Cyc_Absyn_VarargInfo*_tmpB1A;struct
Cyc_List_List*_tmpB1B;struct Cyc_List_List*_tmpB1C;struct Cyc_List_List*_tmpB1E;
enum Cyc_Absyn_AggrKind _tmpB20;struct Cyc_List_List*_tmpB21;struct Cyc_Absyn_AggrInfo
_tmpB23;union Cyc_Absyn_AggrInfoU _tmpB24;union Cyc_Absyn_AggrInfoU*_tmpB25;struct
Cyc_List_List*_tmpB26;struct Cyc_List_List**_tmpB27;struct _tuple2*_tmpB29;struct
Cyc_List_List*_tmpB2A;struct Cyc_List_List**_tmpB2B;struct Cyc_Absyn_Typedefdecl*
_tmpB2C;struct Cyc_Absyn_Typedefdecl**_tmpB2D;void**_tmpB2E;void***_tmpB2F;void*
_tmpB33;void*_tmpB35;void*_tmpB36;void*_tmpB38;void*_tmpB3A;struct Cyc_List_List*
_tmpB3C;_LL566: {struct Cyc_Absyn_VoidType_struct*_tmpAD8=(struct Cyc_Absyn_VoidType_struct*)
_tmpAD7;if(_tmpAD8->tag != 0)goto _LL568;}_LL567: goto _LL565;_LL568: {struct Cyc_Absyn_Evar_struct*
_tmpAD9=(struct Cyc_Absyn_Evar_struct*)_tmpAD7;if(_tmpAD9->tag != 1)goto _LL56A;
else{_tmpADA=_tmpAD9->f1;_tmpADB=(struct Cyc_Core_Opt**)& _tmpAD9->f1;_tmpADC=
_tmpAD9->f2;_tmpADD=(struct Cyc_Core_Opt**)& _tmpAD9->f2;}}_LL569: if(*_tmpADB == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmpADB))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpADB))->v,expected_kind))*_tmpADB=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpADD=(
struct Cyc_Core_Opt*)& urgn;else{*_tmpADD=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp13E9;struct Cyc_Absyn_Less_kb_struct*
_tmp13E8;struct Cyc_Absyn_Tvar*_tmpB3D=Cyc_Tcutil_new_tvar((void*)((_tmp13E8=
_cycalloc(sizeof(*_tmp13E8)),((_tmp13E8[0]=((_tmp13E9.tag=2,((_tmp13E9.f1=0,((
_tmp13E9.f2=expected_kind,_tmp13E9)))))),_tmp13E8)))));{struct Cyc_Absyn_VarType_struct*
_tmp13EF;struct Cyc_Absyn_VarType_struct _tmp13EE;struct Cyc_Core_Opt*_tmp13ED;*
_tmpADD=((_tmp13ED=_cycalloc(sizeof(*_tmp13ED)),((_tmp13ED->v=(void*)((_tmp13EF=
_cycalloc(sizeof(*_tmp13EF)),((_tmp13EF[0]=((_tmp13EE.tag=2,((_tmp13EE.f1=
_tmpB3D,_tmp13EE)))),_tmp13EF)))),_tmp13ED))));}_tmpADF=_tmpB3D;goto _LL56B;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL565;_LL56A: {struct Cyc_Absyn_VarType_struct*_tmpADE=(
struct Cyc_Absyn_VarType_struct*)_tmpAD7;if(_tmpADE->tag != 2)goto _LL56C;else{
_tmpADF=_tmpADE->f1;}}_LL56B:{void*_tmpB43=Cyc_Absyn_compress_kb(_tmpADF->kind);
struct Cyc_Core_Opt*_tmpB45;struct Cyc_Core_Opt**_tmpB46;_LL59D: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB44=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB43;if(_tmpB44->tag != 1)goto
_LL59F;else{_tmpB45=_tmpB44->f1;_tmpB46=(struct Cyc_Core_Opt**)& _tmpB44->f1;}}
_LL59E:{struct Cyc_Absyn_Less_kb_struct*_tmp13F5;struct Cyc_Absyn_Less_kb_struct
_tmp13F4;struct Cyc_Core_Opt*_tmp13F3;*_tmpB46=((_tmp13F3=_cycalloc(sizeof(*
_tmp13F3)),((_tmp13F3->v=(void*)((_tmp13F5=_cycalloc(sizeof(*_tmp13F5)),((
_tmp13F5[0]=((_tmp13F4.tag=2,((_tmp13F4.f1=0,((_tmp13F4.f2=expected_kind,
_tmp13F4)))))),_tmp13F5)))),_tmp13F3))));}goto _LL59C;_LL59F:;_LL5A0: goto _LL59C;
_LL59C:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpADF);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpADF,put_in_effect);{void*_tmpB4A=Cyc_Absyn_compress_kb(_tmpADF->kind);struct
Cyc_Core_Opt*_tmpB4C;struct Cyc_Core_Opt**_tmpB4D;struct Cyc_Absyn_Kind*_tmpB4E;
_LL5A2: {struct Cyc_Absyn_Less_kb_struct*_tmpB4B=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB4A;if(_tmpB4B->tag != 2)goto _LL5A4;else{_tmpB4C=_tmpB4B->f1;_tmpB4D=(struct
Cyc_Core_Opt**)& _tmpB4B->f1;_tmpB4E=_tmpB4B->f2;}}_LL5A3: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB4E)){struct Cyc_Absyn_Less_kb_struct*_tmp13FB;struct Cyc_Absyn_Less_kb_struct
_tmp13FA;struct Cyc_Core_Opt*_tmp13F9;*_tmpB4D=((_tmp13F9=_cycalloc(sizeof(*
_tmp13F9)),((_tmp13F9->v=(void*)((_tmp13FB=_cycalloc(sizeof(*_tmp13FB)),((
_tmp13FB[0]=((_tmp13FA.tag=2,((_tmp13FA.f1=0,((_tmp13FA.f2=expected_kind,
_tmp13FA)))))),_tmp13FB)))),_tmp13F9))));}goto _LL5A1;_LL5A4:;_LL5A5: goto _LL5A1;
_LL5A1:;}goto _LL565;_LL56C: {struct Cyc_Absyn_TypeDeclType_struct*_tmpAE0=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpAD7;if(_tmpAE0->tag != 26)goto _LL56E;else{
_tmpAE1=_tmpAE0->f1;_tmpAE2=*_tmpAE1;_tmpAE3=_tmpAE2.r;_tmpAE4=_tmpAE0->f2;
_tmpAE5=(void***)& _tmpAE0->f2;}}_LL56D: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpB52=_tmpAE3;struct Cyc_Absyn_Aggrdecl*_tmpB54;struct Cyc_Absyn_Enumdecl*
_tmpB56;struct Cyc_Absyn_Datatypedecl*_tmpB58;_LL5A7: {struct Cyc_Absyn_Aggr_td_struct*
_tmpB53=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB52;if(_tmpB53->tag != 0)goto _LL5A9;
else{_tmpB54=_tmpB53->f1;}}_LL5A8: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB54);goto _LL5A6;_LL5A9: {struct Cyc_Absyn_Enum_td_struct*_tmpB55=(struct Cyc_Absyn_Enum_td_struct*)
_tmpB52;if(_tmpB55->tag != 1)goto _LL5AB;else{_tmpB56=_tmpB55->f1;}}_LL5AA: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB56);goto _LL5A6;_LL5AB: {struct Cyc_Absyn_Datatype_td_struct*
_tmpB57=(struct Cyc_Absyn_Datatype_td_struct*)_tmpB52;if(_tmpB57->tag != 2)goto
_LL5A6;else{_tmpB58=_tmpB57->f1;}}_LL5AC: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB58);goto _LL5A6;_LL5A6:;}{void**_tmp13FC;*_tmpAE5=((_tmp13FC=_cycalloc(
sizeof(*_tmp13FC)),((_tmp13FC[0]=new_t,_tmp13FC))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL56E: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpAE6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpAD7;if(_tmpAE6->tag != 15)goto
_LL570;else{_tmpAE7=_tmpAE6->f1;}}_LL56F: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB5A=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpAE7 != 0;_tmpAE7=_tmpAE7->tl){
struct Cyc_Absyn_Enumfield*_tmpB5B=(struct Cyc_Absyn_Enumfield*)_tmpAE7->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB5B->name).f2)){
const char*_tmp1400;void*_tmp13FF[1];struct Cyc_String_pa_struct _tmp13FE;(_tmp13FE.tag=
0,((_tmp13FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB5B->name).f2),((
_tmp13FF[0]=& _tmp13FE,Cyc_Tcutil_terr(_tmpB5B->loc,((_tmp1400="duplicate enum field name %s",
_tag_dyneither(_tmp1400,sizeof(char),29))),_tag_dyneither(_tmp13FF,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1401;prev_fields=((_tmp1401=
_region_malloc(_tmpB5A,sizeof(*_tmp1401)),((_tmp1401->hd=(*_tmpB5B->name).f2,((
_tmp1401->tl=prev_fields,_tmp1401))))));}if(_tmpB5B->tag == 0)_tmpB5B->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB5B->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB5B->tag))){const char*_tmp1405;void*
_tmp1404[1];struct Cyc_String_pa_struct _tmp1403;(_tmp1403.tag=0,((_tmp1403.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB5B->name).f2),((_tmp1404[0]=&
_tmp1403,Cyc_Tcutil_terr(loc,((_tmp1405="enum field %s: expression is not constant",
_tag_dyneither(_tmp1405,sizeof(char),42))),_tag_dyneither(_tmp1404,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB5B->tag))).f1;tag_count=t1 + 1;(*_tmpB5B->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp140B;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp140A;struct _tuple28*_tmp1409;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple28*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB5B->name).f2,(struct _tuple28*)((_tmp1409=_cycalloc(sizeof(*_tmp1409)),((
_tmp1409->f1=(void*)((_tmp140B=_cycalloc(sizeof(*_tmp140B)),((_tmp140B[0]=((
_tmp140A.tag=4,((_tmp140A.f1=(void*)t,((_tmp140A.f2=_tmpB5B,_tmp140A)))))),
_tmp140B)))),((_tmp1409->f2=1,_tmp1409)))))));};};}}goto _LL565;}_LL570: {struct
Cyc_Absyn_EnumType_struct*_tmpAE8=(struct Cyc_Absyn_EnumType_struct*)_tmpAD7;if(
_tmpAE8->tag != 14)goto _LL572;else{_tmpAE9=_tmpAE8->f1;_tmpAEA=_tmpAE8->f2;
_tmpAEB=(struct Cyc_Absyn_Enumdecl**)& _tmpAE8->f2;}}_LL571: if(*_tmpAEB == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpAEB))->fields == 0){struct
_handler_cons _tmpB66;_push_handler(& _tmpB66);{int _tmpB68=0;if(setjmp(_tmpB66.handler))
_tmpB68=1;if(!_tmpB68){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpAE9);*_tmpAEB=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpB67=(void*)_exn_thrown;void*_tmpB6A=_tmpB67;_LL5AE: {struct Cyc_Dict_Absent_struct*
_tmpB6B=(struct Cyc_Dict_Absent_struct*)_tmpB6A;if(_tmpB6B->tag != Cyc_Dict_Absent)
goto _LL5B0;}_LL5AF: {struct Cyc_Tcenv_Genv*_tmpB6C=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp140C;struct Cyc_Absyn_Enumdecl*_tmpB6D=(_tmp140C=
_cycalloc(sizeof(*_tmp140C)),((_tmp140C->sc=Cyc_Absyn_Extern,((_tmp140C->name=
_tmpAE9,((_tmp140C->fields=0,_tmp140C)))))));Cyc_Tc_tcEnumdecl(te,_tmpB6C,loc,
_tmpB6D);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpAE9);*
_tmpAEB=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5AD;};}_LL5B0:;_LL5B1:(void)_throw(
_tmpB6A);_LL5AD:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpAEB);*_tmpAE9=(ed->name)[0];goto _LL565;};_LL572: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAEC=(struct Cyc_Absyn_DatatypeType_struct*)_tmpAD7;if(_tmpAEC->tag != 3)goto
_LL574;else{_tmpAED=_tmpAEC->f1;_tmpAEE=_tmpAED.datatype_info;_tmpAEF=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpAEC->f1).datatype_info;_tmpAF0=_tmpAED.targs;_tmpAF1=(struct Cyc_List_List**)&(
_tmpAEC->f1).targs;}}_LL573: {struct Cyc_List_List*_tmpB6F=*_tmpAF1;{union Cyc_Absyn_DatatypeInfoU
_tmpB70=*_tmpAEF;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB71;struct _tuple2*
_tmpB72;int _tmpB73;struct Cyc_Absyn_Datatypedecl**_tmpB74;struct Cyc_Absyn_Datatypedecl*
_tmpB75;_LL5B3: if((_tmpB70.UnknownDatatype).tag != 1)goto _LL5B5;_tmpB71=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpB70.UnknownDatatype).val;_tmpB72=_tmpB71.name;
_tmpB73=_tmpB71.is_extensible;_LL5B4: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpB76;_push_handler(& _tmpB76);{int _tmpB78=0;if(setjmp(
_tmpB76.handler))_tmpB78=1;if(!_tmpB78){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpB72);;_pop_handler();}else{void*_tmpB77=(void*)_exn_thrown;void*_tmpB7A=
_tmpB77;_LL5B8: {struct Cyc_Dict_Absent_struct*_tmpB7B=(struct Cyc_Dict_Absent_struct*)
_tmpB7A;if(_tmpB7B->tag != Cyc_Dict_Absent)goto _LL5BA;}_LL5B9: {struct Cyc_Tcenv_Genv*
_tmpB7C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp140D;struct Cyc_Absyn_Datatypedecl*
_tmpB7D=(_tmp140D=_cycalloc(sizeof(*_tmp140D)),((_tmp140D->sc=Cyc_Absyn_Extern,((
_tmp140D->name=_tmpB72,((_tmp140D->tvs=0,((_tmp140D->fields=0,((_tmp140D->is_extensible=
_tmpB73,_tmp140D)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpB7C,loc,_tmpB7D);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpB72);if(_tmpB6F != 0){{const char*
_tmp1411;void*_tmp1410[1];struct Cyc_String_pa_struct _tmp140F;(_tmp140F.tag=0,((
_tmp140F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB72)),((_tmp1410[0]=& _tmp140F,Cyc_Tcutil_terr(loc,((_tmp1411="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1411,sizeof(char),47))),_tag_dyneither(_tmp1410,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5B7;}_LL5BA:;_LL5BB:(void)_throw(_tmpB7A);_LL5B7:;}};}
if(_tmpB73  && !(*tudp)->is_extensible){const char*_tmp1415;void*_tmp1414[1];
struct Cyc_String_pa_struct _tmp1413;(_tmp1413.tag=0,((_tmp1413.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB72)),((
_tmp1414[0]=& _tmp1413,Cyc_Tcutil_terr(loc,((_tmp1415="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1415,sizeof(char),41))),_tag_dyneither(_tmp1414,sizeof(void*),
1)))))));}*_tmpAEF=Cyc_Absyn_KnownDatatype(tudp);_tmpB75=*tudp;goto _LL5B6;}
_LL5B5: if((_tmpB70.KnownDatatype).tag != 2)goto _LL5B2;_tmpB74=(struct Cyc_Absyn_Datatypedecl**)(
_tmpB70.KnownDatatype).val;_tmpB75=*_tmpB74;_LL5B6: {struct Cyc_List_List*tvs=
_tmpB75->tvs;for(0;_tmpB6F != 0  && tvs != 0;(_tmpB6F=_tmpB6F->tl,tvs=tvs->tl)){
void*t=(void*)_tmpB6F->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1416;struct _tuple0 _tmpB86=(_tmp1416.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1416.f2=t,_tmp1416)));void*_tmpB87;void*_tmpB89;struct Cyc_Absyn_Tvar*_tmpB8B;
_LL5BD: _tmpB87=_tmpB86.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB88=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB87;if(_tmpB88->tag != 1)goto _LL5BF;};_tmpB89=_tmpB86.f2;{struct Cyc_Absyn_VarType_struct*
_tmpB8A=(struct Cyc_Absyn_VarType_struct*)_tmpB89;if(_tmpB8A->tag != 2)goto _LL5BF;
else{_tmpB8B=_tmpB8A->f1;}};_LL5BE: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpB8B);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB8B,1);continue;_LL5BF:;_LL5C0: goto _LL5BC;_LL5BC:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpB6F != 0){const char*_tmp141A;void*_tmp1419[1];
struct Cyc_String_pa_struct _tmp1418;(_tmp1418.tag=0,((_tmp1418.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB75->name)),((
_tmp1419[0]=& _tmp1418,Cyc_Tcutil_terr(loc,((_tmp141A="too many type arguments for datatype %s",
_tag_dyneither(_tmp141A,sizeof(char),40))),_tag_dyneither(_tmp1419,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp141B;hidden_ts=((_tmp141B=_cycalloc(sizeof(*_tmp141B)),((_tmp141B->hd=e,((
_tmp141B->tl=hidden_ts,_tmp141B))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpAF1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpAF1,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5B2;}_LL5B2:;}goto
_LL565;}_LL574: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpAF2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpAD7;if(_tmpAF2->tag != 4)goto _LL576;else{_tmpAF3=_tmpAF2->f1;_tmpAF4=_tmpAF3.field_info;
_tmpAF5=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpAF2->f1).field_info;_tmpAF6=
_tmpAF3.targs;}}_LL575:{union Cyc_Absyn_DatatypeFieldInfoU _tmpB90=*_tmpAF5;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB91;struct _tuple2*_tmpB92;struct _tuple2*
_tmpB93;int _tmpB94;struct _tuple3 _tmpB95;struct Cyc_Absyn_Datatypedecl*_tmpB96;
struct Cyc_Absyn_Datatypefield*_tmpB97;_LL5C2: if((_tmpB90.UnknownDatatypefield).tag
!= 1)goto _LL5C4;_tmpB91=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpB90.UnknownDatatypefield).val;
_tmpB92=_tmpB91.datatype_name;_tmpB93=_tmpB91.field_name;_tmpB94=_tmpB91.is_extensible;
_LL5C3: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpB98;_push_handler(& _tmpB98);{int _tmpB9A=0;if(setjmp(
_tmpB98.handler))_tmpB9A=1;if(!_tmpB9A){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpB92);;_pop_handler();}else{void*_tmpB99=(void*)_exn_thrown;void*_tmpB9C=
_tmpB99;_LL5C7: {struct Cyc_Dict_Absent_struct*_tmpB9D=(struct Cyc_Dict_Absent_struct*)
_tmpB9C;if(_tmpB9D->tag != Cyc_Dict_Absent)goto _LL5C9;}_LL5C8:{const char*_tmp141F;
void*_tmp141E[1];struct Cyc_String_pa_struct _tmp141D;(_tmp141D.tag=0,((_tmp141D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB92)),((
_tmp141E[0]=& _tmp141D,Cyc_Tcutil_terr(loc,((_tmp141F="unbound datatype %s",
_tag_dyneither(_tmp141F,sizeof(char),20))),_tag_dyneither(_tmp141E,sizeof(void*),
1)))))));}return cvtenv;_LL5C9:;_LL5CA:(void)_throw(_tmpB9C);_LL5C6:;}};}{struct
_handler_cons _tmpBA1;_push_handler(& _tmpBA1);{int _tmpBA3=0;if(setjmp(_tmpBA1.handler))
_tmpBA3=1;if(!_tmpBA3){{struct _RegionHandle*_tmpBA4=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBA5=Cyc_Tcenv_lookup_ordinary(_tmpBA4,te,loc,_tmpB93);struct Cyc_Absyn_Datatypedecl*
_tmpBA7;struct Cyc_Absyn_Datatypefield*_tmpBA8;_LL5CC: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBA6=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBA5;if(_tmpBA6->tag != 2)goto
_LL5CE;else{_tmpBA7=_tmpBA6->f1;_tmpBA8=_tmpBA6->f2;}}_LL5CD: tuf=_tmpBA8;tud=
_tmpBA7;if(_tmpB94  && !tud->is_extensible){const char*_tmp1423;void*_tmp1422[1];
struct Cyc_String_pa_struct _tmp1421;(_tmp1421.tag=0,((_tmp1421.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB92)),((
_tmp1422[0]=& _tmp1421,Cyc_Tcutil_terr(loc,((_tmp1423="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1423,sizeof(char),41))),_tag_dyneither(_tmp1422,sizeof(void*),
1)))))));}goto _LL5CB;_LL5CE:;_LL5CF:{const char*_tmp1428;void*_tmp1427[2];struct
Cyc_String_pa_struct _tmp1426;struct Cyc_String_pa_struct _tmp1425;(_tmp1425.tag=0,((
_tmp1425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB92)),((_tmp1426.tag=0,((_tmp1426.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB93)),((_tmp1427[0]=& _tmp1426,((
_tmp1427[1]=& _tmp1425,Cyc_Tcutil_terr(loc,((_tmp1428="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1428,sizeof(char),37))),_tag_dyneither(_tmp1427,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBB0=cvtenv;_npop_handler(0);return
_tmpBB0;};_LL5CB:;};_pop_handler();}else{void*_tmpBA2=(void*)_exn_thrown;void*
_tmpBB2=_tmpBA2;_LL5D1: {struct Cyc_Dict_Absent_struct*_tmpBB3=(struct Cyc_Dict_Absent_struct*)
_tmpBB2;if(_tmpBB3->tag != Cyc_Dict_Absent)goto _LL5D3;}_LL5D2:{const char*_tmp142D;
void*_tmp142C[2];struct Cyc_String_pa_struct _tmp142B;struct Cyc_String_pa_struct
_tmp142A;(_tmp142A.tag=0,((_tmp142A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB92)),((_tmp142B.tag=0,((_tmp142B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB93)),((
_tmp142C[0]=& _tmp142B,((_tmp142C[1]=& _tmp142A,Cyc_Tcutil_terr(loc,((_tmp142D="unbound field %s in type datatype %s",
_tag_dyneither(_tmp142D,sizeof(char),37))),_tag_dyneither(_tmp142C,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5D3:;_LL5D4:(void)_throw(_tmpBB2);_LL5D0:;}};}*
_tmpAF5=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpB96=tud;_tmpB97=tuf;goto _LL5C5;}
_LL5C4: if((_tmpB90.KnownDatatypefield).tag != 2)goto _LL5C1;_tmpB95=(struct _tuple3)(
_tmpB90.KnownDatatypefield).val;_tmpB96=_tmpB95.f1;_tmpB97=_tmpB95.f2;_LL5C5: {
struct Cyc_List_List*tvs=_tmpB96->tvs;for(0;_tmpAF6 != 0  && tvs != 0;(_tmpAF6=
_tmpAF6->tl,tvs=tvs->tl)){void*t=(void*)_tmpAF6->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp142E;struct _tuple0 _tmpBB9=(
_tmp142E.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp142E.f2=t,_tmp142E)));void*
_tmpBBA;void*_tmpBBC;struct Cyc_Absyn_Tvar*_tmpBBE;_LL5D6: _tmpBBA=_tmpBB9.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpBBB=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBBA;if(_tmpBBB->tag != 1)goto _LL5D8;};_tmpBBC=_tmpBB9.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBBD=(struct Cyc_Absyn_VarType_struct*)_tmpBBC;if(_tmpBBD->tag != 2)goto _LL5D8;
else{_tmpBBE=_tmpBBD->f1;}};_LL5D7: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBBE);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBBE,1);continue;_LL5D8:;_LL5D9: goto _LL5D5;_LL5D5:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmpAF6 != 0){const char*_tmp1433;void*_tmp1432[2];
struct Cyc_String_pa_struct _tmp1431;struct Cyc_String_pa_struct _tmp1430;(_tmp1430.tag=
0,((_tmp1430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB97->name)),((_tmp1431.tag=0,((_tmp1431.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB96->name)),((_tmp1432[0]=& _tmp1431,((
_tmp1432[1]=& _tmp1430,Cyc_Tcutil_terr(loc,((_tmp1433="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp1433,sizeof(char),43))),_tag_dyneither(_tmp1432,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp1438;void*_tmp1437[2];struct Cyc_String_pa_struct
_tmp1436;struct Cyc_String_pa_struct _tmp1435;(_tmp1435.tag=0,((_tmp1435.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB97->name)),((
_tmp1436.tag=0,((_tmp1436.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB96->name)),((_tmp1437[0]=& _tmp1436,((_tmp1437[1]=& _tmp1435,Cyc_Tcutil_terr(
loc,((_tmp1438="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp1438,sizeof(char),42))),_tag_dyneither(_tmp1437,sizeof(void*),2)))))))))))));}
goto _LL5C1;}_LL5C1:;}goto _LL565;_LL576: {struct Cyc_Absyn_PointerType_struct*
_tmpAF7=(struct Cyc_Absyn_PointerType_struct*)_tmpAD7;if(_tmpAF7->tag != 5)goto
_LL578;else{_tmpAF8=_tmpAF7->f1;_tmpAF9=_tmpAF8.elt_typ;_tmpAFA=_tmpAF8.elt_tq;
_tmpAFB=(struct Cyc_Absyn_Tqual*)&(_tmpAF7->f1).elt_tq;_tmpAFC=_tmpAF8.ptr_atts;
_tmpAFD=_tmpAFC.rgn;_tmpAFE=_tmpAFC.nullable;_tmpAFF=_tmpAFC.bounds;_tmpB00=
_tmpAFC.zero_term;}}_LL577: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpAF9,1);_tmpAFB->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpAFB->print_const,_tmpAF9);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5DA: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5DB: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5DC: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpAFD,1);{union Cyc_Absyn_Constraint*
_tmpBC7=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB00);union Cyc_Absyn_Constraint _tmpBC8;int _tmpBC9;union Cyc_Absyn_Constraint
_tmpBCA;int _tmpBCB;_LL5DF: _tmpBC8=*_tmpBC7;if((_tmpBC8.No_constr).tag != 3)goto
_LL5E1;_tmpBC9=(int)(_tmpBC8.No_constr).val;_LL5E0:{void*_tmpBCC=Cyc_Tcutil_compress(
_tmpAF9);enum Cyc_Absyn_Size_of _tmpBCE;_LL5E6: {struct Cyc_Absyn_IntType_struct*
_tmpBCD=(struct Cyc_Absyn_IntType_struct*)_tmpBCC;if(_tmpBCD->tag != 6)goto _LL5E8;
else{_tmpBCE=_tmpBCD->f2;if(_tmpBCE != Cyc_Absyn_Char_sz)goto _LL5E8;}}_LL5E7:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB00,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5E5;_LL5E8:;_LL5E9:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB00,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5E5;
_LL5E5:;}goto _LL5DE;_LL5E1: _tmpBCA=*_tmpBC7;if((_tmpBCA.Eq_constr).tag != 1)goto
_LL5E3;_tmpBCB=(int)(_tmpBCA.Eq_constr).val;if(_tmpBCB != 1)goto _LL5E3;_LL5E2: if(
!Cyc_Tcutil_admits_zero(_tmpAF9)){const char*_tmp143C;void*_tmp143B[1];struct Cyc_String_pa_struct
_tmp143A;(_tmp143A.tag=0,((_tmp143A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAF9)),((_tmp143B[0]=& _tmp143A,Cyc_Tcutil_terr(
loc,((_tmp143C="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp143C,sizeof(char),53))),_tag_dyneither(_tmp143B,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5DE;_LL5E3:;_LL5E4: is_zero_terminated=0;
goto _LL5DE;_LL5DE:;}{void*_tmpBD2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpAFF);struct Cyc_Absyn_Exp*
_tmpBD5;_LL5EB: {struct Cyc_Absyn_DynEither_b_struct*_tmpBD3=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpBD2;if(_tmpBD3->tag != 0)goto _LL5ED;}_LL5EC: goto _LL5EA;_LL5ED: {struct Cyc_Absyn_Upper_b_struct*
_tmpBD4=(struct Cyc_Absyn_Upper_b_struct*)_tmpBD2;if(_tmpBD4->tag != 1)goto _LL5EA;
else{_tmpBD5=_tmpBD4->f1;}}_LL5EE: {struct _RegionHandle*_tmpBD6=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpBD7=Cyc_Tcenv_allow_valueof(_tmpBD6,te);Cyc_Tcexp_tcExp(
_tmpBD7,0,_tmpBD5);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBD5,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpBD5)){const char*_tmp143F;void*
_tmp143E;(_tmp143E=0,Cyc_Tcutil_terr(loc,((_tmp143F="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp143F,sizeof(char),49))),_tag_dyneither(_tmp143E,sizeof(void*),
0)));}{unsigned int _tmpBDB;int _tmpBDC;struct _tuple13 _tmpBDA=Cyc_Evexp_eval_const_uint_exp(
_tmpBD5);_tmpBDB=_tmpBDA.f1;_tmpBDC=_tmpBDA.f2;if(is_zero_terminated  && (!
_tmpBDC  || _tmpBDB < 1)){const char*_tmp1442;void*_tmp1441;(_tmp1441=0,Cyc_Tcutil_terr(
loc,((_tmp1442="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp1442,sizeof(char),55))),_tag_dyneither(_tmp1441,sizeof(void*),
0)));}goto _LL5EA;};}_LL5EA:;}goto _LL565;};}_LL578: {struct Cyc_Absyn_TagType_struct*
_tmpB01=(struct Cyc_Absyn_TagType_struct*)_tmpAD7;if(_tmpB01->tag != 20)goto _LL57A;
else{_tmpB02=(void*)_tmpB01->f1;}}_LL579: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB02,1);goto _LL565;_LL57A: {struct Cyc_Absyn_ValueofType_struct*
_tmpB03=(struct Cyc_Absyn_ValueofType_struct*)_tmpAD7;if(_tmpB03->tag != 19)goto
_LL57C;else{_tmpB04=_tmpB03->f1;}}_LL57B: {struct _RegionHandle*_tmpBDF=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpBE0=Cyc_Tcenv_allow_valueof(_tmpBDF,te);Cyc_Tcexp_tcExp(
_tmpBE0,0,_tmpB04);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB04)){const char*
_tmp1445;void*_tmp1444;(_tmp1444=0,Cyc_Tcutil_terr(loc,((_tmp1445="valueof_t requires an int expression",
_tag_dyneither(_tmp1445,sizeof(char),37))),_tag_dyneither(_tmp1444,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB04,te,cvtenv);goto
_LL565;}_LL57C: {struct Cyc_Absyn_IntType_struct*_tmpB05=(struct Cyc_Absyn_IntType_struct*)
_tmpAD7;if(_tmpB05->tag != 6)goto _LL57E;}_LL57D: goto _LL57F;_LL57E: {struct Cyc_Absyn_FloatType_struct*
_tmpB06=(struct Cyc_Absyn_FloatType_struct*)_tmpAD7;if(_tmpB06->tag != 7)goto
_LL580;}_LL57F: goto _LL581;_LL580: {struct Cyc_Absyn_DoubleType_struct*_tmpB07=(
struct Cyc_Absyn_DoubleType_struct*)_tmpAD7;if(_tmpB07->tag != 8)goto _LL582;}
_LL581: goto _LL565;_LL582: {struct Cyc_Absyn_ArrayType_struct*_tmpB08=(struct Cyc_Absyn_ArrayType_struct*)
_tmpAD7;if(_tmpB08->tag != 9)goto _LL584;else{_tmpB09=_tmpB08->f1;_tmpB0A=_tmpB09.elt_type;
_tmpB0B=_tmpB09.tq;_tmpB0C=(struct Cyc_Absyn_Tqual*)&(_tmpB08->f1).tq;_tmpB0D=
_tmpB09.num_elts;_tmpB0E=(struct Cyc_Absyn_Exp**)&(_tmpB08->f1).num_elts;_tmpB0F=
_tmpB09.zero_term;_tmpB10=_tmpB09.zt_loc;}}_LL583: {struct Cyc_Absyn_Exp*_tmpBE3=*
_tmpB0E;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpB0A,1);_tmpB0C->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB0C->print_const,
_tmpB0A);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpBE4=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB0F);union Cyc_Absyn_Constraint
_tmpBE5;int _tmpBE6;union Cyc_Absyn_Constraint _tmpBE7;int _tmpBE8;_LL5F0: _tmpBE5=*
_tmpBE4;if((_tmpBE5.No_constr).tag != 3)goto _LL5F2;_tmpBE6=(int)(_tmpBE5.No_constr).val;
_LL5F1:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB0F,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5EF;_LL5F2: _tmpBE7=*_tmpBE4;if((_tmpBE7.Eq_constr).tag
!= 1)goto _LL5F4;_tmpBE8=(int)(_tmpBE7.Eq_constr).val;if(_tmpBE8 != 1)goto _LL5F4;
_LL5F3: if(!Cyc_Tcutil_admits_zero(_tmpB0A)){const char*_tmp1449;void*_tmp1448[1];
struct Cyc_String_pa_struct _tmp1447;(_tmp1447.tag=0,((_tmp1447.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB0A)),((
_tmp1448[0]=& _tmp1447,Cyc_Tcutil_terr(loc,((_tmp1449="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp1449,sizeof(char),51))),_tag_dyneither(_tmp1448,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5EF;_LL5F4:;_LL5F5: is_zero_terminated=0;
goto _LL5EF;_LL5EF:;}if(_tmpBE3 == 0){if(is_zero_terminated)*_tmpB0E=(_tmpBE3=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp144C;void*
_tmp144B;(_tmp144B=0,Cyc_Tcutil_warn(loc,((_tmp144C="array bound defaults to 1 here",
_tag_dyneither(_tmp144C,sizeof(char),31))),_tag_dyneither(_tmp144B,sizeof(void*),
0)));}*_tmpB0E=(_tmpBE3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpBE3);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpBE3)){const char*_tmp144F;void*_tmp144E;(_tmp144E=0,Cyc_Tcutil_terr(loc,((
_tmp144F="array bounds expression is not constant",_tag_dyneither(_tmp144F,
sizeof(char),40))),_tag_dyneither(_tmp144E,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpBE3)){const char*_tmp1452;void*_tmp1451;(_tmp1451=0,
Cyc_Tcutil_terr(loc,((_tmp1452="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp1452,sizeof(char),47))),_tag_dyneither(_tmp1451,sizeof(void*),
0)));}{unsigned int _tmpBF3;int _tmpBF4;struct _tuple13 _tmpBF2=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpBE3);_tmpBF3=_tmpBF2.f1;_tmpBF4=_tmpBF2.f2;if((
is_zero_terminated  && _tmpBF4) && _tmpBF3 < 1){const char*_tmp1455;void*_tmp1454;(
_tmp1454=0,Cyc_Tcutil_warn(loc,((_tmp1455="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp1455,sizeof(char),48))),_tag_dyneither(_tmp1454,sizeof(void*),
0)));}if((_tmpBF4  && _tmpBF3 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp1458;void*
_tmp1457;(_tmp1457=0,Cyc_Tcutil_warn(loc,((_tmp1458="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp1458,sizeof(char),75))),_tag_dyneither(_tmp1457,sizeof(void*),
0)));}*_tmpB0E=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL565;};};}
_LL584: {struct Cyc_Absyn_FnType_struct*_tmpB11=(struct Cyc_Absyn_FnType_struct*)
_tmpAD7;if(_tmpB11->tag != 10)goto _LL586;else{_tmpB12=_tmpB11->f1;_tmpB13=_tmpB12.tvars;
_tmpB14=(struct Cyc_List_List**)&(_tmpB11->f1).tvars;_tmpB15=_tmpB12.effect;
_tmpB16=(struct Cyc_Core_Opt**)&(_tmpB11->f1).effect;_tmpB17=_tmpB12.ret_typ;
_tmpB18=_tmpB12.args;_tmpB19=_tmpB12.c_varargs;_tmpB1A=_tmpB12.cyc_varargs;
_tmpB1B=_tmpB12.rgn_po;_tmpB1C=_tmpB12.attributes;}}_LL585: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB1C != 0;
_tmpB1C=_tmpB1C->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB1C->hd)){const char*
_tmp145C;void*_tmp145B[1];struct Cyc_String_pa_struct _tmp145A;(_tmp145A.tag=0,((
_tmp145A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB1C->hd)),((_tmp145B[0]=& _tmp145A,Cyc_Tcutil_terr(loc,((_tmp145C="bad function type attribute %s",
_tag_dyneither(_tmp145C,sizeof(char),31))),_tag_dyneither(_tmp145B,sizeof(void*),
1)))))));}{void*_tmpBFC=(void*)_tmpB1C->hd;enum Cyc_Absyn_Format_Type _tmpC01;int
_tmpC02;int _tmpC03;_LL5F7: {struct Cyc_Absyn_Stdcall_att_struct*_tmpBFD=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpBFC;if(_tmpBFD->tag != 1)goto _LL5F9;}_LL5F8: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5F6;_LL5F9: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpBFE=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpBFC;if(_tmpBFE->tag != 2)goto
_LL5FB;}_LL5FA: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5F6;_LL5FB: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpBFF=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpBFC;if(_tmpBFF->tag != 3)goto _LL5FD;}_LL5FC: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5F6;_LL5FD: {struct Cyc_Absyn_Format_att_struct*_tmpC00=(
struct Cyc_Absyn_Format_att_struct*)_tmpBFC;if(_tmpC00->tag != 19)goto _LL5FF;else{
_tmpC01=_tmpC00->f1;_tmpC02=_tmpC00->f2;_tmpC03=_tmpC00->f3;}}_LL5FE: if(!
seen_format){seen_format=1;ft=_tmpC01;fmt_desc_arg=_tmpC02;fmt_arg_start=_tmpC03;}
else{const char*_tmp145F;void*_tmp145E;(_tmp145E=0,Cyc_Tcutil_terr(loc,((_tmp145F="function can't have multiple format attributes",
_tag_dyneither(_tmp145F,sizeof(char),47))),_tag_dyneither(_tmp145E,sizeof(void*),
0)));}goto _LL5F6;_LL5FF:;_LL600: goto _LL5F6;_LL5F6:;};}if(num_convs > 1){const char*
_tmp1462;void*_tmp1461;(_tmp1461=0,Cyc_Tcutil_terr(loc,((_tmp1462="function can't have multiple calling conventions",
_tag_dyneither(_tmp1462,sizeof(char),49))),_tag_dyneither(_tmp1461,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB14);{struct Cyc_List_List*b=*
_tmpB14;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC08=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC0A;struct Cyc_Absyn_Kind _tmpC0B;enum Cyc_Absyn_KindQual
_tmpC0C;_LL602: {struct Cyc_Absyn_Eq_kb_struct*_tmpC09=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC08;if(_tmpC09->tag != 0)goto _LL604;else{_tmpC0A=_tmpC09->f1;_tmpC0B=*_tmpC0A;
_tmpC0C=_tmpC0B.kind;if(_tmpC0C != Cyc_Absyn_MemKind)goto _LL604;}}_LL603:{const
char*_tmp1466;void*_tmp1465[1];struct Cyc_String_pa_struct _tmp1464;(_tmp1464.tag=
0,((_tmp1464.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp1465[0]=& _tmp1464,Cyc_Tcutil_terr(loc,((_tmp1466="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1466,sizeof(char),51))),_tag_dyneither(_tmp1465,sizeof(void*),
1)))))));}goto _LL601;_LL604:;_LL605: goto _LL601;_LL601:;};}}{struct _RegionHandle*
_tmpC10=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1467;struct Cyc_Tcutil_CVTEnv
_tmpC11=(_tmp1467.r=_tmpC10,((_tmp1467.kind_env=cvtenv.kind_env,((_tmp1467.free_vars=
0,((_tmp1467.free_evars=0,((_tmp1467.generalize_evars=cvtenv.generalize_evars,((
_tmp1467.fn_result=1,_tmp1467)))))))))));_tmpC11=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC11,& Cyc_Tcutil_tmk,_tmpB17,1);_tmpC11.fn_result=0;{struct Cyc_List_List*
a=_tmpB18;for(0;a != 0;a=a->tl){struct _tuple9*_tmpC12=(struct _tuple9*)a->hd;void*
_tmpC13=(*_tmpC12).f3;_tmpC11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC11,& Cyc_Tcutil_tmk,
_tmpC13,1);((*_tmpC12).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpC12).f2).print_const,_tmpC13);}}if(_tmpB1A != 0){if(_tmpB19){const char*
_tmp146A;void*_tmp1469;(_tmp1469=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp146A="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp146A,sizeof(char),29))),_tag_dyneither(_tmp1469,sizeof(void*),
0)));}{void*_tmpC17;int _tmpC18;struct Cyc_Absyn_VarargInfo _tmpC16=*_tmpB1A;
_tmpC17=_tmpC16.type;_tmpC18=_tmpC16.inject;_tmpC11=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC11,& Cyc_Tcutil_tmk,_tmpC17,1);(_tmpB1A->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpB1A->tq).print_const,_tmpC17);if(_tmpC18){void*_tmpC19=Cyc_Tcutil_compress(
_tmpC17);struct Cyc_Absyn_PtrInfo _tmpC1B;void*_tmpC1C;struct Cyc_Absyn_PtrAtts
_tmpC1D;union Cyc_Absyn_Constraint*_tmpC1E;union Cyc_Absyn_Constraint*_tmpC1F;
_LL607: {struct Cyc_Absyn_PointerType_struct*_tmpC1A=(struct Cyc_Absyn_PointerType_struct*)
_tmpC19;if(_tmpC1A->tag != 5)goto _LL609;else{_tmpC1B=_tmpC1A->f1;_tmpC1C=_tmpC1B.elt_typ;
_tmpC1D=_tmpC1B.ptr_atts;_tmpC1E=_tmpC1D.bounds;_tmpC1F=_tmpC1D.zero_term;}}
_LL608:{void*_tmpC20=Cyc_Tcutil_compress(_tmpC1C);_LL60C: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC21=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC20;if(_tmpC21->tag != 3)goto
_LL60E;}_LL60D: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC1F)){const char*_tmp146D;void*_tmp146C;(_tmp146C=0,Cyc_Tcutil_terr(loc,((
_tmp146D="can't inject into a zeroterm pointer",_tag_dyneither(_tmp146D,sizeof(
char),37))),_tag_dyneither(_tmp146C,sizeof(void*),0)));}{void*_tmpC24=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC1E);_LL611: {struct Cyc_Absyn_DynEither_b_struct*_tmpC25=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC24;if(_tmpC25->tag != 0)goto _LL613;}_LL612:{const char*_tmp1470;void*_tmp146F;(
_tmp146F=0,Cyc_Tcutil_terr(loc,((_tmp1470="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp1470,sizeof(char),44))),_tag_dyneither(_tmp146F,sizeof(void*),
0)));}goto _LL610;_LL613:;_LL614: goto _LL610;_LL610:;}goto _LL60B;_LL60E:;_LL60F:{
const char*_tmp1473;void*_tmp1472;(_tmp1472=0,Cyc_Tcutil_terr(loc,((_tmp1473="can't inject a non-datatype type",
_tag_dyneither(_tmp1473,sizeof(char),33))),_tag_dyneither(_tmp1472,sizeof(void*),
0)));}goto _LL60B;_LL60B:;}goto _LL606;_LL609:;_LL60A:{const char*_tmp1476;void*
_tmp1475;(_tmp1475=0,Cyc_Tcutil_terr(loc,((_tmp1476="expecting a datatype pointer type",
_tag_dyneither(_tmp1476,sizeof(char),34))),_tag_dyneither(_tmp1475,sizeof(void*),
0)));}goto _LL606;_LL606:;}};}if(seen_format){int _tmpC2C=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB18);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC2C) || 
fmt_arg_start < 0) || _tmpB1A == 0  && fmt_arg_start != 0) || _tmpB1A != 0  && 
fmt_arg_start != _tmpC2C + 1){const char*_tmp1479;void*_tmp1478;(_tmp1478=0,Cyc_Tcutil_terr(
loc,((_tmp1479="bad format descriptor",_tag_dyneither(_tmp1479,sizeof(char),22))),
_tag_dyneither(_tmp1478,sizeof(void*),0)));}else{void*_tmpC30;struct _tuple9
_tmpC2F=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB18,
fmt_desc_arg - 1);_tmpC30=_tmpC2F.f3;{void*_tmpC31=Cyc_Tcutil_compress(_tmpC30);
struct Cyc_Absyn_PtrInfo _tmpC33;void*_tmpC34;struct Cyc_Absyn_PtrAtts _tmpC35;union
Cyc_Absyn_Constraint*_tmpC36;union Cyc_Absyn_Constraint*_tmpC37;_LL616: {struct
Cyc_Absyn_PointerType_struct*_tmpC32=(struct Cyc_Absyn_PointerType_struct*)
_tmpC31;if(_tmpC32->tag != 5)goto _LL618;else{_tmpC33=_tmpC32->f1;_tmpC34=_tmpC33.elt_typ;
_tmpC35=_tmpC33.ptr_atts;_tmpC36=_tmpC35.bounds;_tmpC37=_tmpC35.zero_term;}}
_LL617:{struct _tuple0 _tmp147A;struct _tuple0 _tmpC39=(_tmp147A.f1=Cyc_Tcutil_compress(
_tmpC34),((_tmp147A.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC36),_tmp147A)));void*_tmpC3A;enum Cyc_Absyn_Sign
_tmpC3C;enum Cyc_Absyn_Size_of _tmpC3D;void*_tmpC3E;_LL61B: _tmpC3A=_tmpC39.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC3B=(struct Cyc_Absyn_IntType_struct*)_tmpC3A;
if(_tmpC3B->tag != 6)goto _LL61D;else{_tmpC3C=_tmpC3B->f1;if(_tmpC3C != Cyc_Absyn_None)
goto _LL61D;_tmpC3D=_tmpC3B->f2;if(_tmpC3D != Cyc_Absyn_Char_sz)goto _LL61D;}};
_tmpC3E=_tmpC39.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC3F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC3E;if(_tmpC3F->tag != 0)goto _LL61D;};_LL61C: goto _LL61A;_LL61D:;_LL61E:{const
char*_tmp147D;void*_tmp147C;(_tmp147C=0,Cyc_Tcutil_terr(loc,((_tmp147D="format descriptor is not a char ? type",
_tag_dyneither(_tmp147D,sizeof(char),39))),_tag_dyneither(_tmp147C,sizeof(void*),
0)));}goto _LL61A;_LL61A:;}goto _LL615;_LL618:;_LL619:{const char*_tmp1480;void*
_tmp147F;(_tmp147F=0,Cyc_Tcutil_terr(loc,((_tmp1480="format descriptor is not a char ? type",
_tag_dyneither(_tmp1480,sizeof(char),39))),_tag_dyneither(_tmp147F,sizeof(void*),
0)));}goto _LL615;_LL615:;}if(fmt_arg_start != 0){int problem;{struct _tuple29
_tmp1481;struct _tuple29 _tmpC45=(_tmp1481.f1=ft,((_tmp1481.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB1A))->type)),
_tmp1481)));enum Cyc_Absyn_Format_Type _tmpC46;void*_tmpC47;struct Cyc_Absyn_DatatypeInfo
_tmpC49;union Cyc_Absyn_DatatypeInfoU _tmpC4A;struct Cyc_Absyn_Datatypedecl**
_tmpC4B;struct Cyc_Absyn_Datatypedecl*_tmpC4C;enum Cyc_Absyn_Format_Type _tmpC4D;
void*_tmpC4E;struct Cyc_Absyn_DatatypeInfo _tmpC50;union Cyc_Absyn_DatatypeInfoU
_tmpC51;struct Cyc_Absyn_Datatypedecl**_tmpC52;struct Cyc_Absyn_Datatypedecl*
_tmpC53;_LL620: _tmpC46=_tmpC45.f1;if(_tmpC46 != Cyc_Absyn_Printf_ft)goto _LL622;
_tmpC47=_tmpC45.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC48=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC47;if(_tmpC48->tag != 3)goto _LL622;else{_tmpC49=_tmpC48->f1;_tmpC4A=_tmpC49.datatype_info;
if((_tmpC4A.KnownDatatype).tag != 2)goto _LL622;_tmpC4B=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC4A.KnownDatatype).val;_tmpC4C=*_tmpC4B;}};_LL621: problem=Cyc_Absyn_qvar_cmp(
_tmpC4C->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL61F;_LL622: _tmpC4D=
_tmpC45.f1;if(_tmpC4D != Cyc_Absyn_Scanf_ft)goto _LL624;_tmpC4E=_tmpC45.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC4F=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC4E;if(_tmpC4F->tag != 3)goto _LL624;else{_tmpC50=_tmpC4F->f1;_tmpC51=_tmpC50.datatype_info;
if((_tmpC51.KnownDatatype).tag != 2)goto _LL624;_tmpC52=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC51.KnownDatatype).val;_tmpC53=*_tmpC52;}};_LL623: problem=Cyc_Absyn_qvar_cmp(
_tmpC53->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL61F;_LL624:;_LL625:
problem=1;goto _LL61F;_LL61F:;}if(problem){const char*_tmp1484;void*_tmp1483;(
_tmp1483=0,Cyc_Tcutil_terr(loc,((_tmp1484="format attribute and vararg types don't match",
_tag_dyneither(_tmp1484,sizeof(char),46))),_tag_dyneither(_tmp1483,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB1B;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpC57;void*_tmpC58;void*_tmpC59;struct _tuple0*_tmpC56=(struct _tuple0*)
rpo->hd;_tmpC57=*_tmpC56;_tmpC58=_tmpC57.f1;_tmpC59=_tmpC57.f2;_tmpC11=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC11,& Cyc_Tcutil_ek,_tmpC58,1);_tmpC11=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC11,& Cyc_Tcutil_trk,_tmpC59,1);}}if(*_tmpB16 != 0)_tmpC11=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC11,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB16))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC11.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpC5B;int _tmpC5C;struct
_tuple27 _tmpC5A=*((struct _tuple27*)tvs->hd);_tmpC5B=_tmpC5A.f1;_tmpC5C=_tmpC5A.f2;
if(!_tmpC5C)continue;{void*_tmpC5D=Cyc_Absyn_compress_kb(_tmpC5B->kind);struct
Cyc_Core_Opt*_tmpC5F;struct Cyc_Core_Opt**_tmpC60;struct Cyc_Absyn_Kind*_tmpC61;
struct Cyc_Absyn_Kind*_tmpC63;struct Cyc_Absyn_Kind*_tmpC65;struct Cyc_Absyn_Kind
_tmpC66;enum Cyc_Absyn_KindQual _tmpC67;struct Cyc_Absyn_Kind*_tmpC69;struct Cyc_Absyn_Kind
_tmpC6A;enum Cyc_Absyn_KindQual _tmpC6B;struct Cyc_Core_Opt*_tmpC6D;struct Cyc_Core_Opt**
_tmpC6E;struct Cyc_Absyn_Kind*_tmpC6F;struct Cyc_Absyn_Kind _tmpC70;enum Cyc_Absyn_KindQual
_tmpC71;struct Cyc_Absyn_Kind*_tmpC73;struct Cyc_Absyn_Kind _tmpC74;enum Cyc_Absyn_KindQual
_tmpC75;struct Cyc_Core_Opt*_tmpC77;struct Cyc_Core_Opt**_tmpC78;_LL627:{struct Cyc_Absyn_Less_kb_struct*
_tmpC5E=(struct Cyc_Absyn_Less_kb_struct*)_tmpC5D;if(_tmpC5E->tag != 2)goto _LL629;
else{_tmpC5F=_tmpC5E->f1;_tmpC60=(struct Cyc_Core_Opt**)& _tmpC5E->f1;_tmpC61=
_tmpC5E->f2;}}if(!(_tmpC61->kind == Cyc_Absyn_RgnKind))goto _LL629;_LL628: if(
_tmpC61->aliasqual == Cyc_Absyn_Top){*_tmpC60=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpC63=_tmpC61;goto _LL62A;}*_tmpC60=Cyc_Tcutil_kind_to_bound_opt(_tmpC61);
_tmpC63=_tmpC61;goto _LL62A;_LL629:{struct Cyc_Absyn_Eq_kb_struct*_tmpC62=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC5D;if(_tmpC62->tag != 0)goto _LL62B;else{_tmpC63=
_tmpC62->f1;}}if(!(_tmpC63->kind == Cyc_Absyn_RgnKind))goto _LL62B;_LL62A:{struct
Cyc_Absyn_AccessEff_struct*_tmp1493;struct Cyc_Absyn_VarType_struct*_tmp1492;
struct Cyc_Absyn_VarType_struct _tmp1491;struct Cyc_Absyn_AccessEff_struct _tmp1490;
struct Cyc_List_List*_tmp148F;effect=((_tmp148F=_cycalloc(sizeof(*_tmp148F)),((
_tmp148F->hd=(void*)((_tmp1493=_cycalloc(sizeof(*_tmp1493)),((_tmp1493[0]=((
_tmp1490.tag=23,((_tmp1490.f1=(void*)((void*)((_tmp1492=_cycalloc(sizeof(*
_tmp1492)),((_tmp1492[0]=((_tmp1491.tag=2,((_tmp1491.f1=_tmpC5B,_tmp1491)))),
_tmp1492))))),_tmp1490)))),_tmp1493)))),((_tmp148F->tl=effect,_tmp148F))))));}
goto _LL626;_LL62B: {struct Cyc_Absyn_Less_kb_struct*_tmpC64=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC5D;if(_tmpC64->tag != 2)goto _LL62D;else{_tmpC65=_tmpC64->f2;_tmpC66=*_tmpC65;
_tmpC67=_tmpC66.kind;if(_tmpC67 != Cyc_Absyn_IntKind)goto _LL62D;}}_LL62C: goto
_LL62E;_LL62D: {struct Cyc_Absyn_Eq_kb_struct*_tmpC68=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC5D;if(_tmpC68->tag != 0)goto _LL62F;else{_tmpC69=_tmpC68->f1;_tmpC6A=*_tmpC69;
_tmpC6B=_tmpC6A.kind;if(_tmpC6B != Cyc_Absyn_IntKind)goto _LL62F;}}_LL62E: goto
_LL626;_LL62F: {struct Cyc_Absyn_Less_kb_struct*_tmpC6C=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC5D;if(_tmpC6C->tag != 2)goto _LL631;else{_tmpC6D=_tmpC6C->f1;_tmpC6E=(struct
Cyc_Core_Opt**)& _tmpC6C->f1;_tmpC6F=_tmpC6C->f2;_tmpC70=*_tmpC6F;_tmpC71=_tmpC70.kind;
if(_tmpC71 != Cyc_Absyn_EffKind)goto _LL631;}}_LL630:*_tmpC6E=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL632;_LL631: {struct Cyc_Absyn_Eq_kb_struct*_tmpC72=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpC5D;if(_tmpC72->tag != 0)goto _LL633;else{_tmpC73=
_tmpC72->f1;_tmpC74=*_tmpC73;_tmpC75=_tmpC74.kind;if(_tmpC75 != Cyc_Absyn_EffKind)
goto _LL633;}}_LL632:{struct Cyc_Absyn_VarType_struct*_tmp1499;struct Cyc_Absyn_VarType_struct
_tmp1498;struct Cyc_List_List*_tmp1497;effect=((_tmp1497=_cycalloc(sizeof(*
_tmp1497)),((_tmp1497->hd=(void*)((_tmp1499=_cycalloc(sizeof(*_tmp1499)),((
_tmp1499[0]=((_tmp1498.tag=2,((_tmp1498.f1=_tmpC5B,_tmp1498)))),_tmp1499)))),((
_tmp1497->tl=effect,_tmp1497))))));}goto _LL626;_LL633: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC76=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC5D;if(_tmpC76->tag != 1)goto
_LL635;else{_tmpC77=_tmpC76->f1;_tmpC78=(struct Cyc_Core_Opt**)& _tmpC76->f1;}}
_LL634:{struct Cyc_Absyn_Less_kb_struct*_tmp149F;struct Cyc_Absyn_Less_kb_struct
_tmp149E;struct Cyc_Core_Opt*_tmp149D;*_tmpC78=((_tmp149D=_cycalloc(sizeof(*
_tmp149D)),((_tmp149D->v=(void*)((_tmp149F=_cycalloc(sizeof(*_tmp149F)),((
_tmp149F[0]=((_tmp149E.tag=2,((_tmp149E.f1=0,((_tmp149E.f2=& Cyc_Tcutil_ak,
_tmp149E)))))),_tmp149F)))),_tmp149D))));}goto _LL636;_LL635:;_LL636:{struct Cyc_Absyn_RgnsEff_struct*
_tmp14AE;struct Cyc_Absyn_VarType_struct*_tmp14AD;struct Cyc_Absyn_VarType_struct
_tmp14AC;struct Cyc_Absyn_RgnsEff_struct _tmp14AB;struct Cyc_List_List*_tmp14AA;
effect=((_tmp14AA=_cycalloc(sizeof(*_tmp14AA)),((_tmp14AA->hd=(void*)((_tmp14AE=
_cycalloc(sizeof(*_tmp14AE)),((_tmp14AE[0]=((_tmp14AB.tag=25,((_tmp14AB.f1=(void*)((
void*)((_tmp14AD=_cycalloc(sizeof(*_tmp14AD)),((_tmp14AD[0]=((_tmp14AC.tag=2,((
_tmp14AC.f1=_tmpC5B,_tmp14AC)))),_tmp14AD))))),_tmp14AB)))),_tmp14AE)))),((
_tmp14AA->tl=effect,_tmp14AA))))));}goto _LL626;_LL626:;};}}{struct Cyc_List_List*
ts=_tmpC11.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpC8A;int _tmpC8B;struct
_tuple28 _tmpC89=*((struct _tuple28*)ts->hd);_tmpC8A=_tmpC89.f1;_tmpC8B=_tmpC89.f2;
if(!_tmpC8B)continue;{struct Cyc_Absyn_Kind*_tmpC8C=Cyc_Tcutil_typ_kind(_tmpC8A);
struct Cyc_Absyn_Kind _tmpC8D;enum Cyc_Absyn_KindQual _tmpC8E;struct Cyc_Absyn_Kind
_tmpC8F;enum Cyc_Absyn_KindQual _tmpC90;struct Cyc_Absyn_Kind _tmpC91;enum Cyc_Absyn_KindQual
_tmpC92;_LL638: _tmpC8D=*_tmpC8C;_tmpC8E=_tmpC8D.kind;if(_tmpC8E != Cyc_Absyn_RgnKind)
goto _LL63A;_LL639:{struct Cyc_Absyn_AccessEff_struct*_tmp14B4;struct Cyc_Absyn_AccessEff_struct
_tmp14B3;struct Cyc_List_List*_tmp14B2;effect=((_tmp14B2=_cycalloc(sizeof(*
_tmp14B2)),((_tmp14B2->hd=(void*)((_tmp14B4=_cycalloc(sizeof(*_tmp14B4)),((
_tmp14B4[0]=((_tmp14B3.tag=23,((_tmp14B3.f1=(void*)_tmpC8A,_tmp14B3)))),_tmp14B4)))),((
_tmp14B2->tl=effect,_tmp14B2))))));}goto _LL637;_LL63A: _tmpC8F=*_tmpC8C;_tmpC90=
_tmpC8F.kind;if(_tmpC90 != Cyc_Absyn_EffKind)goto _LL63C;_LL63B:{struct Cyc_List_List*
_tmp14B5;effect=((_tmp14B5=_cycalloc(sizeof(*_tmp14B5)),((_tmp14B5->hd=_tmpC8A,((
_tmp14B5->tl=effect,_tmp14B5))))));}goto _LL637;_LL63C: _tmpC91=*_tmpC8C;_tmpC92=
_tmpC91.kind;if(_tmpC92 != Cyc_Absyn_IntKind)goto _LL63E;_LL63D: goto _LL637;_LL63E:;
_LL63F:{struct Cyc_Absyn_RgnsEff_struct*_tmp14BB;struct Cyc_Absyn_RgnsEff_struct
_tmp14BA;struct Cyc_List_List*_tmp14B9;effect=((_tmp14B9=_cycalloc(sizeof(*
_tmp14B9)),((_tmp14B9->hd=(void*)((_tmp14BB=_cycalloc(sizeof(*_tmp14BB)),((
_tmp14BB[0]=((_tmp14BA.tag=25,((_tmp14BA.f1=(void*)_tmpC8A,_tmp14BA)))),_tmp14BB)))),((
_tmp14B9->tl=effect,_tmp14B9))))));}goto _LL637;_LL637:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp14C1;struct Cyc_Absyn_JoinEff_struct _tmp14C0;struct Cyc_Core_Opt*_tmp14BF;*
_tmpB16=((_tmp14BF=_cycalloc(sizeof(*_tmp14BF)),((_tmp14BF->v=(void*)((_tmp14C1=
_cycalloc(sizeof(*_tmp14C1)),((_tmp14C1[0]=((_tmp14C0.tag=24,((_tmp14C0.f1=
effect,_tmp14C0)))),_tmp14C1)))),_tmp14BF))));};}if(*_tmpB14 != 0){struct Cyc_List_List*
bs=*_tmpB14;for(0;bs != 0;bs=bs->tl){void*_tmpC9D=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpC9F;struct Cyc_Core_Opt**
_tmpCA0;struct Cyc_Core_Opt*_tmpCA2;struct Cyc_Core_Opt**_tmpCA3;struct Cyc_Absyn_Kind*
_tmpCA4;struct Cyc_Absyn_Kind _tmpCA5;enum Cyc_Absyn_KindQual _tmpCA6;enum Cyc_Absyn_AliasQual
_tmpCA7;struct Cyc_Core_Opt*_tmpCA9;struct Cyc_Core_Opt**_tmpCAA;struct Cyc_Absyn_Kind*
_tmpCAB;struct Cyc_Absyn_Kind _tmpCAC;enum Cyc_Absyn_KindQual _tmpCAD;enum Cyc_Absyn_AliasQual
_tmpCAE;struct Cyc_Core_Opt*_tmpCB0;struct Cyc_Core_Opt**_tmpCB1;struct Cyc_Absyn_Kind*
_tmpCB2;struct Cyc_Absyn_Kind _tmpCB3;enum Cyc_Absyn_KindQual _tmpCB4;enum Cyc_Absyn_AliasQual
_tmpCB5;struct Cyc_Core_Opt*_tmpCB7;struct Cyc_Core_Opt**_tmpCB8;struct Cyc_Absyn_Kind*
_tmpCB9;struct Cyc_Absyn_Kind _tmpCBA;enum Cyc_Absyn_KindQual _tmpCBB;enum Cyc_Absyn_AliasQual
_tmpCBC;struct Cyc_Core_Opt*_tmpCBE;struct Cyc_Core_Opt**_tmpCBF;struct Cyc_Absyn_Kind*
_tmpCC0;struct Cyc_Absyn_Kind _tmpCC1;enum Cyc_Absyn_KindQual _tmpCC2;enum Cyc_Absyn_AliasQual
_tmpCC3;struct Cyc_Core_Opt*_tmpCC5;struct Cyc_Core_Opt**_tmpCC6;struct Cyc_Absyn_Kind*
_tmpCC7;struct Cyc_Absyn_Kind _tmpCC8;enum Cyc_Absyn_KindQual _tmpCC9;enum Cyc_Absyn_AliasQual
_tmpCCA;struct Cyc_Core_Opt*_tmpCCC;struct Cyc_Core_Opt**_tmpCCD;struct Cyc_Absyn_Kind*
_tmpCCE;struct Cyc_Absyn_Kind _tmpCCF;enum Cyc_Absyn_KindQual _tmpCD0;enum Cyc_Absyn_AliasQual
_tmpCD1;struct Cyc_Core_Opt*_tmpCD3;struct Cyc_Core_Opt**_tmpCD4;struct Cyc_Absyn_Kind*
_tmpCD5;struct Cyc_Absyn_Kind _tmpCD6;enum Cyc_Absyn_KindQual _tmpCD7;enum Cyc_Absyn_AliasQual
_tmpCD8;struct Cyc_Core_Opt*_tmpCDA;struct Cyc_Core_Opt**_tmpCDB;struct Cyc_Absyn_Kind*
_tmpCDC;struct Cyc_Absyn_Kind*_tmpCDE;struct Cyc_Absyn_Kind _tmpCDF;enum Cyc_Absyn_KindQual
_tmpCE0;_LL641: {struct Cyc_Absyn_Unknown_kb_struct*_tmpC9E=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC9D;if(_tmpC9E->tag != 1)goto _LL643;else{_tmpC9F=_tmpC9E->f1;_tmpCA0=(struct
Cyc_Core_Opt**)& _tmpC9E->f1;}}_LL642:{const char*_tmp14C5;void*_tmp14C4[1];struct
Cyc_String_pa_struct _tmp14C3;(_tmp14C3.tag=0,((_tmp14C3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp14C4[0]=&
_tmp14C3,Cyc_Tcutil_warn(loc,((_tmp14C5="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp14C5,sizeof(char),47))),_tag_dyneither(_tmp14C4,sizeof(void*),
1)))))));}_tmpCA3=_tmpCA0;goto _LL644;_LL643: {struct Cyc_Absyn_Less_kb_struct*
_tmpCA1=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpCA1->tag != 2)goto _LL645;
else{_tmpCA2=_tmpCA1->f1;_tmpCA3=(struct Cyc_Core_Opt**)& _tmpCA1->f1;_tmpCA4=
_tmpCA1->f2;_tmpCA5=*_tmpCA4;_tmpCA6=_tmpCA5.kind;if(_tmpCA6 != Cyc_Absyn_BoxKind)
goto _LL645;_tmpCA7=_tmpCA5.aliasqual;if(_tmpCA7 != Cyc_Absyn_Top)goto _LL645;}}
_LL644: _tmpCAA=_tmpCA3;goto _LL646;_LL645: {struct Cyc_Absyn_Less_kb_struct*
_tmpCA8=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpCA8->tag != 2)goto _LL647;
else{_tmpCA9=_tmpCA8->f1;_tmpCAA=(struct Cyc_Core_Opt**)& _tmpCA8->f1;_tmpCAB=
_tmpCA8->f2;_tmpCAC=*_tmpCAB;_tmpCAD=_tmpCAC.kind;if(_tmpCAD != Cyc_Absyn_MemKind)
goto _LL647;_tmpCAE=_tmpCAC.aliasqual;if(_tmpCAE != Cyc_Absyn_Top)goto _LL647;}}
_LL646: _tmpCB1=_tmpCAA;goto _LL648;_LL647: {struct Cyc_Absyn_Less_kb_struct*
_tmpCAF=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpCAF->tag != 2)goto _LL649;
else{_tmpCB0=_tmpCAF->f1;_tmpCB1=(struct Cyc_Core_Opt**)& _tmpCAF->f1;_tmpCB2=
_tmpCAF->f2;_tmpCB3=*_tmpCB2;_tmpCB4=_tmpCB3.kind;if(_tmpCB4 != Cyc_Absyn_MemKind)
goto _LL649;_tmpCB5=_tmpCB3.aliasqual;if(_tmpCB5 != Cyc_Absyn_Aliasable)goto _LL649;}}
_LL648: _tmpCB8=_tmpCB1;goto _LL64A;_LL649: {struct Cyc_Absyn_Less_kb_struct*
_tmpCB6=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpCB6->tag != 2)goto _LL64B;
else{_tmpCB7=_tmpCB6->f1;_tmpCB8=(struct Cyc_Core_Opt**)& _tmpCB6->f1;_tmpCB9=
_tmpCB6->f2;_tmpCBA=*_tmpCB9;_tmpCBB=_tmpCBA.kind;if(_tmpCBB != Cyc_Absyn_AnyKind)
goto _LL64B;_tmpCBC=_tmpCBA.aliasqual;if(_tmpCBC != Cyc_Absyn_Top)goto _LL64B;}}
_LL64A: _tmpCBF=_tmpCB8;goto _LL64C;_LL64B: {struct Cyc_Absyn_Less_kb_struct*
_tmpCBD=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpCBD->tag != 2)goto _LL64D;
else{_tmpCBE=_tmpCBD->f1;_tmpCBF=(struct Cyc_Core_Opt**)& _tmpCBD->f1;_tmpCC0=
_tmpCBD->f2;_tmpCC1=*_tmpCC0;_tmpCC2=_tmpCC1.kind;if(_tmpCC2 != Cyc_Absyn_AnyKind)
goto _LL64D;_tmpCC3=_tmpCC1.aliasqual;if(_tmpCC3 != Cyc_Absyn_Aliasable)goto _LL64D;}}
_LL64C:*_tmpCBF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL640;_LL64D: {
struct Cyc_Absyn_Less_kb_struct*_tmpCC4=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;
if(_tmpCC4->tag != 2)goto _LL64F;else{_tmpCC5=_tmpCC4->f1;_tmpCC6=(struct Cyc_Core_Opt**)&
_tmpCC4->f1;_tmpCC7=_tmpCC4->f2;_tmpCC8=*_tmpCC7;_tmpCC9=_tmpCC8.kind;if(_tmpCC9
!= Cyc_Absyn_MemKind)goto _LL64F;_tmpCCA=_tmpCC8.aliasqual;if(_tmpCCA != Cyc_Absyn_Unique)
goto _LL64F;}}_LL64E: _tmpCCD=_tmpCC6;goto _LL650;_LL64F: {struct Cyc_Absyn_Less_kb_struct*
_tmpCCB=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;if(_tmpCCB->tag != 2)goto _LL651;
else{_tmpCCC=_tmpCCB->f1;_tmpCCD=(struct Cyc_Core_Opt**)& _tmpCCB->f1;_tmpCCE=
_tmpCCB->f2;_tmpCCF=*_tmpCCE;_tmpCD0=_tmpCCF.kind;if(_tmpCD0 != Cyc_Absyn_AnyKind)
goto _LL651;_tmpCD1=_tmpCCF.aliasqual;if(_tmpCD1 != Cyc_Absyn_Unique)goto _LL651;}}
_LL650:*_tmpCCD=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL640;_LL651: {
struct Cyc_Absyn_Less_kb_struct*_tmpCD2=(struct Cyc_Absyn_Less_kb_struct*)_tmpC9D;
if(_tmpCD2->tag != 2)goto _LL653;else{_tmpCD3=_tmpCD2->f1;_tmpCD4=(struct Cyc_Core_Opt**)&
_tmpCD2->f1;_tmpCD5=_tmpCD2->f2;_tmpCD6=*_tmpCD5;_tmpCD7=_tmpCD6.kind;if(_tmpCD7
!= Cyc_Absyn_RgnKind)goto _LL653;_tmpCD8=_tmpCD6.aliasqual;if(_tmpCD8 != Cyc_Absyn_Top)
goto _LL653;}}_LL652:*_tmpCD4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL640;_LL653: {struct Cyc_Absyn_Less_kb_struct*_tmpCD9=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9D;if(_tmpCD9->tag != 2)goto _LL655;else{_tmpCDA=_tmpCD9->f1;_tmpCDB=(struct
Cyc_Core_Opt**)& _tmpCD9->f1;_tmpCDC=_tmpCD9->f2;}}_LL654:*_tmpCDB=Cyc_Tcutil_kind_to_bound_opt(
_tmpCDC);goto _LL640;_LL655: {struct Cyc_Absyn_Eq_kb_struct*_tmpCDD=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC9D;if(_tmpCDD->tag != 0)goto _LL657;else{_tmpCDE=_tmpCDD->f1;_tmpCDF=*_tmpCDE;
_tmpCE0=_tmpCDF.kind;if(_tmpCE0 != Cyc_Absyn_MemKind)goto _LL657;}}_LL656:{const
char*_tmp14C8;void*_tmp14C7;(_tmp14C7=0,Cyc_Tcutil_terr(loc,((_tmp14C8="functions can't abstract M types",
_tag_dyneither(_tmp14C8,sizeof(char),33))),_tag_dyneither(_tmp14C7,sizeof(void*),
0)));}goto _LL640;_LL657:;_LL658: goto _LL640;_LL640:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC11.kind_env,*_tmpB14);_tmpC11.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC11.r,_tmpC11.free_vars,*_tmpB14);{struct Cyc_List_List*tvs=_tmpC11.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpCE7;int _tmpCE8;struct
_tuple27 _tmpCE6=*((struct _tuple27*)tvs->hd);_tmpCE7=_tmpCE6.f1;_tmpCE8=_tmpCE6.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpCE7,_tmpCE8);}}{struct Cyc_List_List*evs=_tmpC11.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpCEA;int _tmpCEB;struct _tuple28 _tmpCE9=*((struct _tuple28*)evs->hd);
_tmpCEA=_tmpCE9.f1;_tmpCEB=_tmpCE9.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpCEA,_tmpCEB);}};}goto _LL565;};}_LL586: {struct Cyc_Absyn_TupleType_struct*
_tmpB1D=(struct Cyc_Absyn_TupleType_struct*)_tmpAD7;if(_tmpB1D->tag != 11)goto
_LL588;else{_tmpB1E=_tmpB1D->f1;}}_LL587: for(0;_tmpB1E != 0;_tmpB1E=_tmpB1E->tl){
struct _tuple11*_tmpCED=(struct _tuple11*)_tmpB1E->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpCED).f2,1);((*_tmpCED).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpCED).f1).print_const,(*_tmpCED).f2);}goto _LL565;_LL588: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB1F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpAD7;if(_tmpB1F->tag != 13)goto
_LL58A;else{_tmpB20=_tmpB1F->f1;_tmpB21=_tmpB1F->f2;}}_LL589: {struct
_RegionHandle*_tmpCEE=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB21 != 0;_tmpB21=_tmpB21->tl){struct Cyc_Absyn_Aggrfield _tmpCF0;struct
_dyneither_ptr*_tmpCF1;struct Cyc_Absyn_Tqual _tmpCF2;struct Cyc_Absyn_Tqual*
_tmpCF3;void*_tmpCF4;struct Cyc_Absyn_Exp*_tmpCF5;struct Cyc_List_List*_tmpCF6;
struct Cyc_Absyn_Aggrfield*_tmpCEF=(struct Cyc_Absyn_Aggrfield*)_tmpB21->hd;
_tmpCF0=*_tmpCEF;_tmpCF1=_tmpCF0.name;_tmpCF2=_tmpCF0.tq;_tmpCF3=(struct Cyc_Absyn_Tqual*)&(*
_tmpCEF).tq;_tmpCF4=_tmpCF0.type;_tmpCF5=_tmpCF0.width;_tmpCF6=_tmpCF0.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpCF1)){
const char*_tmp14CC;void*_tmp14CB[1];struct Cyc_String_pa_struct _tmp14CA;(_tmp14CA.tag=
0,((_tmp14CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCF1),((
_tmp14CB[0]=& _tmp14CA,Cyc_Tcutil_terr(loc,((_tmp14CC="duplicate field %s",
_tag_dyneither(_tmp14CC,sizeof(char),19))),_tag_dyneither(_tmp14CB,sizeof(void*),
1)))))));}{const char*_tmp14CD;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpCF1,((
_tmp14CD="",_tag_dyneither(_tmp14CD,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp14CE;prev_fields=((_tmp14CE=_region_malloc(_tmpCEE,sizeof(*_tmp14CE)),((
_tmp14CE->hd=_tmpCF1,((_tmp14CE->tl=prev_fields,_tmp14CE))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpCF4,1);_tmpCF3->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpCF3->print_const,_tmpCF4);if(_tmpB20 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpCF4)){const char*_tmp14D2;void*_tmp14D1[1];struct Cyc_String_pa_struct _tmp14D0;(
_tmp14D0.tag=0,((_tmp14D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpCF1),((_tmp14D1[0]=& _tmp14D0,Cyc_Tcutil_warn(loc,((_tmp14D2="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp14D2,sizeof(char),74))),_tag_dyneither(_tmp14D1,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpCF4,_tmpCF5,_tmpCF1);Cyc_Tcutil_check_field_atts(
loc,_tmpCF1,_tmpCF6);}}goto _LL565;}_LL58A: {struct Cyc_Absyn_AggrType_struct*
_tmpB22=(struct Cyc_Absyn_AggrType_struct*)_tmpAD7;if(_tmpB22->tag != 12)goto
_LL58C;else{_tmpB23=_tmpB22->f1;_tmpB24=_tmpB23.aggr_info;_tmpB25=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB22->f1).aggr_info;_tmpB26=_tmpB23.targs;_tmpB27=(struct Cyc_List_List**)&(
_tmpB22->f1).targs;}}_LL58B:{union Cyc_Absyn_AggrInfoU _tmpCFF=*_tmpB25;struct
_tuple4 _tmpD00;enum Cyc_Absyn_AggrKind _tmpD01;struct _tuple2*_tmpD02;struct Cyc_Core_Opt*
_tmpD03;struct Cyc_Absyn_Aggrdecl**_tmpD04;struct Cyc_Absyn_Aggrdecl*_tmpD05;
_LL65A: if((_tmpCFF.UnknownAggr).tag != 1)goto _LL65C;_tmpD00=(struct _tuple4)(
_tmpCFF.UnknownAggr).val;_tmpD01=_tmpD00.f1;_tmpD02=_tmpD00.f2;_tmpD03=_tmpD00.f3;
_LL65B: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD06;_push_handler(&
_tmpD06);{int _tmpD08=0;if(setjmp(_tmpD06.handler))_tmpD08=1;if(!_tmpD08){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD02);{struct Cyc_Absyn_Aggrdecl*_tmpD09=*adp;if(_tmpD09->kind != 
_tmpD01){if(_tmpD09->kind == Cyc_Absyn_StructA){const char*_tmp14D7;void*_tmp14D6[
2];struct Cyc_String_pa_struct _tmp14D5;struct Cyc_String_pa_struct _tmp14D4;(
_tmp14D4.tag=0,((_tmp14D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD02)),((_tmp14D5.tag=0,((_tmp14D5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD02)),((_tmp14D6[0]=& _tmp14D5,((
_tmp14D6[1]=& _tmp14D4,Cyc_Tcutil_terr(loc,((_tmp14D7="expecting struct %s instead of union %s",
_tag_dyneither(_tmp14D7,sizeof(char),40))),_tag_dyneither(_tmp14D6,sizeof(void*),
2)))))))))))));}else{const char*_tmp14DC;void*_tmp14DB[2];struct Cyc_String_pa_struct
_tmp14DA;struct Cyc_String_pa_struct _tmp14D9;(_tmp14D9.tag=0,((_tmp14D9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD02)),((
_tmp14DA.tag=0,((_tmp14DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD02)),((_tmp14DB[0]=& _tmp14DA,((_tmp14DB[1]=& _tmp14D9,Cyc_Tcutil_terr(loc,((
_tmp14DC="expecting union %s instead of struct %s",_tag_dyneither(_tmp14DC,
sizeof(char),40))),_tag_dyneither(_tmp14DB,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD03  && (int)_tmpD03->v){if(!((unsigned int)_tmpD09->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD09->impl))->tagged){const char*
_tmp14E0;void*_tmp14DF[1];struct Cyc_String_pa_struct _tmp14DE;(_tmp14DE.tag=0,((
_tmp14DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD02)),((_tmp14DF[0]=& _tmp14DE,Cyc_Tcutil_terr(loc,((_tmp14E0="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp14E0,sizeof(char),42))),_tag_dyneither(_tmp14DF,sizeof(void*),
1)))))));}}*_tmpB25=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD07=(void*)_exn_thrown;void*_tmpD16=_tmpD07;_LL65F: {struct Cyc_Dict_Absent_struct*
_tmpD17=(struct Cyc_Dict_Absent_struct*)_tmpD16;if(_tmpD17->tag != Cyc_Dict_Absent)
goto _LL661;}_LL660: {struct Cyc_Tcenv_Genv*_tmpD18=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp14E1;struct Cyc_Absyn_Aggrdecl*_tmpD19=(_tmp14E1=
_cycalloc(sizeof(*_tmp14E1)),((_tmp14E1->kind=_tmpD01,((_tmp14E1->sc=Cyc_Absyn_Extern,((
_tmp14E1->name=_tmpD02,((_tmp14E1->tvs=0,((_tmp14E1->impl=0,((_tmp14E1->attributes=
0,_tmp14E1)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD18,loc,_tmpD19);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD02);*_tmpB25=Cyc_Absyn_KnownAggr(adp);if(*_tmpB27 != 0){{const char*
_tmp14E5;void*_tmp14E4[1];struct Cyc_String_pa_struct _tmp14E3;(_tmp14E3.tag=0,((
_tmp14E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD02)),((_tmp14E4[0]=& _tmp14E3,Cyc_Tcutil_terr(loc,((_tmp14E5="declare parameterized type %s before using",
_tag_dyneither(_tmp14E5,sizeof(char),43))),_tag_dyneither(_tmp14E4,sizeof(void*),
1)))))));}return cvtenv;}goto _LL65E;}_LL661:;_LL662:(void)_throw(_tmpD16);_LL65E:;}};}
_tmpD05=*adp;goto _LL65D;}_LL65C: if((_tmpCFF.KnownAggr).tag != 2)goto _LL659;
_tmpD04=(struct Cyc_Absyn_Aggrdecl**)(_tmpCFF.KnownAggr).val;_tmpD05=*_tmpD04;
_LL65D: {struct Cyc_List_List*tvs=_tmpD05->tvs;struct Cyc_List_List*ts=*_tmpB27;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD1E=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD1F=(void*)ts->hd;{struct _tuple0 _tmp14E6;
struct _tuple0 _tmpD21=(_tmp14E6.f1=Cyc_Absyn_compress_kb(_tmpD1E->kind),((
_tmp14E6.f2=_tmpD1F,_tmp14E6)));void*_tmpD22;void*_tmpD24;struct Cyc_Absyn_Tvar*
_tmpD26;_LL664: _tmpD22=_tmpD21.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD23=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD22;if(_tmpD23->tag != 1)goto _LL666;};
_tmpD24=_tmpD21.f2;{struct Cyc_Absyn_VarType_struct*_tmpD25=(struct Cyc_Absyn_VarType_struct*)
_tmpD24;if(_tmpD25->tag != 2)goto _LL666;else{_tmpD26=_tmpD25->f1;}};_LL665: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD26);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD26,1);continue;_LL666:;_LL667: goto _LL663;_LL663:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp14EA;void*_tmp14E9[1];struct Cyc_String_pa_struct _tmp14E8;(
_tmp14E8.tag=0,((_tmp14E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD05->name)),((_tmp14E9[0]=& _tmp14E8,Cyc_Tcutil_terr(loc,((_tmp14EA="too many parameters for type %s",
_tag_dyneither(_tmp14EA,sizeof(char),32))),_tag_dyneither(_tmp14E9,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp14EB;hidden_ts=((_tmp14EB=_cycalloc(sizeof(*_tmp14EB)),((_tmp14EB->hd=e,((
_tmp14EB->tl=hidden_ts,_tmp14EB))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k,e,1);}*_tmpB27=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB27,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL659:;}goto _LL565;
_LL58C: {struct Cyc_Absyn_TypedefType_struct*_tmpB28=(struct Cyc_Absyn_TypedefType_struct*)
_tmpAD7;if(_tmpB28->tag != 18)goto _LL58E;else{_tmpB29=_tmpB28->f1;_tmpB2A=_tmpB28->f2;
_tmpB2B=(struct Cyc_List_List**)& _tmpB28->f2;_tmpB2C=_tmpB28->f3;_tmpB2D=(struct
Cyc_Absyn_Typedefdecl**)& _tmpB28->f3;_tmpB2E=_tmpB28->f4;_tmpB2F=(void***)&
_tmpB28->f4;}}_LL58D: {struct Cyc_List_List*targs=*_tmpB2B;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpD2B;_push_handler(& _tmpD2B);{int _tmpD2D=0;if(setjmp(
_tmpD2B.handler))_tmpD2D=1;if(!_tmpD2D){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpB29);;_pop_handler();}else{void*_tmpD2C=(void*)_exn_thrown;void*_tmpD2F=
_tmpD2C;_LL669: {struct Cyc_Dict_Absent_struct*_tmpD30=(struct Cyc_Dict_Absent_struct*)
_tmpD2F;if(_tmpD30->tag != Cyc_Dict_Absent)goto _LL66B;}_LL66A:{const char*_tmp14EF;
void*_tmp14EE[1];struct Cyc_String_pa_struct _tmp14ED;(_tmp14ED.tag=0,((_tmp14ED.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB29)),((
_tmp14EE[0]=& _tmp14ED,Cyc_Tcutil_terr(loc,((_tmp14EF="unbound typedef name %s",
_tag_dyneither(_tmp14EF,sizeof(char),24))),_tag_dyneither(_tmp14EE,sizeof(void*),
1)))))));}return cvtenv;_LL66B:;_LL66C:(void)_throw(_tmpD2F);_LL668:;}};}*_tmpB2D=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpB29[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpD34=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);{struct _tuple15*_tmp14F2;struct Cyc_List_List*
_tmp14F1;inst=((_tmp14F1=_region_malloc(_tmpD34,sizeof(*_tmp14F1)),((_tmp14F1->hd=((
_tmp14F2=_region_malloc(_tmpD34,sizeof(*_tmp14F2)),((_tmp14F2->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp14F2->f2=(void*)ts->hd,_tmp14F2)))))),((_tmp14F1->tl=inst,_tmp14F1))))));};}
if(ts != 0){const char*_tmp14F6;void*_tmp14F5[1];struct Cyc_String_pa_struct
_tmp14F4;(_tmp14F4.tag=0,((_tmp14F4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB29)),((_tmp14F5[0]=& _tmp14F4,Cyc_Tcutil_terr(
loc,((_tmp14F6="too many parameters for typedef %s",_tag_dyneither(_tmp14F6,
sizeof(char),35))),_tag_dyneither(_tmp14F5,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp14F7;hidden_ts=((_tmp14F7=_cycalloc(sizeof(*_tmp14F7)),((
_tmp14F7->hd=e,((_tmp14F7->tl=hidden_ts,_tmp14F7))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple15*_tmp14FA;struct Cyc_List_List*_tmp14F9;inst=(
struct Cyc_List_List*)((_tmp14F9=_cycalloc(sizeof(*_tmp14F9)),((_tmp14F9->hd=(
struct _tuple15*)((_tmp14FA=_cycalloc(sizeof(*_tmp14FA)),((_tmp14FA->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp14FA->f2=e,_tmp14FA)))))),((_tmp14F9->tl=inst,
_tmp14F9))))));};}*_tmpB2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD34,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp14FB;*_tmpB2F=((_tmp14FB=_cycalloc(sizeof(*
_tmp14FB)),((_tmp14FB[0]=new_typ,_tmp14FB))));}}goto _LL565;};}_LL58E: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpB30=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpAD7;if(_tmpB30->tag != 22)goto
_LL590;}_LL58F: goto _LL591;_LL590: {struct Cyc_Absyn_HeapRgn_struct*_tmpB31=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpAD7;if(_tmpB31->tag != 21)goto _LL592;}_LL591:
goto _LL565;_LL592: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB32=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpAD7;if(_tmpB32->tag != 16)goto _LL594;else{_tmpB33=(void*)_tmpB32->f1;}}_LL593:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB33,1);
goto _LL565;_LL594: {struct Cyc_Absyn_DynRgnType_struct*_tmpB34=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpAD7;if(_tmpB34->tag != 17)goto _LL596;else{_tmpB35=(void*)_tmpB34->f1;_tmpB36=(
void*)_tmpB34->f2;}}_LL595: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpB35,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpB36,1);goto _LL565;_LL596: {struct Cyc_Absyn_AccessEff_struct*_tmpB37=(struct
Cyc_Absyn_AccessEff_struct*)_tmpAD7;if(_tmpB37->tag != 23)goto _LL598;else{_tmpB38=(
void*)_tmpB37->f1;}}_LL597: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpB38,1);goto _LL565;_LL598: {struct Cyc_Absyn_RgnsEff_struct*
_tmpB39=(struct Cyc_Absyn_RgnsEff_struct*)_tmpAD7;if(_tmpB39->tag != 25)goto _LL59A;
else{_tmpB3A=(void*)_tmpB39->f1;}}_LL599: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB3A,1);goto _LL565;_LL59A: {struct Cyc_Absyn_JoinEff_struct*
_tmpB3B=(struct Cyc_Absyn_JoinEff_struct*)_tmpAD7;if(_tmpB3B->tag != 24)goto _LL565;
else{_tmpB3C=_tmpB3B->f1;}}_LL59B: for(0;_tmpB3C != 0;_tmpB3C=_tmpB3C->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB3C->hd,1);}
goto _LL565;_LL565:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpD3E=t;struct Cyc_Core_Opt*_tmpD40;struct Cyc_Core_Opt*_tmpD41;_LL66E: {
struct Cyc_Absyn_Evar_struct*_tmpD3F=(struct Cyc_Absyn_Evar_struct*)_tmpD3E;if(
_tmpD3F->tag != 1)goto _LL670;else{_tmpD40=_tmpD3F->f1;_tmpD41=_tmpD3F->f2;}}
_LL66F: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpD42=_tmpD40;struct Cyc_Core_Opt
_tmpD43;struct Cyc_Absyn_Kind*_tmpD44;_LL673: if(_tmpD42 != 0)goto _LL675;_LL674:{
const char*_tmp14FC;s=((_tmp14FC="kind=NULL ",_tag_dyneither(_tmp14FC,sizeof(char),
11)));}goto _LL672;_LL675: if(_tmpD42 == 0)goto _LL672;_tmpD43=*_tmpD42;_tmpD44=(
struct Cyc_Absyn_Kind*)_tmpD43.v;_LL676:{const char*_tmp1500;void*_tmp14FF[1];
struct Cyc_String_pa_struct _tmp14FE;s=(struct _dyneither_ptr)((_tmp14FE.tag=0,((
_tmp14FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpD44)),((_tmp14FF[0]=& _tmp14FE,Cyc_aprintf(((_tmp1500="kind=%s ",
_tag_dyneither(_tmp1500,sizeof(char),9))),_tag_dyneither(_tmp14FF,sizeof(void*),
1))))))));}goto _LL672;_LL672:;}{struct Cyc_Core_Opt*_tmpD49=_tmpD41;struct Cyc_Core_Opt
_tmpD4A;void*_tmpD4B;_LL678: if(_tmpD49 != 0)goto _LL67A;_LL679:{const char*_tmp1504;
void*_tmp1503[1];struct Cyc_String_pa_struct _tmp1502;s=(struct _dyneither_ptr)((
_tmp1502.tag=0,((_tmp1502.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp1503[0]=& _tmp1502,Cyc_aprintf(((_tmp1504="%s ref=NULL",_tag_dyneither(
_tmp1504,sizeof(char),12))),_tag_dyneither(_tmp1503,sizeof(void*),1))))))));}
goto _LL677;_LL67A: if(_tmpD49 == 0)goto _LL677;_tmpD4A=*_tmpD49;_tmpD4B=(void*)
_tmpD4A.v;_LL67B:{const char*_tmp1509;void*_tmp1508[2];struct Cyc_String_pa_struct
_tmp1507;struct Cyc_String_pa_struct _tmp1506;s=(struct _dyneither_ptr)((_tmp1506.tag=
0,((_tmp1506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpD4B)),((_tmp1507.tag=0,((_tmp1507.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp1508[0]=& _tmp1507,((_tmp1508[1]=& _tmp1506,Cyc_aprintf(((
_tmp1509="%s ref=%s",_tag_dyneither(_tmp1509,sizeof(char),10))),_tag_dyneither(
_tmp1508,sizeof(void*),2))))))))))))));}goto _LL677;_LL677:;}goto _LL66D;}_LL670:;
_LL671: goto _LL66D;_LL66D:;}{const char*_tmp150F;void*_tmp150E[3];struct Cyc_String_pa_struct
_tmp150D;struct Cyc_String_pa_struct _tmp150C;struct Cyc_String_pa_struct _tmp150B;(
_tmp150B.tag=0,((_tmp150B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp150C.tag=0,((_tmp150C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp150D.tag=0,((
_tmp150D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp150E[0]=& _tmp150D,((_tmp150E[1]=& _tmp150C,((_tmp150E[2]=& _tmp150B,Cyc_Tcutil_terr(
loc,((_tmp150F="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp150F,sizeof(char),51))),_tag_dyneither(_tmp150E,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpD58=e->r;struct Cyc_List_List*_tmpD5E;struct Cyc_Absyn_Exp*_tmpD60;struct
Cyc_Absyn_Exp*_tmpD61;struct Cyc_Absyn_Exp*_tmpD62;struct Cyc_Absyn_Exp*_tmpD64;
struct Cyc_Absyn_Exp*_tmpD65;struct Cyc_Absyn_Exp*_tmpD67;struct Cyc_Absyn_Exp*
_tmpD68;struct Cyc_Absyn_Exp*_tmpD6A;struct Cyc_Absyn_Exp*_tmpD6B;void*_tmpD6D;
struct Cyc_Absyn_Exp*_tmpD6E;void*_tmpD70;void*_tmpD72;void*_tmpD74;struct Cyc_Absyn_Exp*
_tmpD76;_LL67D: {struct Cyc_Absyn_Const_e_struct*_tmpD59=(struct Cyc_Absyn_Const_e_struct*)
_tmpD58;if(_tmpD59->tag != 0)goto _LL67F;}_LL67E: goto _LL680;_LL67F: {struct Cyc_Absyn_Enum_e_struct*
_tmpD5A=(struct Cyc_Absyn_Enum_e_struct*)_tmpD58;if(_tmpD5A->tag != 32)goto _LL681;}
_LL680: goto _LL682;_LL681: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpD5B=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpD58;if(_tmpD5B->tag != 33)goto _LL683;}_LL682: goto _LL684;_LL683: {struct Cyc_Absyn_Var_e_struct*
_tmpD5C=(struct Cyc_Absyn_Var_e_struct*)_tmpD58;if(_tmpD5C->tag != 1)goto _LL685;}
_LL684: goto _LL67C;_LL685: {struct Cyc_Absyn_Primop_e_struct*_tmpD5D=(struct Cyc_Absyn_Primop_e_struct*)
_tmpD58;if(_tmpD5D->tag != 3)goto _LL687;else{_tmpD5E=_tmpD5D->f2;}}_LL686: for(0;
_tmpD5E != 0;_tmpD5E=_tmpD5E->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpD5E->hd,te,cvtenv);}goto _LL67C;_LL687: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD5F=(struct Cyc_Absyn_Conditional_e_struct*)_tmpD58;if(_tmpD5F->tag != 6)goto
_LL689;else{_tmpD60=_tmpD5F->f1;_tmpD61=_tmpD5F->f2;_tmpD62=_tmpD5F->f3;}}_LL688:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD60,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD61,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD62,te,
cvtenv);goto _LL67C;_LL689: {struct Cyc_Absyn_And_e_struct*_tmpD63=(struct Cyc_Absyn_And_e_struct*)
_tmpD58;if(_tmpD63->tag != 7)goto _LL68B;else{_tmpD64=_tmpD63->f1;_tmpD65=_tmpD63->f2;}}
_LL68A: _tmpD67=_tmpD64;_tmpD68=_tmpD65;goto _LL68C;_LL68B: {struct Cyc_Absyn_Or_e_struct*
_tmpD66=(struct Cyc_Absyn_Or_e_struct*)_tmpD58;if(_tmpD66->tag != 8)goto _LL68D;
else{_tmpD67=_tmpD66->f1;_tmpD68=_tmpD66->f2;}}_LL68C: _tmpD6A=_tmpD67;_tmpD6B=
_tmpD68;goto _LL68E;_LL68D: {struct Cyc_Absyn_SeqExp_e_struct*_tmpD69=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpD58;if(_tmpD69->tag != 9)goto _LL68F;else{_tmpD6A=_tmpD69->f1;_tmpD6B=_tmpD69->f2;}}
_LL68E: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD6A,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD6B,te,cvtenv);goto _LL67C;_LL68F: {
struct Cyc_Absyn_Cast_e_struct*_tmpD6C=(struct Cyc_Absyn_Cast_e_struct*)_tmpD58;
if(_tmpD6C->tag != 15)goto _LL691;else{_tmpD6D=(void*)_tmpD6C->f1;_tmpD6E=_tmpD6C->f2;}}
_LL690: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpD6E,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpD6D,1);goto
_LL67C;_LL691: {struct Cyc_Absyn_Offsetof_e_struct*_tmpD6F=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpD58;if(_tmpD6F->tag != 20)goto _LL693;else{_tmpD70=(void*)_tmpD6F->f1;}}_LL692:
_tmpD72=_tmpD70;goto _LL694;_LL693: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpD71=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpD58;if(_tmpD71->tag != 18)goto _LL695;
else{_tmpD72=(void*)_tmpD71->f1;}}_LL694: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpD72,1);goto _LL67C;_LL695: {struct Cyc_Absyn_Valueof_e_struct*
_tmpD73=(struct Cyc_Absyn_Valueof_e_struct*)_tmpD58;if(_tmpD73->tag != 39)goto
_LL697;else{_tmpD74=(void*)_tmpD73->f1;}}_LL696: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpD74,1);goto _LL67C;_LL697: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD75=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpD58;if(_tmpD75->tag != 19)goto
_LL699;else{_tmpD76=_tmpD75->f1;}}_LL698: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpD76,te,cvtenv);goto _LL67C;_LL699:;_LL69A: {const char*_tmp1512;void*_tmp1511;(
_tmp1511=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1512="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1512,sizeof(char),66))),_tag_dyneither(_tmp1511,sizeof(void*),
0)));}_LL67C:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpD79=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpD7B;struct
_tuple27 _tmpD7A=*((struct _tuple27*)vs->hd);_tmpD7B=_tmpD7A.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpD79,_tmpD7B);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpD7D;struct _tuple28 _tmpD7C=*((struct _tuple28*)evs->hd);_tmpD7D=_tmpD7C.f1;{
void*_tmpD7E=Cyc_Tcutil_compress(_tmpD7D);struct Cyc_Core_Opt*_tmpD80;struct Cyc_Core_Opt**
_tmpD81;_LL69C: {struct Cyc_Absyn_Evar_struct*_tmpD7F=(struct Cyc_Absyn_Evar_struct*)
_tmpD7E;if(_tmpD7F->tag != 1)goto _LL69E;else{_tmpD80=_tmpD7F->f4;_tmpD81=(struct
Cyc_Core_Opt**)& _tmpD7F->f4;}}_LL69D: if(*_tmpD81 == 0){struct Cyc_Core_Opt*
_tmp1513;*_tmpD81=((_tmp1513=_cycalloc(sizeof(*_tmp1513)),((_tmp1513->v=_tmpD79,
_tmp1513))));}else{struct Cyc_List_List*_tmpD83=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpD81))->v;struct Cyc_List_List*_tmpD84=0;for(0;_tmpD83 != 0;
_tmpD83=_tmpD83->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpD79,(struct Cyc_Absyn_Tvar*)_tmpD83->hd)){struct Cyc_List_List*_tmp1514;
_tmpD84=((_tmp1514=_cycalloc(sizeof(*_tmp1514)),((_tmp1514->hd=(struct Cyc_Absyn_Tvar*)
_tmpD83->hd,((_tmp1514->tl=_tmpD84,_tmp1514))))));}}{struct Cyc_Core_Opt*_tmp1515;*
_tmpD81=((_tmp1515=_cycalloc(sizeof(*_tmp1515)),((_tmp1515->v=_tmpD84,_tmp1515))));};}
goto _LL69B;_LL69E:;_LL69F: goto _LL69B;_LL69B:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpD87=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpD88=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp1516;struct Cyc_Tcutil_CVTEnv _tmpD89=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp1516.r=_tmpD88,((_tmp1516.kind_env=_tmpD87,((_tmp1516.free_vars=0,((_tmp1516.free_evars=
0,((_tmp1516.generalize_evars=generalize_evars,((_tmp1516.fn_result=0,_tmp1516)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpD8A=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple27*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpD88,(struct Cyc_Absyn_Tvar*(*)(struct _tuple27*))Cyc_Core_fst,
_tmpD89.free_vars);struct Cyc_List_List*_tmpD8B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpD88,(void*(*)(struct _tuple28*))Cyc_Core_fst,_tmpD89.free_evars);if(_tmpD87 != 
0){struct Cyc_List_List*_tmpD8C=0;{struct Cyc_List_List*_tmpD8D=_tmpD8A;for(0;(
unsigned int)_tmpD8D;_tmpD8D=_tmpD8D->tl){struct Cyc_Absyn_Tvar*_tmpD8E=(struct
Cyc_Absyn_Tvar*)_tmpD8D->hd;int found=0;{struct Cyc_List_List*_tmpD8F=_tmpD87;for(
0;(unsigned int)_tmpD8F;_tmpD8F=_tmpD8F->tl){if(Cyc_Absyn_tvar_cmp(_tmpD8E,(
struct Cyc_Absyn_Tvar*)_tmpD8F->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp1517;_tmpD8C=((_tmp1517=_region_malloc(_tmpD88,sizeof(*_tmp1517)),((_tmp1517->hd=(
struct Cyc_Absyn_Tvar*)_tmpD8D->hd,((_tmp1517->tl=_tmpD8C,_tmp1517))))));}}}
_tmpD8A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpD8C);}{struct Cyc_List_List*x=_tmpD8A;for(0;x != 0;x=x->tl){void*_tmpD91=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpD93;struct Cyc_Core_Opt**
_tmpD94;struct Cyc_Core_Opt*_tmpD96;struct Cyc_Core_Opt**_tmpD97;struct Cyc_Absyn_Kind*
_tmpD98;struct Cyc_Absyn_Kind _tmpD99;enum Cyc_Absyn_KindQual _tmpD9A;enum Cyc_Absyn_AliasQual
_tmpD9B;struct Cyc_Core_Opt*_tmpD9D;struct Cyc_Core_Opt**_tmpD9E;struct Cyc_Absyn_Kind*
_tmpD9F;struct Cyc_Absyn_Kind _tmpDA0;enum Cyc_Absyn_KindQual _tmpDA1;enum Cyc_Absyn_AliasQual
_tmpDA2;struct Cyc_Core_Opt*_tmpDA4;struct Cyc_Core_Opt**_tmpDA5;struct Cyc_Absyn_Kind*
_tmpDA6;struct Cyc_Absyn_Kind _tmpDA7;enum Cyc_Absyn_KindQual _tmpDA8;enum Cyc_Absyn_AliasQual
_tmpDA9;struct Cyc_Core_Opt*_tmpDAB;struct Cyc_Core_Opt**_tmpDAC;struct Cyc_Absyn_Kind*
_tmpDAD;struct Cyc_Absyn_Kind _tmpDAE;enum Cyc_Absyn_KindQual _tmpDAF;enum Cyc_Absyn_AliasQual
_tmpDB0;struct Cyc_Core_Opt*_tmpDB2;struct Cyc_Core_Opt**_tmpDB3;struct Cyc_Absyn_Kind*
_tmpDB4;struct Cyc_Absyn_Kind _tmpDB5;enum Cyc_Absyn_KindQual _tmpDB6;enum Cyc_Absyn_AliasQual
_tmpDB7;struct Cyc_Core_Opt*_tmpDB9;struct Cyc_Core_Opt**_tmpDBA;struct Cyc_Absyn_Kind*
_tmpDBB;struct Cyc_Absyn_Kind*_tmpDBD;struct Cyc_Absyn_Kind _tmpDBE;enum Cyc_Absyn_KindQual
_tmpDBF;enum Cyc_Absyn_AliasQual _tmpDC0;_LL6A1: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpD92=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpD91;if(_tmpD92->tag != 1)goto
_LL6A3;else{_tmpD93=_tmpD92->f1;_tmpD94=(struct Cyc_Core_Opt**)& _tmpD92->f1;}}
_LL6A2: _tmpD97=_tmpD94;goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Less_kb_struct*
_tmpD95=(struct Cyc_Absyn_Less_kb_struct*)_tmpD91;if(_tmpD95->tag != 2)goto _LL6A5;
else{_tmpD96=_tmpD95->f1;_tmpD97=(struct Cyc_Core_Opt**)& _tmpD95->f1;_tmpD98=
_tmpD95->f2;_tmpD99=*_tmpD98;_tmpD9A=_tmpD99.kind;if(_tmpD9A != Cyc_Absyn_BoxKind)
goto _LL6A5;_tmpD9B=_tmpD99.aliasqual;if(_tmpD9B != Cyc_Absyn_Top)goto _LL6A5;}}
_LL6A4: _tmpD9E=_tmpD97;goto _LL6A6;_LL6A5: {struct Cyc_Absyn_Less_kb_struct*
_tmpD9C=(struct Cyc_Absyn_Less_kb_struct*)_tmpD91;if(_tmpD9C->tag != 2)goto _LL6A7;
else{_tmpD9D=_tmpD9C->f1;_tmpD9E=(struct Cyc_Core_Opt**)& _tmpD9C->f1;_tmpD9F=
_tmpD9C->f2;_tmpDA0=*_tmpD9F;_tmpDA1=_tmpDA0.kind;if(_tmpDA1 != Cyc_Absyn_MemKind)
goto _LL6A7;_tmpDA2=_tmpDA0.aliasqual;if(_tmpDA2 != Cyc_Absyn_Top)goto _LL6A7;}}
_LL6A6: _tmpDA5=_tmpD9E;goto _LL6A8;_LL6A7: {struct Cyc_Absyn_Less_kb_struct*
_tmpDA3=(struct Cyc_Absyn_Less_kb_struct*)_tmpD91;if(_tmpDA3->tag != 2)goto _LL6A9;
else{_tmpDA4=_tmpDA3->f1;_tmpDA5=(struct Cyc_Core_Opt**)& _tmpDA3->f1;_tmpDA6=
_tmpDA3->f2;_tmpDA7=*_tmpDA6;_tmpDA8=_tmpDA7.kind;if(_tmpDA8 != Cyc_Absyn_MemKind)
goto _LL6A9;_tmpDA9=_tmpDA7.aliasqual;if(_tmpDA9 != Cyc_Absyn_Aliasable)goto _LL6A9;}}
_LL6A8:*_tmpDA5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6A0;_LL6A9: {
struct Cyc_Absyn_Less_kb_struct*_tmpDAA=(struct Cyc_Absyn_Less_kb_struct*)_tmpD91;
if(_tmpDAA->tag != 2)goto _LL6AB;else{_tmpDAB=_tmpDAA->f1;_tmpDAC=(struct Cyc_Core_Opt**)&
_tmpDAA->f1;_tmpDAD=_tmpDAA->f2;_tmpDAE=*_tmpDAD;_tmpDAF=_tmpDAE.kind;if(_tmpDAF
!= Cyc_Absyn_MemKind)goto _LL6AB;_tmpDB0=_tmpDAE.aliasqual;if(_tmpDB0 != Cyc_Absyn_Unique)
goto _LL6AB;}}_LL6AA:*_tmpDAC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6A0;_LL6AB: {struct Cyc_Absyn_Less_kb_struct*_tmpDB1=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD91;if(_tmpDB1->tag != 2)goto _LL6AD;else{_tmpDB2=_tmpDB1->f1;_tmpDB3=(struct
Cyc_Core_Opt**)& _tmpDB1->f1;_tmpDB4=_tmpDB1->f2;_tmpDB5=*_tmpDB4;_tmpDB6=_tmpDB5.kind;
if(_tmpDB6 != Cyc_Absyn_RgnKind)goto _LL6AD;_tmpDB7=_tmpDB5.aliasqual;if(_tmpDB7 != 
Cyc_Absyn_Top)goto _LL6AD;}}_LL6AC:*_tmpDB3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6A0;_LL6AD: {struct Cyc_Absyn_Less_kb_struct*_tmpDB8=(struct Cyc_Absyn_Less_kb_struct*)
_tmpD91;if(_tmpDB8->tag != 2)goto _LL6AF;else{_tmpDB9=_tmpDB8->f1;_tmpDBA=(struct
Cyc_Core_Opt**)& _tmpDB8->f1;_tmpDBB=_tmpDB8->f2;}}_LL6AE:*_tmpDBA=Cyc_Tcutil_kind_to_bound_opt(
_tmpDBB);goto _LL6A0;_LL6AF: {struct Cyc_Absyn_Eq_kb_struct*_tmpDBC=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpD91;if(_tmpDBC->tag != 0)goto _LL6B1;else{_tmpDBD=_tmpDBC->f1;_tmpDBE=*_tmpDBD;
_tmpDBF=_tmpDBE.kind;if(_tmpDBF != Cyc_Absyn_MemKind)goto _LL6B1;_tmpDC0=_tmpDBE.aliasqual;}}
_LL6B0:{const char*_tmp151E;void*_tmp151D[2];struct Cyc_String_pa_struct _tmp151C;
struct Cyc_Absyn_Kind*_tmp151B;struct Cyc_String_pa_struct _tmp151A;(_tmp151A.tag=0,((
_tmp151A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp151B=_cycalloc_atomic(sizeof(*_tmp151B)),((_tmp151B->kind=Cyc_Absyn_MemKind,((
_tmp151B->aliasqual=_tmpDC0,_tmp151B)))))))),((_tmp151C.tag=0,((_tmp151C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp151D[0]=& _tmp151C,((_tmp151D[1]=& _tmp151A,Cyc_Tcutil_terr(loc,((
_tmp151E="type variable %s cannot have kind %s",_tag_dyneither(_tmp151E,sizeof(
char),37))),_tag_dyneither(_tmp151D,sizeof(void*),2)))))))))))));}goto _LL6A0;
_LL6B1:;_LL6B2: goto _LL6A0;_LL6A0:;}}if(_tmpD8A != 0  || _tmpD8B != 0){{void*_tmpDC6=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpDC8;struct Cyc_List_List*_tmpDC9;
struct Cyc_List_List**_tmpDCA;struct Cyc_Core_Opt*_tmpDCB;void*_tmpDCC;struct Cyc_List_List*
_tmpDCD;int _tmpDCE;struct Cyc_Absyn_VarargInfo*_tmpDCF;struct Cyc_List_List*
_tmpDD0;struct Cyc_List_List*_tmpDD1;_LL6B4: {struct Cyc_Absyn_FnType_struct*
_tmpDC7=(struct Cyc_Absyn_FnType_struct*)_tmpDC6;if(_tmpDC7->tag != 10)goto _LL6B6;
else{_tmpDC8=_tmpDC7->f1;_tmpDC9=_tmpDC8.tvars;_tmpDCA=(struct Cyc_List_List**)&(
_tmpDC7->f1).tvars;_tmpDCB=_tmpDC8.effect;_tmpDCC=_tmpDC8.ret_typ;_tmpDCD=
_tmpDC8.args;_tmpDCE=_tmpDC8.c_varargs;_tmpDCF=_tmpDC8.cyc_varargs;_tmpDD0=
_tmpDC8.rgn_po;_tmpDD1=_tmpDC8.attributes;}}_LL6B5: if(*_tmpDCA == 0){*_tmpDCA=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpD8A);_tmpD8A=0;}
goto _LL6B3;_LL6B6:;_LL6B7: goto _LL6B3;_LL6B3:;}if(_tmpD8A != 0){const char*_tmp1522;
void*_tmp1521[1];struct Cyc_String_pa_struct _tmp1520;(_tmp1520.tag=0,((_tmp1520.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpD8A->hd)->name),((
_tmp1521[0]=& _tmp1520,Cyc_Tcutil_terr(loc,((_tmp1522="unbound type variable %s",
_tag_dyneither(_tmp1522,sizeof(char),25))),_tag_dyneither(_tmp1521,sizeof(void*),
1)))))));}if(_tmpD8B != 0)for(0;_tmpD8B != 0;_tmpD8B=_tmpD8B->tl){void*e=(void*)
_tmpD8B->hd;struct Cyc_Absyn_Kind*_tmpDD5=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpDD6;enum Cyc_Absyn_KindQual _tmpDD7;enum Cyc_Absyn_AliasQual _tmpDD8;struct Cyc_Absyn_Kind
_tmpDD9;enum Cyc_Absyn_KindQual _tmpDDA;enum Cyc_Absyn_AliasQual _tmpDDB;struct Cyc_Absyn_Kind
_tmpDDC;enum Cyc_Absyn_KindQual _tmpDDD;enum Cyc_Absyn_AliasQual _tmpDDE;struct Cyc_Absyn_Kind
_tmpDDF;enum Cyc_Absyn_KindQual _tmpDE0;_LL6B9: _tmpDD6=*_tmpDD5;_tmpDD7=_tmpDD6.kind;
if(_tmpDD7 != Cyc_Absyn_RgnKind)goto _LL6BB;_tmpDD8=_tmpDD6.aliasqual;if(_tmpDD8 != 
Cyc_Absyn_Unique)goto _LL6BB;_LL6BA: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1525;void*_tmp1524;(_tmp1524=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1525="can't unify evar with unique region!",
_tag_dyneither(_tmp1525,sizeof(char),37))),_tag_dyneither(_tmp1524,sizeof(void*),
0)));}goto _LL6B8;_LL6BB: _tmpDD9=*_tmpDD5;_tmpDDA=_tmpDD9.kind;if(_tmpDDA != Cyc_Absyn_RgnKind)
goto _LL6BD;_tmpDDB=_tmpDD9.aliasqual;if(_tmpDDB != Cyc_Absyn_Aliasable)goto _LL6BD;
_LL6BC: goto _LL6BE;_LL6BD: _tmpDDC=*_tmpDD5;_tmpDDD=_tmpDDC.kind;if(_tmpDDD != Cyc_Absyn_RgnKind)
goto _LL6BF;_tmpDDE=_tmpDDC.aliasqual;if(_tmpDDE != Cyc_Absyn_Top)goto _LL6BF;
_LL6BE: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1528;
void*_tmp1527;(_tmp1527=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1528="can't unify evar with heap!",_tag_dyneither(
_tmp1528,sizeof(char),28))),_tag_dyneither(_tmp1527,sizeof(void*),0)));}goto
_LL6B8;_LL6BF: _tmpDDF=*_tmpDD5;_tmpDE0=_tmpDDF.kind;if(_tmpDE0 != Cyc_Absyn_EffKind)
goto _LL6C1;_LL6C0: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp152B;void*_tmp152A;(_tmp152A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp152B="can't unify evar with {}!",
_tag_dyneither(_tmp152B,sizeof(char),26))),_tag_dyneither(_tmp152A,sizeof(void*),
0)));}goto _LL6B8;_LL6C1:;_LL6C2:{const char*_tmp1530;void*_tmp152F[2];struct Cyc_String_pa_struct
_tmp152E;struct Cyc_String_pa_struct _tmp152D;(_tmp152D.tag=0,((_tmp152D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp152E.tag=0,((_tmp152E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp152F[0]=& _tmp152E,((_tmp152F[1]=& _tmp152D,Cyc_Tcutil_terr(loc,((
_tmp1530="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1530,sizeof(char),52))),_tag_dyneither(_tmp152F,sizeof(void*),2)))))))))))));}
goto _LL6B8;_LL6B8:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpDEC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpDEE;struct Cyc_List_List*_tmpDEF;struct Cyc_Core_Opt*
_tmpDF0;void*_tmpDF1;_LL6C4: {struct Cyc_Absyn_FnType_struct*_tmpDED=(struct Cyc_Absyn_FnType_struct*)
_tmpDEC;if(_tmpDED->tag != 10)goto _LL6C6;else{_tmpDEE=_tmpDED->f1;_tmpDEF=_tmpDEE.tvars;
_tmpDF0=_tmpDEE.effect;_tmpDF1=_tmpDEE.ret_typ;}}_LL6C5: fd->tvs=_tmpDEF;fd->effect=
_tmpDF0;goto _LL6C3;_LL6C6:;_LL6C7: {const char*_tmp1533;void*_tmp1532;(_tmp1532=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1533="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1533,sizeof(
char),38))),_tag_dyneither(_tmp1532,sizeof(void*),0)));}_LL6C3:;}{struct
_RegionHandle*_tmpDF4=Cyc_Tcenv_get_fnrgn(te);{const char*_tmp1534;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDF4,(struct _dyneither_ptr*(*)(
struct _tuple23*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,((_tmp1534="function declaration has repeated parameter",
_tag_dyneither(_tmp1534,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp1535;fd->cached_typ=((
_tmp1535=_cycalloc(sizeof(*_tmp1535)),((_tmp1535->v=t,_tmp1535))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpDF7=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp1536;
struct Cyc_Tcutil_CVTEnv _tmpDF8=Cyc_Tcutil_check_valid_type(loc,te,((_tmp1536.r=
_tmpDF7,((_tmp1536.kind_env=bound_tvars,((_tmp1536.free_vars=0,((_tmp1536.free_evars=
0,((_tmp1536.generalize_evars=0,((_tmp1536.fn_result=0,_tmp1536)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpDF9=Cyc_Tcutil_remove_bound_tvars(
_tmpDF7,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple27*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDF7,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple27*))Cyc_Core_fst,_tmpDF8.free_vars),bound_tvars);struct Cyc_List_List*
_tmpDFA=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple28*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDF7,(void*(*)(struct _tuple28*))Cyc_Core_fst,
_tmpDF8.free_evars);{struct Cyc_List_List*fs=_tmpDF9;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpDFB=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp153B;void*_tmp153A[2];struct Cyc_String_pa_struct _tmp1539;struct Cyc_String_pa_struct
_tmp1538;(_tmp1538.tag=0,((_tmp1538.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1539.tag=0,((_tmp1539.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpDFB),((_tmp153A[0]=& _tmp1539,((
_tmp153A[1]=& _tmp1538,Cyc_Tcutil_terr(loc,((_tmp153B="unbound type variable %s in type %s",
_tag_dyneither(_tmp153B,sizeof(char),36))),_tag_dyneither(_tmp153A,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpDFA != 0)for(0;_tmpDFA != 0;_tmpDFA=
_tmpDFA->tl){void*e=(void*)_tmpDFA->hd;struct Cyc_Absyn_Kind*_tmpE00=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE01;enum Cyc_Absyn_KindQual _tmpE02;enum Cyc_Absyn_AliasQual
_tmpE03;struct Cyc_Absyn_Kind _tmpE04;enum Cyc_Absyn_KindQual _tmpE05;enum Cyc_Absyn_AliasQual
_tmpE06;struct Cyc_Absyn_Kind _tmpE07;enum Cyc_Absyn_KindQual _tmpE08;enum Cyc_Absyn_AliasQual
_tmpE09;struct Cyc_Absyn_Kind _tmpE0A;enum Cyc_Absyn_KindQual _tmpE0B;_LL6C9:
_tmpE01=*_tmpE00;_tmpE02=_tmpE01.kind;if(_tmpE02 != Cyc_Absyn_RgnKind)goto _LL6CB;
_tmpE03=_tmpE01.aliasqual;if(_tmpE03 != Cyc_Absyn_Unique)goto _LL6CB;_LL6CA: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp153E;void*
_tmp153D;(_tmp153D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp153E="can't unify evar with unique region!",
_tag_dyneither(_tmp153E,sizeof(char),37))),_tag_dyneither(_tmp153D,sizeof(void*),
0)));}goto _LL6C8;_LL6CB: _tmpE04=*_tmpE00;_tmpE05=_tmpE04.kind;if(_tmpE05 != Cyc_Absyn_RgnKind)
goto _LL6CD;_tmpE06=_tmpE04.aliasqual;if(_tmpE06 != Cyc_Absyn_Aliasable)goto _LL6CD;
_LL6CC: goto _LL6CE;_LL6CD: _tmpE07=*_tmpE00;_tmpE08=_tmpE07.kind;if(_tmpE08 != Cyc_Absyn_RgnKind)
goto _LL6CF;_tmpE09=_tmpE07.aliasqual;if(_tmpE09 != Cyc_Absyn_Top)goto _LL6CF;
_LL6CE: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1541;
void*_tmp1540;(_tmp1540=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1541="can't unify evar with heap!",_tag_dyneither(
_tmp1541,sizeof(char),28))),_tag_dyneither(_tmp1540,sizeof(void*),0)));}goto
_LL6C8;_LL6CF: _tmpE0A=*_tmpE00;_tmpE0B=_tmpE0A.kind;if(_tmpE0B != Cyc_Absyn_EffKind)
goto _LL6D1;_LL6D0: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1544;void*_tmp1543;(_tmp1543=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1544="can't unify evar with {}!",
_tag_dyneither(_tmp1544,sizeof(char),26))),_tag_dyneither(_tmp1543,sizeof(void*),
0)));}goto _LL6C8;_LL6D1:;_LL6D2:{const char*_tmp1549;void*_tmp1548[2];struct Cyc_String_pa_struct
_tmp1547;struct Cyc_String_pa_struct _tmp1546;(_tmp1546.tag=0,((_tmp1546.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1547.tag=0,((_tmp1547.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1548[0]=& _tmp1547,((_tmp1548[1]=& _tmp1546,Cyc_Tcutil_terr(loc,((
_tmp1549="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1549,sizeof(char),52))),_tag_dyneither(_tmp1548,sizeof(void*),2)))))))))))));}
goto _LL6C8;_LL6C8:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp154E;void*_tmp154D[2];struct Cyc_String_pa_struct
_tmp154C;struct Cyc_String_pa_struct _tmp154B;(_tmp154B.tag=0,((_tmp154B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp154C.tag=
0,((_tmp154C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp154D[0]=&
_tmp154C,((_tmp154D[1]=& _tmp154B,Cyc_Tcutil_terr(loc,((_tmp154E="%s: %s",
_tag_dyneither(_tmp154E,sizeof(char),7))),_tag_dyneither(_tmp154D,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp154F;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp154F="duplicate type variable",
_tag_dyneither(_tmp154F,sizeof(char),24))));}struct _tuple30{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple31{struct Cyc_List_List*f1;void*f2;};struct _tuple32{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp1550;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1550="",_tag_dyneither(_tmp1550,
sizeof(char),1))))!= 0){struct _tuple30*_tmp1553;struct Cyc_List_List*_tmp1552;
fields=((_tmp1552=_cycalloc(sizeof(*_tmp1552)),((_tmp1552->hd=((_tmp1553=
_cycalloc(sizeof(*_tmp1553)),((_tmp1553->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp1553->f2=0,_tmp1553)))))),((_tmp1552->tl=fields,_tmp1552))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp1555;const char*_tmp1554;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp1555="struct",_tag_dyneither(_tmp1555,sizeof(char),7)):((
_tmp1554="union",_tag_dyneither(_tmp1554,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple31 _tmpE20;struct Cyc_List_List*
_tmpE21;void*_tmpE22;struct _tuple31*_tmpE1F=(struct _tuple31*)des->hd;_tmpE20=*
_tmpE1F;_tmpE21=_tmpE20.f1;_tmpE22=_tmpE20.f2;if(_tmpE21 == 0){struct Cyc_List_List*
_tmpE23=fields;for(0;_tmpE23 != 0;_tmpE23=_tmpE23->tl){if(!(*((struct _tuple30*)
_tmpE23->hd)).f2){(*((struct _tuple30*)_tmpE23->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp155B;struct Cyc_Absyn_FieldName_struct _tmp155A;struct Cyc_List_List*_tmp1559;(*((
struct _tuple31*)des->hd)).f1=(struct Cyc_List_List*)((_tmp1559=_cycalloc(sizeof(*
_tmp1559)),((_tmp1559->hd=(void*)((_tmp155B=_cycalloc(sizeof(*_tmp155B)),((
_tmp155B[0]=((_tmp155A.tag=1,((_tmp155A.f1=((*((struct _tuple30*)_tmpE23->hd)).f1)->name,
_tmp155A)))),_tmp155B)))),((_tmp1559->tl=0,_tmp1559))))));}{struct _tuple32*
_tmp155E;struct Cyc_List_List*_tmp155D;ans=((_tmp155D=_region_malloc(rgn,sizeof(*
_tmp155D)),((_tmp155D->hd=((_tmp155E=_region_malloc(rgn,sizeof(*_tmp155E)),((
_tmp155E->f1=(*((struct _tuple30*)_tmpE23->hd)).f1,((_tmp155E->f2=_tmpE22,
_tmp155E)))))),((_tmp155D->tl=ans,_tmp155D))))));}break;}}if(_tmpE23 == 0){const
char*_tmp1562;void*_tmp1561[1];struct Cyc_String_pa_struct _tmp1560;(_tmp1560.tag=
0,((_tmp1560.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp1561[0]=& _tmp1560,Cyc_Tcutil_terr(loc,((_tmp1562="too many arguments to %s",
_tag_dyneither(_tmp1562,sizeof(char),25))),_tag_dyneither(_tmp1561,sizeof(void*),
1)))))));}}else{if(_tmpE21->tl != 0){const char*_tmp1565;void*_tmp1564;(_tmp1564=0,
Cyc_Tcutil_terr(loc,((_tmp1565="multiple designators are not yet supported",
_tag_dyneither(_tmp1565,sizeof(char),43))),_tag_dyneither(_tmp1564,sizeof(void*),
0)));}else{void*_tmpE2E=(void*)_tmpE21->hd;struct _dyneither_ptr*_tmpE31;_LL6D4: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE2F=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE2E;if(_tmpE2F->tag != 0)goto _LL6D6;}_LL6D5:{const char*_tmp1569;void*_tmp1568[
1];struct Cyc_String_pa_struct _tmp1567;(_tmp1567.tag=0,((_tmp1567.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1568[0]=& _tmp1567,Cyc_Tcutil_terr(
loc,((_tmp1569="array designator used in argument to %s",_tag_dyneither(_tmp1569,
sizeof(char),40))),_tag_dyneither(_tmp1568,sizeof(void*),1)))))));}goto _LL6D3;
_LL6D6: {struct Cyc_Absyn_FieldName_struct*_tmpE30=(struct Cyc_Absyn_FieldName_struct*)
_tmpE2E;if(_tmpE30->tag != 1)goto _LL6D3;else{_tmpE31=_tmpE30->f1;}}_LL6D7: {
struct Cyc_List_List*_tmpE35=fields;for(0;_tmpE35 != 0;_tmpE35=_tmpE35->tl){if(Cyc_strptrcmp(
_tmpE31,((*((struct _tuple30*)_tmpE35->hd)).f1)->name)== 0){if((*((struct _tuple30*)
_tmpE35->hd)).f2){const char*_tmp156D;void*_tmp156C[1];struct Cyc_String_pa_struct
_tmp156B;(_tmp156B.tag=0,((_tmp156B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE31),((_tmp156C[0]=& _tmp156B,Cyc_Tcutil_terr(loc,((_tmp156D="member %s has already been used as an argument",
_tag_dyneither(_tmp156D,sizeof(char),47))),_tag_dyneither(_tmp156C,sizeof(void*),
1)))))));}(*((struct _tuple30*)_tmpE35->hd)).f2=1;{struct _tuple32*_tmp1570;struct
Cyc_List_List*_tmp156F;ans=((_tmp156F=_region_malloc(rgn,sizeof(*_tmp156F)),((
_tmp156F->hd=((_tmp1570=_region_malloc(rgn,sizeof(*_tmp1570)),((_tmp1570->f1=(*((
struct _tuple30*)_tmpE35->hd)).f1,((_tmp1570->f2=_tmpE22,_tmp1570)))))),((
_tmp156F->tl=ans,_tmp156F))))));}break;}}if(_tmpE35 == 0){const char*_tmp1574;void*
_tmp1573[1];struct Cyc_String_pa_struct _tmp1572;(_tmp1572.tag=0,((_tmp1572.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE31),((_tmp1573[0]=& _tmp1572,
Cyc_Tcutil_terr(loc,((_tmp1574="bad field designator %s",_tag_dyneither(_tmp1574,
sizeof(char),24))),_tag_dyneither(_tmp1573,sizeof(void*),1)))))));}goto _LL6D3;}
_LL6D3:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple30*)fields->hd)).f2){{const char*_tmp1577;void*_tmp1576;(
_tmp1576=0,Cyc_Tcutil_terr(loc,((_tmp1577="too few arguments to struct",
_tag_dyneither(_tmp1577,sizeof(char),28))),_tag_dyneither(_tmp1576,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple30*)fields->hd)).f2){if(found){const char*_tmp157A;void*_tmp1579;(_tmp1579=
0,Cyc_Tcutil_terr(loc,((_tmp157A="only one member of a union is allowed",
_tag_dyneither(_tmp157A,sizeof(char),38))),_tag_dyneither(_tmp1579,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp157D;void*_tmp157C;(_tmp157C=0,Cyc_Tcutil_terr(
loc,((_tmp157D="missing member for union",_tag_dyneither(_tmp157D,sizeof(char),
25))),_tag_dyneither(_tmp157C,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE46=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE48;void*_tmpE49;struct Cyc_Absyn_PtrAtts _tmpE4A;union Cyc_Absyn_Constraint*
_tmpE4B;_LL6D9: {struct Cyc_Absyn_PointerType_struct*_tmpE47=(struct Cyc_Absyn_PointerType_struct*)
_tmpE46;if(_tmpE47->tag != 5)goto _LL6DB;else{_tmpE48=_tmpE47->f1;_tmpE49=_tmpE48.elt_typ;
_tmpE4A=_tmpE48.ptr_atts;_tmpE4B=_tmpE4A.bounds;}}_LL6DA: {void*_tmpE4C=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE4B);_LL6DE: {struct Cyc_Absyn_DynEither_b_struct*_tmpE4D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE4C;if(_tmpE4D->tag != 0)goto _LL6E0;}_LL6DF:*elt_typ_dest=_tmpE49;return 1;
_LL6E0:;_LL6E1: return 0;_LL6DD:;}_LL6DB:;_LL6DC: return 0;_LL6D8:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE4E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE50;void*_tmpE51;struct Cyc_Absyn_PtrAtts _tmpE52;union Cyc_Absyn_Constraint*
_tmpE53;_LL6E3: {struct Cyc_Absyn_PointerType_struct*_tmpE4F=(struct Cyc_Absyn_PointerType_struct*)
_tmpE4E;if(_tmpE4F->tag != 5)goto _LL6E5;else{_tmpE50=_tmpE4F->f1;_tmpE51=_tmpE50.elt_typ;
_tmpE52=_tmpE50.ptr_atts;_tmpE53=_tmpE52.zero_term;}}_LL6E4:*elt_typ_dest=
_tmpE51;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpE53);_LL6E5:;_LL6E6: return 0;_LL6E2:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE54=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE56;void*_tmpE57;struct Cyc_Absyn_PtrAtts _tmpE58;
union Cyc_Absyn_Constraint*_tmpE59;union Cyc_Absyn_Constraint*_tmpE5A;struct Cyc_Absyn_ArrayInfo
_tmpE5C;void*_tmpE5D;struct Cyc_Absyn_Tqual _tmpE5E;struct Cyc_Absyn_Exp*_tmpE5F;
union Cyc_Absyn_Constraint*_tmpE60;_LL6E8: {struct Cyc_Absyn_PointerType_struct*
_tmpE55=(struct Cyc_Absyn_PointerType_struct*)_tmpE54;if(_tmpE55->tag != 5)goto
_LL6EA;else{_tmpE56=_tmpE55->f1;_tmpE57=_tmpE56.elt_typ;_tmpE58=_tmpE56.ptr_atts;
_tmpE59=_tmpE58.bounds;_tmpE5A=_tmpE58.zero_term;}}_LL6E9: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE5A)){*ptr_type=t;*elt_type=
_tmpE57;{void*_tmpE61=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpE59);_LL6EF: {struct Cyc_Absyn_DynEither_b_struct*
_tmpE62=(struct Cyc_Absyn_DynEither_b_struct*)_tmpE61;if(_tmpE62->tag != 0)goto
_LL6F1;}_LL6F0:*is_dyneither=1;goto _LL6EE;_LL6F1:;_LL6F2:*is_dyneither=0;goto
_LL6EE;_LL6EE:;}return 1;}else{return 0;}_LL6EA: {struct Cyc_Absyn_ArrayType_struct*
_tmpE5B=(struct Cyc_Absyn_ArrayType_struct*)_tmpE54;if(_tmpE5B->tag != 9)goto
_LL6EC;else{_tmpE5C=_tmpE5B->f1;_tmpE5D=_tmpE5C.elt_type;_tmpE5E=_tmpE5C.tq;
_tmpE5F=_tmpE5C.num_elts;_tmpE60=_tmpE5C.zero_term;}}_LL6EB: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE60)){*elt_type=_tmpE5D;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1592;struct Cyc_Absyn_PtrAtts
_tmp1591;struct Cyc_Absyn_Upper_b_struct _tmp1590;struct Cyc_Absyn_Upper_b_struct*
_tmp158F;struct Cyc_Absyn_PtrInfo _tmp158E;struct Cyc_Absyn_PointerType_struct*
_tmp158D;*ptr_type=(void*)((_tmp158D=_cycalloc(sizeof(*_tmp158D)),((_tmp158D[0]=((
_tmp1592.tag=5,((_tmp1592.f1=((_tmp158E.elt_typ=_tmpE5D,((_tmp158E.elt_tq=
_tmpE5E,((_tmp158E.ptr_atts=((_tmp1591.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1591.nullable=Cyc_Absyn_false_conref,((_tmp1591.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp158F=_cycalloc(sizeof(*_tmp158F)),((
_tmp158F[0]=((_tmp1590.tag=1,((_tmp1590.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpE5F),_tmp1590)))),_tmp158F))))),((_tmp1591.zero_term=_tmpE60,((_tmp1591.ptrloc=
0,_tmp1591)))))))))),_tmp158E)))))),_tmp1592)))),_tmp158D))));}return 1;}else{
return 0;}_LL6EC:;_LL6ED: return 0;_LL6E7:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpE69=e1->r;
struct Cyc_Absyn_Exp*_tmpE6C;struct Cyc_Absyn_Exp*_tmpE6E;struct Cyc_Absyn_Exp*
_tmpE70;struct Cyc_Absyn_Exp*_tmpE72;struct Cyc_Absyn_Exp*_tmpE74;struct Cyc_Absyn_Exp*
_tmpE76;_LL6F4: {struct Cyc_Absyn_Cast_e_struct*_tmpE6A=(struct Cyc_Absyn_Cast_e_struct*)
_tmpE69;if(_tmpE6A->tag != 15)goto _LL6F6;}_LL6F5: {const char*_tmp1596;void*
_tmp1595[1];struct Cyc_String_pa_struct _tmp1594;(_tmp1594.tag=0,((_tmp1594.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1595[0]=& _tmp1594,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1596="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1596,sizeof(char),29))),_tag_dyneither(_tmp1595,sizeof(void*),1)))))));}
_LL6F6: {struct Cyc_Absyn_Deref_e_struct*_tmpE6B=(struct Cyc_Absyn_Deref_e_struct*)
_tmpE69;if(_tmpE6B->tag != 21)goto _LL6F8;else{_tmpE6C=_tmpE6B->f1;}}_LL6F7:
_tmpE6E=_tmpE6C;goto _LL6F9;_LL6F8: {struct Cyc_Absyn_Subscript_e_struct*_tmpE6D=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpE69;if(_tmpE6D->tag != 24)goto _LL6FA;
else{_tmpE6E=_tmpE6D->f1;}}_LL6F9: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpE6E->topt))->v,ptr_type,is_dyneither,
elt_type);_LL6FA: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpE6F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE69;if(_tmpE6F->tag != 23)goto _LL6FC;else{_tmpE70=_tmpE6F->f1;}}_LL6FB:
_tmpE72=_tmpE70;goto _LL6FD;_LL6FC: {struct Cyc_Absyn_AggrMember_e_struct*_tmpE71=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpE69;if(_tmpE71->tag != 22)goto _LL6FE;
else{_tmpE72=_tmpE71->f1;}}_LL6FD: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpE72->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp159A;void*_tmp1599[1];struct Cyc_String_pa_struct _tmp1598;(_tmp1598.tag=
0,((_tmp1598.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1599[0]=& _tmp1598,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159A="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp159A,sizeof(char),51))),_tag_dyneither(_tmp1599,sizeof(void*),
1)))))));}return 0;_LL6FE: {struct Cyc_Absyn_Instantiate_e_struct*_tmpE73=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpE69;if(_tmpE73->tag != 14)goto _LL700;else{
_tmpE74=_tmpE73->f1;}}_LL6FF: _tmpE76=_tmpE74;goto _LL701;_LL700: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpE75=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE69;if(_tmpE75->tag != 13)
goto _LL702;else{_tmpE76=_tmpE75->f1;}}_LL701: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE76->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp159E;void*_tmp159D[1];struct Cyc_String_pa_struct
_tmp159C;(_tmp159C.tag=0,((_tmp159C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp159D[0]=& _tmp159C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159E="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp159E,sizeof(char),49))),_tag_dyneither(_tmp159D,sizeof(void*),
1)))))));}return 0;_LL702: {struct Cyc_Absyn_Var_e_struct*_tmpE77=(struct Cyc_Absyn_Var_e_struct*)
_tmpE69;if(_tmpE77->tag != 1)goto _LL704;}_LL703: return 0;_LL704:;_LL705: {const
char*_tmp15A2;void*_tmp15A1[1];struct Cyc_String_pa_struct _tmp15A0;(_tmp15A0.tag=
0,((_tmp15A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15A1[0]=& _tmp15A0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A2="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp15A2,sizeof(char),39))),_tag_dyneither(_tmp15A1,sizeof(void*),
1)))))));}_LL6F3:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
static int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpE84=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpE87;_LL707: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpE85=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpE84;if(_tmpE85->tag != 22)goto
_LL709;}_LL708: return 1;_LL709: {struct Cyc_Absyn_VarType_struct*_tmpE86=(struct
Cyc_Absyn_VarType_struct*)_tmpE84;if(_tmpE86->tag != 2)goto _LL70B;else{_tmpE87=
_tmpE86->f1;}}_LL70A: {struct Cyc_Absyn_Kind _tmpE89;enum Cyc_Absyn_KindQual
_tmpE8A;enum Cyc_Absyn_AliasQual _tmpE8B;struct Cyc_Absyn_Kind*_tmpE88=Cyc_Tcutil_tvar_kind(
_tmpE87,& Cyc_Tcutil_rk);_tmpE89=*_tmpE88;_tmpE8A=_tmpE89.kind;_tmpE8B=_tmpE89.aliasqual;
if(_tmpE8A == Cyc_Absyn_RgnKind  && (_tmpE8B == Cyc_Absyn_Unique  || _tmpE8B == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpE8C=Cyc_Absyn_compress_kb(_tmpE87->kind);struct
Cyc_Core_Opt*_tmpE8E;struct Cyc_Core_Opt**_tmpE8F;struct Cyc_Absyn_Kind*_tmpE90;
struct Cyc_Absyn_Kind _tmpE91;enum Cyc_Absyn_KindQual _tmpE92;enum Cyc_Absyn_AliasQual
_tmpE93;_LL70E: {struct Cyc_Absyn_Less_kb_struct*_tmpE8D=(struct Cyc_Absyn_Less_kb_struct*)
_tmpE8C;if(_tmpE8D->tag != 2)goto _LL710;else{_tmpE8E=_tmpE8D->f1;_tmpE8F=(struct
Cyc_Core_Opt**)& _tmpE8D->f1;_tmpE90=_tmpE8D->f2;_tmpE91=*_tmpE90;_tmpE92=_tmpE91.kind;
if(_tmpE92 != Cyc_Absyn_RgnKind)goto _LL710;_tmpE93=_tmpE91.aliasqual;if(_tmpE93 != 
Cyc_Absyn_Top)goto _LL710;}}_LL70F:{struct Cyc_Absyn_Less_kb_struct*_tmp15A8;
struct Cyc_Absyn_Less_kb_struct _tmp15A7;struct Cyc_Core_Opt*_tmp15A6;*_tmpE8F=((
_tmp15A6=_cycalloc(sizeof(*_tmp15A6)),((_tmp15A6->v=(void*)((_tmp15A8=_cycalloc(
sizeof(*_tmp15A8)),((_tmp15A8[0]=((_tmp15A7.tag=2,((_tmp15A7.f1=0,((_tmp15A7.f2=&
Cyc_Tcutil_rk,_tmp15A7)))))),_tmp15A8)))),_tmp15A6))));}return 0;_LL710:;_LL711:
return 1;_LL70D:;}return 0;}_LL70B:;_LL70C: return 0;_LL706:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpE97=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE99;struct Cyc_Absyn_PtrAtts _tmpE9A;void*_tmpE9B;struct Cyc_Absyn_Tvar*_tmpE9D;
_LL713: {struct Cyc_Absyn_PointerType_struct*_tmpE98=(struct Cyc_Absyn_PointerType_struct*)
_tmpE97;if(_tmpE98->tag != 5)goto _LL715;else{_tmpE99=_tmpE98->f1;_tmpE9A=_tmpE99.ptr_atts;
_tmpE9B=_tmpE9A.rgn;}}_LL714: return Cyc_Tcutil_is_noalias_region(_tmpE9B,
must_be_unique);_LL715: {struct Cyc_Absyn_VarType_struct*_tmpE9C=(struct Cyc_Absyn_VarType_struct*)
_tmpE97;if(_tmpE9C->tag != 2)goto _LL717;else{_tmpE9D=_tmpE9C->f1;}}_LL716: {
struct Cyc_Absyn_Kind _tmpE9F;enum Cyc_Absyn_KindQual _tmpEA0;enum Cyc_Absyn_AliasQual
_tmpEA1;struct Cyc_Absyn_Kind*_tmpE9E=Cyc_Tcutil_tvar_kind(_tmpE9D,& Cyc_Tcutil_bk);
_tmpE9F=*_tmpE9E;_tmpEA0=_tmpE9F.kind;_tmpEA1=_tmpE9F.aliasqual;switch(_tmpEA0){
case Cyc_Absyn_BoxKind: _LL719: goto _LL71A;case Cyc_Absyn_AnyKind: _LL71A: goto _LL71B;
case Cyc_Absyn_MemKind: _LL71B: if(_tmpEA1 == Cyc_Absyn_Unique  || _tmpEA1 == Cyc_Absyn_Top){
void*_tmpEA2=Cyc_Absyn_compress_kb(_tmpE9D->kind);struct Cyc_Core_Opt*_tmpEA4;
struct Cyc_Core_Opt**_tmpEA5;struct Cyc_Absyn_Kind*_tmpEA6;struct Cyc_Absyn_Kind
_tmpEA7;enum Cyc_Absyn_KindQual _tmpEA8;enum Cyc_Absyn_AliasQual _tmpEA9;_LL71E: {
struct Cyc_Absyn_Less_kb_struct*_tmpEA3=(struct Cyc_Absyn_Less_kb_struct*)_tmpEA2;
if(_tmpEA3->tag != 2)goto _LL720;else{_tmpEA4=_tmpEA3->f1;_tmpEA5=(struct Cyc_Core_Opt**)&
_tmpEA3->f1;_tmpEA6=_tmpEA3->f2;_tmpEA7=*_tmpEA6;_tmpEA8=_tmpEA7.kind;_tmpEA9=
_tmpEA7.aliasqual;if(_tmpEA9 != Cyc_Absyn_Top)goto _LL720;}}_LL71F:{struct Cyc_Absyn_Less_kb_struct*
_tmp15B2;struct Cyc_Absyn_Kind*_tmp15B1;struct Cyc_Absyn_Less_kb_struct _tmp15B0;
struct Cyc_Core_Opt*_tmp15AF;*_tmpEA5=((_tmp15AF=_cycalloc(sizeof(*_tmp15AF)),((
_tmp15AF->v=(void*)((_tmp15B2=_cycalloc(sizeof(*_tmp15B2)),((_tmp15B2[0]=((
_tmp15B0.tag=2,((_tmp15B0.f1=0,((_tmp15B0.f2=((_tmp15B1=_cycalloc_atomic(sizeof(*
_tmp15B1)),((_tmp15B1->kind=_tmpEA8,((_tmp15B1->aliasqual=Cyc_Absyn_Aliasable,
_tmp15B1)))))),_tmp15B0)))))),_tmp15B2)))),_tmp15AF))));}return 0;_LL720:;_LL721:
return 1;_LL71D:;}return 0;default: _LL71C: return 0;}}_LL717:;_LL718: return 0;_LL712:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpEAE=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpEAE))
return 1;{void*_tmpEAF=_tmpEAE;struct Cyc_List_List*_tmpEB1;struct Cyc_Absyn_AggrInfo
_tmpEB3;union Cyc_Absyn_AggrInfoU _tmpEB4;struct Cyc_Absyn_Aggrdecl**_tmpEB5;struct
Cyc_List_List*_tmpEB6;struct Cyc_List_List*_tmpEB8;struct Cyc_Absyn_AggrInfo
_tmpEBA;union Cyc_Absyn_AggrInfoU _tmpEBB;struct _tuple4 _tmpEBC;struct Cyc_Absyn_DatatypeInfo
_tmpEBE;union Cyc_Absyn_DatatypeInfoU _tmpEBF;struct Cyc_List_List*_tmpEC0;struct
Cyc_Absyn_DatatypeFieldInfo _tmpEC2;union Cyc_Absyn_DatatypeFieldInfoU _tmpEC3;
struct Cyc_List_List*_tmpEC4;_LL724: {struct Cyc_Absyn_TupleType_struct*_tmpEB0=(
struct Cyc_Absyn_TupleType_struct*)_tmpEAF;if(_tmpEB0->tag != 11)goto _LL726;else{
_tmpEB1=_tmpEB0->f1;}}_LL725: while(_tmpEB1 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpEB1->hd)).f2))return 1;_tmpEB1=_tmpEB1->tl;}return 0;
_LL726: {struct Cyc_Absyn_AggrType_struct*_tmpEB2=(struct Cyc_Absyn_AggrType_struct*)
_tmpEAF;if(_tmpEB2->tag != 12)goto _LL728;else{_tmpEB3=_tmpEB2->f1;_tmpEB4=_tmpEB3.aggr_info;
if((_tmpEB4.KnownAggr).tag != 2)goto _LL728;_tmpEB5=(struct Cyc_Absyn_Aggrdecl**)(
_tmpEB4.KnownAggr).val;_tmpEB6=_tmpEB3.targs;}}_LL727: if((*_tmpEB5)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpEC5=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpEB5)->tvs,_tmpEB6);struct Cyc_List_List*_tmpEC6=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpEB5)->impl))->fields;void*t;
while(_tmpEC6 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpEC5,((struct Cyc_Absyn_Aggrfield*)
_tmpEC6->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpEC6=_tmpEC6->tl;}return 0;}_LL728: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpEB7=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpEAF;if(_tmpEB7->tag != 13)goto
_LL72A;else{_tmpEB8=_tmpEB7->f2;}}_LL729: while(_tmpEB8 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpEB8->hd)->type))return 1;_tmpEB8=_tmpEB8->tl;}
return 0;_LL72A: {struct Cyc_Absyn_AggrType_struct*_tmpEB9=(struct Cyc_Absyn_AggrType_struct*)
_tmpEAF;if(_tmpEB9->tag != 12)goto _LL72C;else{_tmpEBA=_tmpEB9->f1;_tmpEBB=_tmpEBA.aggr_info;
if((_tmpEBB.UnknownAggr).tag != 1)goto _LL72C;_tmpEBC=(struct _tuple4)(_tmpEBB.UnknownAggr).val;}}
_LL72B: {const char*_tmp15B5;void*_tmp15B4;(_tmp15B4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15B5="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp15B5,sizeof(char),36))),_tag_dyneither(_tmp15B4,sizeof(void*),
0)));}_LL72C: {struct Cyc_Absyn_DatatypeType_struct*_tmpEBD=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpEAF;if(_tmpEBD->tag != 3)goto _LL72E;else{_tmpEBE=_tmpEBD->f1;_tmpEBF=_tmpEBE.datatype_info;
_tmpEC0=_tmpEBE.targs;}}_LL72D: {union Cyc_Absyn_DatatypeInfoU _tmpEC9=_tmpEBF;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpECA;struct _tuple2*_tmpECB;int _tmpECC;
struct Cyc_Absyn_Datatypedecl**_tmpECD;struct Cyc_Absyn_Datatypedecl*_tmpECE;
struct Cyc_Absyn_Datatypedecl _tmpECF;struct Cyc_List_List*_tmpED0;struct Cyc_Core_Opt*
_tmpED1;_LL733: if((_tmpEC9.UnknownDatatype).tag != 1)goto _LL735;_tmpECA=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpEC9.UnknownDatatype).val;_tmpECB=_tmpECA.name;
_tmpECC=_tmpECA.is_extensible;_LL734: {const char*_tmp15B8;void*_tmp15B7;(
_tmp15B7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp15B8="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp15B8,
sizeof(char),40))),_tag_dyneither(_tmp15B7,sizeof(void*),0)));}_LL735: if((
_tmpEC9.KnownDatatype).tag != 2)goto _LL732;_tmpECD=(struct Cyc_Absyn_Datatypedecl**)(
_tmpEC9.KnownDatatype).val;_tmpECE=*_tmpECD;_tmpECF=*_tmpECE;_tmpED0=_tmpECF.tvs;
_tmpED1=_tmpECF.fields;_LL736: return 0;_LL732:;}_LL72E: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpEC1=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpEAF;if(_tmpEC1->tag != 4)
goto _LL730;else{_tmpEC2=_tmpEC1->f1;_tmpEC3=_tmpEC2.field_info;_tmpEC4=_tmpEC2.targs;}}
_LL72F: {union Cyc_Absyn_DatatypeFieldInfoU _tmpED4=_tmpEC3;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpED5;struct _tuple3 _tmpED6;struct Cyc_Absyn_Datatypedecl*_tmpED7;struct Cyc_Absyn_Datatypefield*
_tmpED8;_LL738: if((_tmpED4.UnknownDatatypefield).tag != 1)goto _LL73A;_tmpED5=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpED4.UnknownDatatypefield).val;
_LL739: {const char*_tmp15BB;void*_tmp15BA;(_tmp15BA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15BB="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp15BB,sizeof(char),46))),_tag_dyneither(_tmp15BA,sizeof(void*),
0)));}_LL73A: if((_tmpED4.KnownDatatypefield).tag != 2)goto _LL737;_tmpED6=(struct
_tuple3)(_tmpED4.KnownDatatypefield).val;_tmpED7=_tmpED6.f1;_tmpED8=_tmpED6.f2;
_LL73B: {struct Cyc_List_List*_tmpEDB=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpED7->tvs,_tmpEC4);struct Cyc_List_List*_tmpEDC=
_tmpED8->typs;while(_tmpEDC != 0){_tmpEAE=Cyc_Tcutil_rsubstitute(rgn,_tmpEDB,(*((
struct _tuple11*)_tmpEDC->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpEAE))return 1;_tmpEDC=_tmpEDC->tl;}return 0;}_LL737:;}_LL730:;_LL731: return 0;
_LL723:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpEDD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpEDF;union Cyc_Absyn_AggrInfoU _tmpEE0;struct Cyc_Absyn_Aggrdecl**_tmpEE1;struct
Cyc_Absyn_Aggrdecl*_tmpEE2;struct Cyc_List_List*_tmpEE3;struct Cyc_List_List*
_tmpEE5;_LL73D: {struct Cyc_Absyn_AggrType_struct*_tmpEDE=(struct Cyc_Absyn_AggrType_struct*)
_tmpEDD;if(_tmpEDE->tag != 12)goto _LL73F;else{_tmpEDF=_tmpEDE->f1;_tmpEE0=_tmpEDF.aggr_info;
if((_tmpEE0.KnownAggr).tag != 2)goto _LL73F;_tmpEE1=(struct Cyc_Absyn_Aggrdecl**)(
_tmpEE0.KnownAggr).val;_tmpEE2=*_tmpEE1;_tmpEE3=_tmpEDF.targs;}}_LL73E: _tmpEE5=
_tmpEE2->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpEE2->impl))->fields;
goto _LL740;_LL73F: {struct Cyc_Absyn_AnonAggrType_struct*_tmpEE4=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpEDD;if(_tmpEE4->tag != 13)goto _LL741;else{_tmpEE5=_tmpEE4->f2;}}_LL740: {
struct Cyc_Absyn_Aggrfield*_tmpEE6=Cyc_Absyn_lookup_field(_tmpEE5,f);{struct Cyc_Absyn_Aggrfield*
_tmpEE7=_tmpEE6;struct Cyc_Absyn_Aggrfield _tmpEE8;void*_tmpEE9;_LL744: if(_tmpEE7
!= 0)goto _LL746;_LL745: {const char*_tmp15BE;void*_tmp15BD;(_tmp15BD=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15BE="is_noalias_field: missing field",
_tag_dyneither(_tmp15BE,sizeof(char),32))),_tag_dyneither(_tmp15BD,sizeof(void*),
0)));}_LL746: if(_tmpEE7 == 0)goto _LL743;_tmpEE8=*_tmpEE7;_tmpEE9=_tmpEE8.type;
_LL747: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpEE9);_LL743:;}return 0;}
_LL741:;_LL742: {const char*_tmp15C2;void*_tmp15C1[1];struct Cyc_String_pa_struct
_tmp15C0;(_tmp15C0.tag=0,((_tmp15C0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp15C1[0]=& _tmp15C0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15C2="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp15C2,sizeof(char),36))),_tag_dyneither(_tmp15C1,sizeof(void*),
1)))))));}_LL73C:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpEEF=e->r;void*_tmpEF1;struct Cyc_Absyn_Exp*
_tmpEF4;struct Cyc_Absyn_Exp*_tmpEF6;struct Cyc_Absyn_Exp*_tmpEF8;struct
_dyneither_ptr*_tmpEF9;struct Cyc_Absyn_Exp*_tmpEFB;struct Cyc_Absyn_Exp*_tmpEFC;
struct Cyc_Absyn_Exp*_tmpEFE;struct Cyc_Absyn_Exp*_tmpEFF;struct Cyc_Absyn_Exp*
_tmpF01;struct Cyc_Absyn_Exp*_tmpF03;struct Cyc_Absyn_Stmt*_tmpF05;_LL749: {struct
Cyc_Absyn_Var_e_struct*_tmpEF0=(struct Cyc_Absyn_Var_e_struct*)_tmpEEF;if(_tmpEF0->tag
!= 1)goto _LL74B;else{_tmpEF1=(void*)_tmpEF0->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpEF2=(struct Cyc_Absyn_Global_b_struct*)_tmpEF1;if(_tmpEF2->tag != 1)goto _LL74B;};}}
_LL74A: return 0;_LL74B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEF3=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEEF;if(_tmpEF3->tag != 23)goto _LL74D;else{_tmpEF4=_tmpEF3->f1;}}_LL74C:
_tmpEF6=_tmpEF4;goto _LL74E;_LL74D: {struct Cyc_Absyn_Deref_e_struct*_tmpEF5=(
struct Cyc_Absyn_Deref_e_struct*)_tmpEEF;if(_tmpEF5->tag != 21)goto _LL74F;else{
_tmpEF6=_tmpEF5->f1;}}_LL74E: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEF6->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEF6);_LL74F: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEF7=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpEEF;if(_tmpEF7->tag != 22)goto _LL751;else{_tmpEF8=_tmpEF7->f1;_tmpEF9=_tmpEF7->f2;}}
_LL750: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEF8);_LL751: {struct Cyc_Absyn_Subscript_e_struct*
_tmpEFA=(struct Cyc_Absyn_Subscript_e_struct*)_tmpEEF;if(_tmpEFA->tag != 24)goto
_LL753;else{_tmpEFB=_tmpEFA->f1;_tmpEFC=_tmpEFA->f2;}}_LL752: {void*_tmpF06=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEFB->topt))->v);_LL75E: {struct Cyc_Absyn_TupleType_struct*
_tmpF07=(struct Cyc_Absyn_TupleType_struct*)_tmpF06;if(_tmpF07->tag != 11)goto
_LL760;}_LL75F: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpEFB);_LL760:;_LL761:
return 0;_LL75D:;}_LL753: {struct Cyc_Absyn_Conditional_e_struct*_tmpEFD=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpEEF;if(_tmpEFD->tag != 6)goto _LL755;else{
_tmpEFE=_tmpEFD->f2;_tmpEFF=_tmpEFD->f3;}}_LL754: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpEFE) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpEFF);_LL755: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF00=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpEEF;if(_tmpF00->tag != 9)goto _LL757;
else{_tmpF01=_tmpF00->f2;}}_LL756: _tmpF03=_tmpF01;goto _LL758;_LL757: {struct Cyc_Absyn_Cast_e_struct*
_tmpF02=(struct Cyc_Absyn_Cast_e_struct*)_tmpEEF;if(_tmpF02->tag != 15)goto _LL759;
else{_tmpF03=_tmpF02->f2;}}_LL758: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF03);
_LL759: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF04=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpEEF;if(_tmpF04->tag != 37)goto _LL75B;else{_tmpF05=_tmpF04->f1;}}_LL75A: while(
1){void*_tmpF08=_tmpF05->r;struct Cyc_Absyn_Stmt*_tmpF0A;struct Cyc_Absyn_Stmt*
_tmpF0B;struct Cyc_Absyn_Decl*_tmpF0D;struct Cyc_Absyn_Stmt*_tmpF0E;struct Cyc_Absyn_Exp*
_tmpF10;_LL763: {struct Cyc_Absyn_Seq_s_struct*_tmpF09=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF08;if(_tmpF09->tag != 2)goto _LL765;else{_tmpF0A=_tmpF09->f1;_tmpF0B=_tmpF09->f2;}}
_LL764: _tmpF05=_tmpF0B;goto _LL762;_LL765: {struct Cyc_Absyn_Decl_s_struct*_tmpF0C=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF08;if(_tmpF0C->tag != 12)goto _LL767;else{
_tmpF0D=_tmpF0C->f1;_tmpF0E=_tmpF0C->f2;}}_LL766: _tmpF05=_tmpF0E;goto _LL762;
_LL767: {struct Cyc_Absyn_Exp_s_struct*_tmpF0F=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF08;if(_tmpF0F->tag != 1)goto _LL769;else{_tmpF10=_tmpF0F->f1;}}_LL768: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF10);_LL769:;_LL76A: {const char*_tmp15C5;
void*_tmp15C4;(_tmp15C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15C5="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp15C5,sizeof(char),40))),_tag_dyneither(_tmp15C4,sizeof(void*),
0)));}_LL762:;}_LL75B:;_LL75C: return 1;_LL748:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp15C6;struct _tuple17 bogus_ans=(_tmp15C6.f1=0,((_tmp15C6.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp15C6)));void*_tmpF13=e->r;struct _tuple2*_tmpF15;
void*_tmpF16;struct Cyc_Absyn_Exp*_tmpF18;struct _dyneither_ptr*_tmpF19;int _tmpF1A;
struct Cyc_Absyn_Exp*_tmpF1C;struct _dyneither_ptr*_tmpF1D;int _tmpF1E;struct Cyc_Absyn_Exp*
_tmpF20;struct Cyc_Absyn_Exp*_tmpF22;struct Cyc_Absyn_Exp*_tmpF23;_LL76C: {struct
Cyc_Absyn_Var_e_struct*_tmpF14=(struct Cyc_Absyn_Var_e_struct*)_tmpF13;if(_tmpF14->tag
!= 1)goto _LL76E;else{_tmpF15=_tmpF14->f1;_tmpF16=(void*)_tmpF14->f2;}}_LL76D: {
void*_tmpF24=_tmpF16;struct Cyc_Absyn_Vardecl*_tmpF28;struct Cyc_Absyn_Vardecl*
_tmpF2A;struct Cyc_Absyn_Vardecl*_tmpF2C;struct Cyc_Absyn_Vardecl*_tmpF2E;_LL779: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF25=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF24;if(_tmpF25->tag != 0)goto _LL77B;}_LL77A: goto _LL77C;_LL77B: {struct Cyc_Absyn_Funname_b_struct*
_tmpF26=(struct Cyc_Absyn_Funname_b_struct*)_tmpF24;if(_tmpF26->tag != 2)goto
_LL77D;}_LL77C: return bogus_ans;_LL77D: {struct Cyc_Absyn_Global_b_struct*_tmpF27=(
struct Cyc_Absyn_Global_b_struct*)_tmpF24;if(_tmpF27->tag != 1)goto _LL77F;else{
_tmpF28=_tmpF27->f1;}}_LL77E: {void*_tmpF2F=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL786: {struct Cyc_Absyn_ArrayType_struct*
_tmpF30=(struct Cyc_Absyn_ArrayType_struct*)_tmpF2F;if(_tmpF30->tag != 9)goto
_LL788;}_LL787: {struct _tuple17 _tmp15C7;return(_tmp15C7.f1=1,((_tmp15C7.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp15C7)));}_LL788:;_LL789: {struct _tuple17 _tmp15C8;
return(_tmp15C8.f1=(_tmpF28->tq).real_const,((_tmp15C8.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp15C8)));}_LL785:;}_LL77F: {struct Cyc_Absyn_Local_b_struct*_tmpF29=(struct Cyc_Absyn_Local_b_struct*)
_tmpF24;if(_tmpF29->tag != 4)goto _LL781;else{_tmpF2A=_tmpF29->f1;}}_LL780: {void*
_tmpF33=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL78B: {struct Cyc_Absyn_ArrayType_struct*_tmpF34=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF33;if(_tmpF34->tag != 9)goto _LL78D;}_LL78C: {struct _tuple17 _tmp15C9;return(
_tmp15C9.f1=1,((_tmp15C9.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF2A->rgn))->v,
_tmp15C9)));}_LL78D:;_LL78E: _tmpF2A->escapes=1;{struct _tuple17 _tmp15CA;return(
_tmp15CA.f1=(_tmpF2A->tq).real_const,((_tmp15CA.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF2A->rgn))->v,_tmp15CA)));};_LL78A:;}_LL781: {struct Cyc_Absyn_Pat_b_struct*
_tmpF2B=(struct Cyc_Absyn_Pat_b_struct*)_tmpF24;if(_tmpF2B->tag != 5)goto _LL783;
else{_tmpF2C=_tmpF2B->f1;}}_LL782: _tmpF2E=_tmpF2C;goto _LL784;_LL783: {struct Cyc_Absyn_Param_b_struct*
_tmpF2D=(struct Cyc_Absyn_Param_b_struct*)_tmpF24;if(_tmpF2D->tag != 3)goto _LL778;
else{_tmpF2E=_tmpF2D->f1;}}_LL784: _tmpF2E->escapes=1;{struct _tuple17 _tmp15CB;
return(_tmp15CB.f1=(_tmpF2E->tq).real_const,((_tmp15CB.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF2E->rgn))->v,_tmp15CB)));};_LL778:;}_LL76E: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF17=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF13;if(_tmpF17->tag != 22)goto
_LL770;else{_tmpF18=_tmpF17->f1;_tmpF19=_tmpF17->f2;_tmpF1A=_tmpF17->f3;}}_LL76F:
if(_tmpF1A)return bogus_ans;{void*_tmpF38=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF18->topt))->v);struct Cyc_List_List*_tmpF3A;struct Cyc_Absyn_AggrInfo
_tmpF3C;union Cyc_Absyn_AggrInfoU _tmpF3D;struct Cyc_Absyn_Aggrdecl**_tmpF3E;struct
Cyc_Absyn_Aggrdecl*_tmpF3F;_LL790: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF39=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF38;if(_tmpF39->tag != 13)goto _LL792;
else{_tmpF3A=_tmpF39->f2;}}_LL791: {struct Cyc_Absyn_Aggrfield*_tmpF40=Cyc_Absyn_lookup_field(
_tmpF3A,_tmpF19);if(_tmpF40 != 0  && _tmpF40->width != 0){struct _tuple17 _tmp15CC;
return(_tmp15CC.f1=(_tmpF40->tq).real_const,((_tmp15CC.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF18)).f2,_tmp15CC)));}return bogus_ans;}_LL792: {struct Cyc_Absyn_AggrType_struct*
_tmpF3B=(struct Cyc_Absyn_AggrType_struct*)_tmpF38;if(_tmpF3B->tag != 12)goto
_LL794;else{_tmpF3C=_tmpF3B->f1;_tmpF3D=_tmpF3C.aggr_info;if((_tmpF3D.KnownAggr).tag
!= 2)goto _LL794;_tmpF3E=(struct Cyc_Absyn_Aggrdecl**)(_tmpF3D.KnownAggr).val;
_tmpF3F=*_tmpF3E;}}_LL793: {struct Cyc_Absyn_Aggrfield*_tmpF42=Cyc_Absyn_lookup_decl_field(
_tmpF3F,_tmpF19);if(_tmpF42 != 0  && _tmpF42->width != 0){struct _tuple17 _tmp15CD;
return(_tmp15CD.f1=(_tmpF42->tq).real_const,((_tmp15CD.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF18)).f2,_tmp15CD)));}return bogus_ans;}_LL794:;_LL795: return bogus_ans;
_LL78F:;};_LL770: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF1B=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF13;if(_tmpF1B->tag != 23)goto _LL772;else{_tmpF1C=_tmpF1B->f1;_tmpF1D=_tmpF1B->f2;
_tmpF1E=_tmpF1B->f3;}}_LL771: if(_tmpF1E)return bogus_ans;{void*_tmpF44=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF1C->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF46;void*_tmpF47;struct Cyc_Absyn_PtrAtts _tmpF48;void*_tmpF49;_LL797: {struct
Cyc_Absyn_PointerType_struct*_tmpF45=(struct Cyc_Absyn_PointerType_struct*)
_tmpF44;if(_tmpF45->tag != 5)goto _LL799;else{_tmpF46=_tmpF45->f1;_tmpF47=_tmpF46.elt_typ;
_tmpF48=_tmpF46.ptr_atts;_tmpF49=_tmpF48.rgn;}}_LL798: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF4A=Cyc_Tcutil_compress(_tmpF47);struct Cyc_List_List*_tmpF4C;
struct Cyc_Absyn_AggrInfo _tmpF4E;union Cyc_Absyn_AggrInfoU _tmpF4F;struct Cyc_Absyn_Aggrdecl**
_tmpF50;struct Cyc_Absyn_Aggrdecl*_tmpF51;_LL79C: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF4B=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF4A;if(_tmpF4B->tag != 13)goto
_LL79E;else{_tmpF4C=_tmpF4B->f2;}}_LL79D: finfo=Cyc_Absyn_lookup_field(_tmpF4C,
_tmpF1D);goto _LL79B;_LL79E: {struct Cyc_Absyn_AggrType_struct*_tmpF4D=(struct Cyc_Absyn_AggrType_struct*)
_tmpF4A;if(_tmpF4D->tag != 12)goto _LL7A0;else{_tmpF4E=_tmpF4D->f1;_tmpF4F=_tmpF4E.aggr_info;
if((_tmpF4F.KnownAggr).tag != 2)goto _LL7A0;_tmpF50=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF4F.KnownAggr).val;_tmpF51=*_tmpF50;}}_LL79F: finfo=Cyc_Absyn_lookup_decl_field(
_tmpF51,_tmpF1D);goto _LL79B;_LL7A0:;_LL7A1: return bogus_ans;_LL79B:;}if(finfo != 0
 && finfo->width != 0){struct _tuple17 _tmp15CE;return(_tmp15CE.f1=(finfo->tq).real_const,((
_tmp15CE.f2=_tmpF49,_tmp15CE)));}return bogus_ans;}_LL799:;_LL79A: return bogus_ans;
_LL796:;};_LL772: {struct Cyc_Absyn_Deref_e_struct*_tmpF1F=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF13;if(_tmpF1F->tag != 21)goto _LL774;else{_tmpF20=_tmpF1F->f1;}}_LL773: {void*
_tmpF53=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF20->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpF55;struct Cyc_Absyn_Tqual _tmpF56;struct Cyc_Absyn_PtrAtts
_tmpF57;void*_tmpF58;_LL7A3: {struct Cyc_Absyn_PointerType_struct*_tmpF54=(struct
Cyc_Absyn_PointerType_struct*)_tmpF53;if(_tmpF54->tag != 5)goto _LL7A5;else{
_tmpF55=_tmpF54->f1;_tmpF56=_tmpF55.elt_tq;_tmpF57=_tmpF55.ptr_atts;_tmpF58=
_tmpF57.rgn;}}_LL7A4: {struct _tuple17 _tmp15CF;return(_tmp15CF.f1=_tmpF56.real_const,((
_tmp15CF.f2=_tmpF58,_tmp15CF)));}_LL7A5:;_LL7A6: return bogus_ans;_LL7A2:;}_LL774: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF21=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF13;if(_tmpF21->tag != 24)goto _LL776;else{_tmpF22=_tmpF21->f1;_tmpF23=_tmpF21->f2;}}
_LL775: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF22->topt))->v);void*_tmpF5A=t;struct Cyc_List_List*_tmpF5C;struct Cyc_Absyn_PtrInfo
_tmpF5E;struct Cyc_Absyn_Tqual _tmpF5F;struct Cyc_Absyn_PtrAtts _tmpF60;void*_tmpF61;
struct Cyc_Absyn_ArrayInfo _tmpF63;struct Cyc_Absyn_Tqual _tmpF64;_LL7A8: {struct Cyc_Absyn_TupleType_struct*
_tmpF5B=(struct Cyc_Absyn_TupleType_struct*)_tmpF5A;if(_tmpF5B->tag != 11)goto
_LL7AA;else{_tmpF5C=_tmpF5B->f1;}}_LL7A9: {unsigned int _tmpF66;int _tmpF67;struct
_tuple13 _tmpF65=Cyc_Evexp_eval_const_uint_exp(_tmpF23);_tmpF66=_tmpF65.f1;
_tmpF67=_tmpF65.f2;if(!_tmpF67)return bogus_ans;{struct _tuple11*_tmpF68=Cyc_Absyn_lookup_tuple_field(
_tmpF5C,(int)_tmpF66);if(_tmpF68 != 0){struct _tuple17 _tmp15D0;return(_tmp15D0.f1=((*
_tmpF68).f1).real_const,((_tmp15D0.f2=(Cyc_Tcutil_addressof_props(te,_tmpF22)).f2,
_tmp15D0)));}return bogus_ans;};}_LL7AA: {struct Cyc_Absyn_PointerType_struct*
_tmpF5D=(struct Cyc_Absyn_PointerType_struct*)_tmpF5A;if(_tmpF5D->tag != 5)goto
_LL7AC;else{_tmpF5E=_tmpF5D->f1;_tmpF5F=_tmpF5E.elt_tq;_tmpF60=_tmpF5E.ptr_atts;
_tmpF61=_tmpF60.rgn;}}_LL7AB: {struct _tuple17 _tmp15D1;return(_tmp15D1.f1=_tmpF5F.real_const,((
_tmp15D1.f2=_tmpF61,_tmp15D1)));}_LL7AC: {struct Cyc_Absyn_ArrayType_struct*
_tmpF62=(struct Cyc_Absyn_ArrayType_struct*)_tmpF5A;if(_tmpF62->tag != 9)goto
_LL7AE;else{_tmpF63=_tmpF62->f1;_tmpF64=_tmpF63.tq;}}_LL7AD: {struct _tuple17
_tmp15D2;return(_tmp15D2.f1=_tmpF64.real_const,((_tmp15D2.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF22)).f2,_tmp15D2)));}_LL7AE:;_LL7AF: return bogus_ans;_LL7A7:;}_LL776:;
_LL777:{const char*_tmp15D5;void*_tmp15D4;(_tmp15D4=0,Cyc_Tcutil_terr(e->loc,((
_tmp15D5="unary & applied to non-lvalue",_tag_dyneither(_tmp15D5,sizeof(char),30))),
_tag_dyneither(_tmp15D4,sizeof(void*),0)));}return bogus_ans;_LL76B:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpF6F=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpF71;void*_tmpF72;struct Cyc_Absyn_Tqual
_tmpF73;union Cyc_Absyn_Constraint*_tmpF74;_LL7B1: {struct Cyc_Absyn_ArrayType_struct*
_tmpF70=(struct Cyc_Absyn_ArrayType_struct*)_tmpF6F;if(_tmpF70->tag != 9)goto
_LL7B3;else{_tmpF71=_tmpF70->f1;_tmpF72=_tmpF71.elt_type;_tmpF73=_tmpF71.tq;
_tmpF74=_tmpF71.zero_term;}}_LL7B2: {void*_tmpF76;struct _tuple17 _tmpF75=Cyc_Tcutil_addressof_props(
te,e);_tmpF76=_tmpF75.f2;{struct Cyc_Absyn_Upper_b_struct _tmp15D8;struct Cyc_Absyn_Upper_b_struct*
_tmp15D7;return Cyc_Absyn_atb_typ(_tmpF72,_tmpF76,_tmpF73,(void*)((_tmp15D7=
_cycalloc(sizeof(*_tmp15D7)),((_tmp15D7[0]=((_tmp15D8.tag=1,((_tmp15D8.f1=e,
_tmp15D8)))),_tmp15D7)))),_tmpF74);};}_LL7B3:;_LL7B4: return e_typ;_LL7B0:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpF79=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpF7C;_LL7B6: {struct Cyc_Absyn_DynEither_b_struct*
_tmpF7A=(struct Cyc_Absyn_DynEither_b_struct*)_tmpF79;if(_tmpF7A->tag != 0)goto
_LL7B8;}_LL7B7: return;_LL7B8: {struct Cyc_Absyn_Upper_b_struct*_tmpF7B=(struct Cyc_Absyn_Upper_b_struct*)
_tmpF79;if(_tmpF7B->tag != 1)goto _LL7B5;else{_tmpF7C=_tmpF7B->f1;}}_LL7B9: {
unsigned int _tmpF7E;int _tmpF7F;struct _tuple13 _tmpF7D=Cyc_Evexp_eval_const_uint_exp(
_tmpF7C);_tmpF7E=_tmpF7D.f1;_tmpF7F=_tmpF7D.f2;if(_tmpF7F  && _tmpF7E <= i){const
char*_tmp15DD;void*_tmp15DC[2];struct Cyc_Int_pa_struct _tmp15DB;struct Cyc_Int_pa_struct
_tmp15DA;(_tmp15DA.tag=1,((_tmp15DA.f1=(unsigned long)((int)i),((_tmp15DB.tag=1,((
_tmp15DB.f1=(unsigned long)((int)_tmpF7E),((_tmp15DC[0]=& _tmp15DB,((_tmp15DC[1]=&
_tmp15DA,Cyc_Tcutil_terr(loc,((_tmp15DD="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp15DD,sizeof(char),39))),_tag_dyneither(_tmp15DC,sizeof(void*),
2)))))))))))));}return;}_LL7B5:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpF84=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpF86;_LL7BB: {struct Cyc_Absyn_Upper_b_struct*_tmpF85=(struct Cyc_Absyn_Upper_b_struct*)
_tmpF84;if(_tmpF85->tag != 1)goto _LL7BD;else{_tmpF86=_tmpF85->f1;}}_LL7BC: {
unsigned int _tmpF88;int _tmpF89;struct _tuple13 _tmpF87=Cyc_Evexp_eval_const_uint_exp(
_tmpF86);_tmpF88=_tmpF87.f1;_tmpF89=_tmpF87.f2;return _tmpF89  && _tmpF88 == 1;}
_LL7BD:;_LL7BE: return 0;_LL7BA:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpF8A=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpF92;void*_tmpF93;union Cyc_Absyn_Constraint*
_tmpF94;struct Cyc_List_List*_tmpF96;struct Cyc_Absyn_AggrInfo _tmpF98;union Cyc_Absyn_AggrInfoU
_tmpF99;struct _tuple4 _tmpF9A;struct Cyc_Absyn_AggrInfo _tmpF9C;union Cyc_Absyn_AggrInfoU
_tmpF9D;struct Cyc_Absyn_Aggrdecl**_tmpF9E;struct Cyc_Absyn_Aggrdecl*_tmpF9F;
struct Cyc_List_List*_tmpFA0;struct Cyc_List_List*_tmpFA2;_LL7C0: {struct Cyc_Absyn_VoidType_struct*
_tmpF8B=(struct Cyc_Absyn_VoidType_struct*)_tmpF8A;if(_tmpF8B->tag != 0)goto _LL7C2;}
_LL7C1: goto _LL7C3;_LL7C2: {struct Cyc_Absyn_IntType_struct*_tmpF8C=(struct Cyc_Absyn_IntType_struct*)
_tmpF8A;if(_tmpF8C->tag != 6)goto _LL7C4;}_LL7C3: goto _LL7C5;_LL7C4: {struct Cyc_Absyn_FloatType_struct*
_tmpF8D=(struct Cyc_Absyn_FloatType_struct*)_tmpF8A;if(_tmpF8D->tag != 7)goto
_LL7C6;}_LL7C5: goto _LL7C7;_LL7C6: {struct Cyc_Absyn_DoubleType_struct*_tmpF8E=(
struct Cyc_Absyn_DoubleType_struct*)_tmpF8A;if(_tmpF8E->tag != 8)goto _LL7C8;}
_LL7C7: return 1;_LL7C8: {struct Cyc_Absyn_EnumType_struct*_tmpF8F=(struct Cyc_Absyn_EnumType_struct*)
_tmpF8A;if(_tmpF8F->tag != 14)goto _LL7CA;}_LL7C9: goto _LL7CB;_LL7CA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpF90=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpF8A;if(_tmpF90->tag != 15)goto
_LL7CC;}_LL7CB: return 0;_LL7CC: {struct Cyc_Absyn_ArrayType_struct*_tmpF91=(struct
Cyc_Absyn_ArrayType_struct*)_tmpF8A;if(_tmpF91->tag != 9)goto _LL7CE;else{_tmpF92=
_tmpF91->f1;_tmpF93=_tmpF92.elt_type;_tmpF94=_tmpF92.zero_term;}}_LL7CD: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF94) && Cyc_Tcutil_bits_only(
_tmpF93);_LL7CE: {struct Cyc_Absyn_TupleType_struct*_tmpF95=(struct Cyc_Absyn_TupleType_struct*)
_tmpF8A;if(_tmpF95->tag != 11)goto _LL7D0;else{_tmpF96=_tmpF95->f1;}}_LL7CF: for(0;
_tmpF96 != 0;_tmpF96=_tmpF96->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpF96->hd)).f2))return 0;}return 1;_LL7D0: {struct Cyc_Absyn_AggrType_struct*
_tmpF97=(struct Cyc_Absyn_AggrType_struct*)_tmpF8A;if(_tmpF97->tag != 12)goto
_LL7D2;else{_tmpF98=_tmpF97->f1;_tmpF99=_tmpF98.aggr_info;if((_tmpF99.UnknownAggr).tag
!= 1)goto _LL7D2;_tmpF9A=(struct _tuple4)(_tmpF99.UnknownAggr).val;}}_LL7D1: return
0;_LL7D2: {struct Cyc_Absyn_AggrType_struct*_tmpF9B=(struct Cyc_Absyn_AggrType_struct*)
_tmpF8A;if(_tmpF9B->tag != 12)goto _LL7D4;else{_tmpF9C=_tmpF9B->f1;_tmpF9D=_tmpF9C.aggr_info;
if((_tmpF9D.KnownAggr).tag != 2)goto _LL7D4;_tmpF9E=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF9D.KnownAggr).val;_tmpF9F=*_tmpF9E;_tmpFA0=_tmpF9C.targs;}}_LL7D3: if(
_tmpF9F->impl == 0)return 0;{struct _RegionHandle _tmpFA3=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFA3;_push_region(rgn);{struct Cyc_List_List*_tmpFA4=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpF9F->tvs,_tmpFA0);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF9F->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFA4,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFA5=0;_npop_handler(0);return
_tmpFA5;}}}{int _tmpFA6=1;_npop_handler(0);return _tmpFA6;};};_pop_region(rgn);};
_LL7D4: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFA1=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF8A;if(_tmpFA1->tag != 13)goto _LL7D6;else{_tmpFA2=_tmpFA1->f2;}}_LL7D5: for(0;
_tmpFA2 != 0;_tmpFA2=_tmpFA2->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFA2->hd)->type))return 0;}return 1;_LL7D6:;_LL7D7: return 0;_LL7BF:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpFA7=e->r;struct _tuple2*_tmpFAF;void*_tmpFB0;struct Cyc_Absyn_Exp*_tmpFB2;
struct Cyc_Absyn_Exp*_tmpFB3;struct Cyc_Absyn_Exp*_tmpFB4;struct Cyc_Absyn_Exp*
_tmpFB6;struct Cyc_Absyn_Exp*_tmpFB7;struct Cyc_Absyn_Exp*_tmpFB9;struct Cyc_Absyn_Exp*
_tmpFBB;void*_tmpFBD;struct Cyc_Absyn_Exp*_tmpFBE;enum Cyc_Absyn_Coercion _tmpFBF;
void*_tmpFC1;struct Cyc_Absyn_Exp*_tmpFC2;struct Cyc_Absyn_Exp*_tmpFC4;struct Cyc_Absyn_Exp*
_tmpFC6;struct Cyc_Absyn_Exp*_tmpFC7;struct Cyc_List_List*_tmpFC9;struct Cyc_List_List*
_tmpFCB;struct Cyc_List_List*_tmpFCD;enum Cyc_Absyn_Primop _tmpFCF;struct Cyc_List_List*
_tmpFD0;struct Cyc_List_List*_tmpFD2;struct Cyc_List_List*_tmpFD4;_LL7D9: {struct
Cyc_Absyn_Const_e_struct*_tmpFA8=(struct Cyc_Absyn_Const_e_struct*)_tmpFA7;if(
_tmpFA8->tag != 0)goto _LL7DB;}_LL7DA: goto _LL7DC;_LL7DB: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpFA9=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpFA7;if(_tmpFA9->tag != 18)goto
_LL7DD;}_LL7DC: goto _LL7DE;_LL7DD: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpFAA=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFA7;if(_tmpFAA->tag != 19)goto _LL7DF;}
_LL7DE: goto _LL7E0;_LL7DF: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFAB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpFA7;if(_tmpFAB->tag != 20)goto _LL7E1;}_LL7E0: goto _LL7E2;_LL7E1: {struct Cyc_Absyn_Enum_e_struct*
_tmpFAC=(struct Cyc_Absyn_Enum_e_struct*)_tmpFA7;if(_tmpFAC->tag != 32)goto _LL7E3;}
_LL7E2: goto _LL7E4;_LL7E3: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpFAD=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpFA7;if(_tmpFAD->tag != 33)goto _LL7E5;}_LL7E4: return 1;_LL7E5: {struct Cyc_Absyn_Var_e_struct*
_tmpFAE=(struct Cyc_Absyn_Var_e_struct*)_tmpFA7;if(_tmpFAE->tag != 1)goto _LL7E7;
else{_tmpFAF=_tmpFAE->f1;_tmpFB0=(void*)_tmpFAE->f2;}}_LL7E6: {void*_tmpFD5=
_tmpFB0;struct Cyc_Absyn_Vardecl*_tmpFD8;_LL806: {struct Cyc_Absyn_Funname_b_struct*
_tmpFD6=(struct Cyc_Absyn_Funname_b_struct*)_tmpFD5;if(_tmpFD6->tag != 2)goto
_LL808;}_LL807: return 1;_LL808: {struct Cyc_Absyn_Global_b_struct*_tmpFD7=(struct
Cyc_Absyn_Global_b_struct*)_tmpFD5;if(_tmpFD7->tag != 1)goto _LL80A;else{_tmpFD8=
_tmpFD7->f1;}}_LL809: {void*_tmpFDA=Cyc_Tcutil_compress(_tmpFD8->type);_LL80F: {
struct Cyc_Absyn_ArrayType_struct*_tmpFDB=(struct Cyc_Absyn_ArrayType_struct*)
_tmpFDA;if(_tmpFDB->tag != 9)goto _LL811;}_LL810: goto _LL812;_LL811: {struct Cyc_Absyn_FnType_struct*
_tmpFDC=(struct Cyc_Absyn_FnType_struct*)_tmpFDA;if(_tmpFDC->tag != 10)goto _LL813;}
_LL812: return 1;_LL813:;_LL814: return var_okay;_LL80E:;}_LL80A: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpFD9=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpFD5;if(_tmpFD9->tag != 0)goto
_LL80C;}_LL80B: return 0;_LL80C:;_LL80D: return var_okay;_LL805:;}_LL7E7: {struct Cyc_Absyn_Conditional_e_struct*
_tmpFB1=(struct Cyc_Absyn_Conditional_e_struct*)_tmpFA7;if(_tmpFB1->tag != 6)goto
_LL7E9;else{_tmpFB2=_tmpFB1->f1;_tmpFB3=_tmpFB1->f2;_tmpFB4=_tmpFB1->f3;}}_LL7E8:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpFB2) && Cyc_Tcutil_cnst_exp(te,0,_tmpFB3))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpFB4);_LL7E9: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpFB5=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpFA7;if(_tmpFB5->tag != 9)goto _LL7EB;
else{_tmpFB6=_tmpFB5->f1;_tmpFB7=_tmpFB5->f2;}}_LL7EA: return Cyc_Tcutil_cnst_exp(
te,0,_tmpFB6) && Cyc_Tcutil_cnst_exp(te,0,_tmpFB7);_LL7EB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpFB8=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpFA7;if(_tmpFB8->tag != 13)
goto _LL7ED;else{_tmpFB9=_tmpFB8->f1;}}_LL7EC: _tmpFBB=_tmpFB9;goto _LL7EE;_LL7ED: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpFBA=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpFA7;if(_tmpFBA->tag != 14)goto _LL7EF;else{_tmpFBB=_tmpFBA->f1;}}_LL7EE: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpFBB);_LL7EF: {struct Cyc_Absyn_Cast_e_struct*
_tmpFBC=(struct Cyc_Absyn_Cast_e_struct*)_tmpFA7;if(_tmpFBC->tag != 15)goto _LL7F1;
else{_tmpFBD=(void*)_tmpFBC->f1;_tmpFBE=_tmpFBC->f2;_tmpFBF=_tmpFBC->f4;if(
_tmpFBF != Cyc_Absyn_No_coercion)goto _LL7F1;}}_LL7F0: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpFBE);_LL7F1: {struct Cyc_Absyn_Cast_e_struct*_tmpFC0=(struct Cyc_Absyn_Cast_e_struct*)
_tmpFA7;if(_tmpFC0->tag != 15)goto _LL7F3;else{_tmpFC1=(void*)_tmpFC0->f1;_tmpFC2=
_tmpFC0->f2;}}_LL7F2: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpFC2);_LL7F3: {
struct Cyc_Absyn_Address_e_struct*_tmpFC3=(struct Cyc_Absyn_Address_e_struct*)
_tmpFA7;if(_tmpFC3->tag != 16)goto _LL7F5;else{_tmpFC4=_tmpFC3->f1;}}_LL7F4: return
Cyc_Tcutil_cnst_exp(te,1,_tmpFC4);_LL7F5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpFC5=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpFA7;if(_tmpFC5->tag != 28)
goto _LL7F7;else{_tmpFC6=_tmpFC5->f2;_tmpFC7=_tmpFC5->f3;}}_LL7F6: return Cyc_Tcutil_cnst_exp(
te,0,_tmpFC6) && Cyc_Tcutil_cnst_exp(te,0,_tmpFC7);_LL7F7: {struct Cyc_Absyn_Array_e_struct*
_tmpFC8=(struct Cyc_Absyn_Array_e_struct*)_tmpFA7;if(_tmpFC8->tag != 27)goto _LL7F9;
else{_tmpFC9=_tmpFC8->f1;}}_LL7F8: _tmpFCB=_tmpFC9;goto _LL7FA;_LL7F9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpFCA=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpFA7;if(_tmpFCA->tag != 30)goto
_LL7FB;else{_tmpFCB=_tmpFCA->f2;}}_LL7FA: _tmpFCD=_tmpFCB;goto _LL7FC;_LL7FB: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpFCC=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpFA7;if(_tmpFCC->tag != 29)goto _LL7FD;else{_tmpFCD=_tmpFCC->f3;}}_LL7FC: for(0;
_tmpFCD != 0;_tmpFCD=_tmpFCD->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)
_tmpFCD->hd)).f2))return 0;}return 1;_LL7FD: {struct Cyc_Absyn_Primop_e_struct*
_tmpFCE=(struct Cyc_Absyn_Primop_e_struct*)_tmpFA7;if(_tmpFCE->tag != 3)goto _LL7FF;
else{_tmpFCF=_tmpFCE->f1;_tmpFD0=_tmpFCE->f2;}}_LL7FE: _tmpFD2=_tmpFD0;goto _LL800;
_LL7FF: {struct Cyc_Absyn_Tuple_e_struct*_tmpFD1=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpFA7;if(_tmpFD1->tag != 25)goto _LL801;else{_tmpFD2=_tmpFD1->f1;}}_LL800:
_tmpFD4=_tmpFD2;goto _LL802;_LL801: {struct Cyc_Absyn_Datatype_e_struct*_tmpFD3=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpFA7;if(_tmpFD3->tag != 31)goto _LL803;else{
_tmpFD4=_tmpFD3->f1;}}_LL802: for(0;_tmpFD4 != 0;_tmpFD4=_tmpFD4->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpFD4->hd))return 0;}return 1;_LL803:;_LL804: return 0;
_LL7D8:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmpFDD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFE3;
struct Cyc_Absyn_PtrAtts _tmpFE4;union Cyc_Absyn_Constraint*_tmpFE5;union Cyc_Absyn_Constraint*
_tmpFE6;struct Cyc_Absyn_ArrayInfo _tmpFE8;void*_tmpFE9;struct Cyc_List_List*
_tmpFEB;struct Cyc_Absyn_AggrInfo _tmpFED;union Cyc_Absyn_AggrInfoU _tmpFEE;struct
Cyc_List_List*_tmpFEF;struct Cyc_List_List*_tmpFF1;_LL816: {struct Cyc_Absyn_VoidType_struct*
_tmpFDE=(struct Cyc_Absyn_VoidType_struct*)_tmpFDD;if(_tmpFDE->tag != 0)goto _LL818;}
_LL817: goto _LL819;_LL818: {struct Cyc_Absyn_IntType_struct*_tmpFDF=(struct Cyc_Absyn_IntType_struct*)
_tmpFDD;if(_tmpFDF->tag != 6)goto _LL81A;}_LL819: goto _LL81B;_LL81A: {struct Cyc_Absyn_FloatType_struct*
_tmpFE0=(struct Cyc_Absyn_FloatType_struct*)_tmpFDD;if(_tmpFE0->tag != 7)goto
_LL81C;}_LL81B: goto _LL81D;_LL81C: {struct Cyc_Absyn_DoubleType_struct*_tmpFE1=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFDD;if(_tmpFE1->tag != 8)goto _LL81E;}
_LL81D: return 1;_LL81E: {struct Cyc_Absyn_PointerType_struct*_tmpFE2=(struct Cyc_Absyn_PointerType_struct*)
_tmpFDD;if(_tmpFE2->tag != 5)goto _LL820;else{_tmpFE3=_tmpFE2->f1;_tmpFE4=_tmpFE3.ptr_atts;
_tmpFE5=_tmpFE4.nullable;_tmpFE6=_tmpFE4.bounds;}}_LL81F: {void*_tmpFF4=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpFE6);_LL82F: {struct Cyc_Absyn_DynEither_b_struct*_tmpFF5=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFF4;if(_tmpFF5->tag != 0)goto _LL831;}_LL830: return 1;_LL831:;_LL832: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpFE5);_LL82E:;}
_LL820: {struct Cyc_Absyn_ArrayType_struct*_tmpFE7=(struct Cyc_Absyn_ArrayType_struct*)
_tmpFDD;if(_tmpFE7->tag != 9)goto _LL822;else{_tmpFE8=_tmpFE7->f1;_tmpFE9=_tmpFE8.elt_type;}}
_LL821: return Cyc_Tcutil_supports_default(_tmpFE9);_LL822: {struct Cyc_Absyn_TupleType_struct*
_tmpFEA=(struct Cyc_Absyn_TupleType_struct*)_tmpFDD;if(_tmpFEA->tag != 11)goto
_LL824;else{_tmpFEB=_tmpFEA->f1;}}_LL823: for(0;_tmpFEB != 0;_tmpFEB=_tmpFEB->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpFEB->hd)).f2))return 0;}
return 1;_LL824: {struct Cyc_Absyn_AggrType_struct*_tmpFEC=(struct Cyc_Absyn_AggrType_struct*)
_tmpFDD;if(_tmpFEC->tag != 12)goto _LL826;else{_tmpFED=_tmpFEC->f1;_tmpFEE=_tmpFED.aggr_info;
_tmpFEF=_tmpFED.targs;}}_LL825: {struct Cyc_Absyn_Aggrdecl*_tmpFF6=Cyc_Absyn_get_known_aggrdecl(
_tmpFEE);if(_tmpFF6->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpFF6->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpFF6->tvs,_tmpFEF,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFF6->impl))->fields);}
_LL826: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFF0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFDD;if(_tmpFF0->tag != 13)goto _LL828;else{_tmpFF1=_tmpFF0->f2;}}_LL827: return
Cyc_Tcutil_fields_support_default(0,0,_tmpFF1);_LL828: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFF2=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpFDD;if(_tmpFF2->tag != 15)goto
_LL82A;}_LL829: goto _LL82B;_LL82A: {struct Cyc_Absyn_EnumType_struct*_tmpFF3=(
struct Cyc_Absyn_EnumType_struct*)_tmpFDD;if(_tmpFF3->tag != 14)goto _LL82C;}_LL82B:
return 1;_LL82C:;_LL82D: return 0;_LL815:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmpFF7=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFF7;
_push_region(rgn);{struct Cyc_List_List*_tmpFF8=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmpFF8,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmpFF9=0;_npop_handler(0);return _tmpFF9;}}}{int _tmpFFA=1;_npop_handler(0);
return _tmpFFA;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(void*t){void*
_tmpFFB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1000;struct Cyc_Absyn_PtrAtts
_tmp1001;union Cyc_Absyn_Constraint*_tmp1002;union Cyc_Absyn_Constraint*_tmp1003;
_LL834: {struct Cyc_Absyn_IntType_struct*_tmpFFC=(struct Cyc_Absyn_IntType_struct*)
_tmpFFB;if(_tmpFFC->tag != 6)goto _LL836;}_LL835: goto _LL837;_LL836: {struct Cyc_Absyn_FloatType_struct*
_tmpFFD=(struct Cyc_Absyn_FloatType_struct*)_tmpFFB;if(_tmpFFD->tag != 7)goto
_LL838;}_LL837: goto _LL839;_LL838: {struct Cyc_Absyn_DoubleType_struct*_tmpFFE=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFFB;if(_tmpFFE->tag != 8)goto _LL83A;}
_LL839: return 1;_LL83A: {struct Cyc_Absyn_PointerType_struct*_tmpFFF=(struct Cyc_Absyn_PointerType_struct*)
_tmpFFB;if(_tmpFFF->tag != 5)goto _LL83C;else{_tmp1000=_tmpFFF->f1;_tmp1001=
_tmp1000.ptr_atts;_tmp1002=_tmp1001.nullable;_tmp1003=_tmp1001.bounds;}}_LL83B: {
void*_tmp1004=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp1003);_LL83F: {struct Cyc_Absyn_DynEither_b_struct*
_tmp1005=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1004;if(_tmp1005->tag != 0)goto
_LL841;}_LL840: return 0;_LL841:;_LL842: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp1002);_LL83E:;}_LL83C:;_LL83D: return 0;_LL833:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp1006=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp1008;void*_tmp1009;struct Cyc_Absyn_FnInfo _tmp100B;struct Cyc_List_List*
_tmp100C;_LL844: {struct Cyc_Absyn_PointerType_struct*_tmp1007=(struct Cyc_Absyn_PointerType_struct*)
_tmp1006;if(_tmp1007->tag != 5)goto _LL846;else{_tmp1008=_tmp1007->f1;_tmp1009=
_tmp1008.elt_typ;}}_LL845: return Cyc_Tcutil_is_noreturn(_tmp1009);_LL846: {struct
Cyc_Absyn_FnType_struct*_tmp100A=(struct Cyc_Absyn_FnType_struct*)_tmp1006;if(
_tmp100A->tag != 10)goto _LL848;else{_tmp100B=_tmp100A->f1;_tmp100C=_tmp100B.attributes;}}
_LL847: for(0;_tmp100C != 0;_tmp100C=_tmp100C->tl){void*_tmp100D=(void*)_tmp100C->hd;
_LL84B: {struct Cyc_Absyn_Noreturn_att_struct*_tmp100E=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp100D;if(_tmp100E->tag != 4)goto _LL84D;}_LL84C: return 1;_LL84D:;_LL84E:
continue;_LL84A:;}goto _LL843;_LL848:;_LL849: goto _LL843;_LL843:;}return 0;}struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp100F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp1011;struct Cyc_List_List*
_tmp1012;struct Cyc_List_List**_tmp1013;_LL850: {struct Cyc_Absyn_FnType_struct*
_tmp1010=(struct Cyc_Absyn_FnType_struct*)_tmp100F;if(_tmp1010->tag != 10)goto
_LL852;else{_tmp1011=_tmp1010->f1;_tmp1012=_tmp1011.attributes;_tmp1013=(struct
Cyc_List_List**)&(_tmp1010->f1).attributes;}}_LL851: {struct Cyc_List_List*
_tmp1014=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp15DE;*_tmp1013=((_tmp15DE=_cycalloc(sizeof(*_tmp15DE)),((
_tmp15DE->hd=(void*)atts->hd,((_tmp15DE->tl=*_tmp1013,_tmp15DE))))));}else{
struct Cyc_List_List*_tmp15DF;_tmp1014=((_tmp15DF=_cycalloc(sizeof(*_tmp15DF)),((
_tmp15DF->hd=(void*)atts->hd,((_tmp15DF->tl=_tmp1014,_tmp15DF))))));}}return
_tmp1014;}_LL852:;_LL853: {const char*_tmp15E2;void*_tmp15E1;(_tmp15E1=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15E2="transfer_fn_type_atts",
_tag_dyneither(_tmp15E2,sizeof(char),22))),_tag_dyneither(_tmp15E1,sizeof(void*),
0)));}_LL84F:;}
