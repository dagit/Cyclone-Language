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
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp1083;void*_tmp1082[2];const char*
_tmp1081;const char*_tmp1080;struct Cyc_String_pa_struct _tmp107F;struct Cyc_String_pa_struct
_tmp107E;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp107E.tag=0,((
_tmp107E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp107F.tag=0,((_tmp107F.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp1080="const ",_tag_dyneither(_tmp1080,sizeof(char),7)):((
_tmp1081="",_tag_dyneither(_tmp1081,sizeof(char),1)))),((_tmp1082[0]=& _tmp107F,((
_tmp1082[1]=& _tmp107E,Cyc_aprintf(((_tmp1083="%s%s",_tag_dyneither(_tmp1083,
sizeof(char),5))),_tag_dyneither(_tmp1082,sizeof(void*),2))))))))))))));const
char*_tmp108C;void*_tmp108B[2];const char*_tmp108A;const char*_tmp1089;struct Cyc_String_pa_struct
_tmp1088;struct Cyc_String_pa_struct _tmp1087;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp1087.tag=0,((_tmp1087.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp1088.tag=0,((
_tmp1088.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp1089="const ",
_tag_dyneither(_tmp1089,sizeof(char),7)):((_tmp108A="",_tag_dyneither(_tmp108A,
sizeof(char),1)))),((_tmp108B[0]=& _tmp1088,((_tmp108B[1]=& _tmp1087,Cyc_aprintf(((
_tmp108C="%s%s",_tag_dyneither(_tmp108C,sizeof(char),5))),_tag_dyneither(
_tmp108B,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp1090;void*
_tmp108F[1];struct Cyc_String_pa_struct _tmp108E;(_tmp108E.tag=0,((_tmp108E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp108F[0]=& _tmp108E,Cyc_fprintf(
Cyc_stderr,((_tmp1090="  %s",_tag_dyneither(_tmp1090,sizeof(char),5))),
_tag_dyneither(_tmp108F,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp1093;void*_tmp1092;(_tmp1092=0,Cyc_fprintf(
Cyc_stderr,((_tmp1093="\n\t",_tag_dyneither(_tmp1093,sizeof(char),3))),
_tag_dyneither(_tmp1092,sizeof(void*),0)));}pos=8;}else{{const char*_tmp1096;void*
_tmp1095;(_tmp1095=0,Cyc_fprintf(Cyc_stderr,((_tmp1096=" ",_tag_dyneither(
_tmp1096,sizeof(char),2))),_tag_dyneither(_tmp1095,sizeof(void*),0)));}++ pos;}{
const char*_tmp1099;void*_tmp1098;(_tmp1098=0,Cyc_fprintf(Cyc_stderr,((_tmp1099="and ",
_tag_dyneither(_tmp1099,sizeof(char),5))),_tag_dyneither(_tmp1098,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp109C;void*_tmp109B;(_tmp109B=0,Cyc_fprintf(Cyc_stderr,((_tmp109C="\n\t",
_tag_dyneither(_tmp109C,sizeof(char),3))),_tag_dyneither(_tmp109B,sizeof(void*),
0)));}pos=8;}{const char*_tmp10A0;void*_tmp109F[1];struct Cyc_String_pa_struct
_tmp109E;(_tmp109E.tag=0,((_tmp109E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp109F[0]=& _tmp109E,Cyc_fprintf(Cyc_stderr,((_tmp10A0="%s ",
_tag_dyneither(_tmp10A0,sizeof(char),4))),_tag_dyneither(_tmp109F,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp10A3;void*_tmp10A2;(_tmp10A2=0,Cyc_fprintf(Cyc_stderr,((_tmp10A3="\n\t",
_tag_dyneither(_tmp10A3,sizeof(char),3))),_tag_dyneither(_tmp10A2,sizeof(void*),
0)));}pos=8;}{const char*_tmp10A6;void*_tmp10A5;(_tmp10A5=0,Cyc_fprintf(Cyc_stderr,((
_tmp10A6="are not compatible. ",_tag_dyneither(_tmp10A6,sizeof(char),21))),
_tag_dyneither(_tmp10A5,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp10A9;void*_tmp10A8;(_tmp10A8=0,
Cyc_fprintf(Cyc_stderr,((_tmp10A9="\n\t",_tag_dyneither(_tmp10A9,sizeof(char),3))),
_tag_dyneither(_tmp10A8,sizeof(void*),0)));}{const char*_tmp10AD;void*_tmp10AC[1];
struct Cyc_String_pa_struct _tmp10AB;(_tmp10AB.tag=0,((_tmp10AB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp10AC[0]=&
_tmp10AB,Cyc_fprintf(Cyc_stderr,((_tmp10AD="%s",_tag_dyneither(_tmp10AD,sizeof(
char),3))),_tag_dyneither(_tmp10AC,sizeof(void*),1)))))));};}{const char*_tmp10B0;
void*_tmp10AF;(_tmp10AF=0,Cyc_fprintf(Cyc_stderr,((_tmp10B0="\n",_tag_dyneither(
_tmp10B0,sizeof(char),2))),_tag_dyneither(_tmp10AF,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp10B4;
void*_tmp10B3[1];struct Cyc_String_pa_struct _tmp10B2;(_tmp10B2.tag=0,((_tmp10B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10B3[0]=& _tmp10B2,Cyc_fprintf(
Cyc_stderr,((_tmp10B4="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp10B4,sizeof(char),36))),_tag_dyneither(_tmp10B3,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp10B7;
struct Cyc_Core_Impossible_struct*_tmp10B6;(int)_throw((void*)((_tmp10B6=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10B6)),((_tmp10B6[0]=((
_tmp10B7.tag=Cyc_Core_Impossible,((_tmp10B7.f1=msg,_tmp10B7)))),_tmp10B6)))));};}
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*
tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=
tvs->tl){const char*_tmp10BC;void*_tmp10BB[2];struct Cyc_String_pa_struct _tmp10BA;
struct Cyc_String_pa_struct _tmp10B9;(_tmp10B9.tag=0,((_tmp10B9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind)),((_tmp10BA.tag=0,((_tmp10BA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((
_tmp10BB[0]=& _tmp10BA,((_tmp10BB[1]=& _tmp10B9,Cyc_fprintf(Cyc_stderr,((_tmp10BC="%s::%s ",
_tag_dyneither(_tmp10BC,sizeof(char),8))),_tag_dyneither(_tmp10BB,sizeof(void*),
2)))))))))))));}{const char*_tmp10BF;void*_tmp10BE;(_tmp10BE=0,Cyc_fprintf(Cyc_stderr,((
_tmp10BF="\n",_tag_dyneither(_tmp10BF,sizeof(char),2))),_tag_dyneither(_tmp10BE,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}static struct
Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=
0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(
Cyc_Core_heap_region,fmt,ap);{struct Cyc_List_List*_tmp10C0;Cyc_Tcutil_warning_segs=((
_tmp10C0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10C0)),((_tmp10C0->hd=
sg,((_tmp10C0->tl=Cyc_Tcutil_warning_segs,_tmp10C0))))));}{struct _dyneither_ptr*
_tmp10C3;struct Cyc_List_List*_tmp10C2;Cyc_Tcutil_warning_msgs=((_tmp10C2=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10C2)),((_tmp10C2->hd=((
_tmp10C3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10C3)),((_tmp10C3[0]=
msg,_tmp10C3)))),((_tmp10C2->tl=Cyc_Tcutil_warning_msgs,_tmp10C2))))));};}void
Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)return;{const char*
_tmp10C6;void*_tmp10C5;(_tmp10C5=0,Cyc_fprintf(Cyc_stderr,((_tmp10C6="***Warnings***\n",
_tag_dyneither(_tmp10C6,sizeof(char),16))),_tag_dyneither(_tmp10C5,sizeof(void*),
0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs
!= 0){{const char*_tmp10CB;void*_tmp10CA[2];struct Cyc_String_pa_struct _tmp10C9;
struct Cyc_String_pa_struct _tmp10C8;(_tmp10C8.tag=0,((_tmp10C8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10C9.tag=0,((_tmp10C9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)
_check_null(_tmp36))->hd)),((_tmp10CA[0]=& _tmp10C9,((_tmp10CA[1]=& _tmp10C8,Cyc_fprintf(
Cyc_stderr,((_tmp10CB="%s: %s\n",_tag_dyneither(_tmp10CB,sizeof(char),8))),
_tag_dyneither(_tmp10CA,sizeof(void*),2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*
_tmp10CE;void*_tmp10CD;(_tmp10CD=0,Cyc_fprintf(Cyc_stderr,((_tmp10CE="**************\n",
_tag_dyneither(_tmp10CE,sizeof(char),16))),_tag_dyneither(_tmp10CD,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10CF;*_tmp44=((_tmp10CF=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10CF)),((_tmp10CF[0]=t2,
_tmp10CF))));}return t2;}_LL7: {struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)
_tmp3D;if(_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10D0;*_tmp47=((_tmp10D0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp10D0)),((_tmp10D0->v=t2,_tmp10D0))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
struct Cyc_Absyn_Unknown_kb_struct _tmp10D3;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10D2;return(void*)((_tmp10D2=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp10D2)),((_tmp10D2[0]=((_tmp10D3.tag=1,((_tmp10D3.f1=0,_tmp10D3)))),_tmp10D2))));}
_LL1E: {struct Cyc_Absyn_Less_kb_struct*_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)
_tmp59;if(_tmp5B->tag != 2)goto _LL20;else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct
_tmp10D6;struct Cyc_Absyn_Less_kb_struct*_tmp10D5;return(void*)((_tmp10D5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10D5)),((_tmp10D5[0]=((
_tmp10D6.tag=2,((_tmp10D6.f1=0,((_tmp10D6.f2=_tmp5C,_tmp10D6)))))),_tmp10D5))));}
_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10D7;return(_tmp10D7=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10D7)),((_tmp10D7->name=tv->name,((
_tmp10D7->identity=- 1,((_tmp10D7->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10D7)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple9*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple9*_tmp10D8;return(_tmp10D8=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp10D8)),((_tmp10D8->f1=_tmp64,((_tmp10D8->f2=_tmp65,((_tmp10D8->f3=
Cyc_Tcutil_copy_type(_tmp66),_tmp10D8)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(
struct _tuple11*arg){struct _tuple11 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple11*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple11*_tmp10D9;return(_tmp10D9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp10D9)),((_tmp10D9->f1=_tmp6A,((_tmp10D9->f2=Cyc_Tcutil_copy_type(
_tmp6B),_tmp10D9)))));};}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmp10DA;return(_tmp10DA=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10DA)),((_tmp10DA->name=f->name,((
_tmp10DA->tq=f->tq,((_tmp10DA->type=Cyc_Tcutil_copy_type(f->type),((_tmp10DA->width=
f->width,((_tmp10DA->attributes=f->attributes,_tmp10DA)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10DB;return(_tmp10DB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp10DB)),((_tmp10DB->f1=Cyc_Tcutil_copy_type(_tmp70),((_tmp10DB->f2=
Cyc_Tcutil_copy_type(_tmp71),_tmp10DB)))));};}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*
_tmp10DC;return(_tmp10DC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10DC)),((
_tmp10DC->name=f->name,((_tmp10DC->tag=f->tag,((_tmp10DC->loc=f->loc,_tmp10DC)))))));}
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){struct Cyc_Absyn_VarType_struct
_tmp10DF;struct Cyc_Absyn_VarType_struct*_tmp10DE;return(void*)((_tmp10DE=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10DE)),((_tmp10DE[0]=((
_tmp10DF.tag=2,((_tmp10DF.f1=Cyc_Tcutil_copy_tvar(t),_tmp10DF)))),_tmp10DE))));}
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
else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct _tmp10E2;struct
Cyc_Absyn_VarType_struct*_tmp10E1;return(void*)((_tmp10E1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp10E1)),((_tmp10E1[0]=((_tmp10E2.tag=2,((_tmp10E2.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10E2)))),_tmp10E1))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10E8;struct Cyc_Absyn_DatatypeInfo _tmp10E7;
struct Cyc_Absyn_DatatypeType_struct*_tmp10E6;return(void*)((_tmp10E6=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp10E6)),((_tmp10E6[0]=((
_tmp10E8.tag=3,((_tmp10E8.f1=((_tmp10E7.datatype_info=_tmp7D,((_tmp10E7.targs=
Cyc_Tcutil_copy_types(_tmp7E),_tmp10E7)))),_tmp10E8)))),_tmp10E6))));}_LL2B: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10EE;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10ED;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10EC;return(void*)((_tmp10EC=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp10EC)),((_tmp10EC[0]=((_tmp10EE.tag=4,((_tmp10EE.f1=((_tmp10ED.field_info=
_tmp81,((_tmp10ED.targs=Cyc_Tcutil_copy_types(_tmp82),_tmp10ED)))),_tmp10EE)))),
_tmp10EC))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*_tmp83=(struct Cyc_Absyn_PointerType_struct*)
_tmp76;if(_tmp83->tag != 5)goto _LL2F;else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;
_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;
_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*
_tmpE9=Cyc_Tcutil_copy_type(_tmp85);void*_tmpEA=Cyc_Tcutil_copy_type(_tmp88);
union Cyc_Absyn_Constraint*_tmpEB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual _tmpEC=_tmp86;union
Cyc_Absyn_Constraint*_tmpED=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*_tmpEE=((union
Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp10F8;struct Cyc_Absyn_PtrAtts
_tmp10F7;struct Cyc_Absyn_PtrInfo _tmp10F6;struct Cyc_Absyn_PointerType_struct*
_tmp10F5;return(void*)((_tmp10F5=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp10F5)),((_tmp10F5[0]=((_tmp10F8.tag=5,((_tmp10F8.f1=((_tmp10F6.elt_typ=
_tmpE9,((_tmp10F6.elt_tq=_tmpEC,((_tmp10F6.ptr_atts=((_tmp10F7.rgn=_tmpEA,((
_tmp10F7.nullable=_tmpEB,((_tmp10F7.bounds=_tmpED,((_tmp10F7.zero_term=_tmpEE,((
_tmp10F7.ptrloc=_tmp8C,_tmp10F7)))))))))),_tmp10F6)))))),_tmp10F8)))),_tmp10F5))));}
_LL2F: {struct Cyc_Absyn_IntType_struct*_tmp8D=(struct Cyc_Absyn_IntType_struct*)
_tmp76;if(_tmp8D->tag != 6)goto _LL31;}_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*
_tmp8E=(struct Cyc_Absyn_FloatType_struct*)_tmp76;if(_tmp8E->tag != 7)goto _LL33;}
_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_DoubleType_struct*_tmp8F=(struct Cyc_Absyn_DoubleType_struct*)
_tmp76;if(_tmp8F->tag != 8)goto _LL35;}_LL34: return t;_LL35: {struct Cyc_Absyn_ArrayType_struct*
_tmp90=(struct Cyc_Absyn_ArrayType_struct*)_tmp76;if(_tmp90->tag != 9)goto _LL37;
else{_tmp91=_tmp90->f1;_tmp92=_tmp91.elt_type;_tmp93=_tmp91.tq;_tmp94=_tmp91.num_elts;
_tmp95=_tmp91.zero_term;_tmp96=_tmp91.zt_loc;}}_LL36: {struct Cyc_Absyn_ArrayType_struct
_tmp10FE;struct Cyc_Absyn_ArrayInfo _tmp10FD;struct Cyc_Absyn_ArrayType_struct*
_tmp10FC;return(void*)((_tmp10FC=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp10FC)),((_tmp10FC[0]=((_tmp10FE.tag=9,((_tmp10FE.f1=((_tmp10FD.elt_type=Cyc_Tcutil_copy_type(
_tmp92),((_tmp10FD.tq=_tmp93,((_tmp10FD.num_elts=_tmp94,((_tmp10FD.zero_term=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp95),((_tmp10FD.zt_loc=_tmp96,_tmp10FD)))))))))),_tmp10FE)))),_tmp10FC))));}
_LL37: {struct Cyc_Absyn_FnType_struct*_tmp97=(struct Cyc_Absyn_FnType_struct*)
_tmp76;if(_tmp97->tag != 10)goto _LL39;else{_tmp98=_tmp97->f1;_tmp99=_tmp98.tvars;
_tmp9A=_tmp98.effect;_tmp9B=_tmp98.ret_tqual;_tmp9C=_tmp98.ret_typ;_tmp9D=_tmp98.args;
_tmp9E=_tmp98.c_varargs;_tmp9F=_tmp98.cyc_varargs;_tmpA0=_tmp98.rgn_po;_tmpA1=
_tmp98.attributes;}}_LL38: {struct Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmp99);struct Cyc_Core_Opt*_tmp10FF;struct Cyc_Core_Opt*
_tmpF7=_tmp9A == 0?0:((_tmp10FF=_cycalloc(sizeof(*_tmp10FF)),((_tmp10FF->v=Cyc_Tcutil_copy_type((
void*)_tmp9A->v),_tmp10FF))));void*_tmpF8=Cyc_Tcutil_copy_type(_tmp9C);struct Cyc_List_List*
_tmpF9=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp9D);int _tmpFA=_tmp9E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp9F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp9F;struct Cyc_Absyn_VarargInfo*_tmp1100;cyc_varargs2=((_tmp1100=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1100)),((_tmp1100->name=cv->name,((
_tmp1100->tq=cv->tq,((_tmp1100->type=Cyc_Tcutil_copy_type(cv->type),((_tmp1100->inject=
cv->inject,_tmp1100))))))))));}{struct Cyc_List_List*_tmpFC=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmpA0);struct Cyc_List_List*_tmpFD=_tmpA1;struct Cyc_Absyn_FnType_struct _tmp1106;
struct Cyc_Absyn_FnInfo _tmp1105;struct Cyc_Absyn_FnType_struct*_tmp1104;return(
void*)((_tmp1104=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1104)),((
_tmp1104[0]=((_tmp1106.tag=10,((_tmp1106.f1=((_tmp1105.tvars=_tmpF6,((_tmp1105.effect=
_tmpF7,((_tmp1105.ret_tqual=_tmp9B,((_tmp1105.ret_typ=_tmpF8,((_tmp1105.args=
_tmpF9,((_tmp1105.c_varargs=_tmpFA,((_tmp1105.cyc_varargs=cyc_varargs2,((
_tmp1105.rgn_po=_tmpFC,((_tmp1105.attributes=_tmpFD,_tmp1105)))))))))))))))))),
_tmp1106)))),_tmp1104))));};}_LL39: {struct Cyc_Absyn_TupleType_struct*_tmpA2=(
struct Cyc_Absyn_TupleType_struct*)_tmp76;if(_tmpA2->tag != 11)goto _LL3B;else{
_tmpA3=_tmpA2->f1;}}_LL3A: {struct Cyc_Absyn_TupleType_struct _tmp1109;struct Cyc_Absyn_TupleType_struct*
_tmp1108;return(void*)((_tmp1108=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=11,((_tmp1109.f1=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,
_tmpA3),_tmp1109)))),_tmp1108))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*
_tmpA4=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpA4->tag != 12)goto _LL3D;
else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA5.aggr_info;if((_tmpA6.UnknownAggr).tag != 1)
goto _LL3D;_tmpA7=(struct _tuple4)(_tmpA6.UnknownAggr).val;_tmpA8=_tmpA7.f1;_tmpA9=
_tmpA7.f2;_tmpAA=_tmpA7.f3;_tmpAB=_tmpA5.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmp110F;struct Cyc_Absyn_AggrInfo _tmp110E;struct Cyc_Absyn_AggrType_struct*
_tmp110D;return(void*)((_tmp110D=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp110D)),((_tmp110D[0]=((_tmp110F.tag=12,((_tmp110F.f1=((_tmp110E.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA8,_tmpA9,_tmpAA),((_tmp110E.targs=Cyc_Tcutil_copy_types(
_tmpAB),_tmp110E)))),_tmp110F)))),_tmp110D))));}_LL3D: {struct Cyc_Absyn_AggrType_struct*
_tmpAC=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAC->tag != 12)goto _LL3F;
else{_tmpAD=_tmpAC->f1;_tmpAE=_tmpAD.aggr_info;if((_tmpAE.KnownAggr).tag != 2)
goto _LL3F;_tmpAF=(struct Cyc_Absyn_Aggrdecl**)(_tmpAE.KnownAggr).val;_tmpB0=
_tmpAD.targs;}}_LL3E: {struct Cyc_Absyn_AggrType_struct _tmp1115;struct Cyc_Absyn_AggrInfo
_tmp1114;struct Cyc_Absyn_AggrType_struct*_tmp1113;return(void*)((_tmp1113=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1113)),((_tmp1113[0]=((
_tmp1115.tag=12,((_tmp1115.f1=((_tmp1114.aggr_info=Cyc_Absyn_KnownAggr(_tmpAF),((
_tmp1114.targs=Cyc_Tcutil_copy_types(_tmpB0),_tmp1114)))),_tmp1115)))),_tmp1113))));}
_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*_tmpB1=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp76;if(_tmpB1->tag != 13)goto _LL41;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}
_LL40: {struct Cyc_Absyn_AnonAggrType_struct _tmp1118;struct Cyc_Absyn_AnonAggrType_struct*
_tmp1117;return(void*)((_tmp1117=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1117)),((_tmp1117[0]=((_tmp1118.tag=13,((_tmp1118.f1=_tmpB2,((_tmp1118.f2=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpB3),_tmp1118)))))),
_tmp1117))));}_LL41: {struct Cyc_Absyn_EnumType_struct*_tmpB4=(struct Cyc_Absyn_EnumType_struct*)
_tmp76;if(_tmpB4->tag != 14)goto _LL43;else{_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;}}
_LL42: {struct Cyc_Absyn_EnumType_struct _tmp111B;struct Cyc_Absyn_EnumType_struct*
_tmp111A;return(void*)((_tmp111A=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp111A)),((_tmp111A[0]=((_tmp111B.tag=14,((_tmp111B.f1=_tmpB5,((_tmp111B.f2=
_tmpB6,_tmp111B)))))),_tmp111A))));}_LL43: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB7->tag != 15)goto
_LL45;else{_tmpB8=_tmpB7->f1;}}_LL44: {struct Cyc_Absyn_AnonEnumType_struct
_tmp111E;struct Cyc_Absyn_AnonEnumType_struct*_tmp111D;return(void*)((_tmp111D=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp111D)),((_tmp111D[0]=((
_tmp111E.tag=15,((_tmp111E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*
f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,
_tmpB8),_tmp111E)))),_tmp111D))));}_LL45: {struct Cyc_Absyn_TagType_struct*_tmpB9=(
struct Cyc_Absyn_TagType_struct*)_tmp76;if(_tmpB9->tag != 20)goto _LL47;else{_tmpBA=(
void*)_tmpB9->f1;}}_LL46: {struct Cyc_Absyn_TagType_struct _tmp1121;struct Cyc_Absyn_TagType_struct*
_tmp1120;return(void*)((_tmp1120=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1120)),((_tmp1120[0]=((_tmp1121.tag=20,((_tmp1121.f1=(void*)Cyc_Tcutil_copy_type(
_tmpBA),_tmp1121)))),_tmp1120))));}_LL47: {struct Cyc_Absyn_ValueofType_struct*
_tmpBB=(struct Cyc_Absyn_ValueofType_struct*)_tmp76;if(_tmpBB->tag != 19)goto _LL49;
else{_tmpBC=_tmpBB->f1;}}_LL48: {struct Cyc_Absyn_ValueofType_struct _tmp1124;
struct Cyc_Absyn_ValueofType_struct*_tmp1123;return(void*)((_tmp1123=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1123)),((_tmp1123[0]=((
_tmp1124.tag=19,((_tmp1124.f1=_tmpBC,_tmp1124)))),_tmp1123))));}_LL49: {struct
Cyc_Absyn_RgnHandleType_struct*_tmpBD=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp76;if(_tmpBD->tag != 16)goto _LL4B;else{_tmpBE=(void*)_tmpBD->f1;}}_LL4A: {
struct Cyc_Absyn_RgnHandleType_struct _tmp1127;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1126;return(void*)((_tmp1126=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1126)),((_tmp1126[0]=((_tmp1127.tag=16,((_tmp1127.f1=(void*)Cyc_Tcutil_copy_type(
_tmpBE),_tmp1127)))),_tmp1126))));}_LL4B: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBF=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBF->tag != 17)goto _LL4D;
else{_tmpC0=(void*)_tmpBF->f1;_tmpC1=(void*)_tmpBF->f2;}}_LL4C: {struct Cyc_Absyn_DynRgnType_struct
_tmp112A;struct Cyc_Absyn_DynRgnType_struct*_tmp1129;return(void*)((_tmp1129=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1129)),((_tmp1129[0]=((
_tmp112A.tag=17,((_tmp112A.f1=(void*)Cyc_Tcutil_copy_type(_tmpC0),((_tmp112A.f2=(
void*)Cyc_Tcutil_copy_type(_tmpC1),_tmp112A)))))),_tmp1129))));}_LL4D: {struct
Cyc_Absyn_TypedefType_struct*_tmpC2=(struct Cyc_Absyn_TypedefType_struct*)_tmp76;
if(_tmpC2->tag != 18)goto _LL4F;else{_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;_tmpC5=
_tmpC2->f3;}}_LL4E: {struct Cyc_Absyn_TypedefType_struct _tmp112D;struct Cyc_Absyn_TypedefType_struct*
_tmp112C;return(void*)((_tmp112C=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp112C)),((_tmp112C[0]=((_tmp112D.tag=18,((_tmp112D.f1=_tmpC3,((_tmp112D.f2=
Cyc_Tcutil_copy_types(_tmpC4),((_tmp112D.f3=_tmpC5,((_tmp112D.f4=0,_tmp112D)))))))))),
_tmp112C))));}_LL4F: {struct Cyc_Absyn_UniqueRgn_struct*_tmpC6=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp76;if(_tmpC6->tag != 22)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpC7=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp76;if(_tmpC7->tag != 23)goto _LL53;}
_LL52: goto _LL54;_LL53: {struct Cyc_Absyn_HeapRgn_struct*_tmpC8=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp76;if(_tmpC8->tag != 21)goto _LL55;}_LL54: return t;_LL55: {struct Cyc_Absyn_AccessEff_struct*
_tmpC9=(struct Cyc_Absyn_AccessEff_struct*)_tmp76;if(_tmpC9->tag != 24)goto _LL57;
else{_tmpCA=(void*)_tmpC9->f1;}}_LL56: {struct Cyc_Absyn_AccessEff_struct _tmp1130;
struct Cyc_Absyn_AccessEff_struct*_tmp112F;return(void*)((_tmp112F=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp112F)),((_tmp112F[0]=((_tmp1130.tag=24,((
_tmp1130.f1=(void*)Cyc_Tcutil_copy_type(_tmpCA),_tmp1130)))),_tmp112F))));}_LL57: {
struct Cyc_Absyn_JoinEff_struct*_tmpCB=(struct Cyc_Absyn_JoinEff_struct*)_tmp76;
if(_tmpCB->tag != 25)goto _LL59;else{_tmpCC=_tmpCB->f1;}}_LL58: {struct Cyc_Absyn_JoinEff_struct
_tmp1133;struct Cyc_Absyn_JoinEff_struct*_tmp1132;return(void*)((_tmp1132=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1132)),((_tmp1132[0]=((
_tmp1133.tag=25,((_tmp1133.f1=Cyc_Tcutil_copy_types(_tmpCC),_tmp1133)))),
_tmp1132))));}_LL59: {struct Cyc_Absyn_RgnsEff_struct*_tmpCD=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp76;if(_tmpCD->tag != 26)goto _LL5B;else{_tmpCE=(void*)_tmpCD->f1;}}_LL5A: {
struct Cyc_Absyn_RgnsEff_struct _tmp1136;struct Cyc_Absyn_RgnsEff_struct*_tmp1135;
return(void*)((_tmp1135=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1135)),((
_tmp1135[0]=((_tmp1136.tag=26,((_tmp1136.f1=(void*)Cyc_Tcutil_copy_type(_tmpCE),
_tmp1136)))),_tmp1135))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCF=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp76;if(_tmpCF->tag != 27)goto _LL5D;else{
_tmpD0=_tmpCF->f1;_tmpD1=*_tmpD0;_tmpD2=_tmpD1.r;{struct Cyc_Absyn_Aggr_td_struct*
_tmpD3=(struct Cyc_Absyn_Aggr_td_struct*)_tmpD2;if(_tmpD3->tag != 0)goto _LL5D;
else{_tmpD4=_tmpD3->f1;}};}}_LL5C: {struct Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,
_tmpD4->tvs);struct Cyc_Absyn_AggrType_struct _tmp113C;struct Cyc_Absyn_AggrInfo
_tmp113B;struct Cyc_Absyn_AggrType_struct*_tmp113A;return(void*)((_tmp113A=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp113A)),((_tmp113A[0]=((
_tmp113C.tag=12,((_tmp113C.f1=((_tmp113B.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD4->kind,
_tmpD4->name,0),((_tmp113B.targs=_tmp120,_tmp113B)))),_tmp113C)))),_tmp113A))));}
_LL5D: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD5=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD5->tag != 27)goto _LL5F;else{_tmpD6=_tmpD5->f1;_tmpD7=*_tmpD6;
_tmpD8=_tmpD7.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD9=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD8;if(_tmpD9->tag != 1)goto _LL5F;else{_tmpDA=_tmpD9->f1;}};}}_LL5E: {struct
Cyc_Absyn_EnumType_struct _tmp113F;struct Cyc_Absyn_EnumType_struct*_tmp113E;
return(void*)((_tmp113E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp113E)),((
_tmp113E[0]=((_tmp113F.tag=14,((_tmp113F.f1=_tmpDA->name,((_tmp113F.f2=0,
_tmp113F)))))),_tmp113E))));}_LL5F: {struct Cyc_Absyn_TypeDeclType_struct*_tmpDB=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp76;if(_tmpDB->tag != 27)goto _LL22;else{
_tmpDC=_tmpDB->f1;_tmpDD=*_tmpDC;_tmpDE=_tmpDD.r;{struct Cyc_Absyn_Datatype_td_struct*
_tmpDF=(struct Cyc_Absyn_Datatype_td_struct*)_tmpDE;if(_tmpDF->tag != 2)goto _LL22;
else{_tmpE0=_tmpDF->f1;}};}}_LL60: {struct Cyc_List_List*_tmp126=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,
_tmpE0->tvs);struct Cyc_Absyn_DatatypeType_struct _tmp1149;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp1148;struct Cyc_Absyn_DatatypeInfo _tmp1147;struct Cyc_Absyn_DatatypeType_struct*
_tmp1146;return(void*)((_tmp1146=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1146)),((_tmp1146[0]=((_tmp1149.tag=3,((_tmp1149.f1=((_tmp1147.datatype_info=
Cyc_Absyn_UnknownDatatype(((_tmp1148.name=_tmpE0->name,((_tmp1148.is_extensible=
0,_tmp1148))))),((_tmp1147.targs=_tmp126,_tmp1147)))),_tmp1149)))),_tmp1146))));}
_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){void*_tmp12B=d;struct Cyc_Absyn_Exp*
_tmp12D;struct _dyneither_ptr*_tmp12F;_LL62: {struct Cyc_Absyn_ArrayElement_struct*
_tmp12C=(struct Cyc_Absyn_ArrayElement_struct*)_tmp12B;if(_tmp12C->tag != 0)goto
_LL64;else{_tmp12D=_tmp12C->f1;}}_LL63: {struct Cyc_Absyn_ArrayElement_struct
_tmp114C;struct Cyc_Absyn_ArrayElement_struct*_tmp114B;return(void*)((_tmp114B=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp114B)),((_tmp114B[0]=((
_tmp114C.tag=0,((_tmp114C.f1=Cyc_Tcutil_deep_copy_exp(_tmp12D),_tmp114C)))),
_tmp114B))));}_LL64: {struct Cyc_Absyn_FieldName_struct*_tmp12E=(struct Cyc_Absyn_FieldName_struct*)
_tmp12B;if(_tmp12E->tag != 1)goto _LL61;else{_tmp12F=_tmp12E->f1;}}_LL65: return d;
_LL61:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple18*Cyc_Tcutil_copy_eds(struct _tuple18*e){struct _tuple18*_tmp114D;
return(_tmp114D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp114D)),((
_tmp114D->f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_designator,(e[0]).f1),((_tmp114D->f2=Cyc_Tcutil_deep_copy_exp((
e[0]).f2),_tmp114D)))));}struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*
e){void*_tmp133=e->r;union Cyc_Absyn_Cnst _tmp135;struct _tuple2*_tmp137;void*
_tmp138;struct _tuple2*_tmp13A;enum Cyc_Absyn_Primop _tmp13C;struct Cyc_List_List*
_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Core_Opt*_tmp140;struct Cyc_Absyn_Exp*
_tmp141;struct Cyc_Absyn_Exp*_tmp143;enum Cyc_Absyn_Incrementor _tmp144;struct Cyc_Absyn_Exp*
_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*
_tmp14A;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*
_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*
_tmp153;struct Cyc_List_List*_tmp154;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_List_List*
_tmp157;struct Cyc_Absyn_VarargCallInfo*_tmp158;struct Cyc_Absyn_Exp*_tmp15A;
struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_List_List*
_tmp15F;void*_tmp161;struct Cyc_Absyn_Exp*_tmp162;int _tmp163;enum Cyc_Absyn_Coercion
_tmp164;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*
_tmp169;void*_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;void*_tmp16F;void*_tmp170;
struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_Exp*_tmp174;struct _dyneither_ptr*
_tmp175;int _tmp176;int _tmp177;struct Cyc_Absyn_Exp*_tmp179;struct _dyneither_ptr*
_tmp17A;int _tmp17B;int _tmp17C;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_Absyn_Exp*
_tmp17F;struct Cyc_List_List*_tmp181;struct _tuple9*_tmp183;struct _tuple9 _tmp184;
struct Cyc_Core_Opt*_tmp185;struct Cyc_Absyn_Tqual _tmp186;void*_tmp187;struct Cyc_List_List*
_tmp188;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Vardecl*_tmp18C;struct Cyc_Absyn_Exp*
_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;int _tmp18F;struct _tuple2*_tmp191;struct Cyc_List_List*
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_Absyn_Aggrdecl*_tmp194;void*
_tmp196;struct Cyc_List_List*_tmp197;struct Cyc_List_List*_tmp199;struct Cyc_Absyn_Datatypedecl*
_tmp19A;struct Cyc_Absyn_Datatypefield*_tmp19B;struct _tuple2*_tmp19D;struct Cyc_Absyn_Enumdecl*
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
_tmp141=_tmp13E->f3;}}_LL70: {struct Cyc_Core_Opt*_tmp114E;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13F),(unsigned int)_tmp140?(_tmp114E=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp114E)),((_tmp114E->v=(void*)((enum Cyc_Absyn_Primop)
_tmp140->v),_tmp114E))): 0,Cyc_Tcutil_deep_copy_exp(_tmp141),e->loc);}_LL71: {
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
struct Cyc_Absyn_FnCall_e_struct _tmp1158;struct Cyc_Absyn_VarargInfo*_tmp1157;
struct Cyc_Absyn_VarargCallInfo*_tmp1156;struct Cyc_Absyn_FnCall_e_struct*_tmp1155;
return Cyc_Absyn_new_exp((void*)((_tmp1155=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1155)),((_tmp1155[0]=((_tmp1158.tag=11,((_tmp1158.f1=Cyc_Tcutil_deep_copy_exp(
_tmp156),((_tmp1158.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp157),((_tmp1158.f3=((_tmp1156=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1156)),((_tmp1156->num_varargs=_tmp1BD,((_tmp1156->injectors=_tmp1BE,((
_tmp1156->vai=((_tmp1157=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1157)),((
_tmp1157->name=_tmp1C2,((_tmp1157->tq=_tmp1C3,((_tmp1157->type=Cyc_Tcutil_copy_type(
_tmp1C4),((_tmp1157->inject=_tmp1C5,_tmp1157)))))))))),_tmp1156)))))))),_tmp1158)))))))),
_tmp1155)))),e->loc);};}_LLBC:;_LLBD: {struct Cyc_Absyn_FnCall_e_struct _tmp115B;
struct Cyc_Absyn_FnCall_e_struct*_tmp115A;return Cyc_Absyn_new_exp((void*)((
_tmp115A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp115A)),((_tmp115A[0]=((
_tmp115B.tag=11,((_tmp115B.f1=Cyc_Tcutil_deep_copy_exp(_tmp156),((_tmp115B.f2=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp157),((_tmp115B.f3=_tmp158,
_tmp115B)))))))),_tmp115A)))),e->loc);}_LLB9:;}_LL7F: {struct Cyc_Absyn_Throw_e_struct*
_tmp159=(struct Cyc_Absyn_Throw_e_struct*)_tmp133;if(_tmp159->tag != 12)goto _LL81;
else{_tmp15A=_tmp159->f1;}}_LL80: return Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(
_tmp15A),e->loc);_LL81: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp15B=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp133;if(_tmp15B->tag != 13)goto _LL83;else{
_tmp15C=_tmp15B->f1;}}_LL82: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
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
_tmp173->f4;}}_LL94: {struct Cyc_Absyn_AggrMember_e_struct _tmp115E;struct Cyc_Absyn_AggrMember_e_struct*
_tmp115D;return Cyc_Absyn_new_exp((void*)((_tmp115D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp115D)),((_tmp115D[0]=((_tmp115E.tag=22,((_tmp115E.f1=Cyc_Tcutil_deep_copy_exp(
_tmp174),((_tmp115E.f2=_tmp175,((_tmp115E.f3=_tmp176,((_tmp115E.f4=_tmp177,
_tmp115E)))))))))),_tmp115D)))),e->loc);}_LL95: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp178=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp133;if(_tmp178->tag != 23)goto
_LL97;else{_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;_tmp17B=_tmp178->f3;_tmp17C=
_tmp178->f4;}}_LL96: {struct Cyc_Absyn_AggrArrow_e_struct _tmp1161;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1160;return Cyc_Absyn_new_exp((void*)((_tmp1160=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1160)),((_tmp1160[0]=((_tmp1161.tag=23,((_tmp1161.f1=Cyc_Tcutil_deep_copy_exp(
_tmp179),((_tmp1161.f2=_tmp17A,((_tmp1161.f3=_tmp17B,((_tmp1161.f4=_tmp17C,
_tmp1161)))))))))),_tmp1160)))),e->loc);}_LL97: {struct Cyc_Absyn_Subscript_e_struct*
_tmp17D=(struct Cyc_Absyn_Subscript_e_struct*)_tmp133;if(_tmp17D->tag != 24)goto
_LL99;else{_tmp17E=_tmp17D->f1;_tmp17F=_tmp17D->f2;}}_LL98: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp17E),Cyc_Tcutil_deep_copy_exp(_tmp17F),e->loc);
_LL99: {struct Cyc_Absyn_Tuple_e_struct*_tmp180=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp133;if(_tmp180->tag != 25)goto _LL9B;else{_tmp181=_tmp180->f1;}}_LL9A: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp181),e->loc);
_LL9B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp182=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp133;if(_tmp182->tag != 26)goto _LL9D;else{_tmp183=_tmp182->f1;_tmp184=*_tmp183;
_tmp185=_tmp184.f1;_tmp186=_tmp184.f2;_tmp187=_tmp184.f3;_tmp188=_tmp182->f2;}}
_LL9C: {struct Cyc_Core_Opt*vopt1=_tmp185;if(_tmp185 != 0){struct Cyc_Core_Opt*
_tmp1162;vopt1=((_tmp1162=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1162)),((
_tmp1162->v=(struct _dyneither_ptr*)_tmp185->v,_tmp1162))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp1168;struct _tuple9*_tmp1167;struct Cyc_Absyn_CompoundLit_e_struct*_tmp1166;
return Cyc_Absyn_new_exp((void*)((_tmp1166=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1166)),((_tmp1166[0]=((_tmp1168.tag=26,((_tmp1168.f1=((_tmp1167=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1167)),((_tmp1167->f1=vopt1,((
_tmp1167->f2=_tmp186,((_tmp1167->f3=Cyc_Tcutil_copy_type(_tmp187),_tmp1167)))))))),((
_tmp1168.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct _tuple18*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp188),_tmp1168)))))),
_tmp1166)))),e->loc);};}_LL9D: {struct Cyc_Absyn_Array_e_struct*_tmp189=(struct
Cyc_Absyn_Array_e_struct*)_tmp133;if(_tmp189->tag != 27)goto _LL9F;else{_tmp18A=
_tmp189->f1;}}_LL9E: {struct Cyc_Absyn_Array_e_struct _tmp116B;struct Cyc_Absyn_Array_e_struct*
_tmp116A;return Cyc_Absyn_new_exp((void*)((_tmp116A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp116A)),((_tmp116A[0]=((_tmp116B.tag=27,((_tmp116B.f1=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp18A),_tmp116B)))),_tmp116A)))),e->loc);}_LL9F: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp18B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp133;if(_tmp18B->tag != 28)
goto _LLA1;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;_tmp18E=_tmp18B->f3;
_tmp18F=_tmp18B->f4;}}_LLA0: {struct Cyc_Absyn_Comprehension_e_struct _tmp116E;
struct Cyc_Absyn_Comprehension_e_struct*_tmp116D;return Cyc_Absyn_new_exp((void*)((
_tmp116D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp116D)),((_tmp116D[0]=((
_tmp116E.tag=28,((_tmp116E.f1=_tmp18C,((_tmp116E.f2=Cyc_Tcutil_deep_copy_exp(
_tmp18D),((_tmp116E.f3=Cyc_Tcutil_deep_copy_exp(_tmp18E),((_tmp116E.f4=_tmp18F,
_tmp116E)))))))))),_tmp116D)))),e->loc);}_LLA1: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp190=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp133;if(_tmp190->tag != 29)goto
_LLA3;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;_tmp193=_tmp190->f3;_tmp194=
_tmp190->f4;}}_LLA2: {struct Cyc_Absyn_Aggregate_e_struct _tmp1171;struct Cyc_Absyn_Aggregate_e_struct*
_tmp1170;return Cyc_Absyn_new_exp((void*)((_tmp1170=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1170)),((_tmp1170[0]=((_tmp1171.tag=29,((_tmp1171.f1=_tmp191,((
_tmp1171.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp192),((_tmp1171.f3=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp193),((_tmp1171.f4=_tmp194,_tmp1171)))))))))),_tmp1170)))),e->loc);}_LLA3: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp195=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp133;if(_tmp195->tag != 30)goto _LLA5;else{_tmp196=(void*)_tmp195->f1;_tmp197=
_tmp195->f2;}}_LLA4: {struct Cyc_Absyn_AnonStruct_e_struct _tmp1174;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1173;return Cyc_Absyn_new_exp((void*)((_tmp1173=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1173)),((_tmp1173[0]=((_tmp1174.tag=30,((_tmp1174.f1=(void*)Cyc_Tcutil_copy_type(
_tmp196),((_tmp1174.f2=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp197),
_tmp1174)))))),_tmp1173)))),e->loc);}_LLA5: {struct Cyc_Absyn_Datatype_e_struct*
_tmp198=(struct Cyc_Absyn_Datatype_e_struct*)_tmp133;if(_tmp198->tag != 31)goto
_LLA7;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;_tmp19B=_tmp198->f3;}}_LLA6: {
struct Cyc_Absyn_Datatype_e_struct _tmp1177;struct Cyc_Absyn_Datatype_e_struct*
_tmp1176;return Cyc_Absyn_new_exp((void*)((_tmp1176=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1176)),((_tmp1176[0]=((_tmp1177.tag=31,((_tmp1177.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_deep_copy_exp,_tmp199),((_tmp1177.f2=_tmp19A,((_tmp1177.f3=_tmp19B,
_tmp1177)))))))),_tmp1176)))),e->loc);}_LLA7: {struct Cyc_Absyn_Enum_e_struct*
_tmp19C=(struct Cyc_Absyn_Enum_e_struct*)_tmp133;if(_tmp19C->tag != 32)goto _LLA9;
else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;_tmp19F=_tmp19C->f3;}}_LLA8: return e;
_LLA9: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1A0=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp133;if(_tmp1A0->tag != 33)goto _LLAB;else{_tmp1A1=_tmp1A0->f1;_tmp1A2=(void*)
_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;}}_LLAA: {struct Cyc_Absyn_AnonEnum_e_struct
_tmp117A;struct Cyc_Absyn_AnonEnum_e_struct*_tmp1179;return Cyc_Absyn_new_exp((
void*)((_tmp1179=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1179)),((
_tmp1179[0]=((_tmp117A.tag=33,((_tmp117A.f1=_tmp1A1,((_tmp117A.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A2),((_tmp117A.f3=_tmp1A3,_tmp117A)))))))),_tmp1179)))),e->loc);}_LLAB: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A4=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp133;if(_tmp1A4->tag != 34)goto _LLAD;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A5.is_calloc;
_tmp1A7=_tmp1A5.rgn;_tmp1A8=_tmp1A5.elt_type;_tmp1A9=_tmp1A5.num_elts;_tmp1AA=
_tmp1A5.fat_result;}}_LLAC: {struct Cyc_Absyn_Exp*_tmp1E0=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A7;if(_tmp1A7 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A7);{void**t1=_tmp1A8;if(_tmp1A8 != 0){void**_tmp117B;
t1=((_tmp117B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp117B)),((
_tmp117B[0]=Cyc_Tcutil_copy_type(*_tmp1A8),_tmp117B))));}{struct Cyc_Absyn_Malloc_e_struct
_tmp1181;struct Cyc_Absyn_MallocInfo _tmp1180;struct Cyc_Absyn_Malloc_e_struct*
_tmp117F;_tmp1E0->r=(void*)((_tmp117F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp117F)),((_tmp117F[0]=((_tmp1181.tag=34,((_tmp1181.f1=((_tmp1180.is_calloc=
_tmp1A6,((_tmp1180.rgn=r1,((_tmp1180.elt_type=t1,((_tmp1180.num_elts=_tmp1A9,((
_tmp1180.fat_result=_tmp1AA,_tmp1180)))))))))),_tmp1181)))),_tmp117F))));}return
_tmp1E0;};}_LLAD: {struct Cyc_Absyn_Swap_e_struct*_tmp1AB=(struct Cyc_Absyn_Swap_e_struct*)
_tmp133;if(_tmp1AB->tag != 35)goto _LLAF;else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}
_LLAE: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AC),Cyc_Tcutil_deep_copy_exp(
_tmp1AD),e->loc);_LLAF: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AE=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp133;if(_tmp1AE->tag != 36)goto _LLB1;else{
_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;}}_LLB0: {struct Cyc_Core_Opt*nopt1=
_tmp1AF;if(_tmp1AF != 0){struct Cyc_Core_Opt*_tmp1182;nopt1=((_tmp1182=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1182)),((_tmp1182->v=(struct
_tuple2*)_tmp1AF->v,_tmp1182))));}{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp1185;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1184;return Cyc_Absyn_new_exp((
void*)((_tmp1184=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1184)),((
_tmp1184[0]=((_tmp1185.tag=36,((_tmp1185.f1=nopt1,((_tmp1185.f2=((struct Cyc_List_List*(*)(
struct _tuple18*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1B0),_tmp1185)))))),_tmp1184)))),e->loc);};}_LLB1: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1B1=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp133;if(_tmp1B1->tag != 37)goto
_LLB3;}_LLB2: {struct Cyc_Core_Failure_struct _tmp118B;const char*_tmp118A;struct
Cyc_Core_Failure_struct*_tmp1189;(int)_throw((void*)((_tmp1189=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp1189)),((_tmp1189[0]=((_tmp118B.tag=Cyc_Core_Failure,((
_tmp118B.f1=((_tmp118A="deep_copy: statement expressions unsupported",
_tag_dyneither(_tmp118A,sizeof(char),45))),_tmp118B)))),_tmp1189)))));}_LLB3: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp1B2=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp133;if(_tmp1B2->tag != 38)goto _LLB5;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}
_LLB4: {struct Cyc_Absyn_Tagcheck_e_struct _tmp118E;struct Cyc_Absyn_Tagcheck_e_struct*
_tmp118D;return Cyc_Absyn_new_exp((void*)((_tmp118D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp118D)),((_tmp118D[0]=((_tmp118E.tag=38,((_tmp118E.f1=Cyc_Tcutil_deep_copy_exp(
_tmp1B3),((_tmp118E.f2=_tmp1B4,_tmp118E)))))),_tmp118D)))),e->loc);}_LLB5: {
struct Cyc_Absyn_Valueof_e_struct*_tmp1B5=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp133;if(_tmp1B5->tag != 39)goto _LLB7;else{_tmp1B6=(void*)_tmp1B5->f1;}}_LLB6:
return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp1B6),e->loc);_LLB7: {struct
Cyc_Absyn_Asm_e_struct*_tmp1B7=(struct Cyc_Absyn_Asm_e_struct*)_tmp133;if(_tmp1B7->tag
!= 40)goto _LL66;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}_LLB8: return Cyc_Absyn_asm_exp(
_tmp1B8,_tmp1B9,e->loc);_LL66:;}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum 
Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual
f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind _tmp1EE;enum Cyc_Absyn_KindQual _tmp1EF;enum Cyc_Absyn_AliasQual
_tmp1F0;struct Cyc_Absyn_Kind*_tmp1ED=ka1;_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.kind;
_tmp1F0=_tmp1EE.aliasqual;{struct Cyc_Absyn_Kind _tmp1F2;enum Cyc_Absyn_KindQual
_tmp1F3;enum Cyc_Absyn_AliasQual _tmp1F4;struct Cyc_Absyn_Kind*_tmp1F1=ka2;_tmp1F2=*
_tmp1F1;_tmp1F3=_tmp1F2.kind;_tmp1F4=_tmp1F2.aliasqual;if(_tmp1EF != _tmp1F3){
struct _tuple19 _tmp118F;struct _tuple19 _tmp1F6=(_tmp118F.f1=_tmp1EF,((_tmp118F.f2=
_tmp1F3,_tmp118F)));enum Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual
_tmp1F8;enum Cyc_Absyn_KindQual _tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;enum Cyc_Absyn_KindQual
_tmp1FB;enum Cyc_Absyn_KindQual _tmp1FC;_LLBF: _tmp1F7=_tmp1F6.f1;if(_tmp1F7 != Cyc_Absyn_BoxKind)
goto _LLC1;_tmp1F8=_tmp1F6.f2;if(_tmp1F8 != Cyc_Absyn_MemKind)goto _LLC1;_LLC0: goto
_LLC2;_LLC1: _tmp1F9=_tmp1F6.f1;if(_tmp1F9 != Cyc_Absyn_BoxKind)goto _LLC3;_tmp1FA=
_tmp1F6.f2;if(_tmp1FA != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto _LLC4;_LLC3:
_tmp1FB=_tmp1F6.f1;if(_tmp1FB != Cyc_Absyn_MemKind)goto _LLC5;_tmp1FC=_tmp1F6.f2;
if(_tmp1FC != Cyc_Absyn_AnyKind)goto _LLC5;_LLC4: goto _LLBE;_LLC5:;_LLC6: return 0;
_LLBE:;}if(_tmp1F0 != _tmp1F4){struct _tuple20 _tmp1190;struct _tuple20 _tmp1FE=(
_tmp1190.f1=_tmp1F0,((_tmp1190.f2=_tmp1F4,_tmp1190)));enum Cyc_Absyn_AliasQual
_tmp1FF;enum Cyc_Absyn_AliasQual _tmp200;enum Cyc_Absyn_AliasQual _tmp201;enum Cyc_Absyn_AliasQual
_tmp202;_LLC8: _tmp1FF=_tmp1FE.f1;if(_tmp1FF != Cyc_Absyn_Aliasable)goto _LLCA;
_tmp200=_tmp1FE.f2;if(_tmp200 != Cyc_Absyn_Top)goto _LLCA;_LLC9: goto _LLCB;_LLCA:
_tmp201=_tmp1FE.f1;if(_tmp201 != Cyc_Absyn_Unique)goto _LLCC;_tmp202=_tmp1FE.f2;
if(_tmp202 != Cyc_Absyn_Top)goto _LLCC;_LLCB: return 1;_LLCC:;_LLCD: return 0;_LLC7:;}
return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,
struct Cyc_Absyn_Kind*def){void*_tmp203=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*
_tmp205;struct Cyc_Absyn_Kind*_tmp207;_LLCF: {struct Cyc_Absyn_Eq_kb_struct*
_tmp204=(struct Cyc_Absyn_Eq_kb_struct*)_tmp203;if(_tmp204->tag != 0)goto _LLD1;
else{_tmp205=_tmp204->f1;}}_LLD0: return _tmp205;_LLD1: {struct Cyc_Absyn_Less_kb_struct*
_tmp206=(struct Cyc_Absyn_Less_kb_struct*)_tmp203;if(_tmp206->tag != 2)goto _LLD3;
else{_tmp207=_tmp206->f2;}}_LLD2: return _tmp207;_LLD3:;_LLD4:{struct Cyc_Absyn_Less_kb_struct
_tmp1193;struct Cyc_Absyn_Less_kb_struct*_tmp1192;tv->kind=(void*)((_tmp1192=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1192)),((_tmp1192[0]=((
_tmp1193.tag=2,((_tmp1193.f1=0,((_tmp1193.f2=def,_tmp1193)))))),_tmp1192))));}
return def;_LLCE:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp1194;struct _tuple0 _tmp20B=(_tmp1194.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1194.f2=
Cyc_Absyn_compress_kb(kb2),_tmp1194)));void*_tmp20C;struct Cyc_Absyn_Kind*_tmp20E;
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
struct Cyc_Core_Opt*_tmp1195;*_tmp215=((_tmp1195=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1195)),((_tmp1195->v=kb2,_tmp1195))));}return 1;}else{return 0;}_LLDA:
_tmp21A=_tmp20B.f1;{struct Cyc_Absyn_Eq_kb_struct*_tmp21B=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp21A;if(_tmp21B->tag != 0)goto _LLDC;else{_tmp21C=_tmp21B->f1;}};_tmp21D=
_tmp20B.f2;{struct Cyc_Absyn_Less_kb_struct*_tmp21E=(struct Cyc_Absyn_Less_kb_struct*)
_tmp21D;if(_tmp21E->tag != 2)goto _LLDC;else{_tmp21F=_tmp21E->f1;_tmp220=(struct
Cyc_Core_Opt**)& _tmp21E->f1;_tmp221=_tmp21E->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(
_tmp21C,_tmp221)){{struct Cyc_Core_Opt*_tmp1196;*_tmp220=((_tmp1196=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1196)),((_tmp1196->v=kb1,
_tmp1196))));}return 1;}else{return 0;}_LLDC: _tmp222=_tmp20B.f1;{struct Cyc_Absyn_Less_kb_struct*
_tmp223=(struct Cyc_Absyn_Less_kb_struct*)_tmp222;if(_tmp223->tag != 2)goto _LLDE;
else{_tmp224=_tmp223->f1;_tmp225=(struct Cyc_Core_Opt**)& _tmp223->f1;_tmp226=
_tmp223->f2;}};_tmp227=_tmp20B.f2;{struct Cyc_Absyn_Less_kb_struct*_tmp228=(
struct Cyc_Absyn_Less_kb_struct*)_tmp227;if(_tmp228->tag != 2)goto _LLDE;else{
_tmp229=_tmp228->f1;_tmp22A=(struct Cyc_Core_Opt**)& _tmp228->f1;_tmp22B=_tmp228->f2;}};
_LLDD: if(Cyc_Tcutil_kind_leq(_tmp226,_tmp22B)){{struct Cyc_Core_Opt*_tmp1197;*
_tmp22A=((_tmp1197=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1197)),((
_tmp1197->v=kb1,_tmp1197))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp22B,
_tmp226)){{struct Cyc_Core_Opt*_tmp1198;*_tmp225=((_tmp1198=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1198)),((_tmp1198->v=kb2,_tmp1198))));}return 1;}else{return 0;}}_LLDE:
_tmp22C=_tmp20B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp22D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp22C;if(_tmp22D->tag != 1)goto _LLE0;else{_tmp22E=_tmp22D->f1;_tmp22F=(struct
Cyc_Core_Opt**)& _tmp22D->f1;}};_tmp230=_tmp20B.f2;_LLDF: _tmp231=_tmp230;_tmp235=
_tmp22F;goto _LLE1;_LLE0: _tmp231=_tmp20B.f1;_tmp232=_tmp20B.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp233=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp232;if(_tmp233->tag != 1)goto
_LLD5;else{_tmp234=_tmp233->f1;_tmp235=(struct Cyc_Core_Opt**)& _tmp233->f1;}};
_LLE1:{struct Cyc_Core_Opt*_tmp1199;*_tmp235=((_tmp1199=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1199)),((_tmp1199->v=_tmp231,_tmp1199))));}return 1;_LLD5:;}struct
_tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp23B=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp23D;_LLE3: {struct Cyc_Absyn_VarType_struct*_tmp23C=(
struct Cyc_Absyn_VarType_struct*)_tmp23B;if(_tmp23C->tag != 2)goto _LLE5;else{
_tmp23D=_tmp23C->f1;}}_LLE4: {void*_tmp23E=_tmp23D->kind;_tmp23D->kind=kb;{
struct _tuple15 _tmp119A;return(_tmp119A.f1=_tmp23D,((_tmp119A.f2=_tmp23E,_tmp119A)));};}
_LLE5:;_LLE6: {const char*_tmp119E;void*_tmp119D[1];struct Cyc_String_pa_struct
_tmp119C;(_tmp119C.tag=0,((_tmp119C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp119D[0]=& _tmp119C,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119E="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp119E,sizeof(char),40))),_tag_dyneither(_tmp119D,sizeof(void*),
1)))))));}_LLE2:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp243=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp245;struct Cyc_Absyn_Tvar*_tmp247;
enum Cyc_Absyn_Size_of _tmp24A;struct Cyc_Absyn_DatatypeFieldInfo _tmp257;union Cyc_Absyn_DatatypeFieldInfoU
_tmp258;struct _tuple3 _tmp259;struct Cyc_Absyn_Datatypedecl*_tmp25A;struct Cyc_Absyn_Datatypefield*
_tmp25B;struct Cyc_Absyn_DatatypeFieldInfo _tmp25D;union Cyc_Absyn_DatatypeFieldInfoU
_tmp25E;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp25F;struct Cyc_Absyn_AggrInfo
_tmp261;union Cyc_Absyn_AggrInfoU _tmp262;struct _tuple4 _tmp263;struct Cyc_Absyn_AggrInfo
_tmp265;union Cyc_Absyn_AggrInfoU _tmp266;struct Cyc_Absyn_Aggrdecl**_tmp267;struct
Cyc_Absyn_Aggrdecl*_tmp268;struct Cyc_Absyn_Aggrdecl _tmp269;struct Cyc_Absyn_AggrdeclImpl*
_tmp26A;struct Cyc_Absyn_PtrInfo _tmp26D;struct Cyc_Absyn_Typedefdecl*_tmp273;
struct Cyc_Absyn_TypeDecl*_tmp275;struct Cyc_Absyn_TypeDecl _tmp276;void*_tmp277;
struct Cyc_Absyn_TypeDecl*_tmp27A;struct Cyc_Absyn_TypeDecl _tmp27B;void*_tmp27C;
struct Cyc_Absyn_TypeDecl*_tmp27F;struct Cyc_Absyn_TypeDecl _tmp280;void*_tmp281;
_LLE8: {struct Cyc_Absyn_Evar_struct*_tmp244=(struct Cyc_Absyn_Evar_struct*)
_tmp243;if(_tmp244->tag != 1)goto _LLEA;else{_tmp245=_tmp244->f1;}}_LLE9: return(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp245))->v;_LLEA: {
struct Cyc_Absyn_VarType_struct*_tmp246=(struct Cyc_Absyn_VarType_struct*)_tmp243;
if(_tmp246->tag != 2)goto _LLEC;else{_tmp247=_tmp246->f1;}}_LLEB: return Cyc_Tcutil_tvar_kind(
_tmp247,& Cyc_Tcutil_bk);_LLEC: {struct Cyc_Absyn_VoidType_struct*_tmp248=(struct
Cyc_Absyn_VoidType_struct*)_tmp243;if(_tmp248->tag != 0)goto _LLEE;}_LLED: return&
Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_IntType_struct*_tmp249=(struct Cyc_Absyn_IntType_struct*)
_tmp243;if(_tmp249->tag != 6)goto _LLF0;else{_tmp24A=_tmp249->f2;}}_LLEF: return(
_tmp24A == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp24A == (enum Cyc_Absyn_Size_of)
Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF0: {struct Cyc_Absyn_FloatType_struct*
_tmp24B=(struct Cyc_Absyn_FloatType_struct*)_tmp243;if(_tmp24B->tag != 7)goto _LLF2;}
_LLF1: goto _LLF3;_LLF2: {struct Cyc_Absyn_DoubleType_struct*_tmp24C=(struct Cyc_Absyn_DoubleType_struct*)
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
_LL109: {const char*_tmp11A1;void*_tmp11A0;(_tmp11A0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A1="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp11A1,sizeof(char),39))),_tag_dyneither(_tmp11A0,sizeof(void*),
0)));}_LL10A: {struct Cyc_Absyn_AggrType_struct*_tmp260=(struct Cyc_Absyn_AggrType_struct*)
_tmp243;if(_tmp260->tag != 12)goto _LL10C;else{_tmp261=_tmp260->f1;_tmp262=_tmp261.aggr_info;
if((_tmp262.UnknownAggr).tag != 1)goto _LL10C;_tmp263=(struct _tuple4)(_tmp262.UnknownAggr).val;}}
_LL10B: return& Cyc_Tcutil_ak;_LL10C: {struct Cyc_Absyn_AggrType_struct*_tmp264=(
struct Cyc_Absyn_AggrType_struct*)_tmp243;if(_tmp264->tag != 12)goto _LL10E;else{
_tmp265=_tmp264->f1;_tmp266=_tmp265.aggr_info;if((_tmp266.KnownAggr).tag != 2)
goto _LL10E;_tmp267=(struct Cyc_Absyn_Aggrdecl**)(_tmp266.KnownAggr).val;_tmp268=*
_tmp267;_tmp269=*_tmp268;_tmp26A=_tmp269.impl;}}_LL10D: return _tmp26A == 0?& Cyc_Tcutil_ak:&
Cyc_Tcutil_mk;_LL10E: {struct Cyc_Absyn_AnonAggrType_struct*_tmp26B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp243;if(_tmp26B->tag != 13)goto _LL110;}_LL10F: return& Cyc_Tcutil_mk;_LL110: {
struct Cyc_Absyn_PointerType_struct*_tmp26C=(struct Cyc_Absyn_PointerType_struct*)
_tmp243;if(_tmp26C->tag != 5)goto _LL112;else{_tmp26D=_tmp26C->f1;}}_LL111: {void*
_tmp288=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp26D.ptr_atts).bounds);_LL129: {struct Cyc_Absyn_DynEither_b_struct*
_tmp289=(struct Cyc_Absyn_DynEither_b_struct*)_tmp288;if(_tmp289->tag != 0)goto
_LL12B;}_LL12A: {struct Cyc_Absyn_Kind*_tmp11A2;return(_tmp11A2=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp11A2)),((_tmp11A2->kind=Cyc_Absyn_MemKind,((
_tmp11A2->aliasqual=(Cyc_Tcutil_typ_kind((_tmp26D.ptr_atts).rgn))->aliasqual,
_tmp11A2)))));}_LL12B: {struct Cyc_Absyn_Upper_b_struct*_tmp28A=(struct Cyc_Absyn_Upper_b_struct*)
_tmp288;if(_tmp28A->tag != 1)goto _LL128;}_LL12C: {struct Cyc_Absyn_Kind*_tmp11A3;
return(_tmp11A3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11A3)),((
_tmp11A3->kind=Cyc_Absyn_BoxKind,((_tmp11A3->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp11A3)))));}_LL128:;}_LL112: {struct Cyc_Absyn_ValueofType_struct*
_tmp26E=(struct Cyc_Absyn_ValueofType_struct*)_tmp243;if(_tmp26E->tag != 19)goto
_LL114;}_LL113: return& Cyc_Tcutil_ik;_LL114: {struct Cyc_Absyn_TagType_struct*
_tmp26F=(struct Cyc_Absyn_TagType_struct*)_tmp243;if(_tmp26F->tag != 20)goto _LL116;}
_LL115: return& Cyc_Tcutil_bk;_LL116: {struct Cyc_Absyn_ArrayType_struct*_tmp270=(
struct Cyc_Absyn_ArrayType_struct*)_tmp243;if(_tmp270->tag != 9)goto _LL118;}_LL117:
goto _LL119;_LL118: {struct Cyc_Absyn_TupleType_struct*_tmp271=(struct Cyc_Absyn_TupleType_struct*)
_tmp243;if(_tmp271->tag != 11)goto _LL11A;}_LL119: return& Cyc_Tcutil_mk;_LL11A: {
struct Cyc_Absyn_TypedefType_struct*_tmp272=(struct Cyc_Absyn_TypedefType_struct*)
_tmp243;if(_tmp272->tag != 18)goto _LL11C;else{_tmp273=_tmp272->f3;}}_LL11B: if(
_tmp273 == 0  || _tmp273->kind == 0){const char*_tmp11A7;void*_tmp11A6[1];struct Cyc_String_pa_struct
_tmp11A5;(_tmp11A5.tag=0,((_tmp11A5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp11A6[0]=& _tmp11A5,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A7="typ_kind: typedef found: %s",
_tag_dyneither(_tmp11A7,sizeof(char),28))),_tag_dyneither(_tmp11A6,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp273->kind))->v;
_LL11C: {struct Cyc_Absyn_TypeDeclType_struct*_tmp274=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp243;if(_tmp274->tag != 27)goto _LL11E;else{_tmp275=_tmp274->f1;_tmp276=*
_tmp275;_tmp277=_tmp276.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp278=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp277;if(_tmp278->tag != 0)goto _LL11E;};}}_LL11D: return& Cyc_Tcutil_ak;_LL11E: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp279=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp243;if(_tmp279->tag != 27)goto _LL120;else{_tmp27A=_tmp279->f1;_tmp27B=*
_tmp27A;_tmp27C=_tmp27B.r;{struct Cyc_Absyn_Enum_td_struct*_tmp27D=(struct Cyc_Absyn_Enum_td_struct*)
_tmp27C;if(_tmp27D->tag != 1)goto _LL120;};}}_LL11F: return& Cyc_Tcutil_bk;_LL120: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp27E=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp243;if(_tmp27E->tag != 27)goto _LL122;else{_tmp27F=_tmp27E->f1;_tmp280=*
_tmp27F;_tmp281=_tmp280.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp282=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp281;if(_tmp282->tag != 2)goto _LL122;};}}_LL121: return& Cyc_Tcutil_ak;_LL122: {
struct Cyc_Absyn_AccessEff_struct*_tmp283=(struct Cyc_Absyn_AccessEff_struct*)
_tmp243;if(_tmp283->tag != 24)goto _LL124;}_LL123: goto _LL125;_LL124: {struct Cyc_Absyn_JoinEff_struct*
_tmp284=(struct Cyc_Absyn_JoinEff_struct*)_tmp243;if(_tmp284->tag != 25)goto _LL126;}
_LL125: goto _LL127;_LL126: {struct Cyc_Absyn_RgnsEff_struct*_tmp285=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp243;if(_tmp285->tag != 26)goto _LLE7;}_LL127: return& Cyc_Tcutil_ek;_LLE7:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == 
k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp290;_push_handler(& _tmp290);{int _tmp292=
0;if(setjmp(_tmp290.handler))_tmp292=1;if(!_tmp292){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp293=1;_npop_handler(0);return _tmp293;};;_pop_handler();}else{void*_tmp291=(
void*)_exn_thrown;void*_tmp295=_tmp291;_LL12E: {struct Cyc_Tcutil_Unify_struct*
_tmp296=(struct Cyc_Tcutil_Unify_struct*)_tmp295;if(_tmp296->tag != Cyc_Tcutil_Unify)
goto _LL130;}_LL12F: return 0;_LL130:;_LL131:(void)_throw(_tmp295);_LL12D:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*
_tmp297=t;struct Cyc_Absyn_Tvar*_tmp299;struct Cyc_Core_Opt*_tmp29B;struct Cyc_Core_Opt*
_tmp29C;struct Cyc_Core_Opt**_tmp29D;struct Cyc_Absyn_PtrInfo _tmp29F;struct Cyc_Absyn_ArrayInfo
_tmp2A1;void*_tmp2A2;struct Cyc_Absyn_FnInfo _tmp2A4;struct Cyc_List_List*_tmp2A5;
struct Cyc_Core_Opt*_tmp2A6;struct Cyc_Absyn_Tqual _tmp2A7;void*_tmp2A8;struct Cyc_List_List*
_tmp2A9;int _tmp2AA;struct Cyc_Absyn_VarargInfo*_tmp2AB;struct Cyc_List_List*
_tmp2AC;struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AF;struct Cyc_Absyn_DatatypeInfo
_tmp2B1;struct Cyc_List_List*_tmp2B2;struct Cyc_List_List*_tmp2B4;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_Absyn_AggrInfo _tmp2B9;struct Cyc_List_List*
_tmp2BA;struct Cyc_List_List*_tmp2BC;void*_tmp2BE;void*_tmp2C0;void*_tmp2C2;void*
_tmp2C4;struct Cyc_List_List*_tmp2C6;_LL133: {struct Cyc_Absyn_VarType_struct*
_tmp298=(struct Cyc_Absyn_VarType_struct*)_tmp297;if(_tmp298->tag != 2)goto _LL135;
else{_tmp299=_tmp298->f1;}}_LL134: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmp299)){{const char*_tmp11A8;Cyc_Tcutil_failure_reason=((
_tmp11A8="(type variable would escape scope)",_tag_dyneither(_tmp11A8,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL132;_LL135: {
struct Cyc_Absyn_Evar_struct*_tmp29A=(struct Cyc_Absyn_Evar_struct*)_tmp297;if(
_tmp29A->tag != 1)goto _LL137;else{_tmp29B=_tmp29A->f2;_tmp29C=_tmp29A->f4;_tmp29D=(
struct Cyc_Core_Opt**)& _tmp29A->f4;}}_LL136: if(t == evar){{const char*_tmp11A9;Cyc_Tcutil_failure_reason=((
_tmp11A9="(occurs check)",_tag_dyneither(_tmp11A9,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp29B != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp29B->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp29D))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2C9=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp29D))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp11AA;_tmp2C9=((
_tmp11AA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11AA)),((_tmp11AA->hd=(
struct Cyc_Absyn_Tvar*)s->hd,((_tmp11AA->tl=_tmp2C9,_tmp11AA))))));}}}{struct Cyc_Core_Opt*
_tmp11AB;*_tmp29D=((_tmp11AB=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp11AB)),((_tmp11AB->v=_tmp2C9,_tmp11AB))));};}}}goto _LL132;_LL137: {struct Cyc_Absyn_PointerType_struct*
_tmp29E=(struct Cyc_Absyn_PointerType_struct*)_tmp297;if(_tmp29E->tag != 5)goto
_LL139;else{_tmp29F=_tmp29E->f1;}}_LL138: Cyc_Tcutil_occurs(evar,r,env,_tmp29F.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp29F.ptr_atts).rgn);goto _LL132;_LL139: {struct
Cyc_Absyn_ArrayType_struct*_tmp2A0=(struct Cyc_Absyn_ArrayType_struct*)_tmp297;
if(_tmp2A0->tag != 9)goto _LL13B;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A1.elt_type;}}
_LL13A: Cyc_Tcutil_occurs(evar,r,env,_tmp2A2);goto _LL132;_LL13B: {struct Cyc_Absyn_FnType_struct*
_tmp2A3=(struct Cyc_Absyn_FnType_struct*)_tmp297;if(_tmp2A3->tag != 10)goto _LL13D;
else{_tmp2A4=_tmp2A3->f1;_tmp2A5=_tmp2A4.tvars;_tmp2A6=_tmp2A4.effect;_tmp2A7=
_tmp2A4.ret_tqual;_tmp2A8=_tmp2A4.ret_typ;_tmp2A9=_tmp2A4.args;_tmp2AA=_tmp2A4.c_varargs;
_tmp2AB=_tmp2A4.cyc_varargs;_tmp2AC=_tmp2A4.rgn_po;_tmp2AD=_tmp2A4.attributes;}}
_LL13C: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2A5,env);if(_tmp2A6 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp2A6->v);Cyc_Tcutil_occurs(evar,r,env,_tmp2A8);for(0;
_tmp2A9 != 0;_tmp2A9=_tmp2A9->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple9*)
_tmp2A9->hd)).f3);}if(_tmp2AB != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp2AB->type);
for(0;_tmp2AC != 0;_tmp2AC=_tmp2AC->tl){struct _tuple0 _tmp2CD;void*_tmp2CE;void*
_tmp2CF;struct _tuple0*_tmp2CC=(struct _tuple0*)_tmp2AC->hd;_tmp2CD=*_tmp2CC;
_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;Cyc_Tcutil_occurs(evar,r,env,_tmp2CE);Cyc_Tcutil_occurs(
evar,r,env,_tmp2CF);}goto _LL132;_LL13D: {struct Cyc_Absyn_TupleType_struct*
_tmp2AE=(struct Cyc_Absyn_TupleType_struct*)_tmp297;if(_tmp2AE->tag != 11)goto
_LL13F;else{_tmp2AF=_tmp2AE->f1;}}_LL13E: for(0;_tmp2AF != 0;_tmp2AF=_tmp2AF->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp2AF->hd)).f2);}goto _LL132;
_LL13F: {struct Cyc_Absyn_DatatypeType_struct*_tmp2B0=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp297;if(_tmp2B0->tag != 3)goto _LL141;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B1.targs;}}
_LL140: Cyc_Tcutil_occurslist(evar,r,env,_tmp2B2);goto _LL132;_LL141: {struct Cyc_Absyn_TypedefType_struct*
_tmp2B3=(struct Cyc_Absyn_TypedefType_struct*)_tmp297;if(_tmp2B3->tag != 18)goto
_LL143;else{_tmp2B4=_tmp2B3->f2;}}_LL142: _tmp2B7=_tmp2B4;goto _LL144;_LL143: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2B5=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp297;if(_tmp2B5->tag != 4)goto _LL145;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B6.targs;}}
_LL144: _tmp2BA=_tmp2B7;goto _LL146;_LL145: {struct Cyc_Absyn_AggrType_struct*
_tmp2B8=(struct Cyc_Absyn_AggrType_struct*)_tmp297;if(_tmp2B8->tag != 12)goto
_LL147;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.targs;}}_LL146: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2BA);goto _LL132;_LL147: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2BB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp297;if(_tmp2BB->tag != 13)goto
_LL149;else{_tmp2BC=_tmp2BB->f2;}}_LL148: for(0;_tmp2BC != 0;_tmp2BC=_tmp2BC->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2BC->hd)->type);}
goto _LL132;_LL149: {struct Cyc_Absyn_TagType_struct*_tmp2BD=(struct Cyc_Absyn_TagType_struct*)
_tmp297;if(_tmp2BD->tag != 20)goto _LL14B;else{_tmp2BE=(void*)_tmp2BD->f1;}}_LL14A:
_tmp2C0=_tmp2BE;goto _LL14C;_LL14B: {struct Cyc_Absyn_AccessEff_struct*_tmp2BF=(
struct Cyc_Absyn_AccessEff_struct*)_tmp297;if(_tmp2BF->tag != 24)goto _LL14D;else{
_tmp2C0=(void*)_tmp2BF->f1;}}_LL14C: _tmp2C2=_tmp2C0;goto _LL14E;_LL14D: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp2C1=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp297;if(_tmp2C1->tag != 16)goto _LL14F;else{_tmp2C2=(void*)_tmp2C1->f1;}}_LL14E:
_tmp2C4=_tmp2C2;goto _LL150;_LL14F: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C3=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp297;if(_tmp2C3->tag != 26)goto _LL151;else{
_tmp2C4=(void*)_tmp2C3->f1;}}_LL150: Cyc_Tcutil_occurs(evar,r,env,_tmp2C4);goto
_LL132;_LL151: {struct Cyc_Absyn_JoinEff_struct*_tmp2C5=(struct Cyc_Absyn_JoinEff_struct*)
_tmp297;if(_tmp2C5->tag != 25)goto _LL153;else{_tmp2C6=_tmp2C5->f1;}}_LL152: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2C6);goto _LL132;_LL153:;_LL154: goto _LL132;_LL132:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp11AE;void*
_tmp11AD;(_tmp11AD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11AE="tq1 real_const not set.",_tag_dyneither(_tmp11AE,
sizeof(char),24))),_tag_dyneither(_tmp11AD,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp11B1;void*_tmp11B0;(_tmp11B0=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11B1="tq2 real_const not set.",
_tag_dyneither(_tmp11B1,sizeof(char),24))),_tag_dyneither(_tmp11B0,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp11B2;Cyc_Tcutil_failure_reason=((_tmp11B2="(qualifiers don't match)",
_tag_dyneither(_tmp11B2,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2D5=x;union Cyc_Absyn_Constraint _tmp2D6;int _tmp2D7;union Cyc_Absyn_Constraint
_tmp2D8;void*_tmp2D9;union Cyc_Absyn_Constraint _tmp2DA;union Cyc_Absyn_Constraint*
_tmp2DB;_LL156: _tmp2D6=*_tmp2D5;if((_tmp2D6.No_constr).tag != 3)goto _LL158;
_tmp2D7=(int)(_tmp2D6.No_constr).val;_LL157:{union Cyc_Absyn_Constraint _tmp11B3;*
x=(((_tmp11B3.Forward_constr).val=y,(((_tmp11B3.Forward_constr).tag=2,_tmp11B3))));}
return;_LL158: _tmp2D8=*_tmp2D5;if((_tmp2D8.Eq_constr).tag != 1)goto _LL15A;_tmp2D9=(
void*)(_tmp2D8.Eq_constr).val;_LL159: {union Cyc_Absyn_Constraint*_tmp2DD=y;union
Cyc_Absyn_Constraint _tmp2DE;int _tmp2DF;union Cyc_Absyn_Constraint _tmp2E0;void*
_tmp2E1;union Cyc_Absyn_Constraint _tmp2E2;union Cyc_Absyn_Constraint*_tmp2E3;
_LL15D: _tmp2DE=*_tmp2DD;if((_tmp2DE.No_constr).tag != 3)goto _LL15F;_tmp2DF=(int)(
_tmp2DE.No_constr).val;_LL15E:*y=*x;return;_LL15F: _tmp2E0=*_tmp2DD;if((_tmp2E0.Eq_constr).tag
!= 1)goto _LL161;_tmp2E1=(void*)(_tmp2E0.Eq_constr).val;_LL160: if(cmp(_tmp2D9,
_tmp2E1)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL161: _tmp2E2=*_tmp2DD;if((_tmp2E2.Forward_constr).tag != 2)goto _LL15C;
_tmp2E3=(union Cyc_Absyn_Constraint*)(_tmp2E2.Forward_constr).val;_LL162: {const
char*_tmp11B6;void*_tmp11B5;(_tmp11B5=0,Cyc_Tcutil_impos(((_tmp11B6="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11B6,sizeof(char),40))),_tag_dyneither(_tmp11B5,sizeof(void*),
0)));}_LL15C:;}_LL15A: _tmp2DA=*_tmp2D5;if((_tmp2DA.Forward_constr).tag != 2)goto
_LL155;_tmp2DB=(union Cyc_Absyn_Constraint*)(_tmp2DA.Forward_constr).val;_LL15B: {
const char*_tmp11B9;void*_tmp11B8;(_tmp11B8=0,Cyc_Tcutil_impos(((_tmp11B9="unify_conref: forward after compress",
_tag_dyneither(_tmp11B9,sizeof(char),37))),_tag_dyneither(_tmp11B8,sizeof(void*),
0)));}_LL155:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2E8;
_push_handler(& _tmp2E8);{int _tmp2EA=0;if(setjmp(_tmp2E8.handler))_tmp2EA=1;if(!
_tmp2EA){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2EB=1;_npop_handler(0);return _tmp2EB;};;
_pop_handler();}else{void*_tmp2E9=(void*)_exn_thrown;void*_tmp2ED=_tmp2E9;_LL164: {
struct Cyc_Tcutil_Unify_struct*_tmp2EE=(struct Cyc_Tcutil_Unify_struct*)_tmp2ED;
if(_tmp2EE->tag != Cyc_Tcutil_Unify)goto _LL166;}_LL165: return 0;_LL166:;_LL167:(
void)_throw(_tmp2ED);_LL163:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11BA;struct _tuple0 _tmp2F0=(_tmp11BA.f1=b1,((_tmp11BA.f2=b2,
_tmp11BA)));void*_tmp2F1;void*_tmp2F3;void*_tmp2F5;void*_tmp2F7;void*_tmp2F9;
struct Cyc_Absyn_Exp*_tmp2FB;void*_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FE;_LL169:
_tmp2F1=_tmp2F0.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2F2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2F1;if(_tmp2F2->tag != 0)goto _LL16B;};_tmp2F3=_tmp2F0.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F4=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F3;if(_tmp2F4->tag != 0)goto
_LL16B;};_LL16A: return 0;_LL16B: _tmp2F5=_tmp2F0.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F6=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto
_LL16D;};_LL16C: return - 1;_LL16D: _tmp2F7=_tmp2F0.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F7;if(_tmp2F8->tag != 0)goto
_LL16F;};_LL16E: return 1;_LL16F: _tmp2F9=_tmp2F0.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2FA=(struct Cyc_Absyn_Upper_b_struct*)_tmp2F9;if(_tmp2FA->tag != 1)goto _LL168;
else{_tmp2FB=_tmp2FA->f1;}};_tmp2FC=_tmp2F0.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp2FD=(struct Cyc_Absyn_Upper_b_struct*)_tmp2FC;if(_tmp2FD->tag != 1)goto _LL168;
else{_tmp2FE=_tmp2FD->f1;}};_LL170: return Cyc_Evexp_const_exp_cmp(_tmp2FB,_tmp2FE);
_LL168:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0
_tmp11BB;struct _tuple0 _tmp300=(_tmp11BB.f1=b1,((_tmp11BB.f2=b2,_tmp11BB)));void*
_tmp301;void*_tmp303;void*_tmp305;void*_tmp307;void*_tmp309;struct Cyc_Absyn_Exp*
_tmp30B;void*_tmp30C;struct Cyc_Absyn_Exp*_tmp30E;_LL172: _tmp301=_tmp300.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp302=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp301;if(_tmp302->tag != 0)goto _LL174;};_tmp303=_tmp300.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp304=(struct Cyc_Absyn_DynEither_b_struct*)_tmp303;if(_tmp304->tag != 0)goto
_LL174;};_LL173: return 0;_LL174: _tmp305=_tmp300.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp306=(struct Cyc_Absyn_DynEither_b_struct*)_tmp305;if(_tmp306->tag != 0)goto
_LL176;};_LL175: return - 1;_LL176: _tmp307=_tmp300.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp308=(struct Cyc_Absyn_DynEither_b_struct*)_tmp307;if(_tmp308->tag != 0)goto
_LL178;};_LL177: return 1;_LL178: _tmp309=_tmp300.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp30A=(struct Cyc_Absyn_Upper_b_struct*)_tmp309;if(_tmp30A->tag != 1)goto _LL171;
else{_tmp30B=_tmp30A->f1;}};_tmp30C=_tmp300.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp30D=(struct Cyc_Absyn_Upper_b_struct*)_tmp30C;if(_tmp30D->tag != 1)goto _LL171;
else{_tmp30E=_tmp30D->f1;}};_LL179: return Cyc_Evexp_const_exp_cmp(_tmp30B,_tmp30E);
_LL171:;}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp30F=att;
_LL17B: {struct Cyc_Absyn_Regparm_att_struct*_tmp310=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp30F;if(_tmp310->tag != 0)goto _LL17D;}_LL17C: return 0;_LL17D: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp311=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp30F;if(_tmp311->tag != 1)goto
_LL17F;}_LL17E: return 1;_LL17F: {struct Cyc_Absyn_Cdecl_att_struct*_tmp312=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp30F;if(_tmp312->tag != 2)goto _LL181;}_LL180:
return 2;_LL181: {struct Cyc_Absyn_Fastcall_att_struct*_tmp313=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp30F;if(_tmp313->tag != 3)goto _LL183;}_LL182: return 3;_LL183: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp314=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp30F;if(_tmp314->tag != 4)goto
_LL185;}_LL184: return 4;_LL185: {struct Cyc_Absyn_Const_att_struct*_tmp315=(struct
Cyc_Absyn_Const_att_struct*)_tmp30F;if(_tmp315->tag != 5)goto _LL187;}_LL186:
return 5;_LL187: {struct Cyc_Absyn_Aligned_att_struct*_tmp316=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp30F;if(_tmp316->tag != 6)goto _LL189;}_LL188: return 6;_LL189: {struct Cyc_Absyn_Packed_att_struct*
_tmp317=(struct Cyc_Absyn_Packed_att_struct*)_tmp30F;if(_tmp317->tag != 7)goto
_LL18B;}_LL18A: return 7;_LL18B: {struct Cyc_Absyn_Section_att_struct*_tmp318=(
struct Cyc_Absyn_Section_att_struct*)_tmp30F;if(_tmp318->tag != 8)goto _LL18D;}
_LL18C: return 8;_LL18D: {struct Cyc_Absyn_Nocommon_att_struct*_tmp319=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp30F;if(_tmp319->tag != 9)goto _LL18F;}_LL18E: return 9;_LL18F: {struct Cyc_Absyn_Shared_att_struct*
_tmp31A=(struct Cyc_Absyn_Shared_att_struct*)_tmp30F;if(_tmp31A->tag != 10)goto
_LL191;}_LL190: return 10;_LL191: {struct Cyc_Absyn_Unused_att_struct*_tmp31B=(
struct Cyc_Absyn_Unused_att_struct*)_tmp30F;if(_tmp31B->tag != 11)goto _LL193;}
_LL192: return 11;_LL193: {struct Cyc_Absyn_Weak_att_struct*_tmp31C=(struct Cyc_Absyn_Weak_att_struct*)
_tmp30F;if(_tmp31C->tag != 12)goto _LL195;}_LL194: return 12;_LL195: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp31D=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp30F;if(_tmp31D->tag != 13)goto
_LL197;}_LL196: return 13;_LL197: {struct Cyc_Absyn_Dllexport_att_struct*_tmp31E=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp30F;if(_tmp31E->tag != 14)goto _LL199;}
_LL198: return 14;_LL199: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp31F=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp30F;if(_tmp31F->tag
!= 15)goto _LL19B;}_LL19A: return 15;_LL19B: {struct Cyc_Absyn_Constructor_att_struct*
_tmp320=(struct Cyc_Absyn_Constructor_att_struct*)_tmp30F;if(_tmp320->tag != 16)
goto _LL19D;}_LL19C: return 16;_LL19D: {struct Cyc_Absyn_Destructor_att_struct*
_tmp321=(struct Cyc_Absyn_Destructor_att_struct*)_tmp30F;if(_tmp321->tag != 17)
goto _LL19F;}_LL19E: return 17;_LL19F: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp322=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp30F;if(_tmp322->tag
!= 18)goto _LL1A1;}_LL1A0: return 18;_LL1A1: {struct Cyc_Absyn_Format_att_struct*
_tmp323=(struct Cyc_Absyn_Format_att_struct*)_tmp30F;if(_tmp323->tag != 19)goto
_LL1A3;}_LL1A2: return 19;_LL1A3: {struct Cyc_Absyn_Initializes_att_struct*_tmp324=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp30F;if(_tmp324->tag != 20)goto _LL1A5;}
_LL1A4: return 20;_LL1A5:;_LL1A6: return 21;_LL17A:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp11BC;struct _tuple0 _tmp326=(_tmp11BC.f1=
att1,((_tmp11BC.f2=att2,_tmp11BC)));void*_tmp327;int _tmp329;void*_tmp32A;int
_tmp32C;void*_tmp32D;int _tmp32F;void*_tmp330;int _tmp332;void*_tmp333;int _tmp335;
void*_tmp336;int _tmp338;void*_tmp339;struct _dyneither_ptr _tmp33B;void*_tmp33C;
struct _dyneither_ptr _tmp33E;void*_tmp33F;enum Cyc_Absyn_Format_Type _tmp341;int
_tmp342;int _tmp343;void*_tmp344;enum Cyc_Absyn_Format_Type _tmp346;int _tmp347;int
_tmp348;_LL1A8: _tmp327=_tmp326.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp328=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp327;if(_tmp328->tag != 0)goto _LL1AA;else{
_tmp329=_tmp328->f1;}};_tmp32A=_tmp326.f2;{struct Cyc_Absyn_Regparm_att_struct*
_tmp32B=(struct Cyc_Absyn_Regparm_att_struct*)_tmp32A;if(_tmp32B->tag != 0)goto
_LL1AA;else{_tmp32C=_tmp32B->f1;}};_LL1A9: _tmp32F=_tmp329;_tmp332=_tmp32C;goto
_LL1AB;_LL1AA: _tmp32D=_tmp326.f1;{struct Cyc_Absyn_Initializes_att_struct*_tmp32E=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp32D;if(_tmp32E->tag != 20)goto _LL1AC;
else{_tmp32F=_tmp32E->f1;}};_tmp330=_tmp326.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp331=(struct Cyc_Absyn_Initializes_att_struct*)_tmp330;if(_tmp331->tag != 20)
goto _LL1AC;else{_tmp332=_tmp331->f1;}};_LL1AB: _tmp335=_tmp32F;_tmp338=_tmp332;
goto _LL1AD;_LL1AC: _tmp333=_tmp326.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp334=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp333;if(_tmp334->tag != 6)goto _LL1AE;else{
_tmp335=_tmp334->f1;}};_tmp336=_tmp326.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp337=(struct Cyc_Absyn_Aligned_att_struct*)_tmp336;if(_tmp337->tag != 6)goto
_LL1AE;else{_tmp338=_tmp337->f1;}};_LL1AD: return Cyc_Core_intcmp(_tmp335,_tmp338);
_LL1AE: _tmp339=_tmp326.f1;{struct Cyc_Absyn_Section_att_struct*_tmp33A=(struct Cyc_Absyn_Section_att_struct*)
_tmp339;if(_tmp33A->tag != 8)goto _LL1B0;else{_tmp33B=_tmp33A->f1;}};_tmp33C=
_tmp326.f2;{struct Cyc_Absyn_Section_att_struct*_tmp33D=(struct Cyc_Absyn_Section_att_struct*)
_tmp33C;if(_tmp33D->tag != 8)goto _LL1B0;else{_tmp33E=_tmp33D->f1;}};_LL1AF: return
Cyc_strcmp((struct _dyneither_ptr)_tmp33B,(struct _dyneither_ptr)_tmp33E);_LL1B0:
_tmp33F=_tmp326.f1;{struct Cyc_Absyn_Format_att_struct*_tmp340=(struct Cyc_Absyn_Format_att_struct*)
_tmp33F;if(_tmp340->tag != 19)goto _LL1B2;else{_tmp341=_tmp340->f1;_tmp342=_tmp340->f2;
_tmp343=_tmp340->f3;}};_tmp344=_tmp326.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp345=(struct Cyc_Absyn_Format_att_struct*)_tmp344;if(_tmp345->tag != 19)goto
_LL1B2;else{_tmp346=_tmp345->f1;_tmp347=_tmp345->f2;_tmp348=_tmp345->f3;}};
_LL1B1: {int _tmp349=Cyc_Core_intcmp((int)((unsigned int)_tmp341),(int)((
unsigned int)_tmp346));if(_tmp349 != 0)return _tmp349;{int _tmp34A=Cyc_Core_intcmp(
_tmp342,_tmp347);if(_tmp34A != 0)return _tmp34A;return Cyc_Core_intcmp(_tmp343,
_tmp348);};}_LL1B2:;_LL1B3: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
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
_tmp34B=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp34C;
enum Cyc_Absyn_KindQual _tmp34D;enum Cyc_Absyn_AliasQual _tmp34E;struct Cyc_Absyn_Kind
_tmp34F;enum Cyc_Absyn_KindQual _tmp350;enum Cyc_Absyn_AliasQual _tmp351;struct Cyc_Absyn_Kind
_tmp352;enum Cyc_Absyn_KindQual _tmp353;struct Cyc_Absyn_Kind _tmp354;enum Cyc_Absyn_KindQual
_tmp355;_LL1B5: _tmp34C=*_tmp34B;_tmp34D=_tmp34C.kind;if(_tmp34D != Cyc_Absyn_RgnKind)
goto _LL1B7;_tmp34E=_tmp34C.aliasqual;if(_tmp34E != Cyc_Absyn_Unique)goto _LL1B7;
_LL1B6: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B4;_LL1B7: _tmp34F=*_tmp34B;
_tmp350=_tmp34F.kind;if(_tmp350 != Cyc_Absyn_RgnKind)goto _LL1B9;_tmp351=_tmp34F.aliasqual;
if(_tmp351 != Cyc_Absyn_Aliasable)goto _LL1B9;_LL1B8: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1B4;_LL1B9: _tmp352=*_tmp34B;_tmp353=_tmp352.kind;if(_tmp353 != Cyc_Absyn_EffKind)
goto _LL1BB;_LL1BA: t=Cyc_Absyn_empty_effect;goto _LL1B4;_LL1BB: _tmp354=*_tmp34B;
_tmp355=_tmp354.kind;if(_tmp355 != Cyc_Absyn_IntKind)goto _LL1BD;_LL1BC:{struct Cyc_Absyn_ValueofType_struct
_tmp11BF;struct Cyc_Absyn_ValueofType_struct*_tmp11BE;t=(void*)((_tmp11BE=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11BE)),((_tmp11BE[0]=((
_tmp11BF.tag=19,((_tmp11BF.f1=Cyc_Absyn_uint_exp(0,0),_tmp11BF)))),_tmp11BE))));}
goto _LL1B4;_LL1BD:;_LL1BE: t=Cyc_Absyn_sint_typ;goto _LL1B4;_LL1B4:;}{struct
_tuple15*_tmp11C0;return(_tmp11C0=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp11C0)),((_tmp11C0->f1=tv,((_tmp11C0->f2=t,_tmp11C0)))));};}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp359=Cyc_Tcutil_compress(t);void*_tmp364;void*_tmp366;void*
_tmp367;struct Cyc_Absyn_DatatypeInfo _tmp369;struct Cyc_List_List*_tmp36A;struct
Cyc_Absyn_PtrInfo _tmp36C;void*_tmp36D;struct Cyc_Absyn_PtrAtts _tmp36E;void*
_tmp36F;struct Cyc_Absyn_ArrayInfo _tmp371;void*_tmp372;struct Cyc_List_List*
_tmp374;struct Cyc_Absyn_DatatypeFieldInfo _tmp376;struct Cyc_List_List*_tmp377;
struct Cyc_Absyn_AggrInfo _tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*
_tmp37C;struct Cyc_Absyn_FnInfo _tmp37F;struct Cyc_List_List*_tmp380;struct Cyc_Core_Opt*
_tmp381;struct Cyc_Absyn_Tqual _tmp382;void*_tmp383;struct Cyc_List_List*_tmp384;
struct Cyc_Absyn_VarargInfo*_tmp385;struct Cyc_List_List*_tmp386;void*_tmp38D;
struct Cyc_List_List*_tmp38F;_LL1C0: {struct Cyc_Absyn_VoidType_struct*_tmp35A=(
struct Cyc_Absyn_VoidType_struct*)_tmp359;if(_tmp35A->tag != 0)goto _LL1C2;}_LL1C1:
goto _LL1C3;_LL1C2: {struct Cyc_Absyn_FloatType_struct*_tmp35B=(struct Cyc_Absyn_FloatType_struct*)
_tmp359;if(_tmp35B->tag != 7)goto _LL1C4;}_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_DoubleType_struct*
_tmp35C=(struct Cyc_Absyn_DoubleType_struct*)_tmp359;if(_tmp35C->tag != 8)goto
_LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_EnumType_struct*_tmp35D=(
struct Cyc_Absyn_EnumType_struct*)_tmp359;if(_tmp35D->tag != 14)goto _LL1C8;}_LL1C7:
goto _LL1C9;_LL1C8: {struct Cyc_Absyn_AnonEnumType_struct*_tmp35E=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp359;if(_tmp35E->tag != 15)goto _LL1CA;}_LL1C9: goto _LL1CB;_LL1CA: {struct Cyc_Absyn_ValueofType_struct*
_tmp35F=(struct Cyc_Absyn_ValueofType_struct*)_tmp359;if(_tmp35F->tag != 19)goto
_LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_IntType_struct*_tmp360=(
struct Cyc_Absyn_IntType_struct*)_tmp359;if(_tmp360->tag != 6)goto _LL1CE;}_LL1CD:
return Cyc_Absyn_empty_effect;_LL1CE: {struct Cyc_Absyn_Evar_struct*_tmp361=(
struct Cyc_Absyn_Evar_struct*)_tmp359;if(_tmp361->tag != 1)goto _LL1D0;}_LL1CF: goto
_LL1D1;_LL1D0: {struct Cyc_Absyn_VarType_struct*_tmp362=(struct Cyc_Absyn_VarType_struct*)
_tmp359;if(_tmp362->tag != 2)goto _LL1D2;}_LL1D1: {struct Cyc_Absyn_Kind*_tmp391=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp392;enum Cyc_Absyn_KindQual
_tmp393;struct Cyc_Absyn_Kind _tmp394;enum Cyc_Absyn_KindQual _tmp395;struct Cyc_Absyn_Kind
_tmp396;enum Cyc_Absyn_KindQual _tmp397;_LL1F9: _tmp392=*_tmp391;_tmp393=_tmp392.kind;
if(_tmp393 != Cyc_Absyn_RgnKind)goto _LL1FB;_LL1FA: {struct Cyc_Absyn_AccessEff_struct
_tmp11C3;struct Cyc_Absyn_AccessEff_struct*_tmp11C2;return(void*)((_tmp11C2=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11C2)),((_tmp11C2[0]=((
_tmp11C3.tag=24,((_tmp11C3.f1=(void*)t,_tmp11C3)))),_tmp11C2))));}_LL1FB: _tmp394=*
_tmp391;_tmp395=_tmp394.kind;if(_tmp395 != Cyc_Absyn_EffKind)goto _LL1FD;_LL1FC:
return t;_LL1FD: _tmp396=*_tmp391;_tmp397=_tmp396.kind;if(_tmp397 != Cyc_Absyn_IntKind)
goto _LL1FF;_LL1FE: return Cyc_Absyn_empty_effect;_LL1FF:;_LL200: {struct Cyc_Absyn_RgnsEff_struct
_tmp11C6;struct Cyc_Absyn_RgnsEff_struct*_tmp11C5;return(void*)((_tmp11C5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11C5)),((_tmp11C5[0]=((
_tmp11C6.tag=26,((_tmp11C6.f1=(void*)t,_tmp11C6)))),_tmp11C5))));}_LL1F8:;}
_LL1D2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp363=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp359;if(_tmp363->tag != 16)goto _LL1D4;else{_tmp364=(void*)_tmp363->f1;}}_LL1D3: {
struct Cyc_Absyn_AccessEff_struct _tmp11C9;struct Cyc_Absyn_AccessEff_struct*
_tmp11C8;return(void*)((_tmp11C8=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp11C8)),((_tmp11C8[0]=((_tmp11C9.tag=24,((_tmp11C9.f1=(void*)_tmp364,_tmp11C9)))),
_tmp11C8))));}_LL1D4: {struct Cyc_Absyn_DynRgnType_struct*_tmp365=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp359;if(_tmp365->tag != 17)goto _LL1D6;else{_tmp366=(void*)_tmp365->f1;_tmp367=(
void*)_tmp365->f2;}}_LL1D5: {struct Cyc_Absyn_AccessEff_struct _tmp11CC;struct Cyc_Absyn_AccessEff_struct*
_tmp11CB;return(void*)((_tmp11CB=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp11CB)),((_tmp11CB[0]=((_tmp11CC.tag=24,((_tmp11CC.f1=(void*)_tmp367,_tmp11CC)))),
_tmp11CB))));}_LL1D6: {struct Cyc_Absyn_DatatypeType_struct*_tmp368=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp359;if(_tmp368->tag != 3)goto _LL1D8;else{_tmp369=_tmp368->f1;_tmp36A=_tmp369.targs;}}
_LL1D7: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36A);struct Cyc_Absyn_JoinEff_struct
_tmp11CF;struct Cyc_Absyn_JoinEff_struct*_tmp11CE;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11CE=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11CE)),((
_tmp11CE[0]=((_tmp11CF.tag=25,((_tmp11CF.f1=ts,_tmp11CF)))),_tmp11CE)))));}
_LL1D8: {struct Cyc_Absyn_PointerType_struct*_tmp36B=(struct Cyc_Absyn_PointerType_struct*)
_tmp359;if(_tmp36B->tag != 5)goto _LL1DA;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.elt_typ;
_tmp36E=_tmp36C.ptr_atts;_tmp36F=_tmp36E.rgn;}}_LL1D9: {struct Cyc_Absyn_JoinEff_struct
_tmp11DE;struct Cyc_Absyn_AccessEff_struct*_tmp11DD;struct Cyc_Absyn_AccessEff_struct
_tmp11DC;void*_tmp11DB[2];struct Cyc_Absyn_JoinEff_struct*_tmp11DA;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11DA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11DA)),((
_tmp11DA[0]=((_tmp11DE.tag=25,((_tmp11DE.f1=((_tmp11DB[1]=Cyc_Tcutil_rgns_of(
_tmp36D),((_tmp11DB[0]=(void*)((_tmp11DD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp11DD)),((_tmp11DD[0]=((_tmp11DC.tag=24,((_tmp11DC.f1=(void*)_tmp36F,
_tmp11DC)))),_tmp11DD)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp11DB,sizeof(void*),2)))))),_tmp11DE)))),_tmp11DA)))));}_LL1DA: {
struct Cyc_Absyn_ArrayType_struct*_tmp370=(struct Cyc_Absyn_ArrayType_struct*)
_tmp359;if(_tmp370->tag != 9)goto _LL1DC;else{_tmp371=_tmp370->f1;_tmp372=_tmp371.elt_type;}}
_LL1DB: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp372));_LL1DC: {
struct Cyc_Absyn_TupleType_struct*_tmp373=(struct Cyc_Absyn_TupleType_struct*)
_tmp359;if(_tmp373->tag != 11)goto _LL1DE;else{_tmp374=_tmp373->f1;}}_LL1DD: {
struct Cyc_List_List*_tmp3A7=0;for(0;_tmp374 != 0;_tmp374=_tmp374->tl){struct Cyc_List_List*
_tmp11DF;_tmp3A7=((_tmp11DF=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp11DF)),((_tmp11DF->hd=(*((struct _tuple11*)_tmp374->hd)).f2,((_tmp11DF->tl=
_tmp3A7,_tmp11DF))))));}_tmp377=_tmp3A7;goto _LL1DF;}_LL1DE: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp375=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp359;if(_tmp375->tag != 4)
goto _LL1E0;else{_tmp376=_tmp375->f1;_tmp377=_tmp376.targs;}}_LL1DF: _tmp37A=
_tmp377;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_AggrType_struct*_tmp378=(struct Cyc_Absyn_AggrType_struct*)
_tmp359;if(_tmp378->tag != 12)goto _LL1E2;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}
_LL1E1: {struct Cyc_Absyn_JoinEff_struct _tmp11E2;struct Cyc_Absyn_JoinEff_struct*
_tmp11E1;return Cyc_Tcutil_normalize_effect((void*)((_tmp11E1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp11E1)),((_tmp11E1[0]=((_tmp11E2.tag=25,((_tmp11E2.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37A),
_tmp11E2)))),_tmp11E1)))));}_LL1E2: {struct Cyc_Absyn_AnonAggrType_struct*_tmp37B=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp359;if(_tmp37B->tag != 13)goto _LL1E4;
else{_tmp37C=_tmp37B->f2;}}_LL1E3: {struct Cyc_Absyn_JoinEff_struct _tmp11E5;
struct Cyc_Absyn_JoinEff_struct*_tmp11E4;return Cyc_Tcutil_normalize_effect((void*)((
_tmp11E4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11E4)),((_tmp11E4[0]=((
_tmp11E5.tag=25,((_tmp11E5.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp37C),_tmp11E5)))),
_tmp11E4)))));}_LL1E4: {struct Cyc_Absyn_TagType_struct*_tmp37D=(struct Cyc_Absyn_TagType_struct*)
_tmp359;if(_tmp37D->tag != 20)goto _LL1E6;}_LL1E5: return Cyc_Absyn_empty_effect;
_LL1E6: {struct Cyc_Absyn_FnType_struct*_tmp37E=(struct Cyc_Absyn_FnType_struct*)
_tmp359;if(_tmp37E->tag != 10)goto _LL1E8;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37F.tvars;
_tmp381=_tmp37F.effect;_tmp382=_tmp37F.ret_tqual;_tmp383=_tmp37F.ret_typ;_tmp384=
_tmp37F.args;_tmp385=_tmp37F.cyc_varargs;_tmp386=_tmp37F.rgn_po;}}_LL1E7: {void*
_tmp3AD=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple15*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp380),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp381))->v);return Cyc_Tcutil_normalize_effect(
_tmp3AD);}_LL1E8: {struct Cyc_Absyn_UniqueRgn_struct*_tmp387=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp359;if(_tmp387->tag != 22)goto _LL1EA;}_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp388=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp359;if(_tmp388->tag != 23)goto
_LL1EC;}_LL1EB: goto _LL1ED;_LL1EC: {struct Cyc_Absyn_HeapRgn_struct*_tmp389=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp359;if(_tmp389->tag != 21)goto _LL1EE;}_LL1ED:
return Cyc_Absyn_empty_effect;_LL1EE: {struct Cyc_Absyn_AccessEff_struct*_tmp38A=(
struct Cyc_Absyn_AccessEff_struct*)_tmp359;if(_tmp38A->tag != 24)goto _LL1F0;}
_LL1EF: goto _LL1F1;_LL1F0: {struct Cyc_Absyn_JoinEff_struct*_tmp38B=(struct Cyc_Absyn_JoinEff_struct*)
_tmp359;if(_tmp38B->tag != 25)goto _LL1F2;}_LL1F1: return t;_LL1F2: {struct Cyc_Absyn_RgnsEff_struct*
_tmp38C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp359;if(_tmp38C->tag != 26)goto _LL1F4;
else{_tmp38D=(void*)_tmp38C->f1;}}_LL1F3: return Cyc_Tcutil_rgns_of(_tmp38D);
_LL1F4: {struct Cyc_Absyn_TypedefType_struct*_tmp38E=(struct Cyc_Absyn_TypedefType_struct*)
_tmp359;if(_tmp38E->tag != 18)goto _LL1F6;else{_tmp38F=_tmp38E->f2;}}_LL1F5: {
struct Cyc_Absyn_JoinEff_struct _tmp11E8;struct Cyc_Absyn_JoinEff_struct*_tmp11E7;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11E7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp11E7)),((_tmp11E7[0]=((_tmp11E8.tag=25,((_tmp11E8.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp38F),
_tmp11E8)))),_tmp11E7)))));}_LL1F6: {struct Cyc_Absyn_TypeDeclType_struct*_tmp390=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp359;if(_tmp390->tag != 27)goto _LL1BF;}
_LL1F7: {const char*_tmp11EB;void*_tmp11EA;(_tmp11EA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11EB="typedecl in rgns_of",
_tag_dyneither(_tmp11EB,sizeof(char),20))),_tag_dyneither(_tmp11EA,sizeof(void*),
0)));}_LL1BF:;}void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp3B2=e;struct Cyc_List_List*_tmp3B4;struct Cyc_List_List**_tmp3B5;void*
_tmp3B7;_LL202: {struct Cyc_Absyn_JoinEff_struct*_tmp3B3=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3B2;if(_tmp3B3->tag != 25)goto _LL204;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=(struct
Cyc_List_List**)& _tmp3B3->f1;}}_LL203: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp3B5;for(0;effs != 0;effs=effs->tl){void*_tmp3B8=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp3B8));{void*_tmp3B9=(
void*)effs->hd;void*_tmp3BC;void*_tmp3BF;void*_tmp3C2;_LL209: {struct Cyc_Absyn_JoinEff_struct*
_tmp3BA=(struct Cyc_Absyn_JoinEff_struct*)_tmp3B9;if(_tmp3BA->tag != 25)goto _LL20B;}
_LL20A: goto _LL20C;_LL20B: {struct Cyc_Absyn_AccessEff_struct*_tmp3BB=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3B9;if(_tmp3BB->tag != 24)goto _LL20D;else{_tmp3BC=(void*)_tmp3BB->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp3BD=(struct Cyc_Absyn_HeapRgn_struct*)_tmp3BC;if(
_tmp3BD->tag != 21)goto _LL20D;};}}_LL20C: redo_join=1;goto _LL208;_LL20D: {struct
Cyc_Absyn_AccessEff_struct*_tmp3BE=(struct Cyc_Absyn_AccessEff_struct*)_tmp3B9;
if(_tmp3BE->tag != 24)goto _LL20F;else{_tmp3BF=(void*)_tmp3BE->f1;{struct Cyc_Absyn_RefCntRgn_struct*
_tmp3C0=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp3BF;if(_tmp3C0->tag != 23)goto
_LL20F;};}}_LL20E: redo_join=1;goto _LL208;_LL20F: {struct Cyc_Absyn_AccessEff_struct*
_tmp3C1=(struct Cyc_Absyn_AccessEff_struct*)_tmp3B9;if(_tmp3C1->tag != 24)goto
_LL211;else{_tmp3C2=(void*)_tmp3C1->f1;{struct Cyc_Absyn_UniqueRgn_struct*_tmp3C3=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp3C2;if(_tmp3C3->tag != 22)goto _LL211;};}}
_LL210: redo_join=1;goto _LL208;_LL211:;_LL212: goto _LL208;_LL208:;};}}if(!
redo_join)return e;{struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*
_tmp3B5;for(0;effs != 0;effs=effs->tl){void*_tmp3C4=Cyc_Tcutil_compress((void*)
effs->hd);struct Cyc_List_List*_tmp3C6;void*_tmp3C8;void*_tmp3CB;void*_tmp3CE;
_LL214: {struct Cyc_Absyn_JoinEff_struct*_tmp3C5=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3C4;if(_tmp3C5->tag != 25)goto _LL216;else{_tmp3C6=_tmp3C5->f1;}}_LL215:
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_revappend)(_tmp3C6,effects);goto _LL213;_LL216: {struct Cyc_Absyn_AccessEff_struct*
_tmp3C7=(struct Cyc_Absyn_AccessEff_struct*)_tmp3C4;if(_tmp3C7->tag != 24)goto
_LL218;else{_tmp3C8=(void*)_tmp3C7->f1;{struct Cyc_Absyn_HeapRgn_struct*_tmp3C9=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp3C8;if(_tmp3C9->tag != 21)goto _LL218;};}}
_LL217: goto _LL213;_LL218: {struct Cyc_Absyn_AccessEff_struct*_tmp3CA=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C4;if(_tmp3CA->tag != 24)goto _LL21A;else{_tmp3CB=(void*)_tmp3CA->f1;{struct
Cyc_Absyn_RefCntRgn_struct*_tmp3CC=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp3CB;
if(_tmp3CC->tag != 23)goto _LL21A;};}}_LL219: goto _LL213;_LL21A: {struct Cyc_Absyn_AccessEff_struct*
_tmp3CD=(struct Cyc_Absyn_AccessEff_struct*)_tmp3C4;if(_tmp3CD->tag != 24)goto
_LL21C;else{_tmp3CE=(void*)_tmp3CD->f1;{struct Cyc_Absyn_UniqueRgn_struct*_tmp3CF=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp3CE;if(_tmp3CF->tag != 22)goto _LL21C;};}}
_LL21B: goto _LL213;_LL21C:;_LL21D:{struct Cyc_List_List*_tmp11EC;effects=((
_tmp11EC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp11EC)),((_tmp11EC->hd=
_tmp3C4,((_tmp11EC->tl=effects,_tmp11EC))))));}goto _LL213;_LL213:;}}*_tmp3B5=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return
e;};}_LL204: {struct Cyc_Absyn_RgnsEff_struct*_tmp3B6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3B2;if(_tmp3B6->tag != 26)goto _LL206;else{_tmp3B7=(void*)_tmp3B6->f1;}}_LL205: {
void*_tmp3D1=Cyc_Tcutil_compress(_tmp3B7);_LL21F: {struct Cyc_Absyn_Evar_struct*
_tmp3D2=(struct Cyc_Absyn_Evar_struct*)_tmp3D1;if(_tmp3D2->tag != 1)goto _LL221;}
_LL220: goto _LL222;_LL221: {struct Cyc_Absyn_VarType_struct*_tmp3D3=(struct Cyc_Absyn_VarType_struct*)
_tmp3D1;if(_tmp3D3->tag != 2)goto _LL223;}_LL222: return e;_LL223:;_LL224: return Cyc_Tcutil_rgns_of(
_tmp3B7);_LL21E:;}_LL206:;_LL207: return e;_LL201:;};}static void*Cyc_Tcutil_dummy_fntype(
void*eff){struct Cyc_Absyn_FnType_struct _tmp11F6;struct Cyc_Core_Opt*_tmp11F5;
struct Cyc_Absyn_FnInfo _tmp11F4;struct Cyc_Absyn_FnType_struct*_tmp11F3;struct Cyc_Absyn_FnType_struct*
_tmp3D4=(_tmp11F3=_cycalloc(sizeof(*_tmp11F3)),((_tmp11F3[0]=((_tmp11F6.tag=10,((
_tmp11F6.f1=((_tmp11F4.tvars=0,((_tmp11F4.effect=((_tmp11F5=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp11F5)),((_tmp11F5->v=eff,_tmp11F5)))),((_tmp11F4.ret_tqual=Cyc_Absyn_empty_tqual(
0),((_tmp11F4.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp11F4.args=0,((
_tmp11F4.c_varargs=0,((_tmp11F4.cyc_varargs=0,((_tmp11F4.rgn_po=0,((_tmp11F4.attributes=
0,_tmp11F4)))))))))))))))))),_tmp11F6)))),_tmp11F3)));return Cyc_Absyn_atb_typ((
void*)_tmp3D4,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,
Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*
e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)&
Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)return 1;{void*
_tmp3D9=Cyc_Tcutil_compress(e);void*_tmp3DB;struct Cyc_List_List*_tmp3DD;void*
_tmp3DF;struct Cyc_Core_Opt*_tmp3E1;struct Cyc_Core_Opt*_tmp3E2;struct Cyc_Core_Opt**
_tmp3E3;struct Cyc_Core_Opt*_tmp3E4;_LL226: {struct Cyc_Absyn_AccessEff_struct*
_tmp3DA=(struct Cyc_Absyn_AccessEff_struct*)_tmp3D9;if(_tmp3DA->tag != 24)goto
_LL228;else{_tmp3DB=(void*)_tmp3DA->f1;}}_LL227: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3DB);_tmp3DB=Cyc_Tcutil_compress(_tmp3DB);if(r == _tmp3DB)return 1;{struct
_tuple0 _tmp11F7;struct _tuple0 _tmp3E6=(_tmp11F7.f1=r,((_tmp11F7.f2=_tmp3DB,
_tmp11F7)));void*_tmp3E7;struct Cyc_Absyn_Tvar*_tmp3E9;void*_tmp3EA;struct Cyc_Absyn_Tvar*
_tmp3EC;_LL231: _tmp3E7=_tmp3E6.f1;{struct Cyc_Absyn_VarType_struct*_tmp3E8=(
struct Cyc_Absyn_VarType_struct*)_tmp3E7;if(_tmp3E8->tag != 2)goto _LL233;else{
_tmp3E9=_tmp3E8->f1;}};_tmp3EA=_tmp3E6.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3EB=(struct Cyc_Absyn_VarType_struct*)_tmp3EA;if(_tmp3EB->tag != 2)goto _LL233;
else{_tmp3EC=_tmp3EB->f1;}};_LL232: return Cyc_Absyn_tvar_cmp(_tmp3E9,_tmp3EC)== 0;
_LL233:;_LL234: return 0;_LL230:;};_LL228: {struct Cyc_Absyn_JoinEff_struct*_tmp3DC=(
struct Cyc_Absyn_JoinEff_struct*)_tmp3D9;if(_tmp3DC->tag != 25)goto _LL22A;else{
_tmp3DD=_tmp3DC->f1;}}_LL229: for(0;_tmp3DD != 0;_tmp3DD=_tmp3DD->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp3DD->hd))return 1;}return 0;_LL22A: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3DE=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3D9;if(_tmp3DE->tag != 26)goto _LL22C;
else{_tmp3DF=(void*)_tmp3DE->f1;}}_LL22B: {void*_tmp3ED=Cyc_Tcutil_rgns_of(
_tmp3DF);void*_tmp3EF;_LL236: {struct Cyc_Absyn_RgnsEff_struct*_tmp3EE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3ED;if(_tmp3EE->tag != 26)goto _LL238;else{_tmp3EF=(void*)_tmp3EE->f1;}}_LL237:
if(!constrain)return 0;{void*_tmp3F0=Cyc_Tcutil_compress(_tmp3EF);struct Cyc_Core_Opt*
_tmp3F2;struct Cyc_Core_Opt*_tmp3F3;struct Cyc_Core_Opt**_tmp3F4;struct Cyc_Core_Opt*
_tmp3F5;_LL23B: {struct Cyc_Absyn_Evar_struct*_tmp3F1=(struct Cyc_Absyn_Evar_struct*)
_tmp3F0;if(_tmp3F1->tag != 1)goto _LL23D;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;
_tmp3F4=(struct Cyc_Core_Opt**)& _tmp3F1->f2;_tmp3F5=_tmp3F1->f4;}}_LL23C: {void*
_tmp3F6=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3F5);Cyc_Tcutil_occurs(
_tmp3F6,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F5))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1206;struct Cyc_Absyn_AccessEff_struct*
_tmp1205;struct Cyc_Absyn_AccessEff_struct _tmp1204;void*_tmp1203[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1202;void*_tmp3F7=Cyc_Tcutil_dummy_fntype((void*)((_tmp1202=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp1202)),((_tmp1202[0]=((_tmp1206.tag=25,((
_tmp1206.f1=((_tmp1203[1]=(void*)((_tmp1205=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1205)),((_tmp1205[0]=((_tmp1204.tag=24,((_tmp1204.f1=(void*)r,
_tmp1204)))),_tmp1205)))),((_tmp1203[0]=_tmp3F6,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1203,sizeof(void*),2)))))),
_tmp1206)))),_tmp1202)))));{struct Cyc_Core_Opt*_tmp1207;*_tmp3F4=((_tmp1207=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1207)),((_tmp1207->v=_tmp3F7,
_tmp1207))));}return 1;};}_LL23D:;_LL23E: return 0;_LL23A:;};_LL238:;_LL239: return
Cyc_Tcutil_region_in_effect(constrain,r,_tmp3ED);_LL235:;}_LL22C: {struct Cyc_Absyn_Evar_struct*
_tmp3E0=(struct Cyc_Absyn_Evar_struct*)_tmp3D9;if(_tmp3E0->tag != 1)goto _LL22E;
else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;_tmp3E3=(struct Cyc_Core_Opt**)&
_tmp3E0->f2;_tmp3E4=_tmp3E0->f4;}}_LL22D: if(_tmp3E1 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp3E1->v)->kind != Cyc_Absyn_EffKind){const char*_tmp120A;void*_tmp1209;(
_tmp1209=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp120A="effect evar has wrong kind",_tag_dyneither(_tmp120A,sizeof(char),27))),
_tag_dyneither(_tmp1209,sizeof(void*),0)));}if(!constrain)return 0;{void*_tmp400=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3E4);Cyc_Tcutil_occurs(
_tmp400,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3E4))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp121F;struct Cyc_List_List*
_tmp121E;struct Cyc_Absyn_AccessEff_struct _tmp121D;struct Cyc_Absyn_AccessEff_struct*
_tmp121C;struct Cyc_List_List*_tmp121B;struct Cyc_Absyn_JoinEff_struct*_tmp121A;
struct Cyc_Absyn_JoinEff_struct*_tmp401=(_tmp121A=_cycalloc(sizeof(*_tmp121A)),((
_tmp121A[0]=((_tmp121F.tag=25,((_tmp121F.f1=((_tmp121B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp121B)),((_tmp121B->hd=_tmp400,((_tmp121B->tl=((_tmp121E=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp121E)),((_tmp121E->hd=(void*)((
_tmp121C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp121C)),((_tmp121C[0]=((
_tmp121D.tag=24,((_tmp121D.f1=(void*)r,_tmp121D)))),_tmp121C)))),((_tmp121E->tl=
0,_tmp121E)))))),_tmp121B)))))),_tmp121F)))),_tmp121A)));{struct Cyc_Core_Opt*
_tmp1220;*_tmp3E3=((_tmp1220=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1220)),((_tmp1220->v=(void*)_tmp401,_tmp1220))));}return 1;};};_LL22E:;_LL22F:
return 0;_LL225:;};}static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,
void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp409=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp40C;void*_tmp40E;struct Cyc_Core_Opt*
_tmp410;struct Cyc_Core_Opt*_tmp411;struct Cyc_Core_Opt**_tmp412;struct Cyc_Core_Opt*
_tmp413;_LL240: {struct Cyc_Absyn_AccessEff_struct*_tmp40A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp409;if(_tmp40A->tag != 24)goto _LL242;}_LL241: return 0;_LL242: {struct Cyc_Absyn_JoinEff_struct*
_tmp40B=(struct Cyc_Absyn_JoinEff_struct*)_tmp409;if(_tmp40B->tag != 25)goto _LL244;
else{_tmp40C=_tmp40B->f1;}}_LL243: for(0;_tmp40C != 0;_tmp40C=_tmp40C->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp40C->hd))return 1;}return 0;_LL244: {struct Cyc_Absyn_RgnsEff_struct*
_tmp40D=(struct Cyc_Absyn_RgnsEff_struct*)_tmp409;if(_tmp40D->tag != 26)goto _LL246;
else{_tmp40E=(void*)_tmp40D->f1;}}_LL245: _tmp40E=Cyc_Tcutil_compress(_tmp40E);
if(t == _tmp40E)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp40E);{
void*_tmp414=Cyc_Tcutil_rgns_of(t);void*_tmp416;_LL24B: {struct Cyc_Absyn_RgnsEff_struct*
_tmp415=(struct Cyc_Absyn_RgnsEff_struct*)_tmp414;if(_tmp415->tag != 26)goto _LL24D;
else{_tmp416=(void*)_tmp415->f1;}}_LL24C: {struct _tuple0 _tmp1221;struct _tuple0
_tmp418=(_tmp1221.f1=Cyc_Tcutil_compress(_tmp416),((_tmp1221.f2=_tmp40E,_tmp1221)));
void*_tmp419;struct Cyc_Absyn_Tvar*_tmp41B;void*_tmp41C;struct Cyc_Absyn_Tvar*
_tmp41E;_LL250: _tmp419=_tmp418.f1;{struct Cyc_Absyn_VarType_struct*_tmp41A=(
struct Cyc_Absyn_VarType_struct*)_tmp419;if(_tmp41A->tag != 2)goto _LL252;else{
_tmp41B=_tmp41A->f1;}};_tmp41C=_tmp418.f2;{struct Cyc_Absyn_VarType_struct*
_tmp41D=(struct Cyc_Absyn_VarType_struct*)_tmp41C;if(_tmp41D->tag != 2)goto _LL252;
else{_tmp41E=_tmp41D->f1;}};_LL251: return Cyc_Tcutil_unify(t,_tmp40E);_LL252:;
_LL253: return _tmp416 == _tmp40E;_LL24F:;}_LL24D:;_LL24E: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp414);_LL24A:;};_LL246: {struct Cyc_Absyn_Evar_struct*
_tmp40F=(struct Cyc_Absyn_Evar_struct*)_tmp409;if(_tmp40F->tag != 1)goto _LL248;
else{_tmp410=_tmp40F->f1;_tmp411=_tmp40F->f2;_tmp412=(struct Cyc_Core_Opt**)&
_tmp40F->f2;_tmp413=_tmp40F->f4;}}_LL247: if(_tmp410 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp410->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1224;void*_tmp1223;(
_tmp1223=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1224="effect evar has wrong kind",_tag_dyneither(_tmp1224,sizeof(char),27))),
_tag_dyneither(_tmp1223,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp421=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp413);
Cyc_Tcutil_occurs(_tmp421,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp413))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1239;struct Cyc_List_List*
_tmp1238;struct Cyc_Absyn_RgnsEff_struct _tmp1237;struct Cyc_Absyn_RgnsEff_struct*
_tmp1236;struct Cyc_List_List*_tmp1235;struct Cyc_Absyn_JoinEff_struct*_tmp1234;
struct Cyc_Absyn_JoinEff_struct*_tmp422=(_tmp1234=_cycalloc(sizeof(*_tmp1234)),((
_tmp1234[0]=((_tmp1239.tag=25,((_tmp1239.f1=((_tmp1235=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1235)),((_tmp1235->hd=_tmp421,((_tmp1235->tl=((_tmp1238=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1238)),((_tmp1238->hd=(void*)((
_tmp1236=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1236)),((_tmp1236[0]=((
_tmp1237.tag=26,((_tmp1237.f1=(void*)t,_tmp1237)))),_tmp1236)))),((_tmp1238->tl=
0,_tmp1238)))))),_tmp1235)))))),_tmp1239)))),_tmp1234)));{struct Cyc_Core_Opt*
_tmp123A;*_tmp412=((_tmp123A=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp123A)),((_tmp123A->v=(void*)_tmp422,_tmp123A))));}return 1;};};_LL248:;_LL249:
return 0;_LL23F:;};}static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,
struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp42A=e;struct
Cyc_Absyn_Tvar*_tmp42C;struct Cyc_List_List*_tmp42E;void*_tmp430;struct Cyc_Core_Opt*
_tmp432;struct Cyc_Core_Opt*_tmp433;struct Cyc_Core_Opt**_tmp434;struct Cyc_Core_Opt*
_tmp435;_LL255: {struct Cyc_Absyn_VarType_struct*_tmp42B=(struct Cyc_Absyn_VarType_struct*)
_tmp42A;if(_tmp42B->tag != 2)goto _LL257;else{_tmp42C=_tmp42B->f1;}}_LL256: return
Cyc_Absyn_tvar_cmp(v,_tmp42C)== 0;_LL257: {struct Cyc_Absyn_JoinEff_struct*
_tmp42D=(struct Cyc_Absyn_JoinEff_struct*)_tmp42A;if(_tmp42D->tag != 25)goto _LL259;
else{_tmp42E=_tmp42D->f1;}}_LL258: for(0;_tmp42E != 0;_tmp42E=_tmp42E->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp42E->hd))return 1;}return 0;_LL259: {struct Cyc_Absyn_RgnsEff_struct*
_tmp42F=(struct Cyc_Absyn_RgnsEff_struct*)_tmp42A;if(_tmp42F->tag != 26)goto _LL25B;
else{_tmp430=(void*)_tmp42F->f1;}}_LL25A: {void*_tmp436=Cyc_Tcutil_rgns_of(
_tmp430);void*_tmp438;_LL260: {struct Cyc_Absyn_RgnsEff_struct*_tmp437=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp436;if(_tmp437->tag != 26)goto _LL262;else{_tmp438=(void*)_tmp437->f1;}}_LL261:
if(!may_constrain_evars)return 0;{void*_tmp439=Cyc_Tcutil_compress(_tmp438);
struct Cyc_Core_Opt*_tmp43B;struct Cyc_Core_Opt*_tmp43C;struct Cyc_Core_Opt**
_tmp43D;struct Cyc_Core_Opt*_tmp43E;_LL265: {struct Cyc_Absyn_Evar_struct*_tmp43A=(
struct Cyc_Absyn_Evar_struct*)_tmp439;if(_tmp43A->tag != 1)goto _LL267;else{_tmp43B=
_tmp43A->f1;_tmp43C=_tmp43A->f2;_tmp43D=(struct Cyc_Core_Opt**)& _tmp43A->f2;
_tmp43E=_tmp43A->f4;}}_LL266: {void*_tmp43F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp43E);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp43E))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1249;struct Cyc_Absyn_VarType_struct*_tmp1248;
struct Cyc_Absyn_VarType_struct _tmp1247;void*_tmp1246[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1245;void*_tmp440=Cyc_Tcutil_dummy_fntype((void*)((_tmp1245=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp1245)),((_tmp1245[0]=((_tmp1249.tag=25,((
_tmp1249.f1=((_tmp1246[1]=(void*)((_tmp1248=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1248)),((_tmp1248[0]=((_tmp1247.tag=2,((_tmp1247.f1=v,_tmp1247)))),
_tmp1248)))),((_tmp1246[0]=_tmp43F,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1246,sizeof(void*),2)))))),
_tmp1249)))),_tmp1245)))));{struct Cyc_Core_Opt*_tmp124A;*_tmp43D=((_tmp124A=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp124A)),((_tmp124A->v=_tmp440,
_tmp124A))));}return 1;};}_LL267:;_LL268: return 0;_LL264:;};_LL262:;_LL263: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp436);_LL25F:;}_LL25B: {
struct Cyc_Absyn_Evar_struct*_tmp431=(struct Cyc_Absyn_Evar_struct*)_tmp42A;if(
_tmp431->tag != 1)goto _LL25D;else{_tmp432=_tmp431->f1;_tmp433=_tmp431->f2;_tmp434=(
struct Cyc_Core_Opt**)& _tmp431->f2;_tmp435=_tmp431->f4;}}_LL25C: if(_tmp432 == 0
 || ((struct Cyc_Absyn_Kind*)_tmp432->v)->kind != Cyc_Absyn_EffKind){const char*
_tmp124D;void*_tmp124C;(_tmp124C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp124D="effect evar has wrong kind",
_tag_dyneither(_tmp124D,sizeof(char),27))),_tag_dyneither(_tmp124C,sizeof(void*),
0)));}{void*_tmp449=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,
_tmp435);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp435))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp1262;struct Cyc_List_List*_tmp1261;struct Cyc_Absyn_VarType_struct
_tmp1260;struct Cyc_Absyn_VarType_struct*_tmp125F;struct Cyc_List_List*_tmp125E;
struct Cyc_Absyn_JoinEff_struct*_tmp125D;struct Cyc_Absyn_JoinEff_struct*_tmp44A=(
_tmp125D=_cycalloc(sizeof(*_tmp125D)),((_tmp125D[0]=((_tmp1262.tag=25,((_tmp1262.f1=((
_tmp125E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp125E)),((_tmp125E->hd=
_tmp449,((_tmp125E->tl=((_tmp1261=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1261)),((_tmp1261->hd=(void*)((_tmp125F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp125F)),((_tmp125F[0]=((_tmp1260.tag=2,((_tmp1260.f1=v,_tmp1260)))),
_tmp125F)))),((_tmp1261->tl=0,_tmp1261)))))),_tmp125E)))))),_tmp1262)))),
_tmp125D)));{struct Cyc_Core_Opt*_tmp1263;*_tmp434=((_tmp1263=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1263)),((_tmp1263->v=(void*)_tmp44A,_tmp1263))));}return 1;};};_LL25D:;
_LL25E: return 0;_LL254:;};}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=
Cyc_Tcutil_compress(e);{void*_tmp452=e;struct Cyc_List_List*_tmp454;void*_tmp456;
_LL26A: {struct Cyc_Absyn_JoinEff_struct*_tmp453=(struct Cyc_Absyn_JoinEff_struct*)
_tmp452;if(_tmp453->tag != 25)goto _LL26C;else{_tmp454=_tmp453->f1;}}_LL26B: for(0;
_tmp454 != 0;_tmp454=_tmp454->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp454->hd))return 1;}return 0;_LL26C: {struct Cyc_Absyn_RgnsEff_struct*_tmp455=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp452;if(_tmp455->tag != 26)goto _LL26E;else{
_tmp456=(void*)_tmp455->f1;}}_LL26D: {void*_tmp458=Cyc_Tcutil_rgns_of(_tmp456);
void*_tmp45A;_LL273: {struct Cyc_Absyn_RgnsEff_struct*_tmp459=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp458;if(_tmp459->tag != 26)goto _LL275;else{_tmp45A=(void*)_tmp459->f1;}}_LL274:
return 0;_LL275:;_LL276: return Cyc_Tcutil_evar_in_effect(evar,_tmp458);_LL272:;}
_LL26E: {struct Cyc_Absyn_Evar_struct*_tmp457=(struct Cyc_Absyn_Evar_struct*)
_tmp452;if(_tmp457->tag != 1)goto _LL270;}_LL26F: return evar == e;_LL270:;_LL271:
return 0;_LL269:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2){void*_tmp45B=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp45D;void*
_tmp45F;struct Cyc_Absyn_Tvar*_tmp461;void*_tmp463;struct Cyc_Core_Opt*_tmp465;
struct Cyc_Core_Opt**_tmp466;struct Cyc_Core_Opt*_tmp467;_LL278: {struct Cyc_Absyn_JoinEff_struct*
_tmp45C=(struct Cyc_Absyn_JoinEff_struct*)_tmp45B;if(_tmp45C->tag != 25)goto _LL27A;
else{_tmp45D=_tmp45C->f1;}}_LL279: for(0;_tmp45D != 0;_tmp45D=_tmp45D->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp45D->hd,e2))return 0;}return 1;_LL27A: {struct Cyc_Absyn_AccessEff_struct*
_tmp45E=(struct Cyc_Absyn_AccessEff_struct*)_tmp45B;if(_tmp45E->tag != 24)goto
_LL27C;else{_tmp45F=(void*)_tmp45E->f1;}}_LL27B: return Cyc_Tcutil_region_in_effect(
0,_tmp45F,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp45F,(void*)& Cyc_Absyn_HeapRgn_val);
_LL27C: {struct Cyc_Absyn_VarType_struct*_tmp460=(struct Cyc_Absyn_VarType_struct*)
_tmp45B;if(_tmp460->tag != 2)goto _LL27E;else{_tmp461=_tmp460->f1;}}_LL27D: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp461,e2);_LL27E: {struct Cyc_Absyn_RgnsEff_struct*
_tmp462=(struct Cyc_Absyn_RgnsEff_struct*)_tmp45B;if(_tmp462->tag != 26)goto _LL280;
else{_tmp463=(void*)_tmp462->f1;}}_LL27F: {void*_tmp468=Cyc_Tcutil_rgns_of(
_tmp463);void*_tmp46A;_LL285: {struct Cyc_Absyn_RgnsEff_struct*_tmp469=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp468;if(_tmp469->tag != 26)goto _LL287;else{_tmp46A=(void*)_tmp469->f1;}}_LL286:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp46A,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp46A,Cyc_Absyn_sint_typ);_LL287:;
_LL288: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp468,e2);_LL284:;}
_LL280: {struct Cyc_Absyn_Evar_struct*_tmp464=(struct Cyc_Absyn_Evar_struct*)
_tmp45B;if(_tmp464->tag != 1)goto _LL282;else{_tmp465=_tmp464->f2;_tmp466=(struct
Cyc_Core_Opt**)& _tmp464->f2;_tmp467=_tmp464->f4;}}_LL281: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp1264;*_tmp466=((_tmp1264=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1264)),((_tmp1264->v=Cyc_Absyn_empty_effect,_tmp1264))));}return 1;
_LL282:;_LL283: {const char*_tmp1268;void*_tmp1267[1];struct Cyc_String_pa_struct
_tmp1266;(_tmp1266.tag=0,((_tmp1266.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1267[0]=& _tmp1266,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1268="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1268,sizeof(char),30))),_tag_dyneither(_tmp1267,sizeof(void*),
1)))))));}_LL277:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp470;void*_tmp471;void*_tmp472;struct
_tuple0*_tmp46F=(struct _tuple0*)r1->hd;_tmp470=*_tmp46F;_tmp471=_tmp470.f1;
_tmp472=_tmp470.f2;{int found=_tmp471 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp474;void*_tmp475;void*
_tmp476;struct _tuple0*_tmp473=(struct _tuple0*)r2->hd;_tmp474=*_tmp473;_tmp475=
_tmp474.f1;_tmp476=_tmp474.f2;if(Cyc_Tcutil_unify(_tmp471,_tmp475) && Cyc_Tcutil_unify(
_tmp472,_tmp476)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple21{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple22{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp477=t1;struct Cyc_Core_Opt*_tmp479;struct Cyc_Core_Opt*
_tmp47A;struct Cyc_Core_Opt**_tmp47B;struct Cyc_Core_Opt*_tmp47C;_LL28A: {struct
Cyc_Absyn_Evar_struct*_tmp478=(struct Cyc_Absyn_Evar_struct*)_tmp477;if(_tmp478->tag
!= 1)goto _LL28C;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;_tmp47B=(struct Cyc_Core_Opt**)&
_tmp478->f2;_tmp47C=_tmp478->f4;}}_LL28B: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47C))->v,t2);{struct
Cyc_Absyn_Kind*_tmp47D=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp47D,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp479))->v)){{struct
Cyc_Core_Opt*_tmp1269;*_tmp47B=((_tmp1269=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1269)),((_tmp1269->v=t2,_tmp1269))));}return;}else{{void*_tmp47F=t2;
struct Cyc_Core_Opt*_tmp481;struct Cyc_Core_Opt**_tmp482;struct Cyc_Core_Opt*
_tmp483;struct Cyc_Absyn_PtrInfo _tmp485;_LL28F: {struct Cyc_Absyn_Evar_struct*
_tmp480=(struct Cyc_Absyn_Evar_struct*)_tmp47F;if(_tmp480->tag != 1)goto _LL291;
else{_tmp481=_tmp480->f2;_tmp482=(struct Cyc_Core_Opt**)& _tmp480->f2;_tmp483=
_tmp480->f4;}}_LL290: {struct Cyc_List_List*_tmp486=(struct Cyc_List_List*)_tmp47C->v;{
struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp483))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,_tmp486,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*
_tmp126A;Cyc_Tcutil_failure_reason=((_tmp126A="(type variable would escape scope)",
_tag_dyneither(_tmp126A,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp479->v,_tmp47D)){{struct Cyc_Core_Opt*
_tmp126B;*_tmp482=((_tmp126B=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp126B)),((_tmp126B->v=t1,_tmp126B))));}return;}{const char*_tmp126C;Cyc_Tcutil_failure_reason=((
_tmp126C="(kinds are incompatible)",_tag_dyneither(_tmp126C,sizeof(char),25)));}
goto _LL28E;}_LL291:{struct Cyc_Absyn_PointerType_struct*_tmp484=(struct Cyc_Absyn_PointerType_struct*)
_tmp47F;if(_tmp484->tag != 5)goto _LL293;else{_tmp485=_tmp484->f1;}}if(!(((struct
Cyc_Absyn_Kind*)_tmp479->v)->kind == Cyc_Absyn_BoxKind))goto _LL293;_LL292: {union
Cyc_Absyn_Constraint*_tmp48A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)((_tmp485.ptr_atts).bounds);{union Cyc_Absyn_Constraint*
_tmp48B=_tmp48A;union Cyc_Absyn_Constraint _tmp48C;int _tmp48D;_LL296: _tmp48C=*
_tmp48B;if((_tmp48C.No_constr).tag != 3)goto _LL298;_tmp48D=(int)(_tmp48C.No_constr).val;
_LL297:{struct _union_Constraint_Eq_constr*_tmp126D;(_tmp126D=& _tmp48A->Eq_constr,((
_tmp126D->tag=1,_tmp126D->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*
_tmp126E;*_tmp47B=((_tmp126E=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp126E)),((_tmp126E->v=t2,_tmp126E))));}return;_LL298:;_LL299: goto _LL295;
_LL295:;}goto _LL28E;}_LL293:;_LL294: goto _LL28E;_LL28E:;}{const char*_tmp126F;Cyc_Tcutil_failure_reason=((
_tmp126F="(kinds are incompatible)",_tag_dyneither(_tmp126F,sizeof(char),25)));}(
int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL28C:;_LL28D: goto _LL289;_LL289:;}{
struct _tuple0 _tmp1270;struct _tuple0 _tmp492=(_tmp1270.f1=t2,((_tmp1270.f2=t1,
_tmp1270)));void*_tmp493;void*_tmp495;void*_tmp497;void*_tmp499;struct Cyc_Absyn_Tvar*
_tmp49B;void*_tmp49C;struct Cyc_Absyn_Tvar*_tmp49E;void*_tmp49F;struct Cyc_Absyn_AggrInfo
_tmp4A1;union Cyc_Absyn_AggrInfoU _tmp4A2;struct Cyc_List_List*_tmp4A3;void*_tmp4A4;
struct Cyc_Absyn_AggrInfo _tmp4A6;union Cyc_Absyn_AggrInfoU _tmp4A7;struct Cyc_List_List*
_tmp4A8;void*_tmp4A9;struct _tuple2*_tmp4AB;void*_tmp4AC;struct _tuple2*_tmp4AE;
void*_tmp4AF;struct Cyc_List_List*_tmp4B1;void*_tmp4B2;struct Cyc_List_List*
_tmp4B4;void*_tmp4B5;struct Cyc_Absyn_DatatypeInfo _tmp4B7;union Cyc_Absyn_DatatypeInfoU
_tmp4B8;struct Cyc_Absyn_Datatypedecl**_tmp4B9;struct Cyc_Absyn_Datatypedecl*
_tmp4BA;struct Cyc_List_List*_tmp4BB;void*_tmp4BC;struct Cyc_Absyn_DatatypeInfo
_tmp4BE;union Cyc_Absyn_DatatypeInfoU _tmp4BF;struct Cyc_Absyn_Datatypedecl**
_tmp4C0;struct Cyc_Absyn_Datatypedecl*_tmp4C1;struct Cyc_List_List*_tmp4C2;void*
_tmp4C3;struct Cyc_Absyn_DatatypeFieldInfo _tmp4C5;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4C6;struct _tuple3 _tmp4C7;struct Cyc_Absyn_Datatypedecl*_tmp4C8;struct Cyc_Absyn_Datatypefield*
_tmp4C9;struct Cyc_List_List*_tmp4CA;void*_tmp4CB;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4CD;union Cyc_Absyn_DatatypeFieldInfoU _tmp4CE;struct _tuple3 _tmp4CF;struct Cyc_Absyn_Datatypedecl*
_tmp4D0;struct Cyc_Absyn_Datatypefield*_tmp4D1;struct Cyc_List_List*_tmp4D2;void*
_tmp4D3;struct Cyc_Absyn_PtrInfo _tmp4D5;void*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D7;
struct Cyc_Absyn_PtrAtts _tmp4D8;void*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;
union Cyc_Absyn_Constraint*_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DC;void*_tmp4DD;
struct Cyc_Absyn_PtrInfo _tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Tqual _tmp4E1;struct
Cyc_Absyn_PtrAtts _tmp4E2;void*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E4;union Cyc_Absyn_Constraint*
_tmp4E5;union Cyc_Absyn_Constraint*_tmp4E6;void*_tmp4E7;enum Cyc_Absyn_Sign
_tmp4E9;enum Cyc_Absyn_Size_of _tmp4EA;void*_tmp4EB;enum Cyc_Absyn_Sign _tmp4ED;
enum Cyc_Absyn_Size_of _tmp4EE;void*_tmp4EF;void*_tmp4F1;void*_tmp4F3;int _tmp4F5;
void*_tmp4F6;int _tmp4F8;void*_tmp4F9;void*_tmp4FB;void*_tmp4FC;void*_tmp4FE;void*
_tmp4FF;struct Cyc_Absyn_Exp*_tmp501;void*_tmp502;struct Cyc_Absyn_Exp*_tmp504;
void*_tmp505;struct Cyc_Absyn_ArrayInfo _tmp507;void*_tmp508;struct Cyc_Absyn_Tqual
_tmp509;struct Cyc_Absyn_Exp*_tmp50A;union Cyc_Absyn_Constraint*_tmp50B;void*
_tmp50C;struct Cyc_Absyn_ArrayInfo _tmp50E;void*_tmp50F;struct Cyc_Absyn_Tqual
_tmp510;struct Cyc_Absyn_Exp*_tmp511;union Cyc_Absyn_Constraint*_tmp512;void*
_tmp513;struct Cyc_Absyn_FnInfo _tmp515;struct Cyc_List_List*_tmp516;struct Cyc_Core_Opt*
_tmp517;struct Cyc_Absyn_Tqual _tmp518;void*_tmp519;struct Cyc_List_List*_tmp51A;
int _tmp51B;struct Cyc_Absyn_VarargInfo*_tmp51C;struct Cyc_List_List*_tmp51D;struct
Cyc_List_List*_tmp51E;void*_tmp51F;struct Cyc_Absyn_FnInfo _tmp521;struct Cyc_List_List*
_tmp522;struct Cyc_Core_Opt*_tmp523;struct Cyc_Absyn_Tqual _tmp524;void*_tmp525;
struct Cyc_List_List*_tmp526;int _tmp527;struct Cyc_Absyn_VarargInfo*_tmp528;struct
Cyc_List_List*_tmp529;struct Cyc_List_List*_tmp52A;void*_tmp52B;struct Cyc_List_List*
_tmp52D;void*_tmp52E;struct Cyc_List_List*_tmp530;void*_tmp531;enum Cyc_Absyn_AggrKind
_tmp533;struct Cyc_List_List*_tmp534;void*_tmp535;enum Cyc_Absyn_AggrKind _tmp537;
struct Cyc_List_List*_tmp538;void*_tmp539;void*_tmp53B;void*_tmp53D;void*_tmp53F;
void*_tmp541;void*_tmp543;void*_tmp545;void*_tmp547;void*_tmp548;void*_tmp54A;
void*_tmp54B;void*_tmp54D;void*_tmp54E;void*_tmp54F;void*_tmp551;void*_tmp552;
void*_tmp553;void*_tmp555;void*_tmp557;void*_tmp559;void*_tmp55B;void*_tmp55D;
_LL29B: _tmp493=_tmp492.f1;{struct Cyc_Absyn_Evar_struct*_tmp494=(struct Cyc_Absyn_Evar_struct*)
_tmp493;if(_tmp494->tag != 1)goto _LL29D;};_LL29C: Cyc_Tcutil_unify_it(t2,t1);
return;_LL29D: _tmp495=_tmp492.f1;{struct Cyc_Absyn_VoidType_struct*_tmp496=(
struct Cyc_Absyn_VoidType_struct*)_tmp495;if(_tmp496->tag != 0)goto _LL29F;};
_tmp497=_tmp492.f2;{struct Cyc_Absyn_VoidType_struct*_tmp498=(struct Cyc_Absyn_VoidType_struct*)
_tmp497;if(_tmp498->tag != 0)goto _LL29F;};_LL29E: return;_LL29F: _tmp499=_tmp492.f1;{
struct Cyc_Absyn_VarType_struct*_tmp49A=(struct Cyc_Absyn_VarType_struct*)_tmp499;
if(_tmp49A->tag != 2)goto _LL2A1;else{_tmp49B=_tmp49A->f1;}};_tmp49C=_tmp492.f2;{
struct Cyc_Absyn_VarType_struct*_tmp49D=(struct Cyc_Absyn_VarType_struct*)_tmp49C;
if(_tmp49D->tag != 2)goto _LL2A1;else{_tmp49E=_tmp49D->f1;}};_LL2A0: {struct
_dyneither_ptr*_tmp55F=_tmp49B->name;struct _dyneither_ptr*_tmp560=_tmp49E->name;
int _tmp561=_tmp49B->identity;int _tmp562=_tmp49E->identity;if(_tmp562 == _tmp561)
return;{const char*_tmp1271;Cyc_Tcutil_failure_reason=((_tmp1271="(variable types are not the same)",
_tag_dyneither(_tmp1271,sizeof(char),34)));}goto _LL29A;}_LL2A1: _tmp49F=_tmp492.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4A0=(struct Cyc_Absyn_AggrType_struct*)
_tmp49F;if(_tmp4A0->tag != 12)goto _LL2A3;else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A1.aggr_info;
_tmp4A3=_tmp4A1.targs;}};_tmp4A4=_tmp492.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4A5=(struct Cyc_Absyn_AggrType_struct*)_tmp4A4;if(_tmp4A5->tag != 12)goto
_LL2A3;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A6.aggr_info;_tmp4A8=_tmp4A6.targs;}};
_LL2A2: {enum Cyc_Absyn_AggrKind _tmp565;struct _tuple2*_tmp566;struct _tuple12
_tmp564=Cyc_Absyn_aggr_kinded_name(_tmp4A7);_tmp565=_tmp564.f1;_tmp566=_tmp564.f2;{
enum Cyc_Absyn_AggrKind _tmp568;struct _tuple2*_tmp569;struct _tuple12 _tmp567=Cyc_Absyn_aggr_kinded_name(
_tmp4A2);_tmp568=_tmp567.f1;_tmp569=_tmp567.f2;if(_tmp565 != _tmp568){{const char*
_tmp1272;Cyc_Tcutil_failure_reason=((_tmp1272="(struct and union type)",
_tag_dyneither(_tmp1272,sizeof(char),24)));}goto _LL29A;}if(Cyc_Absyn_qvar_cmp(
_tmp566,_tmp569)!= 0){{const char*_tmp1273;Cyc_Tcutil_failure_reason=((_tmp1273="(different type name)",
_tag_dyneither(_tmp1273,sizeof(char),22)));}goto _LL29A;}Cyc_Tcutil_unify_list(
_tmp4A8,_tmp4A3);return;};}_LL2A3: _tmp4A9=_tmp492.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp4AA=(struct Cyc_Absyn_EnumType_struct*)_tmp4A9;if(_tmp4AA->tag != 14)goto
_LL2A5;else{_tmp4AB=_tmp4AA->f1;}};_tmp4AC=_tmp492.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp4AD=(struct Cyc_Absyn_EnumType_struct*)_tmp4AC;if(_tmp4AD->tag != 14)goto
_LL2A5;else{_tmp4AE=_tmp4AD->f1;}};_LL2A4: if(Cyc_Absyn_qvar_cmp(_tmp4AB,_tmp4AE)
== 0)return;{const char*_tmp1274;Cyc_Tcutil_failure_reason=((_tmp1274="(different enum types)",
_tag_dyneither(_tmp1274,sizeof(char),23)));}goto _LL29A;_LL2A5: _tmp4AF=_tmp492.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp4B0=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4AF;if(_tmp4B0->tag != 15)goto _LL2A7;else{_tmp4B1=_tmp4B0->f1;}};_tmp4B2=
_tmp492.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4B3=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B2;if(_tmp4B3->tag != 15)goto _LL2A7;else{_tmp4B4=_tmp4B3->f1;}};_LL2A6: {int
bad=0;for(0;_tmp4B1 != 0  && _tmp4B4 != 0;(_tmp4B1=_tmp4B1->tl,_tmp4B4=_tmp4B4->tl)){
struct Cyc_Absyn_Enumfield*_tmp56D=(struct Cyc_Absyn_Enumfield*)_tmp4B1->hd;struct
Cyc_Absyn_Enumfield*_tmp56E=(struct Cyc_Absyn_Enumfield*)_tmp4B4->hd;if(Cyc_Absyn_qvar_cmp(
_tmp56D->name,_tmp56E->name)!= 0){{const char*_tmp1275;Cyc_Tcutil_failure_reason=((
_tmp1275="(different names for enum fields)",_tag_dyneither(_tmp1275,sizeof(char),
34)));}bad=1;break;}if(_tmp56D->tag == _tmp56E->tag)continue;if(_tmp56D->tag == 0
 || _tmp56E->tag == 0){{const char*_tmp1276;Cyc_Tcutil_failure_reason=((_tmp1276="(different tag values for enum fields)",
_tag_dyneither(_tmp1276,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp56D->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp56E->tag))){{const char*_tmp1277;Cyc_Tcutil_failure_reason=((_tmp1277="(different tag values for enum fields)",
_tag_dyneither(_tmp1277,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL29A;if(
_tmp4B1 == 0  && _tmp4B4 == 0)return;{const char*_tmp1278;Cyc_Tcutil_failure_reason=((
_tmp1278="(different number of fields for enums)",_tag_dyneither(_tmp1278,
sizeof(char),39)));}goto _LL29A;}_LL2A7: _tmp4B5=_tmp492.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4B6=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4B5;if(_tmp4B6->tag != 3)goto
_LL2A9;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B7.datatype_info;if((_tmp4B8.KnownDatatype).tag
!= 2)goto _LL2A9;_tmp4B9=(struct Cyc_Absyn_Datatypedecl**)(_tmp4B8.KnownDatatype).val;
_tmp4BA=*_tmp4B9;_tmp4BB=_tmp4B7.targs;}};_tmp4BC=_tmp492.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4BD=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4BC;if(_tmp4BD->tag != 3)goto
_LL2A9;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BE.datatype_info;if((_tmp4BF.KnownDatatype).tag
!= 2)goto _LL2A9;_tmp4C0=(struct Cyc_Absyn_Datatypedecl**)(_tmp4BF.KnownDatatype).val;
_tmp4C1=*_tmp4C0;_tmp4C2=_tmp4BE.targs;}};_LL2A8: if(_tmp4BA == _tmp4C1  || Cyc_Absyn_qvar_cmp(
_tmp4BA->name,_tmp4C1->name)== 0){Cyc_Tcutil_unify_list(_tmp4C2,_tmp4BB);return;}
goto _LL29A;_LL2A9: _tmp4C3=_tmp492.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4C4=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4C3;if(_tmp4C4->tag != 4)
goto _LL2AB;else{_tmp4C5=_tmp4C4->f1;_tmp4C6=_tmp4C5.field_info;if((_tmp4C6.KnownDatatypefield).tag
!= 2)goto _LL2AB;_tmp4C7=(struct _tuple3)(_tmp4C6.KnownDatatypefield).val;_tmp4C8=
_tmp4C7.f1;_tmp4C9=_tmp4C7.f2;_tmp4CA=_tmp4C5.targs;}};_tmp4CB=_tmp492.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4CC=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4CB;if(_tmp4CC->tag != 4)goto _LL2AB;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CD.field_info;
if((_tmp4CE.KnownDatatypefield).tag != 2)goto _LL2AB;_tmp4CF=(struct _tuple3)(
_tmp4CE.KnownDatatypefield).val;_tmp4D0=_tmp4CF.f1;_tmp4D1=_tmp4CF.f2;_tmp4D2=
_tmp4CD.targs;}};_LL2AA: if((_tmp4C8 == _tmp4D0  || Cyc_Absyn_qvar_cmp(_tmp4C8->name,
_tmp4D0->name)== 0) && (_tmp4C9 == _tmp4D1  || Cyc_Absyn_qvar_cmp(_tmp4C9->name,
_tmp4D1->name)== 0)){Cyc_Tcutil_unify_list(_tmp4D2,_tmp4CA);return;}{const char*
_tmp1279;Cyc_Tcutil_failure_reason=((_tmp1279="(different datatype field types)",
_tag_dyneither(_tmp1279,sizeof(char),33)));}goto _LL29A;_LL2AB: _tmp4D3=_tmp492.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4D4=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D3;if(_tmp4D4->tag != 5)goto _LL2AD;else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D5.elt_typ;
_tmp4D7=_tmp4D5.elt_tq;_tmp4D8=_tmp4D5.ptr_atts;_tmp4D9=_tmp4D8.rgn;_tmp4DA=
_tmp4D8.nullable;_tmp4DB=_tmp4D8.bounds;_tmp4DC=_tmp4D8.zero_term;}};_tmp4DD=
_tmp492.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4DE=(struct Cyc_Absyn_PointerType_struct*)
_tmp4DD;if(_tmp4DE->tag != 5)goto _LL2AD;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DF.elt_typ;
_tmp4E1=_tmp4DF.elt_tq;_tmp4E2=_tmp4DF.ptr_atts;_tmp4E3=_tmp4E2.rgn;_tmp4E4=
_tmp4E2.nullable;_tmp4E5=_tmp4E2.bounds;_tmp4E6=_tmp4E2.zero_term;}};_LL2AC: Cyc_Tcutil_unify_it(
_tmp4E0,_tmp4D6);Cyc_Tcutil_unify_it(_tmp4D9,_tmp4E3);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp127A;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4E6,_tmp4DC,((_tmp127A="(not both zero terminated)",
_tag_dyneither(_tmp127A,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4E1,
_tmp4E0,_tmp4D7,_tmp4D6);{const char*_tmp127B;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4E5,_tmp4DB,((
_tmp127B="(different pointer bounds)",_tag_dyneither(_tmp127B,sizeof(char),27))));}{
void*_tmp576=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4E5);_LL2D8: {struct Cyc_Absyn_DynEither_b_struct*
_tmp577=(struct Cyc_Absyn_DynEither_b_struct*)_tmp576;if(_tmp577->tag != 0)goto
_LL2DA;}_LL2D9: return;_LL2DA:;_LL2DB: goto _LL2D7;_LL2D7:;}{const char*_tmp127C;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4E4,_tmp4DA,((_tmp127C="(incompatible pointer types)",_tag_dyneither(
_tmp127C,sizeof(char),29))));}return;_LL2AD: _tmp4E7=_tmp492.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4E8=(struct Cyc_Absyn_IntType_struct*)_tmp4E7;if(_tmp4E8->tag != 6)goto _LL2AF;
else{_tmp4E9=_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;}};_tmp4EB=_tmp492.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4EC=(struct Cyc_Absyn_IntType_struct*)_tmp4EB;if(_tmp4EC->tag != 6)goto _LL2AF;
else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;}};_LL2AE: if(_tmp4ED == _tmp4E9  && ((
_tmp4EE == _tmp4EA  || _tmp4EE == Cyc_Absyn_Int_sz  && _tmp4EA == Cyc_Absyn_Long_sz)
 || _tmp4EE == Cyc_Absyn_Long_sz  && _tmp4EA == Cyc_Absyn_Int_sz))return;{const char*
_tmp127D;Cyc_Tcutil_failure_reason=((_tmp127D="(different integral types)",
_tag_dyneither(_tmp127D,sizeof(char),27)));}goto _LL29A;_LL2AF: _tmp4EF=_tmp492.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4F0=(struct Cyc_Absyn_FloatType_struct*)
_tmp4EF;if(_tmp4F0->tag != 7)goto _LL2B1;};_tmp4F1=_tmp492.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp4F2=(struct Cyc_Absyn_FloatType_struct*)_tmp4F1;if(_tmp4F2->tag != 7)goto
_LL2B1;};_LL2B0: return;_LL2B1: _tmp4F3=_tmp492.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp4F4=(struct Cyc_Absyn_DoubleType_struct*)_tmp4F3;if(_tmp4F4->tag != 8)goto
_LL2B3;else{_tmp4F5=_tmp4F4->f1;}};_tmp4F6=_tmp492.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp4F7=(struct Cyc_Absyn_DoubleType_struct*)_tmp4F6;if(_tmp4F7->tag != 8)goto
_LL2B3;else{_tmp4F8=_tmp4F7->f1;}};_LL2B2: if(_tmp4F5 == _tmp4F8)return;goto _LL29A;
_LL2B3: _tmp4F9=_tmp492.f1;{struct Cyc_Absyn_TagType_struct*_tmp4FA=(struct Cyc_Absyn_TagType_struct*)
_tmp4F9;if(_tmp4FA->tag != 20)goto _LL2B5;else{_tmp4FB=(void*)_tmp4FA->f1;}};
_tmp4FC=_tmp492.f2;{struct Cyc_Absyn_TagType_struct*_tmp4FD=(struct Cyc_Absyn_TagType_struct*)
_tmp4FC;if(_tmp4FD->tag != 20)goto _LL2B5;else{_tmp4FE=(void*)_tmp4FD->f1;}};
_LL2B4: Cyc_Tcutil_unify_it(_tmp4FB,_tmp4FE);return;_LL2B5: _tmp4FF=_tmp492.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp500=(struct Cyc_Absyn_ValueofType_struct*)
_tmp4FF;if(_tmp500->tag != 19)goto _LL2B7;else{_tmp501=_tmp500->f1;}};_tmp502=
_tmp492.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp503=(struct Cyc_Absyn_ValueofType_struct*)
_tmp502;if(_tmp503->tag != 19)goto _LL2B7;else{_tmp504=_tmp503->f1;}};_LL2B6: if(!
Cyc_Evexp_same_const_exp(_tmp501,_tmp504)){{const char*_tmp127E;Cyc_Tcutil_failure_reason=((
_tmp127E="(cannot prove expressions are the same)",_tag_dyneither(_tmp127E,
sizeof(char),40)));}goto _LL29A;}return;_LL2B7: _tmp505=_tmp492.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp506=(struct Cyc_Absyn_ArrayType_struct*)_tmp505;if(_tmp506->tag != 9)goto
_LL2B9;else{_tmp507=_tmp506->f1;_tmp508=_tmp507.elt_type;_tmp509=_tmp507.tq;
_tmp50A=_tmp507.num_elts;_tmp50B=_tmp507.zero_term;}};_tmp50C=_tmp492.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp50D=(struct Cyc_Absyn_ArrayType_struct*)_tmp50C;
if(_tmp50D->tag != 9)goto _LL2B9;else{_tmp50E=_tmp50D->f1;_tmp50F=_tmp50E.elt_type;
_tmp510=_tmp50E.tq;_tmp511=_tmp50E.num_elts;_tmp512=_tmp50E.zero_term;}};_LL2B8:
Cyc_Tcutil_unify_it(_tmp50F,_tmp508);Cyc_Tcutil_unify_tqual(_tmp510,_tmp50F,
_tmp509,_tmp508);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp127F;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp50B,_tmp512,((_tmp127F="(not both zero terminated)",_tag_dyneither(_tmp127F,
sizeof(char),27))));}if(_tmp50A == _tmp511)return;if(_tmp50A == 0  || _tmp511 == 0)
goto _LL29A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp50A,(struct Cyc_Absyn_Exp*)
_tmp511))return;{const char*_tmp1280;Cyc_Tcutil_failure_reason=((_tmp1280="(different array sizes)",
_tag_dyneither(_tmp1280,sizeof(char),24)));}goto _LL29A;_LL2B9: _tmp513=_tmp492.f1;{
struct Cyc_Absyn_FnType_struct*_tmp514=(struct Cyc_Absyn_FnType_struct*)_tmp513;
if(_tmp514->tag != 10)goto _LL2BB;else{_tmp515=_tmp514->f1;_tmp516=_tmp515.tvars;
_tmp517=_tmp515.effect;_tmp518=_tmp515.ret_tqual;_tmp519=_tmp515.ret_typ;_tmp51A=
_tmp515.args;_tmp51B=_tmp515.c_varargs;_tmp51C=_tmp515.cyc_varargs;_tmp51D=
_tmp515.rgn_po;_tmp51E=_tmp515.attributes;}};_tmp51F=_tmp492.f2;{struct Cyc_Absyn_FnType_struct*
_tmp520=(struct Cyc_Absyn_FnType_struct*)_tmp51F;if(_tmp520->tag != 10)goto _LL2BB;
else{_tmp521=_tmp520->f1;_tmp522=_tmp521.tvars;_tmp523=_tmp521.effect;_tmp524=
_tmp521.ret_tqual;_tmp525=_tmp521.ret_typ;_tmp526=_tmp521.args;_tmp527=_tmp521.c_varargs;
_tmp528=_tmp521.cyc_varargs;_tmp529=_tmp521.rgn_po;_tmp52A=_tmp521.attributes;}};
_LL2BA: {int done=0;struct _RegionHandle _tmp57D=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp57D;_push_region(rgn);{struct Cyc_List_List*inst=0;while(
_tmp522 != 0){if(_tmp516 == 0){{const char*_tmp1281;Cyc_Tcutil_failure_reason=((
_tmp1281="(second function type has too few type variables)",_tag_dyneither(
_tmp1281,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp57F=((struct Cyc_Absyn_Tvar*)_tmp522->hd)->kind;void*_tmp580=((struct Cyc_Absyn_Tvar*)
_tmp516->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp57F,_tmp580)){{const char*
_tmp1287;void*_tmp1286[3];struct Cyc_String_pa_struct _tmp1285;struct Cyc_String_pa_struct
_tmp1284;struct Cyc_String_pa_struct _tmp1283;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1283.tag=0,((_tmp1283.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp516->hd,& Cyc_Tcutil_bk))),((_tmp1284.tag=0,((_tmp1284.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp522->hd,& Cyc_Tcutil_bk))),((_tmp1285.tag=0,((_tmp1285.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp522->hd)),((_tmp1286[0]=& _tmp1285,((_tmp1286[1]=& _tmp1284,((_tmp1286[2]=&
_tmp1283,Cyc_aprintf(((_tmp1287="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1287,sizeof(char),44))),_tag_dyneither(_tmp1286,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple15*_tmp1291;struct Cyc_Absyn_VarType_struct _tmp1290;struct Cyc_Absyn_VarType_struct*
_tmp128F;struct Cyc_List_List*_tmp128E;inst=((_tmp128E=_region_malloc(rgn,sizeof(*
_tmp128E)),((_tmp128E->hd=((_tmp1291=_region_malloc(rgn,sizeof(*_tmp1291)),((
_tmp1291->f1=(struct Cyc_Absyn_Tvar*)_tmp516->hd,((_tmp1291->f2=(void*)((_tmp128F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp128F)),((_tmp128F[0]=((
_tmp1290.tag=2,((_tmp1290.f1=(struct Cyc_Absyn_Tvar*)_tmp522->hd,_tmp1290)))),
_tmp128F)))),_tmp1291)))))),((_tmp128E->tl=inst,_tmp128E))))));}_tmp522=_tmp522->tl;
_tmp516=_tmp516->tl;};}if(_tmp516 != 0){{const char*_tmp1292;Cyc_Tcutil_failure_reason=((
_tmp1292="(second function type has too many type variables)",_tag_dyneither(
_tmp1292,sizeof(char),51)));}_npop_handler(0);goto _LL29A;}if(inst != 0){{struct
Cyc_Absyn_FnType_struct _tmp129E;struct Cyc_Absyn_FnInfo _tmp129D;struct Cyc_Absyn_FnType_struct*
_tmp129C;struct Cyc_Absyn_FnType_struct _tmp1298;struct Cyc_Absyn_FnInfo _tmp1297;
struct Cyc_Absyn_FnType_struct*_tmp1296;Cyc_Tcutil_unify_it((void*)((_tmp1296=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1296)),((_tmp1296[0]=((
_tmp1298.tag=10,((_tmp1298.f1=((_tmp1297.tvars=0,((_tmp1297.effect=_tmp523,((
_tmp1297.ret_tqual=_tmp524,((_tmp1297.ret_typ=_tmp525,((_tmp1297.args=_tmp526,((
_tmp1297.c_varargs=_tmp527,((_tmp1297.cyc_varargs=_tmp528,((_tmp1297.rgn_po=
_tmp529,((_tmp1297.attributes=_tmp52A,_tmp1297)))))))))))))))))),_tmp1298)))),
_tmp1296)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp129C=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp129C)),((_tmp129C[0]=((_tmp129E.tag=10,((
_tmp129E.f1=((_tmp129D.tvars=0,((_tmp129D.effect=_tmp517,((_tmp129D.ret_tqual=
_tmp518,((_tmp129D.ret_typ=_tmp519,((_tmp129D.args=_tmp51A,((_tmp129D.c_varargs=
_tmp51B,((_tmp129D.cyc_varargs=_tmp51C,((_tmp129D.rgn_po=_tmp51D,((_tmp129D.attributes=
_tmp51E,_tmp129D)))))))))))))))))),_tmp129E)))),_tmp129C))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp525,_tmp519);Cyc_Tcutil_unify_tqual(
_tmp524,_tmp525,_tmp518,_tmp519);for(0;_tmp526 != 0  && _tmp51A != 0;(_tmp526=
_tmp526->tl,_tmp51A=_tmp51A->tl)){struct Cyc_Absyn_Tqual _tmp592;void*_tmp593;
struct _tuple9 _tmp591=*((struct _tuple9*)_tmp526->hd);_tmp592=_tmp591.f2;_tmp593=
_tmp591.f3;{struct Cyc_Absyn_Tqual _tmp595;void*_tmp596;struct _tuple9 _tmp594=*((
struct _tuple9*)_tmp51A->hd);_tmp595=_tmp594.f2;_tmp596=_tmp594.f3;Cyc_Tcutil_unify_it(
_tmp593,_tmp596);Cyc_Tcutil_unify_tqual(_tmp592,_tmp593,_tmp595,_tmp596);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp526 != 0  || _tmp51A != 0){{const char*_tmp129F;
Cyc_Tcutil_failure_reason=((_tmp129F="(function types have different number of arguments)",
_tag_dyneither(_tmp129F,sizeof(char),52)));}_npop_handler(0);goto _LL29A;}if(
_tmp527 != _tmp51B){{const char*_tmp12A0;Cyc_Tcutil_failure_reason=((_tmp12A0="(only one function type takes C varargs)",
_tag_dyneither(_tmp12A0,sizeof(char),41)));}_npop_handler(0);goto _LL29A;}{int
bad_cyc_vararg=0;{struct _tuple21 _tmp12A1;struct _tuple21 _tmp59A=(_tmp12A1.f1=
_tmp528,((_tmp12A1.f2=_tmp51C,_tmp12A1)));struct Cyc_Absyn_VarargInfo*_tmp59B;
struct Cyc_Absyn_VarargInfo*_tmp59C;struct Cyc_Absyn_VarargInfo*_tmp59D;struct Cyc_Absyn_VarargInfo*
_tmp59E;struct Cyc_Absyn_VarargInfo*_tmp59F;struct Cyc_Absyn_VarargInfo _tmp5A0;
struct Cyc_Core_Opt*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A3;int _tmp5A4;
struct Cyc_Absyn_VarargInfo*_tmp5A5;struct Cyc_Absyn_VarargInfo _tmp5A6;struct Cyc_Core_Opt*
_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A9;int _tmp5AA;_LL2DD: _tmp59B=
_tmp59A.f1;if(_tmp59B != 0)goto _LL2DF;_tmp59C=_tmp59A.f2;if(_tmp59C != 0)goto
_LL2DF;_LL2DE: goto _LL2DC;_LL2DF: _tmp59D=_tmp59A.f1;if(_tmp59D != 0)goto _LL2E1;
_LL2E0: goto _LL2E2;_LL2E1: _tmp59E=_tmp59A.f2;if(_tmp59E != 0)goto _LL2E3;_LL2E2:
bad_cyc_vararg=1;{const char*_tmp12A2;Cyc_Tcutil_failure_reason=((_tmp12A2="(only one function type takes varargs)",
_tag_dyneither(_tmp12A2,sizeof(char),39)));}goto _LL2DC;_LL2E3: _tmp59F=_tmp59A.f1;
if(_tmp59F == 0)goto _LL2DC;_tmp5A0=*_tmp59F;_tmp5A1=_tmp5A0.name;_tmp5A2=_tmp5A0.tq;
_tmp5A3=_tmp5A0.type;_tmp5A4=_tmp5A0.inject;_tmp5A5=_tmp59A.f2;if(_tmp5A5 == 0)
goto _LL2DC;_tmp5A6=*_tmp5A5;_tmp5A7=_tmp5A6.name;_tmp5A8=_tmp5A6.tq;_tmp5A9=
_tmp5A6.type;_tmp5AA=_tmp5A6.inject;_LL2E4: Cyc_Tcutil_unify_it(_tmp5A3,_tmp5A9);
Cyc_Tcutil_unify_tqual(_tmp5A2,_tmp5A3,_tmp5A8,_tmp5A9);if(_tmp5A4 != _tmp5AA){
bad_cyc_vararg=1;{const char*_tmp12A3;Cyc_Tcutil_failure_reason=((_tmp12A3="(only one function type injects varargs)",
_tag_dyneither(_tmp12A3,sizeof(char),41)));};}goto _LL2DC;_LL2DC:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL29A;}{int bad_effect=0;{struct _tuple22
_tmp12A4;struct _tuple22 _tmp5AE=(_tmp12A4.f1=_tmp523,((_tmp12A4.f2=_tmp517,
_tmp12A4)));struct Cyc_Core_Opt*_tmp5AF;struct Cyc_Core_Opt*_tmp5B0;struct Cyc_Core_Opt*
_tmp5B1;struct Cyc_Core_Opt*_tmp5B2;_LL2E6: _tmp5AF=_tmp5AE.f1;if(_tmp5AF != 0)goto
_LL2E8;_tmp5B0=_tmp5AE.f2;if(_tmp5B0 != 0)goto _LL2E8;_LL2E7: goto _LL2E5;_LL2E8:
_tmp5B1=_tmp5AE.f1;if(_tmp5B1 != 0)goto _LL2EA;_LL2E9: goto _LL2EB;_LL2EA: _tmp5B2=
_tmp5AE.f2;if(_tmp5B2 != 0)goto _LL2EC;_LL2EB: bad_effect=1;goto _LL2E5;_LL2EC:;
_LL2ED: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp523))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp517))->v);
goto _LL2E5;_LL2E5:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp12A5;Cyc_Tcutil_failure_reason=((_tmp12A5="(function type effects do not unify)",
_tag_dyneither(_tmp12A5,sizeof(char),37)));}_npop_handler(0);goto _LL29A;}if(!Cyc_Tcutil_same_atts(
_tmp51E,_tmp52A)){{const char*_tmp12A6;Cyc_Tcutil_failure_reason=((_tmp12A6="(function types have different attributes)",
_tag_dyneither(_tmp12A6,sizeof(char),43)));}_npop_handler(0);goto _LL29A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp51D,_tmp529)){{const char*_tmp12A7;Cyc_Tcutil_failure_reason=((_tmp12A7="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp12A7,sizeof(char),58)));}_npop_handler(0);goto _LL29A;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2BB: _tmp52B=_tmp492.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp52C=(struct Cyc_Absyn_TupleType_struct*)_tmp52B;
if(_tmp52C->tag != 11)goto _LL2BD;else{_tmp52D=_tmp52C->f1;}};_tmp52E=_tmp492.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp52F=(struct Cyc_Absyn_TupleType_struct*)
_tmp52E;if(_tmp52F->tag != 11)goto _LL2BD;else{_tmp530=_tmp52F->f1;}};_LL2BC: for(0;
_tmp530 != 0  && _tmp52D != 0;(_tmp530=_tmp530->tl,_tmp52D=_tmp52D->tl)){struct Cyc_Absyn_Tqual
_tmp5B7;void*_tmp5B8;struct _tuple11 _tmp5B6=*((struct _tuple11*)_tmp530->hd);
_tmp5B7=_tmp5B6.f1;_tmp5B8=_tmp5B6.f2;{struct Cyc_Absyn_Tqual _tmp5BA;void*_tmp5BB;
struct _tuple11 _tmp5B9=*((struct _tuple11*)_tmp52D->hd);_tmp5BA=_tmp5B9.f1;_tmp5BB=
_tmp5B9.f2;Cyc_Tcutil_unify_it(_tmp5B8,_tmp5BB);Cyc_Tcutil_unify_tqual(_tmp5B7,
_tmp5B8,_tmp5BA,_tmp5BB);};}if(_tmp530 == 0  && _tmp52D == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp12A8;Cyc_Tcutil_failure_reason=((
_tmp12A8="(tuple types have different numbers of components)",_tag_dyneither(
_tmp12A8,sizeof(char),51)));}goto _LL29A;_LL2BD: _tmp531=_tmp492.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp532=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp531;if(_tmp532->tag != 13)goto
_LL2BF;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}};_tmp535=_tmp492.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp536=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp535;if(_tmp536->tag != 13)goto _LL2BF;else{_tmp537=_tmp536->f1;_tmp538=_tmp536->f2;}};
_LL2BE: if(_tmp537 != _tmp533){{const char*_tmp12A9;Cyc_Tcutil_failure_reason=((
_tmp12A9="(struct and union type)",_tag_dyneither(_tmp12A9,sizeof(char),24)));}
goto _LL29A;}for(0;_tmp538 != 0  && _tmp534 != 0;(_tmp538=_tmp538->tl,_tmp534=
_tmp534->tl)){struct Cyc_Absyn_Aggrfield*_tmp5BE=(struct Cyc_Absyn_Aggrfield*)
_tmp538->hd;struct Cyc_Absyn_Aggrfield*_tmp5BF=(struct Cyc_Absyn_Aggrfield*)
_tmp534->hd;if(Cyc_strptrcmp(_tmp5BE->name,_tmp5BF->name)!= 0){{const char*
_tmp12AA;Cyc_Tcutil_failure_reason=((_tmp12AA="(different member names)",
_tag_dyneither(_tmp12AA,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5BE->type,_tmp5BF->type);Cyc_Tcutil_unify_tqual(_tmp5BE->tq,
_tmp5BE->type,_tmp5BF->tq,_tmp5BF->type);if(!Cyc_Tcutil_same_atts(_tmp5BE->attributes,
_tmp5BF->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp12AB;Cyc_Tcutil_failure_reason=((_tmp12AB="(different attributes on member)",
_tag_dyneither(_tmp12AB,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5BE->width != 0  && _tmp5BF->width == 0  || _tmp5BF->width != 0  && _tmp5BE->width
== 0) || (_tmp5BE->width != 0  && _tmp5BF->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5BE->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5BF->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp12AC;Cyc_Tcutil_failure_reason=((_tmp12AC="(different bitfield widths on member)",
_tag_dyneither(_tmp12AC,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp538 == 0  && _tmp534 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp12AD;Cyc_Tcutil_failure_reason=((_tmp12AD="(different number of members)",
_tag_dyneither(_tmp12AD,sizeof(char),30)));}goto _LL29A;_LL2BF: _tmp539=_tmp492.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp53A=(struct Cyc_Absyn_HeapRgn_struct*)_tmp539;
if(_tmp53A->tag != 21)goto _LL2C1;};_tmp53B=_tmp492.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp53C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp53B;if(_tmp53C->tag != 21)goto _LL2C1;};
_LL2C0: return;_LL2C1: _tmp53D=_tmp492.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp53E=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp53D;if(_tmp53E->tag != 22)goto
_LL2C3;};_tmp53F=_tmp492.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp540=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp53F;if(_tmp540->tag != 22)goto _LL2C3;};_LL2C2: return;_LL2C3: _tmp541=_tmp492.f1;{
struct Cyc_Absyn_RefCntRgn_struct*_tmp542=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp541;if(_tmp542->tag != 23)goto _LL2C5;};_tmp543=_tmp492.f2;{struct Cyc_Absyn_RefCntRgn_struct*
_tmp544=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp543;if(_tmp544->tag != 23)goto
_LL2C5;};_LL2C4: return;_LL2C5: _tmp545=_tmp492.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp546=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp545;if(_tmp546->tag != 16)goto
_LL2C7;else{_tmp547=(void*)_tmp546->f1;}};_tmp548=_tmp492.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp549=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp548;if(_tmp549->tag != 16)goto
_LL2C7;else{_tmp54A=(void*)_tmp549->f1;}};_LL2C6: Cyc_Tcutil_unify_it(_tmp547,
_tmp54A);return;_LL2C7: _tmp54B=_tmp492.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp54C=(struct Cyc_Absyn_DynRgnType_struct*)_tmp54B;if(_tmp54C->tag != 17)goto
_LL2C9;else{_tmp54D=(void*)_tmp54C->f1;_tmp54E=(void*)_tmp54C->f2;}};_tmp54F=
_tmp492.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp550=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp54F;if(_tmp550->tag != 17)goto _LL2C9;else{_tmp551=(void*)_tmp550->f1;_tmp552=(
void*)_tmp550->f2;}};_LL2C8: Cyc_Tcutil_unify_it(_tmp54D,_tmp551);Cyc_Tcutil_unify_it(
_tmp54E,_tmp552);return;_LL2C9: _tmp553=_tmp492.f1;{struct Cyc_Absyn_JoinEff_struct*
_tmp554=(struct Cyc_Absyn_JoinEff_struct*)_tmp553;if(_tmp554->tag != 25)goto _LL2CB;};
_LL2CA: goto _LL2CC;_LL2CB: _tmp555=_tmp492.f2;{struct Cyc_Absyn_JoinEff_struct*
_tmp556=(struct Cyc_Absyn_JoinEff_struct*)_tmp555;if(_tmp556->tag != 25)goto _LL2CD;};
_LL2CC: goto _LL2CE;_LL2CD: _tmp557=_tmp492.f1;{struct Cyc_Absyn_AccessEff_struct*
_tmp558=(struct Cyc_Absyn_AccessEff_struct*)_tmp557;if(_tmp558->tag != 24)goto
_LL2CF;};_LL2CE: goto _LL2D0;_LL2CF: _tmp559=_tmp492.f1;{struct Cyc_Absyn_RgnsEff_struct*
_tmp55A=(struct Cyc_Absyn_RgnsEff_struct*)_tmp559;if(_tmp55A->tag != 26)goto _LL2D1;};
_LL2D0: goto _LL2D2;_LL2D1: _tmp55B=_tmp492.f2;{struct Cyc_Absyn_RgnsEff_struct*
_tmp55C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp55B;if(_tmp55C->tag != 26)goto _LL2D3;};
_LL2D2: goto _LL2D4;_LL2D3: _tmp55D=_tmp492.f2;{struct Cyc_Absyn_AccessEff_struct*
_tmp55E=(struct Cyc_Absyn_AccessEff_struct*)_tmp55D;if(_tmp55E->tag != 24)goto
_LL2D5;};_LL2D4: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmp12AE;Cyc_Tcutil_failure_reason=((
_tmp12AE="(effects don't unify)",_tag_dyneither(_tmp12AE,sizeof(char),22)));}
goto _LL29A;_LL2D5:;_LL2D6: goto _LL29A;_LL29A:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return
0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)
_check_null(a1),(void*)_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){int _tmp5C5=(tq1.real_const + (tq1.q_volatile << 1))+ (
tq1.q_restrict << 2);int _tmp5C6=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict
<< 2);return Cyc_Core_intcmp(_tmp5C5,_tmp5C6);}static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp5C7=x;union Cyc_Absyn_Constraint _tmp5C8;int _tmp5C9;
union Cyc_Absyn_Constraint _tmp5CA;void*_tmp5CB;union Cyc_Absyn_Constraint _tmp5CC;
union Cyc_Absyn_Constraint*_tmp5CD;_LL2EF: _tmp5C8=*_tmp5C7;if((_tmp5C8.No_constr).tag
!= 3)goto _LL2F1;_tmp5C9=(int)(_tmp5C8.No_constr).val;_LL2F0: return - 1;_LL2F1:
_tmp5CA=*_tmp5C7;if((_tmp5CA.Eq_constr).tag != 1)goto _LL2F3;_tmp5CB=(void*)(
_tmp5CA.Eq_constr).val;_LL2F2: {union Cyc_Absyn_Constraint*_tmp5CE=y;union Cyc_Absyn_Constraint
_tmp5CF;int _tmp5D0;union Cyc_Absyn_Constraint _tmp5D1;void*_tmp5D2;union Cyc_Absyn_Constraint
_tmp5D3;union Cyc_Absyn_Constraint*_tmp5D4;_LL2F6: _tmp5CF=*_tmp5CE;if((_tmp5CF.No_constr).tag
!= 3)goto _LL2F8;_tmp5D0=(int)(_tmp5CF.No_constr).val;_LL2F7: return 1;_LL2F8:
_tmp5D1=*_tmp5CE;if((_tmp5D1.Eq_constr).tag != 1)goto _LL2FA;_tmp5D2=(void*)(
_tmp5D1.Eq_constr).val;_LL2F9: return cmp(_tmp5CB,_tmp5D2);_LL2FA: _tmp5D3=*_tmp5CE;
if((_tmp5D3.Forward_constr).tag != 2)goto _LL2F5;_tmp5D4=(union Cyc_Absyn_Constraint*)(
_tmp5D3.Forward_constr).val;_LL2FB: {const char*_tmp12B1;void*_tmp12B0;(_tmp12B0=
0,Cyc_Tcutil_impos(((_tmp12B1="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp12B1,sizeof(char),40))),_tag_dyneither(_tmp12B0,sizeof(void*),
0)));}_LL2F5:;}_LL2F3: _tmp5CC=*_tmp5C7;if((_tmp5CC.Forward_constr).tag != 2)goto
_LL2EE;_tmp5CD=(union Cyc_Absyn_Constraint*)(_tmp5CC.Forward_constr).val;_LL2F4: {
const char*_tmp12B4;void*_tmp12B3;(_tmp12B3=0,Cyc_Tcutil_impos(((_tmp12B4="unify_conref: forward after compress",
_tag_dyneither(_tmp12B4,sizeof(char),37))),_tag_dyneither(_tmp12B3,sizeof(void*),
0)));}_LL2EE:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp5DA;struct Cyc_Absyn_Tqual _tmp5DB;void*_tmp5DC;
struct _tuple11*_tmp5D9=tqt1;_tmp5DA=*_tmp5D9;_tmp5DB=_tmp5DA.f1;_tmp5DC=_tmp5DA.f2;{
struct _tuple11 _tmp5DE;struct Cyc_Absyn_Tqual _tmp5DF;void*_tmp5E0;struct _tuple11*
_tmp5DD=tqt2;_tmp5DE=*_tmp5DD;_tmp5DF=_tmp5DE.f1;_tmp5E0=_tmp5DE.f2;{int _tmp5E1=
Cyc_Tcutil_tqual_cmp(_tmp5DB,_tmp5DF);if(_tmp5E1 != 0)return _tmp5E1;return Cyc_Tcutil_typecmp(
_tmp5DC,_tmp5E0);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5E2=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5E2 != 0)return _tmp5E2;{int _tmp5E3=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5E3 != 0)return _tmp5E3;{int _tmp5E4=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5E4 != 0)return _tmp5E4;{int _tmp5E5=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5E5 != 0)return _tmp5E5;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5E6=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5E6 != 0)return _tmp5E6;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5E7=t;_LL2FD: {struct Cyc_Absyn_VoidType_struct*_tmp5E8=(struct
Cyc_Absyn_VoidType_struct*)_tmp5E7;if(_tmp5E8->tag != 0)goto _LL2FF;}_LL2FE: return
0;_LL2FF: {struct Cyc_Absyn_Evar_struct*_tmp5E9=(struct Cyc_Absyn_Evar_struct*)
_tmp5E7;if(_tmp5E9->tag != 1)goto _LL301;}_LL300: return 1;_LL301: {struct Cyc_Absyn_VarType_struct*
_tmp5EA=(struct Cyc_Absyn_VarType_struct*)_tmp5E7;if(_tmp5EA->tag != 2)goto _LL303;}
_LL302: return 2;_LL303: {struct Cyc_Absyn_DatatypeType_struct*_tmp5EB=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5E7;if(_tmp5EB->tag != 3)goto _LL305;}_LL304: return 3;_LL305: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5EC=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5E7;if(_tmp5EC->tag != 4)
goto _LL307;}_LL306: return 4;_LL307: {struct Cyc_Absyn_PointerType_struct*_tmp5ED=(
struct Cyc_Absyn_PointerType_struct*)_tmp5E7;if(_tmp5ED->tag != 5)goto _LL309;}
_LL308: return 5;_LL309: {struct Cyc_Absyn_IntType_struct*_tmp5EE=(struct Cyc_Absyn_IntType_struct*)
_tmp5E7;if(_tmp5EE->tag != 6)goto _LL30B;}_LL30A: return 6;_LL30B: {struct Cyc_Absyn_FloatType_struct*
_tmp5EF=(struct Cyc_Absyn_FloatType_struct*)_tmp5E7;if(_tmp5EF->tag != 7)goto
_LL30D;}_LL30C: return 7;_LL30D: {struct Cyc_Absyn_DoubleType_struct*_tmp5F0=(
struct Cyc_Absyn_DoubleType_struct*)_tmp5E7;if(_tmp5F0->tag != 8)goto _LL30F;}
_LL30E: return 8;_LL30F: {struct Cyc_Absyn_ArrayType_struct*_tmp5F1=(struct Cyc_Absyn_ArrayType_struct*)
_tmp5E7;if(_tmp5F1->tag != 9)goto _LL311;}_LL310: return 9;_LL311: {struct Cyc_Absyn_FnType_struct*
_tmp5F2=(struct Cyc_Absyn_FnType_struct*)_tmp5E7;if(_tmp5F2->tag != 10)goto _LL313;}
_LL312: return 10;_LL313: {struct Cyc_Absyn_TupleType_struct*_tmp5F3=(struct Cyc_Absyn_TupleType_struct*)
_tmp5E7;if(_tmp5F3->tag != 11)goto _LL315;}_LL314: return 11;_LL315: {struct Cyc_Absyn_AggrType_struct*
_tmp5F4=(struct Cyc_Absyn_AggrType_struct*)_tmp5E7;if(_tmp5F4->tag != 12)goto
_LL317;}_LL316: return 12;_LL317: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5F5=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp5E7;if(_tmp5F5->tag != 13)goto _LL319;}
_LL318: return 14;_LL319: {struct Cyc_Absyn_EnumType_struct*_tmp5F6=(struct Cyc_Absyn_EnumType_struct*)
_tmp5E7;if(_tmp5F6->tag != 14)goto _LL31B;}_LL31A: return 16;_LL31B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp5F7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp5E7;if(_tmp5F7->tag != 15)goto
_LL31D;}_LL31C: return 17;_LL31D: {struct Cyc_Absyn_RgnHandleType_struct*_tmp5F8=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5E7;if(_tmp5F8->tag != 16)goto _LL31F;}
_LL31E: return 18;_LL31F: {struct Cyc_Absyn_TypedefType_struct*_tmp5F9=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5E7;if(_tmp5F9->tag != 18)goto _LL321;}_LL320: return 19;_LL321: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp5FA=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp5E7;if(_tmp5FA->tag != 22)goto
_LL323;}_LL322: return 20;_LL323: {struct Cyc_Absyn_HeapRgn_struct*_tmp5FB=(struct
Cyc_Absyn_HeapRgn_struct*)_tmp5E7;if(_tmp5FB->tag != 21)goto _LL325;}_LL324: return
21;_LL325: {struct Cyc_Absyn_AccessEff_struct*_tmp5FC=(struct Cyc_Absyn_AccessEff_struct*)
_tmp5E7;if(_tmp5FC->tag != 24)goto _LL327;}_LL326: return 22;_LL327: {struct Cyc_Absyn_JoinEff_struct*
_tmp5FD=(struct Cyc_Absyn_JoinEff_struct*)_tmp5E7;if(_tmp5FD->tag != 25)goto _LL329;}
_LL328: return 23;_LL329: {struct Cyc_Absyn_RgnsEff_struct*_tmp5FE=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5E7;if(_tmp5FE->tag != 26)goto _LL32B;}_LL32A: return 24;_LL32B: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp5FF=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp5E7;if(_tmp5FF->tag != 23)goto
_LL32D;}_LL32C: return 25;_LL32D: {struct Cyc_Absyn_TagType_struct*_tmp600=(struct
Cyc_Absyn_TagType_struct*)_tmp5E7;if(_tmp600->tag != 20)goto _LL32F;}_LL32E: return
26;_LL32F: {struct Cyc_Absyn_DynRgnType_struct*_tmp601=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5E7;if(_tmp601->tag != 17)goto _LL331;}_LL330: return 27;_LL331: {struct Cyc_Absyn_ValueofType_struct*
_tmp602=(struct Cyc_Absyn_ValueofType_struct*)_tmp5E7;if(_tmp602->tag != 19)goto
_LL333;}_LL332: return 28;_LL333: {struct Cyc_Absyn_TypeDeclType_struct*_tmp603=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp5E7;if(_tmp603->tag != 27)goto _LL2FC;}
_LL334: return 29;_LL2FC:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp604=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp604 != 0)
return _tmp604;{struct _tuple0 _tmp12B5;struct _tuple0 _tmp606=(_tmp12B5.f1=t2,((
_tmp12B5.f2=t1,_tmp12B5)));void*_tmp607;void*_tmp609;void*_tmp60B;struct Cyc_Absyn_Tvar*
_tmp60D;void*_tmp60E;struct Cyc_Absyn_Tvar*_tmp610;void*_tmp611;struct Cyc_Absyn_AggrInfo
_tmp613;union Cyc_Absyn_AggrInfoU _tmp614;struct Cyc_List_List*_tmp615;void*_tmp616;
struct Cyc_Absyn_AggrInfo _tmp618;union Cyc_Absyn_AggrInfoU _tmp619;struct Cyc_List_List*
_tmp61A;void*_tmp61B;struct _tuple2*_tmp61D;void*_tmp61E;struct _tuple2*_tmp620;
void*_tmp621;struct Cyc_List_List*_tmp623;void*_tmp624;struct Cyc_List_List*
_tmp626;void*_tmp627;struct Cyc_Absyn_DatatypeInfo _tmp629;union Cyc_Absyn_DatatypeInfoU
_tmp62A;struct Cyc_Absyn_Datatypedecl**_tmp62B;struct Cyc_Absyn_Datatypedecl*
_tmp62C;struct Cyc_List_List*_tmp62D;void*_tmp62E;struct Cyc_Absyn_DatatypeInfo
_tmp630;union Cyc_Absyn_DatatypeInfoU _tmp631;struct Cyc_Absyn_Datatypedecl**
_tmp632;struct Cyc_Absyn_Datatypedecl*_tmp633;struct Cyc_List_List*_tmp634;void*
_tmp635;struct Cyc_Absyn_DatatypeFieldInfo _tmp637;union Cyc_Absyn_DatatypeFieldInfoU
_tmp638;struct _tuple3 _tmp639;struct Cyc_Absyn_Datatypedecl*_tmp63A;struct Cyc_Absyn_Datatypefield*
_tmp63B;struct Cyc_List_List*_tmp63C;void*_tmp63D;struct Cyc_Absyn_DatatypeFieldInfo
_tmp63F;union Cyc_Absyn_DatatypeFieldInfoU _tmp640;struct _tuple3 _tmp641;struct Cyc_Absyn_Datatypedecl*
_tmp642;struct Cyc_Absyn_Datatypefield*_tmp643;struct Cyc_List_List*_tmp644;void*
_tmp645;struct Cyc_Absyn_PtrInfo _tmp647;void*_tmp648;struct Cyc_Absyn_Tqual _tmp649;
struct Cyc_Absyn_PtrAtts _tmp64A;void*_tmp64B;union Cyc_Absyn_Constraint*_tmp64C;
union Cyc_Absyn_Constraint*_tmp64D;union Cyc_Absyn_Constraint*_tmp64E;void*_tmp64F;
struct Cyc_Absyn_PtrInfo _tmp651;void*_tmp652;struct Cyc_Absyn_Tqual _tmp653;struct
Cyc_Absyn_PtrAtts _tmp654;void*_tmp655;union Cyc_Absyn_Constraint*_tmp656;union Cyc_Absyn_Constraint*
_tmp657;union Cyc_Absyn_Constraint*_tmp658;void*_tmp659;enum Cyc_Absyn_Sign
_tmp65B;enum Cyc_Absyn_Size_of _tmp65C;void*_tmp65D;enum Cyc_Absyn_Sign _tmp65F;
enum Cyc_Absyn_Size_of _tmp660;void*_tmp661;void*_tmp663;void*_tmp665;int _tmp667;
void*_tmp668;int _tmp66A;void*_tmp66B;struct Cyc_Absyn_ArrayInfo _tmp66D;void*
_tmp66E;struct Cyc_Absyn_Tqual _tmp66F;struct Cyc_Absyn_Exp*_tmp670;union Cyc_Absyn_Constraint*
_tmp671;void*_tmp672;struct Cyc_Absyn_ArrayInfo _tmp674;void*_tmp675;struct Cyc_Absyn_Tqual
_tmp676;struct Cyc_Absyn_Exp*_tmp677;union Cyc_Absyn_Constraint*_tmp678;void*
_tmp679;struct Cyc_Absyn_FnInfo _tmp67B;struct Cyc_List_List*_tmp67C;struct Cyc_Core_Opt*
_tmp67D;struct Cyc_Absyn_Tqual _tmp67E;void*_tmp67F;struct Cyc_List_List*_tmp680;
int _tmp681;struct Cyc_Absyn_VarargInfo*_tmp682;struct Cyc_List_List*_tmp683;struct
Cyc_List_List*_tmp684;void*_tmp685;struct Cyc_Absyn_FnInfo _tmp687;struct Cyc_List_List*
_tmp688;struct Cyc_Core_Opt*_tmp689;struct Cyc_Absyn_Tqual _tmp68A;void*_tmp68B;
struct Cyc_List_List*_tmp68C;int _tmp68D;struct Cyc_Absyn_VarargInfo*_tmp68E;struct
Cyc_List_List*_tmp68F;struct Cyc_List_List*_tmp690;void*_tmp691;struct Cyc_List_List*
_tmp693;void*_tmp694;struct Cyc_List_List*_tmp696;void*_tmp697;enum Cyc_Absyn_AggrKind
_tmp699;struct Cyc_List_List*_tmp69A;void*_tmp69B;enum Cyc_Absyn_AggrKind _tmp69D;
struct Cyc_List_List*_tmp69E;void*_tmp69F;void*_tmp6A1;void*_tmp6A2;void*_tmp6A4;
void*_tmp6A5;void*_tmp6A7;void*_tmp6A8;void*_tmp6A9;void*_tmp6AB;void*_tmp6AC;
void*_tmp6AD;void*_tmp6AF;void*_tmp6B0;void*_tmp6B2;void*_tmp6B3;struct Cyc_Absyn_Exp*
_tmp6B5;void*_tmp6B6;struct Cyc_Absyn_Exp*_tmp6B8;void*_tmp6B9;void*_tmp6BB;void*
_tmp6BD;void*_tmp6BF;void*_tmp6C1;void*_tmp6C3;_LL336: _tmp607=_tmp606.f1;{struct
Cyc_Absyn_Evar_struct*_tmp608=(struct Cyc_Absyn_Evar_struct*)_tmp607;if(_tmp608->tag
!= 1)goto _LL338;};_tmp609=_tmp606.f2;{struct Cyc_Absyn_Evar_struct*_tmp60A=(
struct Cyc_Absyn_Evar_struct*)_tmp609;if(_tmp60A->tag != 1)goto _LL338;};_LL337: {
const char*_tmp12B8;void*_tmp12B7;(_tmp12B7=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B8="typecmp: can only compare closed types",
_tag_dyneither(_tmp12B8,sizeof(char),39))),_tag_dyneither(_tmp12B7,sizeof(void*),
0)));}_LL338: _tmp60B=_tmp606.f1;{struct Cyc_Absyn_VarType_struct*_tmp60C=(struct
Cyc_Absyn_VarType_struct*)_tmp60B;if(_tmp60C->tag != 2)goto _LL33A;else{_tmp60D=
_tmp60C->f1;}};_tmp60E=_tmp606.f2;{struct Cyc_Absyn_VarType_struct*_tmp60F=(
struct Cyc_Absyn_VarType_struct*)_tmp60E;if(_tmp60F->tag != 2)goto _LL33A;else{
_tmp610=_tmp60F->f1;}};_LL339: return Cyc_Core_intcmp(_tmp610->identity,_tmp60D->identity);
_LL33A: _tmp611=_tmp606.f1;{struct Cyc_Absyn_AggrType_struct*_tmp612=(struct Cyc_Absyn_AggrType_struct*)
_tmp611;if(_tmp612->tag != 12)goto _LL33C;else{_tmp613=_tmp612->f1;_tmp614=_tmp613.aggr_info;
_tmp615=_tmp613.targs;}};_tmp616=_tmp606.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp617=(struct Cyc_Absyn_AggrType_struct*)_tmp616;if(_tmp617->tag != 12)goto
_LL33C;else{_tmp618=_tmp617->f1;_tmp619=_tmp618.aggr_info;_tmp61A=_tmp618.targs;}};
_LL33B: {struct _tuple2*_tmp6C8;struct _tuple12 _tmp6C7=Cyc_Absyn_aggr_kinded_name(
_tmp614);_tmp6C8=_tmp6C7.f2;{struct _tuple2*_tmp6CA;struct _tuple12 _tmp6C9=Cyc_Absyn_aggr_kinded_name(
_tmp619);_tmp6CA=_tmp6C9.f2;{int _tmp6CB=Cyc_Absyn_qvar_cmp(_tmp6C8,_tmp6CA);if(
_tmp6CB != 0)return _tmp6CB;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp615,_tmp61A);}};};}
_LL33C: _tmp61B=_tmp606.f1;{struct Cyc_Absyn_EnumType_struct*_tmp61C=(struct Cyc_Absyn_EnumType_struct*)
_tmp61B;if(_tmp61C->tag != 14)goto _LL33E;else{_tmp61D=_tmp61C->f1;}};_tmp61E=
_tmp606.f2;{struct Cyc_Absyn_EnumType_struct*_tmp61F=(struct Cyc_Absyn_EnumType_struct*)
_tmp61E;if(_tmp61F->tag != 14)goto _LL33E;else{_tmp620=_tmp61F->f1;}};_LL33D:
return Cyc_Absyn_qvar_cmp(_tmp61D,_tmp620);_LL33E: _tmp621=_tmp606.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp622=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp621;if(_tmp622->tag != 15)goto
_LL340;else{_tmp623=_tmp622->f1;}};_tmp624=_tmp606.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp625=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp624;if(_tmp625->tag != 15)goto
_LL340;else{_tmp626=_tmp625->f1;}};_LL33F: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp623,_tmp626);_LL340: _tmp627=_tmp606.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp628=(struct Cyc_Absyn_DatatypeType_struct*)_tmp627;if(_tmp628->tag != 3)goto
_LL342;else{_tmp629=_tmp628->f1;_tmp62A=_tmp629.datatype_info;if((_tmp62A.KnownDatatype).tag
!= 2)goto _LL342;_tmp62B=(struct Cyc_Absyn_Datatypedecl**)(_tmp62A.KnownDatatype).val;
_tmp62C=*_tmp62B;_tmp62D=_tmp629.targs;}};_tmp62E=_tmp606.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp62F=(struct Cyc_Absyn_DatatypeType_struct*)_tmp62E;if(_tmp62F->tag != 3)goto
_LL342;else{_tmp630=_tmp62F->f1;_tmp631=_tmp630.datatype_info;if((_tmp631.KnownDatatype).tag
!= 2)goto _LL342;_tmp632=(struct Cyc_Absyn_Datatypedecl**)(_tmp631.KnownDatatype).val;
_tmp633=*_tmp632;_tmp634=_tmp630.targs;}};_LL341: if(_tmp633 == _tmp62C)return 0;{
int _tmp6CC=Cyc_Absyn_qvar_cmp(_tmp633->name,_tmp62C->name);if(_tmp6CC != 0)return
_tmp6CC;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp634,_tmp62D);};_LL342: _tmp635=
_tmp606.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp636=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp635;if(_tmp636->tag != 4)goto _LL344;else{_tmp637=_tmp636->f1;_tmp638=_tmp637.field_info;
if((_tmp638.KnownDatatypefield).tag != 2)goto _LL344;_tmp639=(struct _tuple3)(
_tmp638.KnownDatatypefield).val;_tmp63A=_tmp639.f1;_tmp63B=_tmp639.f2;_tmp63C=
_tmp637.targs;}};_tmp63D=_tmp606.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp63E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp63D;if(_tmp63E->tag != 4)
goto _LL344;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63F.field_info;if((_tmp640.KnownDatatypefield).tag
!= 2)goto _LL344;_tmp641=(struct _tuple3)(_tmp640.KnownDatatypefield).val;_tmp642=
_tmp641.f1;_tmp643=_tmp641.f2;_tmp644=_tmp63F.targs;}};_LL343: if(_tmp642 == 
_tmp63A)return 0;{int _tmp6CD=Cyc_Absyn_qvar_cmp(_tmp63A->name,_tmp642->name);if(
_tmp6CD != 0)return _tmp6CD;{int _tmp6CE=Cyc_Absyn_qvar_cmp(_tmp63B->name,_tmp643->name);
if(_tmp6CE != 0)return _tmp6CE;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp644,_tmp63C);};};
_LL344: _tmp645=_tmp606.f1;{struct Cyc_Absyn_PointerType_struct*_tmp646=(struct Cyc_Absyn_PointerType_struct*)
_tmp645;if(_tmp646->tag != 5)goto _LL346;else{_tmp647=_tmp646->f1;_tmp648=_tmp647.elt_typ;
_tmp649=_tmp647.elt_tq;_tmp64A=_tmp647.ptr_atts;_tmp64B=_tmp64A.rgn;_tmp64C=
_tmp64A.nullable;_tmp64D=_tmp64A.bounds;_tmp64E=_tmp64A.zero_term;}};_tmp64F=
_tmp606.f2;{struct Cyc_Absyn_PointerType_struct*_tmp650=(struct Cyc_Absyn_PointerType_struct*)
_tmp64F;if(_tmp650->tag != 5)goto _LL346;else{_tmp651=_tmp650->f1;_tmp652=_tmp651.elt_typ;
_tmp653=_tmp651.elt_tq;_tmp654=_tmp651.ptr_atts;_tmp655=_tmp654.rgn;_tmp656=
_tmp654.nullable;_tmp657=_tmp654.bounds;_tmp658=_tmp654.zero_term;}};_LL345: {
int _tmp6CF=Cyc_Tcutil_typecmp(_tmp652,_tmp648);if(_tmp6CF != 0)return _tmp6CF;{int
_tmp6D0=Cyc_Tcutil_typecmp(_tmp655,_tmp64B);if(_tmp6D0 != 0)return _tmp6D0;{int
_tmp6D1=Cyc_Tcutil_tqual_cmp(_tmp653,_tmp649);if(_tmp6D1 != 0)return _tmp6D1;{int
_tmp6D2=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp657,_tmp64D);if(_tmp6D2 != 0)
return _tmp6D2;{int _tmp6D3=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp658,
_tmp64E);if(_tmp6D3 != 0)return _tmp6D3;{void*_tmp6D4=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp657);_LL36B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6D5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6D4;if(_tmp6D5->tag != 0)goto
_LL36D;}_LL36C: return 0;_LL36D:;_LL36E: goto _LL36A;_LL36A:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp656,_tmp64C);};};};};}_LL346: _tmp659=_tmp606.f1;{struct Cyc_Absyn_IntType_struct*
_tmp65A=(struct Cyc_Absyn_IntType_struct*)_tmp659;if(_tmp65A->tag != 6)goto _LL348;
else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65A->f2;}};_tmp65D=_tmp606.f2;{struct Cyc_Absyn_IntType_struct*
_tmp65E=(struct Cyc_Absyn_IntType_struct*)_tmp65D;if(_tmp65E->tag != 6)goto _LL348;
else{_tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;}};_LL347: if(_tmp65F != _tmp65B)
return Cyc_Core_intcmp((int)((unsigned int)_tmp65F),(int)((unsigned int)_tmp65B));
if(_tmp660 != _tmp65C)return Cyc_Core_intcmp((int)((unsigned int)_tmp660),(int)((
unsigned int)_tmp65C));return 0;_LL348: _tmp661=_tmp606.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp662=(struct Cyc_Absyn_FloatType_struct*)_tmp661;if(_tmp662->tag != 7)goto
_LL34A;};_tmp663=_tmp606.f2;{struct Cyc_Absyn_FloatType_struct*_tmp664=(struct Cyc_Absyn_FloatType_struct*)
_tmp663;if(_tmp664->tag != 7)goto _LL34A;};_LL349: return 0;_LL34A: _tmp665=_tmp606.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp666=(struct Cyc_Absyn_DoubleType_struct*)
_tmp665;if(_tmp666->tag != 8)goto _LL34C;else{_tmp667=_tmp666->f1;}};_tmp668=
_tmp606.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp669=(struct Cyc_Absyn_DoubleType_struct*)
_tmp668;if(_tmp669->tag != 8)goto _LL34C;else{_tmp66A=_tmp669->f1;}};_LL34B: if(
_tmp667 == _tmp66A)return 0;else{if(_tmp667)return - 1;else{return 1;}}_LL34C: _tmp66B=
_tmp606.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp66C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp66B;if(_tmp66C->tag != 9)goto _LL34E;else{_tmp66D=_tmp66C->f1;_tmp66E=_tmp66D.elt_type;
_tmp66F=_tmp66D.tq;_tmp670=_tmp66D.num_elts;_tmp671=_tmp66D.zero_term;}};_tmp672=
_tmp606.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp673=(struct Cyc_Absyn_ArrayType_struct*)
_tmp672;if(_tmp673->tag != 9)goto _LL34E;else{_tmp674=_tmp673->f1;_tmp675=_tmp674.elt_type;
_tmp676=_tmp674.tq;_tmp677=_tmp674.num_elts;_tmp678=_tmp674.zero_term;}};_LL34D: {
int _tmp6D6=Cyc_Tcutil_tqual_cmp(_tmp676,_tmp66F);if(_tmp6D6 != 0)return _tmp6D6;{
int _tmp6D7=Cyc_Tcutil_typecmp(_tmp675,_tmp66E);if(_tmp6D7 != 0)return _tmp6D7;{int
_tmp6D8=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp671,_tmp678);if(_tmp6D8 != 0)
return _tmp6D8;if(_tmp670 == _tmp677)return 0;if(_tmp670 == 0  || _tmp677 == 0){const
char*_tmp12BB;void*_tmp12BA;(_tmp12BA=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12BB="missing expression in array index",
_tag_dyneither(_tmp12BB,sizeof(char),34))),_tag_dyneither(_tmp12BA,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp670,_tmp677);};};}_LL34E: _tmp679=_tmp606.f1;{struct Cyc_Absyn_FnType_struct*
_tmp67A=(struct Cyc_Absyn_FnType_struct*)_tmp679;if(_tmp67A->tag != 10)goto _LL350;
else{_tmp67B=_tmp67A->f1;_tmp67C=_tmp67B.tvars;_tmp67D=_tmp67B.effect;_tmp67E=
_tmp67B.ret_tqual;_tmp67F=_tmp67B.ret_typ;_tmp680=_tmp67B.args;_tmp681=_tmp67B.c_varargs;
_tmp682=_tmp67B.cyc_varargs;_tmp683=_tmp67B.rgn_po;_tmp684=_tmp67B.attributes;}};
_tmp685=_tmp606.f2;{struct Cyc_Absyn_FnType_struct*_tmp686=(struct Cyc_Absyn_FnType_struct*)
_tmp685;if(_tmp686->tag != 10)goto _LL350;else{_tmp687=_tmp686->f1;_tmp688=_tmp687.tvars;
_tmp689=_tmp687.effect;_tmp68A=_tmp687.ret_tqual;_tmp68B=_tmp687.ret_typ;_tmp68C=
_tmp687.args;_tmp68D=_tmp687.c_varargs;_tmp68E=_tmp687.cyc_varargs;_tmp68F=
_tmp687.rgn_po;_tmp690=_tmp687.attributes;}};_LL34F: {const char*_tmp12BE;void*
_tmp12BD;(_tmp12BD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12BE="typecmp: function types not handled",
_tag_dyneither(_tmp12BE,sizeof(char),36))),_tag_dyneither(_tmp12BD,sizeof(void*),
0)));}_LL350: _tmp691=_tmp606.f1;{struct Cyc_Absyn_TupleType_struct*_tmp692=(
struct Cyc_Absyn_TupleType_struct*)_tmp691;if(_tmp692->tag != 11)goto _LL352;else{
_tmp693=_tmp692->f1;}};_tmp694=_tmp606.f2;{struct Cyc_Absyn_TupleType_struct*
_tmp695=(struct Cyc_Absyn_TupleType_struct*)_tmp694;if(_tmp695->tag != 11)goto
_LL352;else{_tmp696=_tmp695->f1;}};_LL351: return((int(*)(int(*cmp)(struct
_tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_tqual_type_cmp,_tmp696,_tmp693);_LL352: _tmp697=_tmp606.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp698=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp697;if(_tmp698->tag != 13)goto
_LL354;else{_tmp699=_tmp698->f1;_tmp69A=_tmp698->f2;}};_tmp69B=_tmp606.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp69C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp69B;if(_tmp69C->tag != 13)goto _LL354;else{_tmp69D=_tmp69C->f1;_tmp69E=_tmp69C->f2;}};
_LL353: if(_tmp69D != _tmp699){if(_tmp69D == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp69E,_tmp69A);_LL354: _tmp69F=_tmp606.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A0=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp69F;if(_tmp6A0->tag != 16)goto
_LL356;else{_tmp6A1=(void*)_tmp6A0->f1;}};_tmp6A2=_tmp606.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A3=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A2;if(_tmp6A3->tag != 16)goto
_LL356;else{_tmp6A4=(void*)_tmp6A3->f1;}};_LL355: return Cyc_Tcutil_typecmp(
_tmp6A1,_tmp6A4);_LL356: _tmp6A5=_tmp606.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp6A6=(struct Cyc_Absyn_DynRgnType_struct*)_tmp6A5;if(_tmp6A6->tag != 17)goto
_LL358;else{_tmp6A7=(void*)_tmp6A6->f1;_tmp6A8=(void*)_tmp6A6->f2;}};_tmp6A9=
_tmp606.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp6AA=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp6A9;if(_tmp6AA->tag != 17)goto _LL358;else{_tmp6AB=(void*)_tmp6AA->f1;_tmp6AC=(
void*)_tmp6AA->f2;}};_LL357: {int _tmp6DD=Cyc_Tcutil_typecmp(_tmp6A7,_tmp6AB);if(
_tmp6DD != 0)return _tmp6DD;else{return Cyc_Tcutil_typecmp(_tmp6A8,_tmp6AC);}}
_LL358: _tmp6AD=_tmp606.f1;{struct Cyc_Absyn_TagType_struct*_tmp6AE=(struct Cyc_Absyn_TagType_struct*)
_tmp6AD;if(_tmp6AE->tag != 20)goto _LL35A;else{_tmp6AF=(void*)_tmp6AE->f1;}};
_tmp6B0=_tmp606.f2;{struct Cyc_Absyn_TagType_struct*_tmp6B1=(struct Cyc_Absyn_TagType_struct*)
_tmp6B0;if(_tmp6B1->tag != 20)goto _LL35A;else{_tmp6B2=(void*)_tmp6B1->f1;}};
_LL359: return Cyc_Tcutil_typecmp(_tmp6AF,_tmp6B2);_LL35A: _tmp6B3=_tmp606.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp6B4=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6B3;if(_tmp6B4->tag != 19)goto _LL35C;else{_tmp6B5=_tmp6B4->f1;}};_tmp6B6=
_tmp606.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp6B7=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6B6;if(_tmp6B7->tag != 19)goto _LL35C;else{_tmp6B8=_tmp6B7->f1;}};_LL35B:
return Cyc_Evexp_const_exp_cmp(_tmp6B5,_tmp6B8);_LL35C: _tmp6B9=_tmp606.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp6BA=(struct Cyc_Absyn_JoinEff_struct*)_tmp6B9;if(
_tmp6BA->tag != 25)goto _LL35E;};_LL35D: goto _LL35F;_LL35E: _tmp6BB=_tmp606.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp6BC=(struct Cyc_Absyn_JoinEff_struct*)_tmp6BB;
if(_tmp6BC->tag != 25)goto _LL360;};_LL35F: goto _LL361;_LL360: _tmp6BD=_tmp606.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp6BE=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6BD;if(_tmp6BE->tag != 24)goto _LL362;};_LL361: goto _LL363;_LL362: _tmp6BF=
_tmp606.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp6C0=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6BF;if(_tmp6C0->tag != 26)goto _LL364;};_LL363: goto _LL365;_LL364: _tmp6C1=
_tmp606.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp6C2=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6C1;if(_tmp6C2->tag != 26)goto _LL366;};_LL365: goto _LL367;_LL366: _tmp6C3=
_tmp606.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp6C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6C3;if(_tmp6C4->tag != 24)goto _LL368;};_LL367: {const char*_tmp12C1;void*
_tmp12C0;(_tmp12C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C1="typecmp: effects not handled",_tag_dyneither(
_tmp12C1,sizeof(char),29))),_tag_dyneither(_tmp12C0,sizeof(void*),0)));}_LL368:;
_LL369: {const char*_tmp12C4;void*_tmp12C3;(_tmp12C3=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C4="Unmatched case in typecmp",
_tag_dyneither(_tmp12C4,sizeof(char),26))),_tag_dyneither(_tmp12C3,sizeof(void*),
0)));}_LL335:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6E2=Cyc_Tcutil_compress(
t);_LL370: {struct Cyc_Absyn_IntType_struct*_tmp6E3=(struct Cyc_Absyn_IntType_struct*)
_tmp6E2;if(_tmp6E3->tag != 6)goto _LL372;}_LL371: goto _LL373;_LL372: {struct Cyc_Absyn_FloatType_struct*
_tmp6E4=(struct Cyc_Absyn_FloatType_struct*)_tmp6E2;if(_tmp6E4->tag != 7)goto
_LL374;}_LL373: goto _LL375;_LL374: {struct Cyc_Absyn_DoubleType_struct*_tmp6E5=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6E2;if(_tmp6E5->tag != 8)goto _LL376;}
_LL375: goto _LL377;_LL376: {struct Cyc_Absyn_EnumType_struct*_tmp6E6=(struct Cyc_Absyn_EnumType_struct*)
_tmp6E2;if(_tmp6E6->tag != 14)goto _LL378;}_LL377: goto _LL379;_LL378: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp6E7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp6E2;if(_tmp6E7->tag != 15)goto
_LL37A;}_LL379: return 1;_LL37A:;_LL37B: return 0;_LL36F:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp12C5;struct _tuple0 _tmp6E9=(_tmp12C5.f1=t1,((_tmp12C5.f2=t2,_tmp12C5)));
void*_tmp6EA;int _tmp6EC;void*_tmp6ED;int _tmp6EF;void*_tmp6F0;void*_tmp6F2;void*
_tmp6F4;void*_tmp6F6;void*_tmp6F8;void*_tmp6FA;void*_tmp6FC;void*_tmp6FE;void*
_tmp700;void*_tmp702;void*_tmp704;enum Cyc_Absyn_Size_of _tmp706;void*_tmp707;
enum Cyc_Absyn_Size_of _tmp709;void*_tmp70A;enum Cyc_Absyn_Size_of _tmp70C;void*
_tmp70D;enum Cyc_Absyn_Size_of _tmp70F;void*_tmp710;enum Cyc_Absyn_Size_of _tmp712;
void*_tmp713;enum Cyc_Absyn_Size_of _tmp715;void*_tmp716;enum Cyc_Absyn_Size_of
_tmp718;void*_tmp719;enum Cyc_Absyn_Size_of _tmp71B;void*_tmp71C;void*_tmp71E;
enum Cyc_Absyn_Size_of _tmp720;void*_tmp721;void*_tmp723;enum Cyc_Absyn_Size_of
_tmp725;void*_tmp726;enum Cyc_Absyn_Size_of _tmp728;void*_tmp729;enum Cyc_Absyn_Size_of
_tmp72B;void*_tmp72C;enum Cyc_Absyn_Size_of _tmp72E;void*_tmp72F;void*_tmp731;
enum Cyc_Absyn_Size_of _tmp733;void*_tmp734;enum Cyc_Absyn_Size_of _tmp736;void*
_tmp737;enum Cyc_Absyn_Size_of _tmp739;void*_tmp73A;enum Cyc_Absyn_Size_of _tmp73C;
void*_tmp73D;enum Cyc_Absyn_Size_of _tmp73F;void*_tmp740;enum Cyc_Absyn_Size_of
_tmp742;void*_tmp743;enum Cyc_Absyn_Size_of _tmp745;void*_tmp746;void*_tmp748;
enum Cyc_Absyn_Size_of _tmp74A;_LL37D: _tmp6EA=_tmp6E9.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6EB=(struct Cyc_Absyn_DoubleType_struct*)_tmp6EA;if(_tmp6EB->tag != 8)goto
_LL37F;else{_tmp6EC=_tmp6EB->f1;}};_tmp6ED=_tmp6E9.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp6EE=(struct Cyc_Absyn_DoubleType_struct*)_tmp6ED;if(_tmp6EE->tag != 8)goto
_LL37F;else{_tmp6EF=_tmp6EE->f1;}};_LL37E: return !_tmp6EF  && _tmp6EC;_LL37F:
_tmp6F0=_tmp6E9.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6F1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6F0;if(_tmp6F1->tag != 8)goto _LL381;};_tmp6F2=_tmp6E9.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp6F3=(struct Cyc_Absyn_FloatType_struct*)_tmp6F2;if(_tmp6F3->tag != 7)goto
_LL381;};_LL380: goto _LL382;_LL381: _tmp6F4=_tmp6E9.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp6F5=(struct Cyc_Absyn_DoubleType_struct*)_tmp6F4;if(_tmp6F5->tag != 8)goto
_LL383;};_tmp6F6=_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp6F7=(struct Cyc_Absyn_IntType_struct*)
_tmp6F6;if(_tmp6F7->tag != 6)goto _LL383;};_LL382: goto _LL384;_LL383: _tmp6F8=
_tmp6E9.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp6F9=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6F8;if(_tmp6F9->tag != 8)goto _LL385;};_tmp6FA=_tmp6E9.f2;{struct Cyc_Absyn_TagType_struct*
_tmp6FB=(struct Cyc_Absyn_TagType_struct*)_tmp6FA;if(_tmp6FB->tag != 20)goto _LL385;};
_LL384: goto _LL386;_LL385: _tmp6FC=_tmp6E9.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6FD=(struct Cyc_Absyn_FloatType_struct*)_tmp6FC;if(_tmp6FD->tag != 7)goto
_LL387;};_tmp6FE=_tmp6E9.f2;{struct Cyc_Absyn_TagType_struct*_tmp6FF=(struct Cyc_Absyn_TagType_struct*)
_tmp6FE;if(_tmp6FF->tag != 20)goto _LL387;};_LL386: goto _LL388;_LL387: _tmp700=
_tmp6E9.f1;{struct Cyc_Absyn_FloatType_struct*_tmp701=(struct Cyc_Absyn_FloatType_struct*)
_tmp700;if(_tmp701->tag != 7)goto _LL389;};_tmp702=_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*
_tmp703=(struct Cyc_Absyn_IntType_struct*)_tmp702;if(_tmp703->tag != 6)goto _LL389;};
_LL388: return 1;_LL389: _tmp704=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*_tmp705=(
struct Cyc_Absyn_IntType_struct*)_tmp704;if(_tmp705->tag != 6)goto _LL38B;else{
_tmp706=_tmp705->f2;if(_tmp706 != Cyc_Absyn_LongLong_sz)goto _LL38B;}};_tmp707=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp708=(struct Cyc_Absyn_IntType_struct*)
_tmp707;if(_tmp708->tag != 6)goto _LL38B;else{_tmp709=_tmp708->f2;if(_tmp709 != Cyc_Absyn_LongLong_sz)
goto _LL38B;}};_LL38A: return 0;_LL38B: _tmp70A=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp70B=(struct Cyc_Absyn_IntType_struct*)_tmp70A;if(_tmp70B->tag != 6)goto _LL38D;
else{_tmp70C=_tmp70B->f2;if(_tmp70C != Cyc_Absyn_LongLong_sz)goto _LL38D;}};_LL38C:
return 1;_LL38D: _tmp70D=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*_tmp70E=(
struct Cyc_Absyn_IntType_struct*)_tmp70D;if(_tmp70E->tag != 6)goto _LL38F;else{
_tmp70F=_tmp70E->f2;if(_tmp70F != Cyc_Absyn_Long_sz)goto _LL38F;}};_tmp710=_tmp6E9.f2;{
struct Cyc_Absyn_IntType_struct*_tmp711=(struct Cyc_Absyn_IntType_struct*)_tmp710;
if(_tmp711->tag != 6)goto _LL38F;else{_tmp712=_tmp711->f2;if(_tmp712 != Cyc_Absyn_Int_sz)
goto _LL38F;}};_LL38E: goto _LL390;_LL38F: _tmp713=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp714=(struct Cyc_Absyn_IntType_struct*)_tmp713;if(_tmp714->tag != 6)goto _LL391;
else{_tmp715=_tmp714->f2;if(_tmp715 != Cyc_Absyn_Int_sz)goto _LL391;}};_tmp716=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp717=(struct Cyc_Absyn_IntType_struct*)
_tmp716;if(_tmp717->tag != 6)goto _LL391;else{_tmp718=_tmp717->f2;if(_tmp718 != Cyc_Absyn_Long_sz)
goto _LL391;}};_LL390: return 0;_LL391: _tmp719=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp71A=(struct Cyc_Absyn_IntType_struct*)_tmp719;if(_tmp71A->tag != 6)goto _LL393;
else{_tmp71B=_tmp71A->f2;if(_tmp71B != Cyc_Absyn_Long_sz)goto _LL393;}};_tmp71C=
_tmp6E9.f2;{struct Cyc_Absyn_FloatType_struct*_tmp71D=(struct Cyc_Absyn_FloatType_struct*)
_tmp71C;if(_tmp71D->tag != 7)goto _LL393;};_LL392: goto _LL394;_LL393: _tmp71E=
_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*_tmp71F=(struct Cyc_Absyn_IntType_struct*)
_tmp71E;if(_tmp71F->tag != 6)goto _LL395;else{_tmp720=_tmp71F->f2;if(_tmp720 != Cyc_Absyn_Int_sz)
goto _LL395;}};_tmp721=_tmp6E9.f2;{struct Cyc_Absyn_FloatType_struct*_tmp722=(
struct Cyc_Absyn_FloatType_struct*)_tmp721;if(_tmp722->tag != 7)goto _LL395;};
_LL394: goto _LL396;_LL395: _tmp723=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp724=(struct Cyc_Absyn_IntType_struct*)_tmp723;if(_tmp724->tag != 6)goto _LL397;
else{_tmp725=_tmp724->f2;if(_tmp725 != Cyc_Absyn_Long_sz)goto _LL397;}};_tmp726=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp727=(struct Cyc_Absyn_IntType_struct*)
_tmp726;if(_tmp727->tag != 6)goto _LL397;else{_tmp728=_tmp727->f2;if(_tmp728 != Cyc_Absyn_Short_sz)
goto _LL397;}};_LL396: goto _LL398;_LL397: _tmp729=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp72A=(struct Cyc_Absyn_IntType_struct*)_tmp729;if(_tmp72A->tag != 6)goto _LL399;
else{_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Int_sz)goto _LL399;}};_tmp72C=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp72D=(struct Cyc_Absyn_IntType_struct*)
_tmp72C;if(_tmp72D->tag != 6)goto _LL399;else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Short_sz)
goto _LL399;}};_LL398: goto _LL39A;_LL399: _tmp72F=_tmp6E9.f1;{struct Cyc_Absyn_TagType_struct*
_tmp730=(struct Cyc_Absyn_TagType_struct*)_tmp72F;if(_tmp730->tag != 20)goto _LL39B;};
_tmp731=_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp732=(struct Cyc_Absyn_IntType_struct*)
_tmp731;if(_tmp732->tag != 6)goto _LL39B;else{_tmp733=_tmp732->f2;if(_tmp733 != Cyc_Absyn_Short_sz)
goto _LL39B;}};_LL39A: goto _LL39C;_LL39B: _tmp734=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp735=(struct Cyc_Absyn_IntType_struct*)_tmp734;if(_tmp735->tag != 6)goto _LL39D;
else{_tmp736=_tmp735->f2;if(_tmp736 != Cyc_Absyn_Long_sz)goto _LL39D;}};_tmp737=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp738=(struct Cyc_Absyn_IntType_struct*)
_tmp737;if(_tmp738->tag != 6)goto _LL39D;else{_tmp739=_tmp738->f2;if(_tmp739 != Cyc_Absyn_Char_sz)
goto _LL39D;}};_LL39C: goto _LL39E;_LL39D: _tmp73A=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp73B=(struct Cyc_Absyn_IntType_struct*)_tmp73A;if(_tmp73B->tag != 6)goto _LL39F;
else{_tmp73C=_tmp73B->f2;if(_tmp73C != Cyc_Absyn_Int_sz)goto _LL39F;}};_tmp73D=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp73E=(struct Cyc_Absyn_IntType_struct*)
_tmp73D;if(_tmp73E->tag != 6)goto _LL39F;else{_tmp73F=_tmp73E->f2;if(_tmp73F != Cyc_Absyn_Char_sz)
goto _LL39F;}};_LL39E: goto _LL3A0;_LL39F: _tmp740=_tmp6E9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp741=(struct Cyc_Absyn_IntType_struct*)_tmp740;if(_tmp741->tag != 6)goto _LL3A1;
else{_tmp742=_tmp741->f2;if(_tmp742 != Cyc_Absyn_Short_sz)goto _LL3A1;}};_tmp743=
_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp744=(struct Cyc_Absyn_IntType_struct*)
_tmp743;if(_tmp744->tag != 6)goto _LL3A1;else{_tmp745=_tmp744->f2;if(_tmp745 != Cyc_Absyn_Char_sz)
goto _LL3A1;}};_LL3A0: goto _LL3A2;_LL3A1: _tmp746=_tmp6E9.f1;{struct Cyc_Absyn_TagType_struct*
_tmp747=(struct Cyc_Absyn_TagType_struct*)_tmp746;if(_tmp747->tag != 20)goto _LL3A3;};
_tmp748=_tmp6E9.f2;{struct Cyc_Absyn_IntType_struct*_tmp749=(struct Cyc_Absyn_IntType_struct*)
_tmp748;if(_tmp749->tag != 6)goto _LL3A3;else{_tmp74A=_tmp749->f2;if(_tmp74A != Cyc_Absyn_Char_sz)
goto _LL3A3;}};_LL3A2: return 1;_LL3A3:;_LL3A4: return 0;_LL37C:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp74B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp12C6;max_arith_type=((_tmp12C6=
_region_malloc(_tmp74B,sizeof(*_tmp12C6)),((_tmp12C6->v=t1,_tmp12C6))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp12CB;void*_tmp12CA[2];struct
Cyc_String_pa_struct _tmp12C9;struct Cyc_String_pa_struct _tmp12C8;(_tmp12C8.tag=0,((
_tmp12C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp12C9.tag=0,((_tmp12C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12CA[0]=& _tmp12C9,((_tmp12CA[1]=& _tmp12C8,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12CB="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12CB,sizeof(char),41))),_tag_dyneither(_tmp12CA,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp751=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL3A6: {struct Cyc_Absyn_PointerType_struct*
_tmp752=(struct Cyc_Absyn_PointerType_struct*)_tmp751;if(_tmp752->tag != 5)goto
_LL3A8;}_LL3A7: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL3A5;_LL3A8:;_LL3A9: return 0;_LL3A5:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp753=Cyc_Tcutil_compress(t);_LL3AB: {struct Cyc_Absyn_IntType_struct*
_tmp754=(struct Cyc_Absyn_IntType_struct*)_tmp753;if(_tmp754->tag != 6)goto _LL3AD;}
_LL3AC: goto _LL3AE;_LL3AD: {struct Cyc_Absyn_TagType_struct*_tmp755=(struct Cyc_Absyn_TagType_struct*)
_tmp753;if(_tmp755->tag != 20)goto _LL3AF;}_LL3AE: goto _LL3B0;_LL3AF: {struct Cyc_Absyn_EnumType_struct*
_tmp756=(struct Cyc_Absyn_EnumType_struct*)_tmp753;if(_tmp756->tag != 14)goto
_LL3B1;}_LL3B0: goto _LL3B2;_LL3B1: {struct Cyc_Absyn_AnonEnumType_struct*_tmp757=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp753;if(_tmp757->tag != 15)goto _LL3B3;}
_LL3B2: return 1;_LL3B3:;_LL3B4: return 0;_LL3AA:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp12CE;void*_tmp12CD;(_tmp12CD=0,Cyc_Tcutil_warn(e->loc,((_tmp12CE="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12CE,sizeof(char),44))),_tag_dyneither(_tmp12CD,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12D1;void*_tmp12D0;(
_tmp12D0=0,Cyc_Tcutil_warn(e->loc,((_tmp12D1="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12D1,sizeof(char),44))),_tag_dyneither(_tmp12D0,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12D2;struct _tuple0 _tmp75D=(_tmp12D2.f1=
t1,((_tmp12D2.f2=t2,_tmp12D2)));void*_tmp75E;struct Cyc_Absyn_PtrInfo _tmp760;void*
_tmp761;struct Cyc_Absyn_PtrInfo _tmp763;void*_tmp764;struct Cyc_Absyn_ArrayInfo
_tmp766;void*_tmp767;struct Cyc_Absyn_Tqual _tmp768;struct Cyc_Absyn_Exp*_tmp769;
union Cyc_Absyn_Constraint*_tmp76A;void*_tmp76B;struct Cyc_Absyn_ArrayInfo _tmp76D;
void*_tmp76E;struct Cyc_Absyn_Tqual _tmp76F;struct Cyc_Absyn_Exp*_tmp770;union Cyc_Absyn_Constraint*
_tmp771;void*_tmp772;void*_tmp774;_LL3B6: _tmp75E=_tmp75D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp75F=(struct Cyc_Absyn_PointerType_struct*)_tmp75E;if(_tmp75F->tag != 5)goto
_LL3B8;else{_tmp760=_tmp75F->f1;}};_tmp761=_tmp75D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp762=(struct Cyc_Absyn_PointerType_struct*)_tmp761;if(_tmp762->tag != 5)goto
_LL3B8;else{_tmp763=_tmp762->f1;}};_LL3B7: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp760.ptr_atts).nullable,(_tmp763.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp760.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp760.ptr_atts).bounds,(
_tmp763.ptr_atts).bounds)){struct _tuple0 _tmp12D3;struct _tuple0 _tmp777=(_tmp12D3.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp760.ptr_atts).bounds),((_tmp12D3.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp763.ptr_atts).bounds),
_tmp12D3)));void*_tmp778;void*_tmp77A;void*_tmp77C;void*_tmp77E;void*_tmp780;
struct Cyc_Absyn_Exp*_tmp782;void*_tmp783;struct Cyc_Absyn_Exp*_tmp785;void*
_tmp786;void*_tmp788;struct Cyc_Absyn_Exp*_tmp78A;_LL3BF: _tmp778=_tmp777.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp779=(struct Cyc_Absyn_Upper_b_struct*)_tmp778;
if(_tmp779->tag != 1)goto _LL3C1;};_tmp77A=_tmp777.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp77B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp77A;if(_tmp77B->tag != 0)goto
_LL3C1;};_LL3C0: goto _LL3C2;_LL3C1: _tmp77C=_tmp777.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp77D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp77C;if(_tmp77D->tag != 0)goto
_LL3C3;};_tmp77E=_tmp777.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp77F=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp77E;if(_tmp77F->tag != 0)goto _LL3C3;};_LL3C2:
okay=1;goto _LL3BE;_LL3C3: _tmp780=_tmp777.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp781=(struct Cyc_Absyn_Upper_b_struct*)_tmp780;if(_tmp781->tag != 1)goto _LL3C5;
else{_tmp782=_tmp781->f1;}};_tmp783=_tmp777.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp784=(struct Cyc_Absyn_Upper_b_struct*)_tmp783;if(_tmp784->tag != 1)goto _LL3C5;
else{_tmp785=_tmp784->f1;}};_LL3C4: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp785,
_tmp782);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp763.ptr_atts).zero_term)){const char*_tmp12D6;void*_tmp12D5;(_tmp12D5=0,
Cyc_Tcutil_warn(loc,((_tmp12D6="implicit cast to shorter array",_tag_dyneither(
_tmp12D6,sizeof(char),31))),_tag_dyneither(_tmp12D5,sizeof(void*),0)));}goto
_LL3BE;_LL3C5: _tmp786=_tmp777.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp787=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp786;if(_tmp787->tag != 0)goto _LL3BE;};
_tmp788=_tmp777.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp789=(struct Cyc_Absyn_Upper_b_struct*)
_tmp788;if(_tmp789->tag != 1)goto _LL3BE;else{_tmp78A=_tmp789->f1;}};_LL3C6: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp760.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp763.ptr_atts).bounds))goto _LL3BE;okay=0;goto
_LL3BE;_LL3BE:;}okay=okay  && (!(_tmp760.elt_tq).real_const  || (_tmp763.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp760.ptr_atts).rgn,(_tmp763.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp760.ptr_atts).rgn,(_tmp763.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp760.ptr_atts).zero_term,(
_tmp763.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp760.ptr_atts).zero_term) && (_tmp763.elt_tq).real_const);{int _tmp78D=((
int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp763.ptr_atts).bounds,
Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp763.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp760.elt_typ,
_tmp763.elt_typ) || (_tmp78D  && ((_tmp763.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp763.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp760.elt_typ,_tmp763.elt_typ));return okay;};}_LL3B8: _tmp764=_tmp75D.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp765=(struct Cyc_Absyn_ArrayType_struct*)
_tmp764;if(_tmp765->tag != 9)goto _LL3BA;else{_tmp766=_tmp765->f1;_tmp767=_tmp766.elt_type;
_tmp768=_tmp766.tq;_tmp769=_tmp766.num_elts;_tmp76A=_tmp766.zero_term;}};_tmp76B=
_tmp75D.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp76C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp76B;if(_tmp76C->tag != 9)goto _LL3BA;else{_tmp76D=_tmp76C->f1;_tmp76E=_tmp76D.elt_type;
_tmp76F=_tmp76D.tq;_tmp770=_tmp76D.num_elts;_tmp771=_tmp76D.zero_term;}};_LL3B9: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp76A,_tmp771) && ((_tmp769 != 0
 && _tmp770 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp769,(
struct Cyc_Absyn_Exp*)_tmp770));return(okay  && Cyc_Tcutil_unify(_tmp767,_tmp76E))
 && (!_tmp768.real_const  || _tmp76F.real_const);}_LL3BA: _tmp772=_tmp75D.f1;{
struct Cyc_Absyn_TagType_struct*_tmp773=(struct Cyc_Absyn_TagType_struct*)_tmp772;
if(_tmp773->tag != 20)goto _LL3BC;};_tmp774=_tmp75D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp775=(struct Cyc_Absyn_IntType_struct*)_tmp774;if(_tmp775->tag != 6)goto _LL3BC;};
_LL3BB: return 0;_LL3BC:;_LL3BD: return Cyc_Tcutil_unify(t1,t2);_LL3B5:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp78E=Cyc_Tcutil_compress(t);_LL3C8: {struct Cyc_Absyn_PointerType_struct*
_tmp78F=(struct Cyc_Absyn_PointerType_struct*)_tmp78E;if(_tmp78F->tag != 5)goto
_LL3CA;}_LL3C9: return 1;_LL3CA:;_LL3CB: return 0;_LL3C7:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp790=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp792;void*
_tmp793;_LL3CD: {struct Cyc_Absyn_PointerType_struct*_tmp791=(struct Cyc_Absyn_PointerType_struct*)
_tmp790;if(_tmp791->tag != 5)goto _LL3CF;else{_tmp792=_tmp791->f1;_tmp793=_tmp792.elt_typ;}}
_LL3CE: return _tmp793;_LL3CF:;_LL3D0: {const char*_tmp12D9;void*_tmp12D8;(_tmp12D8=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12D9="pointer_elt_type",_tag_dyneither(_tmp12D9,sizeof(char),17))),
_tag_dyneither(_tmp12D8,sizeof(void*),0)));}_LL3CC:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp796=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp798;
struct Cyc_Absyn_PtrAtts _tmp799;struct Cyc_Absyn_PtrAtts*_tmp79A;_LL3D2: {struct
Cyc_Absyn_PointerType_struct*_tmp797=(struct Cyc_Absyn_PointerType_struct*)
_tmp796;if(_tmp797->tag != 5)goto _LL3D4;else{_tmp798=_tmp797->f1;_tmp799=_tmp798.ptr_atts;
_tmp79A=(struct Cyc_Absyn_PtrAtts*)&(_tmp797->f1).ptr_atts;}}_LL3D3: return _tmp79A->rgn;
_LL3D4:;_LL3D5: {const char*_tmp12DC;void*_tmp12DB;(_tmp12DB=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DC="pointer_elt_type",
_tag_dyneither(_tmp12DC,sizeof(char),17))),_tag_dyneither(_tmp12DB,sizeof(void*),
0)));}_LL3D1:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp79D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp79F;struct Cyc_Absyn_PtrAtts _tmp7A0;void*_tmp7A1;
_LL3D7: {struct Cyc_Absyn_PointerType_struct*_tmp79E=(struct Cyc_Absyn_PointerType_struct*)
_tmp79D;if(_tmp79E->tag != 5)goto _LL3D9;else{_tmp79F=_tmp79E->f1;_tmp7A0=_tmp79F.ptr_atts;
_tmp7A1=_tmp7A0.rgn;}}_LL3D8:*rgn=_tmp7A1;return 1;_LL3D9:;_LL3DA: return 0;_LL3D6:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp7A2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp7A4;struct Cyc_Absyn_PtrAtts _tmp7A5;union Cyc_Absyn_Constraint*
_tmp7A6;_LL3DC: {struct Cyc_Absyn_PointerType_struct*_tmp7A3=(struct Cyc_Absyn_PointerType_struct*)
_tmp7A2;if(_tmp7A3->tag != 5)goto _LL3DE;else{_tmp7A4=_tmp7A3->f1;_tmp7A5=_tmp7A4.ptr_atts;
_tmp7A6=_tmp7A5.bounds;}}_LL3DD:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7A6)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DE:;_LL3DF: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3DB:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp7A7=e->r;union Cyc_Absyn_Cnst
_tmp7A9;struct _tuple7 _tmp7AA;int _tmp7AB;union Cyc_Absyn_Cnst _tmp7AD;struct _tuple5
_tmp7AE;char _tmp7AF;union Cyc_Absyn_Cnst _tmp7B1;struct _tuple6 _tmp7B2;short _tmp7B3;
union Cyc_Absyn_Cnst _tmp7B5;struct _tuple8 _tmp7B6;long long _tmp7B7;void*_tmp7B9;
struct Cyc_Absyn_Exp*_tmp7BA;_LL3E1: {struct Cyc_Absyn_Const_e_struct*_tmp7A8=(
struct Cyc_Absyn_Const_e_struct*)_tmp7A7;if(_tmp7A8->tag != 0)goto _LL3E3;else{
_tmp7A9=_tmp7A8->f1;if((_tmp7A9.Int_c).tag != 4)goto _LL3E3;_tmp7AA=(struct _tuple7)(
_tmp7A9.Int_c).val;_tmp7AB=_tmp7AA.f2;if(_tmp7AB != 0)goto _LL3E3;}}_LL3E2: goto
_LL3E4;_LL3E3: {struct Cyc_Absyn_Const_e_struct*_tmp7AC=(struct Cyc_Absyn_Const_e_struct*)
_tmp7A7;if(_tmp7AC->tag != 0)goto _LL3E5;else{_tmp7AD=_tmp7AC->f1;if((_tmp7AD.Char_c).tag
!= 2)goto _LL3E5;_tmp7AE=(struct _tuple5)(_tmp7AD.Char_c).val;_tmp7AF=_tmp7AE.f2;
if(_tmp7AF != 0)goto _LL3E5;}}_LL3E4: goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Const_e_struct*
_tmp7B0=(struct Cyc_Absyn_Const_e_struct*)_tmp7A7;if(_tmp7B0->tag != 0)goto _LL3E7;
else{_tmp7B1=_tmp7B0->f1;if((_tmp7B1.Short_c).tag != 3)goto _LL3E7;_tmp7B2=(struct
_tuple6)(_tmp7B1.Short_c).val;_tmp7B3=_tmp7B2.f2;if(_tmp7B3 != 0)goto _LL3E7;}}
_LL3E6: goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Const_e_struct*_tmp7B4=(struct Cyc_Absyn_Const_e_struct*)
_tmp7A7;if(_tmp7B4->tag != 0)goto _LL3E9;else{_tmp7B5=_tmp7B4->f1;if((_tmp7B5.LongLong_c).tag
!= 5)goto _LL3E9;_tmp7B6=(struct _tuple8)(_tmp7B5.LongLong_c).val;_tmp7B7=_tmp7B6.f2;
if(_tmp7B7 != 0)goto _LL3E9;}}_LL3E8: return 1;_LL3E9: {struct Cyc_Absyn_Cast_e_struct*
_tmp7B8=(struct Cyc_Absyn_Cast_e_struct*)_tmp7A7;if(_tmp7B8->tag != 15)goto _LL3EB;
else{_tmp7B9=(void*)_tmp7B8->f1;_tmp7BA=_tmp7B8->f2;}}_LL3EA: return Cyc_Tcutil_is_zero(
_tmp7BA) && Cyc_Tcutil_admits_zero(_tmp7B9);_LL3EB:;_LL3EC: return 0;_LL3E0:;}
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
_tmp7BC;enum Cyc_Absyn_KindQual _tmp7BD;enum Cyc_Absyn_AliasQual _tmp7BE;struct Cyc_Absyn_Kind*
_tmp7BB=ka;_tmp7BC=*_tmp7BB;_tmp7BD=_tmp7BC.kind;_tmp7BE=_tmp7BC.aliasqual;
switch(_tmp7BE){case Cyc_Absyn_Aliasable: _LL3ED: switch(_tmp7BD){case Cyc_Absyn_AnyKind:
_LL3EF: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3F0:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F1: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F2: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F3: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F4: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_iko;}
case Cyc_Absyn_Unique: _LL3EE: switch(_tmp7BD){case Cyc_Absyn_AnyKind: _LL3F7: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F8: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F9: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3FA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;
default: _LL3FB: break;}break;case Cyc_Absyn_Top: _LL3F6: switch(_tmp7BD){case Cyc_Absyn_AnyKind:
_LL3FE: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FF:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL400: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL401: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL402: break;}break;}{const char*_tmp12E0;
void*_tmp12DF[1];struct Cyc_String_pa_struct _tmp12DE;(_tmp12DE.tag=0,((_tmp12DE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((
_tmp12DF[0]=& _tmp12DE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12E0="kind_to_opt: bad kind %s\n",_tag_dyneither(
_tmp12E0,sizeof(char),26))),_tag_dyneither(_tmp12DF,sizeof(void*),1)))))));};}
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
_tmp7C3;enum Cyc_Absyn_KindQual _tmp7C4;enum Cyc_Absyn_AliasQual _tmp7C5;struct Cyc_Absyn_Kind*
_tmp7C2=ka;_tmp7C3=*_tmp7C2;_tmp7C4=_tmp7C3.kind;_tmp7C5=_tmp7C3.aliasqual;
switch(_tmp7C5){case Cyc_Absyn_Aliasable: _LL404: switch(_tmp7C4){case Cyc_Absyn_AnyKind:
_LL406: return& ab;case Cyc_Absyn_MemKind: _LL407: return& mb;case Cyc_Absyn_BoxKind:
_LL408: return& bb;case Cyc_Absyn_RgnKind: _LL409: return& rb;case Cyc_Absyn_EffKind:
_LL40A: return& eb;case Cyc_Absyn_IntKind: _LL40B: return& ib;}case Cyc_Absyn_Unique:
_LL405: switch(_tmp7C4){case Cyc_Absyn_AnyKind: _LL40E: return& uab;case Cyc_Absyn_MemKind:
_LL40F: return& umb;case Cyc_Absyn_BoxKind: _LL410: return& ubb;case Cyc_Absyn_RgnKind:
_LL411: return& urb;default: _LL412: break;}break;case Cyc_Absyn_Top: _LL40D: switch(
_tmp7C4){case Cyc_Absyn_AnyKind: _LL415: return& tab;case Cyc_Absyn_MemKind: _LL416:
return& tmb;case Cyc_Absyn_BoxKind: _LL417: return& tbb;case Cyc_Absyn_RgnKind: _LL418:
return& trb;default: _LL419: break;}break;}{const char*_tmp12E4;void*_tmp12E3[1];
struct Cyc_String_pa_struct _tmp12E2;(_tmp12E2.tag=0,((_tmp12E2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp12E3[0]=&
_tmp12E2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12E4="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp12E4,sizeof(char),24))),
_tag_dyneither(_tmp12E3,sizeof(void*),1)))))));};}void*Cyc_Tcutil_kind_to_bound(
struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
struct Cyc_Absyn_Kind*k){struct Cyc_Core_Opt*_tmp12E5;return(_tmp12E5=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp12E5)),((_tmp12E5->v=Cyc_Tcutil_kind_to_bound(
k),_tmp12E5)));}int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,
struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(
e1)){{struct Cyc_Absyn_Const_e_struct _tmp12E8;struct Cyc_Absyn_Const_e_struct*
_tmp12E7;e1->r=(void*)((_tmp12E7=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp12E7)),((_tmp12E7[0]=((_tmp12E8.tag=0,((_tmp12E8.f1=Cyc_Absyn_Null_c,
_tmp12E8)))),_tmp12E7))));}{struct Cyc_Core_Opt*_tmp7DA=Cyc_Tcenv_lookup_opt_type_vars(
te);struct Cyc_Absyn_PointerType_struct _tmp12F2;struct Cyc_Absyn_PtrAtts _tmp12F1;
struct Cyc_Absyn_PtrInfo _tmp12F0;struct Cyc_Absyn_PointerType_struct*_tmp12EF;
struct Cyc_Absyn_PointerType_struct*_tmp7DB=(_tmp12EF=_cycalloc(sizeof(*_tmp12EF)),((
_tmp12EF[0]=((_tmp12F2.tag=5,((_tmp12F2.f1=((_tmp12F0.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7DA),((_tmp12F0.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp12F0.ptr_atts=((_tmp12F1.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trko,_tmp7DA),((_tmp12F1.nullable=Cyc_Absyn_true_conref,((_tmp12F1.bounds=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12F1.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp12F1.ptrloc=0,
_tmp12F1)))))))))),_tmp12F0)))))),_tmp12F2)))),_tmp12EF)));((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp7DB);{int bogus=0;int retv=Cyc_Tcutil_coerce_arg(
te,e1,t2,& bogus);if(bogus != 0){const char*_tmp12F7;void*_tmp12F6[2];struct Cyc_String_pa_struct
_tmp12F5;struct Cyc_String_pa_struct _tmp12F4;(_tmp12F4.tag=0,((_tmp12F4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((
_tmp12F5.tag=0,((_tmp12F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12F6[0]=& _tmp12F5,((_tmp12F6[1]=& _tmp12F4,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F7="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12F7,sizeof(char),56))),_tag_dyneither(_tmp12F6,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL41B: {
const char*_tmp12F8;return(_tmp12F8="unknown",_tag_dyneither(_tmp12F8,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL41C: {const char*_tmp12F9;return(_tmp12F9="no coercion",
_tag_dyneither(_tmp12F9,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41D: {
const char*_tmp12FA;return(_tmp12FA="null check",_tag_dyneither(_tmp12FA,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL41E: {const char*_tmp12FB;return(
_tmp12FB="other coercion",_tag_dyneither(_tmp12FB,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp1308;const char*_tmp1307;void*_tmp1306[1];struct Cyc_Int_pa_struct
_tmp1305;struct _tuple2*_tmp1304;struct _tuple2*v=(_tmp1304=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1304)),((_tmp1304->f1=Cyc_Absyn_Loc_n,((_tmp1304->f2=((_tmp1308=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1308)),((_tmp1308[0]=(struct
_dyneither_ptr)((_tmp1305.tag=1,((_tmp1305.f1=(unsigned long)counter ++,((
_tmp1306[0]=& _tmp1305,Cyc_aprintf(((_tmp1307="__aliasvar%d",_tag_dyneither(
_tmp1307,sizeof(char),13))),_tag_dyneither(_tmp1306,sizeof(void*),1)))))))),
_tmp1308)))),_tmp1304)))));struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(
void*)& Cyc_Absyn_VoidType_val,0);struct Cyc_Absyn_Local_b_struct _tmp130B;struct
Cyc_Absyn_Local_b_struct*_tmp130A;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(
void*)((_tmp130A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp130A)),((
_tmp130A[0]=((_tmp130B.tag=4,((_tmp130B.f1=vd,_tmp130B)))),_tmp130A)))),e->loc);
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{void*_tmp7E8=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_PtrInfo _tmp7EA;struct Cyc_Absyn_PtrAtts _tmp7EB;void*
_tmp7EC;void**_tmp7ED;_LL421: {struct Cyc_Absyn_PointerType_struct*_tmp7E9=(
struct Cyc_Absyn_PointerType_struct*)_tmp7E8;if(_tmp7E9->tag != 5)goto _LL423;else{
_tmp7EA=_tmp7E9->f1;_tmp7EB=_tmp7EA.ptr_atts;_tmp7EC=_tmp7EB.rgn;_tmp7ED=(void**)&((
_tmp7E9->f1).ptr_atts).rgn;}}_LL422:{struct Cyc_Absyn_VarType_struct _tmp130E;
struct Cyc_Absyn_VarType_struct*_tmp130D;*_tmp7ED=(void*)((_tmp130D=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp130D)),((_tmp130D[0]=((
_tmp130E.tag=2,((_tmp130E.f1=tv,_tmp130E)))),_tmp130D))));}goto _LL420;_LL423:;
_LL424: goto _LL420;_LL420:;}e->topt=0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,
e->loc);{struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct
_tuple14 _tmp130F;return(_tmp130F.f1=d,((_tmp130F.f2=ve,_tmp130F)));};}static int
Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,
struct Cyc_Position_Segment*loc){struct _RegionHandle _tmp7F9=_new_region("r");
struct _RegionHandle*r=& _tmp7F9;_push_region(r);if((Cyc_Tcutil_is_noalias_path(r,
e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(e_typ)){
void*_tmp7FA=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo _tmp7FC;
struct Cyc_Absyn_PtrAtts _tmp7FD;void*_tmp7FE;_LL426: {struct Cyc_Absyn_PointerType_struct*
_tmp7FB=(struct Cyc_Absyn_PointerType_struct*)_tmp7FA;if(_tmp7FB->tag != 5)goto
_LL428;else{_tmp7FC=_tmp7FB->f1;_tmp7FD=_tmp7FC.ptr_atts;_tmp7FE=_tmp7FD.rgn;}}
_LL427: {void*_tmp800=Cyc_Tcutil_compress(_tmp7FE);_LL42D: {struct Cyc_Absyn_HeapRgn_struct*
_tmp801=(struct Cyc_Absyn_HeapRgn_struct*)_tmp800;if(_tmp801->tag != 21)goto _LL42F;}
_LL42E: {int _tmp802=0;_npop_handler(0);return _tmp802;}_LL42F:;_LL430: {struct Cyc_Absyn_Kind*
_tmp803=Cyc_Tcutil_typ_kind(_tmp7FE);int _tmp804=_tmp803->kind == Cyc_Absyn_RgnKind
 && _tmp803->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp804;}
_LL42C:;}_LL428: {struct Cyc_Absyn_TypedefType_struct*_tmp7FF=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7FA;if(_tmp7FF->tag != 18)goto _LL42A;}_LL429: {const char*_tmp1314;void*
_tmp1313[2];struct Cyc_String_pa_struct _tmp1312;struct Cyc_String_pa_struct
_tmp1311;(_tmp1311.tag=0,((_tmp1311.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp1312.tag=0,((_tmp1312.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp1313[0]=& _tmp1312,((_tmp1313[1]=& _tmp1311,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1314="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp1314,sizeof(char),42))),_tag_dyneither(_tmp1313,sizeof(void*),
2)))))))))))));}_LL42A:;_LL42B: {int _tmp809=0;_npop_handler(0);return _tmp809;}
_LL425:;}{int _tmp80A=0;_npop_handler(0);return _tmp80A;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1319;void*_tmp1318[2];
struct Cyc_String_pa_struct _tmp1317;struct Cyc_String_pa_struct _tmp1316;(_tmp1316.tag=
0,((_tmp1316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1317.tag=0,((_tmp1317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1318[0]=& _tmp1317,((_tmp1318[1]=& _tmp1316,Cyc_Tcutil_warn(
e->loc,((_tmp1319="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1319,sizeof(char),53))),_tag_dyneither(_tmp1318,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp131F;void*_tmp131E[3];struct Cyc_String_pa_struct _tmp131D;struct Cyc_String_pa_struct
_tmp131C;struct Cyc_String_pa_struct _tmp131B;(_tmp131B.tag=0,((_tmp131B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp131C.tag=0,((_tmp131C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp131D.tag=0,((_tmp131D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp131E[0]=& _tmp131D,((_tmp131E[1]=& _tmp131C,((
_tmp131E[2]=& _tmp131B,Cyc_Tcutil_warn(e->loc,((_tmp131F="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp131F,sizeof(char),40))),_tag_dyneither(_tmp131E,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp1324;void*_tmp1323[2];
struct Cyc_String_pa_struct _tmp1322;struct Cyc_String_pa_struct _tmp1321;(_tmp1321.tag=
0,((_tmp1321.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1322.tag=0,((_tmp1322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1323[0]=& _tmp1322,((_tmp1323[1]=& _tmp1321,Cyc_Tcutil_warn(
e->loc,((_tmp1324="implicit cast from %s to %s",_tag_dyneither(_tmp1324,sizeof(
char),28))),_tag_dyneither(_tmp1323,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp818=Cyc_Tcutil_compress(t);_LL432: {struct Cyc_Absyn_IntType_struct*
_tmp819=(struct Cyc_Absyn_IntType_struct*)_tmp818;if(_tmp819->tag != 6)goto _LL434;}
_LL433: goto _LL435;_LL434: {struct Cyc_Absyn_FloatType_struct*_tmp81A=(struct Cyc_Absyn_FloatType_struct*)
_tmp818;if(_tmp81A->tag != 7)goto _LL436;}_LL435: goto _LL437;_LL436: {struct Cyc_Absyn_DoubleType_struct*
_tmp81B=(struct Cyc_Absyn_DoubleType_struct*)_tmp818;if(_tmp81B->tag != 8)goto
_LL438;}_LL437: return 1;_LL438:;_LL439: return 0;_LL431:;}static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,
void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*
f3;int f4;};static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,
struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp81D;struct _RegionHandle*
_tmp81E;struct Cyc_Tcenv_Tenv*_tmp81F;int _tmp820;struct _tuple23 _tmp81C=*env;
_tmp81D=_tmp81C.f1;_tmp81E=_tmp81C.f2;_tmp81F=_tmp81C.f3;_tmp820=_tmp81C.f4;{
void*_tmp821=Cyc_Tcutil_rsubstitute(_tmp81E,_tmp81D,x->type);struct Cyc_List_List*
_tmp822=Cyc_Tcutil_flatten_typ(_tmp81E,_tmp820,_tmp81F,_tmp821);if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp822)== 1){struct _tuple11*_tmp1327;
struct Cyc_List_List*_tmp1326;return(_tmp1326=_region_malloc(_tmp81E,sizeof(*
_tmp1326)),((_tmp1326->hd=((_tmp1327=_region_malloc(_tmp81E,sizeof(*_tmp1327)),((
_tmp1327->f1=x->tq,((_tmp1327->f2=_tmp821,_tmp1327)))))),((_tmp1326->tl=0,
_tmp1326)))));}else{return _tmp822;}};}struct _tuple24{struct _RegionHandle*f1;
struct Cyc_Tcenv_Tenv*f2;int f3;};static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(
struct _tuple24*env,struct _tuple11*x){struct _RegionHandle*_tmp826;struct Cyc_Tcenv_Tenv*
_tmp827;int _tmp828;struct _tuple24 _tmp825=*env;_tmp826=_tmp825.f1;_tmp827=_tmp825.f2;
_tmp828=_tmp825.f3;{struct Cyc_Absyn_Tqual _tmp82A;void*_tmp82B;struct _tuple11
_tmp829=*x;_tmp82A=_tmp829.f1;_tmp82B=_tmp829.f2;{struct Cyc_List_List*_tmp82C=
Cyc_Tcutil_flatten_typ(_tmp826,_tmp828,_tmp827,_tmp82B);if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp82C)== 1){struct _tuple11*_tmp132A;struct Cyc_List_List*
_tmp1329;return(_tmp1329=_region_malloc(_tmp826,sizeof(*_tmp1329)),((_tmp1329->hd=((
_tmp132A=_region_malloc(_tmp826,sizeof(*_tmp132A)),((_tmp132A->f1=_tmp82A,((
_tmp132A->f2=_tmp82B,_tmp132A)))))),((_tmp1329->tl=0,_tmp1329)))));}else{return
_tmp82C;}};};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct
_RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(
t1);{void*_tmp82F=t1;struct Cyc_List_List*_tmp832;struct Cyc_Absyn_AggrInfo _tmp834;
union Cyc_Absyn_AggrInfoU _tmp835;struct Cyc_Absyn_Aggrdecl**_tmp836;struct Cyc_Absyn_Aggrdecl*
_tmp837;struct Cyc_List_List*_tmp838;enum Cyc_Absyn_AggrKind _tmp83A;struct Cyc_List_List*
_tmp83B;_LL43B: {struct Cyc_Absyn_VoidType_struct*_tmp830=(struct Cyc_Absyn_VoidType_struct*)
_tmp82F;if(_tmp830->tag != 0)goto _LL43D;}_LL43C: return 0;_LL43D: {struct Cyc_Absyn_TupleType_struct*
_tmp831=(struct Cyc_Absyn_TupleType_struct*)_tmp82F;if(_tmp831->tag != 11)goto
_LL43F;else{_tmp832=_tmp831->f1;}}_LL43E: {struct _tuple24 _tmp132B;struct _tuple24
_tmp83C=(_tmp132B.f1=r,((_tmp132B.f2=te,((_tmp132B.f3=flatten,_tmp132B)))));
struct Cyc_List_List*_tmp83D=_tmp832;struct Cyc_List_List _tmp83E;struct _tuple11*
_tmp83F;struct Cyc_List_List*_tmp840;_LL446: if(_tmp83D != 0)goto _LL448;_LL447:
return 0;_LL448: if(_tmp83D == 0)goto _LL445;_tmp83E=*_tmp83D;_tmp83F=(struct
_tuple11*)_tmp83E.hd;_tmp840=_tmp83E.tl;_LL449: {struct Cyc_List_List*_tmp841=Cyc_Tcutil_rcopy_tqt(&
_tmp83C,_tmp83F);_tmp83C.f3=0;{struct Cyc_List_List*_tmp842=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp83C,_tmp832);struct Cyc_List_List*_tmp132C;struct Cyc_List_List*_tmp843=(
_tmp132C=_region_malloc(r,sizeof(*_tmp132C)),((_tmp132C->hd=_tmp841,((_tmp132C->tl=
_tmp842,_tmp132C)))));struct Cyc_List_List*_tmp132D;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp132D=
_region_malloc(r,sizeof(*_tmp132D)),((_tmp132D->hd=_tmp841,((_tmp132D->tl=
_tmp842,_tmp132D)))))));};}_LL445:;}_LL43F: {struct Cyc_Absyn_AggrType_struct*
_tmp833=(struct Cyc_Absyn_AggrType_struct*)_tmp82F;if(_tmp833->tag != 12)goto
_LL441;else{_tmp834=_tmp833->f1;_tmp835=_tmp834.aggr_info;if((_tmp835.KnownAggr).tag
!= 2)goto _LL441;_tmp836=(struct Cyc_Absyn_Aggrdecl**)(_tmp835.KnownAggr).val;
_tmp837=*_tmp836;_tmp838=_tmp834.targs;}}_LL440: if(((_tmp837->kind == Cyc_Absyn_UnionA
 || _tmp837->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp837->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp837->impl))->rgn_po != 0){
struct _tuple11*_tmp1330;struct Cyc_List_List*_tmp132F;return(_tmp132F=
_region_malloc(r,sizeof(*_tmp132F)),((_tmp132F->hd=((_tmp1330=_region_malloc(r,
sizeof(*_tmp1330)),((_tmp1330->f1=Cyc_Absyn_empty_tqual(0),((_tmp1330->f2=t1,
_tmp1330)))))),((_tmp132F->tl=0,_tmp132F)))));}{struct Cyc_List_List*_tmp849=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp837->tvs,_tmp838);struct _tuple23
_tmp1331;struct _tuple23 env=(_tmp1331.f1=_tmp849,((_tmp1331.f2=r,((_tmp1331.f3=te,((
_tmp1331.f4=flatten,_tmp1331)))))));struct Cyc_List_List*_tmp84A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp837->impl))->fields;struct Cyc_List_List _tmp84B;struct Cyc_Absyn_Aggrfield*
_tmp84C;struct Cyc_List_List*_tmp84D;_LL44B: if(_tmp84A != 0)goto _LL44D;_LL44C:
return 0;_LL44D: if(_tmp84A == 0)goto _LL44A;_tmp84B=*_tmp84A;_tmp84C=(struct Cyc_Absyn_Aggrfield*)
_tmp84B.hd;_tmp84D=_tmp84B.tl;_LL44E: {struct Cyc_List_List*_tmp84E=Cyc_Tcutil_flatten_typ_f(&
env,_tmp84C);env.f4=0;{struct Cyc_List_List*_tmp84F=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp84D);struct Cyc_List_List*_tmp1332;struct Cyc_List_List*_tmp850=(_tmp1332=
_region_malloc(r,sizeof(*_tmp1332)),((_tmp1332->hd=_tmp84E,((_tmp1332->tl=
_tmp84F,_tmp1332)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp850);};}_LL44A:;};_LL441: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp839=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp82F;if(_tmp839->tag != 13)goto _LL443;else{_tmp83A=_tmp839->f1;if(_tmp83A != 
Cyc_Absyn_StructA)goto _LL443;_tmp83B=_tmp839->f2;}}_LL442: {struct _tuple23
_tmp1333;struct _tuple23 env=(_tmp1333.f1=0,((_tmp1333.f2=r,((_tmp1333.f3=te,((
_tmp1333.f4=flatten,_tmp1333)))))));struct Cyc_List_List*_tmp853=_tmp83B;struct
Cyc_List_List _tmp854;struct Cyc_Absyn_Aggrfield*_tmp855;struct Cyc_List_List*
_tmp856;_LL450: if(_tmp853 != 0)goto _LL452;_LL451: return 0;_LL452: if(_tmp853 == 0)
goto _LL44F;_tmp854=*_tmp853;_tmp855=(struct Cyc_Absyn_Aggrfield*)_tmp854.hd;
_tmp856=_tmp854.tl;_LL453: {struct Cyc_List_List*_tmp857=Cyc_Tcutil_flatten_typ_f(&
env,_tmp855);env.f4=0;{struct Cyc_List_List*_tmp858=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),
struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp856);struct Cyc_List_List*_tmp1334;struct Cyc_List_List*_tmp859=(_tmp1334=
_region_malloc(r,sizeof(*_tmp1334)),((_tmp1334->hd=_tmp857,((_tmp1334->tl=
_tmp858,_tmp1334)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp859);};}_LL44F:;}_LL443:;_LL444:
goto _LL43A;_LL43A:;};}{struct _tuple11*_tmp1337;struct Cyc_List_List*_tmp1336;
return(_tmp1336=_region_malloc(r,sizeof(*_tmp1336)),((_tmp1336->hd=((_tmp1337=
_region_malloc(r,sizeof(*_tmp1337)),((_tmp1337->f1=Cyc_Absyn_empty_tqual(0),((
_tmp1337->f2=t1,_tmp1337)))))),((_tmp1336->tl=0,_tmp1336)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp85E=(void*)t->hd;_LL455: {struct Cyc_Absyn_Pure_att_struct*
_tmp85F=(struct Cyc_Absyn_Pure_att_struct*)_tmp85E;if(_tmp85F->tag != 22)goto
_LL457;}_LL456: goto _LL458;_LL457: {struct Cyc_Absyn_Noreturn_att_struct*_tmp860=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp85E;if(_tmp860->tag != 4)goto _LL459;}
_LL458: goto _LL45A;_LL459: {struct Cyc_Absyn_Initializes_att_struct*_tmp861=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp85E;if(_tmp861->tag != 20)goto _LL45B;}
_LL45A: continue;_LL45B:;_LL45C: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL454:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1338;struct _tuple0 _tmp863=(_tmp1338.f1=Cyc_Tcutil_compress(t1),((
_tmp1338.f2=Cyc_Tcutil_compress(t2),_tmp1338)));void*_tmp864;enum Cyc_Absyn_Size_of
_tmp866;void*_tmp867;enum Cyc_Absyn_Size_of _tmp869;_LL45E: _tmp864=_tmp863.f1;{
struct Cyc_Absyn_IntType_struct*_tmp865=(struct Cyc_Absyn_IntType_struct*)_tmp864;
if(_tmp865->tag != 6)goto _LL460;else{_tmp866=_tmp865->f2;}};_tmp867=_tmp863.f2;{
struct Cyc_Absyn_IntType_struct*_tmp868=(struct Cyc_Absyn_IntType_struct*)_tmp867;
if(_tmp868->tag != 6)goto _LL460;else{_tmp869=_tmp868->f2;}};_LL45F: return(_tmp866
== _tmp869  || _tmp866 == Cyc_Absyn_Int_sz  && _tmp869 == Cyc_Absyn_Long_sz) || 
_tmp866 == Cyc_Absyn_Long_sz  && _tmp869 == Cyc_Absyn_Int_sz;_LL460:;_LL461: return 0;
_LL45D:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1339;struct _tuple0 _tmp86B=(
_tmp1339.f1=t1,((_tmp1339.f2=t2,_tmp1339)));void*_tmp86C;struct Cyc_Absyn_PtrInfo
_tmp86E;void*_tmp86F;struct Cyc_Absyn_Tqual _tmp870;struct Cyc_Absyn_PtrAtts _tmp871;
void*_tmp872;union Cyc_Absyn_Constraint*_tmp873;union Cyc_Absyn_Constraint*_tmp874;
union Cyc_Absyn_Constraint*_tmp875;void*_tmp876;struct Cyc_Absyn_PtrInfo _tmp878;
void*_tmp879;struct Cyc_Absyn_Tqual _tmp87A;struct Cyc_Absyn_PtrAtts _tmp87B;void*
_tmp87C;union Cyc_Absyn_Constraint*_tmp87D;union Cyc_Absyn_Constraint*_tmp87E;
union Cyc_Absyn_Constraint*_tmp87F;void*_tmp880;struct Cyc_Absyn_DatatypeFieldInfo
_tmp882;union Cyc_Absyn_DatatypeFieldInfoU _tmp883;struct _tuple3 _tmp884;struct Cyc_Absyn_Datatypedecl*
_tmp885;struct Cyc_Absyn_Datatypefield*_tmp886;struct Cyc_List_List*_tmp887;void*
_tmp888;struct Cyc_Absyn_DatatypeInfo _tmp88A;union Cyc_Absyn_DatatypeInfoU _tmp88B;
struct Cyc_Absyn_Datatypedecl**_tmp88C;struct Cyc_Absyn_Datatypedecl*_tmp88D;
struct Cyc_List_List*_tmp88E;void*_tmp88F;struct Cyc_Absyn_FnInfo _tmp891;void*
_tmp892;struct Cyc_Absyn_FnInfo _tmp894;_LL463: _tmp86C=_tmp86B.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp86D=(struct Cyc_Absyn_PointerType_struct*)_tmp86C;if(_tmp86D->tag != 5)goto
_LL465;else{_tmp86E=_tmp86D->f1;_tmp86F=_tmp86E.elt_typ;_tmp870=_tmp86E.elt_tq;
_tmp871=_tmp86E.ptr_atts;_tmp872=_tmp871.rgn;_tmp873=_tmp871.nullable;_tmp874=
_tmp871.bounds;_tmp875=_tmp871.zero_term;}};_tmp876=_tmp86B.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp877=(struct Cyc_Absyn_PointerType_struct*)_tmp876;if(_tmp877->tag != 5)goto
_LL465;else{_tmp878=_tmp877->f1;_tmp879=_tmp878.elt_typ;_tmp87A=_tmp878.elt_tq;
_tmp87B=_tmp878.ptr_atts;_tmp87C=_tmp87B.rgn;_tmp87D=_tmp87B.nullable;_tmp87E=
_tmp87B.bounds;_tmp87F=_tmp87B.zero_term;}};_LL464: if(_tmp870.real_const  && !
_tmp87A.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp873,
_tmp87D) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp873)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87D))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp875,
_tmp87F) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp875)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87F))return 0;if(!Cyc_Tcutil_unify(_tmp872,_tmp87C) && !Cyc_Tcenv_region_outlives(
te,_tmp872,_tmp87C))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp874,_tmp87E)){struct _tuple0 _tmp133A;struct _tuple0 _tmp896=(_tmp133A.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp874),((_tmp133A.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp87E),_tmp133A)));void*
_tmp897;void*_tmp899;void*_tmp89B;struct Cyc_Absyn_Exp*_tmp89D;void*_tmp89E;
struct Cyc_Absyn_Exp*_tmp8A0;_LL46C: _tmp897=_tmp896.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp898=(struct Cyc_Absyn_Upper_b_struct*)_tmp897;if(_tmp898->tag != 1)goto _LL46E;};
_tmp899=_tmp896.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp89A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp899;if(_tmp89A->tag != 0)goto _LL46E;};_LL46D: goto _LL46B;_LL46E: _tmp89B=
_tmp896.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp89C=(struct Cyc_Absyn_Upper_b_struct*)
_tmp89B;if(_tmp89C->tag != 1)goto _LL470;else{_tmp89D=_tmp89C->f1;}};_tmp89E=
_tmp896.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp89F=(struct Cyc_Absyn_Upper_b_struct*)
_tmp89E;if(_tmp89F->tag != 1)goto _LL470;else{_tmp8A0=_tmp89F->f1;}};_LL46F: if(!
Cyc_Evexp_lte_const_exp(_tmp8A0,_tmp89D))return 0;goto _LL46B;_LL470:;_LL471:
return 0;_LL46B:;}if(!_tmp87A.real_const  && _tmp870.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp879)))return 0;}{int _tmp8A1=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp87E,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp87F);struct _tuple0*
_tmp133D;struct Cyc_List_List*_tmp133C;return(_tmp8A1  && Cyc_Tcutil_ptrsubtype(te,((
_tmp133C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp133C)),((_tmp133C->hd=((
_tmp133D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp133D)),((_tmp133D->f1=
t1,((_tmp133D->f2=t2,_tmp133D)))))),((_tmp133C->tl=assume,_tmp133C)))))),_tmp86F,
_tmp879) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(t1,t2);};_LL465:
_tmp880=_tmp86B.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp881=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp880;if(_tmp881->tag != 4)goto _LL467;else{_tmp882=_tmp881->f1;_tmp883=_tmp882.field_info;
if((_tmp883.KnownDatatypefield).tag != 2)goto _LL467;_tmp884=(struct _tuple3)(
_tmp883.KnownDatatypefield).val;_tmp885=_tmp884.f1;_tmp886=_tmp884.f2;_tmp887=
_tmp882.targs;}};_tmp888=_tmp86B.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmp889=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp888;if(_tmp889->tag != 3)goto _LL467;
else{_tmp88A=_tmp889->f1;_tmp88B=_tmp88A.datatype_info;if((_tmp88B.KnownDatatype).tag
!= 2)goto _LL467;_tmp88C=(struct Cyc_Absyn_Datatypedecl**)(_tmp88B.KnownDatatype).val;
_tmp88D=*_tmp88C;_tmp88E=_tmp88A.targs;}};_LL466: if(_tmp885 != _tmp88D  && Cyc_Absyn_qvar_cmp(
_tmp885->name,_tmp88D->name)!= 0)return 0;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp887)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp88E))return 0;
for(0;_tmp887 != 0;(_tmp887=_tmp887->tl,_tmp88E=_tmp88E->tl)){if(!Cyc_Tcutil_unify((
void*)_tmp887->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp88E))->hd))
return 0;}return 1;_LL467: _tmp88F=_tmp86B.f1;{struct Cyc_Absyn_FnType_struct*
_tmp890=(struct Cyc_Absyn_FnType_struct*)_tmp88F;if(_tmp890->tag != 10)goto _LL469;
else{_tmp891=_tmp890->f1;}};_tmp892=_tmp86B.f2;{struct Cyc_Absyn_FnType_struct*
_tmp893=(struct Cyc_Absyn_FnType_struct*)_tmp892;if(_tmp893->tag != 10)goto _LL469;
else{_tmp894=_tmp893->f1;}};_LL468: if(_tmp891.tvars != 0  || _tmp894.tvars != 0){
struct Cyc_List_List*_tmp8A4=_tmp891.tvars;struct Cyc_List_List*_tmp8A5=_tmp894.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A4)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp8A5))return 0;{struct _RegionHandle*_tmp8A6=
Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;while(_tmp8A4 != 0){if(!Cyc_Tcutil_unify_kindbound(((
struct Cyc_Absyn_Tvar*)_tmp8A4->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp8A5))->hd)->kind))return 0;{struct _tuple15*_tmp1347;struct Cyc_Absyn_VarType_struct
_tmp1346;struct Cyc_Absyn_VarType_struct*_tmp1345;struct Cyc_List_List*_tmp1344;
inst=((_tmp1344=_region_malloc(_tmp8A6,sizeof(*_tmp1344)),((_tmp1344->hd=((
_tmp1347=_region_malloc(_tmp8A6,sizeof(*_tmp1347)),((_tmp1347->f1=(struct Cyc_Absyn_Tvar*)
_tmp8A5->hd,((_tmp1347->f2=(void*)((_tmp1345=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1345)),((_tmp1345[0]=((_tmp1346.tag=2,((_tmp1346.f1=(struct Cyc_Absyn_Tvar*)
_tmp8A4->hd,_tmp1346)))),_tmp1345)))),_tmp1347)))))),((_tmp1344->tl=inst,
_tmp1344))))));}_tmp8A4=_tmp8A4->tl;_tmp8A5=_tmp8A5->tl;}if(inst != 0){_tmp891.tvars=
0;_tmp894.tvars=0;{struct Cyc_Absyn_FnType_struct _tmp134D;struct Cyc_Absyn_FnType_struct*
_tmp134C;struct Cyc_Absyn_FnType_struct _tmp134A;struct Cyc_Absyn_FnType_struct*
_tmp1349;return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1349=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1349)),((_tmp1349[0]=((_tmp134A.tag=10,((_tmp134A.f1=_tmp891,
_tmp134A)))),_tmp1349)))),(void*)((_tmp134C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp134C)),((_tmp134C[0]=((_tmp134D.tag=10,((_tmp134D.f1=_tmp894,
_tmp134D)))),_tmp134C)))));};}};}if(!Cyc_Tcutil_subtype(te,assume,_tmp891.ret_typ,
_tmp894.ret_typ))return 0;{struct Cyc_List_List*_tmp8AF=_tmp891.args;struct Cyc_List_List*
_tmp8B0=_tmp894.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AF)
!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8B0))return 0;for(0;
_tmp8AF != 0;(_tmp8AF=_tmp8AF->tl,_tmp8B0=_tmp8B0->tl)){struct Cyc_Absyn_Tqual
_tmp8B2;void*_tmp8B3;struct _tuple9 _tmp8B1=*((struct _tuple9*)_tmp8AF->hd);_tmp8B2=
_tmp8B1.f2;_tmp8B3=_tmp8B1.f3;{struct Cyc_Absyn_Tqual _tmp8B5;void*_tmp8B6;struct
_tuple9 _tmp8B4=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp8B0))->hd);
_tmp8B5=_tmp8B4.f2;_tmp8B6=_tmp8B4.f3;if(_tmp8B5.real_const  && !_tmp8B2.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp8B6,_tmp8B3))return 0;};}if(_tmp891.c_varargs
!= _tmp894.c_varargs)return 0;if(_tmp891.cyc_varargs != 0  && _tmp894.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp8B7=*_tmp891.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp8B8=*_tmp894.cyc_varargs;if((_tmp8B8.tq).real_const  && !(_tmp8B7.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp8B8.type,_tmp8B7.type))return 0;}else{if(
_tmp891.cyc_varargs != 0  || _tmp894.cyc_varargs != 0)return 0;}if(!Cyc_Tcutil_subset_effect(
1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp891.effect))->v,(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp894.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp891.rgn_po,_tmp894.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp891.attributes,
_tmp894.attributes))return 0;return 1;};_LL469:;_LL46A: return 0;_LL462:;};}static
int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*
t1,void*t2){struct _RegionHandle*_tmp8B9=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*
tqs1=Cyc_Tcutil_flatten_typ(_tmp8B9,1,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(
_tmp8B9,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0)
return 0;{struct _tuple11 _tmp8BB;struct Cyc_Absyn_Tqual _tmp8BC;void*_tmp8BD;struct
_tuple11*_tmp8BA=(struct _tuple11*)tqs1->hd;_tmp8BB=*_tmp8BA;_tmp8BC=_tmp8BB.f1;
_tmp8BD=_tmp8BB.f2;{struct _tuple11 _tmp8BF;struct Cyc_Absyn_Tqual _tmp8C0;void*
_tmp8C1;struct _tuple11*_tmp8BE=(struct _tuple11*)tqs2->hd;_tmp8BF=*_tmp8BE;
_tmp8C0=_tmp8BF.f1;_tmp8C1=_tmp8BF.f2;if(_tmp8BC.real_const  && !_tmp8C0.real_const)
return 0;if((_tmp8C0.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8C1))) && Cyc_Tcutil_subtype(te,assume,_tmp8BD,_tmp8C1))continue;if(Cyc_Tcutil_unify(
_tmp8BD,_tmp8C1))continue;if(Cyc_Tcutil_isomorphic(_tmp8BD,_tmp8C1))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8C2=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8C4;_LL473: {struct Cyc_Absyn_IntType_struct*_tmp8C3=(
struct Cyc_Absyn_IntType_struct*)_tmp8C2;if(_tmp8C3->tag != 6)goto _LL475;else{
_tmp8C4=_tmp8C3->f2;if(_tmp8C4 != Cyc_Absyn_Char_sz)goto _LL475;}}_LL474: return 1;
_LL475:;_LL476: return 0;_LL472:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8C5=t2;enum Cyc_Absyn_Size_of _tmp8C7;enum Cyc_Absyn_Size_of _tmp8C9;_LL478: {
struct Cyc_Absyn_IntType_struct*_tmp8C6=(struct Cyc_Absyn_IntType_struct*)_tmp8C5;
if(_tmp8C6->tag != 6)goto _LL47A;else{_tmp8C7=_tmp8C6->f2;if(_tmp8C7 != Cyc_Absyn_Int_sz)
goto _LL47A;}}_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_IntType_struct*_tmp8C8=(
struct Cyc_Absyn_IntType_struct*)_tmp8C5;if(_tmp8C8->tag != 6)goto _LL47C;else{
_tmp8C9=_tmp8C8->f2;if(_tmp8C9 != Cyc_Absyn_Long_sz)goto _LL47C;}}_LL47B: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL477;_LL47C:;
_LL47D: goto _LL477;_LL477:;}{void*_tmp8CA=t1;struct Cyc_Absyn_PtrInfo _tmp8CC;void*
_tmp8CD;struct Cyc_Absyn_Tqual _tmp8CE;struct Cyc_Absyn_PtrAtts _tmp8CF;void*_tmp8D0;
union Cyc_Absyn_Constraint*_tmp8D1;union Cyc_Absyn_Constraint*_tmp8D2;union Cyc_Absyn_Constraint*
_tmp8D3;struct Cyc_Absyn_ArrayInfo _tmp8D5;void*_tmp8D6;struct Cyc_Absyn_Tqual
_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D8;union Cyc_Absyn_Constraint*_tmp8D9;struct Cyc_Absyn_Enumdecl*
_tmp8DB;void*_tmp8E0;_LL47F: {struct Cyc_Absyn_PointerType_struct*_tmp8CB=(struct
Cyc_Absyn_PointerType_struct*)_tmp8CA;if(_tmp8CB->tag != 5)goto _LL481;else{
_tmp8CC=_tmp8CB->f1;_tmp8CD=_tmp8CC.elt_typ;_tmp8CE=_tmp8CC.elt_tq;_tmp8CF=
_tmp8CC.ptr_atts;_tmp8D0=_tmp8CF.rgn;_tmp8D1=_tmp8CF.nullable;_tmp8D2=_tmp8CF.bounds;
_tmp8D3=_tmp8CF.zero_term;}}_LL480:{void*_tmp8E1=t2;struct Cyc_Absyn_PtrInfo
_tmp8E3;void*_tmp8E4;struct Cyc_Absyn_Tqual _tmp8E5;struct Cyc_Absyn_PtrAtts _tmp8E6;
void*_tmp8E7;union Cyc_Absyn_Constraint*_tmp8E8;union Cyc_Absyn_Constraint*_tmp8E9;
union Cyc_Absyn_Constraint*_tmp8EA;_LL490: {struct Cyc_Absyn_PointerType_struct*
_tmp8E2=(struct Cyc_Absyn_PointerType_struct*)_tmp8E1;if(_tmp8E2->tag != 5)goto
_LL492;else{_tmp8E3=_tmp8E2->f1;_tmp8E4=_tmp8E3.elt_typ;_tmp8E5=_tmp8E3.elt_tq;
_tmp8E6=_tmp8E3.ptr_atts;_tmp8E7=_tmp8E6.rgn;_tmp8E8=_tmp8E6.nullable;_tmp8E9=
_tmp8E6.bounds;_tmp8EA=_tmp8E6.zero_term;}}_LL491: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1350;struct Cyc_List_List*
_tmp134F;struct Cyc_List_List*_tmp8EB=(_tmp134F=_cycalloc(sizeof(*_tmp134F)),((
_tmp134F->hd=((_tmp1350=_cycalloc(sizeof(*_tmp1350)),((_tmp1350->f1=t1,((
_tmp1350->f2=t2,_tmp1350)))))),((_tmp134F->tl=0,_tmp134F)))));int _tmp8EC=_tmp8E5.real_const
 || !_tmp8CE.real_const;int _tmp8ED=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8E9,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8EA);int _tmp8EE=_tmp8EC  && (((_tmp8ED  && Cyc_Tcutil_ptrsubtype(
te,_tmp8EB,_tmp8CD,_tmp8E4) || Cyc_Tcutil_unify(_tmp8CD,_tmp8E4)) || Cyc_Tcutil_isomorphic(
_tmp8CD,_tmp8E4)) || Cyc_Tcutil_unify(_tmp8E4,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8D3,_tmp8EA) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8EA);int _tmp8EF=_tmp8EE?0:((Cyc_Tcutil_bits_only(_tmp8CD) && Cyc_Tcutil_is_char_type(
_tmp8E4)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8EA)) && (_tmp8E5.real_const  || !_tmp8CE.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D2,_tmp8E9);if(!
bounds_ok  && !_tmp8EF){struct _tuple0 _tmp1351;struct _tuple0 _tmp8F1=(_tmp1351.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D2),((_tmp1351.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E9),_tmp1351)));
void*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F4;void*_tmp8F5;struct Cyc_Absyn_Exp*
_tmp8F7;_LL495: _tmp8F2=_tmp8F1.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8F3=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8F2;if(_tmp8F3->tag != 1)goto _LL497;else{
_tmp8F4=_tmp8F3->f1;}};_tmp8F5=_tmp8F1.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8F6=(struct Cyc_Absyn_Upper_b_struct*)_tmp8F5;if(_tmp8F6->tag != 1)goto _LL497;
else{_tmp8F7=_tmp8F6->f1;}};_LL496: if(Cyc_Evexp_lte_const_exp(_tmp8F7,_tmp8F4))
bounds_ok=1;goto _LL494;_LL497:;_LL498: bounds_ok=1;goto _LL494;_LL494:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8D1) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E8))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8EE  || _tmp8EF)) && (Cyc_Tcutil_unify(
_tmp8D0,_tmp8E7) || Cyc_Tcenv_region_outlives(te,_tmp8D0,_tmp8E7)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493: goto _LL48F;
_LL48F:;}return Cyc_Absyn_Unknown_coercion;_LL481: {struct Cyc_Absyn_ArrayType_struct*
_tmp8D4=(struct Cyc_Absyn_ArrayType_struct*)_tmp8CA;if(_tmp8D4->tag != 9)goto
_LL483;else{_tmp8D5=_tmp8D4->f1;_tmp8D6=_tmp8D5.elt_type;_tmp8D7=_tmp8D5.tq;
_tmp8D8=_tmp8D5.num_elts;_tmp8D9=_tmp8D5.zero_term;}}_LL482:{void*_tmp8FA=t2;
struct Cyc_Absyn_ArrayInfo _tmp8FC;void*_tmp8FD;struct Cyc_Absyn_Tqual _tmp8FE;
struct Cyc_Absyn_Exp*_tmp8FF;union Cyc_Absyn_Constraint*_tmp900;_LL49A: {struct Cyc_Absyn_ArrayType_struct*
_tmp8FB=(struct Cyc_Absyn_ArrayType_struct*)_tmp8FA;if(_tmp8FB->tag != 9)goto
_LL49C;else{_tmp8FC=_tmp8FB->f1;_tmp8FD=_tmp8FC.elt_type;_tmp8FE=_tmp8FC.tq;
_tmp8FF=_tmp8FC.num_elts;_tmp900=_tmp8FC.zero_term;}}_LL49B: {int okay;okay=((
_tmp8D8 != 0  && _tmp8FF != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D9,
_tmp900)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8FF,(struct Cyc_Absyn_Exp*)
_tmp8D8);return(okay  && Cyc_Tcutil_unify(_tmp8D6,_tmp8FD)) && (!_tmp8D7.real_const
 || _tmp8FE.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL49C:;_LL49D: return Cyc_Absyn_Unknown_coercion;_LL499:;}return Cyc_Absyn_Unknown_coercion;
_LL483: {struct Cyc_Absyn_EnumType_struct*_tmp8DA=(struct Cyc_Absyn_EnumType_struct*)
_tmp8CA;if(_tmp8DA->tag != 14)goto _LL485;else{_tmp8DB=_tmp8DA->f2;}}_LL484:{void*
_tmp901=t2;struct Cyc_Absyn_Enumdecl*_tmp903;_LL49F: {struct Cyc_Absyn_EnumType_struct*
_tmp902=(struct Cyc_Absyn_EnumType_struct*)_tmp901;if(_tmp902->tag != 14)goto
_LL4A1;else{_tmp903=_tmp902->f2;}}_LL4A0: if((_tmp8DB->fields != 0  && _tmp903->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8DB->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp903->fields))->v))return Cyc_Absyn_No_coercion;goto _LL49E;_LL4A1:;_LL4A2: goto
_LL49E;_LL49E:;}goto _LL486;_LL485: {struct Cyc_Absyn_IntType_struct*_tmp8DC=(
struct Cyc_Absyn_IntType_struct*)_tmp8CA;if(_tmp8DC->tag != 6)goto _LL487;}_LL486:
goto _LL488;_LL487: {struct Cyc_Absyn_FloatType_struct*_tmp8DD=(struct Cyc_Absyn_FloatType_struct*)
_tmp8CA;if(_tmp8DD->tag != 7)goto _LL489;}_LL488: goto _LL48A;_LL489: {struct Cyc_Absyn_DoubleType_struct*
_tmp8DE=(struct Cyc_Absyn_DoubleType_struct*)_tmp8CA;if(_tmp8DE->tag != 8)goto
_LL48B;}_LL48A: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL48B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp8DF=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp8CA;if(_tmp8DF->tag != 16)goto _LL48D;else{_tmp8E0=(void*)_tmp8DF->f1;}}_LL48C:{
void*_tmp904=t2;void*_tmp906;_LL4A4: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp905=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp904;if(_tmp905->tag != 16)goto
_LL4A6;else{_tmp906=(void*)_tmp905->f1;}}_LL4A5: if(Cyc_Tcenv_region_outlives(te,
_tmp8E0,_tmp906))return Cyc_Absyn_No_coercion;goto _LL4A3;_LL4A6:;_LL4A7: goto
_LL4A3;_LL4A3:;}return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E: return Cyc_Absyn_Unknown_coercion;
_LL47E:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp907=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmp1354;struct Cyc_Absyn_Cast_e_struct*_tmp1353;e->r=(
void*)((_tmp1353=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1353)),((
_tmp1353[0]=((_tmp1354.tag=15,((_tmp1354.f1=(void*)t,((_tmp1354.f2=_tmp907,((
_tmp1354.f3=0,((_tmp1354.f4=c,_tmp1354)))))))))),_tmp1353))));}{struct Cyc_Core_Opt*
_tmp1355;e->topt=((_tmp1355=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1355)),((_tmp1355->v=t,_tmp1355))));};}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*
e){void*_tmp90B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL4A9: {struct Cyc_Absyn_IntType_struct*_tmp90C=(struct Cyc_Absyn_IntType_struct*)
_tmp90B;if(_tmp90C->tag != 6)goto _LL4AB;}_LL4AA: goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_struct*
_tmp90D=(struct Cyc_Absyn_EnumType_struct*)_tmp90B;if(_tmp90D->tag != 14)goto
_LL4AD;}_LL4AC: goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_struct*_tmp90E=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp90B;if(_tmp90E->tag != 15)goto _LL4AF;}
_LL4AE: goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_struct*_tmp90F=(struct Cyc_Absyn_TagType_struct*)
_tmp90B;if(_tmp90F->tag != 20)goto _LL4B1;}_LL4B0: return 1;_LL4B1: {struct Cyc_Absyn_Evar_struct*
_tmp910=(struct Cyc_Absyn_Evar_struct*)_tmp90B;if(_tmp910->tag != 1)goto _LL4B3;}
_LL4B2: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4: return 0;_LL4A8:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp911=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL4B6: {struct Cyc_Absyn_FloatType_struct*
_tmp912=(struct Cyc_Absyn_FloatType_struct*)_tmp911;if(_tmp912->tag != 7)goto
_LL4B8;}_LL4B7: goto _LL4B9;_LL4B8: {struct Cyc_Absyn_DoubleType_struct*_tmp913=(
struct Cyc_Absyn_DoubleType_struct*)_tmp911;if(_tmp913->tag != 8)goto _LL4BA;}
_LL4B9: return 1;_LL4BA:;_LL4BB: return 0;_LL4B5:;};}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp914=Cyc_Tcutil_compress(t);_LL4BD: {struct Cyc_Absyn_FnType_struct*
_tmp915=(struct Cyc_Absyn_FnType_struct*)_tmp914;if(_tmp915->tag != 10)goto _LL4BF;}
_LL4BE: return 1;_LL4BF:;_LL4C0: return 0;_LL4BC:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple0 _tmp1356;struct _tuple0 _tmp917=(_tmp1356.f1=t1,((
_tmp1356.f2=t2,_tmp1356)));void*_tmp918;int _tmp91A;void*_tmp91B;int _tmp91D;void*
_tmp91E;void*_tmp920;void*_tmp922;void*_tmp924;void*_tmp926;enum Cyc_Absyn_Sign
_tmp928;enum Cyc_Absyn_Size_of _tmp929;void*_tmp92A;enum Cyc_Absyn_Sign _tmp92C;
enum Cyc_Absyn_Size_of _tmp92D;void*_tmp92E;enum Cyc_Absyn_Size_of _tmp930;void*
_tmp931;enum Cyc_Absyn_Size_of _tmp933;void*_tmp934;enum Cyc_Absyn_Sign _tmp936;
enum Cyc_Absyn_Size_of _tmp937;void*_tmp938;enum Cyc_Absyn_Sign _tmp93A;enum Cyc_Absyn_Size_of
_tmp93B;void*_tmp93C;enum Cyc_Absyn_Sign _tmp93E;enum Cyc_Absyn_Size_of _tmp93F;
void*_tmp940;enum Cyc_Absyn_Sign _tmp942;enum Cyc_Absyn_Size_of _tmp943;void*
_tmp944;enum Cyc_Absyn_Size_of _tmp946;void*_tmp947;enum Cyc_Absyn_Size_of _tmp949;
_LL4C2: _tmp918=_tmp917.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp919=(struct Cyc_Absyn_DoubleType_struct*)
_tmp918;if(_tmp919->tag != 8)goto _LL4C4;else{_tmp91A=_tmp919->f1;}};_tmp91B=
_tmp917.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp91C=(struct Cyc_Absyn_DoubleType_struct*)
_tmp91B;if(_tmp91C->tag != 8)goto _LL4C4;else{_tmp91D=_tmp91C->f1;}};_LL4C3: if(
_tmp91A)return t1;else{return t2;}_LL4C4: _tmp91E=_tmp917.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp91F=(struct Cyc_Absyn_DoubleType_struct*)_tmp91E;if(_tmp91F->tag != 8)goto
_LL4C6;};_LL4C5: return t1;_LL4C6: _tmp920=_tmp917.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp921=(struct Cyc_Absyn_DoubleType_struct*)_tmp920;if(_tmp921->tag != 8)goto
_LL4C8;};_LL4C7: return t2;_LL4C8: _tmp922=_tmp917.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp923=(struct Cyc_Absyn_FloatType_struct*)_tmp922;if(_tmp923->tag != 7)goto
_LL4CA;};_LL4C9: goto _LL4CB;_LL4CA: _tmp924=_tmp917.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp925=(struct Cyc_Absyn_FloatType_struct*)_tmp924;if(_tmp925->tag != 7)goto
_LL4CC;};_LL4CB: return(void*)& Cyc_Absyn_FloatType_val;_LL4CC: _tmp926=_tmp917.f1;{
struct Cyc_Absyn_IntType_struct*_tmp927=(struct Cyc_Absyn_IntType_struct*)_tmp926;
if(_tmp927->tag != 6)goto _LL4CE;else{_tmp928=_tmp927->f1;if(_tmp928 != Cyc_Absyn_Unsigned)
goto _LL4CE;_tmp929=_tmp927->f2;if(_tmp929 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}};
_LL4CD: goto _LL4CF;_LL4CE: _tmp92A=_tmp917.f2;{struct Cyc_Absyn_IntType_struct*
_tmp92B=(struct Cyc_Absyn_IntType_struct*)_tmp92A;if(_tmp92B->tag != 6)goto _LL4D0;
else{_tmp92C=_tmp92B->f1;if(_tmp92C != Cyc_Absyn_Unsigned)goto _LL4D0;_tmp92D=
_tmp92B->f2;if(_tmp92D != Cyc_Absyn_LongLong_sz)goto _LL4D0;}};_LL4CF: return Cyc_Absyn_ulonglong_typ;
_LL4D0: _tmp92E=_tmp917.f1;{struct Cyc_Absyn_IntType_struct*_tmp92F=(struct Cyc_Absyn_IntType_struct*)
_tmp92E;if(_tmp92F->tag != 6)goto _LL4D2;else{_tmp930=_tmp92F->f2;if(_tmp930 != Cyc_Absyn_LongLong_sz)
goto _LL4D2;}};_LL4D1: goto _LL4D3;_LL4D2: _tmp931=_tmp917.f2;{struct Cyc_Absyn_IntType_struct*
_tmp932=(struct Cyc_Absyn_IntType_struct*)_tmp931;if(_tmp932->tag != 6)goto _LL4D4;
else{_tmp933=_tmp932->f2;if(_tmp933 != Cyc_Absyn_LongLong_sz)goto _LL4D4;}};_LL4D3:
return Cyc_Absyn_slonglong_typ;_LL4D4: _tmp934=_tmp917.f1;{struct Cyc_Absyn_IntType_struct*
_tmp935=(struct Cyc_Absyn_IntType_struct*)_tmp934;if(_tmp935->tag != 6)goto _LL4D6;
else{_tmp936=_tmp935->f1;if(_tmp936 != Cyc_Absyn_Unsigned)goto _LL4D6;_tmp937=
_tmp935->f2;if(_tmp937 != Cyc_Absyn_Long_sz)goto _LL4D6;}};_LL4D5: goto _LL4D7;
_LL4D6: _tmp938=_tmp917.f2;{struct Cyc_Absyn_IntType_struct*_tmp939=(struct Cyc_Absyn_IntType_struct*)
_tmp938;if(_tmp939->tag != 6)goto _LL4D8;else{_tmp93A=_tmp939->f1;if(_tmp93A != Cyc_Absyn_Unsigned)
goto _LL4D8;_tmp93B=_tmp939->f2;if(_tmp93B != Cyc_Absyn_Long_sz)goto _LL4D8;}};
_LL4D7: return Cyc_Absyn_ulong_typ;_LL4D8: _tmp93C=_tmp917.f1;{struct Cyc_Absyn_IntType_struct*
_tmp93D=(struct Cyc_Absyn_IntType_struct*)_tmp93C;if(_tmp93D->tag != 6)goto _LL4DA;
else{_tmp93E=_tmp93D->f1;if(_tmp93E != Cyc_Absyn_Unsigned)goto _LL4DA;_tmp93F=
_tmp93D->f2;if(_tmp93F != Cyc_Absyn_Int_sz)goto _LL4DA;}};_LL4D9: goto _LL4DB;_LL4DA:
_tmp940=_tmp917.f2;{struct Cyc_Absyn_IntType_struct*_tmp941=(struct Cyc_Absyn_IntType_struct*)
_tmp940;if(_tmp941->tag != 6)goto _LL4DC;else{_tmp942=_tmp941->f1;if(_tmp942 != Cyc_Absyn_Unsigned)
goto _LL4DC;_tmp943=_tmp941->f2;if(_tmp943 != Cyc_Absyn_Int_sz)goto _LL4DC;}};
_LL4DB: return Cyc_Absyn_uint_typ;_LL4DC: _tmp944=_tmp917.f1;{struct Cyc_Absyn_IntType_struct*
_tmp945=(struct Cyc_Absyn_IntType_struct*)_tmp944;if(_tmp945->tag != 6)goto _LL4DE;
else{_tmp946=_tmp945->f2;if(_tmp946 != Cyc_Absyn_Long_sz)goto _LL4DE;}};_LL4DD:
goto _LL4DF;_LL4DE: _tmp947=_tmp917.f2;{struct Cyc_Absyn_IntType_struct*_tmp948=(
struct Cyc_Absyn_IntType_struct*)_tmp947;if(_tmp948->tag != 6)goto _LL4E0;else{
_tmp949=_tmp948->f2;if(_tmp949 != Cyc_Absyn_Long_sz)goto _LL4E0;}};_LL4DF: return
Cyc_Absyn_slong_typ;_LL4E0:;_LL4E1: return Cyc_Absyn_sint_typ;_LL4C1:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp94A=e->r;struct Cyc_Core_Opt*_tmp94C;_LL4E3: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp94B=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp94A;if(_tmp94B->tag != 4)goto _LL4E5;else{_tmp94C=_tmp94B->f2;if(_tmp94C != 0)
goto _LL4E5;}}_LL4E4:{const char*_tmp1359;void*_tmp1358;(_tmp1358=0,Cyc_Tcutil_warn(
e->loc,((_tmp1359="assignment in test",_tag_dyneither(_tmp1359,sizeof(char),19))),
_tag_dyneither(_tmp1358,sizeof(void*),0)));}goto _LL4E2;_LL4E5:;_LL4E6: goto _LL4E2;
_LL4E2:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp135A;struct _tuple0 _tmp950=(
_tmp135A.f1=c1,((_tmp135A.f2=c2,_tmp135A)));void*_tmp951;struct Cyc_Absyn_Kind*
_tmp953;void*_tmp954;struct Cyc_Absyn_Kind*_tmp956;void*_tmp957;struct Cyc_Core_Opt*
_tmp959;struct Cyc_Core_Opt**_tmp95A;void*_tmp95B;struct Cyc_Core_Opt*_tmp95D;
struct Cyc_Core_Opt**_tmp95E;void*_tmp95F;struct Cyc_Core_Opt*_tmp961;struct Cyc_Core_Opt**
_tmp962;struct Cyc_Absyn_Kind*_tmp963;void*_tmp964;struct Cyc_Absyn_Kind*_tmp966;
void*_tmp967;struct Cyc_Absyn_Kind*_tmp969;void*_tmp96A;struct Cyc_Core_Opt*
_tmp96C;struct Cyc_Core_Opt**_tmp96D;struct Cyc_Absyn_Kind*_tmp96E;void*_tmp96F;
struct Cyc_Core_Opt*_tmp971;struct Cyc_Core_Opt**_tmp972;struct Cyc_Absyn_Kind*
_tmp973;void*_tmp974;struct Cyc_Core_Opt*_tmp976;struct Cyc_Core_Opt**_tmp977;
struct Cyc_Absyn_Kind*_tmp978;_LL4E8: _tmp951=_tmp950.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp952=(struct Cyc_Absyn_Eq_kb_struct*)_tmp951;if(_tmp952->tag != 0)goto _LL4EA;
else{_tmp953=_tmp952->f1;}};_tmp954=_tmp950.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp955=(struct Cyc_Absyn_Eq_kb_struct*)_tmp954;if(_tmp955->tag != 0)goto _LL4EA;
else{_tmp956=_tmp955->f1;}};_LL4E9: return _tmp953 == _tmp956;_LL4EA: _tmp957=
_tmp950.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp958=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp957;if(_tmp958->tag != 1)goto _LL4EC;else{_tmp959=_tmp958->f1;_tmp95A=(struct
Cyc_Core_Opt**)& _tmp958->f1;}};_LL4EB:{struct Cyc_Core_Opt*_tmp135B;*_tmp95A=((
_tmp135B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp135B)),((_tmp135B->v=
c1,_tmp135B))));}return 1;_LL4EC: _tmp95B=_tmp950.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp95C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp95B;if(_tmp95C->tag != 1)goto
_LL4EE;else{_tmp95D=_tmp95C->f1;_tmp95E=(struct Cyc_Core_Opt**)& _tmp95C->f1;}};
_LL4ED:{struct Cyc_Core_Opt*_tmp135C;*_tmp95E=((_tmp135C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp135C)),((_tmp135C->v=c2,_tmp135C))));}return 1;_LL4EE: _tmp95F=_tmp950.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp960=(struct Cyc_Absyn_Less_kb_struct*)_tmp95F;
if(_tmp960->tag != 2)goto _LL4F0;else{_tmp961=_tmp960->f1;_tmp962=(struct Cyc_Core_Opt**)&
_tmp960->f1;_tmp963=_tmp960->f2;}};_tmp964=_tmp950.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp965=(struct Cyc_Absyn_Eq_kb_struct*)_tmp964;if(_tmp965->tag != 0)goto _LL4F0;
else{_tmp966=_tmp965->f1;}};_LL4EF: if(Cyc_Tcutil_kind_leq(_tmp966,_tmp963)){{
struct Cyc_Core_Opt*_tmp135D;*_tmp962=((_tmp135D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp135D)),((_tmp135D->v=c2,_tmp135D))));}return 1;}else{return 0;}_LL4F0:
_tmp967=_tmp950.f1;{struct Cyc_Absyn_Eq_kb_struct*_tmp968=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp967;if(_tmp968->tag != 0)goto _LL4F2;else{_tmp969=_tmp968->f1;}};_tmp96A=
_tmp950.f2;{struct Cyc_Absyn_Less_kb_struct*_tmp96B=(struct Cyc_Absyn_Less_kb_struct*)
_tmp96A;if(_tmp96B->tag != 2)goto _LL4F2;else{_tmp96C=_tmp96B->f1;_tmp96D=(struct
Cyc_Core_Opt**)& _tmp96B->f1;_tmp96E=_tmp96B->f2;}};_LL4F1: if(Cyc_Tcutil_kind_leq(
_tmp969,_tmp96E)){{struct Cyc_Core_Opt*_tmp135E;*_tmp96D=((_tmp135E=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp135E)),((_tmp135E->v=c1,
_tmp135E))));}return 1;}else{return 0;}_LL4F2: _tmp96F=_tmp950.f1;{struct Cyc_Absyn_Less_kb_struct*
_tmp970=(struct Cyc_Absyn_Less_kb_struct*)_tmp96F;if(_tmp970->tag != 2)goto _LL4E7;
else{_tmp971=_tmp970->f1;_tmp972=(struct Cyc_Core_Opt**)& _tmp970->f1;_tmp973=
_tmp970->f2;}};_tmp974=_tmp950.f2;{struct Cyc_Absyn_Less_kb_struct*_tmp975=(
struct Cyc_Absyn_Less_kb_struct*)_tmp974;if(_tmp975->tag != 2)goto _LL4E7;else{
_tmp976=_tmp975->f1;_tmp977=(struct Cyc_Core_Opt**)& _tmp975->f1;_tmp978=_tmp975->f2;}};
_LL4F3: if(Cyc_Tcutil_kind_leq(_tmp973,_tmp978)){{struct Cyc_Core_Opt*_tmp135F;*
_tmp977=((_tmp135F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp135F)),((
_tmp135F->v=c1,_tmp135F))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp978,
_tmp973)){{struct Cyc_Core_Opt*_tmp1360;*_tmp972=((_tmp1360=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1360)),((_tmp1360->v=c2,_tmp1360))));}return 1;}else{return 0;}}_LL4E7:;};}
static int Cyc_Tcutil_tvar_id_counter=0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;const char*_tmp1364;void*_tmp1363[1];
struct Cyc_Int_pa_struct _tmp1362;struct _dyneither_ptr s=(struct _dyneither_ptr)((
_tmp1362.tag=1,((_tmp1362.f1=(unsigned long)i,((_tmp1363[0]=& _tmp1362,Cyc_aprintf(((
_tmp1364="#%d",_tag_dyneither(_tmp1364,sizeof(char),4))),_tag_dyneither(_tmp1363,
sizeof(void*),1))))))));struct _dyneither_ptr*_tmp1367;struct Cyc_Absyn_Tvar*
_tmp1366;return(_tmp1366=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1366)),((
_tmp1366->name=((_tmp1367=_region_malloc(Cyc_Core_unique_region,sizeof(struct
_dyneither_ptr)* 1),((_tmp1367[0]=s,_tmp1367)))),((_tmp1366->identity=- 1,((
_tmp1366->kind=k,_tmp1366)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp984=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp984,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp136B;void*_tmp136A[1];struct Cyc_String_pa_struct
_tmp1369;(_tmp1369.tag=0,((_tmp1369.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp136A[0]=& _tmp1369,Cyc_printf(((_tmp136B="%s",
_tag_dyneither(_tmp136B,sizeof(char),3))),_tag_dyneither(_tmp136A,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp136C;struct
_dyneither_ptr _tmp988=Cyc_strconcat(((_tmp136C="`",_tag_dyneither(_tmp136C,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp136F;char _tmp136E;
struct _dyneither_ptr _tmp136D;(_tmp136D=_dyneither_ptr_plus(_tmp988,sizeof(char),
1),((_tmp136E=*((char*)_check_dyneither_subscript(_tmp136D,sizeof(char),0)),((
_tmp136F='t',((_get_dyneither_size(_tmp136D,sizeof(char))== 1  && (_tmp136E == '\000'
 && _tmp136F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp136D.curr)=_tmp136F)))))));}{
struct _dyneither_ptr*_tmp1370;t->name=((_tmp1370=_region_malloc(Cyc_Core_unique_region,
sizeof(struct _dyneither_ptr)* 1),((_tmp1370[0]=(struct _dyneither_ptr)_tmp988,
_tmp1370))));};};}struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple25*x){struct
Cyc_Core_Opt*_tmp1373;struct _tuple9*_tmp1372;return(_tmp1372=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1372)),((_tmp1372->f1=(struct Cyc_Core_Opt*)((_tmp1373=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp1373)),((_tmp1373->v=(*x).f1,_tmp1373)))),((
_tmp1372->f2=(*x).f2,((_tmp1372->f3=(*x).f3,_tmp1372)))))));}void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp990=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){struct Cyc_List_List*_tmp1374;_tmp990=((_tmp1374=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp1374)),((_tmp1374->hd=(void*)atts->hd,((
_tmp1374->tl=_tmp990,_tmp1374))))));}}}{struct Cyc_Absyn_FnType_struct _tmp137A;
struct Cyc_Absyn_FnInfo _tmp1379;struct Cyc_Absyn_FnType_struct*_tmp1378;return(
void*)((_tmp1378=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1378)),((
_tmp1378[0]=((_tmp137A.tag=10,((_tmp137A.f1=((_tmp1379.tvars=fd->tvs,((_tmp1379.effect=
fd->effect,((_tmp1379.ret_tqual=fd->ret_tqual,((_tmp1379.ret_typ=fd->ret_type,((
_tmp1379.args=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple25*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((
_tmp1379.c_varargs=fd->c_varargs,((_tmp1379.cyc_varargs=fd->cyc_varargs,((
_tmp1379.rgn_po=fd->rgn_po,((_tmp1379.attributes=_tmp990,_tmp1379)))))))))))))))))),
_tmp137A)))),_tmp1378))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple26{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple26*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){
return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp137B;return(_tmp137B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp137B)),((_tmp137B->f1=(*pr).f1,((_tmp137B->f2=t,_tmp137B)))));}
struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple28{
struct _tuple27*f1;void*f2;};static struct _tuple28*Cyc_Tcutil_substitute_f1(struct
_RegionHandle*rgn,struct _tuple9*y){struct _tuple27*_tmp137E;struct _tuple28*
_tmp137D;return(_tmp137D=_region_malloc(rgn,sizeof(*_tmp137D)),((_tmp137D->f1=((
_tmp137E=_region_malloc(rgn,sizeof(*_tmp137E)),((_tmp137E->f1=(*y).f1,((_tmp137E->f2=(*
y).f2,_tmp137E)))))),((_tmp137D->f2=(*y).f3,_tmp137D)))));}static struct _tuple9*
Cyc_Tcutil_substitute_f2(struct _tuple28*w){struct _tuple27*_tmp999;void*_tmp99A;
struct _tuple28 _tmp998=*w;_tmp999=_tmp998.f1;_tmp99A=_tmp998.f2;{struct Cyc_Core_Opt*
_tmp99C;struct Cyc_Absyn_Tqual _tmp99D;struct _tuple27 _tmp99B=*_tmp999;_tmp99C=
_tmp99B.f1;_tmp99D=_tmp99B.f2;{struct _tuple9*_tmp137F;return(_tmp137F=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp137F)),((_tmp137F->f1=_tmp99C,((
_tmp137F->f2=_tmp99D,((_tmp137F->f3=_tmp99A,_tmp137F)))))));};};}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1380;return(
_tmp1380=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1380)),((_tmp1380->name=
f->name,((_tmp1380->tq=f->tq,((_tmp1380->type=t,((_tmp1380->width=f->width,((
_tmp1380->attributes=f->attributes,_tmp1380)))))))))));}static struct Cyc_List_List*
Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*
ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
struct Cyc_Absyn_Exp*_tmp1381;return(_tmp1381=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1381)),((_tmp1381->topt=old->topt,((_tmp1381->r=r,((_tmp1381->loc=
old->loc,((_tmp1381->annot=old->annot,_tmp1381)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp9A1=e->r;enum Cyc_Absyn_Primop _tmp9A7;struct Cyc_List_List*_tmp9A8;
struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*
_tmp9AC;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_Absyn_Exp*
_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B2;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_Absyn_Exp*
_tmp9B5;void*_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B8;int _tmp9B9;enum Cyc_Absyn_Coercion
_tmp9BA;void*_tmp9BC;struct Cyc_Absyn_Exp*_tmp9BE;void*_tmp9C0;void*_tmp9C1;void*
_tmp9C3;_LL4F5: {struct Cyc_Absyn_Const_e_struct*_tmp9A2=(struct Cyc_Absyn_Const_e_struct*)
_tmp9A1;if(_tmp9A2->tag != 0)goto _LL4F7;}_LL4F6: goto _LL4F8;_LL4F7: {struct Cyc_Absyn_Enum_e_struct*
_tmp9A3=(struct Cyc_Absyn_Enum_e_struct*)_tmp9A1;if(_tmp9A3->tag != 32)goto _LL4F9;}
_LL4F8: goto _LL4FA;_LL4F9: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp9A4=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp9A1;if(_tmp9A4->tag != 33)goto _LL4FB;}_LL4FA: goto _LL4FC;_LL4FB: {struct Cyc_Absyn_Var_e_struct*
_tmp9A5=(struct Cyc_Absyn_Var_e_struct*)_tmp9A1;if(_tmp9A5->tag != 1)goto _LL4FD;}
_LL4FC: return e;_LL4FD: {struct Cyc_Absyn_Primop_e_struct*_tmp9A6=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9A1;if(_tmp9A6->tag != 3)goto _LL4FF;else{_tmp9A7=_tmp9A6->f1;_tmp9A8=_tmp9A6->f2;}}
_LL4FE: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A8)== 1){struct
Cyc_Absyn_Exp*_tmp9C4=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp9A8))->hd;struct Cyc_Absyn_Exp*_tmp9C5=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C4);
if(_tmp9C5 == _tmp9C4)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1387;struct Cyc_Absyn_Exp*
_tmp1386[1];struct Cyc_Absyn_Primop_e_struct*_tmp1385;return Cyc_Tcutil_copye(e,(
void*)((_tmp1385=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1385)),((
_tmp1385[0]=((_tmp1387.tag=3,((_tmp1387.f1=_tmp9A7,((_tmp1387.f2=((_tmp1386[0]=
_tmp9C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1386,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1387)))))),
_tmp1385)))));};}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp9A8)== 2){struct Cyc_Absyn_Exp*_tmp9C9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp9A8))->hd;struct Cyc_Absyn_Exp*_tmp9CA=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9A8->tl))->hd;struct Cyc_Absyn_Exp*_tmp9CB=
Cyc_Tcutil_rsubsexp(r,inst,_tmp9C9);struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9CA);if(_tmp9CB == _tmp9C9  && _tmp9CC == _tmp9CA)return e;{struct Cyc_Absyn_Primop_e_struct
_tmp138D;struct Cyc_Absyn_Exp*_tmp138C[2];struct Cyc_Absyn_Primop_e_struct*
_tmp138B;return Cyc_Tcutil_copye(e,(void*)((_tmp138B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp138B)),((_tmp138B[0]=((_tmp138D.tag=3,((_tmp138D.f1=_tmp9A7,((
_tmp138D.f2=((_tmp138C[1]=_tmp9CC,((_tmp138C[0]=_tmp9CB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp138C,sizeof(struct Cyc_Absyn_Exp*),
2)))))),_tmp138D)))))),_tmp138B)))));};}else{const char*_tmp1390;void*_tmp138F;
return(_tmp138F=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1390="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1390,sizeof(char),34))),_tag_dyneither(_tmp138F,sizeof(void*),
0)));}}_LL4FF: {struct Cyc_Absyn_Conditional_e_struct*_tmp9A9=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9A1;if(_tmp9A9->tag != 6)goto _LL501;else{_tmp9AA=_tmp9A9->f1;_tmp9AB=_tmp9A9->f2;
_tmp9AC=_tmp9A9->f3;}}_LL500: {struct Cyc_Absyn_Exp*_tmp9D2=Cyc_Tcutil_rsubsexp(r,
inst,_tmp9AA);struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AB);
struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AC);if((_tmp9D2 == 
_tmp9AA  && _tmp9D3 == _tmp9AB) && _tmp9D4 == _tmp9AC)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1393;struct Cyc_Absyn_Conditional_e_struct*_tmp1392;return Cyc_Tcutil_copye(e,(
void*)((_tmp1392=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1392)),((
_tmp1392[0]=((_tmp1393.tag=6,((_tmp1393.f1=_tmp9D2,((_tmp1393.f2=_tmp9D3,((
_tmp1393.f3=_tmp9D4,_tmp1393)))))))),_tmp1392)))));};}_LL501: {struct Cyc_Absyn_And_e_struct*
_tmp9AD=(struct Cyc_Absyn_And_e_struct*)_tmp9A1;if(_tmp9AD->tag != 7)goto _LL503;
else{_tmp9AE=_tmp9AD->f1;_tmp9AF=_tmp9AD->f2;}}_LL502: {struct Cyc_Absyn_Exp*
_tmp9D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AE);struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9AF);if(_tmp9D7 == _tmp9AE  && _tmp9D8 == _tmp9AF)return e;{struct Cyc_Absyn_And_e_struct
_tmp1396;struct Cyc_Absyn_And_e_struct*_tmp1395;return Cyc_Tcutil_copye(e,(void*)((
_tmp1395=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1395)),((_tmp1395[0]=((
_tmp1396.tag=7,((_tmp1396.f1=_tmp9D7,((_tmp1396.f2=_tmp9D8,_tmp1396)))))),
_tmp1395)))));};}_LL503: {struct Cyc_Absyn_Or_e_struct*_tmp9B0=(struct Cyc_Absyn_Or_e_struct*)
_tmp9A1;if(_tmp9B0->tag != 8)goto _LL505;else{_tmp9B1=_tmp9B0->f1;_tmp9B2=_tmp9B0->f2;}}
_LL504: {struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B1);struct
Cyc_Absyn_Exp*_tmp9DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B2);if(_tmp9DB == _tmp9B1
 && _tmp9DC == _tmp9B2)return e;{struct Cyc_Absyn_Or_e_struct _tmp1399;struct Cyc_Absyn_Or_e_struct*
_tmp1398;return Cyc_Tcutil_copye(e,(void*)((_tmp1398=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1398)),((_tmp1398[0]=((_tmp1399.tag=8,((_tmp1399.f1=_tmp9DB,((
_tmp1399.f2=_tmp9DC,_tmp1399)))))),_tmp1398)))));};}_LL505: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9B3=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp9A1;if(_tmp9B3->tag != 9)goto _LL507;
else{_tmp9B4=_tmp9B3->f1;_tmp9B5=_tmp9B3->f2;}}_LL506: {struct Cyc_Absyn_Exp*
_tmp9DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B4);struct Cyc_Absyn_Exp*_tmp9E0=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9B5);if(_tmp9DF == _tmp9B4  && _tmp9E0 == _tmp9B5)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp139C;struct Cyc_Absyn_SeqExp_e_struct*_tmp139B;return Cyc_Tcutil_copye(e,(void*)((
_tmp139B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp139B)),((_tmp139B[0]=((
_tmp139C.tag=9,((_tmp139C.f1=_tmp9DF,((_tmp139C.f2=_tmp9E0,_tmp139C)))))),
_tmp139B)))));};}_LL507: {struct Cyc_Absyn_Cast_e_struct*_tmp9B6=(struct Cyc_Absyn_Cast_e_struct*)
_tmp9A1;if(_tmp9B6->tag != 15)goto _LL509;else{_tmp9B7=(void*)_tmp9B6->f1;_tmp9B8=
_tmp9B6->f2;_tmp9B9=_tmp9B6->f3;_tmp9BA=_tmp9B6->f4;}}_LL508: {struct Cyc_Absyn_Exp*
_tmp9E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B8);void*_tmp9E4=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9B7);if(_tmp9E3 == _tmp9B8  && _tmp9E4 == _tmp9B7)return e;{struct Cyc_Absyn_Cast_e_struct
_tmp139F;struct Cyc_Absyn_Cast_e_struct*_tmp139E;return Cyc_Tcutil_copye(e,(void*)((
_tmp139E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp139E)),((_tmp139E[0]=((
_tmp139F.tag=15,((_tmp139F.f1=(void*)_tmp9E4,((_tmp139F.f2=_tmp9E3,((_tmp139F.f3=
_tmp9B9,((_tmp139F.f4=_tmp9BA,_tmp139F)))))))))),_tmp139E)))));};}_LL509: {
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp9BB=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp9A1;if(_tmp9BB->tag != 18)goto _LL50B;else{_tmp9BC=(void*)_tmp9BB->f1;}}_LL50A: {
void*_tmp9E7=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BC);if(_tmp9E7 == _tmp9BC)return e;{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmp13A2;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp13A1;return Cyc_Tcutil_copye(e,(void*)((_tmp13A1=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13A1)),((_tmp13A1[0]=((_tmp13A2.tag=18,((_tmp13A2.f1=(void*)_tmp9E7,
_tmp13A2)))),_tmp13A1)))));};}_LL50B: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9BD=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp9A1;if(_tmp9BD->tag != 19)goto
_LL50D;else{_tmp9BE=_tmp9BD->f1;}}_LL50C: {struct Cyc_Absyn_Exp*_tmp9EA=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9BE);if(_tmp9EA == _tmp9BE)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp13A5;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp13A4;return Cyc_Tcutil_copye(e,(
void*)((_tmp13A4=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13A4)),((
_tmp13A4[0]=((_tmp13A5.tag=19,((_tmp13A5.f1=_tmp9EA,_tmp13A5)))),_tmp13A4)))));};}
_LL50D: {struct Cyc_Absyn_Offsetof_e_struct*_tmp9BF=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp9A1;if(_tmp9BF->tag != 20)goto _LL50F;else{_tmp9C0=(void*)_tmp9BF->f1;_tmp9C1=(
void*)_tmp9BF->f2;}}_LL50E: {void*_tmp9ED=Cyc_Tcutil_rsubstitute(r,inst,_tmp9C0);
if(_tmp9ED == _tmp9C0)return e;{struct Cyc_Absyn_Offsetof_e_struct _tmp13A8;struct
Cyc_Absyn_Offsetof_e_struct*_tmp13A7;return Cyc_Tcutil_copye(e,(void*)((_tmp13A7=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13A7)),((_tmp13A7[0]=((
_tmp13A8.tag=20,((_tmp13A8.f1=(void*)_tmp9ED,((_tmp13A8.f2=(void*)_tmp9C1,
_tmp13A8)))))),_tmp13A7)))));};}_LL50F: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9C2=(struct Cyc_Absyn_Valueof_e_struct*)_tmp9A1;if(_tmp9C2->tag != 39)goto
_LL511;else{_tmp9C3=(void*)_tmp9C2->f1;}}_LL510: {void*_tmp9F0=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9C3);if(_tmp9F0 == _tmp9C3)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp13AB;struct Cyc_Absyn_Valueof_e_struct*_tmp13AA;return Cyc_Tcutil_copye(e,(
void*)((_tmp13AA=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13AA)),((
_tmp13AA[0]=((_tmp13AB.tag=39,((_tmp13AB.f1=(void*)_tmp9F0,_tmp13AB)))),_tmp13AA)))));};}
_LL511:;_LL512: {const char*_tmp13AE;void*_tmp13AD;return(_tmp13AD=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13AE="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp13AE,sizeof(char),46))),_tag_dyneither(_tmp13AD,sizeof(void*),
0)));}_LL4F4:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9F5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9F7;
struct Cyc_Absyn_AggrInfo _tmp9F9;union Cyc_Absyn_AggrInfoU _tmp9FA;struct Cyc_List_List*
_tmp9FB;struct Cyc_Absyn_DatatypeInfo _tmp9FD;union Cyc_Absyn_DatatypeInfoU _tmp9FE;
struct Cyc_List_List*_tmp9FF;struct Cyc_Absyn_DatatypeFieldInfo _tmpA01;union Cyc_Absyn_DatatypeFieldInfoU
_tmpA02;struct Cyc_List_List*_tmpA03;struct _tuple2*_tmpA05;struct Cyc_List_List*
_tmpA06;struct Cyc_Absyn_Typedefdecl*_tmpA07;void**_tmpA08;struct Cyc_Absyn_ArrayInfo
_tmpA0A;void*_tmpA0B;struct Cyc_Absyn_Tqual _tmpA0C;struct Cyc_Absyn_Exp*_tmpA0D;
union Cyc_Absyn_Constraint*_tmpA0E;struct Cyc_Position_Segment*_tmpA0F;struct Cyc_Absyn_PtrInfo
_tmpA11;void*_tmpA12;struct Cyc_Absyn_Tqual _tmpA13;struct Cyc_Absyn_PtrAtts _tmpA14;
void*_tmpA15;union Cyc_Absyn_Constraint*_tmpA16;union Cyc_Absyn_Constraint*_tmpA17;
union Cyc_Absyn_Constraint*_tmpA18;struct Cyc_Absyn_FnInfo _tmpA1A;struct Cyc_List_List*
_tmpA1B;struct Cyc_Core_Opt*_tmpA1C;struct Cyc_Absyn_Tqual _tmpA1D;void*_tmpA1E;
struct Cyc_List_List*_tmpA1F;int _tmpA20;struct Cyc_Absyn_VarargInfo*_tmpA21;struct
Cyc_List_List*_tmpA22;struct Cyc_List_List*_tmpA23;struct Cyc_List_List*_tmpA25;
enum Cyc_Absyn_AggrKind _tmpA27;struct Cyc_List_List*_tmpA28;struct Cyc_Core_Opt*
_tmpA2A;void*_tmpA2C;void*_tmpA2E;void*_tmpA2F;void*_tmpA31;struct Cyc_Absyn_Exp*
_tmpA33;void*_tmpA3E;void*_tmpA40;struct Cyc_List_List*_tmpA42;_LL514: {struct Cyc_Absyn_VarType_struct*
_tmp9F6=(struct Cyc_Absyn_VarType_struct*)_tmp9F5;if(_tmp9F6->tag != 2)goto _LL516;
else{_tmp9F7=_tmp9F6->f1;}}_LL515: {struct _handler_cons _tmpA44;_push_handler(&
_tmpA44);{int _tmpA46=0;if(setjmp(_tmpA44.handler))_tmpA46=1;if(!_tmpA46){{void*
_tmpA47=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9F7);_npop_handler(0);return _tmpA47;};_pop_handler();}else{void*_tmpA45=(
void*)_exn_thrown;void*_tmpA49=_tmpA45;_LL54D: {struct Cyc_Core_Not_found_struct*
_tmpA4A=(struct Cyc_Core_Not_found_struct*)_tmpA49;if(_tmpA4A->tag != Cyc_Core_Not_found)
goto _LL54F;}_LL54E: return t;_LL54F:;_LL550:(void)_throw(_tmpA49);_LL54C:;}};}
_LL516: {struct Cyc_Absyn_AggrType_struct*_tmp9F8=(struct Cyc_Absyn_AggrType_struct*)
_tmp9F5;if(_tmp9F8->tag != 12)goto _LL518;else{_tmp9F9=_tmp9F8->f1;_tmp9FA=_tmp9F9.aggr_info;
_tmp9FB=_tmp9F9.targs;}}_LL517: {struct Cyc_List_List*_tmpA4B=Cyc_Tcutil_substs(
rgn,inst,_tmp9FB);struct Cyc_Absyn_AggrType_struct _tmp13B4;struct Cyc_Absyn_AggrInfo
_tmp13B3;struct Cyc_Absyn_AggrType_struct*_tmp13B2;return _tmpA4B == _tmp9FB?t:(
void*)((_tmp13B2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13B2)),((
_tmp13B2[0]=((_tmp13B4.tag=12,((_tmp13B4.f1=((_tmp13B3.aggr_info=_tmp9FA,((
_tmp13B3.targs=_tmpA4B,_tmp13B3)))),_tmp13B4)))),_tmp13B2))));}_LL518: {struct
Cyc_Absyn_DatatypeType_struct*_tmp9FC=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp9F5;if(_tmp9FC->tag != 3)goto _LL51A;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=_tmp9FD.datatype_info;
_tmp9FF=_tmp9FD.targs;}}_LL519: {struct Cyc_List_List*_tmpA4F=Cyc_Tcutil_substs(
rgn,inst,_tmp9FF);struct Cyc_Absyn_DatatypeType_struct _tmp13BA;struct Cyc_Absyn_DatatypeInfo
_tmp13B9;struct Cyc_Absyn_DatatypeType_struct*_tmp13B8;return _tmpA4F == _tmp9FF?t:(
void*)((_tmp13B8=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13B8)),((
_tmp13B8[0]=((_tmp13BA.tag=3,((_tmp13BA.f1=((_tmp13B9.datatype_info=_tmp9FE,((
_tmp13B9.targs=_tmpA4F,_tmp13B9)))),_tmp13BA)))),_tmp13B8))));}_LL51A: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmpA00=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp9F5;if(_tmpA00->tag != 4)goto _LL51C;else{_tmpA01=_tmpA00->f1;_tmpA02=_tmpA01.field_info;
_tmpA03=_tmpA01.targs;}}_LL51B: {struct Cyc_List_List*_tmpA53=Cyc_Tcutil_substs(
rgn,inst,_tmpA03);struct Cyc_Absyn_DatatypeFieldType_struct _tmp13C0;struct Cyc_Absyn_DatatypeFieldInfo
_tmp13BF;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp13BE;return _tmpA53 == 
_tmpA03?t:(void*)((_tmp13BE=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp13BE)),((_tmp13BE[0]=((_tmp13C0.tag=4,((_tmp13C0.f1=((_tmp13BF.field_info=
_tmpA02,((_tmp13BF.targs=_tmpA53,_tmp13BF)))),_tmp13C0)))),_tmp13BE))));}_LL51C: {
struct Cyc_Absyn_TypedefType_struct*_tmpA04=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9F5;if(_tmpA04->tag != 18)goto _LL51E;else{_tmpA05=_tmpA04->f1;_tmpA06=_tmpA04->f2;
_tmpA07=_tmpA04->f3;_tmpA08=_tmpA04->f4;}}_LL51D: {struct Cyc_List_List*_tmpA57=
Cyc_Tcutil_substs(rgn,inst,_tmpA06);struct Cyc_Absyn_TypedefType_struct _tmp13C3;
struct Cyc_Absyn_TypedefType_struct*_tmp13C2;return _tmpA57 == _tmpA06?t:(void*)((
_tmp13C2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13C2)),((_tmp13C2[0]=((
_tmp13C3.tag=18,((_tmp13C3.f1=_tmpA05,((_tmp13C3.f2=_tmpA57,((_tmp13C3.f3=
_tmpA07,((_tmp13C3.f4=_tmpA08,_tmp13C3)))))))))),_tmp13C2))));}_LL51E: {struct
Cyc_Absyn_ArrayType_struct*_tmpA09=(struct Cyc_Absyn_ArrayType_struct*)_tmp9F5;
if(_tmpA09->tag != 9)goto _LL520;else{_tmpA0A=_tmpA09->f1;_tmpA0B=_tmpA0A.elt_type;
_tmpA0C=_tmpA0A.tq;_tmpA0D=_tmpA0A.num_elts;_tmpA0E=_tmpA0A.zero_term;_tmpA0F=
_tmpA0A.zt_loc;}}_LL51F: {void*_tmpA5A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0B);
struct Cyc_Absyn_ArrayType_struct _tmp13C9;struct Cyc_Absyn_ArrayInfo _tmp13C8;
struct Cyc_Absyn_ArrayType_struct*_tmp13C7;return _tmpA5A == _tmpA0B?t:(void*)((
_tmp13C7=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13C7)),((_tmp13C7[0]=((
_tmp13C9.tag=9,((_tmp13C9.f1=((_tmp13C8.elt_type=_tmpA5A,((_tmp13C8.tq=_tmpA0C,((
_tmp13C8.num_elts=_tmpA0D,((_tmp13C8.zero_term=_tmpA0E,((_tmp13C8.zt_loc=_tmpA0F,
_tmp13C8)))))))))),_tmp13C9)))),_tmp13C7))));}_LL520: {struct Cyc_Absyn_PointerType_struct*
_tmpA10=(struct Cyc_Absyn_PointerType_struct*)_tmp9F5;if(_tmpA10->tag != 5)goto
_LL522;else{_tmpA11=_tmpA10->f1;_tmpA12=_tmpA11.elt_typ;_tmpA13=_tmpA11.elt_tq;
_tmpA14=_tmpA11.ptr_atts;_tmpA15=_tmpA14.rgn;_tmpA16=_tmpA14.nullable;_tmpA17=
_tmpA14.bounds;_tmpA18=_tmpA14.zero_term;}}_LL521: {void*_tmpA5E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA12);void*_tmpA5F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA15);union Cyc_Absyn_Constraint*
_tmpA60=_tmpA17;{void*_tmpA61=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpA17);struct Cyc_Absyn_Exp*_tmpA63;_LL552: {
struct Cyc_Absyn_Upper_b_struct*_tmpA62=(struct Cyc_Absyn_Upper_b_struct*)_tmpA61;
if(_tmpA62->tag != 1)goto _LL554;else{_tmpA63=_tmpA62->f1;}}_LL553: {struct Cyc_Absyn_Exp*
_tmpA64=Cyc_Tcutil_rsubsexp(rgn,inst,_tmpA63);if(_tmpA64 != _tmpA63){struct Cyc_Absyn_Upper_b_struct
_tmp13CC;struct Cyc_Absyn_Upper_b_struct*_tmp13CB;_tmpA60=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13CB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13CB)),((_tmp13CB[0]=((_tmp13CC.tag=1,((_tmp13CC.f1=_tmpA64,_tmp13CC)))),
_tmp13CB)))));}goto _LL551;}_LL554:;_LL555: goto _LL551;_LL551:;}if((_tmpA5E == 
_tmpA12  && _tmpA5F == _tmpA15) && _tmpA60 == _tmpA17)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13D6;struct Cyc_Absyn_PtrAtts _tmp13D5;struct Cyc_Absyn_PtrInfo _tmp13D4;struct
Cyc_Absyn_PointerType_struct*_tmp13D3;return(void*)((_tmp13D3=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13D3)),((_tmp13D3[0]=((_tmp13D6.tag=5,((_tmp13D6.f1=((_tmp13D4.elt_typ=
_tmpA5E,((_tmp13D4.elt_tq=_tmpA13,((_tmp13D4.ptr_atts=((_tmp13D5.rgn=_tmpA5F,((
_tmp13D5.nullable=_tmpA16,((_tmp13D5.bounds=_tmpA60,((_tmp13D5.zero_term=_tmpA18,((
_tmp13D5.ptrloc=0,_tmp13D5)))))))))),_tmp13D4)))))),_tmp13D6)))),_tmp13D3))));};}
_LL522: {struct Cyc_Absyn_FnType_struct*_tmpA19=(struct Cyc_Absyn_FnType_struct*)
_tmp9F5;if(_tmpA19->tag != 10)goto _LL524;else{_tmpA1A=_tmpA19->f1;_tmpA1B=_tmpA1A.tvars;
_tmpA1C=_tmpA1A.effect;_tmpA1D=_tmpA1A.ret_tqual;_tmpA1E=_tmpA1A.ret_typ;_tmpA1F=
_tmpA1A.args;_tmpA20=_tmpA1A.c_varargs;_tmpA21=_tmpA1A.cyc_varargs;_tmpA22=
_tmpA1A.rgn_po;_tmpA23=_tmpA1A.attributes;}}_LL523:{struct Cyc_List_List*_tmpA6B=
_tmpA1B;for(0;_tmpA6B != 0;_tmpA6B=_tmpA6B->tl){struct _tuple15*_tmp13E0;struct Cyc_Absyn_VarType_struct
_tmp13DF;struct Cyc_Absyn_VarType_struct*_tmp13DE;struct Cyc_List_List*_tmp13DD;
inst=((_tmp13DD=_region_malloc(rgn,sizeof(*_tmp13DD)),((_tmp13DD->hd=((_tmp13E0=
_region_malloc(rgn,sizeof(*_tmp13E0)),((_tmp13E0->f1=(struct Cyc_Absyn_Tvar*)
_tmpA6B->hd,((_tmp13E0->f2=(void*)((_tmp13DE=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13DE)),((_tmp13DE[0]=((_tmp13DF.tag=2,((_tmp13DF.f1=(struct Cyc_Absyn_Tvar*)
_tmpA6B->hd,_tmp13DF)))),_tmp13DE)))),_tmp13E0)))))),((_tmp13DD->tl=inst,
_tmp13DD))))));}}{struct Cyc_List_List*_tmpA71;struct Cyc_List_List*_tmpA72;struct
_tuple1 _tmpA70=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple28*(*f)(struct _RegionHandle*,struct _tuple9*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,
rgn,_tmpA1F));_tmpA71=_tmpA70.f1;_tmpA72=_tmpA70.f2;{struct Cyc_List_List*_tmpA73=
Cyc_Tcutil_substs(rgn,inst,_tmpA72);struct Cyc_List_List*_tmpA74=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA71,_tmpA73));struct Cyc_Core_Opt*
eff2;if(_tmpA1C == 0)eff2=0;else{void*_tmpA75=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA1C->v);if(_tmpA75 == (void*)_tmpA1C->v)eff2=_tmpA1C;else{struct Cyc_Core_Opt*
_tmp13E1;eff2=((_tmp13E1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13E1)),((
_tmp13E1->v=_tmpA75,_tmp13E1))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(
_tmpA21 == 0)cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA78;struct Cyc_Absyn_Tqual
_tmpA79;void*_tmpA7A;int _tmpA7B;struct Cyc_Absyn_VarargInfo _tmpA77=*_tmpA21;
_tmpA78=_tmpA77.name;_tmpA79=_tmpA77.tq;_tmpA7A=_tmpA77.type;_tmpA7B=_tmpA77.inject;{
void*_tmpA7C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA7A);if(_tmpA7C == _tmpA7A)
cyc_varargs2=_tmpA21;else{struct Cyc_Absyn_VarargInfo*_tmp13E2;cyc_varargs2=((
_tmp13E2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13E2)),((_tmp13E2->name=
_tmpA78,((_tmp13E2->tq=_tmpA79,((_tmp13E2->type=_tmpA7C,((_tmp13E2->inject=
_tmpA7B,_tmp13E2))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA7F;struct Cyc_List_List*_tmpA80;struct _tuple1 _tmpA7E=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA22);_tmpA7F=_tmpA7E.f1;_tmpA80=_tmpA7E.f2;{struct Cyc_List_List*
_tmpA81=Cyc_Tcutil_substs(rgn,inst,_tmpA7F);struct Cyc_List_List*_tmpA82=Cyc_Tcutil_substs(
rgn,inst,_tmpA80);if(_tmpA81 == _tmpA7F  && _tmpA82 == _tmpA80)rgn_po2=_tmpA22;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA81,_tmpA82);}{struct Cyc_Absyn_FnType_struct _tmp13E8;struct
Cyc_Absyn_FnInfo _tmp13E7;struct Cyc_Absyn_FnType_struct*_tmp13E6;return(void*)((
_tmp13E6=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13E6)),((_tmp13E6[0]=((
_tmp13E8.tag=10,((_tmp13E8.f1=((_tmp13E7.tvars=_tmpA1B,((_tmp13E7.effect=eff2,((
_tmp13E7.ret_tqual=_tmpA1D,((_tmp13E7.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpA1E),((_tmp13E7.args=_tmpA74,((_tmp13E7.c_varargs=_tmpA20,((_tmp13E7.cyc_varargs=
cyc_varargs2,((_tmp13E7.rgn_po=rgn_po2,((_tmp13E7.attributes=_tmpA23,_tmp13E7)))))))))))))))))),
_tmp13E8)))),_tmp13E6))));};};};};};};_LL524: {struct Cyc_Absyn_TupleType_struct*
_tmpA24=(struct Cyc_Absyn_TupleType_struct*)_tmp9F5;if(_tmpA24->tag != 11)goto
_LL526;else{_tmpA25=_tmpA24->f1;}}_LL525: {struct Cyc_List_List*_tmpA86=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA25);struct Cyc_List_List*_tmpA87=
Cyc_Tcutil_substs(rgn,inst,_tmpA86);if(_tmpA87 == _tmpA86)return t;{struct Cyc_List_List*
_tmpA88=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA25,_tmpA87);struct Cyc_Absyn_TupleType_struct _tmp13EB;struct Cyc_Absyn_TupleType_struct*
_tmp13EA;return(void*)((_tmp13EA=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp13EA)),((_tmp13EA[0]=((_tmp13EB.tag=11,((_tmp13EB.f1=_tmpA88,_tmp13EB)))),
_tmp13EA))));};}_LL526: {struct Cyc_Absyn_AnonAggrType_struct*_tmpA26=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9F5;if(_tmpA26->tag != 13)goto _LL528;else{_tmpA27=_tmpA26->f1;_tmpA28=_tmpA26->f2;}}
_LL527: {struct Cyc_List_List*_tmpA8B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA28);struct Cyc_List_List*_tmpA8C=Cyc_Tcutil_substs(
rgn,inst,_tmpA8B);if(_tmpA8C == _tmpA8B)return t;{struct Cyc_List_List*_tmpA8D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA28,_tmpA8C);struct Cyc_Absyn_AnonAggrType_struct _tmp13EE;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13ED;return(void*)((_tmp13ED=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp13ED)),((_tmp13ED[0]=((_tmp13EE.tag=13,((_tmp13EE.f1=_tmpA27,((_tmp13EE.f2=
_tmpA8D,_tmp13EE)))))),_tmp13ED))));};}_LL528: {struct Cyc_Absyn_Evar_struct*
_tmpA29=(struct Cyc_Absyn_Evar_struct*)_tmp9F5;if(_tmpA29->tag != 1)goto _LL52A;
else{_tmpA2A=_tmpA29->f2;}}_LL529: if(_tmpA2A != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmpA2A->v);else{return t;}_LL52A: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpA2B=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp9F5;if(_tmpA2B->tag != 16)goto
_LL52C;else{_tmpA2C=(void*)_tmpA2B->f1;}}_LL52B: {void*_tmpA90=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA2C);struct Cyc_Absyn_RgnHandleType_struct _tmp13F1;struct Cyc_Absyn_RgnHandleType_struct*
_tmp13F0;return _tmpA90 == _tmpA2C?t:(void*)((_tmp13F0=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13F0)),((_tmp13F0[0]=((_tmp13F1.tag=16,((_tmp13F1.f1=(void*)_tmpA90,
_tmp13F1)))),_tmp13F0))));}_LL52C: {struct Cyc_Absyn_DynRgnType_struct*_tmpA2D=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp9F5;if(_tmpA2D->tag != 17)goto _LL52E;else{
_tmpA2E=(void*)_tmpA2D->f1;_tmpA2F=(void*)_tmpA2D->f2;}}_LL52D: {void*_tmpA93=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA2E);void*_tmpA94=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmpA2F);struct Cyc_Absyn_DynRgnType_struct _tmp13F4;struct Cyc_Absyn_DynRgnType_struct*
_tmp13F3;return _tmpA93 == _tmpA2E  && _tmpA94 == _tmpA2F?t:(void*)((_tmp13F3=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13F3)),((_tmp13F3[0]=((
_tmp13F4.tag=17,((_tmp13F4.f1=(void*)_tmpA93,((_tmp13F4.f2=(void*)_tmpA94,
_tmp13F4)))))),_tmp13F3))));}_LL52E: {struct Cyc_Absyn_TagType_struct*_tmpA30=(
struct Cyc_Absyn_TagType_struct*)_tmp9F5;if(_tmpA30->tag != 20)goto _LL530;else{
_tmpA31=(void*)_tmpA30->f1;}}_LL52F: {void*_tmpA97=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmpA31);struct Cyc_Absyn_TagType_struct _tmp13F7;struct Cyc_Absyn_TagType_struct*
_tmp13F6;return _tmpA97 == _tmpA31?t:(void*)((_tmp13F6=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13F6)),((_tmp13F6[0]=((_tmp13F7.tag=20,((_tmp13F7.f1=(void*)_tmpA97,
_tmp13F7)))),_tmp13F6))));}_LL530: {struct Cyc_Absyn_ValueofType_struct*_tmpA32=(
struct Cyc_Absyn_ValueofType_struct*)_tmp9F5;if(_tmpA32->tag != 19)goto _LL532;
else{_tmpA33=_tmpA32->f1;}}_LL531: {struct Cyc_Absyn_Exp*_tmpA9A=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA33);struct Cyc_Absyn_ValueofType_struct _tmp13FA;struct Cyc_Absyn_ValueofType_struct*
_tmp13F9;return _tmpA9A == _tmpA33?t:(void*)((_tmp13F9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp13F9)),((_tmp13F9[0]=((_tmp13FA.tag=19,((_tmp13FA.f1=_tmpA9A,
_tmp13FA)))),_tmp13F9))));}_LL532: {struct Cyc_Absyn_EnumType_struct*_tmpA34=(
struct Cyc_Absyn_EnumType_struct*)_tmp9F5;if(_tmpA34->tag != 14)goto _LL534;}_LL533:
goto _LL535;_LL534: {struct Cyc_Absyn_AnonEnumType_struct*_tmpA35=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp9F5;if(_tmpA35->tag != 15)goto _LL536;}_LL535: goto _LL537;_LL536: {struct Cyc_Absyn_VoidType_struct*
_tmpA36=(struct Cyc_Absyn_VoidType_struct*)_tmp9F5;if(_tmpA36->tag != 0)goto _LL538;}
_LL537: goto _LL539;_LL538: {struct Cyc_Absyn_IntType_struct*_tmpA37=(struct Cyc_Absyn_IntType_struct*)
_tmp9F5;if(_tmpA37->tag != 6)goto _LL53A;}_LL539: goto _LL53B;_LL53A: {struct Cyc_Absyn_FloatType_struct*
_tmpA38=(struct Cyc_Absyn_FloatType_struct*)_tmp9F5;if(_tmpA38->tag != 7)goto
_LL53C;}_LL53B: goto _LL53D;_LL53C: {struct Cyc_Absyn_DoubleType_struct*_tmpA39=(
struct Cyc_Absyn_DoubleType_struct*)_tmp9F5;if(_tmpA39->tag != 8)goto _LL53E;}
_LL53D: goto _LL53F;_LL53E: {struct Cyc_Absyn_RefCntRgn_struct*_tmpA3A=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp9F5;if(_tmpA3A->tag != 23)goto _LL540;}_LL53F: goto _LL541;_LL540: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpA3B=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp9F5;if(_tmpA3B->tag != 22)goto
_LL542;}_LL541: goto _LL543;_LL542: {struct Cyc_Absyn_HeapRgn_struct*_tmpA3C=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp9F5;if(_tmpA3C->tag != 21)goto _LL544;}_LL543:
return t;_LL544: {struct Cyc_Absyn_RgnsEff_struct*_tmpA3D=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9F5;if(_tmpA3D->tag != 26)goto _LL546;else{_tmpA3E=(void*)_tmpA3D->f1;}}_LL545: {
void*_tmpA9D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA3E);struct Cyc_Absyn_RgnsEff_struct
_tmp13FD;struct Cyc_Absyn_RgnsEff_struct*_tmp13FC;return _tmpA9D == _tmpA3E?t:(void*)((
_tmp13FC=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13FC)),((_tmp13FC[0]=((
_tmp13FD.tag=26,((_tmp13FD.f1=(void*)_tmpA9D,_tmp13FD)))),_tmp13FC))));}_LL546: {
struct Cyc_Absyn_AccessEff_struct*_tmpA3F=(struct Cyc_Absyn_AccessEff_struct*)
_tmp9F5;if(_tmpA3F->tag != 24)goto _LL548;else{_tmpA40=(void*)_tmpA3F->f1;}}_LL547: {
void*_tmpAA0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA40);struct Cyc_Absyn_AccessEff_struct
_tmp1400;struct Cyc_Absyn_AccessEff_struct*_tmp13FF;return _tmpAA0 == _tmpA40?t:(
void*)((_tmp13FF=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13FF)),((
_tmp13FF[0]=((_tmp1400.tag=24,((_tmp1400.f1=(void*)_tmpAA0,_tmp1400)))),_tmp13FF))));}
_LL548: {struct Cyc_Absyn_JoinEff_struct*_tmpA41=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9F5;if(_tmpA41->tag != 25)goto _LL54A;else{_tmpA42=_tmpA41->f1;}}_LL549: {
struct Cyc_List_List*_tmpAA3=Cyc_Tcutil_substs(rgn,inst,_tmpA42);struct Cyc_Absyn_JoinEff_struct
_tmp1403;struct Cyc_Absyn_JoinEff_struct*_tmp1402;return _tmpAA3 == _tmpA42?t:(void*)((
_tmp1402=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1402)),((_tmp1402[0]=((
_tmp1403.tag=25,((_tmp1403.f1=_tmpAA3,_tmp1403)))),_tmp1402))));}_LL54A: {struct
Cyc_Absyn_TypeDeclType_struct*_tmpA43=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp9F5;if(_tmpA43->tag != 27)goto _LL513;}_LL54B: {const char*_tmp1406;void*
_tmp1405;(_tmp1405=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1406="found typedecltype in rsubs",_tag_dyneither(
_tmp1406,sizeof(char),28))),_tag_dyneither(_tmp1405,sizeof(void*),0)));}_LL513:;}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmpAA8=(void*)ts->hd;
struct Cyc_List_List*_tmpAA9=ts->tl;void*_tmpAAA=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpAA8);struct Cyc_List_List*_tmpAAB=Cyc_Tcutil_substs(rgn,inst,_tmpAA9);if(
_tmpAA8 == _tmpAAA  && _tmpAA9 == _tmpAAB)return ts;{struct Cyc_List_List*_tmp1407;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp1407=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1407)),((_tmp1407->hd=_tmpAAA,((_tmp1407->tl=_tmpAAB,_tmp1407)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpAAD=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp140A;struct _tuple15*_tmp1409;return(
_tmp1409=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1409)),((_tmp1409->f1=
tv,((_tmp1409->f2=Cyc_Absyn_new_evar(_tmpAAD,((_tmp140A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp140A)),((_tmp140A->v=s,_tmp140A))))),_tmp1409)))));}struct _tuple15*
Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,struct Cyc_Absyn_Tvar*tv){struct
_tuple16 _tmpAB1;struct Cyc_List_List*_tmpAB2;struct _RegionHandle*_tmpAB3;struct
_tuple16*_tmpAB0=env;_tmpAB1=*_tmpAB0;_tmpAB2=_tmpAB1.f1;_tmpAB3=_tmpAB1.f2;{
struct Cyc_Core_Opt*_tmpAB4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp140D;struct _tuple15*_tmp140C;return(_tmp140C=
_region_malloc(_tmpAB3,sizeof(*_tmp140C)),((_tmp140C->f1=tv,((_tmp140C->f2=Cyc_Absyn_new_evar(
_tmpAB4,((_tmp140D=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp140D)),((
_tmp140D->v=_tmpAB2,_tmp140D))))),_tmp140C)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp1413;void*_tmp1412[3];struct Cyc_String_pa_struct _tmp1411;struct Cyc_String_pa_struct
_tmp1410;struct Cyc_String_pa_struct _tmp140F;(_tmp140F.tag=0,((_tmp140F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp1410.tag=0,((_tmp1410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp1411.tag=0,((_tmp1411.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp1412[0]=& _tmp1411,((_tmp1412[1]=& _tmp1410,((_tmp1412[2]=&
_tmp140F,Cyc_Tcutil_terr(loc,((_tmp1413="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1413,sizeof(char),59))),_tag_dyneither(_tmp1412,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1416;void*_tmp1415;(_tmp1415=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1416="same type variable has different identity!",
_tag_dyneither(_tmp1416,sizeof(char),43))),_tag_dyneither(_tmp1415,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp1417;return(_tmp1417=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1417)),((
_tmp1417->hd=tv,((_tmp1417->tl=tvs,_tmp1417)))));};}static struct Cyc_List_List*
Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1){const char*_tmp141A;void*_tmp1419;(_tmp1419=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141A="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp141A,sizeof(char),39))),_tag_dyneither(_tmp1419,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAC1=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAC1->identity == - 1){const char*
_tmp141D;void*_tmp141C;(_tmp141C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141D="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp141D,sizeof(char),41))),_tag_dyneither(_tmp141C,sizeof(void*),
0)));}if(_tmpAC1->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp141E;return(_tmp141E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp141E)),((
_tmp141E->hd=tv,((_tmp141E->tl=tvs,_tmp141E)))));};}struct _tuple29{struct Cyc_Absyn_Tvar*
f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity
== - 1){const char*_tmp1421;void*_tmp1420;(_tmp1420=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1421="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1421,sizeof(char),44))),_tag_dyneither(_tmp1420,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple29
_tmpAC8;struct Cyc_Absyn_Tvar*_tmpAC9;int _tmpACA;int*_tmpACB;struct _tuple29*
_tmpAC7=(struct _tuple29*)tvs2->hd;_tmpAC8=*_tmpAC7;_tmpAC9=_tmpAC8.f1;_tmpACA=
_tmpAC8.f2;_tmpACB=(int*)&(*_tmpAC7).f2;if(_tmpAC9->identity == - 1){const char*
_tmp1424;void*_tmp1423;(_tmp1423=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1424="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1424,sizeof(char),46))),_tag_dyneither(_tmp1423,sizeof(void*),
0)));}if(_tmpAC9->identity == tv->identity){*_tmpACB=*_tmpACB  || b;return tvs;}}}{
struct _tuple29*_tmp1427;struct Cyc_List_List*_tmp1426;return(_tmp1426=
_region_malloc(r,sizeof(*_tmp1426)),((_tmp1426->hd=((_tmp1427=_region_malloc(r,
sizeof(*_tmp1427)),((_tmp1427->f1=tv,((_tmp1427->f2=b,_tmp1427)))))),((_tmp1426->tl=
tvs,_tmp1426)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp142B;void*_tmp142A[1];struct Cyc_String_pa_struct _tmp1429;(_tmp1429.tag=0,((
_tmp1429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp142A[0]=& _tmp1429,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp142B="bound tvar id for %s is NULL",
_tag_dyneither(_tmp142B,sizeof(char),29))),_tag_dyneither(_tmp142A,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp142C;return(_tmp142C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp142C)),((_tmp142C->hd=tv,((_tmp142C->tl=tvs,_tmp142C)))));};}struct
_tuple30{void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpAD4=Cyc_Tcutil_compress(
e);int _tmpAD6;_LL557: {struct Cyc_Absyn_Evar_struct*_tmpAD5=(struct Cyc_Absyn_Evar_struct*)
_tmpAD4;if(_tmpAD5->tag != 1)goto _LL559;else{_tmpAD6=_tmpAD5->f3;}}_LL558:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple30 _tmpAD8;void*
_tmpAD9;int _tmpADA;int*_tmpADB;struct _tuple30*_tmpAD7=(struct _tuple30*)es2->hd;
_tmpAD8=*_tmpAD7;_tmpAD9=_tmpAD8.f1;_tmpADA=_tmpAD8.f2;_tmpADB=(int*)&(*_tmpAD7).f2;{
void*_tmpADC=Cyc_Tcutil_compress(_tmpAD9);int _tmpADE;_LL55C: {struct Cyc_Absyn_Evar_struct*
_tmpADD=(struct Cyc_Absyn_Evar_struct*)_tmpADC;if(_tmpADD->tag != 1)goto _LL55E;
else{_tmpADE=_tmpADD->f3;}}_LL55D: if(_tmpAD6 == _tmpADE){if(b != *_tmpADB)*_tmpADB=
1;return es;}goto _LL55B;_LL55E:;_LL55F: goto _LL55B;_LL55B:;};}}{struct _tuple30*
_tmp142F;struct Cyc_List_List*_tmp142E;return(_tmp142E=_region_malloc(r,sizeof(*
_tmp142E)),((_tmp142E->hd=((_tmp142F=_region_malloc(r,sizeof(*_tmp142F)),((
_tmp142F->f1=e,((_tmp142F->f2=b,_tmp142F)))))),((_tmp142E->tl=es,_tmp142E)))));};
_LL559:;_LL55A: return es;_LL556:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1430;r=((_tmp1430=_region_malloc(rgn,sizeof(*_tmp1430)),((
_tmp1430->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1430->tl=r,_tmp1430))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAE3;int _tmpAE4;struct
_tuple29 _tmpAE2=*((struct _tuple29*)tvs->hd);_tmpAE3=_tmpAE2.f1;_tmpAE4=_tmpAE2.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpAE3->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1431;res=((_tmp1431=_region_malloc(r,sizeof(*_tmp1431)),((
_tmp1431->hd=(struct _tuple29*)tvs->hd,((_tmp1431->tl=res,_tmp1431))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp1435;void*_tmp1434[1];struct Cyc_String_pa_struct _tmp1433;(_tmp1433.tag=
0,((_tmp1433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1434[0]=&
_tmp1433,Cyc_Tcutil_terr(loc,((_tmp1435="bitfield %s does not have constant width",
_tag_dyneither(_tmp1435,sizeof(char),41))),_tag_dyneither(_tmp1434,sizeof(void*),
1)))))));}else{unsigned int _tmpAEA;int _tmpAEB;struct _tuple13 _tmpAE9=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAEA=_tmpAE9.f1;_tmpAEB=_tmpAE9.f2;if(!_tmpAEB){
const char*_tmp1438;void*_tmp1437;(_tmp1437=0,Cyc_Tcutil_terr(loc,((_tmp1438="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp1438,sizeof(char),45))),_tag_dyneither(_tmp1437,sizeof(void*),
0)));}w=_tmpAEA;}{void*_tmpAEE=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAF0;_LL561: {struct Cyc_Absyn_IntType_struct*_tmpAEF=(struct Cyc_Absyn_IntType_struct*)
_tmpAEE;if(_tmpAEF->tag != 6)goto _LL563;else{_tmpAF0=_tmpAEF->f2;}}_LL562: switch(
_tmpAF0){case Cyc_Absyn_Char_sz: _LL565: if(w > 8){const char*_tmp143B;void*_tmp143A;(
_tmp143A=0,Cyc_Tcutil_terr(loc,((_tmp143B="bitfield larger than type",
_tag_dyneither(_tmp143B,sizeof(char),26))),_tag_dyneither(_tmp143A,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL566: if(w > 16){const char*_tmp143E;void*
_tmp143D;(_tmp143D=0,Cyc_Tcutil_terr(loc,((_tmp143E="bitfield larger than type",
_tag_dyneither(_tmp143E,sizeof(char),26))),_tag_dyneither(_tmp143D,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL567: goto _LL568;case Cyc_Absyn_Int_sz: _LL568:
if(w > 32){const char*_tmp1441;void*_tmp1440;(_tmp1440=0,Cyc_Tcutil_terr(loc,((
_tmp1441="bitfield larger than type",_tag_dyneither(_tmp1441,sizeof(char),26))),
_tag_dyneither(_tmp1440,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL569: if(w > 64){const char*_tmp1444;void*_tmp1443;(_tmp1443=0,Cyc_Tcutil_terr(
loc,((_tmp1444="bitfield larger than type",_tag_dyneither(_tmp1444,sizeof(char),
26))),_tag_dyneither(_tmp1443,sizeof(void*),0)));}break;}goto _LL560;_LL563:;
_LL564:{const char*_tmp1449;void*_tmp1448[2];struct Cyc_String_pa_struct _tmp1447;
struct Cyc_String_pa_struct _tmp1446;(_tmp1446.tag=0,((_tmp1446.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp1447.tag=0,((_tmp1447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp1448[0]=& _tmp1447,((_tmp1448[1]=& _tmp1446,Cyc_Tcutil_terr(loc,((_tmp1449="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1449,sizeof(char),52))),_tag_dyneither(_tmp1448,sizeof(void*),
2)))))))))))));}goto _LL560;_LL560:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpAFD=(void*)atts->hd;_LL56C: {struct Cyc_Absyn_Packed_att_struct*
_tmpAFE=(struct Cyc_Absyn_Packed_att_struct*)_tmpAFD;if(_tmpAFE->tag != 7)goto
_LL56E;}_LL56D: continue;_LL56E: {struct Cyc_Absyn_Aligned_att_struct*_tmpAFF=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpAFD;if(_tmpAFF->tag != 6)goto _LL570;}
_LL56F: continue;_LL570:;_LL571: {const char*_tmp144E;void*_tmp144D[2];struct Cyc_String_pa_struct
_tmp144C;struct Cyc_String_pa_struct _tmp144B;(_tmp144B.tag=0,((_tmp144B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp144C.tag=0,((_tmp144C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp144D[0]=& _tmp144C,((_tmp144D[1]=& _tmp144B,Cyc_Tcutil_terr(loc,((
_tmp144E="bad attribute %s on member %s",_tag_dyneither(_tmp144E,sizeof(char),30))),
_tag_dyneither(_tmp144D,sizeof(void*),2)))))))))))));}_LL56B:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpB04=t;struct
Cyc_Absyn_Typedefdecl*_tmpB06;void**_tmpB07;_LL573: {struct Cyc_Absyn_TypedefType_struct*
_tmpB05=(struct Cyc_Absyn_TypedefType_struct*)_tmpB04;if(_tmpB05->tag != 18)goto
_LL575;else{_tmpB06=_tmpB05->f3;_tmpB07=_tmpB05->f4;}}_LL574: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpB06))->tq).real_const  || (_tmpB06->tq).print_const){if(
declared_const){const char*_tmp1451;void*_tmp1450;(_tmp1450=0,Cyc_Tcutil_warn(loc,((
_tmp1451="extra const",_tag_dyneither(_tmp1451,sizeof(char),12))),_tag_dyneither(
_tmp1450,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpB07)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpB07);else{return declared_const;}_LL575:;_LL576: return
declared_const;_LL572:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpB0A=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpB0C;struct Cyc_Absyn_PtrAtts _tmpB0D;void*_tmpB0E;
_LL578: {struct Cyc_Absyn_PointerType_struct*_tmpB0B=(struct Cyc_Absyn_PointerType_struct*)
_tmpB0A;if(_tmpB0B->tag != 5)goto _LL57A;else{_tmpB0C=_tmpB0B->f1;_tmpB0D=_tmpB0C.ptr_atts;
_tmpB0E=_tmpB0D.rgn;}}_LL579:{void*_tmpB0F=Cyc_Tcutil_compress(_tmpB0E);struct
Cyc_Absyn_Tvar*_tmpB11;_LL57D: {struct Cyc_Absyn_VarType_struct*_tmpB10=(struct
Cyc_Absyn_VarType_struct*)_tmpB0F;if(_tmpB10->tag != 2)goto _LL57F;else{_tmpB11=
_tmpB10->f1;}}_LL57E: return Cyc_Absyn_tvar_cmp(tvar,_tmpB11)== 0;_LL57F:;_LL580:
goto _LL57C;_LL57C:;}goto _LL577;_LL57A:;_LL57B: goto _LL577;_LL577:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpB12=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB14;
struct Cyc_Absyn_Kind _tmpB15;enum Cyc_Absyn_KindQual _tmpB16;enum Cyc_Absyn_AliasQual
_tmpB17;struct Cyc_Absyn_Kind*_tmpB19;struct Cyc_Absyn_Kind _tmpB1A;enum Cyc_Absyn_KindQual
_tmpB1B;enum Cyc_Absyn_AliasQual _tmpB1C;_LL582: {struct Cyc_Absyn_Less_kb_struct*
_tmpB13=(struct Cyc_Absyn_Less_kb_struct*)_tmpB12;if(_tmpB13->tag != 2)goto _LL584;
else{_tmpB14=_tmpB13->f2;_tmpB15=*_tmpB14;_tmpB16=_tmpB15.kind;if(_tmpB16 != Cyc_Absyn_RgnKind)
goto _LL584;_tmpB17=_tmpB15.aliasqual;if(_tmpB17 != Cyc_Absyn_Top)goto _LL584;}}
_LL583: goto _LL585;_LL584: {struct Cyc_Absyn_Eq_kb_struct*_tmpB18=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB12;if(_tmpB18->tag != 0)goto _LL586;else{_tmpB19=_tmpB18->f1;_tmpB1A=*_tmpB19;
_tmpB1B=_tmpB1A.kind;if(_tmpB1B != Cyc_Absyn_RgnKind)goto _LL586;_tmpB1C=_tmpB1A.aliasqual;
if(_tmpB1C != Cyc_Absyn_Top)goto _LL586;}}_LL585: if(((expected_kind->kind == Cyc_Absyn_BoxKind
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
_tmpB1D=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB20;struct Cyc_Core_Opt**
_tmpB21;struct Cyc_Core_Opt*_tmpB22;struct Cyc_Core_Opt**_tmpB23;struct Cyc_Absyn_Tvar*
_tmpB25;struct Cyc_Absyn_TypeDecl*_tmpB27;struct Cyc_Absyn_TypeDecl _tmpB28;void*
_tmpB29;void**_tmpB2A;void***_tmpB2B;struct Cyc_List_List*_tmpB2D;struct _tuple2*
_tmpB2F;struct Cyc_Absyn_Enumdecl*_tmpB30;struct Cyc_Absyn_Enumdecl**_tmpB31;
struct Cyc_Absyn_DatatypeInfo _tmpB33;union Cyc_Absyn_DatatypeInfoU _tmpB34;union Cyc_Absyn_DatatypeInfoU*
_tmpB35;struct Cyc_List_List*_tmpB36;struct Cyc_List_List**_tmpB37;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB39;union Cyc_Absyn_DatatypeFieldInfoU _tmpB3A;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB3B;struct Cyc_List_List*_tmpB3C;struct Cyc_Absyn_PtrInfo _tmpB3E;void*_tmpB3F;
struct Cyc_Absyn_Tqual _tmpB40;struct Cyc_Absyn_Tqual*_tmpB41;struct Cyc_Absyn_PtrAtts
_tmpB42;void*_tmpB43;union Cyc_Absyn_Constraint*_tmpB44;union Cyc_Absyn_Constraint*
_tmpB45;union Cyc_Absyn_Constraint*_tmpB46;void*_tmpB48;struct Cyc_Absyn_Exp*
_tmpB4A;struct Cyc_Absyn_ArrayInfo _tmpB4F;void*_tmpB50;struct Cyc_Absyn_Tqual
_tmpB51;struct Cyc_Absyn_Tqual*_tmpB52;struct Cyc_Absyn_Exp*_tmpB53;struct Cyc_Absyn_Exp**
_tmpB54;union Cyc_Absyn_Constraint*_tmpB55;struct Cyc_Position_Segment*_tmpB56;
struct Cyc_Absyn_FnInfo _tmpB58;struct Cyc_List_List*_tmpB59;struct Cyc_List_List**
_tmpB5A;struct Cyc_Core_Opt*_tmpB5B;struct Cyc_Core_Opt**_tmpB5C;struct Cyc_Absyn_Tqual
_tmpB5D;struct Cyc_Absyn_Tqual*_tmpB5E;void*_tmpB5F;struct Cyc_List_List*_tmpB60;
int _tmpB61;struct Cyc_Absyn_VarargInfo*_tmpB62;struct Cyc_List_List*_tmpB63;struct
Cyc_List_List*_tmpB64;struct Cyc_List_List*_tmpB66;enum Cyc_Absyn_AggrKind _tmpB68;
struct Cyc_List_List*_tmpB69;struct Cyc_Absyn_AggrInfo _tmpB6B;union Cyc_Absyn_AggrInfoU
_tmpB6C;union Cyc_Absyn_AggrInfoU*_tmpB6D;struct Cyc_List_List*_tmpB6E;struct Cyc_List_List**
_tmpB6F;struct _tuple2*_tmpB71;struct Cyc_List_List*_tmpB72;struct Cyc_List_List**
_tmpB73;struct Cyc_Absyn_Typedefdecl*_tmpB74;struct Cyc_Absyn_Typedefdecl**_tmpB75;
void**_tmpB76;void***_tmpB77;void*_tmpB7C;void*_tmpB7E;void*_tmpB7F;void*_tmpB81;
void*_tmpB83;struct Cyc_List_List*_tmpB85;_LL589: {struct Cyc_Absyn_VoidType_struct*
_tmpB1E=(struct Cyc_Absyn_VoidType_struct*)_tmpB1D;if(_tmpB1E->tag != 0)goto _LL58B;}
_LL58A: goto _LL588;_LL58B: {struct Cyc_Absyn_Evar_struct*_tmpB1F=(struct Cyc_Absyn_Evar_struct*)
_tmpB1D;if(_tmpB1F->tag != 1)goto _LL58D;else{_tmpB20=_tmpB1F->f1;_tmpB21=(struct
Cyc_Core_Opt**)& _tmpB1F->f1;_tmpB22=_tmpB1F->f2;_tmpB23=(struct Cyc_Core_Opt**)&
_tmpB1F->f2;}}_LL58C: if(*_tmpB21 == 0  || Cyc_Tcutil_kind_leq(expected_kind,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB21))->v) && !Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB21))->v,
expected_kind))*_tmpB21=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result
 && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){if(
expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB23=(struct Cyc_Core_Opt*)& urgn;
else{*_tmpB23=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmp1454;struct Cyc_Absyn_Less_kb_struct*_tmp1453;
struct Cyc_Absyn_Tvar*_tmpB86=Cyc_Tcutil_new_tvar((void*)((_tmp1453=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1453)),((_tmp1453[0]=((
_tmp1454.tag=2,((_tmp1454.f1=0,((_tmp1454.f2=expected_kind,_tmp1454)))))),
_tmp1453)))));{struct Cyc_Absyn_VarType_struct*_tmp145A;struct Cyc_Absyn_VarType_struct
_tmp1459;struct Cyc_Core_Opt*_tmp1458;*_tmpB23=((_tmp1458=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1458)),((_tmp1458->v=(void*)((_tmp145A=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp145A)),((_tmp145A[0]=((_tmp1459.tag=2,((_tmp1459.f1=_tmpB86,_tmp1459)))),
_tmp145A)))),_tmp1458))));}_tmpB25=_tmpB86;goto _LL58E;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL588;_LL58D: {struct Cyc_Absyn_VarType_struct*_tmpB24=(struct Cyc_Absyn_VarType_struct*)
_tmpB1D;if(_tmpB24->tag != 2)goto _LL58F;else{_tmpB25=_tmpB24->f1;}}_LL58E:{void*
_tmpB8C=Cyc_Absyn_compress_kb(_tmpB25->kind);struct Cyc_Core_Opt*_tmpB8E;struct
Cyc_Core_Opt**_tmpB8F;_LL5C2: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB8D=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmpB8C;if(_tmpB8D->tag != 1)goto _LL5C4;else{_tmpB8E=
_tmpB8D->f1;_tmpB8F=(struct Cyc_Core_Opt**)& _tmpB8D->f1;}}_LL5C3:{struct Cyc_Absyn_Less_kb_struct*
_tmp1460;struct Cyc_Absyn_Less_kb_struct _tmp145F;struct Cyc_Core_Opt*_tmp145E;*
_tmpB8F=((_tmp145E=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp145E)),((
_tmp145E->v=(void*)((_tmp1460=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1460)),((_tmp1460[0]=((_tmp145F.tag=2,((_tmp145F.f1=0,((_tmp145F.f2=
expected_kind,_tmp145F)))))),_tmp1460)))),_tmp145E))));}goto _LL5C1;_LL5C4:;
_LL5C5: goto _LL5C1;_LL5C1:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,
_tmpB25);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB25,put_in_effect);{void*_tmpB93=Cyc_Absyn_compress_kb(_tmpB25->kind);struct
Cyc_Core_Opt*_tmpB95;struct Cyc_Core_Opt**_tmpB96;struct Cyc_Absyn_Kind*_tmpB97;
_LL5C7: {struct Cyc_Absyn_Less_kb_struct*_tmpB94=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB93;if(_tmpB94->tag != 2)goto _LL5C9;else{_tmpB95=_tmpB94->f1;_tmpB96=(struct
Cyc_Core_Opt**)& _tmpB94->f1;_tmpB97=_tmpB94->f2;}}_LL5C8: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB97)){struct Cyc_Absyn_Less_kb_struct*_tmp1466;struct Cyc_Absyn_Less_kb_struct
_tmp1465;struct Cyc_Core_Opt*_tmp1464;*_tmpB96=((_tmp1464=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1464)),((_tmp1464->v=(void*)((_tmp1466=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1466)),((_tmp1466[0]=((_tmp1465.tag=2,((_tmp1465.f1=0,((_tmp1465.f2=
expected_kind,_tmp1465)))))),_tmp1466)))),_tmp1464))));}goto _LL5C6;_LL5C9:;
_LL5CA: goto _LL5C6;_LL5C6:;}goto _LL588;_LL58F: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpB26=(struct Cyc_Absyn_TypeDeclType_struct*)_tmpB1D;if(_tmpB26->tag != 27)goto
_LL591;else{_tmpB27=_tmpB26->f1;_tmpB28=*_tmpB27;_tmpB29=_tmpB28.r;_tmpB2A=
_tmpB26->f2;_tmpB2B=(void***)& _tmpB26->f2;}}_LL590: {void*new_t=Cyc_Tcutil_copy_type(
Cyc_Tcutil_compress(t));{void*_tmpB9B=_tmpB29;struct Cyc_Absyn_Aggrdecl*_tmpB9D;
struct Cyc_Absyn_Enumdecl*_tmpB9F;struct Cyc_Absyn_Datatypedecl*_tmpBA1;_LL5CC: {
struct Cyc_Absyn_Aggr_td_struct*_tmpB9C=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB9B;
if(_tmpB9C->tag != 0)goto _LL5CE;else{_tmpB9D=_tmpB9C->f1;}}_LL5CD: Cyc_Tc_tcAggrdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB9D);goto _LL5CB;_LL5CE: {struct Cyc_Absyn_Enum_td_struct*
_tmpB9E=(struct Cyc_Absyn_Enum_td_struct*)_tmpB9B;if(_tmpB9E->tag != 1)goto _LL5D0;
else{_tmpB9F=_tmpB9E->f1;}}_LL5CF: Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB9F);goto _LL5CB;_LL5D0: {struct Cyc_Absyn_Datatype_td_struct*_tmpBA0=(struct
Cyc_Absyn_Datatype_td_struct*)_tmpB9B;if(_tmpBA0->tag != 2)goto _LL5CB;else{
_tmpBA1=_tmpBA0->f1;}}_LL5D1: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBA1);goto _LL5CB;_LL5CB:;}{void**_tmp1467;*_tmpB2B=((_tmp1467=_region_malloc(
Cyc_Core_unique_region,sizeof(*_tmp1467)),((_tmp1467[0]=new_t,_tmp1467))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,
put_in_effect);}_LL591: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB2C=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmpB1D;if(_tmpB2C->tag != 15)goto _LL593;else{_tmpB2D=_tmpB2C->f1;}}_LL592: {
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct _RegionHandle*_tmpBA3=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;for(0;_tmpB2D != 0;
_tmpB2D=_tmpB2D->tl){struct Cyc_Absyn_Enumfield*_tmpBA4=(struct Cyc_Absyn_Enumfield*)
_tmpB2D->hd;if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,
prev_fields,(*_tmpBA4->name).f2)){const char*_tmp146B;void*_tmp146A[1];struct Cyc_String_pa_struct
_tmp1469;(_tmp1469.tag=0,((_tmp1469.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*(*_tmpBA4->name).f2),((_tmp146A[0]=& _tmp1469,Cyc_Tcutil_terr(
_tmpBA4->loc,((_tmp146B="duplicate enum field name %s",_tag_dyneither(_tmp146B,
sizeof(char),29))),_tag_dyneither(_tmp146A,sizeof(void*),1)))))));}else{struct
Cyc_List_List*_tmp146C;prev_fields=((_tmp146C=_region_malloc(_tmpBA3,sizeof(*
_tmp146C)),((_tmp146C->hd=(*_tmpBA4->name).f2,((_tmp146C->tl=prev_fields,
_tmp146C))))));}if(_tmpBA4->tag == 0)_tmpBA4->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmpBA4->loc);else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmpBA4->tag))){const char*_tmp1470;void*_tmp146F[1];struct Cyc_String_pa_struct
_tmp146E;(_tmp146E.tag=0,((_tmp146E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*(*_tmpBA4->name).f2),((_tmp146F[0]=& _tmp146E,Cyc_Tcutil_terr(loc,((
_tmp1470="enum field %s: expression is not constant",_tag_dyneither(_tmp1470,
sizeof(char),42))),_tag_dyneither(_tmp146F,sizeof(void*),1)))))));}}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmpBA4->tag))).f1;tag_count=t1 + 1;(*_tmpBA4->name).f1=Cyc_Absyn_Abs_n(te->ns,0);{
struct Cyc_Tcenv_AnonEnumRes_struct*_tmp1476;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp1475;struct _tuple30*_tmp1474;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple30*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpBA4->name).f2,(struct _tuple30*)((_tmp1474=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1474)),((_tmp1474->f1=(void*)((_tmp1476=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1476)),((_tmp1476[0]=((_tmp1475.tag=4,((_tmp1475.f1=(void*)t,((
_tmp1475.f2=_tmpBA4,_tmp1475)))))),_tmp1476)))),((_tmp1474->f2=1,_tmp1474)))))));};};}}
goto _LL588;}_LL593: {struct Cyc_Absyn_EnumType_struct*_tmpB2E=(struct Cyc_Absyn_EnumType_struct*)
_tmpB1D;if(_tmpB2E->tag != 14)goto _LL595;else{_tmpB2F=_tmpB2E->f1;_tmpB30=_tmpB2E->f2;
_tmpB31=(struct Cyc_Absyn_Enumdecl**)& _tmpB2E->f2;}}_LL594: if(*_tmpB31 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB31))->fields == 0){struct
_handler_cons _tmpBAF;_push_handler(& _tmpBAF);{int _tmpBB1=0;if(setjmp(_tmpBAF.handler))
_tmpBB1=1;if(!_tmpBB1){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB2F);*_tmpB31=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpBB0=(void*)_exn_thrown;void*_tmpBB3=_tmpBB0;_LL5D3: {struct Cyc_Dict_Absent_struct*
_tmpBB4=(struct Cyc_Dict_Absent_struct*)_tmpBB3;if(_tmpBB4->tag != Cyc_Dict_Absent)
goto _LL5D5;}_LL5D4: {struct Cyc_Tcenv_Genv*_tmpBB5=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1477;struct Cyc_Absyn_Enumdecl*_tmpBB6=(_tmp1477=
_cycalloc(sizeof(*_tmp1477)),((_tmp1477->sc=Cyc_Absyn_Extern,((_tmp1477->name=
_tmpB2F,((_tmp1477->fields=0,_tmp1477)))))));Cyc_Tc_tcEnumdecl(te,_tmpBB5,loc,
_tmpBB6);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB2F);*
_tmpB31=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5D2;};}_LL5D5:;_LL5D6:(void)_throw(
_tmpBB3);_LL5D2:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB31);*_tmpB2F=(ed->name)[0];goto _LL588;};_LL595: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB32=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB1D;if(_tmpB32->tag != 3)goto
_LL597;else{_tmpB33=_tmpB32->f1;_tmpB34=_tmpB33.datatype_info;_tmpB35=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB32->f1).datatype_info;_tmpB36=_tmpB33.targs;_tmpB37=(struct Cyc_List_List**)&(
_tmpB32->f1).targs;}}_LL596: {struct Cyc_List_List*_tmpBB8=*_tmpB37;{union Cyc_Absyn_DatatypeInfoU
_tmpBB9=*_tmpB35;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBBA;struct _tuple2*
_tmpBBB;int _tmpBBC;struct Cyc_Absyn_Datatypedecl**_tmpBBD;struct Cyc_Absyn_Datatypedecl*
_tmpBBE;_LL5D8: if((_tmpBB9.UnknownDatatype).tag != 1)goto _LL5DA;_tmpBBA=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBB9.UnknownDatatype).val;_tmpBBB=_tmpBBA.name;
_tmpBBC=_tmpBBA.is_extensible;_LL5D9: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBBF;_push_handler(& _tmpBBF);{int _tmpBC1=0;if(setjmp(
_tmpBBF.handler))_tmpBC1=1;if(!_tmpBC1){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBBB);;_pop_handler();}else{void*_tmpBC0=(void*)_exn_thrown;void*_tmpBC3=
_tmpBC0;_LL5DD: {struct Cyc_Dict_Absent_struct*_tmpBC4=(struct Cyc_Dict_Absent_struct*)
_tmpBC3;if(_tmpBC4->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {struct Cyc_Tcenv_Genv*
_tmpBC5=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp1478;struct Cyc_Absyn_Datatypedecl*
_tmpBC6=(_tmp1478=_cycalloc(sizeof(*_tmp1478)),((_tmp1478->sc=Cyc_Absyn_Extern,((
_tmp1478->name=_tmpBBB,((_tmp1478->tvs=0,((_tmp1478->fields=0,((_tmp1478->is_extensible=
_tmpBBC,_tmp1478)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBC5,loc,_tmpBC6);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBBB);if(_tmpBB8 != 0){{const char*
_tmp147C;void*_tmp147B[1];struct Cyc_String_pa_struct _tmp147A;(_tmp147A.tag=0,((
_tmp147A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBBB)),((_tmp147B[0]=& _tmp147A,Cyc_Tcutil_terr(loc,((_tmp147C="declare parameterized datatype %s before using",
_tag_dyneither(_tmp147C,sizeof(char),47))),_tag_dyneither(_tmp147B,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5DC;}_LL5DF:;_LL5E0:(void)_throw(_tmpBC3);_LL5DC:;}};}
if(_tmpBBC  && !(*tudp)->is_extensible){const char*_tmp1480;void*_tmp147F[1];
struct Cyc_String_pa_struct _tmp147E;(_tmp147E.tag=0,((_tmp147E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBB)),((
_tmp147F[0]=& _tmp147E,Cyc_Tcutil_terr(loc,((_tmp1480="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1480,sizeof(char),41))),_tag_dyneither(_tmp147F,sizeof(void*),
1)))))));}*_tmpB35=Cyc_Absyn_KnownDatatype(tudp);_tmpBBE=*tudp;goto _LL5DB;}
_LL5DA: if((_tmpBB9.KnownDatatype).tag != 2)goto _LL5D7;_tmpBBD=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBB9.KnownDatatype).val;_tmpBBE=*_tmpBBD;_LL5DB: {struct Cyc_List_List*tvs=
_tmpBBE->tvs;for(0;_tmpBB8 != 0  && tvs != 0;(_tmpBB8=_tmpBB8->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBB8->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1481;struct _tuple0 _tmpBCF=(_tmp1481.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1481.f2=t,_tmp1481)));void*_tmpBD0;void*_tmpBD2;struct Cyc_Absyn_Tvar*_tmpBD4;
_LL5E2: _tmpBD0=_tmpBCF.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBD1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBD0;if(_tmpBD1->tag != 1)goto _LL5E4;};_tmpBD2=_tmpBCF.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBD3=(struct Cyc_Absyn_VarType_struct*)_tmpBD2;if(_tmpBD3->tag != 2)goto _LL5E4;
else{_tmpBD4=_tmpBD3->f1;}};_LL5E3: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBD4);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBD4,1);continue;_LL5E4:;_LL5E5: goto _LL5E1;_LL5E1:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBB8 != 0){const
char*_tmp1485;void*_tmp1484[1];struct Cyc_String_pa_struct _tmp1483;(_tmp1483.tag=
0,((_tmp1483.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBBE->name)),((_tmp1484[0]=& _tmp1483,Cyc_Tcutil_terr(loc,((_tmp1485="too many type arguments for datatype %s",
_tag_dyneither(_tmp1485,sizeof(char),40))),_tag_dyneither(_tmp1484,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp1486;hidden_ts=((_tmp1486=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1486)),((_tmp1486->hd=e,((_tmp1486->tl=hidden_ts,_tmp1486))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,e,1);}*_tmpB37=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB37,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5D7;}_LL5D7:;}goto
_LL588;}_LL597: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB38=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB1D;if(_tmpB38->tag != 4)goto _LL599;else{_tmpB39=_tmpB38->f1;_tmpB3A=_tmpB39.field_info;
_tmpB3B=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB38->f1).field_info;_tmpB3C=
_tmpB39.targs;}}_LL598:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBD9=*_tmpB3B;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBDA;struct _tuple2*_tmpBDB;struct _tuple2*
_tmpBDC;int _tmpBDD;struct _tuple3 _tmpBDE;struct Cyc_Absyn_Datatypedecl*_tmpBDF;
struct Cyc_Absyn_Datatypefield*_tmpBE0;_LL5E7: if((_tmpBD9.UnknownDatatypefield).tag
!= 1)goto _LL5E9;_tmpBDA=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBD9.UnknownDatatypefield).val;
_tmpBDB=_tmpBDA.datatype_name;_tmpBDC=_tmpBDA.field_name;_tmpBDD=_tmpBDA.is_extensible;
_LL5E8: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBE1;_push_handler(& _tmpBE1);{int _tmpBE3=0;if(setjmp(
_tmpBE1.handler))_tmpBE3=1;if(!_tmpBE3){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBDB);;_pop_handler();}else{void*_tmpBE2=(void*)_exn_thrown;void*_tmpBE5=
_tmpBE2;_LL5EC: {struct Cyc_Dict_Absent_struct*_tmpBE6=(struct Cyc_Dict_Absent_struct*)
_tmpBE5;if(_tmpBE6->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:{const char*_tmp148A;
void*_tmp1489[1];struct Cyc_String_pa_struct _tmp1488;(_tmp1488.tag=0,((_tmp1488.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDB)),((
_tmp1489[0]=& _tmp1488,Cyc_Tcutil_terr(loc,((_tmp148A="unbound datatype %s",
_tag_dyneither(_tmp148A,sizeof(char),20))),_tag_dyneither(_tmp1489,sizeof(void*),
1)))))));}return cvtenv;_LL5EE:;_LL5EF:(void)_throw(_tmpBE5);_LL5EB:;}};}{struct
_handler_cons _tmpBEA;_push_handler(& _tmpBEA);{int _tmpBEC=0;if(setjmp(_tmpBEA.handler))
_tmpBEC=1;if(!_tmpBEC){{struct _RegionHandle*_tmpBED=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBEE=Cyc_Tcenv_lookup_ordinary(_tmpBED,te,loc,_tmpBDC);struct Cyc_Absyn_Datatypedecl*
_tmpBF0;struct Cyc_Absyn_Datatypefield*_tmpBF1;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBEF=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBEE;if(_tmpBEF->tag != 2)goto
_LL5F3;else{_tmpBF0=_tmpBEF->f1;_tmpBF1=_tmpBEF->f2;}}_LL5F2: tuf=_tmpBF1;tud=
_tmpBF0;if(_tmpBDD  && !tud->is_extensible){const char*_tmp148E;void*_tmp148D[1];
struct Cyc_String_pa_struct _tmp148C;(_tmp148C.tag=0,((_tmp148C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDB)),((
_tmp148D[0]=& _tmp148C,Cyc_Tcutil_terr(loc,((_tmp148E="datatype %s was not declared @extensible",
_tag_dyneither(_tmp148E,sizeof(char),41))),_tag_dyneither(_tmp148D,sizeof(void*),
1)))))));}goto _LL5F0;_LL5F3:;_LL5F4:{const char*_tmp1493;void*_tmp1492[2];struct
Cyc_String_pa_struct _tmp1491;struct Cyc_String_pa_struct _tmp1490;(_tmp1490.tag=0,((
_tmp1490.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBDB)),((_tmp1491.tag=0,((_tmp1491.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDC)),((_tmp1492[0]=& _tmp1491,((
_tmp1492[1]=& _tmp1490,Cyc_Tcutil_terr(loc,((_tmp1493="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1493,sizeof(char),37))),_tag_dyneither(_tmp1492,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBF9=cvtenv;_npop_handler(0);return
_tmpBF9;};_LL5F0:;};_pop_handler();}else{void*_tmpBEB=(void*)_exn_thrown;void*
_tmpBFB=_tmpBEB;_LL5F6: {struct Cyc_Dict_Absent_struct*_tmpBFC=(struct Cyc_Dict_Absent_struct*)
_tmpBFB;if(_tmpBFC->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:{const char*_tmp1498;
void*_tmp1497[2];struct Cyc_String_pa_struct _tmp1496;struct Cyc_String_pa_struct
_tmp1495;(_tmp1495.tag=0,((_tmp1495.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDB)),((_tmp1496.tag=0,((_tmp1496.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDC)),((
_tmp1497[0]=& _tmp1496,((_tmp1497[1]=& _tmp1495,Cyc_Tcutil_terr(loc,((_tmp1498="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1498,sizeof(char),37))),_tag_dyneither(_tmp1497,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5F8:;_LL5F9:(void)_throw(_tmpBFB);_LL5F5:;}};}*
_tmpB3B=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBDF=tud;_tmpBE0=tuf;goto _LL5EA;}
_LL5E9: if((_tmpBD9.KnownDatatypefield).tag != 2)goto _LL5E6;_tmpBDE=(struct _tuple3)(
_tmpBD9.KnownDatatypefield).val;_tmpBDF=_tmpBDE.f1;_tmpBE0=_tmpBDE.f2;_LL5EA: {
struct Cyc_List_List*tvs=_tmpBDF->tvs;for(0;_tmpB3C != 0  && tvs != 0;(_tmpB3C=
_tmpB3C->tl,tvs=tvs->tl)){void*t=(void*)_tmpB3C->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp1499;struct _tuple0 _tmpC02=(
_tmp1499.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1499.f2=t,_tmp1499)));void*
_tmpC03;void*_tmpC05;struct Cyc_Absyn_Tvar*_tmpC07;_LL5FB: _tmpC03=_tmpC02.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpC04=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC03;if(_tmpC04->tag != 1)goto _LL5FD;};_tmpC05=_tmpC02.f2;{struct Cyc_Absyn_VarType_struct*
_tmpC06=(struct Cyc_Absyn_VarType_struct*)_tmpC05;if(_tmpC06->tag != 2)goto _LL5FD;
else{_tmpC07=_tmpC06->f1;}};_LL5FC: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpC07);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpC07,1);continue;_LL5FD:;_LL5FE: goto _LL5FA;_LL5FA:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB3C != 0){const
char*_tmp149E;void*_tmp149D[2];struct Cyc_String_pa_struct _tmp149C;struct Cyc_String_pa_struct
_tmp149B;(_tmp149B.tag=0,((_tmp149B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE0->name)),((_tmp149C.tag=0,((
_tmp149C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBDF->name)),((_tmp149D[0]=& _tmp149C,((_tmp149D[1]=& _tmp149B,Cyc_Tcutil_terr(
loc,((_tmp149E="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp149E,sizeof(char),43))),_tag_dyneither(_tmp149D,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp14A3;void*_tmp14A2[2];struct Cyc_String_pa_struct
_tmp14A1;struct Cyc_String_pa_struct _tmp14A0;(_tmp14A0.tag=0,((_tmp14A0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE0->name)),((
_tmp14A1.tag=0,((_tmp14A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBDF->name)),((_tmp14A2[0]=& _tmp14A1,((_tmp14A2[1]=& _tmp14A0,Cyc_Tcutil_terr(
loc,((_tmp14A3="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp14A3,sizeof(char),42))),_tag_dyneither(_tmp14A2,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}goto _LL588;_LL599: {struct Cyc_Absyn_PointerType_struct*
_tmpB3D=(struct Cyc_Absyn_PointerType_struct*)_tmpB1D;if(_tmpB3D->tag != 5)goto
_LL59B;else{_tmpB3E=_tmpB3D->f1;_tmpB3F=_tmpB3E.elt_typ;_tmpB40=_tmpB3E.elt_tq;
_tmpB41=(struct Cyc_Absyn_Tqual*)&(_tmpB3D->f1).elt_tq;_tmpB42=_tmpB3E.ptr_atts;
_tmpB43=_tmpB42.rgn;_tmpB44=_tmpB42.nullable;_tmpB45=_tmpB42.bounds;_tmpB46=
_tmpB42.zero_term;}}_LL59A: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB3F,1);_tmpB41->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB41->print_const,_tmpB3F);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5FF: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL600: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB43,1);{union Cyc_Absyn_Constraint*
_tmpC10=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB46);union Cyc_Absyn_Constraint _tmpC11;int _tmpC12;union Cyc_Absyn_Constraint
_tmpC13;int _tmpC14;_LL604: _tmpC11=*_tmpC10;if((_tmpC11.No_constr).tag != 3)goto
_LL606;_tmpC12=(int)(_tmpC11.No_constr).val;_LL605:{void*_tmpC15=Cyc_Tcutil_compress(
_tmpB3F);enum Cyc_Absyn_Size_of _tmpC17;_LL60B: {struct Cyc_Absyn_IntType_struct*
_tmpC16=(struct Cyc_Absyn_IntType_struct*)_tmpC15;if(_tmpC16->tag != 6)goto _LL60D;
else{_tmpC17=_tmpC16->f2;if(_tmpC17 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB46,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL60A;_LL60D:;_LL60E:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB46,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL60A;
_LL60A:;}goto _LL603;_LL606: _tmpC13=*_tmpC10;if((_tmpC13.Eq_constr).tag != 1)goto
_LL608;_tmpC14=(int)(_tmpC13.Eq_constr).val;if(_tmpC14 != 1)goto _LL608;_LL607: if(
!Cyc_Tcutil_admits_zero(_tmpB3F)){const char*_tmp14A7;void*_tmp14A6[1];struct Cyc_String_pa_struct
_tmp14A5;(_tmp14A5.tag=0,((_tmp14A5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB3F)),((_tmp14A6[0]=& _tmp14A5,Cyc_Tcutil_terr(
loc,((_tmp14A7="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp14A7,sizeof(char),53))),_tag_dyneither(_tmp14A6,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL603;_LL608:;_LL609: is_zero_terminated=0;
goto _LL603;_LL603:;}{void*_tmpC1B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB45);struct Cyc_Absyn_Exp*
_tmpC1E;_LL610: {struct Cyc_Absyn_DynEither_b_struct*_tmpC1C=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC1B;if(_tmpC1C->tag != 0)goto _LL612;}_LL611: goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_struct*
_tmpC1D=(struct Cyc_Absyn_Upper_b_struct*)_tmpC1B;if(_tmpC1D->tag != 1)goto _LL60F;
else{_tmpC1E=_tmpC1D->f1;}}_LL613: {struct _RegionHandle*_tmpC1F=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC20=Cyc_Tcenv_allow_valueof(_tmpC1F,te);Cyc_Tcexp_tcExp(
_tmpC20,0,_tmpC1E);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC1E,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC1E)){const char*_tmp14AA;void*
_tmp14A9;(_tmp14A9=0,Cyc_Tcutil_terr(loc,((_tmp14AA="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp14AA,sizeof(char),49))),_tag_dyneither(_tmp14A9,sizeof(void*),
0)));}{unsigned int _tmpC24;int _tmpC25;struct _tuple13 _tmpC23=Cyc_Evexp_eval_const_uint_exp(
_tmpC1E);_tmpC24=_tmpC23.f1;_tmpC25=_tmpC23.f2;if(is_zero_terminated  && (!
_tmpC25  || _tmpC24 < 1)){const char*_tmp14AD;void*_tmp14AC;(_tmp14AC=0,Cyc_Tcutil_terr(
loc,((_tmp14AD="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp14AD,sizeof(char),55))),_tag_dyneither(_tmp14AC,sizeof(void*),
0)));}goto _LL60F;};}_LL60F:;}goto _LL588;};}_LL59B: {struct Cyc_Absyn_TagType_struct*
_tmpB47=(struct Cyc_Absyn_TagType_struct*)_tmpB1D;if(_tmpB47->tag != 20)goto _LL59D;
else{_tmpB48=(void*)_tmpB47->f1;}}_LL59C: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB48,1);goto _LL588;_LL59D: {struct Cyc_Absyn_ValueofType_struct*
_tmpB49=(struct Cyc_Absyn_ValueofType_struct*)_tmpB1D;if(_tmpB49->tag != 19)goto
_LL59F;else{_tmpB4A=_tmpB49->f1;}}_LL59E: {struct _RegionHandle*_tmpC28=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC29=Cyc_Tcenv_allow_valueof(_tmpC28,te);Cyc_Tcexp_tcExp(
_tmpC29,0,_tmpB4A);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB4A)){const char*
_tmp14B0;void*_tmp14AF;(_tmp14AF=0,Cyc_Tcutil_terr(loc,((_tmp14B0="valueof_t requires an int expression",
_tag_dyneither(_tmp14B0,sizeof(char),37))),_tag_dyneither(_tmp14AF,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB4A,te,cvtenv);goto
_LL588;}_LL59F: {struct Cyc_Absyn_IntType_struct*_tmpB4B=(struct Cyc_Absyn_IntType_struct*)
_tmpB1D;if(_tmpB4B->tag != 6)goto _LL5A1;}_LL5A0: goto _LL5A2;_LL5A1: {struct Cyc_Absyn_FloatType_struct*
_tmpB4C=(struct Cyc_Absyn_FloatType_struct*)_tmpB1D;if(_tmpB4C->tag != 7)goto
_LL5A3;}_LL5A2: goto _LL5A4;_LL5A3: {struct Cyc_Absyn_DoubleType_struct*_tmpB4D=(
struct Cyc_Absyn_DoubleType_struct*)_tmpB1D;if(_tmpB4D->tag != 8)goto _LL5A5;}
_LL5A4: goto _LL588;_LL5A5: {struct Cyc_Absyn_ArrayType_struct*_tmpB4E=(struct Cyc_Absyn_ArrayType_struct*)
_tmpB1D;if(_tmpB4E->tag != 9)goto _LL5A7;else{_tmpB4F=_tmpB4E->f1;_tmpB50=_tmpB4F.elt_type;
_tmpB51=_tmpB4F.tq;_tmpB52=(struct Cyc_Absyn_Tqual*)&(_tmpB4E->f1).tq;_tmpB53=
_tmpB4F.num_elts;_tmpB54=(struct Cyc_Absyn_Exp**)&(_tmpB4E->f1).num_elts;_tmpB55=
_tmpB4F.zero_term;_tmpB56=_tmpB4F.zt_loc;}}_LL5A6: {struct Cyc_Absyn_Exp*_tmpC2C=*
_tmpB54;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,
_tmpB50,1);_tmpB52->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB52->print_const,
_tmpB50);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC2D=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB55);union Cyc_Absyn_Constraint
_tmpC2E;int _tmpC2F;union Cyc_Absyn_Constraint _tmpC30;int _tmpC31;_LL615: _tmpC2E=*
_tmpC2D;if((_tmpC2E.No_constr).tag != 3)goto _LL617;_tmpC2F=(int)(_tmpC2E.No_constr).val;
_LL616:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB55,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL614;_LL617: _tmpC30=*_tmpC2D;if((_tmpC30.Eq_constr).tag
!= 1)goto _LL619;_tmpC31=(int)(_tmpC30.Eq_constr).val;if(_tmpC31 != 1)goto _LL619;
_LL618: if(!Cyc_Tcutil_admits_zero(_tmpB50)){const char*_tmp14B4;void*_tmp14B3[1];
struct Cyc_String_pa_struct _tmp14B2;(_tmp14B2.tag=0,((_tmp14B2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB50)),((
_tmp14B3[0]=& _tmp14B2,Cyc_Tcutil_terr(loc,((_tmp14B4="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp14B4,sizeof(char),51))),_tag_dyneither(_tmp14B3,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL614;_LL619:;_LL61A: is_zero_terminated=0;
goto _LL614;_LL614:;}if(_tmpC2C == 0){if(is_zero_terminated)*_tmpB54=(_tmpC2C=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14B7;void*
_tmp14B6;(_tmp14B6=0,Cyc_Tcutil_warn(loc,((_tmp14B7="array bound defaults to 1 here",
_tag_dyneither(_tmp14B7,sizeof(char),31))),_tag_dyneither(_tmp14B6,sizeof(void*),
0)));}*_tmpB54=(_tmpC2C=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC2C);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC2C)){const char*_tmp14BA;void*_tmp14B9;(_tmp14B9=0,Cyc_Tcutil_terr(loc,((
_tmp14BA="array bounds expression is not constant",_tag_dyneither(_tmp14BA,
sizeof(char),40))),_tag_dyneither(_tmp14B9,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC2C)){const char*_tmp14BD;void*_tmp14BC;(_tmp14BC=0,
Cyc_Tcutil_terr(loc,((_tmp14BD="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14BD,sizeof(char),47))),_tag_dyneither(_tmp14BC,sizeof(void*),
0)));}{unsigned int _tmpC3C;int _tmpC3D;struct _tuple13 _tmpC3B=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC2C);_tmpC3C=_tmpC3B.f1;_tmpC3D=_tmpC3B.f2;if((
is_zero_terminated  && _tmpC3D) && _tmpC3C < 1){const char*_tmp14C0;void*_tmp14BF;(
_tmp14BF=0,Cyc_Tcutil_warn(loc,((_tmp14C0="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14C0,sizeof(char),48))),_tag_dyneither(_tmp14BF,sizeof(void*),
0)));}if((_tmpC3D  && _tmpC3C < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14C3;void*
_tmp14C2;(_tmp14C2=0,Cyc_Tcutil_warn(loc,((_tmp14C3="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14C3,sizeof(char),75))),_tag_dyneither(_tmp14C2,sizeof(void*),
0)));}*_tmpB54=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL588;};};}
_LL5A7: {struct Cyc_Absyn_FnType_struct*_tmpB57=(struct Cyc_Absyn_FnType_struct*)
_tmpB1D;if(_tmpB57->tag != 10)goto _LL5A9;else{_tmpB58=_tmpB57->f1;_tmpB59=_tmpB58.tvars;
_tmpB5A=(struct Cyc_List_List**)&(_tmpB57->f1).tvars;_tmpB5B=_tmpB58.effect;
_tmpB5C=(struct Cyc_Core_Opt**)&(_tmpB57->f1).effect;_tmpB5D=_tmpB58.ret_tqual;
_tmpB5E=(struct Cyc_Absyn_Tqual*)&(_tmpB57->f1).ret_tqual;_tmpB5F=_tmpB58.ret_typ;
_tmpB60=_tmpB58.args;_tmpB61=_tmpB58.c_varargs;_tmpB62=_tmpB58.cyc_varargs;
_tmpB63=_tmpB58.rgn_po;_tmpB64=_tmpB58.attributes;}}_LL5A8: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB64 != 0;
_tmpB64=_tmpB64->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB64->hd)){const char*
_tmp14C7;void*_tmp14C6[1];struct Cyc_String_pa_struct _tmp14C5;(_tmp14C5.tag=0,((
_tmp14C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB64->hd)),((_tmp14C6[0]=& _tmp14C5,Cyc_Tcutil_terr(loc,((_tmp14C7="bad function type attribute %s",
_tag_dyneither(_tmp14C7,sizeof(char),31))),_tag_dyneither(_tmp14C6,sizeof(void*),
1)))))));}{void*_tmpC45=(void*)_tmpB64->hd;enum Cyc_Absyn_Format_Type _tmpC4A;int
_tmpC4B;int _tmpC4C;_LL61C: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC46=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC45;if(_tmpC46->tag != 1)goto _LL61E;}_LL61D: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC47=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC45;if(_tmpC47->tag != 2)goto
_LL620;}_LL61F: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC48=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC45;if(_tmpC48->tag != 3)goto _LL622;}_LL621: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_struct*_tmpC49=(
struct Cyc_Absyn_Format_att_struct*)_tmpC45;if(_tmpC49->tag != 19)goto _LL624;else{
_tmpC4A=_tmpC49->f1;_tmpC4B=_tmpC49->f2;_tmpC4C=_tmpC49->f3;}}_LL623: if(!
seen_format){seen_format=1;ft=_tmpC4A;fmt_desc_arg=_tmpC4B;fmt_arg_start=_tmpC4C;}
else{const char*_tmp14CA;void*_tmp14C9;(_tmp14C9=0,Cyc_Tcutil_terr(loc,((_tmp14CA="function can't have multiple format attributes",
_tag_dyneither(_tmp14CA,sizeof(char),47))),_tag_dyneither(_tmp14C9,sizeof(void*),
0)));}goto _LL61B;_LL624:;_LL625: goto _LL61B;_LL61B:;};}if(num_convs > 1){const char*
_tmp14CD;void*_tmp14CC;(_tmp14CC=0,Cyc_Tcutil_terr(loc,((_tmp14CD="function can't have multiple calling conventions",
_tag_dyneither(_tmp14CD,sizeof(char),49))),_tag_dyneither(_tmp14CC,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB5A);{struct Cyc_List_List*b=*
_tmpB5A;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC51=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC53;struct Cyc_Absyn_Kind _tmpC54;enum Cyc_Absyn_KindQual
_tmpC55;_LL627: {struct Cyc_Absyn_Eq_kb_struct*_tmpC52=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC51;if(_tmpC52->tag != 0)goto _LL629;else{_tmpC53=_tmpC52->f1;_tmpC54=*_tmpC53;
_tmpC55=_tmpC54.kind;if(_tmpC55 != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:{const
char*_tmp14D1;void*_tmp14D0[1];struct Cyc_String_pa_struct _tmp14CF;(_tmp14CF.tag=
0,((_tmp14CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14D0[0]=& _tmp14CF,Cyc_Tcutil_terr(loc,((_tmp14D1="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14D1,sizeof(char),51))),_tag_dyneither(_tmp14D0,sizeof(void*),
1)))))));}goto _LL626;_LL629:;_LL62A: goto _LL626;_LL626:;};}}{struct _RegionHandle*
_tmpC59=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14D2;struct Cyc_Tcutil_CVTEnv
_tmpC5A=(_tmp14D2.r=_tmpC59,((_tmp14D2.kind_env=cvtenv.kind_env,((_tmp14D2.free_vars=
0,((_tmp14D2.free_evars=0,((_tmp14D2.generalize_evars=cvtenv.generalize_evars,((
_tmp14D2.fn_result=1,_tmp14D2)))))))))));_tmpC5A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5A,& Cyc_Tcutil_tmk,_tmpB5F,1);_tmpB5E->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB5E->print_const,_tmpB5F);_tmpC5A.fn_result=0;{struct Cyc_List_List*a=
_tmpB60;for(0;a != 0;a=a->tl){struct _tuple9*_tmpC5B=(struct _tuple9*)a->hd;void*
_tmpC5C=(*_tmpC5B).f3;_tmpC5A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC5A,& Cyc_Tcutil_tmk,
_tmpC5C,1);{int _tmpC5D=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC5B).f2).print_const,
_tmpC5C);((*_tmpC5B).f2).real_const=_tmpC5D;{void*_tmpC5E=Cyc_Tcutil_compress(
_tmpC5C);struct Cyc_Absyn_ArrayInfo _tmpC60;void*_tmpC61;struct Cyc_Absyn_Tqual
_tmpC62;struct Cyc_Absyn_Exp*_tmpC63;union Cyc_Absyn_Constraint*_tmpC64;struct Cyc_Position_Segment*
_tmpC65;_LL62C: {struct Cyc_Absyn_ArrayType_struct*_tmpC5F=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC5E;if(_tmpC5F->tag != 9)goto _LL62E;else{_tmpC60=_tmpC5F->f1;_tmpC61=_tmpC60.elt_type;
_tmpC62=_tmpC60.tq;_tmpC63=_tmpC60.num_elts;_tmpC64=_tmpC60.zero_term;_tmpC65=
_tmpC60.zt_loc;}}_LL62D: {void*_tmpC66=Cyc_Absyn_new_evar(0,0);_tmpC5A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5A,& Cyc_Tcutil_rk,_tmpC66,1);{struct Cyc_Absyn_Upper_b_struct _tmp14D5;
struct Cyc_Absyn_Upper_b_struct*_tmp14D4;void*_tmpC67=Cyc_Absyn_atb_typ(_tmpC61,
_tmpC66,_tmpC62,(void*)((_tmp14D4=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp14D4)),((_tmp14D4[0]=((_tmp14D5.tag=1,((_tmp14D5.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmpC63),_tmp14D5)))),_tmp14D4)))),_tmpC64);(*_tmpC5B).f3=_tmpC67;
goto _LL62B;};}_LL62E:;_LL62F: goto _LL62B;_LL62B:;};};}}if(_tmpB62 != 0){if(_tmpB61){
const char*_tmp14D8;void*_tmp14D7;(_tmp14D7=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp14D8="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp14D8,sizeof(char),29))),_tag_dyneither(_tmp14D7,sizeof(void*),
0)));}{void*_tmpC6D;int _tmpC6E;struct Cyc_Absyn_VarargInfo _tmpC6C=*_tmpB62;
_tmpC6D=_tmpC6C.type;_tmpC6E=_tmpC6C.inject;_tmpC5A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5A,& Cyc_Tcutil_tmk,_tmpC6D,1);(_tmpB62->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmpB62->tq).print_const,_tmpC6D);if(_tmpC6E){void*_tmpC6F=Cyc_Tcutil_compress(
_tmpC6D);struct Cyc_Absyn_PtrInfo _tmpC71;void*_tmpC72;struct Cyc_Absyn_PtrAtts
_tmpC73;union Cyc_Absyn_Constraint*_tmpC74;union Cyc_Absyn_Constraint*_tmpC75;
_LL631: {struct Cyc_Absyn_PointerType_struct*_tmpC70=(struct Cyc_Absyn_PointerType_struct*)
_tmpC6F;if(_tmpC70->tag != 5)goto _LL633;else{_tmpC71=_tmpC70->f1;_tmpC72=_tmpC71.elt_typ;
_tmpC73=_tmpC71.ptr_atts;_tmpC74=_tmpC73.bounds;_tmpC75=_tmpC73.zero_term;}}
_LL632:{void*_tmpC76=Cyc_Tcutil_compress(_tmpC72);_LL636: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC77=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC76;if(_tmpC77->tag != 3)goto
_LL638;}_LL637: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC75)){const char*_tmp14DB;void*_tmp14DA;(_tmp14DA=0,Cyc_Tcutil_terr(loc,((
_tmp14DB="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14DB,sizeof(
char),37))),_tag_dyneither(_tmp14DA,sizeof(void*),0)));}{void*_tmpC7A=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC74);_LL63B: {struct Cyc_Absyn_DynEither_b_struct*_tmpC7B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC7A;if(_tmpC7B->tag != 0)goto _LL63D;}_LL63C:{const char*_tmp14DE;void*_tmp14DD;(
_tmp14DD=0,Cyc_Tcutil_terr(loc,((_tmp14DE="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14DE,sizeof(char),44))),_tag_dyneither(_tmp14DD,sizeof(void*),
0)));}goto _LL63A;_LL63D:;_LL63E: goto _LL63A;_LL63A:;}goto _LL635;_LL638:;_LL639:{
const char*_tmp14E1;void*_tmp14E0;(_tmp14E0=0,Cyc_Tcutil_terr(loc,((_tmp14E1="can't inject a non-datatype type",
_tag_dyneither(_tmp14E1,sizeof(char),33))),_tag_dyneither(_tmp14E0,sizeof(void*),
0)));}goto _LL635;_LL635:;}goto _LL630;_LL633:;_LL634:{const char*_tmp14E4;void*
_tmp14E3;(_tmp14E3=0,Cyc_Tcutil_terr(loc,((_tmp14E4="expecting a datatype pointer type",
_tag_dyneither(_tmp14E4,sizeof(char),34))),_tag_dyneither(_tmp14E3,sizeof(void*),
0)));}goto _LL630;_LL630:;}};}if(seen_format){int _tmpC82=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB60);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC82) || 
fmt_arg_start < 0) || _tmpB62 == 0  && fmt_arg_start != 0) || _tmpB62 != 0  && 
fmt_arg_start != _tmpC82 + 1){const char*_tmp14E7;void*_tmp14E6;(_tmp14E6=0,Cyc_Tcutil_terr(
loc,((_tmp14E7="bad format descriptor",_tag_dyneither(_tmp14E7,sizeof(char),22))),
_tag_dyneither(_tmp14E6,sizeof(void*),0)));}else{void*_tmpC86;struct _tuple9
_tmpC85=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB60,
fmt_desc_arg - 1);_tmpC86=_tmpC85.f3;{void*_tmpC87=Cyc_Tcutil_compress(_tmpC86);
struct Cyc_Absyn_PtrInfo _tmpC89;void*_tmpC8A;struct Cyc_Absyn_PtrAtts _tmpC8B;union
Cyc_Absyn_Constraint*_tmpC8C;union Cyc_Absyn_Constraint*_tmpC8D;_LL640: {struct
Cyc_Absyn_PointerType_struct*_tmpC88=(struct Cyc_Absyn_PointerType_struct*)
_tmpC87;if(_tmpC88->tag != 5)goto _LL642;else{_tmpC89=_tmpC88->f1;_tmpC8A=_tmpC89.elt_typ;
_tmpC8B=_tmpC89.ptr_atts;_tmpC8C=_tmpC8B.bounds;_tmpC8D=_tmpC8B.zero_term;}}
_LL641:{struct _tuple0 _tmp14E8;struct _tuple0 _tmpC8F=(_tmp14E8.f1=Cyc_Tcutil_compress(
_tmpC8A),((_tmp14E8.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC8C),_tmp14E8)));void*_tmpC90;enum Cyc_Absyn_Sign
_tmpC92;enum Cyc_Absyn_Size_of _tmpC93;void*_tmpC94;_LL645: _tmpC90=_tmpC8F.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC91=(struct Cyc_Absyn_IntType_struct*)_tmpC90;
if(_tmpC91->tag != 6)goto _LL647;else{_tmpC92=_tmpC91->f1;if(_tmpC92 != Cyc_Absyn_None)
goto _LL647;_tmpC93=_tmpC91->f2;if(_tmpC93 != Cyc_Absyn_Char_sz)goto _LL647;}};
_tmpC94=_tmpC8F.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC95=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC94;if(_tmpC95->tag != 0)goto _LL647;};_LL646: goto _LL644;_LL647:;_LL648:{const
char*_tmp14EB;void*_tmp14EA;(_tmp14EA=0,Cyc_Tcutil_terr(loc,((_tmp14EB="format descriptor is not a char ? type",
_tag_dyneither(_tmp14EB,sizeof(char),39))),_tag_dyneither(_tmp14EA,sizeof(void*),
0)));}goto _LL644;_LL644:;}goto _LL63F;_LL642:;_LL643:{const char*_tmp14EE;void*
_tmp14ED;(_tmp14ED=0,Cyc_Tcutil_terr(loc,((_tmp14EE="format descriptor is not a char ? type",
_tag_dyneither(_tmp14EE,sizeof(char),39))),_tag_dyneither(_tmp14ED,sizeof(void*),
0)));}goto _LL63F;_LL63F:;}if(fmt_arg_start != 0){int problem;{struct _tuple31
_tmp14EF;struct _tuple31 _tmpC9B=(_tmp14EF.f1=ft,((_tmp14EF.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB62))->type)),
_tmp14EF)));enum Cyc_Absyn_Format_Type _tmpC9C;void*_tmpC9D;struct Cyc_Absyn_DatatypeInfo
_tmpC9F;union Cyc_Absyn_DatatypeInfoU _tmpCA0;struct Cyc_Absyn_Datatypedecl**
_tmpCA1;struct Cyc_Absyn_Datatypedecl*_tmpCA2;enum Cyc_Absyn_Format_Type _tmpCA3;
void*_tmpCA4;struct Cyc_Absyn_DatatypeInfo _tmpCA6;union Cyc_Absyn_DatatypeInfoU
_tmpCA7;struct Cyc_Absyn_Datatypedecl**_tmpCA8;struct Cyc_Absyn_Datatypedecl*
_tmpCA9;_LL64A: _tmpC9C=_tmpC9B.f1;if(_tmpC9C != Cyc_Absyn_Printf_ft)goto _LL64C;
_tmpC9D=_tmpC9B.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC9E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC9D;if(_tmpC9E->tag != 3)goto _LL64C;else{_tmpC9F=_tmpC9E->f1;_tmpCA0=_tmpC9F.datatype_info;
if((_tmpCA0.KnownDatatype).tag != 2)goto _LL64C;_tmpCA1=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCA0.KnownDatatype).val;_tmpCA2=*_tmpCA1;}};_LL64B: problem=Cyc_Absyn_qvar_cmp(
_tmpCA2->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL649;_LL64C: _tmpCA3=
_tmpC9B.f1;if(_tmpCA3 != Cyc_Absyn_Scanf_ft)goto _LL64E;_tmpCA4=_tmpC9B.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpCA5=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCA4;if(_tmpCA5->tag != 3)goto _LL64E;else{_tmpCA6=_tmpCA5->f1;_tmpCA7=_tmpCA6.datatype_info;
if((_tmpCA7.KnownDatatype).tag != 2)goto _LL64E;_tmpCA8=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCA7.KnownDatatype).val;_tmpCA9=*_tmpCA8;}};_LL64D: problem=Cyc_Absyn_qvar_cmp(
_tmpCA9->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL649;_LL64E:;_LL64F:
problem=1;goto _LL649;_LL649:;}if(problem){const char*_tmp14F2;void*_tmp14F1;(
_tmp14F1=0,Cyc_Tcutil_terr(loc,((_tmp14F2="format attribute and vararg types don't match",
_tag_dyneither(_tmp14F2,sizeof(char),46))),_tag_dyneither(_tmp14F1,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB63;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpCAD;void*_tmpCAE;void*_tmpCAF;struct _tuple0*_tmpCAC=(struct _tuple0*)
rpo->hd;_tmpCAD=*_tmpCAC;_tmpCAE=_tmpCAD.f1;_tmpCAF=_tmpCAD.f2;_tmpC5A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5A,& Cyc_Tcutil_ek,_tmpCAE,1);_tmpC5A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5A,& Cyc_Tcutil_trk,_tmpCAF,1);}}if(*_tmpB5C != 0)_tmpC5A=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5A,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB5C))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC5A.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpCB1;int _tmpCB2;struct
_tuple29 _tmpCB0=*((struct _tuple29*)tvs->hd);_tmpCB1=_tmpCB0.f1;_tmpCB2=_tmpCB0.f2;
if(!_tmpCB2)continue;{void*_tmpCB3=Cyc_Absyn_compress_kb(_tmpCB1->kind);struct
Cyc_Core_Opt*_tmpCB5;struct Cyc_Core_Opt**_tmpCB6;struct Cyc_Absyn_Kind*_tmpCB7;
struct Cyc_Absyn_Kind*_tmpCB9;struct Cyc_Absyn_Kind*_tmpCBB;struct Cyc_Absyn_Kind
_tmpCBC;enum Cyc_Absyn_KindQual _tmpCBD;struct Cyc_Absyn_Kind*_tmpCBF;struct Cyc_Absyn_Kind
_tmpCC0;enum Cyc_Absyn_KindQual _tmpCC1;struct Cyc_Core_Opt*_tmpCC3;struct Cyc_Core_Opt**
_tmpCC4;struct Cyc_Absyn_Kind*_tmpCC5;struct Cyc_Absyn_Kind _tmpCC6;enum Cyc_Absyn_KindQual
_tmpCC7;struct Cyc_Absyn_Kind*_tmpCC9;struct Cyc_Absyn_Kind _tmpCCA;enum Cyc_Absyn_KindQual
_tmpCCB;struct Cyc_Core_Opt*_tmpCCD;struct Cyc_Core_Opt**_tmpCCE;_LL651:{struct Cyc_Absyn_Less_kb_struct*
_tmpCB4=(struct Cyc_Absyn_Less_kb_struct*)_tmpCB3;if(_tmpCB4->tag != 2)goto _LL653;
else{_tmpCB5=_tmpCB4->f1;_tmpCB6=(struct Cyc_Core_Opt**)& _tmpCB4->f1;_tmpCB7=
_tmpCB4->f2;}}if(!(_tmpCB7->kind == Cyc_Absyn_RgnKind))goto _LL653;_LL652: if(
_tmpCB7->aliasqual == Cyc_Absyn_Top){*_tmpCB6=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCB9=_tmpCB7;goto _LL654;}*_tmpCB6=Cyc_Tcutil_kind_to_bound_opt(_tmpCB7);
_tmpCB9=_tmpCB7;goto _LL654;_LL653:{struct Cyc_Absyn_Eq_kb_struct*_tmpCB8=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCB3;if(_tmpCB8->tag != 0)goto _LL655;else{_tmpCB9=
_tmpCB8->f1;}}if(!(_tmpCB9->kind == Cyc_Absyn_RgnKind))goto _LL655;_LL654:{struct
Cyc_Absyn_AccessEff_struct*_tmp1501;struct Cyc_Absyn_VarType_struct*_tmp1500;
struct Cyc_Absyn_VarType_struct _tmp14FF;struct Cyc_Absyn_AccessEff_struct _tmp14FE;
struct Cyc_List_List*_tmp14FD;effect=((_tmp14FD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp14FD)),((_tmp14FD->hd=(void*)((_tmp1501=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1501)),((_tmp1501[0]=((_tmp14FE.tag=24,((_tmp14FE.f1=(void*)((void*)((
_tmp1500=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1500)),((_tmp1500[0]=((
_tmp14FF.tag=2,((_tmp14FF.f1=_tmpCB1,_tmp14FF)))),_tmp1500))))),_tmp14FE)))),
_tmp1501)))),((_tmp14FD->tl=effect,_tmp14FD))))));}goto _LL650;_LL655: {struct Cyc_Absyn_Less_kb_struct*
_tmpCBA=(struct Cyc_Absyn_Less_kb_struct*)_tmpCB3;if(_tmpCBA->tag != 2)goto _LL657;
else{_tmpCBB=_tmpCBA->f2;_tmpCBC=*_tmpCBB;_tmpCBD=_tmpCBC.kind;if(_tmpCBD != Cyc_Absyn_IntKind)
goto _LL657;}}_LL656: goto _LL658;_LL657: {struct Cyc_Absyn_Eq_kb_struct*_tmpCBE=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpCB3;if(_tmpCBE->tag != 0)goto _LL659;else{
_tmpCBF=_tmpCBE->f1;_tmpCC0=*_tmpCBF;_tmpCC1=_tmpCC0.kind;if(_tmpCC1 != Cyc_Absyn_IntKind)
goto _LL659;}}_LL658: goto _LL650;_LL659: {struct Cyc_Absyn_Less_kb_struct*_tmpCC2=(
struct Cyc_Absyn_Less_kb_struct*)_tmpCB3;if(_tmpCC2->tag != 2)goto _LL65B;else{
_tmpCC3=_tmpCC2->f1;_tmpCC4=(struct Cyc_Core_Opt**)& _tmpCC2->f1;_tmpCC5=_tmpCC2->f2;
_tmpCC6=*_tmpCC5;_tmpCC7=_tmpCC6.kind;if(_tmpCC7 != Cyc_Absyn_EffKind)goto _LL65B;}}
_LL65A:*_tmpCC4=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL65C;_LL65B: {
struct Cyc_Absyn_Eq_kb_struct*_tmpCC8=(struct Cyc_Absyn_Eq_kb_struct*)_tmpCB3;if(
_tmpCC8->tag != 0)goto _LL65D;else{_tmpCC9=_tmpCC8->f1;_tmpCCA=*_tmpCC9;_tmpCCB=
_tmpCCA.kind;if(_tmpCCB != Cyc_Absyn_EffKind)goto _LL65D;}}_LL65C:{struct Cyc_Absyn_VarType_struct*
_tmp1507;struct Cyc_Absyn_VarType_struct _tmp1506;struct Cyc_List_List*_tmp1505;
effect=((_tmp1505=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1505)),((
_tmp1505->hd=(void*)((_tmp1507=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1507)),((_tmp1507[0]=((_tmp1506.tag=2,((_tmp1506.f1=_tmpCB1,_tmp1506)))),
_tmp1507)))),((_tmp1505->tl=effect,_tmp1505))))));}goto _LL650;_LL65D: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCCC=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpCB3;if(_tmpCCC->tag != 1)goto
_LL65F;else{_tmpCCD=_tmpCCC->f1;_tmpCCE=(struct Cyc_Core_Opt**)& _tmpCCC->f1;}}
_LL65E:{struct Cyc_Absyn_Less_kb_struct*_tmp150D;struct Cyc_Absyn_Less_kb_struct
_tmp150C;struct Cyc_Core_Opt*_tmp150B;*_tmpCCE=((_tmp150B=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp150B)),((_tmp150B->v=(void*)((_tmp150D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp150D)),((_tmp150D[0]=((_tmp150C.tag=2,((_tmp150C.f1=0,((_tmp150C.f2=&
Cyc_Tcutil_ak,_tmp150C)))))),_tmp150D)))),_tmp150B))));}goto _LL660;_LL65F:;
_LL660:{struct Cyc_Absyn_RgnsEff_struct*_tmp151C;struct Cyc_Absyn_VarType_struct*
_tmp151B;struct Cyc_Absyn_VarType_struct _tmp151A;struct Cyc_Absyn_RgnsEff_struct
_tmp1519;struct Cyc_List_List*_tmp1518;effect=((_tmp1518=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1518)),((_tmp1518->hd=(void*)((_tmp151C=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp151C)),((_tmp151C[0]=((_tmp1519.tag=26,((_tmp1519.f1=(void*)((void*)((
_tmp151B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp151B)),((_tmp151B[0]=((
_tmp151A.tag=2,((_tmp151A.f1=_tmpCB1,_tmp151A)))),_tmp151B))))),_tmp1519)))),
_tmp151C)))),((_tmp1518->tl=effect,_tmp1518))))));}goto _LL650;_LL650:;};}}{
struct Cyc_List_List*ts=_tmpC5A.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCE0;
int _tmpCE1;struct _tuple30 _tmpCDF=*((struct _tuple30*)ts->hd);_tmpCE0=_tmpCDF.f1;
_tmpCE1=_tmpCDF.f2;if(!_tmpCE1)continue;{struct Cyc_Absyn_Kind*_tmpCE2=Cyc_Tcutil_typ_kind(
_tmpCE0);struct Cyc_Absyn_Kind _tmpCE3;enum Cyc_Absyn_KindQual _tmpCE4;struct Cyc_Absyn_Kind
_tmpCE5;enum Cyc_Absyn_KindQual _tmpCE6;struct Cyc_Absyn_Kind _tmpCE7;enum Cyc_Absyn_KindQual
_tmpCE8;_LL662: _tmpCE3=*_tmpCE2;_tmpCE4=_tmpCE3.kind;if(_tmpCE4 != Cyc_Absyn_RgnKind)
goto _LL664;_LL663:{struct Cyc_Absyn_AccessEff_struct*_tmp1522;struct Cyc_Absyn_AccessEff_struct
_tmp1521;struct Cyc_List_List*_tmp1520;effect=((_tmp1520=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1520)),((_tmp1520->hd=(void*)((_tmp1522=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1522)),((_tmp1522[0]=((_tmp1521.tag=24,((_tmp1521.f1=(void*)_tmpCE0,
_tmp1521)))),_tmp1522)))),((_tmp1520->tl=effect,_tmp1520))))));}goto _LL661;
_LL664: _tmpCE5=*_tmpCE2;_tmpCE6=_tmpCE5.kind;if(_tmpCE6 != Cyc_Absyn_EffKind)goto
_LL666;_LL665:{struct Cyc_List_List*_tmp1523;effect=((_tmp1523=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1523)),((_tmp1523->hd=_tmpCE0,((_tmp1523->tl=effect,_tmp1523))))));}
goto _LL661;_LL666: _tmpCE7=*_tmpCE2;_tmpCE8=_tmpCE7.kind;if(_tmpCE8 != Cyc_Absyn_IntKind)
goto _LL668;_LL667: goto _LL661;_LL668:;_LL669:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1529;struct Cyc_Absyn_RgnsEff_struct _tmp1528;struct Cyc_List_List*_tmp1527;
effect=((_tmp1527=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1527)),((
_tmp1527->hd=(void*)((_tmp1529=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1529)),((_tmp1529[0]=((_tmp1528.tag=26,((_tmp1528.f1=(void*)_tmpCE0,_tmp1528)))),
_tmp1529)))),((_tmp1527->tl=effect,_tmp1527))))));}goto _LL661;_LL661:;};}}{
struct Cyc_Absyn_JoinEff_struct*_tmp152F;struct Cyc_Absyn_JoinEff_struct _tmp152E;
struct Cyc_Core_Opt*_tmp152D;*_tmpB5C=((_tmp152D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp152D)),((_tmp152D->v=(void*)((_tmp152F=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp152F)),((_tmp152F[0]=((_tmp152E.tag=25,((_tmp152E.f1=effect,_tmp152E)))),
_tmp152F)))),_tmp152D))));};}if(*_tmpB5A != 0){struct Cyc_List_List*bs=*_tmpB5A;
for(0;bs != 0;bs=bs->tl){void*_tmpCF3=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)
bs->hd)->kind);struct Cyc_Core_Opt*_tmpCF5;struct Cyc_Core_Opt**_tmpCF6;struct Cyc_Core_Opt*
_tmpCF8;struct Cyc_Core_Opt**_tmpCF9;struct Cyc_Absyn_Kind*_tmpCFA;struct Cyc_Absyn_Kind
_tmpCFB;enum Cyc_Absyn_KindQual _tmpCFC;enum Cyc_Absyn_AliasQual _tmpCFD;struct Cyc_Core_Opt*
_tmpCFF;struct Cyc_Core_Opt**_tmpD00;struct Cyc_Absyn_Kind*_tmpD01;struct Cyc_Absyn_Kind
_tmpD02;enum Cyc_Absyn_KindQual _tmpD03;enum Cyc_Absyn_AliasQual _tmpD04;struct Cyc_Core_Opt*
_tmpD06;struct Cyc_Core_Opt**_tmpD07;struct Cyc_Absyn_Kind*_tmpD08;struct Cyc_Absyn_Kind
_tmpD09;enum Cyc_Absyn_KindQual _tmpD0A;enum Cyc_Absyn_AliasQual _tmpD0B;struct Cyc_Core_Opt*
_tmpD0D;struct Cyc_Core_Opt**_tmpD0E;struct Cyc_Absyn_Kind*_tmpD0F;struct Cyc_Absyn_Kind
_tmpD10;enum Cyc_Absyn_KindQual _tmpD11;enum Cyc_Absyn_AliasQual _tmpD12;struct Cyc_Core_Opt*
_tmpD14;struct Cyc_Core_Opt**_tmpD15;struct Cyc_Absyn_Kind*_tmpD16;struct Cyc_Absyn_Kind
_tmpD17;enum Cyc_Absyn_KindQual _tmpD18;enum Cyc_Absyn_AliasQual _tmpD19;struct Cyc_Core_Opt*
_tmpD1B;struct Cyc_Core_Opt**_tmpD1C;struct Cyc_Absyn_Kind*_tmpD1D;struct Cyc_Absyn_Kind
_tmpD1E;enum Cyc_Absyn_KindQual _tmpD1F;enum Cyc_Absyn_AliasQual _tmpD20;struct Cyc_Core_Opt*
_tmpD22;struct Cyc_Core_Opt**_tmpD23;struct Cyc_Absyn_Kind*_tmpD24;struct Cyc_Absyn_Kind
_tmpD25;enum Cyc_Absyn_KindQual _tmpD26;enum Cyc_Absyn_AliasQual _tmpD27;struct Cyc_Core_Opt*
_tmpD29;struct Cyc_Core_Opt**_tmpD2A;struct Cyc_Absyn_Kind*_tmpD2B;struct Cyc_Absyn_Kind
_tmpD2C;enum Cyc_Absyn_KindQual _tmpD2D;enum Cyc_Absyn_AliasQual _tmpD2E;struct Cyc_Core_Opt*
_tmpD30;struct Cyc_Core_Opt**_tmpD31;struct Cyc_Absyn_Kind*_tmpD32;struct Cyc_Absyn_Kind*
_tmpD34;struct Cyc_Absyn_Kind _tmpD35;enum Cyc_Absyn_KindQual _tmpD36;_LL66B: {
struct Cyc_Absyn_Unknown_kb_struct*_tmpCF4=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCF3;if(_tmpCF4->tag != 1)goto _LL66D;else{_tmpCF5=_tmpCF4->f1;_tmpCF6=(struct
Cyc_Core_Opt**)& _tmpCF4->f1;}}_LL66C:{const char*_tmp1533;void*_tmp1532[1];struct
Cyc_String_pa_struct _tmp1531;(_tmp1531.tag=0,((_tmp1531.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1532[0]=&
_tmp1531,Cyc_Tcutil_warn(loc,((_tmp1533="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1533,sizeof(char),47))),_tag_dyneither(_tmp1532,sizeof(void*),
1)))))));}_tmpCF9=_tmpCF6;goto _LL66E;_LL66D: {struct Cyc_Absyn_Less_kb_struct*
_tmpCF7=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;if(_tmpCF7->tag != 2)goto _LL66F;
else{_tmpCF8=_tmpCF7->f1;_tmpCF9=(struct Cyc_Core_Opt**)& _tmpCF7->f1;_tmpCFA=
_tmpCF7->f2;_tmpCFB=*_tmpCFA;_tmpCFC=_tmpCFB.kind;if(_tmpCFC != Cyc_Absyn_BoxKind)
goto _LL66F;_tmpCFD=_tmpCFB.aliasqual;if(_tmpCFD != Cyc_Absyn_Top)goto _LL66F;}}
_LL66E: _tmpD00=_tmpCF9;goto _LL670;_LL66F: {struct Cyc_Absyn_Less_kb_struct*
_tmpCFE=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;if(_tmpCFE->tag != 2)goto _LL671;
else{_tmpCFF=_tmpCFE->f1;_tmpD00=(struct Cyc_Core_Opt**)& _tmpCFE->f1;_tmpD01=
_tmpCFE->f2;_tmpD02=*_tmpD01;_tmpD03=_tmpD02.kind;if(_tmpD03 != Cyc_Absyn_MemKind)
goto _LL671;_tmpD04=_tmpD02.aliasqual;if(_tmpD04 != Cyc_Absyn_Top)goto _LL671;}}
_LL670: _tmpD07=_tmpD00;goto _LL672;_LL671: {struct Cyc_Absyn_Less_kb_struct*
_tmpD05=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;if(_tmpD05->tag != 2)goto _LL673;
else{_tmpD06=_tmpD05->f1;_tmpD07=(struct Cyc_Core_Opt**)& _tmpD05->f1;_tmpD08=
_tmpD05->f2;_tmpD09=*_tmpD08;_tmpD0A=_tmpD09.kind;if(_tmpD0A != Cyc_Absyn_MemKind)
goto _LL673;_tmpD0B=_tmpD09.aliasqual;if(_tmpD0B != Cyc_Absyn_Aliasable)goto _LL673;}}
_LL672: _tmpD0E=_tmpD07;goto _LL674;_LL673: {struct Cyc_Absyn_Less_kb_struct*
_tmpD0C=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;if(_tmpD0C->tag != 2)goto _LL675;
else{_tmpD0D=_tmpD0C->f1;_tmpD0E=(struct Cyc_Core_Opt**)& _tmpD0C->f1;_tmpD0F=
_tmpD0C->f2;_tmpD10=*_tmpD0F;_tmpD11=_tmpD10.kind;if(_tmpD11 != Cyc_Absyn_AnyKind)
goto _LL675;_tmpD12=_tmpD10.aliasqual;if(_tmpD12 != Cyc_Absyn_Top)goto _LL675;}}
_LL674: _tmpD15=_tmpD0E;goto _LL676;_LL675: {struct Cyc_Absyn_Less_kb_struct*
_tmpD13=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;if(_tmpD13->tag != 2)goto _LL677;
else{_tmpD14=_tmpD13->f1;_tmpD15=(struct Cyc_Core_Opt**)& _tmpD13->f1;_tmpD16=
_tmpD13->f2;_tmpD17=*_tmpD16;_tmpD18=_tmpD17.kind;if(_tmpD18 != Cyc_Absyn_AnyKind)
goto _LL677;_tmpD19=_tmpD17.aliasqual;if(_tmpD19 != Cyc_Absyn_Aliasable)goto _LL677;}}
_LL676:*_tmpD15=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL66A;_LL677: {
struct Cyc_Absyn_Less_kb_struct*_tmpD1A=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;
if(_tmpD1A->tag != 2)goto _LL679;else{_tmpD1B=_tmpD1A->f1;_tmpD1C=(struct Cyc_Core_Opt**)&
_tmpD1A->f1;_tmpD1D=_tmpD1A->f2;_tmpD1E=*_tmpD1D;_tmpD1F=_tmpD1E.kind;if(_tmpD1F
!= Cyc_Absyn_MemKind)goto _LL679;_tmpD20=_tmpD1E.aliasqual;if(_tmpD20 != Cyc_Absyn_Unique)
goto _LL679;}}_LL678: _tmpD23=_tmpD1C;goto _LL67A;_LL679: {struct Cyc_Absyn_Less_kb_struct*
_tmpD21=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;if(_tmpD21->tag != 2)goto _LL67B;
else{_tmpD22=_tmpD21->f1;_tmpD23=(struct Cyc_Core_Opt**)& _tmpD21->f1;_tmpD24=
_tmpD21->f2;_tmpD25=*_tmpD24;_tmpD26=_tmpD25.kind;if(_tmpD26 != Cyc_Absyn_AnyKind)
goto _LL67B;_tmpD27=_tmpD25.aliasqual;if(_tmpD27 != Cyc_Absyn_Unique)goto _LL67B;}}
_LL67A:*_tmpD23=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL66A;_LL67B: {
struct Cyc_Absyn_Less_kb_struct*_tmpD28=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF3;
if(_tmpD28->tag != 2)goto _LL67D;else{_tmpD29=_tmpD28->f1;_tmpD2A=(struct Cyc_Core_Opt**)&
_tmpD28->f1;_tmpD2B=_tmpD28->f2;_tmpD2C=*_tmpD2B;_tmpD2D=_tmpD2C.kind;if(_tmpD2D
!= Cyc_Absyn_RgnKind)goto _LL67D;_tmpD2E=_tmpD2C.aliasqual;if(_tmpD2E != Cyc_Absyn_Top)
goto _LL67D;}}_LL67C:*_tmpD2A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL66A;_LL67D: {struct Cyc_Absyn_Less_kb_struct*_tmpD2F=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCF3;if(_tmpD2F->tag != 2)goto _LL67F;else{_tmpD30=_tmpD2F->f1;_tmpD31=(struct
Cyc_Core_Opt**)& _tmpD2F->f1;_tmpD32=_tmpD2F->f2;}}_LL67E:*_tmpD31=Cyc_Tcutil_kind_to_bound_opt(
_tmpD32);goto _LL66A;_LL67F: {struct Cyc_Absyn_Eq_kb_struct*_tmpD33=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCF3;if(_tmpD33->tag != 0)goto _LL681;else{_tmpD34=_tmpD33->f1;_tmpD35=*_tmpD34;
_tmpD36=_tmpD35.kind;if(_tmpD36 != Cyc_Absyn_MemKind)goto _LL681;}}_LL680:{const
char*_tmp1536;void*_tmp1535;(_tmp1535=0,Cyc_Tcutil_terr(loc,((_tmp1536="functions can't abstract M types",
_tag_dyneither(_tmp1536,sizeof(char),33))),_tag_dyneither(_tmp1535,sizeof(void*),
0)));}goto _LL66A;_LL681:;_LL682: goto _LL66A;_LL66A:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC5A.kind_env,*_tmpB5A);_tmpC5A.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC5A.r,_tmpC5A.free_vars,*_tmpB5A);{struct Cyc_List_List*tvs=_tmpC5A.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD3D;int _tmpD3E;struct
_tuple29 _tmpD3C=*((struct _tuple29*)tvs->hd);_tmpD3D=_tmpD3C.f1;_tmpD3E=_tmpD3C.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD3D,_tmpD3E);}}{struct Cyc_List_List*evs=_tmpC5A.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD40;int _tmpD41;struct _tuple30 _tmpD3F=*((struct _tuple30*)evs->hd);
_tmpD40=_tmpD3F.f1;_tmpD41=_tmpD3F.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD40,_tmpD41);}};}goto _LL588;};}_LL5A9: {struct Cyc_Absyn_TupleType_struct*
_tmpB65=(struct Cyc_Absyn_TupleType_struct*)_tmpB1D;if(_tmpB65->tag != 11)goto
_LL5AB;else{_tmpB66=_tmpB65->f1;}}_LL5AA: for(0;_tmpB66 != 0;_tmpB66=_tmpB66->tl){
struct _tuple11*_tmpD43=(struct _tuple11*)_tmpB66->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD43).f2,1);((*_tmpD43).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD43).f1).print_const,(*_tmpD43).f2);}goto _LL588;_LL5AB: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB67=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB1D;if(_tmpB67->tag != 13)goto
_LL5AD;else{_tmpB68=_tmpB67->f1;_tmpB69=_tmpB67->f2;}}_LL5AC: {struct
_RegionHandle*_tmpD44=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB69 != 0;_tmpB69=_tmpB69->tl){struct Cyc_Absyn_Aggrfield _tmpD46;struct
_dyneither_ptr*_tmpD47;struct Cyc_Absyn_Tqual _tmpD48;struct Cyc_Absyn_Tqual*
_tmpD49;void*_tmpD4A;struct Cyc_Absyn_Exp*_tmpD4B;struct Cyc_List_List*_tmpD4C;
struct Cyc_Absyn_Aggrfield*_tmpD45=(struct Cyc_Absyn_Aggrfield*)_tmpB69->hd;
_tmpD46=*_tmpD45;_tmpD47=_tmpD46.name;_tmpD48=_tmpD46.tq;_tmpD49=(struct Cyc_Absyn_Tqual*)&(*
_tmpD45).tq;_tmpD4A=_tmpD46.type;_tmpD4B=_tmpD46.width;_tmpD4C=_tmpD46.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD47)){
const char*_tmp153A;void*_tmp1539[1];struct Cyc_String_pa_struct _tmp1538;(_tmp1538.tag=
0,((_tmp1538.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD47),((
_tmp1539[0]=& _tmp1538,Cyc_Tcutil_terr(loc,((_tmp153A="duplicate field %s",
_tag_dyneither(_tmp153A,sizeof(char),19))),_tag_dyneither(_tmp1539,sizeof(void*),
1)))))));}{const char*_tmp153B;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD47,((
_tmp153B="",_tag_dyneither(_tmp153B,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp153C;prev_fields=((_tmp153C=_region_malloc(_tmpD44,sizeof(*_tmp153C)),((
_tmp153C->hd=_tmpD47,((_tmp153C->tl=prev_fields,_tmp153C))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD4A,1);_tmpD49->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD49->print_const,_tmpD4A);if(_tmpB68 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD4A)){const char*_tmp1540;void*_tmp153F[1];struct Cyc_String_pa_struct _tmp153E;(
_tmp153E.tag=0,((_tmp153E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD47),((_tmp153F[0]=& _tmp153E,Cyc_Tcutil_warn(loc,((_tmp1540="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1540,sizeof(char),74))),_tag_dyneither(_tmp153F,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD4A,_tmpD4B,_tmpD47);Cyc_Tcutil_check_field_atts(
loc,_tmpD47,_tmpD4C);}}goto _LL588;}_LL5AD: {struct Cyc_Absyn_AggrType_struct*
_tmpB6A=(struct Cyc_Absyn_AggrType_struct*)_tmpB1D;if(_tmpB6A->tag != 12)goto
_LL5AF;else{_tmpB6B=_tmpB6A->f1;_tmpB6C=_tmpB6B.aggr_info;_tmpB6D=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB6A->f1).aggr_info;_tmpB6E=_tmpB6B.targs;_tmpB6F=(struct Cyc_List_List**)&(
_tmpB6A->f1).targs;}}_LL5AE:{union Cyc_Absyn_AggrInfoU _tmpD55=*_tmpB6D;struct
_tuple4 _tmpD56;enum Cyc_Absyn_AggrKind _tmpD57;struct _tuple2*_tmpD58;struct Cyc_Core_Opt*
_tmpD59;struct Cyc_Absyn_Aggrdecl**_tmpD5A;struct Cyc_Absyn_Aggrdecl*_tmpD5B;
_LL684: if((_tmpD55.UnknownAggr).tag != 1)goto _LL686;_tmpD56=(struct _tuple4)(
_tmpD55.UnknownAggr).val;_tmpD57=_tmpD56.f1;_tmpD58=_tmpD56.f2;_tmpD59=_tmpD56.f3;
_LL685: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD5C;_push_handler(&
_tmpD5C);{int _tmpD5E=0;if(setjmp(_tmpD5C.handler))_tmpD5E=1;if(!_tmpD5E){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD58);{struct Cyc_Absyn_Aggrdecl*_tmpD5F=*adp;if(_tmpD5F->kind != 
_tmpD57){if(_tmpD5F->kind == Cyc_Absyn_StructA){const char*_tmp1545;void*_tmp1544[
2];struct Cyc_String_pa_struct _tmp1543;struct Cyc_String_pa_struct _tmp1542;(
_tmp1542.tag=0,((_tmp1542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD58)),((_tmp1543.tag=0,((_tmp1543.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD58)),((_tmp1544[0]=& _tmp1543,((
_tmp1544[1]=& _tmp1542,Cyc_Tcutil_terr(loc,((_tmp1545="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1545,sizeof(char),40))),_tag_dyneither(_tmp1544,sizeof(void*),
2)))))))))))));}else{const char*_tmp154A;void*_tmp1549[2];struct Cyc_String_pa_struct
_tmp1548;struct Cyc_String_pa_struct _tmp1547;(_tmp1547.tag=0,((_tmp1547.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD58)),((
_tmp1548.tag=0,((_tmp1548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD58)),((_tmp1549[0]=& _tmp1548,((_tmp1549[1]=& _tmp1547,Cyc_Tcutil_terr(loc,((
_tmp154A="expecting union %s instead of struct %s",_tag_dyneither(_tmp154A,
sizeof(char),40))),_tag_dyneither(_tmp1549,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD59  && (int)_tmpD59->v){if(!((unsigned int)_tmpD5F->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD5F->impl))->tagged){const char*
_tmp154E;void*_tmp154D[1];struct Cyc_String_pa_struct _tmp154C;(_tmp154C.tag=0,((
_tmp154C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD58)),((_tmp154D[0]=& _tmp154C,Cyc_Tcutil_terr(loc,((_tmp154E="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp154E,sizeof(char),42))),_tag_dyneither(_tmp154D,sizeof(void*),
1)))))));}}*_tmpB6D=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD5D=(void*)_exn_thrown;void*_tmpD6C=_tmpD5D;_LL689: {struct Cyc_Dict_Absent_struct*
_tmpD6D=(struct Cyc_Dict_Absent_struct*)_tmpD6C;if(_tmpD6D->tag != Cyc_Dict_Absent)
goto _LL68B;}_LL68A: {struct Cyc_Tcenv_Genv*_tmpD6E=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp154F;struct Cyc_Absyn_Aggrdecl*_tmpD6F=(_tmp154F=
_cycalloc(sizeof(*_tmp154F)),((_tmp154F->kind=_tmpD57,((_tmp154F->sc=Cyc_Absyn_Extern,((
_tmp154F->name=_tmpD58,((_tmp154F->tvs=0,((_tmp154F->impl=0,((_tmp154F->attributes=
0,_tmp154F)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD6E,loc,_tmpD6F);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD58);*_tmpB6D=Cyc_Absyn_KnownAggr(adp);if(*_tmpB6F != 0){{const char*
_tmp1553;void*_tmp1552[1];struct Cyc_String_pa_struct _tmp1551;(_tmp1551.tag=0,((
_tmp1551.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD58)),((_tmp1552[0]=& _tmp1551,Cyc_Tcutil_terr(loc,((_tmp1553="declare parameterized type %s before using",
_tag_dyneither(_tmp1553,sizeof(char),43))),_tag_dyneither(_tmp1552,sizeof(void*),
1)))))));}return cvtenv;}goto _LL688;}_LL68B:;_LL68C:(void)_throw(_tmpD6C);_LL688:;}};}
_tmpD5B=*adp;goto _LL687;}_LL686: if((_tmpD55.KnownAggr).tag != 2)goto _LL683;
_tmpD5A=(struct Cyc_Absyn_Aggrdecl**)(_tmpD55.KnownAggr).val;_tmpD5B=*_tmpD5A;
_LL687: {struct Cyc_List_List*tvs=_tmpD5B->tvs;struct Cyc_List_List*ts=*_tmpB6F;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD74=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD75=(void*)ts->hd;{struct _tuple0 _tmp1554;
struct _tuple0 _tmpD77=(_tmp1554.f1=Cyc_Absyn_compress_kb(_tmpD74->kind),((
_tmp1554.f2=_tmpD75,_tmp1554)));void*_tmpD78;void*_tmpD7A;struct Cyc_Absyn_Tvar*
_tmpD7C;_LL68E: _tmpD78=_tmpD77.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD79=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD78;if(_tmpD79->tag != 1)goto _LL690;};
_tmpD7A=_tmpD77.f2;{struct Cyc_Absyn_VarType_struct*_tmpD7B=(struct Cyc_Absyn_VarType_struct*)
_tmpD7A;if(_tmpD7B->tag != 2)goto _LL690;else{_tmpD7C=_tmpD7B->f1;}};_LL68F: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD7C);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD7C,1);continue;_LL690:;_LL691: goto _LL68D;_LL68D:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp1558;void*_tmp1557[1];struct Cyc_String_pa_struct
_tmp1556;(_tmp1556.tag=0,((_tmp1556.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5B->name)),((_tmp1557[0]=& _tmp1556,
Cyc_Tcutil_terr(loc,((_tmp1558="too many parameters for type %s",_tag_dyneither(
_tmp1558,sizeof(char),32))),_tag_dyneither(_tmp1557,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1559;
hidden_ts=((_tmp1559=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1559)),((
_tmp1559->hd=e,((_tmp1559->tl=hidden_ts,_tmp1559))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);}*_tmpB6F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB6F,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL683:;}goto _LL588;
_LL5AF: {struct Cyc_Absyn_TypedefType_struct*_tmpB70=(struct Cyc_Absyn_TypedefType_struct*)
_tmpB1D;if(_tmpB70->tag != 18)goto _LL5B1;else{_tmpB71=_tmpB70->f1;_tmpB72=_tmpB70->f2;
_tmpB73=(struct Cyc_List_List**)& _tmpB70->f2;_tmpB74=_tmpB70->f3;_tmpB75=(struct
Cyc_Absyn_Typedefdecl**)& _tmpB70->f3;_tmpB76=_tmpB70->f4;_tmpB77=(void***)&
_tmpB70->f4;}}_LL5B0: {struct Cyc_List_List*targs=*_tmpB73;struct Cyc_Absyn_Typedefdecl*
td;{struct _handler_cons _tmpD81;_push_handler(& _tmpD81);{int _tmpD83=0;if(setjmp(
_tmpD81.handler))_tmpD83=1;if(!_tmpD83){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpB71);;_pop_handler();}else{void*_tmpD82=(void*)_exn_thrown;void*_tmpD85=
_tmpD82;_LL693: {struct Cyc_Dict_Absent_struct*_tmpD86=(struct Cyc_Dict_Absent_struct*)
_tmpD85;if(_tmpD86->tag != Cyc_Dict_Absent)goto _LL695;}_LL694:{const char*_tmp155D;
void*_tmp155C[1];struct Cyc_String_pa_struct _tmp155B;(_tmp155B.tag=0,((_tmp155B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB71)),((
_tmp155C[0]=& _tmp155B,Cyc_Tcutil_terr(loc,((_tmp155D="unbound typedef name %s",
_tag_dyneither(_tmp155D,sizeof(char),24))),_tag_dyneither(_tmp155C,sizeof(void*),
1)))))));}return cvtenv;_LL695:;_LL696:(void)_throw(_tmpD85);_LL692:;}};}*_tmpB75=(
struct Cyc_Absyn_Typedefdecl*)td;_tmpB71[0]=(td->name)[0];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpD8A=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple15*_tmp1560;struct Cyc_List_List*_tmp155F;inst=((_tmp155F=
_region_malloc(_tmpD8A,sizeof(*_tmp155F)),((_tmp155F->hd=((_tmp1560=
_region_malloc(_tmpD8A,sizeof(*_tmp1560)),((_tmp1560->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1560->f2=(void*)ts->hd,_tmp1560)))))),((_tmp155F->tl=inst,_tmp155F))))));};}
if(ts != 0){const char*_tmp1564;void*_tmp1563[1];struct Cyc_String_pa_struct
_tmp1562;(_tmp1562.tag=0,((_tmp1562.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB71)),((_tmp1563[0]=& _tmp1562,Cyc_Tcutil_terr(
loc,((_tmp1564="too many parameters for typedef %s",_tag_dyneither(_tmp1564,
sizeof(char),35))),_tag_dyneither(_tmp1563,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp1565;hidden_ts=((_tmp1565=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1565)),((_tmp1565->hd=e,((_tmp1565->tl=hidden_ts,_tmp1565))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{struct _tuple15*
_tmp1568;struct Cyc_List_List*_tmp1567;inst=(struct Cyc_List_List*)((_tmp1567=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1567)),((_tmp1567->hd=(struct
_tuple15*)((_tmp1568=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1568)),((
_tmp1568->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1568->f2=e,_tmp1568)))))),((
_tmp1567->tl=inst,_tmp1567))))));};}*_tmpB73=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD8A,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1569;*_tmpB77=((_tmp1569=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1569)),((_tmp1569[0]=new_typ,_tmp1569))));}}goto _LL588;};}_LL5B1: {
struct Cyc_Absyn_RefCntRgn_struct*_tmpB78=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmpB1D;if(_tmpB78->tag != 23)goto _LL5B3;}_LL5B2: goto _LL5B4;_LL5B3: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpB79=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpB1D;if(_tmpB79->tag != 22)goto
_LL5B5;}_LL5B4: goto _LL5B6;_LL5B5: {struct Cyc_Absyn_HeapRgn_struct*_tmpB7A=(
struct Cyc_Absyn_HeapRgn_struct*)_tmpB1D;if(_tmpB7A->tag != 21)goto _LL5B7;}_LL5B6:
goto _LL588;_LL5B7: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB7B=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmpB1D;if(_tmpB7B->tag != 16)goto _LL5B9;else{_tmpB7C=(void*)_tmpB7B->f1;}}_LL5B8:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB7C,1);
goto _LL588;_LL5B9: {struct Cyc_Absyn_DynRgnType_struct*_tmpB7D=(struct Cyc_Absyn_DynRgnType_struct*)
_tmpB1D;if(_tmpB7D->tag != 17)goto _LL5BB;else{_tmpB7E=(void*)_tmpB7D->f1;_tmpB7F=(
void*)_tmpB7D->f2;}}_LL5BA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_rk,_tmpB7E,0);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,
_tmpB7F,1);goto _LL588;_LL5BB: {struct Cyc_Absyn_AccessEff_struct*_tmpB80=(struct
Cyc_Absyn_AccessEff_struct*)_tmpB1D;if(_tmpB80->tag != 24)goto _LL5BD;else{_tmpB81=(
void*)_tmpB80->f1;}}_LL5BC: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,&
Cyc_Tcutil_trk,_tmpB81,1);goto _LL588;_LL5BD: {struct Cyc_Absyn_RgnsEff_struct*
_tmpB82=(struct Cyc_Absyn_RgnsEff_struct*)_tmpB1D;if(_tmpB82->tag != 26)goto _LL5BF;
else{_tmpB83=(void*)_tmpB82->f1;}}_LL5BE: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB83,1);goto _LL588;_LL5BF: {struct Cyc_Absyn_JoinEff_struct*
_tmpB84=(struct Cyc_Absyn_JoinEff_struct*)_tmpB1D;if(_tmpB84->tag != 25)goto _LL588;
else{_tmpB85=_tmpB84->f1;}}_LL5C0: for(0;_tmpB85 != 0;_tmpB85=_tmpB85->tl){cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB85->hd,1);}
goto _LL588;_LL588:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){{
void*_tmpD94=t;struct Cyc_Core_Opt*_tmpD96;struct Cyc_Core_Opt*_tmpD97;_LL698: {
struct Cyc_Absyn_Evar_struct*_tmpD95=(struct Cyc_Absyn_Evar_struct*)_tmpD94;if(
_tmpD95->tag != 1)goto _LL69A;else{_tmpD96=_tmpD95->f1;_tmpD97=_tmpD95->f2;}}
_LL699: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*_tmpD98=_tmpD96;struct Cyc_Core_Opt
_tmpD99;struct Cyc_Absyn_Kind*_tmpD9A;_LL69D: if(_tmpD98 != 0)goto _LL69F;_LL69E:{
const char*_tmp156A;s=((_tmp156A="kind=NULL ",_tag_dyneither(_tmp156A,sizeof(char),
11)));}goto _LL69C;_LL69F: if(_tmpD98 == 0)goto _LL69C;_tmpD99=*_tmpD98;_tmpD9A=(
struct Cyc_Absyn_Kind*)_tmpD99.v;_LL6A0:{const char*_tmp156E;void*_tmp156D[1];
struct Cyc_String_pa_struct _tmp156C;s=(struct _dyneither_ptr)((_tmp156C.tag=0,((
_tmp156C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmpD9A)),((_tmp156D[0]=& _tmp156C,Cyc_aprintf(((_tmp156E="kind=%s ",
_tag_dyneither(_tmp156E,sizeof(char),9))),_tag_dyneither(_tmp156D,sizeof(void*),
1))))))));}goto _LL69C;_LL69C:;}{struct Cyc_Core_Opt*_tmpD9F=_tmpD97;struct Cyc_Core_Opt
_tmpDA0;void*_tmpDA1;_LL6A2: if(_tmpD9F != 0)goto _LL6A4;_LL6A3:{const char*_tmp1572;
void*_tmp1571[1];struct Cyc_String_pa_struct _tmp1570;s=(struct _dyneither_ptr)((
_tmp1570.tag=0,((_tmp1570.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp1571[0]=& _tmp1570,Cyc_aprintf(((_tmp1572="%s ref=NULL",_tag_dyneither(
_tmp1572,sizeof(char),12))),_tag_dyneither(_tmp1571,sizeof(void*),1))))))));}
goto _LL6A1;_LL6A4: if(_tmpD9F == 0)goto _LL6A1;_tmpDA0=*_tmpD9F;_tmpDA1=(void*)
_tmpDA0.v;_LL6A5:{const char*_tmp1577;void*_tmp1576[2];struct Cyc_String_pa_struct
_tmp1575;struct Cyc_String_pa_struct _tmp1574;s=(struct _dyneither_ptr)((_tmp1574.tag=
0,((_tmp1574.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpDA1)),((_tmp1575.tag=0,((_tmp1575.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s),((_tmp1576[0]=& _tmp1575,((_tmp1576[1]=& _tmp1574,Cyc_aprintf(((
_tmp1577="%s ref=%s",_tag_dyneither(_tmp1577,sizeof(char),10))),_tag_dyneither(
_tmp1576,sizeof(void*),2))))))))))))));}goto _LL6A1;_LL6A1:;}goto _LL697;}_LL69A:;
_LL69B: goto _LL697;_LL697:;}{const char*_tmp157D;void*_tmp157C[3];struct Cyc_String_pa_struct
_tmp157B;struct Cyc_String_pa_struct _tmp157A;struct Cyc_String_pa_struct _tmp1579;(
_tmp1579.tag=0,((_tmp1579.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp157A.tag=0,((_tmp157A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp157B.tag=0,((
_tmp157B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp157C[0]=& _tmp157B,((_tmp157C[1]=& _tmp157A,((_tmp157C[2]=& _tmp1579,Cyc_Tcutil_terr(
loc,((_tmp157D="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp157D,sizeof(char),51))),_tag_dyneither(_tmp157C,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpDAE=e->r;struct Cyc_List_List*_tmpDB4;struct Cyc_Absyn_Exp*_tmpDB6;struct
Cyc_Absyn_Exp*_tmpDB7;struct Cyc_Absyn_Exp*_tmpDB8;struct Cyc_Absyn_Exp*_tmpDBA;
struct Cyc_Absyn_Exp*_tmpDBB;struct Cyc_Absyn_Exp*_tmpDBD;struct Cyc_Absyn_Exp*
_tmpDBE;struct Cyc_Absyn_Exp*_tmpDC0;struct Cyc_Absyn_Exp*_tmpDC1;void*_tmpDC3;
struct Cyc_Absyn_Exp*_tmpDC4;void*_tmpDC6;void*_tmpDC8;void*_tmpDCA;struct Cyc_Absyn_Exp*
_tmpDCC;_LL6A7: {struct Cyc_Absyn_Const_e_struct*_tmpDAF=(struct Cyc_Absyn_Const_e_struct*)
_tmpDAE;if(_tmpDAF->tag != 0)goto _LL6A9;}_LL6A8: goto _LL6AA;_LL6A9: {struct Cyc_Absyn_Enum_e_struct*
_tmpDB0=(struct Cyc_Absyn_Enum_e_struct*)_tmpDAE;if(_tmpDB0->tag != 32)goto _LL6AB;}
_LL6AA: goto _LL6AC;_LL6AB: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDB1=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDAE;if(_tmpDB1->tag != 33)goto _LL6AD;}_LL6AC: goto _LL6AE;_LL6AD: {struct Cyc_Absyn_Var_e_struct*
_tmpDB2=(struct Cyc_Absyn_Var_e_struct*)_tmpDAE;if(_tmpDB2->tag != 1)goto _LL6AF;}
_LL6AE: goto _LL6A6;_LL6AF: {struct Cyc_Absyn_Primop_e_struct*_tmpDB3=(struct Cyc_Absyn_Primop_e_struct*)
_tmpDAE;if(_tmpDB3->tag != 3)goto _LL6B1;else{_tmpDB4=_tmpDB3->f2;}}_LL6B0: for(0;
_tmpDB4 != 0;_tmpDB4=_tmpDB4->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpDB4->hd,te,cvtenv);}goto _LL6A6;_LL6B1: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDB5=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDAE;if(_tmpDB5->tag != 6)goto
_LL6B3;else{_tmpDB6=_tmpDB5->f1;_tmpDB7=_tmpDB5->f2;_tmpDB8=_tmpDB5->f3;}}_LL6B2:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB6,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDB7,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB8,te,
cvtenv);goto _LL6A6;_LL6B3: {struct Cyc_Absyn_And_e_struct*_tmpDB9=(struct Cyc_Absyn_And_e_struct*)
_tmpDAE;if(_tmpDB9->tag != 7)goto _LL6B5;else{_tmpDBA=_tmpDB9->f1;_tmpDBB=_tmpDB9->f2;}}
_LL6B4: _tmpDBD=_tmpDBA;_tmpDBE=_tmpDBB;goto _LL6B6;_LL6B5: {struct Cyc_Absyn_Or_e_struct*
_tmpDBC=(struct Cyc_Absyn_Or_e_struct*)_tmpDAE;if(_tmpDBC->tag != 8)goto _LL6B7;
else{_tmpDBD=_tmpDBC->f1;_tmpDBE=_tmpDBC->f2;}}_LL6B6: _tmpDC0=_tmpDBD;_tmpDC1=
_tmpDBE;goto _LL6B8;_LL6B7: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDBF=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDAE;if(_tmpDBF->tag != 9)goto _LL6B9;else{_tmpDC0=_tmpDBF->f1;_tmpDC1=_tmpDBF->f2;}}
_LL6B8: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC0,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC1,te,cvtenv);goto _LL6A6;_LL6B9: {
struct Cyc_Absyn_Cast_e_struct*_tmpDC2=(struct Cyc_Absyn_Cast_e_struct*)_tmpDAE;
if(_tmpDC2->tag != 15)goto _LL6BB;else{_tmpDC3=(void*)_tmpDC2->f1;_tmpDC4=_tmpDC2->f2;}}
_LL6BA: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC4,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDC3,1);goto
_LL6A6;_LL6BB: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDC5=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDAE;if(_tmpDC5->tag != 20)goto _LL6BD;else{_tmpDC6=(void*)_tmpDC5->f1;}}_LL6BC:
_tmpDC8=_tmpDC6;goto _LL6BE;_LL6BD: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDC7=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDAE;if(_tmpDC7->tag != 18)goto _LL6BF;
else{_tmpDC8=(void*)_tmpDC7->f1;}}_LL6BE: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDC8,1);goto _LL6A6;_LL6BF: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDC9=(struct Cyc_Absyn_Valueof_e_struct*)_tmpDAE;if(_tmpDC9->tag != 39)goto
_LL6C1;else{_tmpDCA=(void*)_tmpDC9->f1;}}_LL6C0: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDCA,1);goto _LL6A6;_LL6C1: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDCB=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDAE;if(_tmpDCB->tag != 19)goto
_LL6C3;else{_tmpDCC=_tmpDCB->f1;}}_LL6C2: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDCC,te,cvtenv);goto _LL6A6;_LL6C3:;_LL6C4: {const char*_tmp1580;void*_tmp157F;(
_tmp157F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1580="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1580,sizeof(char),66))),_tag_dyneither(_tmp157F,sizeof(void*),
0)));}_LL6A6:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDCF=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDD1;struct
_tuple29 _tmpDD0=*((struct _tuple29*)vs->hd);_tmpDD1=_tmpDD0.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDCF,_tmpDD1);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDD3;struct _tuple30 _tmpDD2=*((struct _tuple30*)evs->hd);_tmpDD3=_tmpDD2.f1;{
void*_tmpDD4=Cyc_Tcutil_compress(_tmpDD3);struct Cyc_Core_Opt*_tmpDD6;struct Cyc_Core_Opt**
_tmpDD7;_LL6C6: {struct Cyc_Absyn_Evar_struct*_tmpDD5=(struct Cyc_Absyn_Evar_struct*)
_tmpDD4;if(_tmpDD5->tag != 1)goto _LL6C8;else{_tmpDD6=_tmpDD5->f4;_tmpDD7=(struct
Cyc_Core_Opt**)& _tmpDD5->f4;}}_LL6C7: if(*_tmpDD7 == 0){struct Cyc_Core_Opt*
_tmp1581;*_tmpDD7=((_tmp1581=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1581)),((_tmp1581->v=_tmpDCF,_tmp1581))));}else{struct Cyc_List_List*_tmpDD9=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpDD7))->v;struct Cyc_List_List*
_tmpDDA=0;for(0;_tmpDD9 != 0;_tmpDD9=_tmpDD9->tl){if(((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpDCF,(struct Cyc_Absyn_Tvar*)_tmpDD9->hd)){
struct Cyc_List_List*_tmp1582;_tmpDDA=((_tmp1582=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1582)),((_tmp1582->hd=(struct Cyc_Absyn_Tvar*)_tmpDD9->hd,((_tmp1582->tl=
_tmpDDA,_tmp1582))))));}}{struct Cyc_Core_Opt*_tmp1583;*_tmpDD7=((_tmp1583=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1583)),((_tmp1583->v=_tmpDDA,
_tmp1583))));};}goto _LL6C5;_LL6C8:;_LL6C9: goto _LL6C5;_LL6C5:;};}}return cvt;}void
Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*
_tmpDDD=Cyc_Tcenv_lookup_type_vars(te);struct _RegionHandle*_tmpDDE=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_Tcutil_CVTEnv _tmp1584;struct Cyc_Tcutil_CVTEnv _tmpDDF=Cyc_Tcutil_check_valid_type(
loc,te,((_tmp1584.r=_tmpDDE,((_tmp1584.kind_env=_tmpDDD,((_tmp1584.free_vars=0,((
_tmp1584.free_evars=0,((_tmp1584.generalize_evars=generalize_evars,((_tmp1584.fn_result=
0,_tmp1584)))))))))))),& Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDE0=((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple29*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpDDE,(struct Cyc_Absyn_Tvar*(*)(struct
_tuple29*))Cyc_Core_fst,_tmpDDF.free_vars);struct Cyc_List_List*_tmpDE1=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple30*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmpDDE,(void*(*)(struct _tuple30*))Cyc_Core_fst,_tmpDDF.free_evars);
if(_tmpDDD != 0){struct Cyc_List_List*_tmpDE2=0;{struct Cyc_List_List*_tmpDE3=
_tmpDE0;for(0;(unsigned int)_tmpDE3;_tmpDE3=_tmpDE3->tl){struct Cyc_Absyn_Tvar*
_tmpDE4=(struct Cyc_Absyn_Tvar*)_tmpDE3->hd;int found=0;{struct Cyc_List_List*
_tmpDE5=_tmpDDD;for(0;(unsigned int)_tmpDE5;_tmpDE5=_tmpDE5->tl){if(Cyc_Absyn_tvar_cmp(
_tmpDE4,(struct Cyc_Absyn_Tvar*)_tmpDE5->hd)== 0){found=1;break;}}}if(!found){
struct Cyc_List_List*_tmp1585;_tmpDE2=((_tmp1585=_region_malloc(_tmpDDE,sizeof(*
_tmp1585)),((_tmp1585->hd=(struct Cyc_Absyn_Tvar*)_tmpDE3->hd,((_tmp1585->tl=
_tmpDE2,_tmp1585))))));}}}_tmpDE0=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpDE2);}{struct Cyc_List_List*x=_tmpDE0;for(0;x != 0;x=x->tl){
void*_tmpDE7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct
Cyc_Core_Opt*_tmpDE9;struct Cyc_Core_Opt**_tmpDEA;struct Cyc_Core_Opt*_tmpDEC;
struct Cyc_Core_Opt**_tmpDED;struct Cyc_Absyn_Kind*_tmpDEE;struct Cyc_Absyn_Kind
_tmpDEF;enum Cyc_Absyn_KindQual _tmpDF0;enum Cyc_Absyn_AliasQual _tmpDF1;struct Cyc_Core_Opt*
_tmpDF3;struct Cyc_Core_Opt**_tmpDF4;struct Cyc_Absyn_Kind*_tmpDF5;struct Cyc_Absyn_Kind
_tmpDF6;enum Cyc_Absyn_KindQual _tmpDF7;enum Cyc_Absyn_AliasQual _tmpDF8;struct Cyc_Core_Opt*
_tmpDFA;struct Cyc_Core_Opt**_tmpDFB;struct Cyc_Absyn_Kind*_tmpDFC;struct Cyc_Absyn_Kind
_tmpDFD;enum Cyc_Absyn_KindQual _tmpDFE;enum Cyc_Absyn_AliasQual _tmpDFF;struct Cyc_Core_Opt*
_tmpE01;struct Cyc_Core_Opt**_tmpE02;struct Cyc_Absyn_Kind*_tmpE03;struct Cyc_Absyn_Kind
_tmpE04;enum Cyc_Absyn_KindQual _tmpE05;enum Cyc_Absyn_AliasQual _tmpE06;struct Cyc_Core_Opt*
_tmpE08;struct Cyc_Core_Opt**_tmpE09;struct Cyc_Absyn_Kind*_tmpE0A;struct Cyc_Absyn_Kind
_tmpE0B;enum Cyc_Absyn_KindQual _tmpE0C;enum Cyc_Absyn_AliasQual _tmpE0D;struct Cyc_Core_Opt*
_tmpE0F;struct Cyc_Core_Opt**_tmpE10;struct Cyc_Absyn_Kind*_tmpE11;struct Cyc_Absyn_Kind*
_tmpE13;struct Cyc_Absyn_Kind _tmpE14;enum Cyc_Absyn_KindQual _tmpE15;enum Cyc_Absyn_AliasQual
_tmpE16;_LL6CB: {struct Cyc_Absyn_Unknown_kb_struct*_tmpDE8=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDE7;if(_tmpDE8->tag != 1)goto _LL6CD;else{_tmpDE9=_tmpDE8->f1;_tmpDEA=(struct
Cyc_Core_Opt**)& _tmpDE8->f1;}}_LL6CC: _tmpDED=_tmpDEA;goto _LL6CE;_LL6CD: {struct
Cyc_Absyn_Less_kb_struct*_tmpDEB=(struct Cyc_Absyn_Less_kb_struct*)_tmpDE7;if(
_tmpDEB->tag != 2)goto _LL6CF;else{_tmpDEC=_tmpDEB->f1;_tmpDED=(struct Cyc_Core_Opt**)&
_tmpDEB->f1;_tmpDEE=_tmpDEB->f2;_tmpDEF=*_tmpDEE;_tmpDF0=_tmpDEF.kind;if(_tmpDF0
!= Cyc_Absyn_BoxKind)goto _LL6CF;_tmpDF1=_tmpDEF.aliasqual;if(_tmpDF1 != Cyc_Absyn_Top)
goto _LL6CF;}}_LL6CE: _tmpDF4=_tmpDED;goto _LL6D0;_LL6CF: {struct Cyc_Absyn_Less_kb_struct*
_tmpDF2=(struct Cyc_Absyn_Less_kb_struct*)_tmpDE7;if(_tmpDF2->tag != 2)goto _LL6D1;
else{_tmpDF3=_tmpDF2->f1;_tmpDF4=(struct Cyc_Core_Opt**)& _tmpDF2->f1;_tmpDF5=
_tmpDF2->f2;_tmpDF6=*_tmpDF5;_tmpDF7=_tmpDF6.kind;if(_tmpDF7 != Cyc_Absyn_MemKind)
goto _LL6D1;_tmpDF8=_tmpDF6.aliasqual;if(_tmpDF8 != Cyc_Absyn_Top)goto _LL6D1;}}
_LL6D0: _tmpDFB=_tmpDF4;goto _LL6D2;_LL6D1: {struct Cyc_Absyn_Less_kb_struct*
_tmpDF9=(struct Cyc_Absyn_Less_kb_struct*)_tmpDE7;if(_tmpDF9->tag != 2)goto _LL6D3;
else{_tmpDFA=_tmpDF9->f1;_tmpDFB=(struct Cyc_Core_Opt**)& _tmpDF9->f1;_tmpDFC=
_tmpDF9->f2;_tmpDFD=*_tmpDFC;_tmpDFE=_tmpDFD.kind;if(_tmpDFE != Cyc_Absyn_MemKind)
goto _LL6D3;_tmpDFF=_tmpDFD.aliasqual;if(_tmpDFF != Cyc_Absyn_Aliasable)goto _LL6D3;}}
_LL6D2:*_tmpDFB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6CA;_LL6D3: {
struct Cyc_Absyn_Less_kb_struct*_tmpE00=(struct Cyc_Absyn_Less_kb_struct*)_tmpDE7;
if(_tmpE00->tag != 2)goto _LL6D5;else{_tmpE01=_tmpE00->f1;_tmpE02=(struct Cyc_Core_Opt**)&
_tmpE00->f1;_tmpE03=_tmpE00->f2;_tmpE04=*_tmpE03;_tmpE05=_tmpE04.kind;if(_tmpE05
!= Cyc_Absyn_MemKind)goto _LL6D5;_tmpE06=_tmpE04.aliasqual;if(_tmpE06 != Cyc_Absyn_Unique)
goto _LL6D5;}}_LL6D4:*_tmpE02=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6CA;_LL6D5: {struct Cyc_Absyn_Less_kb_struct*_tmpE07=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDE7;if(_tmpE07->tag != 2)goto _LL6D7;else{_tmpE08=_tmpE07->f1;_tmpE09=(struct
Cyc_Core_Opt**)& _tmpE07->f1;_tmpE0A=_tmpE07->f2;_tmpE0B=*_tmpE0A;_tmpE0C=_tmpE0B.kind;
if(_tmpE0C != Cyc_Absyn_RgnKind)goto _LL6D7;_tmpE0D=_tmpE0B.aliasqual;if(_tmpE0D != 
Cyc_Absyn_Top)goto _LL6D7;}}_LL6D6:*_tmpE09=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6CA;_LL6D7: {struct Cyc_Absyn_Less_kb_struct*_tmpE0E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDE7;if(_tmpE0E->tag != 2)goto _LL6D9;else{_tmpE0F=_tmpE0E->f1;_tmpE10=(struct
Cyc_Core_Opt**)& _tmpE0E->f1;_tmpE11=_tmpE0E->f2;}}_LL6D8:*_tmpE10=Cyc_Tcutil_kind_to_bound_opt(
_tmpE11);goto _LL6CA;_LL6D9: {struct Cyc_Absyn_Eq_kb_struct*_tmpE12=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDE7;if(_tmpE12->tag != 0)goto _LL6DB;else{_tmpE13=_tmpE12->f1;_tmpE14=*_tmpE13;
_tmpE15=_tmpE14.kind;if(_tmpE15 != Cyc_Absyn_MemKind)goto _LL6DB;_tmpE16=_tmpE14.aliasqual;}}
_LL6DA:{const char*_tmp158C;void*_tmp158B[2];struct Cyc_String_pa_struct _tmp158A;
struct Cyc_Absyn_Kind*_tmp1589;struct Cyc_String_pa_struct _tmp1588;(_tmp1588.tag=0,((
_tmp1588.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string((
struct Cyc_Absyn_Kind*)((_tmp1589=_region_malloc(Cyc_Core_unique_region,sizeof(*
_tmp1589)),((_tmp1589->kind=Cyc_Absyn_MemKind,((_tmp1589->aliasqual=_tmpE16,
_tmp1589)))))))),((_tmp158A.tag=0,((_tmp158A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp158B[
0]=& _tmp158A,((_tmp158B[1]=& _tmp1588,Cyc_Tcutil_terr(loc,((_tmp158C="type variable %s cannot have kind %s",
_tag_dyneither(_tmp158C,sizeof(char),37))),_tag_dyneither(_tmp158B,sizeof(void*),
2)))))))))))));}goto _LL6CA;_LL6DB:;_LL6DC: goto _LL6CA;_LL6CA:;}}if(_tmpDE0 != 0
 || _tmpDE1 != 0){{void*_tmpE1C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpE1E;struct Cyc_List_List*_tmpE1F;struct Cyc_List_List**_tmpE20;struct Cyc_Core_Opt*
_tmpE21;struct Cyc_Absyn_Tqual _tmpE22;void*_tmpE23;struct Cyc_List_List*_tmpE24;
int _tmpE25;struct Cyc_Absyn_VarargInfo*_tmpE26;struct Cyc_List_List*_tmpE27;struct
Cyc_List_List*_tmpE28;_LL6DE: {struct Cyc_Absyn_FnType_struct*_tmpE1D=(struct Cyc_Absyn_FnType_struct*)
_tmpE1C;if(_tmpE1D->tag != 10)goto _LL6E0;else{_tmpE1E=_tmpE1D->f1;_tmpE1F=_tmpE1E.tvars;
_tmpE20=(struct Cyc_List_List**)&(_tmpE1D->f1).tvars;_tmpE21=_tmpE1E.effect;
_tmpE22=_tmpE1E.ret_tqual;_tmpE23=_tmpE1E.ret_typ;_tmpE24=_tmpE1E.args;_tmpE25=
_tmpE1E.c_varargs;_tmpE26=_tmpE1E.cyc_varargs;_tmpE27=_tmpE1E.rgn_po;_tmpE28=
_tmpE1E.attributes;}}_LL6DF: if(*_tmpE20 == 0){*_tmpE20=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDE0);_tmpDE0=0;}goto _LL6DD;_LL6E0:;
_LL6E1: goto _LL6DD;_LL6DD:;}if(_tmpDE0 != 0){const char*_tmp1590;void*_tmp158F[1];
struct Cyc_String_pa_struct _tmp158E;(_tmp158E.tag=0,((_tmp158E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDE0->hd)->name),((
_tmp158F[0]=& _tmp158E,Cyc_Tcutil_terr(loc,((_tmp1590="unbound type variable %s",
_tag_dyneither(_tmp1590,sizeof(char),25))),_tag_dyneither(_tmp158F,sizeof(void*),
1)))))));}if(_tmpDE1 != 0)for(0;_tmpDE1 != 0;_tmpDE1=_tmpDE1->tl){void*e=(void*)
_tmpDE1->hd;struct Cyc_Absyn_Kind*_tmpE2C=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE2D;enum Cyc_Absyn_KindQual _tmpE2E;enum Cyc_Absyn_AliasQual _tmpE2F;struct Cyc_Absyn_Kind
_tmpE30;enum Cyc_Absyn_KindQual _tmpE31;enum Cyc_Absyn_AliasQual _tmpE32;struct Cyc_Absyn_Kind
_tmpE33;enum Cyc_Absyn_KindQual _tmpE34;enum Cyc_Absyn_AliasQual _tmpE35;struct Cyc_Absyn_Kind
_tmpE36;enum Cyc_Absyn_KindQual _tmpE37;_LL6E3: _tmpE2D=*_tmpE2C;_tmpE2E=_tmpE2D.kind;
if(_tmpE2E != Cyc_Absyn_RgnKind)goto _LL6E5;_tmpE2F=_tmpE2D.aliasqual;if(_tmpE2F != 
Cyc_Absyn_Unique)goto _LL6E5;_LL6E4: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1593;void*_tmp1592;(_tmp1592=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1593="can't unify evar with unique region!",
_tag_dyneither(_tmp1593,sizeof(char),37))),_tag_dyneither(_tmp1592,sizeof(void*),
0)));}goto _LL6E2;_LL6E5: _tmpE30=*_tmpE2C;_tmpE31=_tmpE30.kind;if(_tmpE31 != Cyc_Absyn_RgnKind)
goto _LL6E7;_tmpE32=_tmpE30.aliasqual;if(_tmpE32 != Cyc_Absyn_Aliasable)goto _LL6E7;
_LL6E6: goto _LL6E8;_LL6E7: _tmpE33=*_tmpE2C;_tmpE34=_tmpE33.kind;if(_tmpE34 != Cyc_Absyn_RgnKind)
goto _LL6E9;_tmpE35=_tmpE33.aliasqual;if(_tmpE35 != Cyc_Absyn_Top)goto _LL6E9;
_LL6E8: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp1596;
void*_tmp1595;(_tmp1595=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1596="can't unify evar with heap!",_tag_dyneither(
_tmp1596,sizeof(char),28))),_tag_dyneither(_tmp1595,sizeof(void*),0)));}goto
_LL6E2;_LL6E9: _tmpE36=*_tmpE2C;_tmpE37=_tmpE36.kind;if(_tmpE37 != Cyc_Absyn_EffKind)
goto _LL6EB;_LL6EA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp1599;void*_tmp1598;(_tmp1598=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1599="can't unify evar with {}!",
_tag_dyneither(_tmp1599,sizeof(char),26))),_tag_dyneither(_tmp1598,sizeof(void*),
0)));}goto _LL6E2;_LL6EB:;_LL6EC:{const char*_tmp159E;void*_tmp159D[2];struct Cyc_String_pa_struct
_tmp159C;struct Cyc_String_pa_struct _tmp159B;(_tmp159B.tag=0,((_tmp159B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp159C.tag=0,((_tmp159C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp159D[0]=& _tmp159C,((_tmp159D[1]=& _tmp159B,Cyc_Tcutil_terr(loc,((
_tmp159E="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp159E,sizeof(char),52))),_tag_dyneither(_tmp159D,sizeof(void*),2)))))))))))));}
goto _LL6E2;_LL6E2:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE43=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE45;struct Cyc_List_List*_tmpE46;struct Cyc_Core_Opt*
_tmpE47;struct Cyc_Absyn_Tqual _tmpE48;void*_tmpE49;struct Cyc_List_List*_tmpE4A;
_LL6EE: {struct Cyc_Absyn_FnType_struct*_tmpE44=(struct Cyc_Absyn_FnType_struct*)
_tmpE43;if(_tmpE44->tag != 10)goto _LL6F0;else{_tmpE45=_tmpE44->f1;_tmpE46=_tmpE45.tvars;
_tmpE47=_tmpE45.effect;_tmpE48=_tmpE45.ret_tqual;_tmpE49=_tmpE45.ret_typ;_tmpE4A=
_tmpE45.args;}}_LL6EF: fd->tvs=_tmpE46;fd->effect=_tmpE47;{struct Cyc_List_List*
_tmpE4B=fd->args;for(0;_tmpE4B != 0;(_tmpE4B=_tmpE4B->tl,_tmpE4A=_tmpE4A->tl)){(*((
struct _tuple25*)_tmpE4B->hd)).f2=(*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmpE4A))->hd)).f2;(*((struct _tuple25*)_tmpE4B->hd)).f3=(*((struct
_tuple9*)_tmpE4A->hd)).f3;}}fd->ret_tqual=_tmpE48;fd->ret_type=_tmpE49;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE49);
goto _LL6ED;_LL6F0:;_LL6F1: {const char*_tmp15A1;void*_tmp15A0;(_tmp15A0=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A1="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp15A1,sizeof(char),38))),_tag_dyneither(_tmp15A0,sizeof(void*),
0)));}_LL6ED:;}{struct _RegionHandle*_tmpE4E=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp15A2;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple25*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpE4E,(struct _dyneither_ptr*(*)(struct _tuple25*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp15A2="function declaration has repeated parameter",
_tag_dyneither(_tmp15A2,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp15A3;fd->cached_typ=((
_tmp15A3=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp15A3)),((_tmp15A3->v=
t,_tmp15A3))));};};}void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*
expected_kind,int allow_evars,void*t){struct _RegionHandle*_tmpE51=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_Tcutil_CVTEnv _tmp15A4;struct Cyc_Tcutil_CVTEnv _tmpE52=Cyc_Tcutil_check_valid_type(
loc,te,((_tmp15A4.r=_tmpE51,((_tmp15A4.kind_env=bound_tvars,((_tmp15A4.free_vars=
0,((_tmp15A4.free_evars=0,((_tmp15A4.generalize_evars=0,((_tmp15A4.fn_result=0,
_tmp15A4)))))))))))),expected_kind,t);struct Cyc_List_List*_tmpE53=Cyc_Tcutil_remove_bound_tvars(
_tmpE51,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE51,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple29*))Cyc_Core_fst,_tmpE52.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE54=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple30*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE51,(void*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpE52.free_evars);{struct Cyc_List_List*fs=_tmpE53;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE55=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp15A9;void*_tmp15A8[2];struct Cyc_String_pa_struct _tmp15A7;struct Cyc_String_pa_struct
_tmp15A6;(_tmp15A6.tag=0,((_tmp15A6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp15A7.tag=0,((_tmp15A7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE55),((_tmp15A8[0]=& _tmp15A7,((
_tmp15A8[1]=& _tmp15A6,Cyc_Tcutil_terr(loc,((_tmp15A9="unbound type variable %s in type %s",
_tag_dyneither(_tmp15A9,sizeof(char),36))),_tag_dyneither(_tmp15A8,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE54 != 0)for(0;_tmpE54 != 0;_tmpE54=
_tmpE54->tl){void*e=(void*)_tmpE54->hd;struct Cyc_Absyn_Kind*_tmpE5A=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE5B;enum Cyc_Absyn_KindQual _tmpE5C;enum Cyc_Absyn_AliasQual
_tmpE5D;struct Cyc_Absyn_Kind _tmpE5E;enum Cyc_Absyn_KindQual _tmpE5F;enum Cyc_Absyn_AliasQual
_tmpE60;struct Cyc_Absyn_Kind _tmpE61;enum Cyc_Absyn_KindQual _tmpE62;enum Cyc_Absyn_AliasQual
_tmpE63;struct Cyc_Absyn_Kind _tmpE64;enum Cyc_Absyn_KindQual _tmpE65;_LL6F3:
_tmpE5B=*_tmpE5A;_tmpE5C=_tmpE5B.kind;if(_tmpE5C != Cyc_Absyn_RgnKind)goto _LL6F5;
_tmpE5D=_tmpE5B.aliasqual;if(_tmpE5D != Cyc_Absyn_Unique)goto _LL6F5;_LL6F4: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp15AC;void*
_tmp15AB;(_tmp15AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15AC="can't unify evar with unique region!",
_tag_dyneither(_tmp15AC,sizeof(char),37))),_tag_dyneither(_tmp15AB,sizeof(void*),
0)));}goto _LL6F2;_LL6F5: _tmpE5E=*_tmpE5A;_tmpE5F=_tmpE5E.kind;if(_tmpE5F != Cyc_Absyn_RgnKind)
goto _LL6F7;_tmpE60=_tmpE5E.aliasqual;if(_tmpE60 != Cyc_Absyn_Aliasable)goto _LL6F7;
_LL6F6: goto _LL6F8;_LL6F7: _tmpE61=*_tmpE5A;_tmpE62=_tmpE61.kind;if(_tmpE62 != Cyc_Absyn_RgnKind)
goto _LL6F9;_tmpE63=_tmpE61.aliasqual;if(_tmpE63 != Cyc_Absyn_Top)goto _LL6F9;
_LL6F8: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp15AF;
void*_tmp15AE;(_tmp15AE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15AF="can't unify evar with heap!",_tag_dyneither(
_tmp15AF,sizeof(char),28))),_tag_dyneither(_tmp15AE,sizeof(void*),0)));}goto
_LL6F2;_LL6F9: _tmpE64=*_tmpE5A;_tmpE65=_tmpE64.kind;if(_tmpE65 != Cyc_Absyn_EffKind)
goto _LL6FB;_LL6FA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15B2;void*_tmp15B1;(_tmp15B1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15B2="can't unify evar with {}!",
_tag_dyneither(_tmp15B2,sizeof(char),26))),_tag_dyneither(_tmp15B1,sizeof(void*),
0)));}goto _LL6F2;_LL6FB:;_LL6FC:{const char*_tmp15B7;void*_tmp15B6[2];struct Cyc_String_pa_struct
_tmp15B5;struct Cyc_String_pa_struct _tmp15B4;(_tmp15B4.tag=0,((_tmp15B4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15B5.tag=0,((_tmp15B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15B6[0]=& _tmp15B5,((_tmp15B6[1]=& _tmp15B4,Cyc_Tcutil_terr(loc,((
_tmp15B7="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15B7,sizeof(char),52))),_tag_dyneither(_tmp15B6,sizeof(void*),2)))))))))))));}
goto _LL6F2;_LL6F2:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp15BC;void*_tmp15BB[2];struct Cyc_String_pa_struct
_tmp15BA;struct Cyc_String_pa_struct _tmp15B9;(_tmp15B9.tag=0,((_tmp15B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15BA.tag=
0,((_tmp15BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15BB[0]=&
_tmp15BA,((_tmp15BB[1]=& _tmp15B9,Cyc_Tcutil_terr(loc,((_tmp15BC="%s: %s",
_tag_dyneither(_tmp15BC,sizeof(char),7))),_tag_dyneither(_tmp15BB,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp15BD;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15BD="duplicate type variable",
_tag_dyneither(_tmp15BD,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp15BE;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15BE="",_tag_dyneither(_tmp15BE,
sizeof(char),1))))!= 0){struct _tuple32*_tmp15C1;struct Cyc_List_List*_tmp15C0;
fields=((_tmp15C0=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp15C0)),((
_tmp15C0->hd=((_tmp15C1=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp15C1)),((
_tmp15C1->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp15C1->f2=0,_tmp15C1)))))),((
_tmp15C0->tl=fields,_tmp15C0))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp15C3;const char*_tmp15C2;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp15C3="struct",
_tag_dyneither(_tmp15C3,sizeof(char),7)):((_tmp15C2="union",_tag_dyneither(
_tmp15C2,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple33 _tmpE7A;struct Cyc_List_List*_tmpE7B;void*_tmpE7C;struct _tuple33*
_tmpE79=(struct _tuple33*)des->hd;_tmpE7A=*_tmpE79;_tmpE7B=_tmpE7A.f1;_tmpE7C=
_tmpE7A.f2;if(_tmpE7B == 0){struct Cyc_List_List*_tmpE7D=fields;for(0;_tmpE7D != 0;
_tmpE7D=_tmpE7D->tl){if(!(*((struct _tuple32*)_tmpE7D->hd)).f2){(*((struct
_tuple32*)_tmpE7D->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp15C9;struct
Cyc_Absyn_FieldName_struct _tmp15C8;struct Cyc_List_List*_tmp15C7;(*((struct
_tuple33*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15C7=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp15C7)),((_tmp15C7->hd=(void*)((_tmp15C9=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp15C9)),((_tmp15C9[0]=((_tmp15C8.tag=1,((_tmp15C8.f1=((*((struct
_tuple32*)_tmpE7D->hd)).f1)->name,_tmp15C8)))),_tmp15C9)))),((_tmp15C7->tl=0,
_tmp15C7))))));}{struct _tuple34*_tmp15CC;struct Cyc_List_List*_tmp15CB;ans=((
_tmp15CB=_region_malloc(rgn,sizeof(*_tmp15CB)),((_tmp15CB->hd=((_tmp15CC=
_region_malloc(rgn,sizeof(*_tmp15CC)),((_tmp15CC->f1=(*((struct _tuple32*)_tmpE7D->hd)).f1,((
_tmp15CC->f2=_tmpE7C,_tmp15CC)))))),((_tmp15CB->tl=ans,_tmp15CB))))));}break;}}
if(_tmpE7D == 0){const char*_tmp15D0;void*_tmp15CF[1];struct Cyc_String_pa_struct
_tmp15CE;(_tmp15CE.tag=0,((_tmp15CE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)aggr_str),((_tmp15CF[0]=& _tmp15CE,Cyc_Tcutil_terr(loc,((_tmp15D0="too many arguments to %s",
_tag_dyneither(_tmp15D0,sizeof(char),25))),_tag_dyneither(_tmp15CF,sizeof(void*),
1)))))));}}else{if(_tmpE7B->tl != 0){const char*_tmp15D3;void*_tmp15D2;(_tmp15D2=0,
Cyc_Tcutil_terr(loc,((_tmp15D3="multiple designators are not yet supported",
_tag_dyneither(_tmp15D3,sizeof(char),43))),_tag_dyneither(_tmp15D2,sizeof(void*),
0)));}else{void*_tmpE88=(void*)_tmpE7B->hd;struct _dyneither_ptr*_tmpE8B;_LL6FE: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE89=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE88;if(_tmpE89->tag != 0)goto _LL700;}_LL6FF:{const char*_tmp15D7;void*_tmp15D6[
1];struct Cyc_String_pa_struct _tmp15D5;(_tmp15D5.tag=0,((_tmp15D5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15D6[0]=& _tmp15D5,Cyc_Tcutil_terr(
loc,((_tmp15D7="array designator used in argument to %s",_tag_dyneither(_tmp15D7,
sizeof(char),40))),_tag_dyneither(_tmp15D6,sizeof(void*),1)))))));}goto _LL6FD;
_LL700: {struct Cyc_Absyn_FieldName_struct*_tmpE8A=(struct Cyc_Absyn_FieldName_struct*)
_tmpE88;if(_tmpE8A->tag != 1)goto _LL6FD;else{_tmpE8B=_tmpE8A->f1;}}_LL701: {
struct Cyc_List_List*_tmpE8F=fields;for(0;_tmpE8F != 0;_tmpE8F=_tmpE8F->tl){if(Cyc_strptrcmp(
_tmpE8B,((*((struct _tuple32*)_tmpE8F->hd)).f1)->name)== 0){if((*((struct _tuple32*)
_tmpE8F->hd)).f2){const char*_tmp15DB;void*_tmp15DA[1];struct Cyc_String_pa_struct
_tmp15D9;(_tmp15D9.tag=0,((_tmp15D9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE8B),((_tmp15DA[0]=& _tmp15D9,Cyc_Tcutil_terr(loc,((_tmp15DB="member %s has already been used as an argument",
_tag_dyneither(_tmp15DB,sizeof(char),47))),_tag_dyneither(_tmp15DA,sizeof(void*),
1)))))));}(*((struct _tuple32*)_tmpE8F->hd)).f2=1;{struct _tuple34*_tmp15DE;struct
Cyc_List_List*_tmp15DD;ans=((_tmp15DD=_region_malloc(rgn,sizeof(*_tmp15DD)),((
_tmp15DD->hd=((_tmp15DE=_region_malloc(rgn,sizeof(*_tmp15DE)),((_tmp15DE->f1=(*((
struct _tuple32*)_tmpE8F->hd)).f1,((_tmp15DE->f2=_tmpE7C,_tmp15DE)))))),((
_tmp15DD->tl=ans,_tmp15DD))))));}break;}}if(_tmpE8F == 0){const char*_tmp15E2;void*
_tmp15E1[1];struct Cyc_String_pa_struct _tmp15E0;(_tmp15E0.tag=0,((_tmp15E0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE8B),((_tmp15E1[0]=& _tmp15E0,
Cyc_Tcutil_terr(loc,((_tmp15E2="bad field designator %s",_tag_dyneither(_tmp15E2,
sizeof(char),24))),_tag_dyneither(_tmp15E1,sizeof(void*),1)))))));}goto _LL6FD;}
_LL6FD:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){{const char*_tmp15E5;void*_tmp15E4;(
_tmp15E4=0,Cyc_Tcutil_terr(loc,((_tmp15E5="too few arguments to struct",
_tag_dyneither(_tmp15E5,sizeof(char),28))),_tag_dyneither(_tmp15E4,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple32*)fields->hd)).f2){if(found){const char*_tmp15E8;void*_tmp15E7;(_tmp15E7=
0,Cyc_Tcutil_terr(loc,((_tmp15E8="only one member of a union is allowed",
_tag_dyneither(_tmp15E8,sizeof(char),38))),_tag_dyneither(_tmp15E7,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15EB;void*_tmp15EA;(_tmp15EA=0,Cyc_Tcutil_terr(
loc,((_tmp15EB="missing member for union",_tag_dyneither(_tmp15EB,sizeof(char),
25))),_tag_dyneither(_tmp15EA,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEA0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEA2;void*_tmpEA3;struct Cyc_Absyn_PtrAtts _tmpEA4;union Cyc_Absyn_Constraint*
_tmpEA5;_LL703: {struct Cyc_Absyn_PointerType_struct*_tmpEA1=(struct Cyc_Absyn_PointerType_struct*)
_tmpEA0;if(_tmpEA1->tag != 5)goto _LL705;else{_tmpEA2=_tmpEA1->f1;_tmpEA3=_tmpEA2.elt_typ;
_tmpEA4=_tmpEA2.ptr_atts;_tmpEA5=_tmpEA4.bounds;}}_LL704: {void*_tmpEA6=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpEA5);_LL708: {struct Cyc_Absyn_DynEither_b_struct*_tmpEA7=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEA6;if(_tmpEA7->tag != 0)goto _LL70A;}_LL709:*elt_typ_dest=_tmpEA3;return 1;
_LL70A:;_LL70B: return 0;_LL707:;}_LL705:;_LL706: return 0;_LL702:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEA8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEAA;void*_tmpEAB;struct Cyc_Absyn_PtrAtts _tmpEAC;union Cyc_Absyn_Constraint*
_tmpEAD;_LL70D: {struct Cyc_Absyn_PointerType_struct*_tmpEA9=(struct Cyc_Absyn_PointerType_struct*)
_tmpEA8;if(_tmpEA9->tag != 5)goto _LL70F;else{_tmpEAA=_tmpEA9->f1;_tmpEAB=_tmpEAA.elt_typ;
_tmpEAC=_tmpEAA.ptr_atts;_tmpEAD=_tmpEAC.zero_term;}}_LL70E:*elt_typ_dest=
_tmpEAB;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpEAD);_LL70F:;_LL710: return 0;_LL70C:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEAE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEB0;void*_tmpEB1;struct Cyc_Absyn_PtrAtts _tmpEB2;
union Cyc_Absyn_Constraint*_tmpEB3;union Cyc_Absyn_Constraint*_tmpEB4;struct Cyc_Absyn_ArrayInfo
_tmpEB6;void*_tmpEB7;struct Cyc_Absyn_Tqual _tmpEB8;struct Cyc_Absyn_Exp*_tmpEB9;
union Cyc_Absyn_Constraint*_tmpEBA;_LL712: {struct Cyc_Absyn_PointerType_struct*
_tmpEAF=(struct Cyc_Absyn_PointerType_struct*)_tmpEAE;if(_tmpEAF->tag != 5)goto
_LL714;else{_tmpEB0=_tmpEAF->f1;_tmpEB1=_tmpEB0.elt_typ;_tmpEB2=_tmpEB0.ptr_atts;
_tmpEB3=_tmpEB2.bounds;_tmpEB4=_tmpEB2.zero_term;}}_LL713: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEB4)){*ptr_type=t;*elt_type=
_tmpEB1;{void*_tmpEBB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpEB3);_LL719: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEBC=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEBB;if(_tmpEBC->tag != 0)goto
_LL71B;}_LL71A:*is_dyneither=1;goto _LL718;_LL71B:;_LL71C:*is_dyneither=0;goto
_LL718;_LL718:;}return 1;}else{return 0;}_LL714: {struct Cyc_Absyn_ArrayType_struct*
_tmpEB5=(struct Cyc_Absyn_ArrayType_struct*)_tmpEAE;if(_tmpEB5->tag != 9)goto
_LL716;else{_tmpEB6=_tmpEB5->f1;_tmpEB7=_tmpEB6.elt_type;_tmpEB8=_tmpEB6.tq;
_tmpEB9=_tmpEB6.num_elts;_tmpEBA=_tmpEB6.zero_term;}}_LL715: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEBA)){*elt_type=_tmpEB7;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1600;struct Cyc_Absyn_PtrAtts
_tmp15FF;struct Cyc_Absyn_Upper_b_struct _tmp15FE;struct Cyc_Absyn_Upper_b_struct*
_tmp15FD;struct Cyc_Absyn_PtrInfo _tmp15FC;struct Cyc_Absyn_PointerType_struct*
_tmp15FB;*ptr_type=(void*)((_tmp15FB=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp15FB)),((_tmp15FB[0]=((_tmp1600.tag=5,((_tmp1600.f1=((_tmp15FC.elt_typ=
_tmpEB7,((_tmp15FC.elt_tq=_tmpEB8,((_tmp15FC.ptr_atts=((_tmp15FF.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp15FF.nullable=Cyc_Absyn_false_conref,((_tmp15FF.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp15FD=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp15FD)),((_tmp15FD[0]=((_tmp15FE.tag=1,((_tmp15FE.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmpEB9),_tmp15FE)))),_tmp15FD))))),((_tmp15FF.zero_term=_tmpEBA,((
_tmp15FF.ptrloc=0,_tmp15FF)))))))))),_tmp15FC)))))),_tmp1600)))),_tmp15FB))));}
return 1;}else{return 0;}_LL716:;_LL717: return 0;_LL711:;}int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpEC3=e1->r;struct Cyc_Absyn_Exp*_tmpEC6;struct Cyc_Absyn_Exp*_tmpEC8;struct Cyc_Absyn_Exp*
_tmpECA;struct Cyc_Absyn_Exp*_tmpECC;struct Cyc_Absyn_Exp*_tmpECE;struct Cyc_Absyn_Exp*
_tmpED0;_LL71E: {struct Cyc_Absyn_Cast_e_struct*_tmpEC4=(struct Cyc_Absyn_Cast_e_struct*)
_tmpEC3;if(_tmpEC4->tag != 15)goto _LL720;}_LL71F: {const char*_tmp1604;void*
_tmp1603[1];struct Cyc_String_pa_struct _tmp1602;(_tmp1602.tag=0,((_tmp1602.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1603[0]=& _tmp1602,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1604="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1604,sizeof(char),29))),_tag_dyneither(_tmp1603,sizeof(void*),1)))))));}
_LL720: {struct Cyc_Absyn_Deref_e_struct*_tmpEC5=(struct Cyc_Absyn_Deref_e_struct*)
_tmpEC3;if(_tmpEC5->tag != 21)goto _LL722;else{_tmpEC6=_tmpEC5->f1;}}_LL721:
_tmpEC8=_tmpEC6;goto _LL723;_LL722: {struct Cyc_Absyn_Subscript_e_struct*_tmpEC7=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpEC3;if(_tmpEC7->tag != 24)goto _LL724;
else{_tmpEC8=_tmpEC7->f1;}}_LL723: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEC8->topt))->v,ptr_type,is_dyneither,
elt_type);_LL724: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEC9=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEC3;if(_tmpEC9->tag != 23)goto _LL726;else{_tmpECA=_tmpEC9->f1;}}_LL725:
_tmpECC=_tmpECA;goto _LL727;_LL726: {struct Cyc_Absyn_AggrMember_e_struct*_tmpECB=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpEC3;if(_tmpECB->tag != 22)goto _LL728;
else{_tmpECC=_tmpECB->f1;}}_LL727: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpECC->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp1608;void*_tmp1607[1];struct Cyc_String_pa_struct _tmp1606;(_tmp1606.tag=
0,((_tmp1606.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1607[0]=& _tmp1606,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1608="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp1608,sizeof(char),51))),_tag_dyneither(_tmp1607,sizeof(void*),
1)))))));}return 0;_LL728: {struct Cyc_Absyn_Instantiate_e_struct*_tmpECD=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpEC3;if(_tmpECD->tag != 14)goto _LL72A;else{
_tmpECE=_tmpECD->f1;}}_LL729: _tmpED0=_tmpECE;goto _LL72B;_LL72A: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpECF=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpEC3;if(_tmpECF->tag != 13)
goto _LL72C;else{_tmpED0=_tmpECF->f1;}}_LL72B: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED0->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp160C;void*_tmp160B[1];struct Cyc_String_pa_struct
_tmp160A;(_tmp160A.tag=0,((_tmp160A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp160B[0]=& _tmp160A,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160C="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp160C,sizeof(char),49))),_tag_dyneither(_tmp160B,sizeof(void*),
1)))))));}return 0;_LL72C: {struct Cyc_Absyn_Var_e_struct*_tmpED1=(struct Cyc_Absyn_Var_e_struct*)
_tmpEC3;if(_tmpED1->tag != 1)goto _LL72E;}_LL72D: return 0;_LL72E:;_LL72F: {const
char*_tmp1610;void*_tmp160F[1];struct Cyc_String_pa_struct _tmp160E;(_tmp160E.tag=
0,((_tmp160E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp160F[0]=& _tmp160E,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1610="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1610,sizeof(char),39))),_tag_dyneither(_tmp160F,sizeof(void*),
1)))))));}_LL71D:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpEDE=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpEE2;_LL731: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpEDF=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpEDE;if(_tmpEDF->tag != 23)goto
_LL733;}_LL732: return !must_be_unique;_LL733: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpEE0=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpEDE;if(_tmpEE0->tag != 22)goto
_LL735;}_LL734: return 1;_LL735: {struct Cyc_Absyn_VarType_struct*_tmpEE1=(struct
Cyc_Absyn_VarType_struct*)_tmpEDE;if(_tmpEE1->tag != 2)goto _LL737;else{_tmpEE2=
_tmpEE1->f1;}}_LL736: {struct Cyc_Absyn_Kind _tmpEE4;enum Cyc_Absyn_KindQual
_tmpEE5;enum Cyc_Absyn_AliasQual _tmpEE6;struct Cyc_Absyn_Kind*_tmpEE3=Cyc_Tcutil_tvar_kind(
_tmpEE2,& Cyc_Tcutil_rk);_tmpEE4=*_tmpEE3;_tmpEE5=_tmpEE4.kind;_tmpEE6=_tmpEE4.aliasqual;
if(_tmpEE5 == Cyc_Absyn_RgnKind  && (_tmpEE6 == Cyc_Absyn_Unique  || _tmpEE6 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpEE7=Cyc_Absyn_compress_kb(_tmpEE2->kind);struct
Cyc_Core_Opt*_tmpEE9;struct Cyc_Core_Opt**_tmpEEA;struct Cyc_Absyn_Kind*_tmpEEB;
struct Cyc_Absyn_Kind _tmpEEC;enum Cyc_Absyn_KindQual _tmpEED;enum Cyc_Absyn_AliasQual
_tmpEEE;_LL73A: {struct Cyc_Absyn_Less_kb_struct*_tmpEE8=(struct Cyc_Absyn_Less_kb_struct*)
_tmpEE7;if(_tmpEE8->tag != 2)goto _LL73C;else{_tmpEE9=_tmpEE8->f1;_tmpEEA=(struct
Cyc_Core_Opt**)& _tmpEE8->f1;_tmpEEB=_tmpEE8->f2;_tmpEEC=*_tmpEEB;_tmpEED=_tmpEEC.kind;
if(_tmpEED != Cyc_Absyn_RgnKind)goto _LL73C;_tmpEEE=_tmpEEC.aliasqual;if(_tmpEEE != 
Cyc_Absyn_Top)goto _LL73C;}}_LL73B:{struct Cyc_Absyn_Less_kb_struct*_tmp1616;
struct Cyc_Absyn_Less_kb_struct _tmp1615;struct Cyc_Core_Opt*_tmp1614;*_tmpEEA=((
_tmp1614=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1614)),((_tmp1614->v=(
void*)((_tmp1616=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1616)),((
_tmp1616[0]=((_tmp1615.tag=2,((_tmp1615.f1=0,((_tmp1615.f2=& Cyc_Tcutil_rk,
_tmp1615)))))),_tmp1616)))),_tmp1614))));}return 0;_LL73C:;_LL73D: return 1;_LL739:;}
return 0;}_LL737:;_LL738: return 0;_LL730:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEF2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEF4;struct Cyc_Absyn_PtrAtts _tmpEF5;void*_tmpEF6;struct Cyc_Absyn_Tvar*_tmpEF8;
_LL73F: {struct Cyc_Absyn_PointerType_struct*_tmpEF3=(struct Cyc_Absyn_PointerType_struct*)
_tmpEF2;if(_tmpEF3->tag != 5)goto _LL741;else{_tmpEF4=_tmpEF3->f1;_tmpEF5=_tmpEF4.ptr_atts;
_tmpEF6=_tmpEF5.rgn;}}_LL740: return Cyc_Tcutil_is_noalias_region(_tmpEF6,
must_be_unique);_LL741: {struct Cyc_Absyn_VarType_struct*_tmpEF7=(struct Cyc_Absyn_VarType_struct*)
_tmpEF2;if(_tmpEF7->tag != 2)goto _LL743;else{_tmpEF8=_tmpEF7->f1;}}_LL742: {
struct Cyc_Absyn_Kind _tmpEFA;enum Cyc_Absyn_KindQual _tmpEFB;enum Cyc_Absyn_AliasQual
_tmpEFC;struct Cyc_Absyn_Kind*_tmpEF9=Cyc_Tcutil_tvar_kind(_tmpEF8,& Cyc_Tcutil_bk);
_tmpEFA=*_tmpEF9;_tmpEFB=_tmpEFA.kind;_tmpEFC=_tmpEFA.aliasqual;switch(_tmpEFB){
case Cyc_Absyn_BoxKind: _LL745: goto _LL746;case Cyc_Absyn_AnyKind: _LL746: goto _LL747;
case Cyc_Absyn_MemKind: _LL747: if(_tmpEFC == Cyc_Absyn_Unique  || _tmpEFC == Cyc_Absyn_Top){
void*_tmpEFD=Cyc_Absyn_compress_kb(_tmpEF8->kind);struct Cyc_Core_Opt*_tmpEFF;
struct Cyc_Core_Opt**_tmpF00;struct Cyc_Absyn_Kind*_tmpF01;struct Cyc_Absyn_Kind
_tmpF02;enum Cyc_Absyn_KindQual _tmpF03;enum Cyc_Absyn_AliasQual _tmpF04;_LL74A: {
struct Cyc_Absyn_Less_kb_struct*_tmpEFE=(struct Cyc_Absyn_Less_kb_struct*)_tmpEFD;
if(_tmpEFE->tag != 2)goto _LL74C;else{_tmpEFF=_tmpEFE->f1;_tmpF00=(struct Cyc_Core_Opt**)&
_tmpEFE->f1;_tmpF01=_tmpEFE->f2;_tmpF02=*_tmpF01;_tmpF03=_tmpF02.kind;_tmpF04=
_tmpF02.aliasqual;if(_tmpF04 != Cyc_Absyn_Top)goto _LL74C;}}_LL74B:{struct Cyc_Absyn_Less_kb_struct*
_tmp1620;struct Cyc_Absyn_Kind*_tmp161F;struct Cyc_Absyn_Less_kb_struct _tmp161E;
struct Cyc_Core_Opt*_tmp161D;*_tmpF00=((_tmp161D=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp161D)),((_tmp161D->v=(void*)((_tmp1620=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1620)),((_tmp1620[0]=((_tmp161E.tag=2,((_tmp161E.f1=0,((_tmp161E.f2=((
_tmp161F=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp161F)),((_tmp161F->kind=
_tmpF03,((_tmp161F->aliasqual=Cyc_Absyn_Aliasable,_tmp161F)))))),_tmp161E)))))),
_tmp1620)))),_tmp161D))));}return 0;_LL74C:;_LL74D: return 1;_LL749:;}return 0;
default: _LL748: return 0;}}_LL743:;_LL744: return 0;_LL73E:;}int Cyc_Tcutil_is_noalias_pointer(
void*t){return Cyc_Tcutil_is_noalias_pointer_aux(t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*rgn,void*t){void*_tmpF09=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpF09))return 1;{void*_tmpF0A=_tmpF09;struct Cyc_List_List*_tmpF0C;struct Cyc_Absyn_AggrInfo
_tmpF0E;union Cyc_Absyn_AggrInfoU _tmpF0F;struct Cyc_Absyn_Aggrdecl**_tmpF10;struct
Cyc_List_List*_tmpF11;struct Cyc_List_List*_tmpF13;struct Cyc_Absyn_AggrInfo
_tmpF15;union Cyc_Absyn_AggrInfoU _tmpF16;struct _tuple4 _tmpF17;struct Cyc_Absyn_DatatypeInfo
_tmpF19;union Cyc_Absyn_DatatypeInfoU _tmpF1A;struct Cyc_List_List*_tmpF1B;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF1D;union Cyc_Absyn_DatatypeFieldInfoU _tmpF1E;
struct Cyc_List_List*_tmpF1F;_LL750: {struct Cyc_Absyn_TupleType_struct*_tmpF0B=(
struct Cyc_Absyn_TupleType_struct*)_tmpF0A;if(_tmpF0B->tag != 11)goto _LL752;else{
_tmpF0C=_tmpF0B->f1;}}_LL751: while(_tmpF0C != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpF0C->hd)).f2))return 1;_tmpF0C=_tmpF0C->tl;}return 0;
_LL752: {struct Cyc_Absyn_AggrType_struct*_tmpF0D=(struct Cyc_Absyn_AggrType_struct*)
_tmpF0A;if(_tmpF0D->tag != 12)goto _LL754;else{_tmpF0E=_tmpF0D->f1;_tmpF0F=_tmpF0E.aggr_info;
if((_tmpF0F.KnownAggr).tag != 2)goto _LL754;_tmpF10=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF0F.KnownAggr).val;_tmpF11=_tmpF0E.targs;}}_LL753: if((*_tmpF10)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF20=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpF10)->tvs,_tmpF11);struct Cyc_List_List*_tmpF21=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF10)->impl))->fields;void*t;
while(_tmpF21 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF20,((struct Cyc_Absyn_Aggrfield*)
_tmpF21->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF21=_tmpF21->tl;}return 0;}_LL754: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF12=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF0A;if(_tmpF12->tag != 13)goto
_LL756;else{_tmpF13=_tmpF12->f2;}}_LL755: while(_tmpF13 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF13->hd)->type))return 1;_tmpF13=_tmpF13->tl;}
return 0;_LL756: {struct Cyc_Absyn_AggrType_struct*_tmpF14=(struct Cyc_Absyn_AggrType_struct*)
_tmpF0A;if(_tmpF14->tag != 12)goto _LL758;else{_tmpF15=_tmpF14->f1;_tmpF16=_tmpF15.aggr_info;
if((_tmpF16.UnknownAggr).tag != 1)goto _LL758;_tmpF17=(struct _tuple4)(_tmpF16.UnknownAggr).val;}}
_LL757: {const char*_tmp1623;void*_tmp1622;(_tmp1622=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1623="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1623,sizeof(char),36))),_tag_dyneither(_tmp1622,sizeof(void*),
0)));}_LL758: {struct Cyc_Absyn_DatatypeType_struct*_tmpF18=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpF0A;if(_tmpF18->tag != 3)goto _LL75A;else{_tmpF19=_tmpF18->f1;_tmpF1A=_tmpF19.datatype_info;
_tmpF1B=_tmpF19.targs;}}_LL759: {union Cyc_Absyn_DatatypeInfoU _tmpF24=_tmpF1A;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF25;struct _tuple2*_tmpF26;int _tmpF27;
struct Cyc_Absyn_Datatypedecl**_tmpF28;struct Cyc_Absyn_Datatypedecl*_tmpF29;
struct Cyc_Absyn_Datatypedecl _tmpF2A;struct Cyc_List_List*_tmpF2B;struct Cyc_Core_Opt*
_tmpF2C;_LL75F: if((_tmpF24.UnknownDatatype).tag != 1)goto _LL761;_tmpF25=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF24.UnknownDatatype).val;_tmpF26=_tmpF25.name;
_tmpF27=_tmpF25.is_extensible;_LL760: {const char*_tmp1626;void*_tmp1625;(
_tmp1625=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1626="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1626,
sizeof(char),40))),_tag_dyneither(_tmp1625,sizeof(void*),0)));}_LL761: if((
_tmpF24.KnownDatatype).tag != 2)goto _LL75E;_tmpF28=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF24.KnownDatatype).val;_tmpF29=*_tmpF28;_tmpF2A=*_tmpF29;_tmpF2B=_tmpF2A.tvs;
_tmpF2C=_tmpF2A.fields;_LL762: return 0;_LL75E:;}_LL75A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF1C=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpF0A;if(_tmpF1C->tag != 4)
goto _LL75C;else{_tmpF1D=_tmpF1C->f1;_tmpF1E=_tmpF1D.field_info;_tmpF1F=_tmpF1D.targs;}}
_LL75B: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF2F=_tmpF1E;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF30;struct _tuple3 _tmpF31;struct Cyc_Absyn_Datatypedecl*_tmpF32;struct Cyc_Absyn_Datatypefield*
_tmpF33;_LL764: if((_tmpF2F.UnknownDatatypefield).tag != 1)goto _LL766;_tmpF30=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF2F.UnknownDatatypefield).val;
_LL765: {const char*_tmp1629;void*_tmp1628;(_tmp1628=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1629="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1629,sizeof(char),46))),_tag_dyneither(_tmp1628,sizeof(void*),
0)));}_LL766: if((_tmpF2F.KnownDatatypefield).tag != 2)goto _LL763;_tmpF31=(struct
_tuple3)(_tmpF2F.KnownDatatypefield).val;_tmpF32=_tmpF31.f1;_tmpF33=_tmpF31.f2;
_LL767: {struct Cyc_List_List*_tmpF36=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF32->tvs,_tmpF1F);struct Cyc_List_List*_tmpF37=
_tmpF33->typs;while(_tmpF37 != 0){_tmpF09=Cyc_Tcutil_rsubstitute(rgn,_tmpF36,(*((
struct _tuple11*)_tmpF37->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpF09))return 1;_tmpF37=_tmpF37->tl;}return 0;}_LL763:;}_LL75C:;_LL75D: return 0;
_LL74F:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF38=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF3A;union Cyc_Absyn_AggrInfoU _tmpF3B;struct Cyc_Absyn_Aggrdecl**_tmpF3C;struct
Cyc_Absyn_Aggrdecl*_tmpF3D;struct Cyc_List_List*_tmpF3E;struct Cyc_List_List*
_tmpF40;_LL769: {struct Cyc_Absyn_AggrType_struct*_tmpF39=(struct Cyc_Absyn_AggrType_struct*)
_tmpF38;if(_tmpF39->tag != 12)goto _LL76B;else{_tmpF3A=_tmpF39->f1;_tmpF3B=_tmpF3A.aggr_info;
if((_tmpF3B.KnownAggr).tag != 2)goto _LL76B;_tmpF3C=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF3B.KnownAggr).val;_tmpF3D=*_tmpF3C;_tmpF3E=_tmpF3A.targs;}}_LL76A: _tmpF40=
_tmpF3D->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF3D->impl))->fields;
goto _LL76C;_LL76B: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF3F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF38;if(_tmpF3F->tag != 13)goto _LL76D;else{_tmpF40=_tmpF3F->f2;}}_LL76C: {
struct Cyc_Absyn_Aggrfield*_tmpF41=Cyc_Absyn_lookup_field(_tmpF40,f);{struct Cyc_Absyn_Aggrfield*
_tmpF42=_tmpF41;struct Cyc_Absyn_Aggrfield _tmpF43;void*_tmpF44;_LL770: if(_tmpF42
!= 0)goto _LL772;_LL771: {const char*_tmp162C;void*_tmp162B;(_tmp162B=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp162C="is_noalias_field: missing field",
_tag_dyneither(_tmp162C,sizeof(char),32))),_tag_dyneither(_tmp162B,sizeof(void*),
0)));}_LL772: if(_tmpF42 == 0)goto _LL76F;_tmpF43=*_tmpF42;_tmpF44=_tmpF43.type;
_LL773: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF44);_LL76F:;}return 0;}
_LL76D:;_LL76E: {const char*_tmp1630;void*_tmp162F[1];struct Cyc_String_pa_struct
_tmp162E;(_tmp162E.tag=0,((_tmp162E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp162F[0]=& _tmp162E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1630="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1630,sizeof(char),36))),_tag_dyneither(_tmp162F,sizeof(void*),
1)))))));}_LL768:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF4A=e->r;void*_tmpF4C;struct Cyc_Absyn_Exp*
_tmpF4F;struct Cyc_Absyn_Exp*_tmpF51;struct Cyc_Absyn_Exp*_tmpF53;struct
_dyneither_ptr*_tmpF54;struct Cyc_Absyn_Exp*_tmpF56;struct Cyc_Absyn_Exp*_tmpF57;
struct Cyc_Absyn_Exp*_tmpF59;struct Cyc_Absyn_Exp*_tmpF5A;struct Cyc_Absyn_Exp*
_tmpF5C;struct Cyc_Absyn_Exp*_tmpF5E;struct Cyc_Absyn_Stmt*_tmpF60;_LL775: {struct
Cyc_Absyn_Var_e_struct*_tmpF4B=(struct Cyc_Absyn_Var_e_struct*)_tmpF4A;if(_tmpF4B->tag
!= 1)goto _LL777;else{_tmpF4C=(void*)_tmpF4B->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF4D=(struct Cyc_Absyn_Global_b_struct*)_tmpF4C;if(_tmpF4D->tag != 1)goto _LL777;};}}
_LL776: return 0;_LL777: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF4E=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF4A;if(_tmpF4E->tag != 23)goto _LL779;else{_tmpF4F=_tmpF4E->f1;}}_LL778:
_tmpF51=_tmpF4F;goto _LL77A;_LL779: {struct Cyc_Absyn_Deref_e_struct*_tmpF50=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF4A;if(_tmpF50->tag != 21)goto _LL77B;else{
_tmpF51=_tmpF50->f1;}}_LL77A: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF51->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF51);_LL77B: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF52=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF4A;if(_tmpF52->tag != 22)goto _LL77D;else{_tmpF53=_tmpF52->f1;_tmpF54=_tmpF52->f2;}}
_LL77C: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF53);_LL77D: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF55=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF4A;if(_tmpF55->tag != 24)goto
_LL77F;else{_tmpF56=_tmpF55->f1;_tmpF57=_tmpF55->f2;}}_LL77E: {void*_tmpF61=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF56->topt))->v);_LL78A: {struct Cyc_Absyn_TupleType_struct*
_tmpF62=(struct Cyc_Absyn_TupleType_struct*)_tmpF61;if(_tmpF62->tag != 11)goto
_LL78C;}_LL78B: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF56);_LL78C:;_LL78D:
return 0;_LL789:;}_LL77F: {struct Cyc_Absyn_Conditional_e_struct*_tmpF58=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF4A;if(_tmpF58->tag != 6)goto _LL781;else{
_tmpF59=_tmpF58->f2;_tmpF5A=_tmpF58->f3;}}_LL780: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF59) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF5A);_LL781: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF5B=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF4A;if(_tmpF5B->tag != 9)goto _LL783;
else{_tmpF5C=_tmpF5B->f2;}}_LL782: _tmpF5E=_tmpF5C;goto _LL784;_LL783: {struct Cyc_Absyn_Cast_e_struct*
_tmpF5D=(struct Cyc_Absyn_Cast_e_struct*)_tmpF4A;if(_tmpF5D->tag != 15)goto _LL785;
else{_tmpF5E=_tmpF5D->f2;}}_LL784: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF5E);
_LL785: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF5F=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF4A;if(_tmpF5F->tag != 37)goto _LL787;else{_tmpF60=_tmpF5F->f1;}}_LL786: while(
1){void*_tmpF63=_tmpF60->r;struct Cyc_Absyn_Stmt*_tmpF65;struct Cyc_Absyn_Stmt*
_tmpF66;struct Cyc_Absyn_Decl*_tmpF68;struct Cyc_Absyn_Stmt*_tmpF69;struct Cyc_Absyn_Exp*
_tmpF6B;_LL78F: {struct Cyc_Absyn_Seq_s_struct*_tmpF64=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF63;if(_tmpF64->tag != 2)goto _LL791;else{_tmpF65=_tmpF64->f1;_tmpF66=_tmpF64->f2;}}
_LL790: _tmpF60=_tmpF66;goto _LL78E;_LL791: {struct Cyc_Absyn_Decl_s_struct*_tmpF67=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF63;if(_tmpF67->tag != 12)goto _LL793;else{
_tmpF68=_tmpF67->f1;_tmpF69=_tmpF67->f2;}}_LL792: _tmpF60=_tmpF69;goto _LL78E;
_LL793: {struct Cyc_Absyn_Exp_s_struct*_tmpF6A=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF63;if(_tmpF6A->tag != 1)goto _LL795;else{_tmpF6B=_tmpF6A->f1;}}_LL794: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF6B);_LL795:;_LL796: {const char*_tmp1633;
void*_tmp1632;(_tmp1632=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1633="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1633,sizeof(char),40))),_tag_dyneither(_tmp1632,sizeof(void*),
0)));}_LL78E:;}_LL787:;_LL788: return 1;_LL774:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple17 _tmp1634;struct _tuple17 bogus_ans=(_tmp1634.f1=0,((_tmp1634.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1634)));void*_tmpF6E=e->r;struct _tuple2*_tmpF70;
void*_tmpF71;struct Cyc_Absyn_Exp*_tmpF73;struct _dyneither_ptr*_tmpF74;int _tmpF75;
struct Cyc_Absyn_Exp*_tmpF77;struct _dyneither_ptr*_tmpF78;int _tmpF79;struct Cyc_Absyn_Exp*
_tmpF7B;struct Cyc_Absyn_Exp*_tmpF7D;struct Cyc_Absyn_Exp*_tmpF7E;_LL798: {struct
Cyc_Absyn_Var_e_struct*_tmpF6F=(struct Cyc_Absyn_Var_e_struct*)_tmpF6E;if(_tmpF6F->tag
!= 1)goto _LL79A;else{_tmpF70=_tmpF6F->f1;_tmpF71=(void*)_tmpF6F->f2;}}_LL799: {
void*_tmpF7F=_tmpF71;struct Cyc_Absyn_Vardecl*_tmpF83;struct Cyc_Absyn_Vardecl*
_tmpF85;struct Cyc_Absyn_Vardecl*_tmpF87;struct Cyc_Absyn_Vardecl*_tmpF89;_LL7A5: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF80=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF7F;if(_tmpF80->tag != 0)goto _LL7A7;}_LL7A6: goto _LL7A8;_LL7A7: {struct Cyc_Absyn_Funname_b_struct*
_tmpF81=(struct Cyc_Absyn_Funname_b_struct*)_tmpF7F;if(_tmpF81->tag != 2)goto
_LL7A9;}_LL7A8: return bogus_ans;_LL7A9: {struct Cyc_Absyn_Global_b_struct*_tmpF82=(
struct Cyc_Absyn_Global_b_struct*)_tmpF7F;if(_tmpF82->tag != 1)goto _LL7AB;else{
_tmpF83=_tmpF82->f1;}}_LL7AA: {void*_tmpF8A=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL7B2: {struct Cyc_Absyn_ArrayType_struct*
_tmpF8B=(struct Cyc_Absyn_ArrayType_struct*)_tmpF8A;if(_tmpF8B->tag != 9)goto
_LL7B4;}_LL7B3: {struct _tuple17 _tmp1635;return(_tmp1635.f1=1,((_tmp1635.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp1635)));}_LL7B4:;_LL7B5: {struct _tuple17 _tmp1636;
return(_tmp1636.f1=(_tmpF83->tq).real_const,((_tmp1636.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1636)));}_LL7B1:;}_LL7AB: {struct Cyc_Absyn_Local_b_struct*_tmpF84=(struct Cyc_Absyn_Local_b_struct*)
_tmpF7F;if(_tmpF84->tag != 4)goto _LL7AD;else{_tmpF85=_tmpF84->f1;}}_LL7AC: {void*
_tmpF8E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL7B7: {struct Cyc_Absyn_ArrayType_struct*_tmpF8F=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF8E;if(_tmpF8F->tag != 9)goto _LL7B9;}_LL7B8: {struct _tuple17 _tmp1637;return(
_tmp1637.f1=1,((_tmp1637.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF85->rgn))->v,
_tmp1637)));}_LL7B9:;_LL7BA: _tmpF85->escapes=1;{struct _tuple17 _tmp1638;return(
_tmp1638.f1=(_tmpF85->tq).real_const,((_tmp1638.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF85->rgn))->v,_tmp1638)));};_LL7B6:;}_LL7AD: {struct Cyc_Absyn_Pat_b_struct*
_tmpF86=(struct Cyc_Absyn_Pat_b_struct*)_tmpF7F;if(_tmpF86->tag != 5)goto _LL7AF;
else{_tmpF87=_tmpF86->f1;}}_LL7AE: _tmpF89=_tmpF87;goto _LL7B0;_LL7AF: {struct Cyc_Absyn_Param_b_struct*
_tmpF88=(struct Cyc_Absyn_Param_b_struct*)_tmpF7F;if(_tmpF88->tag != 3)goto _LL7A4;
else{_tmpF89=_tmpF88->f1;}}_LL7B0: _tmpF89->escapes=1;{struct _tuple17 _tmp1639;
return(_tmp1639.f1=(_tmpF89->tq).real_const,((_tmp1639.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF89->rgn))->v,_tmp1639)));};_LL7A4:;}_LL79A: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF72=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF6E;if(_tmpF72->tag != 22)goto
_LL79C;else{_tmpF73=_tmpF72->f1;_tmpF74=_tmpF72->f2;_tmpF75=_tmpF72->f3;}}_LL79B:
if(_tmpF75)return bogus_ans;{void*_tmpF93=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF73->topt))->v);struct Cyc_List_List*_tmpF95;struct Cyc_Absyn_AggrInfo
_tmpF97;union Cyc_Absyn_AggrInfoU _tmpF98;struct Cyc_Absyn_Aggrdecl**_tmpF99;struct
Cyc_Absyn_Aggrdecl*_tmpF9A;_LL7BC: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF94=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF93;if(_tmpF94->tag != 13)goto _LL7BE;
else{_tmpF95=_tmpF94->f2;}}_LL7BD: {struct Cyc_Absyn_Aggrfield*_tmpF9B=Cyc_Absyn_lookup_field(
_tmpF95,_tmpF74);if(_tmpF9B != 0  && _tmpF9B->width == 0){struct _tuple17 _tmp163A;
return(_tmp163A.f1=(_tmpF9B->tq).real_const,((_tmp163A.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF73)).f2,_tmp163A)));}return bogus_ans;}_LL7BE: {struct Cyc_Absyn_AggrType_struct*
_tmpF96=(struct Cyc_Absyn_AggrType_struct*)_tmpF93;if(_tmpF96->tag != 12)goto
_LL7C0;else{_tmpF97=_tmpF96->f1;_tmpF98=_tmpF97.aggr_info;if((_tmpF98.KnownAggr).tag
!= 2)goto _LL7C0;_tmpF99=(struct Cyc_Absyn_Aggrdecl**)(_tmpF98.KnownAggr).val;
_tmpF9A=*_tmpF99;}}_LL7BF: {struct Cyc_Absyn_Aggrfield*_tmpF9D=Cyc_Absyn_lookup_decl_field(
_tmpF9A,_tmpF74);if(_tmpF9D != 0  && _tmpF9D->width == 0){struct _tuple17 _tmp163B;
return(_tmp163B.f1=(_tmpF9D->tq).real_const,((_tmp163B.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF73)).f2,_tmp163B)));}return bogus_ans;}_LL7C0:;_LL7C1: return bogus_ans;
_LL7BB:;};_LL79C: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF76=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF6E;if(_tmpF76->tag != 23)goto _LL79E;else{_tmpF77=_tmpF76->f1;_tmpF78=_tmpF76->f2;
_tmpF79=_tmpF76->f3;}}_LL79D: if(_tmpF79)return bogus_ans;{void*_tmpF9F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF77->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpFA1;void*_tmpFA2;struct Cyc_Absyn_PtrAtts _tmpFA3;void*_tmpFA4;_LL7C3: {struct
Cyc_Absyn_PointerType_struct*_tmpFA0=(struct Cyc_Absyn_PointerType_struct*)
_tmpF9F;if(_tmpFA0->tag != 5)goto _LL7C5;else{_tmpFA1=_tmpFA0->f1;_tmpFA2=_tmpFA1.elt_typ;
_tmpFA3=_tmpFA1.ptr_atts;_tmpFA4=_tmpFA3.rgn;}}_LL7C4: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpFA5=Cyc_Tcutil_compress(_tmpFA2);struct Cyc_List_List*_tmpFA7;
struct Cyc_Absyn_AggrInfo _tmpFA9;union Cyc_Absyn_AggrInfoU _tmpFAA;struct Cyc_Absyn_Aggrdecl**
_tmpFAB;struct Cyc_Absyn_Aggrdecl*_tmpFAC;_LL7C8: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpFA6=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFA5;if(_tmpFA6->tag != 13)goto
_LL7CA;else{_tmpFA7=_tmpFA6->f2;}}_LL7C9: finfo=Cyc_Absyn_lookup_field(_tmpFA7,
_tmpF78);goto _LL7C7;_LL7CA: {struct Cyc_Absyn_AggrType_struct*_tmpFA8=(struct Cyc_Absyn_AggrType_struct*)
_tmpFA5;if(_tmpFA8->tag != 12)goto _LL7CC;else{_tmpFA9=_tmpFA8->f1;_tmpFAA=_tmpFA9.aggr_info;
if((_tmpFAA.KnownAggr).tag != 2)goto _LL7CC;_tmpFAB=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFAA.KnownAggr).val;_tmpFAC=*_tmpFAB;}}_LL7CB: finfo=Cyc_Absyn_lookup_decl_field(
_tmpFAC,_tmpF78);goto _LL7C7;_LL7CC:;_LL7CD: return bogus_ans;_LL7C7:;}if(finfo != 0
 && finfo->width == 0){struct _tuple17 _tmp163C;return(_tmp163C.f1=(finfo->tq).real_const,((
_tmp163C.f2=_tmpFA4,_tmp163C)));}return bogus_ans;}_LL7C5:;_LL7C6: return bogus_ans;
_LL7C2:;};_LL79E: {struct Cyc_Absyn_Deref_e_struct*_tmpF7A=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF6E;if(_tmpF7A->tag != 21)goto _LL7A0;else{_tmpF7B=_tmpF7A->f1;}}_LL79F: {void*
_tmpFAE=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF7B->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpFB0;struct Cyc_Absyn_Tqual _tmpFB1;struct Cyc_Absyn_PtrAtts
_tmpFB2;void*_tmpFB3;_LL7CF: {struct Cyc_Absyn_PointerType_struct*_tmpFAF=(struct
Cyc_Absyn_PointerType_struct*)_tmpFAE;if(_tmpFAF->tag != 5)goto _LL7D1;else{
_tmpFB0=_tmpFAF->f1;_tmpFB1=_tmpFB0.elt_tq;_tmpFB2=_tmpFB0.ptr_atts;_tmpFB3=
_tmpFB2.rgn;}}_LL7D0: {struct _tuple17 _tmp163D;return(_tmp163D.f1=_tmpFB1.real_const,((
_tmp163D.f2=_tmpFB3,_tmp163D)));}_LL7D1:;_LL7D2: return bogus_ans;_LL7CE:;}_LL7A0: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF7C=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF6E;if(_tmpF7C->tag != 24)goto _LL7A2;else{_tmpF7D=_tmpF7C->f1;_tmpF7E=_tmpF7C->f2;}}
_LL7A1: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF7D->topt))->v);void*_tmpFB5=t;struct Cyc_List_List*_tmpFB7;struct Cyc_Absyn_PtrInfo
_tmpFB9;struct Cyc_Absyn_Tqual _tmpFBA;struct Cyc_Absyn_PtrAtts _tmpFBB;void*_tmpFBC;
struct Cyc_Absyn_ArrayInfo _tmpFBE;struct Cyc_Absyn_Tqual _tmpFBF;_LL7D4: {struct Cyc_Absyn_TupleType_struct*
_tmpFB6=(struct Cyc_Absyn_TupleType_struct*)_tmpFB5;if(_tmpFB6->tag != 11)goto
_LL7D6;else{_tmpFB7=_tmpFB6->f1;}}_LL7D5: {unsigned int _tmpFC1;int _tmpFC2;struct
_tuple13 _tmpFC0=Cyc_Evexp_eval_const_uint_exp(_tmpF7E);_tmpFC1=_tmpFC0.f1;
_tmpFC2=_tmpFC0.f2;if(!_tmpFC2)return bogus_ans;{struct _tuple11*_tmpFC3=Cyc_Absyn_lookup_tuple_field(
_tmpFB7,(int)_tmpFC1);if(_tmpFC3 != 0){struct _tuple17 _tmp163E;return(_tmp163E.f1=((*
_tmpFC3).f1).real_const,((_tmp163E.f2=(Cyc_Tcutil_addressof_props(te,_tmpF7D)).f2,
_tmp163E)));}return bogus_ans;};}_LL7D6: {struct Cyc_Absyn_PointerType_struct*
_tmpFB8=(struct Cyc_Absyn_PointerType_struct*)_tmpFB5;if(_tmpFB8->tag != 5)goto
_LL7D8;else{_tmpFB9=_tmpFB8->f1;_tmpFBA=_tmpFB9.elt_tq;_tmpFBB=_tmpFB9.ptr_atts;
_tmpFBC=_tmpFBB.rgn;}}_LL7D7: {struct _tuple17 _tmp163F;return(_tmp163F.f1=_tmpFBA.real_const,((
_tmp163F.f2=_tmpFBC,_tmp163F)));}_LL7D8: {struct Cyc_Absyn_ArrayType_struct*
_tmpFBD=(struct Cyc_Absyn_ArrayType_struct*)_tmpFB5;if(_tmpFBD->tag != 9)goto
_LL7DA;else{_tmpFBE=_tmpFBD->f1;_tmpFBF=_tmpFBE.tq;}}_LL7D9: {struct _tuple17
_tmp1640;return(_tmp1640.f1=_tmpFBF.real_const,((_tmp1640.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF7D)).f2,_tmp1640)));}_LL7DA:;_LL7DB: return bogus_ans;_LL7D3:;}_LL7A2:;
_LL7A3:{const char*_tmp1643;void*_tmp1642;(_tmp1642=0,Cyc_Tcutil_terr(e->loc,((
_tmp1643="unary & applied to non-lvalue",_tag_dyneither(_tmp1643,sizeof(char),30))),
_tag_dyneither(_tmp1642,sizeof(void*),0)));}return bogus_ans;_LL797:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFCA=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFCC;void*_tmpFCD;struct Cyc_Absyn_Tqual
_tmpFCE;union Cyc_Absyn_Constraint*_tmpFCF;_LL7DD: {struct Cyc_Absyn_ArrayType_struct*
_tmpFCB=(struct Cyc_Absyn_ArrayType_struct*)_tmpFCA;if(_tmpFCB->tag != 9)goto
_LL7DF;else{_tmpFCC=_tmpFCB->f1;_tmpFCD=_tmpFCC.elt_type;_tmpFCE=_tmpFCC.tq;
_tmpFCF=_tmpFCC.zero_term;}}_LL7DE: {void*_tmpFD1;struct _tuple17 _tmpFD0=Cyc_Tcutil_addressof_props(
te,e);_tmpFD1=_tmpFD0.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1646;struct Cyc_Absyn_Upper_b_struct*
_tmp1645;return Cyc_Absyn_atb_typ(_tmpFCD,_tmpFD1,_tmpFCE,(void*)((_tmp1645=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1645)),((_tmp1645[0]=((
_tmp1646.tag=1,((_tmp1646.f1=e,_tmp1646)))),_tmp1645)))),_tmpFCF);};}_LL7DF:;
_LL7E0: return e_typ;_LL7DC:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*
loc,unsigned int i,union Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{void*_tmpFD4=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
b);struct Cyc_Absyn_Exp*_tmpFD7;_LL7E2: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFD5=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFD4;if(_tmpFD5->tag != 0)goto
_LL7E4;}_LL7E3: return;_LL7E4: {struct Cyc_Absyn_Upper_b_struct*_tmpFD6=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFD4;if(_tmpFD6->tag != 1)goto _LL7E1;else{_tmpFD7=_tmpFD6->f1;}}_LL7E5: {
unsigned int _tmpFD9;int _tmpFDA;struct _tuple13 _tmpFD8=Cyc_Evexp_eval_const_uint_exp(
_tmpFD7);_tmpFD9=_tmpFD8.f1;_tmpFDA=_tmpFD8.f2;if(_tmpFDA  && _tmpFD9 <= i){const
char*_tmp164B;void*_tmp164A[2];struct Cyc_Int_pa_struct _tmp1649;struct Cyc_Int_pa_struct
_tmp1648;(_tmp1648.tag=1,((_tmp1648.f1=(unsigned long)((int)i),((_tmp1649.tag=1,((
_tmp1649.f1=(unsigned long)((int)_tmpFD9),((_tmp164A[0]=& _tmp1649,((_tmp164A[1]=&
_tmp1648,Cyc_Tcutil_terr(loc,((_tmp164B="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp164B,sizeof(char),39))),_tag_dyneither(_tmp164A,sizeof(void*),
2)))))))))))));}return;}_LL7E1:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFDF=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFE1;_LL7E7: {struct Cyc_Absyn_Upper_b_struct*_tmpFE0=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFDF;if(_tmpFE0->tag != 1)goto _LL7E9;else{_tmpFE1=_tmpFE0->f1;}}_LL7E8: {
unsigned int _tmpFE3;int _tmpFE4;struct _tuple13 _tmpFE2=Cyc_Evexp_eval_const_uint_exp(
_tmpFE1);_tmpFE3=_tmpFE2.f1;_tmpFE4=_tmpFE2.f2;return _tmpFE4  && _tmpFE3 == 1;}
_LL7E9:;_LL7EA: return 0;_LL7E6:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFE5=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFED;void*_tmpFEE;union Cyc_Absyn_Constraint*
_tmpFEF;struct Cyc_List_List*_tmpFF1;struct Cyc_Absyn_AggrInfo _tmpFF3;union Cyc_Absyn_AggrInfoU
_tmpFF4;struct _tuple4 _tmpFF5;struct Cyc_Absyn_AggrInfo _tmpFF7;union Cyc_Absyn_AggrInfoU
_tmpFF8;struct Cyc_Absyn_Aggrdecl**_tmpFF9;struct Cyc_Absyn_Aggrdecl*_tmpFFA;
struct Cyc_List_List*_tmpFFB;struct Cyc_List_List*_tmpFFD;_LL7EC: {struct Cyc_Absyn_VoidType_struct*
_tmpFE6=(struct Cyc_Absyn_VoidType_struct*)_tmpFE5;if(_tmpFE6->tag != 0)goto _LL7EE;}
_LL7ED: goto _LL7EF;_LL7EE: {struct Cyc_Absyn_IntType_struct*_tmpFE7=(struct Cyc_Absyn_IntType_struct*)
_tmpFE5;if(_tmpFE7->tag != 6)goto _LL7F0;}_LL7EF: goto _LL7F1;_LL7F0: {struct Cyc_Absyn_FloatType_struct*
_tmpFE8=(struct Cyc_Absyn_FloatType_struct*)_tmpFE5;if(_tmpFE8->tag != 7)goto
_LL7F2;}_LL7F1: goto _LL7F3;_LL7F2: {struct Cyc_Absyn_DoubleType_struct*_tmpFE9=(
struct Cyc_Absyn_DoubleType_struct*)_tmpFE5;if(_tmpFE9->tag != 8)goto _LL7F4;}
_LL7F3: return 1;_LL7F4: {struct Cyc_Absyn_EnumType_struct*_tmpFEA=(struct Cyc_Absyn_EnumType_struct*)
_tmpFE5;if(_tmpFEA->tag != 14)goto _LL7F6;}_LL7F5: goto _LL7F7;_LL7F6: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpFEB=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpFE5;if(_tmpFEB->tag != 15)goto
_LL7F8;}_LL7F7: return 0;_LL7F8: {struct Cyc_Absyn_ArrayType_struct*_tmpFEC=(struct
Cyc_Absyn_ArrayType_struct*)_tmpFE5;if(_tmpFEC->tag != 9)goto _LL7FA;else{_tmpFED=
_tmpFEC->f1;_tmpFEE=_tmpFED.elt_type;_tmpFEF=_tmpFED.zero_term;}}_LL7F9: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFEF) && Cyc_Tcutil_bits_only(
_tmpFEE);_LL7FA: {struct Cyc_Absyn_TupleType_struct*_tmpFF0=(struct Cyc_Absyn_TupleType_struct*)
_tmpFE5;if(_tmpFF0->tag != 11)goto _LL7FC;else{_tmpFF1=_tmpFF0->f1;}}_LL7FB: for(0;
_tmpFF1 != 0;_tmpFF1=_tmpFF1->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpFF1->hd)).f2))return 0;}return 1;_LL7FC: {struct Cyc_Absyn_AggrType_struct*
_tmpFF2=(struct Cyc_Absyn_AggrType_struct*)_tmpFE5;if(_tmpFF2->tag != 12)goto
_LL7FE;else{_tmpFF3=_tmpFF2->f1;_tmpFF4=_tmpFF3.aggr_info;if((_tmpFF4.UnknownAggr).tag
!= 1)goto _LL7FE;_tmpFF5=(struct _tuple4)(_tmpFF4.UnknownAggr).val;}}_LL7FD: return
0;_LL7FE: {struct Cyc_Absyn_AggrType_struct*_tmpFF6=(struct Cyc_Absyn_AggrType_struct*)
_tmpFE5;if(_tmpFF6->tag != 12)goto _LL800;else{_tmpFF7=_tmpFF6->f1;_tmpFF8=_tmpFF7.aggr_info;
if((_tmpFF8.KnownAggr).tag != 2)goto _LL800;_tmpFF9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFF8.KnownAggr).val;_tmpFFA=*_tmpFF9;_tmpFFB=_tmpFF7.targs;}}_LL7FF: if(
_tmpFFA->impl == 0)return 0;{struct _RegionHandle _tmpFFE=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFFE;_push_region(rgn);{struct Cyc_List_List*_tmpFFF=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFFA->tvs,_tmpFFB);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFFA->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFFF,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp1000=0;_npop_handler(0);
return _tmp1000;}}}{int _tmp1001=1;_npop_handler(0);return _tmp1001;};};_pop_region(
rgn);};_LL800: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFFC=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFE5;if(_tmpFFC->tag != 13)goto _LL802;else{_tmpFFD=_tmpFFC->f2;}}_LL801: for(0;
_tmpFFD != 0;_tmpFFD=_tmpFFD->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFFD->hd)->type))return 0;}return 1;_LL802:;_LL803: return 0;_LL7EB:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp1002=e->r;struct _tuple2*_tmp100A;void*_tmp100B;struct Cyc_Absyn_Exp*
_tmp100D;struct Cyc_Absyn_Exp*_tmp100E;struct Cyc_Absyn_Exp*_tmp100F;struct Cyc_Absyn_Exp*
_tmp1011;struct Cyc_Absyn_Exp*_tmp1012;struct Cyc_Absyn_Exp*_tmp1014;struct Cyc_Absyn_Exp*
_tmp1016;void*_tmp1018;struct Cyc_Absyn_Exp*_tmp1019;enum Cyc_Absyn_Coercion
_tmp101A;void*_tmp101C;struct Cyc_Absyn_Exp*_tmp101D;struct Cyc_Absyn_Exp*_tmp101F;
struct Cyc_Absyn_Exp*_tmp1021;struct Cyc_Absyn_Exp*_tmp1022;struct Cyc_List_List*
_tmp1024;struct Cyc_List_List*_tmp1026;struct Cyc_List_List*_tmp1028;enum Cyc_Absyn_Primop
_tmp102A;struct Cyc_List_List*_tmp102B;struct Cyc_List_List*_tmp102D;struct Cyc_List_List*
_tmp102F;_LL805: {struct Cyc_Absyn_Const_e_struct*_tmp1003=(struct Cyc_Absyn_Const_e_struct*)
_tmp1002;if(_tmp1003->tag != 0)goto _LL807;}_LL806: goto _LL808;_LL807: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp1004=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1002;if(_tmp1004->tag != 18)
goto _LL809;}_LL808: goto _LL80A;_LL809: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1005=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1002;if(_tmp1005->tag != 19)
goto _LL80B;}_LL80A: goto _LL80C;_LL80B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp1006=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp1002;if(_tmp1006->tag != 20)goto
_LL80D;}_LL80C: goto _LL80E;_LL80D: {struct Cyc_Absyn_Enum_e_struct*_tmp1007=(
struct Cyc_Absyn_Enum_e_struct*)_tmp1002;if(_tmp1007->tag != 32)goto _LL80F;}_LL80E:
goto _LL810;_LL80F: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1008=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp1002;if(_tmp1008->tag != 33)goto _LL811;}_LL810: return 1;_LL811: {struct Cyc_Absyn_Var_e_struct*
_tmp1009=(struct Cyc_Absyn_Var_e_struct*)_tmp1002;if(_tmp1009->tag != 1)goto _LL813;
else{_tmp100A=_tmp1009->f1;_tmp100B=(void*)_tmp1009->f2;}}_LL812: {void*_tmp1030=
_tmp100B;struct Cyc_Absyn_Vardecl*_tmp1033;_LL832: {struct Cyc_Absyn_Funname_b_struct*
_tmp1031=(struct Cyc_Absyn_Funname_b_struct*)_tmp1030;if(_tmp1031->tag != 2)goto
_LL834;}_LL833: return 1;_LL834: {struct Cyc_Absyn_Global_b_struct*_tmp1032=(struct
Cyc_Absyn_Global_b_struct*)_tmp1030;if(_tmp1032->tag != 1)goto _LL836;else{
_tmp1033=_tmp1032->f1;}}_LL835: {void*_tmp1035=Cyc_Tcutil_compress(_tmp1033->type);
_LL83B: {struct Cyc_Absyn_ArrayType_struct*_tmp1036=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1035;if(_tmp1036->tag != 9)goto _LL83D;}_LL83C: goto _LL83E;_LL83D: {struct Cyc_Absyn_FnType_struct*
_tmp1037=(struct Cyc_Absyn_FnType_struct*)_tmp1035;if(_tmp1037->tag != 10)goto
_LL83F;}_LL83E: return 1;_LL83F:;_LL840: return var_okay;_LL83A:;}_LL836: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp1034=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp1030;if(_tmp1034->tag != 0)
goto _LL838;}_LL837: return 0;_LL838:;_LL839: return var_okay;_LL831:;}_LL813: {
struct Cyc_Absyn_Conditional_e_struct*_tmp100C=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp1002;if(_tmp100C->tag != 6)goto _LL815;else{_tmp100D=_tmp100C->f1;_tmp100E=
_tmp100C->f2;_tmp100F=_tmp100C->f3;}}_LL814: return(Cyc_Tcutil_cnst_exp(te,0,
_tmp100D) && Cyc_Tcutil_cnst_exp(te,0,_tmp100E)) && Cyc_Tcutil_cnst_exp(te,0,
_tmp100F);_LL815: {struct Cyc_Absyn_SeqExp_e_struct*_tmp1010=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1002;if(_tmp1010->tag != 9)goto _LL817;else{_tmp1011=_tmp1010->f1;_tmp1012=
_tmp1010->f2;}}_LL816: return Cyc_Tcutil_cnst_exp(te,0,_tmp1011) && Cyc_Tcutil_cnst_exp(
te,0,_tmp1012);_LL817: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp1013=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp1002;if(_tmp1013->tag != 13)goto _LL819;
else{_tmp1014=_tmp1013->f1;}}_LL818: _tmp1016=_tmp1014;goto _LL81A;_LL819: {struct
Cyc_Absyn_Instantiate_e_struct*_tmp1015=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1002;if(_tmp1015->tag != 14)goto _LL81B;else{_tmp1016=_tmp1015->f1;}}_LL81A:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1016);_LL81B: {struct Cyc_Absyn_Cast_e_struct*
_tmp1017=(struct Cyc_Absyn_Cast_e_struct*)_tmp1002;if(_tmp1017->tag != 15)goto
_LL81D;else{_tmp1018=(void*)_tmp1017->f1;_tmp1019=_tmp1017->f2;_tmp101A=_tmp1017->f4;
if(_tmp101A != Cyc_Absyn_No_coercion)goto _LL81D;}}_LL81C: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp1019);_LL81D: {struct Cyc_Absyn_Cast_e_struct*_tmp101B=(struct Cyc_Absyn_Cast_e_struct*)
_tmp1002;if(_tmp101B->tag != 15)goto _LL81F;else{_tmp101C=(void*)_tmp101B->f1;
_tmp101D=_tmp101B->f2;}}_LL81E: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp101D);
_LL81F: {struct Cyc_Absyn_Address_e_struct*_tmp101E=(struct Cyc_Absyn_Address_e_struct*)
_tmp1002;if(_tmp101E->tag != 16)goto _LL821;else{_tmp101F=_tmp101E->f1;}}_LL820:
return Cyc_Tcutil_cnst_exp(te,1,_tmp101F);_LL821: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1020=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp1002;if(_tmp1020->tag != 28)
goto _LL823;else{_tmp1021=_tmp1020->f2;_tmp1022=_tmp1020->f3;}}_LL822: return Cyc_Tcutil_cnst_exp(
te,0,_tmp1021) && Cyc_Tcutil_cnst_exp(te,0,_tmp1022);_LL823: {struct Cyc_Absyn_Array_e_struct*
_tmp1023=(struct Cyc_Absyn_Array_e_struct*)_tmp1002;if(_tmp1023->tag != 27)goto
_LL825;else{_tmp1024=_tmp1023->f1;}}_LL824: _tmp1026=_tmp1024;goto _LL826;_LL825: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp1025=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1002;if(_tmp1025->tag != 30)goto _LL827;else{_tmp1026=_tmp1025->f2;}}_LL826:
_tmp1028=_tmp1026;goto _LL828;_LL827: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1027=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp1002;if(_tmp1027->tag != 29)
goto _LL829;else{_tmp1028=_tmp1027->f3;}}_LL828: for(0;_tmp1028 != 0;_tmp1028=
_tmp1028->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple18*)_tmp1028->hd)).f2))
return 0;}return 1;_LL829: {struct Cyc_Absyn_Primop_e_struct*_tmp1029=(struct Cyc_Absyn_Primop_e_struct*)
_tmp1002;if(_tmp1029->tag != 3)goto _LL82B;else{_tmp102A=_tmp1029->f1;_tmp102B=
_tmp1029->f2;}}_LL82A: _tmp102D=_tmp102B;goto _LL82C;_LL82B: {struct Cyc_Absyn_Tuple_e_struct*
_tmp102C=(struct Cyc_Absyn_Tuple_e_struct*)_tmp1002;if(_tmp102C->tag != 25)goto
_LL82D;else{_tmp102D=_tmp102C->f1;}}_LL82C: _tmp102F=_tmp102D;goto _LL82E;_LL82D: {
struct Cyc_Absyn_Datatype_e_struct*_tmp102E=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp1002;if(_tmp102E->tag != 31)goto _LL82F;else{_tmp102F=_tmp102E->f1;}}_LL82E:
for(0;_tmp102F != 0;_tmp102F=_tmp102F->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp102F->hd))return 0;}return 1;_LL82F:;_LL830: return 0;_LL804:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp1038=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp103E;
struct Cyc_Absyn_PtrAtts _tmp103F;union Cyc_Absyn_Constraint*_tmp1040;union Cyc_Absyn_Constraint*
_tmp1041;struct Cyc_Absyn_ArrayInfo _tmp1043;void*_tmp1044;struct Cyc_List_List*
_tmp1046;struct Cyc_Absyn_AggrInfo _tmp1048;union Cyc_Absyn_AggrInfoU _tmp1049;
struct Cyc_List_List*_tmp104A;struct Cyc_List_List*_tmp104C;_LL842: {struct Cyc_Absyn_VoidType_struct*
_tmp1039=(struct Cyc_Absyn_VoidType_struct*)_tmp1038;if(_tmp1039->tag != 0)goto
_LL844;}_LL843: goto _LL845;_LL844: {struct Cyc_Absyn_IntType_struct*_tmp103A=(
struct Cyc_Absyn_IntType_struct*)_tmp1038;if(_tmp103A->tag != 6)goto _LL846;}_LL845:
goto _LL847;_LL846: {struct Cyc_Absyn_FloatType_struct*_tmp103B=(struct Cyc_Absyn_FloatType_struct*)
_tmp1038;if(_tmp103B->tag != 7)goto _LL848;}_LL847: goto _LL849;_LL848: {struct Cyc_Absyn_DoubleType_struct*
_tmp103C=(struct Cyc_Absyn_DoubleType_struct*)_tmp1038;if(_tmp103C->tag != 8)goto
_LL84A;}_LL849: return 1;_LL84A: {struct Cyc_Absyn_PointerType_struct*_tmp103D=(
struct Cyc_Absyn_PointerType_struct*)_tmp1038;if(_tmp103D->tag != 5)goto _LL84C;
else{_tmp103E=_tmp103D->f1;_tmp103F=_tmp103E.ptr_atts;_tmp1040=_tmp103F.nullable;
_tmp1041=_tmp103F.bounds;}}_LL84B: {void*_tmp104F=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1041);_LL85B: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1050=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp104F;if(_tmp1050->tag != 0)goto _LL85D;}_LL85C: return 1;_LL85D:;_LL85E: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp1040);
_LL85A:;}_LL84C: {struct Cyc_Absyn_ArrayType_struct*_tmp1042=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1038;if(_tmp1042->tag != 9)goto _LL84E;else{_tmp1043=_tmp1042->f1;_tmp1044=
_tmp1043.elt_type;}}_LL84D: return Cyc_Tcutil_supports_default(_tmp1044);_LL84E: {
struct Cyc_Absyn_TupleType_struct*_tmp1045=(struct Cyc_Absyn_TupleType_struct*)
_tmp1038;if(_tmp1045->tag != 11)goto _LL850;else{_tmp1046=_tmp1045->f1;}}_LL84F:
for(0;_tmp1046 != 0;_tmp1046=_tmp1046->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple11*)_tmp1046->hd)).f2))return 0;}return 1;_LL850: {struct Cyc_Absyn_AggrType_struct*
_tmp1047=(struct Cyc_Absyn_AggrType_struct*)_tmp1038;if(_tmp1047->tag != 12)goto
_LL852;else{_tmp1048=_tmp1047->f1;_tmp1049=_tmp1048.aggr_info;_tmp104A=_tmp1048.targs;}}
_LL851: {struct Cyc_Absyn_Aggrdecl*_tmp1051=Cyc_Absyn_get_known_aggrdecl(_tmp1049);
if(_tmp1051->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1051->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp1051->tvs,_tmp104A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1051->impl))->fields);}
_LL852: {struct Cyc_Absyn_AnonAggrType_struct*_tmp104B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1038;if(_tmp104B->tag != 13)goto _LL854;else{_tmp104C=_tmp104B->f2;}}_LL853:
return Cyc_Tcutil_fields_support_default(0,0,_tmp104C);_LL854: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp104D=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1038;if(_tmp104D->tag != 15)
goto _LL856;}_LL855: goto _LL857;_LL856: {struct Cyc_Absyn_EnumType_struct*_tmp104E=(
struct Cyc_Absyn_EnumType_struct*)_tmp1038;if(_tmp104E->tag != 14)goto _LL858;}
_LL857: return 1;_LL858:;_LL859: return 0;_LL841:;}void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t){void*_tmp1052=t;struct Cyc_Absyn_Typedefdecl*
_tmp1054;_LL860: {struct Cyc_Absyn_TypedefType_struct*_tmp1053=(struct Cyc_Absyn_TypedefType_struct*)
_tmp1052;if(_tmp1053->tag != 18)goto _LL862;else{_tmp1054=_tmp1053->f3;}}_LL861:
if(_tmp1054 != 0){struct Cyc_Absyn_Tqual _tmp1055=_tmp1054->tq;if(((_tmp1055.print_const
 || _tmp1055.q_volatile) || _tmp1055.q_restrict) || _tmp1055.real_const){const
char*_tmp164F;void*_tmp164E[1];struct Cyc_String_pa_struct _tmp164D;(_tmp164D.tag=
0,((_tmp164D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp164E[0]=& _tmp164D,Cyc_Tcutil_warn(loc,((_tmp164F="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp164F,sizeof(char),44))),_tag_dyneither(_tmp164E,sizeof(void*),
1)))))));}}goto _LL85F;_LL862:;_LL863: goto _LL85F;_LL85F:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp1059=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1059;
_push_region(rgn);{struct Cyc_List_List*_tmp105A=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp105A,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp105B=0;_npop_handler(0);return _tmp105B;}}}{int _tmp105C=1;
_npop_handler(0);return _tmp105C;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp105D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1062;
struct Cyc_Absyn_PtrAtts _tmp1063;union Cyc_Absyn_Constraint*_tmp1064;union Cyc_Absyn_Constraint*
_tmp1065;_LL865: {struct Cyc_Absyn_IntType_struct*_tmp105E=(struct Cyc_Absyn_IntType_struct*)
_tmp105D;if(_tmp105E->tag != 6)goto _LL867;}_LL866: goto _LL868;_LL867: {struct Cyc_Absyn_FloatType_struct*
_tmp105F=(struct Cyc_Absyn_FloatType_struct*)_tmp105D;if(_tmp105F->tag != 7)goto
_LL869;}_LL868: goto _LL86A;_LL869: {struct Cyc_Absyn_DoubleType_struct*_tmp1060=(
struct Cyc_Absyn_DoubleType_struct*)_tmp105D;if(_tmp1060->tag != 8)goto _LL86B;}
_LL86A: return 1;_LL86B: {struct Cyc_Absyn_PointerType_struct*_tmp1061=(struct Cyc_Absyn_PointerType_struct*)
_tmp105D;if(_tmp1061->tag != 5)goto _LL86D;else{_tmp1062=_tmp1061->f1;_tmp1063=
_tmp1062.ptr_atts;_tmp1064=_tmp1063.nullable;_tmp1065=_tmp1063.bounds;}}_LL86C: {
void*_tmp1066=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp1065);_LL870: {struct Cyc_Absyn_DynEither_b_struct*
_tmp1067=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1066;if(_tmp1067->tag != 0)goto
_LL872;}_LL871: return 0;_LL872:;_LL873: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp1064);_LL86F:;}_LL86D:;_LL86E: return 0;_LL864:;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp1068=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp106A;void*_tmp106B;struct Cyc_Absyn_FnInfo _tmp106D;struct Cyc_List_List*
_tmp106E;_LL875: {struct Cyc_Absyn_PointerType_struct*_tmp1069=(struct Cyc_Absyn_PointerType_struct*)
_tmp1068;if(_tmp1069->tag != 5)goto _LL877;else{_tmp106A=_tmp1069->f1;_tmp106B=
_tmp106A.elt_typ;}}_LL876: return Cyc_Tcutil_is_noreturn(_tmp106B);_LL877: {struct
Cyc_Absyn_FnType_struct*_tmp106C=(struct Cyc_Absyn_FnType_struct*)_tmp1068;if(
_tmp106C->tag != 10)goto _LL879;else{_tmp106D=_tmp106C->f1;_tmp106E=_tmp106D.attributes;}}
_LL878: for(0;_tmp106E != 0;_tmp106E=_tmp106E->tl){void*_tmp106F=(void*)_tmp106E->hd;
_LL87C: {struct Cyc_Absyn_Noreturn_att_struct*_tmp1070=(struct Cyc_Absyn_Noreturn_att_struct*)
_tmp106F;if(_tmp1070->tag != 4)goto _LL87E;}_LL87D: return 1;_LL87E:;_LL87F:
continue;_LL87B:;}goto _LL874;_LL879:;_LL87A: goto _LL874;_LL874:;}return 0;}struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp1071=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp1073;struct Cyc_List_List*
_tmp1074;struct Cyc_List_List**_tmp1075;_LL881: {struct Cyc_Absyn_FnType_struct*
_tmp1072=(struct Cyc_Absyn_FnType_struct*)_tmp1071;if(_tmp1072->tag != 10)goto
_LL883;else{_tmp1073=_tmp1072->f1;_tmp1074=_tmp1073.attributes;_tmp1075=(struct
Cyc_List_List**)&(_tmp1072->f1).attributes;}}_LL882: {struct Cyc_List_List*
_tmp1076=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1650;*_tmp1075=((_tmp1650=_region_malloc(Cyc_Core_unique_region,
sizeof(*_tmp1650)),((_tmp1650->hd=(void*)atts->hd,((_tmp1650->tl=*_tmp1075,
_tmp1650))))));}else{struct Cyc_List_List*_tmp1651;_tmp1076=((_tmp1651=
_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp1651)),((_tmp1651->hd=(void*)
atts->hd,((_tmp1651->tl=_tmp1076,_tmp1651))))));}}return _tmp1076;}_LL883:;_LL884: {
const char*_tmp1654;void*_tmp1653;(_tmp1653=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1654="transfer_fn_type_atts",
_tag_dyneither(_tmp1654,sizeof(char),22))),_tag_dyneither(_tmp1653,sizeof(void*),
0)));}_LL880:;}
