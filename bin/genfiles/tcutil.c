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
char*tag;};typedef unsigned int Cyc_Position_seg_t;struct _dyneither_ptr Cyc_Position_string_of_segment(
unsigned int);struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};typedef void*Cyc_Position_error_kind_t;
struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;
struct _dyneither_ptr desc;};typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct
_dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{
void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;
union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};
struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
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
zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct
Cyc_Absyn_Datatype_td_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};typedef
void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};
struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple2*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_RefCntRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{int
tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_struct Cyc_Absyn_RefCntRgn_val;extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};typedef void*Cyc_Absyn_funcparams_t;enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft
 = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int
tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int
tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};
struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8
val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int
tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;
};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
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
topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{int tag;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
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
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*
tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct
_tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
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
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
unsigned int loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct _dyneither_ptr*f1;};extern
char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{char*tag;};int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*
Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);struct
Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple2*,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,unsigned int);struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct
_dyneither_ptr body,unsigned int);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
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
Cyc_Absyn_Tvar*fst_rgn,unsigned int);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
unsigned int,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**
Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
unsigned int,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,
void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();extern struct Cyc_Core_Opt*
Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*t);struct Cyc_Absyn_Exp*
Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*
k1,struct Cyc_Absyn_Kind*k2);struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t,struct Cyc_Absyn_Kind*def);struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);int
Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*,int*alias_coercion);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple15{struct Cyc_Absyn_Decl*
f1;struct Cyc_Absyn_Exp*f2;};struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ);extern int Cyc_Tcutil_warn_alias_coerce;extern int Cyc_Tcutil_warn_region_coerce;
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
_tuple17*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(unsigned int loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct
_dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);void
Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
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
unsigned int,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct
_tuple0*eq;int isTrue;};typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;struct
Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct _dyneither_ptr msg_part);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);int Cyc_Tcutil_warn_region_coerce=
0;void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=
0;void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=
0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(struct _dyneither_ptr){(void*)0,(
void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors
>= Cyc_Position_max_errors)return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
const char*_tmp1083;void*_tmp1082[2];const char*_tmp1081;const char*_tmp1080;struct
Cyc_String_pa_struct _tmp107F;struct Cyc_String_pa_struct _tmp107E;struct
_dyneither_ptr s1=(struct _dyneither_ptr)((_tmp107E.tag=0,((_tmp107E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((
_tmp107F.tag=0,((_tmp107F.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmp1080="const ",_tag_dyneither(_tmp1080,sizeof(char),7)):((_tmp1081="",
_tag_dyneither(_tmp1081,sizeof(char),1)))),((_tmp1082[0]=& _tmp107F,((_tmp1082[1]=&
_tmp107E,Cyc_aprintf(((_tmp1083="%s%s",_tag_dyneither(_tmp1083,sizeof(char),5))),
_tag_dyneither(_tmp1082,sizeof(void*),2))))))))))))));const char*_tmp108C;void*
_tmp108B[2];const char*_tmp108A;const char*_tmp1089;struct Cyc_String_pa_struct
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
struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(unsigned int loc,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct _dyneither_ptr msg=(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmp10B4;
void*_tmp10B3[1];struct Cyc_String_pa_struct _tmp10B2;(_tmp10B2.tag=0,((_tmp10B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp10B3[0]=& _tmp10B2,Cyc_fprintf(
Cyc_stderr,((_tmp10B4="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(
_tmp10B4,sizeof(char),36))),_tag_dyneither(_tmp10B3,sizeof(void*),1)))))));}Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct _tmp10B7;
struct Cyc_Core_Impossible_struct*_tmp10B6;(int)_throw((void*)((_tmp10B6=
_cycalloc(sizeof(*_tmp10B6)),((_tmp10B6[0]=((_tmp10B7.tag=Cyc_Core_Impossible,((
_tmp10B7.f1=msg,_tmp10B7)))),_tmp10B6)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmp10BC;void*_tmp10BB[2];struct Cyc_String_pa_struct
_tmp10BA;struct Cyc_String_pa_struct _tmp10B9;(_tmp10B9.tag=0,((_tmp10B9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmp10BA.tag=0,((_tmp10BA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmp10BB[0]=& _tmp10BA,((_tmp10BB[1]=& _tmp10B9,Cyc_fprintf(Cyc_stderr,((
_tmp10BC="%s::%s ",_tag_dyneither(_tmp10BC,sizeof(char),8))),_tag_dyneither(
_tmp10BB,sizeof(void*),2)))))))))))));}{const char*_tmp10BF;void*_tmp10BE;(
_tmp10BE=0,Cyc_fprintf(Cyc_stderr,((_tmp10BF="\n",_tag_dyneither(_tmp10BF,
sizeof(char),2))),_tag_dyneither(_tmp10BE,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(
unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
struct Cyc_List_List*_tmp10C0;Cyc_Tcutil_warning_segs=((_tmp10C0=_cycalloc(
sizeof(*_tmp10C0)),((_tmp10C0->hd=(void*)sg,((_tmp10C0->tl=Cyc_Tcutil_warning_segs,
_tmp10C0))))));}{struct _dyneither_ptr*_tmp10C3;struct Cyc_List_List*_tmp10C2;Cyc_Tcutil_warning_msgs=((
_tmp10C2=_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2->hd=((_tmp10C3=_cycalloc(
sizeof(*_tmp10C3)),((_tmp10C3[0]=msg,_tmp10C3)))),((_tmp10C2->tl=Cyc_Tcutil_warning_msgs,
_tmp10C2))))));};}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)
return;{const char*_tmp10C6;void*_tmp10C5;(_tmp10C5=0,Cyc_fprintf(Cyc_stderr,((
_tmp10C6="***Warnings***\n",_tag_dyneither(_tmp10C6,sizeof(char),16))),
_tag_dyneither(_tmp10C5,sizeof(void*),0)));}{struct Cyc_List_List*_tmp36=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmp10CB;void*_tmp10CA[2];struct
Cyc_String_pa_struct _tmp10C9;struct Cyc_String_pa_struct _tmp10C8;(_tmp10C8.tag=0,((
_tmp10C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmp10C9.tag=
0,((_tmp10C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((_tmp10CA[0]=&
_tmp10C9,((_tmp10CA[1]=& _tmp10C8,Cyc_fprintf(Cyc_stderr,((_tmp10CB="%s: %s\n",
_tag_dyneither(_tmp10CB,sizeof(char),8))),_tag_dyneither(_tmp10CA,sizeof(void*),
2)))))))))))));}_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}{const char*_tmp10CE;void*_tmp10CD;(
_tmp10CD=0,Cyc_fprintf(Cyc_stderr,((_tmp10CE="**************\n",_tag_dyneither(
_tmp10CE,sizeof(char),16))),_tag_dyneither(_tmp10CD,sizeof(void*),0)));}Cyc_fflush((
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
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmp10CF;*_tmp44=((_tmp10CF=
_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF[0]=t2,_tmp10CF))));}return t2;}_LL7: {
struct Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(
_tmp45->tag != 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)&
_tmp45->f2;}}_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v){struct Cyc_Core_Opt*_tmp10D0;*_tmp47=((_tmp10D0=_cycalloc(sizeof(*
_tmp10D0)),((_tmp10D0->v=t2,_tmp10D0))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
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
struct Cyc_Absyn_Unknown_kb_struct _tmp10D3;struct Cyc_Absyn_Unknown_kb_struct*
_tmp10D2;return(void*)((_tmp10D2=_cycalloc(sizeof(*_tmp10D2)),((_tmp10D2[0]=((
_tmp10D3.tag=1,((_tmp10D3.f1=0,_tmp10D3)))),_tmp10D2))));}_LL1E: {struct Cyc_Absyn_Less_kb_struct*
_tmp5B=(struct Cyc_Absyn_Less_kb_struct*)_tmp59;if(_tmp5B->tag != 2)goto _LL20;
else{_tmp5C=_tmp5B->f2;}}_LL1F: {struct Cyc_Absyn_Less_kb_struct _tmp10D6;struct
Cyc_Absyn_Less_kb_struct*_tmp10D5;return(void*)((_tmp10D5=_cycalloc(sizeof(*
_tmp10D5)),((_tmp10D5[0]=((_tmp10D6.tag=2,((_tmp10D6.f1=0,((_tmp10D6.f2=_tmp5C,
_tmp10D6)))))),_tmp10D5))));}_LL20:;_LL21: return kb;_LL1B:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmp10D7;
return(_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7->name=tv->name,((
_tmp10D7->identity=- 1,((_tmp10D7->kind=Cyc_Tcutil_copy_kindbound(tv->kind),
_tmp10D7)))))));}static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
struct _tuple10 _tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Absyn_Tqual _tmp65;void*
_tmp66;struct _tuple10*_tmp62=arg;_tmp63=*_tmp62;_tmp64=_tmp63.f1;_tmp65=_tmp63.f2;
_tmp66=_tmp63.f3;{struct _tuple10*_tmp10D8;return(_tmp10D8=_cycalloc(sizeof(*
_tmp10D8)),((_tmp10D8->f1=_tmp64,((_tmp10D8->f2=_tmp65,((_tmp10D8->f3=Cyc_Tcutil_copy_type(
_tmp66),_tmp10D8)))))));};}static struct _tuple12*Cyc_Tcutil_copy_tqt(struct
_tuple12*arg){struct _tuple12 _tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;
struct _tuple12*_tmp68=arg;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple12*_tmp10D9;return(_tmp10D9=_cycalloc(sizeof(*_tmp10D9)),((_tmp10D9->f1=
_tmp6A,((_tmp10D9->f2=Cyc_Tcutil_copy_type(_tmp6B),_tmp10D9)))));};}static struct
Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp10DA;return(_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((_tmp10DA->name=f->name,((
_tmp10DA->tq=f->tq,((_tmp10DA->type=Cyc_Tcutil_copy_type(f->type),((_tmp10DA->width=
f->width,((_tmp10DA->attributes=f->attributes,_tmp10DA)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){struct _tuple0 _tmp6F;void*_tmp70;
void*_tmp71;struct _tuple0*_tmp6E=x;_tmp6F=*_tmp6E;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;{
struct _tuple0*_tmp10DB;return(_tmp10DB=_cycalloc(sizeof(*_tmp10DB)),((_tmp10DB->f1=
Cyc_Tcutil_copy_type(_tmp70),((_tmp10DB->f2=Cyc_Tcutil_copy_type(_tmp71),
_tmp10DB)))));};}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmp10DC;return(_tmp10DC=
_cycalloc(sizeof(*_tmp10DC)),((_tmp10DC->name=f->name,((_tmp10DC->tag=f->tag,((
_tmp10DC->loc=f->loc,_tmp10DC)))))));}static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*
t){struct Cyc_Absyn_VarType_struct _tmp10DF;struct Cyc_Absyn_VarType_struct*
_tmp10DE;return(void*)((_tmp10DE=_cycalloc(sizeof(*_tmp10DE)),((_tmp10DE[0]=((
_tmp10DF.tag=2,((_tmp10DF.f1=Cyc_Tcutil_copy_tvar(t),_tmp10DF)))),_tmp10DE))));}
void*Cyc_Tcutil_copy_type(void*t){void*_tmp76=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp7A;struct Cyc_Absyn_DatatypeInfo _tmp7C;union Cyc_Absyn_DatatypeInfoU _tmp7D;
struct Cyc_List_List*_tmp7E;struct Cyc_Absyn_DatatypeFieldInfo _tmp80;union Cyc_Absyn_DatatypeFieldInfoU
_tmp81;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_PtrInfo _tmp84;void*_tmp85;
struct Cyc_Absyn_Tqual _tmp86;struct Cyc_Absyn_PtrAtts _tmp87;void*_tmp88;union Cyc_Absyn_Constraint*
_tmp89;union Cyc_Absyn_Constraint*_tmp8A;union Cyc_Absyn_Constraint*_tmp8B;struct
Cyc_Absyn_PtrLoc*_tmp8C;struct Cyc_Absyn_ArrayInfo _tmp90;void*_tmp91;struct Cyc_Absyn_Tqual
_tmp92;struct Cyc_Absyn_Exp*_tmp93;union Cyc_Absyn_Constraint*_tmp94;unsigned int
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
else{_tmp7A=_tmp79->f1;}}_LL28: {struct Cyc_Absyn_VarType_struct _tmp10E2;struct
Cyc_Absyn_VarType_struct*_tmp10E1;return(void*)((_tmp10E1=_cycalloc(sizeof(*
_tmp10E1)),((_tmp10E1[0]=((_tmp10E2.tag=2,((_tmp10E2.f1=Cyc_Tcutil_copy_tvar(
_tmp7A),_tmp10E2)))),_tmp10E1))));}_LL29: {struct Cyc_Absyn_DatatypeType_struct*
_tmp7B=(struct Cyc_Absyn_DatatypeType_struct*)_tmp76;if(_tmp7B->tag != 3)goto _LL2B;
else{_tmp7C=_tmp7B->f1;_tmp7D=_tmp7C.datatype_info;_tmp7E=_tmp7C.targs;}}_LL2A: {
struct Cyc_Absyn_DatatypeType_struct _tmp10E8;struct Cyc_Absyn_DatatypeInfo _tmp10E7;
struct Cyc_Absyn_DatatypeType_struct*_tmp10E6;return(void*)((_tmp10E6=_cycalloc(
sizeof(*_tmp10E6)),((_tmp10E6[0]=((_tmp10E8.tag=3,((_tmp10E8.f1=((_tmp10E7.datatype_info=
_tmp7D,((_tmp10E7.targs=Cyc_Tcutil_copy_types(_tmp7E),_tmp10E7)))),_tmp10E8)))),
_tmp10E6))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp76;if(_tmp7F->tag != 4)goto _LL2D;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.field_info;
_tmp82=_tmp80.targs;}}_LL2C: {struct Cyc_Absyn_DatatypeFieldType_struct _tmp10EE;
struct Cyc_Absyn_DatatypeFieldInfo _tmp10ED;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp10EC;return(void*)((_tmp10EC=_cycalloc(sizeof(*_tmp10EC)),((_tmp10EC[0]=((
_tmp10EE.tag=4,((_tmp10EE.f1=((_tmp10ED.field_info=_tmp81,((_tmp10ED.targs=Cyc_Tcutil_copy_types(
_tmp82),_tmp10ED)))),_tmp10EE)))),_tmp10EC))));}_LL2D: {struct Cyc_Absyn_PointerType_struct*
_tmp83=(struct Cyc_Absyn_PointerType_struct*)_tmp76;if(_tmp83->tag != 5)goto _LL2F;
else{_tmp84=_tmp83->f1;_tmp85=_tmp84.elt_typ;_tmp86=_tmp84.elt_tq;_tmp87=_tmp84.ptr_atts;
_tmp88=_tmp87.rgn;_tmp89=_tmp87.nullable;_tmp8A=_tmp87.bounds;_tmp8B=_tmp87.zero_term;
_tmp8C=_tmp87.ptrloc;}}_LL2E: {void*_tmpE8=Cyc_Tcutil_copy_type(_tmp85);void*
_tmpE9=Cyc_Tcutil_copy_type(_tmp88);union Cyc_Absyn_Constraint*_tmpEA=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp89);struct Cyc_Absyn_Tqual
_tmpEB=_tmp86;union Cyc_Absyn_Constraint*_tmpEC=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp8A);union Cyc_Absyn_Constraint*
_tmpED=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp8B);struct Cyc_Absyn_PointerType_struct _tmp10F8;struct Cyc_Absyn_PtrAtts
_tmp10F7;struct Cyc_Absyn_PtrInfo _tmp10F6;struct Cyc_Absyn_PointerType_struct*
_tmp10F5;return(void*)((_tmp10F5=_cycalloc(sizeof(*_tmp10F5)),((_tmp10F5[0]=((
_tmp10F8.tag=5,((_tmp10F8.f1=((_tmp10F6.elt_typ=_tmpE8,((_tmp10F6.elt_tq=_tmpEB,((
_tmp10F6.ptr_atts=((_tmp10F7.rgn=_tmpE9,((_tmp10F7.nullable=_tmpEA,((_tmp10F7.bounds=
_tmpEC,((_tmp10F7.zero_term=_tmpED,((_tmp10F7.ptrloc=_tmp8C,_tmp10F7)))))))))),
_tmp10F6)))))),_tmp10F8)))),_tmp10F5))));}_LL2F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp76;if(_tmp8D->tag != 6)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_struct*_tmp8E=(struct Cyc_Absyn_FloatType_struct*)
_tmp76;if(_tmp8E->tag != 7)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8F=(struct Cyc_Absyn_ArrayType_struct*)_tmp76;if(_tmp8F->tag != 8)goto _LL35;
else{_tmp90=_tmp8F->f1;_tmp91=_tmp90.elt_type;_tmp92=_tmp90.tq;_tmp93=_tmp90.num_elts;
_tmp94=_tmp90.zero_term;_tmp95=_tmp90.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmp10FE;struct Cyc_Absyn_ArrayInfo _tmp10FD;struct Cyc_Absyn_ArrayType_struct*
_tmp10FC;return(void*)((_tmp10FC=_cycalloc(sizeof(*_tmp10FC)),((_tmp10FC[0]=((
_tmp10FE.tag=8,((_tmp10FE.f1=((_tmp10FD.elt_type=Cyc_Tcutil_copy_type(_tmp91),((
_tmp10FD.tq=_tmp92,((_tmp10FD.num_elts=_tmp93,((_tmp10FD.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp94),((_tmp10FD.zt_loc=
_tmp95,_tmp10FD)))))))))),_tmp10FE)))),_tmp10FC))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp96=(struct Cyc_Absyn_FnType_struct*)_tmp76;if(_tmp96->tag != 9)goto _LL37;else{
_tmp97=_tmp96->f1;_tmp98=_tmp97.tvars;_tmp99=_tmp97.effect;_tmp9A=_tmp97.ret_tqual;
_tmp9B=_tmp97.ret_typ;_tmp9C=_tmp97.args;_tmp9D=_tmp97.c_varargs;_tmp9E=_tmp97.cyc_varargs;
_tmp9F=_tmp97.rgn_po;_tmpA0=_tmp97.attributes;}}_LL36: {struct Cyc_List_List*
_tmpF5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp98);struct Cyc_Core_Opt*
_tmp10FF;struct Cyc_Core_Opt*_tmpF6=_tmp99 == 0?0:((_tmp10FF=_cycalloc(sizeof(*
_tmp10FF)),((_tmp10FF->v=Cyc_Tcutil_copy_type((void*)_tmp99->v),_tmp10FF))));
void*_tmpF7=Cyc_Tcutil_copy_type(_tmp9B);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp9C);int _tmpF9=_tmp9D;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp9E != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp9E;struct Cyc_Absyn_VarargInfo*
_tmp1100;cyc_varargs2=((_tmp1100=_cycalloc(sizeof(*_tmp1100)),((_tmp1100->name=
cv->name,((_tmp1100->tq=cv->tq,((_tmp1100->type=Cyc_Tcutil_copy_type(cv->type),((
_tmp1100->inject=cv->inject,_tmp1100))))))))));}{struct Cyc_List_List*_tmpFB=((
struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);struct Cyc_List_List*_tmpFC=_tmpA0;
struct Cyc_Absyn_FnType_struct _tmp1106;struct Cyc_Absyn_FnInfo _tmp1105;struct Cyc_Absyn_FnType_struct*
_tmp1104;return(void*)((_tmp1104=_cycalloc(sizeof(*_tmp1104)),((_tmp1104[0]=((
_tmp1106.tag=9,((_tmp1106.f1=((_tmp1105.tvars=_tmpF5,((_tmp1105.effect=_tmpF6,((
_tmp1105.ret_tqual=_tmp9A,((_tmp1105.ret_typ=_tmpF7,((_tmp1105.args=_tmpF8,((
_tmp1105.c_varargs=_tmpF9,((_tmp1105.cyc_varargs=cyc_varargs2,((_tmp1105.rgn_po=
_tmpFB,((_tmp1105.attributes=_tmpFC,_tmp1105)))))))))))))))))),_tmp1106)))),
_tmp1104))));};}_LL37: {struct Cyc_Absyn_TupleType_struct*_tmpA1=(struct Cyc_Absyn_TupleType_struct*)
_tmp76;if(_tmpA1->tag != 10)goto _LL39;else{_tmpA2=_tmpA1->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmp1109;struct Cyc_Absyn_TupleType_struct*_tmp1108;return(void*)((_tmp1108=
_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((_tmp1109.tag=10,((_tmp1109.f1=((
struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA2),_tmp1109)))),_tmp1108))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmpA3=(struct Cyc_Absyn_AggrType_struct*)_tmp76;
if(_tmpA3->tag != 11)goto _LL3B;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA4.aggr_info;if((
_tmpA5.UnknownAggr).tag != 1)goto _LL3B;_tmpA6=(struct _tuple4)(_tmpA5.UnknownAggr).val;
_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_tmpA9=_tmpA6.f3;_tmpAA=_tmpA4.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmp110F;struct Cyc_Absyn_AggrInfo _tmp110E;struct
Cyc_Absyn_AggrType_struct*_tmp110D;return(void*)((_tmp110D=_cycalloc(sizeof(*
_tmp110D)),((_tmp110D[0]=((_tmp110F.tag=11,((_tmp110F.f1=((_tmp110E.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpA7,_tmpA8,_tmpA9),((_tmp110E.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmp110E)))),_tmp110F)))),_tmp110D))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*
_tmpAB=(struct Cyc_Absyn_AggrType_struct*)_tmp76;if(_tmpAB->tag != 11)goto _LL3D;
else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAC.aggr_info;if((_tmpAD.KnownAggr).tag != 2)
goto _LL3D;_tmpAE=(struct Cyc_Absyn_Aggrdecl**)(_tmpAD.KnownAggr).val;_tmpAF=
_tmpAC.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct _tmp1115;struct Cyc_Absyn_AggrInfo
_tmp1114;struct Cyc_Absyn_AggrType_struct*_tmp1113;return(void*)((_tmp1113=
_cycalloc(sizeof(*_tmp1113)),((_tmp1113[0]=((_tmp1115.tag=11,((_tmp1115.f1=((
_tmp1114.aggr_info=Cyc_Absyn_KnownAggr(_tmpAE),((_tmp1114.targs=Cyc_Tcutil_copy_types(
_tmpAF),_tmp1114)))),_tmp1115)))),_tmp1113))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB0=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp76;if(_tmpB0->tag != 12)goto
_LL3F;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmp1118;struct Cyc_Absyn_AnonAggrType_struct*_tmp1117;return(void*)((_tmp1117=
_cycalloc(sizeof(*_tmp1117)),((_tmp1117[0]=((_tmp1118.tag=12,((_tmp1118.f1=
_tmpB1,((_tmp1118.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmpB2),_tmp1118)))))),_tmp1117))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmpB3=(struct Cyc_Absyn_EnumType_struct*)_tmp76;if(_tmpB3->tag != 13)goto _LL41;
else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL40: {struct Cyc_Absyn_EnumType_struct
_tmp111B;struct Cyc_Absyn_EnumType_struct*_tmp111A;return(void*)((_tmp111A=
_cycalloc(sizeof(*_tmp111A)),((_tmp111A[0]=((_tmp111B.tag=13,((_tmp111B.f1=
_tmpB4,((_tmp111B.f2=_tmpB5,_tmp111B)))))),_tmp111A))));}_LL41: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB6=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp76;if(_tmpB6->tag != 14)goto
_LL43;else{_tmpB7=_tmpB6->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmp111E;struct Cyc_Absyn_AnonEnumType_struct*_tmp111D;return(void*)((_tmp111D=
_cycalloc(sizeof(*_tmp111D)),((_tmp111D[0]=((_tmp111E.tag=14,((_tmp111E.f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB7),_tmp111E)))),
_tmp111D))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB8=(struct Cyc_Absyn_TagType_struct*)
_tmp76;if(_tmpB8->tag != 19)goto _LL45;else{_tmpB9=(void*)_tmpB8->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmp1121;struct Cyc_Absyn_TagType_struct*_tmp1120;
return(void*)((_tmp1120=_cycalloc(sizeof(*_tmp1120)),((_tmp1120[0]=((_tmp1121.tag=
19,((_tmp1121.f1=(void*)Cyc_Tcutil_copy_type(_tmpB9),_tmp1121)))),_tmp1120))));}
_LL45: {struct Cyc_Absyn_ValueofType_struct*_tmpBA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp76;if(_tmpBA->tag != 18)goto _LL47;else{_tmpBB=_tmpBA->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmp1124;struct Cyc_Absyn_ValueofType_struct*_tmp1123;return(void*)((_tmp1123=
_cycalloc(sizeof(*_tmp1123)),((_tmp1123[0]=((_tmp1124.tag=18,((_tmp1124.f1=
_tmpBB,_tmp1124)))),_tmp1123))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*
_tmpBC=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp76;if(_tmpBC->tag != 15)goto
_LL49;else{_tmpBD=(void*)_tmpBC->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct
_tmp1127;struct Cyc_Absyn_RgnHandleType_struct*_tmp1126;return(void*)((_tmp1126=
_cycalloc(sizeof(*_tmp1126)),((_tmp1126[0]=((_tmp1127.tag=15,((_tmp1127.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBD),_tmp1127)))),_tmp1126))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpBE=(struct Cyc_Absyn_DynRgnType_struct*)_tmp76;if(_tmpBE->tag != 16)goto _LL4B;
else{_tmpBF=(void*)_tmpBE->f1;_tmpC0=(void*)_tmpBE->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmp112A;struct Cyc_Absyn_DynRgnType_struct*_tmp1129;return(void*)((_tmp1129=
_cycalloc(sizeof(*_tmp1129)),((_tmp1129[0]=((_tmp112A.tag=16,((_tmp112A.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBF),((_tmp112A.f2=(void*)Cyc_Tcutil_copy_type(_tmpC0),
_tmp112A)))))),_tmp1129))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpC1=(
struct Cyc_Absyn_TypedefType_struct*)_tmp76;if(_tmpC1->tag != 17)goto _LL4D;else{
_tmpC2=_tmpC1->f1;_tmpC3=_tmpC1->f2;_tmpC4=_tmpC1->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmp112D;struct Cyc_Absyn_TypedefType_struct*_tmp112C;return(void*)((_tmp112C=
_cycalloc(sizeof(*_tmp112C)),((_tmp112C[0]=((_tmp112D.tag=17,((_tmp112D.f1=
_tmpC2,((_tmp112D.f2=Cyc_Tcutil_copy_types(_tmpC3),((_tmp112D.f3=_tmpC4,((
_tmp112D.f4=0,_tmp112D)))))))))),_tmp112C))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC5=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp76;if(_tmpC5->tag != 21)goto _LL4F;}
_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_struct*_tmpC6=(struct Cyc_Absyn_RefCntRgn_struct*)
_tmp76;if(_tmpC6->tag != 22)goto _LL51;}_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC7=(struct Cyc_Absyn_HeapRgn_struct*)_tmp76;if(_tmpC7->tag != 20)goto _LL53;}
_LL52: return t;_LL53: {struct Cyc_Absyn_AccessEff_struct*_tmpC8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp76;if(_tmpC8->tag != 23)goto _LL55;else{_tmpC9=(void*)_tmpC8->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_struct _tmp1130;struct Cyc_Absyn_AccessEff_struct*
_tmp112F;return(void*)((_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F[0]=((
_tmp1130.tag=23,((_tmp1130.f1=(void*)Cyc_Tcutil_copy_type(_tmpC9),_tmp1130)))),
_tmp112F))));}_LL55: {struct Cyc_Absyn_JoinEff_struct*_tmpCA=(struct Cyc_Absyn_JoinEff_struct*)
_tmp76;if(_tmpCA->tag != 24)goto _LL57;else{_tmpCB=_tmpCA->f1;}}_LL56: {struct Cyc_Absyn_JoinEff_struct
_tmp1133;struct Cyc_Absyn_JoinEff_struct*_tmp1132;return(void*)((_tmp1132=
_cycalloc(sizeof(*_tmp1132)),((_tmp1132[0]=((_tmp1133.tag=24,((_tmp1133.f1=Cyc_Tcutil_copy_types(
_tmpCB),_tmp1133)))),_tmp1132))));}_LL57: {struct Cyc_Absyn_RgnsEff_struct*_tmpCC=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp76;if(_tmpCC->tag != 25)goto _LL59;else{_tmpCD=(
void*)_tmpCC->f1;}}_LL58: {struct Cyc_Absyn_RgnsEff_struct _tmp1136;struct Cyc_Absyn_RgnsEff_struct*
_tmp1135;return(void*)((_tmp1135=_cycalloc(sizeof(*_tmp1135)),((_tmp1135[0]=((
_tmp1136.tag=25,((_tmp1136.f1=(void*)Cyc_Tcutil_copy_type(_tmpCD),_tmp1136)))),
_tmp1135))));}_LL59: {struct Cyc_Absyn_TypeDeclType_struct*_tmpCE=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpCE->tag != 26)goto _LL5B;else{_tmpCF=_tmpCE->f1;_tmpD0=*_tmpCF;
_tmpD1=_tmpD0.r;{struct Cyc_Absyn_Aggr_td_struct*_tmpD2=(struct Cyc_Absyn_Aggr_td_struct*)
_tmpD1;if(_tmpD2->tag != 0)goto _LL5B;else{_tmpD3=_tmpD2->f1;}};}}_LL5A: {struct
Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD3->tvs);struct Cyc_Absyn_AggrType_struct
_tmp113C;struct Cyc_Absyn_AggrInfo _tmp113B;struct Cyc_Absyn_AggrType_struct*
_tmp113A;return(void*)((_tmp113A=_cycalloc(sizeof(*_tmp113A)),((_tmp113A[0]=((
_tmp113C.tag=11,((_tmp113C.f1=((_tmp113B.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD3->kind,
_tmpD3->name,0),((_tmp113B.targs=_tmp11F,_tmp113B)))),_tmp113C)))),_tmp113A))));}
_LL5B: {struct Cyc_Absyn_TypeDeclType_struct*_tmpD4=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpD4->tag != 26)goto _LL5D;else{_tmpD5=_tmpD4->f1;_tmpD6=*_tmpD5;
_tmpD7=_tmpD6.r;{struct Cyc_Absyn_Enum_td_struct*_tmpD8=(struct Cyc_Absyn_Enum_td_struct*)
_tmpD7;if(_tmpD8->tag != 1)goto _LL5D;else{_tmpD9=_tmpD8->f1;}};}}_LL5C: {struct
Cyc_Absyn_EnumType_struct _tmp113F;struct Cyc_Absyn_EnumType_struct*_tmp113E;
return(void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((_tmp113E[0]=((_tmp113F.tag=
13,((_tmp113F.f1=_tmpD9->name,((_tmp113F.f2=0,_tmp113F)))))),_tmp113E))));}_LL5D: {
struct Cyc_Absyn_TypeDeclType_struct*_tmpDA=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp76;if(_tmpDA->tag != 26)goto _LL22;else{_tmpDB=_tmpDA->f1;_tmpDC=*_tmpDB;
_tmpDD=_tmpDC.r;{struct Cyc_Absyn_Datatype_td_struct*_tmpDE=(struct Cyc_Absyn_Datatype_td_struct*)
_tmpDD;if(_tmpDE->tag != 2)goto _LL22;else{_tmpDF=_tmpDE->f1;}};}}_LL5E: {struct
Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDF->tvs);struct Cyc_Absyn_DatatypeType_struct
_tmp1149;struct Cyc_Absyn_UnknownDatatypeInfo _tmp1148;struct Cyc_Absyn_DatatypeInfo
_tmp1147;struct Cyc_Absyn_DatatypeType_struct*_tmp1146;return(void*)((_tmp1146=
_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1149.tag=3,((_tmp1149.f1=((
_tmp1147.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmp1148.name=_tmpDF->name,((
_tmp1148.is_extensible=0,_tmp1148))))),((_tmp1147.targs=_tmp125,_tmp1147)))),
_tmp1149)))),_tmp1146))));}_LL22:;}static void*Cyc_Tcutil_copy_designator(void*d){
void*_tmp12A=d;struct Cyc_Absyn_Exp*_tmp12C;struct _dyneither_ptr*_tmp12E;_LL60: {
struct Cyc_Absyn_ArrayElement_struct*_tmp12B=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp12A;if(_tmp12B->tag != 0)goto _LL62;else{_tmp12C=_tmp12B->f1;}}_LL61: {struct
Cyc_Absyn_ArrayElement_struct _tmp114C;struct Cyc_Absyn_ArrayElement_struct*
_tmp114B;return(void*)((_tmp114B=_cycalloc(sizeof(*_tmp114B)),((_tmp114B[0]=((
_tmp114C.tag=0,((_tmp114C.f1=Cyc_Tcutil_deep_copy_exp(_tmp12C),_tmp114C)))),
_tmp114B))));}_LL62: {struct Cyc_Absyn_FieldName_struct*_tmp12D=(struct Cyc_Absyn_FieldName_struct*)
_tmp12A;if(_tmp12D->tag != 1)goto _LL5F;else{_tmp12E=_tmp12D->f1;}}_LL63: return d;
_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
struct _tuple19*Cyc_Tcutil_copy_eds(struct _tuple19*e){struct _tuple19*_tmp114D;
return(_tmp114D=_cycalloc(sizeof(*_tmp114D)),((_tmp114D->f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_designator,(
e[0]).f1),((_tmp114D->f2=Cyc_Tcutil_deep_copy_exp((e[0]).f2),_tmp114D)))));}
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
_tmp140=_tmp13D->f3;}}_LL6E: {struct Cyc_Core_Opt*_tmp114E;return Cyc_Absyn_assignop_exp(
Cyc_Tcutil_deep_copy_exp(_tmp13E),(unsigned int)_tmp13F?(_tmp114E=
_cycalloc_atomic(sizeof(*_tmp114E)),((_tmp114E->v=(void*)((enum Cyc_Absyn_Primop)
_tmp13F->v),_tmp114E))): 0,Cyc_Tcutil_deep_copy_exp(_tmp140),e->loc);}_LL6F: {
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
struct Cyc_Absyn_FnCall_e_struct _tmp1158;struct Cyc_Absyn_VarargInfo*_tmp1157;
struct Cyc_Absyn_VarargCallInfo*_tmp1156;struct Cyc_Absyn_FnCall_e_struct*_tmp1155;
return Cyc_Absyn_new_exp((void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((
_tmp1155[0]=((_tmp1158.tag=11,((_tmp1158.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp1158.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp156),((
_tmp1158.f3=((_tmp1156=_cycalloc(sizeof(*_tmp1156)),((_tmp1156->num_varargs=
_tmp1BC,((_tmp1156->injectors=_tmp1BD,((_tmp1156->vai=((_tmp1157=_cycalloc(
sizeof(*_tmp1157)),((_tmp1157->name=_tmp1C1,((_tmp1157->tq=_tmp1C2,((_tmp1157->type=
Cyc_Tcutil_copy_type(_tmp1C3),((_tmp1157->inject=_tmp1C4,_tmp1157)))))))))),
_tmp1156)))))))),_tmp1158)))))))),_tmp1155)))),e->loc);};}_LLBA:;_LLBB: {struct
Cyc_Absyn_FnCall_e_struct _tmp115B;struct Cyc_Absyn_FnCall_e_struct*_tmp115A;
return Cyc_Absyn_new_exp((void*)((_tmp115A=_cycalloc(sizeof(*_tmp115A)),((
_tmp115A[0]=((_tmp115B.tag=11,((_tmp115B.f1=Cyc_Tcutil_deep_copy_exp(_tmp155),((
_tmp115B.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,_tmp156),((
_tmp115B.f3=_tmp157,_tmp115B)))))))),_tmp115A)))),e->loc);}_LLB7:;}_LL7D: {
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
_tmp172->f4;}}_LL92: {struct Cyc_Absyn_AggrMember_e_struct _tmp115E;struct Cyc_Absyn_AggrMember_e_struct*
_tmp115D;return Cyc_Absyn_new_exp((void*)((_tmp115D=_cycalloc(sizeof(*_tmp115D)),((
_tmp115D[0]=((_tmp115E.tag=22,((_tmp115E.f1=Cyc_Tcutil_deep_copy_exp(_tmp173),((
_tmp115E.f2=_tmp174,((_tmp115E.f3=_tmp175,((_tmp115E.f4=_tmp176,_tmp115E)))))))))),
_tmp115D)))),e->loc);}_LL93: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp177=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp132;if(_tmp177->tag != 23)goto _LL95;else{
_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;_tmp17A=_tmp177->f3;_tmp17B=_tmp177->f4;}}
_LL94: {struct Cyc_Absyn_AggrArrow_e_struct _tmp1161;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp1160;return Cyc_Absyn_new_exp((void*)((_tmp1160=_cycalloc(sizeof(*_tmp1160)),((
_tmp1160[0]=((_tmp1161.tag=23,((_tmp1161.f1=Cyc_Tcutil_deep_copy_exp(_tmp178),((
_tmp1161.f2=_tmp179,((_tmp1161.f3=_tmp17A,((_tmp1161.f4=_tmp17B,_tmp1161)))))))))),
_tmp1160)))),e->loc);}_LL95: {struct Cyc_Absyn_Subscript_e_struct*_tmp17C=(struct
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
_tmp1162;vopt1=((_tmp1162=_cycalloc(sizeof(*_tmp1162)),((_tmp1162->v=(struct
_dyneither_ptr*)_tmp184->v,_tmp1162))));}{struct Cyc_Absyn_CompoundLit_e_struct
_tmp1168;struct _tuple10*_tmp1167;struct Cyc_Absyn_CompoundLit_e_struct*_tmp1166;
return Cyc_Absyn_new_exp((void*)((_tmp1166=_cycalloc(sizeof(*_tmp1166)),((
_tmp1166[0]=((_tmp1168.tag=26,((_tmp1168.f1=((_tmp1167=_cycalloc(sizeof(*
_tmp1167)),((_tmp1167->f1=vopt1,((_tmp1167->f2=_tmp185,((_tmp1167->f3=Cyc_Tcutil_copy_type(
_tmp186),_tmp1167)))))))),((_tmp1168.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*
f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp187),_tmp1168)))))),_tmp1166)))),e->loc);};}_LL9B: {struct Cyc_Absyn_Array_e_struct*
_tmp188=(struct Cyc_Absyn_Array_e_struct*)_tmp132;if(_tmp188->tag != 27)goto _LL9D;
else{_tmp189=_tmp188->f1;}}_LL9C: {struct Cyc_Absyn_Array_e_struct _tmp116B;struct
Cyc_Absyn_Array_e_struct*_tmp116A;return Cyc_Absyn_new_exp((void*)((_tmp116A=
_cycalloc(sizeof(*_tmp116A)),((_tmp116A[0]=((_tmp116B.tag=27,((_tmp116B.f1=((
struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp189),_tmp116B)))),_tmp116A)))),e->loc);}
_LL9D: {struct Cyc_Absyn_Comprehension_e_struct*_tmp18A=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp132;if(_tmp18A->tag != 28)goto _LL9F;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;
_tmp18D=_tmp18A->f3;_tmp18E=_tmp18A->f4;}}_LL9E: {struct Cyc_Absyn_Comprehension_e_struct
_tmp116E;struct Cyc_Absyn_Comprehension_e_struct*_tmp116D;return Cyc_Absyn_new_exp((
void*)((_tmp116D=_cycalloc(sizeof(*_tmp116D)),((_tmp116D[0]=((_tmp116E.tag=28,((
_tmp116E.f1=_tmp18B,((_tmp116E.f2=Cyc_Tcutil_deep_copy_exp(_tmp18C),((_tmp116E.f3=
Cyc_Tcutil_deep_copy_exp(_tmp18D),((_tmp116E.f4=_tmp18E,_tmp116E)))))))))),
_tmp116D)))),e->loc);}_LL9F: {struct Cyc_Absyn_Aggregate_e_struct*_tmp18F=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp132;if(_tmp18F->tag != 29)goto _LLA1;else{
_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;_tmp192=_tmp18F->f3;_tmp193=_tmp18F->f4;}}
_LLA0: {struct Cyc_Absyn_Aggregate_e_struct _tmp1171;struct Cyc_Absyn_Aggregate_e_struct*
_tmp1170;return Cyc_Absyn_new_exp((void*)((_tmp1170=_cycalloc(sizeof(*_tmp1170)),((
_tmp1170[0]=((_tmp1171.tag=29,((_tmp1171.f1=_tmp190,((_tmp1171.f2=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,
_tmp191),((_tmp1171.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct
_tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,_tmp192),((
_tmp1171.f4=_tmp193,_tmp1171)))))))))),_tmp1170)))),e->loc);}_LLA1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp194=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp132;if(_tmp194->tag != 30)goto
_LLA3;else{_tmp195=(void*)_tmp194->f1;_tmp196=_tmp194->f2;}}_LLA2: {struct Cyc_Absyn_AnonStruct_e_struct
_tmp1174;struct Cyc_Absyn_AnonStruct_e_struct*_tmp1173;return Cyc_Absyn_new_exp((
void*)((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173[0]=((_tmp1174.tag=30,((
_tmp1174.f1=(void*)Cyc_Tcutil_copy_type(_tmp195),((_tmp1174.f2=((struct Cyc_List_List*(*)(
struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp196),_tmp1174)))))),_tmp1173)))),e->loc);}_LLA3: {struct Cyc_Absyn_Datatype_e_struct*
_tmp197=(struct Cyc_Absyn_Datatype_e_struct*)_tmp132;if(_tmp197->tag != 31)goto
_LLA5;else{_tmp198=_tmp197->f1;_tmp199=_tmp197->f2;_tmp19A=_tmp197->f3;}}_LLA4: {
struct Cyc_Absyn_Datatype_e_struct _tmp1177;struct Cyc_Absyn_Datatype_e_struct*
_tmp1176;return Cyc_Absyn_new_exp((void*)((_tmp1176=_cycalloc(sizeof(*_tmp1176)),((
_tmp1176[0]=((_tmp1177.tag=31,((_tmp1177.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_deep_copy_exp,
_tmp198),((_tmp1177.f2=_tmp199,((_tmp1177.f3=_tmp19A,_tmp1177)))))))),_tmp1176)))),
e->loc);}_LLA5: {struct Cyc_Absyn_Enum_e_struct*_tmp19B=(struct Cyc_Absyn_Enum_e_struct*)
_tmp132;if(_tmp19B->tag != 32)goto _LLA7;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;
_tmp19E=_tmp19B->f3;}}_LLA6: return e;_LLA7: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp19F=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp132;if(_tmp19F->tag != 33)goto
_LLA9;else{_tmp1A0=_tmp19F->f1;_tmp1A1=(void*)_tmp19F->f2;_tmp1A2=_tmp19F->f3;}}
_LLA8: {struct Cyc_Absyn_AnonEnum_e_struct _tmp117A;struct Cyc_Absyn_AnonEnum_e_struct*
_tmp1179;return Cyc_Absyn_new_exp((void*)((_tmp1179=_cycalloc(sizeof(*_tmp1179)),((
_tmp1179[0]=((_tmp117A.tag=33,((_tmp117A.f1=_tmp1A0,((_tmp117A.f2=(void*)Cyc_Tcutil_copy_type(
_tmp1A1),((_tmp117A.f3=_tmp1A2,_tmp117A)))))))),_tmp1179)))),e->loc);}_LLA9: {
struct Cyc_Absyn_Malloc_e_struct*_tmp1A3=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp132;if(_tmp1A3->tag != 34)goto _LLAB;else{_tmp1A4=_tmp1A3->f1;_tmp1A5=_tmp1A4.is_calloc;
_tmp1A6=_tmp1A4.rgn;_tmp1A7=_tmp1A4.elt_type;_tmp1A8=_tmp1A4.num_elts;_tmp1A9=
_tmp1A4.fat_result;}}_LLAA: {struct Cyc_Absyn_Exp*_tmp1DF=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp1A6;if(_tmp1A6 != 0)r1=(struct Cyc_Absyn_Exp*)Cyc_Tcutil_deep_copy_exp((
struct Cyc_Absyn_Exp*)_tmp1A6);{void**t1=_tmp1A7;if(_tmp1A7 != 0){void**_tmp117B;
t1=((_tmp117B=_cycalloc(sizeof(*_tmp117B)),((_tmp117B[0]=Cyc_Tcutil_copy_type(*
_tmp1A7),_tmp117B))));}{struct Cyc_Absyn_Malloc_e_struct _tmp1181;struct Cyc_Absyn_MallocInfo
_tmp1180;struct Cyc_Absyn_Malloc_e_struct*_tmp117F;_tmp1DF->r=(void*)((_tmp117F=
_cycalloc(sizeof(*_tmp117F)),((_tmp117F[0]=((_tmp1181.tag=34,((_tmp1181.f1=((
_tmp1180.is_calloc=_tmp1A5,((_tmp1180.rgn=r1,((_tmp1180.elt_type=t1,((_tmp1180.num_elts=
_tmp1A8,((_tmp1180.fat_result=_tmp1A9,_tmp1180)))))))))),_tmp1181)))),_tmp117F))));}
return _tmp1DF;};}_LLAB: {struct Cyc_Absyn_Swap_e_struct*_tmp1AA=(struct Cyc_Absyn_Swap_e_struct*)
_tmp132;if(_tmp1AA->tag != 35)goto _LLAD;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=_tmp1AA->f2;}}
_LLAC: return Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp1AB),Cyc_Tcutil_deep_copy_exp(
_tmp1AC),e->loc);_LLAD: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp1AD=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp132;if(_tmp1AD->tag != 36)goto _LLAF;else{
_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AD->f2;}}_LLAE: {struct Cyc_Core_Opt*nopt1=
_tmp1AE;if(_tmp1AE != 0){struct Cyc_Core_Opt*_tmp1182;nopt1=((_tmp1182=_cycalloc(
sizeof(*_tmp1182)),((_tmp1182->v=(struct _tuple2*)_tmp1AE->v,_tmp1182))));}{
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp1185;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1184;return Cyc_Absyn_new_exp((void*)((_tmp1184=_cycalloc(sizeof(*_tmp1184)),((
_tmp1184[0]=((_tmp1185.tag=36,((_tmp1185.f1=nopt1,((_tmp1185.f2=((struct Cyc_List_List*(*)(
struct _tuple19*(*f)(struct _tuple19*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_eds,
_tmp1AF),_tmp1185)))))),_tmp1184)))),e->loc);};}_LLAF: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp1B0=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp132;if(_tmp1B0->tag != 37)goto
_LLB1;}_LLB0: {struct Cyc_Core_Failure_struct _tmp118B;const char*_tmp118A;struct
Cyc_Core_Failure_struct*_tmp1189;(int)_throw((void*)((_tmp1189=_cycalloc(sizeof(*
_tmp1189)),((_tmp1189[0]=((_tmp118B.tag=Cyc_Core_Failure,((_tmp118B.f1=((
_tmp118A="deep_copy: statement expressions unsupported",_tag_dyneither(_tmp118A,
sizeof(char),45))),_tmp118B)))),_tmp1189)))));}_LLB1: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1B1=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp132;if(_tmp1B1->tag != 38)goto
_LLB3;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=_tmp1B1->f2;}}_LLB2: {struct Cyc_Absyn_Tagcheck_e_struct
_tmp118E;struct Cyc_Absyn_Tagcheck_e_struct*_tmp118D;return Cyc_Absyn_new_exp((
void*)((_tmp118D=_cycalloc(sizeof(*_tmp118D)),((_tmp118D[0]=((_tmp118E.tag=38,((
_tmp118E.f1=Cyc_Tcutil_deep_copy_exp(_tmp1B2),((_tmp118E.f2=_tmp1B3,_tmp118E)))))),
_tmp118D)))),e->loc);}_LLB3: {struct Cyc_Absyn_Valueof_e_struct*_tmp1B4=(struct
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
_tmp1EE != _tmp1F2){struct _tuple20 _tmp118F;struct _tuple20 _tmp1F5=(_tmp118F.f1=
_tmp1EE,((_tmp118F.f2=_tmp1F2,_tmp118F)));enum Cyc_Absyn_KindQual _tmp1F6;enum 
Cyc_Absyn_KindQual _tmp1F7;enum Cyc_Absyn_KindQual _tmp1F8;enum Cyc_Absyn_KindQual
_tmp1F9;enum Cyc_Absyn_KindQual _tmp1FA;enum Cyc_Absyn_KindQual _tmp1FB;_LLBD:
_tmp1F6=_tmp1F5.f1;if(_tmp1F6 != Cyc_Absyn_BoxKind)goto _LLBF;_tmp1F7=_tmp1F5.f2;
if(_tmp1F7 != Cyc_Absyn_MemKind)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1F8=_tmp1F5.f1;
if(_tmp1F8 != Cyc_Absyn_BoxKind)goto _LLC1;_tmp1F9=_tmp1F5.f2;if(_tmp1F9 != Cyc_Absyn_AnyKind)
goto _LLC1;_LLC0: goto _LLC2;_LLC1: _tmp1FA=_tmp1F5.f1;if(_tmp1FA != Cyc_Absyn_MemKind)
goto _LLC3;_tmp1FB=_tmp1F5.f2;if(_tmp1FB != Cyc_Absyn_AnyKind)goto _LLC3;_LLC2: goto
_LLBC;_LLC3:;_LLC4: return 0;_LLBC:;}if(_tmp1EF != _tmp1F3){struct _tuple21 _tmp1190;
struct _tuple21 _tmp1FD=(_tmp1190.f1=_tmp1EF,((_tmp1190.f2=_tmp1F3,_tmp1190)));
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
_LLD1:;_LLD2:{struct Cyc_Absyn_Less_kb_struct _tmp1193;struct Cyc_Absyn_Less_kb_struct*
_tmp1192;tv->kind=(void*)((_tmp1192=_cycalloc(sizeof(*_tmp1192)),((_tmp1192[0]=((
_tmp1193.tag=2,((_tmp1193.f1=0,((_tmp1193.f2=def,_tmp1193)))))),_tmp1192))));}
return def;_LLCC:;}int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct _tuple0
_tmp1194;struct _tuple0 _tmp20A=(_tmp1194.f1=Cyc_Absyn_compress_kb(kb1),((_tmp1194.f2=
Cyc_Absyn_compress_kb(kb2),_tmp1194)));void*_tmp20B;struct Cyc_Absyn_Kind*_tmp20D;
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
struct Cyc_Core_Opt*_tmp1195;*_tmp214=((_tmp1195=_cycalloc(sizeof(*_tmp1195)),((
_tmp1195->v=kb2,_tmp1195))));}return 1;}else{return 0;}_LLD8: _tmp219=_tmp20A.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp21A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp219;if(
_tmp21A->tag != 0)goto _LLDA;else{_tmp21B=_tmp21A->f1;}};_tmp21C=_tmp20A.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp21D=(struct Cyc_Absyn_Less_kb_struct*)_tmp21C;
if(_tmp21D->tag != 2)goto _LLDA;else{_tmp21E=_tmp21D->f1;_tmp21F=(struct Cyc_Core_Opt**)&
_tmp21D->f1;_tmp220=_tmp21D->f2;}};_LLD9: if(Cyc_Tcutil_kind_leq(_tmp21B,_tmp220)){{
struct Cyc_Core_Opt*_tmp1196;*_tmp21F=((_tmp1196=_cycalloc(sizeof(*_tmp1196)),((
_tmp1196->v=kb1,_tmp1196))));}return 1;}else{return 0;}_LLDA: _tmp221=_tmp20A.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp222=(struct Cyc_Absyn_Less_kb_struct*)_tmp221;
if(_tmp222->tag != 2)goto _LLDC;else{_tmp223=_tmp222->f1;_tmp224=(struct Cyc_Core_Opt**)&
_tmp222->f1;_tmp225=_tmp222->f2;}};_tmp226=_tmp20A.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp227=(struct Cyc_Absyn_Less_kb_struct*)_tmp226;if(_tmp227->tag != 2)goto _LLDC;
else{_tmp228=_tmp227->f1;_tmp229=(struct Cyc_Core_Opt**)& _tmp227->f1;_tmp22A=
_tmp227->f2;}};_LLDB: if(Cyc_Tcutil_kind_leq(_tmp225,_tmp22A)){{struct Cyc_Core_Opt*
_tmp1197;*_tmp229=((_tmp1197=_cycalloc(sizeof(*_tmp1197)),((_tmp1197->v=kb1,
_tmp1197))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp22A,_tmp225)){{struct Cyc_Core_Opt*
_tmp1198;*_tmp224=((_tmp1198=_cycalloc(sizeof(*_tmp1198)),((_tmp1198->v=kb2,
_tmp1198))));}return 1;}else{return 0;}}_LLDC: _tmp22B=_tmp20A.f1;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp22C=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp22B;if(_tmp22C->tag != 1)goto
_LLDE;else{_tmp22D=_tmp22C->f1;_tmp22E=(struct Cyc_Core_Opt**)& _tmp22C->f1;}};
_tmp22F=_tmp20A.f2;_LLDD: _tmp230=_tmp22F;_tmp234=_tmp22E;goto _LLDF;_LLDE: _tmp230=
_tmp20A.f1;_tmp231=_tmp20A.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp232=(struct
Cyc_Absyn_Unknown_kb_struct*)_tmp231;if(_tmp232->tag != 1)goto _LLD3;else{_tmp233=
_tmp232->f1;_tmp234=(struct Cyc_Core_Opt**)& _tmp232->f1;}};_LLDF:{struct Cyc_Core_Opt*
_tmp1199;*_tmp234=((_tmp1199=_cycalloc(sizeof(*_tmp1199)),((_tmp1199->v=_tmp230,
_tmp1199))));}return 1;_LLD3:;}struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp23A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp23C;_LLE1: {struct
Cyc_Absyn_VarType_struct*_tmp23B=(struct Cyc_Absyn_VarType_struct*)_tmp23A;if(
_tmp23B->tag != 2)goto _LLE3;else{_tmp23C=_tmp23B->f1;}}_LLE2: {void*_tmp23D=
_tmp23C->kind;_tmp23C->kind=kb;{struct _tuple16 _tmp119A;return(_tmp119A.f1=
_tmp23C,((_tmp119A.f2=_tmp23D,_tmp119A)));};}_LLE3:;_LLE4: {const char*_tmp119E;
void*_tmp119D[1];struct Cyc_String_pa_struct _tmp119C;(_tmp119C.tag=0,((_tmp119C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp119D[0]=& _tmp119C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp119E="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmp119E,sizeof(char),40))),_tag_dyneither(_tmp119D,sizeof(void*),
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
_LL105: {const char*_tmp11A1;void*_tmp11A0;(_tmp11A0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A1="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmp11A1,sizeof(char),39))),_tag_dyneither(_tmp11A0,sizeof(void*),
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
_LL127;}_LL126: {struct Cyc_Absyn_Kind*_tmp11A2;return(_tmp11A2=_cycalloc_atomic(
sizeof(*_tmp11A2)),((_tmp11A2->kind=Cyc_Absyn_MemKind,((_tmp11A2->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp11A2)))));}_LL127: {struct Cyc_Absyn_Upper_b_struct*
_tmp28E=(struct Cyc_Absyn_Upper_b_struct*)_tmp28C;if(_tmp28E->tag != 1)goto _LL124;}
_LL128: {struct Cyc_Absyn_Kind*_tmp11A3;return(_tmp11A3=_cycalloc_atomic(sizeof(*
_tmp11A3)),((_tmp11A3->kind=Cyc_Absyn_BoxKind,((_tmp11A3->aliasqual=(Cyc_Tcutil_typ_kind((
_tmp26D.ptr_atts).rgn))->aliasqual,_tmp11A3)))));}_LL124:;}_LL10E: {struct Cyc_Absyn_ValueofType_struct*
_tmp26E=(struct Cyc_Absyn_ValueofType_struct*)_tmp242;if(_tmp26E->tag != 18)goto
_LL110;}_LL10F: return& Cyc_Tcutil_ik;_LL110: {struct Cyc_Absyn_TagType_struct*
_tmp26F=(struct Cyc_Absyn_TagType_struct*)_tmp242;if(_tmp26F->tag != 19)goto _LL112;}
_LL111: return& Cyc_Tcutil_bk;_LL112: {struct Cyc_Absyn_ArrayType_struct*_tmp270=(
struct Cyc_Absyn_ArrayType_struct*)_tmp242;if(_tmp270->tag != 8)goto _LL114;}_LL113:
goto _LL115;_LL114: {struct Cyc_Absyn_TupleType_struct*_tmp271=(struct Cyc_Absyn_TupleType_struct*)
_tmp242;if(_tmp271->tag != 10)goto _LL116;}_LL115: return& Cyc_Tcutil_mk;_LL116: {
struct Cyc_Absyn_TypedefType_struct*_tmp272=(struct Cyc_Absyn_TypedefType_struct*)
_tmp242;if(_tmp272->tag != 17)goto _LL118;else{_tmp273=_tmp272->f3;}}_LL117: if(
_tmp273 == 0  || _tmp273->kind == 0){const char*_tmp11A7;void*_tmp11A6[1];struct Cyc_String_pa_struct
_tmp11A5;(_tmp11A5.tag=0,((_tmp11A5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp11A6[0]=& _tmp11A5,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A7="typ_kind: typedef found: %s",
_tag_dyneither(_tmp11A7,sizeof(char),28))),_tag_dyneither(_tmp11A6,sizeof(void*),
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
Cyc_Tcutil_fast_tvar_cmp,env,_tmp29D)){{const char*_tmp11A8;Cyc_Tcutil_failure_reason=((
_tmp11A8="(type variable would escape scope)",_tag_dyneither(_tmp11A8,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}goto _LL12E;_LL131: {
struct Cyc_Absyn_Evar_struct*_tmp29E=(struct Cyc_Absyn_Evar_struct*)_tmp29B;if(
_tmp29E->tag != 1)goto _LL133;else{_tmp29F=_tmp29E->f2;_tmp2A0=_tmp29E->f4;_tmp2A1=(
struct Cyc_Core_Opt**)& _tmp29E->f4;}}_LL132: if(t == evar){{const char*_tmp11A9;Cyc_Tcutil_failure_reason=((
_tmp11A9="(occurs check)",_tag_dyneither(_tmp11A9,sizeof(char),15)));}(int)
_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(_tmp29F != 0)Cyc_Tcutil_occurs(evar,
r,env,(void*)_tmp29F->v);else{int problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp2A1))->v;for(0;s != 0;s=s->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp2CD=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp2A1))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmp11AA;_tmp2CD=((
_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA->hd=(struct Cyc_Absyn_Tvar*)s->hd,((
_tmp11AA->tl=_tmp2CD,_tmp11AA))))));}}}{struct Cyc_Core_Opt*_tmp11AB;*_tmp2A1=((
_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB->v=_tmp2CD,_tmp11AB))));};}}}
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
_tmp2D9=x;union Cyc_Absyn_Constraint _tmp2DA;int _tmp2DB;union Cyc_Absyn_Constraint
_tmp2DC;void*_tmp2DD;union Cyc_Absyn_Constraint _tmp2DE;union Cyc_Absyn_Constraint*
_tmp2DF;_LL152: _tmp2DA=*_tmp2D9;if((_tmp2DA.No_constr).tag != 3)goto _LL154;
_tmp2DB=(int)(_tmp2DA.No_constr).val;_LL153:{union Cyc_Absyn_Constraint _tmp11B3;*
x=(((_tmp11B3.Forward_constr).val=y,(((_tmp11B3.Forward_constr).tag=2,_tmp11B3))));}
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
char*_tmp11B6;void*_tmp11B5;(_tmp11B5=0,Cyc_Tcutil_impos(((_tmp11B6="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp11B6,sizeof(char),40))),_tag_dyneither(_tmp11B5,sizeof(void*),
0)));}_LL158:;}_LL156: _tmp2DE=*_tmp2D9;if((_tmp2DE.Forward_constr).tag != 2)goto
_LL151;_tmp2DF=(union Cyc_Absyn_Constraint*)(_tmp2DE.Forward_constr).val;_LL157: {
const char*_tmp11B9;void*_tmp11B8;(_tmp11B8=0,Cyc_Tcutil_impos(((_tmp11B9="unify_conref: forward after compress",
_tag_dyneither(_tmp11B9,sizeof(char),37))),_tag_dyneither(_tmp11B8,sizeof(void*),
0)));}_LL151:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct _handler_cons _tmp2EC;
_push_handler(& _tmp2EC);{int _tmp2EE=0;if(setjmp(_tmp2EC.handler))_tmp2EE=1;if(!
_tmp2EE){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp2EF=1;_npop_handler(0);return _tmp2EF;};;
_pop_handler();}else{void*_tmp2ED=(void*)_exn_thrown;void*_tmp2F1=_tmp2ED;_LL160: {
struct Cyc_Tcutil_Unify_struct*_tmp2F2=(struct Cyc_Tcutil_Unify_struct*)_tmp2F1;
if(_tmp2F2->tag != Cyc_Tcutil_Unify)goto _LL162;}_LL161: return 0;_LL162:;_LL163:(
void)_throw(_tmp2F1);_LL15F:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp11BA;struct _tuple0 _tmp2F4=(_tmp11BA.f1=b1,((_tmp11BA.f2=b2,
_tmp11BA)));void*_tmp2F5;void*_tmp2F7;void*_tmp2F9;void*_tmp2FB;void*_tmp2FD;
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
_tmp11BB;struct _tuple0 _tmp304=(_tmp11BB.f1=b1,((_tmp11BB.f2=b2,_tmp11BB)));void*
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
void*att1,void*att2){struct _tuple0 _tmp11BC;struct _tuple0 _tmp32A=(_tmp11BC.f1=
att1,((_tmp11BC.f2=att2,_tmp11BC)));void*_tmp32B;int _tmp32D;void*_tmp32E;int
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
_tmp11BF;struct Cyc_Absyn_ValueofType_struct*_tmp11BE;t=(void*)((_tmp11BE=
_cycalloc(sizeof(*_tmp11BE)),((_tmp11BE[0]=((_tmp11BF.tag=18,((_tmp11BF.f1=Cyc_Absyn_uint_exp(
0,0),_tmp11BF)))),_tmp11BE))));}goto _LL1B0;_LL1B9:;_LL1BA: t=Cyc_Absyn_sint_typ;
goto _LL1B0;_LL1B0:;}{struct _tuple16*_tmp11C0;return(_tmp11C0=_cycalloc(sizeof(*
_tmp11C0)),((_tmp11C0->f1=tv,((_tmp11C0->f2=t,_tmp11C0)))));};}static void*Cyc_Tcutil_rgns_of(
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
_tmp11C3;struct Cyc_Absyn_AccessEff_struct*_tmp11C2;return(void*)((_tmp11C2=
_cycalloc(sizeof(*_tmp11C2)),((_tmp11C2[0]=((_tmp11C3.tag=23,((_tmp11C3.f1=(void*)
t,_tmp11C3)))),_tmp11C2))));}_LL1F5: _tmp397=*_tmp394;_tmp398=_tmp397.kind;if(
_tmp398 != Cyc_Absyn_EffKind)goto _LL1F7;_LL1F6: return t;_LL1F7: _tmp399=*_tmp394;
_tmp39A=_tmp399.kind;if(_tmp39A != Cyc_Absyn_IntKind)goto _LL1F9;_LL1F8: return Cyc_Absyn_empty_effect;
_LL1F9:;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct _tmp11C6;struct Cyc_Absyn_RgnsEff_struct*
_tmp11C5;return(void*)((_tmp11C5=_cycalloc(sizeof(*_tmp11C5)),((_tmp11C5[0]=((
_tmp11C6.tag=25,((_tmp11C6.f1=(void*)t,_tmp11C6)))),_tmp11C5))));}_LL1F2:;}
_LL1CC: {struct Cyc_Absyn_RgnHandleType_struct*_tmp366=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp35D;if(_tmp366->tag != 15)goto _LL1CE;else{_tmp367=(void*)_tmp366->f1;}}_LL1CD: {
struct Cyc_Absyn_AccessEff_struct _tmp11C9;struct Cyc_Absyn_AccessEff_struct*
_tmp11C8;return(void*)((_tmp11C8=_cycalloc(sizeof(*_tmp11C8)),((_tmp11C8[0]=((
_tmp11C9.tag=23,((_tmp11C9.f1=(void*)_tmp367,_tmp11C9)))),_tmp11C8))));}_LL1CE: {
struct Cyc_Absyn_DynRgnType_struct*_tmp368=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp35D;if(_tmp368->tag != 16)goto _LL1D0;else{_tmp369=(void*)_tmp368->f1;_tmp36A=(
void*)_tmp368->f2;}}_LL1CF: {struct Cyc_Absyn_AccessEff_struct _tmp11CC;struct Cyc_Absyn_AccessEff_struct*
_tmp11CB;return(void*)((_tmp11CB=_cycalloc(sizeof(*_tmp11CB)),((_tmp11CB[0]=((
_tmp11CC.tag=23,((_tmp11CC.f1=(void*)_tmp36A,_tmp11CC)))),_tmp11CB))));}_LL1D0: {
struct Cyc_Absyn_DatatypeType_struct*_tmp36B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp35D;if(_tmp36B->tag != 3)goto _LL1D2;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36C.targs;}}
_LL1D1: {struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp36D);struct Cyc_Absyn_JoinEff_struct
_tmp11CF;struct Cyc_Absyn_JoinEff_struct*_tmp11CE;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11CE=_cycalloc(sizeof(*_tmp11CE)),((_tmp11CE[0]=((_tmp11CF.tag=24,((
_tmp11CF.f1=ts,_tmp11CF)))),_tmp11CE)))));}_LL1D2: {struct Cyc_Absyn_PointerType_struct*
_tmp36E=(struct Cyc_Absyn_PointerType_struct*)_tmp35D;if(_tmp36E->tag != 5)goto
_LL1D4;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.elt_typ;_tmp371=_tmp36F.ptr_atts;
_tmp372=_tmp371.rgn;}}_LL1D3: {struct Cyc_Absyn_JoinEff_struct _tmp11DE;struct Cyc_Absyn_AccessEff_struct*
_tmp11DD;struct Cyc_Absyn_AccessEff_struct _tmp11DC;void*_tmp11DB[2];struct Cyc_Absyn_JoinEff_struct*
_tmp11DA;return Cyc_Tcutil_normalize_effect((void*)((_tmp11DA=_cycalloc(sizeof(*
_tmp11DA)),((_tmp11DA[0]=((_tmp11DE.tag=24,((_tmp11DE.f1=((_tmp11DB[1]=Cyc_Tcutil_rgns_of(
_tmp370),((_tmp11DB[0]=(void*)((_tmp11DD=_cycalloc(sizeof(*_tmp11DD)),((_tmp11DD[
0]=((_tmp11DC.tag=23,((_tmp11DC.f1=(void*)_tmp372,_tmp11DC)))),_tmp11DD)))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp11DB,sizeof(void*),2)))))),_tmp11DE)))),_tmp11DA)))));}_LL1D4: {struct Cyc_Absyn_ArrayType_struct*
_tmp373=(struct Cyc_Absyn_ArrayType_struct*)_tmp35D;if(_tmp373->tag != 8)goto
_LL1D6;else{_tmp374=_tmp373->f1;_tmp375=_tmp374.elt_type;}}_LL1D5: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp375));_LL1D6: {struct Cyc_Absyn_TupleType_struct*_tmp376=(
struct Cyc_Absyn_TupleType_struct*)_tmp35D;if(_tmp376->tag != 10)goto _LL1D8;else{
_tmp377=_tmp376->f1;}}_LL1D7: {struct Cyc_List_List*_tmp3AA=0;for(0;_tmp377 != 0;
_tmp377=_tmp377->tl){struct Cyc_List_List*_tmp11DF;_tmp3AA=((_tmp11DF=_cycalloc(
sizeof(*_tmp11DF)),((_tmp11DF->hd=(*((struct _tuple12*)_tmp377->hd)).f2,((
_tmp11DF->tl=_tmp3AA,_tmp11DF))))));}_tmp37A=_tmp3AA;goto _LL1D9;}_LL1D8: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp378=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp35D;if(_tmp378->tag != 4)goto _LL1DA;else{_tmp379=_tmp378->f1;_tmp37A=_tmp379.targs;}}
_LL1D9: _tmp37D=_tmp37A;goto _LL1DB;_LL1DA: {struct Cyc_Absyn_AggrType_struct*
_tmp37B=(struct Cyc_Absyn_AggrType_struct*)_tmp35D;if(_tmp37B->tag != 11)goto
_LL1DC;else{_tmp37C=_tmp37B->f1;_tmp37D=_tmp37C.targs;}}_LL1DB: {struct Cyc_Absyn_JoinEff_struct
_tmp11E2;struct Cyc_Absyn_JoinEff_struct*_tmp11E1;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11E1=_cycalloc(sizeof(*_tmp11E1)),((_tmp11E1[0]=((_tmp11E2.tag=24,((
_tmp11E2.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp37D),_tmp11E2)))),_tmp11E1)))));}_LL1DC: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp37E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp35D;if(_tmp37E->tag != 12)goto _LL1DE;else{_tmp37F=_tmp37E->f2;}}_LL1DD: {
struct Cyc_Absyn_JoinEff_struct _tmp11E5;struct Cyc_Absyn_JoinEff_struct*_tmp11E4;
return Cyc_Tcutil_normalize_effect((void*)((_tmp11E4=_cycalloc(sizeof(*_tmp11E4)),((
_tmp11E4[0]=((_tmp11E5.tag=24,((_tmp11E5.f1=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp37F),_tmp11E5)))),_tmp11E4)))));}_LL1DE: {struct Cyc_Absyn_TagType_struct*
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
_tmp11E8;struct Cyc_Absyn_JoinEff_struct*_tmp11E7;return Cyc_Tcutil_normalize_effect((
void*)((_tmp11E7=_cycalloc(sizeof(*_tmp11E7)),((_tmp11E7[0]=((_tmp11E8.tag=24,((
_tmp11E8.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp392),_tmp11E8)))),_tmp11E7)))));}_LL1F0: {
struct Cyc_Absyn_TypeDeclType_struct*_tmp393=(struct Cyc_Absyn_TypeDeclType_struct*)
_tmp35D;if(_tmp393->tag != 26)goto _LL1BB;}_LL1F1: {const char*_tmp11EB;void*
_tmp11EA;(_tmp11EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp11EB="typedecl in rgns_of",_tag_dyneither(_tmp11EB,
sizeof(char),20))),_tag_dyneither(_tmp11EA,sizeof(void*),0)));}_LL1BB:;}void*Cyc_Tcutil_normalize_effect(
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
_LL215: goto _LL20D;_LL216:;_LL217:{struct Cyc_List_List*_tmp11EC;effects=((
_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((_tmp11EC->hd=_tmp3C7,((_tmp11EC->tl=
effects,_tmp11EC))))));}goto _LL20D;_LL20D:;}}*_tmp3B8=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp3B9=(struct Cyc_Absyn_RgnsEff_struct*)_tmp3B5;if(_tmp3B9->tag != 25)goto _LL200;
else{_tmp3BA=(void*)_tmp3B9->f1;}}_LL1FF: {void*_tmp3D4=Cyc_Tcutil_compress(
_tmp3BA);_LL219: {struct Cyc_Absyn_Evar_struct*_tmp3D5=(struct Cyc_Absyn_Evar_struct*)
_tmp3D4;if(_tmp3D5->tag != 1)goto _LL21B;}_LL21A: goto _LL21C;_LL21B: {struct Cyc_Absyn_VarType_struct*
_tmp3D6=(struct Cyc_Absyn_VarType_struct*)_tmp3D4;if(_tmp3D6->tag != 2)goto _LL21D;}
_LL21C: return e;_LL21D:;_LL21E: return Cyc_Tcutil_rgns_of(_tmp3BA);_LL218:;}_LL200:;
_LL201: return e;_LL1FB:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmp11F6;struct Cyc_Core_Opt*_tmp11F5;struct Cyc_Absyn_FnInfo _tmp11F4;struct Cyc_Absyn_FnType_struct*
_tmp11F3;struct Cyc_Absyn_FnType_struct*_tmp3D7=(_tmp11F3=_cycalloc(sizeof(*
_tmp11F3)),((_tmp11F3[0]=((_tmp11F6.tag=9,((_tmp11F6.f1=((_tmp11F4.tvars=0,((
_tmp11F4.effect=((_tmp11F5=_cycalloc(sizeof(*_tmp11F5)),((_tmp11F5->v=eff,
_tmp11F5)))),((_tmp11F4.ret_tqual=Cyc_Absyn_empty_tqual(0),((_tmp11F4.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmp11F4.args=0,((_tmp11F4.c_varargs=0,((
_tmp11F4.cyc_varargs=0,((_tmp11F4.rgn_po=0,((_tmp11F4.attributes=0,_tmp11F4)))))))))))))))))),
_tmp11F6)))),_tmp11F3)));return Cyc_Absyn_atb_typ((void*)_tmp3D7,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if((r == (void*)& Cyc_Absyn_HeapRgn_val
 || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{void*_tmp3DC=Cyc_Tcutil_compress(e);void*_tmp3DE;struct Cyc_List_List*
_tmp3E0;void*_tmp3E2;struct Cyc_Core_Opt*_tmp3E4;struct Cyc_Core_Opt*_tmp3E5;
struct Cyc_Core_Opt**_tmp3E6;struct Cyc_Core_Opt*_tmp3E7;_LL220: {struct Cyc_Absyn_AccessEff_struct*
_tmp3DD=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DC;if(_tmp3DD->tag != 23)goto
_LL222;else{_tmp3DE=(void*)_tmp3DD->f1;}}_LL221: if(constrain)return Cyc_Tcutil_unify(
r,_tmp3DE);_tmp3DE=Cyc_Tcutil_compress(_tmp3DE);if(r == _tmp3DE)return 1;{struct
_tuple0 _tmp11F7;struct _tuple0 _tmp3E9=(_tmp11F7.f1=r,((_tmp11F7.f2=_tmp3DE,
_tmp11F7)));void*_tmp3EA;struct Cyc_Absyn_Tvar*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_Tvar*
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
_check_null(_tmp3F8))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmp1206;struct Cyc_Absyn_AccessEff_struct*
_tmp1205;struct Cyc_Absyn_AccessEff_struct _tmp1204;void*_tmp1203[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1202;void*_tmp3FA=Cyc_Tcutil_dummy_fntype((void*)((_tmp1202=_cycalloc(
sizeof(*_tmp1202)),((_tmp1202[0]=((_tmp1206.tag=24,((_tmp1206.f1=((_tmp1203[1]=(
void*)((_tmp1205=_cycalloc(sizeof(*_tmp1205)),((_tmp1205[0]=((_tmp1204.tag=23,((
_tmp1204.f1=(void*)r,_tmp1204)))),_tmp1205)))),((_tmp1203[0]=_tmp3F9,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1203,sizeof(void*),2)))))),
_tmp1206)))),_tmp1202)))));{struct Cyc_Core_Opt*_tmp1207;*_tmp3F7=((_tmp1207=
_cycalloc(sizeof(*_tmp1207)),((_tmp1207->v=_tmp3FA,_tmp1207))));}return 1;};}
_LL237:;_LL238: return 0;_LL234:;};_LL232:;_LL233: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3F0);_LL22F:;}_LL226: {struct Cyc_Absyn_Evar_struct*_tmp3E3=(
struct Cyc_Absyn_Evar_struct*)_tmp3DC;if(_tmp3E3->tag != 1)goto _LL228;else{_tmp3E4=
_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;_tmp3E6=(struct Cyc_Core_Opt**)& _tmp3E3->f2;
_tmp3E7=_tmp3E3->f4;}}_LL227: if(_tmp3E4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3E4->v)->kind
!= Cyc_Absyn_EffKind){const char*_tmp120A;void*_tmp1209;(_tmp1209=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp120A="effect evar has wrong kind",
_tag_dyneither(_tmp120A,sizeof(char),27))),_tag_dyneither(_tmp1209,sizeof(void*),
0)));}if(!constrain)return 0;{void*_tmp403=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_eko,_tmp3E7);Cyc_Tcutil_occurs(_tmp403,Cyc_Core_heap_region,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E7))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmp121F;struct Cyc_List_List*_tmp121E;struct Cyc_Absyn_AccessEff_struct _tmp121D;
struct Cyc_Absyn_AccessEff_struct*_tmp121C;struct Cyc_List_List*_tmp121B;struct Cyc_Absyn_JoinEff_struct*
_tmp121A;struct Cyc_Absyn_JoinEff_struct*_tmp404=(_tmp121A=_cycalloc(sizeof(*
_tmp121A)),((_tmp121A[0]=((_tmp121F.tag=24,((_tmp121F.f1=((_tmp121B=_cycalloc(
sizeof(*_tmp121B)),((_tmp121B->hd=_tmp403,((_tmp121B->tl=((_tmp121E=_cycalloc(
sizeof(*_tmp121E)),((_tmp121E->hd=(void*)((_tmp121C=_cycalloc(sizeof(*_tmp121C)),((
_tmp121C[0]=((_tmp121D.tag=23,((_tmp121D.f1=(void*)r,_tmp121D)))),_tmp121C)))),((
_tmp121E->tl=0,_tmp121E)))))),_tmp121B)))))),_tmp121F)))),_tmp121A)));{struct Cyc_Core_Opt*
_tmp1220;*_tmp3E6=((_tmp1220=_cycalloc(sizeof(*_tmp1220)),((_tmp1220->v=(void*)
_tmp404,_tmp1220))));}return 1;};};_LL228:;_LL229: return 0;_LL21F:;};}static int Cyc_Tcutil_type_in_effect(
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
else{_tmp419=(void*)_tmp418->f1;}}_LL246: {struct _tuple0 _tmp1221;struct _tuple0
_tmp41B=(_tmp1221.f1=Cyc_Tcutil_compress(_tmp419),((_tmp1221.f2=_tmp411,_tmp1221)));
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
_tmp413->v)->kind != Cyc_Absyn_EffKind){const char*_tmp1224;void*_tmp1223;(
_tmp1223=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1224="effect evar has wrong kind",_tag_dyneither(_tmp1224,sizeof(char),27))),
_tag_dyneither(_tmp1223,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp424=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp416);
Cyc_Tcutil_occurs(_tmp424,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp416))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmp1239;struct Cyc_List_List*
_tmp1238;struct Cyc_Absyn_RgnsEff_struct _tmp1237;struct Cyc_Absyn_RgnsEff_struct*
_tmp1236;struct Cyc_List_List*_tmp1235;struct Cyc_Absyn_JoinEff_struct*_tmp1234;
struct Cyc_Absyn_JoinEff_struct*_tmp425=(_tmp1234=_cycalloc(sizeof(*_tmp1234)),((
_tmp1234[0]=((_tmp1239.tag=24,((_tmp1239.f1=((_tmp1235=_cycalloc(sizeof(*
_tmp1235)),((_tmp1235->hd=_tmp424,((_tmp1235->tl=((_tmp1238=_cycalloc(sizeof(*
_tmp1238)),((_tmp1238->hd=(void*)((_tmp1236=_cycalloc(sizeof(*_tmp1236)),((
_tmp1236[0]=((_tmp1237.tag=25,((_tmp1237.f1=(void*)t,_tmp1237)))),_tmp1236)))),((
_tmp1238->tl=0,_tmp1238)))))),_tmp1235)))))),_tmp1239)))),_tmp1234)));{struct Cyc_Core_Opt*
_tmp123A;*_tmp415=((_tmp123A=_cycalloc(sizeof(*_tmp123A)),((_tmp123A->v=(void*)
_tmp425,_tmp123A))));}return 1;};};_LL242:;_LL243: return 0;_LL239:;};}static int Cyc_Tcutil_variable_in_effect(
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
struct Cyc_Absyn_JoinEff_struct _tmp1249;struct Cyc_Absyn_VarType_struct*_tmp1248;
struct Cyc_Absyn_VarType_struct _tmp1247;void*_tmp1246[2];struct Cyc_Absyn_JoinEff_struct*
_tmp1245;void*_tmp443=Cyc_Tcutil_dummy_fntype((void*)((_tmp1245=_cycalloc(
sizeof(*_tmp1245)),((_tmp1245[0]=((_tmp1249.tag=24,((_tmp1249.f1=((_tmp1246[1]=(
void*)((_tmp1248=_cycalloc(sizeof(*_tmp1248)),((_tmp1248[0]=((_tmp1247.tag=2,((
_tmp1247.f1=v,_tmp1247)))),_tmp1248)))),((_tmp1246[0]=_tmp442,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1246,sizeof(void*),2)))))),
_tmp1249)))),_tmp1245)))));{struct Cyc_Core_Opt*_tmp124A;*_tmp440=((_tmp124A=
_cycalloc(sizeof(*_tmp124A)),((_tmp124A->v=_tmp443,_tmp124A))));}return 1;};}
_LL261:;_LL262: return 0;_LL25E:;};_LL25C:;_LL25D: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp439);_LL259:;}_LL255: {struct Cyc_Absyn_Evar_struct*
_tmp434=(struct Cyc_Absyn_Evar_struct*)_tmp42D;if(_tmp434->tag != 1)goto _LL257;
else{_tmp435=_tmp434->f1;_tmp436=_tmp434->f2;_tmp437=(struct Cyc_Core_Opt**)&
_tmp434->f2;_tmp438=_tmp434->f4;}}_LL256: if(_tmp435 == 0  || ((struct Cyc_Absyn_Kind*)
_tmp435->v)->kind != Cyc_Absyn_EffKind){const char*_tmp124D;void*_tmp124C;(
_tmp124C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp124D="effect evar has wrong kind",_tag_dyneither(_tmp124D,sizeof(char),27))),
_tag_dyneither(_tmp124C,sizeof(void*),0)));}{void*_tmp44C=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko,_tmp438);if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp438))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmp1262;
struct Cyc_List_List*_tmp1261;struct Cyc_Absyn_VarType_struct _tmp1260;struct Cyc_Absyn_VarType_struct*
_tmp125F;struct Cyc_List_List*_tmp125E;struct Cyc_Absyn_JoinEff_struct*_tmp125D;
struct Cyc_Absyn_JoinEff_struct*_tmp44D=(_tmp125D=_cycalloc(sizeof(*_tmp125D)),((
_tmp125D[0]=((_tmp1262.tag=24,((_tmp1262.f1=((_tmp125E=_cycalloc(sizeof(*
_tmp125E)),((_tmp125E->hd=_tmp44C,((_tmp125E->tl=((_tmp1261=_cycalloc(sizeof(*
_tmp1261)),((_tmp1261->hd=(void*)((_tmp125F=_cycalloc(sizeof(*_tmp125F)),((
_tmp125F[0]=((_tmp1260.tag=2,((_tmp1260.f1=v,_tmp1260)))),_tmp125F)))),((
_tmp1261->tl=0,_tmp1261)))))),_tmp125E)))))),_tmp1262)))),_tmp125D)));{struct Cyc_Core_Opt*
_tmp1263;*_tmp437=((_tmp1263=_cycalloc(sizeof(*_tmp1263)),((_tmp1263->v=(void*)
_tmp44D,_tmp1263))));}return 1;};};_LL257:;_LL258: return 0;_LL24E:;};}static int Cyc_Tcutil_evar_in_effect(
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
e1,e2)){struct Cyc_Core_Opt*_tmp1264;*_tmp469=((_tmp1264=_cycalloc(sizeof(*
_tmp1264)),((_tmp1264->v=Cyc_Absyn_empty_effect,_tmp1264))));}return 1;_LL27C:;
_LL27D: {const char*_tmp1268;void*_tmp1267[1];struct Cyc_String_pa_struct _tmp1266;(
_tmp1266.tag=0,((_tmp1266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmp1267[0]=& _tmp1266,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1268="subset_effect: bad effect: %s",
_tag_dyneither(_tmp1268,sizeof(char),30))),_tag_dyneither(_tmp1267,sizeof(void*),
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
Cyc_Core_Opt*_tmp1269;*_tmp47E=((_tmp1269=_cycalloc(sizeof(*_tmp1269)),((
_tmp1269->v=t2,_tmp1269))));}return;}else{{void*_tmp482=t2;struct Cyc_Core_Opt*
_tmp484;struct Cyc_Core_Opt**_tmp485;struct Cyc_Core_Opt*_tmp486;struct Cyc_Absyn_PtrInfo
_tmp488;_LL289: {struct Cyc_Absyn_Evar_struct*_tmp483=(struct Cyc_Absyn_Evar_struct*)
_tmp482;if(_tmp483->tag != 1)goto _LL28B;else{_tmp484=_tmp483->f2;_tmp485=(struct
Cyc_Core_Opt**)& _tmp483->f2;_tmp486=_tmp483->f4;}}_LL28A: {struct Cyc_List_List*
_tmp489=(struct Cyc_List_List*)_tmp47F->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp486))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp489,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmp126A;Cyc_Tcutil_failure_reason=((
_tmp126A="(type variable would escape scope)",_tag_dyneither(_tmp126A,sizeof(
char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}if(Cyc_Tcutil_kind_leq((
struct Cyc_Absyn_Kind*)_tmp47C->v,_tmp480)){{struct Cyc_Core_Opt*_tmp126B;*_tmp485=((
_tmp126B=_cycalloc(sizeof(*_tmp126B)),((_tmp126B->v=t1,_tmp126B))));}return;}{
const char*_tmp126C;Cyc_Tcutil_failure_reason=((_tmp126C="(kinds are incompatible)",
_tag_dyneither(_tmp126C,sizeof(char),25)));}goto _LL288;}_LL28B:{struct Cyc_Absyn_PointerType_struct*
_tmp487=(struct Cyc_Absyn_PointerType_struct*)_tmp482;if(_tmp487->tag != 5)goto
_LL28D;else{_tmp488=_tmp487->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp47C->v)->kind
== Cyc_Absyn_BoxKind))goto _LL28D;_LL28C: {union Cyc_Absyn_Constraint*_tmp48D=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((
_tmp488.ptr_atts).bounds);{union Cyc_Absyn_Constraint*_tmp48E=_tmp48D;union Cyc_Absyn_Constraint
_tmp48F;int _tmp490;_LL290: _tmp48F=*_tmp48E;if((_tmp48F.No_constr).tag != 3)goto
_LL292;_tmp490=(int)(_tmp48F.No_constr).val;_LL291:{struct
_union_Constraint_Eq_constr*_tmp126D;(_tmp126D=& _tmp48D->Eq_constr,((_tmp126D->tag=
1,_tmp126D->val=Cyc_Absyn_bounds_one)));}{struct Cyc_Core_Opt*_tmp126E;*_tmp47E=((
_tmp126E=_cycalloc(sizeof(*_tmp126E)),((_tmp126E->v=t2,_tmp126E))));}return;
_LL292:;_LL293: goto _LL28F;_LL28F:;}goto _LL288;}_LL28D:;_LL28E: goto _LL288;_LL288:;}{
const char*_tmp126F;Cyc_Tcutil_failure_reason=((_tmp126F="(kinds are incompatible)",
_tag_dyneither(_tmp126F,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};
_LL286:;_LL287: goto _LL283;_LL283:;}{struct _tuple0 _tmp1270;struct _tuple0 _tmp495=(
_tmp1270.f1=t2,((_tmp1270.f2=t1,_tmp1270)));void*_tmp496;void*_tmp498;void*
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
return;{const char*_tmp1271;Cyc_Tcutil_failure_reason=((_tmp1271="(variable types are not the same)",
_tag_dyneither(_tmp1271,sizeof(char),34)));}goto _LL294;}_LL29B: _tmp4A2=_tmp495.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4A3=(struct Cyc_Absyn_AggrType_struct*)
_tmp4A2;if(_tmp4A3->tag != 11)goto _LL29D;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A4.aggr_info;
_tmp4A6=_tmp4A4.targs;}};_tmp4A7=_tmp495.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4A8=(struct Cyc_Absyn_AggrType_struct*)_tmp4A7;if(_tmp4A8->tag != 11)goto
_LL29D;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A9.aggr_info;_tmp4AB=_tmp4A9.targs;}};
_LL29C: {enum Cyc_Absyn_AggrKind _tmp56C;struct _tuple2*_tmp56D;struct _tuple13
_tmp56B=Cyc_Absyn_aggr_kinded_name(_tmp4AA);_tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;{
enum Cyc_Absyn_AggrKind _tmp56F;struct _tuple2*_tmp570;struct _tuple13 _tmp56E=Cyc_Absyn_aggr_kinded_name(
_tmp4A5);_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;if(_tmp56C != _tmp56F){{const char*
_tmp1272;Cyc_Tcutil_failure_reason=((_tmp1272="(struct and union type)",
_tag_dyneither(_tmp1272,sizeof(char),24)));}goto _LL294;}if(Cyc_Absyn_qvar_cmp(
_tmp56D,_tmp570)!= 0){{const char*_tmp1273;Cyc_Tcutil_failure_reason=((_tmp1273="(different type name)",
_tag_dyneither(_tmp1273,sizeof(char),22)));}goto _LL294;}Cyc_Tcutil_unify_list(
_tmp4AB,_tmp4A6);return;};}_LL29D: _tmp4AC=_tmp495.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp4AD=(struct Cyc_Absyn_EnumType_struct*)_tmp4AC;if(_tmp4AD->tag != 13)goto
_LL29F;else{_tmp4AE=_tmp4AD->f1;}};_tmp4AF=_tmp495.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp4B0=(struct Cyc_Absyn_EnumType_struct*)_tmp4AF;if(_tmp4B0->tag != 13)goto
_LL29F;else{_tmp4B1=_tmp4B0->f1;}};_LL29E: if(Cyc_Absyn_qvar_cmp(_tmp4AE,_tmp4B1)
== 0)return;{const char*_tmp1274;Cyc_Tcutil_failure_reason=((_tmp1274="(different enum types)",
_tag_dyneither(_tmp1274,sizeof(char),23)));}goto _LL294;_LL29F: _tmp4B2=_tmp495.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp4B3=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B2;if(_tmp4B3->tag != 14)goto _LL2A1;else{_tmp4B4=_tmp4B3->f1;}};_tmp4B5=
_tmp495.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp4B6=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4B5;if(_tmp4B6->tag != 14)goto _LL2A1;else{_tmp4B7=_tmp4B6->f1;}};_LL2A0: {int
bad=0;for(0;_tmp4B4 != 0  && _tmp4B7 != 0;(_tmp4B4=_tmp4B4->tl,_tmp4B7=_tmp4B7->tl)){
struct Cyc_Absyn_Enumfield*_tmp574=(struct Cyc_Absyn_Enumfield*)_tmp4B4->hd;struct
Cyc_Absyn_Enumfield*_tmp575=(struct Cyc_Absyn_Enumfield*)_tmp4B7->hd;if(Cyc_Absyn_qvar_cmp(
_tmp574->name,_tmp575->name)!= 0){{const char*_tmp1275;Cyc_Tcutil_failure_reason=((
_tmp1275="(different names for enum fields)",_tag_dyneither(_tmp1275,sizeof(char),
34)));}bad=1;break;}if(_tmp574->tag == _tmp575->tag)continue;if(_tmp574->tag == 0
 || _tmp575->tag == 0){{const char*_tmp1276;Cyc_Tcutil_failure_reason=((_tmp1276="(different tag values for enum fields)",
_tag_dyneither(_tmp1276,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp574->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp575->tag))){{const char*_tmp1277;Cyc_Tcutil_failure_reason=((_tmp1277="(different tag values for enum fields)",
_tag_dyneither(_tmp1277,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL294;if(
_tmp4B4 == 0  && _tmp4B7 == 0)return;{const char*_tmp1278;Cyc_Tcutil_failure_reason=((
_tmp1278="(different number of fields for enums)",_tag_dyneither(_tmp1278,
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
_tmp1279;Cyc_Tcutil_failure_reason=((_tmp1279="(different datatype field types)",
_tag_dyneither(_tmp1279,sizeof(char),33)));}goto _LL294;_LL2A5: _tmp4D6=_tmp495.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp4D7=(struct Cyc_Absyn_PointerType_struct*)
_tmp4D6;if(_tmp4D7->tag != 5)goto _LL2A7;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D8.elt_typ;
_tmp4DA=_tmp4D8.elt_tq;_tmp4DB=_tmp4D8.ptr_atts;_tmp4DC=_tmp4DB.rgn;_tmp4DD=
_tmp4DB.nullable;_tmp4DE=_tmp4DB.bounds;_tmp4DF=_tmp4DB.zero_term;}};_tmp4E0=
_tmp495.f2;{struct Cyc_Absyn_PointerType_struct*_tmp4E1=(struct Cyc_Absyn_PointerType_struct*)
_tmp4E0;if(_tmp4E1->tag != 5)goto _LL2A7;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E2.elt_typ;
_tmp4E4=_tmp4E2.elt_tq;_tmp4E5=_tmp4E2.ptr_atts;_tmp4E6=_tmp4E5.rgn;_tmp4E7=
_tmp4E5.nullable;_tmp4E8=_tmp4E5.bounds;_tmp4E9=_tmp4E5.zero_term;}};_LL2A6: Cyc_Tcutil_unify_it(
_tmp4E3,_tmp4D9);Cyc_Tcutil_unify_it(_tmp4DC,_tmp4E6);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmp127A;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp4E9,_tmp4DF,((_tmp127A="(not both zero terminated)",
_tag_dyneither(_tmp127A,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp4E4,
_tmp4E3,_tmp4DA,_tmp4D9);{const char*_tmp127B;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp4E8,_tmp4DE,((
_tmp127B="(different pointer bounds)",_tag_dyneither(_tmp127B,sizeof(char),27))));}{
void*_tmp57D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp4E8);_LL2D2: {struct Cyc_Absyn_DynEither_b_struct*
_tmp57E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp57D;if(_tmp57E->tag != 0)goto
_LL2D4;}_LL2D3: return;_LL2D4:;_LL2D5: goto _LL2D1;_LL2D1:;}{const char*_tmp127C;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp4E7,_tmp4DD,((_tmp127C="(incompatible pointer types)",_tag_dyneither(
_tmp127C,sizeof(char),29))));}return;_LL2A7: _tmp4EA=_tmp495.f1;{struct Cyc_Absyn_IntType_struct*
_tmp4EB=(struct Cyc_Absyn_IntType_struct*)_tmp4EA;if(_tmp4EB->tag != 6)goto _LL2A9;
else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}};_tmp4EE=_tmp495.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4EF=(struct Cyc_Absyn_IntType_struct*)_tmp4EE;if(_tmp4EF->tag != 6)goto _LL2A9;
else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}};_LL2A8: if(_tmp4F0 == _tmp4EC  && ((
_tmp4F1 == _tmp4ED  || _tmp4F1 == Cyc_Absyn_Int_sz  && _tmp4ED == Cyc_Absyn_Long_sz)
 || _tmp4F1 == Cyc_Absyn_Long_sz  && _tmp4ED == Cyc_Absyn_Int_sz))return;{const char*
_tmp127D;Cyc_Tcutil_failure_reason=((_tmp127D="(different integral types)",
_tag_dyneither(_tmp127D,sizeof(char),27)));}goto _LL294;_LL2A9: _tmp4F2=_tmp495.f1;{
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
_tmp503)){{const char*_tmp127E;Cyc_Tcutil_failure_reason=((_tmp127E="(cannot prove expressions are the same)",
_tag_dyneither(_tmp127E,sizeof(char),40)));}goto _LL294;}return;_LL2AF: _tmp504=
_tmp495.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp505=(struct Cyc_Absyn_ArrayType_struct*)
_tmp504;if(_tmp505->tag != 8)goto _LL2B1;else{_tmp506=_tmp505->f1;_tmp507=_tmp506.elt_type;
_tmp508=_tmp506.tq;_tmp509=_tmp506.num_elts;_tmp50A=_tmp506.zero_term;}};_tmp50B=
_tmp495.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp50C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp50B;if(_tmp50C->tag != 8)goto _LL2B1;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50D.elt_type;
_tmp50F=_tmp50D.tq;_tmp510=_tmp50D.num_elts;_tmp511=_tmp50D.zero_term;}};_LL2B0:
Cyc_Tcutil_unify_it(_tmp50E,_tmp507);Cyc_Tcutil_unify_tqual(_tmp50F,_tmp50E,
_tmp508,_tmp507);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmp127F;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp50A,_tmp511,((_tmp127F="(not both zero terminated)",_tag_dyneither(_tmp127F,
sizeof(char),27))));}if(_tmp509 == _tmp510)return;if(_tmp509 == 0  || _tmp510 == 0)
goto _LL294;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp509,(struct Cyc_Absyn_Exp*)
_tmp510))return;{const char*_tmp1280;Cyc_Tcutil_failure_reason=((_tmp1280="(different array sizes)",
_tag_dyneither(_tmp1280,sizeof(char),24)));}goto _LL294;_LL2B1: _tmp512=_tmp495.f1;{
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
_tmp521 != 0){if(_tmp515 == 0){{const char*_tmp1281;Cyc_Tcutil_failure_reason=((
_tmp1281="(second function type has too few type variables)",_tag_dyneither(
_tmp1281,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{void*
_tmp586=((struct Cyc_Absyn_Tvar*)_tmp521->hd)->kind;void*_tmp587=((struct Cyc_Absyn_Tvar*)
_tmp515->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp586,_tmp587)){{const char*
_tmp1287;void*_tmp1286[3];struct Cyc_String_pa_struct _tmp1285;struct Cyc_String_pa_struct
_tmp1284;struct Cyc_String_pa_struct _tmp1283;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1283.tag=0,((_tmp1283.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp515->hd,& Cyc_Tcutil_bk))),((_tmp1284.tag=0,((_tmp1284.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp521->hd,& Cyc_Tcutil_bk))),((_tmp1285.tag=0,((_tmp1285.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp521->hd)),((_tmp1286[0]=& _tmp1285,((_tmp1286[1]=& _tmp1284,((_tmp1286[2]=&
_tmp1283,Cyc_aprintf(((_tmp1287="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1287,sizeof(char),44))),_tag_dyneither(_tmp1286,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple16*_tmp1291;struct Cyc_Absyn_VarType_struct _tmp1290;struct Cyc_Absyn_VarType_struct*
_tmp128F;struct Cyc_List_List*_tmp128E;inst=((_tmp128E=_region_malloc(rgn,sizeof(*
_tmp128E)),((_tmp128E->hd=((_tmp1291=_region_malloc(rgn,sizeof(*_tmp1291)),((
_tmp1291->f1=(struct Cyc_Absyn_Tvar*)_tmp515->hd,((_tmp1291->f2=(void*)((_tmp128F=
_cycalloc(sizeof(*_tmp128F)),((_tmp128F[0]=((_tmp1290.tag=2,((_tmp1290.f1=(
struct Cyc_Absyn_Tvar*)_tmp521->hd,_tmp1290)))),_tmp128F)))),_tmp1291)))))),((
_tmp128E->tl=inst,_tmp128E))))));}_tmp521=_tmp521->tl;_tmp515=_tmp515->tl;};}if(
_tmp515 != 0){{const char*_tmp1292;Cyc_Tcutil_failure_reason=((_tmp1292="(second function type has too many type variables)",
_tag_dyneither(_tmp1292,sizeof(char),51)));}_npop_handler(0);goto _LL294;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp129E;struct Cyc_Absyn_FnInfo _tmp129D;
struct Cyc_Absyn_FnType_struct*_tmp129C;struct Cyc_Absyn_FnType_struct _tmp1298;
struct Cyc_Absyn_FnInfo _tmp1297;struct Cyc_Absyn_FnType_struct*_tmp1296;Cyc_Tcutil_unify_it((
void*)((_tmp1296=_cycalloc(sizeof(*_tmp1296)),((_tmp1296[0]=((_tmp1298.tag=9,((
_tmp1298.f1=((_tmp1297.tvars=0,((_tmp1297.effect=_tmp522,((_tmp1297.ret_tqual=
_tmp523,((_tmp1297.ret_typ=_tmp524,((_tmp1297.args=_tmp525,((_tmp1297.c_varargs=
_tmp526,((_tmp1297.cyc_varargs=_tmp527,((_tmp1297.rgn_po=_tmp528,((_tmp1297.attributes=
_tmp529,_tmp1297)))))))))))))))))),_tmp1298)))),_tmp1296)))),Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp129C=_cycalloc(sizeof(*_tmp129C)),((_tmp129C[0]=((_tmp129E.tag=
9,((_tmp129E.f1=((_tmp129D.tvars=0,((_tmp129D.effect=_tmp516,((_tmp129D.ret_tqual=
_tmp517,((_tmp129D.ret_typ=_tmp518,((_tmp129D.args=_tmp519,((_tmp129D.c_varargs=
_tmp51A,((_tmp129D.cyc_varargs=_tmp51B,((_tmp129D.rgn_po=_tmp51C,((_tmp129D.attributes=
_tmp51D,_tmp129D)))))))))))))))))),_tmp129E)))),_tmp129C))))));}done=1;}}if(done){
_npop_handler(0);return;}Cyc_Tcutil_unify_it(_tmp524,_tmp518);Cyc_Tcutil_unify_tqual(
_tmp523,_tmp524,_tmp517,_tmp518);for(0;_tmp525 != 0  && _tmp519 != 0;(_tmp525=
_tmp525->tl,_tmp519=_tmp519->tl)){struct Cyc_Absyn_Tqual _tmp599;void*_tmp59A;
struct _tuple10 _tmp598=*((struct _tuple10*)_tmp525->hd);_tmp599=_tmp598.f2;_tmp59A=
_tmp598.f3;{struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59D;struct _tuple10 _tmp59B=*((
struct _tuple10*)_tmp519->hd);_tmp59C=_tmp59B.f2;_tmp59D=_tmp59B.f3;Cyc_Tcutil_unify_it(
_tmp59A,_tmp59D);Cyc_Tcutil_unify_tqual(_tmp599,_tmp59A,_tmp59C,_tmp59D);};}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp525 != 0  || _tmp519 != 0){{const char*_tmp129F;
Cyc_Tcutil_failure_reason=((_tmp129F="(function types have different number of arguments)",
_tag_dyneither(_tmp129F,sizeof(char),52)));}_npop_handler(0);goto _LL294;}if(
_tmp526 != _tmp51A){{const char*_tmp12A0;Cyc_Tcutil_failure_reason=((_tmp12A0="(only one function type takes C varargs)",
_tag_dyneither(_tmp12A0,sizeof(char),41)));}_npop_handler(0);goto _LL294;}{int
bad_cyc_vararg=0;{struct _tuple22 _tmp12A1;struct _tuple22 _tmp5A1=(_tmp12A1.f1=
_tmp527,((_tmp12A1.f2=_tmp51B,_tmp12A1)));struct Cyc_Absyn_VarargInfo*_tmp5A2;
struct Cyc_Absyn_VarargInfo*_tmp5A3;struct Cyc_Absyn_VarargInfo*_tmp5A4;struct Cyc_Absyn_VarargInfo*
_tmp5A5;struct Cyc_Absyn_VarargInfo*_tmp5A6;struct Cyc_Absyn_VarargInfo _tmp5A7;
struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A9;void*_tmp5AA;int _tmp5AB;
struct Cyc_Absyn_VarargInfo*_tmp5AC;struct Cyc_Absyn_VarargInfo _tmp5AD;struct Cyc_Core_Opt*
_tmp5AE;struct Cyc_Absyn_Tqual _tmp5AF;void*_tmp5B0;int _tmp5B1;_LL2D7: _tmp5A2=
_tmp5A1.f1;if(_tmp5A2 != 0)goto _LL2D9;_tmp5A3=_tmp5A1.f2;if(_tmp5A3 != 0)goto
_LL2D9;_LL2D8: goto _LL2D6;_LL2D9: _tmp5A4=_tmp5A1.f1;if(_tmp5A4 != 0)goto _LL2DB;
_LL2DA: goto _LL2DC;_LL2DB: _tmp5A5=_tmp5A1.f2;if(_tmp5A5 != 0)goto _LL2DD;_LL2DC:
bad_cyc_vararg=1;{const char*_tmp12A2;Cyc_Tcutil_failure_reason=((_tmp12A2="(only one function type takes varargs)",
_tag_dyneither(_tmp12A2,sizeof(char),39)));}goto _LL2D6;_LL2DD: _tmp5A6=_tmp5A1.f1;
if(_tmp5A6 == 0)goto _LL2D6;_tmp5A7=*_tmp5A6;_tmp5A8=_tmp5A7.name;_tmp5A9=_tmp5A7.tq;
_tmp5AA=_tmp5A7.type;_tmp5AB=_tmp5A7.inject;_tmp5AC=_tmp5A1.f2;if(_tmp5AC == 0)
goto _LL2D6;_tmp5AD=*_tmp5AC;_tmp5AE=_tmp5AD.name;_tmp5AF=_tmp5AD.tq;_tmp5B0=
_tmp5AD.type;_tmp5B1=_tmp5AD.inject;_LL2DE: Cyc_Tcutil_unify_it(_tmp5AA,_tmp5B0);
Cyc_Tcutil_unify_tqual(_tmp5A9,_tmp5AA,_tmp5AF,_tmp5B0);if(_tmp5AB != _tmp5B1){
bad_cyc_vararg=1;{const char*_tmp12A3;Cyc_Tcutil_failure_reason=((_tmp12A3="(only one function type injects varargs)",
_tag_dyneither(_tmp12A3,sizeof(char),41)));};}goto _LL2D6;_LL2D6:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL294;}{int bad_effect=0;{struct _tuple23
_tmp12A4;struct _tuple23 _tmp5B5=(_tmp12A4.f1=_tmp522,((_tmp12A4.f2=_tmp516,
_tmp12A4)));struct Cyc_Core_Opt*_tmp5B6;struct Cyc_Core_Opt*_tmp5B7;struct Cyc_Core_Opt*
_tmp5B8;struct Cyc_Core_Opt*_tmp5B9;_LL2E0: _tmp5B6=_tmp5B5.f1;if(_tmp5B6 != 0)goto
_LL2E2;_tmp5B7=_tmp5B5.f2;if(_tmp5B7 != 0)goto _LL2E2;_LL2E1: goto _LL2DF;_LL2E2:
_tmp5B8=_tmp5B5.f1;if(_tmp5B8 != 0)goto _LL2E4;_LL2E3: goto _LL2E5;_LL2E4: _tmp5B9=
_tmp5B5.f2;if(_tmp5B9 != 0)goto _LL2E6;_LL2E5: bad_effect=1;goto _LL2DF;_LL2E6:;
_LL2E7: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp522))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp516))->v);
goto _LL2DF;_LL2DF:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp12A5;Cyc_Tcutil_failure_reason=((_tmp12A5="(function type effects do not unify)",
_tag_dyneither(_tmp12A5,sizeof(char),37)));}_npop_handler(0);goto _LL294;}if(!Cyc_Tcutil_same_atts(
_tmp51D,_tmp529)){{const char*_tmp12A6;Cyc_Tcutil_failure_reason=((_tmp12A6="(function types have different attributes)",
_tag_dyneither(_tmp12A6,sizeof(char),43)));}_npop_handler(0);goto _LL294;}if(!Cyc_Tcutil_same_rgn_po(
_tmp51C,_tmp528)){{const char*_tmp12A7;Cyc_Tcutil_failure_reason=((_tmp12A7="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp12A7,sizeof(char),58)));}_npop_handler(0);goto _LL294;}
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
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp12A8;Cyc_Tcutil_failure_reason=((
_tmp12A8="(tuple types have different numbers of components)",_tag_dyneither(
_tmp12A8,sizeof(char),51)));}goto _LL294;_LL2B5: _tmp530=_tmp495.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp531=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp530;if(_tmp531->tag != 12)goto
_LL2B7;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;}};_tmp534=_tmp495.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp535=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp534;if(_tmp535->tag != 12)goto _LL2B7;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;}};
_LL2B6: if(_tmp536 != _tmp532){{const char*_tmp12A9;Cyc_Tcutil_failure_reason=((
_tmp12A9="(struct and union type)",_tag_dyneither(_tmp12A9,sizeof(char),24)));}
goto _LL294;}for(0;_tmp537 != 0  && _tmp533 != 0;(_tmp537=_tmp537->tl,_tmp533=
_tmp533->tl)){struct Cyc_Absyn_Aggrfield*_tmp5C5=(struct Cyc_Absyn_Aggrfield*)
_tmp537->hd;struct Cyc_Absyn_Aggrfield*_tmp5C6=(struct Cyc_Absyn_Aggrfield*)
_tmp533->hd;if(Cyc_strptrcmp(_tmp5C5->name,_tmp5C6->name)!= 0){{const char*
_tmp12AA;Cyc_Tcutil_failure_reason=((_tmp12AA="(different member names)",
_tag_dyneither(_tmp12AA,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp5C5->type,_tmp5C6->type);Cyc_Tcutil_unify_tqual(_tmp5C5->tq,
_tmp5C5->type,_tmp5C6->tq,_tmp5C6->type);if(!Cyc_Tcutil_same_atts(_tmp5C5->attributes,
_tmp5C6->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp12AB;Cyc_Tcutil_failure_reason=((_tmp12AB="(different attributes on member)",
_tag_dyneither(_tmp12AB,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp5C5->width != 0  && _tmp5C6->width == 0  || _tmp5C6->width != 0  && _tmp5C5->width
== 0) || (_tmp5C5->width != 0  && _tmp5C6->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5C5->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp5C6->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp12AC;Cyc_Tcutil_failure_reason=((_tmp12AC="(different bitfield widths on member)",
_tag_dyneither(_tmp12AC,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp537 == 0  && _tmp533 == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp12AD;Cyc_Tcutil_failure_reason=((_tmp12AD="(different number of members)",
_tag_dyneither(_tmp12AD,sizeof(char),30)));}goto _LL294;_LL2B7: _tmp538=_tmp495.f1;{
struct Cyc_Absyn_TypedefType_struct*_tmp539=(struct Cyc_Absyn_TypedefType_struct*)
_tmp538;if(_tmp539->tag != 17)goto _LL2B9;else{_tmp53A=_tmp539->f2;_tmp53B=_tmp539->f3;}};
_tmp53C=_tmp495.f2;{struct Cyc_Absyn_TypedefType_struct*_tmp53D=(struct Cyc_Absyn_TypedefType_struct*)
_tmp53C;if(_tmp53D->tag != 17)goto _LL2B9;else{_tmp53E=_tmp53D->f2;_tmp53F=_tmp53D->f3;}};
_LL2B8: if(_tmp53B != _tmp53F){{const char*_tmp12AE;Cyc_Tcutil_failure_reason=((
_tmp12AE="(different abstract typedefs)",_tag_dyneither(_tmp12AE,sizeof(char),30)));}
goto _LL294;}{const char*_tmp12AF;Cyc_Tcutil_failure_reason=((_tmp12AF="(type parameters to typedef differ)",
_tag_dyneither(_tmp12AF,sizeof(char),36)));}Cyc_Tcutil_unify_list(_tmp53A,
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
t2))return;{const char*_tmp12B0;Cyc_Tcutil_failure_reason=((_tmp12B0="(effects don't unify)",
_tag_dyneither(_tmp12B0,sizeof(char),22)));}goto _LL294;_LL2CF:;_LL2D0: goto _LL294;
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
_tmp5DC.Forward_constr).val;_LL2F5: {const char*_tmp12B3;void*_tmp12B2;(_tmp12B2=
0,Cyc_Tcutil_impos(((_tmp12B3="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp12B3,sizeof(char),40))),_tag_dyneither(_tmp12B2,sizeof(void*),
0)));}_LL2EF:;}_LL2ED: _tmp5D5=*_tmp5D0;if((_tmp5D5.Forward_constr).tag != 2)goto
_LL2E8;_tmp5D6=(union Cyc_Absyn_Constraint*)(_tmp5D5.Forward_constr).val;_LL2EE: {
const char*_tmp12B6;void*_tmp12B5;(_tmp12B5=0,Cyc_Tcutil_impos(((_tmp12B6="unify_conref: forward after compress",
_tag_dyneither(_tmp12B6,sizeof(char),37))),_tag_dyneither(_tmp12B5,sizeof(void*),
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
return _tmp60E;{struct _tuple0 _tmp12B7;struct _tuple0 _tmp610=(_tmp12B7.f1=t2,((
_tmp12B7.f2=t1,_tmp12B7)));void*_tmp611;void*_tmp613;void*_tmp615;struct Cyc_Absyn_Tvar*
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
const char*_tmp12BA;void*_tmp12B9;(_tmp12B9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12BA="typecmp: can only compare closed types",
_tag_dyneither(_tmp12BA,sizeof(char),39))),_tag_dyneither(_tmp12B9,sizeof(void*),
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
char*_tmp12BD;void*_tmp12BC;(_tmp12BC=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12BD="missing expression in array index",
_tag_dyneither(_tmp12BD,sizeof(char),34))),_tag_dyneither(_tmp12BC,sizeof(void*),
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
_tmp68D.rgn_po;_tmp696=_tmp68D.attributes;}};_LL347: {const char*_tmp12C0;void*
_tmp12BF;(_tmp12BF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C0="typecmp: function types not handled",
_tag_dyneither(_tmp12C0,sizeof(char),36))),_tag_dyneither(_tmp12BF,sizeof(void*),
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
_tmp6C9;if(_tmp6CA->tag != 23)goto _LL360;};_LL35F: {const char*_tmp12C3;void*
_tmp12C2;(_tmp12C2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12C3="typecmp: effects not handled",_tag_dyneither(
_tmp12C3,sizeof(char),29))),_tag_dyneither(_tmp12C2,sizeof(void*),0)));}_LL360:;
_LL361: {const char*_tmp12C6;void*_tmp12C5;(_tmp12C5=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C6="Unmatched case in typecmp",
_tag_dyneither(_tmp12C6,sizeof(char),26))),_tag_dyneither(_tmp12C5,sizeof(void*),
0)));}_LL32F:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp6E8=Cyc_Tcutil_compress(
t);_LL368: {struct Cyc_Absyn_IntType_struct*_tmp6E9=(struct Cyc_Absyn_IntType_struct*)
_tmp6E8;if(_tmp6E9->tag != 6)goto _LL36A;}_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_FloatType_struct*
_tmp6EA=(struct Cyc_Absyn_FloatType_struct*)_tmp6E8;if(_tmp6EA->tag != 7)goto
_LL36C;}_LL36B: goto _LL36D;_LL36C: {struct Cyc_Absyn_EnumType_struct*_tmp6EB=(
struct Cyc_Absyn_EnumType_struct*)_tmp6E8;if(_tmp6EB->tag != 13)goto _LL36E;}_LL36D:
goto _LL36F;_LL36E: {struct Cyc_Absyn_AnonEnumType_struct*_tmp6EC=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp6E8;if(_tmp6EC->tag != 14)goto _LL370;}_LL36F: return 1;_LL370:;_LL371: return 0;
_LL367:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp12C7;struct _tuple0 _tmp6EE=(
_tmp12C7.f1=t1,((_tmp12C7.f2=t2,_tmp12C7)));void*_tmp6EF;int _tmp6F1;void*_tmp6F2;
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
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle
_tmp746=_new_region("r");struct _RegionHandle*r=& _tmp746;_push_region(r);{struct
Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)
el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0
 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){struct Cyc_Core_Opt*
_tmp12C8;max_arith_type=((_tmp12C8=_region_malloc(r,sizeof(*_tmp12C8)),((
_tmp12C8->v=t1,_tmp12C8))));}}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(
void*)max_arith_type->v)){int _tmp748=0;_npop_handler(0);return _tmp748;}}}{struct
Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(te,(
struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp12CD;void*_tmp12CC[2];struct Cyc_String_pa_struct
_tmp12CB;struct Cyc_String_pa_struct _tmp12CA;(_tmp12CA.tag=0,((_tmp12CA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp12CB.tag=0,((_tmp12CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12CC[0]=& _tmp12CB,((_tmp12CC[1]=& _tmp12CA,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp12CD="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp12CD,sizeof(char),41))),_tag_dyneither(_tmp12CC,sizeof(void*),
2)))))))))))));}{int _tmp74D=0;_npop_handler(0);return _tmp74D;};}}}{int _tmp74E=1;
_npop_handler(0);return _tmp74E;};;_pop_region(r);}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp74F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL396: {struct Cyc_Absyn_PointerType_struct*_tmp750=(struct Cyc_Absyn_PointerType_struct*)
_tmp74F;if(_tmp750->tag != 5)goto _LL398;}_LL397: Cyc_Tcutil_unchecked_cast(te,e,
Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL395;_LL398:;_LL399: return 0;
_LL395:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t){void*_tmp751=Cyc_Tcutil_compress(
t);_LL39B: {struct Cyc_Absyn_IntType_struct*_tmp752=(struct Cyc_Absyn_IntType_struct*)
_tmp751;if(_tmp752->tag != 6)goto _LL39D;}_LL39C: goto _LL39E;_LL39D: {struct Cyc_Absyn_TagType_struct*
_tmp753=(struct Cyc_Absyn_TagType_struct*)_tmp751;if(_tmp753->tag != 19)goto _LL39F;}
_LL39E: goto _LL3A0;_LL39F: {struct Cyc_Absyn_EnumType_struct*_tmp754=(struct Cyc_Absyn_EnumType_struct*)
_tmp751;if(_tmp754->tag != 13)goto _LL3A1;}_LL3A0: goto _LL3A2;_LL3A1: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp755=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp751;if(_tmp755->tag != 14)goto
_LL3A3;}_LL3A2: return 1;_LL3A3:;_LL3A4: return 0;_LL39A:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp12D0;void*_tmp12CF;(_tmp12CF=0,Cyc_Tcutil_warn(e->loc,((_tmp12D0="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12D0,sizeof(char),44))),_tag_dyneither(_tmp12CF,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp12D3;void*_tmp12D2;(
_tmp12D2=0,Cyc_Tcutil_warn(e->loc,((_tmp12D3="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp12D3,sizeof(char),44))),_tag_dyneither(_tmp12D2,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,unsigned int loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);{struct _tuple0 _tmp12D4;struct _tuple0 _tmp75B=(_tmp12D4.f1=t1,((_tmp12D4.f2=t2,
_tmp12D4)));void*_tmp75C;struct Cyc_Absyn_PtrInfo _tmp75E;void*_tmp75F;struct Cyc_Absyn_PtrInfo
_tmp761;void*_tmp762;struct Cyc_Absyn_ArrayInfo _tmp764;void*_tmp765;struct Cyc_Absyn_Tqual
_tmp766;struct Cyc_Absyn_Exp*_tmp767;union Cyc_Absyn_Constraint*_tmp768;void*
_tmp769;struct Cyc_Absyn_ArrayInfo _tmp76B;void*_tmp76C;struct Cyc_Absyn_Tqual
_tmp76D;struct Cyc_Absyn_Exp*_tmp76E;union Cyc_Absyn_Constraint*_tmp76F;void*
_tmp770;void*_tmp772;_LL3A6: _tmp75C=_tmp75B.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp75D=(struct Cyc_Absyn_PointerType_struct*)_tmp75C;if(_tmp75D->tag != 5)goto
_LL3A8;else{_tmp75E=_tmp75D->f1;}};_tmp75F=_tmp75B.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp760=(struct Cyc_Absyn_PointerType_struct*)_tmp75F;if(_tmp760->tag != 5)goto
_LL3A8;else{_tmp761=_tmp760->f1;}};_LL3A7: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp75E.ptr_atts).nullable,(_tmp761.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp75E.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp75E.ptr_atts).bounds,(
_tmp761.ptr_atts).bounds)){struct _tuple0 _tmp12D5;struct _tuple0 _tmp775=(_tmp12D5.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp75E.ptr_atts).bounds),((_tmp12D5.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp761.ptr_atts).bounds),
_tmp12D5)));void*_tmp776;void*_tmp778;void*_tmp77A;void*_tmp77C;void*_tmp77E;
struct Cyc_Absyn_Exp*_tmp780;void*_tmp781;struct Cyc_Absyn_Exp*_tmp783;void*
_tmp784;void*_tmp786;struct Cyc_Absyn_Exp*_tmp788;_LL3AF: _tmp776=_tmp775.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp777=(struct Cyc_Absyn_Upper_b_struct*)_tmp776;
if(_tmp777->tag != 1)goto _LL3B1;};_tmp778=_tmp775.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp779=(struct Cyc_Absyn_DynEither_b_struct*)_tmp778;if(_tmp779->tag != 0)goto
_LL3B1;};_LL3B0: goto _LL3B2;_LL3B1: _tmp77A=_tmp775.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp77B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp77A;if(_tmp77B->tag != 0)goto
_LL3B3;};_tmp77C=_tmp775.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp77D=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp77C;if(_tmp77D->tag != 0)goto _LL3B3;};_LL3B2:
okay=1;goto _LL3AE;_LL3B3: _tmp77E=_tmp775.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp77F=(struct Cyc_Absyn_Upper_b_struct*)_tmp77E;if(_tmp77F->tag != 1)goto _LL3B5;
else{_tmp780=_tmp77F->f1;}};_tmp781=_tmp775.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp782=(struct Cyc_Absyn_Upper_b_struct*)_tmp781;if(_tmp782->tag != 1)goto _LL3B5;
else{_tmp783=_tmp782->f1;}};_LL3B4: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp783,
_tmp780);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp761.ptr_atts).zero_term)){const char*_tmp12D8;void*_tmp12D7;(_tmp12D7=0,
Cyc_Tcutil_warn(loc,((_tmp12D8="implicit cast to shorter array",_tag_dyneither(
_tmp12D8,sizeof(char),31))),_tag_dyneither(_tmp12D7,sizeof(void*),0)));}goto
_LL3AE;_LL3B5: _tmp784=_tmp775.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp785=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp784;if(_tmp785->tag != 0)goto _LL3AE;};
_tmp786=_tmp775.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp787=(struct Cyc_Absyn_Upper_b_struct*)
_tmp786;if(_tmp787->tag != 1)goto _LL3AE;else{_tmp788=_tmp787->f1;}};_LL3B6: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp75E.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp761.ptr_atts).bounds))goto _LL3AE;okay=0;goto
_LL3AE;_LL3AE:;}okay=okay  && (!(_tmp75E.elt_tq).real_const  || (_tmp761.elt_tq).real_const);
if(!Cyc_Tcutil_unify((_tmp75E.ptr_atts).rgn,(_tmp761.ptr_atts).rgn)){if(Cyc_Tcenv_region_outlives(
te,(_tmp75E.ptr_atts).rgn,(_tmp761.ptr_atts).rgn)){if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp12DD;void*_tmp12DC[2];struct Cyc_String_pa_struct _tmp12DB;struct Cyc_String_pa_struct
_tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((_tmp761.ptr_atts).rgn)),((_tmp12DB.tag=0,((
_tmp12DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
_tmp75E.ptr_atts).rgn)),((_tmp12DC[0]=& _tmp12DB,((_tmp12DC[1]=& _tmp12DA,Cyc_Tcutil_warn(
loc,((_tmp12DD="implicit cast form region %s to region %s",_tag_dyneither(
_tmp12DD,sizeof(char),42))),_tag_dyneither(_tmp12DC,sizeof(void*),2)))))))))))));}}
else{okay=0;}}okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(
_tmp75E.ptr_atts).zero_term,(_tmp761.ptr_atts).zero_term) || ((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp75E.ptr_atts).zero_term)
 && (_tmp761.elt_tq).real_const);{int _tmp78F=((int(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,(_tmp761.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref)
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(
_tmp761.ptr_atts).zero_term);okay=okay  && (Cyc_Tcutil_unify(_tmp75E.elt_typ,
_tmp761.elt_typ) || (_tmp78F  && ((_tmp761.elt_tq).real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp761.elt_typ)))) && Cyc_Tcutil_ptrsubtype(
te,0,_tmp75E.elt_typ,_tmp761.elt_typ));return okay;};}_LL3A8: _tmp762=_tmp75B.f1;{
struct Cyc_Absyn_ArrayType_struct*_tmp763=(struct Cyc_Absyn_ArrayType_struct*)
_tmp762;if(_tmp763->tag != 8)goto _LL3AA;else{_tmp764=_tmp763->f1;_tmp765=_tmp764.elt_type;
_tmp766=_tmp764.tq;_tmp767=_tmp764.num_elts;_tmp768=_tmp764.zero_term;}};_tmp769=
_tmp75B.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp76A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp769;if(_tmp76A->tag != 8)goto _LL3AA;else{_tmp76B=_tmp76A->f1;_tmp76C=_tmp76B.elt_type;
_tmp76D=_tmp76B.tq;_tmp76E=_tmp76B.num_elts;_tmp76F=_tmp76B.zero_term;}};_LL3A9: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp768,_tmp76F) && ((_tmp767 != 0
 && _tmp76E != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp767,(
struct Cyc_Absyn_Exp*)_tmp76E));return(okay  && Cyc_Tcutil_unify(_tmp765,_tmp76C))
 && (!_tmp766.real_const  || _tmp76D.real_const);}_LL3AA: _tmp770=_tmp75B.f1;{
struct Cyc_Absyn_TagType_struct*_tmp771=(struct Cyc_Absyn_TagType_struct*)_tmp770;
if(_tmp771->tag != 19)goto _LL3AC;};_tmp772=_tmp75B.f2;{struct Cyc_Absyn_IntType_struct*
_tmp773=(struct Cyc_Absyn_IntType_struct*)_tmp772;if(_tmp773->tag != 6)goto _LL3AC;};
_LL3AB: return 0;_LL3AC:;_LL3AD: return Cyc_Tcutil_unify(t1,t2);_LL3A5:;};}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp790=Cyc_Tcutil_compress(t);_LL3B8: {struct Cyc_Absyn_PointerType_struct*
_tmp791=(struct Cyc_Absyn_PointerType_struct*)_tmp790;if(_tmp791->tag != 5)goto
_LL3BA;}_LL3B9: return 1;_LL3BA:;_LL3BB: return 0;_LL3B7:;}void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp792=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp794;void*
_tmp795;_LL3BD: {struct Cyc_Absyn_PointerType_struct*_tmp793=(struct Cyc_Absyn_PointerType_struct*)
_tmp792;if(_tmp793->tag != 5)goto _LL3BF;else{_tmp794=_tmp793->f1;_tmp795=_tmp794.elt_typ;}}
_LL3BE: return _tmp795;_LL3BF:;_LL3C0: {const char*_tmp12E0;void*_tmp12DF;(_tmp12DF=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12E0="pointer_elt_type",_tag_dyneither(_tmp12E0,sizeof(char),17))),
_tag_dyneither(_tmp12DF,sizeof(void*),0)));}_LL3BC:;}void*Cyc_Tcutil_pointer_region(
void*t){void*_tmp798=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp79A;
struct Cyc_Absyn_PtrAtts _tmp79B;struct Cyc_Absyn_PtrAtts*_tmp79C;_LL3C2: {struct
Cyc_Absyn_PointerType_struct*_tmp799=(struct Cyc_Absyn_PointerType_struct*)
_tmp798;if(_tmp799->tag != 5)goto _LL3C4;else{_tmp79A=_tmp799->f1;_tmp79B=_tmp79A.ptr_atts;
_tmp79C=(struct Cyc_Absyn_PtrAtts*)&(_tmp799->f1).ptr_atts;}}_LL3C3: return _tmp79C->rgn;
_LL3C4:;_LL3C5: {const char*_tmp12E3;void*_tmp12E2;(_tmp12E2=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E3="pointer_elt_type",
_tag_dyneither(_tmp12E3,sizeof(char),17))),_tag_dyneither(_tmp12E2,sizeof(void*),
0)));}_LL3C1:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){void*_tmp79F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp7A1;struct Cyc_Absyn_PtrAtts _tmp7A2;void*_tmp7A3;
_LL3C7: {struct Cyc_Absyn_PointerType_struct*_tmp7A0=(struct Cyc_Absyn_PointerType_struct*)
_tmp79F;if(_tmp7A0->tag != 5)goto _LL3C9;else{_tmp7A1=_tmp7A0->f1;_tmp7A2=_tmp7A1.ptr_atts;
_tmp7A3=_tmp7A2.rgn;}}_LL3C8:*rgn=_tmp7A3;return 1;_LL3C9:;_LL3CA: return 0;_LL3C6:;}
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){void*_tmp7A4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp7A6;struct Cyc_Absyn_PtrAtts _tmp7A7;union Cyc_Absyn_Constraint*
_tmp7A8;_LL3CC: {struct Cyc_Absyn_PointerType_struct*_tmp7A5=(struct Cyc_Absyn_PointerType_struct*)
_tmp7A4;if(_tmp7A5->tag != 5)goto _LL3CE;else{_tmp7A6=_tmp7A5->f1;_tmp7A7=_tmp7A6.ptr_atts;
_tmp7A8=_tmp7A7.bounds;}}_LL3CD:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp7A8)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3CE:;_LL3CF: return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;
_LL3CB:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*_tmp7A9=e->r;union Cyc_Absyn_Cnst
_tmp7AB;struct _tuple7 _tmp7AC;int _tmp7AD;union Cyc_Absyn_Cnst _tmp7AF;struct _tuple5
_tmp7B0;char _tmp7B1;union Cyc_Absyn_Cnst _tmp7B3;struct _tuple6 _tmp7B4;short _tmp7B5;
union Cyc_Absyn_Cnst _tmp7B7;struct _tuple8 _tmp7B8;long long _tmp7B9;union Cyc_Absyn_Cnst
_tmp7BB;struct _dyneither_ptr _tmp7BC;void*_tmp7BE;struct Cyc_Absyn_Exp*_tmp7BF;
_LL3D1: {struct Cyc_Absyn_Const_e_struct*_tmp7AA=(struct Cyc_Absyn_Const_e_struct*)
_tmp7A9;if(_tmp7AA->tag != 0)goto _LL3D3;else{_tmp7AB=_tmp7AA->f1;if((_tmp7AB.Int_c).tag
!= 5)goto _LL3D3;_tmp7AC=(struct _tuple7)(_tmp7AB.Int_c).val;_tmp7AD=_tmp7AC.f2;
if(_tmp7AD != 0)goto _LL3D3;}}_LL3D2: goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Const_e_struct*
_tmp7AE=(struct Cyc_Absyn_Const_e_struct*)_tmp7A9;if(_tmp7AE->tag != 0)goto _LL3D5;
else{_tmp7AF=_tmp7AE->f1;if((_tmp7AF.Char_c).tag != 2)goto _LL3D5;_tmp7B0=(struct
_tuple5)(_tmp7AF.Char_c).val;_tmp7B1=_tmp7B0.f2;if(_tmp7B1 != 0)goto _LL3D5;}}
_LL3D4: goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Const_e_struct*_tmp7B2=(struct Cyc_Absyn_Const_e_struct*)
_tmp7A9;if(_tmp7B2->tag != 0)goto _LL3D7;else{_tmp7B3=_tmp7B2->f1;if((_tmp7B3.Short_c).tag
!= 4)goto _LL3D7;_tmp7B4=(struct _tuple6)(_tmp7B3.Short_c).val;_tmp7B5=_tmp7B4.f2;
if(_tmp7B5 != 0)goto _LL3D7;}}_LL3D6: goto _LL3D8;_LL3D7: {struct Cyc_Absyn_Const_e_struct*
_tmp7B6=(struct Cyc_Absyn_Const_e_struct*)_tmp7A9;if(_tmp7B6->tag != 0)goto _LL3D9;
else{_tmp7B7=_tmp7B6->f1;if((_tmp7B7.LongLong_c).tag != 6)goto _LL3D9;_tmp7B8=(
struct _tuple8)(_tmp7B7.LongLong_c).val;_tmp7B9=_tmp7B8.f2;if(_tmp7B9 != 0)goto
_LL3D9;}}_LL3D8: return 1;_LL3D9: {struct Cyc_Absyn_Const_e_struct*_tmp7BA=(struct
Cyc_Absyn_Const_e_struct*)_tmp7A9;if(_tmp7BA->tag != 0)goto _LL3DB;else{_tmp7BB=
_tmp7BA->f1;if((_tmp7BB.Wchar_c).tag != 3)goto _LL3DB;_tmp7BC=(struct
_dyneither_ptr)(_tmp7BB.Wchar_c).val;}}_LL3DA: {unsigned long _tmp7C0=Cyc_strlen((
struct _dyneither_ptr)_tmp7BC);int i=0;if(_tmp7C0 >= 2  && *((const char*)
_check_dyneither_subscript(_tmp7BC,sizeof(char),0))== '\\'){if(*((const char*)
_check_dyneither_subscript(_tmp7BC,sizeof(char),1))== '0')i=2;else{if((*((const
char*)_check_dyneither_subscript(_tmp7BC,sizeof(char),1))== 'x'  && _tmp7C0 >= 3)
 && *((const char*)_check_dyneither_subscript(_tmp7BC,sizeof(char),2))== '0')i=3;
else{return 0;}}for(0;i < _tmp7C0;++ i){if(*((const char*)_check_dyneither_subscript(
_tmp7BC,sizeof(char),i))!= '0')return 0;}return 1;}else{return 0;}}_LL3DB: {struct
Cyc_Absyn_Cast_e_struct*_tmp7BD=(struct Cyc_Absyn_Cast_e_struct*)_tmp7A9;if(
_tmp7BD->tag != 15)goto _LL3DD;else{_tmp7BE=(void*)_tmp7BD->f1;_tmp7BF=_tmp7BD->f2;}}
_LL3DC: return Cyc_Tcutil_is_zero(_tmp7BF) && Cyc_Tcutil_admits_zero(_tmp7BE);
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
ka){struct Cyc_Absyn_Kind _tmp7C2;enum Cyc_Absyn_KindQual _tmp7C3;enum Cyc_Absyn_AliasQual
_tmp7C4;struct Cyc_Absyn_Kind*_tmp7C1=ka;_tmp7C2=*_tmp7C1;_tmp7C3=_tmp7C2.kind;
_tmp7C4=_tmp7C2.aliasqual;switch(_tmp7C4){case Cyc_Absyn_Aliasable: _LL3DF: switch(
_tmp7C3){case Cyc_Absyn_AnyKind: _LL3E1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ako;
case Cyc_Absyn_MemKind: _LL3E2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind:
_LL3E3: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3E4:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3E5: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3E6: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3E0: switch(_tmp7C3){case
Cyc_Absyn_AnyKind: _LL3E9: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind:
_LL3EA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3EB:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3EC: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urko;default: _LL3ED: break;}break;case Cyc_Absyn_Top:
_LL3E8: switch(_tmp7C3){case Cyc_Absyn_AnyKind: _LL3F0: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3F1: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko;
case Cyc_Absyn_BoxKind: _LL3F2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind:
_LL3F3: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trko;default: _LL3F4: break;}break;}{
const char*_tmp12E7;void*_tmp12E6[1];struct Cyc_String_pa_struct _tmp12E5;(_tmp12E5.tag=
0,((_tmp12E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12E6[0]=& _tmp12E5,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E7="kind_to_opt: bad kind %s\n",
_tag_dyneither(_tmp12E7,sizeof(char),26))),_tag_dyneither(_tmp12E6,sizeof(void*),
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
ubb_v;static void*urb=(void*)& urb_v;struct Cyc_Absyn_Kind _tmp7C9;enum Cyc_Absyn_KindQual
_tmp7CA;enum Cyc_Absyn_AliasQual _tmp7CB;struct Cyc_Absyn_Kind*_tmp7C8=ka;_tmp7C9=*
_tmp7C8;_tmp7CA=_tmp7C9.kind;_tmp7CB=_tmp7C9.aliasqual;switch(_tmp7CB){case Cyc_Absyn_Aliasable:
_LL3F6: switch(_tmp7CA){case Cyc_Absyn_AnyKind: _LL3F8: return& ab;case Cyc_Absyn_MemKind:
_LL3F9: return& mb;case Cyc_Absyn_BoxKind: _LL3FA: return& bb;case Cyc_Absyn_RgnKind:
_LL3FB: return& rb;case Cyc_Absyn_EffKind: _LL3FC: return& eb;case Cyc_Absyn_IntKind:
_LL3FD: return& ib;}case Cyc_Absyn_Unique: _LL3F7: switch(_tmp7CA){case Cyc_Absyn_AnyKind:
_LL400: return& uab;case Cyc_Absyn_MemKind: _LL401: return& umb;case Cyc_Absyn_BoxKind:
_LL402: return& ubb;case Cyc_Absyn_RgnKind: _LL403: return& urb;default: _LL404: break;}
break;case Cyc_Absyn_Top: _LL3FF: switch(_tmp7CA){case Cyc_Absyn_AnyKind: _LL407:
return& tab;case Cyc_Absyn_MemKind: _LL408: return& tmb;case Cyc_Absyn_BoxKind: _LL409:
return& tbb;case Cyc_Absyn_RgnKind: _LL40A: return& trb;default: _LL40B: break;}break;}{
const char*_tmp12EB;void*_tmp12EA[1];struct Cyc_String_pa_struct _tmp12E9;(_tmp12E9.tag=
0,((_tmp12E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
ka)),((_tmp12EA[0]=& _tmp12E9,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EB="kind_to_b: bad kind %s\n",
_tag_dyneither(_tmp12EB,sizeof(char),24))),_tag_dyneither(_tmp12EA,sizeof(void*),
1)))))));};}void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){return*Cyc_Tcutil_kind_to_b(
k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
struct Cyc_Core_Opt*_tmp12EC;return(_tmp12EC=_cycalloc(sizeof(*_tmp12EC)),((
_tmp12EC->v=Cyc_Tcutil_kind_to_bound(k),_tmp12EC)));}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp12EF;struct Cyc_Absyn_Const_e_struct*
_tmp12EE;e1->r=(void*)((_tmp12EE=_cycalloc(sizeof(*_tmp12EE)),((_tmp12EE[0]=((
_tmp12EF.tag=0,((_tmp12EF.f1=Cyc_Absyn_Null_c,_tmp12EF)))),_tmp12EE))));}{struct
Cyc_Core_Opt*_tmp7E0=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp12F9;struct Cyc_Absyn_PtrAtts _tmp12F8;struct Cyc_Absyn_PtrInfo _tmp12F7;struct
Cyc_Absyn_PointerType_struct*_tmp12F6;struct Cyc_Absyn_PointerType_struct*_tmp7E1=(
_tmp12F6=_cycalloc(sizeof(*_tmp12F6)),((_tmp12F6[0]=((_tmp12F9.tag=5,((_tmp12F9.f1=((
_tmp12F7.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmp7E0),((
_tmp12F7.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp12F7.ptr_atts=((_tmp12F8.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,_tmp7E0),((_tmp12F8.nullable=Cyc_Absyn_true_conref,((
_tmp12F8.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12F8.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp12F8.ptrloc=0,_tmp12F8)))))))))),_tmp12F7)))))),_tmp12F9)))),_tmp12F6)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp7E1);{int bogus=
0;int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);if(bogus != 0){const char*
_tmp12FE;void*_tmp12FD[2];struct Cyc_String_pa_struct _tmp12FC;struct Cyc_String_pa_struct
_tmp12FB;(_tmp12FB.tag=0,((_tmp12FB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(e1->loc)),((_tmp12FC.tag=0,((
_tmp12FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp12FD[0]=& _tmp12FC,((_tmp12FD[1]=& _tmp12FB,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12FE="zero_to_null resulted in an alias coercion on %s at %s\n",
_tag_dyneither(_tmp12FE,sizeof(char),56))),_tag_dyneither(_tmp12FD,sizeof(void*),
2)))))))))))));}return retv;};};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(
enum Cyc_Absyn_Coercion c){switch(c){case Cyc_Absyn_Unknown_coercion: _LL40D: {
const char*_tmp12FF;return(_tmp12FF="unknown",_tag_dyneither(_tmp12FF,sizeof(char),
8));}case Cyc_Absyn_No_coercion: _LL40E: {const char*_tmp1300;return(_tmp1300="no coercion",
_tag_dyneither(_tmp1300,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL40F: {
const char*_tmp1301;return(_tmp1301="null check",_tag_dyneither(_tmp1301,sizeof(
char),11));}case Cyc_Absyn_Other_coercion: _LL410: {const char*_tmp1302;return(
_tmp1302="other coercion",_tag_dyneither(_tmp1302,sizeof(char),15));}}}int Cyc_Tcutil_warn_alias_coerce=
0;struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){static
struct Cyc_Absyn_Eq_kb_struct rgn_kb={0,& Cyc_Tcutil_rk};static int counter=0;struct
_dyneither_ptr*_tmp130F;const char*_tmp130E;void*_tmp130D[1];struct Cyc_Int_pa_struct
_tmp130C;struct _tuple2*_tmp130B;struct _tuple2*v=(_tmp130B=_cycalloc(sizeof(*
_tmp130B)),((_tmp130B->f1=Cyc_Absyn_Loc_n,((_tmp130B->f2=((_tmp130F=_cycalloc(
sizeof(*_tmp130F)),((_tmp130F[0]=(struct _dyneither_ptr)((_tmp130C.tag=1,((
_tmp130C.f1=(unsigned long)counter ++,((_tmp130D[0]=& _tmp130C,Cyc_aprintf(((
_tmp130E="__aliasvar%d",_tag_dyneither(_tmp130E,sizeof(char),13))),
_tag_dyneither(_tmp130D,sizeof(void*),1)))))))),_tmp130F)))),_tmp130B)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,(void*)& Cyc_Absyn_VoidType_val,
0);struct Cyc_Absyn_Local_b_struct _tmp1312;struct Cyc_Absyn_Local_b_struct*
_tmp1311;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp1311=_cycalloc(
sizeof(*_tmp1311)),((_tmp1311[0]=((_tmp1312.tag=4,((_tmp1312.f1=vd,_tmp1312)))),
_tmp1311)))),e->loc);struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);{
void*_tmp7EE=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_PtrInfo _tmp7F0;struct Cyc_Absyn_PtrAtts
_tmp7F1;void*_tmp7F2;void**_tmp7F3;_LL413: {struct Cyc_Absyn_PointerType_struct*
_tmp7EF=(struct Cyc_Absyn_PointerType_struct*)_tmp7EE;if(_tmp7EF->tag != 5)goto
_LL415;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=_tmp7F0.ptr_atts;_tmp7F2=_tmp7F1.rgn;
_tmp7F3=(void**)&((_tmp7EF->f1).ptr_atts).rgn;}}_LL414:{struct Cyc_Absyn_VarType_struct
_tmp1315;struct Cyc_Absyn_VarType_struct*_tmp1314;*_tmp7F3=(void*)((_tmp1314=
_cycalloc(sizeof(*_tmp1314)),((_tmp1314[0]=((_tmp1315.tag=2,((_tmp1315.f1=tv,
_tmp1315)))),_tmp1314))));}goto _LL412;_LL415:;_LL416: goto _LL412;_LL412:;}e->topt=
0;e=Cyc_Absyn_cast_exp(e_typ,e,0,Cyc_Absyn_Unknown_coercion,e->loc);{struct Cyc_Absyn_Decl*
d=Cyc_Absyn_alias_decl(e,tv,vd,e->loc);struct _tuple15 _tmp1316;return(_tmp1316.f1=
d,((_tmp1316.f2=ve,_tmp1316)));};}static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*
e,void*e_typ,void*wants_typ,unsigned int loc){struct _RegionHandle _tmp7FF=
_new_region("r");struct _RegionHandle*r=& _tmp7FF;_push_region(r);if((Cyc_Tcutil_is_noalias_path(
r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ)) && Cyc_Tcutil_is_pointer_type(
e_typ)){void*_tmp800=Cyc_Tcutil_compress(wants_typ);struct Cyc_Absyn_PtrInfo
_tmp802;struct Cyc_Absyn_PtrAtts _tmp803;void*_tmp804;_LL418: {struct Cyc_Absyn_PointerType_struct*
_tmp801=(struct Cyc_Absyn_PointerType_struct*)_tmp800;if(_tmp801->tag != 5)goto
_LL41A;else{_tmp802=_tmp801->f1;_tmp803=_tmp802.ptr_atts;_tmp804=_tmp803.rgn;}}
_LL419: {void*_tmp806=Cyc_Tcutil_compress(_tmp804);_LL41F: {struct Cyc_Absyn_HeapRgn_struct*
_tmp807=(struct Cyc_Absyn_HeapRgn_struct*)_tmp806;if(_tmp807->tag != 20)goto _LL421;}
_LL420: {int _tmp808=0;_npop_handler(0);return _tmp808;}_LL421:;_LL422: {struct Cyc_Absyn_Kind*
_tmp809=Cyc_Tcutil_typ_kind(_tmp804);int _tmp80A=_tmp809->kind == Cyc_Absyn_RgnKind
 && _tmp809->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp80A;}
_LL41E:;}_LL41A: {struct Cyc_Absyn_TypedefType_struct*_tmp805=(struct Cyc_Absyn_TypedefType_struct*)
_tmp800;if(_tmp805->tag != 17)goto _LL41C;}_LL41B: {const char*_tmp131B;void*
_tmp131A[2];struct Cyc_String_pa_struct _tmp1319;struct Cyc_String_pa_struct
_tmp1318;(_tmp1318.tag=0,((_tmp1318.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Position_string_of_segment(loc)),((_tmp1319.tag=0,((_tmp1319.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(
wants_typ))),((_tmp131A[0]=& _tmp1319,((_tmp131A[1]=& _tmp1318,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp131B="got typedef %s in can_insert_alias at %s\n",
_tag_dyneither(_tmp131B,sizeof(char),42))),_tag_dyneither(_tmp131A,sizeof(void*),
2)))))))))))));}_LL41C:;_LL41D: {int _tmp80F=0;_npop_handler(0);return _tmp80F;}
_LL417:;}{int _tmp810=0;_npop_handler(0);return _tmp810;};;_pop_region(r);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){void*
t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
enum Cyc_Absyn_Coercion c;int do_alias_coercion=0;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1320;void*_tmp131F[2];
struct Cyc_String_pa_struct _tmp131E;struct Cyc_String_pa_struct _tmp131D;(_tmp131D.tag=
0,((_tmp131D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp131E.tag=0,((_tmp131E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp131F[0]=& _tmp131E,((_tmp131F[1]=& _tmp131D,Cyc_Tcutil_warn(
e->loc,((_tmp1320="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1320,sizeof(char),53))),_tag_dyneither(_tmp131F,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1326;void*_tmp1325[3];struct Cyc_String_pa_struct _tmp1324;struct Cyc_String_pa_struct
_tmp1323;struct Cyc_String_pa_struct _tmp1322;(_tmp1322.tag=0,((_tmp1322.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp1323.tag=0,((_tmp1323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp1324.tag=0,((_tmp1324.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmp1325[0]=& _tmp1324,((_tmp1325[1]=& _tmp1323,((
_tmp1325[2]=& _tmp1322,Cyc_Tcutil_warn(e->loc,((_tmp1326="implicit alias coercion for %s:%s to %s",
_tag_dyneither(_tmp1326,sizeof(char),40))),_tag_dyneither(_tmp1325,sizeof(void*),
3)))))))))))))))))));}*alias_coercion=1;}if(Cyc_Tcutil_silent_castable(te,e->loc,
t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);return 1;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(
te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){Cyc_Tcutil_unchecked_cast(te,e,
t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*_tmp132B;void*_tmp132A[2];
struct Cyc_String_pa_struct _tmp1329;struct Cyc_String_pa_struct _tmp1328;(_tmp1328.tag=
0,((_tmp1328.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1329.tag=0,((_tmp1329.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp132A[0]=& _tmp1329,((_tmp132A[1]=& _tmp1328,Cyc_Tcutil_warn(
e->loc,((_tmp132B="implicit cast from %s to %s",_tag_dyneither(_tmp132B,sizeof(
char),28))),_tag_dyneither(_tmp132A,sizeof(void*),2)))))))))))));}return 1;}else{
return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t){int bogus=0;return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}int Cyc_Tcutil_coerceable(
void*t){void*_tmp81E=Cyc_Tcutil_compress(t);_LL424: {struct Cyc_Absyn_IntType_struct*
_tmp81F=(struct Cyc_Absyn_IntType_struct*)_tmp81E;if(_tmp81F->tag != 6)goto _LL426;}
_LL425: goto _LL427;_LL426: {struct Cyc_Absyn_FloatType_struct*_tmp820=(struct Cyc_Absyn_FloatType_struct*)
_tmp81E;if(_tmp820->tag != 7)goto _LL428;}_LL427: return 1;_LL428:;_LL429: return 0;
_LL423:;}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,
int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple24{struct Cyc_List_List*
f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){struct
Cyc_List_List*_tmp822;struct _RegionHandle*_tmp823;struct Cyc_Tcenv_Tenv*_tmp824;
int _tmp825;struct _tuple24 _tmp821=*env;_tmp822=_tmp821.f1;_tmp823=_tmp821.f2;
_tmp824=_tmp821.f3;_tmp825=_tmp821.f4;{void*_tmp826=Cyc_Tcutil_rsubstitute(
_tmp823,_tmp822,x->type);struct Cyc_List_List*_tmp827=Cyc_Tcutil_flatten_typ(
_tmp823,_tmp825,_tmp824,_tmp826);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp827)== 1){struct _tuple12*_tmp132E;struct Cyc_List_List*_tmp132D;return(
_tmp132D=_region_malloc(_tmp823,sizeof(*_tmp132D)),((_tmp132D->hd=((_tmp132E=
_region_malloc(_tmp823,sizeof(*_tmp132E)),((_tmp132E->f1=x->tq,((_tmp132E->f2=
_tmp826,_tmp132E)))))),((_tmp132D->tl=0,_tmp132D)))));}else{return _tmp827;}};}
struct _tuple25{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};static
struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
struct _RegionHandle*_tmp82B;struct Cyc_Tcenv_Tenv*_tmp82C;int _tmp82D;struct
_tuple25 _tmp82A=*env;_tmp82B=_tmp82A.f1;_tmp82C=_tmp82A.f2;_tmp82D=_tmp82A.f3;{
struct Cyc_Absyn_Tqual _tmp82F;void*_tmp830;struct _tuple12 _tmp82E=*x;_tmp82F=
_tmp82E.f1;_tmp830=_tmp82E.f2;{struct Cyc_List_List*_tmp831=Cyc_Tcutil_flatten_typ(
_tmp82B,_tmp82D,_tmp82C,_tmp830);if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp831)== 1){struct _tuple12*_tmp1331;struct Cyc_List_List*_tmp1330;return(
_tmp1330=_region_malloc(_tmp82B,sizeof(*_tmp1330)),((_tmp1330->hd=((_tmp1331=
_region_malloc(_tmp82B,sizeof(*_tmp1331)),((_tmp1331->f1=_tmp82F,((_tmp1331->f2=
_tmp830,_tmp1331)))))),((_tmp1330->tl=0,_tmp1330)))));}else{return _tmp831;}};};}
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int
flatten,struct Cyc_Tcenv_Tenv*te,void*t1){if(flatten){t1=Cyc_Tcutil_compress(t1);{
void*_tmp834=t1;struct Cyc_List_List*_tmp837;struct Cyc_Absyn_AggrInfo _tmp839;
union Cyc_Absyn_AggrInfoU _tmp83A;struct Cyc_Absyn_Aggrdecl**_tmp83B;struct Cyc_Absyn_Aggrdecl*
_tmp83C;struct Cyc_List_List*_tmp83D;enum Cyc_Absyn_AggrKind _tmp83F;struct Cyc_List_List*
_tmp840;_LL42B: {struct Cyc_Absyn_VoidType_struct*_tmp835=(struct Cyc_Absyn_VoidType_struct*)
_tmp834;if(_tmp835->tag != 0)goto _LL42D;}_LL42C: return 0;_LL42D: {struct Cyc_Absyn_TupleType_struct*
_tmp836=(struct Cyc_Absyn_TupleType_struct*)_tmp834;if(_tmp836->tag != 10)goto
_LL42F;else{_tmp837=_tmp836->f1;}}_LL42E: {struct _tuple25 _tmp1332;struct _tuple25
_tmp841=(_tmp1332.f1=r,((_tmp1332.f2=te,((_tmp1332.f3=flatten,_tmp1332)))));
struct Cyc_List_List*_tmp842=_tmp837;struct Cyc_List_List _tmp843;struct _tuple12*
_tmp844;struct Cyc_List_List*_tmp845;_LL436: if(_tmp842 != 0)goto _LL438;_LL437:
return 0;_LL438: if(_tmp842 == 0)goto _LL435;_tmp843=*_tmp842;_tmp844=(struct
_tuple12*)_tmp843.hd;_tmp845=_tmp843.tl;_LL439: {struct Cyc_List_List*_tmp846=Cyc_Tcutil_rcopy_tqt(&
_tmp841,_tmp844);_tmp841.f3=0;{struct Cyc_List_List*_tmp847=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),
struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,&
_tmp841,_tmp837);struct Cyc_List_List*_tmp1333;struct Cyc_List_List*_tmp848=(
_tmp1333=_region_malloc(r,sizeof(*_tmp1333)),((_tmp1333->hd=_tmp846,((_tmp1333->tl=
_tmp847,_tmp1333)))));struct Cyc_List_List*_tmp1334;return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1334=
_region_malloc(r,sizeof(*_tmp1334)),((_tmp1334->hd=_tmp846,((_tmp1334->tl=
_tmp847,_tmp1334)))))));};}_LL435:;}_LL42F: {struct Cyc_Absyn_AggrType_struct*
_tmp838=(struct Cyc_Absyn_AggrType_struct*)_tmp834;if(_tmp838->tag != 11)goto
_LL431;else{_tmp839=_tmp838->f1;_tmp83A=_tmp839.aggr_info;if((_tmp83A.KnownAggr).tag
!= 2)goto _LL431;_tmp83B=(struct Cyc_Absyn_Aggrdecl**)(_tmp83A.KnownAggr).val;
_tmp83C=*_tmp83B;_tmp83D=_tmp839.targs;}}_LL430: if(((_tmp83C->kind == Cyc_Absyn_UnionA
 || _tmp83C->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp83C->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp83C->impl))->rgn_po != 0){
struct _tuple12*_tmp1337;struct Cyc_List_List*_tmp1336;return(_tmp1336=
_region_malloc(r,sizeof(*_tmp1336)),((_tmp1336->hd=((_tmp1337=_region_malloc(r,
sizeof(*_tmp1337)),((_tmp1337->f1=Cyc_Absyn_empty_tqual(0),((_tmp1337->f2=t1,
_tmp1337)))))),((_tmp1336->tl=0,_tmp1336)))));}{struct Cyc_List_List*_tmp84E=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp83C->tvs,_tmp83D);struct _tuple24
_tmp1338;struct _tuple24 env=(_tmp1338.f1=_tmp84E,((_tmp1338.f2=r,((_tmp1338.f3=te,((
_tmp1338.f4=flatten,_tmp1338)))))));struct Cyc_List_List*_tmp84F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp83C->impl))->fields;struct Cyc_List_List _tmp850;struct Cyc_Absyn_Aggrfield*
_tmp851;struct Cyc_List_List*_tmp852;_LL43B: if(_tmp84F != 0)goto _LL43D;_LL43C:
return 0;_LL43D: if(_tmp84F == 0)goto _LL43A;_tmp850=*_tmp84F;_tmp851=(struct Cyc_Absyn_Aggrfield*)
_tmp850.hd;_tmp852=_tmp850.tl;_LL43E: {struct Cyc_List_List*_tmp853=Cyc_Tcutil_flatten_typ_f(&
env,_tmp851);env.f4=0;{struct Cyc_List_List*_tmp854=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp852);struct Cyc_List_List*_tmp1339;struct Cyc_List_List*_tmp855=(_tmp1339=
_region_malloc(r,sizeof(*_tmp1339)),((_tmp1339->hd=_tmp853,((_tmp1339->tl=
_tmp854,_tmp1339)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp855);};}_LL43A:;};_LL431: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp83E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp834;if(_tmp83E->tag != 12)goto _LL433;else{_tmp83F=_tmp83E->f1;if(_tmp83F != 
Cyc_Absyn_StructA)goto _LL433;_tmp840=_tmp83E->f2;}}_LL432: {struct _tuple24
_tmp133A;struct _tuple24 env=(_tmp133A.f1=0,((_tmp133A.f2=r,((_tmp133A.f3=te,((
_tmp133A.f4=flatten,_tmp133A)))))));struct Cyc_List_List*_tmp858=_tmp840;struct
Cyc_List_List _tmp859;struct Cyc_Absyn_Aggrfield*_tmp85A;struct Cyc_List_List*
_tmp85B;_LL440: if(_tmp858 != 0)goto _LL442;_LL441: return 0;_LL442: if(_tmp858 == 0)
goto _LL43F;_tmp859=*_tmp858;_tmp85A=(struct Cyc_Absyn_Aggrfield*)_tmp859.hd;
_tmp85B=_tmp859.tl;_LL443: {struct Cyc_List_List*_tmp85C=Cyc_Tcutil_flatten_typ_f(&
env,_tmp85A);env.f4=0;{struct Cyc_List_List*_tmp85D=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),
struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp85B);struct Cyc_List_List*_tmp133B;struct Cyc_List_List*_tmp85E=(_tmp133B=
_region_malloc(r,sizeof(*_tmp133B)),((_tmp133B->hd=_tmp85C,((_tmp133B->tl=
_tmp85D,_tmp133B)))));return((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp85E);};}_LL43F:;}_LL433:;_LL434:
goto _LL42A;_LL42A:;};}{struct _tuple12*_tmp133E;struct Cyc_List_List*_tmp133D;
return(_tmp133D=_region_malloc(r,sizeof(*_tmp133D)),((_tmp133D->hd=((_tmp133E=
_region_malloc(r,sizeof(*_tmp133E)),((_tmp133E->f1=Cyc_Absyn_empty_tqual(0),((
_tmp133E->f2=t1,_tmp133E)))))),((_tmp133D->tl=0,_tmp133D)))));};}static int Cyc_Tcutil_sub_attributes(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*t=a1;for(0;t
!= 0;t=t->tl){void*_tmp863=(void*)t->hd;_LL445: {struct Cyc_Absyn_Pure_att_struct*
_tmp864=(struct Cyc_Absyn_Pure_att_struct*)_tmp863;if(_tmp864->tag != 22)goto
_LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_Noreturn_att_struct*_tmp865=(
struct Cyc_Absyn_Noreturn_att_struct*)_tmp863;if(_tmp865->tag != 4)goto _LL449;}
_LL448: goto _LL44A;_LL449: {struct Cyc_Absyn_Initializes_att_struct*_tmp866=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp863;if(_tmp866->tag != 20)goto _LL44B;}
_LL44A: continue;_LL44B:;_LL44C: if(!((int(*)(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))
return 0;_LL444:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp133F;struct _tuple0 _tmp868=(_tmp133F.f1=Cyc_Tcutil_compress(t1),((
_tmp133F.f2=Cyc_Tcutil_compress(t2),_tmp133F)));void*_tmp869;enum Cyc_Absyn_Size_of
_tmp86B;void*_tmp86C;enum Cyc_Absyn_Size_of _tmp86E;_LL44E: _tmp869=_tmp868.f1;{
struct Cyc_Absyn_IntType_struct*_tmp86A=(struct Cyc_Absyn_IntType_struct*)_tmp869;
if(_tmp86A->tag != 6)goto _LL450;else{_tmp86B=_tmp86A->f2;}};_tmp86C=_tmp868.f2;{
struct Cyc_Absyn_IntType_struct*_tmp86D=(struct Cyc_Absyn_IntType_struct*)_tmp86C;
if(_tmp86D->tag != 6)goto _LL450;else{_tmp86E=_tmp86D->f2;}};_LL44F: return(_tmp86B
== _tmp86E  || _tmp86B == Cyc_Absyn_Int_sz  && _tmp86E == Cyc_Absyn_Long_sz) || 
_tmp86B == Cyc_Absyn_Long_sz  && _tmp86E == Cyc_Absyn_Int_sz;_LL450:;_LL451: return 0;
_LL44D:;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))return 1;{struct Cyc_List_List*
a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1)
 && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))return 1;}}t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1340;struct _tuple0 _tmp870=(
_tmp1340.f1=t1,((_tmp1340.f2=t2,_tmp1340)));void*_tmp871;struct Cyc_Absyn_PtrInfo
_tmp873;void*_tmp874;struct Cyc_Absyn_Tqual _tmp875;struct Cyc_Absyn_PtrAtts _tmp876;
void*_tmp877;union Cyc_Absyn_Constraint*_tmp878;union Cyc_Absyn_Constraint*_tmp879;
union Cyc_Absyn_Constraint*_tmp87A;void*_tmp87B;struct Cyc_Absyn_PtrInfo _tmp87D;
void*_tmp87E;struct Cyc_Absyn_Tqual _tmp87F;struct Cyc_Absyn_PtrAtts _tmp880;void*
_tmp881;union Cyc_Absyn_Constraint*_tmp882;union Cyc_Absyn_Constraint*_tmp883;
union Cyc_Absyn_Constraint*_tmp884;void*_tmp885;struct Cyc_Absyn_DatatypeFieldInfo
_tmp887;union Cyc_Absyn_DatatypeFieldInfoU _tmp888;struct _tuple3 _tmp889;struct Cyc_Absyn_Datatypedecl*
_tmp88A;struct Cyc_Absyn_Datatypefield*_tmp88B;struct Cyc_List_List*_tmp88C;void*
_tmp88D;struct Cyc_Absyn_DatatypeInfo _tmp88F;union Cyc_Absyn_DatatypeInfoU _tmp890;
struct Cyc_Absyn_Datatypedecl**_tmp891;struct Cyc_Absyn_Datatypedecl*_tmp892;
struct Cyc_List_List*_tmp893;void*_tmp894;struct Cyc_Absyn_FnInfo _tmp896;void*
_tmp897;struct Cyc_Absyn_FnInfo _tmp899;_LL453: _tmp871=_tmp870.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp872=(struct Cyc_Absyn_PointerType_struct*)_tmp871;if(_tmp872->tag != 5)goto
_LL455;else{_tmp873=_tmp872->f1;_tmp874=_tmp873.elt_typ;_tmp875=_tmp873.elt_tq;
_tmp876=_tmp873.ptr_atts;_tmp877=_tmp876.rgn;_tmp878=_tmp876.nullable;_tmp879=
_tmp876.bounds;_tmp87A=_tmp876.zero_term;}};_tmp87B=_tmp870.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp87C=(struct Cyc_Absyn_PointerType_struct*)_tmp87B;if(_tmp87C->tag != 5)goto
_LL455;else{_tmp87D=_tmp87C->f1;_tmp87E=_tmp87D.elt_typ;_tmp87F=_tmp87D.elt_tq;
_tmp880=_tmp87D.ptr_atts;_tmp881=_tmp880.rgn;_tmp882=_tmp880.nullable;_tmp883=
_tmp880.bounds;_tmp884=_tmp880.zero_term;}};_LL454: if(_tmp875.real_const  && !
_tmp87F.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp878,
_tmp882) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp878)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp882))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp87A,
_tmp884) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp87A)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp884))return 0;if(!Cyc_Tcutil_unify(_tmp877,_tmp881) && !Cyc_Tcenv_region_outlives(
te,_tmp877,_tmp881))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp879,_tmp883)){struct _tuple0 _tmp1341;struct _tuple0 _tmp89B=(_tmp1341.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp879),((_tmp1341.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp883),_tmp1341)));void*
_tmp89C;void*_tmp89E;void*_tmp8A0;struct Cyc_Absyn_Exp*_tmp8A2;void*_tmp8A3;
struct Cyc_Absyn_Exp*_tmp8A5;_LL45C: _tmp89C=_tmp89B.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp89D=(struct Cyc_Absyn_Upper_b_struct*)_tmp89C;if(_tmp89D->tag != 1)goto _LL45E;};
_tmp89E=_tmp89B.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp89F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp89E;if(_tmp89F->tag != 0)goto _LL45E;};_LL45D: goto _LL45B;_LL45E: _tmp8A0=
_tmp89B.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8A1=(struct Cyc_Absyn_Upper_b_struct*)
_tmp8A0;if(_tmp8A1->tag != 1)goto _LL460;else{_tmp8A2=_tmp8A1->f1;}};_tmp8A3=
_tmp89B.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp8A4=(struct Cyc_Absyn_Upper_b_struct*)
_tmp8A3;if(_tmp8A4->tag != 1)goto _LL460;else{_tmp8A5=_tmp8A4->f1;}};_LL45F: if(!
Cyc_Evexp_lte_const_exp(_tmp8A5,_tmp8A2))return 0;goto _LL45B;_LL460:;_LL461:
return 0;_LL45B:;}if(!_tmp87F.real_const  && _tmp875.real_const){if(!Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp87E)))return 0;}{int _tmp8A6=((int(*)(int(*
cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Tcutil_unify_it_bounds,_tmp883,Cyc_Absyn_bounds_one_conref) && !((int(*)(int
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp884);struct _tuple0*
_tmp1344;struct Cyc_List_List*_tmp1343;return(_tmp8A6  && Cyc_Tcutil_ptrsubtype(te,((
_tmp1343=_cycalloc(sizeof(*_tmp1343)),((_tmp1343->hd=((_tmp1344=_cycalloc(
sizeof(*_tmp1344)),((_tmp1344->f1=t1,((_tmp1344->f2=t2,_tmp1344)))))),((_tmp1343->tl=
assume,_tmp1343)))))),_tmp874,_tmp87E) || Cyc_Tcutil_unify(t1,t2)) || Cyc_Tcutil_isomorphic(
t1,t2);};_LL455: _tmp885=_tmp870.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp886=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp885;if(_tmp886->tag != 4)
goto _LL457;else{_tmp887=_tmp886->f1;_tmp888=_tmp887.field_info;if((_tmp888.KnownDatatypefield).tag
!= 2)goto _LL457;_tmp889=(struct _tuple3)(_tmp888.KnownDatatypefield).val;_tmp88A=
_tmp889.f1;_tmp88B=_tmp889.f2;_tmp88C=_tmp887.targs;}};_tmp88D=_tmp870.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp88E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp88D;if(_tmp88E->tag != 3)goto _LL457;else{_tmp88F=_tmp88E->f1;_tmp890=_tmp88F.datatype_info;
if((_tmp890.KnownDatatype).tag != 2)goto _LL457;_tmp891=(struct Cyc_Absyn_Datatypedecl**)(
_tmp890.KnownDatatype).val;_tmp892=*_tmp891;_tmp893=_tmp88F.targs;}};_LL456: if(
_tmp88A != _tmp892  && Cyc_Absyn_qvar_cmp(_tmp88A->name,_tmp892->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp88C)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp893))return 0;for(0;_tmp88C != 0;(_tmp88C=
_tmp88C->tl,_tmp893=_tmp893->tl)){if(!Cyc_Tcutil_unify((void*)_tmp88C->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp893))->hd))return 0;}return 1;_LL457: _tmp894=
_tmp870.f1;{struct Cyc_Absyn_FnType_struct*_tmp895=(struct Cyc_Absyn_FnType_struct*)
_tmp894;if(_tmp895->tag != 9)goto _LL459;else{_tmp896=_tmp895->f1;}};_tmp897=
_tmp870.f2;{struct Cyc_Absyn_FnType_struct*_tmp898=(struct Cyc_Absyn_FnType_struct*)
_tmp897;if(_tmp898->tag != 9)goto _LL459;else{_tmp899=_tmp898->f1;}};_LL458: if(
_tmp896.tvars != 0  || _tmp899.tvars != 0){struct Cyc_List_List*_tmp8A9=_tmp896.tvars;
struct Cyc_List_List*_tmp8AA=_tmp899.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8A9)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8AA))return 0;{
struct _RegionHandle _tmp8AB=_new_region("r");struct _RegionHandle*r=& _tmp8AB;
_push_region(r);{struct Cyc_List_List*inst=0;while(_tmp8A9 != 0){if(!Cyc_Tcutil_unify_kindbound(((
struct Cyc_Absyn_Tvar*)_tmp8A9->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp8AA))->hd)->kind)){int _tmp8AC=0;_npop_handler(0);return _tmp8AC;}{
struct _tuple16*_tmp134E;struct Cyc_Absyn_VarType_struct _tmp134D;struct Cyc_Absyn_VarType_struct*
_tmp134C;struct Cyc_List_List*_tmp134B;inst=((_tmp134B=_region_malloc(r,sizeof(*
_tmp134B)),((_tmp134B->hd=((_tmp134E=_region_malloc(r,sizeof(*_tmp134E)),((
_tmp134E->f1=(struct Cyc_Absyn_Tvar*)_tmp8AA->hd,((_tmp134E->f2=(void*)((_tmp134C=
_cycalloc(sizeof(*_tmp134C)),((_tmp134C[0]=((_tmp134D.tag=2,((_tmp134D.f1=(
struct Cyc_Absyn_Tvar*)_tmp8A9->hd,_tmp134D)))),_tmp134C)))),_tmp134E)))))),((
_tmp134B->tl=inst,_tmp134B))))));}_tmp8A9=_tmp8A9->tl;_tmp8AA=_tmp8AA->tl;}if(
inst != 0){_tmp896.tvars=0;_tmp899.tvars=0;{struct Cyc_Absyn_FnType_struct _tmp1354;
struct Cyc_Absyn_FnType_struct*_tmp1353;struct Cyc_Absyn_FnType_struct _tmp1351;
struct Cyc_Absyn_FnType_struct*_tmp1350;int _tmp8B5=Cyc_Tcutil_subtype(te,assume,(
void*)((_tmp1350=_cycalloc(sizeof(*_tmp1350)),((_tmp1350[0]=((_tmp1351.tag=9,((
_tmp1351.f1=_tmp896,_tmp1351)))),_tmp1350)))),(void*)((_tmp1353=_cycalloc(
sizeof(*_tmp1353)),((_tmp1353[0]=((_tmp1354.tag=9,((_tmp1354.f1=_tmp899,_tmp1354)))),
_tmp1353)))));_npop_handler(0);return _tmp8B5;};}};_pop_region(r);};}if(!Cyc_Tcutil_subtype(
te,assume,_tmp896.ret_typ,_tmp899.ret_typ))return 0;{struct Cyc_List_List*_tmp8B6=
_tmp896.args;struct Cyc_List_List*_tmp8B7=_tmp899.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp8B6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp8B7))return 0;for(0;_tmp8B6 != 0;(_tmp8B6=_tmp8B6->tl,_tmp8B7=_tmp8B7->tl)){
struct Cyc_Absyn_Tqual _tmp8B9;void*_tmp8BA;struct _tuple10 _tmp8B8=*((struct
_tuple10*)_tmp8B6->hd);_tmp8B9=_tmp8B8.f2;_tmp8BA=_tmp8B8.f3;{struct Cyc_Absyn_Tqual
_tmp8BC;void*_tmp8BD;struct _tuple10 _tmp8BB=*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp8B7))->hd);_tmp8BC=_tmp8BB.f2;_tmp8BD=_tmp8BB.f3;if(_tmp8BC.real_const
 && !_tmp8B9.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8BD,_tmp8BA))return
0;};}if(_tmp896.c_varargs != _tmp899.c_varargs)return 0;if(_tmp896.cyc_varargs != 0
 && _tmp899.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp8BE=*_tmp896.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp8BF=*_tmp899.cyc_varargs;if((_tmp8BF.tq).real_const
 && !(_tmp8BE.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp8BF.type,
_tmp8BE.type))return 0;}else{if(_tmp896.cyc_varargs != 0  || _tmp899.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp896.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp899.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp896.rgn_po,_tmp899.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp896.attributes,_tmp899.attributes))return 0;return 1;};_LL459:;_LL45A: return 0;
_LL452:;};}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle _tmp8C0=_new_region("temp");struct
_RegionHandle*temp=& _tmp8C0;_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
temp,1,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);for(
0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp8C1=0;
_npop_handler(0);return _tmp8C1;}{struct _tuple12 _tmp8C3;struct Cyc_Absyn_Tqual
_tmp8C4;void*_tmp8C5;struct _tuple12*_tmp8C2=(struct _tuple12*)tqs1->hd;_tmp8C3=*
_tmp8C2;_tmp8C4=_tmp8C3.f1;_tmp8C5=_tmp8C3.f2;{struct _tuple12 _tmp8C7;struct Cyc_Absyn_Tqual
_tmp8C8;void*_tmp8C9;struct _tuple12*_tmp8C6=(struct _tuple12*)tqs2->hd;_tmp8C7=*
_tmp8C6;_tmp8C8=_tmp8C7.f1;_tmp8C9=_tmp8C7.f2;if(_tmp8C4.real_const  && !_tmp8C8.real_const){
int _tmp8CA=0;_npop_handler(0);return _tmp8CA;}if((_tmp8C8.real_const  || Cyc_Tcutil_kind_leq(&
Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8C9))) && Cyc_Tcutil_subtype(te,assume,
_tmp8C5,_tmp8C9))continue;if(Cyc_Tcutil_unify(_tmp8C5,_tmp8C9))continue;if(Cyc_Tcutil_isomorphic(
_tmp8C5,_tmp8C9))continue;{int _tmp8CB=0;_npop_handler(0);return _tmp8CB;};};};}{
int _tmp8CC=1;_npop_handler(0);return _tmp8CC;};};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp8CD=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp8CF;_LL463: {
struct Cyc_Absyn_IntType_struct*_tmp8CE=(struct Cyc_Absyn_IntType_struct*)_tmp8CD;
if(_tmp8CE->tag != 6)goto _LL465;else{_tmp8CF=_tmp8CE->f2;if(_tmp8CF != Cyc_Absyn_Char_sz)
goto _LL465;}}_LL464: return 1;_LL465:;_LL466: return 0;_LL462:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){if(
Cyc_Tcutil_unify(t1,t2))return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{
void*_tmp8D0=t2;enum Cyc_Absyn_Size_of _tmp8D2;enum Cyc_Absyn_Size_of _tmp8D4;
_LL468: {struct Cyc_Absyn_IntType_struct*_tmp8D1=(struct Cyc_Absyn_IntType_struct*)
_tmp8D0;if(_tmp8D1->tag != 6)goto _LL46A;else{_tmp8D2=_tmp8D1->f2;if(_tmp8D2 != Cyc_Absyn_Int_sz)
goto _LL46A;}}_LL469: goto _LL46B;_LL46A: {struct Cyc_Absyn_IntType_struct*_tmp8D3=(
struct Cyc_Absyn_IntType_struct*)_tmp8D0;if(_tmp8D3->tag != 6)goto _LL46C;else{
_tmp8D4=_tmp8D3->f2;if(_tmp8D4 != Cyc_Absyn_Long_sz)goto _LL46C;}}_LL46B: if((Cyc_Tcutil_typ_kind(
t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL467;_LL46C:;
_LL46D: goto _LL467;_LL467:;}{void*_tmp8D5=t1;struct Cyc_Absyn_PtrInfo _tmp8D7;void*
_tmp8D8;struct Cyc_Absyn_Tqual _tmp8D9;struct Cyc_Absyn_PtrAtts _tmp8DA;void*_tmp8DB;
union Cyc_Absyn_Constraint*_tmp8DC;union Cyc_Absyn_Constraint*_tmp8DD;union Cyc_Absyn_Constraint*
_tmp8DE;struct Cyc_Absyn_ArrayInfo _tmp8E0;void*_tmp8E1;struct Cyc_Absyn_Tqual
_tmp8E2;struct Cyc_Absyn_Exp*_tmp8E3;union Cyc_Absyn_Constraint*_tmp8E4;struct Cyc_Absyn_Enumdecl*
_tmp8E6;void*_tmp8EA;_LL46F: {struct Cyc_Absyn_PointerType_struct*_tmp8D6=(struct
Cyc_Absyn_PointerType_struct*)_tmp8D5;if(_tmp8D6->tag != 5)goto _LL471;else{
_tmp8D7=_tmp8D6->f1;_tmp8D8=_tmp8D7.elt_typ;_tmp8D9=_tmp8D7.elt_tq;_tmp8DA=
_tmp8D7.ptr_atts;_tmp8DB=_tmp8DA.rgn;_tmp8DC=_tmp8DA.nullable;_tmp8DD=_tmp8DA.bounds;
_tmp8DE=_tmp8DA.zero_term;}}_LL470:{void*_tmp8EB=t2;struct Cyc_Absyn_PtrInfo
_tmp8ED;void*_tmp8EE;struct Cyc_Absyn_Tqual _tmp8EF;struct Cyc_Absyn_PtrAtts _tmp8F0;
void*_tmp8F1;union Cyc_Absyn_Constraint*_tmp8F2;union Cyc_Absyn_Constraint*_tmp8F3;
union Cyc_Absyn_Constraint*_tmp8F4;_LL47E: {struct Cyc_Absyn_PointerType_struct*
_tmp8EC=(struct Cyc_Absyn_PointerType_struct*)_tmp8EB;if(_tmp8EC->tag != 5)goto
_LL480;else{_tmp8ED=_tmp8EC->f1;_tmp8EE=_tmp8ED.elt_typ;_tmp8EF=_tmp8ED.elt_tq;
_tmp8F0=_tmp8ED.ptr_atts;_tmp8F1=_tmp8F0.rgn;_tmp8F2=_tmp8F0.nullable;_tmp8F3=
_tmp8F0.bounds;_tmp8F4=_tmp8F0.zero_term;}}_LL47F: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp1357;struct Cyc_List_List*
_tmp1356;struct Cyc_List_List*_tmp8F5=(_tmp1356=_cycalloc(sizeof(*_tmp1356)),((
_tmp1356->hd=((_tmp1357=_cycalloc(sizeof(*_tmp1357)),((_tmp1357->f1=t1,((
_tmp1357->f2=t2,_tmp1357)))))),((_tmp1356->tl=0,_tmp1356)))));int _tmp8F6=_tmp8EF.real_const
 || !_tmp8D9.real_const;int _tmp8F7=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp8F3,Cyc_Absyn_bounds_one_conref) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(0,_tmp8F4);int _tmp8F8=_tmp8F6  && (((_tmp8F7  && Cyc_Tcutil_ptrsubtype(
te,_tmp8F5,_tmp8D8,_tmp8EE) || Cyc_Tcutil_unify(_tmp8D8,_tmp8EE)) || Cyc_Tcutil_isomorphic(
_tmp8D8,_tmp8EE)) || Cyc_Tcutil_unify(_tmp8EE,(void*)& Cyc_Absyn_VoidType_val));
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp8DE,_tmp8F4) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp8F4);int _tmp8F9=_tmp8F8?0:((Cyc_Tcutil_bits_only(_tmp8D8) && Cyc_Tcutil_is_char_type(
_tmp8EE)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp8F4)) && (_tmp8EF.real_const  || !_tmp8D9.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp8DD,_tmp8F3);if(!
bounds_ok  && !_tmp8F9){struct _tuple0 _tmp1358;struct _tuple0 _tmp8FB=(_tmp1358.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8DD),((_tmp1358.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp8F3),_tmp1358)));
void*_tmp8FC;struct Cyc_Absyn_Exp*_tmp8FE;void*_tmp8FF;struct Cyc_Absyn_Exp*
_tmp901;_LL483: _tmp8FC=_tmp8FB.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp8FD=(
struct Cyc_Absyn_Upper_b_struct*)_tmp8FC;if(_tmp8FD->tag != 1)goto _LL485;else{
_tmp8FE=_tmp8FD->f1;}};_tmp8FF=_tmp8FB.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp900=(struct Cyc_Absyn_Upper_b_struct*)_tmp8FF;if(_tmp900->tag != 1)goto _LL485;
else{_tmp901=_tmp900->f1;}};_LL484: if(Cyc_Evexp_lte_const_exp(_tmp901,_tmp8FE))
bounds_ok=1;goto _LL482;_LL485:;_LL486: bounds_ok=1;goto _LL482;_LL482:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8DC) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8F2))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp8F8  || _tmp8F9)) && (Cyc_Tcutil_unify(
_tmp8DB,_tmp8F1) || Cyc_Tcenv_region_outlives(te,_tmp8DB,_tmp8F1)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL480:;_LL481: goto _LL47D;
_LL47D:;}return Cyc_Absyn_Unknown_coercion;_LL471: {struct Cyc_Absyn_ArrayType_struct*
_tmp8DF=(struct Cyc_Absyn_ArrayType_struct*)_tmp8D5;if(_tmp8DF->tag != 8)goto
_LL473;else{_tmp8E0=_tmp8DF->f1;_tmp8E1=_tmp8E0.elt_type;_tmp8E2=_tmp8E0.tq;
_tmp8E3=_tmp8E0.num_elts;_tmp8E4=_tmp8E0.zero_term;}}_LL472:{void*_tmp904=t2;
struct Cyc_Absyn_ArrayInfo _tmp906;void*_tmp907;struct Cyc_Absyn_Tqual _tmp908;
struct Cyc_Absyn_Exp*_tmp909;union Cyc_Absyn_Constraint*_tmp90A;_LL488: {struct Cyc_Absyn_ArrayType_struct*
_tmp905=(struct Cyc_Absyn_ArrayType_struct*)_tmp904;if(_tmp905->tag != 8)goto
_LL48A;else{_tmp906=_tmp905->f1;_tmp907=_tmp906.elt_type;_tmp908=_tmp906.tq;
_tmp909=_tmp906.num_elts;_tmp90A=_tmp906.zero_term;}}_LL489: {int okay;okay=((
_tmp8E3 != 0  && _tmp909 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E4,
_tmp90A)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp909,(struct Cyc_Absyn_Exp*)
_tmp8E3);return(okay  && Cyc_Tcutil_unify(_tmp8E1,_tmp907)) && (!_tmp8E2.real_const
 || _tmp908.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL48A:;_LL48B: return Cyc_Absyn_Unknown_coercion;_LL487:;}return Cyc_Absyn_Unknown_coercion;
_LL473: {struct Cyc_Absyn_EnumType_struct*_tmp8E5=(struct Cyc_Absyn_EnumType_struct*)
_tmp8D5;if(_tmp8E5->tag != 13)goto _LL475;else{_tmp8E6=_tmp8E5->f2;}}_LL474:{void*
_tmp90B=t2;struct Cyc_Absyn_Enumdecl*_tmp90D;_LL48D: {struct Cyc_Absyn_EnumType_struct*
_tmp90C=(struct Cyc_Absyn_EnumType_struct*)_tmp90B;if(_tmp90C->tag != 13)goto
_LL48F;else{_tmp90D=_tmp90C->f2;}}_LL48E: if((_tmp8E6->fields != 0  && _tmp90D->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp8E6->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp90D->fields))->v))return Cyc_Absyn_No_coercion;goto _LL48C;_LL48F:;_LL490: goto
_LL48C;_LL48C:;}goto _LL476;_LL475: {struct Cyc_Absyn_IntType_struct*_tmp8E7=(
struct Cyc_Absyn_IntType_struct*)_tmp8D5;if(_tmp8E7->tag != 6)goto _LL477;}_LL476:
goto _LL478;_LL477: {struct Cyc_Absyn_FloatType_struct*_tmp8E8=(struct Cyc_Absyn_FloatType_struct*)
_tmp8D5;if(_tmp8E8->tag != 7)goto _LL479;}_LL478: return Cyc_Tcutil_coerceable(t2)?
Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;_LL479: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp8E9=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp8D5;if(_tmp8E9->tag != 15)goto
_LL47B;else{_tmp8EA=(void*)_tmp8E9->f1;}}_LL47A:{void*_tmp90E=t2;void*_tmp910;
_LL492: {struct Cyc_Absyn_RgnHandleType_struct*_tmp90F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp90E;if(_tmp90F->tag != 15)goto _LL494;else{_tmp910=(void*)_tmp90F->f1;}}_LL493:
if(Cyc_Tcenv_region_outlives(te,_tmp8EA,_tmp910))return Cyc_Absyn_No_coercion;
goto _LL491;_LL494:;_LL495: goto _LL491;_LL491:;}return Cyc_Absyn_Unknown_coercion;
_LL47B:;_LL47C: return Cyc_Absyn_Unknown_coercion;_LL46E:;};}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){
struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct
_tmp135B;struct Cyc_Absyn_Cast_e_struct*_tmp135A;e->r=(void*)((_tmp135A=_cycalloc(
sizeof(*_tmp135A)),((_tmp135A[0]=((_tmp135B.tag=15,((_tmp135B.f1=(void*)t,((
_tmp135B.f2=_tmp911,((_tmp135B.f3=0,((_tmp135B.f4=c,_tmp135B)))))))))),_tmp135A))));}{
struct Cyc_Core_Opt*_tmp135C;e->topt=((_tmp135C=_cycalloc(sizeof(*_tmp135C)),((
_tmp135C->v=t,_tmp135C))));};}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp915=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL497: {struct Cyc_Absyn_IntType_struct*_tmp916=(struct Cyc_Absyn_IntType_struct*)
_tmp915;if(_tmp916->tag != 6)goto _LL499;}_LL498: goto _LL49A;_LL499: {struct Cyc_Absyn_EnumType_struct*
_tmp917=(struct Cyc_Absyn_EnumType_struct*)_tmp915;if(_tmp917->tag != 13)goto
_LL49B;}_LL49A: goto _LL49C;_LL49B: {struct Cyc_Absyn_AnonEnumType_struct*_tmp918=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp915;if(_tmp918->tag != 14)goto _LL49D;}
_LL49C: goto _LL49E;_LL49D: {struct Cyc_Absyn_TagType_struct*_tmp919=(struct Cyc_Absyn_TagType_struct*)
_tmp915;if(_tmp919->tag != 19)goto _LL49F;}_LL49E: return 1;_LL49F: {struct Cyc_Absyn_Evar_struct*
_tmp91A=(struct Cyc_Absyn_Evar_struct*)_tmp915;if(_tmp91A->tag != 1)goto _LL4A1;}
_LL4A0: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL4A1:;_LL4A2: return 0;_LL496:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp91B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL4A4: {struct Cyc_Absyn_FloatType_struct*
_tmp91C=(struct Cyc_Absyn_FloatType_struct*)_tmp91B;if(_tmp91C->tag != 7)goto
_LL4A6;}_LL4A5: return 1;_LL4A6:;_LL4A7: return 0;_LL4A3:;};}int Cyc_Tcutil_is_function_type(
void*t){void*_tmp91D=Cyc_Tcutil_compress(t);_LL4A9: {struct Cyc_Absyn_FnType_struct*
_tmp91E=(struct Cyc_Absyn_FnType_struct*)_tmp91D;if(_tmp91E->tag != 9)goto _LL4AB;}
_LL4AA: return 1;_LL4AB:;_LL4AC: return 0;_LL4A8:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple0 _tmp135D;struct _tuple0 _tmp920=(_tmp135D.f1=t1,((
_tmp135D.f2=t2,_tmp135D)));void*_tmp921;int _tmp923;void*_tmp924;int _tmp926;void*
_tmp927;void*_tmp929;void*_tmp92B;enum Cyc_Absyn_Sign _tmp92D;enum Cyc_Absyn_Size_of
_tmp92E;void*_tmp92F;enum Cyc_Absyn_Sign _tmp931;enum Cyc_Absyn_Size_of _tmp932;
void*_tmp933;enum Cyc_Absyn_Size_of _tmp935;void*_tmp936;enum Cyc_Absyn_Size_of
_tmp938;void*_tmp939;enum Cyc_Absyn_Sign _tmp93B;enum Cyc_Absyn_Size_of _tmp93C;
void*_tmp93D;enum Cyc_Absyn_Sign _tmp93F;enum Cyc_Absyn_Size_of _tmp940;void*
_tmp941;enum Cyc_Absyn_Sign _tmp943;enum Cyc_Absyn_Size_of _tmp944;void*_tmp945;
enum Cyc_Absyn_Sign _tmp947;enum Cyc_Absyn_Size_of _tmp948;void*_tmp949;enum Cyc_Absyn_Size_of
_tmp94B;void*_tmp94C;enum Cyc_Absyn_Size_of _tmp94E;_LL4AE: _tmp921=_tmp920.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp922=(struct Cyc_Absyn_FloatType_struct*)
_tmp921;if(_tmp922->tag != 7)goto _LL4B0;else{_tmp923=_tmp922->f1;}};_tmp924=
_tmp920.f2;{struct Cyc_Absyn_FloatType_struct*_tmp925=(struct Cyc_Absyn_FloatType_struct*)
_tmp924;if(_tmp925->tag != 7)goto _LL4B0;else{_tmp926=_tmp925->f1;}};_LL4AF: if(
_tmp923 != 0  && _tmp923 != 1)return t1;else{if(_tmp926 != 0  && _tmp926 != 1)return t2;
else{if(_tmp923 >= _tmp926)return t1;else{return t2;}}}_LL4B0: _tmp927=_tmp920.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp928=(struct Cyc_Absyn_FloatType_struct*)
_tmp927;if(_tmp928->tag != 7)goto _LL4B2;};_LL4B1: return t1;_LL4B2: _tmp929=_tmp920.f2;{
struct Cyc_Absyn_FloatType_struct*_tmp92A=(struct Cyc_Absyn_FloatType_struct*)
_tmp929;if(_tmp92A->tag != 7)goto _LL4B4;};_LL4B3: return t2;_LL4B4: _tmp92B=_tmp920.f1;{
struct Cyc_Absyn_IntType_struct*_tmp92C=(struct Cyc_Absyn_IntType_struct*)_tmp92B;
if(_tmp92C->tag != 6)goto _LL4B6;else{_tmp92D=_tmp92C->f1;if(_tmp92D != Cyc_Absyn_Unsigned)
goto _LL4B6;_tmp92E=_tmp92C->f2;if(_tmp92E != Cyc_Absyn_LongLong_sz)goto _LL4B6;}};
_LL4B5: goto _LL4B7;_LL4B6: _tmp92F=_tmp920.f2;{struct Cyc_Absyn_IntType_struct*
_tmp930=(struct Cyc_Absyn_IntType_struct*)_tmp92F;if(_tmp930->tag != 6)goto _LL4B8;
else{_tmp931=_tmp930->f1;if(_tmp931 != Cyc_Absyn_Unsigned)goto _LL4B8;_tmp932=
_tmp930->f2;if(_tmp932 != Cyc_Absyn_LongLong_sz)goto _LL4B8;}};_LL4B7: return Cyc_Absyn_ulonglong_typ;
_LL4B8: _tmp933=_tmp920.f1;{struct Cyc_Absyn_IntType_struct*_tmp934=(struct Cyc_Absyn_IntType_struct*)
_tmp933;if(_tmp934->tag != 6)goto _LL4BA;else{_tmp935=_tmp934->f2;if(_tmp935 != Cyc_Absyn_LongLong_sz)
goto _LL4BA;}};_LL4B9: goto _LL4BB;_LL4BA: _tmp936=_tmp920.f2;{struct Cyc_Absyn_IntType_struct*
_tmp937=(struct Cyc_Absyn_IntType_struct*)_tmp936;if(_tmp937->tag != 6)goto _LL4BC;
else{_tmp938=_tmp937->f2;if(_tmp938 != Cyc_Absyn_LongLong_sz)goto _LL4BC;}};_LL4BB:
return Cyc_Absyn_slonglong_typ;_LL4BC: _tmp939=_tmp920.f1;{struct Cyc_Absyn_IntType_struct*
_tmp93A=(struct Cyc_Absyn_IntType_struct*)_tmp939;if(_tmp93A->tag != 6)goto _LL4BE;
else{_tmp93B=_tmp93A->f1;if(_tmp93B != Cyc_Absyn_Unsigned)goto _LL4BE;_tmp93C=
_tmp93A->f2;if(_tmp93C != Cyc_Absyn_Long_sz)goto _LL4BE;}};_LL4BD: goto _LL4BF;
_LL4BE: _tmp93D=_tmp920.f2;{struct Cyc_Absyn_IntType_struct*_tmp93E=(struct Cyc_Absyn_IntType_struct*)
_tmp93D;if(_tmp93E->tag != 6)goto _LL4C0;else{_tmp93F=_tmp93E->f1;if(_tmp93F != Cyc_Absyn_Unsigned)
goto _LL4C0;_tmp940=_tmp93E->f2;if(_tmp940 != Cyc_Absyn_Long_sz)goto _LL4C0;}};
_LL4BF: return Cyc_Absyn_ulong_typ;_LL4C0: _tmp941=_tmp920.f1;{struct Cyc_Absyn_IntType_struct*
_tmp942=(struct Cyc_Absyn_IntType_struct*)_tmp941;if(_tmp942->tag != 6)goto _LL4C2;
else{_tmp943=_tmp942->f1;if(_tmp943 != Cyc_Absyn_Unsigned)goto _LL4C2;_tmp944=
_tmp942->f2;if(_tmp944 != Cyc_Absyn_Int_sz)goto _LL4C2;}};_LL4C1: goto _LL4C3;_LL4C2:
_tmp945=_tmp920.f2;{struct Cyc_Absyn_IntType_struct*_tmp946=(struct Cyc_Absyn_IntType_struct*)
_tmp945;if(_tmp946->tag != 6)goto _LL4C4;else{_tmp947=_tmp946->f1;if(_tmp947 != Cyc_Absyn_Unsigned)
goto _LL4C4;_tmp948=_tmp946->f2;if(_tmp948 != Cyc_Absyn_Int_sz)goto _LL4C4;}};
_LL4C3: return Cyc_Absyn_uint_typ;_LL4C4: _tmp949=_tmp920.f1;{struct Cyc_Absyn_IntType_struct*
_tmp94A=(struct Cyc_Absyn_IntType_struct*)_tmp949;if(_tmp94A->tag != 6)goto _LL4C6;
else{_tmp94B=_tmp94A->f2;if(_tmp94B != Cyc_Absyn_Long_sz)goto _LL4C6;}};_LL4C5:
goto _LL4C7;_LL4C6: _tmp94C=_tmp920.f2;{struct Cyc_Absyn_IntType_struct*_tmp94D=(
struct Cyc_Absyn_IntType_struct*)_tmp94C;if(_tmp94D->tag != 6)goto _LL4C8;else{
_tmp94E=_tmp94D->f2;if(_tmp94E != Cyc_Absyn_Long_sz)goto _LL4C8;}};_LL4C7: return
Cyc_Absyn_slong_typ;_LL4C8:;_LL4C9: return Cyc_Absyn_sint_typ;_LL4AD:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmp94F=e->r;struct Cyc_Core_Opt*_tmp951;_LL4CB: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp950=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp94F;if(_tmp950->tag != 4)goto _LL4CD;else{_tmp951=_tmp950->f2;if(_tmp951 != 0)
goto _LL4CD;}}_LL4CC:{const char*_tmp1360;void*_tmp135F;(_tmp135F=0,Cyc_Tcutil_warn(
e->loc,((_tmp1360="assignment in test",_tag_dyneither(_tmp1360,sizeof(char),19))),
_tag_dyneither(_tmp135F,sizeof(void*),0)));}goto _LL4CA;_LL4CD:;_LL4CE: goto _LL4CA;
_LL4CA:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp1361;struct _tuple0 _tmp955=(
_tmp1361.f1=c1,((_tmp1361.f2=c2,_tmp1361)));void*_tmp956;struct Cyc_Absyn_Kind*
_tmp958;void*_tmp959;struct Cyc_Absyn_Kind*_tmp95B;void*_tmp95C;struct Cyc_Core_Opt*
_tmp95E;struct Cyc_Core_Opt**_tmp95F;void*_tmp960;struct Cyc_Core_Opt*_tmp962;
struct Cyc_Core_Opt**_tmp963;void*_tmp964;struct Cyc_Core_Opt*_tmp966;struct Cyc_Core_Opt**
_tmp967;struct Cyc_Absyn_Kind*_tmp968;void*_tmp969;struct Cyc_Absyn_Kind*_tmp96B;
void*_tmp96C;struct Cyc_Absyn_Kind*_tmp96E;void*_tmp96F;struct Cyc_Core_Opt*
_tmp971;struct Cyc_Core_Opt**_tmp972;struct Cyc_Absyn_Kind*_tmp973;void*_tmp974;
struct Cyc_Core_Opt*_tmp976;struct Cyc_Core_Opt**_tmp977;struct Cyc_Absyn_Kind*
_tmp978;void*_tmp979;struct Cyc_Core_Opt*_tmp97B;struct Cyc_Core_Opt**_tmp97C;
struct Cyc_Absyn_Kind*_tmp97D;_LL4D0: _tmp956=_tmp955.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp957=(struct Cyc_Absyn_Eq_kb_struct*)_tmp956;if(_tmp957->tag != 0)goto _LL4D2;
else{_tmp958=_tmp957->f1;}};_tmp959=_tmp955.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp95A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp959;if(_tmp95A->tag != 0)goto _LL4D2;
else{_tmp95B=_tmp95A->f1;}};_LL4D1: return _tmp958 == _tmp95B;_LL4D2: _tmp95C=
_tmp955.f2;{struct Cyc_Absyn_Unknown_kb_struct*_tmp95D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp95C;if(_tmp95D->tag != 1)goto _LL4D4;else{_tmp95E=_tmp95D->f1;_tmp95F=(struct
Cyc_Core_Opt**)& _tmp95D->f1;}};_LL4D3:{struct Cyc_Core_Opt*_tmp1362;*_tmp95F=((
_tmp1362=_cycalloc(sizeof(*_tmp1362)),((_tmp1362->v=c1,_tmp1362))));}return 1;
_LL4D4: _tmp960=_tmp955.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmp961=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp960;if(_tmp961->tag != 1)goto _LL4D6;else{_tmp962=_tmp961->f1;_tmp963=(struct
Cyc_Core_Opt**)& _tmp961->f1;}};_LL4D5:{struct Cyc_Core_Opt*_tmp1363;*_tmp963=((
_tmp1363=_cycalloc(sizeof(*_tmp1363)),((_tmp1363->v=c2,_tmp1363))));}return 1;
_LL4D6: _tmp964=_tmp955.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp965=(struct Cyc_Absyn_Less_kb_struct*)
_tmp964;if(_tmp965->tag != 2)goto _LL4D8;else{_tmp966=_tmp965->f1;_tmp967=(struct
Cyc_Core_Opt**)& _tmp965->f1;_tmp968=_tmp965->f2;}};_tmp969=_tmp955.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp96A=(struct Cyc_Absyn_Eq_kb_struct*)_tmp969;if(_tmp96A->tag != 0)goto _LL4D8;
else{_tmp96B=_tmp96A->f1;}};_LL4D7: if(Cyc_Tcutil_kind_leq(_tmp96B,_tmp968)){{
struct Cyc_Core_Opt*_tmp1364;*_tmp967=((_tmp1364=_cycalloc(sizeof(*_tmp1364)),((
_tmp1364->v=c2,_tmp1364))));}return 1;}else{return 0;}_LL4D8: _tmp96C=_tmp955.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp96D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp96C;if(
_tmp96D->tag != 0)goto _LL4DA;else{_tmp96E=_tmp96D->f1;}};_tmp96F=_tmp955.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp970=(struct Cyc_Absyn_Less_kb_struct*)_tmp96F;
if(_tmp970->tag != 2)goto _LL4DA;else{_tmp971=_tmp970->f1;_tmp972=(struct Cyc_Core_Opt**)&
_tmp970->f1;_tmp973=_tmp970->f2;}};_LL4D9: if(Cyc_Tcutil_kind_leq(_tmp96E,_tmp973)){{
struct Cyc_Core_Opt*_tmp1365;*_tmp972=((_tmp1365=_cycalloc(sizeof(*_tmp1365)),((
_tmp1365->v=c1,_tmp1365))));}return 1;}else{return 0;}_LL4DA: _tmp974=_tmp955.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp975=(struct Cyc_Absyn_Less_kb_struct*)_tmp974;
if(_tmp975->tag != 2)goto _LL4CF;else{_tmp976=_tmp975->f1;_tmp977=(struct Cyc_Core_Opt**)&
_tmp975->f1;_tmp978=_tmp975->f2;}};_tmp979=_tmp955.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp97A=(struct Cyc_Absyn_Less_kb_struct*)_tmp979;if(_tmp97A->tag != 2)goto _LL4CF;
else{_tmp97B=_tmp97A->f1;_tmp97C=(struct Cyc_Core_Opt**)& _tmp97A->f1;_tmp97D=
_tmp97A->f2;}};_LL4DB: if(Cyc_Tcutil_kind_leq(_tmp978,_tmp97D)){{struct Cyc_Core_Opt*
_tmp1366;*_tmp97C=((_tmp1366=_cycalloc(sizeof(*_tmp1366)),((_tmp1366->v=c1,
_tmp1366))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp97D,_tmp978)){{struct Cyc_Core_Opt*
_tmp1367;*_tmp977=((_tmp1367=_cycalloc(sizeof(*_tmp1367)),((_tmp1367->v=c2,
_tmp1367))));}return 1;}else{return 0;}}_LL4CF:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter ++;}static int Cyc_Tcutil_tvar_counter=
0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp136B;void*_tmp136A[1];struct Cyc_Int_pa_struct _tmp1369;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp1369.tag=1,((_tmp1369.f1=(
unsigned long)i,((_tmp136A[0]=& _tmp1369,Cyc_aprintf(((_tmp136B="#%d",
_tag_dyneither(_tmp136B,sizeof(char),4))),_tag_dyneither(_tmp136A,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp136E;struct Cyc_Absyn_Tvar*_tmp136D;return(
_tmp136D=_cycalloc(sizeof(*_tmp136D)),((_tmp136D->name=((_tmp136E=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp136E[0]=s,_tmp136E)))),((_tmp136D->identity=
- 1,((_tmp136D->kind=k,_tmp136D)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _dyneither_ptr _tmp989=*t->name;return*((const char*)
_check_dyneither_subscript(_tmp989,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){{const char*_tmp1372;void*_tmp1371[1];struct Cyc_String_pa_struct
_tmp1370;(_tmp1370.tag=0,((_tmp1370.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*t->name),((_tmp1371[0]=& _tmp1370,Cyc_printf(((_tmp1372="%s",
_tag_dyneither(_tmp1372,sizeof(char),3))),_tag_dyneither(_tmp1371,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmp1373;struct
_dyneither_ptr _tmp98D=Cyc_strconcat(((_tmp1373="`",_tag_dyneither(_tmp1373,
sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char _tmp1376;char _tmp1375;
struct _dyneither_ptr _tmp1374;(_tmp1374=_dyneither_ptr_plus(_tmp98D,sizeof(char),
1),((_tmp1375=*((char*)_check_dyneither_subscript(_tmp1374,sizeof(char),0)),((
_tmp1376='t',((_get_dyneither_size(_tmp1374,sizeof(char))== 1  && (_tmp1375 == '\000'
 && _tmp1376 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1374.curr)=_tmp1376)))))));}{
struct _dyneither_ptr*_tmp1377;t->name=((_tmp1377=_cycalloc(sizeof(struct
_dyneither_ptr)* 1),((_tmp1377[0]=(struct _dyneither_ptr)_tmp98D,_tmp1377))));};};}
struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static
struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple26*x){struct Cyc_Core_Opt*
_tmp137A;struct _tuple10*_tmp1379;return(_tmp1379=_cycalloc(sizeof(*_tmp1379)),((
_tmp1379->f1=(struct Cyc_Core_Opt*)((_tmp137A=_cycalloc(sizeof(*_tmp137A)),((
_tmp137A->v=(*x).f1,_tmp137A)))),((_tmp1379->f2=(*x).f2,((_tmp1379->f3=(*x).f3,
_tmp1379)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmp995=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct
Cyc_List_List*_tmp137B;_tmp995=((_tmp137B=_cycalloc(sizeof(*_tmp137B)),((
_tmp137B->hd=(void*)atts->hd,((_tmp137B->tl=_tmp995,_tmp137B))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp1381;struct Cyc_Absyn_FnInfo _tmp1380;struct Cyc_Absyn_FnType_struct*_tmp137F;
return(void*)((_tmp137F=_cycalloc(sizeof(*_tmp137F)),((_tmp137F[0]=((_tmp1381.tag=
9,((_tmp1381.f1=((_tmp1380.tvars=fd->tvs,((_tmp1380.effect=fd->effect,((_tmp1380.ret_tqual=
fd->ret_tqual,((_tmp1380.ret_typ=fd->ret_type,((_tmp1380.args=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp1380.c_varargs=fd->c_varargs,((_tmp1380.cyc_varargs=fd->cyc_varargs,((
_tmp1380.rgn_po=fd->rgn_po,((_tmp1380.attributes=_tmp995,_tmp1380)))))))))))))))))),
_tmp1381)))),_tmp137F))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple27{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple27*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){
return(*t).f2;}static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp1382;return(_tmp1382=_cycalloc(sizeof(*_tmp1382)),((_tmp1382->f1=(*
pr).f1,((_tmp1382->f2=t,_tmp1382)))));}struct _tuple28{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple29{struct _tuple28*f1;void*f2;};static
struct _tuple29*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
struct _tuple28*_tmp1385;struct _tuple29*_tmp1384;return(_tmp1384=_region_malloc(
rgn,sizeof(*_tmp1384)),((_tmp1384->f1=((_tmp1385=_region_malloc(rgn,sizeof(*
_tmp1385)),((_tmp1385->f1=(*y).f1,((_tmp1385->f2=(*y).f2,_tmp1385)))))),((
_tmp1384->f2=(*y).f3,_tmp1384)))));}static struct _tuple10*Cyc_Tcutil_substitute_f2(
struct _tuple29*w){struct _tuple28*_tmp99E;void*_tmp99F;struct _tuple29 _tmp99D=*w;
_tmp99E=_tmp99D.f1;_tmp99F=_tmp99D.f2;{struct Cyc_Core_Opt*_tmp9A1;struct Cyc_Absyn_Tqual
_tmp9A2;struct _tuple28 _tmp9A0=*_tmp99E;_tmp9A1=_tmp9A0.f1;_tmp9A2=_tmp9A0.f2;{
struct _tuple10*_tmp1386;return(_tmp1386=_cycalloc(sizeof(*_tmp1386)),((_tmp1386->f1=
_tmp9A1,((_tmp1386->f2=_tmp9A2,((_tmp1386->f3=_tmp99F,_tmp1386)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){return f->type;}
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*
f,void*t){struct Cyc_Absyn_Aggrfield*_tmp1387;return(_tmp1387=_cycalloc(sizeof(*
_tmp1387)),((_tmp1387->name=f->name,((_tmp1387->tq=f->tq,((_tmp1387->type=t,((
_tmp1387->width=f->width,((_tmp1387->attributes=f->attributes,_tmp1387)))))))))));}
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts);static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct
Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp1388;return(_tmp1388=_cycalloc(
sizeof(*_tmp1388)),((_tmp1388->topt=old->topt,((_tmp1388->r=r,((_tmp1388->loc=
old->loc,((_tmp1388->annot=old->annot,_tmp1388)))))))));}static struct Cyc_Absyn_Exp*
Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*
e){void*_tmp9A6=e->r;enum Cyc_Absyn_Primop _tmp9AC;struct Cyc_List_List*_tmp9AD;
struct Cyc_Absyn_Exp*_tmp9AF;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_Absyn_Exp*
_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_Absyn_Exp*
_tmp9B6;struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B9;struct Cyc_Absyn_Exp*
_tmp9BA;void*_tmp9BC;struct Cyc_Absyn_Exp*_tmp9BD;int _tmp9BE;enum Cyc_Absyn_Coercion
_tmp9BF;void*_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C3;void*_tmp9C5;void*_tmp9C6;void*
_tmp9C8;_LL4DD: {struct Cyc_Absyn_Const_e_struct*_tmp9A7=(struct Cyc_Absyn_Const_e_struct*)
_tmp9A6;if(_tmp9A7->tag != 0)goto _LL4DF;}_LL4DE: goto _LL4E0;_LL4DF: {struct Cyc_Absyn_Enum_e_struct*
_tmp9A8=(struct Cyc_Absyn_Enum_e_struct*)_tmp9A6;if(_tmp9A8->tag != 32)goto _LL4E1;}
_LL4E0: goto _LL4E2;_LL4E1: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp9A9=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp9A6;if(_tmp9A9->tag != 33)goto _LL4E3;}_LL4E2: goto _LL4E4;_LL4E3: {struct Cyc_Absyn_Var_e_struct*
_tmp9AA=(struct Cyc_Absyn_Var_e_struct*)_tmp9A6;if(_tmp9AA->tag != 1)goto _LL4E5;}
_LL4E4: return e;_LL4E5: {struct Cyc_Absyn_Primop_e_struct*_tmp9AB=(struct Cyc_Absyn_Primop_e_struct*)
_tmp9A6;if(_tmp9AB->tag != 3)goto _LL4E7;else{_tmp9AC=_tmp9AB->f1;_tmp9AD=_tmp9AB->f2;}}
_LL4E6: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9AD)== 1){struct
Cyc_Absyn_Exp*_tmp9C9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp9AD))->hd;struct Cyc_Absyn_Exp*_tmp9CA=Cyc_Tcutil_rsubsexp(r,inst,_tmp9C9);
if(_tmp9CA == _tmp9C9)return e;{struct Cyc_Absyn_Primop_e_struct _tmp138E;struct Cyc_Absyn_Exp*
_tmp138D[1];struct Cyc_Absyn_Primop_e_struct*_tmp138C;return Cyc_Tcutil_copye(e,(
void*)((_tmp138C=_cycalloc(sizeof(*_tmp138C)),((_tmp138C[0]=((_tmp138E.tag=3,((
_tmp138E.f1=_tmp9AC,((_tmp138E.f2=((_tmp138D[0]=_tmp9CA,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp138D,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp138E)))))),_tmp138C)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp9AD)== 2){struct Cyc_Absyn_Exp*_tmp9CE=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp9AD))->hd;struct Cyc_Absyn_Exp*_tmp9CF=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp9AD->tl))->hd;struct
Cyc_Absyn_Exp*_tmp9D0=Cyc_Tcutil_rsubsexp(r,inst,_tmp9CE);struct Cyc_Absyn_Exp*
_tmp9D1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9CF);if(_tmp9D0 == _tmp9CE  && _tmp9D1 == 
_tmp9CF)return e;{struct Cyc_Absyn_Primop_e_struct _tmp1394;struct Cyc_Absyn_Exp*
_tmp1393[2];struct Cyc_Absyn_Primop_e_struct*_tmp1392;return Cyc_Tcutil_copye(e,(
void*)((_tmp1392=_cycalloc(sizeof(*_tmp1392)),((_tmp1392[0]=((_tmp1394.tag=3,((
_tmp1394.f1=_tmp9AC,((_tmp1394.f2=((_tmp1393[1]=_tmp9D1,((_tmp1393[0]=_tmp9D0,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1393,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp1394)))))),_tmp1392)))));};}
else{const char*_tmp1397;void*_tmp1396;return(_tmp1396=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1397="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp1397,sizeof(char),34))),_tag_dyneither(_tmp1396,sizeof(void*),
0)));}}_LL4E7: {struct Cyc_Absyn_Conditional_e_struct*_tmp9AE=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp9A6;if(_tmp9AE->tag != 6)goto _LL4E9;else{_tmp9AF=_tmp9AE->f1;_tmp9B0=_tmp9AE->f2;
_tmp9B1=_tmp9AE->f3;}}_LL4E8: {struct Cyc_Absyn_Exp*_tmp9D7=Cyc_Tcutil_rsubsexp(r,
inst,_tmp9AF);struct Cyc_Absyn_Exp*_tmp9D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B0);
struct Cyc_Absyn_Exp*_tmp9D9=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B1);if((_tmp9D7 == 
_tmp9AF  && _tmp9D8 == _tmp9B0) && _tmp9D9 == _tmp9B1)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp139A;struct Cyc_Absyn_Conditional_e_struct*_tmp1399;return Cyc_Tcutil_copye(e,(
void*)((_tmp1399=_cycalloc(sizeof(*_tmp1399)),((_tmp1399[0]=((_tmp139A.tag=6,((
_tmp139A.f1=_tmp9D7,((_tmp139A.f2=_tmp9D8,((_tmp139A.f3=_tmp9D9,_tmp139A)))))))),
_tmp1399)))));};}_LL4E9: {struct Cyc_Absyn_And_e_struct*_tmp9B2=(struct Cyc_Absyn_And_e_struct*)
_tmp9A6;if(_tmp9B2->tag != 7)goto _LL4EB;else{_tmp9B3=_tmp9B2->f1;_tmp9B4=_tmp9B2->f2;}}
_LL4EA: {struct Cyc_Absyn_Exp*_tmp9DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B3);struct
Cyc_Absyn_Exp*_tmp9DD=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B4);if(_tmp9DC == _tmp9B3
 && _tmp9DD == _tmp9B4)return e;{struct Cyc_Absyn_And_e_struct _tmp139D;struct Cyc_Absyn_And_e_struct*
_tmp139C;return Cyc_Tcutil_copye(e,(void*)((_tmp139C=_cycalloc(sizeof(*_tmp139C)),((
_tmp139C[0]=((_tmp139D.tag=7,((_tmp139D.f1=_tmp9DC,((_tmp139D.f2=_tmp9DD,
_tmp139D)))))),_tmp139C)))));};}_LL4EB: {struct Cyc_Absyn_Or_e_struct*_tmp9B5=(
struct Cyc_Absyn_Or_e_struct*)_tmp9A6;if(_tmp9B5->tag != 8)goto _LL4ED;else{_tmp9B6=
_tmp9B5->f1;_tmp9B7=_tmp9B5->f2;}}_LL4EC: {struct Cyc_Absyn_Exp*_tmp9E0=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9B6);struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B7);
if(_tmp9E0 == _tmp9B6  && _tmp9E1 == _tmp9B7)return e;{struct Cyc_Absyn_Or_e_struct
_tmp13A0;struct Cyc_Absyn_Or_e_struct*_tmp139F;return Cyc_Tcutil_copye(e,(void*)((
_tmp139F=_cycalloc(sizeof(*_tmp139F)),((_tmp139F[0]=((_tmp13A0.tag=8,((_tmp13A0.f1=
_tmp9E0,((_tmp13A0.f2=_tmp9E1,_tmp13A0)))))),_tmp139F)))));};}_LL4ED: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9B8=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp9A6;if(_tmp9B8->tag != 9)goto _LL4EF;
else{_tmp9B9=_tmp9B8->f1;_tmp9BA=_tmp9B8->f2;}}_LL4EE: {struct Cyc_Absyn_Exp*
_tmp9E4=Cyc_Tcutil_rsubsexp(r,inst,_tmp9B9);struct Cyc_Absyn_Exp*_tmp9E5=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9BA);if(_tmp9E4 == _tmp9B9  && _tmp9E5 == _tmp9BA)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp13A3;struct Cyc_Absyn_SeqExp_e_struct*_tmp13A2;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A2=_cycalloc(sizeof(*_tmp13A2)),((_tmp13A2[0]=((_tmp13A3.tag=9,((_tmp13A3.f1=
_tmp9E4,((_tmp13A3.f2=_tmp9E5,_tmp13A3)))))),_tmp13A2)))));};}_LL4EF: {struct Cyc_Absyn_Cast_e_struct*
_tmp9BB=(struct Cyc_Absyn_Cast_e_struct*)_tmp9A6;if(_tmp9BB->tag != 15)goto _LL4F1;
else{_tmp9BC=(void*)_tmp9BB->f1;_tmp9BD=_tmp9BB->f2;_tmp9BE=_tmp9BB->f3;_tmp9BF=
_tmp9BB->f4;}}_LL4F0: {struct Cyc_Absyn_Exp*_tmp9E8=Cyc_Tcutil_rsubsexp(r,inst,
_tmp9BD);void*_tmp9E9=Cyc_Tcutil_rsubstitute(r,inst,_tmp9BC);if(_tmp9E8 == 
_tmp9BD  && _tmp9E9 == _tmp9BC)return e;{struct Cyc_Absyn_Cast_e_struct _tmp13A6;
struct Cyc_Absyn_Cast_e_struct*_tmp13A5;return Cyc_Tcutil_copye(e,(void*)((
_tmp13A5=_cycalloc(sizeof(*_tmp13A5)),((_tmp13A5[0]=((_tmp13A6.tag=15,((_tmp13A6.f1=(
void*)_tmp9E9,((_tmp13A6.f2=_tmp9E8,((_tmp13A6.f3=_tmp9BE,((_tmp13A6.f4=_tmp9BF,
_tmp13A6)))))))))),_tmp13A5)))));};}_LL4F1: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9C0=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp9A6;if(_tmp9C0->tag != 18)goto
_LL4F3;else{_tmp9C1=(void*)_tmp9C0->f1;}}_LL4F2: {void*_tmp9EC=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9C1);if(_tmp9EC == _tmp9C1)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp13A9;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp13A8;return Cyc_Tcutil_copye(e,(
void*)((_tmp13A8=_cycalloc(sizeof(*_tmp13A8)),((_tmp13A8[0]=((_tmp13A9.tag=18,((
_tmp13A9.f1=(void*)_tmp9EC,_tmp13A9)))),_tmp13A8)))));};}_LL4F3: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp9C2=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp9A6;if(_tmp9C2->tag != 19)goto
_LL4F5;else{_tmp9C3=_tmp9C2->f1;}}_LL4F4: {struct Cyc_Absyn_Exp*_tmp9EF=Cyc_Tcutil_rsubsexp(
r,inst,_tmp9C3);if(_tmp9EF == _tmp9C3)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp13AC;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp13AB;return Cyc_Tcutil_copye(e,(
void*)((_tmp13AB=_cycalloc(sizeof(*_tmp13AB)),((_tmp13AB[0]=((_tmp13AC.tag=19,((
_tmp13AC.f1=_tmp9EF,_tmp13AC)))),_tmp13AB)))));};}_LL4F5: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp9C4=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp9A6;if(_tmp9C4->tag != 20)goto
_LL4F7;else{_tmp9C5=(void*)_tmp9C4->f1;_tmp9C6=(void*)_tmp9C4->f2;}}_LL4F6: {
void*_tmp9F2=Cyc_Tcutil_rsubstitute(r,inst,_tmp9C5);if(_tmp9F2 == _tmp9C5)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp13AF;struct Cyc_Absyn_Offsetof_e_struct*
_tmp13AE;return Cyc_Tcutil_copye(e,(void*)((_tmp13AE=_cycalloc(sizeof(*_tmp13AE)),((
_tmp13AE[0]=((_tmp13AF.tag=20,((_tmp13AF.f1=(void*)_tmp9F2,((_tmp13AF.f2=(void*)
_tmp9C6,_tmp13AF)))))),_tmp13AE)))));};}_LL4F7: {struct Cyc_Absyn_Valueof_e_struct*
_tmp9C7=(struct Cyc_Absyn_Valueof_e_struct*)_tmp9A6;if(_tmp9C7->tag != 39)goto
_LL4F9;else{_tmp9C8=(void*)_tmp9C7->f1;}}_LL4F8: {void*_tmp9F5=Cyc_Tcutil_rsubstitute(
r,inst,_tmp9C8);if(_tmp9F5 == _tmp9C8)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp13B2;struct Cyc_Absyn_Valueof_e_struct*_tmp13B1;return Cyc_Tcutil_copye(e,(
void*)((_tmp13B1=_cycalloc(sizeof(*_tmp13B1)),((_tmp13B1[0]=((_tmp13B2.tag=39,((
_tmp13B2.f1=(void*)_tmp9F5,_tmp13B2)))),_tmp13B1)))));};}_LL4F9:;_LL4FA: {const
char*_tmp13B5;void*_tmp13B4;return(_tmp13B4=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13B5="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp13B5,sizeof(char),46))),_tag_dyneither(_tmp13B4,sizeof(void*),
0)));}_LL4DC:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp9FA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9FC;
struct Cyc_Absyn_AggrInfo _tmp9FE;union Cyc_Absyn_AggrInfoU _tmp9FF;struct Cyc_List_List*
_tmpA00;struct Cyc_Absyn_DatatypeInfo _tmpA02;union Cyc_Absyn_DatatypeInfoU _tmpA03;
struct Cyc_List_List*_tmpA04;struct Cyc_Absyn_DatatypeFieldInfo _tmpA06;union Cyc_Absyn_DatatypeFieldInfoU
_tmpA07;struct Cyc_List_List*_tmpA08;struct _tuple2*_tmpA0A;struct Cyc_List_List*
_tmpA0B;struct Cyc_Absyn_Typedefdecl*_tmpA0C;void**_tmpA0D;struct Cyc_Absyn_ArrayInfo
_tmpA0F;void*_tmpA10;struct Cyc_Absyn_Tqual _tmpA11;struct Cyc_Absyn_Exp*_tmpA12;
union Cyc_Absyn_Constraint*_tmpA13;unsigned int _tmpA14;struct Cyc_Absyn_PtrInfo
_tmpA16;void*_tmpA17;struct Cyc_Absyn_Tqual _tmpA18;struct Cyc_Absyn_PtrAtts _tmpA19;
void*_tmpA1A;union Cyc_Absyn_Constraint*_tmpA1B;union Cyc_Absyn_Constraint*_tmpA1C;
union Cyc_Absyn_Constraint*_tmpA1D;struct Cyc_Absyn_FnInfo _tmpA1F;struct Cyc_List_List*
_tmpA20;struct Cyc_Core_Opt*_tmpA21;struct Cyc_Absyn_Tqual _tmpA22;void*_tmpA23;
struct Cyc_List_List*_tmpA24;int _tmpA25;struct Cyc_Absyn_VarargInfo*_tmpA26;struct
Cyc_List_List*_tmpA27;struct Cyc_List_List*_tmpA28;struct Cyc_List_List*_tmpA2A;
enum Cyc_Absyn_AggrKind _tmpA2C;struct Cyc_List_List*_tmpA2D;struct Cyc_Core_Opt*
_tmpA2F;void*_tmpA31;void*_tmpA33;void*_tmpA34;void*_tmpA36;struct Cyc_Absyn_Exp*
_tmpA38;void*_tmpA42;void*_tmpA44;struct Cyc_List_List*_tmpA46;_LL4FC: {struct Cyc_Absyn_VarType_struct*
_tmp9FB=(struct Cyc_Absyn_VarType_struct*)_tmp9FA;if(_tmp9FB->tag != 2)goto _LL4FE;
else{_tmp9FC=_tmp9FB->f1;}}_LL4FD: {struct _handler_cons _tmpA48;_push_handler(&
_tmpA48);{int _tmpA4A=0;if(setjmp(_tmpA48.handler))_tmpA4A=1;if(!_tmpA4A){{void*
_tmpA4B=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmp9FC);_npop_handler(0);return _tmpA4B;};_pop_handler();}else{void*_tmpA49=(
void*)_exn_thrown;void*_tmpA4D=_tmpA49;_LL533: {struct Cyc_Core_Not_found_struct*
_tmpA4E=(struct Cyc_Core_Not_found_struct*)_tmpA4D;if(_tmpA4E->tag != Cyc_Core_Not_found)
goto _LL535;}_LL534: return t;_LL535:;_LL536:(void)_throw(_tmpA4D);_LL532:;}};}
_LL4FE: {struct Cyc_Absyn_AggrType_struct*_tmp9FD=(struct Cyc_Absyn_AggrType_struct*)
_tmp9FA;if(_tmp9FD->tag != 11)goto _LL500;else{_tmp9FE=_tmp9FD->f1;_tmp9FF=_tmp9FE.aggr_info;
_tmpA00=_tmp9FE.targs;}}_LL4FF: {struct Cyc_List_List*_tmpA4F=Cyc_Tcutil_substs(
rgn,inst,_tmpA00);struct Cyc_Absyn_AggrType_struct _tmp13BB;struct Cyc_Absyn_AggrInfo
_tmp13BA;struct Cyc_Absyn_AggrType_struct*_tmp13B9;return _tmpA4F == _tmpA00?t:(
void*)((_tmp13B9=_cycalloc(sizeof(*_tmp13B9)),((_tmp13B9[0]=((_tmp13BB.tag=11,((
_tmp13BB.f1=((_tmp13BA.aggr_info=_tmp9FF,((_tmp13BA.targs=_tmpA4F,_tmp13BA)))),
_tmp13BB)))),_tmp13B9))));}_LL500: {struct Cyc_Absyn_DatatypeType_struct*_tmpA01=(
struct Cyc_Absyn_DatatypeType_struct*)_tmp9FA;if(_tmpA01->tag != 3)goto _LL502;
else{_tmpA02=_tmpA01->f1;_tmpA03=_tmpA02.datatype_info;_tmpA04=_tmpA02.targs;}}
_LL501: {struct Cyc_List_List*_tmpA53=Cyc_Tcutil_substs(rgn,inst,_tmpA04);struct
Cyc_Absyn_DatatypeType_struct _tmp13C1;struct Cyc_Absyn_DatatypeInfo _tmp13C0;
struct Cyc_Absyn_DatatypeType_struct*_tmp13BF;return _tmpA53 == _tmpA04?t:(void*)((
_tmp13BF=_cycalloc(sizeof(*_tmp13BF)),((_tmp13BF[0]=((_tmp13C1.tag=3,((_tmp13C1.f1=((
_tmp13C0.datatype_info=_tmpA03,((_tmp13C0.targs=_tmpA53,_tmp13C0)))),_tmp13C1)))),
_tmp13BF))));}_LL502: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpA05=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp9FA;if(_tmpA05->tag != 4)goto _LL504;else{
_tmpA06=_tmpA05->f1;_tmpA07=_tmpA06.field_info;_tmpA08=_tmpA06.targs;}}_LL503: {
struct Cyc_List_List*_tmpA57=Cyc_Tcutil_substs(rgn,inst,_tmpA08);struct Cyc_Absyn_DatatypeFieldType_struct
_tmp13C7;struct Cyc_Absyn_DatatypeFieldInfo _tmp13C6;struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13C5;return _tmpA57 == _tmpA08?t:(void*)((_tmp13C5=_cycalloc(sizeof(*_tmp13C5)),((
_tmp13C5[0]=((_tmp13C7.tag=4,((_tmp13C7.f1=((_tmp13C6.field_info=_tmpA07,((
_tmp13C6.targs=_tmpA57,_tmp13C6)))),_tmp13C7)))),_tmp13C5))));}_LL504: {struct
Cyc_Absyn_TypedefType_struct*_tmpA09=(struct Cyc_Absyn_TypedefType_struct*)
_tmp9FA;if(_tmpA09->tag != 17)goto _LL506;else{_tmpA0A=_tmpA09->f1;_tmpA0B=_tmpA09->f2;
_tmpA0C=_tmpA09->f3;_tmpA0D=_tmpA09->f4;}}_LL505: {struct Cyc_List_List*_tmpA5B=
Cyc_Tcutil_substs(rgn,inst,_tmpA0B);struct Cyc_Absyn_TypedefType_struct _tmp13CA;
struct Cyc_Absyn_TypedefType_struct*_tmp13C9;return _tmpA5B == _tmpA0B?t:(void*)((
_tmp13C9=_cycalloc(sizeof(*_tmp13C9)),((_tmp13C9[0]=((_tmp13CA.tag=17,((_tmp13CA.f1=
_tmpA0A,((_tmp13CA.f2=_tmpA5B,((_tmp13CA.f3=_tmpA0C,((_tmp13CA.f4=_tmpA0D,
_tmp13CA)))))))))),_tmp13C9))));}_LL506: {struct Cyc_Absyn_ArrayType_struct*
_tmpA0E=(struct Cyc_Absyn_ArrayType_struct*)_tmp9FA;if(_tmpA0E->tag != 8)goto
_LL508;else{_tmpA0F=_tmpA0E->f1;_tmpA10=_tmpA0F.elt_type;_tmpA11=_tmpA0F.tq;
_tmpA12=_tmpA0F.num_elts;_tmpA13=_tmpA0F.zero_term;_tmpA14=_tmpA0F.zt_loc;}}
_LL507: {void*_tmpA5E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA10);struct Cyc_Absyn_ArrayType_struct
_tmp13D0;struct Cyc_Absyn_ArrayInfo _tmp13CF;struct Cyc_Absyn_ArrayType_struct*
_tmp13CE;return _tmpA5E == _tmpA10?t:(void*)((_tmp13CE=_cycalloc(sizeof(*_tmp13CE)),((
_tmp13CE[0]=((_tmp13D0.tag=8,((_tmp13D0.f1=((_tmp13CF.elt_type=_tmpA5E,((
_tmp13CF.tq=_tmpA11,((_tmp13CF.num_elts=_tmpA12,((_tmp13CF.zero_term=_tmpA13,((
_tmp13CF.zt_loc=_tmpA14,_tmp13CF)))))))))),_tmp13D0)))),_tmp13CE))));}_LL508: {
struct Cyc_Absyn_PointerType_struct*_tmpA15=(struct Cyc_Absyn_PointerType_struct*)
_tmp9FA;if(_tmpA15->tag != 5)goto _LL50A;else{_tmpA16=_tmpA15->f1;_tmpA17=_tmpA16.elt_typ;
_tmpA18=_tmpA16.elt_tq;_tmpA19=_tmpA16.ptr_atts;_tmpA1A=_tmpA19.rgn;_tmpA1B=
_tmpA19.nullable;_tmpA1C=_tmpA19.bounds;_tmpA1D=_tmpA19.zero_term;}}_LL509: {
void*_tmpA62=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA17);void*_tmpA63=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA1A);union Cyc_Absyn_Constraint*_tmpA64=_tmpA1C;{void*_tmpA65=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpA1C);struct Cyc_Absyn_Exp*_tmpA67;_LL538: {struct Cyc_Absyn_Upper_b_struct*
_tmpA66=(struct Cyc_Absyn_Upper_b_struct*)_tmpA65;if(_tmpA66->tag != 1)goto _LL53A;
else{_tmpA67=_tmpA66->f1;}}_LL539: {struct Cyc_Absyn_Exp*_tmpA68=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA67);if(_tmpA68 != _tmpA67){struct Cyc_Absyn_Upper_b_struct _tmp13D3;
struct Cyc_Absyn_Upper_b_struct*_tmp13D2;_tmpA64=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp13D2=_cycalloc(sizeof(*_tmp13D2)),((
_tmp13D2[0]=((_tmp13D3.tag=1,((_tmp13D3.f1=_tmpA68,_tmp13D3)))),_tmp13D2)))));}
goto _LL537;}_LL53A:;_LL53B: goto _LL537;_LL537:;}if((_tmpA62 == _tmpA17  && _tmpA63
== _tmpA1A) && _tmpA64 == _tmpA1C)return t;{struct Cyc_Absyn_PointerType_struct
_tmp13DD;struct Cyc_Absyn_PtrAtts _tmp13DC;struct Cyc_Absyn_PtrInfo _tmp13DB;struct
Cyc_Absyn_PointerType_struct*_tmp13DA;return(void*)((_tmp13DA=_cycalloc(sizeof(*
_tmp13DA)),((_tmp13DA[0]=((_tmp13DD.tag=5,((_tmp13DD.f1=((_tmp13DB.elt_typ=
_tmpA62,((_tmp13DB.elt_tq=_tmpA18,((_tmp13DB.ptr_atts=((_tmp13DC.rgn=_tmpA63,((
_tmp13DC.nullable=_tmpA1B,((_tmp13DC.bounds=_tmpA64,((_tmp13DC.zero_term=_tmpA1D,((
_tmp13DC.ptrloc=0,_tmp13DC)))))))))),_tmp13DB)))))),_tmp13DD)))),_tmp13DA))));};}
_LL50A: {struct Cyc_Absyn_FnType_struct*_tmpA1E=(struct Cyc_Absyn_FnType_struct*)
_tmp9FA;if(_tmpA1E->tag != 9)goto _LL50C;else{_tmpA1F=_tmpA1E->f1;_tmpA20=_tmpA1F.tvars;
_tmpA21=_tmpA1F.effect;_tmpA22=_tmpA1F.ret_tqual;_tmpA23=_tmpA1F.ret_typ;_tmpA24=
_tmpA1F.args;_tmpA25=_tmpA1F.c_varargs;_tmpA26=_tmpA1F.cyc_varargs;_tmpA27=
_tmpA1F.rgn_po;_tmpA28=_tmpA1F.attributes;}}_LL50B:{struct Cyc_List_List*_tmpA6F=
_tmpA20;for(0;_tmpA6F != 0;_tmpA6F=_tmpA6F->tl){struct _tuple16*_tmp13E7;struct Cyc_Absyn_VarType_struct
_tmp13E6;struct Cyc_Absyn_VarType_struct*_tmp13E5;struct Cyc_List_List*_tmp13E4;
inst=((_tmp13E4=_region_malloc(rgn,sizeof(*_tmp13E4)),((_tmp13E4->hd=((_tmp13E7=
_region_malloc(rgn,sizeof(*_tmp13E7)),((_tmp13E7->f1=(struct Cyc_Absyn_Tvar*)
_tmpA6F->hd,((_tmp13E7->f2=(void*)((_tmp13E5=_cycalloc(sizeof(*_tmp13E5)),((
_tmp13E5[0]=((_tmp13E6.tag=2,((_tmp13E6.f1=(struct Cyc_Absyn_Tvar*)_tmpA6F->hd,
_tmp13E6)))),_tmp13E5)))),_tmp13E7)))))),((_tmp13E4->tl=inst,_tmp13E4))))));}}{
struct Cyc_List_List*_tmpA75;struct Cyc_List_List*_tmpA76;struct _tuple1 _tmpA74=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple29*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*
env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,
_tmpA24));_tmpA75=_tmpA74.f1;_tmpA76=_tmpA74.f2;{struct Cyc_List_List*_tmpA77=Cyc_Tcutil_substs(
rgn,inst,_tmpA76);struct Cyc_List_List*_tmpA78=((struct Cyc_List_List*(*)(struct
_tuple10*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA75,_tmpA77));struct Cyc_Core_Opt*
eff2;if(_tmpA21 == 0)eff2=0;else{void*_tmpA79=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmpA21->v);if(_tmpA79 == (void*)_tmpA21->v)eff2=_tmpA21;else{struct Cyc_Core_Opt*
_tmp13E8;eff2=((_tmp13E8=_cycalloc(sizeof(*_tmp13E8)),((_tmp13E8->v=_tmpA79,
_tmp13E8))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpA26 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmpA7C;struct Cyc_Absyn_Tqual _tmpA7D;void*
_tmpA7E;int _tmpA7F;struct Cyc_Absyn_VarargInfo _tmpA7B=*_tmpA26;_tmpA7C=_tmpA7B.name;
_tmpA7D=_tmpA7B.tq;_tmpA7E=_tmpA7B.type;_tmpA7F=_tmpA7B.inject;{void*_tmpA80=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA7E);if(_tmpA80 == _tmpA7E)cyc_varargs2=_tmpA26;else{struct Cyc_Absyn_VarargInfo*
_tmp13E9;cyc_varargs2=((_tmp13E9=_cycalloc(sizeof(*_tmp13E9)),((_tmp13E9->name=
_tmpA7C,((_tmp13E9->tq=_tmpA7D,((_tmp13E9->type=_tmpA80,((_tmp13E9->inject=
_tmpA7F,_tmp13E9))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmpA83;struct Cyc_List_List*_tmpA84;struct _tuple1 _tmpA82=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmpA27);_tmpA83=_tmpA82.f1;_tmpA84=_tmpA82.f2;{struct Cyc_List_List*
_tmpA85=Cyc_Tcutil_substs(rgn,inst,_tmpA83);struct Cyc_List_List*_tmpA86=Cyc_Tcutil_substs(
rgn,inst,_tmpA84);if(_tmpA85 == _tmpA83  && _tmpA86 == _tmpA84)rgn_po2=_tmpA27;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmpA85,_tmpA86);}{struct Cyc_Absyn_FnType_struct _tmp13EF;struct
Cyc_Absyn_FnInfo _tmp13EE;struct Cyc_Absyn_FnType_struct*_tmp13ED;return(void*)((
_tmp13ED=_cycalloc(sizeof(*_tmp13ED)),((_tmp13ED[0]=((_tmp13EF.tag=9,((_tmp13EF.f1=((
_tmp13EE.tvars=_tmpA20,((_tmp13EE.effect=eff2,((_tmp13EE.ret_tqual=_tmpA22,((
_tmp13EE.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA23),((_tmp13EE.args=
_tmpA78,((_tmp13EE.c_varargs=_tmpA25,((_tmp13EE.cyc_varargs=cyc_varargs2,((
_tmp13EE.rgn_po=rgn_po2,((_tmp13EE.attributes=_tmpA28,_tmp13EE)))))))))))))))))),
_tmp13EF)))),_tmp13ED))));};};};};};};_LL50C: {struct Cyc_Absyn_TupleType_struct*
_tmpA29=(struct Cyc_Absyn_TupleType_struct*)_tmp9FA;if(_tmpA29->tag != 10)goto
_LL50E;else{_tmpA2A=_tmpA29->f1;}}_LL50D: {struct Cyc_List_List*_tmpA8A=((struct
Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmpA2A);struct Cyc_List_List*_tmpA8B=
Cyc_Tcutil_substs(rgn,inst,_tmpA8A);if(_tmpA8B == _tmpA8A)return t;{struct Cyc_List_List*
_tmpA8C=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmpA2A,_tmpA8B);struct Cyc_Absyn_TupleType_struct _tmp13F2;struct Cyc_Absyn_TupleType_struct*
_tmp13F1;return(void*)((_tmp13F1=_cycalloc(sizeof(*_tmp13F1)),((_tmp13F1[0]=((
_tmp13F2.tag=10,((_tmp13F2.f1=_tmpA8C,_tmp13F2)))),_tmp13F1))));};}_LL50E: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA2B=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp9FA;if(_tmpA2B->tag != 12)goto _LL510;else{_tmpA2C=_tmpA2B->f1;_tmpA2D=_tmpA2B->f2;}}
_LL50F: {struct Cyc_List_List*_tmpA8F=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_field_type,_tmpA2D);struct Cyc_List_List*_tmpA90=Cyc_Tcutil_substs(
rgn,inst,_tmpA8F);if(_tmpA90 == _tmpA8F)return t;{struct Cyc_List_List*_tmpA91=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpA2D,_tmpA90);struct Cyc_Absyn_AnonAggrType_struct _tmp13F5;struct Cyc_Absyn_AnonAggrType_struct*
_tmp13F4;return(void*)((_tmp13F4=_cycalloc(sizeof(*_tmp13F4)),((_tmp13F4[0]=((
_tmp13F5.tag=12,((_tmp13F5.f1=_tmpA2C,((_tmp13F5.f2=_tmpA91,_tmp13F5)))))),
_tmp13F4))));};}_LL510: {struct Cyc_Absyn_Evar_struct*_tmpA2E=(struct Cyc_Absyn_Evar_struct*)
_tmp9FA;if(_tmpA2E->tag != 1)goto _LL512;else{_tmpA2F=_tmpA2E->f2;}}_LL511: if(
_tmpA2F != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpA2F->v);else{return
t;}_LL512: {struct Cyc_Absyn_RgnHandleType_struct*_tmpA30=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp9FA;if(_tmpA30->tag != 15)goto _LL514;else{_tmpA31=(void*)_tmpA30->f1;}}_LL513: {
void*_tmpA94=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA31);struct Cyc_Absyn_RgnHandleType_struct
_tmp13F8;struct Cyc_Absyn_RgnHandleType_struct*_tmp13F7;return _tmpA94 == _tmpA31?t:(
void*)((_tmp13F7=_cycalloc(sizeof(*_tmp13F7)),((_tmp13F7[0]=((_tmp13F8.tag=15,((
_tmp13F8.f1=(void*)_tmpA94,_tmp13F8)))),_tmp13F7))));}_LL514: {struct Cyc_Absyn_DynRgnType_struct*
_tmpA32=(struct Cyc_Absyn_DynRgnType_struct*)_tmp9FA;if(_tmpA32->tag != 16)goto
_LL516;else{_tmpA33=(void*)_tmpA32->f1;_tmpA34=(void*)_tmpA32->f2;}}_LL515: {
void*_tmpA97=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA33);void*_tmpA98=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA34);struct Cyc_Absyn_DynRgnType_struct _tmp13FB;struct Cyc_Absyn_DynRgnType_struct*
_tmp13FA;return _tmpA97 == _tmpA33  && _tmpA98 == _tmpA34?t:(void*)((_tmp13FA=
_cycalloc(sizeof(*_tmp13FA)),((_tmp13FA[0]=((_tmp13FB.tag=16,((_tmp13FB.f1=(void*)
_tmpA97,((_tmp13FB.f2=(void*)_tmpA98,_tmp13FB)))))),_tmp13FA))));}_LL516: {
struct Cyc_Absyn_TagType_struct*_tmpA35=(struct Cyc_Absyn_TagType_struct*)_tmp9FA;
if(_tmpA35->tag != 19)goto _LL518;else{_tmpA36=(void*)_tmpA35->f1;}}_LL517: {void*
_tmpA9B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA36);struct Cyc_Absyn_TagType_struct
_tmp13FE;struct Cyc_Absyn_TagType_struct*_tmp13FD;return _tmpA9B == _tmpA36?t:(void*)((
_tmp13FD=_cycalloc(sizeof(*_tmp13FD)),((_tmp13FD[0]=((_tmp13FE.tag=19,((_tmp13FE.f1=(
void*)_tmpA9B,_tmp13FE)))),_tmp13FD))));}_LL518: {struct Cyc_Absyn_ValueofType_struct*
_tmpA37=(struct Cyc_Absyn_ValueofType_struct*)_tmp9FA;if(_tmpA37->tag != 18)goto
_LL51A;else{_tmpA38=_tmpA37->f1;}}_LL519: {struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmpA38);struct Cyc_Absyn_ValueofType_struct _tmp1401;struct Cyc_Absyn_ValueofType_struct*
_tmp1400;return _tmpA9E == _tmpA38?t:(void*)((_tmp1400=_cycalloc(sizeof(*_tmp1400)),((
_tmp1400[0]=((_tmp1401.tag=18,((_tmp1401.f1=_tmpA9E,_tmp1401)))),_tmp1400))));}
_LL51A: {struct Cyc_Absyn_EnumType_struct*_tmpA39=(struct Cyc_Absyn_EnumType_struct*)
_tmp9FA;if(_tmpA39->tag != 13)goto _LL51C;}_LL51B: goto _LL51D;_LL51C: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpA3A=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp9FA;if(_tmpA3A->tag != 14)goto
_LL51E;}_LL51D: goto _LL51F;_LL51E: {struct Cyc_Absyn_VoidType_struct*_tmpA3B=(
struct Cyc_Absyn_VoidType_struct*)_tmp9FA;if(_tmpA3B->tag != 0)goto _LL520;}_LL51F:
goto _LL521;_LL520: {struct Cyc_Absyn_IntType_struct*_tmpA3C=(struct Cyc_Absyn_IntType_struct*)
_tmp9FA;if(_tmpA3C->tag != 6)goto _LL522;}_LL521: goto _LL523;_LL522: {struct Cyc_Absyn_FloatType_struct*
_tmpA3D=(struct Cyc_Absyn_FloatType_struct*)_tmp9FA;if(_tmpA3D->tag != 7)goto
_LL524;}_LL523: goto _LL525;_LL524: {struct Cyc_Absyn_RefCntRgn_struct*_tmpA3E=(
struct Cyc_Absyn_RefCntRgn_struct*)_tmp9FA;if(_tmpA3E->tag != 22)goto _LL526;}
_LL525: goto _LL527;_LL526: {struct Cyc_Absyn_UniqueRgn_struct*_tmpA3F=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp9FA;if(_tmpA3F->tag != 21)goto _LL528;}_LL527: goto _LL529;_LL528: {struct Cyc_Absyn_HeapRgn_struct*
_tmpA40=(struct Cyc_Absyn_HeapRgn_struct*)_tmp9FA;if(_tmpA40->tag != 20)goto _LL52A;}
_LL529: return t;_LL52A: {struct Cyc_Absyn_RgnsEff_struct*_tmpA41=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp9FA;if(_tmpA41->tag != 25)goto _LL52C;else{_tmpA42=(void*)_tmpA41->f1;}}_LL52B: {
void*_tmpAA1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpA42);struct Cyc_Absyn_RgnsEff_struct
_tmp1404;struct Cyc_Absyn_RgnsEff_struct*_tmp1403;return _tmpAA1 == _tmpA42?t:(void*)((
_tmp1403=_cycalloc(sizeof(*_tmp1403)),((_tmp1403[0]=((_tmp1404.tag=25,((_tmp1404.f1=(
void*)_tmpAA1,_tmp1404)))),_tmp1403))));}_LL52C: {struct Cyc_Absyn_AccessEff_struct*
_tmpA43=(struct Cyc_Absyn_AccessEff_struct*)_tmp9FA;if(_tmpA43->tag != 23)goto
_LL52E;else{_tmpA44=(void*)_tmpA43->f1;}}_LL52D: {void*_tmpAA4=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpA44);struct Cyc_Absyn_AccessEff_struct _tmp1407;struct Cyc_Absyn_AccessEff_struct*
_tmp1406;return _tmpAA4 == _tmpA44?t:(void*)((_tmp1406=_cycalloc(sizeof(*_tmp1406)),((
_tmp1406[0]=((_tmp1407.tag=23,((_tmp1407.f1=(void*)_tmpAA4,_tmp1407)))),_tmp1406))));}
_LL52E: {struct Cyc_Absyn_JoinEff_struct*_tmpA45=(struct Cyc_Absyn_JoinEff_struct*)
_tmp9FA;if(_tmpA45->tag != 24)goto _LL530;else{_tmpA46=_tmpA45->f1;}}_LL52F: {
struct Cyc_List_List*_tmpAA7=Cyc_Tcutil_substs(rgn,inst,_tmpA46);struct Cyc_Absyn_JoinEff_struct
_tmp140A;struct Cyc_Absyn_JoinEff_struct*_tmp1409;return _tmpAA7 == _tmpA46?t:(void*)((
_tmp1409=_cycalloc(sizeof(*_tmp1409)),((_tmp1409[0]=((_tmp140A.tag=24,((_tmp140A.f1=
_tmpAA7,_tmp140A)))),_tmp1409))));}_LL530: {struct Cyc_Absyn_TypeDeclType_struct*
_tmpA47=(struct Cyc_Absyn_TypeDeclType_struct*)_tmp9FA;if(_tmpA47->tag != 26)goto
_LL4FB;}_LL531: {const char*_tmp140D;void*_tmp140C;(_tmp140C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp140D="found typedecltype in rsubs",
_tag_dyneither(_tmp140D,sizeof(char),28))),_tag_dyneither(_tmp140C,sizeof(void*),
0)));}_LL4FB:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*
rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*
_tmpAAC=(void*)ts->hd;struct Cyc_List_List*_tmpAAD=ts->tl;void*_tmpAAE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpAAC);struct Cyc_List_List*_tmpAAF=Cyc_Tcutil_substs(rgn,inst,_tmpAAD);
if(_tmpAAC == _tmpAAE  && _tmpAAD == _tmpAAF)return ts;{struct Cyc_List_List*_tmp140E;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp140E=_cycalloc(sizeof(*
_tmp140E)),((_tmp140E->hd=_tmpAAE,((_tmp140E->tl=_tmpAAF,_tmp140E)))))));};};}
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(
Cyc_Core_heap_region,inst,t);}struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*
s,struct Cyc_Absyn_Tvar*tv){struct Cyc_Core_Opt*_tmpAB1=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(
tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1411;struct _tuple16*_tmp1410;return(
_tmp1410=_cycalloc(sizeof(*_tmp1410)),((_tmp1410->f1=tv,((_tmp1410->f2=Cyc_Absyn_new_evar(
_tmpAB1,((_tmp1411=_cycalloc(sizeof(*_tmp1411)),((_tmp1411->v=s,_tmp1411))))),
_tmp1410)))));}struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,
struct Cyc_Absyn_Tvar*tv){struct _tuple17 _tmpAB5;struct Cyc_List_List*_tmpAB6;
struct _RegionHandle*_tmpAB7;struct _tuple17*_tmpAB4=env;_tmpAB5=*_tmpAB4;_tmpAB6=
_tmpAB5.f1;_tmpAB7=_tmpAB5.f2;{struct Cyc_Core_Opt*_tmpAB8=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*_tmp1414;struct
_tuple16*_tmp1413;return(_tmp1413=_region_malloc(_tmpAB7,sizeof(*_tmp1413)),((
_tmp1413->f1=tv,((_tmp1413->f2=Cyc_Absyn_new_evar(_tmpAB8,((_tmp1414=_cycalloc(
sizeof(*_tmp1414)),((_tmp1414->v=_tmpAB6,_tmp1414))))),_tmp1413)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=
tvs2->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2)){const char*_tmp141A;void*_tmp1419[3];struct Cyc_String_pa_struct _tmp1418;
struct Cyc_String_pa_struct _tmp1417;struct Cyc_String_pa_struct _tmp1416;(_tmp1416.tag=
0,((_tmp1416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k2)),((_tmp1417.tag=0,((_tmp1417.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k1)),((_tmp1418.tag=0,((_tmp1418.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp1419[0]=& _tmp1418,((
_tmp1419[1]=& _tmp1417,((_tmp1419[2]=& _tmp1416,Cyc_Tcutil_terr(loc,((_tmp141A="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp141A,sizeof(char),59))),_tag_dyneither(_tmp1419,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp141D;void*_tmp141C;(_tmp141C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141D="same type variable has different identity!",
_tag_dyneither(_tmp141D,sizeof(char),43))),_tag_dyneither(_tmp141C,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp141E;return(_tmp141E=_cycalloc(sizeof(*_tmp141E)),((_tmp141E->hd=tv,((
_tmp141E->tl=tvs,_tmp141E)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1421;void*_tmp1420;(_tmp1420=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1421="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp1421,sizeof(char),39))),_tag_dyneither(_tmp1420,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmpAC5=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmpAC5->identity == - 1){const char*
_tmp1424;void*_tmp1423;(_tmp1423=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1424="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1424,sizeof(char),41))),_tag_dyneither(_tmp1423,sizeof(void*),
0)));}if(_tmpAC5->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1425;return(_tmp1425=_cycalloc(sizeof(*_tmp1425)),((_tmp1425->hd=tv,((
_tmp1425->tl=tvs,_tmp1425)))));};}struct _tuple30{struct Cyc_Absyn_Tvar*f1;int f2;};
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1428;void*_tmp1427;(_tmp1427=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1428="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1428,sizeof(char),44))),_tag_dyneither(_tmp1427,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple30
_tmpACC;struct Cyc_Absyn_Tvar*_tmpACD;int _tmpACE;int*_tmpACF;struct _tuple30*
_tmpACB=(struct _tuple30*)tvs2->hd;_tmpACC=*_tmpACB;_tmpACD=_tmpACC.f1;_tmpACE=
_tmpACC.f2;_tmpACF=(int*)&(*_tmpACB).f2;if(_tmpACD->identity == - 1){const char*
_tmp142B;void*_tmp142A;(_tmp142A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp142B="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp142B,sizeof(char),46))),_tag_dyneither(_tmp142A,sizeof(void*),
0)));}if(_tmpACD->identity == tv->identity){*_tmpACF=*_tmpACF  || b;return tvs;}}}{
struct _tuple30*_tmp142E;struct Cyc_List_List*_tmp142D;return(_tmp142D=
_region_malloc(r,sizeof(*_tmp142D)),((_tmp142D->hd=((_tmp142E=_region_malloc(r,
sizeof(*_tmp142E)),((_tmp142E->f1=tv,((_tmp142E->f2=b,_tmp142E)))))),((_tmp142D->tl=
tvs,_tmp142D)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1432;void*_tmp1431[1];struct Cyc_String_pa_struct _tmp1430;(_tmp1430.tag=0,((
_tmp1430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp1431[0]=& _tmp1430,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1432="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1432,sizeof(char),29))),_tag_dyneither(_tmp1431,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1433;return(_tmp1433=_cycalloc(sizeof(*
_tmp1433)),((_tmp1433->hd=tv,((_tmp1433->tl=tvs,_tmp1433)))));};}struct _tuple31{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b){void*_tmpAD8=Cyc_Tcutil_compress(
e);int _tmpADA;_LL53D: {struct Cyc_Absyn_Evar_struct*_tmpAD9=(struct Cyc_Absyn_Evar_struct*)
_tmpAD8;if(_tmpAD9->tag != 1)goto _LL53F;else{_tmpADA=_tmpAD9->f3;}}_LL53E:{struct
Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){struct _tuple31 _tmpADC;void*
_tmpADD;int _tmpADE;int*_tmpADF;struct _tuple31*_tmpADB=(struct _tuple31*)es2->hd;
_tmpADC=*_tmpADB;_tmpADD=_tmpADC.f1;_tmpADE=_tmpADC.f2;_tmpADF=(int*)&(*_tmpADB).f2;{
void*_tmpAE0=Cyc_Tcutil_compress(_tmpADD);int _tmpAE2;_LL542: {struct Cyc_Absyn_Evar_struct*
_tmpAE1=(struct Cyc_Absyn_Evar_struct*)_tmpAE0;if(_tmpAE1->tag != 1)goto _LL544;
else{_tmpAE2=_tmpAE1->f3;}}_LL543: if(_tmpADA == _tmpAE2){if(b != *_tmpADF)*_tmpADF=
1;return es;}goto _LL541;_LL544:;_LL545: goto _LL541;_LL541:;};}}{struct _tuple31*
_tmp1436;struct Cyc_List_List*_tmp1435;return(_tmp1435=_region_malloc(r,sizeof(*
_tmp1435)),((_tmp1435->hd=((_tmp1436=_region_malloc(r,sizeof(*_tmp1436)),((
_tmp1436->f1=e,((_tmp1436->f2=b,_tmp1436)))))),((_tmp1435->tl=es,_tmp1435)))));};
_LL53F:;_LL540: return es;_LL53C:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1437;r=((_tmp1437=_region_malloc(rgn,sizeof(*_tmp1437)),((
_tmp1437->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1437->tl=r,_tmp1437))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpAE7;int _tmpAE8;struct
_tuple30 _tmpAE6=*((struct _tuple30*)tvs->hd);_tmpAE7=_tmpAE6.f1;_tmpAE8=_tmpAE6.f2;{
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(_tmpAE7->identity
== ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){
struct Cyc_List_List*_tmp1438;res=((_tmp1438=_region_malloc(r,sizeof(*_tmp1438)),((
_tmp1438->hd=(struct _tuple30*)tvs->hd,((_tmp1438->tl=res,_tmp1438))))));}};}res=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);return res;}
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*
field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){if(width != 0){
unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)width)){
const char*_tmp143C;void*_tmp143B[1];struct Cyc_String_pa_struct _tmp143A;(_tmp143A.tag=
0,((_tmp143A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp143B[0]=&
_tmp143A,Cyc_Tcutil_terr(loc,((_tmp143C="bitfield %s does not have constant width",
_tag_dyneither(_tmp143C,sizeof(char),41))),_tag_dyneither(_tmp143B,sizeof(void*),
1)))))));}else{unsigned int _tmpAEE;int _tmpAEF;struct _tuple14 _tmpAED=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmpAEE=_tmpAED.f1;_tmpAEF=_tmpAED.f2;if(!_tmpAEF){
const char*_tmp143F;void*_tmp143E;(_tmp143E=0,Cyc_Tcutil_terr(loc,((_tmp143F="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp143F,sizeof(char),45))),_tag_dyneither(_tmp143E,sizeof(void*),
0)));}w=_tmpAEE;}{void*_tmpAF2=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmpAF4;_LL547: {struct Cyc_Absyn_IntType_struct*_tmpAF3=(struct Cyc_Absyn_IntType_struct*)
_tmpAF2;if(_tmpAF3->tag != 6)goto _LL549;else{_tmpAF4=_tmpAF3->f2;}}_LL548: switch(
_tmpAF4){case Cyc_Absyn_Char_sz: _LL54B: if(w > 8){const char*_tmp1442;void*_tmp1441;(
_tmp1441=0,Cyc_Tcutil_terr(loc,((_tmp1442="bitfield larger than type",
_tag_dyneither(_tmp1442,sizeof(char),26))),_tag_dyneither(_tmp1441,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL54C: if(w > 16){const char*_tmp1445;void*
_tmp1444;(_tmp1444=0,Cyc_Tcutil_terr(loc,((_tmp1445="bitfield larger than type",
_tag_dyneither(_tmp1445,sizeof(char),26))),_tag_dyneither(_tmp1444,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL54D: goto _LL54E;case Cyc_Absyn_Int_sz: _LL54E:
if(w > 32){const char*_tmp1448;void*_tmp1447;(_tmp1447=0,Cyc_Tcutil_terr(loc,((
_tmp1448="bitfield larger than type",_tag_dyneither(_tmp1448,sizeof(char),26))),
_tag_dyneither(_tmp1447,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL54F: if(w > 64){const char*_tmp144B;void*_tmp144A;(_tmp144A=0,Cyc_Tcutil_terr(
loc,((_tmp144B="bitfield larger than type",_tag_dyneither(_tmp144B,sizeof(char),
26))),_tag_dyneither(_tmp144A,sizeof(void*),0)));}break;}goto _LL546;_LL549:;
_LL54A:{const char*_tmp1450;void*_tmp144F[2];struct Cyc_String_pa_struct _tmp144E;
struct Cyc_String_pa_struct _tmp144D;(_tmp144D.tag=0,((_tmp144D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp144E.tag=0,((_tmp144E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp144F[0]=& _tmp144E,((_tmp144F[1]=& _tmp144D,Cyc_Tcutil_terr(loc,((_tmp1450="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1450,sizeof(char),52))),_tag_dyneither(_tmp144F,sizeof(void*),
2)))))))))))));}goto _LL546;_LL546:;};}}static void Cyc_Tcutil_check_field_atts(
unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmpB01=(void*)atts->hd;_LL552: {struct Cyc_Absyn_Packed_att_struct*
_tmpB02=(struct Cyc_Absyn_Packed_att_struct*)_tmpB01;if(_tmpB02->tag != 7)goto
_LL554;}_LL553: continue;_LL554: {struct Cyc_Absyn_Aligned_att_struct*_tmpB03=(
struct Cyc_Absyn_Aligned_att_struct*)_tmpB01;if(_tmpB03->tag != 6)goto _LL556;}
_LL555: continue;_LL556:;_LL557: {const char*_tmp1455;void*_tmp1454[2];struct Cyc_String_pa_struct
_tmp1453;struct Cyc_String_pa_struct _tmp1452;(_tmp1452.tag=0,((_tmp1452.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1453.tag=0,((_tmp1453.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)
atts->hd)),((_tmp1454[0]=& _tmp1453,((_tmp1454[1]=& _tmp1452,Cyc_Tcutil_terr(loc,((
_tmp1455="bad attribute %s on member %s",_tag_dyneither(_tmp1455,sizeof(char),30))),
_tag_dyneither(_tmp1454,sizeof(void*),2)))))))))))));}_LL551:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};typedef struct
Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;int Cyc_Tcutil_extract_const_from_typedef(
unsigned int loc,int declared_const,void*t){void*_tmpB08=t;struct Cyc_Absyn_Typedefdecl*
_tmpB0A;void**_tmpB0B;_LL559: {struct Cyc_Absyn_TypedefType_struct*_tmpB09=(
struct Cyc_Absyn_TypedefType_struct*)_tmpB08;if(_tmpB09->tag != 17)goto _LL55B;
else{_tmpB0A=_tmpB09->f3;_tmpB0B=_tmpB09->f4;}}_LL55A: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmpB0A))->tq).real_const  || (_tmpB0A->tq).print_const){if(
declared_const){const char*_tmp1458;void*_tmp1457;(_tmp1457=0,Cyc_Tcutil_warn(loc,((
_tmp1458="extra const",_tag_dyneither(_tmp1458,sizeof(char),12))),_tag_dyneither(
_tmp1457,sizeof(void*),0)));}return 1;}if((unsigned int)_tmpB0B)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmpB0B);else{return declared_const;}_LL55B:;_LL55C: return
declared_const;_LL558:;}static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Typedefdecl*td){if(td != 0){if(td->defn != 0){void*_tmpB0E=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);
struct Cyc_Absyn_PtrInfo _tmpB10;struct Cyc_Absyn_PtrAtts _tmpB11;void*_tmpB12;
_LL55E: {struct Cyc_Absyn_PointerType_struct*_tmpB0F=(struct Cyc_Absyn_PointerType_struct*)
_tmpB0E;if(_tmpB0F->tag != 5)goto _LL560;else{_tmpB10=_tmpB0F->f1;_tmpB11=_tmpB10.ptr_atts;
_tmpB12=_tmpB11.rgn;}}_LL55F:{void*_tmpB13=Cyc_Tcutil_compress(_tmpB12);struct
Cyc_Absyn_Tvar*_tmpB15;_LL563: {struct Cyc_Absyn_VarType_struct*_tmpB14=(struct
Cyc_Absyn_VarType_struct*)_tmpB13;if(_tmpB14->tag != 2)goto _LL565;else{_tmpB15=
_tmpB14->f1;}}_LL564: return Cyc_Absyn_tvar_cmp(tvar,_tmpB15)== 0;_LL565:;_LL566:
goto _LL562;_LL562:;}goto _LL55D;_LL560:;_LL561: goto _LL55D;_LL55D:;}}else{return 1;}
return 0;}static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*
tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*
td){void*_tmpB16=Cyc_Absyn_compress_kb(tvar->kind);struct Cyc_Absyn_Kind*_tmpB18;
struct Cyc_Absyn_Kind _tmpB19;enum Cyc_Absyn_KindQual _tmpB1A;enum Cyc_Absyn_AliasQual
_tmpB1B;struct Cyc_Absyn_Kind*_tmpB1D;struct Cyc_Absyn_Kind _tmpB1E;enum Cyc_Absyn_KindQual
_tmpB1F;enum Cyc_Absyn_AliasQual _tmpB20;_LL568: {struct Cyc_Absyn_Less_kb_struct*
_tmpB17=(struct Cyc_Absyn_Less_kb_struct*)_tmpB16;if(_tmpB17->tag != 2)goto _LL56A;
else{_tmpB18=_tmpB17->f2;_tmpB19=*_tmpB18;_tmpB1A=_tmpB19.kind;if(_tmpB1A != Cyc_Absyn_RgnKind)
goto _LL56A;_tmpB1B=_tmpB19.aliasqual;if(_tmpB1B != Cyc_Absyn_Top)goto _LL56A;}}
_LL569: goto _LL56B;_LL56A: {struct Cyc_Absyn_Eq_kb_struct*_tmpB1C=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB16;if(_tmpB1C->tag != 0)goto _LL56C;else{_tmpB1D=_tmpB1C->f1;_tmpB1E=*_tmpB1D;
_tmpB1F=_tmpB1E.kind;if(_tmpB1F != Cyc_Absyn_RgnKind)goto _LL56C;_tmpB20=_tmpB1E.aliasqual;
if(_tmpB20 != Cyc_Absyn_Top)goto _LL56C;}}_LL56B: if(((expected_kind->kind == Cyc_Absyn_BoxKind
 || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind)
 && Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){if(expected_kind->aliasqual == 
Cyc_Absyn_Aliasable)return& Cyc_Tcutil_rk;else{if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}return& Cyc_Tcutil_trk;_LL56C:;_LL56D: return Cyc_Tcutil_tvar_kind(
tvar,def_kind);_LL567:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple32{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){static struct
Cyc_Core_Opt urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt
hrgn={(void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmpB21=Cyc_Tcutil_compress(
t);struct Cyc_Core_Opt*_tmpB24;struct Cyc_Core_Opt**_tmpB25;struct Cyc_Core_Opt*
_tmpB26;struct Cyc_Core_Opt**_tmpB27;struct Cyc_Absyn_Tvar*_tmpB29;struct Cyc_Absyn_TypeDecl*
_tmpB2B;struct Cyc_Absyn_TypeDecl _tmpB2C;void*_tmpB2D;void**_tmpB2E;void***
_tmpB2F;struct Cyc_List_List*_tmpB31;struct _tuple2*_tmpB33;struct Cyc_Absyn_Enumdecl*
_tmpB34;struct Cyc_Absyn_Enumdecl**_tmpB35;struct Cyc_Absyn_DatatypeInfo _tmpB37;
union Cyc_Absyn_DatatypeInfoU _tmpB38;union Cyc_Absyn_DatatypeInfoU*_tmpB39;struct
Cyc_List_List*_tmpB3A;struct Cyc_List_List**_tmpB3B;struct Cyc_Absyn_DatatypeFieldInfo
_tmpB3D;union Cyc_Absyn_DatatypeFieldInfoU _tmpB3E;union Cyc_Absyn_DatatypeFieldInfoU*
_tmpB3F;struct Cyc_List_List*_tmpB40;struct Cyc_Absyn_PtrInfo _tmpB42;void*_tmpB43;
struct Cyc_Absyn_Tqual _tmpB44;struct Cyc_Absyn_Tqual*_tmpB45;struct Cyc_Absyn_PtrAtts
_tmpB46;void*_tmpB47;union Cyc_Absyn_Constraint*_tmpB48;union Cyc_Absyn_Constraint*
_tmpB49;union Cyc_Absyn_Constraint*_tmpB4A;void*_tmpB4C;struct Cyc_Absyn_Exp*
_tmpB4E;struct Cyc_Absyn_ArrayInfo _tmpB52;void*_tmpB53;struct Cyc_Absyn_Tqual
_tmpB54;struct Cyc_Absyn_Tqual*_tmpB55;struct Cyc_Absyn_Exp*_tmpB56;struct Cyc_Absyn_Exp**
_tmpB57;union Cyc_Absyn_Constraint*_tmpB58;unsigned int _tmpB59;struct Cyc_Absyn_FnInfo
_tmpB5B;struct Cyc_List_List*_tmpB5C;struct Cyc_List_List**_tmpB5D;struct Cyc_Core_Opt*
_tmpB5E;struct Cyc_Core_Opt**_tmpB5F;struct Cyc_Absyn_Tqual _tmpB60;struct Cyc_Absyn_Tqual*
_tmpB61;void*_tmpB62;struct Cyc_List_List*_tmpB63;int _tmpB64;struct Cyc_Absyn_VarargInfo*
_tmpB65;struct Cyc_List_List*_tmpB66;struct Cyc_List_List*_tmpB67;struct Cyc_List_List*
_tmpB69;enum Cyc_Absyn_AggrKind _tmpB6B;struct Cyc_List_List*_tmpB6C;struct Cyc_Absyn_AggrInfo
_tmpB6E;union Cyc_Absyn_AggrInfoU _tmpB6F;union Cyc_Absyn_AggrInfoU*_tmpB70;struct
Cyc_List_List*_tmpB71;struct Cyc_List_List**_tmpB72;struct _tuple2*_tmpB74;struct
Cyc_List_List*_tmpB75;struct Cyc_List_List**_tmpB76;struct Cyc_Absyn_Typedefdecl*
_tmpB77;struct Cyc_Absyn_Typedefdecl**_tmpB78;void**_tmpB79;void***_tmpB7A;void*
_tmpB7F;void*_tmpB81;void*_tmpB82;void*_tmpB84;void*_tmpB86;struct Cyc_List_List*
_tmpB88;_LL56F: {struct Cyc_Absyn_VoidType_struct*_tmpB22=(struct Cyc_Absyn_VoidType_struct*)
_tmpB21;if(_tmpB22->tag != 0)goto _LL571;}_LL570: goto _LL56E;_LL571: {struct Cyc_Absyn_Evar_struct*
_tmpB23=(struct Cyc_Absyn_Evar_struct*)_tmpB21;if(_tmpB23->tag != 1)goto _LL573;
else{_tmpB24=_tmpB23->f1;_tmpB25=(struct Cyc_Core_Opt**)& _tmpB23->f1;_tmpB26=
_tmpB23->f2;_tmpB27=(struct Cyc_Core_Opt**)& _tmpB23->f2;}}_LL572: if(*_tmpB25 == 0
 || Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)
_check_null(*_tmpB25))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((
struct Cyc_Core_Opt*)_check_null(*_tmpB25))->v,expected_kind))*_tmpB25=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind
== Cyc_Absyn_RgnKind){if(expected_kind->aliasqual == Cyc_Absyn_Unique)*_tmpB27=(
struct Cyc_Core_Opt*)& urgn;else{*_tmpB27=(struct Cyc_Core_Opt*)& hrgn;}}else{if(
cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp145B;struct Cyc_Absyn_Less_kb_struct*
_tmp145A;struct Cyc_Absyn_Tvar*_tmpB89=Cyc_Tcutil_new_tvar((void*)((_tmp145A=
_cycalloc(sizeof(*_tmp145A)),((_tmp145A[0]=((_tmp145B.tag=2,((_tmp145B.f1=0,((
_tmp145B.f2=expected_kind,_tmp145B)))))),_tmp145A)))));{struct Cyc_Absyn_VarType_struct*
_tmp1461;struct Cyc_Absyn_VarType_struct _tmp1460;struct Cyc_Core_Opt*_tmp145F;*
_tmpB27=((_tmp145F=_cycalloc(sizeof(*_tmp145F)),((_tmp145F->v=(void*)((_tmp1461=
_cycalloc(sizeof(*_tmp1461)),((_tmp1461[0]=((_tmp1460.tag=2,((_tmp1460.f1=
_tmpB89,_tmp1460)))),_tmp1461)))),_tmp145F))));}_tmpB29=_tmpB89;goto _LL574;}
else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,
put_in_effect);}}goto _LL56E;_LL573: {struct Cyc_Absyn_VarType_struct*_tmpB28=(
struct Cyc_Absyn_VarType_struct*)_tmpB21;if(_tmpB28->tag != 2)goto _LL575;else{
_tmpB29=_tmpB28->f1;}}_LL574:{void*_tmpB8F=Cyc_Absyn_compress_kb(_tmpB29->kind);
struct Cyc_Core_Opt*_tmpB91;struct Cyc_Core_Opt**_tmpB92;_LL5A6: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpB90=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpB8F;if(_tmpB90->tag != 1)goto
_LL5A8;else{_tmpB91=_tmpB90->f1;_tmpB92=(struct Cyc_Core_Opt**)& _tmpB90->f1;}}
_LL5A7:{struct Cyc_Absyn_Less_kb_struct*_tmp1467;struct Cyc_Absyn_Less_kb_struct
_tmp1466;struct Cyc_Core_Opt*_tmp1465;*_tmpB92=((_tmp1465=_cycalloc(sizeof(*
_tmp1465)),((_tmp1465->v=(void*)((_tmp1467=_cycalloc(sizeof(*_tmp1467)),((
_tmp1467[0]=((_tmp1466.tag=2,((_tmp1466.f1=0,((_tmp1466.f2=expected_kind,
_tmp1466)))))),_tmp1467)))),_tmp1465))));}goto _LL5A5;_LL5A8:;_LL5A9: goto _LL5A5;
_LL5A5:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB29);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB29,put_in_effect);{void*_tmpB96=Cyc_Absyn_compress_kb(_tmpB29->kind);struct
Cyc_Core_Opt*_tmpB98;struct Cyc_Core_Opt**_tmpB99;struct Cyc_Absyn_Kind*_tmpB9A;
_LL5AB: {struct Cyc_Absyn_Less_kb_struct*_tmpB97=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB96;if(_tmpB97->tag != 2)goto _LL5AD;else{_tmpB98=_tmpB97->f1;_tmpB99=(struct
Cyc_Core_Opt**)& _tmpB97->f1;_tmpB9A=_tmpB97->f2;}}_LL5AC: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmpB9A)){struct Cyc_Absyn_Less_kb_struct*_tmp146D;struct Cyc_Absyn_Less_kb_struct
_tmp146C;struct Cyc_Core_Opt*_tmp146B;*_tmpB99=((_tmp146B=_cycalloc(sizeof(*
_tmp146B)),((_tmp146B->v=(void*)((_tmp146D=_cycalloc(sizeof(*_tmp146D)),((
_tmp146D[0]=((_tmp146C.tag=2,((_tmp146C.f1=0,((_tmp146C.f2=expected_kind,
_tmp146C)))))),_tmp146D)))),_tmp146B))));}goto _LL5AA;_LL5AD:;_LL5AE: goto _LL5AA;
_LL5AA:;}goto _LL56E;_LL575: {struct Cyc_Absyn_TypeDeclType_struct*_tmpB2A=(struct
Cyc_Absyn_TypeDeclType_struct*)_tmpB21;if(_tmpB2A->tag != 26)goto _LL577;else{
_tmpB2B=_tmpB2A->f1;_tmpB2C=*_tmpB2B;_tmpB2D=_tmpB2C.r;_tmpB2E=_tmpB2A->f2;
_tmpB2F=(void***)& _tmpB2A->f2;}}_LL576: {void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(
t));{void*_tmpB9E=_tmpB2D;struct Cyc_Absyn_Aggrdecl*_tmpBA0;struct Cyc_Absyn_Enumdecl*
_tmpBA2;struct Cyc_Absyn_Datatypedecl*_tmpBA4;_LL5B0: {struct Cyc_Absyn_Aggr_td_struct*
_tmpB9F=(struct Cyc_Absyn_Aggr_td_struct*)_tmpB9E;if(_tmpB9F->tag != 0)goto _LL5B2;
else{_tmpBA0=_tmpB9F->f1;}}_LL5B1: Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBA0);goto _LL5AF;_LL5B2: {struct Cyc_Absyn_Enum_td_struct*_tmpBA1=(struct Cyc_Absyn_Enum_td_struct*)
_tmpB9E;if(_tmpBA1->tag != 1)goto _LL5B4;else{_tmpBA2=_tmpBA1->f1;}}_LL5B3: Cyc_Tc_tcEnumdecl(
te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns),loc,_tmpBA2);goto _LL5AF;_LL5B4: {struct Cyc_Absyn_Datatype_td_struct*
_tmpBA3=(struct Cyc_Absyn_Datatype_td_struct*)_tmpB9E;if(_tmpBA3->tag != 2)goto
_LL5AF;else{_tmpBA4=_tmpBA3->f1;}}_LL5B5: Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,
_tmpBA4);goto _LL5AF;_LL5AF:;}{void**_tmp146E;*_tmpB2F=((_tmp146E=_cycalloc(
sizeof(*_tmp146E)),((_tmp146E[0]=new_t,_tmp146E))));}return Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL577: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpB30=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpB21;if(_tmpB30->tag != 14)goto
_LL579;else{_tmpB31=_tmpB30->f1;}}_LL578: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmpBA6=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmpBA6;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;for(0;_tmpB31 != 0;_tmpB31=_tmpB31->tl){struct Cyc_Absyn_Enumfield*
_tmpBA7=(struct Cyc_Absyn_Enumfield*)_tmpB31->hd;if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpBA7->name).f2)){
const char*_tmp1472;void*_tmp1471[1];struct Cyc_String_pa_struct _tmp1470;(_tmp1470.tag=
0,((_tmp1470.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBA7->name).f2),((
_tmp1471[0]=& _tmp1470,Cyc_Tcutil_terr(_tmpBA7->loc,((_tmp1472="duplicate enum field name %s",
_tag_dyneither(_tmp1472,sizeof(char),29))),_tag_dyneither(_tmp1471,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp1473;prev_fields=((_tmp1473=
_region_malloc(uprev_rgn,sizeof(*_tmp1473)),((_tmp1473->hd=(*_tmpBA7->name).f2,((
_tmp1473->tl=prev_fields,_tmp1473))))));}if(_tmpBA7->tag == 0)_tmpBA7->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmpBA7->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpBA7->tag))){const char*_tmp1477;void*
_tmp1476[1];struct Cyc_String_pa_struct _tmp1475;(_tmp1475.tag=0,((_tmp1475.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpBA7->name).f2),((_tmp1476[0]=&
_tmp1475,Cyc_Tcutil_terr(loc,((_tmp1477="enum field %s: expression is not constant",
_tag_dyneither(_tmp1477,sizeof(char),42))),_tag_dyneither(_tmp1476,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmpBA7->tag))).f1;tag_count=t1 + 1;(*_tmpBA7->name).f1=Cyc_Absyn_Abs_n(
te->ns,0);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp147D;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp147C;struct _tuple31*_tmp147B;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple31*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmpBA7->name).f2,(struct _tuple31*)((_tmp147B=_cycalloc(sizeof(*_tmp147B)),((
_tmp147B->f1=(void*)((_tmp147D=_cycalloc(sizeof(*_tmp147D)),((_tmp147D[0]=((
_tmp147C.tag=4,((_tmp147C.f1=(void*)t,((_tmp147C.f2=_tmpBA7,_tmp147C)))))),
_tmp147D)))),((_tmp147B->f2=1,_tmp147B)))))));};};}}_npop_handler(0);goto _LL56E;;
_pop_region(uprev_rgn);}_LL579: {struct Cyc_Absyn_EnumType_struct*_tmpB32=(struct
Cyc_Absyn_EnumType_struct*)_tmpB21;if(_tmpB32->tag != 13)goto _LL57B;else{_tmpB33=
_tmpB32->f1;_tmpB34=_tmpB32->f2;_tmpB35=(struct Cyc_Absyn_Enumdecl**)& _tmpB32->f2;}}
_LL57A: if(*_tmpB35 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmpB35))->fields
== 0){struct _handler_cons _tmpBB2;_push_handler(& _tmpBB2);{int _tmpBB4=0;if(setjmp(
_tmpBB2.handler))_tmpBB4=1;if(!_tmpBB4){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpB33);*_tmpB35=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpBB3=(void*)_exn_thrown;void*_tmpBB6=_tmpBB3;_LL5B7: {struct Cyc_Dict_Absent_struct*
_tmpBB7=(struct Cyc_Dict_Absent_struct*)_tmpBB6;if(_tmpBB7->tag != Cyc_Dict_Absent)
goto _LL5B9;}_LL5B8: {struct Cyc_Tcenv_Genv*_tmpBB8=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp147E;struct Cyc_Absyn_Enumdecl*_tmpBB9=(_tmp147E=
_cycalloc(sizeof(*_tmp147E)),((_tmp147E->sc=Cyc_Absyn_Extern,((_tmp147E->name=
_tmpB33,((_tmp147E->fields=0,_tmp147E)))))));Cyc_Tc_tcEnumdecl(te,_tmpBB8,loc,
_tmpBB9);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmpB33);*
_tmpB35=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL5B6;};}_LL5B9:;_LL5BA:(void)_throw(
_tmpBB6);_LL5B6:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpB35);*_tmpB33=(ed->name)[0];goto _LL56E;};_LL57B: {struct Cyc_Absyn_DatatypeType_struct*
_tmpB36=(struct Cyc_Absyn_DatatypeType_struct*)_tmpB21;if(_tmpB36->tag != 3)goto
_LL57D;else{_tmpB37=_tmpB36->f1;_tmpB38=_tmpB37.datatype_info;_tmpB39=(union Cyc_Absyn_DatatypeInfoU*)&(
_tmpB36->f1).datatype_info;_tmpB3A=_tmpB37.targs;_tmpB3B=(struct Cyc_List_List**)&(
_tmpB36->f1).targs;}}_LL57C: {struct Cyc_List_List*_tmpBBB=*_tmpB3B;{union Cyc_Absyn_DatatypeInfoU
_tmpBBC=*_tmpB39;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBBD;struct _tuple2*
_tmpBBE;int _tmpBBF;struct Cyc_Absyn_Datatypedecl**_tmpBC0;struct Cyc_Absyn_Datatypedecl*
_tmpBC1;_LL5BC: if((_tmpBBC.UnknownDatatype).tag != 1)goto _LL5BE;_tmpBBD=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpBBC.UnknownDatatype).val;_tmpBBE=_tmpBBD.name;
_tmpBBF=_tmpBBD.is_extensible;_LL5BD: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmpBC2;_push_handler(& _tmpBC2);{int _tmpBC4=0;if(setjmp(
_tmpBC2.handler))_tmpBC4=1;if(!_tmpBC4){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmpBBE);;_pop_handler();}else{void*_tmpBC3=(void*)_exn_thrown;void*_tmpBC6=
_tmpBC3;_LL5C1: {struct Cyc_Dict_Absent_struct*_tmpBC7=(struct Cyc_Dict_Absent_struct*)
_tmpBC6;if(_tmpBC7->tag != Cyc_Dict_Absent)goto _LL5C3;}_LL5C2: {struct Cyc_Tcenv_Genv*
_tmpBC8=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp147F;struct Cyc_Absyn_Datatypedecl*
_tmpBC9=(_tmp147F=_cycalloc(sizeof(*_tmp147F)),((_tmp147F->sc=Cyc_Absyn_Extern,((
_tmp147F->name=_tmpBBE,((_tmp147F->tvs=0,((_tmp147F->fields=0,((_tmp147F->is_extensible=
_tmpBBF,_tmp147F)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpBC8,loc,_tmpBC9);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpBBE);if(_tmpBBB != 0){{const char*
_tmp1483;void*_tmp1482[1];struct Cyc_String_pa_struct _tmp1481;(_tmp1481.tag=0,((
_tmp1481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBBE)),((_tmp1482[0]=& _tmp1481,Cyc_Tcutil_terr(loc,((_tmp1483="declare parameterized datatype %s before using",
_tag_dyneither(_tmp1483,sizeof(char),47))),_tag_dyneither(_tmp1482,sizeof(void*),
1)))))));}return cvtenv;}goto _LL5C0;}_LL5C3:;_LL5C4:(void)_throw(_tmpBC6);_LL5C0:;}};}
if(_tmpBBF  && !(*tudp)->is_extensible){const char*_tmp1487;void*_tmp1486[1];
struct Cyc_String_pa_struct _tmp1485;(_tmp1485.tag=0,((_tmp1485.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBBE)),((
_tmp1486[0]=& _tmp1485,Cyc_Tcutil_terr(loc,((_tmp1487="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1487,sizeof(char),41))),_tag_dyneither(_tmp1486,sizeof(void*),
1)))))));}*_tmpB39=Cyc_Absyn_KnownDatatype(tudp);_tmpBC1=*tudp;goto _LL5BF;}
_LL5BE: if((_tmpBBC.KnownDatatype).tag != 2)goto _LL5BB;_tmpBC0=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBBC.KnownDatatype).val;_tmpBC1=*_tmpBC0;_LL5BF: {struct Cyc_List_List*tvs=
_tmpBC1->tvs;for(0;_tmpBBB != 0  && tvs != 0;(_tmpBBB=_tmpBBB->tl,tvs=tvs->tl)){
void*t=(void*)_tmpBBB->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp1488;struct _tuple0 _tmpBD2=(_tmp1488.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp1488.f2=t,_tmp1488)));void*_tmpBD3;void*_tmpBD5;struct Cyc_Absyn_Tvar*_tmpBD7;
_LL5C6: _tmpBD3=_tmpBD2.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpBD4=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpBD3;if(_tmpBD4->tag != 1)goto _LL5C8;};_tmpBD5=_tmpBD2.f2;{struct Cyc_Absyn_VarType_struct*
_tmpBD6=(struct Cyc_Absyn_VarType_struct*)_tmpBD5;if(_tmpBD6->tag != 2)goto _LL5C8;
else{_tmpBD7=_tmpBD6->f1;}};_LL5C7: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpBD7);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpBD7,1);continue;_LL5C8:;_LL5C9: goto _LL5C5;_LL5C5:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpBBB != 0){const
char*_tmp148C;void*_tmp148B[1];struct Cyc_String_pa_struct _tmp148A;(_tmp148A.tag=
0,((_tmp148A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBC1->name)),((_tmp148B[0]=& _tmp148A,Cyc_Tcutil_terr(loc,((_tmp148C="too many type arguments for datatype %s",
_tag_dyneither(_tmp148C,sizeof(char),40))),_tag_dyneither(_tmp148B,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,&
Cyc_Tcutil_bk,expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*
_tmp148D;hidden_ts=((_tmp148D=_cycalloc(sizeof(*_tmp148D)),((_tmp148D->hd=e,((
_tmp148D->tl=hidden_ts,_tmp148D))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,
te,cvtenv,k1,e,1);}*_tmpB3B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB3B,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}goto _LL5BB;}_LL5BB:;}goto
_LL56E;}_LL57D: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB3C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpB21;if(_tmpB3C->tag != 4)goto _LL57F;else{_tmpB3D=_tmpB3C->f1;_tmpB3E=_tmpB3D.field_info;
_tmpB3F=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmpB3C->f1).field_info;_tmpB40=
_tmpB3D.targs;}}_LL57E:{union Cyc_Absyn_DatatypeFieldInfoU _tmpBDC=*_tmpB3F;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBDD;struct _tuple2*_tmpBDE;struct _tuple2*
_tmpBDF;int _tmpBE0;struct _tuple3 _tmpBE1;struct Cyc_Absyn_Datatypedecl*_tmpBE2;
struct Cyc_Absyn_Datatypefield*_tmpBE3;_LL5CB: if((_tmpBDC.UnknownDatatypefield).tag
!= 1)goto _LL5CD;_tmpBDD=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpBDC.UnknownDatatypefield).val;
_tmpBDE=_tmpBDD.datatype_name;_tmpBDF=_tmpBDD.field_name;_tmpBE0=_tmpBDD.is_extensible;
_LL5CC: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpBE4;_push_handler(& _tmpBE4);{int _tmpBE6=0;if(setjmp(
_tmpBE4.handler))_tmpBE6=1;if(!_tmpBE6){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpBDE);;_pop_handler();}else{void*_tmpBE5=(void*)_exn_thrown;void*_tmpBE8=
_tmpBE5;_LL5D0: {struct Cyc_Dict_Absent_struct*_tmpBE9=(struct Cyc_Dict_Absent_struct*)
_tmpBE8;if(_tmpBE9->tag != Cyc_Dict_Absent)goto _LL5D2;}_LL5D1:{const char*_tmp1491;
void*_tmp1490[1];struct Cyc_String_pa_struct _tmp148F;(_tmp148F.tag=0,((_tmp148F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDE)),((
_tmp1490[0]=& _tmp148F,Cyc_Tcutil_terr(loc,((_tmp1491="unbound datatype %s",
_tag_dyneither(_tmp1491,sizeof(char),20))),_tag_dyneither(_tmp1490,sizeof(void*),
1)))))));}return cvtenv;_LL5D2:;_LL5D3:(void)_throw(_tmpBE8);_LL5CF:;}};}{struct
_handler_cons _tmpBED;_push_handler(& _tmpBED);{int _tmpBEF=0;if(setjmp(_tmpBED.handler))
_tmpBEF=1;if(!_tmpBEF){{struct _RegionHandle _tmpBF0=_new_region("r");struct
_RegionHandle*r=& _tmpBF0;_push_region(r);{void*_tmpBF1=Cyc_Tcenv_lookup_ordinary(
r,te,loc,_tmpBDF);struct Cyc_Absyn_Datatypedecl*_tmpBF3;struct Cyc_Absyn_Datatypefield*
_tmpBF4;_LL5D5: {struct Cyc_Tcenv_DatatypeRes_struct*_tmpBF2=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmpBF1;if(_tmpBF2->tag != 2)goto _LL5D7;else{_tmpBF3=_tmpBF2->f1;_tmpBF4=_tmpBF2->f2;}}
_LL5D6: tuf=_tmpBF4;tud=_tmpBF3;if(_tmpBE0  && !tud->is_extensible){const char*
_tmp1495;void*_tmp1494[1];struct Cyc_String_pa_struct _tmp1493;(_tmp1493.tag=0,((
_tmp1493.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBDE)),((_tmp1494[0]=& _tmp1493,Cyc_Tcutil_terr(loc,((_tmp1495="datatype %s was not declared @extensible",
_tag_dyneither(_tmp1495,sizeof(char),41))),_tag_dyneither(_tmp1494,sizeof(void*),
1)))))));}goto _LL5D4;_LL5D7:;_LL5D8:{const char*_tmp149A;void*_tmp1499[2];struct
Cyc_String_pa_struct _tmp1498;struct Cyc_String_pa_struct _tmp1497;(_tmp1497.tag=0,((
_tmp1497.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBDE)),((_tmp1498.tag=0,((_tmp1498.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDF)),((_tmp1499[0]=& _tmp1498,((
_tmp1499[1]=& _tmp1497,Cyc_Tcutil_terr(loc,((_tmp149A="unbound field %s in type datatype %s",
_tag_dyneither(_tmp149A,sizeof(char),37))),_tag_dyneither(_tmp1499,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpBFC=cvtenv;_npop_handler(1);return
_tmpBFC;};_LL5D4:;};_pop_region(r);};_pop_handler();}else{void*_tmpBEE=(void*)
_exn_thrown;void*_tmpBFE=_tmpBEE;_LL5DA: {struct Cyc_Dict_Absent_struct*_tmpBFF=(
struct Cyc_Dict_Absent_struct*)_tmpBFE;if(_tmpBFF->tag != Cyc_Dict_Absent)goto
_LL5DC;}_LL5DB:{const char*_tmp149F;void*_tmp149E[2];struct Cyc_String_pa_struct
_tmp149D;struct Cyc_String_pa_struct _tmp149C;(_tmp149C.tag=0,((_tmp149C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBDE)),((
_tmp149D.tag=0,((_tmp149D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBDF)),((_tmp149E[0]=& _tmp149D,((_tmp149E[1]=& _tmp149C,Cyc_Tcutil_terr(loc,((
_tmp149F="unbound field %s in type datatype %s",_tag_dyneither(_tmp149F,sizeof(
char),37))),_tag_dyneither(_tmp149E,sizeof(void*),2)))))))))))));}return cvtenv;
_LL5DC:;_LL5DD:(void)_throw(_tmpBFE);_LL5D9:;}};}*_tmpB3F=Cyc_Absyn_KnownDatatypefield(
tud,tuf);_tmpBE2=tud;_tmpBE3=tuf;goto _LL5CE;}_LL5CD: if((_tmpBDC.KnownDatatypefield).tag
!= 2)goto _LL5CA;_tmpBE1=(struct _tuple3)(_tmpBDC.KnownDatatypefield).val;_tmpBE2=
_tmpBE1.f1;_tmpBE3=_tmpBE1.f2;_LL5CE: {struct Cyc_List_List*tvs=_tmpBE2->tvs;for(
0;_tmpB40 != 0  && tvs != 0;(_tmpB40=_tmpB40->tl,tvs=tvs->tl)){void*t=(void*)
_tmpB40->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{struct
_tuple0 _tmp14A0;struct _tuple0 _tmpC05=(_tmp14A0.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp14A0.f2=t,_tmp14A0)));void*_tmpC06;void*_tmpC08;struct Cyc_Absyn_Tvar*_tmpC0A;
_LL5DF: _tmpC06=_tmpC05.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpC07=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC06;if(_tmpC07->tag != 1)goto _LL5E1;};_tmpC08=_tmpC05.f2;{struct Cyc_Absyn_VarType_struct*
_tmpC09=(struct Cyc_Absyn_VarType_struct*)_tmpC08;if(_tmpC09->tag != 2)goto _LL5E1;
else{_tmpC0A=_tmpC09->f1;}};_LL5E0: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpC0A);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpC0A,1);continue;_LL5E1:;_LL5E2: goto _LL5DE;_LL5DE:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);Cyc_Tcutil_check_no_qual(loc,t);};}if(_tmpB40 != 0){const
char*_tmp14A5;void*_tmp14A4[2];struct Cyc_String_pa_struct _tmp14A3;struct Cyc_String_pa_struct
_tmp14A2;(_tmp14A2.tag=0,((_tmp14A2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE3->name)),((_tmp14A3.tag=0,((
_tmp14A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE2->name)),((_tmp14A4[0]=& _tmp14A3,((_tmp14A4[1]=& _tmp14A2,Cyc_Tcutil_terr(
loc,((_tmp14A5="too many type arguments for datatype %s.%s",_tag_dyneither(
_tmp14A5,sizeof(char),43))),_tag_dyneither(_tmp14A4,sizeof(void*),2)))))))))))));}
if(tvs != 0){const char*_tmp14AA;void*_tmp14A9[2];struct Cyc_String_pa_struct
_tmp14A8;struct Cyc_String_pa_struct _tmp14A7;(_tmp14A7.tag=0,((_tmp14A7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE3->name)),((
_tmp14A8.tag=0,((_tmp14A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpBE2->name)),((_tmp14A9[0]=& _tmp14A8,((_tmp14A9[1]=& _tmp14A7,Cyc_Tcutil_terr(
loc,((_tmp14AA="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp14AA,sizeof(char),42))),_tag_dyneither(_tmp14A9,sizeof(void*),2)))))))))))));}
goto _LL5CA;}_LL5CA:;}goto _LL56E;_LL57F: {struct Cyc_Absyn_PointerType_struct*
_tmpB41=(struct Cyc_Absyn_PointerType_struct*)_tmpB21;if(_tmpB41->tag != 5)goto
_LL581;else{_tmpB42=_tmpB41->f1;_tmpB43=_tmpB42.elt_typ;_tmpB44=_tmpB42.elt_tq;
_tmpB45=(struct Cyc_Absyn_Tqual*)&(_tmpB41->f1).elt_tq;_tmpB46=_tmpB42.ptr_atts;
_tmpB47=_tmpB46.rgn;_tmpB48=_tmpB46.nullable;_tmpB49=_tmpB46.bounds;_tmpB4A=
_tmpB46.zero_term;}}_LL580: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB43,1);_tmpB45->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB45->print_const,_tmpB43);{struct Cyc_Absyn_Kind*k;switch(expected_kind->aliasqual){
case Cyc_Absyn_Aliasable: _LL5E3: k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique:
_LL5E4: k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL5E5: k=& Cyc_Tcutil_trk;break;}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmpB47,1);{union Cyc_Absyn_Constraint*
_tmpC13=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmpB4A);union Cyc_Absyn_Constraint _tmpC14;int _tmpC15;union Cyc_Absyn_Constraint
_tmpC16;int _tmpC17;_LL5E8: _tmpC14=*_tmpC13;if((_tmpC14.No_constr).tag != 3)goto
_LL5EA;_tmpC15=(int)(_tmpC14.No_constr).val;_LL5E9:{void*_tmpC18=Cyc_Tcutil_compress(
_tmpB43);enum Cyc_Absyn_Size_of _tmpC1A;_LL5EF: {struct Cyc_Absyn_IntType_struct*
_tmpC19=(struct Cyc_Absyn_IntType_struct*)_tmpC18;if(_tmpC19->tag != 6)goto _LL5F1;
else{_tmpC1A=_tmpC19->f2;if(_tmpC1A != Cyc_Absyn_Char_sz)goto _LL5F1;}}_LL5F0:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB4A,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL5EE;_LL5F1:;_LL5F2:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmpB4A,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL5EE;
_LL5EE:;}goto _LL5E7;_LL5EA: _tmpC16=*_tmpC13;if((_tmpC16.Eq_constr).tag != 1)goto
_LL5EC;_tmpC17=(int)(_tmpC16.Eq_constr).val;if(_tmpC17 != 1)goto _LL5EC;_LL5EB: if(
!Cyc_Tcutil_admits_zero(_tmpB43)){const char*_tmp14AE;void*_tmp14AD[1];struct Cyc_String_pa_struct
_tmp14AC;(_tmp14AC.tag=0,((_tmp14AC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB43)),((_tmp14AD[0]=& _tmp14AC,Cyc_Tcutil_terr(
loc,((_tmp14AE="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp14AE,sizeof(char),53))),_tag_dyneither(_tmp14AD,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5E7;_LL5EC:;_LL5ED: is_zero_terminated=0;
goto _LL5E7;_LL5E7:;}{void*_tmpC1E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpB49);struct Cyc_Absyn_Exp*
_tmpC21;_LL5F4: {struct Cyc_Absyn_DynEither_b_struct*_tmpC1F=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC1E;if(_tmpC1F->tag != 0)goto _LL5F6;}_LL5F5: goto _LL5F3;_LL5F6: {struct Cyc_Absyn_Upper_b_struct*
_tmpC20=(struct Cyc_Absyn_Upper_b_struct*)_tmpC1E;if(_tmpC20->tag != 1)goto _LL5F3;
else{_tmpC21=_tmpC20->f1;}}_LL5F7: {struct _RegionHandle _tmpC22=_new_region("temp");
struct _RegionHandle*temp=& _tmpC22;_push_region(temp);{struct Cyc_Tcenv_Tenv*
_tmpC23=Cyc_Tcenv_allow_valueof(temp,te);Cyc_Tcexp_tcExp(_tmpC23,0,_tmpC21);}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC21,te,cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmpC21)){const char*_tmp14B1;void*_tmp14B0;(_tmp14B0=0,Cyc_Tcutil_terr(loc,((
_tmp14B1="pointer bounds expression is not an unsigned int",_tag_dyneither(
_tmp14B1,sizeof(char),49))),_tag_dyneither(_tmp14B0,sizeof(void*),0)));}{
unsigned int _tmpC27;int _tmpC28;struct _tuple14 _tmpC26=Cyc_Evexp_eval_const_uint_exp(
_tmpC21);_tmpC27=_tmpC26.f1;_tmpC28=_tmpC26.f2;if(is_zero_terminated  && (!
_tmpC28  || _tmpC27 < 1)){const char*_tmp14B4;void*_tmp14B3;(_tmp14B3=0,Cyc_Tcutil_terr(
loc,((_tmp14B4="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp14B4,sizeof(char),55))),_tag_dyneither(_tmp14B3,sizeof(void*),
0)));}_npop_handler(0);goto _LL5F3;};;_pop_region(temp);}_LL5F3:;}goto _LL56E;};}
_LL581: {struct Cyc_Absyn_TagType_struct*_tmpB4B=(struct Cyc_Absyn_TagType_struct*)
_tmpB21;if(_tmpB4B->tag != 19)goto _LL583;else{_tmpB4C=(void*)_tmpB4B->f1;}}_LL582:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB4C,1);
goto _LL56E;_LL583: {struct Cyc_Absyn_ValueofType_struct*_tmpB4D=(struct Cyc_Absyn_ValueofType_struct*)
_tmpB21;if(_tmpB4D->tag != 18)goto _LL585;else{_tmpB4E=_tmpB4D->f1;}}_LL584: {
struct _RegionHandle _tmpC2B=_new_region("temp");struct _RegionHandle*temp=& _tmpC2B;
_push_region(temp);{struct Cyc_Tcenv_Tenv*_tmpC2C=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpC2C,0,_tmpB4E);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpB4E)){
const char*_tmp14B7;void*_tmp14B6;(_tmp14B6=0,Cyc_Tcutil_terr(loc,((_tmp14B7="valueof_t requires an int expression",
_tag_dyneither(_tmp14B7,sizeof(char),37))),_tag_dyneither(_tmp14B6,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB4E,te,cvtenv);
_npop_handler(0);goto _LL56E;;_pop_region(temp);}_LL585: {struct Cyc_Absyn_IntType_struct*
_tmpB4F=(struct Cyc_Absyn_IntType_struct*)_tmpB21;if(_tmpB4F->tag != 6)goto _LL587;}
_LL586: goto _LL588;_LL587: {struct Cyc_Absyn_FloatType_struct*_tmpB50=(struct Cyc_Absyn_FloatType_struct*)
_tmpB21;if(_tmpB50->tag != 7)goto _LL589;}_LL588: goto _LL56E;_LL589: {struct Cyc_Absyn_ArrayType_struct*
_tmpB51=(struct Cyc_Absyn_ArrayType_struct*)_tmpB21;if(_tmpB51->tag != 8)goto
_LL58B;else{_tmpB52=_tmpB51->f1;_tmpB53=_tmpB52.elt_type;_tmpB54=_tmpB52.tq;
_tmpB55=(struct Cyc_Absyn_Tqual*)&(_tmpB51->f1).tq;_tmpB56=_tmpB52.num_elts;
_tmpB57=(struct Cyc_Absyn_Exp**)&(_tmpB51->f1).num_elts;_tmpB58=_tmpB52.zero_term;
_tmpB59=_tmpB52.zt_loc;}}_LL58A: {struct Cyc_Absyn_Exp*_tmpC2F=*_tmpB57;cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB53,1);_tmpB55->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB55->print_const,_tmpB53);{int
is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpC30=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmpB58);union Cyc_Absyn_Constraint
_tmpC31;int _tmpC32;union Cyc_Absyn_Constraint _tmpC33;int _tmpC34;_LL5F9: _tmpC31=*
_tmpC30;if((_tmpC31.No_constr).tag != 3)goto _LL5FB;_tmpC32=(int)(_tmpC31.No_constr).val;
_LL5FA:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpB58,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL5F8;_LL5FB: _tmpC33=*_tmpC30;if((_tmpC33.Eq_constr).tag
!= 1)goto _LL5FD;_tmpC34=(int)(_tmpC33.Eq_constr).val;if(_tmpC34 != 1)goto _LL5FD;
_LL5FC: if(!Cyc_Tcutil_admits_zero(_tmpB53)){const char*_tmp14BB;void*_tmp14BA[1];
struct Cyc_String_pa_struct _tmp14B9;(_tmp14B9.tag=0,((_tmp14B9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB53)),((
_tmp14BA[0]=& _tmp14B9,Cyc_Tcutil_terr(loc,((_tmp14BB="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp14BB,sizeof(char),51))),_tag_dyneither(_tmp14BA,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL5F8;_LL5FD:;_LL5FE: is_zero_terminated=0;
goto _LL5F8;_LL5F8:;}if(_tmpC2F == 0){if(is_zero_terminated)*_tmpB57=(_tmpC2F=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp14BE;void*
_tmp14BD;(_tmp14BD=0,Cyc_Tcutil_warn(loc,((_tmp14BE="array bound defaults to 1 here",
_tag_dyneither(_tmp14BE,sizeof(char),31))),_tag_dyneither(_tmp14BD,sizeof(void*),
0)));}*_tmpB57=(_tmpC2F=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpC2F);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpC2F)){const char*_tmp14C1;void*_tmp14C0;(_tmp14C0=0,Cyc_Tcutil_terr(loc,((
_tmp14C1="array bounds expression is not constant",_tag_dyneither(_tmp14C1,
sizeof(char),40))),_tag_dyneither(_tmp14C0,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpC2F)){const char*_tmp14C4;void*_tmp14C3;(_tmp14C3=0,
Cyc_Tcutil_terr(loc,((_tmp14C4="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp14C4,sizeof(char),47))),_tag_dyneither(_tmp14C3,sizeof(void*),
0)));}{unsigned int _tmpC3F;int _tmpC40;struct _tuple14 _tmpC3E=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpC2F);_tmpC3F=_tmpC3E.f1;_tmpC40=_tmpC3E.f2;if((
is_zero_terminated  && _tmpC40) && _tmpC3F < 1){const char*_tmp14C7;void*_tmp14C6;(
_tmp14C6=0,Cyc_Tcutil_warn(loc,((_tmp14C7="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp14C7,sizeof(char),48))),_tag_dyneither(_tmp14C6,sizeof(void*),
0)));}if((_tmpC40  && _tmpC3F < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp14CA;void*
_tmp14C9;(_tmp14C9=0,Cyc_Tcutil_warn(loc,((_tmp14CA="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp14CA,sizeof(char),75))),_tag_dyneither(_tmp14C9,sizeof(void*),
0)));}*_tmpB57=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL56E;};};}
_LL58B: {struct Cyc_Absyn_FnType_struct*_tmpB5A=(struct Cyc_Absyn_FnType_struct*)
_tmpB21;if(_tmpB5A->tag != 9)goto _LL58D;else{_tmpB5B=_tmpB5A->f1;_tmpB5C=_tmpB5B.tvars;
_tmpB5D=(struct Cyc_List_List**)&(_tmpB5A->f1).tvars;_tmpB5E=_tmpB5B.effect;
_tmpB5F=(struct Cyc_Core_Opt**)&(_tmpB5A->f1).effect;_tmpB60=_tmpB5B.ret_tqual;
_tmpB61=(struct Cyc_Absyn_Tqual*)&(_tmpB5A->f1).ret_tqual;_tmpB62=_tmpB5B.ret_typ;
_tmpB63=_tmpB5B.args;_tmpB64=_tmpB5B.c_varargs;_tmpB65=_tmpB5B.cyc_varargs;
_tmpB66=_tmpB5B.rgn_po;_tmpB67=_tmpB5B.attributes;}}_LL58C: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpB67 != 0;
_tmpB67=_tmpB67->tl){if(!Cyc_Absyn_fntype_att((void*)_tmpB67->hd)){const char*
_tmp14CE;void*_tmp14CD[1];struct Cyc_String_pa_struct _tmp14CC;(_tmp14CC.tag=0,((
_tmp14CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmpB67->hd)),((_tmp14CD[0]=& _tmp14CC,Cyc_Tcutil_terr(loc,((_tmp14CE="bad function type attribute %s",
_tag_dyneither(_tmp14CE,sizeof(char),31))),_tag_dyneither(_tmp14CD,sizeof(void*),
1)))))));}{void*_tmpC48=(void*)_tmpB67->hd;enum Cyc_Absyn_Format_Type _tmpC4D;int
_tmpC4E;int _tmpC4F;_LL600: {struct Cyc_Absyn_Stdcall_att_struct*_tmpC49=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpC48;if(_tmpC49->tag != 1)goto _LL602;}_LL601: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL5FF;_LL602: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpC4A=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpC48;if(_tmpC4A->tag != 2)goto
_LL604;}_LL603: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL5FF;_LL604: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpC4B=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpC48;if(_tmpC4B->tag != 3)goto _LL606;}_LL605: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL5FF;_LL606: {struct Cyc_Absyn_Format_att_struct*_tmpC4C=(
struct Cyc_Absyn_Format_att_struct*)_tmpC48;if(_tmpC4C->tag != 19)goto _LL608;else{
_tmpC4D=_tmpC4C->f1;_tmpC4E=_tmpC4C->f2;_tmpC4F=_tmpC4C->f3;}}_LL607: if(!
seen_format){seen_format=1;ft=_tmpC4D;fmt_desc_arg=_tmpC4E;fmt_arg_start=_tmpC4F;}
else{const char*_tmp14D1;void*_tmp14D0;(_tmp14D0=0,Cyc_Tcutil_terr(loc,((_tmp14D1="function can't have multiple format attributes",
_tag_dyneither(_tmp14D1,sizeof(char),47))),_tag_dyneither(_tmp14D0,sizeof(void*),
0)));}goto _LL5FF;_LL608:;_LL609: goto _LL5FF;_LL5FF:;};}if(num_convs > 1){const char*
_tmp14D4;void*_tmp14D3;(_tmp14D3=0,Cyc_Tcutil_terr(loc,((_tmp14D4="function can't have multiple calling conventions",
_tag_dyneither(_tmp14D4,sizeof(char),49))),_tag_dyneither(_tmp14D3,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmpB5D);{struct Cyc_List_List*b=*
_tmpB5D;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpC54=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
struct Cyc_Absyn_Kind*_tmpC56;struct Cyc_Absyn_Kind _tmpC57;enum Cyc_Absyn_KindQual
_tmpC58;_LL60B: {struct Cyc_Absyn_Eq_kb_struct*_tmpC55=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpC54;if(_tmpC55->tag != 0)goto _LL60D;else{_tmpC56=_tmpC55->f1;_tmpC57=*_tmpC56;
_tmpC58=_tmpC57.kind;if(_tmpC58 != Cyc_Absyn_MemKind)goto _LL60D;}}_LL60C:{const
char*_tmp14D8;void*_tmp14D7[1];struct Cyc_String_pa_struct _tmp14D6;(_tmp14D6.tag=
0,((_tmp14D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp14D7[0]=& _tmp14D6,Cyc_Tcutil_terr(loc,((_tmp14D8="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp14D8,sizeof(char),51))),_tag_dyneither(_tmp14D7,sizeof(void*),
1)))))));}goto _LL60A;_LL60D:;_LL60E: goto _LL60A;_LL60A:;};}}{struct _RegionHandle
_tmpC5C=_new_region("newr");struct _RegionHandle*newr=& _tmpC5C;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp14D9;struct Cyc_Tcutil_CVTEnv _tmpC5D=(_tmp14D9.r=newr,((
_tmp14D9.kind_env=cvtenv.kind_env,((_tmp14D9.free_vars=0,((_tmp14D9.free_evars=0,((
_tmp14D9.generalize_evars=cvtenv.generalize_evars,((_tmp14D9.fn_result=1,
_tmp14D9)))))))))));_tmpC5D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC5D,& Cyc_Tcutil_tmk,
_tmpB62,1);_tmpB61->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB61->print_const,
_tmpB62);_tmpC5D.fn_result=0;{struct Cyc_List_List*a=_tmpB63;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpC5E=(struct _tuple10*)a->hd;void*_tmpC5F=(*_tmpC5E).f3;_tmpC5D=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC5D,& Cyc_Tcutil_tmk,_tmpC5F,1);{int
_tmpC60=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpC5E).f2).print_const,
_tmpC5F);((*_tmpC5E).f2).real_const=_tmpC60;{void*_tmpC61=Cyc_Tcutil_compress(
_tmpC5F);struct Cyc_Absyn_ArrayInfo _tmpC63;void*_tmpC64;struct Cyc_Absyn_Tqual
_tmpC65;struct Cyc_Absyn_Exp*_tmpC66;union Cyc_Absyn_Constraint*_tmpC67;
unsigned int _tmpC68;_LL610: {struct Cyc_Absyn_ArrayType_struct*_tmpC62=(struct Cyc_Absyn_ArrayType_struct*)
_tmpC61;if(_tmpC62->tag != 8)goto _LL612;else{_tmpC63=_tmpC62->f1;_tmpC64=_tmpC63.elt_type;
_tmpC65=_tmpC63.tq;_tmpC66=_tmpC63.num_elts;_tmpC67=_tmpC63.zero_term;_tmpC68=
_tmpC63.zt_loc;}}_LL611: {void*_tmpC69=Cyc_Absyn_new_evar(0,0);_tmpC5D=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5D,& Cyc_Tcutil_rk,_tmpC69,1);{struct Cyc_Absyn_Upper_b_struct _tmp14DC;
struct Cyc_Absyn_Upper_b_struct*_tmp14DB;void*_tmpC6A=Cyc_Absyn_atb_typ(_tmpC64,
_tmpC69,_tmpC65,(void*)((_tmp14DB=_cycalloc(sizeof(*_tmp14DB)),((_tmp14DB[0]=((
_tmp14DC.tag=1,((_tmp14DC.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpC66),_tmp14DC)))),
_tmp14DB)))),_tmpC67);(*_tmpC5E).f3=_tmpC6A;goto _LL60F;};}_LL612:;_LL613: goto
_LL60F;_LL60F:;};};}}if(_tmpB65 != 0){if(_tmpB64){const char*_tmp14DF;void*
_tmp14DE;(_tmp14DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp14DF="both c_vararg and cyc_vararg",_tag_dyneither(
_tmp14DF,sizeof(char),29))),_tag_dyneither(_tmp14DE,sizeof(void*),0)));}{void*
_tmpC70;int _tmpC71;struct Cyc_Absyn_VarargInfo _tmpC6F=*_tmpB65;_tmpC70=_tmpC6F.type;
_tmpC71=_tmpC6F.inject;_tmpC5D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpC5D,& Cyc_Tcutil_tmk,
_tmpC70,1);(_tmpB65->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(
_tmpB65->tq).print_const,_tmpC70);if(_tmpC71){void*_tmpC72=Cyc_Tcutil_compress(
_tmpC70);struct Cyc_Absyn_PtrInfo _tmpC74;void*_tmpC75;struct Cyc_Absyn_PtrAtts
_tmpC76;union Cyc_Absyn_Constraint*_tmpC77;union Cyc_Absyn_Constraint*_tmpC78;
_LL615: {struct Cyc_Absyn_PointerType_struct*_tmpC73=(struct Cyc_Absyn_PointerType_struct*)
_tmpC72;if(_tmpC73->tag != 5)goto _LL617;else{_tmpC74=_tmpC73->f1;_tmpC75=_tmpC74.elt_typ;
_tmpC76=_tmpC74.ptr_atts;_tmpC77=_tmpC76.bounds;_tmpC78=_tmpC76.zero_term;}}
_LL616:{void*_tmpC79=Cyc_Tcutil_compress(_tmpC75);_LL61A: {struct Cyc_Absyn_DatatypeType_struct*
_tmpC7A=(struct Cyc_Absyn_DatatypeType_struct*)_tmpC79;if(_tmpC7A->tag != 3)goto
_LL61C;}_LL61B: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpC78)){const char*_tmp14E2;void*_tmp14E1;(_tmp14E1=0,Cyc_Tcutil_terr(loc,((
_tmp14E2="can't inject into a zeroterm pointer",_tag_dyneither(_tmp14E2,sizeof(
char),37))),_tag_dyneither(_tmp14E1,sizeof(void*),0)));}{void*_tmpC7D=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpC77);_LL61F: {struct Cyc_Absyn_DynEither_b_struct*_tmpC7E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC7D;if(_tmpC7E->tag != 0)goto _LL621;}_LL620:{const char*_tmp14E5;void*_tmp14E4;(
_tmp14E4=0,Cyc_Tcutil_terr(loc,((_tmp14E5="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp14E5,sizeof(char),44))),_tag_dyneither(_tmp14E4,sizeof(void*),
0)));}goto _LL61E;_LL621:;_LL622: goto _LL61E;_LL61E:;}goto _LL619;_LL61C:;_LL61D:{
const char*_tmp14E8;void*_tmp14E7;(_tmp14E7=0,Cyc_Tcutil_terr(loc,((_tmp14E8="can't inject a non-datatype type",
_tag_dyneither(_tmp14E8,sizeof(char),33))),_tag_dyneither(_tmp14E7,sizeof(void*),
0)));}goto _LL619;_LL619:;}goto _LL614;_LL617:;_LL618:{const char*_tmp14EB;void*
_tmp14EA;(_tmp14EA=0,Cyc_Tcutil_terr(loc,((_tmp14EB="expecting a datatype pointer type",
_tag_dyneither(_tmp14EB,sizeof(char),34))),_tag_dyneither(_tmp14EA,sizeof(void*),
0)));}goto _LL614;_LL614:;}};}if(seen_format){int _tmpC85=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpB63);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpC85) || 
fmt_arg_start < 0) || _tmpB65 == 0  && fmt_arg_start != 0) || _tmpB65 != 0  && 
fmt_arg_start != _tmpC85 + 1){const char*_tmp14EE;void*_tmp14ED;(_tmp14ED=0,Cyc_Tcutil_terr(
loc,((_tmp14EE="bad format descriptor",_tag_dyneither(_tmp14EE,sizeof(char),22))),
_tag_dyneither(_tmp14ED,sizeof(void*),0)));}else{void*_tmpC89;struct _tuple10
_tmpC88=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpB63,
fmt_desc_arg - 1);_tmpC89=_tmpC88.f3;{void*_tmpC8A=Cyc_Tcutil_compress(_tmpC89);
struct Cyc_Absyn_PtrInfo _tmpC8C;void*_tmpC8D;struct Cyc_Absyn_PtrAtts _tmpC8E;union
Cyc_Absyn_Constraint*_tmpC8F;union Cyc_Absyn_Constraint*_tmpC90;_LL624: {struct
Cyc_Absyn_PointerType_struct*_tmpC8B=(struct Cyc_Absyn_PointerType_struct*)
_tmpC8A;if(_tmpC8B->tag != 5)goto _LL626;else{_tmpC8C=_tmpC8B->f1;_tmpC8D=_tmpC8C.elt_typ;
_tmpC8E=_tmpC8C.ptr_atts;_tmpC8F=_tmpC8E.bounds;_tmpC90=_tmpC8E.zero_term;}}
_LL625:{struct _tuple0 _tmp14EF;struct _tuple0 _tmpC92=(_tmp14EF.f1=Cyc_Tcutil_compress(
_tmpC8D),((_tmp14EF.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpC8F),_tmp14EF)));void*_tmpC93;enum Cyc_Absyn_Sign
_tmpC95;enum Cyc_Absyn_Size_of _tmpC96;void*_tmpC97;_LL629: _tmpC93=_tmpC92.f1;{
struct Cyc_Absyn_IntType_struct*_tmpC94=(struct Cyc_Absyn_IntType_struct*)_tmpC93;
if(_tmpC94->tag != 6)goto _LL62B;else{_tmpC95=_tmpC94->f1;if(_tmpC95 != Cyc_Absyn_None)
goto _LL62B;_tmpC96=_tmpC94->f2;if(_tmpC96 != Cyc_Absyn_Char_sz)goto _LL62B;}};
_tmpC97=_tmpC92.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpC98=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC97;if(_tmpC98->tag != 0)goto _LL62B;};_LL62A: goto _LL628;_LL62B:;_LL62C:{const
char*_tmp14F2;void*_tmp14F1;(_tmp14F1=0,Cyc_Tcutil_terr(loc,((_tmp14F2="format descriptor is not a char ? type",
_tag_dyneither(_tmp14F2,sizeof(char),39))),_tag_dyneither(_tmp14F1,sizeof(void*),
0)));}goto _LL628;_LL628:;}goto _LL623;_LL626:;_LL627:{const char*_tmp14F5;void*
_tmp14F4;(_tmp14F4=0,Cyc_Tcutil_terr(loc,((_tmp14F5="format descriptor is not a char ? type",
_tag_dyneither(_tmp14F5,sizeof(char),39))),_tag_dyneither(_tmp14F4,sizeof(void*),
0)));}goto _LL623;_LL623:;}if(fmt_arg_start != 0){int problem;{struct _tuple32
_tmp14F6;struct _tuple32 _tmpC9E=(_tmp14F6.f1=ft,((_tmp14F6.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmpB65))->type)),
_tmp14F6)));enum Cyc_Absyn_Format_Type _tmpC9F;void*_tmpCA0;struct Cyc_Absyn_DatatypeInfo
_tmpCA2;union Cyc_Absyn_DatatypeInfoU _tmpCA3;struct Cyc_Absyn_Datatypedecl**
_tmpCA4;struct Cyc_Absyn_Datatypedecl*_tmpCA5;enum Cyc_Absyn_Format_Type _tmpCA6;
void*_tmpCA7;struct Cyc_Absyn_DatatypeInfo _tmpCA9;union Cyc_Absyn_DatatypeInfoU
_tmpCAA;struct Cyc_Absyn_Datatypedecl**_tmpCAB;struct Cyc_Absyn_Datatypedecl*
_tmpCAC;_LL62E: _tmpC9F=_tmpC9E.f1;if(_tmpC9F != Cyc_Absyn_Printf_ft)goto _LL630;
_tmpCA0=_tmpC9E.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpCA1=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCA0;if(_tmpCA1->tag != 3)goto _LL630;else{_tmpCA2=_tmpCA1->f1;_tmpCA3=_tmpCA2.datatype_info;
if((_tmpCA3.KnownDatatype).tag != 2)goto _LL630;_tmpCA4=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCA3.KnownDatatype).val;_tmpCA5=*_tmpCA4;}};_LL62F: problem=Cyc_Absyn_qvar_cmp(
_tmpCA5->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL62D;_LL630: _tmpCA6=
_tmpC9E.f1;if(_tmpCA6 != Cyc_Absyn_Scanf_ft)goto _LL632;_tmpCA7=_tmpC9E.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpCA8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCA7;if(_tmpCA8->tag != 3)goto _LL632;else{_tmpCA9=_tmpCA8->f1;_tmpCAA=_tmpCA9.datatype_info;
if((_tmpCAA.KnownDatatype).tag != 2)goto _LL632;_tmpCAB=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCAA.KnownDatatype).val;_tmpCAC=*_tmpCAB;}};_LL631: problem=Cyc_Absyn_qvar_cmp(
_tmpCAC->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL62D;_LL632:;_LL633:
problem=1;goto _LL62D;_LL62D:;}if(problem){const char*_tmp14F9;void*_tmp14F8;(
_tmp14F8=0,Cyc_Tcutil_terr(loc,((_tmp14F9="format attribute and vararg types don't match",
_tag_dyneither(_tmp14F9,sizeof(char),46))),_tag_dyneither(_tmp14F8,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmpB66;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpCB0;void*_tmpCB1;void*_tmpCB2;struct _tuple0*_tmpCAF=(struct _tuple0*)
rpo->hd;_tmpCB0=*_tmpCAF;_tmpCB1=_tmpCB0.f1;_tmpCB2=_tmpCB0.f2;_tmpC5D=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5D,& Cyc_Tcutil_ek,_tmpCB1,1);_tmpC5D=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5D,& Cyc_Tcutil_trk,_tmpCB2,1);}}if(*_tmpB5F != 0)_tmpC5D=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpC5D,& Cyc_Tcutil_ek,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpB5F))->v,
1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=_tmpC5D.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpCB4;int _tmpCB5;struct
_tuple30 _tmpCB3=*((struct _tuple30*)tvs->hd);_tmpCB4=_tmpCB3.f1;_tmpCB5=_tmpCB3.f2;
if(!_tmpCB5)continue;{void*_tmpCB6=Cyc_Absyn_compress_kb(_tmpCB4->kind);struct
Cyc_Core_Opt*_tmpCB8;struct Cyc_Core_Opt**_tmpCB9;struct Cyc_Absyn_Kind*_tmpCBA;
struct Cyc_Absyn_Kind*_tmpCBC;struct Cyc_Absyn_Kind*_tmpCBE;struct Cyc_Absyn_Kind
_tmpCBF;enum Cyc_Absyn_KindQual _tmpCC0;struct Cyc_Absyn_Kind*_tmpCC2;struct Cyc_Absyn_Kind
_tmpCC3;enum Cyc_Absyn_KindQual _tmpCC4;struct Cyc_Core_Opt*_tmpCC6;struct Cyc_Core_Opt**
_tmpCC7;struct Cyc_Absyn_Kind*_tmpCC8;struct Cyc_Absyn_Kind _tmpCC9;enum Cyc_Absyn_KindQual
_tmpCCA;struct Cyc_Absyn_Kind*_tmpCCC;struct Cyc_Absyn_Kind _tmpCCD;enum Cyc_Absyn_KindQual
_tmpCCE;struct Cyc_Core_Opt*_tmpCD0;struct Cyc_Core_Opt**_tmpCD1;_LL635:{struct Cyc_Absyn_Less_kb_struct*
_tmpCB7=(struct Cyc_Absyn_Less_kb_struct*)_tmpCB6;if(_tmpCB7->tag != 2)goto _LL637;
else{_tmpCB8=_tmpCB7->f1;_tmpCB9=(struct Cyc_Core_Opt**)& _tmpCB7->f1;_tmpCBA=
_tmpCB7->f2;}}if(!(_tmpCBA->kind == Cyc_Absyn_RgnKind))goto _LL637;_LL636: if(
_tmpCBA->aliasqual == Cyc_Absyn_Top){*_tmpCB9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
_tmpCBC=_tmpCBA;goto _LL638;}*_tmpCB9=Cyc_Tcutil_kind_to_bound_opt(_tmpCBA);
_tmpCBC=_tmpCBA;goto _LL638;_LL637:{struct Cyc_Absyn_Eq_kb_struct*_tmpCBB=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCB6;if(_tmpCBB->tag != 0)goto _LL639;else{_tmpCBC=
_tmpCBB->f1;}}if(!(_tmpCBC->kind == Cyc_Absyn_RgnKind))goto _LL639;_LL638:{struct
Cyc_Absyn_AccessEff_struct*_tmp1508;struct Cyc_Absyn_VarType_struct*_tmp1507;
struct Cyc_Absyn_VarType_struct _tmp1506;struct Cyc_Absyn_AccessEff_struct _tmp1505;
struct Cyc_List_List*_tmp1504;effect=((_tmp1504=_cycalloc(sizeof(*_tmp1504)),((
_tmp1504->hd=(void*)((_tmp1508=_cycalloc(sizeof(*_tmp1508)),((_tmp1508[0]=((
_tmp1505.tag=23,((_tmp1505.f1=(void*)((void*)((_tmp1507=_cycalloc(sizeof(*
_tmp1507)),((_tmp1507[0]=((_tmp1506.tag=2,((_tmp1506.f1=_tmpCB4,_tmp1506)))),
_tmp1507))))),_tmp1505)))),_tmp1508)))),((_tmp1504->tl=effect,_tmp1504))))));}
goto _LL634;_LL639: {struct Cyc_Absyn_Less_kb_struct*_tmpCBD=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCB6;if(_tmpCBD->tag != 2)goto _LL63B;else{_tmpCBE=_tmpCBD->f2;_tmpCBF=*_tmpCBE;
_tmpCC0=_tmpCBF.kind;if(_tmpCC0 != Cyc_Absyn_IntKind)goto _LL63B;}}_LL63A: goto
_LL63C;_LL63B: {struct Cyc_Absyn_Eq_kb_struct*_tmpCC1=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCB6;if(_tmpCC1->tag != 0)goto _LL63D;else{_tmpCC2=_tmpCC1->f1;_tmpCC3=*_tmpCC2;
_tmpCC4=_tmpCC3.kind;if(_tmpCC4 != Cyc_Absyn_IntKind)goto _LL63D;}}_LL63C: goto
_LL634;_LL63D: {struct Cyc_Absyn_Less_kb_struct*_tmpCC5=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCB6;if(_tmpCC5->tag != 2)goto _LL63F;else{_tmpCC6=_tmpCC5->f1;_tmpCC7=(struct
Cyc_Core_Opt**)& _tmpCC5->f1;_tmpCC8=_tmpCC5->f2;_tmpCC9=*_tmpCC8;_tmpCCA=_tmpCC9.kind;
if(_tmpCCA != Cyc_Absyn_EffKind)goto _LL63F;}}_LL63E:*_tmpCC7=Cyc_Tcutil_kind_to_bound_opt(&
Cyc_Tcutil_ek);goto _LL640;_LL63F: {struct Cyc_Absyn_Eq_kb_struct*_tmpCCB=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpCB6;if(_tmpCCB->tag != 0)goto _LL641;else{_tmpCCC=
_tmpCCB->f1;_tmpCCD=*_tmpCCC;_tmpCCE=_tmpCCD.kind;if(_tmpCCE != Cyc_Absyn_EffKind)
goto _LL641;}}_LL640:{struct Cyc_Absyn_VarType_struct*_tmp150E;struct Cyc_Absyn_VarType_struct
_tmp150D;struct Cyc_List_List*_tmp150C;effect=((_tmp150C=_cycalloc(sizeof(*
_tmp150C)),((_tmp150C->hd=(void*)((_tmp150E=_cycalloc(sizeof(*_tmp150E)),((
_tmp150E[0]=((_tmp150D.tag=2,((_tmp150D.f1=_tmpCB4,_tmp150D)))),_tmp150E)))),((
_tmp150C->tl=effect,_tmp150C))))));}goto _LL634;_LL641: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpCCF=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpCB6;if(_tmpCCF->tag != 1)goto
_LL643;else{_tmpCD0=_tmpCCF->f1;_tmpCD1=(struct Cyc_Core_Opt**)& _tmpCCF->f1;}}
_LL642:{struct Cyc_Absyn_Less_kb_struct*_tmp1514;struct Cyc_Absyn_Less_kb_struct
_tmp1513;struct Cyc_Core_Opt*_tmp1512;*_tmpCD1=((_tmp1512=_cycalloc(sizeof(*
_tmp1512)),((_tmp1512->v=(void*)((_tmp1514=_cycalloc(sizeof(*_tmp1514)),((
_tmp1514[0]=((_tmp1513.tag=2,((_tmp1513.f1=0,((_tmp1513.f2=& Cyc_Tcutil_ak,
_tmp1513)))))),_tmp1514)))),_tmp1512))));}goto _LL644;_LL643:;_LL644:{struct Cyc_Absyn_RgnsEff_struct*
_tmp1523;struct Cyc_Absyn_VarType_struct*_tmp1522;struct Cyc_Absyn_VarType_struct
_tmp1521;struct Cyc_Absyn_RgnsEff_struct _tmp1520;struct Cyc_List_List*_tmp151F;
effect=((_tmp151F=_cycalloc(sizeof(*_tmp151F)),((_tmp151F->hd=(void*)((_tmp1523=
_cycalloc(sizeof(*_tmp1523)),((_tmp1523[0]=((_tmp1520.tag=25,((_tmp1520.f1=(void*)((
void*)((_tmp1522=_cycalloc(sizeof(*_tmp1522)),((_tmp1522[0]=((_tmp1521.tag=2,((
_tmp1521.f1=_tmpCB4,_tmp1521)))),_tmp1522))))),_tmp1520)))),_tmp1523)))),((
_tmp151F->tl=effect,_tmp151F))))));}goto _LL634;_LL634:;};}}{struct Cyc_List_List*
ts=_tmpC5D.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpCE3;int _tmpCE4;struct
_tuple31 _tmpCE2=*((struct _tuple31*)ts->hd);_tmpCE3=_tmpCE2.f1;_tmpCE4=_tmpCE2.f2;
if(!_tmpCE4)continue;{struct Cyc_Absyn_Kind*_tmpCE5=Cyc_Tcutil_typ_kind(_tmpCE3);
struct Cyc_Absyn_Kind _tmpCE6;enum Cyc_Absyn_KindQual _tmpCE7;struct Cyc_Absyn_Kind
_tmpCE8;enum Cyc_Absyn_KindQual _tmpCE9;struct Cyc_Absyn_Kind _tmpCEA;enum Cyc_Absyn_KindQual
_tmpCEB;_LL646: _tmpCE6=*_tmpCE5;_tmpCE7=_tmpCE6.kind;if(_tmpCE7 != Cyc_Absyn_RgnKind)
goto _LL648;_LL647:{struct Cyc_Absyn_AccessEff_struct*_tmp1529;struct Cyc_Absyn_AccessEff_struct
_tmp1528;struct Cyc_List_List*_tmp1527;effect=((_tmp1527=_cycalloc(sizeof(*
_tmp1527)),((_tmp1527->hd=(void*)((_tmp1529=_cycalloc(sizeof(*_tmp1529)),((
_tmp1529[0]=((_tmp1528.tag=23,((_tmp1528.f1=(void*)_tmpCE3,_tmp1528)))),_tmp1529)))),((
_tmp1527->tl=effect,_tmp1527))))));}goto _LL645;_LL648: _tmpCE8=*_tmpCE5;_tmpCE9=
_tmpCE8.kind;if(_tmpCE9 != Cyc_Absyn_EffKind)goto _LL64A;_LL649:{struct Cyc_List_List*
_tmp152A;effect=((_tmp152A=_cycalloc(sizeof(*_tmp152A)),((_tmp152A->hd=_tmpCE3,((
_tmp152A->tl=effect,_tmp152A))))));}goto _LL645;_LL64A: _tmpCEA=*_tmpCE5;_tmpCEB=
_tmpCEA.kind;if(_tmpCEB != Cyc_Absyn_IntKind)goto _LL64C;_LL64B: goto _LL645;_LL64C:;
_LL64D:{struct Cyc_Absyn_RgnsEff_struct*_tmp1530;struct Cyc_Absyn_RgnsEff_struct
_tmp152F;struct Cyc_List_List*_tmp152E;effect=((_tmp152E=_cycalloc(sizeof(*
_tmp152E)),((_tmp152E->hd=(void*)((_tmp1530=_cycalloc(sizeof(*_tmp1530)),((
_tmp1530[0]=((_tmp152F.tag=25,((_tmp152F.f1=(void*)_tmpCE3,_tmp152F)))),_tmp1530)))),((
_tmp152E->tl=effect,_tmp152E))))));}goto _LL645;_LL645:;};}}{struct Cyc_Absyn_JoinEff_struct*
_tmp1536;struct Cyc_Absyn_JoinEff_struct _tmp1535;struct Cyc_Core_Opt*_tmp1534;*
_tmpB5F=((_tmp1534=_cycalloc(sizeof(*_tmp1534)),((_tmp1534->v=(void*)((_tmp1536=
_cycalloc(sizeof(*_tmp1536)),((_tmp1536[0]=((_tmp1535.tag=24,((_tmp1535.f1=
effect,_tmp1535)))),_tmp1536)))),_tmp1534))));};}if(*_tmpB5D != 0){struct Cyc_List_List*
bs=*_tmpB5D;for(0;bs != 0;bs=bs->tl){void*_tmpCF6=Cyc_Absyn_compress_kb(((struct
Cyc_Absyn_Tvar*)bs->hd)->kind);struct Cyc_Core_Opt*_tmpCF8;struct Cyc_Core_Opt**
_tmpCF9;struct Cyc_Core_Opt*_tmpCFB;struct Cyc_Core_Opt**_tmpCFC;struct Cyc_Absyn_Kind*
_tmpCFD;struct Cyc_Absyn_Kind _tmpCFE;enum Cyc_Absyn_KindQual _tmpCFF;enum Cyc_Absyn_AliasQual
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
_tmpD35;struct Cyc_Absyn_Kind*_tmpD37;struct Cyc_Absyn_Kind _tmpD38;enum Cyc_Absyn_KindQual
_tmpD39;_LL64F: {struct Cyc_Absyn_Unknown_kb_struct*_tmpCF7=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpCF6;if(_tmpCF7->tag != 1)goto _LL651;else{_tmpCF8=_tmpCF7->f1;_tmpCF9=(struct
Cyc_Core_Opt**)& _tmpCF7->f1;}}_LL650:{const char*_tmp153A;void*_tmp1539[1];struct
Cyc_String_pa_struct _tmp1538;(_tmp1538.tag=0,((_tmp1538.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1539[0]=&
_tmp1538,Cyc_Tcutil_warn(loc,((_tmp153A="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp153A,sizeof(char),47))),_tag_dyneither(_tmp1539,sizeof(void*),
1)))))));}_tmpCFC=_tmpCF9;goto _LL652;_LL651: {struct Cyc_Absyn_Less_kb_struct*
_tmpCFA=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;if(_tmpCFA->tag != 2)goto _LL653;
else{_tmpCFB=_tmpCFA->f1;_tmpCFC=(struct Cyc_Core_Opt**)& _tmpCFA->f1;_tmpCFD=
_tmpCFA->f2;_tmpCFE=*_tmpCFD;_tmpCFF=_tmpCFE.kind;if(_tmpCFF != Cyc_Absyn_BoxKind)
goto _LL653;_tmpD00=_tmpCFE.aliasqual;if(_tmpD00 != Cyc_Absyn_Top)goto _LL653;}}
_LL652: _tmpD03=_tmpCFC;goto _LL654;_LL653: {struct Cyc_Absyn_Less_kb_struct*
_tmpD01=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;if(_tmpD01->tag != 2)goto _LL655;
else{_tmpD02=_tmpD01->f1;_tmpD03=(struct Cyc_Core_Opt**)& _tmpD01->f1;_tmpD04=
_tmpD01->f2;_tmpD05=*_tmpD04;_tmpD06=_tmpD05.kind;if(_tmpD06 != Cyc_Absyn_MemKind)
goto _LL655;_tmpD07=_tmpD05.aliasqual;if(_tmpD07 != Cyc_Absyn_Top)goto _LL655;}}
_LL654: _tmpD0A=_tmpD03;goto _LL656;_LL655: {struct Cyc_Absyn_Less_kb_struct*
_tmpD08=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;if(_tmpD08->tag != 2)goto _LL657;
else{_tmpD09=_tmpD08->f1;_tmpD0A=(struct Cyc_Core_Opt**)& _tmpD08->f1;_tmpD0B=
_tmpD08->f2;_tmpD0C=*_tmpD0B;_tmpD0D=_tmpD0C.kind;if(_tmpD0D != Cyc_Absyn_MemKind)
goto _LL657;_tmpD0E=_tmpD0C.aliasqual;if(_tmpD0E != Cyc_Absyn_Aliasable)goto _LL657;}}
_LL656: _tmpD11=_tmpD0A;goto _LL658;_LL657: {struct Cyc_Absyn_Less_kb_struct*
_tmpD0F=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;if(_tmpD0F->tag != 2)goto _LL659;
else{_tmpD10=_tmpD0F->f1;_tmpD11=(struct Cyc_Core_Opt**)& _tmpD0F->f1;_tmpD12=
_tmpD0F->f2;_tmpD13=*_tmpD12;_tmpD14=_tmpD13.kind;if(_tmpD14 != Cyc_Absyn_AnyKind)
goto _LL659;_tmpD15=_tmpD13.aliasqual;if(_tmpD15 != Cyc_Absyn_Top)goto _LL659;}}
_LL658: _tmpD18=_tmpD11;goto _LL65A;_LL659: {struct Cyc_Absyn_Less_kb_struct*
_tmpD16=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;if(_tmpD16->tag != 2)goto _LL65B;
else{_tmpD17=_tmpD16->f1;_tmpD18=(struct Cyc_Core_Opt**)& _tmpD16->f1;_tmpD19=
_tmpD16->f2;_tmpD1A=*_tmpD19;_tmpD1B=_tmpD1A.kind;if(_tmpD1B != Cyc_Absyn_AnyKind)
goto _LL65B;_tmpD1C=_tmpD1A.aliasqual;if(_tmpD1C != Cyc_Absyn_Aliasable)goto _LL65B;}}
_LL65A:*_tmpD18=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL64E;_LL65B: {
struct Cyc_Absyn_Less_kb_struct*_tmpD1D=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;
if(_tmpD1D->tag != 2)goto _LL65D;else{_tmpD1E=_tmpD1D->f1;_tmpD1F=(struct Cyc_Core_Opt**)&
_tmpD1D->f1;_tmpD20=_tmpD1D->f2;_tmpD21=*_tmpD20;_tmpD22=_tmpD21.kind;if(_tmpD22
!= Cyc_Absyn_MemKind)goto _LL65D;_tmpD23=_tmpD21.aliasqual;if(_tmpD23 != Cyc_Absyn_Unique)
goto _LL65D;}}_LL65C: _tmpD26=_tmpD1F;goto _LL65E;_LL65D: {struct Cyc_Absyn_Less_kb_struct*
_tmpD24=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;if(_tmpD24->tag != 2)goto _LL65F;
else{_tmpD25=_tmpD24->f1;_tmpD26=(struct Cyc_Core_Opt**)& _tmpD24->f1;_tmpD27=
_tmpD24->f2;_tmpD28=*_tmpD27;_tmpD29=_tmpD28.kind;if(_tmpD29 != Cyc_Absyn_AnyKind)
goto _LL65F;_tmpD2A=_tmpD28.aliasqual;if(_tmpD2A != Cyc_Absyn_Unique)goto _LL65F;}}
_LL65E:*_tmpD26=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL64E;_LL65F: {
struct Cyc_Absyn_Less_kb_struct*_tmpD2B=(struct Cyc_Absyn_Less_kb_struct*)_tmpCF6;
if(_tmpD2B->tag != 2)goto _LL661;else{_tmpD2C=_tmpD2B->f1;_tmpD2D=(struct Cyc_Core_Opt**)&
_tmpD2B->f1;_tmpD2E=_tmpD2B->f2;_tmpD2F=*_tmpD2E;_tmpD30=_tmpD2F.kind;if(_tmpD30
!= Cyc_Absyn_RgnKind)goto _LL661;_tmpD31=_tmpD2F.aliasqual;if(_tmpD31 != Cyc_Absyn_Top)
goto _LL661;}}_LL660:*_tmpD2D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto
_LL64E;_LL661: {struct Cyc_Absyn_Less_kb_struct*_tmpD32=(struct Cyc_Absyn_Less_kb_struct*)
_tmpCF6;if(_tmpD32->tag != 2)goto _LL663;else{_tmpD33=_tmpD32->f1;_tmpD34=(struct
Cyc_Core_Opt**)& _tmpD32->f1;_tmpD35=_tmpD32->f2;}}_LL662:*_tmpD34=Cyc_Tcutil_kind_to_bound_opt(
_tmpD35);goto _LL64E;_LL663: {struct Cyc_Absyn_Eq_kb_struct*_tmpD36=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpCF6;if(_tmpD36->tag != 0)goto _LL665;else{_tmpD37=_tmpD36->f1;_tmpD38=*_tmpD37;
_tmpD39=_tmpD38.kind;if(_tmpD39 != Cyc_Absyn_MemKind)goto _LL665;}}_LL664:{const
char*_tmp153D;void*_tmp153C;(_tmp153C=0,Cyc_Tcutil_terr(loc,((_tmp153D="functions can't abstract M types",
_tag_dyneither(_tmp153D,sizeof(char),33))),_tag_dyneither(_tmp153C,sizeof(void*),
0)));}goto _LL64E;_LL665:;_LL666: goto _LL64E;_LL64E:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
Cyc_Core_heap_region,_tmpC5D.kind_env,*_tmpB5D);_tmpC5D.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpC5D.r,_tmpC5D.free_vars,*_tmpB5D);{struct Cyc_List_List*tvs=_tmpC5D.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpD40;int _tmpD41;struct
_tuple30 _tmpD3F=*((struct _tuple30*)tvs->hd);_tmpD40=_tmpD3F.f1;_tmpD41=_tmpD3F.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpD40,_tmpD41);}}{struct Cyc_List_List*evs=_tmpC5D.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpD43;int _tmpD44;struct _tuple31 _tmpD42=*((struct _tuple31*)evs->hd);
_tmpD43=_tmpD42.f1;_tmpD44=_tmpD42.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpD43,_tmpD44);}};}_npop_handler(0);goto _LL56E;;
_pop_region(newr);};}_LL58D: {struct Cyc_Absyn_TupleType_struct*_tmpB68=(struct
Cyc_Absyn_TupleType_struct*)_tmpB21;if(_tmpB68->tag != 10)goto _LL58F;else{_tmpB69=
_tmpB68->f1;}}_LL58E: for(0;_tmpB69 != 0;_tmpB69=_tmpB69->tl){struct _tuple12*
_tmpD46=(struct _tuple12*)_tmpB69->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_tmk,(*_tmpD46).f2,1);((*_tmpD46).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpD46).f1).print_const,(*_tmpD46).f2);}goto _LL56E;_LL58F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpB6A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpB21;if(_tmpB6A->tag != 12)goto
_LL591;else{_tmpB6B=_tmpB6A->f1;_tmpB6C=_tmpB6A->f2;}}_LL590: {struct
_RegionHandle _tmpD47=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=&
_tmpD47;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;for(0;_tmpB6C
!= 0;_tmpB6C=_tmpB6C->tl){struct Cyc_Absyn_Aggrfield _tmpD49;struct _dyneither_ptr*
_tmpD4A;struct Cyc_Absyn_Tqual _tmpD4B;struct Cyc_Absyn_Tqual*_tmpD4C;void*_tmpD4D;
struct Cyc_Absyn_Exp*_tmpD4E;struct Cyc_List_List*_tmpD4F;struct Cyc_Absyn_Aggrfield*
_tmpD48=(struct Cyc_Absyn_Aggrfield*)_tmpB6C->hd;_tmpD49=*_tmpD48;_tmpD4A=_tmpD49.name;
_tmpD4B=_tmpD49.tq;_tmpD4C=(struct Cyc_Absyn_Tqual*)&(*_tmpD48).tq;_tmpD4D=
_tmpD49.type;_tmpD4E=_tmpD49.width;_tmpD4F=_tmpD49.attributes;if(((int(*)(int(*
compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,
struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpD4A)){const
char*_tmp1541;void*_tmp1540[1];struct Cyc_String_pa_struct _tmp153F;(_tmp153F.tag=
0,((_tmp153F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpD4A),((
_tmp1540[0]=& _tmp153F,Cyc_Tcutil_terr(loc,((_tmp1541="duplicate field %s",
_tag_dyneither(_tmp1541,sizeof(char),19))),_tag_dyneither(_tmp1540,sizeof(void*),
1)))))));}{const char*_tmp1542;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpD4A,((
_tmp1542="",_tag_dyneither(_tmp1542,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp1543;prev_fields=((_tmp1543=_region_malloc(aprev_rgn,sizeof(*_tmp1543)),((
_tmp1543->hd=_tmpD4A,((_tmp1543->tl=prev_fields,_tmp1543))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpD4D,1);_tmpD4C->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpD4C->print_const,_tmpD4D);if(_tmpB6B == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpD4D)){const char*_tmp1547;void*_tmp1546[1];struct Cyc_String_pa_struct _tmp1545;(
_tmp1545.tag=0,((_tmp1545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpD4A),((_tmp1546[0]=& _tmp1545,Cyc_Tcutil_warn(loc,((_tmp1547="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1547,sizeof(char),74))),_tag_dyneither(_tmp1546,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpD4D,_tmpD4E,_tmpD4A);Cyc_Tcutil_check_field_atts(
loc,_tmpD4A,_tmpD4F);}}_npop_handler(0);goto _LL56E;;_pop_region(aprev_rgn);}
_LL591: {struct Cyc_Absyn_AggrType_struct*_tmpB6D=(struct Cyc_Absyn_AggrType_struct*)
_tmpB21;if(_tmpB6D->tag != 11)goto _LL593;else{_tmpB6E=_tmpB6D->f1;_tmpB6F=_tmpB6E.aggr_info;
_tmpB70=(union Cyc_Absyn_AggrInfoU*)&(_tmpB6D->f1).aggr_info;_tmpB71=_tmpB6E.targs;
_tmpB72=(struct Cyc_List_List**)&(_tmpB6D->f1).targs;}}_LL592:{union Cyc_Absyn_AggrInfoU
_tmpD58=*_tmpB70;struct _tuple4 _tmpD59;enum Cyc_Absyn_AggrKind _tmpD5A;struct
_tuple2*_tmpD5B;struct Cyc_Core_Opt*_tmpD5C;struct Cyc_Absyn_Aggrdecl**_tmpD5D;
struct Cyc_Absyn_Aggrdecl*_tmpD5E;_LL668: if((_tmpD58.UnknownAggr).tag != 1)goto
_LL66A;_tmpD59=(struct _tuple4)(_tmpD58.UnknownAggr).val;_tmpD5A=_tmpD59.f1;
_tmpD5B=_tmpD59.f2;_tmpD5C=_tmpD59.f3;_LL669: {struct Cyc_Absyn_Aggrdecl**adp;{
struct _handler_cons _tmpD5F;_push_handler(& _tmpD5F);{int _tmpD61=0;if(setjmp(
_tmpD5F.handler))_tmpD61=1;if(!_tmpD61){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,
_tmpD5B);{struct Cyc_Absyn_Aggrdecl*_tmpD62=*adp;if(_tmpD62->kind != _tmpD5A){if(
_tmpD62->kind == Cyc_Absyn_StructA){const char*_tmp154C;void*_tmp154B[2];struct Cyc_String_pa_struct
_tmp154A;struct Cyc_String_pa_struct _tmp1549;(_tmp1549.tag=0,((_tmp1549.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5B)),((
_tmp154A.tag=0,((_tmp154A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5B)),((_tmp154B[0]=& _tmp154A,((_tmp154B[1]=& _tmp1549,Cyc_Tcutil_terr(loc,((
_tmp154C="expecting struct %s instead of union %s",_tag_dyneither(_tmp154C,
sizeof(char),40))),_tag_dyneither(_tmp154B,sizeof(void*),2)))))))))))));}else{
const char*_tmp1551;void*_tmp1550[2];struct Cyc_String_pa_struct _tmp154F;struct Cyc_String_pa_struct
_tmp154E;(_tmp154E.tag=0,((_tmp154E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5B)),((_tmp154F.tag=0,((_tmp154F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5B)),((
_tmp1550[0]=& _tmp154F,((_tmp1550[1]=& _tmp154E,Cyc_Tcutil_terr(loc,((_tmp1551="expecting union %s instead of struct %s",
_tag_dyneither(_tmp1551,sizeof(char),40))),_tag_dyneither(_tmp1550,sizeof(void*),
2)))))))))))));}}if((unsigned int)_tmpD5C  && (int)_tmpD5C->v){if(!((unsigned int)
_tmpD62->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD62->impl))->tagged){
const char*_tmp1555;void*_tmp1554[1];struct Cyc_String_pa_struct _tmp1553;(_tmp1553.tag=
0,((_tmp1553.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5B)),((_tmp1554[0]=& _tmp1553,Cyc_Tcutil_terr(loc,((_tmp1555="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1555,sizeof(char),42))),_tag_dyneither(_tmp1554,sizeof(void*),
1)))))));}}*_tmpB70=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpD60=(void*)_exn_thrown;void*_tmpD6F=_tmpD60;_LL66D: {struct Cyc_Dict_Absent_struct*
_tmpD70=(struct Cyc_Dict_Absent_struct*)_tmpD6F;if(_tmpD70->tag != Cyc_Dict_Absent)
goto _LL66F;}_LL66E: {struct Cyc_Tcenv_Genv*_tmpD71=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1556;struct Cyc_Absyn_Aggrdecl*_tmpD72=(_tmp1556=
_cycalloc(sizeof(*_tmp1556)),((_tmp1556->kind=_tmpD5A,((_tmp1556->sc=Cyc_Absyn_Extern,((
_tmp1556->name=_tmpD5B,((_tmp1556->tvs=0,((_tmp1556->impl=0,((_tmp1556->attributes=
0,_tmp1556)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpD71,loc,_tmpD72);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpD5B);*_tmpB70=Cyc_Absyn_KnownAggr(adp);if(*_tmpB72 != 0){{const char*
_tmp155A;void*_tmp1559[1];struct Cyc_String_pa_struct _tmp1558;(_tmp1558.tag=0,((
_tmp1558.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpD5B)),((_tmp1559[0]=& _tmp1558,Cyc_Tcutil_terr(loc,((_tmp155A="declare parameterized type %s before using",
_tag_dyneither(_tmp155A,sizeof(char),43))),_tag_dyneither(_tmp1559,sizeof(void*),
1)))))));}return cvtenv;}goto _LL66C;}_LL66F:;_LL670:(void)_throw(_tmpD6F);_LL66C:;}};}
_tmpD5E=*adp;goto _LL66B;}_LL66A: if((_tmpD58.KnownAggr).tag != 2)goto _LL667;
_tmpD5D=(struct Cyc_Absyn_Aggrdecl**)(_tmpD58.KnownAggr).val;_tmpD5E=*_tmpD5D;
_LL66B: {struct Cyc_List_List*tvs=_tmpD5E->tvs;struct Cyc_List_List*ts=*_tmpB72;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpD77=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpD78=(void*)ts->hd;{struct _tuple0 _tmp155B;
struct _tuple0 _tmpD7A=(_tmp155B.f1=Cyc_Absyn_compress_kb(_tmpD77->kind),((
_tmp155B.f2=_tmpD78,_tmp155B)));void*_tmpD7B;void*_tmpD7D;struct Cyc_Absyn_Tvar*
_tmpD7F;_LL672: _tmpD7B=_tmpD7A.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpD7C=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpD7B;if(_tmpD7C->tag != 1)goto _LL674;};
_tmpD7D=_tmpD7A.f2;{struct Cyc_Absyn_VarType_struct*_tmpD7E=(struct Cyc_Absyn_VarType_struct*)
_tmpD7D;if(_tmpD7E->tag != 2)goto _LL674;else{_tmpD7F=_tmpD7E->f1;}};_LL673: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpD7F);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpD7F,1);continue;_LL674:;_LL675: goto _LL671;_LL671:;}{
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);};}if(ts != 0){const char*_tmp155F;void*_tmp155E[1];struct Cyc_String_pa_struct
_tmp155D;(_tmp155D.tag=0,((_tmp155D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpD5E->name)),((_tmp155E[0]=& _tmp155D,
Cyc_Tcutil_terr(loc,((_tmp155F="too many parameters for type %s",_tag_dyneither(
_tmp155F,sizeof(char),32))),_tag_dyneither(_tmp155E,sizeof(void*),1)))))));}if(
tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,0);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp1560;
hidden_ts=((_tmp1560=_cycalloc(sizeof(*_tmp1560)),((_tmp1560->hd=e,((_tmp1560->tl=
hidden_ts,_tmp1560))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmpB72=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(*_tmpB72,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL667:;}goto _LL56E;_LL593: {struct Cyc_Absyn_TypedefType_struct*
_tmpB73=(struct Cyc_Absyn_TypedefType_struct*)_tmpB21;if(_tmpB73->tag != 17)goto
_LL595;else{_tmpB74=_tmpB73->f1;_tmpB75=_tmpB73->f2;_tmpB76=(struct Cyc_List_List**)&
_tmpB73->f2;_tmpB77=_tmpB73->f3;_tmpB78=(struct Cyc_Absyn_Typedefdecl**)& _tmpB73->f3;
_tmpB79=_tmpB73->f4;_tmpB7A=(void***)& _tmpB73->f4;}}_LL594: {struct Cyc_List_List*
targs=*_tmpB76;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpD84;
_push_handler(& _tmpD84);{int _tmpD86=0;if(setjmp(_tmpD84.handler))_tmpD86=1;if(!
_tmpD86){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpB74);;_pop_handler();}else{
void*_tmpD85=(void*)_exn_thrown;void*_tmpD88=_tmpD85;_LL677: {struct Cyc_Dict_Absent_struct*
_tmpD89=(struct Cyc_Dict_Absent_struct*)_tmpD88;if(_tmpD89->tag != Cyc_Dict_Absent)
goto _LL679;}_LL678:{const char*_tmp1564;void*_tmp1563[1];struct Cyc_String_pa_struct
_tmp1562;(_tmp1562.tag=0,((_tmp1562.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB74)),((_tmp1563[0]=& _tmp1562,Cyc_Tcutil_terr(
loc,((_tmp1564="unbound typedef name %s",_tag_dyneither(_tmp1564,sizeof(char),24))),
_tag_dyneither(_tmp1563,sizeof(void*),1)))))));}return cvtenv;_LL679:;_LL67A:(
void)_throw(_tmpD88);_LL676:;}};}*_tmpB78=(struct Cyc_Absyn_Typedefdecl*)td;
_tmpB74[0]=(td->name)[0];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*ts=
targs;struct _RegionHandle _tmpD8D=_new_region("temp");struct _RegionHandle*temp=&
_tmpD8D;_push_region(temp);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(
ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct
Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,(struct Cyc_Absyn_Typedefdecl*)
td);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);Cyc_Tcutil_check_no_qual(
loc,(void*)ts->hd);{struct _tuple16*_tmp1567;struct Cyc_List_List*_tmp1566;inst=((
_tmp1566=_region_malloc(temp,sizeof(*_tmp1566)),((_tmp1566->hd=((_tmp1567=
_region_malloc(temp,sizeof(*_tmp1567)),((_tmp1567->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmp1567->f2=(void*)ts->hd,_tmp1567)))))),((_tmp1566->tl=inst,_tmp1566))))));};}
if(ts != 0){const char*_tmp156B;void*_tmp156A[1];struct Cyc_String_pa_struct
_tmp1569;(_tmp1569.tag=0,((_tmp1569.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB74)),((_tmp156A[0]=& _tmp1569,Cyc_Tcutil_terr(
loc,((_tmp156B="too many parameters for typedef %s",_tag_dyneither(_tmp156B,
sizeof(char),35))),_tag_dyneither(_tmp156A,sizeof(void*),1)))))));}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Kind*
k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,
expected_kind,(struct Cyc_Absyn_Typedefdecl*)td);void*e=Cyc_Absyn_new_evar(0,0);{
struct Cyc_List_List*_tmp156C;hidden_ts=((_tmp156C=_cycalloc(sizeof(*_tmp156C)),((
_tmp156C->hd=e,((_tmp156C->tl=hidden_ts,_tmp156C))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,e,1);{struct _tuple16*_tmp156F;struct Cyc_List_List*_tmp156E;inst=(
struct Cyc_List_List*)((_tmp156E=_cycalloc(sizeof(*_tmp156E)),((_tmp156E->hd=(
struct _tuple16*)((_tmp156F=_cycalloc(sizeof(*_tmp156F)),((_tmp156F->f1=(struct
Cyc_Absyn_Tvar*)tvs->hd,((_tmp156F->f2=e,_tmp156F)))))),((_tmp156E->tl=inst,
_tmp156E))))));};}*_tmpB76=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}if(td->defn != 0){void*
new_typ=Cyc_Tcutil_rsubstitute(temp,inst,(void*)((struct Cyc_Core_Opt*)
_check_null(td->defn))->v);void**_tmp1570;*_tmpB7A=((_tmp1570=_cycalloc(sizeof(*
_tmp1570)),((_tmp1570[0]=new_typ,_tmp1570))));}}_npop_handler(0);goto _LL56E;;
_pop_region(temp);};}_LL595: {struct Cyc_Absyn_RefCntRgn_struct*_tmpB7B=(struct
Cyc_Absyn_RefCntRgn_struct*)_tmpB21;if(_tmpB7B->tag != 22)goto _LL597;}_LL596: goto
_LL598;_LL597: {struct Cyc_Absyn_UniqueRgn_struct*_tmpB7C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpB21;if(_tmpB7C->tag != 21)goto _LL599;}_LL598: goto _LL59A;_LL599: {struct Cyc_Absyn_HeapRgn_struct*
_tmpB7D=(struct Cyc_Absyn_HeapRgn_struct*)_tmpB21;if(_tmpB7D->tag != 20)goto _LL59B;}
_LL59A: goto _LL56E;_LL59B: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB7E=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmpB21;if(_tmpB7E->tag != 15)goto _LL59D;else{
_tmpB7F=(void*)_tmpB7E->f1;}}_LL59C: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_trk,_tmpB7F,1);goto _LL56E;_LL59D: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB80=(struct Cyc_Absyn_DynRgnType_struct*)_tmpB21;if(_tmpB80->tag != 16)goto
_LL59F;else{_tmpB81=(void*)_tmpB80->f1;_tmpB82=(void*)_tmpB80->f2;}}_LL59E:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB81,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpB82,1);
goto _LL56E;_LL59F: {struct Cyc_Absyn_AccessEff_struct*_tmpB83=(struct Cyc_Absyn_AccessEff_struct*)
_tmpB21;if(_tmpB83->tag != 23)goto _LL5A1;else{_tmpB84=(void*)_tmpB83->f1;}}_LL5A0:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpB84,1);
goto _LL56E;_LL5A1: {struct Cyc_Absyn_RgnsEff_struct*_tmpB85=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpB21;if(_tmpB85->tag != 25)goto _LL5A3;else{_tmpB86=(void*)_tmpB85->f1;}}_LL5A2:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB86,1);
goto _LL56E;_LL5A3: {struct Cyc_Absyn_JoinEff_struct*_tmpB87=(struct Cyc_Absyn_JoinEff_struct*)
_tmpB21;if(_tmpB87->tag != 24)goto _LL56E;else{_tmpB88=_tmpB87->f1;}}_LL5A4: for(0;
_tmpB88 != 0;_tmpB88=_tmpB88->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,& Cyc_Tcutil_ek,(void*)_tmpB88->hd,1);}goto _LL56E;_LL56E:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpD97=t;struct Cyc_Core_Opt*
_tmpD99;struct Cyc_Core_Opt*_tmpD9A;_LL67C: {struct Cyc_Absyn_Evar_struct*_tmpD98=(
struct Cyc_Absyn_Evar_struct*)_tmpD97;if(_tmpD98->tag != 1)goto _LL67E;else{_tmpD99=
_tmpD98->f1;_tmpD9A=_tmpD98->f2;}}_LL67D: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpD9B=_tmpD99;struct Cyc_Core_Opt _tmpD9C;struct Cyc_Absyn_Kind*_tmpD9D;_LL681:
if(_tmpD9B != 0)goto _LL683;_LL682:{const char*_tmp1571;s=((_tmp1571="kind=NULL ",
_tag_dyneither(_tmp1571,sizeof(char),11)));}goto _LL680;_LL683: if(_tmpD9B == 0)
goto _LL680;_tmpD9C=*_tmpD9B;_tmpD9D=(struct Cyc_Absyn_Kind*)_tmpD9C.v;_LL684:{
const char*_tmp1575;void*_tmp1574[1];struct Cyc_String_pa_struct _tmp1573;s=(struct
_dyneither_ptr)((_tmp1573.tag=0,((_tmp1573.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpD9D)),((_tmp1574[0]=& _tmp1573,Cyc_aprintf(((
_tmp1575="kind=%s ",_tag_dyneither(_tmp1575,sizeof(char),9))),_tag_dyneither(
_tmp1574,sizeof(void*),1))))))));}goto _LL680;_LL680:;}{struct Cyc_Core_Opt*
_tmpDA2=_tmpD9A;struct Cyc_Core_Opt _tmpDA3;void*_tmpDA4;_LL686: if(_tmpDA2 != 0)
goto _LL688;_LL687:{const char*_tmp1579;void*_tmp1578[1];struct Cyc_String_pa_struct
_tmp1577;s=(struct _dyneither_ptr)((_tmp1577.tag=0,((_tmp1577.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1578[0]=& _tmp1577,Cyc_aprintf(((
_tmp1579="%s ref=NULL",_tag_dyneither(_tmp1579,sizeof(char),12))),_tag_dyneither(
_tmp1578,sizeof(void*),1))))))));}goto _LL685;_LL688: if(_tmpDA2 == 0)goto _LL685;
_tmpDA3=*_tmpDA2;_tmpDA4=(void*)_tmpDA3.v;_LL689:{const char*_tmp157E;void*
_tmp157D[2];struct Cyc_String_pa_struct _tmp157C;struct Cyc_String_pa_struct
_tmp157B;s=(struct _dyneither_ptr)((_tmp157B.tag=0,((_tmp157B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpDA4)),((
_tmp157C.tag=0,((_tmp157C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp157D[0]=& _tmp157C,((_tmp157D[1]=& _tmp157B,Cyc_aprintf(((_tmp157E="%s ref=%s",
_tag_dyneither(_tmp157E,sizeof(char),10))),_tag_dyneither(_tmp157D,sizeof(void*),
2))))))))))))));}goto _LL685;_LL685:;}goto _LL67B;}_LL67E:;_LL67F: goto _LL67B;
_LL67B:;}{const char*_tmp1584;void*_tmp1583[3];struct Cyc_String_pa_struct _tmp1582;
struct Cyc_String_pa_struct _tmp1581;struct Cyc_String_pa_struct _tmp1580;(_tmp1580.tag=
0,((_tmp1580.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp1581.tag=0,((_tmp1581.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1582.tag=0,((
_tmp1582.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1583[0]=& _tmp1582,((_tmp1583[1]=& _tmp1581,((_tmp1583[2]=& _tmp1580,Cyc_Tcutil_terr(
loc,((_tmp1584="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp1584,sizeof(char),51))),_tag_dyneither(_tmp1583,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{
void*_tmpDB1=e->r;struct Cyc_List_List*_tmpDB7;struct Cyc_Absyn_Exp*_tmpDB9;struct
Cyc_Absyn_Exp*_tmpDBA;struct Cyc_Absyn_Exp*_tmpDBB;struct Cyc_Absyn_Exp*_tmpDBD;
struct Cyc_Absyn_Exp*_tmpDBE;struct Cyc_Absyn_Exp*_tmpDC0;struct Cyc_Absyn_Exp*
_tmpDC1;struct Cyc_Absyn_Exp*_tmpDC3;struct Cyc_Absyn_Exp*_tmpDC4;void*_tmpDC6;
struct Cyc_Absyn_Exp*_tmpDC7;void*_tmpDC9;void*_tmpDCB;void*_tmpDCD;struct Cyc_Absyn_Exp*
_tmpDCF;_LL68B: {struct Cyc_Absyn_Const_e_struct*_tmpDB2=(struct Cyc_Absyn_Const_e_struct*)
_tmpDB1;if(_tmpDB2->tag != 0)goto _LL68D;}_LL68C: goto _LL68E;_LL68D: {struct Cyc_Absyn_Enum_e_struct*
_tmpDB3=(struct Cyc_Absyn_Enum_e_struct*)_tmpDB1;if(_tmpDB3->tag != 32)goto _LL68F;}
_LL68E: goto _LL690;_LL68F: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDB4=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDB1;if(_tmpDB4->tag != 33)goto _LL691;}_LL690: goto _LL692;_LL691: {struct Cyc_Absyn_Var_e_struct*
_tmpDB5=(struct Cyc_Absyn_Var_e_struct*)_tmpDB1;if(_tmpDB5->tag != 1)goto _LL693;}
_LL692: goto _LL68A;_LL693: {struct Cyc_Absyn_Primop_e_struct*_tmpDB6=(struct Cyc_Absyn_Primop_e_struct*)
_tmpDB1;if(_tmpDB6->tag != 3)goto _LL695;else{_tmpDB7=_tmpDB6->f2;}}_LL694: for(0;
_tmpDB7 != 0;_tmpDB7=_tmpDB7->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpDB7->hd,te,cvtenv);}goto _LL68A;_LL695: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDB8=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDB1;if(_tmpDB8->tag != 6)goto
_LL697;else{_tmpDB9=_tmpDB8->f1;_tmpDBA=_tmpDB8->f2;_tmpDBB=_tmpDB8->f3;}}_LL696:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDB9,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDBA,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDBB,te,
cvtenv);goto _LL68A;_LL697: {struct Cyc_Absyn_And_e_struct*_tmpDBC=(struct Cyc_Absyn_And_e_struct*)
_tmpDB1;if(_tmpDBC->tag != 7)goto _LL699;else{_tmpDBD=_tmpDBC->f1;_tmpDBE=_tmpDBC->f2;}}
_LL698: _tmpDC0=_tmpDBD;_tmpDC1=_tmpDBE;goto _LL69A;_LL699: {struct Cyc_Absyn_Or_e_struct*
_tmpDBF=(struct Cyc_Absyn_Or_e_struct*)_tmpDB1;if(_tmpDBF->tag != 8)goto _LL69B;
else{_tmpDC0=_tmpDBF->f1;_tmpDC1=_tmpDBF->f2;}}_LL69A: _tmpDC3=_tmpDC0;_tmpDC4=
_tmpDC1;goto _LL69C;_LL69B: {struct Cyc_Absyn_SeqExp_e_struct*_tmpDC2=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDB1;if(_tmpDC2->tag != 9)goto _LL69D;else{_tmpDC3=_tmpDC2->f1;_tmpDC4=_tmpDC2->f2;}}
_LL69C: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC3,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC4,te,cvtenv);goto _LL68A;_LL69D: {
struct Cyc_Absyn_Cast_e_struct*_tmpDC5=(struct Cyc_Absyn_Cast_e_struct*)_tmpDB1;
if(_tmpDC5->tag != 15)goto _LL69F;else{_tmpDC6=(void*)_tmpDC5->f1;_tmpDC7=_tmpDC5->f2;}}
_LL69E: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpDC7,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpDC6,1);goto
_LL68A;_LL69F: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDC8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDB1;if(_tmpDC8->tag != 20)goto _LL6A1;else{_tmpDC9=(void*)_tmpDC8->f1;}}_LL6A0:
_tmpDCB=_tmpDC9;goto _LL6A2;_LL6A1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpDCA=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDB1;if(_tmpDCA->tag != 18)goto _LL6A3;
else{_tmpDCB=(void*)_tmpDCA->f1;}}_LL6A2: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,& Cyc_Tcutil_tak,_tmpDCB,1);goto _LL68A;_LL6A3: {struct Cyc_Absyn_Valueof_e_struct*
_tmpDCC=(struct Cyc_Absyn_Valueof_e_struct*)_tmpDB1;if(_tmpDCC->tag != 39)goto
_LL6A5;else{_tmpDCD=(void*)_tmpDCC->f1;}}_LL6A4: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpDCD,1);goto _LL68A;_LL6A5: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDCE=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDB1;if(_tmpDCE->tag != 19)goto
_LL6A7;else{_tmpDCF=_tmpDCE->f1;}}_LL6A6: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpDCF,te,cvtenv);goto _LL68A;_LL6A7:;_LL6A8: {const char*_tmp1587;void*_tmp1586;(
_tmp1586=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1587="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp1587,sizeof(char),66))),_tag_dyneither(_tmp1586,sizeof(void*),
0)));}_LL68A:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*
expected_kind,void*t){struct Cyc_List_List*_tmpDD2=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(
loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;
vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpDD4;struct _tuple30 _tmpDD3=*((struct _tuple30*)
vs->hd);_tmpDD4=_tmpDD3.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpDD2,
_tmpDD4);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpDD6;struct _tuple31 _tmpDD5=*((struct _tuple31*)evs->hd);_tmpDD6=_tmpDD5.f1;{
void*_tmpDD7=Cyc_Tcutil_compress(_tmpDD6);struct Cyc_Core_Opt*_tmpDD9;struct Cyc_Core_Opt**
_tmpDDA;_LL6AA: {struct Cyc_Absyn_Evar_struct*_tmpDD8=(struct Cyc_Absyn_Evar_struct*)
_tmpDD7;if(_tmpDD8->tag != 1)goto _LL6AC;else{_tmpDD9=_tmpDD8->f4;_tmpDDA=(struct
Cyc_Core_Opt**)& _tmpDD8->f4;}}_LL6AB: if(*_tmpDDA == 0){struct Cyc_Core_Opt*
_tmp1588;*_tmpDDA=((_tmp1588=_cycalloc(sizeof(*_tmp1588)),((_tmp1588->v=_tmpDD2,
_tmp1588))));}else{struct Cyc_List_List*_tmpDDC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDDA))->v;struct Cyc_List_List*_tmpDDD=0;for(0;_tmpDDC != 0;
_tmpDDC=_tmpDDC->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpDD2,(struct Cyc_Absyn_Tvar*)_tmpDDC->hd)){struct Cyc_List_List*_tmp1589;
_tmpDDD=((_tmp1589=_cycalloc(sizeof(*_tmp1589)),((_tmp1589->hd=(struct Cyc_Absyn_Tvar*)
_tmpDDC->hd,((_tmp1589->tl=_tmpDDD,_tmp1589))))));}}{struct Cyc_Core_Opt*_tmp158A;*
_tmpDDA=((_tmp158A=_cycalloc(sizeof(*_tmp158A)),((_tmp158A->v=_tmpDDD,_tmp158A))));};}
goto _LL6A9;_LL6AC:;_LL6AD: goto _LL6A9;_LL6A9:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_List_List*_tmpDE0=Cyc_Tcenv_lookup_type_vars(te);struct
_RegionHandle _tmpDE1=_new_region("temp");struct _RegionHandle*temp=& _tmpDE1;
_push_region(temp);{struct Cyc_Tcutil_CVTEnv _tmp158B;struct Cyc_Tcutil_CVTEnv
_tmpDE2=Cyc_Tcutil_check_valid_type(loc,te,((_tmp158B.r=temp,((_tmp158B.kind_env=
_tmpDE0,((_tmp158B.free_vars=0,((_tmp158B.free_evars=0,((_tmp158B.generalize_evars=
generalize_evars,((_tmp158B.fn_result=0,_tmp158B)))))))))))),& Cyc_Tcutil_tmk,t);
struct Cyc_List_List*_tmpDE3=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(
temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,_tmpDE2.free_vars);
struct Cyc_List_List*_tmpDE4=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct
_tuple31*))Cyc_Core_fst,_tmpDE2.free_evars);if(_tmpDE0 != 0){struct Cyc_List_List*
_tmpDE5=0;{struct Cyc_List_List*_tmpDE6=_tmpDE3;for(0;(unsigned int)_tmpDE6;
_tmpDE6=_tmpDE6->tl){struct Cyc_Absyn_Tvar*_tmpDE7=(struct Cyc_Absyn_Tvar*)_tmpDE6->hd;
int found=0;{struct Cyc_List_List*_tmpDE8=_tmpDE0;for(0;(unsigned int)_tmpDE8;
_tmpDE8=_tmpDE8->tl){if(Cyc_Absyn_tvar_cmp(_tmpDE7,(struct Cyc_Absyn_Tvar*)
_tmpDE8->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*_tmp158C;
_tmpDE5=((_tmp158C=_region_malloc(temp,sizeof(*_tmp158C)),((_tmp158C->hd=(struct
Cyc_Absyn_Tvar*)_tmpDE6->hd,((_tmp158C->tl=_tmpDE5,_tmp158C))))));}}}_tmpDE3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDE5);}{
struct Cyc_List_List*x=_tmpDE3;for(0;x != 0;x=x->tl){void*_tmpDEA=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpDEC;struct Cyc_Core_Opt**
_tmpDED;struct Cyc_Core_Opt*_tmpDEF;struct Cyc_Core_Opt**_tmpDF0;struct Cyc_Absyn_Kind*
_tmpDF1;struct Cyc_Absyn_Kind _tmpDF2;enum Cyc_Absyn_KindQual _tmpDF3;enum Cyc_Absyn_AliasQual
_tmpDF4;struct Cyc_Core_Opt*_tmpDF6;struct Cyc_Core_Opt**_tmpDF7;struct Cyc_Absyn_Kind*
_tmpDF8;struct Cyc_Absyn_Kind _tmpDF9;enum Cyc_Absyn_KindQual _tmpDFA;enum Cyc_Absyn_AliasQual
_tmpDFB;struct Cyc_Core_Opt*_tmpDFD;struct Cyc_Core_Opt**_tmpDFE;struct Cyc_Absyn_Kind*
_tmpDFF;struct Cyc_Absyn_Kind _tmpE00;enum Cyc_Absyn_KindQual _tmpE01;enum Cyc_Absyn_AliasQual
_tmpE02;struct Cyc_Core_Opt*_tmpE04;struct Cyc_Core_Opt**_tmpE05;struct Cyc_Absyn_Kind*
_tmpE06;struct Cyc_Absyn_Kind _tmpE07;enum Cyc_Absyn_KindQual _tmpE08;enum Cyc_Absyn_AliasQual
_tmpE09;struct Cyc_Core_Opt*_tmpE0B;struct Cyc_Core_Opt**_tmpE0C;struct Cyc_Absyn_Kind*
_tmpE0D;struct Cyc_Absyn_Kind _tmpE0E;enum Cyc_Absyn_KindQual _tmpE0F;enum Cyc_Absyn_AliasQual
_tmpE10;struct Cyc_Core_Opt*_tmpE12;struct Cyc_Core_Opt**_tmpE13;struct Cyc_Absyn_Kind*
_tmpE14;struct Cyc_Absyn_Kind*_tmpE16;struct Cyc_Absyn_Kind _tmpE17;enum Cyc_Absyn_KindQual
_tmpE18;enum Cyc_Absyn_AliasQual _tmpE19;_LL6AF: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpDEB=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpDEA;if(_tmpDEB->tag != 1)goto
_LL6B1;else{_tmpDEC=_tmpDEB->f1;_tmpDED=(struct Cyc_Core_Opt**)& _tmpDEB->f1;}}
_LL6B0: _tmpDF0=_tmpDED;goto _LL6B2;_LL6B1: {struct Cyc_Absyn_Less_kb_struct*
_tmpDEE=(struct Cyc_Absyn_Less_kb_struct*)_tmpDEA;if(_tmpDEE->tag != 2)goto _LL6B3;
else{_tmpDEF=_tmpDEE->f1;_tmpDF0=(struct Cyc_Core_Opt**)& _tmpDEE->f1;_tmpDF1=
_tmpDEE->f2;_tmpDF2=*_tmpDF1;_tmpDF3=_tmpDF2.kind;if(_tmpDF3 != Cyc_Absyn_BoxKind)
goto _LL6B3;_tmpDF4=_tmpDF2.aliasqual;if(_tmpDF4 != Cyc_Absyn_Top)goto _LL6B3;}}
_LL6B2: _tmpDF7=_tmpDF0;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Less_kb_struct*
_tmpDF5=(struct Cyc_Absyn_Less_kb_struct*)_tmpDEA;if(_tmpDF5->tag != 2)goto _LL6B5;
else{_tmpDF6=_tmpDF5->f1;_tmpDF7=(struct Cyc_Core_Opt**)& _tmpDF5->f1;_tmpDF8=
_tmpDF5->f2;_tmpDF9=*_tmpDF8;_tmpDFA=_tmpDF9.kind;if(_tmpDFA != Cyc_Absyn_MemKind)
goto _LL6B5;_tmpDFB=_tmpDF9.aliasqual;if(_tmpDFB != Cyc_Absyn_Top)goto _LL6B5;}}
_LL6B4: _tmpDFE=_tmpDF7;goto _LL6B6;_LL6B5: {struct Cyc_Absyn_Less_kb_struct*
_tmpDFC=(struct Cyc_Absyn_Less_kb_struct*)_tmpDEA;if(_tmpDFC->tag != 2)goto _LL6B7;
else{_tmpDFD=_tmpDFC->f1;_tmpDFE=(struct Cyc_Core_Opt**)& _tmpDFC->f1;_tmpDFF=
_tmpDFC->f2;_tmpE00=*_tmpDFF;_tmpE01=_tmpE00.kind;if(_tmpE01 != Cyc_Absyn_MemKind)
goto _LL6B7;_tmpE02=_tmpE00.aliasqual;if(_tmpE02 != Cyc_Absyn_Aliasable)goto _LL6B7;}}
_LL6B6:*_tmpDFE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6AE;_LL6B7: {
struct Cyc_Absyn_Less_kb_struct*_tmpE03=(struct Cyc_Absyn_Less_kb_struct*)_tmpDEA;
if(_tmpE03->tag != 2)goto _LL6B9;else{_tmpE04=_tmpE03->f1;_tmpE05=(struct Cyc_Core_Opt**)&
_tmpE03->f1;_tmpE06=_tmpE03->f2;_tmpE07=*_tmpE06;_tmpE08=_tmpE07.kind;if(_tmpE08
!= Cyc_Absyn_MemKind)goto _LL6B9;_tmpE09=_tmpE07.aliasqual;if(_tmpE09 != Cyc_Absyn_Unique)
goto _LL6B9;}}_LL6B8:*_tmpE05=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto
_LL6AE;_LL6B9: {struct Cyc_Absyn_Less_kb_struct*_tmpE0A=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDEA;if(_tmpE0A->tag != 2)goto _LL6BB;else{_tmpE0B=_tmpE0A->f1;_tmpE0C=(struct
Cyc_Core_Opt**)& _tmpE0A->f1;_tmpE0D=_tmpE0A->f2;_tmpE0E=*_tmpE0D;_tmpE0F=_tmpE0E.kind;
if(_tmpE0F != Cyc_Absyn_RgnKind)goto _LL6BB;_tmpE10=_tmpE0E.aliasqual;if(_tmpE10 != 
Cyc_Absyn_Top)goto _LL6BB;}}_LL6BA:*_tmpE0C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);
goto _LL6AE;_LL6BB: {struct Cyc_Absyn_Less_kb_struct*_tmpE11=(struct Cyc_Absyn_Less_kb_struct*)
_tmpDEA;if(_tmpE11->tag != 2)goto _LL6BD;else{_tmpE12=_tmpE11->f1;_tmpE13=(struct
Cyc_Core_Opt**)& _tmpE11->f1;_tmpE14=_tmpE11->f2;}}_LL6BC:*_tmpE13=Cyc_Tcutil_kind_to_bound_opt(
_tmpE14);goto _LL6AE;_LL6BD: {struct Cyc_Absyn_Eq_kb_struct*_tmpE15=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpDEA;if(_tmpE15->tag != 0)goto _LL6BF;else{_tmpE16=_tmpE15->f1;_tmpE17=*_tmpE16;
_tmpE18=_tmpE17.kind;if(_tmpE18 != Cyc_Absyn_MemKind)goto _LL6BF;_tmpE19=_tmpE17.aliasqual;}}
_LL6BE:{const char*_tmp1593;void*_tmp1592[2];struct Cyc_String_pa_struct _tmp1591;
struct Cyc_Absyn_Kind*_tmp1590;struct Cyc_String_pa_struct _tmp158F;(_tmp158F.tag=0,((
_tmp158F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(((
_tmp1590=_cycalloc_atomic(sizeof(*_tmp1590)),((_tmp1590->kind=Cyc_Absyn_MemKind,((
_tmp1590->aliasqual=_tmpE19,_tmp1590)))))))),((_tmp1591.tag=0,((_tmp1591.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmp1592[0]=& _tmp1591,((_tmp1592[1]=& _tmp158F,Cyc_Tcutil_terr(loc,((
_tmp1593="type variable %s cannot have kind %s",_tag_dyneither(_tmp1593,sizeof(
char),37))),_tag_dyneither(_tmp1592,sizeof(void*),2)))))))))))));}goto _LL6AE;
_LL6BF:;_LL6C0: goto _LL6AE;_LL6AE:;}}if(_tmpDE3 != 0  || _tmpDE4 != 0){{void*_tmpE1F=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpE21;struct Cyc_List_List*_tmpE22;
struct Cyc_List_List**_tmpE23;struct Cyc_Core_Opt*_tmpE24;struct Cyc_Absyn_Tqual
_tmpE25;void*_tmpE26;struct Cyc_List_List*_tmpE27;int _tmpE28;struct Cyc_Absyn_VarargInfo*
_tmpE29;struct Cyc_List_List*_tmpE2A;struct Cyc_List_List*_tmpE2B;_LL6C2: {struct
Cyc_Absyn_FnType_struct*_tmpE20=(struct Cyc_Absyn_FnType_struct*)_tmpE1F;if(
_tmpE20->tag != 9)goto _LL6C4;else{_tmpE21=_tmpE20->f1;_tmpE22=_tmpE21.tvars;
_tmpE23=(struct Cyc_List_List**)&(_tmpE20->f1).tvars;_tmpE24=_tmpE21.effect;
_tmpE25=_tmpE21.ret_tqual;_tmpE26=_tmpE21.ret_typ;_tmpE27=_tmpE21.args;_tmpE28=
_tmpE21.c_varargs;_tmpE29=_tmpE21.cyc_varargs;_tmpE2A=_tmpE21.rgn_po;_tmpE2B=
_tmpE21.attributes;}}_LL6C3: if(*_tmpE23 == 0){*_tmpE23=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmpDE3);_tmpDE3=0;}goto _LL6C1;_LL6C4:;
_LL6C5: goto _LL6C1;_LL6C1:;}if(_tmpDE3 != 0){const char*_tmp1597;void*_tmp1596[1];
struct Cyc_String_pa_struct _tmp1595;(_tmp1595.tag=0,((_tmp1595.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpDE3->hd)->name),((
_tmp1596[0]=& _tmp1595,Cyc_Tcutil_terr(loc,((_tmp1597="unbound type variable %s",
_tag_dyneither(_tmp1597,sizeof(char),25))),_tag_dyneither(_tmp1596,sizeof(void*),
1)))))));}if(_tmpDE4 != 0)for(0;_tmpDE4 != 0;_tmpDE4=_tmpDE4->tl){void*e=(void*)
_tmpDE4->hd;struct Cyc_Absyn_Kind*_tmpE2F=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind
_tmpE30;enum Cyc_Absyn_KindQual _tmpE31;enum Cyc_Absyn_AliasQual _tmpE32;struct Cyc_Absyn_Kind
_tmpE33;enum Cyc_Absyn_KindQual _tmpE34;enum Cyc_Absyn_AliasQual _tmpE35;struct Cyc_Absyn_Kind
_tmpE36;enum Cyc_Absyn_KindQual _tmpE37;enum Cyc_Absyn_AliasQual _tmpE38;struct Cyc_Absyn_Kind
_tmpE39;enum Cyc_Absyn_KindQual _tmpE3A;_LL6C7: _tmpE30=*_tmpE2F;_tmpE31=_tmpE30.kind;
if(_tmpE31 != Cyc_Absyn_RgnKind)goto _LL6C9;_tmpE32=_tmpE30.aliasqual;if(_tmpE32 != 
Cyc_Absyn_Unique)goto _LL6C9;_LL6C8: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp159A;void*_tmp1599;(_tmp1599=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp159A="can't unify evar with unique region!",
_tag_dyneither(_tmp159A,sizeof(char),37))),_tag_dyneither(_tmp1599,sizeof(void*),
0)));}goto _LL6C6;_LL6C9: _tmpE33=*_tmpE2F;_tmpE34=_tmpE33.kind;if(_tmpE34 != Cyc_Absyn_RgnKind)
goto _LL6CB;_tmpE35=_tmpE33.aliasqual;if(_tmpE35 != Cyc_Absyn_Aliasable)goto _LL6CB;
_LL6CA: goto _LL6CC;_LL6CB: _tmpE36=*_tmpE2F;_tmpE37=_tmpE36.kind;if(_tmpE37 != Cyc_Absyn_RgnKind)
goto _LL6CD;_tmpE38=_tmpE36.aliasqual;if(_tmpE38 != Cyc_Absyn_Top)goto _LL6CD;
_LL6CC: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp159D;
void*_tmp159C;(_tmp159C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp159D="can't unify evar with heap!",_tag_dyneither(
_tmp159D,sizeof(char),28))),_tag_dyneither(_tmp159C,sizeof(void*),0)));}goto
_LL6C6;_LL6CD: _tmpE39=*_tmpE2F;_tmpE3A=_tmpE39.kind;if(_tmpE3A != Cyc_Absyn_EffKind)
goto _LL6CF;_LL6CE: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15A0;void*_tmp159F;(_tmp159F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A0="can't unify evar with {}!",
_tag_dyneither(_tmp15A0,sizeof(char),26))),_tag_dyneither(_tmp159F,sizeof(void*),
0)));}goto _LL6C6;_LL6CF:;_LL6D0:{const char*_tmp15A5;void*_tmp15A4[2];struct Cyc_String_pa_struct
_tmp15A3;struct Cyc_String_pa_struct _tmp15A2;(_tmp15A2.tag=0,((_tmp15A2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15A3.tag=0,((_tmp15A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15A4[0]=& _tmp15A3,((_tmp15A4[1]=& _tmp15A2,Cyc_Tcutil_terr(loc,((
_tmp15A5="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15A5,sizeof(char),52))),_tag_dyneither(_tmp15A4,sizeof(void*),2)))))))))))));}
goto _LL6C6;_LL6C6:;}}};_pop_region(temp);}void Cyc_Tcutil_check_fndecl_valid_type(
unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmpE46=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmpE48;struct Cyc_List_List*_tmpE49;struct Cyc_Core_Opt*
_tmpE4A;struct Cyc_Absyn_Tqual _tmpE4B;void*_tmpE4C;struct Cyc_List_List*_tmpE4D;
_LL6D2: {struct Cyc_Absyn_FnType_struct*_tmpE47=(struct Cyc_Absyn_FnType_struct*)
_tmpE46;if(_tmpE47->tag != 9)goto _LL6D4;else{_tmpE48=_tmpE47->f1;_tmpE49=_tmpE48.tvars;
_tmpE4A=_tmpE48.effect;_tmpE4B=_tmpE48.ret_tqual;_tmpE4C=_tmpE48.ret_typ;_tmpE4D=
_tmpE48.args;}}_LL6D3: fd->tvs=_tmpE49;fd->effect=_tmpE4A;{struct Cyc_List_List*
_tmpE4E=fd->args;for(0;_tmpE4E != 0;(_tmpE4E=_tmpE4E->tl,_tmpE4D=_tmpE4D->tl)){(*((
struct _tuple26*)_tmpE4E->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmpE4D))->hd)).f2;(*((struct _tuple26*)_tmpE4E->hd)).f3=(*((struct
_tuple10*)_tmpE4D->hd)).f3;}}fd->ret_tqual=_tmpE4B;fd->ret_type=_tmpE4C;(fd->ret_tqual).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpE4C);
goto _LL6D1;_LL6D4:;_LL6D5: {const char*_tmp15A8;void*_tmp15A7;(_tmp15A7=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15A8="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp15A8,sizeof(char),38))),_tag_dyneither(_tmp15A7,sizeof(void*),
0)));}_LL6D1:;}{struct _RegionHandle _tmpE51=_new_region("r");struct _RegionHandle*
r=& _tmpE51;_push_region(r);{const char*_tmp15A9;Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct
_tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(
struct _tuple26*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,((_tmp15A9="function declaration has repeated parameter",
_tag_dyneither(_tmp15A9,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp15AA;fd->cached_typ=((
_tmp15AA=_cycalloc(sizeof(*_tmp15AA)),((_tmp15AA->v=t,_tmp15AA))));};;
_pop_region(r);};}void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int
allow_evars,void*t){struct _RegionHandle _tmpE54=_new_region("r");struct
_RegionHandle*r=& _tmpE54;_push_region(r);{struct Cyc_Tcutil_CVTEnv _tmp15AB;struct
Cyc_Tcutil_CVTEnv _tmpE55=Cyc_Tcutil_check_valid_type(loc,te,((_tmp15AB.r=r,((
_tmp15AB.kind_env=bound_tvars,((_tmp15AB.free_vars=0,((_tmp15AB.free_evars=0,((
_tmp15AB.generalize_evars=0,((_tmp15AB.fn_result=0,_tmp15AB)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpE56=Cyc_Tcutil_remove_bound_tvars(r,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct
_tuple30*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple30*))Cyc_Core_fst,_tmpE55.free_vars),bound_tvars);struct Cyc_List_List*
_tmpE57=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple31*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple31*))Cyc_Core_fst,
_tmpE55.free_evars);{struct Cyc_List_List*fs=_tmpE56;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpE58=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp15B0;void*_tmp15AF[2];struct Cyc_String_pa_struct _tmp15AE;struct Cyc_String_pa_struct
_tmp15AD;(_tmp15AD.tag=0,((_tmp15AD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp15AE.tag=0,((_tmp15AE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpE58),((_tmp15AF[0]=& _tmp15AE,((
_tmp15AF[1]=& _tmp15AD,Cyc_Tcutil_terr(loc,((_tmp15B0="unbound type variable %s in type %s",
_tag_dyneither(_tmp15B0,sizeof(char),36))),_tag_dyneither(_tmp15AF,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpE57 != 0)for(0;_tmpE57 != 0;_tmpE57=
_tmpE57->tl){void*e=(void*)_tmpE57->hd;struct Cyc_Absyn_Kind*_tmpE5D=Cyc_Tcutil_typ_kind(
e);struct Cyc_Absyn_Kind _tmpE5E;enum Cyc_Absyn_KindQual _tmpE5F;enum Cyc_Absyn_AliasQual
_tmpE60;struct Cyc_Absyn_Kind _tmpE61;enum Cyc_Absyn_KindQual _tmpE62;enum Cyc_Absyn_AliasQual
_tmpE63;struct Cyc_Absyn_Kind _tmpE64;enum Cyc_Absyn_KindQual _tmpE65;enum Cyc_Absyn_AliasQual
_tmpE66;struct Cyc_Absyn_Kind _tmpE67;enum Cyc_Absyn_KindQual _tmpE68;_LL6D7:
_tmpE5E=*_tmpE5D;_tmpE5F=_tmpE5E.kind;if(_tmpE5F != Cyc_Absyn_RgnKind)goto _LL6D9;
_tmpE60=_tmpE5E.aliasqual;if(_tmpE60 != Cyc_Absyn_Unique)goto _LL6D9;_LL6D8: if(!
Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp15B3;void*
_tmp15B2;(_tmp15B2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp15B3="can't unify evar with unique region!",
_tag_dyneither(_tmp15B3,sizeof(char),37))),_tag_dyneither(_tmp15B2,sizeof(void*),
0)));}goto _LL6D6;_LL6D9: _tmpE61=*_tmpE5D;_tmpE62=_tmpE61.kind;if(_tmpE62 != Cyc_Absyn_RgnKind)
goto _LL6DB;_tmpE63=_tmpE61.aliasqual;if(_tmpE63 != Cyc_Absyn_Aliasable)goto _LL6DB;
_LL6DA: goto _LL6DC;_LL6DB: _tmpE64=*_tmpE5D;_tmpE65=_tmpE64.kind;if(_tmpE65 != Cyc_Absyn_RgnKind)
goto _LL6DD;_tmpE66=_tmpE64.aliasqual;if(_tmpE66 != Cyc_Absyn_Top)goto _LL6DD;
_LL6DC: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp15B6;
void*_tmp15B5;(_tmp15B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp15B6="can't unify evar with heap!",_tag_dyneither(
_tmp15B6,sizeof(char),28))),_tag_dyneither(_tmp15B5,sizeof(void*),0)));}goto
_LL6D6;_LL6DD: _tmpE67=*_tmpE5D;_tmpE68=_tmpE67.kind;if(_tmpE68 != Cyc_Absyn_EffKind)
goto _LL6DF;_LL6DE: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmp15B9;void*_tmp15B8;(_tmp15B8=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp15B9="can't unify evar with {}!",
_tag_dyneither(_tmp15B9,sizeof(char),26))),_tag_dyneither(_tmp15B8,sizeof(void*),
0)));}goto _LL6D6;_LL6DF:;_LL6E0:{const char*_tmp15BE;void*_tmp15BD[2];struct Cyc_String_pa_struct
_tmp15BC;struct Cyc_String_pa_struct _tmp15BB;(_tmp15BB.tag=0,((_tmp15BB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp15BC.tag=0,((_tmp15BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp15BD[0]=& _tmp15BC,((_tmp15BD[1]=& _tmp15BB,Cyc_Tcutil_terr(loc,((
_tmp15BE="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp15BE,sizeof(char),52))),_tag_dyneither(_tmp15BD,sizeof(void*),2)))))))))))));}
goto _LL6D6;_LL6D6:;}};_pop_region(r);}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*
tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct
_dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0){const char*_tmp15C3;void*_tmp15C2[2];struct Cyc_String_pa_struct
_tmp15C1;struct Cyc_String_pa_struct _tmp15C0;(_tmp15C0.tag=0,((_tmp15C0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp15C1.tag=
0,((_tmp15C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp15C2[0]=&
_tmp15C1,((_tmp15C2[1]=& _tmp15C0,Cyc_Tcutil_terr(loc,((_tmp15C3="%s: %s",
_tag_dyneither(_tmp15C3,sizeof(char),7))),_tag_dyneither(_tmp15C2,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,unsigned int loc,struct _dyneither_ptr msg){((void(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,
struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp15C4;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp15C4="duplicate type variable",
_tag_dyneither(_tmp15C4,sizeof(char),24))));}struct _tuple33{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple34{struct Cyc_List_List*f1;void*f2;};struct _tuple35{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind
aggr_kind,struct Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*
_tmp15C5;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
sd_fields->hd)->name,((_tmp15C5="",_tag_dyneither(_tmp15C5,sizeof(char),1))))!= 
0){struct _tuple33*_tmp15C8;struct Cyc_List_List*_tmp15C7;fields=((_tmp15C7=
_cycalloc(sizeof(*_tmp15C7)),((_tmp15C7->hd=((_tmp15C8=_cycalloc(sizeof(*
_tmp15C8)),((_tmp15C8->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp15C8->f2=
0,_tmp15C8)))))),((_tmp15C7->tl=fields,_tmp15C7))))));}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{const char*_tmp15CA;const char*
_tmp15C9;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp15CA="struct",
_tag_dyneither(_tmp15CA,sizeof(char),7)):((_tmp15C9="union",_tag_dyneither(
_tmp15C9,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple34 _tmpE7D;struct Cyc_List_List*_tmpE7E;void*_tmpE7F;struct _tuple34*
_tmpE7C=(struct _tuple34*)des->hd;_tmpE7D=*_tmpE7C;_tmpE7E=_tmpE7D.f1;_tmpE7F=
_tmpE7D.f2;if(_tmpE7E == 0){struct Cyc_List_List*_tmpE80=fields;for(0;_tmpE80 != 0;
_tmpE80=_tmpE80->tl){if(!(*((struct _tuple33*)_tmpE80->hd)).f2){(*((struct
_tuple33*)_tmpE80->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp15D0;struct
Cyc_Absyn_FieldName_struct _tmp15CF;struct Cyc_List_List*_tmp15CE;(*((struct
_tuple34*)des->hd)).f1=(struct Cyc_List_List*)((_tmp15CE=_cycalloc(sizeof(*
_tmp15CE)),((_tmp15CE->hd=(void*)((_tmp15D0=_cycalloc(sizeof(*_tmp15D0)),((
_tmp15D0[0]=((_tmp15CF.tag=1,((_tmp15CF.f1=((*((struct _tuple33*)_tmpE80->hd)).f1)->name,
_tmp15CF)))),_tmp15D0)))),((_tmp15CE->tl=0,_tmp15CE))))));}{struct _tuple35*
_tmp15D3;struct Cyc_List_List*_tmp15D2;ans=((_tmp15D2=_region_malloc(rgn,sizeof(*
_tmp15D2)),((_tmp15D2->hd=((_tmp15D3=_region_malloc(rgn,sizeof(*_tmp15D3)),((
_tmp15D3->f1=(*((struct _tuple33*)_tmpE80->hd)).f1,((_tmp15D3->f2=_tmpE7F,
_tmp15D3)))))),((_tmp15D2->tl=ans,_tmp15D2))))));}break;}}if(_tmpE80 == 0){const
char*_tmp15D7;void*_tmp15D6[1];struct Cyc_String_pa_struct _tmp15D5;(_tmp15D5.tag=
0,((_tmp15D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp15D6[0]=& _tmp15D5,Cyc_Tcutil_terr(loc,((_tmp15D7="too many arguments to %s",
_tag_dyneither(_tmp15D7,sizeof(char),25))),_tag_dyneither(_tmp15D6,sizeof(void*),
1)))))));}}else{if(_tmpE7E->tl != 0){const char*_tmp15DA;void*_tmp15D9;(_tmp15D9=0,
Cyc_Tcutil_terr(loc,((_tmp15DA="multiple designators are not yet supported",
_tag_dyneither(_tmp15DA,sizeof(char),43))),_tag_dyneither(_tmp15D9,sizeof(void*),
0)));}else{void*_tmpE8B=(void*)_tmpE7E->hd;struct _dyneither_ptr*_tmpE8E;_LL6E2: {
struct Cyc_Absyn_ArrayElement_struct*_tmpE8C=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpE8B;if(_tmpE8C->tag != 0)goto _LL6E4;}_LL6E3:{const char*_tmp15DE;void*_tmp15DD[
1];struct Cyc_String_pa_struct _tmp15DC;(_tmp15DC.tag=0,((_tmp15DC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp15DD[0]=& _tmp15DC,Cyc_Tcutil_terr(
loc,((_tmp15DE="array designator used in argument to %s",_tag_dyneither(_tmp15DE,
sizeof(char),40))),_tag_dyneither(_tmp15DD,sizeof(void*),1)))))));}goto _LL6E1;
_LL6E4: {struct Cyc_Absyn_FieldName_struct*_tmpE8D=(struct Cyc_Absyn_FieldName_struct*)
_tmpE8B;if(_tmpE8D->tag != 1)goto _LL6E1;else{_tmpE8E=_tmpE8D->f1;}}_LL6E5: {
struct Cyc_List_List*_tmpE92=fields;for(0;_tmpE92 != 0;_tmpE92=_tmpE92->tl){if(Cyc_strptrcmp(
_tmpE8E,((*((struct _tuple33*)_tmpE92->hd)).f1)->name)== 0){if((*((struct _tuple33*)
_tmpE92->hd)).f2){const char*_tmp15E2;void*_tmp15E1[1];struct Cyc_String_pa_struct
_tmp15E0;(_tmp15E0.tag=0,((_tmp15E0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpE8E),((_tmp15E1[0]=& _tmp15E0,Cyc_Tcutil_terr(loc,((_tmp15E2="member %s has already been used as an argument",
_tag_dyneither(_tmp15E2,sizeof(char),47))),_tag_dyneither(_tmp15E1,sizeof(void*),
1)))))));}(*((struct _tuple33*)_tmpE92->hd)).f2=1;{struct _tuple35*_tmp15E5;struct
Cyc_List_List*_tmp15E4;ans=((_tmp15E4=_region_malloc(rgn,sizeof(*_tmp15E4)),((
_tmp15E4->hd=((_tmp15E5=_region_malloc(rgn,sizeof(*_tmp15E5)),((_tmp15E5->f1=(*((
struct _tuple33*)_tmpE92->hd)).f1,((_tmp15E5->f2=_tmpE7F,_tmp15E5)))))),((
_tmp15E4->tl=ans,_tmp15E4))))));}break;}}if(_tmpE92 == 0){const char*_tmp15E9;void*
_tmp15E8[1];struct Cyc_String_pa_struct _tmp15E7;(_tmp15E7.tag=0,((_tmp15E7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE8E),((_tmp15E8[0]=& _tmp15E7,
Cyc_Tcutil_terr(loc,((_tmp15E9="bad field designator %s",_tag_dyneither(_tmp15E9,
sizeof(char),24))),_tag_dyneither(_tmp15E8,sizeof(void*),1)))))));}goto _LL6E1;}
_LL6E1:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple33*)fields->hd)).f2){{const char*_tmp15EC;void*_tmp15EB;(
_tmp15EB=0,Cyc_Tcutil_terr(loc,((_tmp15EC="too few arguments to struct",
_tag_dyneither(_tmp15EC,sizeof(char),28))),_tag_dyneither(_tmp15EB,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple33*)fields->hd)).f2){if(found){const char*_tmp15EF;void*_tmp15EE;(_tmp15EE=
0,Cyc_Tcutil_terr(loc,((_tmp15EF="only one member of a union is allowed",
_tag_dyneither(_tmp15EF,sizeof(char),38))),_tag_dyneither(_tmp15EE,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp15F2;void*_tmp15F1;(_tmp15F1=0,Cyc_Tcutil_terr(
loc,((_tmp15F2="missing member for union",_tag_dyneither(_tmp15F2,sizeof(char),
25))),_tag_dyneither(_tmp15F1,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEA3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEA5;void*_tmpEA6;struct Cyc_Absyn_PtrAtts _tmpEA7;union Cyc_Absyn_Constraint*
_tmpEA8;_LL6E7: {struct Cyc_Absyn_PointerType_struct*_tmpEA4=(struct Cyc_Absyn_PointerType_struct*)
_tmpEA3;if(_tmpEA4->tag != 5)goto _LL6E9;else{_tmpEA5=_tmpEA4->f1;_tmpEA6=_tmpEA5.elt_typ;
_tmpEA7=_tmpEA5.ptr_atts;_tmpEA8=_tmpEA7.bounds;}}_LL6E8: {void*_tmpEA9=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpEA8);_LL6EC: {struct Cyc_Absyn_DynEither_b_struct*_tmpEAA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpEA9;if(_tmpEAA->tag != 0)goto _LL6EE;}_LL6ED:*elt_typ_dest=_tmpEA6;return 1;
_LL6EE:;_LL6EF: return 0;_LL6EB:;}_LL6E9:;_LL6EA: return 0;_LL6E6:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmpEAB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEAD;void*_tmpEAE;struct Cyc_Absyn_PtrAtts _tmpEAF;union Cyc_Absyn_Constraint*
_tmpEB0;_LL6F1: {struct Cyc_Absyn_PointerType_struct*_tmpEAC=(struct Cyc_Absyn_PointerType_struct*)
_tmpEAB;if(_tmpEAC->tag != 5)goto _LL6F3;else{_tmpEAD=_tmpEAC->f1;_tmpEAE=_tmpEAD.elt_typ;
_tmpEAF=_tmpEAD.ptr_atts;_tmpEB0=_tmpEAF.zero_term;}}_LL6F2:*elt_typ_dest=
_tmpEAE;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpEB0);_LL6F3:;_LL6F4: return 0;_LL6F0:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEB1=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpEB3;void*_tmpEB4;struct Cyc_Absyn_PtrAtts _tmpEB5;
union Cyc_Absyn_Constraint*_tmpEB6;union Cyc_Absyn_Constraint*_tmpEB7;struct Cyc_Absyn_ArrayInfo
_tmpEB9;void*_tmpEBA;struct Cyc_Absyn_Tqual _tmpEBB;struct Cyc_Absyn_Exp*_tmpEBC;
union Cyc_Absyn_Constraint*_tmpEBD;_LL6F6: {struct Cyc_Absyn_PointerType_struct*
_tmpEB2=(struct Cyc_Absyn_PointerType_struct*)_tmpEB1;if(_tmpEB2->tag != 5)goto
_LL6F8;else{_tmpEB3=_tmpEB2->f1;_tmpEB4=_tmpEB3.elt_typ;_tmpEB5=_tmpEB3.ptr_atts;
_tmpEB6=_tmpEB5.bounds;_tmpEB7=_tmpEB5.zero_term;}}_LL6F7: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEB7)){*ptr_type=t;*elt_type=
_tmpEB4;{void*_tmpEBE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpEB6);_LL6FD: {struct Cyc_Absyn_DynEither_b_struct*
_tmpEBF=(struct Cyc_Absyn_DynEither_b_struct*)_tmpEBE;if(_tmpEBF->tag != 0)goto
_LL6FF;}_LL6FE:*is_dyneither=1;goto _LL6FC;_LL6FF:;_LL700:*is_dyneither=0;goto
_LL6FC;_LL6FC:;}return 1;}else{return 0;}_LL6F8: {struct Cyc_Absyn_ArrayType_struct*
_tmpEB8=(struct Cyc_Absyn_ArrayType_struct*)_tmpEB1;if(_tmpEB8->tag != 8)goto
_LL6FA;else{_tmpEB9=_tmpEB8->f1;_tmpEBA=_tmpEB9.elt_type;_tmpEBB=_tmpEB9.tq;
_tmpEBC=_tmpEB9.num_elts;_tmpEBD=_tmpEB9.zero_term;}}_LL6F9: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpEBD)){*elt_type=_tmpEBA;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1607;struct Cyc_Absyn_PtrAtts
_tmp1606;struct Cyc_Absyn_Upper_b_struct _tmp1605;struct Cyc_Absyn_Upper_b_struct*
_tmp1604;struct Cyc_Absyn_PtrInfo _tmp1603;struct Cyc_Absyn_PointerType_struct*
_tmp1602;*ptr_type=(void*)((_tmp1602=_cycalloc(sizeof(*_tmp1602)),((_tmp1602[0]=((
_tmp1607.tag=5,((_tmp1607.f1=((_tmp1603.elt_typ=_tmpEBA,((_tmp1603.elt_tq=
_tmpEBB,((_tmp1603.ptr_atts=((_tmp1606.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1606.nullable=Cyc_Absyn_false_conref,((_tmp1606.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1604=_cycalloc(sizeof(*_tmp1604)),((
_tmp1604[0]=((_tmp1605.tag=1,((_tmp1605.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpEBC),_tmp1605)))),_tmp1604))))),((_tmp1606.zero_term=_tmpEBD,((_tmp1606.ptrloc=
0,_tmp1606)))))))))),_tmp1603)))))),_tmp1607)))),_tmp1602))));}return 1;}else{
return 0;}_LL6FA:;_LL6FB: return 0;_LL6F5:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpEC6=e1->r;
struct Cyc_Absyn_Exp*_tmpEC9;struct Cyc_Absyn_Exp*_tmpECB;struct Cyc_Absyn_Exp*
_tmpECD;struct Cyc_Absyn_Exp*_tmpECF;struct Cyc_Absyn_Exp*_tmpED1;struct Cyc_Absyn_Exp*
_tmpED3;_LL702: {struct Cyc_Absyn_Cast_e_struct*_tmpEC7=(struct Cyc_Absyn_Cast_e_struct*)
_tmpEC6;if(_tmpEC7->tag != 15)goto _LL704;}_LL703: {const char*_tmp160B;void*
_tmp160A[1];struct Cyc_String_pa_struct _tmp1609;(_tmp1609.tag=0,((_tmp1609.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp160A[0]=& _tmp1609,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp160B="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp160B,sizeof(char),29))),_tag_dyneither(_tmp160A,sizeof(void*),1)))))));}
_LL704: {struct Cyc_Absyn_Deref_e_struct*_tmpEC8=(struct Cyc_Absyn_Deref_e_struct*)
_tmpEC6;if(_tmpEC8->tag != 21)goto _LL706;else{_tmpEC9=_tmpEC8->f1;}}_LL705:
_tmpECB=_tmpEC9;goto _LL707;_LL706: {struct Cyc_Absyn_Subscript_e_struct*_tmpECA=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpEC6;if(_tmpECA->tag != 24)goto _LL708;
else{_tmpECB=_tmpECA->f1;}}_LL707: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpECB->topt))->v,ptr_type,is_dyneither,
elt_type);_LL708: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpECC=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpEC6;if(_tmpECC->tag != 23)goto _LL70A;else{_tmpECD=_tmpECC->f1;}}_LL709:
_tmpECF=_tmpECD;goto _LL70B;_LL70A: {struct Cyc_Absyn_AggrMember_e_struct*_tmpECE=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpEC6;if(_tmpECE->tag != 22)goto _LL70C;
else{_tmpECF=_tmpECE->f1;}}_LL70B: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpECF->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp160F;void*_tmp160E[1];struct Cyc_String_pa_struct _tmp160D;(_tmp160D.tag=
0,((_tmp160D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp160E[0]=& _tmp160D,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp160F="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp160F,sizeof(char),51))),_tag_dyneither(_tmp160E,sizeof(void*),
1)))))));}return 0;_LL70C: {struct Cyc_Absyn_Instantiate_e_struct*_tmpED0=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpEC6;if(_tmpED0->tag != 14)goto _LL70E;else{
_tmpED1=_tmpED0->f1;}}_LL70D: _tmpED3=_tmpED1;goto _LL70F;_LL70E: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpED2=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpEC6;if(_tmpED2->tag != 13)
goto _LL710;else{_tmpED3=_tmpED2->f1;}}_LL70F: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpED3->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1613;void*_tmp1612[1];struct Cyc_String_pa_struct
_tmp1611;(_tmp1611.tag=0,((_tmp1611.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1612[0]=& _tmp1611,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1613="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1613,sizeof(char),49))),_tag_dyneither(_tmp1612,sizeof(void*),
1)))))));}return 0;_LL710: {struct Cyc_Absyn_Var_e_struct*_tmpED4=(struct Cyc_Absyn_Var_e_struct*)
_tmpEC6;if(_tmpED4->tag != 1)goto _LL712;}_LL711: return 0;_LL712:;_LL713: {const
char*_tmp1617;void*_tmp1616[1];struct Cyc_String_pa_struct _tmp1615;(_tmp1615.tag=
0,((_tmp1615.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1616[0]=& _tmp1615,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1617="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1617,sizeof(char),39))),_tag_dyneither(_tmp1616,sizeof(void*),
1)))))));}_LL701:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)&
Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){void*_tmpEE1=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmpEE5;_LL715: {struct Cyc_Absyn_RefCntRgn_struct*
_tmpEE2=(struct Cyc_Absyn_RefCntRgn_struct*)_tmpEE1;if(_tmpEE2->tag != 22)goto
_LL717;}_LL716: return !must_be_unique;_LL717: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpEE3=(struct Cyc_Absyn_UniqueRgn_struct*)_tmpEE1;if(_tmpEE3->tag != 21)goto
_LL719;}_LL718: return 1;_LL719: {struct Cyc_Absyn_VarType_struct*_tmpEE4=(struct
Cyc_Absyn_VarType_struct*)_tmpEE1;if(_tmpEE4->tag != 2)goto _LL71B;else{_tmpEE5=
_tmpEE4->f1;}}_LL71A: {struct Cyc_Absyn_Kind _tmpEE7;enum Cyc_Absyn_KindQual
_tmpEE8;enum Cyc_Absyn_AliasQual _tmpEE9;struct Cyc_Absyn_Kind*_tmpEE6=Cyc_Tcutil_tvar_kind(
_tmpEE5,& Cyc_Tcutil_rk);_tmpEE7=*_tmpEE6;_tmpEE8=_tmpEE7.kind;_tmpEE9=_tmpEE7.aliasqual;
if(_tmpEE8 == Cyc_Absyn_RgnKind  && (_tmpEE9 == Cyc_Absyn_Unique  || _tmpEE9 == Cyc_Absyn_Top
 && !must_be_unique)){void*_tmpEEA=Cyc_Absyn_compress_kb(_tmpEE5->kind);struct
Cyc_Core_Opt*_tmpEEC;struct Cyc_Core_Opt**_tmpEED;struct Cyc_Absyn_Kind*_tmpEEE;
struct Cyc_Absyn_Kind _tmpEEF;enum Cyc_Absyn_KindQual _tmpEF0;enum Cyc_Absyn_AliasQual
_tmpEF1;_LL71E: {struct Cyc_Absyn_Less_kb_struct*_tmpEEB=(struct Cyc_Absyn_Less_kb_struct*)
_tmpEEA;if(_tmpEEB->tag != 2)goto _LL720;else{_tmpEEC=_tmpEEB->f1;_tmpEED=(struct
Cyc_Core_Opt**)& _tmpEEB->f1;_tmpEEE=_tmpEEB->f2;_tmpEEF=*_tmpEEE;_tmpEF0=_tmpEEF.kind;
if(_tmpEF0 != Cyc_Absyn_RgnKind)goto _LL720;_tmpEF1=_tmpEEF.aliasqual;if(_tmpEF1 != 
Cyc_Absyn_Top)goto _LL720;}}_LL71F:{struct Cyc_Absyn_Less_kb_struct*_tmp161D;
struct Cyc_Absyn_Less_kb_struct _tmp161C;struct Cyc_Core_Opt*_tmp161B;*_tmpEED=((
_tmp161B=_cycalloc(sizeof(*_tmp161B)),((_tmp161B->v=(void*)((_tmp161D=_cycalloc(
sizeof(*_tmp161D)),((_tmp161D[0]=((_tmp161C.tag=2,((_tmp161C.f1=0,((_tmp161C.f2=&
Cyc_Tcutil_rk,_tmp161C)))))),_tmp161D)))),_tmp161B))));}return 0;_LL720:;_LL721:
return 1;_LL71D:;}return 0;}_LL71B:;_LL71C: return 0;_LL714:;}static int Cyc_Tcutil_is_noalias_pointer_aux(
void*t,int must_be_unique){void*_tmpEF5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpEF7;struct Cyc_Absyn_PtrAtts _tmpEF8;void*_tmpEF9;struct Cyc_Absyn_Tvar*_tmpEFB;
_LL723: {struct Cyc_Absyn_PointerType_struct*_tmpEF6=(struct Cyc_Absyn_PointerType_struct*)
_tmpEF5;if(_tmpEF6->tag != 5)goto _LL725;else{_tmpEF7=_tmpEF6->f1;_tmpEF8=_tmpEF7.ptr_atts;
_tmpEF9=_tmpEF8.rgn;}}_LL724: return Cyc_Tcutil_is_noalias_region(_tmpEF9,
must_be_unique);_LL725: {struct Cyc_Absyn_VarType_struct*_tmpEFA=(struct Cyc_Absyn_VarType_struct*)
_tmpEF5;if(_tmpEFA->tag != 2)goto _LL727;else{_tmpEFB=_tmpEFA->f1;}}_LL726: {
struct Cyc_Absyn_Kind _tmpEFD;enum Cyc_Absyn_KindQual _tmpEFE;enum Cyc_Absyn_AliasQual
_tmpEFF;struct Cyc_Absyn_Kind*_tmpEFC=Cyc_Tcutil_tvar_kind(_tmpEFB,& Cyc_Tcutil_bk);
_tmpEFD=*_tmpEFC;_tmpEFE=_tmpEFD.kind;_tmpEFF=_tmpEFD.aliasqual;switch(_tmpEFE){
case Cyc_Absyn_BoxKind: _LL729: goto _LL72A;case Cyc_Absyn_AnyKind: _LL72A: goto _LL72B;
case Cyc_Absyn_MemKind: _LL72B: if(_tmpEFF == Cyc_Absyn_Unique  || _tmpEFF == Cyc_Absyn_Top){
void*_tmpF00=Cyc_Absyn_compress_kb(_tmpEFB->kind);struct Cyc_Core_Opt*_tmpF02;
struct Cyc_Core_Opt**_tmpF03;struct Cyc_Absyn_Kind*_tmpF04;struct Cyc_Absyn_Kind
_tmpF05;enum Cyc_Absyn_KindQual _tmpF06;enum Cyc_Absyn_AliasQual _tmpF07;_LL72E: {
struct Cyc_Absyn_Less_kb_struct*_tmpF01=(struct Cyc_Absyn_Less_kb_struct*)_tmpF00;
if(_tmpF01->tag != 2)goto _LL730;else{_tmpF02=_tmpF01->f1;_tmpF03=(struct Cyc_Core_Opt**)&
_tmpF01->f1;_tmpF04=_tmpF01->f2;_tmpF05=*_tmpF04;_tmpF06=_tmpF05.kind;_tmpF07=
_tmpF05.aliasqual;if(_tmpF07 != Cyc_Absyn_Top)goto _LL730;}}_LL72F:{struct Cyc_Absyn_Less_kb_struct*
_tmp1627;struct Cyc_Absyn_Kind*_tmp1626;struct Cyc_Absyn_Less_kb_struct _tmp1625;
struct Cyc_Core_Opt*_tmp1624;*_tmpF03=((_tmp1624=_cycalloc(sizeof(*_tmp1624)),((
_tmp1624->v=(void*)((_tmp1627=_cycalloc(sizeof(*_tmp1627)),((_tmp1627[0]=((
_tmp1625.tag=2,((_tmp1625.f1=0,((_tmp1625.f2=((_tmp1626=_cycalloc_atomic(sizeof(*
_tmp1626)),((_tmp1626->kind=_tmpF06,((_tmp1626->aliasqual=Cyc_Absyn_Aliasable,
_tmp1626)))))),_tmp1625)))))),_tmp1627)))),_tmp1624))));}return 0;_LL730:;_LL731:
return 1;_LL72D:;}return 0;default: _LL72C: return 0;}}_LL727:;_LL728: return 0;_LL722:;}
int Cyc_Tcutil_is_noalias_pointer(void*t){return Cyc_Tcutil_is_noalias_pointer_aux(
t,0);}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpF0C=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(_tmpF0C))
return 1;{void*_tmpF0D=_tmpF0C;struct Cyc_List_List*_tmpF0F;struct Cyc_Absyn_AggrInfo
_tmpF11;union Cyc_Absyn_AggrInfoU _tmpF12;struct Cyc_Absyn_Aggrdecl**_tmpF13;struct
Cyc_List_List*_tmpF14;struct Cyc_List_List*_tmpF16;struct Cyc_Absyn_AggrInfo
_tmpF18;union Cyc_Absyn_AggrInfoU _tmpF19;struct _tuple4 _tmpF1A;struct Cyc_Absyn_DatatypeInfo
_tmpF1C;union Cyc_Absyn_DatatypeInfoU _tmpF1D;struct Cyc_List_List*_tmpF1E;struct
Cyc_Absyn_DatatypeFieldInfo _tmpF20;union Cyc_Absyn_DatatypeFieldInfoU _tmpF21;
struct Cyc_List_List*_tmpF22;_LL734: {struct Cyc_Absyn_TupleType_struct*_tmpF0E=(
struct Cyc_Absyn_TupleType_struct*)_tmpF0D;if(_tmpF0E->tag != 10)goto _LL736;else{
_tmpF0F=_tmpF0E->f1;}}_LL735: while(_tmpF0F != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple12*)_tmpF0F->hd)).f2))return 1;_tmpF0F=_tmpF0F->tl;}return 0;
_LL736: {struct Cyc_Absyn_AggrType_struct*_tmpF10=(struct Cyc_Absyn_AggrType_struct*)
_tmpF0D;if(_tmpF10->tag != 11)goto _LL738;else{_tmpF11=_tmpF10->f1;_tmpF12=_tmpF11.aggr_info;
if((_tmpF12.KnownAggr).tag != 2)goto _LL738;_tmpF13=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF12.KnownAggr).val;_tmpF14=_tmpF11.targs;}}_LL737: if((*_tmpF13)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpF23=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpF13)->tvs,_tmpF14);struct Cyc_List_List*_tmpF24=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpF13)->impl))->fields;void*t;
while(_tmpF24 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpF23,((struct Cyc_Absyn_Aggrfield*)
_tmpF24->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpF24=_tmpF24->tl;}return 0;}_LL738: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpF15=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpF0D;if(_tmpF15->tag != 12)goto
_LL73A;else{_tmpF16=_tmpF15->f2;}}_LL739: while(_tmpF16 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpF16->hd)->type))return 1;_tmpF16=_tmpF16->tl;}
return 0;_LL73A: {struct Cyc_Absyn_AggrType_struct*_tmpF17=(struct Cyc_Absyn_AggrType_struct*)
_tmpF0D;if(_tmpF17->tag != 11)goto _LL73C;else{_tmpF18=_tmpF17->f1;_tmpF19=_tmpF18.aggr_info;
if((_tmpF19.UnknownAggr).tag != 1)goto _LL73C;_tmpF1A=(struct _tuple4)(_tmpF19.UnknownAggr).val;}}
_LL73B: {const char*_tmp162A;void*_tmp1629;(_tmp1629=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp162A="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp162A,sizeof(char),36))),_tag_dyneither(_tmp1629,sizeof(void*),
0)));}_LL73C: {struct Cyc_Absyn_DatatypeType_struct*_tmpF1B=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpF0D;if(_tmpF1B->tag != 3)goto _LL73E;else{_tmpF1C=_tmpF1B->f1;_tmpF1D=_tmpF1C.datatype_info;
_tmpF1E=_tmpF1C.targs;}}_LL73D: {union Cyc_Absyn_DatatypeInfoU _tmpF27=_tmpF1D;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpF28;struct _tuple2*_tmpF29;int _tmpF2A;
struct Cyc_Absyn_Datatypedecl**_tmpF2B;struct Cyc_Absyn_Datatypedecl*_tmpF2C;
struct Cyc_Absyn_Datatypedecl _tmpF2D;struct Cyc_List_List*_tmpF2E;struct Cyc_Core_Opt*
_tmpF2F;_LL743: if((_tmpF27.UnknownDatatype).tag != 1)goto _LL745;_tmpF28=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpF27.UnknownDatatype).val;_tmpF29=_tmpF28.name;
_tmpF2A=_tmpF28.is_extensible;_LL744: {const char*_tmp162D;void*_tmp162C;(
_tmp162C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp162D="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp162D,
sizeof(char),40))),_tag_dyneither(_tmp162C,sizeof(void*),0)));}_LL745: if((
_tmpF27.KnownDatatype).tag != 2)goto _LL742;_tmpF2B=(struct Cyc_Absyn_Datatypedecl**)(
_tmpF27.KnownDatatype).val;_tmpF2C=*_tmpF2B;_tmpF2D=*_tmpF2C;_tmpF2E=_tmpF2D.tvs;
_tmpF2F=_tmpF2D.fields;_LL746: return 0;_LL742:;}_LL73E: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpF1F=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpF0D;if(_tmpF1F->tag != 4)
goto _LL740;else{_tmpF20=_tmpF1F->f1;_tmpF21=_tmpF20.field_info;_tmpF22=_tmpF20.targs;}}
_LL73F: {union Cyc_Absyn_DatatypeFieldInfoU _tmpF32=_tmpF21;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpF33;struct _tuple3 _tmpF34;struct Cyc_Absyn_Datatypedecl*_tmpF35;struct Cyc_Absyn_Datatypefield*
_tmpF36;_LL748: if((_tmpF32.UnknownDatatypefield).tag != 1)goto _LL74A;_tmpF33=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpF32.UnknownDatatypefield).val;
_LL749: {const char*_tmp1630;void*_tmp162F;(_tmp162F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1630="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1630,sizeof(char),46))),_tag_dyneither(_tmp162F,sizeof(void*),
0)));}_LL74A: if((_tmpF32.KnownDatatypefield).tag != 2)goto _LL747;_tmpF34=(struct
_tuple3)(_tmpF32.KnownDatatypefield).val;_tmpF35=_tmpF34.f1;_tmpF36=_tmpF34.f2;
_LL74B: {struct Cyc_List_List*_tmpF39=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpF35->tvs,_tmpF22);struct Cyc_List_List*_tmpF3A=
_tmpF36->typs;while(_tmpF3A != 0){_tmpF0C=Cyc_Tcutil_rsubstitute(rgn,_tmpF39,(*((
struct _tuple12*)_tmpF3A->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpF0C))return 1;_tmpF3A=_tmpF3A->tl;}return 0;}_LL747:;}_LL740:;_LL741: return 0;
_LL733:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f){void*_tmpF3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpF3D;union Cyc_Absyn_AggrInfoU _tmpF3E;struct Cyc_Absyn_Aggrdecl**_tmpF3F;struct
Cyc_Absyn_Aggrdecl*_tmpF40;struct Cyc_List_List*_tmpF41;struct Cyc_List_List*
_tmpF43;_LL74D: {struct Cyc_Absyn_AggrType_struct*_tmpF3C=(struct Cyc_Absyn_AggrType_struct*)
_tmpF3B;if(_tmpF3C->tag != 11)goto _LL74F;else{_tmpF3D=_tmpF3C->f1;_tmpF3E=_tmpF3D.aggr_info;
if((_tmpF3E.KnownAggr).tag != 2)goto _LL74F;_tmpF3F=(struct Cyc_Absyn_Aggrdecl**)(
_tmpF3E.KnownAggr).val;_tmpF40=*_tmpF3F;_tmpF41=_tmpF3D.targs;}}_LL74E: _tmpF43=
_tmpF40->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF40->impl))->fields;
goto _LL750;_LL74F: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF42=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpF3B;if(_tmpF42->tag != 12)goto _LL751;else{_tmpF43=_tmpF42->f2;}}_LL750: {
struct Cyc_Absyn_Aggrfield*_tmpF44=Cyc_Absyn_lookup_field(_tmpF43,f);{struct Cyc_Absyn_Aggrfield*
_tmpF45=_tmpF44;struct Cyc_Absyn_Aggrfield _tmpF46;void*_tmpF47;_LL754: if(_tmpF45
!= 0)goto _LL756;_LL755: {const char*_tmp1633;void*_tmp1632;(_tmp1632=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1633="is_noalias_field: missing field",
_tag_dyneither(_tmp1633,sizeof(char),32))),_tag_dyneither(_tmp1632,sizeof(void*),
0)));}_LL756: if(_tmpF45 == 0)goto _LL753;_tmpF46=*_tmpF45;_tmpF47=_tmpF46.type;
_LL757: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpF47);_LL753:;}return 0;}
_LL751:;_LL752: {const char*_tmp1637;void*_tmp1636[1];struct Cyc_String_pa_struct
_tmp1635;(_tmp1635.tag=0,((_tmp1635.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1636[0]=& _tmp1635,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1637="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp1637,sizeof(char),36))),_tag_dyneither(_tmp1636,sizeof(void*),
1)))))));}_LL74C:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e){void*_tmpF4D=e->r;void*_tmpF4F;struct Cyc_Absyn_Exp*
_tmpF52;struct Cyc_Absyn_Exp*_tmpF54;struct Cyc_Absyn_Exp*_tmpF56;struct
_dyneither_ptr*_tmpF57;struct Cyc_Absyn_Exp*_tmpF59;struct Cyc_Absyn_Exp*_tmpF5A;
struct Cyc_Absyn_Exp*_tmpF5C;struct Cyc_Absyn_Exp*_tmpF5D;struct Cyc_Absyn_Exp*
_tmpF5F;struct Cyc_Absyn_Exp*_tmpF61;struct Cyc_Absyn_Stmt*_tmpF63;_LL759: {struct
Cyc_Absyn_Var_e_struct*_tmpF4E=(struct Cyc_Absyn_Var_e_struct*)_tmpF4D;if(_tmpF4E->tag
!= 1)goto _LL75B;else{_tmpF4F=(void*)_tmpF4E->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF50=(struct Cyc_Absyn_Global_b_struct*)_tmpF4F;if(_tmpF50->tag != 1)goto _LL75B;};}}
_LL75A: return 0;_LL75B: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF51=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF4D;if(_tmpF51->tag != 23)goto _LL75D;else{_tmpF52=_tmpF51->f1;}}_LL75C:
_tmpF54=_tmpF52;goto _LL75E;_LL75D: {struct Cyc_Absyn_Deref_e_struct*_tmpF53=(
struct Cyc_Absyn_Deref_e_struct*)_tmpF4D;if(_tmpF53->tag != 21)goto _LL75F;else{
_tmpF54=_tmpF53->f1;}}_LL75E: return Cyc_Tcutil_is_noalias_pointer_aux((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpF54->topt))->v,1) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF54);_LL75F: {struct Cyc_Absyn_AggrMember_e_struct*_tmpF55=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpF4D;if(_tmpF55->tag != 22)goto _LL761;else{_tmpF56=_tmpF55->f1;_tmpF57=_tmpF55->f2;}}
_LL760: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF56);_LL761: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF58=(struct Cyc_Absyn_Subscript_e_struct*)_tmpF4D;if(_tmpF58->tag != 24)goto
_LL763;else{_tmpF59=_tmpF58->f1;_tmpF5A=_tmpF58->f2;}}_LL762: {void*_tmpF64=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF59->topt))->v);_LL76E: {struct Cyc_Absyn_TupleType_struct*
_tmpF65=(struct Cyc_Absyn_TupleType_struct*)_tmpF64;if(_tmpF65->tag != 10)goto
_LL770;}_LL76F: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF59);_LL770:;_LL771:
return 0;_LL76D:;}_LL763: {struct Cyc_Absyn_Conditional_e_struct*_tmpF5B=(struct
Cyc_Absyn_Conditional_e_struct*)_tmpF4D;if(_tmpF5B->tag != 6)goto _LL765;else{
_tmpF5C=_tmpF5B->f2;_tmpF5D=_tmpF5B->f3;}}_LL764: return Cyc_Tcutil_is_noalias_path_aux(
r,_tmpF5C) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpF5D);_LL765: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpF5E=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpF4D;if(_tmpF5E->tag != 9)goto _LL767;
else{_tmpF5F=_tmpF5E->f2;}}_LL766: _tmpF61=_tmpF5F;goto _LL768;_LL767: {struct Cyc_Absyn_Cast_e_struct*
_tmpF60=(struct Cyc_Absyn_Cast_e_struct*)_tmpF4D;if(_tmpF60->tag != 15)goto _LL769;
else{_tmpF61=_tmpF60->f2;}}_LL768: return Cyc_Tcutil_is_noalias_path_aux(r,_tmpF61);
_LL769: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF62=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpF4D;if(_tmpF62->tag != 37)goto _LL76B;else{_tmpF63=_tmpF62->f1;}}_LL76A: while(
1){void*_tmpF66=_tmpF63->r;struct Cyc_Absyn_Stmt*_tmpF68;struct Cyc_Absyn_Stmt*
_tmpF69;struct Cyc_Absyn_Decl*_tmpF6B;struct Cyc_Absyn_Stmt*_tmpF6C;struct Cyc_Absyn_Exp*
_tmpF6E;_LL773: {struct Cyc_Absyn_Seq_s_struct*_tmpF67=(struct Cyc_Absyn_Seq_s_struct*)
_tmpF66;if(_tmpF67->tag != 2)goto _LL775;else{_tmpF68=_tmpF67->f1;_tmpF69=_tmpF67->f2;}}
_LL774: _tmpF63=_tmpF69;goto _LL772;_LL775: {struct Cyc_Absyn_Decl_s_struct*_tmpF6A=(
struct Cyc_Absyn_Decl_s_struct*)_tmpF66;if(_tmpF6A->tag != 12)goto _LL777;else{
_tmpF6B=_tmpF6A->f1;_tmpF6C=_tmpF6A->f2;}}_LL776: _tmpF63=_tmpF6C;goto _LL772;
_LL777: {struct Cyc_Absyn_Exp_s_struct*_tmpF6D=(struct Cyc_Absyn_Exp_s_struct*)
_tmpF66;if(_tmpF6D->tag != 1)goto _LL779;else{_tmpF6E=_tmpF6D->f1;}}_LL778: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpF6E);_LL779:;_LL77A: {const char*_tmp163A;
void*_tmp1639;(_tmp1639=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp163A="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp163A,sizeof(char),40))),_tag_dyneither(_tmp1639,sizeof(void*),
0)));}_LL772:;}_LL76B:;_LL76C: return 1;_LL758:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e);}struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple18 _tmp163B;struct _tuple18 bogus_ans=(_tmp163B.f1=0,((_tmp163B.f2=(
void*)& Cyc_Absyn_HeapRgn_val,_tmp163B)));void*_tmpF71=e->r;struct _tuple2*_tmpF73;
void*_tmpF74;struct Cyc_Absyn_Exp*_tmpF76;struct _dyneither_ptr*_tmpF77;int _tmpF78;
struct Cyc_Absyn_Exp*_tmpF7A;struct _dyneither_ptr*_tmpF7B;int _tmpF7C;struct Cyc_Absyn_Exp*
_tmpF7E;struct Cyc_Absyn_Exp*_tmpF80;struct Cyc_Absyn_Exp*_tmpF81;_LL77C: {struct
Cyc_Absyn_Var_e_struct*_tmpF72=(struct Cyc_Absyn_Var_e_struct*)_tmpF71;if(_tmpF72->tag
!= 1)goto _LL77E;else{_tmpF73=_tmpF72->f1;_tmpF74=(void*)_tmpF72->f2;}}_LL77D: {
void*_tmpF82=_tmpF74;struct Cyc_Absyn_Vardecl*_tmpF86;struct Cyc_Absyn_Vardecl*
_tmpF88;struct Cyc_Absyn_Vardecl*_tmpF8A;struct Cyc_Absyn_Vardecl*_tmpF8C;_LL789: {
struct Cyc_Absyn_Unresolved_b_struct*_tmpF83=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpF82;if(_tmpF83->tag != 0)goto _LL78B;}_LL78A: goto _LL78C;_LL78B: {struct Cyc_Absyn_Funname_b_struct*
_tmpF84=(struct Cyc_Absyn_Funname_b_struct*)_tmpF82;if(_tmpF84->tag != 2)goto
_LL78D;}_LL78C: return bogus_ans;_LL78D: {struct Cyc_Absyn_Global_b_struct*_tmpF85=(
struct Cyc_Absyn_Global_b_struct*)_tmpF82;if(_tmpF85->tag != 1)goto _LL78F;else{
_tmpF86=_tmpF85->f1;}}_LL78E: {void*_tmpF8D=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL796: {struct Cyc_Absyn_ArrayType_struct*
_tmpF8E=(struct Cyc_Absyn_ArrayType_struct*)_tmpF8D;if(_tmpF8E->tag != 8)goto
_LL798;}_LL797: {struct _tuple18 _tmp163C;return(_tmp163C.f1=1,((_tmp163C.f2=(void*)&
Cyc_Absyn_HeapRgn_val,_tmp163C)));}_LL798:;_LL799: {struct _tuple18 _tmp163D;
return(_tmp163D.f1=(_tmpF86->tq).real_const,((_tmp163D.f2=(void*)& Cyc_Absyn_HeapRgn_val,
_tmp163D)));}_LL795:;}_LL78F: {struct Cyc_Absyn_Local_b_struct*_tmpF87=(struct Cyc_Absyn_Local_b_struct*)
_tmpF82;if(_tmpF87->tag != 4)goto _LL791;else{_tmpF88=_tmpF87->f1;}}_LL790: {void*
_tmpF91=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL79B: {struct Cyc_Absyn_ArrayType_struct*_tmpF92=(struct Cyc_Absyn_ArrayType_struct*)
_tmpF91;if(_tmpF92->tag != 8)goto _LL79D;}_LL79C: {struct _tuple18 _tmp163E;return(
_tmp163E.f1=1,((_tmp163E.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpF88->rgn))->v,
_tmp163E)));}_LL79D:;_LL79E: _tmpF88->escapes=1;{struct _tuple18 _tmp163F;return(
_tmp163F.f1=(_tmpF88->tq).real_const,((_tmp163F.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF88->rgn))->v,_tmp163F)));};_LL79A:;}_LL791: {struct Cyc_Absyn_Pat_b_struct*
_tmpF89=(struct Cyc_Absyn_Pat_b_struct*)_tmpF82;if(_tmpF89->tag != 5)goto _LL793;
else{_tmpF8A=_tmpF89->f1;}}_LL792: _tmpF8C=_tmpF8A;goto _LL794;_LL793: {struct Cyc_Absyn_Param_b_struct*
_tmpF8B=(struct Cyc_Absyn_Param_b_struct*)_tmpF82;if(_tmpF8B->tag != 3)goto _LL788;
else{_tmpF8C=_tmpF8B->f1;}}_LL794: _tmpF8C->escapes=1;{struct _tuple18 _tmp1640;
return(_tmp1640.f1=(_tmpF8C->tq).real_const,((_tmp1640.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF8C->rgn))->v,_tmp1640)));};_LL788:;}_LL77E: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpF75=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpF71;if(_tmpF75->tag != 22)goto
_LL780;else{_tmpF76=_tmpF75->f1;_tmpF77=_tmpF75->f2;_tmpF78=_tmpF75->f3;}}_LL77F:
if(_tmpF78)return bogus_ans;{void*_tmpF96=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF76->topt))->v);struct Cyc_List_List*_tmpF98;struct Cyc_Absyn_AggrInfo
_tmpF9A;union Cyc_Absyn_AggrInfoU _tmpF9B;struct Cyc_Absyn_Aggrdecl**_tmpF9C;struct
Cyc_Absyn_Aggrdecl*_tmpF9D;_LL7A0: {struct Cyc_Absyn_AnonAggrType_struct*_tmpF97=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmpF96;if(_tmpF97->tag != 12)goto _LL7A2;
else{_tmpF98=_tmpF97->f2;}}_LL7A1: {struct Cyc_Absyn_Aggrfield*_tmpF9E=Cyc_Absyn_lookup_field(
_tmpF98,_tmpF77);if(_tmpF9E != 0  && _tmpF9E->width == 0){struct _tuple18 _tmp1641;
return(_tmp1641.f1=(_tmpF9E->tq).real_const,((_tmp1641.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF76)).f2,_tmp1641)));}return bogus_ans;}_LL7A2: {struct Cyc_Absyn_AggrType_struct*
_tmpF99=(struct Cyc_Absyn_AggrType_struct*)_tmpF96;if(_tmpF99->tag != 11)goto
_LL7A4;else{_tmpF9A=_tmpF99->f1;_tmpF9B=_tmpF9A.aggr_info;if((_tmpF9B.KnownAggr).tag
!= 2)goto _LL7A4;_tmpF9C=(struct Cyc_Absyn_Aggrdecl**)(_tmpF9B.KnownAggr).val;
_tmpF9D=*_tmpF9C;}}_LL7A3: {struct Cyc_Absyn_Aggrfield*_tmpFA0=Cyc_Absyn_lookup_decl_field(
_tmpF9D,_tmpF77);if(_tmpFA0 != 0  && _tmpFA0->width == 0){struct _tuple18 _tmp1642;
return(_tmp1642.f1=(_tmpFA0->tq).real_const,((_tmp1642.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF76)).f2,_tmp1642)));}return bogus_ans;}_LL7A4:;_LL7A5: return bogus_ans;
_LL79F:;};_LL780: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpF79=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpF71;if(_tmpF79->tag != 23)goto _LL782;else{_tmpF7A=_tmpF79->f1;_tmpF7B=_tmpF79->f2;
_tmpF7C=_tmpF79->f3;}}_LL781: if(_tmpF7C)return bogus_ans;{void*_tmpFA2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpF7A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpFA4;void*_tmpFA5;struct Cyc_Absyn_PtrAtts _tmpFA6;void*_tmpFA7;_LL7A7: {struct
Cyc_Absyn_PointerType_struct*_tmpFA3=(struct Cyc_Absyn_PointerType_struct*)
_tmpFA2;if(_tmpFA3->tag != 5)goto _LL7A9;else{_tmpFA4=_tmpFA3->f1;_tmpFA5=_tmpFA4.elt_typ;
_tmpFA6=_tmpFA4.ptr_atts;_tmpFA7=_tmpFA6.rgn;}}_LL7A8: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpFA8=Cyc_Tcutil_compress(_tmpFA5);struct Cyc_List_List*_tmpFAA;
struct Cyc_Absyn_AggrInfo _tmpFAC;union Cyc_Absyn_AggrInfoU _tmpFAD;struct Cyc_Absyn_Aggrdecl**
_tmpFAE;struct Cyc_Absyn_Aggrdecl*_tmpFAF;_LL7AC: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpFA9=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpFA8;if(_tmpFA9->tag != 12)goto
_LL7AE;else{_tmpFAA=_tmpFA9->f2;}}_LL7AD: finfo=Cyc_Absyn_lookup_field(_tmpFAA,
_tmpF7B);goto _LL7AB;_LL7AE: {struct Cyc_Absyn_AggrType_struct*_tmpFAB=(struct Cyc_Absyn_AggrType_struct*)
_tmpFA8;if(_tmpFAB->tag != 11)goto _LL7B0;else{_tmpFAC=_tmpFAB->f1;_tmpFAD=_tmpFAC.aggr_info;
if((_tmpFAD.KnownAggr).tag != 2)goto _LL7B0;_tmpFAE=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFAD.KnownAggr).val;_tmpFAF=*_tmpFAE;}}_LL7AF: finfo=Cyc_Absyn_lookup_decl_field(
_tmpFAF,_tmpF7B);goto _LL7AB;_LL7B0:;_LL7B1: return bogus_ans;_LL7AB:;}if(finfo != 0
 && finfo->width == 0){struct _tuple18 _tmp1643;return(_tmp1643.f1=(finfo->tq).real_const,((
_tmp1643.f2=_tmpFA7,_tmp1643)));}return bogus_ans;}_LL7A9:;_LL7AA: return bogus_ans;
_LL7A6:;};_LL782: {struct Cyc_Absyn_Deref_e_struct*_tmpF7D=(struct Cyc_Absyn_Deref_e_struct*)
_tmpF71;if(_tmpF7D->tag != 21)goto _LL784;else{_tmpF7E=_tmpF7D->f1;}}_LL783: {void*
_tmpFB1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpF7E->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpFB3;struct Cyc_Absyn_Tqual _tmpFB4;struct Cyc_Absyn_PtrAtts
_tmpFB5;void*_tmpFB6;_LL7B3: {struct Cyc_Absyn_PointerType_struct*_tmpFB2=(struct
Cyc_Absyn_PointerType_struct*)_tmpFB1;if(_tmpFB2->tag != 5)goto _LL7B5;else{
_tmpFB3=_tmpFB2->f1;_tmpFB4=_tmpFB3.elt_tq;_tmpFB5=_tmpFB3.ptr_atts;_tmpFB6=
_tmpFB5.rgn;}}_LL7B4: {struct _tuple18 _tmp1644;return(_tmp1644.f1=_tmpFB4.real_const,((
_tmp1644.f2=_tmpFB6,_tmp1644)));}_LL7B5:;_LL7B6: return bogus_ans;_LL7B2:;}_LL784: {
struct Cyc_Absyn_Subscript_e_struct*_tmpF7F=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpF71;if(_tmpF7F->tag != 24)goto _LL786;else{_tmpF80=_tmpF7F->f1;_tmpF81=_tmpF7F->f2;}}
_LL785: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpF80->topt))->v);void*_tmpFB8=t;struct Cyc_List_List*_tmpFBA;struct Cyc_Absyn_PtrInfo
_tmpFBC;struct Cyc_Absyn_Tqual _tmpFBD;struct Cyc_Absyn_PtrAtts _tmpFBE;void*_tmpFBF;
struct Cyc_Absyn_ArrayInfo _tmpFC1;struct Cyc_Absyn_Tqual _tmpFC2;_LL7B8: {struct Cyc_Absyn_TupleType_struct*
_tmpFB9=(struct Cyc_Absyn_TupleType_struct*)_tmpFB8;if(_tmpFB9->tag != 10)goto
_LL7BA;else{_tmpFBA=_tmpFB9->f1;}}_LL7B9: {unsigned int _tmpFC4;int _tmpFC5;struct
_tuple14 _tmpFC3=Cyc_Evexp_eval_const_uint_exp(_tmpF81);_tmpFC4=_tmpFC3.f1;
_tmpFC5=_tmpFC3.f2;if(!_tmpFC5)return bogus_ans;{struct _tuple12*_tmpFC6=Cyc_Absyn_lookup_tuple_field(
_tmpFBA,(int)_tmpFC4);if(_tmpFC6 != 0){struct _tuple18 _tmp1645;return(_tmp1645.f1=((*
_tmpFC6).f1).real_const,((_tmp1645.f2=(Cyc_Tcutil_addressof_props(te,_tmpF80)).f2,
_tmp1645)));}return bogus_ans;};}_LL7BA: {struct Cyc_Absyn_PointerType_struct*
_tmpFBB=(struct Cyc_Absyn_PointerType_struct*)_tmpFB8;if(_tmpFBB->tag != 5)goto
_LL7BC;else{_tmpFBC=_tmpFBB->f1;_tmpFBD=_tmpFBC.elt_tq;_tmpFBE=_tmpFBC.ptr_atts;
_tmpFBF=_tmpFBE.rgn;}}_LL7BB: {struct _tuple18 _tmp1646;return(_tmp1646.f1=_tmpFBD.real_const,((
_tmp1646.f2=_tmpFBF,_tmp1646)));}_LL7BC: {struct Cyc_Absyn_ArrayType_struct*
_tmpFC0=(struct Cyc_Absyn_ArrayType_struct*)_tmpFB8;if(_tmpFC0->tag != 8)goto
_LL7BE;else{_tmpFC1=_tmpFC0->f1;_tmpFC2=_tmpFC1.tq;}}_LL7BD: {struct _tuple18
_tmp1647;return(_tmp1647.f1=_tmpFC2.real_const,((_tmp1647.f2=(Cyc_Tcutil_addressof_props(
te,_tmpF80)).f2,_tmp1647)));}_LL7BE:;_LL7BF: return bogus_ans;_LL7B7:;}_LL786:;
_LL787:{const char*_tmp164A;void*_tmp1649;(_tmp1649=0,Cyc_Tcutil_terr(e->loc,((
_tmp164A="unary & applied to non-lvalue",_tag_dyneither(_tmp164A,sizeof(char),30))),
_tag_dyneither(_tmp1649,sizeof(void*),0)));}return bogus_ans;_LL77B:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpFCD=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpFCF;void*_tmpFD0;struct Cyc_Absyn_Tqual
_tmpFD1;union Cyc_Absyn_Constraint*_tmpFD2;_LL7C1: {struct Cyc_Absyn_ArrayType_struct*
_tmpFCE=(struct Cyc_Absyn_ArrayType_struct*)_tmpFCD;if(_tmpFCE->tag != 8)goto
_LL7C3;else{_tmpFCF=_tmpFCE->f1;_tmpFD0=_tmpFCF.elt_type;_tmpFD1=_tmpFCF.tq;
_tmpFD2=_tmpFCF.zero_term;}}_LL7C2: {void*_tmpFD4;struct _tuple18 _tmpFD3=Cyc_Tcutil_addressof_props(
te,e);_tmpFD4=_tmpFD3.f2;{struct Cyc_Absyn_Upper_b_struct _tmp164D;struct Cyc_Absyn_Upper_b_struct*
_tmp164C;return Cyc_Absyn_atb_typ(_tmpFD0,_tmpFD4,_tmpFD1,(void*)((_tmp164C=
_cycalloc(sizeof(*_tmp164C)),((_tmp164C[0]=((_tmp164D.tag=1,((_tmp164D.f1=e,
_tmp164D)))),_tmp164C)))),_tmpFD2);};}_LL7C3:;_LL7C4: return e_typ;_LL7C0:;}void
Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
b);{void*_tmpFD7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpFDA;_LL7C6: {struct Cyc_Absyn_DynEither_b_struct*
_tmpFD8=(struct Cyc_Absyn_DynEither_b_struct*)_tmpFD7;if(_tmpFD8->tag != 0)goto
_LL7C8;}_LL7C7: return;_LL7C8: {struct Cyc_Absyn_Upper_b_struct*_tmpFD9=(struct Cyc_Absyn_Upper_b_struct*)
_tmpFD7;if(_tmpFD9->tag != 1)goto _LL7C5;else{_tmpFDA=_tmpFD9->f1;}}_LL7C9: {
unsigned int _tmpFDC;int _tmpFDD;struct _tuple14 _tmpFDB=Cyc_Evexp_eval_const_uint_exp(
_tmpFDA);_tmpFDC=_tmpFDB.f1;_tmpFDD=_tmpFDB.f2;if(_tmpFDD  && _tmpFDC <= i){const
char*_tmp1652;void*_tmp1651[2];struct Cyc_Int_pa_struct _tmp1650;struct Cyc_Int_pa_struct
_tmp164F;(_tmp164F.tag=1,((_tmp164F.f1=(unsigned long)((int)i),((_tmp1650.tag=1,((
_tmp1650.f1=(unsigned long)((int)_tmpFDC),((_tmp1651[0]=& _tmp1650,((_tmp1651[1]=&
_tmp164F,Cyc_Tcutil_terr(loc,((_tmp1652="dereference is out of bounds: %d <= %d",
_tag_dyneither(_tmp1652,sizeof(char),39))),_tag_dyneither(_tmp1651,sizeof(void*),
2)))))))))))));}return;}_LL7C5:;};}void Cyc_Tcutil_check_nonzero_bound(
unsigned int loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){void*_tmpFE2=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
b);struct Cyc_Absyn_Exp*_tmpFE4;_LL7CB: {struct Cyc_Absyn_Upper_b_struct*_tmpFE3=(
struct Cyc_Absyn_Upper_b_struct*)_tmpFE2;if(_tmpFE3->tag != 1)goto _LL7CD;else{
_tmpFE4=_tmpFE3->f1;}}_LL7CC: {unsigned int _tmpFE6;int _tmpFE7;struct _tuple14
_tmpFE5=Cyc_Evexp_eval_const_uint_exp(_tmpFE4);_tmpFE6=_tmpFE5.f1;_tmpFE7=
_tmpFE5.f2;return _tmpFE7  && _tmpFE6 == 1;}_LL7CD:;_LL7CE: return 0;_LL7CA:;}int Cyc_Tcutil_bits_only(
void*t){void*_tmpFE8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpFEF;
void*_tmpFF0;union Cyc_Absyn_Constraint*_tmpFF1;struct Cyc_List_List*_tmpFF3;
struct Cyc_Absyn_AggrInfo _tmpFF5;union Cyc_Absyn_AggrInfoU _tmpFF6;struct _tuple4
_tmpFF7;struct Cyc_Absyn_AggrInfo _tmpFF9;union Cyc_Absyn_AggrInfoU _tmpFFA;struct
Cyc_Absyn_Aggrdecl**_tmpFFB;struct Cyc_Absyn_Aggrdecl*_tmpFFC;struct Cyc_List_List*
_tmpFFD;struct Cyc_List_List*_tmpFFF;_LL7D0: {struct Cyc_Absyn_VoidType_struct*
_tmpFE9=(struct Cyc_Absyn_VoidType_struct*)_tmpFE8;if(_tmpFE9->tag != 0)goto _LL7D2;}
_LL7D1: goto _LL7D3;_LL7D2: {struct Cyc_Absyn_IntType_struct*_tmpFEA=(struct Cyc_Absyn_IntType_struct*)
_tmpFE8;if(_tmpFEA->tag != 6)goto _LL7D4;}_LL7D3: goto _LL7D5;_LL7D4: {struct Cyc_Absyn_FloatType_struct*
_tmpFEB=(struct Cyc_Absyn_FloatType_struct*)_tmpFE8;if(_tmpFEB->tag != 7)goto
_LL7D6;}_LL7D5: return 1;_LL7D6: {struct Cyc_Absyn_EnumType_struct*_tmpFEC=(struct
Cyc_Absyn_EnumType_struct*)_tmpFE8;if(_tmpFEC->tag != 13)goto _LL7D8;}_LL7D7: goto
_LL7D9;_LL7D8: {struct Cyc_Absyn_AnonEnumType_struct*_tmpFED=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmpFE8;if(_tmpFED->tag != 14)goto _LL7DA;}_LL7D9: return 0;_LL7DA: {struct Cyc_Absyn_ArrayType_struct*
_tmpFEE=(struct Cyc_Absyn_ArrayType_struct*)_tmpFE8;if(_tmpFEE->tag != 8)goto
_LL7DC;else{_tmpFEF=_tmpFEE->f1;_tmpFF0=_tmpFEF.elt_type;_tmpFF1=_tmpFEF.zero_term;}}
_LL7DB: return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpFF1) && Cyc_Tcutil_bits_only(_tmpFF0);_LL7DC: {struct Cyc_Absyn_TupleType_struct*
_tmpFF2=(struct Cyc_Absyn_TupleType_struct*)_tmpFE8;if(_tmpFF2->tag != 10)goto
_LL7DE;else{_tmpFF3=_tmpFF2->f1;}}_LL7DD: for(0;_tmpFF3 != 0;_tmpFF3=_tmpFF3->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpFF3->hd)).f2))return 0;}return 1;
_LL7DE: {struct Cyc_Absyn_AggrType_struct*_tmpFF4=(struct Cyc_Absyn_AggrType_struct*)
_tmpFE8;if(_tmpFF4->tag != 11)goto _LL7E0;else{_tmpFF5=_tmpFF4->f1;_tmpFF6=_tmpFF5.aggr_info;
if((_tmpFF6.UnknownAggr).tag != 1)goto _LL7E0;_tmpFF7=(struct _tuple4)(_tmpFF6.UnknownAggr).val;}}
_LL7DF: return 0;_LL7E0: {struct Cyc_Absyn_AggrType_struct*_tmpFF8=(struct Cyc_Absyn_AggrType_struct*)
_tmpFE8;if(_tmpFF8->tag != 11)goto _LL7E2;else{_tmpFF9=_tmpFF8->f1;_tmpFFA=_tmpFF9.aggr_info;
if((_tmpFFA.KnownAggr).tag != 2)goto _LL7E2;_tmpFFB=(struct Cyc_Absyn_Aggrdecl**)(
_tmpFFA.KnownAggr).val;_tmpFFC=*_tmpFFB;_tmpFFD=_tmpFF9.targs;}}_LL7E1: if(
_tmpFFC->impl == 0)return 0;{struct _RegionHandle _tmp1000=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp1000;_push_region(rgn);{struct Cyc_List_List*_tmp1001=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpFFC->tvs,_tmpFFD);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpFFC->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp1001,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp1002=0;_npop_handler(0);
return _tmp1002;}}}{int _tmp1003=1;_npop_handler(0);return _tmp1003;};};_pop_region(
rgn);};_LL7E2: {struct Cyc_Absyn_AnonAggrType_struct*_tmpFFE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpFE8;if(_tmpFFE->tag != 12)goto _LL7E4;else{_tmpFFF=_tmpFFE->f2;}}_LL7E3: for(0;
_tmpFFF != 0;_tmpFFF=_tmpFFF->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpFFF->hd)->type))return 0;}return 1;_LL7E4:;_LL7E5: return 0;_LL7CF:;}static int
Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp1004=e->r;struct _tuple2*_tmp100C;void*_tmp100D;struct Cyc_Absyn_Exp*
_tmp100F;struct Cyc_Absyn_Exp*_tmp1010;struct Cyc_Absyn_Exp*_tmp1011;struct Cyc_Absyn_Exp*
_tmp1013;struct Cyc_Absyn_Exp*_tmp1014;struct Cyc_Absyn_Exp*_tmp1016;struct Cyc_Absyn_Exp*
_tmp1018;void*_tmp101A;struct Cyc_Absyn_Exp*_tmp101B;enum Cyc_Absyn_Coercion
_tmp101C;void*_tmp101E;struct Cyc_Absyn_Exp*_tmp101F;struct Cyc_Absyn_Exp*_tmp1021;
struct Cyc_Absyn_Exp*_tmp1023;struct Cyc_Absyn_Exp*_tmp1024;struct Cyc_List_List*
_tmp1026;struct Cyc_List_List*_tmp1028;struct Cyc_List_List*_tmp102A;enum Cyc_Absyn_Primop
_tmp102C;struct Cyc_List_List*_tmp102D;struct Cyc_List_List*_tmp102F;struct Cyc_List_List*
_tmp1031;_LL7E7: {struct Cyc_Absyn_Const_e_struct*_tmp1005=(struct Cyc_Absyn_Const_e_struct*)
_tmp1004;if(_tmp1005->tag != 0)goto _LL7E9;}_LL7E8: goto _LL7EA;_LL7E9: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp1006=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp1004;if(_tmp1006->tag != 18)
goto _LL7EB;}_LL7EA: goto _LL7EC;_LL7EB: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp1007=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1004;if(_tmp1007->tag != 19)
goto _LL7ED;}_LL7EC: goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp1008=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp1004;if(_tmp1008->tag != 20)goto
_LL7EF;}_LL7EE: goto _LL7F0;_LL7EF: {struct Cyc_Absyn_Enum_e_struct*_tmp1009=(
struct Cyc_Absyn_Enum_e_struct*)_tmp1004;if(_tmp1009->tag != 32)goto _LL7F1;}_LL7F0:
goto _LL7F2;_LL7F1: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp100A=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp1004;if(_tmp100A->tag != 33)goto _LL7F3;}_LL7F2: return 1;_LL7F3: {struct Cyc_Absyn_Var_e_struct*
_tmp100B=(struct Cyc_Absyn_Var_e_struct*)_tmp1004;if(_tmp100B->tag != 1)goto _LL7F5;
else{_tmp100C=_tmp100B->f1;_tmp100D=(void*)_tmp100B->f2;}}_LL7F4: {void*_tmp1032=
_tmp100D;struct Cyc_Absyn_Vardecl*_tmp1035;_LL814: {struct Cyc_Absyn_Funname_b_struct*
_tmp1033=(struct Cyc_Absyn_Funname_b_struct*)_tmp1032;if(_tmp1033->tag != 2)goto
_LL816;}_LL815: return 1;_LL816: {struct Cyc_Absyn_Global_b_struct*_tmp1034=(struct
Cyc_Absyn_Global_b_struct*)_tmp1032;if(_tmp1034->tag != 1)goto _LL818;else{
_tmp1035=_tmp1034->f1;}}_LL817: {void*_tmp1037=Cyc_Tcutil_compress(_tmp1035->type);
_LL81D: {struct Cyc_Absyn_ArrayType_struct*_tmp1038=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1037;if(_tmp1038->tag != 8)goto _LL81F;}_LL81E: goto _LL820;_LL81F: {struct Cyc_Absyn_FnType_struct*
_tmp1039=(struct Cyc_Absyn_FnType_struct*)_tmp1037;if(_tmp1039->tag != 9)goto
_LL821;}_LL820: return 1;_LL821:;_LL822: return var_okay;_LL81C:;}_LL818: {struct Cyc_Absyn_Unresolved_b_struct*
_tmp1036=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp1032;if(_tmp1036->tag != 0)
goto _LL81A;}_LL819: return 0;_LL81A:;_LL81B: return var_okay;_LL813:;}_LL7F5: {
struct Cyc_Absyn_Conditional_e_struct*_tmp100E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp1004;if(_tmp100E->tag != 6)goto _LL7F7;else{_tmp100F=_tmp100E->f1;_tmp1010=
_tmp100E->f2;_tmp1011=_tmp100E->f3;}}_LL7F6: return(Cyc_Tcutil_cnst_exp(te,0,
_tmp100F) && Cyc_Tcutil_cnst_exp(te,0,_tmp1010)) && Cyc_Tcutil_cnst_exp(te,0,
_tmp1011);_LL7F7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp1012=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1004;if(_tmp1012->tag != 9)goto _LL7F9;else{_tmp1013=_tmp1012->f1;_tmp1014=
_tmp1012->f2;}}_LL7F8: return Cyc_Tcutil_cnst_exp(te,0,_tmp1013) && Cyc_Tcutil_cnst_exp(
te,0,_tmp1014);_LL7F9: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp1015=(struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp1004;if(_tmp1015->tag != 13)goto _LL7FB;
else{_tmp1016=_tmp1015->f1;}}_LL7FA: _tmp1018=_tmp1016;goto _LL7FC;_LL7FB: {struct
Cyc_Absyn_Instantiate_e_struct*_tmp1017=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1004;if(_tmp1017->tag != 14)goto _LL7FD;else{_tmp1018=_tmp1017->f1;}}_LL7FC:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1018);_LL7FD: {struct Cyc_Absyn_Cast_e_struct*
_tmp1019=(struct Cyc_Absyn_Cast_e_struct*)_tmp1004;if(_tmp1019->tag != 15)goto
_LL7FF;else{_tmp101A=(void*)_tmp1019->f1;_tmp101B=_tmp1019->f2;_tmp101C=_tmp1019->f4;
if(_tmp101C != Cyc_Absyn_No_coercion)goto _LL7FF;}}_LL7FE: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp101B);_LL7FF: {struct Cyc_Absyn_Cast_e_struct*_tmp101D=(struct Cyc_Absyn_Cast_e_struct*)
_tmp1004;if(_tmp101D->tag != 15)goto _LL801;else{_tmp101E=(void*)_tmp101D->f1;
_tmp101F=_tmp101D->f2;}}_LL800: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp101F);
_LL801: {struct Cyc_Absyn_Address_e_struct*_tmp1020=(struct Cyc_Absyn_Address_e_struct*)
_tmp1004;if(_tmp1020->tag != 16)goto _LL803;else{_tmp1021=_tmp1020->f1;}}_LL802:
return Cyc_Tcutil_cnst_exp(te,1,_tmp1021);_LL803: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1022=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp1004;if(_tmp1022->tag != 28)
goto _LL805;else{_tmp1023=_tmp1022->f2;_tmp1024=_tmp1022->f3;}}_LL804: return Cyc_Tcutil_cnst_exp(
te,0,_tmp1023) && Cyc_Tcutil_cnst_exp(te,0,_tmp1024);_LL805: {struct Cyc_Absyn_Array_e_struct*
_tmp1025=(struct Cyc_Absyn_Array_e_struct*)_tmp1004;if(_tmp1025->tag != 27)goto
_LL807;else{_tmp1026=_tmp1025->f1;}}_LL806: _tmp1028=_tmp1026;goto _LL808;_LL807: {
struct Cyc_Absyn_AnonStruct_e_struct*_tmp1027=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1004;if(_tmp1027->tag != 30)goto _LL809;else{_tmp1028=_tmp1027->f2;}}_LL808:
_tmp102A=_tmp1028;goto _LL80A;_LL809: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1029=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp1004;if(_tmp1029->tag != 29)
goto _LL80B;else{_tmp102A=_tmp1029->f3;}}_LL80A: for(0;_tmp102A != 0;_tmp102A=
_tmp102A->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple19*)_tmp102A->hd)).f2))
return 0;}return 1;_LL80B: {struct Cyc_Absyn_Primop_e_struct*_tmp102B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp1004;if(_tmp102B->tag != 3)goto _LL80D;else{_tmp102C=_tmp102B->f1;_tmp102D=
_tmp102B->f2;}}_LL80C: _tmp102F=_tmp102D;goto _LL80E;_LL80D: {struct Cyc_Absyn_Tuple_e_struct*
_tmp102E=(struct Cyc_Absyn_Tuple_e_struct*)_tmp1004;if(_tmp102E->tag != 25)goto
_LL80F;else{_tmp102F=_tmp102E->f1;}}_LL80E: _tmp1031=_tmp102F;goto _LL810;_LL80F: {
struct Cyc_Absyn_Datatype_e_struct*_tmp1030=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp1004;if(_tmp1030->tag != 31)goto _LL811;else{_tmp1031=_tmp1030->f1;}}_LL810:
for(0;_tmp1031 != 0;_tmp1031=_tmp1031->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(struct
Cyc_Absyn_Exp*)_tmp1031->hd))return 0;}return 1;_LL811:;_LL812: return 0;_LL7E6:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp103A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp103F;
struct Cyc_Absyn_PtrAtts _tmp1040;union Cyc_Absyn_Constraint*_tmp1041;union Cyc_Absyn_Constraint*
_tmp1042;struct Cyc_Absyn_ArrayInfo _tmp1044;void*_tmp1045;struct Cyc_List_List*
_tmp1047;struct Cyc_Absyn_AggrInfo _tmp1049;union Cyc_Absyn_AggrInfoU _tmp104A;
struct Cyc_List_List*_tmp104B;struct Cyc_List_List*_tmp104D;_LL824: {struct Cyc_Absyn_VoidType_struct*
_tmp103B=(struct Cyc_Absyn_VoidType_struct*)_tmp103A;if(_tmp103B->tag != 0)goto
_LL826;}_LL825: goto _LL827;_LL826: {struct Cyc_Absyn_IntType_struct*_tmp103C=(
struct Cyc_Absyn_IntType_struct*)_tmp103A;if(_tmp103C->tag != 6)goto _LL828;}_LL827:
goto _LL829;_LL828: {struct Cyc_Absyn_FloatType_struct*_tmp103D=(struct Cyc_Absyn_FloatType_struct*)
_tmp103A;if(_tmp103D->tag != 7)goto _LL82A;}_LL829: return 1;_LL82A: {struct Cyc_Absyn_PointerType_struct*
_tmp103E=(struct Cyc_Absyn_PointerType_struct*)_tmp103A;if(_tmp103E->tag != 5)goto
_LL82C;else{_tmp103F=_tmp103E->f1;_tmp1040=_tmp103F.ptr_atts;_tmp1041=_tmp1040.nullable;
_tmp1042=_tmp1040.bounds;}}_LL82B: {void*_tmp1050=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1042);_LL83B: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1051=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1050;if(_tmp1051->tag != 0)goto _LL83D;}_LL83C: return 1;_LL83D:;_LL83E: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp1041);
_LL83A:;}_LL82C: {struct Cyc_Absyn_ArrayType_struct*_tmp1043=(struct Cyc_Absyn_ArrayType_struct*)
_tmp103A;if(_tmp1043->tag != 8)goto _LL82E;else{_tmp1044=_tmp1043->f1;_tmp1045=
_tmp1044.elt_type;}}_LL82D: return Cyc_Tcutil_supports_default(_tmp1045);_LL82E: {
struct Cyc_Absyn_TupleType_struct*_tmp1046=(struct Cyc_Absyn_TupleType_struct*)
_tmp103A;if(_tmp1046->tag != 10)goto _LL830;else{_tmp1047=_tmp1046->f1;}}_LL82F:
for(0;_tmp1047 != 0;_tmp1047=_tmp1047->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple12*)_tmp1047->hd)).f2))return 0;}return 1;_LL830: {struct Cyc_Absyn_AggrType_struct*
_tmp1048=(struct Cyc_Absyn_AggrType_struct*)_tmp103A;if(_tmp1048->tag != 11)goto
_LL832;else{_tmp1049=_tmp1048->f1;_tmp104A=_tmp1049.aggr_info;_tmp104B=_tmp1049.targs;}}
_LL831: {struct Cyc_Absyn_Aggrdecl*_tmp1052=Cyc_Absyn_get_known_aggrdecl(_tmp104A);
if(_tmp1052->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1052->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmp1052->tvs,_tmp104B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1052->impl))->fields);}
_LL832: {struct Cyc_Absyn_AnonAggrType_struct*_tmp104C=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp103A;if(_tmp104C->tag != 12)goto _LL834;else{_tmp104D=_tmp104C->f2;}}_LL833:
return Cyc_Tcutil_fields_support_default(0,0,_tmp104D);_LL834: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp104E=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp103A;if(_tmp104E->tag != 14)
goto _LL836;}_LL835: goto _LL837;_LL836: {struct Cyc_Absyn_EnumType_struct*_tmp104F=(
struct Cyc_Absyn_EnumType_struct*)_tmp103A;if(_tmp104F->tag != 13)goto _LL838;}
_LL837: return 1;_LL838:;_LL839: return 0;_LL823:;}void Cyc_Tcutil_check_no_qual(
unsigned int loc,void*t){void*_tmp1053=t;struct Cyc_Absyn_Typedefdecl*_tmp1055;
_LL840: {struct Cyc_Absyn_TypedefType_struct*_tmp1054=(struct Cyc_Absyn_TypedefType_struct*)
_tmp1053;if(_tmp1054->tag != 17)goto _LL842;else{_tmp1055=_tmp1054->f3;}}_LL841:
if(_tmp1055 != 0){struct Cyc_Absyn_Tqual _tmp1056=_tmp1055->tq;if(((_tmp1056.print_const
 || _tmp1056.q_volatile) || _tmp1056.q_restrict) || _tmp1056.real_const){const
char*_tmp1656;void*_tmp1655[1];struct Cyc_String_pa_struct _tmp1654;(_tmp1654.tag=
0,((_tmp1654.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp1655[0]=& _tmp1654,Cyc_Tcutil_warn(loc,((_tmp1656="qualifier within typedef type %s is ignored",
_tag_dyneither(_tmp1656,sizeof(char),44))),_tag_dyneither(_tmp1655,sizeof(void*),
1)))))));}}goto _LL83F;_LL842:;_LL843: goto _LL83F;_LL83F:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){struct
_RegionHandle _tmp105A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp105A;
_push_region(rgn);{struct Cyc_List_List*_tmp105B=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp105B,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(!Cyc_Tcutil_supports_default(
t)){int _tmp105C=0;_npop_handler(0);return _tmp105C;}}}{int _tmp105D=1;
_npop_handler(0);return _tmp105D;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t){void*_tmp105E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1062;
struct Cyc_Absyn_PtrAtts _tmp1063;union Cyc_Absyn_Constraint*_tmp1064;union Cyc_Absyn_Constraint*
_tmp1065;_LL845: {struct Cyc_Absyn_IntType_struct*_tmp105F=(struct Cyc_Absyn_IntType_struct*)
_tmp105E;if(_tmp105F->tag != 6)goto _LL847;}_LL846: goto _LL848;_LL847: {struct Cyc_Absyn_FloatType_struct*
_tmp1060=(struct Cyc_Absyn_FloatType_struct*)_tmp105E;if(_tmp1060->tag != 7)goto
_LL849;}_LL848: return 1;_LL849: {struct Cyc_Absyn_PointerType_struct*_tmp1061=(
struct Cyc_Absyn_PointerType_struct*)_tmp105E;if(_tmp1061->tag != 5)goto _LL84B;
else{_tmp1062=_tmp1061->f1;_tmp1063=_tmp1062.ptr_atts;_tmp1064=_tmp1063.nullable;
_tmp1065=_tmp1063.bounds;}}_LL84A: {void*_tmp1066=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1065);_LL84E: {
struct Cyc_Absyn_DynEither_b_struct*_tmp1067=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp1066;if(_tmp1067->tag != 0)goto _LL850;}_LL84F: return 0;_LL850:;_LL851: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1064);
_LL84D:;}_LL84B:;_LL84C: return 0;_LL844:;}int Cyc_Tcutil_is_noreturn(void*t){{void*
_tmp1068=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp106A;void*_tmp106B;
struct Cyc_Absyn_FnInfo _tmp106D;struct Cyc_List_List*_tmp106E;_LL853: {struct Cyc_Absyn_PointerType_struct*
_tmp1069=(struct Cyc_Absyn_PointerType_struct*)_tmp1068;if(_tmp1069->tag != 5)goto
_LL855;else{_tmp106A=_tmp1069->f1;_tmp106B=_tmp106A.elt_typ;}}_LL854: return Cyc_Tcutil_is_noreturn(
_tmp106B);_LL855: {struct Cyc_Absyn_FnType_struct*_tmp106C=(struct Cyc_Absyn_FnType_struct*)
_tmp1068;if(_tmp106C->tag != 9)goto _LL857;else{_tmp106D=_tmp106C->f1;_tmp106E=
_tmp106D.attributes;}}_LL856: for(0;_tmp106E != 0;_tmp106E=_tmp106E->tl){void*
_tmp106F=(void*)_tmp106E->hd;_LL85A: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1070=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp106F;if(_tmp1070->tag != 4)
goto _LL85C;}_LL85B: return 1;_LL85C:;_LL85D: continue;_LL859:;}goto _LL852;_LL857:;
_LL858: goto _LL852;_LL852:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmp1071=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp1073;struct Cyc_List_List*_tmp1074;struct Cyc_List_List**_tmp1075;_LL85F: {
struct Cyc_Absyn_FnType_struct*_tmp1072=(struct Cyc_Absyn_FnType_struct*)_tmp1071;
if(_tmp1072->tag != 9)goto _LL861;else{_tmp1073=_tmp1072->f1;_tmp1074=_tmp1073.attributes;
_tmp1075=(struct Cyc_List_List**)&(_tmp1072->f1).attributes;}}_LL860: {struct Cyc_List_List*
_tmp1076=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1657;*_tmp1075=((_tmp1657=_cycalloc(sizeof(*_tmp1657)),((
_tmp1657->hd=(void*)atts->hd,((_tmp1657->tl=*_tmp1075,_tmp1657))))));}else{
struct Cyc_List_List*_tmp1658;_tmp1076=((_tmp1658=_cycalloc(sizeof(*_tmp1658)),((
_tmp1658->hd=(void*)atts->hd,((_tmp1658->tl=_tmp1076,_tmp1658))))));}}return
_tmp1076;}_LL861:;_LL862: {const char*_tmp165B;void*_tmp165A;(_tmp165A=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp165B="transfer_fn_type_atts",
_tag_dyneither(_tmp165B,sizeof(char),22))),_tag_dyneither(_tmp165A,sizeof(void*),
0)));}_LL85E:;}
