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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};typedef void*Cyc_parg_t;struct _dyneither_ptr Cyc_aprintf(struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
typedef void*Cyc_sarg_t;int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{
void*v;};typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;
};void*Cyc_Core_fst(struct _tuple0*);int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
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
Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tl(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
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
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};typedef
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;typedef struct Cyc_Position_Segment*Cyc_Position_seg_t;
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);typedef struct
_dyneither_ptr*Cyc_Absyn_field_name_t;typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};typedef union Cyc_Absyn_Nmspace
Cyc_Absyn_nmspace_t;union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*
Cyc_Absyn_qvar_opt_t;typedef struct _tuple2*Cyc_Absyn_typedef_name_t;typedef struct
_tuple2*Cyc_Absyn_typedef_name_opt_t;typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;typedef struct Cyc_Absyn_PtrInfo
Cyc_Absyn_ptr_info_t;typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;typedef struct Cyc_Absyn_DatatypeInfo
Cyc_Absyn_datatype_info_t;typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;typedef struct Cyc_Absyn_ArrayInfo
Cyc_Absyn_array_info_t;typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;typedef struct Cyc_Absyn_VarargCallInfo
Cyc_Absyn_vararg_call_info_t;typedef void*Cyc_Absyn_raw_exp_t;typedef struct Cyc_Absyn_Exp*
Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;typedef void*Cyc_Absyn_raw_stmt_t;
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
No_constr;};typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_struct{
int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int
identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{int tag;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;
struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};typedef void*Cyc_Absyn_pointer_qual_t;typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;
struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*
type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct Cyc_Core_Opt*
effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct
_tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int
tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
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
f1;};typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;
struct Cyc_Position_Segment*loc;};typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;
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
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};typedef void*Cyc_Absyn_type_modifier_t;
struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;
char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct
_union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};
struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct
_union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;
struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct
_dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct
_union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};extern union
Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple2*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple10*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
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
_tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_struct{
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
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_struct{
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
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct
_tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;typedef struct Cyc_PP_Out*
Cyc_PP_out_t;struct Cyc_PP_Doc;typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{
int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};typedef struct Cyc_Iter_Iter
Cyc_Iter_iter_t;int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};typedef
struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;typedef struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_rgn_po_t;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
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
_dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
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
struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple15
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
_tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple16 Cyc_Tcutil_swap_kind(
void*t,void*kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct
Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple16*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct
_tuple17*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
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
void*t,struct Cyc_Absyn_Exp*e);struct _tuple18{int f1;void*f2;};struct _tuple18 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_normalize_effect(
void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_snd_tqt(struct _tuple12*);int Cyc_Tcutil_supports_default(
void*);int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void Cyc_Tcutil_check_no_qual(struct Cyc_Position_Segment*loc,void*t);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{
struct _tuple0*eq;int isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Datatypedecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);
extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,
Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_struct{
char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{const char*_tmp1076;void*_tmp1075[2];const char*
_tmp1074;const char*_tmp1073;struct Cyc_String_pa_struct _tmp1072;struct Cyc_String_pa_struct
_tmp1071;struct _dyneither_ptr s1=(struct _dyneither_ptr)((_tmp1071.tag=0,((
_tmp1071.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmp1072.tag=0,((_tmp1072.f1=(struct _dyneither_ptr)(
Cyc_Tcutil_tq1_const?(_tmp1073="const ",_tag_dyneither(_tmp1073,sizeof(char),7)):((
_tmp1074="",_tag_dyneither(_tmp1074,sizeof(char),1)))),((_tmp1075[0]=& _tmp1072,((
_tmp1075[1]=& _tmp1071,Cyc_aprintf(((_tmp1076="%s%s",_tag_dyneither(_tmp1076,
sizeof(char),5))),_tag_dyneither(_tmp1075,sizeof(void*),2))))))))))))));const
char*_tmp107F;void*_tmp107E[2];const char*_tmp107D;const char*_tmp107C;struct Cyc_String_pa_struct
_tmp107B;struct Cyc_String_pa_struct _tmp107A;struct _dyneither_ptr s2=(struct
_dyneither_ptr)((_tmp107A.tag=0,((_tmp107A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure)),((_tmp107B.tag=0,((
_tmp107B.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(_tmp107C="const ",
_tag_dyneither(_tmp107C,sizeof(char),7)):((_tmp107D="",_tag_dyneither(_tmp107D,
sizeof(char),1)))),((_tmp107E[0]=& _tmp107B,((_tmp107E[1]=& _tmp107A,Cyc_aprintf(((
_tmp107F="%s%s",_tag_dyneither(_tmp107F,sizeof(char),5))),_tag_dyneither(
_tmp107E,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmp1083;void*
_tmp1082[1];struct Cyc_String_pa_struct _tmp1081;(_tmp1081.tag=0,((_tmp1081.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp1082[0]=& _tmp1081,Cyc_fprintf(
Cyc_stderr,((_tmp1083="  %s",_tag_dyneither(_tmp1083,sizeof(char),5))),
_tag_dyneither(_tmp1082,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmp1086;void*_tmp1085;(_tmp1085=0,Cyc_fprintf(
Cyc_stderr,((_tmp1086="\n\t",_tag_dyneither(_tmp1086,sizeof(char),3))),
_tag_dyneither(_tmp1085,sizeof(void*),0)));}pos=8;}else{{const char*_tmp1089;void*
_tmp1088;(_tmp1088=0,Cyc_fprintf(Cyc_stderr,((_tmp1089=" ",_tag_dyneither(
_tmp1089,sizeof(char),2))),_tag_dyneither(_tmp1088,sizeof(void*),0)));}++ pos;}{
const char*_tmp108C;void*_tmp108B;(_tmp108B=0,Cyc_fprintf(Cyc_stderr,((_tmp108C="and ",
_tag_dyneither(_tmp108C,sizeof(char),5))),_tag_dyneither(_tmp108B,sizeof(void*),
0)));}pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*
_tmp108F;void*_tmp108E;(_tmp108E=0,Cyc_fprintf(Cyc_stderr,((_tmp108F="\n\t",
_tag_dyneither(_tmp108F,sizeof(char),3))),_tag_dyneither(_tmp108E,sizeof(void*),
0)));}pos=8;}{const char*_tmp1093;void*_tmp1092[1];struct Cyc_String_pa_struct
_tmp1091;(_tmp1091.tag=0,((_tmp1091.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)s2),((_tmp1092[0]=& _tmp1091,Cyc_fprintf(Cyc_stderr,((_tmp1093="%s ",
_tag_dyneither(_tmp1093,sizeof(char),4))),_tag_dyneither(_tmp1092,sizeof(void*),
1)))))));}pos +=_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmp1096;void*_tmp1095;(_tmp1095=0,Cyc_fprintf(Cyc_stderr,((_tmp1096="\n\t",
_tag_dyneither(_tmp1096,sizeof(char),3))),_tag_dyneither(_tmp1095,sizeof(void*),
0)));}pos=8;}{const char*_tmp1099;void*_tmp1098;(_tmp1098=0,Cyc_fprintf(Cyc_stderr,((
_tmp1099="are not compatible. ",_tag_dyneither(_tmp1099,sizeof(char),21))),
_tag_dyneither(_tmp1098,sizeof(void*),0)));}pos +=17;if((char*)Cyc_Tcutil_failure_reason.curr
!= (char*)(_tag_dyneither(0,0,0)).curr){if(pos + Cyc_strlen((struct _dyneither_ptr)
Cyc_Tcutil_failure_reason)>= 80){const char*_tmp109C;void*_tmp109B;(_tmp109B=0,
Cyc_fprintf(Cyc_stderr,((_tmp109C="\n\t",_tag_dyneither(_tmp109C,sizeof(char),3))),
_tag_dyneither(_tmp109B,sizeof(void*),0)));}{const char*_tmp10A0;void*_tmp109F[1];
struct Cyc_String_pa_struct _tmp109E;(_tmp109E.tag=0,((_tmp109E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmp109F[0]=&
_tmp109E,Cyc_fprintf(Cyc_stderr,((_tmp10A0="%s",_tag_dyneither(_tmp10A0,sizeof(
char),3))),_tag_dyneither(_tmp109F,sizeof(void*),1)))))));};}{const char*_tmp10A3;
void*_tmp10A2;(_tmp10A2=0,Cyc_fprintf(Cyc_stderr,((_tmp10A3="\n",_tag_dyneither(
_tmp10A3,sizeof(char),2))),_tag_dyneither(_tmp10A2,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp10A7;
void*_tmp10A6[1];struct Cyc_String_pa_struct _tmp10A5;(_tmp10A5.tag=0,((_tmp10A5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10A6[0]=& _tmp10A5,Cyc_fprintf(
Cyc_stderr,((_tmp10A7="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp10A7,sizeof(char),36))),_tag_dyneither(_tmp10A6,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp10AA;
struct Cyc_Core_Impossible_struct*_tmp10A9;(int)_throw((void*)((_tmp10A9=
_cycalloc(sizeof(*_tmp10A9)),((_tmp10A9[0]=((_tmp10AA.tag=Cyc_Core_Impossible,((
_tmp10AA.f1=msg,_tmp10AA)))),_tmp10A9)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp10AF;void*_tmp10AE[2];struct Cyc_String_pa_struct
_tmp10AD;struct Cyc_String_pa_struct _tmp10AC;(_tmp10AC.tag=0,((_tmp10AC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10AD.tag=0,((_tmp10AD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp10AE[0]=& _tmp10AD,((_tmp10AE[1]=& _tmp10AC,Cyc_fprintf(Cyc_stderr,((
_tmp10AF="%s::%s ",_tag_dyneither(_tmp10AF,sizeof(char),8))),_tag_dyneither(
_tmp10AE,sizeof(void*),2)))))))))))));}{const char*_tmp10B2;void*_tmp10B1;(
_tmp10B1=0,Cyc_fprintf(Cyc_stderr,((_tmp10B2="\n",_tag_dyneither(_tmp10B2,
sizeof(char),2))),_tag_dyneither(_tmp10B1,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp10B3;Cyc_Tcutil_warning_segs=((_tmp10B3=_cycalloc(
sizeof(*_tmp10B3)),((_tmp10B3->hd=sg,((_tmp10B3->tl=Cyc_Tcutil_warning_segs,
_tmp10B3))))));}{struct _dyneither_ptr*_tmp10B6;struct Cyc_List_List*_tmp10B5;Cyc_Tcutil_warning_msgs=((
_tmp10B5=_cycalloc(sizeof(*_tmp10B5)),((_tmp10B5->hd=((_tmp10B6=_cycalloc(
sizeof(*_tmp10B6)),((_tmp10B6[0]=msg,_tmp10B6)))),((_tmp10B5->tl=Cyc_Tcutil_warning_msgs,
_tmp10B5))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp10B9;void*_tmp10B8;(_tmp10B8=0,Cyc_fprintf(Cyc_stderr,((
_tmp10B9="***Warnings***\n",_tag_dyneither(_tmp10B9,sizeof(char),16))),
_tag_dyneither(_tmp10B8,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp10BE;void*_tmp10BD[2];struct
Cyc_String_pa_struct _tmp10BC;struct Cyc_String_pa_struct _tmp10BB;(_tmp10BB.tag=0,((
_tmp10BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10BC.tag=
0,((_tmp10BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10BD[0]=&
_tmp10BC,((_tmp10BD[1]=& _tmp10BB,Cyc_fprintf(Cyc_stderr,((_tmp10BE="%s: %s\n",
_tag_dyneither(_tmp10BE,sizeof(char),8))),_tag_dyneither(_tmp10BD,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10C1;void*_tmp10C0;(
_tmp10C0=0,Cyc_fprintf(Cyc_stderr,((_tmp10C1="**************\n",_tag_dyneither(
_tmp10C1,sizeof(char),16))),_tag_dyneither(_tmp10C0,sizeof(void*),0)));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t){void*
_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;void**_tmp43;void***_tmp44;
struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;struct Cyc_Absyn_Exp*_tmp49;
void**_tmp4B;void*_tmp4C;_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)
_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}
_LL2: goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 17)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 17)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10C2;*_tmp44=((_tmp10C2=
_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2[0]=t2,_tmp10C2))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10C3;*_tmp47=((_tmp10C3=_cycalloc(sizeof(*
_tmp10C3)),((_tmp10C3->v=t2,_tmp10C3))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 18)goto _LLB;
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
struct Cyc_Absyn_Unknown_kb_struct _tmp10C6;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10C5;return(void*)((_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5[0]=((
_tmp10C6.tag=1,((_tmp10C6.f1=0,_tmp10C6)))),_tmp10C5))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp10C9;struct
Cyc_Absyn_Less_kb_struct*_tmp10C8;return(void*)((_tmp10C8=_cycalloc(sizeof(*
_tmp10C8)),((_tmp10C8[0]=((_tmp10C9.tag=2,((_tmp10C9.f1=0,((_tmp10C9.f2=_tmp5C,
_tmp10C9)))))),_tmp10C8))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10CA;
return(_tmp10CA=_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA->name=tv->name,((
_tmp10CA->identity=- 1,((_tmp10CA->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10CA)))))));}static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
struct _tuple10 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple10*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple10*_tmp10CB;return(_tmp10CB=_cycalloc(sizeof(*
_tmp10CB)),((_tmp10CB->f1=_tmp64,((_tmp10CB->f2=_tmp65,((_tmp10CB->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10CB)))))));};}static struct _tuple12*Cyc_Tcutil_copy_tqt(struct
_tuple12*arg){struct _tuple12 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple12*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple12*_tmp10CC;return(_tmp10CC=_cycalloc(sizeof(*_tmp10CC)),((_tmp10CC->f1=
_tmp6A,((_tmp10CC->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp10CC)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10CD;return(_tmp10CD=_cycalloc(sizeof(*_tmp10CD)),((_tmp10CD->name=f->name,((
_tmp10CD->tq=f->tq,((_tmp10CD->type=Cyc_Tcutil_copy_type(f->type),((_tmp10CD->width=
f->width,((_tmp10CD->attributes=f->attributes,_tmp10CD)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10CE;return(_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp10CE->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp10CE)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10CF;return(_tmp10CF=
_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF->name=f->name,((_tmp10CF->tag=f->tag,((
_tmp10CF->loc=f->loc,_tmp10CF)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp10D2;struct Cyc_Absyn_VarType_struct*
_tmp10D1;return(void*)((_tmp10D1=_cycalloc(sizeof(*_tmp10D1)),((_tmp10D1[0]=((
_tmp10D2.tag=2,((_tmp10D2.f1=Cyc_Tcutil_copy_tvar(t),_tmp10D2)))),_tmp10D1))));}
void*Cyc_Tcutil_copy_type(void*t){void*_tmp76=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp7A;struct Cyc_Absyn_DatatypeInfo _tmp7C;union Cyc_Absyn_DatatypeInfoU _tmp7D;
struct Cyc_List_List*_tmp7E;struct Cyc_Absyn_DatatypeFieldInfo _tmp80;union Cyc_Absyn_DatatypeFieldInfoU
_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_PtrInfo _tmp84;void*_tmp85;
struct Cyc_Absyn_Tqual _tmp86;struct Cyc_Absyn_PtrAtts _tmp87;void*_tmp88;union Cyc_Absyn_Constraint*
_tmp89;union Cyc_Absyn_Constraint*_tmp8A;union Cyc_Absyn_Constraint*_tmp8B;struct
Cyc_Absyn_PtrLoc*_tmp8C;struct Cyc_Absyn_ArrayInfo _tmp90;void*_tmp91;struct Cyc_Absyn_Tqual
_tmp92;struct Cyc_Absyn_Exp*_tmp93;union Cyc_Absyn_Constraint*_tmp94;struct Cyc_Position_Segment*
_tmp95;struct Cyc_Absyn_FnInfo _tmp97;struct Cyc_List_List*_tmp98;struct Cyc_Core_Opt*
_tmp99;struct Cyc_Absyn_Tqual _tmp9A;void*_tmp9B;struct Cyc_List_List*_tmp9C;int
_tmp9D;struct Cyc_Absyn_VarargInfo*_tmp9E;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*
_tmpA0;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_AggrInfo _tmpA4;union Cyc_Absyn_AggrInfoU
_tmpA5;struct _tuple4 _tmpA6;enum Cyc_Absyn_AggrKind _tmpA7;struct _tuple2*_tmpA8;
struct Cyc_Core_Opt*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_AggrInfo
_tmpAC;union Cyc_Absyn_AggrInfoU _tmpAD;struct Cyc_Absyn_Aggrdecl**_tmpAE;struct Cyc_List_List*
_tmpAF;enum Cyc_Absyn_AggrKind _tmpB1;struct Cyc_List_List*_tmpB2;struct _tuple2*
_tmpB4;struct Cyc_Absyn_Enumdecl*_tmpB5;struct Cyc_List_List*_tmpB7;void*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBB;void*_tmpBD;void*_tmpBF;void*_tmpC0;struct _tuple2*
_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_Absyn_Typedefdecl*_tmpC4;void*_tmpC9;
struct Cyc_List_List*_tmpCB;void*_tmpCD;struct Cyc_Absyn_TypeDecl*_tmpCF;struct Cyc_Absyn_TypeDecl
_tmpD0;void*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD3;struct Cyc_Absyn_TypeDecl*
_tmpD5;struct Cyc_Absyn_TypeDecl _tmpD6;void*_tmpD7;struct Cyc_Absyn_Enumdecl*
_tmpD9;struct Cyc_Absyn_TypeDecl*_tmpDB;struct Cyc_Absyn_TypeDecl _tmpDC;void*
_tmpDD;struct Cyc_Absyn_Datatypedecl*_tmpDF;_LL23: {struct Cyc_Absyn_VoidType_struct*
_tmp77=(struct Cyc_Absyn_VoidType_struct*)_tmp76;if(_tmp77->tag != 0)goto _LL25;}
_LL24: goto _LL26;_LL25: {struct Cyc_Absyn_Evar_struct*_tmp78=(struct Cyc_Absyn_Evar_struct*)
_tmp76;if(_tmp78->tag != 1)goto _LL27;}_LL26: return t;_LL27: {struct Cyc_Absyn_VarType_struct*
_tmp79=(struct Cyc_Absyn_VarType_struct*)_tmp76;if(_tmp79->tag != 2)goto _LL29;
else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct _tmp10D5;struct
Cyc_Absyn_VarType_struct*_tmp10D4;return(void*)((_tmp10D4=_cycalloc(sizeof(*
_tmp10D4)),((_tmp10D4[0]=((_tmp10D5.tag=2,((_tmp10D5.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10D5)))),_tmp10D4))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10DB;struct Cyc_Absyn_DatatypeInfo _tmp10DA;
struct Cyc_Absyn_DatatypeType_struct*_tmp10D9;return(void*)((_tmp10D9=_cycalloc(
sizeof(*_tmp10D9)),((_tmp10D9[0]=((_tmp10DB.tag=3,((_tmp10DB.f1=((_tmp10DA.datatype_info=
_tmp7D,((_tmp10DA.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp10DA)))),_tmp10DB)))),
_tmp10D9))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10E1;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10E0;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10DF;return(void*)((_tmp10DF=_cycalloc(sizeof(*_tmp10DF)),((_tmp10DF[0]=((
_tmp10E1.tag=4,((_tmp10E1.f1=((_tmp10E0.field_info=_tmp81,((_tmp10E0.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp10E0)))),_tmp10E1)))),_tmp10DF))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE8=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpE9=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpEA=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEB=_tmp86;union Cyc_Absyn_Constraint*_tmpEC=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpED=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp10EB;struct Cyc_Absyn_PtrAtts
_tmp10EA;struct Cyc_Absyn_PtrInfo _tmp10E9;struct Cyc_Absyn_PointerType_struct*
_tmp10E8;return(void*)((_tmp10E8=_cycalloc(sizeof(*_tmp10E8)),((_tmp10E8[0]=((
_tmp10EB.tag=5,((_tmp10EB.f1=((_tmp10E9.elt_typ=_tmpE8,((_tmp10E9.elt_tq=_tmpEB,((
_tmp10E9.ptr_atts=((_tmp10EA.rgn=_tmpE9,((_tmp10EA.nullable=_tmpEA,((_tmp10EA.bounds=
_tmpEC,((_tmp10EA.zero_term=_tmpED,((_tmp10EA.ptrloc=_tmp8C,_tmp10EA)))))))))),
_tmp10E9)))))),_tmp10EB)))),_tmp10E8))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8F=(struct Cyc_Absyn_ArrayType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;
else{_tmp90=_tmp8F->f1;_tmp91=_tmp90.elt_type;_tmp92=_tmp90.tq;_tmp93=_tmp90.num_elts;
_tmp94=_tmp90.zero_term;_tmp95=_tmp90.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmp10F1;struct Cyc_Absyn_ArrayInfo _tmp10F0;struct Cyc_Absyn_ArrayType_struct*
_tmp10EF;return(void*)((_tmp10EF=_cycalloc(sizeof(*_tmp10EF)),((_tmp10EF[0]=((
_tmp10F1.tag=8,((_tmp10F1.f1=((_tmp10F0.elt_type=Cyc_Tcutil_copy_type(_tmp91),((
_tmp10F0.tq=_tmp92,((_tmp10F0.num_elts=_tmp93,((_tmp10F0.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp94),((_tmp10F0.zt_loc=
_tmp95,_tmp10F0)))))))))),_tmp10F1)))),_tmp10EF))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp96=(struct Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp96->tag != 9)goto _LL37;else{
_tmp97=_tmp96->f1;_tmp98=_tmp97.tvars;_tmp99=_tmp97.effect;_tmp9A=_tmp97.ret_tqual;
_tmp9B=_tmp97.ret_typ;_tmp9C=_tmp97.args;_tmp9D=_tmp97.c_varargs;_tmp9E=_tmp97.cyc_varargs;
_tmp9F=_tmp97.rgn_po;_tmpA0=_tmp97.attributes;}}_LL36: {struct Cyc_List_List*
_tmpF5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp98);struct Cyc_Core_Opt*
_tmp10F2;struct Cyc_Core_Opt*_tmpF6=_tmp99 == 0?0:((_tmp10F2=_cycalloc(sizeof(*
_tmp10F2)),((_tmp10F2->v=Cyc_Tcutil_copy_type((void*)_tmp99->v),_tmp10F2))));
void*_tmpF7=Cyc_Tcutil_copy_type(_tmp9B);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9C);int _tmpF9=_tmp9D;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9E != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9E;struct Cyc_Absyn_VarargInfo*
_tmp10F3;cyc_varargs2=((_tmp10F3=_cycalloc(sizeof(*_tmp10F3)),((_tmp10F3->name=
cv->name,((_tmp10F3->tq=cv->tq,((_tmp10F3->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp10F3->inject=cv->inject,_tmp10F3))))))))));}{struct Cyc_List_List*_tmpFB=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);struct Cyc_List_List*_tmpFC=_tmpA0;
struct Cyc_Absyn_FnType_struct _tmp10F9;struct Cyc_Absyn_FnInfo _tmp10F8;struct Cyc_Absyn_FnType_struct*
_tmp10F7;return(void*)((_tmp10F7=_cycalloc(sizeof(*_tmp10F7)),((_tmp10F7[0]=((
_tmp10F9.tag=9,((_tmp10F9.f1=((_tmp10F8.tvars=_tmpF5,((_tmp10F8.effect=_tmpF6,((
_tmp10F8.ret_tqual=_tmp9A,((_tmp10F8.ret_typ=_tmpF7,((_tmp10F8.args=_tmpF8,((
_tmp10F8.c_varargs=_tmpF9,((_tmp10F8.cyc_varargs=cyc_varargs2,((_tmp10F8.rgn_po=
_tmpFB,((_tmp10F8.attributes=_tmpFC,_tmp10F8)))))))))))))))))),_tmp10F9)))),
_tmp10F7))));};}_LL37: {struct Cyc_Absyn_TupleType_struct*_tmpA1=(struct Cyc_Absyn_TupleType_struct*)
_tmp76;if(_tmpA1->tag != 10)goto _LL39;else{_tmpA2=_tmpA1->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmp10FC;struct Cyc_Absyn_TupleType_struct*_tmp10FB;return(void*)((_tmp10FB=
_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10FC.tag=10,((_tmp10FC.f1=((
struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA2),_tmp10FC)))),_tmp10FB))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmpA3=(struct Cyc_Absyn_AggrType_struct*)_tmp76;
if(_tmpA3->tag != 11)goto _LL3B;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA4.aggr_info;if((
_tmpA5.UnknownAggr).tag != 1)goto _LL3B;_tmpA6=(struct _tuple4)(_tmpA5.UnknownAggr).val;
_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_tmpA9=_tmpA6.f3;_tmpAA=_tmpA4.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmp1102;struct Cyc_Absyn_AggrInfo _tmp1101;struct
Cyc_Absyn_AggrType_struct*_tmp1100;return(void*)((_tmp1100=_cycalloc(sizeof(*
_tmp1100)),((_tmp1100[0]=((_tmp1102.tag=11,((_tmp1102.f1=((_tmp1101.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA7,_tmpA8,_tmpA9),((_tmp1101.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmp1101)))),_tmp1102)))),_tmp1100))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAB->tag != 11)goto _LL3D;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;if((_tmpAD.KnownAggr).tag != 2)
goto _LL3D;_tmpAE=(struct Cyc_Absyn_Aggrdecl**)(_tmpAD.KnownAggr).val;_tmpAF=
_tmpAC.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct _tmp1108;struct Cyc_Absyn_AggrInfo
_tmp1107;struct Cyc_Absyn_AggrType_struct*_tmp1106;return(void*)((_tmp1106=
_cycalloc(sizeof(*_tmp1106)),((_tmp1106[0]=((_tmp1108.tag=11,((_tmp1108.f1=((
_tmp1107.aggr_info=Cyc_Absyn_KnownAggr(_tmpAE),((_tmp1107.targs=Cyc_Tcutil_copy_types(
_tmpAF),_tmp1107)))),_tmp1108)))),_tmp1106))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB0->tag != 12)goto
_LL3F;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmp110B;struct Cyc_Absyn_AnonAggrType_struct*_tmp110A;return(void*)((_tmp110A=
_cycalloc(sizeof(*_tmp110A)),((_tmp110A[0]=((_tmp110B.tag=12,((_tmp110B.f1=
_tmpB1,((_tmp110B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB2),_tmp110B)))))),_tmp110A))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmpB3=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB3->tag != 13)goto _LL41;
else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL40: {struct Cyc_Absyn_EnumType_struct
_tmp110E;struct Cyc_Absyn_EnumType_struct*_tmp110D;return(void*)((_tmp110D=
_cycalloc(sizeof(*_tmp110D)),((_tmp110D[0]=((_tmp110E.tag=13,((_tmp110E.f1=
_tmpB4,((_tmp110E.f2=_tmpB5,_tmp110E)))))),_tmp110D))));}_LL41: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB6->tag != 14)goto
_LL43;else{_tmpB7=_tmpB6->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmp1111;struct Cyc_Absyn_AnonEnumType_struct*_tmp1110;return(void*)((_tmp1110=
_cycalloc(sizeof(*_tmp1110)),((_tmp1110[0]=((_tmp1111.tag=14,((_tmp1111.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB7),_tmp1111)))),
_tmp1110))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB8=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB8->tag != 19)goto _LL45;else{_tmpB9=(void*)_tmpB8->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmp1114;struct Cyc_Absyn_TagType_struct*_tmp1113;
return(void*)((_tmp1113=_cycalloc(sizeof(*_tmp1113)),((_tmp1113[0]=((_tmp1114.tag=
19,((_tmp1114.f1=(void*)Cyc_Tcutil_copy_type(_tmpB9),_tmp1114)))),_tmp1113))));}
_LL45: {struct Cyc_Absyn_ValueofType_struct*_tmpBA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBA->tag != 18)goto _LL47;else{_tmpBB=_tmpBA->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmp1117;struct Cyc_Absyn_ValueofType_struct*_tmp1116;return(void*)((_tmp1116=
_cycalloc(sizeof(*_tmp1116)),((_tmp1116[0]=((_tmp1117.tag=18,((_tmp1117.f1=
_tmpBB,_tmp1117)))),_tmp1116))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBC->tag != 15)goto
_LL49;else{_tmpBD=(void*)_tmpBC->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct
_tmp111A;struct Cyc_Absyn_RgnHandleType_struct*_tmp1119;return(void*)((_tmp1119=
_cycalloc(sizeof(*_tmp1119)),((_tmp1119[0]=((_tmp111A.tag=15,((_tmp111A.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBD),_tmp111A)))),_tmp1119))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBE=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBE->tag != 16)goto _LL4B;
else{_tmpBF=(void*)_tmpBE->f1;_tmpC0=(void*)_tmpBE->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmp111D;struct Cyc_Absyn_DynRgnType_struct*_tmp111C;return(void*)((_tmp111C=
_cycalloc(sizeof(*_tmp111C)),((_tmp111C[0]=((_tmp111D.tag=16,((_tmp111D.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBF),((_tmp111D.f2=(void*)Cyc_Tcutil_copy_type(_tmpC0),
_tmp111D)))))),_tmp111C))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpC1=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC1->tag != 17)goto _LL4D;else{
_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;_tmpC4=_tmpC1->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmp1120;struct Cyc_Absyn_TypedefType_struct*_tmp111F;return(void*)((_tmp111F=
_cycalloc(sizeof(*_tmp111F)),((_tmp111F[0]=((_tmp1120.tag=17,((_tmp1120.f1=
_tmpC2,((_tmp1120.f2=Cyc_Tcutil_copy_types(_tmpC3),((_tmp1120.f3=_tmpC4,((
_tmp1120.f4=0,_tmp1120)))))))))),_tmp111F))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC5=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC5->tag != 21)goto _LL4F;}
_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_struct*_tmpC6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp76;if(_tmpC6->tag != 22)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC7=(struct Cyc_Absyn_HeapRgn_struct*)_tmp76;if(_tmpC7->tag != 20)goto _LL53;}
_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_struct*_tmpC8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp76;if(_tmpC8->tag != 23)goto _LL55;else{_tmpC9=(void*)_tmpC8->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_struct _tmp1123;struct Cyc_Absyn_AccessEff_struct*
_tmp1122;return(void*)((_tmp1122=_cycalloc(sizeof(*_tmp1122)),((_tmp1122[0]=((
_tmp1123.tag=23,((_tmp1123.f1=(void*)Cyc_Tcutil_copy_type(_tmpC9),_tmp1123)))),
_tmp1122))));}_LL55: {struct Cyc_Absyn_JoinEff_struct*_tmpCA=(struct Cyc_Absyn_JoinEff_struct*)
_tmp76;if(_tmpCA->tag != 24)goto _LL57;else{_tmpCB=_tmpCA->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_struct
_tmp1126;struct Cyc_Absyn_JoinEff_struct*_tmp1125;return(void*)((_tmp1125=
_cycalloc(sizeof(*_tmp1125)),((_tmp1125[0]=((_tmp1126.tag=24,((_tmp1126.f1=Cyc_Tcutil_copy_types(
_tmpCB),_tmp1126)))),_tmp1125))));}_LL57: {struct Cyc_Absyn_RgnsEff_struct*_tmpCC=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp76;if(_tmpCC->tag != 25)goto _LL59;else{_tmpCD=(
void*)_tmpCC->f1;}}_LL58: {struct Cyc_Absyn_RgnsEff_struct _tmp1129;struct Cyc_Absyn_RgnsEff_struct*
_tmp1128;return(void*)((_tmp1128=_cycalloc(sizeof(*_tmp1128)),((_tmp1128[0]=((
_tmp1129.tag=25,((_tmp1129.f1=(void*)Cyc_Tcutil_copy_type(_tmpCD),_tmp1129)))),
_tmp1128))));}_LL59: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCE=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCE->tag != 26)goto _LL5B;else{_tmpCF=_tmpCE->f1;_tmpD0=*_tmpCF;
_tmpD1=_tmpD0.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD2=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL5B;else{_tmpD3=_tmpD2->f1;}};}}_LL5A: {struct
Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD3->tvs);struct Cyc_Absyn_AggrType_struct
_tmp112F;struct Cyc_Absyn_AggrInfo _tmp112E;struct Cyc_Absyn_AggrType_struct*
_tmp112D;return(void*)((_tmp112D=_cycalloc(sizeof(*_tmp112D)),((_tmp112D[0]=((
_tmp112F.tag=11,((_tmp112F.f1=((_tmp112E.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD3->kind,
_tmpD3->name,0),((_tmp112E.targs=_tmp11F,_tmp112E)))),_tmp112F)))),_tmp112D))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD4=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD4->tag != 26)goto _LL5D;else{_tmpD5=_tmpD4->f1;_tmpD6=*_tmpD5;
_tmpD7=_tmpD6.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD8=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD7;if(_tmpD8->tag != 1)goto _LL5D;else{_tmpD9=_tmpD8->f1;}};}}_LL5C: {struct
Cyc_Absyn_EnumType_struct _tmp1132;struct Cyc_Absyn_EnumType_struct*_tmp1131;
return(void*)((_tmp1131=_cycalloc(sizeof(*_tmp1131)),((_tmp1131[0]=((_tmp1132.tag=
13,((_tmp1132.f1=_tmpD9->name,((_tmp1132.f2=0,_tmp1132)))))),_tmp1131))));}_LL5D: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpDA=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpDA->tag != 26)goto _LL22;else{_tmpDB=_tmpDA->f1;_tmpDC=*_tmpDB;
_tmpDD=_tmpDC.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDE=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDD;if(_tmpDE->tag != 2)goto _LL22;else{_tmpDF=_tmpDE->f1;}};}}_LL5E: {struct
Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDF->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp113C;struct Cyc_Absyn_UnknownDatatypeInfo _tmp113B;struct Cyc_Absyn_DatatypeInfo
_tmp113A;struct Cyc_Absyn_DatatypeType_struct*_tmp1139;return(void*)((_tmp1139=
_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((_tmp113C.tag=3,((_tmp113C.f1=((
_tmp113A.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp113B.name=_tmpDF->name,((
_tmp113B.is_extensible=0,_tmp113B))))),((_tmp113A.targs=_tmp125,_tmp113A)))),
_tmp113C)))),_tmp1139))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp12A=d;struct Cyc_Absyn_Exp*_tmp12C;struct _dyneither_ptr*_tmp12E;_LL60: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12B=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp12A;if(_tmp12B->tag != 0)goto _LL62;else{_tmp12C=_tmp12B->f1;}}_LL61: {struct
Cyc_Absyn_ArrayElement_struct _tmp113F;struct Cyc_Absyn_ArrayElement_struct*
_tmp113E;return(void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((_tmp113E[0]=((
_tmp113F.tag=0,((_tmp113F.f1=Cyc_Tcutil_deep_copy_exp(_tmp12C),_tmp113F)))),
_tmp113E))));}_LL62: {struct Cyc_Absyn_FieldName_struct*_tmp12D=(struct Cyc_Absyn_FieldName_struct*)
_tmp12A;if(_tmp12D->tag != 1)goto _LL5F;else{_tmp12E=_tmp12D->f1;}}_LL63: return d;
_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){struct _tuple19*_tmp1140;
return(_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp1140->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp1140)))));}
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*e){void*_tmp132=
e->r;union Cyc_Absyn_Cnst _tmp134;struct _tuple2*_tmp136;void*_tmp137;struct _tuple2*
_tmp139;enum Cyc_Absyn_Primop _tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Exp*
_tmp13E;struct Cyc_Core_Opt*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*
_tmp142;enum Cyc_Absyn_Incrementor _tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*
_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*
_tmp14A;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*
_tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_List_List*
_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_VarargCallInfo*
_tmp157;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*
_tmp15D;struct Cyc_List_List*_tmp15E;void*_tmp160;struct Cyc_Absyn_Exp*_tmp161;int
_tmp162;enum Cyc_Absyn_Coercion _tmp163;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*
_tmp167;struct Cyc_Absyn_Exp*_tmp168;void*_tmp16A;struct Cyc_Absyn_Exp*_tmp16C;
void*_tmp16E;void*_tmp16F;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*
_tmp173;struct _dyneither_ptr*_tmp174;int _tmp175;int _tmp176;struct Cyc_Absyn_Exp*
_tmp178;struct _dyneither_ptr*_tmp179;int _tmp17A;int _tmp17B;struct Cyc_Absyn_Exp*
_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;struct Cyc_List_List*_tmp180;struct _tuple10*
_tmp182;struct _tuple10 _tmp183;struct Cyc_Core_Opt*_tmp184;struct Cyc_Absyn_Tqual
_tmp185;void*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;
struct Cyc_Absyn_Vardecl*_tmp18B;struct Cyc_Absyn_Exp*_tmp18C;struct Cyc_Absyn_Exp*
_tmp18D;int _tmp18E;struct _tuple2*_tmp190;struct Cyc_List_List*_tmp191;struct Cyc_List_List*
_tmp192;struct Cyc_Absyn_Aggrdecl*_tmp193;void*_tmp195;struct Cyc_List_List*
_tmp196;struct Cyc_List_List*_tmp198;struct Cyc_Absyn_Datatypedecl*_tmp199;struct
Cyc_Absyn_Datatypefield*_tmp19A;struct _tuple2*_tmp19C;struct Cyc_Absyn_Enumdecl*
_tmp19D;struct Cyc_Absyn_Enumfield*_tmp19E;struct _tuple2*_tmp1A0;void*_tmp1A1;
struct Cyc_Absyn_Enumfield*_tmp1A2;struct Cyc_Absyn_MallocInfo _tmp1A4;int _tmp1A5;
struct Cyc_Absyn_Exp*_tmp1A6;void**_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;int _tmp1A9;
struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Core_Opt*
_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B2;struct
_dyneither_ptr*_tmp1B3;void*_tmp1B5;int _tmp1B7;struct _dyneither_ptr _tmp1B8;_LL65: {
struct Cyc_Absyn_Const_e_struct*_tmp133=(struct Cyc_Absyn_Const_e_struct*)_tmp132;
if(_tmp133->tag != 0)goto _LL67;else{_tmp134=_tmp133->f1;}}_LL66: return Cyc_Absyn_const_exp(
_tmp134,e->loc);_LL67: {struct Cyc_Absyn_Var_e_struct*_tmp135=(struct Cyc_Absyn_Var_e_struct*)
_tmp132;if(_tmp135->tag != 1)goto _LL69;else{_tmp136=_tmp135->f1;_tmp137=(void*)
_tmp135->f2;}}_LL68: return Cyc_Absyn_varb_exp(_tmp136,_tmp137,e->loc);_LL69: {
struct Cyc_Absyn_UnknownId_e_struct*_tmp138=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp132;if(_tmp138->tag != 2)goto _LL6B;else{_tmp139=_tmp138->f1;}}_LL6A: return Cyc_Absyn_unknownid_exp(
_tmp139,e->loc);_LL6B: {struct Cyc_Absyn_Primop_e_struct*_tmp13A=(struct Cyc_Absyn_Primop_e_struct*)
_tmp132;if(_tmp13A->tag != 3)goto _LL6D;else{_tmp13B=_tmp13A->f1;_tmp13C=_tmp13A->f2;}}
_LL6C: return Cyc_Absyn_primop_exp(_tmp13B,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp13C),e->loc);_LL6D: {struct Cyc_Absyn_AssignOp_e_struct*_tmp13D=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp132;if(_tmp13D->tag != 4)goto _LL6F;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;
_tmp140=_tmp13D->f3;}}_LL6E: {struct Cyc_Core_Opt*_tmp1141;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13E),(unsigned int)_tmp13F?(_tmp1141=
_cycalloc_atomic(sizeof(*_tmp1141)),((_tmp1141->v=(void*)((enum Cyc_Absyn_Primop)
_tmp13F->v),_tmp1141))): 0,Cyc_Tcutil_deep_copy_exp(_tmp140),e->loc);}_LL6F: {
struct Cyc_Absyn_Increment_e_struct*_tmp141=(struct Cyc_Absyn_Increment_e_struct*)
_tmp132;if(_tmp141->tag != 5)goto _LL71;else{_tmp142=_tmp141->f1;_tmp143=_tmp141->f2;}}
_LL70: return Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp142),_tmp143,e->loc);
_LL71: {struct Cyc_Absyn_Conditional_e_struct*_tmp144=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp132;if(_tmp144->tag != 6)goto _LL73;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;
_tmp147=_tmp144->f3;}}_LL72: return Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(
_tmp145),Cyc_Tcutil_deep_copy_exp(_tmp146),Cyc_Tcutil_deep_copy_exp(_tmp147),e->loc);
_LL73: {struct Cyc_Absyn_And_e_struct*_tmp148=(struct Cyc_Absyn_And_e_struct*)
_tmp132;if(_tmp148->tag != 7)goto _LL75;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}
_LL74: return Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp149),Cyc_Tcutil_deep_copy_exp(
_tmp14A),e->loc);_LL75: {struct Cyc_Absyn_Or_e_struct*_tmp14B=(struct Cyc_Absyn_Or_e_struct*)
_tmp132;if(_tmp14B->tag != 8)goto _LL77;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14B->f2;}}
_LL76: return Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp14C),Cyc_Tcutil_deep_copy_exp(
_tmp14D),e->loc);_LL77: {struct Cyc_Absyn_SeqExp_e_struct*_tmp14E=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp132;if(_tmp14E->tag != 9)goto _LL79;else{_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;}}
_LL78: return Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp14F),Cyc_Tcutil_deep_copy_exp(
_tmp150),e->loc);_LL79: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp151=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp132;if(_tmp151->tag != 10)goto _LL7B;else{_tmp152=_tmp151->f1;_tmp153=_tmp151->f2;}}
_LL7A: return Cyc_Absyn_unknowncall_exp(Cyc_Tcutil_deep_copy_exp(_tmp152),((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp153),e->loc);_LL7B: {struct Cyc_Absyn_FnCall_e_struct*
_tmp154=(struct Cyc_Absyn_FnCall_e_struct*)_tmp132;if(_tmp154->tag != 11)goto _LL7D;
else{_tmp155=_tmp154->f1;_tmp156=_tmp154->f2;_tmp157=_tmp154->f3;}}_LL7C: {
struct Cyc_Absyn_VarargCallInfo*_tmp1BA=_tmp157;struct Cyc_Absyn_VarargCallInfo
_tmp1BB;int _tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_VarargInfo*
_tmp1BE;_LLB8: if(_tmp1BA == 0)goto _LLBA;_tmp1BB=*_tmp1BA;_tmp1BC=_tmp1BB.num_varargs;
_tmp1BD=_tmp1BB.injectors;_tmp1BE=_tmp1BB.vai;_LLB9: {struct Cyc_Absyn_VarargInfo
_tmp1C0;struct Cyc_Core_Opt*_tmp1C1;struct Cyc_Absyn_Tqual _tmp1C2;void*_tmp1C3;int
_tmp1C4;struct Cyc_Absyn_VarargInfo*_tmp1BF=_tmp1BE;_tmp1C0=*_tmp1BF;_tmp1C1=
_tmp1C0.name;_tmp1C2=_tmp1C0.tq;_tmp1C3=_tmp1C0.type;_tmp1C4=_tmp1C0.inject;{
struct Cyc_Absyn_FnCall_e_struct _tmp114B;struct Cyc_Absyn_VarargInfo*_tmp114A;
struct Cyc_Absyn_VarargCallInfo*_tmp1149;struct Cyc_Absyn_FnCall_e_struct*_tmp1148;
return Cyc_Absyn_new_exp((void*)((_tmp1148=_cycalloc(sizeof(*_tmp1148)),((
_tmp1148[0]=((_tmp114B.tag=11,((_tmp114B.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp114B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp156),((
_tmp114B.f3=((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149->num_varargs=
_tmp1BC,((_tmp1149->injectors=_tmp1BD,((_tmp1149->vai=((_tmp114A=_cycalloc(
sizeof(*_tmp114A)),((_tmp114A->name=_tmp1C1,((_tmp114A->tq=_tmp1C2,((_tmp114A->type=
Cyc_Tcutil_copy_type(_tmp1C3),((_tmp114A->inject=_tmp1C4,_tmp114A)))))))))),
_tmp1149)))))))),_tmp114B)))))))),_tmp1148)))),e->loc);};}_LLBA:;_LLBB: {struct
Cyc_Absyn_FnCall_e_struct _tmp114E;struct Cyc_Absyn_FnCall_e_struct*_tmp114D;
return Cyc_Absyn_new_exp((void*)((_tmp114D=_cycalloc(sizeof(*_tmp114D)),((
_tmp114D[0]=((_tmp114E.tag=11,((_tmp114E.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp114E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp156),((
_tmp114E.f3=_tmp157,_tmp114E)))))))),_tmp114D)))),e->loc);}_LLB7:;}_LL7D: {
struct Cyc_Absyn_Throw_e_struct*_tmp158=(struct Cyc_Absyn_Throw_e_struct*)_tmp132;
if(_tmp158->tag != 12)goto _LL7F;else{_tmp159=_tmp158->f1;}}_LL7E: return Cyc_Absyn_throw_exp(
Cyc_Tcutil_deep_copy_exp(_tmp159),e->loc);_LL7F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp15A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp132;if(_tmp15A->tag != 13)
goto _LL81;else{_tmp15B=_tmp15A->f1;}}_LL80: return Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(
_tmp15B),e->loc);_LL81: {struct Cyc_Absyn_Instantiate_e_struct*_tmp15C=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp132;if(_tmp15C->tag != 14)goto _LL83;else{_tmp15D=_tmp15C->f1;_tmp15E=_tmp15C->f2;}}
_LL82: return Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp15D),((struct
Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp15E),e->loc);_LL83: {struct Cyc_Absyn_Cast_e_struct*_tmp15F=(struct Cyc_Absyn_Cast_e_struct*)
_tmp132;if(_tmp15F->tag != 15)goto _LL85;else{_tmp160=(void*)_tmp15F->f1;_tmp161=
_tmp15F->f2;_tmp162=_tmp15F->f3;_tmp163=_tmp15F->f4;}}_LL84: return Cyc_Absyn_cast_exp(
Cyc_Tcutil_copy_type(_tmp160),Cyc_Tcutil_deep_copy_exp(_tmp161),_tmp162,_tmp163,
e->loc);_LL85: {struct Cyc_Absyn_Address_e_struct*_tmp164=(struct Cyc_Absyn_Address_e_struct*)
_tmp132;if(_tmp164->tag != 16)goto _LL87;else{_tmp165=_tmp164->f1;}}_LL86: return
Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp165),e->loc);_LL87: {struct
Cyc_Absyn_New_e_struct*_tmp166=(struct Cyc_Absyn_New_e_struct*)_tmp132;if(_tmp166->tag
!= 17)goto _LL89;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LL88: {struct Cyc_Absyn_Exp*
eo1=_tmp167;if(_tmp167 != 0)eo1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp167);return Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(
_tmp168),e->loc);}_LL89: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp169=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp132;if(_tmp169->tag != 18)goto _LL8B;else{_tmp16A=(void*)_tmp169->f1;}}_LL8A:
return Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp16A),e->loc);_LL8B: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp16B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp132;if(_tmp16B->tag != 19)goto _LL8D;else{_tmp16C=_tmp16B->f1;}}_LL8C: return
Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp16C),e->loc);_LL8D: {struct
Cyc_Absyn_Offsetof_e_struct*_tmp16D=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp132;
if(_tmp16D->tag != 20)goto _LL8F;else{_tmp16E=(void*)_tmp16D->f1;_tmp16F=(void*)
_tmp16D->f2;}}_LL8E: return Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp16E),
_tmp16F,e->loc);_LL8F: {struct Cyc_Absyn_Deref_e_struct*_tmp170=(struct Cyc_Absyn_Deref_e_struct*)
_tmp132;if(_tmp170->tag != 21)goto _LL91;else{_tmp171=_tmp170->f1;}}_LL90: return
Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp171),e->loc);_LL91: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp172=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp132;if(_tmp172->tag != 22)goto
_LL93;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;_tmp175=_tmp172->f3;_tmp176=
_tmp172->f4;}}_LL92: {struct Cyc_Absyn_AggrMember_e_struct _tmp1151;struct Cyc_Absyn_AggrMember_e_struct*
_tmp1150;return Cyc_Absyn_new_exp((void*)((_tmp1150=_cycalloc(sizeof(*_tmp1150)),((
_tmp1150[0]=((_tmp1151.tag=22,((_tmp1151.f1=Cyc_Tcutil_deep_copy_exp(_tmp173),((
_tmp1151.f2=_tmp174,((_tmp1151.f3=_tmp175,((_tmp1151.f4=_tmp176,_tmp1151)))))))))),
_tmp1150)))),e->loc);}_LL93: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp177=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp132;if(_tmp177->tag != 23)goto _LL95;else{
_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;_tmp17A=_tmp177->f3;_tmp17B=_tmp177->f4;}}
_LL94: {struct Cyc_Absyn_AggrArrow_e_struct _tmp1154;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1153;return Cyc_Absyn_new_exp((void*)((_tmp1153=_cycalloc(sizeof(*_tmp1153)),((
_tmp1153[0]=((_tmp1154.tag=23,((_tmp1154.f1=Cyc_Tcutil_deep_copy_exp(_tmp178),((
_tmp1154.f2=_tmp179,((_tmp1154.f3=_tmp17A,((_tmp1154.f4=_tmp17B,_tmp1154)))))))))),
_tmp1153)))),e->loc);}_LL95: {struct Cyc_Absyn_Subscript_e_struct*_tmp17C=(struct
Cyc_Absyn_Subscript_e_struct*)_tmp132;if(_tmp17C->tag != 24)goto _LL97;else{
_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;}}_LL96: return Cyc_Absyn_subscript_exp(
Cyc_Tcutil_deep_copy_exp(_tmp17D),Cyc_Tcutil_deep_copy_exp(_tmp17E),e->loc);
_LL97: {struct Cyc_Absyn_Tuple_e_struct*_tmp17F=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp132;if(_tmp17F->tag != 25)goto _LL99;else{_tmp180=_tmp17F->f1;}}_LL98: return
Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp180),e->loc);
_LL99: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp181=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp132;if(_tmp181->tag != 26)goto _LL9B;else{_tmp182=_tmp181->f1;_tmp183=*_tmp182;
_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;_tmp186=_tmp183.f3;_tmp187=_tmp181->f2;}}
_LL9A: {struct Cyc_Core_Opt*vopt1=_tmp184;if(_tmp184 != 0){struct Cyc_Core_Opt*
_tmp1155;vopt1=((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155->v=(struct
_dyneither_ptr*)_tmp184->v,_tmp1155))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp115B;struct _tuple10*_tmp115A;struct Cyc_Absyn_CompoundLit_e_struct*_tmp1159;
return Cyc_Absyn_new_exp((void*)((_tmp1159=_cycalloc(sizeof(*_tmp1159)),((
_tmp1159[0]=((_tmp115B.tag=26,((_tmp115B.f1=((_tmp115A=_cycalloc(sizeof(*
_tmp115A)),((_tmp115A->f1=vopt1,((_tmp115A->f2=_tmp185,((_tmp115A->f3=Cyc_Tcutil_copy_type(
_tmp186),_tmp115A)))))))),((_tmp115B.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*
f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp187),_tmp115B)))))),_tmp1159)))),e->loc);};}_LL9B: {struct Cyc_Absyn_Array_e_struct*
_tmp188=(struct Cyc_Absyn_Array_e_struct*)_tmp132;if(_tmp188->tag != 27)goto _LL9D;
else{_tmp189=_tmp188->f1;}}_LL9C: {struct Cyc_Absyn_Array_e_struct _tmp115E;struct
Cyc_Absyn_Array_e_struct*_tmp115D;return Cyc_Absyn_new_exp((void*)((_tmp115D=
_cycalloc(sizeof(*_tmp115D)),((_tmp115D[0]=((_tmp115E.tag=27,((_tmp115E.f1=((
struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp189),_tmp115E)))),_tmp115D)))),e->loc);}
_LL9D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp18A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp132;if(_tmp18A->tag != 28)goto _LL9F;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;
_tmp18D=_tmp18A->f3;_tmp18E=_tmp18A->f4;}}_LL9E: {struct Cyc_Absyn_Comprehension_e_struct
_tmp1161;struct Cyc_Absyn_Comprehension_e_struct*_tmp1160;return Cyc_Absyn_new_exp((
void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((_tmp1160[0]=((_tmp1161.tag=28,((
_tmp1161.f1=_tmp18B,((_tmp1161.f2=Cyc_Tcutil_deep_copy_exp(_tmp18C),((_tmp1161.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18D),((_tmp1161.f4=_tmp18E,_tmp1161)))))))))),
_tmp1160)))),e->loc);}_LL9F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp18F=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp132;if(_tmp18F->tag != 29)goto _LLA1;else{
_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;_tmp193=_tmp18F->f4;}}
_LLA0: {struct Cyc_Absyn_Aggregate_e_struct _tmp1164;struct Cyc_Absyn_Aggregate_e_struct*
_tmp1163;return Cyc_Absyn_new_exp((void*)((_tmp1163=_cycalloc(sizeof(*_tmp1163)),((
_tmp1163[0]=((_tmp1164.tag=29,((_tmp1164.f1=_tmp190,((_tmp1164.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp191),((_tmp1164.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct
_tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp192),((
_tmp1164.f4=_tmp193,_tmp1164)))))))))),_tmp1163)))),e->loc);}_LLA1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp194=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp132;if(_tmp194->tag != 30)goto
_LLA3;else{_tmp195=(void*)_tmp194->f1;_tmp196=_tmp194->f2;}}_LLA2: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp1167;struct Cyc_Absyn_AnonStruct_e_struct*_tmp1166;return Cyc_Absyn_new_exp((
void*)((_tmp1166=_cycalloc(sizeof(*_tmp1166)),((_tmp1166[0]=((_tmp1167.tag=30,((
_tmp1167.f1=(void*)Cyc_Tcutil_copy_type(_tmp195),((_tmp1167.f2=((struct Cyc_List_List*(*)(
struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp196),_tmp1167)))))),_tmp1166)))),e->loc);}_LLA3: {struct Cyc_Absyn_Datatype_e_struct*
_tmp197=(struct Cyc_Absyn_Datatype_e_struct*)_tmp132;if(_tmp197->tag != 31)goto
_LLA5;else{_tmp198=_tmp197->f1;_tmp199=_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLA4: {
struct Cyc_Absyn_Datatype_e_struct _tmp116A;struct Cyc_Absyn_Datatype_e_struct*
_tmp1169;return Cyc_Absyn_new_exp((void*)((_tmp1169=_cycalloc(sizeof(*_tmp1169)),((
_tmp1169[0]=((_tmp116A.tag=31,((_tmp116A.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp198),((_tmp116A.f2=_tmp199,((_tmp116A.f3=_tmp19A,_tmp116A)))))))),_tmp1169)))),
e->loc);}_LLA5: {struct Cyc_Absyn_Enum_e_struct*_tmp19B=(struct Cyc_Absyn_Enum_e_struct*)
_tmp132;if(_tmp19B->tag != 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;
_tmp19E=_tmp19B->f3;}}_LLA6: return e;_LLA7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp19F=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp132;if(_tmp19F->tag != 33)goto
_LLA9;else{_tmp1A0=_tmp19F->f1;_tmp1A1=(void*)_tmp19F->f2;_tmp1A2=_tmp19F->f3;}}
_LLA8: {struct Cyc_Absyn_AnonEnum_e_struct _tmp116D;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp116C;return Cyc_Absyn_new_exp((void*)((_tmp116C=_cycalloc(sizeof(*_tmp116C)),((
_tmp116C[0]=((_tmp116D.tag=33,((_tmp116D.f1=_tmp1A0,((_tmp116D.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A1),((_tmp116D.f3=_tmp1A2,_tmp116D)))))))),_tmp116C)))),e->loc);}_LLA9: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A3=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp132;if(_tmp1A3->tag != 34)goto _LLAB;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.is_calloc;
_tmp1A6=_tmp1A4.rgn;_tmp1A7=_tmp1A4.elt_type;_tmp1A8=_tmp1A4.num_elts;_tmp1A9=
_tmp1A4.fat_result;}}_LLAA: {struct Cyc_Absyn_Exp*_tmp1DF=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A6;if(_tmp1A6 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A6);{void**t1=_tmp1A7;if(_tmp1A7 != 0){void**_tmp116E;
t1=((_tmp116E=_cycalloc(sizeof(*_tmp116E)),((_tmp116E[0]=Cyc_Tcutil_copy_type(*
_tmp1A7),_tmp116E))));}{struct Cyc_Absyn_Malloc_e_struct _tmp1174;struct Cyc_Absyn_MallocInfo
_tmp1173;struct Cyc_Absyn_Malloc_e_struct*_tmp1172;_tmp1DF->r=(void*)((_tmp1172=
_cycalloc(sizeof(*_tmp1172)),((_tmp1172[0]=((_tmp1174.tag=34,((_tmp1174.f1=((
_tmp1173.is_calloc=_tmp1A5,((_tmp1173.rgn=r1,((_tmp1173.elt_type=t1,((_tmp1173.num_elts=
_tmp1A8,((_tmp1173.fat_result=_tmp1A9,_tmp1173)))))))))),_tmp1174)))),_tmp1172))));}
return _tmp1DF;};}_LLAB: {struct Cyc_Absyn_Swap_e_struct*_tmp1AA=(struct Cyc_Absyn_Swap_e_struct*)
_tmp132;if(_tmp1AA->tag != 35)goto _LLAD;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}
_LLAC: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AB),Cyc_Tcutil_deep_copy_exp(
_tmp1AC),e->loc);_LLAD: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AD=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp132;if(_tmp1AD->tag != 36)goto _LLAF;else{
_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LLAE: {struct Cyc_Core_Opt*nopt1=
_tmp1AE;if(_tmp1AE != 0){struct Cyc_Core_Opt*_tmp1175;nopt1=((_tmp1175=_cycalloc(
sizeof(*_tmp1175)),((_tmp1175->v=(struct _tuple2*)_tmp1AE->v,_tmp1175))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp1178;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1177;return Cyc_Absyn_new_exp((void*)((_tmp1177=_cycalloc(sizeof(*_tmp1177)),((
_tmp1177[0]=((_tmp1178.tag=36,((_tmp1178.f1=nopt1,((_tmp1178.f2=((struct Cyc_List_List*(*)(
struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1AF),_tmp1178)))))),_tmp1177)))),e->loc);};}_LLAF: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1B0=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp132;if(_tmp1B0->tag != 37)goto
_LLB1;}_LLB0: {struct Cyc_Core_Failure_struct _tmp117E;const char*_tmp117D;struct
Cyc_Core_Failure_struct*_tmp117C;(int)_throw((void*)((_tmp117C=_cycalloc(sizeof(*
_tmp117C)),((_tmp117C[0]=((_tmp117E.tag=Cyc_Core_Failure,((_tmp117E.f1=((
_tmp117D="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp117D,
sizeof(char),45))),_tmp117E)))),_tmp117C)))));}_LLB1: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B1=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp132;if(_tmp1B1->tag != 38)goto
_LLB3;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LLB2: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp1181;struct Cyc_Absyn_Tagcheck_e_struct*_tmp1180;return Cyc_Absyn_new_exp((
void*)((_tmp1180=_cycalloc(sizeof(*_tmp1180)),((_tmp1180[0]=((_tmp1181.tag=38,((
_tmp1181.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B2),((_tmp1181.f2=_tmp1B3,_tmp1181)))))),
_tmp1180)))),e->loc);}_LLB3: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B4=(struct
Cyc_Absyn_Valueof_e_struct*)_tmp132;if(_tmp1B4->tag != 39)goto _LLB5;else{_tmp1B5=(
void*)_tmp1B4->f1;}}_LLB4: return Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(
_tmp1B5),e->loc);_LLB5: {struct Cyc_Absyn_Asm_e_struct*_tmp1B6=(struct Cyc_Absyn_Asm_e_struct*)
_tmp132;if(_tmp1B6->tag != 40)goto _LL64;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}
_LLB6: return Cyc_Absyn_asm_exp(_tmp1B7,_tmp1B8,e->loc);_LL64:;}struct _tuple20{
enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual
f1;enum Cyc_Absyn_AliasQual f2;};int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,
struct Cyc_Absyn_Kind*ka2){struct Cyc_Absyn_Kind _tmp1ED;enum Cyc_Absyn_KindQual
_tmp1EE;enum Cyc_Absyn_AliasQual _tmp1EF;struct Cyc_Absyn_Kind*_tmp1EC=ka1;_tmp1ED=*
_tmp1EC;_tmp1EE=_tmp1ED.kind;_tmp1EF=_tmp1ED.aliasqual;{struct Cyc_Absyn_Kind
_tmp1F1;enum Cyc_Absyn_KindQual _tmp1F2;enum Cyc_Absyn_AliasQual _tmp1F3;struct Cyc_Absyn_Kind*
_tmp1F0=ka2;_tmp1F1=*_tmp1F0;_tmp1F2=_tmp1F1.kind;_tmp1F3=_tmp1F1.aliasqual;if(
_tmp1EE != _tmp1F2){struct _tuple20 _tmp1182;struct _tuple20 _tmp1F5=(_tmp1182.f1=
_tmp1EE,((_tmp1182.f2=_tmp1F2,_tmp1182)));enum Cyc_Absyn_KindQual _tmp1F6;enum 
Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual _tmp1F8;enum Cyc_Absyn_KindQual
_tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;enum Cyc_Absyn_KindQual _tmp1FB;_LLBD:
_tmp1F6=_tmp1F5.f1;if(_tmp1F6 != Cyc_Absyn_BoxKind)goto _LLBF;_tmp1F7=_tmp1F5.f2;
if(_tmp1F7 != Cyc_Absyn_MemKind)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1F8=_tmp1F5.f1;
if(_tmp1F8 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F9=_tmp1F5.f2;if(_tmp1F9 != Cyc_Absyn_AnyKind)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1FA=_tmp1F5.f1;if(_tmp1FA != Cyc_Absyn_MemKind)
goto _LLC3;_tmp1FB=_tmp1F5.f2;if(_tmp1FB != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto
_LLBC;_LLC3:;_LLC4: return 0;_LLBC:;}if(_tmp1EF != _tmp1F3){struct _tuple21 _tmp1183;
struct _tuple21 _tmp1FD=(_tmp1183.f1=_tmp1EF,((_tmp1183.f2=_tmp1F3,_tmp1183)));
enum Cyc_Absyn_AliasQual _tmp1FE;enum Cyc_Absyn_AliasQual _tmp1FF;enum Cyc_Absyn_AliasQual
_tmp200;enum Cyc_Absyn_AliasQual _tmp201;_LLC6: _tmp1FE=_tmp1FD.f1;if(_tmp1FE != 
Cyc_Absyn_Aliasable)goto _LLC8;_tmp1FF=_tmp1FD.f2;if(_tmp1FF != Cyc_Absyn_Top)goto
_LLC8;_LLC7: goto _LLC9;_LLC8: _tmp200=_tmp1FD.f1;if(_tmp200 != Cyc_Absyn_Unique)
goto _LLCA;_tmp201=_tmp1FD.f2;if(_tmp201 != Cyc_Absyn_Top)goto _LLCA;_LLC9: return 1;
_LLCA:;_LLCB: return 0;_LLC5:;}return 1;};}struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){void*_tmp202=Cyc_Absyn_compress_kb(
tv->kind);struct Cyc_Absyn_Kind*_tmp204;struct Cyc_Absyn_Kind*_tmp206;_LLCD: {
struct Cyc_Absyn_Eq_kb_struct*_tmp203=(struct Cyc_Absyn_Eq_kb_struct*)_tmp202;if(
_tmp203->tag != 0)goto _LLCF;else{_tmp204=_tmp203->f1;}}_LLCE: return _tmp204;_LLCF: {
struct Cyc_Absyn_Less_kb_struct*_tmp205=(struct Cyc_Absyn_Less_kb_struct*)_tmp202;
if(_tmp205->tag != 2)goto _LLD1;else{_tmp206=_tmp205->f2;}}_LLD0: return _tmp206;
_LLD1:;_LLD2:{struct Cyc_Absyn_Less_kb_struct _tmp1186;struct Cyc_Absyn_Less_kb_struct*
_tmp1185;tv->kind=(void*)((_tmp1185=_cycalloc(sizeof(*_tmp1185)),((_tmp1185[0]=((
_tmp1186.tag=2,((_tmp1186.f1=0,((_tmp1186.f2=def,_tmp1186)))))),_tmp1185))));}
return def;_LLCC:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp1187;struct _tuple0 _tmp20A=(_tmp1187.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1187.f2=
Cyc_Absyn_compress_kb(kb2),_tmp1187)));void*_tmp20B;struct Cyc_Absyn_Kind*_tmp20D;
void*_tmp20E;struct Cyc_Absyn_Kind*_tmp210;void*_tmp211;struct Cyc_Core_Opt*
_tmp213;struct Cyc_Core_Opt**_tmp214;struct Cyc_Absyn_Kind*_tmp215;void*_tmp216;
struct Cyc_Absyn_Kind*_tmp218;void*_tmp219;struct Cyc_Absyn_Kind*_tmp21B;void*
_tmp21C;struct Cyc_Core_Opt*_tmp21E;struct Cyc_Core_Opt**_tmp21F;struct Cyc_Absyn_Kind*
_tmp220;void*_tmp221;struct Cyc_Core_Opt*_tmp223;struct Cyc_Core_Opt**_tmp224;
struct Cyc_Absyn_Kind*_tmp225;void*_tmp226;struct Cyc_Core_Opt*_tmp228;struct Cyc_Core_Opt**
_tmp229;struct Cyc_Absyn_Kind*_tmp22A;void*_tmp22B;struct Cyc_Core_Opt*_tmp22D;
struct Cyc_Core_Opt**_tmp22E;void*_tmp22F;void*_tmp230;void*_tmp231;struct Cyc_Core_Opt*
_tmp233;struct Cyc_Core_Opt**_tmp234;_LLD4: _tmp20B=_tmp20A.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp20C=(struct Cyc_Absyn_Eq_kb_struct*)_tmp20B;if(_tmp20C->tag != 0)goto _LLD6;
else{_tmp20D=_tmp20C->f1;}};_tmp20E=_tmp20A.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp20F=(struct Cyc_Absyn_Eq_kb_struct*)_tmp20E;if(_tmp20F->tag != 0)goto _LLD6;
else{_tmp210=_tmp20F->f1;}};_LLD5: return _tmp20D == _tmp210;_LLD6: _tmp211=_tmp20A.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp212=(struct Cyc_Absyn_Less_kb_struct*)_tmp211;
if(_tmp212->tag != 2)goto _LLD8;else{_tmp213=_tmp212->f1;_tmp214=(struct Cyc_Core_Opt**)&
_tmp212->f1;_tmp215=_tmp212->f2;}};_tmp216=_tmp20A.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp217=(struct Cyc_Absyn_Eq_kb_struct*)_tmp216;if(_tmp217->tag != 0)goto _LLD8;
else{_tmp218=_tmp217->f1;}};_LLD7: if(Cyc_Tcutil_kind_leq(_tmp218,_tmp215)){{
struct Cyc_Core_Opt*_tmp1188;*_tmp214=((_tmp1188=_cycalloc(sizeof(*_tmp1188)),((
_tmp1188->v=kb2,_tmp1188))));}return 1;}else{return 0;}_LLD8: _tmp219=_tmp20A.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp21A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp219;if(
_tmp21A->tag != 0)goto _LLDA;else{_tmp21B=_tmp21A->f1;}};_tmp21C=_tmp20A.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21D=(struct Cyc_Absyn_Less_kb_struct*)_tmp21C;
if(_tmp21D->tag != 2)goto _LLDA;else{_tmp21E=_tmp21D->f1;_tmp21F=(struct Cyc_Core_Opt**)&
_tmp21D->f1;_tmp220=_tmp21D->f2;}};_LLD9: if(Cyc_Tcutil_kind_leq(_tmp21B,_tmp220)){{
struct Cyc_Core_Opt*_tmp1189;*_tmp21F=((_tmp1189=_cycalloc(sizeof(*_tmp1189)),((
_tmp1189->v=kb1,_tmp1189))));}return 1;}else{return 0;}_LLDA: _tmp221=_tmp20A.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp222=(struct Cyc_Absyn_Less_kb_struct*)_tmp221;
if(_tmp222->tag != 2)goto _LLDC;else{_tmp223=_tmp222->f1;_tmp224=(struct Cyc_Core_Opt**)&
_tmp222->f1;_tmp225=_tmp222->f2;}};_tmp226=_tmp20A.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp227=(struct Cyc_Absyn_Less_kb_struct*)_tmp226;if(_tmp227->tag != 2)goto _LLDC;
else{_tmp228=_tmp227->f1;_tmp229=(struct Cyc_Core_Opt**)& _tmp227->f1;_tmp22A=
_tmp227->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp225,_tmp22A)){{struct Cyc_Core_Opt*
_tmp118A;*_tmp229=((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((_tmp118A->v=kb1,
_tmp118A))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp22A,_tmp225)){{struct Cyc_Core_Opt*
_tmp118B;*_tmp224=((_tmp118B=_cycalloc(sizeof(*_tmp118B)),((_tmp118B->v=kb2,
_tmp118B))));}return 1;}else{return 0;}}_LLDC: _tmp22B=_tmp20A.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22B;if(_tmp22C->tag != 1)goto
_LLDE;else{_tmp22D=_tmp22C->f1;_tmp22E=(struct Cyc_Core_Opt**)& _tmp22C->f1;}};
_tmp22F=_tmp20A.f2;_LLDD: _tmp230=_tmp22F;_tmp234=_tmp22E;goto _LLDF;_LLDE: _tmp230=
_tmp20A.f1;_tmp231=_tmp20A.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp232=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp231;if(_tmp232->tag != 1)goto _LLD3;else{_tmp233=
_tmp232->f1;_tmp234=(struct Cyc_Core_Opt**)& _tmp232->f1;}};_LLDF:{struct Cyc_Core_Opt*
_tmp118C;*_tmp234=((_tmp118C=_cycalloc(sizeof(*_tmp118C)),((_tmp118C->v=_tmp230,
_tmp118C))));}return 1;_LLD3:;}struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp23A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23C;_LLE1: {struct
Cyc_Absyn_VarType_struct*_tmp23B=(struct Cyc_Absyn_VarType_struct*)_tmp23A;if(
_tmp23B->tag != 2)goto _LLE3;else{_tmp23C=_tmp23B->f1;}}_LLE2: {void*_tmp23D=
_tmp23C->kind;_tmp23C->kind=kb;{struct _tuple16 _tmp118D;return(_tmp118D.f1=
_tmp23C,((_tmp118D.f2=_tmp23D,_tmp118D)));};}_LLE3:;_LLE4: {const char*_tmp1191;
void*_tmp1190[1];struct Cyc_String_pa_struct _tmp118F;(_tmp118F.tag=0,((_tmp118F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1190[0]=& _tmp118F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1191="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp1191,sizeof(char),40))),_tag_dyneither(_tmp1190,sizeof(void*),
1)))))));}_LLE0:;}struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){void*_tmp242=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp244;struct Cyc_Absyn_Tvar*_tmp246;
enum Cyc_Absyn_Size_of _tmp249;struct Cyc_Absyn_DatatypeFieldInfo _tmp255;union Cyc_Absyn_DatatypeFieldInfoU
_tmp256;struct _tuple3 _tmp257;struct Cyc_Absyn_Datatypedecl*_tmp258;struct Cyc_Absyn_Datatypefield*
_tmp259;struct Cyc_Absyn_DatatypeFieldInfo _tmp25B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp25C;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp25D;struct Cyc_Absyn_AggrInfo
_tmp25F;union Cyc_Absyn_AggrInfoU _tmp260;struct _tuple4 _tmp261;struct Cyc_Absyn_AggrInfo
_tmp263;union Cyc_Absyn_AggrInfoU _tmp264;struct Cyc_Absyn_Aggrdecl**_tmp265;struct
Cyc_Absyn_Aggrdecl*_tmp266;struct Cyc_Absyn_Aggrdecl _tmp267;struct Cyc_List_List*
_tmp268;struct Cyc_Absyn_AggrdeclImpl*_tmp269;struct Cyc_List_List*_tmp26A;struct
Cyc_Absyn_PtrInfo _tmp26D;struct Cyc_Absyn_Typedefdecl*_tmp273;struct Cyc_Absyn_TypeDecl*
_tmp275;struct Cyc_Absyn_TypeDecl _tmp276;void*_tmp277;struct Cyc_Absyn_TypeDecl*
_tmp27A;struct Cyc_Absyn_TypeDecl _tmp27B;void*_tmp27C;struct Cyc_Absyn_TypeDecl*
_tmp27F;struct Cyc_Absyn_TypeDecl _tmp280;void*_tmp281;_LLE6: {struct Cyc_Absyn_Evar_struct*
_tmp243=(struct Cyc_Absyn_Evar_struct*)_tmp242;if(_tmp243->tag != 1)goto _LLE8;
else{_tmp244=_tmp243->f1;}}_LLE7: return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(_tmp244))->v;_LLE8: {struct Cyc_Absyn_VarType_struct*_tmp245=(struct
Cyc_Absyn_VarType_struct*)_tmp242;if(_tmp245->tag != 2)goto _LLEA;else{_tmp246=
_tmp245->f1;}}_LLE9: return Cyc_Tcutil_tvar_kind(_tmp246,& Cyc_Tcutil_bk);_LLEA: {
struct Cyc_Absyn_VoidType_struct*_tmp247=(struct Cyc_Absyn_VoidType_struct*)
_tmp242;if(_tmp247->tag != 0)goto _LLEC;}_LLEB: return& Cyc_Tcutil_mk;_LLEC: {struct
Cyc_Absyn_IntType_struct*_tmp248=(struct Cyc_Absyn_IntType_struct*)_tmp242;if(
_tmp248->tag != 6)goto _LLEE;else{_tmp249=_tmp248->f2;}}_LLED: return(_tmp249 == (
enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz  || _tmp249 == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?&
Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLEE: {struct Cyc_Absyn_FloatType_struct*_tmp24A=(
struct Cyc_Absyn_FloatType_struct*)_tmp242;if(_tmp24A->tag != 7)goto _LLF0;}_LLEF:
goto _LLF1;_LLF0: {struct Cyc_Absyn_FnType_struct*_tmp24B=(struct Cyc_Absyn_FnType_struct*)
_tmp242;if(_tmp24B->tag != 9)goto _LLF2;}_LLF1: return& Cyc_Tcutil_mk;_LLF2: {struct
Cyc_Absyn_DynRgnType_struct*_tmp24C=(struct Cyc_Absyn_DynRgnType_struct*)_tmp242;
if(_tmp24C->tag != 16)goto _LLF4;}_LLF3: goto _LLF5;_LLF4: {struct Cyc_Absyn_EnumType_struct*
_tmp24D=(struct Cyc_Absyn_EnumType_struct*)_tmp242;if(_tmp24D->tag != 13)goto _LLF6;}
_LLF5: goto _LLF7;_LLF6: {struct Cyc_Absyn_AnonEnumType_struct*_tmp24E=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp242;if(_tmp24E->tag != 14)goto _LLF8;}_LLF7: goto _LLF9;_LLF8: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp24F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp242;if(_tmp24F->tag != 15)goto
_LLFA;}_LLF9: return& Cyc_Tcutil_bk;_LLFA: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp250=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp242;if(_tmp250->tag != 21)goto
_LLFC;}_LLFB: return& Cyc_Tcutil_urk;_LLFC: {struct Cyc_Absyn_HeapRgn_struct*
_tmp251=(struct Cyc_Absyn_HeapRgn_struct*)_tmp242;if(_tmp251->tag != 20)goto _LLFE;}
_LLFD: return& Cyc_Tcutil_rk;_LLFE: {struct Cyc_Absyn_RefCntRgn_struct*_tmp252=(
struct Cyc_Absyn_RefCntRgn_struct*)_tmp242;if(_tmp252->tag != 22)goto _LL100;}_LLFF:
return& Cyc_Tcutil_trk;_LL100: {struct Cyc_Absyn_DatatypeType_struct*_tmp253=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp242;if(_tmp253->tag != 3)goto _LL102;}
_LL101: return& Cyc_Tcutil_ak;_LL102: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp254=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp242;if(_tmp254->tag != 4)
goto _LL104;else{_tmp255=_tmp254->f1;_tmp256=_tmp255.field_info;if((_tmp256.KnownDatatypefield).tag
!= 2)goto _LL104;_tmp257=(struct _tuple3)(_tmp256.KnownDatatypefield).val;_tmp258=
_tmp257.f1;_tmp259=_tmp257.f2;}}_LL103: return& Cyc_Tcutil_mk;_LL104: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp25A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp242;if(_tmp25A->tag != 4)
goto _LL106;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.field_info;if((_tmp25C.UnknownDatatypefield).tag
!= 1)goto _LL106;_tmp25D=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp25C.UnknownDatatypefield).val;}}
_LL105: {const char*_tmp1194;void*_tmp1193;(_tmp1193=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1194="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp1194,sizeof(char),39))),_tag_dyneither(_tmp1193,sizeof(void*),
0)));}_LL106: {struct Cyc_Absyn_AggrType_struct*_tmp25E=(struct Cyc_Absyn_AggrType_struct*)
_tmp242;if(_tmp25E->tag != 11)goto _LL108;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25F.aggr_info;
if((_tmp260.UnknownAggr).tag != 1)goto _LL108;_tmp261=(struct _tuple4)(_tmp260.UnknownAggr).val;}}
_LL107: return& Cyc_Tcutil_ak;_LL108: {struct Cyc_Absyn_AggrType_struct*_tmp262=(
struct Cyc_Absyn_AggrType_struct*)_tmp242;if(_tmp262->tag != 11)goto _LL10A;else{
_tmp263=_tmp262->f1;_tmp264=_tmp263.aggr_info;if((_tmp264.KnownAggr).tag != 2)
goto _LL10A;_tmp265=(struct Cyc_Absyn_Aggrdecl**)(_tmp264.KnownAggr).val;_tmp266=*
_tmp265;_tmp267=*_tmp266;_tmp268=_tmp267.tvs;_tmp269=_tmp267.impl;_tmp26A=
_tmp263.targs;}}_LL109: if(_tmp269 == 0)return& Cyc_Tcutil_ak;{struct Cyc_List_List*
_tmp288=_tmp269->fields;if(_tmp288 == 0)return& Cyc_Tcutil_mk;for(0;_tmp288->tl != 
0;_tmp288=_tmp288->tl){;}{void*_tmp289=((struct Cyc_Absyn_Aggrfield*)_tmp288->hd)->type;
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(_tmp289);if(_tmp26A != 0  && (k == & Cyc_Tcutil_ak
 || k == & Cyc_Tcutil_tak)){struct _RegionHandle _tmp28A=_new_region("temp");struct
_RegionHandle*temp=& _tmp28A;_push_region(temp);{struct Cyc_List_List*_tmp28B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp268,_tmp26A);_tmp289=Cyc_Tcutil_rsubstitute(
temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp268,
_tmp26A),_tmp289);k=Cyc_Tcutil_typ_kind(_tmp289);};_pop_region(temp);}if(k == &
Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)return k;return& Cyc_Tcutil_mk;};};_LL10A: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp26B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp242;if(_tmp26B->tag != 12)goto _LL10C;}_LL10B: return& Cyc_Tcutil_mk;_LL10C: {
struct Cyc_Absyn_PointerType_struct*_tmp26C=(struct Cyc_Absyn_PointerType_struct*)
_tmp242;if(_tmp26C->tag != 5)goto _LL10E;else{_tmp26D=_tmp26C->f1;}}_LL10D: {void*
_tmp28C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp26D.ptr_atts).bounds);_LL125: {struct Cyc_Absyn_DynEither_b_struct*
_tmp28D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp28C;if(_tmp28D->tag != 0)goto
_LL127;}_LL126: {struct Cyc_Absyn_Kind*_tmp1195;return(_tmp1195=_cycalloc_atomic(
sizeof(*_tmp1195)),((_tmp1195->kind=Cyc_Absyn_MemKind,((_tmp1195->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp1195)))));}_LL127: {struct Cyc_Absyn_Upper_b_struct*
_tmp28E=(struct Cyc_Absyn_Upper_b_struct*)_tmp28C;if(_tmp28E->tag != 1)goto _LL124;}
_LL128: {struct Cyc_Absyn_Kind*_tmp1196;return(_tmp1196=_cycalloc_atomic(sizeof(*
_tmp1196)),((_tmp1196->kind=Cyc_Absyn_BoxKind,((_tmp1196->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp1196)))));}_LL124:;}_LL10E: {struct Cyc_Absyn_ValueofType_struct*
_tmp26E=(struct Cyc_Absyn_ValueofType_struct*)_tmp242;if(_tmp26E->tag != 18)goto
_LL110;}_LL10F: return& Cyc_Tcutil_ik;_LL110: {struct Cyc_Absyn_TagType_struct*
_tmp26F=(struct Cyc_Absyn_TagType_struct*)_tmp242;if(_tmp26F->tag != 19)goto _LL112;}
_LL111: return& Cyc_Tcutil_bk;_LL112: {struct Cyc_Absyn_ArrayType_struct*_tmp270=(
struct Cyc_Absyn_ArrayType_struct*)_tmp242;if(_tmp270->tag != 8)goto _LL114;}_LL113:
goto _LL115;_LL114: {struct Cyc_Absyn_TupleType_struct*_tmp271=(struct Cyc_Absyn_TupleType_struct*)
_tmp242;if(_tmp271->tag != 10)goto _LL116;}_LL115: return& Cyc_Tcutil_mk;_LL116: {
struct Cyc_Absyn_TypedefType_struct*_tmp272=(struct Cyc_Absyn_TypedefType_struct*)
_tmp242;if(_tmp272->tag != 17)goto _LL118;else{_tmp273=_tmp272->f3;}}_LL117: if(
_tmp273 == 0  || _tmp273->kind == 0){const char*_tmp119A;void*_tmp1199[1];struct Cyc_String_pa_struct
_tmp1198;(_tmp1198.tag=0,((_tmp1198.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1199[0]=& _tmp1198,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp119A="typ_kind: typedef found: %s",
_tag_dyneither(_tmp119A,sizeof(char),28))),_tag_dyneither(_tmp1199,sizeof(void*),
1)))))));}return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp273->kind))->v;
_LL118: {struct Cyc_Absyn_TypeDeclType_struct*_tmp274=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp242;if(_tmp274->tag != 26)goto _LL11A;else{_tmp275=_tmp274->f1;_tmp276=*
_tmp275;_tmp277=_tmp276.r;{struct Cyc_Absyn_Aggr_td_struct*_tmp278=(struct Cyc_Absyn_Aggr_td_struct*)
_tmp277;if(_tmp278->tag != 0)goto _LL11A;};}}_LL119: return& Cyc_Tcutil_ak;_LL11A: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp279=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp242;if(_tmp279->tag != 26)goto _LL11C;else{_tmp27A=_tmp279->f1;_tmp27B=*
_tmp27A;_tmp27C=_tmp27B.r;{struct Cyc_Absyn_Enum_td_struct*_tmp27D=(struct Cyc_Absyn_Enum_td_struct*)
_tmp27C;if(_tmp27D->tag != 1)goto _LL11C;};}}_LL11B: return& Cyc_Tcutil_bk;_LL11C: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp27E=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp242;if(_tmp27E->tag != 26)goto _LL11E;else{_tmp27F=_tmp27E->f1;_tmp280=*
_tmp27F;_tmp281=_tmp280.r;{struct Cyc_Absyn_Datatype_td_struct*_tmp282=(struct Cyc_Absyn_Datatype_td_struct*)
_tmp281;if(_tmp282->tag != 2)goto _LL11E;};}}_LL11D: return& Cyc_Tcutil_ak;_LL11E: {
struct Cyc_Absyn_AccessEff_struct*_tmp283=(struct Cyc_Absyn_AccessEff_struct*)
_tmp242;if(_tmp283->tag != 23)goto _LL120;}_LL11F: goto _LL121;_LL120: {struct Cyc_Absyn_JoinEff_struct*
_tmp284=(struct Cyc_Absyn_JoinEff_struct*)_tmp242;if(_tmp284->tag != 24)goto _LL122;}
_LL121: goto _LL123;_LL122: {struct Cyc_Absyn_RgnsEff_struct*_tmp285=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp242;if(_tmp285->tag != 25)goto _LLE5;}_LL123: return& Cyc_Tcutil_ek;_LLE5:;}int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){return k1 == 
k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp294;_push_handler(& _tmp294);{int _tmp296=
0;if(setjmp(_tmp294.handler))_tmp296=1;if(!_tmp296){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp297=1;_npop_handler(0);return _tmp297;};;_pop_handler();}else{void*_tmp295=(
void*)_exn_thrown;void*_tmp299=_tmp295;_LL12A: {struct Cyc_Tcutil_Unify_struct*
_tmp29A=(struct Cyc_Tcutil_Unify_struct*)_tmp299;if(_tmp29A->tag != Cyc_Tcutil_Unify)
goto _LL12C;}_LL12B: return 0;_LL12C:;_LL12D:(void)_throw(_tmp299);_LL129:;}};}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*
_tmp29B=t;struct Cyc_Absyn_Tvar*_tmp29D;struct Cyc_Core_Opt*_tmp29F;struct Cyc_Core_Opt*
_tmp2A0;struct Cyc_Core_Opt**_tmp2A1;struct Cyc_Absyn_PtrInfo _tmp2A3;struct Cyc_Absyn_ArrayInfo
_tmp2A5;void*_tmp2A6;struct Cyc_Absyn_FnInfo _tmp2A8;struct Cyc_List_List*_tmp2A9;
struct Cyc_Core_Opt*_tmp2AA;struct Cyc_Absyn_Tqual _tmp2AB;void*_tmp2AC;struct Cyc_List_List*
_tmp2AD;int _tmp2AE;struct Cyc_Absyn_VarargInfo*_tmp2AF;struct Cyc_List_List*
_tmp2B0;struct Cyc_List_List*_tmp2B1;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_DatatypeInfo
_tmp2B5;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_DatatypeFieldInfo
_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_Absyn_AggrInfo _tmp2BD;struct Cyc_List_List*
_tmp2BE;struct Cyc_List_List*_tmp2C0;void*_tmp2C2;void*_tmp2C4;void*_tmp2C6;void*
_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL12F: {struct Cyc_Absyn_VarType_struct*
_tmp29C=(struct Cyc_Absyn_VarType_struct*)_tmp29B;if(_tmp29C->tag != 2)goto _LL131;
else{_tmp29D=_tmp29C->f1;}}_LL130: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmp29D)){{const char*_tmp119B;Cyc_Tcutil_failure_reason=((
_tmp119B="(type variable would escape scope)",_tag_dyneither(_tmp119B,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL12E;_LL131: {
struct Cyc_Absyn_Evar_struct*_tmp29E=(struct Cyc_Absyn_Evar_struct*)_tmp29B;if(
_tmp29E->tag != 1)goto _LL133;else{_tmp29F=_tmp29E->f2;_tmp2A0=_tmp29E->f4;_tmp2A1=(
struct Cyc_Core_Opt**)& _tmp29E->f4;}}_LL132: if(t == evar){{const char*_tmp119C;Cyc_Tcutil_failure_reason=((
_tmp119C="(occurs check)",_tag_dyneither(_tmp119C,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp29F != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp29F->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp2A1))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2CD=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A1))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp119D;_tmp2CD=((
_tmp119D=_cycalloc(sizeof(*_tmp119D)),((_tmp119D->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp119D->tl=_tmp2CD,_tmp119D))))));}}}{struct Cyc_Core_Opt*_tmp119E;*_tmp2A1=((
_tmp119E=_cycalloc(sizeof(*_tmp119E)),((_tmp119E->v=_tmp2CD,_tmp119E))));};}}}
goto _LL12E;_LL133: {struct Cyc_Absyn_PointerType_struct*_tmp2A2=(struct Cyc_Absyn_PointerType_struct*)
_tmp29B;if(_tmp2A2->tag != 5)goto _LL135;else{_tmp2A3=_tmp2A2->f1;}}_LL134: Cyc_Tcutil_occurs(
evar,r,env,_tmp2A3.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(_tmp2A3.ptr_atts).rgn);
goto _LL12E;_LL135: {struct Cyc_Absyn_ArrayType_struct*_tmp2A4=(struct Cyc_Absyn_ArrayType_struct*)
_tmp29B;if(_tmp2A4->tag != 8)goto _LL137;else{_tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A5.elt_type;}}
_LL136: Cyc_Tcutil_occurs(evar,r,env,_tmp2A6);goto _LL12E;_LL137: {struct Cyc_Absyn_FnType_struct*
_tmp2A7=(struct Cyc_Absyn_FnType_struct*)_tmp29B;if(_tmp2A7->tag != 9)goto _LL139;
else{_tmp2A8=_tmp2A7->f1;_tmp2A9=_tmp2A8.tvars;_tmp2AA=_tmp2A8.effect;_tmp2AB=
_tmp2A8.ret_tqual;_tmp2AC=_tmp2A8.ret_typ;_tmp2AD=_tmp2A8.args;_tmp2AE=_tmp2A8.c_varargs;
_tmp2AF=_tmp2A8.cyc_varargs;_tmp2B0=_tmp2A8.rgn_po;_tmp2B1=_tmp2A8.attributes;}}
_LL138: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp2A9,env);if(_tmp2AA != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp2AA->v);Cyc_Tcutil_occurs(evar,r,env,_tmp2AC);for(0;
_tmp2AD != 0;_tmp2AD=_tmp2AD->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)
_tmp2AD->hd)).f3);}if(_tmp2AF != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp2AF->type);
for(0;_tmp2B0 != 0;_tmp2B0=_tmp2B0->tl){struct _tuple0 _tmp2D1;void*_tmp2D2;void*
_tmp2D3;struct _tuple0*_tmp2D0=(struct _tuple0*)_tmp2B0->hd;_tmp2D1=*_tmp2D0;
_tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;Cyc_Tcutil_occurs(evar,r,env,_tmp2D2);Cyc_Tcutil_occurs(
evar,r,env,_tmp2D3);}goto _LL12E;_LL139: {struct Cyc_Absyn_TupleType_struct*
_tmp2B2=(struct Cyc_Absyn_TupleType_struct*)_tmp29B;if(_tmp2B2->tag != 10)goto
_LL13B;else{_tmp2B3=_tmp2B2->f1;}}_LL13A: for(0;_tmp2B3 != 0;_tmp2B3=_tmp2B3->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp2B3->hd)).f2);}goto _LL12E;
_LL13B: {struct Cyc_Absyn_DatatypeType_struct*_tmp2B4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp29B;if(_tmp2B4->tag != 3)goto _LL13D;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B5.targs;}}
_LL13C: Cyc_Tcutil_occurslist(evar,r,env,_tmp2B6);goto _LL12E;_LL13D: {struct Cyc_Absyn_TypedefType_struct*
_tmp2B7=(struct Cyc_Absyn_TypedefType_struct*)_tmp29B;if(_tmp2B7->tag != 17)goto
_LL13F;else{_tmp2B8=_tmp2B7->f2;}}_LL13E: _tmp2BB=_tmp2B8;goto _LL140;_LL13F: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2B9=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp29B;if(_tmp2B9->tag != 4)goto _LL141;else{_tmp2BA=_tmp2B9->f1;_tmp2BB=_tmp2BA.targs;}}
_LL140: _tmp2BE=_tmp2BB;goto _LL142;_LL141: {struct Cyc_Absyn_AggrType_struct*
_tmp2BC=(struct Cyc_Absyn_AggrType_struct*)_tmp29B;if(_tmp2BC->tag != 11)goto
_LL143;else{_tmp2BD=_tmp2BC->f1;_tmp2BE=_tmp2BD.targs;}}_LL142: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2BE);goto _LL12E;_LL143: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2BF=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp29B;if(_tmp2BF->tag != 12)goto
_LL145;else{_tmp2C0=_tmp2BF->f2;}}_LL144: for(0;_tmp2C0 != 0;_tmp2C0=_tmp2C0->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp2C0->hd)->type);}
goto _LL12E;_LL145: {struct Cyc_Absyn_TagType_struct*_tmp2C1=(struct Cyc_Absyn_TagType_struct*)
_tmp29B;if(_tmp2C1->tag != 19)goto _LL147;else{_tmp2C2=(void*)_tmp2C1->f1;}}_LL146:
_tmp2C4=_tmp2C2;goto _LL148;_LL147: {struct Cyc_Absyn_AccessEff_struct*_tmp2C3=(
struct Cyc_Absyn_AccessEff_struct*)_tmp29B;if(_tmp2C3->tag != 23)goto _LL149;else{
_tmp2C4=(void*)_tmp2C3->f1;}}_LL148: _tmp2C6=_tmp2C4;goto _LL14A;_LL149: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp2C5=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp29B;if(_tmp2C5->tag != 15)goto _LL14B;else{_tmp2C6=(void*)_tmp2C5->f1;}}_LL14A:
_tmp2C8=_tmp2C6;goto _LL14C;_LL14B: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C7=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp29B;if(_tmp2C7->tag != 25)goto _LL14D;else{
_tmp2C8=(void*)_tmp2C7->f1;}}_LL14C: Cyc_Tcutil_occurs(evar,r,env,_tmp2C8);goto
_LL12E;_LL14D: {struct Cyc_Absyn_JoinEff_struct*_tmp2C9=(struct Cyc_Absyn_JoinEff_struct*)
_tmp29B;if(_tmp2C9->tag != 24)goto _LL14F;else{_tmp2CA=_tmp2C9->f1;}}_LL14E: Cyc_Tcutil_occurslist(
evar,r,env,_tmp2CA);goto _LL12E;_LL14F:;_LL150: goto _LL12E;_LL12E:;};}static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const char*_tmp11A1;void*
_tmp11A0;(_tmp11A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11A1="tq1 real_const not set.",_tag_dyneither(_tmp11A1,
sizeof(char),24))),_tag_dyneither(_tmp11A0,sizeof(void*),0)));}if(tq2.print_const
 && !tq2.real_const){const char*_tmp11A4;void*_tmp11A3;(_tmp11A3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A4="tq2 real_const not set.",
_tag_dyneither(_tmp11A4,sizeof(char),24))),_tag_dyneither(_tmp11A3,sizeof(void*),
0)));}if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile)
 || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=tq2.real_const;{
const char*_tmp11A5;Cyc_Tcutil_failure_reason=((_tmp11A5="(qualifiers don't match)",
_tag_dyneither(_tmp11A5,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const
 && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static
void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return;{union Cyc_Absyn_Constraint*
_tmp2D9=x;union Cyc_Absyn_Constraint _tmp2DA;int _tmp2DB;union Cyc_Absyn_Constraint
_tmp2DC;void*_tmp2DD;union Cyc_Absyn_Constraint _tmp2DE;union Cyc_Absyn_Constraint*
_tmp2DF;_LL152: _tmp2DA=*_tmp2D9;if((_tmp2DA.No_constr).tag != 3)goto _LL154;
_tmp2DB=(int)(_tmp2DA.No_constr).val;_LL153:{union Cyc_Absyn_Constraint _tmp11A6;*
x=(((_tmp11A6.Forward_constr).val=y,(((_tmp11A6.Forward_constr).tag=2,_tmp11A6))));}
return;_LL154: _tmp2DC=*_tmp2D9;if((_tmp2DC.Eq_constr).tag != 1)goto _LL156;_tmp2DD=(
void*)(_tmp2DC.Eq_constr).val;_LL155: {union Cyc_Absyn_Constraint*_tmp2E1=y;union
Cyc_Absyn_Constraint _tmp2E2;int _tmp2E3;union Cyc_Absyn_Constraint _tmp2E4;void*
_tmp2E5;union Cyc_Absyn_Constraint _tmp2E6;union Cyc_Absyn_Constraint*_tmp2E7;
_LL159: _tmp2E2=*_tmp2E1;if((_tmp2E2.No_constr).tag != 3)goto _LL15B;_tmp2E3=(int)(
_tmp2E2.No_constr).val;_LL15A:*y=*x;return;_LL15B: _tmp2E4=*_tmp2E1;if((_tmp2E4.Eq_constr).tag
!= 1)goto _LL15D;_tmp2E5=(void*)(_tmp2E4.Eq_constr).val;_LL15C: if(cmp(_tmp2DD,
_tmp2E5)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
return;_LL15D: _tmp2E6=*_tmp2E1;if((_tmp2E6.Forward_constr).tag != 2)goto _LL158;
_tmp2E7=(union Cyc_Absyn_Constraint*)(_tmp2E6.Forward_constr).val;_LL15E: {const
char*_tmp11A9;void*_tmp11A8;(_tmp11A8=0,Cyc_Tcutil_impos(((_tmp11A9="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11A9,sizeof(char),40))),_tag_dyneither(_tmp11A8,sizeof(void*),
0)));}_LL158:;}_LL156: _tmp2DE=*_tmp2D9;if((_tmp2DE.Forward_constr).tag != 2)goto
_LL151;_tmp2DF=(union Cyc_Absyn_Constraint*)(_tmp2DE.Forward_constr).val;_LL157: {
const char*_tmp11AC;void*_tmp11AB;(_tmp11AB=0,Cyc_Tcutil_impos(((_tmp11AC="unify_conref: forward after compress",
_tag_dyneither(_tmp11AC,sizeof(char),37))),_tag_dyneither(_tmp11AB,sizeof(void*),
0)));}_LL151:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2EC;
_push_handler(& _tmp2EC);{int _tmp2EE=0;if(setjmp(_tmp2EC.handler))_tmp2EE=1;if(!
_tmp2EE){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2EF=1;_npop_handler(0);return _tmp2EF;};;
_pop_handler();}else{void*_tmp2ED=(void*)_exn_thrown;void*_tmp2F1=_tmp2ED;_LL160: {
struct Cyc_Tcutil_Unify_struct*_tmp2F2=(struct Cyc_Tcutil_Unify_struct*)_tmp2F1;
if(_tmp2F2->tag != Cyc_Tcutil_Unify)goto _LL162;}_LL161: return 0;_LL162:;_LL163:(
void)_throw(_tmp2F1);_LL15F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11AD;struct _tuple0 _tmp2F4=(_tmp11AD.f1=b1,((_tmp11AD.f2=b2,
_tmp11AD)));void*_tmp2F5;void*_tmp2F7;void*_tmp2F9;void*_tmp2FB;void*_tmp2FD;
struct Cyc_Absyn_Exp*_tmp2FF;void*_tmp300;struct Cyc_Absyn_Exp*_tmp302;_LL165:
_tmp2F5=_tmp2F4.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp2F6=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp2F5;if(_tmp2F6->tag != 0)goto _LL167;};_tmp2F7=_tmp2F4.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2F8=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F7;if(_tmp2F8->tag != 0)goto
_LL167;};_LL166: return 0;_LL167: _tmp2F9=_tmp2F4.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2FA=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2F9;if(_tmp2FA->tag != 0)goto
_LL169;};_LL168: return - 1;_LL169: _tmp2FB=_tmp2F4.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp2FC=(struct Cyc_Absyn_DynEither_b_struct*)_tmp2FB;if(_tmp2FC->tag != 0)goto
_LL16B;};_LL16A: return 1;_LL16B: _tmp2FD=_tmp2F4.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp2FE=(struct Cyc_Absyn_Upper_b_struct*)_tmp2FD;if(_tmp2FE->tag != 1)goto _LL164;
else{_tmp2FF=_tmp2FE->f1;}};_tmp300=_tmp2F4.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp301=(struct Cyc_Absyn_Upper_b_struct*)_tmp300;if(_tmp301->tag != 1)goto _LL164;
else{_tmp302=_tmp301->f1;}};_LL16C: return Cyc_Evexp_const_exp_cmp(_tmp2FF,_tmp302);
_LL164:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){struct _tuple0
_tmp11AE;struct _tuple0 _tmp304=(_tmp11AE.f1=b1,((_tmp11AE.f2=b2,_tmp11AE)));void*
_tmp305;void*_tmp307;void*_tmp309;void*_tmp30B;void*_tmp30D;struct Cyc_Absyn_Exp*
_tmp30F;void*_tmp310;struct Cyc_Absyn_Exp*_tmp312;_LL16E: _tmp305=_tmp304.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp306=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp305;if(_tmp306->tag != 0)goto _LL170;};_tmp307=_tmp304.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp308=(struct Cyc_Absyn_DynEither_b_struct*)_tmp307;if(_tmp308->tag != 0)goto
_LL170;};_LL16F: return 0;_LL170: _tmp309=_tmp304.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp30A=(struct Cyc_Absyn_DynEither_b_struct*)_tmp309;if(_tmp30A->tag != 0)goto
_LL172;};_LL171: return - 1;_LL172: _tmp30B=_tmp304.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp30C=(struct Cyc_Absyn_DynEither_b_struct*)_tmp30B;if(_tmp30C->tag != 0)goto
_LL174;};_LL173: return 1;_LL174: _tmp30D=_tmp304.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp30E=(struct Cyc_Absyn_Upper_b_struct*)_tmp30D;if(_tmp30E->tag != 1)goto _LL16D;
else{_tmp30F=_tmp30E->f1;}};_tmp310=_tmp304.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp311=(struct Cyc_Absyn_Upper_b_struct*)_tmp310;if(_tmp311->tag != 1)goto _LL16D;
else{_tmp312=_tmp311->f1;}};_LL175: return Cyc_Evexp_const_exp_cmp(_tmp30F,_tmp312);
_LL16D:;}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp313=att;
_LL177: {struct Cyc_Absyn_Regparm_att_struct*_tmp314=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp313;if(_tmp314->tag != 0)goto _LL179;}_LL178: return 0;_LL179: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp315=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp313;if(_tmp315->tag != 1)goto
_LL17B;}_LL17A: return 1;_LL17B: {struct Cyc_Absyn_Cdecl_att_struct*_tmp316=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp313;if(_tmp316->tag != 2)goto _LL17D;}_LL17C:
return 2;_LL17D: {struct Cyc_Absyn_Fastcall_att_struct*_tmp317=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp313;if(_tmp317->tag != 3)goto _LL17F;}_LL17E: return 3;_LL17F: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp318=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp313;if(_tmp318->tag != 4)goto
_LL181;}_LL180: return 4;_LL181: {struct Cyc_Absyn_Const_att_struct*_tmp319=(struct
Cyc_Absyn_Const_att_struct*)_tmp313;if(_tmp319->tag != 5)goto _LL183;}_LL182:
return 5;_LL183: {struct Cyc_Absyn_Aligned_att_struct*_tmp31A=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp313;if(_tmp31A->tag != 6)goto _LL185;}_LL184: return 6;_LL185: {struct Cyc_Absyn_Packed_att_struct*
_tmp31B=(struct Cyc_Absyn_Packed_att_struct*)_tmp313;if(_tmp31B->tag != 7)goto
_LL187;}_LL186: return 7;_LL187: {struct Cyc_Absyn_Section_att_struct*_tmp31C=(
struct Cyc_Absyn_Section_att_struct*)_tmp313;if(_tmp31C->tag != 8)goto _LL189;}
_LL188: return 8;_LL189: {struct Cyc_Absyn_Nocommon_att_struct*_tmp31D=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp313;if(_tmp31D->tag != 9)goto _LL18B;}_LL18A: return 9;_LL18B: {struct Cyc_Absyn_Shared_att_struct*
_tmp31E=(struct Cyc_Absyn_Shared_att_struct*)_tmp313;if(_tmp31E->tag != 10)goto
_LL18D;}_LL18C: return 10;_LL18D: {struct Cyc_Absyn_Unused_att_struct*_tmp31F=(
struct Cyc_Absyn_Unused_att_struct*)_tmp313;if(_tmp31F->tag != 11)goto _LL18F;}
_LL18E: return 11;_LL18F: {struct Cyc_Absyn_Weak_att_struct*_tmp320=(struct Cyc_Absyn_Weak_att_struct*)
_tmp313;if(_tmp320->tag != 12)goto _LL191;}_LL190: return 12;_LL191: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp321=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp313;if(_tmp321->tag != 13)goto
_LL193;}_LL192: return 13;_LL193: {struct Cyc_Absyn_Dllexport_att_struct*_tmp322=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp313;if(_tmp322->tag != 14)goto _LL195;}
_LL194: return 14;_LL195: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp323=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp313;if(_tmp323->tag
!= 15)goto _LL197;}_LL196: return 15;_LL197: {struct Cyc_Absyn_Constructor_att_struct*
_tmp324=(struct Cyc_Absyn_Constructor_att_struct*)_tmp313;if(_tmp324->tag != 16)
goto _LL199;}_LL198: return 16;_LL199: {struct Cyc_Absyn_Destructor_att_struct*
_tmp325=(struct Cyc_Absyn_Destructor_att_struct*)_tmp313;if(_tmp325->tag != 17)
goto _LL19B;}_LL19A: return 17;_LL19B: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp326=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp313;if(_tmp326->tag
!= 18)goto _LL19D;}_LL19C: return 18;_LL19D: {struct Cyc_Absyn_Format_att_struct*
_tmp327=(struct Cyc_Absyn_Format_att_struct*)_tmp313;if(_tmp327->tag != 19)goto
_LL19F;}_LL19E: return 19;_LL19F: {struct Cyc_Absyn_Initializes_att_struct*_tmp328=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp313;if(_tmp328->tag != 20)goto _LL1A1;}
_LL1A0: return 20;_LL1A1:;_LL1A2: return 21;_LL176:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2){struct _tuple0 _tmp11AF;struct _tuple0 _tmp32A=(_tmp11AF.f1=
att1,((_tmp11AF.f2=att2,_tmp11AF)));void*_tmp32B;int _tmp32D;void*_tmp32E;int
_tmp330;void*_tmp331;int _tmp333;void*_tmp334;int _tmp336;void*_tmp337;int _tmp339;
void*_tmp33A;int _tmp33C;void*_tmp33D;struct _dyneither_ptr _tmp33F;void*_tmp340;
struct _dyneither_ptr _tmp342;void*_tmp343;enum Cyc_Absyn_Format_Type _tmp345;int
_tmp346;int _tmp347;void*_tmp348;enum Cyc_Absyn_Format_Type _tmp34A;int _tmp34B;int
_tmp34C;_LL1A4: _tmp32B=_tmp32A.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp32C=(
struct Cyc_Absyn_Regparm_att_struct*)_tmp32B;if(_tmp32C->tag != 0)goto _LL1A6;else{
_tmp32D=_tmp32C->f1;}};_tmp32E=_tmp32A.f2;{struct Cyc_Absyn_Regparm_att_struct*
_tmp32F=(struct Cyc_Absyn_Regparm_att_struct*)_tmp32E;if(_tmp32F->tag != 0)goto
_LL1A6;else{_tmp330=_tmp32F->f1;}};_LL1A5: _tmp333=_tmp32D;_tmp336=_tmp330;goto
_LL1A7;_LL1A6: _tmp331=_tmp32A.f1;{struct Cyc_Absyn_Initializes_att_struct*_tmp332=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp331;if(_tmp332->tag != 20)goto _LL1A8;
else{_tmp333=_tmp332->f1;}};_tmp334=_tmp32A.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp335=(struct Cyc_Absyn_Initializes_att_struct*)_tmp334;if(_tmp335->tag != 20)
goto _LL1A8;else{_tmp336=_tmp335->f1;}};_LL1A7: _tmp339=_tmp333;_tmp33C=_tmp336;
goto _LL1A9;_LL1A8: _tmp337=_tmp32A.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp338=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp337;if(_tmp338->tag != 6)goto _LL1AA;else{
_tmp339=_tmp338->f1;}};_tmp33A=_tmp32A.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp33B=(struct Cyc_Absyn_Aligned_att_struct*)_tmp33A;if(_tmp33B->tag != 6)goto
_LL1AA;else{_tmp33C=_tmp33B->f1;}};_LL1A9: return Cyc_Core_intcmp(_tmp339,_tmp33C);
_LL1AA: _tmp33D=_tmp32A.f1;{struct Cyc_Absyn_Section_att_struct*_tmp33E=(struct Cyc_Absyn_Section_att_struct*)
_tmp33D;if(_tmp33E->tag != 8)goto _LL1AC;else{_tmp33F=_tmp33E->f1;}};_tmp340=
_tmp32A.f2;{struct Cyc_Absyn_Section_att_struct*_tmp341=(struct Cyc_Absyn_Section_att_struct*)
_tmp340;if(_tmp341->tag != 8)goto _LL1AC;else{_tmp342=_tmp341->f1;}};_LL1AB: return
Cyc_strcmp((struct _dyneither_ptr)_tmp33F,(struct _dyneither_ptr)_tmp342);_LL1AC:
_tmp343=_tmp32A.f1;{struct Cyc_Absyn_Format_att_struct*_tmp344=(struct Cyc_Absyn_Format_att_struct*)
_tmp343;if(_tmp344->tag != 19)goto _LL1AE;else{_tmp345=_tmp344->f1;_tmp346=_tmp344->f2;
_tmp347=_tmp344->f3;}};_tmp348=_tmp32A.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp349=(struct Cyc_Absyn_Format_att_struct*)_tmp348;if(_tmp349->tag != 19)goto
_LL1AE;else{_tmp34A=_tmp349->f1;_tmp34B=_tmp349->f2;_tmp34C=_tmp349->f3;}};
_LL1AD: {int _tmp34D=Cyc_Core_intcmp((int)((unsigned int)_tmp345),(int)((
unsigned int)_tmp34A));if(_tmp34D != 0)return _tmp34D;{int _tmp34E=Cyc_Core_intcmp(
_tmp346,_tmp34B);if(_tmp34E != 0)return _tmp34E;return Cyc_Core_intcmp(_tmp347,
_tmp34C);};}_LL1AE:;_LL1AF: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL1A3:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),void*env,struct
Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return
0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{struct Cyc_Absyn_Kind*
_tmp34F=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind _tmp350;
enum Cyc_Absyn_KindQual _tmp351;enum Cyc_Absyn_AliasQual _tmp352;struct Cyc_Absyn_Kind
_tmp353;enum Cyc_Absyn_KindQual _tmp354;enum Cyc_Absyn_AliasQual _tmp355;struct Cyc_Absyn_Kind
_tmp356;enum Cyc_Absyn_KindQual _tmp357;struct Cyc_Absyn_Kind _tmp358;enum Cyc_Absyn_KindQual
_tmp359;_LL1B1: _tmp350=*_tmp34F;_tmp351=_tmp350.kind;if(_tmp351 != Cyc_Absyn_RgnKind)
goto _LL1B3;_tmp352=_tmp350.aliasqual;if(_tmp352 != Cyc_Absyn_Unique)goto _LL1B3;
_LL1B2: t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1B0;_LL1B3: _tmp353=*_tmp34F;
_tmp354=_tmp353.kind;if(_tmp354 != Cyc_Absyn_RgnKind)goto _LL1B5;_tmp355=_tmp353.aliasqual;
if(_tmp355 != Cyc_Absyn_Aliasable)goto _LL1B5;_LL1B4: t=(void*)& Cyc_Absyn_HeapRgn_val;
goto _LL1B0;_LL1B5: _tmp356=*_tmp34F;_tmp357=_tmp356.kind;if(_tmp357 != Cyc_Absyn_EffKind)
goto _LL1B7;_LL1B6: t=Cyc_Absyn_empty_effect;goto _LL1B0;_LL1B7: _tmp358=*_tmp34F;
_tmp359=_tmp358.kind;if(_tmp359 != Cyc_Absyn_IntKind)goto _LL1B9;_LL1B8:{struct Cyc_Absyn_ValueofType_struct
_tmp11B2;struct Cyc_Absyn_ValueofType_struct*_tmp11B1;t=(void*)((_tmp11B1=
_cycalloc(sizeof(*_tmp11B1)),((_tmp11B1[0]=((_tmp11B2.tag=18,((_tmp11B2.f1=Cyc_Absyn_uint_exp(
0,0),_tmp11B2)))),_tmp11B1))));}goto _LL1B0;_LL1B9:;_LL1BA: t=Cyc_Absyn_sint_typ;
goto _LL1B0;_LL1B0:;}{struct _tuple16*_tmp11B3;return(_tmp11B3=_cycalloc(sizeof(*
_tmp11B3)),((_tmp11B3->f1=tv,((_tmp11B3->f2=t,_tmp11B3)))));};}static void*Cyc_Tcutil_rgns_of(
void*t){void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp367;void*_tmp369;void*
_tmp36A;struct Cyc_Absyn_DatatypeInfo _tmp36C;struct Cyc_List_List*_tmp36D;struct
Cyc_Absyn_PtrInfo _tmp36F;void*_tmp370;struct Cyc_Absyn_PtrAtts _tmp371;void*
_tmp372;struct Cyc_Absyn_ArrayInfo _tmp374;void*_tmp375;struct Cyc_List_List*
_tmp377;struct Cyc_Absyn_DatatypeFieldInfo _tmp379;struct Cyc_List_List*_tmp37A;
struct Cyc_Absyn_AggrInfo _tmp37C;struct Cyc_List_List*_tmp37D;struct Cyc_List_List*
_tmp37F;struct Cyc_Absyn_FnInfo _tmp382;struct Cyc_List_List*_tmp383;struct Cyc_Core_Opt*
_tmp384;struct Cyc_Absyn_Tqual _tmp385;void*_tmp386;struct Cyc_List_List*_tmp387;
struct Cyc_Absyn_VarargInfo*_tmp388;struct Cyc_List_List*_tmp389;void*_tmp390;
struct Cyc_List_List*_tmp392;_LL1BC: {struct Cyc_Absyn_VoidType_struct*_tmp35E=(
struct Cyc_Absyn_VoidType_struct*)_tmp35D;if(_tmp35E->tag != 0)goto _LL1BE;}_LL1BD:
goto _LL1BF;_LL1BE: {struct Cyc_Absyn_FloatType_struct*_tmp35F=(struct Cyc_Absyn_FloatType_struct*)
_tmp35D;if(_tmp35F->tag != 7)goto _LL1C0;}_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_EnumType_struct*
_tmp360=(struct Cyc_Absyn_EnumType_struct*)_tmp35D;if(_tmp360->tag != 13)goto
_LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_AnonEnumType_struct*_tmp361=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp35D;if(_tmp361->tag != 14)goto _LL1C4;}
_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_ValueofType_struct*_tmp362=(struct Cyc_Absyn_ValueofType_struct*)
_tmp35D;if(_tmp362->tag != 18)goto _LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_IntType_struct*
_tmp363=(struct Cyc_Absyn_IntType_struct*)_tmp35D;if(_tmp363->tag != 6)goto _LL1C8;}
_LL1C7: return Cyc_Absyn_empty_effect;_LL1C8: {struct Cyc_Absyn_Evar_struct*_tmp364=(
struct Cyc_Absyn_Evar_struct*)_tmp35D;if(_tmp364->tag != 1)goto _LL1CA;}_LL1C9: goto
_LL1CB;_LL1CA: {struct Cyc_Absyn_VarType_struct*_tmp365=(struct Cyc_Absyn_VarType_struct*)
_tmp35D;if(_tmp365->tag != 2)goto _LL1CC;}_LL1CB: {struct Cyc_Absyn_Kind*_tmp394=
Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp395;enum Cyc_Absyn_KindQual
_tmp396;struct Cyc_Absyn_Kind _tmp397;enum Cyc_Absyn_KindQual _tmp398;struct Cyc_Absyn_Kind
_tmp399;enum Cyc_Absyn_KindQual _tmp39A;_LL1F3: _tmp395=*_tmp394;_tmp396=_tmp395.kind;
if(_tmp396 != Cyc_Absyn_RgnKind)goto _LL1F5;_LL1F4: {struct Cyc_Absyn_AccessEff_struct
_tmp11B6;struct Cyc_Absyn_AccessEff_struct*_tmp11B5;return(void*)((_tmp11B5=
_cycalloc(sizeof(*_tmp11B5)),((_tmp11B5[0]=((_tmp11B6.tag=23,((_tmp11B6.f1=(void*)
t,_tmp11B6)))),_tmp11B5))));}_LL1F5: _tmp397=*_tmp394;_tmp398=_tmp397.kind;if(
_tmp398 != Cyc_Absyn_EffKind)goto _LL1F7;_LL1F6: return t;_LL1F7: _tmp399=*_tmp394;
_tmp39A=_tmp399.kind;if(_tmp39A != Cyc_Absyn_IntKind)goto _LL1F9;_LL1F8: return Cyc_Absyn_empty_effect;
_LL1F9:;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct _tmp11B9;struct Cyc_Absyn_RgnsEff_struct*
_tmp11B8;return(void*)((_tmp11B8=_cycalloc(sizeof(*_tmp11B8)),((_tmp11B8[0]=((
_tmp11B9.tag=25,((_tmp11B9.f1=(void*)t,_tmp11B9)))),_tmp11B8))));}_LL1F2:;}
_LL1CC: {struct Cyc_Absyn_RgnHandleType_struct*_tmp366=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp35D;if(_tmp366->tag != 15)goto _LL1CE;else{_tmp367=(void*)_tmp366->f1;}}_LL1CD: {
struct Cyc_Absyn_AccessEff_struct _tmp11BC;struct Cyc_Absyn_AccessEff_struct*
_tmp11BB;return(void*)((_tmp11BB=_cycalloc(sizeof(*_tmp11BB)),((_tmp11BB[0]=((
_tmp11BC.tag=23,((_tmp11BC.f1=(void*)_tmp367,_tmp11BC)))),_tmp11BB))));}_LL1CE: {
struct Cyc_Absyn_DynRgnType_struct*_tmp368=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp35D;if(_tmp368->tag != 16)goto _LL1D0;else{_tmp369=(void*)_tmp368->f1;_tmp36A=(
void*)_tmp368->f2;}}_LL1CF: {struct Cyc_Absyn_AccessEff_struct _tmp11BF;struct Cyc_Absyn_AccessEff_struct*
_tmp11BE;return(void*)((_tmp11BE=_cycalloc(sizeof(*_tmp11BE)),((_tmp11BE[0]=((
_tmp11BF.tag=23,((_tmp11BF.f1=(void*)_tmp36A,_tmp11BF)))),_tmp11BE))));}_LL1D0: {
struct Cyc_Absyn_DatatypeType_struct*_tmp36B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp35D;if(_tmp36B->tag != 3)goto _LL1D2;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.targs;}}
_LL1D1: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36D);struct Cyc_Absyn_JoinEff_struct
_tmp11C2;struct Cyc_Absyn_JoinEff_struct*_tmp11C1;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11C1=_cycalloc(sizeof(*_tmp11C1)),((_tmp11C1[0]=((_tmp11C2.tag=24,((
_tmp11C2.f1=ts,_tmp11C2)))),_tmp11C1)))));}_LL1D2: {struct Cyc_Absyn_PointerType_struct*
_tmp36E=(struct Cyc_Absyn_PointerType_struct*)_tmp35D;if(_tmp36E->tag != 5)goto
_LL1D4;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.elt_typ;_tmp371=_tmp36F.ptr_atts;
_tmp372=_tmp371.rgn;}}_LL1D3: {struct Cyc_Absyn_JoinEff_struct _tmp11D1;struct Cyc_Absyn_AccessEff_struct*
_tmp11D0;struct Cyc_Absyn_AccessEff_struct _tmp11CF;void*_tmp11CE[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11CD;return Cyc_Tcutil_normalize_effect((void*)((_tmp11CD=_cycalloc(sizeof(*
_tmp11CD)),((_tmp11CD[0]=((_tmp11D1.tag=24,((_tmp11D1.f1=((_tmp11CE[1]=Cyc_Tcutil_rgns_of(
_tmp370),((_tmp11CE[0]=(void*)((_tmp11D0=_cycalloc(sizeof(*_tmp11D0)),((_tmp11D0[
0]=((_tmp11CF.tag=23,((_tmp11CF.f1=(void*)_tmp372,_tmp11CF)))),_tmp11D0)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11CE,sizeof(void*),2)))))),_tmp11D1)))),_tmp11CD)))));}_LL1D4: {struct Cyc_Absyn_ArrayType_struct*
_tmp373=(struct Cyc_Absyn_ArrayType_struct*)_tmp35D;if(_tmp373->tag != 8)goto
_LL1D6;else{_tmp374=_tmp373->f1;_tmp375=_tmp374.elt_type;}}_LL1D5: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp375));_LL1D6: {struct Cyc_Absyn_TupleType_struct*_tmp376=(
struct Cyc_Absyn_TupleType_struct*)_tmp35D;if(_tmp376->tag != 10)goto _LL1D8;else{
_tmp377=_tmp376->f1;}}_LL1D7: {struct Cyc_List_List*_tmp3AA=0;for(0;_tmp377 != 0;
_tmp377=_tmp377->tl){struct Cyc_List_List*_tmp11D2;_tmp3AA=((_tmp11D2=_cycalloc(
sizeof(*_tmp11D2)),((_tmp11D2->hd=(*((struct _tuple12*)_tmp377->hd)).f2,((
_tmp11D2->tl=_tmp3AA,_tmp11D2))))));}_tmp37A=_tmp3AA;goto _LL1D9;}_LL1D8: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp378=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp35D;if(_tmp378->tag != 4)goto _LL1DA;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}
_LL1D9: _tmp37D=_tmp37A;goto _LL1DB;_LL1DA: {struct Cyc_Absyn_AggrType_struct*
_tmp37B=(struct Cyc_Absyn_AggrType_struct*)_tmp35D;if(_tmp37B->tag != 11)goto
_LL1DC;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37C.targs;}}_LL1DB: {struct Cyc_Absyn_JoinEff_struct
_tmp11D5;struct Cyc_Absyn_JoinEff_struct*_tmp11D4;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11D4=_cycalloc(sizeof(*_tmp11D4)),((_tmp11D4[0]=((_tmp11D5.tag=24,((
_tmp11D5.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37D),_tmp11D5)))),_tmp11D4)))));}_LL1DC: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp37E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35D;if(_tmp37E->tag != 12)goto _LL1DE;else{_tmp37F=_tmp37E->f2;}}_LL1DD: {
struct Cyc_Absyn_JoinEff_struct _tmp11D8;struct Cyc_Absyn_JoinEff_struct*_tmp11D7;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((
_tmp11D7[0]=((_tmp11D8.tag=24,((_tmp11D8.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp37F),_tmp11D8)))),_tmp11D7)))));}_LL1DE: {struct Cyc_Absyn_TagType_struct*
_tmp380=(struct Cyc_Absyn_TagType_struct*)_tmp35D;if(_tmp380->tag != 19)goto _LL1E0;}
_LL1DF: return Cyc_Absyn_empty_effect;_LL1E0: {struct Cyc_Absyn_FnType_struct*
_tmp381=(struct Cyc_Absyn_FnType_struct*)_tmp35D;if(_tmp381->tag != 9)goto _LL1E2;
else{_tmp382=_tmp381->f1;_tmp383=_tmp382.tvars;_tmp384=_tmp382.effect;_tmp385=
_tmp382.ret_tqual;_tmp386=_tmp382.ret_typ;_tmp387=_tmp382.args;_tmp388=_tmp382.cyc_varargs;
_tmp389=_tmp382.rgn_po;}}_LL1E1: {void*_tmp3B0=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_region_free_subst,_tmp383),(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp384))->v);return Cyc_Tcutil_normalize_effect(_tmp3B0);}_LL1E2: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp38A=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp35D;if(_tmp38A->tag != 21)goto
_LL1E4;}_LL1E3: goto _LL1E5;_LL1E4: {struct Cyc_Absyn_RefCntRgn_struct*_tmp38B=(
struct Cyc_Absyn_RefCntRgn_struct*)_tmp35D;if(_tmp38B->tag != 22)goto _LL1E6;}
_LL1E5: goto _LL1E7;_LL1E6: {struct Cyc_Absyn_HeapRgn_struct*_tmp38C=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp35D;if(_tmp38C->tag != 20)goto _LL1E8;}_LL1E7: return Cyc_Absyn_empty_effect;
_LL1E8: {struct Cyc_Absyn_AccessEff_struct*_tmp38D=(struct Cyc_Absyn_AccessEff_struct*)
_tmp35D;if(_tmp38D->tag != 23)goto _LL1EA;}_LL1E9: goto _LL1EB;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*
_tmp38E=(struct Cyc_Absyn_JoinEff_struct*)_tmp35D;if(_tmp38E->tag != 24)goto _LL1EC;}
_LL1EB: return t;_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*_tmp38F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp35D;if(_tmp38F->tag != 25)goto _LL1EE;else{_tmp390=(void*)_tmp38F->f1;}}_LL1ED:
return Cyc_Tcutil_rgns_of(_tmp390);_LL1EE: {struct Cyc_Absyn_TypedefType_struct*
_tmp391=(struct Cyc_Absyn_TypedefType_struct*)_tmp35D;if(_tmp391->tag != 17)goto
_LL1F0;else{_tmp392=_tmp391->f2;}}_LL1EF: {struct Cyc_Absyn_JoinEff_struct
_tmp11DB;struct Cyc_Absyn_JoinEff_struct*_tmp11DA;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11DA=_cycalloc(sizeof(*_tmp11DA)),((_tmp11DA[0]=((_tmp11DB.tag=24,((
_tmp11DB.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp392),_tmp11DB)))),_tmp11DA)))));}_LL1F0: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp393=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp35D;if(_tmp393->tag != 26)goto _LL1BB;}_LL1F1: {const char*_tmp11DE;void*
_tmp11DD;(_tmp11DD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11DE="typedecl in rgns_of",_tag_dyneither(_tmp11DE,
sizeof(char),20))),_tag_dyneither(_tmp11DD,sizeof(void*),0)));}_LL1BB:;}void*Cyc_Tcutil_normalize_effect(
void*e){e=Cyc_Tcutil_compress(e);{void*_tmp3B5=e;struct Cyc_List_List*_tmp3B7;
struct Cyc_List_List**_tmp3B8;void*_tmp3BA;_LL1FC: {struct Cyc_Absyn_JoinEff_struct*
_tmp3B6=(struct Cyc_Absyn_JoinEff_struct*)_tmp3B5;if(_tmp3B6->tag != 24)goto _LL1FE;
else{_tmp3B7=_tmp3B6->f1;_tmp3B8=(struct Cyc_List_List**)& _tmp3B6->f1;}}_LL1FD: {
int redo_join=0;{struct Cyc_List_List*effs=*_tmp3B8;for(0;effs != 0;effs=effs->tl){
void*_tmp3BB=(void*)effs->hd;effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp3BB));{void*_tmp3BC=(void*)effs->hd;void*_tmp3BF;void*_tmp3C2;void*_tmp3C5;
_LL203: {struct Cyc_Absyn_JoinEff_struct*_tmp3BD=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3BC;if(_tmp3BD->tag != 24)goto _LL205;}_LL204: goto _LL206;_LL205: {struct Cyc_Absyn_AccessEff_struct*
_tmp3BE=(struct Cyc_Absyn_AccessEff_struct*)_tmp3BC;if(_tmp3BE->tag != 23)goto
_LL207;else{_tmp3BF=(void*)_tmp3BE->f1;{struct Cyc_Absyn_HeapRgn_struct*_tmp3C0=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp3BF;if(_tmp3C0->tag != 20)goto _LL207;};}}
_LL206: redo_join=1;goto _LL202;_LL207: {struct Cyc_Absyn_AccessEff_struct*_tmp3C1=(
struct Cyc_Absyn_AccessEff_struct*)_tmp3BC;if(_tmp3C1->tag != 23)goto _LL209;else{
_tmp3C2=(void*)_tmp3C1->f1;{struct Cyc_Absyn_RefCntRgn_struct*_tmp3C3=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp3C2;if(_tmp3C3->tag != 22)goto _LL209;};}}_LL208: redo_join=1;goto _LL202;_LL209: {
struct Cyc_Absyn_AccessEff_struct*_tmp3C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3BC;if(_tmp3C4->tag != 23)goto _LL20B;else{_tmp3C5=(void*)_tmp3C4->f1;{struct
Cyc_Absyn_UniqueRgn_struct*_tmp3C6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp3C5;
if(_tmp3C6->tag != 21)goto _LL20B;};}}_LL20A: redo_join=1;goto _LL202;_LL20B:;_LL20C:
goto _LL202;_LL202:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp3B8;for(0;effs != 0;effs=effs->tl){void*_tmp3C7=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp3C9;void*_tmp3CB;void*_tmp3CE;void*
_tmp3D1;_LL20E: {struct Cyc_Absyn_JoinEff_struct*_tmp3C8=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3C7;if(_tmp3C8->tag != 24)goto _LL210;else{_tmp3C9=_tmp3C8->f1;}}_LL20F:
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_revappend)(_tmp3C9,effects);goto _LL20D;_LL210: {struct Cyc_Absyn_AccessEff_struct*
_tmp3CA=(struct Cyc_Absyn_AccessEff_struct*)_tmp3C7;if(_tmp3CA->tag != 23)goto
_LL212;else{_tmp3CB=(void*)_tmp3CA->f1;{struct Cyc_Absyn_HeapRgn_struct*_tmp3CC=(
struct Cyc_Absyn_HeapRgn_struct*)_tmp3CB;if(_tmp3CC->tag != 20)goto _LL212;};}}
_LL211: goto _LL20D;_LL212: {struct Cyc_Absyn_AccessEff_struct*_tmp3CD=(struct Cyc_Absyn_AccessEff_struct*)
_tmp3C7;if(_tmp3CD->tag != 23)goto _LL214;else{_tmp3CE=(void*)_tmp3CD->f1;{struct
Cyc_Absyn_RefCntRgn_struct*_tmp3CF=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp3CE;
if(_tmp3CF->tag != 22)goto _LL214;};}}_LL213: goto _LL20D;_LL214: {struct Cyc_Absyn_AccessEff_struct*
_tmp3D0=(struct Cyc_Absyn_AccessEff_struct*)_tmp3C7;if(_tmp3D0->tag != 23)goto
_LL216;else{_tmp3D1=(void*)_tmp3D0->f1;{struct Cyc_Absyn_UniqueRgn_struct*_tmp3D2=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp3D1;if(_tmp3D2->tag != 21)goto _LL216;};}}
_LL215: goto _LL20D;_LL216:;_LL217:{struct Cyc_List_List*_tmp11DF;effects=((
_tmp11DF=_cycalloc(sizeof(*_tmp11DF)),((_tmp11DF->hd=_tmp3C7,((_tmp11DF->tl=
effects,_tmp11DF))))));}goto _LL20D;_LL20D:;}}*_tmp3B8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3B9=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3B5;if(_tmp3B9->tag != 25)goto _LL200;
else{_tmp3BA=(void*)_tmp3B9->f1;}}_LL1FF: {void*_tmp3D4=Cyc_Tcutil_compress(
_tmp3BA);_LL219: {struct Cyc_Absyn_Evar_struct*_tmp3D5=(struct Cyc_Absyn_Evar_struct*)
_tmp3D4;if(_tmp3D5->tag != 1)goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_Absyn_VarType_struct*
_tmp3D6=(struct Cyc_Absyn_VarType_struct*)_tmp3D4;if(_tmp3D6->tag != 2)goto _LL21D;}
_LL21C: return e;_LL21D:;_LL21E: return Cyc_Tcutil_rgns_of(_tmp3BA);_LL218:;}_LL200:;
_LL201: return e;_LL1FB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp11E9;struct Cyc_Core_Opt*_tmp11E8;struct Cyc_Absyn_FnInfo _tmp11E7;struct Cyc_Absyn_FnType_struct*
_tmp11E6;struct Cyc_Absyn_FnType_struct*_tmp3D7=(_tmp11E6=_cycalloc(sizeof(*
_tmp11E6)),((_tmp11E6[0]=((_tmp11E9.tag=9,((_tmp11E9.f1=((_tmp11E7.tvars=0,((
_tmp11E7.effect=((_tmp11E8=_cycalloc(sizeof(*_tmp11E8)),((_tmp11E8->v=eff,
_tmp11E8)))),((_tmp11E7.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp11E7.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmp11E7.args=0,((_tmp11E7.c_varargs=0,((
_tmp11E7.cyc_varargs=0,((_tmp11E7.rgn_po=0,((_tmp11E7.attributes=0,_tmp11E7)))))))))))))))))),
_tmp11E9)))),_tmp11E6)));return Cyc_Absyn_atb_typ((void*)_tmp3D7,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{void*_tmp3DC=Cyc_Tcutil_compress(e);void*_tmp3DE;struct Cyc_List_List*
_tmp3E0;void*_tmp3E2;struct Cyc_Core_Opt*_tmp3E4;struct Cyc_Core_Opt*_tmp3E5;
struct Cyc_Core_Opt**_tmp3E6;struct Cyc_Core_Opt*_tmp3E7;_LL220: {struct Cyc_Absyn_AccessEff_struct*
_tmp3DD=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DC;if(_tmp3DD->tag != 23)goto
_LL222;else{_tmp3DE=(void*)_tmp3DD->f1;}}_LL221: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3DE);_tmp3DE=Cyc_Tcutil_compress(_tmp3DE);if(r == _tmp3DE)return 1;{struct
_tuple0 _tmp11EA;struct _tuple0 _tmp3E9=(_tmp11EA.f1=r,((_tmp11EA.f2=_tmp3DE,
_tmp11EA)));void*_tmp3EA;struct Cyc_Absyn_Tvar*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_Tvar*
_tmp3EF;_LL22B: _tmp3EA=_tmp3E9.f1;{struct Cyc_Absyn_VarType_struct*_tmp3EB=(
struct Cyc_Absyn_VarType_struct*)_tmp3EA;if(_tmp3EB->tag != 2)goto _LL22D;else{
_tmp3EC=_tmp3EB->f1;}};_tmp3ED=_tmp3E9.f2;{struct Cyc_Absyn_VarType_struct*
_tmp3EE=(struct Cyc_Absyn_VarType_struct*)_tmp3ED;if(_tmp3EE->tag != 2)goto _LL22D;
else{_tmp3EF=_tmp3EE->f1;}};_LL22C: return Cyc_Absyn_tvar_cmp(_tmp3EC,_tmp3EF)== 0;
_LL22D:;_LL22E: return 0;_LL22A:;};_LL222: {struct Cyc_Absyn_JoinEff_struct*_tmp3DF=(
struct Cyc_Absyn_JoinEff_struct*)_tmp3DC;if(_tmp3DF->tag != 24)goto _LL224;else{
_tmp3E0=_tmp3DF->f1;}}_LL223: for(0;_tmp3E0 != 0;_tmp3E0=_tmp3E0->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp3E0->hd))return 1;}return 0;_LL224: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3E1=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3DC;if(_tmp3E1->tag != 25)goto _LL226;
else{_tmp3E2=(void*)_tmp3E1->f1;}}_LL225: {void*_tmp3F0=Cyc_Tcutil_rgns_of(
_tmp3E2);void*_tmp3F2;_LL230: {struct Cyc_Absyn_RgnsEff_struct*_tmp3F1=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3F0;if(_tmp3F1->tag != 25)goto _LL232;else{_tmp3F2=(void*)_tmp3F1->f1;}}_LL231:
if(!constrain)return 0;{void*_tmp3F3=Cyc_Tcutil_compress(_tmp3F2);struct Cyc_Core_Opt*
_tmp3F5;struct Cyc_Core_Opt*_tmp3F6;struct Cyc_Core_Opt**_tmp3F7;struct Cyc_Core_Opt*
_tmp3F8;_LL235: {struct Cyc_Absyn_Evar_struct*_tmp3F4=(struct Cyc_Absyn_Evar_struct*)
_tmp3F3;if(_tmp3F4->tag != 1)goto _LL237;else{_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;
_tmp3F7=(struct Cyc_Core_Opt**)& _tmp3F4->f2;_tmp3F8=_tmp3F4->f4;}}_LL236: {void*
_tmp3F9=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp3F8);Cyc_Tcutil_occurs(
_tmp3F9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F8))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp11F9;struct Cyc_Absyn_AccessEff_struct*
_tmp11F8;struct Cyc_Absyn_AccessEff_struct _tmp11F7;void*_tmp11F6[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11F5;void*_tmp3FA=Cyc_Tcutil_dummy_fntype((void*)((_tmp11F5=_cycalloc(
sizeof(*_tmp11F5)),((_tmp11F5[0]=((_tmp11F9.tag=24,((_tmp11F9.f1=((_tmp11F6[1]=(
void*)((_tmp11F8=_cycalloc(sizeof(*_tmp11F8)),((_tmp11F8[0]=((_tmp11F7.tag=23,((
_tmp11F7.f1=(void*)r,_tmp11F7)))),_tmp11F8)))),((_tmp11F6[0]=_tmp3F9,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp11F6,sizeof(void*),2)))))),
_tmp11F9)))),_tmp11F5)))));{struct Cyc_Core_Opt*_tmp11FA;*_tmp3F7=((_tmp11FA=
_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA->v=_tmp3FA,_tmp11FA))));}return 1;};}
_LL237:;_LL238: return 0;_LL234:;};_LL232:;_LL233: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3F0);_LL22F:;}_LL226: {struct Cyc_Absyn_Evar_struct*_tmp3E3=(
struct Cyc_Absyn_Evar_struct*)_tmp3DC;if(_tmp3E3->tag != 1)goto _LL228;else{_tmp3E4=
_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;_tmp3E6=(struct Cyc_Core_Opt**)& _tmp3E3->f2;
_tmp3E7=_tmp3E3->f4;}}_LL227: if(_tmp3E4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E4->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp11FD;void*_tmp11FC;(_tmp11FC=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11FD="effect evar has wrong kind",
_tag_dyneither(_tmp11FD,sizeof(char),27))),_tag_dyneither(_tmp11FC,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp403=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3E7);Cyc_Tcutil_occurs(_tmp403,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E7))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp1212;struct Cyc_List_List*_tmp1211;struct Cyc_Absyn_AccessEff_struct _tmp1210;
struct Cyc_Absyn_AccessEff_struct*_tmp120F;struct Cyc_List_List*_tmp120E;struct Cyc_Absyn_JoinEff_struct*
_tmp120D;struct Cyc_Absyn_JoinEff_struct*_tmp404=(_tmp120D=_cycalloc(sizeof(*
_tmp120D)),((_tmp120D[0]=((_tmp1212.tag=24,((_tmp1212.f1=((_tmp120E=_cycalloc(
sizeof(*_tmp120E)),((_tmp120E->hd=_tmp403,((_tmp120E->tl=((_tmp1211=_cycalloc(
sizeof(*_tmp1211)),((_tmp1211->hd=(void*)((_tmp120F=_cycalloc(sizeof(*_tmp120F)),((
_tmp120F[0]=((_tmp1210.tag=23,((_tmp1210.f1=(void*)r,_tmp1210)))),_tmp120F)))),((
_tmp1211->tl=0,_tmp1211)))))),_tmp120E)))))),_tmp1212)))),_tmp120D)));{struct Cyc_Core_Opt*
_tmp1213;*_tmp3E6=((_tmp1213=_cycalloc(sizeof(*_tmp1213)),((_tmp1213->v=(void*)
_tmp404,_tmp1213))));}return 1;};};_LL228:;_LL229: return 0;_LL21F:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp40C=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp40F;void*_tmp411;struct Cyc_Core_Opt*
_tmp413;struct Cyc_Core_Opt*_tmp414;struct Cyc_Core_Opt**_tmp415;struct Cyc_Core_Opt*
_tmp416;_LL23A: {struct Cyc_Absyn_AccessEff_struct*_tmp40D=(struct Cyc_Absyn_AccessEff_struct*)
_tmp40C;if(_tmp40D->tag != 23)goto _LL23C;}_LL23B: return 0;_LL23C: {struct Cyc_Absyn_JoinEff_struct*
_tmp40E=(struct Cyc_Absyn_JoinEff_struct*)_tmp40C;if(_tmp40E->tag != 24)goto _LL23E;
else{_tmp40F=_tmp40E->f1;}}_LL23D: for(0;_tmp40F != 0;_tmp40F=_tmp40F->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp40F->hd))return 1;}return 0;_LL23E: {struct Cyc_Absyn_RgnsEff_struct*
_tmp410=(struct Cyc_Absyn_RgnsEff_struct*)_tmp40C;if(_tmp410->tag != 25)goto _LL240;
else{_tmp411=(void*)_tmp410->f1;}}_LL23F: _tmp411=Cyc_Tcutil_compress(_tmp411);
if(t == _tmp411)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp411);{
void*_tmp417=Cyc_Tcutil_rgns_of(t);void*_tmp419;_LL245: {struct Cyc_Absyn_RgnsEff_struct*
_tmp418=(struct Cyc_Absyn_RgnsEff_struct*)_tmp417;if(_tmp418->tag != 25)goto _LL247;
else{_tmp419=(void*)_tmp418->f1;}}_LL246: {struct _tuple0 _tmp1214;struct _tuple0
_tmp41B=(_tmp1214.f1=Cyc_Tcutil_compress(_tmp419),((_tmp1214.f2=_tmp411,_tmp1214)));
void*_tmp41C;struct Cyc_Absyn_Tvar*_tmp41E;void*_tmp41F;struct Cyc_Absyn_Tvar*
_tmp421;_LL24A: _tmp41C=_tmp41B.f1;{struct Cyc_Absyn_VarType_struct*_tmp41D=(
struct Cyc_Absyn_VarType_struct*)_tmp41C;if(_tmp41D->tag != 2)goto _LL24C;else{
_tmp41E=_tmp41D->f1;}};_tmp41F=_tmp41B.f2;{struct Cyc_Absyn_VarType_struct*
_tmp420=(struct Cyc_Absyn_VarType_struct*)_tmp41F;if(_tmp420->tag != 2)goto _LL24C;
else{_tmp421=_tmp420->f1;}};_LL24B: return Cyc_Tcutil_unify(t,_tmp411);_LL24C:;
_LL24D: return _tmp419 == _tmp411;_LL249:;}_LL247:;_LL248: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp417);_LL244:;};_LL240: {struct Cyc_Absyn_Evar_struct*
_tmp412=(struct Cyc_Absyn_Evar_struct*)_tmp40C;if(_tmp412->tag != 1)goto _LL242;
else{_tmp413=_tmp412->f1;_tmp414=_tmp412->f2;_tmp415=(struct Cyc_Core_Opt**)&
_tmp412->f2;_tmp416=_tmp412->f4;}}_LL241: if(_tmp413 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp413->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1217;void*_tmp1216;(
_tmp1216=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1217="effect evar has wrong kind",_tag_dyneither(_tmp1217,sizeof(char),27))),
_tag_dyneither(_tmp1216,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp424=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp416);
Cyc_Tcutil_occurs(_tmp424,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp416))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp122C;struct Cyc_List_List*
_tmp122B;struct Cyc_Absyn_RgnsEff_struct _tmp122A;struct Cyc_Absyn_RgnsEff_struct*
_tmp1229;struct Cyc_List_List*_tmp1228;struct Cyc_Absyn_JoinEff_struct*_tmp1227;
struct Cyc_Absyn_JoinEff_struct*_tmp425=(_tmp1227=_cycalloc(sizeof(*_tmp1227)),((
_tmp1227[0]=((_tmp122C.tag=24,((_tmp122C.f1=((_tmp1228=_cycalloc(sizeof(*
_tmp1228)),((_tmp1228->hd=_tmp424,((_tmp1228->tl=((_tmp122B=_cycalloc(sizeof(*
_tmp122B)),((_tmp122B->hd=(void*)((_tmp1229=_cycalloc(sizeof(*_tmp1229)),((
_tmp1229[0]=((_tmp122A.tag=25,((_tmp122A.f1=(void*)t,_tmp122A)))),_tmp1229)))),((
_tmp122B->tl=0,_tmp122B)))))),_tmp1228)))))),_tmp122C)))),_tmp1227)));{struct Cyc_Core_Opt*
_tmp122D;*_tmp415=((_tmp122D=_cycalloc(sizeof(*_tmp122D)),((_tmp122D->v=(void*)
_tmp425,_tmp122D))));}return 1;};};_LL242:;_LL243: return 0;_LL239:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp42D=e;struct Cyc_Absyn_Tvar*_tmp42F;struct Cyc_List_List*_tmp431;void*
_tmp433;struct Cyc_Core_Opt*_tmp435;struct Cyc_Core_Opt*_tmp436;struct Cyc_Core_Opt**
_tmp437;struct Cyc_Core_Opt*_tmp438;_LL24F: {struct Cyc_Absyn_VarType_struct*
_tmp42E=(struct Cyc_Absyn_VarType_struct*)_tmp42D;if(_tmp42E->tag != 2)goto _LL251;
else{_tmp42F=_tmp42E->f1;}}_LL250: return Cyc_Absyn_tvar_cmp(v,_tmp42F)== 0;_LL251: {
struct Cyc_Absyn_JoinEff_struct*_tmp430=(struct Cyc_Absyn_JoinEff_struct*)_tmp42D;
if(_tmp430->tag != 24)goto _LL253;else{_tmp431=_tmp430->f1;}}_LL252: for(0;_tmp431
!= 0;_tmp431=_tmp431->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp431->hd))return 1;}return 0;_LL253: {struct Cyc_Absyn_RgnsEff_struct*
_tmp432=(struct Cyc_Absyn_RgnsEff_struct*)_tmp42D;if(_tmp432->tag != 25)goto _LL255;
else{_tmp433=(void*)_tmp432->f1;}}_LL254: {void*_tmp439=Cyc_Tcutil_rgns_of(
_tmp433);void*_tmp43B;_LL25A: {struct Cyc_Absyn_RgnsEff_struct*_tmp43A=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp439;if(_tmp43A->tag != 25)goto _LL25C;else{_tmp43B=(void*)_tmp43A->f1;}}_LL25B:
if(!may_constrain_evars)return 0;{void*_tmp43C=Cyc_Tcutil_compress(_tmp43B);
struct Cyc_Core_Opt*_tmp43E;struct Cyc_Core_Opt*_tmp43F;struct Cyc_Core_Opt**
_tmp440;struct Cyc_Core_Opt*_tmp441;_LL25F: {struct Cyc_Absyn_Evar_struct*_tmp43D=(
struct Cyc_Absyn_Evar_struct*)_tmp43C;if(_tmp43D->tag != 1)goto _LL261;else{_tmp43E=
_tmp43D->f1;_tmp43F=_tmp43D->f2;_tmp440=(struct Cyc_Core_Opt**)& _tmp43D->f2;
_tmp441=_tmp43D->f4;}}_LL260: {void*_tmp442=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp441);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp441))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmp123C;struct Cyc_Absyn_VarType_struct*_tmp123B;
struct Cyc_Absyn_VarType_struct _tmp123A;void*_tmp1239[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1238;void*_tmp443=Cyc_Tcutil_dummy_fntype((void*)((_tmp1238=_cycalloc(
sizeof(*_tmp1238)),((_tmp1238[0]=((_tmp123C.tag=24,((_tmp123C.f1=((_tmp1239[1]=(
void*)((_tmp123B=_cycalloc(sizeof(*_tmp123B)),((_tmp123B[0]=((_tmp123A.tag=2,((
_tmp123A.f1=v,_tmp123A)))),_tmp123B)))),((_tmp1239[0]=_tmp442,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1239,sizeof(void*),2)))))),
_tmp123C)))),_tmp1238)))));{struct Cyc_Core_Opt*_tmp123D;*_tmp440=((_tmp123D=
_cycalloc(sizeof(*_tmp123D)),((_tmp123D->v=_tmp443,_tmp123D))));}return 1;};}
_LL261:;_LL262: return 0;_LL25E:;};_LL25C:;_LL25D: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp439);_LL259:;}_LL255: {struct Cyc_Absyn_Evar_struct*
_tmp434=(struct Cyc_Absyn_Evar_struct*)_tmp42D;if(_tmp434->tag != 1)goto _LL257;
else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;_tmp437=(struct Cyc_Core_Opt**)&
_tmp434->f2;_tmp438=_tmp434->f4;}}_LL256: if(_tmp435 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp435->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1240;void*_tmp123F;(
_tmp123F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1240="effect evar has wrong kind",_tag_dyneither(_tmp1240,sizeof(char),27))),
_tag_dyneither(_tmp123F,sizeof(void*),0)));}{void*_tmp44C=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp438);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp438))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp1255;
struct Cyc_List_List*_tmp1254;struct Cyc_Absyn_VarType_struct _tmp1253;struct Cyc_Absyn_VarType_struct*
_tmp1252;struct Cyc_List_List*_tmp1251;struct Cyc_Absyn_JoinEff_struct*_tmp1250;
struct Cyc_Absyn_JoinEff_struct*_tmp44D=(_tmp1250=_cycalloc(sizeof(*_tmp1250)),((
_tmp1250[0]=((_tmp1255.tag=24,((_tmp1255.f1=((_tmp1251=_cycalloc(sizeof(*
_tmp1251)),((_tmp1251->hd=_tmp44C,((_tmp1251->tl=((_tmp1254=_cycalloc(sizeof(*
_tmp1254)),((_tmp1254->hd=(void*)((_tmp1252=_cycalloc(sizeof(*_tmp1252)),((
_tmp1252[0]=((_tmp1253.tag=2,((_tmp1253.f1=v,_tmp1253)))),_tmp1252)))),((
_tmp1254->tl=0,_tmp1254)))))),_tmp1251)))))),_tmp1255)))),_tmp1250)));{struct Cyc_Core_Opt*
_tmp1256;*_tmp437=((_tmp1256=_cycalloc(sizeof(*_tmp1256)),((_tmp1256->v=(void*)
_tmp44D,_tmp1256))));}return 1;};};_LL257:;_LL258: return 0;_LL24E:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp455=e;struct Cyc_List_List*
_tmp457;void*_tmp459;_LL264: {struct Cyc_Absyn_JoinEff_struct*_tmp456=(struct Cyc_Absyn_JoinEff_struct*)
_tmp455;if(_tmp456->tag != 24)goto _LL266;else{_tmp457=_tmp456->f1;}}_LL265: for(0;
_tmp457 != 0;_tmp457=_tmp457->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp457->hd))return 1;}return 0;_LL266: {struct Cyc_Absyn_RgnsEff_struct*_tmp458=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp455;if(_tmp458->tag != 25)goto _LL268;else{
_tmp459=(void*)_tmp458->f1;}}_LL267: {void*_tmp45B=Cyc_Tcutil_rgns_of(_tmp459);
void*_tmp45D;_LL26D: {struct Cyc_Absyn_RgnsEff_struct*_tmp45C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp45B;if(_tmp45C->tag != 25)goto _LL26F;else{_tmp45D=(void*)_tmp45C->f1;}}_LL26E:
return 0;_LL26F:;_LL270: return Cyc_Tcutil_evar_in_effect(evar,_tmp45B);_LL26C:;}
_LL268: {struct Cyc_Absyn_Evar_struct*_tmp45A=(struct Cyc_Absyn_Evar_struct*)
_tmp455;if(_tmp45A->tag != 1)goto _LL26A;}_LL269: return evar == e;_LL26A:;_LL26B:
return 0;_LL263:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2){void*_tmp45E=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp460;void*
_tmp462;struct Cyc_Absyn_Tvar*_tmp464;void*_tmp466;struct Cyc_Core_Opt*_tmp468;
struct Cyc_Core_Opt**_tmp469;struct Cyc_Core_Opt*_tmp46A;_LL272: {struct Cyc_Absyn_JoinEff_struct*
_tmp45F=(struct Cyc_Absyn_JoinEff_struct*)_tmp45E;if(_tmp45F->tag != 24)goto _LL274;
else{_tmp460=_tmp45F->f1;}}_LL273: for(0;_tmp460 != 0;_tmp460=_tmp460->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp460->hd,e2))return 0;}return 1;_LL274: {struct Cyc_Absyn_AccessEff_struct*
_tmp461=(struct Cyc_Absyn_AccessEff_struct*)_tmp45E;if(_tmp461->tag != 23)goto
_LL276;else{_tmp462=(void*)_tmp461->f1;}}_LL275: return Cyc_Tcutil_region_in_effect(
0,_tmp462,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp462,(void*)& Cyc_Absyn_HeapRgn_val);
_LL276: {struct Cyc_Absyn_VarType_struct*_tmp463=(struct Cyc_Absyn_VarType_struct*)
_tmp45E;if(_tmp463->tag != 2)goto _LL278;else{_tmp464=_tmp463->f1;}}_LL277: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp464,e2);_LL278: {struct Cyc_Absyn_RgnsEff_struct*
_tmp465=(struct Cyc_Absyn_RgnsEff_struct*)_tmp45E;if(_tmp465->tag != 25)goto _LL27A;
else{_tmp466=(void*)_tmp465->f1;}}_LL279: {void*_tmp46B=Cyc_Tcutil_rgns_of(
_tmp466);void*_tmp46D;_LL27F: {struct Cyc_Absyn_RgnsEff_struct*_tmp46C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp46B;if(_tmp46C->tag != 25)goto _LL281;else{_tmp46D=(void*)_tmp46C->f1;}}_LL280:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp46D,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp46D,Cyc_Absyn_sint_typ);_LL281:;
_LL282: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp46B,e2);_LL27E:;}
_LL27A: {struct Cyc_Absyn_Evar_struct*_tmp467=(struct Cyc_Absyn_Evar_struct*)
_tmp45E;if(_tmp467->tag != 1)goto _LL27C;else{_tmp468=_tmp467->f2;_tmp469=(struct
Cyc_Core_Opt**)& _tmp467->f2;_tmp46A=_tmp467->f4;}}_LL27B: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmp1257;*_tmp469=((_tmp1257=_cycalloc(sizeof(*
_tmp1257)),((_tmp1257->v=Cyc_Absyn_empty_effect,_tmp1257))));}return 1;_LL27C:;
_LL27D: {const char*_tmp125B;void*_tmp125A[1];struct Cyc_String_pa_struct _tmp1259;(
_tmp1259.tag=0,((_tmp1259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp125A[0]=& _tmp1259,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125B="subset_effect: bad effect: %s",
_tag_dyneither(_tmp125B,sizeof(char),30))),_tag_dyneither(_tmp125A,sizeof(void*),
1)))))));}_LL271:;}static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp473;void*_tmp474;void*_tmp475;struct
_tuple0*_tmp472=(struct _tuple0*)r1->hd;_tmp473=*_tmp472;_tmp474=_tmp473.f1;
_tmp475=_tmp473.f2;{int found=_tmp474 == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*
r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp477;void*_tmp478;void*
_tmp479;struct _tuple0*_tmp476=(struct _tuple0*)r2->hd;_tmp477=*_tmp476;_tmp478=
_tmp477.f1;_tmp479=_tmp477.f2;if(Cyc_Tcutil_unify(_tmp474,_tmp478) && Cyc_Tcutil_unify(
_tmp475,_tmp479)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple23{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)
_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t1 == t2)return;{void*_tmp47A=t1;struct Cyc_Core_Opt*_tmp47C;struct Cyc_Core_Opt*
_tmp47D;struct Cyc_Core_Opt**_tmp47E;struct Cyc_Core_Opt*_tmp47F;_LL284: {struct
Cyc_Absyn_Evar_struct*_tmp47B=(struct Cyc_Absyn_Evar_struct*)_tmp47A;if(_tmp47B->tag
!= 1)goto _LL286;else{_tmp47C=_tmp47B->f1;_tmp47D=_tmp47B->f2;_tmp47E=(struct Cyc_Core_Opt**)&
_tmp47B->f2;_tmp47F=_tmp47B->f4;}}_LL285: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp47F))->v,t2);{struct
Cyc_Absyn_Kind*_tmp480=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp480,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp47C))->v)){{struct
Cyc_Core_Opt*_tmp125C;*_tmp47E=((_tmp125C=_cycalloc(sizeof(*_tmp125C)),((
_tmp125C->v=t2,_tmp125C))));}return;}else{{void*_tmp482=t2;struct Cyc_Core_Opt*
_tmp484;struct Cyc_Core_Opt**_tmp485;struct Cyc_Core_Opt*_tmp486;struct Cyc_Absyn_PtrInfo
_tmp488;_LL289: {struct Cyc_Absyn_Evar_struct*_tmp483=(struct Cyc_Absyn_Evar_struct*)
_tmp482;if(_tmp483->tag != 1)goto _LL28B;else{_tmp484=_tmp483->f2;_tmp485=(struct
Cyc_Core_Opt**)& _tmp483->f2;_tmp486=_tmp483->f4;}}_LL28A: {struct Cyc_List_List*
_tmp489=(struct Cyc_List_List*)_tmp47F->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp486))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp489,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp125D;Cyc_Tcutil_failure_reason=((
_tmp125D="(type variable would escape scope)",_tag_dyneither(_tmp125D,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp47C->v,_tmp480)){{struct Cyc_Core_Opt*_tmp125E;*_tmp485=((
_tmp125E=_cycalloc(sizeof(*_tmp125E)),((_tmp125E->v=t1,_tmp125E))));}return;}{
const char*_tmp125F;Cyc_Tcutil_failure_reason=((_tmp125F="(kinds are incompatible)",
_tag_dyneither(_tmp125F,sizeof(char),25)));}goto _LL288;}_LL28B:{struct Cyc_Absyn_PointerType_struct*
_tmp487=(struct Cyc_Absyn_PointerType_struct*)_tmp482;if(_tmp487->tag != 5)goto
_LL28D;else{_tmp488=_tmp487->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp47C->v)->kind
== Cyc_Absyn_BoxKind))goto _LL28D;_LL28C: {union Cyc_Absyn_Constraint*_tmp48D=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp488.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp48E=_tmp48D;union Cyc_Absyn_Constraint
_tmp48F;int _tmp490;_LL290: _tmp48F=*_tmp48E;if((_tmp48F.No_constr).tag != 3)goto
_LL292;_tmp490=(int)(_tmp48F.No_constr).val;_LL291:{struct
_union_Constraint_Eq_constr*_tmp1260;(_tmp1260=& _tmp48D->Eq_constr,((_tmp1260->tag=
1,_tmp1260->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp1261;*_tmp47E=((
_tmp1261=_cycalloc(sizeof(*_tmp1261)),((_tmp1261->v=t2,_tmp1261))));}return;
_LL292:;_LL293: goto _LL28F;_LL28F:;}goto _LL288;}_LL28D:;_LL28E: goto _LL288;_LL288:;}{
const char*_tmp1262;Cyc_Tcutil_failure_reason=((_tmp1262="(kinds are incompatible)",
_tag_dyneither(_tmp1262,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL286:;_LL287: goto _LL283;_LL283:;}{struct _tuple0 _tmp1263;struct _tuple0 _tmp495=(
_tmp1263.f1=t2,((_tmp1263.f2=t1,_tmp1263)));void*_tmp496;void*_tmp498;void*
_tmp49A;void*_tmp49C;struct Cyc_Absyn_Tvar*_tmp49E;void*_tmp49F;struct Cyc_Absyn_Tvar*
_tmp4A1;void*_tmp4A2;struct Cyc_Absyn_AggrInfo _tmp4A4;union Cyc_Absyn_AggrInfoU
_tmp4A5;struct Cyc_List_List*_tmp4A6;void*_tmp4A7;struct Cyc_Absyn_AggrInfo _tmp4A9;
union Cyc_Absyn_AggrInfoU _tmp4AA;struct Cyc_List_List*_tmp4AB;void*_tmp4AC;struct
_tuple2*_tmp4AE;void*_tmp4AF;struct _tuple2*_tmp4B1;void*_tmp4B2;struct Cyc_List_List*
_tmp4B4;void*_tmp4B5;struct Cyc_List_List*_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_DatatypeInfo
_tmp4BA;union Cyc_Absyn_DatatypeInfoU _tmp4BB;struct Cyc_Absyn_Datatypedecl**
_tmp4BC;struct Cyc_Absyn_Datatypedecl*_tmp4BD;struct Cyc_List_List*_tmp4BE;void*
_tmp4BF;struct Cyc_Absyn_DatatypeInfo _tmp4C1;union Cyc_Absyn_DatatypeInfoU _tmp4C2;
struct Cyc_Absyn_Datatypedecl**_tmp4C3;struct Cyc_Absyn_Datatypedecl*_tmp4C4;
struct Cyc_List_List*_tmp4C5;void*_tmp4C6;struct Cyc_Absyn_DatatypeFieldInfo
_tmp4C8;union Cyc_Absyn_DatatypeFieldInfoU _tmp4C9;struct _tuple3 _tmp4CA;struct Cyc_Absyn_Datatypedecl*
_tmp4CB;struct Cyc_Absyn_Datatypefield*_tmp4CC;struct Cyc_List_List*_tmp4CD;void*
_tmp4CE;struct Cyc_Absyn_DatatypeFieldInfo _tmp4D0;union Cyc_Absyn_DatatypeFieldInfoU
_tmp4D1;struct _tuple3 _tmp4D2;struct Cyc_Absyn_Datatypedecl*_tmp4D3;struct Cyc_Absyn_Datatypefield*
_tmp4D4;struct Cyc_List_List*_tmp4D5;void*_tmp4D6;struct Cyc_Absyn_PtrInfo _tmp4D8;
void*_tmp4D9;struct Cyc_Absyn_Tqual _tmp4DA;struct Cyc_Absyn_PtrAtts _tmp4DB;void*
_tmp4DC;union Cyc_Absyn_Constraint*_tmp4DD;union Cyc_Absyn_Constraint*_tmp4DE;
union Cyc_Absyn_Constraint*_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_PtrInfo _tmp4E2;
void*_tmp4E3;struct Cyc_Absyn_Tqual _tmp4E4;struct Cyc_Absyn_PtrAtts _tmp4E5;void*
_tmp4E6;union Cyc_Absyn_Constraint*_tmp4E7;union Cyc_Absyn_Constraint*_tmp4E8;
union Cyc_Absyn_Constraint*_tmp4E9;void*_tmp4EA;enum Cyc_Absyn_Sign _tmp4EC;enum 
Cyc_Absyn_Size_of _tmp4ED;void*_tmp4EE;enum Cyc_Absyn_Sign _tmp4F0;enum Cyc_Absyn_Size_of
_tmp4F1;void*_tmp4F2;int _tmp4F4;void*_tmp4F5;int _tmp4F7;void*_tmp4F8;void*
_tmp4FA;void*_tmp4FB;void*_tmp4FD;void*_tmp4FE;struct Cyc_Absyn_Exp*_tmp500;void*
_tmp501;struct Cyc_Absyn_Exp*_tmp503;void*_tmp504;struct Cyc_Absyn_ArrayInfo
_tmp506;void*_tmp507;struct Cyc_Absyn_Tqual _tmp508;struct Cyc_Absyn_Exp*_tmp509;
union Cyc_Absyn_Constraint*_tmp50A;void*_tmp50B;struct Cyc_Absyn_ArrayInfo _tmp50D;
void*_tmp50E;struct Cyc_Absyn_Tqual _tmp50F;struct Cyc_Absyn_Exp*_tmp510;union Cyc_Absyn_Constraint*
_tmp511;void*_tmp512;struct Cyc_Absyn_FnInfo _tmp514;struct Cyc_List_List*_tmp515;
struct Cyc_Core_Opt*_tmp516;struct Cyc_Absyn_Tqual _tmp517;void*_tmp518;struct Cyc_List_List*
_tmp519;int _tmp51A;struct Cyc_Absyn_VarargInfo*_tmp51B;struct Cyc_List_List*
_tmp51C;struct Cyc_List_List*_tmp51D;void*_tmp51E;struct Cyc_Absyn_FnInfo _tmp520;
struct Cyc_List_List*_tmp521;struct Cyc_Core_Opt*_tmp522;struct Cyc_Absyn_Tqual
_tmp523;void*_tmp524;struct Cyc_List_List*_tmp525;int _tmp526;struct Cyc_Absyn_VarargInfo*
_tmp527;struct Cyc_List_List*_tmp528;struct Cyc_List_List*_tmp529;void*_tmp52A;
struct Cyc_List_List*_tmp52C;void*_tmp52D;struct Cyc_List_List*_tmp52F;void*
_tmp530;enum Cyc_Absyn_AggrKind _tmp532;struct Cyc_List_List*_tmp533;void*_tmp534;
enum Cyc_Absyn_AggrKind _tmp536;struct Cyc_List_List*_tmp537;void*_tmp538;struct
Cyc_List_List*_tmp53A;struct Cyc_Absyn_Typedefdecl*_tmp53B;void*_tmp53C;struct Cyc_List_List*
_tmp53E;struct Cyc_Absyn_Typedefdecl*_tmp53F;void*_tmp540;void*_tmp542;void*
_tmp544;void*_tmp546;void*_tmp548;void*_tmp54A;void*_tmp54C;void*_tmp54E;void*
_tmp54F;void*_tmp551;void*_tmp552;void*_tmp554;void*_tmp555;void*_tmp556;void*
_tmp558;void*_tmp559;void*_tmp55A;void*_tmp55C;void*_tmp55E;void*_tmp560;void*
_tmp562;void*_tmp564;_LL295: _tmp496=_tmp495.f1;{struct Cyc_Absyn_Evar_struct*
_tmp497=(struct Cyc_Absyn_Evar_struct*)_tmp496;if(_tmp497->tag != 1)goto _LL297;};
_LL296: Cyc_Tcutil_unify_it(t2,t1);return;_LL297: _tmp498=_tmp495.f1;{struct Cyc_Absyn_VoidType_struct*
_tmp499=(struct Cyc_Absyn_VoidType_struct*)_tmp498;if(_tmp499->tag != 0)goto _LL299;};
_tmp49A=_tmp495.f2;{struct Cyc_Absyn_VoidType_struct*_tmp49B=(struct Cyc_Absyn_VoidType_struct*)
_tmp49A;if(_tmp49B->tag != 0)goto _LL299;};_LL298: return;_LL299: _tmp49C=_tmp495.f1;{
struct Cyc_Absyn_VarType_struct*_tmp49D=(struct Cyc_Absyn_VarType_struct*)_tmp49C;
if(_tmp49D->tag != 2)goto _LL29B;else{_tmp49E=_tmp49D->f1;}};_tmp49F=_tmp495.f2;{
struct Cyc_Absyn_VarType_struct*_tmp4A0=(struct Cyc_Absyn_VarType_struct*)_tmp49F;
if(_tmp4A0->tag != 2)goto _LL29B;else{_tmp4A1=_tmp4A0->f1;}};_LL29A: {struct
_dyneither_ptr*_tmp566=_tmp49E->name;struct _dyneither_ptr*_tmp567=_tmp4A1->name;
int _tmp568=_tmp49E->identity;int _tmp569=_tmp4A1->identity;if(_tmp569 == _tmp568)
return;{const char*_tmp1264;Cyc_Tcutil_failure_reason=((_tmp1264="(variable types are not the same)",
_tag_dyneither(_tmp1264,sizeof(char),34)));}goto _LL294;}_LL29B: _tmp4A2=_tmp495.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4A3=(struct Cyc_Absyn_AggrType_struct*)
_tmp4A2;if(_tmp4A3->tag != 11)goto _LL29D;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A4.aggr_info;
_tmp4A6=_tmp4A4.targs;}};_tmp4A7=_tmp495.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4A8=(struct Cyc_Absyn_AggrType_struct*)_tmp4A7;if(_tmp4A8->tag != 11)goto
_LL29D;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A9.aggr_info;_tmp4AB=_tmp4A9.targs;}};
_LL29C: {enum Cyc_Absyn_AggrKind _tmp56C;struct _tuple2*_tmp56D;struct _tuple13
_tmp56B=Cyc_Absyn_aggr_kinded_name(_tmp4AA);_tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;{
enum Cyc_Absyn_AggrKind _tmp56F;struct _tuple2*_tmp570;struct _tuple13 _tmp56E=Cyc_Absyn_aggr_kinded_name(
_tmp4A5);_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;if(_tmp56C != _tmp56F){{const char*
_tmp1265;Cyc_Tcutil_failure_reason=((_tmp1265="(struct and union type)",
_tag_dyneither(_tmp1265,sizeof(char),24)));}goto _LL294;}if(Cyc_Absyn_qvar_cmp(
_tmp56D,_tmp570)!= 0){{const char*_tmp1266;Cyc_Tcutil_failure_reason=((_tmp1266="(different type name)",
_tag_dyneither(_tmp1266,sizeof(char),22)));}goto _LL294;}Cyc_Tcutil_unify_list(
_tmp4AB,_tmp4A6);return;};}_LL29D: _tmp4AC=_tmp495.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp4AD=(struct Cyc_Absyn_EnumType_struct*)_tmp4AC;if(_tmp4AD->tag != 13)goto
_LL29F;else{_tmp4AE=_tmp4AD->f1;}};_tmp4AF=_tmp495.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp4B0=(struct Cyc_Absyn_EnumType_struct*)_tmp4AF;if(_tmp4B0->tag != 13)goto
_LL29F;else{_tmp4B1=_tmp4B0->f1;}};_LL29E: if(Cyc_Absyn_qvar_cmp(_tmp4AE,_tmp4B1)
== 0)return;{const char*_tmp1267;Cyc_Tcutil_failure_reason=((_tmp1267="(different enum types)",
_tag_dyneither(_tmp1267,sizeof(char),23)));}goto _LL294;_LL29F: _tmp4B2=_tmp495.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp4B3=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B2;if(_tmp4B3->tag != 14)goto _LL2A1;else{_tmp4B4=_tmp4B3->f1;}};_tmp4B5=
_tmp495.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4B6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B5;if(_tmp4B6->tag != 14)goto _LL2A1;else{_tmp4B7=_tmp4B6->f1;}};_LL2A0: {int
bad=0;for(0;_tmp4B4 != 0  && _tmp4B7 != 0;(_tmp4B4=_tmp4B4->tl,_tmp4B7=_tmp4B7->tl)){
struct Cyc_Absyn_Enumfield*_tmp574=(struct Cyc_Absyn_Enumfield*)_tmp4B4->hd;struct
Cyc_Absyn_Enumfield*_tmp575=(struct Cyc_Absyn_Enumfield*)_tmp4B7->hd;if(Cyc_Absyn_qvar_cmp(
_tmp574->name,_tmp575->name)!= 0){{const char*_tmp1268;Cyc_Tcutil_failure_reason=((
_tmp1268="(different names for enum fields)",_tag_dyneither(_tmp1268,sizeof(char),
34)));}bad=1;break;}if(_tmp574->tag == _tmp575->tag)continue;if(_tmp574->tag == 0
 || _tmp575->tag == 0){{const char*_tmp1269;Cyc_Tcutil_failure_reason=((_tmp1269="(different tag values for enum fields)",
_tag_dyneither(_tmp1269,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp574->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp575->tag))){{const char*_tmp126A;Cyc_Tcutil_failure_reason=((_tmp126A="(different tag values for enum fields)",
_tag_dyneither(_tmp126A,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL294;if(
_tmp4B4 == 0  && _tmp4B7 == 0)return;{const char*_tmp126B;Cyc_Tcutil_failure_reason=((
_tmp126B="(different number of fields for enums)",_tag_dyneither(_tmp126B,
sizeof(char),39)));}goto _LL294;}_LL2A1: _tmp4B8=_tmp495.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4B9=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4B8;if(_tmp4B9->tag != 3)goto
_LL2A3;else{_tmp4BA=_tmp4B9->f1;_tmp4BB=_tmp4BA.datatype_info;if((_tmp4BB.KnownDatatype).tag
!= 2)goto _LL2A3;_tmp4BC=(struct Cyc_Absyn_Datatypedecl**)(_tmp4BB.KnownDatatype).val;
_tmp4BD=*_tmp4BC;_tmp4BE=_tmp4BA.targs;}};_tmp4BF=_tmp495.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4C0=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4BF;if(_tmp4C0->tag != 3)goto
_LL2A3;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C1.datatype_info;if((_tmp4C2.KnownDatatype).tag
!= 2)goto _LL2A3;_tmp4C3=(struct Cyc_Absyn_Datatypedecl**)(_tmp4C2.KnownDatatype).val;
_tmp4C4=*_tmp4C3;_tmp4C5=_tmp4C1.targs;}};_LL2A2: if(_tmp4BD == _tmp4C4  || Cyc_Absyn_qvar_cmp(
_tmp4BD->name,_tmp4C4->name)== 0){Cyc_Tcutil_unify_list(_tmp4C5,_tmp4BE);return;}
goto _LL294;_LL2A3: _tmp4C6=_tmp495.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4C7=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4C6;if(_tmp4C7->tag != 4)
goto _LL2A5;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C8.field_info;if((_tmp4C9.KnownDatatypefield).tag
!= 2)goto _LL2A5;_tmp4CA=(struct _tuple3)(_tmp4C9.KnownDatatypefield).val;_tmp4CB=
_tmp4CA.f1;_tmp4CC=_tmp4CA.f2;_tmp4CD=_tmp4C8.targs;}};_tmp4CE=_tmp495.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4CF=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp4CE;if(_tmp4CF->tag != 4)goto _LL2A5;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4D0.field_info;
if((_tmp4D1.KnownDatatypefield).tag != 2)goto _LL2A5;_tmp4D2=(struct _tuple3)(
_tmp4D1.KnownDatatypefield).val;_tmp4D3=_tmp4D2.f1;_tmp4D4=_tmp4D2.f2;_tmp4D5=
_tmp4D0.targs;}};_LL2A4: if((_tmp4CB == _tmp4D3  || Cyc_Absyn_qvar_cmp(_tmp4CB->name,
_tmp4D3->name)== 0) && (_tmp4CC == _tmp4D4  || Cyc_Absyn_qvar_cmp(_tmp4CC->name,
_tmp4D4->name)== 0)){Cyc_Tcutil_unify_list(_tmp4D5,_tmp4CD);return;}{const char*
_tmp126C;Cyc_Tcutil_failure_reason=((_tmp126C="(different datatype field types)",
_tag_dyneither(_tmp126C,sizeof(char),33)));}goto _LL294;_LL2A5: _tmp4D6=_tmp495.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4D7=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D6;if(_tmp4D7->tag != 5)goto _LL2A7;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D8.elt_typ;
_tmp4DA=_tmp4D8.elt_tq;_tmp4DB=_tmp4D8.ptr_atts;_tmp4DC=_tmp4DB.rgn;_tmp4DD=
_tmp4DB.nullable;_tmp4DE=_tmp4DB.bounds;_tmp4DF=_tmp4DB.zero_term;}};_tmp4E0=
_tmp495.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4E1=(struct Cyc_Absyn_PointerType_struct*)
_tmp4E0;if(_tmp4E1->tag != 5)goto _LL2A7;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E2.elt_typ;
_tmp4E4=_tmp4E2.elt_tq;_tmp4E5=_tmp4E2.ptr_atts;_tmp4E6=_tmp4E5.rgn;_tmp4E7=
_tmp4E5.nullable;_tmp4E8=_tmp4E5.bounds;_tmp4E9=_tmp4E5.zero_term;}};_LL2A6: Cyc_Tcutil_unify_it(
_tmp4E3,_tmp4D9);Cyc_Tcutil_unify_it(_tmp4DC,_tmp4E6);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp126D;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4E9,_tmp4DF,((_tmp126D="(not both zero terminated)",
_tag_dyneither(_tmp126D,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4E4,
_tmp4E3,_tmp4DA,_tmp4D9);{const char*_tmp126E;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4E8,_tmp4DE,((
_tmp126E="(different pointer bounds)",_tag_dyneither(_tmp126E,sizeof(char),27))));}{
void*_tmp57D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4E8);_LL2D2: {struct Cyc_Absyn_DynEither_b_struct*
_tmp57E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp57D;if(_tmp57E->tag != 0)goto
_LL2D4;}_LL2D3: return;_LL2D4:;_LL2D5: goto _LL2D1;_LL2D1:;}{const char*_tmp126F;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4E7,_tmp4DD,((_tmp126F="(incompatible pointer types)",_tag_dyneither(
_tmp126F,sizeof(char),29))));}return;_LL2A7: _tmp4EA=_tmp495.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4EB=(struct Cyc_Absyn_IntType_struct*)_tmp4EA;if(_tmp4EB->tag != 6)goto _LL2A9;
else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}};_tmp4EE=_tmp495.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4EF=(struct Cyc_Absyn_IntType_struct*)_tmp4EE;if(_tmp4EF->tag != 6)goto _LL2A9;
else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}};_LL2A8: if(_tmp4F0 == _tmp4EC  && ((
_tmp4F1 == _tmp4ED  || _tmp4F1 == Cyc_Absyn_Int_sz  && _tmp4ED == Cyc_Absyn_Long_sz)
 || _tmp4F1 == Cyc_Absyn_Long_sz  && _tmp4ED == Cyc_Absyn_Int_sz))return;{const char*
_tmp1270;Cyc_Tcutil_failure_reason=((_tmp1270="(different integral types)",
_tag_dyneither(_tmp1270,sizeof(char),27)));}goto _LL294;_LL2A9: _tmp4F2=_tmp495.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp4F3=(struct Cyc_Absyn_FloatType_struct*)
_tmp4F2;if(_tmp4F3->tag != 7)goto _LL2AB;else{_tmp4F4=_tmp4F3->f1;}};_tmp4F5=
_tmp495.f2;{struct Cyc_Absyn_FloatType_struct*_tmp4F6=(struct Cyc_Absyn_FloatType_struct*)
_tmp4F5;if(_tmp4F6->tag != 7)goto _LL2AB;else{_tmp4F7=_tmp4F6->f1;}};_LL2AA: if(
_tmp4F7 == 0  && _tmp4F4 == 0)return;else{if(_tmp4F7 == 1  && _tmp4F4 == 1)return;
else{if(((_tmp4F7 != 0  && _tmp4F7 != 1) && _tmp4F4 != 0) && _tmp4F4 != 1)return;}}
goto _LL294;_LL2AB: _tmp4F8=_tmp495.f1;{struct Cyc_Absyn_TagType_struct*_tmp4F9=(
struct Cyc_Absyn_TagType_struct*)_tmp4F8;if(_tmp4F9->tag != 19)goto _LL2AD;else{
_tmp4FA=(void*)_tmp4F9->f1;}};_tmp4FB=_tmp495.f2;{struct Cyc_Absyn_TagType_struct*
_tmp4FC=(struct Cyc_Absyn_TagType_struct*)_tmp4FB;if(_tmp4FC->tag != 19)goto _LL2AD;
else{_tmp4FD=(void*)_tmp4FC->f1;}};_LL2AC: Cyc_Tcutil_unify_it(_tmp4FA,_tmp4FD);
return;_LL2AD: _tmp4FE=_tmp495.f1;{struct Cyc_Absyn_ValueofType_struct*_tmp4FF=(
struct Cyc_Absyn_ValueofType_struct*)_tmp4FE;if(_tmp4FF->tag != 18)goto _LL2AF;
else{_tmp500=_tmp4FF->f1;}};_tmp501=_tmp495.f2;{struct Cyc_Absyn_ValueofType_struct*
_tmp502=(struct Cyc_Absyn_ValueofType_struct*)_tmp501;if(_tmp502->tag != 18)goto
_LL2AF;else{_tmp503=_tmp502->f1;}};_LL2AE: if(!Cyc_Evexp_same_const_exp(_tmp500,
_tmp503)){{const char*_tmp1271;Cyc_Tcutil_failure_reason=((_tmp1271="(cannot prove expressions are the same)",
_tag_dyneither(_tmp1271,sizeof(char),40)));}goto _LL294;}return;_LL2AF: _tmp504=
_tmp495.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp505=(struct Cyc_Absyn_ArrayType_struct*)
_tmp504;if(_tmp505->tag != 8)goto _LL2B1;else{_tmp506=_tmp505->f1;_tmp507=_tmp506.elt_type;
_tmp508=_tmp506.tq;_tmp509=_tmp506.num_elts;_tmp50A=_tmp506.zero_term;}};_tmp50B=
_tmp495.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp50C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp50B;if(_tmp50C->tag != 8)goto _LL2B1;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.elt_type;
_tmp50F=_tmp50D.tq;_tmp510=_tmp50D.num_elts;_tmp511=_tmp50D.zero_term;}};_LL2B0:
Cyc_Tcutil_unify_it(_tmp50E,_tmp507);Cyc_Tcutil_unify_tqual(_tmp50F,_tmp50E,
_tmp508,_tmp507);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp1272;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp50A,_tmp511,((_tmp1272="(not both zero terminated)",_tag_dyneither(_tmp1272,
sizeof(char),27))));}if(_tmp509 == _tmp510)return;if(_tmp509 == 0  || _tmp510 == 0)
goto _LL294;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp509,(struct Cyc_Absyn_Exp*)
_tmp510))return;{const char*_tmp1273;Cyc_Tcutil_failure_reason=((_tmp1273="(different array sizes)",
_tag_dyneither(_tmp1273,sizeof(char),24)));}goto _LL294;_LL2B1: _tmp512=_tmp495.f1;{
struct Cyc_Absyn_FnType_struct*_tmp513=(struct Cyc_Absyn_FnType_struct*)_tmp512;
if(_tmp513->tag != 9)goto _LL2B3;else{_tmp514=_tmp513->f1;_tmp515=_tmp514.tvars;
_tmp516=_tmp514.effect;_tmp517=_tmp514.ret_tqual;_tmp518=_tmp514.ret_typ;_tmp519=
_tmp514.args;_tmp51A=_tmp514.c_varargs;_tmp51B=_tmp514.cyc_varargs;_tmp51C=
_tmp514.rgn_po;_tmp51D=_tmp514.attributes;}};_tmp51E=_tmp495.f2;{struct Cyc_Absyn_FnType_struct*
_tmp51F=(struct Cyc_Absyn_FnType_struct*)_tmp51E;if(_tmp51F->tag != 9)goto _LL2B3;
else{_tmp520=_tmp51F->f1;_tmp521=_tmp520.tvars;_tmp522=_tmp520.effect;_tmp523=
_tmp520.ret_tqual;_tmp524=_tmp520.ret_typ;_tmp525=_tmp520.args;_tmp526=_tmp520.c_varargs;
_tmp527=_tmp520.cyc_varargs;_tmp528=_tmp520.rgn_po;_tmp529=_tmp520.attributes;}};
_LL2B2: {int done=0;struct _RegionHandle _tmp584=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp584;_push_region(rgn);{struct Cyc_List_List*inst=0;while(
_tmp521 != 0){if(_tmp515 == 0){{const char*_tmp1274;Cyc_Tcutil_failure_reason=((
_tmp1274="(second function type has too few type variables)",_tag_dyneither(
_tmp1274,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp586=((struct Cyc_Absyn_Tvar*)_tmp521->hd)->kind;void*_tmp587=((struct Cyc_Absyn_Tvar*)
_tmp515->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp586,_tmp587)){{const char*
_tmp127A;void*_tmp1279[3];struct Cyc_String_pa_struct _tmp1278;struct Cyc_String_pa_struct
_tmp1277;struct Cyc_String_pa_struct _tmp1276;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1276.tag=0,((_tmp1276.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp515->hd,& Cyc_Tcutil_bk))),((_tmp1277.tag=0,((_tmp1277.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp521->hd,& Cyc_Tcutil_bk))),((_tmp1278.tag=0,((_tmp1278.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp521->hd)),((_tmp1279[0]=& _tmp1278,((_tmp1279[1]=& _tmp1277,((_tmp1279[2]=&
_tmp1276,Cyc_aprintf(((_tmp127A="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp127A,sizeof(char),44))),_tag_dyneither(_tmp1279,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple16*_tmp1284;struct Cyc_Absyn_VarType_struct _tmp1283;struct Cyc_Absyn_VarType_struct*
_tmp1282;struct Cyc_List_List*_tmp1281;inst=((_tmp1281=_region_malloc(rgn,sizeof(*
_tmp1281)),((_tmp1281->hd=((_tmp1284=_region_malloc(rgn,sizeof(*_tmp1284)),((
_tmp1284->f1=(struct Cyc_Absyn_Tvar*)_tmp515->hd,((_tmp1284->f2=(void*)((_tmp1282=
_cycalloc(sizeof(*_tmp1282)),((_tmp1282[0]=((_tmp1283.tag=2,((_tmp1283.f1=(
struct Cyc_Absyn_Tvar*)_tmp521->hd,_tmp1283)))),_tmp1282)))),_tmp1284)))))),((
_tmp1281->tl=inst,_tmp1281))))));}_tmp521=_tmp521->tl;_tmp515=_tmp515->tl;};}if(
_tmp515 != 0){{const char*_tmp1285;Cyc_Tcutil_failure_reason=((_tmp1285="(second function type has too many type variables)",
_tag_dyneither(_tmp1285,sizeof(char),51)));}_npop_handler(0);goto _LL294;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp1291;struct Cyc_Absyn_FnInfo _tmp1290;
struct Cyc_Absyn_FnType_struct*_tmp128F;struct Cyc_Absyn_FnType_struct _tmp128B;
struct Cyc_Absyn_FnInfo _tmp128A;struct Cyc_Absyn_FnType_struct*_tmp1289;Cyc_Tcutil_unify_it((
void*)((_tmp1289=_cycalloc(sizeof(*_tmp1289)),((_tmp1289[0]=((_tmp128B.tag=9,((
_tmp128B.f1=((_tmp128A.tvars=0,((_tmp128A.effect=_tmp522,((_tmp128A.ret_tqual=
_tmp523,((_tmp128A.ret_typ=_tmp524,((_tmp128A.args=_tmp525,((_tmp128A.c_varargs=
_tmp526,((_tmp128A.cyc_varargs=_tmp527,((_tmp128A.rgn_po=_tmp528,((_tmp128A.attributes=
_tmp529,_tmp128A)))))))))))))))))),_tmp128B)))),_tmp1289)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp128F=_cycalloc(sizeof(*_tmp128F)),((_tmp128F[0]=((_tmp1291.tag=
9,((_tmp1291.f1=((_tmp1290.tvars=0,((_tmp1290.effect=_tmp516,((_tmp1290.ret_tqual=
_tmp517,((_tmp1290.ret_typ=_tmp518,((_tmp1290.args=_tmp519,((_tmp1290.c_varargs=
_tmp51A,((_tmp1290.cyc_varargs=_tmp51B,((_tmp1290.rgn_po=_tmp51C,((_tmp1290.attributes=
_tmp51D,_tmp1290)))))))))))))))))),_tmp1291)))),_tmp128F))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp524,_tmp518);Cyc_Tcutil_unify_tqual(
_tmp523,_tmp524,_tmp517,_tmp518);for(0;_tmp525 != 0  && _tmp519 != 0;(_tmp525=
_tmp525->tl,_tmp519=_tmp519->tl)){struct Cyc_Absyn_Tqual _tmp599;void*_tmp59A;
struct _tuple10 _tmp598=*((struct _tuple10*)_tmp525->hd);_tmp599=_tmp598.f2;_tmp59A=
_tmp598.f3;{struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;struct _tuple10 _tmp59B=*((
struct _tuple10*)_tmp519->hd);_tmp59C=_tmp59B.f2;_tmp59D=_tmp59B.f3;Cyc_Tcutil_unify_it(
_tmp59A,_tmp59D);Cyc_Tcutil_unify_tqual(_tmp599,_tmp59A,_tmp59C,_tmp59D);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp525 != 0  || _tmp519 != 0){{const char*_tmp1292;
Cyc_Tcutil_failure_reason=((_tmp1292="(function types have different number of arguments)",
_tag_dyneither(_tmp1292,sizeof(char),52)));}_npop_handler(0);goto _LL294;}if(
_tmp526 != _tmp51A){{const char*_tmp1293;Cyc_Tcutil_failure_reason=((_tmp1293="(only one function type takes C varargs)",
_tag_dyneither(_tmp1293,sizeof(char),41)));}_npop_handler(0);goto _LL294;}{int
bad_cyc_vararg=0;{struct _tuple22 _tmp1294;struct _tuple22 _tmp5A1=(_tmp1294.f1=
_tmp527,((_tmp1294.f2=_tmp51B,_tmp1294)));struct Cyc_Absyn_VarargInfo*_tmp5A2;
struct Cyc_Absyn_VarargInfo*_tmp5A3;struct Cyc_Absyn_VarargInfo*_tmp5A4;struct Cyc_Absyn_VarargInfo*
_tmp5A5;struct Cyc_Absyn_VarargInfo*_tmp5A6;struct Cyc_Absyn_VarargInfo _tmp5A7;
struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5AA;int _tmp5AB;
struct Cyc_Absyn_VarargInfo*_tmp5AC;struct Cyc_Absyn_VarargInfo _tmp5AD;struct Cyc_Core_Opt*
_tmp5AE;struct Cyc_Absyn_Tqual _tmp5AF;void*_tmp5B0;int _tmp5B1;_LL2D7: _tmp5A2=
_tmp5A1.f1;if(_tmp5A2 != 0)goto _LL2D9;_tmp5A3=_tmp5A1.f2;if(_tmp5A3 != 0)goto
_LL2D9;_LL2D8: goto _LL2D6;_LL2D9: _tmp5A4=_tmp5A1.f1;if(_tmp5A4 != 0)goto _LL2DB;
_LL2DA: goto _LL2DC;_LL2DB: _tmp5A5=_tmp5A1.f2;if(_tmp5A5 != 0)goto _LL2DD;_LL2DC:
bad_cyc_vararg=1;{const char*_tmp1295;Cyc_Tcutil_failure_reason=((_tmp1295="(only one function type takes varargs)",
_tag_dyneither(_tmp1295,sizeof(char),39)));}goto _LL2D6;_LL2DD: _tmp5A6=_tmp5A1.f1;
if(_tmp5A6 == 0)goto _LL2D6;_tmp5A7=*_tmp5A6;_tmp5A8=_tmp5A7.name;_tmp5A9=_tmp5A7.tq;
_tmp5AA=_tmp5A7.type;_tmp5AB=_tmp5A7.inject;_tmp5AC=_tmp5A1.f2;if(_tmp5AC == 0)
goto _LL2D6;_tmp5AD=*_tmp5AC;_tmp5AE=_tmp5AD.name;_tmp5AF=_tmp5AD.tq;_tmp5B0=
_tmp5AD.type;_tmp5B1=_tmp5AD.inject;_LL2DE: Cyc_Tcutil_unify_it(_tmp5AA,_tmp5B0);
Cyc_Tcutil_unify_tqual(_tmp5A9,_tmp5AA,_tmp5AF,_tmp5B0);if(_tmp5AB != _tmp5B1){
bad_cyc_vararg=1;{const char*_tmp1296;Cyc_Tcutil_failure_reason=((_tmp1296="(only one function type injects varargs)",
_tag_dyneither(_tmp1296,sizeof(char),41)));};}goto _LL2D6;_LL2D6:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL294;}{int bad_effect=0;{struct _tuple23
_tmp1297;struct _tuple23 _tmp5B5=(_tmp1297.f1=_tmp522,((_tmp1297.f2=_tmp516,
_tmp1297)));struct Cyc_Core_Opt*_tmp5B6;struct Cyc_Core_Opt*_tmp5B7;struct Cyc_Core_Opt*
_tmp5B8;struct Cyc_Core_Opt*_tmp5B9;_LL2E0: _tmp5B6=_tmp5B5.f1;if(_tmp5B6 != 0)goto
_LL2E2;_tmp5B7=_tmp5B5.f2;if(_tmp5B7 != 0)goto _LL2E2;_LL2E1: goto _LL2DF;_LL2E2:
_tmp5B8=_tmp5B5.f1;if(_tmp5B8 != 0)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4: _tmp5B9=
_tmp5B5.f2;if(_tmp5B9 != 0)goto _LL2E6;_LL2E5: bad_effect=1;goto _LL2DF;_LL2E6:;
_LL2E7: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp522))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp516))->v);
goto _LL2DF;_LL2DF:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1298;Cyc_Tcutil_failure_reason=((_tmp1298="(function type effects do not unify)",
_tag_dyneither(_tmp1298,sizeof(char),37)));}_npop_handler(0);goto _LL294;}if(!Cyc_Tcutil_same_atts(
_tmp51D,_tmp529)){{const char*_tmp1299;Cyc_Tcutil_failure_reason=((_tmp1299="(function types have different attributes)",
_tag_dyneither(_tmp1299,sizeof(char),43)));}_npop_handler(0);goto _LL294;}if(!Cyc_Tcutil_same_rgn_po(
_tmp51C,_tmp528)){{const char*_tmp129A;Cyc_Tcutil_failure_reason=((_tmp129A="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp129A,sizeof(char),58)));}_npop_handler(0);goto _LL294;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL2B3: _tmp52A=_tmp495.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp52B=(struct Cyc_Absyn_TupleType_struct*)_tmp52A;
if(_tmp52B->tag != 10)goto _LL2B5;else{_tmp52C=_tmp52B->f1;}};_tmp52D=_tmp495.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp52E=(struct Cyc_Absyn_TupleType_struct*)
_tmp52D;if(_tmp52E->tag != 10)goto _LL2B5;else{_tmp52F=_tmp52E->f1;}};_LL2B4: for(0;
_tmp52F != 0  && _tmp52C != 0;(_tmp52F=_tmp52F->tl,_tmp52C=_tmp52C->tl)){struct Cyc_Absyn_Tqual
_tmp5BE;void*_tmp5BF;struct _tuple12 _tmp5BD=*((struct _tuple12*)_tmp52F->hd);
_tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;{struct Cyc_Absyn_Tqual _tmp5C1;void*_tmp5C2;
struct _tuple12 _tmp5C0=*((struct _tuple12*)_tmp52C->hd);_tmp5C1=_tmp5C0.f1;_tmp5C2=
_tmp5C0.f2;Cyc_Tcutil_unify_it(_tmp5BF,_tmp5C2);Cyc_Tcutil_unify_tqual(_tmp5BE,
_tmp5BF,_tmp5C1,_tmp5C2);};}if(_tmp52F == 0  && _tmp52C == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp129B;Cyc_Tcutil_failure_reason=((
_tmp129B="(tuple types have different numbers of components)",_tag_dyneither(
_tmp129B,sizeof(char),51)));}goto _LL294;_LL2B5: _tmp530=_tmp495.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp531=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp530;if(_tmp531->tag != 12)goto
_LL2B7;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;}};_tmp534=_tmp495.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp535=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp534;if(_tmp535->tag != 12)goto _LL2B7;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;}};
_LL2B6: if(_tmp536 != _tmp532){{const char*_tmp129C;Cyc_Tcutil_failure_reason=((
_tmp129C="(struct and union type)",_tag_dyneither(_tmp129C,sizeof(char),24)));}
goto _LL294;}for(0;_tmp537 != 0  && _tmp533 != 0;(_tmp537=_tmp537->tl,_tmp533=
_tmp533->tl)){struct Cyc_Absyn_Aggrfield*_tmp5C5=(struct Cyc_Absyn_Aggrfield*)
_tmp537->hd;struct Cyc_Absyn_Aggrfield*_tmp5C6=(struct Cyc_Absyn_Aggrfield*)
_tmp533->hd;if(Cyc_strptrcmp(_tmp5C5->name,_tmp5C6->name)!= 0){{const char*
_tmp129D;Cyc_Tcutil_failure_reason=((_tmp129D="(different member names)",
_tag_dyneither(_tmp129D,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5C5->type,_tmp5C6->type);Cyc_Tcutil_unify_tqual(_tmp5C5->tq,
_tmp5C5->type,_tmp5C6->tq,_tmp5C6->type);if(!Cyc_Tcutil_same_atts(_tmp5C5->attributes,
_tmp5C6->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp129E;Cyc_Tcutil_failure_reason=((_tmp129E="(different attributes on member)",
_tag_dyneither(_tmp129E,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5C5->width != 0  && _tmp5C6->width == 0  || _tmp5C6->width != 0  && _tmp5C5->width
== 0) || (_tmp5C5->width != 0  && _tmp5C6->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5C5->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5C6->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp129F;Cyc_Tcutil_failure_reason=((_tmp129F="(different bitfield widths on member)",
_tag_dyneither(_tmp129F,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp537 == 0  && _tmp533 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp12A0;Cyc_Tcutil_failure_reason=((_tmp12A0="(different number of members)",
_tag_dyneither(_tmp12A0,sizeof(char),30)));}goto _LL294;_LL2B7: _tmp538=_tmp495.f1;{
struct Cyc_Absyn_TypedefType_struct*_tmp539=(struct Cyc_Absyn_TypedefType_struct*)
_tmp538;if(_tmp539->tag != 17)goto _LL2B9;else{_tmp53A=_tmp539->f2;_tmp53B=_tmp539->f3;}};
_tmp53C=_tmp495.f2;{struct Cyc_Absyn_TypedefType_struct*_tmp53D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp53C;if(_tmp53D->tag != 17)goto _LL2B9;else{_tmp53E=_tmp53D->f2;_tmp53F=_tmp53D->f3;}};
_LL2B8: if(_tmp53B != _tmp53F){{const char*_tmp12A1;Cyc_Tcutil_failure_reason=((
_tmp12A1="(different abstract typedefs)",_tag_dyneither(_tmp12A1,sizeof(char),30)));}
goto _LL294;}{const char*_tmp12A2;Cyc_Tcutil_failure_reason=((_tmp12A2="(type parameters to typedef differ)",
_tag_dyneither(_tmp12A2,sizeof(char),36)));}Cyc_Tcutil_unify_list(_tmp53A,
_tmp53E);return;_LL2B9: _tmp540=_tmp495.f1;{struct Cyc_Absyn_HeapRgn_struct*
_tmp541=(struct Cyc_Absyn_HeapRgn_struct*)_tmp540;if(_tmp541->tag != 20)goto _LL2BB;};
_tmp542=_tmp495.f2;{struct Cyc_Absyn_HeapRgn_struct*_tmp543=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp542;if(_tmp543->tag != 20)goto _LL2BB;};_LL2BA: return;_LL2BB: _tmp544=_tmp495.f1;{
struct Cyc_Absyn_UniqueRgn_struct*_tmp545=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp544;if(_tmp545->tag != 21)goto _LL2BD;};_tmp546=_tmp495.f2;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp547=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp546;if(_tmp547->tag != 21)goto
_LL2BD;};_LL2BC: return;_LL2BD: _tmp548=_tmp495.f1;{struct Cyc_Absyn_RefCntRgn_struct*
_tmp549=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp548;if(_tmp549->tag != 22)goto
_LL2BF;};_tmp54A=_tmp495.f2;{struct Cyc_Absyn_RefCntRgn_struct*_tmp54B=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp54A;if(_tmp54B->tag != 22)goto _LL2BF;};_LL2BE: return;_LL2BF: _tmp54C=_tmp495.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp54D=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp54C;if(_tmp54D->tag != 15)goto _LL2C1;else{_tmp54E=(void*)_tmp54D->f1;}};
_tmp54F=_tmp495.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp550=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp54F;if(_tmp550->tag != 15)goto _LL2C1;else{_tmp551=(void*)_tmp550->f1;}};
_LL2C0: Cyc_Tcutil_unify_it(_tmp54E,_tmp551);return;_LL2C1: _tmp552=_tmp495.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp553=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp552;if(_tmp553->tag != 16)goto _LL2C3;else{_tmp554=(void*)_tmp553->f1;_tmp555=(
void*)_tmp553->f2;}};_tmp556=_tmp495.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp557=(struct Cyc_Absyn_DynRgnType_struct*)_tmp556;if(_tmp557->tag != 16)goto
_LL2C3;else{_tmp558=(void*)_tmp557->f1;_tmp559=(void*)_tmp557->f2;}};_LL2C2: Cyc_Tcutil_unify_it(
_tmp554,_tmp558);Cyc_Tcutil_unify_it(_tmp555,_tmp559);return;_LL2C3: _tmp55A=
_tmp495.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp55B=(struct Cyc_Absyn_JoinEff_struct*)
_tmp55A;if(_tmp55B->tag != 24)goto _LL2C5;};_LL2C4: goto _LL2C6;_LL2C5: _tmp55C=
_tmp495.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp55D=(struct Cyc_Absyn_JoinEff_struct*)
_tmp55C;if(_tmp55D->tag != 24)goto _LL2C7;};_LL2C6: goto _LL2C8;_LL2C7: _tmp55E=
_tmp495.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp55F=(struct Cyc_Absyn_AccessEff_struct*)
_tmp55E;if(_tmp55F->tag != 23)goto _LL2C9;};_LL2C8: goto _LL2CA;_LL2C9: _tmp560=
_tmp495.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp561=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp560;if(_tmp561->tag != 25)goto _LL2CB;};_LL2CA: goto _LL2CC;_LL2CB: _tmp562=
_tmp495.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp563=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp562;if(_tmp563->tag != 25)goto _LL2CD;};_LL2CC: goto _LL2CE;_LL2CD: _tmp564=
_tmp495.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp565=(struct Cyc_Absyn_AccessEff_struct*)
_tmp564;if(_tmp565->tag != 23)goto _LL2CF;};_LL2CE: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp12A3;Cyc_Tcutil_failure_reason=((_tmp12A3="(effects don't unify)",
_tag_dyneither(_tmp12A3,sizeof(char),22)));}goto _LL294;_LL2CF:;_LL2D0: goto _LL294;
_LL294:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return
- 1;if(a1 != 0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)
_check_null(a2));}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp5CE=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp5CF=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp5CE,_tmp5CF);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint*
_tmp5D0=x;union Cyc_Absyn_Constraint _tmp5D1;int _tmp5D2;union Cyc_Absyn_Constraint
_tmp5D3;void*_tmp5D4;union Cyc_Absyn_Constraint _tmp5D5;union Cyc_Absyn_Constraint*
_tmp5D6;_LL2E9: _tmp5D1=*_tmp5D0;if((_tmp5D1.No_constr).tag != 3)goto _LL2EB;
_tmp5D2=(int)(_tmp5D1.No_constr).val;_LL2EA: return - 1;_LL2EB: _tmp5D3=*_tmp5D0;if((
_tmp5D3.Eq_constr).tag != 1)goto _LL2ED;_tmp5D4=(void*)(_tmp5D3.Eq_constr).val;
_LL2EC: {union Cyc_Absyn_Constraint*_tmp5D7=y;union Cyc_Absyn_Constraint _tmp5D8;
int _tmp5D9;union Cyc_Absyn_Constraint _tmp5DA;void*_tmp5DB;union Cyc_Absyn_Constraint
_tmp5DC;union Cyc_Absyn_Constraint*_tmp5DD;_LL2F0: _tmp5D8=*_tmp5D7;if((_tmp5D8.No_constr).tag
!= 3)goto _LL2F2;_tmp5D9=(int)(_tmp5D8.No_constr).val;_LL2F1: return 1;_LL2F2:
_tmp5DA=*_tmp5D7;if((_tmp5DA.Eq_constr).tag != 1)goto _LL2F4;_tmp5DB=(void*)(
_tmp5DA.Eq_constr).val;_LL2F3: return cmp(_tmp5D4,_tmp5DB);_LL2F4: _tmp5DC=*_tmp5D7;
if((_tmp5DC.Forward_constr).tag != 2)goto _LL2EF;_tmp5DD=(union Cyc_Absyn_Constraint*)(
_tmp5DC.Forward_constr).val;_LL2F5: {const char*_tmp12A6;void*_tmp12A5;(_tmp12A5=
0,Cyc_Tcutil_impos(((_tmp12A6="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp12A6,sizeof(char),40))),_tag_dyneither(_tmp12A5,sizeof(void*),
0)));}_LL2EF:;}_LL2ED: _tmp5D5=*_tmp5D0;if((_tmp5D5.Forward_constr).tag != 2)goto
_LL2E8;_tmp5D6=(union Cyc_Absyn_Constraint*)(_tmp5D5.Forward_constr).val;_LL2EE: {
const char*_tmp12A9;void*_tmp12A8;(_tmp12A8=0,Cyc_Tcutil_impos(((_tmp12A9="unify_conref: forward after compress",
_tag_dyneither(_tmp12A9,sizeof(char),37))),_tag_dyneither(_tmp12A8,sizeof(void*),
0)));}_LL2E8:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct
_tuple12*tqt2){struct _tuple12 _tmp5E3;struct Cyc_Absyn_Tqual _tmp5E4;void*_tmp5E5;
struct _tuple12*_tmp5E2=tqt1;_tmp5E3=*_tmp5E2;_tmp5E4=_tmp5E3.f1;_tmp5E5=_tmp5E3.f2;{
struct _tuple12 _tmp5E7;struct Cyc_Absyn_Tqual _tmp5E8;void*_tmp5E9;struct _tuple12*
_tmp5E6=tqt2;_tmp5E7=*_tmp5E6;_tmp5E8=_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;{int _tmp5EA=
Cyc_Tcutil_tqual_cmp(_tmp5E4,_tmp5E8);if(_tmp5EA != 0)return _tmp5EA;return Cyc_Tcutil_typecmp(
_tmp5E5,_tmp5E9);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp5EB=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp5EB != 0)return _tmp5EB;{int _tmp5EC=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp5EC != 0)return _tmp5EC;{int _tmp5ED=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp5ED != 0)return _tmp5ED;{int _tmp5EE=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp5EE != 0)return _tmp5EE;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp5EF=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp5EF != 0)return _tmp5EF;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp5F0=t;int _tmp5F9;_LL2F7: {struct Cyc_Absyn_VoidType_struct*
_tmp5F1=(struct Cyc_Absyn_VoidType_struct*)_tmp5F0;if(_tmp5F1->tag != 0)goto _LL2F9;}
_LL2F8: return 0;_LL2F9: {struct Cyc_Absyn_Evar_struct*_tmp5F2=(struct Cyc_Absyn_Evar_struct*)
_tmp5F0;if(_tmp5F2->tag != 1)goto _LL2FB;}_LL2FA: return 1;_LL2FB: {struct Cyc_Absyn_VarType_struct*
_tmp5F3=(struct Cyc_Absyn_VarType_struct*)_tmp5F0;if(_tmp5F3->tag != 2)goto _LL2FD;}
_LL2FC: return 2;_LL2FD: {struct Cyc_Absyn_DatatypeType_struct*_tmp5F4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp5F0;if(_tmp5F4->tag != 3)goto _LL2FF;}_LL2FE: return 3;_LL2FF: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp5F5=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp5F0;if(_tmp5F5->tag != 4)
goto _LL301;}_LL300: return 4;_LL301: {struct Cyc_Absyn_PointerType_struct*_tmp5F6=(
struct Cyc_Absyn_PointerType_struct*)_tmp5F0;if(_tmp5F6->tag != 5)goto _LL303;}
_LL302: return 5;_LL303: {struct Cyc_Absyn_IntType_struct*_tmp5F7=(struct Cyc_Absyn_IntType_struct*)
_tmp5F0;if(_tmp5F7->tag != 6)goto _LL305;}_LL304: return 6;_LL305: {struct Cyc_Absyn_FloatType_struct*
_tmp5F8=(struct Cyc_Absyn_FloatType_struct*)_tmp5F0;if(_tmp5F8->tag != 7)goto
_LL307;else{_tmp5F9=_tmp5F8->f1;if(_tmp5F9 != 0)goto _LL307;}}_LL306: return 7;
_LL307: {struct Cyc_Absyn_FloatType_struct*_tmp5FA=(struct Cyc_Absyn_FloatType_struct*)
_tmp5F0;if(_tmp5FA->tag != 7)goto _LL309;}_LL308: return 8;_LL309: {struct Cyc_Absyn_ArrayType_struct*
_tmp5FB=(struct Cyc_Absyn_ArrayType_struct*)_tmp5F0;if(_tmp5FB->tag != 8)goto
_LL30B;}_LL30A: return 9;_LL30B: {struct Cyc_Absyn_FnType_struct*_tmp5FC=(struct Cyc_Absyn_FnType_struct*)
_tmp5F0;if(_tmp5FC->tag != 9)goto _LL30D;}_LL30C: return 10;_LL30D: {struct Cyc_Absyn_TupleType_struct*
_tmp5FD=(struct Cyc_Absyn_TupleType_struct*)_tmp5F0;if(_tmp5FD->tag != 10)goto
_LL30F;}_LL30E: return 11;_LL30F: {struct Cyc_Absyn_AggrType_struct*_tmp5FE=(struct
Cyc_Absyn_AggrType_struct*)_tmp5F0;if(_tmp5FE->tag != 11)goto _LL311;}_LL310:
return 12;_LL311: {struct Cyc_Absyn_AnonAggrType_struct*_tmp5FF=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5F0;if(_tmp5FF->tag != 12)goto _LL313;}_LL312: return 14;_LL313: {struct Cyc_Absyn_EnumType_struct*
_tmp600=(struct Cyc_Absyn_EnumType_struct*)_tmp5F0;if(_tmp600->tag != 13)goto
_LL315;}_LL314: return 16;_LL315: {struct Cyc_Absyn_AnonEnumType_struct*_tmp601=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp5F0;if(_tmp601->tag != 14)goto _LL317;}
_LL316: return 17;_LL317: {struct Cyc_Absyn_RgnHandleType_struct*_tmp602=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5F0;if(_tmp602->tag != 15)goto _LL319;}_LL318: return 18;_LL319: {struct Cyc_Absyn_TypedefType_struct*
_tmp603=(struct Cyc_Absyn_TypedefType_struct*)_tmp5F0;if(_tmp603->tag != 17)goto
_LL31B;}_LL31A: return 19;_LL31B: {struct Cyc_Absyn_UniqueRgn_struct*_tmp604=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp5F0;if(_tmp604->tag != 21)goto _LL31D;}
_LL31C: return 20;_LL31D: {struct Cyc_Absyn_HeapRgn_struct*_tmp605=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp5F0;if(_tmp605->tag != 20)goto _LL31F;}_LL31E: return 21;_LL31F: {struct Cyc_Absyn_AccessEff_struct*
_tmp606=(struct Cyc_Absyn_AccessEff_struct*)_tmp5F0;if(_tmp606->tag != 23)goto
_LL321;}_LL320: return 22;_LL321: {struct Cyc_Absyn_JoinEff_struct*_tmp607=(struct
Cyc_Absyn_JoinEff_struct*)_tmp5F0;if(_tmp607->tag != 24)goto _LL323;}_LL322: return
23;_LL323: {struct Cyc_Absyn_RgnsEff_struct*_tmp608=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp5F0;if(_tmp608->tag != 25)goto _LL325;}_LL324: return 24;_LL325: {struct Cyc_Absyn_RefCntRgn_struct*
_tmp609=(struct Cyc_Absyn_RefCntRgn_struct*)_tmp5F0;if(_tmp609->tag != 22)goto
_LL327;}_LL326: return 25;_LL327: {struct Cyc_Absyn_TagType_struct*_tmp60A=(struct
Cyc_Absyn_TagType_struct*)_tmp5F0;if(_tmp60A->tag != 19)goto _LL329;}_LL328: return
26;_LL329: {struct Cyc_Absyn_DynRgnType_struct*_tmp60B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp5F0;if(_tmp60B->tag != 16)goto _LL32B;}_LL32A: return 27;_LL32B: {struct Cyc_Absyn_ValueofType_struct*
_tmp60C=(struct Cyc_Absyn_ValueofType_struct*)_tmp5F0;if(_tmp60C->tag != 18)goto
_LL32D;}_LL32C: return 28;_LL32D: {struct Cyc_Absyn_TypeDeclType_struct*_tmp60D=(
struct Cyc_Absyn_TypeDeclType_struct*)_tmp5F0;if(_tmp60D->tag != 26)goto _LL2F6;}
_LL32E: return 29;_LL2F6:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp60E=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp60E != 0)
return _tmp60E;{struct _tuple0 _tmp12AA;struct _tuple0 _tmp610=(_tmp12AA.f1=t2,((
_tmp12AA.f2=t1,_tmp12AA)));void*_tmp611;void*_tmp613;void*_tmp615;struct Cyc_Absyn_Tvar*
_tmp617;void*_tmp618;struct Cyc_Absyn_Tvar*_tmp61A;void*_tmp61B;struct Cyc_Absyn_AggrInfo
_tmp61D;union Cyc_Absyn_AggrInfoU _tmp61E;struct Cyc_List_List*_tmp61F;void*_tmp620;
struct Cyc_Absyn_AggrInfo _tmp622;union Cyc_Absyn_AggrInfoU _tmp623;struct Cyc_List_List*
_tmp624;void*_tmp625;struct _tuple2*_tmp627;void*_tmp628;struct _tuple2*_tmp62A;
void*_tmp62B;struct Cyc_List_List*_tmp62D;void*_tmp62E;struct Cyc_List_List*
_tmp630;void*_tmp631;struct Cyc_Absyn_DatatypeInfo _tmp633;union Cyc_Absyn_DatatypeInfoU
_tmp634;struct Cyc_Absyn_Datatypedecl**_tmp635;struct Cyc_Absyn_Datatypedecl*
_tmp636;struct Cyc_List_List*_tmp637;void*_tmp638;struct Cyc_Absyn_DatatypeInfo
_tmp63A;union Cyc_Absyn_DatatypeInfoU _tmp63B;struct Cyc_Absyn_Datatypedecl**
_tmp63C;struct Cyc_Absyn_Datatypedecl*_tmp63D;struct Cyc_List_List*_tmp63E;void*
_tmp63F;struct Cyc_Absyn_DatatypeFieldInfo _tmp641;union Cyc_Absyn_DatatypeFieldInfoU
_tmp642;struct _tuple3 _tmp643;struct Cyc_Absyn_Datatypedecl*_tmp644;struct Cyc_Absyn_Datatypefield*
_tmp645;struct Cyc_List_List*_tmp646;void*_tmp647;struct Cyc_Absyn_DatatypeFieldInfo
_tmp649;union Cyc_Absyn_DatatypeFieldInfoU _tmp64A;struct _tuple3 _tmp64B;struct Cyc_Absyn_Datatypedecl*
_tmp64C;struct Cyc_Absyn_Datatypefield*_tmp64D;struct Cyc_List_List*_tmp64E;void*
_tmp64F;struct Cyc_Absyn_PtrInfo _tmp651;void*_tmp652;struct Cyc_Absyn_Tqual _tmp653;
struct Cyc_Absyn_PtrAtts _tmp654;void*_tmp655;union Cyc_Absyn_Constraint*_tmp656;
union Cyc_Absyn_Constraint*_tmp657;union Cyc_Absyn_Constraint*_tmp658;void*_tmp659;
struct Cyc_Absyn_PtrInfo _tmp65B;void*_tmp65C;struct Cyc_Absyn_Tqual _tmp65D;struct
Cyc_Absyn_PtrAtts _tmp65E;void*_tmp65F;union Cyc_Absyn_Constraint*_tmp660;union Cyc_Absyn_Constraint*
_tmp661;union Cyc_Absyn_Constraint*_tmp662;void*_tmp663;enum Cyc_Absyn_Sign
_tmp665;enum Cyc_Absyn_Size_of _tmp666;void*_tmp667;enum Cyc_Absyn_Sign _tmp669;
enum Cyc_Absyn_Size_of _tmp66A;void*_tmp66B;int _tmp66D;void*_tmp66E;int _tmp670;
void*_tmp671;struct Cyc_Absyn_ArrayInfo _tmp673;void*_tmp674;struct Cyc_Absyn_Tqual
_tmp675;struct Cyc_Absyn_Exp*_tmp676;union Cyc_Absyn_Constraint*_tmp677;void*
_tmp678;struct Cyc_Absyn_ArrayInfo _tmp67A;void*_tmp67B;struct Cyc_Absyn_Tqual
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
_tmp6C3;void*_tmp6C5;void*_tmp6C7;void*_tmp6C9;_LL330: _tmp611=_tmp610.f1;{struct
Cyc_Absyn_Evar_struct*_tmp612=(struct Cyc_Absyn_Evar_struct*)_tmp611;if(_tmp612->tag
!= 1)goto _LL332;};_tmp613=_tmp610.f2;{struct Cyc_Absyn_Evar_struct*_tmp614=(
struct Cyc_Absyn_Evar_struct*)_tmp613;if(_tmp614->tag != 1)goto _LL332;};_LL331: {
const char*_tmp12AD;void*_tmp12AC;(_tmp12AC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12AD="typecmp: can only compare closed types",
_tag_dyneither(_tmp12AD,sizeof(char),39))),_tag_dyneither(_tmp12AC,sizeof(void*),
0)));}_LL332: _tmp615=_tmp610.f1;{struct Cyc_Absyn_VarType_struct*_tmp616=(struct
Cyc_Absyn_VarType_struct*)_tmp615;if(_tmp616->tag != 2)goto _LL334;else{_tmp617=
_tmp616->f1;}};_tmp618=_tmp610.f2;{struct Cyc_Absyn_VarType_struct*_tmp619=(
struct Cyc_Absyn_VarType_struct*)_tmp618;if(_tmp619->tag != 2)goto _LL334;else{
_tmp61A=_tmp619->f1;}};_LL333: return Cyc_Core_intcmp(_tmp61A->identity,_tmp617->identity);
_LL334: _tmp61B=_tmp610.f1;{struct Cyc_Absyn_AggrType_struct*_tmp61C=(struct Cyc_Absyn_AggrType_struct*)
_tmp61B;if(_tmp61C->tag != 11)goto _LL336;else{_tmp61D=_tmp61C->f1;_tmp61E=_tmp61D.aggr_info;
_tmp61F=_tmp61D.targs;}};_tmp620=_tmp610.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp621=(struct Cyc_Absyn_AggrType_struct*)_tmp620;if(_tmp621->tag != 11)goto
_LL336;else{_tmp622=_tmp621->f1;_tmp623=_tmp622.aggr_info;_tmp624=_tmp622.targs;}};
_LL335: {struct _tuple2*_tmp6CE;struct _tuple13 _tmp6CD=Cyc_Absyn_aggr_kinded_name(
_tmp61E);_tmp6CE=_tmp6CD.f2;{struct _tuple2*_tmp6D0;struct _tuple13 _tmp6CF=Cyc_Absyn_aggr_kinded_name(
_tmp623);_tmp6D0=_tmp6CF.f2;{int _tmp6D1=Cyc_Absyn_qvar_cmp(_tmp6CE,_tmp6D0);if(
_tmp6D1 != 0)return _tmp6D1;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp61F,_tmp624);}};};}
_LL336: _tmp625=_tmp610.f1;{struct Cyc_Absyn_EnumType_struct*_tmp626=(struct Cyc_Absyn_EnumType_struct*)
_tmp625;if(_tmp626->tag != 13)goto _LL338;else{_tmp627=_tmp626->f1;}};_tmp628=
_tmp610.f2;{struct Cyc_Absyn_EnumType_struct*_tmp629=(struct Cyc_Absyn_EnumType_struct*)
_tmp628;if(_tmp629->tag != 13)goto _LL338;else{_tmp62A=_tmp629->f1;}};_LL337:
return Cyc_Absyn_qvar_cmp(_tmp627,_tmp62A);_LL338: _tmp62B=_tmp610.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp62C=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp62B;if(_tmp62C->tag != 14)goto
_LL33A;else{_tmp62D=_tmp62C->f1;}};_tmp62E=_tmp610.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp62F=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp62E;if(_tmp62F->tag != 14)goto
_LL33A;else{_tmp630=_tmp62F->f1;}};_LL339: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp62D,_tmp630);_LL33A: _tmp631=_tmp610.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp632=(struct Cyc_Absyn_DatatypeType_struct*)_tmp631;if(_tmp632->tag != 3)goto
_LL33C;else{_tmp633=_tmp632->f1;_tmp634=_tmp633.datatype_info;if((_tmp634.KnownDatatype).tag
!= 2)goto _LL33C;_tmp635=(struct Cyc_Absyn_Datatypedecl**)(_tmp634.KnownDatatype).val;
_tmp636=*_tmp635;_tmp637=_tmp633.targs;}};_tmp638=_tmp610.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp639=(struct Cyc_Absyn_DatatypeType_struct*)_tmp638;if(_tmp639->tag != 3)goto
_LL33C;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp63A.datatype_info;if((_tmp63B.KnownDatatype).tag
!= 2)goto _LL33C;_tmp63C=(struct Cyc_Absyn_Datatypedecl**)(_tmp63B.KnownDatatype).val;
_tmp63D=*_tmp63C;_tmp63E=_tmp63A.targs;}};_LL33B: if(_tmp63D == _tmp636)return 0;{
int _tmp6D2=Cyc_Absyn_qvar_cmp(_tmp63D->name,_tmp636->name);if(_tmp6D2 != 0)return
_tmp6D2;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp63E,_tmp637);};_LL33C: _tmp63F=
_tmp610.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp640=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp63F;if(_tmp640->tag != 4)goto _LL33E;else{_tmp641=_tmp640->f1;_tmp642=_tmp641.field_info;
if((_tmp642.KnownDatatypefield).tag != 2)goto _LL33E;_tmp643=(struct _tuple3)(
_tmp642.KnownDatatypefield).val;_tmp644=_tmp643.f1;_tmp645=_tmp643.f2;_tmp646=
_tmp641.targs;}};_tmp647=_tmp610.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp648=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp647;if(_tmp648->tag != 4)
goto _LL33E;else{_tmp649=_tmp648->f1;_tmp64A=_tmp649.field_info;if((_tmp64A.KnownDatatypefield).tag
!= 2)goto _LL33E;_tmp64B=(struct _tuple3)(_tmp64A.KnownDatatypefield).val;_tmp64C=
_tmp64B.f1;_tmp64D=_tmp64B.f2;_tmp64E=_tmp649.targs;}};_LL33D: if(_tmp64C == 
_tmp644)return 0;{int _tmp6D3=Cyc_Absyn_qvar_cmp(_tmp644->name,_tmp64C->name);if(
_tmp6D3 != 0)return _tmp6D3;{int _tmp6D4=Cyc_Absyn_qvar_cmp(_tmp645->name,_tmp64D->name);
if(_tmp6D4 != 0)return _tmp6D4;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp64E,_tmp646);};};
_LL33E: _tmp64F=_tmp610.f1;{struct Cyc_Absyn_PointerType_struct*_tmp650=(struct Cyc_Absyn_PointerType_struct*)
_tmp64F;if(_tmp650->tag != 5)goto _LL340;else{_tmp651=_tmp650->f1;_tmp652=_tmp651.elt_typ;
_tmp653=_tmp651.elt_tq;_tmp654=_tmp651.ptr_atts;_tmp655=_tmp654.rgn;_tmp656=
_tmp654.nullable;_tmp657=_tmp654.bounds;_tmp658=_tmp654.zero_term;}};_tmp659=
_tmp610.f2;{struct Cyc_Absyn_PointerType_struct*_tmp65A=(struct Cyc_Absyn_PointerType_struct*)
_tmp659;if(_tmp65A->tag != 5)goto _LL340;else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65B.elt_typ;
_tmp65D=_tmp65B.elt_tq;_tmp65E=_tmp65B.ptr_atts;_tmp65F=_tmp65E.rgn;_tmp660=
_tmp65E.nullable;_tmp661=_tmp65E.bounds;_tmp662=_tmp65E.zero_term;}};_LL33F: {
int _tmp6D5=Cyc_Tcutil_typecmp(_tmp65C,_tmp652);if(_tmp6D5 != 0)return _tmp6D5;{int
_tmp6D6=Cyc_Tcutil_typecmp(_tmp65F,_tmp655);if(_tmp6D6 != 0)return _tmp6D6;{int
_tmp6D7=Cyc_Tcutil_tqual_cmp(_tmp65D,_tmp653);if(_tmp6D7 != 0)return _tmp6D7;{int
_tmp6D8=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp661,_tmp657);if(_tmp6D8 != 0)
return _tmp6D8;{int _tmp6D9=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp662,
_tmp658);if(_tmp6D9 != 0)return _tmp6D9;{void*_tmp6DA=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp661);_LL363: {struct Cyc_Absyn_DynEither_b_struct*
_tmp6DB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp6DA;if(_tmp6DB->tag != 0)goto
_LL365;}_LL364: return 0;_LL365:;_LL366: goto _LL362;_LL362:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp660,_tmp656);};};};};}_LL340: _tmp663=_tmp610.f1;{struct Cyc_Absyn_IntType_struct*
_tmp664=(struct Cyc_Absyn_IntType_struct*)_tmp663;if(_tmp664->tag != 6)goto _LL342;
else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;}};_tmp667=_tmp610.f2;{struct Cyc_Absyn_IntType_struct*
_tmp668=(struct Cyc_Absyn_IntType_struct*)_tmp667;if(_tmp668->tag != 6)goto _LL342;
else{_tmp669=_tmp668->f1;_tmp66A=_tmp668->f2;}};_LL341: if(_tmp669 != _tmp665)
return Cyc_Core_intcmp((int)((unsigned int)_tmp669),(int)((unsigned int)_tmp665));
if(_tmp66A != _tmp666)return Cyc_Core_intcmp((int)((unsigned int)_tmp66A),(int)((
unsigned int)_tmp666));return 0;_LL342: _tmp66B=_tmp610.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp66C=(struct Cyc_Absyn_FloatType_struct*)_tmp66B;if(_tmp66C->tag != 7)goto
_LL344;else{_tmp66D=_tmp66C->f1;}};_tmp66E=_tmp610.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp66F=(struct Cyc_Absyn_FloatType_struct*)_tmp66E;if(_tmp66F->tag != 7)goto
_LL344;else{_tmp670=_tmp66F->f1;}};_LL343: if(_tmp66D == _tmp670)return 0;else{if(
_tmp670 == 0)return - 1;else{if(_tmp670 == 1  && _tmp66D == 0)return - 1;else{return 1;}}}
_LL344: _tmp671=_tmp610.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp672=(struct Cyc_Absyn_ArrayType_struct*)
_tmp671;if(_tmp672->tag != 8)goto _LL346;else{_tmp673=_tmp672->f1;_tmp674=_tmp673.elt_type;
_tmp675=_tmp673.tq;_tmp676=_tmp673.num_elts;_tmp677=_tmp673.zero_term;}};_tmp678=
_tmp610.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp679=(struct Cyc_Absyn_ArrayType_struct*)
_tmp678;if(_tmp679->tag != 8)goto _LL346;else{_tmp67A=_tmp679->f1;_tmp67B=_tmp67A.elt_type;
_tmp67C=_tmp67A.tq;_tmp67D=_tmp67A.num_elts;_tmp67E=_tmp67A.zero_term;}};_LL345: {
int _tmp6DC=Cyc_Tcutil_tqual_cmp(_tmp67C,_tmp675);if(_tmp6DC != 0)return _tmp6DC;{
int _tmp6DD=Cyc_Tcutil_typecmp(_tmp67B,_tmp674);if(_tmp6DD != 0)return _tmp6DD;{int
_tmp6DE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp677,_tmp67E);if(_tmp6DE != 0)
return _tmp6DE;if(_tmp676 == _tmp67D)return 0;if(_tmp676 == 0  || _tmp67D == 0){const
char*_tmp12B0;void*_tmp12AF;(_tmp12AF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B0="missing expression in array index",
_tag_dyneither(_tmp12B0,sizeof(char),34))),_tag_dyneither(_tmp12AF,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp676,_tmp67D);};};}_LL346: _tmp67F=_tmp610.f1;{struct Cyc_Absyn_FnType_struct*
_tmp680=(struct Cyc_Absyn_FnType_struct*)_tmp67F;if(_tmp680->tag != 9)goto _LL348;
else{_tmp681=_tmp680->f1;_tmp682=_tmp681.tvars;_tmp683=_tmp681.effect;_tmp684=
_tmp681.ret_tqual;_tmp685=_tmp681.ret_typ;_tmp686=_tmp681.args;_tmp687=_tmp681.c_varargs;
_tmp688=_tmp681.cyc_varargs;_tmp689=_tmp681.rgn_po;_tmp68A=_tmp681.attributes;}};
_tmp68B=_tmp610.f2;{struct Cyc_Absyn_FnType_struct*_tmp68C=(struct Cyc_Absyn_FnType_struct*)
_tmp68B;if(_tmp68C->tag != 9)goto _LL348;else{_tmp68D=_tmp68C->f1;_tmp68E=_tmp68D.tvars;
_tmp68F=_tmp68D.effect;_tmp690=_tmp68D.ret_tqual;_tmp691=_tmp68D.ret_typ;_tmp692=
_tmp68D.args;_tmp693=_tmp68D.c_varargs;_tmp694=_tmp68D.cyc_varargs;_tmp695=
_tmp68D.rgn_po;_tmp696=_tmp68D.attributes;}};_LL347: {const char*_tmp12B3;void*
_tmp12B2;(_tmp12B2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12B3="typecmp: function types not handled",
_tag_dyneither(_tmp12B3,sizeof(char),36))),_tag_dyneither(_tmp12B2,sizeof(void*),
0)));}_LL348: _tmp697=_tmp610.f1;{struct Cyc_Absyn_TupleType_struct*_tmp698=(
struct Cyc_Absyn_TupleType_struct*)_tmp697;if(_tmp698->tag != 10)goto _LL34A;else{
_tmp699=_tmp698->f1;}};_tmp69A=_tmp610.f2;{struct Cyc_Absyn_TupleType_struct*
_tmp69B=(struct Cyc_Absyn_TupleType_struct*)_tmp69A;if(_tmp69B->tag != 10)goto
_LL34A;else{_tmp69C=_tmp69B->f1;}};_LL349: return((int(*)(int(*cmp)(struct
_tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_tqual_type_cmp,_tmp69C,_tmp699);_LL34A: _tmp69D=_tmp610.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp69E=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp69D;if(_tmp69E->tag != 12)goto
_LL34C;else{_tmp69F=_tmp69E->f1;_tmp6A0=_tmp69E->f2;}};_tmp6A1=_tmp610.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp6A2=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6A1;if(_tmp6A2->tag != 12)goto _LL34C;else{_tmp6A3=_tmp6A2->f1;_tmp6A4=_tmp6A2->f2;}};
_LL34B: if(_tmp6A3 != _tmp69F){if(_tmp6A3 == Cyc_Absyn_StructA)return - 1;else{return
1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,
_tmp6A4,_tmp6A0);_LL34C: _tmp6A5=_tmp610.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A6=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A5;if(_tmp6A6->tag != 15)goto
_LL34E;else{_tmp6A7=(void*)_tmp6A6->f1;}};_tmp6A8=_tmp610.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp6A9=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A8;if(_tmp6A9->tag != 15)goto
_LL34E;else{_tmp6AA=(void*)_tmp6A9->f1;}};_LL34D: return Cyc_Tcutil_typecmp(
_tmp6A7,_tmp6AA);_LL34E: _tmp6AB=_tmp610.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp6AC=(struct Cyc_Absyn_DynRgnType_struct*)_tmp6AB;if(_tmp6AC->tag != 16)goto
_LL350;else{_tmp6AD=(void*)_tmp6AC->f1;_tmp6AE=(void*)_tmp6AC->f2;}};_tmp6AF=
_tmp610.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp6B0=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp6AF;if(_tmp6B0->tag != 16)goto _LL350;else{_tmp6B1=(void*)_tmp6B0->f1;_tmp6B2=(
void*)_tmp6B0->f2;}};_LL34F: {int _tmp6E3=Cyc_Tcutil_typecmp(_tmp6AD,_tmp6B1);if(
_tmp6E3 != 0)return _tmp6E3;else{return Cyc_Tcutil_typecmp(_tmp6AE,_tmp6B2);}}
_LL350: _tmp6B3=_tmp610.f1;{struct Cyc_Absyn_TagType_struct*_tmp6B4=(struct Cyc_Absyn_TagType_struct*)
_tmp6B3;if(_tmp6B4->tag != 19)goto _LL352;else{_tmp6B5=(void*)_tmp6B4->f1;}};
_tmp6B6=_tmp610.f2;{struct Cyc_Absyn_TagType_struct*_tmp6B7=(struct Cyc_Absyn_TagType_struct*)
_tmp6B6;if(_tmp6B7->tag != 19)goto _LL352;else{_tmp6B8=(void*)_tmp6B7->f1;}};
_LL351: return Cyc_Tcutil_typecmp(_tmp6B5,_tmp6B8);_LL352: _tmp6B9=_tmp610.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp6BA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6B9;if(_tmp6BA->tag != 18)goto _LL354;else{_tmp6BB=_tmp6BA->f1;}};_tmp6BC=
_tmp610.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp6BD=(struct Cyc_Absyn_ValueofType_struct*)
_tmp6BC;if(_tmp6BD->tag != 18)goto _LL354;else{_tmp6BE=_tmp6BD->f1;}};_LL353:
return Cyc_Evexp_const_exp_cmp(_tmp6BB,_tmp6BE);_LL354: _tmp6BF=_tmp610.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp6C0=(struct Cyc_Absyn_JoinEff_struct*)_tmp6BF;if(
_tmp6C0->tag != 24)goto _LL356;};_LL355: goto _LL357;_LL356: _tmp6C1=_tmp610.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp6C2=(struct Cyc_Absyn_JoinEff_struct*)_tmp6C1;
if(_tmp6C2->tag != 24)goto _LL358;};_LL357: goto _LL359;_LL358: _tmp6C3=_tmp610.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp6C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6C3;if(_tmp6C4->tag != 23)goto _LL35A;};_LL359: goto _LL35B;_LL35A: _tmp6C5=
_tmp610.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp6C6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6C5;if(_tmp6C6->tag != 25)goto _LL35C;};_LL35B: goto _LL35D;_LL35C: _tmp6C7=
_tmp610.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp6C8=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp6C7;if(_tmp6C8->tag != 25)goto _LL35E;};_LL35D: goto _LL35F;_LL35E: _tmp6C9=
_tmp610.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp6CA=(struct Cyc_Absyn_AccessEff_struct*)
_tmp6C9;if(_tmp6CA->tag != 23)goto _LL360;};_LL35F: {const char*_tmp12B6;void*
_tmp12B5;(_tmp12B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12B6="typecmp: effects not handled",_tag_dyneither(
_tmp12B6,sizeof(char),29))),_tag_dyneither(_tmp12B5,sizeof(void*),0)));}_LL360:;
_LL361: {const char*_tmp12B9;void*_tmp12B8;(_tmp12B8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B9="Unmatched case in typecmp",
_tag_dyneither(_tmp12B9,sizeof(char),26))),_tag_dyneither(_tmp12B8,sizeof(void*),
0)));}_LL32F:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6E8=Cyc_Tcutil_compress(
t);_LL368: {struct Cyc_Absyn_IntType_struct*_tmp6E9=(struct Cyc_Absyn_IntType_struct*)
_tmp6E8;if(_tmp6E9->tag != 6)goto _LL36A;}_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_FloatType_struct*
_tmp6EA=(struct Cyc_Absyn_FloatType_struct*)_tmp6E8;if(_tmp6EA->tag != 7)goto
_LL36C;}_LL36B: goto _LL36D;_LL36C: {struct Cyc_Absyn_EnumType_struct*_tmp6EB=(
struct Cyc_Absyn_EnumType_struct*)_tmp6E8;if(_tmp6EB->tag != 13)goto _LL36E;}_LL36D:
goto _LL36F;_LL36E: {struct Cyc_Absyn_AnonEnumType_struct*_tmp6EC=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp6E8;if(_tmp6EC->tag != 14)goto _LL370;}_LL36F: return 1;_LL370:;_LL371: return 0;
_LL367:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12BA;struct _tuple0 _tmp6EE=(
_tmp12BA.f1=t1,((_tmp12BA.f2=t2,_tmp12BA)));void*_tmp6EF;int _tmp6F1;void*_tmp6F2;
int _tmp6F4;void*_tmp6F5;void*_tmp6F7;void*_tmp6F9;void*_tmp6FB;void*_tmp6FD;
enum Cyc_Absyn_Size_of _tmp6FF;void*_tmp700;enum Cyc_Absyn_Size_of _tmp702;void*
_tmp703;enum Cyc_Absyn_Size_of _tmp705;void*_tmp706;enum Cyc_Absyn_Size_of _tmp708;
void*_tmp709;enum Cyc_Absyn_Size_of _tmp70B;void*_tmp70C;enum Cyc_Absyn_Size_of
_tmp70E;void*_tmp70F;enum Cyc_Absyn_Size_of _tmp711;void*_tmp712;enum Cyc_Absyn_Size_of
_tmp714;void*_tmp715;int _tmp717;void*_tmp718;enum Cyc_Absyn_Size_of _tmp71A;void*
_tmp71B;int _tmp71D;void*_tmp71E;enum Cyc_Absyn_Size_of _tmp720;void*_tmp721;enum 
Cyc_Absyn_Size_of _tmp723;void*_tmp724;enum Cyc_Absyn_Size_of _tmp726;void*_tmp727;
enum Cyc_Absyn_Size_of _tmp729;void*_tmp72A;void*_tmp72C;enum Cyc_Absyn_Size_of
_tmp72E;void*_tmp72F;enum Cyc_Absyn_Size_of _tmp731;void*_tmp732;enum Cyc_Absyn_Size_of
_tmp734;void*_tmp735;enum Cyc_Absyn_Size_of _tmp737;void*_tmp738;enum Cyc_Absyn_Size_of
_tmp73A;void*_tmp73B;enum Cyc_Absyn_Size_of _tmp73D;void*_tmp73E;enum Cyc_Absyn_Size_of
_tmp740;void*_tmp741;void*_tmp743;enum Cyc_Absyn_Size_of _tmp745;_LL373: _tmp6EF=
_tmp6EE.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6F0=(struct Cyc_Absyn_FloatType_struct*)
_tmp6EF;if(_tmp6F0->tag != 7)goto _LL375;else{_tmp6F1=_tmp6F0->f1;}};_tmp6F2=
_tmp6EE.f2;{struct Cyc_Absyn_FloatType_struct*_tmp6F3=(struct Cyc_Absyn_FloatType_struct*)
_tmp6F2;if(_tmp6F3->tag != 7)goto _LL375;else{_tmp6F4=_tmp6F3->f1;}};_LL374: return
_tmp6F4 == 0  && _tmp6F1 != 0  || (_tmp6F4 == 1  && _tmp6F1 != 0) && _tmp6F1 != 1;_LL375:
_tmp6F5=_tmp6EE.f1;{struct Cyc_Absyn_FloatType_struct*_tmp6F6=(struct Cyc_Absyn_FloatType_struct*)
_tmp6F5;if(_tmp6F6->tag != 7)goto _LL377;};_tmp6F7=_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*
_tmp6F8=(struct Cyc_Absyn_IntType_struct*)_tmp6F7;if(_tmp6F8->tag != 6)goto _LL377;};
_LL376: goto _LL378;_LL377: _tmp6F9=_tmp6EE.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp6FA=(struct Cyc_Absyn_FloatType_struct*)_tmp6F9;if(_tmp6FA->tag != 7)goto
_LL379;};_tmp6FB=_tmp6EE.f2;{struct Cyc_Absyn_TagType_struct*_tmp6FC=(struct Cyc_Absyn_TagType_struct*)
_tmp6FB;if(_tmp6FC->tag != 19)goto _LL379;};_LL378: return 1;_LL379: _tmp6FD=_tmp6EE.f1;{
struct Cyc_Absyn_IntType_struct*_tmp6FE=(struct Cyc_Absyn_IntType_struct*)_tmp6FD;
if(_tmp6FE->tag != 6)goto _LL37B;else{_tmp6FF=_tmp6FE->f2;if(_tmp6FF != Cyc_Absyn_LongLong_sz)
goto _LL37B;}};_tmp700=_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp701=(struct
Cyc_Absyn_IntType_struct*)_tmp700;if(_tmp701->tag != 6)goto _LL37B;else{_tmp702=
_tmp701->f2;if(_tmp702 != Cyc_Absyn_LongLong_sz)goto _LL37B;}};_LL37A: return 0;
_LL37B: _tmp703=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*_tmp704=(struct Cyc_Absyn_IntType_struct*)
_tmp703;if(_tmp704->tag != 6)goto _LL37D;else{_tmp705=_tmp704->f2;if(_tmp705 != Cyc_Absyn_LongLong_sz)
goto _LL37D;}};_LL37C: return 1;_LL37D: _tmp706=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp707=(struct Cyc_Absyn_IntType_struct*)_tmp706;if(_tmp707->tag != 6)goto _LL37F;
else{_tmp708=_tmp707->f2;if(_tmp708 != Cyc_Absyn_Long_sz)goto _LL37F;}};_tmp709=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp70A=(struct Cyc_Absyn_IntType_struct*)
_tmp709;if(_tmp70A->tag != 6)goto _LL37F;else{_tmp70B=_tmp70A->f2;if(_tmp70B != Cyc_Absyn_Int_sz)
goto _LL37F;}};_LL37E: goto _LL380;_LL37F: _tmp70C=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp70D=(struct Cyc_Absyn_IntType_struct*)_tmp70C;if(_tmp70D->tag != 6)goto _LL381;
else{_tmp70E=_tmp70D->f2;if(_tmp70E != Cyc_Absyn_Int_sz)goto _LL381;}};_tmp70F=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp710=(struct Cyc_Absyn_IntType_struct*)
_tmp70F;if(_tmp710->tag != 6)goto _LL381;else{_tmp711=_tmp710->f2;if(_tmp711 != Cyc_Absyn_Long_sz)
goto _LL381;}};_LL380: return 0;_LL381: _tmp712=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp713=(struct Cyc_Absyn_IntType_struct*)_tmp712;if(_tmp713->tag != 6)goto _LL383;
else{_tmp714=_tmp713->f2;if(_tmp714 != Cyc_Absyn_Long_sz)goto _LL383;}};_tmp715=
_tmp6EE.f2;{struct Cyc_Absyn_FloatType_struct*_tmp716=(struct Cyc_Absyn_FloatType_struct*)
_tmp715;if(_tmp716->tag != 7)goto _LL383;else{_tmp717=_tmp716->f1;if(_tmp717 != 0)
goto _LL383;}};_LL382: goto _LL384;_LL383: _tmp718=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp719=(struct Cyc_Absyn_IntType_struct*)_tmp718;if(_tmp719->tag != 6)goto _LL385;
else{_tmp71A=_tmp719->f2;if(_tmp71A != Cyc_Absyn_Int_sz)goto _LL385;}};_tmp71B=
_tmp6EE.f2;{struct Cyc_Absyn_FloatType_struct*_tmp71C=(struct Cyc_Absyn_FloatType_struct*)
_tmp71B;if(_tmp71C->tag != 7)goto _LL385;else{_tmp71D=_tmp71C->f1;if(_tmp71D != 0)
goto _LL385;}};_LL384: goto _LL386;_LL385: _tmp71E=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp71F=(struct Cyc_Absyn_IntType_struct*)_tmp71E;if(_tmp71F->tag != 6)goto _LL387;
else{_tmp720=_tmp71F->f2;if(_tmp720 != Cyc_Absyn_Long_sz)goto _LL387;}};_tmp721=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp722=(struct Cyc_Absyn_IntType_struct*)
_tmp721;if(_tmp722->tag != 6)goto _LL387;else{_tmp723=_tmp722->f2;if(_tmp723 != Cyc_Absyn_Short_sz)
goto _LL387;}};_LL386: goto _LL388;_LL387: _tmp724=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp725=(struct Cyc_Absyn_IntType_struct*)_tmp724;if(_tmp725->tag != 6)goto _LL389;
else{_tmp726=_tmp725->f2;if(_tmp726 != Cyc_Absyn_Int_sz)goto _LL389;}};_tmp727=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp728=(struct Cyc_Absyn_IntType_struct*)
_tmp727;if(_tmp728->tag != 6)goto _LL389;else{_tmp729=_tmp728->f2;if(_tmp729 != Cyc_Absyn_Short_sz)
goto _LL389;}};_LL388: goto _LL38A;_LL389: _tmp72A=_tmp6EE.f1;{struct Cyc_Absyn_TagType_struct*
_tmp72B=(struct Cyc_Absyn_TagType_struct*)_tmp72A;if(_tmp72B->tag != 19)goto _LL38B;};
_tmp72C=_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp72D=(struct Cyc_Absyn_IntType_struct*)
_tmp72C;if(_tmp72D->tag != 6)goto _LL38B;else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Short_sz)
goto _LL38B;}};_LL38A: goto _LL38C;_LL38B: _tmp72F=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp730=(struct Cyc_Absyn_IntType_struct*)_tmp72F;if(_tmp730->tag != 6)goto _LL38D;
else{_tmp731=_tmp730->f2;if(_tmp731 != Cyc_Absyn_Long_sz)goto _LL38D;}};_tmp732=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp733=(struct Cyc_Absyn_IntType_struct*)
_tmp732;if(_tmp733->tag != 6)goto _LL38D;else{_tmp734=_tmp733->f2;if(_tmp734 != Cyc_Absyn_Char_sz)
goto _LL38D;}};_LL38C: goto _LL38E;_LL38D: _tmp735=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp736=(struct Cyc_Absyn_IntType_struct*)_tmp735;if(_tmp736->tag != 6)goto _LL38F;
else{_tmp737=_tmp736->f2;if(_tmp737 != Cyc_Absyn_Int_sz)goto _LL38F;}};_tmp738=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp739=(struct Cyc_Absyn_IntType_struct*)
_tmp738;if(_tmp739->tag != 6)goto _LL38F;else{_tmp73A=_tmp739->f2;if(_tmp73A != Cyc_Absyn_Char_sz)
goto _LL38F;}};_LL38E: goto _LL390;_LL38F: _tmp73B=_tmp6EE.f1;{struct Cyc_Absyn_IntType_struct*
_tmp73C=(struct Cyc_Absyn_IntType_struct*)_tmp73B;if(_tmp73C->tag != 6)goto _LL391;
else{_tmp73D=_tmp73C->f2;if(_tmp73D != Cyc_Absyn_Short_sz)goto _LL391;}};_tmp73E=
_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp73F=(struct Cyc_Absyn_IntType_struct*)
_tmp73E;if(_tmp73F->tag != 6)goto _LL391;else{_tmp740=_tmp73F->f2;if(_tmp740 != Cyc_Absyn_Char_sz)
goto _LL391;}};_LL390: goto _LL392;_LL391: _tmp741=_tmp6EE.f1;{struct Cyc_Absyn_TagType_struct*
_tmp742=(struct Cyc_Absyn_TagType_struct*)_tmp741;if(_tmp742->tag != 19)goto _LL393;};
_tmp743=_tmp6EE.f2;{struct Cyc_Absyn_IntType_struct*_tmp744=(struct Cyc_Absyn_IntType_struct*)
_tmp743;if(_tmp744->tag != 6)goto _LL393;else{_tmp745=_tmp744->f2;if(_tmp745 != Cyc_Absyn_Char_sz)
goto _LL393;}};_LL392: return 1;_LL393:;_LL394: return 0;_LL372:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp746=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp12BB;max_arith_type=((_tmp12BB=
_region_malloc(_tmp746,sizeof(*_tmp12BB)),((_tmp12BB->v=t1,_tmp12BB))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp12C0;void*_tmp12BF[2];struct
Cyc_String_pa_struct _tmp12BE;struct Cyc_String_pa_struct _tmp12BD;(_tmp12BD.tag=0,((
_tmp12BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp12BE.tag=0,((_tmp12BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12BF[0]=& _tmp12BE,((_tmp12BF[1]=& _tmp12BD,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12C0="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12C0,sizeof(char),41))),_tag_dyneither(_tmp12BF,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp74C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL396: {struct Cyc_Absyn_PointerType_struct*
_tmp74D=(struct Cyc_Absyn_PointerType_struct*)_tmp74C;if(_tmp74D->tag != 5)goto
_LL398;}_LL397: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL395;_LL398:;_LL399: return 0;_LL395:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp74E=Cyc_Tcutil_compress(t);_LL39B: {struct Cyc_Absyn_IntType_struct*
_tmp74F=(struct Cyc_Absyn_IntType_struct*)_tmp74E;if(_tmp74F->tag != 6)goto _LL39D;}
_LL39C: goto _LL39E;_LL39D: {struct Cyc_Absyn_TagType_struct*_tmp750=(struct Cyc_Absyn_TagType_struct*)
_tmp74E;if(_tmp750->tag != 19)goto _LL39F;}_LL39E: goto _LL3A0;_LL39F: {struct Cyc_Absyn_EnumType_struct*
_tmp751=(struct Cyc_Absyn_EnumType_struct*)_tmp74E;if(_tmp751->tag != 13)goto
_LL3A1;}_LL3A0: goto _LL3A2;_LL3A1: {struct Cyc_Absyn_AnonEnumType_struct*_tmp752=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp74E;if(_tmp752->tag != 14)goto _LL3A3;}
_LL3A2: return 1;_LL3A3:;_LL3A4: return 0;_LL39A:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp12C3;void*_tmp12C2;(_tmp12C2=0,Cyc_Tcutil_warn(e->loc,((_tmp12C3="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12C3,sizeof(char),44))),_tag_dyneither(_tmp12C2,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12C6;void*_tmp12C5;(
_tmp12C5=0,Cyc_Tcutil_warn(e->loc,((_tmp12C6="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12C6,sizeof(char),44))),_tag_dyneither(_tmp12C5,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12C7;struct _tuple0 _tmp758=(_tmp12C7.f1=
t1,((_tmp12C7.f2=t2,_tmp12C7)));void*_tmp759;struct Cyc_Absyn_PtrInfo _tmp75B;void*
_tmp75C;struct Cyc_Absyn_PtrInfo _tmp75E;void*_tmp75F;struct Cyc_Absyn_ArrayInfo
_tmp761;void*_tmp762;struct Cyc_Absyn_Tqual _tmp763;struct Cyc_Absyn_Exp*_tmp764;
union Cyc_Absyn_Constraint*_tmp765;void*_tmp766;struct Cyc_Absyn_ArrayInfo _tmp768;
void*_tmp769;struct Cyc_Absyn_Tqual _tmp76A;struct Cyc_Absyn_Exp*_tmp76B;union Cyc_Absyn_Constraint*
_tmp76C;void*_tmp76D;void*_tmp76F;_LL3A6: _tmp759=_tmp758.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp75A=(struct Cyc_Absyn_PointerType_struct*)_tmp759;if(_tmp75A->tag != 5)goto
_LL3A8;else{_tmp75B=_tmp75A->f1;}};_tmp75C=_tmp758.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp75D=(struct Cyc_Absyn_PointerType_struct*)_tmp75C;if(_tmp75D->tag != 5)goto
_LL3A8;else{_tmp75E=_tmp75D->f1;}};_LL3A7: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp75B.ptr_atts).nullable,(_tmp75E.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp75B.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp75B.ptr_atts).bounds,(
_tmp75E.ptr_atts).bounds)){struct _tuple0 _tmp12C8;struct _tuple0 _tmp772=(_tmp12C8.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp75B.ptr_atts).bounds),((_tmp12C8.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp75E.ptr_atts).bounds),
_tmp12C8)));void*_tmp773;void*_tmp775;void*_tmp777;void*_tmp779;void*_tmp77B;
struct Cyc_Absyn_Exp*_tmp77D;void*_tmp77E;struct Cyc_Absyn_Exp*_tmp780;void*
_tmp781;void*_tmp783;struct Cyc_Absyn_Exp*_tmp785;_LL3AF: _tmp773=_tmp772.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp774=(struct Cyc_Absyn_Upper_b_struct*)_tmp773;
if(_tmp774->tag != 1)goto _LL3B1;};_tmp775=_tmp772.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp776=(struct Cyc_Absyn_DynEither_b_struct*)_tmp775;if(_tmp776->tag != 0)goto
_LL3B1;};_LL3B0: goto _LL3B2;_LL3B1: _tmp777=_tmp772.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp778=(struct Cyc_Absyn_DynEither_b_struct*)_tmp777;if(_tmp778->tag != 0)goto
_LL3B3;};_tmp779=_tmp772.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp77A=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp779;if(_tmp77A->tag != 0)goto _LL3B3;};_LL3B2:
okay=1;goto _LL3AE;_LL3B3: _tmp77B=_tmp772.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp77C=(struct Cyc_Absyn_Upper_b_struct*)_tmp77B;if(_tmp77C->tag != 1)goto _LL3B5;
else{_tmp77D=_tmp77C->f1;}};_tmp77E=_tmp772.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp77F=(struct Cyc_Absyn_Upper_b_struct*)_tmp77E;if(_tmp77F->tag != 1)goto _LL3B5;
else{_tmp780=_tmp77F->f1;}};_LL3B4: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp780,
_tmp77D);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp75E.ptr_atts).zero_term)){const char*_tmp12CB;void*_tmp12CA;(_tmp12CA=0,
Cyc_Tcutil_warn(loc,((_tmp12CB="implicit cast to shorter array",_tag_dyneither(
_tmp12CB,sizeof(char),31))),_tag_dyneither(_tmp12CA,sizeof(void*),0)));}goto
_LL3AE;_LL3B5: _tmp781=_tmp772.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp782=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp781;if(_tmp782->tag != 0)goto _LL3AE;};
_tmp783=_tmp772.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp784=(struct Cyc_Absyn_Upper_b_struct*)
_tmp783;if(_tmp784->tag != 1)goto _LL3AE;else{_tmp785=_tmp784->f1;}};_LL3B6: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp75B.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp75E.ptr_atts).bounds))goto _LL3AE;okay=0;goto
_LL3AE;_LL3AE:;}okay=okay  && (!(_tmp75B.elt_tq).real_const  || (_tmp75E.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp75B.ptr_atts).rgn,(_tmp75E.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp75B.ptr_atts).rgn,(_tmp75E.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp75B.ptr_atts).zero_term,(
_tmp75E.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp75B.ptr_atts).zero_term) && (_tmp75E.elt_tq).real_const);{int _tmp788=((
int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp75E.ptr_atts).bounds,
Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp75E.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp75B.elt_typ,
_tmp75E.elt_typ) || (_tmp788  && ((_tmp75E.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp75E.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp75B.elt_typ,_tmp75E.elt_typ));return okay;};}_LL3A8: _tmp75F=_tmp758.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp760=(struct Cyc_Absyn_ArrayType_struct*)
_tmp75F;if(_tmp760->tag != 8)goto _LL3AA;else{_tmp761=_tmp760->f1;_tmp762=_tmp761.elt_type;
_tmp763=_tmp761.tq;_tmp764=_tmp761.num_elts;_tmp765=_tmp761.zero_term;}};_tmp766=
_tmp758.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp767=(struct Cyc_Absyn_ArrayType_struct*)
_tmp766;if(_tmp767->tag != 8)goto _LL3AA;else{_tmp768=_tmp767->f1;_tmp769=_tmp768.elt_type;
_tmp76A=_tmp768.tq;_tmp76B=_tmp768.num_elts;_tmp76C=_tmp768.zero_term;}};_LL3A9: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp765,_tmp76C) && ((_tmp764 != 0
 && _tmp76B != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp764,(
struct Cyc_Absyn_Exp*)_tmp76B));return(okay  && Cyc_Tcutil_unify(_tmp762,_tmp769))
 && (!_tmp763.real_const  || _tmp76A.real_const);}_LL3AA: _tmp76D=_tmp758.f1;{
struct Cyc_Absyn_TagType_struct*_tmp76E=(struct Cyc_Absyn_TagType_struct*)_tmp76D;
if(_tmp76E->tag != 19)goto _LL3AC;};_tmp76F=_tmp758.f2;{struct Cyc_Absyn_IntType_struct*
_tmp770=(struct Cyc_Absyn_IntType_struct*)_tmp76F;if(_tmp770->tag != 6)goto _LL3AC;};
_LL3AB: return 0;_LL3AC:;_LL3AD: return Cyc_Tcutil_unify(t1,t2);_LL3A5:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp789=Cyc_Tcutil_compress(t);_LL3B8: {struct Cyc_Absyn_PointerType_struct*
_tmp78A=(struct Cyc_Absyn_PointerType_struct*)_tmp789;if(_tmp78A->tag != 5)goto
_LL3BA;}_LL3B9: return 1;_LL3BA:;_LL3BB: return 0;_LL3B7:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp78B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp78D;void*
_tmp78E;_LL3BD: {struct Cyc_Absyn_PointerType_struct*_tmp78C=(struct Cyc_Absyn_PointerType_struct*)
_tmp78B;if(_tmp78C->tag != 5)goto _LL3BF;else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78D.elt_typ;}}
_LL3BE: return _tmp78E;_LL3BF:;_LL3C0: {const char*_tmp12CE;void*_tmp12CD;(_tmp12CD=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12CE="pointer_elt_type",_tag_dyneither(_tmp12CE,sizeof(char),17))),
_tag_dyneither(_tmp12CD,sizeof(void*),0)));}_LL3BC:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp791=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp793;
struct Cyc_Absyn_PtrAtts _tmp794;struct Cyc_Absyn_PtrAtts*_tmp795;_LL3C2: {struct
Cyc_Absyn_PointerType_struct*_tmp792=(struct Cyc_Absyn_PointerType_struct*)
_tmp791;if(_tmp792->tag != 5)goto _LL3C4;else{_tmp793=_tmp792->f1;_tmp794=_tmp793.ptr_atts;
_tmp795=(struct Cyc_Absyn_PtrAtts*)&(_tmp792->f1).ptr_atts;}}_LL3C3: return _tmp795->rgn;
_LL3C4:;_LL3C5: {const char*_tmp12D1;void*_tmp12D0;(_tmp12D0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D1="pointer_elt_type",
_tag_dyneither(_tmp12D1,sizeof(char),17))),_tag_dyneither(_tmp12D0,sizeof(void*),
0)));}_LL3C1:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp798=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp79A;struct Cyc_Absyn_PtrAtts _tmp79B;void*_tmp79C;
_LL3C7: {struct Cyc_Absyn_PointerType_struct*_tmp799=(struct Cyc_Absyn_PointerType_struct*)
_tmp798;if(_tmp799->tag != 5)goto _LL3C9;else{_tmp79A=_tmp799->f1;_tmp79B=_tmp79A.ptr_atts;
_tmp79C=_tmp79B.rgn;}}_LL3C8:*rgn=_tmp79C;return 1;_LL3C9:;_LL3CA: return 0;_LL3C6:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp79D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp79F;struct Cyc_Absyn_PtrAtts _tmp7A0;union Cyc_Absyn_Constraint*
_tmp7A1;_LL3CC: {struct Cyc_Absyn_PointerType_struct*_tmp79E=(struct Cyc_Absyn_PointerType_struct*)
_tmp79D;if(_tmp79E->tag != 5)goto _LL3CE;else{_tmp79F=_tmp79E->f1;_tmp7A0=_tmp79F.ptr_atts;
_tmp7A1=_tmp7A0.bounds;}}_LL3CD:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7A1)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3CE:;_LL3CF: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3CB:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp7A2=e->r;union Cyc_Absyn_Cnst
_tmp7A4;struct _tuple7 _tmp7A5;int _tmp7A6;union Cyc_Absyn_Cnst _tmp7A8;struct _tuple5
_tmp7A9;char _tmp7AA;union Cyc_Absyn_Cnst _tmp7AC;struct _tuple6 _tmp7AD;short _tmp7AE;
union Cyc_Absyn_Cnst _tmp7B0;struct _tuple8 _tmp7B1;long long _tmp7B2;union Cyc_Absyn_Cnst
_tmp7B4;struct _dyneither_ptr _tmp7B5;void*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;
_LL3D1: {struct Cyc_Absyn_Const_e_struct*_tmp7A3=(struct Cyc_Absyn_Const_e_struct*)
_tmp7A2;if(_tmp7A3->tag != 0)goto _LL3D3;else{_tmp7A4=_tmp7A3->f1;if((_tmp7A4.Int_c).tag
!= 5)goto _LL3D3;_tmp7A5=(struct _tuple7)(_tmp7A4.Int_c).val;_tmp7A6=_tmp7A5.f2;
if(_tmp7A6 != 0)goto _LL3D3;}}_LL3D2: goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Const_e_struct*
_tmp7A7=(struct Cyc_Absyn_Const_e_struct*)_tmp7A2;if(_tmp7A7->tag != 0)goto _LL3D5;
else{_tmp7A8=_tmp7A7->f1;if((_tmp7A8.Char_c).tag != 2)goto _LL3D5;_tmp7A9=(struct
_tuple5)(_tmp7A8.Char_c).val;_tmp7AA=_tmp7A9.f2;if(_tmp7AA != 0)goto _LL3D5;}}
_LL3D4: goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Const_e_struct*_tmp7AB=(struct Cyc_Absyn_Const_e_struct*)
_tmp7A2;if(_tmp7AB->tag != 0)goto _LL3D7;else{_tmp7AC=_tmp7AB->f1;if((_tmp7AC.Short_c).tag
!= 4)goto _LL3D7;_tmp7AD=(struct _tuple6)(_tmp7AC.Short_c).val;_tmp7AE=_tmp7AD.f2;
if(_tmp7AE != 0)goto _LL3D7;}}_LL3D6: goto _LL3D8;_LL3D7: {struct Cyc_Absyn_Const_e_struct*
_tmp7AF=(struct Cyc_Absyn_Const_e_struct*)_tmp7A2;if(_tmp7AF->tag != 0)goto _LL3D9;
else{_tmp7B0=_tmp7AF->f1;if((_tmp7B0.LongLong_c).tag != 6)goto _LL3D9;_tmp7B1=(
struct _tuple8)(_tmp7B0.LongLong_c).val;_tmp7B2=_tmp7B1.f2;if(_tmp7B2 != 0)goto
_LL3D9;}}_LL3D8: return 1;_LL3D9: {struct Cyc_Absyn_Const_e_struct*_tmp7B3=(struct
Cyc_Absyn_Const_e_struct*)_tmp7A2;if(_tmp7B3->tag != 0)goto _LL3DB;else{_tmp7B4=
_tmp7B3->f1;if((_tmp7B4.Wchar_c).tag != 3)goto _LL3DB;_tmp7B5=(struct
_dyneither_ptr)(_tmp7B4.Wchar_c).val;}}_LL3DA: {unsigned long _tmp7B9=Cyc_strlen((
struct _dyneither_ptr)_tmp7B5);int i=0;if(_tmp7B9 >= 2  && *((const char*)
_check_dyneither_subscript(_tmp7B5,sizeof(char),0))== '\\'){if(*((const char*)
_check_dyneither_subscript(_tmp7B5,sizeof(char),1))== '0')i=2;else{if((*((const
char*)_check_dyneither_subscript(_tmp7B5,sizeof(char),1))== 'x'  && _tmp7B9 >= 3)
 && *((const char*)_check_dyneither_subscript(_tmp7B5,sizeof(char),2))== '0')i=3;
else{return 0;}}for(0;i < _tmp7B9;++ i){if(*((const char*)_check_dyneither_subscript(
_tmp7B5,sizeof(char),i))!= '0')return 0;}return 1;}else{return 0;}}_LL3DB: {struct
Cyc_Absyn_Cast_e_struct*_tmp7B6=(struct Cyc_Absyn_Cast_e_struct*)_tmp7A2;if(
_tmp7B6->tag != 15)goto _LL3DD;else{_tmp7B7=(void*)_tmp7B6->f1;_tmp7B8=_tmp7B6->f2;}}
_LL3DC: return Cyc_Tcutil_is_zero(_tmp7B8) && Cyc_Tcutil_admits_zero(_tmp7B7);
_LL3DD:;_LL3DE: return 0;_LL3D0:;}struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,
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
ka){struct Cyc_Absyn_Kind _tmp7BB;enum Cyc_Absyn_KindQual _tmp7BC;enum Cyc_Absyn_AliasQual
_tmp7BD;struct Cyc_Absyn_Kind*_tmp7BA=ka;_tmp7BB=*_tmp7BA;_tmp7BC=_tmp7BB.kind;
_tmp7BD=_tmp7BB.aliasqual;switch(_tmp7BD){case Cyc_Absyn_Aliasable: _LL3DF: switch(
_tmp7BC){case Cyc_Absyn_AnyKind: _LL3E1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;
case Cyc_Absyn_MemKind: _LL3E2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind:
_LL3E3: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3E4:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3E5: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3E6: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3E0: switch(_tmp7BC){case
Cyc_Absyn_AnyKind: _LL3E9: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind:
_LL3EA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3EB:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3EC: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;default: _LL3ED: break;}break;case Cyc_Absyn_Top:
_LL3E8: switch(_tmp7BC){case Cyc_Absyn_AnyKind: _LL3F0: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3F1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;
case Cyc_Absyn_BoxKind: _LL3F2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind:
_LL3F3: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3F4: break;}break;}{
const char*_tmp12D5;void*_tmp12D4[1];struct Cyc_String_pa_struct _tmp12D3;(_tmp12D3.tag=
0,((_tmp12D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12D4[0]=& _tmp12D3,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D5="kind_to_opt: bad kind %s\n",
_tag_dyneither(_tmp12D5,sizeof(char),26))),_tag_dyneither(_tmp12D4,sizeof(void*),
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
ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind _tmp7C2;enum Cyc_Absyn_KindQual
_tmp7C3;enum Cyc_Absyn_AliasQual _tmp7C4;struct Cyc_Absyn_Kind*_tmp7C1=ka;_tmp7C2=*
_tmp7C1;_tmp7C3=_tmp7C2.kind;_tmp7C4=_tmp7C2.aliasqual;switch(_tmp7C4){case Cyc_Absyn_Aliasable:
_LL3F6: switch(_tmp7C3){case Cyc_Absyn_AnyKind: _LL3F8: return& ab;case Cyc_Absyn_MemKind:
_LL3F9: return& mb;case Cyc_Absyn_BoxKind: _LL3FA: return& bb;case Cyc_Absyn_RgnKind:
_LL3FB: return& rb;case Cyc_Absyn_EffKind: _LL3FC: return& eb;case Cyc_Absyn_IntKind:
_LL3FD: return& ib;}case Cyc_Absyn_Unique: _LL3F7: switch(_tmp7C3){case Cyc_Absyn_AnyKind:
_LL400: return& uab;case Cyc_Absyn_MemKind: _LL401: return& umb;case Cyc_Absyn_BoxKind:
_LL402: return& ubb;case Cyc_Absyn_RgnKind: _LL403: return& urb;default: _LL404: break;}
break;case Cyc_Absyn_Top: _LL3FF: switch(_tmp7C3){case Cyc_Absyn_AnyKind: _LL407:
return& tab;case Cyc_Absyn_MemKind: _LL408: return& tmb;case Cyc_Absyn_BoxKind: _LL409:
return& tbb;case Cyc_Absyn_RgnKind: _LL40A: return& trb;default: _LL40B: break;}break;}{
const char*_tmp12D9;void*_tmp12D8[1];struct Cyc_String_pa_struct _tmp12D7;(_tmp12D7.tag=
0,((_tmp12D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12D8[0]=& _tmp12D7,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D9="kind_to_b: bad kind %s\n",
_tag_dyneither(_tmp12D9,sizeof(char),24))),_tag_dyneither(_tmp12D8,sizeof(void*),
1)))))));};}void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
struct Cyc_Core_Opt*_tmp12DA;return(_tmp12DA=_cycalloc(sizeof(*_tmp12DA)),((
_tmp12DA->v=Cyc_Tcutil_kind_to_bound(k),_tmp12DA)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12DD;struct Cyc_Absyn_Const_e_struct*
_tmp12DC;e1->r=(void*)((_tmp12DC=_cycalloc(sizeof(*_tmp12DC)),((_tmp12DC[0]=((
_tmp12DD.tag=0,((_tmp12DD.f1=Cyc_Absyn_Null_c,_tmp12DD)))),_tmp12DC))));}{struct
Cyc_Core_Opt*_tmp7D9=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12E7;struct Cyc_Absyn_PtrAtts _tmp12E6;struct Cyc_Absyn_PtrInfo _tmp12E5;struct
Cyc_Absyn_PointerType_struct*_tmp12E4;struct Cyc_Absyn_PointerType_struct*_tmp7DA=(
_tmp12E4=_cycalloc(sizeof(*_tmp12E4)),((_tmp12E4[0]=((_tmp12E7.tag=5,((_tmp12E7.f1=((
_tmp12E5.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7D9),((
_tmp12E5.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12E5.ptr_atts=((_tmp12E6.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7D9),((_tmp12E6.nullable=Cyc_Absyn_true_conref,((
_tmp12E6.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12E6.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12E6.ptrloc=0,_tmp12E6)))))))))),_tmp12E5)))))),_tmp12E7)))),_tmp12E4)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7DA);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp12EC;void*_tmp12EB[2];struct Cyc_String_pa_struct _tmp12EA;struct Cyc_String_pa_struct
_tmp12E9;(_tmp12E9.tag=0,((_tmp12E9.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12EA.tag=0,((
_tmp12EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12EB[0]=& _tmp12EA,((_tmp12EB[1]=& _tmp12E9,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EC="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12EC,sizeof(char),56))),_tag_dyneither(_tmp12EB,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL40D: {
const char*_tmp12ED;return(_tmp12ED="unknown",_tag_dyneither(_tmp12ED,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL40E: {const char*_tmp12EE;return(_tmp12EE="no coercion",
_tag_dyneither(_tmp12EE,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL40F: {
const char*_tmp12EF;return(_tmp12EF="null check",_tag_dyneither(_tmp12EF,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL410: {const char*_tmp12F0;return(
_tmp12F0="other coercion",_tag_dyneither(_tmp12F0,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp12FD;const char*_tmp12FC;void*_tmp12FB[1];struct Cyc_Int_pa_struct
_tmp12FA;struct _tuple2*_tmp12F9;struct _tuple2*v=(_tmp12F9=_cycalloc(sizeof(*
_tmp12F9)),((_tmp12F9->f1=Cyc_Absyn_Loc_n,((_tmp12F9->f2=((_tmp12FD=_cycalloc(
sizeof(*_tmp12FD)),((_tmp12FD[0]=(struct _dyneither_ptr)((_tmp12FA.tag=1,((
_tmp12FA.f1=(unsigned long)counter ++,((_tmp12FB[0]=& _tmp12FA,Cyc_aprintf(((
_tmp12FC="__aliasvar%d",_tag_dyneither(_tmp12FC,sizeof(char),13))),
_tag_dyneither(_tmp12FB,sizeof(void*),1)))))))),_tmp12FD)))),_tmp12F9)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp1300;struct Cyc_Absyn_Local_b_struct*
_tmp12FF;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp12FF=_cycalloc(
sizeof(*_tmp12FF)),((_tmp12FF[0]=((_tmp1300.tag=4,((_tmp1300.f1=vd,_tmp1300)))),
_tmp12FF)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7E7=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7E9;struct Cyc_Absyn_PtrAtts
_tmp7EA;void*_tmp7EB;void**_tmp7EC;_LL413: {struct Cyc_Absyn_PointerType_struct*
_tmp7E8=(struct Cyc_Absyn_PointerType_struct*)_tmp7E7;if(_tmp7E8->tag != 5)goto
_LL415;else{_tmp7E9=_tmp7E8->f1;_tmp7EA=_tmp7E9.ptr_atts;_tmp7EB=_tmp7EA.rgn;
_tmp7EC=(void**)&((_tmp7E8->f1).ptr_atts).rgn;}}_LL414:{struct Cyc_Absyn_VarType_struct
_tmp1303;struct Cyc_Absyn_VarType_struct*_tmp1302;*_tmp7EC=(void*)((_tmp1302=
_cycalloc(sizeof(*_tmp1302)),((_tmp1302[0]=((_tmp1303.tag=2,((_tmp1303.f1=tv,
_tmp1303)))),_tmp1302))));}goto _LL412;_LL415:;_LL416: goto _LL412;_LL412:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple15 _tmp1304;return(_tmp1304.f1=
d,((_tmp1304.f2=ve,_tmp1304)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,struct Cyc_Position_Segment*loc){struct _RegionHandle
_tmp7F8=_new_region("r");struct _RegionHandle*r=& _tmp7F8;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp7F9=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp7FB;struct Cyc_Absyn_PtrAtts _tmp7FC;void*_tmp7FD;_LL418: {struct Cyc_Absyn_PointerType_struct*
_tmp7FA=(struct Cyc_Absyn_PointerType_struct*)_tmp7F9;if(_tmp7FA->tag != 5)goto
_LL41A;else{_tmp7FB=_tmp7FA->f1;_tmp7FC=_tmp7FB.ptr_atts;_tmp7FD=_tmp7FC.rgn;}}
_LL419: {void*_tmp7FF=Cyc_Tcutil_compress(_tmp7FD);_LL41F: {struct Cyc_Absyn_HeapRgn_struct*
_tmp800=(struct Cyc_Absyn_HeapRgn_struct*)_tmp7FF;if(_tmp800->tag != 20)goto _LL421;}
_LL420: {int _tmp801=0;_npop_handler(0);return _tmp801;}_LL421:;_LL422: {struct Cyc_Absyn_Kind*
_tmp802=Cyc_Tcutil_typ_kind(_tmp7FD);int _tmp803=_tmp802->kind == Cyc_Absyn_RgnKind
 && _tmp802->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp803;}
_LL41E:;}_LL41A: {struct Cyc_Absyn_TypedefType_struct*_tmp7FE=(struct Cyc_Absyn_TypedefType_struct*)
_tmp7F9;if(_tmp7FE->tag != 17)goto _LL41C;}_LL41B: {const char*_tmp1309;void*
_tmp1308[2];struct Cyc_String_pa_struct _tmp1307;struct Cyc_String_pa_struct
_tmp1306;(_tmp1306.tag=0,((_tmp1306.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp1307.tag=0,((_tmp1307.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp1308[0]=& _tmp1307,((_tmp1308[1]=& _tmp1306,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1309="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp1309,sizeof(char),42))),_tag_dyneither(_tmp1308,sizeof(void*),
2)))))))))))));}_LL41C:;_LL41D: {int _tmp808=0;_npop_handler(0);return _tmp808;}
_LL417:;}{int _tmp809=0;_npop_handler(0);return _tmp809;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp130E;void*_tmp130D[2];
struct Cyc_String_pa_struct _tmp130C;struct Cyc_String_pa_struct _tmp130B;(_tmp130B.tag=
0,((_tmp130B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp130C.tag=0,((_tmp130C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp130D[0]=& _tmp130C,((_tmp130D[1]=& _tmp130B,Cyc_Tcutil_warn(
e->loc,((_tmp130E="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp130E,sizeof(char),53))),_tag_dyneither(_tmp130D,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1314;void*_tmp1313[3];struct Cyc_String_pa_struct _tmp1312;struct Cyc_String_pa_struct
_tmp1311;struct Cyc_String_pa_struct _tmp1310;(_tmp1310.tag=0,((_tmp1310.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1311.tag=0,((_tmp1311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1312.tag=0,((_tmp1312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp1313[0]=& _tmp1312,((_tmp1313[1]=& _tmp1311,((
_tmp1313[2]=& _tmp1310,Cyc_Tcutil_warn(e->loc,((_tmp1314="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp1314,sizeof(char),40))),_tag_dyneither(_tmp1313,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp1319;void*_tmp1318[2];
struct Cyc_String_pa_struct _tmp1317;struct Cyc_String_pa_struct _tmp1316;(_tmp1316.tag=
0,((_tmp1316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1317.tag=0,((_tmp1317.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1318[0]=& _tmp1317,((_tmp1318[1]=& _tmp1316,Cyc_Tcutil_warn(
e->loc,((_tmp1319="implicit cast from %s to %s",_tag_dyneither(_tmp1319,sizeof(
char),28))),_tag_dyneither(_tmp1318,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp817=Cyc_Tcutil_compress(t);_LL424: {struct Cyc_Absyn_IntType_struct*
_tmp818=(struct Cyc_Absyn_IntType_struct*)_tmp817;if(_tmp818->tag != 6)goto _LL426;}
_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_FloatType_struct*_tmp819=(struct Cyc_Absyn_FloatType_struct*)
_tmp817;if(_tmp819->tag != 7)goto _LL428;}_LL427: return 1;_LL428:;_LL429: return 0;
_LL423:;}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,
int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple24{struct Cyc_List_List*
f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){struct
Cyc_List_List*_tmp81B;struct _RegionHandle*_tmp81C;struct Cyc_Tcenv_Tenv*_tmp81D;
int _tmp81E;struct _tuple24 _tmp81A=*env;_tmp81B=_tmp81A.f1;_tmp81C=_tmp81A.f2;
_tmp81D=_tmp81A.f3;_tmp81E=_tmp81A.f4;{void*_tmp81F=Cyc_Tcutil_rsubstitute(
_tmp81C,_tmp81B,x->type);struct Cyc_List_List*_tmp820=Cyc_Tcutil_flatten_typ(
_tmp81C,_tmp81E,_tmp81D,_tmp81F);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp820)== 1){struct _tuple12*_tmp131C;struct Cyc_List_List*_tmp131B;return(
_tmp131B=_region_malloc(_tmp81C,sizeof(*_tmp131B)),((_tmp131B->hd=((_tmp131C=
_region_malloc(_tmp81C,sizeof(*_tmp131C)),((_tmp131C->f1=x->tq,((_tmp131C->f2=
_tmp81F,_tmp131C)))))),((_tmp131B->tl=0,_tmp131B)))));}else{return _tmp820;}};}
struct _tuple25{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};static
struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
struct _RegionHandle*_tmp824;struct Cyc_Tcenv_Tenv*_tmp825;int _tmp826;struct
_tuple25 _tmp823=*env;_tmp824=_tmp823.f1;_tmp825=_tmp823.f2;_tmp826=_tmp823.f3;{
struct Cyc_Absyn_Tqual _tmp828;void*_tmp829;struct _tuple12 _tmp827=*x;_tmp828=
_tmp827.f1;_tmp829=_tmp827.f2;{struct Cyc_List_List*_tmp82A=Cyc_Tcutil_flatten_typ(
_tmp824,_tmp826,_tmp825,_tmp829);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp82A)== 1){struct _tuple12*_tmp131F;struct Cyc_List_List*_tmp131E;return(
_tmp131E=_region_malloc(_tmp824,sizeof(*_tmp131E)),((_tmp131E->hd=((_tmp131F=
_region_malloc(_tmp824,sizeof(*_tmp131F)),((_tmp131F->f1=_tmp828,((_tmp131F->f2=
_tmp829,_tmp131F)))))),((_tmp131E->tl=0,_tmp131E)))));}else{return _tmp82A;}};};}
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int
flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(t1);{
void*_tmp82D=t1;struct Cyc_List_List*_tmp830;struct Cyc_Absyn_AggrInfo _tmp832;
union Cyc_Absyn_AggrInfoU _tmp833;struct Cyc_Absyn_Aggrdecl**_tmp834;struct Cyc_Absyn_Aggrdecl*
_tmp835;struct Cyc_List_List*_tmp836;enum Cyc_Absyn_AggrKind _tmp838;struct Cyc_List_List*
_tmp839;_LL42B: {struct Cyc_Absyn_VoidType_struct*_tmp82E=(struct Cyc_Absyn_VoidType_struct*)
_tmp82D;if(_tmp82E->tag != 0)goto _LL42D;}_LL42C: return 0;_LL42D: {struct Cyc_Absyn_TupleType_struct*
_tmp82F=(struct Cyc_Absyn_TupleType_struct*)_tmp82D;if(_tmp82F->tag != 10)goto
_LL42F;else{_tmp830=_tmp82F->f1;}}_LL42E: {struct _tuple25 _tmp1320;struct _tuple25
_tmp83A=(_tmp1320.f1=r,((_tmp1320.f2=te,((_tmp1320.f3=flatten,_tmp1320)))));
struct Cyc_List_List*_tmp83B=_tmp830;struct Cyc_List_List _tmp83C;struct _tuple12*
_tmp83D;struct Cyc_List_List*_tmp83E;_LL436: if(_tmp83B != 0)goto _LL438;_LL437:
return 0;_LL438: if(_tmp83B == 0)goto _LL435;_tmp83C=*_tmp83B;_tmp83D=(struct
_tuple12*)_tmp83C.hd;_tmp83E=_tmp83C.tl;_LL439: {struct Cyc_List_List*_tmp83F=Cyc_Tcutil_rcopy_tqt(&
_tmp83A,_tmp83D);_tmp83A.f3=0;{struct Cyc_List_List*_tmp840=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),
struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp83A,_tmp830);struct Cyc_List_List*_tmp1321;struct Cyc_List_List*_tmp841=(
_tmp1321=_region_malloc(r,sizeof(*_tmp1321)),((_tmp1321->hd=_tmp83F,((_tmp1321->tl=
_tmp840,_tmp1321)))));struct Cyc_List_List*_tmp1322;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1322=
_region_malloc(r,sizeof(*_tmp1322)),((_tmp1322->hd=_tmp83F,((_tmp1322->tl=
_tmp840,_tmp1322)))))));};}_LL435:;}_LL42F: {struct Cyc_Absyn_AggrType_struct*
_tmp831=(struct Cyc_Absyn_AggrType_struct*)_tmp82D;if(_tmp831->tag != 11)goto
_LL431;else{_tmp832=_tmp831->f1;_tmp833=_tmp832.aggr_info;if((_tmp833.KnownAggr).tag
!= 2)goto _LL431;_tmp834=(struct Cyc_Absyn_Aggrdecl**)(_tmp833.KnownAggr).val;
_tmp835=*_tmp834;_tmp836=_tmp832.targs;}}_LL430: if(((_tmp835->kind == Cyc_Absyn_UnionA
 || _tmp835->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp835->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp835->impl))->rgn_po != 0){
struct _tuple12*_tmp1325;struct Cyc_List_List*_tmp1324;return(_tmp1324=
_region_malloc(r,sizeof(*_tmp1324)),((_tmp1324->hd=((_tmp1325=_region_malloc(r,
sizeof(*_tmp1325)),((_tmp1325->f1=Cyc_Absyn_empty_tqual(0),((_tmp1325->f2=t1,
_tmp1325)))))),((_tmp1324->tl=0,_tmp1324)))));}{struct Cyc_List_List*_tmp847=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp835->tvs,_tmp836);struct _tuple24
_tmp1326;struct _tuple24 env=(_tmp1326.f1=_tmp847,((_tmp1326.f2=r,((_tmp1326.f3=te,((
_tmp1326.f4=flatten,_tmp1326)))))));struct Cyc_List_List*_tmp848=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp835->impl))->fields;struct Cyc_List_List _tmp849;struct Cyc_Absyn_Aggrfield*
_tmp84A;struct Cyc_List_List*_tmp84B;_LL43B: if(_tmp848 != 0)goto _LL43D;_LL43C:
return 0;_LL43D: if(_tmp848 == 0)goto _LL43A;_tmp849=*_tmp848;_tmp84A=(struct Cyc_Absyn_Aggrfield*)
_tmp849.hd;_tmp84B=_tmp849.tl;_LL43E: {struct Cyc_List_List*_tmp84C=Cyc_Tcutil_flatten_typ_f(&
env,_tmp84A);env.f4=0;{struct Cyc_List_List*_tmp84D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp84B);struct Cyc_List_List*_tmp1327;struct Cyc_List_List*_tmp84E=(_tmp1327=
_region_malloc(r,sizeof(*_tmp1327)),((_tmp1327->hd=_tmp84C,((_tmp1327->tl=
_tmp84D,_tmp1327)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp84E);};}_LL43A:;};_LL431: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp837=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp82D;if(_tmp837->tag != 12)goto _LL433;else{_tmp838=_tmp837->f1;if(_tmp838 != 
Cyc_Absyn_StructA)goto _LL433;_tmp839=_tmp837->f2;}}_LL432: {struct _tuple24
_tmp1328;struct _tuple24 env=(_tmp1328.f1=0,((_tmp1328.f2=r,((_tmp1328.f3=te,((
_tmp1328.f4=flatten,_tmp1328)))))));struct Cyc_List_List*_tmp851=_tmp839;struct
Cyc_List_List _tmp852;struct Cyc_Absyn_Aggrfield*_tmp853;struct Cyc_List_List*
_tmp854;_LL440: if(_tmp851 != 0)goto _LL442;_LL441: return 0;_LL442: if(_tmp851 == 0)
goto _LL43F;_tmp852=*_tmp851;_tmp853=(struct Cyc_Absyn_Aggrfield*)_tmp852.hd;
_tmp854=_tmp852.tl;_LL443: {struct Cyc_List_List*_tmp855=Cyc_Tcutil_flatten_typ_f(&
env,_tmp853);env.f4=0;{struct Cyc_List_List*_tmp856=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp854);struct Cyc_List_List*_tmp1329;struct Cyc_List_List*_tmp857=(_tmp1329=
_region_malloc(r,sizeof(*_tmp1329)),((_tmp1329->hd=_tmp855,((_tmp1329->tl=
_tmp856,_tmp1329)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp857);};}_LL43F:;}_LL433:;_LL434:
goto _LL42A;_LL42A:;};}{struct _tuple12*_tmp132C;struct Cyc_List_List*_tmp132B;
return(_tmp132B=_region_malloc(r,sizeof(*_tmp132B)),((_tmp132B->hd=((_tmp132C=
_region_malloc(r,sizeof(*_tmp132C)),((_tmp132C->f1=Cyc_Absyn_empty_tqual(0),((
_tmp132C->f2=t1,_tmp132C)))))),((_tmp132B->tl=0,_tmp132B)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp85C=(void*)t->hd;_LL445: {struct Cyc_Absyn_Pure_att_struct*
_tmp85D=(struct Cyc_Absyn_Pure_att_struct*)_tmp85C;if(_tmp85D->tag != 22)goto
_LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Noreturn_att_struct*_tmp85E=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp85C;if(_tmp85E->tag != 4)goto _LL449;}
_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Initializes_att_struct*_tmp85F=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp85C;if(_tmp85F->tag != 20)goto _LL44B;}
_LL44A: continue;_LL44B:;_LL44C: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL444:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp132D;struct _tuple0 _tmp861=(_tmp132D.f1=Cyc_Tcutil_compress(t1),((
_tmp132D.f2=Cyc_Tcutil_compress(t2),_tmp132D)));void*_tmp862;enum Cyc_Absyn_Size_of
_tmp864;void*_tmp865;enum Cyc_Absyn_Size_of _tmp867;_LL44E: _tmp862=_tmp861.f1;{
struct Cyc_Absyn_IntType_struct*_tmp863=(struct Cyc_Absyn_IntType_struct*)_tmp862;
if(_tmp863->tag != 6)goto _LL450;else{_tmp864=_tmp863->f2;}};_tmp865=_tmp861.f2;{
struct Cyc_Absyn_IntType_struct*_tmp866=(struct Cyc_Absyn_IntType_struct*)_tmp865;
if(_tmp866->tag != 6)goto _LL450;else{_tmp867=_tmp866->f2;}};_LL44F: return(_tmp864
== _tmp867  || _tmp864 == Cyc_Absyn_Int_sz  && _tmp867 == Cyc_Absyn_Long_sz) || 
_tmp864 == Cyc_Absyn_Long_sz  && _tmp867 == Cyc_Absyn_Int_sz;_LL450:;_LL451: return 0;
_LL44D:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp132E;struct _tuple0 _tmp869=(
_tmp132E.f1=t1,((_tmp132E.f2=t2,_tmp132E)));void*_tmp86A;struct Cyc_Absyn_PtrInfo
_tmp86C;void*_tmp86D;struct Cyc_Absyn_Tqual _tmp86E;struct Cyc_Absyn_PtrAtts _tmp86F;
void*_tmp870;union Cyc_Absyn_Constraint*_tmp871;union Cyc_Absyn_Constraint*_tmp872;
union Cyc_Absyn_Constraint*_tmp873;void*_tmp874;struct Cyc_Absyn_PtrInfo _tmp876;
void*_tmp877;struct Cyc_Absyn_Tqual _tmp878;struct Cyc_Absyn_PtrAtts _tmp879;void*
_tmp87A;union Cyc_Absyn_Constraint*_tmp87B;union Cyc_Absyn_Constraint*_tmp87C;
union Cyc_Absyn_Constraint*_tmp87D;void*_tmp87E;struct Cyc_Absyn_DatatypeFieldInfo
_tmp880;union Cyc_Absyn_DatatypeFieldInfoU _tmp881;struct _tuple3 _tmp882;struct Cyc_Absyn_Datatypedecl*
_tmp883;struct Cyc_Absyn_Datatypefield*_tmp884;struct Cyc_List_List*_tmp885;void*
_tmp886;struct Cyc_Absyn_DatatypeInfo _tmp888;union Cyc_Absyn_DatatypeInfoU _tmp889;
struct Cyc_Absyn_Datatypedecl**_tmp88A;struct Cyc_Absyn_Datatypedecl*_tmp88B;
struct Cyc_List_List*_tmp88C;void*_tmp88D;struct Cyc_Absyn_FnInfo _tmp88F;void*
_tmp890;struct Cyc_Absyn_FnInfo _tmp892;_LL453: _tmp86A=_tmp869.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp86B=(struct Cyc_Absyn_PointerType_struct*)_tmp86A;if(_tmp86B->tag != 5)goto
_LL455;else{_tmp86C=_tmp86B->f1;_tmp86D=_tmp86C.elt_typ;_tmp86E=_tmp86C.elt_tq;
_tmp86F=_tmp86C.ptr_atts;_tmp870=_tmp86F.rgn;_tmp871=_tmp86F.nullable;_tmp872=
_tmp86F.bounds;_tmp873=_tmp86F.zero_term;}};_tmp874=_tmp869.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp875=(struct Cyc_Absyn_PointerType_struct*)_tmp874;if(_tmp875->tag != 5)goto
_LL455;else{_tmp876=_tmp875->f1;_tmp877=_tmp876.elt_typ;_tmp878=_tmp876.elt_tq;
_tmp879=_tmp876.ptr_atts;_tmp87A=_tmp879.rgn;_tmp87B=_tmp879.nullable;_tmp87C=
_tmp879.bounds;_tmp87D=_tmp879.zero_term;}};_LL454: if(_tmp86E.real_const  && !
_tmp878.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp871,
_tmp87B) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp871)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87B))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp873,
_tmp87D) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp873)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87D))return 0;if(!Cyc_Tcutil_unify(_tmp870,_tmp87A) && !Cyc_Tcenv_region_outlives(
te,_tmp870,_tmp87A))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp872,_tmp87C)){struct _tuple0 _tmp132F;struct _tuple0 _tmp894=(_tmp132F.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp872),((_tmp132F.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp87C),_tmp132F)));void*
_tmp895;void*_tmp897;void*_tmp899;struct Cyc_Absyn_Exp*_tmp89B;void*_tmp89C;
struct Cyc_Absyn_Exp*_tmp89E;_LL45C: _tmp895=_tmp894.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp896=(struct Cyc_Absyn_Upper_b_struct*)_tmp895;if(_tmp896->tag != 1)goto _LL45E;};
_tmp897=_tmp894.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp898=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp897;if(_tmp898->tag != 0)goto _LL45E;};_LL45D: goto _LL45B;_LL45E: _tmp899=
_tmp894.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp89A=(struct Cyc_Absyn_Upper_b_struct*)
_tmp899;if(_tmp89A->tag != 1)goto _LL460;else{_tmp89B=_tmp89A->f1;}};_tmp89C=
_tmp894.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp89D=(struct Cyc_Absyn_Upper_b_struct*)
_tmp89C;if(_tmp89D->tag != 1)goto _LL460;else{_tmp89E=_tmp89D->f1;}};_LL45F: if(!
Cyc_Evexp_lte_const_exp(_tmp89E,_tmp89B))return 0;goto _LL45B;_LL460:;_LL461:
return 0;_LL45B:;}if(!_tmp878.real_const  && _tmp86E.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp877)))return 0;}{int _tmp89F=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp87C,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp87D);struct _tuple0*
_tmp1332;struct Cyc_List_List*_tmp1331;return(_tmp89F  && Cyc_Tcutil_ptrsubtype(te,((
_tmp1331=_cycalloc(sizeof(*_tmp1331)),((_tmp1331->hd=((_tmp1332=_cycalloc(
sizeof(*_tmp1332)),((_tmp1332->f1=t1,((_tmp1332->f2=t2,_tmp1332)))))),((_tmp1331->tl=
assume,_tmp1331)))))),_tmp86D,_tmp877) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL455: _tmp87E=_tmp869.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp87F=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp87E;if(_tmp87F->tag != 4)
goto _LL457;else{_tmp880=_tmp87F->f1;_tmp881=_tmp880.field_info;if((_tmp881.KnownDatatypefield).tag
!= 2)goto _LL457;_tmp882=(struct _tuple3)(_tmp881.KnownDatatypefield).val;_tmp883=
_tmp882.f1;_tmp884=_tmp882.f2;_tmp885=_tmp880.targs;}};_tmp886=_tmp869.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp887=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp886;if(_tmp887->tag != 3)goto _LL457;else{_tmp888=_tmp887->f1;_tmp889=_tmp888.datatype_info;
if((_tmp889.KnownDatatype).tag != 2)goto _LL457;_tmp88A=(struct Cyc_Absyn_Datatypedecl**)(
_tmp889.KnownDatatype).val;_tmp88B=*_tmp88A;_tmp88C=_tmp888.targs;}};_LL456: if(
_tmp883 != _tmp88B  && Cyc_Absyn_qvar_cmp(_tmp883->name,_tmp88B->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp885)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp88C))return 0;for(0;_tmp885 != 0;(_tmp885=
_tmp885->tl,_tmp88C=_tmp88C->tl)){if(!Cyc_Tcutil_unify((void*)_tmp885->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp88C))->hd))return 0;}return 1;_LL457: _tmp88D=
_tmp869.f1;{struct Cyc_Absyn_FnType_struct*_tmp88E=(struct Cyc_Absyn_FnType_struct*)
_tmp88D;if(_tmp88E->tag != 9)goto _LL459;else{_tmp88F=_tmp88E->f1;}};_tmp890=
_tmp869.f2;{struct Cyc_Absyn_FnType_struct*_tmp891=(struct Cyc_Absyn_FnType_struct*)
_tmp890;if(_tmp891->tag != 9)goto _LL459;else{_tmp892=_tmp891->f1;}};_LL458: if(
_tmp88F.tvars != 0  || _tmp892.tvars != 0){struct Cyc_List_List*_tmp8A2=_tmp88F.tvars;
struct Cyc_List_List*_tmp8A3=_tmp892.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8A2)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8A3))return 0;{
struct _RegionHandle*_tmp8A4=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp8A2 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp8A2->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp8A3))->hd)->kind))return 0;{struct _tuple16*_tmp133C;struct Cyc_Absyn_VarType_struct
_tmp133B;struct Cyc_Absyn_VarType_struct*_tmp133A;struct Cyc_List_List*_tmp1339;
inst=((_tmp1339=_region_malloc(_tmp8A4,sizeof(*_tmp1339)),((_tmp1339->hd=((
_tmp133C=_region_malloc(_tmp8A4,sizeof(*_tmp133C)),((_tmp133C->f1=(struct Cyc_Absyn_Tvar*)
_tmp8A3->hd,((_tmp133C->f2=(void*)((_tmp133A=_cycalloc(sizeof(*_tmp133A)),((
_tmp133A[0]=((_tmp133B.tag=2,((_tmp133B.f1=(struct Cyc_Absyn_Tvar*)_tmp8A2->hd,
_tmp133B)))),_tmp133A)))),_tmp133C)))))),((_tmp1339->tl=inst,_tmp1339))))));}
_tmp8A2=_tmp8A2->tl;_tmp8A3=_tmp8A3->tl;}if(inst != 0){_tmp88F.tvars=0;_tmp892.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1342;struct Cyc_Absyn_FnType_struct*_tmp1341;
struct Cyc_Absyn_FnType_struct _tmp133F;struct Cyc_Absyn_FnType_struct*_tmp133E;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp133E=_cycalloc(sizeof(*_tmp133E)),((
_tmp133E[0]=((_tmp133F.tag=9,((_tmp133F.f1=_tmp88F,_tmp133F)))),_tmp133E)))),(
void*)((_tmp1341=_cycalloc(sizeof(*_tmp1341)),((_tmp1341[0]=((_tmp1342.tag=9,((
_tmp1342.f1=_tmp892,_tmp1342)))),_tmp1341)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp88F.ret_typ,_tmp892.ret_typ))return 0;{struct Cyc_List_List*_tmp8AD=
_tmp88F.args;struct Cyc_List_List*_tmp8AE=_tmp892.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp8AD)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8AE))return 0;for(0;_tmp8AD != 0;(_tmp8AD=_tmp8AD->tl,_tmp8AE=_tmp8AE->tl)){
struct Cyc_Absyn_Tqual _tmp8B0;void*_tmp8B1;struct _tuple10 _tmp8AF=*((struct
_tuple10*)_tmp8AD->hd);_tmp8B0=_tmp8AF.f2;_tmp8B1=_tmp8AF.f3;{struct Cyc_Absyn_Tqual
_tmp8B3;void*_tmp8B4;struct _tuple10 _tmp8B2=*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp8AE))->hd);_tmp8B3=_tmp8B2.f2;_tmp8B4=_tmp8B2.f3;if(_tmp8B3.real_const
 && !_tmp8B0.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B4,_tmp8B1))return
0;};}if(_tmp88F.c_varargs != _tmp892.c_varargs)return 0;if(_tmp88F.cyc_varargs != 0
 && _tmp892.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8B5=*_tmp88F.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8B6=*_tmp892.cyc_varargs;if((_tmp8B6.tq).real_const
 && !(_tmp8B5.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8B6.type,
_tmp8B5.type))return 0;}else{if(_tmp88F.cyc_varargs != 0  || _tmp892.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp88F.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp892.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp88F.rgn_po,_tmp892.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp88F.attributes,_tmp892.attributes))return 0;return 1;};_LL459:;_LL45A: return 0;
_LL452:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle*_tmp8B7=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp8B7,1,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp8B7,1,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple12 _tmp8B9;struct Cyc_Absyn_Tqual
_tmp8BA;void*_tmp8BB;struct _tuple12*_tmp8B8=(struct _tuple12*)tqs1->hd;_tmp8B9=*
_tmp8B8;_tmp8BA=_tmp8B9.f1;_tmp8BB=_tmp8B9.f2;{struct _tuple12 _tmp8BD;struct Cyc_Absyn_Tqual
_tmp8BE;void*_tmp8BF;struct _tuple12*_tmp8BC=(struct _tuple12*)tqs2->hd;_tmp8BD=*
_tmp8BC;_tmp8BE=_tmp8BD.f1;_tmp8BF=_tmp8BD.f2;if(_tmp8BA.real_const  && !_tmp8BE.real_const)
return 0;if((_tmp8BE.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(
_tmp8BF))) && Cyc_Tcutil_subtype(te,assume,_tmp8BB,_tmp8BF))continue;if(Cyc_Tcutil_unify(
_tmp8BB,_tmp8BF))continue;if(Cyc_Tcutil_isomorphic(_tmp8BB,_tmp8BF))continue;
return 0;};};}return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmp8C0=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Size_of _tmp8C2;_LL463: {struct Cyc_Absyn_IntType_struct*_tmp8C1=(
struct Cyc_Absyn_IntType_struct*)_tmp8C0;if(_tmp8C1->tag != 6)goto _LL465;else{
_tmp8C2=_tmp8C1->f2;if(_tmp8C2 != Cyc_Absyn_Char_sz)goto _LL465;}}_LL464: return 1;
_LL465:;_LL466: return 0;_LL462:;}enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*
_tmp8C3=t2;enum Cyc_Absyn_Size_of _tmp8C5;enum Cyc_Absyn_Size_of _tmp8C7;_LL468: {
struct Cyc_Absyn_IntType_struct*_tmp8C4=(struct Cyc_Absyn_IntType_struct*)_tmp8C3;
if(_tmp8C4->tag != 6)goto _LL46A;else{_tmp8C5=_tmp8C4->f2;if(_tmp8C5 != Cyc_Absyn_Int_sz)
goto _LL46A;}}_LL469: goto _LL46B;_LL46A: {struct Cyc_Absyn_IntType_struct*_tmp8C6=(
struct Cyc_Absyn_IntType_struct*)_tmp8C3;if(_tmp8C6->tag != 6)goto _LL46C;else{
_tmp8C7=_tmp8C6->f2;if(_tmp8C7 != Cyc_Absyn_Long_sz)goto _LL46C;}}_LL46B: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL467;_LL46C:;
_LL46D: goto _LL467;_LL467:;}{void*_tmp8C8=t1;struct Cyc_Absyn_PtrInfo _tmp8CA;void*
_tmp8CB;struct Cyc_Absyn_Tqual _tmp8CC;struct Cyc_Absyn_PtrAtts _tmp8CD;void*_tmp8CE;
union Cyc_Absyn_Constraint*_tmp8CF;union Cyc_Absyn_Constraint*_tmp8D0;union Cyc_Absyn_Constraint*
_tmp8D1;struct Cyc_Absyn_ArrayInfo _tmp8D3;void*_tmp8D4;struct Cyc_Absyn_Tqual
_tmp8D5;struct Cyc_Absyn_Exp*_tmp8D6;union Cyc_Absyn_Constraint*_tmp8D7;struct Cyc_Absyn_Enumdecl*
_tmp8D9;void*_tmp8DD;_LL46F: {struct Cyc_Absyn_PointerType_struct*_tmp8C9=(struct
Cyc_Absyn_PointerType_struct*)_tmp8C8;if(_tmp8C9->tag != 5)goto _LL471;else{
_tmp8CA=_tmp8C9->f1;_tmp8CB=_tmp8CA.elt_typ;_tmp8CC=_tmp8CA.elt_tq;_tmp8CD=
_tmp8CA.ptr_atts;_tmp8CE=_tmp8CD.rgn;_tmp8CF=_tmp8CD.nullable;_tmp8D0=_tmp8CD.bounds;
_tmp8D1=_tmp8CD.zero_term;}}_LL470:{void*_tmp8DE=t2;struct Cyc_Absyn_PtrInfo
_tmp8E0;void*_tmp8E1;struct Cyc_Absyn_Tqual _tmp8E2;struct Cyc_Absyn_PtrAtts _tmp8E3;
void*_tmp8E4;union Cyc_Absyn_Constraint*_tmp8E5;union Cyc_Absyn_Constraint*_tmp8E6;
union Cyc_Absyn_Constraint*_tmp8E7;_LL47E: {struct Cyc_Absyn_PointerType_struct*
_tmp8DF=(struct Cyc_Absyn_PointerType_struct*)_tmp8DE;if(_tmp8DF->tag != 5)goto
_LL480;else{_tmp8E0=_tmp8DF->f1;_tmp8E1=_tmp8E0.elt_typ;_tmp8E2=_tmp8E0.elt_tq;
_tmp8E3=_tmp8E0.ptr_atts;_tmp8E4=_tmp8E3.rgn;_tmp8E5=_tmp8E3.nullable;_tmp8E6=
_tmp8E3.bounds;_tmp8E7=_tmp8E3.zero_term;}}_LL47F: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1345;struct Cyc_List_List*
_tmp1344;struct Cyc_List_List*_tmp8E8=(_tmp1344=_cycalloc(sizeof(*_tmp1344)),((
_tmp1344->hd=((_tmp1345=_cycalloc(sizeof(*_tmp1345)),((_tmp1345->f1=t1,((
_tmp1345->f2=t2,_tmp1345)))))),((_tmp1344->tl=0,_tmp1344)))));int _tmp8E9=_tmp8E2.real_const
 || !_tmp8CC.real_const;int _tmp8EA=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8E6,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8E7);int _tmp8EB=_tmp8E9  && (((_tmp8EA  && Cyc_Tcutil_ptrsubtype(
te,_tmp8E8,_tmp8CB,_tmp8E1) || Cyc_Tcutil_unify(_tmp8CB,_tmp8E1)) || Cyc_Tcutil_isomorphic(
_tmp8CB,_tmp8E1)) || Cyc_Tcutil_unify(_tmp8E1,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8D1,_tmp8E7) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8E7);int _tmp8EC=_tmp8EB?0:((Cyc_Tcutil_bits_only(_tmp8CB) && Cyc_Tcutil_is_char_type(
_tmp8E1)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8E7)) && (_tmp8E2.real_const  || !_tmp8CC.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8D0,_tmp8E6);if(!
bounds_ok  && !_tmp8EC){struct _tuple0 _tmp1346;struct _tuple0 _tmp8EE=(_tmp1346.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8D0),((_tmp1346.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8E6),_tmp1346)));
void*_tmp8EF;struct Cyc_Absyn_Exp*_tmp8F1;void*_tmp8F2;struct Cyc_Absyn_Exp*
_tmp8F4;_LL483: _tmp8EF=_tmp8EE.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8F0=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8EF;if(_tmp8F0->tag != 1)goto _LL485;else{
_tmp8F1=_tmp8F0->f1;}};_tmp8F2=_tmp8EE.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp8F3=(struct Cyc_Absyn_Upper_b_struct*)_tmp8F2;if(_tmp8F3->tag != 1)goto _LL485;
else{_tmp8F4=_tmp8F3->f1;}};_LL484: if(Cyc_Evexp_lte_const_exp(_tmp8F4,_tmp8F1))
bounds_ok=1;goto _LL482;_LL485:;_LL486: bounds_ok=1;goto _LL482;_LL482:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8CF) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E5))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8EB  || _tmp8EC)) && (Cyc_Tcutil_unify(
_tmp8CE,_tmp8E4) || Cyc_Tcenv_region_outlives(te,_tmp8CE,_tmp8E4)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL480:;_LL481: goto _LL47D;
_LL47D:;}return Cyc_Absyn_Unknown_coercion;_LL471: {struct Cyc_Absyn_ArrayType_struct*
_tmp8D2=(struct Cyc_Absyn_ArrayType_struct*)_tmp8C8;if(_tmp8D2->tag != 8)goto
_LL473;else{_tmp8D3=_tmp8D2->f1;_tmp8D4=_tmp8D3.elt_type;_tmp8D5=_tmp8D3.tq;
_tmp8D6=_tmp8D3.num_elts;_tmp8D7=_tmp8D3.zero_term;}}_LL472:{void*_tmp8F7=t2;
struct Cyc_Absyn_ArrayInfo _tmp8F9;void*_tmp8FA;struct Cyc_Absyn_Tqual _tmp8FB;
struct Cyc_Absyn_Exp*_tmp8FC;union Cyc_Absyn_Constraint*_tmp8FD;_LL488: {struct Cyc_Absyn_ArrayType_struct*
_tmp8F8=(struct Cyc_Absyn_ArrayType_struct*)_tmp8F7;if(_tmp8F8->tag != 8)goto
_LL48A;else{_tmp8F9=_tmp8F8->f1;_tmp8FA=_tmp8F9.elt_type;_tmp8FB=_tmp8F9.tq;
_tmp8FC=_tmp8F9.num_elts;_tmp8FD=_tmp8F9.zero_term;}}_LL489: {int okay;okay=((
_tmp8D6 != 0  && _tmp8FC != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D7,
_tmp8FD)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp8FC,(struct Cyc_Absyn_Exp*)
_tmp8D6);return(okay  && Cyc_Tcutil_unify(_tmp8D4,_tmp8FA)) && (!_tmp8D5.real_const
 || _tmp8FB.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL48A:;_LL48B: return Cyc_Absyn_Unknown_coercion;_LL487:;}return Cyc_Absyn_Unknown_coercion;
_LL473: {struct Cyc_Absyn_EnumType_struct*_tmp8D8=(struct Cyc_Absyn_EnumType_struct*)
_tmp8C8;if(_tmp8D8->tag != 13)goto _LL475;else{_tmp8D9=_tmp8D8->f2;}}_LL474:{void*
_tmp8FE=t2;struct Cyc_Absyn_Enumdecl*_tmp900;_LL48D: {struct Cyc_Absyn_EnumType_struct*
_tmp8FF=(struct Cyc_Absyn_EnumType_struct*)_tmp8FE;if(_tmp8FF->tag != 13)goto
_LL48F;else{_tmp900=_tmp8FF->f2;}}_LL48E: if((_tmp8D9->fields != 0  && _tmp900->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8D9->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp900->fields))->v))return Cyc_Absyn_No_coercion;goto _LL48C;_LL48F:;_LL490: goto
_LL48C;_LL48C:;}goto _LL476;_LL475: {struct Cyc_Absyn_IntType_struct*_tmp8DA=(
struct Cyc_Absyn_IntType_struct*)_tmp8C8;if(_tmp8DA->tag != 6)goto _LL477;}_LL476:
goto _LL478;_LL477: {struct Cyc_Absyn_FloatType_struct*_tmp8DB=(struct Cyc_Absyn_FloatType_struct*)
_tmp8C8;if(_tmp8DB->tag != 7)goto _LL479;}_LL478: return Cyc_Tcutil_coerceable(t2)?
Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;_LL479: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8DC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp8C8;if(_tmp8DC->tag != 15)goto
_LL47B;else{_tmp8DD=(void*)_tmp8DC->f1;}}_LL47A:{void*_tmp901=t2;void*_tmp903;
_LL492: {struct Cyc_Absyn_RgnHandleType_struct*_tmp902=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp901;if(_tmp902->tag != 15)goto _LL494;else{_tmp903=(void*)_tmp902->f1;}}_LL493:
if(Cyc_Tcenv_region_outlives(te,_tmp8DD,_tmp903))return Cyc_Absyn_No_coercion;
goto _LL491;_LL494:;_LL495: goto _LL491;_LL491:;}return Cyc_Absyn_Unknown_coercion;
_LL47B:;_LL47C: return Cyc_Absyn_Unknown_coercion;_LL46E:;};}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){
struct Cyc_Absyn_Exp*_tmp904=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct
_tmp1349;struct Cyc_Absyn_Cast_e_struct*_tmp1348;e->r=(void*)((_tmp1348=_cycalloc(
sizeof(*_tmp1348)),((_tmp1348[0]=((_tmp1349.tag=15,((_tmp1349.f1=(void*)t,((
_tmp1349.f2=_tmp904,((_tmp1349.f3=0,((_tmp1349.f4=c,_tmp1349)))))))))),_tmp1348))));}{
struct Cyc_Core_Opt*_tmp134A;e->topt=((_tmp134A=_cycalloc(sizeof(*_tmp134A)),((
_tmp134A->v=t,_tmp134A))));};}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp908=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL497: {struct Cyc_Absyn_IntType_struct*_tmp909=(struct Cyc_Absyn_IntType_struct*)
_tmp908;if(_tmp909->tag != 6)goto _LL499;}_LL498: goto _LL49A;_LL499: {struct Cyc_Absyn_EnumType_struct*
_tmp90A=(struct Cyc_Absyn_EnumType_struct*)_tmp908;if(_tmp90A->tag != 13)goto
_LL49B;}_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_AnonEnumType_struct*_tmp90B=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp908;if(_tmp90B->tag != 14)goto _LL49D;}
_LL49C: goto _LL49E;_LL49D: {struct Cyc_Absyn_TagType_struct*_tmp90C=(struct Cyc_Absyn_TagType_struct*)
_tmp908;if(_tmp90C->tag != 19)goto _LL49F;}_LL49E: return 1;_LL49F: {struct Cyc_Absyn_Evar_struct*
_tmp90D=(struct Cyc_Absyn_Evar_struct*)_tmp908;if(_tmp90D->tag != 1)goto _LL4A1;}
_LL4A0: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL4A1:;_LL4A2: return 0;_LL496:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp90E=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL4A4: {struct Cyc_Absyn_FloatType_struct*
_tmp90F=(struct Cyc_Absyn_FloatType_struct*)_tmp90E;if(_tmp90F->tag != 7)goto
_LL4A6;}_LL4A5: return 1;_LL4A6:;_LL4A7: return 0;_LL4A3:;};}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp910=Cyc_Tcutil_compress(t);_LL4A9: {struct Cyc_Absyn_FnType_struct*
_tmp911=(struct Cyc_Absyn_FnType_struct*)_tmp910;if(_tmp911->tag != 9)goto _LL4AB;}
_LL4AA: return 1;_LL4AB:;_LL4AC: return 0;_LL4A8:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple0 _tmp134B;struct _tuple0 _tmp913=(_tmp134B.f1=t1,((
_tmp134B.f2=t2,_tmp134B)));void*_tmp914;int _tmp916;void*_tmp917;int _tmp919;void*
_tmp91A;void*_tmp91C;void*_tmp91E;enum Cyc_Absyn_Sign _tmp920;enum Cyc_Absyn_Size_of
_tmp921;void*_tmp922;enum Cyc_Absyn_Sign _tmp924;enum Cyc_Absyn_Size_of _tmp925;
void*_tmp926;enum Cyc_Absyn_Size_of _tmp928;void*_tmp929;enum Cyc_Absyn_Size_of
_tmp92B;void*_tmp92C;enum Cyc_Absyn_Sign _tmp92E;enum Cyc_Absyn_Size_of _tmp92F;
void*_tmp930;enum Cyc_Absyn_Sign _tmp932;enum Cyc_Absyn_Size_of _tmp933;void*
_tmp934;enum Cyc_Absyn_Sign _tmp936;enum Cyc_Absyn_Size_of _tmp937;void*_tmp938;
enum Cyc_Absyn_Sign _tmp93A;enum Cyc_Absyn_Size_of _tmp93B;void*_tmp93C;enum Cyc_Absyn_Size_of
_tmp93E;void*_tmp93F;enum Cyc_Absyn_Size_of _tmp941;_LL4AE: _tmp914=_tmp913.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp915=(struct Cyc_Absyn_FloatType_struct*)
_tmp914;if(_tmp915->tag != 7)goto _LL4B0;else{_tmp916=_tmp915->f1;}};_tmp917=
_tmp913.f2;{struct Cyc_Absyn_FloatType_struct*_tmp918=(struct Cyc_Absyn_FloatType_struct*)
_tmp917;if(_tmp918->tag != 7)goto _LL4B0;else{_tmp919=_tmp918->f1;}};_LL4AF: if(
_tmp916 != 0  && _tmp916 != 1)return t1;else{if(_tmp919 != 0  && _tmp919 != 1)return t2;
else{if(_tmp916 >= _tmp919)return t1;else{return t2;}}}_LL4B0: _tmp91A=_tmp913.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp91B=(struct Cyc_Absyn_FloatType_struct*)
_tmp91A;if(_tmp91B->tag != 7)goto _LL4B2;};_LL4B1: return t1;_LL4B2: _tmp91C=_tmp913.f2;{
struct Cyc_Absyn_FloatType_struct*_tmp91D=(struct Cyc_Absyn_FloatType_struct*)
_tmp91C;if(_tmp91D->tag != 7)goto _LL4B4;};_LL4B3: return t2;_LL4B4: _tmp91E=_tmp913.f1;{
struct Cyc_Absyn_IntType_struct*_tmp91F=(struct Cyc_Absyn_IntType_struct*)_tmp91E;
if(_tmp91F->tag != 6)goto _LL4B6;else{_tmp920=_tmp91F->f1;if(_tmp920 != Cyc_Absyn_Unsigned)
goto _LL4B6;_tmp921=_tmp91F->f2;if(_tmp921 != Cyc_Absyn_LongLong_sz)goto _LL4B6;}};
_LL4B5: goto _LL4B7;_LL4B6: _tmp922=_tmp913.f2;{struct Cyc_Absyn_IntType_struct*
_tmp923=(struct Cyc_Absyn_IntType_struct*)_tmp922;if(_tmp923->tag != 6)goto _LL4B8;
else{_tmp924=_tmp923->f1;if(_tmp924 != Cyc_Absyn_Unsigned)goto _LL4B8;_tmp925=
_tmp923->f2;if(_tmp925 != Cyc_Absyn_LongLong_sz)goto _LL4B8;}};_LL4B7: return Cyc_Absyn_ulonglong_typ;
_LL4B8: _tmp926=_tmp913.f1;{struct Cyc_Absyn_IntType_struct*_tmp927=(struct Cyc_Absyn_IntType_struct*)
_tmp926;if(_tmp927->tag != 6)goto _LL4BA;else{_tmp928=_tmp927->f2;if(_tmp928 != Cyc_Absyn_LongLong_sz)
goto _LL4BA;}};_LL4B9: goto _LL4BB;_LL4BA: _tmp929=_tmp913.f2;{struct Cyc_Absyn_IntType_struct*
_tmp92A=(struct Cyc_Absyn_IntType_struct*)_tmp929;if(_tmp92A->tag != 6)goto _LL4BC;
else{_tmp92B=_tmp92A->f2;if(_tmp92B != Cyc_Absyn_LongLong_sz)goto _LL4BC;}};_LL4BB:
return Cyc_Absyn_slonglong_typ;_LL4BC: _tmp92C=_tmp913.f1;{struct Cyc_Absyn_IntType_struct*
_tmp92D=(struct Cyc_Absyn_IntType_struct*)_tmp92C;if(_tmp92D->tag != 6)goto _LL4BE;
else{_tmp92E=_tmp92D->f1;if(_tmp92E != Cyc_Absyn_Unsigned)goto _LL4BE;_tmp92F=
_tmp92D->f2;if(_tmp92F != Cyc_Absyn_Long_sz)goto _LL4BE;}};_LL4BD: goto _LL4BF;
_LL4BE: _tmp930=_tmp913.f2;{struct Cyc_Absyn_IntType_struct*_tmp931=(struct Cyc_Absyn_IntType_struct*)
_tmp930;if(_tmp931->tag != 6)goto _LL4C0;else{_tmp932=_tmp931->f1;if(_tmp932 != Cyc_Absyn_Unsigned)
goto _LL4C0;_tmp933=_tmp931->f2;if(_tmp933 != Cyc_Absyn_Long_sz)goto _LL4C0;}};
_LL4BF: return Cyc_Absyn_ulong_typ;_LL4C0: _tmp934=_tmp913.f1;{struct Cyc_Absyn_IntType_struct*
_tmp935=(struct Cyc_Absyn_IntType_struct*)_tmp934;if(_tmp935->tag != 6)goto _LL4C2;
else{_tmp936=_tmp935->f1;if(_tmp936 != Cyc_Absyn_Unsigned)goto _LL4C2;_tmp937=
_tmp935->f2;if(_tmp937 != Cyc_Absyn_Int_sz)goto _LL4C2;}};_LL4C1: goto _LL4C3;_LL4C2:
_tmp938=_tmp913.f2;{struct Cyc_Absyn_IntType_struct*_tmp939=(struct Cyc_Absyn_IntType_struct*)
_tmp938;if(_tmp939->tag != 6)goto _LL4C4;else{_tmp93A=_tmp939->f1;if(_tmp93A != Cyc_Absyn_Unsigned)
goto _LL4C4;_tmp93B=_tmp939->f2;if(_tmp93B != Cyc_Absyn_Int_sz)goto _LL4C4;}};
_LL4C3: return Cyc_Absyn_uint_typ;_LL4C4: _tmp93C=_tmp913.f1;{struct Cyc_Absyn_IntType_struct*
_tmp93D=(struct Cyc_Absyn_IntType_struct*)_tmp93C;if(_tmp93D->tag != 6)goto _LL4C6;
else{_tmp93E=_tmp93D->f2;if(_tmp93E != Cyc_Absyn_Long_sz)goto _LL4C6;}};_LL4C5:
goto _LL4C7;_LL4C6: _tmp93F=_tmp913.f2;{struct Cyc_Absyn_IntType_struct*_tmp940=(
struct Cyc_Absyn_IntType_struct*)_tmp93F;if(_tmp940->tag != 6)goto _LL4C8;else{
_tmp941=_tmp940->f2;if(_tmp941 != Cyc_Absyn_Long_sz)goto _LL4C8;}};_LL4C7: return
Cyc_Absyn_slong_typ;_LL4C8:;_LL4C9: return Cyc_Absyn_sint_typ;_LL4AD:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp942=e->r;struct Cyc_Core_Opt*_tmp944;_LL4CB: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp943=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp942;if(_tmp943->tag != 4)goto _LL4CD;else{_tmp944=_tmp943->f2;if(_tmp944 != 0)
goto _LL4CD;}}_LL4CC:{const char*_tmp134E;void*_tmp134D;(_tmp134D=0,Cyc_Tcutil_warn(
e->loc,((_tmp134E="assignment in test",_tag_dyneither(_tmp134E,sizeof(char),19))),
_tag_dyneither(_tmp134D,sizeof(void*),0)));}goto _LL4CA;_LL4CD:;_LL4CE: goto _LL4CA;
_LL4CA:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp134F;struct _tuple0 _tmp948=(
_tmp134F.f1=c1,((_tmp134F.f2=c2,_tmp134F)));void*_tmp949;struct Cyc_Absyn_Kind*
_tmp94B;void*_tmp94C;struct Cyc_Absyn_Kind*_tmp94E;void*_tmp94F;struct Cyc_Core_Opt*
_tmp951;struct Cyc_Core_Opt**_tmp952;void*_tmp953;struct Cyc_Core_Opt*_tmp955;
struct Cyc_Core_Opt**_tmp956;void*_tmp957;struct Cyc_Core_Opt*_tmp959;struct Cyc_Core_Opt**
_tmp95A;struct Cyc_Absyn_Kind*_tmp95B;void*_tmp95C;struct Cyc_Absyn_Kind*_tmp95E;
void*_tmp95F;struct Cyc_Absyn_Kind*_tmp961;void*_tmp962;struct Cyc_Core_Opt*
_tmp964;struct Cyc_Core_Opt**_tmp965;struct Cyc_Absyn_Kind*_tmp966;void*_tmp967;
struct Cyc_Core_Opt*_tmp969;struct Cyc_Core_Opt**_tmp96A;struct Cyc_Absyn_Kind*
_tmp96B;void*_tmp96C;struct Cyc_Core_Opt*_tmp96E;struct Cyc_Core_Opt**_tmp96F;
struct Cyc_Absyn_Kind*_tmp970;_LL4D0: _tmp949=_tmp948.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp94A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp949;if(_tmp94A->tag != 0)goto _LL4D2;
else{_tmp94B=_tmp94A->f1;}};_tmp94C=_tmp948.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp94D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp94C;if(_tmp94D->tag != 0)goto _LL4D2;
else{_tmp94E=_tmp94D->f1;}};_LL4D1: return _tmp94B == _tmp94E;_LL4D2: _tmp94F=
_tmp948.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp950=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp94F;if(_tmp950->tag != 1)goto _LL4D4;else{_tmp951=_tmp950->f1;_tmp952=(struct
Cyc_Core_Opt**)& _tmp950->f1;}};_LL4D3:{struct Cyc_Core_Opt*_tmp1350;*_tmp952=((
_tmp1350=_cycalloc(sizeof(*_tmp1350)),((_tmp1350->v=c1,_tmp1350))));}return 1;
_LL4D4: _tmp953=_tmp948.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp954=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp953;if(_tmp954->tag != 1)goto _LL4D6;else{_tmp955=_tmp954->f1;_tmp956=(struct
Cyc_Core_Opt**)& _tmp954->f1;}};_LL4D5:{struct Cyc_Core_Opt*_tmp1351;*_tmp956=((
_tmp1351=_cycalloc(sizeof(*_tmp1351)),((_tmp1351->v=c2,_tmp1351))));}return 1;
_LL4D6: _tmp957=_tmp948.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp958=(struct Cyc_Absyn_Less_kb_struct*)
_tmp957;if(_tmp958->tag != 2)goto _LL4D8;else{_tmp959=_tmp958->f1;_tmp95A=(struct
Cyc_Core_Opt**)& _tmp958->f1;_tmp95B=_tmp958->f2;}};_tmp95C=_tmp948.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp95D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95C;if(_tmp95D->tag != 0)goto _LL4D8;
else{_tmp95E=_tmp95D->f1;}};_LL4D7: if(Cyc_Tcutil_kind_leq(_tmp95E,_tmp95B)){{
struct Cyc_Core_Opt*_tmp1352;*_tmp95A=((_tmp1352=_cycalloc(sizeof(*_tmp1352)),((
_tmp1352->v=c2,_tmp1352))));}return 1;}else{return 0;}_LL4D8: _tmp95F=_tmp948.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp960=(struct Cyc_Absyn_Eq_kb_struct*)_tmp95F;if(
_tmp960->tag != 0)goto _LL4DA;else{_tmp961=_tmp960->f1;}};_tmp962=_tmp948.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp963=(struct Cyc_Absyn_Less_kb_struct*)_tmp962;
if(_tmp963->tag != 2)goto _LL4DA;else{_tmp964=_tmp963->f1;_tmp965=(struct Cyc_Core_Opt**)&
_tmp963->f1;_tmp966=_tmp963->f2;}};_LL4D9: if(Cyc_Tcutil_kind_leq(_tmp961,_tmp966)){{
struct Cyc_Core_Opt*_tmp1353;*_tmp965=((_tmp1353=_cycalloc(sizeof(*_tmp1353)),((
_tmp1353->v=c1,_tmp1353))));}return 1;}else{return 0;}_LL4DA: _tmp967=_tmp948.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp968=(struct Cyc_Absyn_Less_kb_struct*)_tmp967;
if(_tmp968->tag != 2)goto _LL4CF;else{_tmp969=_tmp968->f1;_tmp96A=(struct Cyc_Core_Opt**)&
_tmp968->f1;_tmp96B=_tmp968->f2;}};_tmp96C=_tmp948.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp96D=(struct Cyc_Absyn_Less_kb_struct*)_tmp96C;if(_tmp96D->tag != 2)goto _LL4CF;
else{_tmp96E=_tmp96D->f1;_tmp96F=(struct Cyc_Core_Opt**)& _tmp96D->f1;_tmp970=
_tmp96D->f2;}};_LL4DB: if(Cyc_Tcutil_kind_leq(_tmp96B,_tmp970)){{struct Cyc_Core_Opt*
_tmp1354;*_tmp96F=((_tmp1354=_cycalloc(sizeof(*_tmp1354)),((_tmp1354->v=c1,
_tmp1354))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp970,_tmp96B)){{struct Cyc_Core_Opt*
_tmp1355;*_tmp96A=((_tmp1355=_cycalloc(sizeof(*_tmp1355)),((_tmp1355->v=c2,
_tmp1355))));}return 1;}else{return 0;}}_LL4CF:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp1359;void*_tmp1358[1];struct Cyc_Int_pa_struct _tmp1357;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1357.tag=1,((_tmp1357.f1=(
unsigned long)i,((_tmp1358[0]=& _tmp1357,Cyc_aprintf(((_tmp1359="#%d",
_tag_dyneither(_tmp1359,sizeof(char),4))),_tag_dyneither(_tmp1358,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp135C;struct Cyc_Absyn_Tvar*_tmp135B;return(
_tmp135B=_cycalloc(sizeof(*_tmp135B)),((_tmp135B->name=((_tmp135C=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp135C[0]=s,_tmp135C)))),((_tmp135B->identity=
- 1,((_tmp135B->kind=k,_tmp135B)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp97C=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp97C,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1360;void*_tmp135F[1];struct Cyc_String_pa_struct
_tmp135E;(_tmp135E.tag=0,((_tmp135E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp135F[0]=& _tmp135E,Cyc_printf(((_tmp1360="%s",
_tag_dyneither(_tmp1360,sizeof(char),3))),_tag_dyneither(_tmp135F,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1361;struct
_dyneither_ptr _tmp980=Cyc_strconcat(((_tmp1361="`",_tag_dyneither(_tmp1361,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1364;char _tmp1363;
struct _dyneither_ptr _tmp1362;(_tmp1362=_dyneither_ptr_plus(_tmp980,sizeof(char),
1),((_tmp1363=*((char*)_check_dyneither_subscript(_tmp1362,sizeof(char),0)),((
_tmp1364='t',((_get_dyneither_size(_tmp1362,sizeof(char))== 1  && (_tmp1363 == '\000'
 && _tmp1364 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1362.curr)=_tmp1364)))))));}{
struct _dyneither_ptr*_tmp1365;t->name=((_tmp1365=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1365[0]=(struct _dyneither_ptr)_tmp980,_tmp1365))));};};}
struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple26*x){struct Cyc_Core_Opt*
_tmp1368;struct _tuple10*_tmp1367;return(_tmp1367=_cycalloc(sizeof(*_tmp1367)),((
_tmp1367->f1=(struct Cyc_Core_Opt*)((_tmp1368=_cycalloc(sizeof(*_tmp1368)),((
_tmp1368->v=(*x).f1,_tmp1368)))),((_tmp1367->f2=(*x).f2,((_tmp1367->f3=(*x).f3,
_tmp1367)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp988=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp1369;_tmp988=((_tmp1369=_cycalloc(sizeof(*_tmp1369)),((
_tmp1369->hd=(void*)atts->hd,((_tmp1369->tl=_tmp988,_tmp1369))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp136F;struct Cyc_Absyn_FnInfo _tmp136E;struct Cyc_Absyn_FnType_struct*_tmp136D;
return(void*)((_tmp136D=_cycalloc(sizeof(*_tmp136D)),((_tmp136D[0]=((_tmp136F.tag=
9,((_tmp136F.f1=((_tmp136E.tvars=fd->tvs,((_tmp136E.effect=fd->effect,((_tmp136E.ret_tqual=
fd->ret_tqual,((_tmp136E.ret_typ=fd->ret_type,((_tmp136E.args=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp136E.c_varargs=fd->c_varargs,((_tmp136E.cyc_varargs=fd->cyc_varargs,((
_tmp136E.rgn_po=fd->rgn_po,((_tmp136E.attributes=_tmp988,_tmp136E)))))))))))))))))),
_tmp136F)))),_tmp136D))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple27{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple27*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){
return(*t).f2;}static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp1370;return(_tmp1370=_cycalloc(sizeof(*_tmp1370)),((_tmp1370->f1=(*
pr).f1,((_tmp1370->f2=t,_tmp1370)))));}struct _tuple28{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple29{struct _tuple28*f1;void*f2;};static
struct _tuple29*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
struct _tuple28*_tmp1373;struct _tuple29*_tmp1372;return(_tmp1372=_region_malloc(
rgn,sizeof(*_tmp1372)),((_tmp1372->f1=((_tmp1373=_region_malloc(rgn,sizeof(*
_tmp1373)),((_tmp1373->f1=(*y).f1,((_tmp1373->f2=(*y).f2,_tmp1373)))))),((
_tmp1372->f2=(*y).f3,_tmp1372)))));}static struct _tuple10*Cyc_Tcutil_substitute_f2(
struct _tuple29*w){struct _tuple28*_tmp991;void*_tmp992;struct _tuple29 _tmp990=*w;
_tmp991=_tmp990.f1;_tmp992=_tmp990.f2;{struct Cyc_Core_Opt*_tmp994;struct Cyc_Absyn_Tqual
_tmp995;struct _tuple28 _tmp993=*_tmp991;_tmp994=_tmp993.f1;_tmp995=_tmp993.f2;{
struct _tuple10*_tmp1374;return(_tmp1374=_cycalloc(sizeof(*_tmp1374)),((_tmp1374->f1=
_tmp994,((_tmp1374->f2=_tmp995,((_tmp1374->f3=_tmp992,_tmp1374)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){return f->type;}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*
f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1375;return(_tmp1375=_cycalloc(sizeof(*
_tmp1375)),((_tmp1375->name=f->name,((_tmp1375->tq=f->tq,((_tmp1375->type=t,((
_tmp1375->width=f->width,((_tmp1375->attributes=f->attributes,_tmp1375)))))))))));}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp1376;return(_tmp1376=_cycalloc(
sizeof(*_tmp1376)),((_tmp1376->topt=old->topt,((_tmp1376->r=r,((_tmp1376->loc=
old->loc,((_tmp1376->annot=old->annot,_tmp1376)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp999=e->r;enum Cyc_Absyn_Primop _tmp99F;struct Cyc_List_List*_tmp9A0;
struct Cyc_Absyn_Exp*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A3;struct Cyc_Absyn_Exp*
_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A6;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*
_tmp9A9;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AC;struct Cyc_Absyn_Exp*
_tmp9AD;void*_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;int _tmp9B1;enum Cyc_Absyn_Coercion
_tmp9B2;void*_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;void*_tmp9B8;void*_tmp9B9;void*
_tmp9BB;_LL4DD: {struct Cyc_Absyn_Const_e_struct*_tmp99A=(struct Cyc_Absyn_Const_e_struct*)
_tmp999;if(_tmp99A->tag != 0)goto _LL4DF;}_LL4DE: goto _LL4E0;_LL4DF: {struct Cyc_Absyn_Enum_e_struct*
_tmp99B=(struct Cyc_Absyn_Enum_e_struct*)_tmp999;if(_tmp99B->tag != 32)goto _LL4E1;}
_LL4E0: goto _LL4E2;_LL4E1: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp99C=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp999;if(_tmp99C->tag != 33)goto _LL4E3;}_LL4E2: goto _LL4E4;_LL4E3: {struct Cyc_Absyn_Var_e_struct*
_tmp99D=(struct Cyc_Absyn_Var_e_struct*)_tmp999;if(_tmp99D->tag != 1)goto _LL4E5;}
_LL4E4: return e;_LL4E5: {struct Cyc_Absyn_Primop_e_struct*_tmp99E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp999;if(_tmp99E->tag != 3)goto _LL4E7;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99E->f2;}}
_LL4E6: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9A0)== 1){struct
Cyc_Absyn_Exp*_tmp9BC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp9A0))->hd;struct Cyc_Absyn_Exp*_tmp9BD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9BC);
if(_tmp9BD == _tmp9BC)return e;{struct Cyc_Absyn_Primop_e_struct _tmp137C;struct Cyc_Absyn_Exp*
_tmp137B[1];struct Cyc_Absyn_Primop_e_struct*_tmp137A;return Cyc_Tcutil_copye(e,(
void*)((_tmp137A=_cycalloc(sizeof(*_tmp137A)),((_tmp137A[0]=((_tmp137C.tag=3,((
_tmp137C.f1=_tmp99F,((_tmp137C.f2=((_tmp137B[0]=_tmp9BD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp137B,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp137C)))))),_tmp137A)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp9A0)== 2){struct Cyc_Absyn_Exp*_tmp9C1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9A0))->hd;struct Cyc_Absyn_Exp*_tmp9C2=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9A0->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9C3=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C1);struct Cyc_Absyn_Exp*
_tmp9C4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C2);if(_tmp9C3 == _tmp9C1  && _tmp9C4 == 
_tmp9C2)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1382;struct Cyc_Absyn_Exp*
_tmp1381[2];struct Cyc_Absyn_Primop_e_struct*_tmp1380;return Cyc_Tcutil_copye(e,(
void*)((_tmp1380=_cycalloc(sizeof(*_tmp1380)),((_tmp1380[0]=((_tmp1382.tag=3,((
_tmp1382.f1=_tmp99F,((_tmp1382.f2=((_tmp1381[1]=_tmp9C4,((_tmp1381[0]=_tmp9C3,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1381,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1382)))))),_tmp1380)))));};}
else{const char*_tmp1385;void*_tmp1384;return(_tmp1384=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1385="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1385,sizeof(char),34))),_tag_dyneither(_tmp1384,sizeof(void*),
0)));}}_LL4E7: {struct Cyc_Absyn_Conditional_e_struct*_tmp9A1=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp999;if(_tmp9A1->tag != 6)goto _LL4E9;else{_tmp9A2=_tmp9A1->f1;_tmp9A3=_tmp9A1->f2;
_tmp9A4=_tmp9A1->f3;}}_LL4E8: {struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Tcutil_rsubsexp(r,
inst,_tmp9A2);struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A3);
struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A4);if((_tmp9CA == 
_tmp9A2  && _tmp9CB == _tmp9A3) && _tmp9CC == _tmp9A4)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp1388;struct Cyc_Absyn_Conditional_e_struct*_tmp1387;return Cyc_Tcutil_copye(e,(
void*)((_tmp1387=_cycalloc(sizeof(*_tmp1387)),((_tmp1387[0]=((_tmp1388.tag=6,((
_tmp1388.f1=_tmp9CA,((_tmp1388.f2=_tmp9CB,((_tmp1388.f3=_tmp9CC,_tmp1388)))))))),
_tmp1387)))));};}_LL4E9: {struct Cyc_Absyn_And_e_struct*_tmp9A5=(struct Cyc_Absyn_And_e_struct*)
_tmp999;if(_tmp9A5->tag != 7)goto _LL4EB;else{_tmp9A6=_tmp9A5->f1;_tmp9A7=_tmp9A5->f2;}}
_LL4EA: {struct Cyc_Absyn_Exp*_tmp9CF=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A6);struct
Cyc_Absyn_Exp*_tmp9D0=Cyc_Tcutil_rsubsexp(r,inst,_tmp9A7);if(_tmp9CF == _tmp9A6
 && _tmp9D0 == _tmp9A7)return e;{struct Cyc_Absyn_And_e_struct _tmp138B;struct Cyc_Absyn_And_e_struct*
_tmp138A;return Cyc_Tcutil_copye(e,(void*)((_tmp138A=_cycalloc(sizeof(*_tmp138A)),((
_tmp138A[0]=((_tmp138B.tag=7,((_tmp138B.f1=_tmp9CF,((_tmp138B.f2=_tmp9D0,
_tmp138B)))))),_tmp138A)))));};}_LL4EB: {struct Cyc_Absyn_Or_e_struct*_tmp9A8=(
struct Cyc_Absyn_Or_e_struct*)_tmp999;if(_tmp9A8->tag != 8)goto _LL4ED;else{_tmp9A9=
_tmp9A8->f1;_tmp9AA=_tmp9A8->f2;}}_LL4EC: {struct Cyc_Absyn_Exp*_tmp9D3=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9A9);struct Cyc_Absyn_Exp*_tmp9D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AA);
if(_tmp9D3 == _tmp9A9  && _tmp9D4 == _tmp9AA)return e;{struct Cyc_Absyn_Or_e_struct
_tmp138E;struct Cyc_Absyn_Or_e_struct*_tmp138D;return Cyc_Tcutil_copye(e,(void*)((
_tmp138D=_cycalloc(sizeof(*_tmp138D)),((_tmp138D[0]=((_tmp138E.tag=8,((_tmp138E.f1=
_tmp9D3,((_tmp138E.f2=_tmp9D4,_tmp138E)))))),_tmp138D)))));};}_LL4ED: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9AB=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp999;if(_tmp9AB->tag != 9)goto _LL4EF;
else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;}}_LL4EE: {struct Cyc_Absyn_Exp*
_tmp9D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp9AC);struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9AD);if(_tmp9D7 == _tmp9AC  && _tmp9D8 == _tmp9AD)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp1391;struct Cyc_Absyn_SeqExp_e_struct*_tmp1390;return Cyc_Tcutil_copye(e,(void*)((
_tmp1390=_cycalloc(sizeof(*_tmp1390)),((_tmp1390[0]=((_tmp1391.tag=9,((_tmp1391.f1=
_tmp9D7,((_tmp1391.f2=_tmp9D8,_tmp1391)))))),_tmp1390)))));};}_LL4EF: {struct Cyc_Absyn_Cast_e_struct*
_tmp9AE=(struct Cyc_Absyn_Cast_e_struct*)_tmp999;if(_tmp9AE->tag != 15)goto _LL4F1;
else{_tmp9AF=(void*)_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;_tmp9B1=_tmp9AE->f3;_tmp9B2=
_tmp9AE->f4;}}_LL4F0: {struct Cyc_Absyn_Exp*_tmp9DB=Cyc_Tcutil_rsubsexp(r,inst,
_tmp9B0);void*_tmp9DC=Cyc_Tcutil_rsubstitute(r,inst,_tmp9AF);if(_tmp9DB == 
_tmp9B0  && _tmp9DC == _tmp9AF)return e;{struct Cyc_Absyn_Cast_e_struct _tmp1394;
struct Cyc_Absyn_Cast_e_struct*_tmp1393;return Cyc_Tcutil_copye(e,(void*)((
_tmp1393=_cycalloc(sizeof(*_tmp1393)),((_tmp1393[0]=((_tmp1394.tag=15,((_tmp1394.f1=(
void*)_tmp9DC,((_tmp1394.f2=_tmp9DB,((_tmp1394.f3=_tmp9B1,((_tmp1394.f4=_tmp9B2,
_tmp1394)))))))))),_tmp1393)))));};}_LL4F1: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9B3=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp999;if(_tmp9B3->tag != 18)goto
_LL4F3;else{_tmp9B4=(void*)_tmp9B3->f1;}}_LL4F2: {void*_tmp9DF=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9B4);if(_tmp9DF == _tmp9B4)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp1397;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp1396;return Cyc_Tcutil_copye(e,(
void*)((_tmp1396=_cycalloc(sizeof(*_tmp1396)),((_tmp1396[0]=((_tmp1397.tag=18,((
_tmp1397.f1=(void*)_tmp9DF,_tmp1397)))),_tmp1396)))));};}_LL4F3: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9B5=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp999;if(_tmp9B5->tag != 19)goto
_LL4F5;else{_tmp9B6=_tmp9B5->f1;}}_LL4F4: {struct Cyc_Absyn_Exp*_tmp9E2=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9B6);if(_tmp9E2 == _tmp9B6)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp139A;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp1399;return Cyc_Tcutil_copye(e,(
void*)((_tmp1399=_cycalloc(sizeof(*_tmp1399)),((_tmp1399[0]=((_tmp139A.tag=19,((
_tmp139A.f1=_tmp9E2,_tmp139A)))),_tmp1399)))));};}_LL4F5: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9B7=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp999;if(_tmp9B7->tag != 20)goto
_LL4F7;else{_tmp9B8=(void*)_tmp9B7->f1;_tmp9B9=(void*)_tmp9B7->f2;}}_LL4F6: {
void*_tmp9E5=Cyc_Tcutil_rsubstitute(r,inst,_tmp9B8);if(_tmp9E5 == _tmp9B8)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp139D;struct Cyc_Absyn_Offsetof_e_struct*
_tmp139C;return Cyc_Tcutil_copye(e,(void*)((_tmp139C=_cycalloc(sizeof(*_tmp139C)),((
_tmp139C[0]=((_tmp139D.tag=20,((_tmp139D.f1=(void*)_tmp9E5,((_tmp139D.f2=(void*)
_tmp9B9,_tmp139D)))))),_tmp139C)))));};}_LL4F7: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9BA=(struct Cyc_Absyn_Valueof_e_struct*)_tmp999;if(_tmp9BA->tag != 39)goto
_LL4F9;else{_tmp9BB=(void*)_tmp9BA->f1;}}_LL4F8: {void*_tmp9E8=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9BB);if(_tmp9E8 == _tmp9BB)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp13A0;struct Cyc_Absyn_Valueof_e_struct*_tmp139F;return Cyc_Tcutil_copye(e,(
void*)((_tmp139F=_cycalloc(sizeof(*_tmp139F)),((_tmp139F[0]=((_tmp13A0.tag=39,((
_tmp13A0.f1=(void*)_tmp9E8,_tmp13A0)))),_tmp139F)))));};}_LL4F9:;_LL4FA: {const
char*_tmp13A3;void*_tmp13A2;return(_tmp13A2=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A3="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp13A3,sizeof(char),46))),_tag_dyneither(_tmp13A2,sizeof(void*),
0)));}_LL4DC:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9EF;
struct Cyc_Absyn_AggrInfo _tmp9F1;union Cyc_Absyn_AggrInfoU _tmp9F2;struct Cyc_List_List*
_tmp9F3;struct Cyc_Absyn_DatatypeInfo _tmp9F5;union Cyc_Absyn_DatatypeInfoU _tmp9F6;
struct Cyc_List_List*_tmp9F7;struct Cyc_Absyn_DatatypeFieldInfo _tmp9F9;union Cyc_Absyn_DatatypeFieldInfoU
_tmp9FA;struct Cyc_List_List*_tmp9FB;struct _tuple2*_tmp9FD;struct Cyc_List_List*
_tmp9FE;struct Cyc_Absyn_Typedefdecl*_tmp9FF;void**_tmpA00;struct Cyc_Absyn_ArrayInfo
_tmpA02;void*_tmpA03;struct Cyc_Absyn_Tqual _tmpA04;struct Cyc_Absyn_Exp*_tmpA05;
union Cyc_Absyn_Constraint*_tmpA06;struct Cyc_Position_Segment*_tmpA07;struct Cyc_Absyn_PtrInfo
_tmpA09;void*_tmpA0A;struct Cyc_Absyn_Tqual _tmpA0B;struct Cyc_Absyn_PtrAtts _tmpA0C;
void*_tmpA0D;union Cyc_Absyn_Constraint*_tmpA0E;union Cyc_Absyn_Constraint*_tmpA0F;
union Cyc_Absyn_Constraint*_tmpA10;struct Cyc_Absyn_FnInfo _tmpA12;struct Cyc_List_List*
_tmpA13;struct Cyc_Core_Opt*_tmpA14;struct Cyc_Absyn_Tqual _tmpA15;void*_tmpA16;
struct Cyc_List_List*_tmpA17;int _tmpA18;struct Cyc_Absyn_VarargInfo*_tmpA19;struct
Cyc_List_List*_tmpA1A;struct Cyc_List_List*_tmpA1B;struct Cyc_List_List*_tmpA1D;
enum Cyc_Absyn_AggrKind _tmpA1F;struct Cyc_List_List*_tmpA20;struct Cyc_Core_Opt*
_tmpA22;void*_tmpA24;void*_tmpA26;void*_tmpA27;void*_tmpA29;struct Cyc_Absyn_Exp*
_tmpA2B;void*_tmpA35;void*_tmpA37;struct Cyc_List_List*_tmpA39;_LL4FC: {struct Cyc_Absyn_VarType_struct*
_tmp9EE=(struct Cyc_Absyn_VarType_struct*)_tmp9ED;if(_tmp9EE->tag != 2)goto _LL4FE;
else{_tmp9EF=_tmp9EE->f1;}}_LL4FD: {struct _handler_cons _tmpA3B;_push_handler(&
_tmpA3B);{int _tmpA3D=0;if(setjmp(_tmpA3B.handler))_tmpA3D=1;if(!_tmpA3D){{void*
_tmpA3E=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9EF);_npop_handler(0);return _tmpA3E;};_pop_handler();}else{void*_tmpA3C=(
void*)_exn_thrown;void*_tmpA40=_tmpA3C;_LL533: {struct Cyc_Core_Not_found_struct*
_tmpA41=(struct Cyc_Core_Not_found_struct*)_tmpA40;if(_tmpA41->tag != Cyc_Core_Not_found)
goto _LL535;}_LL534: return t;_LL535:;_LL536:(void)_throw(_tmpA40);_LL532:;}};}
_LL4FE: {struct Cyc_Absyn_AggrType_struct*_tmp9F0=(struct Cyc_Absyn_AggrType_struct*)
_tmp9ED;if(_tmp9F0->tag != 11)goto _LL500;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F1.aggr_info;
_tmp9F3=_tmp9F1.targs;}}_LL4FF: {struct Cyc_List_List*_tmpA42=Cyc_Tcutil_substs(
rgn,inst,_tmp9F3);struct Cyc_Absyn_AggrType_struct _tmp13A9;struct Cyc_Absyn_AggrInfo
_tmp13A8;struct Cyc_Absyn_AggrType_struct*_tmp13A7;return _tmpA42 == _tmp9F3?t:(
void*)((_tmp13A7=_cycalloc(sizeof(*_tmp13A7)),((_tmp13A7[0]=((_tmp13A9.tag=11,((
_tmp13A9.f1=((_tmp13A8.aggr_info=_tmp9F2,((_tmp13A8.targs=_tmpA42,_tmp13A8)))),
_tmp13A9)))),_tmp13A7))));}_LL500: {struct Cyc_Absyn_DatatypeType_struct*_tmp9F4=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp9ED;if(_tmp9F4->tag != 3)goto _LL502;
else{_tmp9F5=_tmp9F4->f1;_tmp9F6=_tmp9F5.datatype_info;_tmp9F7=_tmp9F5.targs;}}
_LL501: {struct Cyc_List_List*_tmpA46=Cyc_Tcutil_substs(rgn,inst,_tmp9F7);struct
Cyc_Absyn_DatatypeType_struct _tmp13AF;struct Cyc_Absyn_DatatypeInfo _tmp13AE;
struct Cyc_Absyn_DatatypeType_struct*_tmp13AD;return _tmpA46 == _tmp9F7?t:(void*)((
_tmp13AD=_cycalloc(sizeof(*_tmp13AD)),((_tmp13AD[0]=((_tmp13AF.tag=3,((_tmp13AF.f1=((
_tmp13AE.datatype_info=_tmp9F6,((_tmp13AE.targs=_tmpA46,_tmp13AE)))),_tmp13AF)))),
_tmp13AD))));}_LL502: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp9F8=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp9ED;if(_tmp9F8->tag != 4)goto _LL504;else{
_tmp9F9=_tmp9F8->f1;_tmp9FA=_tmp9F9.field_info;_tmp9FB=_tmp9F9.targs;}}_LL503: {
struct Cyc_List_List*_tmpA4A=Cyc_Tcutil_substs(rgn,inst,_tmp9FB);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp13B5;struct Cyc_Absyn_DatatypeFieldInfo _tmp13B4;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13B3;return _tmpA4A == _tmp9FB?t:(void*)((_tmp13B3=_cycalloc(sizeof(*_tmp13B3)),((
_tmp13B3[0]=((_tmp13B5.tag=4,((_tmp13B5.f1=((_tmp13B4.field_info=_tmp9FA,((
_tmp13B4.targs=_tmpA4A,_tmp13B4)))),_tmp13B5)))),_tmp13B3))));}_LL504: {struct
Cyc_Absyn_TypedefType_struct*_tmp9FC=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9ED;if(_tmp9FC->tag != 17)goto _LL506;else{_tmp9FD=_tmp9FC->f1;_tmp9FE=_tmp9FC->f2;
_tmp9FF=_tmp9FC->f3;_tmpA00=_tmp9FC->f4;}}_LL505: {struct Cyc_List_List*_tmpA4E=
Cyc_Tcutil_substs(rgn,inst,_tmp9FE);struct Cyc_Absyn_TypedefType_struct _tmp13B8;
struct Cyc_Absyn_TypedefType_struct*_tmp13B7;return _tmpA4E == _tmp9FE?t:(void*)((
_tmp13B7=_cycalloc(sizeof(*_tmp13B7)),((_tmp13B7[0]=((_tmp13B8.tag=17,((_tmp13B8.f1=
_tmp9FD,((_tmp13B8.f2=_tmpA4E,((_tmp13B8.f3=_tmp9FF,((_tmp13B8.f4=_tmpA00,
_tmp13B8)))))))))),_tmp13B7))));}_LL506: {struct Cyc_Absyn_ArrayType_struct*
_tmpA01=(struct Cyc_Absyn_ArrayType_struct*)_tmp9ED;if(_tmpA01->tag != 8)goto
_LL508;else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA02.elt_type;_tmpA04=_tmpA02.tq;
_tmpA05=_tmpA02.num_elts;_tmpA06=_tmpA02.zero_term;_tmpA07=_tmpA02.zt_loc;}}
_LL507: {void*_tmpA51=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA03);struct Cyc_Absyn_ArrayType_struct
_tmp13BE;struct Cyc_Absyn_ArrayInfo _tmp13BD;struct Cyc_Absyn_ArrayType_struct*
_tmp13BC;return _tmpA51 == _tmpA03?t:(void*)((_tmp13BC=_cycalloc(sizeof(*_tmp13BC)),((
_tmp13BC[0]=((_tmp13BE.tag=8,((_tmp13BE.f1=((_tmp13BD.elt_type=_tmpA51,((
_tmp13BD.tq=_tmpA04,((_tmp13BD.num_elts=_tmpA05,((_tmp13BD.zero_term=_tmpA06,((
_tmp13BD.zt_loc=_tmpA07,_tmp13BD)))))))))),_tmp13BE)))),_tmp13BC))));}_LL508: {
struct Cyc_Absyn_PointerType_struct*_tmpA08=(struct Cyc_Absyn_PointerType_struct*)
_tmp9ED;if(_tmpA08->tag != 5)goto _LL50A;else{_tmpA09=_tmpA08->f1;_tmpA0A=_tmpA09.elt_typ;
_tmpA0B=_tmpA09.elt_tq;_tmpA0C=_tmpA09.ptr_atts;_tmpA0D=_tmpA0C.rgn;_tmpA0E=
_tmpA0C.nullable;_tmpA0F=_tmpA0C.bounds;_tmpA10=_tmpA0C.zero_term;}}_LL509: {
void*_tmpA55=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA0A);void*_tmpA56=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA0D);union Cyc_Absyn_Constraint*_tmpA57=_tmpA0F;{void*_tmpA58=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA0F);struct Cyc_Absyn_Exp*_tmpA5A;_LL538: {struct Cyc_Absyn_Upper_b_struct*
_tmpA59=(struct Cyc_Absyn_Upper_b_struct*)_tmpA58;if(_tmpA59->tag != 1)goto _LL53A;
else{_tmpA5A=_tmpA59->f1;}}_LL539: {struct Cyc_Absyn_Exp*_tmpA5B=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA5A);if(_tmpA5B != _tmpA5A){struct Cyc_Absyn_Upper_b_struct _tmp13C1;
struct Cyc_Absyn_Upper_b_struct*_tmp13C0;_tmpA57=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13C0=_cycalloc(sizeof(*_tmp13C0)),((
_tmp13C0[0]=((_tmp13C1.tag=1,((_tmp13C1.f1=_tmpA5B,_tmp13C1)))),_tmp13C0)))));}
goto _LL537;}_LL53A:;_LL53B: goto _LL537;_LL537:;}if((_tmpA55 == _tmpA0A  && _tmpA56
== _tmpA0D) && _tmpA57 == _tmpA0F)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13CB;struct Cyc_Absyn_PtrAtts _tmp13CA;struct Cyc_Absyn_PtrInfo _tmp13C9;struct
Cyc_Absyn_PointerType_struct*_tmp13C8;return(void*)((_tmp13C8=_cycalloc(sizeof(*
_tmp13C8)),((_tmp13C8[0]=((_tmp13CB.tag=5,((_tmp13CB.f1=((_tmp13C9.elt_typ=
_tmpA55,((_tmp13C9.elt_tq=_tmpA0B,((_tmp13C9.ptr_atts=((_tmp13CA.rgn=_tmpA56,((
_tmp13CA.nullable=_tmpA0E,((_tmp13CA.bounds=_tmpA57,((_tmp13CA.zero_term=_tmpA10,((
_tmp13CA.ptrloc=0,_tmp13CA)))))))))),_tmp13C9)))))),_tmp13CB)))),_tmp13C8))));};}
_LL50A: {struct Cyc_Absyn_FnType_struct*_tmpA11=(struct Cyc_Absyn_FnType_struct*)
_tmp9ED;if(_tmpA11->tag != 9)goto _LL50C;else{_tmpA12=_tmpA11->f1;_tmpA13=_tmpA12.tvars;
_tmpA14=_tmpA12.effect;_tmpA15=_tmpA12.ret_tqual;_tmpA16=_tmpA12.ret_typ;_tmpA17=
_tmpA12.args;_tmpA18=_tmpA12.c_varargs;_tmpA19=_tmpA12.cyc_varargs;_tmpA1A=
_tmpA12.rgn_po;_tmpA1B=_tmpA12.attributes;}}_LL50B:{struct Cyc_List_List*_tmpA62=
_tmpA13;for(0;_tmpA62 != 0;_tmpA62=_tmpA62->tl){struct _tuple16*_tmp13D5;struct Cyc_Absyn_VarType_struct
_tmp13D4;struct Cyc_Absyn_VarType_struct*_tmp13D3;struct Cyc_List_List*_tmp13D2;
inst=((_tmp13D2=_region_malloc(rgn,sizeof(*_tmp13D2)),((_tmp13D2->hd=((_tmp13D5=
_region_malloc(rgn,sizeof(*_tmp13D5)),((_tmp13D5->f1=(struct Cyc_Absyn_Tvar*)
_tmpA62->hd,((_tmp13D5->f2=(void*)((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((
_tmp13D3[0]=((_tmp13D4.tag=2,((_tmp13D4.f1=(struct Cyc_Absyn_Tvar*)_tmpA62->hd,
_tmp13D4)))),_tmp13D3)))),_tmp13D5)))))),((_tmp13D2->tl=inst,_tmp13D2))))));}}{
struct Cyc_List_List*_tmpA68;struct Cyc_List_List*_tmpA69;struct _tuple1 _tmpA67=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple29*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,
_tmpA17));_tmpA68=_tmpA67.f1;_tmpA69=_tmpA67.f2;{struct Cyc_List_List*_tmpA6A=Cyc_Tcutil_substs(
rgn,inst,_tmpA69);struct Cyc_List_List*_tmpA6B=((struct Cyc_List_List*(*)(struct
_tuple10*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA68,_tmpA6A));struct Cyc_Core_Opt*
eff2;if(_tmpA14 == 0)eff2=0;else{void*_tmpA6C=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA14->v);if(_tmpA6C == (void*)_tmpA14->v)eff2=_tmpA14;else{struct Cyc_Core_Opt*
_tmp13D6;eff2=((_tmp13D6=_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6->v=_tmpA6C,
_tmp13D6))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA19 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA6F;struct Cyc_Absyn_Tqual _tmpA70;void*
_tmpA71;int _tmpA72;struct Cyc_Absyn_VarargInfo _tmpA6E=*_tmpA19;_tmpA6F=_tmpA6E.name;
_tmpA70=_tmpA6E.tq;_tmpA71=_tmpA6E.type;_tmpA72=_tmpA6E.inject;{void*_tmpA73=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA71);if(_tmpA73 == _tmpA71)cyc_varargs2=_tmpA19;else{struct Cyc_Absyn_VarargInfo*
_tmp13D7;cyc_varargs2=((_tmp13D7=_cycalloc(sizeof(*_tmp13D7)),((_tmp13D7->name=
_tmpA6F,((_tmp13D7->tq=_tmpA70,((_tmp13D7->type=_tmpA73,((_tmp13D7->inject=
_tmpA72,_tmp13D7))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA76;struct Cyc_List_List*_tmpA77;struct _tuple1 _tmpA75=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA1A);_tmpA76=_tmpA75.f1;_tmpA77=_tmpA75.f2;{struct Cyc_List_List*
_tmpA78=Cyc_Tcutil_substs(rgn,inst,_tmpA76);struct Cyc_List_List*_tmpA79=Cyc_Tcutil_substs(
rgn,inst,_tmpA77);if(_tmpA78 == _tmpA76  && _tmpA79 == _tmpA77)rgn_po2=_tmpA1A;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA78,_tmpA79);}{struct Cyc_Absyn_FnType_struct _tmp13DD;struct
Cyc_Absyn_FnInfo _tmp13DC;struct Cyc_Absyn_FnType_struct*_tmp13DB;return(void*)((
_tmp13DB=_cycalloc(sizeof(*_tmp13DB)),((_tmp13DB[0]=((_tmp13DD.tag=9,((_tmp13DD.f1=((
_tmp13DC.tvars=_tmpA13,((_tmp13DC.effect=eff2,((_tmp13DC.ret_tqual=_tmpA15,((
_tmp13DC.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA16),((_tmp13DC.args=
_tmpA6B,((_tmp13DC.c_varargs=_tmpA18,((_tmp13DC.cyc_varargs=cyc_varargs2,((
_tmp13DC.rgn_po=rgn_po2,((_tmp13DC.attributes=_tmpA1B,_tmp13DC)))))))))))))))))),
_tmp13DD)))),_tmp13DB))));};};};};};};_LL50C: {struct Cyc_Absyn_TupleType_struct*
_tmpA1C=(struct Cyc_Absyn_TupleType_struct*)_tmp9ED;if(_tmpA1C->tag != 10)goto
_LL50E;else{_tmpA1D=_tmpA1C->f1;}}_LL50D: {struct Cyc_List_List*_tmpA7D=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA1D);struct Cyc_List_List*_tmpA7E=
Cyc_Tcutil_substs(rgn,inst,_tmpA7D);if(_tmpA7E == _tmpA7D)return t;{struct Cyc_List_List*
_tmpA7F=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA1D,_tmpA7E);struct Cyc_Absyn_TupleType_struct _tmp13E0;struct Cyc_Absyn_TupleType_struct*
_tmp13DF;return(void*)((_tmp13DF=_cycalloc(sizeof(*_tmp13DF)),((_tmp13DF[0]=((
_tmp13E0.tag=10,((_tmp13E0.f1=_tmpA7F,_tmp13E0)))),_tmp13DF))));};}_LL50E: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA1E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9ED;if(_tmpA1E->tag != 12)goto _LL510;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1E->f2;}}
_LL50F: {struct Cyc_List_List*_tmpA82=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA20);struct Cyc_List_List*_tmpA83=Cyc_Tcutil_substs(
rgn,inst,_tmpA82);if(_tmpA83 == _tmpA82)return t;{struct Cyc_List_List*_tmpA84=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA20,_tmpA83);struct Cyc_Absyn_AnonAggrType_struct _tmp13E3;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13E2;return(void*)((_tmp13E2=_cycalloc(sizeof(*_tmp13E2)),((_tmp13E2[0]=((
_tmp13E3.tag=12,((_tmp13E3.f1=_tmpA1F,((_tmp13E3.f2=_tmpA84,_tmp13E3)))))),
_tmp13E2))));};}_LL510: {struct Cyc_Absyn_Evar_struct*_tmpA21=(struct Cyc_Absyn_Evar_struct*)
_tmp9ED;if(_tmpA21->tag != 1)goto _LL512;else{_tmpA22=_tmpA21->f2;}}_LL511: if(
_tmpA22 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA22->v);else{return
t;}_LL512: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA23=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9ED;if(_tmpA23->tag != 15)goto _LL514;else{_tmpA24=(void*)_tmpA23->f1;}}_LL513: {
void*_tmpA87=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA24);struct Cyc_Absyn_RgnHandleType_struct
_tmp13E6;struct Cyc_Absyn_RgnHandleType_struct*_tmp13E5;return _tmpA87 == _tmpA24?t:(
void*)((_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((_tmp13E5[0]=((_tmp13E6.tag=15,((
_tmp13E6.f1=(void*)_tmpA87,_tmp13E6)))),_tmp13E5))));}_LL514: {struct Cyc_Absyn_DynRgnType_struct*
_tmpA25=(struct Cyc_Absyn_DynRgnType_struct*)_tmp9ED;if(_tmpA25->tag != 16)goto
_LL516;else{_tmpA26=(void*)_tmpA25->f1;_tmpA27=(void*)_tmpA25->f2;}}_LL515: {
void*_tmpA8A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA26);void*_tmpA8B=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA27);struct Cyc_Absyn_DynRgnType_struct _tmp13E9;struct Cyc_Absyn_DynRgnType_struct*
_tmp13E8;return _tmpA8A == _tmpA26  && _tmpA8B == _tmpA27?t:(void*)((_tmp13E8=
_cycalloc(sizeof(*_tmp13E8)),((_tmp13E8[0]=((_tmp13E9.tag=16,((_tmp13E9.f1=(void*)
_tmpA8A,((_tmp13E9.f2=(void*)_tmpA8B,_tmp13E9)))))),_tmp13E8))));}_LL516: {
struct Cyc_Absyn_TagType_struct*_tmpA28=(struct Cyc_Absyn_TagType_struct*)_tmp9ED;
if(_tmpA28->tag != 19)goto _LL518;else{_tmpA29=(void*)_tmpA28->f1;}}_LL517: {void*
_tmpA8E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA29);struct Cyc_Absyn_TagType_struct
_tmp13EC;struct Cyc_Absyn_TagType_struct*_tmp13EB;return _tmpA8E == _tmpA29?t:(void*)((
_tmp13EB=_cycalloc(sizeof(*_tmp13EB)),((_tmp13EB[0]=((_tmp13EC.tag=19,((_tmp13EC.f1=(
void*)_tmpA8E,_tmp13EC)))),_tmp13EB))));}_LL518: {struct Cyc_Absyn_ValueofType_struct*
_tmpA2A=(struct Cyc_Absyn_ValueofType_struct*)_tmp9ED;if(_tmpA2A->tag != 18)goto
_LL51A;else{_tmpA2B=_tmpA2A->f1;}}_LL519: {struct Cyc_Absyn_Exp*_tmpA91=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA2B);struct Cyc_Absyn_ValueofType_struct _tmp13EF;struct Cyc_Absyn_ValueofType_struct*
_tmp13EE;return _tmpA91 == _tmpA2B?t:(void*)((_tmp13EE=_cycalloc(sizeof(*_tmp13EE)),((
_tmp13EE[0]=((_tmp13EF.tag=18,((_tmp13EF.f1=_tmpA91,_tmp13EF)))),_tmp13EE))));}
_LL51A: {struct Cyc_Absyn_EnumType_struct*_tmpA2C=(struct Cyc_Absyn_EnumType_struct*)
_tmp9ED;if(_tmpA2C->tag != 13)goto _LL51C;}_LL51B: goto _LL51D;_LL51C: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpA2D=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp9ED;if(_tmpA2D->tag != 14)goto
_LL51E;}_LL51D: goto _LL51F;_LL51E: {struct Cyc_Absyn_VoidType_struct*_tmpA2E=(
struct Cyc_Absyn_VoidType_struct*)_tmp9ED;if(_tmpA2E->tag != 0)goto _LL520;}_LL51F:
goto _LL521;_LL520: {struct Cyc_Absyn_IntType_struct*_tmpA2F=(struct Cyc_Absyn_IntType_struct*)
_tmp9ED;if(_tmpA2F->tag != 6)goto _LL522;}_LL521: goto _LL523;_LL522: {struct Cyc_Absyn_FloatType_struct*
_tmpA30=(struct Cyc_Absyn_FloatType_struct*)_tmp9ED;if(_tmpA30->tag != 7)goto
_LL524;}_LL523: goto _LL525;_LL524: {struct Cyc_Absyn_RefCntRgn_struct*_tmpA31=(
struct Cyc_Absyn_RefCntRgn_struct*)_tmp9ED;if(_tmpA31->tag != 22)goto _LL526;}
_LL525: goto _LL527;_LL526: {struct Cyc_Absyn_UniqueRgn_struct*_tmpA32=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp9ED;if(_tmpA32->tag != 21)goto _LL528;}_LL527: goto _LL529;_LL528: {struct Cyc_Absyn_HeapRgn_struct*
_tmpA33=(struct Cyc_Absyn_HeapRgn_struct*)_tmp9ED;if(_tmpA33->tag != 20)goto _LL52A;}
_LL529: return t;_LL52A: {struct Cyc_Absyn_RgnsEff_struct*_tmpA34=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9ED;if(_tmpA34->tag != 25)goto _LL52C;else{_tmpA35=(void*)_tmpA34->f1;}}_LL52B: {
void*_tmpA94=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA35);struct Cyc_Absyn_RgnsEff_struct
_tmp13F2;struct Cyc_Absyn_RgnsEff_struct*_tmp13F1;return _tmpA94 == _tmpA35?t:(void*)((
_tmp13F1=_cycalloc(sizeof(*_tmp13F1)),((_tmp13F1[0]=((_tmp13F2.tag=25,((_tmp13F2.f1=(
void*)_tmpA94,_tmp13F2)))),_tmp13F1))));}_LL52C: {struct Cyc_Absyn_AccessEff_struct*
_tmpA36=(struct Cyc_Absyn_AccessEff_struct*)_tmp9ED;if(_tmpA36->tag != 23)goto
_LL52E;else{_tmpA37=(void*)_tmpA36->f1;}}_LL52D: {void*_tmpA97=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA37);struct Cyc_Absyn_AccessEff_struct _tmp13F5;struct Cyc_Absyn_AccessEff_struct*
_tmp13F4;return _tmpA97 == _tmpA37?t:(void*)((_tmp13F4=_cycalloc(sizeof(*_tmp13F4)),((
_tmp13F4[0]=((_tmp13F5.tag=23,((_tmp13F5.f1=(void*)_tmpA97,_tmp13F5)))),_tmp13F4))));}
_LL52E: {struct Cyc_Absyn_JoinEff_struct*_tmpA38=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9ED;if(_tmpA38->tag != 24)goto _LL530;else{_tmpA39=_tmpA38->f1;}}_LL52F: {
struct Cyc_List_List*_tmpA9A=Cyc_Tcutil_substs(rgn,inst,_tmpA39);struct Cyc_Absyn_JoinEff_struct
_tmp13F8;struct Cyc_Absyn_JoinEff_struct*_tmp13F7;return _tmpA9A == _tmpA39?t:(void*)((
_tmp13F7=_cycalloc(sizeof(*_tmp13F7)),((_tmp13F7[0]=((_tmp13F8.tag=24,((_tmp13F8.f1=
_tmpA9A,_tmp13F8)))),_tmp13F7))));}_LL530: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpA3A=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp9ED;if(_tmpA3A->tag != 26)goto
_LL4FB;}_LL531: {const char*_tmp13FB;void*_tmp13FA;(_tmp13FA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FB="found typedecltype in rsubs",
_tag_dyneither(_tmp13FB,sizeof(char),28))),_tag_dyneither(_tmp13FA,sizeof(void*),
0)));}_LL4FB:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*
rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*
_tmpA9F=(void*)ts->hd;struct Cyc_List_List*_tmpAA0=ts->tl;void*_tmpAA1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA9F);struct Cyc_List_List*_tmpAA2=Cyc_Tcutil_substs(rgn,inst,_tmpAA0);
if(_tmpA9F == _tmpAA1  && _tmpAA0 == _tmpAA2)return ts;{struct Cyc_List_List*_tmp13FC;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp13FC=_cycalloc(sizeof(*
_tmp13FC)),((_tmp13FC->hd=_tmpAA1,((_tmp13FC->tl=_tmpAA2,_tmp13FC)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpAA4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp13FF;struct _tuple16*_tmp13FE;return(
_tmp13FE=_cycalloc(sizeof(*_tmp13FE)),((_tmp13FE->f1=tv,((_tmp13FE->f2=Cyc_Absyn_new_evar(
_tmpAA4,((_tmp13FF=_cycalloc(sizeof(*_tmp13FF)),((_tmp13FF->v=s,_tmp13FF))))),
_tmp13FE)))));}struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple17 _tmpAA8;struct Cyc_List_List*_tmpAA9;
struct _RegionHandle*_tmpAAA;struct _tuple17*_tmpAA7=env;_tmpAA8=*_tmpAA7;_tmpAA9=
_tmpAA8.f1;_tmpAAA=_tmpAA8.f2;{struct Cyc_Core_Opt*_tmpAAB=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1402;struct
_tuple16*_tmp1401;return(_tmp1401=_region_malloc(_tmpAAA,sizeof(*_tmp1401)),((
_tmp1401->f1=tv,((_tmp1401->f2=Cyc_Absyn_new_evar(_tmpAAB,((_tmp1402=_cycalloc(
sizeof(*_tmp1402)),((_tmp1402->v=_tmpAA9,_tmp1402))))),_tmp1401)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;
for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,
tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const char*_tmp1408;void*_tmp1407[3];
struct Cyc_String_pa_struct _tmp1406;struct Cyc_String_pa_struct _tmp1405;struct Cyc_String_pa_struct
_tmp1404;(_tmp1404.tag=0,((_tmp1404.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((_tmp1405.tag=0,((_tmp1405.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((
_tmp1406.tag=0,((_tmp1406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((
_tmp1407[0]=& _tmp1406,((_tmp1407[1]=& _tmp1405,((_tmp1407[2]=& _tmp1404,Cyc_Tcutil_terr(
loc,((_tmp1408="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp1408,sizeof(char),59))),_tag_dyneither(_tmp1407,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp140B;void*_tmp140A;(_tmp140A=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140B="same type variable has different identity!",
_tag_dyneither(_tmp140B,sizeof(char),43))),_tag_dyneither(_tmp140A,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp140C;return(_tmp140C=_cycalloc(sizeof(*_tmp140C)),((_tmp140C->hd=tv,((
_tmp140C->tl=tvs,_tmp140C)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp140F;void*_tmp140E;(_tmp140E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140F="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp140F,sizeof(char),39))),_tag_dyneither(_tmp140E,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAB8=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAB8->identity == - 1){const char*
_tmp1412;void*_tmp1411;(_tmp1411=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1412="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1412,sizeof(char),41))),_tag_dyneither(_tmp1411,sizeof(void*),
0)));}if(_tmpAB8->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1413;return(_tmp1413=_cycalloc(sizeof(*_tmp1413)),((_tmp1413->hd=tv,((
_tmp1413->tl=tvs,_tmp1413)))));};}struct _tuple30{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1416;void*_tmp1415;(_tmp1415=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1416="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1416,sizeof(char),44))),_tag_dyneither(_tmp1415,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple30
_tmpABF;struct Cyc_Absyn_Tvar*_tmpAC0;int _tmpAC1;int*_tmpAC2;struct _tuple30*
_tmpABE=(struct _tuple30*)tvs2->hd;_tmpABF=*_tmpABE;_tmpAC0=_tmpABF.f1;_tmpAC1=
_tmpABF.f2;_tmpAC2=(int*)&(*_tmpABE).f2;if(_tmpAC0->identity == - 1){const char*
_tmp1419;void*_tmp1418;(_tmp1418=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1419="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp1419,sizeof(char),46))),_tag_dyneither(_tmp1418,sizeof(void*),
0)));}if(_tmpAC0->identity == tv->identity){*_tmpAC2=*_tmpAC2  || b;return tvs;}}}{
struct _tuple30*_tmp141C;struct Cyc_List_List*_tmp141B;return(_tmp141B=
_region_malloc(r,sizeof(*_tmp141B)),((_tmp141B->hd=((_tmp141C=_region_malloc(r,
sizeof(*_tmp141C)),((_tmp141C->f1=tv,((_tmp141C->f2=b,_tmp141C)))))),((_tmp141B->tl=
tvs,_tmp141B)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1420;void*_tmp141F[1];struct Cyc_String_pa_struct _tmp141E;(_tmp141E.tag=0,((
_tmp141E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp141F[0]=& _tmp141E,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1420="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1420,sizeof(char),29))),_tag_dyneither(_tmp141F,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1421;return(_tmp1421=_cycalloc(sizeof(*
_tmp1421)),((_tmp1421->hd=tv,((_tmp1421->tl=tvs,_tmp1421)))));};}struct _tuple31{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpACB=Cyc_Tcutil_compress(
e);int _tmpACD;_LL53D: {struct Cyc_Absyn_Evar_struct*_tmpACC=(struct Cyc_Absyn_Evar_struct*)
_tmpACB;if(_tmpACC->tag != 1)goto _LL53F;else{_tmpACD=_tmpACC->f3;}}_LL53E:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple31 _tmpACF;void*
_tmpAD0;int _tmpAD1;int*_tmpAD2;struct _tuple31*_tmpACE=(struct _tuple31*)es2->hd;
_tmpACF=*_tmpACE;_tmpAD0=_tmpACF.f1;_tmpAD1=_tmpACF.f2;_tmpAD2=(int*)&(*_tmpACE).f2;{
void*_tmpAD3=Cyc_Tcutil_compress(_tmpAD0);int _tmpAD5;_LL542: {struct Cyc_Absyn_Evar_struct*
_tmpAD4=(struct Cyc_Absyn_Evar_struct*)_tmpAD3;if(_tmpAD4->tag != 1)goto _LL544;
else{_tmpAD5=_tmpAD4->f3;}}_LL543: if(_tmpACD == _tmpAD5){if(b != *_tmpAD2)*_tmpAD2=
1;return es;}goto _LL541;_LL544:;_LL545: goto _LL541;_LL541:;};}}{struct _tuple31*
_tmp1424;struct Cyc_List_List*_tmp1423;return(_tmp1423=_region_malloc(r,sizeof(*
_tmp1423)),((_tmp1423->hd=((_tmp1424=_region_malloc(r,sizeof(*_tmp1424)),((
_tmp1424->f1=e,((_tmp1424->f2=b,_tmp1424)))))),((_tmp1423->tl=es,_tmp1423)))));};
_LL53F:;_LL540: return es;_LL53C:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1425;r=((_tmp1425=_region_malloc(rgn,sizeof(*_tmp1425)),((
_tmp1425->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1425->tl=r,_tmp1425))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpADA;int _tmpADB;struct
_tuple30 _tmpAD9=*((struct _tuple30*)tvs->hd);_tmpADA=_tmpAD9.f1;_tmpADB=_tmpAD9.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpADA->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1426;res=((_tmp1426=_region_malloc(r,sizeof(*_tmp1426)),((
_tmp1426->hd=(struct _tuple30*)tvs->hd,((_tmp1426->tl=res,_tmp1426))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 
0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp142A;void*_tmp1429[1];struct Cyc_String_pa_struct _tmp1428;(_tmp1428.tag=
0,((_tmp1428.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1429[0]=&
_tmp1428,Cyc_Tcutil_terr(loc,((_tmp142A="bitfield %s does not have constant width",
_tag_dyneither(_tmp142A,sizeof(char),41))),_tag_dyneither(_tmp1429,sizeof(void*),
1)))))));}else{unsigned int _tmpAE1;int _tmpAE2;struct _tuple14 _tmpAE0=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAE1=_tmpAE0.f1;_tmpAE2=_tmpAE0.f2;if(!_tmpAE2){
const char*_tmp142D;void*_tmp142C;(_tmp142C=0,Cyc_Tcutil_terr(loc,((_tmp142D="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp142D,sizeof(char),45))),_tag_dyneither(_tmp142C,sizeof(void*),
0)));}w=_tmpAE1;}{void*_tmpAE5=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAE7;_LL547: {struct Cyc_Absyn_IntType_struct*_tmpAE6=(struct Cyc_Absyn_IntType_struct*)
_tmpAE5;if(_tmpAE6->tag != 6)goto _LL549;else{_tmpAE7=_tmpAE6->f2;}}_LL548: switch(
_tmpAE7){case Cyc_Absyn_Char_sz: _LL54B: if(w > 8){const char*_tmp1430;void*_tmp142F;(
_tmp142F=0,Cyc_Tcutil_terr(loc,((_tmp1430="bitfield larger than type",
_tag_dyneither(_tmp1430,sizeof(char),26))),_tag_dyneither(_tmp142F,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL54C: if(w > 16){const char*_tmp1433;void*
_tmp1432;(_tmp1432=0,Cyc_Tcutil_terr(loc,((_tmp1433="bitfield larger than type",
_tag_dyneither(_tmp1433,sizeof(char),26))),_tag_dyneither(_tmp1432,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL54D: goto _LL54E;case Cyc_Absyn_Int_sz: _LL54E:
if(w > 32){const char*_tmp1436;void*_tmp1435;(_tmp1435=0,Cyc_Tcutil_terr(loc,((
_tmp1436="bitfield larger than type",_tag_dyneither(_tmp1436,sizeof(char),26))),
_tag_dyneither(_tmp1435,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL54F: if(w > 64){const char*_tmp1439;void*_tmp1438;(_tmp1438=0,Cyc_Tcutil_terr(
loc,((_tmp1439="bitfield larger than type",_tag_dyneither(_tmp1439,sizeof(char),
26))),_tag_dyneither(_tmp1438,sizeof(void*),0)));}break;}goto _LL546;_LL549:;
_LL54A:{const char*_tmp143E;void*_tmp143D[2];struct Cyc_String_pa_struct _tmp143C;
struct Cyc_String_pa_struct _tmp143B;(_tmp143B.tag=0,((_tmp143B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp143C.tag=0,((_tmp143C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp143D[0]=& _tmp143C,((_tmp143D[1]=& _tmp143B,Cyc_Tcutil_terr(loc,((_tmp143E="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp143E,sizeof(char),52))),_tag_dyneither(_tmp143D,sizeof(void*),
2)))))))))))));}goto _LL546;_LL546:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmpAF4=(void*)atts->hd;_LL552: {struct Cyc_Absyn_Packed_att_struct*
_tmpAF5=(struct Cyc_Absyn_Packed_att_struct*)_tmpAF4;if(_tmpAF5->tag != 7)goto
_LL554;}_LL553: continue;_LL554: {struct Cyc_Absyn_Aligned_att_struct*_tmpAF6=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpAF4;if(_tmpAF6->tag != 6)goto _LL556;}
_LL555: continue;_LL556:;_LL557: {const char*_tmp1443;void*_tmp1442[2];struct Cyc_String_pa_struct
_tmp1441;struct Cyc_String_pa_struct _tmp1440;(_tmp1440.tag=0,((_tmp1440.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1441.tag=0,((_tmp1441.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1442[0]=& _tmp1441,((_tmp1442[1]=& _tmp1440,Cyc_Tcutil_terr(loc,((
_tmp1443="bad attribute %s on member %s",_tag_dyneither(_tmp1443,sizeof(char),30))),
_tag_dyneither(_tmp1442,sizeof(void*),2)))))))))))));}_LL551:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};typedef struct
Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmpAFB=t;struct
Cyc_Absyn_Typedefdecl*_tmpAFD;void**_tmpAFE;_LL559: {struct Cyc_Absyn_TypedefType_struct*
_tmpAFC=(struct Cyc_Absyn_TypedefType_struct*)_tmpAFB;if(_tmpAFC->tag != 17)goto
_LL55B;else{_tmpAFD=_tmpAFC->f3;_tmpAFE=_tmpAFC->f4;}}_LL55A: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpAFD))->tq).real_const  || (_tmpAFD->tq).print_const){if(
declared_const){const char*_tmp1446;void*_tmp1445;(_tmp1445=0,Cyc_Tcutil_warn(loc,((
_tmp1446="extra const",_tag_dyneither(_tmp1446,sizeof(char),12))),_tag_dyneither(
_tmp1445,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpAFE)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpAFE);else{return declared_const;}_LL55B:;_LL55C: return
declared_const;_LL558:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpB01=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpB03;struct Cyc_Absyn_PtrAtts _tmpB04;void*_tmpB05;
_LL55E: {struct Cyc_Absyn_PointerType_struct*_tmpB02=(struct Cyc_Absyn_PointerType_struct*)
_tmpB01;if(_tmpB02->tag != 5)goto _LL560;else{_tmpB03=_tmpB02->f1;_tmpB04=_tmpB03.ptr_atts;
_tmpB05=_tmpB04.rgn;}}_LL55F:{void*_tmpB06=Cyc_Tcutil_compress(_tmpB05);struct
Cyc_Absyn_Tvar*_tmpB08;_LL563: {struct Cyc_Absyn_VarType_struct*_tmpB07=(struct
Cyc_Absyn_VarType_struct*)_tmpB06;if(_tmpB07->tag != 2)goto _LL565;else{_tmpB08=
_tmpB07->f1;}}_LL564: return Cyc_Absyn_tvar_cmp(tvar,_tmpB08)== 0;_LL565:;_LL566:
goto _LL562;_LL562:;}goto _LL55D;_LL560:;_LL561: goto _LL55D;_LL55D:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpB09=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB0B;
struct Cyc_Absyn_Kind _tmpB0C;enum Cyc_Absyn_KindQual _tmpB0D;enum Cyc_Absyn_AliasQual
_tmpB0E;struct Cyc_Absyn_Kind*_tmpB10;struct Cyc_Absyn_Kind _tmpB11;enum Cyc_Absyn_KindQual
_tmpB12;enum Cyc_Absyn_AliasQual _tmpB13;_LL568: {struct Cyc_Absyn_Less_kb_struct*
_tmpB0A=(struct Cyc_Absyn_Less_kb_struct*)_tmpB09;if(_tmpB0A->tag != 2)goto _LL56A;
else{_tmpB0B=_tmpB0A->f2;_tmpB0C=*_tmpB0B;_tmpB0D=_tmpB0C.kind;if(_tmpB0D != Cyc_Absyn_RgnKind)
goto _LL56A;_tmpB0E=_tmpB0C.aliasqual;if(_tmpB0E != Cyc_Absyn_Top)goto _LL56A;}}
_LL569: goto _LL56B;_LL56A: {struct Cyc_Absyn_Eq_kb_struct*_tmpB0F=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB09;if(_tmpB0F->tag != 0)goto _LL56C;else{_tmpB10=_tmpB0F->f1;_tmpB11=*_tmpB10;
_tmpB12=_tmpB11.kind;if(_tmpB12 != Cyc_Absyn_RgnKind)goto _LL56C;_tmpB13=_tmpB11.aliasqual;
if(_tmpB13 != Cyc_Absyn_Top)goto _LL56C;}}_LL56B: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL56C:;_LL56D: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL567:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple32{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int
put_in_effect){static struct Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
static struct Cyc_Core_Opt hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*
_tmpB14=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB17;struct Cyc_Core_Opt**
_tmpB18;struct Cyc_Core_Opt*_tmpB19;struct Cyc_Core_Opt**_tmpB1A;struct Cyc_Absyn_Tvar*
_tmpB1C;struct Cyc_Absyn_TypeDecl*_tmpB1E;struct Cyc_Absyn_TypeDecl _tmpB1F;void*
_tmpB20;void**_tmpB21;void***_tmpB22;struct Cyc_List_List*_tmpB24;struct _tuple2*
_tmpB26;struct Cyc_Absyn_Enumdecl*_tmpB27;struct Cyc_Absyn_Enumdecl**_tmpB28;
struct Cyc_Absyn_DatatypeInfo _tmpB2A;union Cyc_Absyn_DatatypeInfoU _tmpB2B;union Cyc_Absyn_DatatypeInfoU*
_tmpB2C;struct Cyc_List_List*_tmpB2D;struct Cyc_List_List**_tmpB2E;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB30;union Cyc_Absyn_DatatypeFieldInfoU _tmpB31;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB32;struct Cyc_List_List*_tmpB33;struct Cyc_Absyn_PtrInfo _tmpB35;void*_tmpB36;
struct Cyc_Absyn_Tqual _tmpB37;struct Cyc_Absyn_Tqual*_tmpB38;struct Cyc_Absyn_PtrAtts
_tmpB39;void*_tmpB3A;union Cyc_Absyn_Constraint*_tmpB3B;union Cyc_Absyn_Constraint*
_tmpB3C;union Cyc_Absyn_Constraint*_tmpB3D;void*_tmpB3F;struct Cyc_Absyn_Exp*
_tmpB41;struct Cyc_Absyn_ArrayInfo _tmpB45;void*_tmpB46;struct Cyc_Absyn_Tqual
_tmpB47;struct Cyc_Absyn_Tqual*_tmpB48;struct Cyc_Absyn_Exp*_tmpB49;struct Cyc_Absyn_Exp**
_tmpB4A;union Cyc_Absyn_Constraint*_tmpB4B;struct Cyc_Position_Segment*_tmpB4C;
struct Cyc_Absyn_FnInfo _tmpB4E;struct Cyc_List_List*_tmpB4F;struct Cyc_List_List**
_tmpB50;struct Cyc_Core_Opt*_tmpB51;struct Cyc_Core_Opt**_tmpB52;struct Cyc_Absyn_Tqual
_tmpB53;struct Cyc_Absyn_Tqual*_tmpB54;void*_tmpB55;struct Cyc_List_List*_tmpB56;
int _tmpB57;struct Cyc_Absyn_VarargInfo*_tmpB58;struct Cyc_List_List*_tmpB59;struct
Cyc_List_List*_tmpB5A;struct Cyc_List_List*_tmpB5C;enum Cyc_Absyn_AggrKind _tmpB5E;
struct Cyc_List_List*_tmpB5F;struct Cyc_Absyn_AggrInfo _tmpB61;union Cyc_Absyn_AggrInfoU
_tmpB62;union Cyc_Absyn_AggrInfoU*_tmpB63;struct Cyc_List_List*_tmpB64;struct Cyc_List_List**
_tmpB65;struct _tuple2*_tmpB67;struct Cyc_List_List*_tmpB68;struct Cyc_List_List**
_tmpB69;struct Cyc_Absyn_Typedefdecl*_tmpB6A;struct Cyc_Absyn_Typedefdecl**_tmpB6B;
void**_tmpB6C;void***_tmpB6D;void*_tmpB72;void*_tmpB74;void*_tmpB75;void*_tmpB77;
void*_tmpB79;struct Cyc_List_List*_tmpB7B;_LL56F: {struct Cyc_Absyn_VoidType_struct*
_tmpB15=(struct Cyc_Absyn_VoidType_struct*)_tmpB14;if(_tmpB15->tag != 0)goto _LL571;}
_LL570: goto _LL56E;_LL571: {struct Cyc_Absyn_Evar_struct*_tmpB16=(struct Cyc_Absyn_Evar_struct*)
_tmpB14;if(_tmpB16->tag != 1)goto _LL573;else{_tmpB17=_tmpB16->f1;_tmpB18=(struct
Cyc_Core_Opt**)& _tmpB16->f1;_tmpB19=_tmpB16->f2;_tmpB1A=(struct Cyc_Core_Opt**)&
_tmpB16->f2;}}_LL572: if(*_tmpB18 == 0  || Cyc_Tcutil_kind_leq(expected_kind,(
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v) && !Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmpB18))->v,
expected_kind))*_tmpB18=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result
 && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){if(
expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB1A=(struct Cyc_Core_Opt*)& urgn;
else{*_tmpB1A=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmp1449;struct Cyc_Absyn_Less_kb_struct*_tmp1448;
struct Cyc_Absyn_Tvar*_tmpB7C=Cyc_Tcutil_new_tvar((void*)((_tmp1448=_cycalloc(
sizeof(*_tmp1448)),((_tmp1448[0]=((_tmp1449.tag=2,((_tmp1449.f1=0,((_tmp1449.f2=
expected_kind,_tmp1449)))))),_tmp1448)))));{struct Cyc_Absyn_VarType_struct*
_tmp144F;struct Cyc_Absyn_VarType_struct _tmp144E;struct Cyc_Core_Opt*_tmp144D;*
_tmpB1A=((_tmp144D=_cycalloc(sizeof(*_tmp144D)),((_tmp144D->v=(void*)((_tmp144F=
_cycalloc(sizeof(*_tmp144F)),((_tmp144F[0]=((_tmp144E.tag=2,((_tmp144E.f1=
_tmpB7C,_tmp144E)))),_tmp144F)))),_tmp144D))));}_tmpB1C=_tmpB7C;goto _LL574;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL56E;_LL573: {struct Cyc_Absyn_VarType_struct*_tmpB1B=(
struct Cyc_Absyn_VarType_struct*)_tmpB14;if(_tmpB1B->tag != 2)goto _LL575;else{
_tmpB1C=_tmpB1B->f1;}}_LL574:{void*_tmpB82=Cyc_Absyn_compress_kb(_tmpB1C->kind);
struct Cyc_Core_Opt*_tmpB84;struct Cyc_Core_Opt**_tmpB85;_LL5A6: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB83=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB82;if(_tmpB83->tag != 1)goto
_LL5A8;else{_tmpB84=_tmpB83->f1;_tmpB85=(struct Cyc_Core_Opt**)& _tmpB83->f1;}}
_LL5A7:{struct Cyc_Absyn_Less_kb_struct*_tmp1455;struct Cyc_Absyn_Less_kb_struct
_tmp1454;struct Cyc_Core_Opt*_tmp1453;*_tmpB85=((_tmp1453=_cycalloc(sizeof(*
_tmp1453)),((_tmp1453->v=(void*)((_tmp1455=_cycalloc(sizeof(*_tmp1455)),((
_tmp1455[0]=((_tmp1454.tag=2,((_tmp1454.f1=0,((_tmp1454.f2=expected_kind,
_tmp1454)))))),_tmp1455)))),_tmp1453))));}goto _LL5A5;_LL5A8:;_LL5A9: goto _LL5A5;
_LL5A5:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB1C);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB1C,put_in_effect);{void*_tmpB89=Cyc_Absyn_compress_kb(_tmpB1C->kind);struct
Cyc_Core_Opt*_tmpB8B;struct Cyc_Core_Opt**_tmpB8C;struct Cyc_Absyn_Kind*_tmpB8D;
_LL5AB: {struct Cyc_Absyn_Less_kb_struct*_tmpB8A=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB89;if(_tmpB8A->tag != 2)goto _LL5AD;else{_tmpB8B=_tmpB8A->f1;_tmpB8C=(struct
Cyc_Core_Opt**)& _tmpB8A->f1;_tmpB8D=_tmpB8A->f2;}}_LL5AC: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB8D)){struct Cyc_Absyn_Less_kb_struct*_tmp145B;struct Cyc_Absyn_Less_kb_struct
_tmp145A;struct Cyc_Core_Opt*_tmp1459;*_tmpB8C=((_tmp1459=_cycalloc(sizeof(*
_tmp1459)),((_tmp1459->v=(void*)((_tmp145B=_cycalloc(sizeof(*_tmp145B)),((
_tmp145B[0]=((_tmp145A.tag=2,((_tmp145A.f1=0,((_tmp145A.f2=expected_kind,
_tmp145A)))))),_tmp145B)))),_tmp1459))));}goto _LL5AA;_LL5AD:;_LL5AE: goto _LL5AA;
_LL5AA:;}goto _LL56E;_LL575: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB1D=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB14;if(_tmpB1D->tag != 26)goto _LL577;else{
_tmpB1E=_tmpB1D->f1;_tmpB1F=*_tmpB1E;_tmpB20=_tmpB1F.r;_tmpB21=_tmpB1D->f2;
_tmpB22=(void***)& _tmpB1D->f2;}}_LL576: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpB91=_tmpB20;struct Cyc_Absyn_Aggrdecl*_tmpB93;struct Cyc_Absyn_Enumdecl*
_tmpB95;struct Cyc_Absyn_Datatypedecl*_tmpB97;_LL5B0: {struct Cyc_Absyn_Aggr_td_struct*
_tmpB92=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB91;if(_tmpB92->tag != 0)goto _LL5B2;
else{_tmpB93=_tmpB92->f1;}}_LL5B1: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB93);goto _LL5AF;_LL5B2: {struct Cyc_Absyn_Enum_td_struct*_tmpB94=(struct Cyc_Absyn_Enum_td_struct*)
_tmpB91;if(_tmpB94->tag != 1)goto _LL5B4;else{_tmpB95=_tmpB94->f1;}}_LL5B3: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpB95);goto _LL5AF;_LL5B4: {struct Cyc_Absyn_Datatype_td_struct*
_tmpB96=(struct Cyc_Absyn_Datatype_td_struct*)_tmpB91;if(_tmpB96->tag != 2)goto
_LL5AF;else{_tmpB97=_tmpB96->f1;}}_LL5B5: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpB97);goto _LL5AF;_LL5AF:;}{void**_tmp145C;*_tmpB22=((_tmp145C=_cycalloc(
sizeof(*_tmp145C)),((_tmp145C[0]=new_t,_tmp145C))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL577: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB23=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB14;if(_tmpB23->tag != 14)goto
_LL579;else{_tmpB24=_tmpB23->f1;}}_LL578: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmpB99=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmpB24 != 0;_tmpB24=_tmpB24->tl){
struct Cyc_Absyn_Enumfield*_tmpB9A=(struct Cyc_Absyn_Enumfield*)_tmpB24->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpB9A->name).f2)){
const char*_tmp1460;void*_tmp145F[1];struct Cyc_String_pa_struct _tmp145E;(_tmp145E.tag=
0,((_tmp145E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB9A->name).f2),((
_tmp145F[0]=& _tmp145E,Cyc_Tcutil_terr(_tmpB9A->loc,((_tmp1460="duplicate enum field name %s",
_tag_dyneither(_tmp1460,sizeof(char),29))),_tag_dyneither(_tmp145F,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1461;prev_fields=((_tmp1461=
_region_malloc(_tmpB99,sizeof(*_tmp1461)),((_tmp1461->hd=(*_tmpB9A->name).f2,((
_tmp1461->tl=prev_fields,_tmp1461))))));}if(_tmpB9A->tag == 0)_tmpB9A->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpB9A->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpB9A->tag))){const char*_tmp1465;void*
_tmp1464[1];struct Cyc_String_pa_struct _tmp1463;(_tmp1463.tag=0,((_tmp1463.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpB9A->name).f2),((_tmp1464[0]=&
_tmp1463,Cyc_Tcutil_terr(loc,((_tmp1465="enum field %s: expression is not constant",
_tag_dyneither(_tmp1465,sizeof(char),42))),_tag_dyneither(_tmp1464,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpB9A->tag))).f1;tag_count=t1 + 1;(*_tmpB9A->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp146B;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp146A;struct _tuple31*_tmp1469;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple31*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpB9A->name).f2,(struct _tuple31*)((_tmp1469=_cycalloc(sizeof(*_tmp1469)),((
_tmp1469->f1=(void*)((_tmp146B=_cycalloc(sizeof(*_tmp146B)),((_tmp146B[0]=((
_tmp146A.tag=4,((_tmp146A.f1=(void*)t,((_tmp146A.f2=_tmpB9A,_tmp146A)))))),
_tmp146B)))),((_tmp1469->f2=1,_tmp1469)))))));};};}}goto _LL56E;}_LL579: {struct
Cyc_Absyn_EnumType_struct*_tmpB25=(struct Cyc_Absyn_EnumType_struct*)_tmpB14;if(
_tmpB25->tag != 13)goto _LL57B;else{_tmpB26=_tmpB25->f1;_tmpB27=_tmpB25->f2;
_tmpB28=(struct Cyc_Absyn_Enumdecl**)& _tmpB25->f2;}}_LL57A: if(*_tmpB28 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB28))->fields == 0){struct
_handler_cons _tmpBA5;_push_handler(& _tmpBA5);{int _tmpBA7=0;if(setjmp(_tmpBA5.handler))
_tmpBA7=1;if(!_tmpBA7){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB26);*_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpBA6=(void*)_exn_thrown;void*_tmpBA9=_tmpBA6;_LL5B7: {struct Cyc_Dict_Absent_struct*
_tmpBAA=(struct Cyc_Dict_Absent_struct*)_tmpBA9;if(_tmpBAA->tag != Cyc_Dict_Absent)
goto _LL5B9;}_LL5B8: {struct Cyc_Tcenv_Genv*_tmpBAB=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp146C;struct Cyc_Absyn_Enumdecl*_tmpBAC=(_tmp146C=
_cycalloc(sizeof(*_tmp146C)),((_tmp146C->sc=Cyc_Absyn_Extern,((_tmp146C->name=
_tmpB26,((_tmp146C->fields=0,_tmp146C)))))));Cyc_Tc_tcEnumdecl(te,_tmpBAB,loc,
_tmpBAC);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB26);*
_tmpB28=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5B6;};}_LL5B9:;_LL5BA:(void)_throw(
_tmpBA9);_LL5B6:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB28);*_tmpB26=(ed->name)[0];goto _LL56E;};_LL57B: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB29=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB14;if(_tmpB29->tag != 3)goto
_LL57D;else{_tmpB2A=_tmpB29->f1;_tmpB2B=_tmpB2A.datatype_info;_tmpB2C=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB29->f1).datatype_info;_tmpB2D=_tmpB2A.targs;_tmpB2E=(struct Cyc_List_List**)&(
_tmpB29->f1).targs;}}_LL57C: {struct Cyc_List_List*_tmpBAE=*_tmpB2E;{union Cyc_Absyn_DatatypeInfoU
_tmpBAF=*_tmpB2C;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBB0;struct _tuple2*
_tmpBB1;int _tmpBB2;struct Cyc_Absyn_Datatypedecl**_tmpBB3;struct Cyc_Absyn_Datatypedecl*
_tmpBB4;_LL5BC: if((_tmpBAF.UnknownDatatype).tag != 1)goto _LL5BE;_tmpBB0=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBAF.UnknownDatatype).val;_tmpBB1=_tmpBB0.name;
_tmpBB2=_tmpBB0.is_extensible;_LL5BD: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBB5;_push_handler(& _tmpBB5);{int _tmpBB7=0;if(setjmp(
_tmpBB5.handler))_tmpBB7=1;if(!_tmpBB7){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBB1);;_pop_handler();}else{void*_tmpBB6=(void*)_exn_thrown;void*_tmpBB9=
_tmpBB6;_LL5C1: {struct Cyc_Dict_Absent_struct*_tmpBBA=(struct Cyc_Dict_Absent_struct*)
_tmpBB9;if(_tmpBBA->tag != Cyc_Dict_Absent)goto _LL5C3;}_LL5C2: {struct Cyc_Tcenv_Genv*
_tmpBBB=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp146D;struct Cyc_Absyn_Datatypedecl*
_tmpBBC=(_tmp146D=_cycalloc(sizeof(*_tmp146D)),((_tmp146D->sc=Cyc_Absyn_Extern,((
_tmp146D->name=_tmpBB1,((_tmp146D->tvs=0,((_tmp146D->fields=0,((_tmp146D->is_extensible=
_tmpBB2,_tmp146D)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBBB,loc,_tmpBBC);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBB1);if(_tmpBAE != 0){{const char*
_tmp1471;void*_tmp1470[1];struct Cyc_String_pa_struct _tmp146F;(_tmp146F.tag=0,((
_tmp146F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBB1)),((_tmp1470[0]=& _tmp146F,Cyc_Tcutil_terr(loc,((_tmp1471="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1471,sizeof(char),47))),_tag_dyneither(_tmp1470,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5C0;}_LL5C3:;_LL5C4:(void)_throw(_tmpBB9);_LL5C0:;}};}
if(_tmpBB2  && !(*tudp)->is_extensible){const char*_tmp1475;void*_tmp1474[1];
struct Cyc_String_pa_struct _tmp1473;(_tmp1473.tag=0,((_tmp1473.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBB1)),((
_tmp1474[0]=& _tmp1473,Cyc_Tcutil_terr(loc,((_tmp1475="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1475,sizeof(char),41))),_tag_dyneither(_tmp1474,sizeof(void*),
1)))))));}*_tmpB2C=Cyc_Absyn_KnownDatatype(tudp);_tmpBB4=*tudp;goto _LL5BF;}
_LL5BE: if((_tmpBAF.KnownDatatype).tag != 2)goto _LL5BB;_tmpBB3=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBAF.KnownDatatype).val;_tmpBB4=*_tmpBB3;_LL5BF: {struct Cyc_List_List*tvs=
_tmpBB4->tvs;for(0;_tmpBAE != 0  && tvs != 0;(_tmpBAE=_tmpBAE->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBAE->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1476;struct _tuple0 _tmpBC5=(_tmp1476.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1476.f2=t,_tmp1476)));void*_tmpBC6;void*_tmpBC8;struct Cyc_Absyn_Tvar*_tmpBCA;
_LL5C6: _tmpBC6=_tmpBC5.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBC7=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBC6;if(_tmpBC7->tag != 1)goto _LL5C8;};_tmpBC8=_tmpBC5.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBC9=(struct Cyc_Absyn_VarType_struct*)_tmpBC8;if(_tmpBC9->tag != 2)goto _LL5C8;
else{_tmpBCA=_tmpBC9->f1;}};_LL5C7: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBCA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBCA,1);continue;_LL5C8:;_LL5C9: goto _LL5C5;_LL5C5:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBAE != 0){const
char*_tmp147A;void*_tmp1479[1];struct Cyc_String_pa_struct _tmp1478;(_tmp1478.tag=
0,((_tmp1478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBB4->name)),((_tmp1479[0]=& _tmp1478,Cyc_Tcutil_terr(loc,((_tmp147A="too many type arguments for datatype %s",
_tag_dyneither(_tmp147A,sizeof(char),40))),_tag_dyneither(_tmp1479,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp147B;hidden_ts=((_tmp147B=_cycalloc(sizeof(*_tmp147B)),((_tmp147B->hd=e,((
_tmp147B->tl=hidden_ts,_tmp147B))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB2E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB2E,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5BB;}_LL5BB:;}goto
_LL56E;}_LL57D: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB2F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB14;if(_tmpB2F->tag != 4)goto _LL57F;else{_tmpB30=_tmpB2F->f1;_tmpB31=_tmpB30.field_info;
_tmpB32=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB2F->f1).field_info;_tmpB33=
_tmpB30.targs;}}_LL57E:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBCF=*_tmpB32;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBD0;struct _tuple2*_tmpBD1;struct _tuple2*
_tmpBD2;int _tmpBD3;struct _tuple3 _tmpBD4;struct Cyc_Absyn_Datatypedecl*_tmpBD5;
struct Cyc_Absyn_Datatypefield*_tmpBD6;_LL5CB: if((_tmpBCF.UnknownDatatypefield).tag
!= 1)goto _LL5CD;_tmpBD0=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBCF.UnknownDatatypefield).val;
_tmpBD1=_tmpBD0.datatype_name;_tmpBD2=_tmpBD0.field_name;_tmpBD3=_tmpBD0.is_extensible;
_LL5CC: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBD7;_push_handler(& _tmpBD7);{int _tmpBD9=0;if(setjmp(
_tmpBD7.handler))_tmpBD9=1;if(!_tmpBD9){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBD1);;_pop_handler();}else{void*_tmpBD8=(void*)_exn_thrown;void*_tmpBDB=
_tmpBD8;_LL5D0: {struct Cyc_Dict_Absent_struct*_tmpBDC=(struct Cyc_Dict_Absent_struct*)
_tmpBDB;if(_tmpBDC->tag != Cyc_Dict_Absent)goto _LL5D2;}_LL5D1:{const char*_tmp147F;
void*_tmp147E[1];struct Cyc_String_pa_struct _tmp147D;(_tmp147D.tag=0,((_tmp147D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD1)),((
_tmp147E[0]=& _tmp147D,Cyc_Tcutil_terr(loc,((_tmp147F="unbound datatype %s",
_tag_dyneither(_tmp147F,sizeof(char),20))),_tag_dyneither(_tmp147E,sizeof(void*),
1)))))));}return cvtenv;_LL5D2:;_LL5D3:(void)_throw(_tmpBDB);_LL5CF:;}};}{struct
_handler_cons _tmpBE0;_push_handler(& _tmpBE0);{int _tmpBE2=0;if(setjmp(_tmpBE0.handler))
_tmpBE2=1;if(!_tmpBE2){{struct _RegionHandle*_tmpBE3=Cyc_Tcenv_get_fnrgn(te);void*
_tmpBE4=Cyc_Tcenv_lookup_ordinary(_tmpBE3,te,loc,_tmpBD2);struct Cyc_Absyn_Datatypedecl*
_tmpBE6;struct Cyc_Absyn_Datatypefield*_tmpBE7;_LL5D5: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpBE5=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpBE4;if(_tmpBE5->tag != 2)goto
_LL5D7;else{_tmpBE6=_tmpBE5->f1;_tmpBE7=_tmpBE5->f2;}}_LL5D6: tuf=_tmpBE7;tud=
_tmpBE6;if(_tmpBD3  && !tud->is_extensible){const char*_tmp1483;void*_tmp1482[1];
struct Cyc_String_pa_struct _tmp1481;(_tmp1481.tag=0,((_tmp1481.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD1)),((
_tmp1482[0]=& _tmp1481,Cyc_Tcutil_terr(loc,((_tmp1483="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1483,sizeof(char),41))),_tag_dyneither(_tmp1482,sizeof(void*),
1)))))));}goto _LL5D4;_LL5D7:;_LL5D8:{const char*_tmp1488;void*_tmp1487[2];struct
Cyc_String_pa_struct _tmp1486;struct Cyc_String_pa_struct _tmp1485;(_tmp1485.tag=0,((
_tmp1485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD1)),((_tmp1486.tag=0,((_tmp1486.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD2)),((_tmp1487[0]=& _tmp1486,((
_tmp1487[1]=& _tmp1485,Cyc_Tcutil_terr(loc,((_tmp1488="unbound field %s in type datatype %s",
_tag_dyneither(_tmp1488,sizeof(char),37))),_tag_dyneither(_tmp1487,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBEF=cvtenv;_npop_handler(0);return
_tmpBEF;};_LL5D4:;};_pop_handler();}else{void*_tmpBE1=(void*)_exn_thrown;void*
_tmpBF1=_tmpBE1;_LL5DA: {struct Cyc_Dict_Absent_struct*_tmpBF2=(struct Cyc_Dict_Absent_struct*)
_tmpBF1;if(_tmpBF2->tag != Cyc_Dict_Absent)goto _LL5DC;}_LL5DB:{const char*_tmp148D;
void*_tmp148C[2];struct Cyc_String_pa_struct _tmp148B;struct Cyc_String_pa_struct
_tmp148A;(_tmp148A.tag=0,((_tmp148A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD1)),((_tmp148B.tag=0,((_tmp148B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD2)),((
_tmp148C[0]=& _tmp148B,((_tmp148C[1]=& _tmp148A,Cyc_Tcutil_terr(loc,((_tmp148D="unbound field %s in type datatype %s",
_tag_dyneither(_tmp148D,sizeof(char),37))),_tag_dyneither(_tmp148C,sizeof(void*),
2)))))))))))));}return cvtenv;_LL5DC:;_LL5DD:(void)_throw(_tmpBF1);_LL5D9:;}};}*
_tmpB32=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpBD5=tud;_tmpBD6=tuf;goto _LL5CE;}
_LL5CD: if((_tmpBCF.KnownDatatypefield).tag != 2)goto _LL5CA;_tmpBD4=(struct _tuple3)(
_tmpBCF.KnownDatatypefield).val;_tmpBD5=_tmpBD4.f1;_tmpBD6=_tmpBD4.f2;_LL5CE: {
struct Cyc_List_List*tvs=_tmpBD5->tvs;for(0;_tmpB33 != 0  && tvs != 0;(_tmpB33=
_tmpB33->tl,tvs=tvs->tl)){void*t=(void*)_tmpB33->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp148E;struct _tuple0 _tmpBF8=(
_tmp148E.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp148E.f2=t,_tmp148E)));void*
_tmpBF9;void*_tmpBFB;struct Cyc_Absyn_Tvar*_tmpBFD;_LL5DF: _tmpBF9=_tmpBF8.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpBFA=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBF9;if(_tmpBFA->tag != 1)goto _LL5E1;};_tmpBFB=_tmpBF8.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBFC=(struct Cyc_Absyn_VarType_struct*)_tmpBFB;if(_tmpBFC->tag != 2)goto _LL5E1;
else{_tmpBFD=_tmpBFC->f1;}};_LL5E0: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBFD);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBFD,1);continue;_LL5E1:;_LL5E2: goto _LL5DE;_LL5DE:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB33 != 0){const
char*_tmp1493;void*_tmp1492[2];struct Cyc_String_pa_struct _tmp1491;struct Cyc_String_pa_struct
_tmp1490;(_tmp1490.tag=0,((_tmp1490.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD6->name)),((_tmp1491.tag=0,((
_tmp1491.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD5->name)),((_tmp1492[0]=& _tmp1491,((_tmp1492[1]=& _tmp1490,Cyc_Tcutil_terr(
loc,((_tmp1493="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp1493,sizeof(char),43))),_tag_dyneither(_tmp1492,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp1498;void*_tmp1497[2];struct Cyc_String_pa_struct
_tmp1496;struct Cyc_String_pa_struct _tmp1495;(_tmp1495.tag=0,((_tmp1495.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD6->name)),((
_tmp1496.tag=0,((_tmp1496.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBD5->name)),((_tmp1497[0]=& _tmp1496,((_tmp1497[1]=& _tmp1495,Cyc_Tcutil_terr(
loc,((_tmp1498="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp1498,sizeof(char),42))),_tag_dyneither(_tmp1497,sizeof(void*),2)))))))))))));}
goto _LL5CA;}_LL5CA:;}goto _LL56E;_LL57F: {struct Cyc_Absyn_PointerType_struct*
_tmpB34=(struct Cyc_Absyn_PointerType_struct*)_tmpB14;if(_tmpB34->tag != 5)goto
_LL581;else{_tmpB35=_tmpB34->f1;_tmpB36=_tmpB35.elt_typ;_tmpB37=_tmpB35.elt_tq;
_tmpB38=(struct Cyc_Absyn_Tqual*)&(_tmpB34->f1).elt_tq;_tmpB39=_tmpB35.ptr_atts;
_tmpB3A=_tmpB39.rgn;_tmpB3B=_tmpB39.nullable;_tmpB3C=_tmpB39.bounds;_tmpB3D=
_tmpB39.zero_term;}}_LL580: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB36,1);_tmpB38->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB38->print_const,_tmpB36);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5E3: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5E4: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5E5: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB3A,1);{union Cyc_Absyn_Constraint*
_tmpC06=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB3D);union Cyc_Absyn_Constraint _tmpC07;int _tmpC08;union Cyc_Absyn_Constraint
_tmpC09;int _tmpC0A;_LL5E8: _tmpC07=*_tmpC06;if((_tmpC07.No_constr).tag != 3)goto
_LL5EA;_tmpC08=(int)(_tmpC07.No_constr).val;_LL5E9:{void*_tmpC0B=Cyc_Tcutil_compress(
_tmpB36);enum Cyc_Absyn_Size_of _tmpC0D;_LL5EF: {struct Cyc_Absyn_IntType_struct*
_tmpC0C=(struct Cyc_Absyn_IntType_struct*)_tmpC0B;if(_tmpC0C->tag != 6)goto _LL5F1;
else{_tmpC0D=_tmpC0C->f2;if(_tmpC0D != Cyc_Absyn_Char_sz)goto _LL5F1;}}_LL5F0:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5EE;_LL5F1:;_LL5F2:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB3D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5EE;
_LL5EE:;}goto _LL5E7;_LL5EA: _tmpC09=*_tmpC06;if((_tmpC09.Eq_constr).tag != 1)goto
_LL5EC;_tmpC0A=(int)(_tmpC09.Eq_constr).val;if(_tmpC0A != 1)goto _LL5EC;_LL5EB: if(
!Cyc_Tcutil_admits_zero(_tmpB36)){const char*_tmp149C;void*_tmp149B[1];struct Cyc_String_pa_struct
_tmp149A;(_tmp149A.tag=0,((_tmp149A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB36)),((_tmp149B[0]=& _tmp149A,Cyc_Tcutil_terr(
loc,((_tmp149C="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp149C,sizeof(char),53))),_tag_dyneither(_tmp149B,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5E7;_LL5EC:;_LL5ED: is_zero_terminated=0;
goto _LL5E7;_LL5E7:;}{void*_tmpC11=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB3C);struct Cyc_Absyn_Exp*
_tmpC14;_LL5F4: {struct Cyc_Absyn_DynEither_b_struct*_tmpC12=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC11;if(_tmpC12->tag != 0)goto _LL5F6;}_LL5F5: goto _LL5F3;_LL5F6: {struct Cyc_Absyn_Upper_b_struct*
_tmpC13=(struct Cyc_Absyn_Upper_b_struct*)_tmpC11;if(_tmpC13->tag != 1)goto _LL5F3;
else{_tmpC14=_tmpC13->f1;}}_LL5F7: {struct _RegionHandle*_tmpC15=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC16=Cyc_Tcenv_allow_valueof(_tmpC15,te);Cyc_Tcexp_tcExp(
_tmpC16,0,_tmpC14);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC14,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpC14)){const char*_tmp149F;void*
_tmp149E;(_tmp149E=0,Cyc_Tcutil_terr(loc,((_tmp149F="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp149F,sizeof(char),49))),_tag_dyneither(_tmp149E,sizeof(void*),
0)));}{unsigned int _tmpC1A;int _tmpC1B;struct _tuple14 _tmpC19=Cyc_Evexp_eval_const_uint_exp(
_tmpC14);_tmpC1A=_tmpC19.f1;_tmpC1B=_tmpC19.f2;if(is_zero_terminated  && (!
_tmpC1B  || _tmpC1A < 1)){const char*_tmp14A2;void*_tmp14A1;(_tmp14A1=0,Cyc_Tcutil_terr(
loc,((_tmp14A2="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp14A2,sizeof(char),55))),_tag_dyneither(_tmp14A1,sizeof(void*),
0)));}goto _LL5F3;};}_LL5F3:;}goto _LL56E;};}_LL581: {struct Cyc_Absyn_TagType_struct*
_tmpB3E=(struct Cyc_Absyn_TagType_struct*)_tmpB14;if(_tmpB3E->tag != 19)goto _LL583;
else{_tmpB3F=(void*)_tmpB3E->f1;}}_LL582: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB3F,1);goto _LL56E;_LL583: {struct Cyc_Absyn_ValueofType_struct*
_tmpB40=(struct Cyc_Absyn_ValueofType_struct*)_tmpB14;if(_tmpB40->tag != 18)goto
_LL585;else{_tmpB41=_tmpB40->f1;}}_LL584: {struct _RegionHandle*_tmpC1E=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpC1F=Cyc_Tcenv_allow_valueof(_tmpC1E,te);Cyc_Tcexp_tcExp(
_tmpC1F,0,_tmpB41);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB41)){const char*
_tmp14A5;void*_tmp14A4;(_tmp14A4=0,Cyc_Tcutil_terr(loc,((_tmp14A5="valueof_t requires an int expression",
_tag_dyneither(_tmp14A5,sizeof(char),37))),_tag_dyneither(_tmp14A4,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB41,te,cvtenv);goto
_LL56E;}_LL585: {struct Cyc_Absyn_IntType_struct*_tmpB42=(struct Cyc_Absyn_IntType_struct*)
_tmpB14;if(_tmpB42->tag != 6)goto _LL587;}_LL586: goto _LL588;_LL587: {struct Cyc_Absyn_FloatType_struct*
_tmpB43=(struct Cyc_Absyn_FloatType_struct*)_tmpB14;if(_tmpB43->tag != 7)goto
_LL589;}_LL588: goto _LL56E;_LL589: {struct Cyc_Absyn_ArrayType_struct*_tmpB44=(
struct Cyc_Absyn_ArrayType_struct*)_tmpB14;if(_tmpB44->tag != 8)goto _LL58B;else{
_tmpB45=_tmpB44->f1;_tmpB46=_tmpB45.elt_type;_tmpB47=_tmpB45.tq;_tmpB48=(struct
Cyc_Absyn_Tqual*)&(_tmpB44->f1).tq;_tmpB49=_tmpB45.num_elts;_tmpB4A=(struct Cyc_Absyn_Exp**)&(
_tmpB44->f1).num_elts;_tmpB4B=_tmpB45.zero_term;_tmpB4C=_tmpB45.zt_loc;}}_LL58A: {
struct Cyc_Absyn_Exp*_tmpC22=*_tmpB4A;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_tmk,_tmpB46,1);_tmpB48->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB48->print_const,_tmpB46);{int is_zero_terminated;{union Cyc_Absyn_Constraint*
_tmpC23=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB4B);union Cyc_Absyn_Constraint _tmpC24;int _tmpC25;union Cyc_Absyn_Constraint
_tmpC26;int _tmpC27;_LL5F9: _tmpC24=*_tmpC23;if((_tmpC24.No_constr).tag != 3)goto
_LL5FB;_tmpC25=(int)(_tmpC24.No_constr).val;_LL5FA:((int(*)(int(*cmp)(int,int),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB4B,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5F8;
_LL5FB: _tmpC26=*_tmpC23;if((_tmpC26.Eq_constr).tag != 1)goto _LL5FD;_tmpC27=(int)(
_tmpC26.Eq_constr).val;if(_tmpC27 != 1)goto _LL5FD;_LL5FC: if(!Cyc_Tcutil_admits_zero(
_tmpB46)){const char*_tmp14A9;void*_tmp14A8[1];struct Cyc_String_pa_struct _tmp14A7;(
_tmp14A7.tag=0,((_tmp14A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpB46)),((_tmp14A8[0]=& _tmp14A7,Cyc_Tcutil_terr(loc,((_tmp14A9="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp14A9,sizeof(char),51))),_tag_dyneither(_tmp14A8,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5F8;_LL5FD:;_LL5FE: is_zero_terminated=0;
goto _LL5F8;_LL5F8:;}if(_tmpC22 == 0){if(is_zero_terminated)*_tmpB4A=(_tmpC22=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14AC;void*
_tmp14AB;(_tmp14AB=0,Cyc_Tcutil_warn(loc,((_tmp14AC="array bound defaults to 1 here",
_tag_dyneither(_tmp14AC,sizeof(char),31))),_tag_dyneither(_tmp14AB,sizeof(void*),
0)));}*_tmpB4A=(_tmpC22=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC22);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC22)){const char*_tmp14AF;void*_tmp14AE;(_tmp14AE=0,Cyc_Tcutil_terr(loc,((
_tmp14AF="array bounds expression is not constant",_tag_dyneither(_tmp14AF,
sizeof(char),40))),_tag_dyneither(_tmp14AE,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC22)){const char*_tmp14B2;void*_tmp14B1;(_tmp14B1=0,
Cyc_Tcutil_terr(loc,((_tmp14B2="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14B2,sizeof(char),47))),_tag_dyneither(_tmp14B1,sizeof(void*),
0)));}{unsigned int _tmpC32;int _tmpC33;struct _tuple14 _tmpC31=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC22);_tmpC32=_tmpC31.f1;_tmpC33=_tmpC31.f2;if((
is_zero_terminated  && _tmpC33) && _tmpC32 < 1){const char*_tmp14B5;void*_tmp14B4;(
_tmp14B4=0,Cyc_Tcutil_warn(loc,((_tmp14B5="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14B5,sizeof(char),48))),_tag_dyneither(_tmp14B4,sizeof(void*),
0)));}if((_tmpC33  && _tmpC32 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14B8;void*
_tmp14B7;(_tmp14B7=0,Cyc_Tcutil_warn(loc,((_tmp14B8="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14B8,sizeof(char),75))),_tag_dyneither(_tmp14B7,sizeof(void*),
0)));}*_tmpB4A=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL56E;};};}
_LL58B: {struct Cyc_Absyn_FnType_struct*_tmpB4D=(struct Cyc_Absyn_FnType_struct*)
_tmpB14;if(_tmpB4D->tag != 9)goto _LL58D;else{_tmpB4E=_tmpB4D->f1;_tmpB4F=_tmpB4E.tvars;
_tmpB50=(struct Cyc_List_List**)&(_tmpB4D->f1).tvars;_tmpB51=_tmpB4E.effect;
_tmpB52=(struct Cyc_Core_Opt**)&(_tmpB4D->f1).effect;_tmpB53=_tmpB4E.ret_tqual;
_tmpB54=(struct Cyc_Absyn_Tqual*)&(_tmpB4D->f1).ret_tqual;_tmpB55=_tmpB4E.ret_typ;
_tmpB56=_tmpB4E.args;_tmpB57=_tmpB4E.c_varargs;_tmpB58=_tmpB4E.cyc_varargs;
_tmpB59=_tmpB4E.rgn_po;_tmpB5A=_tmpB4E.attributes;}}_LL58C: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB5A != 0;
_tmpB5A=_tmpB5A->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB5A->hd)){const char*
_tmp14BC;void*_tmp14BB[1];struct Cyc_String_pa_struct _tmp14BA;(_tmp14BA.tag=0,((
_tmp14BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB5A->hd)),((_tmp14BB[0]=& _tmp14BA,Cyc_Tcutil_terr(loc,((_tmp14BC="bad function type attribute %s",
_tag_dyneither(_tmp14BC,sizeof(char),31))),_tag_dyneither(_tmp14BB,sizeof(void*),
1)))))));}{void*_tmpC3B=(void*)_tmpB5A->hd;enum Cyc_Absyn_Format_Type _tmpC40;int
_tmpC41;int _tmpC42;_LL600: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC3C=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC3B;if(_tmpC3C->tag != 1)goto _LL602;}_LL601: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5FF;_LL602: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC3D=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC3B;if(_tmpC3D->tag != 2)goto
_LL604;}_LL603: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5FF;_LL604: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC3E=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC3B;if(_tmpC3E->tag != 3)goto _LL606;}_LL605: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5FF;_LL606: {struct Cyc_Absyn_Format_att_struct*_tmpC3F=(
struct Cyc_Absyn_Format_att_struct*)_tmpC3B;if(_tmpC3F->tag != 19)goto _LL608;else{
_tmpC40=_tmpC3F->f1;_tmpC41=_tmpC3F->f2;_tmpC42=_tmpC3F->f3;}}_LL607: if(!
seen_format){seen_format=1;ft=_tmpC40;fmt_desc_arg=_tmpC41;fmt_arg_start=_tmpC42;}
else{const char*_tmp14BF;void*_tmp14BE;(_tmp14BE=0,Cyc_Tcutil_terr(loc,((_tmp14BF="function can't have multiple format attributes",
_tag_dyneither(_tmp14BF,sizeof(char),47))),_tag_dyneither(_tmp14BE,sizeof(void*),
0)));}goto _LL5FF;_LL608:;_LL609: goto _LL5FF;_LL5FF:;};}if(num_convs > 1){const char*
_tmp14C2;void*_tmp14C1;(_tmp14C1=0,Cyc_Tcutil_terr(loc,((_tmp14C2="function can't have multiple calling conventions",
_tag_dyneither(_tmp14C2,sizeof(char),49))),_tag_dyneither(_tmp14C1,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB50);{struct Cyc_List_List*b=*
_tmpB50;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC47=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC49;struct Cyc_Absyn_Kind _tmpC4A;enum Cyc_Absyn_KindQual
_tmpC4B;_LL60B: {struct Cyc_Absyn_Eq_kb_struct*_tmpC48=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC47;if(_tmpC48->tag != 0)goto _LL60D;else{_tmpC49=_tmpC48->f1;_tmpC4A=*_tmpC49;
_tmpC4B=_tmpC4A.kind;if(_tmpC4B != Cyc_Absyn_MemKind)goto _LL60D;}}_LL60C:{const
char*_tmp14C6;void*_tmp14C5[1];struct Cyc_String_pa_struct _tmp14C4;(_tmp14C4.tag=
0,((_tmp14C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14C5[0]=& _tmp14C4,Cyc_Tcutil_terr(loc,((_tmp14C6="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14C6,sizeof(char),51))),_tag_dyneither(_tmp14C5,sizeof(void*),
1)))))));}goto _LL60A;_LL60D:;_LL60E: goto _LL60A;_LL60A:;};}}{struct _RegionHandle*
_tmpC4F=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp14C7;struct Cyc_Tcutil_CVTEnv
_tmpC50=(_tmp14C7.r=_tmpC4F,((_tmp14C7.kind_env=cvtenv.kind_env,((_tmp14C7.free_vars=
0,((_tmp14C7.free_evars=0,((_tmp14C7.generalize_evars=cvtenv.generalize_evars,((
_tmp14C7.fn_result=1,_tmp14C7)))))))))));_tmpC50=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC50,& Cyc_Tcutil_tmk,_tmpB55,1);_tmpB54->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB54->print_const,_tmpB55);_tmpC50.fn_result=0;{struct Cyc_List_List*a=
_tmpB56;for(0;a != 0;a=a->tl){struct _tuple10*_tmpC51=(struct _tuple10*)a->hd;void*
_tmpC52=(*_tmpC51).f3;_tmpC50=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC50,& Cyc_Tcutil_tmk,
_tmpC52,1);{int _tmpC53=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC51).f2).print_const,
_tmpC52);((*_tmpC51).f2).real_const=_tmpC53;{void*_tmpC54=Cyc_Tcutil_compress(
_tmpC52);struct Cyc_Absyn_ArrayInfo _tmpC56;void*_tmpC57;struct Cyc_Absyn_Tqual
_tmpC58;struct Cyc_Absyn_Exp*_tmpC59;union Cyc_Absyn_Constraint*_tmpC5A;struct Cyc_Position_Segment*
_tmpC5B;_LL610: {struct Cyc_Absyn_ArrayType_struct*_tmpC55=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC54;if(_tmpC55->tag != 8)goto _LL612;else{_tmpC56=_tmpC55->f1;_tmpC57=_tmpC56.elt_type;
_tmpC58=_tmpC56.tq;_tmpC59=_tmpC56.num_elts;_tmpC5A=_tmpC56.zero_term;_tmpC5B=
_tmpC56.zt_loc;}}_LL611: {void*_tmpC5C=Cyc_Absyn_new_evar(0,0);_tmpC50=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC50,& Cyc_Tcutil_rk,_tmpC5C,1);{struct Cyc_Absyn_Upper_b_struct _tmp14CA;
struct Cyc_Absyn_Upper_b_struct*_tmp14C9;void*_tmpC5D=Cyc_Absyn_atb_typ(_tmpC57,
_tmpC5C,_tmpC58,(void*)((_tmp14C9=_cycalloc(sizeof(*_tmp14C9)),((_tmp14C9[0]=((
_tmp14CA.tag=1,((_tmp14CA.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC59),_tmp14CA)))),
_tmp14C9)))),_tmpC5A);(*_tmpC51).f3=_tmpC5D;goto _LL60F;};}_LL612:;_LL613: goto
_LL60F;_LL60F:;};};}}if(_tmpB58 != 0){if(_tmpB57){const char*_tmp14CD;void*
_tmp14CC;(_tmp14CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14CD="both c_vararg and cyc_vararg",_tag_dyneither(
_tmp14CD,sizeof(char),29))),_tag_dyneither(_tmp14CC,sizeof(void*),0)));}{void*
_tmpC63;int _tmpC64;struct Cyc_Absyn_VarargInfo _tmpC62=*_tmpB58;_tmpC63=_tmpC62.type;
_tmpC64=_tmpC62.inject;_tmpC50=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC50,& Cyc_Tcutil_tmk,
_tmpC63,1);(_tmpB58->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(
_tmpB58->tq).print_const,_tmpC63);if(_tmpC64){void*_tmpC65=Cyc_Tcutil_compress(
_tmpC63);struct Cyc_Absyn_PtrInfo _tmpC67;void*_tmpC68;struct Cyc_Absyn_PtrAtts
_tmpC69;union Cyc_Absyn_Constraint*_tmpC6A;union Cyc_Absyn_Constraint*_tmpC6B;
_LL615: {struct Cyc_Absyn_PointerType_struct*_tmpC66=(struct Cyc_Absyn_PointerType_struct*)
_tmpC65;if(_tmpC66->tag != 5)goto _LL617;else{_tmpC67=_tmpC66->f1;_tmpC68=_tmpC67.elt_typ;
_tmpC69=_tmpC67.ptr_atts;_tmpC6A=_tmpC69.bounds;_tmpC6B=_tmpC69.zero_term;}}
_LL616:{void*_tmpC6C=Cyc_Tcutil_compress(_tmpC68);_LL61A: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC6D=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC6C;if(_tmpC6D->tag != 3)goto
_LL61C;}_LL61B: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC6B)){const char*_tmp14D0;void*_tmp14CF;(_tmp14CF=0,Cyc_Tcutil_terr(loc,((
_tmp14D0="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14D0,sizeof(
char),37))),_tag_dyneither(_tmp14CF,sizeof(void*),0)));}{void*_tmpC70=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC6A);_LL61F: {struct Cyc_Absyn_DynEither_b_struct*_tmpC71=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC70;if(_tmpC71->tag != 0)goto _LL621;}_LL620:{const char*_tmp14D3;void*_tmp14D2;(
_tmp14D2=0,Cyc_Tcutil_terr(loc,((_tmp14D3="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14D3,sizeof(char),44))),_tag_dyneither(_tmp14D2,sizeof(void*),
0)));}goto _LL61E;_LL621:;_LL622: goto _LL61E;_LL61E:;}goto _LL619;_LL61C:;_LL61D:{
const char*_tmp14D6;void*_tmp14D5;(_tmp14D5=0,Cyc_Tcutil_terr(loc,((_tmp14D6="can't inject a non-datatype type",
_tag_dyneither(_tmp14D6,sizeof(char),33))),_tag_dyneither(_tmp14D5,sizeof(void*),
0)));}goto _LL619;_LL619:;}goto _LL614;_LL617:;_LL618:{const char*_tmp14D9;void*
_tmp14D8;(_tmp14D8=0,Cyc_Tcutil_terr(loc,((_tmp14D9="expecting a datatype pointer type",
_tag_dyneither(_tmp14D9,sizeof(char),34))),_tag_dyneither(_tmp14D8,sizeof(void*),
0)));}goto _LL614;_LL614:;}};}if(seen_format){int _tmpC78=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB56);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC78) || 
fmt_arg_start < 0) || _tmpB58 == 0  && fmt_arg_start != 0) || _tmpB58 != 0  && 
fmt_arg_start != _tmpC78 + 1){const char*_tmp14DC;void*_tmp14DB;(_tmp14DB=0,Cyc_Tcutil_terr(
loc,((_tmp14DC="bad format descriptor",_tag_dyneither(_tmp14DC,sizeof(char),22))),
_tag_dyneither(_tmp14DB,sizeof(void*),0)));}else{void*_tmpC7C;struct _tuple10
_tmpC7B=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB56,
fmt_desc_arg - 1);_tmpC7C=_tmpC7B.f3;{void*_tmpC7D=Cyc_Tcutil_compress(_tmpC7C);
struct Cyc_Absyn_PtrInfo _tmpC7F;void*_tmpC80;struct Cyc_Absyn_PtrAtts _tmpC81;union
Cyc_Absyn_Constraint*_tmpC82;union Cyc_Absyn_Constraint*_tmpC83;_LL624: {struct
Cyc_Absyn_PointerType_struct*_tmpC7E=(struct Cyc_Absyn_PointerType_struct*)
_tmpC7D;if(_tmpC7E->tag != 5)goto _LL626;else{_tmpC7F=_tmpC7E->f1;_tmpC80=_tmpC7F.elt_typ;
_tmpC81=_tmpC7F.ptr_atts;_tmpC82=_tmpC81.bounds;_tmpC83=_tmpC81.zero_term;}}
_LL625:{struct _tuple0 _tmp14DD;struct _tuple0 _tmpC85=(_tmp14DD.f1=Cyc_Tcutil_compress(
_tmpC80),((_tmp14DD.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC82),_tmp14DD)));void*_tmpC86;enum Cyc_Absyn_Sign
_tmpC88;enum Cyc_Absyn_Size_of _tmpC89;void*_tmpC8A;_LL629: _tmpC86=_tmpC85.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC87=(struct Cyc_Absyn_IntType_struct*)_tmpC86;
if(_tmpC87->tag != 6)goto _LL62B;else{_tmpC88=_tmpC87->f1;if(_tmpC88 != Cyc_Absyn_None)
goto _LL62B;_tmpC89=_tmpC87->f2;if(_tmpC89 != Cyc_Absyn_Char_sz)goto _LL62B;}};
_tmpC8A=_tmpC85.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC8B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC8A;if(_tmpC8B->tag != 0)goto _LL62B;};_LL62A: goto _LL628;_LL62B:;_LL62C:{const
char*_tmp14E0;void*_tmp14DF;(_tmp14DF=0,Cyc_Tcutil_terr(loc,((_tmp14E0="format descriptor is not a char ? type",
_tag_dyneither(_tmp14E0,sizeof(char),39))),_tag_dyneither(_tmp14DF,sizeof(void*),
0)));}goto _LL628;_LL628:;}goto _LL623;_LL626:;_LL627:{const char*_tmp14E3;void*
_tmp14E2;(_tmp14E2=0,Cyc_Tcutil_terr(loc,((_tmp14E3="format descriptor is not a char ? type",
_tag_dyneither(_tmp14E3,sizeof(char),39))),_tag_dyneither(_tmp14E2,sizeof(void*),
0)));}goto _LL623;_LL623:;}if(fmt_arg_start != 0){int problem;{struct _tuple32
_tmp14E4;struct _tuple32 _tmpC91=(_tmp14E4.f1=ft,((_tmp14E4.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB58))->type)),
_tmp14E4)));enum Cyc_Absyn_Format_Type _tmpC92;void*_tmpC93;struct Cyc_Absyn_DatatypeInfo
_tmpC95;union Cyc_Absyn_DatatypeInfoU _tmpC96;struct Cyc_Absyn_Datatypedecl**
_tmpC97;struct Cyc_Absyn_Datatypedecl*_tmpC98;enum Cyc_Absyn_Format_Type _tmpC99;
void*_tmpC9A;struct Cyc_Absyn_DatatypeInfo _tmpC9C;union Cyc_Absyn_DatatypeInfoU
_tmpC9D;struct Cyc_Absyn_Datatypedecl**_tmpC9E;struct Cyc_Absyn_Datatypedecl*
_tmpC9F;_LL62E: _tmpC92=_tmpC91.f1;if(_tmpC92 != Cyc_Absyn_Printf_ft)goto _LL630;
_tmpC93=_tmpC91.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpC94=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC93;if(_tmpC94->tag != 3)goto _LL630;else{_tmpC95=_tmpC94->f1;_tmpC96=_tmpC95.datatype_info;
if((_tmpC96.KnownDatatype).tag != 2)goto _LL630;_tmpC97=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC96.KnownDatatype).val;_tmpC98=*_tmpC97;}};_LL62F: problem=Cyc_Absyn_qvar_cmp(
_tmpC98->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL62D;_LL630: _tmpC99=
_tmpC91.f1;if(_tmpC99 != Cyc_Absyn_Scanf_ft)goto _LL632;_tmpC9A=_tmpC91.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpC9B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpC9A;if(_tmpC9B->tag != 3)goto _LL632;else{_tmpC9C=_tmpC9B->f1;_tmpC9D=_tmpC9C.datatype_info;
if((_tmpC9D.KnownDatatype).tag != 2)goto _LL632;_tmpC9E=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC9D.KnownDatatype).val;_tmpC9F=*_tmpC9E;}};_LL631: problem=Cyc_Absyn_qvar_cmp(
_tmpC9F->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL62D;_LL632:;_LL633:
problem=1;goto _LL62D;_LL62D:;}if(problem){const char*_tmp14E7;void*_tmp14E6;(
_tmp14E6=0,Cyc_Tcutil_terr(loc,((_tmp14E7="format attribute and vararg types don't match",
_tag_dyneither(_tmp14E7,sizeof(char),46))),_tag_dyneither(_tmp14E6,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB59;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpCA3;void*_tmpCA4;void*_tmpCA5;struct _tuple0*_tmpCA2=(struct _tuple0*)
rpo->hd;_tmpCA3=*_tmpCA2;_tmpCA4=_tmpCA3.f1;_tmpCA5=_tmpCA3.f2;_tmpC50=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC50,& Cyc_Tcutil_ek,_tmpCA4,1);_tmpC50=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC50,& Cyc_Tcutil_trk,_tmpCA5,1);}}if(*_tmpB52 != 0)_tmpC50=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC50,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB52))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC50.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpCA7;int _tmpCA8;struct
_tuple30 _tmpCA6=*((struct _tuple30*)tvs->hd);_tmpCA7=_tmpCA6.f1;_tmpCA8=_tmpCA6.f2;
if(!_tmpCA8)continue;{void*_tmpCA9=Cyc_Absyn_compress_kb(_tmpCA7->kind);struct
Cyc_Core_Opt*_tmpCAB;struct Cyc_Core_Opt**_tmpCAC;struct Cyc_Absyn_Kind*_tmpCAD;
struct Cyc_Absyn_Kind*_tmpCAF;struct Cyc_Absyn_Kind*_tmpCB1;struct Cyc_Absyn_Kind
_tmpCB2;enum Cyc_Absyn_KindQual _tmpCB3;struct Cyc_Absyn_Kind*_tmpCB5;struct Cyc_Absyn_Kind
_tmpCB6;enum Cyc_Absyn_KindQual _tmpCB7;struct Cyc_Core_Opt*_tmpCB9;struct Cyc_Core_Opt**
_tmpCBA;struct Cyc_Absyn_Kind*_tmpCBB;struct Cyc_Absyn_Kind _tmpCBC;enum Cyc_Absyn_KindQual
_tmpCBD;struct Cyc_Absyn_Kind*_tmpCBF;struct Cyc_Absyn_Kind _tmpCC0;enum Cyc_Absyn_KindQual
_tmpCC1;struct Cyc_Core_Opt*_tmpCC3;struct Cyc_Core_Opt**_tmpCC4;_LL635:{struct Cyc_Absyn_Less_kb_struct*
_tmpCAA=(struct Cyc_Absyn_Less_kb_struct*)_tmpCA9;if(_tmpCAA->tag != 2)goto _LL637;
else{_tmpCAB=_tmpCAA->f1;_tmpCAC=(struct Cyc_Core_Opt**)& _tmpCAA->f1;_tmpCAD=
_tmpCAA->f2;}}if(!(_tmpCAD->kind == Cyc_Absyn_RgnKind))goto _LL637;_LL636: if(
_tmpCAD->aliasqual == Cyc_Absyn_Top){*_tmpCAC=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCAF=_tmpCAD;goto _LL638;}*_tmpCAC=Cyc_Tcutil_kind_to_bound_opt(_tmpCAD);
_tmpCAF=_tmpCAD;goto _LL638;_LL637:{struct Cyc_Absyn_Eq_kb_struct*_tmpCAE=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCA9;if(_tmpCAE->tag != 0)goto _LL639;else{_tmpCAF=
_tmpCAE->f1;}}if(!(_tmpCAF->kind == Cyc_Absyn_RgnKind))goto _LL639;_LL638:{struct
Cyc_Absyn_AccessEff_struct*_tmp14F6;struct Cyc_Absyn_VarType_struct*_tmp14F5;
struct Cyc_Absyn_VarType_struct _tmp14F4;struct Cyc_Absyn_AccessEff_struct _tmp14F3;
struct Cyc_List_List*_tmp14F2;effect=((_tmp14F2=_cycalloc(sizeof(*_tmp14F2)),((
_tmp14F2->hd=(void*)((_tmp14F6=_cycalloc(sizeof(*_tmp14F6)),((_tmp14F6[0]=((
_tmp14F3.tag=23,((_tmp14F3.f1=(void*)((void*)((_tmp14F5=_cycalloc(sizeof(*
_tmp14F5)),((_tmp14F5[0]=((_tmp14F4.tag=2,((_tmp14F4.f1=_tmpCA7,_tmp14F4)))),
_tmp14F5))))),_tmp14F3)))),_tmp14F6)))),((_tmp14F2->tl=effect,_tmp14F2))))));}
goto _LL634;_LL639: {struct Cyc_Absyn_Less_kb_struct*_tmpCB0=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCA9;if(_tmpCB0->tag != 2)goto _LL63B;else{_tmpCB1=_tmpCB0->f2;_tmpCB2=*_tmpCB1;
_tmpCB3=_tmpCB2.kind;if(_tmpCB3 != Cyc_Absyn_IntKind)goto _LL63B;}}_LL63A: goto
_LL63C;_LL63B: {struct Cyc_Absyn_Eq_kb_struct*_tmpCB4=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCA9;if(_tmpCB4->tag != 0)goto _LL63D;else{_tmpCB5=_tmpCB4->f1;_tmpCB6=*_tmpCB5;
_tmpCB7=_tmpCB6.kind;if(_tmpCB7 != Cyc_Absyn_IntKind)goto _LL63D;}}_LL63C: goto
_LL634;_LL63D: {struct Cyc_Absyn_Less_kb_struct*_tmpCB8=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCA9;if(_tmpCB8->tag != 2)goto _LL63F;else{_tmpCB9=_tmpCB8->f1;_tmpCBA=(struct
Cyc_Core_Opt**)& _tmpCB8->f1;_tmpCBB=_tmpCB8->f2;_tmpCBC=*_tmpCBB;_tmpCBD=_tmpCBC.kind;
if(_tmpCBD != Cyc_Absyn_EffKind)goto _LL63F;}}_LL63E:*_tmpCBA=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL640;_LL63F: {struct Cyc_Absyn_Eq_kb_struct*_tmpCBE=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCA9;if(_tmpCBE->tag != 0)goto _LL641;else{_tmpCBF=
_tmpCBE->f1;_tmpCC0=*_tmpCBF;_tmpCC1=_tmpCC0.kind;if(_tmpCC1 != Cyc_Absyn_EffKind)
goto _LL641;}}_LL640:{struct Cyc_Absyn_VarType_struct*_tmp14FC;struct Cyc_Absyn_VarType_struct
_tmp14FB;struct Cyc_List_List*_tmp14FA;effect=((_tmp14FA=_cycalloc(sizeof(*
_tmp14FA)),((_tmp14FA->hd=(void*)((_tmp14FC=_cycalloc(sizeof(*_tmp14FC)),((
_tmp14FC[0]=((_tmp14FB.tag=2,((_tmp14FB.f1=_tmpCA7,_tmp14FB)))),_tmp14FC)))),((
_tmp14FA->tl=effect,_tmp14FA))))));}goto _LL634;_LL641: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCC2=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpCA9;if(_tmpCC2->tag != 1)goto
_LL643;else{_tmpCC3=_tmpCC2->f1;_tmpCC4=(struct Cyc_Core_Opt**)& _tmpCC2->f1;}}
_LL642:{struct Cyc_Absyn_Less_kb_struct*_tmp1502;struct Cyc_Absyn_Less_kb_struct
_tmp1501;struct Cyc_Core_Opt*_tmp1500;*_tmpCC4=((_tmp1500=_cycalloc(sizeof(*
_tmp1500)),((_tmp1500->v=(void*)((_tmp1502=_cycalloc(sizeof(*_tmp1502)),((
_tmp1502[0]=((_tmp1501.tag=2,((_tmp1501.f1=0,((_tmp1501.f2=& Cyc_Tcutil_ak,
_tmp1501)))))),_tmp1502)))),_tmp1500))));}goto _LL644;_LL643:;_LL644:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1511;struct Cyc_Absyn_VarType_struct*_tmp1510;struct Cyc_Absyn_VarType_struct
_tmp150F;struct Cyc_Absyn_RgnsEff_struct _tmp150E;struct Cyc_List_List*_tmp150D;
effect=((_tmp150D=_cycalloc(sizeof(*_tmp150D)),((_tmp150D->hd=(void*)((_tmp1511=
_cycalloc(sizeof(*_tmp1511)),((_tmp1511[0]=((_tmp150E.tag=25,((_tmp150E.f1=(void*)((
void*)((_tmp1510=_cycalloc(sizeof(*_tmp1510)),((_tmp1510[0]=((_tmp150F.tag=2,((
_tmp150F.f1=_tmpCA7,_tmp150F)))),_tmp1510))))),_tmp150E)))),_tmp1511)))),((
_tmp150D->tl=effect,_tmp150D))))));}goto _LL634;_LL634:;};}}{struct Cyc_List_List*
ts=_tmpC50.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCD6;int _tmpCD7;struct
_tuple31 _tmpCD5=*((struct _tuple31*)ts->hd);_tmpCD6=_tmpCD5.f1;_tmpCD7=_tmpCD5.f2;
if(!_tmpCD7)continue;{struct Cyc_Absyn_Kind*_tmpCD8=Cyc_Tcutil_typ_kind(_tmpCD6);
struct Cyc_Absyn_Kind _tmpCD9;enum Cyc_Absyn_KindQual _tmpCDA;struct Cyc_Absyn_Kind
_tmpCDB;enum Cyc_Absyn_KindQual _tmpCDC;struct Cyc_Absyn_Kind _tmpCDD;enum Cyc_Absyn_KindQual
_tmpCDE;_LL646: _tmpCD9=*_tmpCD8;_tmpCDA=_tmpCD9.kind;if(_tmpCDA != Cyc_Absyn_RgnKind)
goto _LL648;_LL647:{struct Cyc_Absyn_AccessEff_struct*_tmp1517;struct Cyc_Absyn_AccessEff_struct
_tmp1516;struct Cyc_List_List*_tmp1515;effect=((_tmp1515=_cycalloc(sizeof(*
_tmp1515)),((_tmp1515->hd=(void*)((_tmp1517=_cycalloc(sizeof(*_tmp1517)),((
_tmp1517[0]=((_tmp1516.tag=23,((_tmp1516.f1=(void*)_tmpCD6,_tmp1516)))),_tmp1517)))),((
_tmp1515->tl=effect,_tmp1515))))));}goto _LL645;_LL648: _tmpCDB=*_tmpCD8;_tmpCDC=
_tmpCDB.kind;if(_tmpCDC != Cyc_Absyn_EffKind)goto _LL64A;_LL649:{struct Cyc_List_List*
_tmp1518;effect=((_tmp1518=_cycalloc(sizeof(*_tmp1518)),((_tmp1518->hd=_tmpCD6,((
_tmp1518->tl=effect,_tmp1518))))));}goto _LL645;_LL64A: _tmpCDD=*_tmpCD8;_tmpCDE=
_tmpCDD.kind;if(_tmpCDE != Cyc_Absyn_IntKind)goto _LL64C;_LL64B: goto _LL645;_LL64C:;
_LL64D:{struct Cyc_Absyn_RgnsEff_struct*_tmp151E;struct Cyc_Absyn_RgnsEff_struct
_tmp151D;struct Cyc_List_List*_tmp151C;effect=((_tmp151C=_cycalloc(sizeof(*
_tmp151C)),((_tmp151C->hd=(void*)((_tmp151E=_cycalloc(sizeof(*_tmp151E)),((
_tmp151E[0]=((_tmp151D.tag=25,((_tmp151D.f1=(void*)_tmpCD6,_tmp151D)))),_tmp151E)))),((
_tmp151C->tl=effect,_tmp151C))))));}goto _LL645;_LL645:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1524;struct Cyc_Absyn_JoinEff_struct _tmp1523;struct Cyc_Core_Opt*_tmp1522;*
_tmpB52=((_tmp1522=_cycalloc(sizeof(*_tmp1522)),((_tmp1522->v=(void*)((_tmp1524=
_cycalloc(sizeof(*_tmp1524)),((_tmp1524[0]=((_tmp1523.tag=24,((_tmp1523.f1=
effect,_tmp1523)))),_tmp1524)))),_tmp1522))));};}if(*_tmpB50 != 0){struct Cyc_List_List*
bs=*_tmpB50;for(0;bs != 0;bs=bs->tl){void*_tmpCE9=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCEB;struct Cyc_Core_Opt**
_tmpCEC;struct Cyc_Core_Opt*_tmpCEE;struct Cyc_Core_Opt**_tmpCEF;struct Cyc_Absyn_Kind*
_tmpCF0;struct Cyc_Absyn_Kind _tmpCF1;enum Cyc_Absyn_KindQual _tmpCF2;enum Cyc_Absyn_AliasQual
_tmpCF3;struct Cyc_Core_Opt*_tmpCF5;struct Cyc_Core_Opt**_tmpCF6;struct Cyc_Absyn_Kind*
_tmpCF7;struct Cyc_Absyn_Kind _tmpCF8;enum Cyc_Absyn_KindQual _tmpCF9;enum Cyc_Absyn_AliasQual
_tmpCFA;struct Cyc_Core_Opt*_tmpCFC;struct Cyc_Core_Opt**_tmpCFD;struct Cyc_Absyn_Kind*
_tmpCFE;struct Cyc_Absyn_Kind _tmpCFF;enum Cyc_Absyn_KindQual _tmpD00;enum Cyc_Absyn_AliasQual
_tmpD01;struct Cyc_Core_Opt*_tmpD03;struct Cyc_Core_Opt**_tmpD04;struct Cyc_Absyn_Kind*
_tmpD05;struct Cyc_Absyn_Kind _tmpD06;enum Cyc_Absyn_KindQual _tmpD07;enum Cyc_Absyn_AliasQual
_tmpD08;struct Cyc_Core_Opt*_tmpD0A;struct Cyc_Core_Opt**_tmpD0B;struct Cyc_Absyn_Kind*
_tmpD0C;struct Cyc_Absyn_Kind _tmpD0D;enum Cyc_Absyn_KindQual _tmpD0E;enum Cyc_Absyn_AliasQual
_tmpD0F;struct Cyc_Core_Opt*_tmpD11;struct Cyc_Core_Opt**_tmpD12;struct Cyc_Absyn_Kind*
_tmpD13;struct Cyc_Absyn_Kind _tmpD14;enum Cyc_Absyn_KindQual _tmpD15;enum Cyc_Absyn_AliasQual
_tmpD16;struct Cyc_Core_Opt*_tmpD18;struct Cyc_Core_Opt**_tmpD19;struct Cyc_Absyn_Kind*
_tmpD1A;struct Cyc_Absyn_Kind _tmpD1B;enum Cyc_Absyn_KindQual _tmpD1C;enum Cyc_Absyn_AliasQual
_tmpD1D;struct Cyc_Core_Opt*_tmpD1F;struct Cyc_Core_Opt**_tmpD20;struct Cyc_Absyn_Kind*
_tmpD21;struct Cyc_Absyn_Kind _tmpD22;enum Cyc_Absyn_KindQual _tmpD23;enum Cyc_Absyn_AliasQual
_tmpD24;struct Cyc_Core_Opt*_tmpD26;struct Cyc_Core_Opt**_tmpD27;struct Cyc_Absyn_Kind*
_tmpD28;struct Cyc_Absyn_Kind*_tmpD2A;struct Cyc_Absyn_Kind _tmpD2B;enum Cyc_Absyn_KindQual
_tmpD2C;_LL64F: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCEA=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCE9;if(_tmpCEA->tag != 1)goto _LL651;else{_tmpCEB=_tmpCEA->f1;_tmpCEC=(struct
Cyc_Core_Opt**)& _tmpCEA->f1;}}_LL650:{const char*_tmp1528;void*_tmp1527[1];struct
Cyc_String_pa_struct _tmp1526;(_tmp1526.tag=0,((_tmp1526.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1527[0]=&
_tmp1526,Cyc_Tcutil_warn(loc,((_tmp1528="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1528,sizeof(char),47))),_tag_dyneither(_tmp1527,sizeof(void*),
1)))))));}_tmpCEF=_tmpCEC;goto _LL652;_LL651: {struct Cyc_Absyn_Less_kb_struct*
_tmpCED=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;if(_tmpCED->tag != 2)goto _LL653;
else{_tmpCEE=_tmpCED->f1;_tmpCEF=(struct Cyc_Core_Opt**)& _tmpCED->f1;_tmpCF0=
_tmpCED->f2;_tmpCF1=*_tmpCF0;_tmpCF2=_tmpCF1.kind;if(_tmpCF2 != Cyc_Absyn_BoxKind)
goto _LL653;_tmpCF3=_tmpCF1.aliasqual;if(_tmpCF3 != Cyc_Absyn_Top)goto _LL653;}}
_LL652: _tmpCF6=_tmpCEF;goto _LL654;_LL653: {struct Cyc_Absyn_Less_kb_struct*
_tmpCF4=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;if(_tmpCF4->tag != 2)goto _LL655;
else{_tmpCF5=_tmpCF4->f1;_tmpCF6=(struct Cyc_Core_Opt**)& _tmpCF4->f1;_tmpCF7=
_tmpCF4->f2;_tmpCF8=*_tmpCF7;_tmpCF9=_tmpCF8.kind;if(_tmpCF9 != Cyc_Absyn_MemKind)
goto _LL655;_tmpCFA=_tmpCF8.aliasqual;if(_tmpCFA != Cyc_Absyn_Top)goto _LL655;}}
_LL654: _tmpCFD=_tmpCF6;goto _LL656;_LL655: {struct Cyc_Absyn_Less_kb_struct*
_tmpCFB=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;if(_tmpCFB->tag != 2)goto _LL657;
else{_tmpCFC=_tmpCFB->f1;_tmpCFD=(struct Cyc_Core_Opt**)& _tmpCFB->f1;_tmpCFE=
_tmpCFB->f2;_tmpCFF=*_tmpCFE;_tmpD00=_tmpCFF.kind;if(_tmpD00 != Cyc_Absyn_MemKind)
goto _LL657;_tmpD01=_tmpCFF.aliasqual;if(_tmpD01 != Cyc_Absyn_Aliasable)goto _LL657;}}
_LL656: _tmpD04=_tmpCFD;goto _LL658;_LL657: {struct Cyc_Absyn_Less_kb_struct*
_tmpD02=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;if(_tmpD02->tag != 2)goto _LL659;
else{_tmpD03=_tmpD02->f1;_tmpD04=(struct Cyc_Core_Opt**)& _tmpD02->f1;_tmpD05=
_tmpD02->f2;_tmpD06=*_tmpD05;_tmpD07=_tmpD06.kind;if(_tmpD07 != Cyc_Absyn_AnyKind)
goto _LL659;_tmpD08=_tmpD06.aliasqual;if(_tmpD08 != Cyc_Absyn_Top)goto _LL659;}}
_LL658: _tmpD0B=_tmpD04;goto _LL65A;_LL659: {struct Cyc_Absyn_Less_kb_struct*
_tmpD09=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;if(_tmpD09->tag != 2)goto _LL65B;
else{_tmpD0A=_tmpD09->f1;_tmpD0B=(struct Cyc_Core_Opt**)& _tmpD09->f1;_tmpD0C=
_tmpD09->f2;_tmpD0D=*_tmpD0C;_tmpD0E=_tmpD0D.kind;if(_tmpD0E != Cyc_Absyn_AnyKind)
goto _LL65B;_tmpD0F=_tmpD0D.aliasqual;if(_tmpD0F != Cyc_Absyn_Aliasable)goto _LL65B;}}
_LL65A:*_tmpD0B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL64E;_LL65B: {
struct Cyc_Absyn_Less_kb_struct*_tmpD10=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;
if(_tmpD10->tag != 2)goto _LL65D;else{_tmpD11=_tmpD10->f1;_tmpD12=(struct Cyc_Core_Opt**)&
_tmpD10->f1;_tmpD13=_tmpD10->f2;_tmpD14=*_tmpD13;_tmpD15=_tmpD14.kind;if(_tmpD15
!= Cyc_Absyn_MemKind)goto _LL65D;_tmpD16=_tmpD14.aliasqual;if(_tmpD16 != Cyc_Absyn_Unique)
goto _LL65D;}}_LL65C: _tmpD19=_tmpD12;goto _LL65E;_LL65D: {struct Cyc_Absyn_Less_kb_struct*
_tmpD17=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;if(_tmpD17->tag != 2)goto _LL65F;
else{_tmpD18=_tmpD17->f1;_tmpD19=(struct Cyc_Core_Opt**)& _tmpD17->f1;_tmpD1A=
_tmpD17->f2;_tmpD1B=*_tmpD1A;_tmpD1C=_tmpD1B.kind;if(_tmpD1C != Cyc_Absyn_AnyKind)
goto _LL65F;_tmpD1D=_tmpD1B.aliasqual;if(_tmpD1D != Cyc_Absyn_Unique)goto _LL65F;}}
_LL65E:*_tmpD19=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL64E;_LL65F: {
struct Cyc_Absyn_Less_kb_struct*_tmpD1E=(struct Cyc_Absyn_Less_kb_struct*)_tmpCE9;
if(_tmpD1E->tag != 2)goto _LL661;else{_tmpD1F=_tmpD1E->f1;_tmpD20=(struct Cyc_Core_Opt**)&
_tmpD1E->f1;_tmpD21=_tmpD1E->f2;_tmpD22=*_tmpD21;_tmpD23=_tmpD22.kind;if(_tmpD23
!= Cyc_Absyn_RgnKind)goto _LL661;_tmpD24=_tmpD22.aliasqual;if(_tmpD24 != Cyc_Absyn_Top)
goto _LL661;}}_LL660:*_tmpD20=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL64E;_LL661: {struct Cyc_Absyn_Less_kb_struct*_tmpD25=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCE9;if(_tmpD25->tag != 2)goto _LL663;else{_tmpD26=_tmpD25->f1;_tmpD27=(struct
Cyc_Core_Opt**)& _tmpD25->f1;_tmpD28=_tmpD25->f2;}}_LL662:*_tmpD27=Cyc_Tcutil_kind_to_bound_opt(
_tmpD28);goto _LL64E;_LL663: {struct Cyc_Absyn_Eq_kb_struct*_tmpD29=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCE9;if(_tmpD29->tag != 0)goto _LL665;else{_tmpD2A=_tmpD29->f1;_tmpD2B=*_tmpD2A;
_tmpD2C=_tmpD2B.kind;if(_tmpD2C != Cyc_Absyn_MemKind)goto _LL665;}}_LL664:{const
char*_tmp152B;void*_tmp152A;(_tmp152A=0,Cyc_Tcutil_terr(loc,((_tmp152B="functions can't abstract M types",
_tag_dyneither(_tmp152B,sizeof(char),33))),_tag_dyneither(_tmp152A,sizeof(void*),
0)));}goto _LL64E;_LL665:;_LL666: goto _LL64E;_LL64E:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC50.kind_env,*_tmpB50);_tmpC50.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC50.r,_tmpC50.free_vars,*_tmpB50);{struct Cyc_List_List*tvs=_tmpC50.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD33;int _tmpD34;struct
_tuple30 _tmpD32=*((struct _tuple30*)tvs->hd);_tmpD33=_tmpD32.f1;_tmpD34=_tmpD32.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD33,_tmpD34);}}{struct Cyc_List_List*evs=_tmpC50.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD36;int _tmpD37;struct _tuple31 _tmpD35=*((struct _tuple31*)evs->hd);
_tmpD36=_tmpD35.f1;_tmpD37=_tmpD35.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD36,_tmpD37);}};}goto _LL56E;};}_LL58D: {struct Cyc_Absyn_TupleType_struct*
_tmpB5B=(struct Cyc_Absyn_TupleType_struct*)_tmpB14;if(_tmpB5B->tag != 10)goto
_LL58F;else{_tmpB5C=_tmpB5B->f1;}}_LL58E: for(0;_tmpB5C != 0;_tmpB5C=_tmpB5C->tl){
struct _tuple12*_tmpD39=(struct _tuple12*)_tmpB5C->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpD39).f2,1);((*_tmpD39).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD39).f1).print_const,(*_tmpD39).f2);}goto _LL56E;_LL58F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB5D=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB14;if(_tmpB5D->tag != 12)goto
_LL591;else{_tmpB5E=_tmpB5D->f1;_tmpB5F=_tmpB5D->f2;}}_LL590: {struct
_RegionHandle*_tmpD3A=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmpB5F != 0;_tmpB5F=_tmpB5F->tl){struct Cyc_Absyn_Aggrfield _tmpD3C;struct
_dyneither_ptr*_tmpD3D;struct Cyc_Absyn_Tqual _tmpD3E;struct Cyc_Absyn_Tqual*
_tmpD3F;void*_tmpD40;struct Cyc_Absyn_Exp*_tmpD41;struct Cyc_List_List*_tmpD42;
struct Cyc_Absyn_Aggrfield*_tmpD3B=(struct Cyc_Absyn_Aggrfield*)_tmpB5F->hd;
_tmpD3C=*_tmpD3B;_tmpD3D=_tmpD3C.name;_tmpD3E=_tmpD3C.tq;_tmpD3F=(struct Cyc_Absyn_Tqual*)&(*
_tmpD3B).tq;_tmpD40=_tmpD3C.type;_tmpD41=_tmpD3C.width;_tmpD42=_tmpD3C.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD3D)){
const char*_tmp152F;void*_tmp152E[1];struct Cyc_String_pa_struct _tmp152D;(_tmp152D.tag=
0,((_tmp152D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD3D),((
_tmp152E[0]=& _tmp152D,Cyc_Tcutil_terr(loc,((_tmp152F="duplicate field %s",
_tag_dyneither(_tmp152F,sizeof(char),19))),_tag_dyneither(_tmp152E,sizeof(void*),
1)))))));}{const char*_tmp1530;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD3D,((
_tmp1530="",_tag_dyneither(_tmp1530,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1531;prev_fields=((_tmp1531=_region_malloc(_tmpD3A,sizeof(*_tmp1531)),((
_tmp1531->hd=_tmpD3D,((_tmp1531->tl=prev_fields,_tmp1531))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD40,1);_tmpD3F->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD3F->print_const,_tmpD40);if(_tmpB5E == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD40)){const char*_tmp1535;void*_tmp1534[1];struct Cyc_String_pa_struct _tmp1533;(
_tmp1533.tag=0,((_tmp1533.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD3D),((_tmp1534[0]=& _tmp1533,Cyc_Tcutil_warn(loc,((_tmp1535="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1535,sizeof(char),74))),_tag_dyneither(_tmp1534,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD40,_tmpD41,_tmpD3D);Cyc_Tcutil_check_field_atts(
loc,_tmpD3D,_tmpD42);}}goto _LL56E;}_LL591: {struct Cyc_Absyn_AggrType_struct*
_tmpB60=(struct Cyc_Absyn_AggrType_struct*)_tmpB14;if(_tmpB60->tag != 11)goto
_LL593;else{_tmpB61=_tmpB60->f1;_tmpB62=_tmpB61.aggr_info;_tmpB63=(union Cyc_Absyn_AggrInfoU*)&(
_tmpB60->f1).aggr_info;_tmpB64=_tmpB61.targs;_tmpB65=(struct Cyc_List_List**)&(
_tmpB60->f1).targs;}}_LL592:{union Cyc_Absyn_AggrInfoU _tmpD4B=*_tmpB63;struct
_tuple4 _tmpD4C;enum Cyc_Absyn_AggrKind _tmpD4D;struct _tuple2*_tmpD4E;struct Cyc_Core_Opt*
_tmpD4F;struct Cyc_Absyn_Aggrdecl**_tmpD50;struct Cyc_Absyn_Aggrdecl*_tmpD51;
_LL668: if((_tmpD4B.UnknownAggr).tag != 1)goto _LL66A;_tmpD4C=(struct _tuple4)(
_tmpD4B.UnknownAggr).val;_tmpD4D=_tmpD4C.f1;_tmpD4E=_tmpD4C.f2;_tmpD4F=_tmpD4C.f3;
_LL669: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpD52;_push_handler(&
_tmpD52);{int _tmpD54=0;if(setjmp(_tmpD52.handler))_tmpD54=1;if(!_tmpD54){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD4E);{struct Cyc_Absyn_Aggrdecl*_tmpD55=*adp;if(_tmpD55->kind != 
_tmpD4D){if(_tmpD55->kind == Cyc_Absyn_StructA){const char*_tmp153A;void*_tmp1539[
2];struct Cyc_String_pa_struct _tmp1538;struct Cyc_String_pa_struct _tmp1537;(
_tmp1537.tag=0,((_tmp1537.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4E)),((_tmp1538.tag=0,((_tmp1538.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD4E)),((_tmp1539[0]=& _tmp1538,((
_tmp1539[1]=& _tmp1537,Cyc_Tcutil_terr(loc,((_tmp153A="expecting struct %s instead of union %s",
_tag_dyneither(_tmp153A,sizeof(char),40))),_tag_dyneither(_tmp1539,sizeof(void*),
2)))))))))))));}else{const char*_tmp153F;void*_tmp153E[2];struct Cyc_String_pa_struct
_tmp153D;struct Cyc_String_pa_struct _tmp153C;(_tmp153C.tag=0,((_tmp153C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD4E)),((
_tmp153D.tag=0,((_tmp153D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4E)),((_tmp153E[0]=& _tmp153D,((_tmp153E[1]=& _tmp153C,Cyc_Tcutil_terr(loc,((
_tmp153F="expecting union %s instead of struct %s",_tag_dyneither(_tmp153F,
sizeof(char),40))),_tag_dyneither(_tmp153E,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpD4F  && (int)_tmpD4F->v){if(!((unsigned int)_tmpD55->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD55->impl))->tagged){const char*
_tmp1543;void*_tmp1542[1];struct Cyc_String_pa_struct _tmp1541;(_tmp1541.tag=0,((
_tmp1541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4E)),((_tmp1542[0]=& _tmp1541,Cyc_Tcutil_terr(loc,((_tmp1543="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1543,sizeof(char),42))),_tag_dyneither(_tmp1542,sizeof(void*),
1)))))));}}*_tmpB63=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD53=(void*)_exn_thrown;void*_tmpD62=_tmpD53;_LL66D: {struct Cyc_Dict_Absent_struct*
_tmpD63=(struct Cyc_Dict_Absent_struct*)_tmpD62;if(_tmpD63->tag != Cyc_Dict_Absent)
goto _LL66F;}_LL66E: {struct Cyc_Tcenv_Genv*_tmpD64=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1544;struct Cyc_Absyn_Aggrdecl*_tmpD65=(_tmp1544=
_cycalloc(sizeof(*_tmp1544)),((_tmp1544->kind=_tmpD4D,((_tmp1544->sc=Cyc_Absyn_Extern,((
_tmp1544->name=_tmpD4E,((_tmp1544->tvs=0,((_tmp1544->impl=0,((_tmp1544->attributes=
0,_tmp1544)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD64,loc,_tmpD65);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD4E);*_tmpB63=Cyc_Absyn_KnownAggr(adp);if(*_tmpB65 != 0){{const char*
_tmp1548;void*_tmp1547[1];struct Cyc_String_pa_struct _tmp1546;(_tmp1546.tag=0,((
_tmp1546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD4E)),((_tmp1547[0]=& _tmp1546,Cyc_Tcutil_terr(loc,((_tmp1548="declare parameterized type %s before using",
_tag_dyneither(_tmp1548,sizeof(char),43))),_tag_dyneither(_tmp1547,sizeof(void*),
1)))))));}return cvtenv;}goto _LL66C;}_LL66F:;_LL670:(void)_throw(_tmpD62);_LL66C:;}};}
_tmpD51=*adp;goto _LL66B;}_LL66A: if((_tmpD4B.KnownAggr).tag != 2)goto _LL667;
_tmpD50=(struct Cyc_Absyn_Aggrdecl**)(_tmpD4B.KnownAggr).val;_tmpD51=*_tmpD50;
_LL66B: {struct Cyc_List_List*tvs=_tmpD51->tvs;struct Cyc_List_List*ts=*_tmpB65;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD6A=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD6B=(void*)ts->hd;{struct _tuple0 _tmp1549;
struct _tuple0 _tmpD6D=(_tmp1549.f1=Cyc_Absyn_compress_kb(_tmpD6A->kind),((
_tmp1549.f2=_tmpD6B,_tmp1549)));void*_tmpD6E;void*_tmpD70;struct Cyc_Absyn_Tvar*
_tmpD72;_LL672: _tmpD6E=_tmpD6D.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD6F=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD6E;if(_tmpD6F->tag != 1)goto _LL674;};
_tmpD70=_tmpD6D.f2;{struct Cyc_Absyn_VarType_struct*_tmpD71=(struct Cyc_Absyn_VarType_struct*)
_tmpD70;if(_tmpD71->tag != 2)goto _LL674;else{_tmpD72=_tmpD71->f1;}};_LL673: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD72);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD72,1);continue;_LL674:;_LL675: goto _LL671;_LL671:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp154D;void*_tmp154C[1];struct Cyc_String_pa_struct
_tmp154B;(_tmp154B.tag=0,((_tmp154B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD51->name)),((_tmp154C[0]=& _tmp154B,
Cyc_Tcutil_terr(loc,((_tmp154D="too many parameters for type %s",_tag_dyneither(
_tmp154D,sizeof(char),32))),_tag_dyneither(_tmp154C,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp154E;
hidden_ts=((_tmp154E=_cycalloc(sizeof(*_tmp154E)),((_tmp154E->hd=e,((_tmp154E->tl=
hidden_ts,_tmp154E))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB65,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL667:;}goto _LL56E;_LL593: {struct Cyc_Absyn_TypedefType_struct*
_tmpB66=(struct Cyc_Absyn_TypedefType_struct*)_tmpB14;if(_tmpB66->tag != 17)goto
_LL595;else{_tmpB67=_tmpB66->f1;_tmpB68=_tmpB66->f2;_tmpB69=(struct Cyc_List_List**)&
_tmpB66->f2;_tmpB6A=_tmpB66->f3;_tmpB6B=(struct Cyc_Absyn_Typedefdecl**)& _tmpB66->f3;
_tmpB6C=_tmpB66->f4;_tmpB6D=(void***)& _tmpB66->f4;}}_LL594: {struct Cyc_List_List*
targs=*_tmpB69;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpD77;
_push_handler(& _tmpD77);{int _tmpD79=0;if(setjmp(_tmpD77.handler))_tmpD79=1;if(!
_tmpD79){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB67);;_pop_handler();}else{
void*_tmpD78=(void*)_exn_thrown;void*_tmpD7B=_tmpD78;_LL677: {struct Cyc_Dict_Absent_struct*
_tmpD7C=(struct Cyc_Dict_Absent_struct*)_tmpD7B;if(_tmpD7C->tag != Cyc_Dict_Absent)
goto _LL679;}_LL678:{const char*_tmp1552;void*_tmp1551[1];struct Cyc_String_pa_struct
_tmp1550;(_tmp1550.tag=0,((_tmp1550.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp1551[0]=& _tmp1550,Cyc_Tcutil_terr(
loc,((_tmp1552="unbound typedef name %s",_tag_dyneither(_tmp1552,sizeof(char),24))),
_tag_dyneither(_tmp1551,sizeof(void*),1)))))));}return cvtenv;_LL679:;_LL67A:(
void)_throw(_tmpD7B);_LL676:;}};}*_tmpB6B=(struct Cyc_Absyn_Typedefdecl*)td;
_tmpB67[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle*_tmpD80=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=
Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1555;struct Cyc_List_List*_tmp1554;inst=((_tmp1554=
_region_malloc(_tmpD80,sizeof(*_tmp1554)),((_tmp1554->hd=((_tmp1555=
_region_malloc(_tmpD80,sizeof(*_tmp1555)),((_tmp1555->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd,((_tmp1555->f2=(void*)ts->hd,_tmp1555)))))),((_tmp1554->tl=inst,_tmp1554))))));};}
if(ts != 0){const char*_tmp1559;void*_tmp1558[1];struct Cyc_String_pa_struct
_tmp1557;(_tmp1557.tag=0,((_tmp1557.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB67)),((_tmp1558[0]=& _tmp1557,Cyc_Tcutil_terr(
loc,((_tmp1559="too many parameters for typedef %s",_tag_dyneither(_tmp1559,
sizeof(char),35))),_tag_dyneither(_tmp1558,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp155A;hidden_ts=((_tmp155A=_cycalloc(sizeof(*_tmp155A)),((
_tmp155A->hd=e,((_tmp155A->tl=hidden_ts,_tmp155A))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple16*_tmp155D;struct Cyc_List_List*_tmp155C;inst=(
struct Cyc_List_List*)((_tmp155C=_cycalloc(sizeof(*_tmp155C)),((_tmp155C->hd=(
struct _tuple16*)((_tmp155D=_cycalloc(sizeof(*_tmp155D)),((_tmp155D->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp155D->f2=e,_tmp155D)))))),((_tmp155C->tl=inst,
_tmp155C))))));};}*_tmpB69=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(_tmpD80,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp155E;*_tmpB6D=((_tmp155E=_cycalloc(sizeof(*
_tmp155E)),((_tmp155E[0]=new_typ,_tmp155E))));}}goto _LL56E;};}_LL595: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpB6E=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpB14;if(_tmpB6E->tag != 22)goto
_LL597;}_LL596: goto _LL598;_LL597: {struct Cyc_Absyn_UniqueRgn_struct*_tmpB6F=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmpB14;if(_tmpB6F->tag != 21)goto _LL599;}
_LL598: goto _LL59A;_LL599: {struct Cyc_Absyn_HeapRgn_struct*_tmpB70=(struct Cyc_Absyn_HeapRgn_struct*)
_tmpB14;if(_tmpB70->tag != 20)goto _LL59B;}_LL59A: goto _LL56E;_LL59B: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpB71=(struct Cyc_Absyn_RgnHandleType_struct*)_tmpB14;if(_tmpB71->tag != 15)goto
_LL59D;else{_tmpB72=(void*)_tmpB71->f1;}}_LL59C: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB72,1);goto _LL56E;_LL59D: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB73=(struct Cyc_Absyn_DynRgnType_struct*)_tmpB14;if(_tmpB73->tag != 16)goto
_LL59F;else{_tmpB74=(void*)_tmpB73->f1;_tmpB75=(void*)_tmpB73->f2;}}_LL59E:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB74,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB75,1);
goto _LL56E;_LL59F: {struct Cyc_Absyn_AccessEff_struct*_tmpB76=(struct Cyc_Absyn_AccessEff_struct*)
_tmpB14;if(_tmpB76->tag != 23)goto _LL5A1;else{_tmpB77=(void*)_tmpB76->f1;}}_LL5A0:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB77,1);
goto _LL56E;_LL5A1: {struct Cyc_Absyn_RgnsEff_struct*_tmpB78=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpB14;if(_tmpB78->tag != 25)goto _LL5A3;else{_tmpB79=(void*)_tmpB78->f1;}}_LL5A2:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB79,1);
goto _LL56E;_LL5A3: {struct Cyc_Absyn_JoinEff_struct*_tmpB7A=(struct Cyc_Absyn_JoinEff_struct*)
_tmpB14;if(_tmpB7A->tag != 24)goto _LL56E;else{_tmpB7B=_tmpB7A->f1;}}_LL5A4: for(0;
_tmpB7B != 0;_tmpB7B=_tmpB7B->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB7B->hd,1);}goto _LL56E;_LL56E:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpD8A=t;struct Cyc_Core_Opt*
_tmpD8C;struct Cyc_Core_Opt*_tmpD8D;_LL67C: {struct Cyc_Absyn_Evar_struct*_tmpD8B=(
struct Cyc_Absyn_Evar_struct*)_tmpD8A;if(_tmpD8B->tag != 1)goto _LL67E;else{_tmpD8C=
_tmpD8B->f1;_tmpD8D=_tmpD8B->f2;}}_LL67D: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpD8E=_tmpD8C;struct Cyc_Core_Opt _tmpD8F;struct Cyc_Absyn_Kind*_tmpD90;_LL681:
if(_tmpD8E != 0)goto _LL683;_LL682:{const char*_tmp155F;s=((_tmp155F="kind=NULL ",
_tag_dyneither(_tmp155F,sizeof(char),11)));}goto _LL680;_LL683: if(_tmpD8E == 0)
goto _LL680;_tmpD8F=*_tmpD8E;_tmpD90=(struct Cyc_Absyn_Kind*)_tmpD8F.v;_LL684:{
const char*_tmp1563;void*_tmp1562[1];struct Cyc_String_pa_struct _tmp1561;s=(struct
_dyneither_ptr)((_tmp1561.tag=0,((_tmp1561.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpD90)),((_tmp1562[0]=& _tmp1561,Cyc_aprintf(((
_tmp1563="kind=%s ",_tag_dyneither(_tmp1563,sizeof(char),9))),_tag_dyneither(
_tmp1562,sizeof(void*),1))))))));}goto _LL680;_LL680:;}{struct Cyc_Core_Opt*
_tmpD95=_tmpD8D;struct Cyc_Core_Opt _tmpD96;void*_tmpD97;_LL686: if(_tmpD95 != 0)
goto _LL688;_LL687:{const char*_tmp1567;void*_tmp1566[1];struct Cyc_String_pa_struct
_tmp1565;s=(struct _dyneither_ptr)((_tmp1565.tag=0,((_tmp1565.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1566[0]=& _tmp1565,Cyc_aprintf(((
_tmp1567="%s ref=NULL",_tag_dyneither(_tmp1567,sizeof(char),12))),_tag_dyneither(
_tmp1566,sizeof(void*),1))))))));}goto _LL685;_LL688: if(_tmpD95 == 0)goto _LL685;
_tmpD96=*_tmpD95;_tmpD97=(void*)_tmpD96.v;_LL689:{const char*_tmp156C;void*
_tmp156B[2];struct Cyc_String_pa_struct _tmp156A;struct Cyc_String_pa_struct
_tmp1569;s=(struct _dyneither_ptr)((_tmp1569.tag=0,((_tmp1569.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD97)),((
_tmp156A.tag=0,((_tmp156A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp156B[0]=& _tmp156A,((_tmp156B[1]=& _tmp1569,Cyc_aprintf(((_tmp156C="%s ref=%s",
_tag_dyneither(_tmp156C,sizeof(char),10))),_tag_dyneither(_tmp156B,sizeof(void*),
2))))))))))))));}goto _LL685;_LL685:;}goto _LL67B;}_LL67E:;_LL67F: goto _LL67B;
_LL67B:;}{const char*_tmp1572;void*_tmp1571[3];struct Cyc_String_pa_struct _tmp1570;
struct Cyc_String_pa_struct _tmp156F;struct Cyc_String_pa_struct _tmp156E;(_tmp156E.tag=
0,((_tmp156E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp156F.tag=0,((_tmp156F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1570.tag=0,((
_tmp1570.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1571[0]=& _tmp1570,((_tmp1571[1]=& _tmp156F,((_tmp1571[2]=& _tmp156E,Cyc_Tcutil_terr(
loc,((_tmp1572="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1572,sizeof(char),51))),_tag_dyneither(_tmp1571,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpDA4=e->r;struct Cyc_List_List*_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAC;struct
Cyc_Absyn_Exp*_tmpDAD;struct Cyc_Absyn_Exp*_tmpDAE;struct Cyc_Absyn_Exp*_tmpDB0;
struct Cyc_Absyn_Exp*_tmpDB1;struct Cyc_Absyn_Exp*_tmpDB3;struct Cyc_Absyn_Exp*
_tmpDB4;struct Cyc_Absyn_Exp*_tmpDB6;struct Cyc_Absyn_Exp*_tmpDB7;void*_tmpDB9;
struct Cyc_Absyn_Exp*_tmpDBA;void*_tmpDBC;void*_tmpDBE;void*_tmpDC0;struct Cyc_Absyn_Exp*
_tmpDC2;_LL68B: {struct Cyc_Absyn_Const_e_struct*_tmpDA5=(struct Cyc_Absyn_Const_e_struct*)
_tmpDA4;if(_tmpDA5->tag != 0)goto _LL68D;}_LL68C: goto _LL68E;_LL68D: {struct Cyc_Absyn_Enum_e_struct*
_tmpDA6=(struct Cyc_Absyn_Enum_e_struct*)_tmpDA4;if(_tmpDA6->tag != 32)goto _LL68F;}
_LL68E: goto _LL690;_LL68F: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDA7=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDA4;if(_tmpDA7->tag != 33)goto _LL691;}_LL690: goto _LL692;_LL691: {struct Cyc_Absyn_Var_e_struct*
_tmpDA8=(struct Cyc_Absyn_Var_e_struct*)_tmpDA4;if(_tmpDA8->tag != 1)goto _LL693;}
_LL692: goto _LL68A;_LL693: {struct Cyc_Absyn_Primop_e_struct*_tmpDA9=(struct Cyc_Absyn_Primop_e_struct*)
_tmpDA4;if(_tmpDA9->tag != 3)goto _LL695;else{_tmpDAA=_tmpDA9->f2;}}_LL694: for(0;
_tmpDAA != 0;_tmpDAA=_tmpDAA->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpDAA->hd,te,cvtenv);}goto _LL68A;_LL695: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDAB=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDA4;if(_tmpDAB->tag != 6)goto
_LL697;else{_tmpDAC=_tmpDAB->f1;_tmpDAD=_tmpDAB->f2;_tmpDAE=_tmpDAB->f3;}}_LL696:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAC,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDAD,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDAE,te,
cvtenv);goto _LL68A;_LL697: {struct Cyc_Absyn_And_e_struct*_tmpDAF=(struct Cyc_Absyn_And_e_struct*)
_tmpDA4;if(_tmpDAF->tag != 7)goto _LL699;else{_tmpDB0=_tmpDAF->f1;_tmpDB1=_tmpDAF->f2;}}
_LL698: _tmpDB3=_tmpDB0;_tmpDB4=_tmpDB1;goto _LL69A;_LL699: {struct Cyc_Absyn_Or_e_struct*
_tmpDB2=(struct Cyc_Absyn_Or_e_struct*)_tmpDA4;if(_tmpDB2->tag != 8)goto _LL69B;
else{_tmpDB3=_tmpDB2->f1;_tmpDB4=_tmpDB2->f2;}}_LL69A: _tmpDB6=_tmpDB3;_tmpDB7=
_tmpDB4;goto _LL69C;_LL69B: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDB5=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDA4;if(_tmpDB5->tag != 9)goto _LL69D;else{_tmpDB6=_tmpDB5->f1;_tmpDB7=_tmpDB5->f2;}}
_LL69C: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB6,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB7,te,cvtenv);goto _LL68A;_LL69D: {
struct Cyc_Absyn_Cast_e_struct*_tmpDB8=(struct Cyc_Absyn_Cast_e_struct*)_tmpDA4;
if(_tmpDB8->tag != 15)goto _LL69F;else{_tmpDB9=(void*)_tmpDB8->f1;_tmpDBA=_tmpDB8->f2;}}
_LL69E: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBA,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDB9,1);goto
_LL68A;_LL69F: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDBB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDA4;if(_tmpDBB->tag != 20)goto _LL6A1;else{_tmpDBC=(void*)_tmpDBB->f1;}}_LL6A0:
_tmpDBE=_tmpDBC;goto _LL6A2;_LL6A1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDBD=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDA4;if(_tmpDBD->tag != 18)goto _LL6A3;
else{_tmpDBE=(void*)_tmpDBD->f1;}}_LL6A2: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDBE,1);goto _LL68A;_LL6A3: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDBF=(struct Cyc_Absyn_Valueof_e_struct*)_tmpDA4;if(_tmpDBF->tag != 39)goto
_LL6A5;else{_tmpDC0=(void*)_tmpDBF->f1;}}_LL6A4: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDC0,1);goto _LL68A;_LL6A5: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDC1=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDA4;if(_tmpDC1->tag != 19)goto
_LL6A7;else{_tmpDC2=_tmpDC1->f1;}}_LL6A6: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDC2,te,cvtenv);goto _LL68A;_LL6A7:;_LL6A8: {const char*_tmp1575;void*_tmp1574;(
_tmp1574=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1575="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1575,sizeof(char),66))),_tag_dyneither(_tmp1574,sizeof(void*),
0)));}_LL68A:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){struct Cyc_List_List*_tmpDC5=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDC7;struct
_tuple30 _tmpDC6=*((struct _tuple30*)vs->hd);_tmpDC7=_tmpDC6.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpDC5,_tmpDC7);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDC9;struct _tuple31 _tmpDC8=*((struct _tuple31*)evs->hd);_tmpDC9=_tmpDC8.f1;{
void*_tmpDCA=Cyc_Tcutil_compress(_tmpDC9);struct Cyc_Core_Opt*_tmpDCC;struct Cyc_Core_Opt**
_tmpDCD;_LL6AA: {struct Cyc_Absyn_Evar_struct*_tmpDCB=(struct Cyc_Absyn_Evar_struct*)
_tmpDCA;if(_tmpDCB->tag != 1)goto _LL6AC;else{_tmpDCC=_tmpDCB->f4;_tmpDCD=(struct
Cyc_Core_Opt**)& _tmpDCB->f4;}}_LL6AB: if(*_tmpDCD == 0){struct Cyc_Core_Opt*
_tmp1576;*_tmpDCD=((_tmp1576=_cycalloc(sizeof(*_tmp1576)),((_tmp1576->v=_tmpDC5,
_tmp1576))));}else{struct Cyc_List_List*_tmpDCF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDCD))->v;struct Cyc_List_List*_tmpDD0=0;for(0;_tmpDCF != 0;
_tmpDCF=_tmpDCF->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDC5,(struct Cyc_Absyn_Tvar*)_tmpDCF->hd)){struct Cyc_List_List*_tmp1577;
_tmpDD0=((_tmp1577=_cycalloc(sizeof(*_tmp1577)),((_tmp1577->hd=(struct Cyc_Absyn_Tvar*)
_tmpDCF->hd,((_tmp1577->tl=_tmpDD0,_tmp1577))))));}}{struct Cyc_Core_Opt*_tmp1578;*
_tmpDCD=((_tmp1578=_cycalloc(sizeof(*_tmp1578)),((_tmp1578->v=_tmpDD0,_tmp1578))));};}
goto _LL6A9;_LL6AC:;_LL6AD: goto _LL6A9;_LL6A9:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpDD3=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpDD4=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp1579;struct Cyc_Tcutil_CVTEnv _tmpDD5=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp1579.r=_tmpDD4,((_tmp1579.kind_env=_tmpDD3,((_tmp1579.free_vars=0,((_tmp1579.free_evars=
0,((_tmp1579.generalize_evars=generalize_evars,((_tmp1579.fn_result=0,_tmp1579)))))))))))),&
Cyc_Tcutil_tmk,t);struct Cyc_List_List*_tmpDD6=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpDD4,(struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,
_tmpDD5.free_vars);struct Cyc_List_List*_tmpDD7=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpDD4,(void*(*)(struct _tuple31*))Cyc_Core_fst,_tmpDD5.free_evars);if(_tmpDD3 != 
0){struct Cyc_List_List*_tmpDD8=0;{struct Cyc_List_List*_tmpDD9=_tmpDD6;for(0;(
unsigned int)_tmpDD9;_tmpDD9=_tmpDD9->tl){struct Cyc_Absyn_Tvar*_tmpDDA=(struct
Cyc_Absyn_Tvar*)_tmpDD9->hd;int found=0;{struct Cyc_List_List*_tmpDDB=_tmpDD3;for(
0;(unsigned int)_tmpDDB;_tmpDDB=_tmpDDB->tl){if(Cyc_Absyn_tvar_cmp(_tmpDDA,(
struct Cyc_Absyn_Tvar*)_tmpDDB->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp157A;_tmpDD8=((_tmp157A=_region_malloc(_tmpDD4,sizeof(*_tmp157A)),((_tmp157A->hd=(
struct Cyc_Absyn_Tvar*)_tmpDD9->hd,((_tmp157A->tl=_tmpDD8,_tmp157A))))));}}}
_tmpDD6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpDD8);}{struct Cyc_List_List*x=_tmpDD6;for(0;x != 0;x=x->tl){void*_tmpDDD=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDDF;struct Cyc_Core_Opt**
_tmpDE0;struct Cyc_Core_Opt*_tmpDE2;struct Cyc_Core_Opt**_tmpDE3;struct Cyc_Absyn_Kind*
_tmpDE4;struct Cyc_Absyn_Kind _tmpDE5;enum Cyc_Absyn_KindQual _tmpDE6;enum Cyc_Absyn_AliasQual
_tmpDE7;struct Cyc_Core_Opt*_tmpDE9;struct Cyc_Core_Opt**_tmpDEA;struct Cyc_Absyn_Kind*
_tmpDEB;struct Cyc_Absyn_Kind _tmpDEC;enum Cyc_Absyn_KindQual _tmpDED;enum Cyc_Absyn_AliasQual
_tmpDEE;struct Cyc_Core_Opt*_tmpDF0;struct Cyc_Core_Opt**_tmpDF1;struct Cyc_Absyn_Kind*
_tmpDF2;struct Cyc_Absyn_Kind _tmpDF3;enum Cyc_Absyn_KindQual _tmpDF4;enum Cyc_Absyn_AliasQual
_tmpDF5;struct Cyc_Core_Opt*_tmpDF7;struct Cyc_Core_Opt**_tmpDF8;struct Cyc_Absyn_Kind*
_tmpDF9;struct Cyc_Absyn_Kind _tmpDFA;enum Cyc_Absyn_KindQual _tmpDFB;enum Cyc_Absyn_AliasQual
_tmpDFC;struct Cyc_Core_Opt*_tmpDFE;struct Cyc_Core_Opt**_tmpDFF;struct Cyc_Absyn_Kind*
_tmpE00;struct Cyc_Absyn_Kind _tmpE01;enum Cyc_Absyn_KindQual _tmpE02;enum Cyc_Absyn_AliasQual
_tmpE03;struct Cyc_Core_Opt*_tmpE05;struct Cyc_Core_Opt**_tmpE06;struct Cyc_Absyn_Kind*
_tmpE07;struct Cyc_Absyn_Kind*_tmpE09;struct Cyc_Absyn_Kind _tmpE0A;enum Cyc_Absyn_KindQual
_tmpE0B;enum Cyc_Absyn_AliasQual _tmpE0C;_LL6AF: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDDE=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDDD;if(_tmpDDE->tag != 1)goto
_LL6B1;else{_tmpDDF=_tmpDDE->f1;_tmpDE0=(struct Cyc_Core_Opt**)& _tmpDDE->f1;}}
_LL6B0: _tmpDE3=_tmpDE0;goto _LL6B2;_LL6B1: {struct Cyc_Absyn_Less_kb_struct*
_tmpDE1=(struct Cyc_Absyn_Less_kb_struct*)_tmpDDD;if(_tmpDE1->tag != 2)goto _LL6B3;
else{_tmpDE2=_tmpDE1->f1;_tmpDE3=(struct Cyc_Core_Opt**)& _tmpDE1->f1;_tmpDE4=
_tmpDE1->f2;_tmpDE5=*_tmpDE4;_tmpDE6=_tmpDE5.kind;if(_tmpDE6 != Cyc_Absyn_BoxKind)
goto _LL6B3;_tmpDE7=_tmpDE5.aliasqual;if(_tmpDE7 != Cyc_Absyn_Top)goto _LL6B3;}}
_LL6B2: _tmpDEA=_tmpDE3;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Less_kb_struct*
_tmpDE8=(struct Cyc_Absyn_Less_kb_struct*)_tmpDDD;if(_tmpDE8->tag != 2)goto _LL6B5;
else{_tmpDE9=_tmpDE8->f1;_tmpDEA=(struct Cyc_Core_Opt**)& _tmpDE8->f1;_tmpDEB=
_tmpDE8->f2;_tmpDEC=*_tmpDEB;_tmpDED=_tmpDEC.kind;if(_tmpDED != Cyc_Absyn_MemKind)
goto _LL6B5;_tmpDEE=_tmpDEC.aliasqual;if(_tmpDEE != Cyc_Absyn_Top)goto _LL6B5;}}
_LL6B4: _tmpDF1=_tmpDEA;goto _LL6B6;_LL6B5: {struct Cyc_Absyn_Less_kb_struct*
_tmpDEF=(struct Cyc_Absyn_Less_kb_struct*)_tmpDDD;if(_tmpDEF->tag != 2)goto _LL6B7;
else{_tmpDF0=_tmpDEF->f1;_tmpDF1=(struct Cyc_Core_Opt**)& _tmpDEF->f1;_tmpDF2=
_tmpDEF->f2;_tmpDF3=*_tmpDF2;_tmpDF4=_tmpDF3.kind;if(_tmpDF4 != Cyc_Absyn_MemKind)
goto _LL6B7;_tmpDF5=_tmpDF3.aliasqual;if(_tmpDF5 != Cyc_Absyn_Aliasable)goto _LL6B7;}}
_LL6B6:*_tmpDF1=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6AE;_LL6B7: {
struct Cyc_Absyn_Less_kb_struct*_tmpDF6=(struct Cyc_Absyn_Less_kb_struct*)_tmpDDD;
if(_tmpDF6->tag != 2)goto _LL6B9;else{_tmpDF7=_tmpDF6->f1;_tmpDF8=(struct Cyc_Core_Opt**)&
_tmpDF6->f1;_tmpDF9=_tmpDF6->f2;_tmpDFA=*_tmpDF9;_tmpDFB=_tmpDFA.kind;if(_tmpDFB
!= Cyc_Absyn_MemKind)goto _LL6B9;_tmpDFC=_tmpDFA.aliasqual;if(_tmpDFC != Cyc_Absyn_Unique)
goto _LL6B9;}}_LL6B8:*_tmpDF8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6AE;_LL6B9: {struct Cyc_Absyn_Less_kb_struct*_tmpDFD=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDDD;if(_tmpDFD->tag != 2)goto _LL6BB;else{_tmpDFE=_tmpDFD->f1;_tmpDFF=(struct
Cyc_Core_Opt**)& _tmpDFD->f1;_tmpE00=_tmpDFD->f2;_tmpE01=*_tmpE00;_tmpE02=_tmpE01.kind;
if(_tmpE02 != Cyc_Absyn_RgnKind)goto _LL6BB;_tmpE03=_tmpE01.aliasqual;if(_tmpE03 != 
Cyc_Absyn_Top)goto _LL6BB;}}_LL6BA:*_tmpDFF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6AE;_LL6BB: {struct Cyc_Absyn_Less_kb_struct*_tmpE04=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDDD;if(_tmpE04->tag != 2)goto _LL6BD;else{_tmpE05=_tmpE04->f1;_tmpE06=(struct
Cyc_Core_Opt**)& _tmpE04->f1;_tmpE07=_tmpE04->f2;}}_LL6BC:*_tmpE06=Cyc_Tcutil_kind_to_bound_opt(
_tmpE07);goto _LL6AE;_LL6BD: {struct Cyc_Absyn_Eq_kb_struct*_tmpE08=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDDD;if(_tmpE08->tag != 0)goto _LL6BF;else{_tmpE09=_tmpE08->f1;_tmpE0A=*_tmpE09;
_tmpE0B=_tmpE0A.kind;if(_tmpE0B != Cyc_Absyn_MemKind)goto _LL6BF;_tmpE0C=_tmpE0A.aliasqual;}}
_LL6BE:{const char*_tmp1581;void*_tmp1580[2];struct Cyc_String_pa_struct _tmp157F;
struct Cyc_Absyn_Kind*_tmp157E;struct Cyc_String_pa_struct _tmp157D;(_tmp157D.tag=0,((
_tmp157D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp157E=_cycalloc_atomic(sizeof(*_tmp157E)),((_tmp157E->kind=Cyc_Absyn_MemKind,((
_tmp157E->aliasqual=_tmpE0C,_tmp157E)))))))),((_tmp157F.tag=0,((_tmp157F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1580[0]=& _tmp157F,((_tmp1580[1]=& _tmp157D,Cyc_Tcutil_terr(loc,((
_tmp1581="type variable %s cannot have kind %s",_tag_dyneither(_tmp1581,sizeof(
char),37))),_tag_dyneither(_tmp1580,sizeof(void*),2)))))))))))));}goto _LL6AE;
_LL6BF:;_LL6C0: goto _LL6AE;_LL6AE:;}}if(_tmpDD6 != 0  || _tmpDD7 != 0){{void*_tmpE12=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE14;struct Cyc_List_List*_tmpE15;
struct Cyc_List_List**_tmpE16;struct Cyc_Core_Opt*_tmpE17;struct Cyc_Absyn_Tqual
_tmpE18;void*_tmpE19;struct Cyc_List_List*_tmpE1A;int _tmpE1B;struct Cyc_Absyn_VarargInfo*
_tmpE1C;struct Cyc_List_List*_tmpE1D;struct Cyc_List_List*_tmpE1E;_LL6C2: {struct
Cyc_Absyn_FnType_struct*_tmpE13=(struct Cyc_Absyn_FnType_struct*)_tmpE12;if(
_tmpE13->tag != 9)goto _LL6C4;else{_tmpE14=_tmpE13->f1;_tmpE15=_tmpE14.tvars;
_tmpE16=(struct Cyc_List_List**)&(_tmpE13->f1).tvars;_tmpE17=_tmpE14.effect;
_tmpE18=_tmpE14.ret_tqual;_tmpE19=_tmpE14.ret_typ;_tmpE1A=_tmpE14.args;_tmpE1B=
_tmpE14.c_varargs;_tmpE1C=_tmpE14.cyc_varargs;_tmpE1D=_tmpE14.rgn_po;_tmpE1E=
_tmpE14.attributes;}}_LL6C3: if(*_tmpE16 == 0){*_tmpE16=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDD6);_tmpDD6=0;}goto _LL6C1;_LL6C4:;
_LL6C5: goto _LL6C1;_LL6C1:;}if(_tmpDD6 != 0){const char*_tmp1585;void*_tmp1584[1];
struct Cyc_String_pa_struct _tmp1583;(_tmp1583.tag=0,((_tmp1583.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDD6->hd)->name),((
_tmp1584[0]=& _tmp1583,Cyc_Tcutil_terr(loc,((_tmp1585="unbound type variable %s",
_tag_dyneither(_tmp1585,sizeof(char),25))),_tag_dyneither(_tmp1584,sizeof(void*),
1)))))));}if(_tmpDD7 != 0)for(0;_tmpDD7 != 0;_tmpDD7=_tmpDD7->tl){void*e=(void*)
_tmpDD7->hd;struct Cyc_Absyn_Kind*_tmpE22=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE23;enum Cyc_Absyn_KindQual _tmpE24;enum Cyc_Absyn_AliasQual _tmpE25;struct Cyc_Absyn_Kind
_tmpE26;enum Cyc_Absyn_KindQual _tmpE27;enum Cyc_Absyn_AliasQual _tmpE28;struct Cyc_Absyn_Kind
_tmpE29;enum Cyc_Absyn_KindQual _tmpE2A;enum Cyc_Absyn_AliasQual _tmpE2B;struct Cyc_Absyn_Kind
_tmpE2C;enum Cyc_Absyn_KindQual _tmpE2D;_LL6C7: _tmpE23=*_tmpE22;_tmpE24=_tmpE23.kind;
if(_tmpE24 != Cyc_Absyn_RgnKind)goto _LL6C9;_tmpE25=_tmpE23.aliasqual;if(_tmpE25 != 
Cyc_Absyn_Unique)goto _LL6C9;_LL6C8: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp1588;void*_tmp1587;(_tmp1587=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1588="can't unify evar with unique region!",
_tag_dyneither(_tmp1588,sizeof(char),37))),_tag_dyneither(_tmp1587,sizeof(void*),
0)));}goto _LL6C6;_LL6C9: _tmpE26=*_tmpE22;_tmpE27=_tmpE26.kind;if(_tmpE27 != Cyc_Absyn_RgnKind)
goto _LL6CB;_tmpE28=_tmpE26.aliasqual;if(_tmpE28 != Cyc_Absyn_Aliasable)goto _LL6CB;
_LL6CA: goto _LL6CC;_LL6CB: _tmpE29=*_tmpE22;_tmpE2A=_tmpE29.kind;if(_tmpE2A != Cyc_Absyn_RgnKind)
goto _LL6CD;_tmpE2B=_tmpE29.aliasqual;if(_tmpE2B != Cyc_Absyn_Top)goto _LL6CD;
_LL6CC: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp158B;
void*_tmp158A;(_tmp158A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp158B="can't unify evar with heap!",_tag_dyneither(
_tmp158B,sizeof(char),28))),_tag_dyneither(_tmp158A,sizeof(void*),0)));}goto
_LL6C6;_LL6CD: _tmpE2C=*_tmpE22;_tmpE2D=_tmpE2C.kind;if(_tmpE2D != Cyc_Absyn_EffKind)
goto _LL6CF;_LL6CE: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp158E;void*_tmp158D;(_tmp158D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp158E="can't unify evar with {}!",
_tag_dyneither(_tmp158E,sizeof(char),26))),_tag_dyneither(_tmp158D,sizeof(void*),
0)));}goto _LL6C6;_LL6CF:;_LL6D0:{const char*_tmp1593;void*_tmp1592[2];struct Cyc_String_pa_struct
_tmp1591;struct Cyc_String_pa_struct _tmp1590;(_tmp1590.tag=0,((_tmp1590.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp1591.tag=0,((_tmp1591.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp1592[0]=& _tmp1591,((_tmp1592[1]=& _tmp1590,Cyc_Tcutil_terr(loc,((
_tmp1593="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp1593,sizeof(char),52))),_tag_dyneither(_tmp1592,sizeof(void*),2)))))))))))));}
goto _LL6C6;_LL6C6:;}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE39=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE3B;struct Cyc_List_List*_tmpE3C;struct Cyc_Core_Opt*
_tmpE3D;struct Cyc_Absyn_Tqual _tmpE3E;void*_tmpE3F;struct Cyc_List_List*_tmpE40;
_LL6D2: {struct Cyc_Absyn_FnType_struct*_tmpE3A=(struct Cyc_Absyn_FnType_struct*)
_tmpE39;if(_tmpE3A->tag != 9)goto _LL6D4;else{_tmpE3B=_tmpE3A->f1;_tmpE3C=_tmpE3B.tvars;
_tmpE3D=_tmpE3B.effect;_tmpE3E=_tmpE3B.ret_tqual;_tmpE3F=_tmpE3B.ret_typ;_tmpE40=
_tmpE3B.args;}}_LL6D3: fd->tvs=_tmpE3C;fd->effect=_tmpE3D;{struct Cyc_List_List*
_tmpE41=fd->args;for(0;_tmpE41 != 0;(_tmpE41=_tmpE41->tl,_tmpE40=_tmpE40->tl)){(*((
struct _tuple26*)_tmpE41->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmpE40))->hd)).f2;(*((struct _tuple26*)_tmpE41->hd)).f3=(*((struct
_tuple10*)_tmpE40->hd)).f3;}}fd->ret_tqual=_tmpE3E;fd->ret_type=_tmpE3F;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE3F);
goto _LL6D1;_LL6D4:;_LL6D5: {const char*_tmp1596;void*_tmp1595;(_tmp1595=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1596="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp1596,sizeof(char),38))),_tag_dyneither(_tmp1595,sizeof(void*),
0)));}_LL6D1:;}{struct _RegionHandle*_tmpE44=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp1597;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple26*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpE44,(struct _dyneither_ptr*(*)(struct _tuple26*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp1597="function declaration has repeated parameter",
_tag_dyneither(_tmp1597,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp1598;fd->cached_typ=((
_tmp1598=_cycalloc(sizeof(*_tmp1598)),((_tmp1598->v=t,_tmp1598))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpE47=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp1599;
struct Cyc_Tcutil_CVTEnv _tmpE48=Cyc_Tcutil_check_valid_type(loc,te,((_tmp1599.r=
_tmpE47,((_tmp1599.kind_env=bound_tvars,((_tmp1599.free_vars=0,((_tmp1599.free_evars=
0,((_tmp1599.generalize_evars=0,((_tmp1599.fn_result=0,_tmp1599)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE49=Cyc_Tcutil_remove_bound_tvars(
_tmpE47,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE47,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple30*))Cyc_Core_fst,_tmpE48.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE4A=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple31*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpE47,(void*(*)(struct _tuple31*))Cyc_Core_fst,
_tmpE48.free_evars);{struct Cyc_List_List*fs=_tmpE49;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE4B=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp159E;void*_tmp159D[2];struct Cyc_String_pa_struct _tmp159C;struct Cyc_String_pa_struct
_tmp159B;(_tmp159B.tag=0,((_tmp159B.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp159C.tag=0,((_tmp159C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE4B),((_tmp159D[0]=& _tmp159C,((
_tmp159D[1]=& _tmp159B,Cyc_Tcutil_terr(loc,((_tmp159E="unbound type variable %s in type %s",
_tag_dyneither(_tmp159E,sizeof(char),36))),_tag_dyneither(_tmp159D,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE4A != 0)for(0;_tmpE4A != 0;_tmpE4A=
_tmpE4A->tl){void*e=(void*)_tmpE4A->hd;struct Cyc_Absyn_Kind*_tmpE50=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE51;enum Cyc_Absyn_KindQual _tmpE52;enum Cyc_Absyn_AliasQual
_tmpE53;struct Cyc_Absyn_Kind _tmpE54;enum Cyc_Absyn_KindQual _tmpE55;enum Cyc_Absyn_AliasQual
_tmpE56;struct Cyc_Absyn_Kind _tmpE57;enum Cyc_Absyn_KindQual _tmpE58;enum Cyc_Absyn_AliasQual
_tmpE59;struct Cyc_Absyn_Kind _tmpE5A;enum Cyc_Absyn_KindQual _tmpE5B;_LL6D7:
_tmpE51=*_tmpE50;_tmpE52=_tmpE51.kind;if(_tmpE52 != Cyc_Absyn_RgnKind)goto _LL6D9;
_tmpE53=_tmpE51.aliasqual;if(_tmpE53 != Cyc_Absyn_Unique)goto _LL6D9;_LL6D8: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp15A1;void*
_tmp15A0;(_tmp15A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15A1="can't unify evar with unique region!",
_tag_dyneither(_tmp15A1,sizeof(char),37))),_tag_dyneither(_tmp15A0,sizeof(void*),
0)));}goto _LL6D6;_LL6D9: _tmpE54=*_tmpE50;_tmpE55=_tmpE54.kind;if(_tmpE55 != Cyc_Absyn_RgnKind)
goto _LL6DB;_tmpE56=_tmpE54.aliasqual;if(_tmpE56 != Cyc_Absyn_Aliasable)goto _LL6DB;
_LL6DA: goto _LL6DC;_LL6DB: _tmpE57=*_tmpE50;_tmpE58=_tmpE57.kind;if(_tmpE58 != Cyc_Absyn_RgnKind)
goto _LL6DD;_tmpE59=_tmpE57.aliasqual;if(_tmpE59 != Cyc_Absyn_Top)goto _LL6DD;
_LL6DC: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp15A4;
void*_tmp15A3;(_tmp15A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15A4="can't unify evar with heap!",_tag_dyneither(
_tmp15A4,sizeof(char),28))),_tag_dyneither(_tmp15A3,sizeof(void*),0)));}goto
_LL6D6;_LL6DD: _tmpE5A=*_tmpE50;_tmpE5B=_tmpE5A.kind;if(_tmpE5B != Cyc_Absyn_EffKind)
goto _LL6DF;_LL6DE: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15A7;void*_tmp15A6;(_tmp15A6=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A7="can't unify evar with {}!",
_tag_dyneither(_tmp15A7,sizeof(char),26))),_tag_dyneither(_tmp15A6,sizeof(void*),
0)));}goto _LL6D6;_LL6DF:;_LL6E0:{const char*_tmp15AC;void*_tmp15AB[2];struct Cyc_String_pa_struct
_tmp15AA;struct Cyc_String_pa_struct _tmp15A9;(_tmp15A9.tag=0,((_tmp15A9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15AA.tag=0,((_tmp15AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15AB[0]=& _tmp15AA,((_tmp15AB[1]=& _tmp15A9,Cyc_Tcutil_terr(loc,((
_tmp15AC="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15AC,sizeof(char),52))),_tag_dyneither(_tmp15AB,sizeof(void*),2)))))))))))));}
goto _LL6D6;_LL6D6:;}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp15B1;void*_tmp15B0[2];struct Cyc_String_pa_struct
_tmp15AF;struct Cyc_String_pa_struct _tmp15AE;(_tmp15AE.tag=0,((_tmp15AE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15AF.tag=
0,((_tmp15AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15B0[0]=&
_tmp15AF,((_tmp15B0[1]=& _tmp15AE,Cyc_Tcutil_terr(loc,((_tmp15B1="%s: %s",
_tag_dyneither(_tmp15B1,sizeof(char),7))),_tag_dyneither(_tmp15B0,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(
struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr
msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmp15B2;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15B2="duplicate type variable",
_tag_dyneither(_tmp15B2,sizeof(char),24))));}struct _tuple33{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple34{struct Cyc_List_List*f1;void*f2;};struct _tuple35{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmp15B3;if(Cyc_strcmp((struct _dyneither_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp15B3="",_tag_dyneither(_tmp15B3,
sizeof(char),1))))!= 0){struct _tuple33*_tmp15B6;struct Cyc_List_List*_tmp15B5;
fields=((_tmp15B5=_cycalloc(sizeof(*_tmp15B5)),((_tmp15B5->hd=((_tmp15B6=
_cycalloc(sizeof(*_tmp15B6)),((_tmp15B6->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmp15B6->f2=0,_tmp15B6)))))),((_tmp15B5->tl=fields,_tmp15B5))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
const char*_tmp15B8;const char*_tmp15B7;struct _dyneither_ptr aggr_str=aggr_kind == 
Cyc_Absyn_StructA?(_tmp15B8="struct",_tag_dyneither(_tmp15B8,sizeof(char),7)):((
_tmp15B7="union",_tag_dyneither(_tmp15B7,sizeof(char),6)));struct Cyc_List_List*
ans=0;for(0;des != 0;des=des->tl){struct _tuple34 _tmpE70;struct Cyc_List_List*
_tmpE71;void*_tmpE72;struct _tuple34*_tmpE6F=(struct _tuple34*)des->hd;_tmpE70=*
_tmpE6F;_tmpE71=_tmpE70.f1;_tmpE72=_tmpE70.f2;if(_tmpE71 == 0){struct Cyc_List_List*
_tmpE73=fields;for(0;_tmpE73 != 0;_tmpE73=_tmpE73->tl){if(!(*((struct _tuple33*)
_tmpE73->hd)).f2){(*((struct _tuple33*)_tmpE73->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*
_tmp15BE;struct Cyc_Absyn_FieldName_struct _tmp15BD;struct Cyc_List_List*_tmp15BC;(*((
struct _tuple34*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15BC=_cycalloc(sizeof(*
_tmp15BC)),((_tmp15BC->hd=(void*)((_tmp15BE=_cycalloc(sizeof(*_tmp15BE)),((
_tmp15BE[0]=((_tmp15BD.tag=1,((_tmp15BD.f1=((*((struct _tuple33*)_tmpE73->hd)).f1)->name,
_tmp15BD)))),_tmp15BE)))),((_tmp15BC->tl=0,_tmp15BC))))));}{struct _tuple35*
_tmp15C1;struct Cyc_List_List*_tmp15C0;ans=((_tmp15C0=_region_malloc(rgn,sizeof(*
_tmp15C0)),((_tmp15C0->hd=((_tmp15C1=_region_malloc(rgn,sizeof(*_tmp15C1)),((
_tmp15C1->f1=(*((struct _tuple33*)_tmpE73->hd)).f1,((_tmp15C1->f2=_tmpE72,
_tmp15C1)))))),((_tmp15C0->tl=ans,_tmp15C0))))));}break;}}if(_tmpE73 == 0){const
char*_tmp15C5;void*_tmp15C4[1];struct Cyc_String_pa_struct _tmp15C3;(_tmp15C3.tag=
0,((_tmp15C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15C4[0]=& _tmp15C3,Cyc_Tcutil_terr(loc,((_tmp15C5="too many arguments to %s",
_tag_dyneither(_tmp15C5,sizeof(char),25))),_tag_dyneither(_tmp15C4,sizeof(void*),
1)))))));}}else{if(_tmpE71->tl != 0){const char*_tmp15C8;void*_tmp15C7;(_tmp15C7=0,
Cyc_Tcutil_terr(loc,((_tmp15C8="multiple designators are not yet supported",
_tag_dyneither(_tmp15C8,sizeof(char),43))),_tag_dyneither(_tmp15C7,sizeof(void*),
0)));}else{void*_tmpE7E=(void*)_tmpE71->hd;struct _dyneither_ptr*_tmpE81;_LL6E2: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE7F=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE7E;if(_tmpE7F->tag != 0)goto _LL6E4;}_LL6E3:{const char*_tmp15CC;void*_tmp15CB[
1];struct Cyc_String_pa_struct _tmp15CA;(_tmp15CA.tag=0,((_tmp15CA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15CB[0]=& _tmp15CA,Cyc_Tcutil_terr(
loc,((_tmp15CC="array designator used in argument to %s",_tag_dyneither(_tmp15CC,
sizeof(char),40))),_tag_dyneither(_tmp15CB,sizeof(void*),1)))))));}goto _LL6E1;
_LL6E4: {struct Cyc_Absyn_FieldName_struct*_tmpE80=(struct Cyc_Absyn_FieldName_struct*)
_tmpE7E;if(_tmpE80->tag != 1)goto _LL6E1;else{_tmpE81=_tmpE80->f1;}}_LL6E5: {
struct Cyc_List_List*_tmpE85=fields;for(0;_tmpE85 != 0;_tmpE85=_tmpE85->tl){if(Cyc_strptrcmp(
_tmpE81,((*((struct _tuple33*)_tmpE85->hd)).f1)->name)== 0){if((*((struct _tuple33*)
_tmpE85->hd)).f2){const char*_tmp15D0;void*_tmp15CF[1];struct Cyc_String_pa_struct
_tmp15CE;(_tmp15CE.tag=0,((_tmp15CE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE81),((_tmp15CF[0]=& _tmp15CE,Cyc_Tcutil_terr(loc,((_tmp15D0="member %s has already been used as an argument",
_tag_dyneither(_tmp15D0,sizeof(char),47))),_tag_dyneither(_tmp15CF,sizeof(void*),
1)))))));}(*((struct _tuple33*)_tmpE85->hd)).f2=1;{struct _tuple35*_tmp15D3;struct
Cyc_List_List*_tmp15D2;ans=((_tmp15D2=_region_malloc(rgn,sizeof(*_tmp15D2)),((
_tmp15D2->hd=((_tmp15D3=_region_malloc(rgn,sizeof(*_tmp15D3)),((_tmp15D3->f1=(*((
struct _tuple33*)_tmpE85->hd)).f1,((_tmp15D3->f2=_tmpE72,_tmp15D3)))))),((
_tmp15D2->tl=ans,_tmp15D2))))));}break;}}if(_tmpE85 == 0){const char*_tmp15D7;void*
_tmp15D6[1];struct Cyc_String_pa_struct _tmp15D5;(_tmp15D5.tag=0,((_tmp15D5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE81),((_tmp15D6[0]=& _tmp15D5,
Cyc_Tcutil_terr(loc,((_tmp15D7="bad field designator %s",_tag_dyneither(_tmp15D7,
sizeof(char),24))),_tag_dyneither(_tmp15D6,sizeof(void*),1)))))));}goto _LL6E1;}
_LL6E1:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple33*)fields->hd)).f2){{const char*_tmp15DA;void*_tmp15D9;(
_tmp15D9=0,Cyc_Tcutil_terr(loc,((_tmp15DA="too few arguments to struct",
_tag_dyneither(_tmp15DA,sizeof(char),28))),_tag_dyneither(_tmp15D9,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple33*)fields->hd)).f2){if(found){const char*_tmp15DD;void*_tmp15DC;(_tmp15DC=
0,Cyc_Tcutil_terr(loc,((_tmp15DD="only one member of a union is allowed",
_tag_dyneither(_tmp15DD,sizeof(char),38))),_tag_dyneither(_tmp15DC,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15E0;void*_tmp15DF;(_tmp15DF=0,Cyc_Tcutil_terr(
loc,((_tmp15E0="missing member for union",_tag_dyneither(_tmp15E0,sizeof(char),
25))),_tag_dyneither(_tmp15DF,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE96=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpE98;void*_tmpE99;struct Cyc_Absyn_PtrAtts _tmpE9A;union Cyc_Absyn_Constraint*
_tmpE9B;_LL6E7: {struct Cyc_Absyn_PointerType_struct*_tmpE97=(struct Cyc_Absyn_PointerType_struct*)
_tmpE96;if(_tmpE97->tag != 5)goto _LL6E9;else{_tmpE98=_tmpE97->f1;_tmpE99=_tmpE98.elt_typ;
_tmpE9A=_tmpE98.ptr_atts;_tmpE9B=_tmpE9A.bounds;}}_LL6E8: {void*_tmpE9C=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpE9B);_LL6EC: {struct Cyc_Absyn_DynEither_b_struct*_tmpE9D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE9C;if(_tmpE9D->tag != 0)goto _LL6EE;}_LL6ED:*elt_typ_dest=_tmpE99;return 1;
_LL6EE:;_LL6EF: return 0;_LL6EB:;}_LL6E9:;_LL6EA: return 0;_LL6E6:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpE9E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEA0;void*_tmpEA1;struct Cyc_Absyn_PtrAtts _tmpEA2;union Cyc_Absyn_Constraint*
_tmpEA3;_LL6F1: {struct Cyc_Absyn_PointerType_struct*_tmpE9F=(struct Cyc_Absyn_PointerType_struct*)
_tmpE9E;if(_tmpE9F->tag != 5)goto _LL6F3;else{_tmpEA0=_tmpE9F->f1;_tmpEA1=_tmpEA0.elt_typ;
_tmpEA2=_tmpEA0.ptr_atts;_tmpEA3=_tmpEA2.zero_term;}}_LL6F2:*elt_typ_dest=
_tmpEA1;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpEA3);_LL6F3:;_LL6F4: return 0;_LL6F0:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEA4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEA6;void*_tmpEA7;struct Cyc_Absyn_PtrAtts _tmpEA8;
union Cyc_Absyn_Constraint*_tmpEA9;union Cyc_Absyn_Constraint*_tmpEAA;struct Cyc_Absyn_ArrayInfo
_tmpEAC;void*_tmpEAD;struct Cyc_Absyn_Tqual _tmpEAE;struct Cyc_Absyn_Exp*_tmpEAF;
union Cyc_Absyn_Constraint*_tmpEB0;_LL6F6: {struct Cyc_Absyn_PointerType_struct*
_tmpEA5=(struct Cyc_Absyn_PointerType_struct*)_tmpEA4;if(_tmpEA5->tag != 5)goto
_LL6F8;else{_tmpEA6=_tmpEA5->f1;_tmpEA7=_tmpEA6.elt_typ;_tmpEA8=_tmpEA6.ptr_atts;
_tmpEA9=_tmpEA8.bounds;_tmpEAA=_tmpEA8.zero_term;}}_LL6F7: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEAA)){*ptr_type=t;*elt_type=
_tmpEA7;{void*_tmpEB1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpEA9);_LL6FD: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEB2=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEB1;if(_tmpEB2->tag != 0)goto
_LL6FF;}_LL6FE:*is_dyneither=1;goto _LL6FC;_LL6FF:;_LL700:*is_dyneither=0;goto
_LL6FC;_LL6FC:;}return 1;}else{return 0;}_LL6F8: {struct Cyc_Absyn_ArrayType_struct*
_tmpEAB=(struct Cyc_Absyn_ArrayType_struct*)_tmpEA4;if(_tmpEAB->tag != 8)goto
_LL6FA;else{_tmpEAC=_tmpEAB->f1;_tmpEAD=_tmpEAC.elt_type;_tmpEAE=_tmpEAC.tq;
_tmpEAF=_tmpEAC.num_elts;_tmpEB0=_tmpEAC.zero_term;}}_LL6F9: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEB0)){*elt_type=_tmpEAD;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp15F5;struct Cyc_Absyn_PtrAtts
_tmp15F4;struct Cyc_Absyn_Upper_b_struct _tmp15F3;struct Cyc_Absyn_Upper_b_struct*
_tmp15F2;struct Cyc_Absyn_PtrInfo _tmp15F1;struct Cyc_Absyn_PointerType_struct*
_tmp15F0;*ptr_type=(void*)((_tmp15F0=_cycalloc(sizeof(*_tmp15F0)),((_tmp15F0[0]=((
_tmp15F5.tag=5,((_tmp15F5.f1=((_tmp15F1.elt_typ=_tmpEAD,((_tmp15F1.elt_tq=
_tmpEAE,((_tmp15F1.ptr_atts=((_tmp15F4.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp15F4.nullable=Cyc_Absyn_false_conref,((_tmp15F4.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp15F2=_cycalloc(sizeof(*_tmp15F2)),((
_tmp15F2[0]=((_tmp15F3.tag=1,((_tmp15F3.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEAF),_tmp15F3)))),_tmp15F2))))),((_tmp15F4.zero_term=_tmpEB0,((_tmp15F4.ptrloc=
0,_tmp15F4)))))))))),_tmp15F1)))))),_tmp15F5)))),_tmp15F0))));}return 1;}else{
return 0;}_LL6FA:;_LL6FB: return 0;_LL6F5:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEB9=e1->r;
struct Cyc_Absyn_Exp*_tmpEBC;struct Cyc_Absyn_Exp*_tmpEBE;struct Cyc_Absyn_Exp*
_tmpEC0;struct Cyc_Absyn_Exp*_tmpEC2;struct Cyc_Absyn_Exp*_tmpEC4;struct Cyc_Absyn_Exp*
_tmpEC6;_LL702: {struct Cyc_Absyn_Cast_e_struct*_tmpEBA=(struct Cyc_Absyn_Cast_e_struct*)
_tmpEB9;if(_tmpEBA->tag != 15)goto _LL704;}_LL703: {const char*_tmp15F9;void*
_tmp15F8[1];struct Cyc_String_pa_struct _tmp15F7;(_tmp15F7.tag=0,((_tmp15F7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp15F8[0]=& _tmp15F7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15F9="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp15F9,sizeof(char),29))),_tag_dyneither(_tmp15F8,sizeof(void*),1)))))));}
_LL704: {struct Cyc_Absyn_Deref_e_struct*_tmpEBB=(struct Cyc_Absyn_Deref_e_struct*)
_tmpEB9;if(_tmpEBB->tag != 21)goto _LL706;else{_tmpEBC=_tmpEBB->f1;}}_LL705:
_tmpEBE=_tmpEBC;goto _LL707;_LL706: {struct Cyc_Absyn_Subscript_e_struct*_tmpEBD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpEB9;if(_tmpEBD->tag != 24)goto _LL708;
else{_tmpEBE=_tmpEBD->f1;}}_LL707: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpEBE->topt))->v,ptr_type,is_dyneither,
elt_type);_LL708: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpEBF=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEB9;if(_tmpEBF->tag != 23)goto _LL70A;else{_tmpEC0=_tmpEBF->f1;}}_LL709:
_tmpEC2=_tmpEC0;goto _LL70B;_LL70A: {struct Cyc_Absyn_AggrMember_e_struct*_tmpEC1=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpEB9;if(_tmpEC1->tag != 22)goto _LL70C;
else{_tmpEC2=_tmpEC1->f1;}}_LL70B: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpEC2->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp15FD;void*_tmp15FC[1];struct Cyc_String_pa_struct _tmp15FB;(_tmp15FB.tag=
0,((_tmp15FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp15FC[0]=& _tmp15FB,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15FD="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp15FD,sizeof(char),51))),_tag_dyneither(_tmp15FC,sizeof(void*),
1)))))));}return 0;_LL70C: {struct Cyc_Absyn_Instantiate_e_struct*_tmpEC3=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpEB9;if(_tmpEC3->tag != 14)goto _LL70E;else{
_tmpEC4=_tmpEC3->f1;}}_LL70D: _tmpEC6=_tmpEC4;goto _LL70F;_LL70E: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpEC5=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpEB9;if(_tmpEC5->tag != 13)
goto _LL710;else{_tmpEC6=_tmpEC5->f1;}}_LL70F: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEC6->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1601;void*_tmp1600[1];struct Cyc_String_pa_struct
_tmp15FF;(_tmp15FF.tag=0,((_tmp15FF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1600[0]=& _tmp15FF,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1601="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1601,sizeof(char),49))),_tag_dyneither(_tmp1600,sizeof(void*),
1)))))));}return 0;_LL710: {struct Cyc_Absyn_Var_e_struct*_tmpEC7=(struct Cyc_Absyn_Var_e_struct*)
_tmpEB9;if(_tmpEC7->tag != 1)goto _LL712;}_LL711: return 0;_LL712:;_LL713: {const
char*_tmp1605;void*_tmp1604[1];struct Cyc_String_pa_struct _tmp1603;(_tmp1603.tag=
0,((_tmp1603.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1604[0]=& _tmp1603,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1605="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1605,sizeof(char),39))),_tag_dyneither(_tmp1604,sizeof(void*),
1)))))));}_LL701:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpED4=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpED8;_LL715: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpED5=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpED4;if(_tmpED5->tag != 22)goto
_LL717;}_LL716: return !must_be_unique;_LL717: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpED6=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpED4;if(_tmpED6->tag != 21)goto
_LL719;}_LL718: return 1;_LL719: {struct Cyc_Absyn_VarType_struct*_tmpED7=(struct
Cyc_Absyn_VarType_struct*)_tmpED4;if(_tmpED7->tag != 2)goto _LL71B;else{_tmpED8=
_tmpED7->f1;}}_LL71A: {struct Cyc_Absyn_Kind _tmpEDA;enum Cyc_Absyn_KindQual
_tmpEDB;enum Cyc_Absyn_AliasQual _tmpEDC;struct Cyc_Absyn_Kind*_tmpED9=Cyc_Tcutil_tvar_kind(
_tmpED8,& Cyc_Tcutil_rk);_tmpEDA=*_tmpED9;_tmpEDB=_tmpEDA.kind;_tmpEDC=_tmpEDA.aliasqual;
if(_tmpEDB == Cyc_Absyn_RgnKind  && (_tmpEDC == Cyc_Absyn_Unique  || _tmpEDC == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpEDD=Cyc_Absyn_compress_kb(_tmpED8->kind);struct
Cyc_Core_Opt*_tmpEDF;struct Cyc_Core_Opt**_tmpEE0;struct Cyc_Absyn_Kind*_tmpEE1;
struct Cyc_Absyn_Kind _tmpEE2;enum Cyc_Absyn_KindQual _tmpEE3;enum Cyc_Absyn_AliasQual
_tmpEE4;_LL71E: {struct Cyc_Absyn_Less_kb_struct*_tmpEDE=(struct Cyc_Absyn_Less_kb_struct*)
_tmpEDD;if(_tmpEDE->tag != 2)goto _LL720;else{_tmpEDF=_tmpEDE->f1;_tmpEE0=(struct
Cyc_Core_Opt**)& _tmpEDE->f1;_tmpEE1=_tmpEDE->f2;_tmpEE2=*_tmpEE1;_tmpEE3=_tmpEE2.kind;
if(_tmpEE3 != Cyc_Absyn_RgnKind)goto _LL720;_tmpEE4=_tmpEE2.aliasqual;if(_tmpEE4 != 
Cyc_Absyn_Top)goto _LL720;}}_LL71F:{struct Cyc_Absyn_Less_kb_struct*_tmp160B;
struct Cyc_Absyn_Less_kb_struct _tmp160A;struct Cyc_Core_Opt*_tmp1609;*_tmpEE0=((
_tmp1609=_cycalloc(sizeof(*_tmp1609)),((_tmp1609->v=(void*)((_tmp160B=_cycalloc(
sizeof(*_tmp160B)),((_tmp160B[0]=((_tmp160A.tag=2,((_tmp160A.f1=0,((_tmp160A.f2=&
Cyc_Tcutil_rk,_tmp160A)))))),_tmp160B)))),_tmp1609))));}return 0;_LL720:;_LL721:
return 1;_LL71D:;}return 0;}_LL71B:;_LL71C: return 0;_LL714:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEE8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEEA;struct Cyc_Absyn_PtrAtts _tmpEEB;void*_tmpEEC;struct Cyc_Absyn_Tvar*_tmpEEE;
_LL723: {struct Cyc_Absyn_PointerType_struct*_tmpEE9=(struct Cyc_Absyn_PointerType_struct*)
_tmpEE8;if(_tmpEE9->tag != 5)goto _LL725;else{_tmpEEA=_tmpEE9->f1;_tmpEEB=_tmpEEA.ptr_atts;
_tmpEEC=_tmpEEB.rgn;}}_LL724: return Cyc_Tcutil_is_noalias_region(_tmpEEC,
must_be_unique);_LL725: {struct Cyc_Absyn_VarType_struct*_tmpEED=(struct Cyc_Absyn_VarType_struct*)
_tmpEE8;if(_tmpEED->tag != 2)goto _LL727;else{_tmpEEE=_tmpEED->f1;}}_LL726: {
struct Cyc_Absyn_Kind _tmpEF0;enum Cyc_Absyn_KindQual _tmpEF1;enum Cyc_Absyn_AliasQual
_tmpEF2;struct Cyc_Absyn_Kind*_tmpEEF=Cyc_Tcutil_tvar_kind(_tmpEEE,& Cyc_Tcutil_bk);
_tmpEF0=*_tmpEEF;_tmpEF1=_tmpEF0.kind;_tmpEF2=_tmpEF0.aliasqual;switch(_tmpEF1){
case Cyc_Absyn_BoxKind: _LL729: goto _LL72A;case Cyc_Absyn_AnyKind: _LL72A: goto _LL72B;
case Cyc_Absyn_MemKind: _LL72B: if(_tmpEF2 == Cyc_Absyn_Unique  || _tmpEF2 == Cyc_Absyn_Top){
void*_tmpEF3=Cyc_Absyn_compress_kb(_tmpEEE->kind);struct Cyc_Core_Opt*_tmpEF5;
struct Cyc_Core_Opt**_tmpEF6;struct Cyc_Absyn_Kind*_tmpEF7;struct Cyc_Absyn_Kind
_tmpEF8;enum Cyc_Absyn_KindQual _tmpEF9;enum Cyc_Absyn_AliasQual _tmpEFA;_LL72E: {
struct Cyc_Absyn_Less_kb_struct*_tmpEF4=(struct Cyc_Absyn_Less_kb_struct*)_tmpEF3;
if(_tmpEF4->tag != 2)goto _LL730;else{_tmpEF5=_tmpEF4->f1;_tmpEF6=(struct Cyc_Core_Opt**)&
_tmpEF4->f1;_tmpEF7=_tmpEF4->f2;_tmpEF8=*_tmpEF7;_tmpEF9=_tmpEF8.kind;_tmpEFA=
_tmpEF8.aliasqual;if(_tmpEFA != Cyc_Absyn_Top)goto _LL730;}}_LL72F:{struct Cyc_Absyn_Less_kb_struct*
_tmp1615;struct Cyc_Absyn_Kind*_tmp1614;struct Cyc_Absyn_Less_kb_struct _tmp1613;
struct Cyc_Core_Opt*_tmp1612;*_tmpEF6=((_tmp1612=_cycalloc(sizeof(*_tmp1612)),((
_tmp1612->v=(void*)((_tmp1615=_cycalloc(sizeof(*_tmp1615)),((_tmp1615[0]=((
_tmp1613.tag=2,((_tmp1613.f1=0,((_tmp1613.f2=((_tmp1614=_cycalloc_atomic(sizeof(*
_tmp1614)),((_tmp1614->kind=_tmpEF9,((_tmp1614->aliasqual=Cyc_Absyn_Aliasable,
_tmp1614)))))),_tmp1613)))))),_tmp1615)))),_tmp1612))));}return 0;_LL730:;_LL731:
return 1;_LL72D:;}return 0;default: _LL72C: return 0;}}_LL727:;_LL728: return 0;_LL722:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpEFF=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpEFF))
return 1;{void*_tmpF00=_tmpEFF;struct Cyc_List_List*_tmpF02;struct Cyc_Absyn_AggrInfo
_tmpF04;union Cyc_Absyn_AggrInfoU _tmpF05;struct Cyc_Absyn_Aggrdecl**_tmpF06;struct
Cyc_List_List*_tmpF07;struct Cyc_List_List*_tmpF09;struct Cyc_Absyn_AggrInfo
_tmpF0B;union Cyc_Absyn_AggrInfoU _tmpF0C;struct _tuple4 _tmpF0D;struct Cyc_Absyn_DatatypeInfo
_tmpF0F;union Cyc_Absyn_DatatypeInfoU _tmpF10;struct Cyc_List_List*_tmpF11;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF13;union Cyc_Absyn_DatatypeFieldInfoU _tmpF14;
struct Cyc_List_List*_tmpF15;_LL734: {struct Cyc_Absyn_TupleType_struct*_tmpF01=(
struct Cyc_Absyn_TupleType_struct*)_tmpF00;if(_tmpF01->tag != 10)goto _LL736;else{
_tmpF02=_tmpF01->f1;}}_LL735: while(_tmpF02 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple12*)_tmpF02->hd)).f2))return 1;_tmpF02=_tmpF02->tl;}return 0;
_LL736: {struct Cyc_Absyn_AggrType_struct*_tmpF03=(struct Cyc_Absyn_AggrType_struct*)
_tmpF00;if(_tmpF03->tag != 11)goto _LL738;else{_tmpF04=_tmpF03->f1;_tmpF05=_tmpF04.aggr_info;
if((_tmpF05.KnownAggr).tag != 2)goto _LL738;_tmpF06=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF05.KnownAggr).val;_tmpF07=_tmpF04.targs;}}_LL737: if((*_tmpF06)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF16=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpF06)->tvs,_tmpF07);struct Cyc_List_List*_tmpF17=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF06)->impl))->fields;void*t;
while(_tmpF17 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF16,((struct Cyc_Absyn_Aggrfield*)
_tmpF17->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF17=_tmpF17->tl;}return 0;}_LL738: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF08=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF00;if(_tmpF08->tag != 12)goto
_LL73A;else{_tmpF09=_tmpF08->f2;}}_LL739: while(_tmpF09 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF09->hd)->type))return 1;_tmpF09=_tmpF09->tl;}
return 0;_LL73A: {struct Cyc_Absyn_AggrType_struct*_tmpF0A=(struct Cyc_Absyn_AggrType_struct*)
_tmpF00;if(_tmpF0A->tag != 11)goto _LL73C;else{_tmpF0B=_tmpF0A->f1;_tmpF0C=_tmpF0B.aggr_info;
if((_tmpF0C.UnknownAggr).tag != 1)goto _LL73C;_tmpF0D=(struct _tuple4)(_tmpF0C.UnknownAggr).val;}}
_LL73B: {const char*_tmp1618;void*_tmp1617;(_tmp1617=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1618="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp1618,sizeof(char),36))),_tag_dyneither(_tmp1617,sizeof(void*),
0)));}_LL73C: {struct Cyc_Absyn_DatatypeType_struct*_tmpF0E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpF00;if(_tmpF0E->tag != 3)goto _LL73E;else{_tmpF0F=_tmpF0E->f1;_tmpF10=_tmpF0F.datatype_info;
_tmpF11=_tmpF0F.targs;}}_LL73D: {union Cyc_Absyn_DatatypeInfoU _tmpF1A=_tmpF10;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF1B;struct _tuple2*_tmpF1C;int _tmpF1D;
struct Cyc_Absyn_Datatypedecl**_tmpF1E;struct Cyc_Absyn_Datatypedecl*_tmpF1F;
struct Cyc_Absyn_Datatypedecl _tmpF20;struct Cyc_List_List*_tmpF21;struct Cyc_Core_Opt*
_tmpF22;_LL743: if((_tmpF1A.UnknownDatatype).tag != 1)goto _LL745;_tmpF1B=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF1A.UnknownDatatype).val;_tmpF1C=_tmpF1B.name;
_tmpF1D=_tmpF1B.is_extensible;_LL744: {const char*_tmp161B;void*_tmp161A;(
_tmp161A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp161B="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp161B,
sizeof(char),40))),_tag_dyneither(_tmp161A,sizeof(void*),0)));}_LL745: if((
_tmpF1A.KnownDatatype).tag != 2)goto _LL742;_tmpF1E=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF1A.KnownDatatype).val;_tmpF1F=*_tmpF1E;_tmpF20=*_tmpF1F;_tmpF21=_tmpF20.tvs;
_tmpF22=_tmpF20.fields;_LL746: return 0;_LL742:;}_LL73E: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF12=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpF00;if(_tmpF12->tag != 4)
goto _LL740;else{_tmpF13=_tmpF12->f1;_tmpF14=_tmpF13.field_info;_tmpF15=_tmpF13.targs;}}
_LL73F: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF25=_tmpF14;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF26;struct _tuple3 _tmpF27;struct Cyc_Absyn_Datatypedecl*_tmpF28;struct Cyc_Absyn_Datatypefield*
_tmpF29;_LL748: if((_tmpF25.UnknownDatatypefield).tag != 1)goto _LL74A;_tmpF26=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF25.UnknownDatatypefield).val;
_LL749: {const char*_tmp161E;void*_tmp161D;(_tmp161D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp161E="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp161E,sizeof(char),46))),_tag_dyneither(_tmp161D,sizeof(void*),
0)));}_LL74A: if((_tmpF25.KnownDatatypefield).tag != 2)goto _LL747;_tmpF27=(struct
_tuple3)(_tmpF25.KnownDatatypefield).val;_tmpF28=_tmpF27.f1;_tmpF29=_tmpF27.f2;
_LL74B: {struct Cyc_List_List*_tmpF2C=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF28->tvs,_tmpF15);struct Cyc_List_List*_tmpF2D=
_tmpF29->typs;while(_tmpF2D != 0){_tmpEFF=Cyc_Tcutil_rsubstitute(rgn,_tmpF2C,(*((
struct _tuple12*)_tmpF2D->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpEFF))return 1;_tmpF2D=_tmpF2D->tl;}return 0;}_LL747:;}_LL740:;_LL741: return 0;
_LL733:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF2E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF30;union Cyc_Absyn_AggrInfoU _tmpF31;struct Cyc_Absyn_Aggrdecl**_tmpF32;struct
Cyc_Absyn_Aggrdecl*_tmpF33;struct Cyc_List_List*_tmpF34;struct Cyc_List_List*
_tmpF36;_LL74D: {struct Cyc_Absyn_AggrType_struct*_tmpF2F=(struct Cyc_Absyn_AggrType_struct*)
_tmpF2E;if(_tmpF2F->tag != 11)goto _LL74F;else{_tmpF30=_tmpF2F->f1;_tmpF31=_tmpF30.aggr_info;
if((_tmpF31.KnownAggr).tag != 2)goto _LL74F;_tmpF32=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF31.KnownAggr).val;_tmpF33=*_tmpF32;_tmpF34=_tmpF30.targs;}}_LL74E: _tmpF36=
_tmpF33->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF33->impl))->fields;
goto _LL750;_LL74F: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF35=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF2E;if(_tmpF35->tag != 12)goto _LL751;else{_tmpF36=_tmpF35->f2;}}_LL750: {
struct Cyc_Absyn_Aggrfield*_tmpF37=Cyc_Absyn_lookup_field(_tmpF36,f);{struct Cyc_Absyn_Aggrfield*
_tmpF38=_tmpF37;struct Cyc_Absyn_Aggrfield _tmpF39;void*_tmpF3A;_LL754: if(_tmpF38
!= 0)goto _LL756;_LL755: {const char*_tmp1621;void*_tmp1620;(_tmp1620=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1621="is_noalias_field: missing field",
_tag_dyneither(_tmp1621,sizeof(char),32))),_tag_dyneither(_tmp1620,sizeof(void*),
0)));}_LL756: if(_tmpF38 == 0)goto _LL753;_tmpF39=*_tmpF38;_tmpF3A=_tmpF39.type;
_LL757: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF3A);_LL753:;}return 0;}
_LL751:;_LL752: {const char*_tmp1625;void*_tmp1624[1];struct Cyc_String_pa_struct
_tmp1623;(_tmp1623.tag=0,((_tmp1623.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1624[0]=& _tmp1623,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1625="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1625,sizeof(char),36))),_tag_dyneither(_tmp1624,sizeof(void*),
1)))))));}_LL74C:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF40=e->r;void*_tmpF42;struct Cyc_Absyn_Exp*
_tmpF45;struct Cyc_Absyn_Exp*_tmpF47;struct Cyc_Absyn_Exp*_tmpF49;struct
_dyneither_ptr*_tmpF4A;struct Cyc_Absyn_Exp*_tmpF4C;struct Cyc_Absyn_Exp*_tmpF4D;
struct Cyc_Absyn_Exp*_tmpF4F;struct Cyc_Absyn_Exp*_tmpF50;struct Cyc_Absyn_Exp*
_tmpF52;struct Cyc_Absyn_Exp*_tmpF54;struct Cyc_Absyn_Stmt*_tmpF56;_LL759: {struct
Cyc_Absyn_Var_e_struct*_tmpF41=(struct Cyc_Absyn_Var_e_struct*)_tmpF40;if(_tmpF41->tag
!= 1)goto _LL75B;else{_tmpF42=(void*)_tmpF41->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF43=(struct Cyc_Absyn_Global_b_struct*)_tmpF42;if(_tmpF43->tag != 1)goto _LL75B;};}}
_LL75A: return 0;_LL75B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF44=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF40;if(_tmpF44->tag != 23)goto _LL75D;else{_tmpF45=_tmpF44->f1;}}_LL75C:
_tmpF47=_tmpF45;goto _LL75E;_LL75D: {struct Cyc_Absyn_Deref_e_struct*_tmpF46=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF40;if(_tmpF46->tag != 21)goto _LL75F;else{
_tmpF47=_tmpF46->f1;}}_LL75E: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF47->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF47);_LL75F: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF48=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF40;if(_tmpF48->tag != 22)goto _LL761;else{_tmpF49=_tmpF48->f1;_tmpF4A=_tmpF48->f2;}}
_LL760: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF49);_LL761: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF4B=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF40;if(_tmpF4B->tag != 24)goto
_LL763;else{_tmpF4C=_tmpF4B->f1;_tmpF4D=_tmpF4B->f2;}}_LL762: {void*_tmpF57=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF4C->topt))->v);_LL76E: {struct Cyc_Absyn_TupleType_struct*
_tmpF58=(struct Cyc_Absyn_TupleType_struct*)_tmpF57;if(_tmpF58->tag != 10)goto
_LL770;}_LL76F: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF4C);_LL770:;_LL771:
return 0;_LL76D:;}_LL763: {struct Cyc_Absyn_Conditional_e_struct*_tmpF4E=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF40;if(_tmpF4E->tag != 6)goto _LL765;else{
_tmpF4F=_tmpF4E->f2;_tmpF50=_tmpF4E->f3;}}_LL764: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF4F) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF50);_LL765: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF51=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF40;if(_tmpF51->tag != 9)goto _LL767;
else{_tmpF52=_tmpF51->f2;}}_LL766: _tmpF54=_tmpF52;goto _LL768;_LL767: {struct Cyc_Absyn_Cast_e_struct*
_tmpF53=(struct Cyc_Absyn_Cast_e_struct*)_tmpF40;if(_tmpF53->tag != 15)goto _LL769;
else{_tmpF54=_tmpF53->f2;}}_LL768: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF54);
_LL769: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF55=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF40;if(_tmpF55->tag != 37)goto _LL76B;else{_tmpF56=_tmpF55->f1;}}_LL76A: while(
1){void*_tmpF59=_tmpF56->r;struct Cyc_Absyn_Stmt*_tmpF5B;struct Cyc_Absyn_Stmt*
_tmpF5C;struct Cyc_Absyn_Decl*_tmpF5E;struct Cyc_Absyn_Stmt*_tmpF5F;struct Cyc_Absyn_Exp*
_tmpF61;_LL773: {struct Cyc_Absyn_Seq_s_struct*_tmpF5A=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF59;if(_tmpF5A->tag != 2)goto _LL775;else{_tmpF5B=_tmpF5A->f1;_tmpF5C=_tmpF5A->f2;}}
_LL774: _tmpF56=_tmpF5C;goto _LL772;_LL775: {struct Cyc_Absyn_Decl_s_struct*_tmpF5D=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF59;if(_tmpF5D->tag != 12)goto _LL777;else{
_tmpF5E=_tmpF5D->f1;_tmpF5F=_tmpF5D->f2;}}_LL776: _tmpF56=_tmpF5F;goto _LL772;
_LL777: {struct Cyc_Absyn_Exp_s_struct*_tmpF60=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF59;if(_tmpF60->tag != 1)goto _LL779;else{_tmpF61=_tmpF60->f1;}}_LL778: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF61);_LL779:;_LL77A: {const char*_tmp1628;
void*_tmp1627;(_tmp1627=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp1628="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp1628,sizeof(char),40))),_tag_dyneither(_tmp1627,sizeof(void*),
0)));}_LL772:;}_LL76B:;_LL76C: return 1;_LL758:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple18 _tmp1629;struct _tuple18 bogus_ans=(_tmp1629.f1=0,((_tmp1629.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1629)));void*_tmpF64=e->r;struct _tuple2*_tmpF66;
void*_tmpF67;struct Cyc_Absyn_Exp*_tmpF69;struct _dyneither_ptr*_tmpF6A;int _tmpF6B;
struct Cyc_Absyn_Exp*_tmpF6D;struct _dyneither_ptr*_tmpF6E;int _tmpF6F;struct Cyc_Absyn_Exp*
_tmpF71;struct Cyc_Absyn_Exp*_tmpF73;struct Cyc_Absyn_Exp*_tmpF74;_LL77C: {struct
Cyc_Absyn_Var_e_struct*_tmpF65=(struct Cyc_Absyn_Var_e_struct*)_tmpF64;if(_tmpF65->tag
!= 1)goto _LL77E;else{_tmpF66=_tmpF65->f1;_tmpF67=(void*)_tmpF65->f2;}}_LL77D: {
void*_tmpF75=_tmpF67;struct Cyc_Absyn_Vardecl*_tmpF79;struct Cyc_Absyn_Vardecl*
_tmpF7B;struct Cyc_Absyn_Vardecl*_tmpF7D;struct Cyc_Absyn_Vardecl*_tmpF7F;_LL789: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF76=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF75;if(_tmpF76->tag != 0)goto _LL78B;}_LL78A: goto _LL78C;_LL78B: {struct Cyc_Absyn_Funname_b_struct*
_tmpF77=(struct Cyc_Absyn_Funname_b_struct*)_tmpF75;if(_tmpF77->tag != 2)goto
_LL78D;}_LL78C: return bogus_ans;_LL78D: {struct Cyc_Absyn_Global_b_struct*_tmpF78=(
struct Cyc_Absyn_Global_b_struct*)_tmpF75;if(_tmpF78->tag != 1)goto _LL78F;else{
_tmpF79=_tmpF78->f1;}}_LL78E: {void*_tmpF80=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL796: {struct Cyc_Absyn_ArrayType_struct*
_tmpF81=(struct Cyc_Absyn_ArrayType_struct*)_tmpF80;if(_tmpF81->tag != 8)goto
_LL798;}_LL797: {struct _tuple18 _tmp162A;return(_tmp162A.f1=1,((_tmp162A.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp162A)));}_LL798:;_LL799: {struct _tuple18 _tmp162B;
return(_tmp162B.f1=(_tmpF79->tq).real_const,((_tmp162B.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp162B)));}_LL795:;}_LL78F: {struct Cyc_Absyn_Local_b_struct*_tmpF7A=(struct Cyc_Absyn_Local_b_struct*)
_tmpF75;if(_tmpF7A->tag != 4)goto _LL791;else{_tmpF7B=_tmpF7A->f1;}}_LL790: {void*
_tmpF84=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL79B: {struct Cyc_Absyn_ArrayType_struct*_tmpF85=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF84;if(_tmpF85->tag != 8)goto _LL79D;}_LL79C: {struct _tuple18 _tmp162C;return(
_tmp162C.f1=1,((_tmp162C.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF7B->rgn))->v,
_tmp162C)));}_LL79D:;_LL79E: _tmpF7B->escapes=1;{struct _tuple18 _tmp162D;return(
_tmp162D.f1=(_tmpF7B->tq).real_const,((_tmp162D.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF7B->rgn))->v,_tmp162D)));};_LL79A:;}_LL791: {struct Cyc_Absyn_Pat_b_struct*
_tmpF7C=(struct Cyc_Absyn_Pat_b_struct*)_tmpF75;if(_tmpF7C->tag != 5)goto _LL793;
else{_tmpF7D=_tmpF7C->f1;}}_LL792: _tmpF7F=_tmpF7D;goto _LL794;_LL793: {struct Cyc_Absyn_Param_b_struct*
_tmpF7E=(struct Cyc_Absyn_Param_b_struct*)_tmpF75;if(_tmpF7E->tag != 3)goto _LL788;
else{_tmpF7F=_tmpF7E->f1;}}_LL794: _tmpF7F->escapes=1;{struct _tuple18 _tmp162E;
return(_tmp162E.f1=(_tmpF7F->tq).real_const,((_tmp162E.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF7F->rgn))->v,_tmp162E)));};_LL788:;}_LL77E: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF68=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF64;if(_tmpF68->tag != 22)goto
_LL780;else{_tmpF69=_tmpF68->f1;_tmpF6A=_tmpF68->f2;_tmpF6B=_tmpF68->f3;}}_LL77F:
if(_tmpF6B)return bogus_ans;{void*_tmpF89=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF69->topt))->v);struct Cyc_List_List*_tmpF8B;struct Cyc_Absyn_AggrInfo
_tmpF8D;union Cyc_Absyn_AggrInfoU _tmpF8E;struct Cyc_Absyn_Aggrdecl**_tmpF8F;struct
Cyc_Absyn_Aggrdecl*_tmpF90;_LL7A0: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF8A=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF89;if(_tmpF8A->tag != 12)goto _LL7A2;
else{_tmpF8B=_tmpF8A->f2;}}_LL7A1: {struct Cyc_Absyn_Aggrfield*_tmpF91=Cyc_Absyn_lookup_field(
_tmpF8B,_tmpF6A);if(_tmpF91 != 0  && _tmpF91->width == 0){struct _tuple18 _tmp162F;
return(_tmp162F.f1=(_tmpF91->tq).real_const,((_tmp162F.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF69)).f2,_tmp162F)));}return bogus_ans;}_LL7A2: {struct Cyc_Absyn_AggrType_struct*
_tmpF8C=(struct Cyc_Absyn_AggrType_struct*)_tmpF89;if(_tmpF8C->tag != 11)goto
_LL7A4;else{_tmpF8D=_tmpF8C->f1;_tmpF8E=_tmpF8D.aggr_info;if((_tmpF8E.KnownAggr).tag
!= 2)goto _LL7A4;_tmpF8F=(struct Cyc_Absyn_Aggrdecl**)(_tmpF8E.KnownAggr).val;
_tmpF90=*_tmpF8F;}}_LL7A3: {struct Cyc_Absyn_Aggrfield*_tmpF93=Cyc_Absyn_lookup_decl_field(
_tmpF90,_tmpF6A);if(_tmpF93 != 0  && _tmpF93->width == 0){struct _tuple18 _tmp1630;
return(_tmp1630.f1=(_tmpF93->tq).real_const,((_tmp1630.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF69)).f2,_tmp1630)));}return bogus_ans;}_LL7A4:;_LL7A5: return bogus_ans;
_LL79F:;};_LL780: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF6C=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF64;if(_tmpF6C->tag != 23)goto _LL782;else{_tmpF6D=_tmpF6C->f1;_tmpF6E=_tmpF6C->f2;
_tmpF6F=_tmpF6C->f3;}}_LL781: if(_tmpF6F)return bogus_ans;{void*_tmpF95=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF6D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpF97;void*_tmpF98;struct Cyc_Absyn_PtrAtts _tmpF99;void*_tmpF9A;_LL7A7: {struct
Cyc_Absyn_PointerType_struct*_tmpF96=(struct Cyc_Absyn_PointerType_struct*)
_tmpF95;if(_tmpF96->tag != 5)goto _LL7A9;else{_tmpF97=_tmpF96->f1;_tmpF98=_tmpF97.elt_typ;
_tmpF99=_tmpF97.ptr_atts;_tmpF9A=_tmpF99.rgn;}}_LL7A8: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpF9B=Cyc_Tcutil_compress(_tmpF98);struct Cyc_List_List*_tmpF9D;
struct Cyc_Absyn_AggrInfo _tmpF9F;union Cyc_Absyn_AggrInfoU _tmpFA0;struct Cyc_Absyn_Aggrdecl**
_tmpFA1;struct Cyc_Absyn_Aggrdecl*_tmpFA2;_LL7AC: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF9C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF9B;if(_tmpF9C->tag != 12)goto
_LL7AE;else{_tmpF9D=_tmpF9C->f2;}}_LL7AD: finfo=Cyc_Absyn_lookup_field(_tmpF9D,
_tmpF6E);goto _LL7AB;_LL7AE: {struct Cyc_Absyn_AggrType_struct*_tmpF9E=(struct Cyc_Absyn_AggrType_struct*)
_tmpF9B;if(_tmpF9E->tag != 11)goto _LL7B0;else{_tmpF9F=_tmpF9E->f1;_tmpFA0=_tmpF9F.aggr_info;
if((_tmpFA0.KnownAggr).tag != 2)goto _LL7B0;_tmpFA1=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFA0.KnownAggr).val;_tmpFA2=*_tmpFA1;}}_LL7AF: finfo=Cyc_Absyn_lookup_decl_field(
_tmpFA2,_tmpF6E);goto _LL7AB;_LL7B0:;_LL7B1: return bogus_ans;_LL7AB:;}if(finfo != 0
 && finfo->width == 0){struct _tuple18 _tmp1631;return(_tmp1631.f1=(finfo->tq).real_const,((
_tmp1631.f2=_tmpF9A,_tmp1631)));}return bogus_ans;}_LL7A9:;_LL7AA: return bogus_ans;
_LL7A6:;};_LL782: {struct Cyc_Absyn_Deref_e_struct*_tmpF70=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF64;if(_tmpF70->tag != 21)goto _LL784;else{_tmpF71=_tmpF70->f1;}}_LL783: {void*
_tmpFA4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF71->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpFA6;struct Cyc_Absyn_Tqual _tmpFA7;struct Cyc_Absyn_PtrAtts
_tmpFA8;void*_tmpFA9;_LL7B3: {struct Cyc_Absyn_PointerType_struct*_tmpFA5=(struct
Cyc_Absyn_PointerType_struct*)_tmpFA4;if(_tmpFA5->tag != 5)goto _LL7B5;else{
_tmpFA6=_tmpFA5->f1;_tmpFA7=_tmpFA6.elt_tq;_tmpFA8=_tmpFA6.ptr_atts;_tmpFA9=
_tmpFA8.rgn;}}_LL7B4: {struct _tuple18 _tmp1632;return(_tmp1632.f1=_tmpFA7.real_const,((
_tmp1632.f2=_tmpFA9,_tmp1632)));}_LL7B5:;_LL7B6: return bogus_ans;_LL7B2:;}_LL784: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF72=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF64;if(_tmpF72->tag != 24)goto _LL786;else{_tmpF73=_tmpF72->f1;_tmpF74=_tmpF72->f2;}}
_LL785: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF73->topt))->v);void*_tmpFAB=t;struct Cyc_List_List*_tmpFAD;struct Cyc_Absyn_PtrInfo
_tmpFAF;struct Cyc_Absyn_Tqual _tmpFB0;struct Cyc_Absyn_PtrAtts _tmpFB1;void*_tmpFB2;
struct Cyc_Absyn_ArrayInfo _tmpFB4;struct Cyc_Absyn_Tqual _tmpFB5;_LL7B8: {struct Cyc_Absyn_TupleType_struct*
_tmpFAC=(struct Cyc_Absyn_TupleType_struct*)_tmpFAB;if(_tmpFAC->tag != 10)goto
_LL7BA;else{_tmpFAD=_tmpFAC->f1;}}_LL7B9: {unsigned int _tmpFB7;int _tmpFB8;struct
_tuple14 _tmpFB6=Cyc_Evexp_eval_const_uint_exp(_tmpF74);_tmpFB7=_tmpFB6.f1;
_tmpFB8=_tmpFB6.f2;if(!_tmpFB8)return bogus_ans;{struct _tuple12*_tmpFB9=Cyc_Absyn_lookup_tuple_field(
_tmpFAD,(int)_tmpFB7);if(_tmpFB9 != 0){struct _tuple18 _tmp1633;return(_tmp1633.f1=((*
_tmpFB9).f1).real_const,((_tmp1633.f2=(Cyc_Tcutil_addressof_props(te,_tmpF73)).f2,
_tmp1633)));}return bogus_ans;};}_LL7BA: {struct Cyc_Absyn_PointerType_struct*
_tmpFAE=(struct Cyc_Absyn_PointerType_struct*)_tmpFAB;if(_tmpFAE->tag != 5)goto
_LL7BC;else{_tmpFAF=_tmpFAE->f1;_tmpFB0=_tmpFAF.elt_tq;_tmpFB1=_tmpFAF.ptr_atts;
_tmpFB2=_tmpFB1.rgn;}}_LL7BB: {struct _tuple18 _tmp1634;return(_tmp1634.f1=_tmpFB0.real_const,((
_tmp1634.f2=_tmpFB2,_tmp1634)));}_LL7BC: {struct Cyc_Absyn_ArrayType_struct*
_tmpFB3=(struct Cyc_Absyn_ArrayType_struct*)_tmpFAB;if(_tmpFB3->tag != 8)goto
_LL7BE;else{_tmpFB4=_tmpFB3->f1;_tmpFB5=_tmpFB4.tq;}}_LL7BD: {struct _tuple18
_tmp1635;return(_tmp1635.f1=_tmpFB5.real_const,((_tmp1635.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF73)).f2,_tmp1635)));}_LL7BE:;_LL7BF: return bogus_ans;_LL7B7:;}_LL786:;
_LL787:{const char*_tmp1638;void*_tmp1637;(_tmp1637=0,Cyc_Tcutil_terr(e->loc,((
_tmp1638="unary & applied to non-lvalue",_tag_dyneither(_tmp1638,sizeof(char),30))),
_tag_dyneither(_tmp1637,sizeof(void*),0)));}return bogus_ans;_LL77B:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFC0=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFC2;void*_tmpFC3;struct Cyc_Absyn_Tqual
_tmpFC4;union Cyc_Absyn_Constraint*_tmpFC5;_LL7C1: {struct Cyc_Absyn_ArrayType_struct*
_tmpFC1=(struct Cyc_Absyn_ArrayType_struct*)_tmpFC0;if(_tmpFC1->tag != 8)goto
_LL7C3;else{_tmpFC2=_tmpFC1->f1;_tmpFC3=_tmpFC2.elt_type;_tmpFC4=_tmpFC2.tq;
_tmpFC5=_tmpFC2.zero_term;}}_LL7C2: {void*_tmpFC7;struct _tuple18 _tmpFC6=Cyc_Tcutil_addressof_props(
te,e);_tmpFC7=_tmpFC6.f2;{struct Cyc_Absyn_Upper_b_struct _tmp163B;struct Cyc_Absyn_Upper_b_struct*
_tmp163A;return Cyc_Absyn_atb_typ(_tmpFC3,_tmpFC7,_tmpFC4,(void*)((_tmp163A=
_cycalloc(sizeof(*_tmp163A)),((_tmp163A[0]=((_tmp163B.tag=1,((_tmp163B.f1=e,
_tmp163B)))),_tmp163A)))),_tmpFC5);};}_LL7C3:;_LL7C4: return e_typ;_LL7C0:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFCA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFCD;_LL7C6: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFCB=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFCA;if(_tmpFCB->tag != 0)goto
_LL7C8;}_LL7C7: return;_LL7C8: {struct Cyc_Absyn_Upper_b_struct*_tmpFCC=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFCA;if(_tmpFCC->tag != 1)goto _LL7C5;else{_tmpFCD=_tmpFCC->f1;}}_LL7C9: {
unsigned int _tmpFCF;int _tmpFD0;struct _tuple14 _tmpFCE=Cyc_Evexp_eval_const_uint_exp(
_tmpFCD);_tmpFCF=_tmpFCE.f1;_tmpFD0=_tmpFCE.f2;if(_tmpFD0  && _tmpFCF <= i){const
char*_tmp1640;void*_tmp163F[2];struct Cyc_Int_pa_struct _tmp163E;struct Cyc_Int_pa_struct
_tmp163D;(_tmp163D.tag=1,((_tmp163D.f1=(unsigned long)((int)i),((_tmp163E.tag=1,((
_tmp163E.f1=(unsigned long)((int)_tmpFCF),((_tmp163F[0]=& _tmp163E,((_tmp163F[1]=&
_tmp163D,Cyc_Tcutil_terr(loc,((_tmp1640="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1640,sizeof(char),39))),_tag_dyneither(_tmp163F,sizeof(void*),
2)))))))))))));}return;}_LL7C5:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b){void*_tmpFD5=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*
_tmpFD7;_LL7CB: {struct Cyc_Absyn_Upper_b_struct*_tmpFD6=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFD5;if(_tmpFD6->tag != 1)goto _LL7CD;else{_tmpFD7=_tmpFD6->f1;}}_LL7CC: {
unsigned int _tmpFD9;int _tmpFDA;struct _tuple14 _tmpFD8=Cyc_Evexp_eval_const_uint_exp(
_tmpFD7);_tmpFD9=_tmpFD8.f1;_tmpFDA=_tmpFD8.f2;return _tmpFDA  && _tmpFD9 == 1;}
_LL7CD:;_LL7CE: return 0;_LL7CA:;}int Cyc_Tcutil_bits_only(void*t){void*_tmpFDB=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmpFE2;void*_tmpFE3;union Cyc_Absyn_Constraint*
_tmpFE4;struct Cyc_List_List*_tmpFE6;struct Cyc_Absyn_AggrInfo _tmpFE8;union Cyc_Absyn_AggrInfoU
_tmpFE9;struct _tuple4 _tmpFEA;struct Cyc_Absyn_AggrInfo _tmpFEC;union Cyc_Absyn_AggrInfoU
_tmpFED;struct Cyc_Absyn_Aggrdecl**_tmpFEE;struct Cyc_Absyn_Aggrdecl*_tmpFEF;
struct Cyc_List_List*_tmpFF0;struct Cyc_List_List*_tmpFF2;_LL7D0: {struct Cyc_Absyn_VoidType_struct*
_tmpFDC=(struct Cyc_Absyn_VoidType_struct*)_tmpFDB;if(_tmpFDC->tag != 0)goto _LL7D2;}
_LL7D1: goto _LL7D3;_LL7D2: {struct Cyc_Absyn_IntType_struct*_tmpFDD=(struct Cyc_Absyn_IntType_struct*)
_tmpFDB;if(_tmpFDD->tag != 6)goto _LL7D4;}_LL7D3: goto _LL7D5;_LL7D4: {struct Cyc_Absyn_FloatType_struct*
_tmpFDE=(struct Cyc_Absyn_FloatType_struct*)_tmpFDB;if(_tmpFDE->tag != 7)goto
_LL7D6;}_LL7D5: return 1;_LL7D6: {struct Cyc_Absyn_EnumType_struct*_tmpFDF=(struct
Cyc_Absyn_EnumType_struct*)_tmpFDB;if(_tmpFDF->tag != 13)goto _LL7D8;}_LL7D7: goto
_LL7D9;_LL7D8: {struct Cyc_Absyn_AnonEnumType_struct*_tmpFE0=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmpFDB;if(_tmpFE0->tag != 14)goto _LL7DA;}_LL7D9: return 0;_LL7DA: {struct Cyc_Absyn_ArrayType_struct*
_tmpFE1=(struct Cyc_Absyn_ArrayType_struct*)_tmpFDB;if(_tmpFE1->tag != 8)goto
_LL7DC;else{_tmpFE2=_tmpFE1->f1;_tmpFE3=_tmpFE2.elt_type;_tmpFE4=_tmpFE2.zero_term;}}
_LL7DB: return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpFE4) && Cyc_Tcutil_bits_only(_tmpFE3);_LL7DC: {struct Cyc_Absyn_TupleType_struct*
_tmpFE5=(struct Cyc_Absyn_TupleType_struct*)_tmpFDB;if(_tmpFE5->tag != 10)goto
_LL7DE;else{_tmpFE6=_tmpFE5->f1;}}_LL7DD: for(0;_tmpFE6 != 0;_tmpFE6=_tmpFE6->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFE6->hd)).f2))return 0;}return 1;
_LL7DE: {struct Cyc_Absyn_AggrType_struct*_tmpFE7=(struct Cyc_Absyn_AggrType_struct*)
_tmpFDB;if(_tmpFE7->tag != 11)goto _LL7E0;else{_tmpFE8=_tmpFE7->f1;_tmpFE9=_tmpFE8.aggr_info;
if((_tmpFE9.UnknownAggr).tag != 1)goto _LL7E0;_tmpFEA=(struct _tuple4)(_tmpFE9.UnknownAggr).val;}}
_LL7DF: return 0;_LL7E0: {struct Cyc_Absyn_AggrType_struct*_tmpFEB=(struct Cyc_Absyn_AggrType_struct*)
_tmpFDB;if(_tmpFEB->tag != 11)goto _LL7E2;else{_tmpFEC=_tmpFEB->f1;_tmpFED=_tmpFEC.aggr_info;
if((_tmpFED.KnownAggr).tag != 2)goto _LL7E2;_tmpFEE=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFED.KnownAggr).val;_tmpFEF=*_tmpFEE;_tmpFF0=_tmpFEC.targs;}}_LL7E1: if(
_tmpFEF->impl == 0)return 0;{struct _RegionHandle _tmpFF3=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpFF3;_push_region(rgn);{struct Cyc_List_List*_tmpFF4=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFEF->tvs,_tmpFF0);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFEF->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpFF4,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpFF5=0;_npop_handler(0);return
_tmpFF5;}}}{int _tmpFF6=1;_npop_handler(0);return _tmpFF6;};};_pop_region(rgn);};
_LL7E2: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFF1=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFDB;if(_tmpFF1->tag != 12)goto _LL7E4;else{_tmpFF2=_tmpFF1->f2;}}_LL7E3: for(0;
_tmpFF2 != 0;_tmpFF2=_tmpFF2->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFF2->hd)->type))return 0;}return 1;_LL7E4:;_LL7E5: return 0;_LL7CF:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpFF7=e->r;struct _tuple2*_tmpFFF;void*_tmp1000;struct Cyc_Absyn_Exp*
_tmp1002;struct Cyc_Absyn_Exp*_tmp1003;struct Cyc_Absyn_Exp*_tmp1004;struct Cyc_Absyn_Exp*
_tmp1006;struct Cyc_Absyn_Exp*_tmp1007;struct Cyc_Absyn_Exp*_tmp1009;struct Cyc_Absyn_Exp*
_tmp100B;void*_tmp100D;struct Cyc_Absyn_Exp*_tmp100E;enum Cyc_Absyn_Coercion
_tmp100F;void*_tmp1011;struct Cyc_Absyn_Exp*_tmp1012;struct Cyc_Absyn_Exp*_tmp1014;
struct Cyc_Absyn_Exp*_tmp1016;struct Cyc_Absyn_Exp*_tmp1017;struct Cyc_List_List*
_tmp1019;struct Cyc_List_List*_tmp101B;struct Cyc_List_List*_tmp101D;enum Cyc_Absyn_Primop
_tmp101F;struct Cyc_List_List*_tmp1020;struct Cyc_List_List*_tmp1022;struct Cyc_List_List*
_tmp1024;_LL7E7: {struct Cyc_Absyn_Const_e_struct*_tmpFF8=(struct Cyc_Absyn_Const_e_struct*)
_tmpFF7;if(_tmpFF8->tag != 0)goto _LL7E9;}_LL7E8: goto _LL7EA;_LL7E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpFF9=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpFF7;if(_tmpFF9->tag != 18)goto
_LL7EB;}_LL7EA: goto _LL7EC;_LL7EB: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpFFA=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFF7;if(_tmpFFA->tag != 19)goto _LL7ED;}
_LL7EC: goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Offsetof_e_struct*_tmpFFB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpFF7;if(_tmpFFB->tag != 20)goto _LL7EF;}_LL7EE: goto _LL7F0;_LL7EF: {struct Cyc_Absyn_Enum_e_struct*
_tmpFFC=(struct Cyc_Absyn_Enum_e_struct*)_tmpFF7;if(_tmpFFC->tag != 32)goto _LL7F1;}
_LL7F0: goto _LL7F2;_LL7F1: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpFFD=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpFF7;if(_tmpFFD->tag != 33)goto _LL7F3;}_LL7F2: return 1;_LL7F3: {struct Cyc_Absyn_Var_e_struct*
_tmpFFE=(struct Cyc_Absyn_Var_e_struct*)_tmpFF7;if(_tmpFFE->tag != 1)goto _LL7F5;
else{_tmpFFF=_tmpFFE->f1;_tmp1000=(void*)_tmpFFE->f2;}}_LL7F4: {void*_tmp1025=
_tmp1000;struct Cyc_Absyn_Vardecl*_tmp1028;_LL814: {struct Cyc_Absyn_Funname_b_struct*
_tmp1026=(struct Cyc_Absyn_Funname_b_struct*)_tmp1025;if(_tmp1026->tag != 2)goto
_LL816;}_LL815: return 1;_LL816: {struct Cyc_Absyn_Global_b_struct*_tmp1027=(struct
Cyc_Absyn_Global_b_struct*)_tmp1025;if(_tmp1027->tag != 1)goto _LL818;else{
_tmp1028=_tmp1027->f1;}}_LL817: {void*_tmp102A=Cyc_Tcutil_compress(_tmp1028->type);
_LL81D: {struct Cyc_Absyn_ArrayType_struct*_tmp102B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp102A;if(_tmp102B->tag != 8)goto _LL81F;}_LL81E: goto _LL820;_LL81F: {struct Cyc_Absyn_FnType_struct*
_tmp102C=(struct Cyc_Absyn_FnType_struct*)_tmp102A;if(_tmp102C->tag != 9)goto
_LL821;}_LL820: return 1;_LL821:;_LL822: return var_okay;_LL81C:;}_LL818: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp1029=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp1025;if(_tmp1029->tag != 0)
goto _LL81A;}_LL819: return 0;_LL81A:;_LL81B: return var_okay;_LL813:;}_LL7F5: {
struct Cyc_Absyn_Conditional_e_struct*_tmp1001=(struct Cyc_Absyn_Conditional_e_struct*)
_tmpFF7;if(_tmp1001->tag != 6)goto _LL7F7;else{_tmp1002=_tmp1001->f1;_tmp1003=
_tmp1001->f2;_tmp1004=_tmp1001->f3;}}_LL7F6: return(Cyc_Tcutil_cnst_exp(te,0,
_tmp1002) && Cyc_Tcutil_cnst_exp(te,0,_tmp1003)) && Cyc_Tcutil_cnst_exp(te,0,
_tmp1004);_LL7F7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp1005=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpFF7;if(_tmp1005->tag != 9)goto _LL7F9;else{_tmp1006=_tmp1005->f1;_tmp1007=
_tmp1005->f2;}}_LL7F8: return Cyc_Tcutil_cnst_exp(te,0,_tmp1006) && Cyc_Tcutil_cnst_exp(
te,0,_tmp1007);_LL7F9: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp1008=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmpFF7;if(_tmp1008->tag != 13)goto _LL7FB;else{
_tmp1009=_tmp1008->f1;}}_LL7FA: _tmp100B=_tmp1009;goto _LL7FC;_LL7FB: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp100A=(struct Cyc_Absyn_Instantiate_e_struct*)_tmpFF7;if(_tmp100A->tag != 14)
goto _LL7FD;else{_tmp100B=_tmp100A->f1;}}_LL7FC: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmp100B);_LL7FD: {struct Cyc_Absyn_Cast_e_struct*_tmp100C=(struct Cyc_Absyn_Cast_e_struct*)
_tmpFF7;if(_tmp100C->tag != 15)goto _LL7FF;else{_tmp100D=(void*)_tmp100C->f1;
_tmp100E=_tmp100C->f2;_tmp100F=_tmp100C->f4;if(_tmp100F != Cyc_Absyn_No_coercion)
goto _LL7FF;}}_LL7FE: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp100E);_LL7FF: {
struct Cyc_Absyn_Cast_e_struct*_tmp1010=(struct Cyc_Absyn_Cast_e_struct*)_tmpFF7;
if(_tmp1010->tag != 15)goto _LL801;else{_tmp1011=(void*)_tmp1010->f1;_tmp1012=
_tmp1010->f2;}}_LL800: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1012);_LL801: {
struct Cyc_Absyn_Address_e_struct*_tmp1013=(struct Cyc_Absyn_Address_e_struct*)
_tmpFF7;if(_tmp1013->tag != 16)goto _LL803;else{_tmp1014=_tmp1013->f1;}}_LL802:
return Cyc_Tcutil_cnst_exp(te,1,_tmp1014);_LL803: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1015=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpFF7;if(_tmp1015->tag != 28)
goto _LL805;else{_tmp1016=_tmp1015->f2;_tmp1017=_tmp1015->f3;}}_LL804: return Cyc_Tcutil_cnst_exp(
te,0,_tmp1016) && Cyc_Tcutil_cnst_exp(te,0,_tmp1017);_LL805: {struct Cyc_Absyn_Array_e_struct*
_tmp1018=(struct Cyc_Absyn_Array_e_struct*)_tmpFF7;if(_tmp1018->tag != 27)goto
_LL807;else{_tmp1019=_tmp1018->f1;}}_LL806: _tmp101B=_tmp1019;goto _LL808;_LL807: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp101A=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpFF7;if(_tmp101A->tag != 30)goto _LL809;else{_tmp101B=_tmp101A->f2;}}_LL808:
_tmp101D=_tmp101B;goto _LL80A;_LL809: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp101C=(struct Cyc_Absyn_Aggregate_e_struct*)_tmpFF7;if(_tmp101C->tag != 29)goto
_LL80B;else{_tmp101D=_tmp101C->f3;}}_LL80A: for(0;_tmp101D != 0;_tmp101D=_tmp101D->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple19*)_tmp101D->hd)).f2))return 0;}
return 1;_LL80B: {struct Cyc_Absyn_Primop_e_struct*_tmp101E=(struct Cyc_Absyn_Primop_e_struct*)
_tmpFF7;if(_tmp101E->tag != 3)goto _LL80D;else{_tmp101F=_tmp101E->f1;_tmp1020=
_tmp101E->f2;}}_LL80C: _tmp1022=_tmp1020;goto _LL80E;_LL80D: {struct Cyc_Absyn_Tuple_e_struct*
_tmp1021=(struct Cyc_Absyn_Tuple_e_struct*)_tmpFF7;if(_tmp1021->tag != 25)goto
_LL80F;else{_tmp1022=_tmp1021->f1;}}_LL80E: _tmp1024=_tmp1022;goto _LL810;_LL80F: {
struct Cyc_Absyn_Datatype_e_struct*_tmp1023=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpFF7;if(_tmp1023->tag != 31)goto _LL811;else{_tmp1024=_tmp1023->f1;}}_LL810:
for(0;_tmp1024 != 0;_tmp1024=_tmp1024->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp1024->hd))return 0;}return 1;_LL811:;_LL812: return 0;_LL7E6:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp102D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1032;
struct Cyc_Absyn_PtrAtts _tmp1033;union Cyc_Absyn_Constraint*_tmp1034;union Cyc_Absyn_Constraint*
_tmp1035;struct Cyc_Absyn_ArrayInfo _tmp1037;void*_tmp1038;struct Cyc_List_List*
_tmp103A;struct Cyc_Absyn_AggrInfo _tmp103C;union Cyc_Absyn_AggrInfoU _tmp103D;
struct Cyc_List_List*_tmp103E;struct Cyc_List_List*_tmp1040;_LL824: {struct Cyc_Absyn_VoidType_struct*
_tmp102E=(struct Cyc_Absyn_VoidType_struct*)_tmp102D;if(_tmp102E->tag != 0)goto
_LL826;}_LL825: goto _LL827;_LL826: {struct Cyc_Absyn_IntType_struct*_tmp102F=(
struct Cyc_Absyn_IntType_struct*)_tmp102D;if(_tmp102F->tag != 6)goto _LL828;}_LL827:
goto _LL829;_LL828: {struct Cyc_Absyn_FloatType_struct*_tmp1030=(struct Cyc_Absyn_FloatType_struct*)
_tmp102D;if(_tmp1030->tag != 7)goto _LL82A;}_LL829: return 1;_LL82A: {struct Cyc_Absyn_PointerType_struct*
_tmp1031=(struct Cyc_Absyn_PointerType_struct*)_tmp102D;if(_tmp1031->tag != 5)goto
_LL82C;else{_tmp1032=_tmp1031->f1;_tmp1033=_tmp1032.ptr_atts;_tmp1034=_tmp1033.nullable;
_tmp1035=_tmp1033.bounds;}}_LL82B: {void*_tmp1043=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1035);_LL83B: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1044=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1043;if(_tmp1044->tag != 0)goto _LL83D;}_LL83C: return 1;_LL83D:;_LL83E: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp1034);
_LL83A:;}_LL82C: {struct Cyc_Absyn_ArrayType_struct*_tmp1036=(struct Cyc_Absyn_ArrayType_struct*)
_tmp102D;if(_tmp1036->tag != 8)goto _LL82E;else{_tmp1037=_tmp1036->f1;_tmp1038=
_tmp1037.elt_type;}}_LL82D: return Cyc_Tcutil_supports_default(_tmp1038);_LL82E: {
struct Cyc_Absyn_TupleType_struct*_tmp1039=(struct Cyc_Absyn_TupleType_struct*)
_tmp102D;if(_tmp1039->tag != 10)goto _LL830;else{_tmp103A=_tmp1039->f1;}}_LL82F:
for(0;_tmp103A != 0;_tmp103A=_tmp103A->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple12*)_tmp103A->hd)).f2))return 0;}return 1;_LL830: {struct Cyc_Absyn_AggrType_struct*
_tmp103B=(struct Cyc_Absyn_AggrType_struct*)_tmp102D;if(_tmp103B->tag != 11)goto
_LL832;else{_tmp103C=_tmp103B->f1;_tmp103D=_tmp103C.aggr_info;_tmp103E=_tmp103C.targs;}}
_LL831: {struct Cyc_Absyn_Aggrdecl*_tmp1045=Cyc_Absyn_get_known_aggrdecl(_tmp103D);
if(_tmp1045->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1045->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp1045->tvs,_tmp103E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1045->impl))->fields);}
_LL832: {struct Cyc_Absyn_AnonAggrType_struct*_tmp103F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp102D;if(_tmp103F->tag != 12)goto _LL834;else{_tmp1040=_tmp103F->f2;}}_LL833:
return Cyc_Tcutil_fields_support_default(0,0,_tmp1040);_LL834: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1041=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp102D;if(_tmp1041->tag != 14)
goto _LL836;}_LL835: goto _LL837;_LL836: {struct Cyc_Absyn_EnumType_struct*_tmp1042=(
struct Cyc_Absyn_EnumType_struct*)_tmp102D;if(_tmp1042->tag != 13)goto _LL838;}
_LL837: return 1;_LL838:;_LL839: return 0;_LL823:;}void Cyc_Tcutil_check_no_qual(
struct Cyc_Position_Segment*loc,void*t){void*_tmp1046=t;struct Cyc_Absyn_Typedefdecl*
_tmp1048;_LL840: {struct Cyc_Absyn_TypedefType_struct*_tmp1047=(struct Cyc_Absyn_TypedefType_struct*)
_tmp1046;if(_tmp1047->tag != 17)goto _LL842;else{_tmp1048=_tmp1047->f3;}}_LL841:
if(_tmp1048 != 0){struct Cyc_Absyn_Tqual _tmp1049=_tmp1048->tq;if(((_tmp1049.print_const
 || _tmp1049.q_volatile) || _tmp1049.q_restrict) || _tmp1049.real_const){const
char*_tmp1644;void*_tmp1643[1];struct Cyc_String_pa_struct _tmp1642;(_tmp1642.tag=
0,((_tmp1642.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1643[0]=& _tmp1642,Cyc_Tcutil_warn(loc,((_tmp1644="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp1644,sizeof(char),44))),_tag_dyneither(_tmp1643,sizeof(void*),
1)))))));}}goto _LL83F;_LL842:;_LL843: goto _LL83F;_LL83F:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp104D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp104D;
_push_region(rgn);{struct Cyc_List_List*_tmp104E=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp104E,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp104F=0;_npop_handler(0);return _tmp104F;}}}{int _tmp1050=1;
_npop_handler(0);return _tmp1050;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp1051=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1055;
struct Cyc_Absyn_PtrAtts _tmp1056;union Cyc_Absyn_Constraint*_tmp1057;union Cyc_Absyn_Constraint*
_tmp1058;_LL845: {struct Cyc_Absyn_IntType_struct*_tmp1052=(struct Cyc_Absyn_IntType_struct*)
_tmp1051;if(_tmp1052->tag != 6)goto _LL847;}_LL846: goto _LL848;_LL847: {struct Cyc_Absyn_FloatType_struct*
_tmp1053=(struct Cyc_Absyn_FloatType_struct*)_tmp1051;if(_tmp1053->tag != 7)goto
_LL849;}_LL848: return 1;_LL849: {struct Cyc_Absyn_PointerType_struct*_tmp1054=(
struct Cyc_Absyn_PointerType_struct*)_tmp1051;if(_tmp1054->tag != 5)goto _LL84B;
else{_tmp1055=_tmp1054->f1;_tmp1056=_tmp1055.ptr_atts;_tmp1057=_tmp1056.nullable;
_tmp1058=_tmp1056.bounds;}}_LL84A: {void*_tmp1059=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1058);_LL84E: {
struct Cyc_Absyn_DynEither_b_struct*_tmp105A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1059;if(_tmp105A->tag != 0)goto _LL850;}_LL84F: return 0;_LL850:;_LL851: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1057);
_LL84D:;}_LL84B:;_LL84C: return 0;_LL844:;}int Cyc_Tcutil_is_noreturn(void*t){{void*
_tmp105B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp105D;void*_tmp105E;
struct Cyc_Absyn_FnInfo _tmp1060;struct Cyc_List_List*_tmp1061;_LL853: {struct Cyc_Absyn_PointerType_struct*
_tmp105C=(struct Cyc_Absyn_PointerType_struct*)_tmp105B;if(_tmp105C->tag != 5)goto
_LL855;else{_tmp105D=_tmp105C->f1;_tmp105E=_tmp105D.elt_typ;}}_LL854: return Cyc_Tcutil_is_noreturn(
_tmp105E);_LL855: {struct Cyc_Absyn_FnType_struct*_tmp105F=(struct Cyc_Absyn_FnType_struct*)
_tmp105B;if(_tmp105F->tag != 9)goto _LL857;else{_tmp1060=_tmp105F->f1;_tmp1061=
_tmp1060.attributes;}}_LL856: for(0;_tmp1061 != 0;_tmp1061=_tmp1061->tl){void*
_tmp1062=(void*)_tmp1061->hd;_LL85A: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1063=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1062;if(_tmp1063->tag != 4)
goto _LL85C;}_LL85B: return 1;_LL85C:;_LL85D: continue;_LL859:;}goto _LL852;_LL857:;
_LL858: goto _LL852;_LL852:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp1064=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp1066;struct Cyc_List_List*_tmp1067;struct Cyc_List_List**_tmp1068;_LL85F: {
struct Cyc_Absyn_FnType_struct*_tmp1065=(struct Cyc_Absyn_FnType_struct*)_tmp1064;
if(_tmp1065->tag != 9)goto _LL861;else{_tmp1066=_tmp1065->f1;_tmp1067=_tmp1066.attributes;
_tmp1068=(struct Cyc_List_List**)&(_tmp1065->f1).attributes;}}_LL860: {struct Cyc_List_List*
_tmp1069=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1645;*_tmp1068=((_tmp1645=_cycalloc(sizeof(*_tmp1645)),((
_tmp1645->hd=(void*)atts->hd,((_tmp1645->tl=*_tmp1068,_tmp1645))))));}else{
struct Cyc_List_List*_tmp1646;_tmp1069=((_tmp1646=_cycalloc(sizeof(*_tmp1646)),((
_tmp1646->hd=(void*)atts->hd,((_tmp1646->tl=_tmp1069,_tmp1646))))));}}return
_tmp1069;}_LL861:;_LL862: {const char*_tmp1649;void*_tmp1648;(_tmp1648=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1649="transfer_fn_type_atts",
_tag_dyneither(_tmp1649,sizeof(char),22))),_tag_dyneither(_tmp1648,sizeof(void*),
0)));}_LL85E:;}
